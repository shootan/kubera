#pragma once
#pragma comment(lib, "ws2_32")
#include "targetver.h"

#include <stdio.h>
#include <string.h>
#include <tchar.h>
#include <WinSock2.h>
#include <process.h>
#include <math.h>
#include <d3dx9math.h>

// BUFSIZE
#define BUFSIZE 1024*4
#define MAXPLAYER 30
#define HEADERSIZE sizeof(int)
struct Vector3
{
	float x, y, z;
};

typedef enum OPCODE
{
	OP_INIT,
	OP_RECV,
	OP_RECV_DONE,
	OP_SEND,
	OP_SEND_FINISH,
	OP_DISCONNECT
} OPCODE;

struct PlayerInfo{
	int		m_ID;
	int		m_iState;
	int		m_iTargetID;
	int		m_Type;
	Vector3 m_Pos;
	float	m_HP;
	float	m_Rot;
	float	m_Damage;
};


struct PlayerPacket
{
	int			size;
	PlayerInfo	PI;
};

struct Player{
	int m_Id;
	BOOL m_Connect;
	float	m_AttackTime;
	PlayerPacket*	m_PI;
	Player*			m_pNext;
};

struct ObjectInfo{
	int		m_Id;
	int		m_Hp;
	int		m_Damage;
	int		m_Level;
	Vector3 m_Pos;
};

struct IOBuffer{
	SOCKET		m_ClientSock;
	int			m_Id;
	OVERLAPPED	m_Overlapped;
	char		m_RecvBuf[BUFSIZE]; 
	char		m_SendBuf[BUFSIZE]; 
	int			m_iRecvbytes;
	int			m_iSendbytes;
	int			m_iSendbytesCount;
	int			m_MinionCount;
	int			m_ReconnectCount;
	BOOL		m_bSendFinish;
	WSABUF		m_Wsabuf;
	WSABUF		m_SendWsabuf;
	OPCODE		m_Opcode;
	BOOL		m_Connect;
	IOBuffer*	m_pNext;
	IOBuffer*	m_pPrev;
	Player*		m_pPlayer;
};

struct Minion
{
	int		m_ID;
	int		m_Root;
	BOOL	m_Live;
	Vector3 m_TargetPos;
	Vector3 m_Pos;
	float	m_Rot;
};

struct MinionInfo
{
	int		m_ID;
	bool    m_Live;
	Vector3 m_Pos;
};

struct MinionPacket
{
	int size;
	MinionInfo MI;
};

enum NETWORKCODE
{
	NETNONE, HERODATA, HEROCOUNT, INITCLIENT, MINIONDATA, HEROCHOOSE
};
