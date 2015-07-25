#include "GameFramework.h"
#include "MapEditorManager.h"
#include "LoadManager.h"

CGameFramework::CGameFramework()
{
	m_pd3dDevice = NULL;
	m_pDXGISwapChain = NULL;
	m_pd3dRenderTargetView = NULL;
	m_pd3dDepthStencilView = NULL;
	m_pd3dDeviceContext = NULL;
	m_depthStencilState = 0;
	m_depthDisabledStencilState = 0;

	m_nWndClientWidth = FRAME_BUFFER_WIDTH;
	m_nWndClientHeight = FRAME_BUFFER_HEIGHT;

	m_pScene = NULL;
	_tcscpy_s(m_pszBuffer, _T("Kubera ("));

	m_CameraZoom = 37.f;
	m_CameraUpDown = 0.f;

	ZeroMemory(&HeroInfo, sizeof(PlayerPacket));
	HeroInfo.size = sizeof(PlayerPacket);

	m_pTxtHelper = NULL;
	m_pTxtHelper2 = NULL;
	m_pCharacterInfo = NULL;

	m_SendTick = 0;

	m_pCamera = NULL;
	m_pCameraMinimap = NULL;
	m_pUICamera = NULL;
	m_pSelectCamera = NULL;
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
 	/*char IP[30];
  	printf("IP : ");
  	scanf("%s", IP);
  	Net.InitClient(IP, 9000);
 
     while (Net.m_InitFinish)
     {
     	Sleep(100);
     }*/
   	MapEditorManager::sharedManager()->LoadMapData();
 	printf("Server Connect \n");
	
	//렌더링할 객체(게임 월드 객체)를 생성한다. 

	HeroManager::sharedManager()->SetID(Net.m_ID);
	HeroManager::sharedManager()->SetTeam(1);
	HeroManager::sharedManager()->SetType(2);

	printf("SetData \n");
	
	m_CameraPosX = 390.f;
	m_CameraPosZ = -10.f;
	
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

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	// Set up the description of the stencil state.
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing.
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing.
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Create the depth stencil state.
	m_pd3dDevice->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
	m_pd3dDeviceContext->OMSetDepthStencilState(m_depthStencilState, 1);

	D3D11_DEPTH_STENCIL_DESC depthDisabledStencilDesc;
	ZeroMemory(&depthDisabledStencilDesc, sizeof(depthDisabledStencilDesc));
	depthDisabledStencilDesc.DepthEnable = false;
	depthDisabledStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthDisabledStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthDisabledStencilDesc.StencilEnable = true;
	depthDisabledStencilDesc.StencilReadMask = 0xFF;
	depthDisabledStencilDesc.StencilWriteMask = 0xFF;
	depthDisabledStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthDisabledStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthDisabledStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthDisabledStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDisabledStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	m_pd3dDevice->CreateDepthStencilState(&depthDisabledStencilDesc, &m_depthDisabledStencilState);



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
	m_pTxtHelper = new CDXUTTextHelper( m_pd3dDevice, m_pd3dDeviceContext, &m_DialogResourceManager, 20 );
	m_pTxtHelper2 = new CDXUTTextHelper( m_pd3dDevice, m_pd3dDeviceContext, &m_DialogResourceManager, 20 );
	m_pCharacterInfo = new CDXUTTextHelper(m_pd3dDevice, m_pd3dDeviceContext, &m_DialogResourceManager, 20);

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
	if(m_pScene) m_pScene->OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
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

			m_pCamera->SetViewport(m_pd3dDeviceContext, 0, 0, m_nWndClientWidth, m_nWndClientHeight, 0.9f, 1.0f);
			m_pUICamera->SetViewport(m_pd3dDeviceContext, 0, 0, m_nWndClientWidth, m_nWndClientHeight, 0.0f, 0.1f);
			m_pSelectCamera->SetViewport(m_pd3dDeviceContext, 0, 0, m_nWndClientWidth, m_nWndClientHeight, 0.9f, 1.0f);


			if(ST::sharedManager()->m_bStart == TRUE)
			{
				//UI크기 및 배치 재설정
				D3DXMatrixOrthoLH(&m_orthoMatrix, (float)m_nWndClientWidth, (float)m_nWndClientHeight, 1.0f, 500.0f);

				m_pCameraMinimap->SetViewport(m_pd3dDeviceContext,m_nWndClientWidth - m_pScene->GetMinimapUIWidth()  + m_pScene->GetMinimapUIWidth()/7, m_nWndClientHeight - m_pScene->GetMinimapUIHeight() + m_pScene->GetMinimapUIHeight()/13, m_pScene->GetMinimapUIWidth() - m_pScene->GetMinimapUIWidth()/7 , m_pScene->GetMinimapUIHeight(), 0.3f, 0.4f);
			}
			

			CreateRenderTargetDepthStencilView();

			m_DialogResourceManager.OnD3D11ResizedSwapChain( m_pd3dDevice, m_nWndClientWidth, m_nWndClientHeight );
			break;
		}
	case WM_LBUTTONDOWN:
		
		break;
	case WM_RBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
		//OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
		break;
	case WM_KEYDOWN:
		switch (wParam) 
		{
		case VK_SPACE:
			ST::sharedManager()->m_bSelected = TRUE;
			if(HeroManager::sharedManager()->m_pHero != NULL && ST::sharedManager()->m_bStart == TRUE)
			{
				m_CameraPosX = HeroManager::sharedManager()->m_pHero->GetPos().x;
				m_CameraPosZ = HeroManager::sharedManager()->m_pHero->GetPos().z-10;
			}			
			break;
		} 
	case WM_KEYUP:
		//OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
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

	if(m_depthStencilState)
	{
		m_depthStencilState->Release();
		m_depthStencilState = 0;
	}
	if(m_depthDisabledStencilState)
	{
		m_depthDisabledStencilState->Release();
		m_depthDisabledStencilState = 0;
	}

	m_DialogResourceManager.OnD3D11DestroyDevice();
	SAFE_DELETE( m_pTxtHelper );
	SAFE_DELETE( m_pTxtHelper2 );
	SAFE_DELETE( m_pCharacterInfo );
}

