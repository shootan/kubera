#include "GameFramework.h"
#include "MinionManager.h"
#include "MapEditorManager.h"

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

	m_CameraZoom = 37.f;
	m_CameraUpDown = 0.f;

	ZeroMemory(&HeroInfo, sizeof(PlayerPacket));
	HeroInfo.size = sizeof(PlayerPacket);

	m_pTxtHelper = NULL;
	m_pTxtHelper2 = NULL;

	m_SendTick = 0;

	m_pCamera = NULL;
	m_pCameraMinimap = NULL;
}

CGameFramework::~CGameFramework()
{
}

//���� �Լ��� ���� ���α׷��� ����Ǹ� ȣ��ȴٴ� �Ϳ� �����϶�. 
bool CGameFramework::OnCreate(HINSTANCE hInstance, HWND hMainWnd)
{
	m_hInstance = hInstance;
	m_hWnd = hMainWnd;

	//Direct3D ����̽�, ����̽� ���ؽ�Ʈ, ���� ü�� ���� �����ϴ� �Լ��� ȣ���Ѵ�. 
	if (!CreateDirect3DDisplay()) return(false); 
//  	char IP[30];
//   	printf("IP : ");
//   	scanf("%s", IP);
//   	Net.InitClient(IP, 9000);
//  
//      while (Net.m_InitFinish)
//      {
//      	Sleep(100);
//      }
   	
	MapEditorManager::sharedManager()->LoadMapData();
 	int herotype1 = 1;
 	printf("Server Connect \n");


	while(TRUE)
	{
		if(herotype1 == 0)
		{
			printf(" \n\nHero ����( 1. ������, 2. ���ڵ� ) : ");

			scanf("%d", &herotype1);
		}
		
		if(herotype1 == 1 || herotype1 == 2)
		{
			printf("Character Selected \n");
			break;
		}
	}
	

	//�������� ��ü(���� ���� ��ü)�� �����Ѵ�. 

	HeroManager::sharedManager()->SetID(Net.m_ID);
	HeroManager::sharedManager()->SetType(herotype1);
	HeroManager::sharedManager()->SetStartPos(D3DXVECTOR3(Net.m_Pos.x, Net.m_Pos.y, Net.m_Pos.z));
	//HeroManager::sharedManager()->SetHP(Net.m_HP);
	printf("SetData \n");
	if(Net.m_ID%2 == 0) //�ʱ� ���� ī�޸���ġ ����
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

	//����̽��� �����ϱ� ���Ͽ� �õ��� ����̹� ������ ������ ��Ÿ����.
	D3D_DRIVER_TYPE d3dDriverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE
	};
	UINT nDriverTypes = sizeof(d3dDriverTypes) / sizeof(D3D10_DRIVER_TYPE);

	//����̽��� �����ϱ� ���Ͽ� �õ��� Ư�� ������ ������ ��Ÿ����.
	D3D_FEATURE_LEVEL d3dFeatureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0
	};
	UINT nFeatureLevels = sizeof(d3dFeatureLevels) / sizeof(D3D_FEATURE_LEVEL);

	//������ ���� ü���� �����ϴ� ����ü�̴�.
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
	//����̽��� ����̹� ������ Ư�� ������ �����ϴ� ����̽��� ���� ü���� �����Ѵ�. �������� ����̽� ������ �õ��ϰ� �����ϸ� ���� ������ ����̽��� �����Ѵ�.
	for (UINT i = 0; i < nDriverTypes; i++)
	{
		nd3dDriverType = d3dDriverTypes[i];
		if (SUCCEEDED(hResult = D3D11CreateDeviceAndSwapChain(NULL, nd3dDriverType, NULL, dwCreateDeviceFlags, d3dFeatureLevels, nFeatureLevels, D3D11_SDK_VERSION, &dxgiSwapChainDesc, &m_pDXGISwapChain, &m_pd3dDevice, &nd3dFeatureLevel, &m_pd3dDeviceContext))) break;
	}
	if (!m_pDXGISwapChain || !m_pd3dDevice || !m_pd3dDeviceContext) return(false);

	//���� Ÿ�� �並 �����ϴ� �Լ��� ȣ���Ѵ�.
	if(!CreateRenderTargetDepthStencilView()) return(false);


	m_DialogResourceManager.OnD3D11CreateDevice( m_pd3dDevice, m_pd3dDeviceContext );
	m_pTxtHelper = new CDXUTTextHelper( m_pd3dDevice, m_pd3dDeviceContext, &m_DialogResourceManager, 20 );
	m_pTxtHelper2 = new CDXUTTextHelper( m_pd3dDevice, m_pd3dDeviceContext, &m_DialogResourceManager, 20 );

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
		/*�������� ũ�Ⱑ ����� ��(����� ��Alt+Enter�� ��ü ȭ�� ���� ������ ���� ��ȯ�� ��) ���� ü���� �ĸ���� ũ�⸦ �����ϰ� �ĸ���ۿ� ���� ���� Ÿ�� �並 �ٽ� �����Ѵ�. */
	case WM_SIZE:
		{
			m_nWndClientWidth = LOWORD(lParam);
			m_nWndClientHeight = HIWORD(lParam);

			m_pd3dDeviceContext->OMSetRenderTargets(0, NULL, NULL);

			if (m_pd3dRenderTargetView) m_pd3dRenderTargetView->Release();
			if (m_pd3dDepthStencilBuffer) m_pd3dDepthStencilBuffer->Release();
			if (m_pd3dDepthStencilView) m_pd3dDepthStencilView->Release();

			m_pDXGISwapChain->ResizeBuffers(2, m_nWndClientWidth, m_nWndClientHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
			//SetViewport(m_pd3dDeviceContext, 0, 0, m_nWndClientWidth, m_nWndClientHeight, 0.0f, 1.0f);
			m_pCamera->SetViewport(m_pd3dDeviceContext, 0, 0, m_nWndClientWidth, m_nWndClientHeight, 0.9f, 1.0f);
			m_pCameraMinimap->SetViewport(m_pd3dDeviceContext, m_nWndClientWidth - m_nWndClientWidth/5 - 10, m_nWndClientHeight - m_nWndClientHeight/5 - 10, m_nWndClientWidth/5, m_nWndClientHeight/5, 0.0f, 0.1f);
			

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
		case VK_UP:
			HeroManager::sharedManager()->m_pHero->SetState(DEATH);
			break;

		case VK_DOWN:
			HeroManager::sharedManager()->m_pHero->SetState(SKILL1);
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

//���� �Լ��� ���� ���α׷��� ����� �� ȣ��ȴٴ� �Ϳ� �����϶�. 
void CGameFramework::OnDestroy()
{
	//���� ��ü�� �Ҹ��Ѵ�. 
	ReleaseObjects();

	//Direct3D�� ���õ� ��ü�� �Ҹ��Ѵ�. 
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
	printf("SetCamera");
	//������
	m_pScene = new CScene();
	//ī�޶� ����
	m_pCamera = new CCamera();

	m_pCamera->CreateShaderVariables(m_pd3dDevice);
	m_pCamera->SetViewport(m_pd3dDeviceContext, 0, 0, m_nWndClientWidth, m_nWndClientHeight, 0.9f, 1.0f);

	//SetViewport(m_pd3dDeviceContext, 0, 0, m_nWndClientWidth, m_nWndClientHeight, 0.0f, 1.0f);
	
	////���� ��ȯ ����� �����Ѵ�. 
	//pCamera->GenerateProjectionMatrix(1.0f, 500.0f, float(m_nWndClientWidth)/float(m_nWndClientHeight), 90.0f);
	m_pCamera->SetProjParams((float)D3DXToRadian(90.0f), float(m_nWndClientWidth)/float(m_nWndClientHeight), 1.0f, 500.0f);

	m_pCameraMinimap = new CCamera();
	m_pCameraMinimap->CreateShaderVariables(m_pd3dDevice);
	m_pCameraMinimap->SetViewport(m_pd3dDeviceContext, m_nWndClientWidth - m_nWndClientWidth/5 - 10, m_nWndClientHeight - m_nWndClientHeight/5 - 10, m_nWndClientWidth/5, m_nWndClientHeight/5, 0.0f, 0.1f);
	m_pCameraMinimap->SetProjParams((float)D3DXToRadian(90.0f), float(120)/float(80), 1.0f, 500.0f);
	D3DXVECTOR3 d3dxvEyePosition = D3DXVECTOR3(0, 500, 0);
	D3DXVECTOR3 d3dxvLookAt = D3DXVECTOR3(0, 0, 17);
	m_pCameraMinimap->SetViewParams( &d3dxvEyePosition, &d3dxvLookAt );

	m_pScene->m_Camera = m_pCamera;
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
	m_SendTick += 1;


	this->ExchangeInfo();
	m_pScene->OtherPlayerTargetSetting();
	
	ProcessInput();
	AnimateObjects();

	SetCameraPos();

	float fClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f }; 
	//���� Ÿ�� �並 ����(RGB: 0.0f, 0.125f, 0.3f)���� �����. 
	m_pd3dDeviceContext->ClearRenderTargetView(m_pd3dRenderTargetView, fClearColor);
	if (m_pd3dDepthStencilView) m_pd3dDeviceContext->ClearDepthStencilView(m_pd3dDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	//m_pd3dRenderTargetView = DXUTGetD3D11RenderTargetView();
	//m_pd3dDeviceContext->ClearRenderTargetView( m_pd3dRenderTargetView, fClearColor );
	//m_pd3dDepthStencilView = DXUTGetD3D11DepthStencilView();
	//m_pd3dDeviceContext->ClearDepthStencilView( m_pd3dDepthStencilView, D3D11_CLEAR_DEPTH, 1.0, 0 );
	//�ĸ���۸� ������۷� ������Ʈ�Ѵ�. 

	

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
	m_pCameraMinimap->FrameMove(m_GameTimer.GetTimeElapsed());
	m_pd3dDeviceContext->RSSetViewports(1, &m_pCameraMinimap->GetViewport());
	m_pScene->Render(m_pd3dDeviceContext, ::timeGetTime() * 0.001f, m_pCameraMinimap);

	m_pDXGISwapChain->Present(0, 0);

	m_GameTimer.GetFrameRate(m_pszBuffer+8, 37);
	::SetWindowText(m_hWnd, m_pszBuffer);

}

void CGameFramework::SetCameraPos()
{
	D3DXVECTOR3 d3dxvEyePosition = D3DXVECTOR3(m_CameraPosX, m_CameraZoom, m_CameraPosZ);
	D3DXVECTOR3 d3dxvLookAt = D3DXVECTOR3(m_CameraPosX, 0.0f, m_CameraPosZ+17.0f);
	//m_pCamera->SetPosition(d3dxvEyePosition);
	//m_pCamera->SetLookAtPosition(d3dxvLookAt);
	//m_pCamera->GenerateViewMatrix();
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
		HeroInfo.PI.m_iState = HeroManager::sharedManager()->m_pHero->GetState();
		HeroInfo.PI.m_iTargetID = HeroManager::sharedManager()->m_pHero->GetTargetID();
		HeroInfo.PI.m_ID = Net.m_ID;
		HeroInfo.PI.m_HP = HeroManager::sharedManager()->m_pHero->GetHP();
		HeroInfo.PI.m_Damage = HeroManager::sharedManager()->m_pHero->GetDamage();
		HeroInfo.PI.m_Rot = HeroManager::sharedManager()->m_pHero->GetRot();
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
		if(TowerManager::sharedManager()->m_pTower[i]==NULL) continue;
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
}