#include <windows.h>
#include <tchar.h>
#include <stdio.h>	// _stprintf 함수 사용
#include <math.h>	// floor 함수 사용
#pragma comment(lib, "msimg32") // GradientFill 함수 사용

#include "CCustomProgressBar.h"

CCustomProgressBar::CCustomProgressBar()
{
	m_hParent = NULL;
	m_hProgress = NULL;
	m_dCurPercent = 0;
	m_uDirection = 0;
	m_bShowAtCreation = FALSE;
	m_x = 0;
	m_y = 0;
	m_cx = 0;
	m_cy = 0;		
	m_hEvent = NULL;
}

CCustomProgressBar::CCustomProgressBar(HWND hParent, unsigned uDirection, BOOL bShowAtCreation, int x, int y, int cx, int cy)
{
	Create(hParent, uDirection, bShowAtCreation, x, y, cx, cy);
	return;
}

CCustomProgressBar::~CCustomProgressBar()
{
	if (m_hProgress && IsWindow(m_hProgress)) SendMessage(m_hProgress, WM_CLOSE, 0, 0);
}

HWND CCustomProgressBar::Create(HWND hParent, unsigned uDirection, BOOL bShowAtCreation, int x, int y, int cx, int cy)
{
	DWORD tid;	
	HINSTANCE hInstance = GetModuleHandle(NULL);
	TCHAR szTitle[] = _T("");
	TCHAR szClass[] = _T("CustomProgressBar");
	WNDCLASSEX wcex = {0};

	m_hParent = hParent;
	m_hProgress = NULL;
	m_uDirection = uDirection;
	m_bShowAtCreation = bShowAtCreation;
	m_x = x;
	m_y = y;
	m_cx = cx;
	m_cy = cy;
	m_hEvent = NULL;

	///// 부모 윈도우가 없는 경우 스레드로 독립적인 윈도우를 생성한다.
	if (!m_hParent) {
		m_hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		CloseHandle(CreateThread(NULL, 0, CreateNoParentByThread, this, 0, &tid));
		WaitForSingleObject(m_hEvent, 3000);
		CloseHandle(m_hEvent);
		goto finish;
	}

	///// 자식의 형태로 삽입하는 경우
	wcex.cbSize			= sizeof(WNDCLASSEX);
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)ProgressBarProc;
	wcex.hInstance		= hInstance;
	wcex.hbrBackground	= (HBRUSH)GetStockObject(NULL_BRUSH);
	wcex.lpszClassName	= szClass;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
//	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 4;	// 윈도우별 CCustomProgressBar 객체 주소를 저장하기 위한 4바이트 공간 할당
//	wcex.hIcon			= hIcon;
//	wcex.lpszMenuName	= NULL;
	RegisterClassEx(&wcex);

	m_hProgress = CreateWindow(szClass, szTitle, m_bShowAtCreation ? WS_CHILD | WS_VISIBLE : WS_CHILD,
							   m_x, m_y, m_cx, m_cy,
							   m_hParent, (HMENU)NULL, hInstance, this);

finish:
	if (!m_hProgress) {
		MessageBoxW(NULL, L"프로그레스바 생성에 실패하였습니다.", L"CCustomProgressBar::Create", MB_ICONWARNING | MB_TOPMOST);
	}
	return m_hProgress;
}

DWORD WINAPI CCustomProgressBar::CreateNoParentByThread(LPVOID lp)
{
	CCustomProgressBar *pcpb = (CCustomProgressBar *)lp;

	HINSTANCE hInstance = GetModuleHandle(NULL);
	TCHAR szTitle[] = _T("");
	TCHAR szClass[] = _T("CustomProgressBarNoParent");
	WNDCLASSEX wcex = {0};
	MSG msg;
	
	wcex.cbSize			= sizeof(WNDCLASSEX);
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)ProgressBarProc;
	wcex.hInstance		= hInstance;
	wcex.hbrBackground	= (HBRUSH)GetStockObject(NULL_BRUSH);
	wcex.lpszClassName	= szClass;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
//	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 4;	// 윈도우별 CCustomProgressBar 객체 주소를 저장하기 위한 4바이트 공간 할당
//	wcex.hIcon			= hIcon;
//	wcex.lpszMenuName	= NULL;
	RegisterClassEx(&wcex);

	// WS_EX_TOOLWINDOW 플래그를 줘야 작업표시줄에 나타나지 않는다.
	pcpb->m_hProgress = CreateWindowEx(WS_EX_TOOLWINDOW, szClass, szTitle, WS_POPUP,
									   pcpb->m_x, pcpb->m_y, pcpb->m_cx, pcpb->m_cy,
									   NULL, (HMENU)NULL, hInstance, pcpb);
	SetEvent(pcpb->m_hEvent);

	if (pcpb->m_bShowAtCreation) ShowWindow(pcpb->m_hProgress, SW_SHOW);

	while(GetMessage(&msg, NULL, 0, 0)){
	//	TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (DWORD)(pcpb->m_hProgress);
}

