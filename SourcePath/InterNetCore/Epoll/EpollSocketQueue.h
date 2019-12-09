
/******************************************************************************************
 *
 *	Epoll连接操作队列
 *	ShawnLee
 *	2019-12-6
 *	
 *	Desc
 *		每个操作队列只能做一个操作（读、写），操作完成后，将结果返回给
 *		具体Worker，由Worker再将读写结果通知上层，并将其从队列中移除
 *	
 *	PS:	内存不是在这里进行管理，这里不负责释放IO操作指向的内存	
 *
 *******************************************************************************************/

#ifndef __EPOLL_SOCKET_QUEUE_H__
#define __EPOLL_SOCKET_QUEUE_H__

#include "EpollQueueElementDataDefine.h"
#include <vector>
#include <map>

class _PER_IO_CONTEXT;
class _PER_SOCKET_CONTEXT;

//	操作队列
class EpollSocketQueue
{
public:
	EpollSocketQueue();
	virtual ~EpollSocketQueue();

#pragma region Socket Functional operate
	//	设置本队列功能标记，设计目的只支持单一操作，要么读、要么写(或监听连接)。
	virtual bool SetQueueFuncMask(SI16 nMask);
	//	检查本队列是否具有功能标记
	virtual bool CheckQueueCanDo(EpollOperateType eMask);
#pragma endregion

	//	添加IO操作，将一个IO操作扔到队列中来，如果有操作，会将此操作返回滴
	virtual bool AddIOOperate(_PER_IO_CONTEXT* pContext);
	//	队列工作，目前还没想好返回值用什么，只是觉得应该返回错误类型等等这些信息
	virtual int QueueWork(_PER_SOCKET_CONTEXT* pSocketContext);

protected:

#pragma region Socket queue operate process
	//	客户端连接操作
	virtual int DoAccept(_PER_SOCKET_CONTEXT* pSocketContext, _PER_IO_CONTEXT* pIoContext);
	//	接收消息操作
	virtual int DoReciveMsg(_PER_SOCKET_CONTEXT* pSocketContext, _PER_IO_CONTEXT* pIoContext);
	//	发送消息操作
	virtual int DoSendMsg(_PER_SOCKET_CONTEXT* pSocketContext, _PER_IO_CONTEXT* pIoContext);
#pragma endregion

#pragma region Variable
	_PER_IO_CONTEXT*				m_arrQueue[_PER_IO_CONTEXT];
	SI16						m_QueueFuncMask;	//	队列操作标记
#pragma endregion
};


#endif	//	__EPOLL_SOCKET_QUEUE_H__

