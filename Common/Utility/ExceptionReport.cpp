

///////////////////////////////////////////////////////////////////
//	INCLUDE
///////////////////////////////////////////////////////////////////

#include "STDAFX.H"
#include "EXCEPTIONREPORT.H"
#include "TIMESET.H"
#include "COMMONMACROS.H"
#include "BASESET.H"
#include "DEBUGUTIL.H"
#include "SAFESTRINGMACROS.H"
#include "SundriesFunc.h"

///////////////////////////////////////////////////////////////////
//	IMPLEMENTATION
///////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

DEFINE_SINGLETON_CLASS(CExceptionReport);

// 잘못된 STL Container index 참조
static void myInvalidParameterHandler(const wchar_t* expression,
									  const wchar_t* function, 
									  const wchar_t* file, 
									  unsigned int line, 
									  uintptr_t pReserved)
{
	RaiseException( EXCEPTION_ACCESS_VIOLATION, EXCEPTION_NONCONTINUABLE , 0, 0);
}

// 잘못된 Pure virtual function call
static void myPurecallHandler()
{
	RaiseException( EXCEPTION_ACCESS_VIOLATION, EXCEPTION_NONCONTINUABLE , 0, 0);
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CExceptionReport::CExceptionReport
//*---------------------------------------------------------------
// DESC : 생성자
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
CExceptionReport::CExceptionReport() : m_MiniDumpType(MiniDumpNormal), m_bDoSetFilter(TRUE), m_bDoCallStack(TRUE), m_InitSymbolEngine(FALSE), m_lpfnOutputLogProc(NULL), m_lpOutputLogProcParam(NULL), m_bIsDumping(false)
{
#if defined(_X86_)
	m_MachineType	= IMAGE_FILE_MACHINE_I386;
#elif defined(_AMD64_)
	m_MachineType	= IMAGE_FILE_MACHINE_AMD64;
#elif defined(_IA64_)
	m_MachineType	= IMAGE_FILE_MACHINE_IA64;
#else	// !_X86 && !_AMD64_ && !_IA64_
	#pragma FORCE COMPILE ABORT!
#endif	// !_X86 && !_AMD64_ && !_IA64_

	::memset(m_szText, 0, sizeof(m_szText));
	::memset(m_szExceptionDesc, 0, sizeof(m_szExceptionDesc));
	::memset(m_btSymbol, 0, sizeof(m_btSymbol));
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CExceptionReport::~CExceptionReport
//*---------------------------------------------------------------
// DESC : 소멸자
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
CExceptionReport::~CExceptionReport()
{
	
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CExceptionReport::Open
//*---------------------------------------------------------------
// DESC : 객체자원 초기화
// PARM :	1 . lpszFilePath - 예외 레포트와 미니덤프 파일이 저장될 폴더 경로
//			2 . bDoSetFilter - 처리되지 않은 예외처리 필터 설정여부 (기본값 TRUE)
//			3 . bDoCallStack - 콜스택 저장 여부 (기본값 TRUE)
//			4 . pMiniDumpType - 미니덤프 기록형태
// RETV : NOERROR - 성공 / 그외 - 실패
// PRGM : B4nFter
//*---------------------------------------------------------------
DWORD CExceptionReport::Open(LPCTSTR lpszFilePath, BOOL bDoSetFilter, BOOL bDoCallStack, MINIDUMP_TYPE pMiniDumpType)
{
	DN_STRING(lpszFilePath,	MAX_PATH);

/*
	DWORD dwRetVal = m_Lock.Open();
	if (NOERROR != dwRetVal) {
		DN_RETURN(dwRetVal);
	}
*/

	STRNCPY(m_szFilePath, COUNT_OF(m_szFilePath), lpszFilePath);

	DWORD dwRetVal = m_MiniDump.Open();
	if (NOERROR != dwRetVal) {
		DN_RETURN(dwRetVal);
	}

	m_bDoSetFilter = bDoSetFilter;
	m_bDoCallStack = bDoCallStack;
	m_MiniDumpType = pMiniDumpType;

	if (m_bDoSetFilter) {
		m_pInvalidParameterHandler = _set_invalid_parameter_handler( myInvalidParameterHandler );
		m_pPureCallHandler = _set_purecall_handler( myPurecallHandler );
		::SetUnhandledExceptionFilter(Proc);
	}

	return NOERROR;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CExceptionReport::Close
//*---------------------------------------------------------------
// DESC : 객체자원 해제
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
VOID CExceptionReport::Close()
{
	::SetUnhandledExceptionFilter( NULL );	
	_set_purecall_handler( m_pPureCallHandler );
	_set_invalid_parameter_handler( m_pInvalidParameterHandler );
	
	m_MiniDump.Close();
//	m_Lock.Close();
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CExceptionReport::SetUserMessage
//*---------------------------------------------------------------
// DESC : 미니덤프에 남게될 사용자 메시지를 지정
// PARM :	1 . lpszUserMessage - 사용자 메시지
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
VOID CExceptionReport::SetUserMessage(LPCTSTR lpszUserMessage)
{
	m_MiniDump.SetUserMessage(lpszUserMessage);
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CExceptionReport::SetOutputLogProc
//*---------------------------------------------------------------
// DESC : 예외 레포트를 출력할 외부 프로시저를 등록
// PARM :	1 . lpfnOutputLogProc - 로그 출력 프로시저 포인터
//			2 . lpOutputLogProcParam - 파라메터
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
VOID CExceptionReport::SetOutputLogProc(CExceptionReport::LPFN_OUTPUT_LOGPROC lpfnOutputLogProc, LPVOID lpOutputLogProcParam)
{
	m_lpfnOutputLogProc = lpfnOutputLogProc;
	m_lpOutputLogProcParam = lpOutputLogProcParam;
}

DWORD WINAPI CExceptionReport::_StackOverflowException( LPVOID pParam )
{
	return(ProcNoLock( static_cast<_EXCEPTION_POINTERS*>(pParam), GetInstancePtr()->GetMiniDumpType()));
}

//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CExceptionReport::Proc
//*---------------------------------------------------------------
// DESC : 예외처리 필터에서 예외를 수신했을 때 실제로 이를 처리할 콜백함수 (잠금있음)
// PARM :	1 . lpstExceptionPointers - EXCEPTION_POINTERS 구조체 포인터
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
LONG WINAPI CExceptionReport::Proc(LPEXCEPTION_POINTERS lpstExceptionPointers)
{
	TP_LOCKAUTO aLockAuto(GetInstancePtr()->m_Lock);

	if( lpstExceptionPointers->ExceptionRecord->ExceptionCode == EXCEPTION_STACK_OVERFLOW ) {
		HANDLE hThread = ::CreateThread( 0, 0, _StackOverflowException, ( LPVOID )lpstExceptionPointers, 0, NULL );
		WaitForSingleObject( hThread, INFINITE );
		return EXCEPTION_EXECUTE_HANDLER;
	}
	return(ProcNoLock(lpstExceptionPointers, GetInstancePtr()->GetMiniDumpType()));
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CExceptionReport::Proc
//*---------------------------------------------------------------
// DESC : 예외처리 필터에서 예외를 수신했을 때 실제로 이를 처리할 콜백함수 (잠금있음)
// PARM :	1 . lpstExceptionPointers - EXCEPTION_POINTERS 구조체 포인터
//			2 . pMiniDumpType - 미니덤프 기록형태
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
LONG WINAPI CExceptionReport::Proc(LPEXCEPTION_POINTERS lpstExceptionPointers, MINIDUMP_TYPE pMiniDumpType)
{
	TP_LOCKAUTO aLockAuto(GetInstancePtr()->m_Lock);

	return(ProcNoLock(lpstExceptionPointers, pMiniDumpType));
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CExceptionReport::ProcNoLock
//*---------------------------------------------------------------
// DESC : 예외처리 필터에서 예외를 수신했을 때 실제로 이를 처리할 콜백함수 (잠금없음)
// PARM :	1 . lpstExceptionPointers - EXCEPTION_POINTERS 구조체 포인터
//			2 . pMiniDumpType - 미니덤프 기록형태
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
#pragma warning(disable:4706 4995)
LONG WINAPI CExceptionReport::ProcNoLock(LPEXCEPTION_POINTERS lpstExceptionPointers, MINIDUMP_TYPE pMiniDumpType)
{
#if defined(_SERVER)
	switch(pMiniDumpType)
	{
	case MiniDumpWithFullMemory:
		{
			if (GetFreeDiskSpaceAsGigaByte() < 5)
			{
				wprintf(L"Insufficient DiskSpace MiniDumpWithFullMemory\n");
				return EXCEPTION_EXECUTE_HANDLER;
			}
		}
		break;
	}
#endif	//defined(_SERVER)

	GetInstancePtr()->SetNowDump(true);

	__try
	{
		DN_READ(lpstExceptionPointers, sizeof(LPEXCEPTION_POINTERS));

#if defined(_DEBUG)
		// BreakPoint()에 의한 예외는 Debugger에게 통보되도록 넘김
		if (EXCEPTION_BREAKPOINT == lpstExceptionPointers->ExceptionRecord->ExceptionCode) {
			return(EXCEPTION_CONTINUE_SEARCH);
		}
#endif	// _DEBUG

		GetInstancePtr()->m_lpstExceptionPointers = lpstExceptionPointers;

		DWORD dwRetVal = GetInstancePtr()->GetMiniDump().Write(lpstExceptionPointers, pMiniDumpType, GetInstancePtr()->GetFilePath());
		if (NOERROR != dwRetVal) {
			DN_STRING(GetInstancePtr()->GetMiniDump().GetFileName(), MAX_PATH);
		}

		DN_ASSERT(!GetInstancePtr()->m_File.IsOpen(),	"Invalid!");

		TCHAR szPrefix[128] = { _T('\0'), };
		CTimeSet CurTime;
		::wsprintf(szPrefix, _T("%04d%02d%02d%02d%02d%02d%03d"),
			CurTime.GetYear(), CurTime.GetMonth(), CurTime.GetDay(),
			CurTime.GetHour(), CurTime.GetMinute(),CurTime.GetSecond(),CurTime.GetMilliseconds());
		::wsprintf(GetInstancePtr()->m_szFileName, _T("%s%s_ecp_%s.txt"),
			GetInstancePtr()->m_szFilePath, CDebugUtil::GetProgramName(), szPrefix);
		MAKE_STRING(GetInstancePtr()->m_szFileName);

		dwRetVal = GetInstancePtr()->m_File.Open(GetInstancePtr()->m_szFileName, GENERIC_WRITE, FILE_SHARE_READ, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL, NULL);
		if (NOERROR != dwRetVal) {
			DN_ASSERT(0,		"Invalid!");
		}

#if defined(_UNICODE)
		dwRetVal = GetInstancePtr()->m_File.WriteBOM(CFileSet::EV_BT_UTF16_LE);
#endif	// _UNICODE
		if (NOERROR != dwRetVal) {
			DN_ASSERT(0,		"Invalid!");
		}

		// 1 . 기본 사유
		GetInstancePtr()->Write(_T("------------------------------------------------------------------------------\r\n"), EV_WF_FILE);
		GetInstancePtr()->Write(_T("    Basic Information\r\n"), EV_WF_FILE);
		GetInstancePtr()->Write(_T("------------------------------------------------------------------------------\r\n\r\n"), EV_WF_FILE);

		LPTSTR szText = GetInstancePtr()->GetFaultReason(szPrefix);
		if (szText) {
			// 출력
			GetInstancePtr()->Write(szText, EV_WF_ALL);
			GetInstancePtr()->Write(_T("\r\n"), EV_WF_FILE);
		}
		GetInstancePtr()->Write(_T("MiniDump("), EV_WF_FILE);
		GetInstancePtr()->Write(szPrefix, EV_WF_FILE);
		GetInstancePtr()->Write(_T(") : "), EV_WF_FILE);
		GetInstancePtr()->Write(GetInstancePtr()->GetMiniDump().GetFileName(), EV_WF_FILE);
		GetInstancePtr()->Write(_T("\r\n\r\n\r\n"), EV_WF_FILE);

		// 2 . 레지스터
		GetInstancePtr()->Write(_T("------------------------------------------------------------------------------\r\n"), EV_WF_FILE);
#if defined(_X86_)
		GetInstancePtr()->Write(_T("    Registers (X86)\r\n"), EV_WF_FILE);
#elif defined(_AMD64_)
		GetInstancePtr()->Write(_T("    Registers (X64)\r\n"), EV_WF_FILE);
#elif defined(_IA64_)
		#pragma message("IA64 NOT DEFINED!!")
		#pragma FORCE COMPILATION ABORT!
#else	// !_X86 && !_AMD64_ && !_IA64_
		#pragma message("CPU NOT DEFINED!!")
		#pragma FORCE COMPILATION ABORT!
#endif	// !_X86 && !_AMD64_ && !_IA64_
		GetInstancePtr()->Write(_T("------------------------------------------------------------------------------\r\n\r\n"), EV_WF_FILE);

		szText = GetInstancePtr()->GetRegisterString();
		if (szText) {
			// 출력
			GetInstancePtr()->Write(szText, EV_WF_FILE);
		}
		GetInstancePtr()->Write(_T("\r\n\r\n\r\n"), EV_WF_FILE);

		// 3 . 콜스택
		GetInstancePtr()->Write(_T("------------------------------------------------------------------------------\r\n"), EV_WF_FILE);
		GetInstancePtr()->Write(_T("    Call Stack\r\n"), EV_WF_FILE);
		GetInstancePtr()->Write(_T("------------------------------------------------------------------------------\r\n\r\n"), EV_WF_FILE);

		if (GetInstancePtr()->DoCallStack()) {
			szText = GetInstancePtr()->GetFirstStackTraceString(szPrefix);
			if (szText) {
				// 출력
				GetInstancePtr()->Write(szText, EV_WF_ALL);
				GetInstancePtr()->Write(_T("\r\n"), EV_WF_FILE);
			}
			WHILE((szText = GetInstancePtr()->GetNextStackTraceString(szPrefix)), 1000) {
				// 출력
				GetInstancePtr()->Write(szText, EV_WF_ALL);
				GetInstancePtr()->Write(_T("\r\n"), EV_WF_FILE);
				}
		}

		GetInstancePtr()->Write(_T("\r\n\r\n\r\n"), EV_WF_FILE);

		// 4. 유저메모리
		GetInstancePtr()->Write(_T("------------------------------------------------------------------------------\r\n"), EV_WF_FILE);
		GetInstancePtr()->Write(_T("    User Memory\r\n"), EV_WF_FILE);
		GetInstancePtr()->Write(_T("------------------------------------------------------------------------------\r\n\r\n"), EV_WF_FILE);

		szText = GetInstancePtr()->GetUserMemoryString();
		if (szText) 
			GetInstancePtr()->Write(szText, EV_WF_FILE);
		GetInstancePtr()->Write(_T("\r\n"), EV_WF_FILE);
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		DN_ASSERT(0,	"Invalid!");
	}

	GetInstancePtr()->m_File.Close();
	GetInstancePtr()->SetNowDump(false);
	return(EXCEPTION_EXECUTE_HANDLER);
}
#pragma warning(default:4706 4995)

//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CExceptionReport::InitSymbolEngine
//*---------------------------------------------------------------
// DESC : 심볼엔진 초기화
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
VOID CExceptionReport::InitSymbolEngine()
{
	if (m_InitSymbolEngine) {
		return;
	}

	DWORD dwOption = ::SymGetOptions();
	::SymSetOptions(dwOption | SYMOPT_LOAD_LINES);

	m_InitSymbolEngine = ::SymInitialize(::GetCurrentProcess(), NULL, TRUE);
	DN_ASSERT(TRUE == m_InitSymbolEngine, "@err, hr");
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CExceptionReport::ReadProcessMemoryWork
//*---------------------------------------------------------------
// DESC : 프로세스 메모리의 특정 번지에서 데이터를 읽어들임
// PARM :	1 . hModule - 모듈핸들
//			2 . dw64BaseAddr - 읽어들일 프로세스 메모리
//			3 . lpBuffer - 읽은 메모리 데이터를 저장할 버퍼
//			4 . dwBufferSize - 버퍼의 크기
//			5 . lpReadBytesSize - 읽은 데이터의 크기
// RETV : TRUE - 성공 / FALSE - 실패
// PRGM : B4nFter
//*---------------------------------------------------------------
BOOL __stdcall CExceptionReport::ReadProcessMemoryWork(HANDLE /*hModule*/, DWORD64 dw64BaseAddr, LPVOID lpBuffer, DWORD dwBufferSize, LPDWORD lpReadBytesSize)
{
	return(::ReadProcessMemory(::GetCurrentProcess(), (LPCVOID)dw64BaseAddr, lpBuffer, dwBufferSize, (SIZE_T*)lpReadBytesSize));	
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CExceptionReport::GetFaultReason
//*---------------------------------------------------------------
// DESC : 실패사유 정보를 얻어옴
// PARM :	1 . lpszPrefix - 선행정보 문자열
// RETV : 결과문자열
// PRGM : B4nFter
//*---------------------------------------------------------------
#pragma warning(disable:4995 4996)
const LPTSTR CExceptionReport::GetFaultReason(LPCTSTR lpszPrefix)
{
	DN_ASSERT(NULL != lpszPrefix,	"Invalid!");
	DN_READ(m_lpstExceptionPointers, sizeof(EXCEPTION_POINTERS*));

	const TCHAR* szResult = NULL;
	__try
	{
		InitSymbolEngine();

		INT nOffset = 0;
		nOffset += ::wsprintf(m_szText,	_T("Exception(%s) : "), lpszPrefix);
		nOffset += ::GetModuleBaseName(::GetCurrentProcess(), NULL, m_szText+nOffset, COUNT_OF(m_szText));
		nOffset += ::wsprintf(m_szText+nOffset,	_T(" caused an "));

		m_szExceptionDesc[0] = _T('\0');
		CMiniDumpSet::GetExceptionCause(m_lpstExceptionPointers->ExceptionRecord->ExceptionCode, m_szExceptionDesc, COUNT_OF(m_szExceptionDesc));
		if (_T('\0') != m_szExceptionDesc[0]) {
			nOffset += ::wsprintf(m_szText+nOffset, _T("%s (ThdId:%u)"), m_szExceptionDesc, ::GetCurrentThreadId());
		}
		else {
			nOffset += ::FormatMessage(FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_HMODULE,
									 ::GetModuleHandle(_T("NTDLL.DLL")),
									 m_lpstExceptionPointers->ExceptionRecord->ExceptionCode,
									 0, m_szText+nOffset, COUNT_OF(m_szText), 0);
		}
		DN_ASSERT(nOffset < (COUNT_OF(m_szText)-MAX_PATH), "error!");

		nOffset += ::wsprintf(m_szText+nOffset, _T(" in module "));
		DWORD64 lpStrPtr = ::SymGetModuleBase64(::GetCurrentProcess(),
									(DWORD64)m_lpstExceptionPointers->ExceptionRecord->ExceptionAddress);
		DN_ASSERT(NULL != lpStrPtr, "error!");
		if (NULL == lpStrPtr) {
			nOffset += ::wsprintf(m_szText+nOffset, _T("<UNKNOWN>"));
		}
		else {
			nOffset += ::GetModuleBaseName(::GetCurrentProcess(), (HINSTANCE)lpStrPtr,
											m_szText+nOffset, COUNT_OF(m_szText)-nOffset);
		}

		// 출력내용이 많고, 별의미가 없다.
		#if (0)
		#if defined(_WIN64)
		nOffset += ::wsprintf(m_szText+nOffset, _T(" at %016X"),
							m_lpstExceptionPointers->ExceptionRecord->ExceptionAddress);
		#else	// _WIN64
		nOffset += ::wsprintf(m_szText+nOffset, _T(" at %04X:%08X"),
							m_lpstExceptionPointers->ContextRecord->SegCs,
							m_lpstExceptionPointers->ExceptionRecord->ExceptionAddress);
		#endif	// _WIN64
		DN_ASSERT(nOffset < (COUNT_OF(m_szText)-200), "error!");
		#endif

		// Start looking up the exception address.
		PIMAGEHLP_SYMBOL64 lpSymbol = (PIMAGEHLP_SYMBOL64)&m_btSymbol;
		::memset(lpSymbol, 0, sizeof(m_btSymbol));
		lpSymbol->SizeOfStruct	= sizeof(IMAGEHLP_SYMBOL64);
		lpSymbol->MaxNameLength = sizeof(m_btSymbol) - sizeof(IMAGEHLP_SYMBOL64);

		DWORD64 n64LineDisp;
		if (TRUE == ::SymGetSymFromAddr64(::GetCurrentProcess(),
										 (DWORD64)m_lpstExceptionPointers->ExceptionRecord->ExceptionAddress,
										 &n64LineDisp, lpSymbol))
		{
			USES_CONVERSION;

			INT nStrLen = ::lstrlenA(lpSymbol->Name);
			// Make sure there's enough room for the longest symbol
			// and the displacement.
			if (nStrLen > (INT)((COUNT_OF(m_szText) - nOffset) - (COUNT_OF(m_btSymbol) + 50))) {
				::lstrcpyn(m_szText+nOffset, A2T(lpSymbol->Name), COUNT_OF(m_szText) - nOffset - 1);
				// Gotta leave now
				szResult = m_szText;
				__leave;
			}
			else {
				// 함수명만으로도 충분하다.
				#if (0)
				if (n64LineDisp > 0) {
					nOffset += ::wsprintf(m_szText+nOffset, _T(" %s()+%04d Byte(s)"), A2T(lpSymbol->Name), n64LineDisp);
				}
				else {
					nOffset += ::wsprintf(m_szText+nOffset, _T(" %s"), A2T(lpSymbol->Name));
				}
				#else
				nOffset += ::wsprintf(m_szText+nOffset, _T(" %s()"), A2T(lpSymbol->Name));
				#endif
			}
		}
		else {
			// If the symbol wasn't found, the source and line won't be found either, so leave now.
			szResult = m_szText;
			__leave;
		}

		DN_ASSERT(nOffset < (COUNT_OF(m_szText)-200), "error!");

		// Look up the source file and line number.
		::memset(&m_Line , 0, sizeof(IMAGEHLP_LINE64));
		m_Line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
        
		DWORD nLineDisp;
		if (TRUE == ::SymGetLineFromAddr64(::GetCurrentProcess(),
										  (DWORD64)m_lpstExceptionPointers->ExceptionRecord->ExceptionAddress ,
										  &nLineDisp, &m_Line))
		{
			USES_CONVERSION;

			nOffset += ::wsprintf(m_szText+nOffset, _T(","));

			// Copy no more of the source file and line number
			// information than there's room for.
			INT nStrLen = ::lstrlenA(m_Line.FileName);
			if (nStrLen > (INT)(COUNT_OF(m_szText) - nOffset - MAX_PATH - 50)) {
				::lstrcpyn(m_szText+nOffset, A2T(m_Line.FileName), COUNT_OF(m_szText)-nOffset-1);
				// Gotta leave now
				szResult = m_szText;
				__leave;
			}
			else {
				// full-path가 아닌 file명과 line번호만 출력한다.
				#if (0)
				if (nLineDisp > 0) {
					nOffset += ::wsprintf(m_szText+nOffset, _T(" %s, line %04d+%04d Byte(s)"),
										A2T(m_Line.FileName), m_Line.LineNumber, nLineDisp);
				}
				else {
					nOffset += ::wsprintf(m_szText+nOffset, _T(" %s, line %04d"), 
										A2T(m_Line.FileName), m_Line.LineNumber);
				}
				#else
				INT nLocalStrLen = ::lstrlenA(m_Line.FileName);
				for (--nLocalStrLen ; 0 <= nLocalStrLen ; --nLocalStrLen) {
					if (('\\' == m_Line.FileName[nLocalStrLen]) || ('/' == m_Line.FileName[nLocalStrLen])) {
						break;
					}
				}
				nOffset += ::wsprintf(m_szText+nOffset, _T(" Source(%s:%d)"),
									A2T(&m_Line.FileName[nLocalStrLen+1]), m_Line.LineNumber);
				#endif
			}
		}
		szResult = m_szText;
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		DN_ASSERT(0, "Crashed in CExceptionReport::GetFaultReason()");
		szResult = NULL;
	}

	return(const_cast<LPTSTR>(szResult));
}
#pragma warning(default:4995 4996)


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CExceptionReport::GetStackTraceString
//*---------------------------------------------------------------
// DESC : 예외 발생 시 저장된 EXCEPTION_POINTERS 포인터에서 호출스택 추적정보를 읽어서 처리함 
// PARM :	1 . lpszPrefix - 선행정보 문자열
// RETV : 결과문자열
// PRGM : B4nFter
//*---------------------------------------------------------------
#pragma warning(disable:4995 4996)
const LPTSTR CExceptionReport::GetStackTraceString(LPCTSTR lpszPrefix)
{
	DN_ASSERT(NULL != lpszPrefix,	"Invalid!");

	// The value that is returned
	const TCHAR* szResult = NULL;

	// The module base address. I look this up right after the stack
	// walk to ensure that the module is valid.
	DWORD64 dwModBase;
	__try
	{
		// Initialize the symbol engine in case it isn't initialized.
		InitSymbolEngine();

		// Note:  If the source file and line number functions are used,
		//        StackWalk can cause an access violation.
		BOOL bRetVal = ::StackWalk64(m_MachineType, ::GetCurrentProcess(), ::GetCurrentThread(),
								   &m_StackFame, &m_stContext,
								   ReadProcessMemoryWork, SymFunctionTableAccess64, SymGetModuleBase64,
								   NULL);
		if ((FALSE == bRetVal) || (0 == m_StackFame.AddrFrame.Offset)) {
			szResult = NULL;
			__leave;
		}

		// Before I get too carried away and start calculating
		// everything, I need to double-check that the address returned
		// by StackWalk really exists. I've seen cases in which
		// StackWalk returns TRUE but the address doesn't belong to
		// a module in the process.
		dwModBase = ::SymGetModuleBase64(::GetCurrentProcess(), m_StackFame.AddrPC.Offset);
		if (0 == dwModBase) {
			szResult = NULL;
			__leave;
		}

		INT nOffset = 0;

		nOffset += ::wsprintf(m_szText,	_T("CallStack(%s) : "), lpszPrefix);

		// 출력내용만 많고, 사용방법이 확실하지 않기에 일단 막음
		#if (0)
		// At a minimum, put in the address.
		#ifdef _WIN64
		nOffset += ::wsprintf(m_szText+nOffset, _T("0x%016X"), m_StackFame.AddrPC.Offset);
		#else
		nOffset += ::wsprintf(m_szText+nOffset, _T("%04X:%08X"), m_stContext.SegCs, m_StackFame.AddrPC.Offset);
		#endif

		// Output the parameters.
		nOffset += ::wsprintf(m_szText+nOffset, 
							#ifdef _WIN64
								_T(" (0x%016X 0x%016X 0x%016X 0x%016X)"),
							#else	// _WIN64
								_T(" (0x%08X 0x%08X 0x%08X 0x%08X)"),
							#endif	// _WIN64
								m_StackFame.Params[0], m_StackFame.Params[1],
								m_StackFame.Params[2], m_StackFame.Params[3]);
		#endif

		// Output the module name.
		DN_ASSERT(nOffset < (COUNT_OF(m_szText) - MAX_PATH), "error!");
		//nOffset += ::wsprintf(m_szText+nOffset, _T(" "));	// 앞의 기능이 OFF 됐으므로 막음
		nOffset += ::GetModuleBaseName(::GetCurrentProcess(), (HINSTANCE)dwModBase,
									  m_szText+nOffset, COUNT_OF(m_szText)-nOffset);

		// Output the symbol name?
		DN_ASSERT(nOffset < (COUNT_OF(m_szText) - MAX_PATH), "error!");
		DWORD64 n64LineDisp;
		// Start looking up the exception address.
		PIMAGEHLP_SYMBOL64 lpSymbol = (PIMAGEHLP_SYMBOL64)&m_btSymbol;
		::memset(lpSymbol, 0, sizeof(m_btSymbol));
		lpSymbol->SizeOfStruct	= sizeof(IMAGEHLP_SYMBOL64);
		lpSymbol->MaxNameLength = COUNT_OF(m_btSymbol) - sizeof(IMAGEHLP_SYMBOL64);
		lpSymbol->Address		= m_StackFame.AddrPC.Offset;
		if (TRUE == ::SymGetSymFromAddr64(::GetCurrentProcess(), m_StackFame.AddrPC.Offset, &n64LineDisp, lpSymbol))
		{
			USES_CONVERSION;

			// Copy no more symbol information than there's room
			// for.  Symbols are ANSI
			INT nStrLen = ::lstrlenA(lpSymbol->Name);
			if (nStrLen > (INT)(COUNT_OF(m_szText) - nOffset - (COUNT_OF(m_btSymbol) + 50)))
			{
				::lstrcpyn(m_szText+nOffset, A2T(lpSymbol->Name), COUNT_OF(m_szText) - nOffset - 1);
				// Gotta leave now
				szResult = m_szText;
				__leave;
			}
			else
			{
				// 밑에서 소스명과 line번호를 출력하므로 이곳에서는 이름만 출력
				#if (1)
//				#if (0)
				if (n64LineDisp > 0)
				{
					nOffset += ::wsprintf(m_szText+nOffset, _T(" %s()+%04d Byte(s)"),
										A2T(lpSymbol->Name), n64LineDisp);
				}
				else
				{
					nOffset += ::wsprintf(m_szText+nOffset, _T(" %s "), A2T(lpSymbol->Name));
				}
				#else
				nOffset += ::wsprintf(m_szText+nOffset, _T(" Function(%s) "), A2T(lpSymbol->Name));
				#endif
			}
		}
		else
		{
			// If the symbol wasn't found, the source file and line
			// number won't be found either, so leave now.
			szResult = m_szText;
			__leave;
		}

		// Output the source file and line number information?
		DN_ASSERT(nOffset < (COUNT_OF(m_szText) - MAX_PATH), "error!");
		::memset(&m_Line, 0, sizeof(IMAGEHLP_LINE64));
		m_Line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);

		DWORD nLineDisp;
		if (TRUE == ::SymGetLineFromAddr64(::GetCurrentProcess(), m_StackFame.AddrPC.Offset,
											&nLineDisp, &m_Line))
		{
			USES_CONVERSION;

			// m_Line.FileName은 full-path이므로 그냥 file명과 line번호만 출력
			#if (0)
			// Copy no more of the source file and line number
			// information than there's room for.
			INT nStrLen = ::lstrlenA(m_Line.FileName);
			if (nStrLen > (INT)(COUNT_OF(m_szText) - nOffset -	(MAX_PATH + 50))) {
				::lstrcpynA(m_szText+nOffset, m_Line.FileName, COUNT_OF(m_szText) - nOffset - 1);

				// Gotta leave now
				szResult = m_szText;
				__leave;
			}
			else {
				if (nLineDisp > 0) {
					nOffset += ::wsprintf(m_szText+nOffset, _T(" %s, line %04d+%04d Byte(s)"),
										A2T(m_Line.FileName), m_Line.LineNumber, nLineDisp);
				}
				else {
					nOffset += ::wsprintf(m_szText+nOffset, _T(" %s, line %04d"),
										A2T(m_Line.FileName), m_Line.LineNumber);
				}
			}
			#else
			int nStrLen = ::lstrlenA(m_Line.FileName);
			for (--nStrLen ; 0 <= nStrLen ; --nStrLen) {
				if (('\\' == m_Line.FileName[nStrLen]) || ('/' == m_Line.FileName[nStrLen])) {
					break;
				}
			}
			nOffset += ::wsprintf(m_szText+nOffset, _T(", Source(%s:%d)"),
								A2T(&m_Line.FileName[nStrLen+1]), m_Line.LineNumber);
			#endif
		}

		szResult = m_szText;
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		DN_ASSERT(0, "error!");
		szResult = NULL;
	}

	return(const_cast<LPTSTR>(szResult));
}
#pragma warning(default:4995 4996)


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CExceptionReport::GetFirstStackTraceString
//*---------------------------------------------------------------
// DESC : 호출스택 추적정보 얻기 시작
// PARM :	1 . lpszPrefix - 선행정보 문자열
// RETV : 결과문자열
// PRGM : B4nFter
//*---------------------------------------------------------------
const LPTSTR CExceptionReport::GetFirstStackTraceString(LPCTSTR lpszPrefix)
{
	DN_ASSERT(NULL != lpszPrefix,	"Invalid!");
	DN_READ(m_lpstExceptionPointers, sizeof(EXCEPTION_POINTERS*));

	{
		::memset(&m_StackFame, 0, sizeof(m_StackFame));

#ifdef _X86_
		m_StackFame.AddrPC.Offset       = m_lpstExceptionPointers->ContextRecord->Eip;
		m_StackFame.AddrPC.Mode         = AddrModeFlat;
		m_StackFame.AddrStack.Offset    = m_lpstExceptionPointers->ContextRecord->Esp;
		m_StackFame.AddrStack.Mode      = AddrModeFlat;
		m_StackFame.AddrFrame.Offset    = m_lpstExceptionPointers->ContextRecord->Ebp;
		m_StackFame.AddrFrame.Mode      = AddrModeFlat;
#elif  defined(_AMD64_)
		m_StackFame.AddrPC.Offset       = m_lpstExceptionPointers->ContextRecord->Rip;
		m_StackFame.AddrPC.Mode         = AddrModeFlat;
		m_StackFame.AddrStack.Offset    = m_lpstExceptionPointers->ContextRecord->Rsp;
		m_StackFame.AddrStack.Mode      = AddrModeFlat;
		m_StackFame.AddrFrame.Offset    = m_lpstExceptionPointers->ContextRecord->Rbp;
		m_StackFame.AddrFrame.Mode      = AddrModeFlat;
#elif  defined(_IA64_)
		#pragma message("IA64 NOT DEFINED!!")
		#pragma FORCE COMPILATION ABORT!
#else
		#pragma message("CPU NOT DEFINED!!")
		#pragma FORCE COMPILATION ABORT!
#endif
	}

	m_stContext = *(m_lpstExceptionPointers->ContextRecord);

	return(GetStackTraceString(lpszPrefix));
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CExceptionReport::GetNextStackTraceString
//*---------------------------------------------------------------
// DESC : 호출스택 추적정보 얻기 진행
// PARM :	1 . lpszPrefix - 선행정보 문자열
// RETV : 결과문자열
// PRGM : B4nFter
//*---------------------------------------------------------------
const LPTSTR CExceptionReport::GetNextStackTraceString(LPCTSTR lpszPrefix)
{
	DN_ASSERT(NULL != lpszPrefix,	"Invalid!");

	return(GetStackTraceString(lpszPrefix));
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CExceptionReport::GetRegisterString
//*---------------------------------------------------------------
// DESC : 예외 발생 시 저장된 EXCEPTION_POINTERS 포인터에서 레지스터 정보를 얻어옴
// PARM : N/A
// RETV : 결과문자열
// PRGM : B4nFter
//*---------------------------------------------------------------
#pragma warning(disable:4995)
const LPTSTR CExceptionReport::GetRegisterString()
{
	DN_READ(m_lpstExceptionPointers, sizeof(EXCEPTION_POINTERS*));

#if defined(_X86_)
    // This call puts 48 bytes on the stack, which could be a problem if
    // the stack is blown.
	::wsprintf(m_szText ,
         _T ("EAX=%08X  EBX=%08X  ECX=%08X  EDX=%08X  ESI=%08X\r\n")\
         _T ("EDI=%08X  EBP=%08X  ESP=%08X  EIP=%08X  FLG=%08X\r\n")\
         _T ("CS=%04X  DS=%04X  SS=%04X  ES=%04X   ")\
         _T ("FS=%04X  GS=%04X" ) ,
         m_lpstExceptionPointers->ContextRecord->Eax      ,
         m_lpstExceptionPointers->ContextRecord->Ebx      ,
         m_lpstExceptionPointers->ContextRecord->Ecx      ,
         m_lpstExceptionPointers->ContextRecord->Edx      ,
         m_lpstExceptionPointers->ContextRecord->Esi      ,
         m_lpstExceptionPointers->ContextRecord->Edi      ,
         m_lpstExceptionPointers->ContextRecord->Ebp      ,
         m_lpstExceptionPointers->ContextRecord->Esp      ,
         m_lpstExceptionPointers->ContextRecord->Eip      ,
         m_lpstExceptionPointers->ContextRecord->EFlags   ,
         m_lpstExceptionPointers->ContextRecord->SegCs    ,
         m_lpstExceptionPointers->ContextRecord->SegDs    ,
         m_lpstExceptionPointers->ContextRecord->SegSs    ,
         m_lpstExceptionPointers->ContextRecord->SegEs    ,
         m_lpstExceptionPointers->ContextRecord->SegFs    ,
         m_lpstExceptionPointers->ContextRecord->SegGs     ) ;
#elif defined(_AMD64_)
    ::wsprintf(m_szText ,
        _T ("RAX=%016X  RBX=%016X  RCX=%016X  RDX=%016X  RSI=%016X\r\n")\
        _T ("RDI=%016X  RBP=%016X  RSP=%016X  RIP=%016X  FLG=%016X\r\n")\
        _T (" R8=%016X   R9=%016X  R10=%016X  R11=%016X  R12=%016X\r\n")\
        _T ("R13=%016X  R14=%016X  R15=%016X" ) ,
        m_lpstExceptionPointers->ContextRecord->Rax      ,
        m_lpstExceptionPointers->ContextRecord->Rbx      ,
        m_lpstExceptionPointers->ContextRecord->Rcx      ,
        m_lpstExceptionPointers->ContextRecord->Rdx      ,
        m_lpstExceptionPointers->ContextRecord->Rsi      ,
        m_lpstExceptionPointers->ContextRecord->Rdi      ,
        m_lpstExceptionPointers->ContextRecord->Rbp      ,
        m_lpstExceptionPointers->ContextRecord->Rsp      ,
        m_lpstExceptionPointers->ContextRecord->Rip      ,
        m_lpstExceptionPointers->ContextRecord->EFlags   ,
        m_lpstExceptionPointers->ContextRecord->R8       ,
        m_lpstExceptionPointers->ContextRecord->R9       ,
        m_lpstExceptionPointers->ContextRecord->R10      ,
        m_lpstExceptionPointers->ContextRecord->R11      ,
        m_lpstExceptionPointers->ContextRecord->R12      ,
        m_lpstExceptionPointers->ContextRecord->R13      ,
        m_lpstExceptionPointers->ContextRecord->R14      ,
        m_lpstExceptionPointers->ContextRecord->R15       ) ;
#elif defined(_IA64_)
    #pragma message("IA64 NOT DEFINED!!")
    #pragma FORCE COMPILATION ABORT!
#else	// !_X86 && !_AMD64_ && !_IA64_
    #pragma message("CPU NOT DEFINED!!")
    #pragma FORCE COMPILATION ABORT!
#endif	// !_X86 && !_AMD64_ && !_IA64_

    return m_szText;
}
#pragma warning(default:4995)


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CExceptionReport::Write
//*---------------------------------------------------------------
// DESC : 기록이 필요한 텍스트를 특정한 방법으로 저장
// PARM :	1 . lpszText - 저장할 문자열
//			2 . eWriteFlag - 저장 플래그
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
VOID CExceptionReport::Write(LPCTSTR lpszText, EF_WRITEFLAGE eWriteFlag)
{
	DN_ASSERT(NULL != lpszText,	"Invalid!");

	ULONG uTextSize = (ULONG)::_tcslen(lpszText);

	if (eWriteFlag | EV_WF_FILE) {
		ULONG uIoBytes = uTextSize * (ULONG)sizeof(TCHAR);
		m_File.Write(lpszText, uIoBytes);
	}
	if (eWriteFlag | EV_WF_LOGPROC &&
		m_lpfnOutputLogProc
		)
	{
		m_lpfnOutputLogProc(lpszText, uTextSize, m_lpOutputLogProcParam);
	}	
	::OutputDebugString(lpszText);	// !!! 임시
}

const LPTSTR CExceptionReport::GetUserMemoryString()
{
	::memset(m_szText, 0, sizeof(m_szText));
	::wsprintf(m_szText, _T("[ACC:%d][CHRID:%I64d][MainCmd:%d][SubCmd:%d]"), m_nAccountID, m_biCharacterDBID, m_nMainCmd, m_nSubCmd);

	return m_szText;
}

void CExceptionReport::SetValues(UINT nAccountID, INT64 biCharacterDBID, int nMainCmd, int nSubCmd)
{
	m_nAccountID = nAccountID;
	m_biCharacterDBID = biCharacterDBID;
	m_nMainCmd = nMainCmd;
	m_nSubCmd = nSubCmd;
}

UINT CExceptionReport::m_nAccountID = 0;
INT64 CExceptionReport::m_biCharacterDBID = 0;
int CExceptionReport::m_nMainCmd = 0;
int CExceptionReport::m_nSubCmd = 0;

