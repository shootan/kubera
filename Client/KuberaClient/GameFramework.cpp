#include "GameFramework.h"
#include "MinionManager.h"


CGameFramework::CGameFramework()
{
	m_pd3dDevice = NULL;
	m_pDXGISwapChain = NULL;
	m_pd3dRenderTargetView = NULL;
	m_pd3dDepthStencilView = NULL;
	m_pd3dDeviceContext = NULL;

	m_nWndClientWidth = FRAME_BUFFER_WIDTH;
	m_nWndClientHeight = FRAME_BUFFER_HEIGHT;

	m_pScene = NULL;
	_tcscpy_s(m_pszBuffer, _T("Kubera ("));

	m_CameraZoom = 60.f;
	m_CameraUpDown = 0.f;

	ZeroMemory(&HeroInfo, sizeof(PlayerPacket));
	HeroInfo.size = sizeof(PlayerPacket);

	m_pTxtHelper = NULL;
	m_pTxtHelper2 = NULL;
}

CGameFramework::~CGameFramework()
{
}

//다음 함수는 응용 프로그램이 실행되면 호출된다는 것에 유의하라. 
bool CGameFramework::OnCreate(HINSTANCE hInstance, HWND hMainWnd)
{
	m_hInstance = hInstance;
	m_hWnd = hMainWnd;

	//Direct3D 디바이스, 디바이스 컨텍스트, 스왑 체인 등을 생성하는 함수를 호출한다. 
	if (!CreateDirect3DDisplay()) return(false); 
	char IP[30];
	scanf("%s", IP);
	Net.InitClient(IP, 9000);

	while (Net.m_ID == 0)
	{
		Sleep(100);
	}
	//렌더링할 객체(게임 월드 객체)를 생성한다. 

	HeroManager::sharedManager()->SetID(Net.m_ID);
	HeroManager::sharedManager()->SetStartPos(D3DXVECTOR3(Net.m_Pos.x, Net.m_Pos.y, Net.m_Pos.z));
	HeroManager::sharedManager()->SetHP(Net.m_HP);
	if(Net.m_ID%2 == 0) //초기 시작 카메리위치 설정
	{
		m_CameraPosX = 500.f;
		m_CameraPosZ = -10.f;
	}
	else
	{
		m_CameraPosX = -500.f;
		m_CameraPosZ = -10.f;
	}
	BuildObjects();


	time = 0.0f;

	return(true);
}



bool CGameFramework::CreateRenderTargetDepthStencilView()
{
	HRESULT hResult = S_OK;

	ID3D11Texture2D *pd3dBackBuffer;
	if (FAILED(hResult = m_pDXGISwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID *)&pd3dBackBuffer))) return(false);
	if (FAILED(hResult = m_pd3dDevice->CreateRenderTargetView(pd3dBackBuffer, NULL, &m_pd3dRenderTargetView))) return(false);
	if (pd3dBackBuffer) pd3dBackBuffer->Release();

	// Create depth stencil texture
	D3D11_TEXTURE2D_DESC d3dDepthStencilBufferDesc;
	ZeroMemory(&d3dDepthStencilBufferDesc, sizeof(D3D11_TEXTURE2D_DESC));
	d3dDepthStencilBufferDesc.Width = m_nWndClientWidth;
	d3dDepthStencilBufferDesc.Height = m_nWndClientHeight;
	d3dDepthStencilBufferDesc.MipLevels = 1;
	d3dDepthStencilBufferDesc.ArraySize = 1;
	d3dDepthStencilBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dDepthStencilBufferDesc.SampleDesc.Count = 1;
	d3dDepthStencilBufferDesc.SampleDesc.Quality = 0;
	d3dDepthStencilBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dDepthStencilBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	d3dDepthStencilBufferDesc.CPUAccessFlags = 0;
	d3dDepthStencilBufferDesc.MiscFlags = 0;
	if (FAILED(hResult = m_pd3dDevice->CreateTexture2D(&d3dDepthStencilBufferDesc, NULL, &m_pd3dDepthStencilBuffer))) return(false);

	// Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC d3dDepthStencilViewDesc;
	ZeroMemory(&d3dDepthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	d3dDepthStencilViewDesc.Format = d3dDepthStencilBufferDesc.Format;
	d3dDepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	d3dDepthStencilViewDesc.Texture2D.MipSlice = 0;
	if (FAILED(hResult = m_pd3dDevice->CreateDepthStencilView(m_pd3dDepthStencilBuffer, &d3dDepthStencilViewDesc, &m_pd3dDepthStencilView))) return(false);

	m_pd3dDeviceContext->OMSetRenderTargets(1, &m_pd3dRenderTargetView, m_pd3dDepthStencilView);

	return(true);

}

