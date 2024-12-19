#include "StdAfx.h"
#include "EtResourceMng.h"
#include "Stream.h"
#include "EtPackingStream.h"
#include "PerfCheck.h"
#include "StringUtil.h"
#include "EtFileSystem.h"
#include <string.h>

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

CEtResourceMng::CEtResourceMng( bool bUsePackingFile, bool bUseDynamic )
{
	ScopeLock<CSyncLock> Lock(m_SyncLock, bUseDynamic);
	m_pFileSystem = NULL;

	m_bUsePackingFile = bUsePackingFile;
	m_bUseDynamic = bUseDynamic;
	if( bUsePackingFile ) {
		m_pFileSystem = new CEtFileSystem( bUseDynamic );
	}

	WCHAR wszTemp[_MAX_PATH] = { 0, };
	::GetCurrentDirectoryW( _MAX_PATH, wszTemp );
	m_wszRootDirectory = wszTemp;

	char szTemp[_MAX_PATH] = { 0, };
	WideCharToMultiByte( CP_ACP, 0, wszTemp, -1, szTemp, _MAX_PATH, NULL, NULL );
	AddResourcePath( szTemp );

	m_nCodePage = 949;
	m_bUseMapAccessSuccessFileName = true;
}

CEtResourceMng::~CEtResourceMng()
{
	ClearCacheMemoryStream();
	SAFE_DELETE( m_pFileSystem );
}

bool CEtResourceMng::AddResourcePath( const char *szPath, bool bIncludeSubDir )
{
	bool bResult = _AddResourcePath( szPath, bIncludeSubDir );

	if( m_bUsePackingFile ) CEtFileSystem::GetInstance().AddResourcePath( szPath, bIncludeSubDir );

	return bResult;
}

void CEtResourceMng::RemoveResourcePath( const char *szPath, bool bIncludeSubDir )
{
	_RemoveResourcePath( szPath, bIncludeSubDir );

	if( m_bUsePackingFile ) CEtFileSystem::GetInstance().RemoveResourcePath( szPath, bIncludeSubDir );
}

void CEtResourceMng::ReserchAddResourcePath( const char* szPath )
{
	if( szPath == NULL ) return;

	CFileNameString szPathString = szPath;
	ToLowerA( szPathString );
	std::map<CFileNameString, int>::iterator it = m_szMapCacheFolderList.find( szPathString );
	if( it == m_szMapCacheFolderList.end() ) return;
	
	int nCashFilePos = it->second;
	CacheFileList( nCashFilePos, true );
}

bool CEtResourceMng::_AddResourcePath( const char *szPath, bool bIncludeSubDir )
{
	ScopeLock<CSyncLock> Lock(m_SyncLock, m_bUseDynamic );
	CFileNameString szTemp = szPath;
	ToLowerA( szTemp );
	if( szTemp[0] == '.' && szTemp[1] == '\\' ) {
		char szTempPath[_MAX_PATH] = { 0, };
		sprintf_s( szTempPath, "%s\\%s", m_szVecResourcePath[0].c_str(), szTemp.c_str() + 2 );
		/*
		for( DWORD i=1; i<m_szVecResourcePath.size(); i++ ) {
			if( strcmp( m_szVecResourcePath[i].c_str(), szTempPath ) == NULL ) return false;
		}
		*/
		if( m_szMapCacheFolderList.find( szTempPath ) != m_szMapCacheFolderList.end() ) return false;

		m_szVecResourcePath.push_back( szTempPath );
		m_szMapCacheFolderList.insert( make_pair( szTempPath, (int)m_szVecResourcePath.size() - 1 ) );
	}
	else {
		/*
		for( DWORD i=1; i<m_szVecResourcePath.size(); i++ ) {
			if( strcmp( m_szVecResourcePath[i].c_str(), szTemp.c_str() ) == NULL ) return false;
		}
		*/
		if( m_szMapCacheFolderList.find( szTemp ) != m_szMapCacheFolderList.end() ) return false;
		m_szVecResourcePath.push_back( szTemp );
		m_szMapCacheFolderList.insert( make_pair( szTemp, (int)m_szVecResourcePath.size() - 1 ) );
	}

	CacheFileList( (int)m_szVecResourcePath.size() - 1, true );

	if( bIncludeSubDir == true ) {
		if( szPath[1] == ':' && strlen(szPath) < 3 ) return true;
		std::vector<CFileNameString> szVecList;
		char szTempPath[_MAX_PATH];
		_FindFolder( szTemp.c_str(), szVecList );
		for( DWORD i=0; i<szVecList.size(); i++ ) {
			sprintf_s( szTempPath, "%s\\%s", szTemp.c_str(), szVecList[i].c_str() );
			_strlwr_s( szTempPath, _MAX_PATH );
			_AddResourcePath( szTempPath, true );
		}
	}
	return true;
}

