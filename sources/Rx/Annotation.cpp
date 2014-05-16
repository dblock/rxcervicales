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
#include "Annotation.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CAnnotation::CAnnotation() {
	CVectorObject::CVectorObject();
	m_TopLeft = CPoint(0,0);
	m_Editing = FALSE;
	InitFont();
	CreateFont();
	m_Bound = CalcBound();
}

CAnnotation::~CAnnotation(){

}

CAnnotation::CAnnotation(const CString& iStr) {	
	CVectorObject::CVectorObject();
	m_TopLeft = CPoint(0,0);
	m_Editing = FALSE;
	m_Text = iStr;	
	InitFont();
	CreateFont();
	m_Bound = CalcBound();
}

CAnnotation::CAnnotation(const CAnnotation& Rhs) {
	(* this)=Rhs;
}

BOOL CAnnotation::SetPosition(CPoint Pos){
	if (m_TopLeft != Pos) {
		m_TopLeft = Pos;
		m_Bound = CalcBound();
		return TRUE;
	} else return FALSE;
}

CPoint CAnnotation::GetPosition(void) const {
	return m_TopLeft;
}

CAnnotation& CAnnotation::operator=(const CAnnotation& Rhs){
	CVectorObject::operator=(Rhs);

	m_Editing = Rhs.m_Editing;
	m_TopLeft = Rhs.m_TopLeft;	
	m_Text = Rhs.m_Text;
	m_EditPosition = Rhs.m_EditPosition;
	
	m_FontSize = Rhs.m_FontSize;
	m_FontName = Rhs.m_FontName;
	m_Bold = Rhs.m_Bold;
	m_Italic = Rhs.m_Italic;
	m_Underline = Rhs.m_Underline;
	m_StrikeOut = Rhs.m_StrikeOut;

	CreateFont();
	m_Bound = CalcBound();

	return (* this);
}

BOOL CAnnotation::isEditing(void) const {
	return m_Editing;
}

BOOL CAnnotation::SetEditing(BOOL Value) {
	if (Value != m_Editing) {
		m_Editing = Value;
		m_EditPosition = m_Text.GetLength();
		return TRUE;
	} else return FALSE;
}

CRect CAnnotation::CalcBound() {
	//return CRect(m_TopLeft, CSize((m_Text.GetLength()+1)*m_TextMetrics.tmAveCharWidth, m_TextMetrics.tmHeight));
	return CRect(m_TopLeft, m_TextSize + CSize(5,0));
}

CRect CAnnotation::Bound(){
	CRect m_BoundInflate = CVectorObject::Bound();
	m_BoundInflate.InflateRect(CSize(25,0));
	return m_BoundInflate;
}

void CAnnotation::Paint(CDC * pDC, const CRedimension& Redimension){	
	CreateFont(pDC, (CRedimension *) &Redimension);	
	CPoint m_CurrentTopLeft = m_TopLeft;
	pDC->LPtoDP(&m_CurrentTopLeft);
	pDC->SelectStockObject(GRAY_BRUSH);
	pDC->SetTextColor(m_Color);
	pDC->SetBkMode(TRANSPARENT);
	CFont * oldFont = pDC->SelectObject(&m_Font);
	if (m_Text.GetLength()) m_TextSize = pDC->GetTextExtent(m_Text, m_Text.GetLength());
	else {
		m_TextSize = pDC->GetTextExtent("W", 1);
		m_TextSize.cx = 0;		
	}	
	pDC->LPtoDP(&m_TextSize);	
	m_Bound = CalcBound();	
	CRect cur_Pos = CRect(m_CurrentTopLeft, m_TextSize); Redimension.TranslateRect(cur_Pos);
	CRect cur_Bound = m_Bound; Redimension.TranslateRect(cur_Bound);
	pDC->DrawText(m_Text, cur_Pos, DT_NOCLIP | DT_LEFT);
	if (!pDC->IsPrinting()) {
		if (m_Selected) {
			pDC->FrameRect(cur_Bound, pDC->GetCurrentBrush());
		}
		if (m_Editing) {		
			static CBrush CursorBrush((COLORREF) 255);
			CSize LeftSize = pDC->GetTextExtent(m_Text.Mid(0, m_EditPosition));
			pDC->FillRect(
				CRect(
				cur_Bound.left + LeftSize.cx,			
				cur_Bound.top, 
				cur_Bound.left + LeftSize.cx+2,			
				cur_Bound.bottom),
				&CursorBrush);
		}
	}
	pDC->SelectObject(oldFont);
}

BOOL CAnnotation::Intersects(const CPoint& APoint) {		
	return m_Bound.PtInRect(APoint);
}

