#include "NMManager.h"
#include "NMCOHelpers.h"


BOOL NMCOHelpers::MinimizeAll
(
	BOOL				bMinimize
)
{
	CNMMinimizeAllFunc	func( bMinimize );
	return				CNMManager::GetInstance().CallNMFunc( &func );
}

BOOL NMCOHelpers::IsGuestIDPassport
(
	LPCTSTR				szPassport
 )
{
	CNMIsGuestIDPassportFunc	func( szPassport );
	return				CNMManager::GetInstance().CallNMFunc( &func );
}

BOOL NMCOHelpers::WriteToWiselog
(
 LPCTSTR				szUrl		// 1024
 )
{
	CNMWriteToWiselogFunc	func( szUrl );
	return				CNMManager::GetInstance().CallNMFunc( &func );
}

LPCTSTR NMCOHelpers::GetCommonPath
( 
	LPTSTR		szPath				// MAX_PATH
)
{
	CNMGetPathFunc	func( CNMGetPathFunc::kCommon );
	
	if ( CNMManager::GetInstance().CallNMFunc( &func ) )
	{
		NMSAFE_STRCOPY( szPath, func.szPath, MAX_PATH );
		return	szPath;
	}

	szPath[ 0 ]		= NULL;
	return NULL;
}

LPCTSTR NMCOHelpers::GetMessengerPath
( 
	LPTSTR		szPath				// MAX_PATH
)
{
	CNMGetPathFunc	func( CNMGetPathFunc::kMessengerClient );
	
	if ( CNMManager::GetInstance().CallNMFunc( &func ) )
	{
		NMSAFE_STRCOPY( szPath, func.szPath, MAX_PATH );
		return szPath;
	}

	szPath[ 0 ]		= NULL;
	return NULL;
}

LPCTSTR NMCOHelpers::GetBannerPath
(
	LPTSTR		szBannerDir			// MAX_PATH
)
{
	NMCOHelpers::GetMessengerPath( szBannerDir );
	_tcscat( szBannerDir, _T( "\\Banner" ) );

	return szBannerDir;
}

LPCTSTR NMCOHelpers::GetBannerFileName
(
	LPCTSTR szBannerUrl
,	LPTSTR szFileNameOut
)
{
	const TCHAR * pChar = NULL;
	
	NMCOHelpers::GetBannerPath( szFileNameOut );
	_tcscat( szFileNameOut, _T( "\\" ) );

	if ( ( pChar = ::_tcsrchr( szBannerUrl, _T( '/' ) ) ) != NULL )
	{
		_tcscat( szFileNameOut, pChar + 1 );
	}
	else
	{
		_tcscat( szFileNameOut, szBannerUrl );
	}

	return szFileNameOut;
}

LPCTSTR NMCOHelpers::GetGameButtonPath
(
 LPTSTR		szGameButtonDir			// MAX_PATH
 )
{
	NMCOHelpers::GetMessengerPath( szGameButtonDir );
	_tcscat( szGameButtonDir, _T( "\\GameButton" ) );

	return szGameButtonDir;
}

LPCTSTR NMCOHelpers::GetGameButtonFileName
(
 LPCTSTR szGameButtonUrl
 ,	LPTSTR szFileNameOut
 )
{
	const TCHAR * pChar = NULL;

	NMCOHelpers::GetGameButtonPath( szFileNameOut );
	_tcscat( szFileNameOut, _T( "\\" ) );

	if ( ( pChar = ::_tcsrchr( szGameButtonUrl, _T( '/' ) ) ) != NULL )
	{
		_tcscat( szFileNameOut, pChar + 1 );
	}
	else
	{
		_tcscat( szFileNameOut, szGameButtonUrl );
	}

	return szFileNameOut;
}

void NMCOHelpers::MakeExecDir
(
	LPCTSTR		szExecPath			// MAX_PATH
)
{
	TCHAR	szDirName[ MAX_PATH ];

	_SInt32_ nLen = static_cast<INT32>( _tcslen( szExecPath ) );
	_SInt32_ index = 0;

	for ( _SInt32_ i = 0; i < nLen; i++ )
	{
		if ( szExecPath[ i ] == _T( '\\') )
		{
			szDirName[ index ] = 0;
			::CreateDirectory( szDirName, NULL );
		}
		szDirName[ index++ ] = szExecPath[ i ];
	}
}

