#pragma once

//------------------------------------------------------------------------------
// Define the machine type.
#ifdef _X86_
#define CH_MACHINE IMAGE_FILE_MACHINE_I386
#elif _AMD64_
#define CH_MACHINE IMAGE_FILE_MACHINE_AMD64
#elif _IA64_
#define CH_MACHINE IMAGE_FILE_MACHINE_IA64
#else
#pragma FORCE COMPILE ABORT!
#endif

#define GSTSO_PARAMS    0x01
#define GSTSO_MODULE    0x02
#define GSTSO_SYMBOL    0x04
#define GSTSO_SRCLINE   0x08
#define GSTSO_ALL		(GSTSO_PARAMS|GSTSO_MODULE|GSTSO_SYMBOL|GSTSO_SRCLINE)

#define k_NAMEDISPFMT       ( " %s()+%04d byte(s)" )
#define k_NAMEFMT           ( " %s " )
#define k_FILELINEDISPFMT   ( " %s, line %04d+%04d byte(s)" )
#define k_FILELINEFMT       ( " %s, line %04d" )

#ifdef _WIN64
#define k_PARAMFMTSTRING    ( " (0x%016X 0x%016X 0x%016X 0x%016X)" )
#else
#define k_PARAMFMTSTRING    ( " (0x%08X 0x%08X 0x%08X 0x%08X)" )
#endif


#ifdef UNICODE
#define BSUGetModuleBaseName BSUGetModuleBaseNameW
#else
#define BSUGetModuleBaseName BSUGetModuleBaseNameA
#endif

const char* __stdcall GetFirstStackTraceString( DWORD dwOpts, EXCEPTION_POINTERS* pExPtrs );
const char* __stdcall GetNextStackTraceString ( DWORD dwOpts, EXCEPTION_POINTERS* pExPtrs );

void FillInStackFrame ( PCONTEXT pCtx );

const char* InternalGetStackTraceString ( DWORD dwOpts );

// Initializes the symbol engine if needed
void InitSymEng ( void );

// Cleans up the symbol engine if needed
void CleanupSymEng ( void );

BOOL __stdcall CH_ReadProcessMemory ( HANDLE hProcess, DWORD64 qwBaseAddress, PVOID lpBuffer, DWORD nSize, LPDWORD lpNumberOfBytesRead );

DWORD __stdcall BSUGetModuleBaseNameW ( HANDLE hProcess, HMODULE hModule, LPWSTR lpBaseName, DWORD nSize );
DWORD __stdcall BSUGetModuleBaseNameA ( HANDLE hProcess, HMODULE hModule, LPSTR  lpBaseName, DWORD nSize );

DWORD __stdcall BSUWide2Ansi ( const wchar_t * szWide, char* szANSI, int iANSILen ) ;
DWORD __stdcall BSUAnsi2Wide ( const char * szANSI, wchar_t* szWide, int iWideLen );

BOOL InitPSAPI ( void );

void GetCallStack(OUT std::string& szString, EXCEPTION_POINTERS* pExPtrs, DWORD dwOpt = GSTSO_ALL );
LONG __stdcall CallStackDump(EXCEPTION_POINTERS* pExPtrs);
void NxGetCallStack(OUT std::string& szString, EXCEPTION_POINTERS* pExPtrs,  DWORD dwOpt );
const char* GetRegisterString(EXCEPTION_POINTERS* pExPtrs);
const char* GetFaultReason(EXCEPTION_POINTERS* pExPtrs);