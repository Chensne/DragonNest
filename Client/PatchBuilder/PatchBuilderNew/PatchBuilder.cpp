// PatchBuilder.cpp : ���� ���α׷��� ���� Ŭ���� ������ �����մϴ�.
//

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

int __cdecl DefautAllocHook( int, void *, size_t, int, long, const unsigned char *, int )
{
	return 1;
}

int __cdecl MyAllocHook(
						int nAllocType,
						void * pvData,
						size_t nSize,
						int nBlockUse,
						long lRequest,
						const unsigned char * szFileName,
						int nLine
						)
{
	if( nAllocType == _HOOK_ALLOC && nSize == 2844 )
	{
		int a = 0;
	}
	return 1;
}

// CPatchBuilderApp ����
CPatchBuilderApp::CPatchBuilderApp()
{
	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
	// InitInstance�� ��� �߿��� �ʱ�ȭ �۾��� ��ġ�մϴ�.
//	_CrtSetAllocHook( MyAllocHook );
//	_CrtSetBreakAlloc(59925);
}

CPatchBuilderApp::~CPatchBuilderApp()
{
	LogWnd::DestroyLog();
}

// ������ CPatchBuilderApp ��ü�Դϴ�.
CPatchBuilderApp theApp;

// CPatchBuilderApp �ʱ�ȭ
BOOL CPatchBuilderApp::InitInstance()
{
	// ���� ���α׷� �Ŵ��佺Ʈ�� ComCtl32.dll ���� 6 �̻��� ����Ͽ� ���־� ��Ÿ����
	// ����ϵ��� �����ϴ� ���, Windows XP �󿡼� �ݵ�� InitCommonControlsEx()�� �ʿ��մϴ�.
	// InitCommonControlsEx()�� ������� ������ â�� ���� �� �����ϴ�.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ���� ���α׷����� ����� ��� ���� ��Ʈ�� Ŭ������ �����ϵ���
	// �� �׸��� �����Ͻʽÿ�.
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
		dlg.m_szSVNUrlBuildSet = pProfile->szSVNUrlBuildSet.c_str();
		dlg.m_szSVNUrlServer = pProfile->szSVNUrlServer.c_str();
		dlg.m_szSVNUpdateFolder = pProfile->szSVNUpdateFolder.c_str();
		dlg.m_szPatchFolder = pProfile->szPatchFolder.c_str();
		dlg.m_szRMakeCmd = pProfile->szRMakeCmd.c_str();
		dlg.m_szManualPatchExe = pProfile->szManualPatchExe.c_str();
		dlg.m_szCountryCode = pProfile->szCountryCode.c_str();
	}
	else
	{
		dlg.m_szSVNUrl = "http://depot.eyedentitygames.com/svn/dragonnest/stable/client";
		dlg.m_szSVNUrlBuildSet = "http://depot.eyedentitygames.com/svn/dragonnest/stable/client";
		dlg.m_szSVNUrlServer = "http://depot.eyedentitygames.com/svn/dragonnest/stable/client";
		dlg.m_szSVNUpdateFolder = "c:\\SVNUpdateFolder";
		dlg.m_szPatchFolder = "c:\\PatchFolder";
		dlg.m_szRMakeCmd = "\\\\192.168.0.11\\tooldata\\Utility\\RMakeCmd.exe";
		dlg.m_szCountryCode = "Kor";
		dlg.m_szSVNUrlBuildSet = "http://depot.eyedentitygames.com/svn/dragonnest/stable/server";
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
