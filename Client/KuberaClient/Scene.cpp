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
	m_bJoinOtherPlayer = FALSE;
	m_bJoin = FALSE;
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
	m_ppShaders[0]->CreateShader(pd3dDevice, 6);

	//게임 객체에 대한 포인터들의 배열을 정의한다.
	m_nObjects = 6;
	m_ppObjects = new CGameObject*[m_nObjects]; 

	//정육면체 메쉬를 생성하고 객체에 연결한다.
	//CCubeMesh *pMesh = new CCubeMesh(pd3dDevice, 15.0f, 15.0f, 15.0f);
	pHeroMesh = new CFBXMesh(pd3dDevice, L"Wizard101310.FBX");
	pHeroMesh->LoadTexture(pd3dDevice, L"micro_wizard_col.tif");

	CFBXMesh *pPlaneMesh = new CFBXMesh(pd3dDevice, L"Plane4.FBX");
	pPlaneMesh->LoadTexture(pd3dDevice, L"floor.png");

	CFBXMesh *pObstacleMesh = new CFBXMesh(pd3dDevice, L"20Box.FBX");
	pObstacleMesh->LoadTexture(pd3dDevice, L"micro_wizard_col.tif");

	//삼각형 객체(CTriangleObject)를 생성하고 삼각형 메쉬를 연결한다.
	CGameObject *pHero = new CGameObject();
	pHero->SetMesh(pHeroMesh);
	m_Control.m_Player = pHero;
	pHero->SetBoundSize(10, 13, 10);

	CGameObject *pPlane = new CGameObject();
	pPlane->SetMesh(pPlaneMesh);
	
	CGameObject *pObstacle = new CGameObject();
	pObstacle->SetMesh(pObstacleMesh);
	pObstacle->SetBoundSize(20, 20, 20);
	pObstacle->SetPosition(D3DXVECTOR3(25,0,0));

	//충돌박스
	int iObjectNum = 2;
	CCubeMesh *pBox[2];
	CGameObject *pBoundBox[2];
	pBox[0] = new CCubeMesh(pd3dDevice,pHero->GetBoundSizeX(),pHero->GetBoundSizeY(),pHero->GetBoundSizeZ());
	pBox[1] = new CCubeMesh(pd3dDevice,pObstacle->GetBoundSizeX(),pObstacle->GetBoundSizeY(),pObstacle->GetBoundSizeZ());
	
	for(int i=0; i<iObjectNum; i++)
	{
		pBoundBox[i] = new CGameObject;
		pBoundBox[i]->SetMesh(pBox[i]);
	}
	pBoundBox[1]->SetPosition(pObstacle->GetPosition());


	//pFBXMesh->Release();

 	//삼각형 객체를 쉐이더 객체에 연결한다.
 	m_ppShaders[0]->AddObject(pHero);
	m_ppShaders[0]->AddObject(pPlane);
	m_ppShaders[0]->AddObject(pObstacle);
	for(int i=0; i<iObjectNum; i++)
	{
		m_ppShaders[0]->AddObject(pBoundBox[i]);
	}

 	m_ppObjects[0] = pHero;
	m_ppObjects[0]->SetTag(HERO);
	m_ppObjects[1] = pPlane;
	m_ppObjects[1]->SetTag(PLANE);
	m_ppObjects[2] = pObstacle;
	m_ppObjects[2]->SetTag(OBSTACLE);
	m_ppObjects[3] = pBoundBox[0];
	m_ppObjects[3]->SetTag(HERO_BOUND);
	m_ppObjects[4] = pBoundBox[1];
	m_ppObjects[4]->SetTag(OBSTACLE_BOUND);
	m_ppObjects[5] = NULL;
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
		for (int j = 0; j < m_nObjects; j++) 
		{
			if(m_ppObjects[j] == NULL) continue;
			m_ppObjects[j]->Release();
		}
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
		/*윈도우의 크기가 변경될 때(현재는 “Alt+Enter“ 전체 화면 모드와 윈도우 모드로 전환될 때) 스왑 체인의 후면버퍼 크기를 조정하고 후면버퍼에 대한 렌더 타겟 뷰를 다시 생성한다. */
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

