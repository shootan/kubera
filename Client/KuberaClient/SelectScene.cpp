#pragma once
#include "SelectScene.h"
#include "LoadManager.h"
#include "HeroManager.h"

SelectScene::SelectScene()
{

	m_ArrowWidth = 20.f; m_ArrowHeight = 30.f;
	m_TeamWidth = 200.f; m_TeamHeight = 100.f;
	m_ChoiceButtonWidth = 30.f, m_ChoiceButtonHeight = 30.f; //ĳ���� ��ȣ
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
	m_pWarrior->SetPosition(D3DXVECTOR3(-25, 0, 0));
	m_pWizard->SetPosition(D3DXVECTOR3(25, 0, 0));
	m_pWizard->SetState(IDLE);
	m_pWarrior->SetState(IDLE);
	m_pWizard->SetMesh(LoadManager::sharedManager()->m_pMageMesh);
	m_pWarrior->SetMesh(LoadManager::sharedManager()->m_pVarianMesh);
	m_pWizard->SetType(WIZARD);
	m_pWarrior->SetType(KNIGHT);

	//��������
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

bool SelectScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)	
	{
		/*�������� ũ�Ⱑ ����� ��(����� ��Alt+Enter�� ��ü ȭ�� ���� ������ ���� ��ȯ�� ��) ���� ü���� �ĸ���� ũ�⸦ �����ϰ� �ĸ���ۿ� ���� ���� Ÿ�� �並 �ٽ� �����Ѵ�. */
	case WM_SIZE:
		{
			m_nWndClientWidth = LOWORD(lParam);
			m_nWndClientHeight = HIWORD(lParam);

			m_ArrowWidth = (m_nWndClientWidth*m_ArrowWidth) / m_nPrevWndClientWidth;
			m_ArrowHeight = (m_nWndClientHeight*m_ArrowHeight) / m_nPrevWndClientHeight; 
			m_TeamWidth = (m_nWndClientWidth*m_TeamWidth) / m_nPrevWndClientWidth;
			m_TeamHeight = (m_nWndClientHeight*m_TeamHeight) / m_nPrevWndClientHeight; 
			m_ChoiceButtonWidth = (m_nWndClientWidth*m_ChoiceButtonWidth) / m_nPrevWndClientWidth;
			m_ChoiceButtonHeight = (m_nWndClientHeight*m_ChoiceButtonHeight) / m_nPrevWndClientHeight; 

			m_nPrevWndClientWidth = m_nWndClientWidth;
			m_nPrevWndClientHeight = m_nWndClientHeight;

			for(int i=0; i<SLECTSCENE_MAX_UI; i++)
			{
				if(m_pUIObjects[i]->GetUI()->GetScreenW() != m_nWndClientWidth || m_pUIObjects[i]->GetUI()->GetScreenH() != m_nWndClientHeight)
					m_pUIObjects[i]->GetUI()->SetScreenWH(m_nWndClientWidth, m_nWndClientHeight);
			}

			m_pUI[0]->SetBitmapWH(m_ArrowWidth, m_ArrowHeight);
			m_pUI[1]->SetBitmapWH(m_ArrowWidth, m_ArrowHeight);
			m_pUI[2]->SetBitmapWH(m_TeamWidth, m_TeamHeight);
			m_pUI[3]->SetBitmapWH(m_TeamWidth, m_TeamHeight);
			m_pUI[4]->SetBitmapWH(m_ChoiceButtonWidth, m_ChoiceButtonHeight);
			m_pUI[5]->SetBitmapWH(m_ChoiceButtonWidth, m_ChoiceButtonHeight);

			break;
		}
	}

	return(false);
}

void SelectScene::CreateUI(ID3D11Device *pd3dDevice,int  _wndWidth,int  _wndHeight)
{
	m_nWndClientWidth = _wndWidth;
	m_nWndClientHeight = _wndHeight;
	m_nPrevWndClientWidth = m_nWndClientWidth;
	m_nPrevWndClientHeight = m_nWndClientHeight;

	m_pUIShaders = LoadManager::sharedManager()->m_pUIShaders_Select;

	for(int i=0; i<SLECTSCENE_MAX_UI; i++)
		m_pUI[i] = new UIClass(pd3dDevice);

	m_pUI[0]->Initialize(pd3dDevice, _wndWidth, _wndHeight, L"UI/red_arrow.png",m_ArrowWidth, m_ArrowHeight); //���� ȭ��ǥ
	m_pUI[1]->Initialize(pd3dDevice, _wndWidth, _wndHeight, L"UI/blue_arrow.png",m_ArrowWidth, m_ArrowHeight); //��� ȭ��ǥ
	m_pUI[2]->Initialize(pd3dDevice, _wndWidth, _wndHeight, L"UI/red_team.png",m_TeamWidth, m_TeamHeight); // ���� ��
	m_pUI[3]->Initialize(pd3dDevice, _wndWidth, _wndHeight, L"UI/blue_team.png",m_TeamWidth, m_TeamHeight); //��� ��
	m_pUI[4]->Initialize(pd3dDevice, _wndWidth, _wndHeight, L"UI/characterchoice_1.png",m_ChoiceButtonWidth, m_ChoiceButtonHeight); // ���� ��
	m_pUI[5]->Initialize(pd3dDevice, _wndWidth, _wndHeight, L"UI/characterchoice_2.png",m_ChoiceButtonWidth, m_ChoiceButtonHeight); //��� ��

	for(int i=0; i<SLECTSCENE_MAX_UI; i++)
	{
		m_pUIObjects[i] = new UIObject();
		m_pUIObjects[i]->SetUI(m_pUI[i]);
	}

	for(int i=0; i<SLECTSCENE_MAX_UI; i++)
		m_pUIShaders->AddObject(m_pUIObjects[i]);
}

