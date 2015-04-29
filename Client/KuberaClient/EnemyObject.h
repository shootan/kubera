#pragma once
#include "GameObject.h"
#include "ST.h"

class EnemyObject : public CGameObject
{
private:
	
	BOOL m_bSetDestination;

public:
	EnemyObject();
	~EnemyObject();

	virtual void Animate(float fTimeElapsed);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);
	virtual void SetNewDestination ( D3DXVECTOR3 _pos );
	virtual void Update(float _fMoveIncrement);
	bool InMotion();

};