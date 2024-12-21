#include "StdAfx.h"
#include "DnMainFrame.h"
#include "GlobalValue.h"
#include "PerfCheck.h"
#include "InputDevice.h"
#include "TaskManager.h"
#include "Resource.h"
#include "DnTableDB.h"
#include "DnWorld.h"

#include "DnActorClassDefine.h"
#include "DnPlayerActor.h"
#include "DnWeapon.h"
#include "DnInCodeResource.h"

// Task Include
#include "DnTitleTask.h"
#include "DnGameTask.h"
#include "DnCommonTask.h"
#include "DnVillageTask.h"
#include "DnDebugTask.h"
#include "DnLoginTask.h"
#include "DnPartyTask.h"
#include "DnItemTask.h"
#include "DnSkillTask.h"
#include "DnCutSceneTask.h"
#include "DnBridgeTask.h"
#include "DnFriendTask.h"
#include "DnGuildTask.h"
#include "DnTradeTask.h"
#include "DnSkillTask.h"
#include "DnAppellationTask.h"
#include "DnHangCrashTask.h"
#include "DnCashShopTask.h"

// ETC
#include "shlobj.h"
#include <mmsystem.h>
#include "EtSoundEngine.h"
#include "EtSoundChannelGroup.h"
#include "DnMouseCursor.h"

#include "DnMouseCursor.h"
#include "ClientSessionManager.h"

#include "EtUIMan.h"
#include "EtUIIME.h"
#include "GameSendPacket.h"

#include "DnDataManager.h"
#include "DnInterface.h"
#include "DnUISound.h"
#include "DebugSet.h"
#include "EtUIXML.h"

#include "DebugCmdProc.h"
#include "BugReporter.h"

#include "EtBCLEngine.h"

#include "GameOption.h"
#include "EtWater.h"
#ifndef _FINAL_BUILD
#include "DnDebugRender.h"
#include "DnDarklairClearImp.h"
#endif
#include <shellapi.h>
#include <Direct.h>
#include "DnServiceSetup.h"
#include "EtRenderStack.h"
#include "DnPlayerCamera.h"
#include "..\\..\\Common\\EtFileSystem\\EtFileSystem.h"

#ifdef _USE_VOICECHAT
#include "..\\..\\Common\\VoiceChat\\VoiceChatClient.h"
#endif

#include "DnAuthTask.h"

#ifdef _CH
#include "DnSDOAService.h"
#endif

#include "SystemInfo.h"
#include "DnMainMenuDlg.h"

#include "DnRecordDlg.h"

#include "SyncTimer.h"

#include "EtActionCoreMng.h"
#include "EtMRTMng.h"

#include "DNSecure.h"

#ifdef _TEST_CODE_KAL
#include "ItemSendPacket.h"
#endif

#include "DnLifeChannelDlg.h"

#ifdef PRE_ADD_ITEMNAME_AUTOCOMPLETE
#include "DnNameAutoComplete.h"
#endif		//#ifdef PRE_ADD_ITEMNAME_AUTOCOMPLETE

#ifdef PRE_ADD_DWC
#include "DnDWCTask.h"
#endif

extern TCHAR g_szTitle[128];
extern TCHAR g_szWindowClass[128];

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

#ifndef _FINAL_BUILD
extern bool g_bPauseMode;
#endif

extern HWND g_hwndChargeBrowser;

#include "../RlktGuard/GuardThread.h"

#include "DnLocalPlayerActor.h"
#include "VillageSendPacket.h"
#include "DnChatRoomTask.h"
LPTOP_LEVEL_EXCEPTION_FILTER CDnMainFrame::s_PrevExceptionFilter = NULL;



CDnMainFrame::CDnMainFrame( HINSTANCE hInst )
#ifdef PRE_ADD_STEAMWORKS
: m_CallbackGameOverlayActivated( this, &CDnMainFrame::OnSteamOverlayActivated )
#endif // PRE_ADD_STEAMWORKS
{
	m_dwMainThreadID = GetCurrentThreadId();
	m_hInstance = hInst;
	m_pInputDevice = NULL;
	m_pTaskMng = NULL;
	m_pTableDB = NULL;
	m_pSoundEngine = NULL;
	m_pUISound = NULL;
	m_pSessionMng = NULL;
	m_pCursor = NULL;
	m_pInterface = NULL;
	m_pUIXML = NULL;
	m_pInCodeRes = NULL;
	m_pLoadingTask = NULL;
	m_pHangCrashTask = NULL;
	m_bDestroy = false;
	m_bCreateWindow = false;
	m_pResMng = NULL;

	m_pBridgeTask = NULL;
	m_pAuthTask = NULL;
	m_hWnd = NULL;
#ifdef PRE_ADD_DWC
	m_pDWCTask = NULL;
#endif // PRE_ADD_DWC


	memset( &m_MousePos, 0, sizeof(POINT) );
	memset( &m_WindowPos, 0, sizeof(POINT) );
	m_bBeginCaption = false;
	m_bMaximize = false;
	m_pCaptureEngine = NULL;
	m_pRecordDlg = NULL;
	memset( m_bCheckRecordDiskSpace, 0, sizeof(m_bCheckRecordDiskSpace) );

#ifndef _FINAL_BUILD
	g_pDnDebugRender = new CDnDebugRender();
#endif

	s_PrevExceptionFilter = SetUnhandledExceptionFilter( UnhandledExceptionFilter );
	m_nWinVer = 0;

#ifndef PRE_FIX_CLIENT_MEMOPTIMIZE
	m_hPreloadPlayerThreadHandle = 0;
	m_dwPreloadPlayerThreadID = 0;
	m_bFinishPlayerPreload = true;
#endif
#ifdef PRE_ADD_EXCEPTIONAL_MAKESTRING
	m_pExceptionalUIXML = NULL;
#endif // PRE_ADD_EXCEPTIONAL_MAKESTRING
#ifdef PRE_ADD_ITEMNAME_AUTOCOMPLETE
	m_pNameAutoComplete = NULL;
#endif		//#ifdef PRE_ADD_ITEMNAME_AUTOCOMPLETE

	m_pSyncTimer = new CSyncTimer;
	CDNSecure::CreateInstance();
}

CDnMainFrame::~CDnMainFrame()
{
	SAFE_DELETE( m_pSyncTimer );

#ifndef _FINAL_BUILD
	SAFE_DELETE( g_pDnDebugRender );
#endif
	Finalize();
	FinalizeDevice();

	SAFE_DELETE( g_pEtRenderLock );
}
#ifdef PRE_ADD_RELOAD_ACTFILE
// 지정된 폴더의 하위 파일 리스트를 탐색해서 새롭게 추가된 파일이 존재한다면 추가
void CDnMainFrame::AddResourcePathByFolderName( CFileNameString& szPath )
{
	if( szPath.empty() )	return;

	std::vector<CFileNameString> FolderList;
	char szFullPath[_MAX_PATH] = {0,};
	_FindFolder( szPath.c_str(), FolderList );
	
	for( int i = 0; i < (int)FolderList.size(); ++i )
	{
		sprintf_s( szFullPath, "%s\\%s", szPath.c_str(), FolderList[i].c_str() );
		AddResourcePathByFolderName( CFileNameString( szFullPath ) );
		CEtResourceMng::GetInstance().ReserchAddResourcePath( szFullPath );
	}

}

void CDnMainFrame::ReLoadActionFilePath()
{
	std::string szString;

#ifndef _FINAL_BUILD
	if( !CGlobalValue::GetInstance().m_szResourcePath.empty() ) {
		szString = CGlobalValue::GetInstance().m_szResourcePath.c_str();
	}
#endif //_FINAL_BUILD

	CFileNameString szRootPath = szString;

	AddResourcePathByFolderName( szRootPath + "\\Resource\\Weapon" );
	AddResourcePathByFolderName( szRootPath + "\\Resource\\Char" );
	AddResourcePathByFolderName( szRootPath + "\\Resource\\Particle" );
	AddResourcePathByFolderName( szRootPath + "\\Resource\\StateEffect" );
	AddResourcePathByFolderName( szRootPath + "\\Resource\\Etc" );
}
#endif 

