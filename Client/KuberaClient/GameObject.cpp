#include "GameObject.h"


CGameObject::CGameObject(void)
{
	D3DXMatrixIdentity(&m_d3dxmtxWorld);
	
	m_pMesh = NULL;
	m_Pos = D3DXVECTOR3(0, 0, 0);
	m_vDestination = m_Pos;
	m_Scale = D3DXVECTOR3(1,1,1);
	m_Rot = 1.0f;

	m_vFacingDirection= D3DXVECTOR3(0,0,1);

	m_nReferences = 1;

	m_fWalkSpeed= 0.1f;
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

void CGameObject::Animate(float fTimeElapsed)
{
}

void CGameObject::Render(ID3D11DeviceContext *pd3dDeviceContext)
{
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

	//mWorld *= mtxRotate;

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

void CGameObject::SetNewDestination ( D3DXVECTOR3 _pos ) {

	m_vDestination.x = _pos.x;
	m_vDestination.y = _pos.y;
	m_vDestination.z = _pos.z;       
	m_vWalkIncrement = m_vDestination - m_Pos;
	D3DXVec3Normalize ( &m_vWalkIncrement, &m_vWalkIncrement );

	//// Calculate the rotation angle before. Next, change the walk direction into 
	//// an increment by multiplying by speed.
	float fAngle = D3DXVec3Dot( &m_vWalkIncrement, &m_vFacingDirection );
	D3DXVECTOR3 cross;
	D3DXVec3Cross( &cross, &m_vWalkIncrement, &m_vFacingDirection );
	fAngle = acosf( fAngle );
	if ( cross.y >  0.0f ) {
		fAngle *=-1.0f;
	}
	fAngle /= D3DX_PI;
	this->SetRotation(2, 1/fAngle);

	m_vWalkIncrement *= m_fWalkSpeed;       

}

bool CGameObject::InMotion()
{
	if ( m_Pos.x == m_vDestination.x && m_Pos.y == m_vDestination.y ) return false;
	else return true;
}

void CGameObject::Update(float _fMoveIncrement)
{
	if ( InMotion() ) {
		D3DXVECTOR3 update_delta = m_vWalkIncrement * _fMoveIncrement;
		D3DXVECTOR3 location_vector = m_vDestination - m_Pos;
		m_Pos += update_delta;
		//determine  if we've moved past our target ( so we can stop ).
		float finished = D3DXVec3Dot( &m_vWalkIncrement, &location_vector );
		if ( finished < 0.0f ) m_Pos = m_vDestination;
	}
}

CRotatingObject::CRotatingObject()
{
	//this->SetRotation(2, 180.0f);
}

CRotatingObject::~CRotatingObject()
{
}

void CRotatingObject::Animate(float fTimeElapsed)
{

}

void CRotatingObject::Render(ID3D11DeviceContext *pd3dDeviceContext)
{
	CGameObject::Render(pd3dDeviceContext);
}

