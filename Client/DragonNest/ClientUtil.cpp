#include "stdafx.h"
#include "ClientUtil.h"

#if ( defined(_KR) || defined(_US) ) && defined(_AUTH)

#ifndef STRSAFE_NO_DEPRECATE
#define STRSAFE_NO_DEPRECATE
#endif
#include <strsafe.h>

#include <atlsync.h>

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

namespace ClientUtil
{
	//
	//	Logger
	//
	class CLogger
	{
	public:
		enum
		{
			LOG_MESSAGE_BUFFER_SIZE = 1024
		};

	public:
		CLogger()
			: m_hwnd( NULL )
		{
		}
		void Initialize( HWND hWnd )
		{
			this->m_hwnd = hWnd;
		}
		bool IsValid() const
		{
			return ( m_hwnd != NULL );
		}
		HWND GetSafeHwnd() const
		{
			return m_hwnd;
		}
		void LogTime( LPCTSTR pszMessage )
		{
			if ( this->GetSafeHwnd() )
			{
				SYSTEMTIME st;
				GetLocalTime( &st );

				TCHAR szOutout[ LOG_MESSAGE_BUFFER_SIZE ];
				if ( SUCCEEDED( ::StringCchPrintf( szOutout, LOG_MESSAGE_BUFFER_SIZE, _T("%04d/%02d/%02d %02d:%02d:%02d, %s"), st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, pszMessage ) ) )
				{
					CCritSecLock lock( this->m_cs );
					{
						LRESULT lResult = LB_ERR;
						lResult = ::SendMessage( this->GetSafeHwnd(), LB_ADDSTRING, 0, reinterpret_cast<LPARAM>( _T("") ) );
						lResult = ::SendMessage( this->GetSafeHwnd(), LB_ADDSTRING, 0, reinterpret_cast<LPARAM>( szOutout ) );

						if ( lResult != LB_ERR )
						{
							::SendMessage( this->GetSafeHwnd(), LB_SETCURSEL, lResult, 0 );
						}
					}
				}
			}
		}
		void LogInfo( LPCTSTR pszMessage, ... )
		{
			if ( this->GetSafeHwnd() )
			{
				CCritSecLock lock( this->m_cs );
				{
					LRESULT lResult = LB_ERR;
					lResult = ::SendMessage( this->GetSafeHwnd(), LB_ADDSTRING, 0, reinterpret_cast<LPARAM>( pszMessage ) );

					if ( lResult != LB_ERR )
					{
						::SendMessage( this->GetSafeHwnd(), LB_SETCURSEL, lResult, 0 );
					}
				}
			}
		}

	private:
		HWND					m_hwnd;
		ATL::CCriticalSection	m_cs;
	};

	CLogger g_logger;

	BOOL GetPrivateProfileFileName( LPTSTR pszFilePath, DWORD dwFilePathSize )
	{
		TCHAR szPath[ MAX_PATH ];
		if ( 0 < ::GetModuleFileName( NULL, szPath, MAX_PATH ) )
		{
			::PathRemoveFileSpec( szPath );
			::PathAppend( szPath, _T("Client.ini") );
			::StringCchCopy( pszFilePath, dwFilePathSize, szPath );
			return TRUE;
		}
		return FALSE;
	}

	//
	//	General private profile access template
	//
	template< class T >
	T GetPrivateProfileValue( LPCTSTR pszAppName, LPCTSTR pszKeyName, T const & defaultValue )
	{
		TCHAR szFilePath[ MAX_PATH ];
		if ( ClientUtil::GetPrivateProfileFileName( szFilePath, MAX_PATH ) )
		{
			return static_cast<T>( ::GetPrivateProfileInt( pszAppName, pszKeyName, defaultValue, szFilePath ) );
		}
		return defaultValue;
	}

