/*//===============================================================

	<< TIME SET >>

	FILE : TIMESET.H, TIMESET.CPP
	DESC : 시간 관리 클래스
	INIT BUILT DATE : 2007. 04. 22
	LAST BUILT DATE : 2008. 05. 29

	P.S.>
		- tm 구조체 구조
			int tm_sec          : 초 (0 ~ 59) (*)
			int tm_min          : 분 0 에서 59 까지의 범위를 갖는 분(minute) (*)
			int tm_hour         : 시 0 에서 23까지의 범위를 갖는 시(hours) (*)
			int tm_mday         : 일 (한달주기, 1 ~ 31) (*)
			int tm_mon          : 월 (0 ~ 11) (*)
			int tm_year         : 년 (1900년 이후) (*)
			int tm_wday         : 일 (한주주기, 0 ~ 6)
			int tm_yday			: 일 (한해주기, 0 ~ 365)
			int tm_isdst		: 일광 절약 시간제 (서머타임) 플래그

			P.S.> 데이터 멤버 설명 뒤에 (*) 가 붙는 멤버만이 __time32_t, __time64_t 로 변환 시 영향을 주고 나머지는 무관

		- DBTIMESTAMP 구조체 구조
			SHORT year			: 년 (A.D. 이후 0 ~ 9999)
			USHORT month		: 월 (1 ~ 12)
			USHORT day			: 일 (1 ~ 31)
			USHORT hour			: 시 (0 ~ 23)
			USHORT minute		: 분 (0 ~ 59) 
			USHORT second		: 초 (0 ~ 59) 
			ULONG fraction		: 정밀도 (1/10억 초까지 정밀도를 표현, 0 ~ 999,999,999)

		- __time32_t 정밀도		: 1970-01-01 00:00:00 ~ 2038-01-18 12:14:07
		- __time64_t 정밀도		: 1970-01-01 00:00:00 ~ 2999-12-31 23:59:59

	Copyrightⓒ 2002~2009 B4nFter (b4nfter@gmail.com) and Co-Writers. All Rights Reserved.

*///===============================================================


#pragma once


///////////////////////////////////////////////////////////////////
//	INCLUDE
///////////////////////////////////////////////////////////////////

#include <WINDOWS.H>
#include <OLEDB.H>
#include <sqltypes.h>


///////////////////////////////////////////////////////////////////
//	DEFINE
///////////////////////////////////////////////////////////////////

#define DF_TM_YEAR_EPOCH		(1900)					// tm 구조체 년도 구분 값
#define DF_TIME32_MAX			(0x7FFFFFFF)			// __time32_t 데이터형의 최대값
#define DF_TIME64_MAX			(0x00000007933FF16F)	// __time64_t 데이터형의 최대값


///////////////////////////////////////////////////////////////////
//	DECLARATION
///////////////////////////////////////////////////////////////////

//*---------------------------------------------------------------
// TYPE : CLASS
// NAME : CTimeSet
//*---------------------------------------------------------------
// DESC : 시간 관리 클래스
// PRGM : B4nFter
//*---------------------------------------------------------------
class CTimeSet
{
public:
	// SYSTEMTIME::wDayOfWeek 의 값순서와 동일해야 함
	enum EF_DAYOFWEEK { EV_DAYOFWEEK_SUN=0, EV_DAYOFWEEK_MON, EV_DAYOFWEEK_TUE, EV_DAYOFWEEK_WEN, EV_DAYOFWEEK_THU, EV_DAYOFWEEK_FRI, EV_DAYOFWEEK_SAT };
public:
	CTimeSet();
	CTimeSet(WORD wYear, WORD wMonth, WORD wDay, WORD wHour, WORD wMinute, WORD wSecond);
	CTimeSet(const __time32_t& pTime, bool pIsLocal);	// 20100506
	CTimeSet(const __time64_t& pTime, bool pIsLocal);	// 20100506
	CTimeSet(const char* szTime, bool pIsLocal);		// 20110104 by robust
	explicit CTimeSet(const FILETIME& stTime);
	explicit CTimeSet(const SYSTEMTIME& stTime);
	explicit CTimeSet(const DBTIMESTAMP& stTime);

public:
	WORD GetYear() const { return m_stLocalTime.wYear; }
	WORD GetMonth() const { return m_stLocalTime.wMonth; }
	WORD GetDay() const { return m_stLocalTime.wDay; }
	WORD GetHour() const { return m_stLocalTime.wHour; }
	WORD GetMinute() const { return m_stLocalTime.wMinute; }
	WORD GetSecond() const { return m_stLocalTime.wSecond; }
	WORD GetMilliseconds() const { return m_stLocalTime.wMilliseconds; }
	WORD GetDayOfWeek() const { return m_stLocalTime.wDayOfWeek	; }