void CGameFramework::SetViewport(ID3D11DeviceContext *pd3dDeviceContext, DWORD xTopLeft, DWORD yTopLeft, DWORD nWidth, DWORD nHeight, float fMinZ, float fMaxZ)
{
	D3D11_VIEWPORT m_d3dViewport;
	m_d3dViewport.TopLeftX = float(0);
	m_d3dViewport.TopLeftY = float(0);
	m_d3dViewport.Width = float(m_nWndClientWidth);
	m_d3dViewport.Height = float(m_nWndClientHeight);
	m_d3dViewport.MinDepth = 0;
	m_d3dViewport.MaxDepth = 1.0f;
	m_pd3dDeviceContext->RSSetViewports(1, &m_d3dViewport);
}

bool CGameFramework::CreateDirect3DDisplay()
{
	RECT rcClient;
	::GetClientRect(m_hWnd, &rcClient);
	m_nWndClientWidth = rcClient.right - rcClient.left;
	m_nWndClientHeight = rcClient.bottom - rcClient.top;

	UINT dwCreateDeviceFlags = 0;
#ifdef _DEBUG
	dwCreateDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	//디바이스를 생성하기 위하여 시도할 드라이버 유형의 순서를 나타낸다.
	D3D_DRIVER_TYPE d3dDriverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE
	};
	UINT nDriverTypes = sizeof(d3dDriverTypes) / sizeof(D3D10_DRIVER_TYPE);

	//디바이스를 생성하기 위하여 시도할 특성 레벨의 순서를 나타낸다.
	D3D_FEATURE_LEVEL d3dFeatureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0
	};
	UINT nFeatureLevels = sizeof(d3dFeatureLevels) / sizeof(D3D_FEATURE_LEVEL);

	//생성할 스왑 체인을 서술하는 구조체이다.
	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	::ZeroMemory(&dxgiSwapChainDesc, sizeof(dxgiSwapChainDesc));
	dxgiSwapChainDesc.BufferCount = 1;
	dxgiSwapChainDesc.BufferDesc.Width = m_nWndClientWidth;
	dxgiSwapChainDesc.BufferDesc.Height = m_nWndClientHeight;
	dxgiSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	dxgiSwapChainDesc.OutputWindow = m_hWnd;
	dxgiSwapChainDesc.SampleDesc.Count = 1;
	dxgiSwapChainDesc.SampleDesc.Quality = 0;
	dxgiSwapChainDesc.Windowed = TRUE;

	D3D_DRIVER_TYPE nd3dDriverType = D3D_DRIVER_TYPE_NULL;
	D3D_FEATURE_LEVEL nd3dFeatureLevel = D3D_FEATURE_LEVEL_11_0;

	HRESULT hResult = S_OK;
	//디바이스의 드라이버 유형과 특성 레벨을 지원하는 디바이스와 스왑 체인을 생성한다. 고수준의 디바이스 생성을 시도하고 실패하면 다음 수준의 디바이스를 생성한다.
	for (UINT i = 0; i < nDriverTypes; i++)
	{
		nd3dDriverType = d3dDriverTypes[i];
		if (SUCCEEDED(hResult = D3D11CreateDeviceAndSwapChain(NULL, nd3dDriverType, NULL, dwCreateDeviceFlags, d3dFeatureLevels, nFeatureLevels, D3D11_SDK_VERSION, &dxgiSwapChainDesc, &m_pDXGISwapChain, &m_pd3dDevice, &nd3dFeatureLevel, &m_pd3dDeviceContext))) break;
	}
	if (!m_pDXGISwapChain || !m_pd3dDevice || !m_pd3dDeviceContext) return(false);

	//렌더 타겟 뷰를 생성하는 함수를 호출한다.
	if(!CreateRenderTargetDepthStencilView()) return(false);


	m_DialogResourceManager.OnD3D11CreateDevice( m_pd3dDevice, m_pd3dDeviceContext );
	m_pTxtHelper = new CDXUTTextHelper( m_pd3dDevice, m_pd3dDeviceContext, &m_DialogResourceManager, 15 );
	m_pTxtHelper2 = new CDXUTTextHelper( m_pd3dDevice, m_pd3dDeviceContext, &m_DialogResourceManager, 15 );

	return(true);
}

