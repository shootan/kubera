#include "MinionObject.h"

MinionObject::MinionObject(void)
{
	CGameObject::CGameObject();
	m_Visible = FALSE;
}

MinionObject::~MinionObject(void)
{
	CGameObject::~CGameObject();
}

void MinionObject::AddRef() 
{ 
	m_nReferences++; 
}

void MinionObject::Release() 
{ 
	if (m_nReferences > 0) m_nReferences--; 
	if (m_nReferences <= 0) delete this;
}

void MinionObject::Render(ID3D11DeviceContext *pd3dDeviceContext)
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



void MinionObject::SetNewDestination ( D3DXVECTOR3 _pos ) {

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

bool MinionObject::InMotion()
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
	else
		return false;
}

void MinionObject::Update(float fTimeElapsed)
{
	if(m_Visible == FALSE)
		return;

	if(!m_bAstar)
	{
		if ( InMotion() && m_iTag == MINION )
		{
			D3DXVECTOR3 update_delta = m_vWalkIncrement *2.0f;
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

		if ( InMotion() && m_iTag == MINION ) {

			D3DXVECTOR3 update_delta = m_vWalkIncrement * 2.0f;
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

void MinionObject::Animate(float fTimeElapsed)
{
}