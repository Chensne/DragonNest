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
#include "Settings.h"

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
CString			g_szOriginalCmdLine;		// 繁媚菩摹甫 困秦辑 檬扁蔼 焊粮
CString			g_szCommandLinePatchURL;	// 目盖靛 扼牢栏肺 PatchConfigList.xml捞 乐绰 镑狼 林家甫 傈崔 且 版快 (惫郴)
int				g_nInitErrorCode = 0;		// 繁贸 檬扁拳 矫 俊矾 内靛

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
extern "C" { FILE __iob_func[3] = { *stdin,*stdout,*stderr }; }
CDnLauncherApp::CDnLauncherApp()
{
#ifdef _SKY
	Settings::GetInstance().LoadSettings();
#endif
#ifdef _USE_LOG
	DeleteOldLogFile();
	UseLogFile();
#endif // _USE_LOG

#ifdef _DEBUG
	//叼滚弊矫 2005 俊辑绰 TRACE 荤侩矫 胶飘傅捞辨搁 俊矾啊 唱坷扁嫪矫 肺拿阑 悸泼秦拎具 茄促. 
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
	ParseCommandLine();	// 目盖靛 扼牢 颇教

/*	while(1)
	{

	}*/

#if defined(_KRAZ) && defined(_DEBUG)
	// 颇扼概磐 txt肺 佬绢郴扁
	/*
	std::vector<std::wstring> tokens;
	TokenizeW( m_lpCmdLine, tokens, std::wstring( L" " ) );
	if(!tokens.empty())
	{
		FILE* stream = NULL;
		if(fopen_s(&stream, "c:\\ActozParameter.txt", "w") != 0)
			return false;
		fseek(stream, 0L, SEEK_SET);
		std::string request;
		std::string keyid;
		ToMultiString(tokens[0], request);
		ToMultiString(tokens[1], keyid);
		fprintf(stream,"/requesttime:%s", request.c_str());
		fprintf(stream," /keyid:%s", keyid.c_str());
		fclose(stream);
	}*/

	// 颇扼概磐 ClipBoard俊 历厘
	std::vector<std::wstring> tokens;
	TokenizeW( m_lpCmdLine, tokens, std::wstring( L" " ) );
	if(!tokens.empty())
	{
		WCHAR wszParameter[512] = { 0 , };
		swprintf_s(wszParameter, L"/requesttime:%s /keyid:%s", tokens[0].c_str(), tokens[1].c_str());
		if(::OpenClipboard(NULL))
		{
			::EmptyClipboard();
			if( HGLOBAL hMem = ::GlobalAlloc(GHND | GMEM_SHARE, (wcslen(wszParameter)+1)*sizeof(WCHAR)) )
			{
				LPWSTR pClipData = (LPWSTR)::GlobalLock(hMem);
				wcscpy(pClipData, wszParameter);
				::GlobalUnlock(hMem);
				::SetClipboardData(CF_UNICODETEXT, hMem);
			}
		}
		::CloseClipboard();
	}
#endif // defined(_KRAZ) && defined(_DEBUG)

#ifdef _USE_MULTILANGUAGE
	SetLanguageParam();
#endif // _USE_MULTILANGUAGE

#if defined(_KRAZ) && !defined(_DEBUG)
	LPCWSTR cmdLine = GetCommandLine();
	int nArgNum = 0;
	LPWSTR* argv = CommandLineToArgvW( cmdLine, &nArgNum );
	if( argv )
	{
		std::wstring strRootPath = argv[0];
		int nSlashIndex = strRootPath.rfind( L"\\" );
		strRootPath.erase( nSlashIndex, strRootPath.length() - nSlashIndex );
		SetCurrentDirectory( strRootPath.c_str() );
	}
#endif // defined(_KRAZ) && !defined(_DEBUG)

	if (PathFileExistsA(".\\DNBR_Launcher.exe"))
	{
		DeleteFileA(".\\DNBR_Launcher.exe");
	}

	//繁媚 眉农
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

	return FALSE;
}