void CGameFramework::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		break;
	case WM_MOUSEMOVE:
		break;
	default:
		break;
	}
}

void CGameFramework::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYUP:
		switch (wParam) 
		{
		case VK_ESCAPE:
			::PostQuitMessage(0);
			break;
		default:
			break;
		} 
		break;
	default:
		break;
	}
}

LRESULT CALLBACK CGameFramework::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	m_pScene->OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
	if(nMessageID != WM_MOUSEWHEEL)
		m_CameraUpDown = 0;

	switch (nMessageID)
	{
		/*윈도우의 크기가 변경될 때(현재는 “Alt+Enter“ 전체 화면 모드와 윈도우 모드로 전환될 때) 스왑 체인의 후면버퍼 크기를 조정하고 후면버퍼에 대한 렌더 타겟 뷰를 다시 생성한다. */
	case WM_SIZE:
		{
			m_nWndClientWidth = LOWORD(lParam);
			m_nWndClientHeight = HIWORD(lParam);

			m_pd3dDeviceContext->OMSetRenderTargets(0, NULL, NULL);

			if (m_pd3dRenderTargetView) m_pd3dRenderTargetView->Release();
			if (m_pd3dDepthStencilBuffer) m_pd3dDepthStencilBuffer->Release();
			if (m_pd3dDepthStencilView) m_pd3dDepthStencilView->Release();

			m_pDXGISwapChain->ResizeBuffers(2, m_nWndClientWidth, m_nWndClientHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
			SetViewport(m_pd3dDeviceContext, 0, 0, m_nWndClientWidth, m_nWndClientHeight, 0.0f, 1.0f);

			CreateRenderTargetDepthStencilView();

			m_DialogResourceManager.OnD3D11ResizedSwapChain( m_pd3dDevice, m_nWndClientWidth, m_nWndClientHeight );
			break;
		}
	case WM_LBUTTONDOWN:
		//MinionManager::sharedManager()->ChangeVisible();
		break;
	case WM_RBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
		OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
		break;
	case WM_KEYDOWN:
		switch (wParam) 
		{
		case VK_SPACE:
			if(HeroManager::sharedManager()->m_pHero != NULL)
			{
				m_CameraPosX = HeroManager::sharedManager()->m_pHero->GetPos().x;
				m_CameraPosZ = HeroManager::sharedManager()->m_pHero->GetPos().z-10;
			}
			
			break;
		} 

		break;
	case WM_KEYUP:
		OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
		break;
	case WM_MOUSEWHEEL:
		if (((short) HIWORD(wParam))/120 > 0 )
			m_CameraUpDown = 2;
		else if (((short) HIWORD(wParam))/120 < 0 )
			m_CameraUpDown = 1;
		break;
	}
	return(0);
}

