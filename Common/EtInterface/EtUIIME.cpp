#include "StdAfx.h"
#include "EtUIIME.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

INPUTCONTEXT* (WINAPI * CEtUIIME::_ImmLockIMC)( HIMC ) = CEtUIIME::Dummy_ImmLockIMC;
BOOL (WINAPI * CEtUIIME::_ImmUnlockIMC)( HIMC ) = CEtUIIME::Dummy_ImmUnlockIMC;
LPVOID (WINAPI * CEtUIIME::_ImmLockIMCC)( HIMCC ) = CEtUIIME::Dummy_ImmLockIMCC;
BOOL (WINAPI * CEtUIIME::_ImmUnlockIMCC)( HIMCC ) = CEtUIIME::Dummy_ImmUnlockIMCC;
BOOL (WINAPI * CEtUIIME::_ImmDisableTextFrameService)( DWORD ) = CEtUIIME::Dummy_ImmDisableTextFrameService;
LONG (WINAPI * CEtUIIME::_ImmGetCompositionStringW)( HIMC, DWORD, LPVOID, DWORD ) = CEtUIIME::Dummy_ImmGetCompositionStringW;
DWORD (WINAPI * CEtUIIME::_ImmGetCandidateListW)( HIMC, DWORD, LPCANDIDATELIST, DWORD ) = CEtUIIME::Dummy_ImmGetCandidateListW;
HIMC (WINAPI * CEtUIIME::_ImmGetContext)( HWND ) = CEtUIIME::Dummy_ImmGetContext;
BOOL (WINAPI * CEtUIIME::_ImmReleaseContext)( HWND, HIMC ) = CEtUIIME::Dummy_ImmReleaseContext;
HIMC (WINAPI * CEtUIIME::_ImmAssociateContext)( HWND, HIMC ) = CEtUIIME::Dummy_ImmAssociateContext;
BOOL (WINAPI * CEtUIIME::_ImmGetOpenStatus)( HIMC ) = CEtUIIME::Dummy_ImmGetOpenStatus;
BOOL (WINAPI * CEtUIIME::_ImmSetOpenStatus)( HIMC, BOOL ) = CEtUIIME::Dummy_ImmSetOpenStatus;
BOOL (WINAPI * CEtUIIME::_ImmGetConversionStatus)( HIMC, LPDWORD, LPDWORD ) = CEtUIIME::Dummy_ImmGetConversionStatus;
HWND (WINAPI * CEtUIIME::_ImmGetDefaultIMEWnd)( HWND ) = CEtUIIME::Dummy_ImmGetDefaultIMEWnd;
UINT (WINAPI * CEtUIIME::_ImmGetIMEFileNameA)( HKL, LPSTR, UINT ) = CEtUIIME::Dummy_ImmGetIMEFileNameA;
UINT (WINAPI * CEtUIIME::_ImmGetVirtualKey)( HWND ) = CEtUIIME::Dummy_ImmGetVirtualKey;
BOOL (WINAPI * CEtUIIME::_ImmNotifyIME)( HIMC, DWORD, DWORD, DWORD ) = CEtUIIME::Dummy_ImmNotifyIME;
BOOL (WINAPI * CEtUIIME::_ImmSetConversionStatus)( HIMC, DWORD, DWORD ) = CEtUIIME::Dummy_ImmSetConversionStatus;
BOOL (WINAPI * CEtUIIME::_ImmSimulateHotKey)( HWND, DWORD ) = CEtUIIME::Dummy_ImmSimulateHotKey;
BOOL (WINAPI * CEtUIIME::_ImmIsIME)( HKL ) = CEtUIIME::Dummy_ImmIsIME;
UINT (WINAPI * CEtUIIME::_GetReadingString)( HIMC, UINT, LPWSTR, PINT, BOOL*, PUINT ) = CEtUIIME::Dummy_GetReadingString; // Traditional Chinese IME
BOOL (WINAPI * CEtUIIME::_ShowReadingWindow)( HIMC, BOOL ) = CEtUIIME::Dummy_ShowReadingWindow; // Traditional Chinese IME
BOOL (APIENTRY * CEtUIIME::_VerQueryValueA)( const LPVOID, LPSTR, LPVOID *, PUINT ) = CEtUIIME::Dummy_VerQueryValueA;
BOOL (APIENTRY * CEtUIIME::_GetFileVersionInfoA)( LPSTR, DWORD, DWORD, LPVOID )= CEtUIIME::Dummy_GetFileVersionInfoA;
DWORD (APIENTRY * CEtUIIME::_GetFileVersionInfoSizeA)( LPSTR, LPDWORD ) = CEtUIIME::Dummy_GetFileVersionInfoSizeA;

