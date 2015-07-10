
// EditorDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "Editor.h"
#include "EditorDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)

END_MESSAGE_MAP()


// CEditorDlg 대화 상자




CEditorDlg::CEditorDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CEditorDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_Check = false;
}

void CEditorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CEditorDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CEditorDlg::OnBnClickedOk)
	ON_WM_MOUSEMOVE()
	ON_COMMAND(ID_MENU_EXIT, &CEditorDlg::OnMenuExit)
	ON_COMMAND(ID_NEXUS, &CEditorDlg::OnNexus)
	ON_WM_LBUTTONUP()
	ON_COMMAND(ID_REd, &CEditorDlg::OnRed)
	ON_COMMAND(ID_ROCK, &CEditorDlg::OnRock)
	ON_COMMAND(ID_BLUE, &CEditorDlg::OnBlue)
	ON_COMMAND(ID_TREE, &CEditorDlg::OnTree)
	ON_COMMAND(ID_TOWER, &CEditorDlg::OnTower)
	ON_UPDATE_COMMAND_UI(ID_REd, &CEditorDlg::OnUpdateRed)
	ON_UPDATE_COMMAND_UI(ID_BLUE, &CEditorDlg::OnUpdateBlue)
	ON_WM_RBUTTONUP()
	ON_MESSAGE( WM_KICKIDLE  , OnKickIdle)
	ON_WM_SIZE()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_TIMER()
	ON_COMMAND(ID_SAVE, &CEditorDlg::OnSave)
	ON_COMMAND(ID_LOAD, &CEditorDlg::OnLoad)
	ON_COMMAND(ID_ROCK2, &CEditorDlg::OnRock2)
	ON_COMMAND(ID_RESET, &CEditorDlg::OnReset)
END_MESSAGE_MAP()


// CEditorDlg 메시지 처리기

