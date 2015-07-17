#include "UIObject.h"

UIObject::UIObject(void)
{
	CGameObject::CGameObject();

	//m_iTag = OBSTACLE;
	m_vFacingDirection= D3DXVECTOR3(0,0,-1);
	m_Visible = TRUE;
	m_bUsed = FALSE;
	m_pUI = NULL;
}

UIObject::~UIObject(void)
{
	CGameObject::~CGameObject();
}

void UIObject::Render(ID3D11DeviceContext *pd3dDeviceContext, float x, float y)
{
	m_pUI->Render(pd3dDeviceContext, x, y);
}

void UIObject::SetUI(UIClass* _UI)
{
	m_pUI = _UI;
}