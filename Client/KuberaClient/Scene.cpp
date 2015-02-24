#include "Scene.h"


CScene::CScene(void)
{
	m_ppShaders = NULL;
	m_nShaders = 0;

	m_ppObjects = NULL;       
	m_nObjects = 0;

	m_MousePosX = 0;
	m_MousePosY = 0;

	m_bRbutton = FALSE;
}


CScene::~CScene(void)
{

}

void CScene::BuildObjects(ID3D11Device *pd3dDevice)
{
	m_Control.m_Camera = m_Camera;
	//�� ���̴� ��ü�� ���� �����͵��� �迭�� �����Ѵ�.
	m_nShaders = 1;
	m_ppShaders = new CObjectShader*[m_nShaders];
	//CObjectShader Ŭ���� ��ü�� �����Ѵ�.
	m_ppShaders[0] = new CObjectShader();
	m_ppShaders[0]->CreateShader(pd3dDevice, 3);

	//���� ��ü�� ���� �����͵��� �迭�� �����Ѵ�.
	m_nObjects = 3;
	m_ppObjects = new CGameObject*[m_nObjects]; 

	//������ü �޽��� �����ϰ� ��ü�� �����Ѵ�.
	//CCubeMesh *pMesh = new CCubeMesh(pd3dDevice, 15.0f, 15.0f, 15.0f);
	CFBXMesh *pFBXMesh = new CFBXMesh(pd3dDevice, L"micro_wizard.FBX");
	pFBXMesh->LoadTexture(pd3dDevice, L"micro_wizard_col.tif");

	CFBXMesh *pPlane = new CFBXMesh(pd3dDevice, L"Plane.FBX");
	pPlane->LoadTexture(pd3dDevice, L"floor.png");

	CFBXMesh *pFBXMesh1 = new CFBXMesh(pd3dDevice, L"20Box.FBX");
	pFBXMesh1->LoadTexture(pd3dDevice, L"micro_wizard_col.tif");

	//�ﰢ�� ��ü(CTriangleObject)�� �����ϰ� �ﰢ�� �޽��� �����Ѵ�.
	CGameObject *pObject = new CGameObject();
	pObject->SetMesh(pFBXMesh);
	pObject->SetScale(D3DXVECTOR3(0.2, 0.2, 0.2));
	//pObject->SetRotation(2, 180);
	m_Control.m_Player = pObject;


	CGameObject *pObject2 = new CGameObject();
	pObject2->SetMesh(pPlane);
	pObject2->SetScale(D3DXVECTOR3(20, 20, 20));
	//pObject2->SetRotation(2, 180);
	pObject2->SetRotation(1, -2);

	CGameObject *pObject3 = new CGameObject();
	pObject3->SetMesh(pFBXMesh1);
	
	pObject3->SetPosition(D3DXVECTOR3(25,0,0));


	pFBXMesh->Release();

 	//�ﰢ�� ��ü�� ���̴� ��ü�� �����Ѵ�.
 	m_ppShaders[0]->AddObject(pObject);
	m_ppShaders[0]->AddObject(pObject2);
	m_ppShaders[0]->AddObject(pObject3);
 	m_ppObjects[0] = pObject;
	m_ppObjects[1] = pObject2;
	m_ppObjects[2] = pObject3;
}

void CScene::ReleaseObjects()
{
	//���̴� ��ü ����Ʈ�� �� ��ü�� �Ҹ��Ű�� ����Ʈ�� �Ҹ��Ų��.
	if (m_ppShaders) 
	{
		for (int j = 0; j < m_nShaders; j++) if (m_ppShaders[j]) delete m_ppShaders[j];
		delete [] m_ppShaders;
	}

	//���� ��ü ����Ʈ�� �� ��ü�� ��ȯ(Release)�ϰ� ����Ʈ�� �Ҹ��Ų��.
	if (m_ppObjects)
	{
		for (int j = 0; j < m_nObjects; j++) m_ppObjects[j]->Release();
		delete [] m_ppObjects;
	}
}

bool CScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	return(false);
}

bool CScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
		/*�������� ũ�Ⱑ ����� ��(����� ��Alt+Enter�� ��ü ȭ�� ���� ������ ���� ��ȯ�� ��) ���� ü���� �ĸ���� ũ�⸦ �����ϰ� �ĸ���ۿ� ���� ���� Ÿ�� �並 �ٽ� �����Ѵ�. */
	case WM_SIZE:
		{
			break;
		}
	case WM_LBUTTONDOWN:
		
		break;
	case WM_RBUTTONDOWN:
		m_MousePosX = (INT)LOWORD(lParam); 
		m_MousePosY = (INT)HIWORD(lParam);
		m_Control.TouchDown((float)m_MousePosX, (float)m_MousePosY, hWnd);
		m_bRbutton = TRUE;
		break;
	case WM_RBUTTONUP:
		m_bRbutton = FALSE;
		break;
	case WM_LBUTTONUP:
		break;
	case WM_MOUSEMOVE:
		m_MousePosX = (INT)LOWORD(lParam); 
		m_MousePosY = (INT)HIWORD(lParam);

		if(m_bRbutton == TRUE)
			m_Control.TouchDown((float)m_MousePosX, (float)m_MousePosY, hWnd);
		//OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
		break;
	case WM_KEYDOWN:
	case WM_KEYUP:
		//OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
		break;
	}
	return(false);
}

bool CScene::ProcessInput()
{
	return(false);
}

void CScene::AnimateObjects(float fTimeElapsed)
{
	for (int j = 0; j < m_nObjects; j++)
	{
			m_ppObjects[j]->Animate(fTimeElapsed);
			m_ppObjects[j]->Update(fTimeElapsed);
	}

}

void CScene::Render(ID3D11DeviceContext*pd3dDeviceContext)
{
	//���̴� ��ü ����Ʈ�� �� ���̴� ��ü�� �������Ѵ�.
	for (int i = 0; i < m_nShaders; i++)
	{
		m_ppShaders[i]->Render(pd3dDeviceContext);
	}
}

int CScene::GetMousePosX()
{
	return m_MousePosX;
}

int CScene::GetMousePosY()
{
	return m_MousePosY;
}