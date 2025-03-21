#include "stdafx.h"

#ifdef _HSHIELD
#include "HShieldSetup.h"
#endif // _HSHIELD
#if defined(_US)
#include "shellapi.h"
#endif // _US

/*
넥슨 와이즈 로그 url

웹	게임다운로드	http://dragonnest.nexon.com/wiselog/clientldownload.aspx
웹게임스타트	http://dragonnest.nexon.com/wiselog/gamestart.aspx
클라이언트	런처시작	http://dragonnest.nexon.com/wiselog/gamelauncher.aspx
버전체크시작	http://dragonnest.nexon.com/wiselog/versioncheckstart.aspx
버전체크완료	http://dragonnest.nexon.com/wiselog/versioncheckend.aspx
패치시작	http://dragonnest.nexon.com/wiselog/patchstart.aspx
패치완료	http://dragonnest.nexon.com/wiselog/patchend.aspx
핵실드초기화시작	http://dragonnest.nexon.com/wiselog/hackinistart.aspx
핵실드초기화완료	http://dragonnest.nexon.com/wiselog/hackiniend.aspx
게임엔진시작	http://dragonnest.nexon.com/wiselog/gameengin.aspx
CI동영상시작	http://dragonnest.nexon.com/wiselog/cimov.aspx
서버선택	http://dragonnest.nexon.com/wiselog/server.aspx
캐릭터선택	http://dragonnest.nexon.com/wiselog/character.aspx
채널선택	http://dragonnest.nexon.com/wiselog/channel.aspx

로그 매니져 http://gamelog.nexon.com/dragonnest/check.html
*/	

#if ( defined(_KR) || defined(_US) ) && defined(_AUTH)

#include "DnString.h"
#include "BugReporter.h"
#include "DnNexonService.h"
#include "DnMainFrame.h"
#include "ClientUtil.h"
#include "LoginSendPacket.h"
#include "DnLoginTask.h"
#include "TaskManager.h"
//	필요한 메신저 헤더 파일 인클루드
#include "KOR_NEXON/NMClass/NMManager.h"
#include "KOR_NEXON/NMClass/NMEventHandler.h"
#include "KOR_NEXON/NMClass/NMGameLogManager.h"
// 와이즈 로그
#include "../WiseLog/WiseLog.h"

WiseLog g_WiseLog;
//NMGameLogManager g_NMGameLogManager;

#define	_nmman	CNMManager::GetInstance()
#define	_nmco	CNMCOClientObject::GetInstance()

//	메신저 이벤트를 처리하기 위한 윈도 메시지 정의
#define	WM_NMEVENT	( WM_USER + 17 )

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


//	메신저 이벤트 핸들러 선언
class CMessengerEventHandler : public CNMEventHandler
{
public:
	//	인증서버와의 접속이 끊어졌을 경우에 발생
	virtual void OnAuthConnectionClosedEvent( CNMAuthConnectionClosedEvent * pEvent );

	//	메신저서버와의 접속이 연결되었을 경우에 발생
	virtual void OnMessengerReplyEvent( CNMMessengerReplyEvent * pEvent );

	//	메신저서버와의 접속이 끊어졌을 경우에 발생
	virtual void OnMsgConnectionClosedEvent( CNMMsgConnectionClosedEvent * pEvent );
}g_msngrEventHandler;

void CMessengerEventHandler::OnAuthConnectionClosedEvent( CNMAuthConnectionClosedEvent * pEvent )
{
	ClientUtil::Log::LogTime( _T("%s, %d, %s"), _T(__FUNCTION__), pEvent->uType, ClientUtil::Convert::AuthResultToString( static_cast<NMLoginAuthReplyCode>( pEvent->uType ) ) );
}

//	메신저서버와의 접속이 연결되었을 경우에 발생
void CMessengerEventHandler::OnMessengerReplyEvent( CNMMessengerReplyEvent * pEvent )
{
	ClientUtil::Log::LogTime( _T("%s, %d, %s"), _T(__FUNCTION__), pEvent->nResultCode, ClientUtil::Convert::MessengerResultToString( static_cast<NMMessengerReplyCode>( pEvent->nResultCode ) ) );

	if ( pEvent->nResultCode == kMessengerReplyOK || pEvent->nResultCode == kMessengerReplyNewUser )
	{
		_nmco.GetMyInfo();

		//	리얼유저 정보 표시
		ClientUtil::Log::LogInfo( _T("    Name: %s"), _nmco.GetMyName() );
		ClientUtil::Log::LogInfo( _T("    Nickname: %s"), _nmco.GetMyNickName() );
		SYSTEMTIME st = _nmco.GetMyBirthday();
		ClientUtil::Log::LogInfo( _T("    Birthday: %04d/%02d/%02d"), st.wYear, st.wMonth, st.wDay );

		// 메신저 서버와 접속 완료 이벤트 
		g_pServiceSetup->OnEvent( 2, NULL );
	}
}

