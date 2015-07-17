#include "Network.h"

Network::Network()
{
	ZeroMemory(PI, sizeof(PlayerStruct) * 10);

	m_bJoinPlayer = FALSE;
	m_ClientCount = 0;
	m_ID = 0;
	m_Type = 0;
	m_InitFinish = TRUE;
}

Network::~Network()
{

}

// 소켓함수오류출력후종료 
void Network::err_quit(char *msg) 
{ 
	LPVOID lpMsgBuf; 
	FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER| 
		FORMAT_MESSAGE_FROM_SYSTEM, 
		NULL, WSAGetLastError(), 
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (LPCTSTR)lpMsgBuf); 
	LocalFree(lpMsgBuf); 
	exit(-1); 
}
// 소켓함수오류출력 
void Network::err_display(char *msg) 
{ 
	LPVOID lpMsgBuf; 
	FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER| 
		FORMAT_MESSAGE_FROM_SYSTEM, 
		NULL, WSAGetLastError(), 
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
		(LPTSTR)&lpMsgBuf, 0, NULL); 
	printf("[%s] %s", msg, (LPCTSTR)lpMsgBuf); 
	LocalFree(lpMsgBuf); 
}

BOOL Network::InitClient(char *_ip, int _port)
{

	//서버 시작
	int retval;

	if(WSAStartup(MAKEWORD(2,2), &m_Wsa) != 0)
		return FALSE;

	retval = WSAGetLastError();

	//listensocket 초기화
	sockaddr_in server_addr;

	ZeroMemory(&server_addr,sizeof(server_addr));

	m_ConnectSock = socket(AF_INET, SOCK_STREAM, 0);

	//서버 IP, 포트 설정
	server_addr.sin_port = htons(_port);
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr  = inet_addr(_ip);

	retval = connect(m_ConnectSock, (sockaddr*)&server_addr, sizeof(sockaddr));
	HANDLE ThreadHandle;
	unsigned int ThreadID;
	ThreadHandle = (HANDLE)_beginthreadex(NULL, 0, WorkerThread, this, 0, &ThreadID);
	if(SOCKET_ERROR == retval)
	{
		return FALSE;
	}

	return TRUE;
}

UINT WINAPI Network::WorkerThread(LPVOID arg)
{
	int retval = 0;
	DWORD dwSize;
	unsigned long key;
	LPOVERLAPPED over;
	Network* server = (Network*)arg;
	int size = 0;
	PlayerPacket p;
	int Number = 0;
	char Buf[BUFSIZE];
	int Count = 0;
	int Header = 0;
	while(TRUE)
	{
		/////////////////////////////// 헤더 받아서 ㅇㅋ?

		retval = recv(server->m_ConnectSock, (char*)&Header, sizeof(int), 0);
		if(retval == SOCKET_ERROR)
			break;

		switch(Header)
		{
		case INITCLIENT:
			{
				retval = recv(server->m_ConnectSock, (char*)&server->m_ID, sizeof(int), 0);
				retval = recv(server->m_ConnectSock, (char*)&server->m_Pos, sizeof(Vector3), 0);
				retval = recv(server->m_ConnectSock, (char*)&server->m_HP, sizeof(float), 0);
				retval = recv(server->m_ConnectSock, (char*)&server->m_Type, sizeof(int), 0);
				server->m_InitFinish = FALSE;
				break;
			}
		case HERODATA:
			{
				retval = recv(server->m_ConnectSock, (char*)&p, sizeof(PlayerPacket), 0);
				if(retval == SOCKET_ERROR)
					break;

				for(int i=0; i<4; i++)
				{
					if(server->PI[i].PI.m_ID == p.PI.m_ID)
					{
						server->PI[i].PI = p.PI;
						break;
					}

					if(i == 3)
					{
						for(int j=0; j<4; j++)
						{
							if(server->PI[j].PI.m_ID == 0)
							{
								server->PI[j].PI = p.PI;
								break;
							}
						}
					}
				}
				break;
			}
		case HEROCOUNT:
			{
				retval = recv(server->m_ConnectSock, (char*)&server->m_ClientCount, sizeof(int), 0);
				if(retval == SOCKET_ERROR)
					break;

				break;
			}
		
		}


	}
	return 0;
}

BOOL Network::SendData(PlayerPacket* _pi)
{
	int retval = 0;
	int p = sizeof(PlayerPacket);
	
	retval = send(m_ConnectSock, (char*)_pi, sizeof(PlayerPacket), 0);
	if(retval == SOCKET_ERROR)
		return FALSE;

	return TRUE;
}

BOOL Network::SendType(int _type)
{
	int retval = 0;
	int header = HEROCHOOSE;
	int p = sizeof(PlayerPacket);
	retval = send(m_ConnectSock, (char*)&header, sizeof(int), 0);
	if(retval == SOCKET_ERROR)
		return FALSE;

	retval = send(m_ConnectSock, (char*)&_type, sizeof(PlayerPacket), 0);
	if(retval == SOCKET_ERROR)
		return FALSE;

	return TRUE;
}