bool CDnMainFrame::PreInitialize()
{
	//InitGuard();
	m_pResMng = new CEtResourceMng( CGlobalInfo::GetInstance().m_bUsePacking );

#ifdef PRE_ADD_MULTILANGUAGE
	m_pResMng->SetLanguage( CGlobalInfo::GetInstance().m_szLanguage );
#endif // PRE_ADD_MULTILANGUAGE

	CFileNameString szRoot = CEtResourceMng::GetInstance().GetCurrentDirectory();

	// 해외OS에서 일부 스트링은 제대로 읽어오지 못하기때문에 이렇게 루트폴더를 별도로 받아야 확실히 찾을 수 있다.
	std::wstring wszRoot = CEtResourceMng::GetInstance().GetRootDirectoryW();

#ifndef _FINAL_BUILD
	if( !CGlobalValue::GetInstance().m_szResourcePath.empty() ) {
		szRoot = CGlobalValue::GetInstance().m_szResourcePath.c_str();
		::ToWideString((char*)szRoot.c_str(), wszRoot);
	}
#endif //_FINAL_BUILD

	gs_BugReporter.AddLogA("MainFrame::PreInit.. RootFolder %s", szRoot.c_str());

	// Packing 등록
	gs_BugReporter.AddLogA("Resource Folder %s", szRoot.c_str());

	CEtResourceMng::GetInstance().SetPackingFolder( wszRoot.c_str() );

	if( CEtResourceMng::GetInstance().IsUsePackingFile() ) {	// 패킹파일 리소스 로딩 실패...
		if( CEtFileSystem::GetInstance().IsEmpty() ) {
			return false;
		}
	}

#ifndef _FINAL_BUILD
	if( !CGlobalValue::GetInstance().m_szNation.empty() ) {
		CFileNameString szNationMapData = szRoot + "\\MapData_" + CGlobalValue::GetInstance().m_szNation;
		CFileNameString szNationResource = szRoot + "\\Resource_" + CGlobalValue::GetInstance().m_szNation;

		CEtResourceMng::GetInstance().AddResourcePath( szNationResource );
		CEtResourceMng::GetInstance().AddResourcePath( szNationResource + "\\SharedEffect" );

		CEtResourceMng::GetInstance().AddResourcePath( szNationMapData );
		CEtResourceMng::GetInstance().AddResourcePath( szNationMapData + "\\Resource" );

		CEtResourceMng::GetInstance().AddResourcePath( szNationResource + "\\Char", true );

		CEtResourceMng::GetInstance().AddResourcePath( szNationResource + "\\Ext" );
		CEtResourceMng::GetInstance().AddResourcePath( szNationResource + "\\Weapon", true );
		CEtResourceMng::GetInstance().AddResourcePath( szNationResource + "\\Item", true );
		CEtResourceMng::GetInstance().AddResourcePath( szNationResource + "\\Sound", true );
		CEtResourceMng::GetInstance().AddResourcePath( szNationResource + "\\Particle" );
		CEtResourceMng::GetInstance().AddResourcePath( szNationResource + "\\HelpContent" );
		CEtResourceMng::GetInstance().AddResourcePath( szNationResource + "\\UIString" );
#ifdef PRE_ADD_RENEWUI
		if (CGlobalValue::GetInstance().m_bReNewUI)
			CEtResourceMng::GetInstance().AddResourcePath( szNationResource + "\\UI_new", true );
		else
			CEtResourceMng::GetInstance().AddResourcePath( szNationResource + "\\UI", true );
#else
		CEtResourceMng::GetInstance().AddResourcePath( szNationResource + "\\UI", true );
#endif
		CEtResourceMng::GetInstance().AddResourcePath( szNationResource + "\\UITemplateTexture" );
		CEtResourceMng::GetInstance().AddResourcePath( szNationResource + "\\Effect" );
		CEtResourceMng::GetInstance().AddResourcePath( szNationResource + "\\fonts" );
#ifdef PRE_ADD_MULTILANGUAGE
		CEtResourceMng::GetInstance().AddResourcePath( szNationResource + "\\Script\\BossAI", true );
		CEtResourceMng::GetInstance().AddResourcePath( szNationResource + "\\Script\\Dark_Lair", true );
		CEtResourceMng::GetInstance().AddResourcePath( szNationResource + "\\Script\\MonsterAI", true );
		CEtResourceMng::GetInstance().AddResourcePath( szNationResource + "\\Script\\QuestNPC_Common", true );

		if( CGlobalInfo::GetInstance().m_szLanguage.empty() )
		{
			CEtResourceMng::GetInstance().AddResourcePath( szNationResource + "\\Script\\Talk_Npc", true );
			CEtResourceMng::GetInstance().AddResourcePath( szNationResource + "\\Script\\Talk_Quest", true );
		}
		else
		{
			CEtResourceMng::GetInstance().AddResourcePath( szNationResource + "\\Script\\Talk_Npc_" + CGlobalInfo::GetInstance().m_szLanguage, true );
			CEtResourceMng::GetInstance().AddResourcePath( szNationResource + "\\Script\\Talk_Quest_" + CGlobalInfo::GetInstance().m_szLanguage, true );
		}
#else // PRE_ADD_MULTILANGUAGE
		CEtResourceMng::GetInstance().AddResourcePath( szNationResource + "\\Script", true );
#endif // PRE_ADD_MULTILANGUAGE
		CEtResourceMng::GetInstance().AddResourcePath( szNationResource + "\\Dmv" );
		CEtResourceMng::GetInstance().AddResourcePath( szNationResource + "\\NPC" );
		CEtResourceMng::GetInstance().AddResourcePath( szNationResource + "\\Quest" );
		CEtResourceMng::GetInstance().AddResourcePath( szNationResource + "\\StateEffect" );
		CEtResourceMng::GetInstance().AddResourcePath( szNationResource + "\\Etc" );
		CEtResourceMng::GetInstance().AddResourcePath( szNationResource + "\\Camera" );
		CEtResourceMng::GetInstance().AddResourcePath( szNationResource + "\\Movie" );
	}

#endif //_FINAL_BUILD

	// Path 설정
	CEtResourceMng::GetInstance().AddResourcePath( szRoot + "\\Resource" );
	CEtResourceMng::GetInstance().AddResourcePath( szRoot + "\\Resource\\SharedEffect" );

	CEtResourceMng::GetInstance().AddResourcePath( szRoot + "\\MapData" );
	CEtResourceMng::GetInstance().AddResourcePath( szRoot + "\\MapData\\Resource" );

	CEtResourceMng::GetInstance().AddResourcePath( szRoot + "\\Resource\\Char", true );

	CEtResourceMng::GetInstance().AddResourcePath(szRoot + "\\Resource\\Ext");
	CEtResourceMng::GetInstance().AddResourcePath(szRoot + "\\Resource\\Ext\\RLKT"); //rlkt_EXT
	CEtResourceMng::GetInstance().AddResourcePath( szRoot + "\\Resource\\Weapon", true );
	CEtResourceMng::GetInstance().AddResourcePath( szRoot + "\\Resource\\Item", true );
	CEtResourceMng::GetInstance().AddResourcePath( szRoot + "\\Resource\\Sound", true );
	CEtResourceMng::GetInstance().AddResourcePath( szRoot + "\\Resource\\Particle" );
	CEtResourceMng::GetInstance().AddResourcePath( szRoot + "\\Resource\\HelpContent" );
	CEtResourceMng::GetInstance().AddResourcePath( szRoot + "\\Resource\\UIString" );
#ifdef PRE_ADD_RENEWUI
//	if (CGlobalValue::GetInstance().m_bReNewUI)
//		CEtResourceMng::GetInstance().AddResourcePath( szRoot + "\\Resource\\UI_New", true );
//	else
		CEtResourceMng::GetInstance().AddResourcePath( szRoot + "\\Resource\\UI", true );
#else
	CEtResourceMng::GetInstance().AddResourcePath( szRoot + "\\Resource\\UI", true );
#endif
	CEtResourceMng::GetInstance().AddResourcePath( szRoot + "\\Resource\\UITemplateTexture" );
	CEtResourceMng::GetInstance().AddResourcePath( szRoot + "\\Resource\\Effect" );
	CEtResourceMng::GetInstance().AddResourcePath( szRoot + "\\Resource\\fonts" );
#ifdef PRE_ADD_MULTILANGUAGE
	CEtResourceMng::GetInstance().AddResourcePath( szRoot + "\\Resource\\Script\\BossAI", true );
	CEtResourceMng::GetInstance().AddResourcePath( szRoot + "\\Resource\\Script\\Dark_Lair", true );
	CEtResourceMng::GetInstance().AddResourcePath( szRoot + "\\Resource\\Script\\MonsterAI", true );
	CEtResourceMng::GetInstance().AddResourcePath( szRoot + "\\Resource\\Script\\QuestNPC_Common", true );

	if( CGlobalInfo::GetInstance().m_szLanguage.empty() )
	{
		CEtResourceMng::GetInstance().AddResourcePath( szRoot + "\\Resource\\Script\\Talk_Npc", true );
		CEtResourceMng::GetInstance().AddResourcePath( szRoot + "\\Resource\\Script\\Talk_Quest", true );
	}
	else
	{
		CEtResourceMng::GetInstance().AddResourcePath( szRoot + "\\Resource\\Script\\Talk_Npc_" + CGlobalInfo::GetInstance().m_szLanguage, true );
		CEtResourceMng::GetInstance().AddResourcePath( szRoot + "\\Resource\\Script\\Talk_Quest_" + CGlobalInfo::GetInstance().m_szLanguage, true );
	}
#else // PRE_ADD_MULTILANGUAGE
	CEtResourceMng::GetInstance().AddResourcePath( szRoot + "\\Resource\\Script", true );
#endif // PRE_ADD_MULTILANGUAGE
	CEtResourceMng::GetInstance().AddResourcePath( szRoot + "\\Resource\\Dmv" );
	CEtResourceMng::GetInstance().AddResourcePath( szRoot + "\\Resource\\NPC" );
	CEtResourceMng::GetInstance().AddResourcePath( szRoot + "\\Resource\\Quest" );
	CEtResourceMng::GetInstance().AddResourcePath( szRoot + "\\Resource\\StateEffect" );
	CEtResourceMng::GetInstance().AddResourcePath( szRoot + "\\Resource\\Etc" );
	CEtResourceMng::GetInstance().AddResourcePath( szRoot + "\\Resource\\Camera" );
	CEtResourceMng::GetInstance().AddResourcePath( szRoot + "\\Resource\\Movie" );

	CEtResourceMng::GetInstance().UseMapAccessSuccessFileName( false );

	return true;
}

bool CDnMainFrame::InitializeWindow()
{
	int nWidth = CGameOption::GetInstance().m_nWidth;
	int nHeight = CGameOption::GetInstance().m_nHeight;

	DWORD dwStyle = WS_POPUP;

	if( CGameOption::GetInstance().m_bWindow )
	{
		dwStyle = WS_DRAGONNEST;
	}

	int nWindowPosX = CGameOption::GetInstance().m_nWndPosX;
	int nWindowPosY = CGameOption::GetInstance().m_nWndPosY;

	RECT rc;
	SetRect( &rc, nWindowPosX, nWindowPosY, nWindowPosX + nWidth, nWindowPosY + nHeight );

	RECT rcScreen;
	SetRect(&rcScreen, 0, 0, GetSystemMetrics(SM_CXFULLSCREEN), GetSystemMetrics(SM_CYFULLSCREEN));
	if (rc.left > rcScreen.right || rc.right < rcScreen.left || rc.top > rcScreen.bottom || rc.bottom < rcScreen.top)
	{
		rcScreen.left = GetSystemMetrics(SM_XVIRTUALSCREEN);
		rcScreen.top = GetSystemMetrics(SM_YVIRTUALSCREEN);
		rcScreen.right = rcScreen.left + GetSystemMetrics(SM_CXVIRTUALSCREEN);
		rcScreen.bottom = rcScreen.top + GetSystemMetrics(SM_CYVIRTUALSCREEN);
	}

#define WINDOW_RESET_POS_RATIO 0.7f
	bool bResetPos = false;
	if (rc.left > rcScreen.right)
	{
		bResetPos = true;
	}

	if (rc.left < rcScreen.left)
	{
		if (abs(rcScreen.left - rc.left) > nWidth * WINDOW_RESET_POS_RATIO)
			bResetPos = true;
	}

	if (rc.right > rcScreen.right)
	{
		if (abs(rc.right - rcScreen.right) > nWidth * WINDOW_RESET_POS_RATIO)
			bResetPos = true;
	}

	if (rc.bottom > rcScreen.bottom)
	{
		if (abs(rc.bottom - rcScreen.bottom) > nHeight * WINDOW_RESET_POS_RATIO)
			bResetPos = true;
	}

	if (rc.top < rcScreen.top)
		bResetPos = true;

	AdjustWindowRect( &rc, dwStyle, FALSE );

	m_bCreateWindow = false;
	m_hWnd = CreateWindow( g_szWindowClass, g_szTitle, dwStyle, bResetPos ? CW_USEDEFAULT : nWindowPosX, bResetPos ? CW_USEDEFAULT : nWindowPosY, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, m_hInstance, NULL );

	if( !m_hWnd ) return false;
	m_bCreateWindow = true;


	ShowWindow( m_hWnd, SW_SHOW);
	UpdateWindow( m_hWnd );

	SetOutputDebugFuncPtr( _OutputDebug );

	/*
	#ifdef _DEBUG
	SetOutputDebugFuncPtr( _OutputDebug );
	#else
	SetOutputDebugFuncPtr( CDnDebugTask::OutputScriptString );
	#endif
	*/
	return true;
}

void __cdecl CDnMainFrame::BadAllocFilter()
{
	if( CDnMainFrame::IsActive() ) {
		GetEtDevice()->SetDialogBoxMode( true );

		static const int ONE_K = 1024;
		static const int ONE_M = ONE_K * ONE_K;
		static const int ONE_G = ONE_K * ONE_K * ONE_K;

		MEMORYSTATUS MemInfo;
		MemInfo.dwLength = sizeof(MemInfo);
		GlobalMemoryStatus(&MemInfo);

		char szMemoryInfo[2048] = {0,};

		sprintf(szMemoryInfo,
			"%d MB physical memory.\n"
			"%d MB physical memory free.\n"
			"%d MB paging file.\n"
			"%d MB paging file free.\n"
			"%d MB user address space.\n"
			"%d MB user address space free.\n"
			"Video Memory : %.2f\n"
			"Engine : %s",
			(MemInfo.dwTotalPhys + ONE_M - 1) / ONE_M, 
			(MemInfo.dwAvailPhys + ONE_M - 1) / ONE_M, 
			(MemInfo.dwTotalPageFile + ONE_M - 1) / ONE_M, 
			(MemInfo.dwAvailPageFile + ONE_M - 1) / ONE_M, 
			(MemInfo.dwTotalVirtual + ONE_M - 1) / ONE_M, 
			(MemInfo.dwAvailVirtual + ONE_M - 1) / ONE_M,
			GetEtDevice()->GetMemoryUsePercent(),
			GetEtDevice()->GetLastErrorMsg() );

		char szStr[4096];
		sprintf_s( szStr, "Out Of Memory. (%s)\n%s", ( GetEtDevice()->IsCallOutOfMemory() == true ) ? "Engine" : "Physical", szMemoryInfo );

		MessageBoxA( CDnMainFrame::GetInstance().GetHWnd(), szStr, "Critical Error!", MB_OK );
	}
#if defined(_KR)
	// TODO : 안정화 모듈관련
	g_pServiceSetup->WriteErrorLog_(9 , L"INSUFFICIENT_MEMORY");
#endif	// #if defined(_KR)
	_exit( 0 );
}

void CDnMainFrame::DnOutOfMemoryFunc::Run()
{
	BadAllocFilter();
}

bool CDnMainFrame::PreInitializeDevice()
{
	bool bResult = true;

	// 3D Device Initialize
	SGraphicOption Option = CGameOption::GetInstance().GetGraphicOption();

#ifdef PRE_ADD_VSYNC_OFF
	CGameOption::GetInstance().m_bVSync = true;
#endif // #ifdef PRE_ADD_VSYNC_OFF

	// 엔진 시작 와이즈 로그
	g_pServiceSetup->OnEvent(0, "WiseLogEvent");
#ifdef _FINAL_BUILD
	// PIX 못돌리게 막음
	D3DPERF_SetOptions( 1 );
	bResult = EternityEngine::InitializeEngine( m_hWnd, CGameOption::GetInstance().m_nWidth, CGameOption::GetInstance().m_nHeight, CGameOption::GetInstance().m_bWindow, true, &Option, CGameOption::GetInstance().m_bVSync );
#else
	bResult = EternityEngine::InitializeEngine( m_hWnd, CGameOption::GetInstance().m_nWidth, CGameOption::GetInstance().m_nHeight, CGameOption::GetInstance().m_bWindow, true, &Option, CGameOption::GetInstance().m_bVSync );
#endif //_FINAL_BUILD
	if( bResult ) {
		GetCurRenderStack()->EnableInstancing( true );
		//GetEtWater()->Optimize( true );	// 물 옵티마이즈 일단 끈다.
		GetEtDevice()->SetOutOfMemoryCallBack( &m_DnOutOfMemoryFunc );
#ifdef PRE_FIX_CLIENT_MEMOPTIMIZE
		GetEtDevice()->SetWaitDeleteCallback( FlushWaitDelete );
#endif
		std::set_new_handler( BadAllocFilter );
#ifdef PRE_MOD_MEMORY_CHECK
#else
		CHAR szWinVer[4096] = {0,};
		CHAR szMajorMinorBuild[4096] = {0,};
		SystemInfo::GetWinVersion(szWinVer, &m_nWinVer, szMajorMinorBuild);
		if( m_nWinVer >= 106 ) {		// Vista(106), Win7(107) 은 DDraw 대신 D3D9 의 MemCheck 를 사용.
			GetEtDevice()->UseDDrawMemoyCheck( false );
		}
#endif
	}
	else {			
		MessageBox( m_hWnd, L"Unable to create d3d device.\n Please install latest video card driver.", L"Error Message", MB_OK);
		return false;
	}	

	return true;
}

