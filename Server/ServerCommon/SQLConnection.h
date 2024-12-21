#pragma once

#if defined(_US)
#include <sys/timeb.h>
#endif	// #if defined(_US)

const USHORT QUERYSIZEMAX = 20480;


// DESC : 특정 프로시저에 사용되는 입력/출력 파라메터 변수에 대해서 SQLBindParameter(...) 의 마지막 인자 'StrLen_or_IndPtr' 를 특정 조건에 맞게 기본값을 지정해 줄 수 있음
// 주의!> 입력 파라메터이고 기본값 (NULL, DEFAULT) 지정이 필요한 경우는 반드시 입력 파라메터 변수 1개당 1개씩 배정이 되어야 함
class CDNSqlLen
{
public:
	CDNSqlLen() : m_nSqlLen(0)
	{

	}
	SQLLEN* Get(SQLLEN pSize)
	{
		DN_ASSERT(0 == m_nSqlLen,	"Check!");
		return(&(m_nSqlLen = pSize));
	}
	SQLLEN* GetNull(bool pCond, SQLLEN pSize)
	{
		DN_ASSERT(0 == m_nSqlLen,	"Check!");
		return(&(m_nSqlLen = (pCond)?(SQL_NULL_DATA):(pSize)));
	}
	SQLLEN* GetDefault(bool pCond, SQLLEN pSize)
	{
		DN_ASSERT(0 == m_nSqlLen,	"Check!");
		return(&(m_nSqlLen = (pCond)?(SQL_DEFAULT_PARAM):(pSize)));
	}
private:
	SQLLEN m_nSqlLen;
};


class CSQLConnection
{
private:
	char m_szIp[IPLENMAX];
	int m_nPort;
	WCHAR m_wszDBName[DBNAMELENMAX];
	WCHAR m_wszID[IDLENMAX];
	LONG m_RefCount;

	bool m_bMembershipDB;
	bool m_bWorldDB;
	bool m_bActozCommonDB;
	bool m_bActozCommonTestDB;

protected:
	SQLHENV m_henv;
	SQLHDBC m_hdbc;
	SQLHSTMT m_hstmt;

	WCHAR m_wszQuery[QUERYSIZEMAX];

	int Reconnect();

	int CommonRowcountQuery(WCHAR *pQuery);	// 걍 공통적으로 사용할때 호출 (return값이 @@rowcount)
	int CommonResultQuery(WCHAR *pQuery);	// 걍 공통적으로 사용할때 호출 (일반적인 return값 받기)
	int CommonReturnValueQuery(WCHAR *pQuery);	// 걍 공통적으로 사용할때 호출 (일반적인 return값 받기) -> 도열님 renewal 된것부턴 이거 사용

public:
	BYTE m_cThreadID;
	int m_nWorldSetID;

public:
	CSQLConnection(void);
	virtual ~CSQLConnection(void);

	SQLHSTMT GetStmt() { return m_hstmt;};	

	int CheckConnect();

	int Connect(char *pIp, int nPort, WCHAR *pwszDBName, WCHAR *pwszID);
	void Disconnect();

	int CheckRetCode(SQLRETURN RetCode, WCHAR *pDesc);
#if defined(PRE_FIX_SQLCONNECTFAIL_LOG)
	int DisplaySQLConnectError();
#endif	// #if defined(PRE_FIX_SQLCONNECTFAIL_LOG)

	void DisplayInfo(SQLRETURN RetCode, WCHAR *pDesc);

	LONG IncRefCount() { return(InterlockedIncrement(&m_RefCount)); }
	LONG DecRefCount() { return(InterlockedDecrement(&m_RefCount)); }

	// 편지 내용이나 제목중에 작은 따옴표가 들어간경우 쿼리문 내에서 에러가 발생하기 때문에 작은 따옴표 하나를 작은따옴표 두개로 바꿔주는 함수
	void ConvertQuery(WCHAR *pSrc, int nSrcMaxSize, WCHAR *pResultStr, int nDesMaxSize);
	void CheckColumnCount(int nBindCount, const char* pszQuery);

	void SetMembershipDB() { m_bMembershipDB = true; }
	void SetWorldDB() { m_bWorldDB = true; }
	void SetActozCommonDB() { m_bActozCommonDB = true; }
	void SetActozCommonTestDB() { m_bActozCommonTestDB = true; }
};

#if defined(PRE_ADD_SQL_RENEW)

