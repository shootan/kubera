#pragma once

#include "Object.h"
#include "Shader.h"

class CScene
{
public:
	CScene();
	~CScene();

	bool OnprocessingMouseMessage (HWND hwnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	bool OnProcessingKeyboardMessage (HWND hwnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	
	void BuildObjects (ID3D11Device *pd3dDevice);
	void ReleaseObjects();

	bool ProcessInput();
	void AnimateObjects (float fTimeElapsed);
	void Render (ID3D11DeviceContext *pd3dDeviceContext, CCamera *pCamera);

private:
	CSceneShader **m_ppShaders;
	int m_nShaders;

	CGameObject **m_ppObjects;
	int m_nObjects;

	CSceneShader *m_pShaders;
};

