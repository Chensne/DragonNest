//#include "StdAfx.h"
//#include "Log.h"
//#include "TimeSet.h"
//
//#if defined( _GAMESERVER )
//#include "DnUserSession.h"
//#include "DNLogConnection.h"
//#if defined( PRE_TRIGGER_UNITTEST_LOG )
//CLog g_TriggerUnitTestLog;
//#endif // #if defined( PRE_TRIGGER_UNITTEST_LOG )
//#if defined( PRE_QUESTSCRIPT_LOG )
//CLog g_QuestLog;
//#endif // #if defined( PRE_QUESTSCRIPT_LOG )
//#if defined( PRE_TRIGGER_LOG )
//CLog g_TriggerLog;
//#endif // #if defined( PRE_TRIGGER_LOG )
//#elif defined( _MASTERSERVER )
//extern TMasterConfig g_Config;
//#include "DNLogConnection.h"
//#include "DNUser.h"
//#elif defined( _VILLAGESERVER )
//#include "DNLogConnection.h"
//#include "DnUserSession.h"
//#elif defined( _LOGINSERVER )
//#include "DNUserConnection.h"
//#include "DNLogConnection.h"
//#include "DNServiceConnection.h"
//#elif defined( _DBSERVER )
//#include "DNLogConnection.h"
//#include "DNServiceConnection.h"
//#elif defined( _CASHSERVER )
//#include "DNLogConnection.h"
//#include "DNServiceConnection.h"
//#include "DNUserRepository.h"
//#endif // #if defined( _GAMESERVER )
//
//CLog g_SPLog;
//CLog g_Log;
//
//#ifdef _DEBUG
//#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
//#endif
//
//CLog::CLog(void): m_pFile(NULL), m_nOldDay(0), m_nOldHour(0), m_cType(LOGTYPE_CRT)
//{
//	memset(&m_wszFileName, 0, sizeof(m_wszFileName));
//	memset(&m_wszDirName, 0, sizeof(m_wszDirName));
//	m_nCounter = 0;
//	m_bActive = false;
//#if defined (_SERVICEMANAGER_EX)
//	m_pDisplayer = NULL;
//#endif // #if defined (_SERVICEMANAGER_EX)
//	m_nServerID = 0;
//}
//
//CLog::~CLog(void)
//{
//	if (m_pFile)
//		fclose(m_pFile);
//}
//
//void CLog::CalcCounter()
//{
//	if( m_cType == LOGTYPE_FILE ) {
//		std::vector<std::string> szVecFileList;
//		FindFileListInDirectory( ".\\Log", "*.log", szVecFileList );
//		int nCount = 0;
//		const char *pszTemp, *pszTemp2;
//		for( DWORD i=0; i<szVecFileList.size(); i++ ) {
//			pszTemp = _GetSubStrByCount( 2, (char*)szVecFileList[i].c_str(), '_' );
//			if( !pszTemp ) continue;
//			pszTemp2 = _GetSubStrByCount( 0, pszTemp, '.' );
//			if( !pszTemp2 ) continue;
//			if( atoi(pszTemp2) > nCount ) nCount = atoi(pszTemp2);
//		}
//		m_nCounter = nCount + 1;
//	}
//}
//
//void CLog::Init(BYTE nType)
//{
//	_wcscpy(m_wszFileName, _countof(m_wszFileName), L"log", (int)wcslen(L"log"));
//	_wcscpy(m_wszDirName, _countof(m_wszDirName), L"log", (int)wcslen(L"log"));
//
//	CreateDirectoryW(m_wszDirName, NULL);
//
//	m_cType = nType;
//	CalcCounter();
//}
//
//void CLog::Init(const WCHAR *pwszFileName, BYTE nType)
//{
//	_wcscpy(m_wszFileName, _countof(m_wszFileName), pwszFileName, (int)wcslen(pwszFileName));
//	_wcscpy(m_wszDirName, _countof(m_wszDirName), L"log", (int)wcslen(L"log"));
//	CreateDirectoryW(m_wszDirName, NULL);
//
//	m_cType = nType;
//	CalcCounter();
//}
//
//void CLog::Init(const WCHAR *pwszDirName, const WCHAR *pwszFileName, BYTE nType)
//{
//	_wcscpy(m_wszFileName, _countof(m_wszFileName), pwszFileName, (int)wcslen(pwszFileName));
//	_wcscpy(m_wszDirName, _countof(m_wszDirName), pwszDirName, (int)wcslen(pwszDirName));
//
//	CreateDirectoryW(m_wszDirName, NULL);
//
//	m_cType = nType;
//	CalcCounter();
//}
//
//void CLog::LogTypeLog(LogType::eLogType eType, WCHAR* buf)
//{
//	switch (m_cType)
//	{
//	case LOGTYPE_CRT:
//		{
//#if !defined (_SERVICEMANAGER_EX)
//			Display(buf);
//#else
//			Display(eType, buf);
//#endif // #if !defined (_SERVICEMANAGER_EX)
//		}
//		break;
//
//	case LOGTYPE_CRT_FILE_HOUR:
//	case LOGTYPE_CRT_FILE_DAY:
//		{
//#if !defined (_SERVICEMANAGER_EX)
//			Display(buf);
//#else
//			Display(eType, buf);
//#endif // #if !defined (_SERVICEMANAGER_EX)
//			Filelog(buf);
//		}
//		break;
//
//	case LOGTYPE_FILEDB_DAY:
//		Filelog(buf);
//		break;
//
//	default	:
//		{
//#if defined( PRE_FILELOG )
//			Filelog(buf);
//#endif // #if defined( PRE_FILELOG )
//
//#if defined(_FINAL_BUILD)
//			switch( eType )
//			{
//			case LogType::_FILELOG:
//			case LogType::_FILEDBLOG:
//			case LogType::_DBSYSTEM_ERROR:
//				Filelog(buf);
//				break;
//			}
//#endif	// #if defined(_FINAL_BUILD)
//		}
//	}
//}
//
//void CLog::Display(wchar_t* buf)
//{
//	wprintf(L"%s", buf);	
//}
//
//#if defined (_SERVICEMANAGER_EX)
//void CLog::Display(LogType::eLogType type, const wchar_t* buf)
//{
//	if (m_pDisplayer)
//		m_pDisplayer->Display(type, buf);
//}
//#endif // #if defined (_SERVICEMANAGER_EX)
//
//bool CLog::Filelog(wchar_t* buf)
//{
//	char temp_file_name[256];	
//	char temp_date_buf[256];
//	wchar_t temp_write_data[256];	// 파일에 쓰는 날짜이기 때문에 유니코드
//	struct	tm *tm_ptr;
//	time_t	raw;		
//	
//	time(&raw);
//	tm_ptr = localtime(&raw);	
//	
//	wcsftime(temp_write_data, 256, L" '%H:%M:%S' ", tm_ptr);
//
//#if defined (_DBSERVER) || defined (_CASHSERVER)
//	m_LogLock.Lock();
//#endif
//
//	switch( m_cType )
//	{
//	case LOGTYPE_FILE :		// 실시간
//		{
//			strftime(temp_date_buf, 256, "%Y-%m-%d", tm_ptr);
//
//			sprintf(temp_file_name, "%S\\%S_%s_%d.log", m_wszDirName, m_wszFileName, temp_date_buf, m_nCounter);
//
//			if (m_pFile)
//			{
//				fclose(m_pFile);
//				m_pFile = NULL;
//			}
//
//			if ((m_pFile = fopen(temp_file_name, "a")) == NULL)
//			{
//#if defined (_DBSERVER) || defined (_CASHSERVER)
//				m_LogLock.UnLock();
//#endif
//				return false;
//			}
//		}
//		break;
//
//	case LOGTYPE_CRT_FILE_HOUR:
//	case LOGTYPE_FILE_HOUR :			// 한시간에 파일 한개씩
//#if defined( PRE_TRIGGER_LOG )
//	case LOGTYPE_CRT_FILE_TRIGGER_TEST:
//#endif // #if defined( PRE_TRIGGER_LOG )
//		{
//			strftime(temp_date_buf, 256, "%Y-%m-%d-%H", tm_ptr);
//
//			sprintf(temp_file_name, "%S\\%S_%s.log", m_wszDirName, m_wszFileName, temp_date_buf);
//
//			if( m_nOldDay != tm_ptr->tm_yday || m_nOldHour != tm_ptr->tm_hour || m_pFile == NULL )
//			{				
//				m_nOldHour = tm_ptr->tm_hour;
//				m_nOldDay = tm_ptr->tm_yday;
//				if( m_pFile )
//				{
//					fclose(m_pFile);
//					m_pFile = NULL;
//				}
//
//				if ((m_pFile = fopen(temp_file_name, "a")) == NULL)
//				{
//#if defined (_DBSERVER) || defined (_CASHSERVER)
//					m_LogLock.UnLock();
//#endif
//					return false;
//				}
//			} // if( m_oldtime < tm_ptr->tm_hour ||   == NULL ) 
//		}
//		break;
//
//	case LOGTYPE_CRT_FILE_DAY :
//	case LOGTYPE_FILE_DAY :			// 하루에 파일 한개씩
//	case LOGTYPE_FILEDB_DAY:
//		{
//			strftime(temp_date_buf, 256, "%Y-%m-%d", tm_ptr);
//
//			sprintf(temp_file_name, "%S\\%S_%s.log", m_wszDirName, m_wszFileName, temp_date_buf);
//
//			if( m_nOldDay != tm_ptr->tm_yday || m_pFile == NULL )
//			{				
//				m_nOldDay = tm_ptr->tm_yday;
//				if( m_pFile )
//				{
//					fclose(m_pFile);
//					m_pFile = NULL;
//				}				
//
//				if ((m_pFile = fopen(temp_file_name, "a")) == NULL)
//				{
//#if defined (_DBSERVER) || defined (_CASHSERVER)
//					m_LogLock.UnLock();
//#endif
//					return false;
//				}
//			} 
//		}
//		break;
//	}
//
//	fputws(temp_write_data, m_pFile);
//    fputws(buf, m_pFile);	
//	fflush(m_pFile);
//
//#if defined (_DBSERVER) || defined (_CASHSERVER)
//	m_LogLock.UnLock();
//#endif
//	return true;	
//}
//
//void CLog::Log( LogType::eLogType eType, char* fmt, ... )
//{
//#if defined( SKIP_LOG )
//#if defined( PRE_TRIGGER_LOG )
//	if( m_cType != LOGTYPE_CRT_FILE_TRIGGER_TEST )
//#endif // #if defined( PRE_TRIGGER_LOG )
//		return;
//#endif
//
//	WCHAR buf[1024] = {0,};
//	{
//#if defined( PRE_DELETE_LOGLOCK )
//#else
//		ScopeLock<CSyncLock> lock(m_LogLock);
//#endif // #if defined( PRE_DELETE_LOGLOCK )
//
//#if !defined( _FINAL_BUILD )
//		switch( eType )
//		{
//		case LogType::_ERROR:
//			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
//			break;
//		case LogType::_NORMAL:
//			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_INTENSITY);
//			break;
//		}
//#endif // #if !defined( _FINAL_BUILD )
//
//		va_list args;    
//		va_start(args,fmt);
//
//		char buf2[2048] = {0,};	
//		_vsnprintf(buf2, sizeof(buf2), fmt, args);
//		va_end(args);
//
//		MultiByteToWideChar(CP_ACP, 0, buf2, -1, buf, 1024 );
//
//		LogTypeLog(eType, buf);
//
//		va_end(args);
//
//#if !defined( _FINAL_BUILD )
//		// 원래의 회색 글자로 되돌리기
//		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
//#endif // #if !defined( _FINAL_BUILD )
//	}
//
//	switch (m_cType)
//	{
//	case LOGTYPE_CRT:
//		break;
//	default	:
//		{
//			DBLog( eType, m_nServerID, 0, 0, 0, 0, buf );
//			break;
//		}
//	}
//}
//
//void CLog::Log( LogType::eLogType eType, wchar_t* fmt, ... )
//{
//
//#if defined( SKIP_LOG )
//#if defined( PRE_TRIGGER_LOG )
//	if( m_cType != LOGTYPE_CRT_FILE_TRIGGER_TEST )
//#endif // #if defined( PRE_TRIGGER_LOG )
//		return;
//#endif
//
//	wchar_t buf[8192] = {0,} ;	
//	{
//#if defined( PRE_DELETE_LOGLOCK )
//#else
//		ScopeLock<CSyncLock> lock(m_LogLock);
//#endif // #if defined( PRE_DELETE_LOGLOCK )
//		
//#if !defined( _FINAL_BUILD )
//		switch( eType )
//		{
//		case LogType::_ERROR:
//			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
//			break;
//		case LogType::_NORMAL:
//			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_INTENSITY);
//			break;
//		}
//		
//#endif // #if !defined( _FINAL_BUILD )
//
//		va_list args;    
//		va_start(args,fmt);
//
//		_vsnwprintf(buf, 8192, fmt, args);
//
//		LogTypeLog(eType, buf);
//
//		wprintf(L"%ws\n",buf);
//		va_end(args);
//
//		// 원래의 회색 글자로 되돌리기
//		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
//		
//
//	}
//
//	switch (m_cType)
//	{
//	case LOGTYPE_CRT:
//		break;
//
//	default	:
//		{
//			DBLog( eType, m_nServerID, 0, 0, 0, 0, buf );
//			break;
//		}
//	}
//}
//
//void CLog::Log( LogType::eLogType eType, int iWorldID, UINT uiAccountDBID, INT64 biCharDBID, UINT uiSessionID, char* fmt, ... )
//{
//#if defined( SKIP_LOG )
//#if defined( PRE_TRIGGER_LOG )
//	if( m_cType != LOGTYPE_CRT_FILE_TRIGGER_TEST )
//#endif // #if defined( PRE_TRIGGER_LOG )
//		return;
//#endif
//
//	WCHAR buf[1024] = {0,};
//	{
//#if defined( PRE_DELETE_LOGLOCK )
//#else
//		ScopeLock<CSyncLock> lock(m_LogLock);
//#endif // #if defined( PRE_DELETE_LOGLOCK )
//
//#if !defined( _FINAL_BUILD )
//		switch( eType )
//		{
//		case LogType::_ERROR:
//			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
//			break;
//		case LogType::_NORMAL:
//			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_INTENSITY);
//			break;
//		}
//#endif // #if !defined( _FINAL_BUILD )
//
//		va_list args;    
//		va_start(args,fmt);
//
//		char buf2[2048] = {0,};	
//		_vsnprintf(buf2, sizeof(buf2), fmt, args);
//		va_end(args);
//
//		MultiByteToWideChar(CP_ACP, 0, buf2, -1, buf, 1024 );
//
//		LogTypeLog(eType, buf);
//
//		va_end(args);
//
//#if !defined( _FINAL_BUILD )
//		// 원래의 회색 글자로 되돌리기
//		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
//#endif // #if !defined( _FINAL_BUILD )
//	}
//
//	switch (m_cType)
//	{
//		case LOGTYPE_CRT:
//			break;
//
//		default	:
//		{
//			DBLog( eType, m_nServerID, iWorldID, uiAccountDBID, biCharDBID, uiSessionID, buf );
//			break;
//		}
//	}
//}
//
//void CLog::Log( LogType::eLogType eType, int iWorldID, UINT uiAccountDBID, INT64 biCharDBID, UINT uiSessionID, wchar_t* fmt, ... )
//{
//#if defined( SKIP_LOG )
//#if defined( PRE_TRIGGER_LOG )
//	if( m_cType != LOGTYPE_CRT_FILE_TRIGGER_TEST )
//#endif // #if defined( PRE_TRIGGER_LOG )
//		return;
//#endif
//
//	wchar_t buf[8192] = {0,} ;	
//	{
//#if defined( PRE_DELETE_LOGLOCK )
//#else
//		ScopeLock<CSyncLock> lock(m_LogLock);
//#endif // #if defined( PRE_DELETE_LOGLOCK )
//
//#if !defined( _FINAL_BUILD )
//		switch( eType )
//		{
//		case LogType::_ERROR:
//			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
//			break;
//		case LogType::_NORMAL:
//			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_INTENSITY);
//			break;
//		}
//#endif // #if !defined( _FINAL_BUILD )
//
//		va_list args;    
//		va_start(args,fmt);
//
//		_vsnwprintf(buf, 8192, fmt, args);
//
//		LogTypeLog(eType, buf);
//
//		va_end(args);
//
//#if !defined( _FINAL_BUILD )
//		// 원래의 회색 글자로 되돌리기
//		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
//#endif // #if !defined( _FINAL_BUILD )
//	}
//
//	switch (m_cType)
//	{
//		case LOGTYPE_CRT:
//			break;
//
//		default	:
//		{
//			DBLog( eType, m_nServerID, iWorldID, uiAccountDBID, biCharDBID, uiSessionID, buf );
//			break;
//		}
//	}
//}
//
//#if defined( _GAMESERVER ) || defined( _VILLAGESERVER )
//
//void CLog::Log( LogType::eLogType eType, CDNUserSession* pSession, wchar_t* fmt, ... )
//{
//#if defined( SKIP_LOG )
//#if defined( PRE_TRIGGER_LOG )
//	if( m_cType != LOGTYPE_CRT_FILE_TRIGGER_TEST )
//#endif // #if defined( PRE_TRIGGER_LOG )
//		return;
//#endif
//
//	wchar_t buf[8192] = {0,} ;	
//	{
//#if defined( PRE_DELETE_LOGLOCK )
//#else
//		ScopeLock<CSyncLock> lock(m_LogLock);
//#endif // #if defined( PRE_DELETE_LOGLOCK )
//
//#if !defined( _FINAL_BUILD )
//		switch( eType )
//		{
//		case LogType::_ERROR:
//			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
//			break;
//		case LogType::_NORMAL:
//			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_INTENSITY);
//			break;
//		}
//#endif // #if !defined( _FINAL_BUILD )
//
//		va_list args;    
//		va_start(args,fmt);
//
//		_vsnwprintf(buf, 8192, fmt, args);
//
//		LogTypeLog(eType, buf);
//
//		va_end(args);
//
//#if !defined( _FINAL_BUILD )
//		// 원래의 회색 글자로 되돌리기
//		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
//#endif // #if !defined( _FINAL_BUILD )
//	}
//
//	switch (m_cType)
//	{
//		case LOGTYPE_CRT:
//			break;
//
//		default	:
//		{
//			if( pSession )
//				DBLog( eType, m_nServerID, pSession->GetWorldSetID(), pSession->GetAccountDBID(), pSession->GetCharacterDBID(), pSession->GetSessionID(), buf );
//			else
//				DBLog( eType, m_nServerID, 0, 0, 0, 0, buf );
//
//			break;
//		}
//	}
//}
//
//#elif defined( _MASTERSERVER )
//
//void CLog::Log( LogType::eLogType eType, CDNUser* pUser, wchar_t* fmt, ... )
//{
//#if defined( SKIP_LOG )
//#if defined( PRE_TRIGGER_LOG )
//	if( m_cType != LOGTYPE_CRT_FILE_TRIGGER_TEST )
//#endif // #if defined( PRE_TRIGGER_LOG )
//		return;
//#endif
//
//	wchar_t buf[8192] = {0,} ;	
//	{
//#if defined( PRE_DELETE_LOGLOCK )
//#else
//		ScopeLock<CSyncLock> lock(m_LogLock);
//#endif // #if defined( PRE_DELETE_LOGLOCK )
//
//#if !defined( _FINAL_BUILD )
//		switch( eType )
//		{
//		case LogType::_ERROR:
//			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
//			break;
//		case LogType::_NORMAL:
//			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_INTENSITY);
//			break;
//		}
//#endif // #if !defined( _FINAL_BUILD )
//
//		va_list args;    
//		va_start(args,fmt);
//
//		_vsnwprintf(buf, 8192, fmt, args);
//
//		LogTypeLog(eType, buf);
//
//		va_end(args);
//
//#if !defined( _FINAL_BUILD )
//		// 원래의 회색 글자로 되돌리기
//		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
//#endif // #if !defined( _FINAL_BUILD )
//	}
//	switch (m_cType)
//	{
//		case LOGTYPE_CRT:
//			break;
//
//		default	:
//		{
//			if( pUser )
//				DBLog( eType, m_nServerID, g_Config.nWorldSetID, pUser->GetAccountDBID(), pUser->GetCharacterDBID(), pUser->GetSessionID(), buf );
//			else
//				DBLog( eType, m_nServerID, 0, 0, 0, 0, buf );
//
//			break;
//		}
//	}
//}
//
//#elif defined( _LOGINSERVER )
//
//void CLog::Log( LogType::eLogType eType, CDNUserConnection* pUser, wchar_t* fmt, ... )
//{
//#if defined( SKIP_LOG )
//#if defined( PRE_TRIGGER_LOG )
//	if( m_cType != LOGTYPE_CRT_FILE_TRIGGER_TEST )
//#endif // #if defined( PRE_TRIGGER_LOG )
//		return;
//#endif
//
//	wchar_t buf[8192] = {0,} ;	
//	{
//#if defined( PRE_DELETE_LOGLOCK )
//#else
//		ScopeLock<CSyncLock> lock(m_LogLock);
//#endif // #if defined( PRE_DELETE_LOGLOCK )
//
//		switch( eType )
//		{
//		case LogType::_ERROR:
//			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
//			break;
//		case LogType::_NORMAL:
//			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_INTENSITY);
//			break;
//		}
//#if !defined( _FINAL_BUILD )
//#endif // #if !defined( _FINAL_BUILD )
//
//		va_list args;    
//		va_start(args,fmt);
//
//		_vsnwprintf(buf, 8192, fmt, args);
//
//		LogTypeLog(eType, buf);
//
//		va_end(args);
//
//
//		// 원래의 회색 글자로 되돌리기
//		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
//#if !defined( _FINAL_BUILD )
//#endif // #if !defined( _FINAL_BUILD )
//	}
//
//	switch (m_cType)
//	{
//		case LOGTYPE_CRT:
//			break;
//
//		default	:
//		{
//			if( pUser )
//				DBLog( eType, m_nServerID, 0, pUser->GetAccountDBID(), 0, pUser->GetSessionID(), buf );
//			else
//				DBLog( eType, m_nServerID, 0, 0, 0, 0, buf );
//
//			break;
//		}
//	}
//}
//
//#elif defined( _CASHSERVER )
//
//void CLog::Log( LogType::eLogType eType, CDNUserRepository* pUser, wchar_t* fmt, ... )
//{
//#if defined( SKIP_LOG )
//#if defined( PRE_TRIGGER_LOG )
//	if( m_cType != LOGTYPE_CRT_FILE_TRIGGER_TEST )
//#endif // #if defined( PRE_TRIGGER_LOG )
//		return;
//#endif
//
//	wchar_t buf[8192] = {0,} ;	
//	{
//#if defined( PRE_DELETE_LOGLOCK )
//#else
//		ScopeLock<CSyncLock> lock(m_LogLock);
//#endif // #if defined( PRE_DELETE_LOGLOCK )
//
//
//		switch( eType )
//		{
//		case LogType::_ERROR:
//			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
//			break;
//		case LogType::_NORMAL:
//			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_INTENSITY);
//			break;
//		}
//
//
//		wprintf(L"[LOG]: %ws",fmt);
//
//#if !defined( _FINAL_BUILD )
//#endif // #if !defined( _FINAL_BUILD )
//
//		va_list args;    
//		va_start(args,fmt);
//
//		_vsnwprintf(buf, 8192, fmt, args);
//
//		LogTypeLog(eType, buf);
//
//		va_end(args);
//
//#if !defined( _FINAL_BUILD )
//		// 원래의 회색 글자로 되돌리기
//		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
//#endif // #if !defined( _FINAL_BUILD )
//	}
//	switch (m_cType)
//	{
//		case LOGTYPE_CRT:
//			break;
//
//		default	:
//		{
//			if( pUser )
//				DBLog( eType, m_nServerID, pUser->GetWorldID(), pUser->GetAccountDBID(), pUser->GetCharacterDBID(), 0, buf );
//			else
//				DBLog( eType, m_nServerID, 0, 0, 0, 0, buf );
//
//			break;
//		}
//	}
//}
//#endif // #if defined( _GAMESERVER )
//
//bool CLog::DBLog( LogType::eLogType eType, int nServerID, int iWorldID, UINT uiAccountDBID, INT64 biCharDBID, UINT uiSessionID, wchar_t* buf )
//{
//#if defined( _LOGSERVER ) || !defined( _SERVER ) || defined (_SERVICEMANAGER)
//#else
//	if( eType == LogType::_FILELOG )
//		return true;
//
//	SMLog (eType, iWorldID, uiAccountDBID, biCharDBID, uiSessionID, buf);
//
//	TLogFile TxPacket;
//	memset( &TxPacket, 0, sizeof(TxPacket) );
//
//	TxPacket.unLogType		= static_cast<USHORT>(eType);
//	TxPacket.unWorldSetID		= static_cast<USHORT>(iWorldID);
//	TxPacket.uiAccountDBID	= uiAccountDBID;
//	TxPacket.biCharDBID		= biCharDBID;
//	TxPacket.uiSessionID	= uiSessionID;
//	TxPacket.unServerType	= static_cast<USHORT>(_SERVER_TYPEID);
//	TxPacket.unLen			= static_cast<USHORT>(wcslen(buf));
//	TxPacket.nServerID		= nServerID;
//
//	if( TxPacket.unLen >= _countof(TxPacket.wszBuf) )
//		return false;
//	_wcscpy( TxPacket.wszBuf, _countof(TxPacket.wszBuf), buf, (int)wcslen(buf) );
//
//	if( g_pLogConnection )
//	{
//#if defined( PRE_FIX_SOCKETCONTEXT_DANGLINGPTR )
//		if( g_pLogConnection->GetSocketContextPtr() == NULL )
//			return true;
//
//		CScopeInterlocked Scope( &g_pLogConnection->GetSocketContextPtr()->m_lActiveCount );
//		if( Scope.bIsDelete() )
//			return true;
//#endif // #if defined( PRE_FIX_SOCKETCONTEXT_DANGLINGPTR )
//
//		int iLen = sizeof(TxPacket)-sizeof(TxPacket.wszBuf)+(sizeof(WCHAR)*(TxPacket.unLen+1));
//		g_pLogConnection->AddSendData( LOG_FILE, 0, reinterpret_cast<char*>(&TxPacket), iLen );
//	}
//#endif // #if defiend( _LOGSERVER )
//
//	return true;	
//}
//
//bool CLog::SMLog(LogType::eLogType eType, int iWorldID, UINT uiAccountDBID, INT64 biCharDBID, UINT uiSessionID, wchar_t* buf)
//{
//#if defined (_DBSERVER)
//	if (eType != LogType::_ERROR && eType != LogType::_DBSYSTEM_ERROR)
//		return false;
//
//	TQueryErrorReport TxPacket;
//	memset( &TxPacket, 0, sizeof(TxPacket) );
//
//	TxPacket.unLogType		= static_cast<USHORT>(eType);
//	TxPacket.unWorldSetID	= static_cast<USHORT>(iWorldID);
//	TxPacket.uiAccountDBID	= uiAccountDBID;
//	TxPacket.biCharDBID		= biCharDBID;
//	TxPacket.uiSessionID	= uiSessionID;
//	TxPacket.unServerType	= static_cast<USHORT>(_SERVER_TYPEID);
//	TxPacket.unLen			= static_cast<USHORT>(wcslen(buf));
//	if( TxPacket.unLen >= _countof(TxPacket.wszBuf) )
//	{
//		TxPacket.unLen = _countof(TxPacket.wszBuf)-1;
//		//return false;
//	}
//	_wcscpy( TxPacket.wszBuf, _countof(TxPacket.wszBuf)-1, buf, _countof(TxPacket.wszBuf)-1 );
//
//	if( g_pServiceConnection )
//	{
//		if( g_pServiceConnection->GetSocketContextPtr() == NULL )
//			return true;
//
//		CScopeInterlocked Scope( &g_pServiceConnection->GetSocketContextPtr()->m_lActiveCount );
//		if( Scope.bIsDelete() )
//			return true;
//
//		int iLen = sizeof(TxPacket)-sizeof(TxPacket.wszBuf)+(sizeof(WCHAR)*(TxPacket.unLen+1));
//		g_pServiceConnection->AddSendData( SERVICE_SERVER_DBERROR_REPORT, 0, reinterpret_cast<char*>(&TxPacket), iLen );
//	}
//#endif // #if defined (_DBSERVER)
//
//	return true;
//}
//
//#if defined( _GAMESERVER )
//#if defined( PRE_QUESTSCRIPT_LOG )
//
//CScopeScriptLog::CScopeScriptLog( int iMapIndex, const char* pszFuncName )
//:m_iMapIndex(iMapIndex),m_strFuncName(pszFuncName)
//{
//	QueryPerformanceFrequency(&m_liFrequency);
//	QueryPerformanceCounter(&m_liStartTime);	
//}
//
//CScopeScriptLog::~CScopeScriptLog()
//{
//	QueryPerformanceCounter(&m_liCurTime);
//	double dElapsed = (1000 * ( m_liCurTime.QuadPart - m_liStartTime.QuadPart ) / static_cast<double>( m_liFrequency.QuadPart ));
//
//	if( dElapsed > 0.f )
//	{
//		char szLog[256];
//		sprintf( szLog, "[%.3f ms] MapIndex=%d FuncName:%s\r\n", dElapsed, m_iMapIndex, m_strFuncName.c_str() );
//		g_QuestLog.Log( LogType::_FILELOG, szLog );
//	}
//}
//
//#endif // #if defined( PRE_QUESTSCRIPT_LOG )
//#endif // #if defined( _GAMESERVER )
//
//#if !defined( _FINAL_BUILD )
//
//#include <iostream>
//
//CPerformanceLog::CPerformanceLog(const char* pszName, int iOverTick/*=0*/ )
//:m_strName(pszName),m_iOverTick(iOverTick)
//{
//	QueryPerformanceFrequency(&m_liFrequency);
//	QueryPerformanceCounter(&m_liStartTime);	
//}
//
//CPerformanceLog::~CPerformanceLog()
//{
//	QueryPerformanceCounter(&m_liCurTime);
//	double dElapsed = (1000 * ( m_liCurTime.QuadPart - m_liStartTime.QuadPart ) / static_cast<double>( m_liFrequency.QuadPart ));
//
//	if( dElapsed >= m_iOverTick )
//	{
//		char szLog[256];
//		sprintf( szLog, "[%.3f ms] Name:%s\r\n", dElapsed, m_strName.c_str() );
//		std::cout << szLog;
//	}
//}
//
//#endif // #if !defined( _FINAL_BUILD )
//
//#if defined( PRE_ADD_QUERYTIMELOG )
//
//CQueryTimeLog::CQueryTimeLog( const char* pszQuery )
//:m_strQuery(pszQuery),m_dwStartTime(timeGetTime())
//{
//	if (g_SPLog.GetActive())
//		g_SPLog.Log( LogType::_FILELOG, L"QueryTimeLog:%S ThreadID:%d \n", m_strQuery.c_str(), ::GetCurrentThreadId());
//}
//
//CQueryTimeLog::~CQueryTimeLog()
//{
//	DWORD dwElapsed = timeGetTime()-m_dwStartTime;
//	if( dwElapsed > QUERYOVERTIME )
//	{
//		g_Log.Log( LogType::_FILELOG, L"[Query Over Time:%S] ThreadID:%d time=%d ms\n", m_strQuery.c_str(), ::GetCurrentThreadId(), dwElapsed );
//		if (g_pServiceConnection)
//			g_pServiceConnection->SendQueryDelayedReport(g_pServiceConnection->GetManagedID(), m_strQuery.c_str(), ::GetCurrentThreadId(), dwElapsed);
//	}
//}
//
//#endif // #if defined( PRE_ADD_QUERYTIMELOG )
//
//#if defined( PRE_ADD_QUERYTIMELOG )
//CTimeParamSet::CTimeParamSet(CQueryTimeLog* pQuery, __time64_t& pTime64, TIMESTAMP_STRUCT& dbtime, int iWorldID, UINT uiAccountDBID, INT64 biCharDBID)
//{
//	CTimeSet TimeChecker(dbtime.year, dbtime.month, dbtime.day, dbtime.hour, dbtime.minute, dbtime.second);
//
//	// NULL일 경우
//	if (!dbtime.year && !dbtime.month && !dbtime.day && !dbtime.hour && !dbtime.minute && !dbtime.second)
//	{
//		pTime64 = TimeChecker.GetTimeT64_LC();
//		m_bValid = true;
//		return;
//	}
//
//	if(!TimeChecker.CheckIntegrity())
//	{
//		m_bValid = false;
//
//		// SM 보고
//		WCHAR wszLogBuf[256];
//		wsprintf(wszLogBuf, L"[%S] Invalid Time %d:%d:%d:%d:%d:%d\n", pQuery ? pQuery->GetQueryString() : "UnKnwon", dbtime.year, dbtime.month, dbtime.day, dbtime.hour, dbtime.minute, dbtime.second);
//		g_Log.SMLog(LogType::_ERROR, iWorldID, uiAccountDBID, biCharDBID, 0, wszLogBuf);
//		return;
//	}
//
//	pTime64 = TimeChecker.GetTimeT64_LC();
//	m_bValid = true;
//}
//#endif
#include "StdAfx.h"
#include "Log.h"
#include "TimeSet.h"

