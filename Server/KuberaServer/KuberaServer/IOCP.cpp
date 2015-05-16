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
	m_MinionTimer			= 0.0f;

	Check8 = 0;
	Check3000 = 0;
	Check0 = 0;

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

	Player* playe;
	while(m_pPlayerList != NULL)
	{
		playe = m_pPlayerList;
		m_pPlayerList = playe->m_pNext;
		delete playe;
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
	SOCKADDR_IN sock_addr;
	SOCKADDR_IN sock_addr2;
	char* p;
	char* q;
	int soklen;
	int soklen2;
	BOOL SameIP = FALSE;
	
	while (1)
	{
		if(pThis->m_bServerShutDown) break;
		SameIP = FALSE;
		ZeroMemory(&pThis->m_ClinetAddr, sizeof(SOCKADDR));
		int addr_len = sizeof(pThis->m_ClinetAddr);

		SOCKET client_sock = WSAAccept(pThis->m_ListenSock, (sockaddr *)&pThis->m_ClinetAddr, &addr_len, NULL, NULL);
		if (client_sock == INVALID_SOCKET) {
			exit(-1);
		}
		
		
		printf("IP 주소= %s, 포트번호= %d , Count = %d\n", 
			inet_ntoa(pThis->m_ClinetAddr.sin_addr), 
			ntohs(pThis->m_ClinetAddr.sin_port), pThis->m_iClientCount); 

		//IP체크후 동기화
		IOBuffer* buffer;
		buffer = pThis->m_pNextBufferList;
		while(buffer != NULL)
		{
			soklen = sizeof(sock_addr);
			getpeername(buffer->m_ClientSock,(struct sockaddr*)&sock_addr,&soklen);
			p = (char*)inet_ntoa((struct in_addr)sock_addr.sin_addr);
			char IP[16];
			strcpy(IP, p);
			q = (char*)inet_ntoa(pThis->m_ClinetAddr.sin_addr);


			if(!strcmp(IP, q))
			{
				buffer->m_ClientSock = client_sock;
				buffer->m_Opcode			= OP_INIT;
				buffer->m_Connect		= TRUE;
				buffer->m_pPlayer->m_Connect = TRUE;
				buffer->m_iSendbytes = 0;
				buffer->m_iSendbytesCount = 0;
				int header = INITCLIENT;
				send(client_sock, (char*)&header, sizeof(int), 0);
				send(client_sock, (char*)&buffer->m_Id, sizeof(int), 0);
				send(client_sock, (char*)&buffer->m_pPlayer->m_PI->PI.m_Pos, sizeof(Vector3), 0);
				SameIP = TRUE;
				CreateIoCompletionPort((HANDLE)client_sock, pThis->m_hIO, (DWORD)buffer, 0);
				PostQueuedCompletionStatus(pThis->m_hIO, 0, (ULONG_PTR)buffer, &buffer->m_Overlapped);
				break;
			}
			buffer = buffer->m_pNext;
		}
		//////////////////////////////////////

		if(SameIP) continue;
		pThis->m_iClientCount++;
		//버퍼 생성
		buffer = new IOBuffer;
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
			buffer->m_iSendbytes = 0;
			buffer->m_iSendbytesCount = 0;
			buffer->m_Wsabuf.buf		= buffer->m_RecvBuf;
			buffer->m_Wsabuf.len		= BUFSIZE;
			buffer->m_SendWsabuf.buf	= buffer->m_SendBuf;
			buffer->m_SendWsabuf.len	= BUFSIZE;
			buffer->m_ClientSock		= client_sock;
			buffer->m_Opcode			= OP_INIT;
			buffer->m_Connect		= TRUE;

			int header = INITCLIENT;
			send(client_sock, (char*)&header, sizeof(int), 0);
			send(client_sock, (char*)&buffer->m_Id, sizeof(int), 0);
			Vector3 po;
			if(buffer->m_Id % 2 == 0)
			{
				
				po.x = 550;
				po.y = 0;
				po.z = 0;
				send(client_sock, (char*)&po, sizeof(Vector3), 0);
			}
			else
			{
				po.x = -550;
				po.y = 0;
				po.z = 0;
				send(client_sock, (char*)&po, sizeof(Vector3), 0);
			}

			Player* pl = new Player;
			ZeroMemory(pl, sizeof(Player));
			pl->m_Id = buffer->m_Id;
			pl->m_PI = new PlayerPacket;
			ZeroMemory(pl->m_PI, sizeof(PlayerPacket));
			pl->m_PI->PI.m_Pos = po;
			pl->m_PI->PI.m_ID = buffer->m_Id;
			pl->m_PI->size = 32;
			pl->m_pNext = pThis->m_pPlayerList;
			pl->m_Connect = TRUE;
			
			pThis->m_pPlayerList = pl;
			buffer->m_pPlayer = pl;

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

		if(buff->m_Opcode != OP_RECV_DONE && dwSize == 32 )
		{
			//server->SetOpCode(buff, OP_RECV_DONE);
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
		case OP_SEND:
			server->OnSend(buff, dwSize);
			break;
		case OP_SEND_FINISH:
			server->OnSendFinish(buff, dwSize);
			break;
		case OP_DISCONNECT:
			buff->m_ReconnectCount++;
			if(buff->m_ReconnectCount < 40) server->OnInit(buff);
			else buff->m_ReconnectCount = 0;
			
			break;
		}
	}
	return 0;
}

