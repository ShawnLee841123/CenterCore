﻿
#ifndef __PORT_COMPLETE_CORE_H__
#define __PORT_COMPLETE_CORE_H__

#define PCWORK_TCOUNT 2		//	完成端口线程数量


#include "../CenterCoreBasic/CenterCoreBasic.h"
#include "../../PublicLib/Include/Common/TypeDefines.h"
#include "PortCompleteQueueElementDataDefine.h"
#include <map>


/*

消息收发的一个轮回
发送
	1、SocketInfo Status设置为Send
	2、将具体的消息内容和socket填入队列
	3、完成端口的工作线程完成发送
	4、将发送结果反馈给主线程
	5、主线程将socketInfo 的Status设置为wait

接收
	1、工作线程接收到消息，通知主线程现在为recv
	2、收消息完毕后，将内容填充到主线程工作队列
	3、主线程处理消息内容后
	4、将SocketInfo的Status设置为wait
	5、通知工作线程丢弃数据内容,重新接收

只有主线程的SocketInfo为wait的状态下才能进行收或者是发。
*/

#pragma region declear forward extern
#ifdef _WIN_
typedef struct _PER_IO_CONTEXT OPERATE_IO_CONTEXT;
typedef struct _PER_SOCKET_CONTEXT OPERATE_SOCKET_CONTEXT;
#endif
class PortCompleteWorker;
class UnLockQueueDataElementBase;
class UnLockQueueBase;

#pragma endregion

class PortCompleteSocketInfo
{
public:
	PortCompleteSocketInfo();
	virtual ~PortCompleteSocketInfo();

	SI64*	pSocket;
	int		nCurStatus;
	char	strSocketInfo[1024];
	char	strSendMsg[LISTEN_LINK_COUNT];
	char	strRecvMsg[LISTEN_LINK_COUNT];
	UI32	uSendCount;
	UI32	uRecvCount;
	SI64	nLinkID;
};

//	所有socket链接都在listener里创建

class PortCompleteCore : public CenterCoreBasic
{
public:
	PortCompleteCore();
	virtual ~PortCompleteCore();

protected:

#pragma region Parent function override
	virtual bool OnInitialize() override;
	virtual bool OnStart() override;
	virtual bool OnTick(int nElapse) override;
	virtual bool OnDestroy() override;
#pragma endregion
	
#pragma region Socket data operate
	//	功能不全，暂时简单实用存储数量来做。后期会修改

	virtual SI64 MakeStoreID();
#ifdef _WIN_
	virtual bool AddSocketContext(SI64 nStoreID, OPERATE_SOCKET_CONTEXT* pContext);
	virtual bool RemoveSocketContext(SI64 nStoreID);
	virtual bool ClearAllSockContext();
	virtual OPERATE_SOCKET_CONTEXT* GetSockContext(SI64 nStoreID);
#endif
#pragma region Useless
	//	添加链接
	virtual bool AddNewLink(SI64 nLinkID, PortCompleteSocketInfo* pInfo);
	//	移除链接
	virtual bool RemoveLink(SI64 nLinkID);
	//	获取链接
	virtual PortCompleteSocketInfo* GetLink(SI64 nLinkID);
	//	发送消息
	virtual bool LinkSendMsg(SI64 nLinkID, const char* strMsg);
	//	接收消息
	virtual bool LinkRecvMsg(SI64 nLinkID, const char* strMsg);
#pragma endregion

	//	清空链接
	virtual bool ClearAllLink();
#pragma endregion

#pragma region Unlock queue about
	//	加入新的工作线程
	//virtual bool AddNewWorkThread
#pragma endregion

#pragma region Queue process
	virtual bool OnReadQueueTick(int nElapse);
	virtual bool OnQueueElementProcessEnter(UnLockQueueDataElementBase* pElement);
#ifdef _WIN_
	virtual bool OnSocketRegister(UnLockQueueDataElementBase* pElement);
	virtual bool AddSocket2MsgThread(OPERATE_SOCKET_CONTEXT* pSockContext, SI64 nThreadID);
#endif
	virtual bool OnSocketMessage(UnLockQueueDataElementBase* pElement);

#pragma endregion

#pragma region variable
	std::map<SI64, PortCompleteSocketInfo*>	m_dicLinkPool;		//	暂时不确定到底使用哪个
#ifdef _WIN_
	std::map<SI64, OPERATE_SOCKET_CONTEXT*>		m_dicSocktPool;		//	
#endif
	PortCompleteWorker*			m_arrWorkThread[PCWORK_TCOUNT];
	void*						m_pCompletionPort;

	std::map<SI64, SI32>						m_dicWorkerSocketCount;	//	线程上的Socket数量，计数使用，方便后续线程间的负载均衡
	
	//	读取队列
	std::map<SI32, UnLockQueueBase*>			m_dicCoreReadQueue;
	//	写入队列
	std::map<SI32, UnLockQueueBase*>			m_dicCoreWriteQueue;

#pragma endregion
};


#endif	//	__PORT_COMPLETE_CORE_H__
