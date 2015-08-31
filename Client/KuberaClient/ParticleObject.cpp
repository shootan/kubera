#include "ParticleObject.h"
#include "MinionManager.h"
#include "OtherPlayerManager.h"
#include "HeroManager.h"
ParticleObject::ParticleObject(void)
{
	CGameObject::CGameObject();

	m_Pos = D3DXVECTOR3(-1200,0,1200);
	m_iTag = EFFECT;
	m_vFacingDirection= D3DXVECTOR3(0,0,-1);
	m_Visible = TRUE;
	m_bUsed = FALSE;
	m_time = 0.f;
	m_iType = EFFECTTYPE_NONE;
	m_fWalkSpeed = 45.0f;
	m_pAttacker  = NULL;

	m_CameraPosition = D3DXVECTOR3(0, 0, 0);
}

ParticleObject::~ParticleObject(void)
{
	CGameObject::~CGameObject();
}

void ParticleObject::SetNewDestination ( D3DXVECTOR3 _pos ) {

	m_vDestination.x = _pos.x;
	m_vDestination.y = _pos.y;
	m_vDestination.z = _pos.z;       
	m_vWalkIncrement = m_vDestination - m_Pos;
	D3DXVec3Normalize ( &m_vWalkIncrement, &m_vWalkIncrement );

	//// Calculate the rotation angle before. Next, change the walk direction into 
	//// an increment by multiplying by speed.
	//float fAngle = D3DXVec3Dot( &m_vWalkIncrement, &m_vFacingDirection );
	//D3DXVECTOR3 cross;
	//D3DXVec3Cross( &cross, &m_vWalkIncrement, &m_vFacingDirection );
	//fAngle = acosf( fAngle );
	//if ( cross.y >  0.0f ) {
	//	fAngle *=-1.0f;
	//}
	//fAngle /= D3DX_PI;
	//this->SetRotation(2, 1/fAngle);

	m_vWalkIncrement *= m_fWalkSpeed;       

}

void ParticleObject::Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera)
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
	double angleY, angleX, angleZ;
	float rotationY , rotationX, rotationZ;
	D3DXMATRIX mtxRotate;
	D3DXMatrixIdentity(&mtxRotate);

	if(m_axis == 1)
		D3DXMatrixRotationX(&mtxRotate, D3DX_PI / m_Rot);
	else if(m_axis == 2)
		D3DXMatrixRotationY(&mtxRotate, D3DX_PI / m_Rot);
	else if(m_axis == 3)
		D3DXMatrixRotationZ(&mtxRotate, D3DX_PI / m_Rot);

	//angleY = atan2(m_Pos.x - m_CameraPosition.x, m_Pos.z - m_CameraPosition.z + 17.0f) * (180.0 / D3DX_PI);
	//rotationY = (float)angleY * 0.0174532925f;
	//angleX = atan2(m_Pos.y - m_CameraPosition.y, m_Pos.z - m_CameraPosition.z + 17.0f) * (180.0 / D3DX_PI);
	//rotationX = (float)angleX * 0.0174532925f;
	//angleZ = atan2(m_Pos.y - m_CameraPosition.y, m_Pos.x - m_CameraPosition.x) * (180.0 / D3DX_PI);
	//rotationZ = (float)angleZ * 0.0174532925f;
	//D3DXMatrixRotationY(&mtxRotate, rotationY);
	//D3DXMatrixRotationX(&mtxRotate, rotationX);
	//D3DXMatrixRotationZ(&mtxRotate, rotationZ)
	//D3DXMatrixRotationYawPitchRoll(&mtxRotate, rotationY, rotationX, 0);


	mWorld *= mtxRotate;

	//T
	D3DXMATRIX mtxTrans;
	D3DXMatrixIdentity(&mtxTrans);

	D3DXMatrixTranslation(&mtxTrans, m_Pos.x, m_Pos.y, m_Pos.z);
	//mWorld *= mtxTrans;

	mWorld = mtxScale * mtxRotate * mtxTrans;
	m_d3dxmtxWorld = mWorld;


	if(m_iType != WIZARD_SKILL_BODY)
	{
		D3DXVECTOR3 d3dxvRight, d3dxvUp(0.0f, 1.0f, 0.0f);
		D3DXVECTOR3 d3dxvLook = pCamera->m_CameraPos - this->GetPosition();
		D3DXVec3Normalize(&d3dxvLook, &d3dxvLook);
		D3DXVec3Cross(&d3dxvRight, &d3dxvUp, &d3dxvLook);
		D3DXVec3Normalize(&d3dxvRight, &d3dxvRight);
		m_d3dxmtxWorld._11 = d3dxvRight.x; 
		m_d3dxmtxWorld._12 = d3dxvRight.y; 
		m_d3dxmtxWorld._13 = d3dxvRight.z; 
		m_d3dxmtxWorld._21 = d3dxvUp.x; 
		m_d3dxmtxWorld._22 = d3dxvUp.y; 
		m_d3dxmtxWorld._23 = d3dxvUp.z; 
		m_d3dxmtxWorld._31 = d3dxvLook.x; 
		m_d3dxmtxWorld._32 = d3dxvLook.y; 
		m_d3dxmtxWorld._33 = d3dxvLook.z; 
	}

	if (m_pMesh)
	{
		bool bIsVisible = true;
		if (pCamera)
		{
			AABB bcBoundingCube = m_pMesh->GetBoundingCube();
			bcBoundingCube.Update(&m_d3dxmtxWorld);
			bIsVisible = pCamera->IsInFrustum(&bcBoundingCube); 
		}
		if (bIsVisible) m_pMesh->Render(pd3dDeviceContext);
	}
}

