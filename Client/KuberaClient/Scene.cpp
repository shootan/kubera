#include "Scene.h"

CScene::CScene(void)
{
	m_pObjectShaders = NULL;
	m_pInstancingShaders = NULL;
	m_nShaders = 0;
     
	m_nObjects = 0;
	m_nIntanceObjects = 0;

	m_MousePosX = 0;
	m_MousePosY = 0;

	m_bRbutton = FALSE;
	m_bJoinOtherPlayer = FALSE;
	m_bJoin = FALSE;

	m_fMinionRespawnTime = 0.f;
	m_nMinionObjects = 0;

	m_pHero = NULL;
	m_pPlane = NULL;

	for(int i=0; i<10; i++)
		m_pTower[i] = NULL;
}


CScene::~CScene(void)
{

}

void CScene::BuildObjects(ID3D11Device *pd3dDevice)
{
	m_Control.m_Camera = m_Camera;
	//이 쉐이더 객체에 대한 포인터들의 배열을 정의한다.

	//CObjectShader 클래스 객체를 생성한다.
	m_pObjectShaders = new CObjectShader();
	m_pObjectShaders->CreateShader(pd3dDevice, 100);

	m_pInstancingShaders = new CInstancingShader();
	m_pInstancingShaders->CreateShader(pd3dDevice, 10);
	m_pInstancingShaders->BuildObjects(pd3dDevice);
		
	//게임 객체에 대한 포인터들의 배열을 정의한다.
	m_nIntanceObjects = m_pInstancingShaders->GetObjectsNumber();
	m_nObjects = 20 + m_nIntanceObjects;

	//정육면체 메쉬를 생성하고 객체에 연결한다.
	//CCubeMesh *pMesh = new CCubeMesh(pd3dDevice, 15.0f, 15.0f, 15.0f);
	pHeroMesh = new CFBXMesh(pd3dDevice, L"Wizard101310.FBX");
	pHeroMesh->LoadTexture(pd3dDevice, L"micro_wizard_col.tif");

	CFBXMesh *pPlaneMesh = new CFBXMesh(pd3dDevice, L"Plane4.FBX");
	pPlaneMesh->LoadTexture(pd3dDevice, L"map2.png");

	CFBXMesh *pObstacleMesh = new CFBXMesh(pd3dDevice, L"tower/Tower1_303030.FBX");
	pObstacleMesh->LoadTexture(pd3dDevice, L"micro_wizard_col.tif");

	pMinionDragonMesh = new CFBXMesh(pd3dDevice, L"Dragon7107.FBX");
	pMinionDragonMesh->LoadTexture(pd3dDevice, L"micro_dragon_col.tif");


	//삼각형 객체(CTriangleObject)를 생성하고 삼각형 메쉬를 연결한다.
	m_pHero = new HeroObject;
	m_pHero->SetMesh(pHeroMesh);
	m_Control.m_Player = m_pHero;
	m_pHero->SetPosition(D3DXVECTOR3(550, 0, 0));
	m_pHero->SetBoundSize(10, 13, 10);

	m_pPlane = new CGameObject();
	m_pPlane->SetMesh(pPlaneMesh);

	
	
// 	MinionObject* pMinion[50];
// 	for(int i=0; i<50; i++)
// 	{
// 		pMinion[i] = new MinionObject();
// 		pMinion[i]->SetMesh(pMinionDragonMesh);
// 		pMinion[i]->SetBoundSize(7, 10 ,7);
// 		m_ppShaders[0]->AddObject(pMinion[i]);
// 	}

	/*m_pTower = new TowerObject();
	m_pTower->SetMesh(pObstacleMesh);
	m_pTower->SetPosition(D3DXVECTOR3(450,0,0));
*/

	//충돌박스
	int iObjectNum = 2;
	CCubeMesh *pBox[2];
	CGameObject *pBoundBox[2];
	pBox[0] = new CCubeMesh(pd3dDevice,m_pHero->GetBoundSizeX(),m_pHero->GetBoundSizeY(),m_pHero->GetBoundSizeZ());
	pBox[1] = new CCubeMesh(pd3dDevice,20,20,20);
	
	for(int i=0; i<iObjectNum; i++)
	{
		pBoundBox[i] = new CGameObject;
		pBoundBox[i]->SetMesh(pBox[i]);
	}
	pBoundBox[1]->SetPosition(D3DXVECTOR3(25,0,0));


	//pFBXMesh->Release();

 	//삼각형 객체를 쉐이더 객체에 연결한다.
 	m_pObjectShaders->AddObject(m_pHero);
	m_pObjectShaders->AddObject(m_pPlane);
	for(int i=0; i<10; i++)
		m_pObjectShaders->AddObject(m_pTower[i]);

	for(int i=0; i<iObjectNum; i++)
		m_pObjectShaders->AddObject(pBoundBox[i]);

	for(int i=0; i<10;i++)
	{
		m_pOtherPlayer[i] = new EnemyObject;
		m_pOtherPlayer[i]->SetMesh(pHeroMesh);
		m_pObjectShaders->AddObject(m_pOtherPlayer[i]);
	}


	for(int i=0; i < 10; i++)
	{
		m_pTower[i] = m_pInstancingShaders->GetTowerObject(i);
	}


	
	//for(int i=0; i<m_nIntanceObjects; i++)
	//	m_pTower[i] = m_ppShaders[1]->GetObject(i);

	//m_ppObjects[m_nIntanceObjects] = m_pHero;
	//m_ppObjects[m_nIntanceObjects + 1] = pPlane;
	//m_ppObjects[m_nIntanceObjects + 1]->SetTag(PLANE);
	//m_ppObjects[m_nIntanceObjects + 2] = NULL;
	//m_ppObjects[m_nIntanceObjects + 3] = pBoundBox[0];
	//m_ppObjects[m_nIntanceObjects + 3]->SetTag(HERO_BOUND);
	//m_ppObjects[m_nIntanceObjects + 4] = pBoundBox[1];
	//m_ppObjects[m_nIntanceObjects + 4]->SetTag(OBSTACLE_BOUND);

	//m_ppObjects[m_nIntanceObjects + 5] = pTower;
	//m_ppObjects[m_nIntanceObjects + 5]->SetTag(TOWER);
// 
// 	for(int i=6; i<56; i++)
// 		m_ppObjects[i] = pMinion[i-5];

	//for(int i= m_nIntanceObjects + 6; i<m_nObjects; i++)
	//	m_ppObjects[i] = NULL;

	this->AddOtherPlayer(pd3dDevice);

}

