

#include "../../Include/Linux/LinuxTimeSystem.h"
#include <time.h>
#include <sys/time.h>
#include <stdio.h>

//	获取格林尼治时间（单位：秒）
double Linux_GetGMTTime()
{
	struct timeval tv;
	gettimeofday(&tv, nullptr);
	return double(tv.tv_sec) + (double)tv.tv_usec / 1000 / 1000;
}

//	获取格林尼治时间时间戳（单位：毫秒）
double Linux_GetGMTTimeStamp()
{
	double dTimeStamp =  Linux_GetGMTTime() * 1000;
	return dTimeStamp;
}

//	获取时间戳（单位：毫秒）
UI32 Linux_GetTimeStamp()
{
	return Linux_GetGMTTimeStamp();
}

//	获取日期
bool Linux_GetCurTimeString(char* strTime, UI32 uStrCout, const char* strParam, bool bNeedDayOfWeek)
{
	if (nullptr == strTime)
		return false;

	if (uStrCout <= 0)
		return false;

	struct timeval tv;
	gettimeofday(&tv, nullptr);
	tm* pt = localtime(&tv.tv_sec);
	
	char strYear[16] = {0};
	sprintf(strYear, "%d", pt->tm_year + 1900);

	char strMonth[16] = {0};
	int nTempMonth = pt->tm_mon + 1;
	if (nTempMonth > 9)
		sprintf(strMonth, "%d", nTempMonth);
	else
		sprintf(strMonth, "0%d", nTempMonth);

	char strDay[16] = {0};
	if (pt->tm_mday > 9)
		sprintf(strDay, "%d", pt->tm_mday);
	else
		sprintf(strDay, "0%d", pt->tm_mday);

	char strHour[16] = {0};
	if (pt->tm_hour > 9)
		sprintf(strHour, "%d", pt->tm_hour);
	else
		sprintf(strHour, "0%d", pt->tm_hour);

	char strMinute[16] = {0};
	if (pt->tm_min > 9)
		sprintf(strMinute, "%d", pt->tm_min);
	else
		sprintf(strMinute, "0%d", pt->tm_min);

	char strSecond[16] = {0};
	if (pt->tm_sec > 9)
		sprintf(strSecond, "%d", pt->tm_sec);
	else
		sprintf(strSecond, "0%d", pt->tm_sec);

	char strMillonSec[16] = {0};
	int nTempMillonSec = tv.tv_usec / 1000;
	if (nTempMillonSec > 99)
		sprintf(strMillonSec , "%d", nTempMillonSec);
	else
	{
		if (nTempMillonSec > 9)
			sprintf(strMillonSec, "0%d", nTempMillonSec);
		else
			sprintf(strMillonSec, "00%d", nTempMillonSec);
	}

	char strDayOfWeek[16] = {0};
	sprintf(strDayOfWeek, "%d", pt->tm_wday);

	if (bNeedDayOfWeek)
		sprintf(strTime, "%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s", strYear, strParam, strMonth, strParam, strDay, strParam, strDayOfWeek, strParam, strHour, strParam, strMinute, strParam, strSecond, strParam, strMillonSec);
	else
		sprintf(strTime, "%s%s%s%s%s%s%s%s%s%s%s%s%s", strYear, strParam, strMonth, strParam, strDay, strParam, strHour, strParam, strMinute, strParam, strSecond, strParam, strMillonSec);

	return true;
}




