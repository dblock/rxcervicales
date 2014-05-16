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
#include "AVector.h"
#include "AVectorManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#include <math.h>
#include <float.h>
#include "Rx.h"

#define C_SIZE 15

extern CRxApp theApp;

CAVector::CAVector() {
	m_Parent = NULL;
	m_Distance = 0;
	m_Selected = FALSE;
	m_Sel = 2;
	m_Color = (COLORREF) 0;	 	 	 
	m_Pen.CreatePen(PS_SOLID, 1, (COLORREF) m_Color);
	m_Font.CreateFont(12, 0, 0, 0, FW_BOLD, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
					  CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Helvetica");
	m_ShowDistance = FALSE;
}

CAVector::~CAVector() {

}

CAVector& CAVector::Clone(const CAVector& Rhs){	
	CVectorObject::operator=(Rhs);
	m_Points.Copy(Rhs.m_Points);	
	m_Sel = Rhs.m_Sel;			
	m_Pen.DeleteObject();
	m_Pen.CreatePen(PS_SOLID, 1, Rhs.m_Color);
	m_ShowDistance = Rhs.m_ShowDistance;
	m_Distance = Rhs.m_Distance;
	m_Parent = Rhs.m_Parent;
	return (* this);
}

CAVector::CAVector(const CAVector& Other){
	Clone(Other);
}

CAVector& CAVector::Remove(int Index) {
	m_Points.RemoveAt(Index);
	return (* this);
}

CAVector& CAVector::Add(int X, int Y){
	CPoint Point(X, Y);
	return Add(Point);
}

CAVector& CAVector::Add(const CPoint& Point){
	m_Points.Add((CPoint&)Point);
	m_Bound = CalcBound();
	return (* this);
}

CAVector& CAVector::operator+=(const CPoint& Point){
	return Add(Point);
}


CAVector& CAVector::operator=(const CAVector& Other){
	Clone(Other);
	return (* this);
}

int CAVector::operator>(const CAVector& Other){
	return (m_Points.GetSize() > Other.m_Points.GetSize());	
}

int CAVector::operator<(const CAVector& Other){
	return (m_Points.GetSize() < Other.m_Points.GetSize());
}

int CAVector::operator>=(const CAVector& Other){
	return (m_Points.GetSize() >= Other.m_Points.GetSize());
}

int CAVector::operator<=(const CAVector& Other){
	return (m_Points.GetSize() <= Other.m_Points.GetSize());
}

int CAVector::operator==(const CAVector& Other){
	if (m_Points.GetSize() != Other.m_Points.GetSize()) return 0;
	else for (int i=0;i<m_Points.GetSize();i++)
		if (m_Points[i] != Other.m_Points[i]) return 0;
	return 1;
}

CPoint CAVector::GetAt(int Index) const {	
	return m_Points.GetAt(Index);
}

CPoint CAVector::operator[](int Index){
	return GetAt(Index);
}

int CAVector::GetSize(void) const {
	return m_Points.GetSize();
}

CRect CAVector::Anchor(int j){
	return CRect(GetAt(j).x-m_Sel, GetAt(j).y-m_Sel, GetAt(j).x+m_Sel, GetAt(j).y+m_Sel);
}

CRect CAVector::CalcBound() {
	CRect aBound;
	aBound.CopyRect(Anchor(0));
	for (int j = 1; j < GetSize();j++) {	
		aBound |= Anchor(j);		
	}
	aBound.InflateRect(CSize(C_SIZE*4,C_SIZE*4));
	return aBound;
}

void CAVector::Exchange(CPoint& Pt1, CPoint& Pt2){
	CPoint Pt = Pt1;
	Pt1 = Pt2;
	Pt2 = Pt;
}

void CAVector::PaintIntersection(CDC * pDC, const CRedimension& Redimension, CAVector& Second){	
	if (m_ShowDistance || Second.m_ShowDistance) return;
	int m_Angle;
	CPoint m_Int; 	
	CString m_AngleText;
	CPoint Points[4];			
	pDC->SetBkMode(TRANSPARENT); 		
	for (int i=0;i<GetSize()-1;i++)
		for (int j=0;j<Second.GetSize()-1;j++){
			Points[0] = GetAt(i); Points[1] = GetAt(i+1); Points[2] = Second.GetAt(j); Points[3] = Second.GetAt(j+1);			
			pDC->LPtoDP(&Points[0]); pDC->LPtoDP(&Points[1]); pDC->LPtoDP(&Points[2]); pDC->LPtoDP(&Points[3]);
			for (int p=0;p<4;p++) Redimension.TranslatePoint(Points[p]);
			if (Intersection(Points[0], Points[1], Points[2], Points[3], m_Int)){				
				m_Angle = (int) AngleXYZ(m_Int,Points[0],Points[2]);				
				int m_Angle_abs = abs(m_Angle);				
				if (m_Angle < 0) {
					pDC->SetArcDirection(AD_COUNTERCLOCKWISE);					
				} else {					
					if (theApp.IsWindows95()) Exchange(Points[0], Points[2]);
					pDC->SetArcDirection(AD_CLOCKWISE);
				}
				if ((m_Angle != 0)&&(m_Angle_abs != 180)) {
					if ((m_Angle_abs != 90)&&(m_Angle_abs != 180)) m_AngleText.Format("%d° (%d°)", m_Angle_abs, 180-m_Angle_abs);
					else m_AngleText.Format("%d°", m_Angle_abs);
					CPen * oldPen = pDC->SelectObject(&m_Pen);	
					pDC->Arc(CRect(m_Int.x - C_SIZE, m_Int.y - C_SIZE, m_Int.x + C_SIZE, m_Int.y + C_SIZE),
						Points[0],
						Points[2]);	
					pDC->SelectObject(oldPen);
					pDC->SetTextColor(m_Color);
					CFont * oldFont = pDC->SelectObject(&m_Font);
					pDC->DrawText(m_AngleText, CRect(m_Int, CSize(0, 0)), DT_NOCLIP | DT_LEFT);			
					pDC->SelectObject(oldFont);				
				}
			}
		}		
}

int CAVector::CalcDistance(int i1, int i2) const {
	if (GetSize() > 1) {
		CPoint First = GetAt(i1);
		CPoint Second = GetAt(i2);
		int d1 = Second.y - First.y; d1 *= d1;
		int d2 = Second.x - First.x; d2 *= d2;
		return (int) sqrt(d1 + d2);
	} else return 0;
}

void CAVector::Paint(CDC * pDC, const CRedimension& Redimension) {
	Paint(pDC, Redimension, FALSE);
}

void CAVector::RecalcDistance(void) {
	m_Distance = CalcDistance(0, 1);	
	int n_Size = GetSize();
	for (int j=0;j<n_Size-1;j++){		
		if (j && (n_Size >= 3)){
			m_Distance+=CalcDistance(j, j+1);
		}			
	}	
}

void CAVector::Paint(CDC * pDC, const CRedimension& Redimension, BOOL m_ShowAngles){
	CreateFont(pDC, (CRedimension *) & Redimension);
	CPen * oldPen = pDC->SelectObject(&m_Pen);		
	CFont * oldFont = pDC->SelectObject(&m_Font);	
	pDC->SetBkMode(TRANSPARENT); 
	int m_Angle;
	m_Distance = CalcDistance(0, 1);
	CString m_AngleText;
	int n_Size = GetSize();
	for (int j=0;j<n_Size-1;j++){		
		CPoint Pt1 = GetAt(j); pDC->LPtoDP(&Pt1);
		CPoint Pt2 = GetAt(j+1); pDC->LPtoDP(&Pt2);
		/*
		Pt1+=CPoint(Redimension.tX, Redimension.tY);
		Pt2+=CPoint(Redimension.tX, Redimension.tY);
		*/
		Redimension.TranslatePoint(Pt1);
		Redimension.TranslatePoint(Pt2);
		if (j && (n_Size >= 3)){
			m_Distance+=CalcDistance(j, j+1);
			if (!m_ShowDistance && m_ShowAngles) {
				CPoint Pt0 = GetAt(j-1); pDC->LPtoDP(&Pt0);
				Redimension.TranslatePoint(Pt0);
				m_Angle = (int) AngleXYZ(Pt1,Pt0,Pt2);			
				int m_Angle_abs = abs(m_Angle);
				if ((m_Angle != 0)&&(m_Angle_abs != 180)) {
					if (m_Angle < 0) pDC->SetArcDirection(AD_COUNTERCLOCKWISE);
					else pDC->SetArcDirection(AD_CLOCKWISE);				
					if ((m_Angle_abs != 90)&&(m_Angle_abs != 180)) m_AngleText.Format("%d° (%d°)", m_Angle_abs, 180-m_Angle_abs);
					else m_AngleText.Format("%d°", m_Angle_abs);					
					pDC->Arc(CRect(Pt1.x - C_SIZE, Pt1.y - C_SIZE, Pt1.x + C_SIZE, Pt1.y + C_SIZE), Pt0, Pt2);
					pDC->SetTextColor(m_Color);
					pDC->DrawText(m_AngleText, CRect(Pt1, CSize(0, 0)), DT_NOCLIP | DT_LEFT);
				}		
			}
		}
		if (m_Selected && (!pDC->IsPrinting())) {
			CRect AnchorRect = Anchor(j); 
			Redimension.TranslateRect(AnchorRect);
			pDC->LPtoDP(&AnchorRect); pDC->FillSolidRect(AnchorRect, 0);
			AnchorRect = Anchor(j+1); 
			Redimension.TranslateRect(AnchorRect);
			pDC->LPtoDP(&AnchorRect); pDC->FillSolidRect(AnchorRect, 0);
		}
		pDC->MoveTo(Pt1);
		pDC->LineTo(Pt2);
	}	
	if (m_ShowDistance && m_Parent) {
		pDC->SetTextColor(m_Color);
		CString m_DistanceText; m_DistanceText.Format("%0.02f mm", m_Distance*m_Parent->GetMetric());
		CRect m_DistanceTextPoint = Anchor(0) + CSize(3,3);
		pDC->LPtoDP(&m_DistanceTextPoint);
		Redimension.TranslateRect(m_DistanceTextPoint);
		pDC->DrawText(m_DistanceText, m_DistanceTextPoint, DT_NOCLIP | DT_LEFT);
	}
	pDC->SelectObject(oldPen);		
	pDC->SelectObject(oldFont);
}

double CAVector::Distance(const CPoint& First, const CPoint& Second, const CPoint& A) {
	return Distance((double) First.x, (double) First.y, (double) Second.x, (double) Second.y, (double) A.x, (double) A.y);
}

/*
	Distance between a point and a segment
	*/
double CAVector::Distance(double x1, double x2, double y1, double y2, double a, double b) {
	double v1,v2,vtn1,vtn2,k,l,r;	
	if (y2 == x2) y2+=0.0001;
	v1 = y1-x1;
	v2 = y2-x2;	
	r = sqrt((v1*v1)+(v2*v2));
	vtn1 = -v2/r;
	vtn2 = v1/r;
	k = (x1+((b*v1)/v2)-((x2*v1)/v2)-a)/(vtn1-((vtn2*v1)/v2));	
	l = (b+((vtn2*x1)/vtn1)-((a*vtn2)/vtn1)-x2)/(v2-((vtn2*v1)/vtn1));	
	if (l <= 0) return sqrt((x1-a)*(x1-a)+(x2-b)*(x2-b));
	else if (l >= 1.0) return sqrt((y1-a)*(y1-a)+(y2-b)*(y2-b));	
	else return fabs(k);	
}

BOOL CAVector::Intersects(const CPoint& Source, const CPoint& Target, const CPoint& Victim){
	double curDist = Distance(Source.x, Source.y, Target.x, Target.y, Victim.x, Victim.y);		
	return (curDist < 5);
}

BOOL CAVector::Intersects(const CPoint& APoint){
	for (int i=0;i<m_Points.GetSize()-1;i++){
		if (Intersects(m_Points[i], m_Points[i+1], APoint)) return TRUE;
	}
	return FALSE;	
}

BOOL CAVector::Select(const CPoint& APoint){	
	m_Selected = Intersects(APoint);
	m_Anchors.RemoveAll();
	if (m_Selected) {
		for (int i=0;i<m_Points.GetSize();i++){
			if (Anchor(i).PtInRect(APoint)) m_Anchors.Add(i);	
		}
		if (!m_Anchors.GetSize()) for (int i=0;i<m_Points.GetSize();i++) m_Anchors.Add(i);
		m_SelPoint = APoint;
		m_SelInitPoint = APoint;
	}
	return m_Selected;
}

BOOL CAVector::CanSelect(const CPoint& APoint){	
	return Intersects(APoint);	
}

BOOL CAVector::OnMouseMove(const CPoint& APoint){
	if (m_Selected) {
		for (int i=0;i<m_Points.GetSize();i++){
			if (Anchor(i).PtInRect(APoint)) {
				m_Cursor = IDC_CROSS;
				//SetCursor(LoadCursor(0, IDC_CROSS));
				return TRUE;
			}			
		}
	}
	if (CanSelect(APoint)) {
		m_Cursor = IDC_UPARROW;
		//SetCursor(LoadCursor(0, IDC_UPARROW));
		return TRUE;
	} else return FALSE; 	
}


BOOL CAVector::MouseMove(const CPoint& APoint){
	if (m_Anchors.GetSize()){
		//if (m_Anchors.GetSize() == m_Points.GetSize()) SetCursor(LoadCursor(0, IDC_UPARROW));
		//else SetCursor(LoadCursor(0, IDC_CROSS));
		if (m_Anchors.GetSize() == m_Points.GetSize()) m_Cursor = IDC_UPARROW;
		else m_Cursor = IDC_CROSS;
		AdjustShiftPoint((CPoint&) APoint);

		if ((GetKeyState(VK_CONTROL) >> 3)&&
			(m_Anchors.GetSize() == 1)&&
			(m_Points.GetSize() == 3)&&
			((m_Anchors[0] == 0)||(m_Anchors[0] == 2))){
			
			m_Points[m_Anchors[0]].x += (APoint.x - m_SelPoint.x);
			m_Points[m_Anchors[0]].y += (APoint.y - m_SelPoint.y);			
			m_Points[2 - m_Anchors[0]].x -= (APoint.x - m_SelPoint.x);
			m_Points[2 - m_Anchors[0]].y -= (APoint.y - m_SelPoint.y);

		} else for (int i=0;i<m_Anchors.GetSize();i++){
			m_Points[m_Anchors[i]].x += (APoint.x - m_SelPoint.x);
			m_Points[m_Anchors[i]].y += (APoint.y - m_SelPoint.y);				
		}
		m_SelPoint = APoint;
	}
	m_Bound = CalcBound();
	return TRUE;
}

BOOL CAVector::SetColor(COLORREF newColor){
	if (m_Color != newColor){
		m_Color = newColor;				
		m_Pen.DeleteObject();
		m_Pen.CreatePen(PS_SOLID, 1, (COLORREF) m_Color);
		return TRUE;
	} else return FALSE;
}

COLORREF CAVector::GetColor(){
	return m_Color;
}

void CAVector::Serialize(CArchive& ar){	
	if (ar.IsStoring()){
		ar << m_Color;			
		m_Points.Serialize(ar);
	} else {
		ar >> m_Color;
		m_Points.Serialize(ar);
	}
}

BOOL CAVector::EndMove(void){
	m_Anchors.RemoveAll();
	m_Bound = CalcBound();
	return TRUE;
}

void CAVector::AdjustShiftPoint(CPoint& point){
	if (GetKeyState(VK_SHIFT) >> 3){
		int dx = abs(m_SelInitPoint.x-point.x);
		int dy = abs(m_SelInitPoint.y-point.y);
		if (dx > dy) point.y = m_SelInitPoint.y;
		else point.x = m_SelInitPoint.x;
	}
}

double CAVector::AngleXYZ(const CPoint& X, const CPoint& Y, const CPoint& Z) {
	return AngleXYZ(X.x, X.y, Y.x, Y.y, Z.x, Z.y);
}

double CAVector::AngleXYZ(double x1,double x2, double y1,double y2, double z1,double z2) {
	double r,v1,v2,k,l,vtn1,vtn2,i1,i2,a,b,res1,res2, res3;
	v1=y1-x1;
	v2=y2-x2;	
	if (!v1) v1=DBL_EPSILON;
	if (!v2) v2=DBL_EPSILON;
	r=sqrt((v1*v1)+(v2*v2));
	if (v2 == DBL_EPSILON) vtn1=-DBL_EPSILON; else vtn1=-v2/r;
	if (v1 == DBL_EPSILON) vtn2=DBL_EPSILON; else vtn2=v1/r;
	k=(x1+((z2*v1)/v2)-((x2*v1)/v2)-z1)/(vtn1-((vtn2*v1)/v2));
	l=(z2+((vtn2*x1)/vtn1)-((z1*vtn2)/vtn1)-x2)/(v2-((vtn2*v1)/vtn1));
	i1=x1+l*v1;
	i2=x2+l*v2;
	a=sqrt(((x1-i1)*(x1-i1))+((x2-i2)*(x2-i2)));
	b=sqrt(((z1-i1)*(z1-i1))+((z2-i2)*(z2-i2)));	
	res1=fabs(atan(b/a));			
	if(l <= 0) res2=(M_PI-res1); else res2=res1;	
	res3 = (res2/M_PI)*180;	
	if (k >= 0) return -res3;
	else return res3;
}

double CAVector::AngleSeg(const CPoint& X, const CPoint& Y, const CPoint& Z, const CPoint& W) {
	return AngleSeg(X.x, X.y, Y.x, Y.y, Z.x, Z.y, W.x, W.y);
}

int CAVector::Intersection(const CPoint& X, const CPoint& Y, const CPoint& Z, const CPoint& W, CPoint& Target){
	return Intersection(X.x, X.y, Y.x, Y.y, Z.x, Z.y, W.x, W.y, Target);
}

int CAVector::Intersection(double x1,double x2, double y1,double y2, double z1,double z2,double w1, double w2, CPoint& Target){
	double r,v1,v2,k,l,vtn1,vtn2;
	v1=y1-x1;
	v2=y2-x2;
	if (!v1) v1=0.00000000001;
	if (!v2) v2=0.00000000001;
	r=sqrt((v1*v1)+(v2*v2));
	vtn1=w1-z1;
	vtn2=w2-z2;
	if (!vtn1) vtn1=0.00000000001;
	if (!vtn2) vtn2=0.00000000001;
	k=(x1+((z2*v1)/v2)-((x2*v1)/v2)-z1)/(vtn1-((vtn2*v1)/v2));
	l=(z2+((vtn2*x1)/vtn1)-((z1*vtn2)/vtn1)-x2)/(v2-((vtn2*v1)/vtn1));
	if(l < 0.0) return 0;
	else if(l > 1.0) return 0;
	if(k < 0.0) return 0;
	else if(k > 1.0) return 0;
	Target.x = (long) (x1+l*v1);
	Target.y = (long) (x2+l*v2);
	return 1;
}

double CAVector::AngleSeg(double x1,double x2, double y1,double y2, double z1,double z2,double w1, double w2) {
	CPoint Int; 
	if (Intersection(x1, x2, y1, y2, z1, z2, w1, w2, Int))
		return AngleXYZ(Int.x,Int.y,x1,x2,z1,z2);
	else return -1;
}

BOOL CAVector::GetShowDistance(void) const {
	return m_ShowDistance;
}

BOOL CAVector::SetShowDistance(BOOL Value) {
	if (m_ShowDistance != Value) {
		m_ShowDistance = Value;
		return TRUE;
	} else return FALSE;
}

CPoint CAVector::GetCenter(void) const {
	return CPoint((long)(m_Bound.left + m_Bound.Width()/2), (long) (m_Bound.top + m_Bound.Height()/2));
}

BOOL CAVector::Rotate(float m_angle) {
	CPoint m_OldCenter = GetCenter();
	m_angle = (float) (M_PI * m_angle / 180);	
	float cosine = (float)cos(-m_angle);
	float sine = (float)sin(-m_angle);

	double s_pow = pow(cosine,2)+pow(sine,2);
	if (!s_pow) return FALSE;

	int nHeight = m_Bound.Height();
	int nWidth = m_Bound.Width();
	
	int x1 = (int)(-nHeight * sine);	
	int y1 = (int)(nHeight * cosine);
	int x2 = (int)(nWidth * cosine - nHeight * sine);
	int y2 = (int)(nHeight * cosine + nWidth * sine);
	int x3 = (int)(nWidth * cosine);	
	int y3 = (int)(nWidth * sine);
	
	int minx = min(0,min(x1, min(x2,x3)));	
	int miny = min(0,min(y1, min(y2,y3)));
	int maxx = max(0,max(x1, max(x2,x3)));	
	int maxy = max(0,max(y1, max(y2,y3)));	

	for (int i=0;i<m_Points.GetSize();i++){
		// rotate m_Points[i]				
		int sourcex = m_Points[i].x - m_Bound.left;
		int sourcey = m_Points[i].y - m_Bound.top;
		int newx = m_Bound.left + (int)((-(pow(cosine,2)*minx)-minx*pow(sine,2)+cosine*sourcex-sine*sourcey)/s_pow);
		int newy = m_Bound.top + (int)((-(pow(cosine,2)*miny)-miny*pow(sine,2)+sine*sourcex+cosine*sourcey)/s_pow);
		m_Points[i] = CPoint(newx, newy);			
	}
	m_Bound = CalcBound();
	CPoint m_NewCenter = GetCenter();
	int dx = m_NewCenter.x - m_OldCenter.x;
	int dy = m_NewCenter.y - m_OldCenter.y;
	for (int j=0;j<m_Points.GetSize();j++){		
		m_Points[j] = CPoint(m_Points[j].x - dx, m_Points[j].y - dy);
	}
	m_Bound = CalcBound();
	return TRUE;
}

BOOL CAVector::Rotate(const CPoint& APoint){
	float m_Angle=(float)AngleXYZ(GetCenter(), APoint, m_SelPoint); 
	m_SelPoint = APoint;
	return Rotate(m_Angle);
}

BOOL CAVector::RotateSelect(const CPoint& APoint){
	m_SelInitPoint = APoint;
	return TRUE;
}


int CAVector::GetDistance(void) const {
	return m_Distance;
}

BOOL CAVector::SetSelsize(int Value) {
	if (Value != m_Sel) {
		m_Sel = Value;
		return TRUE;
	} else return FALSE;
}

int CAVector::GetSelsize(void) const {
	return m_Sel;
}
	

double CAVector::Distance(const CPoint& First, const CPoint& Second) {
	int d1 = Second.y - First.y; d1 *= d1;
	int d2 = Second.x - First.x; d2 *= d2;
	return (double) sqrt(d1 + d2);
}

void CAVector::SortY(int l, int r) {
  int i = l;
  int j = r;
  int pivot = m_Points[(l+r)/2].y; 
  while (i<=j) {
    while(m_Points[i].y>pivot) i++;
    while(m_Points[j].y<pivot) j--;
    if (i<=j){
      CPoint t = m_Points[i];
      m_Points[i] = m_Points[j];
      m_Points[j] = t;
      i++;
      j--;
    }
  }
  if (l < j) SortY(l, j);
  if (i < r) SortY(i, r);
}

void CAVector::SortY(void){
	SortY(0, m_Points.GetSize()-1);

}

void CAVector::SortDistance(void) {
	double Min, Dist;
	int Pos;
	for (int i=0;i<m_Points.GetSize()-1;i++){
		Min = CalcDistance(i, i+1);
		Pos = i+1;
		for (int j=i+1;j<m_Points.GetSize()-1;j++) {
			Dist = CalcDistance(i, j);
			if (Dist > Min) {
				Dist = Min;
				Pos = j;
			}
		}
		if (Pos != i+1) Exchange(m_Points[j], m_Points[i+1]);	
	}		
}

CAVector& CAVector::RemoveAll(void) {
	m_Points.RemoveAll();
	return (* this);
}

void CAVector::CreateFont(CDC * pDC, CRedimension * Redimension) {
	m_Font.DeleteObject();
	HDC hDC;
	POINT pt;
	if (pDC != NULL) hDC = pDC->m_hAttribDC; else hDC = ::GetDC(NULL);	
	pt.y = 12; if (pDC && pDC->IsPrinting()) pt.y = (int) (pt.y * Redimension->tFontScale);
	m_Font.CreateFont(pt.y, 0, 0, 0, FW_BOLD, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
					  CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Helvetica");
}
