#pragma once
#include "header.h"
#include "OtherPlayerObject.h"
#include "Mesh.h"

class OtherPlayerManager
{
public:
	OtherPlayerObject* m_pOtherPlayer;
	GFBX::Mesh *m_pWizardMesh;
	GFBX::Mesh *m_pWarriorMesh;
	int m_Type;
	int m_Team;


public:
	OtherPlayerManager()
	{
		memset(&m_pOtherPlayer,NULL,sizeof(m_pOtherPlayer));
	}
	~OtherPlayerManager()
	{

		m_pOtherPlayer->Release();
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
		m_pOtherPlayer = new OtherPlayerObject();
		//m_pOtherPlayer->SetAniMesh(mesh);
		m_pOtherPlayer->SetPosition(pos);
		m_pOtherPlayer->SetBoundSize(boundx, boundy, boundz);
		//m_pOtherPlayer->SetScale(D3DXVECTOR3(0.1, 0.1, 0.1));
		
	}

	void SetMesh(GFBX::Mesh* _warrior, GFBX::Mesh* _wizard)
	{
		m_pWarriorMesh = _warrior;
		m_pWizardMesh = _wizard;
	}

	void SetTeam(int _team) {m_Team = _team;}
	int GetTeam() {return m_Team;}
	void SetType(int _type) { m_Type = _type; }
	int GetType() { return m_Type; }
};