bool CDnMainFrame::InitializeDevice()
{
	CGameOption::GetInstance().ApplyGraphicOption();

	EtInterface::Initialize();
#if defined(_WORK)
	CEtFontMng::GetInstance().SetForceBlur( true );
#endif
//rlkt disabled
	CEtUIIME::s_bDisableIMEonKillFocus = true;

#if defined(_WORK) || defined(_KR) || defined(_KRAZ)
#else
	CEtUIXML::s_bCheckKoreanWord = true;
#endif

#if defined(_JP)
	CEtFontMng::s_fLinePitchRate = 0.7f;
#endif

#if defined(_JP)
	CEtUIEditBox::AddInvalidCharacter( 0xFFFF, 0xFFFF );
#endif

	//rlkt 2016
	
	//rlkt may 18
	CEtUIIME::s_bProcess_GCS_COMPCLAUSE = true;

//#if defined(_US) || defined(_TH) || defined(_ID) || defined(_RU) || defined(_EU)	// Word Break 사용하는 국가는 추가할 것
	CEtFontMng::s_bUseWordBreak = false;
//#endif // _US, _TH

//#if defined(_US) || defined (_ID)
	//rlkt ENABLE
	//CEtUIIME::s_bDisableIMEonIMEControl = false;
	//CEtUIIME::s_bDisableIMEonKillFocus = false;
	CEtUIIME::_ImmAssociateContext( m_hWnd, NULL );
//#endif

#if defined(_SG)
	CEtUIIME::ConverseNativeToEng();
#endif

//	
#if defined(_CH) || defined(_TW) || defined(_SG) || defined(_JP)
	CEtFontMng::s_bUseUniscribe = false; 
#endif // _CH, _TW, _SG, _JP

#if defined(_TH)
	CEtUIEditBox::s_bCheckThaiLanguageVowelRule = true;
#endif

	CEtMaterial::LoadCompiledShaders();

#ifdef PRE_MOD_OPTION_TEXTURE_QUALITY
	GetEtDevice()->SetApplyQualityPath( "resource\\char" );
	GetEtDevice()->SetApplyQualityPath( "resource\\Item" );
	GetEtDevice()->SetApplyQualityPath( "resource\\weapon" );
	GetEtDevice()->SetApplyQualityPath( "resource\\particle" );
	GetEtDevice()->SetApplyQualityPath( "resource\\effect" );
	GetEtDevice()->SetApplyQualityPath( "resource\\stateeffect" );
	GetEtDevice()->SetApplyQualityPath( "resource\\etc" );
	GetEtDevice()->SetApplyQualityPath( "mapdata\\resource\\prop" );
#endif // PRE_MOD_OPTION_TEXTURE_QUALITY

#ifdef PRE_MOD_IGNORE_MIPMAP
	CEtTexture::AddIgnoreMipmapPath( "resource\\ui" );
	CEtTexture::AddMipmapPath( "resource\\ui\\npc\\Cross" );
	CEtTexture::AddMipmapPath( "resource\\ui\\npc\\cross" );	// 패킹하면 소문자로 검사.
	CEtTexture::AddMipmapPath( "resource\\ui\\crosshair" );
#endif

#ifdef PRE_MOD_PRELOAD_SHADER
#ifdef PRE_FIX_MATERIAL_DUMP
	if( GetEtEngine()->PreLoadShader() == false ) {
		MessageBox( m_hWnd, L"Invalid Shader Data", L"Error Message", MB_OK );
		return false;
	}
#else
	GetEtEngine()->PreLoadShader();
#endif
#endif

#ifdef PRE_MOD_OPTION_EFFECT_QUALITY
	if(CGameOption::IsActive())
		CEtBillboardEffect::SetEffectCountOption( CGameOption::GetInstance().GetCurEffectQuality() );
#else
	if(CGameOption::IsActive())
		CEtBillboardEffect::SetEffectCountOption( CGameOption::GetInstance().GetCurGraphicQuality() );
#endif

	// Input Device Initialize
	m_pInputDevice = new CInputDevice;
	if (m_pInputDevice == NULL)
		return false;

	bool bResult = m_pInputDevice->Initialize( m_hWnd, CInputDevice::KEYBOARD | CInputDevice::MOUSE | CInputDevice::JOYPAD, true );
	if( !bResult ) {
		MessageBox( m_hWnd, L"Direct Input Failed.", L"Error Message", MB_OK);
		return bResult;
	}

	InitilaizeEffect();

	// Sound Device Initialize
	m_pSoundEngine = new CEtSoundEngine;
	if (m_pSoundEngine == NULL)
		return false;

	m_pSoundEngine->Initialize( (m_nWinVer >= 106) ? CEtSoundEngine::WASAPI : CEtSoundEngine::DirectSound );
	m_pSoundEngine->CreateChannelGroup( "BGM" );
	m_pSoundEngine->CreateChannelGroup( "3D" );
	m_pSoundEngine->CreateChannelGroup( "2D" );
	m_pSoundEngine->CreateChannelGroup( "VOICE" );

#ifndef _FINAL_BUILD
	if( CGlobalValue::GetInstance().m_bSoundOff ) 
	{
		CEtSoundChannelGroup *pGroup = m_pSoundEngine->GetChannelGroup( "BGM" );
		pGroup->SetVolume( 0.f );

		pGroup = m_pSoundEngine->GetChannelGroup( "3D" );
		pGroup->SetVolume( 0.f );

		pGroup = m_pSoundEngine->GetChannelGroup( "2D" );
		pGroup->SetVolume( 0.f );

		pGroup = m_pSoundEngine->GetChannelGroup( "VOICE" );
		pGroup->SetVolume( 0.f );

		m_pSoundEngine->SetEnable(false);
	} else 
#endif //_FINAL_BUILD
	CGameOption::GetInstance().ApplySound();

	CDebugSet::Initialize( CEtResourceMng::GetInstance().GetCurrentDirectory().c_str(), true );

	// Font Initialize
	CEtFontMng::GetInstance().Initialize( CEtResourceMng::GetInstance().GetFullName("ui.fontset").c_str() );


	// Network Session Initialize
	m_pSessionMng = new CClientSessionManager;
	if (m_pSessionMng == NULL)
		return false;

	// Mouse Initialize
	m_pCursor = new CDnMouseCursor;
	if (m_pCursor == NULL)
		return false;

	m_pCursor->Create();

#ifndef _FINAL_BUILD
	CInputKeyboard* pKeyboard = static_cast<CInputKeyboard*>(m_pInputDevice->GetDeviceObject(CInputDevice::KEYBOARD));
	CInputMouse* pMouse = static_cast<CInputMouse*>(m_pInputDevice->GetDeviceObject(CInputDevice::MOUSE));
	CInputJoyPad* pJoypad = static_cast<CInputJoyPad*>(m_pInputDevice->GetDeviceObject(CInputDevice::JOYPAD));
	if (pKeyboard && pMouse && pJoypad && CGlobalValue::GetInstance().m_bEnableWinKey)
	{
		HRESULT hResult;
		hResult = pKeyboard->SetCooperativeLevel(DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
		if( FAILED( hResult ) ) assert(0);
		hResult = pMouse->SetCooperativeLevel(DISCL_NONEXCLUSIVE | DISCL_BACKGROUND);
		if( FAILED( hResult ) ) assert(0);
		hResult = pJoypad->SetCooperativeLevel(DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
		if( FAILED( hResult ) ) assert(0);
	}
#endif

	CGameOption::GetInstance().ApplyControl();
	CGameOption::GetInstance().ApplyVoiceChat();
	CGameOption::GetInstance().ApplyInputDevice();

#ifdef PRE_ADD_ACTION_DYNAMIC_OPTIMIZE
	CEtActionCoreMng::GetInstance().EnableProcessOptimize( false );
#endif

	return true;
}

bool CDnMainFrame::ReInitializeDevice()
{
	if( CGameOption::GetInstance().m_bWindow ) {
		RECT rcRect;
		if( !m_hWnd ) return true;
		if( !m_bCreateWindow ) return true;
		if( GetClientRect( m_hWnd, &rcRect ) == FALSE ) {
			return true;
		}
		if( ( rcRect.right - rcRect.left == 0 ) || ( rcRect.bottom - rcRect.top == 0 ) )
		{
			return true;
		}
		/*
		CGameOption::GetInstance().m_nWidth = rcRect.right - rcRect.left;
		CGameOption::GetInstance().m_nHeight = rcRect.bottom - rcRect.top;
		*/
		EternityEngine::ReinitializeEngine( CGameOption::GetInstance().m_nWidth, CGameOption::GetInstance().m_nHeight );
		CEtUIDialog::ProcessChangeResolution();
	}
	else {
	}

	return true;
}

bool g_bFinalizeTest = false;
bool CDnMainFrame::FinalizeDevice()
{		
#ifdef _USE_VOICECHAT
	CVoiceChatClient::Finalize();
#endif
	if( CEtFontMng::IsActive() ) CEtFontMng::GetInstance().Finalize();
	CDebugSet::Finalize();

	SAFE_DELETE( m_pInterface );
	SAFE_DELETE( m_pUIXML );
	SAFE_DELETE( m_pCursor );
	SAFE_DELETE( m_pSessionMng );

	if( m_pUISound ) 
	{
		m_pUISound->Finalize();
		SAFE_DELETE( m_pUISound );
	}

#ifdef PRE_ADD_ITEMNAME_AUTOCOMPLETE
	SAFE_DELETE( m_pNameAutoComplete );
#endif		//#ifdef PRE_ADD_ITEMNAME_AUTOCOMPLETE

	g_bFinalizeTest = true;
	if( m_pSoundEngine ) 
	{
		m_pSoundEngine->Finalize();
		SAFE_DELETE( m_pSoundEngine );
	}
#ifdef PRE_ADD_EXCEPTIONAL_MAKESTRING
	SAFE_DELETE( m_pExceptionalUIXML );
#endif // PRE_ADD_EXCEPTIONAL_MAKESTRING

	SAFE_DELETE( m_pInputDevice );
	EtInterface::Finalize();
	FinializeEffect();
#ifdef PRE_MOD_PRELOAD_SHADER
	GetEtEngine()->ReleasePreLoadShader();
#endif
	EternityEngine::FinalizeEngine();
	SAFE_DELETE( m_pResMng );

	GetEtDevice()->Clear();

	if( CDNSecure::IsActive() )
		CDNSecure::DestroyInstance();

	return true;
}

#ifndef PRE_FIX_CLIENT_MEMOPTIMIZE
UINT __stdcall PlayerPreloadThread( void *pParam )
{
	((CDnMainFrame*)pParam)->SetFinishPreload( false );
	((CDnMainFrame*)pParam)->ThreadPreloadPlayer();
	((CDnMainFrame*)pParam)->SetFinishPreload( true );
	return 0;
}
#endif

bool CDnMainFrame::ThreadInitialize()
{
	CDnTitleTask *pTitleTask = (CDnTitleTask *)CTaskManager::GetInstance().GetTask( "TitleTask" );
	if( pTitleTask ) pTitleTask->ForceAddHoundLoading( 26, 17000 );

	// DB Load
	SAFE_DELETE( m_pTableDB );
	m_pTableDB = new CDnTableDB;
	if( m_pTableDB == NULL || !m_pTableDB->Initialize() ) return false;

	if( pTitleTask ) pTitleTask->CheckPointHoundLoading( 30 );

	// UI XML String File Load
	m_pUIXML = new CEtUIXML;
	if (m_pUIXML == NULL)
		return false;

	EtInterface::xml::SetXML( m_pUIXML );

	if( pTitleTask ) pTitleTask->CheckPointHoundLoading( 33 );

#ifdef PRE_ADD_MULTILANGUAGE
	char szUIString[64] = { 0, };
#ifdef PRE_ADD_UISTRING_DIVIDE
	char szUIStringItem[64] = { 0, };
#endif // #ifdef PRE_ADD_UISTRING_DIVIDE
	char szUIStringServer[64] = { 0, };
	char szProhibitWord[64] = { 0, };

	if( !CGlobalInfo::GetInstance().m_szLanguage.empty() )
	{
		sprintf_s( szUIString, "uistring_%s.xml", CGlobalInfo::GetInstance().m_szLanguage.c_str() );
#ifdef PRE_ADD_UISTRING_DIVIDE
		sprintf_s( szUIStringItem, "uistring_item_%s.xml", CGlobalInfo::GetInstance().m_szLanguage.c_str() );
#endif // #ifdef PRE_ADD_UISTRING_DIVIDE
		sprintf_s( szUIStringServer, "uistring_server_%s.xml", CGlobalInfo::GetInstance().m_szLanguage.c_str() );
		sprintf_s( szProhibitWord, "prohibitword_%s.xml", CGlobalInfo::GetInstance().m_szLanguage.c_str() );
	}
	else
	{
		sprintf_s( szUIString, "uistring.xml" );
#ifdef PRE_ADD_UISTRING_DIVIDE
		sprintf_s( szUIStringItem, "uistring_item.xml" );
#endif // #ifdef PRE_ADD_UISTRING_DIVIDE
		sprintf_s( szUIStringServer, "uistring_server.xml" );
		sprintf_s( szProhibitWord, "prohibitword.xml" );
	}

	if( !m_pUIXML->LoadXML( szUIString, CEtUIXML::idCategory1 ) ) {
		ASSERT( false && "UIString.xml 로딩 에러");
		return false;
	}

#ifdef PRE_ADD_UISTRING_DIVIDE
	if( !m_pUIXML->LoadXML( szUIStringItem, CEtUIXML::idCategory1 ) ) {
		ASSERT( false && "UIString_Item.xml 로딩 에러");
		return false;
	}
#endif // #ifdef PRE_ADD_UISTRING_DIVIDE

	if( !m_pUIXML->LoadXML( szUIStringServer, CEtUIXML::idCategory2 ) ) {
		ASSERT( false && "UIString_server.xml 로딩 에러");
		return false;
	}

	if( !m_pUIXML->LoadFilter( szProhibitWord ) ) {
		ASSERT( false && "prohibitword.xml 로딩 에러");
		return false;
	}
#else // PRE_ADD_MULTILANGUAGE
	if( !m_pUIXML->LoadXML( "uistring.xml", CEtUIXML::idCategory1 ) ) {
		ASSERT( false && "UIString.xml 로딩 에러");
		return false;
	}

    //__debugbreak();
	//if (!m_pUIXML->LoadXML("uistring_rlkt.xml", CEtUIXML::idCategory1)) {
	//	ASSERT(false && "UIString_rlkt.xml 로딩 에러");
	//	return false;
	//}

#ifdef PRE_ADD_UISTRING_DIVIDE
	if( !m_pUIXML->LoadXML( "uistring_item.xml", CEtUIXML::idCategory1 ) ) {
		ASSERT( false && "UIString_Item.xml 로딩 에러");
		return false;
	}
#endif // #ifdef PRE_ADD_UISTRING_DIVIDE

	if( !m_pUIXML->LoadXML( "uistring_server.xml", CEtUIXML::idCategory2 ) ) {
		ASSERT( false && "UIString_server.xml 로딩 에러");
		return false;
	}

	if( !m_pUIXML->LoadFilter( "prohibitword.xml" ) ) {
		ASSERT( false && "prohibitword.xml 로딩 에러");
		return false;
	}
#endif // PRE_ADD_MULTILANGUAGE

	if( pTitleTask ) pTitleTask->CheckPointHoundLoading( 35 );

	// 파일 없을수도 있으니 없으면 그냥 패스.
	m_pUIXML->LoadWhiteList( "WhiteListWord.xml" );

	if( pTitleTask ) pTitleTask->CheckPointHoundLoading( 38 );

#ifdef PRE_ADD_EXCEPTIONAL_MAKESTRING
	m_pExceptionalUIXML = new CEtExceptionalUIXML;
	if( m_pExceptionalUIXML )
		m_pExceptionalUIXML->LoadXML( "uistring_exception.xml" );
#endif // PRE_ADD_EXCEPTIONAL_MAKESTRING

#ifdef PRE_ADD_ITEMNAME_AUTOCOMPLETE
	m_pNameAutoComplete = new CNameAutoComplete;
	m_pNameAutoComplete->LoadData((int)CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::PlayerLevelLimit ));
#endif		//#ifdef PRE_ADD_ITEMNAME_AUTOCOMPLETE

	if( pTitleTask ) pTitleTask->CheckPointHoundLoading( 43 );

	// npc 대사 & 퀘스트 정보 모두 로드
	g_DataManager.LoadData();

	if( pTitleTask ) pTitleTask->CheckPointHoundLoading( 46 );

	// UI Sound Initialize
	m_pUISound = new CDnUISound;
	if (m_pUISound == NULL)
		return false;
	m_pUISound->Initialize();

	if( pTitleTask ) pTitleTask->CheckPointHoundLoading( 49 );
	if( pTitleTask ) pTitleTask->ForceAddHoundLoading( 19, 15000 );

	m_pInterface = new CDnInterface;
	if (m_pInterface == NULL)
		return false;
	m_pInterface->PreInitialize();

	if( pTitleTask ) pTitleTask->CheckPointHoundLoading( 68 );
	if( pTitleTask ) pTitleTask->ForceAddHoundLoading( 4, 1500 );

	// Hardcoding Resource Manager
	m_pInCodeRes = new CDnInCodeResource;	
	if (m_pInCodeRes == NULL)
		return false;
	m_pInCodeRes->Initialize();

	if( pTitleTask ) pTitleTask->CheckPointHoundLoading( 73 );

	// 레코딩 UI
	m_pRecordDlg = new CDnRecordDlg( UI_TYPE_SELF );
	if (m_pRecordDlg == NULL)
		return false;
	m_pRecordDlg->Initialize( false );

	if( pTitleTask ) pTitleTask->CheckPointHoundLoading( 75 );

	CEtResource::FlushWaitDelete();
	CommonUtil::CalcJobKindMax();

	return true;
}

bool CDnMainFrame::Initialize()
{
	m_pTaskMng = &CTaskManager::GetInstance();
	if (m_pTaskMng == NULL)
		return false;

	// DeadLock Detect Task 생성
	DWORD dwDetectTime = 60000 * 10;
#if defined( PRE_FIX_CLIENT_FREEZING )
	dwDetectTime = 60000 * 2;
#endif

	m_pHangCrashTask = new CDnHangCrashTask( dwDetectTime ); // 5분동안 동작없으면 크래쉬
	if (m_pHangCrashTask == NULL)
		return false;
	m_pTaskMng->AddTask( m_pHangCrashTask, "HangCrashTask", -1, true );

	// Bridge Task 생성
	m_pBridgeTask = new CDnBridgeTask;
	if (m_pBridgeTask == NULL)
		return false;
	m_pTaskMng->AddTask( m_pBridgeTask, "BridgeTask", -1 );

	m_pAuthTask = new CDnAuthTask;
	if (m_pAuthTask == NULL)
		return false;
	m_pTaskMng->AddTask( m_pAuthTask, "AuthTask", -1 );

	// Loading Task 생성
	m_pLoadingTask = new CDnLoadingTask;
	if (m_pLoadingTask == NULL)
		return false;
	m_pLoadingTask->Initialize( NULL );
	CTaskManager::GetInstance().AddTask( m_pLoadingTask, "LoadingTask", -1, true );

	CDnTitleTask *pTitleTask = new CDnTitleTask;
	if( pTitleTask == NULL || pTitleTask->Initialize() == false ) {
		MessageBox( m_hWnd, _T("'Title Task' Initialize Failed"), _T("Error"), MB_OK );
		SAFE_DELETE( pTitleTask );
		return false;
	}
	m_pTaskMng->AddTask( pTitleTask, "TitleTask", -1, false );

#ifdef PRE_ADD_DWC
	// DWC Task 생성
	m_pDWCTask = new CDnDWCTask;
	if( m_pDWCTask == NULL )
		return false;	
	m_pTaskMng->AddTask( m_pDWCTask, "DWCTask", -1 );
#endif // PRE_ADD_DWC

	return true;
}

bool CDnMainFrame::Finalize()
{
#ifndef PRE_FIX_CLIENT_MEMOPTIMIZE
	while( !IsFinishPreloadPlayer() ) { Sleep(1); }

	for( int i=0; i<10; i++ ) {
		SAFE_RELEASE_SPTR( m_hPreLoadPlayerClass[i] );
	}
#endif
	CDnActor::ReleaseClass();

	SAFE_DELETE( m_pRecordDlg );
	SAFE_DELETE( m_pCaptureEngine );
	// 디버그 타스크는 NEw 에서 넣은게 아니니까 RemoveAllTask 하기전에 빼준다.
	if( CDnLoadingTask::IsActive() ) CDnLoadingTask::GetInstance().EnableBackgroundLoading( false );
	if( m_pTaskMng ) {
		m_pTaskMng->RemoveTask( "DebugTask" );
		m_pTaskMng->RemoveAllTask();
	}

	SAFE_DELETE( m_pInCodeRes );
	SAFE_DELETE( m_pTableDB );
	CDnWorld::GetInstance().Destroy();
	CEtActionCoreMng::GetInstance().Finalize();

	DeleteFile( _T( "Config.ini_" ) );

	return true;
}

int g_nTestSoundIndex = 0;
UINT __stdcall TestThread( void *pParam )
{
	while(1) {
		if( CTaskManager::GetInstance().GetTask( "VillageTask" ) ) break;
		Sleep(1);
	}

	while(1) {
		if( g_bFinalizeTest ) break;
		g_nTestSoundIndex = CEtSoundEngine::GetInstance().LoadSound( "Bat_Fly_a.wav", true, false );
		if( g_bFinalizeTest ) break;
		for( int i=0; i<rand()%10000; i++ ) { int asdf = 0; asdf++; }
		if( g_bFinalizeTest ) break;
		CEtSoundEngine::GetInstance().RemoveSound( g_nTestSoundIndex );
		if( g_bFinalizeTest ) break;
		g_nTestSoundIndex = 0;
	}
	return 0;
}

UINT __stdcall TestThread2( void *pParam )
{
	while(1) {
		if( CTaskManager::GetInstance().GetTask( "VillageTask" ) ) break;
		Sleep(1);
	}

	while(1) {
		if( g_nTestSoundIndex ) {
			if( g_bFinalizeTest ) break;
			EtSoundChannelHandle hChannel = CEtSoundEngine::GetInstance().PlaySound__( "3D", g_nTestSoundIndex, false, true );
			if( g_bFinalizeTest ) break;

			if( hChannel ) {
				int nCount = rand()%100;
				for( int i=0; i<20 + nCount; i++ ) {
					if( g_bFinalizeTest ) break;
					hChannel->SetVolume( 1.f );
					if( g_bFinalizeTest ) break;
					hChannel->SetPosition( *CDnActor::s_hLocalActor->GetPosition() );
					if( g_bFinalizeTest ) break;
					hChannel->SetRollOff( 3, 0.f, 1.f, 1000.f * 0.3f, 1.f, 1000.f, 0.f );
					if( g_bFinalizeTest ) break;
					if( hChannel->IsPause() ) hChannel->Resume();
					if( g_bFinalizeTest ) break;
				}
			}
		}
		else for( int i=0; i<rand()%1000; i++ ) { int asdf = 0; asdf++; }
	}
	return 0;
}

bool CDnMainFrame::Execute()
{
	MSG msg;
//	unsigned TestThreadID;
//	unsigned TestThreadID2;
//	_beginthreadex( NULL, 65536, TestThread, NULL, 0, &TestThreadID );
//	_beginthreadex( NULL, 65536, TestThread2, NULL, 0, &TestThreadID2 );
	while(1)
	{
		BOOL bResult = PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE );
        if (msg.message == WM_QUIT) {
            break;
        }

		// _KR 국내빌드에선 조합 및 캔디데이트를 직접 렌더링하니 이렇게 처리.
		//if( msg.message == WM_PAINT ) bResult = false;
		if( bResult ) 
		{
			if(msg.message >= WM_KEYFIRST && msg.message <= WM_KEYLAST)
			{
				char pszClassName[256];
				GetClassNameA(msg.hwnd, pszClassName, 255);
				if(strcmp(pszClassName, "Internet Explorer_Server") == 0)
				{
#if defined(_CH) && defined(_AUTH)
					bool bSend = true;
					if( CDnSDOAService::IsActive() && CDnSDOAService::GetInstance().IsFocus() )
						bSend = false;
					if( bSend )
#endif // _CH
					::SendMessage(GetHWnd(), msg.message, msg.wParam, msg.lParam);
				}
			}

			if( TranslateAccelerator( msg.hwnd, NULL, &msg ) == 0 ) 
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			CDnMouseCursor::GetInstance().UpdateCursor();
			// Network Process
			CClientSessionManager::GetInstance().Process();
			// Timer
			CSyncTimer::GetInstance().Process();

			// Process Task
            if (m_pTaskMng->Execute() == false) {
                break;
            }

            if (m_bDestroy) {
                break;
            }
		}

		if( m_bBeginCaption ) {
			if (IsWindow(g_hwndChargeBrowser) == false)
			{
				POINT p;
				GetCursorPos( &p );

				RECT Rect;
				GetWindowRect( msg.hwnd, &Rect );

				POINT pr;
				pr.x = m_MousePos.x - p.x;
				pr.y = m_MousePos.y - p.y;
				//			if( m_WindowPos.x - pr.x > 0 || 
				OutputDebug( "Window Pos : %d, %d  CursorPos : %d, %d, MousePos : %d, %d\n", m_WindowPos.x, m_WindowPos.y, p.x, p.y, m_MousePos.x, m_MousePos.y );
				MoveWindow( msg.hwnd, m_WindowPos.x - pr.x, m_WindowPos.y - pr.y, Rect.right - Rect.left, Rect.bottom - Rect.top, TRUE );
			}
		}

		if( GetAsyncKeyState( VK_LBUTTON ) == 0 ) {
			m_bBeginCaption = false;
		}

#ifdef PRE_ADD_STEAMWORKS
		if( CGlobalInfo::GetInstance().m_bUseSteamworks )
			SteamAPI_RunCallbacks();	// Run Steam client callbacks
#endif // PRE_ADD_STEAMWORKS
	}
	return true;
}