void CEtResourceMng::_RemoveResourcePath( const char *szPath, bool bIncludeSubDir )
{
	if( NULL == szPath ) return;
	if( strlen(szPath) == 0 ) return;

	ScopeLock<CSyncLock> Lock(m_SyncLock, m_bUseDynamic);
	CFileNameString szTemp = szPath;

	ToLowerA( szTemp );
	std::map<CFileNameString, int>::iterator it = m_szMapCacheFolderList.find( szTemp );
	if( it == m_szMapCacheFolderList.end() ) return;

	int nOffset = it->second;
	for( int i=nOffset; i<(int)m_szVecResourcePath.size(); i++ ) {
		if( _stricmp( m_szVecResourcePath[i].c_str(), szTemp.c_str() ) == NULL ) {
			CacheFileList( i, false );
			m_szMapCacheFolderList.erase( m_szVecResourcePath[i] );
			m_szVecResourcePath.erase( m_szVecResourcePath.begin() + i );
			if( !bIncludeSubDir ) break;
			i--;
			if( szTemp[(int)strlen(szPath)-1] != '\\' ) szTemp += "\\";
			continue;
		}
		if( bIncludeSubDir ) {
			if( szPath[1] == ':' && strlen(szPath) < 3 ) return;
				CacheFileList( i, false );
				m_szMapCacheFolderList.erase( m_szVecResourcePath[i] );
				m_szVecResourcePath.erase( m_szVecResourcePath.begin() + i );
				i--;
		}
	}
	/*
	for( DWORD i=1; i<m_szVecResourcePath.size(); i++ ) {
		if( strstr( m_szVecResourcePath[i].c_str(), szTemp.c_str() ) ) {
			if( _stricmp( m_szVecResourcePath[i].c_str(), szTemp.c_str() ) == NULL ) {
				CacheFileList( i, false );
				m_szVecResourcePath.erase( m_szVecResourcePath.begin() + i );
				if( !bIncludeSubDir ) break;
				i--;
				if( szTemp[strlen(szPath)-1] != '\\' ) szTemp += "\\";
				continue;
			}
			if( bIncludeSubDir ) {
				if( szPath[1] == ':' && strlen(szPath) < 3 ) return;
				CacheFileList( i, false );
				m_szVecResourcePath.erase( m_szVecResourcePath.begin() + i );
				i--;
			}
		}
	}
	*/
	/*
	if( bIncludeSubDir && bValid ) {
		std::vector<CFileNameString> szVecList;
		char szTempPath[_MAX_PATH];
		_FindFolder( szTemp.c_str(), szVecList );
		for( DWORD i=0; i<szVecList.size(); i++ ) {
			sprintf_s( szTempPath, "%s\\%s", szTemp.c_str(), szVecList[i].c_str() );
			strlwr( szTempPath );
			_RemoveResourcePath( szTempPath, bIncludeSubDir );
		}
	}
	*/
}

bool CEtResourceMng::AddResourcePathByCodePage( const char *szPath, int nCodePage, bool bIncludeSubDir )
{
	ScopeLock<CSyncLock> Lock(m_SyncLock, m_bUseDynamic);
	char szTemp[256] = { 0, };
	const char *szCodePageString = FindCodePageString( ( nCodePage == -1 ) ? m_nCodePage : nCodePage );
	if( szCodePageString != NULL ) {
		sprintf_s( szTemp, "%s\\%s", szPath, szCodePageString );
		AddResourcePath( szTemp, bIncludeSubDir );
	}
	return AddResourcePath( szPath, bIncludeSubDir );
}

bool CEtResourceMng::AddResourcePath( CFileNameString &szPath, bool bIncludeSubDir )
{
	return AddResourcePath( szPath.c_str(), bIncludeSubDir );
}

void CEtResourceMng::RemoveResourcePath( CFileNameString &szPath, bool bIncludeSubDir )
{
	RemoveResourcePath( szPath.c_str(), bIncludeSubDir );
}

bool CEtResourceMng::AddResourcePathByCodePage( CFileNameString &szPath, int nCodePage, bool bIncludeSubDir )
{
	return AddResourcePathByCodePage( szPath.c_str(), nCodePage, bIncludeSubDir );
}

CFileNameString CEtResourceMng::GetFullName( CFileNameString &szFileName, bool *bFind )
{
	return GetFullName( szFileName.c_str(), bFind );
}

CFileNameString CEtResourceMng::GetFullPath( CFileNameString &szPath, bool bSearchSubDir )
{
	return GetFullPath( szPath.c_str(), bSearchSubDir );
}

