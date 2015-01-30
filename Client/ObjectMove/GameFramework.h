#pragma once

#include "Timer.h"
#include "Player.h"

#include "Scene.h"


class CGameFramework
{
private:
	HINSTANCE m_hInstance;
	HWND	  m_hwnd;

	int m_nwndClientWidth;
	int m_nwndClientHeight;

	// ����̽� �������̽��� ���� �������̴�. �ַ� ���ҽ��� �����ϱ� ���Ͽ� �ʿ��ϴ�.
	ID3D11Device *m_pd3dDevice;
	
	// ���� ü�� �������̽��� ���� �������̴�. �ַ� ���÷��̸� �����ϱ� ���Ͽ� �ʿ��ϴ�.
	IDXGISwapChain *m_pDXGISwapChain;

	// ����̽� ���ؽ�Ʈ�� ���� �������̴�. �ַ� ���������� ������ �ϱ� ���Ͽ� �ʿ��ϴ�.
	ID3D11DeviceContext *m_pd3dDeviceContext;

	// ���� Ÿ�� �� �������̽��� ���� �������̴�.
	ID3D11RenderTargetView *m_pd3dRenderTargetView;

	int m_nPlayers;
	CPlayer **m_ppPlayers;

	ID3D11Texture2D *m_pd3dDepthStencilBuffer;
	ID3D11DepthStencilView *m_pd3dDepthStencilView;

public:
	CGameFramework(void);
	~CGameFramework(void);

	bool OnCreate (HINSTANCE hInstance, HWND hMainwnd);
	void OnDestroy ();
	
	//void SetViewPort();
	

	// ����̽�, ����ü��, ����̽� ���ؽ�Ʈ, ����̽��� ���õ� �並 �����ϴ� �Լ��̴�.
	bool CreateRenderTargetDepthStencilView();

	bool CreateDirect3DDisplay();

	// �������� �޽�, ��ü�� �����ϰ� �Ҹ��ϴ� �Լ��̴�.
	void BuildObjects();
	void ReleaseObjects();

	// �����ӿ�ũ�� �ٽ� (����� �Է�, ���ϸ��̼�, ������) �� �����ϴ� �Լ��̴�.
	void ProcessInput();
	void AnimateObjects();
	void FrameAdvance();

	// �������� �޽��� (Ű����, ���콺 �Է�) �� ó���ϴ� �Լ��̴�.
	void OnProcessingMouseMessage (HWND hwnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage (HWND hwnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnprocessingWindowMessage (HWND hwnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

private:
	// ���� �����ӿ�ũ���� ����� Ÿ�̸�
	CGameTimer m_GameTimer;

	// ������ ����� �����ϴ� ��ü�� ���� �����͸� ��Ÿ����
	CScene *m_pScene;

	// ������ ����Ʈ�� �� �������� ĸ�ǿ� ����ϱ� ���� ���ڿ��̴�
	_TCHAR m_pszBuffer[50];
	
	POINT m_ptOldCursorPos;
};