BOOL NMCOHelpers::ExecutePatcher
(	
	LPCTSTR		szUrl				// 256
,	LPCTSTR		szPatchDir			// 256
,	LPCTSTR		szExecCmdLine		// 256
,	BOOL		bWaitParentExit		// = TRUE
,	BOOL		bWaitPatchEnd )		// = FALSE
{
	CNMExecutePatcherFunc	func( szUrl, szPatchDir, szExecCmdLine, bWaitParentExit, bWaitPatchEnd );
	return CNMManager::GetInstance().CallNMFunc( &func );
}

BOOL NMCOHelpers::ExecuteLauncher
(	
	_UInt32_	uGameCode
,	_UInt32_	uLaunchType
,	LPCTSTR		szArgument			// = NULL, 256
,	BOOL		bWaitParent			// = FALSE
)
{
	CNMExecuteLauncherFunc	func( uGameCode, uLaunchType, szArgument, bWaitParent );
	return		CNMManager::GetInstance().CallNMFunc( &func );
}

BOOL NMCOHelpers::ExecuteNGMPatcher
(	
	_UInt32_	uGameCode			// = kGameCode_NULL
,	LPCTSTR		szUrl				// 256
,	LPCTSTR		szPatchDir			// 256
,	LPCTSTR		szExecCmdLine		// 1024
,	BOOL		bWaitParentExit		// = TRUE
,	BOOL		bSmallDialog		// = FALSE
,	BOOL		bUseLocalNGM		// = FALSE
,	LPCTSTR		szSessionID			// = NULL, 32
,	_UInt32_	uPatchType			// = kPatchType_Normal
)
{
	CNMExecuteNGMPatcherFunc	func( uGameCode, szUrl, szPatchDir, szExecCmdLine, bWaitParentExit, bSmallDialog, bUseLocalNGM, szSessionID, uPatchType );
	return CNMManager::GetInstance().CallNMFunc( &func );
}

BOOL NMCOHelpers::ExecuteNGMLauncher
(	
	_UInt32_	uGameCode
,	_UInt32_	uLaunchType
,	LPCTSTR		szArgument			// = NULL, 1024
,	BOOL		bWaitParent			// = FALSE
,	LPCTSTR		szPassport			// = NULL, 1024
,	BOOL		bUseLocalNGM		// = FALSE
,	LPCTSTR		szStickPath			// = NULL, 8
)
{
	CNMExecuteNGMLauncherFunc	func( uGameCode, uLaunchType, szArgument, bWaitParent, szPassport, bUseLocalNGM, szStickPath );
	return		CNMManager::GetInstance().CallNMFunc( &func );
}

BOOL NMCOHelpers::ExecuteNGMInstaller
(	
	_UInt32_	uGameCode
,	BOOL		bWaitParent			// = FALSE
,	BOOL		bLaunchAfterInstall	// = FALSE
,	LPCTSTR		szArgument			// = NULL, 1024
,	LPCTSTR		szPassport			// = NULL, 1024
,	LPCTSTR		szSessionID			// = NULL, 32
)
{
	CNMExecuteNGMInstallerFunc	func( uGameCode, bWaitParent, bLaunchAfterInstall, szArgument, szPassport, szSessionID );
	return		CNMManager::GetInstance().CallNMFunc( &func );
}

BOOL NMCOHelpers::IsNGMInstalled
(
	void
)
{
	CNMIsNGMInstalledFunc	func;

	if ( CNMManager::GetInstance().CallNMFunc( &func ) )
	{
		return func.bInstalled;
	}

	return FALSE;
}

BOOL NMCOHelpers::GetUrlFromPage
(	
	NMURLPAGE	uPage
,	LPTSTR		szUrlOut		// 1024
,	_UInt32_	uParam			// = 0,		for kURL_MemberPopup_OID, kURL_MoneyRefillForWebGame
,	LPCTSTR		szParam			// = NULL,	for kURL_MemberPopup_LoginID, kURL_WriteMail_Target
,	BOOL		bLoginDirect	// = TRUE
,	BOOL		bIsPopup		// = FALSE
,	BOOL		bNoActiveX		// = FALSE
,	UINT32		uMaskGameCode	// = kGameCode_nexoncom
)
{
	if ( szUrlOut != NULL )
	{
		CNMGetUrlExFunc		func;
		func.uType			= kURLType_Page;
		func.uPage			= uPage;
		func.uParam			= uParam;
		if ( szParam != NULL )
		{
			NMSAFE_STRCOPY( func.szParam, szParam, 256 );
		}
		func.bLoginDirect	= bLoginDirect;
		func.bIsPopup		= bIsPopup;
		func.bNoActiveX		= bNoActiveX;
		func.uMaskGameCode	= uMaskGameCode;

		if ( CNMManager::GetInstance().CallNMFunc( &func ) )
		{
			NMSAFE_STRCOPY( szUrlOut, func.szUrlOut, 1024 );
			return TRUE;
		}
	}
	return FALSE;
}

