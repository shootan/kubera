#include "Scene.h"

CScene::CScene(void)
{
	m_pObjectShaders = NULL;
	m_pInstancingShaders = NULL;
	m_pAnimationShaders = NULL;
	m_pParticleShaders = NULL;
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
	m_pWarriorMesh = NULL;
	m_pWizardMesh = NULL;

	m_particleEnableBlendingState = 0;
	m_particleDisableBlendingState = 0;

	m_pLights = NULL;
	m_pd3dcbLights = NULL;
}


CScene::~CScene(void)
{

}

void CScene::BuildObjects(ID3D11Device *pd3dDevice)
{
	CreateBlend(pd3dDevice); //이펙트 블렌딩 설정

	m_Control.m_Camera = m_Camera;
	//이 쉐이더 객체에 대한 포인터들의 배열을 정의한다.
	printf("CreateObjectShader \n");
	//CObjectShader 클래스 객체를 생성한다.
	m_pObjectShaders = new CObjectShader();
	m_pObjectShaders->CreateShader(pd3dDevice, 100);
	printf("CreateInstancingShader\n");
	m_pInstancingShaders = new CInstancingShader();
	m_pInstancingShaders->CreateShader(pd3dDevice, 10);
	m_pInstancingShaders->BuildObjects(pd3dDevice);
	printf("CreateAnimationShader \n");
	m_pAnimationShaders = new CAnimationShader();
	m_pAnimationShaders->CreateShader(pd3dDevice, 100);
	printf("CreateParticleShader \n");
	m_pParticleShaders = new CParticleShader();
	m_pParticleShaders->CreateShader(pd3dDevice, 300);
		
	//게임 객체에 대한 포인터들의 배열을 정의한다.
	m_nIntanceObjects = m_pInstancingShaders->GetObjectsNumber();
	m_nObjects = 20 + m_nIntanceObjects;

	//정육면체 메쉬를 생성하고 객체에 연결한다.
	printf("Load WarriorModel \n");

	//워리어 메쉬
	/*m_pWarriorMesh = new GFBX::Mesh();
	GFBXMeshLoader::getInstance()->OnCreateDevice(pd3dDevice);
	GFBXMeshLoader::getInstance()->LoadFBXMesh(m_pWarriorMesh, L"Hero/Hero/knight2.FBX", pd3dDevice);
	m_pWarriorMesh->OnCreateDevice(pd3dDevice);
	for(int i=0; i<m_pWarriorMesh->GetSubsetCount(); i++)
		m_pWarriorMesh->GetSubset(i)->LoadTexture(pd3dDevice, L"Hero/micro_knight.png");*/

	printf("Success Load \n");
	printf("Load WizardModel \n");

	//위자드 메쉬
	m_pWizardMesh = new GFBX::Mesh();
	GFBXMeshLoader::getInstance()->OnCreateDevice(pd3dDevice);
	GFBXMeshLoader::getInstance()->LoadFBXMesh(m_pWizardMesh, L"Hero/Hero/Wizard2.FBX", pd3dDevice);
	m_pWizardMesh->OnCreateDevice(pd3dDevice);
	for(int i=0; i<m_pWizardMesh->GetSubsetCount(); i++)
	{
		m_pWizardMesh->GetSubset(i)->LoadTexture(pd3dDevice, L"Hero/micro_wizard_col.tif");
		m_pWizardMesh->GetSubset(i)->SetBoundingCube(D3DXVECTOR3(10, 13, 10));
	}
	printf("Success Load \n");

	printf("Load Object");
	//바닥 메쉬
	GFBX::Mesh *pPlaneMesh = new GFBX::Mesh();
	GFBXMeshLoader::getInstance()->LoadFBXMesh(pPlaneMesh, L"imagefile/Plane4.FBX", pd3dDevice);
	pPlaneMesh->GetSubset(0)->SetUVTilling(10);
	pPlaneMesh->OnCreateDevice(pd3dDevice);
	for(int i=0; i<pPlaneMesh->GetSubsetCount(); i++)
	{
		pPlaneMesh->GetSubset(i)->LoadTexture(pd3dDevice, L"imagefile/12.png");
		pPlaneMesh->GetSubset(i)->SetBoundingCube(D3DXVECTOR3(1200, 0, 800));
	}
	printf(".");
	
	//블루팀 넥서스 메쉬
	GFBX::Mesh *pBlueNexusMesh = new GFBX::Mesh();
	GFBXMeshLoader::getInstance()->LoadFBXMesh(pBlueNexusMesh, L"tower/Nexus.FBX", pd3dDevice);
	pBlueNexusMesh->OnCreateDevice(pd3dDevice);
	for(int i=0; i<pBlueNexusMesh->GetSubsetCount(); i++)
	{
		pBlueNexusMesh->GetSubset(i)->LoadTexture(pd3dDevice, L"tower/Nexus.png");
		pBlueNexusMesh->GetSubset(i)->SetBoundingCube(D3DXVECTOR3(50, 50, 50));
	}
	printf(".");

	//빨강팀 넥서스 메쉬
	GFBX::Mesh *pRedNexusMesh = new GFBX::Mesh();
	GFBXMeshLoader::getInstance()->LoadFBXMesh(pRedNexusMesh, L"tower/Nexus.FBX", pd3dDevice);
	pRedNexusMesh->OnCreateDevice(pd3dDevice);
	for(int i=0; i<pRedNexusMesh->GetSubsetCount(); i++)
	{
		pRedNexusMesh->GetSubset(i)->LoadTexture(pd3dDevice, L"tower/Nexus2.png");
		pRedNexusMesh->GetSubset(i)->SetBoundingCube(D3DXVECTOR3(50, 50, 50));
	}
	printf(".");

	//파티클 메쉬
	m_pParticleMesh = new ParticleMesh(pd3dDevice);
	m_pParticleMesh->Initialize(pd3dDevice, L"effect/star.dds");
	printf(".");
	m_pParticle2Mesh = new Particle3Mesh(pd3dDevice);
	m_pParticle2Mesh->Initialize(pd3dDevice, L"effect/star.dds");
	m_pParticle2Mesh->SetScale(D3DXVECTOR2(5, 5));
	printf(".");
	m_pParticle3Mesh = new Particle3Mesh(pd3dDevice);
	m_pParticle3Mesh->Initialize(pd3dDevice, L"effect/rocketlauncher_fx-2.tif");
	printf(".");

	//파티클 생성
	for(int i=0; i<4; i++)
		ParticleManager::sharedManager()->CreateParticle(D3DXVECTOR3(1200, 10, 0), m_pParticleMesh, WIZARD_SKILL_BODY);
	for(int i=0; i<30; i++)
		ParticleManager::sharedManager()->CreateParticle(D3DXVECTOR3(1200, 10, 0), m_pParticle2Mesh, WIZARD_ATTACK);
	for(int i=0; i<10; i++)
		ParticleManager::sharedManager()->CreateParticle(D3DXVECTOR3(1200, 10, 0), m_pParticle3Mesh, WIZARD_SKILL_MISSILE);

	CMaterial *pMaterial = new CMaterial();
	pMaterial->AddRef();
	pMaterial->m_Material.m_d3dxcDiffuse = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.0f);//D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pMaterial->m_Material.m_d3dxcAmbient = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.0f);//D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pMaterial->m_Material.m_d3dxcSpecular = D3DXCOLOR(0.1f, 0.1f, 0.1f, 5.0f);// D3DXCOLOR(1.0f, 1.0f, 1.0f, 5.0f);
	pMaterial->m_Material.m_d3dxcEmissive = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
	
	//히어로 생성
	HeroManager::sharedManager()->CreateHero(m_pWarriorMesh, m_pWizardMesh, 10, 13, 10);
	HeroManager::sharedManager()->m_pHero->SetScale(D3DXVECTOR3(0.1, 0.1, 0.1));
	HeroManager::sharedManager()->m_pHero->SetMaterial(pMaterial);
	//바닥 생성
	m_pPlane = new CGameObject();
	m_pPlane->SetMesh(pPlaneMesh);
	m_pPlane->SetMaterial(pMaterial);

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


	//넥서스 2개 생성
	m_pBlueNexus = new CGameObject();
	m_pBlueNexus->SetMesh(pBlueNexusMesh);
	m_pBlueNexus->SetPosition(D3DXVECTOR3(-550, 0, 0));
	m_pBlueNexus->SetBoundSize(50, 20, 50);
	m_pBlueNexus->SetHP(2000);
	m_pBlueNexus->SetID(175);
	m_pBlueNexus->SetMaterial(pMaterial);
	m_pBlueNexus->SetTeam(BLUE_TEAM);

	m_pRedNexus = new CGameObject();
	m_pRedNexus->SetMesh(pRedNexusMesh);
	m_pRedNexus->SetPosition(D3DXVECTOR3(550, 0, 0));
	m_pRedNexus->SetBoundSize(50, 50, 50);
	m_pRedNexus->SetHP(2000);
	m_pRedNexus->SetID(176);
	m_pRedNexus->SetMaterial(pMaterial);
	m_pRedNexus->SetTeam(RED_TEAM);

	HeroManager::sharedManager()->SetNexus(m_pRedNexus, m_pBlueNexus);


	//pFBXMesh->Release();

 	//애니메이션 쉐이더에 히어로 연결
 	m_pAnimationShaders->AddObject(HeroManager::sharedManager()->m_pHero);

	//오브젝트 쉐이더에 지형 넥서스 연결
	m_pAnimationShaders->AddObject(m_pPlane);
	for(int i=0; i<iObjectNum; i++)
		m_pObjectShaders->AddObject(pBoundBox[i]);
	m_pAnimationShaders->AddObject(m_pBlueNexus);
	m_pAnimationShaders->AddObject(m_pRedNexus);


	//아더 플레이어 생성
	OtherPlayerManager::sharedManager()->SetMesh(m_pWarriorMesh, m_pWizardMesh);
	OtherPlayerManager::sharedManager()->CreateOtherPlayer(D3DXVECTOR3(1500, 0, 0), 10, 13, 10);
	for(int i=0; i<MAX_OTHER_PLAYER;i++)
		m_pAnimationShaders->AddObject(OtherPlayerManager::sharedManager()->m_pOtherPlayer[i]);
	
	this->AddOtherPlayer(pd3dDevice);


	//파티클 객체 쉐이더에 연결
	for(int i=0; i<MAX_PARTICLE; i++)
	{
		if(ParticleManager::sharedManager()->m_pParticle[i] == NULL) continue;
		m_pParticleShaders->AddObject(ParticleManager::sharedManager()->m_pParticle[i]);
	}

	//조명 생성
	CreateLightShaderVariables(pd3dDevice);
}

