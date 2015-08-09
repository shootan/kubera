#include "OtherPlayerObject.h"
#include "MissileManager.h"
#include "Shader.h"

OtherPlayerObject::OtherPlayerObject(void)
{
	CGameObject::CGameObject();

	m_iTag = OTHERPLAYER;
	m_Visible = TRUE;
	m_ID = 0;
	m_PrevState = IDLE;
	m_bSetDestination = FALSE;
	m_iTargetID = 0;
	m_bWarriorAttack = TRUE;
	m_bWarriorSkill = TRUE;

	m_fAttackTime = 0.0f;
	m_Time = 0.0f;
	m_fRespawnTime = 0.0f;
	m_fWalkSpeed = 25.0f;

	m_iparticleNum = 500;
	m_bUseParticle = FALSE;
	m_bUseParticleMissile = FALSE;
	m_bUseParticleAttack = FALSE;


	//초기 정보값
	m_Level = 1;			//레벨1
	m_HP = 100.0f;				//hp 100
	m_PrevHP = m_HP;
	m_Defense = 3;			//방어력 3
	m_fWalkSpeed = 30.0f;	//스피드 15
	m_Damage = 10.0f;		//데미지 10
	m_SkillDamage = 20 + m_Damage;		//스킬데미지 20

	m_nDeathCount = 0;
}

OtherPlayerObject::~OtherPlayerObject(void)
{
	CGameObject::~CGameObject();
}

void OtherPlayerObject::Render(ID3D11DeviceContext *pd3dDeviceContext, float fTimeElapsed, CCamera *pCamera)
{
	if (m_pMaterial) CMaterialShader::UpdateMaterialShaderVariable(pd3dDeviceContext, &m_pMaterial->m_Material);

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


	if (m_pAniMesh)
	{
		bool bIsVisible = true;

		for(int i=0; i<m_pAniMesh->GetSubsetCount(); i++)
		{
			if (pCamera)
			{
				AABB bcBoundingCube = m_pAniMesh->GetSubset(i)->GetBoundingCube();
				bcBoundingCube.Update(&m_d3dxmtxWorld);
				bIsVisible = pCamera->IsInFrustum(&bcBoundingCube);

				if (bIsVisible) break;
			}
		}

		if (bIsVisible) m_pAniMesh->Render(pd3dDeviceContext, m_Time);
	}
}

