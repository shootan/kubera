#include "Scene.h"

CScene::CScene(void)
{
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

	m_fMissileAttackTime = 0.f;

	m_pPlane = NULL;
	m_pBlueNexus = NULL;
	m_pRedNexus = NULL;
	m_pWarriorMesh = NULL;
	m_pWizardMesh = NULL;

	m_particleEnableBlendingState = 0;
	m_particleDisableBlendingState = 0;
}


CScene::~CScene(void)
{

}

void CScene::BuildObjects(ID3D11Device *pd3dDevice)
{
	CreateBlend(pd3dDevice); //����Ʈ ���� ����

	m_Control.m_Camera = m_Camera;
	//�� ���̴� ��ü�� ���� �����͵��� �迭�� �����Ѵ�.
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
		
	//���� ��ü�� ���� �����͵��� �迭�� �����Ѵ�.
	m_nIntanceObjects = m_pInstancingShaders->GetObjectsNumber();
	m_nObjects = 20 + m_nIntanceObjects;

	//������ü �޽��� �����ϰ� ��ü�� �����Ѵ�.
	printf("Load WarriorModel \n");

	//������ �޽�
	m_pWarriorMesh = new GFBX::Mesh();
	GFBXMeshLoader::getInstance()->OnCreateDevice(pd3dDevice);
	GFBXMeshLoader::getInstance()->LoadFBXMesh(m_pWarriorMesh, L"Hero/Hero/knight2.FBX", pd3dDevice);
	m_pWarriorMesh->OnCreateDevice(pd3dDevice);
	for(int i=0; i<m_pWarriorMesh->GetSubsetCount(); i++)
		m_pWarriorMesh->GetSubset(i)->LoadTexture(pd3dDevice, L"Hero/micro_knight.png");

	printf("Success Load \n");
	printf("Load WizardModel \n");

	//���ڵ� �޽�
	m_pWizardMesh = new GFBX::Mesh();
	GFBXMeshLoader::getInstance()->OnCreateDevice(pd3dDevice);
	GFBXMeshLoader::getInstance()->LoadFBXMesh(m_pWizardMesh, L"Hero/Hero/Wizard2.FBX", pd3dDevice);
	m_pWizardMesh->OnCreateDevice(pd3dDevice);
	for(int i=0; i<m_pWizardMesh->GetSubsetCount(); i++)
		m_pWizardMesh->GetSubset(i)->LoadTexture(pd3dDevice, L"Hero/micro_wizard_col.tif");
	printf("Success Load \n");

	printf("Load Object");
	//�ٴ� �޽�
	GFBX::Mesh *pPlaneMesh = new GFBX::Mesh();
	GFBXMeshLoader::getInstance()->LoadFBXMesh(pPlaneMesh, L"imagefile/Plane4.FBX", pd3dDevice);
	pPlaneMesh->GetSubset(0)->SetUVTilling(10);
	pPlaneMesh->OnCreateDevice(pd3dDevice);
	for(int i=0; i<pPlaneMesh->GetSubsetCount(); i++)
		pPlaneMesh->GetSubset(i)->LoadTexture(pd3dDevice, L"imagefile/12.png");
	printf(".");
	
	//����� �ؼ��� �޽�
	GFBX::Mesh *pBlueNexusMesh = new GFBX::Mesh();
	GFBXMeshLoader::getInstance()->LoadFBXMesh(pBlueNexusMesh, L"tower/Nexus.FBX", pd3dDevice);
	pBlueNexusMesh->OnCreateDevice(pd3dDevice);
	for(int i=0; i<pBlueNexusMesh->GetSubsetCount(); i++)
		pBlueNexusMesh->GetSubset(i)->LoadTexture(pd3dDevice, L"tower/Nexus.png");
	printf(".");

	//������ �ؼ��� �޽�
	GFBX::Mesh *pRedNexusMesh = new GFBX::Mesh();
	GFBXMeshLoader::getInstance()->LoadFBXMesh(pRedNexusMesh, L"tower/Nexus.FBX", pd3dDevice);
	pRedNexusMesh->OnCreateDevice(pd3dDevice);
	for(int i=0; i<pRedNexusMesh->GetSubsetCount(); i++)
		pRedNexusMesh->GetSubset(i)->LoadTexture(pd3dDevice, L"tower/Nexus2.png");
	printf(".");

	//��ƼŬ �޽�
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

	//��ƼŬ ����
	for(int i=0; i<4; i++)
		ParticleManager::sharedManager()->CreateParticle(D3DXVECTOR3(1200, 10, 0), m_pParticleMesh, WIZARD_SKILL_BODY);
	for(int i=0; i<30; i++)
		ParticleManager::sharedManager()->CreateParticle(D3DXVECTOR3(1200, 10, 0), m_pParticle2Mesh, WIZARD_ATTACK);
	for(int i=0; i<10; i++)
		ParticleManager::sharedManager()->CreateParticle(D3DXVECTOR3(1200, 10, 0), m_pParticle3Mesh, WIZARD_SKILL_MISSILE);
	
	//����� ����
	HeroManager::sharedManager()->CreateHero(m_pWarriorMesh, m_pWizardMesh, 10, 13, 10);
	HeroManager::sharedManager()->m_pHero->SetScale(D3DXVECTOR3(0.1, 0.1, 0.1));

	//�ٴ� ����
	m_pPlane = new CGameObject();
	m_pPlane->SetMesh(pPlaneMesh);

	//�浹�ڽ�
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


	//�ؼ��� 2�� ����
	Vector3 p = MapEditorManager::sharedManager()->GetBlueNexusPos();
	m_pBlueNexus = new CGameObject();
	m_pBlueNexus->SetMesh(pBlueNexusMesh);
	m_pBlueNexus->SetPosition(D3DXVECTOR3(p.x, 0, p.z));
	m_pBlueNexus->SetBoundSize(50, 20, 50);
	m_pBlueNexus->SetHP(2000);
	m_pBlueNexus->SetID(175);

	p = MapEditorManager::sharedManager()->GetRedNexusPos();
	m_pRedNexus = new CGameObject();
	m_pRedNexus->SetMesh(pRedNexusMesh);
	m_pRedNexus->SetPosition(D3DXVECTOR3(p.x, 0, p.z));
	m_pRedNexus->SetBoundSize(50, 50, 50);
	m_pRedNexus->SetHP(2000);
	m_pRedNexus->SetID(176);

	HeroManager::sharedManager()->SetNexus(m_pRedNexus, m_pBlueNexus);


	//pFBXMesh->Release();

 	//�ִϸ��̼� ���̴��� ����� ����
 	m_pAnimationShaders->AddObject(HeroManager::sharedManager()->m_pHero);

	//������Ʈ ���̴��� ���� �ؼ��� ����
	m_pAnimationShaders->AddObject(m_pPlane);
	m_pAnimationShaders->AddObject(m_pBlueNexus);
	m_pAnimationShaders->AddObject(m_pRedNexus);


	//�ƴ� �÷��̾� ����
	OtherPlayerManager::sharedManager()->SetMesh(m_pWarriorMesh, m_pWizardMesh);
	OtherPlayerManager::sharedManager()->CreateOtherPlayer(D3DXVECTOR3(1500, 0, 0), 10, 13, 10);
	for(int i=0; i<MAX_OTHER_PLAYER;i++)
		m_pAnimationShaders->AddObject(OtherPlayerManager::sharedManager()->m_pOtherPlayer[i]);
	
	this->AddOtherPlayer(pd3dDevice);


	//��ƼŬ ��ü ���̴��� ����
	for(int i=0; i<MAX_PARTICLE; i++)
	{
		if(ParticleManager::sharedManager()->m_pParticle[i] == NULL) continue;
		m_pParticleShaders->AddObject(ParticleManager::sharedManager()->m_pParticle[i]);
	}
}