CFileNameString CEtResourceMng::GetFullNameRandom( CFileNameString &szFileName, int nIndex, bool *bFind )
{
	ScopeLock<CSyncLock> Lock(m_SyncLock, m_bUseDynamic);
	char szTempName[_MAX_PATH];
	char szTempFileName[_MAX_PATH];
	char szExt[32];
	_GetFileName( szTempFileName, _countof(szTempFileName), szFileName.c_str() );
	_GetExt( szExt, _countof(szExt), szFileName.c_str() );

	sprintf_s( szTempName, "%s_%c.%s", szTempFileName, 'a' + nIndex, szExt );

	return GetFullName( szTempName, bFind );
}

CStream *CEtResourceMng::GetStreamRandom( CFileNameString &szFileName, int nIndex, bool *bFind )
{
	ScopeLock<CSyncLock> Lock(m_SyncLock, m_bUseDynamic);
	char szTempName[_MAX_PATH];
	char szTempFileName[_MAX_PATH];
	char szExt[32];
	_GetFileName( szTempFileName, _countof(szTempFileName), szFileName.c_str() );
	_GetExt( szExt, _countof(szExt), szFileName.c_str() );

	sprintf_s( szTempName, "%s_%c.%s", szTempFileName, 'a' + nIndex, szExt );

	return GetStream( szTempName, bFind );
}

CFileNameString CEtResourceMng::GetFullName( const char *szFileName, bool *bFind )
{
	CFileNameString strFullName;

	ScopeLock<CSyncLock> Lock(m_SyncLock, m_bUseDynamic);
	if( szFileName == NULL || strlen(szFileName) == 0 ) 
	{
		if( bFind ) *bFind = false;
		return strFullName;
	}

	if( m_bUsePackingFile && szFileName[0] != '.' ) 
	{
		strFullName = CEtFileSystem::GetInstance().GetFullName( szFileName, bFind );
	}
	else 
	{
		char szTemp[4096];
		if( szFileName && strlen(szFileName) > 2 && ( ( szFileName[1] == ':' ) || ( szFileName[0] == '\\' && szFileName[1] == '\\' ) ) ) 
		{
			if( bFind ) *bFind = IsExistFile( szFileName );
			strFullName = szFileName;
		}
		else
		{
			char szName[_MAX_PATH] = { 0, };
			_GetFullFileName( szName, _countof(szName), szFileName );
			_strlwr_s( szName, _MAX_PATH );

			std::map<CFileNameString, CFileNameString>::iterator it = m_szMapAccessSuccessFileName.find( szName );
			if( it != m_szMapAccessSuccessFileName.end() )
			{
				if( bFind ) *bFind = true;
				strFullName = it->second;
			}

			if( strFullName.empty() )
			{
				it = m_szMapAccessFailedFileName.find( szName );
				if( it != m_szMapAccessFailedFileName.end() ) 
				{
					if( bFind ) *bFind = false;
					strFullName = szFileName;
				}
			}

			if( strFullName.empty() )
			{
				it = m_szMapCacheFileList.find( szName );
				if( it != m_szMapCacheFileList.end() ) 
				{
					if( bFind ) *bFind = true;
					strFullName = it->second;
				}
			}

			// 어느순간 보니, 아래 IsExistFile 함수가 랜덤하게 느려지는 현상이 발생했다.
			// 파일 있는지 검사하는 단순한 함수인데, GetFileAttribute로 바꾸고 쓰레드를 안쓰고 해봐도,
			// 정말 랜덤하게 느려질때가 있다.
			// (아래 m_szVecResourcePath 리스트 개수가 760개일때 테스트한건데, 빠르게 돌땐 0-16ms나오지만 느리게 돌면 140-250ms나온다.)
			// 우선은 해외빌드도 대기중이고 본섭 클라이언트도 로딩때문에 문제가 발생한거 같아서, 툴이나 기타 상황에선 쓰고, 클라에선 안쓰는거로 막아둔다.

			if( strFullName.empty() )
			{
				if( m_bUseMapAccessSuccessFileName ) 
				{
					for( DWORD i=1; i<m_szVecResourcePath.size(); i++ ) 
					{
						if( m_szVecResourcePath[i][0] == '.' && m_szVecResourcePath[i][1] == '\\' )
						{
							sprintf_s( szTemp, "%s\\%s\\%s", m_szVecResourcePath[0].c_str(), m_szVecResourcePath[i].c_str() + 2, szFileName );
						}
						else
						{
							sprintf_s( szTemp, "%s\\%s", m_szVecResourcePath[i].c_str(), szFileName );
						}

						if( IsExistFile( szTemp ) == true ) 
						{
							m_szMapAccessSuccessFileName.insert( make_pair( szName, szTemp ) );
							if( bFind ) *bFind = true;
							strFullName = szTemp;
						}
					}
				}

				if( strFullName.empty() )
				{
					m_szMapAccessFailedFileName.insert( make_pair( szName, szName ) );

					if( bFind ) *bFind = false;
					strFullName = szFileName;
				}
			}
		}
	}

	// ui,dds 파일 다국어에 맞춰서 파일명 교체
	if( !m_strLanguage.empty() )
	{
		char szExt[32];
		_GetExt( szExt, _countof(szExt), strFullName.c_str() );

		if( ( _stricmp( szExt, "ui" ) == NULL || _stricmp( szExt, "dds" ) == NULL ) ) 
		{
			strFullName = ChangeFileNameByLanguage( strFullName );
		}
	}

	return strFullName;
}

