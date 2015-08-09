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
	int m_Id;

public:
	OtherPlayerManager()
	{
		memset(&m_pOtherPlayer,NULL,sizeof(m_pOtherPlayer));
		m_Type = 1;
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
		m_pOtherPlayer->SetTeam(m_Id);
		m_pOtherPlayer->SetType(m_Type);
		if(m_Type == 1 )
		{
			m_pOtherPlayer->SetMesh(m_pWarriorMesh);
		}
		if(m_Type == 2)
		{
			m_pOtherPlayer->SetMesh(m_pWizardMesh);
		}
		m_pOtherPlayer->SetTeam(m_Id);
		if(m_Id == 1)
			pos = D3DXVECTOR3(390, 0, 0);
		if(m_Id == 2)
			pos = D3DXVECTOR3(-390, 0, 0);
		m_pOtherPlayer->SetPosition(pos);
		m_pOtherPlayer->SetBoundSize(boundx, boundy, boundz);

		
	}

	void SetPlayerInfo(PlayerPacket _pi)
	{
		D3DXVECTOR3 q;
		q.x = _pi.PI.m_Data.m_Pos.x;
		q.y = 0;
		q.z =_pi.PI.m_Data.m_Pos.z;
		m_pOtherPlayer->SetNewDestination(q);
		m_pOtherPlayer->SetRot(_pi.PI.m_Data.m_Rot);
		m_pOtherPlayer->SetState(_pi.PI.m_iState);
		m_pOtherPlayer->SetTargetID(_pi.PI.m_iTargetID);
		m_pOtherPlayer->SetHP(_pi.PI.m_Data.m_HP);
		m_pOtherPlayer->SetDamage(_pi.PI.m_Data.m_Damage);
		m_pOtherPlayer->SetDefense(_pi.PI.m_Data.m_Defence);
		m_pOtherPlayer->SetSpeed(_pi.PI.m_Data.m_Speed);
	}

	void SetMesh(GFBX::Mesh* _warrior, GFBX::Mesh* _wizard)
	{
		m_pWarriorMesh = _warrior;
		m_pWizardMesh = _wizard;
	}


	void SetTeam(int _team) {m_Team = _team;}
	int GetTeam() {return m_Team;}
	int GetType() { return m_Type; }

	void SetType(int _type)
	{
		m_Type = _type;
	}

	void SetId(int _id)
	{
		m_Id = _id;
	}

};