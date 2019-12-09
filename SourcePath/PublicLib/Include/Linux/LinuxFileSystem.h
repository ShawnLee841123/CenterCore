





#ifndef __LIB_LINUX_FILE_SYSTEM_H__
#define __LIB_LINUX_FILE_SYSTEM_H__

#include "../Common/TypeDefines.h"
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

//	获取当前工作路径
bool Linux_GetCurrentDir(char* strOut, UI32 strCount);

//	创建目录
bool Linux_CreatePath(const char* strName);

//	删除目录
bool Linux_DeletePath(const char* strName);

//	文件是否存在
bool Linux_FileExists(const char* strFileName);

//	目录是否存在
bool Linux_PathExists(const char* strName);

//	创建文件(未实现)
FILE* Linux_CreateFile(const char* strFileName);

//	删除文件
bool Linux_DeleteFile(const char* strFileName);

//	检查文件或目录权限(非公开调用)
EFilePermissionCheckResult Linux_CheckFileOrPathPermission(const char* strName, EFileCheckSystemType eType);

//	检查文件或目录是否有权限（公开）
//	参数strName问路径时，只能检查路径是否存在
EFilePermissionCheckResult Linux_CheckFilePermission(const char* strName, SI32 eType);

void Linux_PrintLogTextToScreen(const char* strValue, void* pConsole, ELogLevelType eType);

#endif

