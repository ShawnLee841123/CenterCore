
#ifndef __EPOLL_CENTER_CORE_H__
#define __EPOLL_CENTER_CORE_H__

#include "../CenterCoreBasic/CenterCoreBasic.h"
#include "../../PublicLib/Include/Common/TypeDefines.h"

#include <map>

#pragma region declear forward extern

class UnLockQueueDataElementBase;
class UnLockQueueBase;
#pragma endregion

class EpollCenterCore: public CenterCoreBasic
{
public:
	EpollCenterCore();
	virtual ~EpollCenterCore();

protected:

#pragma region protect function

#pragma region Parent function override
	virtual bool OnInitialize() override;
	virtual bool OnStart() override;
	virtual bool OnTick(int nElapse) override;
	virtual bool OnDestroy() override;
#pragma endregion

	virtual int64 MakeStoreID();

#pragma region Queue process
	virtual bool OnReadQueueTick(int nElapse);
	virtual bool OnQueueElementProcessEnter(UnLockQueueDataElementBase* pElement);
#pragma endregion

	virtual bool OnSocketMessage(UnLockQueueDataElementBase* pElement);
#pragma endregion

#pragma region variable

	std::map<int64, int32> 			m_dicWorkerSocketCount;
	std::map<int32, UnLockQueueBase*>	m_dicCoreReadQueue;
	std::map<int32, UnLockQueueBase*>	m_dicCoreWriteQueue;
#pragma endregion

};

#endif


