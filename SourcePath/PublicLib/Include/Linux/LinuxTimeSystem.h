

#ifndef __LINUX_TIME_SYSTEM_H__
#define __LINUX_TIME_SYSTEM_H__

#include "../Common/TypeDefines.h"

//	获取格林尼治时间（单位：秒）
double Linux_GetGMTTime();

//	获取格林尼治时间时间戳（单位：毫秒）
double Linux_GetGMTTimeStamp();

//	获取时间戳（单位：毫秒）
uint32 Linux_GetTimeStamp();

//	获取日期
bool Linux_GetCurTimeString(char* strTime, uint32 uStrCout, const char* strParam = "", bool bNeedDayOfWeek = false);

#endif	//	__LINUX_TIME_SYSTEM_H__

