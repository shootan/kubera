#include "MinionObject.h"
#include "ST.h"

MinionObject::MinionObject(void)
{
	CGameObject::CGameObject();
	m_Visible		 = FALSE;
	m_Pos			 = D3DXVECTOR3(1000, 0, 0);
	m_vDestination	 = D3DXVECTOR3(1000, 0, 0);
	m_iTag = MINION;

	m_bSetDestination = FALSE;

}

MinionObject::~MinionObject(void)
{
	CGameObject::~CGameObject();
}

void MinionObject::Render(ID3D11DeviceContext *pd3dDeviceContext)
{
	CGameObject::Render(pd3dDeviceContext);
	//if(m_Visible != TRUE) return;

	//if(m_iTag == OTHERPLAYER)
	//{
	//	int asdfdf;
	//	asdfdf = 0;
	//}
	//D3DXMATRIX mWorld;
	//D3DXMatrixIdentity(&mWorld);

	////S
	//D3DXMATRIX mtxScale;
	//D3DXMatrixIdentity(&mtxScale);

	//D3DXMatrixScaling(&mtxScale, m_Scale.x, m_Scale.y, m_Scale.z);
	////mWorld *= mtxScale;

	////R
	//D3DXMATRIX mtxRotate;
	//D3DXMatrixIdentity(&mtxRotate);

	//if(m_axis == 1)
	//	D3DXMatrixRotationX(&mtxRotate, D3DX_PI / m_Rot);
	//else if(m_axis == 2)
	//	D3DXMatrixRotationY(&mtxRotate, D3DX_PI / m_Rot);
	//else if(m_axis == 3)
	//	D3DXMatrixRotationZ(&mtxRotate, D3DX_PI / m_Rot);

	//mWorld *= mtxRotate;

	////T
	//D3DXMATRIX mtxTrans;
	//D3DXMatrixIdentity(&mtxTrans);

	//D3DXMatrixTranslation(&mtxTrans, m_Pos.x, m_Pos.y, m_Pos.z);
	////mWorld *= mtxTrans;

	//mWorld = mtxScale * mtxRotate * mtxTrans;
	//m_d3dxmtxWorld = mWorld;
}



void MinionObject::SetNewDestination ( D3DXVECTOR3 _pos ) {
	if(m_Visible == FALSE) return;
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
	m_vDestination.z = _pos.z;       
	m_vWalkIncrement = m_vDestination - m_Pos;
	D3DXVec3Normalize ( &m_vWalkIncrement, &m_vWalkIncrement );
	//m_bFindPath = FALSE;


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
	m_bSetDestination = TRUE;

}

bool MinionObject::InMotion()
{	
	if(m_Pos.x == m_vDestination.x && m_Pos.z == m_vDestination.z) return false;
	else return true;
}

void MinionObject::Update(float fTimeElapsed)
{
	if(m_Visible == FALSE)
	{
		m_Pos.x = -1200;
		m_Pos.y = 0;
		m_Pos.z = -1200;
		return;
	}
	if ( InMotion() && m_bSetDestination == TRUE)
	{
		D3DXVECTOR3 update_delta = m_vWalkIncrement *7.0f;
		D3DXVECTOR3 location_vector = m_vDestination - m_Pos;

		m_Pos += update_delta;

		Vector3 f_pos;
		Vector3 s_pos;
		ZeroMemory(&f_pos, sizeof(Vector3));
		ZeroMemory(&s_pos, sizeof(Vector3));

		f_pos.x = m_Pos.x;
		f_pos.z = m_Pos.z;
		s_pos.x = m_vDestination.x;
		s_pos.z = m_vDestination.z;
		float finished = ST::sharedManager()->GetDistance(f_pos, s_pos);

		if ( finished < 0.5f ) 
			m_Pos = m_vDestination;
	}
}


void MinionObject::Animate(float fTimeElapsed)
{
}