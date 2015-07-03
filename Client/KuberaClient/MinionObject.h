#pragma once
#include "GameObject.h"

class MinionObject : public CGameObject
{
private:
	BOOL m_bSetDestination;
public:
	MinionObject();
	~MinionObject();

	virtual void Animate(float fTimeElapsed);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera);
	virtual void SetNewDestination ( D3DXVECTOR3 _pos );
	virtual void Update(float fTimeElapsed);
	bool InMotion();

};