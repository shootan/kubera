#pragma once
#include "header.h"
#include "TowerObject.h"
#include "Mesh.h"

#define MAX_TOWER 50

class TowerManager
{
public:
	TowerObject* m_pTower[MAX_TOWER];
private:
	TowerManager()
	{
		memset(&m_pTower,NULL,sizeof(m_pTower));
	}
public:
	~TowerManager()
	{
		for(int i=0; i<MAX_TOWER; i++)
			m_pTower[i]->Release();

		delete instance;
	}
public:
	static TowerManager* instance;
	static TowerManager* sharedManager()
	{
		if(instance == NULL) 
			instance = new TowerManager;
		return instance;
	}
public:
	int size;
	void CreateTower(D3DXVECTOR3 pos, GFBX::Mesh* mesh, float boundx, float boundy, float boundz, int team)
	{
		for(int i=0; i<size; i++)
		{
			if(m_pTower[i] != NULL) continue;
			m_pTower[i] = new TowerObject();
			m_pTower[i]->SetID(i+165);
			m_pTower[i]->SetMesh(mesh);
			m_pTower[i]->SetPosition(pos);
			m_pTower[i]->SetTeam(team);
			//m_pTower[i]->SetP(pos.x, pos.y, pos.z);
			m_pTower[i]->SetBoundSize(boundx, boundy, boundz);
			m_pTower[i]->SetFaceType(TOWER_FACE);
			break;
		}
	}

	void SetTowerSize(int _size)
	{
		size = _size;
	}
};