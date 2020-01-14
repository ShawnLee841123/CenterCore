

#include "ClientDemoCore.h"


ClientDemoCore::ClientDemoCore(): m_Sock(0), m_bInitial(false)
{}

ClientDemoCore::~ClientDemoCore()
{
	SAFE_RELEASE_SOCKET(m_Sock);
}

bool ClientDemoCore::Initialize()
{
	return m_bInitial;
}

bool ClientDemoCore::Start()
{
	return true;
}

bool ClientDemoCore::MainLoop()
{
	return false;
}

bool ClientDemoCore::CreateServerLink()
{
	return false;
}

bool ClientDemoCore::ReciveMsg()
{
	return false;
}

bool ClientDemoCore::SendMsg()
{
	return false;
}