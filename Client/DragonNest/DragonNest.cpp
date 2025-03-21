// DragonNest.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include <conio.h>
#include "DragonNest.h"
#include "DnMainFrame.h"
#include "DnCmdLineParse.h"
#include "GlobalValue.h"
#include "PerfCheck.h"
#include "ExceptionDump.h"
#include "FireWall.h"
#include "DnServiceSetup.h"
#include "ClientSessionManager.h"
#include "BugReporter.h"
#include "GameOption.h"
#include "DnLoadingTask.h"
#include "DnString.h"
#include "CountryByCountryKeyboard.h"
#include "InputWrapper.h"
#include "MemPool.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 
#if defined(_WORK)
#include "PsUpdater.h"
#endif	// #if defined(_WORK)

#ifdef _CH
#include "DnSDOAService.h"
#endif

#include "shellapi.h"
#include "DnLocalDataMgr.h"
#ifdef PRE_ADD_STEAMWORKS
#include "../../Extern/Steamworks/public/steam/steam_api.h"
#pragma comment (lib, "../../Extern/Steamworks/redistributable_bin/steam_api.lib")
#endif // PRE_ADD_STEAMWORKS

#ifndef _FINAL_BUILD
//#define _MEM_ALLOC_CHECK
#endif
#if defined(_MEM_ALLOC_CHECK)
CSyncLock memLock;
#include "ThreadCallstack.h"
size_t g_AllocSize = 0;
void *pAllocPtr = 0;
std::map<std::string, float> AllocateCounterMax;
std::map<std::string, float> AllocateCounterCurrent;
std::map<void *, std::pair<std::string, size_t> > sizeBlocks;
VOID LogMsgProc(LPCSTR pText, UINT pTextSize, LPVOID pParam)
{
	ScopeLock<CSyncLock> Lock(memLock);
	AllocateCounterCurrent[ pText ] += (float)(g_AllocSize/(1024.0f*1024.0f));
	AllocateCounterMax[ pText ] += (float)(g_AllocSize/(1024.0f*1024.0f));
	sizeBlocks[ pAllocPtr ] = std::make_pair( pText, g_AllocSize);
}
CThreadCallstack g_ThreadCallstack;
bool bSkipCheck = false;
void *__CRTDECL operator new(size_t size) _THROW1(_STD bad_alloc)
{
	void *p;
	if ((p = malloc(size)) == 0)
	{       // report no memory
		static const std::bad_alloc nomem;
	_RAISE(nomem);
	}
	// try to allocate size bytes
	static size_t nCheck = 4096;
	if( size > nCheck && !bSkipCheck ) {
		bSkipCheck = true;
		if( g_ThreadCallstack.IsOpen() ) {
			g_AllocSize = size;
			pAllocPtr = p;
			g_ThreadCallstack.Write(LogMsgProc, NULL, "");	
		}
		bSkipCheck = false;
	}
	return (p);
}
void __CRTDECL operator delete (void *p) 
{
	free( p );
	ScopeLock<CSyncLock> Lock(memLock);
	if( sizeBlocks.count(p) != 0 ) {
		ASSERT( sizeBlocks[p].second > 0 );
		AllocateCounterCurrent[ sizeBlocks[p].first ] -= (float)(sizeBlocks[p].second/(1024.0f*1024.0f));
		if( AllocateCounterCurrent[ sizeBlocks[p].first ] <= 0.00000001f ) {
			AllocateCounterCurrent.erase( sizeBlocks[p].first );
		}
		sizeBlocks.erase( p );
	}
}
#endif

#ifdef _HSHIELD
#include "HShieldSetup.h"
#endif // _HSHIELD


#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR g_szTitle[MAX_LOADSTRING];				// The title bar text
TCHAR g_szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);

void LogoutNotifyDialog();
ULONG ProcIDFromWnd(HWND hwnd);
HWND GetWinHandle(ULONG pid);
void ExecuteEndingBanner();

