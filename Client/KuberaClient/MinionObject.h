#pragma once
#include "GameObject.h"

class MinionObject : public CGameObject
{
public:
	MinionObject();
	~MinionObject();

	virtual void AddRef();
	virtual void Release();
	virtual void Animate(float fTimeElapsed);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);
	virtual void SetNewDestination ( D3DXVECTOR3 _pos );
	virtual void Update(float _fMoveIncrement);
	virtual bool InMotion();

};