//	메신저서버와의 접속이 끊어졌을 경우에 발생
void CMessengerEventHandler::OnMsgConnectionClosedEvent( CNMMsgConnectionClosedEvent * pEvent )
{
	ClientUtil::Log::LogTime( _T("%s, %d, %s"), _T(__FUNCTION__), pEvent->uType, ClientUtil::Convert::MessengerConnectionClosedEventToString( pEvent->uType ) );
}



//----------------------------------------------------------------------------------------------------------------------------
DnNexonService::DnNexonService()
{
	m_nSelectCharIndex = -1;
	m_szNexonPassPort.reserve( NXPASSPORT_SIZE );
	m_szPassPort.reserve( NXPASSPORT_SIZE );
	m_wszCharacterName.clear();
	m_nServerIndex = 1; 
	m_eGameState = eGameState::eNone;
	m_bStandAloneMode = false;
}

DnNexonService::~DnNexonService()
{
	CNMManager *pManager = &CNMManager::GetInstance();
	SAFE_DELETE( pManager );
	CNMCOClientObject *pObject = &CNMCOClientObject::GetInstance();
	SAFE_DELETE( pObject );
}

int DnNexonService::PreInitialize( void* pCustomData )
{
	// Get Passport from CommandLine.
	TCHAR* pCmdLine = ::GetCommandLine();
	std::wstring wszCmdLine; 
	wszCmdLine = pCmdLine;
	std::vector<std::wstring> tokens;
	TokenizeW( wszCmdLine, tokens, std::wstring(L"/") );

	if( tokens.size() > 1 )
	{
		wszCmdLine = tokens[0];

		size_t nFirst = wszCmdLine.find_first_of( L'"' );
		size_t nLast = wszCmdLine.find_last_of( L'"' );

		if( nLast != string::npos && nFirst != string::npos )
			wszCmdLine.erase( nFirst, nLast+1 );

		size_t nSpace = wszCmdLine.find_first_of( L' ' );
		if( nSpace != string::npos  )
			wszCmdLine.erase( 0, nSpace+1 );

		RemoveSpaceW( wszCmdLine );

		m_szPassPort = wszCmdLine.c_str();
	}

	for( DWORD i=0; i<tokens.size(); i++ )	// StandAlone 모드인지 확인
	{
		if( wcscmp( tokens[i].c_str(), L"stand_alone" ) == 0 )
		{
			m_bStandAloneMode = true;
			break;
		}
	}

#if defined(_US)
	int nArgs = 0;
	LPWSTR* pWstrArg = ::CommandLineToArgvW( pCmdLine, &nArgs );
	if( !m_bStandAloneMode && nArgs == 1 )
	{
		ShowNexonWebpage();
		return -1;
	}
#endif // _US

	//	[필수] 메신저 매니저 초기화
#if defined(_KR)
	NMLOCALEID	uLocaleId = kLocaleID_KR;
#elif defined(_US)
	NMLOCALEID	uLocaleId = kLocaleID_US;
	NMREGIONCODE uRegionCode = kRegionCode_NXA2;
#endif
	NMGameCode	uGameCode = 0x0200700D;
	std::wstring szMsg;
	BOOL bResult = TRUE;
	
	OutputDebug( "DnNexonService Initialize\n" );
	gs_BugReporter.AddLogW( _T( "DnNexonService Initialize\n" ) );
	_nmman.Init();

	//	[필수] 로케일 설정
	if( bResult )
	{
#if defined(_KR)
		bResult = _nmco.SetLocale( uLocaleId );
#elif defined(_US)
		bResult = _nmco.SetPatchOption( false );
		bResult = _nmco.SetLocaleAndRegion( uLocaleId, uRegionCode );
#endif
		ClientUtil::Log::LogTime( _T("_nmco.SetLocaleAndRegion( %s ): %s!"), ClientUtil::Convert::LocaleIdToString( uLocaleId ), ClientUtil::Convert::ResultToString( bResult ) );
//		gs_BugReporter.AddLogW( _T( "_nmco.SetLocale( %s ): %s!"), ClientUtil::Convert::LocaleIdToString( uLocaleId ), ClientUtil::Convert::ResultToString( bResult ) );
	}
	else
	{
		szMsg = L"NMService intialize failed. ( SetLocale )";
	}

	//	[필수] 메신저 모듈 초기화
	if( bResult )
	{
		bResult = _nmco.Initialize( uGameCode );
	
		bool bFindSID = false;
		UINT uiIndex;

		wchar_t * wszSID =  L"sid:";
		static std::wstring wStrSid; 

		for( uiIndex=0; uiIndex<tokens.size(); uiIndex++ )
		{
			bFindSID = ParsingByString( wszSID , tokens[uiIndex] );

			if( bFindSID )
				break;
		}

		BOOL LogMResult = false;

		if( bFindSID )
		{
			int iLen = (int)_tcslen( wszSID );

			wchar_t wszNumber[256]={0,};
//			memcpy( wszNumber, tokens[uiIndex].c_str()+iLen, ((tokens[uiIndex].length() - iLen) * sizeof(wchar_t)) );
			
			wStrSid = tokens[uiIndex].c_str() + iLen;
			RemoveSpaceW( wStrSid );
			tokens.clear();

			char szNumber[256]={0,};
			WideCharToMultiByte( CP_ACP, 0, wStrSid.c_str() , -1, szNumber, 256, NULL, NULL );
			gs_BugReporter.SetSID( szNumber );
			LogMResult = NMGameLogManager::Initialize( L"DragonNest", L"http://gamelog.nexon.com/dragonnest/check.html", wStrSid.c_str() );

			bool bCheckNum = false;

			for (int i = 0 ; i < (int)wStrSid.size() ; i++ )
			{
				if( wStrSid[i] >=  0x30 && wStrSid[i] <= 0x39 );
				else
				{
					LogMResult = 0; 
					bCheckNum = true; 
					break; 
				}
			}
		}
		else
		{
//			LPTSTR		szSessionID = NULL;
			WCHAR       wszSessionID[100];
			SecureZeroMemory( wszSessionID, sizeof( wszSessionID ) );

			NMGameLogManager::GetSessionID( wszSessionID );
			LogMResult = NMGameLogManager::Initialize( L"DragonNest", L"http://gamelog.nexon.com/dragonnest/check.html", wszSessionID );
			ErrorLog( "Not SID found." );
			WriteErrorLog_( SID_Not_Found , _T("Not SID found.") );
		}

		if( !LogMResult ) 
		{
//			CString Str;
//			Str.Format(_T("NGM 초기화 에러 sid:%d") ,wStrSid.c_str() );
// 			MessageBox(NULL , Str.GetBuffer() , L"Error" ,MB_OK );
//			HWND hWnd = GetEtDevice()->GetHWnd();
//			SendMessage(hWnd, WM_CLOSE, 0,0);
//			return -1;
			ErrorLog("NMGameLogManager::Initialize() failed.." );
		}
		//blondy end

		ClientUtil::Log::LogTime( _T("_nmco.Initialize( %s ): %s!"), ClientUtil::Convert::GameCodeToString( uGameCode ), ClientUtil::Convert::ResultToString( bResult ) );
	}
	else
	{
		szMsg = L"NMService intialize failed. ( nmco.Initialize )";
	}

	//	[필수] 메신저 이벤트 핸들러 등록
	//  메신저에서 발생하는 각종 이벤트를 받아 볼 수 있도록 이벤트 핸들러를 등록합니다
	if( bResult )
	{
		HWND hWnd = 0;
		if( CDnMainFrame::IsActive() )
		{
			hWnd = CDnMainFrame::GetInstance().GetHWnd();
		}
		
		bResult = _nmco.RegisterCallbackMessage( hWnd, WM_NMEVENT );
		ClientUtil::Log::LogTime( _T("_nmco.RegisterCallbackMessage( 0x%08x, WM_NMEVENT ): %s!"), hWnd, ClientUtil::Convert::ResultToString( bResult ) );
//		gs_BugReporter.AddLogW( _T("_nmco.RegisterCallbackMessage( 0x%08x, WM_NMEVENT ): %s!"), hWnd, ClientUtil::Convert::ResultToString( bResult ) );
	}
	else
	{
		szMsg = L"NMService intialize failed. ( nmco.RegisterCallbackMessage )";
	}

	//	[필수] 인증서버 로그인
	//  이 함수는 동기적으로 실행되므로 함수가 리턴할 때까지 프로세스는 블록됩니다.
	if( !m_bStandAloneMode && bResult )
	{
		bResult = NGMAuthWebLaunchingMode( uLocaleId, uGameCode, szMsg );
	}

	if( bResult == FALSE && !CGlobalInfo::GetInstance().m_bThemida_hsbgen )
	{
#ifdef _FINAL_BUILD 
		//szMsg = "정상적인 경로로 실행해 주시기 바랍니다.";
#endif // _FINAL_BUILD 

		MessageBoxW( 0, szMsg.c_str(), L"DragonNest", 0 );

#if defined(_KR)
		if( g_pServiceSetup )
			g_pServiceSetup->WriteErrorLog_( 8 ,L"INVALID_PATH" );
		ErrorLog( "g_pServiceSetup->WriteErrorLog_(8 ,INVALID_PATH)" );
#endif	// #if defined(_KR)

		HWND hWnd = GetEtDevice()->GetHWnd();
		SendMessage( hWnd, WM_CLOSE, 0, 0 );

		return -1;
	}
#ifdef _HSHIELD
	bResult = InitHackShield();
#endif // _HSHIELD

	return 0;
}

