#include "IOCP.h"

IOCPServer::IOCPServer()
{
	InitializeCriticalSection(&m_BufferListLock);
	m_pNextBufferList = NULL;
	m_hIO = NULL;
	m_hListenThread = NULL;
	m_hWorkerThread = NULL;

	m_bServerShutDown = TRUE;
	m_bServerStart = FALSE;

	ZeroMemory(&m_ClinetAddr, sizeof(SOCKADDR));
}

IOCPServer::~IOCPServer()
{
	DeleteCriticalSection(&m_BufferListLock);
}

BOOL IOCPServer::StartServer(int port)
{
	m_hIO = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0); 
	m_bServerStart = TRUE;
	m_bServerShutDown = FALSE;

	if(!this->InitServer(port))
		return FALSE;
	
	if(!this->InitThread())
		return FALSE;

	return TRUE;
}

BOOL IOCPServer::InitThread()
{
	DWORD ThreadID;
	
	m_hListenThread = CreateThread(NULL, 0, ListenThread, this, 0, &ThreadID);
	if(m_hListenThread == NULL)
		return FALSE;

	// CPU ����Ȯ�� 
	SYSTEM_INFO si; 
	GetSystemInfo(&si); 

	// (CPU ����* 2)�����۾��ڽ�������� 
	for(int i=0; i < (int)si.dwNumberOfProcessors*2; ++i) 
	{ 
		m_hWorkerThread = CreateThread(NULL, 0, WorkerThread, this, 0, &ThreadID); 
		if(m_hWorkerThread == NULL) 
			return FALSE;
	} 
	return TRUE;
}

DWORD WINAPI IOCPServer::ListenThread(LPVOID arg)
{
	IOCPServer* pThis = (IOCPServer*)arg;
	
	while (1)
	{
		if(pThis->m_bServerShutDown) break;
		ZeroMemory(&pThis->m_ClinetAddr, sizeof(SOCKADDR));
		int addr_len = sizeof(pThis->m_ClinetAddr);

		SOCKET client_sock = WSAAccept(pThis->m_ListenSock, (sockaddr *)&pThis->m_ClinetAddr, &addr_len, NULL, NULL);
		if (client_sock == INVALID_SOCKET) {
			exit(-1);
		}

		printf("[TCP ����] Ŭ���̾�Ʈ����: IP �ּ�= %s, ��Ʈ��ȣ= %d \n", 
			inet_ntoa(pThis->m_ClinetAddr.sin_addr), 
			ntohs(pThis->m_ClinetAddr.sin_port)); 

		CreateIoCompletionPort((HANDLE)client_sock, pThis->m_hIO, 0, 0);

		//���� ����
		IOBuffer* buffer = new IOBuffer;
		ZeroMemory(buffer, sizeof(IOBuffer));
		buffer->m_pNext = pThis->m_pNextBufferList;
		pThis->m_pNextBufferList = buffer;
		buffer->recvbytes = 0;
		buffer->sendbytes = 0;
		buffer->wsabuf.buf = buffer->m_Buf;
		buffer->wsabuf.len = BUFSIZE;
		buffer->m_ClientSock = client_sock;

		//�񵿱� ����� ����
		DWORD recvbytes;
		DWORD flags = 0;

		int retval = WSARecv(client_sock, &(buffer->wsabuf), 1, &recvbytes, &flags, &(buffer->m_Overlapped), NULL);
		if(retval == SOCKET_ERROR)
		{
			if(WSAGetLastError() != ERROR_IO_PENDING)
			{ 
				return -1;
			} 
			
		}		
	}
}

DWORD WINAPI IOCPServer::WorkerThread(LPVOID arg)
{
	int retval = 0;
	DWORD dwSize;
	IOBuffer* buff;
	IOCPServer* server = (IOCPServer*)arg;
	while(1)
	{
		GetQueuedCompletionStatus(server->m_hIO, &dwSize, (PULONG_PTR)&buff, (LPOVERLAPPED*)&buff->m_Overlapped, INFINITE);

		// Ŭ���̾�Ʈ������� 
		SOCKADDR_IN clientaddr; 
		int addrlen = sizeof(clientaddr); 
		getpeername(buff->m_ClientSock, (SOCKADDR*)&clientaddr, &addrlen); 

		if(dwSize == 0)
		{
			break;
		}
		else
		{
			DWORD b;
			b= dwSize;

		}

		
	}
	return 0;
}