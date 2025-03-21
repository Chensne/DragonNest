#include "StdAfx.h"
#include "DnSDOAService.h"
#include "LoginSendPacket.h"
#include "DnMainFrame.h"
#include "shlobj.h"
#include "DnInterface.h"

#pragma warning( disable : 4996 ) // disable deprecated warning 
#include <strsafe.h>
#pragma warning( default : 4996 ) 

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

#if defined(_CH) && defined(_AUTH)

CDnSDOAService::CDnSDOAService()
{
	m_hIGW = NULL;

#ifdef PRE_ADD_SHANDA_GPLUS
	m_fpGPlusInitialize = NULL;
	m_fpGPlusGetModule = NULL;
	m_fpGPlusTerminal = NULL;
	m_pGPlusRender = NULL;
	m_pGPlusApp = NULL;
#endif // #ifdef PRE_ADD_SHANDA_GPLUS

	m_pIGWInitialize = NULL;
	m_pIGWGetModule = NULL;
	m_pIGWTerminal = NULL;
	m_pSDOARender = NULL;
	m_pSDOAApp = NULL;
	m_bSDOLogin = false;
	m_bShow = false; 
	m_bShowLoginDialog = false;
	m_pPrevPrePresentCallback = NULL;
	m_bShowChargeDlg = false;

#ifdef PRE_ADD_SHANDA_GPLUS
	m_fpGPlusInitialize = NULL;
	m_fpGPlusGetModule = NULL;
	m_fpGPlusTerminal = NULL;
	m_pGPlusApp = NULL;
	m_pGPlusRender = NULL;
#endif // #ifdef PRE_ADD_SHANDA_GPLUS
}

CDnSDOAService::~CDnSDOAService()
{
}

bool CDnSDOAService::Initialize( bool bShow )
{
#ifdef PRE_ADD_SHANDA_GPLUS
	if( InitializeGPlus() == false )
	{
		FinalizeGPlus();
		return false;
	}
#endif // #ifdef PRE_ADD_SHANDA_GPLUS

	if( InitializeIGW() == false ) {
		FinalizeIGW();
		return false;
	}

	Show( bShow );

	// 미리 디바이스 PrePresentCallback에 등록시켜둔다.
	// 이렇게 하지 않으면 BCL관련 콜백과 꼬일 수 있어서 미리 하는거다.
	// (캡쳐시작 - SDOA콜백등록 - SDOA콜백해제 - 캡쳐끝 이거나
	//  SDOA콜백등록 - 캡쳐시작 - 캡쳐끝 - SDOA콜백해제 같은 순서는 괜찮다.
	//  그러나 캡쳐시작 - SDOA콜백등록 - 캡쳐끝 이렇게 순서가 꼬여버리면 콜백에 NULL이 들어가서 나중에 등록된 콜백함수가 호출안되게 된다.)
	m_pPrevPrePresentCallback = ( void ( __stdcall * )() )GetEtDevice()->SetPrePresentCallback( PrePresentCallback );

	return true;
}

void CDnSDOAService::Finalize()
{
	Logout();

	GetEtDevice()->SetPrePresentCallback( m_pPrevPrePresentCallback );

	FinalizeIGW();

#ifdef PRE_ADD_SHANDA_GPLUS
	FinalizeGPlus();
#endif // #ifdef PRE_ADD_SHANDA_GPLUS
}


int CDnSDOAService::GetPartitionID()
{
	int nResult = -1;
	FILE *stream = NULL;
	char szString[256];
	TCHAR pBuffer[MAX_PATH] = { 0, };
	char szPath[_MAX_PATH] = { 0, };
	SHGetSpecialFolderPath( CDnMainFrame::GetInstance().GetHWnd(), pBuffer, CSIDL_PERSONAL, 0 );
	WideCharToMultiByte( CP_ACP, 0, pBuffer, -1, szPath, sizeof(szPath), NULL, NULL );

	char szPartitionFilename[MAX_PATH]={0,};
	sprintf_s( szPartitionFilename, "%s\\DragonNest\\Config\\%s", szPath, "Partition.ini" );
	if( fopen_s( &stream, szPartitionFilename, "r+" ) == 0 )
	{
		fseek( stream, 0L, SEEK_SET );
		fscanf_s( stream, "%s", szString );
		if( strcmp("SelectChannelNum",szString ) == 0 )
			fscanf_s( stream, "%d", &nResult );
		//		fscanf_s( stream, "%s", szString );
		//		if( strcmp("SelectPartitionNum",szString ) == 0 )
		//			fscanf_s( stream, "%d", &nValue );
		fscanf_s( stream, "%s", szString );
		if( strcmp("SelectPartitionId",szString ) == 0 )
			fscanf_s( stream, "%d", &nResult );
		fclose( stream );
	}

	return nResult;
}