	const SYSTEMTIME& GetSystemTime() const { return m_stLocalTime; }
	SYSTEMTIME& GetSystemTime() { return m_stLocalTime; }
	__time32_t GetTimeT32_LC() const;	// 20100506
	__time64_t GetTimeT64_LC() const;	// 20100506
	__time32_t GetTimeT32_GM() const;	// 20100506
	__time64_t GetTimeT64_GM() const;	// 20100506

	TIMESTAMP_STRUCT& GetDBTimeStampStruct();
	DBTIMESTAMP& GetDbTimeStamp();
	DBDATE& GetDBDate();

	EF_DAYOFWEEK CalculateDayOfWeek();	// 20100728

	VOID Reset();
	VOID Set(WORD wYear, WORD wMonth, WORD wDay, WORD wHour, WORD wMinute,WORD wSecond);
	VOID Set(const __time32_t& pTime, bool pIsLocal);
	VOID Set(const __time64_t& pTime, bool pIsLocal);
	VOID Set(const FILETIME& stTime);
	VOID Set(const SYSTEMTIME& stTime);
	VOID Set(const DBTIMESTAMP& stTime);

	DWORD GetTimeTo(EF_DAYOFWEEK eDayOfWeek, WORD wHour, WORD wMinute, WORD wSecond);	// 최대차이 : 주 (단위:1/1000초)
	DWORD GetTimeTo(WORD wHour, WORD wMinute, WORD wSecond);	// 최대차이 : 일 (단위:1/1000초)
	DWORD GetTimeTo(WORD wMinute, WORD wSecond);	// 최대차이 : 시간 (단위:1/1000초)

	INT CompareFileTime(const FILETIME& stFileTime1, const FILETIME& stFileTime2);	// stFileTime1 와 stFileTime2 중 어느쪽이 큰지 비교 (-1:첫번째큼 / 0:같음 / 1:두번째큼)
	BOOL CheckIntegrity();

	VOID AddSecond(INT64 ui64Second);	// 20100728
	INT64 operator-(const CTimeSet& pTime) const;

	static VOID ConvertFileTimeToDbTimeStamp(const FILETIME* lpFileTime, DBTIMESTAMP* lpDbTime);
	static VOID ConvertDbTimeStampToFileTime(const DBTIMESTAMP* lpDbTime, FILETIME* lpFileTime);
	static __time32_t ConvertTmToTimeT32_LC(tm* lpTm);
	static __time64_t ConvertTmToTimeT64_LC(tm* lpTm);
	static __time32_t ConvertTmToTimeT32_GM(tm* lpTm);
	static __time64_t ConvertTmToTimeT64_GM(tm* lpTm);
	static __time32_t ConvertDbTimeStampToTimeT32_LC(const DBTIMESTAMP* lpDbTime);
	static __time64_t ConvertDbTimeStampToTimeT64_LC(const DBTIMESTAMP* lpDbTime);
	static __time32_t ConvertDbTimeStampToTimeT32_GM(const DBTIMESTAMP* lpDbTime);
	static __time64_t ConvertDbTimeStampToTimeT64_GM(const DBTIMESTAMP* lpDbTime);
	static BOOL ConvertTimeT32ToTm_LC(__time32_t pTimeT32, tm* lpTm);
	static BOOL ConvertTimeT64ToTm_LC(__time64_t pTimeT64, tm* lpTm);
	static BOOL ConvertTimeT32ToTm_GM(__time32_t pTimeT32, tm* lpTm);
	static BOOL ConvertTimeT64ToTm_GM(__time64_t pTimeT64, tm* lpTm);
	static BOOL ConvertTimeT32ToDbTimeStamp_LC(__time32_t pTimeT32, DBTIMESTAMP* lpDbTime);
	static BOOL ConvertTimeT64ToDbTimeStamp_LC(__time64_t pTimeT64, DBTIMESTAMP* lpDbTime);
	static BOOL ConvertTimeT32ToDbTimeStamp_GM(__time32_t pTimeT32, DBTIMESTAMP* lpDbTime);
	static BOOL ConvertTimeT64ToDbTimeStamp_GM(__time64_t pTimeT64, DBTIMESTAMP* lpDbTime);
	static EF_DAYOFWEEK CalculateDayOfWeek(WORD wYear, WORD wMonth, WORD wDay);	// 20100728

private:
	SYSTEMTIME m_stLocalTime;		// 실제 현재 시간을 유지하는 변수
	TIMESTAMP_STRUCT m_DBTimeStampStruct;
	DBTIMESTAMP m_stDbTimeStamp;	// 필요할 때만 사용하는 임시변수
	DBDATE m_DBDate;
};

