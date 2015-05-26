#include "HeroObject.h"

HeroObject::HeroObject(void)
{
	CGameObject::CGameObject();

	m_pAniMesh = NULL;

	m_iTag = HERO;
	m_bMove = FALSE;
	m_pTarget = NULL;
	m_Visible = TRUE;
	m_fAttackTime = 0.f;

	m_pAstar = NULL;
	m_pBestWay = NULL;
	m_bAstar = FALSE;
	m_bFindPath = FALSE;

	//m_HP = 300.0;
	m_Damage = 50.0f;

	m_time = 0.0f;

	m_fWalkSpeed = 25.0f;

	node_t* temp;

	while(m_pBestWay) {
		m_pBestWay = m_pBestWay->next_node;
		free(m_pBestWay);
		m_pBestWay = temp;
	}
}

HeroObject::~HeroObject(void)
{
	CGameObject::~CGameObject();
}


void HeroObject::Render(ID3D11DeviceContext *pd3dDeviceContext)
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

	if(m_time > 180.0f) m_time = 0.0f;

	if (m_pAniMesh) m_pAniMesh->Render(pd3dDeviceContext, m_time);

}

void HeroObject::SetNewDestination ( D3DXVECTOR3 _pos ) {

	m_vDestination.x = _pos.x;
	m_vDestination.y = _pos.y;
	m_vDestination.z = _pos.z;       
	m_vWalkIncrement = m_vDestination - m_Pos;
	D3DXVec3Normalize ( &m_vWalkIncrement, &m_vWalkIncrement );

	m_pBestWay = NULL;
	m_bFindPath = FALSE;
	if(m_pAstar != NULL)
		delete m_pAstar;

	m_pAstar = new Astar();

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

bool HeroObject::InMotion()
{	
	if(!m_bAstar)
	{
		if ( m_Pos.x == m_vDestination.x && m_Pos.z == m_vDestination.z ) return false;
		else return true;
	}
	else if(m_bAstar)
	{
		if(m_pBestWay == NULL)
			return false;

		if ( m_Pos.x == m_pBestWay->x && m_Pos.z == m_pBestWay->y ) return false;
		else return true;
	}
	else
		return false;
}

void HeroObject::Update(float fTimeElapsed)
{
	if(m_Visible == FALSE) return;
	
	//printf("%.0f \n", m_HP);
	if(m_HP < 1.0f)
	{
		Vector3 po;
		if(m_ID % 2 == 0)
		{
			po.x = 510;
			po.y = 0;
			po.z = 0;
		}
		else
		{
			po.y = 0;
			po.x = -510;
			po.z = 0;
		}
		m_HP = 500.0f;
		this->SetPos(po);
	}
	if(m_iState != MOVE) return;

	if(!m_bAstar)
	{
		if ( InMotion() && m_iTag == HERO )
		{
			D3DXVECTOR3 update_delta = m_vWalkIncrement;
			D3DXVECTOR3 location_vector = m_vDestination - m_Pos;

			m_Pos += update_delta * fTimeElapsed;

			float finished = D3DXVec3Dot( &m_vWalkIncrement, &location_vector );
			if ( finished < 0.0f ) 
			{
				m_Pos = m_vDestination;
				m_iState = IDLE;
			}
		}
	}
	else if(m_bAstar)
	{
		if(m_pAstar == NULL)
			return;

		if(m_bFindPath == FALSE)
		{
			node_t* temp;

			while(m_pBestWay) {
				m_pBestWay = m_pBestWay->next_node;
				free(m_pBestWay);
				m_pBestWay = temp;
			}

			m_pBestWay = m_pAstar->find_path(m_Pos.x, m_Pos.z, m_vDestination.x, m_vDestination.z);
			m_pBestWay = m_pBestWay->prev_node;
			m_bFindPath = TRUE; 
		}

		if ( InMotion() && m_iTag == HERO ) {
			if(m_pBestWay == NULL)
				return;

			if(m_bMove == FALSE)
			{
				D3DXVECTOR3 m_vAstarIncrement = D3DXVECTOR3(m_pBestWay->x - m_Pos.x, 0, m_pBestWay->y - m_Pos.z);
				D3DXVec3Normalize ( &m_vAstarIncrement, &m_vAstarIncrement );
				//m_vAstarIncrement *= m_fWalkSpeed; 
				m_bMove = TRUE;
			}
			D3DXVECTOR3 update_delta = m_vAstarIncrement;
			D3DXVECTOR3 location_vector = D3DXVECTOR3(m_pBestWay->x - m_Pos.x, 0, m_pBestWay->y - m_Pos.z);
			//D3DXVECTOR3 location_vector = m_vDestination - m_Pos;

			m_Pos += update_delta;

			////determine  if we've moved past our target ( so we can stop ).
			float finished = D3DXVec3Dot( &update_delta, &location_vector );
			//if ( finished < 0.0f ) m_Pos = m_vDestination;
			if ( finished < 0.0f ) 
			{
				m_Pos = D3DXVECTOR3(m_pBestWay->x, 0, m_pBestWay->y);
				m_pBestWay = m_pBestWay->prev_node;
				m_bMove = FALSE;
			}
		}
		else
		{
			if(m_pBestWay == NULL)
				return;

			m_pBestWay = m_pBestWay->prev_node;
			m_bMove = FALSE;

/*			m_vAstarIncrement = D3DXVECTOR3(m_pBestWay->x - m_Pos.x, 0, m_pBestWay->y - m_Pos.z);
			D3DXVec3Normalize ( &m_vAstarIncrement, &m_vAstarIncrement );
			m_vAstarIncrement *= m_fWalkSpeed;  */ 
		}
	}
}

void HeroObject::Animate(float fTimeElapsed)
{
	m_time += fTimeElapsed * 1.7f;
	printf(" %.3f \n", m_time);
	if(m_iState == IDLE)
	{
		if(m_time < 17.2f) m_time = 17.2f;
		if(m_time > 18.7f) m_time = 17.2f;
	}
	else if(m_iState == ATTACK)
	{
		if(m_pTarget->GetHP() < 1 || ST::sharedManager()->GetDistance(this->GetPos(), m_pTarget->GetPos()) > 40.0f)
		{
			m_pTarget = NULL;
			m_fAttackTime = 0.0f;
			m_iState = IDLE;
			return;
		}
		m_fAttackTime += fTimeElapsed;

		for(int i=0; i<MAX_MISSILE; i++)
		{
			if(MissileManager::sharedManager()->m_pMissile[i]->GetUsed() == TRUE) continue;

			if(m_fAttackTime >= 1.0f)
			{
				MissileManager::sharedManager()->m_pMissile[i]->SetPosition(m_Pos + D3DXVECTOR3(0 , BoundsizeY *2/3, 0));
				MissileManager::sharedManager()->m_pMissile[i]->SetUsed(TRUE);
				MissileManager::sharedManager()->m_pMissile[i]->SetTarget(m_pTarget);
				MissileManager::sharedManager()->m_pMissile[i]->SetAttacker(this);

				m_fAttackTime = 0.f;
			}
		}
	}
	else if(m_iState == MOVE)
	{
		if(m_time < 7.45f) m_time = 7.45f;
		if(m_time > 9.3f ) m_time = 7.45f;
		m_fAttackTime = 0.f;

		if(m_pTarget == NULL) return;
		
		if(ST::sharedManager()->GetDistance(this->GetPos(), m_pTarget->GetPos()) < 40.f && m_pTarget->GetTeam() != this->GetTeam())
			m_iState = ATTACK;
	}
	else if(m_iState == DEATH)
	{
		if(m_time < 22.45f) m_time = 22.45f;
		if(m_time > 25 )
		{
			m_iState = IDLE;
		}
		
	}
}


void HeroObject::SetAniMesh(GFBX::Mesh *pAniMesh)
{
	//if (m_pAniMesh) m_pAniMesh->Release();
	m_pAniMesh = pAniMesh;
	//if (m_pAniMesh) m_pAniMesh->AddRef();
}