	//
	//	String specialization
	//
	template<>
	CAtlString GetPrivateProfileValue<CAtlString>( LPCTSTR pszAppName, LPCTSTR pszKeyName, CAtlString const & defaultValue )
	{
		enum { VALUE_SIZE = 512 };

		TCHAR szFilePath[ MAX_PATH ];
		if ( ClientUtil::GetPrivateProfileFileName( szFilePath, MAX_PATH ) )
		{
			TCHAR szValue[ VALUE_SIZE ];
			if ( 0 < ::GetPrivateProfileString( pszAppName, pszKeyName, defaultValue, szValue, VALUE_SIZE, szFilePath ) )
			{
				return szValue;
			}
		}
		return defaultValue;
	}

	namespace Settings
	{
		BOOL GetPatchOption()
		{
			return ClientUtil::GetPrivateProfileValue<BOOL>( _T("Settings"), _T("PatchOption"), TRUE );
		}

		BOOL GetUseFriendModuleOption()
		{
			return ClientUtil::GetPrivateProfileValue<BOOL>( _T("Settings"), _T("UseFriendModuleOption"), TRUE );
		}

		BOOL GetUseGuildOnlineInfo()
		{
			return ClientUtil::GetPrivateProfileValue<BOOL>( _T("Settings"), _T("UseGuildOnlineInfo"), TRUE );
		}

		BOOL GetLoginAuthOption()
		{
			return ClientUtil::GetPrivateProfileValue<BOOL>( _T("Settings"), _T("LoginAuthOption"), TRUE );
		}

		NMLOCALEID GetLocaleId()
		{
#if defined(_KR)
			return ClientUtil::GetPrivateProfileValue<NMLOCALEID>( _T("Settings"), _T("LocaleID"), kLocaleID_KR );
#elif defined(_US)
			return ClientUtil::GetPrivateProfileValue<NMLOCALEID>( _T("Settings"), _T("LocaleID"), kLocaleID_US );
#endif
		}

		UINT32 GetRegionCode()
		{
			return ClientUtil::GetPrivateProfileValue<UINT32>( _T("Settings"), _T("RegionCode"), 0 );
		}

		NMGameCode GetGameCode()
		{
			return ClientUtil::GetPrivateProfileValue<NMGameCode>( _T("Settings"), _T("GameCode"), kGameCode_NULL );
		}

		CAtlString GetAuthServerIp()
		{
			return ClientUtil::GetPrivateProfileValue<CAtlString>( _T("Settings"), _T("AuthServerIP"), _T("") );
		}

		UINT16 GetAuthServerPort()
		{
			return ClientUtil::GetPrivateProfileValue<UINT16>( _T("Settings"), _T("AuthServerPort"), 0 );
		}

		CAtlString GetMessengerServerIp()
		{
			return ClientUtil::GetPrivateProfileValue<CAtlString>( _T("Settings"), _T("MessengerServerIP"), _T("") );
		}

		UINT16 GetMessengerServerPort()
		{
			return ClientUtil::GetPrivateProfileValue<UINT16>( _T("Settings"), _T("MessengerServerPort"), 0 );
		}

		CAtlString GetSessionServerIp()
		{
			return ClientUtil::GetPrivateProfileValue<CAtlString>( _T("Settings"), _T("SessionServerIP"), _T("") );
		}

		UINT16 GetSessionServerPort()
		{
			return ClientUtil::GetPrivateProfileValue<UINT16>( _T("Settings"), _T("SessionServerPort"), 0 );
		}

		CAtlString GetNexonId()
		{
			return ClientUtil::GetPrivateProfileValue<CAtlString>( _T("Settings"), _T("NexonID"), _T("") );
		}

		CAtlString GetPassword()
		{
			return ClientUtil::GetPrivateProfileValue<CAtlString>( _T("Settings"), _T("Password"), _T("") );
		}

		CAtlString GetSecondaryNexonId()
		{
			return ClientUtil::GetPrivateProfileValue<CAtlString>( _T("Settings"), _T("SecondaryNexonID"), _T("") );
		}