//다음 함수는 응용 프로그램이 종료될 때 호출된다는 것에 유의하라. 
void CGameFramework::OnDestroy()
{
	//게임 객체를 소멸한다. 
	ReleaseObjects();

	//Direct3D와 관련된 객체를 소멸한다. 
	if (m_pd3dDeviceContext) m_pd3dDeviceContext->ClearState();
	if (m_pd3dRenderTargetView) m_pd3dRenderTargetView->Release();
	if (m_pd3dDepthStencilBuffer) m_pd3dDepthStencilBuffer->Release();
	if (m_pd3dDepthStencilView) m_pd3dDepthStencilView->Release();
	if (m_pDXGISwapChain) m_pDXGISwapChain->Release();
	if (m_pd3dDeviceContext) m_pd3dDeviceContext->Release();
	if (m_pd3dDevice) m_pd3dDevice->Release();

	m_DialogResourceManager.OnD3D11DestroyDevice();
	SAFE_DELETE( m_pTxtHelper );
	SAFE_DELETE( m_pTxtHelper2 );
}

void CGameFramework::BuildObjects()
{
	m_pScene = new CScene();
	m_pScene->m_Camera = &m_vCamera;

	//플레이어 객체를 생성한다.
	//m_nPlayers = 1;
	//m_ppPlayers = new CPlayer*[m_nPlayers];
	//m_ppPlayers[0] = new CPlayer();

	////카메라 객체를 생성하고 뷰-포트를 설정한다. 
	//CModelViewerCamera *pCamera = new CModelViewerCamera();
	//pCamera->CreateShaderVariables(m_pd3dDevice);
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VS_CB_VIEWPROJECTION_MATRIX);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_pd3dDevice->CreateBuffer(&bd, NULL, &m_pd3dcbViewProjection);

	SetViewport(m_pd3dDeviceContext, 0, 0, m_nWndClientWidth, m_nWndClientHeight, 0.0f, 1.0f);
	
	////투영 변환 행렬을 생성한다. 
	//pCamera->GenerateProjectionMatrix(1.0f, 500.0f, float(m_nWndClientWidth)/float(m_nWndClientHeight), 90.0f);
	m_vCamera.SetProjParams((float)D3DXToRadian(90.0f), float(m_nWndClientWidth)/float(m_nWndClientHeight), 1.0f, 500.0f);

	////카메라 변환 행렬을 생성한다. 
	
	//D3DXVECTOR3 d3dxvUp = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	//pCamera->GenerateViewMatrix(d3dxvEyePosition, d3dxvLookAt, d3dxvUp);
	////카메라 객체를 플레이어 객체에 설정한다. 
	//m_ppPlayers[0]->SetCamera(pCamera);
	//m_ppPlayers[0]->CreateShaderVariables(m_pd3dDevice);

	if (m_pScene) m_pScene->BuildObjects(m_pd3dDevice);

	m_DialogResourceManager.OnD3D11ResizedSwapChain( m_pd3dDevice, m_nWndClientWidth, m_nWndClientHeight );
}

void CGameFramework::ReleaseObjects()
{
	if (m_pScene) m_pScene->ReleaseObjects();
	if (m_pScene) delete m_pScene;

// 	if (m_ppPlayers) 
// 	{
// 		for (int j = 0; j < m_nPlayers; j++) delete m_ppPlayers[j];
// 		delete [] m_ppPlayers;
// 	}


}

void CGameFramework::ProcessInput()
{

}

void CGameFramework::AnimateObjects()
{
	if (m_pScene) m_pScene->AnimateObjects(m_GameTimer.GetTimeElapsed(), m_pd3dDevice);
}