int CEtUIIME::s_nRefCount = 0;
HINSTANCE CEtUIIME::s_hDllImm32 = NULL;
HINSTANCE CEtUIIME::s_hDllIme = NULL;
HINSTANCE CEtUIIME::s_hDllVer = NULL;
HIMC CEtUIIME::s_hImcDef = NULL;
CEtUIIME::IMESTATE CEtUIIME::s_ImeState;
bool CEtUIIME::s_bEnableImeSystem = true;
int CEtUIIME::s_nCompCaret = 0;
int CEtUIIME::s_nFirstTargetConv = -1;
float CEtUIIME::s_fCompStringX;
float CEtUIIME::s_fCompStringY;
BYTE CEtUIIME::s_abyCompStringAttr[ MAX_COMPSTRING_SIZE ];
DWORD CEtUIIME::s_adwCompStringClause[ MAX_COMPSTRING_SIZE ];
WCHAR CEtUIIME::s_wszReadingString[ 32 ];
HKL CEtUIIME::s_hklCurrent = NULL;
SCandList CEtUIIME::s_CandList;
bool CEtUIIME::s_bVerticalCand = true;
bool CEtUIIME::s_bShowReadingWindow = false;
bool CEtUIIME::s_bHorizontalReading = true;
CEtUIUniBuffer CEtUIIME::s_CompString;
std::vector< SInputLocale > CEtUIIME::s_vecLocale;
LPWSTR CEtUIIME::s_pwszCurrIndicator = NULL;
WCHAR CEtUIIME::s_aszIndicator[ 5 ][ 3 ] =
{
	L"En",
	L"\x7B80",
	L"\x7E41",
	L"\xAC00",
	L"\x3042",
};
bool CEtUIIME::s_bChineseIME = false;
bool CEtUIIME::s_bInsertOnType = false;

#if defined(DEBUG) || defined(_DEBUG)
bool CEtUIIME::m_bIMEStaticMsgProcCalled = true;
#endif

bool CEtUIIME::s_bHideCaret = false;
bool CEtUIIME::s_bDisableIMEonKillFocus = false;
bool CEtUIIME::s_bRESULTSTR_NotSendComp = false;
bool CEtUIIME::s_bProcess_GCS_COMPCLAUSE = true;
bool CEtUIIME::s_bDisableIMEonIMEControl = false;

CEtUIIME::CEtUIIME( void )
{
	Initialize();
	if( _ImmDisableTextFrameService )
	{
		_ImmDisableTextFrameService( 0 );  // Disable TSF for the current process
	}
	s_bEnableImeSystem = true;
}

CEtUIIME::~CEtUIIME(void)
{
	Uninitialize();
}

void CEtUIIME::Initialize()
{
	++s_nRefCount;

	if( s_hDllImm32 )
	{
		return;
	}

	char szPath[ MAX_PATH + 1 ];

	s_CompString.SetBufferSize( MAX_COMPSTRING_SIZE );
	if( !::GetSystemDirectory( szPath, MAX_PATH + 1 ) )
	{
		return;
	}
	strcat( szPath, IMM32_DLLNAME );
	s_hDllImm32 = LoadLibrary( szPath );
	if( s_hDllImm32 )
	{
		*( FARPROC * )&_ImmLockIMC = GetProcAddress( s_hDllImm32, "ImmLockIMC" );
		*( FARPROC * )&_ImmUnlockIMC = GetProcAddress( s_hDllImm32, "ImmUnlockIMC" );
		*( FARPROC * )&_ImmLockIMCC = GetProcAddress( s_hDllImm32, "ImmLockIMCC" );
		*( FARPROC * )&_ImmUnlockIMCC = GetProcAddress( s_hDllImm32, "ImmUnlockIMCC" );
		*( FARPROC * )&_ImmDisableTextFrameService = GetProcAddress( s_hDllImm32, "ImmDisableTextFrameService" );
		*( FARPROC * )&_ImmGetCompositionStringW = GetProcAddress( s_hDllImm32, "ImmGetCompositionStringW" );
		*( FARPROC * )&_ImmGetCandidateListW = GetProcAddress( s_hDllImm32, "ImmGetCandidateListW" );
		*( FARPROC * )&_ImmGetContext = GetProcAddress( s_hDllImm32, "ImmGetContext" );
		*( FARPROC * )&_ImmReleaseContext = GetProcAddress( s_hDllImm32, "ImmReleaseContext" );
		*( FARPROC * )&_ImmAssociateContext = GetProcAddress( s_hDllImm32, "ImmAssociateContext" );
		*( FARPROC * )&_ImmGetOpenStatus = GetProcAddress( s_hDllImm32, "ImmGetOpenStatus" );
		*( FARPROC * )&_ImmSetOpenStatus = GetProcAddress( s_hDllImm32, "ImmSetOpenStatus" );
		*( FARPROC * )&_ImmGetConversionStatus = GetProcAddress( s_hDllImm32, "ImmGetConversionStatus" );
		*( FARPROC * )&_ImmGetDefaultIMEWnd = GetProcAddress( s_hDllImm32, "ImmGetDefaultIMEWnd" );
		*( FARPROC * )&_ImmGetIMEFileNameA = GetProcAddress( s_hDllImm32, "ImmGetIMEFileNameA" );
		*( FARPROC * )&_ImmGetVirtualKey = GetProcAddress( s_hDllImm32, "ImmGetVirtualKey" );
		*( FARPROC * )&_ImmNotifyIME = GetProcAddress( s_hDllImm32, "ImmNotifyIME" );
		*( FARPROC * )&_ImmSetConversionStatus = GetProcAddress( s_hDllImm32, "ImmSetConversionStatus" );
		*( FARPROC * )&_ImmSimulateHotKey = GetProcAddress( s_hDllImm32, "ImmSimulateHotKey" );
		*( FARPROC * )&_ImmIsIME = GetProcAddress( s_hDllImm32, "ImmIsIME" );
	}

	if( !::GetSystemDirectory( szPath, MAX_PATH + 1 ) )
	{
		return;
	}
	strcat( szPath, VER_DLLNAME );
	s_hDllVer = LoadLibrary( szPath );
	if( s_hDllVer )
	{
		*( FARPROC * )&_VerQueryValueA = GetProcAddress( s_hDllVer, "VerQueryValueA" );
		*( FARPROC * )&_GetFileVersionInfoA = GetProcAddress( s_hDllVer, "GetFileVersionInfoA" );
		*( FARPROC * )&_GetFileVersionInfoSizeA = GetProcAddress( s_hDllVer, "GetFileVersionInfoSizeA" );
	}

	StaticOnCreateDevice();
}