void CScene::AnimateObjects(float fTimeElapsed, ID3D11Device *pd3dDevice)
{
	if(m_bJoinOtherPlayer == TRUE && m_bJoin == FALSE)
	{
		AddOtherPlayer(pd3dDevice);
		m_bJoinOtherPlayer = FALSE;
		m_bJoin  = TRUE;
	}

	for (int j = 0; j < m_nObjects; j++)
	{
		if(m_ppObjects[j] == NULL) continue;

		m_ppObjects[j]->Animate(fTimeElapsed);
		m_ppObjects[j]->Update(fTimeElapsed);

		if(m_ppObjects[j]->GetTag() == HERO_BOUND) //플레이어 충돌박스 보이기
			m_ppObjects[j]->SetPosition(m_ppObjects[0]->GetPosition());

		if(m_ppObjects[j]->GetTag() == HERO)
		{
			for(int i=0; i< m_nObjects; i++)
			{
				if(m_ppObjects[i] == NULL)
					continue;

				if(m_ppObjects[i]->GetTag() == OBSTACLE)
				{
					if(m_ppObjects[i]->GetBoundSizeX() + m_ppObjects[j]->GetBoundSizeX()>
						sqrt(double((m_ppObjects[i]->GetPosition().x - m_ppObjects[j]->GetPosition().x)*
						(m_ppObjects[i]->GetPosition().x - m_ppObjects[j]->GetPosition().x)) + 
						double((m_ppObjects[i]->GetPosition().z - m_ppObjects[j]->GetPosition().z)*
						(m_ppObjects[i]->GetPosition().z - m_ppObjects[j]->GetPosition().z))))
						m_ppObjects[j]->SetAstar(TRUE);
					else
						if(m_ppObjects[j]->GetAstar() == TRUE)
							m_ppObjects[j]->SetAstar(FALSE);
				}
			}
		}
	}

	GameCollision();

}

