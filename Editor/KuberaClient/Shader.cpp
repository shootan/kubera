#include "Shader.h"
#include "MapEditorManager.h"

CShader::CShader(void)
{
	m_pd3dVertexShader = NULL;
	m_pd3dPixelShader = NULL;
	m_pd3dVertexLayout = NULL;
	m_pSamLinear = NULL;

	m_ppObjects = NULL;       
	m_nObjects = 0;
	m_nIndexToAdd = 0;

	m_pCamera = NULL;
}


CShader::~CShader(void)
{
	if (m_pd3dVertexShader) m_pd3dVertexShader->Release();
	if (m_pd3dPixelShader) m_pd3dPixelShader->Release();
	if (m_pd3dVertexLayout) m_pd3dVertexLayout->Release();
	if (m_pSamLinear) m_pSamLinear->Release();
}

void CShader::CreateVertexShaderFromFile(ID3D11Device *pd3dDevice, WCHAR *pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderModel, ID3D11VertexShader **ppd3dVertexShader, D3D11_INPUT_ELEMENT_DESC *pd3dInputLayout, UINT nElements, ID3D11InputLayout **ppd3dVertexLayout)
{
	HRESULT hResult;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob *pd3dShaderBlob = NULL, *pd3dErrorBlob = NULL;
	/*파일(pszFileName)에서 쉐이더 함수(pszShaderName)를 컴파일하여 컴파일된 쉐이더 코드의 메모리 주소(pd3dShaderBlob)를 반환한다.*/ 
	if (SUCCEEDED(hResult = D3DX11CompileFromFile(pszFileName, NULL, NULL, pszShaderName, pszShaderModel, dwShaderFlags, 0, NULL, &pd3dShaderBlob, &pd3dErrorBlob, NULL)))
	{
		//컴파일된 쉐이더 코드의 메모리 주소에서 정점-쉐이더를 생성한다. 
		pd3dDevice->CreateVertexShader(pd3dShaderBlob->GetBufferPointer(), pd3dShaderBlob->GetBufferSize(), NULL, ppd3dVertexShader);
		//컴파일된 쉐이더 코드의 메모리 주소와 입력 레이아웃에서 정점 레이아웃을 생성한다. 
		pd3dDevice->CreateInputLayout(pd3dInputLayout, nElements, pd3dShaderBlob->GetBufferPointer(), pd3dShaderBlob->GetBufferSize(), ppd3dVertexLayout);
		pd3dShaderBlob->Release();
	}
}

void CShader::CreatePixelShaderFromFile(ID3D11Device *pd3dDevice, WCHAR *pszFileName, LPCSTR pszShaderName, LPCSTR pszShaderModel, ID3D11PixelShader **ppd3dPixelShader)
{
	HRESULT hResult;

	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

	ID3DBlob *pd3dShaderBlob = NULL, *pd3dErrorBlob = NULL;
	/*파일(pszFileName)에서 쉐이더 함수(pszShaderName)를 컴파일하여 컴파일된 쉐이더 코드의 메모리 주소(pd3dShaderBlob)를 반환한다.*/ 
	if (SUCCEEDED(hResult = D3DX11CompileFromFile(pszFileName, NULL, NULL, pszShaderName, pszShaderModel, dwShaderFlags, 0, NULL, &pd3dShaderBlob, &pd3dErrorBlob, NULL)))
	{
		//컴파일된 쉐이더 코드의 메모리 주소에서 픽셀-쉐이더를 생성한다. 
		pd3dDevice->CreatePixelShader(pd3dShaderBlob->GetBufferPointer(), pd3dShaderBlob->GetBufferSize(), NULL, ppd3dPixelShader);
		pd3dShaderBlob->Release();
	}
}

void CShader::CreateShader(ID3D11Device *pd3dDevice, int nObjects)
{

}

void CShader::Render(ID3D11DeviceContext *pd3dDeviceContext)
{
	//정점의 입력-레이아웃을 디바이스 컨텍스트에 연결(설정)한다. 
	if (m_pd3dVertexLayout) pd3dDeviceContext->IASetInputLayout(m_pd3dVertexLayout);
	//정점-쉐이더를 디바이스 컨텍스트에 연결(설정)한다. 
	if (m_pd3dVertexShader) pd3dDeviceContext->VSSetShader(m_pd3dVertexShader, NULL, 0);
	//픽셀-쉐이더를 디바이스 컨텍스트에 연결(설정)한다. 
	if (m_pd3dPixelShader) pd3dDeviceContext->PSSetShader(m_pd3dPixelShader, NULL, 0);

	if (m_pSamLinear) pd3dDeviceContext->PSSetSamplers( 0, 1, &m_pSamLinear );
}

void CShader::CreateShaderVariables(ID3D11Device *pd3dDevice)
{
}

void CShader::ReleaseShaderVariables()
{
}

void CShader::UpdateShaderVariables(ID3D11DeviceContext *pd3dDeviceContext)
{
}


CObjectShader::CObjectShader()
{
	CShader::CShader();

	m_pd3dcbWorldMatrix = NULL;
}

CObjectShader::~CObjectShader()
{
	for (int j = 0; j < m_nObjects; j++) if (m_ppObjects[j]) m_ppObjects[j]->Release();
	if (m_ppObjects) delete [] m_ppObjects;       

	ReleaseShaderVariables();

}

void CObjectShader::AddObject(CGameObject *pObject) 
{ 
	//쉐이더에 객체를 연결(설정)하고 참조 카운터를 1만큼 증가시킨다. 
	if (m_ppObjects) m_ppObjects[m_nIndexToAdd++] = pObject; 
	if (pObject) pObject->AddRef();
}

