#pragma once

#define LOGTYPE_CRT				1
#define LOGTYPE_FILE_HOUR		2
#define LOGTYPE_FILE_DAY		3
#define LOGTYPE_FILE			4	// 실시간
#define LOGTYPE_CRT_FILE_DAY	5	// CRT and FILE_DAY
#define LOGTYPE_CRT_FILE_HOUR	6	// CRT and FILE_HOUR
#define LOGTYPE_FILEDB_DAY		7	// DB and FILE_DAY
#if defined( PRE_TRIGGER_LOG )
#define LOGTYPE_CRT_FILE_TRIGGER_TEST	50	// 로그테스트용
#endif // #if defined( PRE_TRIGGER_LOG )

enum Color
{
	_RED = 0,
	_BLUE,
	_GREEN,
	_ERROR = _RED,
	_WARNING = _BLUE,
	_SUCCESS = _GREEN,
};

struct LogType
{
	enum eLogType
	{
		_NORMAL = 0,
		_ERROR,
		_DANGERPOINT,
		_FILELOG,
		_HACK,
		_HACKSHIELD,
		_ITEMDUPLICATE,
		_WEAPONTYPE,
		_SESSIONCRASH,
		_ROOMCRASH,
		_UNREACHABLE,
		_LOADINGDISCONNECT,
		_ROOMFINALIZECRASH,
		_GAMECONNECTLOG,
		_GAMESERVERDELAY,
		_ENTERGAMECHECK,
		_DROPITEMPENALTY,
		_TREASUREPENALTY,
		_CLEARGRADEPENALTY,
		_PRELOADED_DYNAMICLOAD,
		_DELOBJECT_NAMEKEYFINDERR,
		_OCTREELOG,
		_INVALIDACTOR,
		_GPKERROR,
		_ACCEPTORERROR,
		_QUESTHACK,
		_ZOMBIEUSER,
		_EVENTLIST,
		_FARM,
		_GUILDWAR,
		_MOVEPACKET_SEQ,		//이동메세지를 보내 놓고 먼가 할경우
		_SKILL_ACTION,
		_FILEDBLOG,				// File, DB 둘다 남기기??
		_LADDER,
		_PVPROOM,
		_PING,
		_SENDTHREAD,
		_LASTVILLAGEERROR,		// 마지막 빌리지서버 이상한 로그
		_DBCONNECTIONLOG,
		_ZEROPOPULATION,
		_RESTRICTIP,			// IP로 너무 많은 접속이 이루어질 경우 관련 로그
		_SOCIALCONNECT,			// 중국 소셜 컨넥트관련 로그
		_ASIASOFTLOG,			//
		_SHUTDOWN,
		_DBSYSTEM_ERROR,
		_COMEBACK,				// 복귀유저로그 서버장애시 또는 Logout타임값 갱신이 안되어질 경우 복수번 받을 수도 있어서 로그 단디
		_BESTFRIEND,
		_BEGINNETGUILD,
		_REMAINEDACTOR,			// NPC, 몬스터 남아있는 현상
		_PARTYERROR,			// 파티 관련 오류
		_GAMEQUITREWARD,
		_PROFESSORK_SKILL,		// 프로페서K 스킬 안멈추는 현상
		_DOORSLOG,
		_STEAMUSERCOUNT,
	};
};

#if defined( _GAMESERVER ) || defined( _VILLAGESERVER )
class CDNUserSession;
#elif defined( _MASTERSERVER )
class CDNUser;
#elif defined( _LOGINSERVER )
class CDNUserConnection;
#elif defined( _CASHSERVER )
class CDNUserRepository;
#endif // #if defined( _GAMESERVER )

#if defined (_SERVICEMANAGER_EX)
class CDisplayer
{
public:
	virtual ~CDisplayer() {};

public:
	virtual void Display(LogType::eLogType type, const wchar_t* buf) = 0;
};
#endif // #if defined (_SERVICEMANAGER_EX)

class CLog
{
protected:
#if defined( PRE_DELETE_LOGLOCK )
#else
	CSyncLock m_LogLock;
#endif // #if defined( PRE_DELETE_LOGLOCK )

#if defined (_DBSERVER) || defined (_CASHSERVER)
	CSyncLock m_LogLock;
#endif

