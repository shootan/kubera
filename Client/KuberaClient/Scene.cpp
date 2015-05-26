#include "Scene.h"

CScene::CScene(void)
{
	m_pObjectShaders = NULL;
	m_pInstancingShaders = NULL;
	m_pAnimationShaders = NULL;
	m_nShaders = 0;
     
	m_nObjects = 0;
	m_nIntanceObjects = 0;
	m_nAnimationObjects = 0;

	m_MousePosX = 0;
	m_MousePosY = 0;

	m_bRbutton = FALSE;
	m_bLbutton = FALSE;
	m_bJoinOtherPlayer = FALSE;
	m_bJoin = FALSE;

	m_fMinionRespawnTime = 0.f;
	m_fMissileAttackTime = 0.f;
	m_nMinionObjects = 0;

	m_pPlane = NULL;
	m_pBlueNexus = NULL;
	m_pRedNexus = NULL;
	m_pHeroMesh = NULL;

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

	m_pAnimationShaders = new CAnimationShader();
	m_pAnimationShaders->CreateShader(pd3dDevice, 15);
		
	//게임 객체에 대한 포인터들의 배열을 정의한다.
	m_nIntanceObjects = m_pInstancingShaders->GetObjectsNumber();
	m_nObjects = 20 + m_nIntanceObjects;

	//정육면체 메쉬를 생성하고 객체에 연결한다.
	//CCubeMesh *pMesh = new CCubeMesh(pd3dDevice, 15.0f, 15.0f, 15.0f);
	//pHeroMesh = new CFBXMesh(pd3dDevice, L"Hero/Wizard101310.FBX");
	m_pHeroMesh = new GFBX::Mesh();
	
	GFBXMeshLoader::getInstance()->LoadFBXMesh(m_pHeroMesh, L"Hero/golem.fbx", pd3dDevice);
	m_pHeroMesh->OnCreateDevice(pd3dDevice);
	for(int i=0; i<m_pHeroMesh->GetSubsetCount(); i++)
		m_pHeroMesh->GetSubset(i)->LoadTexture(pd3dDevice, L"Hero/golem.png");
		

	/*
	GFBXMeshLoader::getInstance()->LoadFBXMesh(m_pHeroMesh, L"Hero/Wizard101310.fbx", pd3dDevice);
	m_pHeroMesh->OnCreateDevice(pd3dDevice);
	for(int i=0; i<m_pHeroMesh->GetSubsetCount(); i++)
		m_pHeroMesh->GetSubset(i)->LoadTexture(pd3dDevice, L"Hero/micro_wizard_col.tif");
		*/

	CFBXMesh *pPlaneMesh = new CFBXMesh(pd3dDevice, L"imagefile/Plane4.FBX", 10);
	pPlaneMesh->LoadTexture(pd3dDevice, L"imagefile/12.png");

	CFBXMesh *pObstacleMesh = new CFBXMesh(pd3dDevice, L"tower/Tower1_303030.FBX");
	pObstacleMesh->LoadTexture(pd3dDevice, L"Hero/micro_wizard_col.tif");

	CFBXMesh *pBlueNexusMesh = new CFBXMesh(pd3dDevice, L"tower/Nexus.FBX");
	pBlueNexusMesh->LoadTexture(pd3dDevice, L"tower/Nexus.png");

	CFBXMesh *pRedNexusMesh = new CFBXMesh(pd3dDevice, L"tower/Nexus.FBX");
	pRedNexusMesh->LoadTexture(pd3dDevice, L"tower/Nexus2.png");

	//히어로 생성
	HeroManager::sharedManager()->CreateHero(m_pHeroMesh, 10, 13, 10);
	Vector3 s;
	s.x = 0.3;
	s.y = 0.3;
	s.z = 0.3;
	HeroManager::sharedManager()->m_pHero->SetScale(s);

	m_pPlane = new CGameObject();
	m_pPlane->SetMesh(pPlaneMesh);

	//충돌박스
	int iObjectNum = 2;
	CCubeMesh *pBox[2];
	pBoundBox[2];
	pBox[0] = new CCubeMesh(pd3dDevice,HeroManager::sharedManager()->m_pHero->GetBoundSizeX(),
		HeroManager::sharedManager()->m_pHero->GetBoundSizeY(),HeroManager::sharedManager()->m_pHero->GetBoundSizeZ());
	pBox[1] = new CCubeMesh(pd3dDevice,20,20,20);
	
	for(int i=0; i<iObjectNum; i++)
	{
		pBoundBox[i] = new CGameObject;
		pBoundBox[i]->SetMesh(pBox[i]);
	}
	pBoundBox[1]->SetPosition(D3DXVECTOR3(25,0,0));

	m_pBlueNexus = new CGameObject();
	m_pBlueNexus->SetMesh(pBlueNexusMesh);
	m_pBlueNexus->SetPosition(D3DXVECTOR3(-550, 0, 0));
	m_pBlueNexus->SetBoundSize(50, 20, 50);
	m_pBlueNexus->SetHP(2000);
	m_pBlueNexus->SetID(175);

	m_pRedNexus = new CGameObject();
	m_pRedNexus->SetMesh(pRedNexusMesh);
	m_pRedNexus->SetPosition(D3DXVECTOR3(550, 0, 0));
	m_pRedNexus->SetBoundSize(50, 50, 50);
	m_pRedNexus->SetHP(2000);
	m_pRedNexus->SetID(176);

	HeroManager::sharedManager()->SetNexus(m_pRedNexus, m_pBlueNexus);


	//pFBXMesh->Release();

 	//삼각형 객체를 쉐이더 객체에 연결한다.
 	m_pAnimationShaders->AddObject(HeroManager::sharedManager()->m_pHero);
	m_pObjectShaders->AddObject(m_pPlane);
	for(int i=0; i<iObjectNum; i++)
		m_pObjectShaders->AddObject(pBoundBox[i]);
	m_pObjectShaders->AddObject(m_pBlueNexus);
	m_pObjectShaders->AddObject(m_pRedNexus);

	OtherPlayerManager::sharedManager()->CreateOtherPlayer(D3DXVECTOR3(1500, 0, 0), m_pHeroMesh, 10, 13, 10);
	for(int i=0; i<MAX_OTHER_PLAYER;i++)
		m_pAnimationShaders->AddObject(OtherPlayerManager::sharedManager()->m_pOtherPlayer[i]);
	
	this->AddOtherPlayer(pd3dDevice);

}

