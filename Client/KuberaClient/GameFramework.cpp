#include "GameFramework.h"


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

	m_CameraPosX = 500.f;
	m_CameraPosZ = -10.f;
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

	//�������� ��ü(���� ���� ��ü)�� �����Ѵ�. 
	BuildObjects();

	Net.InitClient("10.254.0.40", 9000);
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
	//����̽��� ����̹� ������ Ư�� ������ �����ϴ� ����̽��� ���� ü���� �����Ѵ�. ������� ����̽� ������ �õ��ϰ� �����ϸ� ���� ������ ����̽��� �����Ѵ�.
	for (UINT i = 0; i < nDriverTypes; i++)
	{
		nd3dDriverType = d3dDriverTypes[i];
		if (SUCCEEDED(hResult = D3D11CreateDeviceAndSwapChain(NULL, nd3dDriverType, NULL, dwCreateDeviceFlags, d3dFeatureLevels, nFeatureLevels, D3D11_SDK_VERSION, &dxgiSwapChainDesc, &m_pDXGISwapChain, &m_pd3dDevice, &nd3dFeatureLevel, &m_pd3dDeviceContext))) break;
	}
	if (!m_pDXGISwapChain || !m_pd3dDevice || !m_pd3dDeviceContext) return(false);

	//���� Ÿ�� �並 �����ϴ� �Լ��� ȣ���Ѵ�.
	if(!CreateRenderTargetDepthStencilView()) return(false);

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
			SetViewport(m_pd3dDeviceContext, 0, 0, m_nWndClientWidth, m_nWndClientHeight, 0.0f, 1.0f);

			CreateRenderTargetDepthStencilView();

			break;
		}
	case WM_LBUTTONDOWN:
		break;
	case WM_RBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
		OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
		break;
	case WM_KEYDOWN:
	case WM_KEYUP:
		OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
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
}

void CGameFramework::BuildObjects()
{
	m_pScene = new CScene();
	m_pScene->m_Camera = &m_vCamera;

	//�÷��̾� ��ü�� �����Ѵ�.
	//m_nPlayers = 1;
	//m_ppPlayers = new CPlayer*[m_nPlayers];
	//m_ppPlayers[0] = new CPlayer();

	////ī�޶� ��ü�� �����ϰ� ��-��Ʈ�� �����Ѵ�. 
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
	
	////���� ��ȯ ����� �����Ѵ�. 
	//pCamera->GenerateProjectionMatrix(1.0f, 500.0f, float(m_nWndClientWidth)/float(m_nWndClientHeight), 90.0f);
	m_vCamera.SetProjParams((float)D3DXToRadian(90.0f), float(m_nWndClientWidth)/float(m_nWndClientHeight), 1.0f, 500.0f);
	////ī�޶� ��ȯ ����� �����Ѵ�. 
	
	//D3DXVECTOR3 d3dxvUp = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	//pCamera->GenerateViewMatrix(d3dxvEyePosition, d3dxvLookAt, d3dxvUp);
	////ī�޶� ��ü�� �÷��̾� ��ü�� �����Ѵ�. 
	//m_ppPlayers[0]->SetCamera(pCamera);
	//m_ppPlayers[0]->CreateShaderVariables(m_pd3dDevice);

	if (m_pScene) m_pScene->BuildObjects(m_pd3dDevice);

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


	PlayerPacket* a = new PlayerPacket;
	ZeroMemory(a, sizeof(PlayerPacket));
	a->size= sizeof(PlayerPacket);
	a->PI.m_Pos = m_pScene->GetObject(0)->GetPos();
	a->PI.m_Scale = m_pScene->GetObject(0)->GetScale();
	a->PI.m_Rot = m_pScene->GetObject(0)->GetRot();

	Net.SendData(a);

	delete a;


	if(Net.PI.size != 0)
	{
		m_pScene->m_bJoinOtherPlayer = TRUE;

		if(m_pScene->GetObject(3) != NULL)
		{
			m_pScene->GetObject(3)->SetPos(Net.PI.PI.m_Pos);
			m_pScene->GetObject(3)->SetRot(Net.PI.PI.m_Rot);
			m_pScene->GetObject(3)->SetScale(Net.PI.PI.m_Scale);
		}
	}
	

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

	D3DXMATRIX m_d3dxmtxView = *m_vCamera.GetViewMatrix();         
	D3DXMATRIX m_d3dxmtxProjection = *m_vCamera.GetProjMatrix();   

	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	/*��� ������ �޸� �ּҸ� �����ͼ� ī�޶� ��ȯ ��İ� ���� ��ȯ ����� �����Ѵ�. ���̴����� ����� ��� ���� �ٲ�� �Ϳ� �����϶�.*/
	m_pd3dDeviceContext->Map(m_pd3dcbViewProjection, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	VS_CB_VIEWPROJECTION_MATRIX *pcbViewProjection = (VS_CB_VIEWPROJECTION_MATRIX *)d3dMappedResource.pData;
	D3DXMatrixTranspose(&pcbViewProjection->m_d3dxmtxView, &m_d3dxmtxView);
	D3DXMatrixTranspose(&pcbViewProjection->m_d3dxmtxProjection, &m_d3dxmtxProjection);
	m_pd3dDeviceContext->Unmap(m_pd3dcbViewProjection, 0);

	//��� ���۸� ����(VS_SLOT_VIEWPROJECTION_MATRIX)�� �����Ѵ�.
	m_pd3dDeviceContext->VSSetConstantBuffers( VS_SLOT_VIEWPROJECTION_MATRIX, 1, &m_pd3dcbViewProjection);

	//////////////////
	m_pScene->Render(m_pd3dDeviceContext);

	m_pDXGISwapChain->Present(0, 0);

	m_GameTimer.GetFrameRate(m_pszBuffer+8, 37);
	::SetWindowText(m_hWnd, m_pszBuffer);

}

void CGameFramework::SetCameraPos()
{


	D3DXVECTOR3 d3dxvEyePosition = D3DXVECTOR3(m_CameraPosX, 50.0f, m_CameraPosZ);
	D3DXVECTOR3 d3dxvLookAt = D3DXVECTOR3(m_CameraPosX, 0.0f, m_CameraPosZ+17.0f);
	m_vCamera.SetViewParams( &d3dxvEyePosition, &d3dxvLookAt );



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
		m_CameraPosZ <= -350;
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