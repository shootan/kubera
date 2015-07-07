#pragma once
#include "header.h"
#include "Mesh.h"




struct VS_CB_VIEWPROJECTION_MATRIX
{
	D3DXMATRIX m_d3dxmtxView;         
	D3DXMATRIX m_d3dxmtxProjection;   
};

class CCamera : public CModelViewerCamera
{
public:
	CCamera();
	virtual ~CCamera();

public:
	ID3D11Buffer *m_pd3dcbCamera;
	D3D11_VIEWPORT m_d3dViewport;
	D3DXPLANE m_d3dxFrustumPlanes[6];    

	D3DXVECTOR3 m_CameraPos;
public:
	//절두체의 6개 평면을 계산한다.
	void CalculateFrustumPlanes();
	//바운딩 박스가 절두체에 완전히 포함되거나 일부라도 포함되는 가를 검사한다.
	bool IsInFrustum(D3DXVECTOR3& d3dxvMinimum, D3DXVECTOR3& d3dxvMaximum);
	bool IsInFrustum(AABB *pAABB);

	void CreateShaderVariables(ID3D11Device *pd3dDevice);
	void UpdateShaderVariables(ID3D11DeviceContext *pd3dDeviceContext);
	//뷰-포트를 설정하는 멤버 함수를 선언한다.
	void SetViewport(ID3D11DeviceContext *pd3dDeviceContext, DWORD xStart, DWORD yStart, DWORD nWidth, DWORD nHeight, float fMinZ=0.0f, float fMaxZ=1.0f);
	D3D11_VIEWPORT GetViewport() { return(m_d3dViewport); }

};