void OtherPlayerObject::SetNewDestination ( D3DXVECTOR3 _pos ) {
	if(_pos == m_Pos) return;
	
	if(m_iState == ATTACK || m_iState == DEATH) return;

	Vector3 f_pos;
	Vector3 s_pos;
	f_pos.x = m_Pos.x;
	f_pos.y = m_Pos.y;
	f_pos.z = m_Pos.z;
	s_pos.x = _pos.x;
	s_pos.y = _pos.y;
	s_pos.z = _pos.z;
	float finished = ST::sharedManager()->GetDistance(f_pos, s_pos);

	if(finished > 50.0f)
	{
		this->m_Pos = _pos;
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
// 	float fAngle = D3DXVec3Dot( &m_vWalkIncrement, &m_vFacingDirection );
// 	D3DXVECTOR3 cross;
// 	D3DXVec3Cross( &cross, &m_vWalkIncrement, &m_vFacingDirection );
// 	fAngle = acosf( fAngle );
// 	if ( cross.y >  0.0f ) {
// 		fAngle *=-1.0f;
// 	}
// 	fAngle /= D3DX_PI;
// 	this->SetRotation(2, 1/fAngle);

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
	printf("1 : X: %d, Z:%d, STATE : %d, ROT : %.3f \n", m_Pos.x, m_Pos.z, m_iState, m_Rot);
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
	m_Time += fTimeElapsed*1.5f;
	//printf("STATE : %d \n", m_iState);
	switch(m_iState)
	{
	case IDLE:
		switch(m_iType)
		{
		case KNIGHT:
			if(m_Time < 16.8f) m_Time = 16.8f;
			if(m_Time > 21.3f) m_Time = 16.8f;
			break;
		case WIZARD:
			if(m_Time < 6.9f) m_Time = 6.9f;
			if(m_Time > 11.7f) m_Time = 6.9f; 
			break;
		}
		break;
	case MOVE:
		if(m_iparticleNum  != 500)
		{
			m_bUseParticle = FALSE;
			m_bUseParticleMissile = FALSE;
			ParticleManager::sharedManager()->m_pParticle[m_iparticleNum]->SetUsed(FALSE);
			ParticleManager::sharedManager()->m_pParticle[m_iparticleNum]->SetTarget(NULL);
			ParticleManager::sharedManager()->m_pParticle[m_iparticleNum]->SetPosition(D3DXVECTOR3(0, 0, -2000));
			m_iparticleNum = 500;
		}
		switch(m_iType)
		{
		case KNIGHT:
// 			if(m_pTarget != NULL && 
// 				ST::sharedManager()->GetDistance(this->GetPos(), m_pTarget->GetPos()) < 25.f && m_pTarget->GetTeam() != this->GetTeam())
				//m_iState = ATTACK;

			m_bWarriorAttack = TRUE;
			m_bWarriorSkill = TRUE;

			if(m_Time < 57.5f) m_Time = 57.5f;
			if(m_Time > 59.7f ) m_Time = 57.5f;
			break;
		case WIZARD:
// 			if(m_pTarget != NULL && 
// 				ST::sharedManager()->GetDistance(this->GetPos(), m_pTarget->GetPos()) < 50.f && m_pTarget->GetTeam() != this->GetTeam())
				//m_iState = ATTACK;

			m_bUseParticleAttack = FALSE;
			m_bUseParticleMissile = FALSE;

			if(m_Time < 0.1f) m_Time = 0.1f;
			if(m_Time > 1.7f) m_Time = 0.1f; 
			break;
		}
		m_fAttackTime = 0.f;
		break;
	case DEATH:
		switch(m_iType)
		{
		case KNIGHT:
			if(m_Time < 6.9f) m_Time = 6.9f;
			if(m_Time > 11.9f) m_Time = 6.9f;
			if(m_Time > 11.8f && m_Time < 11.9f)
			{
				m_Time = 1.1f;
				//m_iState = IDLE;
				m_nDeathCount++;
			}
			break;
		case WIZARD:
			if(m_Time < 31.5f) m_Time = 31.5f;
			if(m_Time > 36.3f) m_Time = 31.5f;
			if(m_Time > 36.2f && m_Time < 36.3f)
			{
				m_Time = 1.1f;
				//m_iState = IDLE;
				m_nDeathCount++;
			}

			break;
		}
		break;
	case ATTACK:
		switch(m_iType)
		{
		case KNIGHT:
			if(m_Time < 26.7f) m_Time = 26.7f;
			if(m_Time > 29.0f)
			{
				m_bWarriorAttack = TRUE;
				m_Time = 26.7f;
			}
			if(m_Time > 28.0f && m_bWarriorAttack)
			{
				m_pTarget->SetAttackDamage(this->m_Damage - m_pTarget->GetDefense());
				m_pTarget->SetAttacker(this);

				m_bWarriorAttack = FALSE;
			}
			break;
		case WIZARD:
			if(m_Time < 24.5f) m_Time = 24.5f;
			if(m_Time > 26.9f)
			{
				m_Time = 24.5f; 
				m_bUseParticleAttack = FALSE;
			}
			if(m_Time > 26.0f && m_Time < 26.1f)
			{
				if(m_bUseParticleAttack == FALSE)
				{
					for(int i=0; i<MAX_PARTICLE; i++)
					{
						if(ParticleManager::sharedManager()->m_pParticle[i] == NULL) continue;
						if(ParticleManager::sharedManager()->m_pParticle[i]->GetUsed() == TRUE) continue;

						if(ParticleManager::sharedManager()->m_pParticle[i]->GetType() == WIZARD_ATTACK)
						{
							D3DXVec3Normalize ( &m_vWalkIncrement, &m_vWalkIncrement );
							ParticleManager::sharedManager()->m_pParticle[i]->SetPosition(m_Pos + D3DXVECTOR3(0 , BoundsizeY *2/3, 0) + m_vWalkIncrement*15);
							ParticleManager::sharedManager()->m_pParticle[i]->SetUsed(TRUE);
							ParticleManager::sharedManager()->m_pParticle[i]->SetTarget(m_pTarget);
							ParticleManager::sharedManager()->m_pParticle[i]->SetAttacker(this);
							m_bUseParticleAttack = TRUE;
							break;
						}
					}
				}
			}
			break;
		}

		if(m_pTarget->GetHP() <= 0  || ST::sharedManager()->GetDistance(this->GetPos(), m_pTarget->GetPos()) > 50.0f)
		{
			m_pTarget = NULL;

			//m_iState = IDLE;
			return;
		}
		break;
	case WAIT:
		break;
		
	case SKILL1:
		switch(m_iType)
		{
		case KNIGHT:
			if(m_pTarget) SetWatchTarget(m_pTarget->GetPosition());

			if(m_Time < 34.3f) m_Time = 34.3f;
			if(m_Time > 37.0f) m_Time = 34.3f;
			if(m_Time > 36.9f && m_Time < 37.0f)
			{
				m_Time = 1.1f;
				m_iState = IDLE;
				m_bWarriorSkill = TRUE;
			}
			if(m_Time > 35.7f && m_Time < 36.6f  && m_bWarriorSkill)
			{
				if(m_pTarget && m_pTarget->GetState() != DEATH)
				{
					if(ST::sharedManager()->GetDistance(this->GetPosition(), m_pTarget->GetPosition()) < 40.f)
					{
						m_pTarget->SetAttackDamage(this->m_SkillDamage - m_pTarget->GetDefense());
						m_pTarget->SetAttacker(this);
					}
				}
				m_bWarriorSkill = FALSE;
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
			if(m_Time < 41.3f) m_Time = 41.3f;
			if(m_Time > 44.7f) m_Time = 41.3f;
			if(m_Time > 44.6f)
			{
				m_bUseParticle = FALSE;
				m_bUseParticleMissile = FALSE;
				ParticleManager::sharedManager()->m_pParticle[m_iparticleNum]->SetUsed(FALSE);
				ParticleManager::sharedManager()->m_pParticle[m_iparticleNum]->SetTarget(NULL);
				ParticleManager::sharedManager()->m_pParticle[m_iparticleNum]->SetPosition(D3DXVECTOR3(1200, 0, 0));
				m_iparticleNum = 500;
				//m_iState = IDLE;
			}
			if(m_Time > 43.6f && m_Time < 43.7f)
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
							D3DXVec3Normalize ( &m_vWalkIncrement, &m_vWalkIncrement );
							ParticleManager::sharedManager()->m_pParticle[i]->SetDirection(m_vWalkIncrement);
							ParticleManager::sharedManager()->m_pParticle[i]->SetPosition(D3DXVECTOR3(m_Pos.x, m_Pos.y + 10, m_Pos.z) + m_vWalkIncrement * 30);
							ParticleManager::sharedManager()->m_pParticle[i]->SetAttacker(this);
							m_bUseParticleMissile = TRUE;
							break;
						}
					}
				}
			}
			break;
		}
		break;
	}
	
}


void OtherPlayerObject::SetWatchTarget(D3DXVECTOR3 _pos)
{
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
}

void OtherPlayerObject::SetAniMesh(GFBX::Mesh *pAniMesh)
{
	//if (m_pAniMesh) m_pAniMesh->Release();
	m_pAniMesh = pAniMesh;
	//if (m_pAniMesh) m_pAniMesh->AddRef();
}