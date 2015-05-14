#pragma once
#include "header.h"
#include "MinionObject.h"
#include "Mesh.h"

#define MAX_MINION 160

class MinionManager
{
public:
	MinionObject* m_pMinion1[MAX_MINION];
	MinionObject* m_pMinion2[MAX_MINION];
	MinionObject* m_pMinion3[MAX_MINION];
	MinionObject* m_pMinion4[MAX_MINION];

private:
	MinionManager()
	{
		memset(&m_pMinion1,NULL,sizeof(m_pMinion1));
		memset(&m_pMinion2,NULL,sizeof(m_pMinion2));
		memset(&m_pMinion3,NULL,sizeof(m_pMinion3));
		memset(&m_pMinion4,NULL,sizeof(m_pMinion4));
	}
public:
	~MinionManager()
	{
		for(int i=0; i<MAX_MINION; i++)
		{
			m_pMinion1[i]->Release();
			m_pMinion2[i]->Release();
			m_pMinion3[i]->Release();
			m_pMinion4[i]->Release();
		}
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
	void CreateMinion1(D3DXVECTOR3 pos, CMesh* mesh)
	{
		for(int i=0; i<MAX_MINION; i++)
		{
			if(m_pMinion1[i] != NULL) continue;
			m_pMinion1[i] = new MinionObject();
			m_pMinion1[i]->SetID(i+5);
			m_pMinion1[i]->SetMesh(mesh);
			m_pMinion1[i]->SetPosition(pos);
			m_pMinion1[i]->SetVisible(FALSE);
		}
	}
	void CreateMinion2(D3DXVECTOR3 pos, CMesh* mesh)
	{
		for(int i=0; i<MAX_MINION; i++)
		{
			if(m_pMinion2[i] != NULL) continue;
			m_pMinion2[i] = new MinionObject();
			m_pMinion2[i]->SetMesh(mesh);
			m_pMinion2[i]->SetPosition(pos);
			m_pMinion2[i]->SetVisible(FALSE);
		}
	}
	void CreateMinion3(D3DXVECTOR3 pos, CMesh* mesh)
	{
		for(int i=0; i<MAX_MINION; i++)
		{
			if(m_pMinion3[i] != NULL) continue;
			m_pMinion3[i] = new MinionObject();
			m_pMinion3[i]->SetMesh(mesh);
			m_pMinion3[i]->SetPosition(pos);
			m_pMinion3[i]->SetVisible(TRUE);
		}
	}
	void CreateMinion4(D3DXVECTOR3 pos, CMesh* mesh)
	{
		for(int i=0; i<MAX_MINION; i++)
		{
			if(m_pMinion4[i] != NULL) continue;
			m_pMinion4[i] = new MinionObject();
			m_pMinion4[i]->SetMesh(mesh);
			m_pMinion4[i]->SetPosition(pos);
			m_pMinion4[i]->SetVisible(TRUE);
		}
	}

	void ChangeVisible()
	{
		for(int i=0; i<MAX_MINION; i++)
		{
			if(m_pMinion1[i]->GetVisible() == TRUE)
				m_pMinion1[i]->SetVisible(FALSE);
			else
				m_pMinion1[i]->SetVisible(TRUE);

			if(m_pMinion2[i]->GetVisible() == TRUE)
				m_pMinion2[i]->SetVisible(FALSE);
			else
				m_pMinion2[i]->SetVisible(TRUE);

			if(m_pMinion3[i]->GetVisible() == TRUE)
				m_pMinion3[i]->SetVisible(FALSE);
			else
				m_pMinion3[i]->SetVisible(TRUE);

			if(m_pMinion4[i]->GetVisible() == TRUE)
				m_pMinion4[i]->SetVisible(FALSE);
			else
				m_pMinion4[i]->SetVisible(TRUE);

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