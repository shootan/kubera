#pragma once
#include "GameObject.h"

class ObstacleObject : public CGameObject
{
private:
	BOOL m_bUsed;
public:
	ObstacleObject();
	~ObstacleObject();

	virtual void Animate(float fTimeElapsed){}
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);
	virtual void SetNewDestination ( D3DXVECTOR3 _pos ){}
	virtual void Update(float fTimeElapsed){}
	
	void SetUsed(BOOL _used) { m_bUsed = _used;}
	BOOL GetUsed() { return m_bUsed; }
};