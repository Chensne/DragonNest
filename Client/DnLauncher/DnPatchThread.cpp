#include "stdafx.h"
#include "DnPatchThread.h"
#include "SundriesFunc.h"
#include "MD5Checksum.h"
#include "VarArg.h"
#include "fdi.h"
#include "fcntl.h"
#include <io.h>
#ifdef _USE_RTPATCH
#include "patchwin.h"
#endif // _USE_RTPATCH
#if defined(_KR_NEXON)
#include "DnServiceModule.h"
#endif // _KR_NEXON

#pragma comment (lib, "setupapi.lib")
#pragma comment (lib, "fdi.lib")


extern CString g_szOriginalCmdLine;
#if defined(_KR_NEXON)
extern WiseLog* g_pWiseLog;
#endif // _KR_NEXON
#ifndef _DEBUG
extern HANDLE g_hMutex;
#endif // _DEBUG


stDownloadInfoUpdateData g_DownloadInfoData;	// 프로세스에서 다운로드 쓰레드의 상태값을 읽기 위한 전역 데이타

//////////////////////////////////////////////////////////////////////////
// Thread Base Class
//////////////////////////////////////////////////////////////////////////

CThread::CThread() : m_nThreadID( 0 ), m_hHandleThread( INVALID_HANDLE_VALUE )
{
	m_bThreadLoop = FALSE;
}

CThread::~CThread()
{
	m_bThreadLoop = FALSE;
}

BOOL CThread::Start()
{
	if( m_hHandleThread != INVALID_HANDLE_VALUE )
		return FALSE;

	m_hHandleThread = ::CreateThread( 0, 0, _Runner, ( LPVOID )this, 0, &m_nThreadID );
	if( m_hHandleThread == 0 )
	{
		return FALSE;
	}

	m_bThreadLoop = TRUE;

	return TRUE;
}

BOOL CThread::Terminate( DWORD nExitCode )
{
	BOOL ret = FALSE;

	if( m_hHandleThread == INVALID_HANDLE_VALUE )
		return TRUE;

	ret	= ( ::TerminateThread( m_hHandleThread, nExitCode ) == TRUE );
	m_hHandleThread	= INVALID_HANDLE_VALUE;

	return ret;
}

void CThread::SetThreadName( DWORD dwThreadID, const char* szThreadName )
{ 
	THREADNAME_INFO info;
	info.dwType = 0x1000;
	info.szName = szThreadName;
	info.dwThreadID = dwThreadID;
	info.dwFlags = 0;

	__try
	{
		RaiseException( 0x406D1388, 0, sizeof(info)/sizeof(DWORD), (ULONG_PTR*)&info );
	}
	__except (EXCEPTION_CONTINUE_EXECUTION)
	{
	}
}

BOOL CThread::WaitForTerminate( DWORD nTimeout )
{
	if( m_hHandleThread == INVALID_HANDLE_VALUE )
		return FALSE;

	return ( ::WaitForSingleObject( m_hHandleThread, nTimeout ) == WAIT_OBJECT_0 );
}

DWORD WINAPI CThread::_Runner( LPVOID pParam )
{
	CThread* pInstance = static_cast<CThread*>( pParam );
	pInstance->Run();
	return 0;
}



//////////////////////////////////////////////////////////////////////////
// "First" Patch Download Thread
//////////////////////////////////////////////////////////////////////////
CDnFistPatchDownloadThread * g_pFirstPatchDownloadThread = NULL;
FirstPatchReturn			 g_nFirstPatchErrorMessage = FPR_OK;


CDnFistPatchDownloadThread::CDnFistPatchDownloadThread()
: m_bTerminateThread( FALSE )
, m_hWnd( NULL )
{
	g_pFirstPatchDownloadThread = this;
}

CDnFistPatchDownloadThread::~CDnFistPatchDownloadThread()
{
}

void CDnFistPatchDownloadThread::CloseThread()
{
	TerminateThread();
}

BOOL CDnFistPatchDownloadThread::ProcessFullVersionPatch()
{
	LogWnd::TraceLog( _T( "ProcessFullVersionPatch" ) );

#ifdef _FIRST_PATCH
	g_nFirstPatchErrorMessage = FPR_NEED_FULLPATCH;
#endif

#if defined(_KR_NEXON)
	if( g_pServiceModule )
		g_pServiceModule->OnForceFullVersionPatch();
	SendMessage( m_hWnd, WM_CLOSE, 0, 0 );
	return FALSE;
#else // _KR_NEXON
	ErrorMessageBoxLog( _S( STR_PATCH_NEED_FULL_VERSION + DNPATCHINFO.GetLanguageOffset() ) );
	SendMessage( m_hWnd, WM_CLOSE, 0, 0 );
	CloseThread();
	return FALSE;
#endif // _KR_NEXON
}

void CDnFistPatchDownloadThread::Run()
{
	FirstPatchReturn PatchResult = BegineModulePatch();

	switch(PatchResult)
	{
	case FPR_OK:
		{
			// * 딱 한번 Pak파일에 저장한다 *
			if( !SaveModuleVersionToPak(DNFIRSTPATCHINFO.GetLocalModuleVersion()) )
			{
				ErrorMessageBoxLog( _S( STR_PATCH_FAILED + DNPATCHINFO.GetLanguageOffset() ) ); // 패치 실패.
			}
		}
		break;

	case FPR_FAIL:
		{
			ErrorMessageBoxLog( _S( STR_PATCH_FAILED + DNPATCHINFO.GetLanguageOffset() ) ); // 패치 실패.
			CloseThread();
		}
		break;

	case FPR_NEED_FULLPATCH:
		{
			ProcessFullVersionPatch();
		}
		break;

	case FPR_LAUNCHER_PATCH:
		{
			ProcessRebootLauncher();
		}
		break;
	}

	// 스레드 닫기
	//CloseThread();
}
void CDnFistPatchDownloadThread::ProcessRebootLauncher()
{
	// * 딱 한번 Pak파일에 저장한다 *
	BOOL bSaveResult = SaveModuleVersionToPak(DNFIRSTPATCHINFO.GetLocalModuleVersion());

	if(bSaveResult)
	{
		//if( AfxMessageBox(_T("런처가 패치되었습니다.\n런처를 재실행합니다."), MB_OK, MB_ICONINFORMATION) == IDOK )
		{
			CString strParam	= DNPATCHINFO.GetTotalParameter();	// 파라매터
			CString strExeFile	= DNPATCHINFO.GetClientPath();		// 다운로드 경로
			strExeFile += DNLAUNCHER_NAME;

#ifdef _USE_COMMAND_LINE
			ShellExecute( m_hWnd, NULL, strExeFile, g_szOriginalCmdLine.GetBuffer(), NULL, SW_SHOWNORMAL );
#else
			ShellExecute( m_hWnd, NULL, strExeFile, strParam.GetBuffer(), NULL, SW_SHOWNORMAL );
#endif
			KillMyProcess(_T(DNLAUNCHER_NAME)); // old프로세스 kill.
		}
	}
	else
	{
		ErrorMessageBoxLog( _S( STR_PATCH_FAILED+DNPATCHINFO.GetLanguageOffset() ) );
	}
}

FirstPatchReturn CDnFistPatchDownloadThread::BegineModulePatch()
{
	BOOL bModuleResult	 = FALSE;
	BOOL bLauncherResult = FALSE;

	// "서버" 버전.
	int nDownServerVersion = DNPATCHINFO.GetServerVersion();
	
	// "모듈" 버전.
	int nLocalModuleVersion  = DNFIRSTPATCHINFO.GetLocalModuleVersion();
	LogWnd::TraceLog(_T("서버버전: [%d] / 모듈버전: [%d]"), nDownServerVersion, nLocalModuleVersion );

	if( nLocalModuleVersion == nDownServerVersion )		// * 버전이 같다면, 더이상 진행할 이유가 없음 *
	{
		LogWnd::Log(LogLevel::Error, _T("ModulePatch - Version is Concur"));
		return FPR_OK;
	}
	else if( nLocalModuleVersion > nDownServerVersion ) // * 버전 꼬임. 풀패치로 넘김 *
	{
		LogWnd::Log(LogLevel::Error, _T("ModulePatch - Version Check Failed, Go to FullPatch"));
		return FPR_NEED_FULLPATCH;
	}
	else if( nLocalModuleVersion == -1 )
	{
		LogWnd::Log(LogLevel::Error, _T("ModulePatch - Module Version is -1"));
		return FPR_NEED_FULLPATCH;
	}
	
	// * 패치 시작! * 
	for( int i = nLocalModuleVersion ; i < nDownServerVersion ;  )
	{
		if( i < nDownServerVersion )
		{
			i++;

			// 모듈 패치
			bModuleResult = DownLoadModulePatch(i);	// FirstPatchList.txt 다운로드.

			if(bModuleResult) // FirstPatchList.txt파일이 존재한다면 실행.
			{
				if( !ParsingModuleList() )	// "FirstPatchList.txt" 파싱
				{
					return FPR_FAIL;
				}

				if( !ChangeModuleFiles(i) )	// 모듈 파일 변경.
				{
					return FPR_FAIL;
				}
			}

			// 런처 패치
			bLauncherResult = PatchLauncherFiles(i); // * Launcher.exe파일 다운로드 & 적용 *

			// 현재 모듈 버전을 저장. ( 변수에 저장.. )
			DNFIRSTPATCHINFO.SetLocalModuleVersion(i);

			// 버전 변경
			CString szPath;
			szPath = DNPATCHINFO.GetClientPath();
			szPath += CLIENT_VERSION_NAME;

			// version.cfg파일에만 저장.
			if( !SaveModuleVersionFileWithOutPakSave(szPath, i) )
			{
				return FPR_FAIL;
			}

			// DnLuancher.exe가 패치됬다면, 런처 재구동.
			if( bLauncherResult )
			{
				return FPR_LAUNCHER_PATCH;
			}
		}
	} // end of for

	return FPR_OK;
}

BOOL CDnFistPatchDownloadThread::DownLoadModulePatch( int nVersion )
{
	LogWnd::TraceLog( L"ModuleFile DownLoad Start" );

	CString strVersion;
	strVersion.Format( _T( "%08d" ), nVersion );
	LogWnd::TraceLog( _T("Download Launcher Patch Version [%s]"), strVersion.GetBuffer() );

	CString strUrl;
	strUrl += DNPATCHINFO.GetPatchUrl();
	strUrl += strVersion;
	strUrl += _T("/FirstPatch/");
	
	CString szPatchListUrl; // PatchURL / FirstPatch / FirstPatchList.txt
	szPatchListUrl.Format( _T( "%s%s" ), strUrl.GetBuffer(), FIRSTPATCHLIST_NAME );
	
	// 클라이언트 로컬 파일저장 경로
	m_strPatchListFile.Format( _T( "%s%s" ), DNPATCHINFO.GetClientPath().GetBuffer() , FIRSTPATCHLIST_NAME);
	
	BOOL	bResult = DeleteUrlCacheEntry( szPatchListUrl.GetBuffer() );
	HRESULT hr		= DownloadToFile( szPatchListUrl.GetBuffer(), m_strPatchListFile.GetBuffer() );

	// 다운로드에 실패했다면.
	if( hr != S_OK )
	{
		LogWnd::TraceLog( _T("FirstPatch.txt Download Failed!") );
		return FALSE;
	}
	LogWnd::TraceLog( L"FirstPatch.txt File Download Success" );
	
	// 파일 속성 체크.
	if( ::GetFileAttributes( m_strPatchListFile.GetBuffer() ) == -1 )
	{
		LogWnd::TraceLog( L"Failed! GetFileAttributes=[%s]", m_strPatchListFile.GetBuffer() );
		return FALSE;
	}

	LogWnd::TraceLog( L"ModuleFile DownLoad End" );
	return TRUE;
}

BOOL CDnFistPatchDownloadThread::PatchLauncherFiles( int nVersion )
{
	LogWnd::TraceLog( _T("DnLauncher.exe FIle Download Start") );

	CString strVersion;
	strVersion.Format( _T( "%08d" ), nVersion );

	CString strUrl;
	strUrl += DNPATCHINFO.GetPatchUrl();
	strUrl += strVersion;
	strUrl += _T("/FirstPatch/");
	
	// Down URL 주소.
	CString szPatchListUrl;
	szPatchListUrl.Format( _T( "%s%s" ), strUrl.GetBuffer(), _T(DNLAUNCHER_NAME));
	
	if( !IsExistFile( szPatchListUrl ) )
		return FALSE;

	// Client 로컬 경로 (저장되는 장소)
	m_strPatchListFile.Format( _T( "%s%s" ), DNPATCHINFO.GetClientPath().GetBuffer() , _T(DNLAUNCHER_NAME));
	
	// 기존의 확장자 exe를 tmp로 변경한다.
	if( _access(DNLAUNCHER_NAME , 0) == 0 )
	{
		MoveFile(_T(DNLAUNCHER_NAME), _T(DNLAUNCHER_NAME_TMP));
	}

	HRESULT hr = E_FAIL;
	for(int i = 0 ; i < RETRY_MAX_COUNT ; ++i)
	{
		DeleteUrlCacheEntry( szPatchListUrl.GetBuffer() );
		hr = DownloadToFile( szPatchListUrl.GetBuffer(), m_strPatchListFile.GetBuffer() );

		if( hr == S_OK )
		{
			break;
		}
	}
	LogWnd::TraceLog( _T("DnLauncher.exe FIle Download Success") );

	// 다운로드에 실패했다면.
	if( hr != S_OK )
	{
		MoveFile(_T(DNLAUNCHER_NAME_TMP), _T(DNLAUNCHER_NAME)); // 'tmp' ---> 'exe'로 다시 복구.
		LogWnd::TraceLog( _T("Download DnLauncher.exe File Failed!") );
		return FALSE;
	}
	
	// 파일 속성 체크.
	if( ::GetFileAttributes( m_strPatchListFile.GetBuffer() ) == -1 )
	{
		LogWnd::TraceLog( L"Failed! GetFileAttributes=[%s]", m_strPatchListFile.GetBuffer() );
		return FALSE;
	}

	return TRUE;
}