BOOL CDnLauncherApp::InitLauncher()
{
	LogWnd::TraceLog( L"InitLauncher Start!" );

#ifdef _USE_SINGLE_CLIENT
	if( FindProcessName( _T(DNCLIENT_NAME) ) )	// 霸烙 角青 吝俊绰 备悼登瘤 臼澜
	{
		ErrorMessageBoxLog( _S( STR_CLIENT_ALEADY_RUN + DNPATCHINFO.GetLanguageOffset() ) );
		g_nInitErrorCode = INIT_ERROR_CLIENT_ALEADY_RUN;
		return FALSE;
	}
#endif // _USE_SINGLE_CLIENT
		
#ifdef _FIRST_PATCH
	if( !InitFirstPatchThread() )			// FirstPatch狼 Thread甫 积己茄促.
	{	
		return InitFail();
	}
#endif

	if( !CheckDirectXVersion() )			// DirectX 滚傈 眉农
	{
		return InitFail();
	}

	if( DNPATCHINFO.Init() != S_OK )		// PatchInfo 檬扁拳 ( PatchConfigList.xml 促款, 颇教, 努扼捞攫飘 版肺 汲沥, xml昏力 )
	{
		return InitFail();
	}

#ifndef _USE_PARTITION_SELECT
	if( !DNPATCHINFO.SetPatchInfo() )		// PatchInfo 汲沥   ( PatchConfigList.xml 郴侩阑 郴何函荐肺 汲沥, 颇扼概磐 汲沥, 菩摹 滚傈甫 咯扁辑 眉农 )	
	{
		return InitFail();
	}
#endif // _USE_PARTITION_SELECT

#ifdef _USE_MULTILANGUAGE
	// 促惫绢 瘤盔狼 版快 guidepage父 汲沥等 攫绢狼 盲澄俊辑 佬绢客 悸泼 窃
	DNPATCHINFO.SetLocaleGuidePage();
#endif // _USE_MULTILANGUAGE

#if defined(_FIRST_PATCH)
#ifdef _USE_PARTITION_SELECT
	DNPATCHINFO.LoadVersionData();			// 颇萍记 沥焊 && 滚傈 佬绢坷扁.
#endif
	if( DNFIRSTPATCHINFO.GetFirstPatchStatus() == DNFIRSTPATCHINFO.EM_FIRSTPATCH_OK )
	{
		if( !DNFIRSTPATCHINFO.SetPatchInfo() ) // First Patch "檬扁拳" & "胶饭靛 矫累"
		{
			return InitFail();
		}
	}
	TerminateFirstPatchThread();			// * First Patch俊辑 荤侩茄 胶饭靛 沥府 * 
#endif // _FIRST_PATCH

#if defined( _USE_COMMAND_LINE )			// 目盖靛扼牢 荤侩 矫 颇扼固磐 绝阑 版快 权其捞瘤肺 楷搬
	if( g_szCmdLine.GetLength() == 0 )
	{
		ShellExecute( NULL, L"open", DNPATCHINFO.GetHomepageUrl(), NULL, NULL, SW_SHOW );
		return FALSE;
	}
#endif // _USE_COMMAND_LINE

	g_pServiceModule = DnServiceModule::CreateServiceModule();
	if( g_pServiceModule && !g_pServiceModule->Initialize() )	// 辑厚胶 诀眉喊 Initialize
	{
		g_nInitErrorCode = INIT_ERROR_SERVICE_MODULE;
		return InitFail();
	}
	
	if( !RemoveUsedFile() )	 // 繁贸 备悼 矫 荤侩 沁带 颇老 棺 繁贸 诀单捞飘 矫 积己等 tmp 颇老 昏力
	{
		return InitFail();
	}

#ifdef _FIRST_PATCH
#if !defined(_TEST)
	if(g_nFirstPatchErrorMessage == FPR_NEED_FULLPATCH)
	{
		// 努扼捞攫飘 犁汲摹甫 夸窃. 弊成 辆丰矫糯.
		LogWnd::TraceLog( L"FirstPatch Failed / need Client Full Patch!" );
		LogWnd::Log( LogLevel::Error, _T("FirstPatch Failed / Need Full Patch") );
		return InitFail();
	}
#endif // _TEST
#endif // _FIRST_PATCH

	LogWnd::TraceLog( L"InitLauncher Success!" );
	return TRUE;
}

