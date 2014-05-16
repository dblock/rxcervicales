#if !defined(AFX_METRICDIALOG_H__960E7038_CEEE_11D2_B49E_0080C8881D5B__INCLUDED_)
#define AFX_METRICDIALOG_H__960E7038_CEEE_11D2_B49E_0080C8881D5B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MetricDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMetricDialog dialog

#include "resource.h"
#include "defuns.h"

class CMetricDialog : public CDialog
{
// Construction
public:
	CMetricDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CMetricDialog)
	enum { IDD = IDD_METRIC };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMetricDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMetricDialog)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnChangeMetricInput();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	PROPERTY(double, Metric);
	PROPERTY(int, Distance);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_METRICDIALOG_H__960E7038_CEEE_11D2_B49E_0080C8881D5B__INCLUDED_)
