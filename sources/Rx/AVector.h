/*
	Rx Cervicales - Projet de Recherche sur la Radiologie Cervicales Fonctionnelle
	Travail de Licence, © Univerité de Genève, 1999, Tous droits reservés
	------------------------------------------------------------------------------
	écrit par Daniel Doubrovkine (doubrov5@cuimail.unige.ch / dblock@vestris.com)
	http://cuiwww.unige.ch/~doubrov5/uni/license
	------------------------------------------------------------------------------
	Ce code est du domaine public. Il est distribué sans aucune garantie.
	*/

#ifndef C_AVECTOR_H
#define C_AVECTOR_H

class CAVectorManager;
#include "defuns.h"
class CRxView;

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef M_PI
#define M_PI (double) 3.14159265358979
#endif

#include <afxtempl.h>
#include "VectorObject.h"

class CAVector : public CVectorObject {
private:	
	void CreateFont(CDC * pDC = NULL, CRedimension * = NULL);
	void Exchange(CPoint& Pt1, CPoint& Pt2);		
	CPen m_Pen;
	//CBrush m_Brush;
	void AdjustShiftPoint(CPoint&);
	CArray<CPoint, CPoint&> m_Points;	
	CArray<int, int> m_Anchors;	
	int m_Sel;	
	CRect Anchor(int);
	CPoint m_SelInitPoint;		
	BOOL m_ShowDistance;
	int m_Distance;
	void SortY(int l, int r);
public:
	void RecalcDistance(void);
	void SortDistance(void);
	void SortY(void);	
	int CalcDistance(int i1, int i2) const;
	int GetDistance(void) const;
	CPoint GetCenter(void) const;
	BOOL GetShowDistance(void) const;
	BOOL SetShowDistance(BOOL Value = TRUE);
	char * m_Cursor;
	BOOL EndMove(void);
	BOOL OnMouseMove(const CPoint& APoint);
	void Serialize(CArchive&);
	BOOL MouseMove(const CPoint&);	
	static double Distance(double, double, double, double, double, double);
	static double Distance(const CPoint&, const CPoint&, const CPoint&);
	static double Distance(const CPoint&, const CPoint&);
	BOOL Select(const CPoint&);		
	BOOL CanSelect(const CPoint&);
	BOOL Intersects(const CPoint&);
	BOOL Intersects(const CPoint&, const CPoint&, const CPoint&);		
	void Paint(CDC * pDC, const CRedimension&, BOOL m_ShowAngles);
	void Paint(CDC * pDC, const CRedimension&);
	void PaintIntersection(CDC * pDC, const CRedimension&, CAVector& Second);
	CAVector& Clone(const CAVector&);
	CAVector();
	CAVector(const CAVector&);
	CAVector& Remove(int);
	CAVector& RemoveAll(void);
	CAVector& Add(int, int);
	CAVector& Add(const CPoint& Point);
	CAVector& operator+=(const CPoint& Point);
	CAVector& operator=(const CAVector&);
	int operator>(const CAVector&);
	int operator<(const CAVector&);
	int operator>=(const CAVector&);
	int operator<=(const CAVector&);
	int operator==(const CAVector&);
	int GetSize(void) const;
	BOOL SetSelsize(int Value);
	int GetSelsize(void) const;
	virtual ~CAVector();
	CPoint GetAt(int) const;
	CPoint operator[](int);	
	CRect CalcBound();
	BOOL SetColor(COLORREF);
	COLORREF GetColor();
	BOOL Rotate(const CPoint& APoint);
	BOOL Rotate(float);
	BOOL RotateSelect(const CPoint& APoint);
	static double AngleXYZ(const CPoint& X, const CPoint& Y, const CPoint& Z); // angle in X (from Y to Z)
	static double AngleXYZ(double x1, double x2, double y1, double y2, double z1, double z2); // angle in x (from y to z)
	static double AngleSeg(double x1,double x2, double y1,double y2, double z1,double z2,double w1, double w2); // angle x to z between two segs xy and zw (-1 if no intersection)
	static double AngleSeg(const CPoint& X, const CPoint& Y, const CPoint& Z, const CPoint& W); // angle X to Z between two segs XY and ZW (-1 if no intersection)
	static int Intersection(const CPoint& X, const CPoint& Y, const CPoint& Z, const CPoint& W, CPoint& Target); // intersection between two segs XY and ZW
	static int Intersection(double x1,double x2, double y1,double y2, double z1,double z2,double w1, double w2, CPoint& Target); // intersection between two segs XY and ZW	
	PROPERTY(CAVectorManager *, Parent);
};

#endif
