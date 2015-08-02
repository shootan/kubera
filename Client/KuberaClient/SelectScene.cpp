#pragma once
#include "SelectScene.h"
#include "LoadManager.h"
#include "HeroManager.h"

SelectScene::SelectScene()
{
}
SelectScene::~SelectScene()
{

}

void SelectScene::BuildObject(ID3D11Device *m_pd3dDevice)
{
	m_pAniShaders = LoadManager::sharedManager()->m_pAnimationShaders_NoLight;
	m_pWarrior = new HeroObject();
	m_pWarrior->SetHP(50);
	m_pWarrior->SetTeam(1);
	m_pWizard = new HeroObject();
	m_pWizard->SetHP(50);
	m_pWizard->SetTeam(1);
	m_pWarrior->SetPosition(D3DXVECTOR3(25, 0, 0));
	m_pWizard->SetPosition(D3DXVECTOR3(-25, 0, 0));
	m_pWizard->SetState(IDLE);
	m_pWarrior->SetState(IDLE);
	m_pWizard->SetMesh(LoadManager::sharedManager()->m_pMageMesh);
	m_pWarrior->SetMesh(LoadManager::sharedManager()->m_pVarianMesh);
	m_pWizard->SetType(WIZARD);
	m_pWarrior->SetType(KNIGHT);

	//재질설정
	CMaterial *pMaterial = new CMaterial();
	pMaterial->AddRef();
	pMaterial->m_Material.m_d3dxcDiffuse = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.0f);//D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pMaterial->m_Material.m_d3dxcAmbient = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.0f);//D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pMaterial->m_Material.m_d3dxcSpecular = D3DXCOLOR(0.1f, 0.1f, 0.1f, 5.0f);// D3DXCOLOR(1.0f, 1.0f, 1.0f, 5.0f);
	pMaterial->m_Material.m_d3dxcEmissive = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);

	m_pWizard->SetMaterial(pMaterial);
	m_pWarrior->SetMaterial(pMaterial);

	m_pAniShaders->AddObject(m_pWarrior);
	m_pAniShaders->AddObject(m_pWizard);
}

void SelectScene::AnimateObject(float _dt)
{
	m_pWarrior->Animate(_dt);
	m_pWizard->Animate(_dt);
}
void SelectScene::RenderObject(ID3D11DeviceContext*pd3dDeviceContext, float fTimeElapsed, CCamera *pCamera)
{
	m_pAniShaders->Render(pd3dDeviceContext);

	m_pAniShaders->UpdateShaderVariables(pd3dDeviceContext, &m_pWarrior->m_d3dxmtxWorld);
	m_pWarrior->Render(pd3dDeviceContext, fTimeElapsed, NULL);
	m_pAniShaders->UpdateShaderVariables(pd3dDeviceContext, &m_pWizard->m_d3dxmtxWorld);
	m_pWizard->Render(pd3dDeviceContext, fTimeElapsed, NULL);
}
