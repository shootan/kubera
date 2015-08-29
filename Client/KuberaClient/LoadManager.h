#pragma once
#include "header.h"
#include "GameObject.h"
#include "HeroObject.h"
#include "ObstacleObject.h"
#include "Shader.h"
#include "ControlManager.h"
#include "TowerObject.h"
#include "ST.h"
#include "OtherPlayerObject.h"
#include "TowerManager.h"
#include "OtherPlayerManager.h"
#include "ParticleManager.h"
#include "FBX\GFBXMeshLoader.h"
#include "ParticleMesh.h"
#include "Particle2Mesh.h"
//#include "Particle3Mesh.h"
#include "ParticleWizardSkillMesh.h"
#include "ParticleBarianSkillMesh.h"
#include "Camera.h"
#include "MapEditorManager.h"

class LoadManager
{
public:
	CInstancingShader *m_pInstancingShaders;
	CAnimationShader *m_pAnimationShaders;
	CAnimationShader *m_pAnimationShaders_NoLight;
	CParticleShader *m_pParticleShaders;
	CObjectShader *m_pObjectShaders;

	CUIShader *m_pUIShaders_Loading;
	CUIShader *m_pUIShaders_Select;
	CUIShader *m_pUIShaders_Game;
	int m_nShaders;

	int m_nObjects;
	int m_nIntanceObjects;
	int m_nAnimationObjects;

	//히어로 메쉬
	GFBX::Mesh *m_pWarriorMesh;
	GFBX::Mesh *m_pWizardMesh;

	GFBX::Mesh *m_pMageMesh;
	GFBX::Mesh *m_pVarianMesh;

	//몬스터 메쉬
	GFBX::Mesh *m_pCleftMesh;
	GFBX::Mesh *m_pTurtleMesh;
	GFBX::Mesh *m_pCannonGolemMesh;
	//바닥 메쉬
	GFBX::Mesh *pPlaneMesh;
	//넥서스메쉬
	GFBX::Mesh *pBlueNexusMesh;
	GFBX::Mesh *pRedNexusMesh;
	GFBX::Mesh *pDestroyNexusMesh;

	ParticleMesh *m_pParticleMesh;
	Particle2Mesh *m_pParticle2Mesh;
	//Particle3Mesh *m_pParticle3Mesh;
	ParticleWizardSkillMesh *m_pParticleWizardSkillMesh;
	ParticleBarianSkillMesh *m_pParticleBarianSkillMesh;

	CGameObject *m_pPlane;

	CGameObject *pBoundBox[2];
	CGameObject *m_pBlueNexus;
	CGameObject *m_pRedNexus;
	CGameObject *m_pDestroyNexus;
	CCubeMesh *pBox[2];
	
	BOOL LoadFinish;
private:
	LoadManager()
	{
		LoadFinish = FALSE;
	}
public:
	~LoadManager()
	{
		delete instance;	
	}
public:
	static LoadManager* instance;
	static LoadManager* sharedManager()
	{
		if(instance == NULL) 
			instance = new LoadManager;
		return instance;
	}
public:
	void LoadUIShader(ID3D11Device* pd3Device)
	{
		m_pUIShaders_Loading = new CUIShader();
		m_pUIShaders_Loading->CreateShader(pd3Device, 80);

		m_pUIShaders_Select = new CUIShader();
		m_pUIShaders_Select->CreateShader(pd3Device, 80);

		m_pUIShaders_Game = new CUIShader();
		m_pUIShaders_Game->CreateShader(pd3Device, 80);
	}
	void LoadShaderInstancing1(ID3D11Device *pd3dDevice)
	{
		
		//이 쉐이더 객체에 대한 포인터들의 배열을 정의한다.
		m_pObjectShaders = new CObjectShader();
		m_pObjectShaders->CreateShader(pd3dDevice, 100);
		

		
	}

	void LoadShaderInstancing2(ID3D11Device *pd3dDevice)
	{
		
		printf("CreateInstancingShader\n");
		m_pInstancingShaders = new CInstancingShader();
		m_pInstancingShaders->CreateShader(pd3dDevice, 10);
		m_pInstancingShaders->BuildObjects(pd3dDevice);
		
		//게임 객체에 대한 포인터들의 배열을 정의한다.
		m_nIntanceObjects = m_pInstancingShaders->GetObjectsNumber();
		m_nObjects = m_nIntanceObjects;
	}