int CDnFistPatchDownloadThread::ParsingModuleList()
{
	m_vecCopyList.clear();
	m_vecDeleteList.clear();

	//m_strPostPatchList.Format( _T( "%sFirstPatch%08d.txt" ), DNPATCHINFO.GetClientPath(), m_nServerModuleVersion ); // testLauncher/FirstPatch00000007.txt 이런식.
	HANDLE hFile = CreateFile( m_strPatchListFile.GetBuffer(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	if( hFile == INVALID_HANDLE_VALUE )
	{
		LogWnd::TraceLog( _T("%s File Not Found"), m_strPatchListFile.GetBuffer() );
		return FALSE;
	}

	char	cPatchCode, szString[1024], *pFindPtr;
	int		nFileSize	= GetFileSize( hFile, NULL );
	char*	pBuffer		= new char [ nFileSize + 3 ]; // 파일 끝이 없을경우 pBuffer + 2 한 후에 pBuffer 가 쓰레기값이여서 strchr 에서 뻑날 수 있다.
	memset( pBuffer, 0, nFileSize + 3 );

	char*	pBufferBackup = pBuffer;
	DWORD	dwReadSize;
	ReadFile( hFile, pBuffer, nFileSize, &dwReadSize, NULL );
	
	while( 1 )
	{
		cPatchCode = *pBuffer;
		pBuffer += 2;
		pFindPtr = strchr( pBuffer, 0x0d );

		if( pFindPtr == NULL )
			break;

		if( (int)( pBuffer - pBufferBackup ) > (int)dwReadSize )
			break;

		*pFindPtr = 0;
		strcpy_s( szString, 1024, pBuffer );
		int nStrLength = ( int )strlen( szString );

		if( ( szString[ nStrLength - 1 ] == 0x0d ) || ( ( szString[ nStrLength - 1 ] == 0x0a ) ) )
			szString[ nStrLength - 1 ] = 0;

		pBuffer += nStrLength + 1;
		if( *pBuffer == 0x0a )
			pBuffer++;

		switch( cPatchCode )
		{
		case 'D':
			m_vecDeleteList.push_back( szString );
			break;
		case 'C':
			m_vecCopyList.push_back( szString );
			break;
		default:
			assert( 0 && "Invalid Patch Code" );
			SAFE_DELETE_ARRAY( pBufferBackup );
			CloseHandle( hFile );
			return FALSE;
		}
	}
	CloseHandle( hFile );
	SAFE_DELETE_ARRAY( pBufferBackup );

	// PostPatchList.txt. 파일삭제
	DeleteFile(m_strPatchListFile);

	return true;
}

int CDnFistPatchDownloadThread::ChangeModuleFiles(int nVer)
{
	USES_CONVERSION;

	CString strOriginalFileName,strBackUpFileName;
	WCHAR	wszFileName[_MAX_PATH];

	std::vector<std::string>::iterator it = m_vecCopyList.begin();
	for( ; it != m_vecCopyList.end() ; ++it )
	{
		// WCHAR로 변환.
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, (*it).c_str(), -1, wszFileName, _MAX_PATH);
		
		// "DnLauncher.exe"는 여기서 처리하지 않는다.
		if( wcscmp(wszFileName, _T("DnLauncher.exe")) == 0 )
		{
			continue;
		}
		
		// 1. 원본파일이름 변경.
		strOriginalFileName.Format(_T("%s%s"),	DNPATCHINFO.GetClientPath(), wszFileName);
		strBackUpFileName.Format(_T("%s%s%s"),	DNPATCHINFO.GetClientPath(), _T("BackUp_"), wszFileName);
		rename(CT2A(strOriginalFileName), CT2A(strBackUpFileName));

		// 2. 새로운 파일 다운로드.
		m_strFirstPatchURLPath.Format(_T("%s%08d%s%s"), DNPATCHINFO.GetPatchUrl(), nVer, _T("/FirstPatch/"),wszFileName);

		HRESULT hr;
		for( int i = 0 ; i < RETRY_MAX_COUNT ; ++i )
		{
			DeleteUrlCacheEntry( m_strFirstPatchURLPath.GetBuffer() );
			hr = DownloadToFile( m_strFirstPatchURLPath.GetBuffer(), CVarArg<__MAX_PATH>(wszFileName)); // DNPATCHINFO.GetClientPath().GetBuffer() );

			if(hr == S_OK)
			{
				ClientDeleteFile(strBackUpFileName); // "BackUp_파일" 삭제.
				break;
			}
		}

		// 3. 실패 : 기존파일 이름 복원.
		if( hr != S_OK )
		{
			rename(CT2A(strBackUpFileName), CT2A(strOriginalFileName)); // 원래 이름으로 복구
		}
	}

	
	//-------------------------------------------
	// 삭제목록 처리
	//-------------------------------------------
	std::vector<std::string>::iterator itDelete = m_vecDeleteList.begin();
	for( ; itDelete != m_vecDeleteList.end() ; ++itDelete )
	{
		MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, (*itDelete).c_str(), _MAX_PATH, wszFileName, _MAX_PATH);
		ClientDeleteFile(wszFileName);
	}

	return true;
}

BOOL CDnFistPatchDownloadThread::SaveModuleVersionToPak( int nVersion )
{
	CString szFindPackingFile;

	szFindPackingFile =  DNPATCHINFO.GetClientPath();
	szFindPackingFile += _T("Resource00.pak");

	CEtPackingFile *pPackingFile = new CEtPackingFile();

	char strVersion[256]={0,};
	sprintf_s( strVersion, sizeof(strVersion), "version %d\r\nModule %d", DNPATCHINFO.GetClientVersion(), nVersion );

	USES_CONVERSION;

	char szTemp[ _MAX_PATH ] = {0,};
	char StrVersionName[32]= "\\version.cfg";
	WideCharToMultiByte( CP_ACP, 0, szFindPackingFile.GetBuffer(), -1, szTemp, _MAX_PATH, NULL, NULL );

	// 어쩔수 없다. 512M 넘어가는것중에 골라서 추가하자. 
	if( pPackingFile->OpenFileSystem( szTemp ) )
	{
		pPackingFile->Remove( StrVersionName );
		pPackingFile->AddFile( StrVersionName, strVersion, sizeof(strVersion) );
	}
	else 
	{
		AfxMessageBox( _T("Version can not be saved") );
		SAFE_DELETE( pPackingFile );
		return FALSE;
	}

	pPackingFile->CloseFileSystem();

	SAFE_DELETE( pPackingFile );
	return TRUE;
}

BOOL CDnFistPatchDownloadThread::SaveModuleVersionFile( CString& szFilePath, int nVersion )
{
	if( !SaveModuleVersionToPak( nVersion ) )
	{
		return FALSE;
	}

	char* buffer = NULL;
	HANDLE	hFile = INVALID_HANDLE_VALUE;

	hFile = CreateFile( szFilePath.GetBuffer(), GENERIC_WRITE, FILE_SHARE_READ , NULL, CREATE_ALWAYS , FILE_ATTRIBUTE_NORMAL,NULL );

	if( hFile == INVALID_HANDLE_VALUE )
	{
		LogWnd::Log( LogLevel::Error, L"Save Version File Failed!" );
		return FALSE;
	}

	char szVersion[ 256 ]= {0,};
	DWORD dwWriteSize;
	
	sprintf_s( szVersion, sizeof(szVersion), "Version %d\r\nModule %d", DNPATCHINFO.GetClientVersion(), nVersion );
	
	WriteFile( hFile, szVersion,(int)strlen(szVersion)+1 , &dwWriteSize, NULL );
	CloseHandle(hFile);

	return TRUE;
}

BOOL CDnFistPatchDownloadThread::SaveModuleVersionFileWithOutPakSave( CString& szFilePath, int nVersion )
{
	char* buffer = NULL;
	HANDLE	hFile = INVALID_HANDLE_VALUE;

	hFile = CreateFile( szFilePath.GetBuffer(), GENERIC_WRITE, FILE_SHARE_READ , NULL, CREATE_ALWAYS , FILE_ATTRIBUTE_NORMAL,NULL );

	if( hFile == INVALID_HANDLE_VALUE )
	{
		LogWnd::Log( LogLevel::Error, L"Save Version File Failed!" );
		return FALSE;
	}

	char szVersion[ 256 ]= {0,};
	DWORD dwWriteSize;

	sprintf_s( szVersion, sizeof(szVersion), "Version %d\r\nModule %d", DNPATCHINFO.GetClientVersion(), nVersion );

	WriteFile( hFile, szVersion,(int)strlen(szVersion)+1 , &dwWriteSize, NULL );
	CloseHandle(hFile);

	return TRUE;
}


//////////////////////////////////////////////////////////////////////////
// Patch Download Thread
//////////////////////////////////////////////////////////////////////////

CDnPatchDownloadThread*	g_pPatchDownloadThread = NULL;
CSyncLock				g_PatchThreadLock;

CDnPatchDownloadThread::CDnPatchDownloadThread()
: m_hHttpConnection( NULL )
, m_hWnd( NULL )
, m_bTerminateThread( FALSE )
, m_fRate( 0.0f )
, m_fPercent( 0.0f )
, m_nCurPatchCount( 0 )
, m_nTotalPatchCount( 0 )
, m_dwDownloadFileLength( 0 )
, m_dwTempFileLength( 0 )
, m_dwTotalBytes( 0 )
, m_dwTotalFileLength( 0 )
, m_emDownPatchState( PATCH_NONE )
{
	g_pPatchDownloadThread = this;
}

CDnPatchDownloadThread::~CDnPatchDownloadThread()
{
	m_bThreadLoop = FALSE;

	m_vecPackingFile.clear();
	m_vecDeleteList.clear();
	m_vecPatchList.clear();
	m_vecCopyList.clear();
	m_vecReserveIndex.clear();
	
	::TerminateThread( m_hHandleThread, 0 );
	CloseHandle( m_hHandleThread );
	WaitForTerminate( 1000 );
	g_pPatchDownloadThread = NULL;
}

void CDnPatchDownloadThread::SendStatusMsg( DownloadPatchState nPatchState )
{
	m_emDownPatchState = nPatchState;
	
	if( m_hWnd )
		PostMessage( m_hWnd, WM_STATUS_MSG, m_emDownPatchState, 0 );
}

void CDnPatchDownloadThread::Run()
{
	LogWnd::TraceLog( L"★Patch Start!" );
	if( m_hWnd == NULL )
		return;

	SetThreadName( m_nThreadID, "CDnPatchDownloadThread" );
	ScopeLock<CSyncLock> lock( g_PatchThreadLock );
	
#if defined(_KR_NEXON)
	if( g_pWiseLog ) g_pWiseLog->WriteToWiseLog( "versioncheckstart.aspx" );
#endif // _KR_NEXON

	PatchReturn nRet = PR_FAIL;

	nRet = BeginPatch();

	switch( nRet )
	{
	case PR_OK:
		{
			LogWnd::TraceLog( L"★Patch Success!" );
			SendStatusMsg( PATCH_COMPLETE );
#if defined(_KR_NEXON)
			if( g_pWiseLog ) g_pWiseLog->SendWebPost( "patchend.aspx" );
#endif // _KR_NEXON
		}
		break;
	case PR_LAUNCHER_PATCH:
		{
			LogWnd::TraceLog( _T( "★Launcher Restart because Launcher Patched."));
#ifndef _DEBUG
			if( g_hMutex )
			{
				CloseHandle( g_hMutex );
				g_hMutex = NULL;
			}
#endif // _DEBUG
			CString strExeFile = DNPATCHINFO.GetClientPath();
			strExeFile += DNLAUNCHER_NAME;

			ShellExecute( m_hWnd, NULL, strExeFile, g_szOriginalCmdLine.GetBuffer(), NULL, SW_SHOWNORMAL );
			SendStatusMsg( PATCH_LAUNCHER );
		}
		break;
	case PR_FULLPATCH:
		{
			ProcessFullVersionPatch();
		}
		break;
	case PR_FAIL:
		{
			LogWnd::TraceLog( L"★Patch Failed!" );
			SendStatusMsg( PATCH_FAILED );
		}
		break;
	case PR_TERMINATE:
		{
			LogWnd::TraceLog( L"★Patch Terminate!" );
			SendStatusMsg( PATCH_TERMINATE );
		}
		break;
	}
}

