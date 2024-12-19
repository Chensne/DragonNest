#include "stdafx.h"
#include "SundriesFunc.h"
#include "PatchBuilder.h"
#include "PatchBuilderDlg.h"
#include "tinyxml.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CPatchBuilderApp
BEGIN_MESSAGE_MAP(CPatchBuilderApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

// CPatchBuilderApp 생성
CPatchBuilderApp::CPatchBuilderApp()
{
}

CPatchBuilderApp::~CPatchBuilderApp()
{
	LogWnd::DestroyLog();
}

// 유일한 CPatchBuilderApp 개체입니다.
CPatchBuilderApp theApp;

// CPatchBuilderApp 초기화
BOOL CPatchBuilderApp::InitInstance()
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

	CPatchBuilderDlg dlg;
	m_pMainWnd = &dlg;

	UseLogFile();

	if( m_RegProfile.GetProfileCount() )
	{
		SProfile *pProfile = m_RegProfile.GetProfile( 0 );
		dlg.m_szSVNUrl = pProfile->szSVNUrl.c_str();
		dlg.m_szSVNUpdateFolder = pProfile->szSVNUpdateFolder.c_str();
		dlg.m_szPatchFolder = pProfile->szPatchFolder.c_str();
		dlg.m_szRMakeCmd = pProfile->szRMakeCmd.c_str();
		dlg.m_szManualPatchExe = pProfile->szManualPatchExe.c_str();
		dlg.m_szCountryCode = pProfile->szCountryCode.c_str();
		dlg.m_szSVNUrlServer = pProfile->szSVNUrlServer.c_str();
	}
	else
	{
		dlg.m_szSVNUrl = "http://depot.eyedentitygames.com/svn/dragonnest/stable/client";
		dlg.m_szSVNUpdateFolder = "c:\\SVNUpdateFolder";
		dlg.m_szPatchFolder = "c:\\PatchFolder";
		dlg.m_szRMakeCmd = "\\\\192.168.0.11\\tooldata\\Utility\\RMakeCmd.exe";
		dlg.m_szCountryCode = "Kor";
		dlg.m_szSVNUrlServer = "http://depot.eyedentitygames.com/svn/dragonnest/stable/server";
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

		pFindPtr = strstr( m_lpCmdLine, "/UseRTPatch:true" );
		if( pFindPtr )
		{
			dlg.m_bUseRTPatch = TRUE;
		}
		else
		{
			pFindPtr = strstr( m_lpCmdLine, "/UseRTPatch:false" );
			if( pFindPtr )
			{
				dlg.m_bUseRTPatch = FALSE;
			}
		}
	}

	INT_PTR nResponse = dlg.DoModal();

	return FALSE;
}

void CPatchBuilderApp::CheckPatchFolder()
{
	std::vector< std::string > szVecResult;
	std::string szLastPatchFolder;

	szLastPatchFolder = ( ( CPatchBuilderDlg * )m_pMainWnd )->m_szPatchFolder.GetBuffer();

	CreateFolder( szLastPatchFolder.c_str() );
	_FindFolder( szLastPatchFolder.c_str(), szVecResult, false, NULL );
	( ( CPatchBuilderDlg * )m_pMainWnd )->m_nCurrentVersion = 1;
	if( szVecResult.empty() )
	{
		return;
	}

	bool bDeleteFolder = false;
	int i;
	std::sort( szVecResult.begin(), szVecResult.end() );
	for( i = ( int )szVecResult.size() - 1; i >= 0; i-- )
	{
		szLastPatchFolder = ( ( CPatchBuilderDlg * )m_pMainWnd )->m_szPatchFolder.GetBuffer();
		szLastPatchFolder += "\\";
		szLastPatchFolder += szVecResult[ i ];

		std::vector< std::string > szVecRevision;
		_FindFolder( szLastPatchFolder.c_str(), szVecRevision, false, NULL );
		std::sort( szVecRevision.begin(), szVecRevision.end() );
		if( !szVecRevision.empty() )
		{
			const char *pFindPtr = strchr( szVecRevision[ 0 ].c_str(), '-' );
			if( pFindPtr )
			{
				if( bDeleteFolder )
				{
					( ( CPatchBuilderDlg * )m_pMainWnd )->m_nCurrentVersion = atoi( szVecResult[ i ].c_str() );
					( ( CPatchBuilderDlg * )m_pMainWnd )->UpdatePatchInfo();
				}
				( ( CPatchBuilderDlg * )m_pMainWnd )->m_nStartRevision = atoi( pFindPtr + 1 );
				( ( CPatchBuilderDlg * )m_pMainWnd )->m_nCurrentVersion = atoi( szVecResult[ i ].c_str() ) + 1;
				return;
			}
		}

		char szString[ 1024 ];
		sprintf_s( szString, 1024, "%s 폴더는 잘못된 폴더 입니다. 지울까요?", szLastPatchFolder.c_str() );
		if( MessageBox( m_pMainWnd->GetSafeHwnd(), szString, NULL, MB_YESNO ) == IDYES )
		{
			DeleteFolder( szLastPatchFolder.c_str() );
			bDeleteFolder = true;
		}
	}
}

void CPatchBuilderApp::UseLogFile()
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