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
	CParticleShader *m_pParticleShaders;
	CObjectShader *m_pObjectShaders;
	int m_nShaders;

	int m_nObjects;
	int m_nIntanceObjects;
	int m_nAnimationObjects;

	GFBX::Mesh *m_pWarriorMesh;
	GFBX::Mesh *m_pWizardMesh;
	GFBX::Mesh *pPlaneMesh;
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

	ID3D11BlendState* m_particleEnableBlendingState;
	ID3D11BlendState* m_particleDisableBlendingState;
	
private:
	LoadManager()
	{

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

	void LoadShaderInstancing(ID3D11Device *pd3dDevice)
	{
		CreateBlend(pd3dDevice); //이펙트 블렌딩 설정

		
		//이 쉐이더 객체에 대한 포인터들의 배열을 정의한다.
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

	}

	HRESULT CreateBlend(ID3D11Device *pd3dDevice)
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
			m_pWarriorMesh->GetSubset(i)->LoadTexture(pd3dDevice, L"Hero/micro_knight.png");

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
			m_pWizardMesh->GetSubset(i)->LoadTexture(pd3dDevice, L"Hero/micro_wizard_col.tif");
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

		//넥서스 2개 생성
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

		
	}
};