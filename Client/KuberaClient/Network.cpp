#include "Network.h"

Network::Network()
{
	ZeroMemory(PI, sizeof(PlayerStruct) * 10);
	ZeroMemory(MI1, sizeof(MinionInfo)*40);
	ZeroMemory(MI2, sizeof(MinionInfo)*40);
	ZeroMemory(MI3, sizeof(MinionInfo)*40);
	ZeroMemory(MI4, sizeof(MinionInfo)*40);

	ZeroMemory(MI, sizeof(MinionInfo)*160);
	m_bJoinPlayer = FALSE;
	m_ClientCount = 0;
	m_ID = 0;
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
				break;
			}
		case HERODATA:
			{
				retval = recv(server->m_ConnectSock, (char*)&p, sizeof(PlayerPacket), 0);
				if(retval == SOCKET_ERROR)
					break;

				for(int i=0; i<10; i++)
				{
					if(server->PI[i].PI.m_ID == p.PI.m_ID)
					{
						server->PI[i].PI = p.PI;
						break;
					}

					if(i == 9)
					{
						for(int j=0; j<10; j++)
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
		case MINIONDATA:
			{
				retval = recv(server->m_ConnectSock, (char*)&server->MI, sizeof(MinionInfo)*160, 0);
				if(retval == SOCKET_ERROR)
					break;
				/*recv(server->m_ConnectSock,(char*)&Number, sizeof(int), 0);

				switch(Number)
				{
				case 1:
					retval = recv(server->m_ConnectSock, (char*)server->MI1, sizeof(MinionInfo)*40, 0);
					break;
				case 2:
					retval = recv(server->m_ConnectSock, (char*)server->MI2, sizeof(MinionInfo)*40, 0);
					break;
				case 3:
					retval = recv(server->m_ConnectSock, (char*)server->MI3, sizeof(MinionInfo)*40, 0);
					break;
				case 4:
					retval = recv(server->m_ConnectSock, (char*)server->MI4, sizeof(MinionInfo)*40, 0);
					break;
				}*/
				

				for(int i=0; i<40; i++)
				{
// 					if(server->MI1[i].m_Live == TRUE)
// 						printf("%.1f, %.1f, %.1f \n", server->MI1[i].m_Pos.x,server->MI1[i].m_Pos.y,server->MI1[i].m_Pos.z);
//  					if(server->MI2[i].m_Live == TRUE)
// 						printf("%.1f, %.1f, %.1f \n", server->MI2[i].m_Pos.x,server->MI2[i].m_Pos.y,server->MI2[i].m_Pos.z);
// 					if(server->MI3[i].m_Live == TRUE)
// 						printf("%.1f, %.1f, %.1f \n", server->MI3[i].m_Pos.x,server->MI3[i].m_Pos.y,server->MI3[i].m_Pos.z);
//  					if(server->MI4[i].m_Live == TRUE)
//  						printf("%.1f, %.1f, %.1f \n", server->MI4[i].m_Pos.x,server->MI4[i].m_Pos.y,server->MI4[i].m_Pos.z);

				}
				/*retval = recv(server->m_ConnectSock, (char*)&Number, sizeof(int));

				switch(Number)
				{
				case 1:
					retval = recv(server->m_ConnectSock, (char*)server->MI1, sizeof(MinionInfo)*40, 0);
					break;
				case 2:
					retval = recv(server->m_ConnectSock, (char*)server->MI2, sizeof(MinionInfo)*40, 0);
					break;
				case 3:
					retval = recv(server->m_ConnectSock, (char*)server->MI3, sizeof(MinionInfo)*40, 0);
					break;
				case 4:
					retval = recv(server->m_ConnectSock, (char*)server->MI4, sizeof(MinionInfo)*40, 0);
					break;
				}*/
				

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