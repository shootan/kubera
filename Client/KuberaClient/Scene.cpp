#include "Scene.h"


CScene::CScene(void)
{
	m_ppShaders = NULL;
	m_nShaders = 0;

	m_ppObjects = NULL;       
	m_nObjects = 0;
	m_nIntanceObjects = 0;

	m_MousePosX = 0;
	m_MousePosY = 0;

	m_bRbutton = FALSE;
	m_bJoinOtherPlayer = FALSE;
	m_bJoin = FALSE;

	m_fMinionRespawnTime = 0.f;
	m_nMinionObjects = 0;

}


CScene::~CScene(void)
{

}

void CScene::BuildObjects(ID3D11Device *pd3dDevice)
{
	m_Control.m_Camera = m_Camera;
	//�� ���̴� ��ü�� ���� �����͵��� �迭�� �����Ѵ�.
	m_nShaders = 2;
	m_ppShaders = new CShader*[m_nShaders];
	//CObjectShader Ŭ���� ��ü�� �����Ѵ�.
	m_ppShaders[0] = new CObjectShader();
	m_ppShaders[0]->CreateShader(pd3dDevice, 100);

	m_ppShaders[1] = new CInstancingShader();
	m_ppShaders[1]->CreateShader(pd3dDevice, 10);
	m_ppShaders[1]->BuildObjects(pd3dDevice);
		
	//���� ��ü�� ���� �����͵��� �迭�� �����Ѵ�.
	m_nIntanceObjects = m_ppShaders[1]->GetObjectsNumber();
	m_nObjects = 20 + m_nIntanceObjects;
	m_ppObjects = new CGameObject*[m_nObjects]; 

	//������ü �޽��� �����ϰ� ��ü�� �����Ѵ�.
	//CCubeMesh *pMesh = new CCubeMesh(pd3dDevice, 15.0f, 15.0f, 15.0f);
	pHeroMesh = new CFBXMesh(pd3dDevice, L"Wizard101310.FBX");
	pHeroMesh->LoadTexture(pd3dDevice, L"micro_wizard_col.tif");

	CFBXMesh *pPlaneMesh = new CFBXMesh(pd3dDevice, L"Plane4.FBX");
	pPlaneMesh->LoadTexture(pd3dDevice, L"floor_v5.png");

	CFBXMesh *pObstacleMesh = new CFBXMesh(pd3dDevice, L"tower/Tower1_303030.FBX");
	pObstacleMesh->LoadTexture(pd3dDevice, L"micro_wizard_col.tif");

	pMinionDragonMesh = new CFBXMesh(pd3dDevice, L"Dragon7107.FBX");
	pMinionDragonMesh->LoadTexture(pd3dDevice, L"micro_dragon_col.tif");

	//CFBXMesh *pObstacleBushMesh = new CFBXMesh(pd3dDevice, L"Bush1.fbx");
	//pObstacleBushMesh->LoadTexture(pd3dDevice, L"Bush1.tif");

	//�ﰢ�� ��ü(CTriangleObject)�� �����ϰ� �ﰢ�� �޽��� �����Ѵ�.
	HeroObject *pHero = new HeroObject();
	pHero->SetMesh(pHeroMesh);
	m_Control.m_Player = pHero;
	pHero->SetPosition(D3DXVECTOR3(550, 0, 0));
	pHero->SetBoundSize(10, 13, 10);

	CGameObject *pPlane = new CGameObject();
	pPlane->SetMesh(pPlaneMesh);
	
// 	MinionObject* pMinion[50];
// 	for(int i=0; i<50; i++)
// 	{
// 		pMinion[i] = new MinionObject();
// 		pMinion[i]->SetMesh(pMinionDragonMesh);
// 		pMinion[i]->SetBoundSize(7, 10 ,7);
// 		m_ppShaders[0]->AddObject(pMinion[i]);
// 	}

	TowerObject* pTower = new TowerObject();
	pTower->SetMesh(pObstacleMesh);
	pTower->SetPosition(D3DXVECTOR3(450,0,0));


	//�浹�ڽ�
	int iObjectNum = 2;
	CCubeMesh *pBox[2];
	CGameObject *pBoundBox[2];
	pBox[0] = new CCubeMesh(pd3dDevice,pHero->GetBoundSizeX(),pHero->GetBoundSizeY(),pHero->GetBoundSizeZ());
	pBox[1] = new CCubeMesh(pd3dDevice,20,20,20);
	
	for(int i=0; i<iObjectNum; i++)
	{
		pBoundBox[i] = new CGameObject;
		pBoundBox[i]->SetMesh(pBox[i]);
	}
	pBoundBox[1]->SetPosition(D3DXVECTOR3(25,0,0));


	//pFBXMesh->Release();

 	//�ﰢ�� ��ü�� ���̴� ��ü�� �����Ѵ�.
 	m_ppShaders[0]->AddObject(pHero);
	m_ppShaders[0]->AddObject(pPlane);
	m_ppShaders[0]->AddObject(pTower);

	for(int i=0; i<iObjectNum; i++)
	{
		m_ppShaders[0]->AddObject(pBoundBox[i]);
	}

	for(int i=0; i<m_nIntanceObjects; i++)
		m_ppObjects[i] = m_ppShaders[1]->GetObject(i);

	m_ppObjects[m_nIntanceObjects] = pHero;
	m_ppObjects[m_nIntanceObjects + 1] = pPlane;
	m_ppObjects[m_nIntanceObjects + 1]->SetTag(PLANE);
	m_ppObjects[m_nIntanceObjects + 2] = NULL;
	m_ppObjects[m_nIntanceObjects + 3] = pBoundBox[0];
	m_ppObjects[m_nIntanceObjects + 3]->SetTag(HERO_BOUND);
	m_ppObjects[m_nIntanceObjects + 4] = pBoundBox[1];
	m_ppObjects[m_nIntanceObjects + 4]->SetTag(OBSTACLE_BOUND);

	m_ppObjects[m_nIntanceObjects + 5] = pTower;
	//m_ppObjects[m_nIntanceObjects + 5]->SetTag(TOWER);
// 
// 	for(int i=6; i<56; i++)
// 		m_ppObjects[i] = pMinion[i-5];

	for(int i= m_nIntanceObjects + 6; i<m_nObjects; i++)
		m_ppObjects[i] = NULL;

	this->AddOtherPlayer(pd3dDevice);

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

	for (int j = 0; j < m_nObjects; j++)
	{
		if(m_ppObjects[j] == NULL) continue;

		m_ppObjects[j]->Animate(fTimeElapsed);
		m_ppObjects[j]->Update(fTimeElapsed);

		if(m_ppObjects[j]->GetTag() == TOWER)
		{
			for(int i = 0; i<m_nObjects; i++)
			{
				if(m_ppObjects[i] == NULL) continue;
				if(m_ppObjects[i]->GetTag() == HERO)
				{
					float distance = ST::sharedManager()->GetDistance(m_ppObjects[j]->GetPos(), m_ppObjects[i]->GetPos());
					
					if(distance < 50.0f)
						m_ppObjects[j]->SetPos(m_ppObjects[i]->GetPos());
					//	TowerObject* tt = m_ppObjects[j];

				}
			}
		}

		if(m_ppObjects[j]->GetTag() == HERO_BOUND) //�÷��̾� �浹�ڽ� ���̱�
			m_ppObjects[j]->SetPosition(m_ppObjects[0]->GetPosition());

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
	}

	//GameCollision();

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

void CScene::AddOtherPlayer(ID3D11Device *pd3dDevice)
{
	int CheckCount = 0;
	for(int i=0; i<m_nObjects; i++)
	{
		if ( m_ppObjects[i] != NULL) continue;
		CGameObject* OtherPlayer = new CGameObject();
		OtherPlayer->SetMesh(pHeroMesh);
		m_ppShaders[0]->AddObject(OtherPlayer);  //���ý� �迭 ���� ����
		m_ppObjects[i] = OtherPlayer;  //���ý� �迭 ���� ����
		m_ppObjects[i]->SetTag(OTHERPLAYER);
		m_ppObjects[i]->SetVisible(FALSE);
		CheckCount++;
		if(CheckCount > 9) break;
	}
}

void CScene::SetOtherClient(PlayerStruct* _PI, int _Count)
{
	_Count -= 1;

	for(int i=0; i<_Count; i++)
	{
		if(_PI[i].Use == TRUE) continue;
		if(_PI[i].PI.m_ID == 0) continue;
		for(int j=0; j<m_nObjects; j++)
		{
			if(m_ppObjects[j] == NULL) continue;
			if(m_ppObjects[j]->GetTag() == HERO) continue;
			if(m_ppObjects[j]->GetTag() != OTHERPLAYER || m_ppObjects[j]->GetID() != 0) continue;

			m_ppObjects[j]->SetID(_PI[i].PI.m_ID);
			m_ppObjects[j]->SetPos(_PI[i].PI.m_Pos);
			m_ppObjects[j]->SetRot(_PI[i].PI.m_Rot);
			m_ppObjects[j]->SetScale(_PI[i].PI.m_Scale);
			m_ppObjects[j]->SetVisible(TRUE);
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
		for(int j=0; j<m_nObjects; j++)
		{
			if(m_ppObjects[j] == NULL) continue;
			if(m_ppObjects[j]->GetTag() != OTHERPLAYER || m_ppObjects[j]->GetID() != _PI[i].PI.m_ID) continue;
			m_ppObjects[j]->SetPos(_PI[i].PI.m_Pos);
			m_ppObjects[j]->SetRot(_PI[i].PI.m_Rot);
			m_ppObjects[j]->SetScale(_PI[i].PI.m_Scale);
			break;
		}
	}
}

void CScene::AddMinion(ID3D11Device *pd3dDevice)
{

	for(int i=0; i<m_nObjects; i++)
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
	}
}

CGameObject* CScene::GetObject(int num)
{
	return m_ppObjects[num];
}

//�ڽ���ڽ� �浹üũ
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

			//ĳ���� ���ʹ���
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

			//ĳ���� ������ ����
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
			//ĳ���� ���� ����
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
	
			//ĳ���� �Ʒ��� ����
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