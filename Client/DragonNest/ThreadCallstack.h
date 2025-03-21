/*//===============================================================

	<< TREAD CALLSTACK >>

	PRGM : B4nFter

	FILE : TREADCALLSTACK.H, TREADCALLSTACK.CPP
	DESC : 스레드 호출스택 로그 클래스
	INIT BUILT DATE : 2009. 09. 25
	LAST BUILT DATE : 2009. 09. 25

	P.S.> 
		- 본 클래스는 Debuggin Application for Windows.NET 의 BugSlayer 의 CrashHandler 를 기반으로 제작되었음을 알립니다.
		- 본 클래스 자체는 Thread-Safe 하며 다만 사용자 로그출력 방식의 Thread-Safe 여부는 별개입니다.
		- 본 클래스 객체로 로그 기록 시 프로그램 데이터베이스 (.pdb) 파일이 같은 폴더 내에 있으면 객체 초기화 (...::Open()) 시 심볼정보를 로드하여 소스 파일명과 행 위치가 함께 기록 됩니다.
		- Release (X86/X64) 모드 컴파일 시 코드 최적화로 인해 스레드 호출스택 정보가 제대로 남지 않으므로 정확한 데이터가 필요하다면 최적화 옵션을 조절하시기 바랍니다. (프로젝트 속성 → C/C++ → Optimization → Optimization → Disabled '/Od')

	Copyrightⓒ 2002~2009 B4nFter (b4nfter@gmail.com) and Co-Writers. All Rights Reserved.

*///===============================================================


#pragma once


///////////////////////////////////////////////////////////////////
//	INCLUDE
///////////////////////////////////////////////////////////////////

#include <WINDOWS.H>
#include <TCHAR.H>
#include <DBGHELP.H>
#include <PSAPI.H>
#define	_CONVERSION_DONT_USE_THREAD_LOCALE	// ATL 7.0 부터 생긴 각 스레드별 코드페이지를 얻어오지 않고 기존 ATL 3.0 처럼 CP_THREAD_ACP 매크로 사용
#include <ATLCONV.H>

#pragma comment (lib, "DBGHELP.LIB")
#pragma comment(lib, "PSAPI.LIB")


///////////////////////////////////////////////////////////////////
//	DEFINE
///////////////////////////////////////////////////////////////////

#define DF_THREADCALLSTACK_TEXTBUFFERSIZE		(2048)
#define DF_THREADCALLSTACK_MAXSYMBOLSIZE		(512)


///////////////////////////////////////////////////////////////////
//	DECLARATION
///////////////////////////////////////////////////////////////////

//*---------------------------------------------------------------
// TYPE : CLASS
// NAME : CThreadCallstack
//*---------------------------------------------------------------
// DESC : 스레드 호출스택 로그 클래스
// PRGM : B4nFter
//*---------------------------------------------------------------
class CThreadCallstack
{

public:
	typedef VOID (*LPFN_LOGPROC)(LPCSTR /*텍스트버퍼*/, UINT /*텍스트버퍼크기(TCHAR기준)*/, LPVOID /*사용자파라메터*/);		// 외부 로그출력 함수로 호출스택 출력결과를 보내기 위한 함수포인터 선언
	typedef VOID (WINAPI *LPFN_RTLCAPTURECONTEXT)(PCONTEXT /*컨텍스트*/);

private:
// 	DISABLE_COPY_ASSIGN(CThreadCallstack);	// 객체간 복사, 대입 금지

//private:
public:
	CThreadCallstack();
	~CThreadCallstack();

 	DWORD Open(LPFN_LOGPROC pLogProc = NULL);
	VOID Close();
	BOOL IsOpen() const { return(m_InitSymbolEngine); }

 	DWORD Write(LPFN_LOGPROC pLogProc = NULL, LPVOID pParam = NULL, LPCSTR pPrefix = NULL, SIZE_T pPrefixSize = 0);

private:
	VOID Reset();

	const LPCSTR GetStackTraceString(PCONTEXT pContext, PIMAGEHLP_LINE64 pLine, LPSTACKFRAME64 pStackFrame, LPCSTR pPrefix, SIZE_T pPrefixSize, LPSTR pText, SIZE_T pTextSize, PBYTE pSymbol, SIZE_T pSymbolSize);

private:
	static BOOL __stdcall ReadProcessMemoryWork(HANDLE hModule, DWORD64 dw64BaseAddr, LPVOID lpBuffer, DWORD dwBufferSize, LPDWORD lpReadBytesSize);

private:
 	DWORD m_MachineType;
 	BOOL m_InitSymbolEngine;
	LPFN_LOGPROC m_LogProc;
	LPFN_RTLCAPTURECONTEXT m_RtlCaptureContext;

};

