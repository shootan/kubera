#pragma once
#include "Mesh.h"
#include "header.h"

class CGameObject
{
public:
	CGameObject(void);
	~CGameObject(void);

private:
	int m_nReferences;
	D3DXVECTOR3 m_Pos;
	D3DXVECTOR3 m_Scale;
	FLOAT		m_Rot;
	D3DXVECTOR3 m_vDestination;
	D3DXVECTOR3 m_vWalkIncrement;
	D3DXVECTOR3 m_vFacingDirection;

	float		m_fWalkSpeed;

public:
	void AddRef();
	void Release();

	D3DXMATRIX m_d3dxmtxWorld;         
	CMesh *m_pMesh;

	virtual void SetMesh(CMesh *pMesh);
	virtual void Animate(float fTimeElapsed);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);
	virtual void SetScale(D3DXVECTOR3 _size);
	virtual void SetRotation(float _rot);
	virtual void SetPosition(D3DXVECTOR3 _pos);
	virtual void SetNewDestination ( D3DXVECTOR3 _pos );
	void Update(float _fMoveIncrement);

	bool InMotion();

	D3DXVECTOR3 GetPosition() { return m_Pos; }
};



class CRotatingObject : public CGameObject
{
public:
	float a;
	CRotatingObject();
	virtual ~CRotatingObject();

	virtual void Animate(float fTimeElapsed);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);
};
