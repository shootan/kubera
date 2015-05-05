#include "TowerObject.h"

TowerObject::TowerObject(void)
{
	CGameObject::CGameObject();
	m_pTarget = NULL;
	m_pPrevTarget = NULL;
	m_iTag = TOWER;
	m_Visible = TRUE;
}

TowerObject::~TowerObject(void)
{
	CGameObject::~CGameObject();
}

void TowerObject::Render(ID3D11DeviceContext *pd3dDeviceContext)
{
	CGameObject::Render(pd3dDeviceContext);
}

void TowerObject::Update(float _fMoveIncrement)
{
	if(m_pTarget == NULL) return;

	for(int i=0; i<MAX_MISSILE; i++)
	{
		if(MissileManager::sharedManager()->m_pMissile[i]->GetUsed() == TRUE) continue;

		MissileManager::sharedManager()->m_pMissile[i]->SetPosition(m_Pos);
		MissileManager::sharedManager()->m_pMissile[i]->SetUsed(TRUE);
		MissileManager::sharedManager()->m_pMissile[i]->SetTarget(m_pTarget);
	}

}