#if defined( _GAMESERVER )
#include "DnUserSession.h"
#include "DNLogConnection.h"
#if defined( PRE_TRIGGER_UNITTEST_LOG )
CLog g_TriggerUnitTestLog;
#endif // #if defined( PRE_TRIGGER_UNITTEST_LOG )
#if defined( PRE_QUESTSCRIPT_LOG )
CLog g_QuestLog;
#endif // #if defined( PRE_QUESTSCRIPT_LOG )
#if defined( PRE_TRIGGER_LOG )
CLog g_TriggerLog;
#endif // #if defined( PRE_TRIGGER_LOG )
#elif defined( _MASTERSERVER )
extern TMasterConfig g_Config;
#include "DNLogConnection.h"
#include "DNUser.h"
#elif defined( _VILLAGESERVER )
#include "DNLogConnection.h"
#include "DnUserSession.h"
#elif defined( _LOGINSERVER )
#include "DNUserConnection.h"
#include "DNLogConnection.h"
#include "DNServiceConnection.h"
#elif defined( _DBSERVER )
#include "DNLogConnection.h"
#include "DNServiceConnection.h"
#elif defined( _CASHSERVER )
#include "DNLogConnection.h"
#include "DNServiceConnection.h"
#include "DNUserRepository.h"
#endif // #if defined( _GAMESERVER )

