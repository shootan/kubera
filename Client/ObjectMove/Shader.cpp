#include "StdAfx.h"
#include "Shader.h"


CShader::CShader()
{
	m_pd3dVertexShader = NULL;
	m_pd3dPixelShader = NULL;
	m_pd3dVertexLayout = NULL;
}


CShader::~CShader()
{
	if (m_pd3dVertexShader) m_pd3dVertexShader->Release();
	if (m_pd3dPixelShader) m_pd3dPixelShader->Release();
	if (m_pd3dVertexLayout) m_pd3dVertexLayout->Release();
}

void CShader::CreateVertexShaderFromFile(ID3D11Device *pd3dDevice, WCHAR *pszFileName, LPCSTR pszShaderName, 
	LPCSTR pszShaderModel, ID3D11VertexShader **ppd3dVertexShader, D3D11_INPUT_ELEMENT_DESC *pd3dInputLayout,
	UINT nElements, ID3D11InputLayout **ppd3dVertexLayout)
{
	HRESULT hResult;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined (DEBUG) || defined (_DEBUG)
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob *pd3dShaderBlob = NULL, *pd3dErrorBlob = NULL;

	/* ����(pszfileName)���� ���̴��Լ� (pszShaderName)�� �������Ͽ� ������ �� ���̴� �ڵ��� �޸��ּ� (pd3dShaderBlob)�� ��ȯ�Ѵ� */
	if (SUCCEEDED (hResult = D3DX11CompileFromFile (pszFileName, NULL, NULL, pszShaderName, pszShaderModel, dwShaderFlags, 0, NULL, &pd3dShaderBlob, &pd3dErrorBlob, NULL)))
	{
		// �����ϵ� ���̴� �ڵ��� �޸� �ּҿ��� ����-���̴��� �����Ѵ�.
		pd3dDevice->CreateVertexShader(pd3dShaderBlob->GetBufferPointer(), pd3dShaderBlob->GetBufferSize(), NULL, ppd3dVertexShader);
		
		// �����ϵ� ���̴� �ڵ��� �޸� �ּҿ� �Է� ���̾ƿ����� ���� ���̾ƿ��� �����Ѵ�.
		pd3dDevice->CreateInputLayout(pd3dInputLayout, nElements, pd3dShaderBlob->GetBufferPointer(), pd3dShaderBlob->GetBufferSize(), ppd3dVertexLayout);

		pd3dShaderBlob->Release();
	}
}

void CShader::CreatePixelShaderFromFile(ID3D11Device *pd3dDevice, WCHAR *pszFileName, LPCSTR pszShaderName, 
	LPCSTR pszShaderModel, ID3D11PixelShader **ppd3dPixelShader)
{
	HRESULT hResult;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined (DEBUG) || defined (_DEBUG)
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob *pd3dShaderBlob = NULL, *pd3dErrorBlob = NULL;

	/* ���� (pszFileName)���� ���̴� �Լ� (pszShaderName)�� �������Ͽ� �����ϵ� ���̴� �ڵ��� �޸��ּ� (pd3dShaderBlob)�� ��ȯ�Ѵ�*/
	if (SUCCEEDED (hResult = D3DX11CompileFromFile (pszFileName, NULL, NULL, pszShaderName, pszShaderModel, dwShaderFlags, 0, NULL, &pd3dShaderBlob, &pd3dErrorBlob, NULL)))
	{
		// �����ϵ� ���̴� �ڵ��� �޸� �ּҿ��� �ȼ�-���̴��� �����Ѵ�.
		pd3dDevice->CreatePixelShader(pd3dShaderBlob->GetBufferPointer(), pd3dShaderBlob->GetBufferSize(), NULL, ppd3dPixelShader);
		pd3dShaderBlob->Release();
	}
}

void CShader::CreateShader(ID3D11Device *pd3dDevice, int nObjects)
{

}



