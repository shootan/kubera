#include "ControlManager.h"

void ControlManager::TouchRightDown(float _x, float _y, HWND hWnd)
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
			HeroManager::sharedManager()->m_pHero->SetNewDestination(vGroundPlaneHit);
			HeroManager::sharedManager()->m_pHero->SetState(MOVE);
		}
	}     

	SetTarget(vRayDirection, vRayOrigin);
}

void ControlManager::TouchLeftDown(float _x, float _y , HWND hWnd)
{
	D3DXVECTOR3 vRayDirection;
	D3DXVECTOR3 vRayOrigin = *m_Camera->GetEyePt();
	this->CalculateScreenRayFromCoordinates( _x, _y, vRayDirection , hWnd );

	SetTarget(vRayDirection, vRayOrigin);
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

	//3D좌표로 변경
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

float ControlManager::INTersectRaySphere(const D3DXVECTOR3 vRayDirection, D3DXVECTOR3 vRayOrigin, D3DXVECTOR3 vSphereOrigin,  float fpRadiusSquared)
{
	D3DXVECTOR3 vRayToSphere = vSphereOrigin - vRayOrigin ;
	D3DXVECTOR3 vNormal = vRayToSphere;

	float fpB = D3DXVec3Dot( &vRayToSphere, &vRayDirection );
	float fpC = D3DXVec3Dot( &vRayToSphere, &vRayToSphere );
	float fpD = fpB * fpB - fpC + fpRadiusSquared; 
	return fpD > 0 ? sqrt(fpB) : -2e32f;
}

void ControlManager::SetTarget(const D3DXVECTOR3 vRayDirection, D3DXVECTOR3 vRayOrigin)
{
	for(int i=0; i<MAX_TOWER; i++)  //타워 타겟 잡기
	{
		//if(TowerManager::sharedManager()->m_pTower[i]->GetSelected() == TRUE) continue;

		float dist = INTersectRaySphere(vRayDirection, vRayOrigin, TowerManager::sharedManager()->m_pTower[i]->GetPosition(), 
			TowerManager::sharedManager()->m_pTower[i]->GetBoundSizeX()*2);

		if( dist > 0 )
		{
			//TowerManager::sharedManager()->m_pTower[i]->SetSelected(TRUE);
			HeroManager::sharedManager()->m_pHero->SetTarget(TowerManager::sharedManager()->m_pTower[i]);
			HeroManager::sharedManager()->m_pHero->SetTargetID(TowerManager::sharedManager()->m_pTower[i]->GetID());
			return;
		}
		//TowerManager::sharedManager()->m_pTower[i]->SetSelected(FALSE);
	}

	for(int i=0; i<MAX_OTHER_PLAYER; i++)  //다른 플레이어 타겟 잡기
	{
		if(OtherPlayerManager::sharedManager()->m_pOtherPlayer[i]->GetVisible() != TRUE) continue;

		float dist = INTersectRaySphere(vRayDirection, vRayOrigin, OtherPlayerManager::sharedManager()->m_pOtherPlayer[i]->GetPosition(), 
			OtherPlayerManager::sharedManager()->m_pOtherPlayer[i]->GetBoundSizeX()*2);

		if( dist > 0 )
		{
			HeroManager::sharedManager()->m_pHero->SetTarget(OtherPlayerManager::sharedManager()->m_pOtherPlayer[i]);
			HeroManager::sharedManager()->m_pHero->SetTargetID(OtherPlayerManager::sharedManager()->m_pOtherPlayer[i]->GetID());
			return;
		}
	}

	for(int i=0; i<MAX_MINION; i++)  //미니언 타겟 잡기
	{
		float dist = INTersectRaySphere(vRayDirection, vRayOrigin, MinionManager::sharedManager()->m_pMinion1[i]->GetPosition(), 
			MinionManager::sharedManager()->m_pMinion1[i]->GetBoundSizeX()*2);

		if( dist > 0 )
		{
			HeroManager::sharedManager()->m_pHero->SetTarget(MinionManager::sharedManager()->m_pMinion1[i]);
			HeroManager::sharedManager()->m_pHero->SetTargetID(MinionManager::sharedManager()->m_pMinion1[i]->GetID());
			return;
		}
	}


	HeroManager::sharedManager()->m_pHero->SetTarget(NULL);
}