#ifndef _LIB
int AddToFirewall()
{
	WriteLog( 1, ", Info, AddToFirewall..\n" );
	HRESULT hr = S_OK;
	HRESULT comInit = E_FAIL;
	INetFwProfile* fwProfile = NULL;

	// Initialize COM.
	comInit = CoInitializeEx(
		0,
		COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE
		);

	// Ignore RPC_E_CHANGED_MODE; this just means that COM has already been
	// initialized with a different mode. Since we don't care what the mode is,
	// we'll just use the existing mode.
	if (comInit != RPC_E_CHANGED_MODE)
	{
		hr = comInit;
		if (FAILED(hr))
		{
			WriteLog(1, ", Error, CoInitializeEx failed: 0x%08lx\n", hr);
			goto error;
		}
	}

	// Retrieve the firewall profile currently in effect.
	hr = WindowsFirewallInitialize(&fwProfile);
	if (FAILED(hr))
	{
		WriteLog(1, ", Error, WindowsFirewallInitialize failed: 0x%08lx\n", hr);
		goto error;
	}

	WCHAR wszFileName[ _MAX_PATH ];
	HMODULE hModule;
	hModule = GetModuleHandle( NULL );
	GetModuleFileNameW( hModule, wszFileName, _MAX_PATH );
	// Add Windows Messenger to the authorized application collection.
	hr = WindowsFirewallAddApp(
		fwProfile,
		wszFileName,
		L"Dragon Nest"
		);
	if (FAILED(hr))
	{
		WriteLog(1, ", Error, WindowsFirewallAddApp failed: 0x%08lx\n", hr);
		goto error;
	}

error:

	// Release the firewall profile.
	WindowsFirewallCleanup(fwProfile);

	// Uninitialize COM.
	if (SUCCEEDED(comInit))
	{
		CoUninitialize();
		WriteLog( 1, ", Info, Finalize AddFirewall.. True" );
	}
	WriteLog( 1, ", Error, Finalize AddFirewall.. false" );

	return 0;

}
#endif //_LIB

#undef _SCL_SECURE_VALIDATE_RANGE

std::string g_szDebugStr1;
std::string g_szDebugStr2;
std::string g_szDebugStr3;
std::string g_szDebugStr4;
std::string g_szDebugStr5;

class DnExceptionCallback : public ExceptionCallback
{
public:
	virtual void OnException(_EXCEPTION_POINTERS* pExceptionInfo)
	{
#ifdef _KR
		//blondy 
		//크래쉬 보고 
		g_pServiceSetup->WriteErrorLog_( 1 ,L"Game_Crash");
		//blondy end
#endif
		ErrorLog("APP Crashed!!");
#ifdef _HSHIELD
		HShieldSetup::Stop();
#endif 
		GetEtDevice()->SetDialogBoxMode( true );
	}
}g_ExceptionCallback;

#if !defined( _FINAL_BUILD )

bool bIsGTest( LPTSTR lpCmdLine )
{
	std::wstring				strValue(lpCmdLine);
	std::vector<std::wstring>	vSplit;
	boost::algorithm::split( vSplit, strValue, boost::algorithm::is_any_of(" ") );

	UINT uiFound = static_cast<UINT>(vSplit.size());


	for( UINT i=0 ;i<vSplit.size() ; ++i )
	{
		if( wcswcs( vSplit[i].c_str(), L"--gtest_filter" ) )
		{
			uiFound = i;
			break;
		}
	}

	if( uiFound == vSplit.size() )
		return false;

	wchar_t *argv[] = { NULL, const_cast<wchar_t*>(vSplit[uiFound].c_str()) };
	
	int argc = 2;
	AllocConsole();
	testing::InitGoogleMock(&argc, argv);
	freopen( "CONOUT$", "wt", stdout );
	return true;
}
#endif

#if defined(_KR) && defined(_FINAL_BUILD)
void SendEyeWebPost(const char* szLogFile)
{
	return;
	char szUrl[2048] = {0,};
	char szCookieValue[2048] = {0,};

	sprintf_s( szUrl, "http://211.39.159.9/%s", szLogFile );

	HINTERNET hInternet = InternetOpenA("USER-AGENT-GAME", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);

	if( hInternet == NULL )
		return;

	HINTERNET hURL = InternetOpenUrlA( hInternet, szUrl, NULL, 0, INTERNET_FLAG_RELOAD, 0 );
	if( hURL == NULL )
	{
		InternetCloseHandle( hInternet );
		return;
	}

	InternetCloseHandle( hURL );
	InternetCloseHandle( hInternet );

}
#endif 

#ifdef PRE_CRASH_CHECK_BACKUP
BYTE cOption[ sizeof(CGameOption) ];
CGameOption *g_BackupOption = (CGameOption*)cOption;
#endif

CGameOption* g_pOption = NULL;
CDnLocalDataMgr* g_pLocalData = NULL;
void LoadOption()
{
	// 해상도 셋팅이나 그래픽 옵션 바꾸다 비정상 종료 됐으면 이전 Config.dat_ 파일을 원래 파일로 카피한다.
	if( GetFileAttributes( _T( "Config.ini_" ) ) != INVALID_FILE_ATTRIBUTES )
	{
		CopyFile( _T( "Config.ini_" ), _T( "Config.ini" ), FALSE );
		DeleteFile( _T( "Config.ini_" ) );
	}

	// 최종버전에선 GameOption 사용( Config.cfg 파일 사용되지 않는다. )
	// ip 랑 port 는 Final 에서도 들어가도록 수정.
	g_pOption = new CGameOption;
	g_pOption->Load();

#ifdef PRE_CRASH_CHECK_BACKUP
	*g_BackupOption = *g_pOption;
#endif

	g_pLocalData = new CDnLocalDataMgr;
	bool bIsLoadOk = g_pLocalData->Load();
#ifdef PRE_MOD_IDN_GRAPHICOPTION_LOW
	if (bIsLoadOk == false ||
		g_pLocalData->IsSetForceGraphicOptionLow() == false)
	{
		g_pOption->m_nGraphicQuality = CGameOption::Graphic_Low;
		g_pOption->m_nCurGraphicQuality = g_pOption->m_nGraphicQuality;
	}
#endif
}

