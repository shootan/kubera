#pragma once
#include "GameObject.h"
#include "ST.h"


class ParticleObject : public CGameObject
{
private:
	BOOL m_bUsed;
	float m_time;
	CGameObject* m_pAttacker;

	D3DXVECTOR3 m_CameraPosition;
public:
	ParticleObject();
	~ParticleObject();

	virtual void Animate(float fTimeElapsed){}
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera);
	virtual void SetNewDestination ( D3DXVECTOR3 _pos );
	virtual void Update(float fTimeElapsed);

	void SetType(int _type) { m_iType = _type; }
	int GetType() { return m_iType; }

	void SetUsed(BOOL _used) { m_bUsed = _used;}
	BOOL GetUsed() { return m_bUsed; }
	CMesh GetMesh() { return *m_pMesh; }

	void SetCameraPos(D3DXVECTOR3 _camerapos) { m_CameraPosition = _camerapos; }

	void SetAttacker(CGameObject* _attacker) { m_pAttacker = _attacker; }
	CGameObject* GetAttacker() { return m_pAttacker; }
};