bool CDnMainFrame::IMEProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	bool bRet = false;
	if( message == WM_IME_COMPOSITION )
	{
		CEtUIIME::s_bRESULTSTR_NotSendComp = true;
		GetEtDevice()->SetDialogBoxMode( true );
	}
	else if( message == WM_IME_NOTIFY && GetEtDevice() && CDnInterface::IsActive() )
	{
		// 포커스가 채팅창에 가있지않다면 디폴트프로시저에도 보내지 않는다.
		bool bReturn = true;
		if( GetInterface().IsFocusEditBox() )
			bReturn = false;
#if defined(_CH) && defined(_AUTH)
		if( CDnSDOAService::IsActive() && CDnSDOAService::GetInstance().IsFocus() )
			bReturn = false;
#endif
		if( !bReturn && wParam == IMN_OPENCANDIDATE )
		{
			float fX1 = CEtUIIMEEditBox::s_CurrentCaretPos.fX * DEFAULT_UI_SCREEN_WIDTH;
			float fX2 = CEtUIIMEEditBox::s_CurrentCaretPos.fY * DEFAULT_UI_SCREEN_HEIGHT;

			// 이동시킨 창모드부터 풀스크린까지 전부 고려해야한다.
			RECT rcWindow;
			GetWindowRect( GetHWnd(), &rcWindow );

			POINT pt;
			pt.x = (long)fX1 - rcWindow.left;
			pt.y = (long)fX2 - rcWindow.top;

			// 풀스크린때는 윈도우 캡션을 고려하지 않고 그 자리에 렌더링해 원래 입력창이 가려진다.
			// 그래서 강제로 20픽셀 내리도록 하겠다.
			if( CGameOption::GetInstance().m_bWindow == false )
				pt.y += 20;

			// 만약 조합윈도우가 열려있다면 위치가 겹쳐서 가려버리게 될 것이다.
			// 조합윈도우가 열려있다면 x축으로도 밀어놓겠다.
			if( CEtUIIME::s_bRESULTSTR_NotSendComp )
				pt.x += 50;

			ClientToScreen(hWnd, &pt);

			CANDIDATEFORM form;
			HIMC hIMC;
			if( ( hIMC = ImmGetContext( hWnd ) ) )
			{
				form.dwIndex = 0;

				// 컴퓨터마다 필요 인자가 다른 듯 합니다...
				form.dwStyle = CFS_FORCE_POSITION | CFS_CANDIDATEPOS;

				// 다음 두 줄은 원하는 Candidiate Window의 좌표입니다.
				form.ptCurrentPos.x = pt.x;
				form.ptCurrentPos.y = pt.y;

				// 계속 테스트 해보니 캔디데이트를 직접 움직이는 것보다 컴포지션 윈도우를 이동시켜 처리하는게
				// 창 위치 잡는데 있어서 더 좋은 거 같다.
				// 만약 나중에 다시 이 작업을 하게 된다면 컴포지션 윈도우의 위치를 적절하게 잡고 나머지는 알아서 API에 맡기는게 나을 거 같다.
				//ImmSetCandidateWindow(hIMC, &form);
				//ImmReleaseContext(hWnd, hIMC);
			}
			GetEtDevice()->SetDialogBoxMode( true );
		}
		else if( wParam == IMN_CLOSECANDIDATE )
		{
			GetEtDevice()->SetDialogBoxMode( false );
		}
		else if( wParam == IMN_SETCONVERSIONMODE || wParam == IMN_SETOPENSTATUS )
		{
			CEtUIIME::CheckToggleState();
		}

		bRet = true;
	}

	return bRet;
}