CFileNameString CEtResourceMng::ChangeFileNameByLanguage( CFileNameString strFullName )
{
	if( m_strLanguage.empty() )
		return strFullName;

	char szExt[32];
	char szTempFileName[_MAX_PATH];
	char szChangeFileName[_MAX_PATH];
	_GetFileName( szTempFileName, _countof(szTempFileName), strFullName.c_str() );
	_GetExt( szExt, _countof(szExt), strFullName.c_str() );

	sprintf_s( szChangeFileName, "%s_%s.%s", szTempFileName, m_strLanguage.c_str(), szExt );
	_strlwr_s( szChangeFileName, _MAX_PATH );

	bool bFind = false;
	if( m_bUsePackingFile ) 
	{
		CFileNameString strFindName = CEtFileSystem::GetInstance().GetFullName( szChangeFileName, &bFind );
		if( bFind )
			strFullName = strFindName;
	}
	else
	{
		std::map<CFileNameString, CFileNameString>::iterator it = m_szMapCacheFileList.find( szChangeFileName );
		if( it != m_szMapCacheFileList.end() ) 
		{
			bFind = true;
			strFullName = it->second;
		}
	}

	return strFullName;
}

CFileNameString CEtResourceMng::GetFullName( std::string &szFileName, bool *bFind )
{
	return GetFullName( szFileName.c_str(), bFind );
}

CFileNameString CEtResourceMng::GetFullNameInCacheList( const char *szFileName )
{
	char szName[_MAX_PATH] = { 0, };
	_GetFullFileName( szName, _countof(szName), szFileName );
	_strlwr_s( szName, _MAX_PATH );

	std::map<CFileNameString, CFileNameString>::iterator it = m_szMapCacheFileList.find( szName );
	if( it != m_szMapCacheFileList.end() ) {
		return it->second;
	}
	return "";
}

CFileNameString CEtResourceMng::GetFullNameRandom( std::string &szFileName, int nIndex, bool *bFind )
{
	ScopeLock<CSyncLock> Lock(m_SyncLock, m_bUseDynamic);
	char szTempName[_MAX_PATH];
	char szTempFileName[_MAX_PATH];
	char szExt[32];
	_GetFileName( szTempFileName, _countof(szTempFileName), szFileName.c_str() );
	_GetExt( szExt, _countof(szExt), szFileName.c_str() );

	sprintf_s( szTempName, "%s_%c.%s", szTempFileName, 'a' + nIndex, szExt );

	return GetFullName( szTempName, bFind );
}

CFileNameString CEtResourceMng::GetFullPath( const char *szPath, bool bSearchSubDir )
{
	ScopeLock<CSyncLock> Lock(m_SyncLock, m_bUseDynamic);

	if( m_bUsePackingFile ) {
		return CEtFileSystem::GetInstance().GetFullPath( szPath );
	}
	else {
		char szTemp[_MAX_PATH];
		std::vector< CFileNameString > szVecList;
		int nLength = (int)strlen(szPath);
		for( DWORD i=0; i<m_szVecResourcePath.size(); i++ ) {
			szVecList.clear();
			CFileNameString szTempPath = szPath;
			ToLowerA( szTempPath );
			char *pFindPtr = (char*)strstr( m_szVecResourcePath[i].c_str(), szTempPath.c_str() );
			/*
			if( pFindPtr ) {
				if( pFindPtr[nLength] == '\\' || pFindPtr[nLength] == '\r' || pFindPtr[nLength] == '\n' || pFindPtr[nLength] == 0 )
					return m_szVecResourcePath[i];
				else {
					strncpy_s( szTemp, m_szVecResourcePath[i].c_str(), strlen(m_szVecResourcePath[i].c_str()) - strlen(pFindPtr) - 1 );
					_FindFolder( szTemp, szVecList );
					for( DWORD j=0; j<szVecList.size(); j++ ) {
						if( _stricmp( szVecList[j].c_str(), szTempPath.c_str() ) == NULL ) {
							szTempPath = szTemp + CFileNameString("\\") + szVecList[j];
							return szTempPath;
						}
					}
				}
			}
			*/
			if( pFindPtr && ( pFindPtr[nLength] == '\\' || pFindPtr[nLength] == '\r' || pFindPtr[nLength] == '\n' || pFindPtr[nLength] == 0 ) )
				return m_szVecResourcePath[i];
			else if( bSearchSubDir ) {
				_FindFolder( m_szVecResourcePath[i].c_str(), szVecList );
				for( DWORD j=0; j<szVecList.size(); j++ ) {
					if( _stricmp( szVecList[j].c_str(), szTempPath.c_str() ) == NULL ) {
						if( m_szVecResourcePath[i][0] == '.' && m_szVecResourcePath[i][1] == '\\' ) {
							sprintf_s( szTemp, "%s\\%s\\%s", m_szVecResourcePath[0].c_str(), m_szVecResourcePath[i].c_str() + 2, szTempPath.c_str() );
						}
						else sprintf_s( szTemp, "%s\\%s", m_szVecResourcePath[i].c_str(), szTempPath.c_str() );
						return szTemp;
					}
				}
			}
		}
		return szPath;
	}
	return "";
}

