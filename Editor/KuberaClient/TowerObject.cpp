#include "TowerObject.h"

TowerObject::TowerObject(void)
{
	CGameObject::CGameObject();
	m_pTarget = NULL;
	m_pPrevTarget = NULL;
	m_iTag = TOWER;
	m_Visible = TRUE;
	m_fAttackTime = 0.f;
	m_HP = 300.0f;
	m_Damage = 50.0f;
	m_bExplosion = false;
}

TowerObject::~TowerObject(void)
{
	CGameObject::~CGameObject();
}

void TowerObject::Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera)
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

void TowerObject::Update(float fTimeElapsed)
{
	if(m_HP< 1.0f && !m_bExplosion)
	{
		for(int i=0;i<MAX_DESTROY_TOWER; i++)
		{
			if(ObstacleManager::sharedManager()->m_pDestroyTower[i]->GetUsed() == TRUE) continue;

			ObstacleManager::sharedManager()->m_pDestroyTower[i]->SetUsed(TRUE);
			ObstacleManager::sharedManager()->m_pDestroyTower[i]->SetPosition(m_Pos);
			break;
		}
		m_Pos = D3DXVECTOR3(0, 0, -2000);
		m_pTarget = NULL;
		m_bExplosion = true;
		return;
	}

	if(m_pTarget == NULL) return;


	m_fAttackTime += fTimeElapsed;

	if(m_pTarget->GetHP() < 1 || ST::sharedManager()->GetDistance(this->GetPos(), m_pTarget->GetPos())> 50.0f)
	{
		m_pTarget = NULL;
		m_fAttackTime = 0.0f;
		return;
	}
	
	for(int i=0; i<MAX_PARTICLE; i++)
	{
		if(ParticleManager::sharedManager()->m_pParticle[i] == NULL) continue;
		if(MissileManager::sharedManager()->m_pMissile[i]->GetUsed() == TRUE) continue;


		if(ParticleManager::sharedManager()->m_pParticle[i]->GetType() == WIZARD_ATTACK)
		{
			if(m_fAttackTime >= 1.3f)
			{
				ParticleManager::sharedManager()->m_pParticle[i]->SetPosition(m_Pos + D3DXVECTOR3(0, BoundsizeY * 2/3, 0));
				ParticleManager::sharedManager()->m_pParticle[i]->SetUsed(TRUE);
				ParticleManager::sharedManager()->m_pParticle[i]->SetTarget(m_pTarget);
				ParticleManager::sharedManager()->m_pParticle[i]->SetAttacker(this);

				m_fAttackTime = 0.f;
			}

		//if(m_fAttackTime >= 2.0f)
		//{
		//	D3DXVECTOR3 p = m_Pos;
		//	//p.y+= 5.0f;
		//	MissileManager::sharedManager()->m_pMissile[i]->SetPosition(p + D3DXVECTOR3(0, BoundsizeY * 2/3, 0));
		//	MissileManager::sharedManager()->m_pMissile[i]->SetUsed(TRUE);
		//	MissileManager::sharedManager()->m_pMissile[i]->SetTarget(m_pTarget);
		//	MissileManager::sharedManager()->m_pMissile[i]->SetAttacker(this);


		}
	}
}