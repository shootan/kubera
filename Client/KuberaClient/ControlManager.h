#pragma once
#include "header.h"
#include "HeroObject.h"

class ControlManager 
{
public:
	HeroObject*				m_Player;
	CModelViewerCamera*		m_Camera;

	//
	void TouchDown(float _x, float _y , HWND hWnd);
	void AssignSelectedUnitsToNewDestination ( const D3DXVECTOR3 &vec );
	void CalculateScreenRayFromCoordinates( float x, float y, D3DXVECTOR3 &vRayDirection , HWND hWnd);

};