void CEtUIIME::Uninitialize()
{
	--s_nRefCount;
	if( s_nRefCount > 0 )
		return;

	if( s_hDllImm32 )
	{
		_ImmLockIMC = Dummy_ImmLockIMC;
		_ImmUnlockIMC = Dummy_ImmUnlockIMC;
		_ImmLockIMCC = Dummy_ImmLockIMCC;
		_ImmUnlockIMCC = Dummy_ImmUnlockIMCC;
		_ImmDisableTextFrameService = Dummy_ImmDisableTextFrameService;
		_ImmGetCompositionStringW = Dummy_ImmGetCompositionStringW;
		_ImmGetCandidateListW = Dummy_ImmGetCandidateListW;
		_ImmGetContext = Dummy_ImmGetContext;
		_ImmReleaseContext = Dummy_ImmReleaseContext;
		_ImmAssociateContext = Dummy_ImmAssociateContext;
		_ImmGetOpenStatus = Dummy_ImmGetOpenStatus;
		_ImmSetOpenStatus = Dummy_ImmSetOpenStatus;
		_ImmGetConversionStatus = Dummy_ImmGetConversionStatus;
		_ImmGetDefaultIMEWnd = Dummy_ImmGetDefaultIMEWnd;
		_ImmGetIMEFileNameA = Dummy_ImmGetIMEFileNameA;
		_ImmGetVirtualKey = Dummy_ImmGetVirtualKey;
		_ImmNotifyIME = Dummy_ImmNotifyIME;
		_ImmSetConversionStatus = Dummy_ImmSetConversionStatus;
		_ImmSimulateHotKey = Dummy_ImmSimulateHotKey;
		_ImmIsIME = Dummy_ImmIsIME;
		FreeLibrary( s_hDllImm32 );
		s_hDllImm32 = NULL;
	}
	if( s_hDllIme )
	{
		_GetReadingString = Dummy_GetReadingString;
		_ShowReadingWindow = Dummy_ShowReadingWindow;
		FreeLibrary( s_hDllIme );
		s_hDllIme = NULL;
	}
	if( s_hDllVer )
	{
		_VerQueryValueA = Dummy_VerQueryValueA;
		_GetFileVersionInfoA = Dummy_GetFileVersionInfoA;
		_GetFileVersionInfoSizeA = Dummy_GetFileVersionInfoSizeA;
		FreeLibrary( s_hDllVer );
		s_hDllVer = NULL;
	}
}

void CEtUIIME::StaticOnCreateDevice()
{
	s_hImcDef = _ImmGetContext( GetEtDevice()->GetHWnd() );
	_ImmReleaseContext( GetEtDevice()->GetHWnd(), s_hImcDef );
}

bool CEtUIIME::StaticMsgProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	HIMC hImc;

	if( !s_bEnableImeSystem )
	{
		return false;
	}

#if defined(DEBUG) || defined(_DEBUG)
	m_bIMEStaticMsgProcCalled = true;
