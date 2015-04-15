#pragma once
#pragma comment(lib, "ws2_32")
#include "targetver.h"

#include <stdio.h>
#include <string.h>
#include <tchar.h>
#include <WinSock2.h>
#include <process.h>
#include <math.h>


// BUFSIZE
#define BUFSIZE 1024*2
#define MAXPLAYER 30
#define HEADERSIZE sizeof(int)
struct Vector3
{
	float x, y, z;
};

struct PlayerInfo{
	int		m_ID;
	Vector3 m_Pos;
	Vector3 m_Scale;
	float	m_Rot;
};

struct PlayerPacket
{
	int			size;
	PlayerInfo	PI;
};

enum NETWORKCODE
{
	 NETNONE, HERODATA, HEROCOUNT, INITCLIENT
};