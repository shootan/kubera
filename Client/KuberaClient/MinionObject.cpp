#include "MinionObject.h"

MinionObject::MinionObject(void)
{
	CGameObject::CGameObject();
	m_Visible		 = FALSE;
	m_Pos			 = D3DXVECTOR3(1000, 0, 0);
	m_vDestination	 = D3DXVECTOR3(1000, 0, 0);
	m_iTag = MINION;

}

MinionObject::~MinionObject(void)
{
	CGameObject::~CGameObject();
}

void MinionObject::Render(ID3D11DeviceContext *pd3dDeviceContext)
{
	CGameObject::Render(pd3dDeviceContext);
}



void MinionObject::SetNewDestination ( D3DXVECTOR3 _pos ) {

	m_vDestination.x = _pos.x;
	m_vDestination.y = _pos.y;
	m_vDestination.z = _pos.z;       
	m_vWalkIncrement = m_vDestination - m_Pos;
	D3DXVec3Normalize ( &m_vWalkIncrement, &m_vWalkIncrement );
	//m_pBestWay = NULL;
	//m_bFindPath = FALSE;
	//if(m_pAstar != NULL)
	//	delete m_pAstar;

	//m_pAstar = new Astar();

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
	//if(!m_bAstar)
	//{
	//	if ( m_Pos.x == m_vDestination.x && m_Pos.z == m_vDestination.z ) return false;
	//	else return true;
	//}
	//else if(m_bAstar)
	//{
	//	if(m_pBestWay == NULL)
	//		return false;

	//	if ( m_Pos.x == m_pBestWay->x && m_Pos.z == m_pBestWay->y ) return false;
	//	else return true;
	//}
	//else
		return false;
}

void MinionObject::Update(float fTimeElapsed)
{
	//if(m_Visible == FALSE) return;

	//if(!m_bAstar)
	//{
	//	if ( InMotion() && m_iTag == MINION )
	//	{
	//		D3DXVECTOR3 update_delta = m_vWalkIncrement *2.0f;
	//		D3DXVECTOR3 location_vector = m_vDestination - m_Pos;

	//		m_Pos += update_delta;

	//		float finished = D3DXVec3Dot( &m_vWalkIncrement, &location_vector );
	//		if ( finished < 0.0f ) m_Pos = m_vDestination;
	//	}
	//}
	//else if(m_bAstar)
	//{
	//	if(m_pAstar == NULL)
	//		return;

	//	if(m_bFindPath == FALSE)
	//	{
	//		m_pBestWay = m_pAstar->find_path(m_Pos.x, m_Pos.z, m_vDestination.x, m_vDestination.z);
	//		m_bFindPath = TRUE;
	//	}

	//	if ( InMotion() && m_iTag == MINION ) {

	//		D3DXVECTOR3 update_delta = m_vWalkIncrement * 2.0f;
	//		D3DXVECTOR3 location_vector = D3DXVECTOR3(m_pBestWay->x - m_Pos.x, 0, m_pBestWay->y - m_Pos.z);

	//		//D3DXVECTOR3 location_vector = m_vDestination - m_Pos;

	//		m_Pos += location_vector;

	//		//determine  if we've moved past our target ( so we can stop ).
	//		float finished = D3DXVec3Dot( &m_vWalkIncrement, &location_vector );
	//		//if ( finished < 0.0f ) m_Pos = m_vDestination;
	//		if ( finished < 0.0f ) m_Pos = D3DXVECTOR3(m_pBestWay->x, 0, m_pBestWay->y);
	//	}
	//	else
	//	{
	//		if(m_pBestWay == NULL)
	//			return;
	//		m_pBestWay = m_pBestWay->prev_node;
	//	}
	//}

}

void MinionObject::Animate(float fTimeElapsed)
{
}