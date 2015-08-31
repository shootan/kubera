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
	save1 = 0;
	save2 = 0;
	DummyCount = 0;
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

	// (CPU 개수* 1)개의작업자스레드생성 
	for(int i=0; i < (int)si.dwNumberOfProcessors*1; ++i) 
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
	char* dummy = "127.0.0.1";
	
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
		q = (char*)inet_ntoa(pThis->m_ClinetAddr.sin_addr);
// 		if(!strcmp(dummy, q))
// 		{
// 			pThis->DummyCount++;
// 			printf("Dummy Client Check %d", pThis->DummyCount);
// 			continue;
// 		}		

		IOBuffer* buffer;

		//IP체크후 동기화
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
				if(buffer->m_bGameStart)
				{
					int header = INITCLIENT;
					send(client_sock, (char*)&header, sizeof(int), 0);
					send(client_sock, (char*)&buffer->m_pPlayer->m_PI, sizeof(PlayerPacket), 0);
					SameIP = TRUE;
				}
				CreateIoCompletionPort((HANDLE)client_sock, pThis->m_hIO, (DWORD)buffer, 0);
				PostQueuedCompletionStatus(pThis->m_hIO, 0, (ULONG_PTR)buffer, &buffer->m_Overlapped);
				//pThis->m_iClientCount--;
				break;
			}
			buffer = buffer->m_pNext;
		}
		//////////////////////////////////////
		pThis->m_iClientCount++;
		if(SameIP)
		{
			printf("IP 주소= %s, 포트번호= %d , 재접속 \n", inet_ntoa(pThis->m_ClinetAddr.sin_addr), 
				ntohs(pThis->m_ClinetAddr.sin_port), pThis->m_iClientCount);
			SameIP = FALSE;
				continue;
		}
		
		printf("IP 주소= %s, 포트번호= %d , Count = %d\n", 
			inet_ntoa(pThis->m_ClinetAddr.sin_addr), 
			ntohs(pThis->m_ClinetAddr.sin_port), pThis->m_iClientCount); 
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

			Player* pl = new Player;
			ZeroMemory(pl, sizeof(Player));
			pl->m_Id = buffer->m_Id;
			pl->m_Char = 1;
			pl->m_Team = buffer->m_Id;
			/*pl->m_PI = new PlayerPacket;*/
			ZeroMemory(&pl->m_PI, sizeof(PlayerPacket));
			//pl->m_PI->PI.m_Pos = po;
			pl->m_PI.PI.m_ID = buffer->m_Id;
			pl->m_PI.size = 32;
			pl->m_pNext = pThis->m_pPlayerList;
			pl->m_Connect = TRUE;
			
			pThis->m_pPlayerList = pl;
			buffer->m_pPlayer = pl;

			//비동기 입출력 시작
			DWORD flags = 0;
			CreateIoCompletionPort((HANDLE)client_sock, pThis->m_hIO, (ULONG_PTR)buffer, 0);

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
			server->OnRecv(buff, buff->m_RecvBuf, dwSize);
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
			if(buff->m_ReconnectCount < 100)
			{
					server->OnInit(buff);
					printf("*****\n");
			}
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
	IOBuffer* Buffer = m_pNextBufferList;
	send(_buff->m_ClientSock, (char*)&_buff->m_Id, sizeof(int), 0);

 	while (Buffer != NULL)
 	{
 		if( _buff->m_Id != Buffer->m_Id)
 		{
 			int Header = CLIENT_CONNECT;
 			int retval = send(_buff->m_ClientSock, (char*)&Header, sizeof(int), 0);
 			retval = send(_buff->m_ClientSock, (char*)&Buffer->m_Id, sizeof(int), 0);
 		}
 		Buffer = Buffer->m_pNext;
 	}
	this->SetOpCode(_buff, OP_RECV);
	_buff->m_iSendbytes = 0;
	_buff->m_iSendbytesCount = 0;
	_buff->m_iRecvbytes = 0;
	//printf("INIT ID : %d, %d, %d", _buff->m_Id, _buff->m_iSendbytes, _buff->m_iSendbytesCount);
	bSuccess = PostQueuedCompletionStatus(m_hIO, 0, (ULONG_PTR)_buff, &(_buff->m_Overlapped));

	if ( !bSuccess && WSAGetLastError() != ERROR_IO_PENDING )
	{
		// error
	}
}

