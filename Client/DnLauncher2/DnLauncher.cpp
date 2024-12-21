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
CString			g_szOriginalCmdLine;		// 런쳐패치를 위해서 초기값 보존
CString			g_szCommandLinePatchURL;	// 커맨드 라인으로 PatchConfigList.xml이 있는 곳의 주소를 전달 할 경우 (국내)
int				g_nInitErrorCode = 0;		// 런처 초기화 시 에러 코드

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
	//디버그시 2005 에서는 TRACE 사용시 스트링이길면 에러가 나오기떔시 로컬을 셋팅해줘야 한다. 
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
	ParseCommandLine();	// 커맨드 라인 파싱

	//런쳐 체크
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
	if( FindProcessName( _T(DNCLIENT_NAME) ) )	// 게임 실행 중에는 구동되지 않음
	{
		ErrorMessageBoxLog( _S( STR_CLIENT_ALEADY_RUN + DNPATCHINFO.GetLanguageOffset() ) );
		g_nInitErrorCode = INIT_ERROR_CLIENT_ALEADY_RUN;
		return FALSE;
	}
#endif // _USE_SINGLE_CLIENT
		
#ifdef _FIRST_PATCH
	if( !InitFirstPatchThread() )			// FirstPatch의 Thread를 생성한다.
	{
		return InitFail();
	}
#endif

	if( !CheckDirectXVersion() )			// DirectX 버전 체크
	{
		return InitFail();
	}

	if( DNPATCHINFO.Init() != S_OK )		// PatchInfo 초기화 ( PatchConfigList.xml 다운, 파싱, 클라이언트 경로 설정, xml삭제 )
	{
		return InitFail();
	}

#ifndef _USE_PARTITION_SELECT
	if( !DNPATCHINFO.SetPatchInfo() )		// PatchInfo 설정   ( PatchConfigList.xml 내용을 내부변수로 설정, 파라매터 설정, 패치 버전를 여기서 체크 )	
	{
		return InitFail();
	}
#endif // _USE_PARTITION_SELECT


#if defined(_FIRST_PATCH)

#ifdef _USE_PARTITION_SELECT
	DNPATCHINFO.LoadVersionData();			// 파티션 정보 && 버전 읽어오기.
#endif

	if( DNFIRSTPATCHINFO.GetFirstPatchStatus() == DNFIRSTPATCHINFO.EM_FIRSTPATCH_OK )
	{
		if( !DNFIRSTPATCHINFO.SetPatchInfo() ) // First Patch "초기화" & "스레드 시작"
		{
			return InitFail();
		}
	}

	TerminateFirstPatchThread();			// * First Patch에서 사용한 스레드 정리 * 
#endif // _FIRST_PATCH

#if defined( _USE_COMMAND_LINE )			// 커맨드라인 사용 시 파라미터 없을 경우 홈페이지로 연결
	if( g_szCmdLine.GetLength() == 0 )
	{
		ShellExecute( NULL, L"open", DNPATCHINFO.GetHomepageUrl(), NULL, NULL, SW_SHOW );
		return FALSE;
	}
#endif // _USE_COMMAND_LINE

	g_pServiceModule = DnServiceModule::CreateServiceModule();
	if( g_pServiceModule && !g_pServiceModule->Initialize() )	// 서비스 업체별 Initialize
	{
		g_nInitErrorCode = INIT_ERROR_SERVICE_MODULE;
		return InitFail();
	}
	
	if( !RemoveUsedFile() )	 // 런처 구동 시 사용 했던 파일 및 런처 업데이트 시 생성된 tmp 파일 삭제
	{
		return InitFail();
	}

#ifdef _FIRST_PATCH
#if !defined(_TEST)
	if(g_nFirstPatchErrorMessage == FPR_NEED_FULLPATCH)
	{
		// 클라이언트 재설치를 요함. 그냥 종료시킴.
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
	// * Tmp파일이 있으면 삭제.
	if(_access(DNLAUNCHER_NAME_TMP, 0) == 0)
	{
		if( !ClientDeleteFile(_T(DNLAUNCHER_NAME_TMP)) )
		{
			LogWnd::Log( LogLevel::Warning, _T("dnlauncher.tmp File Delete Failed") );
		}
	}

	// * FirstPatch 진행상황을 보여주는 Dialog Thread 시작 *
	g_FirstPatchDlgHandle = (HANDLE)_beginthreadex(NULL, 0, ShowFirstPatchDlg, NULL, 0, NULL);
	if(g_FirstPatchDlgHandle == NULL)
	{
		LogWnd::Log( LogLevel::Error, _T("FirstPatch Dialog Thread Create Failed") );
		return FALSE;
	}

	// 모듈패치가 시작됬다.
	DNFIRSTPATCHINFO.SetFirstPatchProcess(DNFIRSTPATCHINFO.EM_FIRSTPATCH_ING);
	return TRUE;
}

void CDnLauncherApp::TerminateFirstPatchThread()
{
	if( g_pFirstPatchDownloadThread ) // 다운로드 스레드.
	{
		// FirstPatch를 다운 받는 스레드가 종료됬다.
		WaitForSingleObject( g_pFirstPatchDownloadThread->GetThreadHandle(), INFINITE );
	}

	// 다이얼로그 종료.
	if( g_FirstPatchDlg )
	{
		// * FirstPatch관련 다이얼로그 & 스레드 종료 * //
		DWORD dwExitCode;
		if( GetExitCodeThread( g_FirstPatchDlgHandle, &dwExitCode ) )
		{
			TerminateThread( g_FirstPatchDlgHandle, dwExitCode );
		}

		g_FirstPatchDlg.EndDialog( IDYES );
	}

	// 모듈패치가 끝났다.
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
		// 스레드가 모두 종료됬으면, 종료시킨다.
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
		if( NULL != StrStr( tokens[i].c_str(), szlpURL ) )			// PatchConfigList.xml의 주소가 들어 있다
		{
			g_szCommandLinePatchURL = tokens[i].c_str();			// 주소 저장
			g_szCmdLine.Replace( g_szCommandLinePatchURL, L"" );	// 커맨드 라인에서 주소 지워준다
#ifndef _USE_MULTILANGUAGE
			break;
#endif // _USE_MULTILANGUAGE
		}
#ifdef _USE_MULTILANGUAGE
		if( NULL != StrStr( tokens[i].c_str(), szlpLanguage ) )			// PatchConfigList.xml의 주소가 들어 있다
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

		time( &systemTime );				// 현재 시각을 초 단위로 얻기
		localtime_s( &now, &systemTime );	// 초 단위의 시간을 분리하여 구조체에 넣기

		TCHAR szCurDate[1024];
		_sntprintf_s( szCurDate, 1024, _T("%04d%02d%02d"), 1900+now.tm_year, now.tm_mon, now.tm_mday );	// 한달 넘은 것들은 삭제
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
		KillProcess( _T(DNLAUNCHER_NAME) );	// 현재 프로세스창에서 dnlauncher.exe를 모두 강제종료 시킨다.
		if( ClientDeleteFile( _T(DNLAUNCHER_NAME_TMP) ) == FALSE )
			bRtn = FALSE;
	}

	if( ClientDeleteFile( PATCHCONFIG_LIST ) == FALSE ) 
		bRtn = FALSE;

	if( !bRtn )
		g_nInitErrorCode = INIT_ERROR_REMOVE_FILE;

	return bRtn;
}