void InitializeBuildVersion()
{
	char buff[512] = {0,};
	sprintf_s( buff, "DN.%d.%d.%s(%d).%s.%04d.%02d.%02d %02d:%02d", g_nBuildVersionMajor, g_nBuildVersionMinor, g_szBuildVersionRevision,
		g_nBuildCount, g_szBuildString, g_nBuildTimeYear, g_nBuildTimeMonth, g_nBuildTimeDay, g_nBuildTimeHour, g_nBuildTimeMinute );

	ToWideString( buff, CGlobalInfo::GetInstance().m_wszBuildVersion );
}

void InitializeBugReporter( HINSTANCE hInstance )
{
	int nDumpLevel = 1;
#ifndef _FINAL_BUILD
	if( CGlobalValue::IsActive() )
	{
		nDumpLevel = CGlobalValue::GetInstance().m_nDumpLevel;
	}
#endif
	// 회사 내부에서 실행하면 무조건 풀덤프 남도록 한다..
/*
	if( GetFileAttributes( _T( "\\\\193.168.0.11\\tooldata\\Utility\\EtWorldPainter.exe" ) ) != INVALID_FILE_ATTRIBUTES )
	{
		nDumpLevel = 2;
	}
*/

	if( ( nDumpLevel >= 0 ) && ( nDumpLevel <= 2 ) )
	{
		std::string szBuildVersion;
		ToMultiString( CGlobalInfo::GetInstance().m_wszBuildVersion, szBuildVersion );
		gs_BugReporter.SetBuildVersion( szBuildVersion );
		
		BugReporter::ReportingServerInfo info;
#ifdef _FINAL_BUILD
		info.bSaveLocal = false;
#else
		info.bSaveLocal = true;
#endif
		info.bUseAutoView = false;
		info.bUseConfirm = true;

		std::wstring szDumpNoticeTitle;
		std::wstring szDumpNoticeMsg;

		szDumpNoticeTitle = STR_DUMP_NOTICE_TITLE;
		szDumpNoticeMsg = STR_DUMP_NOTICE_MSG;
		info.szSendButtonStr = STR_DUMP_SEND_BUTTON;
		info.szNotSendButtonStr = STR_DUMP_NOT_SEND_BUTTON;

		WCHAR wszTemp[4096];
		info.szAppName = g_szTitle;
		swprintf_s( wszTemp, szDumpNoticeTitle.c_str(), g_szTitle );
		info.szNoticeTitle = wszTemp;
		swprintf_s( wszTemp, szDumpNoticeMsg.c_str(), g_szTitle );
		info.szNoticeMsg = wszTemp;

		info.hInst = hInstance;
		info.hAppIcon = ::LoadIcon((HINSTANCE)GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_DRAGONNEST));

		gs_BugReporter.SetReportingServerInfo(info);
#if defined(_CH) || defined(_SG)
		gs_BugReporter.EnableCompressDump( true );
#endif // #if defined(_CH) || defined(_SG)
		gs_BugReporter.Enable( ( BugReporter::DumpLevel )nDumpLevel );
//		gs_BugReporter.AddLogW( L"lpCmdLine : %s", lpCmdLine );
		gs_BugReporter.AddDebugData( &g_szDebugStr1 );
		gs_BugReporter.AddDebugData( &g_szDebugStr2 );
		gs_BugReporter.AddDebugData( &g_szDebugStr3 );
		gs_BugReporter.AddDebugData( &g_szDebugStr4 );
		gs_BugReporter.AddDebugData( &g_szDebugStr5 );
		gs_BugReporter.SetExceptionCallBack( &g_ExceptionCallback );
	}
}