BOOL CAnnotation::Select(BOOL Value){
	if (CVectorObject::Select(Value)) {
		m_Editing = m_Selected;
		m_EditPosition = m_Text.GetLength();
		return TRUE;
	} else return FALSE;
}

BOOL CAnnotation::Select(const CPoint& APoint){
	m_Selected = Intersects(APoint);
	m_Editing = m_Selected;
	m_EditPosition = m_Text.GetLength();
	if (m_Selected) m_SelPoint = APoint;
	return m_Selected;
}

BOOL CAnnotation::CanSelect(const CPoint& APoint){
	return Intersects(APoint);	
}

BOOL CAnnotation::OnMouseMove(const CPoint& APoint){
	if (CanSelect(APoint)) {
		m_Cursor = IDC_UPARROW;
		return TRUE;
	} else return FALSE; 	
}

BOOL CAnnotation::MouseMove(const CPoint& APoint){		
	m_TopLeft.x += (APoint.x - m_SelPoint.x);
	m_TopLeft.y += (APoint.y - m_SelPoint.y);			
	m_SelPoint = APoint;
	m_Bound = CalcBound();
	return TRUE;
}

BOOL CAnnotation::Delete(void){
	return FALSE;
}

void CAnnotation::Serialize(CArchive& ar){	
	if (ar.IsStoring()){
		ar << m_Text << m_TopLeft << m_FontSize << m_FontName << m_Bold << m_Italic << m_Underline << m_StrikeOut << m_Color;
	} else {
		ar >> m_Text;
		ar >> m_TopLeft;
		ar >> m_FontSize;
		ar >> m_FontName;
		ar >> m_Bold;
		ar >> m_Italic;
		ar >> m_Underline;
		ar >> m_StrikeOut;		
		ar >> m_Color;
		CreateFont();
		m_Bound = CalcBound();
	}
}

BOOL CAnnotation::EndMove(void){
	m_Bound = CalcBound();
	return TRUE;
}

void CAnnotation::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags){	
	if (nChar >= ' ') {
		m_Text.Insert(m_EditPosition, (char) nChar);			
		m_EditPosition++;
	} else OnKeyDown(nChar, nRepCnt, nFlags);
}

void CAnnotation::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags){		
	switch (nChar){
	case 37: /* left */
		if (m_EditPosition) m_EditPosition--;
		break;
	case 39: /* right */
		if (m_EditPosition < m_Text.GetLength()) m_EditPosition++;
		break;
	case 46: /* del */
		if (m_EditPosition < m_Text.GetLength()) {
			m_Text.Delete(m_EditPosition, 1);			
		}
		break;		
	case 8: /* backspace */
		if (m_EditPosition) {
			m_Text.Delete(m_EditPosition-1, 1);
			m_EditPosition--;
		}
		break;
	case 36: /* home */
		m_EditPosition = 0;
		break;
	case 35: /* end */
		m_EditPosition = m_Text.GetLength();
		break;
	}
}

void CAnnotation::CreateFont(CDC * pDC, CRedimension * Redimension) {
	m_Font.DeleteObject();
	HDC hDC;
	if (pDC != NULL) hDC = pDC->m_hAttribDC; else hDC = ::GetDC(NULL);	
	POINT pt;
	pt.y = (::GetDeviceCaps(hDC, LOGPIXELSY) * m_FontSize) / 72;	
	if (pDC && pDC->IsPrinting() && Redimension) pt.y = (long) ((double)Redimension->tFontScale * m_FontSize * 96 / 72 - 8);
	m_Font.CreateFont(pt.y, 0, 0, 0, 
		(m_Bold)?FW_BOLD:FW_REGULAR, 
		m_Italic, m_Underline, m_StrikeOut,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, 
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, 
		m_FontName);
}

void CAnnotation::SetFontFromDialog(const CFontDialog& Dialog){
	m_FontSize = Dialog.GetSize()/10;
	m_FontName = Dialog.GetFaceName();
	m_Bold = Dialog.IsBold();
	m_Italic = Dialog.IsItalic();
	m_Underline = Dialog.IsUnderline();
	m_StrikeOut = Dialog.IsStrikeOut();	m_Color = Dialog.GetColor();
	CreateFont();	
}

void CAnnotation::InitFont() {
	m_FontSize = 12;
	m_FontName = "Helvetica";
	m_Bold = FALSE;
	m_Italic = FALSE;
	m_Underline = FALSE;
	m_StrikeOut = FALSE;
}

int CAnnotation::GetTextLength(void) const {
	return m_Text.GetLength();
}

