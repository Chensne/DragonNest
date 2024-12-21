#include "stdafx.h"
#include "DnPatchInfo.h"
#include "VarArg.h"
#include "wininet.h"
#include "EtFileSystem.h"
#include "Stream.h"
#include "DnLauncher.h"
#include "DnControlData.h"
#include "DnFirstPatchInfo.h"
#include "DnExplorerDlg.h"
#include "DnFIrstPatchDlg.h"
#include "MD5Text.h"
#include "Settings.h"
#include "RLKTAuth.h"

#pragma comment ( lib, "Wininet.lib" )
#pragma warning ( disable : 4018 )

extern CString g_szCmdLine;
extern CString g_szCommandLinePatchURL;
extern int g_nInitErrorCode;
extern CDnFIrstPatchDlg	 g_FirstPatchDlg;

//////////////////////////////////////////////////////////////////////////
// CDnPatchInfo Class
//////////////////////////////////////////////////////////////////////////

CDnPatchInfo::CDnPatchInfo()
: m_bSetPatchInfo( FALSE )
, m_nClientVersion( -1 )
, m_nServerVersion( -1 )
#ifdef _USE_AUTOUPDATE
, m_nNextVersion( -1 )
#endif // _USE_AUTOUPDATE
, m_wOpen( 0 )
, m_nLanguageOffset( 0 )
{
}

CDnPatchInfo::~CDnPatchInfo()
{
	m_vecChannelList.clear();
	m_vecSkipVersion.clear();
	m_vecLoginServerList.clear();
}

CDnPatchInfo& CDnPatchInfo::GetInstance()
{
	static CDnPatchInfo global;
	return global;
}

HRESULT CDnPatchInfo::Init()
{
	LogWnd::TraceLog( L"PatchInfo Init Start!" );
	// 1. 지정된 경로에서 PATCHCONFIG_LIST 파일을 다운받는다.
	if( FAILED( DownLoadPatchConfigList() ) )
		return E_FAIL;

	// 2. 다운 받은 xml 파일을 Parsing
	if( FAILED( ParsingPatchConfigList() ) )
		return E_FAIL;

	// 클라이언트 경로 셋팅
	TCHAR szCurDir[ _MAX_PATH ]={0,};
	GetCurrentDirectory( _MAX_PATH, szCurDir );
	m_strClientPath = szCurDir;

	int nLast = m_strClientPath.GetLength();
	if ( m_strClientPath[nLast] != _T('\\') || m_strClientPath[nLast] != _T('/') )
		m_strClientPath += _T('\\');

	// 다운받았던 xml파일 삭제
	ClientDeleteFile( m_strClientPath + PATCHCONFIG_LIST );
	LogWnd::TraceLog( L"PatchInfo Init Success!" );

	return S_OK;
}

#ifdef _USE_MULTILANGUAGE

void CDnPatchInfo::SetLanguageParam( CString strLanguageParam )
{
	m_strLanguageParam = strLanguageParam;
	m_nLanguageOffset = GetLanguageResourceIndexOffset( strLanguageParam );

	DNCTRLDATA.SetLanguageParam( m_strLanguageParam );
}

void CDnPatchInfo::SetLanguageParamByLanguageID( int nLanguageID )
{
#ifdef _EU
	switch( nLanguageID )
	{
	case LANG_ENGLISH:
		DNPATCHINFO.SetLanguageParam( L"ENG" );
		break;
	case LANG_GERMAN:
		DNPATCHINFO.SetLanguageParam( L"GER" );
		break;
	case LANG_FRENCH:
		DNPATCHINFO.SetLanguageParam( L"FRA" );
		break;
	case LANG_SPANISH:
		DNPATCHINFO.SetLanguageParam( L"ESP" );
		break;
	default:
		DNPATCHINFO.SetLanguageParam( L"ENG" );
		break;
	}
#endif // _EU
}

void CDnPatchInfo::AddLanguageParamToTotalParam()
{
	if( m_strLanguageParam.GetLength() > 0 )
	{
		CString strLanguageParam;
		strLanguageParam.Format( _T( " /language:%s" ), m_strLanguageParam );
		m_strTotalParam += strLanguageParam;
	}
}

void CDnPatchInfo::SetLocaleGuidePage()
{
	std::vector<stChannelListSet>::iterator iter1 = m_vecChannelList.begin();

	if( iter1 != m_vecChannelList.end() )
	{
		std::vector<stPartitionListSet>::iterator iter2 = (*iter1).m_vecPartitionList.begin();

		for( ; iter2 != (*iter1).m_vecPartitionList.end(); iter2++ )
		{
			if( (*iter2).m_strPartitionName == m_strLanguageParam )
			{
				m_strGuidepageUrl = (*iter2).m_strGuidepageUrl;
				break;
			}
		}
	}
}

#endif // _USE_MULTILANGUAGE