void CGameFramework::BuildObjects()
{
	CMaterialShader::CreateMaterialShaderVariables(m_pd3dDevice);

	m_pLoadScene = new LoadScene();
	m_pLoadScene->SetSize(m_nWndClientWidth, m_nWndClientHeight);

	printf("SetCamera");
	
	//카메라 생성
	m_pCamera = new CCamera();

	m_pCamera->CreateShaderVariables(m_pd3dDevice);
	m_pCamera->SetViewport(m_pd3dDeviceContext, 0, 0, m_nWndClientWidth, m_nWndClientHeight, 0.9f, 1.0f);
	////투영 변환 행렬을 생성한다. 
	m_pCamera->SetProjParams((float)D3DXToRadian(90.0f), float(m_nWndClientWidth)/float(m_nWndClientHeight), 1.0f, 500.0f);
	m_pCamera->SetMode(CAMERA);

	m_pSelectCamera = new CCamera();
	m_pSelectCamera->CreateShaderVariables(m_pd3dDevice);
	m_pSelectCamera->SetViewport(m_pd3dDeviceContext, 0, 0, m_nWndClientWidth, m_nWndClientHeight, 0.9f, 1.0f);
	m_pSelectCamera->SetProjParams((float)D3DXToRadian(90.0f), float(m_nWndClientWidth)/float(m_nWndClientHeight), 1.0f, 500.0f);
	m_pSelectCamera->SetViewParams( &D3DXVECTOR3(0, 30, -40), &D3DXVECTOR3(0, 0, 1) );
	m_pSelectCamera->m_CameraPos = D3DXVECTOR3(0, 30, -40);

	m_pCameraMinimap = new CCamera();
	m_pCameraMinimap->CreateShaderVariables(m_pd3dDevice);
	m_pCameraMinimap->SetViewport(m_pd3dDeviceContext,m_nWndClientWidth - 197.6573f  + 197.6573f/7, m_nWndClientHeight - 234.375f + 234.375f/13, 197.6573f - 197.6573f/7 , 234.375f, 0.3f, 0.4f);
	m_pCameraMinimap->SetProjParams((float)D3DXToRadian(90.0f), float(120)/float(80), 1.0f, 500.0f);
	D3DXVECTOR3 d3dxvEyePosition = D3DXVECTOR3(0, 500, 0);
	m_pCameraMinimap->m_CameraPos = d3dxvEyePosition;
	D3DXVECTOR3 d3dxvLookAt = D3DXVECTOR3(0, 0, 1);
	m_pCameraMinimap->SetViewParams( &d3dxvEyePosition, &d3dxvLookAt );
	m_pCameraMinimap->CalculateFrustumPlanes();
	m_pCameraMinimap->SetMode(MINIMAP_CAMERA);

	LoadManager::sharedManager()->LoadUIShader(m_pd3dDevice);
	//m_pUIShaders = LoadManager::sharedManager()->m_pUIShaders;
	m_pUICamera = new CCamera();
	m_pUICamera->CreateShaderVariables(m_pd3dDevice);
	m_pUICamera->SetViewport(m_pd3dDeviceContext, 0, 0, m_nWndClientWidth, m_nWndClientHeight, 0.0f, 0.1f);
	m_pUICamera->SetProjParams((float)D3DXToRadian(90.0f), float(m_nWndClientWidth)/float(m_nWndClientHeight), 1.0f, 500.0f);
	m_pUICamera->SetViewParams( &D3DXVECTOR3(0, 0, -10), &D3DXVECTOR3(0, 0, 1) );
	//UI를 위한 투영
	D3DXMatrixOrthoLH(&m_orthoMatrix, (float)m_nWndClientWidth, (float)m_nWndClientHeight, 1.0f, 500.0f);


	m_pLoadScene->BuildObject(m_pd3dDevice);
	m_DialogResourceManager.OnD3D11ResizedSwapChain( m_pd3dDevice, m_nWndClientWidth, m_nWndClientHeight );
}