void IOCPServer::OnRecv(IOBuffer* _buff, char* _recvBuff, int _size)
{
 	/*if(_size == 0)
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
 	}*/

	if(_size != 0)
	{
		int q;
		q = 0;
	}

	DWORD dwRecv = 0, dwFlags = 0;
	WSABUF buffRecv;
	//printf("RECV: %d, %d, SIZE : %d \n", _buff->m_Id, _buff->m_Opcode, _size);

	this->SetOpCode(_buff, OP_RECV_DONE);

	buffRecv.buf = _recvBuff;
	buffRecv.len = BUFSIZE;

	int retval = WSARecv(_buff->m_ClientSock, &buffRecv, 1, &dwRecv, &dwFlags, &(_buff->m_Overlapped), NULL);


	if ( retval == SOCKET_ERROR && (ERROR_IO_PENDING != WSAGetLastError())) 
	{
		
	}
}

void IOCPServer::OnRecvFinish(IOBuffer* _buff, DWORD _size)
{
	if(_size == 0)
	{
		//_buff->m_Connect = FALSE;
		//_buff->m_pPlayer->m_Connect = FALSE;
 		//SetOpCode(_buff, OP_DISCONNECT);
 		//BOOL bSuccess = PostQueuedCompletionStatus(m_hIO, 0, (ULONG_PTR)_buff, &(_buff->m_Overlapped));
	   // printf("??\n");
		//_buff->m_iSendbytes = 0;
		//_buff->m_iSendbytesCount= 0;
		m_iClientCount--;
		printf("%d 번 로그아웃 \n", _buff->m_Id);
		return;
	}
	else if(_size != 0 && _buff->m_Connect == FALSE)
	{
		//_buff->m_Connect = TRUE;
		//_buff->m_pPlayer->m_Connect = TRUE;
	//	printf("어디냐4\n");
	}
	int Header = 0;
	int p = 0;
	PlayerPacket pl;
	Player* play;

	memcpy(&Header, _buff->m_RecvBuf, sizeof(int));

	
	switch(Header)
	{
	case HERODATA:
		_buff->m_Header = Header;
		
		memcpy(&pl, _buff->m_RecvBuf+ HEADERSIZE, sizeof(PlayerPacket));

		
		play = m_pPlayerList;

		while(play != NULL)
		{
			if(_buff->m_Id == play->m_Id)
			{
				play->m_PI = pl;
				break;
			}
			play = play->m_pNext;
		}
		break;

	case CLIENT_CONNECT:
		_buff->m_Header = Header;

		break;

	case SELECT_TEAM_RED:
		_buff->m_Header = Header;
		int p;
		memcpy(&p, _buff->m_RecvBuf+ HEADERSIZE, sizeof(int));
		play = m_pPlayerList;

		while(play != NULL)
		{
			if(_buff->m_Id == play->m_Id)
			{
				play->m_PI.PI.m_ID = p;
				break;
			}
			play = play->m_pNext;
		}
		break;

	case SELECT_TEAM_BLUE:
		_buff->m_Header = Header;
	
		memcpy(&p, _buff->m_RecvBuf+ HEADERSIZE, sizeof(int));
		play = m_pPlayerList;

		while(play != NULL)
		{
			if(_buff->m_Id == play->m_Id)
			{
				play->m_PI.PI.m_ID = p;
				break;
			}
			play = play->m_pNext;
		}
		break;

	case SELECT_CHAR_WARIOR:
		_buff->m_Header = SELECT_CHAR;
		
		memcpy(&p, _buff->m_RecvBuf+ HEADERSIZE, sizeof(int));
		play = m_pPlayerList;
		printf("%d번, 워리어선택 \n", _buff->m_Id);

		_buff->m_pPlayer->m_PI.PI.m_Type = p;
		break;

		break;

	case SELECT_CHAR_WIZARD:
		_buff->m_Header = SELECT_CHAR;
		
		memcpy(&p, _buff->m_RecvBuf+ HEADERSIZE, sizeof(int));
		play = m_pPlayerList;
		printf("%d번, 위자드선택 \n", _buff->m_Id);
		while(play != NULL)
		{
			if(_buff->m_Id == play->m_Id)
			{
				_buff->m_pPlayer->m_PI.PI.m_Type = p;
				break;
			}
			play = play->m_pNext;
		}
		break;

	case READY_GAME:
		_buff->m_Header = Header;
		printf("게임 준비 \n");

		break;
	case ATTACK_INFO:
		printf("타격 \n");
		_buff->m_Header = Header;
		break;
	}
	
	
	//printf("RECVDONE: %d, %d \n", _buff->m_Id, _buff->m_Opcode);
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
	IOBuffer* Buffer;
	Player*   play;
	int Number = 0;
	int Header = 0;

	switch(_buff->m_Header)
	{
	case HERODATA:
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
				int Size = HEADERSIZE + sizeof(PlayerPacket);
				char* Buffer = new char[Size];
				*(int*)Buffer = HERODATA;
				memcpy(Buffer+HEADERSIZE, &play->m_PI, Size);
				int retval = send(_buff->m_ClientSock, Buffer, Size, 0);
			}
			play = play->m_pNext;
		}
		break;

	case CLIENT_CONNECT:
		Buffer = m_pNextBufferList;
		while (Buffer != NULL)
		{
			if( _buff->m_Id != Buffer->m_Id)
			{
				int Header = CLIENT_CONNECT;
				int retval = send(Buffer->m_ClientSock, (char*)&Header, sizeof(int), 0);
				retval = send(Buffer->m_ClientSock, (char*)&_buff->m_Id, sizeof(int), 0);
			}
			Buffer = Buffer->m_pNext;
		}

		break;

	case SELECT_TEAM:
		Buffer = m_pNextBufferList;
		while (Buffer != NULL)
		{
			if( _buff->m_Id != Buffer->m_Id)
			{
				int Size = HEADERSIZE + sizeof(int);
				char* buff = new char[Size];
				*(int*)buff = SELECT_TEAM;
				memcpy(buff+HEADERSIZE, &_buff->m_pPlayer->m_PI.PI.m_ID, sizeof(int));
				int retval = send(Buffer->m_ClientSock, buff, Size, 0);
				
			}
			Buffer = Buffer->m_pNext;
		}
		break;

	case SELECT_CHAR:
		Buffer = m_pNextBufferList;
		while (Buffer != NULL)
		{
			if( _buff->m_Id != Buffer->m_Id)
			{
				int Size = HEADERSIZE + sizeof(int);
				char* buff = new char[Size];
				*(int*)buff = SELECT_CHAR;
				memcpy(buff+HEADERSIZE, &_buff->m_pPlayer->m_PI.PI.m_Type, sizeof(int));
				int retval = send(Buffer->m_ClientSock, buff, Size, 0);

			}
			Buffer = Buffer->m_pNext;
		}
		break;

	case READY_GAME:

  		if(m_iClientCount > 1)
  		{
			Buffer = m_pNextBufferList;
			while (Buffer != NULL)
			{

				int Header = START_GAME;
				int retval = send(Buffer->m_ClientSock, (char*)&Header, sizeof(int), 0);
				Buffer->m_bGameStart = TRUE;

				Buffer = Buffer->m_pNext;
			}
		}
		break;

	case ATTACK_INFO:
		Buffer = m_pNextBufferList;
		while (Buffer != NULL)
		{
			AttackInfo ATI;
			memcpy(&ATI, _buff->m_RecvBuf+ HEADERSIZE, sizeof(AttackInfo));

			int Size = HEADERSIZE + sizeof(AttackInfo);
			char* buff = new char[Size];
			*(int*)buff = ATTACK_INFO;
			memcpy(buff+HEADERSIZE, &ATI, sizeof(AttackInfo));
			int retval = send(Buffer->m_ClientSock, buff, Size, 0);

			//int retval = send(Buffer->m_ClientSock, (char*)&ATI, sizeof(AttackInfo), 0);

			Buffer = Buffer->m_pNext;
		}
	}

	this->SetOpCode(_buff, OP_RECV);
	BOOL bSuccess = PostQueuedCompletionStatus(m_hIO, 0, (ULONG_PTR)_buff, &(_buff->m_Overlapped));
	_buff->m_bSendFinish = TRUE;
	
}