void CScene::ReleaseObjects()
{
	//쉐이더 객체 리스트의 각 객체를 소멸시키고 리스트를 소멸시킨다.

	if(m_pObjectShaders) delete m_pObjectShaders;
	if(m_pInstancingShaders) delete m_pInstancingShaders;

	//게임 객체 리스트의 각 객체를 반환(Release)하고 리스트를 소멸시킨다.

	if(m_pHero)	m_pHero->Release();
	if(m_pPlane) m_pPlane->Release();

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
	m_fMinionRespawnTime += fTimeElapsed;

	if(m_fMinionRespawnTime >= 0.5f && m_nMinionObjects < 10)
	{
		AddMinion(pd3dDevice);
		m_fMinionRespawnTime = 0.f;
	}

	if(m_nMinionObjects == 10)
	{
		if(m_fMinionRespawnTime > 20.0f)
			m_nMinionObjects = 0;
	}

	m_pHero->Animate(fTimeElapsed);
	m_pHero->Update(fTimeElapsed);

	
	for(int i=0;i<10; i++)
	{
		m_pTower[i]->Update(fTimeElapsed);

		distance = ST::sharedManager()->GetDistance(m_pHero->GetPos(), m_pTower[i]->GetPos());
		if(distance < 50.0f)
			m_pTower[i]->SetPos(m_pHero->GetPos());
	}
	
	//for (int j = 0; j < m_nObjects; j++)
	//{
	//	if(m_ppObjects[j] == NULL) continue;

	//	m_ppObjects[j]->Animate(fTimeElapsed);
	//	m_ppObjects[j]->Update(fTimeElapsed);

	//	if(m_ppObjects[j]->GetTag() == TOWER)
	//	{
	//		for(int i = 0; i<m_nObjects; i++)
	//		{
	//			if(m_ppObjects[i] == NULL) continue;
	//			if(m_ppObjects[i]->GetTag() == HERO)
	//			{
	//				float distance = ST::sharedManager()->GetDistance(m_ppObjects[j]->GetPos(), m_ppObjects[i]->GetPos());
	//				
	//				if(distance < 50.0f)
	//					m_ppObjects[j]->SetPos(m_ppObjects[i]->GetPos());
	//				//	TowerObject* tt = m_ppObjects[j];

	//			}
	//		}
	//	}

	//	if(m_ppObjects[j]->GetTag() == HERO_BOUND) //플레이어 충돌박스 보이기
	//		m_ppObjects[j]->SetPosition(m_ppObjects[0]->GetPosition());

		/*if(m_ppObjects[j]->GetTag() == HERO)
		{
			for(int i=0; i< m_nObjects; i++)
			{
				if(m_ppObjects[i] == NULL)
					continue;
				if(m_ppObjects[i]->GetTag() == OBSTACLE)
				{
					if(m_ppObjects[i]->GetBoundSizeX()*2 + m_ppObjects[j]->GetBoundSizeX()>
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
		}*/
	//}

	//GameCollision();

}

