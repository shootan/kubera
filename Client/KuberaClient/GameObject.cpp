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

	astar = NULL;
	best = NULL;
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

void CGameObject::Animate(float fTimeElapsed)
{
}

void CGameObject::Render(ID3D11DeviceContext *pd3dDeviceContext)
{
	if(m_Visible != TRUE) return;

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
	best = NULL;
	m_bFindPath = FALSE;
	if(astar != NULL)
		delete astar;

	astar = new Astar();

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
	if(!m_bAstar)
	{
		if ( m_Pos.x == m_vDestination.x && m_Pos.z == m_vDestination.z ) return false;
		else return true;
	}
	else if(m_bAstar)
	{
		if(best == NULL)
			return false;

		if ( m_Pos.x == best->x && m_Pos.z == best->y ) return false;
		else return true;
	}
}

void CGameObject::Update(float fTimeElapsed)
{
	if(!m_bAstar)
	{
		if ( InMotion() && m_iTag == HERO )
		{
			D3DXVECTOR3 update_delta = m_vWalkIncrement * 5.0f;
			D3DXVECTOR3 location_vector = m_vDestination - m_Pos;

			m_Pos += update_delta;

			float finished = D3DXVec3Dot( &m_vWalkIncrement, &location_vector );
			if ( finished < 0.0f ) m_Pos = m_vDestination;
		}
	}
	else if(m_bAstar)
	{
		if(astar == NULL)
			return;

		if(m_bFindPath == FALSE)
		{
			best = astar->find_path(m_Pos.x, m_Pos.z, m_vDestination.x, m_vDestination.z);
			m_bFindPath = TRUE;
		}

		if ( InMotion() && m_iTag == HERO ) {

			D3DXVECTOR3 update_delta = m_vWalkIncrement * 5.0f;
			D3DXVECTOR3 location_vector = D3DXVECTOR3(best->x - m_Pos.x, 0, best->y - m_Pos.z);

			//D3DXVECTOR3 location_vector = m_vDestination - m_Pos;

			m_Pos += location_vector;

			//determine  if we've moved past our target ( so we can stop ).
			float finished = D3DXVec3Dot( &m_vWalkIncrement, &location_vector );
			//if ( finished < 0.0f ) m_Pos = m_vDestination;
			if ( finished < 0.0f ) m_Pos = D3DXVECTOR3(best->x, 0, best->y);
		}
		else
		{
			if(best == NULL)
				return;
			best = best->prev_node;
		}
	}

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