CLog g_SPLog;
CLog g_Log;

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CLog::CLog(void) : m_pFile(NULL), m_nOldDay(0), m_nOldHour(0), m_cType(LOGTYPE_CRT)
{
	memset(&m_wszFileName, 0, sizeof(m_wszFileName));
	memset(&m_wszDirName, 0, sizeof(m_wszDirName));
	m_nCounter = 0;
	m_bActive = false;
#if defined (_SERVICEMANAGER_EX)
	m_pDisplayer = NULL;
#endif // #if defined (_SERVICEMANAGER_EX)
	m_nServerID = 0;
}

CLog::~CLog(void)
{
	if (m_pFile)
		fclose(m_pFile);
}

void CLog::CalcCounter()
{
	if (m_cType == LOGTYPE_FILE) {
		std::vector<std::string> szVecFileList;
		FindFileListInDirectory(".\\Log", "*.log", szVecFileList);
		int nCount = 0;
		const char *pszTemp, *pszTemp2;
		for (DWORD i = 0; i<szVecFileList.size(); i++) {
			pszTemp = _GetSubStrByCount(2, (char*)szVecFileList[i].c_str(), '_');
			if (!pszTemp) continue;
			pszTemp2 = _GetSubStrByCount(0, pszTemp, '.');
			if (!pszTemp2) continue;
			if (atoi(pszTemp2) > nCount) nCount = atoi(pszTemp2);
		}
		m_nCounter = nCount + 1;
	}
}

