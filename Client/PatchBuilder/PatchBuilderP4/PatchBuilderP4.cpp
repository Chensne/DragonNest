// PatchBuilderP4.cpp : 응용 프로그램에 대한 클래스 동작을 정의합니다.
//

#include "stdafx.h"
#include "SundriesFunc.h"
#include "PatchBuilderP4.h"
#include "PatchBuilderP4Dlg.h"
#include "tinyxml.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPatchBuilderP4App

BEGIN_MESSAGE_MAP(CPatchBuilderP4App, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()



// CPatchBuilderP4App 생성
CPatchBuilderP4App::CPatchBuilderP4App()
{
	// TODO: 여기에 생성 코드를 추가합니다.
	// InitInstance에 모든 중요한 초기화 작업을 배치합니다.
}

CPatchBuilderP4App::~CPatchBuilderP4App()
{
	LogWnd::DestroyLog();
}

// 유일한 CPatchBuilderP4App 개체입니다.
CPatchBuilderP4App theApp;

// CPatchBuilderP4App 초기화
BOOL CPatchBuilderP4App::InitInstance()
{
	// 응용 프로그램 매니페스트가 ComCtl32.dll 버전 6 이상을 사용하여 비주얼 스타일을
	// 사용하도록 지정하는 경우, Windows XP 상에서 반드시 InitCommonControlsEx()가 필요합니다.
	// InitCommonControlsEx()를 사용하지 않으면 창을 만들 수 없습니다.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 응용 프로그램에서 사용할 모든 공용 컨트롤 클래스를 포함하도록
	// 이 항목을 설정하십시오.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	m_RegProfile.LoadProfile();

	CPatchBuilderP4Dlg dlg;
	m_pMainWnd = &dlg;

	UseLogFile();

	if( m_RegProfile.GetProfileCount() )
	{
		SProfile *pProfile = m_RegProfile.GetProfile( 0 );
		dlg.m_szP4Url = pProfile->szP4Url.c_str();
		dlg.m_szP4UrlFolder = pProfile->szP4UrlFolder.c_str();
		dlg.m_szP4UrlServer = pProfile->szP4UrlServer.c_str();
		dlg.m_szP4UrlServerFolder = pProfile->szP4UrlServerFolder.c_str();
		dlg.m_szP4Workspace = pProfile->szP4Workspace.c_str();
		dlg.m_szRMakeCmd = pProfile->szRMakeCmd.c_str();
		dlg.m_szManualPatchExe = pProfile->szManualPatchExe.c_str();
		dlg.m_szCountryCode = pProfile->szCountryCode.c_str();
	}
	else
	{
		dlg.m_szP4Url = "10.0.1.40:1666";
		dlg.m_szP4UrlFolder = "//DragonNest/Client/...";
		dlg.m_szP4UrlServer = "10.0.1.40:1666";
		dlg.m_szP4UrlServerFolder = "//DragonNest/Server/...";
		dlg.m_szP4Workspace = "c:\\UpdateFolder";
		dlg.m_szRMakeCmd = "\\\\192.168.0.11\\tooldata\\Utility\\RMakeCmd.exe";
		dlg.m_szCountryCode = "Kor";
		dlg.m_szManualPatchExe = "\\\\192.168.0.11\\tooldata\\Utility\\DnManualUpdater_KR.exe";
	}

	DWORD dwValue;
	GetRegistryNumber( HKEY_CURRENT_USER, REG_SUBKEY, "MakeFullVersion", dwValue );
	if( dwValue ) dlg.m_bMakeFullVersion = TRUE;
	GetRegistryNumber( HKEY_CURRENT_USER, REG_SUBKEY, "MakeServerData", dwValue );
	if( dwValue ) dlg.m_bCopyServerData = TRUE;
	GetRegistryNumber( HKEY_CURRENT_USER, REG_SUBKEY, "RollBack", dwValue );
	if( dwValue ) dlg.m_bRollBack = TRUE;
	GetRegistryNumber( HKEY_CURRENT_USER, REG_SUBKEY, "ManualPatch", dwValue );
	if( dwValue ) dlg.m_bManualPatch = TRUE;

	if( strlen( m_lpCmdLine ) )
	{
		char *pFindPtr;
		pFindPtr = strstr( m_lpCmdLine, "Profile:" );
		if( pFindPtr )
		{
			pFindPtr += strlen( "Profile:" );
			int nProfile = atoi( pFindPtr );
			dlg.m_nDefaultProfile = nProfile;
			dlg.m_bAutoStart = TRUE;
		}
		pFindPtr = strstr( m_lpCmdLine, "/MakeFull:true" );
		if( pFindPtr )
		{
			dlg.m_bMakeFullVersion = TRUE;
		}
		else
		{
			pFindPtr = strstr( m_lpCmdLine, "/MakeFull:false" );
			if( pFindPtr )
			{
				dlg.m_bMakeFullVersion = FALSE;
			}
		}
		pFindPtr = strstr( m_lpCmdLine, "/CopyServerData:true" );
		if( pFindPtr )
		{
			dlg.m_bCopyServerData = TRUE;
		}
		else
		{
			pFindPtr = strstr( m_lpCmdLine, "/CopyServerData:false" );
			if( pFindPtr )
			{
				dlg.m_bCopyServerData = FALSE;
			}
		}

		pFindPtr = strstr( m_lpCmdLine, "/RollBack:true" );
		if( pFindPtr )
		{
			dlg.m_bRollBack = TRUE;
		}
		else
		{
			pFindPtr = strstr( m_lpCmdLine, "/RollBack:false" );
			if( pFindPtr )
			{
				dlg.m_bRollBack = FALSE;
			}
		}

		pFindPtr = strstr( m_lpCmdLine, "/ManualPatch:true" );
		if( pFindPtr )
		{
			dlg.m_bManualPatch = TRUE;
		}
		else
		{
			pFindPtr = strstr( m_lpCmdLine, "/ManualPatch:false" );
			if( pFindPtr )
			{
				dlg.m_bManualPatch = FALSE;
			}
		}
	}

	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
	}
	else if (nResponse == IDCANCEL)
	{
	}

	return FALSE;
}

void CPatchBuilderP4App::UseLogFile()
{
	TCHAR szCurFileName[MAX_PATH]= {0,};
	::GetModuleFileName( NULL, szCurFileName, MAX_PATH );
	TCHAR szDrive[MAX_PATH] = { 0, };
	TCHAR szDir[MAX_PATH] = { 0, };
	TCHAR szFileName[MAX_PATH] = { 0, };
	TCHAR szExt[MAX_PATH] = { 0, };
	_splitpath_s( szCurFileName, szDrive, szDir, szFileName, szExt );

	CString _szCurPath;
	_szCurPath.Format( _T("%s%s\\Log"), szDrive, szDir );

	if( GetFileAttributes( _szCurPath.GetBuffer() ) != FILE_ATTRIBUTE_DIRECTORY )
		CreateDirectory( _szCurPath.GetBuffer(), NULL);

	LogWnd::CreateLog( true, LogWnd::LOG_TARGET_FILE );
}

