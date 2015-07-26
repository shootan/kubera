#include "HeroObject.h"
#include "Shader.h"

HeroObject::HeroObject(void)
{
	CGameObject::CGameObject();

	m_iTag = HERO;
	m_bMove = FALSE;
	m_pTarget = NULL;
	m_Visible = TRUE;
	m_fAttackTime = 0.f;

	m_pAstar = NULL;
	m_pBestWay = NULL;
	m_bAstar = FALSE;
	m_bFindPath = FALSE;
	m_bDeathAnimation = FALSE;
	m_iPrevState = 0;

	m_time = 0.0f;

	m_iparticleNum = 500;
	m_bUseParticle = FALSE;
	m_bWarriorAttack = TRUE;
	m_bUseParticleMissile = FALSE;
	m_bUseParticleAttack = FALSE;

	//초기 정보값
	m_Level = 1;			//레벨1
	m_HP = 100.f;				//hp 100
	m_Defense = 3;			//방어력 3
	m_fWalkSpeed = 15.f;	//스피드 15
	m_Damage = 10.f;		//데미지 10
	m_SkillDamage = 20 + m_Damage;		//스킬데미지 20
	m_Exp = 10;				//필요 경험치 10
	m_Speed_Level = 1;		//스피드 레벨 1
	m_Defence_Level = 1;	//방어력 레벨 1
	m_Damage_Level = 1;		//데미지 레벨 1


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


void HeroObject::Render(ID3D11DeviceContext *pd3dDeviceContext, float fTimeElapsed, CCamera *pCamera)
{
	if (m_pMaterial) CMaterialShader::UpdateMaterialShaderVariable(pd3dDeviceContext, &m_pMaterial->m_Material);

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
			else
				bIsVisible = true;
		}

		if (bIsVisible) m_pAniMesh->Render(pd3dDeviceContext, m_time);
	}
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

	LevelUp(); //레벨업 할시 들어오는 함수

	if(m_iPrevState != m_iState)
	{
		m_time = 1.1f;
		m_iPrevState = m_iState;
	}

	if(m_HP <= 0.0f)
	{
		if(m_bDeathAnimation)
		{
			Vector3 po;
			if(m_ID % 2 == 0)
			{
				po.x = 390;
				po.y = 0;
				po.z = 0;
			}
			else
			{
				po.y = 0;
				po.x = -390;
				po.z = 0;
			}
			m_HP = m_Level * 100;
			this->SetPos(po);
			m_bDeathAnimation = FALSE;
			return;
		}
		m_iState = DEATH;
		return;
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
	m_time += fTimeElapsed * 2.0f;

	//printf(" %.3f \n", m_time);


	//printf(" %.1f \n", m_time);

	if(m_iState == IDLE)
	{
		switch(m_iType)
		{
		case KNIGHT:
			if(m_time < 1.1f) m_time = 1.1f;
			if(m_time > 7.0f) m_time = 1.1f;
			break;
		case WIZARD:
			if(m_time < 1.1f) m_time = 1.1f;
			if(m_time > 7.1f) m_time = 1.1f;
			break;
		}
	}
	else if(m_iState == ATTACK)
	{
		switch(m_iType)
		{
		case KNIGHT:
			if(m_time < 41.0f) m_time = 41.0f;
			if(m_time > 43.0f && m_bWarriorAttack)
			{
				m_pTarget->SetAttackDamage(this->m_Damage);
				m_bWarriorAttack = FALSE;
			}
			if(m_time > 44.0f)
			{
				m_bWarriorAttack = TRUE;
				m_time = 41.0f;
			}
			break;
		case WIZARD:
			if(m_time < 20.0f) m_time = 20.0f;
			if(m_time > 21.5f && m_time < 22.0f)
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
			if(m_time > 23.0f) 
			{
				m_time = 20.0f;
				m_bUseParticleAttack = FALSE;
			}
			break;
		}

		if(m_pTarget->GetHP() < 1 || ST::sharedManager()->GetDistance(this->GetPos(), m_pTarget->GetPos()) > 50.0f)
		{
			m_pTarget = NULL;
			m_fAttackTime = 0.0f;
			m_iState = IDLE;
			return;
		}
		m_fAttackTime += fTimeElapsed;
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
			if(m_pTarget != NULL && 
				ST::sharedManager()->GetDistance(this->GetPos(), m_pTarget->GetPos()) < 25.f && m_pTarget->GetTeam() != this->GetTeam())
				m_iState = ATTACK;

			if(m_time < 57.5f) m_time = 57.5f;
			if(m_time > 59.7f ) m_time = 57.5f;
			break;
		case WIZARD:
			if(m_pTarget != NULL && 
				ST::sharedManager()->GetDistance(this->GetPos(), m_pTarget->GetPos()) < 50.f && m_pTarget->GetTeam() != this->GetTeam())
				m_iState = ATTACK;

			if(m_time < 68.0f) m_time = 68.0f;
			if(m_time > 70.3f ) m_time = 68.0f;
			break;
		}
		m_fAttackTime = 0.f;
	}
	else if(m_iState == DEATH)
	{
		switch(m_iType)
		{
		case KNIGHT:
			if(m_time < 30.0f) m_time = 30.0f;
			if(m_time > 34.0f)
			{
				m_time = 1.1f;
				m_iState = IDLE;
				m_bDeathAnimation = TRUE;
			}
			break;
		case WIZARD:
			if(m_time < 30.0f) m_time = 30.0f;
			if(m_time > 34.0f)
			{
				m_time = 1.1f;
				m_iState = IDLE;
				m_bDeathAnimation = TRUE;
			}

			break;
		}
	}
	else if(m_iState == SKILL1)
	{
		switch(m_iType)
		{
		case KNIGHT:
			if(m_time < 50.0f)  m_time = 50.0f;
			if(m_time > 54.0f) 
			{
				m_time = 1.1f;
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
			if(m_time < 45.0f) m_time = 45.0f;
			if(m_time > 48.5f && m_time < 49.f)
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
	
							//ParticleManager::sharedManager()->m_pParticle[i]->SetTarget(OtherPlayerManager::sharedManager()->m_pOtherPlayer);
							
							if(ParticleManager::sharedManager()->m_pParticle[i]->GetTarget() == NULL)
								ParticleManager::sharedManager()->m_pParticle[i]->SetTarget(this);
							D3DXVec3Normalize ( &m_vWalkIncrement, &m_vWalkIncrement );
							ParticleManager::sharedManager()->m_pParticle[i]->SetDirection(m_vWalkIncrement);
							ParticleManager::sharedManager()->m_pParticle[i]->SetPosition(D3DXVECTOR3(m_Pos.x, m_Pos.y + 10, m_Pos.z) + m_vWalkIncrement * 15);
							ParticleManager::sharedManager()->m_pParticle[i]->SetAttacker(this);
							m_bUseParticleMissile = TRUE;
							break;
						}
					}
				}
			}
			if(m_time > 49.5f)
			{
				m_bUseParticle = FALSE;
				m_bUseParticleMissile = FALSE;
				ParticleManager::sharedManager()->m_pParticle[m_iparticleNum]->SetUsed(FALSE);
				ParticleManager::sharedManager()->m_pParticle[m_iparticleNum]->SetTarget(NULL);
				ParticleManager::sharedManager()->m_pParticle[m_iparticleNum]->SetPosition(D3DXVECTOR3(1200, 0, 0));
				m_iparticleNum = 500;
				m_time = 1.1f;
				m_iState = IDLE;
			}

			break;
		}

	}
}


void HeroObject::LevelUp()
{
	if(m_Exp <= 0)
	{
		m_Level += 1;
		UpdateCharacterInfo(m_Level);
	}
}

void HeroObject::UpdateCharacterInfo(int _Level)
{
	m_HP = _Level * 100;  //초기 100 부터 100씩 증가
	m_Exp = _Level * 10;  //초기 10 부터 10씩 증가
}

void HeroObject::DamageUp(float _damage)
{
	m_Damage = _damage * 5 + 5;    //초기10 부터 5씩증가 
	m_Speed_Level = 20 + m_Damage;
}
void HeroObject::SpeedUp(float _speed)
{
	m_fWalkSpeed = _speed * 3 + 12;  //초기 15 부터 3씩증가
}
void HeroObject::DefenseUp(float _defense)
{
	m_Defense = _defense * 3;  //초기 3부터 3씩 증가
}