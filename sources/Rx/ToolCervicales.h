/*
	Rx Cervicales - Projet de Recherche sur la Radiologie Cervicales Fonctionnelle
	Travail de Licence, © Univerité de Genève, 1999, Tous droits reservés
	------------------------------------------------------------------------------
	écrit par Daniel Doubrovkine (doubrov5@cuimail.unige.ch / dblock@vestris.com)
	http://cuiwww.unige.ch/~doubrov5/uni/license
	------------------------------------------------------------------------------
	Ce code est du domaine public. Il est distribué sans aucune garantie.
	*/

#ifndef C_TOOL_CERVICALES_H
#define C_TOOL_CERVICALES_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CRxDoc;

#include "defuns.h"

#include "ToolBase.h"
#include "CDib.h"
#include "AVector.h"
#include <afxtempl.h>

class CToolCervicales : public CToolBase {
private:	
	CAVector * m_FlexionVector;
	CAVector * m_ExtensionVector;
	CDib * m_Flexion;
	CDib * m_Extension;
	CPoint GetNPoint(CPoint& i_Point, int i_Index) const;
	int FindInList(CList<CPoint, CPoint&>& i_List, const CPoint& i_Point) const;
	void AddNeighbors(CDib * m_Dib, CList<CPoint, CPoint&>& i_List, CPoint& i_Point, int);	
	void AssistTraceVector(int Index, double * Result, double);
	PROPERTY(int, ControlPoints);
	CAVector SegmentationGlob(CDib *, CRxDoc * i_Document, CList<CPoint, CPoint&>&); 
	void Segmentation(CDib * m_Dib, CRxDoc * i_Document, POSITION c_Pos, POSITION l_Pos, CList<CPoint, CPoint&>& i_List, int Iteration, CList<CPoint, CPoint&>& Result);
	void TraceContourTRACER(CDib * m_Dib, const CPoint& i_InitialPoint, CList<CPoint, CPoint&>&);
	void RemoveClosePoints(CAVector&, int Tol);
	void RemovePatternPoints(CAVector& Vector);
public:
	void AssistTrace(void);
	void RemoveAll(void);
	BOOL OnRemoveDIB(CDib *);
	BOOL OnRemoveVector(CAVector *);
	BOOL SetFlexion(CDib *);
	BOOL SetExtension(CDib *);
	BOOL SetFlexionVector(CAVector *);
	BOOL SetExtensionVector(CAVector *);
	CDib * GetFlexion(void) const;
	CDib * GetExtension(void) const;
	CAVector * GetFlexionVector(void) const;
	CAVector * GetExtensionVector(void) const;
	CToolCervicales();
	BOOL IsTracable(void) const;
	BOOL IsAssistable(void) const;
	void Trace(CRxDoc * i_Document);
	virtual ~CToolCervicales();
	void TraceContour(CDib * m_Dib, CRxDoc * i_Document);	
	/*
		properties
		*/
	PROPERTY(CRxDoc *, Document);
};

#endif
