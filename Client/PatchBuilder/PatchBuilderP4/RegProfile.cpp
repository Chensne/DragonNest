#include "StdAfx.h"
#include "RegProfile.h"
#include "SundriesFunc.h"

CRegProfile::CRegProfile(void)
{
}

CRegProfile::~CRegProfile(void)
{
}

void CRegProfile::LoadProfile()
{
	int i;
	DWORD dwProfileCount = 0;
	SProfile CurProfile;
	char szProfile[ 1024 ], szString[ 1024 ];

	sprintf_s( szProfile, 1024, "%s\\Profile", REG_SUBKEY );
	GetRegistryNumber( HKEY_CURRENT_USER, szProfile, "ProfileCount", dwProfileCount );
	for( i = 0; i < ( int )dwProfileCount; i++ )
	{
		CurProfile.szP4Url = "";
		CurProfile.szP4UrlFolder = "";
		CurProfile.szP4UrlServer = "";
		CurProfile.szP4UrlServerFolder = "";
		CurProfile.szP4Workspace = "";
		CurProfile.szPatchFolder = "";
		CurProfile.szRMakeCmd = "";
		CurProfile.szCountryCode = "";
		CurProfile.szManualPatchExe = "";

		sprintf_s( szProfile, 1024, "%s\\Profile\\Profile %d", REG_SUBKEY, i );
		if( GetRegistryString( HKEY_CURRENT_USER, szProfile, "ProfileName", szString ) == ERROR_SUCCESS )
		{
			CurProfile.szProfileName = szString;
		}
		if( GetRegistryString( HKEY_CURRENT_USER, szProfile, "P4Url", szString ) == ERROR_SUCCESS )
		{
			CurProfile.szP4Url = szString;
		}
		if( GetRegistryString( HKEY_CURRENT_USER, szProfile, "P4UrlFolder", szString ) == ERROR_SUCCESS )
		{
			CurProfile.szP4UrlFolder = szString;
		}
		if( GetRegistryString( HKEY_CURRENT_USER, szProfile, "P4UrlServer", szString ) == ERROR_SUCCESS )
		{
			CurProfile.szP4UrlServer = szString;
		}
		if( GetRegistryString( HKEY_CURRENT_USER, szProfile, "P4UrlServerFolder", szString ) == ERROR_SUCCESS )
		{
			CurProfile.szP4UrlServerFolder = szString;
		}
		if( GetRegistryString( HKEY_CURRENT_USER, szProfile, "P4Workspace", szString ) == ERROR_SUCCESS )
		{
			CurProfile.szP4Workspace = szString;
		}
		if( GetRegistryString( HKEY_CURRENT_USER, szProfile, "PatchFolder", szString ) == ERROR_SUCCESS )
		{
			CurProfile.szPatchFolder = szString;
		}
		if( GetRegistryString( HKEY_CURRENT_USER, szProfile, "RMakeCmd", szString ) == ERROR_SUCCESS )
		{
			CurProfile.szRMakeCmd = szString;
		}
		else
		{
			CurProfile.szRMakeCmd = "\\\\192.168.0.11\\tooldata\\Utility\\RMakeCmd.exe";
		}

		if( GetRegistryString( HKEY_CURRENT_USER, szProfile, "ManuPatchExe", szString ) == ERROR_SUCCESS )
		{
			CurProfile.szManualPatchExe = szString;
		}
		else
		{
			CurProfile.szManualPatchExe = "\\\\192.168.0.11\\tooldata\\Utility\\DnManualUpdater_KR.exe";
		}

		if( GetRegistryString( HKEY_CURRENT_USER, szProfile, "CountryCode", szString ) == ERROR_SUCCESS )
		{
			CurProfile.szCountryCode = szString;
		}

		m_vecProfile.push_back( CurProfile );
	}
}

void CRegProfile::SaveProfile()
{
	int i;
	char szProfile[ 1024 ];

	sprintf_s( szProfile, 1024, "%s\\Profile", REG_SUBKEY );
	SetRegistryNumber( HKEY_CURRENT_USER, szProfile, "ProfileCount", ( DWORD )m_vecProfile.size() );
	for( i = 0; i < ( int )m_vecProfile.size(); i++ )
	{
		sprintf_s( szProfile, 1024, "%s\\Profile\\Profile %d", REG_SUBKEY, i );
		SetRegistryString( HKEY_CURRENT_USER, szProfile, "ProfileName", ( char * )m_vecProfile[ i ].szProfileName.c_str() );
		SetRegistryString( HKEY_CURRENT_USER, szProfile, "P4Url",		( char * )m_vecProfile[ i ].szP4Url.c_str() );
		SetRegistryString( HKEY_CURRENT_USER, szProfile, "P4UrlFolder",		( char * )m_vecProfile[ i ].szP4UrlFolder.c_str() );
		SetRegistryString( HKEY_CURRENT_USER, szProfile, "P4UrlServer",( char * )m_vecProfile[ i ].szP4UrlServer.c_str() );
		SetRegistryString( HKEY_CURRENT_USER, szProfile, "P4UrlServerFolder",( char * )m_vecProfile[ i ].szP4UrlServerFolder.c_str() );
		SetRegistryString( HKEY_CURRENT_USER, szProfile, "P4Workspace",( char * )m_vecProfile[ i ].szP4Workspace.c_str() );
		SetRegistryString( HKEY_CURRENT_USER, szProfile, "PatchFolder", ( char * )m_vecProfile[ i ].szPatchFolder.c_str() );
		SetRegistryString( HKEY_CURRENT_USER, szProfile, "RMakeCmd",	( char * )m_vecProfile[ i ].szRMakeCmd.c_str() );
		SetRegistryString( HKEY_CURRENT_USER, szProfile, "CountryCode", ( char * )m_vecProfile[ i ].szCountryCode.c_str() );
		SetRegistryString( HKEY_CURRENT_USER, szProfile, "ManuPatchExe",( char * )m_vecProfile[ i ].szManualPatchExe.c_str() );
	}
}

SProfile *CRegProfile::GetProfile( int nIndex )
{
	if( nIndex >= ( int )m_vecProfile.size() )
	{
		return NULL;
	}
	return &m_vecProfile[ nIndex ];
}

void CRegProfile::AddProfile( SProfile &AddProfile )
{
	m_vecProfile.push_back( AddProfile );
	SaveProfile();
}

void CRegProfile::DeleteProfile( int nIndex )
{
	char szProfile[ 1024 ];

	sprintf_s( szProfile, 1024, "%s\\Profile", REG_SUBKEY );
	DeleteRegistry( HKEY_CURRENT_USER, szProfile );
	m_vecProfile.erase( m_vecProfile.begin() + nIndex );
	SaveProfile();
}
