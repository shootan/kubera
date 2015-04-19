#pragma once
#include "GameObject.h"

class TowerObject : public CGameObject
{
private:
	CGameObject* Target;

public:
	TowerObject();
	~TowerObject();

	virtual void Animate(float fTimeElapsed){}
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);
	virtual void SetNewDestination ( D3DXVECTOR3 _pos ){}
	virtual void Update(float _fMoveIncrement);

	void SetTaget(CGameObject* _target) { Target = _target; }
};