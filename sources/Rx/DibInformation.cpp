#include "stdafx.h"
#include "Rx.h"
#include "DibInformation.h"
#include "RxDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CDibInformation, CBufferView)

CDibInformation::CDibInformation() {
	m_GrayPen.CreatePen(PS_SOLID, 1, COLORREF(RGB(255, 0, 0)));
	m_Font.CreateFont(12, 0, 0, 0, 400, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
					  CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Arial");
}

CDibInformation::~CDibInformation() {
	m_GrayPen.DeleteObject();
	m_Font.DeleteObject();
}


BEGIN_MESSAGE_MAP(CDibInformation, CBufferView)
	//{{AFX_MSG_MAP(CDibInformation)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CDibInformation::DrawHistogram(CDC * pDC, const CRect& Rect, CDib * i_DIB){
	CPen * oldPen;

	float l_Div = (float)Rect.Width() / 256;

	oldPen = (CPen *) pDC->SelectStockObject(BLACK_BRUSH);

	pDC->MoveTo(CPoint(Rect.left, Rect.bottom));
	pDC->LineTo(Rect.BottomRight());

	int l_Value;		
	for (int i=0;i<256;i++){
		pDC->MoveTo(CPoint(Rect.left + (int) (l_Div*i), Rect.bottom));
		l_Value = (int) (i_DIB->m_Histogram.PDF(i) * Rect.Height());
		pDC->LineTo(CPoint(Rect.left + (int) (l_Div*i), Rect.bottom - l_Value));
	}
	
	pDC->SelectObject(oldPen);	

	oldPen = pDC->SelectObject(&m_GrayPen);	
	pDC->MoveTo(CPoint(Rect.left + (int) (i_DIB->m_Histogram.GetAverageLevel()*l_Div), Rect.bottom));
	pDC->LineTo(CPoint(Rect.left + (int) (i_DIB->m_Histogram.GetAverageLevel()*l_Div), Rect.bottom - Rect.Height()));	
	pDC->SelectObject(oldPen);
	
}

void CDibInformation::DrawHistogramIntegral(CDC * pDC, const CRect& Rect, CDib * i_DIB){
	CPen * oldPen;

	float l_Div = (float)Rect.Width() / 256;

	oldPen = (CPen *) pDC->SelectStockObject(BLACK_BRUSH);

	pDC->MoveTo(CPoint(Rect.left, Rect.bottom));
	pDC->LineTo(Rect.BottomRight());

	/*
	int l_Value;
	unsigned long l_Sum = 0;
	unsigned long l_Diff = i_DIB->m_Histogram.GetHistogramIntegral();
	l_Diff = (l_Diff)?l_Diff:1;	
	for (int i=0;i<256;i++){
		pDC->MoveTo(CPoint(Rect.left + (int) (l_Div*i), Rect.bottom));
		l_Sum += i_DIB->m_Histogram[i];
		l_Value = (int) ((double) l_Sum * Rect.Height()) / l_Diff;
		pDC->LineTo(CPoint(Rect.left + (int) (l_Div*i), Rect.bottom - l_Value));		
	}
	*/
		
	int l_Value;	
	for (int i=0;i<256;i++){
		pDC->MoveTo(CPoint(Rect.left + (int) (l_Div*i), Rect.bottom));		
		l_Value = (int) (i_DIB->m_Histogram.GetCumulativeAt(i) * Rect.Height());
		pDC->LineTo(CPoint(Rect.left + (int) (l_Div*i), Rect.bottom - l_Value));		
	}	
	
	pDC->SelectObject(oldPen);
}


void CDibInformation::OnDrawPrivate(CDC* pDC) {	
	CRxDoc * pDoc = (CRxDoc *) GetDocument();	
	CDib * l_DIB = pDoc->GetSelectedDIB();
	CFont * oldFont = (CFont *) pDC->SelectObject(&m_Font);	
	CString l_Information;
	if (l_DIB) {
		
		CRect l_Rect; CBufferView::GetClientRect(l_Rect);
		if (l_DIB->GetDepth() == 1) {
			pDC->TextOut(10,50, "No Histogram, DIB is Duotone.");
		} else {			
			
			CRect l_HRect = l_Rect;
			l_HRect.top = 50;
			l_HRect.bottom = 50 + l_Rect.Height()/3;
			l_HRect.left += l_Rect.Width() / 20;
			l_HRect.right -= l_Rect.Width() / 20;

			DrawHistogram(pDC, l_HRect, l_DIB);

			l_HRect = l_Rect;
			l_HRect.top = 50 + l_Rect.Height()/3 + 10;
			l_HRect.bottom = 50 + 2*(l_Rect.Height()/3) + 10;
			l_HRect.left += l_Rect.Width() / 20;
			l_HRect.right -= l_Rect.Width() / 20;

			DrawHistogramIntegral(pDC, l_HRect, l_DIB);

		}
		
		l_Information.Format("Color depth: %d bits/pixel", l_DIB->GetDepth());	
		pDC->TextOut(10, 10, l_Information);
		l_Information.Format("Rotation Angle: %d°", (360-l_DIB->GetDegAngle())%360);	
		pDC->TextOut(10, 20, l_Information);
		l_Information.Format("Image Dimensions: %.02fx%.02f mm", l_DIB->GetDimensions().cx*pDoc->m_VectorManager.GetMetric(), l_DIB->GetDimensions().cy*pDoc->m_VectorManager.GetMetric());
		pDC->TextOut(10, 30, l_Information);
		l_Information.Format("Scaling: x=%d%% y=%d%%", l_DIB->GetXPercent(), l_DIB->GetYPercent());
		pDC->TextOut(10, 40, l_Information);

	} else {		
		if (pDoc->m_AnnotationManager.GetSelectedIndex() != -1) {
			l_Information.Format("Font: %s", pDoc->m_AnnotationManager.GetSelected()->GetFontName());
			pDC->TextOut(10, 10, l_Information);
			l_Information.Format("Font Size: %d points", pDoc->m_AnnotationManager.GetSelected()->GetFontSize());
			pDC->TextOut(10, 20, l_Information);
			l_Information.Format("Text Length: %d characters", pDoc->m_AnnotationManager.GetSelected()->GetTextLength());
			pDC->TextOut(10, 30, l_Information);
		} else if (pDoc->m_VectorManager.GetSelectedIndex() != -1) {
			l_Information.Format("Segment length: %.02f mm", pDoc->m_VectorManager.GetSelected()->GetDistance()*pDoc->m_VectorManager.GetMetric());	
			pDC->TextOut(10, 10, l_Information);
			l_Information.Format("Edge Count: %d", pDoc->m_VectorManager.GetSelected()->GetSize());	
			pDC->TextOut(10, 20, l_Information);			
		} else pDC->TextOut(10, 10, "No valid object selected.");
	}

	pDC->SelectObject(oldFont);
}

void CDibInformation::OnInitialUpdate() {	
	CBufferView::OnInitialUpdate();
	CRect l_ParentRect;	
	CBufferView::GetParent()->GetParent()->GetClientRect(&l_ParentRect);
	CRect l_Position;
	int l_Width = 280;
	int l_Height = 220;
	l_Position.left = l_ParentRect.left + l_ParentRect.Width() - l_Width;
	l_Position.top = 0;
	l_Position.right = l_Position.left + l_Width;
	l_Position.bottom = l_Position.top + l_Height;
	CBufferView::GetParent()->MoveWindow(l_Position);	
}