void CGameFramework::ReleaseObjects()
{
	if (m_pScene) m_pScene->ReleaseObjects();
	if (m_pScene) delete m_pScene;
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
	m_SendTick += 1;

	if(!LoadManager::sharedManager()->LoadFinish)
	{
		float fClearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f }; 
		m_pd3dDeviceContext->ClearRenderTargetView(m_pd3dRenderTargetView, fClearColor);
		if (m_pd3dDepthStencilView) m_pd3dDeviceContext->ClearDepthStencilView(m_pd3dDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

		TurnZBufferOff();

		m_pUICamera->UpdateShaderVariables(m_pd3dDeviceContext, m_orthoMatrix);
		m_pd3dDeviceContext->RSSetViewports(1, &m_pUICamera->GetViewport());

		m_pLoadScene->LoadData(m_pd3dDevice, m_pd3dDeviceContext);

		TurnZBufferOn();

		if (m_pLoadScene->GetInfo() == 49)
		{
 			m_pSelectScene = new SelectScene();
 			m_pSelectScene->m_pCamera = m_pSelectCamera;
 			if (m_pSelectScene) m_pSelectScene->BuildObject(m_pd3dDevice);
 			LoadManager::sharedManager()->LoadFinish = TRUE;

			delete m_pLoadScene;
			printf("LoadFinish \n");
 			return;
		}
	}
 	
 	if(LoadManager::sharedManager()->LoadFinish && !ST::sharedManager()->m_bStart)
 	{
		float fClearColor[4] = { 0.0f, 0, 0, 1.0f }; 
		m_pd3dDeviceContext->ClearRenderTargetView(m_pd3dRenderTargetView, fClearColor);
		if (m_pd3dDepthStencilView) m_pd3dDeviceContext->ClearDepthStencilView(m_pd3dDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

		m_pSelectCamera->UpdateShaderVariables(m_pd3dDeviceContext);
		//m_pSelectCamera->FrameMove(m_GameTimer.GetTimeElapsed());
		m_pd3dDeviceContext->RSSetViewports(1, &m_pSelectCamera->GetViewport());
		m_pSelectScene->AnimateObject(m_GameTimer.GetTimeElapsed());
		m_pSelectScene->RenderObject(m_pd3dDeviceContext, m_GameTimer.GetTimeElapsed(), m_pSelectCamera);
 		if (ST::sharedManager()->m_bSelected == TRUE)
		{
 			m_pScene = new CScene();
 			m_pScene->m_Camera = m_pCamera;
 			if (m_pScene)
			{
				m_pScene->BuildObjects(m_pd3dDevice);
				m_pScene->CreateUI(m_pd3dDevice, m_nWndClientWidth, m_nWndClientHeight);
			}
 			ST::sharedManager()->m_bStart = TRUE;
			printf("SelectFinish \n");
 			return;
 		}
 	}

	if(ST::sharedManager()->m_bStart == TRUE)
	//if(LoadManager::sharedManager()->LoadFinish)
	{

		this->ExchangeInfo();
		m_pScene->OtherPlayerTargetSetting();

		ProcessInput();
		AnimateObjects();

		SetCameraPos();

		float fClearColor[4] = { 0, 0, 0, 1.0f }; 
		m_pd3dDeviceContext->ClearRenderTargetView(m_pd3dRenderTargetView, fClearColor);
		if (m_pd3dDepthStencilView) m_pd3dDeviceContext->ClearDepthStencilView(m_pd3dDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

		//후면버퍼를 전면버퍼로 프리젠트한다. 

		if(m_SendTick > 1)
		{
			this->SendHeroData();
			m_SendTick  = 0;
 		}

		m_pCamera->UpdateShaderVariables(m_pd3dDeviceContext);
		m_pCamera->FrameMove(m_GameTimer.GetTimeElapsed());
		m_pd3dDeviceContext->RSSetViewports(1, &m_pCamera->GetViewport());
		m_pScene->Render(m_pd3dDeviceContext, ::timeGetTime() * 0.001f, m_pCamera);

		RenderText();

		m_pCameraMinimap->UpdateShaderVariables(m_pd3dDeviceContext);
		//m_pCameraMinimap->FrameMove(m_GameTimer.GetTimeElapsed());
		m_pd3dDeviceContext->RSSetViewports(1, &m_pCameraMinimap->GetViewport());
		m_pScene->Render(m_pd3dDeviceContext, ::timeGetTime() * 0.001f, m_pCameraMinimap);

		//ui렌더
		TurnZBufferOff();
		m_pUICamera->UpdateShaderVariables(m_pd3dDeviceContext, m_orthoMatrix);
		m_pd3dDeviceContext->RSSetViewports(1, &m_pUICamera->GetViewport());
		m_pScene->RenderUI(m_pd3dDeviceContext, m_nWndClientWidth, m_nWndClientHeight);
		TurnZBufferOn();
	}

	m_pDXGISwapChain->Present(0, 0);

	m_GameTimer.GetFrameRate(m_pszBuffer+8, 37);
	::SetWindowText(m_hWnd, m_pszBuffer);

}

void CGameFramework::SetCameraPos()
{
	D3DXVECTOR3 d3dxvEyePosition = D3DXVECTOR3(m_CameraPosX, m_CameraZoom, m_CameraPosZ);
	m_pCamera->m_CameraPos = d3dxvEyePosition;
	D3DXVECTOR3 d3dxvLookAt = D3DXVECTOR3(m_CameraPosX, 0.0f, m_CameraPosZ+17.0f);
	m_pCamera->SetViewParams( &d3dxvEyePosition, &d3dxvLookAt );
	m_pCamera->CalculateFrustumPlanes();

	if(m_CameraUpDown == 1)
		m_CameraZoom += 1 * 3.0f;
	else if(m_CameraUpDown == 2)
		m_CameraZoom -= 1 * 3.0f;

	if(m_CameraZoom <= 1)
		m_CameraZoom = 1;
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

		m_pScene->SetOtherClient(Net.PI, Net.m_ClientCount);
		m_pScene->UpdateOtherClient(Net.PI, Net.m_ClientCount);
	}	
}

void CGameFramework::SendHeroData()
{
	if(Net.m_ID != 0)
	{
		HeroInfo.PI.m_Data.m_Pos = HeroManager::sharedManager()->m_pHero->GetPos();
		HeroInfo.PI.m_iState = HeroManager::sharedManager()->m_pHero->GetState();
		HeroInfo.PI.m_iTargetID = HeroManager::sharedManager()->m_pHero->GetTargetID();
		HeroInfo.PI.m_ID = Net.m_ID;
		HeroInfo.PI.m_Data.m_HP = HeroManager::sharedManager()->m_pHero->GetHP();
		HeroInfo.PI.m_Data.m_Damage = HeroManager::sharedManager()->m_pHero->GetDamage();
		HeroInfo.PI.m_Data.m_Rot = HeroManager::sharedManager()->m_pHero->GetRot();
		HeroInfo.PI.m_Type = HeroManager::sharedManager()->m_pHero->GetType();
		HeroInfo.size = sizeof(PlayerPacket);

		Net.SendData(&HeroInfo);
	}
}


void CGameFramework::RenderText()
{
	m_pTxtHelper->Begin();
	m_pTxtHelper->SetInsertionPos(30, 40);
	m_pTxtHelper->SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ) );
	//m_pTxtHelper->DrawTextLine(L"Target : ");
	WCHAR str[255];
	for(int i=0; i<MAX_TOWER; i++)
	{
		if(TowerManager::sharedManager()->m_pTower[i] == NULL) continue;
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
		if(MinionManager::sharedManager()->m_pMinion[i] == NULL ) continue;
		if(HeroManager::sharedManager()->m_pHero->GetTarget() == MinionManager::sharedManager()->m_pMinion[i])
		{
			int id = MinionManager::sharedManager()->m_pMinion[i]->GetID();
			swprintf(str, 255, L"Target : Minion [%d], HP : %.0f",id, MinionManager::sharedManager()->m_pMinion[i]->GetHP());
			m_pTxtHelper->DrawTextLine(str);
			break;
		}

	}
	

	if(HeroManager::sharedManager()->m_pHero->GetTarget() == OtherPlayerManager::sharedManager()->m_pOtherPlayer)
	{
		int id = OtherPlayerManager::sharedManager()->m_pOtherPlayer->GetID();
		swprintf(str, 255, L"Target : OtherPlayer [%d], HP : %.0f",id, OtherPlayerManager::sharedManager()->m_pOtherPlayer->GetHP());
		m_pTxtHelper->DrawTextLine(str);
	}
	

	if(HeroManager::sharedManager()->m_pHero->GetTarget() == HeroManager::sharedManager()->RedNexus)
	{
		swprintf(str, 255, L"Target : RedNexus, HP : %.0f", HeroManager::sharedManager()->RedNexus->GetHP());
		m_pTxtHelper->DrawTextLine(str);
	}

	if(HeroManager::sharedManager()->m_pHero->GetTarget() == HeroManager::sharedManager()->BlueNexus)
	{
		swprintf(str, 255, L"Target : BlueNexus, HP : %.0f", HeroManager::sharedManager()->BlueNexus->GetHP());
		m_pTxtHelper->DrawTextLine(str);
	}

	m_pTxtHelper->End();

	m_pTxtHelper2->Begin();
	m_pTxtHelper2->SetInsertionPos(5, 10);
	m_pTxtHelper2->SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ) );
	//m_pTxtHelper->DrawTextLine(L"Target : ");
	swprintf(str, 255, L"HERO HP : [ %.0f ]", HeroManager::sharedManager()->m_pHero->GetHP());
	m_pTxtHelper->DrawTextLine(str);
	m_pTxtHelper2->End();

	/*m_pCharacterInfo->Begin();
	m_pCharacterInfo->SetInsertionPos(m_UIInfoWidth/2 + m_UIInfoWidth/20 + m_SwordWidth, m_nWndClientHeight - m_UIInfoHeight + m_UIInfoHeight/3 - m_SwordHeight/2);
	m_pCharacterInfo->SetForegroundColor( D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f));
	swprintf(str, 255, L" : %.0f\n : %.0f\n : %.0f", HeroManager::sharedManager()->m_pHero->GetDamage(),HeroManager::sharedManager()->m_pHero->GetDefense(),HeroManager::sharedManager()->m_pHero->GetSpeed());
	m_pCharacterInfo->DrawTextLine(str);
	m_pCharacterInfo->End();*/
}

void CGameFramework::TurnZBufferOn()
{
	m_pd3dDeviceContext->OMSetDepthStencilState(m_depthStencilState, 1);
	return;
}


void CGameFramework::TurnZBufferOff()
{
	m_pd3dDeviceContext->OMSetDepthStencilState(m_depthDisabledStencilState, 1);
	return;
}

void CGameFramework::SetUIUpdate()
{
	//캐릭터 Hp바
	//m_HpbarGWidth *= HeroManager::sharedManager()->m_pHero->GetHP()/HeroManager::sharedManager()->m_pHero->GetLevel() * 100; 
	//m_pUI[8]->SetBitmapWH(m_HpbarGWidth, m_HpbarGHeight);

}