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
#include "DibTracker.h"
#include "AVector.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define A_WIDTH 4

CDibTracker::CDibTracker(){
	m_Selected = FALSE;	
	m_Anchor = AP_VOID;
	m_Pen.CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
	m_Brush.CreateSolidBrush(RGB(128,128,128));
	m_Font.CreateFont(12, 0, 0, 0, 400, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
					  CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Arial");
	m_ChgSize = CSize(0,0);
	m_ChgAngle = 0;
	m_dib.SetDibTracker(this);
}

CDibTracker::~CDibTracker(){

}

CDibTracker::CDibTracker(const CDib& ADib){
	m_dib = ADib;
	m_dib.SetDibTracker(this);
	m_size = m_dib.GetDimensions();
	m_rect = CRect(CPoint(0, 0), m_size);
}

CDibTracker::CDibTracker(const CDibTracker& Rhs){
	(* this) = Rhs;
}

CDibTracker& CDibTracker::operator=(const CDibTracker& Rhs){
	CVectorObject::operator=(Rhs);
	m_dib = Rhs.m_dib;
	m_dib.SetDibTracker(this);
	m_size = Rhs.m_size;
	m_rect = Rhs.m_rect;
	m_ChgAngle = Rhs.m_ChgAngle;
	m_ChgSize = Rhs.m_ChgSize;
	return (* this);
}

CRgn& CDibTracker::GetRotateRgn(CRgn& Result){	
	return m_dib.GetRotateRgn(m_rect.TopLeft(), m_rect.Size(), Result);
}

CRgn& CDibTracker::GetAnchorRgn(CRgn& Result){
	POINT Bounds[4];
	int m_dibAngle = abs(m_dib.GetDegAngle()) % 180;
	if ((m_dibAngle == 0)||(m_dibAngle == 90)){		
		CRect ResRect(m_rect);
		ResRect.InflateRect(CSize(A_WIDTH, A_WIDTH));
		Bounds[0] = ResRect.TopLeft();
		Bounds[1] = CPoint(ResRect.left, ResRect.bottom);
		Bounds[2] = ResRect.BottomRight();
		Bounds[3] = CPoint(ResRect.right, ResRect.top);
	} else {
		Bounds[0] = AnchorPt(AP_TOPLEFT);
		Bounds[1] = AnchorPt(AP_BOTTOMLEFT);
		Bounds[2] = AnchorPt(AP_BOTTOMRIGHT);
		Bounds[3] = AnchorPt(AP_TOPRIGHT);	

		int i;
		int leftmost_x = 0; for (i=1;i<4;i++) if (Bounds[i].x < Bounds[leftmost_x].x) leftmost_x = i;
		int rightmost_x = 0; for (i=1;i<4;i++) if (Bounds[i].x > Bounds[rightmost_x].x) rightmost_x = i;
		int topmost_y = 0; for (i=1;i<4;i++) if (Bounds[i].y < Bounds[topmost_y].y) topmost_y = i;
		int bottommost_y = 0; for (i=1;i<4;i++) if (Bounds[i].y > Bounds[bottommost_y].y) bottommost_y= i;

		Bounds[leftmost_x].x -= A_WIDTH;
		Bounds[rightmost_x].x += A_WIDTH;
		Bounds[topmost_y].y -= A_WIDTH;
		Bounds[bottommost_y].y += A_WIDTH;				
	}
	Result.CreatePolygonRgn(Bounds, 4, ALTERNATE);	
	return Result;
}

CRect CDibTracker::GetRect(void){	
	//return m_rect;
	m_dib.MakeRotateConstants(m_rect.TopLeft(), m_rect.Size());
	CRect Result = m_dib.GetRotateBounds(m_rect.TopLeft(), m_rect.Size());
	Result.InflateRect(CSize(A_WIDTH, A_WIDTH));
	return Result;	
}

CSize CDibTracker::GetSize(void){
	return m_size;
}

CPoint CDibTracker::AnchorPt(CAnchorPosition Pos){
	switch(Pos){
	case AP_TOPLEFT: 
		return CPoint(m_TopLeft.x, m_TopLeft.y);
	case AP_TOPMIDDLE: 
		return CPoint(m_TopLeft.x + (m_TopRight.x-m_TopLeft.x)/2, m_TopLeft.y + (m_TopRight.y-m_TopLeft.y)/2); 					 
	case AP_MIDDLETOP:
	case AP_TOPRIGHT:		
		return CPoint(m_TopRight.x, m_TopRight.y);
	case AP_MIDDLELEFT:		
		return CPoint(m_TopLeft.x + (m_BottomLeft.x-m_TopLeft.x)/2, m_TopLeft.y + (m_BottomLeft.y-m_TopLeft.y)/2);
	case AP_MIDDLERIGHT:		
		return CPoint(m_TopRight.x + (m_BottomRight.x-m_TopRight.x)/2, m_TopRight.y+(m_BottomRight.y-m_TopRight.y)/2);
	case AP_BOTTOMLEFT:		
		return CPoint(m_BottomLeft.x, m_BottomLeft.y);
	case AP_MIDDLEBOTTOM:
	case AP_BOTTOMMIDDLE:		
		return CPoint(m_BottomLeft.x + (m_BottomRight.x - m_BottomLeft.x)/2, m_BottomLeft.y + (m_BottomRight.y - m_BottomLeft.y)/2);
	case AP_BOTTOMRIGHT:		
		return CPoint(m_BottomRight.x,m_BottomRight.y);
	}
	return m_rect.TopLeft();
}

CRect CDibTracker::Anchor(CAnchorPosition Pos){	
	return CRect(AnchorPt(Pos) - CSize(A_WIDTH/2, A_WIDTH/2), CSize(A_WIDTH, A_WIDTH));	
}

void CDibTracker::Paint(CDC* memDC, const CRedimension& Redimension, BOOL ShowContents){		
	if (!m_dib.isEmpty()){
		CRect _m_rect = m_rect;		
		memDC->LPtoDP(&_m_rect);		
		
		if (ShowContents) m_dib.Draw(memDC, Redimension.GetPoint(_m_rect.TopLeft()), _m_rect.Size());

		m_dib.MakeRotateConstants(_m_rect.TopLeft(), _m_rect.Size(), TRUE);
		m_BottomLeft = m_dib.GetBottomLeft(_m_rect.TopLeft(), _m_rect.Size());
		m_BottomRight = m_dib.GetBottomRight(_m_rect.TopLeft(), _m_rect.Size());
		m_TopRight = m_dib.GetTopRight(_m_rect.TopLeft(), _m_rect.Size());
		m_TopLeft = m_dib.GetTopLeft(_m_rect.TopLeft(), _m_rect.Size());

		if (m_Selected && (!memDC->IsPrinting())){
			Redimension.TranslateRect(_m_rect);
			/*
			memDC->SelectStockObject(GRAY_BRUSH);
			CRgn pRgn; GetAnchorRgn(pRgn);
			memDC->FillRgn(&pRgn, memDC->GetCurrentBrush());	
			*/
			memDC->FrameRect(&_m_rect, &m_Brush);

			CFont * oldFont = memDC->SelectObject(&m_Font);			
			CString m_AngleString = ""; if (m_dib.GetDegAngle()) m_AngleString.Format("%d° ", 360 - m_dib.GetDegAngle());
			CString m_ScaleString = ""; if ((m_dib.GetXScale() != 1)||(m_dib.GetYScale() != 1)) m_ScaleString.Format("(%d%%:%d%%) ", m_dib.GetXPercent(), m_dib.GetYPercent());
			CString m_RLeg = m_AngleString + m_ScaleString + m_dib.GetComment();			
			if (m_RLeg.GetLength()) {
				memDC->SetTextColor((COLORREF) 0);
				memDC->DrawText(m_RLeg, _m_rect+CSize(3,3), DT_NOCLIP | DT_LEFT);
			}
			memDC->SelectObject(oldFont);
			CPen * oldPen = memDC->SelectObject(&m_Pen);			
			memDC->MoveTo(Redimension.GetPoint(AnchorPt(AP_TOPLEFT)));
			memDC->LineTo(Redimension.GetPoint(AnchorPt(AP_TOPRIGHT)));						
			memDC->LineTo(Redimension.GetPoint(AnchorPt(AP_BOTTOMRIGHT)));
			memDC->LineTo(Redimension.GetPoint(AnchorPt(AP_BOTTOMLEFT)));
			memDC->LineTo(Redimension.GetPoint(AnchorPt(AP_TOPLEFT)));
			//memDC->Draw3dRect(m_rect, (COLORREF) 224, (COLORREF) 224);
			memDC->FillSolidRect(Redimension.GetRect(Anchor(AP_TOPLEFT)), RGB(255, 0, 0));			
			memDC->FillSolidRect(Redimension.GetRect(Anchor(AP_TOPRIGHT)), RGB(255, 0, 0));			
			memDC->FillSolidRect(Redimension.GetRect(Anchor(AP_TOPMIDDLE)), RGB(255, 0, 0));
			memDC->FillSolidRect(Redimension.GetRect(Anchor(AP_MIDDLELEFT)), RGB(255, 0, 0));
			memDC->FillSolidRect(Redimension.GetRect(Anchor(AP_MIDDLERIGHT)), RGB(255, 0, 0));
			memDC->FillSolidRect(Redimension.GetRect(Anchor(AP_BOTTOMLEFT)), RGB(255, 0, 0));
			memDC->FillSolidRect(Redimension.GetRect(Anchor(AP_BOTTOMMIDDLE)), RGB(255, 0, 0));
			memDC->FillSolidRect(Redimension.GetRect(Anchor(AP_BOTTOMRIGHT)), RGB(255, 0, 0));
			memDC->SelectObject(oldPen);		
		}
	}
}

CDib * CDibTracker::GetDib(void){
	return &m_dib;
}

void CDibTracker::Serialize(CArchive& ar){
	try {		
		if (ar.IsStoring()){		
			ar << m_rect;
			ar << (float) m_dib.GetAngle();
		} else {
			ar >> m_rect;
			float m_Angle;
			ar >> m_Angle;
			m_dib.SetAngle(m_Angle);
		}
		m_dib.Serialize(ar);
	} catch (...) {
		MessageBox(::GetFocus(), "Serialization Error (DIBTracker)", "Rx Error", MB_OK);
	}
}

void CDibTracker::ViewPaletteChanged(CClientDC& dc, UINT wParam){
	m_dib.UsePalette(&dc, wParam);
}

void CDibTracker::SetSystemPalette(CDC * dc){
	m_dib.SetSystemPalette(dc);
}

void CDibTracker::UsePalette(CDC * dc, BOOL Background){
	m_dib.UsePalette(dc, Background);
}

COleDataSource* CDibTracker::SaveDib(){	
	return m_dib.SaveDib();
}

void CDibTracker::CopyDibTo(void){
	m_dib.CopyDibTo();
}

BOOL CDibTracker::EditPaste(void){
	COleDataObject dataObject;
    VERIFY(dataObject.AttachClipboard());    
	// update command user interface should keep us out of here if not CF_DIB
    if (dataObject.IsDataAvailable(CF_DIB)) {
        // Seems to be MOVEABLE memory, so we must use GlobalLock!
		//  (hDib != lpDib) GetGlobalData copies the memory, so we can
		//  hang onto it until we delete the CDib.
		HGLOBAL hDib = dataObject.GetGlobalData(CF_DIB);
		ASSERT(hDib != NULL);
		LPVOID lpDib = ::GlobalLock(hDib);
		ASSERT(lpDib != NULL);
		if (m_dib.AttachMemory(lpDib, TRUE, hDib)){			
			m_size = m_dib.GetDimensions();
			m_rect = CRect(CPoint(0, 0), m_size);
			return TRUE;
		} else return FALSE;
	} else return FALSE;
}

BOOL CDibTracker::EditPastefrom(void){
	static char BASED_CODE szFilter[] = "Bitmap Files (*.bmp)|*.bmp|Dib Files (*.dib)|*.dib|All Files (*.*)|*.*||";
	CFileDialog dlg(TRUE, "bmp", "*.bmp", 
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST,
		szFilter);
    if (dlg.DoModal() != IDOK) return FALSE;	
    if (m_dib.AttachMapFile(dlg.GetPathName(), TRUE)){
		m_size = m_dib.GetDimensions();
		m_rect = CRect(CPoint(0, 0), m_size);
		return TRUE;
	} else return FALSE;
}

BOOL CDibTracker::isEmpty(void){
	return m_dib.isEmpty();
}

BOOL CDibTracker::Select(const CPoint& Point){		
	m_Anchor = AP_VOID;
	CRgn RRgn;
	if (GetAnchorRgn(RRgn).PtInRegion(Point)) {
		m_SelPoint = Point;
		m_InitSelPoint = Point;
		m_Selected = TRUE;
		m_SelAngle = m_dib.GetDegAngle();

		if (Anchor(AP_TOPLEFT).PtInRect(Point)) m_Anchor = AP_TOPLEFT;
		else if (Anchor(AP_TOPMIDDLE).PtInRect(Point)) m_Anchor = AP_TOPMIDDLE;
		else if (Anchor(AP_TOPRIGHT).PtInRect(Point)) m_Anchor = AP_TOPRIGHT;
		else if (Anchor(AP_MIDDLELEFT).PtInRect(Point)) m_Anchor = AP_MIDDLELEFT;
		else if (Anchor(AP_MIDDLERIGHT).PtInRect(Point)) m_Anchor = AP_MIDDLERIGHT;
		else if (Anchor(AP_BOTTOMLEFT).PtInRect(Point)) m_Anchor = AP_BOTTOMLEFT;
		else if (Anchor(AP_BOTTOMMIDDLE).PtInRect(Point)) m_Anchor = AP_BOTTOMMIDDLE;
		else if (Anchor(AP_BOTTOMRIGHT).PtInRect(Point)) m_Anchor = AP_BOTTOMRIGHT;		

		return TRUE;
	} else {
		m_Selected = FALSE;
		return FALSE;
	}
}

BOOL CDibTracker::OnMouseMove(const CPoint& Point){
	if (m_Selected) {
		if (Anchor(AP_TOPLEFT).PtInRect(Point) ||
			Anchor(AP_TOPMIDDLE).PtInRect(Point) ||
			Anchor(AP_TOPRIGHT).PtInRect(Point) ||
			Anchor(AP_MIDDLELEFT).PtInRect(Point) ||
			Anchor(AP_MIDDLERIGHT).PtInRect(Point) ||
			Anchor(AP_BOTTOMLEFT).PtInRect(Point) ||
			Anchor(AP_BOTTOMMIDDLE).PtInRect(Point) ||
			Anchor(AP_BOTTOMRIGHT).PtInRect(Point)){
			//SetCursor(LoadCursor(0, IDC_CROSS));
			m_Cursor = IDC_CROSS;
			return TRUE;
		}
	} else {
		CRgn RRgn;
		if (GetAnchorRgn(RRgn).PtInRegion(Point)) {
			//SetCursor(LoadCursor(0, IDC_UPARROW));
			m_Cursor = IDC_UPARROW;
			return TRUE;
		} else return FALSE;
	}
	return FALSE;
}

CPoint CDibTracker::GetCenter(void){
	return CPoint((long)(m_rect.left + m_rect.Width()/2), (long) (m_rect.top + m_rect.Height()/2));
}

BOOL CDibTracker::GetRotating(void) const {
	return (m_Anchor != AP_VOID);
	/*
	if (GetKeyState(VK_CONTROL) >> 3){
		if (m_Anchor != AP_VOID){			
			return TRUE;
		}
	}
	return FALSE;
	*/
}

BOOL CDibTracker::GetRotatingChanged(void) {
	if (GetRotating()) {
		if (m_ChgAngle != m_dib.GetDegAngle()) {
			m_ChgAngle = m_dib.GetDegAngle();
			return TRUE;
		}
		if (m_ChgSize != m_rect.Size()) {
			m_ChgSize = m_rect.Size();
			return TRUE;
		}
	} 
	return FALSE;
}

CAnchorPosition CDibTracker::NormalizeAnchor(CAnchorPosition Anchor) {
	int l_Angle = 360 - m_dib.GetDegAngle();
	if ((l_Angle >= 45) && (l_Angle < 135)) {
		switch(Anchor) {
			case AP_TOPLEFT:		return AP_TOPRIGHT;		
			case AP_TOPMIDDLE:		
			case AP_MIDDLETOP:		return AP_MIDDLERIGHT;
			case AP_TOPRIGHT:		return AP_BOTTOMRIGHT;
			case AP_MIDDLELEFT:		return AP_MIDDLETOP;
			case AP_MIDDLERIGHT:	return AP_BOTTOMMIDDLE;
			case AP_BOTTOMLEFT:		return AP_TOPLEFT;
			case AP_MIDDLEBOTTOM:
			case AP_BOTTOMMIDDLE:	return AP_MIDDLELEFT;
			case AP_BOTTOMRIGHT:	return AP_BOTTOMLEFT;
		}	
	} else if ((l_Angle >= 135) && (l_Angle < 225)) {
		switch(Anchor) {
			case AP_TOPLEFT:		return AP_BOTTOMRIGHT;		
			case AP_TOPMIDDLE:					
			case AP_MIDDLETOP:		return AP_MIDDLEBOTTOM;
			case AP_TOPRIGHT:		return AP_BOTTOMLEFT;
			case AP_MIDDLELEFT:		return AP_MIDDLERIGHT;
			case AP_MIDDLERIGHT:	return AP_MIDDLELEFT;
			case AP_BOTTOMLEFT:		return AP_TOPRIGHT;
			case AP_MIDDLEBOTTOM:
			case AP_BOTTOMMIDDLE:	return AP_MIDDLETOP;
			case AP_BOTTOMRIGHT:	return AP_TOPLEFT;
		}
	} else if ((l_Angle >= 225) && (l_Angle < 315)) {
		switch(Anchor) {
			case AP_TOPLEFT:		return AP_BOTTOMLEFT;		
			case AP_TOPMIDDLE:					
			case AP_MIDDLETOP:		return AP_MIDDLELEFT;
			case AP_TOPRIGHT:		return AP_TOPLEFT;
			case AP_MIDDLELEFT:		return AP_MIDDLEBOTTOM;
			case AP_MIDDLERIGHT:	return AP_MIDDLETOP;
			case AP_BOTTOMLEFT:		return AP_BOTTOMRIGHT;
			case AP_MIDDLEBOTTOM:
			case AP_BOTTOMMIDDLE:	return AP_MIDDLERIGHT;
			case AP_BOTTOMRIGHT:	return AP_TOPRIGHT;
		}
	} 	
	return Anchor;
}

BOOL CDibTracker::MouseMove(const CPoint& APoint){		
	if (GetKeyState(VK_CONTROL) >> 3){
		if (m_Anchor != AP_VOID){			
			m_dib.SetDegAngle(m_SelAngle + (int) CAVector::AngleXYZ(GetCenter(), APoint, m_SelPoint));
		} else return FALSE;
		return TRUE;
	} 	
	const int MinDim = 15;	
	CRect newRect = m_rect;			
	if (m_Anchor != AP_VOID){
		int sf_half = 0;
		int dx = APoint.x - m_SelPoint.x;
		int dy = APoint.y - m_SelPoint.y;	
		CAnchorPosition m_SaveAnchor = m_Anchor;
		m_Anchor = NormalizeAnchor(m_Anchor);		
		switch(m_Anchor){
			case AP_TOPLEFT:
				newRect.left += dx;
				if (GetKeyState(VK_SHIFT) >> 3)					
					newRect.top = newRect.bottom - (long) ((float)((float)((float) newRect.Width() / m_dib.nWidth)) * m_dib.nHeight);
				else newRect.top += dy;
				break;
			case AP_TOPMIDDLE:					
			case AP_MIDDLETOP:
				newRect.top += dy;
				if (GetKeyState(VK_SHIFT) >> 3)	{					
					sf_half = (long) ((float)((float)((float) newRect.Height() / m_dib.nHeight)) * m_dib.nWidth);
					dx = (sf_half-newRect.Width())/2;
					newRect.left = newRect.right - sf_half + dx;					
					newRect.right += dx;
				}
				break;
			case AP_TOPRIGHT:
				newRect.right += dx;
				if (GetKeyState(VK_SHIFT) >> 3)					
					newRect.top = newRect.bottom - (long) ((float)((float)((float) newRect.Width() / m_dib.nWidth)) * m_dib.nHeight);
				else newRect.top += dy;
				break;
			case AP_MIDDLELEFT:
				newRect.left += dx;
				if (GetKeyState(VK_SHIFT) >> 3)	{					
					sf_half = (long) ((float)((float)((float) newRect.Width() / m_dib.nWidth)) * m_dib.nHeight);
					dx = (sf_half-newRect.Height())/2;
					newRect.top = newRect.bottom - sf_half + dx;					
					newRect.bottom += dx;
				}
				break;
			case AP_MIDDLERIGHT:
				newRect.right += dx;
				if (GetKeyState(VK_SHIFT) >> 3)	{					
					sf_half = (long) ((float)((float)((float) newRect.Width() / m_dib.nWidth)) * m_dib.nHeight);
					dx = (sf_half-newRect.Height())/2;
					newRect.top = newRect.bottom - sf_half + dx;					
					newRect.bottom += dx;
				}
				break;
			case AP_BOTTOMLEFT:
				newRect.left += dx;
				if (GetKeyState(VK_SHIFT) >> 3)					
					newRect.bottom = newRect.top + (long) ((float)((float)((float) newRect.Width() / m_dib.nWidth)) * m_dib.nHeight);
				else newRect.bottom += dy;
				break;
			case AP_MIDDLEBOTTOM:
			case AP_BOTTOMMIDDLE:
				newRect.bottom += dy;
				if (GetKeyState(VK_SHIFT) >> 3)	{					
					sf_half = (long) ((float)((float)((float) newRect.Height() / m_dib.nHeight)) * m_dib.nWidth);
					dx = (sf_half-newRect.Width())/2;
					newRect.left = newRect.right - sf_half + dx;					
					newRect.right += dx;
				}
				break;			
			case AP_BOTTOMRIGHT:
				newRect.right += dx;
				if (GetKeyState(VK_SHIFT) >> 3)					
					newRect.bottom = newRect.top + (long) ((float)((float)((float) newRect.Width() / m_dib.nWidth)) * m_dib.nHeight);
				else newRect.bottom += dy;
				break;
		}		
		m_Anchor = m_SaveAnchor;
		if (!newRect.IsRectEmpty() && (newRect.Width() > MinDim) && (newRect.Height() > MinDim)) {
			m_rect = newRect;
		} else return TRUE;		
	} else {
		AdjustShiftPoint((CPoint&)APoint);
		m_rect = CRect(m_rect.left + (APoint.x - m_SelPoint.x), m_rect.top + (APoint.y - m_SelPoint.y), 
				   m_rect.right + (APoint.x - m_SelPoint.x), m_rect.bottom + (APoint.y - m_SelPoint.y));
	}
	m_SelPoint = APoint;
	return TRUE;
}

void CDibTracker::AdjustShiftPoint(CPoint& point){
	if (GetKeyState(VK_SHIFT) >> 3){
		int dx = abs(m_InitSelPoint.x-point.x);
		int dy = abs(m_InitSelPoint.y-point.y);
		if (dx > dy) point.y = m_InitSelPoint.y;
		else point.x = m_InitSelPoint.x;
	}
}


BOOL CDibTracker::SetDibSize(void){	
	if (!m_dib.isEmpty()){		
		CRect newRect = CRect(m_rect.TopLeft(), m_dib.GetDimensions());
		if (m_rect != newRect){
			m_rect = newRect;
			return TRUE;
		} else return FALSE;
	} else return FALSE;
}

BOOL CDibTracker::SetDibRotation(void){
	if (!m_dib.isEmpty()){
		return m_dib.SetAngle(0);		
	} else return FALSE;
}

BOOL CDibTracker::Rotate(const CPoint& APoint){
	m_dib.SetDegAngle(m_SelAngle + (int) CAVector::AngleXYZ(GetCenter(), APoint, m_SelPoint));
	return TRUE;
}

BOOL CDibTracker::RotateSelect(const CPoint& APoint){
	m_SelPoint = APoint;
	m_SelAngle = m_dib.GetDegAngle();
	return TRUE;
}

void CDibTracker::Paint(CDC * memDC, const CRedimension& Redimension) {
	Paint(memDC, Redimension, TRUE);
}

void CDibTracker::EndMove(void) {
	m_Anchor = AP_VOID;	
}

BOOL CDibTracker::MouseMoveErase(const CPoint& APoint) {
	// transform the point coordinates
	
	int x = APoint.x - m_rect.left;
	int y = m_rect.Height() - (APoint.y - m_rect.top);
	CPoint l_Real = m_dib.CalcRealPoint(x, y);
	x = l_Real.x;
	y = l_Real.y;

	int xmin = x - 5; if (xmin < 0) xmin = 0; if (xmin > m_dib.GetWidth()) return FALSE;
	int ymin = y - 5; if (ymin < 0) ymin = 0; if (ymin > m_dib.GetHeight()) return FALSE;
	int xmax = x + 5; if (xmax >= m_dib.GetWidth()) xmax = m_dib.GetWidth();
	int ymax = y + 5; if (ymax >= m_dib.GetHeight()) ymax = m_dib.GetHeight();
	int nRowBytes = m_dib.RowBytes();
	m_dib.PutPixelRegion(xmin, ymin, xmax, ymax, 0xFFFFFFFF);	
	m_dib.Invalidate();
	return FALSE;
}