void CScene::Render(ID3D11DeviceContext*pd3dDeviceContext)
{
	//쉐이더 객체 리스트의 각 쉐이더 객체를 렌더링한다.
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

void CScene::AddOtherPlayer(ID3D11Device *pd3dDevice)
{
	CGameObject* OtherPlayer = new CGameObject();
	OtherPlayer->SetMesh(pHeroMesh);

	m_ppShaders[0]->AddObject(OtherPlayer);  //세팅시 배열 숫자 조정
	m_ppObjects[5] = OtherPlayer;  //세팅시 배열 숫자 조정
}


CGameObject* CScene::GetObject(int num)
{
	return m_ppObjects[num];
}

//박스대박스 충돌체크
// box vs box
BOOL CScene::CheckCollisionAABBAxis( float c1, float w1, float c2, float w2)
{
	if ( abs(c1-c2) < (w1+w2)/2)
		return TRUE;
	return FALSE;
}
// box vs box
BOOL CScene::CheckCollision(D3DXVECTOR3 c1, float w1, float h1, float d1, 
	D3DXVECTOR3 c2, float w2, float h2, float d2)
{
	if (CheckCollisionAABBAxis( c1.x, w1, c2.x, w2)
		&& CheckCollisionAABBAxis( c1.y, h1, c2.y, h2)
		&& CheckCollisionAABBAxis( c1.z, d1, c2.z, d2)) {
			return TRUE;
	}

	return FALSE;
}

void CScene::GameCollision()
{
	for(int i = 1; i < m_nObjects; i++)
	{
		if(m_ppObjects[i] == NULL) continue;

		
		if(CheckCollision(m_ppObjects[0]->GetPosition(), m_ppObjects[0]->GetBoundSizeX(), m_ppObjects[0]->GetBoundSizeY(), 
			m_ppObjects[0]->GetBoundSizeZ(), m_ppObjects[2]->GetPosition(),m_ppObjects[2]->GetBoundSizeX(), m_ppObjects[2]->GetBoundSizeY(), 
			m_ppObjects[2]->GetBoundSizeZ()))
		{

			//캐릭터 왼쪽방향
			if(m_ppObjects[0]->GetPosition().x > m_ppObjects[2]->GetPosition().x && 
				abs(m_ppObjects[2]->GetPosition().x - m_ppObjects[0]->GetPosition().x) > 
				abs(m_ppObjects[2]->GetPosition().z - m_ppObjects[0]->GetPosition().z))
			{
				if(m_ppObjects[0]->GetPosition().x - m_ppObjects[0]->GetBoundSizeX()/2 < 
					m_ppObjects[2]->GetPosition().x + m_ppObjects[2]->GetBoundSizeX()/2)
				{
					D3DXVECTOR3 v = D3DXVECTOR3(m_ppObjects[2]->GetPosition().x + m_ppObjects[2]->GetBoundSizeX()/2 +
						m_ppObjects[0]->GetBoundSizeX()/2, m_ppObjects[0]->GetPosition().y, m_ppObjects[0]->GetPosition().z);
					m_ppObjects[0]->SetPosition(v);
				}
			}

			//캐릭터 오른쪽 방향
			else if(m_ppObjects[0]->GetPosition().x < m_ppObjects[2]->GetPosition().x &&
				abs(m_ppObjects[2]->GetPosition().x - m_ppObjects[0]->GetPosition().x) > 
				abs(m_ppObjects[2]->GetPosition().z - m_ppObjects[0]->GetPosition().z))
			{
				if(m_ppObjects[0]->GetPosition().x + m_ppObjects[0]->GetBoundSizeX()/2 > 
					m_ppObjects[2]->GetPosition().x - m_ppObjects[2]->GetBoundSizeX()/2)
				{
					D3DXVECTOR3 v = D3DXVECTOR3(m_ppObjects[2]->GetPosition().x - m_ppObjects[2]->GetBoundSizeX()/2 - 
						m_ppObjects[0]->GetBoundSizeX()/2, m_ppObjects[0]->GetPosition().y, m_ppObjects[0]->GetPosition().z);
					m_ppObjects[0]->SetPosition(v);
				}
			}
			//캐릭터 위쪽 방향
			else if(m_ppObjects[0]->GetPosition().z < m_ppObjects[2]->GetPosition().z &&
				abs(m_ppObjects[2]->GetPosition().x - m_ppObjects[0]->GetPosition().x) < 
				abs(m_ppObjects[2]->GetPosition().z - m_ppObjects[0]->GetPosition().z))
			{
				if(m_ppObjects[0]->GetPosition().z + m_ppObjects[0]->GetBoundSizeZ()/2 > 
					m_ppObjects[2]->GetPosition().z - m_ppObjects[2]->GetBoundSizeZ()/2)
				{
					D3DXVECTOR3 v = D3DXVECTOR3(m_ppObjects[0]->GetPosition().x, m_ppObjects[0]->GetPosition().y,
						m_ppObjects[2]->GetPosition().z - m_ppObjects[2]->GetBoundSizeZ()/2 - m_ppObjects[0]->GetBoundSizeZ()/2);
					m_ppObjects[0]->SetPosition(v);
				}
			}
	
			//캐릭터 아래쪽 방향
			else if(m_ppObjects[0]->GetPosition().z > m_ppObjects[2]->GetPosition().z &&
				abs(m_ppObjects[2]->GetPosition().x - m_ppObjects[0]->GetPosition().x) < 
				abs(m_ppObjects[2]->GetPosition().z - m_ppObjects[0]->GetPosition().z))
			{
				if(m_ppObjects[0]->GetPosition().z + m_ppObjects[0]->GetBoundSizeZ()/2 > 
					m_ppObjects[2]->GetPosition().z - m_ppObjects[2]->GetBoundSizeZ()/2)
				{
					D3DXVECTOR3 v = D3DXVECTOR3(m_ppObjects[0]->GetPosition().x, m_ppObjects[0]->GetPosition().y, 
						m_ppObjects[2]->GetPosition().z + m_ppObjects[2]->GetBoundSizeZ()/2 + m_ppObjects[0]->GetBoundSizeZ()/2);
					m_ppObjects[0]->SetPosition(v);
				}
			}

			break;
		}
		

	}
}