	WCHAR m_wszFileName[256];
	WCHAR m_wszDirName[256];
	int m_nCounter;

	FILE* m_pFile;

	int	m_nOldDay;
	int	m_nOldHour;
	BYTE m_cType;
	bool m_bActive;
	int m_nServerID;

	void LogTypeLog(LogType::eLogType eType, WCHAR* buf);

	void Display(WCHAR* buf);
	bool Filelog(WCHAR* buf);
	bool DBLog( LogType::eLogType eType, int nServerID, int iWorldID, UINT uiAccountDBID, INT64 biCharDBID, UINT uiSessionID, wchar_t* buf );

	void CalcCounter();

public:
	CLog(void);
	virtual ~CLog();

	void Init(BYTE nType);										// 기본 디렉토리 및 파일 이름은 log
	void Init(const WCHAR *pwszFileName, BYTE nType);
	void Init(const WCHAR *pwszDirName, const WCHAR *pwszFileName, BYTE nType);

	void SetServerID(int nServerID) { m_nServerID = nServerID;};

	void Log( LogType::eLogType eType, char* fmt, ... );
	void Log( LogType::eLogType eType, wchar_t* fmt, ... );

	void Log( LogType::eLogType eType, int iWorldID, UINT uiAccountDBID, INT64 biCharDBID, UINT uiSessionID, char* fmt, ... );
	void Log( LogType::eLogType eType, int iWorldID, UINT uiAccountDBID, INT64 biCharDBID, UINT uiSessionID, wchar_t* fmt, ... );

#if defined( _GAMESERVER ) || defined( _VILLAGESERVER )
	void Log( LogType::eLogType eType, CDNUserSession* pSession, wchar_t* fmt, ... );
#elif defined( _MASTERSERVER )
	void Log( LogType::eLogType eType, CDNUser* pUser, wchar_t* fmt, ... );
#elif defined( _LOGINSERVER )
	void Log( LogType::eLogType eType, CDNUserConnection* pUser, wchar_t* fmt, ... );
#elif defined( _CASHSERVER )
	void Log( LogType::eLogType eType, CDNUserRepository* pUser, wchar_t* fmt, ... );
#endif // #if defined( _GAMESERVER )

#if defined (_SERVICEMANAGER_EX)
	CDisplayer* m_pDisplayer;
	void Display(LogType::eLogType type, const wchar_t* buf);
	void SetDisplayer(CDisplayer* pDisplayer) { m_pDisplayer = pDisplayer; }
#endif // #if defined (_SERVICEMANAGER_EX)
	bool SMLog(LogType::eLogType eType, int iWorldID, UINT uiAccountDBID, INT64 biCharDBID, UINT uiSessionID, wchar_t* buf);
	void SetActive(bool bActive) {m_bActive = bActive;}
	bool GetActive() {return m_bActive;}
};

#if defined( _GAMESERVER )

#if defined( PRE_TRIGGER_UNITTEST_LOG )
extern CLog g_TriggerUnitTestLog;
#endif // #if defined( PRE_TRIGGER_UNITTEST_LOG )

#if defined( PRE_TRIGGER_LOG )
extern CLog g_TriggerLog;
#endif // #if defined( PRE_TRIGGER_LOG )

#if defined( PRE_QUESTSCRIPT_LOG )
extern CLog g_QuestLog;
#endif // #if defined( PRE_QUESTSCRIPT_LOG )

extern CLog g_Log;
#else
extern CLog g_Log;
extern CLog g_SPLog;
#endif

#ifdef _FINAL_BUILD
#define	_DANGER_POINT() {}
#define	_DANGER_POINT_MSG(msg) {}
#else
#define	_DANGER_POINT() \
{	g_Log.Log( LogType::_DANGERPOINT, L"DANGER_POINT [%s:%d]\r\n", \
			  wcschr(_T(__FILE__), '\\') ? wcschr(_T(__FILE__), '\\')+1 : _T(__FILE__), \
			  __LINE__); }

#define	_DANGER_POINT_MSG(msg) \
{	g_Log.Log( LogType::_DANGERPOINT, L"DANGER_POINT [%s:%d] msg[%s]\r\n", \
	wcschr(_T(__FILE__), '\\') ? wcschr(_T(__FILE__), '\\')+1 : _T(__FILE__), \
	__LINE__, msg); }
