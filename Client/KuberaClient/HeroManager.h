#pragma once
#include "header.h"
#include "HeroObject.h"
#include "Mesh.h"


class HeroManager
{
public:
	HeroObject* m_pHero;
	int m_iID;
	D3DXVECTOR3 m_vPos;
	float m_HP;
	int m_Type;
	int m_Team;
	float m_defence;
	float m_Speed;

private:
	HeroManager()
	{
		memset(&m_pHero,NULL,sizeof(m_pHero));
	}
public:
	~HeroManager()
	{
		m_pHero->Release();

		delete instance;
	}
public:
	static HeroManager* instance;
	static HeroManager* sharedManager()
	{
		if(instance == NULL) 
			instance = new HeroManager;
		return instance;
	}
public:
	CGameObject* RedNexus;
	CGameObject* BlueNexus;

	void CreateHero(GFBX::Mesh* _warrior,GFBX::Mesh* _wizard, float boundx, float boundy, float boundz)
	{
		m_pHero = new HeroObject();

		m_pHero->SetTeam(m_Team);
		m_pHero->SetID(m_Team);

		if(m_Team == 1 && !ST::sharedManager()->m_bReconnect)
			m_vPos = D3DXVECTOR3(390, 0, 0);
		if(m_Team == 2 && !ST::sharedManager()->m_bReconnect)
			m_vPos = D3DXVECTOR3(-390, 0, 0);

		m_pHero->SetPosition(m_vPos);
		if(ST::sharedManager()->m_bReconnect)
		{

			m_pHero->SetHP(m_HP);
			m_pHero->SetSpeed(m_Speed);
			m_pHero->SetDefense(m_defence);
		}


		m_pHero->SetBoundSize(boundx, boundy, boundz);
		m_pHero->SetType(m_Type);

		if(m_Type == 1)
		{
			m_pHero->SetMesh(_warrior);
		}
		else if(m_Type == 2)
		{
			m_pHero->SetMesh(_wizard);
		}
		
	}

	void SetPlayerInfo(PlayerInfo _info)
	{
		m_Team = _info.m_ID;
		m_HP = _info.m_Data.m_HP;
		m_Type = _info.m_Type;
		m_defence = _info.m_Data.m_Defence;
		m_Speed = _info.m_Data.m_Speed;
		m_vPos.x = _info.m_Data.m_Pos.x;
		m_vPos.y = _info.m_Data.m_Pos.y;
		m_vPos.z = _info.m_Data.m_Pos.z;


	}
	void SetTeam(int _team) 
	{
		m_Team = _team;
	}
	void SetNexus(CGameObject* _red, CGameObject* _blue) { RedNexus= _red; BlueNexus = _blue; }
	void SetID(int _id){ m_iID = _id; }
	void SetStartPos(D3DXVECTOR3 _v) { m_vPos = _v; }
	void SetHP(float _hp) { m_HP = _hp; }
	void SetType(int _type) { m_Type = _type; }
	int GetID() { return m_iID; }
};