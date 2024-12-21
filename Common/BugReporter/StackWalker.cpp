#include "stdafx.h"
#include <windows.h>
#include <assert.h>
#include <time.h>
#include <tchar.h>
#include <DbgHelp.h>
#include <string>
#include <iostream>

#include "StackWalker.h"
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif



const int MAX_SYM_SIZE = 512;
const int BUFF_SIZE = 8192;
const int SYM_BUFF_SIZE  = 1024;

// The stack frame used in walking the stack
static BOOL g_bSymEngInit = FALSE ;
static STACKFRAME64 g_stFrame;
static CONTEXT g_stContext;

// The static buffer returned by various functions. This buffer
// allows data to be transferred without using the stack.
static CHAR g_szBuff[BUFF_SIZE] = "";

// The static symbol lookup buffer
static BYTE g_stSymbol [ SYM_BUFF_SIZE ] ;

// The static source file and line number structure
static IMAGEHLP_LINE64 g_stLine ;

#define TRACE              __noop


/*//////////////////////////////////////////////////////////////////////
Typedefs
//////////////////////////////////////////////////////////////////////*/
// The typedefs for the PSAPI.DLL functions used by this module.
typedef BOOL (WINAPI *ENUMPROCESSMODULES) ( HANDLE    hProcess   ,
										   HMODULE * lphModule  ,
										   DWORD     cb         ,
										   LPDWORD   lpcbNeeded  ) ;

typedef DWORD (WINAPI *GETMODULEBASENAMEW) ( HANDLE  hProcess   ,
											HMODULE hModule    ,
											LPWSTR  lpBaseName ,
											DWORD   nSize       ) ;

typedef DWORD (WINAPI *GETMODULEFILENAMEEXW) ( HANDLE  hProcess   ,
											  HMODULE hModule    ,
											  LPWSTR  lpFilename ,
											  DWORD   nSize       ) ;


/*//////////////////////////////////////////////////////////////////////
File Static Data
//////////////////////////////////////////////////////////////////////*/
// Has the function stuff here been initialized?  This is only to be
// used by the InitPSAPI function and nothing else.
static BOOL g_bInitialized = FALSE ;
// The pointer to EnumProcessModules.
static ENUMPROCESSMODULES g_pEnumProcessModules = NULL ;
// The pointer to GetModuleBaseName.
static GETMODULEBASENAMEW g_pGetModuleBaseName = NULL ;
// The pointer to GetModuleFileNameEx.
static GETMODULEFILENAMEEXW g_pGetModuleFileNameEx = NULL ;



