#include "stdafx.h"
#include "DnOptionData.h"

//////////////////////////////////////////////////////////////////////////
// CDnOptionData Class
//////////////////////////////////////////////////////////////////////////

CDnOptionData::CDnOptionData()
: m_nWidth( 1280 )
, m_nHeight( 800 )
, m_nGraphicQuality( 0 )
, m_nTextureQuality( 0 )
, m_bWindow( FALSE )
, m_bVSync( FALSE )
, m_bAutoGameStart( FALSE )
#ifdef _USE_PARTITION_SELECT
, m_nSelectChannelNum( 0 )
, m_nSelectPartitionId( 0 )
#endif // _USE_PARTITION_SELECT
{
}

CDnOptionData::~CDnOptionData()
{
	m_vecOptionData.clear();
}

CDnOptionData& CDnOptionData::GetInstance()
{
	static CDnOptionData global;
	return global;
}

BOOL CDnOptionData::SaveConfigOption( HWND hWnd )
{
	FILE *stream = NULL;
	TCHAR szFilename[MAX_PATH] = {0,};
	TCHAR szPath[_MAX_PATH]={0,};
	SHGetSpecialFolderPath( hWnd, szPath, CSIDL_PERSONAL, 0 );

	swprintf_s( szFilename, L"%s\\DragonNest", szPath );
	_wmkdir( szFilename );
	swprintf_s( szFilename, L"%s\\DragonNest\\Config", szPath );
	_wmkdir( szFilename );
	swprintf_s( szFilename, L"%s\\DragonNest\\Config\\%s", szPath,  L"Config.ini" );

	CString szInstallPath = szFilename;

	if( _wfopen_s( &stream, szInstallPath.GetString() , L"w+" ) != 0 )
		return FALSE;

	fseek( stream, 0L, SEEK_SET );

	fprintf( stream, " Width %d\n ", m_nWidth );
	fprintf( stream, "Height %d\n ", m_nHeight );
	fprintf( stream, "GraphicQuality %d\n ", m_nGraphicQuality );
	fprintf( stream, "TextureQuality %d\n ", m_nTextureQuality );
	fprintf( stream, "WindowMode %d\n ", m_bWindow ? 1 : 0  );
	fprintf( stream, "VSync %d\n ", m_bVSync ? 1 : 0  );

	std::vector<stOptionData>::iterator iter = m_vecOptionData.begin();
	for( ; iter != m_vecOptionData.end(); iter++ )
		fprintf( stream, "%s %s\n ", (*iter).m_strOptionField.c_str(), (*iter).m_strOptionValue.c_str() );

	fclose( stream );

	return TRUE;
}

BOOL CDnOptionData::LoadConfigOption( HWND hWnd )
{
	FILE *stream = NULL;
	char szString[256]={0,};
	TCHAR szFilename[MAX_PATH]={0,};
	TCHAR szPath[_MAX_PATH]={0,};
	SHGetSpecialFolderPath( hWnd, szPath, CSIDL_PERSONAL, 0 );

	swprintf_s( szFilename, L"%s\\DragonNest", szPath );
	_wmkdir( szFilename );
	swprintf_s( szFilename, L"%s\\DragonNest\\Config", szPath );
	_wmkdir( szFilename );
	swprintf_s( szFilename, L"%s\\DragonNest\\Config\\%s", szPath,  L"Config.ini" );

	CString szInstallPath = szFilename;

	if( _wfopen_s( &stream, szInstallPath.GetString(), L"r+" ) != 0 )
		return FALSE;

	m_vecOptionData.clear();
	fseek( stream, 0L, SEEK_SET );

	for( int i = 0; ;i++ )
	{
		int numRead = fscanf_s( stream, "%s", szString, 256 );
		if( numRead != 1 ) break;
		if( !SetOptionValue( stream , szString ) )	// 해상도, 품질, 창모드 제외 나머지 옵션은 아래에서 저장
		{
			stOptionData OptionData;
			OptionData.m_strOptionField = szString;
			fscanf_s( stream, "%s", szString, 256 );
			OptionData.m_strOptionValue = szString;
			m_vecOptionData.push_back( OptionData );
		}
	}

	fclose( stream );

	if( m_nWidth < 1024 )
	{
		m_nWidth = 1024;
		m_nHeight = 768;
	}

	return TRUE;
}
#ifdef _USE_PARTITION_SELECT
BOOL CDnOptionData::SavePartitionOption( HWND hWnd )
{
	FILE *stream = NULL;
	TCHAR szFilename[MAX_PATH] = {0,};
	CString szInstallPath = DNPATCHINFO.GetClientPath();

	TCHAR szPath[_MAX_PATH]={0,};
	SHGetSpecialFolderPath( hWnd, szPath, CSIDL_PERSONAL, 0 );

	swprintf_s( szFilename, L"%s\\DragonNest", szPath );
	_wmkdir( szFilename );
	swprintf_s( szFilename, L"%s\\DragonNest\\Config", szPath );
	_wmkdir( szFilename );
	swprintf_s( szFilename, L"%s\\DragonNest\\Config\\%s", szPath,  L"Partition.ini" );

	szInstallPath = szFilename;

	if( _wfopen_s( &stream, szInstallPath.GetString(), L"w+" ) != 0 )
		return FALSE;

	fseek( stream, 0L, SEEK_SET );

	fprintf( stream, " SelectChannelNum %d\n ", m_nSelectChannelNum  );
	fprintf( stream, "SelectPartitionId %d\n ", m_nSelectPartitionId  );

	fclose( stream );

	return TRUE;
}

