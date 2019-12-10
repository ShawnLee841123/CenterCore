
#ifndef __EPOLL_SOCKET_QUEUE_ELEMENT_H__
#define __EPOLL_SOCKET_QUEUE_ELEMENT_H__

#include "EpollQueueElementDataDefine.h"
#include <vector>
#pragma region IO Operation
class _PER_IO_CONTEXT
{
public:
	_PER_IO_CONTEXT();
	virtual ~_PER_IO_CONTEXT();
	_PER_IO_CONTEXT& operator=(const _PER_IO_CONTEXT rhv);
	void ResetDataBuff();

	char databuf[IO_BUFFER_SIZE];		//	字符缓冲区
	SI32 datalength;			//	字符缓冲区长度
	EpollOperateType operateType;		//	操作类型
	CORE_SOCKET link;			//	本操作使用的socket
};
#define OPERATE_IO_CONTEXT _PER_IO_CONTEXT
#define LPOPERATE_IO_CONTEXT _PER_IO_CONTEXT*

#pragma endregion


#pragma region Socket Operation
class _PER_SOCKET_CONTEXT
{
public:
	_PER_SOCKET_CONTEXT();
	virtual ~_PER_SOCKET_CONTEXT();
	_PER_SOCKET_CONTEXT& operator=(const _PER_SOCKET_CONTEXT rhv);
	void ClearOperate();
	LPOPERATE_IO_CONTEXT GetNewIoOperate();
	void RemoveIoOperate(LPOPERATE_IO_CONTEXT pOperate);

	CORE_SOCKET				link;			//	客户端链接
	CORE_SOCKETADDR_IN*			clientAddr;		//	客户端地址
	SI32					RecvThreadID;		//	接收消息线程ID
	SI32					SendThreadID;		//	发送消息线程ID
	std::vector<LPOPERATE_IO_CONTEXT>	vIoContext;		//	IO操作队列
	SI64					storeID;
};

#define OPERATE_SOCKET_CONTEXT _PER_SOCKET_CONTEXT
#define LPOPERATE_SOCKET_CONTEXT _PER_SOCKET_CONTEXT*
#pragma endregion

struct WorkerStoreInfo
{
	CORE_SOCKET		link;
	CORE_SOCKETADDR_IN*	addr;
	UI64			UUID;
};


#endif	//	__EPOLL_SOCKET_QUEUE_ELEMENT_H__

