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
#include "ToolCervicales.h"
#include "AVector.h"
#include "RxDoc.h"
#include "ATraceReport.h"
#include <math.h>

extern CRxApp theApp;

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CToolCervicales::CToolCervicales() {
	m_Flexion = NULL;
	m_Extension = NULL;
	m_Document = NULL;
	m_FlexionVector = NULL;
	m_ExtensionVector = NULL;
	m_ControlPoints = 14;
}

CToolCervicales::~CToolCervicales() {

}

BOOL CToolCervicales::SetFlexionVector(CAVector * Vector) {
	if (m_FlexionVector != Vector) {
		if (Vector && (Vector == m_ExtensionVector)){
			MessageBox(::GetFocus(), "Extension vector must be different from Flexion vector!", "Cervicales Tracer Error", MB_OK | MB_ICONERROR);
			return FALSE;
		}
		if (Vector && (Vector->GetSize() != m_ControlPoints)) {
			MessageBox(::GetFocus(), "Invalid control points count!", "Cervicales Tracer Error", MB_OK | MB_ICONERROR);
			return FALSE;			
		}
		if (m_FlexionVector) m_FlexionVector->SetColor((COLORREF) 0);
		m_FlexionVector = Vector;
		if (m_FlexionVector) m_FlexionVector->SetColor((COLORREF) 255);
		return TRUE;
	} else return FALSE;
}

BOOL CToolCervicales::SetExtensionVector(CAVector * Vector) {
	if (m_ExtensionVector != Vector) {
		if (Vector && (Vector == m_FlexionVector)){
			MessageBox(::GetFocus(), "Extension vector must be different from Flexion vector!", "Cervicales Tracer Error", MB_OK | MB_ICONERROR);
			return FALSE;
		}
		if (Vector && (Vector->GetSize() != m_ControlPoints)) {
			MessageBox(::GetFocus(), "Invalid control points count!", "Cervicales Tracer Error", MB_OK | MB_ICONERROR);
			return FALSE;			
		}
		if (m_ExtensionVector) m_ExtensionVector->SetColor((COLORREF) 0);
		m_ExtensionVector = Vector;
		if (m_ExtensionVector) m_ExtensionVector->SetColor((COLORREF) 255);
		return TRUE;
	} else return FALSE;
}

BOOL CToolCervicales::SetFlexion(CDib * Tracker) {
	if (m_Flexion != Tracker) {
		if (Tracker && (Tracker == m_Extension)) {
			MessageBox(::GetFocus(), "Extension must be different from Flexion!", "Cervicales Tracer Error", MB_OK | MB_ICONERROR);
			return FALSE;
		}
		if (m_Flexion) m_Flexion->SetComment("");
		m_Flexion = Tracker;
		if (m_Flexion) m_Flexion->SetComment("[flexion]");		
		return TRUE;
	} else return FALSE;
}

BOOL CToolCervicales::SetExtension(CDib * Tracker){
	if (m_Extension != Tracker) {		
		if (Tracker && (Tracker == m_Flexion)) {
			MessageBox(::GetFocus(), "Extension must be different from Flexion!", "Cervicales Tracer Error", MB_OK | MB_ICONERROR);
			return FALSE;
		}
		if (m_Extension) m_Extension->SetComment("");
		m_Extension = Tracker;
		if (m_Extension) m_Extension->SetComment("[extension]");
		return TRUE;
	} else return FALSE;
}

CDib * CToolCervicales::GetFlexion(void) const {
	return m_Flexion;
}

CAVector * CToolCervicales::GetFlexionVector(void) const {
	return m_FlexionVector;
}

CAVector * CToolCervicales::GetExtensionVector(void) const {
	return m_ExtensionVector;
}

CDib * CToolCervicales::GetExtension(void) const {
	return m_Extension;
}

BOOL CToolCervicales::OnRemoveVector(CAVector * Vector) {
	BOOL Result = FALSE;
	if (m_FlexionVector == Vector) {
		m_FlexionVector = NULL;
		Result = TRUE;
	}
	if (m_ExtensionVector == Vector) {
		m_ExtensionVector = NULL;
		Result = TRUE;
	}
	return Result;
}

BOOL CToolCervicales::OnRemoveDIB(CDib * Dib) {
	BOOL Result = FALSE;
	if (m_Flexion == Dib) {
		m_Flexion = NULL;
		Result = TRUE;
	}
	if (m_Extension == Dib) {
		m_Extension = NULL;
		Result = TRUE;
	}
	return Result;
}

void CToolCervicales::RemoveAll(void) {
	m_Flexion = NULL;
	m_Extension = NULL;
}

