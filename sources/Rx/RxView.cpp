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

#include "RxDoc.h"
#include "RxView.h"
#include "CDib.h"
#include "Rx.h"

extern CRxApp theApp;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define TM_REPAINT_ROTATE 1
#include "MainFrm.h"
#include "DibInformation.h"

IMPLEMENT_DYNCREATE(CRxView, CScrollView)

BEGIN_MESSAGE_MAP(CRxView, CScrollView)
	//{{AFX_MSG_MAP(CRxView)		
	ON_WM_PAINT()	
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()	
	ON_WM_KEYDOWN()
    ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
    ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
    ON_COMMAND(ID_EDIT_COPYTO, OnEditCopyto)
    ON_COMMAND(ID_EDIT_CUT, OnEditCut)
    ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
    ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
    ON_COMMAND(ID_EDIT_PASTEFROM, OnEditPastefrom)        
	ON_MESSAGE(WM_VIEWPALETTECHANGED, OnViewPaletteChanged)
	ON_WM_SETFOCUS()
	ON_COMMAND(ID_VIEW_ZOOMIN, OnViewZoomin)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ZOOMIN, OnUpdateViewZoomin)
	ON_COMMAND(ID_VIEW_ZOOMOUT, OnViewZoomout)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ZOOMOUT, OnUpdateViewZoomout)
	ON_WM_CHAR()	
	ON_COMMAND(ID_DTOOL_COLORS, OnDtoolColors)
	ON_WM_TIMER()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
    ON_UPDATE_COMMAND_UI(ID_EDIT_COPYTO, OnUpdateEditCopy)
    ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCopy)	
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_FILE_PRINT, CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CScrollView::OnFilePrintPreview)
END_MESSAGE_MAP()

BOOL CRxView::PreCreateWindow(CREATESTRUCT& cs) {
	return CScrollView::PreCreateWindow(cs);	
}

void CRxView::OnInitialUpdate() {	
	if (mBitmap) delete mBitmap;
	mBitmap = NULL;
	mW = -1;
	mH = -1;
	CScrollView::OnInitialUpdate();
	SetPageSize();	
	CScrollView::SetTimer(TM_REPAINT_ROTATE, 1500, NULL);
}

void CRxView::SetPageSize(void){
	if (m_sizeTotal != GetDocument()->m_paperSize){
		m_sizeTotal = GetDocument()->m_paperSize;		
		CSize m_viewTotal = m_sizeTotal;
		m_viewTotal.cx += 5;
		m_viewTotal.cy += 5;		
		CSize sizePage(m_sizeTotal.cx / 2, m_sizeTotal.cy / 2);	
		CSize sizeLine(m_sizeTotal.cx / 50, m_sizeTotal.cy / 50);		
		SetScrollSizes(MM_TEXT, m_viewTotal, sizePage, sizeLine);	
	}
}

CRxView::CRxView() {
	STVector = NULL;
	mBitmap = NULL;
	STAnnotation = NULL;
	mPen = new CPen();
	mPen->CreatePen(PS_SOLID, 1, RGB(0,0,0));			
	ArrowCursor = LoadCursor(0, IDC_ARROW);
	CrossCursor = LoadCursor(0, IDC_CROSS);
	UpArrowCursor = LoadCursor(0, IDC_UPARROW);	
	
	m_pMemDC = new CDC ;	
	m_pBm = new CBitmap;
}

CRxView::~CRxView() {
	//CScrollView::KillTimer(TM_REPAINT_ROTATE);
	if (mBitmap) delete mBitmap;
	if (mPen) delete mPen;
	
	delete m_pMemDC; //CLEAN UP OUR VARIABLES	
	delete	m_pBm;
}

