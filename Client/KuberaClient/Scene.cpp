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
	m_bLbutton = FALSE;
	m_bJoinOtherPlayer = FALSE;
	m_bJoin = FALSE;

	m_fMinionRespawnTime = 0.f;
	m_fMissileAttackTime = 0.f;
	m_nMinionObjects = 0;

	m_pHero = NULL;
	m_pPlane = NULL;

	//m_pTxtHelper = NULL;
}


CScene::~CScene(void)
{

}

void CScene::BuildObjects(ID3D11Device *pd3dDevice)
{
	m_Control.m_Camera = m_Camera;
	//�� ���̴� ��ü�� ���� �����͵��� �迭�� �����Ѵ�.

	//CObjectShader Ŭ���� ��ü�� �����Ѵ�.
	m_pObjectShaders = new CObjectShader();
	m_pObjectShaders->CreateShader(pd3dDevice, 100);

	m_pInstancingShaders = new CInstancingShader();
	m_pInstancingShaders->CreateShader(pd3dDevice, 10);
	m_pInstancingShaders->BuildObjects(pd3dDevice);
		
	//���� ��ü�� ���� �����͵��� �迭�� �����Ѵ�.
	m_nIntanceObjects = m_pInstancingShaders->GetObjectsNumber();
	m_nObjects = 20 + m_nIntanceObjects;

	//������ü �޽��� �����ϰ� ��ü�� �����Ѵ�.
	//CCubeMesh *pMesh = new CCubeMesh(pd3dDevice, 15.0f, 15.0f, 15.0f);
	pHeroMesh = new CFBXMesh(pd3dDevice, L"Wizard101310.FBX");
	pHeroMesh->LoadTexture(pd3dDevice, L"micro_wizard_col.tif");

	CFBXMesh *pPlaneMesh = new CFBXMesh(pd3dDevice, L"Plane4.FBX");
	pPlaneMesh->LoadTexture(pd3dDevice, L"floor.png");

	CFBXMesh *pObstacleMesh = new CFBXMesh(pd3dDevice, L"tower/Tower1_303030.FBX");
	pObstacleMesh->LoadTexture(pd3dDevice, L"micro_wizard_col.tif");

	pMinionDragonMesh = new CFBXMesh(pd3dDevice, L"minion/Dragon7107.FBX");
	pMinionDragonMesh->LoadTexture(pd3dDevice, L"minion/micro_dragon_col.tif");

	//�ﰢ�� ��ü(CTriangleObject)�� �����ϰ� �ﰢ�� �޽��� �����Ѵ�.
	m_pHero = new HeroObject;
	m_pHero->SetMesh(pHeroMesh);
	m_Control.m_Player = m_pHero;
	m_pHero->SetPosition(D3DXVECTOR3(550, 0, 0));
	m_pHero->SetBoundSize(10, 13, 10);

	for(int i=0; i<MAX_MINION; i++)
	{
		m_pMinion1[i] = new MinionObject();
		m_pMinion1[i]->SetMesh(pMinionDragonMesh);
		m_pMinion1[i]->SetPosition(D3DXVECTOR3(485,0,20));
	}
	for(int i=0; i<MAX_MINION; i++)
	{
		m_pMinion2[i] = new MinionObject();
		m_pMinion2[i]->SetMesh(pMinionDragonMesh);
		m_pMinion2[i]->SetPosition(D3DXVECTOR3(-485,0,20));
	}
	for(int i=0; i<MAX_MINION; i++)
	{
		m_pMinion3[i] = new MinionObject();
		m_pMinion3[i]->SetMesh(pMinionDragonMesh);
		m_pMinion3[i]->SetPosition(D3DXVECTOR3(485,0,-20));
	}
	for(int i=0; i<MAX_MINION; i++)
	{
		m_pMinion4[i] = new MinionObject();
		m_pMinion4[i]->SetMesh(pMinionDragonMesh);
		m_pMinion4[i]->SetPosition(D3DXVECTOR3(-485,0,-20));
	}

	
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


	//�浹�ڽ�
	int iObjectNum = 2;
	CCubeMesh *pBox[2];
	pBoundBox[2];
	pBox[0] = new CCubeMesh(pd3dDevice,m_pHero->GetBoundSizeX(),m_pHero->GetBoundSizeY(),m_pHero->GetBoundSizeZ());
	pBox[1] = new CCubeMesh(pd3dDevice,20,20,20);
	
	for(int i=0; i<iObjectNum; i++)
	{
		pBoundBox[i] = new CGameObject;
		pBoundBox[i]->SetMesh(pBox[i]);
	}
	pBoundBox[1]->SetPosition(D3DXVECTOR3(25,0,0));


	//pFBXMesh->Release();

 	//�ﰢ�� ��ü�� ���̴� ��ü�� �����Ѵ�.
 	m_pObjectShaders->AddObject(m_pHero);
	m_pObjectShaders->AddObject(m_pPlane);

	for(int i=0; i<iObjectNum; i++)
		m_pObjectShaders->AddObject(pBoundBox[i]);

	for(int i=0; i<10;i++)
	{
		m_pOtherPlayer[i] = new EnemyObject;
		m_pOtherPlayer[i]->SetMesh(pHeroMesh);
		m_pObjectShaders->AddObject(m_pOtherPlayer[i]);
	}


// 	for(int i=6; i<56; i++)
// 		m_ppObjects[i] = pMinion[i-5];


	this->AddOtherPlayer(pd3dDevice);

}

