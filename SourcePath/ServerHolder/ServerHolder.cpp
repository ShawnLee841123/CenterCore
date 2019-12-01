// ServerCore.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <stdio.h>


//#include "PortCompletion/PortCompleteCore.h"
#include "../InterNetCore/PortCompletion/PortCompleteCore.h"

int main()
{

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

	return 0;
}