void CScene::Render(ID3D11DeviceContext*pd3dDeviceContext)
{
	//쉐이더 객체 리스트의 각 쉐이더 객체를 렌더링한다.
	
	m_pObjectShaders->Render(pd3dDeviceContext);

	m_pObjectShaders->UpdateShaderVariables(pd3dDeviceContext, &m_pHero->m_d3dxmtxWorld);
	m_pHero->Render(pd3dDeviceContext);

	for(int i=0; i<10; i++)
	{
		if(m_pOtherPlayer[i]->GetVisible() != TRUE) continue;

		m_pObjectShaders->UpdateShaderVariables(pd3dDeviceContext, &m_pOtherPlayer[i]->m_d3dxmtxWorld);
		m_pOtherPlayer[i]->Render(pd3dDeviceContext);
	}
	m_pObjectShaders->UpdateShaderVariables(pd3dDeviceContext, &m_pPlane->m_d3dxmtxWorld);
	m_pPlane->Render(pd3dDeviceContext);

	m_pInstancingShaders->Render(pd3dDeviceContext);
	/*for(int i=0; i<10; i++)
		m_pTower[i]->Render(pd3dDeviceContext);*/
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
	//int CheckCount = 0;
	//for(int i=0; i<m_nObjects; i++)
	//{
	//	if ( m_ppObjects[i] != NULL) continue;
	//	CGameObject* OtherPlayer = new CGameObject();
	//	OtherPlayer->SetMesh(pHeroMesh);
	//	m_ppShaders[0]->AddObject(OtherPlayer);  //세팅시 배열 숫자 조정
	//	m_ppObjects[i] = OtherPlayer;  //세팅시 배열 숫자 조정
	//	m_ppObjects[i]->SetTag(OTHERPLAYER);
	//	m_ppObjects[i]->SetVisible(FALSE);
	//	CheckCount++;
	//	if(CheckCount > 9) break;
	//}
}

void CScene::SetOtherClient(PlayerStruct* _PI, int _Count)
{
	_Count -= 1;

	for(int i=0; i<_Count; i++)
	{
		if(_PI[i].Use == TRUE) continue;
		if(_PI[i].PI.m_ID == 0) continue;
		for(int j=0; j<10; j++)
		{
			if(m_pOtherPlayer[j]->GetID() != 0) continue;
			m_pOtherPlayer[j]->SetID(_PI[i].PI.m_ID);
			m_pOtherPlayer[j]->SetPos(_PI[i].PI.m_Pos);
			m_pOtherPlayer[j]->SetRot(_PI[i].PI.m_Rot);
			m_pOtherPlayer[j]->SetScale(_PI[i].PI.m_Scale);
			m_pOtherPlayer[j]->SetVisible(TRUE);
			_PI[i].Use = TRUE;
			break;
		}
	}
}

void CScene::UpdateOtherClient(PlayerStruct* _PI, int _Count)
{
	_Count -= 1;

	for(int i=0; i<_Count; i++)
	{
		if(_PI[i].Use != TRUE) continue;
		if(_PI[i].PI.m_ID == 0) continue;
		for(int j=0; j<10; j++)
		{
			if(m_pOtherPlayer[j]->GetID() != _PI[i].PI.m_ID) continue;

			m_pOtherPlayer[j]->SetPos(_PI[i].PI.m_Pos);
			m_pOtherPlayer[j]->SetRot(_PI[i].PI.m_Rot);
			m_pOtherPlayer[j]->SetScale(_PI[i].PI.m_Scale);
			break;
		}
	}
}

void CScene::AddMinion(ID3D11Device *pd3dDevice)
{

	/*for(int i=0; i<m_nObjects; i++)
	{
		if(m_ppObjects[i] == NULL)
			continue;
		if(m_ppObjects[i]->GetTag() == MINION && m_ppObjects[i]->GetVisible() == FALSE)
		{
			m_ppObjects[i]->SetVisible(TRUE);
			m_ppObjects[i]->SetPosition(D3DXVECTOR3(500,0,0));
			if(m_nMinionObjects % 2 == 0)
				m_ppObjects[i]->SetNewDestination(D3DXVECTOR3(0, 0, 300));
			else
				m_ppObjects[i]->SetNewDestination(D3DXVECTOR3(0, 0, -300));
			m_nMinionObjects++;
			return;
		}
	}*/
}

