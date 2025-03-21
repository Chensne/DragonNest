#include "stdafx.h"

#ifdef _HSHIELD
#include "HShieldSetup.h"
#include "../HShield/SDKs/Include/HShield.h"
#include "../HShield/SDKs/Include/HSUpChk.h"
#include "DNProtocol.h"
#include "LoginSendPacket.h"
#include "DnServiceSetup.h"
#include "DnString.h"
#include "GlobalValue.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


const WCHAR __szMonitorIPDummy1[16] = { 112, 52, 52,59,99,67,32,78,63,33,54,66,70, 0,45,22};
const WCHAR __szMonitorIPDummy2[16] = { 6, 21, 45, 33, 22, 0, 140, 57, 254, 2, 3, 44, 22, 16, 12, 0};
#if defined(_RU)
// 아래값은 195.211.128.163 ip 를 180 이란 키로 ^= 한 결과 이다 .
const WCHAR __szMonitorIP[16]		= { 133, 141, 129, 154, 134, 133, 133, 154, 133, 134, 140, 154, 133, 130, 135, 0};
#else
// 아래값은 211.218.230.106 ip 를 180 이란 키로 ^= 한 결과 이다 .
const WCHAR __szMonitorIP[16]		= { 134, 133, 133, 154, 134, 133, 140, 154, 134, 135, 132, 154, 133, 132, 130, 0};
#endif
const WCHAR __szMonitorIPDummy3[16] = { 136, 232, 53, 134, 133, 173, 180, 164, 204, 134, 142, 15, 73, 82, 99, 0};
const WCHAR __szMonitorIPDummy4[16] = { 334, 139, 130, 154, 124, 163, 190, 254, 194, 195, 143, 43, 38, 62, 0, 0};
const WCHAR __szMonitorIPDummy5[16] = { 14, 13, 113, 11, 134, 154, 190, 194, 184, 165, 132, 134, 132, 112, 110, 0};


std::wstring ___GetMonitorIP()
{
	WCHAR buff[1024] = {0,};
	wcscpy(buff, __szMonitorIP);

	for ( size_t i = 0 ; i < wcslen(__szMonitorIP) ; i++ )
	{
		buff[i] ^= 180;
	}

	return std::wstring(buff);
}

std::string __szLogDirctory( "HShieldLog" );
void HShieldLog_Init()
{
	CreateDirectoryA( __szLogDirctory.c_str(), NULL );
}

void HShieldLog_GetFilePath( std::string & wszPath )
{
	char temp_file_name[256] = {0,};
	char temp_date_buf[256] = {0,};
	struct	tm *tm_ptr;
	time_t	raw;

	time(&raw);
	tm_ptr = localtime(&raw);

	strftime(temp_date_buf, _countof(temp_date_buf), "%Y-%m-%d", tm_ptr);
	sprintf_s( temp_file_name, _countof(temp_file_name), "%s\\HLog%s.txt", __szLogDirctory.c_str(), temp_date_buf );

	wszPath = std::string( temp_file_name );
}

void HShieldLog_FileLog( const unsigned char * byBuffer, const unsigned short nLength, const DWORD dwRet, bool bBefore )
{
	std::string szPath;
	HShieldLog_GetFilePath( szPath );

	FILE * pFile = NULL;
	fopen_s( &pFile, szPath.c_str(), "a" );
	if( NULL == pFile )
		return;

	WCHAR wszOutputString[1024] = {0,};
	if( bBefore )
		swprintf_s( wszOutputString, _countof(wszOutputString), L"[_AhnHS_ MakeResponse before] m_GuidMsg.byBuffer.byBuffer[%x] m_GuidMsg.byBuffer.nLength[%x] : (0x%x)\n", byBuffer, nLength, dwRet );
	else
		swprintf_s( wszOutputString, _countof(wszOutputString), L"[_AhnHS_ MakeResponse after] m_GuidMsg.byBuffer.byBuffer[%x] m_GuidMsg.byBuffer.nLength[%x] : (0x%x)\n", byBuffer, nLength, dwRet );

	fputws( wszOutputString , pFile );

	fflush( pFile );
	fclose( pFile );
}

#pragma warning ( disable : 4996 )

// 디버깅 되는 개발 버전.
#if defined(_DEBUG_HSHIELD)
#pragma comment (lib, "../../Extern/HShield/SDKs_Developer/Lib/Client/x86/Win/MultiThreaded/HShield/HShield.lib" )
#else															
#pragma comment (lib, "../../Extern/HShield/SDKs/Lib/Client/x86/Win/Multithreaded/HShield/HShield.lib" )// 디버깅 안되는 버전.
#endif

