/*
	Rx Cervicales - Projet de Recherche sur la Radiologie Cervicales Fonctionnelle
	Travail de Licence, © Univerité de Genève, 1999, Tous droits reservés
	------------------------------------------------------------------------------
	écrit par Daniel Doubrovkine (doubrov5@cuimail.unige.ch / dblock@vestris.com)
	http://cuiwww.unige.ch/~doubrov5/uni/license
	------------------------------------------------------------------------------
	Ce code est du domaine public. Il est distribué sans aucune garantie.
	*/

#if !defined(AFX_RXVIEW_H__F481A56D_ABBA_11D2_9BFD_000001352962__INCLUDED_)
#define AFX_RXVIEW_H__F481A56D_ABBA_11D2_9BFD_000001352962__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <afxole.h>

#define WM_VIEWPALETTECHANGED	WM_USER + 5

class CRxView : public CScrollView {
private:
	/* print buffer  */
	CDC * m_pMemDC;  //A memory device context compatible with our printer DC.
	CRect m_rect;    //To hold the dimensions of our printing area while scaling.
	CBitmap * m_pBm; //Capture the screen image as a Bitmap
	/* --- */
	CRedimension m_Redimension;
	CSize m_sizeTotal;
	COleDataSource * SaveDib();	
	/* ================================== */
	CPoint initPoint;
	CPoint prevPoint;	
	CPoint prevOpPoint;

	CAVector * STVector;
	CAnnotation * STAnnotation;

	HCURSOR ArrowCursor;
	HCURSOR CrossCursor;
	HCURSOR UpArrowCursor;	

	CBitmap* mBitmap,*oldBits;
	CPen*    mPen,*oldPen;
	int		 mW,mH,mX,mY;
	CDC*	 dragDC;

	CDC*	beginDraw(CDC* = NULL, CRgn* = NULL);	
	void	endDraw(CDC* memDC,CDC* pDC = NULL);

	CColorDialog colorDialog;	
	void AdjustShiftPoint(const CPoint& initPoint, CPoint& point);	
	void SYMMouseMove(CPoint point);
	void LINVECMouseMove(CPoint point);
	void NODEMouseMove(CPoint point);
	BOOL SetScreenCursorLoad(char * Cursor);
	BOOL SetScreenCursor(HCURSOR curCursor);	
	void PrintPageHeader(CDC* pDC, CPrintInfo* pInfo);	
	void SetPageSize(void);
protected:
	CRxView();
	DECLARE_DYNCREATE(CRxView)
public:
	CRxDoc* GetDocument();		
	void RepaintRect(const CRect&);
	void RepaintAll(void);		
	CRect& TranslateRect(CRect& Rect);
	CPoint& TranslatePoint(CPoint& Point);
	CPoint RealPoint(const CPoint& tPoint);
public:		
	//{{AFX_VIRTUAL(CRxView)
	public:
	virtual void OnDraw(CDC* pDC);  
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnScroll(UINT nScrollCode, UINT nPos, BOOL bDoScroll = TRUE);
	protected:
	virtual void OnInitialUpdate(); // called first time after construct
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnPrepareDC(CDC * pDC, CPrintInfo* pInfo = NULL);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrint(CDC * pDC, CPrintInfo * pInfo = NULL);		
	//}}AFX_VIRTUAL
public:
	virtual ~CRxView();		
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
protected:
	//{{AFX_MSG(CRxView)		
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);	
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnEditCopy();
    afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
    afx_msg void OnEditCopyto();
    afx_msg void OnEditCut();
    afx_msg void OnEditPaste();
    afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
    afx_msg void OnEditPastefrom();	
	afx_msg LONG OnViewPaletteChanged(UINT, LONG);		
	afx_msg void OnSetFocus(CWnd *);
	afx_msg void OnViewZoomin();
	afx_msg void OnUpdateViewZoomin(CCmdUI* pCmdUI);
	afx_msg void OnViewZoomout();
	afx_msg void OnUpdateViewZoomout(CCmdUI* pCmdUI);		
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnDtoolColors();	
	afx_msg void OnTimer(UINT nIDEvent);	
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in RxView.cpp
inline CRxDoc* CRxView::GetDocument() { 
	return (CRxDoc*)m_pDocument; 
}
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RXVIEW_H__F481A56D_ABBA_11D2_9BFD_000001352962__INCLUDED_)