void CLog::Init(BYTE nType)
{
	_wcscpy(m_wszFileName, _countof(m_wszFileName), L"log", (int)wcslen(L"log"));
	_wcscpy(m_wszDirName, _countof(m_wszDirName), L"log", (int)wcslen(L"log"));

	CreateDirectoryW(m_wszDirName, NULL);

	m_cType = nType;
	CalcCounter();
}

void CLog::Init(const WCHAR *pwszFileName, BYTE nType)
{
	_wcscpy(m_wszFileName, _countof(m_wszFileName), pwszFileName, (int)wcslen(pwszFileName));
	_wcscpy(m_wszDirName, _countof(m_wszDirName), L"log", (int)wcslen(L"log"));
	CreateDirectoryW(m_wszDirName, NULL);

	m_cType = nType;
	CalcCounter();
}

void CLog::Init(const WCHAR *pwszDirName, const WCHAR *pwszFileName, BYTE nType)
{
	_wcscpy(m_wszFileName, _countof(m_wszFileName), pwszFileName, (int)wcslen(pwszFileName));
	_wcscpy(m_wszDirName, _countof(m_wszDirName), pwszDirName, (int)wcslen(pwszDirName));

	CreateDirectoryW(m_wszDirName, NULL);

	m_cType = nType;
	CalcCounter();
}

