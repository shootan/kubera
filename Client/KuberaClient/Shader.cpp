#include "Shader.h"


CShader::CShader(void)
{
	m_pd3dVertexShader = NULL;
	m_pd3dPixelShader = NULL;
	m_pd3dVertexLayout = NULL;
	m_pSamLinear = NULL;

	m_ppObjects = NULL;       
	m_nObjects = 0;
	m_nIndexToAdd = 0;

	m_pCBClickTarget = NULL;
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
	D3D11_BUFFER_DESC Desc;
	Desc.Usage = D3D11_USAGE_DYNAMIC;
	Desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	Desc.MiscFlags = 0;

	Desc.ByteWidth = sizeof( CBClickTarget );
	pd3dDevice->CreateBuffer( &Desc, NULL, &m_pCBClickTarget );
	DXUT_SetDebugName( m_pCBClickTarget, "CBClickTarget" );
}

void CShader::ReleaseShaderVariables()
{
}

void CShader::UpdateShaderVariables(ID3D11DeviceContext *pd3dDeviceContext)
{
	D3DXVECTOR4 color = D3DXVECTOR4(0.0f,1.0f,0.0f,1.0f);
	m_CBClickTarget.m_vSelected = color;

	D3D11_MAPPED_SUBRESOURCE MappedResource;
	pd3dDeviceContext->Map( m_pCBClickTarget, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource ); 
	memcpy(MappedResource.pData, &m_pCBClickTarget, sizeof (m_pCBClickTarget));
	pd3dDeviceContext->Unmap( m_pCBClickTarget, 0 );

	pd3dDeviceContext->PSSetConstantBuffers(2, 1, &m_pCBClickTarget); 
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
	CreateVertexShaderFromFile(pd3dDevice, L"Effect.fx", "VS", "vs_4_0", &m_pd3dVertexShader, d3dInputLayout, nElements, &m_pd3dVertexLayout);
	//파일 “Effect.fx”에서 픽셀-쉐이더의 시작 함수가 "PS"인 픽셀-쉐이더를 생성한다. 
	CreatePixelShaderFromFile(pd3dDevice, L"Effect.fx", "PS", "ps_4_0", &m_pd3dPixelShader);



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


CInstancingShader::CInstancingShader()
{
	CShader::CShader();

	m_pd3dcbWorldMatrix = NULL;

	m_pBush3Mesh = NULL;
	m_pRock2Mesh = NULL;
	m_pRock3Mesh = NULL;
	m_pMissileMesh = NULL;
	m_pTowerMesh = NULL;
	m_pMinionMesh = NULL;

	m_pd3dcbBush3InstanceMatrices = NULL;
	m_pd3dcbRock2InstanceMatrices = NULL;
	m_pd3dcbRock3InstanceMatrices = NULL;
	m_pd3dcbMissileInstanceMatrices = NULL;
	m_pd3dcbTowerInstanceMatrices = NULL;
	m_pd3dcbMinionInstanceMatrices = NULL;

	m_nBush3Objects = 0;
	m_nRock2Objects = 0;
	m_nRock3Objects = 0;
	m_nMissileObjects = 0;
	m_nTowerObjects = 0;
	m_nMinionObjects = 0;
}

CInstancingShader::~CInstancingShader()
{
}

void CInstancingShader::AnimateObjects(float fTimeElapsed)
{
}

void CInstancingShader::ReleaseObjects()
{
	if (m_pBush3Mesh) m_pBush3Mesh->Release();
	if (m_pRock2Mesh) m_pRock2Mesh->Release();
	if (m_pRock2Mesh) m_pRock3Mesh->Release();
	if (m_pMissileMesh) m_pMissileMesh->Release();
	if (m_pTowerMesh) m_pTowerMesh->Release();
	if (m_pMinionMesh) m_pMinionMesh->Release();

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
	if (m_pd3dcbTowerInstanceMatrices) m_pd3dcbTowerInstanceMatrices->Release();
	if (m_pd3dcbMinionInstanceMatrices) m_pd3dcbMinionInstanceMatrices->Release();
}


void CInstancingShader::BuildObjects(ID3D11Device *pd3dDevice)
{
	//m_pMesh = new CCubeMesh(pd3dDevice, 10.0f, 13.0f, 10.0f);
	//m_pMesh->AddRef();

	m_pBush3Mesh = new CFBXMesh(pd3dDevice, L"obstacle/bush3.FBX");
	m_pBush3Mesh->LoadTexture(pd3dDevice, L"obstacle/bush3.tif");

	m_pRock2Mesh = new CFBXMesh(pd3dDevice, L"obstacle/Rock2.FBX");
	m_pRock2Mesh->LoadTexture(pd3dDevice, L"obstacle/Rock2.tif");

	m_pRock3Mesh = new CFBXMesh(pd3dDevice, L"obstacle/Rock3.FBX");
	m_pRock3Mesh->LoadTexture(pd3dDevice, L"obstacle/Rock3.tif");

	m_pMissileMesh = new CFBXMesh(pd3dDevice, L"obstacle/Rock3.FBX");
	m_pMissileMesh->LoadTexture(pd3dDevice, L"obstacle/Rock3.tif");

	m_pTowerMesh = new CFBXMesh(pd3dDevice, L"tower/Tower2_303030.FBX");
	m_pTowerMesh->LoadTexture(pd3dDevice, L"tower/tower.png");

	/*m_pMinionMesh = new CFBXMesh(pd3dDevice, L"minion/Dragon7107.FBX");
	m_pMinionMesh->LoadTexture(pd3dDevice, L"minion/micro_dragon_col.tif");*/

	int bush3x = 24, bush3z = 7, i = 0;  //위아래 100 픽셀
	int bush3x1 = 5, bush3z1 = 10; //좌우 100픽셀

	int Rock2x = 5, Rock2z = 5;

	int Rock3x = 6, Rock3z = 12;

	m_nBush3Objects = (bush3x*bush3z*4) + (bush3x1*bush3z1*4);
	m_nRock2Objects = (Rock2x*Rock2z*4);
	m_nRock3Objects = (Rock3x*Rock3z*2);
	m_nMissileObjects = MAX_MISSILE;
	m_nTowerObjects = MAX_TOWER;
	//m_nMinionObjects = MAX_MINION * 4;

	m_nObjects = m_nBush3Objects + m_nRock2Objects + m_nRock3Objects + m_nMissileObjects + m_nTowerObjects;
	//인스턴싱을 할 객체들의 배열이다.
	m_ppObjects = new CGameObject*[m_nObjects]; 

	for(int x = 0; x < bush3x; x++)
	{
		for(int z = 0; z < bush3z; z++)
		{
			ObstacleManager::sharedManager()->CreateObstacle(D3DXVECTOR3(x*25 + 12.5f, 0,(400.f-12.5f) - z*25), m_pBush3Mesh, 25, 25, 25);
			ObstacleManager::sharedManager()->CreateObstacle(D3DXVECTOR3(-(x*25) - 12.5f, 0,(400.f-12.5f) - z*25), m_pBush3Mesh, 25, 25, 25);
			ObstacleManager::sharedManager()->CreateObstacle(D3DXVECTOR3((x*25) + 12.5f, 0,(-225.f-12.5f) - z*25), m_pBush3Mesh, 25, 25, 25);
			ObstacleManager::sharedManager()->CreateObstacle(D3DXVECTOR3(-(x*25) - 12.5f, 0,(-225.f-12.5f) - z*25), m_pBush3Mesh, 25, 25, 25);
		}
	}

	for(int x=0; x<bush3x1; x++)
	{
		for(int z=0; z<bush3z1; z++)
		{
			ObstacleManager::sharedManager()->CreateObstacle(D3DXVECTOR3((-600.f + 12.5f) + x*25, 0,(300.f-12.5f) - z*25), m_pBush3Mesh, 25, 25, 25);
			ObstacleManager::sharedManager()->CreateObstacle(D3DXVECTOR3((-600.f + 12.5f) + x*25, 0,(-50.f-12.5f) - z*25), m_pBush3Mesh, 25, 25, 25);
			ObstacleManager::sharedManager()->CreateObstacle(D3DXVECTOR3((475.f + 12.5f) + x*25, 0,(300.f-12.5f) - z*25), m_pBush3Mesh, 25, 25, 25);
			ObstacleManager::sharedManager()->CreateObstacle(D3DXVECTOR3((475.f + 12.5f) + x*25, 0,(-50.f-12.5f) - z*25), m_pBush3Mesh, 25, 25, 25);
		}
	}
	for(int j=0; j< m_nBush3Objects; j++)
	{
		m_ppObjects[i++] = ObstacleManager::sharedManager()->m_pObstacle[j];
		ObstacleManager::sharedManager()->m_pObstacle[j]->SetRotation(1, 3.5);
	}

	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	d3dBufferDesc.ByteWidth = sizeof(D3DXMATRIX) * m_nBush3Objects;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, NULL, &m_pd3dcbBush3InstanceMatrices);

	m_pBush3Mesh->AppendVertexBuffer(m_pd3dcbBush3InstanceMatrices, sizeof(D3DXMATRIX), 0);


	for(int x = 0; x < Rock2x; x++)
	{
		for(int z = 0; z < Rock2z; z++)
		{
			ObstacleManager::sharedManager()->CreateObstacle(D3DXVECTOR3((-400.f + 12.5f) + x*25, 0,(150.f-12.5f) - z*25), m_pRock2Mesh, 25, 25, 25);
			ObstacleManager::sharedManager()->CreateObstacle(D3DXVECTOR3((-400.f + 12.5f) + x*25, 0,(-25.f-12.5f) - z*25), m_pRock2Mesh, 25, 25, 25);
			ObstacleManager::sharedManager()->CreateObstacle(D3DXVECTOR3((275.f + 12.5f) + x*25, 0,(150.f-12.5f) - z*25), m_pRock2Mesh, 25, 25, 25);
			ObstacleManager::sharedManager()->CreateObstacle(D3DXVECTOR3((275.f + 12.5f) + x*25, 0,(-25.f-12.5f) - z*25), m_pRock2Mesh, 25, 25, 25);
		}
	}
	for(int j=m_nBush3Objects; j< m_nBush3Objects + m_nRock2Objects; j++)
		m_ppObjects[i++] = ObstacleManager::sharedManager()->m_pObstacle[j];

	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	d3dBufferDesc.ByteWidth = sizeof(D3DXMATRIX) * m_nRock2Objects;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, NULL, &m_pd3dcbRock2InstanceMatrices);

	m_pRock2Mesh->AppendVertexBuffer(m_pd3dcbRock2InstanceMatrices, sizeof(D3DXMATRIX), 0);


	for(int x = 0; x < Rock3x; x++)
	{
		for(int z = 0; z < Rock3z; z++)
		{
			ObstacleManager::sharedManager()->CreateObstacle(D3DXVECTOR3((-200.f + 12.5f) + x*25, 0,(150.f-12.5f) - z*25), m_pRock3Mesh, 25, 25, 25);
			ObstacleManager::sharedManager()->CreateObstacle(D3DXVECTOR3((50.f + 12.5f) + x*25, 0,(150.f-12.5f) - z*25), m_pRock3Mesh, 25, 25, 25);
		}
	}
	for(int j= m_nBush3Objects + m_nRock2Objects; j< m_nBush3Objects + m_nRock2Objects + m_nRock3Objects; j++)
		m_ppObjects[i++] = ObstacleManager::sharedManager()->m_pObstacle[j];

	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	d3dBufferDesc.ByteWidth = sizeof(D3DXMATRIX) * m_nRock3Objects;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, NULL, &m_pd3dcbRock3InstanceMatrices);

	m_pRock3Mesh->AppendVertexBuffer(m_pd3dcbRock3InstanceMatrices, sizeof(D3DXMATRIX), 0);


	MissileManager::sharedManager()->CreateMissile(D3DXVECTOR3(1200, 0, 0), m_pMissileMesh);
	for(int j=0; j<m_nMissileObjects; j++)
		m_ppObjects[i++] = MissileManager::sharedManager()->m_pMissile[j];
	

	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	d3dBufferDesc.ByteWidth = sizeof(D3DXMATRIX) * m_nMissileObjects;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, NULL, &m_pd3dcbMissileInstanceMatrices);

	m_pMissileMesh->AppendVertexBuffer(m_pd3dcbMissileInstanceMatrices, sizeof(D3DXMATRIX), 0);


	TowerManager::sharedManager()->CreateTower(D3DXVECTOR3(-400.f + 15.f , 0, 165.f), m_pTowerMesh, 30, 30, 30);
	TowerManager::sharedManager()->CreateTower(D3DXVECTOR3(-50.f - 15.f, 0, 165.f), m_pTowerMesh, 30, 30, 30);
	TowerManager::sharedManager()->CreateTower(D3DXVECTOR3(50.f + 15.f, 0, 165.f), m_pTowerMesh, 30, 30, 30);
	TowerManager::sharedManager()->CreateTower(D3DXVECTOR3(400.f - 15.f, 0, 165.f), m_pTowerMesh, 30, 30, 30);
	TowerManager::sharedManager()->CreateTower(D3DXVECTOR3(-400.f + 15.f, 0, -165.f), m_pTowerMesh, 30, 30, 30);
	TowerManager::sharedManager()->CreateTower(D3DXVECTOR3(-50.f - 15.f, 0, -165.f), m_pTowerMesh, 30, 30, 30);
	TowerManager::sharedManager()->CreateTower(D3DXVECTOR3(50.f + 15.f, 0, -165.f), m_pTowerMesh, 30, 30, 30);
	TowerManager::sharedManager()->CreateTower(D3DXVECTOR3(400.f - 15.f, 0, -165.f), m_pTowerMesh, 30, 30, 30);
	TowerManager::sharedManager()->CreateTower(D3DXVECTOR3(-275.f, 0, -10.f), m_pTowerMesh, 30, 30, 30);
	TowerManager::sharedManager()->CreateTower(D3DXVECTOR3(275.f, 0, 10.f), m_pTowerMesh, 30, 30, 30);
	for(int j=0; j<m_nTowerObjects; j++)
		m_ppObjects[i++] = TowerManager::sharedManager()->m_pTower[j];


	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	d3dBufferDesc.ByteWidth = sizeof(D3DXMATRIX) * m_nTowerObjects;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, NULL, &m_pd3dcbTowerInstanceMatrices);

	m_pTowerMesh->AppendVertexBuffer(m_pd3dcbTowerInstanceMatrices, sizeof(D3DXMATRIX), 0);


	//미니언 인스턴싱 대신 일반으로 작업.
	/*MinionManager::sharedManager()->CreateMinion1(D3DXVECTOR3(485, 0, 20), m_pMinionMesh);
	MinionManager::sharedManager()->CreateMinion2(D3DXVECTOR3(-485, 0, 20), m_pMinionMesh);
	MinionManager::sharedManager()->CreateMinion3(D3DXVECTOR3(485, 0, -20), m_pMinionMesh);
	MinionManager::sharedManager()->CreateMinion4(D3DXVECTOR3(-485, 0, -20), m_pMinionMesh);
	for(int j=0; j<MAX_MINION; j++)
	{
		m_ppObjects[i++] = MinionManager::sharedManager()->m_pMinion1[j];
		m_ppObjects[i++] = MinionManager::sharedManager()->m_pMinion2[j];
		m_ppObjects[i++] = MinionManager::sharedManager()->m_pMinion3[j];
		m_ppObjects[i++] = MinionManager::sharedManager()->m_pMinion4[j];
	}

	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	d3dBufferDesc.ByteWidth = sizeof(D3DXMATRIX) * m_nMinionObjects;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, NULL, &m_pd3dcbMinionInstanceMatrices);

	m_pMinionMesh->AppendVertexBuffer(m_pd3dcbMinionInstanceMatrices, sizeof(D3DXMATRIX), 0);*/

}

