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
	//버퍼를 리스트로 관리해줄 포인터
	IOBuffer* m_pNextBufferList;

	//IO핸들
	HANDLE m_hIO;

public:
	BOOL StartServer(int port);
	BOOL InitThread();
	void CreateBuffer(IOBuffer* buffer);

private: 
	DWORD WINAPI ListenThread(LPVOID arg);
	DWORD WINAPI WorkerThread(LPVOID arg);

};