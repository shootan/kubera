#include "IOCP.h"

IOCPServer::IOCPServer()
{
	InitializeCriticalSection(&m_BufferListLock);
	m_pNextBufferList		= NULL;
	m_pPlayerList			= NULL;
	m_hIO					= NULL;
	m_hListenThread			= NULL;
	m_hWorkerThread			= NULL;

	m_bServerShutDown		= TRUE;
	m_bServerStart			= FALSE;

	m_iClientCount			= 0;

	ZeroMemory(&m_ClinetAddr, sizeof(SOCKADDR));
	ZeroMemory(m_ID, sizeof(char)*100);

}

IOCPServer::~IOCPServer()
{
	IOBuffer* next;
	while (m_pNextBufferList != NULL)
	{
		next = m_pNextBufferList;
		m_pNextBufferList = next->m_pNext;
		delete next;
	}
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
	unsigned int ThreadID;
	
	m_hListenThread = (HANDLE)_beginthreadex(NULL, 0, ListenThread, this, 0, &ThreadID);
	
	if(m_hListenThread == NULL)
		return FALSE;

	// CPU 갯수확인 
	SYSTEM_INFO si; 
	GetSystemInfo(&si); 

	// (CPU 개수* 2)개의작업자스레드생성 
	for(int i=0; i < (int)si.dwNumberOfProcessors*2; ++i) 
	{ 
		m_hWorkerThread = (HANDLE)_beginthreadex(NULL, 0, WorkerThread, this, 0, &ThreadID); 
		if(m_hWorkerThread == NULL) 
			return FALSE;
	} 
	return TRUE;
}

UINT WINAPI IOCPServer::ListenThread(LPVOID arg)
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
		
		pThis->m_iClientCount++;
		printf("IP 주소= %s, 포트번호= %d , Count = %d\n", 
			inet_ntoa(pThis->m_ClinetAddr.sin_addr), 
			ntohs(pThis->m_ClinetAddr.sin_port), pThis->m_iClientCount); 

		//버퍼 생성
		IOBuffer* buffer = new IOBuffer;
		ZeroMemory(buffer, sizeof(IOBuffer));
		buffer->m_Id = pThis->GetNewId();
		if(buffer->m_Id == 0)
		{
			delete buffer;
		}
		else
		{
			buffer->m_pNext = pThis->m_pNextBufferList;
			pThis->m_pNextBufferList = buffer;
			buffer->m_iRecvbytes = 0;
			buffer->m_iSendbytes = 0;
			buffer->m_Wsabuf.buf = buffer->m_RecvBuf;
			buffer->m_Wsabuf.len = BUFSIZE;
			buffer->m_ClientSock = client_sock;
			buffer->m_Opcode = OP_INIT;

			Player* pl = new Player;
			ZeroMemory(pl, sizeof(Player));
			pl->m_Id = buffer->m_Id;
			pl->m_pNext = pThis->m_pPlayerList;
			pThis->m_pPlayerList = pl;

			//비동기 입출력 시작
			DWORD flags = 0;
			CreateIoCompletionPort((HANDLE)client_sock, pThis->m_hIO, (DWORD)buffer, 0);

			PostQueuedCompletionStatus(pThis->m_hIO, 0, (ULONG_PTR)buffer, &buffer->m_Overlapped);
		}
	}
	return 0;
}

int IOCPServer::GetNewId()
{
	for(int i=1; i<100; i++)
	{
		if(m_ID[i] == NULL) return i;
	}

	return 0;
}

UINT WINAPI IOCPServer::WorkerThread(LPVOID arg)
{
	int retval = 0;
	DWORD dwSize;
	IOBuffer* buff;
	LPOVERLAPPED over;
	IOCPServer* server = (IOCPServer*)arg;

	while(!server->m_bServerShutDown)
	{
		GetQueuedCompletionStatus(server->m_hIO, &dwSize, (PULONG_PTR)&buff, (LPOVERLAPPED*)&over, INFINITE);

		if(dwSize == 0)
		{
			//접속해제
		}

		// 클라이언트정보얻기
		SOCKADDR_IN clientaddr; 
		int addrlen = sizeof(clientaddr); 
		getpeername(buff->m_ClientSock, (SOCKADDR*)&clientaddr, &addrlen);

		switch ( buff->m_Opcode )
		{
		case OP_INIT:
			server->OnInit(buff);
			break;
		case OP_RECV:
			server->OnRecv(buff, buff->m_RecvBuf, BUFSIZE);
			break;
		case OP_RECV_DONE:
			server->OnRecvFinish(buff, dwSize);
			break;
		case OP_SEND_FINISH:

			break;
		}
	}
	return 0;
}