#endif

	switch( uMsg )
	{
	case WM_ACTIVATEAPP:
		if( wParam )
		{
			UINT i, cKL;
			HKL *phKL;

			cKL = GetKeyboardLayoutList( 0, NULL );
			s_vecLocale.clear();
			phKL = new HKL[ cKL ];
			if( phKL )
			{
				GetKeyboardLayoutList( cKL, phKL );
				for( i = 0; i < cKL; i++ )
				{
					int e;
					SInputLocale Locale;
					bool bBreak;

					if( ( PRIMARYLANGID( LOWORD( phKL[ i ] ) ) == LANG_CHINESE || PRIMARYLANGID( LOWORD( phKL[ i ] ) ) == LANG_JAPANESE ||
						PRIMARYLANGID( LOWORD( phKL[ i ] ) ) == LANG_KOREAN ) && ( !_ImmIsIME( phKL[ i ] ) ) )
					{
						continue;
					}
					bBreak = false;
					for( e = 0; e < ( int )s_vecLocale.size(); e++ )
					{
						if( LOWORD( s_vecLocale[ e ].m_hKL ) == LOWORD( phKL[ i ] ) )
						{
							bBreak = true;
							break;
						}
					}
					if( bBreak )
					{
						break;
					}

					WCHAR wszDesc[ 128 ] = L"";

					Locale.m_hKL = phKL[ i ];
					switch( PRIMARYLANGID( LOWORD( phKL[ i ] ) ) )
					{
					case LANG_CHINESE:
						switch( SUBLANGID( LOWORD( phKL[ i ] ) ) )
						{
						case SUBLANG_CHINESE_SIMPLIFIED:
							wcscpy_s( Locale.m_wszLangAbb, 3, s_aszIndicator[ INDICATOR_CHS ] );
							break;
						case SUBLANG_CHINESE_TRADITIONAL:
							wcscpy_s( Locale.m_wszLangAbb, 3, s_aszIndicator[ INDICATOR_CHT ] );
							break;
						default:
							GetLocaleInfoW( MAKELCID( LOWORD( phKL[ i ] ), SORT_DEFAULT ), LOCALE_SABBREVLANGNAME, wszDesc, 128 );
							Locale.m_wszLangAbb[ 0 ] = wszDesc[ 0 ];
							Locale.m_wszLangAbb[ 1 ] = towlower( wszDesc[ 1 ] );
							Locale.m_wszLangAbb[ 2 ] = L'\0';
							break;
						}
						break;
					case LANG_KOREAN:
						wcscpy_s( Locale.m_wszLangAbb, 3, s_aszIndicator[ INDICATOR_KOREAN ] );
						break;
					case LANG_JAPANESE:
						wcscpy_s( Locale.m_wszLangAbb, 3, s_aszIndicator[ INDICATOR_JAPANESE ] );
						break;         
					default:
						GetLocaleInfoW( MAKELCID( LOWORD( phKL[ i ] ), SORT_DEFAULT ), LOCALE_SABBREVLANGNAME, wszDesc, 128 );
						Locale.m_wszLangAbb[ 0 ] = wszDesc[ 0 ];
						Locale.m_wszLangAbb[ 1 ] = towlower( wszDesc[ 1 ] );
						Locale.m_wszLangAbb[ 2 ] = L'\0';
						break;
					}

					GetLocaleInfoW( MAKELCID( LOWORD( phKL[ i ] ), SORT_DEFAULT ), LOCALE_SLANGUAGE, wszDesc, 128 );
					wcscpy_s( Locale.m_wszLang, 64, wszDesc );
					s_vecLocale.push_back( Locale );
				}
				delete [] phKL;
			}
		}
		break;

	case WM_INPUTLANGCHANGE:
		{
			UINT uLang;

			uLang = GetPrimaryLanguage();
			CheckToggleState();
			if( uLang != GetPrimaryLanguage() )
			{
				s_bInsertOnType = ( GetPrimaryLanguage() == LANG_KOREAN );
			}

			SetupImeApi();
			if( _ShowReadingWindow )
			{
				hImc = _ImmGetContext( GetEtDevice()->GetHWnd() );
				if( hImc )
				{
					_ShowReadingWindow( hImc, false );
					_ImmReleaseContext( GetEtDevice()->GetHWnd(), hImc );
				}
			}
		}
		return true;

	case WM_IME_SETCONTEXT:
		lParam = 0;
		return false;

	case WM_IME_STARTCOMPOSITION:
		ResetCompositionString();
		s_bHideCaret = true;
		return true;

	case WM_IME_COMPOSITION:
		return false;
	}

	return false;
}

void CEtUIIME::SendKey( BYTE nVirtKey )
{
	keybd_event( nVirtKey, 0, 0,               0 );
	keybd_event( nVirtKey, 0, KEYEVENTF_KEYUP, 0 );
}

#define LCID_INVARIANT MAKELCID( MAKELANGID( LANG_ENGLISH, SUBLANG_ENGLISH_US ), SORT_DEFAULT )

