/*//===============================================================

	<< LOG FILE >>

	PRGM : B4nFter

	FILE : LOGFILE.HPP
	DESC : 로그출력 파일관리 클래스
	INIT BUILT DATE : 2005. 01. 12
	LAST BUILT DATE : 2007. 04. 10

	Copyrightⓒ 2002~2009 B4nFter (b4nfter@gmail.com) and Co-Writers. All Rights Reserved.

*///===============================================================


#pragma once


///////////////////////////////////////////////////////////////////
//	INCLUDE
///////////////////////////////////////////////////////////////////

#include "LOGBASE.HPP"
#include "CRITICALSECTION.HPP"


///////////////////////////////////////////////////////////////////
//	DEFINE
///////////////////////////////////////////////////////////////////

#define DF_DEFAULT_LOGFILENAME			_T("LOGFILE")
#define DF_DEFAULT_LOGEXTNAME			_T("log")
#define DF_DEFAULT_LINEBUFFERSIZE		(1024 * 64)


///////////////////////////////////////////////////////////////////
//	DECLARATION
///////////////////////////////////////////////////////////////////

//*---------------------------------------------------------------
// TYPE : CLASS
// NAME : CLogFile
//*---------------------------------------------------------------
// DESC : 로그를 파일에 출력하기 위한 관리 클래스
// PRGM : B4nFter
//*---------------------------------------------------------------
class CLogFile : public CLogBase
{
public:
	typedef VOID (*LPFN_OUTPUT_LOGPROC)(LPCTSTR /*로그텍스트*/, LPVOID /*사용자파라메터*/);		// 외부의 로그출력 함수로 이 로그출력 객체의 결과를 보내기 위한 함수포인터 선언

private:
	FILE* m_lpLogFile;
	TCHAR m_szFileName[MAX_PATH];
	TCHAR m_szDirName[MAX_PATH];
	TCHAR m_szExtName[MAX_PATH];
	TCHAR m_szTextBuffer[DF_DEFAULT_LINEBUFFERSIZE];

	SYSTEMTIME m_stCurSysTime;
	SYSTEMTIME m_stSysTime;
	CCriticalSection m_crLogFile;

public:
	inline CLogFile();
	inline ~CLogFile();

	inline BOOL Create(
		LPCTSTR lpszFileName = DF_DEFAULT_LOGFILENAME, 
		LPCTSTR lpszDirName = NULL,
		LPCTSTR lpszExtName = DF_DEFAULT_LOGEXTNAME,
		BOOL bAddDate = TRUE
		);
	inline VOID Close();

	inline virtual BOOL Write(LPCTSTR lpszFormat, ...);			// P.S.> 명백한 오류, 차후 수정 (*.h, *.cpp 로 분리 필요)
	inline virtual BOOL WriteDate(LPCTSTR lpszFormat, ...);		// P.S.> 명백한 오류, 차후 수정 (*.h, *.cpp 로 분리 필요)
	inline virtual BOOL WriteDirect(LPCTSTR lpszText);			// P.S.> 명백한 오류, 차후 수정 (*.h, *.cpp 로 분리 필요)
	inline VOID UpdateFileNameDate();

private:
	inline BOOL Initialize();
	inline VOID Finalize();

	inline BOOL ChangeFileNameDate(SYSTEMTIME& stSysTime);
};


///////////////////////////////////////////////////////////////////
//	IMPLEMENTATION
///////////////////////////////////////////////////////////////////