const char*
GetFaultReason(EXCEPTION_POINTERS* pExPtrs)
{
	if (::IsBadReadPtr(pExPtrs, sizeof(EXCEPTION_POINTERS))) 
		return ("bad exception pointers");

	// 간단한 에러 코드라면 그냥 변환할 수 있다.
	switch (pExPtrs->ExceptionRecord->ExceptionCode)
	{
	case EXCEPTION_ACCESS_VIOLATION:         return ("EXCEPTION_ACCESS_VIOLATION");
	case EXCEPTION_DATATYPE_MISALIGNMENT:    return ("EXCEPTION_DATATYPE_MISALIGNMENT");
	case EXCEPTION_BREAKPOINT:               return ("EXCEPTION_BREAKPOINT");
	case EXCEPTION_SINGLE_STEP:              return ("EXCEPTION_SINGLE_STEP");
	case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:    return ("EXCEPTION_ARRAY_BOUNDS_EXCEEDED");
	case EXCEPTION_FLT_DENORMAL_OPERAND:     return ("EXCEPTION_FLT_DENORMAL_OPERAND");
	case EXCEPTION_FLT_DIVIDE_BY_ZERO:       return ("EXCEPTION_FLT_DIVIDE_BY_ZERO");
	case EXCEPTION_FLT_INEXACT_RESULT:       return ("EXCEPTION_FLT_INEXACT_RESULT");
	case EXCEPTION_FLT_INVALID_OPERATION:    return ("EXCEPTION_FLT_INVALID_OPERATION");
	case EXCEPTION_FLT_OVERFLOW:             return ("EXCEPTION_FLT_OVERFLOW");
	case EXCEPTION_FLT_STACK_CHECK:          return ("EXCEPTION_FLT_STACK_CHECK");
	case EXCEPTION_FLT_UNDERFLOW:            return ("EXCEPTION_FLT_UNDERFLOW");
	case EXCEPTION_INT_DIVIDE_BY_ZERO:       return ("EXCEPTION_INT_DIVIDE_BY_ZERO");
	case EXCEPTION_INT_OVERFLOW:             return ("EXCEPTION_INT_OVERFLOW");
	case EXCEPTION_PRIV_INSTRUCTION:         return ("EXCEPTION_PRIV_INSTRUCTION");
	case EXCEPTION_IN_PAGE_ERROR:            return ("EXCEPTION_IN_PAGE_ERROR");
	case EXCEPTION_ILLEGAL_INSTRUCTION:      return ("EXCEPTION_ILLEGAL_INSTRUCTION");
	case EXCEPTION_NONCONTINUABLE_EXCEPTION: return ("EXCEPTION_NONCONTINUABLE_EXCEPTION");
	case EXCEPTION_STACK_OVERFLOW:           return ("EXCEPTION_STACK_OVERFLOW");
	case EXCEPTION_INVALID_DISPOSITION:      return ("EXCEPTION_INVALID_DISPOSITION");
	case EXCEPTION_GUARD_PAGE:               return ("EXCEPTION_GUARD_PAGE");
	case EXCEPTION_INVALID_HANDLE:           return ("EXCEPTION_INVALID_HANDLE");
		//case EXCEPTION_POSSIBLE_DEADLOCK:        return ("EXCEPTION_POSSIBLE_DEADLOCK");
	case CONTROL_C_EXIT:                     return ("CONTROL_C_EXIT");
	case 0xE06D7363:                         return ("Microsoft C++ Exception");
	default:
		break;
	}

	// 뭔가 좀 더 복잡한 에러라면...
	static CHAR szFaultReason[2048] = "";
	::strcpy(szFaultReason, ("Unknown")); 
	::FormatMessageA(
		FORMAT_MESSAGE_FROM_HMODULE | FORMAT_MESSAGE_IGNORE_INSERTS,
		::GetModuleHandleA(("ntdll.dll")),
		pExPtrs->ExceptionRecord->ExceptionCode, 
		0,
		szFaultReason,
		0,
		NULL);

	return szFaultReason;
}


