#include "HeroObject.h"
#include "Shader.h"
#include "MissileManager.h"
#include "ParticleManager.h"
#include "OtherPlayerManager.h"
#include "SoundManager.h"

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
	m_iPrevState = m_iState;

	m_time = 0.0f;
	m_fRespawnTime = 0.0f;

	m_iparticleNum = 500;
	m_bUseParticle = FALSE;
	m_bWarriorAttack = TRUE;
	m_bWarriorSkill = TRUE;
	m_bUseParticleMissile = FALSE;
	m_bUseParticleAttack = FALSE;

	m_bSoundLimit = FALSE;

	//초기 정보값
	m_Level = 1;			//레벨1
	m_HP = 100.0f;				//hp 100
	m_PrevHP = m_HP;
	m_Defense = 3;			//방어력 3
	m_fWalkSpeed = 50.0f;	//스피드 15
	m_Damage = 10.0f;		//데미지 10
	m_SkillDamage = 20 + m_Damage;		//스킬데미지 20
	m_Exp = 10;				//필요 경험치 10
	m_Speed_Level = 1;		//스피드 레벨 1
	m_Defence_Level = 1;	//방어력 레벨 1
	m_Damage_Level = 1;		//데미지 레벨 1

	m_bUpgrade = FALSE;
	m_bHpFUCK = TRUE;
	m_nDeathCount = 0;

	m_iState = IDLE;
	m_iPrevState = m_iState;

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
		m_time = 0.f;
		m_iPrevState = m_iState;
	}

	if(m_bHpFUCK)
	{
		m_HP = m_Level * 100;
		m_bHpFUCK = FALSE;
	}

	if(m_HP <= 0.0f)
	{
		if(!m_bDeathAnimation)
		{
			m_HP = m_Level * 100;
			m_iState = DEATH;
			m_bDeathAnimation = TRUE;
			m_nDeathCount++;
		}
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

	if(m_iState != m_iPrevState)
	{
		//m_iPrevState = m_iState;
		m_bSoundLimit = FALSE;
	}
	//printf(" %.1f \n", m_time);
	

	if(m_iState == IDLE)
	{
		switch(m_iType)
		{
		case KNIGHT:
			if(m_time < 16.8f) m_time = 16.8f;
			if(m_time > 21.3f) m_time = 16.8f;
			break;
		case WIZARD:
			if(m_time < 6.9f) m_time = 6.9f;
			if(m_time > 11.7f) m_time = 6.9f; 
			break;
		}
	}
	else if(m_iState == ATTACK)
	{
		if(!m_pTarget || m_pTarget->GetState() == DEATH) //상대가 죽었을시 공격 안하게 설정
		{
			m_iState = IDLE;
			m_time = 0.f;
			return;
		}

		switch(m_iType)
		{
		case KNIGHT:
			if(m_time < 26.7f) 
			{
				m_time = 26.7f;
				if(!m_bSoundLimit)
				{
					SoundManager::sharedManager()->play(SOUND_WARR_ATTACK);
					m_bSoundLimit = TRUE;
				}

			}
			if(m_time > 29.0f)
			{
				m_bSoundLimit = FALSE;
				m_bWarriorAttack = TRUE;
				m_time = 26.7f;
			}
			if(m_time > 28.0f && m_bWarriorAttack)
			{
				//m_pTarget->SetAttackDamage(this->m_Damage - m_pTarget->GetDefense());
				m_pTarget->SetAttacker(this);
				AttackInfo ATI;
				ATI.m_ID = m_pTarget->GetID();
				ATI.m_Damage = this->m_Damage - m_pTarget->GetDefense();
				ST::sharedManager()->Net->SendData(ATTACK_INFO, &ATI, sizeof(AttackInfo));
				m_bWarriorAttack = FALSE;
				

			}
			break;
		case WIZARD:
			if(m_time < 24.5f) m_time = 24.5f;
			if(m_time > 26.9f)
			{
				m_time = 24.5f; 
				m_bUseParticleAttack = FALSE;
				m_bSoundLimit = FALSE;
			}
			if(m_time > 26.0f && m_time < 26.1f)
			{
				if(m_bUseParticleAttack == FALSE)
				{
					if(!m_bSoundLimit)
					{
						SoundManager::sharedManager()->play(SOUND_SKEL_ATTACK);
						m_bSoundLimit = TRUE;
					}
					
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

		if(m_pTarget->GetHP() < 0 || ST::sharedManager()->GetDistance(this->GetPos(), m_pTarget->GetPos()) > 50.0f)
		{
			m_pTarget = NULL;
			m_fAttackTime = 0.0f;
			m_iState = IDLE;
			return;
		}
	}
	else if(m_iState == MOVE)
	{
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
			m_bWarriorAttack = TRUE;
			m_bWarriorSkill = TRUE;

			if(m_pTarget != NULL &&	m_pTarget->GetTeam() != this->GetTeam())
			{
				if(m_pTarget->GetFaceType() == GOLEM_FACE || m_pTarget->GetFaceType() == TURTLE_FACE || m_pTarget->GetFaceType() == CLEFT_FACE)
				{
					if(ST::sharedManager()->GetDistance(this->GetPos(), m_pTarget->GetPos()) < GetBoundSizeX()/2 + m_pTarget->GetBoundSizeX()/2)
						m_iState = ATTACK;
				}
				else if (m_pTarget->GetFaceType() == NEXUS_FACE)
				{
					if(ST::sharedManager()->GetDistance(this->GetPos(), m_pTarget->GetPos()) < 35.f)
						m_iState = ATTACK;
				}
				else
					if(ST::sharedManager()->GetDistance(this->GetPos(), m_pTarget->GetPos()) < 25.f)
						m_iState = ATTACK;
			}

			if(m_time < 0.1f)
			{
				if(!m_bSoundLimit)
				{
					SoundManager::sharedManager()->play(SOUND_WARR_MOVE);
					m_bSoundLimit = TRUE;
				}
				m_time = 0.1f;
			}
			if(m_time > 1.7f)
			{
				m_time = 0.1f;
				m_bSoundLimit = FALSE;
			}
			break;
		case WIZARD:
			m_bUseParticleAttack = FALSE;
			m_bUseParticleMissile = FALSE;

			if(m_pTarget != NULL && 
				ST::sharedManager()->GetDistance(this->GetPos(), m_pTarget->GetPos()) < 50.f && 
				m_pTarget->GetTeam() != this->GetTeam())
				m_iState = ATTACK;

			if(m_time < 0.1f)
			{
				if(!m_bSoundLimit)
				{
					SoundManager::sharedManager()->play(SOUND_SKEL_MOVE);
					m_bSoundLimit = TRUE;
				}
				m_time = 0.1f;
			}
			if(m_time > 1.7f) {
				m_time = 0.1f; 
				m_bSoundLimit = FALSE;
			}
			break;
		}
	}
	else if(m_iState == DEATH)
	{
		switch(m_iType)
		{
		case KNIGHT:
			if(m_time < 6.9f)
			{
				if(!m_bSoundLimit)
				{
					SoundManager::sharedManager()->play(SOUND_SKEL_MOVE);
					m_bSoundLimit = TRUE;
				}
				m_time = 6.9f;
			}
			if(m_time > 11.9f) m_time = 6.9f;
			if(m_time > 11.8f && m_time < 11.9f)
			{
				m_bSoundLimit = FALSE;
				m_time = 1.1f;
				m_iState = WAIT;
				SetPosition(D3DXVECTOR3(1000, 0, 200));
			}
			break;
		case WIZARD:
			if(m_time < 31.5f)
			{
				m_time = 31.5f;
				if(!m_bSoundLimit)
				{
					SoundManager::sharedManager()->play(SOUND_SKEL_MOVE);
					m_bSoundLimit = TRUE;
				}
			}
			if(m_time > 36.3f) m_time = 31.5f;
			if(m_time > 36.2f && m_time < 36.3f)
			{
				m_bSoundLimit = FALSE;
				m_time = 1.1f;
				m_iState = WAIT;
				SetPosition(D3DXVECTOR3(1000, 0, 200));
			}
			break;
		}
	}
	else if(m_iState == WAIT)
	{
		m_fRespawnTime += fTimeElapsed;

		if(m_fRespawnTime >= 5.0f)
		{
			m_fRespawnTime = 0;
			m_iState = IDLE;
			m_bDeathAnimation = FALSE;

			Vector3 po;
			if(m_ID % 2 == 0)
			{
				po.x = -390;
				po.y = 0;
				po.z = 0;
			}
			else
			{
				po.y = 0;
				po.x = 390;
				po.z = 0;
			}
			this->SetPos(po);
		}
	}
	else if(m_iState == SKILL1)
	{
		switch(m_iType)
		{
		case KNIGHT:
			if(m_pTarget) SetWatchTarget(m_pTarget->GetPosition());

			if(m_time < 34.3f)
			{
				m_time = 34.3f;
				if(!m_bSoundLimit)
				{
					SoundManager::sharedManager()->play(SOUND_WARR_SKILL);
					m_bSoundLimit = TRUE;
				}
			}
			if(m_time > 37.0f) m_time = 34.3f;
			if(m_time > 36.9f && m_time < 37.0f) //스킬 끝날시간
			{
				if(m_pTarget) m_iState = ATTACK;
				else m_iState = IDLE;

				m_time = 1.1f;
				m_bWarriorAttack = TRUE;
				m_bWarriorSkill = TRUE;
				m_bSoundLimit = FALSE;
			}
			if(m_time > 35.7f && m_time < 36.6f  && m_bWarriorSkill) //데미지 입히는 시간
			{
				if(m_pTarget && m_pTarget->GetState() != DEATH)
				{
					if(ST::sharedManager()->GetDistance(this->GetPosition(), m_pTarget->GetPosition()) < 50.f)
					{
						//m_pTarget->SetAttackDamage(this->m_SkillDamage - m_pTarget->GetDefense());
						m_pTarget->SetAttacker(this);
						AttackInfo ATI;
						ATI.m_ID = m_pTarget->GetID();
						ATI.m_Damage = this->m_SkillDamage - m_pTarget->GetDefense();
						ST::sharedManager()->Net->SendData(ATTACK_INFO, &ATI, sizeof(AttackInfo));

						//ST::sharedManager()->Net->SendData()
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
			if(m_time < 41.3f) m_time = 41.3f;
			if(m_time > 44.7f) m_time = 41.3f;
			if(m_time > 44.6f)
			{
				m_bUseParticle = FALSE;
				m_bUseParticleMissile = FALSE;
				ParticleManager::sharedManager()->m_pParticle[m_iparticleNum]->SetUsed(FALSE);
				ParticleManager::sharedManager()->m_pParticle[m_iparticleNum]->SetTarget(NULL);
				ParticleManager::sharedManager()->m_pParticle[m_iparticleNum]->SetPosition(D3DXVECTOR3(1200, 0, 0));
				m_iparticleNum = 500;
				m_iState = IDLE;
				m_bSoundLimit = FALSE;
			}
			if(m_time > 43.6f && m_time < 43.7f)
			{
				if(!m_bSoundLimit)
				{
					SoundManager::sharedManager()->play(SOUND_SKEL_SKILL);
					m_bSoundLimit = TRUE;
				}
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
							ParticleManager::sharedManager()->m_pParticle[i]->SetPosition(D3DXVECTOR3(m_Pos.x, m_Pos.y + 10, m_Pos.z) + m_vWalkIncrement * 15);
							ParticleManager::sharedManager()->m_pParticle[i]->SetAttacker(this);
							m_bUseParticleMissile = TRUE;
							break;
						}
					}
				}
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

		if(m_Level > 10)
		{
			m_Level = 10;
			return;
		}
		UpdateCharacterInfo(m_Level);
		m_bUpgrade = TRUE;
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
	m_SkillDamage = 20 + m_Damage;
}
void HeroObject::SpeedUp(float _speed)
{
	m_fWalkSpeed = _speed * 3 + 47;  //초기 15 부터 3씩증가
}
void HeroObject::DefenseUp(float _defense)
{
	m_Defense = _defense * 3;  //초기 3부터 3씩 증가
}

void HeroObject::SetWatchTarget(D3DXVECTOR3 _pos)
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