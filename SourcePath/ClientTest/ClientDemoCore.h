
#ifndef __CLIENT_DEMO_CORE_H__
#define __CLIENT_DEMO_CORE_H__

#include <winsock.h>

#define SAFE_RELEASE_SOCKET(a) {if (INVALID_SOCKET != a){closesocket(a); a = INVALID_SOCKET;}}

class ClientDemoCore
{
public:
	ClientDemoCore();
	~ClientDemoCore();

	bool Initialize();
	bool Start();
	bool MainLoop();

	bool CreateServerLink();
	bool ReciveMsg();
	bool SendMsg();

protected:
	bool			m_bInitial;

	SOCKET			m_Sock;
};

#endif	//	__CLIENT_DEMO_CORE_H__