void CLog::LogTypeLog(LogType::eLogType eType, WCHAR* buf)
{
	switch (m_cType)
	{
	case LOGTYPE_CRT:
	{
#if !defined (_SERVICEMANAGER_EX)
		Display(buf);
#else
		Display(eType, buf);
#endif // #if !defined (_SERVICEMANAGER_EX)
	}
	break;

	case LOGTYPE_CRT_FILE_HOUR:
	case LOGTYPE_CRT_FILE_DAY:
	{
#if !defined (_SERVICEMANAGER_EX)
		Display(buf);
#else
		Display(eType, buf);
#endif // #if !defined (_SERVICEMANAGER_EX)
		Filelog(buf);
	}
	break;

	case LOGTYPE_FILEDB_DAY:
		Filelog(buf);
		break;

	default:
	{
#if defined( PRE_FILELOG )
		Filelog(buf);
#endif // #if defined( PRE_FILELOG )

#if defined(_FINAL_BUILD)
		switch (eType)
		{
		case LogType::_FILELOG:
		case LogType::_FILEDBLOG:
		case LogType::_DBSYSTEM_ERROR:
			Filelog(buf);
			break;
		}
#endif	// #if defined(_FINAL_BUILD)
	}
	}
}

void CLog::Display(wchar_t* buf)
{
	wprintf(L"%s", buf);
}

#if defined (_SERVICEMANAGER_EX)
void CLog::Display(LogType::eLogType type, const wchar_t* buf)
{
	if (m_pDisplayer)
		m_pDisplayer->Display(type, buf);
}
#endif // #if defined (_SERVICEMANAGER_EX)

