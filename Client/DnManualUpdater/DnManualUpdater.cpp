// DnManualUpdater.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "DnManualUpdater.h"
#include "DnManualUpdaterDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDnManualUpdaterApp

BEGIN_MESSAGE_MAP(CDnManualUpdaterApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CDnManualUpdaterApp construction

CDnManualUpdaterApp::CDnManualUpdaterApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CDnManualUpdaterApp object

CDnManualUpdaterApp theApp;


// CDnManualUpdaterApp initialization

BOOL CDnManualUpdaterApp::InitInstance()
{
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

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));


	CString szArg = AfxGetApp()->m_lpCmdLine;
	if( wcsstr( szArg.GetBuffer(), L"/s" ) || wcsstr( szArg.GetBuffer(), L"/S" ) )
		g_bSilenceMode = true;

#ifdef USE_SERVER_VERSION_CHECK
	if( wcsstr( szArg.GetBuffer(), L"/passcheckserverversion" ) || wcsstr( szArg.GetBuffer(), L"/PassCheckServerVersion" ) )
		g_bPassCheckVersion = true;
#endif // USE_SERVER_VERSION_CHECK

	CDnManualUpdaterDlg dlg;
	m_pMainWnd = &dlg;
	if( g_bSilenceMode ) {
		dlg.InitializePatchUpdate();
		while( !dlg.IsFinishThread() ) { Sleep(1); }
		m_pMainWnd = NULL;
//		SAFE_DELETE( dlg );
	}
	else dlg.DoModal();


	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
