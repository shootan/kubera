#pragma once
#include "GameObject.h"
#include "astarclass.h"
#include "ST.h"
#include "MissileManager.h"
#include "FBX\GFBXMesh.h"

class HeroObject : public CGameObject
{
private:
	D3DXVECTOR3 m_vAstarIncrement;
	BOOL m_bMove;
	float m_time;
	
	float m_fAttackTime;

	Astar* m_pAstar;
	node_t* m_pBestWay;
	BOOL m_bAstar;
	BOOL m_bFindPath;

	GFBX::Mesh *m_pAniMesh;
	
public:
	HeroObject();
	~HeroObject();

	virtual void Animate(float fTimeElapsed);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext, float fTimeElapsed);
	virtual void SetNewDestination ( D3DXVECTOR3 _pos );
	virtual void Update(float fTimeElapsed);
	bool InMotion();

	void SetAstar(BOOL _astar) {m_bAstar = _astar;}
	BOOL GetAstar() { return m_bAstar; }

	
	

	void SetState(int _state) { m_iState = _state;}

	void SetAniMesh(GFBX::Mesh *pAniMesh);
	 Vector3 GetDestination()
	 {
		 Vector3 p;
		 p.x = m_vDestination.x;
		 p.y = m_vDestination.y;
		 p.z = m_vDestination.z; 
		 return p;
	 }

};