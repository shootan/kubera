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
	PlayerStruct PI[10];

	MinionInfo	MI1[40];
	MinionInfo	MI2[40];
	MinionInfo	MI3[40];
	MinionInfo	MI4[40];

	bool	m_bJoinPlayer;
	int		m_ClientCount;
	int		m_ID;
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