#ifdef PRE_ADD_SHANDA_GPLUS
bool CDnSDOAService::InitializeGPlus()
{
	m_hGPlus = LoadLibraryW( L"./GPlus/GPlusBridge.dll" );

	int appId = 200010900;
	FILE* f = NULL;
	fopen_s( &f, "./appid.txt","r" );
	if(f != NULL){
		char value[1024] = {0};
		if(fgets(value,1024,f))
		{
			appId = atoi(value);
		}
		fclose(f);
	}

	if( !m_hGPlus )
	{
		WCHAR strExePath[MAX_PATH] = {0};
		WCHAR strExeName[MAX_PATH] = {0};
		WCHAR* strLastSlash = NULL;
		GetModuleFileNameW( NULL, strExePath, MAX_PATH );
		strExePath[MAX_PATH-1]=0;
		strLastSlash = wcsrchr( strExePath, TEXT('\\') );
		if( strLastSlash )
		{	// 돤돕EXE杰瞳쨌쓺
			StringCchCopyW( strExeName, MAX_PATH, &strLastSlash[1] );
			*strLastSlash = 0;
			strLastSlash = wcsrchr( strExeName, TEXT('.') );
			if( strLastSlash )
				*strLastSlash = 0;
		}

		WCHAR strGPlusDllFileName[MAX_PATH] = {0};
		StringCchPrintfW( strGPlusDllFileName, MAX_PATH, L"%s\\GPlus\\GPlusBridge.dll", strExePath );

		if ( GetFileAttributesW(strGPlusDllFileName) != 0xFFFFFFFF )
		{	// 冷돕莉숭橙속潼	
			m_hGPlus = LoadLibrary(strGPlusDllFileName);
		}

		if( !m_hGPlus )
			return false;
	}

	//삿혤돔놔변鑒
	m_fpGPlusInitialize = (LPGPlusInitialize)GetProcAddress( m_hGPlus, "GPlusInitialize" );
	m_fpGPlusGetModule = (LPGPlusGetModule)GetProcAddress( m_hGPlus, "GPlusGetModule" );
	m_fpGPlusTerminal = (LPGPlusTerminal)GetProcAddress( m_hGPlus, "GPlusTerminal" );

	if( !m_fpGPlusInitialize || !m_fpGPlusGetModule || !m_fpGPlusTerminal )
		return false;

	// 파티션ID 읽어오기. 런처쪽 코드 가져와서 읽는거다. 파일포맷 바뀌면 신찬씨한테 다시 물어보기.
	int nValue = GetPartitionID();
	
	//// 鬧雷GPlusAppInfo잚謹杰唐俚뙈극伎輕
	//GPlusAppInfo Info = {
	//	sizeof(GPlusAppInfo),       // 써뭐竟댕鬼
	//	appId,						// 쌈흙壇痰ID，닒역랙諒貢籃櫓헝
	//	L"질繭빪氣OnLine",          // 壇痰츰냔
	//	L"1.0.0.1",
	//	GPLUS_RENDERTYPE_D3D9,      // 와빵똥連넣돨暠近多헐잚謹
	//	0,
	//	1,
	//	2,
	//};

	WCHAR wszVersion[32] = { 0, };
	GPlusAppInfo Info;
	Info.cbSize = sizeof(GPlusAppInfo);
	Info.nAppID = 89;
	Info.pwcsAppName = L"DragonNest";
	MultiByteToWideChar( CP_ACP, 0, szVersion, -1, wszVersion, 32 );
	Info.pwcsAppVer = wszVersion;
	Info.nRenderType = GPLUS_RENDERTYPE_D3D9;
	Info.nDeviceIndex = 0;
	Info.nAreaId = nValue;
	Info.nGroupId = -1;

	//놓迦뺏
	if( m_fpGPlusInitialize(GPLUS_SDK_VERSION, &Info) != GPLUS_OK ) 
		return false;

	// false 가 리턴돼서 일단 그냥 호출만..
	m_fpGPlusGetModule(__uuidof(IGPLUSApp), (void**)&m_pGPlusApp);
	m_fpGPlusGetModule(__uuidof(IGPLUSDx9), (void**)&m_pGPlusRender);
		

		/* 鬧雷:苟충侶寧뙈덜쯤,壇맡瞳踏狗실되쩌냥묘疼딧痰,侶쟁쏭鱗蘿刻痰
		if ( g_GPlusDx9 && g_GPlusApp)
		{
		GPlusUserInfo tmpRoleInfo = { sizeof(GPlusUserInfo),
		L"鑒俚id" , 
		L"실츰"} ;

		g_GPlusApp->Login(&tmpRoleInfo);
		}*/

	return true;
}

