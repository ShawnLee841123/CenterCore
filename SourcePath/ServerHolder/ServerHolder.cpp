// ServerCore.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <stdio.h>


//#include "PortCompletion/PortCompleteCore.h"
#ifdef _WIN_
#include "../InterNetCore/PortCompletion/PortCompleteCore.h"
#else
#endif

int main()
{

#ifdef _WIN_
	PortCompleteCore oCore;
	if (!oCore.Initialize(nullptr))
		return 0;

	if (!oCore.Start())
		return 0;

	while (true)
	{
		oCore.Tick(0);
	}

	oCore.Destroy();
#else
#endif
	return 0;
}