void IOCPServer::OnSendFinish(IOBuffer* _buff, DWORD _size)
{
	/*
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
  	if(_size ==0)
  	{		
		Check0++;
  	}
  	if(_size == 8)
  	{
  		Check8++;
  	}
 	
 // 
  	if(_size == 48 )
  	{
  		Check3000++;
  	}
 
 	if(_size != 0 && _size != 8 && _size != 48)
 	{
 		save1 =  _size;
 	}

	*////////////////////////////
 	SetOpCode(_buff, OP_RECV);
 	_buff->m_iRecvbytes = 0;
 	BOOL bSuccess = PostQueuedCompletionStatus(m_hIO, 0, (ULONG_PTR)_buff, &(_buff->m_Overlapped));
 	
 	if ( !bSuccess && WSAGetLastError() != ERROR_IO_PENDING )
 	{
 		// error
 	}
// 	int retval;

	/*//////////////////////
 	_buff->m_iSendbytes += _size;
	//printf("%d \n", _size);
	//printf("%d, %d \n", _buff->m_iSendbytes, _buff->m_iSendbytesCount);
// 	if(_size == 32)
// 	{
// 		int d;
// 		d = 0;
// 
// 	}
 
 	//printf("7: %d,   2: %d \n", _buff->m_iSendbytes, _buff->m_iSendbytesCount);
 
	//printf("4: %d, %d \n", _buff->m_Id, _buff->m_Opcode);
	if(_buff->m_iSendbytes != _buff->m_iSendbytesCount)
	{
		printf("안맞음: %d, %d, %d \n",_buff->m_Id,  _buff->m_iSendbytes, _buff->m_iSendbytesCount);
		//_buff->m_iSendbytes = 0;
		//_buff->m_iSendbytesCount = 0;
	}
	if(_buff->m_iSendbytes == 0)
	{
		return;
	}
 	//if(_buff->m_iSendbytes == _buff->m_iSendbytesCount || _buff->m_iSendbytes > 140)
	//if(_buff->m_iSendbytes == _buff->m_iSendbytesCount || _buff->m_iSendbytes > 140)
	if(_buff->m_iSendbytes == 56)
 	{
		//printf("어디냐 9 \n");
		printf("SUCCESS!!\n");
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
// 	else
// 	{
// 		BOOL bSuccess = PostQueuedCompletionStatus(m_hIO, 0, (ULONG_PTR)_buff, &(_buff->m_Overlapped));
// 
// 		if ( !bSuccess && WSAGetLastError() != ERROR_IO_PENDING )
// 		{
// 			// error
// 		}
// 	}
*/
}