const char* 
GetRegisterString(EXCEPTION_POINTERS* pExPtrs)
{
	static CHAR szBuff[8192*4] = {0,};

#ifdef _X86_
	// This call puts 48 bytes on the stack, which could be a problem if
	// the stack is blown.
	sprintf(szBuff ,
		"EAX=%08X  EBX=%08X  ECX=%08X  EDX=%08X  ESI=%08X\n"\
		"EDI=%08X  EBP=%08X  ESP=%08X  EIP=%08X  FLG=%08X\n"\
		"CS=%04X   DS=%04X  SS=%04X  ES=%04X   "\
		"FS=%04X  GS=%04X" ,
		pExPtrs->ContextRecord->Eax      ,
		pExPtrs->ContextRecord->Ebx      ,
		pExPtrs->ContextRecord->Ecx      ,
		pExPtrs->ContextRecord->Edx      ,
		pExPtrs->ContextRecord->Esi      ,
		pExPtrs->ContextRecord->Edi      ,
		pExPtrs->ContextRecord->Ebp      ,
		pExPtrs->ContextRecord->Esp      ,
		pExPtrs->ContextRecord->Eip      ,
		pExPtrs->ContextRecord->EFlags   ,
		pExPtrs->ContextRecord->SegCs    ,
		pExPtrs->ContextRecord->SegDs    ,
		pExPtrs->ContextRecord->SegSs    ,
		pExPtrs->ContextRecord->SegEs    ,
		pExPtrs->ContextRecord->SegFs    ,
		pExPtrs->ContextRecord->SegGs     ) ;
#elif _AMD64_
	sprintf ( szBuff ,
		"RAX=%016X  RBX=%016X  RCX=%016X  RDX=%016X  RSI=%016X\n"\
		"RDI=%016X  RBP=%016X  RSP=%016X  RIP=%016X  FLG=%016X\n"\
		" R8=%016X   R9=%016X  R10=%016X  R11=%016X  R12=%016X\n"\
		"R13=%016X  R14=%016X  R15=%016X" ,
		pExPtrs->ContextRecord->Rax      ,
		pExPtrs->ContextRecord->Rbx      ,
		pExPtrs->ContextRecord->Rcx      ,
		pExPtrs->ContextRecord->Rdx      ,
		pExPtrs->ContextRecord->Rsi      ,
		pExPtrs->ContextRecord->Rdi      ,
		pExPtrs->ContextRecord->Rbp      ,
		pExPtrs->ContextRecord->Rsp      ,
		pExPtrs->ContextRecord->Rip      ,
		pExPtrs->ContextRecord->EFlags   ,
		pExPtrs->ContextRecord->R8       ,
		pExPtrs->ContextRecord->R9       ,
		pExPtrs->ContextRecord->R10      ,
		pExPtrs->ContextRecord->R11      ,
		pExPtrs->ContextRecord->R12      ,
		pExPtrs->ContextRecord->R13      ,
		pExPtrs->ContextRecord->R14      ,
		pExPtrs->ContextRecord->R15       ) ;
#elif _IA64_
#pragma message ( "IA64 NOT DEFINED!!" )
#pragma FORCE COMPILATION ABORT!
#else
#pragma message ( "CPU NOT DEFINED!!" )
#pragma FORCE COMPILATION ABORT!
#endif

	return ( szBuff ) ;

}



const char* __stdcall
GetFirstStackTraceString( DWORD dwOpts, EXCEPTION_POINTERS * pExPtrs )
{
	assert ( FALSE == IsBadReadPtr ( pExPtrs , sizeof(EXCEPTION_POINTERS*)) );

	if ( TRUE == IsBadReadPtr ( pExPtrs, sizeof(EXCEPTION_POINTERS*)) )
	{
		return ("GetFirstStackTraceString - invalid pExPtrs!\n");
	}


	// Get the stack frame filled in.
	FillInStackFrame ( pExPtrs->ContextRecord ) ;

	// Copy over the exception pointers fields so I don't corrupt the
	// real one.
	g_stContext = *(pExPtrs->ContextRecord) ;

	return ( InternalGetStackTraceString ( dwOpts ) ) ;
}


const char* __stdcall
GetNextStackTraceString( DWORD dwOpts, EXCEPTION_POINTERS* pExPtrs )
{
	// All error checking is in InternalGetStackTraceString.
	// Assume that GetFirstStackTraceString has already initialized the
	// stack frame information.
	return ( InternalGetStackTraceString ( dwOpts ) ) ;
}


// Helper function to isolate filling out the stack frame, which is CPU
// specific.
void FillInStackFrame ( PCONTEXT pCtx )
{
	// Initialize the STACKFRAME structure.
	ZeroMemory ( &g_stFrame , sizeof ( STACKFRAME64 ) ) ;

#ifdef _X86_
	g_stFrame.AddrPC.Offset       = pCtx->Eip    ;
	g_stFrame.AddrPC.Mode         = AddrModeFlat ;
	g_stFrame.AddrStack.Offset    = pCtx->Esp    ;
	g_stFrame.AddrStack.Mode      = AddrModeFlat ;
	g_stFrame.AddrFrame.Offset    = pCtx->Ebp    ;
	g_stFrame.AddrFrame.Mode      = AddrModeFlat ;
#elif  _AMD64_
	g_stFrame.AddrPC.Offset       = pCtx->Rip    ;
	g_stFrame.AddrPC.Mode         = AddrModeFlat ;
	g_stFrame.AddrStack.Offset    = pCtx->Rsp    ;
	g_stFrame.AddrStack.Mode      = AddrModeFlat ;
	g_stFrame.AddrFrame.Offset    = pCtx->Rbp    ;
	g_stFrame.AddrFrame.Mode      = AddrModeFlat ;
#elif  _IA64_
#pragma message ( "IA64 NOT DEFINED!!" )
#pragma FORCE COMPILATION ABORT!
#else
#pragma message ( "CPU NOT DEFINED!!" )
#pragma FORCE COMPILATION ABORT!
#endif
}