		CAtlString GetSecondaryPassword()
		{
			return ClientUtil::GetPrivateProfileValue<CAtlString>( _T("Settings"), _T("SecondaryPassword"), _T("") );
		}

		CAtlString GetCharacterId()
		{
			return ClientUtil::GetPrivateProfileValue<CAtlString>( _T("Settings"), _T("CharacterID"), _T("") );
		}

		INT32 GetCharacterSn()
		{
			return ClientUtil::GetPrivateProfileValue<INT32>( _T("Settings"), _T("CharacterSN"), 1 );
		}

		BOOL GetForceCharacterMatchOption()
		{
			return ClientUtil::GetPrivateProfileValue<BOOL>( _T("Settings"), _T("ForceCharacterMatchOption"), TRUE );
		}

		CAtlString GetRequestNewFriendId()
		{
			return ClientUtil::GetPrivateProfileValue<CAtlString>( _T("Settings"), _T("RequestNewFriendID"), _T("") );
		}

		CAtlString GetExternalPassport()
		{
			return ClientUtil::GetPrivateProfileValue<CAtlString>( _T("Settings"), _T("ExternalPassport"), _T("") );
		}
	};

	namespace Convert
	{
		CAtlString BOOLToString( BOOL bValue )
		{
			switch ( bValue )
			{
			case TRUE:				return _T("TRUE");
			case FALSE:				return _T("FALSE");
			default:				return _T("FALSE");
			}
		}

		CAtlString LocaleIdToString( NMLOCALEID uLocaleId )
		{
			switch ( uLocaleId )
			{
			case kLocaleID_KR:		return _T("kLocaleID_KR");
			case kLocaleID_JP:		return _T("kLocaleID_JP");
			case kLocaleID_CN:		return _T("kLocaleID_CN");
			case kLocaleID_TW:		return _T("kLocaleID_TW");
			case kLocaleID_US:		return _T("kLocaleID_US");
			default:				return _T("kLocaleID_Unknown");
			}
		}

		CAtlString GameCodeToString( NMGameCode uGameCode )
		{
			switch ( uGameCode )
			{
			case kGameCode_testgame:		return _T("kGameCode_testgame");
			case kGameCode_bigshot:			return _T("kGameCode_bigshot");
			case kGameCode_bigshot_cn:		return _T("kGameCode_bigshot_cn");
			case kGameCode_kartrider:		return _T("kGameCode_kartrider");
			case kGameCode_karttest:		return _T("kGameCode_karttest");
			case kGameCode_kartrider_cn:	return _T("kGameCode_kartrider_cn");
			case kGameCode_karttest_cn:		return _T("kGameCode_karttest_cn");
			case kGameCode_kartrider_tw:	return _T("kGameCode_kartrider_tw");
			case kGameCode_karttest_tw:		return _T("kGameCode_karttest_tw");
			case kGameCode_kartrider_us:	return _T("kGameCode_kartrider_us");
			case kGameCode_karttest_us:		return _T("kGameCode_karttest_us");
			case kGameCode_nexonbyul:		return _T("kGameCode_nexonbyul");
			case kGameCode_combatarms:		return _T("kGameCode_combatarms");
			case kGameCode_combatarms_us:	return _T("kGameCode_combatarms_us");
			case kGameCode_warrock:			return _T("kGameCode_warrock");
			case kGameCode_lunia:			return _T("kGameCode_lunia");
			case kGameCode_lunia_jp:		return _T("kGameCode_lunia_jp");
			case kGameCode_ninedragons:		return _T("kGameCode_ninedragons");
			case kGameCode_nanaimo:			return _T("kGameCode_nanaimo");
			case kGameCode_elswordtest:		return _T("kGameCode_elswordtest");
			case kGameCode_kickoff:			return _T("kGameCode_kickoff");
			case kGameCode_kickofftest:		return _T("kGameCode_kickofftest");
			case kGameCode_koongpa:			return _T("kGameCode_koongpa");
			case kGameCode_koongpa_cn:		return _T("kGameCode_koongpa_cn");
			case kGameCode_zone4:			return _T("kGameCode_zone4");
			case kGameCode_slapshot:		return _T("kGameCode_slapshot");
			case kGameCode_elsword:			return _T("kGameCode_elsword");
			case kGameCode_talesweaver:		return _T("kGameCode_talesweaver");
			case kGameCode_talesweaver_cn:	return _T("kGameCode_talesweaver_cn");
			case kGameCode_cso:				return _T("kGameCode_cso");
			case kGameCode_csotest:			return _T("kGameCode_csotest");
			case kGameCode_csointernal:		return _T("kGameCode_csointernal");
			case kGameCode_csodevelopment:	return _T("kGameCode_csodevelopment");
			case kGameCode_ca_jp:			return _T("kGameCode_ca_jp");
			case kGameCode_heroes:			return _T("kGameCode_heroes");
			case kGameCode_petcity:			return _T("kGameCode_petcity");
			case kGameCode_trashbuster:		return _T("kGameCode_trashbuster");
			case kGameCode_motoloco:		return _T("kGameCode_motoloco");
			case kGameCode_sugarrush:		return _T("kGameCode_sugarrush");
			case kGameCode_projectmv:		return _T("kGameCode_projectmv");
			case kGameCode_bubblefighter:	return _T("kGameCode_bubblefighter");
			case kGameCode_dragonnest:		return _T("kGameCode_dragonnest");
			default:						return _T("kGameCode_Unknown");
			}
		}