bool InitializeFrame( CDnMainFrame& Frame )
{
	if( Frame.InitializeWindow() == false ) {
		MessageBox( NULL, L"InitializeWindow Failed.", L"Error", MB_OK );
		return false;
	}

	if( Frame.PreInitializeDevice() == false ) {		
		MessageBox( NULL, L"PreInitializeDevice Failed.", L"Error", MB_OK );
#if defined(_KR)
		//blondy 그래픽 디바이스 에러
		g_pServiceSetup->WriteErrorLog_( 4 ,L"GRAPHIC_DEVICE_ERROR" );
		DebugLog("g_pServiceSetup->WriteErrorLog_(4 ,GRAPHIC_DEVICE_ERROR)");
		//blondy end
#endif	// #if defined(_KR)
		return false;
	}

	PROFILE_TICK_TEST_BLOCK_START(" PreInitialize()" );
	if( Frame.PreInitialize() == false ) {
#if defined(_KR)
		//blondy 리소스 파일 에러
		g_pServiceSetup->WriteErrorLog_( 7 ,L"RESOURCE_PACKING_CORRUPTED_ERROR" );
		DebugLog("g_pServiceSetup->WriteErrorLog_(7 ,RESOURCE_PACKING_CORRUPTED_ERROR)");
		//blondy end
#endif	// #if defined(_KR)
		MessageBox( NULL, L"PreInitialize Failed", L"Error", MB_OK );
		return false;
	}
	PROFILE_TICK_TEST_BLOCK_END();

	PROFILE_TICK_TEST_BLOCK_START(" InitlizeDevice()" );
	if( Frame.InitializeDevice() == false ) {
//		MessageBox( NULL, L"InitializeDevice Failed.", L"Error", MB_OK );
#if defined(_KR)
		//blondy 인풋 디바이스 에러
		g_pServiceSetup->WriteErrorLog_( 5 ,L"INPUT_DEVICE_ERROR" );
		DebugLog("g_pServiceSetup->WriteErrorLog_(5 ,INPUT_DEVICE_ERROR)");
		//blondy end
#endif	// #if defined(_KR)
		return false;
	}
	PROFILE_TICK_TEST_BLOCK_END();

	PROFILE_TICK_TEST_BLOCK_START(" Initialize()" );
	if( Frame.Initialize() == false ) {
		MessageBox( NULL, L"Initialize Failed.", L"Error", MB_OK );
#if defined(_KR)
		//blondy 알수없는 에러
		g_pServiceSetup->WriteErrorLog_( 99 ,L"UNKNOWN_ERROR" );
		DebugLog("g_pServiceSetup->WriteErrorLog_(99 ,UNKNOWN_ERROR)");
		//blondy end
#endif	// #if defined(_KR)
		return false;
	}
	PROFILE_TICK_TEST_BLOCK_END();

	return true;
}

//////////////////////////////////////////////////////////////////////////
// WinMain
extern "C" { FILE __iob_func[3] = { *stdin,*stdout,*stderr }; }
extern bool g_bEndingBanner;
int APIENTRY _tWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow )
{
#ifdef PRE_ADD_STEAMWORKS
	TCHAR* pCmdLine = ::GetCommandLine();
	std::wstring wszCmdLine; 
	wszCmdLine = pCmdLine;
	std::wstring::size_type pos = wszCmdLine.find( L" " );
	if( std::wstring::npos != pos )
		wszCmdLine.erase( 0, pos + 1 );

	if( StrStrW( wszCmdLine.c_str() , L"steam" ) )
	{
		if( !SteamAPI_Init() )
		{
			ErrorLog( "SteamAPI_Init() failed\n" );
			return -1;
		}
	}
#endif // PRE_ADD_STEAMWORKS

//	CLfhHeap::GetInstance()->InitPool();

#if defined(_FILELOG_PACKET) || defined(_FILELOG_SPOS)
	g_Log.Init( L"DragonNest", LOGTYPE_FILE );
#endif

#ifdef PRE_MEMORY_SHUFFLE
	MSDT_SETTERM( s_fDefaultFps, EV_MSDT_ALTT_VERYFAST );
#endif	// PRE_MEMORY_SHUFFLE

#if defined(_WORK)
#if !defined(_DEBUG)
	if( !g_PsUpdater )
		g_PsUpdater = new(std::nothrow) CPsUpdater;

	g_PsUpdater->AsyncUpdate();
#endif
#endif	// #if defined(_WORK)

//#if defined(_KR) && defined(_FINAL_BUILD)
//	SendEyeWebPost("clientstart.aspx");
//#endif

#if !defined( _FINAL_BUILD )
	if( bIsGTest( lpCmdLine ) )
	{
		int Ret = RUN_ALL_TESTS();
		_getch();
		FreeConsole();
		return Ret;
	}
#endif

#if defined(_MEM_ALLOC_CHECK)
	g_ThreadCallstack.Open();
#endif
	
	WriteLog( 0, ", Info, Client Initialize\n" );
	WriteLog( 1, ", Info, Client Initialize\n" );
	// 혹시 모를 체크 -_-
	CHECK_ALIGNMENT();

	UNREFERENCED_PARAMETER( hPrevInstance );
	UNREFERENCED_PARAMETER( lpCmdLine );
#ifdef _DEBUG
//	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
//	_CrtSetBreakAlloc(2030000);
//	_CrtMemDumpAllObjectsSince(0); 
//	_CrtSetBreakAlloc(4001507);
#endif //_DEBUG
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );

