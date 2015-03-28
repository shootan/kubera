#pragma once




#define FRAME_BUFFER_WIDTH		800
#define FRAME_BUFFER_HEIGHT		600
#define VS_SLOT_VIEWPROJECTION_MATRIX	0x00
#define VS_SLOT_WORLD_MATRIX			0x01




#include <windows.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx11tex.h>
#include <Mmsystem.h>
#include <math.h>
#include <d3dcompiler.h>
#include <D3DX10Math.h>
#include "DXUT.h"
#include "DXUTcamera.h"
#include "DXUTgui.h"
#include "DXUTsettingsDlg.h"
#include "SDKmisc.h"
#include "SDKMesh.h"
#include <process.h>

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

enum TAG
{
	NONE, HERO, OTHERPLAYER, OBSTACLE, PLANE
};
