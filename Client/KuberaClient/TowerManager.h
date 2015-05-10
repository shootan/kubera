#pragma once
#include "header.h"
#include "TowerObject.h"
#include "Mesh.h"

#define MAX_TOWER 10

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
		//DeleteObject(m_pResource);
		/*delete[] m_pMissile;
		memset(&m_pMissile, NULL, sizeof(m_pMissile));*/
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
	void CreateTower(D3DXVECTOR3 pos, CMesh* mesh, float boundx, float boundy, float boundz)
	{
		for(int i=0; i<MAX_TOWER; i++)
		{
			if(m_pTower[i] != NULL) continue;
			m_pTower[i] = new TowerObject();
			m_pTower[i]->SetID(i+164);
			m_pTower[i]->SetMesh(mesh);
			m_pTower[i]->SetPosition(pos);
			//m_pTower[i]->SetP(pos.x, pos.y, pos.z);
			m_pTower[i]->SetBoundSize(boundx, boundy, boundz);
			break;
		}
	}
	//void AddMissile(CGPoint _pos, Object_Enemy* _target, int _damage)
	//{
	//	for(int i=0; i<MAX_MISSILE; i++)
	//	{
	//		if(m_pMissile[i] != NULL) continue;
	//		m_pMissile[i] = new Object_Missile;
	//		m_pMissile[i]->init();
	//		m_pMissile[i]->setPosition(_pos);
	//		m_pMissile[i]->SetDamage(_damage);
	//		m_pMissile[i]->SetTarget(_target);
	//		m_pScene->addChild(m_pMissile[i]);
	//		break;
	//	}
	//}

	//void Update()
	//{
	//	for(int i=0; i<MAX_MISSILE; i++)
	//	{
	//		if(m_pMissile[i] == NULL) continue;
	//		if(m_pMissile[i]->getActive() == false)
	//		{
	//			m_pScene->removeChild(m_pMissile[i],true);
	//			//delete m_pMissile[i];
	//			m_pMissile[i] = NULL;
	//			continue;
	//		}
	//	}
	//}
};