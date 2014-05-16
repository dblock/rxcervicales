/*
	Rx Cervicales - Projet de Recherche sur la Radiologie Cervicales Fonctionnelle
	Travail de Licence, © Univerité de Genève, 1999, Tous droits reservés
	------------------------------------------------------------------------------
	écrit par Daniel Doubrovkine (doubrov5@cuimail.unige.ch / dblock@vestris.com)
	http://cuiwww.unige.ch/~doubrov5/uni/license
	------------------------------------------------------------------------------
	Ce code est du domaine public. Il est distribué sans aucune garantie.
	*/

#ifndef C_ANNOTATION_H
#define C_ANNOTATION_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "defuns.h"

#include <afxtempl.h>
#include "VectorObject.h"

class CAnnotation : public CVectorObject {
	PROPERTY(COLORREF, Color);
	PROPERTY(CString, Text);
	PROPERTY(int, FontSize);
	PROPERTY(CString, FontName);
	PROPERTY(BOOL, Bold);
	PROPERTY(BOOL, Italic);
	PROPERTY(BOOL, Underline);
	PROPERTY(BOOL, StrikeOut);	
private:
	void InitFont(void);		
	BOOL m_Editing;
	CPoint m_TopLeft;		
	CSize m_TextSize;		
	int m_EditPosition;
	void CreateFont(CDC * pDC = NULL, CRedimension * = NULL);
public:
	int GetTextLength(void) const;
	void SetFontFromDialog(const CFontDialog&);
	char * m_Cursor;
	CAnnotation();
	CAnnotation(const CString&);
	CAnnotation(const CAnnotation&);
	CPoint GetPosition(void) const;
	BOOL SetPosition(CPoint);	
	BOOL SetEditing(BOOL);	
	inline BOOL isEditing(void) const;
	virtual ~CAnnotation();
	CAnnotation& operator=(const CAnnotation&);
	BOOL Intersects(const CPoint& APoint);
	BOOL EndMove(void);
	BOOL OnMouseMove(const CPoint& APoint);
	void Serialize(CArchive&);
	BOOL MouseMove(const CPoint&);	
	BOOL Select(const CPoint&);		
	BOOL Select(BOOL Value);
	BOOL CanSelect(const CPoint&);
	BOOL Delete(void);		
	void Paint(CDC * pDC, const CRedimension&);
	CRect Bound();
	CRect CalcBound();		
	void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
};

#endif 
