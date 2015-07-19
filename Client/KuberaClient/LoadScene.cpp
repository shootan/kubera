#include "LoadScene.h"
#include "LoadManager.h"

LoadScene::LoadScene()
{
	m_Count = 0;
}

LoadScene::~LoadScene()
{

}

void LoadScene::LoadData(ID3D11Device *m_pd3dDevice, ID3D11DeviceContext* m_pd3dDeviceContext)
{
	m_pUIShaders->UpdateShaderVariables(m_pd3dDeviceContext, &m_BackImageObject->m_d3dxmtxWorld);
	m_BackImageObject->Render(m_pd3dDeviceContext, 0, 0);
	for(int i=0; i<m_Count; i++)
	{
		m_pUIShaders->UpdateShaderVariables(m_pd3dDeviceContext, &m_ButtonImageObject[i]->m_d3dxmtxWorld);
		m_ButtonImageObject[i]->Render(m_pd3dDeviceContext, 5+(i*17), 675);
	}
  	switch(m_Count)
  	{
  	case 2:
  		LoadManager::sharedManager()->LoadShaderInstancing1(m_pd3dDevice);
  		break;
	case 4:
		LoadManager::sharedManager()->LoadShaderInstancing2(m_pd3dDevice);
		break;
	case 5:
		LoadManager::sharedManager()->LoadShaderInstancing3(m_pd3dDevice);
		break;
	case 8:
		LoadManager::sharedManager()->LoadShaderInstancing4(m_pd3dDevice);
		break;
  	case 16:
  		LoadManager::sharedManager()->LoadWarriorModel(m_pd3dDevice);
  		break;
  	case 21:
  		LoadManager::sharedManager()->LoadWizardModel(m_pd3dDevice);
  		break;
  	case 26:
  		LoadManager::sharedManager()->LoadParticle(m_pd3dDevice);
  		break;
  	case 29:
  		LoadManager::sharedManager()->LoadMesh(m_pd3dDevice);
  		break;
  	}
	m_Count++;
}

void LoadScene::BuildObject(ID3D11Device *m_pd3dDevice)
{
	m_BackImage = new UIClass(m_pd3dDevice);
	m_BackImage->Initialize(m_pd3dDevice, m_Width, m_Height, L"UI/BackImage.png",m_Width, m_Height);
	m_BackImageObject =  new UIObject();
	m_BackImageObject->SetUI(m_BackImage);
	m_pUIShaders = LoadManager::sharedManager()->m_pUIShaders;
	m_pUIShaders->AddObject(m_BackImageObject);

	for(int i=0; i<30; i++)
	{
		m_ButtonImage[i] = new UIClass(m_pd3dDevice);
		m_ButtonImage[i]->Initialize(m_pd3dDevice, m_Width, m_Height, L"UI/bar1.png",11, 35);
		m_ButtonImageObject[i] = new UIObject();
		m_ButtonImageObject[i]->SetUI(m_ButtonImage[i]);
		m_pUIShaders->AddObject(m_ButtonImageObject[i]);
	}
}