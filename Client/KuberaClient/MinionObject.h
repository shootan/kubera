#pragma once
#include "GameObject.h"
#include "ST.h"
#include "FBX\GFBXMesh.h"
#include "ParticleManager.h"


class MinionObject : public CGameObject
{
private:
	float m_fAttackTime;
	float m_Time;
	float m_fRespawnTime;
	int m_PrevState;

	int m_iparticleNum;
	BOOL m_bUseParticle;
	BOOL m_bUseParticleMissile;
	BOOL m_bUseParticleAttack;

	BOOL m_bDeathAnimation;
	BOOL m_bAttack;

	D3DXVECTOR3 m_vWayPoint;  //Ã³À½ »ý¼º ÁöÁ¡

	//Á×Àº È½¼ö
	int m_nDeathCount;

public:
	MinionObject();
	~MinionObject();

	virtual void Animate(float fTimeElapsed);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext, float fTimeElapsed, CCamera *pCamera);
	virtual void SetNewDestination ( D3DXVECTOR3 _pos );
	virtual void Update(float fTimeElapsed);

	void SetWatchTarget(D3DXVECTOR3 _pos);

	void SetWayPoint(D3DXVECTOR3 _v) { m_vWayPoint = _v; }
	D3DXVECTOR3 GetWayPoint() { return m_vWayPoint; }

};