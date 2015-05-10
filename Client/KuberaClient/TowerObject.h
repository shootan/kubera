#pragma once
#include "GameObject.h"
#include "ST.h"
#include "MissileManager.h"

class TowerObject : public CGameObject
{
private:
	CGameObject* m_pTarget;
	CGameObject* m_pPrevTarget;
	float		 m_fAttackTime;
public:
	TowerObject();
	~TowerObject();

	virtual void Animate(float fTimeElapsed){}
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);
	virtual void SetNewDestination ( D3DXVECTOR3 _pos ){}
	virtual void Update(float fTimeElapsed);

	void SetTarget(CGameObject* _target) { m_pTarget = _target; }
	CGameObject* GetTarget() { return m_pTarget; }

	void SetAttackTime(float _attacktime) { m_fAttackTime = _attacktime; }
};