	void LoadShaderInstancing3(ID3D11Device *pd3dDevice)
	{
		printf("CreateAnimationShader \n");
		m_pAnimationShaders = new CAnimationShader();
		m_pAnimationShaders->CreateShader(pd3dDevice, 100, TRUE);

		m_pAnimationShaders_NoLight = new CAnimationShader();
		m_pAnimationShaders_NoLight->CreateShader(pd3dDevice, 100, FALSE);
	}

	void LoadShaderInstancing4(ID3D11Device *pd3dDevice)
	{
		
		printf("CreateParticleShader \n");
		m_pParticleShaders = new CParticleShader();
		m_pParticleShaders->CreateShader(pd3dDevice, 300);
		
	}

	void LoadWarriorModel(ID3D11Device *pd3dDevice)
	{
		//정육면체 메쉬를 생성하고 객체에 연결한다.
		printf("Load WarriorModel \n");

		//워리어 메쉬
		m_pWarriorMesh = new GFBX::Mesh();
		GFBXMeshLoader::getInstance()->OnCreateDevice(pd3dDevice);
		GFBXMeshLoader::getInstance()->LoadFBXMesh(m_pWarriorMesh, L"Hero/Hero/knight2.FBX", pd3dDevice);
		m_pWarriorMesh->OnCreateDevice(pd3dDevice);
		for(int i=0; i<m_pWarriorMesh->GetSubsetCount(); i++)
		{
			m_pWarriorMesh->GetSubset(i)->SetBoundingCube(D3DXVECTOR3(10, 13, 10));
			m_pWarriorMesh->GetSubset(i)->LoadTexture(pd3dDevice, L"Hero/micro_knight.png");
		}
		printf("Success Load \n");
		
	}

	void LoadWizardModel(ID3D11Device *pd3dDevice)
	{
		printf("Load WizardModel \n");

		//위자드 메쉬
		m_pWizardMesh = new GFBX::Mesh();
		GFBXMeshLoader::getInstance()->OnCreateDevice(pd3dDevice);
		GFBXMeshLoader::getInstance()->LoadFBXMesh(m_pWizardMesh, L"Hero/Hero/Wizard2.FBX", pd3dDevice);
		m_pWizardMesh->OnCreateDevice(pd3dDevice);
		for(int i=0; i<m_pWizardMesh->GetSubsetCount(); i++)
		{
			m_pWizardMesh->GetSubset(i)->SetBoundingCube(D3DXVECTOR3(10, 13, 10));
			m_pWizardMesh->GetSubset(i)->LoadTexture(pd3dDevice, L"Hero/micro_wizard_col.tif");
		}
		printf("Success Load \n");
	}

	void LoadCleftModel(ID3D11Device *pd3dDevice)
	{
		printf("Load CleftModel \n");

		//코뿔소 메쉬
		m_pCleftMesh = new GFBX::Mesh();
		GFBXMeshLoader::getInstance()->OnCreateDevice(pd3dDevice);
		GFBXMeshLoader::getInstance()->LoadFBXMesh(m_pCleftMesh, L"Monster/cleft/cleft.FBX", pd3dDevice);
		m_pCleftMesh->OnCreateDevice(pd3dDevice);
		for(int i=0; i<m_pCleftMesh->GetSubsetCount(); i++)
		{
			m_pCleftMesh->GetSubset(i)->SetBoundingCube(D3DXVECTOR3(60, 30, 40));
			m_pCleftMesh->GetSubset(i)->LoadTexture(pd3dDevice, L"Monster/cleft/cleft.tif");
		}
		printf("Success Load \n");
	}

	void LoadTurtleModel(ID3D11Device *pd3dDevice)
	{
		printf("Load TurtleModel \n");

		//거북이 메쉬
		m_pTurtleMesh = new GFBX::Mesh();
		GFBXMeshLoader::getInstance()->OnCreateDevice(pd3dDevice);
		GFBXMeshLoader::getInstance()->LoadFBXMesh(m_pTurtleMesh, L"Monster/turtle/turtle3.FBX", pd3dDevice);
		m_pTurtleMesh->OnCreateDevice(pd3dDevice);
		for(int i=0; i<m_pTurtleMesh->GetSubsetCount(); i++)
		{
			m_pTurtleMesh->GetSubset(i)->SetBoundingCube(D3DXVECTOR3(50, 20, 30));
			m_pTurtleMesh->GetSubset(i)->LoadTexture(pd3dDevice, L"Monster/turtle/turtle.tif");
		}
		printf("Success Load \n");
	}

