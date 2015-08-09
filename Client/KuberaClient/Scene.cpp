#include "Scene.h"
#include "LoadManager.h"
#include "MinionManager.h"
#include "SoundManager.h"
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

	
	m_fMissileAttackTime = 0.f;

	m_pPlane = NULL;
	m_pBlueNexus = NULL;
	m_pRedNexus = NULL;
	m_pDestroyNexus = NULL;
	m_pWarriorMesh = NULL;
	m_pWizardMesh = NULL;

	m_bGameOver = FALSE;

	m_particleEnableBlendingState = 0;
	m_particleDisableBlendingState = 0;

	//조명
	m_pLights = NULL;
	m_pd3dcbLights = NULL;

	//ui
	m_pUIShaders = NULL;

	m_UIskillbarWidth = 329.4289f, m_UIskillbarHeight = 156.25f;
	m_UIMinimapWidth= 197.6573f, m_UIMinimapHeight = 234.375f;
	m_UIInfoWidth = 197.6573f, m_UIInfoHeight = 156.25f;
	m_UIScoreWidth = 131.7715f, m_UIScoreHeight = 78.125f;
	m_UITargetInfoWidth = 197.6573f, m_UITargetInfoHeight = 156.25f;

	m_SwordWidth = 20, m_SwordHeight = 20;
	m_ShielWidth = 20, m_ShielHeight = 20;
	m_BootsWidth = 20, m_BootsHeight = 20;

	m_HpbarRWidth = 250, m_HpbarRHeight = 15;
	m_HpbarGWidth = 250, m_HpbarGHeight = 15;
	m_Hpbar = 250;

	m_TargetHpbarRWidth = 95, m_TargetHpbarRHeight = 10; //타겟 hp바
	m_TargetHpbarGWidth = 95, m_TargetHpbarGHeight = 10;
	m_TargetHpbar = 95;

	m_CharacterFaceWidth = 47, m_CharacterFaceHeight = 80;
	m_Skillq_buttonWidth = 25, m_Skillq_buttonHeight = 45;
	m_Skillw_buttonWidth = 25, m_Skillw_buttonHeight = 45; //스킬 버튼
	m_Skille_buttonWidth = 25, m_Skille_buttonHeight = 45; //스킬 버튼
	m_Skillr_buttonWidth = 25, m_Skillr_buttonHeight = 45; //스킬 버튼

	m_UpgradeSwordWidth = 23, m_UpgradeSwordHeight = 40;
	m_UpgradeShieldWidth = 23, m_UpgradeShieldHeight = 40;
	m_UpgradeBootsWidth = 23, m_UpgradeBootsHeight = 40;

	m_UpgradeButtonWidth= 60, m_UpgradeButtonHeight = 20; 
	m_UpgradeButtonNumberWidth = 10, m_UpgradeButtonNumberHeight = 18; //업그레이드 버튼 숫자
	m_GameWinLoseWidth = 300, m_GameWinLoseHeight = 200;

	m_nPrevWndClientWidth = 900;
	m_nPrevWndClientHeight = 600;

	m_fGameTimeSecond = 0.0f;
	m_fGameTimeMinute = 0.0f;
	m_bChangeImage = FALSE;
	m_bChangeHpbar = FALSE;
	m_bUIReset = FALSE;
}


CScene::~CScene(void)
{

}