bool CLog::Filelog(wchar_t* buf)
{
	char temp_file_name[256];
	char temp_date_buf[256];
	wchar_t temp_write_data[256];	// 파일에 쓰는 날짜이기 때문에 유니코드
	struct	tm *tm_ptr;
	time_t	raw;

	time(&raw);
	tm_ptr = localtime(&raw);

	wcsftime(temp_write_data, 256, L" '%H:%M:%S' ", tm_ptr);

#if defined (_DBSERVER) || defined (_CASHSERVER)
	m_LogLock.Lock();
#endif

	switch (m_cType)
	{
	case LOGTYPE_FILE:		// 실시간
	{
		strftime(temp_date_buf, 256, "%Y-%m-%d", tm_ptr);

		sprintf(temp_file_name, "%S\\%S_%s_%d.log", m_wszDirName, m_wszFileName, temp_date_buf, m_nCounter);

		if (m_pFile)
		{
			fclose(m_pFile);
			m_pFile = NULL;
		}

		if ((m_pFile = fopen(temp_file_name, "a")) == NULL)
		{
#if defined (_DBSERVER) || defined (_CASHSERVER)
			m_LogLock.UnLock();
#endif
			return false;
		}
	}
	break;

	case LOGTYPE_CRT_FILE_HOUR:
	case LOGTYPE_FILE_HOUR:			// 한시간에 파일 한개씩
#if defined( PRE_TRIGGER_LOG )
	case LOGTYPE_CRT_FILE_TRIGGER_TEST:
#endif // #if defined( PRE_TRIGGER_LOG )
	{
		strftime(temp_date_buf, 256, "%Y-%m-%d-%H", tm_ptr);

		sprintf(temp_file_name, "%S\\%S_%s.log", m_wszDirName, m_wszFileName, temp_date_buf);

		if (m_nOldDay != tm_ptr->tm_yday || m_nOldHour != tm_ptr->tm_hour || m_pFile == NULL)
		{
			m_nOldHour = tm_ptr->tm_hour;
			m_nOldDay = tm_ptr->tm_yday;
			if (m_pFile)
			{
				fclose(m_pFile);
				m_pFile = NULL;
			}

			if ((m_pFile = fopen(temp_file_name, "a")) == NULL)
			{
#if defined (_DBSERVER) || defined (_CASHSERVER)
				m_LogLock.UnLock();
#endif
				return false;
			}
		} // if( m_oldtime < tm_ptr->tm_hour ||   == NULL ) 
	}
	break;

	case LOGTYPE_CRT_FILE_DAY:
	case LOGTYPE_FILE_DAY:			// 하루에 파일 한개씩
	case LOGTYPE_FILEDB_DAY:
	{
		strftime(temp_date_buf, 256, "%Y-%m-%d", tm_ptr);

		sprintf(temp_file_name, "%S\\%S_%s.log", m_wszDirName, m_wszFileName, temp_date_buf);

		if (m_nOldDay != tm_ptr->tm_yday || m_pFile == NULL)
		{
			m_nOldDay = tm_ptr->tm_yday;
			if (m_pFile)
			{
				fclose(m_pFile);
				m_pFile = NULL;
			}

			if ((m_pFile = fopen(temp_file_name, "a")) == NULL)
			{
#if defined (_DBSERVER) || defined (_CASHSERVER)
				m_LogLock.UnLock();
#endif
				return false;
			}
		}
	}
	break;
	}

	fputws(temp_write_data, m_pFile);
	fputws(buf, m_pFile);
	fflush(m_pFile);

#if defined (_DBSERVER) || defined (_CASHSERVER)
	m_LogLock.UnLock();
#endif
	return true;
}

void CLog::Log(LogType::eLogType eType, char* fmt, ...)
{
#if defined( SKIP_LOG )
#if defined( PRE_TRIGGER_LOG )
	if (m_cType != LOGTYPE_CRT_FILE_TRIGGER_TEST)
#endif // #if defined( PRE_TRIGGER_LOG )
		return;
#endif

	WCHAR buf[1024] = { 0, };
	{
#if defined( PRE_DELETE_LOGLOCK )
#else
		ScopeLock<CSyncLock> lock(m_LogLock);
#endif // #if defined( PRE_DELETE_LOGLOCK )

#if !defined( _FINAL_BUILD )
		switch (eType)
		{
		case LogType::_ERROR:
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
			break;
		case LogType::_NORMAL:
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_INTENSITY);
			break;
		}
#endif // #if !defined( _FINAL_BUILD )

		va_list args;
		va_start(args, fmt);

		char buf2[2048] = { 0, };
		_vsnprintf(buf2, sizeof(buf2), fmt, args);
		va_end(args);

		MultiByteToWideChar(CP_ACP, 0, buf2, -1, buf, 1024);

		LogTypeLog(eType, buf);

		va_end(args);

#if !defined( _FINAL_BUILD )
		// 원래의 회색 글자로 되돌리기
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
#endif // #if !defined( _FINAL_BUILD )
	}

	switch (m_cType)
	{
	case LOGTYPE_CRT:
		break;
	default:
	{
		DBLog(eType, m_nServerID, 0, 0, 0, 0, buf);
		break;
	}
	}
}

void CLog::Log(LogType::eLogType eType, wchar_t* fmt, ...)
{
#if defined( SKIP_LOG )
#if defined( PRE_TRIGGER_LOG )
	if (m_cType != LOGTYPE_CRT_FILE_TRIGGER_TEST)
#endif // #if defined( PRE_TRIGGER_LOG )
		return;
#endif

	wchar_t buf[8192] = { 0, };
	{
#if defined( PRE_DELETE_LOGLOCK )
#else
		ScopeLock<CSyncLock> lock(m_LogLock);
#endif // #if defined( PRE_DELETE_LOGLOCK )

#if !defined( _FINAL_BUILD )
		switch (eType)
		{
		case LogType::_ERROR:
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
			break;
		case LogType::_NORMAL:
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_INTENSITY);
			break;
		}
#endif // #if !defined( _FINAL_BUILD )

		va_list args;
		va_start(args, fmt);

		_vsnwprintf(buf, 8192, fmt, args);

		LogTypeLog(eType, buf);

		va_end(args);

#if !defined( _FINAL_BUILD )
		// 원래의 회색 글자로 되돌리기
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
#endif // #if !defined( _FINAL_BUILD )
	}

	switch (m_cType)
	{
	case LOGTYPE_CRT:
		break;

	default:
	{
		DBLog(eType, m_nServerID, 0, 0, 0, 0, buf);
		break;
	}
	}
}

void CLog::Log(LogType::eLogType eType, int iWorldID, UINT uiAccountDBID, INT64 biCharDBID, UINT uiSessionID, char* fmt, ...)
{
#if defined( SKIP_LOG )
#if defined( PRE_TRIGGER_LOG )
	if (m_cType != LOGTYPE_CRT_FILE_TRIGGER_TEST)
#endif // #if defined( PRE_TRIGGER_LOG )
		return;
#endif

	WCHAR buf[1024] = { 0, };
	{
#if defined( PRE_DELETE_LOGLOCK )
#else
		ScopeLock<CSyncLock> lock(m_LogLock);
#endif // #if defined( PRE_DELETE_LOGLOCK )

#if !defined( _FINAL_BUILD )
		switch (eType)
		{
		case LogType::_ERROR:
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
			break;
		case LogType::_NORMAL:
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_INTENSITY);
			break;
		}
#endif // #if !defined( _FINAL_BUILD )

		va_list args;
		va_start(args, fmt);

		char buf2[2048] = { 0, };
		_vsnprintf(buf2, sizeof(buf2), fmt, args);
		va_end(args);

		MultiByteToWideChar(CP_ACP, 0, buf2, -1, buf, 1024);

		LogTypeLog(eType, buf);

		va_end(args);

#if !defined( _FINAL_BUILD )
		// 원래의 회색 글자로 되돌리기
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
#endif // #if !defined( _FINAL_BUILD )
	}

	switch (m_cType)
	{
	case LOGTYPE_CRT:
		break;

	default:
	{
		DBLog(eType, m_nServerID, iWorldID, uiAccountDBID, biCharDBID, uiSessionID, buf);
		break;
	}
	}
}