void CScene::ReleaseObjects()
{
	//���̴� ��ü ����Ʈ�� �� ��ü�� �Ҹ��Ű�� ����Ʈ�� �Ҹ��Ų��.

	if(m_pInstancingShaders) delete m_pInstancingShaders;
	if(m_pAnimationShaders) delete m_pAnimationShaders;
	if(m_pParticleShaders) delete m_pParticleShaders;
	//���� ��ü ����Ʈ�� �� ��ü�� ��ȯ(Release)�ϰ� ����Ʈ�� �Ҹ��Ų��.

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
	HeroManager::sharedManager()->m_pHero->Animate(fTimeElapsed);
	HeroManager::sharedManager()->m_pHero->Update(fTimeElapsed);
 
   
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


	for(int i=0;i<MAX_TOWER; i++)  //Ÿ���� ĳ���� ����
	{
		if(TowerManager::sharedManager()->m_pTower[i] == NULL) continue;

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
	
	//�÷��̾� �浹�ڽ� ���̱�
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

	for(int i=0; i<MAX_OBSTACLE; i++) //ĳ���Ϳ� ��ֹ� �浹üũ
	{
		if(ObstacleManager::sharedManager()->m_pObstacle[i] == NULL) continue;
		GameCollision(HeroManager::sharedManager()->m_pHero, ObstacleManager::sharedManager()->m_pObstacle[i]);
	}

	for(int i=0; i<MAX_DESTROY_TOWER; i++) //ĳ���Ϳ� �μ��� Ÿ�� �浹üũ
	{
		if(ObstacleManager::sharedManager()->m_pDestroyTower[i] == NULL) continue;
		GameCollision(HeroManager::sharedManager()->m_pHero, ObstacleManager::sharedManager()->m_pDestroyTower[i]);
	}

	for(int i=0; i<MAX_TOWER; i++) //ĳ���Ϳ� Ÿ�� �浹üũ
	{
		if(TowerManager::sharedManager()->m_pTower[i] == NULL) continue;
		GameCollision(HeroManager::sharedManager()->m_pHero, TowerManager::sharedManager()->m_pTower[i]);
	}
	GameCollision(HeroManager::sharedManager()->m_pHero, m_pBlueNexus); //ĳ���Ϳ� �ؼ��� �浹üũ
	GameCollision(HeroManager::sharedManager()->m_pHero, m_pRedNexus);
}

void CScene::Render(ID3D11DeviceContext*pd3dDeviceContext, float fTimeElapsed, CCamera *pCamera)
{
	//���̴� ��ü ����Ʈ�� �� ���̴� ��ü�� �������Ѵ�.

	m_pInstancingShaders->Render(pd3dDeviceContext, pCamera);

	for(int i=0; i<MAX_MISSILE; i++)
	{
		//if(MissileManager::sharedManager()->m_pMissile[i]->IsVisible(pCamera))
			MissileManager::sharedManager()->m_pMissile[i]->Render(pd3dDeviceContext, pCamera);
	}
	for(int i=0; i<MAX_TOWER; i++)
	{
		if(TowerManager::sharedManager()->m_pTower[i]==NULL) continue;
			TowerManager::sharedManager()->m_pTower[i]->Render(pd3dDeviceContext, pCamera);
	}
	int q = MapEditorManager::sharedManager()->Rock2Size;
	q += MapEditorManager::sharedManager()->RockSize;
	q += MapEditorManager::sharedManager()->TreeSize;
	for(int i=0; i<q; i++)
	{
		//if(ObstacleManager::sharedManager()->m_pObstacle[i]->IsVisible(pCamera))
			ObstacleManager::sharedManager()->m_pObstacle[i]->Render(pd3dDeviceContext, pCamera);
	}
  	
	for(int i=0; i<MAX_DESTROY_TOWER; i++)
	{
		//if(ObstacleManager::sharedManager()->m_pDestroyTower[i]->IsVisible(pCamera))
			ObstacleManager::sharedManager()->m_pDestroyTower[i]->Render(pd3dDeviceContext, pCamera);
	}
 
	m_pAnimationShaders->Render(pd3dDeviceContext);

	//if(HeroManager::sharedManager()->m_pHero->IsVisible(pCamera))
	//{
		m_pAnimationShaders->UpdateShaderVariables(pd3dDeviceContext, &HeroManager::sharedManager()->m_pHero->m_d3dxmtxWorld);
		HeroManager::sharedManager()->m_pHero->Render(pd3dDeviceContext, fTimeElapsed, pCamera);
	//}

	//if(m_pPlane->IsVisible(pCamera))
	//{
		m_pAnimationShaders->UpdateShaderVariables(pd3dDeviceContext, &m_pPlane->m_d3dxmtxWorld);
		m_pPlane->Render(pd3dDeviceContext, pCamera);
	//}

	//if(m_pBlueNexus->IsVisible(pCamera))
	//{
		m_pAnimationShaders->UpdateShaderVariables(pd3dDeviceContext, &m_pBlueNexus->m_d3dxmtxWorld);
		m_pBlueNexus->Render(pd3dDeviceContext, pCamera);
	//}

	//if(m_pRedNexus->IsVisible(pCamera))
	//{
		m_pAnimationShaders->UpdateShaderVariables(pd3dDeviceContext, &m_pRedNexus->m_d3dxmtxWorld);
		m_pRedNexus->Render(pd3dDeviceContext, pCamera);
	//}

	for(int i=0; i<MAX_OTHER_PLAYER; i++)
	{
		if(OtherPlayerManager::sharedManager()->m_pOtherPlayer[i]->GetVisible() != TRUE) continue;

		//if(OtherPlayerManager::sharedManager()->m_pOtherPlayer[i]->IsVisible(pCamera))
		//{
			m_pAnimationShaders->UpdateShaderVariables(pd3dDeviceContext, &OtherPlayerManager::sharedManager()->m_pOtherPlayer[i]->m_d3dxmtxWorld);
			OtherPlayerManager::sharedManager()->m_pOtherPlayer[i]->Render(pd3dDeviceContext, fTimeElapsed, pCamera);
		//}
	}


	TurnOnAlphaBlending(pd3dDeviceContext, m_particleEnableBlendingState);
	m_pParticleShaders->Render(pd3dDeviceContext);
	for(int i=0; i<MAX_PARTICLE; i++)
	{
		if(ParticleManager::sharedManager()->m_pParticle[i] == NULL) continue;

		//if(ParticleManager::sharedManager()->m_pParticle[i]->IsVisible(pCamera))
		//{
			m_pParticleShaders->UpdateShaderVariables(pd3dDeviceContext, &ParticleManager::sharedManager()->m_pParticle[i]->m_d3dxmtxWorld);
			ParticleManager::sharedManager()->m_pParticle[i]->Render(pd3dDeviceContext, pCamera);
		//}
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

void CScene::OtherPlayerTargetSetting()
{
	for(int i=0; i<MAX_OTHER_PLAYER; i++)
	{
		if(OtherPlayerManager::sharedManager()->m_pOtherPlayer[i]->GetVisible() != TRUE) continue;

		for(int j=0; j<MAX_TOWER; j++)   //Ÿ������ Ÿ�� üũ
		{
			if(TowerManager::sharedManager()->m_pTower[i]==NULL) continue;
			if(OtherPlayerManager::sharedManager()->m_pOtherPlayer[i]->GetTargetID() ==	TowerManager::sharedManager()->m_pTower[j]->GetID()
				&& OtherPlayerManager::sharedManager()->m_pOtherPlayer[i]->GetTeam() !=	TowerManager::sharedManager()->m_pTower[j]->GetTeam())
			{
				OtherPlayerManager::sharedManager()->m_pOtherPlayer[i]->SetTarget(TowerManager::sharedManager()->m_pTower[j]);
				return;
			}
		}

		//�÷��̾���� Ÿ�� üũ
		if(OtherPlayerManager::sharedManager()->m_pOtherPlayer[i]->GetTargetID() ==	HeroManager::sharedManager()->m_pHero->GetID()
			&& OtherPlayerManager::sharedManager()->m_pOtherPlayer[i]->GetTeam() != HeroManager::sharedManager()->m_pHero->GetTeam())              
		{
			OtherPlayerManager::sharedManager()->m_pOtherPlayer[i]->SetTarget(HeroManager::sharedManager()->m_pHero);
			return;
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