void CInstancingShader::CreateShader(ID3D11Device *pd3dDevice, int nObjects)
{
	D3D11_INPUT_ELEMENT_DESC d3dInputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		//{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT , D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		//인스턴스 데이터는 위치 벡터(32비트 실수 3개)이고 인스턴스마다 정점 쉐이더에 전달된다.
		{ "POSINSTANCE", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "POSINSTANCE", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "POSINSTANCE", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "POSINSTANCE", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1 }
	};
	UINT nElements = ARRAYSIZE(d3dInputLayout);
	CreateVertexShaderFromFile(pd3dDevice, L"instance.fx", "VSInstancedDiffusedColor", "vs_4_0", &m_pd3dVertexShader, d3dInputLayout, nElements, &m_pd3dVertexLayout);
	CreatePixelShaderFromFile(pd3dDevice, L"instance.fx", "PSInstancedDiffusedColor", "ps_4_0", &m_pd3dPixelShader);

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

	CShader::CreateShaderVariables(pd3dDevice);
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

void CInstancingShader::UpdateShaderVariables(ID3D11DeviceContext *pd3dDeviceContext)
{
	CShader::UpdateShaderVariables(pd3dDeviceContext);

	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	pd3dDeviceContext->Map(m_pd3dcbBush3InstanceMatrices, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	D3DXMATRIX *pcbWorldMatrix = (D3DXMATRIX *)d3dMappedResource.pData;
	//인스턴싱 객체들의 월드 변환 행렬을 정점 버퍼에 쓴다.
	for (int j = 0; j < m_nBush3Objects; j++) pcbWorldMatrix[j] = m_ppObjects[j]->m_d3dxmtxWorld;
	pd3dDeviceContext->Unmap(m_pd3dcbBush3InstanceMatrices, 0);

	pd3dDeviceContext->Map(m_pd3dcbRock2InstanceMatrices, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	pcbWorldMatrix = (D3DXMATRIX *)d3dMappedResource.pData;
	//인스턴싱 객체들의 월드 변환 행렬을 정점 버퍼에 쓴다.
	for (int j = 0; j < m_nRock2Objects; j++) pcbWorldMatrix[j] = m_ppObjects[m_nBush3Objects + j]->m_d3dxmtxWorld;
	pd3dDeviceContext->Unmap(m_pd3dcbRock2InstanceMatrices, 0);

	pd3dDeviceContext->Map(m_pd3dcbRock3InstanceMatrices, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	pcbWorldMatrix = (D3DXMATRIX *)d3dMappedResource.pData;
	//인스턴싱 객체들의 월드 변환 행렬을 정점 버퍼에 쓴다.
	for (int j = 0; j < m_nRock3Objects; j++) pcbWorldMatrix[j] = m_ppObjects[m_nBush3Objects + m_nRock2Objects + j]->m_d3dxmtxWorld;
	pd3dDeviceContext->Unmap(m_pd3dcbRock3InstanceMatrices, 0);

	pd3dDeviceContext->Map(m_pd3dcbMissileInstanceMatrices, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	pcbWorldMatrix = (D3DXMATRIX *)d3dMappedResource.pData;
	//인스턴싱 객체들의 월드 변환 행렬을 정점 버퍼에 쓴다.
 	for (int j = 0; j < m_nMissileObjects; j++)
 		pcbWorldMatrix[j] = m_ppObjects[m_nBush3Objects + m_nRock2Objects + m_nRock3Objects + j]->m_d3dxmtxWorld;
 	pd3dDeviceContext->Unmap(m_pd3dcbMissileInstanceMatrices, 0);

	pd3dDeviceContext->Map(m_pd3dcbTowerInstanceMatrices, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	pcbWorldMatrix = (D3DXMATRIX *)d3dMappedResource.pData;
	//인스턴싱 객체들의 월드 변환 행렬을 정점 버퍼에 쓴다.
	for (int j = 0; j < m_nTowerObjects; j++)
		pcbWorldMatrix[j] = m_ppObjects[m_nBush3Objects + m_nRock2Objects + m_nRock3Objects + m_nMissileObjects + j]->m_d3dxmtxWorld;
	pd3dDeviceContext->Unmap(m_pd3dcbTowerInstanceMatrices, 0);


// 	pd3dDeviceContext->Map(m_pd3dcbMinionInstanceMatrices, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
// 	pcbWorldMatrix = (D3DXMATRIX *)d3dMappedResource.pData;
	////인스턴싱 객체들의 월드 변환 행렬을 정점 버퍼에 쓴다.
	//for (int j = 0; j < m_nMinionObjects; j++)
	//{
	//	if(m_ppObjects[m_nBush3Objects + m_nRock2Objects + m_nRock3Objects + m_nMissileObjects + m_nTowerObjects + j]->GetVisible()== FALSE) continue;
	//	pcbWorldMatrix[j] = m_ppObjects[m_nBush3Objects + m_nRock2Objects + m_nRock3Objects + m_nMissileObjects + m_nTowerObjects + j]->m_d3dxmtxWorld;
	//}
	//pd3dDeviceContext->Unmap(m_pd3dcbMinionInstanceMatrices, 0);
}

void CInstancingShader::Render(ID3D11DeviceContext *pd3dDeviceContext)
{
	CShader::Render(pd3dDeviceContext);

	UpdateShaderVariables(pd3dDeviceContext);

	//메쉬의 인스턴스들을 렌더링한다.
	if (m_pBush3Mesh) m_pBush3Mesh->RenderInstanced(pd3dDeviceContext, m_nBush3Objects, 0);
	if (m_pRock2Mesh) m_pRock2Mesh->RenderInstanced(pd3dDeviceContext, m_nRock2Objects, 0);
	if (m_pRock3Mesh) m_pRock3Mesh->RenderInstanced(pd3dDeviceContext, m_nRock3Objects, 0);
	if (m_pMissileMesh) m_pMissileMesh->RenderInstanced(pd3dDeviceContext, m_nMissileObjects, 0);
	if (m_pTowerMesh) m_pTowerMesh->RenderInstanced(pd3dDeviceContext, m_nTowerObjects, 0);
	/*if (m_pMinionMesh) m_pMinionMesh->RenderInstanced(pd3dDeviceContext, m_nMinionObjects, 0);*/
}

void CInstancingShader::AddObject(CGameObject *pObject) 
{ 
	//쉐이더에 객체를 연결(설정)하고 참조 카운터를 1만큼 증가시킨다. 
	if (m_ppObjects) m_ppObjects[m_nIndexToAdd++] = pObject; 
	if (pObject) pObject->AddRef();
}

