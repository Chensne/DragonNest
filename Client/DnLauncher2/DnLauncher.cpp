// DnLauncher.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "DnLauncher.h"
#include "DnLauncherDlg.h"
#include "DnServiceModule.h"
#include "DnFIrstPatchDlg.h"
#include <locale.h>
#include <process.h>
#include "DnControlData.h"
#include <io.h>
#ifdef _USE_BITTORRENT
#include "WLibTorrent.h"
#endif // _USE_BITTORRENT


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma comment( lib, "shlwapi.lib" )

// CDnLauncherApp
BEGIN_MESSAGE_MAP(CDnLauncherApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

int __cdecl DefautAllocHook( int, void *, size_t, int, long, const unsigned char *, int )
{
	return 1;
}

int __cdecl MyAllocHook( int nAllocType, void * pvData, size_t nSize, int nBlockUse,
						long lRequest, const unsigned char * szFileName, int nLine )
{
	if( nAllocType == _HOOK_ALLOC && nSize == 106 )
	{
		int a = 0;
	}
	return 1;
}


// The one and only CDnLauncherApp object
CDnLauncherApp	theApp;
CString			g_szCmdLine;
CString			g_szOriginalCmdLine;		// ������ġ�� ���ؼ� �ʱⰪ ����
CString			g_szCommandLinePatchURL;	// Ŀ�ǵ� �������� PatchConfigList.xml�� �ִ� ���� �ּҸ� ���� �� ��� (����)
int				g_nInitErrorCode = 0;		// ��ó �ʱ�ȭ �� ���� �ڵ�

extern CDnFistPatchDownloadThread*  g_pFirstPatchDownloadThread;
CDnFIrstPatchDlg					g_FirstPatchDlg;
HANDLE								g_FirstPatchDlgHandle;
extern FirstPatchReturn				g_nFirstPatchErrorMessage;
unsigned int WINAPI ShowFirstPatchDlg(void* arg)
{
	INT_PTR nResult = g_FirstPatchDlg.DoModal();
	return 0;
}

#ifndef _DEBUG
static const char g_szMutexName[] = "Global\\___DN___";
HANDLE g_hMutex = NULL;
#endif // _DEBUG


// CDnLauncherApp construction
CDnLauncherApp::CDnLauncherApp()
{
#ifdef _USE_LOG
	DeleteOldLogFile();
	UseLogFile();
#endif // _USE_LOG

#ifdef _DEBUG
	//����׽� 2005 ������ TRACE ���� ��Ʈ���̱�� ������ �����⋪�� ������ ��������� �Ѵ�. 
	_tsetlocale( LC_ALL, _T("korean") );

//	_CrtSetAllocHook( MyAllocHook );
//	_CrtSetBreakAlloc( 2599 );
#endif
}

CDnLauncherApp::~CDnLauncherApp()
{
#ifndef _DEBUG
	if( g_hMutex )
	{
		CloseHandle( g_hMutex );
		g_hMutex = NULL;
	}
#endif // _DEBUG

	if( g_pServiceModule )
		g_pServiceModule->Destroy();

	SAFE_DELETE( g_pServiceModule );

	LogWnd::DestroyLog();
}

// CDnLauncherApp initialization
BOOL CDnLauncherApp::InitInstance()
{
	g_szOriginalCmdLine = g_szCmdLine = m_lpCmdLine;
	ParseCommandLine();	// Ŀ�ǵ� ���� �Ľ�

	//���� üũ
#ifndef _DEBUG
	g_hMutex = ::OpenMutexA( MUTEX_ALL_ACCESS, FALSE, g_szMutexName );
	if( g_hMutex == NULL )
	{
		g_hMutex = ::CreateMutexA( NULL, FALSE, g_szMutexName );
	}
	else
	{
		ErrorMessageBoxLog( _S( STR_ALEADY_RUN + DNPATCHINFO.GetLanguageOffset() ) );
		return FALSE;
	}
#endif // _DEBUG

	CHECK_ALIGNMENT();

	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	AfxEnableControlContainer();
	SetRegistryKey(_T("DragonNest Launcher"));

//	ErrorMessageBoxLog( g_szCmdLine );
//	LogWnd::TraceLog( g_szCmdLine );

	if( InitLauncher() )
	{
		CDnLauncherDlg* pDlg = CDnLauncherDlg::CreateLauncherDlg();
		m_pMainWnd			 = pDlg;
		INT_PTR nResponse	 = pDlg->DoModal();

		SAFE_DELETE( pDlg );
	}

#ifdef _USE_BITTORRENT
	WLibTorrent::DestroyInstance();
#endif // _USE_BITTORRENT

	return FALSE;
}

BOOL CDnLauncherApp::InitLauncher()
{
	LogWnd::TraceLog( L"InitLauncher Start!" );

#ifdef _USE_SINGLE_CLIENT
	if( FindProcessName( _T(DNCLIENT_NAME) ) )	// ���� ���� �߿��� �������� ����
	{
		ErrorMessageBoxLog( _S( STR_CLIENT_ALEADY_RUN + DNPATCHINFO.GetLanguageOffset() ) );
		g_nInitErrorCode = INIT_ERROR_CLIENT_ALEADY_RUN;
		return FALSE;
	}
#endif // _USE_SINGLE_CLIENT
		
#ifdef _FIRST_PATCH
	if( !InitFirstPatchThread() )			// FirstPatch�� Thread�� �����Ѵ�.
	{
		return InitFail();
	}
#endif

	if( !CheckDirectXVersion() )			// DirectX ���� üũ
	{
		return InitFail();
	}

	if( DNPATCHINFO.Init() != S_OK )		// PatchInfo �ʱ�ȭ ( PatchConfigList.xml �ٿ�, �Ľ�, Ŭ���̾�Ʈ ��� ����, xml���� )
	{
		return InitFail();
	}

#ifndef _USE_PARTITION_SELECT
	if( !DNPATCHINFO.SetPatchInfo() )		// PatchInfo ����   ( PatchConfigList.xml ������ ���κ����� ����, �Ķ���� ����, ��ġ ������ ���⼭ üũ )	
	{
		return InitFail();
	}
#endif // _USE_PARTITION_SELECT


#if defined(_FIRST_PATCH)

#ifdef _USE_PARTITION_SELECT
	DNPATCHINFO.LoadVersionData();			// ��Ƽ�� ���� && ���� �о����.
#endif

	if( DNFIRSTPATCHINFO.GetFirstPatchStatus() == DNFIRSTPATCHINFO.EM_FIRSTPATCH_OK )
	{
		if( !DNFIRSTPATCHINFO.SetPatchInfo() ) // First Patch "�ʱ�ȭ" & "������ ����"
		{
			return InitFail();
		}
	}

	TerminateFirstPatchThread();			// * First Patch���� ����� ������ ���� * 
#endif // _FIRST_PATCH

#if defined( _USE_COMMAND_LINE )			// Ŀ�ǵ���� ��� �� �Ķ���� ���� ��� Ȩ�������� ����
	if( g_szCmdLine.GetLength() == 0 )
	{
		ShellExecute( NULL, L"open", DNPATCHINFO.GetHomepageUrl(), NULL, NULL, SW_SHOW );
		return FALSE;
	}
#endif // _USE_COMMAND_LINE

	g_pServiceModule = DnServiceModule::CreateServiceModule();
	if( g_pServiceModule && !g_pServiceModule->Initialize() )	// ���� ��ü�� Initialize
	{
		g_nInitErrorCode = INIT_ERROR_SERVICE_MODULE;
		return InitFail();
	}
	
	if( !RemoveUsedFile() )	 // ��ó ���� �� ��� �ߴ� ���� �� ��ó ������Ʈ �� ������ tmp ���� ����
	{
		return InitFail();
	}

#ifdef _FIRST_PATCH
#if !defined(_TEST)
	if(g_nFirstPatchErrorMessage == FPR_NEED_FULLPATCH)
	{
		// Ŭ���̾�Ʈ �缳ġ�� ����. �׳� �����Ŵ.
		LogWnd::TraceLog( L"FirstPatch Failed / need Client Full Patch!" );
		LogWnd::Log( LogLevel::Error, _T("FirstPatch Failed / Need Full Patch") );
		return InitFail();
	}
#endif // _TEST
#endif // _FIRST_PATCH

#ifdef _USE_BITTORRENT
	if( !WLibTorrent::GetInstance()->InitSession( L"DN" ) )
	{
		g_nInitErrorCode = INIT_ERROR_BITTORRENT_SESSION;
		return InitFail();
	}
#endif // _USE_BITTORRENT

	LogWnd::TraceLog( L"InitLauncher Success!" );
	return TRUE;
}

#ifdef _FIRST_PATCH
BOOL CDnLauncherApp::InitFirstPatchThread()
{
	// * Tmp������ ������ ����.
	if(_access(DNLAUNCHER_NAME_TMP, 0) == 0)
	{
		if( !ClientDeleteFile(_T(DNLAUNCHER_NAME_TMP)) )
		{
			LogWnd::Log( LogLevel::Warning, _T("dnlauncher.tmp File Delete Failed") );
		}
	}

	// * FirstPatch �����Ȳ�� �����ִ� Dialog Thread ���� *
	g_FirstPatchDlgHandle = (HANDLE)_beginthreadex(NULL, 0, ShowFirstPatchDlg, NULL, 0, NULL);
	if(g_FirstPatchDlgHandle == NULL)
	{
		LogWnd::Log( LogLevel::Error, _T("FirstPatch Dialog Thread Create Failed") );
		return FALSE;
	}

	// �����ġ�� ���ۉ��.
	DNFIRSTPATCHINFO.SetFirstPatchProcess(DNFIRSTPATCHINFO.EM_FIRSTPATCH_ING);
	return TRUE;
}

void CDnLauncherApp::TerminateFirstPatchThread()
{
	if( g_pFirstPatchDownloadThread ) // �ٿ�ε� ������.
	{
		// FirstPatch�� �ٿ� �޴� �����尡 ������.
		WaitForSingleObject( g_pFirstPatchDownloadThread->GetThreadHandle(), INFINITE );
	}

	// ���̾�α� ����.
	if( g_FirstPatchDlg )
	{
		// * FirstPatch���� ���̾�α� & ������ ���� * //
		DWORD dwExitCode;
		if( GetExitCodeThread( g_FirstPatchDlgHandle, &dwExitCode ) )
		{
			TerminateThread( g_FirstPatchDlgHandle, dwExitCode );
		}

		g_FirstPatchDlg.EndDialog( IDYES );
	}

	// �����ġ�� ������.
	DNFIRSTPATCHINFO.SetFirstPatchProcess(DNFIRSTPATCHINFO.EM_FIRSTPATCH_NOT);
}
#endif // _FIRST_PATCH

BOOL CDnLauncherApp::InitFail()
{
	CString strError;
	strError.Format( _S( STR_LAUNCHER_INIT_FAILED + DNPATCHINFO.GetLanguageOffset() ), g_nInitErrorCode );
	LogWnd::Log( LogLevel::Error, strError );
	AfxMessageBox( strError );
	
#ifdef _FIRST_PATCH
	HANDLE	pHandles[2];
	pHandles[0] = g_FirstPatchDlgHandle;
	if(g_pFirstPatchDownloadThread)
	{
		pHandles[1] = g_pFirstPatchDownloadThread->GetThreadHandle();
	}
	
	DWORD dwExitCode;
	GetExitCodeThread(g_FirstPatchDlgHandle, &dwExitCode);
	TerminateThread(g_FirstPatchDlgHandle, dwExitCode);
	
	if( WAIT_FAILED != WaitForMultipleObjects(2, pHandles, TRUE, INFINITE) )
	{
		// �����尡 ��� ���������, �����Ų��.
		return FALSE;
	}
#endif
	
	return FALSE;
}

BOOL CDnLauncherApp::CheckDirectXVersion()
{
	DWORD dwDirectXVersion = 0;
	TCHAR strDirectXVersion[10] = {0,};

	std::wstring wstrVersion;

	int nResult = GetDXVersion( &dwDirectXVersion, strDirectXVersion, 10);

	CString szStr;
	LogWnd::TraceLog( _T("DirectX Version (%u) (%s)"), dwDirectXVersion, strDirectXVersion );

	if( dwDirectXVersion < 0x00090000 )
	{
		wstrVersion = strDirectXVersion;
		if( wstrVersion != L"9.0c" )
		{
			CString szStr;
			szStr.Format ( _S( STR_DX_LOW_VER + DNPATCHINFO.GetLanguageOffset() ), wstrVersion.c_str() );
			ErrorMessageBoxLog( szStr );
			g_nInitErrorCode = INIT_ERROR_DX_VER;
			return FALSE;
		}
	}

	return TRUE;
}

void CDnLauncherApp::ParseCommandLine()
{
	if( g_szCmdLine.GetLength() == 0 )
		return;

	LPCTSTR szlpURL = L"http://";
#ifdef _USE_MULTILANGUAGE
	LPCTSTR szlpLanguage = L"language:";
#endif // _USE_MULTILANGUAGE

	std::wstring wszCmdLine;	 
	wszCmdLine = g_szCmdLine;
	std::vector<std::wstring> tokens;
	TokenizeW( wszCmdLine, tokens, std::wstring(L" ") );

	for( DWORD i=0; i<tokens.size(); i++ )
	{
		if( NULL != StrStr( tokens[i].c_str(), szlpURL ) )			// PatchConfigList.xml�� �ּҰ� ��� �ִ�
		{
			g_szCommandLinePatchURL = tokens[i].c_str();			// �ּ� ����
			g_szCmdLine.Replace( g_szCommandLinePatchURL, L"" );	// Ŀ�ǵ� ���ο��� �ּ� �����ش�
#ifndef _USE_MULTILANGUAGE
			break;
#endif // _USE_MULTILANGUAGE
		}
#ifdef _USE_MULTILANGUAGE
		if( NULL != StrStr( tokens[i].c_str(), szlpLanguage ) )			// PatchConfigList.xml�� �ּҰ� ��� �ִ�
		{
			tokens[i].erase( 0, tokens[i].find(L":") + 1 );
			DNPATCHINFO.SetLanguageParam( tokens[i].c_str() );
		}
#endif // _USE_MULTILANGUAGE
	}

	tokens.clear();

	if( g_szCommandLinePatchURL.GetLength() > 0 && g_szCommandLinePatchURL.GetBuffer()[g_szCommandLinePatchURL.GetLength()-1] != '/' )
		g_szCommandLinePatchURL += '/';
}

void CDnLauncherApp::DeleteOldLogFile()
{
	TCHAR szCurFileName[MAX_PATH]= {0,};
	::GetModuleFileName( NULL, szCurFileName, MAX_PATH );
	TCHAR szDrive[MAX_PATH] = { 0, };
	TCHAR szDir[MAX_PATH] = { 0, };
	TCHAR szFileName[MAX_PATH] = { 0, };
	TCHAR szExt[MAX_PATH] = { 0, };
	_tsplitpath( szCurFileName, szDrive, szDir, szFileName, szExt );

	CString _szCurPath;
	_szCurPath.Format( _T("%s%s\\Log\\"), szDrive, szDir );

	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	CString szFindLogFile;
	szFindLogFile = _szCurPath;
	szFindLogFile += "DNLauncher_*_Log.txt";
	hFind = FindFirstFile( szFindLogFile.GetBuffer(), &FindFileData );

	std::vector<CString> vecStrLogFileName;
	while( hFind != INVALID_HANDLE_VALUE ) 
	{
		if( !( FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) )
		{
			CString szLogFile;
			szLogFile = _szCurPath;
			szLogFile += FindFileData.cFileName;

			vecStrLogFileName.push_back( szLogFile );
		}
		if( !FindNextFile( hFind, &FindFileData ) ) break;
	}
	FindClose( hFind );

	std::vector<CString> vecStrDeleteLogFileName;
	for( int i=0; i<static_cast<int>( vecStrLogFileName.size() ); i++ )
	{
		CString strTemp = vecStrLogFileName[i];
		strTemp.Delete( 0, strTemp.Find( L"_" ) + 1 );
		int nIndex = strTemp.Find( L"_" );
		strTemp.Delete( nIndex, strTemp.GetLength() - nIndex );
		int nLogDate = _wtoi( strTemp.GetBuffer() );

		struct tm now;
		time_t systemTime;

		time( &systemTime );				// ���� �ð��� �� ������ ���
		localtime_s( &now, &systemTime );	// �� ������ �ð��� �и��Ͽ� ����ü�� �ֱ�

		TCHAR szCurDate[1024];
		_sntprintf_s( szCurDate, 1024, _T("%04d%02d%02d"), 1900+now.tm_year, now.tm_mon, now.tm_mday );	// �Ѵ� ���� �͵��� ����
		int nCurDate = _wtoi( szCurDate );

		if( nLogDate < nCurDate )
		{
			vecStrDeleteLogFileName.push_back( vecStrLogFileName[i] );
		}
	}

	for( int i=0; i<static_cast<int>( vecStrDeleteLogFileName.size() ); i++ )
	{
		ClientDeleteFile( vecStrDeleteLogFileName[i] );
	}

	vecStrLogFileName.clear();
	vecStrDeleteLogFileName.clear();
}

void CDnLauncherApp::UseLogFile()
{
	TCHAR szCurFileName[MAX_PATH]= {0,};
	::GetModuleFileName( NULL, szCurFileName, MAX_PATH );
	TCHAR szDrive[MAX_PATH] = { 0, };
	TCHAR szDir[MAX_PATH] = { 0, };
	TCHAR szFileName[MAX_PATH] = { 0, };
	TCHAR szExt[MAX_PATH] = { 0, };
	_tsplitpath( szCurFileName, szDrive, szDir, szFileName, szExt );

	CString _szCurPath;
	_szCurPath.Format( _T("%s%s\\Log"), szDrive, szDir );

	if( GetFileAttributes( _szCurPath.GetBuffer() ) != FILE_ATTRIBUTE_DIRECTORY )
		CreateDirectory( _szCurPath.GetBuffer(), NULL);

	LogWnd::CreateLog( true, LogWnd::LOG_TARGET_FILE );
}

void CDnLauncherApp::UseLogWindow()
{
	TCHAR szCurFileName[MAX_PATH]= {0,};
	::GetModuleFileName( NULL, szCurFileName, MAX_PATH );
	TCHAR szDrive[MAX_PATH] = { 0, };
	TCHAR szDir[MAX_PATH] = { 0, };
	TCHAR szFileName[MAX_PATH] = { 0, };
	TCHAR szExt[MAX_PATH] = { 0, };
	_tsplitpath( szCurFileName, szDrive, szDir, szFileName, szExt );

	CString _szCurPath;
	_szCurPath.Format( _T("%s%s\\Log"), szDrive, szDir );

	if( GetFileAttributes( _szCurPath.GetBuffer() ) != FILE_ATTRIBUTE_DIRECTORY )
		CreateDirectory( _szCurPath.GetBuffer(), NULL);

	LogWnd::CreateLog( true, LogWnd::LOG_TARGET_WINDOW );
}

BOOL CDnLauncherApp::RemoveUsedFile()
{
	BOOL bRtn = TRUE;
	LogWnd::TraceLog( L"RemoveUsedFile" );
	if( ClientDeleteFile( _T(DNLAUNCHER_NAME_TMP) ) == FALSE )
	{		
		KillProcess( _T(DNLAUNCHER_NAME) );	// ���� ���μ���â���� dnlauncher.exe�� ��� �������� ��Ų��.
		if( ClientDeleteFile( _T(DNLAUNCHER_NAME_TMP) ) == FALSE )
			bRtn = FALSE;
	}

	if( ClientDeleteFile( PATCHCONFIG_LIST ) == FALSE ) 
		bRtn = FALSE;

	if( !bRtn )
		g_nInitErrorCode = INIT_ERROR_REMOVE_FILE;

	return bRtn;
}