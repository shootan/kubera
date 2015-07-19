#pragma once
#include "header.h"
#include "OtherPlayerObject.h"
#include "Mesh.h"

#define MAX_OTHER_PLAYER 4

class OtherPlayerManager
{
public:
	OtherPlayerObject* m_pOtherPlayer[MAX_OTHER_PLAYER];
	GFBX::Mesh *m_pWizardMesh;
	GFBX::Mesh *m_pWarriorMesh;

public:
	OtherPlayerManager()
	{
		memset(&m_pOtherPlayer,NULL,sizeof(m_pOtherPlayer));
	}
	~OtherPlayerManager()
	{
		for(int i=0; i<MAX_OTHER_PLAYER; i++)
			m_pOtherPlayer[i]->Release();

		delete instance;
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
	void CreateOtherPlayer(D3DXVECTOR3 pos, float boundx, float boundy, float boundz)
	{
		for(int i=0; i<MAX_OTHER_PLAYER; i++)
		{
			if(m_pOtherPlayer[i] != NULL) continue;
			m_pOtherPlayer[i] = new OtherPlayerObject();
			//m_pOtherPlayer[i]->SetAniMesh(mesh);
			m_pOtherPlayer[i]->SetPosition(pos);
			//m_pTower[i]->SetP(pos.x, pos.y, pos.z);
			m_pOtherPlayer[i]->SetBoundSize(boundx, boundy, boundz);
			m_pOtherPlayer[i]->SetScale(D3DXVECTOR3(0.1, 0.1, 0.1));
		}
	}

	void SetMesh(GFBX::Mesh* _warrior, GFBX::Mesh* _wizard)
	{
		m_pWarriorMesh = _warrior;
		m_pWizardMesh = _wizard;
	}
};