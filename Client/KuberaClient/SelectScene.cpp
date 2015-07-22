#pragma once
#include "SelectScene.h"
#include "LoadManager.h"
#include "HeroManager.h"

SelectScene::SelectScene()
{
	m_pLights = NULL;
	m_pd3dcbLights = NULL;
}
SelectScene::~SelectScene()
{

}

void SelectScene::BuildObject(ID3D11Device *m_pd3dDevice)
{
	m_pAniShaders = LoadManager::sharedManager()->m_pAnimationShaders;
	m_pWarrior = new HeroObject();
	m_pWarrior->SetHP(50);
	m_pWarrior->SetTeam(1);
	m_pWizard = new HeroObject();
	m_pWizard->SetHP(50);
	m_pWizard->SetTeam(1);
	m_pWarrior->SetPosition(D3DXVECTOR3(25, 0, 0));
	m_pWizard->SetPosition(D3DXVECTOR3(-1, 0, 0));
	m_pWarrior->SetScale(D3DXVECTOR3(0.1, 0.1, 0.1));
	m_pWizard->SetScale(D3DXVECTOR3(0.1, 0.1, 0.1));
	m_pWizard->SetState(IDLE);
	m_pWarrior->SetState(IDLE);
	m_pWizard->SetMesh(LoadManager::sharedManager()->m_pWizardMesh);
	m_pWarrior->SetMesh(LoadManager::sharedManager()->m_pWarriorMesh);
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

	CreateLightShaderVariables(m_pd3dDevice);
}

void SelectScene::AnimateObject(float _dt)
{
	if(m_pLights && m_pd3dcbLights) 
	{
		m_pLights->m_d3dxvCameraPosition =  D3DXVECTOR4(m_pCamera->m_CameraPos, 1.0f);

		//히어로 조명
		m_pLights->m_pLights[0].m_d3dxvPosition = m_pWarrior->GetPosition() + D3DXVECTOR3(0, 350, 0);
		m_pLights->m_pLights[1].m_d3dxvPosition = m_pWizard->GetPosition() + D3DXVECTOR3(0, 350, 0);
	}

	m_pWarrior->Animate(_dt);
	m_pWizard->Animate(_dt);
}
void SelectScene::RenderObject(ID3D11DeviceContext*pd3dDeviceContext, float fTimeElapsed, CCamera *pCamera)
{
	if(m_pLights && m_pd3dcbLights) UpdateLightShaderVariable(pd3dDeviceContext, m_pLights);

	m_pAniShaders->Render(pd3dDeviceContext);
	m_pAniShaders->UpdateShaderVariables(pd3dDeviceContext, &m_pWarrior->m_d3dxmtxWorld);
	m_pWarrior->Render(pd3dDeviceContext, fTimeElapsed, pCamera);
	m_pAniShaders->UpdateShaderVariables(pd3dDeviceContext, &m_pWizard->m_d3dxmtxWorld);
	m_pWizard->Render(pd3dDeviceContext, fTimeElapsed, pCamera);
}

void SelectScene::CreateLightShaderVariables(ID3D11Device *pd3dDevice)
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
void SelectScene::UpdateLightShaderVariable(ID3D11DeviceContext *pd3dDeviceContext, LIGHTS *pLights)
{
	D3D11_MAPPED_SUBRESOURCE d3dMappedResource;
	pd3dDeviceContext->Map(m_pd3dcbLights, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3dMappedResource);
	LIGHTS *pcbLight = (LIGHTS *)d3dMappedResource.pData;
	memcpy(pcbLight, pLights, sizeof(LIGHTS));
	pd3dDeviceContext->Unmap(m_pd3dcbLights, 0);
	pd3dDeviceContext->PSSetConstantBuffers(PS_SLOT_LIGHT, 1, &m_pd3dcbLights);
}
void SelectScene::ReleaseLightShaderVariables()
{
	if (m_pLights) delete m_pLights;
	if (m_pd3dcbLights) m_pd3dcbLights->Release();
}