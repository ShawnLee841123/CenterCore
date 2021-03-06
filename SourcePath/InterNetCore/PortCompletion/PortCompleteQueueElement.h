﻿
#ifndef __PORT_COMPLETE_QUEUE_ELEMENT_H__
#define __PORT_COMPLETE_QUEUE_ELEMENT_H__


#include "../../CoreInterface/ICompletionPort.h"

#include "PortCompleteQueueElementDataDefine.h"
#include "../../PublicLib/Include/Common/UnLockQueue.h"
#include "../../PublicLib/Include/Common/TypeDefines.h"


#define MSG_BUFFER_COUNT	2048

#ifdef _WIN_

typedef struct _PER_IO_CONTEXT OPERATE_IO_CONTEXT;
typedef struct _PER_SOCKET_CONTEXT OPERATE_SOCKET_CONTEXT;

class SocketMessageData : public UnLockQueueElementDataBase
{
public:
	SocketMessageData();
	virtual ~SocketMessageData();

	SocketMessageData& operator=(const UnLockQueueElementDataBase rhv) override;
	SocketMessageData& operator=(const SocketMessageData rhv);

	CORE_SOCKET				pSocket;
	char					strMesBuffer[IO_BUFFER_SIZE];
	UI32					uBufferSize;
	SI64					nStoreID;
};

class SocketRegisterData : public UnLockQueueElementDataBase
{
public:
	SocketRegisterData();
	virtual ~SocketRegisterData();

	SocketRegisterData& operator=(const UnLockQueueElementDataBase rhv) override;
	SocketRegisterData& operator=(const SocketRegisterData rhv);

	PortCompletionSocketRegisterType	eRegisterType;
	OPERATE_SOCKET_CONTEXT*				pSocketContext;
	SI32								nThreadID;
};
#endif	//	_WIN_

#endif	//	__PORT_COMPLETE_QUEUE_ELEMENT_H__


