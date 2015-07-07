#pragma once
#include "header.h"
#include "ObstacleObject.h"
#include "Mesh.h"

#define MAX_OBSTACLE 2000
#define MAX_DESTROY_TOWER 10

class ObstacleManager
{
public:
	ObstacleObject* m_pObstacle[MAX_OBSTACLE];
	ObstacleObject* m_pDestroyTower[MAX_DESTROY_TOWER];
private:
	ObstacleManager()
	{
		memset(&m_pObstacle,NULL,sizeof(m_pObstacle));
		memset(&m_pDestroyTower,NULL,sizeof(m_pDestroyTower));
	}
public:
	~ObstacleManager()
	{
		for(int i=0; i<MAX_OBSTACLE; i++)
			m_pObstacle[i]->Release();
		for(int i=0; i<MAX_DESTROY_TOWER; i++)
			m_pDestroyTower[i]->Release();

		delete instance;
		//DeleteObject(m_pResource);
		/*delete[] m_pMissile;
		memset(&m_pMissile, NULL, sizeof(m_pMissile));*/
	}
public:
	static ObstacleManager* instance;
	static ObstacleManager* sharedManager()
	{
		if(instance == NULL) 
			instance = new ObstacleManager;
		return instance;
	}
public:
	void CreateObstacle(D3DXVECTOR3 pos, GFBX::Mesh* mesh, float boundx, float boundy, float boundz)
	{
		for(int i=0; i<MAX_OBSTACLE; i++)
		{
			if(m_pObstacle[i] != NULL) continue;
			m_pObstacle[i] = new ObstacleObject();
			m_pObstacle[i]->SetMesh(mesh);
			m_pObstacle[i]->SetPosition(pos);
			m_pObstacle[i]->SetP(pos.x, pos.y, pos.z);
			m_pObstacle[i]->SetBoundSize(boundx, boundy, boundz);
			break;
		}
	}

	void CreateDestroyTower(D3DXVECTOR3 pos, GFBX::Mesh* mesh, float boundx, float boundy, float boundz)
	{
		for(int i=0; i<MAX_DESTROY_TOWER; i++)
		{
			if(m_pDestroyTower[i] != NULL) continue;
			m_pDestroyTower[i] = new ObstacleObject();
			m_pDestroyTower[i]->SetMesh(mesh);
			m_pDestroyTower[i]->SetPosition(pos);
			m_pDestroyTower[i]->SetP(pos.x, pos.y, pos.z);
			m_pDestroyTower[i]->SetBoundSize(boundx, boundy, boundz);
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