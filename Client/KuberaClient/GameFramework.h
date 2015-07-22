#pragma once
#include "header.h"
#include "Timer.h"
#include "Scene.h"
#include "Network.h"
#include "Camera.h"
#include "UIClass.h"
#include "Shader.h"
#include "UIObject.h"

#define MAX_UI 9

class CGameFramework
{
private:
	HINSTANCE m_hInstance;
	HWND m_hWnd; 

	int m_nWndClientWidth;
	int m_nWndClientHeight;
	int m_nPrevWndClientWidth;
	int m_nPrevWndClientHeight;

	PlayerPacket HeroInfo;

	CTimer m_GameTimer;
	CScene *m_pScene;
	_TCHAR m_pszBuffer[50];

	CCamera *m_pCamera; 
	CCamera *m_pCameraMinimap;
	CCamera *m_pUICamera;

	PlayerPacket m_MyHero;

	float m_CameraPosX;
	float m_CameraPosZ;
	float m_CameraZoom;
	float m_CameraUpDown;

	int	m_SendTick;

	//디바이스 인터페이스에 대한 포인터이다. 주로 리소스를 생성하기 위하여 필요하다.
	ID3D11Device *m_pd3dDevice;
	//스왑 체인 인터페이스에 대한 포인터이다. 주로 디스플레이를 제어하기 위하여 필요하다.
	IDXGISwapChain *m_pDXGISwapChain;
	//디바이스 컨텍스트에 대한 포인터이다. 주로 파이프라인 설정을 하기 위하여 필요하다.
	ID3D11DeviceContext *m_pd3dDeviceContext;
	//렌더 타겟 뷰 인터페이스에 대한 포인터이다. 
	ID3D11RenderTargetView *m_pd3dRenderTargetView;
	ID3D11Texture2D *m_pd3dDepthStencilBuffer;
	ID3D11DepthStencilView *m_pd3dDepthStencilView;

	ID3D11DepthStencilState* m_depthStencilState;
	ID3D11DepthStencilState* m_depthDisabledStencilState;

	D3DXMATRIX m_orthoMatrix;

	CDXUTTextHelper* m_pTxtHelper;
	CDXUTTextHelper* m_pTxtHelper2;
	CDXUTTextHelper* m_pCharacterInfo;

	CDXUTDialogResourceManager m_DialogResourceManager;

	// Network
	Network Net;
	float time;

	//UI
	UIClass* m_pUI[MAX_UI];
	CUIShader *m_pUIShaders;
	UIObject* m_pUIObjects[MAX_UI];

	float m_UIskillbarWidth, m_UIskillbarHeight; //스킬바 UI
	float m_UIMinimapWidth, m_UIMinimapHeight;   //미니맵 UI
	float m_UIInfoWidth, m_UIInfoHeight;			//캐릭터 정보창 UI
	float m_UIScoreWidth, m_UIScoreHeight;		//점수판 UI

	float m_SwordWidth, m_SwordHeight; //특성 아이콘
	float m_ShielWidth, m_ShielHeight;
	float m_BootsWidth, m_BootsHeight;

	float m_HpbarRWidth, m_HpbarRHeight; //hp바
	float m_HpbarGWidth, m_HpbarGHeight;
public:
	CGameFramework();
	~CGameFramework();

	bool OnCreate(HINSTANCE hInstance, HWND hMainWnd);
	void OnDestroy();
	void ExchangeInfo();
	void SendHeroData();

	//디바이스, 스왑 체인, 디바이스 컨텍스트, 디바이스와 관련된 뷰를 생성하는 함수이다. 

	bool CreateRenderTargetDepthStencilView();
	bool CreateDirect3DDisplay();

	//렌더링할 메쉬, 객체를 생성하고 소멸하는 함수이다. 
	void BuildObjects();
	void ReleaseObjects();

	//프레임워크의 핵심(사용자 입력, 애니메이션, 렌더링)을 구성하는 함수이다. 
	void ProcessInput();
	void AnimateObjects();
	void FrameAdvance();

	//윈도우의 메시지(키보드, 마우스 입력)를 처리하는 함수이다. 
	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	//카메라 움직임
	void SetCameraPos();
	void SetViewport(ID3D11DeviceContext *pd3dDeviceContext, DWORD xStart, DWORD yStart, DWORD nWidth, DWORD nHeight, float fMinZ=0.0f, float fMaxZ=1.0f);

	void RenderText();

	void TurnZBufferOn();
	void TurnZBufferOff();

	//블렌딩
	ID3D11BlendState* m_UIEnableBlendingState;
	ID3D11BlendState* m_UIDisableBlendingState;
	void TurnOnAlphaBlending(ID3D11DeviceContext *pd3dDeviceContext, ID3D11BlendState* blendstate);
	void TurnOffAlphaBlending(ID3D11DeviceContext *pd3dDeviceContext, ID3D11BlendState* blendstate);
	HRESULT CreateBlend(ID3D11Device *pd3dDevice);

	//UI
	void SetUIUpdate();
};
