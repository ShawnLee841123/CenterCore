﻿#include "PortCompleteCore.h"
#include "PortCompleteWorker.h"
#include "PortCompleteBaseDefine.h"
#include "PortCompleteQueueElement.h"
#include "../../PublicLib/Include/Common/UnLockQueue.h"
#include "../../PublicLib/Include/Common/UnLockElementTypeDefine.h"

#ifdef _WIN_
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <mswsock.h>

#pragma warning(disable:4996)
#endif

#pragma region PortCompleteSocketInfo

PortCompleteSocketInfo::PortCompleteSocketInfo():pSocket(nullptr), uRecvCount(0), uSendCount(0), nLinkID(0), nCurStatus(0)
{
	memset(strSocketInfo, 0, sizeof(char) * 1024);
	memset(strRecvMsg, 0, sizeof(char) * LISTEN_LINK_COUNT);
	memset(strSendMsg, 0, sizeof(char) * LISTEN_LINK_COUNT);
}

PortCompleteSocketInfo::~PortCompleteSocketInfo()
{}

#pragma endregion


PortCompleteCore::PortCompleteCore() : m_pCompletionPort(nullptr)
{
	for (int i = 0; i < PCWORK_TCOUNT; i++)
	{
		m_arrWorkThread[i] = nullptr;
	}
}

PortCompleteCore::~PortCompleteCore()
{}

#pragma region Parent function override
bool PortCompleteCore::OnInitialize()
{
	bool bRet = true;
	bRet &= CenterCoreBasic::OnInitialize();

#ifdef _WIN_
	//	初始化Sockect
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	m_pCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (nullptr == m_pCompletionPort)
	{
		CORE_ERROR("Completion port Create failed");
		return false;
	}


#endif

#pragma region Initialize worker thread
	for (int i = 0; i < PCWORK_TCOUNT; i++)
	{
		m_arrWorkThread[i] = new PortCompleteWorker();
		bRet &= m_arrWorkThread[i]->SetCompletionPort(m_pCompletionPort);
		
		//	打开工作线程的各个功能
		if (i == 0)
			m_arrWorkThread[i]->WorkFunctionEnable(EPCTFT_LISTEN, true);

		m_arrWorkThread[i]->WorkFunctionEnable(EPCTFT_RECV, true);
		m_arrWorkThread[i]->WorkFunctionEnable(EPCTFT_SEND, true);
	
		bRet &= m_arrWorkThread[i]->OnThreadInitialize(0);
	}
#pragma endregion

	return bRet;
}

bool PortCompleteCore::OnStart()
{
	bool bRet = true;
	bRet &= CenterCoreBasic::OnStart();

	for (int i = 0; i < PCWORK_TCOUNT; i++)
	{
		//	Register unlockqueue
		UnLockQueueBase* pCoreReadQueue = new UnLockQueueBase();
		UnLockQueueBase* pCoreWriteQueue = new UnLockQueueBase();
		char strReadQueueName[64] = { 0 };
		char strWriteQueueName[64] = { 0 };
		SI32 nThreadID = i + 1;

		sprintf(strReadQueueName, "CoreReadQueue[%d]", nThreadID);
		sprintf(strWriteQueueName, "CoreWriteQueue[%d]", nThreadID);
		m_arrWorkThread[i]->SetThreadID(nThreadID);
		m_arrWorkThread[i]->RegisterQueue(pCoreReadQueue, strReadQueueName, ESQT_WRITE_QUEUE);
		m_arrWorkThread[i]->RegisterQueue(pCoreWriteQueue, strWriteQueueName, ESQT_READ_QUEUE);
		m_dicCoreReadQueue.insert(std::pair<SI32, UnLockQueueBase*>(nThreadID, pCoreReadQueue));
		m_dicCoreWriteQueue.insert(std::pair<SI32, UnLockQueueBase*>(nThreadID, pCoreWriteQueue));

		//	初始化当前负载数量
		m_dicWorkerSocketCount.insert(std::pair<SI32, SI32>(nThreadID, 0));

		//	Register LogQueue
		char strLogQueueName[64] = { 0 };
		sprintf(strLogQueueName, "ThreadLogQueue[%d]", nThreadID);
		ThreadRegisterLog(m_arrWorkThread[i], strLogQueueName);

		//	start thread
		bRet &= m_arrWorkThread[i]->OnThreadStart(nThreadID);
	}

	return bRet;
}

