

///////////////////////////////////////////////////////////////////
//	INCLUDE
///////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "THREADCALLSTACK.H"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

///////////////////////////////////////////////////////////////////
//	IMPLEMENTATION
///////////////////////////////////////////////////////////////////


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CThreadCallstack::CThreadCallstack
//*---------------------------------------------------------------
// DESC : 생성자
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
CThreadCallstack::CThreadCallstack()
{
#if defined(_X86_)
	m_MachineType	= IMAGE_FILE_MACHINE_I386;
#elif defined(_AMD64_)
	m_MachineType	= IMAGE_FILE_MACHINE_AMD64;
#elif defined(_IA64_)
	m_MachineType	= IMAGE_FILE_MACHINE_IA64;
#else	// !_X86 !_AMD64_ !_IA64_
	#pragma FORCE COMPILE ABORT!
#endif	// !_X86 !_AMD64_ !_IA64_

	Reset();
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CThreadCallstack::~CThreadCallstack
//*---------------------------------------------------------------
// DESC : 소멸자
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
CThreadCallstack::~CThreadCallstack()
{
	
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CThreadCallstack::Open
//*---------------------------------------------------------------
// DESC : 객체자원 초기화
// PARM :	1 . pLogProc - 로그출력 함수 포인터
// RETV : NOERROR - 성공 / 그외 - 실패
// PRGM : B4nFter
//*---------------------------------------------------------------
DWORD CThreadCallstack::Open(LPFN_LOGPROC pLogProc)
{
//	BASE_ASSERT(!IsOpen(),	"Already Opened!");

	m_LogProc = pLogProc;

	// ::RtlCaptureContext 지원여부 확인
	{
		m_RtlCaptureContext = (LPFN_RTLCAPTURECONTEXT)::GetProcAddress(GetModuleHandleW(L"kernel32.dll"), "RtlCaptureContext");
		if(!m_RtlCaptureContext) {
			return 1;
//			BASE_RETURN(HASERROR+0);
		}
	}

	// 심볼엔진 초기화
	{
		DWORD dwOption = ::SymGetOptions();
		::SymSetOptions(dwOption | SYMOPT_LOAD_LINES);

		m_InitSymbolEngine = ::SymInitialize(::GetCurrentProcess(), NULL, TRUE);
		if (!m_InitSymbolEngine) {
			return 2;
//			BASE_RETURN(HASERROR+1);
		}
	}

	return NOERROR;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CThreadCallstack::Close
//*---------------------------------------------------------------
// DESC : 객체자원 해제
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
VOID CThreadCallstack::Close()
{
	// 심볼엔진 자원 해제
	if (m_InitSymbolEngine) {
		::SymCleanup(::GetCurrentProcess());
	}

	Reset();
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CThreadCallstack::Reset
//*---------------------------------------------------------------
// DESC : 객체자원 초기화
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
VOID CThreadCallstack::Reset()
{
	m_InitSymbolEngine = FALSE;
	m_LogProc = NULL;
	m_RtlCaptureContext = NULL;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CThreadCallstack::Write
//*---------------------------------------------------------------
// DESC : 객체자원 초기화
// PARM :	1 . pLogProc - 로그출력 함수 포인터
//			2 . pParam - 사용자 파라메터
//			3 . pPrefix - 로그 접두사
//			4 . pPrefixSize - 로그 접두사 크기
// RETV : NOERROR - 성공 / 그외 - 실패
// PRGM : B4nFter
//*---------------------------------------------------------------
DWORD CThreadCallstack::Write(LPFN_LOGPROC pLogProc, LPVOID pParam, LPCSTR pPrefix, SIZE_T pPrefixSize)
{
	pLogProc, pParam, pPrefix, pPrefixSize;

	LPFN_LOGPROC aLogProc = (pLogProc)?(pLogProc):(m_LogProc);
	if (!aLogProc) {
		return 1;
//		BASE_RETURN(HASERROR+0);
	}

	CONTEXT aContext;
	IMAGEHLP_LINE64	aLine;
	STACKFRAME64 aStackFrame;
	CHAR aText[DF_THREADCALLSTACK_TEXTBUFFERSIZE+1] = { ('\0'), };
	BYTE aSymbol[DF_THREADCALLSTACK_MAXSYMBOLSIZE] = { 0, };

	m_RtlCaptureContext(&aContext);
	
	{
		::memset(&aStackFrame, 0, sizeof(STACKFRAME64));

#ifdef _X86_
		aStackFrame.AddrPC.Offset       = aContext.Eip;
		aStackFrame.AddrPC.Mode         = AddrModeFlat;
		aStackFrame.AddrStack.Offset    = aContext.Esp;
		aStackFrame.AddrStack.Mode      = AddrModeFlat;
		aStackFrame.AddrFrame.Offset    = aContext.Ebp;
		aStackFrame.AddrFrame.Mode      = AddrModeFlat;
#elif  defined(_AMD64_)
		aStackFrame.AddrPC.Offset       = aContext.Rip;
		aStackFrame.AddrPC.Mode         = AddrModeFlat;
		aStackFrame.AddrStack.Offset    = aContext.Rsp;
		aStackFrame.AddrStack.Mode      = AddrModeFlat;
		aStackFrame.AddrFrame.Offset    = aContext.Rbp;
		aStackFrame.AddrFrame.Mode      = AddrModeFlat;
#elif  defined(_IA64_)
		#pragma message("IA64 NOT DEFINED!!")
		#pragma FORCE COMPILATION ABORT!
#else
		#pragma message("CPU NOT DEFINED!!")
		#pragma FORCE COMPILATION ABORT!
#endif
	}

	LPCSTR aLogMsg = GetStackTraceString(&aContext, &aLine, &aStackFrame, pPrefix, pPrefixSize, aText, _countof(aText), aSymbol, sizeof(aSymbol));
	while(aLogMsg) {
		aLogMsg = GetStackTraceString(&aContext, &aLine, &aStackFrame, pPrefix, pPrefixSize, aText, _countof(aText), aSymbol, sizeof(aSymbol));
		if( strstr( aLogMsg, "DragonNest.exe operator new") ) continue;
		if( strstr( aLogMsg, "DragonNest.exe std::") ) continue;				
		aLogProc(aLogMsg, _countof(aText), pParam);
		break;
	}

	return NOERROR;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CThreadCallstack::ReadProcessMemoryWork
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
BOOL __stdcall CThreadCallstack::ReadProcessMemoryWork(HANDLE /*hModule*/, DWORD64 dw64BaseAddr, LPVOID lpBuffer, DWORD dwBufferSize, LPDWORD lpReadBytesSize)
{
	return(::ReadProcessMemory(::GetCurrentProcess(), (LPCVOID)dw64BaseAddr, lpBuffer, dwBufferSize, (SIZE_T*)lpReadBytesSize));	
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CThreadCallstack::GetStackTraceString
//*---------------------------------------------------------------
// DESC : 예외 발생 시 저장된 EXCEPTION_POINTERS 포인터에서 호출스택 추적정보를 읽어서 처리함 
// PARM :	1 . pContext - 컨텍스트 정보
//			2 . pLine - 소스파일 라인 정보
//			3 . pStackFrame - 스택 프레임 정보
//			4 . pPrefix - 로그 접두사
//			5 . pPrefixSize - 로그 접두사 크기
//			6 . pText - 로그 문자열
//			7 . pTextSize - 로그 문자열 크기
//			8 . pSymbol - 심볼 정보
//			9 . pSymbolSize - 심볼 정보 크기
// RETV : 결과 문자열
// PRGM : B4nFter
//*---------------------------------------------------------------
#pragma warning(disable:4995 4996)
const LPCSTR CThreadCallstack::GetStackTraceString(PCONTEXT pContext, PIMAGEHLP_LINE64 pLine, LPSTACKFRAME64 pStackFrame, LPCSTR pPrefix, SIZE_T pPrefixSize, LPSTR pText, SIZE_T pTextSize, PBYTE pSymbol, SIZE_T pSymbolSize)
{
// 	BASE_ASSERT(NULL != pContext,		"Invalid!");
// 	BASE_ASSERT(NULL != pLine,			"Invalid!");
// 	BASE_ASSERT(NULL != pStackFrame,	"Invalid!");
// 	BASE_ASSERT(NULL != pPrefix,		"Invalid!");
// 	BASE_ASSERT(0 < pPrefixSize,		"Invalid!");
// 	BASE_ASSERT(NULL != pText,			"Invalid!");
// 	BASE_ASSERT(0 < pTextSize,			"Invalid!");
// 	BASE_ASSERT(NULL != pSymbol,		"Invalid!");
// 	BASE_ASSERT(0 < pSymbolSize,		"Invalid!");

	// The value that is returned
	const CHAR* szResult = NULL;

	// The module base address. I look this up right after the stack
	// walk to ensure that the module is valid.
	DWORD64 dwModBase;
	__try
	{
		// Note:  If the source file and line number functions are used,
		//        StackWalk can cause an access violation.
		BOOL bRetVal = ::StackWalk64(m_MachineType, ::GetCurrentProcess(), ::GetCurrentThread(),
								   pStackFrame, pContext,
								   ReadProcessMemoryWork, SymFunctionTableAccess64, SymGetModuleBase64,
								   NULL);
		if ((FALSE == bRetVal) || (0 == pStackFrame->AddrFrame.Offset)) {
			szResult = NULL;
			__leave;
		}

		// Before I get too carried away and start calculating
		// everything, I need to double-check that the address returned
		// by StackWalk really exists. I've seen cases in which
		// StackWalk returns TRUE but the address doesn't belong to
		// a module in the process.
		dwModBase = ::SymGetModuleBase64(::GetCurrentProcess(), pStackFrame->AddrPC.Offset);
		if (0 == dwModBase) {
			szResult = NULL;
			__leave;
		}

		INT nOffset = 0;

		nOffset += ::sprintf(pText, "CallStack(%s) : ", (pPrefix)?(pPrefix):(""));

		// 출력내용만 많고, 사용방법이 확실하지 않기에 일단 막음
		#if (0)
		// At a minimum, put in the address.
		#ifdef _WIN64
		nOffset += ::wsprintf(pText+nOffset, _T("0x%016X"), pStackFrame->AddrPC.Offset);
		#else
		nOffset += ::wsprintf(pText+nOffset, _T("%04X:%08X"), pContext.SegCs, pStackFrame->AddrPC.Offset);
		#endif

		// Output the parameters.
		nOffset += ::wsprintf(pText+nOffset, 
							#ifdef _WIN64
								_T(" (0x%016X 0x%016X 0x%016X 0x%016X)"),
							#else	// _WIN64
								_T(" (0x%08X 0x%08X 0x%08X 0x%08X)"),
							#endif	// _WIN64
								pStackFrame->Params[0], pStackFrame->Params[1],
								pStackFrame->Params[2], pStackFrame->Params[3]);
		#endif

		// Output the module name.
//		BASE_ASSERT(nOffset < (pTextSize - MAX_PATH), "error!");
//		nOffset += ::wsprintf(pText+nOffset, _T(" "));	// 앞의 기능이 OFF 됐으므로 막음
		nOffset += ::GetModuleBaseNameA(::GetCurrentProcess(), (HINSTANCE)dwModBase, pText+nOffset, (DWORD)pTextSize-nOffset);

		// Output the symbol name?
//		BASE_ASSERT(nOffset < (pTextSize - MAX_PATH), "error!");
		DWORD64 n64LineDisp;
		// Start looking up the exception address.
		PIMAGEHLP_SYMBOL64 lpSymbol = (PIMAGEHLP_SYMBOL64)pSymbol;
		::memset(lpSymbol, 0, pSymbolSize);
		lpSymbol->SizeOfStruct	= sizeof(IMAGEHLP_SYMBOL64);
		lpSymbol->MaxNameLength = (DWORD)pSymbolSize - sizeof(IMAGEHLP_SYMBOL64);
		lpSymbol->Address		= pStackFrame->AddrPC.Offset;

		if (TRUE == ::SymGetSymFromAddr64(::GetCurrentProcess(), pStackFrame->AddrPC.Offset, &n64LineDisp, lpSymbol)) {
			USES_CONVERSION;

			// Copy no more symbol information than there's room
			// for.  Symbols are ANSI
			INT nStrLen = ::lstrlenA(lpSymbol->Name);
			if (nStrLen > (INT)(pTextSize - nOffset - (pSymbolSize + 50)))
			{
				::strncpy(pText+nOffset, lpSymbol->Name, (INT)pTextSize - nOffset - 1);
				// Gotta leave now
				szResult = pText;
				__leave;
			}
			else
			{
				// 밑에서 소스파일 이름과 행 번호를 출력하므로 이곳에서는 이름만 출력
				#if (1)
//				#if (0)
				/*if (n64LineDisp > 0)
				{
					nOffset += ::sprintf(pText+nOffset, " %s()+%04d Byte(s)", lpSymbol->Name, n64LineDisp);
				}
				else
				{*/
					nOffset += ::sprintf(pText+nOffset, "%s ", lpSymbol->Name);
				//}
				#else
				nOffset += ::wsprintf(pText+nOffset, _T(" Function(%s) "), A2T(lpSymbol->Name));
				#endif
			}
		}
		else
		{
			// If the symbol wasn't found, the source file and line
			// number won't be found either, so leave now.
			szResult = pText;
			__leave;
		}

		// Output the source file and line number information?
//		BASE_ASSERT(nOffset < (pTextSize - MAX_PATH), "error!");
		::memset(pLine, 0, sizeof(IMAGEHLP_LINE64));
		pLine->SizeOfStruct = sizeof(IMAGEHLP_LINE64);

		DWORD nLineDisp;
		if (TRUE == ::SymGetLineFromAddr64(::GetCurrentProcess(), pStackFrame->AddrPC.Offset, &nLineDisp, pLine)) {
			USES_CONVERSION;

			// pLine->FileName 은 full-path 이므로 그냥 소스파일 이름과 행 번호만 출력
			#if (0)
			// Copy no more of the source file and line number
			// information than there's room for.
			INT nStrLen = ::lstrlenA(pLine->FileName);
			if (nStrLen > (INT)(pTextSize - nOffset -	(MAX_PATH + 50))) {
				::lstrcpynA(pText+nOffset, pLine->FileName, pTextSize - nOffset - 1);

				// Gotta leave now
				szResult = pText;
				__leave;
			}
			else {
				if (nLineDisp > 0) {
					nOffset += ::wsprintf(pText+nOffset, _T(" %s, line %04d+%04d Byte(s)"), A2T(pLine->FileName), pLine->LineNumber, nLineDisp);
				}
				else {
					nOffset += ::wsprintf(pText+nOffset, _T(" %s, line %04d"), A2T(pLine->FileName), pLine->LineNumber);
				}
			}
			#else
			INT nStrLen = ::lstrlenA(pLine->FileName);
			for (--nStrLen ; 0 <= nStrLen ; --nStrLen) {
				if (('\\' == pLine->FileName[nStrLen]) || ('/' == pLine->FileName[nStrLen])) {
					break;
				}
			}
			nOffset += ::sprintf(pText+nOffset, ", Source(%s:%d)", &pLine->FileName[nStrLen+1], pLine->LineNumber);
			#endif
		}

		szResult = pText;
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
//		BASE_ASSERT(0, "error!");
		szResult = NULL;
	}

	return(const_cast<LPSTR>(szResult));
}
#pragma warning(default:4995 4996)