//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CLogFile::CLogFile
//*---------------------------------------------------------------
// DESC : 생성자
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
CLogFile::CLogFile()
{
	Initialize();
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CLogFile::~CLogFile
//*---------------------------------------------------------------
// DESC : 소멸자
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
CLogFile::~CLogFile()
{
	Finalize();
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CLogFile::Initialize
//*---------------------------------------------------------------
// DESC : 객체 관련 자원들을 초기화
// PARM : N/A
// RETV : TRUE - 성공 / FALSE - 실패
// PRGM : B4nFter
//*---------------------------------------------------------------
BOOL CLogFile::Initialize()
{
	m_lpLogFile = NULL;
	::memset(m_szDirName, 0, sizeof(m_szDirName));
	::memset(m_szFileName, 0, sizeof(m_szFileName));
	if (NOERROR != m_crLogFile.Open()) {
		BASE_RETURN(FALSE);
	}
	return TRUE;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CLogFile::Finalize
//*---------------------------------------------------------------
// DESC : 객체 관련 자원들을 초기화
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
VOID CLogFile::Finalize()
{
	Close();
	m_crLogFile.Close();
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CLogFile::Create
//*---------------------------------------------------------------
// DESC : 로그기록 파일을 생성
// PARM :	1 . lpszFileName - 로그파일 이름
//			2 . lpszDirName - 로그파일이 위치할 폴더 이름
//			3 . lpszExtName - 로그파일의 확장자 이름
//			4 . bAddDate - 로그파일 이름에 날짜를 붙일지 여부
// RETV : TRUE - 성공 / FALSE - 실패
// PRGM : B4nFter
//*---------------------------------------------------------------
BOOL CLogFile::Create(LPCTSTR lpszFileName, LPCTSTR lpszDirName, LPCTSTR lpszExtName, BOOL bAddDate)
{
	CLockAuto AutoLock(& m_crLogFile);

	::GetLocalTime (& m_stSysTime);
	m_stCurSysTime = m_stSysTime;

	if (!lstrcmp(lpszFileName, _T(""))) {
		STRNCPY(m_szFileName, DF_DEFAULT_LOGFILENAME, COUNT_OF(m_szFileName));
	}
	else {
		STRNCPY(m_szFileName, lpszFileName, COUNT_OF(m_szFileName));
	}

	if (!lpszExtName || !lstrcmp(lpszExtName, _T(""))) {
		::memset(m_szExtName, 0, sizeof(m_szExtName));
	}
	else {
		STRNCPY(m_szExtName, lpszExtName, COUNT_OF(m_szExtName));
	}

	TCHAR szLogFileFullName[MAX_PATH];
	if (!lpszDirName || !lstrcmp(lpszDirName, _T(""))) {
		::memset(m_szDirName, 0, sizeof(m_szDirName));

		if (bAddDate) {
			SNPRINTF(MODE_DBG_EX(szLogFileFullName, COUNT_OF(szLogFileFullName)), COUNT_OF(szLogFileFullName),
				_T(".\\%s_%04d%02d%02d%02d%02d%02d.%s"),
				m_szFileName,
				m_stSysTime.wYear,
				m_stSysTime.wMonth,
				m_stSysTime.wDay,
				m_stSysTime.wHour,
				m_stSysTime.wMinute,
				m_stSysTime.wSecond,
				m_szExtName
				);
		}
		else {
			SNPRINTF(MODE_DBG_EX(szLogFileFullName, COUNT_OF(szLogFileFullName)), COUNT_OF(szLogFileFullName),
				_T(".\\%s.%s"),
				m_szFileName,
				m_szExtName
				);
		}
	}
	else {
		STRNCPY(m_szDirName, lpszDirName, COUNT_OF(m_szDirName));
		::CreateDirectory(m_szDirName, NULL);

		if (bAddDate) {
			SNPRINTF(MODE_DBG_EX(szLogFileFullName, COUNT_OF(szLogFileFullName)), COUNT_OF(szLogFileFullName),
				_T("%s\\%s_%04d%02d%02d%02d%02d%02d.%s"),
				m_szDirName,
				m_szFileName,
				m_stSysTime.wYear,
				m_stSysTime.wMonth,
				m_stSysTime.wDay,
				m_stSysTime.wHour,
				m_stSysTime.wMinute,
				m_stSysTime.wSecond,
				m_szExtName
				);
		}
		else {
			SNPRINTF(MODE_DBG_EX(szLogFileFullName, COUNT_OF(szLogFileFullName)), COUNT_OF(szLogFileFullName),
				_T("%s\\%s.%s"),
				m_szDirName,
				m_szFileName,
				m_szExtName
				);
		}
	}

	m_lpLogFile = _tfopen(szLogFileFullName, _T("a+b"));
	if (!m_lpLogFile) {
		BASE_TRACE(_T("!m_lpLogFile - CLogFile::Create()\n"));
		return FALSE;
	}

#if defined(_UNICODE)
	// 유니코드일 경우 UCS-2 (Little Endian) 을 나타내는 0xFF, 0xFE 의 두 바이트를 붙여주어야 함
	::fputc(0xFF, m_lpLogFile);
	::fputc(0xFE, m_lpLogFile);
#endif	// _UNICODE

	return TRUE;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CLogFile::Close
//*---------------------------------------------------------------
// DESC : 로그기록 파일을 닫음
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
VOID CLogFile::Close()
{
	CLockAuto AutoLock(& m_crLogFile);

	if (m_lpLogFile) {
		::fclose (m_lpLogFile);
		m_lpLogFile = NULL;
	}
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CLogFile::Write
//*---------------------------------------------------------------
// DESC : 로그를 파일에 기록 (날짜없음)
// PARM :	1 . lpszFormat - 로그내용
// RETV : TRUE - 성공 / FALSE - 실패
// PRGM : B4nFter
//*---------------------------------------------------------------
BOOL CLogFile::Write(LPCTSTR lpszFormat, ...)
{
	if (!m_lpLogFile) {
		BASE_TRACE(_T("!m_lpLogFile - CLogFile::WriteDate()\n"));
		return FALSE;
	}

	va_list lpArgPtr;
	INT iChrWritten;

	CLockAuto AutoLock(& m_crLogFile);

	va_start(lpArgPtr, lpszFormat);
#if	(_MSC_VER >= 1400)	// VS.NET 2005 이상
	iChrWritten = _vsntprintf_s(m_szTextBuffer, COUNT_OF(m_szTextBuffer), lpszFormat, lpArgPtr);
#else	// #if	(_MSC_VER >= 1400)
	#pragma warning (disable:4995)
	iChrWritten = _vsntprintf(m_szTextBuffer, COUNT_OF(m_szTextBuffer), lpszFormat, lpArgPtr);
	#pragma warning (default:4995)
#endif	// #if	(_MSC_VER >= 1400)
	va_end(lpArgPtr);

#if	(_MSC_VER >= 1400)	// VS.NET 2005 이상
	_ftprintf_s(m_lpLogFile, _T("%s\r\n"), m_szTextBuffer);
#else	// #if	(_MSC_VER >= 1400)
	_ftprintf(m_lpLogFile, _T("%s\r\n"), m_szTextBuffer);
#endif	// #if	(_MSC_VER >= 1400)

	// !!! 주의 - 무한재귀, 교착상태 유발 ?
/*
	if (m_lpLogLink) {
		m_lpLogLink->Write(lpszFormat, lpArgPtr);
	}
*/

	return TRUE;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CLogFile::WriteDate
//*---------------------------------------------------------------
// DESC : 로그를 파일에 기록 (날짜있음)
// PARM :	1 . lpszFormat - 로그내용
// RETV : TRUE - 성공 / FALSE - 실패
// PRGM : B4nFter
//*---------------------------------------------------------------
BOOL CLogFile::WriteDate(LPCTSTR lpszFormat, ...)
{
	if (!m_lpLogFile) {
		BASE_TRACE(_T("!m_lpLogFile - CLogFile::WriteDate()\n"));
		return FALSE;
	}

	va_list lpArgPtr;
	INT iChrWritten;

	CLockAuto AutoLock(& m_crLogFile);

	va_start(lpArgPtr, lpszFormat);
#if	(_MSC_VER >= 1400)	// VS.NET 2005 이상
	iChrWritten = _vsntprintf_s(m_szTextBuffer, COUNT_OF(m_szTextBuffer), lpszFormat, lpArgPtr);
#else	// #if	(_MSC_VER >= 1400)
	#pragma warning (disable:4995)
	iChrWritten = _vsntprintf(m_szTextBuffer, COUNT_OF(m_szTextBuffer), lpszFormat, lpArgPtr);
	#pragma warning (default:4995)
#endif	// #if	(_MSC_VER >= 1400)
	va_end(lpArgPtr);

	::GetLocalTime(& m_stSysTime);

	// 파일로그의 경우 한 파일에 여러 날짜가 이어질 수 있기 때문에 년/월/일 을 같이 기록
#if	(_MSC_VER >= 1400)	// VS.NET 2005 이상
	_ftprintf_s(m_lpLogFile, _T("%04d-%02d-%02d %02d:%02d:%02d  %s\r\n"), 
#else	// #if	(_MSC_VER >= 1400)
	_ftprintf(m_lpLogFile, _T("%04d-%02d-%02d %02d:%02d:%02d  %s\r\n"), 
#endif	// #if	(_MSC_VER >= 1400)
		m_stSysTime.wYear,
		m_stSysTime.wMonth,
		m_stSysTime.wDay,
		m_stSysTime.wHour,
		m_stSysTime.wMinute,
		m_stSysTime.wSecond,
		m_szTextBuffer
		);

	// !!! 주의 - 무한재귀, 교착상태 유발 ?
/*
	if (m_lpLogLink) {
		m_lpLogLink->WriteDate(lpszFormat, lpArgPtr);
	}
*/

	return TRUE;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CLogFile::WriteDirect
//*---------------------------------------------------------------
// DESC : 로그를 형식 가공 없이 파일에 기록
// PARM :	1 . lpszText - 로그내용
// RETV : TRUE - 성공 / FALSE - 실패
// PRGM : B4nFter
//*---------------------------------------------------------------
BOOL CLogFile::WriteDirect(LPCTSTR lpszText)
{
	if (!m_lpLogFile) {
		BASE_TRACE(_T("!m_lpLogFile - CLogFile::WriteDirect()\n"));
		return FALSE;
	}

	CLockAuto AutoLock(& m_crLogFile);

#if	(_MSC_VER >= 1400)	// VS.NET 2005 이상
	_ftprintf_s(m_lpLogFile, _T("%s\r\n"), lpszText);
#else	// #if	(_MSC_VER >= 1400)
	_ftprintf(m_lpLogFile, _T("%s\r\n"), lpszText);
#endif	// #if	(_MSC_VER >= 1400)

	// !!! 주의 - 무한재귀, 교착상태 유발 ?
/*
	if (m_lpLogLink) {
		m_lpLogLink->WriteDirect(lpszText);
	}
*/

	return TRUE;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CLogFile::UpdateFileNameDate
//*---------------------------------------------------------------
// DESC : 날짜가 변경되었는지 여부를 확인하여 파일이름 변경
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
VOID CLogFile::UpdateFileNameDate()
{
	CLockAuto AutoLock(& m_crLogFile);

	::GetLocalTime(& m_stSysTime);

	if (m_stSysTime.wDay != m_stCurSysTime.wDay) {
		ChangeFileNameDate(m_stSysTime);
		m_stCurSysTime = m_stSysTime;
	}
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CLogFile::ChangeFileNameDate
//*---------------------------------------------------------------
// DESC : 입력된 날짜대로 파일이름 변경
// PARM :	1 . stSysTime - 파일이름을 변경할 날짜
// RETV : TRUE - 성공 / FALSE - 실패
// PRGM : B4nFter
//*---------------------------------------------------------------
BOOL CLogFile::ChangeFileNameDate(SYSTEMTIME& stSysTime)
{
	if (m_lpLogFile) {
		::fclose(m_lpLogFile);

		m_stSysTime = stSysTime;

		TCHAR szLogFileFullName[MAX_PATH];

		if (!lstrcmp(m_szDirName, _T(""))) {
			SNPRINTF(MODE_DBG_EX(szLogFileFullName, COUNT_OF(szLogFileFullName)), COUNT_OF(szLogFileFullName),
				_T(".\\%s_%04d%02d%02d%02d%02d%02d.%s"),
				m_szFileName,
				m_stSysTime.wYear,
				m_stSysTime.wMonth,
				m_stSysTime.wDay,
				m_stSysTime.wHour,
				m_stSysTime.wMinute,
				m_stSysTime.wSecond,
				m_szExtName
				);
		}
		else {
			::CreateDirectory(m_szDirName, NULL);

			SNPRINTF(MODE_DBG_EX(szLogFileFullName, COUNT_OF(szLogFileFullName)), COUNT_OF(szLogFileFullName),
				_T("%s\\%s_%04d%02d%02d%02d%02d%02d.%s"),
				m_szDirName,
				m_szFileName,
				m_stSysTime.wYear,
				m_stSysTime.wMonth,
				m_stSysTime.wDay,
				m_stSysTime.wHour,
				m_stSysTime.wMinute,
				m_stSysTime.wSecond,
				m_szExtName
				);
		}

		if (!(m_lpLogFile = _tfopen(szLogFileFullName, _T("a+")))) {
			BASE_TRACE(_T("!(m_lpLogFile = _tfopen(szLogFileFullName, _T(\"a+\"))) - CLogFile::ChangeFileNameDate()\n"));
			return FALSE;
		}
	}

	return TRUE;
}

