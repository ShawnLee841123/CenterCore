#include "EpollSocketQueueElement.h"
#include "EpollQueueElementDataDefine.h"
#include <string.h>

#pragma region IO Operation

_PER_IO_CONTEXT::_PER_IO_CONTEXT():link(INVALID_SOCKET), operateType(ECPOT_NONE), datalength(0)
{
	ResetDataBuff();
}

_PER_IO_CONTEXT::~_PER_IO_CONTEXT()
{
	ResetDataBuff();
}

_PER_IO_CONTEXT& _PER_IO_CONTEXT::operator=(const _PER_IO_CONTEXT rhv)
{
	datalength = rhv.datalength;
	link = rhv.link;
	operateType = rhv.operateType;
	memcpy(databuf, rhv.databuf, sizeof(char) * IO_BUFFER_SIZE);
	return *this;
}

void _PER_IO_CONTEXT::ResetDataBuff()
{
	memset(databuf, 0, sizeof(char) * IO_BUFFER_SIZE);
	datalength = 0;
}


#pragma endregion


#pragma region Socket Operation

_PER_SOCKET_CONTEXT::_PER_SOCKET_CONTEXT(): link(INVALID_SOCKET), storeID(0)
{
	vIoContext.clear();
	memset(clientAddr, 0, sizeof(CORE_SOCKETADDR_IN));
}

_PER_SOCKET_CONTEXT::~_PER_SOCKET_CONTEXT()
{
	ClearOperate();
}

_PER_SOCKET_CONTEXT& _PER_SOCKET_CONTEXT::operator=(const _PER_SOCKET_CONTEXT rhv)
{
	link = rhv.link;
	storeID = rhv.storeID;
	memcpy(clientAddr, (rhv.clientAddr), sizeof(CORE_SOCKETADDR_IN));
	int nIoCount = (int)rhv.vIoContext.size();
	for (int i = 0; i < nIoCount; i++)
	{
		if (nullptr != rhv.vIoContext[i])
		{
			LPOPERATE_IO_CONTEXT pTemp = GetNewIoOperate();
			pTemp = rhv.vIoContext[i];
		}

	}
	
	return *this;
}

void _PER_SOCKET_CONTEXT::ClearOperate()
{
	int nIoCount = (int)vIoContext.size();
	if (nIoCount < 1)
		return;

	for (int i = 0; i < nIoCount; i++)
	{
		if (nullptr != vIoContext[i])
			delete vIoContext[i];

		vIoContext[i] = nullptr;
	}

	vIoContext.clear();
}

LPOPERATE_IO_CONTEXT _PER_SOCKET_CONTEXT::GetNewIoOperate()
{
	LPOPERATE_IO_CONTEXT p = new OPERATE_IO_CONTEXT();
	vIoContext.push_back(p);

	return p;
}

void _PER_SOCKET_CONTEXT::RemoveIoOperate(LPOPERATE_IO_CONTEXT pOperate)
{
	if (nullptr == pOperate)
		return;

	std::vector<LPOPERATE_IO_CONTEXT>::iterator iter = vIoContext.begin();
	for (; iter != vIoContext.end(); ++iter)
	{
		if (*iter == pOperate)
		{
			delete pOperate;
			pOperate = nullptr;
			iter = vIoContext.erase(iter);
			return;
		}
	}
}



#pragma endregion