#ifndef _FINAL_BUILD
void CDnMainFrame::TemporaryKeyProc()
{
#ifdef PRE_ADD_NPC_REPUTATION_SYSTEM
	//if( GetAsyncKeyState( VK_F9) ) 
	//	GetInterface().GetMainMenuDialog()->ShowNpcReputationDlg( true );
#endif // #ifdef PRE_ADD_NPC_REPUTATION_SYSTEM

	if( CGlobalValue::GetInstance().IsComputerName("kwcsc-pc") )
	{
		if( GetAsyncKeyState( VK_F1 ) )
		{
		}
		else if( GetAsyncKeyState( VK_F3 ) )
		{
		}
		else if( GetAsyncKeyState( VK_F4 ) )
		{
		}
		return;
	}

#if defined(TEST_CUTSCENE)
	static CDnGameTask* pGameTaskTemp = NULL;
	static CDnVillageTask* pVillageTaskTemp = NULL;
	static CDnCutSceneTask* pCutSceneTask = NULL;

	// 컷 신 플레이 테스트, 한기가 추가. 
	if( CGlobalValue::GetInstance().IsComputerName("jhk8211-pc") ) 
	{
		if( GetAsyncKeyState( VK_F11 ) )
		{
			CTaskManager* pTaskManager = CTaskManager::GetInstancePtr();

			// 임의로 패킷을 만들어서 보내줌..
			CDnCommonTask* pCommonTask = static_cast<CDnCommonTask*>(pTaskManager->GetTask( "CommonTask" ));
			SCPlayCutScene MockCutScenePlayPacket;
			MockCutScenePlayPacket.nCutSceneTableID = 60;
			pCommonTask->OnRecvQuestPlayCutScene( &MockCutScenePlayPacket );

			//pCutSceneTask = new CDnCutSceneTask;
			////pCutSceneTask->Initialize( "client_test.dmv" );
			//pCutSceneTask->Initialize( "R:/GameRes/Resource/Dmv/Q_ManaRidge_DestinyGirl_01.dmv" );

			//if( CDnWorld::MapTypeEnum::MapTypeVillage != CDnWorld::GetInstance().GetMapType() )
			//{
			//	pGameTaskTemp = static_cast<CDnGameTask*>(pTaskManager->GetTask( "GameTask" ));
			//	pTaskManager->RemoveTask( "GameTask" );
			//}
			//else
			//{
			//	pVillageTaskTemp = static_cast<CDnVillageTask*>(pTaskManager->GetTask( "VillageTask" ));
			//	pTaskManager->RemoveTask( "VillageTask" );
			//}

			//pCutSceneTask->EnableTaskProcess( true );
			//pCutSceneTask->SetTaskName( "CutSceneTask" );
			//pTaskManager->AddTask( pCutSceneTask, "CutSceneTask", -1, false );

			// 플레이 시작!
			//pNewCutSceneTask->StartPlay( LocalTime );

			//int iNumTask = pTaskManager->GetTaskCount();
			//CDnVillageTask pTaskManager->GetTask( "VillageTask" );
			//for( int iTask = 0; iTask < iNumTask; ++iTask )
			//{
			//	pTaskManager->G
			//}
		}
		else
		if( GetAsyncKeyState( VK_F9 ) )
		{
			CTaskManager* pTaskManager = CTaskManager::GetInstancePtr();
			pCutSceneTask->Finalize();
			pTaskManager->RemoveTask( "CutSceneTask" );
			SAFE_DELETE( pCutSceneTask );
			//pGameTaskTemp->Enable

			if( CDnWorld::MapTypeEnum::MapTypeVillage != CDnWorld::GetInstance().GetMapType() )
				pTaskManager->AddTask( pGameTaskTemp, "GameTask", -1, false );
			else
				pTaskManager->AddTask( pVillageTaskTemp, "VillageTask", -1, false );
		}
	}
#endif // TEST_CUTSCENE

#ifdef _TEST_CODE_KAL
	if( GetAsyncKeyState( VK_F7 ) )
	{
		CDnInterface::GetInstance().OpenItemUpgradeExchangeDialog();
	}

	if( GetAsyncKeyState( VK_F8 ) )
	{
		GetInterface().ProgressSimpleMsgBox(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 3450), 5.f, true, UI_DISABLE_DIALOG, NULL); // UISTRING : 파티 재접속중...
	}
#endif // _TEST_CODE_KAL

#ifdef PRE_ADD_GACHA_JAPAN
	//if( CGlobalValue::GetInstance().IsComputerName("jhk8211-pc") )
	//{
		//if( GetAsyncKeyState( VK_F9 ) )
		//{
		//	GetInterface().ShowGachaDialog( 1 );
		//}
	//}
#endif // PRE_ADD_GACHA_JAPAN

#ifdef _TEST_CODE_KAL
#else
	// F8 키 누르면 포커스 이동 가능 
	if( GetAsyncKeyState( VK_F8 ) )
		CDnMouseCursor::GetInstance().ShowCursor( !CDnMouseCursor::GetInstance().IsShowCursor(), true );