CDC * CRxView::beginDraw(CDC* pDC,CRgn* aRgn) {		
	RECT nRect;	CScrollView::GetClientRect(&nRect);	
	CRect aRect(nRect);
	BOOL iDo = !pDC;
	if (!pDC) pDC = GetDC();
	pDC->SetMapMode(MM_ANISOTROPIC);
	if (aRgn) pDC->SelectClipRgn(aRgn); 
	if (!mBitmap || (mW != aRect.right - aRect.left) || (mH != aRect.bottom-aRect.top)) {
		if (mBitmap) delete mBitmap;
		mBitmap = new CBitmap();
		mBitmap->CreateCompatibleBitmap(pDC,mW = (aRect.right-aRect.left),mH = (aRect.bottom - aRect.top));
	}	
	CDC* memDC = new CDC();	
	memDC->CreateCompatibleDC(pDC);
	oldBits = (CBitmap *)memDC->SelectObject(mBitmap);
	oldPen = (CPen* )memDC->SelectObject(mPen);		
	
	memDC->PatBlt(0,0,mW,mH,WHITENESS);
	/*
	if (aRgn) {
		memDC->SelectStockObject(GRAY_BRUSH);	
		memDC->FillRgn(aRgn, memDC->GetCurrentBrush());
	}
	*/	
	m_Redimension.tX = -CScrollView::GetScrollPosition().x;
	m_Redimension.tY = -CScrollView::GetScrollPosition().y;
	m_Redimension.tFontScale = 1;

	GetDocument()->Paint(memDC, m_Redimension);	
	((CMainFrame *) theApp.m_pMainWnd)->ProcessView(GetDocument(), theApp.p_InfoTemplate, RUNTIME_CLASS(CDibInformation), frmInvalidate);
	if (STVector) STVector->Paint(memDC, m_Redimension);	
	memDC->SelectStockObject(GRAY_BRUSH);
	CRect l_FrameRect(5, 5, m_sizeTotal.cx, m_sizeTotal.cy);
	TranslateRect(l_FrameRect);
	memDC->FrameRect(l_FrameRect, memDC->GetCurrentBrush());
	if (iDo) ReleaseDC(pDC);
	return memDC;
}

void CRxView::endDraw(CDC* memDC,CDC* pDC) {
	BOOL iDo = !pDC;
	if (!pDC) pDC = GetDC();
	//pDC->BitBlt(-CScrollView::GetScrollPosition().x, -CScrollView::GetScrollPosition().y, mW,mH,memDC,0,0,SRCCOPY);	
	pDC->BitBlt(0,0,mW,mH,memDC,0,0,SRCCOPY);	
	memDC->SelectObject(oldBits);
	memDC->SelectObject(oldPen);
	memDC->DeleteDC();
	delete memDC;
	if (iDo) {
		ReleaseDC(pDC);
		delete pDC;
	}
}

void CRxView::OnDraw(CDC* pDC) {			
	SetPageSize();
	CDC * currentDC = GetDC();
	CDC *memDC = beginDraw(currentDC);	
	endDraw(memDC,currentDC);	
	ReleaseDC(currentDC);
	/*	
		CDC *memDC = beginDraw(pDC);
		endDraw(memDC,pDC);	
		*/
}


BOOL CRxView::OnPreparePrinting(CPrintInfo* pInfo){			
	pInfo->SetMaxPage(1);	
	return DoPreparePrinting(pInfo);
}

void CRxView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo){	
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
	m_Redimension.tX = 0;
	m_Redimension.tY = 0;
	GetDocument()->Paint(m_pMemDC, m_Redimension);	
}

void CRxView::OnEndPrinting(CDC* pDC, CPrintInfo* /*pInfo*/){	
	RepaintAll();
}

#ifdef _DEBUG
void CRxView::AssertValid() const {
	CScrollView::AssertValid();
}

void CRxView::Dump(CDumpContext& dc) const {
	CScrollView::Dump(dc);
}

CRxDoc* CRxView::GetDocument() {
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CRxDoc)));
	return (CRxDoc*)m_pDocument;
}
#endif

void CRxView::OnRButtonDown(UINT, CPoint point) {	
	switch(GetDocument()->GetTool()){
	case ID_DTOOL_LINE:
	case ID_DTOOL_DISTANCE:
	case ID_DTOOL_VECTOR:
	case ID_DTOOL_MDIST:
	case ID_DTOOL_ALPHA:
		if (STVector){
			AdjustShiftPoint(initPoint, prevPoint);
			STVector->Add(RealPoint(prevPoint));
			STVector->SetColor(GetDocument()->GetColor());
			if ((GetDocument()->GetTool() == ID_DTOOL_DISTANCE)||(GetDocument()->GetTool() == ID_DTOOL_MDIST)) STVector->SetShowDistance();
			GetDocument()->Add(* STVector);			
			GetDocument()->Select(RealPoint(prevPoint));
			delete STVector;
			STVector = NULL;
			ReleaseCapture();
			RepaintAll();
		}
		break;
	}
		
}

