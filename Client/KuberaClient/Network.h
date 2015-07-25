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
	int			OtherChar;




	bool	m_bJoinPlayer;
	int		m_ClientCount;
	int		m_ID;
	Vector3 m_Pos;
	float	m_HP;
	int		m_Type;

	BOOL	m_InitFinish;

	Network();
	~Network();

	BOOL InitClient(char *_ip, int _port);

	void ShutDown();

	void err_display(char *msg);
	void err_quit(char *msg);

	BOOL SendData(int _header, void* _packet, int _size);
	BOOL SendHeader(int _header);
	BOOL SendType(int _type);


	//½º·¹µå
	static UINT WINAPI WorkerThread(LPVOID arg);

};