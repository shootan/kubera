#pragma once
#include "header.h"
#include "OtherPlayerObject.h"
#include "Mesh.h"

#define MAX_OTHER_PLAYER 10

class OtherPlayerManager
{
public:
	OtherPlayerObject* m_pOtherPlayer[MAX_OTHER_PLAYER];

private:
	OtherPlayerManager()
	{
		memset(&m_pOtherPlayer,NULL,sizeof(m_pOtherPlayer));
	}
public:
	~OtherPlayerManager()
	{
		for(int i=0; i<MAX_OTHER_PLAYER; i++)
			m_pOtherPlayer[i]->Release();

		delete instance;
		//DeleteObject(m_pResource);
		/*delete[] m_pMissile;
		memset(&m_pMissile, NULL, sizeof(m_pMissile));*/
	}
public:
	static OtherPlayerManager* instance;
	static OtherPlayerManager* sharedManager()
	{
		if(instance == NULL) 
			instance = new OtherPlayerManager;
		return instance;
	}
public:
	void CreateOtherPlayer(D3DXVECTOR3 pos, CMesh* mesh, float boundx, float boundy, float boundz)
	{
		for(int i=0; i<MAX_OTHER_PLAYER; i++)
		{
			if(m_pOtherPlayer[i] != NULL) continue;
			m_pOtherPlayer[i] = new OtherPlayerObject();
			m_pOtherPlayer[i]->SetMesh(mesh);
			m_pOtherPlayer[i]->SetPosition(pos);
			//m_pTower[i]->SetP(pos.x, pos.y, pos.z);
			m_pOtherPlayer[i]->SetBoundSize(boundx, boundy, boundz);
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