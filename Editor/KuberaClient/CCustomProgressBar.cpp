#include <windows.h>
#include <tchar.h>
#include <stdio.h>	// _stprintf �Լ� ���
#include <math.h>	// floor �Լ� ���
#pragma comment(lib, "msimg32") // GradientFill �Լ� ���

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

	///// �θ� �����찡 ���� ��� ������� �������� �����츦 �����Ѵ�.
	if (!m_hParent) {
		m_hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		CloseHandle(CreateThread(NULL, 0, CreateNoParentByThread, this, 0, &tid));
		WaitForSingleObject(m_hEvent, 3000);
		CloseHandle(m_hEvent);
		goto finish;
	}

	///// �ڽ��� ���·� �����ϴ� ���
	wcex.cbSize			= sizeof(WNDCLASSEX);
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)ProgressBarProc;
	wcex.hInstance		= hInstance;
	wcex.hbrBackground	= (HBRUSH)GetStockObject(NULL_BRUSH);
	wcex.lpszClassName	= szClass;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
//	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 4;	// �����캰 CCustomProgressBar ��ü �ּҸ� �����ϱ� ���� 4����Ʈ ���� �Ҵ�
//	wcex.hIcon			= hIcon;
//	wcex.lpszMenuName	= NULL;
	RegisterClassEx(&wcex);

	m_hProgress = CreateWindow(szClass, szTitle, m_bShowAtCreation ? WS_CHILD | WS_VISIBLE : WS_CHILD,
							   m_x, m_y, m_cx, m_cy,
							   m_hParent, (HMENU)NULL, hInstance, this);

finish:
	if (!m_hProgress) {
		MessageBoxW(NULL, L"���α׷����� ������ �����Ͽ����ϴ�.", L"CCustomProgressBar::Create", MB_ICONWARNING | MB_TOPMOST);
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
	wcex.cbWndExtra		= 4;	// �����캰 CCustomProgressBar ��ü �ּҸ� �����ϱ� ���� 4����Ʈ ���� �Ҵ�
//	wcex.hIcon			= hIcon;
//	wcex.lpszMenuName	= NULL;
	RegisterClassEx(&wcex);

	// WS_EX_TOOLWINDOW �÷��׸� ��� �۾�ǥ���ٿ� ��Ÿ���� �ʴ´�.
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
		SetWindowLong(hWnd, 0, long(((LPCREATESTRUCT)lParam)->lpCreateParams)); // CreateWindow���� ������ �μ��� ������ ��ü�� �ּҸ� 4����Ʈ �Ҵ� ������ �Է��� �ش�. 
		return 0;

	/*
	// �������� ������ ��� ���콺 ��Ŭ�� & �巡�׷� �����̰��� �� ��
	case WM_LBUTTONDOWN:
		if (!((CCustomProgressBar *)GetWindowLong(hWnd, 0))->m_hParent) SendMessage(hWnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
		break;
	*/

	case WM_ERASEBKGND: // ���� ������� �׸��� ������ �������� ���ֱ� ���� ����ó��.
		return 0;

	case WM_PAINT:
		pcpb = (CCustomProgressBar *)GetWindowLong(hWnd, 0);	// �ش� ������� �¹��� ��ü�� �ּҸ� ���Ѵ�.
		hdc = BeginPaint(hWnd, &ps);

		// �������� ũ�Ⱑ ���ϴ� ��� ��� �׶��׶� �����ش�.
		GetClientRect(hWnd, &rc);
		pcpb->m_cx = rc.right - rc.left;
		pcpb->m_cy = rc.bottom - rc.top;

		hFont = CreateFont(18, 0, 0, 0, 500, 0, 0, 0, 0, 0, 0, 0, 0, _T("Arial"));
		hOldFont = (HFONT)SelectObject(hdc, hFont);
		SetBkMode(hdc, TRANSPARENT);

		// �Ǽ��� ���ڿ� ǥ���� ����(�ݿø� ó�� ����)
		//_stprintf(szText, _T("%.01f%%"), pcpb->m_dCurPercent);	// �� ��� �Ҽ��� ��°�ڸ����� �ݿø��ؼ� ǥ�õ�. ��) 99.99 -> "100.0" 
		
		// ���� �ڸ��� ���� ó���ؼ� ǥ���ϰ��� �� ���
		//_stprintf(szText, _T("%.01f%%"), (double)(int)(pcpb->m_dCurPercent * 10) / 10);
		_stprintf(szText, _T("%.01f%%"), floor(10. * pcpb->m_dCurPercent) / 10.);
		
		///////// �����ϴ� �κ��� �׷��ش�.
		if (pcpb->m_dCurPercent > 0)
		{
			///// �����ϴ� ������ ������ ���Ѵ�.
			if (pcpb->m_uDirection == 0) {				
				hRgn = CreateRectRgn(0, 0, pcpb->m_cx * (unsigned int)pcpb->m_dCurPercent / 100, pcpb->m_cy);
			} else if (pcpb->m_uDirection == 1) {
				hRgn = CreateRectRgn(0, pcpb->m_cy - pcpb->m_cy * (unsigned int)pcpb->m_dCurPercent / 100, pcpb->m_cx, pcpb->m_cy);
			}
			SelectClipRgn(hdc, hRgn);
		
			///// �׶��̼� ó��
			// �׶��̼��� ������ǥ�� ����Ѵ�.
			if (pcpb->m_uDirection == 0) {
				vert[0].x = 0;
				vert[0].y = 0;
			} else if (pcpb->m_uDirection == 1) {
				vert[0].x = 0;
				vert[0].y = pcpb->m_cy - pcpb->m_cy * (unsigned int)pcpb->m_dCurPercent / 100;	
			}

			// �׶��̼��� ���ۻ����� ����Ѵ�.
			vert[0].Red    = 0xFF00;
			vert[0].Green  = 0xFF00;
			vert[0].Blue   = 0x0000;
			vert[0].Alpha  = 0x0000;
 
			// �׶��̼��� ����ǥ�� ����Ѵ�.
			if (pcpb->m_uDirection == 0) {
				vert[1].x = pcpb->m_cx * (unsigned int)pcpb->m_dCurPercent / 100;
				vert[1].y = pcpb->m_cy;
			} else if (pcpb->m_uDirection == 1) {
				vert[1].x = pcpb->m_cx;
				vert[1].y = pcpb->m_cy;
			}
			
			// �׶��̼��� ������ ����Ѵ�.
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

			///// ���ڿ��� ǥ���Ѵ�.
			SetTextColor(hdc, RGB(0, 0, 255));
			DrawText(hdc, szText, -1, &rc, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

			DeleteObject(hRgn);
		}

		///////// �������� �ʴ� �κ��� �׷��ش�.
		if (pcpb->m_dCurPercent < 100)
		{
			///// �������� �ʴ� ������ ������ ���Ѵ�.
			if (pcpb->m_uDirection == 0) {				
				hRgn = CreateRectRgn(pcpb->m_cx * (unsigned int)pcpb->m_dCurPercent / 100, 0, pcpb->m_cx, pcpb->m_cy);
			} else if (pcpb->m_uDirection == 1) {
				hRgn = CreateRectRgn(0, 0, pcpb->m_cx, pcpb->m_cy - pcpb->m_cy * (unsigned int)pcpb->m_dCurPercent / 100);
			}
			SelectClipRgn(hdc, hRgn);

			///// �׶��̼� ó��
			// �׶��̼��� ������ǥ�� ����Ѵ�.	
			if (pcpb->m_uDirection == 0) {
				vert[0].x = pcpb->m_cx * (unsigned int)pcpb->m_dCurPercent / 100;
				vert[0].y = 0;
			} else if (pcpb->m_uDirection == 1) {
				vert[0].x = 0;
				vert[0].y = 0;
			}

			// �׶��̼��� ���ۻ����� ����Ѵ�.
			vert[0].Red    = 0x0000;
			vert[0].Green  = 0x0000;
			vert[0].Blue   = 0x0000;
			vert[0].Alpha  = 0x0000;
 
			// �׶��̼��� ����ǥ�� ����Ѵ�.
			if (pcpb->m_uDirection == 0) {
				vert[1].x = pcpb->m_cx;
				vert[1].y = pcpb->m_cy;
			} else if (pcpb->m_uDirection == 1) {
				vert[1].x = pcpb->m_cx;
				vert[1].y = pcpb->m_cy - pcpb->m_cy * (unsigned int)pcpb->m_dCurPercent / 100;
			}

			// �׶��̼��� ������ ����Ѵ�.
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

			///// ���ڿ��� ǥ���Ѵ�.
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