void CDnSDOAService::FinalizeGPlus( void )
{
	m_pGPlusRender = NULL;
	m_pGPlusApp = NULL;

	m_fpGPlusInitialize = NULL;
	if( m_fpGPlusTerminal)
	{  // 姦렴DLL품悧角邱딧痰gpTerminal
		m_fpGPlusTerminal();
		m_fpGPlusTerminal = NULL;
	}
	if( m_hGPlus )
	{
		FreeLibrary( m_hGPlus );
		m_hGPlus = NULL;
	}
}

void CDnSDOAService::GPlusLogin_deprecated( const LoginResult* pLoginResult )
{
	if( m_pSDOAApp && m_pGPlusApp && m_pGPlusRender )
	{
		wchar_t ID[256]={0};
		MultiByteToWideChar(CP_ACP, 0, pLoginResult->szSndaid, -1, ID, _countof(ID));
		GPlusUserInfo gplustmpRoleInfo = { sizeof(GPlusUserInfo),
			ID , 
			m_RoleInfo.pwcsRoleName} ;

		m_pGPlusApp->Login(&gplustmpRoleInfo);
	}
}

void CDnSDOAService::SetLoginResultShandaID( LPCSTR pShandaID )
{
	if( pShandaID )
		m_strShandaID = pShandaID;
}

void CDnSDOAService::GPlusLogin( LPCWSTR pwcsRoleName )
{
	wchar_t ID[256]={0};
	MultiByteToWideChar(CP_ACP, 0, m_strShandaID.c_str(), -1, ID, _countof(ID));
	GPlusUserInfo gplustmpRoleInfo = { sizeof(GPlusUserInfo), ID , pwcsRoleName };

	m_pGPlusApp->Login(&gplustmpRoleInfo);
}

#endif // #ifdef PRE_ADD_SHANDA_GPLUS