void CGameFramework::FrameAdvance()
{    
	m_GameTimer.Tick(60);

	this->ExchangeInfo();
	m_pScene->OtherPlayerTargetSetting();
	
	ProcessInput();
	AnimateObjects();

	SetCameraPos();

	float fClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; 
	//렌더 타겟 뷰를 색상(RGB: 0.0f, 0.125f, 0.3f)으로 지운다. 
	m_pd3dDeviceContext->ClearRenderTargetView(m_pd3dRenderTargetView, fClearColor);
	if (m_pd3dDepthStencilView) m_pd3dDeviceContext->ClearDepthStencilView(m_pd3dDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	//m_pd3dRenderTargetView = DXUTGetD3D11RenderTargetView();
	//m_pd3dDeviceContext->ClearRenderTargetView( m_pd3dRenderTargetView, fClearColor );
	//m_pd3dDepthStencilView = DXUTGetD3D11DepthStencilView();
	//m_pd3dDeviceContext->ClearDepthStencilView( m_pd3dDepthStencilView, D3D11_CLEAR_DEPTH, 1.0, 0 );
	//후면버퍼를 전면버퍼로 프리젠트한다. 

	D3DXMATRIX m_d3dxmtxView = *m_vCamera.GetViewMatrix();         
	D3DXMATRIX m_d3dxmtxProjection = *m_vCamera.GetProjMatrix();   

	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	/*상수 버퍼의 메모리 주소를 가져와서 카메라 변환 행렬과 투영 변환 행렬을 복사한다. 쉐이더에서 행렬의 행과 열이 바뀌는 것에 주의하라.*/
	m_pd3dDeviceContext->Map(m_pd3dcbViewProjection, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	VS_CB_VIEWPROJECTION_MATRIX *pcbViewProjection = (VS_CB_VIEWPROJECTION_MATRIX *)d3dMappedResource.pData;
	D3DXMatrixTranspose(&pcbViewProjection->m_d3dxmtxView, &m_d3dxmtxView);
	D3DXMatrixTranspose(&pcbViewProjection->m_d3dxmtxProjection, &m_d3dxmtxProjection);
	m_pd3dDeviceContext->Unmap(m_pd3dcbViewProjection, 0);

	//상수 버퍼를 슬롯(VS_SLOT_VIEWPROJECTION_MATRIX)에 설정한다.
	m_pd3dDeviceContext->VSSetConstantBuffers( VS_SLOT_VIEWPROJECTION_MATRIX, 1, &m_pd3dcbViewProjection);

	//////////////////
	this->SendHeroData();
	m_pScene->Render(m_pd3dDeviceContext);

	RenderText();

	m_pDXGISwapChain->Present(0, 0);

	m_GameTimer.GetFrameRate(m_pszBuffer+8, 37);
	::SetWindowText(m_hWnd, m_pszBuffer);

}

void CGameFramework::SetCameraPos()
{
	D3DXVECTOR3 d3dxvEyePosition = D3DXVECTOR3(m_CameraPosX, m_CameraZoom, m_CameraPosZ);
	D3DXVECTOR3 d3dxvLookAt = D3DXVECTOR3(m_CameraPosX, 0.0f, m_CameraPosZ+17.0f);
	m_vCamera.SetViewParams( &d3dxvEyePosition, &d3dxvLookAt );

	if(m_CameraUpDown == 1)
		m_CameraZoom += 1 * 3.0f;
	else if(m_CameraUpDown == 2)
		m_CameraZoom -= 1 * 3.0f;

	if(m_CameraZoom <= 30)
		m_CameraZoom = 30;
	if(m_CameraZoom >= 100)
		m_CameraZoom = 100;



	if(m_CameraPosX <= -500)
	{
		m_CameraPosX = -500;
		if(m_pScene->GetMousePosX() > m_nWndClientWidth - 10) m_CameraPosX += 400 * m_GameTimer.GetTimeElapsed();

		if(m_CameraPosZ <= -350)
		{
			m_CameraPosZ = -350;
			if(m_pScene->GetMousePosY() < 10) m_CameraPosZ += 400 * m_GameTimer.GetTimeElapsed();
			return;
		}
		if(m_CameraPosZ >= 300)
		{
			m_CameraPosZ = 300;
			if(m_pScene->GetMousePosY() > m_nWndClientHeight - 10) m_CameraPosZ -= 400 * m_GameTimer.GetTimeElapsed();
			return;
		}

		if(m_pScene->GetMousePosY() < 10) m_CameraPosZ += 400 * m_GameTimer.GetTimeElapsed();
		if(m_pScene->GetMousePosY() > m_nWndClientHeight - 10) m_CameraPosZ -= 400 * m_GameTimer.GetTimeElapsed();
		

		return;
	}

	if(m_CameraPosX >= 500)
	{
		m_CameraPosX = 500;
		if(m_pScene->GetMousePosX() < 10) m_CameraPosX -= 400 * m_GameTimer.GetTimeElapsed();

		if(m_CameraPosZ <= -350)
		{
			m_CameraPosZ = -350;
			if(m_pScene->GetMousePosY() < 10) m_CameraPosZ += 400 * m_GameTimer.GetTimeElapsed();
			return;
		}
		if(m_CameraPosZ >= 300)
		{
			m_CameraPosZ = 300;
			if(m_pScene->GetMousePosY() > m_nWndClientHeight - 10) m_CameraPosZ -= 400 * m_GameTimer.GetTimeElapsed();
			return;
		}

		if(m_pScene->GetMousePosY() < 10) m_CameraPosZ += 400 * m_GameTimer.GetTimeElapsed();
		if(m_pScene->GetMousePosY() > m_nWndClientHeight - 10) m_CameraPosZ -= 400 * m_GameTimer.GetTimeElapsed();

		return;
	}

	if(m_CameraPosZ >= 300)
	{
		m_CameraPosZ = 300;
		if(m_pScene->GetMousePosY() > m_nWndClientHeight - 10) m_CameraPosZ -= 400 * m_GameTimer.GetTimeElapsed();


		if(m_CameraPosX <= -500)
		{
			m_CameraPosX = -500;
			if(m_pScene->GetMousePosX() > m_nWndClientWidth - 10) m_CameraPosX += 400 * m_GameTimer.GetTimeElapsed();
			return;
		}
		if(m_CameraPosX >= 500)
		{
			m_CameraPosX = 500;
			if(m_pScene->GetMousePosX() < 10) m_CameraPosX -= 400 * m_GameTimer.GetTimeElapsed();
			return;
		}

		if(m_pScene->GetMousePosX() > m_nWndClientWidth - 10) m_CameraPosX += 400 * m_GameTimer.GetTimeElapsed();
		if(m_pScene->GetMousePosX() < 10) m_CameraPosX -= 400 * m_GameTimer.GetTimeElapsed();

		return;
	}

	if(m_CameraPosZ <= -350)
	{
	//	m_CameraPosZ <= -350;
		if(m_pScene->GetMousePosY() < 10) m_CameraPosZ += 400 * m_GameTimer.GetTimeElapsed();

		if(m_CameraPosX <= -500)
		{
			m_CameraPosX = -500;
			if(m_pScene->GetMousePosX() > m_nWndClientWidth - 10) m_CameraPosX += 400 * m_GameTimer.GetTimeElapsed();
			return;
		}
		if(m_CameraPosX >= 500)
		{
			m_CameraPosX = 500;
			if(m_pScene->GetMousePosX() < 10) m_CameraPosX -= 400 * m_GameTimer.GetTimeElapsed();
			return;
		}

		if(m_pScene->GetMousePosX() > m_nWndClientWidth - 10) m_CameraPosX += 400 * m_GameTimer.GetTimeElapsed();
		if(m_pScene->GetMousePosX() < 10) m_CameraPosX -= 400 * m_GameTimer.GetTimeElapsed();

		return;
	}

	if(m_pScene->GetMousePosX() < 10) m_CameraPosX -= 400 * m_GameTimer.GetTimeElapsed();
	if(m_pScene->GetMousePosY() < 10) m_CameraPosZ += 400 * m_GameTimer.GetTimeElapsed();
	if(m_pScene->GetMousePosX() > m_nWndClientWidth - 10) m_CameraPosX += 400 * m_GameTimer.GetTimeElapsed();
	if(m_pScene->GetMousePosY() > m_nWndClientHeight - 10) m_CameraPosZ -= 400 * m_GameTimer.GetTimeElapsed();
}

void CGameFramework::ExchangeInfo()
{
 	if(Net.m_ClientCount != 0)
 	{
		m_pScene->SetMinionInfo(Net.MI);

		m_pScene->SetOtherClient(Net.PI, Net.m_ClientCount);
		m_pScene->UpdateOtherClient(Net.PI, Net.m_ClientCount);
	}	
}

void CGameFramework::SendHeroData()
{
	if(Net.m_ID != 0)
	{
		HeroInfo.PI.m_Pos = HeroManager::sharedManager()->m_pHero->GetPos();
		HeroInfo.PI.m_Rot = HeroManager::sharedManager()->m_pHero->GetRot();
		HeroInfo.PI.m_iState = HeroManager::sharedManager()->m_pHero->GetState();
		HeroInfo.PI.m_iTargetID = HeroManager::sharedManager()->m_pHero->GetTargetID();
		HeroInfo.PI.m_ID = Net.m_ID;
		HeroInfo.PI.m_HP = HeroManager::sharedManager()->m_pHero->GetHP();
		HeroInfo.PI.m_Damage = HeroManager::sharedManager()->m_pHero->GetDamage();
		HeroInfo.size = sizeof(PlayerPacket);

		Net.SendData(&HeroInfo);
	}
}


void CGameFramework::RenderText()
{
	m_pTxtHelper->Begin();
	m_pTxtHelper->SetInsertionPos(5, 30);
	m_pTxtHelper->SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ) );
	//m_pTxtHelper->DrawTextLine(L"Target : ");
	WCHAR str[255];
	for(int i=0; i<MAX_TOWER; i++)
	{
		if(HeroManager::sharedManager()->m_pHero->GetTarget() == TowerManager::sharedManager()->m_pTower[i])
		{
			int id = TowerManager::sharedManager()->m_pTower[i]->GetID();
			swprintf(str, 255, L"Target : Tower [%d], HP : %.0f",id, TowerManager::sharedManager()->m_pTower[i]->GetHP());
			m_pTxtHelper->DrawTextLine(str);
			break;
		}
	}
	for(int i=0; i<MAX_MINION; i++)
	{
		if(HeroManager::sharedManager()->m_pHero->GetTarget() == MinionManager::sharedManager()->m_pMinion1[i])
		{
			int id = MinionManager::sharedManager()->m_pMinion1[i]->GetID();
			swprintf(str, 255, L"Target : Minion [%d]",id);
			m_pTxtHelper->DrawTextLine(str);
			break;
		}
	}
	for(int i=0; i<MAX_OTHER_PLAYER; i++)
	{
		if(OtherPlayerManager::sharedManager()->m_pOtherPlayer[i]->GetVisible() != TRUE) continue;

		if(HeroManager::sharedManager()->m_pHero->GetTarget() == OtherPlayerManager::sharedManager()->m_pOtherPlayer[i])
		{
			int id = OtherPlayerManager::sharedManager()->m_pOtherPlayer[i]->GetID();
			swprintf(str, 255, L"Target : OtherPlayer [%d], HP : %.0f",id, OtherPlayerManager::sharedManager()->m_pOtherPlayer[i]->GetHP());
			m_pTxtHelper->DrawTextLine(str);
			break;
		}
	}

	if(HeroManager::sharedManager()->m_pHero->GetTarget() == HeroManager::sharedManager()->RedNexus)
	{
		swprintf(str, 255, L"Target : RedNexus, HP : %.0f", HeroManager::sharedManager()->RedNexus->GetHP());
		m_pTxtHelper->DrawTextLine(str);
	}

	if(HeroManager::sharedManager()->m_pHero->GetTarget() == HeroManager::sharedManager()->BlueNexus)
	{
		swprintf(str, 255, L"Target : RedNexus, HP : %.0f", HeroManager::sharedManager()->BlueNexus->GetHP());
		m_pTxtHelper->DrawTextLine(str);
	}

	m_pTxtHelper->End();

	m_pTxtHelper2->Begin();
	m_pTxtHelper2->SetInsertionPos(5, 10);
	m_pTxtHelper2->SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ) );
	//m_pTxtHelper->DrawTextLine(L"Target : ");
	str[255];
	swprintf(str, 255, L"HERO HP : [ %.0f ]", HeroManager::sharedManager()->m_pHero->GetHP());
	m_pTxtHelper->DrawTextLine(str);
	m_pTxtHelper2->End();
}