void SelectScene::RenderUI(ID3D11DeviceContext *pd3dDeviceContext,int  _wndWidth,int  _wndHeight)
{
	m_pUIShaders->Render(pd3dDeviceContext);


	if(HeroManager::sharedManager()->GetTeam() == 1)
	{
		//���� ��
		m_pUIShaders->UpdateShaderVariables(pd3dDeviceContext, &m_pUIObjects[2]->m_d3dxmtxWorld);
		m_pUIObjects[2]->Render(pd3dDeviceContext, _wndWidth/2 - m_TeamWidth/2, _wndHeight - m_TeamHeight*2 );

		if(HeroManager::sharedManager()->GetType() == 1)
		{
			//���� ȭ��ǥ
			m_pUIShaders->UpdateShaderVariables(pd3dDeviceContext, &m_pUIObjects[0]->m_d3dxmtxWorld);
			m_pUIObjects[0]->Render(pd3dDeviceContext, _wndWidth/4 - m_ArrowWidth/2 - m_ArrowWidth/2 , m_ArrowHeight*2 );
		}
		else if(HeroManager::sharedManager()->GetType() == 2)
		{
			//���� ȭ��ǥ
			m_pUIShaders->UpdateShaderVariables(pd3dDeviceContext, &m_pUIObjects[0]->m_d3dxmtxWorld);
			m_pUIObjects[0]->Render(pd3dDeviceContext, _wndWidth*3/4 - m_ArrowWidth/2 - m_ArrowWidth/2 , m_ArrowHeight*2 );
		}
	}
	else if(HeroManager::sharedManager()->GetTeam() == 2)
	{
		//��� ��
		m_pUIShaders->UpdateShaderVariables(pd3dDeviceContext, &m_pUIObjects[3]->m_d3dxmtxWorld);
		m_pUIObjects[3]->Render(pd3dDeviceContext, _wndWidth/2 - m_TeamWidth/2, _wndHeight - m_TeamHeight*2);

		if(HeroManager::sharedManager()->GetType() == 1)
		{
			//��� ȭ��ǥ
			m_pUIShaders->UpdateShaderVariables(pd3dDeviceContext, &m_pUIObjects[1]->m_d3dxmtxWorld);
			m_pUIObjects[1]->Render(pd3dDeviceContext, _wndWidth/4 - m_ArrowWidth/2 + m_ArrowWidth/2 , m_ArrowHeight*2);
		}
		else if(HeroManager::sharedManager()->GetType() == 2)
		{
			//��� ȭ��ǥ
			m_pUIShaders->UpdateShaderVariables(pd3dDeviceContext, &m_pUIObjects[1]->m_d3dxmtxWorld);
			m_pUIObjects[1]->Render(pd3dDeviceContext, _wndWidth*3/4 - m_ArrowWidth/2 + m_ArrowWidth/2 , m_ArrowHeight*2);
		}
	}


	//�ƴ��÷��̾ ����
	/*if(OtherPlayerManager::sharedManager()->GetTeam() == 1)
	{
		if(OtherPlayerManager::sharedManager()->GetType() == 1)
		{

		}
		else if(OtherPlayerManager::sharedManager()->GetType() == 2)
		{

		}
	}
	else if(OtherPlayerManager::sharedManager()->GetTeam() == 2)
	{
		if(OtherPlayerManager::sharedManager()->GetType() == 1)
		{

		}
		else if(OtherPlayerManager::sharedManager()->GetType() == 2)
		{

		}
	}*/

	////���� ȭ��ǥ
	//m_pUIShaders->UpdateShaderVariables(pd3dDeviceContext, &m_pUIObjects[0]->m_d3dxmtxWorld);
	//m_pUIObjects[0]->Render(pd3dDeviceContext, _wndWidth/4 - m_ArrowWidth/2 - m_ArrowWidth/2 , m_ArrowHeight*2 );
	////��� ȭ��ǥ
	//m_pUIShaders->UpdateShaderVariables(pd3dDeviceContext, &m_pUIObjects[1]->m_d3dxmtxWorld);
	//m_pUIObjects[1]->Render(pd3dDeviceContext, _wndWidth*3/4 - m_ArrowWidth/2 + m_ArrowWidth/2 , m_ArrowHeight*2);
	////���� ��
	//m_pUIShaders->UpdateShaderVariables(pd3dDeviceContext, &m_pUIObjects[2]->m_d3dxmtxWorld);
	//m_pUIObjects[2]->Render(pd3dDeviceContext, _wndWidth/4 - m_TeamWidth/2, _wndHeight - m_TeamHeight*2 );
	////��� ��
	//m_pUIShaders->UpdateShaderVariables(pd3dDeviceContext, &m_pUIObjects[3]->m_d3dxmtxWorld);
	//m_pUIObjects[3]->Render(pd3dDeviceContext, _wndWidth*3/4 - m_TeamWidth/2, _wndHeight - m_TeamHeight*2);

	//ĳ���� ��ư1
	m_pUIShaders->UpdateShaderVariables(pd3dDeviceContext, &m_pUIObjects[4]->m_d3dxmtxWorld);
	m_pUIObjects[4]->Render(pd3dDeviceContext, _wndWidth/4 - m_ChoiceButtonWidth/2 - m_ChoiceButtonWidth*2, m_ChoiceButtonHeight*4 );
	//ĳ���� ��ư2
	m_pUIShaders->UpdateShaderVariables(pd3dDeviceContext, &m_pUIObjects[5]->m_d3dxmtxWorld);
	m_pUIObjects[5]->Render(pd3dDeviceContext, _wndWidth*3/4 - m_ChoiceButtonWidth/2 - m_ChoiceButtonWidth*2, m_ChoiceButtonHeight*4);
}