bool PortCompleteCore::OnTick(int nElapse)
{
	bool bRet = true;
	bRet &= CenterCoreBasic::OnTick(nElapse);

	bRet &= OnReadQueueTick(nElapse);

	return bRet;
}

bool PortCompleteCore::OnDestroy()
{
	bool bRet = true;
	bRet &= CenterCoreBasic::OnDestroy();

	for (int i = 0; i < PCWORK_TCOUNT; i++)
	{
		if (nullptr != m_arrWorkThread[i])
		{
			m_arrWorkThread[i]->OnThreadDestroy();
			delete m_arrWorkThread[i];
		}
		
		m_arrWorkThread[i] = nullptr;
	}

	bRet &= ClearAllLink();

	m_dicWorkerSocketCount.clear();
#ifdef _WIN_
	bRet &= ClearAllSockContext();
	if (nullptr != m_pCompletionPort && INVALID_HANDLE_VALUE != m_pCompletionPort)
	{
		CloseHandle(m_pCompletionPort);
		m_pCompletionPort = nullptr;
	}

#endif
	WSACleanup();
	return bRet;
}
#pragma endregion

#pragma region Socket data operate
#ifdef _WIN_
SI64 PortCompleteCore::MakeStoreID()
{
	std::map<SI64, SI32>::iterator iter = m_dicWorkerSocketCount.begin();
	SI64 nMinThreadID = iter->first;
	SI32 nMinCount = iter->second;

	//	找出负载最小的线程ID
	for (; iter != m_dicWorkerSocketCount.end(); ++iter)
	{
		if (iter->second < nMinCount)
			nMinThreadID = iter->first;
	}

	//return m_dicSocktPool.size() + 1;
	return nMinThreadID;
}

bool PortCompleteCore::AddSocketContext(SI64 nStoreID, OPERATE_SOCKET_CONTEXT* pContext)
{
	std::map<SI64, OPERATE_SOCKET_CONTEXT*>::iterator iter = m_dicSocktPool.find(nStoreID);
	SI32 nThreadCurSocketCount = m_dicWorkerSocketCount[nStoreID];

	if (iter == m_dicSocktPool.end())
	{
		OPERATE_SOCKET_CONTEXT* pNewContext = new OPERATE_SOCKET_CONTEXT();
		*pNewContext = *pContext;
		pNewContext->storeID = nStoreID;
		m_dicSocktPool.insert(std::pair<SI64, OPERATE_SOCKET_CONTEXT*>(nStoreID, pNewContext));
		m_dicWorkerSocketCount[nStoreID] = nThreadCurSocketCount + 1;
		return true;
	}

	CORE_WARNING("[PortCompleteCore::AddSocketContext] Add socket context same storeid[%llu] ", nStoreID);
	return false;
}
bool PortCompleteCore::RemoveSocketContext(SI64 nStoreID)
{
	if (m_dicSocktPool.size() < 1)
	{
		CORE_WARNING("[PortCompleteCore::RemoveSocketContext]Port completion socket pool is empty");
		return true;
	}

	std::map<SI64, OPERATE_SOCKET_CONTEXT*>::iterator iter = m_dicSocktPool.find(nStoreID);
	if (iter != m_dicSocktPool.end())
	{
		CORE_MSG("[PortCompleteCore::RemoveSocketContext] Remove SockContext[%llu]", nStoreID);
		m_dicSocktPool.erase(iter);
		return true;
	}

	CORE_MSG("[PortCompleteCore::RemoveSocketContext] Remove can not find remove SockContext[%llu]", nStoreID);
	return true;
}

