#pragma once
#include "Network.h"



struct IOBuffer{
	SOCKET m_ClientSock;
	OVERLAPPED m_Overlapped; 
	char m_Buf[BUFSIZE+1]; 
	int recvbytes; 
	int sendbytes; 
	WSABUF wsabuf;
	IOBuffer *m_pNext;
};

class IOCPServer : public Network
{
private: 
	//스레드
	static DWORD WINAPI ListenThread(LPVOID arg);
	static DWORD WINAPI WorkerThread(LPVOID arg);

	public:
	//버퍼를 리스트로 관리해줄 포인터
	IOBuffer* m_pNextBufferList;

	//핸들
	HANDLE m_hIO;
	HANDLE m_hListenThread;
	HANDLE m_hWorkerThread;

	//
	CRITICAL_SECTION m_BufferListLock;
	SOCKADDR_IN m_ClinetAddr;

	//
	BOOL m_bServerStart;
	BOOL m_bServerShutDown;
	

	

public:
	IOCPServer();
	~IOCPServer();

	BOOL StartServer(int port);
	BOOL InitThread();
	void CreateBuffer(IOBuffer* buffer);



};