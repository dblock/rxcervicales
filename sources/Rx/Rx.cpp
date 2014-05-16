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

#include "MainFrm.h"
#include "ChildFrm.h"
#include "RxDoc.h"
#include "RxView.h"
#include "DibInformation.h"
#include <atlbase.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define __RX_VERSION__ 0

BEGIN_MESSAGE_MAP(CRxApp, CWinApp)
	//{{AFX_MSG_MAP(CRxApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRxApp construction

CRxApp::CRxApp(){
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CRxApp object

CRxApp theApp;

BOOL CRxApp::InitInstance(){
	m_StatusShown = 0;
	// define global operating system values, create information strings		
	CheckOperatingSystem();
	MakeRxString();
	// construct the initialization window and show it		
	CDialog InitDialog;
	InitDialog.Create(IDD_INIT);
	InitDialog.SetDlgItemText(IDC_SYSINFO, GetVersionString());
	CString l_Initializing = "Initializing " + theApp.GetRxString() + ", please be patient...";
	InitDialog.SetDlgItemText(IDC_RXVERSION, l_Initializing);	
	InitDialog.ShowWindow(m_nCmdShow);
	InitDialog.UpdateWindow();	
	// Standard AFX initialization		
	AfxEnableControlContainer();	
#ifdef _AFXDLL
	Enable3dControls();			// MFC in a shared DLL
#else
	Enable3dControlsStatic();	// linking to MFC statically
#endif
	SetRegistryKey(_T("University of Geneva"));
	LoadStdProfileSettings();  // Load standard INI file options (including MRU)
	// Register document templates		
	p_DocTemplate = new CMultiDocTemplate(
		IDR_RXTYPE,
		RUNTIME_CLASS(CRxDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CRxView));
	AddDocTemplate(p_DocTemplate);

	p_InfoTemplate = new CMultiDocTemplate(
		IDR_RXTYPE,
		RUNTIME_CLASS(CRxDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CDibInformation));	

	// create main MDI Frame window		
	CMainFrame* pMainFrame = new CMainFrame;	
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME)) return FALSE;
	m_pMainWnd = pMainFrame;	
	// Enable drag/drop open		
	m_pMainWnd->DragAcceptFiles();
	// Enable DDE Execute open		
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);
	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo)) return FALSE;	
	pMainFrame->ActivateFrame(m_nCmdShow);
	//pMainFrame->ShowWindow(m_nCmdShow);
	//pMainFrame->UpdateWindow();
	return TRUE;
}


