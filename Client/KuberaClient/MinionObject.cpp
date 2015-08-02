#include "MinionObject.h"
#include "MissileManager.h"
#include "Shader.h"
#include "HeroManager.h"

MinionObject::MinionObject(void)
{
	CGameObject::CGameObject();

	m_pAniMesh = NULL;
	m_iTag = MINION;
	m_Visible = FALSE;
	m_ID = 0;
	m_PrevState = IDLE;
	m_iTargetID = 0;
	m_iTeam = NONE_TEAM;
	m_iType = 0;

	m_fAttackTime = 0.0f;
	m_Time = 0.0f;
	m_fRespawnTime = 0.0f;

	m_Level = 1;
	m_fWalkSpeed = 10.0f;
	m_HP = 50.f;
	m_PrevHP = m_HP;

	m_Damage = 6.f;
	m_Defense = 3;

	m_iparticleNum = 500;
	m_bUseParticle = FALSE;
	m_bUseParticleMissile = FALSE;
	m_bUseParticleAttack = FALSE;

	m_bDeathAnimation = FALSE;
	m_bAttack = FALSE;
	m_nDeathCount = 0;
	
	m_vWayPoint = D3DXVECTOR3(0 , 0, 0);
}

MinionObject::~MinionObject(void)
{
	CGameObject::~CGameObject();
}

void MinionObject::Render(ID3D11DeviceContext *pd3dDeviceContext, float fTimeElapsed, CCamera *pCamera)
{
	if (m_pMaterial) CMaterialShader::UpdateMaterialShaderVariable(pd3dDeviceContext, &m_pMaterial->m_Material);

	if(m_pAniMesh == NULL) return;

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
		if (bIsVisible)	m_pAniMesh->Render(pd3dDeviceContext, m_Time);
	}
}

void MinionObject::SetNewDestination ( D3DXVECTOR3 _pos ) {
	if(_pos == m_Pos) return;

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


}

void MinionObject::Update(float fTimeElapsed)
{
	if(m_pTarget)
	{
		if((ST::sharedManager()->GetDistance(m_Pos, m_vWayPoint) > 100 || 
			ST::sharedManager()->GetDistance(m_vWayPoint, m_pTarget->GetPosition()) > 100) && 
			m_iState != DEATH && m_iState != WAIT)
		{//몬스터가 시작지점에서 100 이상 떨이지거나 타겟이 웨이포인트에서 100이상 떨어질시
			m_pTarget = NULL;
			m_iState = MOVE;
			SetNewDestination(m_vWayPoint);
		}
	}
	else
	{
		if(ST::sharedManager()->GetDistance(HeroManager::sharedManager()->m_pHero->GetPosition(), m_Pos) < 50 &&
			ST::sharedManager()->GetDistance(m_vWayPoint, HeroManager::sharedManager()->m_pHero->GetPosition()) <= 100 &&
			!m_bDeathAnimation ) //히어로 타겟잡기
		{
			m_pTarget = HeroManager::sharedManager()->m_pHero;
			m_iState = MOVE;
			SetNewDestination(m_pTarget->GetPosition());
		}
	}

	if(m_HP <= 0.0f)
	{
		if(!m_bDeathAnimation)
		{
			m_HP = m_Level * 50;
			if(m_pAttacker)
				m_pAttacker->SetExp(m_Level * 3);
			m_iState = DEATH;
			m_bDeathAnimation = TRUE;
			m_nDeathCount++;
		}
	}

	if(m_nDeathCount >= 3)
	{
		m_nDeathCount = 0;

		m_Level++;
		m_HP = m_Level * 50;
		m_Damage = m_Level*2 + 4;
		m_Defense = m_Level + 2;
	}

}

