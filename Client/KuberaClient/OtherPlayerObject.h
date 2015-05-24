#pragma once
#include "GameObject.h"
#include "ST.h"
#include "FBX\GFBXMesh.h"

class OtherPlayerObject : public CGameObject
{
private:
	
	BOOL m_bSetDestination;	

	float m_fAttackTime;

	GFBX::Mesh *m_pAniMesh;

public:
	OtherPlayerObject();
	~OtherPlayerObject();

	virtual void Animate(float fTimeElapsed);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);
	virtual void SetNewDestination ( D3DXVECTOR3 _pos );
	virtual void Update(float fTimeElapsed);
	bool InMotion();

	void SetAniMesh(GFBX::Mesh *pAniMesh);

};