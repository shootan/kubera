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
	//����ü�� 6�� ����� ����Ѵ�.
	void CalculateFrustumPlanes();
	//�ٿ�� �ڽ��� ����ü�� ������ ���Եǰų� �Ϻζ� ���ԵǴ� ���� �˻��Ѵ�.
	bool IsInFrustum(D3DXVECTOR3& d3dxvMinimum, D3DXVECTOR3& d3dxvMaximum);
	bool IsInFrustum(AABB *pAABB);

	void CreateShaderVariables(ID3D11Device *pd3dDevice);
	void UpdateShaderVariables(ID3D11DeviceContext *pd3dDeviceContext);
	//��-��Ʈ�� �����ϴ� ��� �Լ��� �����Ѵ�.
	void SetViewport(ID3D11DeviceContext *pd3dDeviceContext, DWORD xStart, DWORD yStart, DWORD nWidth, DWORD nHeight, float fMinZ=0.0f, float fMaxZ=1.0f);
	D3D11_VIEWPORT GetViewport() { return(m_d3dViewport); }

};