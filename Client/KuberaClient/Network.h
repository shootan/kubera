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
	PlayerPacket PI;
	bool m_bJoinPlayer;

	Network();
	~Network();

	BOOL InitClient(char *_ip, int _port);

	void ShutDown();

	void err_display(char *msg);
	void err_quit(char *msg);

	BOOL SendData(PlayerPacket* PI);


	//½º·¹µå
	static UINT WINAPI WorkerThread(LPVOID arg);

};