void CScene::ReleaseObjects()
{
	//���̴� ��ü ����Ʈ�� �� ��ü�� �Ҹ��Ű�� ����Ʈ�� �Ҹ��Ų��.

	if(m_pObjectShaders) delete m_pObjectShaders;
	if(m_pInstancingShaders) delete m_pInstancingShaders;

	//���� ��ü ����Ʈ�� �� ��ü�� ��ȯ(Release)�ϰ� ����Ʈ�� �Ҹ��Ų��.

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
		/*�������� ũ�Ⱑ ����� ��(����� ��Alt+Enter�� ��ü ȭ�� ���� ������ ���� ��ȯ�� ��) ���� ü���� �ĸ���� ũ�⸦ �����ϰ� �ĸ���ۿ� ���� ���� Ÿ�� �並 �ٽ� �����Ѵ�. */
	case WM_SIZE:
		{
			break;
		}
	case WM_LBUTTONDOWN:
		m_MousePosX = (INT)LOWORD(lParam); 
		m_MousePosY = (INT)HIWORD(lParam);
		m_Control.TouchLeftDown((float)m_MousePosX, (float)m_MousePosY, hWnd);
		m_bLbutton = TRUE;
		break;
	case WM_RBUTTONDOWN:
		m_MousePosX = (INT)LOWORD(lParam); 
		m_MousePosY = (INT)HIWORD(lParam);
		m_Control.TouchRightDown((float)m_MousePosX, (float)m_MousePosY, hWnd);
		m_bRbutton = TRUE;
		break;
	case WM_RBUTTONUP:
		m_bRbutton = FALSE;
		
		break;
	case WM_LBUTTONUP:
		m_bLbutton = FALSE;
		break;
	case WM_MOUSEMOVE:
		m_MousePosX = (INT)LOWORD(lParam); 
		m_MousePosY = (INT)HIWORD(lParam);

		if(m_bRbutton == TRUE)
			m_Control.TouchRightDown((float)m_MousePosX, (float)m_MousePosY, hWnd);
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

	for(int i=0; i<MAX_MINION; i++)
	{
		m_pMinion1[i]->Update(fTimeElapsed);
		m_pMinion2[i]->Update(fTimeElapsed);
		m_pMinion3[i]->Update(fTimeElapsed);
		m_pMinion4[i]->Update(fTimeElapsed);

		m_pMinion1[i]->Animate(fTimeElapsed);
		m_pMinion2[i]->Animate(fTimeElapsed);
		m_pMinion3[i]->Animate(fTimeElapsed);
		m_pMinion4[i]->Animate(fTimeElapsed);
	}

	for(int i=0; i<MAX_MISSILE; i++)
		MissileManager::sharedManager()->m_pMissile[i]->Update(fTimeElapsed);



	for(int i=0;i<MAX_TOWER; i++)
	{
		TowerManager::sharedManager()->m_pTower[i]->Update(fTimeElapsed);

		distance = ST::sharedManager()->GetDistance(m_pHero->GetPos(), TowerManager::sharedManager()->m_pTower[i]->GetPos());
		if(distance < 50.0f)
		{
			TowerManager::sharedManager()->m_pTower[i]->SetTarget(m_pHero);
		}
		else
		{
			TowerManager::sharedManager()->m_pTower[i]->SetTarget(NULL);
			TowerManager::sharedManager()->m_pTower[i]->SetAttackTime(0.f);
		}
	}
	
	//�÷��̾� �浹�ڽ� ���̱�
	pBoundBox[0]->SetPosition(m_pHero->GetPosition());

	//m_pHero->SetAstar(TRUE);
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

	for(int i=0; i<MAX_OBSTACLE; i++) //ĳ���Ϳ� ��ֹ� �浹üũ
	{
		if(ObstacleManager::sharedManager()->m_pObstacle[i] == NULL) continue;
		GameCollision(m_pHero, ObstacleManager::sharedManager()->m_pObstacle[i]);
	}

	for(int i=0; i<MAX_TOWER; i++) //ĳ���Ϳ� Ÿ�� �浹üũ
	{
		if(TowerManager::sharedManager()->m_pTower[i] == NULL) continue;
		GameCollision(m_pHero, TowerManager::sharedManager()->m_pTower[i]);
	}
}

