#include <stdio.h>
#include <WinSock2.h>
#include <stdlib.h>

#pragma comment(lib, "ws2_32.lib")
HANDLE hIocp;
int g_Port;
int main()
{
	WSADATA wsa;
	SOCKET sock;
	SOCKADDR_IN serveraddr;
	HANDLE h_Iocp;
	int retval = WSAStartup(MAKEWORD(2,2), &wsa);
	h_Iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	
	retval = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	
	ZeroMemory(&serveraddr, sizeof(struct SOCKADDR));
	serveraddr.sin_addr.S_addr = htonl("127.0.0.1");
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(g_Port);

	retval = bind(sock, (SOCKADDR*)&serveraddr, sizeof(struct SOCKADDR));
	if(retval == SOCKET_ERROR)
	{
		
	}







	return 0;
}

DWORD WINAPI Process(LPVOID arg)
{

}