// 게임 서버와의 인증은 샘플로 제작되어 있지 않지만,
// 게임 클라이언트는 여기서 받아온 넥슨 패스포트를 게임 서버로 전송하여 게임 서버와 넥슨 인증 서버 간의 2차 인증을 거친다.
int DnNexonService::Initialize( void* pCustomData )
{
	if( m_bStandAloneMode )
		return 0;

	//	[필수] 게임 서버를 통한 2차 인증
	if( m_szNexonPassPort.empty() == false )
	{
		WCHAR szLocalIP[ 32 ];
		SecureZeroMemory( szLocalIP, sizeof( szLocalIP ) );
		CClientSessionManager::GetInstance().GetLocalIP( szLocalIP );
#if defined(_KR)
		SendCheckLoginKR((WCHAR*)m_szNexonPassPort.c_str(), szLocalIP );
#elif defined(_US)
		SendCheckLoginUS((WCHAR*)m_szNexonPassPort.c_str(), szLocalIP );
#endif // _KR or _US
		DebugLog( "DnNexonService::Initialize(). Send SendKorCheckLogin. " );
		return 0;
	}
	else
	{
		ErrorLog( "DnNexonService::Initialize() m_szNexonPassPort.empty so we not send login." );
	}

	return -1;
}

int DnNexonService::Release()
{
	//	인증서버에서 로그아웃
	if( m_bStandAloneMode )
		_nmco.LogoutAuth();
	else
		_nmco.DetachAuth();
	
	//	메신저 이벤트 핸들러 등록 제거
	HWND hWnd = NULL ; 
	if( CDnMainFrame::IsActive() ) 
	{
		hWnd = CDnMainFrame::GetInstance().GetHWnd();
	}
	_nmco.ResetCallbackMessage( hWnd );
	//	메신저 매니저 종료
	_nmman.Finalize();

	return 0;
}

