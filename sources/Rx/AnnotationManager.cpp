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
#include "Rx.h"
#include "AnnotationManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CAnnotationManager::CAnnotationManager(){
	CObjectManager::CObjectManager();
	m_SelVector = NULL;	
}

CAnnotationManager::~CAnnotationManager(){

}

void CAnnotationManager::Add(const CAnnotation& AVector){
	if (m_Selected != -1) m_SelVector->Select(FALSE);	
	m_VList.AddTail((CAnnotation&) AVector);
	m_Selected = m_VList.GetCount()-1;
	m_SelVector = &m_VList.GetAt(m_VList.FindIndex(m_Selected));
	m_SelVector->Select(TRUE);		
}

void CAnnotationManager::RemoveAll(void){
	m_VList.RemoveAll();
}

void CAnnotationManager::RemoveAt(int Index){
	m_VList.RemoveAt(m_VList.FindIndex(Index));
}

BOOL CAnnotationManager::DeleteSelected(void){
	if (m_Selected != -1) {		
		m_VList.RemoveAt(m_VList.FindIndex(m_Selected));		
		if (m_VList.GetCount()) {
			m_Selected = 0;
			m_SelVector = &m_VList.GetAt(m_VList.FindIndex(m_Selected));
			m_SelVector->Select(TRUE);
			return TRUE;	
		} else {
			m_Selected = -1;
			m_SelVector = 0;
			Cleanup();
			return TRUE;
		}
	} else return FALSE;
}

void CAnnotationManager::Serialize(CArchive& ar){
	int i;
	if (ar.IsStoring()){		
		ar << m_VList.GetCount();
		POSITION i_Pos = m_VList.GetHeadPosition();
		while (i_Pos != NULL) m_VList.GetNext(i_Pos).Serialize(ar);		
	} else {		
		m_Selected = -1;
		m_SelVector = 0;
		m_Color = (COLORREF) 0;
		m_VList.RemoveAll();
		
		int Size;
		ar >> Size;
		for (i=0;i<Size;i++) {
			CAnnotation newVector;
			newVector.Serialize(ar);			
			Add(newVector);
			Deselect();
		}		
	}	
}

void CAnnotationManager::Paint(CDC * memDC, const CRedimension& Redimension){		
	CAnnotation * curVector = NULL;
	POSITION i_Pos = m_VList.GetHeadPosition();
	while (i_Pos != NULL) m_VList.GetNext(i_Pos).Paint(memDC, Redimension);	
}

BOOL CAnnotationManager::CanSelect(const CPoint& APoint) const {	
	POSITION i_Pos = m_VList.GetHeadPosition();
	while (i_Pos != NULL) {		
		if (m_VList.GetNext(i_Pos).CanSelect(APoint)) return TRUE;		
	}
	return FALSE;
}

BOOL CAnnotationManager::Select(const CPoint& APoint){
	if ((m_Selected != -1)&&(m_SelVector->Select(APoint))) return TRUE;
	BOOL Result = FALSE;
	m_Selected = -1;
	m_SelVector = 0;	
	Cleanup();
	CAnnotation * curVector;
	for (int i=m_VList.GetCount()-1;i>=0;i--){
		curVector = &m_VList.GetAt(m_VList.FindIndex(i));
		if (!Result){			
			if (curVector->Select(APoint)){
				m_Selected = i;
				m_SelVector = curVector;
				Result = TRUE;
			}
		} else curVector->Select(FALSE);
	}
	return Result;	
}

BOOL CAnnotationManager::MouseMove(const CPoint& APoint){
	if (m_Selected!=-1) {		
		if (m_SelVector->MouseMove(APoint)){
			m_Cursor = m_SelVector->m_Cursor;
			return TRUE;
		} else return FALSE;
	} else return FALSE;
}

BOOL CAnnotationManager::OnMouseMove(const CPoint& Point){
	POSITION i_Pos = m_VList.GetHeadPosition();
	while (i_Pos != NULL) {		
		CAnnotation& curElt = m_VList.GetNext(i_Pos);
		if (curElt.OnMouseMove(Point)) {			
			m_Cursor = curElt.m_Cursor;
			return TRUE;
		}
	}
	return FALSE;
}

