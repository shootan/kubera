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

	// 디바이스 인터페이스에 대한 포인터이다. 주로 리소스를 생성하기 위하여 필요하다.
	ID3D11Device *m_pd3dDevice;
	
	// 스왑 체인 인터페이스에 대한 포인터이다. 주로 디스플레이를 제어하기 위하여 필요하다.
	IDXGISwapChain *m_pDXGISwapChain;

	// 디바이스 컨텍스트에 대한 포인터이다. 주로 파이프라인 설정을 하기 위하여 필요하다.
	ID3D11DeviceContext *m_pd3dDeviceContext;

	// 렌더 타켓 뷰 인터페이스에 대한 포인터이다.
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
	

	// 디바이스, 스왑체인, 디바이스 컨텍스트, 디바이스와 관련된 뷰를 생성하는 함수이다.
	bool CreateRenderTargetDepthStencilView();

	bool CreateDirect3DDisplay();

	// 렌더링할 메쉬, 객체를 생성하고 소멸하는 함수이다.
	void BuildObjects();
	void ReleaseObjects();

	// 프레임워크의 핵식 (사용자 입력, 에니메이션, 렌더링) 을 구성하는 함수이다.
	void ProcessInput();
	void AnimateObjects();
	void FrameAdvance();

	// 윈도우의 메시지 (키보드, 마우스 입력) 를 처리하는 함수이다.
	void OnProcessingMouseMessage (HWND hwnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage (HWND hwnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnprocessingWindowMessage (HWND hwnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

private:
	// 게임 프레임워크에서 사용할 타이머
	CGameTimer m_GameTimer;

	// 게임의 장면을 관리하는 객체에 대한 포인터를 나타낸다
	CScene *m_pScene;

	// 프레임 레이트를 주 윈도우의 캡션에 출력하기 위한 문자열이다
	_TCHAR m_pszBuffer[50];
	
	POINT m_ptOldCursorPos;
};

