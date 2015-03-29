#pragma once
#include "Mesh.h"
#include "header.h"

class CGameObject
{
public:
	CGameObject(void);
	~CGameObject(void);

private:
	//
	int			m_ID;
	BOOL		m_Visible;
	int			m_iTag;

	//
	int m_nReferences;
	D3DXVECTOR3 m_Pos;
	D3DXVECTOR3 m_Scale;
	FLOAT		m_Rot;
	int			m_axis;
	D3DXVECTOR3 m_vDestination;
	D3DXVECTOR3 m_vWalkIncrement;
	D3DXVECTOR3 m_vFacingDirection;

	float		m_fWalkSpeed;

	float BoundsizeX, BoundsizeY, BoundsizeZ;

public:
	void AddRef();
	void Release();

	D3DXMATRIX m_d3dxmtxWorld;         
	CMesh *m_pMesh;

	virtual void SetMesh(CMesh *pMesh);
	virtual void Animate(float fTimeElapsed);
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);
	virtual void SetScale(D3DXVECTOR3 _size);
	virtual void SetRotation(int xyz, float _rot);  // x =1 y = 2 z =3
	virtual void SetPosition(D3DXVECTOR3 _pos);
	virtual void SetNewDestination ( D3DXVECTOR3 _pos );
	void Update(float _fMoveIncrement);
	Vector3 GetPos();
	Vector3 GetScale();
	float GetRot();
	void SetPos(Vector3 v);
	void SetScale(Vector3 v);
	void SetRot(float f);

	bool InMotion();

	int	GetID() { return m_ID; }
	void SetID(int _id) { m_ID= _id;}
	void SetVisible(BOOL _vi) { m_Visible  = _vi; }
	BOOL GetVisible() { return m_Visible; }
	int GetTag() { return m_iTag; }
	void SetTag(int _tag) { m_iTag = _tag;}

	D3DXVECTOR3 GetPosition() { return m_Pos; }

	void SetBoundSize(float x, float y, float z);
	float GetBoundSizeX() {return BoundsizeX;}
	float GetBoundSizeY() {return BoundsizeY;}
	float GetBoundSizeZ() {return BoundsizeZ;}
};