BOOL CDnOptionData::LoadPartitionOption( HWND hWnd )
{
	FILE *stream = NULL;
	char szString[256]={0,};
	TCHAR szFilename[MAX_PATH]={0,};
	CString szInstallPath =  DNPATCHINFO.GetClientPath();

	TCHAR szPath[_MAX_PATH]={0,};
	SHGetSpecialFolderPath( hWnd, szPath, CSIDL_PERSONAL, 0 );

	swprintf_s( szFilename, L"%s\\DragonNest", szPath );
	_wmkdir( szFilename );
	swprintf_s( szFilename, L"%s\\DragonNest\\Config", szPath );
	_wmkdir( szFilename );
	swprintf_s( szFilename, L"%s\\DragonNest\\Config\\%s", szPath,  L"Partition.ini" );

	szInstallPath = szFilename;

	if( _wfopen_s( &stream, szInstallPath.GetString(), L"r+" ) != 0 )
		return FALSE;

	fseek( stream, 0L, SEEK_SET );

	fscanf_s( stream, "%s", szString, 256 );
	if( strcmp("SelectChannelNum",szString ) == 0 )
		fscanf_s( stream, "%d", &(m_nSelectChannelNum) );

	fscanf_s( stream, "%s", szString, 256 );
	if( strcmp("SelectPartitionId",szString ) == 0 )
		fscanf_s( stream, "%d", &(m_nSelectPartitionId) );

	fclose( stream );

	return TRUE;
}
#endif // _USE_PARTITION_SELECT
BOOL CDnOptionData::SetOptionValue( FILE *stream, char * szString )
{
	if( !szString)
		return false;

	if( strcmp( "Width", szString ) == 0 )
	{
		fscanf_s( stream, "%d", &(m_nWidth) );
		return TRUE;
	}
	else if( strcmp( "Height", szString ) == 0 )
	{
		fscanf_s( stream, "%d", &(m_nHeight) );
		return TRUE;
	}
	else if( strcmp( "GraphicQuality", szString ) == 0 )
	{
		fscanf_s( stream, "%d", &(m_nGraphicQuality) );
		return TRUE;
	}
	else if( strcmp( "TextureQuality", szString ) == 0 )
	{
		fscanf_s( stream, "%d", &(m_nTextureQuality) );
		return TRUE;
	}
	else if( strcmp( "WindowMode", szString ) == 0 )
	{
		int iWindowMode = 0;
		fscanf_s( stream, "%d", &(iWindowMode) );
		m_bWindow = iWindowMode?true:false;
		return TRUE;
	}
	else if( strcmp( "VSync", szString ) == 0 )
	{
		int iVSync = 0;
		fscanf_s( stream, "%d", &(iVSync) );
		m_bVSync = iVSync?true:false;
		return TRUE;
	}
	return FALSE;
}

