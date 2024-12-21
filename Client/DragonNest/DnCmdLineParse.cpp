#include "StdAfx.h"

#include <stdio.h>
#include <string.h>

#include "DnCmdLineParse.h"
#include "GlobalValue.h"
#include "TaskManager.h"
#include "DebugCmdProc.h"
#include "DnMainFrame.h"
#include "GameOption.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

enum CmdLineIndexEnum {
	CMDLINE_IP,
	CMDLINE_PORT,
	CMDLINE_UNUSEPACK,
	CMDLINE_THEMIDA_HSBGEN,
	CMDLINE_UITEST,
#ifdef PRE_ADD_MULTILANGUAGE
	CMDLINE_LANGUAGE,
#endif // PRE_ADD_MULTILANGUAGE
#ifdef PRE_ADD_STEAMWORKS
	CMDLINE_STEAMWORKS,
#endif // PRE_ADD_STEAMWORKS
#ifndef _FINAL_BUILD
	CMDLINE_REQUEST_TIME,
	CMDLINE_KEY_ID,
	CMDLINE_VERSION,

	CMDLINE_WINDOW,
	CMDLINE_RES,
	CMDLINE_DEBUGTASK,
	CMDLINE_FPS,
	CMDLINE_VILLAGEID,
	CMDLINE_VILLAGEPASS,
	CMDLINE_SINGLETEST,
	CMDLINE_ENVI,
	CMDLINE_PLAYER,
	CMDLINE_SOUND,
	CMDLINE_LEVEL,
	CMDLINE_LOGWND,
	CMDLINE_NATION,
	CMDLINE_NODAMAGE,
	CMDLINE_SERVERNAME,
	CMDLINE_CHANNELINDEX,
	CMDLINE_RESPATH,
	CMDLINE_USEPACK,
	CMDLINE_DUMPLEVEL,
	CMDLINE_ASSERT,
	CMDLINE_USEWINKEY,
	CMDLINE_NOHSHIELD,
	CMDLINE_SECONDAUTH,
	CMDLINE_RENEWUI,
#endif
	CmdLineIndexEnum_END,
};


CDnCmdLineParse::CmdLineDefineStruct g_CmdLineDefineList[] = {
	{ CMDLINE_IP, _T("ip") },
	{ CMDLINE_PORT, _T("port") },
	{ CMDLINE_UNUSEPACK, _T("unuse_packing") },
	{ CMDLINE_THEMIDA_HSBGEN, _T("themida_hsbgen") },
	{ CMDLINE_UITEST, _T("uitest")},
#ifdef PRE_ADD_MULTILANGUAGE
	{ CMDLINE_LANGUAGE, _T("language") },
#endif // PRE_ADD_MULTILANGUAGE
#ifdef PRE_ADD_STEAMWORKS
	{ CMDLINE_STEAMWORKS, _T("steam") },
#endif // PRE_ADD_STEAMWORKS
#ifndef _FINAL_BUILD
	{ CMDLINE_REQUEST_TIME, _T("requesttime") },
	{ CMDLINE_KEY_ID,  _T("keyid") },
	{ CMDLINE_VERSION, _T("lver")},

	{ CMDLINE_WINDOW, _T("window") },
	{ CMDLINE_RES, _T("res") },
	{ CMDLINE_DEBUGTASK, _T("debugtask") },
	{ CMDLINE_FPS, _T("fps") },
	{ CMDLINE_VILLAGEID, _T("id") },
	{ CMDLINE_VILLAGEPASS, _T("pass") },
	{ CMDLINE_SINGLETEST, _T("single") },
	{ CMDLINE_ENVI, _T("envi") },
	{ CMDLINE_PLAYER, _T("player") },
	{ CMDLINE_SOUND, _T("nosound") },
	{ CMDLINE_LEVEL, _T("level") },
	{ CMDLINE_LOGWND, _T("logwnd") },
	{ CMDLINE_NATION, _T("nation") },
	{ CMDLINE_NODAMAGE, _T("nodamage") },
	{ CMDLINE_SERVERNAME, _T("server") },
	{ CMDLINE_CHANNELINDEX, _T("channel") },
	{ CMDLINE_RESPATH, _T("respath") },
	{ CMDLINE_USEPACK, _T("use_packing") },
	{ CMDLINE_DUMPLEVEL, _T("dumplevel") },
	{ CMDLINE_ASSERT, _T("assert") },
	{ CMDLINE_USEWINKEY, _T("winkey")},
	{ CMDLINE_NOHSHIELD, _T("nohs")},
	{ CMDLINE_SECONDAUTH, _T("secondpass")},
	{ CMDLINE_RENEWUI, _T("uirenew") },
#endif
	{ CmdLineIndexEnum_END, NULL },
};