void CEtResourceMng::FindFullPathList( const char *szPath, std::vector<CFileNameString> &szVecResult )
{
	ScopeLock<CSyncLock> Lock(m_SyncLock, m_bUseDynamic);

	if( m_bUsePackingFile ) {
		return CEtFileSystem::GetInstance().FindFullPathList( szPath, szVecResult );
	}
	else {
		//char szTemp[_MAX_PATH];
		std::vector< CFileNameString > szVecList;
		int nLength = (int)strlen(szPath);
		for( DWORD i=0; i<m_szVecResourcePath.size(); i++ ) {
			szVecList.clear();
			CFileNameString szTempPath = szPath;
			ToLowerA( szTempPath );
			char *pFindPtr = (char*)strstr( m_szVecResourcePath[i].c_str(), szTempPath.c_str() );

			if( pFindPtr && ( pFindPtr[nLength] == '\\' || pFindPtr[nLength] == '\r' || pFindPtr[nLength] == '\n' || pFindPtr[nLength] == 0 ) )
				szVecResult.push_back( m_szVecResourcePath[i] );
			/*
			else if( bSearchSubDir ) {
				_FindFolder( m_szVecResourcePath[i].c_str(), szVecList );
				for( DWORD j=0; j<szVecList.size(); j++ ) {
					if( _stricmp( szVecList[j].c_str(), szTempPath.c_str() ) == NULL ) {
						if( m_szVecResourcePath[i][0] == '.' && m_szVecResourcePath[i][1] == '\\' ) {
							sprintf_s( szTemp, "%s\\%s\\%s", m_szVecResourcePath[0].c_str(), m_szVecResourcePath[i].c_str() + 2, szTempPath.c_str() );
						}
						else sprintf_s( szTemp, "%s\\%s", m_szVecResourcePath[i].c_str(), szTempPath.c_str() );
						szVecResult.push_back( szTemp );
					}
				}
			}
			*/
		}
	}
}

bool CEtResourceMng::AddResourcePath( std::string &szPath, bool bIncludeSubDir )
{
	return AddResourcePath( szPath.c_str(), bIncludeSubDir );
}

void CEtResourceMng::RemoveResourcePath( std::string &szPath, bool bIncludeSubDir )
{
	RemoveResourcePath( szPath.c_str(), bIncludeSubDir );
}

bool CEtResourceMng::AddResourcePathByCodePage( std::string &szPath, int nCodePage, bool bIncludeSubDir )
{
	return AddResourcePathByCodePage( szPath.c_str(), nCodePage, bIncludeSubDir );
}

bool CEtResourceMng::IsExistFile( const char *szFileName )
{
	//ScopeLock<CSyncLock> Lock(m_SyncLock, m_bUseDynamic );	 
	bool bExist = (_access(szFileName, 0)==0);	// 여긴 Lock 없어도 될것 같습니다.
	return bExist;
}

