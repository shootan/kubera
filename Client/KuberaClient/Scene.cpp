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
	//이 쉐이더 객체에 대한 포인터들의 배열을 정의한다.
	m_nShaders = 1;
	m_ppShaders = new CObjectShader*[m_nShaders];
	//CObjectShader 클래스 객체를 생성한다.
	m_ppShaders[0] = new CObjectShader();
	m_ppShaders[0]->CreateShader(pd3dDevice, 2);

	//게임 객체에 대한 포인터들의 배열을 정의한다.
	m_nObjects = 2;
	m_ppObjects = new CGameObject*[m_nObjects]; 

	//정육면체 메쉬를 생성하고 객체에 연결한다.
	//CCubeMesh *pMesh = new CCubeMesh(pd3dDevice, 15.0f, 15.0f, 15.0f);
	CFBXMesh *pFBXMesh = new CFBXMesh(pd3dDevice, L"Wizard.FBX");
	//CFBXMesh *pFBXMesh = new CFBXMesh(pd3dDevice, L"20box.fbx");
	pFBXMesh->LoadTexture(pd3dDevice, L"micro_wizard_col.tif");
	CFBXMesh *pPlane = new CFBXMesh(pd3dDevice, L"floor.FBX");
	pPlane->LoadTexture(pd3dDevice, L"floor.png");
	

	//삼각형 객체(CTriangleObject)를 생성하고 삼각형 메쉬를 연결한다.
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

 	//삼각형 객체를 쉐이더 객체에 연결한다.
 	m_ppShaders[0]->AddObject(pObject);
	m_ppShaders[0]->AddObject(pObject2);
 	m_ppObjects[0] = pObject;
	m_ppObjects[1] = pObject2;
}

void CScene::ReleaseObjects()
{
	//쉐이더 객체 리스트의 각 객체를 소멸시키고 리스트를 소멸시킨다.
	if (m_ppShaders) 
	{
		for (int j = 0; j < m_nShaders; j++) if (m_ppShaders[j]) delete m_ppShaders[j];
		delete [] m_ppShaders;
	}

	//게임 객체 리스트의 각 객체를 반환(Release)하고 리스트를 소멸시킨다.
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
		/*윈도우의 크기가 변경될 때(현재는 “Alt+Enter“ 전체 화면 모드와 윈도우 모드로 전환될 때) 스왑 체인의 후면버퍼 크기를 조정하고 후면버퍼에 대한 렌더 타겟 뷰를 다시 생성한다. */
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
	//쉐이더 객체 리스트의 각 쉐이더 객체를 렌더링한다.
	for (int i = 0; i < m_nShaders; i++)
	{
		m_ppShaders[i]->Render(pd3dDeviceContext);
	}
}