void MinionObject::Animate(float fTimeElapsed)
{
	m_Time += fTimeElapsed * 2.0f;

	if(m_iState == IDLE)
	{
		switch(m_iType)
		{
		case CLEFT:
			if(m_Time < 7.7f) m_Time = 7.7f;
			if(m_Time > 14.3f) m_Time = 7.7f;
			break;
		case TURTLE:
			if(m_Time < 0.1f) m_Time = 0.1f;
			if(m_Time > 8.4f) m_Time = 0.1f;
			break;
		case CANNONGOLEM:
			if(m_Time < 48.0f) m_Time = 48.0f;
			if(m_Time > 55.4f) m_Time = 48.0f;
			break;
		}
	}
	else if(m_iState == ATTACK)
	{
		SetWatchTarget(m_pTarget->GetPosition());
		if(ST::sharedManager()->GetDistance(m_Pos, m_pTarget->GetPosition()) >  m_pTarget->GetBoundSizeX()/2 + this->GetBoundSizeX()/2)
		{//타겟과의 거리가 멀어지면 무브
			m_iState = MOVE;
			SetNewDestination(m_pTarget->GetPosition());
		}

		switch(m_iType)
		{
		case CLEFT:
			if(m_Time < 38.5f) m_Time = 38.5f;
			if(m_Time > 40.0f && m_Time < 41.6f && !m_bAttack)
			{
				m_pTarget->SetAttackDamage(m_Damage - m_pTarget->GetDefense());
				m_bAttack = TRUE;
			}
			if(m_Time > 41.6f)
			{
				m_bAttack = FALSE;
				m_Time = 38.5f;
			}
			break;
		case TURTLE:
			if(m_Time < 29.4f) m_Time = 29.4f;
			if(m_Time > 30.8f && m_Time < 31.8f && !m_bAttack)
			{
				m_pTarget->SetAttackDamage(m_Damage - m_pTarget->GetDefense());
				m_bAttack = TRUE;
			}
			if(m_Time > 31.8f)
			{
				m_bAttack = FALSE;
				m_Time = 29.4f;
			}
			break;
		case CANNONGOLEM:
			if(m_Time < 21.5f) m_Time = 21.5f;
			if(m_Time > 23.8f && m_Time < 24.8f && !m_bAttack)
			{
				m_pTarget->SetAttackDamage(m_Damage - m_pTarget->GetDefense());
				m_bAttack = TRUE;
			}
			if(m_Time > 24.8f)
			{
				m_bAttack = FALSE;
				m_Time = 21.5f;
			}
			break;
		}
	}
	else if(m_iState == MOVE)
	{
		m_Pos += m_vWalkIncrement * fTimeElapsed;
		if(m_pTarget) SetNewDestination(m_pTarget->GetPosition());
		float finished = ST::sharedManager()->GetDistance(m_Pos, m_vDestination);

		if(m_pTarget)  //타겟이 있을경우
		{
			if(finished <= m_pTarget->GetBoundSizeX()/2 + this->GetBoundSizeX()/2)
				m_iState = ATTACK;
		}
		else  //타겟이 없을 경우 웨이포인트로
		{
			if(finished <= 0.5f)
			{
				m_Pos = m_vDestination;
				m_iState = IDLE;
			}
		}

		switch(m_iType)
		{
		case CLEFT:
			if(m_Time < 0.1f) m_Time = 0.1f;
			if(m_Time > 2.7f) m_Time = 0.1f;
			break;
		case TURTLE:
			if(m_Time < 15.5f) m_Time = 15.5f;
			if(m_Time > 17.0f) m_Time = 15.5f;
			break;
		case CANNONGOLEM:
			if(m_Time < 0.1f) m_Time = 0.1f;
			if(m_Time > 5.0f) m_Time = 0.1f;
			break;
		}
	}
	else if(m_iState == DEATH)
	{
		switch(m_iType)
		{
		case CLEFT:
			if(m_Time < 20.0f) m_Time = 20.0f;
			if(m_Time > 25.0f) m_Time = 20.0f;
			if(m_Time > 24.8f && m_Time < 25.0f)
			{
				m_iState = WAIT;
				SetPosition(D3DXVECTOR3(1000, 0, 1000));
				m_bDeathAnimation = FALSE;
			}
			break;
		case TURTLE:
			if(m_Time < 37.5f) m_Time = 37.5f;
			if(m_Time > 45.5f)
			{
				m_iState = WAIT;
				SetPosition(D3DXVECTOR3(1000, 0, 1000));
				m_bDeathAnimation = FALSE;
			}
			break;
		case CANNONGOLEM:
			if(m_Time < 38.0f) m_Time = 38.0f;
			if(m_Time > 43.8f) m_Time = 38.0f;
			if(m_Time > 42.8f && m_Time < 43.8f)
			{
				m_iState = WAIT;
				SetPosition(D3DXVECTOR3(1000, 0, 1000));
				m_bDeathAnimation = FALSE;
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
			SetPosition(m_vWayPoint);
			m_pTarget = NULL;
		}
	}
	else if(m_iState == SKILL1)
	{

	}
}


void MinionObject::SetWatchTarget(D3DXVECTOR3 _pos)
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