bool PortCompleteCore::ClearAllSockContext()
{
	if (m_dicSocktPool.size() > 0)
		return true;

	std::map<SI64, OPERATE_SOCKET_CONTEXT*>::iterator iter = m_dicSocktPool.begin();
	for (; iter != m_dicSocktPool.end(); ++iter)
	{
		if (nullptr != iter->second)
			delete iter->second;

		iter->second = nullptr;
	}

	CORE_WARNING("[PortCompleteCore::ClearAllSockContext]Clear port completion socket pool");
	m_dicSocktPool.clear();

	return true;
}

OPERATE_SOCKET_CONTEXT* PortCompleteCore::GetSockContext(SI64 nStoreID)
{
	std::map<SI64, OPERATE_SOCKET_CONTEXT*>::iterator iter = m_dicSocktPool.find(nStoreID);
	if (iter != m_dicSocktPool.end())
		return iter->second;

	CORE_MSG("[PortCompleteCore::GetSockContext] Can not find SockContext[%llu]", nStoreID);
	return nullptr;
}

#endif

#pragma region Useless
//	添加链接
bool PortCompleteCore::AddNewLink(SI64 nLinkID, PortCompleteSocketInfo* pInfo)
{
	if (nullptr == pInfo)
		return false;

	std::map<SI64, PortCompleteSocketInfo*>::iterator iter = m_dicLinkPool.find(nLinkID);
	if (iter != m_dicLinkPool.end())
	{
		CORE_WARNING("Repead socket");
		return false;
	}

	m_dicLinkPool.insert(std::pair<SI64, PortCompleteSocketInfo*>(nLinkID, pInfo));
	return true;
}
//	移除链接
bool PortCompleteCore::RemoveLink(SI64 nLinkID)
{
	std::map<SI64, PortCompleteSocketInfo*>::iterator iter = m_dicLinkPool.find(nLinkID);
	if (iter != m_dicLinkPool.end())
	{
		if ((SOCKET)(iter->second->pSocket) != INVALID_SOCKET)
			closesocket((SOCKET)(iter->second->pSocket));

		delete iter->second;
		iter->second = nullptr;
		m_dicLinkPool.erase(iter);
		CORE_MSG("Remove socket");
		return true;
	}

	return false;
}

//	获取链接
PortCompleteSocketInfo* PortCompleteCore::GetLink(SI64 nLinkID)
{
	std::map<SI64, PortCompleteSocketInfo*>::iterator iter = m_dicLinkPool.find(nLinkID);
	if (iter != m_dicLinkPool.end())
	{
		return iter->second;
	}

	return nullptr;
}
//	发送消息
bool PortCompleteCore::LinkSendMsg(SI64 nLinkID, const char* strMsg)
{
	return true;
}

//	接收消息
bool PortCompleteCore::LinkRecvMsg(SI64 nLinkID, const char* strMsg)
{
	return true;
}
#pragma endregion

//	清空链接
bool PortCompleteCore::ClearAllLink()
{
	std::map<SI64, PortCompleteSocketInfo*>::iterator iter = m_dicLinkPool.begin();
	for (; iter != m_dicLinkPool.end(); ++iter)
	{
		if (nullptr != iter->second)
		{
			if ((SOCKET)(iter->second->pSocket) != INVALID_SOCKET)
				closesocket((SOCKET)(iter->second->pSocket));

			delete iter->second;
			iter->second = nullptr;
		}
	}

	m_dicLinkPool.clear();
	return true;
}
#pragma endregion