void CRxView::OnLButtonDown(UINT, CPoint point) {
	CPoint OpPoint;
	CRect m_Move = GetDocument()->GetBoundMove();
	switch(GetDocument()->GetTool()){
	case ID_DTOOL_TEXT:
		if (!STAnnotation) {
			GetDocument()->Deselect();
			STAnnotation = new CAnnotation;
			STAnnotation->Select(TRUE);
			STAnnotation->SetEditing(TRUE);
			STAnnotation->SetPosition(RealPoint(point));
			GetDocument()->Add(* STAnnotation);
			RepaintAll();			
			delete STAnnotation;
			STAnnotation = NULL;
		}
		break;
	case ID_DTOOL_SYM:
		AdjustShiftPoint(initPoint, point);
		if (STVector){
			OpPoint = CPoint(2*initPoint.x-point.x, 2*initPoint.y-point.y);
			STVector->Add(RealPoint(OpPoint));
			STVector->Add(RealPoint(initPoint));
			STVector->Add(RealPoint(point));
			GetDocument()->Add(* STVector);
			GetDocument()->Select(RealPoint(initPoint));
			m_Move |= GetDocument()->GetBoundMove();
			RepaintRect(m_Move);
			ReleaseCapture();
			delete STVector;
			STVector = NULL;
		} else {
			GetDocument()->Deselect();
			SetCapture();
			STVector = new CAVector();
			STVector->SetColor(GetDocument()->GetColor());			
			if ((GetDocument()->GetTool() == ID_DTOOL_DISTANCE)||(GetDocument()->GetTool() == ID_DTOOL_MDIST)) STVector->SetShowDistance();
			m_Move |= GetDocument()->GetBoundMove();
			RepaintRect(m_Move);
		}
		initPoint = point;
		prevPoint = point;
		break;
	case ID_DTOOL_VECTOR:		
	case ID_DTOOL_MDIST:
		AdjustShiftPoint(initPoint, point);
		if (STVector){
			STVector->Add(RealPoint(point));			
			RepaintRect(STVector->Bound());
			//RepaintAll();
		} else {			
			GetDocument()->Deselect();
			SetCapture();
			STVector = new CAVector();
			STVector->SetColor(GetDocument()->GetColor());
			if ((GetDocument()->GetTool() == ID_DTOOL_DISTANCE)||(GetDocument()->GetTool() == ID_DTOOL_MDIST)) STVector->SetShowDistance();
			STVector->Add(RealPoint(point));
			m_Move |= GetDocument()->GetBoundMove();
			RepaintRect(m_Move);
		}
		initPoint = point;
		prevPoint = point;
		break;		
	case ID_DTOOL_ALPHA:
	case ID_DTOOL_LINE:		
	case ID_DTOOL_DISTANCE:
		AdjustShiftPoint(initPoint, point);
		if (STVector){			
			STVector->Add(RealPoint(point));
			if (GetDocument()->GetTool() == ID_DTOOL_DISTANCE) STVector->SetShowDistance();
			if ((STVector->GetSize() == 3)||(GetDocument()->GetTool() != ID_DTOOL_ALPHA)) {
				GetDocument()->Add(* STVector);
				GetDocument()->Select(RealPoint(point));
				m_Move |= GetDocument()->GetBoundMove();
				RepaintRect(m_Move);
				ReleaseCapture();
				delete STVector;
				STVector = NULL;
			} else {
				initPoint = point;
				prevPoint = point;
			}
		} else {
			GetDocument()->Deselect();
			SetCapture();
			STVector = new CAVector();
			STVector->SetColor(GetDocument()->GetColor());
			if ((GetDocument()->GetTool() == ID_DTOOL_DISTANCE)||(GetDocument()->GetTool() == ID_DTOOL_MDIST)) STVector->SetShowDistance();
			initPoint = point;
			prevPoint = point;
			STVector->Add(RealPoint(point));
			m_Move |= GetDocument()->GetBoundMove();
			RepaintRect(m_Move);
		}
		break;
	case ID_DTOOL_ROTATE:
		GetDocument()->Select(RealPoint(point));
		SetCapture();
		m_Move |= GetDocument()->GetBoundMove();
		RepaintRect(m_Move);
		break;
	case ID_DTOOL_ERASE:
	case ID_DTOOL_NODE:
		if (GetDocument()->Select(RealPoint(point))) SetCapture();
		m_Move |= GetDocument()->GetBoundMove();
		RepaintRect(m_Move);
		break;
	}	
}

void CRxView::RepaintRect(const CRect& iRect){		
	dragDC = GetDC();
	CRect nRect = iRect;
	TranslateRect(nRect);
	CRgn aRgn; aRgn.CreateRectRgn(
		nRect.left - 1,
		nRect.top - 1,
		nRect.right+1,
		nRect.bottom+1);
	CDC * memDC = beginDraw(dragDC,&aRgn);			
	endDraw(memDC,dragDC);	
	ReleaseDC(dragDC);
}

