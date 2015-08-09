#pragma once
#include "GameObject.h"
#include "astarclass.h"
#include "ST.h"


class HeroObject : public CGameObject
{
private:
	D3DXVECTOR3 m_vAstarIncrement;
	BOOL m_bMove;
	float m_time;
	float m_fRespawnTime;
	BOOL m_bWarriorAttack;
	BOOL m_bWarriorSkill;
	
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

	//정보
	int m_Speed_Level;
	int m_Defence_Level;
	int m_Damage_Level;

	BOOL m_bUpgrade;

	BOOL m_bSoundLimit;

	//hp가 시작하자마자 -값이 되어버리는 버그때문에 만듬
	BOOL m_bHpFUCK;

	//ui 데스 카운트
	int m_nDeathCount;
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

	void SetUpgradePossible(BOOL _upgrade) { m_bUpgrade = _upgrade;}
	BOOL GetUpgradePossible() { return m_bUpgrade;}

	void SetDamageLevel(int _level) { m_Damage_Level = _level;}
	void SetDefenseLevel(int _level) { m_Defence_Level = _level;}
	void SetSpeedLevel(int _level) { m_Speed_Level = _level;}
	int GetDamageLevel() { return m_Damage_Level;}
	int GetDefenseLevel() { return m_Defence_Level;}
	int GetSpeedLevel() { return m_Speed_Level;}

	//레벨 업에 따른 캐릭터 정보
	void UpdateCharacterInfo(int _Level);
	void LevelUp();
	void DamageUp(float _damage);
	void SpeedUp(float _speed);
	void DefenseUp(float _defense);

	int GetDeathCount() {return m_nDeathCount;}
	void SetWatchTarget(D3DXVECTOR3 _pos);

	Vector3 GetDestination()
	{
		Vector3 p;
		p.x = m_vDestination.x;
		p.y = m_vDestination.y;
		p.z = m_vDestination.z; 
		return p;
	}

};