void CEtResourceMng::Initialize()
{
	ScopeLock<CSyncLock> Lock(m_SyncLock, m_bUseDynamic);
	// 현제 국가코드 얻어놓는다.
	WCHAR wszCodePage[ 8 ];
	if( GetLocaleInfoW( MAKELCID( GetKeyboardLayout( 0 ), SORT_DEFAULT ), LOCALE_IDEFAULTANSICODEPAGE, wszCodePage, sizeof( wszCodePage ) / sizeof( wszCodePage[ 0 ] ) ) ) {
		m_nCodePage = wcstoul( wszCodePage, NULL, 0 ); // LOCALE_SABBREVLANGNAME
		// 만약 현제 국가코드가 없을경우엔 등록한다.
		if( FindCodePageString( m_nCodePage ) == NULL ) {
			if( GetLocaleInfoW( MAKELCID( GetKeyboardLayout( 0 ), SORT_DEFAULT ), LOCALE_SABBREVLANGNAME, wszCodePage, sizeof( wszCodePage ) / sizeof( wszCodePage[ 0 ] ) ) ) {
				char szTemp[8];
				WideCharToMultiByte( CP_ACP, 0, wszCodePage, -1, szTemp, 8, NULL, NULL );
				_strlwr_s( szTemp, 8 );
				m_mapCodePageString.insert( make_pair( m_nCodePage, szTemp ) );
			}
		}
	}
}

void CEtResourceMng::AddCodePage( int nCodePage, const char *szCodePageStr )
{
	ScopeLock<CSyncLock> Lock(m_SyncLock, m_bUseDynamic);
	if( FindCodePageString( nCodePage ) != NULL ) return;
	m_mapCodePageString.insert( make_pair( nCodePage, szCodePageStr ) );
}

const char *CEtResourceMng::FindCodePageString( int nCodePage )
{
	ScopeLock<CSyncLock> Lock(m_SyncLock, m_bUseDynamic);
	std::map<int, CFileNameString>::iterator it = m_mapCodePageString.find( nCodePage );
	if( it == m_mapCodePageString.end() ) return NULL;
	return it->second.c_str();
}

bool CEtResourceMng::IsCodePageFolder( const char *szCodePageStr )
{
	ScopeLock<CSyncLock> Lock(m_SyncLock, m_bUseDynamic);
	for( DWORD i=0; i<m_mapCodePageString.size(); i++ ) {
		if( _stricmp( m_mapCodePageString[i].c_str(), szCodePageStr ) == NULL ) return true;
	}
	return false;
}

void CEtResourceMng::SetPackingFolder( const char *szPath )
{
	if( m_bUsePackingFile ) CEtFileSystem::GetInstance().AddPackingFolder( szPath );
}

void CEtResourceMng::SetPackingFolder( const WCHAR *szPath )
{
	if( m_bUsePackingFile ) CEtFileSystem::GetInstance().AddPackingFolder( szPath );
}

void CEtResourceMng::FindFileListInFolder( const char *szFolderName, const char *szExt, std::vector<CFileNameString> &szVecResult)
{
	if ( m_bUsePackingFile )
	{
		CEtFileSystem::GetInstance().FindFileListInFolder( szFolderName, szExt, szVecResult);
	}
	else
	{
		FindFileListInDirectory(szFolderName, szExt, szVecResult);
	}
}

void CEtResourceMng::FindFileListAll_IgnoreExistFile( const char *szFolderName, const char *szExt, std::vector<CFileNameString> &szVecResult )
{
	std::vector<CFileNameString> szVecFolderList;

	FindFullPathList( szFolderName, szVecFolderList );

	char szTemp[256];
	std::vector<CFileNameString> szVecFileNameList;

	for( DWORD i=0; i<szVecFolderList.size(); i++ ) {
		std::vector<CFileNameString> szVecTemp;
		FindFileListInFolder( szVecFolderList[i].c_str(), szExt, szVecTemp );

		for( DWORD j=0; j<szVecTemp.size(); j++ ) {
			sprintf_s( szTemp, szVecTemp[j].c_str() );
			_strlwr_s( szTemp );
			bool bExistFile = false;
			for( DWORD k=0; k<szVecFileNameList.size(); k++ ) {
				if( strcmp( szVecFileNameList[k].c_str(), szTemp ) == NULL ) {
					bExistFile = true;
					break;
				}
			}
			if( !bExistFile ) {
				szVecFileNameList.push_back( szTemp );
				sprintf_s( szTemp, "%s\\%s", szVecFolderList[i].c_str(), szVecTemp[j].c_str() );
				szVecResult.push_back( szTemp );
			}
		}
	}
}

CStream *CEtResourceMng::GetStream( const char *szFileName, bool *bFind )
{
	ScopeLock<CSyncLock> Lock(m_SyncLock, m_bUseDynamic);

	bool bExistFile = false;
	if( bFind ) *bFind = false;

	CFileNameString szResultName = szFileName;
	if( m_bUsePackingFile && szFileName[0] != '.' ) {

		CFileNameString szFullName = GetFullName( szFileName, &bExistFile );
		CStream *pCacheStream = GetCacheMemoryStream( szFullName.c_str() );
		if( pCacheStream ) return pCacheStream;

		CPackingStream *pStream = new CPackingStream( szResultName.c_str() );
		if( !pStream->IsValid() ) {
			SAFE_DELETE( pStream );
			return NULL;
		}

		if( bFind ) *bFind = true;
		return pStream;
	}
	else {
		CFileNameString szFullName = GetFullName( szFileName, &bExistFile );

		if( bExistFile ) {
			CStream *pCacheStream = GetCacheMemoryStream( szFullName.c_str() );
			if( pCacheStream ) return pCacheStream;

			CFileStream *pStream = new CFileStream( szFullName.c_str(), CFileStream::OPEN_READ );
			if( !pStream->IsValid() ) {
				SAFE_DELETE( pStream );
				return NULL;
			}

			if( bFind ) *bFind = true;
			return pStream;
		}
	}

	return NULL;
}