#endif

	if( GetAsyncKeyState( VK_F9 ) )
	{
		/*
		CEtResource::DeleteAllObject();
		CEtResource::FlushWaitDelete();
		Sleep(10000000);
		*/
		FILE *fp;
		char szBuf[512];
		fopen_s( &fp, "c:\\soundresult.txt", "wt" );
		for( int i=0; i<CEtSoundEngine::GetInstance().GetSoundStructCount(); i++ ) {
			CEtSoundEngine::SoundStruct *pStruct = CEtSoundEngine::GetInstance().GetSoundStruct(i);
			if( !pStruct ) continue;
			if( pStruct->Type != FMOD_SOUND_TYPE_OGGVORBIS ) continue;

			sprintf_s( szBuf, "FileName : %s, RefCount : %d\n", pStruct->szFileName.c_str(), pStruct->nRefCount );
			fprintf_s( fp, szBuf );
		}
		fclose(fp);
		GetEtDevice()->EvictManagedResources();
		/*
		static int nCount = 0;
		if( CDnWorld::IsActive() ) {
			switch( nCount ) {
				case 2:
					SendChatMsg( CHATTYPE_NORMAL, L"/go 11" );
					break;
				case 0:
					SendChatMsg( CHATTYPE_NORMAL, L"/go 14031 4" );
					break;
				case 1:
					SendChatMsg( CHATTYPE_NORMAL, L"/go 14032 4" );
					break;
			}
			nCount++;
			if( nCount == 3 ) nCount = 0;
		}
		*/
//		GetInterface().GetMainMenuDialog()->ShowTimeEventDialog( true );
		//GetInterface().GetMainMenuDialog()->OpenItemCompound2Dialog( 5001 );
		/*
		static int nDistMin = 1;
		static int nDistMax = 10;
		static int nDistFix = 0;
		
		CDnPlayerActor *pPlayer = (CDnPlayerActor *)CDnActor::s_hLocalActor.GetPointer();

		float fFrame = 0.f;
		float fPrevFrame = 0.f;
		EtVector3 vDist;
		EtVector3 vTemp;
		pPlayer->SetAction( "Stand", 0.f, 0.f );
		CDnActionBase::ActionElementStruct *pStruct = pPlayer->GetElement( "Stand" );
		int nAniIndex = pPlayer->GetAniIndex( pStruct->szLinkAniName.c_str() );
		for( int i=0; ; i++ ) {
			float fTime = 0.f;
			if( nDistFix == 0 ) fTime = ( nDistMin + (rand()%( nDistMax - nDistMin )) ) / 10.f;
			else fTime = nDistFix / 10.f;
			fFrame += fTime;
			if( fFrame >= 50.f ) {
				pPlayer->CalcAniDistance( nAniIndex, 50.f, fPrevFrame, vTemp );
				vDist += vTemp;
				break;
			}
			pPlayer->CalcAniDistance( nAniIndex, fFrame, fPrevFrame, vTemp );
			vDist += vTemp;

			fPrevFrame = fFrame;
		}
		OutputDebug( "Result = %.2f, %.2f\n", vDist.x, vDist.z );
		*/
		/*
		DNVector(DnDropItemHandle) hVecList;
		CDnDropItem::ScanItem( *CDnActor::s_hLocalActor->GetPosition(), 100000.f, hVecList );
		if( !hVecList.empty() ) {
			for( int h=0; h<10; h++ ) {
				int nIndex = rand()%(int)hVecList.size();
				BYTE pBuffer[128];
				CPacketCompressStream Stream( pBuffer, 128 );

				DWORD dwUniqueID = hVecList[nIndex]->GetUniqueID();
				int nSignalIndex = h;
				Stream.Write( &dwUniqueID, sizeof(DWORD) );
				Stream.Write( &nSignalIndex, sizeof(int), CPacketCompressStream::INTEGER_CHAR );
				Stream.Write( hVecList[nIndex]->GetPosition(), sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );

				EtVector3 vPos = *CDnActor::s_hLocalActor->GetPosition();
				CDnActor::s_hLocalActor->GetMatEx()->m_vPosition = *hVecList[nIndex]->GetPosition();
				((CDnLocalPlayerActor*)CDnActor::s_hLocalActor.GetPointer())->CmdStop( "PickUp" );
				((CDnLocalPlayerActor*)CDnActor::s_hLocalActor.GetPointer())->Send( eActor::SC_CMDPICKUPITEM, &Stream );
				CDnActor::s_hLocalActor->GetMatEx()->m_vPosition = vPos;
			}
		}
		*/
		/*
		DNVector(DnActorHandle) hVecList;
		CDnActor::ScanActor( *CDnActor::s_hLocalActor->GetPosition(), 100000.f, hVecList );
		static bool bFlag = false;
		if( !hVecList.empty() ) {
			int nIndex = rand()%(int)hVecList.size();

			if(bFlag == false) {
				EtVector3 vPos = *CDnActor::s_hLocalActor->GetPosition();
//							CDnActor::s_hLocalActor->GetMatEx()->m_vPosition = *hVecList[nIndex]->GetPosition();
//							CDnActor::s_hLocalActor->GetMatEx()->m_vPosition -= CDnActor::s_hLocalActor->GetMatEx()->m_vZAxis * 100.f;
				((CDnLocalPlayerActor*)CDnActor::s_hLocalActor.GetPointer())->CmdAction( "Attack1" );
				bFlag = true;
			}
			else {
				EtVector3 vGaraPos = EtVector3( 1000000.f, 1000000.f, 1000000.f );
				BYTE pBuffer[128] = { 0, };
				CPacketCompressStream Stream( pBuffer, 128 );

				DWORD dwGap = 0;
				bool bResetStop = false;
				Stream.Write( &dwGap, sizeof(DWORD) );
				Stream.Write( &vGaraPos, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
				Stream.Write( &bResetStop, sizeof(bool) );

				CDnActor::s_hLocalActor->Send( eActor::CS_CMDSTOP, &Stream );
				bFlag = false;
			}
		}
		*/
		if(CDnActor::s_hLocalActor)
		{
			EtVector3 vPos = *CDnActor::s_hLocalActor->GetPosition();
			vPos.y = 100000.f;
			BYTE pBuffer[128] = { 0, };
			CPacketCompressStream Stream( pBuffer, 128 );

			DWORD dwGap = ((CDnLocalPlayerActor*)CDnActor::s_hLocalActor.GetPointer())->GetSyncDatumGap();
			bool bResetStop = false;
			Stream.Write( &dwGap, sizeof(DWORD) );
			Stream.Write( &vPos, sizeof(EtVector3), CPacketCompressStream::VECTOR3_BIT );
			Stream.Write( &bResetStop, sizeof(bool) );

			CDnActor::s_hLocalActor->Send( eActor::CS_CMDSTOP, &Stream );
		}
	}

	if( GetAsyncKeyState( VK_MENU ) < 0 && // CTRL+ALT+D  -> DebugBreak	// 실행도중 갑자기 브레이크 걸고 싶을때 사용.
		GetAsyncKeyState( VK_CONTROL ) < 0 && 
		GetAsyncKeyState( 'D' ) < 0 ) {
			DebugBreak();
	}

	if( GetAsyncKeyState( VK_CONTROL ) < 0 && // Ctrl + '~'
		GetAsyncKeyState( VK_OEM_3 ) < 0 ) 
	{
		CGlobalValue::GetInstance().m_bUseLogWnd = !CGlobalValue::GetInstance().m_bUseLogWnd;

		if ( CGlobalValue::GetInstance().m_bUseLogWnd ) 
		{
			LogWnd::CreateLog(false,LogWnd::LOG_TARGET_WINDOW/*|LogWnd::LOG_TARGET_FILE*/);
			LogWnd::SetCommandCallBack(DebugCmdProc);
		}
		else
		{
			LogWnd::DestroyLog();
		}
		return; 

		static DWORD s_dwDebugTaskHandle = -1;
		static CDnDebugTask s_DebugTask;

		if( m_pTaskMng->GetTask( s_dwDebugTaskHandle ) == NULL ) {
			s_dwDebugTaskHandle = m_pTaskMng->AddTask( &s_DebugTask, "DebugTask", -1 );
			s_DebugTask.Show( true );
		}
		else {
			m_pTaskMng->RemoveTask( s_dwDebugTaskHandle );
			s_DebugTask.Show( false );
		}
	}

	if( GetAsyncKeyState( VK_CONTROL ) < 0 && // Ctrl + '~'
		GetAsyncKeyState( VK_OEM_PLUS ) < 0 )
	{
		if( CDnActor::s_hLocalActor ) {
			CDnActor::s_hLocalActor->SetMoveSpeed(1200);

			CDnPlayerActor *pPlayer = dynamic_cast<CDnPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
			if(pPlayer && pPlayer->IsVehicleMode() && pPlayer->GetMyVehicleActor() )
				pPlayer->GetMyVehicleActor()->SetMoveSpeed(1500);

		}
	}

	if( GetAsyncKeyState( VK_PAUSE ) )
		g_bPauseMode = !g_bPauseMode;

	if( CGlobalValue::GetInstance().IsComputerName("realgaia-pc") ) {
		if( GetAsyncKeyState( VK_F9) ) {
			GetInterface().GetMainMenuDialog()->OpenMarketDialog();
		}
		if( GetAsyncKeyState( VK_F8) ) {
			if( CDnActor::s_hLocalActor  ) {
				EtVector3 vTargetPos( 500, 0, -500);
				CDnActor::s_hLocalActor->MoveTargetNavi( vTargetPos, 10.0f, "Move_Front" );
				CDnActor::s_hLocalActor->SetActionQueue( "Move_Front", -1, 0.0f, 0.f, true );
			}
		}
		if( GetAsyncKeyState( VK_F7) ) {
			if( CDnActor::s_hLocalActor ) {
				CDnPlayerActor *pPlayer = dynamic_cast<CDnPlayerActor *>(CDnActor::s_hLocalActor.GetPointer());
				GetInterface().OpenDirectDialog( CDnActor::s_hLocalActor->GetUniqueID(), CDnActor::s_hLocalActor->GetLevel(), 
					pPlayer->GetJobName(), CDnActor::s_hLocalActor->GetName() );
			}
		}
		if( GetAsyncKeyState( VK_F6) ) {
			CDnActor::s_hLocalActor->SetPosition( EtVector3(0,0,0) );
			CDnActor::s_hLocalActor->Look( EtVector2(0,1) );

			MatrixEx Cross;						
			CDnPlayerCamera *pCamera = (CDnPlayerCamera*)CDnCamera::FindCamera( CDnCamera::PlayerCamera ).GetPointer();
			pCamera->AttachActor( CDnActor::s_hLocalActor );
			*pCamera->GetMatEx() = Cross;
			pCamera->ResetCamera();
		}
		if( GetAsyncKeyState( VK_F4) ) {
			CDnActor::s_hLocalActor->SetPosition( EtVector3(0,1000,0) );
			CDnActor::s_hLocalActor->Look( EtVector2(0,1) );

			MatrixEx Cross;
			CDnPlayerCamera *pCamera = (CDnPlayerCamera*)CDnCamera::FindCamera( CDnCamera::PlayerCamera ).GetPointer();
			pCamera->DetachActor();
			EtMatrixRotationX( *pCamera->GetMatEx(), -ET_PI*0.5f);						
			pCamera->ResetCamera();
			pCamera->GetMatEx()->SetPosition(EtVector3(0,5000,0));
		}
		static DnActorHandle hActor;
		if( CDnActor::s_hLocalActor && hActor != CDnActor::s_hLocalActor ) {
			//CDnActor::s_hLocalActor->SetMoveSpeed(1200);
			hActor = CDnActor::s_hLocalActor;
		}
	}
}
#endif // _FINAL_BUILD

LRESULT CDnMainFrame::WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
// 이제는 JP도 윈도우 내장 IME를 사용하기때문에 이 방식으로 처리한다.
#if defined(_CH) || defined(_JP) || defined(_TW)
	if( IMEProc( hWnd, message, wParam, lParam ) )
		return DefWindowProc( hWnd, message, wParam, lParam );
#endif // #if defined(_CH) || defined(_JP)

	int wmId, wmEvent;

	// 아래 변수처리가 없으면 윈도우 클라이언트 영역을 누른채 타이틀로 마우스무브시 윈도가 이동되게 된다.
	static bool s_bLButtonPressed = false;

	if( m_pTaskMng ) m_pTaskMng->WndProc( hWnd, message, wParam, lParam );
#ifdef PRE_MOD_BROWSER
	if( CDnInterface::GetInstancePtr() )
		GetInterface().OnWndProcForInternetBrowser(hWnd, message, wParam, lParam);