void CObjectShader::CreateShader(ID3D11Device *pd3dDevice, int nObjects)
{
	/*IA 단계에 설정할 입력-레이아웃을 정의한다. 정점 버퍼의 한 원소가 CVertex 클래스의 멤버 변수(D3DXVECTOR3 즉, 실수 세 개)이므로 입력-레이아웃은 실수(32-비트) 3개로 구성되며 시멘틱이 “POSITION”이고 정점 데이터임을 표현한다.*/ 
	D3D11_INPUT_ELEMENT_DESC d3dInputLayout[] =
	{
		{ "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT nElements = ARRAYSIZE(d3dInputLayout);
	//파일 “Effect.fx”에서 정점-쉐이더의 시작 함수가 "VS"인 정점-쉐이더를 생성한다. 
	CreateVertexShaderFromFile(pd3dDevice, L"fx/Effect.fx", "VS", "vs_4_0", &m_pd3dVertexShader, d3dInputLayout, nElements, &m_pd3dVertexLayout);
	//파일 “Effect.fx”에서 픽셀-쉐이더의 시작 함수가 "PS"인 픽셀-쉐이더를 생성한다. 
	CreatePixelShaderFromFile(pd3dDevice, L"fx/Effect.fx", "PS", "ps_4_0", &m_pd3dPixelShader);



	// Create a sampler state
	D3D11_SAMPLER_DESC SamDesc;
	SamDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	SamDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	SamDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	SamDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	SamDesc.MipLODBias = 0.0f;
	SamDesc.MaxAnisotropy = 1;
	SamDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	SamDesc.BorderColor[0] = SamDesc.BorderColor[1] = SamDesc.BorderColor[2] = SamDesc.BorderColor[3] = 0;
	SamDesc.MinLOD = 0;
	SamDesc.MaxLOD = D3D11_FLOAT32_MAX;
	pd3dDevice->CreateSamplerState( &SamDesc, &m_pSamLinear );
	DXUT_SetDebugName( m_pSamLinear, "Primary" );



	//렌더링할 객체의 개수가 nObjects이므로 객체에 대한 포인터들의 배열을 정의한다.
	m_nObjects = nObjects; 
	if (m_nObjects > 0) 
	{
		m_ppObjects = new CGameObject*[m_nObjects]; 
		for (int i = 0; i < m_nObjects; i++) m_ppObjects[i] = NULL;
	}

	CreateShaderVariables(pd3dDevice);

}


void CObjectShader::Render(ID3D11DeviceContext *pd3dDeviceContext)
{
	CShader::Render(pd3dDeviceContext);

	//for (int j = 0; j < m_nObjects; j++)
	//{
	//	if (m_ppObjects[j]) 
	//	{
	//		if(m_ppObjects[j]->GetVisible() != TRUE) continue;
	//		UpdateShaderVariables(pd3dDeviceContext, &m_ppObjects[j]->m_d3dxmtxWorld);
	//		m_ppObjects[j]->Render(pd3dDeviceContext);
	//	}
	//}
}


void CObjectShader::CreateShaderVariables(ID3D11Device *pd3dDevice)
{
	CShader::CreateShaderVariables(pd3dDevice);

	//월드 변환 행렬을 위한 상수 버퍼를 생성한다.
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VS_CB_WORLD_MATRIX);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	pd3dDevice->CreateBuffer(&bd, NULL, &m_pd3dcbWorldMatrix);
} 

void CObjectShader::ReleaseShaderVariables()
{
	//월드 변환 행렬을 위한 상수 버퍼를 반환한다.
	if (m_pd3dcbWorldMatrix) m_pd3dcbWorldMatrix->Release();
}

void CObjectShader::UpdateShaderVariables(ID3D11DeviceContext *pd3dDeviceContext, D3DXMATRIX *pd3dxmtxWorld)
{
	CShader::UpdateShaderVariables(pd3dDeviceContext);

	//월드 변환 행렬을 상수 버퍼에 복사한다.
	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	pd3dDeviceContext->Map(m_pd3dcbWorldMatrix, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	VS_CB_WORLD_MATRIX *pcbWorldMatrix = (VS_CB_WORLD_MATRIX *)d3dMappedResource.pData;
	D3DXMatrixTranspose(&pcbWorldMatrix->m_d3dxmtxWorld, pd3dxmtxWorld);
	pd3dDeviceContext->Unmap(m_pd3dcbWorldMatrix, 0);

	//상수 버퍼를 디바이스의 슬롯(VS_SLOT_WORLD_MATRIX)에 연결한다.
	pd3dDeviceContext->VSSetConstantBuffers(VS_SLOT_WORLD_MATRIX, 1, &m_pd3dcbWorldMatrix);
}




CParticleShader::CParticleShader()
{
	CShader::CShader();

	m_pd3dcbWorldMatrix = NULL;
}

CParticleShader::~CParticleShader()
{
	for (int j = 0; j < m_nObjects; j++) if (m_ppObjects[j]) m_ppObjects[j]->Release();
	if (m_ppObjects) delete [] m_ppObjects;       

	ReleaseShaderVariables();

}

void CParticleShader::AddObject(CGameObject *pObject) 
{ 
	//쉐이더에 객체를 연결(설정)하고 참조 카운터를 1만큼 증가시킨다. 
	if (m_ppObjects) m_ppObjects[m_nIndexToAdd++] = pObject; 
	if (pObject) pObject->AddRef();
}

void CParticleShader::CreateShader(ID3D11Device *pd3dDevice, int nObjects)
{
	/*IA 단계에 설정할 입력-레이아웃을 정의한다. 정점 버퍼의 한 원소가 CVertex 클래스의 멤버 변수(D3DXVECTOR3 즉, 실수 세 개)이므로 입력-레이아웃은 실수(32-비트) 3개로 구성되며 시멘틱이 “POSITION”이고 정점 데이터임을 표현한다.*/ 
	D3D11_INPUT_ELEMENT_DESC d3dInputLayout[] =
	{
		{ "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",    0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",  0, DXGI_FORMAT_R32G32B32A32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT nElements = ARRAYSIZE(d3dInputLayout);
	//파일 “Effect.fx”에서 정점-쉐이더의 시작 함수가 "VS"인 정점-쉐이더를 생성한다. 
	CreateVertexShaderFromFile(pd3dDevice, L"fx/particle.vs", "ParticleVertexShader", "vs_5_0", &m_pd3dVertexShader, d3dInputLayout, nElements, &m_pd3dVertexLayout);
	//파일 “Effect.fx”에서 픽셀-쉐이더의 시작 함수가 "PS"인 픽셀-쉐이더를 생성한다. 
	CreatePixelShaderFromFile(pd3dDevice, L"fx/particle.ps", "ParticlePixelShader", "ps_5_0", &m_pd3dPixelShader);



	// Create a sampler state
	D3D11_SAMPLER_DESC SamDesc;
	SamDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	SamDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	SamDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	SamDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	SamDesc.MipLODBias = 0.0f;
	SamDesc.MaxAnisotropy = 1;
	SamDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	SamDesc.BorderColor[0] = SamDesc.BorderColor[1] = SamDesc.BorderColor[2] = SamDesc.BorderColor[3] = 0;
	SamDesc.MinLOD = 0;
	SamDesc.MaxLOD = D3D11_FLOAT32_MAX;
	pd3dDevice->CreateSamplerState( &SamDesc, &m_pSamLinear );
	DXUT_SetDebugName( m_pSamLinear, "Primary" );



	//렌더링할 객체의 개수가 nObjects이므로 객체에 대한 포인터들의 배열을 정의한다.
	m_nObjects = nObjects; 
	if (m_nObjects > 0) 
	{
		m_ppObjects = new CGameObject*[m_nObjects]; 
		for (int i = 0; i < m_nObjects; i++) m_ppObjects[i] = NULL;
	}

	CreateShaderVariables(pd3dDevice);

}


void CParticleShader::Render(ID3D11DeviceContext *pd3dDeviceContext)
{
	CShader::Render(pd3dDeviceContext);
}


void CParticleShader::CreateShaderVariables(ID3D11Device *pd3dDevice)
{
	CShader::CreateShaderVariables(pd3dDevice);

	//월드 변환 행렬을 위한 상수 버퍼를 생성한다.
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VS_CB_WORLD_MATRIX);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	pd3dDevice->CreateBuffer(&bd, NULL, &m_pd3dcbWorldMatrix);
} 

void CParticleShader::ReleaseShaderVariables()
{
	//월드 변환 행렬을 위한 상수 버퍼를 반환한다.
	if (m_pd3dcbWorldMatrix) m_pd3dcbWorldMatrix->Release();
}

void CParticleShader::UpdateShaderVariables(ID3D11DeviceContext *pd3dDeviceContext, D3DXMATRIX *pd3dxmtxWorld)
{
	CShader::UpdateShaderVariables(pd3dDeviceContext);

	//월드 변환 행렬을 상수 버퍼에 복사한다.
	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	pd3dDeviceContext->Map(m_pd3dcbWorldMatrix, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	VS_CB_WORLD_MATRIX *pcbWorldMatrix = (VS_CB_WORLD_MATRIX *)d3dMappedResource.pData;
	D3DXMatrixTranspose(&pcbWorldMatrix->m_d3dxmtxWorld, pd3dxmtxWorld);
	pd3dDeviceContext->Unmap(m_pd3dcbWorldMatrix, 0);

	//상수 버퍼를 디바이스의 슬롯(VS_SLOT_WORLD_MATRIX)에 연결한다.
	pd3dDeviceContext->VSSetConstantBuffers(VS_SLOT_WORLD_MATRIX, 1, &m_pd3dcbWorldMatrix);
}


CAnimationShader::CAnimationShader()
{
	CShader::CShader();

	m_pd3dcbWorldMatrix = NULL;
}

CAnimationShader::~CAnimationShader()
{
	for (int j = 0; j < m_nObjects; j++) if (m_ppObjects[j]) m_ppObjects[j]->Release();
	if (m_ppObjects) delete [] m_ppObjects;       

	ReleaseShaderVariables();

}

void CAnimationShader::AddObject(CGameObject *pObject) 
{ 
	//쉐이더에 객체를 연결(설정)하고 참조 카운터를 1만큼 증가시킨다. 
	if (m_ppObjects) m_ppObjects[m_nIndexToAdd++] = pObject; 
	if (pObject) pObject->AddRef();
}

void CAnimationShader::CreateShader(ID3D11Device *pd3dDevice, int nObjects)
{
	/*IA 단계에 설정할 입력-레이아웃을 정의한다. 정점 버퍼의 한 원소가 CVertex 클래스의 멤버 변수(D3DXVECTOR3 즉, 실수 세 개)이므로 입력-레이아웃은 실수(32-비트) 3개로 구성되며 시멘틱이 “POSITION”이고 정점 데이터임을 표현한다.*/ 
	D3D11_INPUT_ELEMENT_DESC d3dInputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,    0, D3D11_INPUT_PER_VERTEX_DATA, 0 },   //pos 16  //weight 4 , bones 4 , normal 12 
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,        12, D3D11_INPUT_PER_VERTEX_DATA, 0 },		//texcoord 8, tanget 16 binormal 4
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,         24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,    32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,   44, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BONES", 0, DXGI_FORMAT_R8G8B8A8_UINT, 0,           56, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,        60, D3D11_INPUT_PER_VERTEX_DATA, 0 }, 
	};
	UINT nElements = ARRAYSIZE(d3dInputLayout);
	//파일 “Effect.fx”에서 정점-쉐이더의 시작 함수가 "VS"인 정점-쉐이더를 생성한다. 
	CreateVertexShaderFromFile(pd3dDevice, L"fx/Soldier.fx", "VSSkinnedmain", "vs_4_0", &m_pd3dVertexShader, d3dInputLayout, nElements, &m_pd3dVertexLayout);
	//파일 “Effect.fx”에서 픽셀-쉐이더의 시작 함수가 "PS"인 픽셀-쉐이더를 생성한다. 
	CreatePixelShaderFromFile(pd3dDevice, L"fx/Soldier.fx", "PSSkinnedmain", "ps_4_0_level_9_1", &m_pd3dPixelShader);



	// Create a sampler state
	D3D11_SAMPLER_DESC SamDesc;
	SamDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	SamDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	SamDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	SamDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	SamDesc.MipLODBias = 0.0f;
	SamDesc.MaxAnisotropy = 1;
	SamDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	SamDesc.BorderColor[0] = SamDesc.BorderColor[1] = SamDesc.BorderColor[2] = SamDesc.BorderColor[3] = 0;
	SamDesc.MinLOD = 0;
	SamDesc.MaxLOD = D3D11_FLOAT32_MAX;
	pd3dDevice->CreateSamplerState( &SamDesc, &m_pSamLinear );
	DXUT_SetDebugName( m_pSamLinear, "Primary" );



	//렌더링할 객체의 개수가 nObjects이므로 객체에 대한 포인터들의 배열을 정의한다.
	m_nObjects = nObjects; 
	if (m_nObjects > 0) 
	{
		m_ppObjects = new CGameObject*[m_nObjects]; 
		for (int i = 0; i < m_nObjects; i++) m_ppObjects[i] = NULL;
	}

	CreateShaderVariables(pd3dDevice);

}


void CAnimationShader::Render(ID3D11DeviceContext *pd3dDeviceContext)
{
	CShader::Render(pd3dDeviceContext);

	//for (int j = 0; j < m_nObjects; j++)
	//{
	//	if (m_ppObjects[j]) 
	//	{
	//		if(m_ppObjects[j]->GetVisible() != TRUE) continue;
	//		UpdateShaderVariables(pd3dDeviceContext, &m_ppObjects[j]->m_d3dxmtxWorld);
	//		m_ppObjects[j]->Render(pd3dDeviceContext);
	//	}
	//}
}


void CAnimationShader::CreateShaderVariables(ID3D11Device *pd3dDevice)
{
	CShader::CreateShaderVariables(pd3dDevice);

	//월드 변환 행렬을 위한 상수 버퍼를 생성한다.
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VS_CB_WORLD_MATRIX);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	pd3dDevice->CreateBuffer(&bd, NULL, &m_pd3dcbWorldMatrix);
} 

