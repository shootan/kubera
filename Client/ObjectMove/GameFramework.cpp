#include "StdAfx.h"
#include "GameFramework.h"


CGameFramework::CGameFramework(void)
{
	m_pd3dDevice = NULL;
	m_pDXGISwapChain = NULL;
	m_pd3dRenderTargetView = NULL;
	m_pd3dDeviceContext = NULL;

	m_nwndClientWidth = FRAME_BUFFER_WIDTH;
	m_nwndClientHeight = FRAME_BUFFER_HEIGHT;

	m_pScene = NULL;
	_tcscpy_s (m_pszBuffer, _T("MoveMove   ("));

	m_nPlayers = 0;
	m_ppPlayers = NULL;

	m_pd3dDepthStencilBuffer = NULL;
	m_pd3dDepthStencilView = NULL;
}

CGameFramework::~CGameFramework(void)
{
}

// 다음 함수는 응용 프로그램이 실행되면 호출된다는 것에 유의할 것
bool CGameFramework::OnCreate(HINSTANCE hInstance, HWND hMainwnd)
{
	m_hInstance = hInstance;
	m_hwnd = hMainwnd;

	// Direct3D 디바이스, 디바이스 컨텍스트, 스왑체인 등을 생성하는 함수를 호출한다
	if (!CreateDirect3DDisplay())	return (false);

	// 렌더링 할 객체 (게임월드객체) 생성
	BuildObjects();

	return (true);
}

bool CGameFramework::CreateRenderTargetDepthStencilView()
{
	HRESULT hResult = S_OK;

	// 스왑 체인 후면버퍼에 대한 렌더 타겟 뷰를 생성
	ID3D11Texture2D *pd3dBackBuffer;
	if (FAILED (hResult = m_pDXGISwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*) &pd3dBackBuffer)))
		return (false);
	if (FAILED (hResult = m_pd3dDevice->CreateRenderTargetView (pd3dBackBuffer, NULL, &m_pd3dRenderTargetView)))
		return (false);
	if (pd3dBackBuffer)	pd3dBackBuffer->Release();

	// create depth stencil texture
	D3D11_TEXTURE2D_DESC d3dDepthStencilBufferDesc;
	ZeroMemory (&d3dDepthStencilBufferDesc, sizeof(D3D11_TEXTURE2D_DESC));
	d3dDepthStencilBufferDesc.Width = m_nwndClientWidth;
	d3dDepthStencilBufferDesc.Height = m_nwndClientHeight;
	d3dDepthStencilBufferDesc.MipLevels = 1;
	d3dDepthStencilBufferDesc.ArraySize = 1;
	d3dDepthStencilBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dDepthStencilBufferDesc.SampleDesc.Count = 1;
	d3dDepthStencilBufferDesc.SampleDesc.Quality = 0;
	d3dDepthStencilBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dDepthStencilBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	d3dDepthStencilBufferDesc.CPUAccessFlags = 0;
	d3dDepthStencilBufferDesc.MiscFlags = 0;
	if (FAILED (hResult = m_pd3dDevice->CreateTexture2D(&d3dDepthStencilBufferDesc, NULL, &m_pd3dDepthStencilBuffer)))
		return (false);

	// create the depth Stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC d3dDepthStencilViewDesc;
	ZeroMemory(&d3dDepthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	d3dDepthStencilViewDesc.Format = d3dDepthStencilBufferDesc.Format;
	d3dDepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	if (FAILED (hResult = m_pd3dDevice->CreateDepthStencilView(m_pd3dDepthStencilBuffer, &d3dDepthStencilViewDesc, &m_pd3dDepthStencilView)))
		return (false);


	// 렌더 타겟 뷰를 생성하고 출력-병합 단계에 연결한다.
	m_pd3dDeviceContext->OMSetRenderTargets(1, &m_pd3dRenderTargetView, m_pd3dDepthStencilView);

	return true;
}