// 필수
#pragma comment (lib, "version.lib" )
#pragma comment (lib, "../../Extern/HShield/SDKs/Lib/Client/x86/Win/Multithreaded/Update/HsUpChk.lib" )


HShieldSetup::Info HShieldSetup::m_Info;
std::wstring HShieldSetup::m_szLastErrorMsg;
HANDLE HShieldSetup::m_hForceQuitThread = NULL;
unsigned HShieldSetup::m_dwForceQuitThreadID = NULL;
bool HShieldSetup::m_bStartMonitor = false;

int HShieldSetup::Init(HWND hWnd)
{
	m_Info.m_dwMainThreadID = GetCurrentThreadId();
	m_Info.m_hWnd = hWnd;
	m_szLastErrorMsg = L"";
	TCHAR	szMsg[1024]={0,};
	TCHAR	*pEnd = NULL;

	// 현재 프로세스 실행 기준으로 FULL-PATH를 얻음
	GetModuleFileName(NULL, m_Info.m_szFullFileName, MAX_PATH);
	pEnd = _tcsrchr( m_Info.m_szFullFileName, _T('\\'));
	if (!pEnd)
	{
		return FALSE;	
	}
	*pEnd = _T('\0');


	_stprintf(m_Info.m_szHShieldPath, _T("%s\\HShield"), m_Info.m_szFullFileName);	// HShield-Path 세팅
	_tcscat(m_Info.m_szFullFileName, _T("\\HShield\\EhSvc.dll"));			// EHSVC.DLL =Path 세팅.
	//_stprintf(g_szIniPath, _T("%s"), m_Info.m_szFullFileName);				
	//_tcscat(g_szIniPath, _T("MiniAEnv.INI"));						// MiniAEnv.ini Path 세팅

	return 0;
}

int __stdcall HShieldSetup::AhnHS_Callback(long lCode, long lParamSize, void* pParam)
{
	TCHAR* pTemp = (TCHAR*)(pParam);
	if ( lCode != AHNHS_ENGINE_DETECT_WINDOWED_HACK )
		WriteLog( 0, ", Info, AS__Callback : lCode(%d), lParamSize(%d), pParam(%s)\n", lCode, lParamSize, pTemp ? pTemp : _T("NoneParam") );

	switch(lCode)
	{
		//Engine Callback
	case AHNHS_ENGINE_DETECT_GAME_HACK:
		{
			TCHAR szMsg[255] = {0,};
			_stprintf(szMsg, STR_AHNHS_ENGINE_DETECT_GAME_HACK);
			OutputDebugString(szMsg);
			m_szLastErrorMsg = szMsg;
			m_szLastErrorMsg += L"\n";			
//			PostMessage( m_Info.m_hWnd, WM_ALERT_HSHIELD, 0, 0);
			HSErrorMessageBox();			
			break;
		}
		//창모드 해킹툴 감지
	case AHNHS_ENGINE_DETECT_WINDOWED_HACK:
		{
			// 창모드 이건 무시
			break;
		}
		//Speed 관련
	case AHNHS_ACTAPC_DETECT_SPEEDHACK:
		{
			TCHAR szMsg[255] = {0,};
			_stprintf(szMsg, STR_AHNHS_ACTAPC_DETECT_SPEEDHACK );
			OutputDebugString(szMsg);
			m_szLastErrorMsg = szMsg;
			m_szLastErrorMsg += L"\n";
			HSErrorMessageBox();			
			break;
		}

		//디버깅 방지 
	case AHNHS_ACTAPC_DETECT_KDTRACE:	
		{
			TCHAR szMsg[255];
			_stprintf(szMsg, STR_AHNHS_ACTAPC_DETECT_KDTRACE_CHANGED , lCode);
			OutputDebugString(szMsg);
			m_szLastErrorMsg = szMsg;
			m_szLastErrorMsg += L"\n";
			//break;			
			HSErrorMessageBox();			
			break;
		}

	case AHNHS_ACTAPC_DETECT_AUTOMACRO:
		{
			TCHAR szMsg[255];
			_stprintf(szMsg, STR_AHNHS_ACTAPC_DETECT_AUTOMACRO , lCode);
			OutputDebugString(szMsg);
			m_szLastErrorMsg = szMsg;
			m_szLastErrorMsg += L"\n";			
			HSErrorMessageBox();			
			break;
		}

		// 코드 패치 감지
	case AHNHS_ACTAPC_DETECT_ABNORMAL_MEMORY_ACCESS:
		{
			TCHAR szMsg[255];
			_stprintf(szMsg,STR_AHNHS_ACTAPC_DETECT_ABNORMAL_MEMORY_ACCESS , (TCHAR*)pParam);
			OutputDebugString(szMsg);
			m_szLastErrorMsg = szMsg;
			m_szLastErrorMsg += L"\n";
			//break;			
			HSErrorMessageBox();			
			break;
		}

	case AHNHS_ACTAPC_DETECT_MEM_MODIFY_FROM_LMP:
		{
			TCHAR szMsg[255];
			_stprintf(szMsg, STR_AHNHS_ACTAPC_DETECT_MEM_MODIFY_FROM_LMP );
			OutputDebugString(szMsg);
			m_szLastErrorMsg = szMsg;
			m_szLastErrorMsg += L"\n";						
			HSErrorMessageBox();			
			break;
		}

		//그외 해킹 방지 기능 이상 
	case AHNHS_ACTAPC_DETECT_DRIVERFAILED:
	case AHNHS_ACTAPC_DETECT_HOOKFUNCTION:
	case AHNHS_ACTAPC_DETECT_ENGINEFAILED:
	case AHNHS_ACTAPC_DETECT_CODEMISMATCH:
	case AHNHS_ACTAPC_DETECT_ABNORMAL_HACKSHIELD_STATUS:
		{
			TCHAR szMsg[1024] = {0,};
			TCHAR szString[1024] = {0,};
			_stprintf(szString, STR_HS_ERR_DEFAULT , lCode);
			_stprintf(szMsg, L"AhnHS_Callback : %s", szString);

			OutputDebugString(szMsg);
			m_szLastErrorMsg += szMsg;
			m_szLastErrorMsg += L"\n";
			//break;			
			HSErrorMessageBox();			
			break;
		}
	}
	return 1;
}