void CShader::Render(ID3D11DeviceContext *pd3dDeviceContext)
{
	// ������ �Է�-���̾ƿ��� ����̽� ���ؽ�Ʈ�� ����(����)�Ѵ�.
	if (m_pd3dVertexLayout)
		pd3dDeviceContext->IASetInputLayout(m_pd3dVertexLayout);
	// ����-���̴��� ����̽� ���ؽ�Ʈ�� ����(����) �Ѵ�.
	if (m_pd3dVertexShader) pd3dDeviceContext->VSSetShader(m_pd3dVertexShader, NULL, 0);
	// �ȼ�-���̴��� ����̽� ���ؽ�Ʈ�� ����(����) �Ѵ�.
	if (m_pd3dPixelShader) pd3dDeviceContext->PSSetShader(m_pd3dPixelShader, NULL, 0);
}

void CShader::CreateShaderVariables(ID3D11Device *pd3dDevice) {}

void CShader::ReleaseShaderVariables() {}

void CShader::UpdateShaderVariables(ID3D11DeviceContext *pd3dDeviceContext) {}

CDiffusedShader::CDiffusedShader()
{

}

CDiffusedShader::~CDiffusedShader()
{

}

void CDiffusedShader::CreateShader (ID3D11Device *pd3dDevice)
{
	D3D11_INPUT_ELEMENT_DESC d3dInputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT nElements = ARRAYSIZE(d3dInputLayout);
	CreateVertexShaderFromFile(pd3dDevice, L"Effect.fx", "VSDiffusedColor", "vs_4_0", &m_pd3dVertexShader, d3dInputLayout, nElements, &m_pd3dVertexLayout);
	CreatePixelShaderFromFile(pd3dDevice, L"Effect.fx", "PSDiffusedColor", "ps_4_0", &m_pd3dPixelShader);
}

void CDiffusedShader::CreateShaderVariables (ID3D11Device *pd3dDevice)
{
	CShader::CreateShaderVariables(pd3dDevice);
}

void CDiffusedShader::UpdateShaderVariables (ID3D11DeviceContext *pd3dDeviceContext)
{
	CShader::UpdateShaderVariables(pd3dDeviceContext);
}

void CDiffusedShader::Render (ID3D11DeviceContext *pd3dDeviceContext)
{
	CShader::Render(pd3dDeviceContext);
}

CSceneShader::CSceneShader()
{
	m_ppObjects = NULL;
	m_nObjects = 0;
	m_pd3dcbWorldMatrix = NULL;
}

CSceneShader::~CSceneShader()
{

}

void CSceneShader::BuildObjects(ID3D11Device *pd3dDevice)
{
	//����x����x���̰� 12x12x12�� ������ü �޽��� �����Ѵ�.
	CCubeMesh *pCubeMesh = new CCubeMesh(pd3dDevice, 12.0f, 12.0f, 12.0f);

	int xObjects = 0.5, yObjects = 0.5, zObjects = 0.5, i = 0;
	/*<�׸� 21>�� ���� x-��, y-��, z-������ 23���� �� 23 x 23 x 23 = 12,167���� ������ü�� ��ġ�Ѵ�. 
	x-��, y-��, z-�࿡�� ������ �������� 11��, ���������� 11��, ������ 1�� ��ġ�Ѵ�. 
	���� ������ ����Ʈ�� �ʹ� ������ 11�� 6������ �ٲٵ��� ����.*/  
	m_nObjects = (xObjects*2+1)*(yObjects*2+1)*(zObjects*2+1);
	m_ppObjects = new CGameObject*[m_nObjects]; 

	float fxPitch = 12.0f * 1.9f;
	float fyPitch = 12.0f * 1.9f;
	float fzPitch = 12.0f * 1.9f;
	//������ü ��ü�� �����Ѵ�.
	CRotatingObject *pRotatingObject = NULL;
	for (int x = -xObjects; x <= xObjects; x++)
	{
		for (int y = -yObjects; y <= yObjects; y++)
		{
			for (int z = -zObjects; z <= zObjects; z++)
			{
				pRotatingObject = new CRotatingObject();
				pRotatingObject->SetMesh(pCubeMesh);
				//�� ������ü ��ü�� ��ġ�� �����Ѵ�.
				pRotatingObject->SetPosition(fxPitch*x, fyPitch*y, fzPitch*z);
				//�� ������ü ��ü�� ȸ������ y-������ �����Ѵ�.
				pRotatingObject->SetRotationAxis(D3DXVECTOR3(0.0f, 1.0f, 0.0f));
				pRotatingObject->SetRotationSpeed(10.0f*(i % 10));
				m_ppObjects[i++] = pRotatingObject;
			}
		}
	}

	CreateShaderVariables(pd3dDevice);
}