HRESULT CDnPatchInfo::DownLoadPatchConfigList()
{
	// 로컬 주소파일 체크
	WCHAR LocalLoginListUrl[1024]={ 0, };
	TCHAR szCurDir[ _MAX_PATH ]={0,};
	bool bUseLocalURL = false;
	GetCurrentDirectory( _MAX_PATH, szCurDir );

	TCHAR szLocalURL[MAX_PATH]={0,};
	swprintf_s( szLocalURL, L"%s\\%s", szCurDir,  L"LocalURL.ini" );

	FILE *stream = NULL;
	char szString[256]={0,};
	if( _wfopen_s( &stream, szLocalURL, L"r" ) == 0 )
	{
		fseek( stream, 0L, SEEK_SET );
		fscanf_s( stream, "%s", szString, 256 );

		fclose( stream );

		MultiByteToWideChar( CP_ACP, 0, szString, -1, LocalLoginListUrl, _countof(LocalLoginListUrl) );
		bUseLocalURL = true;
	}

	WCHAR PatchConfigListUrl[1024]={ 0, };
	//kat 2016
	//if( g_szCommandLinePatchURL.GetLength() > 0 )	// 패치환경 파일의 주소가 커맨드라인 파라미터로 넘어오는 경우 ( 현재 한국, 미국 사용 )
	//	wcscpy_s( PatchConfigListUrl, _countof(PatchConfigListUrl), g_szCommandLinePatchURL );
	//else
		wcscpy_s( PatchConfigListUrl, _countof(PatchConfigListUrl), PATCHCONFIG_LIST_URL );

	LogWnd::TraceLog( _T("DownLoad PatchConfigList (FileName %s)"), PATCHCONFIG_LIST );

//	CString test;
//	test.Format( L"%s%s", PatchConfigListUrl, PATCHCONFIG_LIST );
//	ErrorMessageBoxLog( test );
//	LogWnd::TraceLog( test );

	HRESULT hr = S_FALSE;
	for( int i=0; i<RETRY_MAX_COUNT; i++ )
	{
		if( bUseLocalURL )	// 로컬에 지정된 주소파일이 있는 경우 ( 현재 중국에서 제휴사 관련해서 사용 )
		{
			BOOL bResult = DeleteUrlCacheEntry( CVarArg<__MAX_PATH>(_T("%s"), LocalLoginListUrl) );
			hr = DownloadToFile( CVarArg<__MAX_PATH>(_T("%s"), LocalLoginListUrl), CVarArg<__MAX_PATH>(_T(".\\%s"), PATCHCONFIG_LIST) );
		}
		else
		{
			BOOL bResult = DeleteUrlCacheEntry( CVarArg<__MAX_PATH>(_T("%s%s"), PatchConfigListUrl, PATCHCONFIG_LIST) );
			hr = DownloadToFile( CVarArg<__MAX_PATH>(_T("%s%s"), PatchConfigListUrl, PATCHCONFIG_LIST), CVarArg<__MAX_PATH>(_T(".\\%s"), PATCHCONFIG_LIST) );
		}
		if( hr == S_OK )
			break;
	}

	if( hr == S_FALSE )
		hr = E_FAIL;

	if( FAILED( hr ) )
	{
		ErrorMessageBoxLog( _S( STR_LOAD_PATCHCONFIGLIST_FAILED + DNPATCHINFO.GetLanguageOffset() ) );
		g_nInitErrorCode = INIT_ERROR_PATCHCONFIGLIST_DOWNLOAD;
	}

	return hr;
}

HRESULT CDnPatchInfo::ParsingPatchConfigList()
{
	LogWnd::TraceLog( L"ParsingPatchConfigList" );

	if( ReadConfig() )	// 로컬 config파일을 먼저 읽는다. 못 읽었다면 다운로드 받은 xml파일에서 셋팅
		return S_OK;

	CXMLParser aParser;
	{
		USES_CONVERSION;

		if( !aParser.Open( CVarArgA<__MAX_PATH>(".\\%s", T2CA(PATCHCONFIG_LIST)) ) )
		{
			LogWnd::Log( LogLevel::Error, _T("%s Files - Parsing Failed."), PATCHCONFIG_LIST );
			g_nInitErrorCode = INIT_ERROR_PATCHCONFIGLIST_PARSING;
			return E_FAIL;
		}
	}

	WCHAR szChannellName[MAX_PATH] = { '\0', };
	WCHAR szLocalName[MAX_PATH] = { '\0', };
	WCHAR szLoginAddr[MAX_PATH] = { '\0', };
	WCHAR szGuidepageAddr[1024] = { '\0', };
	WCHAR szHomepageAddr[1024] = { '\0', };
	WCHAR szUpdateAddr[1024] = { '\0', };
	WCHAR szVersionAddr[1024] = { '\0', };
	WCHAR szStateTestAddr[1024] = { '\0', };

	WORD wLoginPort = 0;
	WORD wNew = 0;
	WORD wPartitionID = 0;
	WORD wOpen = 0;

	std::vector<stLoginServerSet> vecLoginServerList;
	int nPartitionCount = 0;

	if( aParser.FirstChildElement( "document", true ) ) {
		if( aParser.FirstChildElement( "ChannelList", true ) ) {
			do {
				::wcsncpy_s( szChannellName, _countof(szChannellName), aParser.GetAttribute( "channel_name" ), _countof(szChannellName) );

				stChannelListSet channellistset;
				channellistset.m_strChannelName = szChannellName;

				if( aParser.FirstChildElement( "Local", true ) ) {
					do {
						::wcsncpy_s( szLocalName, _countof(szLocalName), aParser.GetAttribute( "local_name" ), _countof(szLocalName) );

						USES_CONVERSION;
						
						if( aParser.GetAttribute( "new" ) )
							wNew = _wtoi( aParser.GetAttribute( "new" ) );
						if( aParser.GetAttribute( "partitionid" ) )
							wPartitionID = _wtoi( aParser.GetAttribute( "partitionid" ) );
						if( aParser.GetAttribute("open") )
							wOpen = _wtoi( aParser.GetAttribute("open") );
						
						if( aParser.FirstChildElement( "guidepage", false ) ) 
							::wcsncpy_s( szGuidepageAddr, _countof(szGuidepageAddr), aParser.GetAttribute( "addr" ), _countof(szGuidepageAddr) );

						if( aParser.FirstChildElement( "homepage", false ) ) 
							::wcsncpy_s( szHomepageAddr, _countof(szHomepageAddr), aParser.GetAttribute( "addr" ), _countof(szHomepageAddr) );

						if( aParser.FirstChildElement( "version", false ) ) 
							::wcsncpy_s( szVersionAddr, _countof(szVersionAddr), aParser.GetAttribute( "addr" ), _countof(szVersionAddr) );

						if( aParser.FirstChildElement( "update", false ) )
							::wcsncpy_s( szUpdateAddr, _countof(szUpdateAddr), aParser.GetAttribute( "addr" ), _countof(szUpdateAddr) );
						
						if( aParser.FirstChildElement( "statetest", false ) )
							::wcsncpy_s( szStateTestAddr, _countof(szStateTestAddr), aParser.GetAttribute( "addr" ), _countof(szStateTestAddr) );
						
						if( aParser.FirstChildElement( "login", false ) ) {
							do {
								::wcsncpy_s( szLoginAddr, _countof(szLoginAddr), aParser.GetAttribute( "addr" ), _countof(szLoginAddr) );
								wLoginPort = _wtoi( aParser.GetAttribute( "port" ) );

								if( L'\0' == szLoginAddr[0] || !wLoginPort )
									continue;

								stLoginServerSet loginserverset;
								loginserverset.m_strLoginUrl = szLoginAddr;
								loginserverset.m_wLoginPort = wLoginPort;

								vecLoginServerList.push_back( loginserverset );

							} while( aParser.NextSiblingElement( "login" ) );
						}

						SetPartitionList( channellistset, wNew, wPartitionID, wOpen, szGuidepageAddr, szHomepageAddr, 
											szLocalName, szVersionAddr, szUpdateAddr, szStateTestAddr, vecLoginServerList );
						nPartitionCount++;
						vecLoginServerList.clear();

					} while( aParser.NextSiblingElement( "Local" ) );
				}

				m_vecChannelList.push_back( channellistset );
				aParser.GoParent();
			} while( aParser.NextSiblingElement( "ChannelList" ) );
		}
		aParser.GoParent();
	}

	return S_OK;
}

