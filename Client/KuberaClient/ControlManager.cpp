#include "ControlManager.h"

void ControlManager::TouchRightDown(float _x, float _y, HWND hWnd)
{
	D3DXVECTOR3 vRayDirection;
	D3DXVECTOR3 vRayOrigin = *m_Camera->GetEyePt();
	//D3DXVECTOR3 vRayOrigin = m_Camera->GetPosition();
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
			if(HeroManager::sharedManager()->m_pHero->GetState() != WAIT && 
				HeroManager::sharedManager()->m_pHero->GetState() != DEATH)  //죽으뒤 리스폰시간에 움직이면 move로 바뀌기때문에 설정 부활이 우선
			{
				HeroManager::sharedManager()->m_pHero->SetNewDestination(vGroundPlaneHit);
				//printf("X: %.1f y: %.1f z: %.1f \n", vGroundPlaneHit.x, vGroundPlaneHit.y, vGroundPlaneHit.z);
				HeroManager::sharedManager()->m_pHero->SetState(MOVE);
			}
		}
	}     

	SetTarget(vRayDirection, vRayOrigin);
}

void ControlManager::TouchLeftDown(float _x, float _y , HWND hWnd)
{
	D3DXVECTOR3 vRayDirection;
	D3DXVECTOR3 vRayOrigin = *m_Camera->GetEyePt();
	//D3DXVECTOR3 vRayOrigin = m_Camera->GetPosition();
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
	//const D3DXMATRIX* pmatProj = &m_Camera->GetProjectionMatrix();

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
	//const D3DXMATRIX matView = m_Camera->GetViewMatrix();
	//D3DXMATRIX mx;
	//D3DXMatrixIdentity(&mx);
	//const D3DXMATRIX matWorld =mx;
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
		if(TowerManager::sharedManager()->m_pTower[i] == NULL) continue;
		//if(TowerManager::sharedManager()->m_pTower[i]->GetSelected() == TRUE) continue;
		if(TowerManager::sharedManager()->m_pTower[i]->GetHP() <= 0.0f) continue;

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

	for(int i=0; i<MAX_MINION; i++) //미니언 타겟 잡기
	{
		if(MinionManager::sharedManager()->m_pMinion[i] == NULL) continue;

		float dist = INTersectRaySphere(vRayDirection, vRayOrigin, MinionManager::sharedManager()->m_pMinion[i]->GetPosition(), 
			MinionManager::sharedManager()->m_pMinion[i]->GetBoundSizeX()*2);

		if( dist > 0 )
		{
			HeroManager::sharedManager()->m_pHero->SetTarget(MinionManager::sharedManager()->m_pMinion[i]);
			HeroManager::sharedManager()->m_pHero->SetTargetID(MinionManager::sharedManager()->m_pMinion[i]->GetID());
			return;
		}
	}

	//다른 플레이어 타겟 잡기
	
	float dist = INTersectRaySphere(vRayDirection, vRayOrigin, OtherPlayerManager::sharedManager()->m_pOtherPlayer->GetPosition(), 
		OtherPlayerManager::sharedManager()->m_pOtherPlayer->GetBoundSizeX()*2);

	if( dist > 0 )
	{
		HeroManager::sharedManager()->m_pHero->SetTarget(OtherPlayerManager::sharedManager()->m_pOtherPlayer);
		HeroManager::sharedManager()->m_pHero->SetTargetID(OtherPlayerManager::sharedManager()->m_pOtherPlayer->GetID());
		return;
	}
	

	//넥서스 타겟
	dist = INTersectRaySphere(vRayDirection, vRayOrigin, HeroManager::sharedManager()->RedNexus->GetPosition(), 
		HeroManager::sharedManager()->RedNexus->GetBoundSizeX()*2);
	if(dist> 0)
	{
		HeroManager::sharedManager()->m_pHero->SetTarget(HeroManager::sharedManager()->RedNexus);
		HeroManager::sharedManager()->m_pHero->SetTargetID(HeroManager::sharedManager()->RedNexus->GetID());
		return;
	}

	dist = INTersectRaySphere(vRayDirection, vRayOrigin, HeroManager::sharedManager()->BlueNexus->GetPosition(), 
		HeroManager::sharedManager()->BlueNexus->GetBoundSizeX()*2);
	if(dist> 0)
	{
		HeroManager::sharedManager()->m_pHero->SetTarget(HeroManager::sharedManager()->BlueNexus);
		HeroManager::sharedManager()->m_pHero->SetTargetID(HeroManager::sharedManager()->BlueNexus->GetID());
		return;
	}

	HeroManager::sharedManager()->m_pHero->SetTarget(NULL);
	HeroManager::sharedManager()->m_pHero->SetTargetID(0);
}