// Initializes the symbol engine if needed
void InitSymEng ( void )
{
	if ( FALSE == g_bSymEngInit )
	{
		// Set up the symbol engine.
		DWORD dwOpts = SymGetOptions ( ) ;

		// Turn on line loading.
		SymSetOptions ( dwOpts                |
			SYMOPT_LOAD_LINES      ) ;

		// Force the invade process flag on.
		BOOL bRet = SymInitialize ( GetCurrentProcess ( ) ,
			NULL                    ,
			TRUE                     ) ;
		assert ( TRUE == bRet ) ;
		g_bSymEngInit = bRet ;
	}
}

// Cleans up the symbol engine if needed
void CleanupSymEng ( void )
{
	if ( TRUE == g_bSymEngInit )
	{
		assert ( SymCleanup ( GetCurrentProcess ( ) ) ) ;
		g_bSymEngInit = FALSE ;
	}
}


BOOL __stdcall CH_ReadProcessMemory ( HANDLE                           ,
									 DWORD64     qwBaseAddress        ,
									 PVOID       lpBuffer             ,
									 DWORD       nSize                ,
									 LPDWORD     lpNumberOfBytesRead  )
{
	return ( ReadProcessMemory ( GetCurrentProcess ( )  ,
		(LPCVOID)qwBaseAddress ,
		lpBuffer               ,
		nSize                  ,
		(SIZE_T*)lpNumberOfBytesRead     ) ) ;
}


