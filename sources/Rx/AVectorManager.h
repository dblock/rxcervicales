/*
	Rx Cervicales - Projet de Recherche sur la Radiologie Cervicales Fonctionnelle
	Travail de Licence, © Univerité de Genève, 1999, Tous droits reservés
	------------------------------------------------------------------------------
	écrit par Daniel Doubrovkine (doubrov5@cuimail.unige.ch / dblock@vestris.com)
	http://cuiwww.unige.ch/~doubrov5/uni/license
	------------------------------------------------------------------------------
	Ce code est du domaine public. Il est distribué sans aucune garantie.
	*/

#ifndef C_AVECTOR_MANAGER_H
#define C_AVECTOR_MANAGER_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AVector.h"
#include "defuns.h"
#include "ObjectManager.h"
class CRxView;

class CAVectorManager : public CObjectManager {
private:
	BOOL m_ShowAngles;
	CList<CAVector, CAVector&> m_VList;	
	CAVector * m_SelVector;	
public:	
	CAVector * GetTail(void);
	BOOL EndMove(void);
	CAVectorManager();
	virtual ~CAVectorManager();
	void Add(const CAVector&);	
	void RemoveAll(void);
	void Serialize(CArchive&);
	void Paint(CDC *, const CRedimension& Redimension);
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
	BOOL SelectFirstElement(void);
	BOOL SelectNextElement(void);
	BOOL SelectLastElement(void);
	BOOL SelectPrevElement(void);
	BOOL Rotate(const CPoint& APoint);
	BOOL RotateSelect(const CPoint& APoint);
	BOOL SetShowAngles(BOOL Value = TRUE);
	BOOL GetShowAngles(void) const;
	CAVector * GetSelected();
	PROPERTY(double, Metric);
};

#endif