void CRxView::OnLButtonUp(UINT, CPoint point) {	
	CRect m_Move = GetDocument()->GetBoundMove();
	if (GetCapture() != this) return;			
	CPoint OpPoint;
	switch(GetDocument()->GetTool()){
	case ID_DTOOL_SYM:
		AdjustShiftPoint(initPoint, point);
		if ((STVector)&&((abs(initPoint.x - point.x) > 20)||(abs(initPoint.y - point.y) > 20))){						
			STVector->SetColor(GetDocument()->GetColor());						
			OpPoint = CPoint(2*initPoint.x-point.x, 2*initPoint.y-point.y);
			STVector->Add(RealPoint(OpPoint));
			STVector->Add(RealPoint(initPoint));
			STVector->Add(RealPoint(point));
			GetDocument()->Add(* STVector);
			GetDocument()->Select(RealPoint(initPoint));
			//RepaintRect(STVector->Bound());
			//RepaintAll();
			m_Move |= GetDocument()->GetBoundMove();
			RepaintRect(m_Move);
			ReleaseCapture();
			delete STVector;
			STVector = NULL;
		}
	case ID_DTOOL_VECTOR:
	case ID_DTOOL_MDIST:
		AdjustShiftPoint(initPoint, point);
		if ((STVector)&&((abs(initPoint.x - point.x) > 20)||(abs(initPoint.y - point.y) > 20))) {
			STVector->Add(RealPoint(point));									
			//RepaintRect(STVector->Bound());
			//RepaintAll();
			m_Move |= GetDocument()->GetBoundMove();
			RepaintRect(m_Move);
			initPoint = point;
			prevPoint = point;
		}
		break;		
	case ID_DTOOL_ALPHA:
	case ID_DTOOL_LINE:
	case ID_DTOOL_DISTANCE:
		AdjustShiftPoint(initPoint, point);
		if ((STVector)&&((abs(initPoint.x - point.x) > 20)||(abs(initPoint.y - point.y) > 20))){			
			STVector->SetColor(GetDocument()->GetColor());
			STVector->Add(RealPoint(point));
			if (GetDocument()->GetTool() == ID_DTOOL_DISTANCE) STVector->SetShowDistance();
			if ((STVector->GetSize() == 3)||(GetDocument()->GetTool() != ID_DTOOL_ALPHA)) {
				GetDocument()->Add(* STVector);
				GetDocument()->Select(RealPoint(point));			
				//RepaintRect(STVector->Bound());
				//RepaintAll();
				m_Move |= GetDocument()->GetBoundMove();
				RepaintRect(m_Move);
				ReleaseCapture();			
				delete STVector;
				STVector = NULL;
			} else {
				prevPoint = point;
				initPoint = point;
			}
		}
		break;
	case ID_DTOOL_ROTATE:
		GetDocument()->MouseUp(point);
		ReleaseCapture();
		if (!GetDocument()->GetShowContents()) {
			m_Move |= GetDocument()->GetBoundMove();
			RepaintRect(m_Move);
			//RepaintRect(GetDocument()->GetBoundMove());
		}
		break;
	case ID_DTOOL_ERASE:
	case ID_DTOOL_NODE:				 
		GetDocument()->MouseUp(point);		
		ReleaseCapture();	
		RepaintAll();		
		break;
	}	
}


void CRxView::AdjustShiftPoint(const CPoint& initPoint, CPoint& point){	
	if (GetKeyState(VK_SHIFT) >> 3){
		int dx = abs(initPoint.x-point.x);
		int dy = abs(initPoint.y-point.y);
		if (dx > dy) point.y = initPoint.y;
		else point.x = initPoint.x;
	}	
}

void CRxView::SYMMouseMove(CPoint point){	
	AdjustShiftPoint(initPoint, point);
	CPoint OpPoint = CPoint(2*initPoint.x-point.x, 2*initPoint.y-point.y);			
	CRgn aRgn;
	int m1 = min(min(min(min(prevPoint.x, initPoint.x),point.x), OpPoint.x), prevOpPoint.x)-1;
	int	m2 = min(min(min(min(prevPoint.y, initPoint.y), point.y), OpPoint.y), prevOpPoint.y)-1;
	int m3 = max(max(max(max(prevPoint.x, initPoint.x), point.x), OpPoint.x), prevOpPoint.x)+1;
	int m4 = max(max(max(max(prevPoint.y, initPoint.y), point.y), OpPoint.y), prevOpPoint.y)+1;	
	aRgn.CreateRectRgn(m1, m2, m3, m4);		  	
	dragDC = GetDC();
	CDC * memDC = beginDraw(dragDC,&aRgn);
	CPen newPen(PS_SOLID, 1, (COLORREF) GetDocument()->GetColor());			
	CPen * oldPen = memDC->SelectObject(&newPen);
	memDC->MoveTo(initPoint);		
	memDC->LineTo(point);
	memDC->MoveTo(initPoint);
	memDC->LineTo(OpPoint);				
	memDC->FillRect(CRect(initPoint.x - 2, initPoint.y - 2, initPoint.x + 2, initPoint.y + 2), memDC->GetCurrentBrush());		
	memDC->SelectObject(oldPen);
	endDraw(memDC,dragDC);
	ReleaseDC(dragDC);	
	prevPoint = point;		
	prevOpPoint = OpPoint;
}