void CAnimationShader::ReleaseShaderVariables()
{
	//월드 변환 행렬을 위한 상수 버퍼를 반환한다.
	if (m_pd3dcbWorldMatrix) m_pd3dcbWorldMatrix->Release();
}

void CAnimationShader::UpdateShaderVariables(ID3D11DeviceContext *pd3dDeviceContext, D3DXMATRIX *pd3dxmtxWorld)
{
	CShader::UpdateShaderVariables(pd3dDeviceContext);

	//월드 변환 행렬을 상수 버퍼에 복사한다.
	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	pd3dDeviceContext->Map(m_pd3dcbWorldMatrix, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	VS_CB_WORLD_MATRIX *pcbWorldMatrix = (VS_CB_WORLD_MATRIX *)d3dMappedResource.pData;
	D3DXMatrixTranspose(&pcbWorldMatrix->m_d3dxmtxWorld, pd3dxmtxWorld);
	pd3dDeviceContext->Unmap(m_pd3dcbWorldMatrix, 0);

	//상수 버퍼를 디바이스의 슬롯(VS_SLOT_WORLD_MATRIX)에 연결한다.
	pd3dDeviceContext->VSSetConstantBuffers(VS_SLOT_WORLD_MATRIX, 1, &m_pd3dcbWorldMatrix);
}


CInstancingShader::CInstancingShader()
{
	CShader::CShader();

	m_pd3dcbWorldMatrix = NULL;

	m_pBush3Mesh = NULL;
	m_pRock2Mesh = NULL;
	m_pRock3Mesh = NULL;
	m_pMissileMesh = NULL;
	m_pBlueTowerMesh = NULL;
	m_pRedTowerMesh = NULL;
	m_pMinionMesh = NULL;
	m_pDestroyTowerMesh = NULL;

	m_pd3dcbBush3InstanceMatrices = NULL;
	m_pd3dcbRock2InstanceMatrices = NULL;
	m_pd3dcbRock3InstanceMatrices = NULL;
	m_pd3dcbMissileInstanceMatrices = NULL;
	m_pd3dcbBlueTowerInstanceMatrices = NULL;
	m_pd3dcbRedTowerInstanceMatrices = NULL;
	m_pd3dcbMinionInstanceMatrices = NULL;
	m_pd3dcbDestroyTowerInstanceMatrices = NULL;

	m_nBush3Objects = 0;
	m_nRock2Objects = 0;
	m_nRock3Objects = 0;
	m_nMissileObjects = 0;
	m_nBlueTowerObjects = 0;
	m_nRedTowerObjects = 0;
	m_nMinionObjects = 0;
	m_nDestroyTowerObjects = 0;

	m_pd3dcbInstanceColors = NULL;
	m_nColorBufferStride = 0;
	m_nColorBufferOffset = 0;

	m_nInstanceBufferStride = 0;
	m_nInstanceBufferOffset = 0;
}

CInstancingShader::~CInstancingShader()
{
}

void CInstancingShader::AnimateObjects(float fTimeElapsed)
{
}

void CInstancingShader::ReleaseObjects()
{
	//if (m_pBush3Mesh) m_pBush3Mesh->Release();
	//if (m_pRock2Mesh) m_pRock2Mesh->Release();
	//if (m_pRock2Mesh) m_pRock3Mesh->Release();
	//if (m_pMissileMesh) m_pMissileMesh->Release();
	//if (m_pBlueTowerMesh) m_pBlueTowerMesh->Release();
	//if (m_pRedTowerMesh) m_pRedTowerMesh->Release();
	//if (m_pMinionMesh) m_pMinionMesh->Release();
	//if (m_pDestroyTowerMesh) m_pDestroyTowerMesh->Release();

	if (m_pd3dcbWorldMatrix) m_pd3dcbWorldMatrix->Release();
	if (m_ppObjects)
	{
		for (int j = 0; j < m_nObjects; j++) delete m_ppObjects[j];
		delete [] m_ppObjects;
	}

	if (m_pd3dcbBush3InstanceMatrices) m_pd3dcbBush3InstanceMatrices->Release();
	if (m_pd3dcbRock2InstanceMatrices) m_pd3dcbRock2InstanceMatrices->Release();
	if (m_pd3dcbRock3InstanceMatrices) m_pd3dcbRock3InstanceMatrices->Release();
	if (m_pd3dcbMissileInstanceMatrices) m_pd3dcbMissileInstanceMatrices->Release();
	if (m_pd3dcbBlueTowerInstanceMatrices) m_pd3dcbBlueTowerInstanceMatrices->Release();
	if (m_pd3dcbRedTowerInstanceMatrices) m_pd3dcbRedTowerInstanceMatrices->Release();
	if (m_pd3dcbMinionInstanceMatrices) m_pd3dcbMinionInstanceMatrices->Release();
	if (m_pd3dcbDestroyTowerInstanceMatrices) m_pd3dcbDestroyTowerInstanceMatrices->Release();
	if (m_pd3dcbInstanceColors) m_pd3dcbInstanceColors->Release();
}


void CInstancingShader::BuildObjects(ID3D11Device *pd3dDevice)
{
	m_nInstanceBufferStride = sizeof(D3DXMATRIX);
	m_nInstanceBufferOffset = 0;

	m_pBush3Mesh = new GFBX::Mesh();
	GFBXMeshLoader::getInstance()->LoadFBXMesh(m_pBush3Mesh, L"obstacle/bush3.FBX", pd3dDevice);
	for(int j=0; j<m_pBush3Mesh->GetSubsetCount(); j++)
	{
		m_pBush3Mesh->GetSubset(j)->OnCreateDevice(pd3dDevice);
		m_pBush3Mesh->GetSubset(j)->LoadTexture(pd3dDevice, L"obstacle/bush3.tif");
		m_pBush3Mesh->GetSubset(j)->ResetCULLNONECreateRasterizerState(pd3dDevice);
	}

	m_pRock2Mesh = new GFBX::Mesh();
	GFBXMeshLoader::getInstance()->LoadFBXMesh(m_pRock2Mesh, L"obstacle/Rock2.FBX", pd3dDevice);
	for(int j=0; j<m_pRock2Mesh->GetSubsetCount(); j++)
	{
		m_pRock2Mesh->GetSubset(j)->OnCreateDevice(pd3dDevice);
		m_pRock2Mesh->GetSubset(j)->LoadTexture(pd3dDevice, L"obstacle/Rock2.tif");
	}

	m_pRock3Mesh = new GFBX::Mesh();
	GFBXMeshLoader::getInstance()->LoadFBXMesh(m_pRock3Mesh, L"obstacle/Rock3.FBX", pd3dDevice);
	for(int j=0; j<m_pRock3Mesh->GetSubsetCount(); j++)
	{
		m_pRock3Mesh->GetSubset(j)->OnCreateDevice(pd3dDevice);
		m_pRock3Mesh->GetSubset(j)->LoadTexture(pd3dDevice, L"obstacle/Rock3.tif");
	}

	m_pMissileMesh = new GFBX::Mesh();
	GFBXMeshLoader::getInstance()->LoadFBXMesh(m_pMissileMesh, L"missile/Missile.FBX", pd3dDevice);
	for(int j=0; j<m_pMissileMesh->GetSubsetCount(); j++)
	{
		m_pMissileMesh->GetSubset(j)->OnCreateDevice(pd3dDevice);
		m_pMissileMesh->GetSubset(j)->LoadTexture(pd3dDevice, L"missile/Missile.png");
	}

	m_pBlueTowerMesh = new GFBX::Mesh();
	GFBXMeshLoader::getInstance()->LoadFBXMesh(m_pBlueTowerMesh, L"tower/BlueHqTower.FBX", pd3dDevice);
	for(int j=0; j<m_pBlueTowerMesh->GetSubsetCount(); j++)
	{
		m_pBlueTowerMesh->GetSubset(j)->OnCreateDevice(pd3dDevice);
		m_pBlueTowerMesh->GetSubset(j)->LoadTexture(pd3dDevice, L"tower/BlueHqTower.png");
	}

	m_pRedTowerMesh = new GFBX::Mesh();
	GFBXMeshLoader::getInstance()->LoadFBXMesh(m_pRedTowerMesh, L"tower/RedHqTower.FBX", pd3dDevice);
	for(int j=0; j<m_pRedTowerMesh->GetSubsetCount(); j++)
	{
		m_pRedTowerMesh->GetSubset(j)->OnCreateDevice(pd3dDevice);
		m_pRedTowerMesh->GetSubset(j)->LoadTexture(pd3dDevice, L"tower/RedHqTower.png");
	}

	m_pMinionMesh = new GFBX::Mesh();
	GFBXMeshLoader::getInstance()->LoadFBXMesh(m_pMinionMesh, L"minion/Dragon7107.FBX", pd3dDevice);
	for(int j=0; j<m_pMinionMesh->GetSubsetCount(); j++)
	{
		m_pMinionMesh->GetSubset(j)->OnCreateDevice(pd3dDevice);
		m_pMinionMesh->GetSubset(j)->LoadTexture(pd3dDevice, L"minion/micro_dragon_col.tif");
	}

	m_pDestroyTowerMesh = new GFBX::Mesh();
	GFBXMeshLoader::getInstance()->LoadFBXMesh(m_pDestroyTowerMesh, L"tower/DestroyedTower.FBX", pd3dDevice);
	for(int j=0; j<m_pDestroyTowerMesh->GetSubsetCount(); j++)
	{
		m_pDestroyTowerMesh->GetSubset(j)->OnCreateDevice(pd3dDevice);
		m_pDestroyTowerMesh->GetSubset(j)->LoadTexture(pd3dDevice, L"tower/DestroyedTower.png");
	}
	/*m_pBush3Mesh = new CFBXMesh(pd3dDevice, L"obstacle/bush3.FBX");
	m_pBush3Mesh->LoadTexture(pd3dDevice, L"obstacle/bush3.tif");

	m_pRock2Mesh = new CFBXMesh(pd3dDevice, L"obstacle/Rock2.FBX");
	m_pRock2Mesh->LoadTexture(pd3dDevice, L"obstacle/Rock2.tif");

	m_pRock3Mesh = new CFBXMesh(pd3dDevice, L"obstacle/Rock3.FBX");
	m_pRock3Mesh->LoadTexture(pd3dDevice, L"obstacle/Rock3.tif");

	m_pMissileMesh = new CFBXMesh(pd3dDevice, L"missile/Missile.FBX");
	m_pMissileMesh->LoadTexture(pd3dDevice, L"missile/Missile.png");

	m_pBlueTowerMesh = new CFBXMesh(pd3dDevice, L"tower/BlueHqTower.FBX");
	m_pBlueTowerMesh->LoadTexture(pd3dDevice, L"tower/BlueHqTower.png");

	m_pRedTowerMesh = new CFBXMesh(pd3dDevice, L"tower/RedHqTower.FBX");
	m_pRedTowerMesh->LoadTexture(pd3dDevice, L"tower/RedHqTower.png");

	m_pMinionMesh = new CFBXMesh(pd3dDevice, L"minion/Dragon7107.FBX");
	m_pMinionMesh->LoadTexture(pd3dDevice, L"minion/micro_dragon_col.tif");

	m_pDestroyTowerMesh = new CFBXMesh(pd3dDevice, L"tower/DestroyedTower.FBX");
	m_pDestroyTowerMesh->LoadTexture(pd3dDevice, L"tower/DestroyedTower.png");*/

	int bush3x = 24, bush3z = 7, i = 0;  //위아래 100 픽셀
	int bush3x1 = 5, bush3z1 = 10; //좌우 100픽셀

	int Rock2x = 5, Rock2z = 5;

	int Rock3x = 6, Rock3z = 12;

	printf("CreateShader \n");
	m_nBush3Objects = MapEditorManager::sharedManager()->GetTreeSize();
	m_nRock2Objects = MapEditorManager::sharedManager()->GetRockSize();
	m_nRock3Objects = MapEditorManager::sharedManager()->GetRock2Size();
	m_nMissileObjects = MAX_MISSILE;
 	m_nBlueTowerObjects = MapEditorManager::sharedManager()->GetBlueTowerSize();
 	m_nRedTowerObjects = MapEditorManager::sharedManager()->GetRedTowerSize();

	TowerManager::sharedManager()->SetTowerSize(m_nBlueTowerObjects+m_nRedTowerObjects);
	
	m_nMinionObjects = MAX_MINION;
	m_nDestroyTowerObjects = MAX_DESTROY_TOWER;

	m_nObjects = m_nBush3Objects + m_nRock2Objects + m_nRock3Objects + m_nMissileObjects + 
		m_nBlueTowerObjects + m_nRedTowerObjects + m_nMinionObjects + m_nDestroyTowerObjects ;
	//인스턴싱을 할 객체들의 배열이다.
	m_ppObjects = new CGameObject*[m_nObjects]; 

	printf("CreateTreeObject \n");
	for(int x = 0; x < m_nBush3Objects; x++)
	{
		Information In = MapEditorManager::sharedManager()->GetTreeData(x);
		ObstacleManager::sharedManager()->CreateObstacle(D3DXVECTOR3(In.x, 0, In.z), m_pBush3Mesh, 25, 25, 25);
			//if(x == m_nBush3Objects/2) 
				//printf("CreateTreeObject in Half \n");
	}

	for(int j=0; j< m_nBush3Objects; j++)
	{
		m_ppObjects[i++] = ObstacleManager::sharedManager()->m_pObstacle[j];
		ObstacleManager::sharedManager()->m_pObstacle[j]->SetRotation(1, 3.5);
	}

	m_pd3dcbBush3InstanceMatrices = CreateInstanceBuffer(pd3dDevice, m_nBush3Objects, m_nInstanceBufferStride, NULL);
	for(int j=0; j<m_pBush3Mesh->GetSubsetCount(); j++)
		m_pBush3Mesh->GetSubset(j)->AssembleToVertexBuffer(1, &m_pd3dcbBush3InstanceMatrices, &m_nInstanceBufferStride, &m_nInstanceBufferOffset);

	printf("CreateRockObject \n");
	for(int x = 0; x < m_nRock2Objects; x++)
	{
		Information In = MapEditorManager::sharedManager()->GetRockData(x);
		ObstacleManager::sharedManager()->CreateObstacle(D3DXVECTOR3(In.x, 0, In.z), m_pRock2Mesh, 25, 25, 25);
		//if(x == m_nRock2Objects/2) 
			//printf("CreateObject in Half \n");
			
	}
	for(int j=m_nBush3Objects; j< m_nBush3Objects + m_nRock2Objects; j++)
		m_ppObjects[i++] = ObstacleManager::sharedManager()->m_pObstacle[j];

	m_pd3dcbRock2InstanceMatrices = CreateInstanceBuffer(pd3dDevice, m_nRock2Objects, m_nInstanceBufferStride, NULL);
	for(int j=0; j<m_pRock2Mesh->GetSubsetCount(); j++)
		m_pRock2Mesh->GetSubset(j)->AssembleToVertexBuffer(1, &m_pd3dcbRock2InstanceMatrices, &m_nInstanceBufferStride, &m_nInstanceBufferOffset);

	printf("CreateRock2Object \n");
	for(int x = 0; x < m_nRock3Objects; x++)
	{
		Information In = MapEditorManager::sharedManager()->GetRock2Data(x);
		ObstacleManager::sharedManager()->CreateObstacle(D3DXVECTOR3(In.x, 0, In.z), m_pRock3Mesh, 25, 25, 25);
			
	}
	for(int j= m_nBush3Objects + m_nRock2Objects; j< m_nBush3Objects + m_nRock2Objects + m_nRock3Objects; j++)
		m_ppObjects[i++] = ObstacleManager::sharedManager()->m_pObstacle[j];

	m_pd3dcbRock3InstanceMatrices = CreateInstanceBuffer(pd3dDevice, m_nRock3Objects, m_nInstanceBufferStride, NULL);
	for(int j=0; j<m_pRock3Mesh->GetSubsetCount(); j++)
		m_pRock3Mesh->GetSubset(j)->AssembleToVertexBuffer(1, &m_pd3dcbRock3InstanceMatrices, &m_nInstanceBufferStride, &m_nInstanceBufferOffset);

	printf("CreateMissileObject \n");
	MissileManager::sharedManager()->CreateMissile(D3DXVECTOR3(1200, 0, 0), m_pMissileMesh);
	for(int j=0; j<m_nMissileObjects; j++)
		m_ppObjects[i++] = MissileManager::sharedManager()->m_pMissile[j];

	m_pd3dcbMissileInstanceMatrices = CreateInstanceBuffer(pd3dDevice, m_nMissileObjects, m_nInstanceBufferStride, NULL);
	for(int j=0; j<m_pMissileMesh->GetSubsetCount(); j++)
		m_pMissileMesh->GetSubset(j)->AssembleToVertexBuffer(1, &m_pd3dcbMissileInstanceMatrices, &m_nInstanceBufferStride, &m_nInstanceBufferOffset);

	printf("CreateRedTowerObject \n");
	for(int x = 0; x < m_nRedTowerObjects; x++)
	{
		Information In = MapEditorManager::sharedManager()->GetRedTowerData(x);
		TowerManager::sharedManager()->CreateTower(D3DXVECTOR3(In.x, 0, In.z), m_pRedTowerMesh, 30, 30, 30, RED_TEAM);
	}
	printf("CreateBlueTowerObject \n");
	for(int x = 0; x < m_nBlueTowerObjects; x++)
	{
		Information In = MapEditorManager::sharedManager()->GetBlueTowerData(x);
		TowerManager::sharedManager()->CreateTower(D3DXVECTOR3(In.x, 0, In.z), m_pBlueTowerMesh, 30, 30, 30, BLUE_TEAM);
	}

// 	TowerManager::sharedManager()->CreateTower(D3DXVECTOR3(-400.f + 15.f , 0, 165.f), m_pBlueTowerMesh, 30, 30, 30, BLUE_TEAM);
// 	TowerManager::sharedManager()->CreateTower(D3DXVECTOR3(-50.f - 15.f, 0, 165.f), m_pBlueTowerMesh, 30, 30, 30, BLUE_TEAM);
// 	TowerManager::sharedManager()->CreateTower(D3DXVECTOR3(50.f + 15.f, 0, 165.f), m_pRedTowerMesh, 30, 30, 30, RED_TEAM);
// 	TowerManager::sharedManager()->CreateTower(D3DXVECTOR3(400.f - 15.f, 0, 165.f), m_pRedTowerMesh, 30, 30, 30, RED_TEAM);
// 	TowerManager::sharedManager()->CreateTower(D3DXVECTOR3(-400.f + 15.f, 0, -165.f), m_pBlueTowerMesh, 30, 30, 30, BLUE_TEAM);
// 	TowerManager::sharedManager()->CreateTower(D3DXVECTOR3(-50.f - 15.f, 0, -165.f), m_pBlueTowerMesh, 30, 30, 30, BLUE_TEAM);
// 	TowerManager::sharedManager()->CreateTower(D3DXVECTOR3(50.f + 15.f, 0, -165.f), m_pRedTowerMesh, 30, 30, 30, RED_TEAM);
// 	TowerManager::sharedManager()->CreateTower(D3DXVECTOR3(400.f - 15.f, 0, -165.f), m_pRedTowerMesh, 30, 30, 30, RED_TEAM);
// 	TowerManager::sharedManager()->CreateTower(D3DXVECTOR3(-275.f, 0, -10.f), m_pBlueTowerMesh, 30, 30, 30, BLUE_TEAM);
// 	TowerManager::sharedManager()->CreateTower(D3DXVECTOR3(275.f, 0, 10.f), m_pRedTowerMesh, 30, 30, 30, RED_TEAM);
	for(int j=0; j<m_nBlueTowerObjects+m_nRedTowerObjects; j++)
	{
		if(TowerManager::sharedManager()->m_pTower[j]->GetTeam() == BLUE_TEAM)
			m_ppObjects[i++] = TowerManager::sharedManager()->m_pTower[j];
	}
	for(int j=0; j<m_nRedTowerObjects+m_nBlueTowerObjects; j++)
	{
		if(TowerManager::sharedManager()->m_pTower[j]->GetTeam() == RED_TEAM)
			m_ppObjects[i++] = TowerManager::sharedManager()->m_pTower[j];
	}

	m_pd3dcbBlueTowerInstanceMatrices = CreateInstanceBuffer(pd3dDevice, m_nBlueTowerObjects, m_nInstanceBufferStride, NULL);
	for(int j=0; j<m_pBlueTowerMesh->GetSubsetCount(); j++)
		m_pBlueTowerMesh->GetSubset(j)->AssembleToVertexBuffer(1, &m_pd3dcbBlueTowerInstanceMatrices, &m_nInstanceBufferStride, &m_nInstanceBufferOffset);

	m_pd3dcbRedTowerInstanceMatrices = CreateInstanceBuffer(pd3dDevice, m_nRedTowerObjects, m_nInstanceBufferStride, NULL);
	for(int j=0; j<m_pRedTowerMesh->GetSubsetCount(); j++)
		m_pRedTowerMesh->GetSubset(j)->AssembleToVertexBuffer(1, &m_pd3dcbRedTowerInstanceMatrices, &m_nInstanceBufferStride, &m_nInstanceBufferOffset);


	//미니언 인스턴싱
 	MinionManager::sharedManager()->CreateMinion1(D3DXVECTOR3(-1200, 0, -1220), m_pMinionMesh, 7, 10 ,7);
 	
 	for(int j=0; j<MAX_MINION; j++)
 	{
 		m_ppObjects[i++] = MinionManager::sharedManager()->m_pMinion1[j];
 	}
 
	m_pd3dcbMinionInstanceMatrices = CreateInstanceBuffer(pd3dDevice, m_nMinionObjects, m_nInstanceBufferStride, NULL);
	for(int j=0; j<m_pMinionMesh->GetSubsetCount(); j++)
		m_pMinionMesh->GetSubset(j)->AssembleToVertexBuffer(1, &m_pd3dcbMinionInstanceMatrices, &m_nInstanceBufferStride, &m_nInstanceBufferOffset);


	//부서진 타워 인스턴싱
	ObstacleManager::sharedManager()->CreateDestroyTower(D3DXVECTOR3(0, 0,-2000), m_pDestroyTowerMesh, 30, 30, 30);

	for(int j=0; j<MAX_DESTROY_TOWER; j++)
		m_ppObjects[i++] = ObstacleManager::sharedManager()->m_pDestroyTower[j];
	
	m_pd3dcbDestroyTowerInstanceMatrices = CreateInstanceBuffer(pd3dDevice, m_nDestroyTowerObjects, m_nInstanceBufferStride, NULL);
	for(int j=0; j<m_pDestroyTowerMesh->GetSubsetCount(); j++)
		m_pDestroyTowerMesh->GetSubset(j)->AssembleToVertexBuffer(1, &m_pd3dcbDestroyTowerInstanceMatrices, &m_nInstanceBufferStride, &m_nInstanceBufferOffset);

}

void CInstancingShader::CreateShader(ID3D11Device *pd3dDevice, int nObjects)
{
	D3D11_INPUT_ELEMENT_DESC d3dInputLayout[] =
	{
		//{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		//{ "NORMAL",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		//{ "TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,    0, D3D11_INPUT_PER_VERTEX_DATA, 0 },   //pos 16  //weight 4 , bones 4 , normal 12 
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,        12, D3D11_INPUT_PER_VERTEX_DATA, 0 },		//texcoord 8, tanget 16 binormal 4
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,         24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,    32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,   44, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BONES", 0, DXGI_FORMAT_R8G8B8A8_UINT, 0,           56, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,        60, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "POSINSTANCE", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "POSINSTANCE", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "POSINSTANCE", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "POSINSTANCE", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1 }
	};
	UINT nElements = ARRAYSIZE(d3dInputLayout);
	CreateVertexShaderFromFile(pd3dDevice, L"fx/instance.fx", "VSInstancedDiffusedColor", "vs_4_0", &m_pd3dVertexShader, d3dInputLayout, nElements, &m_pd3dVertexLayout);
	CreatePixelShaderFromFile(pd3dDevice, L"fx/instance.fx", "PSInstancedDiffusedColor", "ps_4_0", &m_pd3dPixelShader);

	// Create a sampler state
	D3D11_SAMPLER_DESC SamDesc;
	SamDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	SamDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	SamDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	SamDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	SamDesc.MipLODBias = 0.0f;
	SamDesc.MaxAnisotropy = 1;
	SamDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	SamDesc.BorderColor[0] = SamDesc.BorderColor[1] = SamDesc.BorderColor[2] = SamDesc.BorderColor[3] = 0;
	SamDesc.MinLOD = 0;
	SamDesc.MaxLOD = D3D11_FLOAT32_MAX;
	pd3dDevice->CreateSamplerState( &SamDesc, &m_pSamLinear );
	DXUT_SetDebugName( m_pSamLinear, "Primary" );

}

