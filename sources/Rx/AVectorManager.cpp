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
#include "AVectorManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CAVectorManager::CAVectorManager(){
	CObjectManager::CObjectManager();
	m_Color = (COLORREF) 0;
	m_Selected = -1;
	m_SelVector = NULL;	
	m_ShowAngles = TRUE;
	m_Metric = 1;
}

CAVectorManager::~CAVectorManager(){

}

void CAVectorManager::Add(const CAVector& AVector){	
	((CAVector&) AVector).SetParent(this);
	m_VList.AddTail((CAVector&) AVector);		
}

CAVector * CAVectorManager::GetTail(void) {
	return &m_VList.GetTail();
}

void CAVectorManager::RemoveAll(void){
	m_VList.RemoveAll();
}

void CAVectorManager::RemoveAt(int Index){
	m_VList.RemoveAt(m_VList.FindIndex(Index));
}

BOOL CAVectorManager::DeleteSelected(void){
	if (m_Selected != -1) {		
		m_VList.RemoveAt(m_VList.FindIndex(m_Selected));		
		if (m_VList.GetCount()) {
			m_Selected = 0;
			m_SelVector = &m_VList.GetAt(m_VList.FindIndex(m_Selected));
			m_SelVector->CVectorObject::Select(TRUE);
			return TRUE;	
		} else {
			m_Selected = -1;
			m_SelVector = 0;
			return TRUE;
		}
	} else return FALSE;
}

void CAVectorManager::Serialize(CArchive& ar){
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
			CAVector newVector;
			newVector.Serialize(ar);
			Add(newVector);
			Deselect();
		}		
	}	
}

void CAVectorManager::Paint(CDC * memDC, const CRedimension& Redimension){		
	POSITION i_Pos = m_VList.GetHeadPosition();
	while (i_Pos != NULL) {		
		CAVector& curVector = m_VList.GetNext(i_Pos);
		curVector.Paint(memDC, Redimension, m_ShowAngles);
		/*
		if (m_Rotating && (curVector == m_SelVector)) {		
			CPoint m_Center = curVector->GetCenter();
			memDC->LPtoDP(&m_Center); 
			memDC->MoveTo(m_Center.x - 3, m_Center.y - 3);
			memDC->LineTo(m_Center.x + 3, m_Center.y + 3);
			memDC->MoveTo(m_Center.x - 3, m_Center.y + 3);
			memDC->LineTo(m_Center.x + 3, m_Center.y - 3);		
		}
		*/
		/*
		memDC->SelectStockObject(GRAY_BRUSH);
		memDC->FrameRect(curVector->Bound(), memDC->GetCurrentBrush());
		*/				
		if (m_ShowAngles && i_Pos){
			POSITION s_Pos = i_Pos;
			while (s_Pos != NULL) curVector.PaintIntersection(memDC, Redimension, m_VList.GetNext(s_Pos));
		}		
	}	
}

BOOL CAVectorManager::CanSelect(const CPoint& APoint) const {
	POSITION i_Pos = m_VList.GetHeadPosition();
	while (i_Pos != NULL) {		
		if (m_VList.GetNext(i_Pos).CanSelect(APoint)) return TRUE;		
	}
	return FALSE;
}

BOOL CAVectorManager::Select(const CPoint& APoint){
	BOOL Result = FALSE;
	m_Selected = -1;
	m_SelVector = 0;	
	CAVector * curVector;
	for (int i=m_VList.GetCount()-1;i>=0;i--){
		curVector = &m_VList.GetAt(m_VList.FindIndex(i));
		if (!Result){			
			if (curVector->Select(APoint)){
				m_Selected = i;
				m_SelVector = curVector;
				Result = TRUE;
			}
		} else curVector->CVectorObject::Select(FALSE);
	}	
	return Result;	
}

BOOL CAVectorManager::MouseMove(const CPoint& APoint){
	if (m_Selected!=-1) {		
		if (m_SelVector->MouseMove(APoint)){
			m_Cursor = m_SelVector->m_Cursor;
			return TRUE;
		} else return FALSE;
	} else return FALSE;
}

