#include "TowerObject.h"

TowerObject::TowerObject(void)
{
	CGameObject::CGameObject();
	m_pTarget = NULL;
	m_pPrevTarget = NULL;
	m_iTag = TOWER;
	m_Visible = TRUE;
	m_fAttackTime = 0.f;
}

TowerObject::~TowerObject(void)
{
	CGameObject::~CGameObject();
}

void TowerObject::Render(ID3D11DeviceContext *pd3dDeviceContext)
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
	if(m_pTarget == NULL) return;

	m_fAttackTime += fTimeElapsed;

	for(int i=0; i<MAX_MISSILE; i++)
	{
		if(MissileManager::sharedManager()->m_pMissile[i]->GetUsed() == TRUE) continue;

		if(m_fAttackTime >= 1.5f)
		{
			MissileManager::sharedManager()->m_pMissile[i]->SetPosition(m_Pos + D3DXVECTOR3(0, BoundsizeY * 2/3, 0));
			MissileManager::sharedManager()->m_pMissile[i]->SetUsed(TRUE);
			MissileManager::sharedManager()->m_pMissile[i]->SetTarget(m_pTarget);

			m_fAttackTime = 0.f;
		}
	}
}