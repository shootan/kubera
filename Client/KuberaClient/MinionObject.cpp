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


	m_fAttackTime = 0.0f;
	m_Time = 0.0f;

	m_fWalkSpeed = 15.0f;
	m_HP = 50.f;
	m_Damage = 10.f;
	m_Defense = 3;

	m_iparticleNum = 500;
	m_bUseParticle = FALSE;
	m_bUseParticleMissile = FALSE;
	m_bUseParticleAttack = FALSE;

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
	
	//if(m_iState == ATTACK || m_iState == DEATH) return;

	//Vector3 f_pos;
	//Vector3 s_pos;
	//f_pos.x = m_Pos.x;
	//f_pos.y = m_Pos.y;
	//f_pos.z = m_Pos.z;
	//s_pos.x = _pos.x;
	//s_pos.y = _pos.y;
	//s_pos.z = _pos.z;
	//float finished = ST::sharedManager()->GetDistance(f_pos, s_pos);

	//if(finished > 50.0f)
	//{
	//	this->m_Pos = _pos;
	//	return;
	//}

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
		if(ST::sharedManager()->GetDistance(m_Pos, m_vWayPoint) > 100 || ST::sharedManager()->GetDistance(m_vWayPoint, m_pTarget->GetPosition()) > 100)
		{//몬스터가 시작지점에서 100 이상 떨이지거나 타겟이 웨이포인트에서 100이상 떨어질시
			m_pTarget = NULL;
			m_iState = MOVE;
			SetNewDestination(m_vWayPoint);
		}
	}
	else
	{
		if(ST::sharedManager()->GetDistance(HeroManager::sharedManager()->m_pHero->GetPosition(), m_Pos) < 50 &&
			ST::sharedManager()->GetDistance(m_vWayPoint, HeroManager::sharedManager()->m_pHero->GetPosition()) <= 100) //히어로 타겟잡기
		{
			m_pTarget = HeroManager::sharedManager()->m_pHero;
			m_iState = MOVE;
			SetNewDestination(m_pTarget->GetPosition());
		}
	}
}

void MinionObject::Animate(float fTimeElapsed)
{
	m_Time += fTimeElapsed * 2.0f;

	if(m_iState == IDLE)
	{

	}
	else if(m_iState == ATTACK)
	{
		SetWatchTarget(m_pTarget->GetPosition());
		if(ST::sharedManager()->GetDistance(m_Pos, m_pTarget->GetPosition()) >  m_pTarget->GetBoundSizeX()/2 + this->GetBoundSizeX()/2)
		{//타겟과의 거리가 멀어지면 무브
			m_iState = MOVE;
			SetNewDestination(m_pTarget->GetPosition());
		}
	}
	else if(m_iState == MOVE)
	{
		m_Pos += m_vWalkIncrement * fTimeElapsed;

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
	}
	else if(m_iState == DEATH)
	{
		m_iState = IDLE;
		SetPosition(m_vWayPoint);
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