bool CDnSDOAService::InitializeIGW()
{
#ifdef PRE_MOD_SDOLOGIN
	m_hIGW = LoadLibraryW( L"sdo\\sdologinentry.dll" );
#else
	m_hIGW = LoadLibraryW( L"GameWidget.dll" );
#endif
	if( !m_hIGW )
	{
		WCHAR strExePath[MAX_PATH] = {0};
		WCHAR strExeName[MAX_PATH] = {0};
		WCHAR* strLastSlash = NULL;
		GetModuleFileNameW( NULL, strExePath, MAX_PATH );
		strExePath[MAX_PATH-1] = 0;
		strLastSlash = wcsrchr( strExePath, TEXT('\\') );
		if( strLastSlash )
		{
			StringCchCopyW( strExeName, MAX_PATH, &strLastSlash[1] );
			*strLastSlash = 0;
			strLastSlash = wcsrchr( strExeName, TEXT('.') );
			if( strLastSlash )
				*strLastSlash = 0;
		}

		WCHAR strGameWidgetDll[MAX_PATH] = {0};
#ifdef PRE_MOD_SDOLOGIN
		StringCchPrintfW( strGameWidgetDll, MAX_PATH, L"%s\\sdo\\sdologinentry.dll", strExePath );
#else
		StringCchPrintfW( strGameWidgetDll, MAX_PATH, L"%s\\GameWidget.dll", strExePath );
#endif
		if( GetFileAttributesW( strGameWidgetDll ) != 0xFFFFFFFF )
			m_hIGW = LoadLibraryW( strGameWidgetDll );
		if( !m_hIGW ) return false;
	}

	m_pIGWInitialize = (LPigwInitialize)GetProcAddress( m_hIGW, "igwInitialize" );;
	m_pIGWGetModule = (LPigwGetModule)GetProcAddress( m_hIGW, "igwGetModule" );;
	m_pIGWTerminal = (LPigwTerminal)GetProcAddress( m_hIGW, "igwTerminal" );
	if( !m_pIGWInitialize || !m_pIGWGetModule || !m_pIGWTerminal ) return false;

	// 파티션ID 읽어오기. 런처쪽 코드 가져와서 읽는거다. 파일포맷 바뀌면 신찬씨한테 다시 물어보기.
	int nValue = GetPartitionID();

	/*
	AppInfo Info = { 
		sizeof(AppInfo),
		89,
		L"DragonNest",
		L"0.1.2.0",
		SDOA_RENDERTYPE_D3D9,
		1,
		-1,
		-1,
	};
	*/

	WCHAR wszVersion[32] = { 0, };
	/*
	AppInfo Info = {
		sizeof(AppInfo),       // size of the struct which is easy to expand
		89,                  // APPID，apply from open.sdo.com
		L"DragonNest",     // APPID NAME
		L"0.1.2.0",            // app client current version number
		SDOA_RENDERTYPE_D3D9,  // graphics engine supported by the client，d3d7
		1,                     // max number of player per computer(1 is the common situation ,but in KOF the number is 2)
		-1,                    // district_ID unavailable
		-1                     //group_ID unavailable
	};
	*/
	AppInfo Info;
	Info.cbSize = sizeof(AppInfo);
	Info.nAppID = 89;
	Info.pwcsAppName = L"DragonNest";
	MultiByteToWideChar( CP_ACP, 0, szVersion, -1, wszVersion, 32 );
	Info.pwcsAppVer = wszVersion;
	Info.nRenderType = SDOA_RENDERTYPE_D3D9;
	Info.nMaxUser = 1;
	Info.nAreaId = nValue;
	Info.nGroupId = -1;

	if( m_pIGWInitialize( SDOA_SDK_VERSION, &Info ) != SDOA_OK ) return false;

	if( m_pIGWGetModule(__uuidof(ISDOADx9), (void**)&m_pSDOARender) == false ) return false;
	if( m_pIGWGetModule(__uuidof(ISDOAApp), (void**)&m_pSDOAApp) == false ) return false;

	// 후킹을 쓰면 디바이스 복구부터 렌더까지 다 알아서 해줘서 편하지만, 문제가 있었다.
	// 후킹을 쓰게되면 모든게 다 자동이라 세로형태의 메인 태스크바도 알아서 렌더링이 되버린다.
	// 이걸 임의로 안보이게 하려고 제공되는 api를 다 뒤져봤는데,
	// (SetTaskBarPosition으로 화면밖으로 옮겨도 클라이언트 영역 안으로 자동으로 들어와버리고,
	//  SetScreenStatus( SDOA_SCREEN_MINI )를 호출해도 디스에이블된 메인버튼 하나는 무조건 남는다.
	//  마지막으로 아래 코드도 해봤는데,
	//SDOAWinProperty tempWinProperty;
	//if( m_pSDOAApp->GetWinProperty( L"taskbarsvertical", &tempWinProperty) == SDOA_OK )
	//{
	//	tempWinProperty.nStatus = SDOA_WINDOWSSTATUS_HIDE;
	//	if( m_pSDOAApp->SetWinProperty( L"taskbarsvertical", &tempWinProperty) == SDOA_OK )
	//	{
	//	}
	//}
	//  이거 역시 위에 MINI로 최소화 시키는것까지가 전부였다.)
	//
	// 결국 unHook을 해놓고 필요할때 다시 Hook할까 했는데,
	// 플레이중간에 Hook하면 약 1초정도 렌더링 딜레이가 생겨서 아무래도 별로인거 같아, 그냥 직접 렌더링 하는 방법을 택하게 되었다.
	//if( m_pIGWGetModule(__uuidof(ISDOADx9Hook), (void**)&m_pSDOARenderHook) == false ) return false;

	return true;
}

