#pragma once
#include "Network.h"


typedef enum OPCODE
{
	OP_INIT,
	OP_RECV,
	OP_RECV_DONE,
	OP_SEND_FINISH
} OPCODE;

struct IOBuffer{
	SOCKET		m_ClientSock;
	int			m_Id;
	OVERLAPPED	m_Overlapped;
	char		m_RecvBuf[BUFSIZE]; 
	char		m_SendBuf[BUFSIZE]; 
	int			m_iRecvbytes;
	int			m_iSendbytes;
	WSABUF		m_Wsabuf;
	OPCODE		m_Opcode;
	PlayerInfo	m_PlayerData;
	BOOL		m_Use;
	IOBuffer*	m_pNext;
};

struct Player{
	int m_Id;
	PlayerPacket	m_PI;
	Player*			m_pNext;
};

class IOCPServer : public Network
{
public:
	//리스트로 관리해줄 포인터
	IOBuffer*	m_pNextBufferList;
	Player*		m_pPlayerList;

	//핸들
	HANDLE m_hIO;
	HANDLE m_hListenThread;
	HANDLE m_hWorkerThread;

	//
	CRITICAL_SECTION m_BufferListLock;
	SOCKADDR_IN m_ClinetAddr;

	//
	BOOL	m_bServerStart;
	BOOL	m_bServerShutDown;

	int		m_iClientCount;

	char	m_ID[100];
	
	//함수
	IOCPServer();
	~IOCPServer();

	BOOL StartServer(int port);
	BOOL InitThread();
	void CreateBuffer(IOBuffer* buffer);

	int GetNewId();

	//OpCode
	void OnInit(IOBuffer* _buff);
	void OnRecv(IOBuffer* _buff, char* _recvBuff, int _size);
	void OnRecvFinish(IOBuffer* _buff, DWORD _size);
	void OnSendFinish(IOBuffer* _buff, DWORD _size);

	void SetOpCode(IOBuffer* _buff, OPCODE _opCode);

	// Send
	BOOL SendData();
	void SendPacket(IOBuffer* _buffer, void *_packet);

	//스레드
	static UINT WINAPI ListenThread(LPVOID arg);
	static UINT WINAPI WorkerThread(LPVOID arg);
};