DWORD CEtUIIME::GetImeId( UINT uIndex )
{
	static HKL hklPrev = 0;
	static DWORD dwID[ 2 ] = { 0, 0 };

	DWORD dwVerSize;
	DWORD dwVerHandle;
	void *pVerBuffer;
	void *pVerData;
	UINT uVerData;
	char szTmp[ 1024 ];

	if( uIndex >= sizeof( dwID ) / sizeof( dwID[ 0 ] ) )
	{
		return 0;
	}
	if( hklPrev == s_hklCurrent )
	{
		return dwID[ uIndex ];
	}
	hklPrev = s_hklCurrent;

	if( !( ( s_hklCurrent == _CHT_HKL ) || ( s_hklCurrent == _CHT_HKL2 ) || ( s_hklCurrent == _CHS_HKL ) ) )
	{
		dwID[ 0 ] = dwID[ 1 ] = 0;
		return dwID[ uIndex ];
	}

	if( !_ImmGetIMEFileNameA( s_hklCurrent, szTmp, ( sizeof( szTmp ) / sizeof( szTmp[ 0 ] ) ) - 1 ) )
	{
		dwID[ 0 ] = dwID[ 1 ] = 0;
		return dwID[ uIndex ];
	}

	if( !_GetReadingString )
	{
		if( ( CompareStringA( LCID_INVARIANT, NORM_IGNORECASE, szTmp, -1, CHT_IMEFILENAME1, -1 ) != CSTR_EQUAL ) &&
			( CompareStringA( LCID_INVARIANT, NORM_IGNORECASE, szTmp, -1, CHT_IMEFILENAME2, -1 ) != CSTR_EQUAL ) &&
			( CompareStringA( LCID_INVARIANT, NORM_IGNORECASE, szTmp, -1, CHT_IMEFILENAME3, -1 ) != CSTR_EQUAL ) &&
			( CompareStringA( LCID_INVARIANT, NORM_IGNORECASE, szTmp, -1, CHS_IMEFILENAME1, -1 ) != CSTR_EQUAL ) &&
			( CompareStringA( LCID_INVARIANT, NORM_IGNORECASE, szTmp, -1, CHS_IMEFILENAME2, -1 ) != CSTR_EQUAL ) )
		{
			dwID[ 0 ] = dwID[ 1 ] = 0;
			return dwID[ uIndex ];
		}
	}

	dwVerSize = _GetFileVersionInfoSizeA( szTmp, &dwVerHandle );
	if( dwVerSize )
	{
		pVerBuffer = HeapAlloc( GetProcessHeap(), 0, dwVerSize );
		if( pVerBuffer )
		{
			if( _GetFileVersionInfoA( szTmp, dwVerHandle, dwVerSize, pVerBuffer ) )
			{
				if( _VerQueryValueA( pVerBuffer, "\\", &pVerData, &uVerData ) )
				{
					DWORD dwVer;

					dwVer = ( ( VS_FIXEDFILEINFO * )pVerData )->dwFileVersionMS;
					dwVer = ( dwVer & 0x00ff0000 ) << 8 | ( dwVer & 0x000000ff ) << 16;
					if( _GetReadingString 
						||
						( GetLanguage() == LANG_CHT &&
						( dwVer == MAKEIMEVERSION( 4, 2 ) || 
						dwVer == MAKEIMEVERSION( 4, 3 ) || 
						dwVer == MAKEIMEVERSION( 4, 4 ) || 
						dwVer == MAKEIMEVERSION( 5, 0 ) ||
						dwVer == MAKEIMEVERSION( 5, 1 ) ||
						dwVer == MAKEIMEVERSION( 5, 2 ) ||
						dwVer == MAKEIMEVERSION( 6, 0 ) ) ) 
						||
						( GetLanguage() == LANG_CHS &&
						( dwVer == MAKEIMEVERSION( 4, 1 ) ||
						dwVer == MAKEIMEVERSION( 4, 2 ) ||
						dwVer == MAKEIMEVERSION( 5, 3 ) ) )
						)
					{
						dwID[ 0 ] = dwVer | GetLanguage();
						dwID[ 1 ] = ( ( VS_FIXEDFILEINFO * )pVerData )->dwFileVersionLS;
					}
				}
			}
			HeapFree( GetProcessHeap(), 0, pVerBuffer );
		}
	}

	return dwID[ uIndex ];
}

void CEtUIIME::CheckInputLocale()
{
	static HKL hklPrev = 0;

	s_hklCurrent = GetKeyboardLayout( 0 );
	if( hklPrev == s_hklCurrent )
	{
		return;
	}

	hklPrev = s_hklCurrent;
	switch ( GetPrimaryLanguage() )
	{
	case LANG_CHINESE:
		s_bVerticalCand = true;
		switch ( GetSubLanguage() )
		{
		case SUBLANG_CHINESE_SIMPLIFIED:
			s_pwszCurrIndicator = s_aszIndicator[ INDICATOR_CHS ];
			s_bVerticalCand = GetImeId() == 0;
			break;
		case SUBLANG_CHINESE_TRADITIONAL:
			s_pwszCurrIndicator = s_aszIndicator[ INDICATOR_CHT ];
			break;
		default:
			s_pwszCurrIndicator = s_aszIndicator[ INDICATOR_NON_IME ];
			break;
		}
		break;
	case LANG_KOREAN:
		s_pwszCurrIndicator = s_aszIndicator[ INDICATOR_KOREAN ];
		s_bVerticalCand = false;
		break;
	case LANG_JAPANESE:
		s_pwszCurrIndicator = s_aszIndicator[ INDICATOR_JAPANESE ];
		s_bVerticalCand = true;
		break;
	default:
		s_pwszCurrIndicator = s_aszIndicator[ INDICATOR_NON_IME ];
		break;
	}

	if( s_pwszCurrIndicator == s_aszIndicator[ INDICATOR_NON_IME ] )
	{
		WCHAR wszLang[ 5 ];

		GetLocaleInfoW( MAKELCID( LOWORD( s_hklCurrent ), SORT_DEFAULT ), LOCALE_SABBREVLANGNAME, wszLang, 5 );
		s_pwszCurrIndicator[ 0 ] = wszLang[ 0 ];
		s_pwszCurrIndicator[ 1 ] = towlower( wszLang[ 1 ] );
	}
}

void CEtUIIME::CheckToggleState()
{
	bool bIme;
	HIMC hImc;

	CheckInputLocale();
	bIme = _ImmIsIME( s_hklCurrent ) != 0;
	s_bChineseIME = ( GetPrimaryLanguage() == LANG_CHINESE ) && bIme;

	hImc = _ImmGetContext( GetEtDevice()->GetHWnd() );
	if( hImc != NULL )
	{
		if( s_bChineseIME )
		{
			DWORD dwConvMode, dwSentMode;

			_ImmGetConversionStatus( hImc, &dwConvMode, &dwSentMode );
			if( dwConvMode & IME_CMODE_NATIVE )
			{
				s_ImeState = IMEUI_STATE_ON;
			}
			else
			{
				s_ImeState = IMEUI_STATE_ENGLISH;
			}
		}
		else
		{
			if( ( bIme ) && ( _ImmGetOpenStatus( hImc ) != 0 ) )
			{
				s_ImeState = IMEUI_STATE_ON;
			}
			else
			{
				s_ImeState = IMEUI_STATE_OFF;
			}
		}
		_ImmReleaseContext( GetEtDevice()->GetHWnd(), hImc );
	}
	else
	{
		s_ImeState = IMEUI_STATE_OFF;
	}
}