int HShieldSetup::Update()
{
	// -------------------------------------------------------------------------
	// [_AhnHS_HSUpdate]
	// -------------------------------------------------------------------------
	// - DESCRIPTION: 
	//	 HackShield 엔진 파일들을 업데이트합니다.
	//
	// - SYNTAX:
	//	 DWORD __stdcall _AhnHS_HSUpdate( LPCTSTR szUpdateDir, DWORD dwTimeOut );
	//	
	// - PARAMETERS:
	//	 .szUpdateDir				업데이트 파일이 설치되어 있는 폴더
	//	 .dwTimeOut	milliseconds	업데이트 시 타임 아웃 시간. 0으로 설정 시 INFINITE으로 설정

	// - RETURN VALUE:
	//			. HACKSHIELD_ERROR_SUCESS	: 업데이트 성공 (Value = 0x00000000)
	//          . 기타						: 기타 리턴값에 대해서는 핵쉴드 프로그래밍 가이드 참고바람.

	AHNHS_EXT_ERRORINFO HsExtError;

	swprintf( HsExtError.szServer, L"%s", ___GetMonitorIP().c_str() );		// 모니터링 서버주소
	swprintf( HsExtError.szUserId, L"%s", L"Test" );				// 유저 아이디
	// _AhnHS_Initialize시점에 아이디를 얻을 수 없는 경우에는 널 스트링을 입력한다.
	// 아이디를 얻는 시점에 _AhnHS_SetUserId를 호출하여 아이디를 전송한다.
	// sprintf( HsExtError.szUserId, "%s", "" ); 
	swprintf( HsExtError.szGameVersion, L"%s", CGlobalInfo::GetInstance().m_wszBuildVersion.c_str() );			// 게임 버전

	int GameCode = 9921 ; 
#ifdef _KR 
	GameCode = 9921; 
#elif _JP 
	GameCode = 5860;
#elif _TW
	GameCode = 5931;
#elif _US
	GameCode = 9970;
#elif _TH
	GameCode = 5862;
#elif _ID
	GameCode = 5861;
#elif _RU
	GameCode = 5863;
#elif _EU
	GameCode = 5864;
#elif _KRAZ
	GameCode = 5869;
#endif
	//DWORD dwResult = 0; 
	DWORD dwResult = _AhnHS_HSUpdateEx(m_Info.m_szHShieldPath , 1000*600  , (INT64)GameCode 
		, AHNHSUPDATE_CHKOPT_GAMECODE 
#if !defined(_TW)
		| AHNHSUPDATE_CHKOPT_HOSTFILE 
#endif
		,  HsExtError, 1000*20 );
	//dwResult = _AhnHS_HSUpdate( m_Info.m_szHShieldPath , 1000*600  );

	LPTSTR szErrorStr = _T("");
	switch(dwResult)
	{
	case 0x00000000:
		szErrorStr = _T("HS_ERR_OK");
		return 1;
		break;
	case 0x30000010:
		szErrorStr = _T("HS_ERR_ENVFILE_NOTREAD");
		break;
	case 0x30000020:
		szErrorStr = _T("HS_ERR_ENVFILE_NOTWRITE");
		break;
	case 0x30000030:
		szErrorStr = _T("HS_ERR_NETWORK_CONNECT_FAIL");
		break;
	case 0x30000050:
		szErrorStr = _T("HS_ERR_LIB_NOTEDIT_REG");
		break;
	case 0x30000060:
		szErrorStr = _T("HS_ERR_NOTFINDFILE");
		break;
	case 0x30000070:
		szErrorStr = _T("HS_ERR_PROTECT_LISTLOAD_FAIL");
		break;
	case 0x30000080:
		szErrorStr = _T("HS_ERR_PROTECT_VERITY_FAIL");
		break;
	case 0x30000090:
		szErrorStr = _T("HS_ERR_HSUPDATE_TIMEOUT");
		break;
	default :
		return 1;
		break;
	}

	TCHAR szMsg[2048] ={0,};
	_stprintf(szMsg, _T("%s[%x] = HSUpdate(%s, %d)"), szErrorStr, dwResult, m_Info.m_szHShieldPath, 0 );

	DebugLog("%s[%x] = HSUpdate(%d)",dwResult, m_Info.m_szHShieldPath, 0 );

	OutputDebugString(szMsg);
	m_szLastErrorMsg = szMsg;
	m_szLastErrorMsg += L"\n";

	HSErrorMessageBox();

	//MessageBox(m_Info.m_hWnd, szMsg, m_Info.m_szWinTitle, MB_OK);
	//PostThreadMessage(m_Info.m_dwMainThreadID, WM_QUIT, 0, 0);

	//	TRACE("%s[%x] = AhnHS_HSUpdate(%s, %d)", szErrorStr, dwResult, m_szHShieldPath, 0);
	//TRACE( _T("%s[%x] = AhnHS_HSUpdate(%s, %d)"), szErrorStr, dwResult, m_Info.m_szHShieldPath, 0 );
	return -1;
}

