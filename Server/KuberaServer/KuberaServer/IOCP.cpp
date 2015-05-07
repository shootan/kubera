#define _CRT_SERCURE_NO_WARNINGS
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
			if(pThis->m_pNextBufferList != NULL)
				pThis->m_pNextBufferList->m_pPrev = buffer;
			pThis->m_pNextBufferList	= buffer;
			buffer->m_iRecvbytes		= 0;
			buffer->m_iSendbytes		= 0;
			buffer->m_Wsabuf.buf		= buffer->m_RecvBuf;
			buffer->m_Wsabuf.len		= BUFSIZE;
			buffer->m_SendWsabuf.buf	= buffer->m_SendBuf;
			buffer->m_SendWsabuf.len	= BUFSIZE;
			buffer->m_ClientSock		= client_sock;
			buffer->m_Opcode			= OP_INIT;
			buffer->m_Disconnect		= FALSE;

			int header = INITCLIENT;
			send(client_sock, (char*)&header, sizeof(int), 0);
			send(client_sock, (char*)&buffer->m_Id, sizeof(int), 0);

			Player* pl = new Player;
			ZeroMemory(pl, sizeof(Player));
			pl->m_Id = buffer->m_Id;
			
			pl->m_PI = new PlayerPacket;
			ZeroMemory(pl->m_PI, sizeof(PlayerPacket));
			pl->m_PI->PI.m_ID = buffer->m_Id;
			pl->m_PI->size = 32;
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
		if(m_ID[i] == NULL)
		{
			m_ID[i] = 'a';
			return i;
		}
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
	BOOL Check  = TRUE;

	while(!server->m_bServerShutDown)
	{
		Check = GetQueuedCompletionStatus(server->m_hIO, &dwSize, (PULONG_PTR)&buff, (LPOVERLAPPED*)&over, INFINITE);

		if(buff->m_Opcode == OP_RECV_DONE)
		{
			if(dwSize == 0 && Check != TRUE)
			{
				/*if(buff == server->m_pNextBufferList) 
					server->m_pNextBufferList = buff->m_pNext;
				else
				{
					buff->m_pPrev->m_pNext = buff->m_pNext;
					if(buff->m_pNext != NULL)
						buff->m_pNext->m_pPrev = buff->m_pPrev;
				}

				server->m_iClientCount--;
				buff->m_Disconnect = TRUE;
				closesocket(buff->m_ClientSock);
				delete buff;
				continue;*/
			}
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
			server->OnSendFinish(buff, dwSize);
			break;
		}
	}
	return 0;
}

void IOCPServer::SetOpCode(IOBuffer* _buff, OPCODE _opCode)
{
	if(_buff->m_Disconnect) return;
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

	if(_buff->m_Disconnect) return;
	int retval = WSARecv(_buff->m_ClientSock, &buffRecv, 1, &dwRecv, &dwFlags, &(_buff->m_Overlapped), NULL);

	if ( retval == SOCKET_ERROR && (ERROR_IO_PENDING != WSAGetLastError())) 
	{
		
	}
}

void IOCPServer::OnRecvFinish(IOBuffer* _buff, DWORD _size)
{
	if ( _size == 0 )
	{
		/*m_iClientCount--;
		delete _buff;
		return;*/
	}

	Player* play;
	play = m_pPlayerList;

	while(play != NULL)
	{
		if(_buff->m_Id == play->m_Id)
		{
			play->m_PI = (PlayerPacket*)_buff->m_RecvBuf;
			break;
		}
		play = play->m_pNext;
	}
	
	SetOpCode(_buff, OP_RECV);
	if(_buff->m_Disconnect) return;
	BOOL bSuccess = PostQueuedCompletionStatus(m_hIO, 0, (ULONG_PTR)_buff, &(_buff->m_Overlapped));
	_buff->m_iRecvbytes = 0;

	if ( !bSuccess && WSAGetLastError() != ERROR_IO_PENDING )
	{

	}
}

void IOCPServer::OnSendFinish(IOBuffer* _buff, DWORD _size)
{
	//if ( _size == 0 )
	//{
	//	//check
	//}

	//SetOpCode(_buff, OP_RECV);
	//_buff->m_iRecvbytes = 0;
	//BOOL bSuccess = PostQueuedCompletionStatus(m_hIO, 0, (ULONG_PTR)_buff, &(_buff->m_Overlapped));
	//
	//if ( !bSuccess && WSAGetLastError() != ERROR_IO_PENDING )
	//{
	//	// error
	//}
	int retval;

	_buff->m_iSendbytes -= _size;

	
	DWORD dwRecv = 0, dwFlags = 0;
	WSABUF buffRecv;

	this->SetOpCode(_buff, OP_RECV_DONE);

	buffRecv.buf = _buff->m_RecvBuf;
	buffRecv.len = BUFSIZE;

	retval = WSARecv(_buff->m_ClientSock, &buffRecv, 1, &dwRecv, &dwFlags, &(_buff->m_Overlapped), NULL);
	
	//if ( retval == SOCKET_ERROR && (ERROR_IO_PENDING != WSAGetLastError())) 
	//{
	//	// error
	//}
}

