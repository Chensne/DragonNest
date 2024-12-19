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
				case kLoginAuth_ServiceShutdown:			return _T( "���� ���񽺰� �Ͻ������� �ߴܵǾ����ϴ�. Ȩ������ ���������� Ȯ���� �ּ���." );
				case kLoginAuth_BlockedIP:					return _T( "���ѵ� IP�ּ� �Դϴ�. �����ǿ� ������ �ּ���." );
				case kLoginAuth_NotAllowedLocale:			return _T( "���� ������ ���� �ʽ��ϴ�. �����ǿ� ������ �ּ���." );
				case kLoginAuth_ServerFailed:				return _T( "���������� ������ �� �����ϴ�. ��� �Ŀ� �ٽ� �õ��� �ּ���." );
				case kLoginAuth_WrongID:					return _T( "�߸��� ���̵� �Դϴ�. �ٽ� Ȯ�� �� �α��� �� �ּ���." );
				case kLoginAuth_WrongPassword:				return _T( "��й�ȣ�� Ʋ�Ƚ��ϴ�. �ٽ� Ȯ�� �� �α��� �� �ּ���." );
				case kLoginAuth_WrongOwner:					return _T( "���� ���̵�� ���� �ƴ��� Ȯ�εǾ� ����Ͻ� �� �����ϴ�." );
				case kLoginAuth_WithdrawnUser:				return _T( "���� ���̵�� Ż���� ���̵�� Ȯ�εǾ� ����Ͻ� �� �����ϴ�." );
				case kLoginAuth_UserNotExists:				return _T( "�������� �ʴ� ���̵� �Դϴ�. �ٽ� Ȯ�� �� �α��� �� �ּ���." );
				case kLoginAuth_TempBlockedByLoginFail:		return _T( "�߸��� ��� ��ȣ�Դϴ�. ���̵� ���� ��ȣ�� ���� ��� �α����� �����մϴ�." );
				case kLoginAuth_TempBlockedByWarning:		return _T( "���� ���Ͽ� ���̵� ����� ���ѵǾ����ϴ�." );
				case kLoginAuth_BlockedByAdmin:				return _T( "�����ڿ� ���� ���̵� ����� �������� �Ǿ����ϴ�." );
				case kLoginAuth_NotAllowedServer:			return _T( "���� ������ �����߽��ϴ�. ��� �Ŀ� �ٽ� �õ��� �ּ���." );
				case kLoginAuth_InvalidPassport:			return _T( "�н���Ʈ ������ �����Ͽ����ϴ�. ��� �Ŀ� �ٽ� �õ��� �ּ���." );
				case kLoginAuth_ModuleNotInitialized:		return _T( "�޽��� ����� �ʱ�ȭ���� �ʾҽ��ϴ�." );
				case kLoginAuth_ModuleInitializeFailed:		return _T( "�޽��� ��� �ʱ�ȭ�� �����߽��ϴ�." );
				default:									return _T( "�α��ο� �����߽��ϴ�." );
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
				case CNMAuthConnectionClosedEvent::kType_Disconnected:		return _T( "�ٸ� ���ǿ� ���� �α׾ƿ� �Ǿ����ϴ�." );
				case CNMAuthConnectionClosedEvent::kType_InvalidPassport:	return _T( "�ؽ��н���Ʈ�� ���� �ʽ��ϴ�." );
				case CNMAuthConnectionClosedEvent::kType_InvalidUserIP:		return _T( "IP �ּҰ� �ؽ��н���Ʈ ������ ��ġ���� �ʽ��ϴ�." );
				case CNMAuthConnectionClosedEvent::kType_NetworkError:		return _T( "��Ʈ��ũ ���ῡ �����߽��ϴ�." );
				default:													return _T( "���񽺰� �����Ǿ����ϴ�." );
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
				case kMessengerReplyNewUser:				return _T( "�޽����� �����߽��ϴ�" );
				case kMessengerReplyWrongId:				return _T( "���̵� Ʋ�Ƚ��ϴ�. �ٽ� Ȯ�� �� �α��� �� �ּ���." );
				case kMessengerReplyWrongPwd:				return _T( "��й�ȣ�� Ʋ�Ƚ��ϴ�. �ٽ� Ȯ�� �� �α��� �� �ּ���." );
				case kMessengerReplyWrongClientVersion:		return _T( "�޽��� ������ ���� �ʽ��ϴ�." );
				case kMessengerReplyWrongMsgVersion:		return _T( "�޽��� ������ ���� �ʽ��ϴ�." );
				case kMessengerReplyServiceShutdown:		return _T( "���� ������ �Դϴ�. ���� �ð��� ���� �� �̿��� �ּ���. ���� �ð��� Ȩ�������� ������ �ּ���." );
				case kMessengerReplyLockedByAnotherProcess:	return _T( "�̹� �ٸ� ���̵�� �ؽ� ���������� �α��� ���Դϴ�." );
				case kMessengerReplyWrongOwner:				return _T( "���� ���̵�� ���� �ƴ��� Ȯ�εǾ� ����Ͻ� �� �����ϴ�." );
				case kMessengerReplyBlockByAdmin:			return _T( "�����ڿ� ���� ���̵� ����� �������� �Ǿ����ϴ�." );
				case kMessengerReplyTempBlockByWarning:		return _T( "���� ���Ͽ� ���̵� ����� ���ѵǾ����ϴ�." );
				case kMessengerReplyTempBlockByLoginFail:	return _T( "�߸��� ��� ��ȣ�Դϴ�. ���̵� ���� ��ȣ�� ���� ��� �α����� �����մϴ�." );
				case kMessengerReplyBlockedIp:				return _T( "Ư�� IP �� �ü�������� �α����� �� �� �����ϴ�. �ڼ��� ������ �����Ϳ� �����ϼ���." );
				case kMessengerReplyNotAuthenticated:		return _T( "���� ������ ���ӵǾ� ���� �ʽ��ϴ�." );
				default:									return _T( "�޽��� ������ ������ �� �����ϴ�." );
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
				case CNMMsgConnectionClosedEvent::kType_ByError:	return _T( "�޽��������� ������ ���������ϴ�." );
				case CNMMsgConnectionClosedEvent::kType_ByServer:	return _T( "�޽��������� ������ ���������ϴ�." );
				default:											return _T( "�޽��������� ������ ���������ϴ�." );
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
			case kSex_Female:	return _T("����");
			case kSex_Male:		return _T("����");
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