void HShieldSetup::SetMyID(const WCHAR* szUserID)
{
	_tcscpy_s( m_Info.m_ID ,szUserID );
	if( true == m_bStartMonitor )
		_AhnHS_SetUserId( m_Info.m_ID );
}

int HShieldSetup::Start()
{
	TCHAR	szMsg[1024]={0,};
	int		nRet;

	// HS-Monitoring 서비스 사용을 위한 구조체 선언
	AHNHS_EXT_ERRORINFO HsExtError;
	// 아이디나 모니터링 주소, 게임버전등은 암호화나 인코딩 하여 보관하였다가 실제 아래 함수에 입력시
	// 복호화나 디코딩 하여 입력하는 것이 좋다.

	swprintf( HsExtError.szServer, L"%s", ___GetMonitorIP().c_str() );		// 모니터링 서버주소
	swprintf( HsExtError.szUserId, L"%s", m_Info.m_ID );				// 유저 아이디
	// _AhnHS_Initialize시점에 아이디를 얻을 수 없는 경우에는 널 스트링을 입력한다.
	// 아이디를 얻는 시점에 _AhnHS_SetUserId를 호출하여 아이디를 전송한다.
	// sprintf( HsExtError.szUserId, "%s", "" ); 
	swprintf( HsExtError.szGameVersion, L"%s", CGlobalInfo::GetInstance().m_wszBuildVersion.c_str() );			// 게임 버전

	// -------------------------------------------------------------------------
	// [_AhnHS_StartMonitor]
	// -------------------------------------------------------------------------
	// - DESCRIPTION: 
	//	  핵쉴드 모니터링 서비스를 시작합니다.
	// - SYNTAX:
	//	   EHSVC_API int __stdcall _AhnHS_StartMonitor(AHNHS_EXT_ERRORINFO HsExtError, LPCSTR szFullFileName)
	//
	//	  .  AHNHS_EXT_ERRORINFO HsExtErrorInfo		: AHNHS_EXT_ERRORINFO HsExtErrorInfo :  모니터링 관련 입력 정보들
	//						                          - 서버 URL주소. ( "123.456.789.123" or "www.monitor.com" )
	//												  - User ID		  ( "User1" )
	//												  - Game Version  ( "5.0.2.1" )
	//	  .  szFullFileName							: EHSVC.DLL 전체 경로
	//
	// - RETURN VALUE:
	//     . HS_ERR_OK          : 성공
	//     . 기타				: 기타 리턴값에 대해서는 핵쉴드 프로그래밍 가이드 참고바람.
	//
	nRet = _AhnHS_StartMonitor( HsExtError, m_Info.m_szFullFileName );

	if (nRet != HS_ERR_OK)
	{
		_stprintf(szMsg, STR_HS_SHIELD_MONITER_ERROR , nRet);
		m_szLastErrorMsg = szMsg;
		m_szLastErrorMsg += L"\n";
		HSErrorMessageBox();		
		return -1;
	}

	m_bStartMonitor = true;
	_AhnHS_SetUserId( m_Info.m_ID );

	// -------------------------------------------------------------------------
	// [_AhnHS_Initialize]
	// -------------------------------------------------------------------------
	// - DESCRIPTION: 
	//	  초기화 함수 호출 
	//    HackShield를 초기화하고, 옵션을 설정합니다. 
	//	  프로그램이 초기화될 때 한 번만 호출할 수 있습니다. 
	//    다른 게임 프로그램에서 HackShield를 사용하고 있거나 서비스가 비정상적으로 
	//    종료되었을 경우 오류가 발생할 수 있습니다. 
	//
	// - SYNTAX:
	//	   EHSVC_API int __stdcall _AhnHS_Initialize(const char* szFileName,	PFN_AhnHS_Callback pfn_Callback, 
	//												int nGameCode, const char* szLicenseKey, DWORD dwOption,
	//												UINT unSHackSensingRatio)
	//	  . szFileName			: EHSvc.dll의 전체 경로
	//	  . Pfn_Callback		: Callback 함수의 포인터
	//    . nGameCode			: 4자리 숫자	각 게임에 해당하는 고유 ID 코드
	//    . szLicenseKe			: 24자리 문자열	각 게임에 해당하는 라이선스 키
	//    . dwOption			: 초기화 옵션 설정
	//    . unSHackSensingRatio : 스피드 핵 감지율 레벨
	//
	// - OPTIONS:
	//   핵쉴드 프로그래밍 가이드 참고바람.
	//
	// - RETURN VALUE:
	//     . HS_ERR_OK          : 성공
	//     . 기타				: 기타 리턴값에 대해서는 핵쉴드 프로그래밍 가이드 참고바람.

	nRet = _AhnHS_Initialize(m_Info.m_szFullFileName, AhnHS_Callback,
#ifdef _KR 
		9921, L"EFFCCB3C8ECE5E6DD1FEB66C", 
#elif _JP 
		5860 , L"11467A7CC3E2E21D9D6F35D1",
#elif _TW
		5931 , L"BE16FA9F95AAB794786F2AE1",
#elif _US
		9970, L"5818273E8AC27265D033B0CD",
#elif _TH
		5862, L"0B540D60271E11BDECEF4651",
#elif _ID
		5861, L"33EAC9B4393428E7DC685162",
#elif _RU
		5863, L"51D1D6375BA6F296F8B62C84",
#elif _EU
		5864, L"4727214110BBA5B3AFA6368B",
#elif _KRAZ
		5869, L"5133E755F445F5C820179142",
#else 
		5869, L"5133E755F445F5C820179142",
#endif 
		AHNHS_CHKOPT_ALL
		| AHNHS_DONOT_TERMINATE_PROCESS
		| AHNHS_CHKOPT_LOCAL_MEMORY_PROTECTION
		| AHNHS_CHKOPT_SEND_MONITOR_DELAY
#if !defined(_EU)
		| AHNHS_CHKOPT_DETECT_VIRTUAL_MACHINE
#endif
		| AHNHS_CHKOPT_STANDALONE
#if !defined(_RU)
		| AHNHS_CHKOPT_UPDATED_FILE_CHECK 
#endif
		, AHNHS_SPEEDHACK_SENSING_RATIO_HIGHEST);

	//아래 에러는 개발과정에서만 발생할 수 있으면 
	//이후 결코 발생해서는 안되는 에러이므로 assert처리를 했습니다.
	assert(nRet != HS_ERR_INVALID_PARAM);
	assert(nRet != HS_ERR_INVALID_LICENSE);
	assert(nRet != HS_ERR_ALREADY_INITIALIZED);

	if (nRet != HS_ERR_OK) 
	{		
		//Error 처리 
		switch(nRet)
		{
		case HS_ERR_ANOTHER_SERVICE_RUNNING:
			{
				MessageBox(m_Info.m_hWnd, STR_HS_ERR_ANOTHER_SERVICE_RUNNING , m_Info.m_szWinTitle, MB_OK);
				break;
			}
		case HS_ERR_INVALID_FILES:
			{
				MessageBox(m_Info.m_hWnd, STS_HS_ERR_INVALID_FILES , m_Info.m_szWinTitle, MB_OK);
				break;
			}
		case HS_ERR_DEBUGGER_DETECT:
			{
				MessageBox(m_Info.m_hWnd, STR_HS_ERR_DEBUGGER_DETECT , m_Info.m_szWinTitle, MB_OK);
				break;
			}
		case HS_ERR_NEED_ADMIN_RIGHTS:
			{
				MessageBox(m_Info.m_hWnd, STR_HS_ERR_NEED_ADMIN_RIGHTS , m_Info.m_szWinTitle, MB_OK);
				break;
			}
		case HS_ERR_COMPATIBILITY_MODE_RUNNING:
			{
				MessageBox(m_Info.m_hWnd, STR_HS_ERR_COMPATIBILITY_MODE_RUNNING , m_Info.m_szWinTitle, MB_OK);
				break;				
			}
		default:
			{
				TCHAR szString[1024]={0,};
				_stprintf(szString, STR_HS_ERR_DEFAULT , nRet);
				_stprintf(szMsg, L"_AhnHS_Initialize : %s", szString);

				MessageBox(m_Info.m_hWnd, szMsg, m_Info.m_szWinTitle, MB_OK);
				break;
			}
		}

		//OutputDebugString(szMsg);
		//m_szLastErrorMsg = szMsg;
		//m_szLastErrorMsg += L"\n";

		return -1;
	}

	// -------------------------------------------------------------------------
	// [_AhnHS_StartService]
	// -------------------------------------------------------------------------
	// - DESCRIPTION:
	//	   해킹 툴 탐지 기능과 해킹 차단 기능을 동작시킵니다. 
	//	   _AhnHS_Initialize 함수를 호출한 다음에 호출해야 하며, 중복 호출할 수 없습니다. 
	//	   _AhnHS_StopService 함수를 호출하여 서비스를 중지한 경우에는, 이 함수를 다시 
	//	   호출하여 서비스를 다시 시작시킬 수 있습니다.
	//
	// - SYNTAX:
	//	  EHSVC_API int __stdcall _AhnHS_StartService( );
	//
	// - RETURN VALUE:
	//    . HS_ERR_OK          : 성공
	//    . 기타				: 기타 리턴값에 대해서는 핵쉴드 프로그래밍 가이드 참고바람.

	nRet = _AhnHS_StartService();
	assert(nRet != HS_ERR_NOT_INITIALIZED);
	assert(nRet != HS_ERR_ALREADY_SERVICE_RUNNING);

	if (nRet != HS_ERR_OK)
	{
		TCHAR szString[1024]={0,};
		_stprintf(szString, STR_HS_ERR_DEFAULT , nRet);
		_stprintf(szMsg, L"_AhnHS_StartService : %s", szString);

		MessageBox(m_Info.m_hWnd, szMsg, m_Info.m_szWinTitle, MB_OK);
		return -1;
	}
	return 0;
}

