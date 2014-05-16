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
#include "BufferView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CBufferView, CView)

CBufferView::CBufferView() {
	mBitmap = NULL;
	mPen = new CPen();
	mPen->CreatePen(PS_SOLID, 1, RGB(0,0,0));			
}

CBufferView::~CBufferView() {
	if (mPen) delete mPen;
	if (mBitmap) delete mBitmap;
}


BEGIN_MESSAGE_MAP(CBufferView, CView)
	//{{AFX_MSG_MAP(CBufferView)
	ON_WM_SHOWWINDOW()	
	ON_WM_PAINT()	
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#ifdef _DEBUG
void CBufferView::AssertValid() const {
	CView::AssertValid();
}

void CBufferView::Dump(CDumpContext& dc) const {
	CView::Dump(dc);
}
#endif //_DEBUG

void CBufferView::OnInitialUpdate() {	
	if (mBitmap) delete mBitmap;
	mBitmap = NULL;
	mW = -1;
	mH = -1;
	CView::OnInitialUpdate();
}

void CBufferView::OnShowWindow(BOOL bShow, UINT nStatus) {	
	CView::OnShowWindow(bShow, nStatus);		
}

CDC* CBufferView::beginDraw(CDC* pDC,CRgn* aRgn) {		
	RECT nRect;	GetClientRect(&nRect);	
	CRect aRect(nRect);		
	BOOL iDo = !pDC;
	if (!pDC) pDC = GetDC();	
	if (aRgn) pDC->SelectClipRgn(aRgn); 	
	if (!mBitmap || (mW != aRect.right - aRect.left) || (mH != aRect.bottom-aRect.top)) {
		if (mBitmap) delete mBitmap;
		mBitmap = new CBitmap();
		mX = aRect.left;
		mY = aRect.top;
		mBitmap->CreateCompatibleBitmap(pDC,mW = aRect.right-aRect.left,mH = aRect.bottom - aRect.top);
	}	
	CDC* memDC = new CDC();	
	memDC->CreateCompatibleDC(pDC);
	oldBits = (CBitmap *)memDC->SelectObject(mBitmap);
	oldPen = (CPen* )memDC->SelectObject(mPen);		
	memDC->PatBlt(0,0,mW,mH,WHITENESS);			
	if (iDo) ReleaseDC(pDC);
	return memDC;
}

void CBufferView::endDraw(CDC* memDC,CDC* pDC) {	
	BOOL iDo = !pDC;
	if (!pDC) pDC = GetDC();	
	pDC->BitBlt(0, 0, mW,mH,memDC,0,0,SRCCOPY);	
	memDC->SelectObject(oldBits);
	memDC->SelectObject(oldPen);
	memDC->DeleteDC();
	delete memDC;	
	if (iDo) {
		ReleaseDC(pDC);
		delete pDC;
	}	
}

void CBufferView::OnDrawPrivate(CDC* pDC) {
}


void CBufferView::OnDraw(CDC* pDC) {
	CDC * currentDC = GetDC();
	CDC *memDC = beginDraw(currentDC);	
	OnDrawPrivate(memDC);
	endDraw(memDC,currentDC);	
	ReleaseDC(currentDC);
}

void CBufferView::OnPaint(){
	CPaintDC dc(this);
	OnPrepareDC(&dc);	
	OnDraw(&dc);	
}

