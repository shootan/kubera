#pragma once
#include "header.h"
#include "HeroObject.h"
#include "TowerManager.h"
#include "ST.h"

class ControlManager 
{
public:
	HeroObject*				m_Player;
	CModelViewerCamera*		m_Camera;

	//
	void TouchRightDown(float _x, float _y , HWND hWnd);
	void TouchLeftDown(float _x, float _y , HWND hWnd);
	void AssignSelectedUnitsToNewDestination ( const D3DXVECTOR3 &vec );
	void CalculateScreenRayFromCoordinates( float x, float y, D3DXVECTOR3 &vRayDirection , HWND hWnd);
	float INTersectRaySphere(const D3DXVECTOR3 vRayDirection, D3DXVECTOR3 vRayOrigin, D3DXVECTOR3 vSphereOrigin,  float fpRadiusSquared);

	void SetTargetTower(const D3DXVECTOR3 vRayDirection, D3DXVECTOR3 vRayOrigin);

};