int HShieldSetup::Stop()
{
	TCHAR	szMsg[1024]={0,};
	int		nRet;

	// -------------------------------------------------------------------------
	// [_AhnHS_StopService]
	// -------------------------------------------------------------------------
	// - DESCRIPTION:
	//	  해킹 차단 기능과 해킹 툴 탐지 기능을 정지시킵니다.
	//
	// - SYNTAX:
	// 	  EHSVC_API int __stdcall _AhnHS_StopService( );
	//
	// - RETURN VALUE:
	//     . HS_ERR_OK					 : 성공
	//	   . HS_ERR_NOT_INITIALIZED		 : _AhnHS_Initialize 함수를 호출하여 HackShield를 초기화하지 않은 상태에서 
	//										호출한 경우 발생하는 오류입니다. 
	//										이 오류는 개발 과정에서만 발생할 수 있는 오류로 별도의 처리는 하지 않아도 됩니다. (Value = 0x003)
	//     . HS_ERR_SERVICE_NOT_RUNNING  : _AhnHS_StartService 함수를 호출하여 HackShield를 시작하지 않은 상태에서 
	//                                     호출한 경우 발생한 오류입니다. 개발 과정에서만 발생할 수 있는 오류로 별도의
	//                                      처리는 하지 않아도 됩니다. (Value = 0x301)

	nRet = _AhnHS_StopService();

	if ( nRet != HS_ERR_SERVICE_NOT_RUNNING && nRet != HS_ERR_NOT_INITIALIZED && nRet != HS_ERR_OK)
	{
		_stprintf(szMsg, _T(" StopService Error(Error Code = %x)"), nRet);
		MessageBox(m_Info.m_hWnd, szMsg, m_Info.m_szWinTitle, MB_OK);
		return nRet;
	}


	// -------------------------------------------------------------------------
	// [_AhnHS_Uninitialize]
	// -------------------------------------------------------------------------
	// - DESCRIPTION:
	//	 시스템 내부적으로 사용되었던 메모리를 해제하고 변수를 초기화합니다.
	//
	// - SYNTAX:
	// 	 EHSVC_API int __stdcall _AhnHS_Uninitialize( );
	//
	// - RETURN VALUE:
	//   . HS_ERR_OK				    : 성공
	//	 . HS_ERR_SERVICE_STILL_RUNNING : _AhnHS_StopSerivice 함수를 호출하여 HackShield를 종료하지 않은 상태에서 
	//                                   호출하면 발생하는 오류입니다. 이 오류는 개발 과정에서만 발생할 수 있으므로 
	//                                   별도의 처리는 하지 않아도 됩니다. (Value = 0x302)
	//   . HS_ERR_NOT_INITIALIZED	     : _AhnHS_Initialize 함수 호출을 통하여 시스템을 초기화하지 않은 상태에서 
	//                                     호출하면 발생하는 오류입니다. 이 오류는 개발 과정에서만 발생할 수 있으므로 
	//                                     별도의 처리는 하지 않아도 됩니다. (Value = 0x003)


	nRet = _AhnHS_Uninitialize();

	if (nRet != HS_ERR_SERVICE_NOT_RUNNING && nRet != HS_ERR_NOT_INITIALIZED  && nRet != HS_ERR_OK)
	{
		_stprintf(szMsg, _T("_AhnHS_Uninitialize Error (Error Code = %x)"), nRet);
		MessageBox(NULL, szMsg, m_Info.m_szWinTitle, MB_OK);
		return nRet;
	}

	return nRet;
}