void CScene::ReleaseObjects()
{
	//쉐이더 객체 리스트의 각 객체를 소멸시키고 리스트를 소멸시킨다.

	if(m_pObjectShaders) delete m_pObjectShaders;
	if(m_pInstancingShaders) delete m_pInstancingShaders;
	if(m_pAnimationShaders) delete m_pAnimationShaders;
	//게임 객체 리스트의 각 객체를 반환(Release)하고 리스트를 소멸시킨다.

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
	HeroManager::sharedManager()->m_pHero->Animate(fTimeElapsed);
	HeroManager::sharedManager()->m_pHero->Update(fTimeElapsed);
 
   	for(int i=0; i<MAX_MINION; i++)
   	{
  		MinionManager::sharedManager()->m_pMinion1[i]->Update(fTimeElapsed);
  		MinionManager::sharedManager()->m_pMinion1[i]->Animate(fTimeElapsed);
   	}

	for(int i=0; i<MAX_MISSILE; i++)
		MissileManager::sharedManager()->m_pMissile[i]->Update(fTimeElapsed);



	for(int i=0;i<MAX_TOWER; i++)  //타워의 캐릭터 공격
	{
		//if(TowerManager::sharedManager()->m_pTower[i]->GetTarget() != NULL) continue;

		TowerManager::sharedManager()->m_pTower[i]->Update(fTimeElapsed);

		m_DistanceToHero = ST::sharedManager()->GetDistance(HeroManager::sharedManager()->m_pHero->GetPos(), 
			TowerManager::sharedManager()->m_pTower[i]->GetPos());
		if(m_DistanceToHero < 50.0f 
			&& TowerManager::sharedManager()->m_pTower[i]->GetTeam() != HeroManager::sharedManager()->m_pHero->GetTeam() && HeroManager::sharedManager()->m_pHero->GetHP() > 1.0f)
		{
			TowerManager::sharedManager()->m_pTower[i]->SetTarget(HeroManager::sharedManager()->m_pHero);
			continue;
		}
		else
		{
			//TowerManager::sharedManager()->m_pTower[i]->SetTarget(NULL);
			//TowerManager::sharedManager()->m_pTower[i]->SetAttackTime(0.f);
		}

		for(int j=0; j<MAX_OTHER_PLAYER; j++)
		{
			if(OtherPlayerManager::sharedManager()->m_pOtherPlayer[j]->GetVisible() != TRUE) continue;
			
			m_DistanceToOtherPlayer = ST::sharedManager()->GetDistance(OtherPlayerManager::sharedManager()->m_pOtherPlayer[j]->GetPos(), 
				TowerManager::sharedManager()->m_pTower[i]->GetPos()); 
			if(m_DistanceToOtherPlayer < 50.f 
				&& TowerManager::sharedManager()->m_pTower[i]->GetTeam() != OtherPlayerManager::sharedManager()->m_pOtherPlayer[j]->GetTeam())
			{
				TowerManager::sharedManager()->m_pTower[i]->SetTarget(OtherPlayerManager::sharedManager()->m_pOtherPlayer[j]);
			}
			else
			{
				TowerManager::sharedManager()->m_pTower[i]->SetTarget(NULL);
				TowerManager::sharedManager()->m_pTower[i]->SetAttackTime(0.f);
			}
			
		}
	}

	for(int i=0; i<MAX_OTHER_PLAYER; i++)
	{
		if(OtherPlayerManager::sharedManager()->m_pOtherPlayer[i]->GetVisible() != TRUE) continue;

		OtherPlayerManager::sharedManager()->m_pOtherPlayer[i]->Update(fTimeElapsed);
		OtherPlayerManager::sharedManager()->m_pOtherPlayer[i]->Animate(fTimeElapsed);
	}
	
	//플레이어 충돌박스 보이기
	pBoundBox[0]->SetPosition(HeroManager::sharedManager()->m_pHero->GetPosition());

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

	for(int i=0; i<MAX_OBSTACLE; i++) //캐릭터와 장애물 충돌체크
	{
		if(ObstacleManager::sharedManager()->m_pObstacle[i] == NULL) continue;
		GameCollision(HeroManager::sharedManager()->m_pHero, ObstacleManager::sharedManager()->m_pObstacle[i]);
	}

	for(int i=0; i<MAX_DESTROY_TOWER; i++) //캐릭터와 부서진 타워 충돌체크
	{
		if(ObstacleManager::sharedManager()->m_pDestroyTower[i] == NULL) continue;
		GameCollision(HeroManager::sharedManager()->m_pHero, ObstacleManager::sharedManager()->m_pDestroyTower[i]);
	}

	for(int i=0; i<MAX_TOWER; i++) //캐릭터와 타워 충돌체크
	{
		if(TowerManager::sharedManager()->m_pTower[i] == NULL) continue;
		GameCollision(HeroManager::sharedManager()->m_pHero, TowerManager::sharedManager()->m_pTower[i]);
	}
	GameCollision(HeroManager::sharedManager()->m_pHero, m_pBlueNexus); //캐릭터와 넥서스 충돌체크
	GameCollision(HeroManager::sharedManager()->m_pHero, m_pRedNexus);
}