#endif

#if defined( _GAMESERVER )
#if defined( PRE_QUESTSCRIPT_LOG )

class CScopeScriptLog
{
public:

	CScopeScriptLog( int iMapIndex, const char* pszFuncName );
	~CScopeScriptLog();

private:

	int				m_iMapIndex;
	std::string		m_strFuncName;
	LARGE_INTEGER	m_liFrequency;
	LARGE_INTEGER	m_liStartTime;
	LARGE_INTEGER	m_liCurTime;

};

#endif // #if defined( PRE_QUESTSCRIPT_LOG )
#endif // #if defined( _GAMESERVER )

#if !defined( _FINAL_BUILD )

class CPerformanceLog
{
public:

	CPerformanceLog( const char* pszName, int iOverTick=0 );
	~CPerformanceLog();

private:

	std::string		m_strName;
	int				m_iOverTick;
	LARGE_INTEGER	m_liFrequency;
	LARGE_INTEGER	m_liStartTime;
	LARGE_INTEGER	m_liCurTime;
};
#else
class CPerformanceLog
{
public:
	CPerformanceLog( const char* pszName ) {}
	~CPerformanceLog() {}
};

#endif // #if !defined( _FINAL_BUILD )

class CExceptionCodeLog
{
public:
	CExceptionCodeLog(DWORD pExceptionCode, const wchar_t* pMsg, int nMainCmd=0, int nSubCmd=0) {
		g_Log.Log(LogType::_FILELOG, L"[Exception Occured] TID : %d, Code : 0x%X - %s MainCmd : %d SubCmd : %d\r\n", ::GetCurrentThreadId(), pExceptionCode, ((pMsg)?(pMsg):(L"<NONE>")), nMainCmd, nSubCmd);
	}
};

#if defined( _LOGINSERVER )
#if defined( PRE_LOGINDUMP_LOG )
class CExceptionCodeLogLoginServer
{	
public:
	CExceptionCodeLogLoginServer(DWORD pExceptionCode, const wchar_t* pMsg, int nMainCmd, int nSubCmd, int nReconnectStep, CSReconnectLogin *ReconnectData) {
		if(ReconnectData)
			g_Log.Log(LogType::_FILELOG, L"[Exception Occured] TID : %d, Code : 0x%X - %s MainCmd : %d SubCmd : %d Step:%d SessionID:%d AccountDBID:%d \r\n", ::GetCurrentThreadId(), pExceptionCode, ((pMsg)?(pMsg):(L"<NONE>")), nMainCmd, nSubCmd, nReconnectStep, ReconnectData->nSessionID, ReconnectData->nAccountDBID);
		else
			g_Log.Log(LogType::_FILELOG, L"[Exception Occured] TID : %d, Code : 0x%X - %s MainCmd : %d SubCmd : %d Step:%d\r\n", ::GetCurrentThreadId(), pExceptionCode, ((pMsg)?(pMsg):(L"<NONE>")), nMainCmd, nSubCmd, nReconnectStep);
	}
};
#endif
#endif

#if defined( PRE_ADD_QUERYTIMELOG )

class CQueryTimeLog
{
public:
	CQueryTimeLog( const char* pszQuery );
	~CQueryTimeLog();
	const char* GetQueryString() {return m_strQuery.c_str();}

private:
	std::string m_strQuery;
	DWORD		m_dwStartTime;
};

#else

class CQueryTimeLog
{
public:
	CQueryTimeLog( const char* pszQuery ){}
	~CQueryTimeLog(){}
};

#endif // #if defined( PRE_ADD_QUERYTIMELOG )

#if defined( PRE_ADD_QUERYTIMELOG ) && !defined (_CLIENT)
class CTimeParamSet
{
public:
	CTimeParamSet(CQueryTimeLog* pQuery, __time64_t& pTime64, TIMESTAMP_STRUCT& dbtime, int iWorldID = 0, UINT uiAccountDBID = 0, INT64 biCharDBID = 0);
	bool IsValid() {return m_bValid;}
private:
	bool m_bValid;
};

#endif
