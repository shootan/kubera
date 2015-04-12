#pragma once
#include "GameObject.h"

class HeroObject : public CGameObject
{
private:
	D3DXVECTOR3 m_vAstarIncrement;
	BOOL m_bMove;

public:
	HeroObject();
	~HeroObject();

	virtual void Animate(float fTimeElapsed);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);
	virtual void SetNewDestination ( D3DXVECTOR3 _pos );
	virtual void Update(float _fMoveIncrement);
	bool InMotion();

};