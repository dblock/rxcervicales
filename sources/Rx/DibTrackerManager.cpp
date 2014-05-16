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
#include "DibTrackerManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


CDibTrackerManager::CDibTrackerManager(){
	CObjectManager::CObjectManager();
	m_Selected = -1;
	m_SelectedDIB = 0;
	m_Rotating = FALSE;
	m_ShowContents = FALSE;
	m_Validate = FALSE;
}

CDibTrackerManager::~CDibTrackerManager(){

}

void CDibTrackerManager::Add(const CDib& ADib){
	CDibTracker newTracker(ADib);	
	m_DibTrackList.AddTail(newTracker);
	//SelectAt(m_DibTrackList.GetCount() - 1);
	SelectLastElement();
}

CDib * CDibTrackerManager::GetAt(int Index){
	return m_DibTrackList.GetAt(m_DibTrackList.FindIndex(Index)).GetDib();
}

CDib * CDibTrackerManager::GetSelected(void){
	if (m_Selected != -1){
		//return GetAt(m_Selected);
		return m_SelectedDIB->GetDib();
	} else return NULL;
}

void CDibTrackerManager::Paint(CDC * memDC, const CRedimension& Redimension){	
	BOOL ShowContents;
	if (m_Validate || memDC->IsPrinting()) {
		ShowContents = TRUE;
		m_Validate = FALSE;
	} else {
		ShowContents = (m_Rotating?m_ShowContents:1);		
		if (ShowContents && !m_ShowContents)
			if ((m_Selected != -1) && m_SelectedDIB->GetRotating())
				ShowContents = 0;
	}
	
	int i=0;
	POSITION i_Pos = m_DibTrackList.GetHeadPosition();
	while (i_Pos != NULL) {		
		m_DibTrackList.GetNext(i_Pos).Paint(memDC, Redimension, (memDC, ShowContents || (i!=m_Selected)));
		i++;
	}	
}

void CDibTrackerManager::RemoveAll(void){
	m_Selected = -1;
	m_SelectedDIB = NULL;
	m_DibTrackList.RemoveAll();	
}

void CDibTrackerManager::Serialize(CArchive& ar){
	try {
		int i;
		if (ar.IsStoring()){
			ar << m_DibTrackList.GetCount();
			for (int i=m_DibTrackList.GetCount()-1;i>=0;i--){
				m_DibTrackList.GetAt(m_DibTrackList.FindIndex(i)).Serialize(ar);			
			}
		} else {
			int nSize; ar >> nSize;
			for (i=0;i<nSize;i++){
				CDibTracker newTracker;
				newTracker.Serialize(ar);
				m_DibTrackList.AddTail(newTracker);
				Deselect();
			}
		}
	} catch (...) {
		MessageBox(::GetFocus(), "Serialization Error (DIBTrackerManager)", "Rx Error", MB_OK);
	}
}

BOOL CDibTrackerManager::Select(const CPoint& point){		
	if ((m_Selected != -1)&&(m_SelectedDIB->Select(point))) return TRUE;	
	BOOL Result = FALSE;
	m_Selected = -1;
	m_SelectedDIB = 0;
	CDibTracker * curDibTracker;
	for (int i=m_DibTrackList.GetCount()-1;i>=0;i--){
		curDibTracker = &m_DibTrackList.GetAt(m_DibTrackList.FindIndex(i));
		if (!Result){			
			if (curDibTracker->Select(point)){
				Result = TRUE;
				m_Selected = i;				
				m_SelectedDIB = curDibTracker;
			}
		} else curDibTracker->CVectorObject::Select(FALSE);
	}	
	return Result;
}

BOOL CDibTrackerManager::SelectAt(int Index) {
	ASSERT((Index >= 0) && (Index < m_DibTrackList.GetCount()));
	if (m_Selected == Index) return TRUE;
	if (m_Selected != -1) m_SelectedDIB->CVectorObject::Select(FALSE);			
	m_SelectedDIB = &m_DibTrackList.GetAt(m_DibTrackList.FindIndex(Index));
	m_SelectedDIB->CVectorObject::Select(TRUE);
	m_Selected = Index;	
	return TRUE;
}

BOOL CDibTrackerManager::Select(int Index){		
	return SelectAt(Index);
}


