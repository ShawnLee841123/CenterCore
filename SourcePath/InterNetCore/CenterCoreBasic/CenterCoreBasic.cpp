﻿
#include "CenterCoreBasic.h"
#include "../../PublicLib/Include/Common/LogThreadBase.h"
#include "../../PublicLib/Include/Common/UnLockQueue.h"
#include "../../PublicLib/Include/Common/StandardUnLockElement.h"
#include "../../PublicLib/Include/Common/tools.h"
#include "../../PublicLib/Include/System/FileSystem.h"
#include "../../PublicLib/Include/System/TimeSystem.h"
#include <vector>
#include <string.h>
//	for va_start va_end
#include <stdarg.h>
#ifdef _WIN_
#include <windows.h>
#endif





CenterCoreBasic::CenterCoreBasic() : m_pParentCore(nullptr), m_pSelfLogQueue(nullptr), m_pLogRegisterQueue(nullptr), m_pLogThread(nullptr)
{
	sprintf(m_strName, "%s", "CenterCoreBasic");
}
CenterCoreBasic::~CenterCoreBasic()
{}

#pragma region Parent Interface
//	初始化
bool CenterCoreBasic::Initialize(ICenterCore* pCoreRoot)
{
	m_pParentCore = pCoreRoot == nullptr ? nullptr : pCoreRoot;
#pragma region Initial Log about
	if (nullptr == m_pLogThread)
	{
		m_pLogThread = new LogThreadBase();
		m_pLogThread->OnThreadInitialize(0);
		//	Get current working path
		char strCurWorkingPath[512] = { 0 };
		if (!GetCurrentDir(strCurWorkingPath, 512))
			return false;

		//	Make Log Path
#pragma region make log path
		std::vector<std::string> vPath = SplitString(strCurWorkingPath, "\\");
		int nPathCount = (int)vPath.size();
		if (nPathCount <= 0)
			return false;

		std::string strLogPath = "";
		for (int i = 0; i < nPathCount - 1; i++)
		{
			strLogPath += vPath[i] + "/";
		}

		strLogPath += "Log/";
		if (!CreatePath(strLogPath.c_str()))
			return false;
#pragma endregion

		//	Make log file 
#pragma region Make log file

		char strTime[64] = { 0 };
		GetCurTimeString(strTime, 64);
		std::string strLogFileName = strLogPath + m_strName;
		strLogFileName += "-";
		strLogFileName += strTime;
		strLogFileName += ".log";
		FILE* pLogFile = System_CreateFile(strLogFileName.c_str());
#pragma endregion

#pragma region call log thread "BeforeLogStart" func
#ifdef _WIN_
		//	windows system Get Console
		void* pHandle = GetStdHandle(STD_OUTPUT_HANDLE);
		if (nullptr == pHandle)
			return false;

		m_pLogThread->BeforeLogStart(2, 2, pLogFile, pHandle);
#else
		m_pLogThread->BeforeLogStart(2, 2, pLogFile);
#endif
#pragma endregion

	}
#pragma endregion
	return OnInitialize();
}
//	开始
bool CenterCoreBasic::Start()
{
	return OnStart();
}
//	循环
bool CenterCoreBasic::Tick(int nElapse)
{
	return OnTick(nElapse);
}
//	销毁
bool CenterCoreBasic::Destroy()
{
	OnDestroy();
	//	Release log register queue
	if (nullptr != m_pLogRegisterQueue)
	{
		m_pLogRegisterQueue->Destroy();
		delete m_pLogRegisterQueue;
	}
	m_pLogRegisterQueue = nullptr;

	//	Release self log queue
	if (nullptr != m_pSelfLogQueue)
	{
		m_pSelfLogQueue->Destroy();
		delete m_pSelfLogQueue;
	}
	m_pSelfLogQueue = nullptr;

	//	release log thread
	if (nullptr != m_pLogThread)
	{
		m_pLogThread->OnThreadDestroy();
		delete m_pLogThread;
	}
	m_pLogThread = nullptr;

	//	set parent core null
	m_pParentCore = nullptr;

	return true;
}
//	获取核心根
ICenterCore* CenterCoreBasic::GetCoreRoot()
{
	return m_pParentCore; 
}
//	注册
bool CenterCoreBasic::RegisterServerCore(ICenterCore* pCore)
{
	if (nullptr == pCore)
	{
		//	todo: Error log
		return false;
	}

	std::map<std::string, ICenterCore*>::iterator iter = m_dicCoreElement.find(pCore->GetName());
	if (iter != m_dicCoreElement.end())
	{
		//	todo: Error log
		return false;
	}

	m_dicCoreElement.insert(std::pair<std::string, ICenterCore*>(pCore->GetName(), pCore));
	return true;
}

