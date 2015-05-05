#pragma once
#include "header.h"
#include "MinionObject.h"
#include "Mesh.h"

#define MAX_MINION 256

class MinionManager
{
public:
	MinionObject* m_pMinion[MAX_MINION];

private:
	MinionManager()
	{
		memset(&m_pMinion,NULL,sizeof(m_pMinion));
	}
public:
	~MinionManager()
	{
		for(int i=0; i<MAX_MINION; i++)
			m_pMinion[i]->Release();

		delete instance;
		//DeleteObject(m_pResource);
		/*delete[] m_pMissile;
		memset(&m_pMissile, NULL, sizeof(m_pMissile));*/
	}
public:
	static MinionManager* instance;
	static MinionManager* sharedManager()
	{
		if(instance == NULL) 
			instance = new MinionManager;
		return instance;
	}
public:
	void CreateMinion(D3DXVECTOR3 pos, CMesh* mesh)
	{
		for(int i=0; i<MAX_MINION; i++)
		{
			if(m_pMinion[i] != NULL) continue;
			m_pMinion[i] = new MinionObject();
			m_pMinion[i]->SetMesh(mesh);
			m_pMinion[i]->SetPosition(pos);
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