BOOL CAVectorManager::OnMouseMove(const CPoint& Point){
	POSITION i_Pos = m_VList.GetHeadPosition();
	while (i_Pos != NULL) {		
		CAVector& curElt = m_VList.GetNext(i_Pos);
		if (curElt.OnMouseMove(Point)) {			
			m_Cursor = curElt.m_Cursor;
			return TRUE;
		}
	}	
	return FALSE;
}

CRect CAVectorManager::GetBoundMove(void) const {
	CRect aRect;
	if (m_Selected!=-1) aRect = m_SelVector->Bound();
	return aRect;
}

BOOL CAVectorManager::SetSelectedColor(COLORREF newColor){
	if (m_Selected!=-1) return m_SelVector->SetColor(newColor);
	else return FALSE;
}

int CAVectorManager::GetCount(void) const {
	return m_VList.GetCount();
}

BOOL CAVectorManager::Deselect(void){
	if (m_Selected!=-1) {
		m_SelVector->CVectorObject::Select(FALSE);
		m_Selected = -1;
		m_SelVector = 0;
		return TRUE;
	} else return FALSE;
}

BOOL CAVectorManager::EndMove(void){
	::SetCursor(LoadCursor(0, IDC_ARROW));
	if (m_Selected!=-1) return m_SelVector->EndMove();		
	else return FALSE;
}

BOOL CAVectorManager::SelectFirstElement(void){	
	if (m_Selected != -1) m_SelVector->CVectorObject::Select(FALSE);
	if (m_VList.GetCount()) {		
		m_Selected = 0;
		m_SelVector = &m_VList.GetAt(m_VList.FindIndex(m_Selected));
		m_SelVector->CVectorObject::Select(TRUE);
		return TRUE;
	} else {
		m_Selected = -1;
		m_SelVector = NULL;
		return FALSE;
	}
}

BOOL CAVectorManager::SelectNextElement(void){
	if (m_Selected != -1) m_SelVector->CVectorObject::Select(FALSE);
	if (m_Selected + 1 < m_VList.GetCount()) {		
		m_Selected++;
		m_SelVector = &m_VList.GetAt(m_VList.FindIndex(m_Selected));
		m_SelVector->CVectorObject::Select(TRUE);
		return TRUE;
	} else {
		m_Selected = -1;
		m_SelVector = NULL;
		return FALSE;
	}
}

BOOL CAVectorManager::SelectPrevElement(void){
	if (m_Selected != -1) m_SelVector->CVectorObject::Select(FALSE);
	if (m_Selected > 0) {		
		m_Selected--;
		m_SelVector = &m_VList.GetAt(m_VList.FindIndex(m_Selected));
		m_SelVector->CVectorObject::Select(TRUE);
		return TRUE;
	} else {
		m_Selected = -1;
		m_SelVector = NULL;
		return FALSE;
	}
}

BOOL CAVectorManager::SelectLastElement(void){
	if (m_Selected != -1) m_SelVector->CVectorObject::Select(FALSE);
	if (m_VList.GetCount()) {		
		m_Selected = m_VList.GetCount()-1;
		m_SelVector = &m_VList.GetAt(m_VList.FindIndex(m_Selected));
		m_SelVector->CVectorObject::Select(TRUE);
		return TRUE;
	} else {
		m_Selected = -1;
		m_SelVector = NULL;
		return FALSE;
	}
}

BOOL CAVectorManager::Rotate(const CPoint& APoint){
	if ((m_Selected != -1)&&(m_Rotating)) {
		return m_SelVector->Rotate(APoint);
	} else return FALSE;
}

BOOL CAVectorManager::RotateSelect(const CPoint& APoint){
	if (m_Selected != -1) {
		SetRotating();
		return m_SelVector->RotateSelect(APoint);
	} else return FALSE;
}

BOOL CAVectorManager::SetShowAngles(BOOL Value) {
	if (m_ShowAngles != Value) {
		m_ShowAngles = Value;
		return TRUE;
	} else return FALSE;
}

BOOL CAVectorManager::GetShowAngles(void) const {
	return m_ShowAngles;
}

CAVector * CAVectorManager::GetSelected() {
	if (m_Selected != -1) {
		return m_SelVector;
	} else return NULL;
}