#pragma once
#include "GameObject.h"

class HeroObject : public CGameObject
{
public:
	HeroObject();
	~HeroObject();

	virtual void Animate(float fTimeElapsed);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);
	virtual void SetNewDestination ( D3DXVECTOR3 _pos );
	virtual void Update(float _fMoveIncrement);
	bool InMotion();

};