BOOL CDnPatchInfo::ReadConfig()
{
	CHAR szCurDir[ _MAX_PATH ]= { 0, };
	CHAR szPatchInfoName[128]={0,};

	// 현재 실행 모듈을 알아낸다.
	CHAR ModuleName[MAX_PATH] ={0,};
	::GetModuleFileNameA( NULL, ModuleName , MAX_PATH );

	CHAR drive[_MAX_DRIVE];
	CHAR dir[_MAX_DIR];
	CHAR fname[_MAX_FNAME];
	CHAR ext[_MAX_EXT];

	_splitpath( ModuleName , drive, dir, fname, ext );

	CStringA Str;

#ifndef _DEBUG
	Str = drive;
	Str += dir;
#endif // !_DEBUG
	Str += PATCHINFONAME;

	CFileStream* pStream = new CFileStream( Str.GetBuffer() );

	if( pStream == NULL || !pStream->IsValid() )
	{
		SAFE_DELETE( pStream );
		return FALSE;
	}

	int nSize = pStream->Size()+ 3 ;	
	TCHAR *szCmdLine = new TCHAR[nSize];
	char *szTemp = new char[nSize];

	ZeroMemory( szCmdLine , sizeof(TCHAR)*nSize );
	ZeroMemory( szTemp , sizeof(char)*nSize );

	pStream->Read( szTemp, nSize );
	SAFE_DELETE( pStream );
	MultiByteToWideChar( CP_ACP, 0, szTemp, -1, szCmdLine, sizeof(TCHAR)*nSize );
	ParseCommandLineDev( szCmdLine );

	SAFE_DELETE_ARRAY( szCmdLine );
	SAFE_DELETE_ARRAY( szTemp );
	SAFE_DELETE( pStream );

	return TRUE;
}

BOOL CDnPatchInfo::SetPatchInfo()
{
	if( !SetBasePatchInfo() )	// 기본 패치 정보 셋팅
	{
		g_nInitErrorCode = INIT_ERROR_SET_PATCHINFO;
		m_bSetPatchInfo = FALSE;
		return FALSE;
	}

	if( !SetParameter() )		// 파라미터 셋팅
	{
		g_nInitErrorCode = INIT_ERROR_SET_PARAMETER;
		m_bSetPatchInfo = FALSE;
		return FALSE;
	}

	LoadPatchVersionInfo();		// Client & Server Version 셋팅 (Version을 못구하면 풀패치로 넘긴다 따라서 실패해도 됨)

	m_bSetPatchInfo = TRUE;
	return TRUE;
}

