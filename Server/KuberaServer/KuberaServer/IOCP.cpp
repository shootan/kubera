#include "IOCP.h"

BOOL IOCPServer::StartServer(int port)
{
	m_hIO = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0); 


	if(!this->InitServer(port))
		return FALSE;
	
	if(!this->InitThread())
		return FALSE;

	return TRUE;

}

BOOL IOCPServer::InitThread()
{
	//CreateThread();
	return TRUE;
}

DWORD WINAPI IOCPServer::ListenThread(LPVOID arg)
{
	while (1)
	{
		int addr_len = sizeof(m_ListenSock);
		SOCKET client_sock = WSAAccept(m_ListenSock, (sockaddr *)(&m_ListenSock), &addr_len, NULL, NULL);
		if (INVALID_SOCKET == client_sock) {
			exit(-1);
		}

		CreateIoCompletionPort((HANDLE)client_sock, m_hIO, 0, 0);

		//���� ����
		IOBuffer* buffer = new IOBuffer;
		ZeroMemory(buffer, sizeof(IOBuffer));



		//���ۻ����� Recv�� ���.
		//WSARecv(client_sock, )
		
	}

}

DWORD WINAPI IOCPServer::WorkerThread(LPVOID arg)
{
	return 0;
}