		CAtlString AuthResultToString( NMLoginAuthReplyCode uAuthResult )
		{
#ifdef _KR
			switch ( uAuthResult )
			{
				case kLoginAuth_ServiceShutdown:			return _T( "인증 서비스가 일시적으로 중단되었습니다. 홈페이지 공지사항을 확인해 주세요." );
				case kLoginAuth_BlockedIP:					return _T( "제한된 IP주소 입니다. 고객상담실에 문의해 주세요." );
				case kLoginAuth_NotAllowedLocale:			return _T( "지역 정보가 맞지 않습니다. 고객상담실에 문의해 주세요." );
				case kLoginAuth_ServerFailed:				return _T( "인증서버에 접근할 수 없습니다. 잠시 후에 다시 시도해 주세요." );
				case kLoginAuth_WrongID:					return _T( "잘못된 아이디 입니다. 다시 확인 후 로그인 해 주세요." );
				case kLoginAuth_WrongPassword:				return _T( "비밀번호가 틀렸습니다. 다시 확인 후 로그인 해 주세요." );
				case kLoginAuth_WrongOwner:					return _T( "현재 아이디는 본인 아님이 확인되어 사용하실 수 없습니다." );
				case kLoginAuth_WithdrawnUser:				return _T( "현재 아이디는 탈퇴한 아이디로 확인되어 사용하실 수 없습니다." );
				case kLoginAuth_UserNotExists:				return _T( "존재하지 않는 아이디 입니다. 다시 확인 후 로그인 해 주세요." );
				case kLoginAuth_TempBlockedByLoginFail:		return _T( "잘못된 비밀 번호입니다. 아이디 정보 보호를 위해 잠시 로그인을 제한합니다." );
				case kLoginAuth_TempBlockedByWarning:		return _T( "경고로 인하여 아이디 사용이 제한되었습니다." );
				case kLoginAuth_BlockedByAdmin:				return _T( "관리자에 의해 아이디 사용이 영구정지 되었습니다." );
				case kLoginAuth_NotAllowedServer:			return _T( "서버 인증에 실패했습니다. 잠시 후에 다시 시도해 주세요." );
				case kLoginAuth_InvalidPassport:			return _T( "패스포트 인증에 실패하였습니다. 잠시 후에 다시 시도해 주세요." );
				case kLoginAuth_ModuleNotInitialized:		return _T( "메신저 모듈이 초기화되지 않았습니다." );
				case kLoginAuth_ModuleInitializeFailed:		return _T( "메신저 모듈 초기화에 실패했습니다." );
				default:									return _T( "로그인에 실패했습니다." );
			}
#elif _US
			switch ( uAuthResult )
			{
				case kLoginAuth_ServiceShutdown:			return _T( "The authentication server is temporarily offline. Please check the website for more information." );
				case kLoginAuth_BlockedIP:					return _T( "This is a restricted IP. Please contact Customer Support." );
				case kLoginAuth_NotAllowedLocale:			return _T( "Incorrect area information. Please contact Customer Support." );
				case kLoginAuth_ServerFailed:				return _T( "Cannot access the authentication server. Please try again." );
				case kLoginAuth_WrongID:					return _T( "Incorrect username. Please try again." );
				case kLoginAuth_WrongPassword:				return _T( "Incorrect password. Please try again." );
				case kLoginAuth_WrongOwner:					return _T( "That username doesn't belong to you. You cannot use it." );
				case kLoginAuth_WithdrawnUser:				return _T( "That username is not active. You cannot use it." );
				case kLoginAuth_UserNotExists:				return _T( "That username does not exist. Please try again." );
				case kLoginAuth_TempBlockedByLoginFail:		return _T( "Incorrect password. For security reasons, your login will be restricted for a short duration." );
				case kLoginAuth_TempBlockedByWarning:		return _T( "That username has been temporarily blocked because it received a warning." );
				case kLoginAuth_BlockedByAdmin:				return _T( "That username has been permanently blocked by the administrator." );
				case kLoginAuth_NotAllowedServer:			return _T( "Server authentication failed. Please try again." );
				case kLoginAuth_InvalidPassport:			return _T( "Passport authentication failed. Please try again." );
				case kLoginAuth_ModuleNotInitialized:		return _T( "Failed to reset the messenger module." );
				case kLoginAuth_ModuleInitializeFailed:		return _T( "Failed to initialize the messenger module." );
				default:									return _T( "Failed to login." );
			}
#else
			return _T( "" );
#endif
		}