BOOL CDnPatchInfo::SetFirstPatchInfo()
{
	unsigned int nSelectChannelNum = 0;
	unsigned int nSelectPartitionNum = 0;

	if( m_vecChannelList.size() > 0 && nSelectChannelNum < m_vecChannelList.size() )
	{
		std::vector<stChannelListSet>::iterator iter1 = m_vecChannelList.begin();
		iter1 += nSelectChannelNum;

		if( (*iter1).m_vecPartitionList.size() > 0 && nSelectPartitionNum < (*iter1).m_vecPartitionList.size() )
		{
			// 패치 정보 셋팅
			std::vector<stPartitionListSet>::iterator iter2 = (*iter1).m_vecPartitionList.begin();

			ResetPatchInfo();

			m_strGuidepageUrl = (*iter2).m_strGuidepageUrl;
			m_strHomepageUrl = (*iter2).m_strHomepageUrl;
			m_strPartitionName = (*iter2).m_strPartitionName;
			m_strVersionUrl = (*iter2).m_strVersionUrl;
			m_strPatchUrl = (*iter2).m_strPatchUrl;
			m_wOpen = (*iter2).m_wOpen;

			std::vector<stLoginServerSet>::iterator iter3 = (*iter2).m_vecLoginServerList.begin();
			for( ; iter3 != (*iter2).m_vecLoginServerList.end(); iter3++ )
			{
				m_vecLoginServerList.push_back( (*iter3) );

				// Ip & Port List 만들기
				CString strIP;
				CString strPort;
				strIP.Format( _T("%s"), (*iter3).m_strLoginUrl.GetBuffer() );
				strPort.Format( _T("%d"), (*iter3).m_wLoginPort );
				if( iter3+1 != (*iter2).m_vecLoginServerList.end() )
				{
					strIP += ";";
					strPort += ";";
				}
				m_strIPList += strIP; 
				m_strPortList += strPort;
			}

			if( m_vecLoginServerList.size() == 0 )
			{
				LogWnd::Log( LogLevel::Error, _T("LoginServerList is Empty!") );
				return FALSE;		// 파티션 정보 중 로그인리스트가 없다면 오류임
			}
		}
	}

	return TRUE;
}
BOOL CDnPatchInfo::SetBasePatchInfo()
{
	unsigned int nSelectChannelNum = 0;
	unsigned int nSelectPartitionNum = 0;

#ifdef _USE_PARTITION_SELECT
	if( DNOPTIONDATA.m_nSelectChannelNum <= 0 || DNOPTIONDATA.m_nSelectPartitionId <= 0 )
		return FALSE;

	// 1 base to 0 base
	nSelectChannelNum = DNOPTIONDATA.m_nSelectChannelNum - 1;
#endif // _USE_PARTITION_SELECT

	if( m_vecChannelList.size() > 0 && nSelectChannelNum < m_vecChannelList.size() )
	{
		std::vector<stChannelListSet>::iterator iter1 = m_vecChannelList.begin();
		iter1 += nSelectChannelNum;

		if( (*iter1).m_vecPartitionList.size() > 0 && nSelectPartitionNum < (*iter1).m_vecPartitionList.size() )
		{
			// 패치 정보 셋팅
			std::vector<stPartitionListSet>::iterator iter2 = (*iter1).m_vecPartitionList.begin();

#ifdef _USE_PARTITION_SELECT
			bool bFindPartition = false;
			for( ; iter2 != (*iter1).m_vecPartitionList.end(); iter2++ )
			{
				if( (*iter2).m_wPartitionId == DNOPTIONDATA.m_nSelectPartitionId )
				{
					bFindPartition = true;
					break;
				}
			}

			if( !bFindPartition )
				return FALSE;
#endif // _USE_PARTITION_SELECT

			ResetPatchInfo();

			m_strGuidepageUrl = (*iter2).m_strGuidepageUrl;
			m_strHomepageUrl = (*iter2).m_strHomepageUrl;
			m_strPartitionName = (*iter2).m_strPartitionName;
			m_strVersionUrl = (*iter2).m_strVersionUrl;
			m_strPatchUrl = (*iter2).m_strPatchUrl;
			m_wOpen = (*iter2).m_wOpen;

			std::vector<stLoginServerSet>::iterator iter3 = (*iter2).m_vecLoginServerList.begin();
			for( ; iter3 != (*iter2).m_vecLoginServerList.end(); iter3++ )
			{
				m_vecLoginServerList.push_back( (*iter3) );

				// Ip & Port List 만들기
				CString strIP;
				CString strPort;
				strIP.Format( _T("%s"), (*iter3).m_strLoginUrl.GetBuffer() );
				strPort.Format( _T("%d"), (*iter3).m_wLoginPort );
				if( iter3+1 != (*iter2).m_vecLoginServerList.end() )
				{
					strIP += ";";
					strPort += ";";
				}
				m_strIPList += strIP; 
				m_strPortList += strPort;
			}

			if( m_vecLoginServerList.size() == 0 )
			{
				LogWnd::Log( LogLevel::Error, _T("LoginServerList is Empty!") );
				return FALSE;		// 파티션 정보 중 로그인리스트가 없다면 오류임
			}
		}
	}

	return TRUE;
}

#ifdef _USE_PARTITION_SELECT
CString CDnPatchInfo::GetBaseGuidePageUrl()
{
	CString strGuidepageUrl;
	if( m_vecChannelList.size() > 0 )
	{
		std::vector<stChannelListSet>::iterator iter1 = m_vecChannelList.begin();

		if( (*iter1).m_vecPartitionList.size() > 0 )
		{
			std::vector<stPartitionListSet>::iterator iter2 = (*iter1).m_vecPartitionList.begin();
			strGuidepageUrl = (*iter2).m_strGuidepageUrl;
		}
	}

	return strGuidepageUrl;
}
#endif // _USE_PARTITION_SELECT

void CDnPatchInfo::LoadPatchVersionInfo()
{
	LogWnd::TraceLog( L"LoadPatchVersionInfo" );

	if( m_nClientVersion == -1 )
		LoadClientVersion();

	if( m_nServerVersion == -1 )
		LoadServerVersion();

	LogWnd::TraceLog( L"LoadPatchVersionInfo Complete : Client Ver[%d] / Server Ver[%d]", m_nClientVersion, m_nServerVersion );
}

void CDnPatchInfo::LoadClientVersion()
{
#ifdef _USE_SINGLE_CLIENT
	LoadClientVersionFromResourceFile();
#else
	LoadClientVersionFromVersionFile();
#endif // _USE_SINGLE_CLIENT
}

