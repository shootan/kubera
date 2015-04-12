#include "ObstacleObject.h"

ObstacleObject::ObstacleObject(void)
{
	CGameObject::CGameObject();

	m_iTag = OBSTACLE;
	m_vFacingDirection= D3DXVECTOR3(0,0,-1);
}

ObstacleObject::~ObstacleObject(void)
{
	CGameObject::~CGameObject();
}

void ObstacleObject::Render(ID3D11DeviceContext *pd3dDeviceContext)
{
	CGameObject::Render(pd3dDeviceContext);
}