void CScene::BuildObjects(ID3D11Device *pd3dDevice)
{
	m_Control.m_Camera = m_Camera;
	CreateBlend(pd3dDevice);

	//이 쉐이더 객체에 대한 포인터들의 배열을 정의한다.
	m_pObjectShaders = LoadManager::sharedManager()->m_pObjectShaders;
	m_pInstancingShaders = LoadManager::sharedManager()->m_pInstancingShaders;
	//m_pInstancingShaders->BuildObjects(pd3dDevice);   //이거 때메 터졌던거
	m_pAnimationShaders = LoadManager::sharedManager()->m_pAnimationShaders;
	m_pParticleShaders = LoadManager::sharedManager()->m_pParticleShaders;

	//게임 객체에 대한 포인터들의 배열을 정의한다
	m_nIntanceObjects = LoadManager::sharedManager()->m_nIntanceObjects;
	m_nObjects = LoadManager::sharedManager()->m_nObjects;

	//워리어 메쉬
	m_pWarriorMesh = LoadManager::sharedManager()->m_pVarianMesh;

	//위자드 메쉬
	m_pWizardMesh = LoadManager::sharedManager()->m_pMageMesh;

	//재질설정
	CMaterial *pMaterial = new CMaterial();
	pMaterial->AddRef();
	pMaterial->m_Material.m_d3dxcDiffuse = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.0f);//D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pMaterial->m_Material.m_d3dxcAmbient = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.0f);//D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pMaterial->m_Material.m_d3dxcSpecular = D3DXCOLOR(0.1f, 0.1f, 0.1f, 5.0f);// D3DXCOLOR(1.0f, 1.0f, 1.0f, 5.0f);
	pMaterial->m_Material.m_d3dxcEmissive = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
	
	//히어로 생성
	HeroManager::sharedManager()->CreateHero(LoadManager::sharedManager()->m_pVarianMesh, LoadManager::sharedManager()->m_pMageMesh, 10, 13, 10);
	HeroManager::sharedManager()->m_pHero->SetMaterial(pMaterial);
	if(HeroManager::sharedManager()->m_pHero->GetType() == 1)
		HeroManager::sharedManager()->m_pHero->SetFaceType(HERO1_FACE);
	else if(HeroManager::sharedManager()->m_pHero->GetType() == 2)
		HeroManager::sharedManager()->m_pHero->SetFaceType(HERO2_FACE);

	//바닥 생성
	m_pPlane = new CGameObject();
	m_pPlane->SetMesh(LoadManager::sharedManager()->pPlaneMesh);
	m_pPlane->SetMaterial(pMaterial);


	//넥서스 2개 생성
	m_pBlueNexus = LoadManager::sharedManager()->m_pBlueNexus;
	m_pRedNexus = LoadManager::sharedManager()->m_pRedNexus;
	m_pDestroyNexus = LoadManager::sharedManager()->m_pDestroyNexus;

	HeroManager::sharedManager()->SetNexus(m_pRedNexus, m_pBlueNexus);

	//pFBXMesh->Release();

	//파티클 메쉬
	//m_pParticleMesh = LoadManager::sharedManager()->m_pParticleMesh;
	//m_pParticle2Mesh = LoadManager::sharedManager()->m_pParticle2Mesh;
	//m_pParticle3Mesh = LoadManager::sharedManager()->m_pParticle3Mesh;

 	//애니메이션 쉐이더에 객체 연결
 	m_pAnimationShaders->AddObject(HeroManager::sharedManager()->m_pHero);
	m_pAnimationShaders->AddObject(m_pPlane);
	m_pAnimationShaders->AddObject(m_pBlueNexus);
	m_pAnimationShaders->AddObject(m_pRedNexus);
	m_pAnimationShaders->AddObject(m_pDestroyNexus);
	

	//아더 플레이어 생성
	OtherPlayerManager::sharedManager()->SetMesh(m_pWarriorMesh, m_pWizardMesh);
	OtherPlayerManager::sharedManager()->CreateOtherPlayer(D3DXVECTOR3(0, 0, 0), 10, 13, 10);
	OtherPlayerManager::sharedManager()->m_pOtherPlayer->SetMaterial(pMaterial);

	if(OtherPlayerManager::sharedManager()->m_pOtherPlayer->GetType() == 1)
		OtherPlayerManager::sharedManager()->m_pOtherPlayer->SetFaceType(HERO1_FACE);
	else if(OtherPlayerManager::sharedManager()->m_pOtherPlayer->GetType() == 2)
		OtherPlayerManager::sharedManager()->m_pOtherPlayer->SetFaceType(HERO2_FACE);
	m_pAnimationShaders->AddObject(OtherPlayerManager::sharedManager()->m_pOtherPlayer);

	//미니언 생성
	MinionManager::sharedManager()->CreateMinion(D3DXVECTOR3(100, 0, 0), LoadManager::sharedManager()->m_pCannonGolemMesh, 50, 10, 25);
	MinionManager::sharedManager()->CreateMinion(D3DXVECTOR3(-100, 0, 0), LoadManager::sharedManager()->m_pCannonGolemMesh, 50, 10, 25);
	MinionManager::sharedManager()->CreateMinion(D3DXVECTOR3(-400, 0, 160), LoadManager::sharedManager()->m_pCleftMesh, 50, 10, 25);
	MinionManager::sharedManager()->CreateMinion(D3DXVECTOR3(-400, 0, -160), LoadManager::sharedManager()->m_pTurtleMesh, 50, 10, 25);
	MinionManager::sharedManager()->CreateMinion(D3DXVECTOR3(400, 0, 160), LoadManager::sharedManager()->m_pCleftMesh, 50, 10, 25);
	MinionManager::sharedManager()->CreateMinion(D3DXVECTOR3(400, 0, -160), LoadManager::sharedManager()->m_pTurtleMesh, 50, 10, 25);

	MinionManager::sharedManager()->m_pMinion[0]->SetType(CANNONGOLEM);
	MinionManager::sharedManager()->m_pMinion[0]->SetFaceType(GOLEM_FACE);
	MinionManager::sharedManager()->m_pMinion[1]->SetType(CANNONGOLEM);
	MinionManager::sharedManager()->m_pMinion[1]->SetFaceType(GOLEM_FACE);
	MinionManager::sharedManager()->m_pMinion[2]->SetType(CLEFT);
	MinionManager::sharedManager()->m_pMinion[2]->SetFaceType(CLEFT_FACE);
	MinionManager::sharedManager()->m_pMinion[3]->SetType(TURTLE);
	MinionManager::sharedManager()->m_pMinion[3]->SetFaceType(TURTLE_FACE);
	MinionManager::sharedManager()->m_pMinion[4]->SetType(CLEFT);
	MinionManager::sharedManager()->m_pMinion[4]->SetFaceType(CLEFT_FACE);
	MinionManager::sharedManager()->m_pMinion[5]->SetType(TURTLE);
	MinionManager::sharedManager()->m_pMinion[5]->SetFaceType(TURTLE_FACE);
	for(int i=0; i< MAX_MINION; i++)
	{
		if(MinionManager::sharedManager()->m_pMinion[i] == NULL) continue;
		MinionManager::sharedManager()->m_pMinion[i]->SetMaterial(pMaterial);
		MinionManager::sharedManager()->m_pMinion[i]->SetWayPoint(MinionManager::sharedManager()->m_pMinion[i]->GetPosition());
		//MinionManager::sharedManager()->m_pMinion[i]->SetScale(D3DXVECTOR3(0.1, 0.1, 0.1));
		m_pAnimationShaders->AddObject(MinionManager::sharedManager()->m_pMinion[i]);
	}
	
	this->AddOtherPlayer(pd3dDevice);


	//파티클 쉐이더에 파티클 연결
	for(int i=0; i<MAX_PARTICLE; i++)
	{
		if(ParticleManager::sharedManager()->m_pParticle[i] == NULL) continue;
		m_pParticleShaders->AddObject(ParticleManager::sharedManager()->m_pParticle[i]);
	}


	//미니맵에 사용할 큐브 객체 생성
	//미니맵 넥서스
	CCubeMesh* MyNexusBox = new CCubeMesh(pd3dDevice, 40, 10, 40, D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f));
	CCubeMesh* YourNexusBox = new CCubeMesh(pd3dDevice, 40, 10, 40, D3DXCOLOR(1.0f, 0.0f, 0.0f, 0.0f));
	for(int i=0;i<2; i++)
		m_pNexusBox[i] = new CGameObject();
	m_pNexusBox[0]->SetMesh(MyNexusBox);
	m_pNexusBox[1]->SetMesh(YourNexusBox);
	if(HeroManager::sharedManager()->m_pHero->GetTeam() == BLUE_TEAM)
	{
		m_pNexusBox[0]->SetPosition(m_pBlueNexus->GetPosition());
		m_pNexusBox[1]->SetPosition(m_pRedNexus->GetPosition());
	}
	else if(HeroManager::sharedManager()->m_pHero->GetTeam() == RED_TEAM)
	{
		m_pNexusBox[0]->SetPosition(m_pRedNexus->GetPosition());
		m_pNexusBox[1]->SetPosition(m_pBlueNexus->GetPosition());
	}
	//미니맵 타워
	CCubeMesh* MyTowerBox = new CCubeMesh(pd3dDevice, 25, 10, 25, D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f));
	CCubeMesh* YourTowerBox = new CCubeMesh(pd3dDevice, 25, 10, 25, D3DXCOLOR(1.0f, 0.0f, 0.0f, 0.0f));
	for(int i=0;i<MAX_TOWER;i++)
		m_pTowerBox[i] = new CGameObject();
	for(int i=0; i<MAX_TOWER; i++)
	{
		if(TowerManager::sharedManager()->m_pTower[i] == NULL) continue;

		if(TowerManager::sharedManager()->m_pTower[i]->GetTeam() == HeroManager::sharedManager()->m_pHero->GetTeam())
		{
			m_pTowerBox[i]->SetMesh(MyTowerBox);
			m_pTowerBox[i]->SetPosition(TowerManager::sharedManager()->m_pTower[i]->GetPosition());
		}
		else if(TowerManager::sharedManager()->m_pTower[i]->GetTeam() != HeroManager::sharedManager()->m_pHero->GetTeam())
		{
			m_pTowerBox[i]->SetMesh(YourTowerBox);
			m_pTowerBox[i]->SetPosition(TowerManager::sharedManager()->m_pTower[i]->GetPosition());
		}
	}
	//미니맵 플레이어
	CCubeMesh* MyPlayerBox = new CCubeMesh(pd3dDevice, 25, 10, 25, D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f));
	CCubeMesh* YourPlayerBox = new CCubeMesh(pd3dDevice, 25, 10, 25, D3DXCOLOR(1.0f, 0.0f, 0.0f, 0.0f));
	for(int i=0; i<2;i++)
		m_pPlayerBox[i] = new CGameObject();
	m_pPlayerBox[0]->SetMesh(MyPlayerBox);
	m_pPlayerBox[0]->SetPosition(HeroManager::sharedManager()->m_pHero->GetPosition());
	m_pPlayerBox[1]->SetMesh(YourPlayerBox);
	m_pPlayerBox[1]->SetPosition(OtherPlayerManager::sharedManager()->m_pOtherPlayer->GetPosition());
	
	//미니맵 몬스터
	CCubeMesh* MinionBox = new CCubeMesh(pd3dDevice, 25, 10, 25, D3DXCOLOR(1.0f, 1.0f, 0.0f, 0.0f));
	for(int i=0; i<MAX_MINION; i++)
	{
		m_pMinionBox[i] = new CGameObject();
		m_pMinionBox[i]->SetMesh(MinionBox);
		m_pMinionBox[i]->SetPosition(MinionManager::sharedManager()->m_pMinion[i]->GetPosition());
	}
	//오브젝트 쉐이더에 객체 연결
	for(int i=0;i<2;i++)
		m_pObjectShaders->AddObject(m_pNexusBox[i]);
	for(int i=0;i<MAX_TOWER;i++)
	{
		if(TowerManager::sharedManager()->m_pTower[i] == NULL) continue;
		m_pObjectShaders->AddObject(m_pTowerBox[i]);
	}
	for(int i=0;i<2;i++)
		m_pObjectShaders->AddObject(m_pPlayerBox[i]);
	for(int i=0; i<MAX_MINION; i++)
		m_pObjectShaders->AddObject(m_pMinionBox[i]);
	
	//조명 생성
	CreateLightShaderVariables(pd3dDevice);

	SoundManager::sharedManager()->play(SOUND_MAIN_BGM);


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
			m_nWndClientWidth = LOWORD(lParam);
			m_nWndClientHeight = HIWORD(lParam);

			m_UIskillbarWidth = (m_nWndClientWidth*m_UIskillbarWidth) / m_nPrevWndClientWidth;
			m_UIskillbarHeight = (m_nWndClientHeight*m_UIskillbarHeight) / m_nPrevWndClientHeight; 
			m_UIMinimapWidth = (m_nWndClientWidth*m_UIMinimapWidth) / m_nPrevWndClientWidth;
			m_UIMinimapHeight = (m_nWndClientHeight*m_UIMinimapHeight) / m_nPrevWndClientHeight; 
			m_UIInfoWidth = (m_nWndClientWidth*m_UIInfoWidth) / m_nPrevWndClientWidth;
			m_UIInfoHeight = (m_nWndClientHeight*m_UIInfoHeight) / m_nPrevWndClientHeight; 
			m_UIScoreWidth = (m_nWndClientWidth*m_UIScoreWidth) / m_nPrevWndClientWidth;
			m_UIScoreHeight = (m_nWndClientHeight*m_UIScoreHeight) / m_nPrevWndClientHeight; 
			m_UITargetInfoWidth = (m_nWndClientWidth*m_UITargetInfoWidth) / m_nPrevWndClientWidth;
			m_UITargetInfoHeight = (m_nWndClientHeight*m_UITargetInfoHeight) / m_nPrevWndClientHeight; 

			m_SwordWidth = (m_nWndClientWidth*m_SwordWidth) / m_nPrevWndClientWidth;
			m_SwordHeight = (m_nWndClientHeight*m_SwordHeight) / m_nPrevWndClientHeight; 
			m_ShielWidth = (m_nWndClientWidth*m_ShielWidth) / m_nPrevWndClientWidth;
			m_ShielHeight = (m_nWndClientHeight*m_ShielHeight) / m_nPrevWndClientHeight; 
			m_BootsWidth = (m_nWndClientWidth*m_BootsWidth) / m_nPrevWndClientWidth;
			m_BootsHeight = (m_nWndClientHeight*m_BootsHeight) / m_nPrevWndClientHeight; 

			m_HpbarRWidth = (m_nWndClientWidth*m_HpbarRWidth) / m_nPrevWndClientWidth;
			m_HpbarRHeight = (m_nWndClientHeight*m_HpbarRHeight) / m_nPrevWndClientHeight; 
			m_HpbarGWidth = (m_nWndClientWidth*m_HpbarGWidth) / m_nPrevWndClientWidth;
			m_HpbarGHeight = (m_nWndClientHeight*m_HpbarGHeight) / m_nPrevWndClientHeight; 

			m_TargetHpbarRWidth = (m_nWndClientWidth*m_TargetHpbarRWidth) / m_nPrevWndClientWidth;
			m_TargetHpbarRHeight = (m_nWndClientHeight*m_TargetHpbarRHeight) / m_nPrevWndClientHeight; 
			m_TargetHpbarGWidth = (m_nWndClientWidth*m_TargetHpbarGWidth) / m_nPrevWndClientWidth;
			m_TargetHpbarGHeight = (m_nWndClientHeight*m_TargetHpbarGHeight) / m_nPrevWndClientHeight;

			m_CharacterFaceWidth = (m_nWndClientWidth*m_CharacterFaceWidth) / m_nPrevWndClientWidth;
			m_CharacterFaceHeight = (m_nWndClientHeight*m_CharacterFaceHeight) / m_nPrevWndClientHeight; 
			m_Skillq_buttonWidth = (m_nWndClientWidth*m_Skillq_buttonWidth) / m_nPrevWndClientWidth;
			m_Skillq_buttonHeight = (m_nWndClientHeight*m_Skillq_buttonHeight) / m_nPrevWndClientHeight; 
			m_Skillw_buttonWidth = (m_nWndClientWidth*m_Skillw_buttonWidth) / m_nPrevWndClientWidth;
			m_Skillw_buttonHeight = (m_nWndClientHeight*m_Skillw_buttonHeight) / m_nPrevWndClientHeight; 
			m_Skille_buttonWidth = (m_nWndClientWidth*m_Skille_buttonWidth) / m_nPrevWndClientWidth;
			m_Skille_buttonHeight = (m_nWndClientHeight*m_Skille_buttonHeight) / m_nPrevWndClientHeight; 
			m_Skillr_buttonWidth = (m_nWndClientWidth*m_Skillr_buttonWidth) / m_nPrevWndClientWidth;
			m_Skillr_buttonHeight = (m_nWndClientHeight*m_Skillr_buttonHeight) / m_nPrevWndClientHeight; 

			m_UpgradeSwordWidth = (m_nWndClientWidth*m_UpgradeSwordWidth) / m_nPrevWndClientWidth;
			m_UpgradeSwordHeight = (m_nWndClientHeight*m_UpgradeSwordHeight) / m_nPrevWndClientHeight; 
			m_UpgradeShieldWidth = (m_nWndClientWidth*m_UpgradeShieldWidth) / m_nPrevWndClientWidth;
			m_UpgradeShieldHeight = (m_nWndClientHeight*m_UpgradeShieldHeight) / m_nPrevWndClientHeight; 
			m_UpgradeBootsWidth = (m_nWndClientWidth*m_UpgradeBootsWidth) / m_nPrevWndClientWidth;
			m_UpgradeBootsHeight = (m_nWndClientHeight*m_UpgradeBootsHeight) / m_nPrevWndClientHeight; 

			m_UpgradeButtonWidth = (m_nWndClientWidth*m_UpgradeButtonWidth) / m_nPrevWndClientWidth;
			m_UpgradeButtonHeight = (m_nWndClientHeight*m_UpgradeButtonHeight) / m_nPrevWndClientHeight; 
			m_UpgradeButtonNumberWidth = (m_nWndClientWidth*m_UpgradeButtonNumberWidth) / m_nPrevWndClientWidth;
			m_UpgradeButtonNumberHeight = (m_nWndClientHeight*m_UpgradeButtonNumberHeight) / m_nPrevWndClientHeight; 

			m_GameWinLoseWidth = (m_nWndClientWidth*m_GameWinLoseWidth) / m_nPrevWndClientWidth;
			m_GameWinLoseHeight = (m_nWndClientHeight*m_GameWinLoseHeight) / m_nPrevWndClientHeight; 


			m_nPrevWndClientWidth = m_nWndClientWidth;
			m_nPrevWndClientHeight = m_nWndClientHeight;

			for(int i=0; i<MAX_UI; i++)
			{
				if(m_pUIObjects[i]->GetUI()->GetScreenW() != m_nWndClientWidth || m_pUIObjects[i]->GetUI()->GetScreenH() != m_nWndClientHeight)
					m_pUIObjects[i]->GetUI()->SetScreenWH(m_nWndClientWidth, m_nWndClientHeight);
			}
			m_pUI[0]->SetBitmapWH(m_UIskillbarWidth, m_UIskillbarHeight);
			m_pUI[1]->SetBitmapWH(m_UIMinimapWidth, m_UIMinimapHeight);
			m_pUI[2]->SetBitmapWH(m_UIInfoWidth, m_UIInfoHeight);
			m_pUI[3]->SetBitmapWH(m_UIScoreWidth, m_UIScoreHeight);
			m_pUI[4]->SetBitmapWH(m_SwordWidth, m_SwordHeight);
			m_pUI[5]->SetBitmapWH(m_ShielWidth, m_ShielHeight);
			m_pUI[6]->SetBitmapWH(m_BootsWidth, m_BootsHeight);
			m_pUI[7]->SetBitmapWH(m_HpbarRWidth, m_HpbarRHeight);
			m_pUI[8]->SetBitmapWH(m_HpbarGWidth, m_HpbarGHeight);
			m_pUI[9]->SetBitmapWH(m_CharacterFaceWidth, m_CharacterFaceHeight);
			m_pUI[10]->SetBitmapWH(m_Skillq_buttonWidth, m_Skillq_buttonHeight);
			m_pUI[11]->SetBitmapWH(m_Skillw_buttonWidth, m_Skillw_buttonHeight);
			m_pUI[12]->SetBitmapWH(m_Skille_buttonWidth, m_Skille_buttonHeight);
			m_pUI[13]->SetBitmapWH(m_Skillr_buttonWidth, m_Skillr_buttonHeight);
			m_pUI[14]->SetBitmapWH(m_UpgradeSwordWidth, m_UpgradeSwordHeight);
			m_pUI[15]->SetBitmapWH(m_UpgradeShieldWidth, m_UpgradeShieldHeight);
			m_pUI[16]->SetBitmapWH(m_UpgradeBootsWidth, m_UpgradeBootsHeight);
			m_pUI[17]->SetBitmapWH(m_UITargetInfoWidth, m_UITargetInfoHeight);
			m_pUI[18]->SetBitmapWH(m_SwordWidth, m_SwordHeight);
			m_pUI[19]->SetBitmapWH(m_ShielWidth, m_ShielHeight);
			m_pUI[20]->SetBitmapWH(m_BootsWidth, m_BootsHeight);
			m_pUI[21]->SetBitmapWH(m_CharacterFaceWidth, m_CharacterFaceHeight);
			m_pUI[22]->SetBitmapWH(m_TargetHpbarRWidth, m_TargetHpbarRHeight);
			m_pUI[23]->SetBitmapWH(m_TargetHpbarGWidth, m_TargetHpbarGHeight);
			m_pUI[24]->SetBitmapWH(m_UpgradeButtonWidth, m_UpgradeButtonHeight);
			m_pUI[25]->SetBitmapWH(m_UpgradeButtonNumberWidth, m_UpgradeButtonNumberHeight);
			m_pUI[26]->SetBitmapWH(m_UpgradeButtonNumberWidth, m_UpgradeButtonNumberHeight);
			m_pUI[27]->SetBitmapWH(m_UpgradeButtonNumberWidth, m_UpgradeButtonNumberHeight);
			m_pUI[28]->SetBitmapWH(m_GameWinLoseWidth, m_GameWinLoseHeight);
			m_pUI[29]->SetBitmapWH(m_GameWinLoseWidth, m_GameWinLoseHeight);
			m_pUI[30]->SetBitmapWH(m_GameWinLoseWidth, m_GameWinLoseHeight);
			break;
		}
	case WM_LBUTTONDOWN:
		if(ST::sharedManager()->m_bStart == FALSE) return true;
		m_MousePosX = (INT)LOWORD(lParam); 
		m_MousePosY = (INT)HIWORD(lParam);
		m_Control.TouchLeftDown((float)m_MousePosX, (float)m_MousePosY, hWnd);
		m_bLbutton = TRUE;
		break;
	case WM_RBUTTONDOWN:
		if(ST::sharedManager()->m_bStart == FALSE) return true;
		m_MousePosX = (INT)LOWORD(lParam); 
		m_MousePosY = (INT)HIWORD(lParam);
		m_Control.TouchRightDown((float)m_MousePosX, (float)m_MousePosY, hWnd);
		m_bRbutton = TRUE;
		break;
	case WM_RBUTTONUP:
		if(ST::sharedManager()->m_bStart == FALSE) return true;
		m_bRbutton = FALSE;
		
		break;
	case WM_LBUTTONUP:
		if(ST::sharedManager()->m_bStart == FALSE) return true;
		m_bLbutton = FALSE;
		break;
	case WM_MOUSEMOVE:
		if(ST::sharedManager()->m_bStart== FALSE)
			return true;
		m_MousePosX = (INT)LOWORD(lParam); 
		m_MousePosY = (INT)HIWORD(lParam);

		if(m_bRbutton == TRUE)
			m_Control.TouchRightDown((float)m_MousePosX, (float)m_MousePosY, hWnd);
		OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
		break;
	case WM_KEYDOWN:
		switch (wParam) 
		{
		case 'Q':
			HeroManager::sharedManager()->m_pHero->SetState(SKILL1);
			break;
		case '1':
			if(HeroManager::sharedManager()->m_pHero->GetUpgradePossible() == TRUE)
			{
				HeroManager::sharedManager()->m_pHero->SetDamageLevel(HeroManager::sharedManager()->m_pHero->GetDamageLevel() + 1);
				HeroManager::sharedManager()->m_pHero->DamageUp(HeroManager::sharedManager()->m_pHero->GetDamageLevel());
				HeroManager::sharedManager()->m_pHero->SetUpgradePossible(FALSE);
			}
			break;
		case '2':
			if(HeroManager::sharedManager()->m_pHero->GetUpgradePossible() == TRUE)
			{
				HeroManager::sharedManager()->m_pHero->SetDefenseLevel(HeroManager::sharedManager()->m_pHero->GetDefenseLevel() + 1);
				HeroManager::sharedManager()->m_pHero->DefenseUp(HeroManager::sharedManager()->m_pHero->GetDefenseLevel());
				HeroManager::sharedManager()->m_pHero->SetUpgradePossible(FALSE);
			}
			break;
		case '3':
			if(HeroManager::sharedManager()->m_pHero->GetUpgradePossible() == TRUE)
			{
				HeroManager::sharedManager()->m_pHero->SetSpeedLevel(HeroManager::sharedManager()->m_pHero->GetSpeedLevel() + 1);
				HeroManager::sharedManager()->m_pHero->SpeedUp(HeroManager::sharedManager()->m_pHero->GetSpeedLevel());
				HeroManager::sharedManager()->m_pHero->SetUpgradePossible(FALSE);
			}
			break;
		}

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
	//게임시간
	m_fGameTimeSecond += fTimeElapsed;
	if(m_fGameTimeSecond > 59.99f)
	{
		m_fGameTimeSecond = 0.0f;
		m_fGameTimeMinute++;
	}
	if(m_fGameTimeMinute >= 15)
		m_bGameOver = TRUE;

	//조명
	if(m_pLights && m_pd3dcbLights) 
	{
		m_pLights->m_d3dxvCameraPosition =  D3DXVECTOR4(m_Camera->m_CameraPos, 1.0f);

		//히어로 조명
		m_pLights->m_pLights[0].m_d3dxvPosition = HeroManager::sharedManager()->m_pHero->GetPosition() + D3DXVECTOR3(0, 350, 0);
	}

	//ui 타겟 얼굴 설정/
	if(HeroManager::sharedManager()->m_pHero->GetTarget() != NULL)
	{
		if(m_iPrevFaceType != HeroManager::sharedManager()->m_pHero->GetTarget()->GetFaceType())
			m_bChangeImage = FALSE;

		if(HeroManager::sharedManager()->m_pHero->GetTarget()->GetFaceType() == CLEFT_FACE && m_bChangeImage == FALSE)
		{
			m_pUIObjects[21]->GetUI()->SetTexture(pd3dDevice, L"UI/cleft_face.png");
			m_bChangeImage = TRUE;
			m_iPrevFaceType = HeroManager::sharedManager()->m_pHero->GetTarget()->GetFaceType();
		}
		else if(HeroManager::sharedManager()->m_pHero->GetTarget()->GetFaceType() == GOLEM_FACE && m_bChangeImage == FALSE)
		{
			m_pUIObjects[21]->GetUI()->SetTexture(pd3dDevice, L"UI/golem_face.png");
			m_bChangeImage = TRUE;
			m_iPrevFaceType = HeroManager::sharedManager()->m_pHero->GetTarget()->GetFaceType();
		}
		else if(HeroManager::sharedManager()->m_pHero->GetTarget()->GetFaceType() ==  TURTLE_FACE && m_bChangeImage == FALSE)
		{
			m_pUIObjects[21]->GetUI()->SetTexture(pd3dDevice, L"UI/turtle_face.png");
			m_bChangeImage = TRUE;
			m_iPrevFaceType = HeroManager::sharedManager()->m_pHero->GetTarget()->GetFaceType();
		}
		else if(HeroManager::sharedManager()->m_pHero->GetTarget()->GetFaceType() ==  NEXUS_FACE && m_bChangeImage == FALSE)
		{
			m_pUIObjects[21]->GetUI()->SetTexture(pd3dDevice, L"UI/nexus_face.png");
			m_bChangeImage = TRUE;
			m_iPrevFaceType = HeroManager::sharedManager()->m_pHero->GetTarget()->GetFaceType();
		}
		else if(HeroManager::sharedManager()->m_pHero->GetTarget()->GetFaceType() ==  TOWER_FACE && m_bChangeImage == FALSE)
		{
			m_pUIObjects[21]->GetUI()->SetTexture(pd3dDevice, L"UI/tower_face.png");
			m_bChangeImage = TRUE;
			m_iPrevFaceType = HeroManager::sharedManager()->m_pHero->GetTarget()->GetFaceType();
		}
		else if(HeroManager::sharedManager()->m_pHero->GetTarget()->GetFaceType() ==  HERO1_FACE && m_bChangeImage == FALSE)
		{
			m_pUIObjects[21]->GetUI()->SetTexture(pd3dDevice, L"UI/varian.png");
			m_bChangeImage = TRUE;
			m_iPrevFaceType = HeroManager::sharedManager()->m_pHero->GetTarget()->GetFaceType();
		}
		else if(HeroManager::sharedManager()->m_pHero->GetTarget()->GetFaceType() ==  HERO2_FACE && m_bChangeImage == FALSE)
		{
			m_pUIObjects[21]->GetUI()->SetTexture(pd3dDevice, L"UI/Coldwraith.png");
			m_bChangeImage = TRUE;
			m_iPrevFaceType = HeroManager::sharedManager()->m_pHero->GetTarget()->GetFaceType();
		}
	}
	else
		m_bChangeImage = FALSE;

	//히어로 
	HeroManager::sharedManager()->m_pHero->Animate(fTimeElapsed);
	HeroManager::sharedManager()->m_pHero->Update(fTimeElapsed);
 

	//for(int i=0; i<MAX_MISSILE; i++)
	//	MissileManager::sharedManager()->m_pMissile[i]->Update(fTimeElapsed);

	for(int i=0; i<MAX_PARTICLE; i++)
	{
		if(ParticleManager::sharedManager()->m_pParticle[i] == NULL) continue;
		//if(ParticleManager::sharedManager()->m_pParticle[i]->GetUsed() == FALSE) continue;
		ParticleManager::sharedManager()->m_pParticle[i]->Update(fTimeElapsed);
	}
	m_pPlayerBox[0]->SetPosition(HeroManager::sharedManager()->m_pHero->GetPosition()); //미니맵 캐릭터 박스 이동
	m_pPlayerBox[1]->SetPosition(OtherPlayerManager::sharedManager()->m_pOtherPlayer->GetPosition());

	for(int i = 0; i < MAX_MINION; i++)
	{
		if(MinionManager::sharedManager()->m_pMinion[i] == NULL) continue;
		MinionManager::sharedManager()->m_pMinion[i]->Animate(fTimeElapsed);
		MinionManager::sharedManager()->m_pMinion[i]->Update(fTimeElapsed);
	}


	for(int i=0;i<MAX_TOWER; i++)  //타워의 캐릭터 공격
	{
		if(TowerManager::sharedManager()->m_pTower[i] == NULL) continue;
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

			
		m_DistanceToOtherPlayer = ST::sharedManager()->GetDistance(OtherPlayerManager::sharedManager()->m_pOtherPlayer->GetPos(), 
			TowerManager::sharedManager()->m_pTower[i]->GetPos()); 
		if(m_DistanceToOtherPlayer < 50.f 
			&& TowerManager::sharedManager()->m_pTower[i]->GetTeam() != OtherPlayerManager::sharedManager()->m_pOtherPlayer->GetTeam())
		{
			TowerManager::sharedManager()->m_pTower[i]->SetTarget(OtherPlayerManager::sharedManager()->m_pOtherPlayer);
		}
		else
		{
			TowerManager::sharedManager()->m_pTower[i]->SetTarget(NULL);
			TowerManager::sharedManager()->m_pTower[i]->SetAttackTime(0.f);
		}
			
		
	}

	OtherPlayerManager::sharedManager()->m_pOtherPlayer->Update(fTimeElapsed);
	OtherPlayerManager::sharedManager()->m_pOtherPlayer->Animate(fTimeElapsed);
	
	
	//넥서스 터질때
	if(m_pBlueNexus->GetHP() <= 0 && m_bGameOver == FALSE)
	{
		m_pDestroyNexus->SetPosition(m_pBlueNexus->GetPosition());
		m_pBlueNexus->SetPosition(D3DXVECTOR3(-2000, 0, 0));
		m_bGameOver = TRUE;
	}
	else if(m_pRedNexus->GetHP() <= 0 && m_bGameOver == FALSE)
	{
		m_pDestroyNexus->SetPosition(m_pRedNexus->GetPosition());
		m_pRedNexus->SetPosition(D3DXVECTOR3(-2000, 0, 0));
		m_bGameOver = TRUE;
	}

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
	GameCollision(HeroManager::sharedManager()->m_pHero, m_pDestroyNexus);


}

