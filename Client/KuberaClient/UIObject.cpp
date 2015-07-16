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

void UIObject::Render(ID3D11DeviceContext *pd3dDeviceContext, int x, int y)
{
	//if(m_Visible != TRUE) return;

	//if(m_iTag == OTHERPLAYER)
	//{
	//	int asdfdf;
	//	asdfdf = 0;
	//}
	//D3DXMATRIX mWorld;
	//D3DXMatrixIdentity(&mWorld);

	////S
	//D3DXMATRIX mtxScale;
	//D3DXMatrixIdentity(&mtxScale);

	//D3DXMatrixScaling(&mtxScale, m_Scale.x, m_Scale.y, m_Scale.z);
	////mWorld *= mtxScale;

	////R
	//D3DXMATRIX mtxRotate;
	//D3DXMatrixIdentity(&mtxRotate);

	//if(m_axis == 1)
	//	D3DXMatrixRotationX(&mtxRotate, D3DX_PI / m_Rot);
	//else if(m_axis == 2)
	//	D3DXMatrixRotationY(&mtxRotate, D3DX_PI / m_Rot);
	//else if(m_axis == 3)
	//	D3DXMatrixRotationZ(&mtxRotate, D3DX_PI / m_Rot);

	//mWorld *= mtxRotate;

	////T
	//D3DXMATRIX mtxTrans;
	//D3DXMatrixIdentity(&mtxTrans);

	//D3DXMatrixTranslation(&mtxTrans, m_Pos.x, m_Pos.y, m_Pos.z);
	////mWorld *= mtxTrans;

	//mWorld = mtxScale * mtxRotate * mtxTrans;
	//m_d3dxmtxWorld = mWorld;

	m_pUI->Render(pd3dDeviceContext, x, y);
}

void UIObject::SetUI(UIClass* _UI)
{
	m_pUI = _UI;
}