class CParamODBC
{
public:
	SQLSMALLINT			m_sType;
	SQLSMALLINT			m_sValueType;
	SQLSMALLINT			m_sParamType;
	SQLPOINTER			m_pValue;
	SQLUINTEGER			m_unColumnSize;
	SQLLEN				m_StrLenIndPtr;
	bool				m_bNull;

public:
	CParamODBC(SQLSMALLINT sType, SQLSMALLINT sValueType, SQLSMALLINT sParamType, SQLUINTEGER unColumnSize, SQLPOINTER pValue, bool bNull)
	{
		m_sType			= sType;
		m_sValueType	= sValueType;
		m_sParamType	= sParamType;
		m_unColumnSize	= unColumnSize;
		m_pValue		= pValue;
		m_bNull			= bNull;
		m_StrLenIndPtr  = 0;
	}
};

typedef std::vector<CParamODBC>				BIND_PARAM_ARRAY;
typedef std::vector<CParamODBC>::iterator	BIND_PARAM_ARRAY_IT;

class CSQLConnection;

class CSQLRecordSet
{
private :
	CSQLConnection* m_pConnection;	
	BIND_PARAM_ARRAY m_vBindParam;
	SQLHSTMT m_hstmt;
	bool	m_bFetch;
	int		m_nBindCount;

public :
	CSQLRecordSet(CSQLConnection* pConnection);
	~CSQLRecordSet();
	//AddParam --> Excute_Procedure --> Bind --> Fetch
	void	AddParamBit(SQLSMALLINT type, SQLPOINTER pValue, bool bInputNull=false);
	void	AddParamByte(SQLSMALLINT type, SQLPOINTER pValue, bool bInputNull=false);
	void	AddParamShort(SQLSMALLINT type, SQLPOINTER pValue, bool bInputNull=false);
	void	AddParamInt(SQLSMALLINT type, SQLPOINTER pValue, bool bInputNull=false);
	void	AddParamInt64(SQLSMALLINT type, SQLPOINTER pValue, bool bInputNull=false);
	void	AddParamTimeStamp(SQLSMALLINT type, SQLPOINTER pValue, bool bInputNull=false);
	void	AddParamString(SQLSMALLINT type, SQLPOINTER pValue, int nSize, bool bInputNull=false);
	void	AddParamWString(SQLSMALLINT type, SQLPOINTER pValue, int nSize, bool bInputNull=false);
	void	AddParamBinary(SQLSMALLINT type, SQLPOINTER pValue, int nSize, bool bInputNull=false);

	bool	Execute(const WCHAR *strQuery);
	bool	Execute_Procedure(const WCHAR *strProcName, bool bReturn);

	void	AddBindColBit(SQLPOINTER pValue);
	void	AddBindColByte(SQLPOINTER pValue);
	void	AddBindColShort(SQLPOINTER pValue);
	void	AddBindColInt(SQLPOINTER pValue);
	void	AddBindColInt64(SQLPOINTER pValue);
	void	AddBindColTimeStamp(SQLPOINTER pValue);
	void	AddBindColString(SQLPOINTER pValue, int nSize);
	void	AddBindColWString(SQLPOINTER pValue, int nSize);

	bool	Fetch();
};
#endif //#if defined(PRE_ADD_SQL_RENEW)


inline __time32_t _dbStamptoTime32_t(TIMESTAMP_STRUCT &dbtime)
{
	struct tm Tm;
	Tm.tm_year = dbtime.year - 1900;	//struct tm years since 1900
	Tm.tm_mon = dbtime.month - 1;		//starting at 0
	Tm.tm_mday = dbtime.day;
	Tm.tm_hour = dbtime.hour;
	Tm.tm_min = dbtime.minute;
	Tm.tm_sec = dbtime.second;
	Tm.tm_wday  = 0;
	Tm.tm_yday  = 0;
#if defined(_US)
	_timeb summertime;
	_ftime(&summertime);
	Tm.tm_isdst = summertime.dstflag;
#else
	Tm.tm_isdst = 0; // Seriously, i don't know.
#endif

	return _mktime32(&Tm);
}

inline __time64_t _dbStamptoTime64_t(TIMESTAMP_STRUCT &dbtime)
{
	struct tm Tm;
	Tm.tm_year = dbtime.year - 1900;	//struct tm years since 1900
	Tm.tm_mon = dbtime.month - 1;		//starting at 0
	Tm.tm_mday = dbtime.day;
	Tm.tm_hour = dbtime.hour;
	Tm.tm_min = dbtime.minute;
	Tm.tm_sec = dbtime.second;
	Tm.tm_wday  = 0;
	Tm.tm_yday  = 0;
#if defined(_US)
	__timeb64 summertime;
	_ftime64(&summertime);
	Tm.tm_isdst = summertime.dstflag;
#else
	Tm.tm_isdst = 0; // Seriously, i don't know.
#endif

	return _mktime64(&Tm);
}