CGameObject* CScene::GetObject(int num)
{
	//return m_ppObjects[num];
	return 0;
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
	//for(int i = 1; i < m_nObjects; i++)
	//{
	//	if(m_ppObjects[i] == NULL) continue;

	//	
	//	if(CheckCollision(m_ppObjects[0]->GetPosition(), m_ppObjects[0]->GetBoundSizeX(), m_ppObjects[0]->GetBoundSizeY(), 
	//		m_ppObjects[0]->GetBoundSizeZ(), m_ppObjects[2]->GetPosition(),m_ppObjects[2]->GetBoundSizeX(), m_ppObjects[2]->GetBoundSizeY(), 
	//		m_ppObjects[2]->GetBoundSizeZ()))
	//	{

	//		//캐릭터 왼쪽방향
	//		if(m_ppObjects[0]->GetPosition().x > m_ppObjects[2]->GetPosition().x && 
	//			abs(m_ppObjects[2]->GetPosition().x - m_ppObjects[0]->GetPosition().x) > 
	//			abs(m_ppObjects[2]->GetPosition().z - m_ppObjects[0]->GetPosition().z))
	//		{
	//			if(m_ppObjects[0]->GetPosition().x - m_ppObjects[0]->GetBoundSizeX()/2 < 
	//				m_ppObjects[2]->GetPosition().x + m_ppObjects[2]->GetBoundSizeX()/2)
	//			{
	//				D3DXVECTOR3 v = D3DXVECTOR3(m_ppObjects[2]->GetPosition().x + m_ppObjects[2]->GetBoundSizeX()/2 +
	//					m_ppObjects[0]->GetBoundSizeX()/2, m_ppObjects[0]->GetPosition().y, m_ppObjects[0]->GetPosition().z);
	//				m_ppObjects[0]->SetPosition(v);
	//			}
	//		}

	//		//캐릭터 오른쪽 방향
	//		else if(m_ppObjects[0]->GetPosition().x < m_ppObjects[2]->GetPosition().x &&
	//			abs(m_ppObjects[2]->GetPosition().x - m_ppObjects[0]->GetPosition().x) > 
	//			abs(m_ppObjects[2]->GetPosition().z - m_ppObjects[0]->GetPosition().z))
	//		{
	//			if(m_ppObjects[0]->GetPosition().x + m_ppObjects[0]->GetBoundSizeX()/2 > 
	//				m_ppObjects[2]->GetPosition().x - m_ppObjects[2]->GetBoundSizeX()/2)
	//			{
	//				D3DXVECTOR3 v = D3DXVECTOR3(m_ppObjects[2]->GetPosition().x - m_ppObjects[2]->GetBoundSizeX()/2 - 
	//					m_ppObjects[0]->GetBoundSizeX()/2, m_ppObjects[0]->GetPosition().y, m_ppObjects[0]->GetPosition().z);
	//				m_ppObjects[0]->SetPosition(v);
	//			}
	//		}
	//		//캐릭터 위쪽 방향
	//		else if(m_ppObjects[0]->GetPosition().z < m_ppObjects[2]->GetPosition().z &&
	//			abs(m_ppObjects[2]->GetPosition().x - m_ppObjects[0]->GetPosition().x) < 
	//			abs(m_ppObjects[2]->GetPosition().z - m_ppObjects[0]->GetPosition().z))
	//		{
	//			if(m_ppObjects[0]->GetPosition().z + m_ppObjects[0]->GetBoundSizeZ()/2 > 
	//				m_ppObjects[2]->GetPosition().z - m_ppObjects[2]->GetBoundSizeZ()/2)
	//			{
	//				D3DXVECTOR3 v = D3DXVECTOR3(m_ppObjects[0]->GetPosition().x, m_ppObjects[0]->GetPosition().y,
	//					m_ppObjects[2]->GetPosition().z - m_ppObjects[2]->GetBoundSizeZ()/2 - m_ppObjects[0]->GetBoundSizeZ()/2);
	//				m_ppObjects[0]->SetPosition(v);
	//			}
	//		}
	//
	//		//캐릭터 아래쪽 방향
	//		else if(m_ppObjects[0]->GetPosition().z > m_ppObjects[2]->GetPosition().z &&
	//			abs(m_ppObjects[2]->GetPosition().x - m_ppObjects[0]->GetPosition().x) < 
	//			abs(m_ppObjects[2]->GetPosition().z - m_ppObjects[0]->GetPosition().z))
	//		{
	//			if(m_ppObjects[0]->GetPosition().z + m_ppObjects[0]->GetBoundSizeZ()/2 > 
	//				m_ppObjects[2]->GetPosition().z - m_ppObjects[2]->GetBoundSizeZ()/2)
	//			{
	//				D3DXVECTOR3 v = D3DXVECTOR3(m_ppObjects[0]->GetPosition().x, m_ppObjects[0]->GetPosition().y, 
	//					m_ppObjects[2]->GetPosition().z + m_ppObjects[2]->GetBoundSizeZ()/2 + m_ppObjects[0]->GetBoundSizeZ()/2);
	//				m_ppObjects[0]->SetPosition(v);
	//			}
	//		}

	//		break;
	//	}
	//	

	//}
}