void ParticleObject::Update(float fTimeElapsed)
{
	//if(m_pTarget != NULL || m_pAttacker != NULL)


	m_pMesh->Frame(fTimeElapsed);

	if(m_bUsed == FALSE) return;

	if(m_iType == WIZARD_SKILL_BODY && m_pTarget != NULL)
		SetPosition(m_pTarget->GetPosition());
	else if(m_iType == WIZARD_SKILL_MISSILE && m_pAttacker != NULL)
	{
		m_Pos += m_vWalkIncrement * fTimeElapsed * m_fWalkSpeed;


		for(int i=0;i<MAX_MINION; i++) //미니언 스킬 적중 처리
		{
			if(MinionManager::sharedManager()->m_pMinion[i] == NULL) continue;

			if (ST::sharedManager()->GetDistance(this->GetPosition(), MinionManager::sharedManager()->m_pMinion[i]->GetPosition() + D3DXVECTOR3(0, 10, 0)) <= 10.f)
			{
				//MinionManager::sharedManager()->m_pMinion[i]->SetAttackDamage(m_pAttacker->GetSkillDamage() - MinionManager::sharedManager()->m_pMinion[i]->GetDefense());
				if(m_pAttacker != ST::sharedManager()->m_pOtherPlayer)
				{
					AttackInfo ATI;
					ATI.m_ID = MinionManager::sharedManager()->m_pMinion[i]->GetID();
					ATI.m_Damage = m_pAttacker->GetSkillDamage() - MinionManager::sharedManager()->m_pMinion[i]->GetDefense();
					ST::sharedManager()->Net->SendData(ATTACK_INFO, &ATI, sizeof(AttackInfo));
				}
				
				MinionManager::sharedManager()->m_pMinion[i]->SetAttacker(m_pAttacker);
				m_pTarget = NULL;
				m_pAttacker = NULL;
				m_Pos = D3DXVECTOR3(0, 0, -2000);
				m_bUsed = FALSE;
				break;
			}

			if(ST::sharedManager()->GetDistance(this->GetPosition(), OtherPlayerManager::sharedManager()->m_pOtherPlayer->GetPosition() + D3DXVECTOR3(0, 10, 0)) <= 10.f)
			{
				//OtherPlayerManager::sharedManager()->m_pOtherPlayer->SetAttackDamage(m_pAttacker->GetSkillDamage() - OtherPlayerManager::sharedManager()->m_pOtherPlayer->GetDefense());
				if(m_pAttacker != ST::sharedManager()->m_pOtherPlayer)
				{
					AttackInfo ATI;
					ATI.m_ID = OtherPlayerManager::sharedManager()->m_pOtherPlayer->GetID();
					ATI.m_Damage = m_pAttacker->GetSkillDamage() - OtherPlayerManager::sharedManager()->m_pOtherPlayer->GetDefense();
					ST::sharedManager()->Net->SendData(ATTACK_INFO, &ATI, sizeof(AttackInfo));
				}

				OtherPlayerManager::sharedManager()->m_pOtherPlayer->SetAttacker(m_pAttacker);
				m_pTarget = NULL;
				m_pAttacker = NULL;
				m_Pos = D3DXVECTOR3(0, 0, -2000);
				m_bUsed = FALSE;
				break;
			}

			if (ST::sharedManager()->GetDistance(this->GetPosition(), m_pAttacker->GetPosition() + D3DXVECTOR3(0, 10, 0)) >= 100.f)
			{
				m_pTarget = NULL;
				m_pAttacker = NULL;
				m_Pos = D3DXVECTOR3(0, 0, -2000);
				m_bUsed = FALSE;
				break;
			}

			if(ST::sharedManager()->GetDistance(this->GetPosition(), HeroManager::sharedManager()->m_pHero->GetPosition() + D3DXVECTOR3(0, 10, 0)) <= 10.f)
			{
				//HeroManager::sharedManager()->m_pHero->SetAttackDamage(m_pAttacker->GetSkillDamage() - HeroManager::sharedManager()->m_pHero->GetDefense());
				if(m_pAttacker != ST::sharedManager()->m_pOtherPlayer)
				{
					AttackInfo ATI;
					ATI.m_ID = HeroManager::sharedManager()->m_pHero->GetID();
					ATI.m_Damage = m_pAttacker->GetSkillDamage() - HeroManager::sharedManager()->m_pHero->GetDefense();
					ST::sharedManager()->Net->SendData(ATTACK_INFO, &ATI, sizeof(AttackInfo));
				}

				HeroManager::sharedManager()->m_pHero->SetAttacker(m_pAttacker);
				m_pTarget = NULL;
				m_pAttacker = NULL;
				m_Pos = D3DXVECTOR3(0, 0, -2000);
				m_bUsed = FALSE;
				break;
			}
		}

	}
	else if(m_iType == WIZARD_ATTACK && m_pTarget != NULL && m_pAttacker != NULL)
	{
		SetNewDestination(m_pTarget->GetPosition() + D3DXVECTOR3(0 , m_pTarget->GetBoundSizeY()/2 * 2, 0));

		if(m_pTarget->GetHP() <= 0.0f)
		{
			m_Pos = D3DXVECTOR3(0, 0 ,-2000);
			m_bUsed = FALSE;
			m_pTarget = NULL;
			m_pAttacker = NULL;
			return;
		}

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

		if ( finished < 1.0f ) 
		{
			//m_pTarget->SetAttackDamage(m_pAttacker->GetDamage() - m_pTarget->GetDefense());
			if(m_pAttacker != ST::sharedManager()->m_pOtherPlayer)
			{
				AttackInfo ATI;
				ATI.m_ID = m_pTarget->GetID();
				ATI.m_Damage = m_pAttacker->GetDamage() - m_pTarget->GetDefense();
				ST::sharedManager()->Net->SendData(ATTACK_INFO, &ATI, sizeof(AttackInfo));
			}
			m_pTarget->SetAttacker(m_pAttacker);
			m_Pos = D3DXVECTOR3(0, 0 ,-2000);
			m_bUsed = FALSE;
			m_pTarget = NULL;
			m_pAttacker = NULL;
		}
		else if(finished > 80.0f)
		{
			m_Pos = D3DXVECTOR3(0, 0 ,-2000);
			m_bUsed = FALSE;
			m_pTarget = NULL;
			m_pAttacker = NULL;
		}
	}
}