BOOL IOCPServer::SendData()
{
	IOBuffer* Buffer;
	Player*   play;
	Buffer = m_pNextBufferList;
	int Number = 0;

	while(Buffer != NULL)
	{
		//EnterCriticalSection(&m_BufferListLock);

		int* Count = new int;
		*Count = m_iClientCount;
		this->SendPacket(Buffer, HEROCOUNT, Count, sizeof(int));
		this->SetOpCode(Buffer, OP_SEND_FINISH);

		play = m_pPlayerList;
		while(play != NULL)
		{
			if( play->m_Id == Buffer->m_Id)
			{
				play = play->m_pNext;
				continue;
			}
			
			if(!Buffer->m_Disconnect)
			{
				//printf("ID : %d, x: %3f, y: %3f, z : %3f, size : %d \n", play->m_PI->PI.m_ID, play->m_PI->PI.m_Pos.x, play->m_PI->PI.m_Pos.y, play->m_PI->PI.m_Pos.z, play->m_PI->size);
				this->SendPacket(Buffer, HERODATA, play->m_PI, sizeof(PlayerPacket));
				this->SetOpCode(Buffer, OP_SEND_FINISH);

				
// 				Number = 1;
// 				int size = sizeof(int) + sizeof(MinionInfo)*40;
// 				char* buffer = new char[size];
// 				*(int*)buffer = Number;
// 				memcpy(buffer+sizeof(int), Arrange.MI1, size);
				this->SendPacket(Buffer, MINIONDATA, Arrange.MI1, sizeof(MinionInfo)*40);
				this->SetOpCode(Buffer, OP_SEND_FINISH);

// 				Number = 2;
// 				*(int*)buffer = Number;
// 				memcpy(buffer+sizeof(int), Arrange.MI1, size);
				this->SendPacket(Buffer, MINIONDATA, Arrange.MI2, sizeof(MinionInfo)*40);
				this->SetOpCode(Buffer, OP_SEND_FINISH);

// 				Number = 3;
// 				*(int*)buffer = Number;
// 				memcpy(buffer+sizeof(int), Arrange.MI1, size);
				this->SendPacket(Buffer, MINIONDATA, Arrange.MI3, sizeof(MinionInfo)*40);
				this->SetOpCode(Buffer, OP_SEND_FINISH);

// 				Number = 4;
// 				*(int*)buffer = Number;
// 				memcpy(buffer+sizeof(int), Arrange.MI1, size);
				this->SendPacket(Buffer, MINIONDATA, Arrange.MI4, sizeof(MinionInfo)*40);
				this->SetOpCode(Buffer, OP_SEND_FINISH);
			}
			play = play->m_pNext;
		}

		Buffer = Buffer->m_pNext;
		//LeaveCriticalSection(&m_BufferListLock);
	}
	return TRUE;
}

void IOCPServer::SendPacket(IOBuffer* _buffer, int NetworkCode, void *_packet, int _size)
{
	int adq = reinterpret_cast<unsigned char *>(_packet)[0];

	int Size = HEADERSIZE + _size;
	char* Buffer = new char[Size];
	*(int*)Buffer = NetworkCode;
	
	memcpy(Buffer+HEADERSIZE, _packet, Size);

	_buffer->m_SendWsabuf.buf = _buffer->m_SendBuf;
	_buffer->m_SendWsabuf.len = Size;
	_buffer->m_iSendbytes = Size;

	unsigned long io_size;

	memcpy(_buffer->m_SendBuf, Buffer, Size);
	
	if(_buffer->m_Disconnect) return;
	WSASend(_buffer->m_ClientSock, &_buffer->m_SendWsabuf, 1, &io_size, NULL, &_buffer->m_Overlapped, NULL);
}

void IOCPServer::ArrangeDataInfo(float _dt)
{
	if(m_iClientCount < 1) return;

	Arrange.SetTime(_dt);
	Arrange.RegenMinion();
	Arrange.SetMinionPosition(_dt);
}