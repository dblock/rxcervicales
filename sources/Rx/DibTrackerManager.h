/*
	Rx Cervicales - Projet de Recherche sur la Radiologie Cervicales Fonctionnelle
	Travail de Licence, © Univerité de Genève, 1999, Tous droits reservés
	------------------------------------------------------------------------------
	écrit par Daniel Doubrovkine (doubrov5@cuimail.unige.ch / dblock@vestris.com)
	http://cuiwww.unige.ch/~doubrov5/uni/license
	------------------------------------------------------------------------------
	Ce code est du domaine public. Il est distribué sans aucune garantie.
	*/

#if !defined(C_DIB_TRACKER_MANAGER_H)
#define C_DIB_TRACKER_MANAGER_H

#include <afxtempl.h>

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DibTracker.h"
#include "ObjectManager.h"
#include "defuns.h"

class CDibTrackerManager : public CObjectManager {
private:
	BOOL m_Validate;
	BOOL m_ShowContents;
	CList<CDibTracker, CDibTracker&> m_DibTrackList;
	CDibTracker * m_SelectedDIB;
public:	
	BOOL GetShowContents(void) const;
	BOOL SetShowContents(BOOL Value = TRUE);
	BOOL EditOSize(void);
	BOOL EditOZero(void);
	COleDataSource* SaveDib(int Index);
	COleDataSource* SaveDibSelected(void);
	void RemoveAt(int);
	BOOL DeleteSelected(void);
	BOOL Deselect(void);
	BOOL SelectAt(int Index);
	void Serialize(CArchive&);
	void RemoveAll(void);
	void Paint(CDC *, const CRedimension&);	
	CRect GetBoundMove(void) const;
	void Add(const CDib&);	
	CDib * GetAt(int Index);
	CDib * GetSelected();
	CDibTrackerManager();
	virtual ~CDibTrackerManager();
	BOOL Select(const CPoint& point);
	BOOL Select(int Index);
	int GetCount(void);
	void ViewPaletteChanged(CClientDC& dc, UINT wParam);
	void UsePalette(CDC *, BOOL bBackground = FALSE);
	void SetSystemPalette(CDC *);
	void CopyDibTo(int Index);
	void CopyDibSelectedTo(void);
	BOOL EditPastefrom(void);
	BOOL EditPaste(void);
	BOOL MouseMoveErase(const CPoint&);
	BOOL MouseMove(const CPoint&);
	BOOL OnMouseMove(const CPoint& APoint);
	BOOL SelectFirstElement(void);
	BOOL SelectNextElement(void);
	BOOL SelectLastElement(void);
	BOOL SelectPrevElement(void);
	BOOL Rotate(const CPoint& APoint);
	BOOL RotateSelect(const CPoint& APoint);	
	void EndMove(void);
	BOOL OnTimer(UINT nIDEvent);
};

#endif
