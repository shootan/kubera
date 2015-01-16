#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define MAX_BUFFSIZE		4096

typedef enum OPCODE
{
	OP_INIT,
	OP_RECV,
	OP_RECV_DONE,
	OP_SEND,
	OP_SEND_DONE
} OPCODE;

typedef struct NETCONTEXT
{
	WSAOVERLAPPED m_ov;
	char          m_szSendBuff[MAX_BUFFSIZE];
	char          m_szRecvBuff[MAX_BUFFSIZE];
	WSABUF        m_wsaSendBuf;
	WSABUF        m_wsaRecvBuf;
	int           m_nExpectSumOfSent;
	int           m_nSumOfSent;
	int           m_nExpectSumOfRecv;
	int	          m_nSumOfRecv;
	OPCODE        m_opCode;
	SOCKET        m_Sock;
} NETCONTEXT, *PNETCONTEXT;

HANDLE g_hIOCP = NULL;
HANDLE g_hNetEvent = NULL;
SOCKET g_nSvrSock = INVALID_SOCKET;
int    g_nPort = 1111;
int    g_nShutdown = 0;

void CreateServer();
DWORD WINAPI Process(LPVOID arg);
void SetOpCode(NETCONTEXT *pCtx, OPCODE opCode);

int main(int argc, char* argv[])
{
	CreateServer();
	WSACleanup();
	return 0;
}

void CreateServer()
{
	int i = 0, nLen, nCPU = 0;
	WSADATA wsa;
	SOCKET nCliSock;
	SYSTEM_INFO si;
	SOCKADDR_IN saServer;
	WSANETWORKEVENTS events;

	WSAStartup(MAKEWORD(2,2), &wsa);
	//int retval = WSAGetLastError();
	g_hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	g_nSvrSock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	g_hNetEvent = WSACreateEvent();

	WSAEventSelect(g_nSvrSock, g_hNetEvent, FD_ACCEPT);

	saServer.sin_port = htons(g_nPort);
	saServer.sin_family = AF_INET;
	saServer.sin_addr.s_addr = INADDR_ANY;

	bind(g_nSvrSock, (LPSOCKADDR)&saServer, sizeof(struct sockaddr));
	listen(g_nSvrSock, 5);
	printf("start server %d\n", g_nPort);
	GetSystemInfo(&si);
	nCPU = si.dwNumberOfProcessors * 2;
	//for ( i = 0; i < nCPU; i++ ) 
		CreateThread(NULL, 0, Process, NULL, THREAD_PRIORITY_NORMAL, NULL);

	nLen = sizeof(struct sockaddr_in);
	while ( !g_nShutdown )
	{
		if ( WSAWaitForMultipleEvents(1, &g_hNetEvent, FALSE, 100, FALSE) == WSA_WAIT_TIMEOUT )
			continue;

		if ( WSAEnumNetworkEvents(g_nSvrSock, g_hNetEvent, &events) == SOCKET_ERROR )
			break;

		if ( (events.lNetworkEvents & FD_ACCEPT) != FD_ACCEPT )
			continue;

		if ( events.iErrorCode[FD_ACCEPT_BIT] != 0 )
			break;

		nCliSock = WSAAccept(g_nSvrSock, NULL, &nLen, 0, 0);

		NETCONTEXT *pCtx = new NETCONTEXT;
		memset(pCtx, 0, sizeof(NETCONTEXT));
		SetOpCode(pCtx, OP_INIT);
		pCtx->m_Sock = nCliSock;
		pCtx->m_nExpectSumOfRecv = 10;
		pCtx->m_wsaRecvBuf.buf = pCtx->m_szRecvBuff;
		pCtx->m_wsaSendBuf.buf = pCtx->m_szSendBuff;
		CreateIoCompletionPort((HANDLE)nCliSock, g_hIOCP, (DWORD)pCtx, 0);
		PostQueuedCompletionStatus(g_hIOCP, 0, (ULONG_PTR)pCtx, &pCtx->m_ov);
	}
}

void SetOpCode(NETCONTEXT *pCtx, OPCODE opCode)
{
	ZeroMemory(&pCtx->m_ov, sizeof(OVERLAPPED));
	pCtx->m_opCode = opCode;
}

void OnInit(NETCONTEXT *pCtx)
{
	BOOL bSuccess;

	SetOpCode(pCtx, OP_RECV);
	bSuccess = PostQueuedCompletionStatus(g_hIOCP, 0, (ULONG_PTR)pCtx, &(pCtx->m_ov));

	if ( !bSuccess && WSAGetLastError() != ERROR_IO_PENDING )
	{
		// error
	}
}

