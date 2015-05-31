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
	PlayerStruct PI[4];

	MinionInfo	MI1[40];
	MinionInfo	MI2[40];
	MinionInfo	MI3[40];
	MinionInfo	MI4[40];

	MinionInfo	MI[160];

	bool	m_bJoinPlayer;
	int		m_ClientCount;
	int		m_ID;
	Vector3 m_Pos;
	float	m_HP;

	Network();
	~Network();

	BOOL InitClient(char *_ip, int _port);

	void ShutDown();

	void err_display(char *msg);
	void err_quit(char *msg);

	BOOL SendData(PlayerPacket* PI);
	BOOL SendType(int _type);


	//½º·¹µå
	static UINT WINAPI WorkerThread(LPVOID arg);

};