BOOL NMCOHelpers::GetUrlFromGameCode
(	
	_UInt32_	uGameCode
,	LPTSTR		szUrlOut		// 1024
,	BOOL		bLoginDirect	// = TRUE
,	BOOL		bIsPopup		// = FALSE 
,	BOOL		bNoActiveX		// = FALSE
,	UINT32		uMaskGameCode	// = kGameCode_nexoncom
)
{
	if ( szUrlOut != NULL )
	{
		CNMGetUrlExFunc		func;
		func.uType			= kURLType_Game;
		func.uGameCode		= uGameCode;
		func.bLoginDirect	= bLoginDirect;
		func.bIsPopup		= bIsPopup;
		func.bNoActiveX		= bNoActiveX;
		func.uMaskGameCode	= uMaskGameCode;

		if ( CNMManager::GetInstance().CallNMFunc( &func ) )
		{
			NMSAFE_STRCOPY( szUrlOut, func.szUrlOut, 1024 );
			return TRUE;
		}
	}
	return FALSE;
}

BOOL NMCOHelpers::GetURLFromGuildKey
(	
	NMGuildKey	uGuildKey
,	LPTSTR		szUrlOut		// 1024
,	BOOL		bLoginDirect	// = TRUE
,	BOOL		bIsPopup		// = FALSE 
,	BOOL		bNoActiveX		// = FALSE
,	UINT32		uMaskGameCode	// = kGameCode_nexoncom
)
{
	if ( szUrlOut != NULL )
	{
		CNMGetUrlExFunc		func;
		func.uType			= kURLType_Guild;
		func.uGuildKey		= uGuildKey;
		func.bLoginDirect	= bLoginDirect;
		func.bIsPopup		= bIsPopup;
		func.bNoActiveX		= bNoActiveX;
		func.uMaskGameCode	= uMaskGameCode;

		if ( CNMManager::GetInstance().CallNMFunc( &func ) )
		{
			NMSAFE_STRCOPY( szUrlOut, func.szUrlOut, 1024 );
			return TRUE;
		}
	}
	return FALSE;
}

BOOL NMCOHelpers::GetLoginDirectUrl
(	
	LPCTSTR		szUrl			// 512
,	LPTSTR		szUrlOut		// 1024
,	BOOL		bIsPopup		// = FALSE
,	BOOL		bNoActiveX		// = FALSE
,	UINT32		uMaskGameCode	// = kGameCode_nexoncom
)	
{
	if ( szUrl != NULL && szUrlOut != NULL )
	{
		CNMGetUrlExFunc		func;
		func.uType			= kURLType_DirectURL;
		NMSAFE_STRCOPY( func.szUrl, szUrl, 512 );
		func.bLoginDirect	= TRUE;
		func.bIsPopup		= bIsPopup;
		func.bNoActiveX		= bNoActiveX;
		func.uMaskGameCode	= uMaskGameCode;

		if ( CNMManager::GetInstance().CallNMFunc( &func ) )
		{
			NMSAFE_STRCOPY( szUrlOut, func.szUrlOut, 1024 );
			return TRUE;
		}
	}
	return FALSE;
}

BOOL NMCOHelpers::GetSessionValue
(	
	LPCTSTR		szName			// 32
,	LPTSTR		szValue			// 256
)
{
	CNMGetSessionValueFunc	func( szName );
	
	if ( CNMManager::GetInstance().CallNMFunc( &func ) )
	{
		NMSAFE_STRCOPY( szValue, func.szValue, 256 );
		return TRUE;
	}

	return FALSE;
}

BOOL NMCOHelpers::SetSessionValue
(	
	LPCTSTR		szName			// 32
,	LPCTSTR		szValue			// 256, = NULL, NULL : delete
,	BOOL		bAttachToProcess// = FALSE 
)	
{
	CNMSetSessionValueFunc	func( szName, szValue, bAttachToProcess );
	return CNMManager::GetInstance().CallNMFunc( &func );
}

