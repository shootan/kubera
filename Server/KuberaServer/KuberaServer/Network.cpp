#include "Network.h"


Network::Network()
{

}

Network::~Network()
{

}

BOOL Network::InitServer(int _port)
{
	//���� ����
	int retval;

	if(WSAStartup(MAKEWORD(2,2), &m_Wsa) != 0)
		return FALSE;

	retval = WSAGetLastError();

	//listensocket �ʱ�ȭ
	sockaddr_in accept_addr;
	m_ListenSock = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	ZeroMemory(&accept_addr,sizeof(accept_addr));

	//���� IP, ��Ʈ ����
	accept_addr.sin_port = htons(_port);
	accept_addr.sin_family = AF_INET;
	accept_addr.sin_addr.s_addr  = htonl(INADDR_ANY);

	retval = bind(m_ListenSock, (sockaddr*)&accept_addr, sizeof(sockaddr));
	if(SOCKET_ERROR == retval)
	{
		return FALSE;
	}
	
	retval = listen(m_ListenSock, 10);

	return TRUE;
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
	server_addr.sin_addr.s_addr  = htonl(INADDR_ANY);

	retval = connect(m_ConnectSock, (sockaddr*)&server_addr, sizeof(sockaddr));
	if(SOCKET_ERROR == retval)
	{
		return FALSE;
	}

	return TRUE;
}