LRESULT DnNexonService::MsgProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch( message )
	{
	case WM_NMEVENT:	//	메신저 이벤트 처리
		g_msngrEventHandler.HandleNMEvent( wParam, lParam );
		break;
	}

	return 0;
}

int DnNexonService::OnEvent( int nType, void* pExtra )
{
	switch(nType)
	{
	case 0:	// 게임엔진시작
		{
			g_WiseLog.WriteToWiseLog( "gameengin.aspx" );
			InfoLog( "g_WiseLog.WriteToWiseLog() game init." );
		}
		break;
	case 1:	// 게임시작
		{
			g_WiseLog.WriteToWiseLog( "cimov.aspx" );
			InfoLog( "g_WiseLog.WriteToWiseLog() game start." );
		}
		break;
/*	case 2:	// 메신저 서버와 접속 완료 되었으니 캐릭터를 로그인 시키자.
		{
//			TCharListData& CharInfo = CGlobalValue::GetInstance().m_CharListData.CharListData[CGlobalValue::GetInstance().m_nSelectCharIndex];
			TCharListData& CharInfo = m_CharListData.CharListData[m_nSelectCharIndex];
//			_nmco.LoginVirtual( CharInfo.wszCharacterName, int(CharInfo.biUniqueID));
			m_wszCharacterName = CharInfo.wszCharacterName;
			m_UniqueID = CharInfo.biUniqueID ; 

			int iResult = _nmco.CharacterLogin64(CharInfo.wszCharacterName ,m_UniqueID );
			if ( iResult )
			{
				ErrorLog("DnNexonService::OnEvent:;CharacterLogin64 failed  Name %s ID %d ErrorType %d",CharInfo.wszCharacterName , CharInfo.biUniqueID  , iResult );
				//MessageBox(this->m_, _T("메신저 서버에 로그인 할수 없습니다."), NULL, NULL);
				return -1;
			}
		}
		break;*/
	default:
	    break;
	}

	return 0;
}