BOOL IOCPServer::SendData(float _dt)
{
	Player*   play;

	play = m_pPlayerList;
// 	while(play != NULL)
// 	{
// 		
// 
// 		//printf("ID : %d, x: %.1f, y: %.1f, z : %.1f \n", play->m_PI->PI.m_ID, play->m_PI->PI.m_Pos.x, play->m_PI->PI.m_Pos.y, play->m_PI->PI.m_Pos.z);
// 		
// 
// 		play = play->m_pNext;
// 	}
// 	return TRUE;



	/////////
	IOBuffer* Buffer;
	
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
			this->SendPacket(Buffer, HERODATA, &play->m_PI, sizeof(PlayerPacket));
			
			play = play->m_pNext;
		}

		Buffer = Buffer->m_pNext;
		//LeaveCriticalSection(&m_BufferListLock);
	}

// 	m_MinionTimer += _dt;
// 
// 	if(m_MinionTimer > 0.05f)
// 	{
// 		Buffer = m_pNextBufferList;
// 
// 		while(Buffer != NULL)
// 		{
// 			if(!Buffer->m_Connect)
// 			{
// 				Buffer = Buffer->m_pNext;
// 				continue;
// 			}
// 			this->SetOpCode(Buffer, OP_SEND_FINISH);
// 			this->SendPacket(Buffer, MINIONDATA, Arrange.MI, sizeof(MinionInfo)*160);
// 
// 			Buffer = Buffer->m_pNext;
// 		}
// 		
// 		//LeaveCriticalSection(&m_BufferListLock);
// 		m_MinionTimer = 0.0f;
// 	}
	return TRUE;
}

void IOCPServer::SendPacket(IOBuffer* _buffer, int NetworkCode, void *_packet, int _size)
{
	int adq = reinterpret_cast<unsigned char *>(_packet)[0];
	int Size = HEADERSIZE + _size;
 	char* Buffer = new char[Size];
	*(int*)Buffer = NetworkCode;
	memcpy(Buffer+HEADERSIZE, _packet, Size);
	int retval = send(_buffer->m_ClientSock, Buffer, Size, 0);
	//_buffer->m_SendWsabuf.buf = _buffer->m_SendBuf;
	//_buffer->m_SendWsabuf.len = Size;
//	_buffer->m_iSendbytes = Size;

	//unsigned long io_size;

	//memcpy(_buffer->m_SendBuf, Buffer, Size);
	
	//_buffer->m_iSendbytesCount += Size;
	//printf("%d \n", Size);
	
// 
// 	if(retval > 800)
// 	{
// 		int ddfaf;
// 		ddfaf = 9;
// 	}
}

void IOCPServer::ArrangeDataInfo(ArrangeData* _ar)
{

}