PatchReturn CDnPatchDownloadThread::BeginPatch()
{
	LogWnd::TraceLog( L"BeginPatch - Start" );

	PatchReturn nRetValue = PR_FAIL;

	SendStatusMsg( PATCH_CHECK_VERSION );
	
	int nCurClientVersion = DNPATCHINFO.GetClientVersion();
	int nCurServerVersion = DNPATCHINFO.GetServerVersion();

	LogWnd::Log( LogLevel::Info, L"Patch Check Version (Client Ver:%d, Server Ver:%d)", nCurClientVersion, nCurServerVersion );

	if( nCurClientVersion == -1 || nCurServerVersion == -1 )	// 현재 버전과 서버 버전을 확인을 못했다. 풀패치로 넘어가야한다.
	{
		LogWnd::TraceLog( _T("Version Check Failed Run Fullpatch Process"));
		SendStatusMsg( PATCH_CHECK_VERSION_FAILED );
		return PR_FULLPATCH;
	}

#if defined(_KR_NEXON)
	if( g_pWiseLog ) g_pWiseLog->WriteToWiseLog( "versioncheckend.aspx" );
#endif // _KR_NEXON

	if( nCurClientVersion == nCurServerVersion )			// 서버와 클라이언트 버전이 같으면 패치할 필요가 없다.
		return PR_OK;
	else if( nCurClientVersion > nCurServerVersion )		// 클라이언트 버전이 서버 버전보다 크면 풀패치
		return PR_FULLPATCH;
	else													// 클라이언트 버전이 낮을 경우 패치 진행
	{
		int nDownVersion = nCurClientVersion + 1;
#if defined(_KR_NEXON)
		if( g_pWiseLog ) g_pWiseLog->WriteToWiseLog("patchstart.aspx");
#endif // _KR_NEXON

		for( int i=nDownVersion; i<=nCurServerVersion; i++ )
		{
			g_DownloadInfoData.m_nCurrentFileCount = i - nCurClientVersion;
			g_DownloadInfoData.m_nTotalFileCount = nCurServerVersion - nCurClientVersion;
			g_DownloadInfoData.m_nDownloadProgressMin = 0;
			g_DownloadInfoData.m_nDownloadProgressMax = 100;
			
			// 롤백패치 대응 다운받을 버젼을 항시 체크 해서 스킵할 버전이면 스킵한다.
			std::vector<int>::iterator itSkip = std::find( DNPATCHINFO.GetSkipVersion().begin(), DNPATCHINFO.GetSkipVersion().end(), i );
			if( itSkip != DNPATCHINFO.GetSkipVersion().end() )
			{
				CString szPath;
				szPath = DNPATCHINFO.GetClientPath();
				szPath += CLIENT_VERSION_NAME;
				
				int gap = nCurServerVersion - i;
				if( gap >= 1 )	//버젼 차이가 2이상 나면 롤백한 버젼까지 바로 이동
				{
					DNPATCHINFO.SetClientVersion( i+1 );
					SaveVersionFile( szPath, i+1 );
					i++;
				}
				else 
				{
					DNPATCHINFO.SetClientVersion( i );
					SaveVersionFile( szPath, i );
				}
				continue;
			}

			SendStatusMsg( PATCH_DOWNLOAD_DATA );
			int nFailCount = 0;
#ifdef _FIRST_PATCH
			BOOL bOnlyFirstPatch = FALSE;	// 모듈패치만 있는 경우
#endif // _FIRST_PATCH

			while(1)
			{
				BOOL bResult = DownLoadPatch( i );
				if( bResult )
				{
					if( m_bTerminateThread ) return PR_TERMINATE; // 다운로드 완료 시 종료 처리 안되면 packing까지 넘어가기 때문에 종료가 오래 걸림
					break;
				}
#ifdef _FIRST_PATCH
				if( CheckExistFirstPatch( i ) )	// 모듈패치만 있는 패치버전인지 체크
				{
					bOnlyFirstPatch = TRUE;
					break;
				}
#endif // _FIRST_PATCH
				nFailCount++;

				if( nFailCount < RETRY_MAX_COUNT )
				{
					SendStatusMsg( PATCH_DOWNLOAD_DATA );
				}
				else
				{
					if( i != nCurServerVersion )
					{
						// 패치 다운로드 실패했고.. 가장 최신패치 다운로드해서 성공하면
						// 너무 오래된 버전이라서 이버전의 패치가 삭제 됐으므로 풀버전 다운로드로 넘긴다.
						CString strPatchFileUrl;
						strPatchFileUrl.Format( _T( "%s%08d/Patch%08d.txt" ), DNPATCHINFO.GetPatchUrl().GetBuffer(), nCurServerVersion, nCurServerVersion );
						BOOL bResult = DeleteUrlCacheEntry( strPatchFileUrl.GetBuffer() );

						HRESULT hr = URLDownload( strPatchFileUrl.GetBuffer(), DNPATCHINFO.GetClientPath().GetBuffer(), _T( "DownTest.tmp" ) );
						if( hr == S_OK )
						{
							ClientDeleteFile( _T( "DownTest.tmp" ) );
							LogWnd::TraceLog ( _T("Patch Version too Old Run Fullpatch Process"));
							return PR_FULLPATCH;
						}
					}
					LogWnd::TraceLog( _T( "%d Version Patch Download Failed!"), i );
					return PR_FAIL;
				}

				if( m_bTerminateThread ) return PR_TERMINATE;
			}

#ifdef _FIRST_PATCH
			if( bOnlyFirstPatch ) continue;
#endif // _FIRST_PATCH

			nRetValue = PreApplyPakPatch();
			if( nRetValue != PR_OK )
				return nRetValue;

			g_DownloadInfoData.m_nDownloadProgressMin = 0;
			g_DownloadInfoData.m_nDownloadProgressMax = 100;
			SendStatusMsg( PATCH_APPLY_PAK_FILE );	// 설치 프로그래스 진행을 위해 프로그래스 Min/Max 초기화

			// 받은 패치를 적용한다.
			nRetValue = ApplyPakPatch();

			if( nRetValue == PR_TERMINATE )
				break;
#ifdef _USE_RTPATCH
			DeleteFolder( m_strRTPatchTempFolder.c_str() );			// RTPatch적용 임시 폴더 삭제
#endif // _USE_RTPATCH

			ClientDeleteFile( m_strPatchListFile.GetBuffer() );		// ApplyPak완료 후 txt파일은 바로 삭제

			if( nRetValue == PR_OK || nRetValue == PR_LAUNCHER_PATCH )
			{
				ClientDeleteFile( m_strPatchFile.GetBuffer() );		// 다운로드 받은 Pak파일은 Apply 성공시에만 삭제

				LogWnd::TraceLog( _T( "%d Version Patch Apply Success!"), i );
				DNPATCHINFO.SetClientVersion( i );
				SendStatusMsg( PATCH_APPLY_PAK_FILE_END );

				CString szPath;
				szPath = DNPATCHINFO.GetClientPath();
				szPath += CLIENT_VERSION_NAME;
				
				if( !SaveVersionFile( szPath, i ) )	// Version.cfg에 바뀐 버전 저장
				{
					nRetValue = PR_FAIL;
					break;
				}

				if( nRetValue == PR_LAUNCHER_PATCH )	// 런처 패치가 되었다면 중단하고 새 런처를 띄우고 패치한다.
					break;
			}
			else	// 패치하다가 에러 났다.. 첨부터 다시 깔아야 겠다..
			{
				SendStatusMsg( PATCH_APPLY_PAK_FILE_FAILED );
				LogWnd::TraceLog( _T( "%d Version Patch Apply Failed!"), i );
				return PR_FULLPATCH;
			}
		}
	}

	LogWnd::TraceLog( L"BeginPatch - End" );

	return nRetValue;
}



BOOL CDnPatchDownloadThread::DownLoadPatch( int nVersion )
{
	LogWnd::TraceLog( L"Patch Step 1 - DownloadPatch Start" );

	CString strVersion;
	strVersion.Format( _T( "%08d" ), nVersion );
	LogWnd::TraceLog( _T("Download Patch Version [%s]"), strVersion.GetBuffer() );

	CString strUrl;
	strUrl += DNPATCHINFO.GetPatchUrl();
	strUrl += strVersion;
	strUrl += _T("/");

	CString strPatchFileUrl, strPatchFileName;
	strPatchFileUrl.Format( _T( "%sPatch%08d.pak" ), strUrl.GetBuffer(), nVersion );
	strPatchFileName.Format( _T( "Patch%08d.pak" ), nVersion );

	BOOL bResult = DeleteUrlCacheEntry( strPatchFileUrl.GetBuffer() );
	HRESULT hr = URLDownload( strPatchFileUrl.GetBuffer(), DNPATCHINFO.GetClientPath().GetBuffer(), strPatchFileName.GetBuffer() );

	if( hr != S_OK )
	{
#ifdef _USE_SPLIT_COMPRESS_FILE
		HRESULT hr = DownLoadSplitFile( strUrl, nVersion );
		if( hr != S_OK )
		{
			LogWnd::Log( LogLevel::Error, L"Download Failed!");
			return FALSE;
		}
#else // _USE_SPLIT_COMPRESS_FILE
		LogWnd::Log( LogLevel::Error, L"Download Failed!");
		return FALSE;
#endif // _USE_SPLIT_COMPRESS_FILE
	}

	if( ::GetFileAttributes( m_strPatchFile.GetBuffer() ) == -1 )
	{
		LogWnd::Log( LogLevel::Error, L"Failed! GetFileAttributes=[%s]", m_strPatchFile.GetBuffer() );
		return FALSE;
	}

	LogWnd::TraceLog( L"Download txt File");
	CString szPatchListUrl;
	szPatchListUrl.Format( _T( "%sPatch%08d.txt" ), strUrl.GetBuffer(), nVersion );
	m_strPatchListFile.Format( _T( "%sPatch%08d.txt" ), DNPATCHINFO.GetClientPath().GetBuffer(), nVersion );

	bResult = DeleteUrlCacheEntry( szPatchListUrl.GetBuffer() );
	hr = DownloadToFile( szPatchListUrl.GetBuffer(), m_strPatchListFile.GetBuffer() );

	if( hr != S_OK )
	{
		LogWnd::TraceLog( _T("Download txt File Failed!") );
		return FALSE;
	}
	LogWnd::TraceLog( L"Download txt File Success" );

	if( ::GetFileAttributes( m_strPatchListFile.GetBuffer() ) == -1 )
	{
		LogWnd::TraceLog( L"Failed! GetFileAttributes=[%s]", m_strPatchListFile.GetBuffer() );
		return FALSE;
	}

	LogWnd::TraceLog( _T("Patch Step 1 - DownloadPatch End") );
	return TRUE;
}

#ifdef _USE_SPLIT_COMPRESS_FILE
HRESULT CDnPatchDownloadThread::URLDownload( LPCTSTR strPatchFileUrl, LPCTSTR strFilePath, LPCTSTR strFileName,
											BOOL bSplitDownload, DWORD dwAlreadyDownloadSize, DWORD dwRemainDownloadFileSize )