CRect CAnnotationManager::GetBoundMove(void) const {
	CRect aRect;
	if (m_Selected!=-1) aRect = m_SelVector->Bound();
	return aRect;
}

BOOL CAnnotationManager::SetSelectedColor(COLORREF newColor){
	if (m_Selected!=-1) return m_SelVector->SetColor(newColor);
	else return FALSE;
}

int CAnnotationManager::GetCount(void) const {
	return m_VList.GetCount();
}

BOOL CAnnotationManager::Deselect(void){
	if (m_Selected!=-1) {
		m_SelVector->Select(FALSE);
		m_Selected = -1;
		m_SelVector = 0;
		Cleanup();
		return TRUE;
	} else return FALSE;
}

BOOL CAnnotationManager::EndMove(void){
	::SetCursor(LoadCursor(0, IDC_ARROW));
	if (m_Selected!=-1) return m_SelVector->EndMove();		
	else return FALSE;
}

void CAnnotationManager::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags){
	if (m_Selected != -1) {
		m_SelVector->OnKeyDown(nChar, nRepCnt, nFlags);
	}
}

void CAnnotationManager::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags){
	if (m_Selected != -1) {
		m_SelVector->OnChar(nChar, nRepCnt, nFlags);
	}
}

BOOL CAnnotationManager::SetFontFromDialog(const CFontDialog& FontDialog){
	if (m_Selected != -1) {
		m_SelVector->SetFontFromDialog(FontDialog);
		return TRUE;
	} else return FALSE;
}

CAnnotation * CAnnotationManager::GetSelected(void) const {
	if (m_Selected != -1)
		return m_SelVector;
	else return NULL;
}

void CAnnotationManager::Cleanup(void) {
	if (m_Selected == -1) {
		CAnnotation * curElt;
		for (int i=m_VList.GetCount()-1;i>=0;i--){	
			curElt = &m_VList.GetAt(m_VList.FindIndex(i));
			if (!curElt->GetTextLength()) {
				m_VList.RemoveAt(m_VList.FindIndex(i));
			}
		}
	}
}

BOOL CAnnotationManager::SelectFirstElement(void){
	if (m_Selected != -1) m_SelVector->Select(FALSE);
	if (m_VList.GetCount()) {		
		m_Selected = 0;
		m_SelVector = &m_VList.GetAt(m_VList.FindIndex(m_Selected));
		m_SelVector->Select(TRUE);
		return TRUE;
	} else {
		m_Selected = -1;
		m_SelVector = NULL;
		return FALSE;
	}
}

BOOL CAnnotationManager::SelectNextElement(void){
	if (m_Selected != -1) m_SelVector->Select(FALSE);
	if (m_Selected + 1 < m_VList.GetCount()) {		
		m_Selected++;
		m_SelVector = &m_VList.GetAt(m_VList.FindIndex(m_Selected));
		m_SelVector->Select(TRUE);
		return TRUE;
	} else {
		m_Selected = -1;
		m_SelVector = NULL;
		return FALSE;
	}
}

BOOL CAnnotationManager::SelectPrevElement(void){
	if (m_Selected != -1) m_SelVector->Select(FALSE);
	if (m_Selected > 0) {		
		m_Selected--;
		m_SelVector = &m_VList.GetAt(m_VList.FindIndex(m_Selected));
		m_SelVector->Select(TRUE);
		return TRUE;
	} else {
		m_Selected = -1;
		m_SelVector = NULL;
		return FALSE;
	}
}

BOOL CAnnotationManager::SelectLastElement(void){
	if (m_Selected != -1) m_SelVector->Select(FALSE);
	if (m_VList.GetCount()) {	
		m_Selected = m_VList.GetCount()-1;
		m_SelVector = &m_VList.GetAt(m_VList.FindIndex(m_Selected));
		m_SelVector->Select(TRUE);
		return TRUE;
	} else {
		m_Selected = -1;
		m_SelVector = NULL;
		return FALSE;
	}
}