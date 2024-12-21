

///////////////////////////////////////////////////////////////////
//	INCLUDE
///////////////////////////////////////////////////////////////////

#include "STDAFX.H"
#include "TIMESET.H"
#include <TIME.H>
#include "COMMONMACROS.H"
#include "BASESET.H"
#include "DEBUGUTIL.H"
#include "SAFESTRINGMACROS.H"

#if defined(_US)
#include <sys/timeb.h>
#endif	// #if defined(_US)

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

///////////////////////////////////////////////////////////////////
//	IMPLEMENTATION
///////////////////////////////////////////////////////////////////

//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CTimeSet::CTimeSet
//*---------------------------------------------------------------
// DESC : 생성자
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
CTimeSet::CTimeSet()
{
	Reset();
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CTimeSet::CTimeSet
//*---------------------------------------------------------------
// DESC : 생성자
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
CTimeSet::CTimeSet(WORD wYear, WORD wMonth, WORD wDay, WORD wHour, WORD wMinute, WORD wSecond)
{
	Set(wYear, wMonth, wDay, wHour, wMinute,wSecond);
}


CTimeSet::CTimeSet(const __time32_t& pTime, bool pIsLocal)
{
	Set(pTime, pIsLocal);
}


CTimeSet::CTimeSet(const __time64_t& pTime, bool pIsLocal)
{
	Set(pTime, pIsLocal);
}

CTimeSet::CTimeSet(const char* szTime, bool pIsLocal)
{
	char szYear[5]	= {0,};
	char szMonth[3]	= {0,};
	char szDay[3]	= {0,};
	char szHour[3]	= {0,};
	char szMin[3]	= {0,};

	int nYear	= 0;
	int nMonth	= 0;
	int nDay	= 0;
	int nHour	= 0;
	int nMin	= 0;

	sscanf_s (szTime, "%4s%2s%2s%2s%2s", 
				szYear, sizeof(szYear),
				szMonth, sizeof(szMonth),
				szDay, sizeof(szDay),
				szHour, sizeof(szHour),
				szMin, sizeof(szMin));

	nYear	= atoi (szYear);
	nMonth	= atoi (szMonth);
	nDay	= atoi (szDay);
	nHour	= atoi (szHour);
	nMin	= atoi (szMin);

	Set (nYear, nMonth, nDay, nHour, nMin, 0);
}

//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CTimeSet::CTimeSet
//*---------------------------------------------------------------
// DESC : 생성자
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
CTimeSet::CTimeSet(const FILETIME& stTime)
{
	Set(stTime);
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CTimeSet::CTimeSet
//*---------------------------------------------------------------
// DESC : 생성자
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
CTimeSet::CTimeSet(const SYSTEMTIME& stTime)
{
	Set(stTime);
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CTimeSet::CTimeSet
//*---------------------------------------------------------------
// DESC : 생성자
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
CTimeSet::CTimeSet(const DBTIMESTAMP& stTime)
{
	Set(stTime);
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CTimeSet::Reset
//*---------------------------------------------------------------
// DESC : 시간 객체 정보를 리셋 (현재시간으로 세팅)
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
VOID CTimeSet::Reset()
{
	::GetLocalTime(&m_stLocalTime);
	memset(&m_stDbTimeStamp, 0, sizeof(m_stDbTimeStamp));
	memset(&m_DBTimeStampStruct, 0, sizeof(m_DBTimeStampStruct));
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CTimeSet::Set
//*---------------------------------------------------------------
// DESC : 시간 정보를 세팅
// PARM :	1 . wYear - 년
//			2 . wMonth - 월
//			3 . wDay - 일
//			4 . wHour - 시
//			5 . wMinute - 분
//			6 . wSecond - 초
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
VOID CTimeSet::Set(WORD wYear, WORD wMonth, WORD wDay, WORD wHour, WORD wMinute,WORD wSecond)
{
	m_stLocalTime.wYear = wYear;
	m_stLocalTime.wMonth = wMonth;
	m_stLocalTime.wDay = wDay;
	m_stLocalTime.wDayOfWeek = 0;
	m_stLocalTime.wHour = wHour;
	m_stLocalTime.wMinute = wMinute;
	m_stLocalTime.wSecond = wSecond;
	m_stLocalTime.wMilliseconds = 0;
}

VOID CTimeSet::Set(const __time32_t& pTime, bool pIsLocal)
{
	tm stTm;
//	::memset(&stTm, 0, sizeof(stTm));	// 영향 없음 (헤더 주석 참조할 것)

	if (pIsLocal) {
		ConvertTimeT32ToTm_LC(pTime, &stTm);
	}
	else {
		ConvertTimeT32ToTm_GM(pTime, &stTm);
	}

	m_stLocalTime.wYear = stTm.tm_year;
	m_stLocalTime.wMonth = stTm.tm_mon;
	m_stLocalTime.wDay = stTm.tm_mday;
	m_stLocalTime.wDayOfWeek = 0;
	m_stLocalTime.wHour = stTm.tm_hour;
	m_stLocalTime.wMinute = stTm.tm_min;
	m_stLocalTime.wSecond = stTm.tm_sec;
	m_stLocalTime.wMilliseconds = 0;
}

VOID CTimeSet::Set(const __time64_t& pTime, bool pIsLocal)
{
	tm stTm;
//	::memset(&stTm, 0, sizeof(stTm));	// 영향 없음 (헤더 주석 참조할 것)

	if (pIsLocal) {
		ConvertTimeT64ToTm_LC(pTime, &stTm);
	}
	else {
		ConvertTimeT64ToTm_GM(pTime, &stTm);
	}

	m_stLocalTime.wYear = stTm.tm_year;
	m_stLocalTime.wMonth = stTm.tm_mon;
	m_stLocalTime.wDay = stTm.tm_mday;
	m_stLocalTime.wDayOfWeek = 0;
	m_stLocalTime.wHour = stTm.tm_hour;
	m_stLocalTime.wMinute = stTm.tm_min;
	m_stLocalTime.wSecond = stTm.tm_sec;
	m_stLocalTime.wMilliseconds = 0;
}

//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CTimeSet::Set
//*---------------------------------------------------------------
// DESC : 시간 정보를 세팅
// PARM :	1 . stTime - FILETIME 시간정보
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
VOID CTimeSet::Set(const FILETIME& stTime)
{
	FILETIME stFileTime;
	::FileTimeToLocalFileTime(&stTime, &stFileTime);
	::FileTimeToSystemTime(&stFileTime, &m_stLocalTime);
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CTimeSet::Set
//*---------------------------------------------------------------
// DESC : 시간 정보를 세팅
// PARM :	1 . stTime - SYSTEMTIME 시간정보
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
VOID CTimeSet::Set(const SYSTEMTIME& stTime)
{
	m_stLocalTime = stTime;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CTimeSet::Set
//*---------------------------------------------------------------
// DESC : 시간 정보를 세팅
// PARM :	1 . stTime - DBTIMESTAMP 시간정보
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
VOID CTimeSet::Set(const DBTIMESTAMP& stTime)
{
	FILETIME stFileTime;

	ConvertDbTimeStampToFileTime(&stTime, &stFileTime);
	::FileTimeToSystemTime(&stFileTime, &m_stLocalTime);
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CTimeSet::GetTimeTo
//*---------------------------------------------------------------
// DESC : 현재 객체에 지정된 시간에서 인자로 전달된 특정 시간까지의 남은 초 반환
// PARM :	1 . eDayOfWeek - 주
//			2 . wHour - 시
//			3 . wMinute - 분
//			4 . wSecond - 초
// RETV : 인자로 전달된 특정 시간까지의 남은 초
// PRGM : B4nFter
//*---------------------------------------------------------------
DWORD CTimeSet::GetTimeTo(EF_DAYOFWEEK eDayOfWeek, WORD wHour, WORD wMinute, WORD wSecond)
{
	DN_ASSERT((EV_DAYOFWEEK_SUN <= eDayOfWeek) && (eDayOfWeek <= EV_DAYOFWEEK_SAT),	"Invalid!");
	DN_ASSERT(wHour < 24,	"Invalid!");
	DN_ASSERT(wMinute < 60,	"Invalid!");
	DN_ASSERT(wSecond < 60,	"Invalid!");

	DWORD dwCurSec = (m_stLocalTime.wSecond) + (m_stLocalTime.wMinute*60) + (m_stLocalTime.wHour*60*60) + (m_stLocalTime.wDayOfWeek*60*60*24);
	DWORD dwArgSec = (wSecond) + (wMinute*60) + (wHour*60*60) + (eDayOfWeek*60*60*24);

	if (dwCurSec < dwArgSec) {
		return((dwArgSec-dwCurSec)*1000);
	}
	else if (dwArgSec < dwCurSec) {
		return((7*24*60*60 - dwCurSec + dwArgSec)*1000);
	}
	else {
		return (0);
	}
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CTimeSet::GetTimeTo
//*---------------------------------------------------------------
// DESC : 현재 객체에 지정된 시간에서 인자로 전달된 특정 시간까지의 남은 초 반환
// PARM :	1 . wHour - 시
//			2 . wMinute - 분
//			3 . wSecond - 초
// RETV : 인자로 전달된 특정 시간까지의 남은 초
// PRGM : B4nFter
//*---------------------------------------------------------------
DWORD CTimeSet::GetTimeTo(WORD wHour, WORD wMinute, WORD wSecond)
{
	DN_ASSERT(wHour < 24,	"Invalid!");
	DN_ASSERT(wMinute < 60,	"Invalid!");
	DN_ASSERT(wSecond < 60,	"Invalid!");

	DWORD dwCurSec = (m_stLocalTime.wSecond) + (m_stLocalTime.wMinute*60) + (m_stLocalTime.wHour*60*60);
	DWORD dwArgSec = (wSecond) + (wMinute*60) + (wHour*60*60);
	
	if (dwCurSec < dwArgSec) {
		return((dwArgSec-dwCurSec)*1000);
	}
	else if (dwArgSec < dwCurSec) {
		return((24*60*60 - dwCurSec + dwArgSec)*1000);
	}
	else {
		return (0);
	}
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CTimeSet::GetTimeTo
//*---------------------------------------------------------------
// DESC : 현재 객체에 지정된 시간에서 인자로 전달된 특정 시간까지의 남은 초 반환
// PARM :	1 . wMinute - 분
//			2 . wSecond - 초
// RETV : 인자로 전달된 특정 시간까지의 남은 초
// PRGM : B4nFter
//*---------------------------------------------------------------
DWORD CTimeSet::GetTimeTo(WORD wMinute, WORD wSecond)
{
	DN_ASSERT(wMinute < 60,	"Invalid!");
	DN_ASSERT(wSecond < 60,	"Invalid!");

	DWORD dwCurSec = (m_stLocalTime.wSecond) + (m_stLocalTime.wMinute*60);
	DWORD dwArgSec = (wSecond) + (wMinute*60);

	if (dwCurSec < dwArgSec) {
		return((dwArgSec-dwCurSec)*1000);
	}
	else if (dwArgSec < dwCurSec) {
		return((60*60 - dwCurSec + dwArgSec)*1000);
	}
	else {
		return (0);
	}
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CTimeSet::CompareFileTime
//*---------------------------------------------------------------
// DESC : 두 개의 FILETIME 시간 중 어느쪽이 큰지 반환
// PARM :	1 . stFileTime1 - FILETIME 시간정보 1
//			2 . stFileTime2 - FILETIME 시간정보 2
// RETV : -1 : stFileTime1 이 큼 / 0 : 둘 다 같음 / 1 : stFileTime2 가 큼
// PRGM : B4nFter
//*---------------------------------------------------------------
INT CTimeSet::CompareFileTime(const FILETIME& stFileTime1, const FILETIME& stFileTime2)
{
	return(::CompareFileTime(&stFileTime1, &stFileTime2));
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CTimeSet::CheckIntegrity
//*---------------------------------------------------------------
// DESC : 객체의 현재 시간정보의 무결성 체크
// PARM : N/A
// RETV : TRUE - 성공 / FALSE - 실패
// PRGM : B4nFter
//*---------------------------------------------------------------
BOOL CTimeSet::CheckIntegrity()
{
	if (!m_stLocalTime.wMonth || m_stLocalTime.wMonth > 12) { return FALSE; }
	if (m_stLocalTime.wDayOfWeek  > 6) { return FALSE; }
	if (!m_stLocalTime.wDay || m_stLocalTime.wDay > 31) { return FALSE; }
	if (m_stLocalTime.wHour > 24) { return FALSE; }
	if (m_stLocalTime.wMinute > 60) { return FALSE; }
	if (m_stLocalTime.wSecond > 60) { return FALSE; }
	if (m_stLocalTime.wMilliseconds > 999) { return FALSE; }
	return TRUE;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CTimeSet::AddSecond
//*---------------------------------------------------------------
// DESC : 현재 객체의 시간정보에 N 초만큼 더함
// PARM :	1 . i64Second - 초 (±)
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
VOID CTimeSet::AddSecond(INT64 i64Second)
{
	DN_ASSERT(0 != i64Second,	"Invalid!");

	FILETIME stFileTime;
	::SystemTimeToFileTime(&m_stLocalTime, &stFileTime);
	ULARGE_INTEGER uValue;
	uValue.LowPart = stFileTime.dwLowDateTime;
	uValue.HighPart = stFileTime.dwHighDateTime;
	uValue.QuadPart += (i64Second * 10000000);		// 100-nano로 변환
	stFileTime.dwLowDateTime = uValue.LowPart;
	stFileTime.dwHighDateTime = uValue.HighPart;
	::FileTimeToSystemTime(&stFileTime, &m_stLocalTime);
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CTimeSet::operator-
//*---------------------------------------------------------------
// DESC : 현재 객체의 시간정보에 다른 시간객체의 시간정보를 뺌
// PARM :	1 . Time - 시간객체
// RETV : 현재 객체에서 다른 객체의 시간정보를 뺀 값 (단위:초)
// PRGM : B4nFter
//*---------------------------------------------------------------
INT64 CTimeSet::operator-(const CTimeSet& pTime) const
{
	FILETIME stFileTime;

	::SystemTimeToFileTime(&m_stLocalTime, &stFileTime);
	LARGE_INTEGER lValue1;
	lValue1.LowPart	= stFileTime.dwLowDateTime;
	lValue1.HighPart	= static_cast<LONG>(stFileTime.dwHighDateTime);

	::SystemTimeToFileTime(&pTime.m_stLocalTime, &stFileTime);
	LARGE_INTEGER lValue2;
	lValue2.LowPart = stFileTime.dwLowDateTime;
	lValue2.HighPart = static_cast<LONG>(stFileTime.dwHighDateTime);

	lValue1.QuadPart /= 10000000;	// 100-nano를 초로 변환.
	lValue2.QuadPart /= 10000000;
	return(static_cast<INT64>(lValue1.QuadPart - lValue2.QuadPart));
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CTimeSet::GetTimeT32_LC
//*---------------------------------------------------------------
// DESC : 현재 시간정보를 __time32_t 시간정보로 변환
// PARM : N/A
// RETV : __time32_t 시간정보
// PRGM : B4nFter
// P.S.>
//		- _mktime32() 을 사용하며 이것은 현재 시스템의 달력시간을 기준으로 변환
//		- 달력시간 (Calendar Value) 은 환경변수 TZ 의 영향을 받음 (ex> 대한민국 TZ = GMT - 09:00:00)
//*---------------------------------------------------------------
__time32_t CTimeSet::GetTimeT32_LC() const
{
	tm stTm;
//	::memset(&stTm, 0, sizeof(stTm));	// 영향 없음 (헤더 주석 참조할 것)

	stTm.tm_year	= m_stLocalTime.wYear - DF_TM_YEAR_EPOCH;
	stTm.tm_mon		= m_stLocalTime.wMonth - 1;
	stTm.tm_mday	= m_stLocalTime.wDay;
	stTm.tm_hour	= m_stLocalTime.wHour;
	stTm.tm_min		= m_stLocalTime.wMinute;
	stTm.tm_sec		= m_stLocalTime.wSecond;
#if defined(_US)
	_timeb summertime;
	_ftime(&summertime);
	stTm.tm_isdst = summertime.dstflag;
#else
	stTm.tm_isdst = 0; // Seriously, i don't know.
#endif

	return(::_mktime32(&stTm));
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CTimeSet::GetTimeT64_LC
//*---------------------------------------------------------------
// DESC : 현재 시간정보를 __time64_t 시간정보로 변환
// PARM : N/A
// RETV : __time64_t 시간정보
// PRGM : B4nFter
// P.S.>
//		- _mktime64() 을 사용하며 이것은 현재 시스템의 달력시간을 기준으로 변환
//		- 달력시간 (Calendar Value) 은 환경변수 TZ 의 영향을 받음 (ex> 대한민국 TZ = GMT - 09:00:00)
//*---------------------------------------------------------------
__time64_t CTimeSet::GetTimeT64_LC() const
{
	tm stTm;
//	::memset(&stTm, 0, sizeof(stTm));	// 영향 없음 (헤더 주석 참조할 것)

	stTm.tm_year	= m_stLocalTime.wYear - DF_TM_YEAR_EPOCH;
	stTm.tm_mon		= m_stLocalTime.wMonth - 1;
	stTm.tm_mday	= m_stLocalTime.wDay;
	stTm.tm_hour	= m_stLocalTime.wHour;
	stTm.tm_min		= m_stLocalTime.wMinute;
	stTm.tm_sec		= m_stLocalTime.wSecond;
#if defined(_US)
	__timeb64 summertime;
	_ftime64(&summertime);
	stTm.tm_isdst = summertime.dstflag;
#else
	stTm.tm_isdst = 0; // Seriously, i don't know.
#endif
	return(::_mktime64(&stTm));
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CTimeSet::GetTimeT32_GM
//*---------------------------------------------------------------
// DESC : 현재 시간정보를 __time32_t 시간정보로 변환
// PARM : N/A
// RETV : __time32_t 시간정보
// PRGM : B4nFter
// P.S.>
//		- _mkgmtime32() 을 사용하며 이것은 GMT 기준으로 변환
//		- GMT (Greenwich Mean Time) 은 영국 그리니치 천문대 기준의 세계 표준시 (시간관련 C 런타임 라이브러리에서는 UTC (Coodinated Universal Time, 협정 세계시) 와 사실상 같은 의미로 사용됨)
//*---------------------------------------------------------------
__time32_t CTimeSet::GetTimeT32_GM() const
{
	tm stTm;
//	::memset(&stTm, 0, sizeof(stTm));	// 영향 없음 (헤더 주석 참조할 것)

	stTm.tm_year	= m_stLocalTime.wYear - DF_TM_YEAR_EPOCH;
	stTm.tm_mon		= m_stLocalTime.wMonth - 1;
	stTm.tm_mday	= m_stLocalTime.wDay;
	stTm.tm_hour	= m_stLocalTime.wHour;
	stTm.tm_min		= m_stLocalTime.wMinute;
	stTm.tm_sec		= m_stLocalTime.wSecond;

	return(::_mkgmtime32(&stTm));
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CTimeSet::GetTimeT64_GM
//*---------------------------------------------------------------
// DESC : 현재 시간정보를 __time64_t 시간정보로 변환
// PARM : N/A
// RETV : __time64_t 시간정보
// PRGM : B4nFter
// P.S.>
//		- _mkgmtime64() 을 사용하며 이것은 GMT 기준으로 변환
//		- GMT (Greenwich Mean Time) 은 영국 그리니치 천문대 기준의 세계 표준시 (시간관련 C 런타임 라이브러리에서는 UTC (Coodinated Universal Time, 협정 세계시) 와 사실상 같은 의미로 사용됨)
//*---------------------------------------------------------------
__time64_t CTimeSet::GetTimeT64_GM() const
{
	tm stTm;
//	::memset(&stTm, 0, sizeof(stTm));	// 영향 없음 (헤더 주석 참조할 것)

	stTm.tm_year	= m_stLocalTime.wYear - DF_TM_YEAR_EPOCH;
	stTm.tm_mon		= m_stLocalTime.wMonth - 1;
	stTm.tm_mday	= m_stLocalTime.wDay;
	stTm.tm_hour	= m_stLocalTime.wHour;
	stTm.tm_min		= m_stLocalTime.wMinute;
	stTm.tm_sec		= m_stLocalTime.wSecond;

	return(::_mkgmtime64(&stTm));
}


TIMESTAMP_STRUCT& CTimeSet::GetDBTimeStampStruct()
{
	m_DBTimeStampStruct.year = m_stLocalTime.wYear;
	m_DBTimeStampStruct.month = m_stLocalTime.wMonth;
	m_DBTimeStampStruct.day = m_stLocalTime.wDay;
	m_DBTimeStampStruct.hour = m_stLocalTime.wHour;
	m_DBTimeStampStruct.minute = m_stLocalTime.wMinute;
	m_DBTimeStampStruct.second = m_stLocalTime.wSecond;
	m_DBTimeStampStruct.fraction = static_cast<ULONG>(m_stLocalTime.wMilliseconds*1000000);

	return m_DBTimeStampStruct;
}

//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CTimeSet::GetDbTimeStamp
//*---------------------------------------------------------------
// DESC : 현재 객체에 지정된 시간정보를 DBTIMESTAMP 형태로 반환
// PARM : N/A
// RETV : DBTIMESTAMP 시간정보
// PRGM : B4nFter
//*---------------------------------------------------------------
DBTIMESTAMP& CTimeSet::GetDbTimeStamp()
{
	m_stDbTimeStamp.year = m_stLocalTime.wYear;
	m_stDbTimeStamp.month = m_stLocalTime.wMonth;
	m_stDbTimeStamp.day = m_stLocalTime.wDay;
	m_stDbTimeStamp.hour = m_stLocalTime.wHour;
	m_stDbTimeStamp.minute = m_stLocalTime.wMinute;
	m_stDbTimeStamp.second = m_stLocalTime.wSecond;
	m_stDbTimeStamp.fraction = static_cast<ULONG>(m_stLocalTime.wMilliseconds*1000000);
	return m_stDbTimeStamp;
}

DBDATE& CTimeSet::GetDBDate()
{
	m_DBDate.year = m_stLocalTime.wYear;
	m_DBDate.month = m_stLocalTime.wMonth;
	m_DBDate.day = m_stLocalTime.wDay;

	return m_DBDate;
}

//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CTimeSet::ConvertFileTimeToDbTimeStamp
//*---------------------------------------------------------------
// DESC : 특정 FILETIME 시간정보를 DBTIMESTAMP 시간정보로 변환
// PARM :	1 . lpFileTime - 시간정보
//			2 . lpDbTime - DBTIMESTAMP 시간정보 출력버퍼
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
VOID CTimeSet::ConvertFileTimeToDbTimeStamp(const FILETIME* lpFileTime, DBTIMESTAMP* lpDbTime)
{
	DN_ASSERT(NULL != lpFileTime,	"Invalid!");
	DN_ASSERT(NULL != lpDbTime,		"Invalid!");

	SYSTEMTIME stSystemTime;
	::FileTimeToSystemTime(lpFileTime, &stSystemTime);

	lpDbTime->year     = stSystemTime.wYear;
	lpDbTime->month    = stSystemTime.wMonth;
	lpDbTime->day      = stSystemTime.wDay;
	lpDbTime->hour     = stSystemTime.wHour;
	lpDbTime->minute   = stSystemTime.wMinute;
	lpDbTime->second   = stSystemTime.wSecond;
	lpDbTime->fraction = static_cast<ULONG>(stSystemTime.wMilliseconds * 1000000);

	if (lpDbTime->year < 1900) {
		lpDbTime->year = 1900;
	}
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CTimeSet::ConvertDbTimeStampToFileTime
//*---------------------------------------------------------------
// DESC : 특정 DBTIMESTAMP 시간정보를 FILETIME 시간정보로 변환
// PARM :	1 . lpDbTime - DBTIMESTAMP 시간정보
//			2 . lpFileTime - FILETIME 시간정보 출력버퍼
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
VOID CTimeSet::ConvertDbTimeStampToFileTime(const DBTIMESTAMP* lpDbTime, FILETIME* lpFileTime)
{
	DN_ASSERT(NULL != lpDbTime,		"Invalid!");
	DN_ASSERT(NULL != lpFileTime,	"Invalid!");

	SYSTEMTIME stSystemTime;
//	::memset(&stSystemTime, 0, sizeof(stSystemTime));	// 아래 모든 멤버를 세팅하고 있음

	stSystemTime.wDayOfWeek    = 0;
	stSystemTime.wYear         = lpDbTime->year;
	stSystemTime.wMonth        = lpDbTime->month;
	stSystemTime.wDay          = lpDbTime->day;
	stSystemTime.wHour         = lpDbTime->hour;
	stSystemTime.wMinute       = lpDbTime->minute;
	stSystemTime.wSecond       = lpDbTime->second;
	stSystemTime.wMilliseconds = static_cast<WORD>(lpDbTime->fraction / 1000000);

	::SystemTimeToFileTime(&stSystemTime, lpFileTime);
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CTimeSet::ConvertTmToTimeT32_LC
//*---------------------------------------------------------------
// DESC : 특정 tm 시간정보를 __time32_t 시간정보로 변환
// PARM :	1 . lpTm - tm 시간정보
// RETV : __time32_t 시간정보
// PRGM : B4nFter
// P.S.>
//		- _mktime32() 을 사용하며 이것은 현재 시스템의 달력시간을 기준으로 변환
//		- 달력시간 (Calendar Value) 은 환경변수 TZ 의 영향을 받음 (ex> 대한민국 TZ = GMT - 09:00:00)
//*---------------------------------------------------------------
__time32_t CTimeSet::ConvertTmToTimeT32_LC(tm* lpTm)
{
	DN_ASSERT(NULL != lpTm,	"Invalid!");

	return(::_mktime32(lpTm));
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CTimeSet::ConvertTmToTimeT64_LC
//*---------------------------------------------------------------
// DESC : 특정 tm 시간정보를 __time64_t 시간정보로 변환
// PARM :	1 . lpTm - tm 시간정보
// RETV : __time64_t 시간정보
// PRGM : B4nFter
// P.S.>
//		- _mktime64() 을 사용하며 이것은 현재 시스템의 달력시간을 기준으로 변환
//		- 달력시간 (Calendar Value) 은 환경변수 TZ 의 영향을 받음 (ex> 대한민국 TZ = GMT - 09:00:00)
//*---------------------------------------------------------------
__time64_t CTimeSet::ConvertTmToTimeT64_LC(tm* lpTm)
{
	DN_ASSERT(NULL != lpTm,	"Invalid!");

	return(::_mktime64(lpTm));
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CTimeSet::ConvertTmToTimeT32_GM
//*---------------------------------------------------------------
// DESC : 특정 tm 시간정보를 __time32_t 시간정보로 변환
// PARM :	1 . lpTm - tm 시간정보
// RETV : __time32_t 시간정보
// PRGM : B4nFter
// P.S.>
//		- _mkgmtime32() 을 사용하며 이것은 GMT 기준으로 변환
//		- GMT (Greenwich Mean Time) 은 영국 그리니치 천문대 기준의 세계 표준시 (시간관련 C 런타임 라이브러리에서는 UTC (Coodinated Universal Time, 협정 세계시) 와 사실상 같은 의미로 사용됨)
//*---------------------------------------------------------------
__time32_t CTimeSet::ConvertTmToTimeT32_GM(tm* lpTm)
{
	DN_ASSERT(NULL != lpTm,	"Invalid!");

	return(::_mkgmtime32(lpTm));
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CTimeSet::ConvertTmToTimeT64_GM
//*---------------------------------------------------------------
// DESC : 특정 tm 시간정보를 __time64_t 시간정보로 변환
// PARM :	1 . lpTm - tm 시간정보
// RETV : __time64_t 시간정보
// PRGM : B4nFter
// P.S.>
//		- _mkgmtime64() 을 사용하며 이것은 GMT 기준으로 변환
//		- GMT (Greenwich Mean Time) 은 영국 그리니치 천문대 기준의 세계 표준시 (시간관련 C 런타임 라이브러리에서는 UTC (Coodinated Universal Time, 협정 세계시) 와 사실상 같은 의미로 사용됨)
//*---------------------------------------------------------------
__time64_t CTimeSet::ConvertTmToTimeT64_GM(tm* lpTm)
{
	DN_ASSERT(NULL != lpTm,	"Invalid!");

	return(::_mkgmtime64(lpTm));
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CTimeSet::ConvertDbTimeStampToTimeT32_LC
//*---------------------------------------------------------------
// DESC : 특정 DBTIMESTAMP 시간정보를 __time32_t 시간정보로 변환
// PARM :	1 . lpDbTime - DBTIMESTAMP 시간정보
// RETV : __time32_t 시간정보
// PRGM : B4nFter
// P.S.>
//		- _mktime32() 을 사용하며 이것은 현재 시스템의 달력시간을 기준으로 변환
//		- 달력시간 (Calendar Value) 은 환경변수 TZ 의 영향을 받음 (ex> 대한민국 TZ = GMT - 09:00:00)
//*---------------------------------------------------------------
__time32_t CTimeSet::ConvertDbTimeStampToTimeT32_LC(const DBTIMESTAMP*lpDbTime)
{
	DN_ASSERT(NULL != lpDbTime,	"Invalid!");

	tm stTm;
//	::memset(&stTm, 0, sizeof(stTm));	// 영향 없음 (헤더 주석 참조할 것)

	stTm.tm_year	= lpDbTime->year - DF_TM_YEAR_EPOCH;
	stTm.tm_mon		= lpDbTime->month - 1;
	stTm.tm_mday	= lpDbTime->day;
	stTm.tm_hour	= lpDbTime->hour;
	stTm.tm_min		= lpDbTime->minute;
	stTm.tm_sec		= lpDbTime->second;
#if defined(_US)
	_timeb summertime;
	_ftime(&summertime);
	stTm.tm_isdst = summertime.dstflag;
#else
	stTm.tm_isdst = 0; // Seriously, i don't know.
#endif
	return(::_mktime32(&stTm));
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CTimeSet::ConvertDbTimeStampToTimeT64_LC
//*---------------------------------------------------------------
// DESC : 특정 DBTIMESTAMP 시간정보를 __time64_t 시간정보로 변환
// PARM :	1 . lpDbTime - DBTIMESTAMP 시간정보
// RETV : __time64_t 시간정보
// PRGM : B4nFter
// P.S.>
//		- _mktime64() 을 사용하며 이것은 현재 시스템의 달력시간을 기준으로 변환
//		- 달력시간 (Calendar Value) 은 환경변수 TZ 의 영향을 받음 (ex> 대한민국 TZ = GMT - 09:00:00)
//*---------------------------------------------------------------
__time64_t CTimeSet::ConvertDbTimeStampToTimeT64_LC(const DBTIMESTAMP* lpDbTime)
{
	DN_ASSERT(NULL != lpDbTime,	"Invalid!");

	tm stTm;
//	::memset(&stTm, 0, sizeof(stTm));	// 영향 없음 (헤더 주석 참조할 것)

	stTm.tm_year	= lpDbTime->year - DF_TM_YEAR_EPOCH;
	stTm.tm_mon		= lpDbTime->month - 1;
	stTm.tm_mday	= lpDbTime->day;
	stTm.tm_hour	= lpDbTime->hour;
	stTm.tm_min		= lpDbTime->minute;
	stTm.tm_sec		= lpDbTime->second;
#if defined(_US)
	__timeb64 summertime;
	_ftime64(&summertime);
	stTm.tm_isdst = summertime.dstflag;
#else
	stTm.tm_isdst = 0; // Seriously, i don't know.
#endif
	return(::_mktime64(&stTm));
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CTimeSet::ConvertDbTimeStampToTimeT32_GM
//*---------------------------------------------------------------
// DESC : 특정 DBTIMESTAMP 시간정보를 __time32_t 시간정보로 변환
// PARM :	1 . lpDbTime - DBTIMESTAMP 시간정보
// RETV : __time32_t 시간정보
// PRGM : B4nFter
// P.S.>
//		- _mkgmtime32() 을 사용하며 이것은 GMT 기준으로 변환
//		- GMT (Greenwich Mean Time) 은 영국 그리니치 천문대 기준의 세계 표준시 (시간관련 C 런타임 라이브러리에서는 UTC (Coodinated Universal Time, 협정 세계시) 와 사실상 같은 의미로 사용됨)
//*---------------------------------------------------------------
__time32_t CTimeSet::ConvertDbTimeStampToTimeT32_GM(const DBTIMESTAMP*lpDbTime)
{
	DN_ASSERT(NULL != lpDbTime,	"Invalid!");

	tm stTm;
//	::memset(&stTm, 0, sizeof(stTm));	// 영향 없음 (헤더 주석 참조할 것)

	stTm.tm_year	= lpDbTime->year - DF_TM_YEAR_EPOCH;
	stTm.tm_mon		= lpDbTime->month - 1;
	stTm.tm_mday	= lpDbTime->day;
	stTm.tm_hour	= lpDbTime->hour;
	stTm.tm_min		= lpDbTime->minute;
	stTm.tm_sec		= lpDbTime->second;

	return(::_mkgmtime32(&stTm));
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CTimeSet::ConvertDbTimeStampToTimeT64_GM
//*---------------------------------------------------------------
// DESC : 특정 DBTIMESTAMP 시간정보를 __time64_t 시간정보로 변환
// PARM :	1 . lpDbTime - DBTIMESTAMP 시간정보
// RETV : __time64_t 시간정보
// PRGM : B4nFter
// P.S.>
//		- _mkgmtime64() 을 사용하며 이것은 GMT 기준으로 변환
//		- GMT (Greenwich Mean Time) 은 영국 그리니치 천문대 기준의 세계 표준시 (시간관련 C 런타임 라이브러리에서는 UTC (Coodinated Universal Time, 협정 세계시) 와 사실상 같은 의미로 사용됨)
//*---------------------------------------------------------------
__time64_t CTimeSet::ConvertDbTimeStampToTimeT64_GM(const DBTIMESTAMP* lpDbTime)
{
	DN_ASSERT(NULL != lpDbTime,	"Invalid!");

	tm stTm;
//	::memset(&stTm, 0, sizeof(stTm));	// 영향 없음 (헤더 주석 참조할 것)

	stTm.tm_year	= lpDbTime->year - DF_TM_YEAR_EPOCH;
	stTm.tm_mon		= lpDbTime->month - 1;
	stTm.tm_mday	= lpDbTime->day;
	stTm.tm_hour	= lpDbTime->hour;
	stTm.tm_min		= lpDbTime->minute;
	stTm.tm_sec		= lpDbTime->second;

	return(::_mkgmtime64(&stTm));
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CTimeSet::ConvertTimeT32ToTm_LC
//*---------------------------------------------------------------
// DESC : 특정 __time32_t 시간정보를 tm 시간정보로 변환
// PARM :	1 . pTimeT32 - __time32_t 시간정보
//			2 . lpTm - tm 시간정보 출력버퍼
// RETV : TRUE - 성공 / FALSE - 실패
// PRGM : B4nFter
// P.S.>
//		- _localtime32_s() 을 사용하며 이것은 현재 시스템의 달력시간을 기준으로 변환
//		- 달력시간 (Calendar Value) 은 환경변수 TZ 의 영향을 받음 (ex> 대한민국 TZ = GMT - 09:00:00)
//*---------------------------------------------------------------
BOOL CTimeSet::ConvertTimeT32ToTm_LC(__time32_t pTimeT32, tm* lpTm)
{
	DN_ASSERT(NULL != lpTm,	"Invalid!");

	errno_t aRetVal = ::_localtime32_s(lpTm, &pTimeT32);
	if (aRetVal) {
		return FALSE;
	}

	lpTm->tm_year += DF_TM_YEAR_EPOCH;
	lpTm->tm_mon += 1;

	return TRUE;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CTimeSet::ConvertTimeT64ToTm_LC
//*---------------------------------------------------------------
// DESC : 특정 __time64_t 시간정보를 tm 시간정보로 변환
// PARM :	1 . pTimeT64 - __time64_t 시간정보
//			2 . lpTm - tm 시간정보 출력버퍼
// RETV : TRUE - 성공 / FALSE - 실패
// PRGM : B4nFter
// P.S.>
//		- _localtime64_s() 을 사용하며 이것은 현재 시스템의 달력시간을 기준으로 변환
//		- 달력시간 (Calendar Value) 은 환경변수 TZ 의 영향을 받음 (ex> 대한민국 TZ = GMT - 09:00:00)
//*---------------------------------------------------------------
BOOL CTimeSet::ConvertTimeT64ToTm_LC(__time64_t pTimeT64, tm* lpTm)
{
	DN_ASSERT(NULL != lpTm,	"Invalid!");

	errno_t aRetVal = ::_localtime64_s(lpTm, &pTimeT64);
	if (aRetVal) {
		return FALSE;
	}

	lpTm->tm_year += DF_TM_YEAR_EPOCH;
	lpTm->tm_mon += 1;

	return TRUE;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CTimeSet::ConvertTimeT32ToTm_GM
//*---------------------------------------------------------------
// DESC : 특정 __time32_t 시간정보를 tm 시간정보로 변환
// PARM :	1 . pTimeT32 - __time32_t 시간정보
//			2 . lpTm - tm 시간정보 출력버퍼
// RETV : TRUE - 성공 / FALSE - 실패
// PRGM : B4nFter
// P.S.>
//		- _gmtime32_s() 을 사용하며 이것은 GMT 기준으로 변환
//		- GMT (Greenwich Mean Time) 은 영국 그리니치 천문대 기준의 세계 표준시 (시간관련 C 런타임 라이브러리에서는 UTC (Coodinated Universal Time, 협정 세계시) 와 사실상 같은 의미로 사용됨)
//*---------------------------------------------------------------
BOOL CTimeSet::ConvertTimeT32ToTm_GM(__time32_t pTimeT32, tm* lpTm)
{
	DN_ASSERT(NULL != lpTm,	"Invalid!");

	errno_t aRetVal = ::_gmtime32_s(lpTm, &pTimeT32);
	if (aRetVal) {
		return FALSE;
	}

	lpTm->tm_year += DF_TM_YEAR_EPOCH;
	lpTm->tm_mon += 1;

	return TRUE;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CTimeSet::ConvertTimeT64ToTm_GM
//*---------------------------------------------------------------
// DESC : 특정 __time64_t 시간정보를 tm 시간정보로 변환
// PARM :	1 . pTimeT64 - __time64_t 시간정보
//			2 . lpTm - tm 시간정보 출력버퍼
// RETV : TRUE - 성공 / FALSE - 실패
// PRGM : B4nFter
// P.S.>
//		- _gmtime32_s() 을 사용하며 이것은 GMT 기준으로 변환
//		- GMT (Greenwich Mean Time) 은 영국 그리니치 천문대 기준의 세계 표준시 (시간관련 C 런타임 라이브러리에서는 UTC (Coodinated Universal Time, 협정 세계시) 와 사실상 같은 의미로 사용됨)
//*---------------------------------------------------------------
BOOL CTimeSet::ConvertTimeT64ToTm_GM(__time64_t pTimeT64, tm* lpTm)
{
	DN_ASSERT(NULL != lpTm,	"Invalid!");

	errno_t aRetVal = ::_gmtime64_s(lpTm, &pTimeT64);
	if (aRetVal) {
		return FALSE;
	}

	lpTm->tm_year += DF_TM_YEAR_EPOCH;
	lpTm->tm_mon += 1;

	return TRUE;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CTimeSet::ConvertTimeT32ToDbTimeStamp_LC
//*---------------------------------------------------------------
// DESC : 특정 __time32_t 시간정보를 DBTIMESTAMP 시간정보로 변환
// PARM :	1 . pTimeT32 - __time32_t 시간정보
//			2 . lpDbTime - DBTIMESTAMP 시간정보 출력버퍼
// RETV : TRUE - 성공 / FALSE - 실패
// PRGM : B4nFter
// P.S.>
//		- _localtime32_s() 을 사용하며 이것은 현재 시스템의 달력시간을 기준으로 변환
//		- 달력시간 (Calendar Value) 은 환경변수 TZ 의 영향을 받음 (ex> 대한민국 TZ = GMT - 09:00:00)
//*---------------------------------------------------------------
BOOL CTimeSet::ConvertTimeT32ToDbTimeStamp_LC(__time32_t pTimeT32, DBTIMESTAMP* lpDbTime)
{
	DN_ASSERT(NULL != lpDbTime,	"Invalid!");

	tm stTm;
	errno_t aRetVal = ::_localtime32_s(&stTm, &pTimeT32);
	if (aRetVal) {
		return FALSE;
	}

	lpDbTime->year		= static_cast<SHORT>(stTm.tm_year + DF_TM_YEAR_EPOCH);
	lpDbTime->month		= static_cast<USHORT>(stTm.tm_mon + 1);
	lpDbTime->day		= static_cast<USHORT>(stTm.tm_mday);
	lpDbTime->hour		= static_cast<USHORT>(stTm.tm_hour);
	lpDbTime->minute	= static_cast<USHORT>(stTm.tm_min);
	lpDbTime->second	= static_cast<USHORT>(stTm.tm_sec);
	lpDbTime->fraction	= 0;

	return TRUE;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CTimeSet::ConvertTimeT64ToDbTimeStamp_LC
//*---------------------------------------------------------------
// DESC : 특정 __time64_t 시간정보를 DBTIMESTAMP 시간정보로 변환
// PARM :	1 . pTimeT64 - __time64_t 시간정보
//			2 . lpDbTime - DBTIMESTAMP 시간정보 출력버퍼
// RETV : TRUE - 성공 / FALSE - 실패
// PRGM : B4nFter
// P.S.>
//		- _localtime64_s() 을 사용하며 이것은 현재 시스템의 달력시간을 기준으로 변환
//		- 달력시간 (Calendar Value) 은 환경변수 TZ 의 영향을 받음 (ex> 대한민국 TZ = GMT - 09:00:00)
//*---------------------------------------------------------------
BOOL CTimeSet::ConvertTimeT64ToDbTimeStamp_LC(__time64_t pTimeT64, DBTIMESTAMP* lpDbTime)
{
	DN_ASSERT(NULL != lpDbTime,	"Invalid!");

	tm stTm;
	errno_t aRetVal = ::_localtime64_s(&stTm, &pTimeT64);
	if (aRetVal) {
		return FALSE;
	}

	lpDbTime->year		= static_cast<SHORT>(stTm.tm_year + DF_TM_YEAR_EPOCH);
	lpDbTime->month		= static_cast<USHORT>(stTm.tm_mon + 1);
	lpDbTime->day		= static_cast<USHORT>(stTm.tm_mday);
	lpDbTime->hour		= static_cast<USHORT>(stTm.tm_hour);
	lpDbTime->minute	= static_cast<USHORT>(stTm.tm_min);
	lpDbTime->second	= static_cast<USHORT>(stTm.tm_sec);
	lpDbTime->fraction	= 0;

	return TRUE;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CTimeSet::ConvertTimeT32ToDbTimeStamp_GM
//*---------------------------------------------------------------
// DESC : 특정 __time32_t 시간정보를 DBTIMESTAMP 시간정보로 변환
// PARM :	1 . pTimeT32 - __time32_t 시간정보
//			2 . lpDbTime - DBTIMESTAMP 시간정보 출력버퍼
// RETV : TRUE - 성공 / FALSE - 실패
// PRGM : B4nFter
// P.S.>
//		- _gmtime32_s() 을 사용하며 이것은 GMT 기준으로 변환
//		- GMT (Greenwich Mean Time) 은 영국 그리니치 천문대 기준의 세계 표준시 (시간관련 C 런타임 라이브러리에서는 UTC (Coodinated Universal Time, 협정 세계시) 와 사실상 같은 의미로 사용됨)
//*---------------------------------------------------------------
BOOL CTimeSet::ConvertTimeT32ToDbTimeStamp_GM(__time32_t pTimeT32, DBTIMESTAMP* lpDbTime)
{
	DN_ASSERT(NULL != lpDbTime,	"Invalid!");

	tm stTm;
	errno_t aRetVal = ::_gmtime32_s(&stTm, &pTimeT32);
	if (aRetVal) {
		return FALSE;
	}

	lpDbTime->year		= static_cast<SHORT>(stTm.tm_year + DF_TM_YEAR_EPOCH);
	lpDbTime->month		= static_cast<USHORT>(stTm.tm_mon + 1);
	lpDbTime->day		= static_cast<USHORT>(stTm.tm_mday);
	lpDbTime->hour		= static_cast<USHORT>(stTm.tm_hour);
	lpDbTime->minute	= static_cast<USHORT>(stTm.tm_min);
	lpDbTime->second	= static_cast<USHORT>(stTm.tm_sec);
	lpDbTime->fraction	= 0;

	return TRUE;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CTimeSet::ConvertTimeT64ToDbTimeStamp_GM
//*---------------------------------------------------------------
// DESC : 특정 __time64_t 시간정보를 DBTIMESTAMP 시간정보로 변환
// PARM :	1 . pTimeT64 - __time64_t 시간정보
//			2 . lpDbTime - DBTIMESTAMP 시간정보 출력버퍼
// RETV : TRUE - 성공 / FALSE - 실패
// PRGM : B4nFter
// P.S.>
//		- _gmtime32_s() 을 사용하며 이것은 GMT 기준으로 변환
//		- GMT (Greenwich Mean Time) 은 영국 그리니치 천문대 기준의 세계 표준시 (시간관련 C 런타임 라이브러리에서는 UTC (Coodinated Universal Time, 협정 세계시) 와 사실상 같은 의미로 사용됨)
//*---------------------------------------------------------------
BOOL CTimeSet::ConvertTimeT64ToDbTimeStamp_GM(__time64_t pTimeT64, DBTIMESTAMP* lpDbTime)
{
	DN_ASSERT(NULL != lpDbTime,	"Invalid!");

	tm stTm;
	errno_t aRetVal = ::_gmtime64_s(&stTm, &pTimeT64);
	if (aRetVal) {
		return FALSE;
	}

	lpDbTime->year		= static_cast<SHORT>(stTm.tm_year + DF_TM_YEAR_EPOCH);
	lpDbTime->month		= static_cast<USHORT>(stTm.tm_mon + 1);
	lpDbTime->day		= static_cast<USHORT>(stTm.tm_mday);
	lpDbTime->hour		= static_cast<USHORT>(stTm.tm_hour);
	lpDbTime->minute	= static_cast<USHORT>(stTm.tm_min);
	lpDbTime->second	= static_cast<USHORT>(stTm.tm_sec);
	lpDbTime->fraction	= 0;

	return TRUE;
}


CTimeSet::EF_DAYOFWEEK CTimeSet::CalculateDayOfWeek()		// 20100728
{
	return(CTimeSet::CalculateDayOfWeek(m_stLocalTime.wYear, m_stLocalTime.wMonth, m_stLocalTime.wDay));
}


CTimeSet::EF_DAYOFWEEK CTimeSet::CalculateDayOfWeek(WORD wYear, WORD wMonth, WORD wDay)		// 20100728
{
	INT aMonthDays[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };	// 12 Month

	long aTotal = (wYear-1)*365L + (wYear-1)/4 - (wYear-1)/100 + (wYear-1)/400;

	if (!(wYear%4) && (wYear%100) || !(wYear%400)) {
		++aMonthDays[1];
	}

	for (int aIndex = 0 ; wMonth-1 > aIndex ; ++aIndex) {
		aTotal += aMonthDays[aIndex];
	}

	aTotal += wDay;

	return(static_cast<CTimeSet::EF_DAYOFWEEK>(aTotal % 7));
}

