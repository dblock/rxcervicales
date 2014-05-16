/*
	Rx Cervicales - Projet de Recherche sur la Radiologie Cervicales Fonctionnelle
	Travail de Licence, © Univerité de Genève, 1999, Tous droits reservés
	------------------------------------------------------------------------------
	écrit par Daniel Doubrovkine (doubrov5@cuimail.unige.ch / dblock@vestris.com)
	http://cuiwww.unige.ch/~doubrov5/uni/license
	------------------------------------------------------------------------------
	Ce code est du domaine public. Il est distribué sans aucune garantie.
	*/

#ifndef C_ATRACE_REPORT
#define C_ATRACE_REPORT

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "defuns.h"
#include "ToolCervicales.h"

class CATraceReport : public CDialog {
private:
	CSize m_sizeTotal;
	CDC * m_pMemDC;  //A memory device context compatible with our printer DC.
	CRect m_rect;    //To hold the dimensions of our printing area while scaling.
	CBitmap * m_pBm; //Capture the screen image as a Bitmap	
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrint(CDC * pDC, CPrintInfo * pInfo = NULL);		
	PROPERTY(int, VectorSize);	
	PROPERTY(CToolCervicales *, ToolCervicales);
public:	
	double * m_Angles;
	double * m_FlexDistances;
	double * m_ExtDistances;
	void Allocate(int);
	CATraceReport(CWnd* pParent = NULL);   // standard constructor
	virtual ~CATraceReport(void);

// Dialog Data
	//{{AFX_DATA(CATraceReport)
	enum { IDD = IDD_ATRACE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CATraceReport)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CATraceReport)
	afx_msg void OnAtrCopy();
	afx_msg void OnAtprint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif
