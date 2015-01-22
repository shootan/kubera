#pragma once
#include "stdafx.h"

class Network 
{
protected:
	SOCKET m_ListenSock;
	SOCKET m_ConnectSock;
	WSADATA m_Wsa;
	
public:
	Network();
	~Network();

	BOOL InitServer(int _port);
	BOOL InitClient(char *_ip, int _port);

	void ShutDown();

};