BOOL CToolCervicales::IsTracable(void) const {
	return (m_Flexion != NULL) && (m_Extension != NULL);
}

BOOL CToolCervicales::IsAssistable(void) const {
	return (m_FlexionVector != NULL) && (m_ExtensionVector != NULL);
}

void CToolCervicales::Trace(CRxDoc * i_Document) {
	if (!IsTracable()) return;
	theApp.ShowProgress("Initializing cervicales angle tracer.");
	CDib m_DibFlexion; m_DibFlexion = * m_Flexion;
	CDib m_DibExtension; m_DibExtension = * m_Extension;
	m_DibFlexion.MedianFilter(2, 2);
	m_DibExtension.MedianFilter(2, 2);
	m_DibFlexion.CnvMonochrome();
	m_DibExtension.CnvMonochrome();
	theApp.SetProgress("Segmentating, please wait.");	
	TraceContour(&m_DibFlexion, i_Document);
	m_FlexionVector = i_Document->GetTailVector();
	TraceContour(&m_DibExtension, i_Document);
	m_ExtensionVector = i_Document->GetTailVector();
	AssistTrace();
	theApp.HideProgress();
}

void CToolCervicales::AddNeighbors(CDib * m_Dib, CList<CPoint, CPoint&>& i_List, CPoint& i_Point, int i_Direction){
	i_List.AddTail(i_Point);
	CPoint n_Point;
	for (int i=0;i<8;i+=2){
		if ((i + 4 == i_Direction)||(i - 4 == i_Direction)) continue;
		n_Point = GetNPoint(i_Point, i);
		if ((n_Point.x >= 0)&&(n_Point.x < m_Dib->GetWidth())) {
			if ((n_Point.y >= 0)&&(n_Point.y < m_Dib->GetHeight())) {
				if ((m_Dib->GetPixel(n_Point.x, n_Point.y) == 0)&&(FindInList(i_List, n_Point) == -1)) {
					AddNeighbors(m_Dib, i_List, n_Point, i);
				}
			}
		}
	}
}

#define VERTEBRE_COUNT 12
#define REAL_VERTEBRE_COUNT 7

void CToolCervicales::Segmentation(CDib * m_Dib, CRxDoc * i_Document, POSITION c_Pos, POSITION l_Pos, CList<CPoint, CPoint&>& i_List, int Iteration, CList<CPoint, CPoint&>& Result) {
	if ((!c_Pos) || (!l_Pos) || (c_Pos == l_Pos)) return;
	POSITION first_Position = c_Pos;
	POSITION last_Position = l_Pos;
	CPoint l_FirstPoint = i_List.GetNext(c_Pos);
	CPoint l_LastPoint = i_List.GetNext(l_Pos);
	/* plot first and last points */
	m_Dib->PutPixel(l_FirstPoint.x, l_FirstPoint.y, (COLORREF) 255);
	m_Dib->PutPixel(l_LastPoint.x, l_LastPoint.y, (COLORREF) 255);
	/* find the maximum error point */
	CPoint l_MaxPoint = l_FirstPoint, l_Point;
	double l_Distance, max_Distance = -1;
	POSITION max_Position = NULL, p_Pos;
	while (c_Pos != NULL) {
		p_Pos = c_Pos;
		l_Point = i_List.GetNext(c_Pos);
		l_Distance = CAVector::Distance(l_FirstPoint, l_LastPoint, l_Point);
		if (l_Distance > max_Distance) {
			l_MaxPoint = l_Point;
			max_Distance = l_Distance;
			max_Position = p_Pos;
		}
		//m_Dib->PutPixel(l_Point.x, l_Point.y, 0);
	}
	if (max_Distance > 1) m_Dib->PutPixel(l_MaxPoint.x, l_MaxPoint.y, (COLORREF) 255);
	else return;
	/*
	CAVector FirstVector; FirstVector.Add(l_FirstPoint); FirstVector.Add(l_MaxPoint); FirstVector.Add(l_LastPoint);
	i_Document->Add(FirstVector);
	*/
	if (Iteration >= 4) {
		return;
	} else if (max_Distance > 0) {
		if ((l_MaxPoint != l_FirstPoint)&&(l_MaxPoint != l_LastPoint)) {
			Segmentation(m_Dib, i_Document, first_Position, max_Position, i_List, Iteration+1, Result);
			Result.AddTail(l_MaxPoint);
			Segmentation(m_Dib, i_Document, max_Position, last_Position, i_List, Iteration+1, Result);
		}
	}
}