CStream *CEtResourceMng::GetStream( CFileNameString &szFileName, bool *bFind )
{
	return GetStream( szFileName.c_str(), bFind );
}

void CEtResourceMng::CacheFileList( int nIndex, bool bAdd )
{
	ScopeLock<CSyncLock> Lock(m_SyncLock, m_bUseDynamic);
	char szTemp[4096];
	std::vector<CFileNameString> szVecFileList;
	CFileNameString szFullName;


	if( m_szVecResourcePath[nIndex][0] == '.' && m_szVecResourcePath[nIndex][1] == '\\' ) {
		sprintf_s( szTemp, "%s\\%s\\", m_szVecResourcePath[0].c_str(), m_szVecResourcePath[nIndex].c_str() + 2 );
	}
	else
		sprintf_s( szTemp, "%s\\", m_szVecResourcePath[nIndex].c_str() );

	if( bAdd ) {
		OffsetStruct Struct;
		szVecFileList.clear();
		FindFileListInDirectory( szTemp, "*.*", szVecFileList );
		for( DWORD j=0; j<szVecFileList.size(); j++ ) {
			szFullName = CFileNameString(szTemp) + szVecFileList[j];
			ToLowerA( szVecFileList[j] );
			if( m_szMapCacheFileList.find( szVecFileList[j] ) != m_szMapCacheFileList.end() ) continue;

			m_szMapCacheFileList.insert( make_pair( szVecFileList[j], szFullName ) );
			Struct.szVecFileList.push_back( szVecFileList[j] );
		}
		m_VecPathCacheFileOffset.push_back( Struct );
	}
	else {
		if( nIndex < 0 || nIndex >= (int)m_VecPathCacheFileOffset.size() ) return;
		for( DWORD j=0; j<m_VecPathCacheFileOffset[nIndex].szVecFileList.size(); j++ ) {
			m_szMapCacheFileList.erase( m_VecPathCacheFileOffset[nIndex].szVecFileList[j] );
		}

		m_VecPathCacheFileOffset.erase( m_VecPathCacheFileOffset.begin() + nIndex );
	}
}

bool CEtResourceMng::AddCacheMemoryStream( const char *szFileName )
{
	bool bValid;
	CFileNameString szFullName = GetFullName( szFileName, &bValid );
	if( !bValid ) return false;
	ToLowerA( szFullName );

	std::map<CFileNameString, CacheMemoryBuffer *>::iterator it = m_szMapCacheMemoryBufferList.find( szFullName );
	if( it != m_szMapCacheMemoryBufferList.end() ) return false;

	CResMngStream Stream( szFullName.c_str() );
	if( !Stream.IsValid() ) return false;

	CacheMemoryBuffer *pBuffer = new CacheMemoryBuffer;
	pBuffer->pPtr = new char[Stream.Size()];
	pBuffer->nSize = Stream.Size();
	Stream.Read( pBuffer->pPtr, Stream.Size() );

	m_szMapCacheMemoryBufferList.insert( make_pair( szFullName, pBuffer ) );

	return true;
}

bool CEtResourceMng::RemoveCacheMemoryStream( const char *szFileName )
{
	bool bValid;
	CFileNameString szFullName = GetFullName( szFileName, &bValid );
	if( !bValid ) return false;
	ToLowerA( szFullName );

	std::map<CFileNameString, CacheMemoryBuffer *>::iterator iter = m_szMapCacheMemoryBufferList.find( szFullName );
	if( iter != m_szMapCacheMemoryBufferList.end() )
	{
		SAFE_DELETEA( iter->second->pPtr );
		SAFE_DELETE( iter->second );
		m_szMapCacheMemoryBufferList.erase( iter );
	}

	return true;
}

CStream *CEtResourceMng::GetCacheMemoryStream( const char *szFileName )
{
	CFileNameString szTemp = szFileName;
	ToLowerA( szTemp );
	std::map<CFileNameString, CacheMemoryBuffer *>::iterator it = m_szMapCacheMemoryBufferList.find( szTemp );
	if( it == m_szMapCacheMemoryBufferList.end() ) return NULL;

	CMemoryStream *pStream = new CMemoryStream( it->second->pPtr, it->second->nSize );
	pStream->SetName( szTemp.c_str() );
	return pStream;
}