void CScene::Render(ID3D11DeviceContext*pd3dDeviceContext)
{
	//���̴� ��ü ����Ʈ�� �� ���̴� ��ü�� �������Ѵ�.
	
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

	m_pObjectShaders->UpdateShaderVariables(pd3dDeviceContext, &pBoundBox[0]->m_d3dxmtxWorld);
	pBoundBox[0]->Render(pd3dDeviceContext);

	for(int i=0; i<MAX_MINION; i++)
	{
		m_pObjectShaders->UpdateShaderVariables(pd3dDeviceContext, &m_pMinion1[i]->m_d3dxmtxWorld);
		m_pMinion1[i]->Render(pd3dDeviceContext);
		m_pObjectShaders->UpdateShaderVariables(pd3dDeviceContext, &m_pMinion2[i]->m_d3dxmtxWorld);
		m_pMinion2[i]->Render(pd3dDeviceContext);
		m_pObjectShaders->UpdateShaderVariables(pd3dDeviceContext, &m_pMinion3[i]->m_d3dxmtxWorld);
		m_pMinion3[i]->Render(pd3dDeviceContext);
		m_pObjectShaders->UpdateShaderVariables(pd3dDeviceContext, &m_pMinion4[i]->m_d3dxmtxWorld);
		m_pMinion4[i]->Render(pd3dDeviceContext);
	}

	m_pInstancingShaders->Render(pd3dDeviceContext);

	for(int i=0; i<MAX_MISSILE; i++)
		MissileManager::sharedManager()->m_pMissile[i]->Render(pd3dDeviceContext);
	for(int i=0; i<MAX_TOWER; i++)
		TowerManager::sharedManager()->m_pTower[i]->Render(pd3dDeviceContext);
	for(int i=0; i<872; i++)
		ObstacleManager::sharedManager()->m_pObstacle[i]->Render(pd3dDeviceContext);
	/*for(int i=0; i<MAX_MINION; i++)
	{
		MinionManager::sharedManager()->m_pMinion1[i]->Render(pd3dDeviceContext);
		MinionManager::sharedManager()->m_pMinion2[i]->Render(pd3dDeviceContext);
		MinionManager::sharedManager()->m_pMinion3[i]->Render(pd3dDeviceContext);
		MinionManager::sharedManager()->m_pMinion4[i]->Render(pd3dDeviceContext);
	}*/

	//DXUT_BeginPerfEvent( DXUT_PERFEVENTCOLOR, L"HUD / Stats" );
	//RenderText();
	//DXUT_EndPerfEvent();
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
	//	m_ppShaders[0]->AddObject(OtherPlayer);  //���ý� �迭 ���� ����
	//	m_ppObjects[i] = OtherPlayer;  //���ý� �迭 ���� ����
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

void CScene::GameCollision(CGameObject* obj1, CGameObject* obj2)
{

	if(CheckCollision(obj1->GetPosition(), obj1->GetBoundSizeX(), obj1->GetBoundSizeY(), 
		obj1->GetBoundSizeZ(), obj2->GetPosition(),obj2->GetBoundSizeX(), obj2->GetBoundSizeY(), 
		obj2->GetBoundSizeZ()))
	{

		//ĳ���� ���ʹ���
		if(obj1->GetPosition().x > obj2->GetPosition().x && 
			abs(obj2->GetPosition().x - obj1->GetPosition().x) > 
			abs(obj2->GetPosition().z - obj1->GetPosition().z))
		{
			if(obj1->GetPosition().x - obj1->GetBoundSizeX()/2 < 
				obj2->GetPosition().x + obj2->GetBoundSizeX()/2)
			{
				D3DXVECTOR3 v = D3DXVECTOR3(obj2->GetPosition().x + obj2->GetBoundSizeX()/2 +
					obj1->GetBoundSizeX()/2, obj1->GetPosition().y, obj1->GetPosition().z);
				obj1->SetPosition(v);
			}
		}

		//ĳ���� ������ ����
		else if(obj1->GetPosition().x < obj2->GetPosition().x &&
			abs(obj2->GetPosition().x - obj1->GetPosition().x) > 
			abs(obj2->GetPosition().z - obj1->GetPosition().z))
		{
			if(obj1->GetPosition().x + obj1->GetBoundSizeX()/2 > 
				obj2->GetPosition().x - obj2->GetBoundSizeX()/2)
			{
				D3DXVECTOR3 v = D3DXVECTOR3(obj2->GetPosition().x - obj2->GetBoundSizeX()/2 - 
					obj1->GetBoundSizeX()/2, obj1->GetPosition().y, obj1->GetPosition().z);
				obj1->SetPosition(v);
			}
		}
		//ĳ���� ���� ����
		else if(obj1->GetPosition().z < obj2->GetPosition().z &&
			abs(obj2->GetPosition().x - obj1->GetPosition().x) < 
			abs(obj2->GetPosition().z - obj1->GetPosition().z))
		{
			if(obj1->GetPosition().z + obj1->GetBoundSizeZ()/2 > 
				obj2->GetPosition().z - obj2->GetBoundSizeZ()/2)
			{
				D3DXVECTOR3 v = D3DXVECTOR3(obj1->GetPosition().x, obj1->GetPosition().y,
					obj2->GetPosition().z - obj2->GetBoundSizeZ()/2 - obj1->GetBoundSizeZ()/2);
				obj1->SetPosition(v);
			}
		}

		//ĳ���� �Ʒ��� ����
		else if(obj1->GetPosition().z > obj2->GetPosition().z &&
			abs(obj2->GetPosition().x - obj1->GetPosition().x) < 
			abs(obj2->GetPosition().z - obj1->GetPosition().z))
		{
			if(obj1->GetPosition().z + obj1->GetBoundSizeZ()/2 > 
				obj2->GetPosition().z - obj2->GetBoundSizeZ()/2)
			{
				D3DXVECTOR3 v = D3DXVECTOR3(obj1->GetPosition().x, obj1->GetPosition().y, 
					obj2->GetPosition().z + obj2->GetBoundSizeZ()/2 + obj1->GetBoundSizeZ()/2);
				obj1->SetPosition(v);
			}
		}
	}
	
	
}

void CScene::SetMinionInfo(MinionInfo* _MI1, MinionInfo* _MI2, MinionInfo* _MI3, MinionInfo* _MI4)
{
	D3DXVECTOR3 des;
	ZeroMemory(&des, sizeof(D3DXVECTOR3));
	for(int i=0; i<MAX_MINION; i++)
	{
		des.x = _MI1[i].m_Pos.x;
		des.y= _MI1[i].m_Pos.y;
		des.z = _MI1[i].m_Pos.z;

		m_pMinion1[i]->SetNewDestination(des);
		m_pMinion1[i]->SetVisible(_MI1[i].m_Live);

 		des.x = _MI2[i].m_Pos.x;
 		des.y= _MI2[i].m_Pos.y;
 		des.z = _MI2[i].m_Pos.z;
 		m_pMinion2[i]->SetNewDestination(des);
 		m_pMinion2[i]->SetVisible(_MI2[i].m_Live);
 
 		des.x = _MI3[i].m_Pos.x;
 		des.y= _MI3[i].m_Pos.y;
 		des.z = _MI3[i].m_Pos.z;
 		m_pMinion3[i]->SetNewDestination(des);
 		m_pMinion3[i]->SetVisible(_MI3[i].m_Live);
 
 		des.x = _MI4[i].m_Pos.x;
 		des.y= _MI4[i].m_Pos.y;
 		des.z = _MI4[i].m_Pos.z;
 		m_pMinion4[i]->SetNewDestination(des);
 		m_pMinion4[i]->SetVisible(_MI4[i].m_Live);

	}
	printf("%.2f, %.2f, %.2f \n", m_pMinion1[1]->GetPos().x, m_pMinion1[1]->GetPos().y, m_pMinion1[1]->GetPos().z);
	printf("%.2f, %.2f, %.2f \n", m_pMinion4[1]->GetPos().x, m_pMinion4[1]->GetPos().y, m_pMinion4[1]->GetPos().z);
}

//void CScene::RenderText()
//{
//	m_pTxtHelper->Begin();
//	m_pTxtHelper->SetInsertionPos(5, 50);
//	m_pTxtHelper->SetForegroundColor( D3DXCOLOR( 0.0f, 0.0f, 0.0f, 1.0f ) );
//	m_pTxtHelper->DrawTextLine( DXUTGetFrameStats( DXUTIsVsyncEnabled() ) );
//	m_pTxtHelper->DrawTextLine( DXUTGetDeviceStats() );
//	m_pTxtHelper->DrawTextLine(L"Target : ");
//
//	m_pTxtHelper->End();
//}

