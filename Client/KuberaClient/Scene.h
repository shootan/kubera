#pragma once
#include "header.h"
#include "GameObject.h"
#include "Shader.h"
#include "ControlManager.h"

class CScene
{
private:
	CObjectShader **m_ppShaders;
	int m_nShaders;

	CGameObject **m_ppObjects;       
	int m_nObjects;

	int m_MousePosX;
	int m_MousePosY;

	BOOL m_bRbutton;



public:
	CScene(void);
	~CScene(void);

	CModelViewerCamera* m_Camera;
	ControlManager		m_Control;

	bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	void BuildObjects(ID3D11Device *pd3dDevice);
	void ReleaseObjects();

	bool ProcessInput();
	void AnimateObjects(float fTimeElapsed, ID3D11Device *pd3dDevice);
	void Render(ID3D11DeviceContext *pd3dDeviceContext);

	int GetMousePosX();
	int GetMousePosY();

	void AddOtherPlayer(ID3D11Device *pd3dDevice);

	CGameObject* GetObject(int num);
	BOOL m_bJoinOtherPlayer;
	BOOL m_bJoin;
};

