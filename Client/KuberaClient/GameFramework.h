#pragma once
#include "header.h"
#include "Timer.h"
#include "Scene.h"
#include "Network.h"
#include "Camera.h"
#include "UIClass.h"
#include "Shader.h"
#include "UIObject.h"
#include "LoadScene.h"
#include "SelectScene.h"
#include "ST.h"

#define MAX_UI_FONT 11

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
	SelectScene *m_pSelectScene;
	LoadScene *m_pLoadScene;
	_TCHAR m_pszBuffer[50];

	CCamera *m_pCamera; 
	CCamera *m_pCameraMinimap;
	CCamera *m_pUICamera;
	CCamera *m_pSelectCamera;

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
	CDXUTTextHelper* m_pUIFont[MAX_UI_FONT];

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

};