#ifndef _LIB
	AddToFirewall();
#endif //_LIB

	WriteLog( 1, ", Info, Build Version Type : %s\n", g_szBuildString );

	// 최종버전에선 GlobalValue 자체를 참조안한다.
#ifndef _FINAL_BUILD
	CGlobalValue Value;
#endif //_FINAL_BUILD
	CGlobalInfo Info;
	
	CDnMainFrame Frame( hInstance );
	LoadOption();									// 옵션 로딩

	InitializeBuildVersion();

//	TCHAR str[] =  L"metp://119.235.231.236:8080//go/go;USN:12=100000003231&fciv:1=0&fcvr:1=0&jidType:1=1&k111e:4=0000&k125e:2=26&k126e:6=urlhgc&k150e:4=1.00&k22e:7=J_DNEST&k37e:6=999999&k3e:9=pepsired2&k60e:10=DragonNest&k87e:3=hgc&k88e:51=http://pubdownt.hangame.co.jp/downbase/purple/sdurl&k89e:50=http://pubdownt.hangame.co.jp/downbase/purple/dist&k90e:4=1.00&k91e:4=1.00&k93e:7=Hangame&k95e:25=http://www.hangame.co.jp/&k96e:49=http://pubdownt.hangame.co.jp/downbase/purple/hul&lang:8=JAPANESE&m1e:7=2160257&mbrAccId:12=100000003231&nors:1=Y&plii:50=http://pubdownt.hangame.co.jp/downbase/purple/plii&pub10e:4=4355&pub1e:341=hangametest=UjU8ItIQsO7ph_ATC152%2CU4PXJ7dm60BXX8DS80afhUALHSE-P7fQn42fJUbksSYf676A%2CUz5xovmPX-Xp0868%2CM%2C1%2CY%2C111ANNN-4_P_V_U_L0X2_H2F4_F01_PE6_21DI_41HB%2CY%2C*%2CUa2IDF3F%2C%2C%2C%2C%2C%2Chgj%2C195254%2CUUUU875A%2CHGJ%2CUspnP7DYRMfspRvxnHEWfvaKDnz--UEianfzpRAxjn7d53A4%2C%2C8%2C0%2CUsznp7dFnM7W803E%2CUszsPl6yU_6Y08EA%2CUnU7FEEA%2C1&pub2e:12=hangametest=&pub3e:20=hangametest=alpha-id&pub4e:48=UspnP7DYRMDRpvL5nidCfRCyDmDP-1dEam6rpvK5jmxc0622&pub5e:0=&pub6e:24=hangametest=Uszn-7KA0DFD&pubenc:1=y&pubpwd:5=dummy&scvr:1=1&wbers:1=y ID:100000003231 /ip:10.0.3.15 /port:14300 /use_packing";
//	TCHAR str[] =  L"2012-10-10\_322\_702\_700 55212457EDD1739BF29F68AA654F9EBE6E8A5203FBCC2DA098C251646A53B15DD66D 31 00160F52 0 /window:true /res:1024x768 /ip:211.39.159.20 /port:14300 /respath:R:\TB_KOR /nation:KOR";
//	lpCmdLine = str;
	DebugLog("CommandLine %s",lpCmdLine);

	CDnCmdLineParse Parse( lpCmdLine );

#if defined( PRE_ADD_MULTILANGUAGE )
	CCountryByCountryKeyboard::CreateInstance();
	int nKeyboardType = CCountryByCountryKeyboard::eQWERTY_KEYBOARD;

#if defined(PRE_ADD_COUNTRY_BY_COUNTRY_CONTROL)
	MultiLanguage::SupportLanguage::eSupportLanguage eLanuage = CGlobalInfo::GetInstance().m_eLanguage;
	if( MultiLanguage::SupportLanguage::Fra == eLanuage )
		nKeyboardType = CCountryByCountryKeyboard::eAZERTY_KEYBORAD;
	else if( MultiLanguage::SupportLanguage::Ger == eLanuage )
		nKeyboardType = CCountryByCountryKeyboard::eQWERTZ_KEYBORAD;
	else if( MultiLanguage::SupportLanguage::Esp == eLanuage )
		nKeyboardType = CCountryByCountryKeyboard::eSPAIN_KEYBOARD;
#endif	// #if defined(PRE_ADD_COUNTRY_BY_COUNTRY_CONTROL)

	GetCountryByCountryKeyboard().ChangeCountryByKeyboard( nKeyboardType, g_szKeyString );
