// MetricDialog.cpp : implementation file
//

#include "stdafx.h"
#include "Rx.h"
#include "MetricDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMetricDialog dialog


CMetricDialog::CMetricDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CMetricDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMetricDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CMetricDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMetricDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMetricDialog, CDialog)
	//{{AFX_MSG_MAP(CMetricDialog)
	ON_EN_CHANGE(IDC_METRIC_INPUT, OnChangeMetricInput)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMetricDialog message handlers

BOOL CMetricDialog::PreCreateWindow(CREATESTRUCT& cs) {
	return CDialog::PreCreateWindow(cs);
}

BOOL CMetricDialog::OnInitDialog() {
	CDialog::OnInitDialog();
	CString l_MetricStr; l_MetricStr.Format("What is the length of this %d pixel vector? [mm]", m_Distance);
	SetDlgItemText(IDC_QMETRIC, l_MetricStr);
	CString l_Metric; l_Metric.Format("%.02f", m_Metric * m_Distance);
	CEdit * l_Edit = (CEdit *) GetDlgItem(IDC_METRIC_INPUT);
	l_Edit->SetWindowText(l_Metric);
	l_Edit->LimitText(10);
	l_Edit->SetSel(0, -1);
	return TRUE;  
}

void CMetricDialog::OnOK() {
	CString l_MetricStr;
	(((CEdit *) GetDlgItem(IDC_METRIC_INPUT))->GetWindowText(l_MetricStr)); 
	SetMetric(atof(l_MetricStr)/GetDistance());	
	CDialog::OnOK();
}

void CMetricDialog::OnChangeMetricInput() {
	CString l_MetricStr;
	(((CEdit *) GetDlgItem(IDC_METRIC_INPUT))->GetWindowText(l_MetricStr)); 
	((CButton *) GetDlgItem(IDOK))->EnableWindow(FALSE);
	for (int i=0;i<l_MetricStr.GetLength();i++)
		if((!isdigit(l_MetricStr[i]))&&(l_MetricStr[i]!='.')){
			return;
		}
	if (l_MetricStr.GetLength()) ((CButton *) GetDlgItem(IDOK))->EnableWindow(TRUE);
}
