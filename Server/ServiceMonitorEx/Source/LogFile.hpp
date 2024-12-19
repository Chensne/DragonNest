/*//===============================================================

	<< LOG FILE >>

	PRGM : B4nFter

	FILE : LOGFILE.HPP
	DESC : �α���� ���ϰ��� Ŭ����
	INIT BUILT DATE : 2005. 01. 12
	LAST BUILT DATE : 2007. 04. 10

	Copyright�� 2002~2009 B4nFter (b4nfter@gmail.com) and Co-Writers. All Rights Reserved.

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
// DESC : �α׸� ���Ͽ� ����ϱ� ���� ���� Ŭ����
// PRGM : B4nFter
//*---------------------------------------------------------------
class CLogFile : public CLogBase
{
public:
	typedef VOID (*LPFN_OUTPUT_LOGPROC)(LPCTSTR /*�α��ؽ�Ʈ*/, LPVOID /*������Ķ����*/);		// �ܺ��� �α���� �Լ��� �� �α���� ��ü�� ����� ������ ���� �Լ������� ����

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

	inline virtual BOOL Write(LPCTSTR lpszFormat, ...);			// P.S.> ����� ����, ���� ���� (*.h, *.cpp �� �и� �ʿ�)
	inline virtual BOOL WriteDate(LPCTSTR lpszFormat, ...);		// P.S.> ����� ����, ���� ���� (*.h, *.cpp �� �и� �ʿ�)
	inline virtual BOOL WriteDirect(LPCTSTR lpszText);			// P.S.> ����� ����, ���� ���� (*.h, *.cpp �� �и� �ʿ�)
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
// DESC : ������
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
// DESC : �Ҹ���
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
// DESC : ��ü ���� �ڿ����� �ʱ�ȭ
// PARM : N/A
// RETV : TRUE - ���� / FALSE - ����
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
// DESC : ��ü ���� �ڿ����� �ʱ�ȭ
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
// DESC : �αױ�� ������ ����
// PARM :	1 . lpszFileName - �α����� �̸�
//			2 . lpszDirName - �α������� ��ġ�� ���� �̸�
//			3 . lpszExtName - �α������� Ȯ���� �̸�
//			4 . bAddDate - �α����� �̸��� ��¥�� ������ ����
// RETV : TRUE - ���� / FALSE - ����
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
	// �����ڵ��� ��� UCS-2 (Little Endian) �� ��Ÿ���� 0xFF, 0xFE �� �� ����Ʈ�� �ٿ��־�� ��
	::fputc(0xFF, m_lpLogFile);
	::fputc(0xFE, m_lpLogFile);
#endif	// _UNICODE

	return TRUE;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CLogFile::Close
//*---------------------------------------------------------------
// DESC : �αױ�� ������ ����
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
// DESC : �α׸� ���Ͽ� ��� (��¥����)
// PARM :	1 . lpszFormat - �α׳���
// RETV : TRUE - ���� / FALSE - ����
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
#if	(_MSC_VER >= 1400)	// VS.NET 2005 �̻�
	iChrWritten = _vsntprintf_s(m_szTextBuffer, COUNT_OF(m_szTextBuffer), lpszFormat, lpArgPtr);
#else	// #if	(_MSC_VER >= 1400)
	#pragma warning (disable:4995)
	iChrWritten = _vsntprintf(m_szTextBuffer, COUNT_OF(m_szTextBuffer), lpszFormat, lpArgPtr);
	#pragma warning (default:4995)
#endif	// #if	(_MSC_VER >= 1400)
	va_end(lpArgPtr);

#if	(_MSC_VER >= 1400)	// VS.NET 2005 �̻�
	_ftprintf_s(m_lpLogFile, _T("%s\r\n"), m_szTextBuffer);
#else	// #if	(_MSC_VER >= 1400)
	_ftprintf(m_lpLogFile, _T("%s\r\n"), m_szTextBuffer);
#endif	// #if	(_MSC_VER >= 1400)

	// !!! ���� - �������, �������� ���� ?
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
// DESC : �α׸� ���Ͽ� ��� (��¥����)
// PARM :	1 . lpszFormat - �α׳���
// RETV : TRUE - ���� / FALSE - ����
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
#if	(_MSC_VER >= 1400)	// VS.NET 2005 �̻�
	iChrWritten = _vsntprintf_s(m_szTextBuffer, COUNT_OF(m_szTextBuffer), lpszFormat, lpArgPtr);
#else	// #if	(_MSC_VER >= 1400)
	#pragma warning (disable:4995)
	iChrWritten = _vsntprintf(m_szTextBuffer, COUNT_OF(m_szTextBuffer), lpszFormat, lpArgPtr);
	#pragma warning (default:4995)
#endif	// #if	(_MSC_VER >= 1400)
	va_end(lpArgPtr);

	::GetLocalTime(& m_stSysTime);

	// ���Ϸα��� ��� �� ���Ͽ� ���� ��¥�� �̾��� �� �ֱ� ������ ��/��/�� �� ���� ���
#if	(_MSC_VER >= 1400)	// VS.NET 2005 �̻�
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

	// !!! ���� - �������, �������� ���� ?
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
// DESC : �α׸� ���� ���� ���� ���Ͽ� ���
// PARM :	1 . lpszText - �α׳���
// RETV : TRUE - ���� / FALSE - ����
// PRGM : B4nFter
//*---------------------------------------------------------------
BOOL CLogFile::WriteDirect(LPCTSTR lpszText)
{
	if (!m_lpLogFile) {
		BASE_TRACE(_T("!m_lpLogFile - CLogFile::WriteDirect()\n"));
		return FALSE;
	}

	CLockAuto AutoLock(& m_crLogFile);

#if	(_MSC_VER >= 1400)	// VS.NET 2005 �̻�
	_ftprintf_s(m_lpLogFile, _T("%s\r\n"), lpszText);
#else	// #if	(_MSC_VER >= 1400)
	_ftprintf(m_lpLogFile, _T("%s\r\n"), lpszText);
#endif	// #if	(_MSC_VER >= 1400)

	// !!! ���� - �������, �������� ���� ?
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
// DESC : ��¥�� ����Ǿ����� ���θ� Ȯ���Ͽ� �����̸� ����
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
// DESC : �Էµ� ��¥��� �����̸� ����
// PARM :	1 . stSysTime - �����̸��� ������ ��¥
// RETV : TRUE - ���� / FALSE - ����
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

