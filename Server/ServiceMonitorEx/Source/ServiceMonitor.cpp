// ServiceMonitor.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "ServiceMonitor.h"
#include "MainFrm.h"

#include "MainDoc.h"
#include "MainView.h"

#include "ConfigMngr.h"
#include "ServiceInfo.h"
#include "ErrorLogMngr.h"
#include "NetConnection.h"

#include "BasePartitionView.h"

#include "ConnectionInfoMngr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// SERVICEMONITORCONFIG

SERVICEMONITORCONFIG::SERVICEMONITORCONFIG()
{
	Reset();
}

VOID SERVICEMONITORCONFIG::Reset()
{
	m_ServiceManagerIpAddr = 0;
	m_ServiceManagerPortNo = 0;
	
}


// CServiceMonitorApp

BEGIN_MESSAGE_MAP(CServiceMonitorApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CServiceMonitorApp::OnAppAbout)
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, &CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinApp::OnFileOpen)
END_MESSAGE_MAP()


// CServiceMonitorApp construction

CServiceMonitorApp::CServiceMonitorApp()
{
	// TODO: add construction code here,
	m_AppDoc = NULL;
	m_Lock.Open ();
}

CServiceMonitorApp::~CServiceMonitorApp()
{
	m_Lock.Close ();
}


// The one and only CServiceMonitorApp object

CServiceMonitorApp theApp;

ULONG_PTR g_GdiPlusToken;	// GDI+


// CServiceMonitorApp initialization

BOOL CServiceMonitorApp::InitInstance()
{
	SetMiniDump();

	// Configuration Manager Initialize
	{
		DWORD aRetVal = CConfigMngr::GetInstancePtr()->Open(DF_CONFIGFILE_NAME);
		if (NOERROR != aRetVal) {
			BASE_RETURN(FALSE);
		}
	}

	// Connection Info Initialize
	CConnectionInfoMngr::GetInstancePtr()->LoadData();
	
	// GDI+ Initialize
	{
		GdiplusStartupInput gdiplusStartupInput;
		if (::GdiplusStartup(&g_GdiPlusToken, &gdiplusStartupInput, NULL) != Ok)
		{
			AfxMessageBox(_T("ERROR: Failed to initialize GDI+ library!"));
			return FALSE;
		}
	}

	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));
	LoadStdProfileSettings(4);  // Load standard INI file options (including MRU)
	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CMainDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CMainView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);



	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);


	// Dispatch commands specified on the command line.  Will return FALSE if
	// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// Load Basic Information

	// Error Log Manager Initialize
	{
		DWORD aRetVal = CErrorLogMngr::GetInstancePtr()->Open();
		if (NOERROR != aRetVal) {
			BASE_RETURN(FALSE);
		}
	}


	// The one and only window has been initialized, so show and update it
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	// call DragAcceptFiles only if there's a suffix
	//  In an SDI app, this should occur after ProcessShellCommand
	return TRUE;
}



// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
protected:
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
public:
	afx_msg void OnBnClickedOk();
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

void CAboutDlg::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CDialog::OnActivate(nState, pWndOther, bMinimized);

	GetDlgItem(IDC_STATIC_VERSION)->SetWindowText(CVarArg<MAX_PATH>(_T("%d.%d.%d"), g_SMBuildVersionMajor, g_SMBuildVersionMinor, g_SMBuildVersionRevision));

	GetDlgItem(IDC_STATIC_DATE)->SetWindowText(CVarArg<MAX_PATH>(_T("%04d.%02d.%02d"), g_SMBuildTimeYear, g_SMBuildTimeMonth, g_SMBuildTimeDay));
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
  ON_WM_ACTIVATE()
  ON_BN_CLICKED(IDOK, &CAboutDlg::OnBnClickedOk)
END_MESSAGE_MAP()

// App command to run the dialog
void CServiceMonitorApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


void CServiceMonitorApp::SetAppDoc(CDocument* pDoc)
{
	ASSERT(NULL != pDoc);

	m_AppDoc = pDoc;
}



CDocument* CServiceMonitorApp::GetAppDoc()
{
	return(m_AppDoc);
}

void CServiceMonitorApp::SetMainFrame (CMainFrame* pWnd)
{
	m_MainWnd = pWnd;
}

CMainFrame* CServiceMonitorApp::GetMainFrame ()
{
	return m_MainWnd;
}

CConnectionDialog& CServiceMonitorApp::GetConnectionDialog ()
{
	return static_cast<CMainFrame*>(m_MainWnd)->GetConnectionDialog();
}

void CServiceMonitorApp::SetPartitionView(ULONG nConID, CBasePartitionView* pView)
{
	CLockAutoEx<CCriticalSection> AutoLock (m_Lock);
	m_MapPartitionView[nConID] = pView;
}

CBasePartitionView* CServiceMonitorApp::GetPartitionView(ULONG nConID)
{
	CLockAutoEx<CCriticalSection> AutoLock (m_Lock);

	std::map<ULONG, CBasePartitionView*>::iterator iter = m_MapPartitionView.find(nConID);

	if (iter != m_MapPartitionView.end())
		return iter->second;

	return NULL;	
}

CBaseMonitorView* CServiceMonitorApp::GetMoniterView (ULONG nConID)
{
	CBasePartitionView* pPartitionView = GetPartitionView(nConID);
	if (pPartitionView)
		return pPartitionView->GetMoniterView();

	return NULL;
}

CBaseInformationView* CServiceMonitorApp::GetInformationView (ULONG nConID)
{
	CBasePartitionView* pPartitionView = GetPartitionView(nConID);
	if (pPartitionView)
		return pPartitionView->GetInformationView();

	return NULL;
}

CBaseErrorLogView* CServiceMonitorApp::GetErrorLogView (ULONG nConID)
{
	CBasePartitionView* pPartitionView = GetPartitionView(nConID);
	if (pPartitionView)
		return pPartitionView->GetErrorLogView();

	return NULL;
}

void CServiceMonitorApp::ForceAllTabViewActivate()
{
	CLockAutoEx<CCriticalSection> AutoLock (m_Lock);

	for(std::map<ULONG, CBasePartitionView*>::iterator iter = m_MapPartitionView.begin(); iter!=m_MapPartitionView.end(); iter++ )
		iter->second->ForceAllTabViewActivate();
}



// CServiceMonitorApp message handlers


int CServiceMonitorApp::ExitInstance()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	::GdiplusShutdown(g_GdiPlusToken);

	return CWinApp::ExitInstance();
}

void CAboutDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnOK();
}
