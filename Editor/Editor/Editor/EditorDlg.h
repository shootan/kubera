
// EditorDlg.h : 헤더 파일
//

#pragma once
#include "ZDIB.h"
#include "CBufferDC.h"
#include <list>

using namespace std;
/*ULONG_PTR token;*/

struct Information
{
	int team;
	float x, z;
};

// CEditorDlg 대화 상자
class CEditorDlg : public CDialogEx
{
// 생성입니다.
public:
	CEditorDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
	enum { IDD = IDD_EDITOR_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.

	

// 구현입니다.
protected:
	//
	int m_nCurHeight;
	int m_nScrollPos;
	CRect m_rect;
	//
	list<Information> Rock;
	list<Information> Rock2;
	list<Information> Tree;
	list<Information> Tower;
	list<Information> Nexus;

	int LastBuild;

	// 
	HICON m_hIcon;
	CString m_str;
	CPoint m_pos;
	bool m_Check;
	CDC m_display_memory;
	
	BOOL m_bClick;

	char SaveIndex[2];

	int m_BuildType;
	int m_TeamType;
	bool teee;

	HPEN pen2;
	int ScreenX, ScreenY;
	CPoint GamePos;
	//
	ZDIB m_image, m_image2, m_image3, m_image4, m_RockImage,m_RockImage2, m_TowerImage, m_TreeImage, m_NexusImage;
	CRect m_Rect1, m_Rect2, m_Rect3, m_Rect4;
	HPEN pen;
	int lineX, lineY;
	// 생성된 메시지 맵 함수
	
public:
	
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	afx_msg void OnBnClickedOk();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMenuExit();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	
	afx_msg void OnNexus();
	afx_msg void OnRed();
	afx_msg void OnRock();
	afx_msg void OnBlue();
	afx_msg void OnTree();
	afx_msg void OnTower();
	afx_msg void OnUpdateRed(CCmdUI *pCmdUI);
	afx_msg void OnUpdateBlue(CCmdUI *pCmdUI);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg LRESULT OnKickIdle(WPARAM wParam, LPARAM lParam);
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

	CRect SetRectScrollPos(CRect _rect);
	CPoint SetPosScrollPos(CPoint _pos);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSave();
	afx_msg void OnLoad();
	afx_msg void OnRock2();
	afx_msg void OnReset();
};