void CSceneShader::ReleaseObjects()
{
	if (m_pd3dcbWorldMatrix) m_pd3dcbWorldMatrix->Release();

	if (m_ppObjects)
	{
		for (int j = 0; j < m_nObjects; j++) delete m_ppObjects[j];
		delete [] m_ppObjects;
	}
}

void CSceneShader::AnimateObjects(float fTimeElapsed)
{
	for (int j = 0; j < m_nObjects; j++)
	{
		m_ppObjects[j]->Animate(fTimeElapsed);
	}
}

void CSceneShader::CreateShader(ID3D11Device *pd3dDevice)
{
	CDiffusedShader::CreateShader(pd3dDevice);
}

void CSceneShader::CreateShaderVariables(ID3D11Device *pd3dDevice)
{
	CDiffusedShader::CreateShaderVariables(pd3dDevice);

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VS_CB_WORLD_MATRIX);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	pd3dDevice->CreateBuffer(&bd, NULL, &m_pd3dcbWorldMatrix);
}

void CSceneShader::UpdateShaderVariables(ID3D11DeviceContext *pd3dDeviceContext, D3DXMATRIX *pd3dxmtxWorld)
{
	CDiffusedShader::UpdateShaderVariables(pd3dDeviceContext);

	//Update World Matrix Constant Buffer
	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	pd3dDeviceContext->Map(m_pd3dcbWorldMatrix, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	VS_CB_WORLD_MATRIX *pcbWorldMatrix = (VS_CB_WORLD_MATRIX *)d3dMappedResource.pData;
	D3DXMatrixTranspose(&pcbWorldMatrix->m_d3dxmtWorld, pd3dxmtxWorld);
	pd3dDeviceContext->Unmap(m_pd3dcbWorldMatrix, 0);
	pd3dDeviceContext->VSSetConstantBuffers(VS_SLOT_WORLD_MATRIX, 1, &m_pd3dcbWorldMatrix);
}

void CSceneShader::Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera /* = NULL */ )
{
	CDiffusedShader::Render(pd3dDeviceContext);

	for (int j = 0; j < m_nObjects; j++)
	{
		if (m_ppObjects[j]) 
		{
			UpdateShaderVariables(pd3dDeviceContext, &m_ppObjects[j]->m_d3dxmtxWorld);
			m_ppObjects[j]->Render(pd3dDeviceContext, pCamera);
		}
	}
}

CPlayerShader::CPlayerShader()
{
	m_pd3dcbWorldMatrix = NULL;
}

CPlayerShader::~CPlayerShader()
{
	if (m_pd3dcbWorldMatrix) m_pd3dcbWorldMatrix->Release();
}

void CPlayerShader::CreateShader(ID3D11Device *pd3dDevice)
{
	CDiffusedShader::CreateShader(pd3dDevice);
}

void CPlayerShader::CreateShaderVariables(ID3D11Device *pd3dDevice)
{
	CDiffusedShader::CreateShaderVariables(pd3dDevice);

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VS_CB_WORLD_MATRIX);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	pd3dDevice->CreateBuffer(&bd, NULL, &m_pd3dcbWorldMatrix);
}

void CPlayerShader::UpdateShaderVariables(ID3D11DeviceContext *pd3dDeviceContext, D3DXMATRIX *pd3dxmtxWorld)
{
	CDiffusedShader::UpdateShaderVariables(pd3dDeviceContext);

	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	pd3dDeviceContext->Map(m_pd3dcbWorldMatrix, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	VS_CB_WORLD_MATRIX *pcbWorldMatrix = (VS_CB_WORLD_MATRIX *)d3dMappedResource.pData;
	D3DXMatrixTranspose(&pcbWorldMatrix->m_d3dxmtWorld, pd3dxmtxWorld);
	pd3dDeviceContext->Unmap(m_pd3dcbWorldMatrix, 0);
	pd3dDeviceContext->VSSetConstantBuffers(VS_SLOT_WORLD_MATRIX, 1, &m_pd3dcbWorldMatrix);
}