BOOL CEditorDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
// 	GetDlgItem(IDC_PICTURE)->GetWindowRect(m_Rect1);
// 	ScreenToClient(m_Rect1);

	GetWindowRect(m_rect);
	m_nScrollPos = 0;
	m_nCurHeight = 0;

	//SetScrollRange(0, 1000);

	m_bClick = FALSE;
	teee = false;

	m_Rect1.left = 10;
	m_Rect1.right = 1210;
	m_Rect1.top = 10;
	m_Rect1.bottom = 810;

	m_Rect2.left = 0;
	m_Rect2.right = 1600;
	m_Rect2.top = 0;
	m_Rect2.bottom = 1200;

	m_Rect3.left = 7;
	m_Rect3.right = 1213;
	m_Rect3.top = 7;
	m_Rect3.bottom = 813;

	LastBuild = 0;
	
	m_image.LoadDibPath("res/13.bmp");
	m_image2.LoadDibPath("res/2.bmp");
	m_image3.LoadDibPath("res/3.bmp");
	m_image4.LoadDibPath("res/4.bmp");

	ZeroMemory(&GamePos, sizeof(CPoint));
	ScreenX = 0;
	ScreenY = 0;
	pen = CreatePen(PS_SOLID, 1, RGB(255, 255, 50));
	pen2 = CreatePen(PS_SOLID, 1, RGB(0, 255, 0));
	m_BuildType = 0;
	m_TeamType = 1;

	m_RockImage.LoadDibPath("res/rock.bmp");
	m_TowerImage.LoadDibPath("res/tower.bmp");
	m_TreeImage.LoadDibPath("res/tree.bmp");
	m_NexusImage.LoadDibPath("res/Nexus.bmp");

	SetTimer(1, 50, NULL);

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CEditorDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CEditorDlg::OnPaint()
{
	list<Information>::iterator iterList;
	CRect re;
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CBufferDC dc(this);

		//바닥 셋팅
		m_image2.DrawDIB(&dc, SetRectScrollPos(m_Rect2));
		m_image3.DrawDIB(&dc, SetRectScrollPos(m_Rect3));
		m_image.DrawDIB(&dc, SetRectScrollPos(m_Rect1));

		// 좌표
		dc.SetBkMode(TRANSPARENT);
		SetTextColor(dc, RGB(255, 255, 50));
		dc.TextOut(m_pos.x, m_pos.y, m_str);

		// 그리기
		for(iterList = Rock.begin(); iterList!= Rock.end(); iterList++)
		{
			re.left = (int)iterList->x - 12 + 610;
			re.right =  (int)iterList->x + 12  + 610;
			re.top = 410  - (int)iterList->z - 12;
			re.bottom = 410  - (int)iterList->z + 12;
			m_RockImage.DrawDIB(&dc, SetRectScrollPos(re));
		}

		for(iterList = Rock2.begin(); iterList!= Rock2.end(); iterList++)
		{
			re.left = (int)iterList->x - 12 + 610;
			re.right =  (int)iterList->x + 12  + 610;
			re.top = 410  - (int)iterList->z - 12;
			re.bottom = 410  - (int)iterList->z + 12;
			m_RockImage.DrawDIB(&dc, SetRectScrollPos(re));
		}
		for(iterList = Tree.begin(); iterList!= Tree.end(); iterList++)
		{
			re.left = (int)iterList->x - 12 + 610;
			re.right =  (int)iterList->x + 12  + 610;
			re.top = 410  - (int)iterList->z - 12;
			re.bottom = 410  - (int)iterList->z + 12;
			m_TreeImage.DrawDIB(&dc, SetRectScrollPos(re));
		}
		for(iterList = Tower.begin(); iterList!= Tower.end(); iterList++)
		{
			re.left = (int)iterList->x - 12 + 610;
			re.right =  (int)iterList->x + 12  + 610;
			re.top = 410  - (int)iterList->z - 12;
			re.bottom = 410  - (int)iterList->z + 12;
			m_TowerImage.DrawDIB(&dc, SetRectScrollPos(re));
		}
		for(iterList = Nexus.begin(); iterList!= Nexus.end(); iterList++)
		{
			re.left = (int)iterList->x - 12 + 610;
			re.right =  (int)iterList->x + 12  + 610;
			re.top = 410  - (int)iterList->z - 12;
			re.bottom = 410  - (int)iterList->z + 12;
			m_NexusImage.DrawDIB(&dc, SetRectScrollPos(re));
		}
		if(m_bClick)
		{
			m_image4.DrawDIB(&dc, m_Rect4);
		}

		// 세팅
		
		SelectObject(dc, pen);
		dc.MoveTo(0, lineY);
		dc.LineTo(800,lineY);

		dc.MoveTo(lineX, 0);
		dc.LineTo(lineX,800);

		SelectObject(dc, pen2);
		dc.MoveTo(-m_nScrollPos, 410-m_nCurHeight);
		dc.LineTo(1300-m_nScrollPos, 410-m_nCurHeight);

		dc.MoveTo(610- m_nScrollPos, -m_nCurHeight);
		dc.LineTo(610- m_nScrollPos, 810-m_nCurHeight);

		dc.FillSolidRect(0, 0, 6, 800, RGB(255,255,255));
		dc.FillSolidRect(0, 0, 800, 6, RGB(255,255,255));
		dc.FillSolidRect(713, 0, 800, 800, RGB(255,255,255));
		dc.FillSolidRect(0, 443, 800, 800, RGB(255,255,255));
		
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CEditorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CEditorDlg::OnBnClickedOk()
{
	CDialogEx::OnOK();
}

void CEditorDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	CPoint t = point;
	lineX = point.x;
	lineY = point.y;

	CPoint p = point;
	//MoveScreen(point);

	//point.y += m_nScrollPos;
	p.x = point.x - 610 ;
	p.y = 410 - point.y ;
	m_str.Format(_T("[X] : %d, [Z] : %d"), SetPosScrollPos(p).x, SetPosScrollPos(p).y);
	m_Rect4.left = t.x - 12;
	m_Rect4.right = t.x + 12;
	m_Rect4.top = t.y - 12;
	m_Rect4.bottom = t.y + 12;
	t.y += 13;
	t.x -= 35;

	m_pos = t;

	InvalidateRect(NULL, FALSE);

	CDialogEx::OnMouseMove(nFlags, point);
}