void IOCPServer::SetOpCode(IOBuffer* _buff, OPCODE _opCode)
{
	ZeroMemory(&_buff->m_Overlapped, sizeof(OVERLAPPED));
	_buff->m_Opcode = _opCode;
}

void IOCPServer::OnInit(IOBuffer* _buff)
{
	BOOL bSuccess;
	this->SetOpCode(_buff, OP_RECV);
	bSuccess = PostQueuedCompletionStatus(m_hIO, 0, (ULONG_PTR)_buff, &(_buff->m_Overlapped));

	if ( !bSuccess && WSAGetLastError() != ERROR_IO_PENDING )
	{
		// error
	}
}

void IOCPServer::OnRecv(IOBuffer* _buff, char* _recvBuff, int _size)
{
	DWORD dwRecv = 0, dwFlags = 0;
	WSABUF buffRecv;

	this->SetOpCode(_buff, OP_RECV_DONE);

	buffRecv.buf = _recvBuff;
	buffRecv.len = _size;

	int retval = WSARecv(_buff->m_ClientSock, &buffRecv, 1, &dwRecv, &dwFlags, &(_buff->m_Overlapped), NULL);

	if ( retval == SOCKET_ERROR && (ERROR_IO_PENDING != WSAGetLastError())) 
	{
		// error
	}
}

void IOCPServer::OnRecvFinish(IOBuffer* _buff, DWORD _size)
{
	if ( _size == 0 )
	{
		m_iClientCount--;
		return;
	}
	
	_buff->m_iRecvbytes = _size;
	_buff->m_RecvBuf[_buff->m_iRecvbytes] = 0;

	
	printf("[RECV] %s\n", _buff->m_RecvBuf);
	
	SetOpCode(_buff, OP_RECV);
	BOOL bSuccess = PostQueuedCompletionStatus(m_hIO, 0, (ULONG_PTR)_buff, &(_buff->m_Overlapped));
	_buff->m_iRecvbytes = 0;

	if ( !bSuccess && WSAGetLastError() != ERROR_IO_PENDING )
	{
		// error
	}
}

void IOCPServer::OnSendFinish(IOBuffer* _buff, DWORD _size)
{
	if ( _size == 0 )
	{
		m_iClientCount--;
		return;
	}

	SetOpCode(_buff, OP_RECV);
	_buff->m_iRecvbytes = 0;
	BOOL bSuccess = PostQueuedCompletionStatus(m_hIO, 0, (ULONG_PTR)_buff, &(_buff->m_Overlapped));
	
	if ( !bSuccess && WSAGetLastError() != ERROR_IO_PENDING )
	{
		// error
	}
}

BOOL IOCPServer::SendData()
{
	IOBuffer* Buffer;
	Player*   play;
	Buffer = m_pNextBufferList;
	while(Buffer->m_pNext != NULL)
	{
		play = m_pPlayerList;
		for(int i=0; i<m_iClientCount; i++)
		{
			if(Buffer->m_Id == play->m_Id || play->m_pNext == NULL) continue;
			this->SendPacket(Buffer, &play->m_PI);
		}
		Buffer = Buffer->m_pNext;
	}
	return TRUE;
}

void IOCPServer::SendPacket(IOBuffer* _buffer, void *_packet)
{
	int packet_size = reinterpret_cast<unsigned char *>(_packet)[0];

	_buffer->m_Wsabuf.buf = _buffer->m_SendBuf;
	_buffer->m_Wsabuf.len = packet_size;

	unsigned long io_size;

	memcpy(_buffer->m_SendBuf, _packet, packet_size);

	WSASend(_buffer->m_ClientSock, &_buffer->m_Wsabuf, 1, &io_size, NULL, &_buffer->m_Overlapped, NULL);
}