	void LoadCannonGolemModel(ID3D11Device *pd3dDevice)
	{
		printf("Load CannonGolemModel \n");

		//골렘 메쉬
		m_pCannonGolemMesh = new GFBX::Mesh();
		GFBXMeshLoader::getInstance()->OnCreateDevice(pd3dDevice);
		GFBXMeshLoader::getInstance()->LoadFBXMesh(m_pCannonGolemMesh, L"Monster/cannongolem/cannongolem2.FBX", pd3dDevice);
		m_pCannonGolemMesh->OnCreateDevice(pd3dDevice);
		for(int i=0; i<m_pCannonGolemMesh->GetSubsetCount(); i++)
		{
			m_pCannonGolemMesh->GetSubset(i)->SetBoundingCube(D3DXVECTOR3(80, 50, 50));
			m_pCannonGolemMesh->GetSubset(i)->LoadTexture(pd3dDevice, L"Monster/cannongolem/cannongolem.tif");
		}
		printf("Success Load \n");
	}

	void LoadMageModel(ID3D11Device *pd3dDevice)
	{
		printf("Load LichKingModel \n");

		//마법사 메쉬
		m_pMageMesh = new GFBX::Mesh();
		GFBXMeshLoader::getInstance()->OnCreateDevice(pd3dDevice);
		GFBXMeshLoader::getInstance()->LoadFBXMesh(m_pMageMesh, L"Hero/mage/mage.FBX", pd3dDevice);
		m_pMageMesh->OnCreateDevice(pd3dDevice);
		for(int i=0; i<m_pMageMesh->GetSubsetCount(); i++)
		{
			m_pMageMesh->GetSubset(i)->SetBoundingCube(D3DXVECTOR3(10, 30, 10));
			m_pMageMesh->GetSubset(i)->LoadTexture(pd3dDevice, L"Hero/mage/mage.tif");
		}
		printf("Success Load \n");
	}

	void LoadVarianModel(ID3D11Device *pd3dDevice)
	{
		printf("Load TestModel \n");

		//전사 메쉬
		m_pVarianMesh = new GFBX::Mesh();
		GFBXMeshLoader::getInstance()->OnCreateDevice(pd3dDevice);
		GFBXMeshLoader::getInstance()->LoadFBXMesh(m_pVarianMesh, L"Hero/varian/varian.FBX", pd3dDevice);
		m_pVarianMesh->OnCreateDevice(pd3dDevice);
		for(int i=0; i<m_pVarianMesh->GetSubsetCount(); i++)
			m_pVarianMesh->GetSubset(i)->SetBoundingCube(D3DXVECTOR3(10, 30, 10));

		m_pVarianMesh->GetSubset(0)->LoadTexture(pd3dDevice, L"Hero/varian/varian01.tif");
		m_pVarianMesh->GetSubset(1)->LoadTexture(pd3dDevice, L"Hero/varian/varian02.tif");
		m_pVarianMesh->GetSubset(2)->LoadTexture(pd3dDevice, L"Hero/varian/varian01.tif");
		printf("Success Load \n");
	}