// The internal function that does all the stack walking
const char* InternalGetStackTraceString ( DWORD dwOpts )
{

	// The value that is returned
	const char* szRet ;
	// The module base address. I look this up right after the stack
	// walk to ensure that the module is valid.
	DWORD64 dwModBase ;

	__try
	{
		// Initialize the symbol engine in case it isn't initialized.
		InitSymEng ( ) ;

		// Note:  If the source file and line number functions are used,
		//        StackWalk can cause an access violation.
		BOOL bSWRet = StackWalk64 ( CH_MACHINE                         ,
			GetCurrentProcess ( )              ,
			GetCurrentThread ( )               ,
			&g_stFrame                         ,
			&g_stContext                       ,
			CH_ReadProcessMemory               ,
			SymFunctionTableAccess64           ,
			SymGetModuleBase64                 ,
			NULL                               );
		if ( ( FALSE == bSWRet ) || ( 0 == g_stFrame.AddrFrame.Offset ))
		{
			szRet = NULL ;
			__leave ;
		}

		// Before I get too carried away and start calculating
		// everything, I need to double-check that the address returned
		// by StackWalk really exists. I've seen cases in which
		// StackWalk returns TRUE but the address doesn't belong to
		// a module in the process.
		dwModBase = SymGetModuleBase64 ( GetCurrentProcess ( )   ,
			g_stFrame.AddrPC.Offset  ) ;
		if ( 0 == dwModBase )
		{
			szRet = NULL ;
			__leave ;
		}

		int iCurr = 0 ;

		// At a minimum, put in the address.
#ifdef _WIN64
		iCurr += sprintf ( g_szBuff + iCurr        ,
			( "0x%016X" )         ,
			g_stFrame.AddrPC.Offset  ) ;
#else
		iCurr += sprintf ( g_szBuff + iCurr        ,
			( "%04X:%08X" )      ,
			g_stContext.SegCs       ,
			g_stFrame.AddrPC.Offset  ) ;
#endif

		// Output the parameters?
		if ( GSTSO_PARAMS == ( dwOpts & GSTSO_PARAMS ) )
		{
			iCurr += sprintf ( g_szBuff + iCurr          ,
				k_PARAMFMTSTRING          ,
				g_stFrame.Params[ 0 ]     ,
				g_stFrame.Params[ 1 ]     ,
				g_stFrame.Params[ 2 ]     ,
				g_stFrame.Params[ 3 ]      ) ;
		}
		// Output the module name.
		if ( GSTSO_MODULE == ( dwOpts & GSTSO_MODULE ) )
		{
			iCurr += sprintf ( g_szBuff + iCurr  ,  ( " " ) ) ;

			assert ( iCurr < ( BUFF_SIZE - MAX_PATH ) ) ;
			iCurr += BSUGetModuleBaseNameA ( GetCurrentProcess ( ) ,
				(HINSTANCE)dwModBase  ,
				g_szBuff + iCurr      ,
				BUFF_SIZE - iCurr      ) ;
		}

		assert ( iCurr < ( BUFF_SIZE - MAX_PATH ) ) ;
		DWORD64 dwDisp ;

		// Output the symbol name?
		if ( GSTSO_SYMBOL == ( dwOpts & GSTSO_SYMBOL ) )
		{

			// Start looking up the exception address.
			PIMAGEHLP_SYMBOL64 pSym = (PIMAGEHLP_SYMBOL64)&g_stSymbol ;
			ZeroMemory ( pSym , SYM_BUFF_SIZE ) ;
			pSym->SizeOfStruct = sizeof ( IMAGEHLP_SYMBOL64 ) ;
			pSym->MaxNameLength = SYM_BUFF_SIZE -
				sizeof ( IMAGEHLP_SYMBOL64 ) ;
			pSym->Address = g_stFrame.AddrPC.Offset ;

			if ( TRUE ==
				SymGetSymFromAddr64 ( GetCurrentProcess ( )      ,
				g_stFrame.AddrPC.Offset    ,
				&dwDisp                    ,
				pSym                        ) )
			{
				if ( dwOpts & ~GSTSO_SYMBOL )
				{
					iCurr += sprintf ( g_szBuff + iCurr , ( "," ));
				}

				// Copy no more symbol information than there's room
				// for.  Symbols are ANSI
				int iLen = (int)strlen ( pSym->Name ) ;
				if ( iLen > ( BUFF_SIZE - iCurr -
					( MAX_SYM_SIZE + 50 ) ) )
				{
					strncpy ( g_szBuff + iCurr      ,
						pSym->Name            ,
						BUFF_SIZE - iCurr - 1  ) ;


					// Gotta leave now
					szRet = g_szBuff ;
					__leave ;
				}
				else
				{
					if ( dwDisp > 0 )
					{
						iCurr += sprintf ( g_szBuff + iCurr    ,
							k_NAMEDISPFMT       ,
							pSym->Name          ,
							dwDisp               ) ;
					}
					else
					{
						iCurr += sprintf ( g_szBuff + iCurr ,
							k_NAMEFMT        ,
							pSym->Name        ) ;
					}
				}
			}
			else
			{
				// If the symbol wasn't found, the source file and line
				// number won't be found either, so leave now.
				szRet = g_szBuff ;
				__leave ;
			}

		}

		assert ( iCurr < ( BUFF_SIZE - MAX_PATH ) ) ;

		// Output the source file and line number information?
		if ( GSTSO_SRCLINE == ( dwOpts & GSTSO_SRCLINE ) )
		{
			ZeroMemory ( &g_stLine , sizeof ( IMAGEHLP_LINE64 ) ) ;
			g_stLine.SizeOfStruct = sizeof ( IMAGEHLP_LINE64 ) ;

			DWORD dwLineDisp ;
			if ( TRUE == SymGetLineFromAddr64 ( GetCurrentProcess ( )  ,
				g_stFrame.AddrPC.Offset,
				&dwLineDisp            ,
				&g_stLine             ))
			{
				if ( dwOpts & ~GSTSO_SRCLINE )
				{
					iCurr += sprintf ( g_szBuff + iCurr , ( "," ));
				}

				// Copy no more of the source file and line number
				// information than there's room for.
				int iLen = lstrlenA ( g_stLine.FileName ) ;
				if ( iLen > ( BUFF_SIZE - iCurr -
					( MAX_PATH + 50     ) ) )
				{

					strncpy ( g_szBuff + iCurr      ,
						g_stLine.FileName     ,
						BUFF_SIZE - iCurr - 1  ) ;

					// Gotta leave now
					szRet = g_szBuff ;
					__leave ;
				}
				else
				{
					if ( dwLineDisp > 0 )
					{
						iCurr += sprintf( g_szBuff + iCurr     ,
							k_FILELINEDISPFMT    ,
							g_stLine.FileName    ,
							g_stLine.LineNumber  ,
							dwLineDisp            ) ;
					}
					else
					{
						iCurr += sprintf ( g_szBuff + iCurr    ,
							k_FILELINEFMT       ,
							g_stLine.FileName   ,
							g_stLine.LineNumber  ) ;
					}
				}
			}
		}

		szRet = g_szBuff ;
	}
	__except ( EXCEPTION_EXECUTE_HANDLER )
	{
		assert ( !"Crashed in InternalGetStackTraceString" ) ;
		szRet = NULL ;
	}
	return ( szRet ) ;
}