BOOL NMCOHelpers::BringForwardStandAloneMsg
(
	LPCTSTR		szDomain
)
{
	CNMBringForwardStandAloneMsgFunc
							func( szDomain );
	return CNMManager::GetInstance().CallNMFunc( &func );
}

BOOL NMCOHelpers::DownloadFile
(
	LPCTSTR		szUrl
,	LPCTSTR		szFileName
)
{
	CNMDownloadFileFunc	func( szUrl, szFileName );

	return	CNMManager::GetInstance().CallNMFunc( &func );
}

BOOL NMCOHelpers::UploadFile
(
	LPCTSTR			szUrl
,	LPCTSTR			szFileName
,	LPCTSTR			szPostField
,	LPTSTR			szResponse
,	BOOL			bAppendPassport
)
{
	CNMUploadFileFunc	func( szUrl, szFileName, szPostField, bAppendPassport );

	if ( CNMManager::GetInstance().CallNMFunc( &func ) )
	{
		NMSAFE_STRCOPY( szResponse, func.szResponse, 1024 );
		return TRUE;
	}

	szResponse[ 0 ]		= NULL;
	return FALSE;
}

BOOL NMCOHelpers::GetSupportGameList
(
	NMGameCodeList&		aGameCodeList
)
{
	CNMGetSupportGameListFunc	func( FALSE );

	if ( CNMManager::GetInstance().CallNMFunc( &func ) )
	{
		aGameCodeList	= func.aGameCodeList;
		return TRUE;
	}

	return FALSE;
}

BOOL NMCOHelpers::GetGameServerList
(
	NMGameCode				uGameCode
,	NMGameServerCodeList&	aGameServerCodeList
)
{
	CNMGetGameServerListFunc	func( uGameCode );

	if ( CNMManager::GetInstance().CallNMFunc( &func ) )
	{
		aGameServerCodeList	= func.aGameServerCodeList;
		return TRUE;
	}

	return FALSE;
}

BOOL NMCOHelpers::GetGameFullName
(
	NMGameCode				uGameCode
,	LPTSTR					szGameFullName
)
{
	CNMGetGameFullNameFunc		func( uGameCode );

	if ( CNMManager::GetInstance().CallNMFunc( &func ) )
	{
		NMSAFE_STRCOPY( szGameFullName, func.szGameFullName, 128 );
		return TRUE;
	}

	return FALSE;
}

BOOL NMCOHelpers::GetGameShortName
(
	NMGameCode				uGameCode
,	LPTSTR					szGameShortName
)
{
	CNMGetGameShortNameFunc		func( uGameCode );

	if ( CNMManager::GetInstance().CallNMFunc( &func ) )
	{
		NMSAFE_STRCOPY( szGameShortName, func.szGameShortName, 128 );
		return TRUE;
	}

	return FALSE;
}

BOOL NMCOHelpers::GetGameFriendTitle
(
	NMGameCode				uGameCode
,	LPTSTR					szGameFriendTitle
)
{
	CNMGetGameFriendTitleFunc		func( uGameCode );

	if ( CNMManager::GetInstance().CallNMFunc( &func ) )
	{
		NMSAFE_STRCOPY( szGameFriendTitle, func.szGameFriendTitle, 128 );
		return TRUE;
	}

	return FALSE;
}

BOOL NMCOHelpers::GetGameServerName
(
	NMGameCode				uGameCode
,	NMGameServerCode		uGameServerCode
,	LPTSTR					szGameServerName
 )
{
	CNMGetGameServerNameFunc		func( uGameCode, uGameServerCode );

	if ( CNMManager::GetInstance().CallNMFunc( &func ) )
	{
		NMSAFE_STRCOPY( szGameServerName, func.szGameServerName, 128 );
		return TRUE;
	}

	return FALSE;
}

BOOL NMCOHelpers::DownloadGuildMarkFile
(
	NMGameCode	uGameCode
,	NMGuildKey	uGuildKey
,	LPCTSTR		szDownloadDir
)
{
	CNMDownloadGuildMarkFileFunc func( uGameCode, uGuildKey, szDownloadDir );

	return CNMManager::GetInstance().CallNMFunc( &func );
}