	void LoadMesh(ID3D11Device *pd3dDevice)
	{
		printf("Load Object");
		//바닥 메쉬
		pPlaneMesh = new GFBX::Mesh();
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
		pBlueNexusMesh = new GFBX::Mesh();
		GFBXMeshLoader::getInstance()->LoadFBXMesh(pBlueNexusMesh, L"tower/Nexus.FBX", pd3dDevice);
		pBlueNexusMesh->OnCreateDevice(pd3dDevice);
		for(int i=0; i<pBlueNexusMesh->GetSubsetCount(); i++)
		{
			pBlueNexusMesh->GetSubset(i)->LoadTexture(pd3dDevice, L"tower/Nexus.png");
			pBlueNexusMesh->GetSubset(i)->SetBoundingCube(D3DXVECTOR3(50, 50, 50));
		}
		printf(".");

		//빨강팀 넥서스 메쉬
		pRedNexusMesh = new GFBX::Mesh();
		GFBXMeshLoader::getInstance()->LoadFBXMesh(pRedNexusMesh, L"tower/Nexus.FBX", pd3dDevice);
		pRedNexusMesh->OnCreateDevice(pd3dDevice);
		for(int i=0; i<pRedNexusMesh->GetSubsetCount(); i++)
		{
			pRedNexusMesh->GetSubset(i)->LoadTexture(pd3dDevice, L"tower/Nexus2.png");
			pRedNexusMesh->GetSubset(i)->SetBoundingCube(D3DXVECTOR3(50, 50, 50));
		}
		printf(".");

		//부서진 넥서스 메쉬
		pDestroyNexusMesh = new GFBX::Mesh();
		GFBXMeshLoader::getInstance()->LoadFBXMesh(pDestroyNexusMesh, L"tower/DestroyedNexus.FBX", pd3dDevice);
		pDestroyNexusMesh->OnCreateDevice(pd3dDevice);
		for(int i=0; i<pDestroyNexusMesh->GetSubsetCount(); i++)
		{
			pDestroyNexusMesh->GetSubset(i)->LoadTexture(pd3dDevice, L"tower/DestroyedNexus.png");
			pDestroyNexusMesh->GetSubset(i)->SetBoundingCube(D3DXVECTOR3(50, 50, 50));
		}
		printf(".");

		//넥서스 2개 생성
		Vector3 p = MapEditorManager::sharedManager()->GetBlueNexusPos();
		m_pBlueNexus = new CGameObject();
		m_pBlueNexus->SetMesh(pBlueNexusMesh);
		m_pBlueNexus->SetPosition(D3DXVECTOR3(p.x, 0, p.z));
		m_pBlueNexus->SetBoundSize(50, 50, 50);
		m_pBlueNexus->SetHP(500);
		m_pBlueNexus->SetID(17);
		m_pBlueNexus->SetFaceType(NEXUS_FACE);

		p = MapEditorManager::sharedManager()->GetRedNexusPos();
		m_pRedNexus = new CGameObject();
		m_pRedNexus->SetMesh(pRedNexusMesh);
		m_pRedNexus->SetPosition(D3DXVECTOR3(p.x, 0, p.z));
		m_pRedNexus->SetBoundSize(50, 50, 50);
		m_pRedNexus->SetHP(500);
		m_pRedNexus->SetID(18);
		m_pRedNexus->SetFaceType(NEXUS_FACE);

		m_pDestroyNexus = new CGameObject();
		m_pDestroyNexus->SetMesh(pDestroyNexusMesh);
		m_pDestroyNexus->SetPosition(D3DXVECTOR3(-1200, 0, 0));
		m_pDestroyNexus->SetBoundSize(50, 50, 50);
		m_pDestroyNexus->SetHP(500);
		m_pDestroyNexus->SetID(177);
		m_pDestroyNexus->SetFaceType(NEXUS_FACE);
	}

	void LoadParticle(ID3D11Device *pd3dDevice)
	{
		//파티클 메쉬
		m_pParticleMesh = new ParticleMesh(pd3dDevice);
		m_pParticleMesh->Initialize(pd3dDevice, L"effect/star.dds");
		printf(".");
		m_pParticle2Mesh = new Particle2Mesh(pd3dDevice);
		m_pParticle2Mesh->Initialize(pd3dDevice, L"effect/redstar.png");


		//m_pParticle3Mesh = new Particle3Mesh(pd3dDevice);
		//m_pParticle3Mesh->Initialize(pd3dDevice, L"effect/rocketlauncher_fx-2.tif");
		printf(".");
		m_pParticleWizardSkillMesh = new ParticleWizardSkillMesh(pd3dDevice);
		m_pParticleWizardSkillMesh->Initialize(pd3dDevice, L"effect/yellowboom.png");
		printf(".");

		m_pParticleBarianSkillMesh = new ParticleBarianSkillMesh(pd3dDevice);
		m_pParticleBarianSkillMesh->Initialize(pd3dDevice, L"effect/yellowboom.png");

		//파티클 생성
		for(int i=0; i<4; i++)
			ParticleManager::sharedManager()->CreateParticle(D3DXVECTOR3(0, 10, -2000), m_pParticleMesh, WIZARD_SKILL_BODY);
		for(int i=0; i<30; i++)
			ParticleManager::sharedManager()->CreateParticle(D3DXVECTOR3(0, 10, -2000), m_pParticle2Mesh, WIZARD_ATTACK);
		for(int i=0; i<10; i++)
			ParticleManager::sharedManager()->CreateParticle(D3DXVECTOR3(0, 10, -2000), m_pParticleWizardSkillMesh, WIZARD_SKILL_MISSILE);
		for(int i=0; i<3; i++)
			ParticleManager::sharedManager()->CreateParticle(D3DXVECTOR3(0, 10, -2000), m_pParticleBarianSkillMesh, BARIAN_SKILL_EFFECT);

		
	}
};