#endif

	switch( message )
	{
	case WM_CREATE:
		break;
	case WM_GETMINMAXINFO:
		{
			int nWidth = CGameOption::GetInstance().m_nWidth;
			int nHeight = CGameOption::GetInstance().m_nHeight;

			DWORD dwStyle = WS_POPUP;

			if( CGameOption::GetInstance().m_bWindow )
			{
				dwStyle = WS_DRAGONNEST;
			}

			RECT rc;
			SetRect( &rc, 0, 0, nWidth, nHeight );
			AdjustWindowRect( &rc, dwStyle, FALSE );

			POINT p;
			p.x = rc.right - rc.left;;
			p.y = rc.bottom - rc.top;
			((MINMAXINFO*)lParam)->ptMaxTrackSize = p;
			((MINMAXINFO*)lParam)->ptMinTrackSize = p;
			((MINMAXINFO*)lParam)->ptMaxSize = p;
		}
		return FALSE;
	case WM_ERASEBKGND:
		if( GetEtDevice()->GetDevicePtr() ) return 1;
		break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// 메뉴의 선택 영역을 구문 분석합니다.
		switch (wmId)
		{
		case IDM_EXIT:
			DestroyWindow( hWnd );
			break;
		default:
			return DefWindowProc( hWnd, message, wParam, lParam );
		}
		break;
/*
		case WM_SETFOCUS:
		if( CInputDevice::IsActive() ) {
		CInputDevice::GetInstance().ResetAllInput();
		CDnMouseCursor::GetInstance().ShowCursor( false, false );
		}
		break;
*/
	case WM_KILLFOCUS:
		s_bLButtonPressed = false;
		m_bBeginCaption = false;
		if( CInputDevice::IsActive() ) {
			CInputDevice::GetInstance().ResetAllInput();
			if( CDnMouseCursor::GetInstancePtr() ) {
				CDnMouseCursor::GetInstance().ShowCursor( true, true );
			}
		}
		break;
#ifdef _FINAL_BUILD
	case WM_SYSKEYDOWN:
		switch (wParam)
		{
		case VK_F4:
			return 0;
		}
		break;
#endif
	case WM_CLOSE:
		if( m_pTaskMng ) {
			if( m_pTaskMng->OnCloseTask() ) return 0;
		}
		break;
	case WM_DESTROY:
		{
			PostQuitMessage(0);
		}
		break;
	case WM_ACTIVATEAPP:
		if( wParam == FALSE ) {
			if( CGameOption::IsActive() && CGameOption::GetInstance().m_bWindow == false && GetEtDevice() ) {
				GetEtDevice()->SetClipCursor( false );

				s_bLButtonPressed = false;
				m_bBeginCaption = false;
				if( CInputDevice::IsActive() ) {
					CInputDevice::GetInstance().ResetAllInput();
					if( CDnMouseCursor::GetInstancePtr() ) {
						CDnMouseCursor::GetInstance().ShowCursor( true, true );
					}
				}
			}
		}
		break;
	case WM_ACTIVATE:
		{
			if (IsWindow(g_hwndChargeBrowser))
				SetFocus(CDnMainFrame::GetHWnd());
		}
		break;
	case WM_KEYUP:
		{
			if( wParam == VK_SNAPSHOT ) {
				if( !(GetKeyState( VK_LCONTROL )&0x80) )
					SaveScreenShotToFile( (D3DXIMAGE_FILEFORMAT)CGameOption::GetInstance().m_nScreenShotFormat );
			}
		}
		break;
	case WM_KEYDOWN:
	{
#ifndef _FINAL_BUILD
					   TemporaryKeyProc();
#endif // _FINAL_BUILD

			//RLKT_DEBUG!!!	
			if (CGlobalValue::GetInstance().IsComputerName("K-PC"))
			{
				if (GetAsyncKeyState(VK_F7))
				{
					GetInterface().ShowJobChangeUI();
				}

				if (GetAsyncKeyState(VK_F8))
				{
					//GetInterface().ShowInstantSpecializationBtn(true);
				}
			}
			//

			// UI 없는 상태에서 패드로 퀵슬롯 처리하는 부분은 어쩔수 없이 이렇게 한다.
			// GetKeyState값 처리하는 과정에서 패드는 직접 키를 입력하고 있는게 아니라서 처리가 안되기 때문이다.
			if( CDnInterface::GetInstancePtr() )
				GetInterface().JoypadPostProcess();
		
			if( wParam == VK_SCROLL ) {
				ToggleRecord();
			}

			if( GetAsyncKeyState( VK_MENU ) < 0 && // CTRL+ALT+0  -> QA 전용 농장 구역보여줌
				GetAsyncKeyState( VK_CONTROL ) < 0 && 
				GetAsyncKeyState( VK_UP ) < 0 ) 
			{
				if( CDnInterface::GetInstancePtr() )
				{
#if defined(_WORK)	// verygoodd21 전용 치트키
					CDnLifeChannelDlg * pLifeChannelDlg = GetInterface().GetLifeChannelDlg();		
					if( pLifeChannelDlg )
						pLifeChannelDlg->ToggleQAZone();
#endif	// #if defined(_WORK)
				}
			}

#if defined(_WORK)
			if( GetAsyncKeyState( VK_TAB ) < 0 )
			{
				if( GetAsyncKeyState( VK_UP ) < 0 )
				{
					float fBlurWeight = CEtFontMng::GetInstance().GetForceBlurWeight();
					fBlurWeight += 0.05f;
					if( fBlurWeight > 1.f )
						fBlurWeight = 1.f;

					CEtFontMng::GetInstance().SetForceBlurWeight( fBlurWeight );
					CEtFontMng::GetInstance().FlushFontCache();

					WCHAR wszString[256] = {0,};
					swprintf_s( wszString, _countof( wszString ), L"현재 Blur Weight : %1f", fBlurWeight );
					GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", wszString );
				}
				else if( GetAsyncKeyState( VK_DOWN ) < 0 )
				{
					float fBlurWeight = CEtFontMng::GetInstance().GetForceBlurWeight();
					fBlurWeight -= 0.05f;
					if( fBlurWeight < 0.f )
						fBlurWeight = 0.f;

					CEtFontMng::GetInstance().SetForceBlurWeight( fBlurWeight );
					CEtFontMng::GetInstance().FlushFontCache();

					WCHAR wszString[256] = {0,};
					swprintf_s( wszString, _countof( wszString ), L"현재 Blur Weight : %1f", fBlurWeight );
					GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", wszString );
				}
				else if( GetAsyncKeyState( VK_LEFT ) < 0 )
				{
					int nMinimizeSize = CEtFontMng::GetInstance().GetForceMinimizeSize();
					nMinimizeSize -= 1;
					if( nMinimizeSize < 1 )
						nMinimizeSize = 1;

					CEtFontMng::GetInstance().SetForceMinimizeSize( nMinimizeSize );
					CEtFontMng::GetInstance().FlushFontCache();

					WCHAR wszString[256] = {0,};
					swprintf_s( wszString, _countof( wszString ), L"현재 적용 폰트크기 : %d", nMinimizeSize );
					GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", wszString );
				}
				else if( GetAsyncKeyState( VK_RIGHT ) < 0 )
				{
					int nMinimizeSize = CEtFontMng::GetInstance().GetForceMinimizeSize();
					nMinimizeSize += 1;

					CEtFontMng::GetInstance().SetForceMinimizeSize( nMinimizeSize );
					CEtFontMng::GetInstance().FlushFontCache();

					WCHAR wszString[256] = {0,};
					swprintf_s( wszString, _countof( wszString ), L"현재 적용 폰트크기 : %d", nMinimizeSize );
					GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", wszString );
				}
			}
#endif
		}
		break;
	case UM_CAPTURE_ERROR:
		{
			if( !m_pCaptureEngine ) break;
			if( !CDnInterface::IsActive() ) break;
			if( !CEtUIXML::IsActive() ) break;
			int nErrorStrIndex = -1;
			switch( wParam ) {
				case CEtBCLEngine::ErrorVideo:
				case CEtBCLEngine::ErrorAudio:
					nErrorStrIndex = 6107;
					break;
				case CEtBCLEngine::ErrorWriteFile:
				case CEtBCLEngine::ErrorCreateFile:
					nErrorStrIndex = 6108;
					break;
				case CEtBCLEngine::ErrorNotEnoughDiskSpace:
					{
						INT64 nFreeByteCaller, nTotalByte, nFreeByte;
						GetDiskFreeSpaceEx( m_pCaptureEngine->GetDefaultPath(), (PULARGE_INTEGER)&nFreeByteCaller, (PULARGE_INTEGER)&nTotalByte, (PULARGE_INTEGER)&nFreeByte );
						if( nFreeByte < 1048576 ) { // 1메가 이하일때만 에러처리하자.
							nErrorStrIndex = 6109;
						}
					}
					break;
			}
			if( nErrorStrIndex == -1 ) break;
			RecordEnd();
			GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nErrorStrIndex ), false );
		}
		break;
	case WM_SIZE:
		{
			if( ( m_bMaximize ) && ( CGameOption::GetInstance().m_bWindow ) )
			{
				RECT rect;
				GetClientRect( GetHWnd(), &rect );

				int nNewWidth = max(1024, (rect.right - rect.left));
				int nNewHeight = max(720, (rect.bottom - rect.top));

				if( nNewWidth != CGameOption::GetInstance().m_nWidth || 
					nNewHeight != CGameOption::GetInstance().m_nHeight ) {
						CGameOption::GetInstance().m_nWidth = nNewWidth;
						CGameOption::GetInstance().m_nHeight = nNewHeight;
						CGameOption::GetInstance().ChangeDisplayMode();
				}

				m_bMaximize = false;
			}
		}
		break;
	case WM_SYSCOMMAND:
		if( ( wParam == SC_KEYMENU ) && ( lParam == 0 ) )
		{
			return 1;
		}
		else if( wParam == SC_MAXIMIZE || wParam == SC_RESTORE )
		{
			m_bMaximize = true;
		}
		else
		{
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_SETCURSOR:
		{
			SetCursor( NULL );

			if( CDnMouseCursor::IsActive() )
			{
				GetEtDevice()->ShowCursor( CDnMouseCursor::GetInstance().IsShowCursor() );
			}
		}
		return TRUE;
	case WM_LBUTTONDOWN:
		s_bLButtonPressed = true;
		if( CDnInterface::IsActive() && CEtUIXML::IsActive() )
			GetInterface().CloseNameLinkToolTipDlg();
		break;
	case WM_LBUTTONUP:
		s_bLButtonPressed = false;
		break;
	case WM_MOUSEMOVE:
		if( GetAsyncKeyState( VK_LBUTTON ) == 0 )
			s_bLButtonPressed = false;
		break;
	case WM_NCHITTEST:
		{
#if defined(_CH) && defined(_AUTH)
			// OA창 웹페이지때문에 타이틀 잡고 윈도우 이동시키는 루틴 사용하지 않는다.
			break;
#endif
			LRESULT dwHit = DefWindowProc( hWnd, message, wParam, lParam );
			if( dwHit == HTCAPTION || dwHit == HTSYSMENU ) {
				if( GetAsyncKeyState( VK_LBUTTON ) < 0 && s_bLButtonPressed == false ) {
					m_MousePos.x = ((int)(short)LOWORD(lParam));
					m_MousePos.y = ((int)(short)HIWORD(lParam));
					OutputDebug( "MousePos : %d, %d\n", m_MousePos.x, m_MousePos.y );

					RECT Rect;
					GetWindowRect( hWnd, &Rect );
					m_WindowPos.x = Rect.left;
					m_WindowPos.y = Rect.top;
					OutputDebug( "Window Pos : %d, %d ( %x )\n", m_WindowPos.x, m_WindowPos.y, hWnd );

					m_bBeginCaption = true;
				}
				return HTCLIENT;
			}
			else 
				break;
		}
		break;
	case WM_CONTEXTMENU:	// 캡션바에다가 마우스 오른쪽 버튼 누를때
	case 0x313:				// 작업표시줄에서 마우슨 오른쪽 버튼 누를때
		return 1;
	}

	CTask *pTask = ( m_pTaskMng ) ? m_pTaskMng->GetTask( "DebugTask" ) : NULL;

	bool bMsgProc = true;
	if( CDnMouseCursor::IsActive() && !CDnMouseCursor::GetInstance().IsShowCursor() ) {
		if( message == WM_MOUSEMOVE || message == WM_LBUTTONDOWN || message == WM_LBUTTONUP || message == WM_LBUTTONDBLCLK ||
			message == WM_RBUTTONDOWN || message == WM_RBUTTONUP || message == WM_RBUTTONDBLCLK ||
			message == WM_MBUTTONDOWN || message == WM_MBUTTONUP || message == WM_MBUTTONDBLCLK ) bMsgProc = false;
	}

	if( !pTask && EtInterface::StaticMsgProc( hWnd, message, wParam, lParam ) )
	{
		if( bMsgProc ) {
			if( CInputDevice::IsActive() ) CInputDevice::GetInstance().SetForceProcessPass();
			// 여기서 정말 리턴할 것들만 리턴하게 하자..
			switch( message ) {
				// WM_IME_COMPOSITION으로 오는 글자들이 두번씩 찍히는 문제때문에 여기서 이렇게 리턴하도록 되어있다.
				// IMEEditBox::SendCompString 에서 직접 메세지를 날리는 구조라, 여기서 리턴을 해야 두번 처리되지 않는 것이다.
				case WM_IME_COMPOSITION:
				// 직접 렌더링하는 IME를 사용하는 도중에 윈도우용 IME창이 화면 상단에 그려진다면,
				// 처리되지 않아야할 IME관련 메세지들이 디폴트프로시저로 갔을 확률이 높다.
				// 현재 CH, JP둘다 외부 IME를 사용하기때문에 처리할 필요 없으나, 나중에 필요하면 주석 풀고 return 1 하면 될 것이다.
				//case WM_IME_STARTCOMPOSITION:
				//case WM_IME_NOTIFY:
				case WM_KEYDOWN:
					return 1;
			}
		}
	}

	if( message == WM_IME_SETCONTEXT )
		lParam = 0;

	return DefWindowProc(hWnd, message, wParam, lParam);
}

LONG __stdcall CDnMainFrame::UnhandledExceptionFilter( _EXCEPTION_POINTERS* pExceptionInfo )
{	
	if( CClientSessionManager::IsActive() ) {
		CClientSessionManager::GetInstance().DisConnectServer(false);
		/*CClientSessionManager::GetInstance().DisconnectLoginServer( false );
		CClientSessionManager::GetInstance().DisconnectVillageServer( false );
		CClientSessionManager::GetInstance().FinalizeUdpSession();*/
	}

	if( s_PrevExceptionFilter ) {
		return s_PrevExceptionFilter( pExceptionInfo );
	}
	return EXCEPTION_CONTINUE_SEARCH;
}

