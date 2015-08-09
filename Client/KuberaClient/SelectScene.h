#pragma once
#include "UIClass.h"
#include "UIObject.h"
#include "header.h"
#include "Shader.h"
#include "Camera.h"
#include "HeroObject.h"
#include "Light.h"

#define SLECTSCENE_MAX_UI 6

class SelectScene
{
private:

	int m_Count;
	
	CAnimationShader* m_pAniShaders;
	HeroObject *m_pWarrior;
	HeroObject *m_pWizard;

	//UI
	UIClass* m_pUI[SLECTSCENE_MAX_UI];
	CUIShader *m_pUIShaders;
	UIObject* m_pUIObjects[SLECTSCENE_MAX_UI];

	int m_nWndClientWidth;
	int m_nWndClientHeight;
	int m_nPrevWndClientWidth;
	int m_nPrevWndClientHeight;

	float m_ArrowWidth, m_ArrowHeight; //화살표
	float m_TeamWidth, m_TeamHeight; //팀
	float m_ChoiceButtonWidth, m_ChoiceButtonHeight; //캐릭터 번호

public:
	SelectScene();
	~SelectScene();
	CCamera* m_pCamera;
	void BuildObject(ID3D11Device *m_pd3dDevice);
	void AnimateObject(float _dt);
	void RenderObject(ID3D11DeviceContext*pd3dDeviceContext, float fTimeElapsed, CCamera *pCamera);
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	//ui
	void CreateUI(ID3D11Device *pd3dDevice,int  _wndWidth,int  _wndHeight);
	void RenderUI(ID3D11DeviceContext *pd3dDeviceContext, int _wndWidth, int _wndHeight);
	
};