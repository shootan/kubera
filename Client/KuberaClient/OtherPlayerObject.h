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

	int m_PrevState;

	GFBX::Mesh *m_pAniMesh;

	int m_iparticleNum;
	BOOL m_bUseParticle;
	BOOL m_bUseParticleMissile;
	BOOL m_bUseParticleAttack;
public:
	OtherPlayerObject();
	~OtherPlayerObject();

	virtual void Animate(float fTimeElapsed);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext, float fTimeElapsed);
	virtual void SetNewDestination ( D3DXVECTOR3 _pos );
	virtual void Update(float fTimeElapsed);
	bool InMotion();

	void SetAniMesh(GFBX::Mesh *pAniMesh);

};