void CDnCmdLineParse::DetectedCommand( CmdLineIndexEnum Index )
{
	switch( Index ) {
		case CMDLINE_IP:
			{
				char szTemp[256] = { 0, };
				WideCharToMultiByte( CP_ACP, 0, m_szParam, -1, szTemp, 256, NULL, NULL );
				CGlobalInfo::GetInstance().m_szIP = szTemp;
			}
			break;
		case CMDLINE_PORT:
			{
				char szTemp[256] = { 0, };
				WideCharToMultiByte( CP_ACP, 0, m_szParam, -1, szTemp, 256, NULL, NULL );
				CGlobalInfo::GetInstance().m_szPort = szTemp;
			}
			break;
		case CMDLINE_UNUSEPACK:
			CGlobalInfo::GetInstance().m_bUsePacking = false;
			break;
		case CMDLINE_THEMIDA_HSBGEN:
			CGlobalInfo::GetInstance().m_bThemida_hsbgen = true;
			break;
		case CMDLINE_UITEST:
			CGlobalInfo::GetInstance().m_bUITest = true;
			break;
#ifdef PRE_ADD_MULTILANGUAGE
		case CMDLINE_LANGUAGE:
			{
				char szTemp[64] = { 0, };
				WideCharToMultiByte( CP_ACP, 0, m_szParam, -1, szTemp, 64, NULL, NULL );
				std::string strLanguage = szTemp;
				std::transform( strLanguage.begin(), strLanguage.end(), strLanguage.begin(), toupper );
				CGlobalInfo::GetInstance().m_szLanguage = strLanguage;

				bool bChecker = false;
				for (int i = 0; i < MultiLanguage::SupportLanguage::NationMax; i++)
				{
					if (strstr(MultiLanguage::NationString[i].c_str(), CGlobalInfo::GetInstance().m_szLanguage.c_str()))
					{
						CGlobalInfo::GetInstance().m_eLanguage = static_cast<MultiLanguage::SupportLanguage::eSupportLanguage>(i);
						if( i == 0 )
							CGlobalInfo::GetInstance().m_szLanguage = "";
						bChecker = true;
						break;
					}
				}

				_ASSERT(bChecker == true);
			}
			break;
#endif // PRE_ADD_MULTILANGUAGE
#ifdef PRE_ADD_STEAMWORKS
		case CMDLINE_STEAMWORKS:
			CGlobalInfo::GetInstance().m_bUseSteamworks = true;
			break;
#endif // PRE_ADD_STEAMWORKS
#ifndef _FINAL_BUILD
		case CMDLINE_REQUEST_TIME:
			{
				std::string tempStr;
				ToMultiString(m_szParam,tempStr);
				TrimString(const_cast<char*>(tempStr.c_str()));
				CGlobalValue::GetInstance().m_szRequestTime = tempStr;
			}
			break;
		case CMDLINE_KEY_ID:
			{
				std::string tempStr;
				ToMultiString(m_szParam,tempStr);
				TrimString(const_cast<char*>(tempStr.c_str()));
				CGlobalValue::GetInstance().m_szKeyID = tempStr;
			}
			break;

		case CMDLINE_WINDOW:
			if( _tcscmp( m_szParam, _T("true") ) == NULL )
				CGameOption::GetInstance().m_bWindow = TRUE;
			else if( _tcscmp( m_szParam, _T("false") ) == NULL )
				CGameOption::GetInstance().m_bWindow = FALSE;
			break;
		case CMDLINE_RES:
			{
				char szTemp[64] = { 0, };
				WideCharToMultiByte( CP_ACP, 0, m_szParam, -1, szTemp, 64, NULL, NULL );

				sscanf_s( szTemp, "%dx%d", &CGameOption::GetInstance().m_nWidth, &CGameOption::GetInstance().m_nHeight );
			}
			break;
		case CMDLINE_DEBUGTASK:
			CTaskManager::GetInstance().EnableDebugTaskMode( true );
			break;
		case CMDLINE_FPS:
			s_fDefaultFps = (float)_ttoi( m_szParam );
			break;
		case CMDLINE_VILLAGEID:
			CGlobalValue::GetInstance().m_szID = m_szParam;
			break;
		case CMDLINE_VILLAGEPASS:
			CGlobalValue::GetInstance().m_szPass = m_szParam;
			break;
		case CMDLINE_SINGLETEST:
			CGlobalValue::GetInstance().m_bSingleTest = true;
			if( _tcslen( m_szParam ) > 0 ) {
				TCHAR szMapName[64] = { 0, };
				int nArrayIndex = 1;

				swscanf_s( m_szParam, L"%s,%d", szMapName, 64, &nArrayIndex ); 
				CGlobalValue::GetInstance().m_szSingleMapName = szMapName;
				CGlobalValue::GetInstance().m_nSingleMapArrayIndex = nArrayIndex;
			}
			break;
		case CMDLINE_ENVI:
			if( _tcslen( m_szParam ) > 0 )
				CGlobalValue::GetInstance().m_szSingleMapEnviName = m_szParam;
			break;
		case CMDLINE_PLAYER:
			CGlobalValue::GetInstance().m_nLocalPlayerClassID = _ttoi( m_szParam );
			break;
		case CMDLINE_SOUND:
			CGlobalValue::GetInstance().m_bSoundOff = true;
			break;
		case CMDLINE_LEVEL:
			{
				char szTemp[64] = { 0, };
				WideCharToMultiByte( CP_ACP, 0, m_szParam, -1, szTemp, 64, NULL, NULL );

				sscanf_s( szTemp, "%d,%d", &CGlobalValue::GetInstance().m_nPlayerLevel, &CGlobalValue::GetInstance().m_nMapLevel );
			}
			break;
		case CMDLINE_LOGWND:
			{
				CGlobalValue::GetInstance().m_bUseLogWnd = true;
				LogWnd::CreateLog(false,LogWnd::LOG_TARGET_WINDOW/*|LogWnd::LOG_TARGET_FILE*/);
				LogWnd::SetCommandCallBack(DebugCmdProc);
			}
			break;
		case CMDLINE_NATION:
			if( CEtResourceMng::IsActive() && CEtResourceMng::GetInstance().IsUsePackingFile() ) break;
			ToMultiString( m_szParam, CGlobalValue::GetInstance().m_szNation );
			break;
		case CMDLINE_NODAMAGE:
			CGlobalValue::GetInstance().m_bNoDamage = true;
			break;
		case CMDLINE_SERVERNAME:
			CGlobalValue::GetInstance().m_szServerName = m_szParam;
			break;
		case CMDLINE_CHANNELINDEX:
			CGlobalValue::GetInstance().m_nChannelIndex = _ttoi( m_szParam );
			break;
		case CMDLINE_RESPATH:
			ToMultiString( m_szParam, CGlobalValue::GetInstance().m_szResourcePath );
			break;
		case CMDLINE_USEPACK:
			CGlobalInfo::GetInstance().m_bUsePacking = true;
			break;
		case CMDLINE_DUMPLEVEL:
			CGlobalValue::GetInstance().m_nDumpLevel = _ttoi( m_szParam );
			break;
		case CMDLINE_ASSERT:
			EnableAssert( true );
			break;
		case CMDLINE_USEWINKEY:
			CGlobalValue::GetInstance().m_bEnableWinKey = true;
			break;
		case CMDLINE_NOHSHIELD:
			CGlobalInfo::GetInstance().m_bEnableHShield = false;
			break;
		case CMDLINE_SECONDAUTH:
			CGlobalValue::GetInstance().m_szSecondPass = m_szParam;
			break;
	#ifdef PRE_ADD_RENEWUI
		case CMDLINE_RENEWUI:
			CGlobalValue::GetInstance().m_bReNewUI = true;
			break;
	#endif // PRE_ADD_RENEWUI
#endif
	}
}

CDnCmdLineParse::CDnCmdLineParse( LPTSTR szString )
{
#ifdef _FINAL_BUILD
	memset( m_szCmdLine, 0, sizeof(m_szCmdLine) );
	for( int i=0; ; i++ ) {
		if( szString[i] == 0 ) break;
		m_szCmdLine[i] = szString[i];
	}
#else
	CFileStream* pStream = new CFileStream(".\\Config.cfg");
	
	if( pStream && pStream->IsValid() ) {
		char szTemp[2048] = { 0, };
		pStream->Read( szTemp, ( pStream->Size() > 2048 ) ? 2048 : pStream->Size() );
		SAFE_DELETE( pStream );
		MultiByteToWideChar( CP_ACP, 0, szTemp, -1, m_szCmdLine, 2048 );
	}
	else {
		memset( m_szCmdLine, 0, sizeof(m_szCmdLine) );
		for( int i=0; ; i++ ) {
			if( szString[i] == 0 ) break;
			m_szCmdLine[i] = szString[i];
		}
	}
	SAFE_DELETE( pStream );
#endif

	ChangeLowerCase( m_szCmdLine );
	ProcessParse( m_szCmdLine );
}

CDnCmdLineParse::~CDnCmdLineParse()
{
}

void CDnCmdLineParse::ProcessParse( TCHAR *szString )
{
	std::vector<int> nVecList;
	std::vector<int> nSortList;

	for( int i=0; i<(int)CmdLineIndexEnum_END; i++ ) nVecList.push_back(i);

	while( !nVecList.empty() ) {
		int nMaxLength = 0;
		int nLength;
		int nIndex = -1;
		for( DWORD i=0; i<nVecList.size(); i++ ) {
			nLength = (int)_tcslen( g_CmdLineDefineList[nVecList[i]].szString );
			if( nLength > nMaxLength ) nIndex = i;
		}
		nSortList.push_back( nVecList[nIndex] );
		nVecList.erase( nVecList.begin() + nIndex );
	}

	for( DWORD i=0; i<nSortList.size(); i++ ) {
		if( FindString( g_CmdLineDefineList[nSortList[i]].szString ) == true ) {
			DetectedCommand( g_CmdLineDefineList[nSortList[i]].Index );
		}
	};
}

