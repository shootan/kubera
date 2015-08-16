#pragma once
#include "header.h"
#include "MinionObject.h"
#include "Mesh.h"

#define MAX_MINION 6

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
		{
			m_pMinion[i]->Release();
		}
		delete instance;
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
	void CreateMinion(D3DXVECTOR3 pos, GFBX::Mesh* mesh, float boundx, float boundy, float boundz)
	{
		for(int i=0; i<MAX_MINION; i++)
		{
			if(m_pMinion[i] != NULL) continue;
			m_pMinion[i] = new MinionObject();
			m_pMinion[i]->SetID(i+11);
			m_pMinion[i]->SetMesh(mesh);
			m_pMinion[i]->SetPosition(pos);
			m_pMinion[i]->SetBoundSize(boundx, boundy, boundz);
			break;
		}
	}
};