#endif	// #if defined( PRE_ADD_MULTILANGUAGE )

	int nResult= 0;
	g_pServiceSetup = DnServiceSetup::Create();		// 퍼블리셔별 서비스 셋업 ( 보안툴 관련 호출 부분이 여기서 실행되므로 가급적 이 함수가 프로그램 시작후 바로 실행 되도록 한다. )

#if defined(_KRAZ) && defined(_AUTH) && defined(_FINAL_BUILD)
	if( (nResult = g_pServiceSetup->PreInitialize(lpCmdLine)) < 0 )
#else
	if( (nResult = g_pServiceSetup->PreInitialize(0)) < 0 )
#endif
	{
		ErrorLog( "g_pServiceSetup->PreInitialize failed Result:%d", nResult );
		return 0;
	}

	swprintf_s( g_szTitle, STR_APP_TITLE );			// 윈도우 Title 셋팅 ( 국가별로 다름 )
	swprintf_s( g_szWindowClass, STR_DRAGONNEST );

	InitializeBugReporter( hInstance );				// 버그 리포트 초기화

	MyRegisterClass(hInstance);

#ifdef _FINAL_BUILD									// 중복 실행 검사 ( 개발시에는 중복실행이 필요할 수 있다. )
	SECURITY_DESCRIPTOR m_SD;
	SECURITY_ATTRIBUTES m_SA;
	HANDLE m_hMutex;
	InitializeSecurityDescriptor( &m_SD, SECURITY_DESCRIPTOR_REVISION );
	SetSecurityDescriptorDacl( &m_SD, TRUE, NULL, FALSE );
	m_SA.nLength = sizeof(m_SA);
	m_SA.lpSecurityDescriptor = &m_SD;
	m_SA.bInheritHandle = FALSE;
	m_hMutex = CreateMutex( &m_SA, FALSE, L"Global\\MutexDragonNest" );
	if( GetLastError() == ERROR_ALREADY_EXISTS )
	{
		ReleaseMutex( m_hMutex );
		CloseHandle( m_hMutex );

#if defined(_KR)
		g_pServiceSetup->WriteErrorLog_( 6, L"APP_DUPLICATED_ERROR" );
#endif	 //#if defined(_KR)
		//return 0;  //[debug]  不允许多开
	}
#endif  //_FINAL_BUILD
	
	if( !InitializeFrame( Frame ) )					// 메인 프레임 초기화
	{
#ifdef _FINAL_BUILD
		ReleaseMutex( m_hMutex );
		CloseHandle( m_hMutex );
#endif // _FINAL_BUILD
		return 0;
	}
#ifdef _CH
	SetForegroundWindow( Frame.GetHWnd() );			// SDOA뜨고나면 뒤로가서 앞으로 직접 이동시킨다.
#endif //_CH
	gs_BugReporter.GetReportingServerInfo().hWnd = Frame.GetHWnd();

	CClientSessionManager::GetInstance().AddListener( g_pServiceSetup );

	// 게임 메인 프로세스
	g_pServiceSetup->WriteStageLog_( IServiceSetup::GameStart, L"Game_Start" );
	DebugLog( "g_pServiceSetup->WriteStageLog_( IServiceSetup::GameStart , Game_Start )" );
	
	if( Frame.Execute() == false ) 
	{
		OutputDebug( "Quit Failed.\n" );
#if defined(_KR)
		g_pServiceSetup->WriteErrorLog_( 99, L"UNKNOWN_ERROR" );
		DebugLog( "g_pServiceSetup->WriteErrorLog_(99 ,UNKNOWN_ERROR)" );
#endif	// #if defined(_KR)
#ifdef _FINAL_BUILD
		ReleaseMutex( m_hMutex );
		CloseHandle( m_hMutex );
#endif // _FINAL_BUILD
		return 0;
	}

	if( CDnLoadingTask::IsActive() ) 
	{
		CDnLoadingTask::GetInstance().EnableBackgroundLoading( false );
	}

	CClientSessionManager::GetInstance().DisConnectServer( true );

	Frame.SaveWindowPos();

	g_pOption->Save();								// 옵션 저장
	SAFE_DELETE( g_pOption );
	if (g_pLocalData)
	{
		g_pLocalData->Save();
		SAFE_DELETE( g_pLocalData );
	}

#ifdef _HSHIELD
	HShieldSetup::Stop();
#endif //_HSHIELD
	
#ifndef _FINAL_BUILD
	if( CGlobalValue::GetInstance().m_bUseLogWnd )
		LogWnd::DestroyLog();
#endif 

	WriteLog( 1, ", Info, Client Finalize\n" );
	WriteLog( 0, ", %s, Client Finalize\n", ( g_bInvalidDisconnect ) ? "Warning" : "Info" );

#if defined(_WORK)
	SAFE_DELETE( g_PsUpdater );
#endif	// #if defined(_WORK)
	