#ifdef _FIRST_PATCH
BOOL CDnLauncherApp::InitFirstPatchThread()
{
	// * Tmp颇老捞 乐栏搁 昏力.
	if(_access(DNLAUNCHER_NAME_TMP, 0) == 0)
	{
		if( !ClientDeleteFile(_T(DNLAUNCHER_NAME_TMP)) )
		{
			LogWnd::Log( LogLevel::Warning, _T("dnlauncher.tmp File Delete Failed") );
		}
	}

	// * FirstPatch 柳青惑炔阑 焊咯林绰 Dialog Thread 矫累 *
	g_FirstPatchDlgHandle = (HANDLE)_beginthreadex(NULL, 0, ShowFirstPatchDlg, NULL, 0, NULL);
	if(g_FirstPatchDlgHandle == NULL)
	{
		LogWnd::Log( LogLevel::Error, _T("FirstPatch Dialog Thread Create Failed") );
		g_nInitErrorCode = INIT_ERROR_FIRSTPATCH_THREAD_CREATE_FAILED;
		return FALSE;
	}

	// 葛碘菩摹啊 矫累夌促.
	DNFIRSTPATCHINFO.SetFirstPatchProcess(DNFIRSTPATCHINFO.EM_FIRSTPATCH_ING);
	return TRUE;
}

void CDnLauncherApp::TerminateFirstPatchThread()
{
	if( g_pFirstPatchDownloadThread ) // 促款肺靛 胶饭靛.
	{
		// FirstPatch甫 促款 罐绰 胶饭靛啊 辆丰夌促.
		WaitForSingleObject( g_pFirstPatchDownloadThread->GetThreadHandle(), INFINITE );
	}

	// 促捞倔肺弊 辆丰.
	if( g_FirstPatchDlg )
	{
		// * FirstPatch包访 促捞倔肺弊 & 胶饭靛 辆丰 * //
		DWORD dwExitCode;
		if( GetExitCodeThread( g_FirstPatchDlgHandle, &dwExitCode ) )
		{
			TerminateThread( g_FirstPatchDlgHandle, dwExitCode );
		}

		g_FirstPatchDlg.EndDialog( IDYES );
	}

	// 葛碘菩摹啊 场车促.
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
		// 胶饭靛啊 葛滴 辆丰夌栏搁, 辆丰矫挪促.
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
		if( NULL != StrStr( tokens[i].c_str(), szlpURL ) )			// PatchConfigList.xml狼 林家啊 甸绢 乐促
		{
			g_szCommandLinePatchURL = tokens[i].c_str();			// 林家 历厘
			g_szCmdLine.Replace( g_szCommandLinePatchURL, L"" );	// 目盖靛 扼牢俊辑 林家 瘤况霖促
#ifndef _USE_MULTILANGUAGE
			break;
#endif // _USE_MULTILANGUAGE
		}
#ifdef _USE_MULTILANGUAGE
		if( NULL != StrStr( tokens[i].c_str(), szlpLanguage ) )			// PatchConfigList.xml狼 林家啊 甸绢 乐促
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

		time( &systemTime );				// 泅犁 矫阿阑 檬 窜困肺 掘扁
		localtime_s( &now, &systemTime );	// 檬 窜困狼 矫埃阑 盒府窍咯 备炼眉俊 持扁

		TCHAR szCurDate[1024];
		_sntprintf_s( szCurDate, 1024, _T("%04d%02d%02d"), 1900+now.tm_year, now.tm_mon, now.tm_mday );	// 茄崔 逞篮 巴甸篮 昏力
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
		KillProcess( _T(DNLAUNCHER_NAME) );	// 泅犁 橇肺技胶芒俊辑 dnlauncher.exe甫 葛滴 碍力辆丰 矫挪促.
		if( ClientDeleteFile( _T(DNLAUNCHER_NAME_TMP) ) == FALSE )
			bRtn = FALSE;
	}

	if( ClientDeleteFile( PATCHCONFIG_LIST ) == FALSE ) 
		bRtn = FALSE;

	if( !bRtn )
		g_nInitErrorCode = INIT_ERROR_REMOVE_FILE;

	return bRtn;
}

#ifdef _USE_MULTILANGUAGE
void CDnLauncherApp::SetLanguageParam()
{
	CString strLanguageParam = DNPATCHINFO.GetLanguageParam();
	if( strLanguageParam.GetLength() == 0 )
	{
		// 内靛其捞瘤 沥焊肺 悸泼
		int nLanguageID = PRIMARYLANGID( LOWORD( GetKeyboardLayout(0) ) );
		DNPATCHINFO.SetLanguageParamByLanguageID( nLanguageID );
	}
}
#endif // _USE_MULTILANGUAGE