class CAboutDlg : public CDialog {
public:
	CAboutDlg();
// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();		
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD) {
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT	
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	ON_WM_TIMER()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CRxApp::OnAppAbout(){
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CRxApp message handlers


void CRxApp::ShowError(int ErrorCode) {
	LPVOID lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | 
			      FORMAT_MESSAGE_FROM_SYSTEM |     
				  FORMAT_MESSAGE_IGNORE_INSERTS,    
				  NULL,
				  (ErrorCode?ErrorCode:GetLastError()),
				  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
				  (LPTSTR) &lpMsgBuf,    0,    NULL );// Process any inserts in lpMsgBuf.
	MessageBox(::GetFocus(), (LPCTSTR)lpMsgBuf, "Error", MB_OK | MB_ICONINFORMATION);
	LocalFree(lpMsgBuf);
}

CString CRxApp::QueryRegKeyString(HKEY hKeyGroup, LPCTSTR lpszParent, LPCTSTR lpszKey) {
	CRegKey Parent;
	if (Parent.Open(hKeyGroup, lpszParent, KEY_QUERY_VALUE) != ERROR_SUCCESS) return "";
	LPTSTR szValue = NULL;
	DWORD pdwCount = 1;
	if (Parent.QueryValue(szValue, lpszKey, &pdwCount) != ERROR_SUCCESS) return "";
	szValue = (char *) malloc (sizeof(char) * (pdwCount + 1));
	if (Parent.QueryValue(szValue, lpszKey, &pdwCount) != ERROR_SUCCESS) return "";		
	Parent.Close();
	CString Result = szValue;	
	free(szValue);
	return Result;
}

int CRxApp::QueryRegKeyDWord(HKEY hKeyGroup, LPCTSTR lpszParent, LPCTSTR lpszKey) {
	CRegKey Parent;
	if (Parent.Open(hKeyGroup, lpszParent, KEY_QUERY_VALUE) != ERROR_SUCCESS) {		
		return 0;
	}
	DWORD szValue = 0;	
	if (Parent.QueryValue(szValue, lpszKey) != ERROR_SUCCESS) return 0;	
	Parent.Close();	
	return szValue;
}


BOOL CRxApp::IsWindows95(void) const {
	return m_Windows95;
}

CString CRxApp::GetVersionString(void) const {
	return m_VersionString;
}

CString CRxApp::GetRxString(void) const {
	return m_RxString;
}

CString CRxApp::GetComputerName(void) const {
	return m_ComputerName;
}

CString CRxApp::GetMemoryStatus(void) const {
	MEMORYSTATUS l_MemStatus;
	l_MemStatus.dwLength = sizeof(MEMORYSTATUS);
	GlobalMemoryStatus(&l_MemStatus);
	CString l_MemStatusResult;
	l_MemStatusResult.Format("Total memory: %d Kb (physical), %d Kb (page file)\nFree memory: %d Kb (physical), %d Kb (page file)", 
		l_MemStatus.dwTotalPhys/1024,
		l_MemStatus.dwTotalPageFile/1024,
		l_MemStatus.dwAvailPhys/1024,
		l_MemStatus.dwAvailPageFile/1024);   
	return l_MemStatusResult;
}

void CRxApp::MakeRxString(void) {	
	CString m_RxVersion;
	try {
		static char * AMonthsEnglishShort[] = {"Jan","Feb","Mar","Apr", "May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
		char l_month_literal[4];
		int l_month = -1;
		int l_day;
		int l_year;
		sscanf(__DATE__, "%s%d%d", l_month_literal, &l_day, &l_year); // Mmm dd yyyy
		for (int i=0;i<12;i++){
			if (!strcmpi(l_month_literal, AMonthsEnglishShort[i])) {
				l_month = i;
				break;
			}
		}
		if (l_month >= 0) {
			m_RxVersion.Format("%d.%.02d%.02d", __RX_VERSION__, l_month + 1, l_day);
		} else m_RxVersion.Format("%d.x", __RX_VERSION__);
	} catch (...) {
		m_RxVersion.Format("%d.x", __RX_VERSION__);
	}
	
	m_RxString = "Rx ";
	m_RxString = m_RxString + m_RxVersion + " (compiled ";
	m_RxString = m_RxString + __DATE__;	
	m_RxString = m_RxString + " / MSVC++)";	
}

void CRxApp::MakeComputerName(void) {
	unsigned long l_ComputerNameLength = MAX_COMPUTERNAME_LENGTH;
	char * l_ComputerName = (char *) malloc (sizeof(char) * (l_ComputerNameLength + 1));
	if (::GetComputerName(l_ComputerName, &l_ComputerNameLength)) m_ComputerName = l_ComputerName;	
	free(l_ComputerName);
	m_ComputerName.TrimLeft();
	if (!m_ComputerName.GetLength()) m_ComputerName = "This computer";
}

void CRxApp::CheckOperatingSystem(void) {
	MakeComputerName();	
	OSVERSIONINFO l_VersionInfo;
	l_VersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	if (!GetVersionEx(&l_VersionInfo)) return;	
	SYSTEM_INFO l_SystemInfo;
	GetSystemInfo(&l_SystemInfo);
	m_VersionString.Format("%s is running on %d", m_ComputerName, l_SystemInfo.dwNumberOfProcessors);
	switch (l_SystemInfo.wProcessorArchitecture) {
      case 0:
		  switch(l_SystemInfo.wProcessorLevel) {
			case 3: case 386: m_VersionString = m_VersionString + " Intel 386 processor"; break;
			case 4: case 486: m_VersionString = m_VersionString + " Intel 486 processor"; break;
			case 5: case 586: m_VersionString = m_VersionString + " Intel Pentium processor"; break;
			default:
				switch(l_SystemInfo.dwProcessorType) {
				case 1: case 386: m_VersionString = m_VersionString + " Intel 386 processor"; break;
				case 2: case 486: m_VersionString = m_VersionString + " Intel 486 processor"; break;
				case 3: case 586: m_VersionString = m_VersionString + " Intel Pentium processor"; break;
				default: m_VersionString = m_VersionString + " Intel processor";
				};
			break;
		  };
		  break;
      case 1: 
		  switch(l_SystemInfo.wProcessorLevel){
		  case 4: m_VersionString = m_VersionString + " Mips R4000 processor"; break;
		  default: m_VersionString = m_VersionString + " Mips processor"; break;
		  };
		  break;
	case 2: 
		switch(l_SystemInfo.wProcessorLevel) {
        case 21064: m_VersionString = m_VersionString + " Alpha 21064 processor"; break;
        case 21066: m_VersionString = m_VersionString + " Alpha 21066 processor"; break;
        case 21164: m_VersionString = m_VersionString + " Alpha 21164 processor"; break;
        default: m_VersionString = m_VersionString + " Alpha processor"; break;
		};
		break;
    case 3: 
		switch(l_SystemInfo.wProcessorLevel) {
         case 1: m_VersionString = m_VersionString + " PowerPC 601 processor"; break;
         case 3: m_VersionString = m_VersionString + " PowerPC 603 processor"; break;
         case 4: m_VersionString = m_VersionString + " PowerPC 604 processor"; break;
         case 6: m_VersionString = m_VersionString + " PowerPC 603+ processor"; break;
         case 9: m_VersionString = m_VersionString + " PowerPC 604+ processor"; break;
         case 20: m_VersionString = m_VersionString + " PowerPC 620 processor"; break;
         default: m_VersionString=m_VersionString + " PowerPC processor"; break;
		};
		break;
	};
   if (l_SystemInfo.dwNumberOfProcessors > 1) m_VersionString = m_VersionString + 's';


   if (l_VersionInfo.dwPlatformId == 2) {
	   for (unsigned int i=0;i<l_SystemInfo.dwNumberOfProcessors;i++) {
		   CString pRegID; pRegID.Format("HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\%d", i);
		   m_VersionString = m_VersionString + "\n" + QueryRegKeyString(HKEY_LOCAL_MACHINE, pRegID, "VendorIdentifier");
		   m_VersionString = m_VersionString + " " + QueryRegKeyString(HKEY_LOCAL_MACHINE, pRegID, "Identifier");
		   int mhZ = QueryRegKeyDWord(HKEY_LOCAL_MACHINE, pRegID, "~Mhz");
		   if (mhZ) {
			   CString tMhZ; tMhZ.Format("%d", mhZ);
			   m_VersionString = m_VersionString + " at " + tMhZ + " Mhz.";
		   }
	   }
   }

   m_VersionString = m_VersionString + "\nunder";
   m_Windows95 = FALSE; 
   switch (l_VersionInfo.dwPlatformId) {
      case 0: m_VersionString = m_VersionString + " Windows 3.1x "; break;
      case 1: m_Windows95 = TRUE; m_VersionString = m_VersionString + " Windows 95 "; break;
      case 2: m_VersionString = m_VersionString + " Windows NT "; break;
	  default: m_VersionString = m_VersionString + " an Unsupported Platform "; break;
   }
	
   CString l_OSVersion;
   l_OSVersion.Format("%d.%d (build %d)", l_VersionInfo.dwMajorVersion, l_VersionInfo.dwMinorVersion, l_VersionInfo.dwBuildNumber & 0xFFFF);
   m_VersionString = m_VersionString + l_OSVersion + "\n" + GetMemoryStatus();
}

BOOL CAboutDlg::OnInitDialog() {
	CDialog::OnInitDialog();
	CDialog::SetDlgItemText(IDC_SYSINFO, theApp.GetVersionString());
	CDialog::SetDlgItemText(IDC_RXVERSION, theApp.GetRxString());
	return TRUE;
}

void CRxApp::ShowProgress(CString Msg){
	try {
	if (m_StatusShown == 0) {
		m_ProgressDialog.Create(IDD_PROGRESS);	
		m_ProgressDialog.SetDlgItemText(IDC_PROGRESS, Msg);
		m_ProgressDialog.ShowWindow(m_nCmdShow);	
		m_ProgressDialog.UpdateWindow();
	} else {
		m_ProgressDialog.SetDlgItemText(IDC_PROGRESS, Msg);
		m_ProgressDialog.UpdateWindow();
	}
	m_StatusShown++;
	} catch (...) {
		m_StatusShown = 0;
		ShowProgress(Msg);
	}
}

void CRxApp::HideProgress(void){
	try {
		m_StatusShown--;
		if (!m_StatusShown) m_ProgressDialog.DestroyWindow();
	} catch (...) {
	}
}

void CRxApp::SetProgress(CString Value) {
	if (m_StatusShown) {
		m_ProgressDialog.CDialog::SetDlgItemText(IDC_PROGRESS, Value);
		m_ProgressDialog.UpdateWindow();
	}
}

void CRxApp::SetProgress(int Value){
	if (m_StatusShown) {
		((CProgressCtrl *) m_ProgressDialog.GetDlgItem(IDC_PROGRESSBAR))->SetPos(Value);	
		m_ProgressDialog.UpdateWindow();
	}
	
}
