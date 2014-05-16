/*
	Rx Cervicales - Projet de Recherche sur la Radiologie Cervicales Fonctionnelle
	Travail de Licence, © Univerité de Genève, 1999, Tous droits reservés
	------------------------------------------------------------------------------
	écrit par Daniel Doubrovkine (doubrov5@cuimail.unige.ch / dblock@vestris.com)
	http://cuiwww.unige.ch/~doubrov5/uni/license
	------------------------------------------------------------------------------
	Ce code est du domaine public. Il est distribué sans aucune garantie.
	*/

#ifndef C_ANNOTATION_MANAGER_H
#define C_ANNOTATION_MANAGER_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Annotation.h"
#include "ObjectManager.h"

class CAnnotationManager : public CObjectManager {
private:
	CList<CAnnotation, CAnnotation&> m_VList;	
	CAnnotation * m_SelVector;	
	void Cleanup(void);
public:
	BOOL SetFontFromDialog(const CFontDialog&);	
	CAnnotationManager();
	virtual ~CAnnotationManager();
	void Add(const CAnnotation&);	
	void RemoveAll(void);
	void Serialize(CArchive&);
	void Paint(CDC *, const CRedimension&);
	BOOL DeleteSelected(void);
	void RemoveAt(int Index);
	BOOL CanSelect(const CPoint&) const;
	BOOL Select(const CPoint&);
	BOOL MouseMove(const CPoint&);
	BOOL OnMouseMove(const CPoint&);
	CRect GetBoundMove(void) const;
	BOOL SetSelectedColor(COLORREF);	
	BOOL Deselect(void);
	int GetCount(void) const;	
	CAnnotation * GetSelected(void) const;
	BOOL EndMove(void);
	void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	BOOL SelectFirstElement(void);
	BOOL SelectNextElement(void);
	BOOL SelectLastElement(void);
	BOOL SelectPrevElement(void);
};	

#endif
