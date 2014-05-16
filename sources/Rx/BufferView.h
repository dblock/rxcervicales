/*
	Rx Cervicales - Projet de Recherche sur la Radiologie Cervicales Fonctionnelle
	Travail de Licence, © Univerité de Genève, 1999, Tous droits reservés
	------------------------------------------------------------------------------
	écrit par Daniel Doubrovkine (doubrov5@cuimail.unige.ch / dblock@vestris.com)
	http://cuiwww.unige.ch/~doubrov5/uni/license
	------------------------------------------------------------------------------
	Ce code est du domaine public. Il est distribué sans aucune garantie.
	*/

#ifndef C_BUFFER_VIEW_H
#define C_BUFFER_VIEW_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CBufferView : public CView {
private:
	/*
		Double Buffer
		*/
	CBitmap*	mBitmap,*oldBits;
	CPen*		mPen,*oldPen;
	int			mW,mH,mX,mY;
	CDC*		dragDC;
	CDC*		beginDraw(CDC* = NULL, CRgn* = NULL);	
	void		endDraw(CDC* memDC,CDC* pDC = NULL);
public:
	//{{AFX_VIRTUAL(CBufferView)
public:
	virtual void OnInitialUpdate();	
protected:
	virtual void OnDraw(CDC* pDC);  	
	//}}AFX_VIRTUAL
protected:
	virtual void OnDrawPrivate(CDC* pDC);
	void OnPaint();	
protected:
	CBufferView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CBufferView)
	virtual ~CBufferView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
protected:
	//{{AFX_MSG(CBufferView)
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif 