/*
	Rx Cervicales - Projet de Recherche sur la Radiologie Cervicales Fonctionnelle
	Travail de Licence, © Univerité de Genève, 1999, Tous droits reservés
	------------------------------------------------------------------------------
	écrit par Daniel Doubrovkine (doubrov5@cuimail.unige.ch / dblock@vestris.com)
	http://cuiwww.unige.ch/~doubrov5/uni/license
	------------------------------------------------------------------------------
	Ce code est du domaine public. Il est distribué sans aucune garantie.
	*/

#include "stdafx.h"
#include "Rx.h"

#include "MainFrm.h"
#include "DibInformation.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	// Global help commands
	ON_COMMAND(ID_HELP_FINDER, CMDIFrameWnd::OnHelpFinder)
	ON_COMMAND(ID_HELP, CMDIFrameWnd::OnHelp)
	ON_COMMAND(ID_CONTEXT_HELP, CMDIFrameWnd::OnContextHelp)
	ON_COMMAND(ID_DEFAULT_HELP, CMDIFrameWnd::OnHelpFinder)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

CMainFrame::CMainFrame() : m_sHeading("Window Size"), m_sRect("Rect"), 
					       m_sIcon("Iconized"), m_sMax("Maximized") {
}

CMainFrame::~CMainFrame() {
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) {			
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1) {
		theApp.ShowError();
		return -1;
	}
	SetIcon(theApp.LoadIcon(IDR_MAINFRAME), FALSE);
	if (!m_wndStatusBar.Create(this) || !m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT))) {
		theApp.ShowError();
		return -1;
	}
	
	EnableDocking(CBRS_ALIGN_ANY);	
	if (!m_wndToolBar.Create(this) || !m_wndToolBar.LoadToolBar(IDR_MAINFRAME)) {
		theApp.ShowError();
		return -1;      
	}	
	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);				
	DockControlBar(&m_wndToolBar);		
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs) {
	if( !CMDIFrameWnd::PreCreateWindow(cs))
		return FALSE;
	return TRUE;
}

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG

void CMainFrame::ActivateFrame(int nCmdShow) {		
	try {
		CRect rect;
		if (!m_bFirstTime) {
			CMDIFrameWnd::ActivateFrame(nCmdShow);
			return;
		}
		LoadBarState(theApp.m_pszProfileName);
		m_bFirstTime = FALSE;
		CString text = theApp.GetProfileString(m_sHeading, m_sRect);
			if (!text.IsEmpty() && (text.GetLength() >= 15)) {
			rect.left = atoi((const char*) text);
			rect.top = atoi((const char*) text + 5);
			rect.right = atoi((const char*) text + 10);
			rect.bottom = atoi((const char*) text + 15);
		} else {
			CMDIFrameWnd::ActivateFrame(nCmdShow);
			return;
		}    
		BOOL bIconic = theApp.GetProfileInt(m_sHeading, m_sIcon, 0);
		BOOL bMaximized = theApp.GetProfileInt(m_sHeading, m_sMax, 0);    
		UINT flags;
		if (bIconic) {
			nCmdShow = SW_SHOWMINNOACTIVE;
			if (bMaximized) 
				flags = WPF_RESTORETOMAXIMIZED;
			} else {
				if (bMaximized) {
					nCmdShow = SW_SHOWMAXIMIZED;
					flags = WPF_RESTORETOMAXIMIZED;
				} else {
				nCmdShow = SW_NORMAL;
				flags = 0;
			}
		}
		WINDOWPLACEMENT wndpl;
		wndpl.length = sizeof(WINDOWPLACEMENT);
		wndpl.showCmd = nCmdShow;
		wndpl.flags = flags;
		wndpl.ptMinPosition = CPoint(0, 0);
		wndpl.ptMaxPosition = CPoint(-::GetSystemMetrics(SM_CXBORDER), -::GetSystemMetrics(SM_CYBORDER));
		wndpl.rcNormalPosition = rect;	
		SetWindowPlacement(&wndpl);		
		CMDIFrameWnd::ActivateFrame(nCmdShow);			
	} catch (...) { 
		CMDIFrameWnd::ActivateFrame(nCmdShow);
	}
}

void CMainFrame::OnClose() {
	SaveBarState(theApp.m_pszProfileName);	
	CMDIFrameWnd::OnClose();
}

void CMainFrame::OnDestroy() {
	try {
		CString text, temp;
		BOOL bIconic, bMaximized;
		WINDOWPLACEMENT wndpl;
		wndpl.length = sizeof(WINDOWPLACEMENT);
		// gets current window position and iconized/maximized status
		BOOL bRet = GetWindowPlacement(&wndpl);
		if (wndpl.showCmd == SW_SHOWNORMAL) {
			bIconic = FALSE;
			bMaximized = FALSE;
		} else if (wndpl.showCmd == SW_SHOWMAXIMIZED) {
			bIconic = FALSE;
			bMaximized = TRUE;
		} else if (wndpl.showCmd == SW_SHOWMINIMIZED) {
			bIconic = TRUE;
			if (wndpl.flags) bMaximized = TRUE;
			else bMaximized = FALSE;
		}
		wsprintf(text.GetBuffer(20), "%04d %04d %04d %04d",
			wndpl.rcNormalPosition.left, wndpl.rcNormalPosition.top,
			wndpl.rcNormalPosition.right, wndpl.rcNormalPosition.bottom);
		text.ReleaseBuffer();
		theApp.WriteProfileString(m_sHeading, m_sRect, text);
		theApp.WriteProfileInt(m_sHeading, m_sIcon, bIconic);
		theApp.WriteProfileInt(m_sHeading, m_sMax, bMaximized);	
		CMDIFrameWnd::OnDestroy();
	} catch (...) { 
		CMDIFrameWnd::OnDestroy();
	}
	
}

CView * CMainFrame::ProcessView(CDocument * pDoc, CDocTemplate* pTemplate, CRuntimeClass *pViewClass, int i_Flags){
	if (!pDoc) return NULL;
	CView* pView;
	POSITION pos = pDoc->GetFirstViewPosition();
	while (pos != NULL)	{
		pView = pDoc->GetNextView(pos);
		if (pView->IsKindOf(pViewClass)) {
			if (i_Flags & frmActivate) pView->GetParentFrame()->ActivateFrame();
			else if (i_Flags & frmDestroy) pView->GetParentFrame()->DestroyWindow();			
			else if (i_Flags & frmInvalidate) pView->PostMessage(WM_PAINT);
			return pView;
		}
	}
	
	if (i_Flags & frmCreate) {
		CMDIChildWnd* pNewFrame = (CMDIChildWnd*) pTemplate->CreateNewFrame(pDoc, NULL);
		if (pNewFrame) pTemplate->InitialUpdateFrame(pNewFrame, pDoc);
		return (CView *) pNewFrame;
	} else return NULL;
}

CView * CMainFrame::FindView(CDocument * pDoc, CDocTemplate* pTemplate, CRuntimeClass *pViewClass){
	if (!pDoc) return NULL;
	CView* pView;
	POSITION pos = pDoc->GetFirstViewPosition();
	while (pos != NULL)
	{
		pView = pDoc->GetNextView(pos);
		if (pView->IsKindOf(pViewClass))
		{
			return pView;
		}
	}	
	return NULL;
}

void CMainFrame::SetStatus(CString Msg){
	m_wndStatusBar.SetPaneText(0, Msg);
}