void CRxView::LINVECMouseMove(CPoint point){
	AdjustShiftPoint(initPoint, point);		
	CRgn aRgn; 
	int m1 = min(min(prevPoint.x, initPoint.x),point.x)-1;
	int m2 = min(min(prevPoint.y, initPoint.y), point.y)-1;
	int m3 = max(max(prevPoint.x, initPoint.x), point.x)+1;
	int m4 = max(max(prevPoint.y, initPoint.y), point.y)+1;
	/*
	m1 -= CScrollView::GetScrollPosition().x;
	m2 += CScrollView::GetScrollPosition().y;
	*/
	aRgn.CreateRectRgn(m1, m2, m3, m4);
	dragDC = GetDC();
	CDC * memDC = beginDraw(dragDC,&aRgn);		
	CPen newPen(PS_SOLID, 1, (COLORREF) GetDocument()->GetColor());			
	CPen * oldPen = memDC->SelectObject(&newPen);			
	memDC->MoveTo(initPoint);		
	memDC->LineTo(point);		
	memDC->SelectObject(oldPen);	
	endDraw(memDC,dragDC);
	ReleaseDC(dragDC);
	prevPoint = point;
}

void CRxView::NODEMouseMove(CPoint point){	
	CRect lRect = GetDocument()->GetBoundMove();
	if (GetDocument()->MouseMove(point)){		
		lRect |= GetDocument()->GetBoundMove();		
		RepaintRect(lRect);
		//RepaintAll();
		SetScreenCursorLoad(GetDocument()->m_Cursor);
	}
}

BOOL CRxView::SetScreenCursor(HCURSOR curCursor){	
	SetCursor(curCursor);
	return TRUE;	
}

BOOL CRxView::SetScreenCursorLoad(char * Cursor){
	if (Cursor == IDC_ARROW) return SetScreenCursor(ArrowCursor);
	else if (Cursor == IDC_CROSS) return SetScreenCursor(CrossCursor);
	else if (Cursor == IDC_UPARROW) return SetScreenCursor(UpArrowCursor);	
	else return FALSE;
}

void CRxView::OnMouseMove(UINT, CPoint point) {			
	if (!GetDocument()->OnMouseMove(RealPoint(point))){		
		GetDocument()->m_Cursor = IDC_ARROW;
	} 
	if (GetCapture() != this) {
		SetScreenCursorLoad(GetDocument()->m_Cursor);
		return;	
	}
	switch(GetDocument()->GetTool()){	
	case ID_DTOOL_SYM:
		SYMMouseMove(point);		
		break;
	case ID_DTOOL_LINE:
	case ID_DTOOL_DISTANCE:
	case ID_DTOOL_VECTOR:
	case ID_DTOOL_MDIST:
	case ID_DTOOL_ALPHA:
		LINVECMouseMove(point);
		break;
	case ID_DTOOL_ERASE:
	case ID_DTOOL_NODE:
	case ID_DTOOL_ROTATE:
		NODEMouseMove(RealPoint(point));
		break;
	}	
}

void CRxView::OnPaint(){
	CPaintDC dc(this);
	OnPrepareDC(&dc);	
	OnDraw(&dc);	
}

