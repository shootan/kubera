#include "EnemyObject.h"

EnemyObject::EnemyObject(void)
{
	CGameObject::CGameObject();

	m_iTag = OTHERPLAYER;
	m_Visible = FALSE;
	m_ID = 0;
	m_bSetDestination = FALSE;
}

EnemyObject::~EnemyObject(void)
{
	CGameObject::~CGameObject();
}


void EnemyObject::Render(ID3D11DeviceContext *pd3dDeviceContext)
{
	CGameObject::Render(pd3dDeviceContext);
}

void EnemyObject::SetNewDestination ( D3DXVECTOR3 _pos ) {
	if(_pos == m_Pos) return;
	m_vDestination.x = _pos.x;
	m_vDestination.y = _pos.y;
	m_vDestination.z = _pos.z;       
	m_vWalkIncrement = m_vDestination - m_Pos;
	D3DXVec3Normalize ( &m_vWalkIncrement, &m_vWalkIncrement );
	//m_bFindPath = FALSE;


	//// Calculate the rotation angle before. Next, change the walk direction into 
	//// an increment by multiplying by speed.
	/*float fAngle = D3DXVec3Dot( &m_vWalkIncrement, &m_vFacingDirection );
	D3DXVECTOR3 cross;
	D3DXVec3Cross( &cross, &m_vWalkIncrement, &m_vFacingDirection );
	fAngle = acosf( fAngle );
	if ( cross.y >  0.0f ) {
		fAngle *=-1.0f;
	}
	fAngle /= D3DX_PI;
	this->SetRotation(2, 1/fAngle);
*/
	m_vWalkIncrement *= m_fWalkSpeed;
	m_bSetDestination = TRUE;

}

bool EnemyObject::InMotion()
{	
	if(m_Pos.x == m_vDestination.x && m_Pos.z == m_vDestination.z) return false;
	else return true;
}

void EnemyObject::Update(float fTimeElapsed)
{
	if(m_Visible == FALSE) return;
	if ( InMotion() && m_bSetDestination == TRUE)
	{
		D3DXVECTOR3 update_delta = m_vWalkIncrement *5.0f;
		D3DXVECTOR3 location_vector = m_vDestination - m_Pos;

		m_Pos += update_delta;

		Vector3 f_pos;
		Vector3 s_pos;
		f_pos.x = m_Pos.x;
		f_pos.y = m_Pos.y;
		f_pos.z = m_Pos.z;
		s_pos.x = m_vDestination.x;
		s_pos.y = m_vDestination.y;
		s_pos.z = m_vDestination.z;
		float finished = ST::sharedManager()->GetDistance(f_pos, s_pos);

		if ( finished < 0.5f ) 
			m_Pos = m_vDestination;
	}
}

void EnemyObject::Animate(float fTimeElapsed)
{

}