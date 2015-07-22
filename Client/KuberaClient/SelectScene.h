#pragma once
#include "UIClass.h"
#include "UIObject.h"
#include "header.h"
#include "Shader.h"
#include "Camera.h"
#include "HeroObject.h"
#include "Light.h"
class SelectScene
{
private:

	int m_Count;
	int m_Width;
	int m_Height;
	UIClass* m_TeamImage[2];
	UIObject* m_TeamImageObject[2];
	CUIShader* m_pUIShaders;
	
	CAnimationShader* m_pAniShaders;
	HeroObject *m_pWarrior;
	HeroObject *m_pWizard;

	LIGHTS *m_pLights;
	ID3D11Buffer *m_pd3dcbLights;


public:
	SelectScene();
	~SelectScene();
	CCamera* m_pCamera;
	void BuildObject(ID3D11Device *m_pd3dDevice);
	void AnimateObject(float _dt);
	void RenderObject(ID3D11DeviceContext*pd3dDeviceContext, float fTimeElapsed, CCamera *pCamera);

	void CreateLightShaderVariables(ID3D11Device *pd3dDevice);
	void UpdateLightShaderVariable(ID3D11DeviceContext *pd3dDeviceContext, LIGHTS *pLights);
	void ReleaseLightShaderVariables();
	
};