void CLog::Log(LogType::eLogType eType, int iWorldID, UINT uiAccountDBID, INT64 biCharDBID, UINT uiSessionID, wchar_t* fmt, ...)
{
#if defined( SKIP_LOG )
#if defined( PRE_TRIGGER_LOG )
	if (m_cType != LOGTYPE_CRT_FILE_TRIGGER_TEST)
#endif // #if defined( PRE_TRIGGER_LOG )
		return;
#endif

	wchar_t buf[8192] = { 0, };
	{
#if defined( PRE_DELETE_LOGLOCK )
#else
		ScopeLock<CSyncLock> lock(m_LogLock);
#endif // #if defined( PRE_DELETE_LOGLOCK )

#if !defined( _FINAL_BUILD )
		switch (eType)
		{
		case LogType::_ERROR:
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
			break;
		case LogType::_NORMAL:
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_INTENSITY);
			break;
		}
#endif // #if !defined( _FINAL_BUILD )

		va_list args;
		va_start(args, fmt);

		_vsnwprintf(buf, 8192, fmt, args);

		LogTypeLog(eType, buf);

		va_end(args);

#if !defined( _FINAL_BUILD )
		// 원래의 회색 글자로 되돌리기
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
#endif // #if !defined( _FINAL_BUILD )
	}

	switch (m_cType)
	{
	case LOGTYPE_CRT:
		break;

	default:
	{
		DBLog(eType, m_nServerID, iWorldID, uiAccountDBID, biCharDBID, uiSessionID, buf);
		break;
	}
	}
}

#if defined( _GAMESERVER ) || defined( _VILLAGESERVER )

void CLog::Log(LogType::eLogType eType, CDNUserSession* pSession, wchar_t* fmt, ...)
{
#if defined( SKIP_LOG )
#if defined( PRE_TRIGGER_LOG )
	if (m_cType != LOGTYPE_CRT_FILE_TRIGGER_TEST)
#endif // #if defined( PRE_TRIGGER_LOG )
		return;
#endif

	wchar_t buf[8192] = { 0, };
	{
#if defined( PRE_DELETE_LOGLOCK )
#else
		ScopeLock<CSyncLock> lock(m_LogLock);
#endif // #if defined( PRE_DELETE_LOGLOCK )

#if !defined( _FINAL_BUILD )
		switch (eType)
		{
		case LogType::_ERROR:
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
			break;
		case LogType::_NORMAL:
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_INTENSITY);
			break;
		}
#endif // #if !defined( _FINAL_BUILD )

		va_list args;
		va_start(args, fmt);

		_vsnwprintf(buf, 8192, fmt, args);

		LogTypeLog(eType, buf);

		va_end(args);

#if !defined( _FINAL_BUILD )
		// 원래의 회색 글자로 되돌리기
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
#endif // #if !defined( _FINAL_BUILD )
	}

	switch (m_cType)
	{
	case LOGTYPE_CRT:
		break;

	default:
	{
		if (pSession)
			DBLog(eType, m_nServerID, pSession->GetWorldSetID(), pSession->GetAccountDBID(), pSession->GetCharacterDBID(), pSession->GetSessionID(), buf);
		else
			DBLog(eType, m_nServerID, 0, 0, 0, 0, buf);

		break;
	}
	}
}

#elif defined( _MASTERSERVER )

void CLog::Log(LogType::eLogType eType, CDNUser* pUser, wchar_t* fmt, ...)
{
#if defined( SKIP_LOG )
#if defined( PRE_TRIGGER_LOG )
	if (m_cType != LOGTYPE_CRT_FILE_TRIGGER_TEST)
#endif // #if defined( PRE_TRIGGER_LOG )
		return;
#endif

	wchar_t buf[8192] = { 0, };
	{
#if defined( PRE_DELETE_LOGLOCK )
#else
		ScopeLock<CSyncLock> lock(m_LogLock);
#endif // #if defined( PRE_DELETE_LOGLOCK )

#if !defined( _FINAL_BUILD )
		switch (eType)
		{
		case LogType::_ERROR:
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
			break;
		case LogType::_NORMAL:
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_INTENSITY);
			break;
		}
#endif // #if !defined( _FINAL_BUILD )

		va_list args;
		va_start(args, fmt);

		_vsnwprintf(buf, 8192, fmt, args);

		LogTypeLog(eType, buf);

		va_end(args);

#if !defined( _FINAL_BUILD )
		// 원래의 회색 글자로 되돌리기
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
#endif // #if !defined( _FINAL_BUILD )
	}
	switch (m_cType)
	{
	case LOGTYPE_CRT:
		break;

	default:
	{
		if (pUser)
			DBLog(eType, m_nServerID, g_Config.nWorldSetID, pUser->GetAccountDBID(), pUser->GetCharacterDBID(), pUser->GetSessionID(), buf);
		else
			DBLog(eType, m_nServerID, 0, 0, 0, 0, buf);

		break;
	}
	}
}

#elif defined( _LOGINSERVER )

void CLog::Log(LogType::eLogType eType, CDNUserConnection* pUser, wchar_t* fmt, ...)
{
#if defined( SKIP_LOG )
#if defined( PRE_TRIGGER_LOG )
	if (m_cType != LOGTYPE_CRT_FILE_TRIGGER_TEST)
#endif // #if defined( PRE_TRIGGER_LOG )
		return;
#endif

	wchar_t buf[8192] = { 0, };
	{
#if defined( PRE_DELETE_LOGLOCK )
#else
		ScopeLock<CSyncLock> lock(m_LogLock);
#endif // #if defined( PRE_DELETE_LOGLOCK )

#if !defined( _FINAL_BUILD )
		switch (eType)
		{
		case LogType::_ERROR:
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
			break;
		case LogType::_NORMAL:
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_INTENSITY);
			break;
		}
#endif // #if !defined( _FINAL_BUILD )

		va_list args;
		va_start(args, fmt);

		_vsnwprintf(buf, 8192, fmt, args);

		LogTypeLog(eType, buf);

		va_end(args);

#if !defined( _FINAL_BUILD )
		// 원래의 회색 글자로 되돌리기
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
#endif // #if !defined( _FINAL_BUILD )
	}

	switch (m_cType)
	{
	case LOGTYPE_CRT:
		break;

	default:
	{
		if (pUser)
			DBLog(eType, m_nServerID, 0, pUser->GetAccountDBID(), 0, pUser->GetSessionID(), buf);
		else
			DBLog(eType, m_nServerID, 0, 0, 0, 0, buf);

		break;
	}
	}
}

#elif defined( _CASHSERVER )

void CLog::Log(LogType::eLogType eType, CDNUserRepository* pUser, wchar_t* fmt, ...)
{
#if defined( SKIP_LOG )
#if defined( PRE_TRIGGER_LOG )
	if (m_cType != LOGTYPE_CRT_FILE_TRIGGER_TEST)
#endif // #if defined( PRE_TRIGGER_LOG )
		return;
#endif

	wchar_t buf[8192] = { 0, };
	{
#if defined( PRE_DELETE_LOGLOCK )
#else
		ScopeLock<CSyncLock> lock(m_LogLock);
#endif // #if defined( PRE_DELETE_LOGLOCK )

#if !defined( _FINAL_BUILD )
		switch (eType)
		{
		case LogType::_ERROR:
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_INTENSITY);
			break;
		case LogType::_NORMAL:
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_INTENSITY);
			break;
		}
#endif // #if !defined( _FINAL_BUILD )

		va_list args;
		va_start(args, fmt);

		_vsnwprintf(buf, 8192, fmt, args);

		LogTypeLog(eType, buf);

		va_end(args);

#if !defined( _FINAL_BUILD )
		// 원래의 회색 글자로 되돌리기
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
#endif // #if !defined( _FINAL_BUILD )
	}
	switch (m_cType)
	{
	case LOGTYPE_CRT:
		break;

	default:
	{
		if (pUser)
			DBLog(eType, m_nServerID, pUser->GetWorldID(), pUser->GetAccountDBID(), pUser->GetCharacterDBID(), 0, buf);
		else
			DBLog(eType, m_nServerID, 0, 0, 0, 0, buf);

		break;
	}
	}
}
#endif // #if defined( _GAMESERVER )