bool CGameFramework::CreateDirect3DDisplay()
{
	RECT rcClient;
	::GetClientRect (m_hwnd, &rcClient);
	m_nwndClientWidth = rcClient.right - rcClient.left;
	m_nwndClientHeight = rcClient.bottom - rcClient.top;

	UINT dwCreateDeviceFlags = 0;

#ifdef  _DEBUG
	dwCreateDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	// 디바이스를 생성하기 위하여 시도할 드라이버 유형의 순서를 나타낸다.
	D3D_DRIVER_TYPE d3dDriverTypes[] = 
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE
	};
	UINT nDriverTypes = sizeof (d3dDriverTypes) / sizeof (D3D10_DRIVER_TYPE);

	// 디바이스를 생성하기 위하여 시도할 특성 레벨의 순서를 나타낸다.
	D3D_FEATURE_LEVEL d3dFeatureLevels[] = 
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0
	};
	UINT nFeatureLevels = sizeof(d3dFeatureLevels) / sizeof(D3D_FEATURE_LEVEL);

	// 생성할 스왑 체인을 서술하는 구조체이다.
	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	::ZeroMemory (&dxgiSwapChainDesc, sizeof (dxgiSwapChainDesc));
	dxgiSwapChainDesc.BufferCount = 1;
	dxgiSwapChainDesc.BufferDesc.Width = m_nwndClientWidth;
	dxgiSwapChainDesc.BufferDesc.Height = m_nwndClientHeight;
	dxgiSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	dxgiSwapChainDesc.OutputWindow = m_hwnd;
	dxgiSwapChainDesc.SampleDesc.Count = 1;
	dxgiSwapChainDesc.SampleDesc.Quality = 0;
	dxgiSwapChainDesc.Windowed = TRUE;

	D3D_DRIVER_TYPE nd3dDriverType = D3D_DRIVER_TYPE_NULL;
	D3D_FEATURE_LEVEL nd3dFeatureLevel = D3D_FEATURE_LEVEL_11_0;

	HRESULT hResult = S_OK;

	// 디바이스의 드라이버 유형과 특성 레벨을 지원하는 디바이스와 스왑체인을 생성한다.
	// 고수준의 디바이스 생서을 시도하고 실패하면 다음 수준의 디바이스를 생성한다.
	for (UINT i=0; i<nDriverTypes; i++)
	{
		nd3dDriverType = d3dDriverTypes[i];
		if (SUCCEEDED (hResult = D3D11CreateDeviceAndSwapChain(NULL, nd3dDriverType, NULL, dwCreateDeviceFlags, d3dFeatureLevels, nFeatureLevels,
															   D3D11_SDK_VERSION, &dxgiSwapChainDesc, &m_pDXGISwapChain, &m_pd3dDevice,
															   &nd3dFeatureLevel, &m_pd3dDeviceContext)))	
		break;
	}

	if (!m_pDXGISwapChain || !m_pd3dDevice || !m_pd3dDeviceContext)
		return false;

	// 렌더 타겟 뷰를 생성하는 함수를 호출한다.
	if (!CreateRenderTargetDepthStencilView()) return false;

	//SetViewPort();

	return true;
}

