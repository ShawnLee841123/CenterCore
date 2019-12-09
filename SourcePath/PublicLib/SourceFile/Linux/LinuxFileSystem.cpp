
#include "../../Include/Linux/LinuxFileSystem.h"



//	获取当前工作路径
bool Linux_GetCurrentDir(char* strOut, UI32 strCount)
{
	char* res = getcwd(strOut, strCount);
	if (nullptr == res)
	{
		strOut[0] = 0;
		return false;
	}
	
	return true;
}

//	创建目录
bool Linux_CreatePath(const char* strName)
{
	if (nullptr == strName)
		return false;

	if (0 == strcmp("", strName))
		return false;

	//	S_IRWXU is 0700
	return 0 == mkdir(strName, S_IRWXU);
}

//	删除目录
bool Linux_DeletePath(const char* strName)
{
	if (nullptr == strName)
		return false;

	if (0 == strcmp("", strName))
		return false;

	return 0 == rmdir(strName);
}

//	文件是否存在
bool Linux_FileExists(const char* strFileName)
{
	if (nullptr == strFileName)
		return false;

	if (0 == strFileName[0])
		return false;

	return EFPCR_SUCCESS == Linux_CheckFilePermission(strFileName, EFCST_EXISTS);
}

//	目录是否存在
bool Linux_PathExists(const char* strName)
{
	if (nullptr == strName)
		return false;

	if (0 == strName[0])
		return false;

	return EFPCR_SUCCESS == Linux_CheckFilePermission(strName, EFCST_EXISTS);
}

//	创建文件(未实现)
FILE* Linux_CreateFile(const char* strFileName)
{
	return fopen(strFileName, "w");
}

//	删除文件
bool Linux_DeleteFile(const char* strFileName)
{
	if (nullptr == strFileName)
		return false;

	if (0 == strFileName[0])
		return false;

	if (!Linux_FileExists(strFileName))
		return false;

	EFileDeleteErrorType eRetType = (EFileDeleteErrorType)remove(strFileName);
	return EFDET_SUCCESS == eRetType;
}

//	检查文件或目录权限(非公开调用)
EFilePermissionCheckResult Linux_CheckFileOrPathPermission(const char* strName, EFileCheckSystemType eType)
{
	if (nullptr == strName)
		return EFPCR_NO_PERMISSION;

	if (0 == strName[0])
		return EFPCR_NO_PERMISSION;

	return ((access(strName, (SI32)eType) < 0) ? EFPCR_NO_PERMISSION : EFPCR_SUCCESS);
}

//	检查文件或目录是否有权限（公开）
//	参数strName问路径时，只能检查路径是否存在
EFilePermissionCheckResult Linux_CheckFilePermission(const char* strName, SI32 eType)
{
	int eRet = (int)EFPCR_NO_PERMISSION;
	if (nullptr == strName)
		return (EFilePermissionCheckResult)eRet;

	if (0 == strName[0])
		return (EFilePermissionCheckResult)eRet;

	if (EFCT_NONE == (EFileCheckType)eType)
		return (EFilePermissionCheckResult)eRet;

	if ((EFCST_EXISTS & eType) == EFCST_EXISTS)
		eRet &= (SI32)Linux_CheckFileOrPathPermission(strName, EFCST_EXISTS);

	if (((EFCT_READ & eType) == EFCT_READ) && ((EFCT_WRITE & eType) == EFCT_WRITE))
	{
		eRet &= (SI32)Linux_CheckFileOrPathPermission(strName, EFCST_READ_WRITE);
		return (EFilePermissionCheckResult)eRet;
	}

	if ((EFCT_READ & eType) == EFCT_READ)
	{
		eRet &= (SI32)Linux_CheckFileOrPathPermission(strName, EFCST_READ);
		return (EFilePermissionCheckResult)eRet;
	}

	if ((EFCT_WRITE & eType) == EFCT_WRITE)
	{
		eRet &= (SI32)Linux_CheckFileOrPathPermission(strName, EFCST_WRITE);
		return (EFilePermissionCheckResult)eRet;
	}

	return (EFilePermissionCheckResult)eRet;
}

void Linux_PrintLogTextToScreen(const char* strValue, void* pConsole, ELogLevelType eType)
{
	if (nullptr == strValue)
		return;

	if (0 == strValue[0])
		return;

	if (eType == ELLT_ERROR)
		printf("\033[1m\033[40;31m%s\033[0m", strValue);
	else if (eType == ELLT_WARNING)
		printf("\033[1m\033[40;33m%s\033[0m", strValue);
	else
		printf("%s", strValue);
}