		CAtlString AuthConnectionClosedEventToString( UINT32 uType )
		{
#ifdef _KR
			switch ( uType )
			{
				case CNMAuthConnectionClosedEvent::kType_Disconnected:		return _T( "다른 세션에 의해 로그아웃 되었습니다." );
				case CNMAuthConnectionClosedEvent::kType_InvalidPassport:	return _T( "넥슨패스포트가 맞지 않습니다." );
				case CNMAuthConnectionClosedEvent::kType_InvalidUserIP:		return _T( "IP 주소가 넥슨패스포트 정보와 일치하지 않습니다." );
				case CNMAuthConnectionClosedEvent::kType_NetworkError:		return _T( "네트워크 연결에 실패했습니다." );
				default:													return _T( "서비스가 중지되었습니다." );
			}
#elif _US
			switch ( uType )
			{
				case CNMAuthConnectionClosedEvent::kType_Disconnected:		return _T( "You've been logged out because a different session is active." );
				case CNMAuthConnectionClosedEvent::kType_InvalidPassport:	return _T( "Incorrect Nexon Passport." );
				case CNMAuthConnectionClosedEvent::kType_InvalidUserIP:		return _T( "Your IP does not match with the Nexon Passport information." );
				case CNMAuthConnectionClosedEvent::kType_NetworkError:		return _T( "Network connection failed." );
				default:													return _T( "Service is not available." );
			}
#else
			return _T( "" );
#endif
		}