bool CDnCmdLineParse::FindString( TCHAR *szString )
{
	TCHAR szTemp[64];
	_stprintf_s( szTemp, _T("/%s"), szString );

	TCHAR *pFindPtr = _tcsstr( m_szCmdLine, szTemp );
	if( pFindPtr == NULL ) return false;
	if( pFindPtr != m_szCmdLine && pFindPtr[-1] == '/') {
		return false;	/*   '//' 은 주석으로 간주 */
	}

	int nSize = (int)_tcslen(szTemp);
	for( int i=0; i<nSize; i++ ) {
		*pFindPtr = ' ';
		pFindPtr++;
	}

	// 변수 복사.
	memset( m_szParam, 0, sizeof(m_szParam) );
	if( *pFindPtr != ':' ) return true;
	*pFindPtr = ' ';
	pFindPtr++;

	for( int i=0; ; i++ ) 
	{
		if( *pFindPtr == L'/' || *pFindPtr == 0 || *pFindPtr == L' ' || *pFindPtr == L'\r' || *pFindPtr == L'\n' ) 
			break;

		m_szParam[i] = *pFindPtr;
		*pFindPtr = ' ';
		pFindPtr++;
	}

	return true;
}

void CDnCmdLineParse::ChangeLowerCase( TCHAR *szString )
{
	bool bCmd = false;
	int nLength = (int)_tcslen(szString);
	for( int i=0; i<nLength; i++ ) {
		if( szString[i] == '/' ) bCmd = true;
		if( szString[i] == ':' || szString[i] == ' ' ) bCmd = false;
		if( bCmd == false ) continue;

		if( szString[i] >= 'A' && szString[i] <= 'Z' ) szString[i] += 32;
	}
}