void CScene::Render(ID3D11DeviceContext*pd3dDeviceContext)
{
	//쉐이더 객체 리스트의 각 쉐이더 객체를 렌더링한다.
	
	m_pObjectShaders->Render(pd3dDeviceContext);

	
	//m_pObjectShaders->UpdateShaderVariables(pd3dDeviceContext, &HeroManager::sharedManager()->m_pHero->m_d3dxmtxWorld);
	//HeroManager::sharedManager()->m_pHero->Render(pd3dDeviceContext);
	
	/*for(int i=0; i<MAX_OTHER_PLAYER; i++)
	{
		if(OtherPlayerManager::sharedManager()->m_pOtherPlayer[i]->GetVisible() != TRUE) continue;

		m_pObjectShaders->UpdateShaderVariables(pd3dDeviceContext, &OtherPlayerManager::sharedManager()->m_pOtherPlayer[i]->m_d3dxmtxWorld);
		OtherPlayerManager::sharedManager()->m_pOtherPlayer[i]->Render(pd3dDeviceContext);
	}*/
	m_pObjectShaders->UpdateShaderVariables(pd3dDeviceContext, &m_pPlane->m_d3dxmtxWorld);
	m_pPlane->Render(pd3dDeviceContext);

	//m_pObjectShaders->UpdateShaderVariables(pd3dDeviceContext, &pBoundBox[0]->m_d3dxmtxWorld);
	//pBoundBox[0]->Render(pd3dDeviceContext);
	m_pObjectShaders->UpdateShaderVariables(pd3dDeviceContext, &m_pBlueNexus->m_d3dxmtxWorld);
	m_pBlueNexus->Render(pd3dDeviceContext);
	m_pObjectShaders->UpdateShaderVariables(pd3dDeviceContext, &m_pRedNexus->m_d3dxmtxWorld);
	m_pRedNexus->Render(pd3dDeviceContext);


	m_pInstancingShaders->Render(pd3dDeviceContext);

	for(int i=0; i<MAX_MISSILE; i++)
		MissileManager::sharedManager()->m_pMissile[i]->Render(pd3dDeviceContext);
	for(int i=0; i<MAX_TOWER; i++)
		TowerManager::sharedManager()->m_pTower[i]->Render(pd3dDeviceContext);
	for(int i=0; i<872; i++)
		ObstacleManager::sharedManager()->m_pObstacle[i]->Render(pd3dDeviceContext);
  	for(int i=0; i<MAX_MINION; i++)
  		MinionManager::sharedManager()->m_pMinion1[i]->Render(pd3dDeviceContext);
	for(int i=0; i<MAX_DESTROY_TOWER; i++)
		ObstacleManager::sharedManager()->m_pDestroyTower[i]->Render(pd3dDeviceContext);
 
	m_pAnimationShaders->Render(pd3dDeviceContext);

	m_pAnimationShaders->UpdateShaderVariables(pd3dDeviceContext, &HeroManager::sharedManager()->m_pHero->m_d3dxmtxWorld);
	HeroManager::sharedManager()->m_pHero->Render(pd3dDeviceContext);

	for(int i=0; i<MAX_OTHER_PLAYER; i++)
	{
		if(OtherPlayerManager::sharedManager()->m_pOtherPlayer[i]->GetVisible() != TRUE) continue;

		m_pAnimationShaders->UpdateShaderVariables(pd3dDeviceContext, &OtherPlayerManager::sharedManager()->m_pOtherPlayer[i]->m_d3dxmtxWorld);
		OtherPlayerManager::sharedManager()->m_pOtherPlayer[i]->Render(pd3dDeviceContext);
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
		for(int j=0; j<MAX_OTHER_PLAYER; j++)
		{
			if(OtherPlayerManager::sharedManager()->m_pOtherPlayer[j]->GetID() != 0) continue;
			OtherPlayerManager::sharedManager()->m_pOtherPlayer[j]->SetID(_PI[i].PI.m_ID);
			OtherPlayerManager::sharedManager()->m_pOtherPlayer[j]->SetPos(_PI[i].PI.m_Pos);
			OtherPlayerManager::sharedManager()->m_pOtherPlayer[j]->SetRot(_PI[i].PI.m_Rot);
			OtherPlayerManager::sharedManager()->m_pOtherPlayer[j]->SetState(_PI[i].PI.m_iState);
			OtherPlayerManager::sharedManager()->m_pOtherPlayer[j]->SetTargetID(_PI[i].PI.m_iTargetID);
			OtherPlayerManager::sharedManager()->m_pOtherPlayer[j]->SetHP(_PI[i].PI.m_HP);
			OtherPlayerManager::sharedManager()->m_pOtherPlayer[j]->SetDamage(_PI[i].PI.m_Damage);
			if(_PI[i].PI.m_ID%2 == 0)
				OtherPlayerManager::sharedManager()->m_pOtherPlayer[j]->SetTeam(RED_TEAM);
			else
				OtherPlayerManager::sharedManager()->m_pOtherPlayer[j]->SetTeam(BLUE_TEAM);
			OtherPlayerManager::sharedManager()->m_pOtherPlayer[j]->SetVisible(TRUE);
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
		for(int j=0; j<MAX_OTHER_PLAYER; j++)
		{
			if(OtherPlayerManager::sharedManager()->m_pOtherPlayer[j]->GetID() != _PI[i].PI.m_ID) continue;
			D3DXVECTOR3 q;
			q.x = _PI[i].PI.m_Pos.x;
			q.y = _PI[i].PI.m_Pos.y;
			q.z = _PI[i].PI.m_Pos.z;
			OtherPlayerManager::sharedManager()->m_pOtherPlayer[j]->SetNewDestination(q);
			OtherPlayerManager::sharedManager()->m_pOtherPlayer[j]->SetRot(_PI[i].PI.m_Rot);
			OtherPlayerManager::sharedManager()->m_pOtherPlayer[j]->SetState(_PI[i].PI.m_iState);
			OtherPlayerManager::sharedManager()->m_pOtherPlayer[j]->SetTargetID(_PI[i].PI.m_iTargetID);
			OtherPlayerManager::sharedManager()->m_pOtherPlayer[j]->SetHP(_PI[i].PI.m_HP);
			OtherPlayerManager::sharedManager()->m_pOtherPlayer[j]->SetDamage(_PI[i].PI.m_Damage);
			break;
		}
	}
}

void CScene::AddMinion(ID3D11Device *pd3dDevice)
{
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

void CScene::GameCollision(CGameObject* obj1, CGameObject* obj2)
{

	if(CheckCollision(obj1->GetPosition(), obj1->GetBoundSizeX(), obj1->GetBoundSizeY(), 
		obj1->GetBoundSizeZ(), obj2->GetPosition(),obj2->GetBoundSizeX(), obj2->GetBoundSizeY(), 
		obj2->GetBoundSizeZ()))
	{

		//캐릭터 왼쪽방향
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

		//캐릭터 오른쪽 방향
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
		//캐릭터 위쪽 방향
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

		//캐릭터 아래쪽 방향
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

void CScene::SetMinionInfo(MinionInfo* _MI)
{
	D3DXVECTOR3 des;
	ZeroMemory(&des, sizeof(D3DXVECTOR3));
	for(int i=0; i<MAX_MINION; i++)
	{
		des.x = _MI[i].m_Pos.x;
		des.y= _MI[i].m_Pos.y;
		des.z = _MI[i].m_Pos.z;

		MinionManager::sharedManager()->m_pMinion1[i]->SetNewDestination(des);
		MinionManager::sharedManager()->m_pMinion1[i]->SetVisible(_MI[i].m_Live);
		MinionManager::sharedManager()->m_pMinion1[i]->SetID(_MI[i].m_ID);

	}
}

void CScene::OtherPlayerTargetSetting()
{
	for(int i=0; i<MAX_OTHER_PLAYER; i++)
	{
		if(OtherPlayerManager::sharedManager()->m_pOtherPlayer[i]->GetVisible() != TRUE) continue;

		for(int j=0; j<MAX_TOWER; j++)   //타워와의 타겟 체크
		{
			if(OtherPlayerManager::sharedManager()->m_pOtherPlayer[i]->GetTargetID() ==	TowerManager::sharedManager()->m_pTower[j]->GetID()
				&& OtherPlayerManager::sharedManager()->m_pOtherPlayer[i]->GetTeam() !=	TowerManager::sharedManager()->m_pTower[j]->GetTeam())
			{
				OtherPlayerManager::sharedManager()->m_pOtherPlayer[i]->SetTarget(TowerManager::sharedManager()->m_pTower[j]);
				return;
			}
		}

		//플레이어와의 타겟 체크
		if(OtherPlayerManager::sharedManager()->m_pOtherPlayer[i]->GetTargetID() ==	HeroManager::sharedManager()->m_pHero->GetID()
			&& OtherPlayerManager::sharedManager()->m_pOtherPlayer[i]->GetTeam() != HeroManager::sharedManager()->m_pHero->GetTeam())              
		{
			OtherPlayerManager::sharedManager()->m_pOtherPlayer[i]->SetTarget(HeroManager::sharedManager()->m_pHero);
			return;
		}

		for(int j = 0; j < MAX_MINION; j++)   //미니언과의 타겟 체크
		{
			if(OtherPlayerManager::sharedManager()->m_pOtherPlayer[i]->GetTargetID() ==	MinionManager::sharedManager()->m_pMinion1[j]->GetID()
				&& OtherPlayerManager::sharedManager()->m_pOtherPlayer[i]->GetTeam() !=	MinionManager::sharedManager()->m_pMinion1[j]->GetTeam())
			{
				OtherPlayerManager::sharedManager()->m_pOtherPlayer[i]->SetTarget(MinionManager::sharedManager()->m_pMinion1[j]);
				return;
			}
		}

		if(OtherPlayerManager::sharedManager()->m_pOtherPlayer[i]->GetTargetID() == m_pBlueNexus->GetID())
		{
			OtherPlayerManager::sharedManager()->m_pOtherPlayer[i]->SetTarget(m_pBlueNexus);
			return;
		}

		if(OtherPlayerManager::sharedManager()->m_pOtherPlayer[i]->GetTargetID() == m_pRedNexus->GetID())
		{
			OtherPlayerManager::sharedManager()->m_pOtherPlayer[i]->SetTarget(m_pRedNexus);
			return;
		}
		
		OtherPlayerManager::sharedManager()->m_pOtherPlayer[i]->SetTarget(NULL);
	}
}