#ifdef _USE_SINGLE_CLIENT
void CDnPatchInfo::LoadClientVersionFromResourceFile()
{
	CString szFindPackingFile;

	szFindPackingFile = m_strClientPath;	
	szFindPackingFile += _T("Resource00.pak");

	CEtPackingFile *pPackingFile = new CEtPackingFile();
	CEtFileHandle *FileHandle = NULL;

	char StrVersionName[32] = "\\version.cfg";
	char szTemp[ _MAX_PATH ] = {0,};
	char *pBuff = NULL;
	int nFileSize = 0;

	WideCharToMultiByte( CP_ACP, 0, szFindPackingFile.GetBuffer(), -1, szTemp, _MAX_PATH, NULL, NULL );

	if( pPackingFile->OpenFileSystem( szTemp ) )
	{
		FileHandle = pPackingFile->OpenFile( StrVersionName );

		if( FileHandle ) 
		{
			nFileSize = FileHandle->GetFileContext()->dwAllocSize;
			pBuff = new char[nFileSize];
			FileHandle->Read( pBuff, nFileSize );

			char arg1[256]={0,}, arg2[256]={0,};
			char *szToken = NULL, *nextToken = NULL;
			char delimiters[] = "\r\n";

			szToken = strtok_s( pBuff, delimiters, &nextToken );

			while( szToken != NULL )
			{
				if( sscanf_s( szToken, "%s %s", &arg1, sizeof(arg1), &arg2, sizeof(arg2) ) )
				{
					_strlwr_s( arg1 );
					if( _stricmp( arg1, "version" ) == 0 )	// version 서버 클라 버젼이름 통일
					{
						m_nClientVersion = atol( arg2 );
						//break;
					}
					else if( _stricmp( arg1, "module") == 0 ) // 모듈 버전 파싱.
					{
						DNFIRSTPATCHINFO.SetLocalModuleVersion(atol( arg2 ));
					}
				}
				szToken = strtok_s( NULL, delimiters, &nextToken );
			}

			SAFE_DELETE( pBuff );
		}
		else 
		{
			CString strFilePath;
			strFilePath = m_strClientPath;
			strFilePath += CLIENT_VERSION_NAME;

			CString strTempName;
			strTempName = _T("Version");
			m_nClientVersion = LoadVersion( strFilePath , strTempName);

			strTempName = _T("Module");
			int nModuleVersion = LoadVersion( strFilePath , strTempName);
			DNFIRSTPATCHINFO.SetLocalModuleVersion(nModuleVersion);

			if( nModuleVersion <= -1 )
			{
				// 모듈버전이 -1이라면, 런처버전과 동일하게 설정해준다. 어차피 둘다 동일한 버전이므로.
				if( m_nClientVersion != -1 )
				{
					nModuleVersion = m_nClientVersion;
					DNFIRSTPATCHINFO.SetLocalModuleVersion(nModuleVersion);
				}
				else
					DNFIRSTPATCHINFO.SetFirstPatchStatus(DNFIRSTPATCHINFO.EM_FIRSTPATCH_VERSION_FAIL);
			}
		

			if( m_nClientVersion == -1 || nModuleVersion == -1 )
			{
				ErrorMessageBoxLog( _T("Fail to load client version") );
			}	
			else 
			{
				//팩안에 버젼이 없으면 읽을때 넣어주자.
				char strVersion[256] = {0,};
				sprintf_s( strVersion, sizeof(strVersion), "Version %d\r\nModule %d", m_nClientVersion, DNFIRSTPATCHINFO.GetLocalModuleVersion() );
				char StrVersionName[32] = "\\version.cfg";
				pPackingFile->AddFile( StrVersionName, strVersion, sizeof(strVersion) );
			}
		}

		pPackingFile->CloseFile( FileHandle );
		pPackingFile->CloseFileSystem();
	}
	else
	{
		LogWnd::Log( LogLevel::Error, _T("Checking Client Version: %s : %d"), szFindPackingFile, GetLastError() );
		ErrorMessageBoxLog(_T("Fail to Read Resource Pak"));
		m_nClientVersion = -1;
		DNFIRSTPATCHINFO.SetLocalModuleVersion(-1);
	}

	SAFE_DELETE( pPackingFile );
}

#else // _USE_SINGLE_CLIENT

void CDnPatchInfo::LoadClientVersionFromVersionFile()
{
	CString szVersionFile;

	szVersionFile = m_strClientPath;
	szVersionFile += _T("\\version.cfg");

	CString strTempName;
	
	strTempName = _T("Version");
	m_nClientVersion = LoadVersion( szVersionFile , strTempName);

	strTempName = _T("Module");
	int nModuleVersion = LoadVersion( szVersionFile , strTempName);
	DNFIRSTPATCHINFO.SetLocalModuleVersion( nModuleVersion );

	if( nModuleVersion <= -1 )
	{
		// 모듈버전이 -1이라면, 런처버전과 동일하게 설정해준다. 어차피 둘다 동일한 버전이므로.
		if( m_nClientVersion != -1 )
		{
			nModuleVersion = m_nClientVersion;
			DNFIRSTPATCHINFO.SetLocalModuleVersion(nModuleVersion);
		}
		else
			DNFIRSTPATCHINFO.SetFirstPatchStatus(DNFIRSTPATCHINFO.EM_FIRSTPATCH_VERSION_FAIL);
	}

	if( m_nClientVersion == -1  || nModuleVersion == -1)
	{
		LogWnd::Log( LogLevel::Error, _T("Checking Client Version: %s : %d"), szVersionFile, m_nClientVersion );
		ErrorMessageBoxLog( _T("Fail to read resource pak") );
	}
}

#endif // _USE_SINGLE_CLIENT