AHNHS_TRANS_BUFFER s_DebugRecvData;
void HShieldSetup::OnDispatchMessage(int iMainCmd, int iSubCmd, char * pData, int iLen)
{
	static bool bFirstRequest = false;

	switch( iMainCmd )
	{
	case SC_SYSTEM:
		{
			switch(iSubCmd)
			{
			case eSystem::SC_MAKEREQUEST:
				{
					// -------------------------------------------------------------------------
					// [_AhnHS_MakeResponse]
					// -------------------------------------------------------------------------
					// - DESCRIPTION: 
					//	  클라이언트에서 사용합니다. 서버에서 전달한 암호화된 버전 요구 메시지를 
					//	  복호화하고 현재 클라이언트 파일의 버전을 암호화하여 응답 메시지를 만듭니다.
					//
					// - SYNTAX:
					//		int __stdcall _AhnHS_MakeResponse (unsigned char *pbyRequest, unsigned long ulRequestLength, 
					//									PAHNHS_TRANS_BUFFER pResponseBuffer);
					// - PARAMETERS:
					//		.pbyRequest	unsigned char *				:[IN]	요청 메시지 버퍼
					//		.ulRequestLength	unsigned long		:[IN]	요청 메시지 길이
					//		.pResponseBuffer	PAHNHS_TRANS_BUFFER	:[OUT]  응답 메시지 버퍼
					//
					// - RETURN VALUE:
					//     . ERROR_SUCCESS      : 성공
					//     . 기타				: 기타 리턴값에 대해서는 핵쉴드 프로그래밍 가이드 참고바람.
					AHNHS_TRANS_BUFFER TransBuffer = {0,};
					ZeroMemory(&TransBuffer, sizeof(AHNHS_TRANS_BUFFER));

					AHNHS_TRANS_BUFFER* pRecvData =  (AHNHS_TRANS_BUFFER*)pData;
					// 이 코드는 디버깅을 위해서 박아놉니다.
					s_DebugRecvData = *pRecvData;
					// _AhnHS_MakeResponse 내부에서 죽는 경우가 자꾸 발생한다.
					// 일단 버퍼 사이즈 체크 등등으로 리턴시켜버리자. ( 근데 리턴이 해결책이려나.. )
					if( pRecvData->nLength > ANTICPX_TRANS_BUFFER_MAX || pRecvData->nLength > iLen - sizeof(DNTPacketHeader) ) {
						break;
					}

					if( false == bFirstRequest && true == CGlobalInfo::GetInstance().m_bUseHShieldLog )
					{
						HShieldLog_Init();
						bFirstRequest = true;
					}

					DWORD dwRet = 0;
#if defined( PRE_ADD_HSHIELD_LOG )
					if( true == bFirstRequest )
						HShieldLog_FileLog( pRecvData->byBuffer, pRecvData->nLength, dwRet, true );
#endif	// #if defined( PRE_ADD_HSHIELD_LOG )

#if defined( PRE_FIX_CLIENT_FREEZING )
					CommonUtil::PrintFreezingLog( "_AhnHS_MakeResponse - Before" );
#endif

					dwRet = _AhnHS_MakeResponse( (unsigned char*) pRecvData->byBuffer, pRecvData->nLength, &TransBuffer );

#if defined( PRE_FIX_CLIENT_FREEZING )
					CommonUtil::PrintFreezingLog( "_AhnHS_MakeResponse - After" );
#endif

#if defined( PRE_ADD_HSHIELD_LOG )
					if( true == bFirstRequest )
						HShieldLog_FileLog( pRecvData->byBuffer, pRecvData->nLength, dwRet, false );
#endif	// #if defined( PRE_ADD_HSHIELD_LOG )

					if ( dwRet == ERROR_SUCCESS )
					{
						SendAckCRC(TransBuffer);
					}
					else
					{
//#ifndef _FINAL_BUILD
						std::wstring szMsg = FormatW(STR_HS_DRAGONNEST_REINSTALL , dwRet );
						MessageBoxW(m_Info.m_hWnd, szMsg.c_str(), L"DragonNest", 0);
//#endif
						if( g_pServiceSetup )
							g_pServiceSetup->WriteStageLog_( IServiceSetup::SecurityModule_Failed, L"HackSheild_Failed" );
						ErrorLog("g_pServiceSetup->WriteStageLog_( IServiceSetup::SecurityModule_Failed, HackSheild_Failed)" );
						PostThreadMessage(m_Info.m_dwMainThreadID, WM_QUIT, 0, 0);
					}
				}
				break;
			}
		}
		break;
	}
}

