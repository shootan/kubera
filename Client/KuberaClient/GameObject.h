#pragma once
#include "Mesh.h"
#include "header.h"
//#include "astarclass.h"

class CGameObject
{
public:
	CGameObject(void);
	~CGameObject(void);

protected:
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

	BOOL		 m_bSelected;
	//BOOL m_bAstar;
	//BOOL m_bFindPath;

public:
	void AddRef();
	void Release();

	D3DXMATRIX m_d3dxmtxWorld;         
	CMesh *m_pMesh;
	//Astar* m_pAstar;
	//node_t* m_pBestWay;

	void SetMesh(CMesh *pMesh);
	virtual void Animate(float fTimeElapsed){}
	virtual void Render(ID3D11DeviceContext *pd3dDeviceContext);
	virtual void SetNewDestination ( D3DXVECTOR3 _pos ){}
	virtual void Update(float fTimeElapsed){}

	void SetScale(D3DXVECTOR3 _size);
	void SetRotation(int xyz, float _rot);  // x =1 y = 2 z =3
	void SetPosition(D3DXVECTOR3 _pos);
	D3DXVECTOR3 GetPosition() { return m_Pos; }

	void SetP(float x, float y, float z);


	Vector3 GetPos();
	Vector3 GetScale();
	float GetRot();
	void SetPos(Vector3 v);
	void SetScale(Vector3 v);
	void SetRot(float f);


	int	GetID() { return m_ID; }
	void SetID(int _id) { m_ID= _id;}
	void SetVisible(BOOL _vi) { m_Visible  = _vi; }
	BOOL GetVisible() { return m_Visible; }
	int GetTag() { return m_iTag; }
	void SetTag(int _tag) { m_iTag = _tag;}

	void SetBoundSize(float x, float y, float z);
	float GetBoundSizeX() {return BoundsizeX;}
	float GetBoundSizeY() {return BoundsizeY;}
	float GetBoundSizeZ() {return BoundsizeZ;}
//	void SetAstar(BOOL _astar) {m_bAstar = _astar;}
//	BOOL GetAstar() { return m_bAstar; }

	void SetSelected(BOOL _selected) { m_bSelected = _selected; }
	BOOL GetSelected() { return m_bSelected; }
};