// Helper define to let code compile on pre W2K systems.
#if _WIN32 >= 0x500
#define GET_THREAD_ACP() CP_THREAD_ACP
#else
#define GET_THREAD_ACP() GetACP ()
#endif


DWORD  __stdcall
BSUWide2Ansi ( const wchar_t * szWide  ,
			  char *          szANSI  ,
			  int             iANSILen)
{
	assert ( NULL != szWide ) ;
	assert ( NULL != szANSI ) ;
	assert ( FALSE == IsBadStringPtrW ( szWide , MAX_PATH ) ) ;

	int iRet = WideCharToMultiByte ( GET_THREAD_ACP() ,
		0                ,
		szWide           ,
		-1               ,
		szANSI           ,
		iANSILen         ,
		NULL             ,
		NULL              ) ;
	return ( iRet ) ;
}

DWORD  __stdcall
BSUAnsi2Wide ( const char * szANSI   ,
			  wchar_t *    szWide   ,
			  int          iWideLen  )
{
	assert ( NULL != szWide ) ;
	assert ( NULL != szANSI ) ;
	assert ( FALSE == IsBadStringPtrA ( szANSI , MAX_PATH ) ) ;

	int iRet = MultiByteToWideChar ( GET_THREAD_ACP ( ) ,
		0                  ,
		szANSI             ,
		-1                 ,
		szWide             ,
		iWideLen            ) ;
	return ( iRet ) ;
}


DWORD __stdcall NTGetModuleBaseNameW ( HANDLE  hProcess   ,
									  HMODULE hModule    ,
									  LPWSTR  lpBaseName ,
									  DWORD   nSize       )
{
	// Initialize PSAPI.DLL, if needed.
	if ( FALSE == InitPSAPI ( ) )
	{
		assert ( !"InitiPSAPI failed!" ) ;
		SetLastErrorEx ( ERROR_DLL_INIT_FAILED , SLE_ERROR ) ;
		return ( FALSE ) ;
	}
	return ( g_pGetModuleBaseName ( hProcess    ,
		hModule     ,
		lpBaseName  ,
		nSize        ) ) ;
}

/*//////////////////////////////////////////////////////////////////////
Function Implementation
//////////////////////////////////////////////////////////////////////*/
DWORD __stdcall BSUGetModuleBaseNameA ( HANDLE  hProcess   ,
									   HMODULE hModule    ,
									   LPSTR   lpBaseName ,
									   DWORD   nSize       )
{
	wchar_t * pWideName = (wchar_t*)HeapAlloc ( GetProcessHeap ( )      ,
		HEAP_GENERATE_EXCEPTIONS|
		HEAP_ZERO_MEMORY       ,
		nSize * sizeof(wchar_t));
	DWORD dwRet = BSUGetModuleBaseNameW ( hProcess      ,
		hModule       ,
		pWideName     ,
		nSize          ) ;
	if ( 0 != dwRet )
	{
		if ( FALSE == BSUWide2Ansi ( pWideName , lpBaseName , nSize ) )
		{
			dwRet = 0 ;
		}
	}
	//VERIFY ( HeapFree ( GetProcessHeap ( ) , 0 , pWideName ) ) ;
	return ( dwRet ) ;
}

