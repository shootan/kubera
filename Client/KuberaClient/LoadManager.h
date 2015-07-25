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
#include "Particle3Mesh.h"
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
	CUIShader *m_pUIShaders_Game;
	int m_nShaders;

	int m_nObjects;
	int m_nIntanceObjects;
	int m_nAnimationObjects;

	//����� �޽�
	GFBX::Mesh *m_pWarriorMesh;
	GFBX::Mesh *m_pWizardMesh;
	GFBX::Mesh *m_pLichkingMesh;
	GFBX::Mesh *m_pTestMesh;
	//���� �޽�
	GFBX::Mesh *m_pBearMesh;
	GFBX::Mesh *m_pTurtleMesh;
	//�ٴ� �޽�
	GFBX::Mesh *pPlaneMesh;
	//�ؼ����޽�
	GFBX::Mesh *pBlueNexusMesh;
	GFBX::Mesh *pRedNexusMesh;
	ParticleMesh *m_pParticleMesh;
	Particle3Mesh *m_pParticle2Mesh;
	Particle3Mesh *m_pParticle3Mesh;

	CGameObject *m_pPlane;

	CGameObject *pBoundBox[2];
	CGameObject *m_pBlueNexus;
	CGameObject *m_pRedNexus;
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

		m_pUIShaders_Game = new CUIShader();
		m_pUIShaders_Game->CreateShader(pd3Device, 80);
	}
	void LoadShaderInstancing1(ID3D11Device *pd3dDevice)
	{
		
		//�� ���̴� ��ü�� ���� �����͵��� �迭�� �����Ѵ�.
		m_pObjectShaders = new CObjectShader();
		m_pObjectShaders->CreateShader(pd3dDevice, 100);
		

		
	}

	void LoadShaderInstancing2(ID3D11Device *pd3dDevice)
	{
		
		printf("CreateInstancingShader\n");
		m_pInstancingShaders = new CInstancingShader();
		m_pInstancingShaders->CreateShader(pd3dDevice, 10);
		m_pInstancingShaders->BuildObjects(pd3dDevice);
		
		//���� ��ü�� ���� �����͵��� �迭�� �����Ѵ�.
		m_nIntanceObjects = m_pInstancingShaders->GetObjectsNumber();
		m_nObjects = 20 + m_nIntanceObjects;
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
		//������ü �޽��� �����ϰ� ��ü�� �����Ѵ�.
		printf("Load WarriorModel \n");

		//������ �޽�
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

		//���ڵ� �޽�
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

	void LoadBearModel(ID3D11Device *pd3dDevice)
	{
		printf("Load BearModel \n");

		//�ڻԼ� �޽�
		m_pBearMesh = new GFBX::Mesh();
		GFBXMeshLoader::getInstance()->OnCreateDevice(pd3dDevice);
		GFBXMeshLoader::getInstance()->LoadFBXMesh(m_pBearMesh, L"Monster/bear.FBX", pd3dDevice);
		m_pBearMesh->OnCreateDevice(pd3dDevice);
		for(int i=0; i<m_pBearMesh->GetSubsetCount(); i++)
		{
			m_pBearMesh->GetSubset(i)->SetBoundingCube(D3DXVECTOR3(50, 10, 25));
			m_pBearMesh->GetSubset(i)->LoadTexture(pd3dDevice, L"Monster/bear.tif");
		}
		printf("Success Load \n");
	}

	void LoadTurtleModel(ID3D11Device *pd3dDevice)
	{
		printf("Load TurtleModel \n");

		//�ڻԼ� �޽�
		m_pTurtleMesh = new GFBX::Mesh();
		GFBXMeshLoader::getInstance()->OnCreateDevice(pd3dDevice);
		GFBXMeshLoader::getInstance()->LoadFBXMesh(m_pTurtleMesh, L"Monster/turtle/turtle3.FBX", pd3dDevice);
		m_pTurtleMesh->OnCreateDevice(pd3dDevice);
		for(int i=0; i<m_pTurtleMesh->GetSubsetCount(); i++)
		{
			m_pTurtleMesh->GetSubset(i)->SetBoundingCube(D3DXVECTOR3(50, 10, 25));
			m_pTurtleMesh->GetSubset(i)->LoadTexture(pd3dDevice, L"Monster/turtle/turtle.tif");
		}
		printf("Success Load \n");
	}

	void LoadLichkingModel(ID3D11Device *pd3dDevice)
	{
		printf("Load TurtleModel \n");

		//�ڻԼ� �޽�
		m_pLichkingMesh = new GFBX::Mesh();
		GFBXMeshLoader::getInstance()->OnCreateDevice(pd3dDevice);
		GFBXMeshLoader::getInstance()->LoadFBXMesh(m_pLichkingMesh, L"Monster/lichking2.FBX", pd3dDevice);
		m_pLichkingMesh->OnCreateDevice(pd3dDevice);
		for(int i=0; i<m_pLichkingMesh->GetSubsetCount(); i++)
		{
			m_pLichkingMesh->GetSubset(i)->SetBoundingCube(D3DXVECTOR3(50, 10, 25));
			m_pLichkingMesh->GetSubset(i)->LoadTexture(pd3dDevice, L"Monster/lichking.tif");
		}
		printf("Success Load \n");
	}

	void LoadTestModel(ID3D11Device *pd3dDevice)
	{
		printf("Load TurtleModel \n");

		//�ڻԼ� �޽�
		m_pTestMesh = new GFBX::Mesh();
		GFBXMeshLoader::getInstance()->OnCreateDevice(pd3dDevice);
		GFBXMeshLoader::getInstance()->LoadFBXMesh(m_pTestMesh, L"Monster/angel2.FBX", pd3dDevice);
		m_pTestMesh->OnCreateDevice(pd3dDevice);
		for(int i=0; i<m_pTestMesh->GetSubsetCount(); i++)
		{
			m_pTestMesh->GetSubset(i)->SetBoundingCube(D3DXVECTOR3(50, 10, 25));
			m_pTestMesh->GetSubset(i)->LoadTexture(pd3dDevice, L"Monster/angel.tif");
		}
		printf("Success Load \n");
	}

	void LoadMesh(ID3D11Device *pd3dDevice)
	{
		printf("Load Object");
		//�ٴ� �޽�
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

		//����� �ؼ��� �޽�
		pBlueNexusMesh = new GFBX::Mesh();
		GFBXMeshLoader::getInstance()->LoadFBXMesh(pBlueNexusMesh, L"tower/Nexus.FBX", pd3dDevice);
		pBlueNexusMesh->OnCreateDevice(pd3dDevice);
		for(int i=0; i<pBlueNexusMesh->GetSubsetCount(); i++)
		{
			pBlueNexusMesh->GetSubset(i)->LoadTexture(pd3dDevice, L"tower/Nexus.png");
			pBlueNexusMesh->GetSubset(i)->SetBoundingCube(D3DXVECTOR3(50, 50, 50));
		}
		printf(".");

		//������ �ؼ��� �޽�
		pRedNexusMesh = new GFBX::Mesh();
		GFBXMeshLoader::getInstance()->LoadFBXMesh(pRedNexusMesh, L"tower/Nexus.FBX", pd3dDevice);
		pRedNexusMesh->OnCreateDevice(pd3dDevice);
		for(int i=0; i<pRedNexusMesh->GetSubsetCount(); i++)
		{
			pRedNexusMesh->GetSubset(i)->LoadTexture(pd3dDevice, L"tower/Nexus2.png");
			pRedNexusMesh->GetSubset(i)->SetBoundingCube(D3DXVECTOR3(50, 50, 50));
		}
		printf(".");

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
		
	}

	void LoadParticle(ID3D11Device *pd3dDevice)
	{
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

		
	}
};