#else // _USE_SPLIT_COMPRESS_FILE
HRESULT CDnPatchDownloadThread::URLDownload( LPCTSTR strPatchFileUrl, LPCTSTR strFilePath, LPCTSTR strFileName )
#endif // _USE_SPLIT_COMPRESS_FILE
{
	LogWnd::TraceLog( L"URLDownload Start" );

	HINTERNET hInternetSession;
	// 세션 열기
	LogWnd::TraceLog( L"Open Internet Session" );
	hInternetSession = InternetOpen( NULL, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0 );

	if( hInternetSession == NULL )
	{
		LogWnd::Log( LogLevel::Error, L"Internet Session Open Failed!" );
		return S_FALSE;
	}

	m_strPatchFileUrl = strPatchFileUrl;
	m_strPatchFile.Format( L"%s%s", strFilePath, strFileName );
	m_strPatchFileLocal.Format( L"%s.tmp", m_strPatchFile );

	// 다운로드 받기 전 파일은 먼저 체크
	CFile file;
	FileCheck filecheck;
	DWORD dwDownloadFileSize;
#ifdef _USE_SPLIT_COMPRESS_FILE
	filecheck = CheckPatchFile( hInternetSession, strFileName, file, dwDownloadFileSize, bSplitDownload );
#else // _USE_SPLIT_COMPRESS_FILE
	filecheck = CheckPatchFile( hInternetSession, strFileName, file, dwDownloadFileSize );
#endif // _USE_SPLIT_COMPRESS_FILE

	if( filecheck == FIlE_EXIST )
		return S_OK;
	else if( filecheck == FILE_FALSE )
		return S_FALSE;

	if( m_dwTempFileLength < dwDownloadFileSize )
	{
		// Http 열기
		LogWnd::TraceLog( L"Open Http Connection" );
		CString strAddHeader;
		strAddHeader.Format( L"Range:bytes=%d-\nCache-Control:no-cache\nPragma:no-cache", m_dwTempFileLength );

		m_hHttpConnection = InternetOpenUrl( hInternetSession, m_strPatchFileUrl, strAddHeader, strAddHeader.GetLength(), 
											INTERNET_FLAG_RELOAD | INTERNET_FLAG_DONT_CACHE, 0 );

		if( m_hHttpConnection == NULL )
		{
			LogWnd::Log( LogLevel::Error, L"Http Connection Open Failed!" );
			file.Close();	// http connection error
			InternetCloseHandle( hInternetSession );
			return S_FALSE;
		}

		TCHAR szBuffer[MAX_PATH]={0,};
		DWORD dwBufferSize = MAX_PATH;
		DWORD dwIndex = 0;

		// 다운받아야 할 파일 크기 구하기
		if( HttpQueryInfo( m_hHttpConnection, HTTP_QUERY_CONTENT_LENGTH, (LPVOID)&szBuffer, &dwBufferSize, &dwIndex ) != FALSE )
		{
			m_dwDownloadFileLength = (DWORD)_wtoi( szBuffer );
#ifdef _USE_SPLIT_COMPRESS_FILE
			m_dwTotalFileLength = m_dwTempFileLength + m_dwDownloadFileLength + dwRemainDownloadFileSize;
#else // _USE_SPLIT_COMPRESS_FILE
			m_dwTotalFileLength = m_dwTempFileLength + m_dwDownloadFileLength;
#endif // _USE_SPLIT_COMPRESS_FILE
		}

		LogWnd::TraceLog( L"DownLoad Left File Size : %d", m_dwDownloadFileLength );
		// 다운로드 시작 (이어받기)
		if( m_dwDownloadFileLength > 0 )
		{
			LogWnd::TraceLog( L"Download File Start" );

			DWORD dwRead, dwSize;
			DWORD dwStartTick, dwCurrentTick;
			m_dwTotalBytes = m_dwTempFileLength;

			char szReadBuf[32768];	// 32KiB

			dwStartTick = GetTickCount();
			BOOL bError = FALSE;

			do 
			{
				if( m_bTerminateThread )
					break;

				if( !InternetQueryDataAvailable( m_hHttpConnection, &dwSize, 0, 0 ) )
				{
					LogWnd::Log( LogLevel::Error, L"InternetQueryDataAvailable : data not available!" );
					bError = TRUE;	// error "data not available!"
					break;
				}

				if( !InternetReadFile( m_hHttpConnection, szReadBuf, dwSize, &dwRead ) )
				{
					LogWnd::Log( LogLevel::Error, L"InternetReadFile : reading file fail!" );
					bError = TRUE;	// error "reading file fail!"
					break;
				}

				if( dwRead )
				{
					dwCurrentTick = GetTickCount();
					file.Write( szReadBuf, dwRead );

					m_dwTotalBytes += dwRead;

					if( dwCurrentTick > dwStartTick )
					{
						float fDivideTick = static_cast<float>( dwCurrentTick - dwStartTick ) / 1000.0f;
						m_fRate = static_cast<float>( m_dwTotalBytes ) / 1000.0f / fDivideTick;
						m_fPercent = static_cast<float>( m_dwTotalBytes ) / static_cast<float> ( m_dwTotalFileLength ) * 100.0f;
					}
#ifdef _USE_SPLIT_COMPRESS_FILE
					g_DownloadInfoData.m_nDownloadProgressMin = m_dwTotalBytes + dwAlreadyDownloadSize;
#else // _USE_SPLIT_COMPRESS_FILE
					g_DownloadInfoData.m_nDownloadProgressMin = m_dwTotalBytes;
#endif // _USE_SPLIT_COMPRESS_FILE
					g_DownloadInfoData.m_nDownloadProgressMax = m_dwTotalFileLength;
					g_DownloadInfoData.m_fDownloadRate = m_fRate;
//					TRACE( L"Read : [%09d / %09d] --- [%4.2f KB/s] < %4.0f%% >\n", m_dwTotalBytes, m_dwTotalFileLength, m_fRate, m_fPercent );
				}

			} while( dwRead != 0 );

			if( m_bTerminateThread || bError )	// 터미네이트 되었거나 패킷전송 받을 때 오류가 있을 경우
			{
				file.Close();
				InternetCloseHandle( m_hHttpConnection );
				InternetCloseHandle( hInternetSession );
				m_hHttpConnection = NULL;
				return S_FALSE;
			}
			LogWnd::TraceLog( L"Download File Success" );
		}
		InternetCloseHandle( m_hHttpConnection );
	}
	file.Close();

#ifdef _USE_SPLIT_COMPRESS_FILE
	if( !bSplitDownload )
	{
		LogWnd::TraceLog( L"MD5 Check Start" );
		// 올바르게 다운로드 되었는지 MD5 Check
		if( !GetMD5Checksum() )
		{
			LogWnd::Log( LogLevel::Error, L"MD5 Check Failed!" );
			SendStatusMsg( PATCH_MD5_CHECK_FAILED );
			ClientDeleteFile( m_strPatchFileLocal );
			InternetCloseHandle( hInternetSession );
			return S_FALSE;	// md5 check error
		}
		LogWnd::TraceLog( L"MD5 Check Success" );
	}
#else // _USE_SPLIT_COMPRESS_FILE
	LogWnd::TraceLog( L"MD5 Check Start" );
	// 올바르게 다운로드 되었는지 MD5 Check
#ifndef _USA
	if( !GetMD5Checksum() )
	{
		LogWnd::Log( LogLevel::Error, L"MD5 Check Failed!" );
		SendStatusMsg( PATCH_MD5_CHECK_FAILED );
		ClientDeleteFile( m_strPatchFileLocal );
		InternetCloseHandle( hInternetSession );
		return S_FALSE;	// md5 check error
	}
#endif
	LogWnd::TraceLog( L"MD5 Check Success" );
#endif // _USE_SPLIT_COMPRESS_FILE

	// 정리 tmp -> 원래 파일명
	CString strNewPatchFileLocal = m_strPatchFileLocal.Left( m_strPatchFileLocal.GetLength() - 4 );
	CFile::Rename( m_strPatchFileLocal, strNewPatchFileLocal );

	InternetCloseHandle( hInternetSession );
	LogWnd::TraceLog( L"URLDownload End" );

	return S_OK;
}

#ifdef _USE_SPLIT_COMPRESS_FILE
FileCheck CDnPatchDownloadThread::CheckPatchFile( HINTERNET hInternetSession, LPCTSTR strFileName, 
												 CFile& file, DWORD& dwDownloadFileSize, BOOL bSplitDownload )
#else // _USE_SPLIT_COMPRESS_FILE
FileCheck CDnPatchDownloadThread::CheckPatchFile( HINTERNET hInternetSession, LPCTSTR strFileName, 
												 CFile& file, DWORD& dwDownloadFileSize )
#endif // _USE_SPLIT_COMPRESS_FILE
{
	// 다운 받을 파일 크기 구하기
	dwDownloadFileSize = GetDownloadFileSize( hInternetSession, m_strPatchFileUrl );

	if( dwDownloadFileSize == 0 )
	{
		LogWnd::Log( LogLevel::Error, L"Get Download File Size Failed!" );
		InternetCloseHandle( hInternetSession );
		return FILE_FALSE;
	}

	LogWnd::TraceLog( L"Download Total File Size : %d", dwDownloadFileSize );

#ifdef _USE_SPLIT_COMPRESS_FILE
	if( !bSplitDownload )
	{
		g_DownloadInfoData.m_strFileName = strFileName;
		g_DownloadInfoData.m_nFileSize = dwDownloadFileSize;
	}
#else // _USE_SPLIT_COMPRESS_FILE
	g_DownloadInfoData.m_strFileName = strFileName;
	g_DownloadInfoData.m_nFileSize = dwDownloadFileSize;
#endif // _USE_SPLIT_COMPRESS_FILE

	// 다운 완료된 파일이 있을 경우 유효성 검사
	LogWnd::TraceLog( L"Check Downloaded Temp File" );
	CFileException e;

	if( file.Open( m_strPatchFile, CFile::modeRead, &e ) )
	{
		DWORD dwFileLength = static_cast<DWORD>( file.GetLength() );
		file.Close();
		if( dwFileLength == dwDownloadFileSize )	// 이미 다 받아진 파일이 있음
		{
			LogWnd::Log( LogLevel::Error, L"Already Exist Download Temp File!" );
			InternetCloseHandle( hInternetSession );
			return FIlE_EXIST;
		}
		else	// 받았으나 서버의 파일 정보와 상이할 경우 삭제 후 다시 받음
		{
			LogWnd::Log( LogLevel::Error, L"Already Exist Download Temp File But Delete Wrong File!" );
			if( !ClientDeleteFile( m_strPatchFile ) )
			{
				InternetCloseHandle( hInternetSession );
				return FILE_FALSE;
			}
		}
	}
	else
	{
		if( e.m_cause != CFileException::fileNotFound )
			RecordFileExceptionLog( e );
	}

	// 기존 다운받던 tmp 파일 체크
	FILE* fp;
	fp = _wfopen( m_strPatchFileLocal, _T("r") );
	UINT uiFileModeFlag = CFile::modeCreate | CFile::modeWrite | CFile::shareDenyNone;

	if( fp != NULL )	// 다운받던 tmp파일이 있을 경우
	{
		fclose( fp );
		uiFileModeFlag |= CFile::modeNoTruncate;
	}

	if( !file.Open( m_strPatchFileLocal, uiFileModeFlag, &e ) )
	{
		LogWnd::Log( LogLevel::Error, L"Downloaded Temp File Open Failed!" );
		RecordFileExceptionLog( e );
		InternetCloseHandle( hInternetSession );
		return FILE_FALSE;	// file open error
	}

	file.SeekToEnd();
	m_dwTempFileLength = static_cast<DWORD>( file.GetLength() );

	return FIlE_OK;
}

DWORD CDnPatchDownloadThread::GetDownloadFileSize( HINTERNET hInternetSession, CString strHttpPath )
{
	if( hInternetSession == NULL )
		return 0;

	m_hHttpConnection = InternetOpenUrl( hInternetSession, strHttpPath, NULL, 0, INTERNET_FLAG_RELOAD | INTERNET_FLAG_DONT_CACHE, 0 );

	if( m_hHttpConnection == NULL )
		return 0;

	if( !IsExistFile( m_hHttpConnection ) )
	{
		LogWnd::Log( LogLevel::Error, L"Download File Is Not Exist!" );
		InternetCloseHandle( m_hHttpConnection );
		return 0;
	}

	TCHAR szBuffer[MAX_PATH]={0,};
	DWORD dwBufferSize = MAX_PATH;
	DWORD dwIndex = 0;
	DWORD dwFileSize = 0;

	// 다운받아야 할 파일 크기 구하기
	if( HttpQueryInfo( m_hHttpConnection, HTTP_QUERY_CONTENT_LENGTH, (LPVOID)&szBuffer, &dwBufferSize, &dwIndex ) != FALSE )
		dwFileSize = (DWORD)_wtoi( szBuffer );

	InternetCloseHandle( m_hHttpConnection );
	return dwFileSize;
}

BOOL CDnPatchDownloadThread::GetMD5Checksum()
{
	SendStatusMsg( PATCH_MD5_CHECK );

	CString strHttpMD5 = m_strPatchFileUrl + L".MD5";
	CString strLocalMD5 = m_strPatchFileLocal + L".MD5";

	HRESULT hr = E_FAIL;
	for( int i = 0 ; i < RETRY_MAX_COUNT ; i++ )
	{
		BOOL bResult = DeleteUrlCacheEntry( strHttpMD5 );
		hr = DownloadToFile( strHttpMD5, strLocalMD5 );
		if( hr == S_OK	)
			break;
	}

	if( FAILED( hr ) )
	{
		LogWnd::Log( LogLevel::Error, L"MD5 Check File Download Failed!, Error : %ld", hr );
		return FALSE;
	}

	CString strDownloadChecksum;
	CFile fileMD5;
	CFileException e;

	if( !fileMD5.Open( strLocalMD5, CFile::modeRead, &e ) )
		return FALSE;

	char pBuf[100];
	fileMD5.Read( pBuf, 100 );
	fileMD5.Close();
	ClientDeleteFile( strLocalMD5 );

	strDownloadChecksum = pBuf;
	int nIndex = strDownloadChecksum.Find('\n');
	strDownloadChecksum = strDownloadChecksum.Left( nIndex - 1 );

	CString strLocalChecksum = CMD5Checksum::GetMD5( m_strPatchFileLocal );
	// MD5 비교
	return wcscmp( strLocalChecksum, strDownloadChecksum ) == 0 ? TRUE : FALSE;
}

