#include "Network.h"

Network::Network()
{
	ZeroMemory(&PI, sizeof(PlayerPacket));
	m_bJoinPlayer = FALSE;
}

Network::~Network()
{

}

// �����Լ�������������� 
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
// �����Լ�������� 
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

	//���� ����
	int retval;

	if(WSAStartup(MAKEWORD(2,2), &m_Wsa) != 0)
		return FALSE;

	retval = WSAGetLastError();

	//listensocket �ʱ�ȭ
	sockaddr_in server_addr;

	ZeroMemory(&server_addr,sizeof(server_addr));

	m_ConnectSock = socket(AF_INET, SOCK_STREAM, 0);

	//���� IP, ��Ʈ ����
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
	char Buf[BUFSIZE];
	
	while(TRUE)
	{
		retval = recv(server->m_ConnectSock, (char*)&server->PI, sizeof(PlayerPacket), 0);

		if(retval == SOCKET_ERROR)
			break;

		//printf("x: %d y: %d z : %d \n ", server->PI.PI.m_Pos.x, server->PI.PI.m_Pos.y, server->PI.PI.m_Pos.z);
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