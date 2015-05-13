#pragma once
#include "header.h"
#include "HeroObject.h"
#include "Mesh.h"


class HeroManager
{
public:
	HeroObject* m_pHero;

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
	void CreateHero(D3DXVECTOR3 pos, CMesh* mesh, float boundx, float boundy, float boundz)
	{
		m_pHero = new HeroObject();
		m_pHero->SetMesh(mesh);
		m_pHero->SetPosition(pos);
		//m_pTower[i]->SetP(pos.x, pos.y, pos.z);
		m_pHero->SetBoundSize(boundx, boundy, boundz);
		
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