void HShieldSetup::HSErrorMessageBox()
{
#if !defined(_JP)
	if ( GetEtDevice() )
	{
		HRESULT nResult = GetEtDevice()->SetDialogBoxMode(true);
	}

	std::wstring str = GetLastErrorMsg();

	if ( str.empty() )
	{
		str = _T("Unknown HackShield Error.(It caused ErrorHandler can't get any ErrorMSG.)");
	}

	if( g_pServiceSetup )
		g_pServiceSetup->WriteStageLog_( IServiceSetup::SecurityModule_Failed,L"HackSheild_Failed");
	ErrorLog("g_pServiceSetup->WriteStageLog_(IServiceSetup::SecurityModule_Failed,HackSheild_Failed)");	

	m_hForceQuitThread = (HANDLE)_beginthreadex( NULL, 65536, ForceQuitThread, NULL, 0, &m_dwForceQuitThreadID );

	MessageBox(m_Info.m_hWnd, str.c_str(), L"DragonNest", 0);

	PostThreadMessage(m_Info.m_dwMainThreadID, WM_QUIT, 0, 0);
#endif
}

UINT __stdcall HShieldSetup::ForceQuitThread( void *pParam )
{
	Sleep(5000);

	_exit(0);
	return 0;
}

#endif //_HSHIELD
