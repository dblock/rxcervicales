#ifndef C_DIB_INFORMATION_H
#define C_DIB_INFORMATION_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CDib.h"
#include "BufferView.h"

class CDibInformation : public CBufferView {
private:
	CPen m_GrayPen;
	CFont m_Font;
	void DrawHistogram(CDC *, const CRect& Rect, CDib * i_DIB);
	void DrawHistogramIntegral(CDC * pDC, const CRect& Rect, CDib * i_DIB);
	/*
		Double Buffer
		*/
protected:
	CDibInformation();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CDibInformation)
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDibInformation)
	public:
	virtual void OnInitialUpdate();	
	//}}AFX_VIRTUAL
// Implementation
protected:
	virtual ~CDibInformation();
	virtual void OnDrawPrivate(CDC* pDC);
	// Generated message map functions
protected:
	//{{AFX_MSG(CDibInformation)
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif 
