
#ifndef __CCUSTOMPROGRESSBAR_H_TIMELESS
#define __CCUSTOMPROGRESSBAR_H_TIMELESS


#include <windows.h>
#include <tchar.h>

class CCustomProgressBar
{
public:
	HWND m_hParent;				// 부모 윈도우의 핸들, NULL인 경우 독립적인 형태로 생성
	HWND m_hProgress;			// 생성된 프로그레스바 핸들
	double m_dCurPercent;		// 현재 퍼센트
	unsigned int m_uDirection;	// 프로그레스바 진행방향 / 0: 수평, 1: 수직
	BOOL m_bShowAtCreation;		// 생성시 보임 유무
	int m_x;					// x좌표
	int m_y;					// y좌표
	int m_cx;					// 너비
	int m_cy;					// 폭
	HANDLE m_hEvent;			// 독립적인 형태로 생성시 스레드에서 프로그레스바 생성 후 그 시점을 알리는 데 사용 

	CCustomProgressBar();
	CCustomProgressBar(HWND hParent, unsigned uDirection, BOOL bShowAtCreation, int x, int y, int cx, int cy);
	~CCustomProgressBar();

	HWND Create(HWND hParent, unsigned uDirection, BOOL bShowAtCreation, int x, int y, int cx, int cy);
	void SetPercent(double dCurPercent);
	void SetDirection(unsigned int uDirection);
	BOOL Show(int nCmdShow);	// ShowWindow와 같은 인수 사용

	static DWORD WINAPI CreateNoParentByThread(LPVOID lp);	// Create에서 독립적인 형태로 생성시 내부적으로 사용
	static LRESULT CALLBACK ProgressBarProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};


#endif