#pragma once
#include "GameObject.h"
#include "astarclass.h"

class HeroObject : public CGameObject
{
private:
	D3DXVECTOR3 m_vAstarIncrement;
	BOOL m_bMove;

	Astar* m_pAstar;
	node_t* m_pBestWay;
	BOOL m_bAstar;
	BOOL m_bFindPath;
public:
	HeroObject();
	~HeroObject();

	virtual void Animate(float fTimeElapsed);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);
	virtual void SetNewDestination ( D3DXVECTOR3 _pos );
	virtual void Update(float fTimeElapsed);
	bool InMotion();

	void SetAstar(BOOL _astar) {m_bAstar = _astar;}
	BOOL GetAstar() { return m_bAstar; }

};