void CScene::ReleaseObjects()
{
	//쉐이더 객체 리스트의 각 객체를 소멸시키고 리스트를 소멸시킨다.

	if(m_pObjectShaders) delete m_pObjectShaders;
	if(m_pInstancingShaders) delete m_pInstancingShaders;
	if(m_pAnimationShaders) delete m_pAnimationShaders;
	if(m_pParticleShaders) delete m_pParticleShaders;
	//게임 객체 리스트의 각 객체를 반환(Release)하고 리스트를 소멸시킨다.

	if(m_pPlane) m_pPlane->Release();
	if(m_particleEnableBlendingState)
	{
		m_particleEnableBlendingState->Release();
		m_particleEnableBlendingState = 0;
	}

	if(m_particleDisableBlendingState)
	{
		m_particleDisableBlendingState->Release();
		m_particleDisableBlendingState = 0;
	}

	//조명
	ReleaseLightShaderVariables();
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
	if(m_pLights && m_pd3dcbLights) 
	{
		m_pLights->m_d3dxvCameraPosition =  D3DXVECTOR4(m_Camera->m_CameraPos, 1.0f);

		//히어로 조명
		m_pLights->m_pLights[0].m_d3dxvPosition = HeroManager::sharedManager()->m_pHero->GetPosition() + D3DXVECTOR3(0, 350, 0);
	}

	HeroManager::sharedManager()->m_pHero->Animate(fTimeElapsed);
	HeroManager::sharedManager()->m_pHero->Update(fTimeElapsed);
 
   	for(int i=0; i<MAX_MINION; i++)
   	{
  		MinionManager::sharedManager()->m_pMinion1[i]->Update(fTimeElapsed);
  		MinionManager::sharedManager()->m_pMinion1[i]->Animate(fTimeElapsed);
   	}

	for(int i=0; i<MAX_MISSILE; i++)
		MissileManager::sharedManager()->m_pMissile[i]->Update(fTimeElapsed);

	for(int i=0; i<MAX_PARTICLE; i++)
	{
		if(ParticleManager::sharedManager()->m_pParticle[i] == NULL) continue;

		if(ParticleManager::sharedManager()->m_pParticle[i]->GetTarget() != NULL &&
			ParticleManager::sharedManager()->m_pParticle[i]->GetTarget()->GetTag() == EFFECT)
			ParticleManager::sharedManager()->m_pParticle[i]->SetTarget(HeroManager::sharedManager()->m_pHero);

		ParticleManager::sharedManager()->m_pParticle[i]->Update(fTimeElapsed);
	}


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

void CScene::Render(ID3D11DeviceContext*pd3dDeviceContext, float fTimeElapsed, CCamera *pCamera)
{
	// 조명
	if(m_pLights && m_pd3dcbLights) UpdateLightShaderVariable(pd3dDeviceContext, m_pLights);
	//쉐이더 객체 리스트의 각 쉐이더 객체를 렌더링한다.
	
	//m_pObjectShaders->Render(pd3dDeviceContext);
	//m_pObjectShaders->UpdateShaderVariables(pd3dDeviceContext, &pBoundBox[0]->m_d3dxmtxWorld);
	//pBoundBox[0]->Render(pd3dDeviceContext);
	
	//m_pObjectShaders->UpdateShaderVariables(pd3dDeviceContext, &HeroManager::sharedManager()->m_pHero->m_d3dxmtxWorld);
	//HeroManager::sharedManager()->m_pHero->Render(pd3dDeviceContext);
	
	/*for(int i=0; i<MAX_OTHER_PLAYER; i++)
	{
		if(OtherPlayerManager::sharedManager()->m_pOtherPlayer[i]->GetVisible() != TRUE) continue;

		m_pObjectShaders->UpdateShaderVariables(pd3dDeviceContext, &OtherPlayerManager::sharedManager()->m_pOtherPlayer[i]->m_d3dxmtxWorld);
		OtherPlayerManager::sharedManager()->m_pOtherPlayer[i]->Render(pd3dDeviceContext);
	}*/

	m_pInstancingShaders->Render(pd3dDeviceContext, pCamera);

	for(int i=0; i<MAX_MISSILE; i++)
		MissileManager::sharedManager()->m_pMissile[i]->Render(pd3dDeviceContext, pCamera);
	
	for(int i=0; i<MAX_TOWER; i++)
		TowerManager::sharedManager()->m_pTower[i]->Render(pd3dDeviceContext, pCamera);

	for(int i=0; i<872; i++)
		ObstacleManager::sharedManager()->m_pObstacle[i]->Render(pd3dDeviceContext, pCamera);
	
  	for(int i=0; i<MAX_MINION; i++)
  		MinionManager::sharedManager()->m_pMinion1[i]->Render(pd3dDeviceContext, pCamera);
	
	for(int i=0; i<MAX_DESTROY_TOWER; i++)
		ObstacleManager::sharedManager()->m_pDestroyTower[i]->Render(pd3dDeviceContext, pCamera);
	

	m_pAnimationShaders->Render(pd3dDeviceContext);

	if(HeroManager::sharedManager()->m_pHero->IsVisible(pCamera))
		m_pAnimationShaders->UpdateShaderVariables(pd3dDeviceContext, &HeroManager::sharedManager()->m_pHero->m_d3dxmtxWorld);
	HeroManager::sharedManager()->m_pHero->Render(pd3dDeviceContext, fTimeElapsed, pCamera);

	if(m_pPlane->IsVisible(pCamera))
		m_pAnimationShaders->UpdateShaderVariables(pd3dDeviceContext, &m_pPlane->m_d3dxmtxWorld);
	m_pPlane->Render(pd3dDeviceContext, pCamera);
	

	if(m_pBlueNexus->IsVisible(pCamera))
		m_pAnimationShaders->UpdateShaderVariables(pd3dDeviceContext, &m_pBlueNexus->m_d3dxmtxWorld);
	m_pBlueNexus->Render(pd3dDeviceContext, pCamera);
	

	if(m_pRedNexus->IsVisible(pCamera))
		m_pAnimationShaders->UpdateShaderVariables(pd3dDeviceContext, &m_pRedNexus->m_d3dxmtxWorld);
	m_pRedNexus->Render(pd3dDeviceContext, pCamera);
	

	for(int i=0; i<MAX_OTHER_PLAYER; i++)
	{
		if(OtherPlayerManager::sharedManager()->m_pOtherPlayer[i]->GetVisible() != TRUE) continue;

		if(OtherPlayerManager::sharedManager()->m_pOtherPlayer[i]->IsVisible(pCamera))
			m_pAnimationShaders->UpdateShaderVariables(pd3dDeviceContext, &OtherPlayerManager::sharedManager()->m_pOtherPlayer[i]->m_d3dxmtxWorld);
		OtherPlayerManager::sharedManager()->m_pOtherPlayer[i]->Render(pd3dDeviceContext, fTimeElapsed, pCamera);
	}


	TurnOnAlphaBlending(pd3dDeviceContext, m_particleEnableBlendingState);
	m_pParticleShaders->Render(pd3dDeviceContext);
	for(int i=0; i<MAX_PARTICLE; i++)
	{
		if(ParticleManager::sharedManager()->m_pParticle[i] == NULL) continue;

		if(ParticleManager::sharedManager()->m_pParticle[i]->IsVisible(pCamera))
			m_pParticleShaders->UpdateShaderVariables(pd3dDeviceContext, &ParticleManager::sharedManager()->m_pParticle[i]->m_d3dxmtxWorld);
		ParticleManager::sharedManager()->m_pParticle[i]->Render(pd3dDeviceContext, pCamera);
	
	}
	TurnOffAlphaBlending(pd3dDeviceContext, m_particleDisableBlendingState);

	m_pParticleMesh->SetCamVec(*m_Camera->GetWorldRight(), *m_Camera->GetWorldUp());
	m_pParticle2Mesh->SetCamVec(*m_Camera->GetWorldRight(), *m_Camera->GetWorldUp());
	m_pParticle3Mesh->SetCamVec(*m_Camera->GetWorldRight(), *m_Camera->GetWorldUp());
	//m_pParticleMesh->SetCamVec(D3DXVECTOR3(m_Camera->GetPitch(), 0, 0), D3DXVECTOR3(0, 0, m_Camera->GetRoll()));
	//m_pParticle2Mesh->SetCamVec(D3DXVECTOR3(m_Camera->GetPitch(), 0, 0), D3DXVECTOR3(0, 0, m_Camera->GetRoll()));
	//m_pParticle3Mesh->SetCamVec(D3DXVECTOR3(m_Camera->GetPitch(), 0, 0), D3DXVECTOR3(0, 0, m_Camera->GetRoll()));
	//m_pParticleMesh->SetCamVec(m_Camera->GetRightVector(), m_Camera->GetUpVector());
	//m_pParticle2Mesh->SetCamVec(m_Camera->GetRightVector(), m_Camera->GetUpVector());
	//m_pParticle3Mesh->SetCamVec(m_Camera->GetRightVector(), m_Camera->GetUpVector());
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
		if(_PI[i].PI.m_Type == 0) continue;
		for(int j=0; j<MAX_OTHER_PLAYER; j++)
		{
			if(OtherPlayerManager::sharedManager()->m_pOtherPlayer[j]->GetID() != 0) continue;
			OtherPlayerManager::sharedManager()->m_pOtherPlayer[j]->SetID(_PI[i].PI.m_ID);
			OtherPlayerManager::sharedManager()->m_pOtherPlayer[j]->SetPos(_PI[i].PI.m_Pos);
			OtherPlayerManager::sharedManager()->m_pOtherPlayer[j]->SetState(_PI[i].PI.m_iState);
			OtherPlayerManager::sharedManager()->m_pOtherPlayer[j]->SetTargetID(_PI[i].PI.m_iTargetID);
			OtherPlayerManager::sharedManager()->m_pOtherPlayer[j]->SetHP(_PI[i].PI.m_HP);
			OtherPlayerManager::sharedManager()->m_pOtherPlayer[j]->SetDamage(_PI[i].PI.m_Damage);

			OtherPlayerManager::sharedManager()->m_pOtherPlayer[j]->SetRot(_PI[i].PI.m_Rot);

			OtherPlayerManager::sharedManager()->m_pOtherPlayer[j]->SetType(_PI[i].PI.m_Type);
			if(_PI[i].PI.m_Type == 1)
				OtherPlayerManager::sharedManager()->m_pOtherPlayer[j]->SetAniMesh(m_pWarriorMesh);
			else if(_PI[i].PI.m_Type == 2)
				OtherPlayerManager::sharedManager()->m_pOtherPlayer[j]->SetAniMesh(m_pWizardMesh);


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

//블렌딩
void CScene::TurnOnAlphaBlending(ID3D11DeviceContext *pd3dDeviceContext, ID3D11BlendState* blendstate)
{
	float blendFactor[4];


	// Setup the blend factor.
	blendFactor[0] = 0.0f;
	blendFactor[1] = 0.0f;
	blendFactor[2] = 0.0f;
	blendFactor[3] = 0.0f;

	// Turn on the alpha blending.
	pd3dDeviceContext->OMSetBlendState(blendstate, blendFactor, 0xffffffff);

	return;
}

void CScene::TurnOffAlphaBlending(ID3D11DeviceContext *pd3dDeviceContext, ID3D11BlendState* blendstate)
{
	float blendFactor[4];


	// Setup the blend factor.
	blendFactor[0] = 0.0f;
	blendFactor[1] = 0.0f;
	blendFactor[2] = 0.0f;
	blendFactor[3] = 0.0f;

	// Turn off the alpha blending.
	pd3dDeviceContext->OMSetBlendState(blendstate, blendFactor, 0xffffffff);

	return;
}

HRESULT CScene::CreateBlend(ID3D11Device *pd3dDevice)
{
	HRESULT result;
	D3D11_BLEND_DESC blendStateDescription;
	// Clear the blend state description.
	ZeroMemory(&blendStateDescription, sizeof(D3D11_BLEND_DESC));

	// Create an alpha enabled blend state description.
	blendStateDescription.RenderTarget[0].BlendEnable = TRUE;
	blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].RenderTargetWriteMask = 0x0f;

	result = pd3dDevice->CreateBlendState(&blendStateDescription, &m_particleEnableBlendingState);
	if(FAILED(result))
	{
		return false;
	}

	blendStateDescription.RenderTarget[0].BlendEnable = FALSE;

	result = pd3dDevice->CreateBlendState(&blendStateDescription, &m_particleDisableBlendingState);
	if(FAILED(result))
	{
		return false;
	}

	return S_OK;
}

//조명
void CScene::CreateLightShaderVariables(ID3D11Device *pd3dDevice)
{
	m_pLights = new LIGHTS;
	::ZeroMemory(m_pLights, sizeof(LIGHTS));
	//게임 월드 전체를 비추는 주변조명을 설정한다.
	m_pLights->m_d3dxcGlobalAmbient = D3DXCOLOR(0.3f, 0.3f, 0.3f, 1.0f);

	//3개의 조명(점 광원, 스팟 광원, 방향성 광원)을 설정한다.

	for(int i=0; i<MAX_LIGHTS - 1; i++)
	{
		//m_pLights->m_pLights[i].m_bEnable = 1.0f;
		//m_pLights->m_pLights[i].m_nType = POINT_LIGHT;
		//m_pLights->m_pLights[i].m_fRange = 80.0f;
		//m_pLights->m_pLights[i].m_d3dxcAmbient = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		//m_pLights->m_pLights[i].m_d3dxcDiffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		//m_pLights->m_pLights[i].m_d3dxcSpecular = D3DXCOLOR(0.5f, 0.5f, 0.5f, 0.0f);
		//m_pLights->m_pLights[i].m_d3dxvPosition = D3DXVECTOR3(-600.0f, 100.0f, 100.0f);
		//m_pLights->m_pLights[i].m_d3dxvDirection = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		//m_pLights->m_pLights[i].m_d3dxvAttenuation = D3DXVECTOR3(1.0f, 0.01f, 0.001f);

		m_pLights->m_pLights[i].m_bEnable = 1.0f;
		m_pLights->m_pLights[i].m_nType = SPOT_LIGHT;
		m_pLights->m_pLights[i].m_fRange = 370.0f;
		m_pLights->m_pLights[i].m_d3dxcAmbient = D3DXCOLOR(0.05f, 0.05f, 0.05f, 1.0f);
		m_pLights->m_pLights[i].m_d3dxcDiffuse = D3DXCOLOR(0.05f, 0.05f, 0.05f, 1.0f);
		m_pLights->m_pLights[i].m_d3dxcSpecular = D3DXCOLOR(0.05f, 0.05f, 0.05f, 0.0f);
		m_pLights->m_pLights[i].m_d3dxvPosition = D3DXVECTOR3(500.0f, 300.0f, 500.0f);
		m_pLights->m_pLights[i].m_d3dxvDirection = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
		m_pLights->m_pLights[i].m_d3dxvAttenuation = D3DXVECTOR3(1.0f, 0.01f, 0.0001f);
		m_pLights->m_pLights[i].m_fFalloff = 20.0f;
		m_pLights->m_pLights[i].m_fPhi = (float)cos(D3DXToRadian(40.0f));
		m_pLights->m_pLights[i].m_fTheta = (float)cos(D3DXToRadian(20.0f));
	}

	m_pLights->m_pLights[MAX_LIGHTS - 1].m_bEnable = 1.0f;
	m_pLights->m_pLights[MAX_LIGHTS - 1].m_nType = DIRECTIONAL_LIGHT;
	m_pLights->m_pLights[MAX_LIGHTS - 1].m_d3dxcAmbient = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.0f);
	m_pLights->m_pLights[MAX_LIGHTS - 1].m_d3dxcDiffuse = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.0f);
	m_pLights->m_pLights[MAX_LIGHTS - 1].m_d3dxcSpecular = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f);
	m_pLights->m_pLights[MAX_LIGHTS - 1].m_d3dxvDirection = D3DXVECTOR3(0.0f, -1.0f, 0.0f);

	//조명 배치
	//본진기지 조명
	if(HeroManager::sharedManager()->m_pHero->GetTeam() == RED_TEAM)
		m_pLights->m_pLights[1].m_d3dxvPosition = m_pRedNexus->GetPosition() + D3DXVECTOR3(0, 350, 0);
	else if(HeroManager::sharedManager()->m_pHero->GetTeam() == BLUE_TEAM)
		m_pLights->m_pLights[1].m_d3dxvPosition = m_pBlueNexus->GetPosition() + D3DXVECTOR3(0, 350, 0);

	//타워 조명
	int j = 2;
	for(int i = 0; i < MAX_TOWER; i++)
	{
		if(TowerManager::sharedManager()->m_pTower[i]->GetTeam() == HeroManager::sharedManager()->m_pHero->GetTeam())
			m_pLights->m_pLights[j++].m_d3dxvPosition = TowerManager::sharedManager()->m_pTower[i]->GetPosition() + D3DXVECTOR3(0, 350, 0);
	}

	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(d3dBufferDesc));
	d3dBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	d3dBufferDesc.ByteWidth = sizeof(LIGHTS);
	d3dBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	d3dBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = m_pLights;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dcbLights);
}
void CScene::UpdateLightShaderVariable(ID3D11DeviceContext *pd3dDeviceContext, LIGHTS *pLights)
{
	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	pd3dDeviceContext->Map(m_pd3dcbLights, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	LIGHTS *pcbLight = (LIGHTS *)d3dMappedResource.pData;
	memcpy(pcbLight, pLights, sizeof(LIGHTS));
	pd3dDeviceContext->Unmap(m_pd3dcbLights, 0);
	pd3dDeviceContext->PSSetConstantBuffers(PS_SLOT_LIGHT, 1, &m_pd3dcbLights);
}
void CScene::ReleaseLightShaderVariables()
{
	if (m_pLights) delete m_pLights;
	if (m_pd3dcbLights) m_pd3dcbLights->Release();
}