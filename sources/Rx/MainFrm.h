/*
	Rx Cervicales - Projet de Recherche sur la Radiologie Cervicales Fonctionnelle
	Travail de Licence, © Univerité de Genève, 1999, Tous droits reservés
	------------------------------------------------------------------------------
	écrit par Daniel Doubrovkine (doubrov5@cuimail.unige.ch / dblock@vestris.com)
	http://cuiwww.unige.ch/~doubrov5/uni/license
	------------------------------------------------------------------------------
	Ce code est du domaine public. Il est distribué sans aucune garantie.
	*/

#if !defined(AFX_MAINFRM_H__F481A567_ABBA_11D2_9BFD_000001352962__INCLUDED_)
#define AFX_MAINFRM_H__F481A567_ABBA_11D2_9BFD_000001352962__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

extern CRxApp theApp;
typedef enum {frmActivate=1, frmDestroy=2, frmInvalidate=4, frmCreate=8} frmActions;

class CMainFrame : public CMDIFrameWnd {
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();
	
// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void ActivateFrame(int nCmdShow = -1);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif	
protected:  // control bar embedded members
	CStatusBar	m_wndStatusBar;
	CToolBar    m_wndDToolBar;	
	CToolBar    m_wndToolBar;		
// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:	
	CString m_sHeading, m_sRect, m_sIcon;
    CString m_sMax, m_sTool, m_sStatus;
    BOOL    m_bFirstTime;
public:
	void SetStatus(CString Msg);
	CView * FindView(CDocument * pDoc, CDocTemplate* pTemplate, CRuntimeClass *pViewClass);
	//void ProcessView(CDocTemplate* pTemplate, CRuntimeClass *pViewClass, int i_Flags);
	CView * ProcessView(CDocument * pDoc, CDocTemplate* pTemplate, CRuntimeClass *pViewClass, int i_Flags);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__F481A567_ABBA_11D2_9BFD_000001352962__INCLUDED_)
