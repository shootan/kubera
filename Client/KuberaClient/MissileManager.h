#pragma once
#include "header.h"
#include "MissileObject.h"
#include "Mesh.h"

#define MAX_MISSILE 256

class MissileManager
{
public:
	MissileObject* m_pMissile[MAX_MISSILE];

private:
	MissileManager()
	{
		memset(&m_pMissile,NULL,sizeof(m_pMissile));
	}
public:
	~MissileManager()
	{
		for(int i=0; i<MAX_MISSILE; i++)
			m_pMissile[i]->Release();

		delete instance;
	}
public:
	static MissileManager* instance;
	static MissileManager* sharedManager()
	{
		if(instance == NULL) 
			instance = new MissileManager;
		return instance;
	}
public:
	void CreateMissile(D3DXVECTOR3 pos, CMesh* mesh)
	{
		for(int i=0; i<MAX_MISSILE; i++)
		{
			if(m_pMissile[i] != NULL) continue;
			m_pMissile[i] = new MissileObject();
			m_pMissile[i]->SetMesh(mesh);
			m_pMissile[i]->SetPosition(pos);
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