void OnRecv(NETCONTEXT *pCtx, char *pszBuf, int nLen)
{
	DWORD dwRecv = 0, dwFlags = 0;
	WSABUF buffRecv;

	SetOpCode(pCtx, OP_RECV_DONE);

	buffRecv.buf = pszBuf;
	buffRecv.len = nLen;

	int nRet = WSARecv(pCtx->m_Sock, &buffRecv, 1, &dwRecv, &dwFlags, &(pCtx->m_ov), NULL);

	if ( nRet == SOCKET_ERROR && (ERROR_IO_PENDING != WSAGetLastError())) 
	{
		// 중요한 error
	}
}

void OnRecvDone(NETCONTEXT *pCtx, DWORD dwSize)
{
	if ( dwSize == 0 )
	{
		// 중요한 error
	}

	pCtx->m_nSumOfRecv += (int)dwSize;
	pCtx->m_szRecvBuff[pCtx->m_nSumOfRecv] = 0;

	if ( pCtx->m_nSumOfRecv < pCtx->m_nExpectSumOfRecv )
	{
		// read more packet ..
		char *pszBuf = pCtx->m_szRecvBuff + pCtx->m_nSumOfRecv;
		int nRemain = pCtx->m_nExpectSumOfRecv - pCtx->m_nSumOfRecv;
		OnRecv(pCtx, pszBuf, nRemain);
	}
	else
	{
		strcpy(pCtx->m_szSendBuff, pCtx->m_szRecvBuff);
		fprintf(stderr, "[RECV] %s\n", pCtx->m_szRecvBuff);
		SetOpCode(pCtx, OP_SEND);

		pCtx->m_nExpectSumOfSent = (int)pCtx->m_nSumOfRecv;
		pCtx->m_nSumOfSent = 0;
		pCtx->m_wsaSendBuf.buf = pCtx->m_szSendBuff;

		BOOL bSuccess = PostQueuedCompletionStatus(g_hIOCP, 0, (ULONG_PTR)pCtx, &(pCtx->m_ov));

		if ( !bSuccess && WSAGetLastError() != ERROR_IO_PENDING )
		{
			// 중요한 error
		}
	}
}

void OnSend(NETCONTEXT *pCtx, DWORD dwSize)
{
	int nRet = 0;
	DWORD dwSend = 0, dwFlags = 0;
	WSABUF buffSend;

	pCtx->m_nSumOfSent += (int)dwSize;

	if ( pCtx->m_nSumOfSent < pCtx->m_nExpectSumOfSent )
	{
		pCtx->m_opCode = OP_SEND;
		buffSend.buf = pCtx->m_szSendBuff + pCtx->m_nSumOfSent;
		buffSend.len = pCtx->m_nExpectSumOfSent - pCtx->m_nSumOfSent;

		nRet = WSASend (pCtx->m_Sock, &buffSend, 1, &dwSend, dwFlags, &(pCtx->m_ov), NULL);

		if (SOCKET_ERROR == nRet && (ERROR_IO_PENDING != WSAGetLastError())) 
		{
			// 중요한 error
		}
	}
	else
	{
		pCtx->m_opCode = OP_SEND_DONE;

		BOOL bSuccess = PostQueuedCompletionStatus(g_hIOCP,
			(DWORD)(pCtx->m_nExpectSumOfSent), 
			(ULONG_PTR)pCtx, 
			&(pCtx->m_ov));

		if ( !bSuccess && WSAGetLastError() != ERROR_IO_PENDING )
		{
			// error
		}
	}
}

void OnSendDone(NETCONTEXT *pCtx, DWORD dwSize)
{
	pCtx->m_wsaRecvBuf.buf = pCtx->m_szRecvBuff;
	pCtx->m_wsaRecvBuf.len = MAX_BUFFSIZE;
	pCtx->m_nSumOfRecv = 0;

	SetOpCode(pCtx, OP_RECV);

	BOOL bSuccess = PostQueuedCompletionStatus(g_hIOCP, 0, (ULONG_PTR)pCtx, &(pCtx->m_ov));

	if ( !bSuccess && WSAGetLastError() != ERROR_IO_PENDING )
	{
		// error
	}
}

DWORD WINAPI Process(LPVOID arg)
{
	BOOL bSuccess = FALSE;
	LPOVERLAPPED lpOV = NULL;
	NETCONTEXT *pCtx = NULL;
	DWORD dwSize;

	while ( !g_nShutdown ) 
	{
		GetQueuedCompletionStatus(g_hIOCP, &dwSize, (PULONG_PTR)&pCtx, &lpOV, INFINITE);

		switch ( pCtx->m_opCode )
		{
		case OP_INIT:
			OnInit(pCtx);
			break;
		case OP_RECV:
			OnRecv(pCtx, pCtx->m_wsaRecvBuf.buf, MAX_BUFFSIZE);
			break;
		case OP_RECV_DONE:
			OnRecvDone(pCtx, dwSize);
			break;
		case OP_SEND:
			OnSend(pCtx, dwSize);
			break;
		case OP_SEND_DONE:
			OnSendDone(pCtx, dwSize);
			break;
		default:
			break;
		}
	}

	return 0;
}