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
#include "ATraceReport.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "DibTracker.h"


CATraceReport::CATraceReport(CWnd* pParent /*=NULL*/) : CDialog(CATraceReport::IDD, pParent) {
	m_Angles = NULL;
	m_FlexDistances = NULL;
	m_ExtDistances = NULL;	
	m_sizeTotal = CSize(826, 1168);	
	//{{AFX_DATA_INIT(CATraceReport)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CATraceReport::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CATraceReport)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CATraceReport, CDialog)
	//{{AFX_MSG_MAP(CATraceReport)
	ON_BN_CLICKED(IDC_ATR_COPY, OnAtrCopy)
	ON_BN_CLICKED(IDC_ATPRINT, OnAtprint)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CATraceReport::OnAtrCopy() {
	CTreeCtrl * l_Tree = (CTreeCtrl *) GetDlgItem(IDC_TRACE_TREE);
	ASSERT(l_Tree);
	
	CString l_AngleVector, l_Angle;
	for (int i=0;i<m_VectorSize;i++) {
		if (i==m_VectorSize-1) l_Angle.Format("%.02f\n", m_Angles[i]);
		else l_Angle.Format("%.02f\t%.02f\t%.02f\n", m_Angles[i], m_ExtDistances[i], m_FlexDistances[i]);
		l_AngleVector += l_Angle;
	}
	if (!OpenClipboard()) {
		MessageBox("Error opening the Clipboard!", "Copy Error", MB_OK | MB_ICONERROR);
		return;
	}
	EmptyClipboard();			
	HGLOBAL hHeader = (char *) ::GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, l_AngleVector.GetLength());
	LPVOID pHeader = ::GlobalLock(hHeader);
	memcpy(pHeader, l_AngleVector, l_AngleVector.GetLength());
	::GlobalUnlock(hHeader);	
	SetClipboardData(CF_TEXT, hHeader);
	CloseClipboard();	
}

CATraceReport::~CATraceReport(void) {
	if (m_Angles) delete m_Angles;
	if (m_FlexDistances) delete m_FlexDistances;
	if (m_ExtDistances) delete m_ExtDistances;
}

void CATraceReport::Allocate(int m_ControlPoints){
	m_Angles = (double *) new double[m_ControlPoints];
	m_FlexDistances = (double *) new double[m_ControlPoints];
	m_ExtDistances = (double *) new double[m_ControlPoints];
	memset(m_Angles, 0, m_ControlPoints*sizeof(double));
	memset(m_FlexDistances, 0, m_ControlPoints*sizeof(double));
	memset(m_ExtDistances, 0, m_ControlPoints*sizeof(double));
}


void CATraceReport::OnAtprint() {
	m_pMemDC = new CDC ;	
	m_pBm = new CBitmap;
	CDC dc;    
	CPrintDialog printDlg(FALSE);
    if (printDlg.DoModal() == IDCANCEL)         // Get printer settings from user
        return;
    dc.Attach(printDlg.GetPrinterDC());         // Attach a printer DC
    dc.m_bPrinting = TRUE;
    CString strTitle;                           // Get the application title
    strTitle.LoadString(AFX_IDS_APP_TITLE);
    DOCINFO di;                                 // Initialise print document details
    ::ZeroMemory (&di, sizeof (DOCINFO));    
	di.cbSize = sizeof (DOCINFO);
    di.lpszDocName = strTitle;
    BOOL bPrintingOK = dc.StartDoc(&di);        // Begin a new print job
    // Get the printing extents and store in the m_rectDraw field of a CPrintInfo object    
	CPrintInfo Info;    
	Info.m_rectDraw.SetRect(0,0, dc.GetDeviceCaps(HORZRES), dc.GetDeviceCaps(VERTRES));
    OnBeginPrinting(&dc, &Info);                // Call your "Init printing" funtion
    for (UINT page = Info.GetMinPage(); page <= Info.GetMaxPage() && bPrintingOK; page++) {
		dc.StartPage();                         // begin new page
        Info.m_nCurPage = page;
        OnPrint(&dc, &Info);                    // Call your "Print page" function
        bPrintingOK = (dc.EndPage() > 0);       // end page    
	}
    OnEndPrinting(&dc, &Info);                  // Call your "Clean up" funtion
    if (bPrintingOK)
        dc.EndDoc();                            // end a print job    
	else
        dc.AbortDoc();                          // abort job.
    dc.Detach();                                // detach the printer DC}
	delete m_pMemDC;
	delete m_pBm;
}


void CATraceReport::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo) {
	/*
	if (m_pMemDC->GetSafeHdc()) m_pMemDC->DeleteDC();
	m_pMemDC->CreateCompatibleDC(pDC);	
	CClientDC dc(this);	
	CRect rect = CRect(CPoint(0, 0), m_sizeTotal);	
	m_pMemDC->SetMapMode(MM_ANISOTROPIC);
	m_pMemDC->SetWindowExt(dc.GetDeviceCaps(LOGPIXELSX),dc.GetDeviceCaps(LOGPIXELSY));
	m_pMemDC->SetViewportExt(m_pMemDC->GetDeviceCaps(LOGPIXELSX),m_pMemDC->GetDeviceCaps(LOGPIXELSY));
	if (m_pBm->GetSafeHandle()) m_pBm->DeleteObject();
	m_pBm->CreateCompatibleBitmap(&dc,rect.Width(),rect.Height());
	m_pMemDC->SelectObject(m_pBm);
	dc.DPtoLP(rect); //Convert to Logical Coordinates	
	m_rect = rect;		
	m_pMemDC->PatBlt(0,0,m_sizeTotal.cx,m_sizeTotal.cy,WHITENESS);		
	CRedimension l_Redimension;	
	l_Redimension.tX = 0;
	l_Redimension.tY = 0;
	// draw the extension and the flexion bitmaps
	m_ToolCervicales->GetExtension()->GetDibTracker()->Paint(m_pMemDC, l_Redimension);
	m_ToolCervicales->GetFlexion()->GetDibTracker()->Paint(m_pMemDC, l_Redimension);
	//GetDocument()->Paint(m_pMemDC, m_Redimension);
	m_ToolCervicales->GetExtension()->GetDibTracker()->Paint(pDC, l_Redimension);
	m_ToolCervicales->GetFlexion()->GetDibTracker()->Paint(pDC, l_Redimension);
	*/
	
}

void CATraceReport::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo) {
	
}

void CATraceReport::OnPrint(CDC * pDC, CPrintInfo * pInfo) {
	/*
	pDC->SetMapMode(MM_ANISOTROPIC);
	pDC->SetWindowExt(m_pMemDC->GetDeviceCaps(LOGPIXELSX),m_pMemDC->GetDeviceCaps(LOGPIXELSY));
	pDC->SetViewportExt(pDC->GetDeviceCaps(LOGPIXELSX),pDC->GetDeviceCaps(LOGPIXELSY));
	CRect t_Rect = m_rect;	
	t_Rect.DeflateRect(t_Rect.Width()/25, t_Rect.Height()/25);
	pDC->StretchBlt(0,0,t_Rect.Width(),t_Rect.Height(),m_pMemDC,0,0,m_rect.Width(),m_rect.Height(),SRCCOPY);
	*/
}
	