		CAtlString MessengerResultToString( NMMessengerReplyCode uMessengerResult )
		{
#ifdef _KR
			switch ( uMessengerResult )
			{
				case kMessengerReplyOK:
				case kMessengerReplyNewUser:				return _T( "메신저에 접속했습니다" );
				case kMessengerReplyWrongId:				return _T( "아이디가 틀렸습니다. 다시 확인 후 로그인 해 주세요." );
				case kMessengerReplyWrongPwd:				return _T( "비밀번호가 틀렸습니다. 다시 확인 후 로그인 해 주세요." );
				case kMessengerReplyWrongClientVersion:		return _T( "메신저 버전이 맞지 않습니다." );
				case kMessengerReplyWrongMsgVersion:		return _T( "메신저 버전이 맍지 않습니다." );
				case kMessengerReplyServiceShutdown:		return _T( "서비스 점검중 입니다. 점검 시간이 끝난 후 이용해 주세요. 점검 시간은 홈페이지를 참고해 주세요." );
				case kMessengerReplyLockedByAnotherProcess:	return _T( "이미 다른 아이디로 넥슨 웹페이지에 로그인 중입니다." );
				case kMessengerReplyWrongOwner:				return _T( "현재 아이디는 본인 아님이 확인되어 사용하실 수 없습니다." );
				case kMessengerReplyBlockByAdmin:			return _T( "관리자에 의해 아이디 사용이 영구정지 되었습니다." );
				case kMessengerReplyTempBlockByWarning:		return _T( "경고로 인하여 아이디 사용이 제한되었습니다." );
				case kMessengerReplyTempBlockByLoginFail:	return _T( "잘못된 비밀 번호입니다. 아이디 정보 보호를 위해 잠시 로그인을 제한합니다." );
				case kMessengerReplyBlockedIp:				return _T( "특정 IP 및 운영체제에서는 로그인을 할 수 없습니다. 자세한 내용은 고객센터에 문의하세요." );
				case kMessengerReplyNotAuthenticated:		return _T( "인증 서버에 접속되어 있지 않습니다." );
				default:									return _T( "메신저 서버에 접속할 수 없습니다." );
			}
#elif _US
			switch ( uMessengerResult )
			{
				case kMessengerReplyOK:
				case kMessengerReplyNewUser:				return _T( "You have logged into the messenger." );
				case kMessengerReplyWrongId:				return _T( "Incorrect username. Please try again." );
				case kMessengerReplyWrongPwd:				return _T( "Incorrect password. Please try again." );
				case kMessengerReplyWrongClientVersion:		return _T( "Messenger version doesn't match." );
				case kMessengerReplyWrongMsgVersion:		return _T( "Messenger version doesn't match." );
				case kMessengerReplyServiceShutdown:		return _T( "The game is currently under maintenance. Please try again later. Visit the Dragon Nest website for more information." );
				case kMessengerReplyLockedByAnotherProcess:	return _T( "You are already logged in to the Nexon website using a different username." );
				case kMessengerReplyWrongOwner:				return _T( "That username doesn't belong to you. You cannot use it." );
				case kMessengerReplyBlockByAdmin:			return _T( "That username has been permanently blocked by the administrator." );
				case kMessengerReplyTempBlockByWarning:		return _T( "That username has been temporarily blocked because it received a warning." );
				case kMessengerReplyTempBlockByLoginFail:	return _T( "Incorrect password. For security reasons, your login will be restricted for a short duration." );
				case kMessengerReplyBlockedIp:				return _T( "You cannot log in from certain IPs or with certain operating systems. Contact Customer Support for more information." );
				case kMessengerReplyNotAuthenticated:		return _T( "You are not logged in to the authentication server." );
				default:									return _T( "Unable to connect to the messenger servers." );
			}
#else
			return _T( "" );
#endif
		}

		CAtlString MessengerConnectionClosedEventToString( UINT32 uType )
		{
#ifdef _KR
			switch ( uType )
			{
				case CNMMsgConnectionClosedEvent::kType_ByError:	return _T( "메신저서버와 연결이 끊어졌습니다." );
				case CNMMsgConnectionClosedEvent::kType_ByServer:	return _T( "메신저서버와 연결이 끊어졌습니다." );
				default:											return _T( "메신저서버와 연결이 끊어졌습니다." );
			}
#elif _US
			switch ( uType )
			{
				case CNMMsgConnectionClosedEvent::kType_ByError:	return _T( "Disconnected from the messenger server." );
				case CNMMsgConnectionClosedEvent::kType_ByServer:	return _T( "Disconnected from the messenger server." );
				default:											return _T( "Disconnected from the messenger server." );
			}
#else
			return _T( "" );
#endif
		}

