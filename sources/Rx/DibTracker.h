/*
	Rx Cervicales - Projet de Recherche sur la Radiologie Cervicales Fonctionnelle
	Travail de Licence, © Univerité de Genève, 1999, Tous droits reservés
	------------------------------------------------------------------------------
	écrit par Daniel Doubrovkine (doubrov5@cuimail.unige.ch / dblock@vestris.com)
	http://cuiwww.unige.ch/~doubrov5/uni/license
	------------------------------------------------------------------------------
	Ce code est du domaine public. Il est distribué sans aucune garantie.
	*/

#ifndef C_DIBTRACKER_H
#define C_DIBTRACKER_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CRxView;
#include "CDib.h"
#include "VectorObject.h"

typedef enum CAnchorPosition {AP_VOID, AP_TOPLEFT, AP_TOPMIDDLE, AP_TOPRIGHT,
							  AP_MIDDLELEFT, AP_MIDDLERIGHT, AP_MIDDLETOP, AP_MIDDLEBOTTOM,
							  AP_BOTTOMLEFT, AP_BOTTOMMIDDLE, AP_BOTTOMRIGHT};

class CDibTracker : public CVectorObject {
private:	
	friend class CDib;
	CPen m_Pen;
	CBrush m_Brush;	
	CPoint m_InitSelPoint;
	CDib m_dib;	
	CSize m_size;
	CRect m_rect;	
	CRect Anchor(CAnchorPosition);	
	CPoint AnchorPt(CAnchorPosition Pos);
	CAnchorPosition m_Anchor;
	void AdjustShiftPoint(CPoint&);
	CPoint m_BottomLeft;
	CPoint m_BottomRight;
	CPoint m_TopRight;
	CPoint m_TopLeft;
	int m_SelAngle;
	int m_ChgAngle;
	CSize m_ChgSize;
	CAnchorPosition NormalizeAnchor(CAnchorPosition Anchor);
public:
	char * m_Cursor;
	BOOL SetDibSize(void);
	BOOL SetDibRotation(void);
	BOOL MouseMove(const CPoint& APoint);
	BOOL OnMouseMove(const CPoint& APoint);
	BOOL isEmpty(void);
	void Paint(CDC *, const CRedimension&);
	void Paint(CDC *, const CRedimension&, BOOL ShowContents);
	void Serialize(CArchive&);
	CRect GetRect(void);
	CDib * GetDib(void);
	CSize GetSize(void);
	CDibTracker(void);
	CDibTracker(const CDib&);
	CDibTracker(const CDibTracker&);
	CDibTracker& operator=(const CDibTracker&);
	virtual ~CDibTracker(void);	
	void ViewPaletteChanged(CClientDC& dc, UINT wParam);	
	void UsePalette(CDC *, BOOL bBackground = FALSE);
	void SetSystemPalette(CDC *);
	COleDataSource* SaveDib(void);	
	void CopyDibTo(void);
	BOOL EditPastefrom(void);	
	BOOL EditPaste(void);
	BOOL Select(const CPoint& Point);	
	CPoint GetCenter(void);
	CRgn& GetRotateRgn(CRgn&);
	CRgn& GetAnchorRgn(CRgn&);
	BOOL Rotate(const CPoint& APoint);
	BOOL RotateSelect(const CPoint& APoint);
	BOOL GetRotating(void) const;
	BOOL GetRotatingChanged(void);
	void EndMove(void);
	BOOL MouseMoveErase(const CPoint&);
};

#endif
