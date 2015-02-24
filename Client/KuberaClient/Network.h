#pragma once

//network
#include "header.h"

#pragma comment(lib, "ws2_32")

#define  BUFSIZE 1024*2

class Network 
{
public:
	SOCKET m_ConnectSock;
	WSADATA m_Wsa;
	PlayerInfo PI;

	Network();
	~Network();

	BOOL InitClient(char *_ip, int _port);

	void ShutDown();

	void err_display(char *msg);
	void err_quit(char *msg);

	BOOL SendData(PlayerInfo* _pi);


	//½º·¹µå
	static UINT WINAPI WorkerThread(LPVOID arg);

};