void CEtUIIME::SetIMEMode ( DWORD dwNewConvMode, DWORD dwNewSentMode, BOOL fFlag )
{
	HIMC	hImc;
	DWORD	dwConvMode, dwSentMode;
	BOOL	fRet;

	// Note : 입력 컨텍스트를 가져옵니다.
	//
	hImc = _ImmGetContext(GetEtDevice()->GetHWnd());
	if (hImc)
	{
		// Note : 현재 IME 상태를 가져옵니다.
		//
		_ImmGetConversionStatus ( hImc, &dwConvMode, &dwSentMode );

		// Note : 전달된 옵션을 사용하여 IME 상태를 변경합니다. 
		//
		if (fFlag)
		{
			fRet = _ImmSetConversionStatus ( hImc, dwConvMode | dwNewConvMode, dwSentMode | dwNewSentMode );
			if( dwNewConvMode&IME_CMODE_JAPANESE )
			{
				// Note : 일본어 입력모드에서 항상 직접입력 모드로 시작하기 위해 !을 붙였다.
				//
				_ImmSetOpenStatus ( hImc, !fFlag );
			}
		}
		else
		{
			_ImmSetConversionStatus ( hImc, dwConvMode&~dwNewConvMode, dwSentMode&~dwNewSentMode );
			if( dwNewConvMode&IME_CMODE_JAPANESE )
			{
				_ImmSetOpenStatus ( hImc, fFlag );
			}
		}

		// Note : 입력 컨텍스트를 해제합니다. 
		//
		_ImmReleaseContext ( GetEtDevice()->GetHWnd(), hImc );
	}
}

bool CEtUIIME::IsNativeMode()
{
	HIMC hIMC = _ImmGetContext(GetEtDevice()->GetHWnd());
	if ( hIMC )
	{
		DWORD dwComposition, dwSentence;

		_ImmGetConversionStatus(hIMC, &dwComposition, &dwSentence);
		_ImmReleaseContext ( GetEtDevice()->GetHWnd(), hIMC );

		if ( (dwComposition&IME_CMODE_NATIVE)!=NULL ) return true;
	}

	return false;
}

void CEtUIIME::SetupImeApi()
{
	char szImeFile[ MAX_PATH + 1 ];

	_GetReadingString = NULL;
	_ShowReadingWindow = NULL;
	if( _ImmGetIMEFileNameA( s_hklCurrent, szImeFile, sizeof( szImeFile ) / sizeof( szImeFile[ 0 ] ) - 1 ) == 0 )
	{
		return;
	}

	if( s_hDllIme ) 
	{
		FreeLibrary( s_hDllIme );
	}
	s_hDllIme = LoadLibraryA( szImeFile );
	if( !s_hDllIme )
	{
		return;
	}
	_GetReadingString = ( UINT ( WINAPI * )( HIMC, UINT, LPWSTR, PINT, BOOL*, PUINT ) )( GetProcAddress( s_hDllIme, "GetReadingString" ) );
	_ShowReadingWindow =( BOOL ( WINAPI * )( HIMC, BOOL ) )( GetProcAddress( s_hDllIme, "ShowReadingWindow" ) );
}

void CEtUIIME::ResetCompositionString()
{
	s_nCompCaret = 0;
	s_CompString.SetText( L"" );
	memset( s_abyCompStringAttr, 0, sizeof( s_abyCompStringAttr ) );
}

void CEtUIIME::GetReadingWindowOrientation( DWORD dwId )
{
	s_bHorizontalReading = ( s_hklCurrent == _CHS_HKL ) || ( s_hklCurrent == _CHT_HKL2 ) || ( dwId == 0 );
	if( ( !s_bHorizontalReading ) && ( ( dwId & 0x0000FFFF ) == LANG_CHT ) )
	{
		WCHAR wszRegPath[ MAX_PATH ];
		HKEY hKey;
		DWORD dwVer;
		LONG lRc;

		dwVer = dwId & 0xFFFF0000;
		wcscpy_s( wszRegPath, MAX_PATH, L"software\\microsoft\\windows\\currentversion\\" );
		wcscat_s( wszRegPath, MAX_PATH, ( dwVer >= MAKEIMEVERSION( 5, 1 ) ) ? L"MSTCIPH" : L"TINTLGNT" );
		lRc = RegOpenKeyExW( HKEY_CURRENT_USER, wszRegPath, 0, KEY_READ, &hKey );
		if(lRc == ERROR_SUCCESS)
		{
			DWORD dwSize, dwMapping, dwType;

			dwSize = sizeof( DWORD );
			lRc = RegQueryValueExW( hKey, L"Keyboard Mapping", NULL, &dwType, ( PBYTE )&dwMapping, &dwSize );
			if(lRc == ERROR_SUCCESS)
			{
				if( ( dwVer <= MAKEIMEVERSION( 5, 0 ) && 
					( ( BYTE )dwMapping == 0x22 || ( BYTE )dwMapping == 0x23 ) )
					||
					( ( dwVer == MAKEIMEVERSION( 5, 1 ) || dwVer == MAKEIMEVERSION( 5, 2 ) ) &&
					( BYTE )dwMapping >= 0x22 && ( BYTE )dwMapping <= 0x24 )
					)
				{
					s_bHorizontalReading = true;
				}
			}
			RegCloseKey( hKey );
		}
	}
}

