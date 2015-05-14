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
		//DeleteObject(m_pResource);
		/*delete[] m_pMissile;
		memset(&m_pMissile, NULL, sizeof(m_pMissile));*/
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
	void CreateHero(CMesh* mesh, float boundx, float boundy, float boundz)
	{
		m_pHero = new HeroObject();
		m_pHero->SetMesh(mesh);
		m_pHero->SetPosition(m_vPos);
		m_pHero->SetID(m_iID);
		m_pHero->SetBoundSize(boundx, boundy, boundz);


		if(m_iID % 2 == 0)
			HeroManager::sharedManager()->m_pHero->SetTeam(RED_TEAM);
		else
			HeroManager::sharedManager()->m_pHero->SetTeam(BLUE_TEAM);
	}

	void SetID(int _id){ m_iID = _id; }
	void SetStartPos(D3DXVECTOR3 _v) { m_vPos = _v; }
	int GetID() { return m_iID; }
};