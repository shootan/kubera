#pragma once

#include "Network.h"
#include "ArrangeData.h"




class IOCPServer : public Network
{
public:
	int Check8;
	int Check3000;
	int Check0;
	int save1;
	int save2;
	ArrangeData Arrange;

	//����Ʈ�� �������� ������
	IOBuffer*	m_pNextBufferList;
	Player*		m_pPlayerList;

	//�ڵ�
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

	float	m_MinionTimer;
	
	//�Լ�
	IOCPServer();
	~IOCPServer();

	BOOL StartServer(int port);
	BOOL InitThread();
	void CreateBuffer(IOBuffer* buffer);

	int GetNewId();

	void ArrangeDataInfo(float _dt);

	//OpCode
	void OnInit(IOBuffer* _buff);
	void OnRecv(IOBuffer* _buff, char* _recvBuff, int _size);
	void OnRecvFinish(IOBuffer* _buff, DWORD _size);
	void OnSend(IOBuffer* _buff, DWORD _size);
	void OnSendFinish(IOBuffer* _buff, DWORD _size);

	void SetOpCode(IOBuffer* _buff, OPCODE _opCode);

	// Send
	BOOL SendData(float _dt);
	void SendPacket(IOBuffer* _buffer, int NetworkCode, void *_packet, int _size);

	//������
	static UINT WINAPI ListenThread(LPVOID arg);
	static UINT WINAPI WorkerThread(LPVOID arg);
};