void CEtResourceMng::ClearCacheMemoryStream()
{
	for( std::map<CFileNameString, CacheMemoryBuffer *>::iterator it = m_szMapCacheMemoryBufferList.begin(); it != m_szMapCacheMemoryBufferList.end(); it++ ) {
		SAFE_DELETEA( it->second->pPtr );
		SAFE_DELETE( it->second );
	}
	SAFE_DELETE_VEC( m_szMapCacheMemoryBufferList );
}

CEtResourceAccess::CEtResourceAccess() 
{
	m_pStream = NULL;
	m_bSelfDelete = true;
	m_bAddPath = false;
	m_bValid = false;
	m_bIncludeSubPath = false;
	m_szAddPath[0] = 0;
}

CEtResourceAccess::CEtResourceAccess( const char *pFileName, bool bSelfDelete, bool bIncludeSubPath ) 
{
	Initialize( pFileName, bSelfDelete, bIncludeSubPath );
}

CEtResourceAccess::~CEtResourceAccess() 
{
	Finalize();
}

void CEtResourceAccess::Initialize( const char *pFileName, bool bSelfDelete, bool bIncludeSubPath ) 
{
	m_pStream = NULL;
	m_bSelfDelete = bSelfDelete;
	m_bIncludeSubPath = bIncludeSubPath;
	if( IsFullName( pFileName ) ) {
		memset( m_szAddPath, 0, sizeof(m_szAddPath) );
		_GetPath( m_szAddPath, _countof(m_szAddPath), pFileName );
		if( m_szAddPath[strlen(m_szAddPath)-1] == '\\' )
			m_szAddPath[strlen(m_szAddPath)-1] = 0;
		if( CEtResourceMng::GetInstance().IsUseDynamic() )
			m_bAddPath = CEtResourceMng::GetInstance().AddResourcePath( m_szAddPath, m_bIncludeSubPath );
	}
	else m_bAddPath = false;

	m_szFullName = CEtResourceMng::GetInstance().GetFullName( pFileName, &m_bValid );
	if( !m_bValid ) m_szFullName = pFileName;
}

void CEtResourceAccess::Finalize()
{
	if( m_bAddPath && CEtResourceMng::GetInstance().IsUseDynamic() ) CEtResourceMng::GetInstance().RemoveResourcePath( m_szAddPath, m_bIncludeSubPath );
	if( m_bSelfDelete ) SAFE_DELETE( m_pStream );

	m_pStream = NULL;
	m_bSelfDelete = true;
	m_bAddPath = false;
	m_bValid = false;
}

CStream *CEtResourceAccess::GetStream() 
{ 
	if( !m_bValid ) return NULL;
	m_pStream = CEtResourceMng::GetInstance().GetStream( m_szFullName );
	return m_pStream;
}

CResMngStream::CResMngStream()
{
}

CResMngStream::CResMngStream( const char *pFileName, bool bSelfDelete, bool bIncludeSubPath )
{
	Open( pFileName, bSelfDelete, bIncludeSubPath );
}

CResMngStream::~CResMngStream()
{
}

bool CResMngStream::Open( const char *pFileName, bool bSelfDelete, bool bIncludeSubPath )
{
	Initialize( pFileName, bSelfDelete, bIncludeSubPath );
	GetStream();
	return ( m_pStream ) ? true : false;
}

bool CResMngStream::IsEnd() const 
{
	if( !m_pStream ) return true;
	return m_pStream->IsEnd();
}

long CResMngStream::Tell()
{
	if( !m_pStream ) return 0;
	return m_pStream->Tell();
}

int CResMngStream::Size() const
{
	if( !m_pStream ) return 0;
	return m_pStream->Size();
}

bool CResMngStream::IsValid() const
{
	if( !m_pStream ) return false;
	return m_pStream->IsValid();
}

void CResMngStream::Close()
{
	Finalize();
}

void CResMngStream::Activate()
{
//	if( !m_pStream ) return;
//	m_pStream->Activate();
}

int CResMngStream::Seek( long lOffset, int nOrigin )
{
	if( !m_pStream ) return 0;
	return m_pStream->Seek( lOffset, nOrigin );
}

int CResMngStream::Read( void *pBuffer, int nSize )
{
	if( !m_pStream ) return 0;
	return m_pStream->Read( pBuffer, nSize );
}

int CResMngStream::Write( const void *pBuffer, int nSize )
{
	return 0;
}


const char *CResMngStream::GetName()
{
	if( !m_pStream ) return NULL;
	return m_pStream->GetName();
}