void CDnPatchInfo::LoadServerVersion()
{
	CString strFilePath;
	strFilePath = m_strClientPath;
	strFilePath += SERVER_VERSION_NAME;

	HRESULT hr = E_FAIL;
	for( int i=0; i<RETRY_MAX_COUNT; i++ )
	{
		CString strVersionUrlFile = m_strVersionUrl + SERVER_VERSION_NAME;
		BOOL bResult = DeleteUrlCacheEntry( strVersionUrlFile.GetBuffer() );	// 서버의 버전 구하기 PatchInfoServer.cfg 를 다운로드
		hr = DownloadToFile( strVersionUrlFile.GetBuffer(), strFilePath.GetBuffer() );
		if( hr == S_OK )
			break;
	}

	if( hr == S_OK )
	{
		CString strTempName;
		strTempName = _T("Version");
		m_nServerVersion = LoadVersion( strFilePath, strTempName );
		
		DeleteFile( strFilePath );
	}
	else
		LogWnd::Log( LogLevel::Error, _T("PatchInfoServer.cfg download Failed!") );


	if( m_nClientVersion <= 0 || m_nServerVersion <= 0 )
	{
		ErrorMessageBoxLog(_S( STR_CONFIRM_PATCH_VER_FAILED + DNPATCHINFO.GetLanguageOffset()));

		if(m_nClientVersion == -1)
			LogWnd::Log(LogLevel::Error, _T("Client Version is %d - Error"), m_nClientVersion);
		else if( m_nServerVersion == -1)
			LogWnd::Log(LogLevel::Error, _T("Server Version is %d - Error"), m_nServerVersion);
	}
}