#ifdef _FINAL_BUILD
	ReleaseMutex( m_hMutex );
	CloseHandle( m_hMutex );
#endif
	
	g_pServiceSetup->WriteStageLog_( IServiceSetup::Game_Exit, L"Game_exit" );	
	DebugLog( "g_pServiceSetup->WriteStageLog_( IServiceSetup::Game_Exit ,Game_exit)" );
	g_pServiceSetup->Release();
	SAFE_DELETE( g_pServiceSetup );					// 서비스 셋업 해제

	Frame.Finalize();
	Frame.FinalizeDevice();

#ifdef PRE_ADD_STEAMWORKS
	if( CGlobalInfo::GetInstance().m_bUseSteamworks )
		SteamAPI_Shutdown();
#endif // PRE_ADD_STEAMWORKS

	LogoutNotifyDialog();

	return 0;
}


//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DRAGONNEST));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;//MAKEINTRESOURCE(IDC_DRAGONNEST);
	wcex.lpszClassName	= g_szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
#ifdef _HSHIELD
	if ( message == WM_ALERT_HSHIELD )
	{
		if ( GetEtDevice() )
		{
			HRESULT nResult = GetEtDevice()->SetDialogBoxMode(true);
		}
		
		std::wstring str = HShieldSetup::GetLastErrorMsg();
		if ( str.empty() == false )
		{
			MessageBox(hWnd, str.c_str(), _T("DragonNest"), MB_OK );
		}else
		{
			MessageBox(hWnd, _T("Unknown HackShield Error.(It caused ErrorHandler can't get any ErrorMSG.)"), _T("DragonNest"), MB_OK );
		}
	}
#endif // _HSHEILD_
	
	if( CDnMainFrame::IsActive() == false ) return 0;

	if( g_pServiceSetup )
	{
		g_pServiceSetup->MsgProc(hWnd, message, wParam, lParam);
	}

#if defined(_CH) && defined(_AUTH)
	if( CDnSDOAService::IsActive() )
	{
		LRESULT lResult = 0;
		bool bNoFurtherProcessing = false;
		CDnSDOAService::GetInstance().WndProc( hWnd, message, wParam, lParam, &bNoFurtherProcessing );
		if( bNoFurtherProcessing )
		{
			// 마우스오버된 컨트롤 강제로 푼다.
			if( message == WM_MOUSEMOVE )
				CEtUIDialog::ReleaseMouseEnterControl();
				//CDnMainFrame::GetInstance().WndProc( hWnd, message, wParam, 0 );
			return lResult;
		}
	}
#endif //_CH

	return CDnMainFrame::GetInstance().WndProc( hWnd, message, wParam, lParam );
}


