
#include "EpollSocketQueue.h"
#include "EpollSocketQueueElement.h"
#include "../../PublicLib/Include/Common/UnlockElementTypeDefine.h"

EpollSocketQueue::EpollSocketQueue(): m_QueueFuncMask(0)
{
	m_vQueue.clear();
}

EpollSocketQueue::~EpollSocketQueue()
{
	m_QueueFuncMask = 0;
	m_vQueue.clear();
}

#pragma region 
bool EpollSocketQueue::SetQueueFuncMask(SI16 nMask)
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
	SI16 nFlag = 1;
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

	m_vQueue.push_back(pContext);
	return true;
}

//	队列工作
int EpollSocketQueue::QueueWork(_PER_SOCKET_CONTEXT* pSocketContext)
{
	while (m_vQueue.size() > 0)
	{
		std::vector<_PER_IO_CONTEXT*>::iterator iter = m_vQueue.begin();
		for(; iter != m_vQueue.end();)
		{
			//	TODO: 监听
			if (CheckQueueCanDo(ECPOT_ACCEPT))
			{
				int nRet = DoAccept(pSocketContext, *iter);
			}
			
			//	TODO: 收消息
			if (CheckQueueCanDo(ECPOT_RECIVE))
			{
				int nRet = DoReciveMsg(pSocketContext, *iter);
			}

			//	TODO: 发消息
			if (CheckQueueCanDo(ECPOT_SEND))
			{
				int nRet = DoSendMsg(pSocketContext, *iter);
			}
			
			iter++;
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

