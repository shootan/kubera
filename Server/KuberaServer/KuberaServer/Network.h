#include "stdafx.h"

#define BUFSIZE 1024*2

struct IOBuffer{
	SOCKET m_ClientSock;
	OVERLAPPED m_Overlapped; 
	char m_Buf[BUFSIZE+1]; 
	int recvbytes; 
	int sendbytes; 
	WSABUF wsabuf;
	IOBuffer *m_pNext;
};

class Network 
{
private:
	SOCKET m_ListenSock;
	IOBuffer* m_pNextBufferList;

	
public:
	Network();
	~Network();

	BOOL StartServer(int _port, int);
	void ShutDown();

private: 
	DWORD WINAPI ListenThread(LPVOID arg);
	DWORD WINAPI WorkerThread(LPVOID arg);

}
