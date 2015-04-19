#include "TowerObject.h"

TowerObject::TowerObject(void)
{
	CGameObject::CGameObject();
	Target = NULL;
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
	if(Target != NULL)
		this->SetPos(Target->GetPos());
}