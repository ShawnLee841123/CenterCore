
#ifndef __PORT_COMPLETE_WORKER_H__
#define __PORT_COMPLETE_WORKER_H__

#include "../../PublicLib/Include/Common/LibThreadBase.h"
#include "PortCompleteQueueElementDataDefine.h"

#ifdef _WIN_
typedef struct _PER_IO_CONTEXT OPERATE_IO_CONTEXT;
typedef struct _PER_SOCKET_CONTEXT OPERATE_SOCKET_CONTEXT;
struct WSAOVERLAPPED;
typedef struct _OVERLAPPED OVERLAPPED;
class SocketRegisterData;
#endif

struct WorkerStoreInfo;

class PortCompleteWorker : public ThreadBase
{
public:
	PortCompleteWorker();
	~PortCompleteWorker();

	virtual bool SetCompletionPort(void* pPort);
	virtual bool OnThreadInitialize(int nTickTime) override;
	virtual bool OnThreadRunning() override;
	virtual bool OnThreadDestroy() override;
	virtual bool WorkFunctionEnable(PortCompletionThreadFunctionMask eMask, bool bEnable);
protected:

	virtual bool CheckFunctionEnable(PortCompletionThreadFunctionMask eMask);
#ifdef _WIN_
	virtual bool RegisterConnectSocket(OPERATE_SOCKET_CONTEXT* pSocketContext);
#pragma region Loop about
	//	完成端口的精华就在这几个方法里（Loop about 全部），总共9个，弄清楚了，完成端口就算是会用了。中间的过程，需要自己碰撞才会知道
	bool OnWorkerMainLoop(int nElapse);
	
	bool StartListenConnect();
	bool InitialListenSocket();

	bool DoAccept(OPERATE_SOCKET_CONTEXT* pSockContext, OPERATE_IO_CONTEXT* pIoContext);
	bool DoRecv(OPERATE_SOCKET_CONTEXT* pSockContext, OPERATE_IO_CONTEXT* pIoContext);
	bool PostAccept(OPERATE_SOCKET_CONTEXT* pSockContext, OPERATE_IO_CONTEXT* pIoContext);
	bool PostRecv(OPERATE_SOCKET_CONTEXT* pSockContext, OPERATE_IO_CONTEXT* pIoContext);

	virtual bool OnQueueElement(UnLockQueueElementBase* pElement) override;
	virtual bool OnSocketRegisterData(SocketRegisterData* pData);
#pragma endregion

#endif
#pragma region variable
	void*								m_pCompletionPort;
	UI32								m_uThreadFunc;					//	线程功能标记
#ifdef _WIN_
	void*								m_pFnAcceptEx;					//	函数指针
	void*								m_pFnGetAcceptExSockAddrs;		//	函数指针
	OPERATE_SOCKET_CONTEXT*				m_pListenContext;				//	监听结构
	UI32								m_dwBytesTransfered;

	OPERATE_SOCKET_CONTEXT*				m_pLoopSockContext;
	OVERLAPPED*							m_pLoopOverlapped;
#endif

	std::map<UI32, OPERATE_SOCKET_CONTEXT*>	m_pStoreInfo;
#pragma endregion

};

#endif