void CToolCervicales::RemoveClosePoints(CAVector& Vector, int Tol) {
	CAVector Hector;
	for (int j=0;j<Vector.GetSize();j++) {
		if (Vector.GetSize() <= 4) return;
		BOOL m_Add = TRUE;
		for (int k=0;k<Hector.GetSize();k++) {
			if (CAVector::Distance(Vector[j], Hector[k]) < Tol) {				
				Hector.Add(CPoint((Vector[j].x + Hector[k].x) / 2, (Vector[j].y + Hector[k].y) / 2));
				Hector.Remove(k);
				m_Add = FALSE;
				break;
			}
		}
		if (m_Add) Hector.Add(Vector[j]);
		if (Vector.GetSize() <= 4) break;
	}
	Vector = Hector;		
}

void CToolCervicales::RemovePatternPoints(CAVector& Vector){
	BOOL Removed;
	for (int k=Vector.GetSize()-1;k>=0;k--) {
		// for each existing point do
		Removed = FALSE;
		if (Vector.GetSize() <= 4) break;
		for (int m=0;m<Vector.GetSize();m++) {								
			for (int l=k+1;l<Vector.GetSize();l++){
				if (Vector.GetSize() <= 4) break;
				if ((k!=m)&&(k!=l)) {
					if ((Vector[k].x >= Vector[m].x)&&(Vector[k].y >= Vector[m].y)&&
						(Vector[k].x <= Vector[l].x)&&(Vector[k].y <= Vector[l].y)) {
						Vector.Remove(k);
						Removed = TRUE;
						break;
					}
					if ((Vector[k].x <= Vector[m].x)&&(Vector[k].y <= Vector[m].y)&&
						(Vector[k].x >= Vector[l].x)&&(Vector[k].y >= Vector[l].y)) {
						Vector.Remove(k);
						Removed = TRUE;
						break;
					}
					if ((Vector[k].x >= Vector[m].x)&&(Vector[k].y <= Vector[m].y)&&
						(Vector[k].x <= Vector[l].x)&&(Vector[k].y >= Vector[l].y)) {
						Vector.Remove(k);
						Removed = TRUE;
						break;
					}
					if ((Vector[k].x <= Vector[m].x)&&(Vector[k].y >= Vector[m].y)&&
						(Vector[k].x >= Vector[l].x)&&(Vector[k].y <= Vector[l].y)) {
						Vector.Remove(k);
						Removed = TRUE;
						break;
					}										
				}		
			}
			if (Removed) break;
		}			
	}
}

CAVector CToolCervicales::SegmentationGlob(CDib * m_Dib, CRxDoc * i_Document, CList<CPoint, CPoint&>& i_List) {
	CList<CPoint, CPoint&> Result;
	
	Result.AddTail(i_List.GetHead());
	Segmentation(m_Dib, i_Document, i_List.GetHeadPosition(), i_List.GetTailPosition(), i_List, 0, Result);
	Result.AddTail(i_List.GetTail());

	CAVector Vector; CPoint l_Point;
	POSITION c_Pos = Result.GetHeadPosition();
	while (c_Pos != NULL) {
		l_Point = Result.GetNext(c_Pos);
		Vector.Add(CPoint(l_Point.x, m_Dib->GetHeight()-l_Point.y));
	}
	/* replace very close points by an intermediate value */	
	for (int i=2;i<(int)((double)(m_Dib->GetWidth() + m_Dib->GetHeight())/2 * 0.03);i++) RemoveClosePoints(Vector, i);
	/* remove pattern points */
	RemovePatternPoints(Vector);
	//i_Document->Add(Vector);
	return Vector;
}