void DnNexonService::OnDispatchMessage( int iMainCmd, int iSubCmd, char * pData, int iLen )
{
	switch( iMainCmd )
	{
	case SC_LOGIN:
		{
			switch( iSubCmd )
			{
			case eLogin::SC_CHECKVERSION: 
				{
				}
				break;
			case eLogin::SC_SERVERLIST:	// 게임시작
				{
					g_WiseLog.WriteToWiseLog( "server.aspx" );
					InfoLog( "g_WiseLog.WriteToWiseLog() SC_SERVERLIST ." );
					WriteStageLog_( Server_Certification, _T("Server Certification.") );
				}
				break;
			case eLogin::SC_CHARLIST:
				{
					CDnLoginTask *pTask = (CDnLoginTask *)CTaskManager::GetInstance().GetTask( "LoginTask" );
					if( !pTask ) return;
#ifdef PRE_MOD_SELECT_CHAR
					if( pTask->GetSelectServerIndex() != -1)
					{
						m_nServerIndex = pTask->GetSelectServerIndex();
					}
#else // PRE_MOD_SELECT_CHAR
					if( pTask->GetServerIndex() != -1)
					{
						m_nServerIndex = pTask->GetServerIndex();
					}
#endif // PRE_MOD_SELECT_CHAR

					SCCharList *pPacket = (SCCharList*)pData;
					memset(&m_CharListData, 0, sizeof(SCCharList));
//					StaticAssert( sizeof(SCCharList)==(sizeof(char)+sizeof(BYTE)+sizeof(TCharListData)*CHARCOUNTMAX) );
					m_CharListData.nRet = pPacket->nRet;
					m_CharListData.cCharCount = pPacket->cCharCount;
					for( int i = 0; i < pPacket->cCharCount; i++) {
						m_CharListData.CharListData[i] = pPacket->CharListData[i];
					}
					
					g_WiseLog.WriteToWiseLog( "character.aspx" );
					InfoLog( "g_WiseLog.WriteToWiseLog() SC_CHARLIST." );
				}
				break;
			case eLogin::SC_CHANNELLIST:
				{
					g_WiseLog.WriteToWiseLog( "channel.aspx" );

					CDnLoginTask *pTask = (CDnLoginTask *)CTaskManager::GetInstance().GetTask( "LoginTask" );
					if( !pTask ) return;
#ifdef PRE_MOD_SELECT_CHAR
					m_nSelectCharIndex = pTask->GetSelectActorSlot();
					if( m_nSelectCharIndex > -1 )
#else // PRE_MOD_SELECT_CHAR
					m_nSelectCharIndex = pTask->GetSelectExtendActorSlot();
					if( m_nSelectCharIndex > -1 && m_nSelectCharIndex <= m_CharListData.CharListData[m_CharListData.cCharCount-1].cCharIndex )
#endif // PRE_MOD_SELECT_CHAR
					{
/*
						BOOL bResult = _nmco.LoginMessenger();
						if ( !bResult )
						{
							//MessageBox(NULL, _T("메신저 서버에 로그인 할수 없습니다."), NULL, NULL);
							ErrorLog("g_WiseLog.WriteToWiseLog() SC_CHANNELLIST can't connect to login server..");
							return;
						}
*/
						InfoLog( "g_WiseLog.WriteToWiseLog() SC_CHARLIST." );
					}
				}
				break;
			}
		}
	case SC_SYSTEM:
		{
			switch( iSubCmd )
			{
			case eSystem::SC_CONNECTVILLAGE:
				WriteStageLog_( InVillage, _T("in village") );					
				break;
			case eSystem::SC_TCP_CONNECT_REQ:
				WriteStageLog_( Ingame, _T("In Game.(PVPGame)") );					
				break;			
			}
			break;
		}
	}
}

bool DnNexonService::WriteStageLog_( INT32 nStage, LPCTSTR szComment )
{
	if( nStage > 100 )	// 100이상은 디파인은 일본 에서 사용한다.
	  return true; 

#ifdef _KR
	if( nStage == IServiceSetup::JoinChannel ) 
	{
		TCharListData *pCharInfo = NULL;

		for( int i=0; i < m_CharListData.cCharCount; i++ ) 
		{
#ifdef PRE_MOD_SELECT_CHAR
			CDnLoginTask *pLoginTask = (CDnLoginTask *)CTaskManager::GetInstance().GetTask( "LoginTask" );
			if( pLoginTask && m_CharListData.CharListData[i].biCharacterDBID == pLoginTask->GetSelectCharacterDBID( m_nSelectCharIndex ) )
#else // PRE_MOD_SELECT_CHAR
			if( m_CharListData.CharListData[i].cCharIndex == m_nSelectCharIndex )
#endif // PRE_MOD_SELECT_CHAR
			{
				pCharInfo = &m_CharListData.CharListData[i];
			}
		}
		if( pCharInfo == NULL ) return false;

		m_eGameState = eGameState::eStart;

		m_wszCharacterName = pCharInfo->wszCharacterName;
		m_UniqueID = pCharInfo->biUniqueID ; 

		// 캐릭터 로그인 
		int iResult = _nmco.CharacterLogin64( m_wszCharacterName.c_str(), m_UniqueID, m_nServerIndex );
		if ( iResult )
		{
			std::string strCharName;
			ToMultiString( m_wszCharacterName, strCharName );
			ErrorLog( "DnNexonService::OnEvent:;CharacterLogout64 failed  Name %s ID %d ErrorType %d", strCharName.c_str(), m_UniqueID, iResult );
//			MessageBox(this->m_, _T("메신저 서버에 로그인 할수 없습니다."), NULL, NULL);
//			return false;
		}
		return true; 
	}
	else if( nStage == IServiceSetup::ExitChannel )
	{
		m_eGameState = eGameState::eEnd;
		// 캐릭터 로그 아웃이 일어 났다 .. 
		int iResult = _nmco.CharacterLogout64( m_wszCharacterName.c_str(), m_UniqueID, m_nServerIndex );
		if ( iResult )
		{
			std::string strCharName;
			ToMultiString( m_wszCharacterName, strCharName );
			ErrorLog( "DnNexonService::OnEvent:;CharacterLogout64 failed  Name %s ID %d ErrorType %d", strCharName.c_str(), m_UniqueID, iResult );
//			MessageBox(this->m_, _T("메신저 서버에 로그인 할수 없습니다."), NULL, NULL);
			return false;
		}
		return true; 
	}
	else if( nStage == IServiceSetup::Game_Exit )
	{
		if( m_eGameState == eGameState::eStart )
		{
			_nmco.CharacterLogout64( m_wszCharacterName.c_str(), m_UniqueID, m_nServerIndex );
		}
	}
#endif // _KR

	if( NMGameLogManager::WriteStageLog( nStage, szComment ) )
	{
		DebugLog( "NMGameLogManager::WriteStageLog( %d ) OK. ", nStage );
		return true;
	}
	else
		DebugLog( "NMGameLogManager::WriteStageLog( %d ) Failed. ", nStage );

	return false;
}