void CInstancingShader::CreateShaderVariables(ID3D11Device *pd3dDevice)
{
	CShader::CreateShaderVariables(pd3dDevice);

	//월드 변환 행렬을 위한 상수 버퍼를 생성한다.
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VS_CB_WORLD_MATRIX);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	//pd3dDevice->CreateBuffer(&bd, NULL, &m_pd3dcbWorldMatrix);
}         

ID3D11Buffer *CInstancingShader::CreateInstanceBuffer(ID3D11Device *pd3dDevice, int nObjects, UINT nBufferStride, void *pBufferData)
{
	ID3D11Buffer *pd3dInstanceBuffer = NULL;
	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	/*버퍼의 초기화 데이터가 없으면 동적 버퍼로 생성한다. 즉, 나중에 매핑을 하여 내용을 채우거나 변경한다.*/
	d3dBufferDesc.Usage = (pBufferData) ? D3D11_USAGE_DEFAULT : D3D11_USAGE_DYNAMIC;
	d3dBufferDesc.ByteWidth = nBufferStride * nObjects;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = (pBufferData) ? 0 : D3D11_CPU_ACCESS_WRITE;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = pBufferData;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, (pBufferData) ? &d3dBufferData : NULL, &pd3dInstanceBuffer);
	return(pd3dInstanceBuffer);
}