void CDnMainFrame::SaveScreenShotToFile( D3DXIMAGE_FILEFORMAT Format )
{
	WCHAR szPath[MAX_PATH]={0,};
	SHGetSpecialFolderPath(CDnMainFrame::GetInstance().GetHWnd(), szPath, CSIDL_PERSONAL, 0);

	__time64_t ltime;
	WCHAR szBuf[256] = { 0, }, szFile[256] = { 0, };
	WCHAR szExt[256] = { 0, };
	struct	tm *tm_ptr;
	time(&ltime);
	tm_ptr = localtime(&ltime);

	if( Format == D3DXIFF_PNG ) wsprintf( szExt, L"png" );
	else if( Format == D3DXIFF_TGA ) wsprintf( szExt, L"tga" );
	else wsprintf( szExt, L"jpg" );

	wcsftime(szBuf, 256, L"%Y-%m-%d %H-%M-%S %a", tm_ptr);
	wsprintf( szFile, L"DN %s.%s", szBuf, szExt );

	WCHAR szResultName[MAX_PATH] = { 0, };

	wsprintf( szResultName, L"%s\\DragonNest", szPath );
	if( !PathFileExists( szResultName ) ) _wmkdir( szResultName );

	wsprintf( szResultName, L"%s\\DragonNest\\Screenshot", szPath );
	if( !PathFileExists( szResultName ) ) _wmkdir( szResultName );

	wsprintf( szResultName, L"%s\\DragonNest\\Screenshot\\%s", szPath, szFile );

	if( g_pEtRenderLock ) g_pEtRenderLock->Lock();
	EternityEngine::SaveBackbuffer( szResultName, Format );
	if( g_pEtRenderLock ) g_pEtRenderLock->UnLock();

	if( CDnInterface::IsActive() && CEtUIXML::IsActive() ) {
		wchar_t wszTemp[1024]={0,};
		swprintf_s( wszTemp, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3052 ) , szFile );
		GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", wszTemp, false );
	}
}

void CDnMainFrame::ToggleRecord()
{
	if( !CDnInterface::IsActive() || !CEtUIXML::IsActive() )
		return;

	if( m_pCaptureEngine == NULL ) {
		if( CEtDevice::IsActive() && CEtDevice::GetInstancePtr() ) {
			TCHAR szTemp[256] = { 0, };
			TCHAR szPath[MAX_PATH] = { 0, };
			SHGetSpecialFolderPath( m_hWnd, szPath, CSIDL_PERSONAL, 0);

			wsprintf( szTemp, L"%s\\DragonNest", szPath );
			if( !PathFileExists( szTemp ) ) _wmkdir( szTemp );

			wsprintf( szTemp, L"%s\\DragonNest\\Movie", szPath );
			if( !PathFileExists( szTemp ) ) _wmkdir( szTemp );

			m_pCaptureEngine = new CEtBCLEngine;
			m_pCaptureEngine->Initialize( m_hWnd, CEtDevice::GetInstancePtr() ) ;
			m_pCaptureEngine->SetMinMaxFPS( 30, 60 );
			m_pCaptureEngine->SetDefaultPath( szTemp );

			CEtBCLEngine::CaptureOptionEnum Option = CEtBCLEngine::HalfSize;
			switch( CGameOption::GetInstance().m_nRecordResolution ) {
				case 0: Option = CEtBCLEngine::Default; break;
				case 1: Option = CEtBCLEngine::HalfSize; break;
				case 2: Option = CEtBCLEngine::Res_320x240; break;
				case 3: Option = CEtBCLEngine::Res_640x480; break;
				case 4: Option = CEtBCLEngine::Res_800x600; break;
			}
			m_pCaptureEngine->SetOption( Option );

			BCAP_CONFIG *pConfig = m_pCaptureEngine->GetConfig();
			switch( CGameOption::GetInstance().m_nRecordFPS ) {
				case 0: pConfig->VideoFPS = 15.f; break;
				case 1: pConfig->VideoFPS = 30.f; break;
				case 2: pConfig->VideoFPS = 60.f; break;
			}

			switch( CGameOption::GetInstance().m_nRecordVideoQuality ) {
				case 0: pConfig->VideoQuality = 30; break;
				case 1: pConfig->VideoQuality = 90; break;
			}

			switch( CGameOption::GetInstance().m_nRecordAudioQuality ) {
				case 0:
					pConfig->AdjustAudioMixer = FALSE;
					pConfig->AudioCodec = WAVETAG_NULL;
					break;
				case 1:
					pConfig->AudioChannels = 1;
					pConfig->AudioSampleRate = 22050;
					pConfig->AudioQuality = 30;
					break;
				case 2:
					pConfig->AudioChannels = 2;
					pConfig->AudioSampleRate = 44100;
					pConfig->AudioQuality = 90;
					break;
			}
			m_pCaptureEngine->RefreshConfig();

			m_pCaptureEngine->SetCaptureInfoCallback( DrawCaptureStateCallback );
		}
	}
	if( !m_pCaptureEngine ) return;

	if( !m_pCaptureEngine->IsCapturing() ) {
		RecordBegin();
	}
	else {
		RecordEnd();
	}
}

void __stdcall CDnMainFrame::DrawCaptureStateCallback()
{
	if( !CDnMainFrame::IsActive() ) return;
	if( !CDnMainFrame::GetInstance().GetRecordDlg() ) return;
	if( !CEtBCLEngine::IsActive() ) return;
	if( !CEtDevice::IsActive() ) return;
	if( !CEtSprite::IsActive() ) return;

	CDnMainFrame::GetInstance().GetRecordDlg()->Show( CEtBCLEngine::GetInstance().IsCapturing() );
	CDnMainFrame::GetInstance().GetRecordDlg()->Process( 1.f );

	GetEtDevice()->BeginScene();
	CEtDevice::GetInstance().EnableAlphaBlend( true );
	CEtSprite::GetInstance().Flush();
	CEtSprite::GetInstance().Begin( D3DXSPRITE_DONOTSAVESTATE );
	CDnMainFrame::GetInstance().GetRecordDlg()->Render( 1.f );
	CEtSprite::GetInstance().End();	
	CEtDevice::GetInstance().EnableAlphaBlend( false );
	GetEtDevice()->EndScene();

	CDnMainFrame::GetInstance().CheckRecordDiskSpace( false );
}

bool CDnMainFrame::CheckRecordDiskSpace( bool bBegin )
{
	INT64 nFreeByteCaller, nTotalByte, nFreeByte;
	GetDiskFreeSpaceEx( m_pCaptureEngine->GetDefaultPath(), (PULARGE_INTEGER)&nFreeByteCaller, (PULARGE_INTEGER)&nTotalByte, (PULARGE_INTEGER)&nFreeByte );
	WCHAR wszWarning[256] = { 0, };
	if( nFreeByte < 1048576 ) {
		if( bBegin ) {
			GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 6109 ), false );
			return false;
		}
	}
	else if( !m_bCheckRecordDiskSpace[0] && nFreeByte < 1048576 * 10 ) {
		swprintf_s( wszWarning, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 6106 ), 10 );
		m_bCheckRecordDiskSpace[0] = m_bCheckRecordDiskSpace[1] = m_bCheckRecordDiskSpace[2] = m_bCheckRecordDiskSpace[3] = true;
	}
	else if( !m_bCheckRecordDiskSpace[1] && nFreeByte < 1048576 * 50 ) {
		swprintf_s( wszWarning, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 6106 ), 50 );
		m_bCheckRecordDiskSpace[1] = m_bCheckRecordDiskSpace[2] = m_bCheckRecordDiskSpace[3] = true;
	}
	else if( !m_bCheckRecordDiskSpace[2] && nFreeByte < 1048576 * 100 ) {
		swprintf_s( wszWarning, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 6106 ), 100 );
		m_bCheckRecordDiskSpace[2] = m_bCheckRecordDiskSpace[3] = true;
	}
	else if( !m_bCheckRecordDiskSpace[3] && nFreeByte < 1048576 * 200 ) {
		swprintf_s( wszWarning, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 6106 ), 200 );
		m_bCheckRecordDiskSpace[3] = true;
	}
	if( wcslen( wszWarning ) > 0 )
		GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", wszWarning, false );

	if( bBegin ) {
		double fFreeSize;
		bool bGigaUnit = false;
		if( nFreeByte >= 1073741824 ) { // 기가가 넘어가면 GB 단위로.
			fFreeSize = nFreeByte / 1073741824.f;
			bGigaUnit = true;
		}
		else {
			fFreeSize = nFreeByte / 1048576.f;
		}

		swprintf_s( wszWarning, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 6105 ), fFreeSize, ( bGigaUnit ) ? L"GB" : L"MB" );
		GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", wszWarning, false );
	}

	return true;
}

void CDnMainFrame::RecordBegin()
{
	if( !m_pCaptureEngine ) return;
	memset( m_bCheckRecordDiskSpace, 0, sizeof(m_bCheckRecordDiskSpace) );
	if( CheckRecordDiskSpace( true ) == false ) return;

	m_pCaptureEngine->Start();
}

void CDnMainFrame::RecordEnd()
{
	if( !m_pCaptureEngine ) return;

	WCHAR wszResultMsg[128];

	INT64 nRecordSize = m_pCaptureEngine->GetCaptureFileSize();

	// 용량
	double fRecordSize;
	bool bGigaUnit = false;
	if( nRecordSize >= 1073741824 ) { // 기가가 넘어가면 GB 단위로.
		fRecordSize = nRecordSize / 1073741824.f;
		bGigaUnit = true;
	}
	else {
		fRecordSize = nRecordSize / 1048576.f;
	}

	swprintf_s( wszResultMsg, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 6110 ), m_pCaptureEngine->GetSaveFileName(), fRecordSize, ( bGigaUnit ) ? L"GB" : L"MB" );
	GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", wszResultMsg, false );

	m_pCaptureEngine->Stop();

	SAFE_DELETE( m_pCaptureEngine );
}

#ifdef PRE_FIX_CLIENT_MEMOPTIMIZE
bool __stdcall CDnMainFrame::FlushWaitDelete()
{
	if( !CEtActionCoreMng::IsActive() ) return false;
	if( CEtActionCoreMng::GetInstance().GetWaitDeleteCount() == 0 ) return false;

	CEtActionCoreMng::GetInstance().FlushWaitDelete();
	return true;
}

#else
bool CDnMainFrame::ThreadPreloadPlayer()
{
	// Player Preload
	for( int i=0; i<10; i++ ) {
		m_hPreLoadPlayerClass[i] = CreateActor( i + 1, false, false, false );
	}
	return true;
}

void CDnMainFrame::ThreadInitializePreloadPlayer()
{
	m_hPreloadPlayerThreadHandle = (HANDLE)_beginthreadex( NULL, 65536, PlayerPreloadThread, (void*)this, 0, &m_dwPreloadPlayerThreadID );
}

void CDnMainFrame::FlushPreloadPlayer()
{
	for( int i=0; i<10; i++ ) {
		if( m_hPreLoadPlayerClass[i] ) m_hPreLoadPlayerClass[i]->FlushSoundSignal();
	}
}
#endif

void CDnMainFrame::FlushWndMessage()
{
	MSG msg;
	DWORD dwPrevMsg = 0;
	while( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) ) {
		if( dwPrevMsg == WM_PAINT && msg.message == WM_PAINT ) break;
		dwPrevMsg = msg.message;
	}
}

void CDnMainFrame::SaveWindowPos()
{
	if( CGameOption::IsActive() && CGameOption::GetInstance().m_bWindow)
	{
		WINDOWPLACEMENT wp;
		wp.length = sizeof(WINDOWPLACEMENT);
		GetWindowPlacement(GetHWnd(), &wp);

		CGameOption::GetInstance().m_nWndPosX = wp.rcNormalPosition.left;
		CGameOption::GetInstance().m_nWndPosY = wp.rcNormalPosition.top;
	}
}

#ifdef PRE_ADD_STEAMWORKS
void CDnMainFrame::OnSteamOverlayActivated( GameOverlayActivated_t *callback )
{
	if( !callback || !CGlobalInfo::GetInstance().m_bUseSteamworks ) return;

	if( !callback->m_bActive )
	{
		if( CDnCashShopTask::IsActive() )
			GetCashShopTask().OnSteamOverlayDeActivated();
	}
	
	CDnMouseCursor::GetInstance().SetForceClipCursorNull( true );
}
#endif // PRE_ADD_STEAMWORKS