void CDibTrackerManager::RemoveAt(int Index){	
	m_DibTrackList.RemoveAt(m_DibTrackList.FindIndex(Index));
}

BOOL CDibTrackerManager::DeleteSelected(void){
	if (m_Selected != -1){
		RemoveAt(m_Selected);
		if (m_DibTrackList.GetCount()) {
			m_Selected = 0;
			m_SelectedDIB = &m_DibTrackList.GetAt(m_DibTrackList.FindIndex(m_Selected));
			m_SelectedDIB->CVectorObject::Select(TRUE);
		} else {
			m_Selected = -1;
			m_SelectedDIB = 0;
		}
		return TRUE;
	} else return FALSE;	
}

int CDibTrackerManager::GetCount(void){
	return m_DibTrackList.GetCount();
}

void CDibTrackerManager::ViewPaletteChanged(CClientDC& dc, UINT wParam){
	for (int i=m_DibTrackList.GetCount()-1;i>=0;i--)
		m_DibTrackList.GetAt(m_DibTrackList.FindIndex(i)).ViewPaletteChanged(dc, wParam);
}

void CDibTrackerManager::UsePalette(CDC * dc, BOOL Background){
	for (int i=m_DibTrackList.GetCount()-1;i>=0;i--)
		m_DibTrackList.GetAt(m_DibTrackList.FindIndex(i)).UsePalette(dc, Background);
}

void CDibTrackerManager::SetSystemPalette(CDC * dc){
	for (int i=m_DibTrackList.GetCount()-1;i>=0;i--)
		m_DibTrackList.GetAt(m_DibTrackList.FindIndex(i)).SetSystemPalette(dc);
}

COleDataSource* CDibTrackerManager::SaveDib(int Index){	
	return m_DibTrackList.GetAt(m_DibTrackList.FindIndex(Index)).SaveDib();
}

COleDataSource* CDibTrackerManager::SaveDibSelected(){
	if (m_Selected != -1) return m_SelectedDIB->SaveDib(); else return NULL;
}

void CDibTrackerManager::CopyDibTo(int Index){
	m_DibTrackList.GetAt(m_DibTrackList.FindIndex(Index)).CopyDibTo();
}

void CDibTrackerManager::CopyDibSelectedTo(void){
	if (m_Selected != -1) m_SelectedDIB->CopyDibTo();
}

BOOL CDibTrackerManager::EditPastefrom(void){
	CDibTracker newTracker;
	if (newTracker.EditPastefrom()){			
		m_DibTrackList.AddTail(newTracker);
		//SelectAt((int)(m_DibTrackList.GetCount() - 1));
		SelectLastElement();
		return TRUE;
	} else return FALSE;	
}

BOOL CDibTrackerManager::EditPaste(void){	
	CDibTracker newTracker;
	if (newTracker.EditPaste()){				
		m_DibTrackList.AddTail(newTracker);
		SelectAt(m_DibTrackList.GetCount() - 1);
		return TRUE;
	} else return FALSE;	
}

CRect CDibTrackerManager::GetBoundMove(void) const {
	CRect aRect;
	if (m_Selected!=-1) aRect = m_SelectedDIB->GetRect();
	return aRect;

}

BOOL CDibTrackerManager::MouseMove(const CPoint& Point){
	if (m_Selected!=-1) {		
		return m_SelectedDIB->MouseMove(Point);
	} else return FALSE;
}

BOOL CDibTrackerManager::OnMouseMove(const CPoint& Point){
	if ((m_Selected != -1) && (m_SelectedDIB->OnMouseMove(Point))) {
		m_Cursor = m_SelectedDIB->m_Cursor;
		return TRUE;
	}
	CDibTracker * curTracker;
	for (int i=m_DibTrackList.GetCount()-1;i>=0;i--){
		curTracker = &m_DibTrackList.GetAt(m_DibTrackList.FindIndex(i));
		if (curTracker->OnMouseMove(Point)) {
			m_Cursor = curTracker->m_Cursor;
			return TRUE;	
		}
	}
	return FALSE;
}


BOOL CDibTrackerManager::Deselect(void){
	if (m_Selected!=-1) {
		m_SelectedDIB->CVectorObject::Select(FALSE);
		m_Selected = -1;
		m_SelectedDIB = 0;
		return TRUE;
	} else return FALSE;
}

