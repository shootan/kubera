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
		
		if(m_Team == 1)
			m_vPos = D3DXVECTOR3(390, 0, 0);
		if(m_Team == 2)
			m_vPos = D3DXVECTOR3(-390, 0, 0);

		m_pHero->SetTeam(m_Team);
		m_pHero->SetPosition(m_vPos);
		m_pHero->SetID(m_iID);
		m_pHero->SetBoundSize(boundx, boundy, boundz);
		m_pHero->SetHP(m_HP);
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

	void SetTeam(int _team) {m_Team = _team;}
	int GetTeam() {return m_Team;}
	void SetNexus(CGameObject* _red, CGameObject* _blue) { RedNexus= _red; BlueNexus = _blue; }
	void SetID(int _id){ m_iID = _id; }
	void SetStartPos(D3DXVECTOR3 _v) { m_vPos = _v; }
	void SetHP(float _hp) { m_HP = _hp; }
	void SetType(int _type) { m_Type = _type; }
	int GetType() { return m_Type; }
	int GetID() { return m_iID; }
};