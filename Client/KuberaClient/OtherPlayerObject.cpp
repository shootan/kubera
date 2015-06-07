#include "OtherPlayerObject.h"
#include "MissileManager.h"
OtherPlayerObject::OtherPlayerObject(void)
{
	CGameObject::CGameObject();

	m_pAniMesh = NULL;
	m_iTag = OTHERPLAYER;
	m_Visible = FALSE;
	m_ID = 0;
	m_bSetDestination = FALSE;
	m_iTargetID = 0;

	m_fAttackTime = 0.0f;
	m_Time = 0.0f;
	m_fWalkSpeed = 25.0f;

	m_iparticleNum = 0;
	m_bUseParticle = FALSE;
	m_bUseParticleMissile = FALSE;
}

OtherPlayerObject::~OtherPlayerObject(void)
{
	CGameObject::~CGameObject();
}

void OtherPlayerObject::Render(ID3D11DeviceContext *pd3dDeviceContext, float fTimeElapsed)
{
	if(m_Visible != TRUE) return;
	if(m_pAniMesh == NULL) return;

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



	if (m_pAniMesh) m_pAniMesh->Render(pd3dDeviceContext, m_Time);
}

void OtherPlayerObject::SetNewDestination ( D3DXVECTOR3 _pos ) {
	if(_pos == m_Pos) return;
	if(m_iState == ATTACK) return;

	m_vDestination.x = _pos.x;
	m_vDestination.y = _pos.y;
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
		D3DXVECTOR3 update_delta = m_vWalkIncrement;
		D3DXVECTOR3 location_vector = m_vDestination - m_Pos;

		m_Pos += update_delta * fTimeElapsed;

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
	m_Time += fTimeElapsed*2.0f;
	//printf(" %.3f \n", m_Time);
	if(m_iState == IDLE)
	{
		switch(m_iType)
		{
		case KNIGHT:
			if(m_Time < 1.1f) m_Time = 1.1f;
			if(m_Time > 7.0f) m_Time = 1.1f;
			break;
		case WIZARD:
			if(m_Time < 1.1f) m_Time = 1.1f;
			if(m_Time > 7.1f) m_Time = 1.1f;
			break;
		}
	}
	else if(m_iState == ATTACK)
	{
		switch(m_iType)
		{
		case KNIGHT:
			if(m_Time < 41.0f) m_Time = 41.0f;
			if(m_Time > 49.0f) m_Time = 41.0f;
			break;
		case WIZARD:
			if(m_Time < 20.0f) m_Time = 20.0f;
			if(m_Time > 23.0f) m_Time = 20.0f;
			break;
		}

		if(m_pTarget->GetHP() < 1 || ST::sharedManager()->GetDistance(this->GetPos(), m_pTarget->GetPos()) > 40.0f)
		{
			m_pTarget = NULL;
			m_fAttackTime = 0.0f;
			m_iState = IDLE;
			return;
		}
		m_fAttackTime += fTimeElapsed;

		/*
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
		}*/
	}
	else if(m_iState == MOVE)
	{
		if(m_iparticleNum  != 500)
		{
			m_bUseParticle = FALSE;
			m_bUseParticleMissile = FALSE;
			ParticleManager::sharedManager()->m_pParticle[m_iparticleNum]->SetUsed(FALSE);
			ParticleManager::sharedManager()->m_pParticle[m_iparticleNum]->SetTarget(NULL);
			ParticleManager::sharedManager()->m_pParticle[m_iparticleNum]->SetPosition(D3DXVECTOR3(1200, 0, 0));
		}
		switch(m_iType)
		{
		case KNIGHT:
			if(m_Time < 57.5f) m_Time = 57.5f;
			if(m_Time > 59.7f ) m_Time = 57.5f;
			break;
		case WIZARD:
			if(m_Time < 68.0f) m_Time = 68.0f;
			if(m_Time > 70.3f ) m_Time = 68.0f;
			break;
		}
		m_fAttackTime = 0.f;

		if(m_pTarget == NULL) return;
		
		if(ST::sharedManager()->GetDistance(this->GetPos(), m_pTarget->GetPos()) < 40.f && m_pTarget->GetTeam() != this->GetTeam())
			m_iState = ATTACK;
	}
	else if(m_iState == DEATH)
	{
		switch(m_iType)
		{
		case KNIGHT:
			if(m_Time < 30.0f) m_Time = 30.0f;
			if(m_Time > 34.0f)
			{
				m_Time = 1.1f;
				m_iState = IDLE;
			}
			break;
		case WIZARD:
			if(m_Time < 30.0f) m_Time = 30.0f;
			if(m_Time > 34.0f)
			{
				m_Time = 1.1f;
				m_iState = IDLE;
			}

			break;
		}
	}
	else if(m_iState == SKILL1)
	{
		switch(m_iType)
		{
		case KNIGHT:
			if(m_Time < 50.0f)  m_Time = 50.0f;
			if(m_Time > 54.0f) 
			{
				m_Time = 1.1f;
				m_iState = IDLE;
			}
			break;
		case WIZARD:
			if(m_bUseParticle == FALSE)
			{
				for(int i=0; i<MAX_PARTICLE; i++)
				{
					if(ParticleManager::sharedManager()->m_pParticle[i] == NULL) continue;
					if(ParticleManager::sharedManager()->m_pParticle[i]->GetUsed() == TRUE) continue;

					if(ParticleManager::sharedManager()->m_pParticle[i]->GetType() == WIZARD_SKILL_BODY)
					{
						ParticleManager::sharedManager()->m_pParticle[i]->SetUsed(TRUE);
						ParticleManager::sharedManager()->m_pParticle[i]->SetTarget(this);
						m_iparticleNum = i;
						m_bUseParticle = TRUE;
						break;
					}
				}
			}
			if(m_Time < 45.0f) m_Time = 45.0f;
			if(m_Time > 48.5f && m_Time < 49.f)
			{
				if(m_bUseParticleMissile == FALSE)
				{
					for(int i=0; i<MAX_PARTICLE; i++)
					{
						if(ParticleManager::sharedManager()->m_pParticle[i] == NULL) continue;
						if(ParticleManager::sharedManager()->m_pParticle[i]->GetUsed() == TRUE) continue;
						if(ParticleManager::sharedManager()->m_pParticle[i]->GetType() == WIZARD_SKILL_MISSILE)
						{
							ParticleManager::sharedManager()->m_pParticle[i]->SetUsed(TRUE);
							ParticleManager::sharedManager()->m_pParticle[i]->SetTarget(this);
							D3DXVec3Normalize ( &m_vWalkIncrement, &m_vWalkIncrement );
							ParticleManager::sharedManager()->m_pParticle[i]->SetDirection(m_vWalkIncrement);
							ParticleManager::sharedManager()->m_pParticle[i]->SetPosition(D3DXVECTOR3(m_Pos.x, m_Pos.y + 10, m_Pos.z) + m_vWalkIncrement * 30);
							m_bUseParticleMissile = TRUE;
							break;
						}
					}
				}
			}
			if(m_Time > 49.5f)
			{
				m_bUseParticle = FALSE;
				m_bUseParticleMissile = FALSE;
				ParticleManager::sharedManager()->m_pParticle[m_iparticleNum]->SetUsed(FALSE);
				ParticleManager::sharedManager()->m_pParticle[m_iparticleNum]->SetTarget(NULL);
				ParticleManager::sharedManager()->m_pParticle[m_iparticleNum]->SetPosition(D3DXVECTOR3(1200, 0, 0));
				m_iparticleNum = 500;
				m_Time = 1.1f;
				m_iState = IDLE;
			}

			break;
		}

	}
	
}


void OtherPlayerObject::SetAniMesh(GFBX::Mesh *pAniMesh)
{
	//if (m_pAniMesh) m_pAniMesh->Release();
	m_pAniMesh = pAniMesh;
	//if (m_pAniMesh) m_pAniMesh->AddRef();
}