#pragma region Queue process
bool PortCompleteCore::OnReadQueueTick(int nElapse)
{
	if (m_dicCoreReadQueue.size() < 1)
		return true;

	std::map<SI32, UnLockQueueBase*>::iterator iter = m_dicCoreReadQueue.begin();
	for (; iter != m_dicCoreReadQueue.end(); ++iter)
	{
		if (nullptr == iter->second)
		{
			CORE_ERROR("PortCompleteCore read queue[%d] is null", iter->first);
			return false;
		}

		EQueueOperateResultType eRet = EQORT_SUCCESS;
		do 
		{
			UnLockQueueDataElementBase* pDataElement = (UnLockQueueDataElementBase*)iter->second->PopQueueElement(eRet);
			if (nullptr == pDataElement)
			{
				eRet = EQORT_POP_INVALID_ELEMENT;
				continue;
			}

			if (!OnQueueElementProcessEnter(pDataElement))
				eRet = EQORT_POP_INVALID_ELEMENT;

			pDataElement->ClearElement();
			//	暂时不确定这里是否会有内存泄漏

		} while (eRet == EQORT_SUCCESS);
	}

	return true;
}

bool PortCompleteCore::OnQueueElementProcessEnter(UnLockQueueDataElementBase* pElement)
{
	UI32 uDataType = pElement->GetDataID();
	switch (uDataType)
	{
	case EESDGT_REGISTER:
		{
			return OnSocketRegister(pElement);
		}
		break;
	//case EESDGT_MESSAGE:
	//	{
	//		return OnSocketMessage(pElement);
	//	}
	//	break;
	}

	//	没有找到对应的元素处理函数
	return false;
}

#ifdef _WIN_
bool PortCompleteCore::OnSocketRegister(UnLockQueueDataElementBase* pElement)
{
	//	这里将连上的socket保存起来，后面全部都是使用socket的存储ID来收发消息。
	if (!pElement->Enable())
		return false;

	SocketRegisterData* pData = (SocketRegisterData*)pElement->GetData();
	if (nullptr == pData)
		return false;

	if (nullptr == pData->pSocketContext)
		return false;

	//PortCompleteSocketInfo* pInfo = new PortCompleteSocketInfo();
	//pInfo->pSocket = pData->pSocketContext->link;
	//memcpy(pInfo->strSocketInfo, pData->strSocketInfo, sizeof(char)*IO_BUFFER_SIZE);

	//pInfo->nLinkID = *pInfo->pSocket;
	//pInfo->nCurStatus = ECPOT_SEND;
	if (pData->eRegisterType != EPCSRT_STORE)
	{
		CORE_WARNING("Wrong Type");
		return false;
	}


	OPERATE_SOCKET_CONTEXT* pSockContext = pData->pSocketContext;
	if (pSockContext->storeID > 0)
	{
		CORE_ERROR("new Socket have already store information");
		return false;
	}

	bool bRet = true;
	
	SI64 nID = MakeStoreID();
	pSockContext->storeID = nID;
	pSockContext->RecvThreadID = nID;

	bRet &= AddSocket2MsgThread(pSockContext, nID);
	bRet &= AddSocketContext(nID, pSockContext);

	return bRet;
}

bool PortCompleteCore::AddSocket2MsgThread(OPERATE_SOCKET_CONTEXT* pSockContext, SI64 nThreadID)
{
	if (nullptr == pSockContext)
		return false;

	if (nThreadID < 0)
		return false;

	EQueueOperateResultType eType = EQORT_SUCCESS;
	SocketRegisterData* pData = new SocketRegisterData();
	pData->eRegisterType = EPCSRT_RECV;
	pData->pSocketContext = pSockContext;
	eType = m_dicCoreWriteQueue[nThreadID]->PushQueueElement(pData, sizeof(SocketRegisterData));
	return EQORT_SUCCESS == eType;
}
#endif
bool PortCompleteCore::OnSocketMessage(UnLockQueueDataElementBase* pElement)
{
	//	根据当前socket的状态来做具体的受发操作，只有当前socket的状态为等待时，才会进行收获着发的操作。
	return true;
}
#pragma endregion