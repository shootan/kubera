#pragma once
#include "GameObject.h"
#include "ST.h"

class MissileObject : public CGameObject
{
private:
	BOOL m_bUsed;
public:
	MissileObject();
	~MissileObject();

	virtual void Animate(float fTimeElapsed){}
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);
	virtual void SetNewDestination ( D3DXVECTOR3 _pos );
	virtual void Update(float fTimeElapsed);

	void SetTarget(CGameObject* _target) { m_pTarget = _target; }
	CGameObject* GetTarget() { return m_pTarget; }

	void SetUsed(BOOL _b) { m_bUsed = _b; }
	BOOL GetUsed() { return m_bUsed; }
};