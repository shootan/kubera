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
	void CreateMissile(D3DXVECTOR3 pos, GFBX::Mesh* mesh)
	{
		for(int i=0; i<MAX_MISSILE; i++)
		{
			if(m_pMissile[i] != NULL) continue;
			m_pMissile[i] = new MissileObject();
			m_pMissile[i]->SetMesh(mesh);
			m_pMissile[i]->SetPosition(pos);
		}
	}

};