void CDnSDOAService::FinalizeIGW()
{
	SAFE_RELEASE( m_pSDOARender );
	SAFE_RELEASE( m_pSDOAApp );

	if( m_pIGWTerminal ) m_pIGWTerminal();

	if( m_hIGW ) {
		FreeLibrary( m_hIGW );
		m_hIGW = NULL;
	}

	m_pIGWInitialize = NULL;
	m_pIGWGetModule = NULL;
	m_pIGWTerminal = NULL;
}

void CDnSDOAService::Render()
{
	if( !IsShow() ) return;

#ifdef PRE_ADD_SHANDA_GPLUS
	if( m_pGPlusRender )
		m_pGPlusRender->RenderEx();
#endif // #ifdef PRE_ADD_SHANDA_GPLUS

	if( m_pSDOARender )
		m_pSDOARender->RenderEx();
}

void __stdcall CDnSDOAService::PrePresentCallback()
{
	if( CDnSDOAService::GetInstance().m_pPrevPrePresentCallback )
		CDnSDOAService::GetInstance().m_pPrevPrePresentCallback();

	if( !CDnSDOAService::IsActive() ) return;

	CEtDevice::GetInstance().EnableAlphaBlend( false );
	GetEtDevice()->BeginScene();
	CDnSDOAService::GetInstance().Render();
	GetEtDevice()->EndScene();
}

void CDnSDOAService::Show( bool bShow )
{
	if( m_bShow == bShow )
		return;

	if( !m_pSDOARender ) {
		m_bShow = false;
		return;
	}

	m_bShow = bShow;
}

bool CDnSDOAService::IsFocus()
{
	if( !m_bShow )
		return false;

	if( !m_pSDOAApp )
		return false;

	return m_pSDOAApp->GetFocus();
}

void CDnSDOAService::ShowLoginDialog( bool bShow )
{
	if( m_bShowLoginDialog == bShow )
		return;

	if( !m_pSDOAApp ) {
		m_bShowLoginDialog = false;
		return;
	}

	if( bShow ) {
		SDOAWinProperty WinProperty;
		if( m_pSDOAApp->GetWinProperty( L"igwUserLoginDialog", &WinProperty ) == SDOA_OK ) {
			WinProperty.nStatus = SDOA_WINDOWSSTATUS_SHOW;
			if( m_pSDOAApp->SetWinProperty( L"igwUserLoginDialog", &WinProperty ) == SDOA_OK )
				m_bShowLoginDialog = bShow;
		}
		else {
			// 아래 SetWinProperty를 통해 하이드 시켰다면, ShowLoginDialog를 호출해도 보이지 않는다.
			if( m_pSDOAApp->ShowLoginDialog( &CDnSDOAService::OnLogin, 0, 0 ) == SDOA_OK )
				m_bShowLoginDialog = bShow;
		}
	}
	else {
		SDOAWinProperty WinProperty;
		if( m_pSDOAApp->GetWinProperty( L"igwUserLoginDialog", &WinProperty ) == SDOA_OK ) {
			WinProperty.nStatus = SDOA_WINDOWSSTATUS_HIDE;
			if( m_pSDOAApp->SetWinProperty( L"igwUserLoginDialog", &WinProperty ) == SDOA_OK )
				m_bShowLoginDialog = bShow;
		}
	}
}

