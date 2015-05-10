#pragma once
#include "GameObject.h"
#define MAX_MINION 40

class MinionObject : public CGameObject
{
private:
	BOOL m_bSetDestination;
public:
	MinionObject();
	~MinionObject();

	virtual void Animate(float fTimeElapsed);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);
	virtual void SetNewDestination ( D3DXVECTOR3 _pos );
	virtual void Update(float fTimeElapsed);
	bool InMotion();

};