BOOL CDibTrackerManager::EditOSize(void){
	if (m_Selected!=-1){
		return m_SelectedDIB->SetDibSize();
	} else return FALSE;
}

BOOL CDibTrackerManager::EditOZero(void){
	if (m_Selected!=-1){
		return m_SelectedDIB->SetDibRotation();
	} else return FALSE;
}


BOOL CDibTrackerManager::SelectFirstElement(void){
	if (m_Selected != -1) m_SelectedDIB->CVectorObject::Select(FALSE);
	if (m_DibTrackList.GetCount()) {		
		m_Selected = 0;
		m_SelectedDIB = &m_DibTrackList.GetAt(m_DibTrackList.FindIndex(m_Selected));
		m_SelectedDIB->CVectorObject::Select(TRUE);
		return TRUE;
	} else {
		m_Selected = -1;
		m_SelectedDIB = NULL;
		return FALSE;
	}
}

BOOL CDibTrackerManager::SelectNextElement(void){
	if (m_Selected != -1) m_SelectedDIB->CVectorObject::Select(FALSE);
	if (m_Selected + 1 < m_DibTrackList.GetCount()) {		
		m_Selected++;
		m_SelectedDIB = &m_DibTrackList.GetAt(m_DibTrackList.FindIndex(m_Selected));
		m_SelectedDIB->CVectorObject::Select(TRUE);
		return TRUE;
	} else {
		m_Selected = -1;
		m_SelectedDIB = NULL;
		return FALSE;
	}
}

BOOL CDibTrackerManager::SelectPrevElement(void){
	if (m_Selected != -1) m_SelectedDIB->CVectorObject::Select(FALSE);
	if (m_Selected > 0) {		
		m_Selected--;
		m_SelectedDIB = &m_DibTrackList.GetAt(m_DibTrackList.FindIndex(m_Selected));
		m_SelectedDIB->CVectorObject::Select(TRUE);
		return TRUE;
	} else {
		m_Selected = -1;
		m_SelectedDIB = NULL;
		return FALSE;
	}
}

BOOL CDibTrackerManager::SelectLastElement(void){
	if (m_Selected != -1) m_SelectedDIB->CVectorObject::Select(FALSE);
	if (m_DibTrackList.GetCount()) {		
		m_Selected = m_DibTrackList.GetCount()-1;
		m_SelectedDIB = &m_DibTrackList.GetAt(m_DibTrackList.FindIndex(m_Selected));
		m_SelectedDIB->CVectorObject::Select(TRUE);
		return TRUE;
	} else {
		m_Selected = -1;
		m_SelectedDIB = NULL;
		return FALSE;
	}
}

BOOL CDibTrackerManager::Rotate(const CPoint& APoint){
	if ((m_Selected != -1)&&(m_Rotating)) {
		return m_SelectedDIB->Rotate(APoint);
	} else return FALSE;
}

BOOL CDibTrackerManager::RotateSelect(const CPoint& APoint){
	if (m_Selected != -1) {
		SetRotating();
		return m_SelectedDIB->RotateSelect(APoint);
	} else return FALSE;
}

BOOL CDibTrackerManager::GetShowContents(void) const {	
	return m_ShowContents;
}

BOOL CDibTrackerManager::SetShowContents(BOOL Value) {
	if (m_ShowContents != Value) {
		m_ShowContents = Value;
		return TRUE;
	} else return FALSE;
}

void CDibTrackerManager::EndMove(void) {
	if (m_Selected != -1) m_SelectedDIB->EndMove();	
}

BOOL CDibTrackerManager::OnTimer(UINT nIDEvent) {
	BOOL ShowContents = (m_Rotating?m_ShowContents:1);
	if (ShowContents && !m_ShowContents)
		if ((m_Selected != -1) && m_SelectedDIB->GetRotatingChanged())
			ShowContents = 0;
	m_Validate = !ShowContents;
	return m_Validate;
}


BOOL CDibTrackerManager::MouseMoveErase(const CPoint& APoint) {
	if (m_Selected != -1) {
		return m_SelectedDIB->MouseMoveErase(APoint);
	} else return FALSE;
}