void CEditorDlg::OnMenuExit()
{
	CDialogEx::OnCancel();
}

void CEditorDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	if(!m_bClick) return;
	CPoint p;
	p.x = point.x - 610 + m_nScrollPos;
	p.y = 410 - point.y - m_nCurHeight;
	
	Information i;
	switch(m_BuildType)
	{
	case 1:
		i.team = m_TeamType;
		i.x = (float)p.x;
		i.z = (float)p.y;
		Rock.push_back(i);
		LastBuild = m_BuildType;
		break;
	case 2:
		i.team = m_TeamType;
		i.x = (float)p.x;
		i.z = (float)p.y;
		Rock2.push_back(i);
		LastBuild = m_BuildType;
		break;
	case 3:
		i.team = m_TeamType;
		i.x = (float)p.x;
		i.z = (float)p.y;
		Tree.push_back(i);
		LastBuild = m_BuildType;
		break;
	case 4:
		i.team = m_TeamType;
		i.x = (float)p.x;
		i.z = (float)p.y;
		Tower.push_back(i);
		LastBuild = m_BuildType;
		break;
	case 5:
		i.team = m_TeamType;
		i.x = (float)p.x;
		i.z = (float)p.y;
		Nexus.push_back(i);
		LastBuild = m_BuildType;
		break;
	}
	InvalidateRect(NULL, FALSE);
	CDialogEx::OnLButtonUp(nFlags, point);
}

void CEditorDlg::OnBlue()
{
	m_TeamType = 2;
}

void CEditorDlg::OnRed()
{
	m_TeamType = 1;
}

void CEditorDlg::OnRock()
{
	m_BuildType = 1;
	m_image4.LoadDibPath("res/Rock.bmp");

	m_bClick = TRUE;
}

void CEditorDlg::OnRock2()
{
	m_BuildType = 2;
	m_image4.LoadDibPath("res/Rock.bmp");
	m_bClick = TRUE;
}


void CEditorDlg::OnTree()
{
	m_BuildType = 3;
	m_image4.LoadDibPath("res/tree.bmp");
	m_bClick = TRUE;
}
void CEditorDlg::OnTower()
{
	m_BuildType = 4;
	m_image4.LoadDibPath("res/tower.bmp");
	m_bClick = TRUE;
}

void CEditorDlg::OnNexus()
{
	m_BuildType = 5;
	m_image4.LoadDibPath("res/Nexus.bmp");
	m_bClick = TRUE;
}

void CEditorDlg::OnUpdateRed(CCmdUI *pCmdUI)
{
	pen = CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
}


void CEditorDlg::OnUpdateBlue(CCmdUI *pCmdUI)
{
	pen = CreatePen(PS_SOLID, 1, RGB(0, 0, 255));
}

void CEditorDlg::OnRButtonUp(UINT nFlags, CPoint point)
{
	m_bClick = FALSE;
	
	CDialogEx::OnRButtonUp(nFlags, point);
}


BOOL CEditorDlg::PreTranslateMessage(MSG* pMsg)
{
	if ((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_ESCAPE)) 
	{
		return TRUE;
	}
	if ((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_RETURN)) 
	{
		return TRUE;
	}
	if ((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_DELETE)) 
	{
		switch(LastBuild)
		{
		case 1:
			Rock.pop_back();
			break;
		case 2:
			Rock2.pop_back();
			break;
		case 3:
			Tree.pop_back();
			break;
		case 4:
			Tower.pop_back();
			break;
		case 5:
			Nexus.pop_back();
			break;
		}
		InvalidateRect(FALSE, NULL);
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}

LRESULT CEditorDlg::OnKickIdle(WPARAM, LPARAM lCount)
{
	
	UpdateDialogControls(this, FALSE);
	return (LRESULT)1;
}

void CEditorDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	SCROLLINFO si;
	GetScrollInfo(SB_VERT, &si, NULL);
   	
   	si.cbSize = sizeof(SCROLLINFO);
   	si.fMask = SIF_ALL; // SIF_ALL = SIF_PAGE | SIF_RANGE | SIF_POS;
   	si.nMin = 0;
   	si.nMax = 410;
   	si.nPage = si.nMax/10;
   	si.nPos = 0;
   	SetScrollInfo(SB_VERT, &si, TRUE); 

	SCROLLINFO si2;
	si2.cbSize = sizeof(SCROLLINFO);
	si2.fMask = SIF_ALL; // SIF_ALL = SIF_PAGE | SIF_RANGE | SIF_POS;
	si2.nMin = 0;
	si2.nMax = 550;
	si2.nPage = si.nMax/10;
	si2.nPos = 0;
	SetScrollInfo(SB_HORZ, &si2, TRUE); 

}


void CEditorDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	int nDelta;
	int nMaxPos = m_rect.Height() - m_nCurHeight;

	switch (nSBCode)
	{
	case SB_LINEDOWN:
		if (nMaxPos <= 160)
			return;
		
		nDelta = 5;
		break;

	case SB_LINEUP:
		if (m_nCurHeight <= 0)
			return;
		nDelta = -5;
		break;

	case SB_PAGEDOWN:
		if (m_nCurHeight >= nMaxPos)
			return;
		nDelta = min(nMaxPos/10,nMaxPos-m_nCurHeight);
		break;

	case SB_THUMBPOSITION:
		nDelta = (int)nPos - m_nCurHeight;
		break;

	case SB_PAGEUP:
		if (m_nScrollPos <= 0)
			return;
		nDelta = -min(nMaxPos/10,m_nCurHeight);
		break;

	default:
		return;
	}
	m_nCurHeight += nDelta;
	SetScrollPos(SB_VERT,m_nCurHeight,TRUE);
	ScrollWindow(0,-nDelta);

	InvalidateRect(NULL, TRUE);

	CDialogEx::OnVScroll(nSBCode, nPos, pScrollBar);
}


void CEditorDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	int nDelta;
	int nMaxPos = m_rect.Width() - m_nScrollPos;

	switch (nSBCode)
	{
	case SB_LINEDOWN:
		if (nMaxPos <= 230)
			return;
		nDelta = 7;
		break;
	case SB_LINEUP:
		if (m_nScrollPos <= 0)
			return;
		nDelta = -7;
		break;

	case SB_PAGEDOWN:
		if (m_nScrollPos >= nMaxPos)
			return;
		nDelta = min(nMaxPos/10,nMaxPos-m_nScrollPos);
		break;

	case SB_THUMBPOSITION:
		nDelta = (int)nPos - m_nScrollPos;
		break;

	case SB_PAGEUP:
		if (m_nScrollPos <= 0)
			return;
		nDelta = -min(nMaxPos/10,m_nScrollPos);
		break;

	default:
		return;
	}
	m_nScrollPos += nDelta;
	SetScrollPos(SB_HORZ,m_nScrollPos,TRUE);
	ScrollWindow(-nDelta,0);

	InvalidateRect(NULL, TRUE);

	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}


CRect CEditorDlg::SetRectScrollPos(CRect _rect)
{
	_rect.left -= m_nScrollPos;
	_rect.right -= m_nScrollPos;
	_rect.top -= m_nCurHeight;
	_rect.bottom -= m_nCurHeight;

	return _rect;
}

CPoint CEditorDlg::SetPosScrollPos(CPoint _pos)
{
	_pos.x += m_nScrollPos;
	_pos.y -= m_nCurHeight;

	return _pos;
}

void CEditorDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	
	//OnVScroll(1, 0, NULL);
	CDialogEx::OnTimer(nIDEvent);
}


