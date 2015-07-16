
#ifndef __CCUSTOMPROGRESSBAR_H_TIMELESS
#define __CCUSTOMPROGRESSBAR_H_TIMELESS


#include <windows.h>
#include <tchar.h>

class CCustomProgressBar
{
public:
	HWND m_hParent;				// �θ� �������� �ڵ�, NULL�� ��� �������� ���·� ����
	HWND m_hProgress;			// ������ ���α׷����� �ڵ�
	double m_dCurPercent;		// ���� �ۼ�Ʈ
	unsigned int m_uDirection;	// ���α׷����� ������� / 0: ����, 1: ����
	BOOL m_bShowAtCreation;		// ������ ���� ����
	int m_x;					// x��ǥ
	int m_y;					// y��ǥ
	int m_cx;					// �ʺ�
	int m_cy;					// ��
	HANDLE m_hEvent;			// �������� ���·� ������ �����忡�� ���α׷����� ���� �� �� ������ �˸��� �� ��� 

	CCustomProgressBar();
	CCustomProgressBar(HWND hParent, unsigned uDirection, BOOL bShowAtCreation, int x, int y, int cx, int cy);
	~CCustomProgressBar();

	HWND Create(HWND hParent, unsigned uDirection, BOOL bShowAtCreation, int x, int y, int cx, int cy);
	void SetPercent(double dCurPercent);
	void SetDirection(unsigned int uDirection);
	BOOL Show(int nCmdShow);	// ShowWindow�� ���� �μ� ���

	static DWORD WINAPI CreateNoParentByThread(LPVOID lp);	// Create���� �������� ���·� ������ ���������� ���
	static LRESULT CALLBACK ProgressBarProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};


#endif