void CInstancingShader::UpdateShaderVariables(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera)
{
	CShader::UpdateShaderVariables(pd3dDeviceContext);

	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	pd3dDeviceContext->Map(m_pd3dcbBush3InstanceMatrices, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	D3DXMATRIX *pcbWorldMatrix = (D3DXMATRIX *)d3dMappedResource.pData;
	//인스턴싱 객체들의 월드 변환 행렬을 정점 버퍼에 쓴다.
	for (int j = 0; j < m_nBush3Objects; j++) 
	{
		/*if(m_ppObjects[j])
		{
			if (m_ppObjects[j]->IsVisible(pCamera))
			{*/
				pcbWorldMatrix[j] = m_ppObjects[j]->m_d3dxmtxWorld;
			//}
		//}
	}
	pd3dDeviceContext->Unmap(m_pd3dcbBush3InstanceMatrices, 0);

	pd3dDeviceContext->Map(m_pd3dcbRock2InstanceMatrices, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	pcbWorldMatrix = (D3DXMATRIX *)d3dMappedResource.pData;
	//인스턴싱 객체들의 월드 변환 행렬을 정점 버퍼에 쓴다.
	for (int j = 0; j < m_nRock2Objects; j++)
	{
		/*if(m_ppObjects[m_nBush3Objects + j])
		{
			if (m_ppObjects[m_nBush3Objects + j]->IsVisible(pCamera))
			{*/
				pcbWorldMatrix[j] = m_ppObjects[m_nBush3Objects + j]->m_d3dxmtxWorld;
			//}
		//}
	}
	pd3dDeviceContext->Unmap(m_pd3dcbRock2InstanceMatrices, 0);

	pd3dDeviceContext->Map(m_pd3dcbRock3InstanceMatrices, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	pcbWorldMatrix = (D3DXMATRIX *)d3dMappedResource.pData;
	//인스턴싱 객체들의 월드 변환 행렬을 정점 버퍼에 쓴다.
	for (int j = 0; j < m_nRock3Objects; j++)
	{

		/*if(m_ppObjects[m_nBush3Objects + m_nRock2Objects + j])
		{
			if (m_ppObjects[m_nBush3Objects + m_nRock2Objects + j]->IsVisible(pCamera))
			{*/
				pcbWorldMatrix[j] = m_ppObjects[m_nBush3Objects + m_nRock2Objects + j]->m_d3dxmtxWorld;
			//}
		//}
	}
	pd3dDeviceContext->Unmap(m_pd3dcbRock3InstanceMatrices, 0);

	pd3dDeviceContext->Map(m_pd3dcbMissileInstanceMatrices, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	pcbWorldMatrix = (D3DXMATRIX *)d3dMappedResource.pData;
	//인스턴싱 객체들의 월드 변환 행렬을 정점 버퍼에 쓴다.
 	for (int j = 0; j < m_nMissileObjects; j++)
	{
		/*if(m_ppObjects[m_nBush3Objects + m_nRock2Objects + m_nRock3Objects + j])
		{
			if (m_ppObjects[m_nBush3Objects + m_nRock2Objects + m_nRock3Objects + j]->IsVisible(pCamera))
			{*/
 				pcbWorldMatrix[j] = m_ppObjects[m_nBush3Objects + m_nRock2Objects + m_nRock3Objects + j]->m_d3dxmtxWorld;
			//}
		//}
	}
 	pd3dDeviceContext->Unmap(m_pd3dcbMissileInstanceMatrices, 0);

	pd3dDeviceContext->Map(m_pd3dcbBlueTowerInstanceMatrices, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	pcbWorldMatrix = (D3DXMATRIX *)d3dMappedResource.pData;
	//인스턴싱 객체들의 월드 변환 행렬을 정점 버퍼에 쓴다.
	for (int j = 0; j < m_nBlueTowerObjects; j++)
	{
		/*if(m_ppObjects[m_nBush3Objects + m_nRock2Objects + m_nRock3Objects + m_nMissileObjects + j])
		{
			if (m_ppObjects[m_nBush3Objects + m_nRock2Objects + m_nRock3Objects + m_nMissileObjects + j]->IsVisible(pCamera))
			{*/
				pcbWorldMatrix[j] = m_ppObjects[m_nBush3Objects + m_nRock2Objects + m_nRock3Objects + m_nMissileObjects + j]->m_d3dxmtxWorld;
			//}
		//}
	}
	pd3dDeviceContext->Unmap(m_pd3dcbBlueTowerInstanceMatrices, 0);

	pd3dDeviceContext->Map(m_pd3dcbRedTowerInstanceMatrices, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	pcbWorldMatrix = (D3DXMATRIX *)d3dMappedResource.pData;
	//인스턴싱 객체들의 월드 변환 행렬을 정점 버퍼에 쓴다.
	for (int j = 0; j < m_nRedTowerObjects; j++)
	{
		/*if(m_ppObjects[m_nBush3Objects + m_nRock2Objects + m_nRock3Objects + m_nMissileObjects + m_nBlueTowerObjects + j])
		{
			if (m_ppObjects[m_nBush3Objects + m_nRock2Objects + m_nRock3Objects + m_nMissileObjects + m_nBlueTowerObjects + j]->IsVisible(pCamera))
			{*/
				pcbWorldMatrix[j] = m_ppObjects[m_nBush3Objects + m_nRock2Objects + m_nRock3Objects + m_nMissileObjects + m_nBlueTowerObjects + j]->m_d3dxmtxWorld;
			//}
		//}
	}
	pd3dDeviceContext->Unmap(m_pd3dcbRedTowerInstanceMatrices, 0);


  	pd3dDeviceContext->Map(m_pd3dcbMinionInstanceMatrices, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
  	pcbWorldMatrix = (D3DXMATRIX *)d3dMappedResource.pData;
  	//인스턴싱 객체들의 월드 변환 행렬을 정점 버퍼에 쓴다.
  	for (int j = 0; j < m_nMinionObjects; j++)
	{
		/*if(m_ppObjects[m_nBush3Objects + m_nRock2Objects + m_nRock3Objects + m_nMissileObjects + m_nBlueTowerObjects + m_nRedTowerObjects + j])
		{
			if (m_ppObjects[m_nBush3Objects + m_nRock2Objects + m_nRock3Objects + m_nMissileObjects + m_nBlueTowerObjects + m_nRedTowerObjects + j]->IsVisible(pCamera))
			{*/
  				pcbWorldMatrix[j] = m_ppObjects[m_nBush3Objects + m_nRock2Objects + m_nRock3Objects + m_nMissileObjects + m_nBlueTowerObjects + 
		m_nRedTowerObjects + j]->m_d3dxmtxWorld;
			//}
		//}
	}
  	pd3dDeviceContext->Unmap(m_pd3dcbMinionInstanceMatrices, 0);

	pd3dDeviceContext->Map(m_pd3dcbDestroyTowerInstanceMatrices, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	pcbWorldMatrix = (D3DXMATRIX *)d3dMappedResource.pData;
	//인스턴싱 객체들의 월드 변환 행렬을 정점 버퍼에 쓴다.
 	for (int j = 0; j < m_nDestroyTowerObjects; j++)
 		{
 			/*if(m_ppObjects[m_nBush3Objects + m_nRock2Objects + m_nRock3Objects + m_nMissileObjects + m_nBlueTowerObjects + m_nRedTowerObjects + m_nMinionObjects + j]) 
 			{
 				if (m_ppObjects[m_nBush3Objects + m_nRock2Objects + m_nRock3Objects + m_nMissileObjects + m_nBlueTowerObjects + m_nRedTowerObjects + m_nMinionObjects + j]->IsVisible(pCamera))
 				{*/
 					pcbWorldMatrix[j] = m_ppObjects[m_nBush3Objects + m_nRock2Objects + m_nRock3Objects + m_nMissileObjects + m_nBlueTowerObjects + 
 			m_nRedTowerObjects + m_nMinionObjects + j]->m_d3dxmtxWorld;
 				//}
 			//}
 		}
 		pd3dDeviceContext->Unmap(m_pd3dcbDestroyTowerInstanceMatrices, 0);


}

void CInstancingShader::Render(ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera)
{
	CShader::Render(pd3dDeviceContext);

	UpdateShaderVariables(pd3dDeviceContext, pCamera);

	//메쉬의 인스턴스들을 렌더링한다.
	if (m_pBush3Mesh) m_pBush3Mesh->RenderInstanced(pd3dDeviceContext, m_nBush3Objects, 0);
	if (m_pRock2Mesh) m_pRock2Mesh->RenderInstanced(pd3dDeviceContext, m_nRock2Objects, 0);
	if (m_pRock3Mesh) m_pRock3Mesh->RenderInstanced(pd3dDeviceContext, m_nRock3Objects, 0);
	if (m_pMissileMesh) m_pMissileMesh->RenderInstanced(pd3dDeviceContext, m_nMissileObjects, 0);
	if (m_pBlueTowerMesh) m_pBlueTowerMesh->RenderInstanced(pd3dDeviceContext, m_nBlueTowerObjects, 0);
	if (m_pRedTowerMesh) m_pRedTowerMesh->RenderInstanced(pd3dDeviceContext, m_nRedTowerObjects, 0);
	if (m_pMinionMesh) m_pMinionMesh->RenderInstanced(pd3dDeviceContext, m_nMinionObjects, 0);
	if (m_pDestroyTowerMesh) m_pDestroyTowerMesh->RenderInstanced(pd3dDeviceContext, m_nDestroyTowerObjects, 0);
}

void CInstancingShader::AddObject(CGameObject *pObject) 
{ 
	//쉐이더에 객체를 연결(설정)하고 참조 카운터를 1만큼 증가시킨다. 
	if (m_ppObjects) m_ppObjects[m_nIndexToAdd++] = pObject; 
	if (pObject) pObject->AddRef();
}