bool CLog::DBLog(LogType::eLogType eType, int nServerID, int iWorldID, UINT uiAccountDBID, INT64 biCharDBID, UINT uiSessionID, wchar_t* buf)
{
#if defined( _LOGSERVER ) || !defined( _SERVER ) || defined (_SERVICEMANAGER)
#else
	if (eType == LogType::_FILELOG)
		return true;

	SMLog(eType, iWorldID, uiAccountDBID, biCharDBID, uiSessionID, buf);

	TLogFile TxPacket;
	memset(&TxPacket, 0, sizeof(TxPacket));

	TxPacket.unLogType = static_cast<USHORT>(eType);
	TxPacket.unWorldSetID = static_cast<USHORT>(iWorldID);
	TxPacket.uiAccountDBID = uiAccountDBID;
	TxPacket.biCharDBID = biCharDBID;
	TxPacket.uiSessionID = uiSessionID;
	TxPacket.unServerType = static_cast<USHORT>(_SERVER_TYPEID);
	TxPacket.unLen = static_cast<USHORT>(wcslen(buf));
	TxPacket.nServerID = nServerID;

	if (TxPacket.unLen >= _countof(TxPacket.wszBuf))
		return false;
	_wcscpy(TxPacket.wszBuf, _countof(TxPacket.wszBuf), buf, (int)wcslen(buf));

	if (g_pLogConnection)
	{
#if defined( PRE_FIX_SOCKETCONTEXT_DANGLINGPTR )
		if (g_pLogConnection->GetSocketContextPtr() == NULL)
			return true;

		CScopeInterlocked Scope(&g_pLogConnection->GetSocketContextPtr()->m_lActiveCount);
		if (Scope.bIsDelete())
			return true;
#endif // #if defined( PRE_FIX_SOCKETCONTEXT_DANGLINGPTR )

		int iLen = sizeof(TxPacket) - sizeof(TxPacket.wszBuf) + (sizeof(WCHAR)*(TxPacket.unLen + 1));
		g_pLogConnection->AddSendData(LOG_FILE, 0, reinterpret_cast<char*>(&TxPacket), iLen);
	}
#endif // #if defiend( _LOGSERVER )

	return true;
}

bool CLog::SMLog(LogType::eLogType eType, int iWorldID, UINT uiAccountDBID, INT64 biCharDBID, UINT uiSessionID, wchar_t* buf)
{
#if defined (_DBSERVER)
	if (eType != LogType::_ERROR && eType != LogType::_DBSYSTEM_ERROR)
		return false;

	TQueryErrorReport TxPacket;
	memset(&TxPacket, 0, sizeof(TxPacket));

	TxPacket.unLogType = static_cast<USHORT>(eType);
	TxPacket.unWorldSetID = static_cast<USHORT>(iWorldID);
	TxPacket.uiAccountDBID = uiAccountDBID;
	TxPacket.biCharDBID = biCharDBID;
	TxPacket.uiSessionID = uiSessionID;
	TxPacket.unServerType = static_cast<USHORT>(_SERVER_TYPEID);
	TxPacket.unLen = static_cast<USHORT>(wcslen(buf));
	if (TxPacket.unLen >= _countof(TxPacket.wszBuf))
	{
		TxPacket.unLen = _countof(TxPacket.wszBuf) - 1;
		//return false;
	}
	_wcscpy(TxPacket.wszBuf, _countof(TxPacket.wszBuf) - 1, buf, _countof(TxPacket.wszBuf) - 1);

	if (g_pServiceConnection)
	{
		if (g_pServiceConnection->GetSocketContextPtr() == NULL)
			return true;

		CScopeInterlocked Scope(&g_pServiceConnection->GetSocketContextPtr()->m_lActiveCount);
		if (Scope.bIsDelete())
			return true;

		int iLen = sizeof(TxPacket) - sizeof(TxPacket.wszBuf) + (sizeof(WCHAR)*(TxPacket.unLen + 1));
		g_pServiceConnection->AddSendData(SERVICE_SERVER_DBERROR_REPORT, 0, reinterpret_cast<char*>(&TxPacket), iLen);
	}
#endif // #if defined (_DBSERVER)

	return true;
}

#if defined( _GAMESERVER )
#if defined( PRE_QUESTSCRIPT_LOG )

CScopeScriptLog::CScopeScriptLog(int iMapIndex, const char* pszFuncName)
	:m_iMapIndex(iMapIndex), m_strFuncName(pszFuncName)
{
	QueryPerformanceFrequency(&m_liFrequency);
	QueryPerformanceCounter(&m_liStartTime);
}

CScopeScriptLog::~CScopeScriptLog()
{
	QueryPerformanceCounter(&m_liCurTime);
	double dElapsed = (1000 * (m_liCurTime.QuadPart - m_liStartTime.QuadPart) / static_cast<double>(m_liFrequency.QuadPart));

	if (dElapsed > 0.f)
	{
		char szLog[256];
		sprintf(szLog, "[%.3f ms] MapIndex=%d FuncName:%s\r\n", dElapsed, m_iMapIndex, m_strFuncName.c_str());
		g_QuestLog.Log(LogType::_FILELOG, szLog);
	}
}

#endif // #if defined( PRE_QUESTSCRIPT_LOG )
#endif // #if defined( _GAMESERVER )

#if !defined( _FINAL_BUILD )

#include <iostream>

CPerformanceLog::CPerformanceLog(const char* pszName, int iOverTick/*=0*/)
	:m_strName(pszName), m_iOverTick(iOverTick)
{
	QueryPerformanceFrequency(&m_liFrequency);
	QueryPerformanceCounter(&m_liStartTime);
}

CPerformanceLog::~CPerformanceLog()
{
	QueryPerformanceCounter(&m_liCurTime);
	double dElapsed = (1000 * (m_liCurTime.QuadPart - m_liStartTime.QuadPart) / static_cast<double>(m_liFrequency.QuadPart));

	if (dElapsed >= m_iOverTick)
	{
		char szLog[256];
		sprintf(szLog, "[%.3f ms] Name:%s\r\n", dElapsed, m_strName.c_str());
		std::cout << szLog;
	}
}

#endif // #if !defined( _FINAL_BUILD )

#if defined( PRE_ADD_QUERYTIMELOG )

CQueryTimeLog::CQueryTimeLog(const char* pszQuery)
	:m_strQuery(pszQuery), m_dwStartTime(timeGetTime())
{
	if (g_SPLog.GetActive())
		g_SPLog.Log(LogType::_FILELOG, L"QueryTimeLog:%S ThreadID:%d \n", m_strQuery.c_str(), ::GetCurrentThreadId());
}

CQueryTimeLog::~CQueryTimeLog()
{
	DWORD dwElapsed = timeGetTime() - m_dwStartTime;
	if (dwElapsed > QUERYOVERTIME)
	{
		g_Log.Log(LogType::_FILELOG, L"[Query Over Time:%S] ThreadID:%d time=%d ms\n", m_strQuery.c_str(), ::GetCurrentThreadId(), dwElapsed);
		if (g_pServiceConnection)
			g_pServiceConnection->SendQueryDelayedReport(g_pServiceConnection->GetManagedID(), m_strQuery.c_str(), ::GetCurrentThreadId(), dwElapsed);
	}
}

#endif // #if defined( PRE_ADD_QUERYTIMELOG )

#if defined( PRE_ADD_QUERYTIMELOG )
CTimeParamSet::CTimeParamSet(CQueryTimeLog* pQuery, __time64_t& pTime64, TIMESTAMP_STRUCT& dbtime, int iWorldID, UINT uiAccountDBID, INT64 biCharDBID)
{
	CTimeSet TimeChecker(dbtime.year, dbtime.month, dbtime.day, dbtime.hour, dbtime.minute, dbtime.second);

	// NULL일 경우
	if (!dbtime.year && !dbtime.month && !dbtime.day && !dbtime.hour && !dbtime.minute && !dbtime.second)
	{
		pTime64 = TimeChecker.GetTimeT64_LC();
		m_bValid = true;
		return;
	}

	if (!TimeChecker.CheckIntegrity())
	{
		m_bValid = false;

		// SM 보고
		WCHAR wszLogBuf[256];
		wsprintf(wszLogBuf, L"[%S] Invalid Time %d:%d:%d:%d:%d:%d\n", pQuery ? pQuery->GetQueryString() : "UnKnwon", dbtime.year, dbtime.month, dbtime.day, dbtime.hour, dbtime.minute, dbtime.second);
		g_Log.SMLog(LogType::_ERROR, iWorldID, uiAccountDBID, biCharDBID, 0, wszLogBuf);
		return;
	}

	pTime64 = TimeChecker.GetTimeT64_LC();
	m_bValid = true;
}
#endif