void CGameFramework::OnProcessingMouseMessage(HWND hwnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
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

void CGameFramework::OnProcessingKeyboardMessage(HWND hwnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYUP:
		switch (wParam)
		{
		case VK_F1:
		case VK_F2:
		case VK_F3:
			m_ppPlayers[0]->ChangeCamera(m_pd3dDevice, (wParam - VK_F1+1), m_GameTimer.GetTimeElapsed());
			break;
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

LRESULT CALLBACK CGameFramework::OnprocessingWindowMessage (HWND hwnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
		// 윈도우의 크기가 변경될 때 (현재는 알트+엔터 전체화면 모드와 윈도우 모드 전환할 때) 스왑 체인의 후면버퍼 크기를
		// 조정하고 후면버퍼에 대한 렌더 타겟 뷰를 다시 생성한다.
	case WM_SIZE:
		{
			m_nwndClientWidth = LOWORD (lParam);
			m_nwndClientHeight = HIWORD (lParam);

			m_pd3dDeviceContext->OMSetRenderTargets(0, NULL, NULL);

			if (m_pd3dRenderTargetView)	m_pd3dRenderTargetView->Release();
			if (m_pd3dDepthStencilBuffer) m_pd3dDepthStencilBuffer->Release();
			if (m_pd3dDepthStencilView) m_pd3dDepthStencilView->Release();

			m_pDXGISwapChain->ResizeBuffers(1, 0, 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0);

			CreateRenderTargetDepthStencilView();

			//SetViewPort();

			if (m_ppPlayers)
			{
				CCamera *pCamera = m_ppPlayers[0]->GetCamera();
				pCamera->SetViewPort(m_pd3dDeviceContext, 0, 0, m_nwndClientWidth, m_nwndClientHeight, 0.0f, 1.0f);

			}
			
			break;
		}

	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		// 마우스를 캡쳐를 하고 현재 마우스 위치를 가져온다
		SetCapture(hwnd);
		GetCursorPos (&m_ptOldCursorPos);
		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		ReleaseCapture();
		break;
	case WM_MOUSEMOVE:
		OnProcessingMouseMessage(hwnd, nMessageID, wParam, lParam);
		break;
	case WM_KEYDOWN:
	case WM_KEYUP:
		OnProcessingKeyboardMessage(hwnd, nMessageID, wParam, lParam);
		break;
	}

	return 0;
}

// 다음 함수는 응용 프로그램이 종료될 때 호출된다는 것에 유의하라
void CGameFramework::OnDestroy()
{
	// 게임 객체를 소멸한다.
	ReleaseObjects();

	// Direct3D와 관련된 객체를 소멸한다.
	if (m_pd3dDeviceContext) m_pd3dDeviceContext->ClearState();
	if (m_pd3dRenderTargetView) m_pd3dRenderTargetView->Release();
	if (m_pd3dDepthStencilBuffer) m_pd3dDepthStencilBuffer->Release();
	if (m_pd3dDepthStencilView) m_pd3dDepthStencilView->Release();
	if (m_pDXGISwapChain) m_pDXGISwapChain->Release();
	if (m_pd3dDeviceContext) m_pd3dDeviceContext->Release();
	if (m_pd3dDevice) m_pd3dDevice->Release();
}

void CGameFramework::BuildObjects()
{
	m_pScene = new CScene();

	// 플레이어 객체를 생성한다.
	m_nPlayers = 1;
	m_ppPlayers = new CPlayer*[m_nPlayers];
	
	CAirplanePlayer *pAirplanePlayer = new CAirplanePlayer(m_pd3dDevice);

	// 플레이어의 카메라를 스페이스쉽 카메라로 변경한다
	pAirplanePlayer->ChangeCamera(m_pd3dDevice, SPACESHIP_CAMERA, m_GameTimer.GetTimeElapsed());
	CCamera *pCamera = pAirplanePlayer->GetCamera();
	pCamera->SetViewPort(m_pd3dDeviceContext, 0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
	pCamera->GenerateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 60.0f);
	pCamera->GenerateViewMatrix();

	// 플레이어의 위치를 스페이스쉽 카메라로 변경한다
	pAirplanePlayer->SetPosition(D3DXVECTOR3(0.0f, 10.0f, -50.0f));

	m_ppPlayers[0] = pAirplanePlayer;

	if (m_pScene) m_pScene->BuildObjects(m_pd3dDevice);
}

void CGameFramework::ReleaseObjects()
{
	if (m_pScene) m_pScene->ReleaseObjects();
	if (m_pScene) delete m_pScene;

	if (m_ppPlayers)
	{
		for (int j=0; j<m_nPlayers; j++) delete m_ppPlayers[j];
		delete [] m_ppPlayers;
	}
}

void CGameFramework::ProcessInput()
{
	bool bProcessdByScene = false;
	if (m_pScene) bProcessdByScene = m_pScene->ProcessInput();
	if (!bProcessdByScene)
	{
		static UCHAR pKeyBuffer[256];
		DWORD dwDirection = 0;

		if (GetKeyboardState(pKeyBuffer))
		{
			if (pKeyBuffer[VK_UP] & 0xF0) dwDirection |= DIR_FORWARD;
			if (pKeyBuffer[VK_DOWN] & 0xF0) dwDirection |= DIR_BACKWARD;
			if (pKeyBuffer[VK_LEFT] & 0xF0) dwDirection |= DIR_LEFT;
			if (pKeyBuffer[VK_RIGHT] & 0xF0) dwDirection |= DIR_RIGHT;
			if (pKeyBuffer[VK_PRIOR] & 0xF0) dwDirection |= DIR_UP;
			if (pKeyBuffer[VK_NEXT] & 0xF0) dwDirection |= DIR_DOWN;
		}

		float cxDelta = 0.0f, cyDelta = 0.0f;
		POINT ptCursorPos;

		if (GetCapture() == m_hwnd)
		{
			SetCursor (NULL);
			GetCursorPos(&ptCursorPos);

			cxDelta = (float) (ptCursorPos.x - m_ptOldCursorPos.x) / 3.0f;
			cyDelta = (float) (ptCursorPos.y - m_ptOldCursorPos.y) / 3.0f;
			SetCursorPos(m_ptOldCursorPos.x, m_ptOldCursorPos.y);
		}

		if ((dwDirection != 0) || (cxDelta != 0.0f) || (cyDelta != 0.0f))
		{
			if (cxDelta || cyDelta)
			{
				if (pKeyBuffer[VK_RBUTTON] & 0xF0)
					m_ppPlayers[0]->Rotate(cyDelta, 0.0f, -cxDelta);
				else
					m_ppPlayers[0]->Rotate(cyDelta, cxDelta, 0.0f);
			}

			if (dwDirection) m_ppPlayers[0]->Move(dwDirection, 500.0f * m_GameTimer.GetTimeElapsed(), true);
		}
	}

	m_ppPlayers[0]->Update(m_GameTimer.GetTimeElapsed());
}

void CGameFramework::AnimateObjects()
{
	if (m_pScene) m_pScene->AnimateObjects(m_GameTimer.GetTimeElapsed());
}

void CGameFramework::FrameAdvance()
{
	// 타이머의 시간이 갱신되도록 하고 프레임 레이트를 계산한다
	m_GameTimer.Tick();

	// 사용자 입력을 처리하기 위한 ProcessInput() 함수를 호출한다.
	ProcessInput();
	
	// 게임 객체의 애니메이션을 처리하기 위한 AnimateObjects() 함수를 호출한다.
	AnimateObjects();

	float fclearcolor[4] = {0.0f, 0.125f, 0.3f, 1.0f};

	if (m_pd3dRenderTargetView)
	// 렌더 타겟 뷰를 색상 (RGB: 0.0f 0.125f, 0.3f)로 지운다
	m_pd3dDeviceContext->ClearRenderTargetView(m_pd3dRenderTargetView, fclearcolor);

	if (m_pd3dDepthStencilView)
		m_pd3dDeviceContext->ClearDepthStencilView(m_pd3dDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);


	CCamera *pCamera = NULL;

	for (int i = 0; i < m_nPlayers; i++)
	{
		if (m_ppPlayers[i]) m_ppPlayers[i]->UpdateShaderVariables(m_pd3dDeviceContext);
		pCamera = m_ppPlayers[i]->GetCamera();
		if (m_pScene) m_pScene->Render(m_pd3dDeviceContext, pCamera);
		//3인칭 카메라일 때 플레이어를 렌더링한다.
		for (int j = 0; j < m_nPlayers; j++) if (m_ppPlayers[j]) m_ppPlayers[j]->Render(m_pd3dDeviceContext);
	}

	// 후면 버퍼를 전면버퍼로 프리젠트한다
	m_pDXGISwapChain->Present(0, 0);

	m_GameTimer.GetFrameRate(m_pszBuffer+12, 37);
	::SetWindowText (m_hwnd, m_pszBuffer);
}

