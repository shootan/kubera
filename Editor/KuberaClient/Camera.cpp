#include "Camera.h"


CCamera::CCamera() 
{
}

CCamera::~CCamera() 
{ 
}

void CCamera::CreateShaderVariables(ID3D11Device *pd3dDevice)
{
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VS_CB_VIEWPROJECTION_MATRIX);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	pd3dDevice->CreateBuffer(&bd, NULL, &m_pd3dcbCamera);
}

void CCamera::UpdateShaderVariables(ID3D11DeviceContext *pd3dDeviceContext)
{
	D3DXMATRIX m_d3dxmtxView = *GetViewMatrix();         
	D3DXMATRIX m_d3dxmtxProjection = *GetProjMatrix();  


	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	/*��� ������ �޸� �ּҸ� �����ͼ� ī�޶� ��ȯ ��İ� ���� ��ȯ ����� �����Ѵ�. ���̴����� ����� ��� ���� �ٲ�� �Ϳ� �����϶�.*/
	pd3dDeviceContext->Map(m_pd3dcbCamera, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	VS_CB_VIEWPROJECTION_MATRIX *pcbViewProjection = (VS_CB_VIEWPROJECTION_MATRIX *)d3dMappedResource.pData;
	D3DXMatrixTranspose(&pcbViewProjection->m_d3dxmtxView, &m_d3dxmtxView);
	D3DXMatrixTranspose(&pcbViewProjection->m_d3dxmtxProjection, &m_d3dxmtxProjection);
	pd3dDeviceContext->Unmap(m_pd3dcbCamera, 0);

	//��� ���۸� ����(VS_SLOT_CAMERA)�� �����Ѵ�.
	pd3dDeviceContext->VSSetConstantBuffers(VS_SLOT_VIEWPROJECTION_MATRIX, 1, &m_pd3dcbCamera);
}

void CCamera::SetViewport(ID3D11DeviceContext *pd3dDeviceContext, DWORD xTopLeft, DWORD yTopLeft, DWORD nWidth, DWORD nHeight, float fMinZ, float fMaxZ)
{
	m_d3dViewport.TopLeftX = float(xTopLeft);
	m_d3dViewport.TopLeftY = float(yTopLeft);
	m_d3dViewport.Width = float(nWidth);
	m_d3dViewport.Height = float(nHeight);
	m_d3dViewport.MinDepth = fMinZ;
	m_d3dViewport.MaxDepth = fMaxZ;

	//pd3dDeviceContext->RSSetViewports(2, m_d3dViewport);
}

void CCamera::CalculateFrustumPlanes()
{
	/*ī�޶� ��ȯ ��İ� ���� ���� ��ȯ ����� ���� ����� ����Ͽ� ����ü ������ ���Ѵ�. �� ���� ��ǥ�迡�� ����ü �ø��� �Ѵ�.*/
	D3DXMATRIX m_d3dxmtxView = *GetViewMatrix();
	D3DXMATRIX m_d3dxmtxProjection = *GetProjMatrix();

	D3DXMATRIX mtxViewProject = m_d3dxmtxView * m_d3dxmtxProjection;

	//����ü�� ���� ���
	m_d3dxFrustumPlanes[0].a = -(mtxViewProject._14 + mtxViewProject._11);
	m_d3dxFrustumPlanes[0].b = -(mtxViewProject._24 + mtxViewProject._21);
	m_d3dxFrustumPlanes[0].c = -(mtxViewProject._34 + mtxViewProject._31);
	m_d3dxFrustumPlanes[0].d = -(mtxViewProject._44 + mtxViewProject._41);

	//����ü�� ������ ���
	m_d3dxFrustumPlanes[1].a = -(mtxViewProject._14 - mtxViewProject._11);
	m_d3dxFrustumPlanes[1].b = -(mtxViewProject._24 - mtxViewProject._21);
	m_d3dxFrustumPlanes[1].c = -(mtxViewProject._34 - mtxViewProject._31);
	m_d3dxFrustumPlanes[1].d = -(mtxViewProject._44 - mtxViewProject._41);

	//����ü�� ���� ���
	m_d3dxFrustumPlanes[2].a = -(mtxViewProject._14 - mtxViewProject._12);
	m_d3dxFrustumPlanes[2].b = -(mtxViewProject._24 - mtxViewProject._22);
	m_d3dxFrustumPlanes[2].c = -(mtxViewProject._34 - mtxViewProject._32);
	m_d3dxFrustumPlanes[2].d = -(mtxViewProject._44 - mtxViewProject._42);

	//����ü�� �Ʒ��� ���
	m_d3dxFrustumPlanes[3].a = -(mtxViewProject._14 + mtxViewProject._12);
	m_d3dxFrustumPlanes[3].b = -(mtxViewProject._24 + mtxViewProject._22);
	m_d3dxFrustumPlanes[3].c = -(mtxViewProject._34 + mtxViewProject._32);
	m_d3dxFrustumPlanes[3].d = -(mtxViewProject._44 + mtxViewProject._42);

	//����ü�� �����
	m_d3dxFrustumPlanes[4].a = -(mtxViewProject._13);
	m_d3dxFrustumPlanes[4].b = -(mtxViewProject._23);
	m_d3dxFrustumPlanes[4].c = -(mtxViewProject._33);
	m_d3dxFrustumPlanes[4].d = -(mtxViewProject._43);

	//����ü�� �����
	m_d3dxFrustumPlanes[5].a = -(mtxViewProject._14 - mtxViewProject._13);
	m_d3dxFrustumPlanes[5].b = -(mtxViewProject._24 - mtxViewProject._23);
	m_d3dxFrustumPlanes[5].c = -(mtxViewProject._34 - mtxViewProject._33);
	m_d3dxFrustumPlanes[5].d = -(mtxViewProject._44 - mtxViewProject._43);

	/*����ü�� �� ����� ���� ���� (a, b. c)�� ũ��� a, b, c, d�� ������. ��, ���� ���͸� ����ȭ�ϰ� �������� �������� �Ÿ��� ����Ѵ�.*/
	for (int i = 0; i < 6; i++) D3DXPlaneNormalize(&m_d3dxFrustumPlanes[i], &m_d3dxFrustumPlanes[i]);
}


bool CCamera::IsInFrustum(D3DXVECTOR3& d3dxvMinimum, D3DXVECTOR3& d3dxvMaximum)
{
	D3DXVECTOR3 d3dxvNearPoint, d3dxvFarPoint, d3dxvNormal;
	for (int i = 0; i < 6; i++)
	{
		/*����ü�� �� ��鿡 ���Ͽ� �ٿ�� �ڽ��� �������� ����Ѵ�. �������� x, y, z ��ǥ�� ���� ������ �� ��Ұ� �����̸� �ٿ�� �ڽ��� �ִ����� ��ǥ�� �ǰ� �׷��� ������ �ٿ�� �ڽ��� �ּ����� ��ǥ�� �ȴ�.*/ 
		d3dxvNormal = D3DXVECTOR3(m_d3dxFrustumPlanes[i].a, m_d3dxFrustumPlanes[i].b, m_d3dxFrustumPlanes[i].c);
		if (d3dxvNormal.x >= 0.0f)
		{
			if (d3dxvNormal.y >= 0.0f)
			{
				if (d3dxvNormal.z >= 0.0f) 
				{
					//���� ������ x, y, z ��ǥ�� ��ȣ�� ��� ����̹Ƿ� �������� �ٿ�� �ڽ��� �ּ����̴�.
					d3dxvNearPoint.x = d3dxvMinimum.x; d3dxvNearPoint.y = d3dxvMinimum.y; d3dxvNearPoint.z = d3dxvMinimum.z;                
				} 
				else 
				{
					/*���� ������ x, y ��ǥ�� ��ȣ�� ��� ����̹Ƿ� �������� x, y ��ǥ�� �ٿ�� �ڽ��� �ּ����� x, y ��ǥ�̰� ���� ������ z ��ǥ�� ����̹Ƿ� �������� z ��ǥ�� �ٿ�� �ڽ��� �ִ����� z ��ǥ�̴�.*/
					d3dxvNearPoint.x = d3dxvMinimum.x; d3dxvNearPoint.y = d3dxvMinimum.y; d3dxvNearPoint.z = d3dxvMaximum.z;                 
				} 
			} 
			else
			{
				if (d3dxvNormal.z >= 0.0f) 
				{
					/*���� ������ x, z ��ǥ�� ��ȣ�� ��� ����̹Ƿ� �������� x, z ��ǥ�� �ٿ�� �ڽ��� �ּ����� x, z ��ǥ�̰� ���� ������ y ��ǥ�� ����̹Ƿ� �������� y ��ǥ�� �ٿ�� �ڽ��� �ִ����� y ��ǥ�̴�.*/
					d3dxvNearPoint.x = d3dxvMinimum.x; d3dxvNearPoint.y = d3dxvMaximum.y; d3dxvNearPoint.z = d3dxvMinimum.z;
				} 
				else 
				{
					/*���� ������ y, z ��ǥ�� ��ȣ�� ��� �����̹Ƿ� �������� y, z ��ǥ�� �ٿ�� �ڽ��� �ִ����� y, z ��ǥ�̰� ���� ������ x ��ǥ�� ����̹Ƿ� �������� x ��ǥ�� �ٿ�� �ڽ��� �ּ����� x ��ǥ�̴�.*/
					d3dxvNearPoint.x = d3dxvMinimum.x; d3dxvNearPoint.y = d3dxvMaximum.y; d3dxvNearPoint.z = d3dxvMaximum.z;                 
				} 
			} 
		} 
		else
		{
			if (d3dxvNormal.y >= 0.0f)
			{
				if (d3dxvNormal.z >= 0.0f) 
				{
					/*���� ������ y, z ��ǥ�� ��ȣ�� ��� ����̹Ƿ� �������� y, z ��ǥ�� �ٿ�� �ڽ��� �ּ����� y, z ��ǥ�̰� ���� ������ x ��ǥ�� �����̹Ƿ� �������� x ��ǥ�� �ٿ�� �ڽ��� �ִ����� x ��ǥ�̴�.*/
					d3dxvNearPoint.x = d3dxvMaximum.x; d3dxvNearPoint.y = d3dxvMinimum.y; d3dxvNearPoint.z = d3dxvMinimum.z;
				} 
				else 
				{
					/*���� ������ x, z ��ǥ�� ��ȣ�� ��� �����̹Ƿ� �������� x, z ��ǥ�� �ٿ�� �ڽ��� �ִ����� x, z ��ǥ�̰� ���� ������ y ��ǥ�� ����̹Ƿ� �������� y ��ǥ�� �ٿ�� �ڽ��� �ּ����� y ��ǥ�̴�.*/
					d3dxvNearPoint.x = d3dxvMaximum.x; d3dxvNearPoint.y = d3dxvMinimum.y; d3dxvNearPoint.z = d3dxvMaximum.z;                 
				} 
			} 
			else
			{
				if (d3dxvNormal.z >= 0.0f) 
				{
					/*���� ������ x, y ��ǥ�� ��ȣ�� ��� �����̹Ƿ� �������� x, y ��ǥ�� �ٿ�� �ڽ��� �ִ����� x, y ��ǥ�̰� ���� ������ z ��ǥ�� ����̹Ƿ� �������� z ��ǥ�� �ٿ�� �ڽ��� �ּ����� z ��ǥ�̴�.*/
					d3dxvNearPoint.x = d3dxvMaximum.x; d3dxvNearPoint.y = d3dxvMaximum.y; d3dxvNearPoint.z = d3dxvMinimum.z;                
				} 
				else 
				{
					//���� ������ x, y, z ��ǥ�� ��ȣ�� ��� �����̹Ƿ� �������� �ٿ�� �ڽ��� �ִ����̴�.
					d3dxvNearPoint.x = d3dxvMaximum.x; d3dxvNearPoint.y = d3dxvMaximum.y; d3dxvNearPoint.z = d3dxvMaximum.z;                 
				} 
			} 
		} 
		/*�������� ����ü ��� �� �ϳ��� ����� �ٱ�(��)�ʿ� ������ �������� ����ü�� ���Ե��� �ʴ´�. �������� � ����� �ٱ�(��)�ʿ� ������ �������� ����� �����Ŀ� �����ϸ� ��ȣ�� ����� �ȴ�. �� ����� �����Ŀ� �������� �����ϴ� ���� ������ (x, y, z)�� ����� �������� (a, b, c)�� ������ �������� �������� �Ÿ��� ���� �Ͱ� ����.*/
		if ((D3DXVec3Dot(&d3dxvNormal, &d3dxvNearPoint) + m_d3dxFrustumPlanes[i].d) > 0.0f) return(false);
	} 
	return(true);
}

bool CCamera::IsInFrustum(AABB *pAABB)
{
	return(IsInFrustum(pAABB->m_d3dxvMinimum, pAABB->m_d3dxvMaximum));
}