void CRxView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags){	
	/*
	CString Tmp; Tmp.Format("%d", nChar);
	MessageBox(Tmp);
	*/

	CRect nRect;
	switch(nChar){
	case 9:		
		if (GetKeyState(VK_SHIFT) & 128 > 0) {
			if (GetDocument()->SelectPrevElement()) RepaintAll();
		} else if (GetDocument()->SelectNextElement()) RepaintAll();		
		
		break;
	case 13:	
	case 10:
	case 27:
		switch(GetDocument()->GetTool()){
		case ID_DTOOL_TEXT:
		case ID_DTOOL_ALPHA:
		case ID_DTOOL_LINE:
		case ID_DTOOL_DISTANCE:
		case ID_DTOOL_VECTOR:
		case ID_DTOOL_MDIST:
			if (STVector){
				AdjustShiftPoint(initPoint, prevPoint);
				if ((nChar!=27)||
					((GetDocument()->GetTool()!=ID_DTOOL_VECTOR)&&
					 (GetDocument()->GetTool()!=ID_DTOOL_MDIST))) STVector->Add(RealPoint(prevPoint));
				if ((GetDocument()->GetTool() == ID_DTOOL_DISTANCE)||(GetDocument()->GetTool() == ID_DTOOL_MDIST)) STVector->SetShowDistance();
				GetDocument()->Add(* STVector);
				GetDocument()->Select(RealPoint(initPoint));
				STVector->SetColor(GetDocument()->GetColor());
				delete STVector;
				STVector = NULL;
				ReleaseCapture();	
				if ((nChar==27)&&((GetDocument()->GetTool()==ID_DTOOL_VECTOR)||(GetDocument()->GetTool()==ID_DTOOL_MDIST))) {
					RepaintAll();
				}
			}			
			break;
		}
		break;
	case 46: /* delete */			
		switch (GetDocument()->GetTool()) {
		case ID_DTOOL_TEXT:		
			GetDocument()->OnKeyDown(nChar, nRepCnt, nFlags);
			RepaintRect(GetDocument()->GetBoundMove());	
			break;
		default:
			if (GetDocument()->DeleteSelected()) RepaintAll();			
			break;
		}
		break;
	case 37:
	case 39:
	case 36:
	case 35:
		switch (GetDocument()->GetTool()) {
		case ID_DTOOL_TEXT:
		case ID_DTOOL_ERASE:
		case ID_DTOOL_NODE:
			GetDocument()->OnKeyDown(nChar, nRepCnt, nFlags);
			RepaintRect(GetDocument()->GetBoundMove());	
			break;
		}	
	}	
}

COleDataSource* CRxView::SaveDib(){	
	return GetDocument()->SaveDib();
}

void CRxView::OnEditCopy() {	
	BeginWaitCursor();
	if (OpenClipboard()){
		GetDocument()->SaveDib();
		CloseClipboard();
	}
	/*
	COleDataSource * pSource = GetDocument()->SaveDib();
	if (pSource) {
		pSource->SetClipboard();
		CloseClipboard();
	}
	*/
	GetDocument()->SaveDib();
	EndWaitCursor();
}

void CRxView::OnEditCopyto() {	
	BeginWaitCursor();
	try {
		GetDocument()->CopyDibTo();    
	} catch (...) { 
		theApp.ShowError(); 
	}
	EndWaitCursor();
}

void CRxView::OnEditCut() {
	BeginWaitCursor();
	try {
		OnEditCopy();
		GetDocument()->OnEditClearSelected();
	} catch (...) { 
		theApp.ShowError(); 
	}
	EndWaitCursor();
}

void CRxView::OnEditPaste() {	
	BeginWaitCursor();
	try {
		GetDocument()->EditPaste(CClientDC(this));
	} catch (...) { 
		theApp.ShowError(); 
	}
	EndWaitCursor();	
}

void CRxView::OnUpdateEditPaste(CCmdUI* pCmdUI) {
    COleDataObject dataObject;
    BOOL bAvail = dataObject.AttachClipboard() &&
        dataObject.IsDataAvailable(CF_DIB);
    pCmdUI->Enable(bAvail);
}

void CRxView::OnEditPastefrom() {
	BeginWaitCursor();
	try {
		GetDocument()->EditPastefrom(CClientDC(this));
	} catch (...) { 
		theApp.ShowError(); 
	}
	EndWaitCursor();
}

LONG CRxView::OnViewPaletteChanged(UINT wParam, LONG lParam) {		
	CClientDC dc(this);	
    GetDocument()->UsePalette(&dc, wParam);	
	RepaintAll();
	return 0;
}

void CRxView::OnSetFocus(CWnd* pOldWnd) {
	CScrollView::OnSetFocus(pOldWnd);
	AfxGetApp()->m_pMainWnd->SendMessage(WM_PALETTECHANGED, (UINT) GetSafeHwnd());
}


void CRxView::OnUpdateEditCopy(CCmdUI* pCmdUI) {
	pCmdUI->Enable(GetDocument()->GetSelectedDIBIndex() != -1);	
}

void CRxView::RepaintAll(void){		
	/*
	dragDC = GetDC();
	OnDraw(dragDC);	
	ReleaseDC(dragDC);	
	*/
	OnDraw(NULL);
}