void CToolCervicales::TraceContour(CDib * m_Dib, CRxDoc * i_Document) {
	theApp.ShowProgress("Tracing contour, please be patient.");	
	CList<CPoint, CPoint&> c_List;
	CList<CPoint, CPoint&> total_List;
	
	CList<CPoint, CPoint&> full_List[VERTEBRE_COUNT];
	int full_ListCount = 0;
	
	/* locate any first black pixel */
	CSize m_DibSize = m_Dib->GetDimensions();
	CPoint l_Point;
	POSITION c_Pos; 
	int x, y;
	for (x=0;x<m_DibSize.cx;x++) {
		theApp.SetProgress(x * 100 / m_DibSize.cx);
		for (y=0;y<m_DibSize.cy;y++) {
			if (m_Dib->GetPixel(x, y) == 0) {
				l_Point = CPoint(x,y);
				if (FindInList(total_List, l_Point) == -1) {
					c_List.RemoveAll();
					AddNeighbors(m_Dib, c_List, l_Point, 6);
					// copy to full list
					c_Pos = c_List.GetHeadPosition();
					while (c_Pos != NULL) total_List.AddTail(c_List.GetNext(c_Pos));
					// find the smallest list
					int f_Min = full_ListCount;
					if (full_ListCount == VERTEBRE_COUNT) {
						f_Min = 0;
						//double f_Distance=0; for (int l=0;l<full_List[f_Min].GetCount()-1;l++) f_Distance+=CAVector::Distance(full_List[f_Min].GetAt(full_List[f_Min].FindIndex(l)), full_List[f_Min].GetAt(full_List[f_Min].FindIndex(l+1)));
						for (int i=1;i<VERTEBRE_COUNT;i++) {
							//double i_Distance=0;
							//for (int l=0;l<full_List[i].GetCount()-1;l++) i_Distance+=CAVector::Distance(full_List[i].GetAt(full_List[i].FindIndex(l)), full_List[i].GetAt(full_List[i].FindIndex(l+1)));							
							if (full_List[i].GetCount() < full_List[f_Min].GetCount())
							//if (i_Distance < f_Distance)
								f_Min = i;
						}
					} else full_ListCount++; 
					// copy to array of lists
					full_List[f_Min].RemoveAll();
					c_Pos = c_List.GetHeadPosition();
					while (c_Pos != NULL) full_List[f_Min].AddTail(c_List.GetNext(c_Pos));					
					//TraceContourTRACER(m_Dib, l_Point, c_List);
				}
			}
		}
	}
	/* Segmentation */		
	if (full_ListCount >= REAL_VERTEBRE_COUNT) {		
		m_Dib->Cnv24Bit();		
		m_Dib->Invert();
		CAVector AVectors[VERTEBRE_COUNT];
		CAVector RealVectors[REAL_VERTEBRE_COUNT];
		int l, k;
		for (l=0; l<full_ListCount;l++) {
			AVectors[l] = SegmentationGlob(m_Dib, i_Document, full_List[l]);
			AVectors[l].RecalcDistance();
			//break;
			CString Msg; Msg.Format("Segmentation for object %d done.", l);
			theApp.SetProgress(Msg);
		}
		/* remove vectors of minimal length */		
		for (l=0;l<REAL_VERTEBRE_COUNT;l++) {			
			int MaxIndex = 0;
			for (k=1;k<full_ListCount;k++) {
				if (AVectors[k].GetSize() && (AVectors[k].GetDistance() > AVectors[MaxIndex].GetDistance())) MaxIndex = k;
			}
			if (AVectors[MaxIndex].GetSize()){
				//i_Document->Add(AVectors[MaxIndex]);
				RealVectors[l] = AVectors[MaxIndex];
				AVectors[MaxIndex].RemoveAll();
				AVectors[MaxIndex].RecalcDistance();				
			}			
		}
		/* create a vector of leftmost points only */
		CAVector FrontVector;
		int l_Pos, s_Pos;
		for (l=0;l<REAL_VERTEBRE_COUNT;l++) {			
			// find the leftmost point
			l_Pos = 0; for (k=1;k<RealVectors[l].GetSize();k++) if (RealVectors[l].GetAt(k).x < RealVectors[l].GetAt(l_Pos).x) l_Pos = k; 			
			FrontVector.Add(RealVectors[l].GetAt(l_Pos));
			// find the second leftmost point
			s_Pos = 0; for (k=1;k<RealVectors[l].GetSize();k++) if ((RealVectors[l].GetAt(k).x < RealVectors[l].GetAt(s_Pos).x)&&(k!=l_Pos)) s_Pos = k; 
			if (s_Pos != l_Pos) FrontVector.Add(RealVectors[l].GetAt(s_Pos));
		}	
		FrontVector.SortY();
		while (FrontVector.GetSize() < 14) FrontVector.Add(CPoint(FrontVector[FrontVector.GetSize()-1].x + 5, FrontVector[FrontVector.GetSize()-1].y + 5));

		i_Document->Add(FrontVector);		
		
	} else MessageBox(::GetFocus(), "Insufficient trace zones detected!", "Cervicales Tracer Error", MB_OK | MB_ICONERROR);		
	theApp.HideProgress();		
}

CPoint CToolCervicales::GetNPoint(CPoint& i_Point, int i_Index) const {
	switch(i_Index) {
	case 0: return CPoint(i_Point.x+1, i_Point.y);
	case 1: return CPoint(i_Point.x+1, i_Point.y-1);
	case 2: return CPoint(i_Point.x, i_Point.y-1);
	case 3: return CPoint(i_Point.x-1, i_Point.y-1);
	case 4: return CPoint(i_Point.x-1, i_Point.y);
	case 5: return CPoint(i_Point.x-1, i_Point.y+1);
	case 6: return CPoint(i_Point.x, i_Point.y+1);
	case 7: return CPoint(i_Point.x+1, i_Point.y+1);
	default: return i_Point;
	}
}

