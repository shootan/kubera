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

struct PlayerInfo{
	int		m_ID;
	int		m_iState;
	int		m_iTargetID;
	Vector3 m_Pos;
	float	m_Rot;
	float	m_HP;
	float	m_Damage;
};

struct PlayerPacket
{
	int			size;
	PlayerInfo	PI;
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
	NETNONE, HERODATA, HEROCOUNT, INITCLIENT, MINIONDATA
};