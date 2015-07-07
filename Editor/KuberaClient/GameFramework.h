#pragma once
#include "header.h"
#include "Timer.h"
#include "Scene.h"
#include "Network.h"
#include "Camera.h"	

class CGameFramework
{
private:
	HINSTANCE m_hInstance;
	HWND m_hWnd; 

	int m_nWndClientWidth;
	int m_nWndClientHeight;

	PlayerPacket HeroInfo;

	CTimer m_GameTimer;
	CScene *m_pScene;
	_TCHAR m_pszBuffer[50];

	CCamera *m_pCamera; 
	CCamera *m_pCameraMinimap;

	PlayerPacket m_MyHero;

	float m_CameraPosX;
	float m_CameraPosZ;
	float m_CameraZoom;
	float m_CameraUpDown;

	int	m_SendTick;

	//����̽� �������̽��� ���� �������̴�. �ַ� ���ҽ��� �����ϱ� ���Ͽ� �ʿ��ϴ�.
	ID3D11Device *m_pd3dDevice;
	//���� ü�� �������̽��� ���� �������̴�. �ַ� ���÷��̸� �����ϱ� ���Ͽ� �ʿ��ϴ�.
	IDXGISwapChain *m_pDXGISwapChain;
	//����̽� ���ؽ�Ʈ�� ���� �������̴�. �ַ� ���������� ������ �ϱ� ���Ͽ� �ʿ��ϴ�.
	ID3D11DeviceContext *m_pd3dDeviceContext;
	//���� Ÿ�� �� �������̽��� ���� �������̴�. 
	ID3D11RenderTargetView *m_pd3dRenderTargetView;
	ID3D11Texture2D *m_pd3dDepthStencilBuffer;
	ID3D11DepthStencilView *m_pd3dDepthStencilView;

	CDXUTTextHelper* m_pTxtHelper;
	CDXUTTextHelper* m_pTxtHelper2;
	CDXUTDialogResourceManager m_DialogResourceManager;

	// Network
	Network Net;
	float time;
	
	
public:
	CGameFramework();
	~CGameFramework();

	bool OnCreate(HINSTANCE hInstance, HWND hMainWnd);
	void OnDestroy();
	void ExchangeInfo();
	void SendHeroData();

	//����̽�, ���� ü��, ����̽� ���ؽ�Ʈ, ����̽��� ���õ� �並 �����ϴ� �Լ��̴�. 

	bool CreateRenderTargetDepthStencilView();
	bool CreateDirect3DDisplay();

	//�������� �޽�, ��ü�� �����ϰ� �Ҹ��ϴ� �Լ��̴�. 
	void BuildObjects();
	void ReleaseObjects();

	//�����ӿ�ũ�� �ٽ�(����� �Է�, �ִϸ��̼�, ������)�� �����ϴ� �Լ��̴�. 
	void ProcessInput();
	void AnimateObjects();
	void FrameAdvance();

	//�������� �޽���(Ű����, ���콺 �Է�)�� ó���ϴ� �Լ��̴�. 
	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	//ī�޶� ������
	void SetCameraPos();
	void SetViewport(ID3D11DeviceContext *pd3dDeviceContext, DWORD xStart, DWORD yStart, DWORD nWidth, DWORD nHeight, float fMinZ=0.0f, float fMaxZ=1.0f);

	void RenderText();
};