void CRxView::OnPrepareDC(CDC * pDC, CPrintInfo* pInfo){		
	/*
	CScrollView::OnPrepareDC(pDC, pInfo);
	CSize sizeDoc = m_sizeTotal;
	sizeDoc.cy = -sizeDoc.cy;
	pDC->SetWindowExt(sizeDoc);
	*/
	/*
	CSize sizeNum = m_sizeTotal, sizeDenom = m_sizeTotal; 
	//GetDocument()->GetZoomFactor(&sizeNum, &sizeDenom);
	int xLogPixPerInch = pDC->GetDeviceCaps(LOGPIXELSX);
	int yLogPixPerInch = pDC->GetDeviceCaps(LOGPIXELSY);
	long xExt = (long)sizeDoc.cx * xLogPixPerInch * sizeNum.cx;
	xExt /= 100 * (long)sizeDenom.cx;
	long yExt = (long)sizeDoc.cy * yLogPixPerInch * sizeNum.cy;
	yExt /= 100 * (long)sizeDenom.cy;
	pDC->SetViewportExt((int)xExt, (int)-yExt);
	*/
	/*
	double l_LogPixels = pDC->GetDeviceCaps(LOGPIXELSX);
	if (pDC->IsPrinting()) {
		switch((int) l_LogPixels) {
		case 600:
			m_Redimension.tFontScale = 2.3; //(m_sizeTotal.cx/l_LogPixels)*2.3;
			break;
		case 300:
			m_Redimension.tFontScale = 1.7;
			break;
		default:
			m_Redimension.tFontScale = 1;
			break;
		}
	} else m_Redimension.tFontScale = 1;
	*/
	/*
	CString Tmp; Tmp.Format("%d %f",(int)l_LogPixels, m_Redimension.tFontScale); 
	MessageBox(Tmp);
	*/
	/*
	pDC->SetViewportExt((long) (sizeDoc.cx * m_Redimension.tFontScale), -(long) (sizeDoc.cy * m_Redimension.tFontScale));
	pDC->SetMapMode(MM_ANISOTROPIC);
	*/
	CScrollView::OnPrepareDC(pDC, pInfo);	
}

void CRxView::OnPrint(CDC * pDC, CPrintInfo * pInfo){			
	/*
	CSize old_viewportExt = pDC->GetViewportExt();	
	CSize old_windowExt = pDC->GetWindowExt();
	
	  CString Tmp; Tmp.Format("%d:%d %d:%d %d:%d", 
		old_viewportExt.cx, old_viewportExt.cy, 
		old_windowExt.cx, old_windowExt.cy,
		pInfo->m_rectDraw.Width(),-pInfo->m_rectDraw.Height());
	MessageBox(Tmp);
	

	pDC->SetWindowOrg(pInfo->m_rectDraw.left,-pInfo->m_rectDraw.top);
	PrintPageHeader(pDC, pInfo);	

	pDC->SetMapMode(MM_ANISOTROPIC);	
	CSize sizeDoc = m_sizeTotal;
	sizeDoc.cy = -sizeDoc.cy;
	pDC->SetWindowExt(sizeDoc);

	int xLogPixPerInch = pDC->GetDeviceCaps(LOGPIXELSX);
	int yLogPixPerInch = pDC->GetDeviceCaps(LOGPIXELSY);
	
	long xExt = ((long)sizeDoc.cx * xLogPixPerInch)/100;
	long yExt = ((long)sizeDoc.cy * yLogPixPerInch)/100;	
	
	float l_X = ((float)1.1 * xLogPixPerInch / 600);
	float l_Y = ((float)1.1 * yLogPixPerInch / 600);

	CString Tmp; Tmp.Format("%f:%f", l_X, l_Y);
	MessageBox(Tmp);
	pDC->SetViewportExt((int)(xExt*l_X), (int)(yExt*l_Y));
	*/
	/*
	int xLogPixPerInch = pDC->GetDeviceCaps(LOGPIXELSX);
	int yLogPixPerInch = pDC->GetDeviceCaps(LOGPIXELSY);
	*/
	/*
	CString Tmp; Tmp.Format("%d:%d", xLogPixPerInch, yLogPixPerInch);
	MessageBox(Tmp);
	*/
	/*
	pDC->SetWindowOrg(pInfo->m_rectDraw.left,-pInfo->m_rectDraw.top);
	PrintPageHeader(pDC, pInfo);
	pDC->SetWindowOrg(-10, -60);
	m_Redimension.tX = 0;
	m_Redimension.tY = 0;	
	GetDocument()->Paint(pDC, m_Redimension);
	*/
	/*
	pDC->SetMapMode(MM_ANISOTROPIC);
	pDC->SetWindowOrg(pInfo->m_rectDraw.left,-pInfo->m_rectDraw.top);
	pDC->SetViewportExt(pDC->GetDeviceCaps(LOGPIXELSX)*m_sizeTotal.cx, pDC->GetDeviceCaps(LOGPIXELSY)*m_sizeTotal.cy);
	pDC->SetWindowExt(m_sizeTotal);
	PrintPageHeader(pDC, pInfo);
	pDC->SetWindowOrg(-10, -60);
	GetDocument()->Paint(pDC, m_Redimension);
	*/
	/*
	PrintPageHeader(pDC, pInfo);
	GetDocument()->Paint(pDC, m_Redimension);
	*/
	pDC->SetMapMode(MM_ANISOTROPIC);
	pDC->SetWindowExt(m_pMemDC->GetDeviceCaps(LOGPIXELSX),m_pMemDC->GetDeviceCaps(LOGPIXELSY));
	pDC->SetViewportExt(pDC->GetDeviceCaps(LOGPIXELSX),pDC->GetDeviceCaps(LOGPIXELSY));
	CRect t_Rect = m_rect;	
	t_Rect.DeflateRect(t_Rect.Width()/25, t_Rect.Height()/25);
	pDC->StretchBlt(0,0,t_Rect.Width(),t_Rect.Height(),m_pMemDC,0,0,m_rect.Width(),m_rect.Height(),SRCCOPY);
}