const char* CenterCoreBasic::GetName()
{
	return m_strName;
}

bool CenterCoreBasic::ThreadRegisterLog(ThreadBase* pThread, const char* strQueueName)
{
	if (nullptr == pThread)
		return false;

	if (nullptr == strQueueName)
		return false;

	if (strcmp(strQueueName, "") == 0)
	{
		return false;
	}

	if (nullptr == m_pLogRegisterQueue)
		return false;

	UnLockQueueBase* pThreadLogQueue = new UnLockQueueBase();
	pThread->RegisterQueue(pThreadLogQueue, strQueueName, ESQT_LOG_QUEUE);

	RegisterLogQueueData* pRegisterData = new RegisterLogQueueData();
	pRegisterData->bRegister = true;
	pRegisterData->pThreadLogQueue = pThreadLogQueue;
	pRegisterData->nThreadID = pThread->GetThreadID();

	m_pLogRegisterQueue->PushQueueElement(pRegisterData, sizeof(RegisterLogQueueData));
	return true;
}
#pragma region Log about

//	创建日志
void CenterCoreBasic::RegisterLogFile(const char* strLogName)
{

}
//	输出日志相关
void CenterCoreBasic::LogOutput(int nLevel, const char* strValue, ...)
{
	if (nullptr == strValue)
		return;

	if (strcmp(strValue, "") == 0)
		return;

	if (nLevel > ELLT_ERROR)
		return;

	if (nLevel < ELLT_ECHO)
		return;

	LogQueueElementData oData;
	va_list ap;
	va_start(ap, strValue);
	vsnprintf(oData.strLog, LOG_CHARACTER_MAX, strValue, ap);
	va_end(ap);

	int nLogLen = (int)strlen(oData.strLog) + 1;
	oData.nLogLevel = nLevel;
	oData.nThreadID = 0;

	m_pSelfLogQueue->PushQueueElement(&oData, sizeof(oData));
}
#pragma endregion
#pragma endregion

#pragma region Parent Interface
bool CenterCoreBasic::OnInitialize()
{
	if (nullptr != m_pParentCore)
		m_pParentCore->RegisterServerCore(this);

#pragma region Initialize log thread need register queue and self log queue. Then reigister queue into log thread
	if (nullptr == m_pLogRegisterQueue)
	{
		m_pLogRegisterQueue = new UnLockQueueBase();
	}
	m_pLogThread->RegisterQueue(m_pLogRegisterQueue, "RegisterQueue", ESQT_READ_QUEUE);

	if (nullptr == m_pSelfLogQueue)
	{
		m_pSelfLogQueue = new UnLockQueueBase();
	}
	m_pLogThread->RegisterQueue(m_pSelfLogQueue, "ServerCoreBaseLogQueue", ESQT_READ_QUEUE);
#pragma endregion

	//	active log thread
	m_pLogThread->OnThreadStart(0);

	return true;
}

bool CenterCoreBasic::OnStart()
{

	bool bRet = true;
	if (m_dicCoreElement.size() > 0)
	{
		std::map<std::string, ICenterCore*>::iterator iter = m_dicCoreElement.begin();
		for (; iter != m_dicCoreElement.end(); ++iter)
		{
			if (nullptr == iter->second)
			{
				//	todo Error log
				return false;
			}

			bRet &= iter->second->Start();
		}
	}

	CORE_MSG("Server Core OnStart");
	return bRet;
}

bool CenterCoreBasic::OnTick(int nElapse)
{
	bool bRet = true;
	if (m_dicCoreElement.size() > 0)
	{
		std::map<std::string, ICenterCore*>::iterator iter = m_dicCoreElement.begin();
		for (; iter != m_dicCoreElement.end(); ++iter)
		{
			if (nullptr == iter->second)
			{
				//	todo Error log
				return false;
			}

			bRet &= iter->second->Tick(nElapse);
		}
	}
	//CORE_MSG("Server Core Tick");
	return bRet;
}

bool CenterCoreBasic::OnDestroy()
{
	bool bRet = true;
	if (m_dicCoreElement.size() > 0)
	{
		std::map<std::string, ICenterCore*>::iterator iter = m_dicCoreElement.begin();
		for (; iter != m_dicCoreElement.end(); ++iter)
		{
			if (nullptr == iter->second)
			{
				//	todo Error log
				return false;
			}

			bRet &= iter->second->Destroy();
		}
	}

	CORE_MSG("Server Core OnDestroy");
	return bRet;
}
#pragma endregion 