DWORD __stdcall BSUGetModuleBaseNameW ( HANDLE  hProcess   ,
									   HMODULE hModule    ,
									   LPWSTR lpBaseName  ,
									   DWORD   nSize       )
{
	// Call the NT version.  It is in NT4ProcessInfo because that is
	// where all the PSAPI wrappers are kept.
	return ( NTGetModuleBaseNameW ( hProcess     ,
		hModule      ,
		lpBaseName   ,
		nSize         ) ) ;
}



/*----------------------------------------------------------------------
FUNCTION        :   InitPSAPI
DISCUSSION      :
Loads PSAPI.DLL and initializes all the pointers needed by this
file.  If BugslayerUtil.DLL statically linked to PSAPI.DLL, it would not
work on Windows9x.
Note that I conciously chose to allow the resource leak on loading
PSAPI.DLL.
PARAMETERS      :
None.
RETURNS         :
TRUE  - Everything initialized properly.
FALSE - There was a problem.
----------------------------------------------------------------------*/
static BOOL InitPSAPI ( void )
{
	if ( TRUE == g_bInitialized )
	{
		return ( TRUE ) ;
	}

	// Load up PSAPI.DLL.
	HINSTANCE hInst = LoadLibraryA( "PSAPI.DLL" ) ;
	assert ( NULL != hInst ) ;
	if ( NULL == hInst )
	{
		TRACE ( "Unable to load PSAPI.DLL!\n" ) ;
		return ( FALSE ) ;
	}

	// Now do the GetProcAddress stuff.
	g_pEnumProcessModules =
		(ENUMPROCESSMODULES)GetProcAddress ( hInst ,
		"EnumProcessModules" ) ;
	assert ( NULL != g_pEnumProcessModules ) ;
	if ( NULL == g_pEnumProcessModules )
	{
		TRACE ( "GetProcAddress failed on EnumProcessModules!\n" ) ;
		FreeLibrary(hInst);
		return ( FALSE ) ;
	}

	g_pGetModuleBaseName =
		(GETMODULEBASENAMEW)GetProcAddress ( hInst ,
		"GetModuleBaseNameW" ) ;
	assert ( NULL != g_pGetModuleBaseName ) ;
	if ( NULL == g_pGetModuleBaseName )
	{
		TRACE ( "GetProcAddress failed on GetModuleBaseNameW!\n" ) ;
		FreeLibrary(hInst);
		return ( FALSE ) ;
	}

	g_pGetModuleFileNameEx =
		(GETMODULEFILENAMEEXW)GetProcAddress ( hInst ,
		"GetModuleFileNameExW" );
	assert ( NULL != g_pGetModuleFileNameEx ) ;
	if ( NULL == g_pGetModuleFileNameEx )
	{
		TRACE ( "GetProcAddress failed on GetModuleFileNameExW\n" ) ;
		FreeLibrary(hInst);
		return ( FALSE ) ;
	}

	// All OK, Jumpmaster!
	g_bInitialized = TRUE ;
	return ( TRUE ) ;
}


void NxGetCallStack(OUT std::string& szString, EXCEPTION_POINTERS* pExPtrs,  DWORD dwOpt )
{
	szString = "";

#define _ALL (GSTSO_PARAMS|GSTSO_MODULE|GSTSO_SYMBOL|GSTSO_SRCLINE)

	const char* szBuff = GetFirstStackTraceString ( dwOpt  , pExPtrs  ) ;
	while ( NULL != szBuff )
	{
		szString += szBuff;
		szString += "\n";
		szBuff = GetNextStackTraceString ( dwOpt , pExPtrs ) ;
	}
}

