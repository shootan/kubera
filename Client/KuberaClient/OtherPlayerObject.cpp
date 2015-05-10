#include "OtherPlayerObject.h"
#include "MissileManager.h"
OtherPlayerObject::OtherPlayerObject(void)
{
	CGameObject::CGameObject();

	m_iTag = OTHERPLAYER;
	m_Visible = FALSE;
	m_ID = 0;
	m_bSetDestination = FALSE;
	 m_iTargetID = 0;

	m_fAttackTime = 0.0f;
}

OtherPlayerObject::~OtherPlayerObject(void)
{
	CGameObject::~CGameObject();
}

void OtherPlayerObject::Render(ID3D11DeviceContext *pd3dDeviceContext)
{
	CGameObject::Render(pd3dDeviceContext);
}

void OtherPlayerObject::SetNewDestination ( D3DXVECTOR3 _pos ) {
	if(_pos == m_Pos) return;
	Vector3 f_pos;
	Vector3 s_pos;
	ZeroMemory(&f_pos, sizeof(Vector3));
	ZeroMemory(&s_pos, sizeof(Vector3));
	f_pos.x = m_Pos.x;
	f_pos.z = m_Pos.z;
	s_pos.x = _pos.x;
	s_pos.z = _pos.z;

	float distance = ST::sharedManager()->GetDistance(f_pos, s_pos);
	if(distance > 40.0f)
	{
		m_Pos.x = _pos.x;
		m_Pos.y = _pos.y;
		m_Pos.z = _pos.z;
		return;
	}

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

bool OtherPlayerObject::InMotion()
{	
	if(m_Pos.x == m_vDestination.x && m_Pos.z == m_vDestination.z) return false;
	else return true;
}

void OtherPlayerObject::Update(float fTimeElapsed)
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

void OtherPlayerObject::Animate(float fTimeElapsed)
{
	if(m_iState == ATTACK)
	{
		if(m_pTarget == NULL) return;
		m_fAttackTime += fTimeElapsed;

		for(int i=0; i<MAX_MISSILE; i++)
		{
			if(MissileManager::sharedManager()->m_pMissile[i]->GetUsed() == TRUE) continue;

			if(m_fAttackTime >= 0.5f)
			{
				MissileManager::sharedManager()->m_pMissile[i]->SetPosition(m_Pos);
				MissileManager::sharedManager()->m_pMissile[i]->SetUsed(TRUE);
				MissileManager::sharedManager()->m_pMissile[i]->SetTarget(m_pTarget);

				m_fAttackTime = 0.f;
			}
		}
	}
}