void CDnSDOAService::Logout()
{
	if( m_pSDOAApp && m_bSDOLogin )
	{
		m_pSDOAApp->Logout();
#ifdef PRE_ADD_SHANDA_GPLUS
		m_pGPlusApp->Logout();
#endif // #ifdef PRE_ADD_SHANDA_GPLUS
		m_bSDOLogin = false;
	}
}

void CDnSDOAService::SetTaskBarPosition( int nX, int nY )
{
	// 클라이언트 영역 좌표를 받는다.
	POINT pt;
	pt.x = nX;
	pt.y = nY;
	m_pSDOAApp->SetTaskBarPosition( &pt );
}

bool CDnSDOAService::SetLoginDialogPosition( int nX, int nY )
{
	SDOAWinProperty WinProperty;
	if( m_pSDOAApp->GetWinProperty( L"igwUserLoginDialog", &WinProperty ) == SDOA_OK ) {
		WinProperty.nLeft = nX;
		WinProperty.nTop = nY;
		if( m_pSDOAApp->SetWinProperty( L"igwUserLoginDialog", &WinProperty ) == SDOA_OK )
			return true;
	}
	return false;
}

bool CDnSDOAService::GetLoginDialogRect( int &nX, int &nY, int &nWidth, int &nHeight )
{
	SDOAWinProperty WinProperty;
	if( m_pSDOAApp->GetWinProperty( L"igwUserLoginDialog", &WinProperty ) == SDOA_OK ) {
		nX = WinProperty.nLeft;
		nY = WinProperty.nTop;
		nWidth = WinProperty.nWidth;
		nHeight = WinProperty.nHeight;
		return true;
	}
	return false;
}

void CDnSDOAService::OnCreateDevice()
{
	if( m_pSDOARender )
		m_pSDOARender->Initialize( (LPDIRECT3DDEVICE9)GetEtDevice()->GetDevicePtr(), GetEtDevice()->GetPresentParameter(), false );

#ifdef PRE_ADD_SHANDA_GPLUS
	if( m_pGPlusRender )
		m_pGPlusRender->Initialize( (LPDIRECT3DDEVICE9)GetEtDevice()->GetDevicePtr(), GetEtDevice()->GetPresentParameter(), false );
#endif // #ifdef PRE_ADD_SHANDA_GPLUS
}

void CDnSDOAService::OnDestroyDevice()
{
	if( m_pSDOARender )
		m_pSDOARender->Finalize();

#ifdef PRE_ADD_SHANDA_GPLUS
	if( m_pGPlusRender )
		m_pGPlusRender->Finalize();
#endif // #ifdef PRE_ADD_SHANDA_GPLUS
}

void CDnSDOAService::OnLostDevice()
{
	if( m_pSDOARender )
		m_pSDOARender->OnDeviceLost();

#ifdef PRE_ADD_SHANDA_GPLUS
	if( m_pGPlusRender )
		m_pGPlusRender->OnDeviceLost();
#endif // #ifdef PRE_ADD_SHANDA_GPLUS
}

void CDnSDOAService::OnResetDevice()
{
	if( m_pSDOARender )
		m_pSDOARender->OnDeviceReset( GetEtDevice()->GetPresentParameter() );
	SetTaskBarPosition( 2000, 0 );

#ifdef PRE_ADD_SHANDA_GPLUS
	if( m_pGPlusRender )
		m_pGPlusRender->OnDeviceReset( GetEtDevice()->GetPresentParameter() );
#endif // #ifdef PRE_ADD_SHANDA_GPLUS
}

LRESULT CDnSDOAService::WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, bool *pbNoFurtherProcessing )
{
	if( !IsShow() )
		return false;

#ifdef PRE_ADD_SHANDA_GPLUS
	if( m_pGPlusRender ) {
		LRESULT lResult;
		if( m_pGPlusApp->OnWindowProc( hWnd, message, wParam, lParam, &lResult ) == SDOA_OK ) {
			if( pbNoFurtherProcessing ) *pbNoFurtherProcessing = true;
			return lResult;
		}
	}
#endif // #ifdef PRE_ADD_SHANDA_GPLUS

	if( m_pSDOARender ) {
		LRESULT lResult;
		if( m_pSDOARender->OnWindowProc( hWnd, message, wParam, lParam, &lResult ) == SDOA_OK ) {
			if( pbNoFurtherProcessing ) *pbNoFurtherProcessing = true;
			return lResult;
		}
	}

	return false;
}

