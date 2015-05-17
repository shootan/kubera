#include "MissileObject.h"

MissileObject::MissileObject(void)
{
	CGameObject::CGameObject();

	m_iTag = MISSILE;
	m_vFacingDirection= D3DXVECTOR3(0,0,-1);
	m_Visible = TRUE;
	m_bUsed = FALSE;
	m_pTarget = NULL;
	m_pAttacker  = NULL;
	m_fWalkSpeed = 0.15f;
}

MissileObject::~MissileObject(void)
{
	CGameObject::~CGameObject();
}

void MissileObject::SetNewDestination ( D3DXVECTOR3 _pos ) {

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

void MissileObject::Render(ID3D11DeviceContext *pd3dDeviceContext)
{
	if(m_Visible != TRUE) return;

	if(m_iTag == OTHERPLAYER)
	{
		int asdfdf;
		asdfdf = 0;
	}
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
}

void MissileObject::Update(float fTimeElapsed)
{
	if(m_bUsed == FALSE) return;
	if(m_pTarget != NULL) SetNewDestination(m_pTarget->GetPosition() + D3DXVECTOR3(0 , m_pTarget->GetBoundSizeY()/2, 0));


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
	{
		if(m_pTarget->GetTag() != OTHERPLAYER) m_pTarget->SetDamage(m_pAttacker->GetDamage());
		m_Pos = D3DXVECTOR3(1200, 0 ,0);
		m_bUsed = FALSE;
		m_pTarget = NULL;
	}
}