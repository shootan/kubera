#include "GameObject.h"


CGameObject::CGameObject(void)
{
	D3DXMatrixIdentity(&m_d3dxmtxWorld);
	
	m_pMesh = NULL;
	m_Pos = D3DXVECTOR3(0, 0, 0);
	m_vDestination = m_Pos;
	m_Scale = D3DXVECTOR3(1,1,1);
	m_Rot = 1.0f;
	m_axis = 2;

	m_vFacingDirection= D3DXVECTOR3(0,0,1);

	m_nReferences = 1;

	m_Visible = TRUE;
	m_fWalkSpeed= 0.1f;

	m_iTag = NONE;

	BoundsizeX = 0.0f;
	BoundsizeY = 0.0f;
	BoundsizeZ = 0.0f;

	m_pAstar = NULL;
	m_pBestWay = NULL;
	m_bAstar = FALSE;
	m_bFindPath = FALSE;
}


CGameObject::~CGameObject(void)
{
	if (m_pMesh) m_pMesh->Release();
}

void CGameObject::AddRef() 
{ 
	m_nReferences++; 
}

void CGameObject::Release() 
{ 
	if (m_nReferences > 0) m_nReferences--; 
	if (m_nReferences <= 0) delete this;
}

void CGameObject::SetMesh(CMesh *pMesh)
{
	if (m_pMesh) m_pMesh->Release();
	m_pMesh = pMesh;
	if (m_pMesh) m_pMesh->AddRef();
}

void CGameObject::Render(ID3D11DeviceContext *pd3dDeviceContext)
{
	//if(m_Visible != TRUE) return;

	D3DXMATRIX mWorld;
	D3DXMatrixIdentity(&mWorld);

	//S
	D3DXMATRIX mtxScale;
	D3DXMatrixIdentity(&mtxScale);

	D3DXMatrixScaling(&mtxScale, m_Scale.x, m_Scale.y, m_Scale.z);
	//mWorld *= mtxScale;

	//R
	D3DXMATRIX mtxRotate;
	D3DXMatrixIdentity(&mtxRotate);

	if(m_axis == 1)
		D3DXMatrixRotationX(&mtxRotate, D3DX_PI / m_Rot);
	else if(m_axis == 2)
		D3DXMatrixRotationY(&mtxRotate, D3DX_PI / m_Rot);
	else if(m_axis == 3)
		D3DXMatrixRotationZ(&mtxRotate, D3DX_PI / m_Rot);

	mWorld *= mtxRotate;

	//T
	D3DXMATRIX mtxTrans;
	D3DXMatrixIdentity(&mtxTrans);

	D3DXMatrixTranslation(&mtxTrans, m_Pos.x, m_Pos.y, m_Pos.z);
	//mWorld *= mtxTrans;

	mWorld = mtxScale * mtxRotate * mtxTrans;
	m_d3dxmtxWorld = mWorld;



	if (m_pMesh) m_pMesh->Render(pd3dDeviceContext);
}

void CGameObject::SetScale(D3DXVECTOR3 _size)
{
	m_Scale = _size;
}

void CGameObject::SetRotation(int xyz, float _rot)
{
	m_axis = xyz;
	m_Rot = _rot;
}

void CGameObject::SetPosition(D3DXVECTOR3 _pos)
{
	m_Pos = _pos;
	m_vDestination = _pos;
}


Vector3 CGameObject::GetPos()
{
	Vector3 v;
	v.x = m_Pos.x;
	v.y = m_Pos.y;
	v.z = m_Pos.z;

	return v;
}

Vector3 CGameObject::GetScale()
{
	Vector3 v;
	v.x = m_Scale.x;
	v.y = m_Scale.y;
	v.z = m_Scale.z;

	return v;
}

float CGameObject::GetRot()
{
	return m_Rot;
}

void CGameObject::SetPos(Vector3 v)
{
	D3DXVECTOR3 vec;

	vec.x = v.x;
	vec.y = v.y;
	vec.z = v.z;

	m_Pos.x = vec.x;
	m_Pos.y = vec.y;
	m_Pos.z = vec.z;
}

void CGameObject::SetScale(Vector3 v)
{
	D3DXVECTOR3 vec;

	vec.x = v.x;
	vec.y = v.y;
	vec.z = v.z;

	m_Scale.x = vec.x;
	m_Scale.y = vec.y;
	m_Scale.z = vec.z;
}

void CGameObject::SetRot(float f)
{
	m_Rot = f;
}

void CGameObject::SetBoundSize(float x, float y, float z)
{
	BoundsizeX = x;
	BoundsizeY = y;
	BoundsizeZ = z;
}

void CGameObject::SetP(float x, float y, float z)
{
	m_d3dxmtxWorld._41 = x; 
	m_d3dxmtxWorld._42 = y; 
	m_d3dxmtxWorld._43 = z; 
}