BOOL CDnPatchDownloadThread::LoadPatchList()
{
	m_vecPackingFile.clear();
	m_vecDeleteList.clear();
	m_vecPatchList.clear();
	m_vecCopyList.clear();

	HANDLE hFile = CreateFile( m_strPatchListFile.GetBuffer(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	if( hFile == INVALID_HANDLE_VALUE )
	{
		LogWnd::TraceLog( _T("%s File Not Found"), m_strPatchListFile.GetBuffer() );
		return FALSE;
	}
	char cPatchCode, szString[1024], *pFindPtr;
	int nFileSize = GetFileSize( hFile, NULL );
	char *pBuffer = new char [ nFileSize + 3 ]; // 파일 끝이 없을경우 pBuffer + 2 한 후에 pBuffer 가 쓰레기값이여서 strchr 에서 뻑날 수 있다.
	memset( pBuffer, 0, nFileSize + 3 );

	char *pBufferBackup = pBuffer;
	DWORD dwReadSize;
	ReadFile( hFile, pBuffer, nFileSize, &dwReadSize, NULL );
	while( 1 )
	{
		cPatchCode = *pBuffer;
		pBuffer += 2;
		pFindPtr = strchr( pBuffer, 0x0d );
		
		if( pFindPtr == NULL )
			break;
		
		if( (int)( pBuffer - pBufferBackup ) > (int)dwReadSize )
			break;

		*pFindPtr = 0;
		strcpy_s( szString, 1024, pBuffer );
		int nStrLength = ( int )strlen( szString );
		
		if( ( szString[ nStrLength - 1 ] == 0x0d ) || ( ( szString[ nStrLength - 1 ] == 0x0a ) ) )
			szString[ nStrLength - 1 ] = 0;

		pBuffer += nStrLength + 1;
		if( *pBuffer == 0x0a )
			pBuffer++;

		switch( cPatchCode )
		{
		case 'D':
			m_vecDeleteList.push_back( szString );
			break;
		case 'C':
			m_vecCopyList.push_back( szString );
			break;
		default:
			assert( 0 && "Invalid Patch Code" );
			SAFE_DELETE_ARRAY( pBufferBackup );
			CloseHandle( hFile );
			return FALSE;
		}
	}
	CloseHandle( hFile );
	SAFE_DELETE_ARRAY( pBufferBackup );

	return TRUE;
}

PatchReturn CDnPatchDownloadThread::PreApplyPakPatch()
{
	LogWnd::TraceLog( L"Patch Step 2 - PreApplyPakPatch Start" );

	//패치 상태 리스트 정보를 읽어온다. 
	if( !LoadPatchList() )
	{
		ErrorMessageBoxLog( _S( STR_PATCH_STATE_LIST_NOT_READ + DNPATCHINFO.GetLanguageOffset() ) );
		return PR_FAIL;
	}

	PatchReturn nRetValue = GetPackingFile();	// 리소스 패킹 파일을 찾고 정보를 가져옴

	if( m_vecPackingFile.empty() )
	{
		LogWnd::Log( LogLevel::Error, _T( "*.pak File Not Found!") );
		return PR_FULLPATCH;
	}

	LogWnd::TraceLog( _T( "Patch Step 2 - PreApplyPakPatch End") );

	return nRetValue;
}

PatchReturn CDnPatchDownloadThread::ApplyPakPatch()
{
	LogWnd::TraceLog( L"Patch Step 3 - ApplyPakPatch Start" );
	m_nCurPatchCount = 0;
	m_nTotalPatchCount = 0;

	char szTemp[__MAX_PATH] = {0,};
	CEtPackingFile PatchSource;
	WideCharToMultiByte( CP_ACP, 0, m_strPatchFile.GetBuffer(), -1, szTemp, __MAX_PATH, NULL, NULL );
	if( !PatchSource.OpenFileSystem( szTemp, true ) )
		return PR_FAIL;

	PatchReturn nRetValue = ApplyDeleteList( PatchSource );
	if( nRetValue != PR_OK )
		return nRetValue;

#ifdef _USE_RTPATCH
	char szString[_MAX_PATH];
	CString strClientPath = DNPATCHINFO.GetClientPath();
	strClientPath += "RTPatchFileTemp";
	WideCharToMultiByte( CP_ACP, 0, strClientPath.GetBuffer(), -1, szString, _MAX_PATH, NULL, NULL );
	m_strRTPatchTempFolder = szString;

	DeleteFolder( m_strRTPatchTempFolder.c_str() );
	CreateFolder( m_strRTPatchTempFolder.c_str() );

	char strRTPatchPakFile[_MAX_PATH];
	CString strFileName = DNPATCHINFO.GetClientPath();
	strFileName += "RTPatchFileTemp\\RTPatch.pak";
	WideCharToMultiByte( CP_ACP, 0, strFileName.GetBuffer(), -1, strRTPatchPakFile, _MAX_PATH, NULL, NULL );

	CEtPackingFile RTPatchSource;
	if( !RTPatchSource.NewFileSystem( strRTPatchPakFile ) )
		return PR_FAIL;

	nRetValue = ApplyRTPatch( PatchSource, RTPatchSource );
	if( nRetValue != PR_OK )
		return PR_FAIL;

	PatchSource.CloseFileSystem();

	nRetValue = ApplyPatchList( RTPatchSource );
	if( nRetValue != PR_OK )
		return nRetValue;

	nRetValue = ApplyCopyList( RTPatchSource );

	RTPatchSource.CloseFileSystem();
#else // _USE_RTPATCH
	nRetValue = ApplyPatchList( PatchSource );
	if( nRetValue != PR_OK )
		return nRetValue;

	nRetValue = ApplyCopyList( PatchSource );
#endif // _USE_RTPATCH
	for( int i=0; i<(int)m_vecPackingFile.size(); i++ )
	{
		m_vecPackingFile[ i ]->CloseFileSystem();
		SAFE_DELETE(m_vecPackingFile[i]);
	}

#ifndef _USE_RTPATCH
	PatchSource.CloseFileSystem();
#endif // _USE_RTPATCH

	LogWnd::TraceLog( _T( "Patch Step 3 - ApplyPakPatch End") );

	return nRetValue;
}

PatchReturn CDnPatchDownloadThread::GetPackingFile()
{
	LogWnd::TraceLog( L"GetPackingFile" );

	PatchReturn nRetValue = PR_OK;

	char szTemp[__MAX_PATH] = {0,};
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	CString szFindPackingFile;
	szFindPackingFile = DNPATCHINFO.GetClientPath();
	szFindPackingFile += "Resource*.pak";
	hFind = FindFirstFile( szFindPackingFile.GetBuffer(), &FindFileData );

	while( hFind != INVALID_HANDLE_VALUE ) 
	{
		if( !( FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) )
		{
			CString szPackingFile;
			szPackingFile = DNPATCHINFO.GetClientPath();
			szPackingFile += FindFileData.cFileName;

			CEtPackingFile *pPackingFile = new CEtPackingFile();
			char szTemp[__MAX_PATH] = {0,};
			WideCharToMultiByte( CP_ACP, 0, szPackingFile.GetBuffer(), -1, szTemp, __MAX_PATH, NULL, NULL );
			if( pPackingFile->OpenFileSystem( szTemp ) )
			{
				m_vecPackingFile.push_back( pPackingFile );
			}
			else
			{
				if( pPackingFile->OpenFileSystem( szTemp ) )
				{
					m_vecPackingFile.push_back( pPackingFile );
				}
				else
				{
					LogWnd::Log( LogLevel::Error, L"%s Packing File System Open Failed!", szPackingFile );
					SAFE_DELETE( pPackingFile );
					nRetValue = PR_FAIL;
				}
			}
		}
		if( !FindNextFile( hFind, &FindFileData ) ) break;
		if( m_bTerminateThread )
		{
			for( int m=0; m<(int)m_vecPackingFile.size(); m++ )
			{
				m_vecPackingFile[m]->CloseFileSystem();
				SAFE_DELETE( m_vecPackingFile[m] );
			}
			return PR_TERMINATE;
		}
	}
	FindClose( hFind );

	return nRetValue;
}

PatchReturn CDnPatchDownloadThread::ApplyDeleteList( CEtPackingFile& PatchSource )
{
	LogWnd::TraceLog( L"ApplyDeleteList" );

	m_nTotalPatchCount = (int)m_vecDeleteList.size() + PatchSource.GetFileCount();

	for( int i=0; i<(int)m_vecDeleteList.size(); i++ )
	{
		char szPath[__MAX_PATH+100], szFileName[__MAX_PATH+100], szDelFile[__MAX_PATH] ={ 0,};
		_GetPath( szPath, sizeof(szPath), m_vecDeleteList[ i ].c_str() );
		_GetFullFileName( szFileName, sizeof(szFileName), m_vecDeleteList[ i ].c_str() );
		BOOL bIsNoramlFile = TRUE; 

		for( int j=0; j<(int)m_vecPackingFile.size(); j++ )
		{
			m_vecPackingFile[j]->ChangeDir( "\\" );
			if( szPath[0] )
				m_vecPackingFile[j]->ChangeDir( szPath );

			if( m_vecPackingFile[j]->Remove( szFileName ) )
			{
				// 삭제 했으면 다음 파일로 넘어간다.
				m_nCurPatchCount++;
				g_DownloadInfoData.m_nDownloadProgressMin = m_nCurPatchCount;
				g_DownloadInfoData.m_nDownloadProgressMax = m_nTotalPatchCount;
				bIsNoramlFile = FALSE;
				break;
			}
		}

		if( bIsNoramlFile )
		{
#ifdef _TEST	// 테스트용 런처의 경우 경로가 임의로 셋팅될 수 있으므로 아래처럼 처리
			TCHAR szCurDir[ _MAX_PATH ]={0,};
			GetCurrentDirectory( _MAX_PATH, szCurDir );
			SetCurrentDirectory( DNPATCHINFO.GetClientPath() );
#endif // _TEST
			//일반 디렉토리 삭제 가능 추가. 
			char CheckExe[256]={0 ,};
			_GetExt( CheckExe, sizeof(CheckExe), m_vecDeleteList[i].c_str() );
			if( strlen( CheckExe ) == 0 )
			{
				DeleteFolder( m_vecDeleteList[i].c_str() );
			}
			else
			{
				DeleteFileA( CVarArgA<MAX_PATH>(".\\%s", m_vecDeleteList[ i ].c_str()) );
				m_nCurPatchCount++;
				g_DownloadInfoData.m_nDownloadProgressMin = m_nCurPatchCount;
				g_DownloadInfoData.m_nDownloadProgressMax = m_nTotalPatchCount;
			}
#ifdef _TEST
			SetCurrentDirectory( szCurDir );
#endif // _TEST
		}

		if( m_bTerminateThread )
		{
			for( int m=0; m<(int)m_vecPackingFile.size(); m++ )
			{
				m_vecPackingFile[ m ]->CloseFileSystem();
				SAFE_DELETE(m_vecPackingFile[ m ]);
			}
			PatchSource.CloseFileSystem();
			return PR_TERMINATE;
		}
	}

	return PR_OK;
}

PatchReturn CDnPatchDownloadThread::PatchFileExist( int nPatchFileCount, CEtPackingFile& PatchSource, SPackingFileInfo* pFileInfo, BOOL& bPatch )
{
	if( pFileInfo == NULL )
		return PR_FAIL;

	for( int i=0; i<(int)m_vecPackingFile.size(); i++ )
	{
		int nFindIndex = m_vecPackingFile[i]->FindFile( pFileInfo->szFileName );
		if( nFindIndex != -1 )
		{
			SPackingFileInfo *pFindFileInfo = m_vecPackingFile[i]->GetFileInfo( nFindIndex );

			if( pFindFileInfo->dwAllocSize >= pFileInfo->dwCompressSize )
			{
				CEtFileHandle *pFileHandle = PatchSource.OpenOnly( pFileInfo->szFileName );
				if( pFileHandle == NULL )
					return PR_FAIL;

				m_vecPackingFile[i]->RemoveFileInfo( nFindIndex );
				int nFindEmptyIndex = m_vecPackingFile[i]->FindSuitableEmptySpace( pFileInfo->dwCompressSize );
				if( nFindEmptyIndex == -1 )	// 적당한 공간이 없으면 예약걸기
				{
					stFileIndexSet fileIndex;
					fileIndex.m_dwPakFileNum = i;
					fileIndex.m_dwPatchFileIndex = nPatchFileCount;
					fileIndex.m_dwReserveFileIndex = m_vecPackingFile[i]->ReservePatchFile( pFileInfo );

					m_vecReserveIndex.push_back( fileIndex );
				}
				else	// 적당한 공간이 있다면 패치
				{
					if( !m_vecPackingFile[i]->PatchFileIndex( pFileHandle, nFindEmptyIndex ) )
					{
						LogWnd::Log( LogLevel::Error, L"Patch File Failed!" );
						PatchSource.CloseFile( pFileHandle );
						return PR_FAIL;
					}
				}
				PatchSource.CloseFile( pFileHandle );
				bPatch = TRUE;
			}
			else	// 패치 파일의 크기가 더 클 경우 지우고 빠짐
			{
				// 전채 패킹 파일에서 삭제 시도하도록 바꿈
				BOOL bRemove = FALSE;
				for( int j=0; j<(int)m_vecPackingFile.size(); j++ )
				{
					if( m_vecPackingFile[j]->Remove( pFileInfo->szFileName ) )
						bRemove = TRUE;
				}

				if( !bRemove )
				{
					LogWnd::Log( LogLevel::Error, L"Remove File Failed!" );
					return PR_FAIL;
				}
			}
			break;
		}
	}

	return PR_OK;
}

PatchReturn CDnPatchDownloadThread::PatchFileSuitableSpace( CEtPackingFile& PatchSource, SPackingFileInfo* pFileInfo, BOOL& bPatch )
{
	if( pFileInfo == NULL )
		return PR_FAIL;

	for( int j=0; j<(int)m_vecPackingFile.size(); j++ )
	{
		int nFindIndex = m_vecPackingFile[j]->FindSuitableEmptySpace( pFileInfo->dwCompressSize );

		if( nFindIndex != -1 )
		{
			CEtFileHandle *pFileHandle = PatchSource.OpenOnly( pFileInfo->szFileName );
			if( pFileHandle == NULL )
				return PR_FAIL;

			if( !m_vecPackingFile[j]->PatchFileIndex( pFileHandle, nFindIndex ) )
			{
				LogWnd::Log( LogLevel::Error, L"Patch File Failed!" );
				PatchSource.CloseFile( pFileHandle );
				return PR_FAIL;
			}
			PatchSource.CloseFile( pFileHandle );
			bPatch = TRUE;
			break;
		}
	}

	return PR_OK;
}

void CDnPatchDownloadThread::PatchFileReserve( int nPatchFileCount, SPackingFileInfo* pFileInfo )
{
	// 패치할 위치를 찾지 못했으면 제일 적당한 위치를 찾아서 넣어준다
	int nBestIndex = -1;
	DWORD dwMinSize = ULONG_MAX;
	for( int j=0; j<(int)m_vecPackingFile.size(); j++ )
	{
		DWORD dwPackingFileSize = m_vecPackingFile[j]->GetFileSystemSize();
		// 512메가 보다는 작아야 밀어 넣을 수 있다
		if( ( dwPackingFileSize < dwMinSize ) && ( dwPackingFileSize < (DWORD)DEFAULT_PACKING_FILE_SIZE ) )
		{
			dwMinSize = dwPackingFileSize;
			nBestIndex = j;
		}
	}

	if( nBestIndex == -1 )
	{
		// 패치할 적당한 파일을 못찾았으면 새로운 패치파일을 만든다.
		int nStartFileIndex = ( int )m_vecPackingFile.size();
		while( 1 )
		{
			HANDLE hFind;
			WIN32_FIND_DATA FindFileData;
			CString szNewPackingFile;
			char szTemp[__MAX_PATH] = {0,};
			szNewPackingFile.Format( _T( "%sResource%02d.pak" ), DNPATCHINFO.GetClientPath().GetBuffer(), nStartFileIndex );
			hFind = FindFirstFile( szNewPackingFile.GetBuffer(), &FindFileData );
			if( hFind == INVALID_HANDLE_VALUE )
			{
				WideCharToMultiByte( CP_ACP, 0, szNewPackingFile.GetBuffer(), -1, szTemp, __MAX_PATH, NULL, NULL );
				CEtPackingFile *pNewPackingFile = new CEtPackingFile();
				pNewPackingFile->NewFileSystem( szTemp );
				m_vecPackingFile.push_back( pNewPackingFile );
				nBestIndex = (int)m_vecPackingFile.size() - 1;
				break;
			}
			nStartFileIndex++;
			FindClose( hFind );
		}
	}

	if( nBestIndex != -1 )
	{
		stFileIndexSet fileIndex;
		fileIndex.m_dwPakFileNum = nBestIndex;
		fileIndex.m_dwPatchFileIndex = nPatchFileCount;
		fileIndex.m_dwReserveFileIndex = m_vecPackingFile[ nBestIndex ]->ReservePatchFile( pFileInfo );

		m_vecReserveIndex.push_back( fileIndex );
	}
}

PatchReturn CDnPatchDownloadThread::ApplyPatchList( CEtPackingFile& PatchSource )
{
	LogWnd::TraceLog( L"ApplyPatchList" );

	m_vecReserveIndex.clear();
	int nPatchFileCount = PatchSource.GetFileCount();

	for( int i=0; i<nPatchFileCount; i++ )
	{
		BOOL bSkip = FALSE;
		SPackingFileInfo* pFileInfo = PatchSource.GetFileInfo( i );

		if( pFileInfo == NULL )
			continue;

		for( int j=0; j<(int)m_vecCopyList.size(); j++ )
		{
			// 파일이름 앞에.. \ 붙어 있어서 그거 제외한 이름으로 비교해야 제대로 된다.
			// 카피리스트에 있는것은 팩에 들어가면 안됨 패스
			if( _stricmp( m_vecCopyList[j].c_str(), pFileInfo->szFileName + 1 ) == 0 )
			{
				bSkip = TRUE;
				break;
			}
		}

		if( bSkip )
			continue;

		BOOL bPatch = FALSE;
		if( PatchFileExist( i, PatchSource, pFileInfo, bPatch ) == PR_FAIL )	// 기존에 존재하는 파일인지 검사해서 패치가능하면 패치
			return PR_FAIL;
		if( !bPatch )
		{
			if( PatchFileSuitableSpace( PatchSource, pFileInfo, bPatch ) == PR_FAIL )	// 적당한 공간이 있다면 그곳에 패치
				return PR_FAIL;
			if( !bPatch )
				PatchFileReserve( i, pFileInfo );	// 기존에 존재하지도 적당한 공간도 없다면 공간이 남은 Pak파일의 뒤에 붙이기 위해서 예약만 걸어둠
		}
		
		if( bPatch )
		{
			m_nCurPatchCount++;
			g_DownloadInfoData.m_nDownloadProgressMin = m_nCurPatchCount;
			g_DownloadInfoData.m_nDownloadProgressMax = m_nTotalPatchCount;
		}

		if( m_bTerminateThread ) 
		{
			for( int m=0; m<(int)m_vecPackingFile.size(); m++ )
			{
				m_vecPackingFile[m]->CloseFileSystem();
				SAFE_DELETE(m_vecPackingFile[m]);
			}
			PatchSource.CloseFileSystem();
			return PR_TERMINATE;
		}
	}

	// 예약된 공간을 비워두고 헤더를 Write
	for( int j=0; j<(int)m_vecPackingFile.size(); j++ )
		m_vecPackingFile[ j ]->WriteReserveFileInfo();

	// 예약된 패치파일들 패치
	std::vector<stFileIndexSet>::iterator iter = m_vecReserveIndex.begin();
	for( ; iter != m_vecReserveIndex.end(); iter++ )
	{
		SPackingFileInfo* pFileInfo = PatchSource.GetFileInfo( (*iter).m_dwPatchFileIndex );

		CEtFileHandle* pFileHandle = PatchSource.OpenOnly( pFileInfo->szFileName );
		if( pFileHandle == NULL )
			return PR_FAIL;

		if( !m_vecPackingFile[ (*iter).m_dwPakFileNum ]->PatchFileIndex( pFileHandle, (*iter).m_dwReserveFileIndex, FALSE ) )	// 이미 예약된 FileInfo이므로 FileInfo 갱신하지 않음
		{
			LogWnd::Log( LogLevel::Error, L"Patch File Failed!" );
			PatchSource.CloseFile( pFileHandle );
			return PR_FAIL;
		}
		PatchSource.CloseFile( pFileHandle );

		m_nCurPatchCount++;
		g_DownloadInfoData.m_nDownloadProgressMin = m_nCurPatchCount;
		g_DownloadInfoData.m_nDownloadProgressMax = m_nTotalPatchCount;
	}

	return PR_OK;
}

PatchReturn CDnPatchDownloadThread::ApplyCopyList( CEtPackingFile& PatchSource )
{
	PatchReturn nRetValue = PR_OK;
	LogWnd::TraceLog( L"ApplyCopyList" );

#ifdef _TEST	// 테스트용 런처의 경우 경로가 임의로 셋팅될 수 있으므로 아래처럼 처리
	TCHAR szCurDirectory[ _MAX_PATH ]={0,};
	GetCurrentDirectory( _MAX_PATH, szCurDirectory );
	SetCurrentDirectory( DNPATCHINFO.GetClientPath() );
#endif // _TEST

	for( int i=0; i<(int)m_vecCopyList.size(); i++ )
	{
		//팩안에는 \ 파일앞에 붙기떔시 비교를위해서 붙여준다. 
		char szAddCopyListName[_MAX_PATH]={0,};
		sprintf_s( szAddCopyListName, "\\%s", m_vecCopyList[ i ].c_str() );
		CEtFileHandle *pFileHandle = PatchSource.OpenFile( szAddCopyListName );

		if( pFileHandle )
		{
			char szPath[__MAX_PATH] = {0,};
			char szCurDir[__MAX_PATH] = {0,};

			int nSize1 = (int)strlen( szPath );
			int nSize2 = (int)strlen( m_vecCopyList[i].c_str() );

			if ( nSize1 > _MAX_PATH || nSize2 > _MAX_PATH )
				LogWnd::TraceLog( _T( "nSize1 > _MAX_PATH || nSize2 > _MAX_PATH ") );

			_GetPath( szPath, sizeof(szPath), m_vecCopyList[i].c_str() );

			RemoveSpaceA( m_vecCopyList[i] );

			if( _stricmp( m_vecCopyList[i].c_str(), DNLAUNCHER_NAME ) == NULL )
			{
				ClientDeleteFile( _T( DNLAUNCHER_NAME_TMP ) );
				
				// DNLAUNCHER_NAME가 존재해야하며, DNLAUNCHER_NAME를 DNLAUNCHER_NAME_TMP로 변경한다.
				if( _access( DNLAUNCHER_NAME, 0 ) == 0 && MoveFile( _T( DNLAUNCHER_NAME ), _T( DNLAUNCHER_NAME_TMP ) ) == 0 )	// 런처 파일명을 바꾸고 실행하더라도 런처패치가 되도록 함
				{
					CString strError;
					strError.Format( L"%s %d", _S( STR_LAUNCHER_PATCH_FAIL + DNPATCHINFO.GetLanguageOffset() ), 1 );
					ErrorMessageBoxLog( strError );

					PatchSource.CloseFile( pFileHandle );
					PatchSource.CloseFileSystem();
					LogWnd::Log( LogLevel::Error, _T( "Launcher Patch Failed!") );
					return PR_FAIL;
				}
				LogWnd::TraceLog( _T( "Apply Launcher Patch") );
				nRetValue = PR_LAUNCHER_PATCH;
			}

			if( szPath[0] )
			{
				CreateFolder( szPath );
				GetCurrentDirectoryA( __MAX_PATH, szCurDir );

				char szTemp[ __MAX_PATH ] = {0,} ;
				strcpy_s( szTemp, __MAX_PATH, szCurDir );
				strcat_s( szTemp, __MAX_PATH, "\\" );
				strcat_s( szTemp, __MAX_PATH, szPath );
				SetCurrentDirectoryA( szTemp );
			}

#if defined(_KR_NEXON)
			if( _stricmp( m_vecCopyList[i].c_str(), "nmcogame.dll" ) == NULL )
				MoveFile( _T( "nmcogame.dll" ), _T( "nmcogame.tmp" ) );
			if( _stricmp( m_vecCopyList[i].c_str(), "nmconew.dll" ) == NULL )
				MoveFile( _T( "nmconew.dll" ), _T( "nmconew.tmp" ) );
#endif // _KR_NEXON

			if( !pFileHandle->ExportFile() )
			{
				if( _stricmp( pFileHandle->GetFileContext()->szFileName, DNLAUNCHER_NAME ) == 0 )
				{
					CString strError;
					strError.Format( L"%s %d", _S( STR_LAUNCHER_PATCH_FAIL + DNPATCHINFO.GetLanguageOffset() ), 2 );
					ErrorMessageBoxLog( strError );
				}
				else 
					ErrorMessageBoxLog( _S( STR_FILE_COPY_FAIL + DNPATCHINFO.GetLanguageOffset() ) );

				LogWnd::TraceLog( _T( "%s File Copy Failed! (%d)"), pFileHandle->GetFileContext()->szFileName, GetLastError() );
				PatchSource.CloseFile( pFileHandle );
				PatchSource.CloseFileSystem();
				return PR_FAIL;
			}

			if( szPath[0] )
				SetCurrentDirectoryA( szCurDir );

			PatchSource.CloseFile( pFileHandle );

#if defined(_KR_NEXON)
			if( _stricmp( m_vecCopyList[i].c_str(), "nmcogame.dll" ) == NULL )
				MoveFile( _T( "nmcogame.dll" ), _T( "nmcogame.new" ) );
			if( _stricmp( m_vecCopyList[i].c_str(), "nmconew.dll" ) == NULL )
				MoveFile( _T( "nmconew.dll" ), _T( "nmconew.new" ) );
#endif // _KR_NEXON

			m_nCurPatchCount++;
			g_DownloadInfoData.m_nDownloadProgressMin = m_nCurPatchCount;
			g_DownloadInfoData.m_nDownloadProgressMax = m_nTotalPatchCount;

			if( m_bTerminateThread ) 
			{
				for( int m=0; m<(int)m_vecPackingFile.size(); m++ )
				{
					m_vecPackingFile[m]->CloseFileSystem();
					SAFE_DELETE(m_vecPackingFile[m]);
				}
				PatchSource.CloseFileSystem();
				return PR_TERMINATE;
			}
		}
	}

#ifdef _TEST
	SetCurrentDirectory( szCurDirectory );
#endif // _TEST
	return nRetValue;
}

BOOL CDnPatchDownloadThread::ProcessFullVersionPatch()
{
	LogWnd::TraceLog( _T( "ProcessFullVersionPatch" ) );
#if defined(_KR_NEXON)
	if( g_pServiceModule )
		g_pServiceModule->OnForceFullVersionPatch();
	SendMessage( m_hWnd, WM_CLOSE, 0, 0 );
	return FALSE;
#else // _KR_NEXON
	ErrorMessageBoxLog( _S( STR_PATCH_NEED_FULL_VERSION + DNPATCHINFO.GetLanguageOffset() ) );
	SendMessage( m_hWnd, WM_CLOSE, 0, 0 );
	return FALSE;
#endif // _KR_NEXON
}

BOOL CDnPatchDownloadThread::SaveNewVersionToPak( int nVersion )
{
	CString szFindPackingFile;

	szFindPackingFile =  DNPATCHINFO.GetClientPath();
	szFindPackingFile += _T("Resource00.pak");

	CEtPackingFile *pPackingFile = new CEtPackingFile();

	char strVersion[256]={0,};
	//sprintf_s( strVersion, sizeof(strVersion), "version %d", nVersion );
	sprintf_s( strVersion, sizeof(strVersion), "version %d\r\nModule %d", nVersion , DNFIRSTPATCHINFO.GetLocalModuleVersion() );
	

	USES_CONVERSION;

	char szTemp[ _MAX_PATH ] = {0,};
	char StrVersionName[32]= "\\version.cfg";
	WideCharToMultiByte( CP_ACP, 0, szFindPackingFile.GetBuffer(), -1, szTemp, _MAX_PATH, NULL, NULL );

	// 어쩔수 없다. 512M 넘어가는것중에 골라서 추가하자. 
	if( pPackingFile->OpenFileSystem( szTemp ) )
	{
		pPackingFile->Remove( StrVersionName );
		pPackingFile->AddFile( StrVersionName, strVersion, sizeof(strVersion) );
	}
	else 
	{
		AfxMessageBox( _T("Version can not be saved") );
		SAFE_DELETE( pPackingFile );
		return FALSE;
	}

	pPackingFile->CloseFileSystem();

	SAFE_DELETE( pPackingFile );
	return TRUE;
}

BOOL CDnPatchDownloadThread::SaveVersionFile( CString& szFilePath, int nVersion )
{
	if( !SaveNewVersionToPak( nVersion ) )
		return FALSE;

	char* buffer = NULL;
	HANDLE	hFile = INVALID_HANDLE_VALUE;

	hFile = CreateFile( szFilePath.GetBuffer(), GENERIC_WRITE, FILE_SHARE_READ , NULL, CREATE_ALWAYS , FILE_ATTRIBUTE_NORMAL,NULL );

	if( hFile == INVALID_HANDLE_VALUE )
	{
		LogWnd::Log( LogLevel::Error, L"Save Version File Failed!" );
		return FALSE;
	}

	char szVersion[ 256 ]= {0,};
	DWORD dwWriteSize;
	//sprintf_s( szVersion, 256, "Version %d\r\n", nVersion );
	sprintf_s( szVersion, 256, "Version %d\r\nModule %d", nVersion , DNFIRSTPATCHINFO.GetLocalModuleVersion() );
	WriteFile( hFile, szVersion,(int)strlen(szVersion)+1 , &dwWriteSize, NULL );
	CloseHandle(hFile);

	return TRUE;
}

void CDnPatchDownloadThread::RecordFileExceptionLog( CFileException& e )
{
	CString strTmp = _T("");
	switch (e.m_cause)
	{
	case CFileException::genericException:
		strTmp = _T("An unspecified error occurred.");
		break;
	case CFileException::fileNotFound:
		strTmp = _T("The file could not be located.");
		break;
	case CFileException::badPath:
		strTmp = _T("All or part of the path is invalid.");
		break;
	case CFileException::tooManyOpenFiles:
		strTmp = _T("The permitted number of open files was exceeded.");
		break;
	case CFileException::accessDenied:
		strTmp = _T("The file could not be accessed.");
		break;
	case CFileException::invalidFile:
		strTmp = _T("There was an attempt to use an invalid file handle.");
		break;
	case CFileException::removeCurrentDir:
		strTmp = _T("The current working directory cannot be removed.");
		break;
	case CFileException::directoryFull:
		strTmp = _T("There are no more directory entries.");
		break;
	case CFileException::badSeek:
		strTmp = _T("There was an error trying to set the file pointer.");
		break;
	case CFileException::hardIO:
		strTmp = _T("There was a hardware error.");
		break;
	case CFileException::sharingViolation:
		strTmp = _T("SHARE.EXE was not loaded, or a shared region was locked.");
		break;
	case CFileException::lockViolation:
		strTmp = _T("There was an attempt to lock a region that was already locked.");
		break;
	case CFileException::diskFull:
		strTmp = _T("The disk is full.");
		break;
	case CFileException::endOfFile:
		strTmp = _T("The end of file was reached.");
		break;
	default:
		strTmp = _T("EOCS - Can't Find Error Description.");
		break;
	}

	LogWnd::Log( LogLevel::Error, L"File Open Exception : %d - %s", e.m_cause, strTmp );
}

#ifdef _USE_RTPATCH

static void far* g_pCallBackParam = NULL;
static BOOL g_bInCallBack = FALSE;
static BOOL g_bAbortPatch = FALSE;
static char g_strOriginFileName[_MAX_PATH] = {0,};	// 원본 파일 명 ( ApplyPatch32 실행시 CallBack Function에서 셋팅한다. )

LPVOID CALLBACK EXPORT CDnPatchDownloadThread::RTPatchCALLBACK( UINT Id, LPVOID Param )
{
	//FARPROC lpProcDlg;
	//DLGPROC lpProcDlg;

	MSG msg;
	UINT MsgCount;
	LPVOID RetVal;

	g_pCallBackParam = Param;
	g_bInCallBack = TRUE;
	RetVal = "";

	for( MsgCount = MAX_MESSAGES; MsgCount && PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ); MsgCount-- )
	{
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}

	switch( Id )
	{
		case 1:		// Warning message header
		case 2:		// Warning message text
		case 3:		// Error message header
			break;
		case 4:		// Error message text
			{
				CString strErrorMessage( (char*)Param );
				LogWnd::Log( LogLevel::Error, L"RTPatchCALLBACK Error %s", strErrorMessage );
			}
			break;
		case 9:		// progress message
		case 0xa:	// help message
		case 0xb:	// patch file comment
		case 0xc:	// copyright message
		case 5:		// % completed
		case 6:		// Number of patch files PATCHGUI ignores this
			break;
		case 7:		// begin patch
			{
				sprintf_s( g_strOriginFileName, _MAX_PATH, (char*)Param );
				CString strFileName( (char*)Param );
				LogWnd::Log( LogLevel::Error, L"RTPatchCALLBACK Begin RTPatch %s", strFileName );
			}
			break;
		case 8:		// end patch
			{
				LogWnd::Log( LogLevel::Error, L"RTPatchCALLBACK End RTPatch!" );
			}
			break;
		case 0xd:	// this one shouldn't happen (only occurs if the command line doesn't have a patch file in it, and we insure that it does).
		case 0xe:	// this one shouldn't happen either (same reason)
		case 0xf:	// Password Dialog
		case 0x10:	// Invalid Password Alert
		case 0x11:	// Disk Change Dialog
		case 0x12:	// Disk Change Alert
		case 0x13:	// Confirmation Dialog
		case 0x14:	// Location Dialog
		case 0x16:	// Searching Call-back
		case 0x15:	// Idle...
		default:
			break;
	}

	// do a few more messages while we're here...
	for( MsgCount = MAX_MESSAGES; MsgCount && PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ); MsgCount-- )
	{
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}

	g_bInCallBack = false;

	if( g_bAbortPatch ) // all of our call-back dialogs set this
		return NULL;
	else
		return RetVal;
}