void CRxView::OnViewZoomin() {
	
}

void CRxView::OnUpdateViewZoomin(CCmdUI* pCmdUI) {
	pCmdUI->Enable(FALSE);
}

void CRxView::OnViewZoomout() {
	
}

void CRxView::OnUpdateViewZoomout(CCmdUI* pCmdUI) {
	pCmdUI->Enable(FALSE);	
}


void CRxView::PrintPageHeader(CDC* pDC, CPrintInfo* pInfo) {
	pDC->SetTextAlign(TA_LEFT);
	CFont m_Font; m_Font.CreateFont(16, 0, 0, 0, 400, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
				 CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Helvetica");
	CFont * oldFont = pDC->SelectObject(&m_Font);	
	pDC->SetTextColor((COLORREF) 0);
	TEXTMETRIC textMetric;	pDC->GetTextMetrics(&textMetric);

	pDC->TextOut(0,-25, "Rx Cervicales - © University of Geneva, All Rights Reserved");
	pDC->TextOut(0,-25 - textMetric.tmHeight, "written by Daniel Doubrovkine (doubrov5@cuimail.unige.ch), http://cuiwww.unige.ch/~doubrov5");
	
	time_t c_Time; time(&c_Time); 
	CString tStr; tStr = asctime(localtime(&c_Time)); tStr.TrimRight('\n');	
	pDC->TextOut(0,-25 - 2*textMetric.tmHeight, tStr);	
	
	int y = -25 - 3*textMetric.tmHeight;
	pDC->MoveTo(0, y);
	pDC->LineTo(pInfo->m_rectDraw.right, y);	
	
	pInfo->m_rectDraw.top += y;

	pDC->SelectObject(oldFont);
}

BOOL CRxView::OnScroll(UINT nScrollCode, UINT nPos, BOOL bDoScroll){
	if (CScrollView::OnScroll(nScrollCode, nPos, bDoScroll)) {
		RepaintAll();
		return TRUE;
	} else return FALSE;
}

void CRxView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) {
	GetDocument()->OnChar(nChar, nRepCnt, nFlags);
	RepaintRect(GetDocument()->GetBoundMove());	
	CScrollView::OnChar(nChar, nRepCnt, nFlags);
}



void CRxView::OnDtoolColors() {
	if (colorDialog.DoModal() == IDOK){
		GetDocument()->SetColor(colorDialog.GetColor());
		GetDocument()->SetSelectedColor(GetDocument()->GetColor());
		if (STVector) STVector->SetColor(GetDocument()->GetColor());
		RepaintRect(GetDocument()->GetBoundMove());
		//RepaintAll();		
	}
}


void CRxView::OnTimer(UINT nIDEvent) {
	CScrollView::OnTimer(nIDEvent);
	if (GetCapture() != this) return;	
	if (GetDocument()->OnTimer(nIDEvent)) {
		RepaintRect(GetDocument()->GetBoundMove());
	}
}

CRect& CRxView::TranslateRect(CRect& Rect) {
	int s_Pos_x = CScrollView::GetScrollPosition().x;
	int s_Pos_y = CScrollView::GetScrollPosition().y;
	Rect.left -= s_Pos_x;
	Rect.top -= s_Pos_y;
	Rect.right -= s_Pos_x;
	Rect.bottom -= s_Pos_y;
	return Rect;
}

CPoint& CRxView::TranslatePoint(CPoint& Point){
	Point.x -= CScrollView::GetScrollPosition().x;
	Point.y -= CScrollView::GetScrollPosition().y;
	return Point;
}

CPoint CRxView::RealPoint(const CPoint& tPoint) {
	return CPoint(
		tPoint.x+CScrollView::GetScrollPosition().x,
		tPoint.y+CScrollView::GetScrollPosition().y);
}
