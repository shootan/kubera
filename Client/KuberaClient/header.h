#pragma once




#define FRAME_BUFFER_WIDTH		900
#define FRAME_BUFFER_HEIGHT		600
#define VS_SLOT_VIEWPROJECTION_MATRIX	0x00
#define VS_SLOT_WORLD_MATRIX			0x01
//조명과 재질을 설정하기 위한 상수 버퍼의 슬롯 번호를 정의한다. 
#define PS_SLOT_LIGHT			0x03
#define PS_SLOT_MATERIAL		0x04



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

struct Info{
	int		m_Level;
	Vector3 m_Pos;
	float	m_Rot;
	float	m_HP;
	float	m_Damage;
	float	m_Defence;
	float	m_Speed;
};

struct PlayerInfo{
	int		m_ID;
	int		m_iState;
	int		m_iTargetID;
	int		m_Type;
	Info	m_Data;
};

struct PlayerPacket
{
	int			size;
	PlayerInfo	PI;
};

struct AttackInfo
{
	int		m_ID;
	float	m_Damage;
};


enum TAG
{
	NONE, HERO, OTHERPLAYER, MINION, OBSTACLE, PLANE, HERO_BOUND, OBSTACLE_BOUND, TOWER, MISSILE, EFFECT
};

enum STATE
{
	IDLE, ATTACK, MOVE, DEATH, SKILL1, WAIT
};

enum TEAM
{
	NONE_TEAM, RED_TEAM, BLUE_TEAM
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
	NETNONE, HERODATA, HEROCOUNT, INITCLIENT, CLIENT_CONNECT, CONNECT_SUCCESS,SELECT_TEAM_RED, SELECT_TEAM_BLUE,
	SELECT_TEAM, SELECT_CHAR_WARIOR, SELECT_CHAR_WIZARD, SELECT_CHAR, READY_GAME, START_GAME, ATTACK_INFO
};

enum HEROTYPE
{
	TYPENONE, KNIGHT, WIZARD
};

enum EFFECTTYPE
{
	EFFECTTYPE_NONE, WIZARD_SKILL_BODY, WIZARD_SKILL_MISSILE, WIZARD_ATTACK
};

struct Information
{
	int team;
	float x, z;
};

struct ObjectInfo{
	int		m_Id;
	int		m_Hp;
	int		m_Damage;
	int		m_Level;
	Vector3 m_Pos;
};

enum MONSTERTYPE
{
	MONSTERTYPENONE, CLEFT, TURTLE, CANNONGOLEM
};

enum OBJECT_FACE
{
	NONE_FACE, HERO1_FACE, HERO2_FACE, TOWER_FACE, NEXUS_FACE, CLEFT_FACE, TURTLE_FACE, GOLEM_FACE
};