int CToolCervicales::FindInList(CList<CPoint, CPoint&>& i_List, const CPoint& i_Point) const {
	POSITION i_Pos = i_List.GetHeadPosition();
	while (i_Pos != NULL) {
		if (i_List.GetNext(i_Pos) == i_Point) return (int) i_Pos;
	}	
	return -1;
}

void CToolCervicales::AssistTraceVector(int Index, double * Result, double PrevAngle) {	
	if (Index + 1 >= m_ControlPoints) return;		
	CPoint x1 = m_FlexionVector->GetAt(Index);
	CPoint y1 = m_ExtensionVector->GetAt(Index);
	CPoint x2 = m_FlexionVector->GetAt(Index+1);
	CPoint y2 = m_ExtensionVector->GetAt(Index+1);
	CPoint xc = CPoint((x2.x + x1.x) / 2, (x2.y + x1.y) / 2);
	CPoint yc = CPoint((y2.x + y1.x) / 2, (y2.y + y1.y) / 2);	
	// join centers
	x1.x += (- xc.x + yc.x);
	x2.x += (- xc.x + yc.x);
	x1.y += (- xc.y + yc.y);
	x2.y += (- xc.y + yc.y);
	// calculate angle
	double l_DiffAngle = CAVector::AngleSeg(x1, x2, y1, y2);
	//if (fabs(l_DiffAngle)>180) l_DiffAngle=0;
	l_DiffAngle = fabs(l_DiffAngle);
	/*
	CString Tmp; Tmp.Format("current: %f, previous: %f", l_DiffAngle, PrevAngle);
	MessageBox(0, Tmp, "", MB_OK);
	*/
	Result[Index / 2] = l_DiffAngle-PrevAngle;
	// rotate
	//if (l_DiffAngle != -1) m_ExtensionVector->Rotate((float) l_DiffAngle);
	// calculate difference of next two points
	AssistTraceVector(Index + 2, Result, l_DiffAngle);
}

void CToolCervicales::AssistTrace(void) {
	if (!m_FlexionVector || !m_ExtensionVector) {
		MessageBox(::GetFocus(), "Invalid flexion or extension vector definition.", "Cervicales Tracer", MB_OK);
		return;
	}
	CATraceReport * ATraceDialog = new CATraceReport();
	ATraceDialog->Create(IDD_ATRACE);
	ATraceDialog->Allocate(m_ControlPoints/2);	
	ATraceDialog->SetVectorSize(m_ControlPoints/2);
	CAVector l_ExtensionVector = (* m_ExtensionVector);
	AssistTraceVector(0, ATraceDialog->m_Angles, 0);
	CString AnglesRes, Tmp;
	// extension vector is rotated
	(* m_ExtensionVector) = l_ExtensionVector;
	
	CTreeCtrl * l_Tree = (CTreeCtrl *) ATraceDialog->GetDlgItem(IDC_TRACE_TREE);
	l_Tree->DeleteAllItems();
	for (int i=0;i<m_ControlPoints/2;i++) {
		Tmp.Format("Vertebre %d", i+1);
		HTREEITEM l_Parent = l_Tree->InsertItem(Tmp);		
		if (i) {
			Tmp.Format("Angle between Flex and Ext = %.02f° ", ATraceDialog->m_Angles[i]); 
			l_Tree->InsertItem(Tmp, l_Parent);
		}
		if (i != m_ControlPoints/2-1) {
			ATraceDialog->m_ExtDistances[i] = m_ExtensionVector->CalcDistance(2*i+1, 2*i+2)*m_ExtensionVector->GetParent()->GetMetric();
			ATraceDialog->m_FlexDistances[i] = m_FlexionVector->CalcDistance(2*i+1, 2*i+2)*m_ExtensionVector->GetParent()->GetMetric();
			Tmp.Format("Extension, Distance to vertebre %d = %.02f mm", i+2, ATraceDialog->m_ExtDistances[i]);
			l_Tree->InsertItem(Tmp, l_Parent);
			Tmp.Format("Flexion, Distance to vertebre %d = %.02f mm", i+2, ATraceDialog->m_FlexDistances[i]);
			l_Tree->InsertItem(Tmp, l_Parent);
		}
		l_Tree->Expand(l_Parent, TVE_EXPAND);
	}
	ATraceDialog->ShowWindow(theApp.m_nCmdShow);	
	ATraceDialog->UpdateWindow();	
}