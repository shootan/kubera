
#pragma once
#include "header.h"
#include "Network.h"
#include "GameObject.h"

class ST
{
private:
	ST()
	{
		m_bVisible =FALSE;
		m_bSelected = FALSE;
		m_bStart = FALSE;
		Net = NULL;
		m_bReconnect = FALSE;
	}
public:
	~ST()
	{
		delete instance;
	}

	BOOL m_bVisible;
	BOOL m_bSelected;
	BOOL m_bStart;
	BOOL m_bReconnect;

	Network* Net;

	CGameObject* m_pHero;
	CGameObject* m_pOtherPlayer;
	CGameObject* m_pTowerObject[8];
	CGameObject* m_pMinionObject[6];
	CGameObject* m_pRedNexus;
	CGameObject* m_pBlueNexus;


public:
	static ST* instance;
	static ST* sharedManager()
	{
		if(instance == NULL) instance = new ST;
		return instance;
	}

private:

public:
	float GetDistance(Vector3 f_pos, Vector3 s_pos)
	{
		Vector3 v;
		v.x = f_pos.x - s_pos.x;
		v.y = f_pos.y - s_pos.y;
		v.z = f_pos.z - s_pos.z;
		float distance = sqrtf(v.x*v.x + v.z*v.z);
		return distance;
	}

	float GetDistance(D3DXVECTOR3 f_pos, D3DXVECTOR3 s_pos)
	{
		float distance = sqrtf((f_pos.x - s_pos.x)*(f_pos.x - s_pos.x) + (f_pos.z - s_pos.z)*(f_pos.z - s_pos.z));
		return distance;
	}

	Vector3 GetVector(Vector3 f_pos, Vector3 s_pos)
	{
		Vector3 v;
		v.x = f_pos.x - s_pos.x;
		v.y = f_pos.y - s_pos.y;
		v.z = f_pos.z - s_pos.z;

		float distance = GetDistance(f_pos, s_pos);

		v.x /= distance;
		v.z /= distance;

		return v;
	}

	void SetAttack(AttackInfo _ati)
	{
		printf("ID: %d, Damage : %f \n", _ati.m_ID, _ati.m_Damage);
		
		if(m_pHero->GetID() == _ati.m_ID && m_pHero->GetState() != DEATH && m_pHero->GetHP() > 0.0f)
		{
			m_pHero->SetAttackDamage(_ati.m_Damage);
			return;
		}

		if(m_pOtherPlayer->GetID() == _ati.m_ID && m_pOtherPlayer->GetState() != DEATH && m_pOtherPlayer->GetHP() > 0.0f)
		{
			m_pOtherPlayer->SetAttackDamage(_ati.m_Damage);
			return;
		}

		if(m_pBlueNexus->GetID() == _ati.m_ID && m_pBlueNexus->GetState() != DEATH && m_pBlueNexus->GetHP() > 0.0f)
		{
			m_pBlueNexus->SetAttackDamage(_ati.m_Damage);
			return;
		}

		if(m_pRedNexus->GetID() == _ati.m_ID && m_pRedNexus->GetState() != DEATH && m_pRedNexus->GetHP() > 0.0f)
		{
			m_pRedNexus->SetAttackDamage(_ati.m_Damage);
			return;
		}

		for(int i=0; i<8; i++)
		{
			if(m_pTowerObject[i] == NULL) continue;
			if(m_pTowerObject[i]->GetState() == DEATH) continue;
			if(m_pTowerObject[i]->GetHP() < 0.0f) continue;
			if(m_pTowerObject[i]->GetID() == _ati.m_ID)
			{
				m_pTowerObject[i]->SetAttackDamage(_ati.m_Damage);
			}
		}

		for(int i=0; i<6; i++)
		{
			if(m_pMinionObject[i] == NULL) continue;
			if(m_pMinionObject[i]->GetState() == DEATH) continue;
			if(m_pMinionObject[i]->GetHP() < 0.0f) continue;
			if(m_pMinionObject[i]->GetID() == _ati.m_ID)
			{
				m_pMinionObject[i]->SetAttackDamage(_ati.m_Damage);
			}
		}
	}
};