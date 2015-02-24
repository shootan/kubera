#include "ControlManager.h"

void ControlManager::TouchDown(float _x, float _y, HWND hWnd)
{
	D3DXVECTOR3 vRayDirection;
	D3DXVECTOR3 vRayOrigin = *m_Camera->GetEyePt();
	this->CalculateScreenRayFromCoordinates( _x, _y, vRayDirection , hWnd );
	D3DXVECTOR3 vGroundPlaneHit;

	D3DXVECTOR3 vPlaneNormal = D3DXVECTOR3(0,-1,0);
	float fpGroundPlaneINTersection = D3DXVec3Dot(&vPlaneNormal, &vRayDirection);
	if ( fpGroundPlaneINTersection > .001) {
		float fpINTersectionDistnace = -1* D3DXVec3Dot( &vPlaneNormal, &vRayOrigin );
		float fpINTersectionTime = fpINTersectionDistnace / fpGroundPlaneINTersection;
		if ( fpINTersectionTime > 0) {
			vGroundPlaneHit = vRayOrigin;
			vGroundPlaneHit += ( vRayDirection * fpINTersectionTime );
			m_Player->SetNewDestination(vGroundPlaneHit);
		}
	}     
}

void ControlManager::AssignSelectedUnitsToNewDestination ( const D3DXVECTOR3 &vec )
{
	//return 0;
}

void ControlManager::CalculateScreenRayFromCoordinates( float x, float y, D3DXVECTOR3 &vRayDirection  , HWND hWnd)
{
	RECT windowRect;
	GetClientRect(hWnd, &windowRect);
	const D3DXMATRIX* pmatProj = m_Camera->GetProjMatrix();

	//3DÁÂÇ¥·Î º¯°æ
	D3DXVECTOR3 v;
	v.x = ( ( ( 2.0f * x ) / windowRect.right ) - 1 ) / pmatProj->_11;
	v.y = -( ( ( 2.0f * y ) / windowRect.bottom) - 1 ) / pmatProj->_22;
	v.z = 1.0f;
	D3DXVECTOR3 vn = v;
	D3DXVec3Normalize(&vn, &vn);
	// Get the inverse view matrix
	const D3DXMATRIX matView = *m_Camera->GetViewMatrix();
	const D3DXMATRIX matWorld = *m_Camera->GetWorldMatrix();
	D3DXMATRIX mWorldView = matWorld * matView;
	D3DXMATRIX m; 
	D3DXMatrixInverse( &m, NULL, &mWorldView );

	vRayDirection.x = v.x * m._11 + v.y * m._21 + v.z * m._31;
	vRayDirection.y = v.x * m._12 + v.y * m._22 + v.z * m._32;
	vRayDirection.z = v.x * m._13 + v.y * m._23 + v.z * m._33;
	D3DXVec3Normalize( &vRayDirection, &vRayDirection );
}