void CScene::Render(ID3D11DeviceContext*pd3dDeviceContext, float fTimeElapsed, CCamera *pCamera)
{
	// 조명
	if(m_pLights && m_pd3dcbLights) UpdateLightShaderVariable(pd3dDeviceContext, m_pLights);
	//쉐이더 객체 리스트의 각 쉐이더 객체를 렌더링한다.
	
	//오브젝트 쉐이더 렌더
	if(pCamera->GetMode() == MINIMAP_CAMERA)
	{
		m_pObjectShaders->Render(pd3dDeviceContext);
		for(int i=0;i<2;i++)
		{
			if(m_pNexusBox[i]->IsVisible(pCamera))
				m_pObjectShaders->UpdateShaderVariables(pd3dDeviceContext, &m_pNexusBox[i]->m_d3dxmtxWorld);
			m_pNexusBox[i]->Render(pd3dDeviceContext, pCamera);
		}
		for(int i=0;i<MAX_TOWER;i++)
		{
			if(TowerManager::sharedManager()->m_pTower[i] == NULL) continue;
			if(m_pTowerBox[i]->IsVisible(pCamera))
				m_pObjectShaders->UpdateShaderVariables(pd3dDeviceContext, &m_pTowerBox[i]->m_d3dxmtxWorld);
			m_pTowerBox[i]->Render(pd3dDeviceContext, pCamera);
		}
		for(int i=0;i<2;i++)
		{
			if(m_pPlayerBox[i]->IsVisible(pCamera))
				m_pObjectShaders->UpdateShaderVariables(pd3dDeviceContext, &m_pPlayerBox[i]->m_d3dxmtxWorld);
			m_pPlayerBox[i]->Render(pd3dDeviceContext, pCamera);
		}
		for(int i=0; i<MAX_MINION; i++)
		{
			if(m_pMinionBox[i]->IsVisible(pCamera))
				m_pObjectShaders->UpdateShaderVariables(pd3dDeviceContext, &m_pMinionBox[i]->m_d3dxmtxWorld);
			m_pMinionBox[i]->Render(pd3dDeviceContext, pCamera);
		}
	}


	//인스턴싱 쉐이더 렌더
	m_pInstancingShaders->Render(pd3dDeviceContext, pCamera);

	for(int i=0; i<MAX_MISSILE; i++)
	{
		if(MissileManager::sharedManager()->m_pMissile[i] == NULL) continue;
		MissileManager::sharedManager()->m_pMissile[i]->Render(pd3dDeviceContext, pCamera);
	}

	for(int i=0; i<MAX_OBSTACLE; i++)
	{
		if(ObstacleManager::sharedManager()->m_pObstacle[i] == NULL) continue;
		ObstacleManager::sharedManager()->m_pObstacle[i]->Render(pd3dDeviceContext, pCamera);
	}
  	
	
	if(pCamera->GetMode() == CAMERA)
	{
		for(int i=0; i<MAX_TOWER; i++)
		{
			if(TowerManager::sharedManager()->m_pTower[i] == NULL) continue;
			TowerManager::sharedManager()->m_pTower[i]->Render(pd3dDeviceContext, pCamera);
		}
		for(int i=0; i<MAX_DESTROY_TOWER; i++)
		{
			if(ObstacleManager::sharedManager()->m_pDestroyTower[i] == NULL) continue;
			ObstacleManager::sharedManager()->m_pDestroyTower[i]->Render(pd3dDeviceContext, pCamera);
		}
	}

	//애니메이션 쉐이더 렌더
	m_pAnimationShaders->Render(pd3dDeviceContext);

	if(m_pPlane->IsVisible(pCamera))
		m_pAnimationShaders->UpdateShaderVariables(pd3dDeviceContext, &m_pPlane->m_d3dxmtxWorld);
	m_pPlane->Render(pd3dDeviceContext, pCamera);

	if(pCamera->GetMode() == CAMERA)
	{
		if(HeroManager::sharedManager()->m_pHero->IsVisible(pCamera))
			m_pAnimationShaders->UpdateShaderVariables(pd3dDeviceContext, &HeroManager::sharedManager()->m_pHero->m_d3dxmtxWorld);
		HeroManager::sharedManager()->m_pHero->Render(pd3dDeviceContext, fTimeElapsed, pCamera);

		if(m_pBlueNexus->IsVisible(pCamera))
			m_pAnimationShaders->UpdateShaderVariables(pd3dDeviceContext, &m_pBlueNexus->m_d3dxmtxWorld);
		m_pBlueNexus->Render(pd3dDeviceContext, pCamera);

		if(m_pRedNexus->IsVisible(pCamera))
			m_pAnimationShaders->UpdateShaderVariables(pd3dDeviceContext, &m_pRedNexus->m_d3dxmtxWorld);
		m_pRedNexus->Render(pd3dDeviceContext, pCamera);

		if(m_pDestroyNexus->IsVisible(pCamera))
			m_pAnimationShaders->UpdateShaderVariables(pd3dDeviceContext, &m_pDestroyNexus->m_d3dxmtxWorld);
		m_pDestroyNexus->Render(pd3dDeviceContext, pCamera);

		for(int i=0; i<MAX_MINION; i++)
		{
			if(MinionManager::sharedManager()->m_pMinion[i] == NULL) continue;

			if(MinionManager::sharedManager()->m_pMinion[i]->IsVisible(pCamera))
				m_pAnimationShaders->UpdateShaderVariables(pd3dDeviceContext, &MinionManager::sharedManager()->m_pMinion[i]->m_d3dxmtxWorld);
			MinionManager::sharedManager()->m_pMinion[i]->Render(pd3dDeviceContext, fTimeElapsed, pCamera);
		}

		if(OtherPlayerManager::sharedManager()->m_pOtherPlayer->IsVisible(pCamera))
			m_pAnimationShaders->UpdateShaderVariables(pd3dDeviceContext, &OtherPlayerManager::sharedManager()->m_pOtherPlayer->m_d3dxmtxWorld);
		OtherPlayerManager::sharedManager()->m_pOtherPlayer->Render(pd3dDeviceContext, fTimeElapsed, pCamera);
	}

	


	if(pCamera->GetMode() == CAMERA)
	{
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
	}
	LoadManager::sharedManager()->m_pParticleMesh->SetCamVec(*m_Camera->GetWorldRight(), *m_Camera->GetWorldUp());
	//LoadManager::sharedManager()->m_pParticle2Mesh->SetCamVec(*m_Camera->GetWorldRight(), *m_Camera->GetWorldUp());
	//LoadManager::sharedManager()->m_pParticleWizardSkillMesh->SetCamVec(*m_Camera->GetWorldRight(), *m_Camera->GetWorldUp());

	//LoadManager::sharedManager()->m_pParticle3Mesh->SetCamVec(*m_Camera->GetWorldRight(), *m_Camera->GetWorldUp());
	//m_pParticleMesh->SetCamVec(D3DXVECTOR3(m_Camera->GetPitch(), 0, 0), D3DXVECTOR3(0, 0, m_Camera->GetRoll()));
	//m_pParticle2Mesh->SetCamVec(D3DXVECTOR3(m_Camera->GetPitch(), 0, 0), D3DXVECTOR3(0, 0, m_Camera->GetRoll()));
	//m_pParticle3Mesh->SetCamVec(D3DXVECTOR3(m_Camera->GetPitch(), 0, 0), D3DXVECTOR3(0, 0, m_Camera->GetRoll()));
	//m_pParticleMesh->SetCamVec(m_Camera->GetRightVector(), m_Camera->GetUpVector());
	//m_pParticle2Mesh->SetCamVec(m_Camera->GetRightVector(), m_Camera->GetUpVector());
	//m_pParticle3Mesh->SetCamVec(m_Camera->GetRightVector(), m_Camera->GetUpVector());


	//ui
	SetUIHpUpdate();

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

void CScene::SetOtherClient(PlayerPacket _PI)
{

}

void CScene::UpdateOtherClient(PlayerPacket* _PI)
{
	printf("2 : X: %d, Z:%d, STATE : %d, ROT : %.3f \n", _PI->PI.m_Data.m_Pos.x, _PI->PI.m_Data.m_Pos.z, _PI->PI.m_iState, _PI->PI.m_Data.m_Rot);
	
	D3DXVECTOR3 q;
	q.x = _PI->PI.m_Data.m_Pos.x;
	q.y = _PI->PI.m_Data.m_Pos.y;
	q.z = _PI->PI.m_Data.m_Pos.z;
	OtherPlayerManager::sharedManager()->m_pOtherPlayer->SetNewDestination(q);
	OtherPlayerManager::sharedManager()->m_pOtherPlayer->SetRot(_PI->PI.m_Data.m_Rot);
	OtherPlayerManager::sharedManager()->m_pOtherPlayer->SetState(_PI->PI.m_iState);
	OtherPlayerManager::sharedManager()->m_pOtherPlayer->SetTargetID(_PI->PI.m_iTargetID);
	OtherPlayerManager::sharedManager()->m_pOtherPlayer->SetHP(_PI->PI.m_Data.m_HP);
	OtherPlayerManager::sharedManager()->m_pOtherPlayer->SetDamage(_PI->PI.m_Data.m_Damage);
	OtherPlayerManager::sharedManager()->m_pOtherPlayer->SetDefense(_PI->PI.m_Data.m_Defence);
	OtherPlayerManager::sharedManager()->m_pOtherPlayer->SetSpeed(_PI->PI.m_Data.m_Speed);
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


void CScene::OtherPlayerTargetSetting()
{
	for(int j=0; j<MAX_TOWER; j++)   //타워와의 타겟 체크
	{
		if(TowerManager::sharedManager()->m_pTower[j] == NULL) continue;
		if(OtherPlayerManager::sharedManager()->m_pOtherPlayer->GetTargetID() ==	TowerManager::sharedManager()->m_pTower[j]->GetID()
			&& OtherPlayerManager::sharedManager()->m_pOtherPlayer->GetTeam() !=	TowerManager::sharedManager()->m_pTower[j]->GetTeam())
		{
			OtherPlayerManager::sharedManager()->m_pOtherPlayer->SetTarget(TowerManager::sharedManager()->m_pTower[j]);
			return;
		}
	}

	//플레이어와의 타겟 체크
	if(OtherPlayerManager::sharedManager()->m_pOtherPlayer->GetTargetID() ==	HeroManager::sharedManager()->m_pHero->GetID()
		&& OtherPlayerManager::sharedManager()->m_pOtherPlayer->GetTeam() != HeroManager::sharedManager()->m_pHero->GetTeam())              
	{
		OtherPlayerManager::sharedManager()->m_pOtherPlayer->SetTarget(HeroManager::sharedManager()->m_pHero);
		return;
	}

	if(OtherPlayerManager::sharedManager()->m_pOtherPlayer->GetTargetID() == m_pBlueNexus->GetID())
	{
		OtherPlayerManager::sharedManager()->m_pOtherPlayer->SetTarget(m_pBlueNexus);
		return;
	}

	if(OtherPlayerManager::sharedManager()->m_pOtherPlayer->GetTargetID() == m_pRedNexus->GetID())
	{
		OtherPlayerManager::sharedManager()->m_pOtherPlayer->SetTarget(m_pRedNexus);
		return;
	}
		
	OtherPlayerManager::sharedManager()->m_pOtherPlayer->SetTarget(NULL);
	
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
		if(TowerManager::sharedManager()->m_pTower[i] == NULL) continue;
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


void CScene::CreateUI(ID3D11Device *pd3dDevice,int  _wndWidth,int  _wndHeight)
{
	m_nWndClientWidth = _wndWidth;
	m_nWndClientHeight = _wndHeight;
	m_nPrevWndClientWidth = m_nWndClientWidth;
	m_nPrevWndClientHeight = m_nWndClientHeight;

	m_pUIShaders = LoadManager::sharedManager()->m_pUIShaders_Game;
	for(int i=0; i<MAX_UI; i++)
		m_pUI[i] = new UIClass(pd3dDevice);
	m_pUI[0]->Initialize(pd3dDevice, _wndWidth, _wndHeight, L"UI/UI_skill_black.png",m_UIskillbarWidth, m_UIskillbarHeight); //스킬바
	m_pUI[1]->Initialize(pd3dDevice, _wndWidth, _wndHeight, L"UI/UI_Map_black.png",m_UIMinimapWidth, m_UIMinimapHeight); //미니맵
	m_pUI[2]->Initialize(pd3dDevice, _wndWidth, _wndHeight, L"UI/UI_Info_black.png",m_UIInfoWidth, m_UIInfoHeight); //캐릭터창
	m_pUI[3]->Initialize(pd3dDevice, _wndWidth, _wndHeight, L"UI/UI_Score_black.png",m_UIScoreWidth, m_UIScoreHeight); //킬,데스창
	m_pUI[4]->Initialize(pd3dDevice, _wndWidth, _wndHeight, L"UI/Sword.png",m_SwordWidth, m_SwordHeight); //검
	m_pUI[5]->Initialize(pd3dDevice, _wndWidth, _wndHeight, L"UI/Shield.png",m_ShielWidth, m_ShielHeight); //방패
	m_pUI[6]->Initialize(pd3dDevice, _wndWidth, _wndHeight, L"UI/Boots.png",m_BootsWidth, m_BootsHeight); //신발
	m_pUI[7]->Initialize(pd3dDevice, _wndWidth, _wndHeight, L"UI/Hp_Bar_Red.png",m_HpbarRWidth, m_HpbarRHeight); //hp바 빨간색
	m_pUI[8]->Initialize(pd3dDevice, _wndWidth, _wndHeight, L"UI/Hp_Bar_Green1.png",m_HpbarGWidth, m_HpbarGHeight); //hp바 초록색
	if(HeroManager::sharedManager()->m_pHero->GetType() == 1)
		m_pUI[9]->Initialize(pd3dDevice, _wndWidth, _wndHeight, L"UI/varian.png",m_CharacterFaceWidth, m_CharacterFaceHeight); //캐릭터 얼굴
	else if(HeroManager::sharedManager()->m_pHero->GetType() == 2)
		m_pUI[9]->Initialize(pd3dDevice, _wndWidth, _wndHeight, L"UI/Coldwraith.png",m_CharacterFaceWidth, m_CharacterFaceHeight); //캐릭터 얼굴
	m_pUI[10]->Initialize(pd3dDevice, _wndWidth, _wndHeight, L"UI/skillq_button.png",m_Skillq_buttonWidth, m_Skillq_buttonHeight); //스킬 q
	m_pUI[11]->Initialize(pd3dDevice, _wndWidth, _wndHeight, L"UI/skillw_button.png",m_Skillw_buttonWidth, m_Skillw_buttonHeight); //스킬 w
	m_pUI[12]->Initialize(pd3dDevice, _wndWidth, _wndHeight, L"UI/skillq_button.png",m_Skillq_buttonWidth, m_Skillq_buttonHeight); //스킬 q
	m_pUI[13]->Initialize(pd3dDevice, _wndWidth, _wndHeight, L"UI/skillw_button.png",m_Skillw_buttonWidth, m_Skillw_buttonHeight); //스킬 w

	m_pUI[14]->Initialize(pd3dDevice, _wndWidth, _wndHeight, L"UI/Sword.png",m_UpgradeSwordWidth, m_UpgradeSwordHeight); //특성 업그레이드 공격
	m_pUI[15]->Initialize(pd3dDevice, _wndWidth, _wndHeight, L"UI/Shield.png",m_UpgradeShieldWidth, m_UpgradeShieldHeight); //특성 업그레이드 방어
	m_pUI[16]->Initialize(pd3dDevice, _wndWidth, _wndHeight, L"UI/Boots.png",m_UpgradeBootsWidth, m_UpgradeBootsHeight); //특성 업그레이드 스피드

	m_pUI[17]->Initialize(pd3dDevice, _wndWidth, _wndHeight, L"UI/UI_Info_black.png",m_UITargetInfoWidth, m_UITargetInfoHeight); //타겟 정보창
	m_pUI[18]->Initialize(pd3dDevice, _wndWidth, _wndHeight, L"UI/Sword.png",m_SwordWidth, m_SwordHeight); //검 타겟 정보창
	m_pUI[19]->Initialize(pd3dDevice, _wndWidth, _wndHeight, L"UI/Shield.png",m_ShielWidth, m_ShielHeight); //방패 타겟 정보창
	m_pUI[20]->Initialize(pd3dDevice, _wndWidth, _wndHeight, L"UI/Boots.png",m_BootsWidth, m_BootsHeight); //신발 타겟 정보창
	m_pUI[21]->Initialize(pd3dDevice, _wndWidth, _wndHeight, L"UI/lichking.png",m_CharacterFaceWidth, m_CharacterFaceHeight); //타겟 캐릭터 얼굴
	m_pUI[22]->Initialize(pd3dDevice, _wndWidth, _wndHeight, L"UI/Hp_Bar_Red.png",m_TargetHpbarRWidth, m_TargetHpbarRHeight); //타겟 hp바 빨간색
	m_pUI[23]->Initialize(pd3dDevice, _wndWidth, _wndHeight, L"UI/Hp_Bar_Green1.png",m_TargetHpbarGWidth, m_TargetHpbarGHeight); // 타겟 hp바 초록색

	m_pUI[24]->Initialize(pd3dDevice, _wndWidth, _wndHeight, L"UI/upgrade_button.png",m_UpgradeButtonWidth, m_UpgradeButtonHeight);//업그레이드 버튼
	m_pUI[25]->Initialize(pd3dDevice, _wndWidth, _wndHeight, L"UI/1_button.png",m_UpgradeButtonNumberWidth, m_UpgradeButtonNumberHeight);//1 버튼
	m_pUI[26]->Initialize(pd3dDevice, _wndWidth, _wndHeight, L"UI/2_button.png",m_UpgradeButtonNumberWidth, m_UpgradeButtonNumberHeight);//2 버튼
	m_pUI[27]->Initialize(pd3dDevice, _wndWidth, _wndHeight, L"UI/3_button.png",m_UpgradeButtonNumberWidth, m_UpgradeButtonNumberHeight);//3 버튼

	m_pUI[28]->Initialize(pd3dDevice, _wndWidth, _wndHeight, L"UI/Game_Win.png",m_GameWinLoseWidth, m_GameWinLoseHeight);//WIN
	m_pUI[29]->Initialize(pd3dDevice, _wndWidth, _wndHeight, L"UI/Game_Lose.png",m_GameWinLoseWidth, m_GameWinLoseHeight);//LOSE
	m_pUI[30]->Initialize(pd3dDevice, _wndWidth, _wndHeight, L"UI/Game_Draw.png",m_GameWinLoseWidth, m_GameWinLoseHeight);//DRAW
	for(int i=0; i<MAX_UI; i++)
	{
		m_pUIObjects[i] = new UIObject();
		m_pUIObjects[i]->SetUI(m_pUI[i]);
	}

	for(int i=0; i<MAX_UI; i++)
		m_pUIShaders->AddObject(m_pUIObjects[i]);
}


void CScene::RenderUI(ID3D11DeviceContext *pd3dDeviceContext,int  _wndWidth,int  _wndHeight)
{
	if(m_bUIReset == FALSE)
	{
		SetUISize();
		m_bUIReset = TRUE;
	}

	m_pUIShaders->Render(pd3dDeviceContext);

	//캐릭터 얼굴 창
	m_pUIShaders->UpdateShaderVariables(pd3dDeviceContext, &m_pUIObjects[9]->m_d3dxmtxWorld);
	m_pUIObjects[9]->Render(pd3dDeviceContext, 0 + m_CharacterFaceWidth/4 , _wndHeight - m_UIInfoHeight + m_CharacterFaceHeight/3 + m_CharacterFaceHeight/13 );

	if(HeroManager::sharedManager()->m_pHero->GetTarget() != NULL)
	{
		//타겟 캐릭터 얼굴 창
		m_pUIShaders->UpdateShaderVariables(pd3dDeviceContext, &m_pUIObjects[21]->m_d3dxmtxWorld);
		m_pUIObjects[21]->Render(pd3dDeviceContext, 0 + m_CharacterFaceWidth/4 , m_CharacterFaceHeight/3 + m_CharacterFaceHeight/13 );
	}

	TurnOnAlphaBlending(pd3dDeviceContext, m_particleEnableBlendingState);

	//스킬 바 UI
	m_pUIShaders->UpdateShaderVariables(pd3dDeviceContext, &m_pUIObjects[0]->m_d3dxmtxWorld);
	m_pUIObjects[0]->Render(pd3dDeviceContext, _wndWidth/2 - m_UIskillbarWidth/2, _wndHeight - m_UIskillbarHeight);
	//미니맵 Ui
	m_pUIShaders->UpdateShaderVariables(pd3dDeviceContext, &m_pUIObjects[1]->m_d3dxmtxWorld);
	m_pUIObjects[1]->Render(pd3dDeviceContext, _wndWidth - m_UIMinimapWidth, _wndHeight - m_UIMinimapHeight);
	//캐릭터 정보창 UI
	m_pUIShaders->UpdateShaderVariables(pd3dDeviceContext, &m_pUIObjects[2]->m_d3dxmtxWorld);
	m_pUIObjects[2]->Render(pd3dDeviceContext, 0, _wndHeight - m_UIInfoHeight);
	//스코어 창 UI
	m_pUIShaders->UpdateShaderVariables(pd3dDeviceContext, &m_pUIObjects[3]->m_d3dxmtxWorld);
	m_pUIObjects[3]->Render(pd3dDeviceContext, _wndWidth - m_UIScoreWidth, 0);
	
	if(HeroManager::sharedManager()->m_pHero->GetTarget() != NULL)
	{
		//타겟 정보창 UI
		m_pUIShaders->UpdateShaderVariables(pd3dDeviceContext, &m_pUIObjects[17]->m_d3dxmtxWorld);
		m_pUIObjects[17]->Render(pd3dDeviceContext, 0.f, m_UITargetInfoHeight/100);
	}

	TurnOffAlphaBlending(pd3dDeviceContext, m_particleDisableBlendingState);
	//공격력 특성
	m_pUIShaders->UpdateShaderVariables(pd3dDeviceContext, &m_pUIObjects[4]->m_d3dxmtxWorld);
	m_pUIObjects[4]->Render(pd3dDeviceContext, m_UIInfoWidth/2 + m_UIInfoWidth/20, _wndHeight - m_UIInfoHeight + m_UIInfoHeight/3 - m_SwordHeight/2);
	//방어력 특성
	m_pUIShaders->UpdateShaderVariables(pd3dDeviceContext, &m_pUIObjects[5]->m_d3dxmtxWorld);
	m_pUIObjects[5]->Render(pd3dDeviceContext, m_UIInfoWidth/2 + m_UIInfoWidth/20, _wndHeight - m_UIInfoHeight/2 - m_ShielHeight/2);
	//스피드 특성
	m_pUIShaders->UpdateShaderVariables(pd3dDeviceContext, &m_pUIObjects[6]->m_d3dxmtxWorld);
	m_pUIObjects[6]->Render(pd3dDeviceContext, m_UIInfoWidth/2 + m_UIInfoWidth/20, _wndHeight - m_UIInfoHeight + m_UIInfoHeight*2/3 - m_BootsHeight/2);
	//hp바 빨간색
	m_pUIShaders->UpdateShaderVariables(pd3dDeviceContext, &m_pUIObjects[7]->m_d3dxmtxWorld);
	m_pUIObjects[7]->Render(pd3dDeviceContext, _wndWidth/2 - m_HpbarRWidth/2 , _wndHeight - m_UIskillbarHeight + m_UIskillbarHeight*2/3 + m_HpbarRHeight/2);
	//hp바 초록색
	m_pUIShaders->UpdateShaderVariables(pd3dDeviceContext, &m_pUIObjects[8]->m_d3dxmtxWorld);
	m_pUIObjects[8]->Render(pd3dDeviceContext, _wndWidth/2 - m_HpbarRWidth/2 , _wndHeight - m_UIskillbarHeight + m_UIskillbarHeight*2/3 + m_HpbarGHeight/2);
	//스킬 q버튼
	m_pUIShaders->UpdateShaderVariables(pd3dDeviceContext, &m_pUIObjects[10]->m_d3dxmtxWorld);
	m_pUIObjects[10]->Render(pd3dDeviceContext, _wndWidth/2 - m_Skillq_buttonWidth*4 +m_Skillq_buttonWidth*2/5, _wndHeight - m_UIskillbarHeight + m_Skillq_buttonHeight - m_Skillq_buttonHeight/5);
	//스킬 w버튼
	m_pUIShaders->UpdateShaderVariables(pd3dDeviceContext, &m_pUIObjects[11]->m_d3dxmtxWorld);
	m_pUIObjects[11]->Render(pd3dDeviceContext, _wndWidth/2 - m_Skillq_buttonWidth*3 +m_Skillq_buttonWidth*3/5 + m_Skillq_buttonWidth/15, _wndHeight - m_UIskillbarHeight + m_Skillq_buttonHeight - m_Skillq_buttonHeight/5);
	//스킬 e버튼
	m_pUIShaders->UpdateShaderVariables(pd3dDeviceContext, &m_pUIObjects[12]->m_d3dxmtxWorld);
	m_pUIObjects[12]->Render(pd3dDeviceContext, _wndWidth/2 - m_Skillq_buttonWidth*2 +m_Skillq_buttonWidth - m_Skillq_buttonWidth/10, _wndHeight - m_UIskillbarHeight + m_Skillq_buttonHeight - m_Skillq_buttonHeight/5);
	//스킬 r버튼
	m_pUIShaders->UpdateShaderVariables(pd3dDeviceContext, &m_pUIObjects[13]->m_d3dxmtxWorld);
	m_pUIObjects[13]->Render(pd3dDeviceContext, _wndWidth/2 - m_Skillq_buttonWidth +m_Skillq_buttonWidth*6/5, _wndHeight - m_UIskillbarHeight + m_Skillq_buttonHeight - m_Skillq_buttonHeight/5);
	//특성 업그레이드 공격
	m_pUIShaders->UpdateShaderVariables(pd3dDeviceContext, &m_pUIObjects[14]->m_d3dxmtxWorld);
	m_pUIObjects[14]->Render(pd3dDeviceContext, _wndWidth/2 +m_UpgradeSwordWidth*2 - m_UpgradeSwordWidth/8, _wndHeight - m_UIskillbarHeight + m_UpgradeSwordHeight);
	//특성 업그레이드 방어
	m_pUIShaders->UpdateShaderVariables(pd3dDeviceContext, &m_pUIObjects[15]->m_d3dxmtxWorld);
	m_pUIObjects[15]->Render(pd3dDeviceContext, _wndWidth/2 + m_UpgradeShieldWidth*3 + m_UpgradeShieldWidth*1/15, _wndHeight - m_UIskillbarHeight +m_UpgradeShieldHeight);
	//특성 업그레이드 스피드
	m_pUIShaders->UpdateShaderVariables(pd3dDeviceContext, &m_pUIObjects[16]->m_d3dxmtxWorld);
	m_pUIObjects[16]->Render(pd3dDeviceContext, _wndWidth/2 + m_UpgradeBootsWidth*4 + m_UpgradeBootsWidth*2/5, _wndHeight - m_UIskillbarHeight + m_UpgradeBootsHeight);


	if(HeroManager::sharedManager()->m_pHero->GetTarget() != NULL)
	{
		//공격력 특성 타겟 정보창
		m_pUIShaders->UpdateShaderVariables(pd3dDeviceContext, &m_pUIObjects[18]->m_d3dxmtxWorld);
		m_pUIObjects[18]->Render(pd3dDeviceContext, m_UIInfoWidth/2 + m_UIInfoWidth/20, m_UIInfoHeight/3 - m_SwordHeight/2);
		//방어력 특성 타겟 정보창
		m_pUIShaders->UpdateShaderVariables(pd3dDeviceContext, &m_pUIObjects[19]->m_d3dxmtxWorld);
		m_pUIObjects[19]->Render(pd3dDeviceContext, m_UIInfoWidth/2 + m_UIInfoWidth/20, m_UIInfoHeight/2 - m_ShielHeight/2);
		//스피드 특성 타겟 정보창
		m_pUIShaders->UpdateShaderVariables(pd3dDeviceContext, &m_pUIObjects[20]->m_d3dxmtxWorld);
		m_pUIObjects[20]->Render(pd3dDeviceContext, m_UIInfoWidth/2 + m_UIInfoWidth/20, m_UIInfoHeight*2/3 - m_BootsHeight/2);

		//hp바 빨간색
		m_pUIShaders->UpdateShaderVariables(pd3dDeviceContext, &m_pUIObjects[22]->m_d3dxmtxWorld);
		m_pUIObjects[22]->Render(pd3dDeviceContext, m_UIInfoWidth/2 ,  m_UITargetInfoHeight - m_TargetHpbarRHeight*4);
		//hp바 초록색
		m_pUIShaders->UpdateShaderVariables(pd3dDeviceContext, &m_pUIObjects[23]->m_d3dxmtxWorld);
		m_pUIObjects[23]->Render(pd3dDeviceContext, m_UIInfoWidth/2 ,  m_UITargetInfoHeight - m_TargetHpbarRHeight*4);
	}

	if(HeroManager::sharedManager()->m_pHero->GetUpgradePossible() == TRUE)
	{
		//특성 업그레이드 버튼
		m_pUIShaders->UpdateShaderVariables(pd3dDeviceContext, &m_pUIObjects[24]->m_d3dxmtxWorld);
		m_pUIObjects[24]->Render(pd3dDeviceContext, _wndWidth/2 +m_UpgradeSwordWidth*2 - m_UpgradeSwordWidth/8 + m_UpgradeSwordWidth*2/5, _wndHeight - m_UIskillbarHeight);
		//특성 업그레이드 버튼1
		m_pUIShaders->UpdateShaderVariables(pd3dDeviceContext, &m_pUIObjects[25]->m_d3dxmtxWorld);
		m_pUIObjects[25]->Render(pd3dDeviceContext, _wndWidth/2 +m_UpgradeSwordWidth*2 - m_UpgradeSwordWidth/8, _wndHeight - m_UIskillbarHeight + m_UpgradeSwordHeight);
		//특성 업그레이드 버튼2
		m_pUIShaders->UpdateShaderVariables(pd3dDeviceContext, &m_pUIObjects[26]->m_d3dxmtxWorld);
		m_pUIObjects[26]->Render(pd3dDeviceContext, _wndWidth/2 + m_UpgradeShieldWidth*3 + m_UpgradeShieldWidth*1/15, _wndHeight - m_UIskillbarHeight +m_UpgradeShieldHeight);
		//특성 업그레이드 버튼3
		m_pUIShaders->UpdateShaderVariables(pd3dDeviceContext, &m_pUIObjects[27]->m_d3dxmtxWorld);
		m_pUIObjects[27]->Render(pd3dDeviceContext, _wndWidth/2 + m_UpgradeBootsWidth*4 + m_UpgradeBootsWidth*2/5, _wndHeight - m_UIskillbarHeight + m_UpgradeBootsHeight);
	}


	TurnOnAlphaBlending(pd3dDeviceContext, m_particleEnableBlendingState); //승리 패배
	if(m_bGameOver == TRUE)
	{
		if(HeroManager::sharedManager()->m_pHero->GetTeam() == RED_TEAM)
		{
			if(m_pBlueNexus->GetHP() <= 0)
			{
				//WIN
				m_pUIShaders->UpdateShaderVariables(pd3dDeviceContext, &m_pUIObjects[28]->m_d3dxmtxWorld);
				m_pUIObjects[28]->Render(pd3dDeviceContext, _wndWidth/2 - m_GameWinLoseWidth/2 , _wndHeight/2 - m_GameWinLoseHeight);
			}
			else if(m_pRedNexus->GetHP() <= 0)
			{
				//LOSE
				m_pUIShaders->UpdateShaderVariables(pd3dDeviceContext, &m_pUIObjects[29]->m_d3dxmtxWorld);
				m_pUIObjects[29]->Render(pd3dDeviceContext, _wndWidth/2 - m_GameWinLoseWidth/2 , _wndHeight/2 - m_GameWinLoseHeight);
			}
			else
			{
				if(HeroManager::sharedManager()->m_pHero->GetDeathCount() < OtherPlayerManager::sharedManager()->m_pOtherPlayer->GetDeathCount())
				{
					//WIN
					m_pUIShaders->UpdateShaderVariables(pd3dDeviceContext, &m_pUIObjects[28]->m_d3dxmtxWorld);
					m_pUIObjects[28]->Render(pd3dDeviceContext, _wndWidth/2 - m_GameWinLoseWidth/2 , _wndHeight/2 - m_GameWinLoseHeight);
				}
				else if(HeroManager::sharedManager()->m_pHero->GetDeathCount() > OtherPlayerManager::sharedManager()->m_pOtherPlayer->GetDeathCount())
				{
					//LOSE
					m_pUIShaders->UpdateShaderVariables(pd3dDeviceContext, &m_pUIObjects[29]->m_d3dxmtxWorld);
					m_pUIObjects[29]->Render(pd3dDeviceContext, _wndWidth/2 - m_GameWinLoseWidth/2 , _wndHeight/2 - m_GameWinLoseHeight);
				}
				else
				{
					//DRAW
					m_pUIShaders->UpdateShaderVariables(pd3dDeviceContext, &m_pUIObjects[30]->m_d3dxmtxWorld);
					m_pUIObjects[30]->Render(pd3dDeviceContext, _wndWidth/2 - m_GameWinLoseWidth/2 , _wndHeight/2 - m_GameWinLoseHeight);
				}
			}
		}
		else if(HeroManager::sharedManager()->m_pHero->GetTeam() == BLUE_TEAM)
		{
			if(m_pRedNexus->GetHP() <= 0)
			{
				//WIN
				m_pUIShaders->UpdateShaderVariables(pd3dDeviceContext, &m_pUIObjects[28]->m_d3dxmtxWorld);
				m_pUIObjects[28]->Render(pd3dDeviceContext, _wndWidth/2 - m_GameWinLoseWidth/2 , _wndHeight/2 - m_GameWinLoseHeight);
			}
			else if(m_pBlueNexus->GetHP() <= 0)
			{
				//LOSE
				m_pUIShaders->UpdateShaderVariables(pd3dDeviceContext, &m_pUIObjects[29]->m_d3dxmtxWorld);
				m_pUIObjects[29]->Render(pd3dDeviceContext, _wndWidth/2 - m_GameWinLoseWidth/2 , _wndHeight/2 - m_GameWinLoseHeight);
			}
			else
			{
				if(HeroManager::sharedManager()->m_pHero->GetDeathCount() < OtherPlayerManager::sharedManager()->m_pOtherPlayer->GetDeathCount())
				{
					//WIN
					m_pUIShaders->UpdateShaderVariables(pd3dDeviceContext, &m_pUIObjects[28]->m_d3dxmtxWorld);
					m_pUIObjects[28]->Render(pd3dDeviceContext, _wndWidth/2 - m_GameWinLoseWidth/2 , _wndHeight/2 - m_GameWinLoseHeight);
				}
				else if(HeroManager::sharedManager()->m_pHero->GetDeathCount() > OtherPlayerManager::sharedManager()->m_pOtherPlayer->GetDeathCount())
				{
					//LOSE
					m_pUIShaders->UpdateShaderVariables(pd3dDeviceContext, &m_pUIObjects[29]->m_d3dxmtxWorld);
					m_pUIObjects[29]->Render(pd3dDeviceContext, _wndWidth/2 - m_GameWinLoseWidth/2 , _wndHeight/2 - m_GameWinLoseHeight);
				}
				else
				{
					//DRAW
					m_pUIShaders->UpdateShaderVariables(pd3dDeviceContext, &m_pUIObjects[30]->m_d3dxmtxWorld);
					m_pUIObjects[30]->Render(pd3dDeviceContext, _wndWidth/2 - m_GameWinLoseWidth/2 , _wndHeight/2 - m_GameWinLoseHeight);
				}
			}
		}
	}
	TurnOffAlphaBlending(pd3dDeviceContext, m_particleDisableBlendingState);

}

void CScene::SetUIHpUpdate()
{
	if(HeroManager::sharedManager()->m_pHero->GetHP() != HeroManager::sharedManager()->m_pHero->GetPrevHP())
	{
		HeroManager::sharedManager()->m_pHero->SetPrevHP(HeroManager::sharedManager()->m_pHero->GetHP());

		m_Hpbar = (m_nWndClientWidth*m_HpbarGWidth) / m_nPrevWndClientWidth * (HeroManager::sharedManager()->m_pHero->GetHP()/(HeroManager::sharedManager()->m_pHero->GetLevel()*100));
		m_pUI[8]->SetBitmapWH(m_Hpbar, m_HpbarGHeight);
	}

	if(HeroManager::sharedManager()->m_pHero->GetTarget() != NULL)
	{
		if(m_iPrevHpType != HeroManager::sharedManager()->m_pHero->GetTarget()->GetID())
		{
			m_bChangeHpbar = TRUE;
			m_iPrevHpType = HeroManager::sharedManager()->m_pHero->GetTarget()->GetID();
		}

		if(HeroManager::sharedManager()->m_pHero->GetTarget()->GetHP() != HeroManager::sharedManager()->m_pHero->GetTarget()->GetPrevHP() ||
			m_bChangeHpbar == TRUE)
		{
			HeroManager::sharedManager()->m_pHero->GetTarget()->SetPrevHP(HeroManager::sharedManager()->m_pHero->GetTarget()->GetHP());

			if(HeroManager::sharedManager()->m_pHero->GetTarget()->GetFaceType() == CLEFT_FACE || 
				HeroManager::sharedManager()->m_pHero->GetTarget()->GetFaceType() == TURTLE_FACE ||
				HeroManager::sharedManager()->m_pHero->GetTarget()->GetFaceType() == GOLEM_FACE)
				m_TargetHpbar = (m_nWndClientWidth*m_TargetHpbarGWidth) / m_nPrevWndClientWidth * (HeroManager::sharedManager()->m_pHero->GetTarget()->GetHP()/(HeroManager::sharedManager()->m_pHero->GetTarget()->GetLevel()*50));
			else if(HeroManager::sharedManager()->m_pHero->GetTarget()->GetFaceType() == TOWER_FACE)
				m_TargetHpbar = (m_nWndClientWidth*m_TargetHpbarGWidth) / m_nPrevWndClientWidth * (HeroManager::sharedManager()->m_pHero->GetTarget()->GetHP()/(HeroManager::sharedManager()->m_pHero->GetTarget()->GetLevel()*2));
			else if(HeroManager::sharedManager()->m_pHero->GetTarget()->GetFaceType() == NEXUS_FACE)
				m_TargetHpbar = (m_nWndClientWidth*m_TargetHpbarGWidth) / m_nPrevWndClientWidth * (HeroManager::sharedManager()->m_pHero->GetTarget()->GetHP()/(HeroManager::sharedManager()->m_pHero->GetTarget()->GetLevel()*5));
			else if(HeroManager::sharedManager()->m_pHero->GetTarget()->GetFaceType() == HERO1_FACE ||
				HeroManager::sharedManager()->m_pHero->GetTarget()->GetFaceType() == HERO2_FACE)
				m_TargetHpbar = (m_nWndClientWidth*m_TargetHpbarGWidth) / m_nPrevWndClientWidth * (HeroManager::sharedManager()->m_pHero->GetTarget()->GetHP()/(HeroManager::sharedManager()->m_pHero->GetTarget()->GetLevel()*100));


			m_pUI[23]->SetBitmapWH(m_TargetHpbar, m_TargetHpbarGHeight);

			m_bChangeHpbar = FALSE;
		}
	}
}


void CScene::SetFontUI(CDXUTTextHelper* _text, int _num)
{
	WCHAR str[255];

	_text->Begin();
	if(_num == 0) //공격력
	{
		_text->SetInsertionPos( m_UIInfoWidth/2 + m_UIInfoWidth/20 + m_SwordWidth, m_nWndClientHeight - m_UIInfoHeight + m_UIInfoHeight/3 - m_SwordHeight/2);
		swprintf(str, 255, L"  : %.0f", HeroManager::sharedManager()->m_pHero->GetDamage());
	}
	else if(_num == 1) //방어력
	{
		_text->SetInsertionPos( m_UIInfoWidth/2 + m_UIInfoWidth/20 + m_ShielWidth, m_nWndClientHeight - m_UIInfoHeight/2 - m_ShielHeight/2);
		swprintf(str, 255, L"  : %d", HeroManager::sharedManager()->m_pHero->GetDefense());
	}
	else if(_num == 2) //스피드
	{
		_text->SetInsertionPos( m_UIInfoWidth/2 + m_UIInfoWidth/20 + m_BootsWidth, m_nWndClientHeight - m_UIInfoHeight + m_UIInfoHeight*2/3 - m_BootsHeight/2);
		swprintf(str, 255, L"  : %.0f", HeroManager::sharedManager()->m_pHero->GetSpeed());	
	}
	else if(_num == 3) //레벨 수치
	{
		_text->SetInsertionPos( 0 + m_CharacterFaceWidth/4, m_nWndClientHeight - m_UIInfoHeight + m_CharacterFaceHeight/3 + m_CharacterFaceHeight/12 + m_CharacterFaceHeight);
		swprintf(str, 255, L" Lv : %d", HeroManager::sharedManager()->m_pHero->GetLevel());
	}
	else if(_num == 4) //시간
	{
		_text->SetInsertionPos(m_nWndClientWidth - m_UIScoreWidth + m_UIScoreWidth/10, m_UIScoreHeight/3);
		swprintf(str, 255, L" %.f:%.f", m_fGameTimeMinute, m_fGameTimeSecond);
	}
	else if(_num == 5) // 킬
	{
		_text->SetInsertionPos( m_nWndClientWidth - m_UIScoreWidth + m_UIScoreWidth/3 + m_UIScoreWidth/20, m_UIScoreHeight/3);
		swprintf(str, 255, L" K : %d", OtherPlayerManager::sharedManager()->m_pOtherPlayer->GetDeathCount());
	}
	else if(_num == 6) //데스
	{
		_text->SetInsertionPos(m_nWndClientWidth - m_UIScoreWidth + m_UIScoreWidth*2/3 + m_UIScoreWidth/20, m_UIScoreHeight/3);
		swprintf(str, 255, L" D : %d", HeroManager::sharedManager()->m_pHero->GetDeathCount());
	}
	else if(_num == 7) // 타겟 공격력
	{
		_text->SetInsertionPos( m_UIInfoWidth/2 + m_UIInfoWidth/20 + m_SwordWidth, m_UIInfoHeight/3 - m_SwordHeight/2);
		swprintf(str, 255, L"  : %.0f", HeroManager::sharedManager()->m_pHero->GetTarget()->GetDamage());
	}
	else if(_num == 8) // 타겟 방어력
	{
		_text->SetInsertionPos( m_UIInfoWidth/2 + m_UIInfoWidth/20 + m_ShielWidth, m_UITargetInfoHeight - m_UIInfoHeight/2 - m_ShielHeight/2);
		swprintf(str, 255, L"  : %d", HeroManager::sharedManager()->m_pHero->GetTarget()->GetDefense());
	}
	else if(_num == 9) // 타겟 스피드
	{
		_text->SetInsertionPos( m_UIInfoWidth/2 + m_UIInfoWidth/20 + m_BootsWidth, m_UITargetInfoHeight - m_UIInfoHeight + m_UIInfoHeight*2/3 - m_BootsHeight/2);
		swprintf(str, 255, L"  : %.0f", HeroManager::sharedManager()->m_pHero->GetTarget()->GetSpeed());	
	}
	else if(_num == 10) // 타겟 레벨
	{
		_text->SetInsertionPos( 0 + m_CharacterFaceWidth/4, m_CharacterFaceHeight/3 + m_CharacterFaceHeight/10 + m_CharacterFaceHeight);
		swprintf(str, 255, L" Lv : %d", HeroManager::sharedManager()->m_pHero->GetTarget()->GetLevel());	
	}
	_text->SetForegroundColor( D3DXCOLOR( 0.0f, 1.0f, 0.0f, 1.0f ) );
	_text->DrawTextLine(str);
	_text->End();
}

void CScene::SetUISize()
{
	m_UIskillbarWidth = (m_nWndClientWidth*m_UIskillbarWidth) / 900;
	m_UIskillbarHeight = (m_nWndClientHeight*m_UIskillbarHeight) / 600; 
	m_UIMinimapWidth = (m_nWndClientWidth*m_UIMinimapWidth) / 900;
	m_UIMinimapHeight = (m_nWndClientHeight*m_UIMinimapHeight) / 600; 
	m_UIInfoWidth = (m_nWndClientWidth*m_UIInfoWidth) / 900;
	m_UIInfoHeight = (m_nWndClientHeight*m_UIInfoHeight) / 600; 
	m_UIScoreWidth = (m_nWndClientWidth*m_UIScoreWidth) / 900;
	m_UIScoreHeight = (m_nWndClientHeight*m_UIScoreHeight) / 600; 
	m_UITargetInfoWidth = (m_nWndClientWidth*m_UITargetInfoWidth) / 900;
	m_UITargetInfoHeight = (m_nWndClientHeight*m_UITargetInfoHeight) / 600; 

	m_SwordWidth = (m_nWndClientWidth*m_SwordWidth) / 900;
	m_SwordHeight = (m_nWndClientHeight*m_SwordHeight) / 600; 
	m_ShielWidth = (m_nWndClientWidth*m_ShielWidth) / 900;
	m_ShielHeight = (m_nWndClientHeight*m_ShielHeight) / 600; 
	m_BootsWidth = (m_nWndClientWidth*m_BootsWidth) / 900;
	m_BootsHeight = (m_nWndClientHeight*m_BootsHeight) / 600; 

	m_HpbarRWidth = (m_nWndClientWidth*m_HpbarRWidth) / 900;
	m_HpbarRHeight = (m_nWndClientHeight*m_HpbarRHeight) / 600; 
	m_HpbarGWidth = (m_nWndClientWidth*m_HpbarGWidth) / 900;
	m_HpbarGHeight = (m_nWndClientHeight*m_HpbarGHeight) / 600; 

	m_TargetHpbarRWidth = (m_nWndClientWidth*m_TargetHpbarRWidth) / 900;
	m_TargetHpbarRHeight = (m_nWndClientHeight*m_TargetHpbarRHeight) / 600; 
	m_TargetHpbarGWidth = (m_nWndClientWidth*m_TargetHpbarGWidth) / 900;
	m_TargetHpbarGHeight = (m_nWndClientHeight*m_TargetHpbarGHeight) / 600;

	m_CharacterFaceWidth = (m_nWndClientWidth*m_CharacterFaceWidth) / 900;
	m_CharacterFaceHeight = (m_nWndClientHeight*m_CharacterFaceHeight) / 600; 
	m_Skillq_buttonWidth = (m_nWndClientWidth*m_Skillq_buttonWidth) / 900;
	m_Skillq_buttonHeight = (m_nWndClientHeight*m_Skillq_buttonHeight) / 600; 
	m_Skillw_buttonWidth = (m_nWndClientWidth*m_Skillw_buttonWidth) / 900;
	m_Skillw_buttonHeight = (m_nWndClientHeight*m_Skillw_buttonHeight) / 600; 
	m_Skille_buttonWidth = (m_nWndClientWidth*m_Skille_buttonWidth) / 900;
	m_Skille_buttonHeight = (m_nWndClientHeight*m_Skille_buttonHeight) / 600; 
	m_Skillr_buttonWidth = (m_nWndClientWidth*m_Skillr_buttonWidth) / 900;
	m_Skillr_buttonHeight = (m_nWndClientHeight*m_Skillr_buttonHeight) / 600; 

	m_UpgradeSwordWidth = (m_nWndClientWidth*m_UpgradeSwordWidth) / 900;
	m_UpgradeSwordHeight = (m_nWndClientHeight*m_UpgradeSwordHeight) / 600; 
	m_UpgradeShieldWidth = (m_nWndClientWidth*m_UpgradeShieldWidth) / 900;
	m_UpgradeShieldHeight = (m_nWndClientHeight*m_UpgradeShieldHeight) / 600; 
	m_UpgradeBootsWidth = (m_nWndClientWidth*m_UpgradeBootsWidth) / 900;
	m_UpgradeBootsHeight = (m_nWndClientHeight*m_UpgradeBootsHeight) / 600; 

	m_UpgradeButtonWidth = (m_nWndClientWidth*m_UpgradeButtonWidth) / 900;
	m_UpgradeButtonHeight = (m_nWndClientHeight*m_UpgradeButtonHeight) / 600; 
	m_UpgradeButtonNumberWidth = (m_nWndClientWidth*m_UpgradeButtonNumberWidth) / 900;
	m_UpgradeButtonNumberHeight = (m_nWndClientHeight*m_UpgradeButtonNumberHeight) / 600; 

	m_GameWinLoseWidth = (m_nWndClientWidth*m_GameWinLoseWidth) / 900;
	m_GameWinLoseHeight = (m_nWndClientHeight*m_GameWinLoseHeight) / 600; 

	for(int i=0; i<MAX_UI; i++)
	{
		if(m_pUIObjects[i]->GetUI()->GetScreenW() != m_nWndClientWidth || m_pUIObjects[i]->GetUI()->GetScreenH() != m_nWndClientHeight)
			m_pUIObjects[i]->GetUI()->SetScreenWH(m_nWndClientWidth, m_nWndClientHeight);
	}
	m_pUI[0]->SetBitmapWH(m_UIskillbarWidth, m_UIskillbarHeight);
	m_pUI[1]->SetBitmapWH(m_UIMinimapWidth, m_UIMinimapHeight);
	m_pUI[2]->SetBitmapWH(m_UIInfoWidth, m_UIInfoHeight);
	m_pUI[3]->SetBitmapWH(m_UIScoreWidth, m_UIScoreHeight);
	m_pUI[4]->SetBitmapWH(m_SwordWidth, m_SwordHeight);
	m_pUI[5]->SetBitmapWH(m_ShielWidth, m_ShielHeight);
	m_pUI[6]->SetBitmapWH(m_BootsWidth, m_BootsHeight);
	m_pUI[7]->SetBitmapWH(m_HpbarRWidth, m_HpbarRHeight);
	m_pUI[8]->SetBitmapWH(m_HpbarGWidth, m_HpbarGHeight);
	m_pUI[9]->SetBitmapWH(m_CharacterFaceWidth, m_CharacterFaceHeight);
	m_pUI[10]->SetBitmapWH(m_Skillq_buttonWidth, m_Skillq_buttonHeight);
	m_pUI[11]->SetBitmapWH(m_Skillw_buttonWidth, m_Skillw_buttonHeight);
	m_pUI[12]->SetBitmapWH(m_Skille_buttonWidth, m_Skille_buttonHeight);
	m_pUI[13]->SetBitmapWH(m_Skillr_buttonWidth, m_Skillr_buttonHeight);
	m_pUI[14]->SetBitmapWH(m_UpgradeSwordWidth, m_UpgradeSwordHeight);
	m_pUI[15]->SetBitmapWH(m_UpgradeShieldWidth, m_UpgradeShieldHeight);
	m_pUI[16]->SetBitmapWH(m_UpgradeBootsWidth, m_UpgradeBootsHeight);
	m_pUI[17]->SetBitmapWH(m_UITargetInfoWidth, m_UITargetInfoHeight);
	m_pUI[18]->SetBitmapWH(m_SwordWidth, m_SwordHeight);
	m_pUI[19]->SetBitmapWH(m_ShielWidth, m_ShielHeight);
	m_pUI[20]->SetBitmapWH(m_BootsWidth, m_BootsHeight);
	m_pUI[21]->SetBitmapWH(m_CharacterFaceWidth, m_CharacterFaceHeight);
	m_pUI[22]->SetBitmapWH(m_TargetHpbarRWidth, m_TargetHpbarRHeight);
	m_pUI[23]->SetBitmapWH(m_TargetHpbarGWidth, m_TargetHpbarGHeight);
	m_pUI[24]->SetBitmapWH(m_UpgradeButtonWidth, m_UpgradeButtonHeight);
	m_pUI[25]->SetBitmapWH(m_UpgradeButtonNumberWidth, m_UpgradeButtonNumberHeight);
	m_pUI[26]->SetBitmapWH(m_UpgradeButtonNumberWidth, m_UpgradeButtonNumberHeight);
	m_pUI[27]->SetBitmapWH(m_UpgradeButtonNumberWidth, m_UpgradeButtonNumberHeight);
	m_pUI[28]->SetBitmapWH(m_GameWinLoseWidth, m_GameWinLoseHeight);
	m_pUI[29]->SetBitmapWH(m_GameWinLoseWidth, m_GameWinLoseHeight);
	m_pUI[30]->SetBitmapWH(m_GameWinLoseWidth, m_GameWinLoseHeight);
}