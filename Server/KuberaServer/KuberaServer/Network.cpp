#include "Network.h"


Network::Network()
{

}

Network::~Network()
{

}

BOOL Network::InitServer(int port)
{
	//서버 시작
	int retval;

	if(WSAStartup(MAKEWORD(2,2), &m_Wsa) != 0)
		return -1;

	retval = WSAGetLastError();

	//listensocket 초기화
	sockaddr_in accept_addr;
	m_ListenSock = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	ZeroMemory(&accept_addr,sizeof(accept_addr));

	//서버 IP, 포트 설정
	accept_addr.sin_port = htons(port);
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