bool DnNexonService::WriteErrorLog_( INT32 nStage, LPCTSTR szComment )
{
	if( NMGameLogManager::WriteErrorLog( nStage, szComment ) )
	{
		DebugLog( "NMGameLogManager::WriteErrorLog( %d ) OK. ", nStage );
		return true;
	}
	else
		DebugLog( "NMGameLogManager::WriteErrorLog( %d ) Failed. ", nStage );

	return false;
}

bool DnNexonService::ParsingByString( LPCTSTR szComment, std::wstring wsString )
{
	int iLen = (int)_tcslen(szComment);

	if( iLen <= 0 )
		return false;

	for( int i = 0;i< iLen ;i++ )
	{
		if( szComment[i] !=  (wsString.c_str())[i] )
			return false;
	}

	return true;
}

int DnNexonService::DeleteChracter( int SelectIndex )
{
	TCharListData *pCharInfo = NULL;
	
	for( int i=0; i<m_CharListData.cCharCount; i++ ) 
	{
#ifdef PRE_MOD_SELECT_CHAR
		CDnLoginTask *pLoginTask = (CDnLoginTask *)CTaskManager::GetInstance().GetTask( "LoginTask" );
		if( pLoginTask && m_CharListData.CharListData[i].biCharacterDBID == pLoginTask->GetSelectCharacterDBID( SelectIndex ) )
#else // PRE_MOD_SELECT_CHAR
		if( m_CharListData.CharListData[i].cCharIndex == SelectIndex )
#endif // PRE_MOD_SELECT_CHAR
			pCharInfo = &m_CharListData.CharListData[i];
	}
	if( pCharInfo == NULL ) return -1;
	
	return _nmco.CharacterRemove64( pCharInfo->wszCharacterName, pCharInfo->biUniqueID, m_nServerIndex ); 
}

int DnNexonService::ReviveCharacter( int nSlotIndex )
{
	return _nmco.CharacterSync64( m_wszCharacterName.c_str(), m_UniqueID, m_nServerIndex );
}

#if defined(_US)
void DnNexonService::ShowNexonWebpage()
{
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

	std::wstring wUrl = L"http://dragonnest.nexon.net/";

	HKEY hkey;
	LONG ReturnValue = RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\IEXPLORE.EXE", 0, KEY_ALL_ACCESS, &hkey);
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

	CreateProcess (NULL, (LPTSTR)command.c_str(), NULL, NULL, true, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi);
}
#endif // _US

