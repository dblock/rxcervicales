/*
	Rx Cervicales - Projet de Recherche sur la Radiologie Cervicales Fonctionnelle
	Travail de Licence, © Univerité de Genève, 1999, Tous droits reservés
	------------------------------------------------------------------------------
	écrit par Daniel Doubrovkine (doubrov5@cuimail.unige.ch / dblock@vestris.com)
	http://cuiwww.unige.ch/~doubrov5/uni/license
	------------------------------------------------------------------------------
	Ce code est du domaine public. Il est distribué sans aucune garantie.
	*/

#if !defined(AFX_RX_H__F481A563_ABBA_11D2_9BFD_000001352962__INCLUDED_)
#define AFX_RX_H__F481A563_ABBA_11D2_9BFD_000001352962__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

class CRxApp : public CWinApp {
private:
	int m_StatusShown;
	CDialog m_ProgressDialog;
	CString m_RxString;
	CString m_ComputerName;
	CString m_VersionString;
	BOOL m_Windows95;
	void CheckOperatingSystem(void);
	void MakeComputerName(void);
	void MakeRxString(void);
	static CString QueryRegKeyString(HKEY hKeyGroup, LPCTSTR lpszParent, LPCTSTR lpszKey);
	static int QueryRegKeyDWord(HKEY hKeyGroup, LPCTSTR lpszParent, LPCTSTR lpszKey);
public:	
	CMultiDocTemplate * p_DocTemplate;
	CMultiDocTemplate * p_InfoTemplate;
	CRxApp();
	static void ShowError(int ErrorCode = 0);
	BOOL IsWindows95(void) const;
	CString GetVersionString(void) const;
	CString GetComputerName(void) const;
	CString GetMemoryStatus(void) const;
	CString GetRxString(void) const;
	// progress
	void ShowProgress(CString Msg = "Working, please wait.");
	void HideProgress(void);
	void SetProgress(int Value);
	void SetProgress(CString Value);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRxApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CRxApp)
	afx_msg void OnAppAbout();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RX_H__F481A563_ABBA_11D2_9BFD_000001352962__INCLUDED_)