BOOL CALLBACK CDnSDOAService::OnLogin( int nErrorCode, const LoginResult* pLoginResult, int nUserData, int nReserved )
{
	LoginResult loginResult;
	CDnSDOAService *pInstance = CDnSDOAService::GetInstancePtr();
	if( !pInstance ) return false;
	if( !pInstance->GetSDOAApp() ) return false;

	bool bLogin = false;
	bLogin = (SDOA_ERRORCODE_OK == nErrorCode);
	if( bLogin ) {
		loginResult = *pLoginResult;
		//RoleInfo tmpRoleInfo = { sizeof(RoleInfo), L"쳄챵鉤綾", 0 };
		//pInstance->GetSDOAApp()->SetRoleInfo( &tmpRoleInfo );

		WCHAR szLocalIP[ 32 ];
		CClientSessionManager::GetInstance().GetLocalIP( szLocalIP );
		SendCheckLoginCH( szLocalIP, loginResult.szSessionId );

		//pInstance->Show( false );	// 로그인 후에 항상 계속 보여달라고 해서 하이드 시키지 않습니다.
		pInstance->GetSDOAApp()->SetScreenStatus( SDOA_SCREEN_MINI );	// 기본 상태로 보이게(로그인되기전에 안보이는 상태라면 로그인 후 보이게 하기위해 호출)
		pInstance->ForceSetShowLoginDialogState( false );	// 알아서 닫히므로 해줘야한다.

#ifdef PRE_ADD_SHANDA_GPLUS
		pInstance->SetLoginResultShandaID( pLoginResult->szSndaid );
#endif // #ifdef PRE_ADD_SHANDA_GPLUS
	}
	else {
		// x눌러서 닫은 경우다.
		if( SDOA_ERRORCODE_CANCEL == nErrorCode )
		{
			pInstance->ForceSetShowLoginDialogState( false );
			GetInterface().MessageBox( MESSAGEBOX_21, MB_YESNO, MESSAGEBOX_21, CDnInterface::GetInstancePtr() );
		}
	}
	pInstance->SetLoginState( bLogin );

	// process login error message
	if (SDOA_ERRORCODE_SHOWMESSAGE == nErrorCode) {
		if( nReserved != 0 ) {
			LoginMessage* pCurLoginMsg = (LoginMessage*)nReserved;
			if( pCurLoginMsg->dwSize >= sizeof(LoginMessage) ) {
				// process compatibility
				//  changing the title of error message dialog box
				//SysReAllocString(pCurLoginMsg->pbstrTitle, L"[test:error information]");

				// return false:using default error message

				// return true:using self-defined error message
				//isSDOLogin = TRUE;
				//::MessageBox(0, *pCurLoginMsg->pbstrContent, *pCurLoginMsg->pbstrTitle, MB_OK);
			}
		}
	}
	return bLogin; // general logics:close login dialog box after login successfully
} 

bool CDnSDOAService::ShowChargeDlg(bool bShow)
{
	if (m_pSDOAApp == NULL)
	{
		_ASSERT(0);
		return false;
	}

	if (bShow && m_bShowChargeDlg)
		return false;

	if (bShow)
	{
		if (m_pSDOAApp->OpenWidgetEx(L"sdoNewPay", NULL, SDOA_OPENWIDGET_DEFAULT) != SDOA_OK)
			return false;
	}

	m_bShowChargeDlg = bShow;
	return true;
}

bool CDnSDOAService::IsShowChargeDlg() const
{
	if (m_pSDOAApp == NULL)
	{
		_ASSERT(0);
		return false;
	}

	SDOAWinProperty WinProperty;
	if (m_pSDOAApp->GetWinProperty(L"sdoNewPay", &WinProperty) == SDOA_OK)
		return (WinProperty.nStatus == SDOA_WINDOWSSTATUS_SHOW);

	return false;
}
#endif	// _CH