void CEditorDlg::OnSave()
{
	list<Information>::iterator iterList;
	//char Filter[] = "TXT File(*.txt) |*.txt| 모든파일(*.*) |*.*|";
	CFileDialog dlg(FALSE,_T("save"),NULL,OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST,_T(" |*.txt;|All Files(*.*)|*.*||"),this);

	CString str;
	if( dlg.DoModal() == IDOK )
	{
		FILE *fp;
		fopen_s(&fp, (CStringA)dlg.GetPathName(), "w");
		fprintf(fp, "%d\n", Rock.size());//저장 하기전 저장할 갯수를 저장

		for(iterList = Rock.begin(); iterList!= Rock.end(); iterList++)
		{
			fprintf(fp, "%d %f %f\n", iterList->team, iterList->x, iterList->z);
			
		}

		fprintf(fp, "%d\n", Rock2.size());//저장 하기전 저장할 갯수를 저장

		for(iterList = Rock2.begin(); iterList!= Rock2.end(); iterList++)
		{
			fprintf(fp, "%d %f %f\n", iterList->team, iterList->x, iterList->z);

		}

		fprintf(fp, "%d\n", Tree.size());//저장 하기전 저장할 갯수를 저장

		for(iterList = Tree.begin(); iterList!= Tree.end(); iterList++)
		{
			fprintf(fp, "%d %f %f\n", iterList->team, iterList->x, iterList->z);

		}

		fprintf(fp, "%d\n", Tower.size());//저장 하기전 저장할 갯수를 저장

		for(iterList = Tower.begin(); iterList!= Tower.end(); iterList++)
		{
			fprintf(fp, "%d %f %f\n", iterList->team, iterList->x, iterList->z);

		}

		fprintf(fp, "%d\n", Nexus.size());//저장 하기전 저장할 갯수를 저장

		for(iterList = Nexus.begin(); iterList!= Nexus.end(); iterList++)
		{
			fprintf(fp, "%d %f %f\n", iterList->team, iterList->x, iterList->z);

		}

		fclose(fp);
	}
	else
	{

	}
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}


void CEditorDlg::OnLoad()
{
	list<Information>::iterator iterList;
	//char Filter[] = "TXT File(*.txt) |*.txt| 모든파일(*.*) |*.*|";
	CFileDialog dlg(TRUE,_T("Load"),NULL,OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST,_T(" |*.txt;|All Files(*.*)|*.*||"),this);

	CString str;
	int size=0;
	if( dlg.DoModal() == IDOK )
	{
		FILE *fp;
		fopen_s(&fp, (CStringA)dlg.GetPathName(), "r");

		fscanf_s(fp, "%d", &size);
		
		for(int i=0; i<size; i++)
		{
			Information Info;
			fscanf_s(fp, "%d", &Info.team);
			fscanf_s(fp, "%f", &Info.x);
			fscanf_s(fp, "%f", &Info.z);
			Rock.push_back(Info);
		}

		fscanf_s(fp, "%d", &size);

		for(int i=0; i<size; i++)
		{
			Information Info;
			fscanf_s(fp, "%d", &Info.team);
			fscanf_s(fp, "%f", &Info.x);
			fscanf_s(fp, "%f", &Info.z);
			Rock2.push_back(Info);
		}

		fscanf_s(fp, "%d", &size);

		for(int i=0; i<size; i++)
		{
			Information Info;
			fscanf_s(fp, "%d", &Info.team);
			fscanf_s(fp, "%f", &Info.x);
			fscanf_s(fp, "%f", &Info.z);
			Tree.push_back(Info);
		}

		fscanf_s(fp, "%d", &size);

		for(int i=0; i<size; i++)
		{
			Information Info;
			fscanf_s(fp, "%d", &Info.team);
			fscanf_s(fp, "%f", &Info.x);
			fscanf_s(fp, "%f", &Info.z);
			Tower.push_back(Info);
		}

		fscanf_s(fp, "%d", &size);

		for(int i=0; i<size; i++)
		{
			Information Info;
			fscanf_s(fp, "%d", &Info.team);
			fscanf_s(fp, "%f", &Info.x);
			fscanf_s(fp, "%f", &Info.z);
			Nexus.push_back(Info);
		}
		
		fclose(fp);
	}
	else
	{

	}
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}




void CEditorDlg::OnReset()
{
	Rock.clear();
	Rock2.clear();
	Tree.clear();
	Tower.clear();
	Nexus.clear();
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}