//	-	커맨드라인으로부터 패스포트 스트링을 추출해 냅니다.
//	-	아래 코드는 "GameClient.exe PassportString" 형식으로, 
//	-	클라이언트 경로 뒷 부분에 한칸 띄우고 바로 패스포트 스트링이 넘어오는 경우를 가정하고 있습니다.
//	-	일반적으로는 위 형식을 가장 많이 쓰지만 커맨드라인을 통한 패스포트 전달 형식은 게임팀에서 지정하는 것이 가능하므로,
//	-	특정 형식이 필요한 경우 플랫폼개발팀으로 요청하도록 합니다.
BOOL DnNexonService::NGMAuthWebLaunchingMode( NMLOCALEID uLocaleId, NMGameCode uGameCode, std::wstring& szMsg )
{
	BOOL bResult = FALSE;
	// nmconew.dll에서 테스트서버경로 저장된 .cfg파일을 읽으려면 COM객체를 써야하기 때문에 호출함
#if _WIN32_WINNT >= 0x0400 & defined(_ATL_FREE_THREADED)
	CoInitializeEx( NULL, COINIT_MULTITHREADED );
#else
	CoInitialize( NULL );
#endif

	switch ( uLocaleId )
	{
#if defined(_US)
		case kLocaleID_US:
#endif // _US
		case kLocaleID_KR:
			{
				if( !m_szPassPort.empty() )
				{
//					MessageBoxW( 0, m_szPassPort.c_str(), 0, 0 );
					NMLoginAuthReplyCode resultAuth = _nmco.AttachAuth( m_szPassPort.c_str() );

					if( resultAuth == kLoginAuth_OK )
					{
						ClientUtil::Log::LogTime( _T("_nmco.AttachAuth( \"%s\" ): OK!"), m_szPassPort.c_str() );
						DebugLog( "AttachAuth Ok" );
						WriteStageLog_( Server_Certification, _T("Server Certification.") ); 
//						gs_BugReporter.AddLogW( _T("_nmco.AttachAuth( \"%s\" ): OK!"), m_szPassPort.c_str() );
						bResult = TRUE;
					}
					else
					{
						ClientUtil::Log::LogTime( _T("_nmco.AttachAuth( \"%s\" ): Failed!!!"), m_szPassPort.c_str() );
						ClientUtil::Log::LogInfo( _T("    Result: 0x%08x, \"%s\""), resultAuth, ClientUtil::Convert::AuthResultToString( resultAuth ) );
						DebugLog( " Result %d , %s",resultAuth, ClientUtil::Convert::AuthResultToString( resultAuth ) );
//						gs_BugReporter.AddLogW( _T("_nmco.AttachAuth( \"%s\" ): Failed!!!"), m_szPassPort.c_str() );
//						gs_BugReporter.AddLogW( _T("    Result: 0x%08x, \"%s\""), resultAuth, ClientUtil::Convert::AuthResultToString( resultAuth ) );
						bResult = FALSE;

						std::wstring wszMsg = ClientUtil::Convert::AuthResultToString( resultAuth );

						szMsg = L"NMService intialize failed. ( AttachAuth )\n";
						szMsg += wszMsg;
					}
				}
				else
				{
					ClientUtil::Log::LogTime( _T( "GetCommandLine is null" ) );
					gs_BugReporter.AddLogW( _T( "GetCommandLine is null" ) );
					bResult = FALSE;
					szMsg = L"NMService intialize failed. ( GetCommandLine is null )";

					ErrorLog( "m_szPassPort is NULL" );
				}

				if( bResult )
				{
					// 리얼유저 정보 획득
					_nmco.GetMyInfo();
					UINT32 nMyOid = _nmco.GetMyOid();
					// 와이즈 로그 시작
#ifndef _US	// 미국은 WiseLog사용하지 않음
					g_WiseLog.Start( nMyOid );
#endif // _US

					// 리얼유저 정보 표시
					ClientUtil::Log::LogInfo( _T("    NexonSN: 0x%08x"), _nmco.GetMyOid() );
					ClientUtil::Log::LogInfo( _T("    NexonID: %s"), _nmco.GetMyLoginId() );
					ClientUtil::Log::LogInfo( _T("    Sex: %s"), ClientUtil::Convert::SexTypeToString( _nmco.GetMySex() ) );
					ClientUtil::Log::LogInfo( _T("    Age: %d"), _nmco.GetMyAge() );

					DebugLog( "NexonID: %s", _nmco.GetMyLoginId() );
					DebugLog( "Age: %d", _nmco.GetMyAge() );

//					gs_BugReporter.AddLogW( _T("    NexonSN: 0x%08x"), _nmco.GetMyOid() );
//					gs_BugReporter.AddLogW( _T("    NexonID: %s"), _nmco.GetMyLoginId() );
//					gs_BugReporter.AddLogW( _T("    Sex: %s"), ClientUtil::Convert::SexTypeToString( _nmco.GetMySex() ) );
//					gs_BugReporter.AddLogW( _T("    Age: %d"), _nmco.GetMyAge() );

#ifdef _HSHIELD
					HShieldSetup::SetMyID( _nmco.GetMyLoginId() );
#endif
					//	넥슨패스포트 얻기
					TCHAR szNexonPassport[ NXPASSPORT_SIZE ] ={0,};
					ClientUtil::Log::LogInfo( _T("    Nexon Passport: %s"), _nmco.GetNexonPassport( szNexonPassport ) );
//					gs_BugReporter.AddLogW( _T("    Nexon Passport: %s"), _nmco.GetNexonPassport( szNexonPassport ) );

					m_szNexonPassPort = szNexonPassport;
				}
			}
			break;
		default:
			{
				ClientUtil::Log::LogTime( _T("_nmco.LoginAuth(): Failed!!!") );
				ClientUtil::Log::LogInfo( _T("    Not supported game code: %d"), uGameCode );

				gs_BugReporter.AddLogW( _T("_nmco.LoginAuth(): Failed!!!") );
				gs_BugReporter.AddLogW( _T("    Not supported game code: %d"), uGameCode );

				szMsg = L"NMService intialize failed. ( LoginAuth )";
				bResult = FALSE;
			}
			break;
	}
	
	CoUninitialize();

	return bResult;
}

