#pragma once
#include "GameObject.h"
#include "ST.h"
#include "FBX\GFBXMesh.h"
#include "ParticleManager.h"


class OtherPlayerObject : public CGameObject
{
private:
	
	BOOL m_bSetDestination;	
	BOOL m_bWarriorAttack;

	float m_fAttackTime;
	float m_Time;
	float m_fRespawnTime;

	int m_PrevState;

	int m_iparticleNum;
	BOOL m_bUseParticle;
	BOOL m_bUseParticleMissile;
	BOOL m_bUseParticleAttack;

	//ui 데스 카운트
	int m_nDeathCount;
public:
	OtherPlayerObject();
	~OtherPlayerObject();

	virtual void Animate(float fTimeElapsed);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext, float fTimeElapsed, CCamera *pCamera);
	virtual void SetNewDestination ( D3DXVECTOR3 _pos );
	virtual void Update(float fTimeElapsed);
	bool InMotion();

	int GetDeathCount() {return m_nDeathCount;}

};