BOOL CDnPatchDownloadThread::ExportFile( CEtPackingFile& PakFile, const char* pExportFileName, const char* pCreateFilePath )
{
	CEtFileHandle* pFileHandle = NULL;
	pFileHandle = PakFile.OpenFile( pExportFileName );

	if( pFileHandle == NULL )
		return FALSE;

	int nFileSize = pFileHandle->GetFileContext()->dwOriginalSize;
	char* pBuff = new char[nFileSize];
	if( pFileHandle->Read( pBuff, nFileSize ) == 0 )
		return FALSE;

	PakFile.CloseFile( pFileHandle );

	char szString[_MAX_PATH];
	char szFullFileName[_MAX_PATH];
	_GetFullFileName( szFullFileName, pExportFileName );
	sprintf_s( szString, _MAX_PATH, "%s\\%s", pCreateFilePath, szFullFileName );

	WCHAR szCreateFileName[_MAX_PATH];
	MultiByteToWideChar( CP_ACP, 0, szString, -1, szCreateFileName, _MAX_PATH );

	HANDLE	hFile = INVALID_HANDLE_VALUE;
	hFile = CreateFile( szCreateFileName, GENERIC_READ|GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
	if( hFile == INVALID_HANDLE_VALUE )
		return FALSE;

	DWORD nWriteSize = 0;
	if( WriteFile( hFile, pBuff, nFileSize, &nWriteSize, NULL ) == false )
	{
		CloseHandle( hFile );
		return FALSE;
	}

	CloseHandle( hFile );
	return TRUE;
}

PatchReturn CDnPatchDownloadThread::ApplyRTPatch( CEtPackingFile& PatchSource, CEtPackingFile& RTPatchSource )
{
	int nPatchFileCount = PatchSource.GetFileCount();
	PatchReturn RetValue = PR_OK;

	for( int i=0; i<nPatchFileCount; i++ )
	{
		SPackingFileInfo* pFileInfo = PatchSource.GetFileInfo( i );

		if( pFileInfo == NULL )
			continue;

		// rtp파일 export
		if( !ExportFile( PatchSource, pFileInfo->szFileName, m_strRTPatchTempFolder.c_str() ) )
		{
			CString strFileName( pFileInfo->szFileName );
			LogWnd::Log( LogLevel::Error, L"Error ExportFile %s", strFileName );
			return PR_FAIL;
		}

		std::string strFullPathRTPFileName( pFileInfo->szFileName );					// /path/xxx.exe.rtp
		
		char strRTPFileName[_MAX_PATH];
		char strPath[_MAX_PATH];
		_GetPath( strPath, strFullPathRTPFileName.c_str() );							// /path
		_GetFullFileName( strRTPFileName, strFullPathRTPFileName.c_str() );				// xxx.exe.rtp

		std::string strFullPathOriginFileName( strFullPathRTPFileName.c_str() );
		strFullPathOriginFileName.erase( strFullPathOriginFileName.length() - 4, 4 );	// /path/xxx.exe
		char strOriginFileName[_MAX_PATH];
		_GetFullFileName( strOriginFileName, strFullPathOriginFileName.c_str() );		// xxx.exe

		BOOL bFindFile = FALSE;
		for( int i=0; i<(int)m_vecPackingFile.size(); i++ )
		{
			int nFindIndex = m_vecPackingFile[i]->FindFile( strFullPathOriginFileName.c_str() );
			
			if( nFindIndex != -1 )	// rtp파일에 해당하는 파일이 있다면 추출
			{
				if( !ExportFile( *m_vecPackingFile[i], strFullPathOriginFileName.c_str(), m_strRTPatchTempFolder.c_str() ) )
				{
					CString strFileName( pFileInfo->szFileName );
					LogWnd::Log( LogLevel::Error, L"Error ExportFile %s", strFileName );
					return PR_FAIL;
				}

				bFindFile = TRUE;
				break;
			}
		}

		strFullPathOriginFileName.erase( 0, 1 );	// path/xxx.exe

		if( !bFindFile )	// pak파일에서 못찾았다면 클라이언트 폴더에서 찾아본다.
		{
			char strClientPath[_MAX_PATH];
			WideCharToMultiByte( CP_ACP, 0, DNPATCHINFO.GetClientPath().GetBuffer(), -1, strClientPath, __MAX_PATH, NULL, NULL );

			std::vector<std::string> vecResult;
			FindFileListInDirectory( strClientPath, "*.*", vecResult, false, true, true );
			std::vector<std::string>::iterator iter = vecResult.begin();
			for( ; iter!=vecResult.end(); iter++ )
			{
				char strFullFileName[_MAX_PATH];
				_GetFullFileName( strFullFileName, (*iter).c_str() );
				std::string strLowerFileName = strFullFileName;
				std::transform( strLowerFileName.begin(), strLowerFileName.end(), strLowerFileName.begin(), tolower);

				if( strcmp( strOriginFileName, strLowerFileName.c_str() ) == 0 )
				{
					CString strSourceFile( (*iter).c_str() );

					char strString[_MAX_PATH];
					sprintf_s( strString, _MAX_PATH, "%s\\%s", m_strRTPatchTempFolder.c_str(), strFullFileName );
					WCHAR strDestFile[_MAX_PATH];
					MultiByteToWideChar( CP_ACP, 0, strString, -1, strDestFile, _MAX_PATH );

					if( !CopyFile( strSourceFile, strDestFile, FALSE ) )
					{
						CString strFileName( pFileInfo->szFileName );
						LogWnd::Log( LogLevel::Error, L"Error CopyFile %s", strFileName );
						return PR_FAIL;
					}

					bFindFile = TRUE;
					break;
				}
			}
		}

		char szBuildCommand[_MAX_PATH];
		sprintf( szBuildCommand, "%s \"%s\\%s\" /u", m_strRTPatchTempFolder.c_str(), m_strRTPatchTempFolder.c_str(), strRTPFileName );

		int ret = RTPatchApply32( szBuildCommand, CDnPatchDownloadThread::RTPatchCALLBACK, TRUE );
		if( ret != 0 )
			return PR_FAIL;

		RTPatchSource.ChangeDir( "\\" );
		if( strPath[0] )
		{
			RTPatchSource.ChangeDir( strPath );
		}

		// RTPatch 적용된 파일을 새 파일시스템에 넣는다.
		char szApplyPatchFileName[_MAX_PATH];
		sprintf_s( szApplyPatchFileName, _MAX_PATH, "%s\\%s", m_strRTPatchTempFolder.c_str(), g_strOriginFileName );
		if( !RTPatchSource.AddFile( szApplyPatchFileName ) )
			return PR_FAIL;

		WCHAR strDeleteFileName[_MAX_PATH];
		char strString[_MAX_PATH];
		sprintf_s( strString, _MAX_PATH, "%s\\%s", m_strRTPatchTempFolder.c_str(), strRTPFileName );
		MultiByteToWideChar( CP_ACP, 0, strString, -1, strDeleteFileName, _MAX_PATH );
		ClientDeleteFile( strDeleteFileName );
		sprintf_s( strString, _MAX_PATH, "%s\\%s", m_strRTPatchTempFolder.c_str(), g_strOriginFileName );
		MultiByteToWideChar( CP_ACP, 0, strString, -1, strDeleteFileName, _MAX_PATH );
		ClientDeleteFile( strDeleteFileName );

		if( m_bTerminateThread ) 
		{
			for( int m=0; m<(int)m_vecPackingFile.size(); m++ )
			{
				m_vecPackingFile[m]->CloseFileSystem();
				SAFE_DELETE(m_vecPackingFile[m]);
			}
			PatchSource.CloseFileSystem();
			RTPatchSource.CloseFileSystem();

			DeleteFolder( m_strRTPatchTempFolder.c_str() );
			return PR_TERMINATE;
		}
	}

	return RetValue;
}

#endif // _USE_RTPATCH

#ifdef _USE_SPLIT_COMPRESS_FILE

HRESULT CDnPatchDownloadThread::DownLoadSplitFile( CString strUrl, int nVersion )
{
	LogWnd::TraceLog( L"DownLoadSplitFile Start!" );

	CString strSplitTxtUrl, strSplitFile;
	strSplitTxtUrl.Format( _T( "%sSplit%08d.txt" ), strUrl.GetBuffer(), nVersion );
	strSplitFile.Format( _T( "%sSplit%08d.txt" ), DNPATCHINFO.GetClientPath().GetBuffer(), nVersion );

	HRESULT hr = S_FALSE;
	BOOL bResult = FALSE;
	for( int i=0; i<RETRY_MAX_COUNT; i++ )
	{
		bResult = DeleteUrlCacheEntry( strSplitTxtUrl.GetBuffer() );
		hr = DownloadToFile( strSplitTxtUrl.GetBuffer(), strSplitFile.GetBuffer() );

		if( hr == S_OK )
			break;
	}

	if( hr != S_OK )
		return S_FALSE;

	int nSplitCount = LoadSplitCount( strSplitFile );
	ClientDeleteFile( strSplitFile );

	if( nSplitCount == 0 )
		return S_FALSE;

	if( hr == S_OK )
	{
		LogWnd::Log( LogLevel::Info, L"DownLoadSplitFile Count(%d)", nSplitCount );

		int nSplitCountCipher = 1;
		if( nSplitCount >= 10 && nSplitCount < 100 )
			nSplitCountCipher = 2;
		else if( nSplitCount >= 100 )	// 분할파일 100개 넘어갈 일은 없을 것
			nSplitCountCipher = 3;

		std::vector<DWORD> vecDwSplitFileSizeList;
		DWORD dwTotalSplitFileSize = GetSplitFileSizeList( strUrl, nVersion, nSplitCount, nSplitCountCipher, vecDwSplitFileSizeList );

		if( nSplitCount != static_cast<int>( vecDwSplitFileSizeList.size() ) )
		{
			vecDwSplitFileSizeList.clear();
			return S_FALSE;
		}

		g_DownloadInfoData.m_strFileName.Format( _T( "Patch%08d.pak" ), nVersion );
		g_DownloadInfoData.m_nFileSize = dwTotalSplitFileSize;

		CString strRARFirstFileName;

		for( int i=0; i<nSplitCount; i++ )
		{
			CString strPatchFileUrl, strPatchFileName;

			if( nSplitCountCipher == 1 )
			{
				strPatchFileUrl.Format( _T( "%sPatch%08d.part%d.rar" ), strUrl.GetBuffer(), nVersion, i+1 );
				strPatchFileName.Format( _T( "Patch%08d.part%d.rar" ), nVersion, i+1 );
			}
			else if( nSplitCountCipher == 2 )
			{
				strPatchFileUrl.Format( _T( "%sPatch%08d.part%02d.rar" ), strUrl.GetBuffer(), nVersion, i+1 );
				strPatchFileName.Format( _T( "Patch%08d.part%02d.rar" ), nVersion, i+1 );
			}
			else if( nSplitCountCipher == 3 )
			{
				strPatchFileUrl.Format( _T( "%sPatch%08d.part%03d.rar" ), strUrl.GetBuffer(), nVersion, i+1 );
				strPatchFileName.Format( _T( "Patch%08d.part%03d.rar" ), nVersion, i+1 );
			}

			if( i == 0 )
				strRARFirstFileName = strPatchFileName;

			DWORD dwAlreadyDownloadFileSize = 0;
			DWORD dwRemainDownloadFileSize = 0;
			for( int j=0; j<i; j++ )
				dwAlreadyDownloadFileSize += vecDwSplitFileSizeList[j];

			dwRemainDownloadFileSize = dwAlreadyDownloadFileSize;
			for( int j=i+1; j<nSplitCount; j++ )
				dwRemainDownloadFileSize += vecDwSplitFileSizeList[j];

			bResult = DeleteUrlCacheEntry( strPatchFileUrl.GetBuffer() );
			hr = URLDownload( strPatchFileUrl.GetBuffer(), DNPATCHINFO.GetClientPath().GetBuffer(), strPatchFileName.GetBuffer(), 
							TRUE, dwAlreadyDownloadFileSize, dwRemainDownloadFileSize );
			if( hr != S_OK )
			{
				LogWnd::Log( LogLevel::Error, L"Download SplitFile Failed!");
				break;
			}

			if( ::GetFileAttributes( m_strPatchFile.GetBuffer() ) == -1 )
			{
				hr = S_FALSE;
				LogWnd::Log( LogLevel::Error, L"Failed! GetFileAttributes=[%s]", m_strPatchFile.GetBuffer() );
				break;
			}
		}

		vecDwSplitFileSizeList.clear();
		if( hr != S_OK )
			return hr;

		hUnrarDLL = LoadLibrary( _T( "unrar.dll" ) );
		hr = UnRARArchive( DNPATCHINFO.GetClientPath(), strRARFirstFileName );
		FreeLibrary( hUnrarDLL );
		if( hr == S_OK )
		{
			for( int i=0; i<nSplitCount; i++ )
			{
				CString strPatchFileName;
				if( nSplitCountCipher == 1 )
					strPatchFileName.Format( _T( "Patch%08d.part%d.rar" ), nVersion, i+1 );
				else if( nSplitCountCipher == 2 )
					strPatchFileName.Format( _T( "Patch%08d.part%02d.rar" ), nVersion, i+1 );
				else if( nSplitCountCipher == 3 )
					strPatchFileName.Format( _T( "Patch%08d.part%03d.rar" ), nVersion, i+1 );

				ClientDeleteFile( strPatchFileName );
			}

			m_strPatchFileUrl.Format( _T( "%sPatch%08d.pak" ), strUrl.GetBuffer(), nVersion );
			m_strPatchFileLocal.Format( _T( "Patch%08d.pak" ), nVersion );
			m_strPatchFile.Format( L"%s%s", DNPATCHINFO.GetClientPath(), m_strPatchFileLocal );

			LogWnd::TraceLog( L"MD5 Check Start" );
			// 올바르게 다운로드 되었는지 MD5 Check
			if( !GetMD5Checksum() )
			{
				LogWnd::Log( LogLevel::Error, L"MD5 Check Failed!" );
				SendStatusMsg( PATCH_MD5_CHECK_FAILED );
				ClientDeleteFile( m_strPatchFile );
				return S_FALSE;	// md5 check error
			}
			LogWnd::TraceLog( L"MD5 Check Success" );
		}
	}

	return hr;
}

DWORD CDnPatchDownloadThread::GetSplitFileSizeList( CString strUrl, int nVersion, int nSplitCount, int nSplitCountCipher, std::vector<DWORD>& vecSplitFileSizeList )
{
	DWORD dwTotalDownloadFileSize = 0;
	HINTERNET hInternetSession;
	hInternetSession = InternetOpen( NULL, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0 );

	if( hInternetSession == NULL )
	{
		LogWnd::Log( LogLevel::Error, L"Internet Session Open Failed!" );
		return 0;
	}

	for( int i=0; i<nSplitCount; i++ )
	{
		CString strPatchFileUrl;
		if( nSplitCountCipher == 1 )
			strPatchFileUrl.Format( _T( "%sPatch%08d.part%d.rar" ), strUrl.GetBuffer(), nVersion, i+1 );
		else if( nSplitCountCipher == 2 )
			strPatchFileUrl.Format( _T( "%sPatch%08d.part%02d.rar" ), strUrl.GetBuffer(), nVersion, i+1 );
		else if( nSplitCountCipher == 3 )
			strPatchFileUrl.Format( _T( "%sPatch%08d.part%03d.rar" ), strUrl.GetBuffer(), nVersion, i+1 );

		DWORD dwDownloadFileSize = GetDownloadFileSize( hInternetSession, strPatchFileUrl );
		dwTotalDownloadFileSize += dwDownloadFileSize;
		vecSplitFileSizeList.push_back( dwDownloadFileSize );
	}

	InternetCloseHandle( hInternetSession );

	return dwTotalDownloadFileSize;
}

int CDnPatchDownloadThread::LoadSplitCount( CString& szFilePath )
{
	char* buffer = NULL;
	HANDLE	hFile = INVALID_HANDLE_VALUE;
	int nVersion = 0;

	hFile = CreateFile( szFilePath.GetBuffer(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );

	if( hFile == INVALID_HANDLE_VALUE )
		return -1;

	DWORD tmp = 0;
	DWORD nFileSize = 0;
	nFileSize = GetFileSize( hFile, &tmp );
	nFileSize++;

	buffer = new char[nFileSize+1];
	if( buffer == NULL )
		return -1;

	ZeroMemory( buffer, nFileSize+1 );

	DWORD nReadSize = 0;
	ReadFile( hFile, buffer, nFileSize, &nReadSize, NULL );
	CloseHandle( hFile );	

	char arg1[256]={0,}, arg2[256]={0,};
	char *szToken = NULL, *nextToken = NULL;
	char delimiters[] = "\r\n";

	szToken = strtok_s( buffer, delimiters, &nextToken );

	while( szToken != NULL )
	{
		if( sscanf_s( szToken, "%s %s", &arg1, sizeof(arg1), &arg2, sizeof(arg2) ) )
		{
			_strlwr_s( arg1 );
			if( _stricmp( arg1, "split" ) == 0 )
				nVersion = atol( arg2 );
		}
		szToken = strtok_s( NULL, delimiters, &nextToken );
	}
	SAFE_DELETE_ARRAY( buffer );

	return nVersion;
}

HRESULT CDnPatchDownloadThread::UnRARArchive( CString strPath, CString strRARFile )
{
	// Is the DLL loaded?
	if( hUnrarDLL == NULL )
		return S_FALSE;

	// Get the function pointers we need
	(FARPROC&)OpenArchiveEx = GetProcAddress( hUnrarDLL, "RAROpenArchiveEx" );
	(FARPROC&)CloseArchive = GetProcAddress( hUnrarDLL, "RARCloseArchive" );
	(FARPROC&)ProcessRARFile = GetProcAddress( hUnrarDLL, "RARProcessFile" );
	(FARPROC&)ReadRARHeader = GetProcAddress( hUnrarDLL, "RARReadHeader" );

	// Set up our RAR archive data
	RAROpenArchiveDataEx archiveData;

	ZeroMemory( &archiveData, sizeof(archiveData) );

	char szTemp[1024] = {0,};
	WideCharToMultiByte( CP_ACP, 0, strRARFile.GetBuffer(), -1, szTemp, 1024, NULL, NULL );
	archiveData.ArcName	= szTemp;
	archiveData.CmtBuf = NULL;
	archiveData.OpenMode = 1;

	// Open the archive into a handle
	HANDLE archiveHandle = OpenArchiveEx( &archiveData );

	// Did it fail? Update the error and return false
	if( archiveData.OpenResult != 0 )
		return S_FALSE;

	// Simple header data struct
	RARHeaderData headerData;
	headerData.CmtBuf = NULL;

	// Set variables
	int readHeaderCode, processFileCode;

	HRESULT hr = S_OK;
	WideCharToMultiByte( CP_ACP, 0, strPath.GetBuffer(), -1, szTemp, 1024, NULL, NULL );
	// While the header data is valid..
	while( ( readHeaderCode = ReadRARHeader( archiveHandle, &headerData ) ) == 0 )
	{
		// UnRAR this file
		processFileCode = ProcessRARFile( archiveHandle, 2, szTemp, NULL );

		// Did it go okay? If not, we're done
		if( processFileCode != 0 )
		{
			hr = S_FALSE;
			break;
		}
	}

	// Close the archive
	CloseArchive( archiveHandle );

	return hr;
}

#endif // _USE_SPLIT_COMPRESS_FILE

#ifdef _FIRST_PATCH

BOOL CDnPatchDownloadThread::CheckExistFirstPatch( int nVersion )
{
	CString strFirtPatchURLPath, strPatchListFile;
	strFirtPatchURLPath.Format( _T( "%s%08d%s%s" ), DNPATCHINFO.GetPatchUrl(), nVersion, _T("/FirstPatch/"), FIRSTPATCHLIST_NAME );
	strPatchListFile.Format( _T( "%s%s" ), DNPATCHINFO.GetClientPath().GetBuffer() , FIRSTPATCHLIST_NAME );

	HRESULT hr;
	BOOL bExistFirstPatch = FALSE;

	for( int i = 0 ; i < RETRY_MAX_COUNT ; ++i )
	{
		DeleteUrlCacheEntry( strFirtPatchURLPath.GetBuffer() );
		hr = DownloadToFile( strFirtPatchURLPath.GetBuffer(), strPatchListFile.GetBuffer() );

		if( hr == S_OK )
		{
			ClientDeleteFile( strPatchListFile );
			bExistFirstPatch = TRUE;
			break;
		}
	}

	return bExistFirstPatch;
}

#endif // _FIRST_PATCH