		CAtlString SexTypeToString( NMSEXTYPE uSexType )
		{
			switch ( uSexType )
			{
			case kSex_Female:	return _T("여자");
			case kSex_Male:		return _T("남자");
			default:
				{
					CAtlString strText;
					strText.Format( _T("%d"), uSexType );
					return strText;
				}
				break;
			}
		}

		CAtlString ResultToString( BOOL bResult )
		{
			return bResult ? _T("OK") : _T("Failed");
		}

		CAtlString ConfirmCodeToString( NMCONFIRMCODE uConfirmCode )
		{
			switch ( uConfirmCode )
			{
			case kConfirmOK:		return _T("kConfirmOK");
			case kConfirmDenied:	return _T("kConfirmDenied");
			case kConfirmLater:		return _T("kConfirmLater");
			default:				return _T("kConfirmUnknown");
			}
		}

		CAtlString RefreshEventTypeToString( INT32 nRefreshEventType )
		{
			switch ( nRefreshEventType )
			{
			case CNMRefreshEvent::kType_NULL:			return _T("kType_NULL");
			case CNMRefreshEvent::kType_MyInfo:			return _T("kType_MyInfo");
			case CNMRefreshEvent::kType_UserDataList:	return _T("kType_UserDataList");
			case CNMRefreshEvent::kType_MyGuildList:	return _T("kType_MyGuildList");
			case CNMRefreshEvent::kType_TempNoteBox:	return _T("kType_TempNoteBox");
			case CNMRefreshEvent::kType_PermNoteBox:	return _T("kType_PermNoteBox");
			case CNMRefreshEvent::kType_GameList:		return _T("kType_GameList");
			default:									return _T("kType_NULL");
			}
		}
	};

	namespace Log
	{
		void InitLog( HWND hWnd )
		{
			g_logger.Initialize( hWnd );
		}

		void LogTime( LPCTSTR pszFormat, ... )
		{
			//if ( g_logger.IsValid() )
			{
				va_list	marker;
				va_start( marker, pszFormat );
				TCHAR szBuffer[ CLogger::LOG_MESSAGE_BUFFER_SIZE ];
				if ( SUCCEEDED( ::StringCchVPrintf( szBuffer, CLogger::LOG_MESSAGE_BUFFER_SIZE, pszFormat, marker ) ) )
				{
					OutputDebugString(szBuffer);
					OutputDebugString(_T("\n"));

					g_logger.LogTime( szBuffer );
				}
			}
		}

		void LogInfo( LPCTSTR pszFormat, ... )
		{
			//if ( g_logger.IsValid() )
			{
				va_list	marker;
				va_start( marker, pszFormat );
				TCHAR szBuffer[ CLogger::LOG_MESSAGE_BUFFER_SIZE ];
				if ( SUCCEEDED( ::StringCchVPrintf( szBuffer, CLogger::LOG_MESSAGE_BUFFER_SIZE, pszFormat, marker ) ) )
				{
					OutputDebugString(szBuffer);
					OutputDebugString(_T("\n"));
					g_logger.LogInfo( szBuffer );
				}
			}
		}

		HWND GetLogWindowHandle()
		{
			return g_logger.GetSafeHwnd();
		}
	};

	void MessageBox( LPCTSTR pszTitle, UINT uType, LPCTSTR pszFormat, ... )
	{
		va_list	marker;
		va_start( marker, pszFormat );
		TCHAR szBuffer[ 1024 ];
		if ( SUCCEEDED( ::StringCchVPrintf( szBuffer, 1024, pszFormat, marker ) ) )
		{
			::MessageBox( NULL, szBuffer, pszTitle, uType );
		}
	}
}

#endif // _KR, _US