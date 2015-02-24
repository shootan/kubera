#include "Network.h"

Network::Network()
{
	ZeroMemory(&PI, sizeof(PlayerInfo));
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
	char Buf[BUFSIZE];
	
	while(TRUE)
	{
		/*recv(server->m_ConnectSock,	(char*)&size, sizeof(int), 0);
		
		if(size > 0)
		{
			recv(server->m_ConnectSock, Buf, size, 0);

		}*/
		retval = recv(server->m_ConnectSock, (char*)&server->PI, sizeof(PlayerInfo), 0);
		if(retval == SOCKET_ERROR)
			break;

	}
	return 0;
}

BOOL Network::SendData(PlayerInfo* _pi)
{
	int retval = 0;
	retval = send(m_ConnectSock, (char*)_pi, sizeof(PlayerInfo), 0);
	if(retval == SOCKET_ERROR)
		return FALSE;

	return TRUE;

}