//////////////////////////////////////////////////////////////////////////
void LogoutNotifyDialog()
{
#if defined (_US)
#ifdef PRE_ADD_STEAMWORKS
	if( !CGlobalInfo::GetInstance().m_bUseSteamworks )
		ExecuteEndingBanner();
#else // PRE_ADD_STEAMWORKS
	ExecuteEndingBanner();
#endif // PRE_ADD_STEAMWORKS
	return;
#endif

#if !defined (_CH) && !defined (_TH) && !defined (_EU)
	return;
#else
	BOOL result;

	int fullx = ::GetSystemMetrics(SM_CXFULLSCREEN);
	int fully = ::GetSystemMetrics(SM_CYFULLSCREEN);

	STARTUPINFO si;  
	ZeroMemory(&si,sizeof(si));  
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USEPOSITION | STARTF_USESIZE | STARTF_USESHOWWINDOW;
	si.dwXSize = 800;
	si.dwYSize = 600;
	si.dwX = static_cast<int>( (fullx - si.dwXSize) * 0.5f );
	si.dwY = static_cast<int>( (fully - si.dwYSize) * 0.5f );

	si.wShowWindow = SW_SHOWDEFAULT;

	PROCESS_INFORMATION pi;  
	
	WCHAR appPath[255];

#if defined(_CH)
	std::wstring wUrl = L"http://dn.sdo.com/project/popad/index.html";
#elif defined(_TH)
	std::wstring wUrl = L"http://games.playpark.com/";
#elif defined(_ID)
	std::wstring wUrl = L"http://dn.gemscool.com";
#elif defined(_EU)
	std::wstring wUrl = L"http://ingame.dragonnest.eu/pages/en/ending/endingpage.asp";
	switch(CGlobalInfo::GetInstance().m_eLanguage)
	{
	case MultiLanguage::SupportLanguage::Eng:
		{
			wUrl = L"http://ingame.dragonnest.eu/pages/en/ending/endingpage.asp";
		}
		break;
	case MultiLanguage::SupportLanguage::Ger:
		{
			wUrl = L"http://ingame.dragonnest.eu/pages/de/ending/endingpage.asp";
		}
		break;
	case MultiLanguage::SupportLanguage::Fra:
		{
			wUrl = L"http://ingame.dragonnest.eu/pages/fr/ending/endingpage.asp";
		}
		break;
	case MultiLanguage::SupportLanguage::Esp:
		{
			wUrl = L"http://ingame.dragonnest.eu/pages/es/ending/endingpage.asp";
		}
		break;
	}
#endif
	//std::wstring wUrl = L"http://dn.sdo.com/project/popad/index.asp";
	
	HKEY hkey;
	LONG ReturnValue=RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\IEXPLORE.EXE", 0, KEY_ALL_ACCESS, &hkey);
	if(ReturnValue == ERROR_SUCCESS)
	{
		DWORD dwType;
		DWORD dwSize = 200;
		
		LONG lReturn = RegQueryValueEx (hkey, L"", NULL, &dwType, (BYTE *)appPath, &dwSize);
				
		if(lReturn == ERROR_SUCCESS )
		{
			RegCloseKey(hkey);
		}
	}
	
	std::wstring command = appPath;
	command += L" ";				// 공백
	command += L"\"";				// "시작
	command += wUrl;				// url 주소
	command += L"\"";				// "끝
	
	result = CreateProcess (NULL, (LPTSTR)command.c_str(), NULL, NULL, true, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi);

	/*
	wsprintf(appPath, L"mshta.exe %s", wUrl.c_str());
	result = CreateProcess (NULL, (LPWSTR)appPath, NULL, NULL, true, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi);
	*/

	/*
	SHELLEXECUTEINFO si;
	ZeroMemory(&si, sizeof(SHELLEXECUTEINFO));
	si.cbSize = sizeof(SHELLEXECUTEINFO);
	si.lpVerb = __TEXT("open");				//mode : print, write
	si.lpFile = __TEXT("iexplore.exe");//__TEXT("netsh.exe");			//실행파일
	si.lpParameters = L"http:\\\\www.google.com";//strParam.c_str();	//실행파일 인자값.
	// shellIP.lpDirectory =				//실핼파일 디렉토리.
	si.nShow = SW_SHOW;						//SW_HIDE

	result = ShellExecuteEx(&si);			//성공시 true
	*/

	DWORD errorID = 0;
	if (result == FALSE)
	{
		errorID = GetLastError();
	}
	else
	{
		HANDLE hProcess = pi.hProcess;

		//DWORD dResult = WaitForInputIdle( hProcess, 5 * 100);
		DWORD dResult = WaitForSingleObject( pi.hProcess, 5 * 100 );

		HWND hWnd = GetWinHandle(pi.dwProcessId); // 프로세스 아이디로 윈도우 핸들 얻기  
		
		if( hWnd != NULL )  
		{
			WINDOWINFO winInfo;
			GetWindowInfo(hWnd, &winInfo);

#if defined(_CH) || defined (_TH) || defined (_ID) || defined (_EU)
			RECT rt={0, 0, 800, 600};
#endif

			AdjustWindowRectEx(&rt, winInfo.dwStyle, false, winInfo.dwExStyle);

			int startX = static_cast<int>( (fullx - (rt.left - rt.right)) * 0.5f );
			int startY = static_cast<int>( (fully - (rt.bottom - rt.top)) * 0.5f );

			MoveWindow(hWnd, startX, startY, rt.right - rt.left, rt.bottom - rt.top, FALSE);
		}
	}
#endif
}

// idea from http://support.microsoft.com/kb/242308/en-us  
ULONG ProcIDFromWnd(HWND hwnd) // 윈도우 핸들로 프로세스 아이디 얻기  
{  
	ULONG idProc;  
	GetWindowThreadProcessId( hwnd, &idProc );  
	return idProc;  
}  

HWND GetWinHandle(ULONG pid) // 프로세스 아이디로 윈도우 핸들 얻기  
{  
	HWND tempHwnd = FindWindow(NULL,NULL); // 최상위 윈도우 핸들 찾기  

	while( tempHwnd != NULL )  
	{  
		if( GetParent(tempHwnd) == NULL ) // 최상위 핸들인지 체크, 버튼 등도 핸들을 가질 수 있으므로 무시하기 위해  
			if( pid == ProcIDFromWnd(tempHwnd) )  
				return tempHwnd;  
		tempHwnd = GetWindow(tempHwnd, GW_HWNDNEXT); // 다음 윈도우 핸들 찾기  
	}  
	return NULL;  
}

void ExecuteEndingBanner()
{
	std::string exeFileName;
	exeFileName = "dnendingbanner.exe";

	if (g_bEndingBanner)
	{
		WinExec(exeFileName.c_str(), SW_SHOW);
	}
}
//////////////////////////////////////////////////////////////////////////