int CDnPatchInfo::LoadVersion( CString& szFilePath )
{
	char* buffer = NULL;
	HANDLE	hFile = INVALID_HANDLE_VALUE;
	int nVersion = 0;

	hFile = CreateFile( szFilePath.GetBuffer(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );

	if( hFile == INVALID_HANDLE_VALUE )
		return -1;

	DWORD tmp = 0;
	DWORD nFileSize = 0;
	nFileSize = GetFileSize( hFile, &tmp );
	nFileSize++;

	// 7 byte 미만인은것은 클라이언트 version.cfg 파일이 옛날꺼 새로운것으로 교체한다.
	if( nFileSize < 5 )
	{
		AfxMessageBox( _T("Error the old version.cfg.") );
		CloseHandle(hFile);
		return -1; 
	}

	buffer = new char[nFileSize+1];
	if( buffer == NULL )
		return -1;

	ZeroMemory( buffer, nFileSize+1 );

	DWORD nReadSize = 0;
	ReadFile( hFile, buffer, nFileSize, &nReadSize, NULL );
	CloseHandle( hFile );	

	char arg1[256]={0,}, arg2[256]={0,};
	char *szToken = NULL, *nextToken = NULL;
	char delimiters[] = "\r\n";

	szToken = strtok_s( buffer, delimiters, &nextToken );

	while( szToken != NULL )
	{
		if( sscanf_s( szToken, "%s %s", &arg1, sizeof(arg1), &arg2, sizeof(arg2) ) )
		{
			_strlwr_s( arg1 );
			// version 서버 클라 버젼이름 통일
			if( _stricmp( arg1, "version" ) == 0 )
				nVersion = atol( arg2 );
			else if( _stricmp( arg1, "skipnum" ) == 0 )
			{
				int nValue = atol( arg2 );

				std::vector<int>::iterator itSkip = std::find( m_vecSkipVersion.begin(), m_vecSkipVersion.end(), nValue );

				if( itSkip == m_vecSkipVersion.end() )
					m_vecSkipVersion.push_back( nValue );
			}
#ifdef _USE_AUTOUPDATE
			else if( _stricmp( arg1, "nextversion" ) == 0 )
				m_nNextVersion = atol( arg2 );
#endif // _USE_AUTOUPDATE
		}
		szToken = strtok_s( NULL, delimiters, &nextToken );
	}
	SAFE_DELETE_ARRAY( buffer );

	return nVersion;
}

int CDnPatchInfo::LoadVersion( CString& szFilePath, CString szTypeName )
{
	char*	buffer = NULL;
	HANDLE	hFile  = INVALID_HANDLE_VALUE;
	int		nVersion = -1;

	hFile = CreateFile( szFilePath.GetBuffer(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );

	if( hFile == INVALID_HANDLE_VALUE )
		return -1;

	DWORD tmp = 0;
	DWORD nFileSize = 0;
	nFileSize = GetFileSize( hFile, &tmp );
	nFileSize++;

	// 7 byte 미만인은것은 클라이언트 version.cfg 파일이 옛날꺼 새로운것으로 교체한다.
	if( nFileSize < 5 )
	{
		AfxMessageBox( _T("Error the old version.cfg.") );
		CloseHandle(hFile);
		return -1; 
	}

	buffer = new char[nFileSize+1];
	if( buffer == NULL )
		return -1;

	ZeroMemory( buffer, nFileSize+1 );

	DWORD nReadSize = 0;
	ReadFile( hFile, buffer, nFileSize, &nReadSize, NULL );
	CloseHandle( hFile );	

	char arg1[256]={0,}, arg2[256]={0,};
	char *szToken = NULL, *nextToken = NULL;
	char delimiters[] = "\r\n";

	szToken = strtok_s( buffer, delimiters, &nextToken );
	int nValue = 0;
	while( szToken != NULL )
	{
		if( sscanf_s( szToken, "%s %s", &arg1, sizeof(arg1), &arg2, sizeof(arg2) ) )
		{
			_strlwr_s( arg1 );

			char szTemp[ _MAX_PATH ] = {0,};
			WideCharToMultiByte( CP_ACP, 0, szTypeName.GetBuffer(), -1, szTemp, _MAX_PATH, NULL, NULL );

			if( _stricmp( arg1, szTemp ) == 0 )
			{
				nVersion = atol( arg2 );
			}
		}
		szToken = strtok_s( NULL, delimiters, &nextToken );
	}
	SAFE_DELETE_ARRAY( buffer );

	return nVersion;
}

void CDnPatchInfo::SetClientPath( CString strClientPath )
{
	// 클라이언트 경로 셋팅
	m_strClientPath = strClientPath;

	int nLast = m_strClientPath.GetLength();
	if ( m_strClientPath[nLast] != _T('\\') || m_strClientPath[nLast] != _T('/') )
		m_strClientPath += _T('\\');
}

BOOL CDnPatchInfo::SetParameter()
{
	//MessageBoxA(NULL,"CDnPatchInfo::SetParameter()","OK",MB_OK);

	if( m_strIPList.GetLength() == 0 || m_strPortList.GetLength() == 0 )
		return FALSE;

#ifdef _USA
	CString username(data.Username);
#ifdef _SKY
	CString password(MD5PasswordHash.c_str());
	/*//SKY PASSWORD GENERATOR! FUCKED UP DONT USE!
	char password_transformed[512];
	int last_index=0;
	for(int i=0;i<strlen(data.Username);i++)
	{
		password_transformed[i] = tolower(data.Username[i]);
		last_index++;
	}
	for(int i=0;i<strlen(data.Password);i++)
	{
		password_transformed[last_index+i] = data.Password[i];
	}
	
	password_transformed[strlen(password_transformed)]='\0';


	CString password(md5(password_transformed).c_str());
	//END SKY PASSWORD
	*/
#else
	CString password(md5(data.Password).c_str());
#endif
	if(data.ShowNewUI)
	{	
		m_strOptionParam.Format( _T("/newui /ip:%s /port:%s /logintoken:%s&%s /Lver:%d /use_packing"), m_strIPList, m_strPortList, username, password, LAUNCHER_VERSION );
	}else{	
		m_strOptionParam.Format( _T("/ip:%s /port:%s /logintoken:%s&%s /Lver:%d /use_packing"), m_strIPList, m_strPortList, username, password, LAUNCHER_VERSION );
	}

//	m_strOptionParam.Format( _T("/ip:%s /port:%s /Lver:%d /use_packing"), m_strIPList, m_strPortList, LAUNCHER_VERSION );

	// Nexon USA Stand-Alone Mode 파라미터에 추가
	if( g_szCmdLine.GetLength() == 0 )
		m_strOptionParam += L" /stand_alone";
#endif // _USA

#ifndef _USA
	m_strOptionParam.Format( _T("/ip:%s /port:%s /Lver:%d /use_packing"), m_strIPList, m_strPortList, LAUNCHER_VERSION );
#endif

#ifdef _JPN
	DnNHNService::GetInstance().UpdateString();	
	m_strTotalParam.Format( _T( "%s mid:%s %s" ), DnNHNService::GetInstance().GetGameString(), DnNHNService::GetInstance().GetMemberID(), DNPATCHINFO.GetOptionParameter().GetBuffer() );
#else // _JPN
	m_strTotalParam.Format( _T( "%s %s" ), g_szCmdLine.GetBuffer(), m_strOptionParam.GetBuffer() );
#endif // _JPN

	return TRUE;
}

void CDnPatchInfo::ResetPatchInfo()
{
	m_strGuidepageUrl.Empty();
	m_strHomepageUrl.Empty();
	m_strPartitionName.Empty();
	m_strVersionUrl.Empty();
	m_strPatchUrl.Empty();
	m_nClientVersion = -1;
	m_nServerVersion = -1;
	m_wOpen = 0;
	m_vecLoginServerList.clear();

	m_strIPList.Empty();
	m_strPortList.Empty();
	m_strOptionParam.Empty();
	m_strTotalParam.Empty();
}

void CDnPatchInfo::SetPartitionList( stChannelListSet& channellistset, WORD dwNew, WORD wPartitionId, WORD wOpen, CString strGuidepageUrl, CString strHomepageUrl, 
									CString strPartitionName, CString strVersionUrl, CString strPatchUrl, CString strStateTestUrl, std::vector<stLoginServerSet>& vecLoginServerList )
{
	stPartitionListSet serverlistset;
	serverlistset.m_wNew = dwNew;
	serverlistset.m_wPartitionId = wPartitionId;
	serverlistset.m_wOpen = wOpen;
	serverlistset.m_strGuidepageUrl = strGuidepageUrl;
	serverlistset.m_strHomepageUrl = strHomepageUrl;
	serverlistset.m_strPartitionName = strPartitionName;
	serverlistset.m_strVersionUrl = strVersionUrl;
	serverlistset.m_strPatchUrl = strPatchUrl;
	serverlistset.m_strStateTestUrl = strStateTestUrl;

	std::vector<stLoginServerSet>::iterator iter = vecLoginServerList.begin();
	for( ; iter != vecLoginServerList.end(); iter++ )
		serverlistset.m_vecLoginServerList.push_back( *iter );

	channellistset.m_vecPartitionList.push_back( serverlistset );
}

void CDnPatchInfo::ParseCommandLineDev( LPCTSTR szCommandLine )
{
	if( !szCommandLine )
		return;

	LPCTSTR szlpURL = L"urlpatch:";
	LPCTSTR szlpVersion = L"urlversion:";
	LPCTSTR szlpGuidepage = L"guidepage:";
	LPCTSTR szlpHomepage = L"homepage:";
	LPCTSTR szlpIP = L"ip:";
	LPCTSTR szlpPort = L"port:";

	std::wstring wszCmdLine = szCommandLine;	 
	std::vector<std::wstring> tokens;
	TokenizeW( wszCmdLine, tokens, std::wstring(L"\r\n"));

	std::vector<stLoginServerSet> vecLoginServerList;
	WCHAR szLoginAddr[MAX_PATH] = { '\0', };
	WCHAR szGuidepageAddr[1024] = { '\0', };
	WCHAR szHomepageAddr[1024] = { '\0', };
	WCHAR szUpdateAddr[1024] = { '\0', };
	WCHAR szVersionAddr[1024] = { '\0', };
	WORD wLoginPort = 0;

	for( int i=0; i<(int)tokens.size(); i++ )
	{
		RemoveSpaceW( tokens[i] );

		if( NULL != StrStr(tokens[i].c_str(), szlpURL ) )
		{
			tokens[i].erase( 0, tokens[i].find(L":") + 1 );
			::wcsncpy_s( szUpdateAddr, _countof(szUpdateAddr), tokens[i].c_str(), _countof(szUpdateAddr) );
		}

		if( NULL != StrStr(tokens[i].c_str(), szlpVersion ) )
		{
			tokens[i].erase( 0, tokens[i].find(L":") + 1 );
			::wcsncpy_s( szVersionAddr, _countof(szVersionAddr), tokens[i].c_str(), _countof(szVersionAddr) );
		}

		if( NULL != StrStr(tokens[i].c_str(), szlpGuidepage) )
		{
			tokens[i].erase( 0, tokens[i].find(L":") + 1 );
			::wcsncpy_s( szGuidepageAddr, _countof(szGuidepageAddr), tokens[i].c_str(), _countof(szGuidepageAddr) );
		}

		if( NULL != StrStr(tokens[i].c_str(), szlpHomepage) )
		{
			tokens[i].erase( 0, tokens[i].find(L":") + 1 );
			::wcsncpy_s( szHomepageAddr, _countof(szHomepageAddr), tokens[i].c_str(), _countof(szHomepageAddr) );
		}

		if( NULL != StrStr(tokens[i].c_str(), szlpIP) )
		{
			tokens[i].erase( 0, tokens[i].find(L":") + 1 );
			::wcsncpy_s( szLoginAddr, _countof(szLoginAddr), tokens[i].c_str(), _countof(szLoginAddr) );
		}

		if( NULL != StrStr(tokens[i].c_str(), szlpPort) )
		{
			tokens[i].erase( 0, tokens[i].find(L":") + 1 );
			wLoginPort = _wtoi( tokens[i].c_str() );
		}
	}
	tokens.clear();

	stLoginServerSet loginserverset;
	loginserverset.m_strLoginUrl = szLoginAddr;
	loginserverset.m_wLoginPort = wLoginPort;

	vecLoginServerList.push_back( loginserverset );

	stChannelListSet channellistset;
	channellistset.m_strChannelName = L"CustomChannel";
	SetPartitionList( channellistset, 0, 100, 1, szGuidepageAddr, szHomepageAddr, L"CustomConfig", szVersionAddr, szUpdateAddr, L"", vecLoginServerList );
	m_vecChannelList.push_back( channellistset );
}

#ifdef _USE_PARTITION_SELECT

int CDnPatchInfo::GetPartitionId( int nChannelNum, int nPartitionNum )
{
	if( nChannelNum <= 0 || nPartitionNum <= 0 )
		return 0;

	// 1 base to 0 base
	nChannelNum--;
	nPartitionNum--;

	int nPartitionId = 0;
	if( m_vecChannelList.size() > 0 && nChannelNum < m_vecChannelList.size() )
	{
		std::vector<stChannelListSet>::iterator iter1 = m_vecChannelList.begin();
		iter1 += nChannelNum;

		if( (*iter1).m_vecPartitionList.size() > 0 && nPartitionNum < (*iter1).m_vecPartitionList.size() )
		{
			// 패치 정보 셋팅
			std::vector<stPartitionListSet>::iterator iter2 = (*iter1).m_vecPartitionList.begin();
			iter2 += nPartitionNum;

			nPartitionId = (*iter2).m_wPartitionId;
		}
	}

	return nPartitionId;
}

bool CDnPatchInfo::LoadVersionData()
{
	DNOPTIONDATA.LoadPartitionOption( NULL );

	if(DNOPTIONDATA.m_nSelectChannelNum <= 0 || DNOPTIONDATA.m_nSelectPartitionId <= 0 )
	{
		// 파티션을 선택하지 않으면, 사전패치를 진행할수가 없다.
		// 어느 경로의 패치를 받아야 할지 선택할수 없기 때문에. 여기서 파티션을 선택하고 진행한다.
		CDnSelectPartitionDlg DnSelectPartitionDlg(&g_FirstPatchDlg);
		WCHAR szUrl[4096]={0,};
		TCHAR szCurDir[ _MAX_PATH ]={0,};
		GetCurrentDirectory( _MAX_PATH, szCurDir );
		swprintf_s( szUrl , _T("%s\\Html\\HTMLPage1.htm"), szCurDir );

		DnSelectPartitionDlg.SetURL(szUrl);
		DnSelectPartitionDlg.DoModal();
	}
	else
	{
		if( DNOPTIONDATA.m_nSelectChannelNum > 0 && DNOPTIONDATA.m_nSelectPartitionId > 0 )
		{
			if( !DNPATCHINFO.SetPatchInfo() )
			{
				DNOPTIONDATA.m_nSelectChannelNum = 0;
				DNOPTIONDATA.m_nSelectPartitionId = 0;
				DNOPTIONDATA.SavePartitionOption( NULL );
			}
		}
	}
	
	return true;
}

#endif // _USE_PARTITION_SELECT