void CEtUIIME::GetPrivateReadingString()
{
	DWORD dwId;

	dwId = GetImeId();
	if( !dwId )
	{
		s_bShowReadingWindow = false;
		return;
	}

	HIMC hImc;

	hImc = _ImmGetContext( GetEtDevice()->GetHWnd() );
	if( !hImc )
	{
		s_bShowReadingWindow = false;
		return;
	}

	DWORD dwReadingStrLen, dwErr;
	WCHAR *pwszReadingStringBuffer, *wstr;
	bool bUnicodeIme;
	INPUTCONTEXT *lpIC;

	dwReadingStrLen = 0;
	dwErr = 0;
	pwszReadingStringBuffer = NULL;
	wstr = NULL;
	bUnicodeIme = false;
	lpIC = NULL;
	if( _GetReadingString )
	{
		UINT uMaxUiLen;
		BOOL bVertical;

		dwReadingStrLen = _GetReadingString( hImc, 0, NULL, ( PINT )&dwErr, &bVertical, &uMaxUiLen );
		if( dwReadingStrLen )
		{
			wstr = pwszReadingStringBuffer = ( LPWSTR )HeapAlloc( GetProcessHeap(), 0, sizeof( WCHAR ) * dwReadingStrLen );
			if( !pwszReadingStringBuffer )
			{
				_ImmReleaseContext( GetEtDevice()->GetHWnd(), hImc );
				return;
			}
			dwReadingStrLen = _GetReadingString( hImc, dwReadingStrLen, wstr, ( PINT )&dwErr, &bVertical, &uMaxUiLen );
		}
		s_bHorizontalReading = !bVertical;
		bUnicodeIme = true;
	}
	else
	{
		LPBYTE p;

		lpIC = _ImmLockIMC( hImc );
		p = 0;
		switch( dwId )
		{
		case IMEID_CHT_VER42:
		case IMEID_CHT_VER43:
		case IMEID_CHT_VER44:
			p = *( LPBYTE * )( ( LPBYTE )_ImmLockIMCC( lpIC->hPrivate ) + 24 );
			if( !p ) 
			{
				break;
			}
			dwReadingStrLen = *( DWORD * )( p + 7 * 4 + 32 * 4 );
			dwErr = *( DWORD * )( p + 8 * 4 + 32 * 4 );
			wstr = ( WCHAR * )( p + 56 );
			bUnicodeIme = true;
			break;

		case IMEID_CHT_VER50:
			p = *( LPBYTE * )( ( LPBYTE )_ImmLockIMCC( lpIC->hPrivate ) + 3 * 4 );
			if( !p )
			{
				break;
			}
			p = *( LPBYTE * )( ( LPBYTE )p + 1 * 4 + 5 * 4 + 4 * 2 );
			if( !p )
			{
				break;
			}
			dwReadingStrLen = *( DWORD * )( p + 1 * 4 + ( 16 * 2 + 2 * 4 ) + 5 * 4 + 16 );
			dwErr = *( DWORD * )( p + 1 * 4 + ( 16 * 2 + 2 * 4 ) + 5 * 4 + 16 + 1 * 4 );
			wstr = ( WCHAR * )( p + 1 * 4 + ( 16 * 2 + 2 * 4 ) + 5 * 4 );
			bUnicodeIme = false;
			break;

		case IMEID_CHT_VER51:
		case IMEID_CHT_VER52:
		case IMEID_CHS_VER53:
			p = *( LPBYTE * )( ( LPBYTE )_ImmLockIMCC( lpIC->hPrivate ) + 4 );
			if( !p )
			{
				break;
			}
			p = *( LPBYTE * )( ( LPBYTE )p + 1 * 4 + 5 * 4 );
			if( !p ) 
			{
				break;
			}
			dwReadingStrLen = *( DWORD * )( p + 1 * 4 + ( 16 * 2 + 2 * 4 ) + 5 * 4 + 16 * 2 );
			dwErr = *( DWORD * )( p + 1 * 4 + ( 16 * 2 + 2 * 4 ) + 5 * 4 + 16 * 2 + 1 * 4 );
			wstr  = ( WCHAR * ) ( p + 1 * 4 + ( 16 * 2 + 2 * 4 ) + 5 * 4 );
			bUnicodeIme = true;
			break;

		case IMEID_CHS_VER41:
			{
				int nOffset;

				if( GetImeId( 1 ) >= 0x00000002 )
				{
					nOffset = 8;
				}
				else
				{
					nOffset = 7;
				}

				p = *( LPBYTE * )( ( LPBYTE )_ImmLockIMCC( lpIC->hPrivate ) + nOffset * 4 );
				if( !p )
				{
					break;
				}
				dwReadingStrLen = *( DWORD * )( p + 7 * 4 + 16 * 2 * 4 );
				dwErr = *( DWORD * )( p + 8 * 4 + 16 * 2 * 4 );
				dwErr = min( dwErr, dwReadingStrLen );
				wstr = ( WCHAR * )( p + 6 * 4 + 16 * 2 * 1 );
				bUnicodeIme = true;
				break;
			}

		case IMEID_CHS_VER42:
			{
				OSVERSIONINFOW osi;
				int nTcharSize;

				osi.dwOSVersionInfoSize = sizeof( OSVERSIONINFOW );
				GetVersionExW( &osi );

				if( osi.dwPlatformId == VER_PLATFORM_WIN32_NT )
				{
					nTcharSize = sizeof( WCHAR );
				}
				else
				{
					nTcharSize = sizeof( char );
				}
				p = *( LPBYTE * )( ( LPBYTE )_ImmLockIMCC( lpIC->hPrivate ) + 1 * 4 + 1 * 4 + 6 * 4 );
				if( !p )
				{
					break;
				}
				dwReadingStrLen = *( DWORD * )( p + 1 * 4 + ( 16 * 2 + 2 * 4 ) + 5 * 4 + 16 * nTcharSize);
				dwErr = *( DWORD * )( p + 1 * 4 + ( 16 * 2 + 2 * 4 ) + 5 * 4 + 16 * nTcharSize + 1 * 4 );
				wstr  = ( WCHAR * ) ( p + 1 * 4 + ( 16 * 2 + 2 * 4 ) + 5 * 4 );
				if( osi.dwPlatformId == VER_PLATFORM_WIN32_NT )
				{
					bUnicodeIme = true;
				}
				else
				{
					bUnicodeIme = false;
				}
			}
		} 
	}

	s_CandList.awszCandidate[ 0 ][ 0 ] = 0;
	s_CandList.awszCandidate[ 1 ][ 0 ] = 0;
	s_CandList.awszCandidate[ 2 ][ 0 ] = 0;
	s_CandList.awszCandidate[ 3 ][ 0 ] = 0;
	s_CandList.dwCount = dwReadingStrLen;
	s_CandList.dwSelection = ( DWORD ) - 1;
	if( bUnicodeIme )
	{
		int i;

		for( i = 0; i < ( int )dwReadingStrLen; i++ )
		{
			if( ( i >= ( int )dwErr ) && ( s_CandList.dwSelection == ( DWORD ) - 1 ) )
			{
				s_CandList.dwSelection = i;
			}
			s_CandList.awszCandidate[ i ][ 0 ] = wstr[ i ];
			s_CandList.awszCandidate[ i ][ 1 ] = 0;
		}
		s_CandList.awszCandidate[ i ][ 0 ] = 0;
	}
	else
	{
		char *p;
		DWORD i, j;

		p = ( char * )wstr;
		for( i = 0, j = 0; i < dwReadingStrLen; i++, j++ )
		{
			WCHAR wszCodePage[ 8 ];
			UINT uCodePage;

			if( ( dwErr <= i ) && ( s_CandList.dwSelection == ( DWORD ) - 1 ) )
			{
				s_CandList.dwSelection = j;
			}
			uCodePage = CP_ACP;
			if( GetLocaleInfoW( MAKELCID( GetLanguage(), SORT_DEFAULT ), LOCALE_IDEFAULTANSICODEPAGE, wszCodePage, sizeof( wszCodePage ) / sizeof( wszCodePage[ 0 ] ) ) )
			{
				uCodePage = wcstoul( wszCodePage, NULL, 0 );
			}
			if( IsDBCSLeadByteEx( uCodePage, p[ i ] ) )
			{
				MultiByteToWideChar( uCodePage, 0, p + i, 2, s_CandList.awszCandidate[ j ], 1 );
			}
			else
			{
				MultiByteToWideChar( uCodePage, 0, p + i, 1, s_CandList.awszCandidate[ j ], 1 );
			}
			if( IsDBCSLeadByteEx( uCodePage, p[ i ] ) )
			{
				i++;
			}
		}
		s_CandList.awszCandidate[ j ][ 0 ] = 0;
		s_CandList.dwCount = j;
	}
	if( !_GetReadingString )
	{
		_ImmUnlockIMCC( lpIC->hPrivate );
		_ImmUnlockIMC( hImc );
		GetReadingWindowOrientation( dwId );
	}
	_ImmReleaseContext( GetEtDevice()->GetHWnd(), hImc );

	if( pwszReadingStringBuffer )
	{
		HeapFree( GetProcessHeap(), 0, pwszReadingStringBuffer );
	}

	if( s_CandList.dwCount > 0 )
	{
		s_bShowReadingWindow = true;
	}
	else
	{
		s_bShowReadingWindow = false;
	}
	if( s_bHorizontalReading )
	{
		int i;

		s_CandList.nReadingError = -1;
		s_wszReadingString[ 0 ] = 0;
		for( i = 0; i < ( int )s_CandList.dwCount; i++ )
		{
			if( s_CandList.dwSelection == i )
			{
				s_CandList.nReadingError = lstrlenW( s_wszReadingString );
			}
			wcscat_s( s_wszReadingString, 32, s_CandList.awszCandidate[ i ] );
		}
	}

	s_CandList.dwPageSize = MAX_CANDLIST;
}