void IOCPServer::SetOpCode(IOBuffer* _buff, OPCODE _opCode)
{
//	if(!_buff->m_Connect) return;
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
	if(_size == 0)
	{
		_buff->m_Connect = FALSE;
		_buff->m_pPlayer->m_Connect = FALSE;
		printf("어디냐1\n");
	}
	else if(_size != 0 && _buff->m_Connect == FALSE)
	{
		_buff->m_Connect = TRUE;
		_buff->m_pPlayer->m_Connect = TRUE;
		printf("어디냐2\n");
	}

	DWORD dwRecv = 0, dwFlags = 0;
	WSABUF buffRecv;
	printf("RECV: %d, %d \n", _buff->m_Id, _buff->m_Opcode);

	this->SetOpCode(_buff, OP_RECV_DONE);

	buffRecv.buf = _recvBuff;
	buffRecv.len = _size;

	_buff->m_MinionCount++;
	int retval = WSARecv(_buff->m_ClientSock, &buffRecv, 1, &dwRecv, &dwFlags, &(_buff->m_Overlapped), NULL);


	if ( retval == SOCKET_ERROR && (ERROR_IO_PENDING != WSAGetLastError())) 
	{
		
	}
}

void IOCPServer::OnRecvFinish(IOBuffer* _buff, DWORD _size)
{
	if(_size == 0)
	{
		_buff->m_Connect = FALSE;
		_buff->m_pPlayer->m_Connect = FALSE;
 		SetOpCode(_buff, OP_DISCONNECT);
 		BOOL bSuccess = PostQueuedCompletionStatus(m_hIO, 0, (ULONG_PTR)_buff, &(_buff->m_Overlapped));
		printf("어디냐3\n");
		return;
	}
	else if(_size != 0 && _buff->m_Connect == FALSE)
	{
		_buff->m_Connect = TRUE;
		_buff->m_pPlayer->m_Connect = TRUE;
		printf("어디냐4\n");
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
	
	printf("RECVDONE: %d, %d \n", _buff->m_Id, _buff->m_Opcode);
	SetOpCode(_buff, OP_SEND);
	//if(_buff->m_Disconnect) return;
	BOOL bSuccess = PostQueuedCompletionStatus(m_hIO, 0, (ULONG_PTR)_buff, &(_buff->m_Overlapped));
	_buff->m_iRecvbytes = 0;

	if ( !bSuccess && WSAGetLastError() != ERROR_IO_PENDING )
	{

	}
}

void IOCPServer::OnSend(IOBuffer* _buff, DWORD _size)
{
	//if(_buff->m_iSendbytesCount != 0) return;
	IOBuffer* Buffer;
	Player*   play;
	int Number = 0;
	
	int* Count = new int;
	*Count = m_iClientCount;


	SetOpCode(_buff, OP_SEND_FINISH);	
	this->SendPacket(_buff, HEROCOUNT, Count, sizeof(int));
	printf("1: %d, %d \n", _buff->m_Id, _buff->m_Opcode);

	play = m_pPlayerList;

	while(play != NULL)
	{
		if( play->m_Id == _buff->m_Id)
		{
			play = play->m_pNext;
			continue;
		}

		if(_buff->m_Connect)
		{
			//printf("ID : %d, x: %3f, y: %3f, z : %3f, size : %d \n", play->m_PI->PI.m_ID, play->m_PI->PI.m_Pos.x, play->m_PI->PI.m_Pos.y, play->m_PI->PI.m_Pos.z, play->m_PI->size);
			this->SetOpCode(_buff, OP_SEND_FINISH);
			this->SendPacket(_buff, HERODATA, play->m_PI, sizeof(PlayerPacket));
			printf("2: %d, %d \n", _buff->m_Id, _buff->m_Opcode);
		}
		play = play->m_pNext;
	}

	if(_buff->m_MinionCount > 2)
	{
		_buff->m_MinionCount = 0;
		SetOpCode(_buff, OP_SEND_FINISH);
		this->SendPacket(_buff, MINIONDATA, Arrange.MI, sizeof(MinionInfo)*160);
		printf("3: %d, %d \n", _buff->m_Id, _buff->m_Opcode);
		
	}

	BOOL bSuccess = PostQueuedCompletionStatus(m_hIO, 0, (ULONG_PTR)_buff, &(_buff->m_Overlapped));
	_buff->m_bSendFinish = TRUE;
	
}

void IOCPServer::OnSendFinish(IOBuffer* _buff, DWORD _size)
{
//  	if(_size == 0)
//  	{
//  		_buff->m_Connect = FALSE;
//  		_buff->m_pPlayer->m_Connect = FALSE;
// 		printf("어디냐5\n");
//  	}
//  	else if(_size != 0 && _buff->m_Connect == FALSE)
//  	{
//  		_buff->m_Connect = TRUE;
//  		_buff->m_pPlayer->m_Connect = TRUE;
// 		printf("어디냐6\n");
//  	}
// 
// 	if(_size < 8)
// 	{
// 		Check0++;
// 	}
// 	if(_size == 8)
// 	{
// 		Check8++;
// 	}
// 
// 	if(_size > 8 )
// 	{
// 		Check3000++;
// 	}

	
// 	SetOpCode(_buff, OP_RECV);
// 	_buff->m_iRecvbytes = 0;
// 	BOOL bSuccess = PostQueuedCompletionStatus(m_hIO, 0, (ULONG_PTR)_buff, &(_buff->m_Overlapped));
// 	
// 	if ( !bSuccess && WSAGetLastError() != ERROR_IO_PENDING )
// 	{
// 		// error
// 	}
// 	int retval;

 	_buff->m_iSendbytes += _size;
	//printf("%d \n", _size);
	printf("%d, %d \n", _buff->m_iSendbytes, _buff->m_iSendbytesCount);
	if(_size == 32)
	{
		int d;
		d = 0;

	}
 
 //	printf("1: %d,   2: %d \n", _buff->m_iSendbytes, _buff->m_iSendbytesCount);
 
	printf("4: %d, %d \n", _buff->m_Id, _buff->m_Opcode);
	if(_buff->m_iSendbytes != _buff->m_iSendbytesCount)
	{
		printf("%d, %d \n", _buff->m_iSendbytes, _buff->m_iSendbytesCount);
	}
 	//if(_buff->m_iSendbytes == _buff->m_iSendbytesCount || _buff->m_iSendbytes > 3400)
	if(_buff->m_bSendFinish)
 	{
 		_buff->m_iSendbytes = 0;
 		_buff->m_iSendbytesCount = 0;

		_buff->m_bSendFinish = FALSE;
 
 		SetOpCode(_buff, OP_RECV);
 		_buff->m_iRecvbytes = 0;
 		BOOL bSuccess = PostQueuedCompletionStatus(m_hIO, 0, (ULONG_PTR)_buff, &(_buff->m_Overlapped));
 		
 		if ( !bSuccess && WSAGetLastError() != ERROR_IO_PENDING )
 		{
 			// error
 		}
 	}

}

BOOL IOCPServer::SendData(float _dt)
{
	IOBuffer* Buffer;
	Player*   play;
	Buffer = m_pNextBufferList;
	int Number = 0;

	while(Buffer != NULL)
	{
		//EnterCriticalSection(&m_BufferListLock);
		if(!Buffer->m_Connect)
		{
			Buffer = Buffer->m_pNext;
			continue;
		}
		int* Count = new int;
		*Count = m_iClientCount;
		this->SetOpCode(Buffer, OP_SEND_FINISH);
		this->SendPacket(Buffer, HEROCOUNT, Count, sizeof(int));
		

		play = m_pPlayerList;
		while(play != NULL)
		{
			if( play->m_Id == Buffer->m_Id || !play->m_Connect)
			{
				play = play->m_pNext;
				continue;
			}
			
			//printf("ID : %d, x: %3f, y: %3f, z : %3f, size : %d \n", play->m_PI->PI.m_ID, play->m_PI->PI.m_Pos.x, play->m_PI->PI.m_Pos.y, play->m_PI->PI.m_Pos.z, play->m_PI->size);
			this->SetOpCode(Buffer, OP_SEND_FINISH);
			this->SendPacket(Buffer, HERODATA, play->m_PI, sizeof(PlayerPacket));
			
			play = play->m_pNext;
		}

		Buffer = Buffer->m_pNext;
		//LeaveCriticalSection(&m_BufferListLock);
	}

	m_MinionTimer += _dt;

	if(m_MinionTimer > 0.05f)
	{
		Buffer = m_pNextBufferList;

		while(Buffer != NULL)
		{
			if(!Buffer->m_Connect)
			{
				Buffer = Buffer->m_pNext;
				continue;
			}
			this->SetOpCode(Buffer, OP_SEND_FINISH);
			this->SendPacket(Buffer, MINIONDATA, Arrange.MI, sizeof(MinionInfo)*160);

			Buffer = Buffer->m_pNext;
		}
		
		//LeaveCriticalSection(&m_BufferListLock);
		m_MinionTimer = 0.0f;
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
//	_buffer->m_iSendbytes = Size;

	unsigned long io_size;

	memcpy(_buffer->m_SendBuf, Buffer, Size);
	
	_buffer->m_iSendbytesCount += Size;
	int retval = WSASend(_buffer->m_ClientSock, &_buffer->m_SendWsabuf, 1, &io_size, NULL, &_buffer->m_Overlapped, NULL);
// 
// 	if(retval > 800)
// 	{
// 		int ddfaf;
// 		ddfaf = 9;
// 	}
}

void IOCPServer::ArrangeDataInfo(float _dt)
{
	if(m_iClientCount < 2) return;

	Arrange.SetTime(_dt);
	Arrange.RegenMinion();
	Arrange.SetMinionPosition(_dt);

	Arrange.CheckMinionLive();
}