BOOL DnNexonService::NGMAuthStandAloneMode( std::wstring strNexonID, std::wstring strPassword, std::wstring& szMsg )
{
	BOOL bResult = FALSE;
	// nmconew.dll에서 테스트서버경로 저장된 .cfg파일을 읽으려면 COM객체를 써야하기 때문에 호출함
#if _WIN32_WINNT >= 0x0400 & defined(_ATL_FREE_THREADED)
	CoInitializeEx( NULL, COINIT_MULTITHREADED );
#else
	CoInitialize( NULL );
#endif

	NMLoginAuthReplyCode resultAuth = _nmco.LoginAuth( strNexonID.c_str(), strPassword.c_str() );

	if( resultAuth == kLoginAuth_OK )
	{
		ClientUtil::Log::LogTime( _T("_nmco.LoginAuth : OK!") );
		DebugLog( "LoginAuth Ok" );
		WriteStageLog_( Server_Certification, _T("Server Certification.") ); 
		bResult = TRUE;
	}
	else
	{
		ClientUtil::Log::LogTime( _T("_nmco.LoginAuth : Failed!!!") );
		ClientUtil::Log::LogInfo( _T("    Result: 0x%08x, \"%s\""), resultAuth, ClientUtil::Convert::AuthResultToString( resultAuth ) );
		DebugLog( " Result %d , %s",resultAuth, ClientUtil::Convert::AuthResultToString( resultAuth ) );
		bResult = FALSE;

		szMsg = ClientUtil::Convert::AuthResultToString( resultAuth );
	}

	if( bResult )
	{
		//	리얼유저 정보 획득
		_nmco.GetMyInfo();
		UINT32 nMyOid = _nmco.GetMyOid();
		// 와이즈 로그 시작
#ifndef _US	// 미국은 WiseLog사용하지 않음
		g_WiseLog.Start( nMyOid );
#endif // _US

		//	리얼유저 정보 표시
		ClientUtil::Log::LogInfo( _T("    NexonSN: 0x%08x"), _nmco.GetMyOid() );
		ClientUtil::Log::LogInfo( _T("    NexonID: %s"), _nmco.GetMyLoginId() );
		ClientUtil::Log::LogInfo( _T("    Sex: %s"), ClientUtil::Convert::SexTypeToString( _nmco.GetMySex() ) );
		ClientUtil::Log::LogInfo( _T("    Age: %d"), _nmco.GetMyAge() );

		DebugLog( "NexonID: %s", _nmco.GetMyLoginId() );
		DebugLog( "Age: %d", _nmco.GetMyAge() );

#ifdef _HSHIELD
		HShieldSetup::SetMyID( _nmco.GetMyLoginId() );
#endif
		//	넥슨패스포트 얻기
		TCHAR szNexonPassport[ NXPASSPORT_SIZE ] ={0,};
		ClientUtil::Log::LogInfo( _T("    Nexon Passport: %s"), _nmco.GetNexonPassport( szNexonPassport ) );

		m_szNexonPassPort = szNexonPassport;
	}

	CoUninitialize();

	return bResult;
}

void DnNexonService::LogOutAuth()
{
	_nmco.LogoutAuth();
	m_szNexonPassPort = L"";
}

#ifdef _HSHIELD
BOOL DnNexonService::InitHackShield()
{
	// 핵쉴드 초기화전
	g_WiseLog.WriteToWiseLog( "hackinistart.aspx" );
	// 핵쉴드 초기화
	int nResult = 0;
	nResult = HShieldSetup::Init( GetEtDevice()->GetHWnd() );
	if( nResult < 0 )
	{
		HShieldSetup::Stop();
		MessageBoxW( 0, STR_HS_INITIALIZED_FAIL, L"DragonNest", 0 );
		WriteStageLog_( SecurityModule_Failed, _T("Hack Shield not initialized!") );

		HWND hWnd = GetEtDevice()->GetHWnd();
		SendMessage( hWnd, WM_CLOSE, 0, 0 );
		return FALSE;
	}
#if defined(_DEBUG_HSHIELD)
	DebugLog( "Skiped HackSheild update." );
#else

	nResult = HShieldSetup::Update();
	if( nResult < 0 )
	{
		HShieldSetup::Stop();
		MessageBoxW( 0, STR_HS_UPDATE_FAIL, L"DragonNest", 0 );
		WriteStageLog_( SecurityModule_Failed, _T("Hack Shield not initialized!") );
		HWND hWnd = GetEtDevice()->GetHWnd();
		SendMessage( hWnd, WM_CLOSE, 0, 0 );
		return FALSE;
	}
#endif //_DEBUG_HSHIELD
	nResult = HShieldSetup::Start();
	if( nResult < 0 )	
	{
		HShieldSetup::Stop();
		MessageBoxW( 0, STR_HS_START_FAIL, L"DragonNest", 0 );
		WriteStageLog_( SecurityModule_Failed, _T("Hack Shield not initialized!") );
		HWND hWnd = GetEtDevice()->GetHWnd();
		SendMessage( hWnd, WM_CLOSE, 0, 0 );
		return FALSE;
	}

	// 핵쉴드 초기화후
	g_WiseLog.WriteToWiseLog( "hackiniend.aspx" );
	return TRUE;
}
#endif //_HSHIELD

#endif // _KR, _US
