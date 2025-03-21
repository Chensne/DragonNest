

///////////////////////////////////////////////////////////////////
//	INCLUDE
///////////////////////////////////////////////////////////////////

#include "STDAFX.H"
#include "ERRORFORMAT.H"


///////////////////////////////////////////////////////////////////
//	IMPLEMENTATION
///////////////////////////////////////////////////////////////////


//*---------------------------------------------------------------
// TYPE : VARIABLE
// NAME : CErrorFormat::m_hNetMsg
//*---------------------------------------------------------------
// DESC : 윈속 에러포맷을 얻기 위해 NETMSG.DLL 에서 데이터 타입으로 로드하는 DLL 핸들을 소유
// PRGM : B4nFter
// P.S.>
//		- 최초 1회만 로드하면 모든 객체가 같이 사용할 수 있으며 중간에 변경될 염려도 없으므로 static 멤버로 함
//		- 이 클래스를 멤버객체로 소유한 모든 클래스들에서 m_hNetMsg 를 각각 초기화하여 문제가 되었음 (에러번호:8, 시스템자원 고갈)
//		- 해제는 별도로 하지 않으며 프로세스 종료 시 시스템이 알아서 해제시키는 것으로 함
//*---------------------------------------------------------------
HMODULE CErrorFormat::m_hNetMsg = NULL;


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CErrorFormat::CErrorFormat
//*---------------------------------------------------------------
// DESC : 생성자
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
CErrorFormat::CErrorFormat()
{
	if (!m_hNetMsg) {
		m_hNetMsg = ::LoadLibraryEx(_T("netmsg.dll"), NULL, LOAD_LIBRARY_AS_DATAFILE);
	}

	Clear();
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CErrorFormat::~CErrorFormat
//*---------------------------------------------------------------
// DESC : 소멸자
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
CErrorFormat::~CErrorFormat()
{
	
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CErrorFormat::Write
//*---------------------------------------------------------------
// DESC : 시스템 에러코드를 해석하여 메시지 저장
// PARM :	1 . iErrorCode - 시스템 에러코드
//			2 . lpszErrorMessage - 추가할 에러메시지
// RETV : 해석된 에러 메시지
// PRGM : B4nFter
//*---------------------------------------------------------------
CONST LPTSTR CErrorFormat::Write(INT iErrorCode, LPCTSTR lpszErrorMessage)
{
	DWORD dwFormatFlags = FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_SYSTEM;
	LPCVOID lpModule = NULL;

	if (iErrorCode >= NERR_BASE && iErrorCode <= MAX_NERR && NULL != m_hNetMsg) {
		lpModule = m_hNetMsg;
		dwFormatFlags |= FORMAT_MESSAGE_FROM_HMODULE;
	}

	m_szSystemErrorMessage[0] = _T('\0');
	::FormatMessage(dwFormatFlags, lpModule, iErrorCode, 0, m_szSystemErrorMessage, COUNT_OF(m_szSystemErrorMessage), NULL);
	if (m_szSystemErrorMessage[0] != _T('\0')) {
		LPTSTR lpszNewLine = _tcschr(m_szSystemErrorMessage, _T('\r'));
		if (lpszNewLine) {
			*lpszNewLine = _T('\0');
		}
 		SNPRINTF(MODE_DBG_EX(m_szTotalErrorMessage, COUNT_OF(m_szTotalErrorMessage)), COUNT_OF(m_szTotalErrorMessage), _T("[%d] %s"), iErrorCode, m_szSystemErrorMessage);
	}
	if (lpszErrorMessage) {
		STRNCAT(m_szTotalErrorMessage, lpszErrorMessage, COUNT_OF(m_szTotalErrorMessage));
	}

	m_iErrorCode = iErrorCode;

	return m_szTotalErrorMessage;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CErrorFormat::Write
//*---------------------------------------------------------------
// DESC : 사용자 메시지 저장
// PARM :	1 . lpszErrorMessage - 사용자 에러메시지
// RETV : 저장된 에러 메시지
// PRGM : B4nFter
//*---------------------------------------------------------------
CONST LPTSTR CErrorFormat::Write(LPCTSTR lpszErrorMessage)
{
	if (lpszErrorMessage) {
		_tcsncpy_s(m_szTotalErrorMessage, COUNT_OF(m_szTotalErrorMessage), lpszErrorMessage, _TRUNCATE);
	}
	else {
		_tcsncpy_s(m_szTotalErrorMessage, COUNT_OF(m_szTotalErrorMessage), _T(""), _TRUNCATE);
	}

	m_iErrorCode = 0;

	return m_szTotalErrorMessage;
}