void CCustomProgressBar::SetPercent(double dCurPercent)
{
	m_dCurPercent = dCurPercent;
	if (m_hProgress && IsWindowVisible(m_hProgress)) InvalidateRect(m_hProgress, NULL, 0);
}

void CCustomProgressBar::SetDirection(unsigned int uDirection)
{
	if (!m_hProgress) return;
	m_uDirection = uDirection;
	if (IsWindowVisible(m_hProgress)) InvalidateRect(m_hProgress, NULL, 0);
}

BOOL CCustomProgressBar::Show(int nCmdShow)
{
	if (!m_hProgress) return FALSE;
	return ShowWindow(m_hProgress, nCmdShow);
}

LRESULT CALLBACK CCustomProgressBar::ProgressBarProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	CCustomProgressBar* pcpb;
	PAINTSTRUCT ps;
	HDC hdc;
	HRGN hRgn;
	RECT rc;	
	HFONT hFont, hOldFont;
	TRIVERTEX vert[2];
	GRADIENT_RECT rcGradient;
	TCHAR szText[256];

	switch(msg){
	case WM_CREATE:
		SetWindowLong(hWnd, 0, long(((LPCREATESTRUCT)lParam)->lpCreateParams)); // CreateWindow에서 마지막 인수로 전달한 객체의 주소를 4바이트 할당 공간에 입력해 준다. 
		return 0;

	/*
	// 독립적인 형태인 경우 마우스 좌클릭 & 드래그로 움직이고자 할 때
	case WM_LBUTTONDOWN:
		if (!((CCustomProgressBar *)GetWindowLong(hWnd, 0))->m_hParent) SendMessage(hWnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
		break;
	*/

	case WM_ERASEBKGND: // 덮는 방식으로 그리기 때문에 깜빡임을 없애기 위해 리턴처리.
		return 0;

	case WM_PAINT:
		pcpb = (CCustomProgressBar *)GetWindowLong(hWnd, 0);	// 해당 윈도우와 맞물린 객체의 주소를 구한다.
		hdc = BeginPaint(hWnd, &ps);

		// 윈도우의 크기가 변하는 경우 고려 그때그때 구해준다.
		GetClientRect(hWnd, &rc);
		pcpb->m_cx = rc.right - rc.left;
		pcpb->m_cy = rc.bottom - rc.top;

		hFont = CreateFont(18, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("Arial"));
		hOldFont = (HFONT)SelectObject(hdc, hFont);
		SetBkMode(hdc, TRANSPARENT);

		// 실수형 문자열 표현시 주의(반올림 처리 유의)
		//_stprintf(szText, _T("%.01f%%"), pcpb->m_dCurPercent);	// 이 경우 소숫점 둘째자리에서 반올림해서 표시됨. 예) 99.99 -> "100.0" 
		
		// 이하 자리는 버림 처리해서 표시하고자 할 경우
		//_stprintf(szText, _T("%.01f%%"), (double)(int)(pcpb->m_dCurPercent * 10) / 10);
		_stprintf(szText, _T("%.01f%%"), floor(10. * pcpb->m_dCurPercent) / 10.);
		
		///////// 차지하는 부분을 그려준다.
		if (pcpb->m_dCurPercent > 0)
		{
			///// 차지하는 영역의 리전을 구한다.
			if (pcpb->m_uDirection == 0) {				
				hRgn = CreateRectRgn(0, 0, pcpb->m_cx * (unsigned int)pcpb->m_dCurPercent / 100, pcpb->m_cy);
			} else if (pcpb->m_uDirection == 1) {
				hRgn = CreateRectRgn(0, pcpb->m_cy - pcpb->m_cy * (unsigned int)pcpb->m_dCurPercent / 100, pcpb->m_cx, pcpb->m_cy);
			}
			SelectClipRgn(hdc, hRgn);
		
			///// 그라데이션 처리
			// 그라데이션의 시작좌표를 명시한다.
			if (pcpb->m_uDirection == 0) {
				vert[0].x = 0;
				vert[0].y = 0;
			} else if (pcpb->m_uDirection == 1) {
				vert[0].x = 0;
				vert[0].y = pcpb->m_cy - pcpb->m_cy * (unsigned int)pcpb->m_dCurPercent / 100;	
			}

			// 그라데이션의 시작색상을 명시한다.
			vert[0].Red    = 0xFF00;
			vert[0].Green  = 0xFF00;
			vert[0].Blue   = 0x0000;
			vert[0].Alpha  = 0x0000;
 
			// 그라데이션의 끝좌표를 명시한다.
			if (pcpb->m_uDirection == 0) {
				vert[1].x = pcpb->m_cx * (unsigned int)pcpb->m_dCurPercent / 100;
				vert[1].y = pcpb->m_cy;
			} else if (pcpb->m_uDirection == 1) {
				vert[1].x = pcpb->m_cx;
				vert[1].y = pcpb->m_cy;
			}
			
			// 그라데이션의 끝색상를 명시한다.
			vert[1].Red    = 0xCC00;
			vert[1].Green  = 0xCC00;
			vert[1].Blue   = 0x0000;
			vert[1].Alpha  = 0x0000;
 
			rcGradient.UpperLeft  = 0;
			rcGradient.LowerRight = 1;

			if (pcpb->m_uDirection == 0) {
				GradientFill(hdc, vert, 2, &rcGradient, 1, GRADIENT_FILL_RECT_V);
			} else if (pcpb->m_uDirection == 1) {
				GradientFill(hdc, vert, 2, &rcGradient, 1, GRADIENT_FILL_RECT_H);
			}

			///// 문자열을 표시한다.
			SetTextColor(hdc, RGB(0, 0, 255));
			DrawText(hdc, szText, -1, &rc, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

			DeleteObject(hRgn);
		}

		///////// 차지하지 않는 부분을 그려준다.
		if (pcpb->m_dCurPercent < 100)
		{
			///// 차지하지 않는 영역의 리전을 구한다.
			if (pcpb->m_uDirection == 0) {				
				hRgn = CreateRectRgn(pcpb->m_cx * (unsigned int)pcpb->m_dCurPercent / 100, 0, pcpb->m_cx, pcpb->m_cy);
			} else if (pcpb->m_uDirection == 1) {
				hRgn = CreateRectRgn(0, 0, pcpb->m_cx, pcpb->m_cy - pcpb->m_cy * (unsigned int)pcpb->m_dCurPercent / 100);
			}
			SelectClipRgn(hdc, hRgn);

			///// 그라데이션 처리
			// 그라데이션의 시작좌표를 명시한다.	
			if (pcpb->m_uDirection == 0) {
				vert[0].x = pcpb->m_cx * (unsigned int)pcpb->m_dCurPercent / 100;
				vert[0].y = 0;
			} else if (pcpb->m_uDirection == 1) {
				vert[0].x = 0;
				vert[0].y = 0;
			}

			// 그라데이션의 시작색상을 명시한다.
			vert[0].Red    = 0x0000;
			vert[0].Green  = 0x0000;
			vert[0].Blue   = 0x0000;
			vert[0].Alpha  = 0x0000;
 
			// 그라데이션의 끝좌표를 명시한다.
			if (pcpb->m_uDirection == 0) {
				vert[1].x = pcpb->m_cx;
				vert[1].y = pcpb->m_cy;
			} else if (pcpb->m_uDirection == 1) {
				vert[1].x = pcpb->m_cx;
				vert[1].y = pcpb->m_cy - pcpb->m_cy * (unsigned int)pcpb->m_dCurPercent / 100;
			}

			// 그라데이션의 끝색상를 명시한다.
			vert[1].Red    = 0x9900;
			vert[1].Green  = 0x9900;
			vert[1].Blue   = 0x9900;
			vert[1].Alpha  = 0x0000;
 
			rcGradient.UpperLeft  = 0;
			rcGradient.LowerRight = 1;

			if (pcpb->m_uDirection == 0) {
				GradientFill(hdc, vert, 2, &rcGradient, 1, GRADIENT_FILL_RECT_V);
			} else if (pcpb->m_uDirection == 1) {
				GradientFill(hdc, vert, 2, &rcGradient, 1, GRADIENT_FILL_RECT_H);
			}

			///// 문자열을 표시한다.
			SetTextColor(hdc, RGB(255, 255, 255));	
			DrawText(hdc, szText, -1, &rc, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

			DeleteObject(hRgn);
		}

		SelectObject(hdc, hOldFont);
		DeleteObject(hFont);
		EndPaint(hWnd, &ps);
		return 0;
	
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}


