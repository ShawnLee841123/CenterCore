
#include "EpollSocketQueue.h"
#include "EpollSocketQueueElement.h"
#include "../../PublicLib/Include/Common/UnLockElementTypeDefine.h"

EpollSocketQueue::EpollSocketQueue(): m_QueueFuncMask(0), m_nCurElementCount(0)
{
	for (int i = 0; i < SOCKET_QUERY_ELEMENT_MAX; i++)
	{
		m_arrQueue[i] = nullptr;
	}
}

EpollSocketQueue::~EpollSocketQueue()
{
	m_QueueFuncMask = 0;
	for(int i = 0; i < SOCKET_QUERY_ELEMENT_MAX; i++)
	{
		if (nullptr == m_arrQueue[i])
		{
			delete m_arrQueue[i];
			m_arrQueue[i] = nullptr;
		}
	}
	m_nCurElementCount = 0;
}

#pragma region 
bool EpollSocketQueue::SetQueueFuncMask(UI16 nMask)
{
	m_QueueFuncMask |= nMask;

	//	do not allow one query doing the other things
	if (CheckQueueCanDo(ECPOT_ACCEPT) && CheckQueueCanDo(ECPOT_RECIVE))
		m_QueueFuncMask = 0;

	if (CheckQueueCanDo(ECPOT_ACCEPT) && CheckQueueCanDo(ECPOT_SEND))
		m_QueueFuncMask = 0;

	if (CheckQueueCanDo(ECPOT_RECIVE) && CheckQueueCanDo(ECPOT_SEND))
		m_QueueFuncMask = 0;

	return 0 == m_QueueFuncMask;
}

bool EpollSocketQueue:: CheckQueueCanDo(EpollOperateType eMask)
{
	UI16 nFlag = 1;
	nFlag &= (m_QueueFuncMask >> eMask);

	return nFlag > 0;
}

#pragma endregion

bool EpollSocketQueue::AddIOOperate(_PER_IO_CONTEXT* pContext)
{
	if (nullptr == pContext)
		return false;

	if (!CheckQueueCanDo(pContext->operateType))
		return false;

	if (m_nCurElementCount >= SOCKET_QUERY_ELEMENT_MAX)
		return false;

	m_arrQueue[m_nCurElementCount] = pContext;
	m_nCurElementCount++;
	return true;
}

bool EpollSocketQueue::RemoveIOOperate(int nIndex)
{
	if (nIndex < 0)
		return false;

	if (nIndex >= m_nCurElementCount)
		return false;

	//	将队尾元素拿到这里来
	m_arrQueue[nIndex] = m_arrQueue[m_nCurElementCount];
	m_arrQueue[m_nCurElementCount] = nullptr;
	m_nCurElementCount--;

	return true;
}

//	队列工作
int EpollSocketQueue::QueueWork(_PER_SOCKET_CONTEXT* pSocketContext)
{
	while (m_nCurElementCount > 0)
	{
		for(int i = 0; i < m_nCurElementCount;)
		{
			//	TODO: 监听
			if (CheckQueueCanDo(ECPOT_ACCEPT))
			{
				int nRet = DoAccept(pSocketContext, m_arrQueue[i]);
				if (nRet > 0)
				{
					RemoveIOOperate(i);
					continue;
				}
			}
			
			//	TODO: 收消息
			if (CheckQueueCanDo(ECPOT_RECIVE))
			{
				int nRet = DoReciveMsg(pSocketContext, m_arrQueue[i]);
				if (nRet > 0)
				{
					RemoveIOOperate(i);
					continue;
				}
			}

			//	TODO: 发消息
			if (CheckQueueCanDo(ECPOT_SEND))
			{
				int nRet = DoSendMsg(pSocketContext, m_arrQueue[i]);
				if (nRet > 0)
				{
					RemoveIOOperate(i);
					continue;
				}
			}
			
			i++;
		}
	}

	return 0;
}

#pragma region Socket queue operate process
//	客户端连接操作
int EpollSocketQueue::DoAccept(_PER_SOCKET_CONTEXT* pSocketContext, _PER_IO_CONTEXT* pIoContext)
{
	//	TODO: 监听实现
	return 0;
}

//	接收消息操作
int EpollSocketQueue::DoReciveMsg(_PER_SOCKET_CONTEXT* pSocketContext, _PER_IO_CONTEXT* pIoContext)
{
	//	TODO: 接收消息实现
	return 0;
}

//	发送消息操作
int EpollSocketQueue::DoSendMsg(_PER_SOCKET_CONTEXT* pSocketContext, _PER_IO_CONTEXT* pIoContext)
{
	//	TODO: 发送消息实现
	return 0;
}
#pragma endregion

