#include "Scene.h"


CScene::CScene(void)
{
	m_ppShaders = NULL;
	m_nShaders = 0;

	m_ppObjects = NULL;       
	m_nObjects = 0;

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
	m_ppShaders[0]->CreateShader(pd3dDevice, 2);

	//���� ��ü�� ���� �����͵��� �迭�� �����Ѵ�.
	m_nObjects = 2;
	m_ppObjects = new CGameObject*[m_nObjects]; 

	//������ü �޽��� �����ϰ� ��ü�� �����Ѵ�.
	//CCubeMesh *pMesh = new CCubeMesh(pd3dDevice, 15.0f, 15.0f, 15.0f);
	CFBXMesh *pFBXMesh = new CFBXMesh(pd3dDevice, L"Wizard.FBX");
	//CFBXMesh *pFBXMesh = new CFBXMesh(pd3dDevice, L"20box.fbx");
	pFBXMesh->LoadTexture(pd3dDevice, L"micro_wizard_col.tif");
	CFBXMesh *pPlane = new CFBXMesh(pd3dDevice, L"floor.FBX");
	pPlane->LoadTexture(pd3dDevice, L"floor.png");
	

	//�ﰢ�� ��ü(CTriangleObject)�� �����ϰ� �ﰢ�� �޽��� �����Ѵ�.
	CGameObject *pObject = new CRotatingObject();
	pObject->SetMesh(pFBXMesh);
	pObject->SetScale(D3DXVECTOR3(0.2, 0.2, 0.2));
	//pObject->SetRotation(2, 180);
	m_Control.m_Player = pObject;


	CGameObject *pObject2 = new CRotatingObject();
	pObject2->SetMesh(pPlane);
	pObject2->SetScale(D3DXVECTOR3(4, 4, 4));
	//pObject2->SetRotation(2, 180);
	pObject2->SetRotation(1, -2);
	pFBXMesh->Release();

 	//�ﰢ�� ��ü�� ���̴� ��ü�� �����Ѵ�.
 	m_ppShaders[0]->AddObject(pObject);
	m_ppShaders[0]->AddObject(pObject2);
 	m_ppObjects[0] = pObject;
	m_ppObjects[1] = pObject2;
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
	INT xPos;
	INT yPos;
	switch (nMessageID)
	{
		/*�������� ũ�Ⱑ ����� ��(����� ��Alt+Enter�� ��ü ȭ�� ���� ������ ���� ��ȯ�� ��) ���� ü���� �ĸ���� ũ�⸦ �����ϰ� �ĸ���ۿ� ���� ���� Ÿ�� �並 �ٽ� �����Ѵ�. */
	case WM_SIZE:
		{
			break;
		}
	case WM_LBUTTONDOWN:
		xPos = (INT)LOWORD(lParam); 
		yPos = (INT)HIWORD(lParam);
		m_Control.TouchDown((float)xPos, (float)yPos, hWnd);
		break;
	case WM_RBUTTONDOWN:
		break;
	case WM_LBUTTONUP:
		break;
	case WM_RBUTTONUP:
		break;
	case WM_MOUSEMOVE:
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
			m_ppObjects[j]->Update(5.0f);
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