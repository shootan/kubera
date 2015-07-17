#pragma once
#include "GameObject.h"
#include "astarclass.h"
#include "ST.h"
#include "MissileManager.h"
#include "ParticleManager.h"
#include "OtherPlayerManager.h"

class HeroObject : public CGameObject
{
private:
	D3DXVECTOR3 m_vAstarIncrement;
	BOOL m_bMove;
	float m_time;
	BOOL m_bWarriorAttack;
	
	float m_fAttackTime;
	int m_iPrevState;

	BOOL m_bDeathAnimation;
	Astar* m_pAstar;
	node_t* m_pBestWay;
	BOOL m_bAstar;
	BOOL m_bFindPath;

	int m_iparticleNum;
	BOOL m_bUseParticle;
	BOOL m_bUseParticleMissile;
	BOOL m_bUseParticleAttack;

	//Á¤º¸
	int m_Level;
	int m_Defence;
	int m_SkillDamage;
	int m_Exp;
	int m_Speed_Level;
	int m_Defence_Level;
	int m_Damage_Level;


public:
	HeroObject();
	~HeroObject();

	virtual void Animate(float fTimeElapsed);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext, float fTimeElapsed, CCamera *pCamera);
	virtual void SetNewDestination ( D3DXVECTOR3 _pos );
	virtual void Update(float fTimeElapsed);
	bool InMotion();

	void SetAstar(BOOL _astar) {m_bAstar = _astar;}
	BOOL GetAstar() { return m_bAstar; }

	void SetState(int _state) { m_iState = _state;}

	void CharacterInfo();

	Vector3 GetDestination()
	{
		Vector3 p;
		p.x = m_vDestination.x;
		p.y = m_vDestination.y;
		p.z = m_vDestination.z; 
		return p;
	}

};