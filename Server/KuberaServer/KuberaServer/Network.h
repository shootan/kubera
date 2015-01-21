#pragma once
#include "stdafx.h"

class Network 
{
private:
	SOCKET m_ListenSock;
	WSADATA m_Wsa;
	
public:
	Network();
	~Network();

	BOOL InitServer(int _port);
	void ShutDown();

};
