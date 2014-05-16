/*
	Rx Cervicales - Projet de Recherche sur la Radiologie Cervicales Fonctionnelle
	Travail de Licence, © Univerité de Genève, 1999, Tous droits reservés
	------------------------------------------------------------------------------
	écrit par Daniel Doubrovkine (doubrov5@cuimail.unige.ch / dblock@vestris.com)
	http://cuiwww.unige.ch/~doubrov5/uni/license
	------------------------------------------------------------------------------
	Ce code est du domaine public. Il est distribué sans aucune garantie.
	*/

#ifndef C_VECTOROBJECT_H
#define C_VECTOROBJECT_H

#include "defuns.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CVectorObject {
protected:
	CFont m_Font;
	CRect m_Bound;
	BOOL m_Selected;
	COLORREF m_Color;
	CPoint m_SelPoint;		
public:
	CVectorObject();
	CVectorObject(const CVectorObject&);
	virtual CVectorObject& operator=(const CVectorObject&);
	virtual ~CVectorObject() = 0;
	virtual BOOL GetSelected(void) const;
	virtual BOOL Select(BOOL Value = TRUE);	
	virtual CRect Bound();	
	virtual void Paint(CDC * pDC, const CRedimension&) = 0;	
};

#endif
