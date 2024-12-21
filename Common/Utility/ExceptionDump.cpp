#include "stdafx.h"

#if 0 

#include <windows.h>
#include <assert.h>
#include <time.h>
#include <tchar.h>
#include <DbgHelp.h>
#include <string>
#include <iostream>
#include "ExceptionDump.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

//--------------------------------------------------------------------------------------------------------------
int			ExceptionDump::m_nDumpLevel = ExceptionDump::eMiniDumpNormal;
bool		ExceptionDump::m_bEnableDump = false;
std::string	ExceptionDump::m_szDumpFileName;
LPTOP_LEVEL_EXCEPTION_FILTER ExceptionDump::m_pExceptionFilter = NULL;


// based on dbghelp.h
typedef BOOL (WINAPI *MINIDUMPWRITEDUMP) (  HANDLE hProcess, DWORD dwPid, HANDLE hFile, MINIDUMP_TYPE DumpType,
										  CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
										  CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
										  CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam  );
using std::cout;
using std::endl;

void
ExceptionDump::Enable(DumpLevel level)
{

	m_bEnableDump = true;
	m_nDumpLevel = level;

	std::set_new_handler(ExceptionDump::BadAllocFilter);
	m_pExceptionFilter = SetUnhandledExceptionFilter(ExceptionDump::Exception_Minidump); 
}

void
ExceptionDump::Disable()
{
	m_bEnableDump = false;
	SetUnhandledExceptionFilter(NULL); 

}

void __cdecl
ExceptionDump::BadAllocFilter()
{
	//MessageBox( NULL, _T("메모리 할당 실패 입니다."), _T("치명적인 오류"), MB_OK );
	MessageBox( NULL, _T("Failed to Memory Alloc."), _T("Critical Error"), MB_OK );
	throw std::bad_alloc();
}

LONG __stdcall
ExceptionDump::Exception_Minidump(_EXCEPTION_POINTERS* pExceptionInfo)
{
	CallStackDump(pExceptionInfo);

	// 현재 실행 모듈을 알아낸다.
	CHAR DumpFileName[MAX_PATH];
	::GetModuleFileNameA(NULL, DumpFileName, MAX_PATH);

	CHAR drive[_MAX_DRIVE];
	CHAR dir[_MAX_DIR];
	CHAR fname[_MAX_FNAME];
	CHAR ext[_MAX_EXT];

	_splitpath( DumpFileName, drive, dir, fname, ext ); 

	// 확장자를 제거한 모듈 경로를 준비해둔다.
	std::string szDumpFileName;
	szDumpFileName = drive;
	szDumpFileName += dir;
	szDumpFileName += fname;

	struct tm* now=NULL;
	time_t systemTime;

	time(&systemTime);  
	now=localtime(&systemTime);

	CHAR szTail[MAX_PATH];
	ZeroMemory(szTail, sizeof(CHAR) * MAX_PATH);

	sprintf(szTail, "_%04d년%02d월%02d일_%02d시%02d분%02d초",
		1900+now->tm_year, now->tm_mon+1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec);

	// 모듈명 + 시간정보로 저장할 파일 명을 만든다.
	szDumpFileName += szTail;

	cout << _T("unhandled excetpion writing dump...") << endl;

	std::string szTimeStampDmpFileName = szDumpFileName + ".dmp";
	std::string szTimeStampLogFileName = szTimeStampDmpFileName + "_errorlog_.txt";

	HMODULE hDLL = NULL;
	CHAR szDbgHelpPath[_MAX_PATH] = {0, };

	// 먼저 실행 파일이 있는 디렉토리에서 DBGHELP.DLL을 로드해 본다.
	// Windows 2000 의 System32 디렉토리에 있는 DBGHELP.DLL 파일은 버전이 
	// 오래된 것일 수 있기 때문이다. (최소 5.1.2600.0 이상이어야 한다.)
	if (::GetModuleFileNameA(NULL, szDbgHelpPath, _MAX_PATH))
	{
		if (CHAR* slash = ::strchr(szDbgHelpPath, _T('\\')))
		{
			::strcpy(slash + 1, "DBGHELP.DLL");
			hDLL = ::LoadLibraryA(szDbgHelpPath);
		}
	}

	// 현재 디렉토리에 없다면, 아무 버전이나 로드한다.
	if (hDLL == NULL) 
	{
		hDLL = ::LoadLibraryA("dbghelp.dll");
	}

	// DBGHELP.DLL을 찾을 수 없다면 더 이상 진행할 수 없다.
	if (hDLL == NULL)
	{
		FILE* pFile = fopen(szTimeStampLogFileName.c_str(), "w");
		fprintf(pFile, "dbghelp.dll not found");
		fclose(pFile);

//		if( m_PrevExceptionFilter ) m_PrevExceptionFilter( pExceptionInfo );
		return EXCEPTION_CONTINUE_SEARCH;
	}

	// DLL 내부에서 MiniDumpWriteDump API를 찾는다.
	MINIDUMPWRITEDUMP pfnMiniDumpWriteDump = (MINIDUMPWRITEDUMP)::GetProcAddress(hDLL, "MiniDumpWriteDump");
	if (pfnMiniDumpWriteDump == NULL)
	{
		FILE* pFile = fopen(szTimeStampLogFileName.c_str(), "w");
		fprintf(pFile, "dbghelp.dll too old");
		fclose(pFile);

		//if( m_PrevExceptionFilter ) m_PrevExceptionFilter( pExceptionInfo );
		return EXCEPTION_CONTINUE_SEARCH;
	}

	HANDLE hFile = ::CreateFileA( szTimeStampDmpFileName.c_str(), GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		// 스택과 로드한 모듈 정보를 텍스트 파일로 만든다.
		FILE* pFile = fopen(szTimeStampLogFileName.c_str(), "w");
		fprintf(pFile, "Failed to save Dump File to %s ( error : %d )\n", szTimeStampLogFileName.c_str(), ::GetLastError());
		fclose(pFile);

		//if( m_PrevExceptionFilter ) m_PrevExceptionFilter( pExceptionInfo );
		return EXCEPTION_CONTINUE_SEARCH;
	}

	MINIDUMP_EXCEPTION_INFORMATION ExParam;
	ExParam.ThreadId = ::GetCurrentThreadId();
	ExParam.ExceptionPointers = pExceptionInfo;
	ExParam.ClientPointers = FALSE;

	MINIDUMP_TYPE dumptype = MiniDumpWithFullMemory;
	switch (m_nDumpLevel)
	{
	case eMiniDumpNormal:			dumptype = MiniDumpNormal;         break;
	case eMiniDumpWithDataSegs:		dumptype = MiniDumpWithDataSegs;   break;
	case eMiniDumpWithFullMemory:   dumptype = MiniDumpWithFullMemory; break;
	}

	// 덤프 파일 생성 결과를 로그 파일에다 기록한다.
	pfnMiniDumpWriteDump( ::GetCurrentProcess(), ::GetCurrentProcessId(), hFile, dumptype, &ExParam, NULL, NULL);

	// dmp 파일을 만든다.
	CHAR temp[2048] = "0";

	sprintf(temp, "Exception at 0x%08x\nDump to %s", pExceptionInfo->ExceptionRecord->ExceptionCode, szTimeStampDmpFileName.c_str());
	MessageBoxA(NULL, temp, szDumpFileName.c_str(), MB_OK);
	::CloseHandle(hFile);

	
	
//	if( m_PrevExceptionFilter ) m_PrevExceptionFilter( pExceptionInfo );
	return EXCEPTION_EXECUTE_HANDLER;
}


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

/// \brief 사용자 정보를 반환한다.
/// \return LPCTSTR 사용자 이름
const char*
GetUserInfo()
{
	static CHAR szUserName[200] = {0,};

	ZeroMemory(szUserName, sizeof(szUserName));
	DWORD UserNameSize = sizeof(szUserName) - 1;

	if (!::GetUserNameA(szUserName, &UserNameSize))
		::strcpy(szUserName, "Unknown");

	return szUserName;
}

/// \brief 윈도우즈 버전을 반환한다.
/// \return LPCTSTR 윈도우즈 버전 문자열
const char*
GetOSInfo()
{
	CHAR szWinVer[50] = {0,};
	CHAR szMajorMinorBuild[50] = {0,};
	int nWinVer = 0;
	::GetWinVersion(szWinVer, &nWinVer, szMajorMinorBuild);

	static CHAR szOSInfo[512] = {0,};

	sprintf(szOSInfo, "%s (%s)", szWinVer, szMajorMinorBuild);
	szOSInfo[512] = 0;

	return szOSInfo;
}

/// \brief CPU 정보를 반환한다.
/// \return LPCTSTR CPU 정보 문자열
std::string 
GetCpuInfo()
{
	// CPU 정보 기록
	SYSTEM_INFO	SystemInfo;
	GetSystemInfo(&SystemInfo);

	CHAR szCpuInfo[512] = {0,};

	sprintf(szCpuInfo, "%d processor(s), type %d",
		SystemInfo.dwNumberOfProcessors, SystemInfo.dwProcessorType);

	return std::string(szCpuInfo);
}

std::string 
GetCpuInfoFromRegistry()
{
	std::string szCpuString;
	HKEY hKey;
	DWORD dataSize=0;
	CHAR szCPUNAME [64] = {0,};
	CHAR szTempEx [64] = {0,};
	ZeroMemory(szCPUNAME,64);
	ZeroMemory(szTempEx,64);
	LONG regResult;
	regResult = ::RegOpenKeyExA (HKEY_LOCAL_MACHINE,
		"Hardware\\Description\\System\\CentralProcessor\\0", 0, KEY_QUERY_VALUE, &hKey);

	if(regResult == ERROR_SUCCESS){
		dataSize = sizeof (szCPUNAME);
		regResult = ::RegQueryValueExA (hKey, "ProcessorNameString", NULL, NULL,
			(LPBYTE)szCPUNAME, &dataSize);
		int num=0;
		bool bspace=true;
		for(int i=0;i<64;i++)
		{
			if(szCPUNAME[i] != ' ' || bspace==false)
			{
				bspace = false;
				szTempEx[num++] = szCPUNAME[i];
			}
		}
		szCpuString = szTempEx;
		//wcscpy(m_wszCpuString,szTempEx);
	}
	RegCloseKey (hKey);

	return szCpuString;
}

/// \brief 메모리 정보를 반환한다.
/// \return LPCTSTR 메모리 정보 문자열
const char*
GetMemoryInfo()
{
	static const int ONE_K = 1024;
	static const int ONE_M = ONE_K * ONE_K;
	static const int ONE_G = ONE_K * ONE_K * ONE_K;

	MEMORYSTATUS MemInfo;
	MemInfo.dwLength = sizeof(MemInfo);
	GlobalMemoryStatus(&MemInfo);

	static CHAR szMemoryInfo[2048] = {0,};

	sprintf(szMemoryInfo,
		"%d%% of memory in use.\n"
		"%d MB physical memory.\n"
		"%d MB physical memory free.\n"
		"%d MB paging file.\n"
		"%d MB paging file free.\n"
		"%d MB user address space.\n"
		"%d MB user address space free.",
		MemInfo.dwMemoryLoad, 
		(MemInfo.dwTotalPhys + ONE_M - 1) / ONE_M, 
		(MemInfo.dwAvailPhys + ONE_M - 1) / ONE_M, 
		(MemInfo.dwTotalPageFile + ONE_M - 1) / ONE_M, 
		(MemInfo.dwAvailPageFile + ONE_M - 1) / ONE_M, 
		(MemInfo.dwTotalVirtual + ONE_M - 1) / ONE_M, 
		(MemInfo.dwAvailVirtual + ONE_M - 1) / ONE_M);

	return szMemoryInfo;
}

/// \brief 윈도우즈 버전을 알아낸다.
///
/// This table has been assembled from Usenet postings, personal observations, 
/// and reading other people's code.  Please feel free to add to it or correct 
/// it.
///
/// <pre>
/// dwPlatFormID  dwMajorVersion  dwMinorVersion  dwBuildNumber
/// 95            1               4                 0            950
/// 95 SP1        1               4                 0            >950 && <=1080
/// 95 OSR2       1               4               <10            >1080
/// 98            1               4                10            1998
/// 98 SP1        1               4                10            >1998 && <2183
/// 98 SE         1               4                10            >=2183
/// ME            1               4                90            3000
///
/// NT 3.51       2               3                51
/// NT 4          2               4                 0            1381
/// 2000          2               5                 0            2195
/// XP            2               5                 1            2600
/// 2003 Server   2               5                 2            3790
///
/// CE            3
/// </pre>
///
/// \param pszVersion 버전 문자열을 집어넣을 포인터
/// \param nVersion 버전 숫자값을 집어넣을 포인터
/// \param pszMajorMinorBuild 빌드 문자열을 집어넣을 포인터
/// \return bool 무사히 실행한 경우에는 true, 뭔가 에러가 생긴 경우에는 false
bool
GetWinVersion(char* pszVersion, int *nVersion, char* pszMajorMinorBuild)
{
	// from winbase.h
#ifndef VER_PLATFORM_WIN32s
#define VER_PLATFORM_WIN32s 0
#endif

#ifndef VER_PLATFORM_WIN32_WINDOWS
#define VER_PLATFORM_WIN32_WINDOWS 1
#endif

#ifndef VER_PLATFORM_WIN32_NT
#define VER_PLATFORM_WIN32_NT 2
#endif

#ifndef VER_PLATFORM_WIN32_CE
#define VER_PLATFORM_WIN32_CE 3
#endif

	static const char* WUNKNOWNSTR     = ("Unknown Windows Version");
	static const char* W95STR          = ("Windows 95");
	static const char* W95SP1STR       = ("Windows 95 SP1");
	static const char* W95OSR2STR      = ("Windows 95 OSR2");
	static const char* W98STR          = ("Windows 98");
	static const char* W98SP1STR       = ("Windows 98 SP1");
	static const char* W98SESTR        = ("Windows 98 SE");
	static const char* WMESTR          = ("Windows ME");
	static const char* WNT351STR       = ("Windows NT 3.51");
	static const char* WNT4STR         = ("Windows NT 4");
	static const char* W2KSTR          = ("Windows 2000");
	static const char* WXPSTR          = ("Windows XP");
	static const char* W2003SERVERSTR  = ("Windows 2003 Server");
	static const char* WCESTR          = ("Windows CE");

	static const int WUNKNOWN      = 0;
	static const int W9XFIRST      = 1;
	static const int W95           = 1;
	static const int W95SP1        = 2;
	static const int W95OSR2       = 3;
	static const int W98           = 4;
	static const int W98SP1        = 5;
	static const int W98SE         = 6;
	static const int WME           = 7;
	static const int W9XLAST       = 99;
	static const int WNTFIRST      = 101;
	static const int WNT351        = 101;
	static const int WNT4          = 102;
	static const int W2K           = 103;
	static const int WXP           = 104;
	static const int W2003SERVER   = 105;
	static const int WNTLAST       = 199;
	static const int WCEFIRST      = 201;
	static const int WCE           = 201;
	static const int WCELAST       = 299;

	if (!pszVersion || !nVersion || !pszMajorMinorBuild) 
		return false;

	::strcpy(pszVersion, WUNKNOWNSTR);
	*nVersion = WUNKNOWN;

	OSVERSIONINFO osinfo;
	osinfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	if (!GetVersionEx(&osinfo)) return false;

	DWORD dwPlatformId   = osinfo.dwPlatformId;
	DWORD dwMinorVersion = osinfo.dwMinorVersion;
	DWORD dwMajorVersion = osinfo.dwMajorVersion;
	DWORD dwBuildNumber  = osinfo.dwBuildNumber & 0xFFFF; // Win 95 needs this

	CHAR buf[1024] = {0, };
	sprintf(buf, "%u.%u.%u", dwMajorVersion, dwMinorVersion, dwBuildNumber);
	::strcpy(pszMajorMinorBuild, buf);

	if ((dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) && (dwMajorVersion == 4))
	{
		if ((dwMinorVersion < 10) && (dwBuildNumber == 950))
		{
			::strcpy(pszVersion, W95STR);
			*nVersion = W95;
		}
		else if ((dwMinorVersion < 10) && 
			((dwBuildNumber > 950) && (dwBuildNumber <= 1080)))
		{
			::strcpy(pszVersion, W95SP1STR);
			*nVersion = W95SP1;
		}
		else if ((dwMinorVersion < 10) && (dwBuildNumber > 1080))
		{
			::strcpy(pszVersion, W95OSR2STR);
			*nVersion = W95OSR2;
		}
		else if ((dwMinorVersion == 10) && (dwBuildNumber == 1998))
		{
			::strcpy(pszVersion, W98STR);
			*nVersion = W98;
		}
		else if ((dwMinorVersion == 10) && 
			((dwBuildNumber > 1998) && (dwBuildNumber < 2183)))
		{
			::strcpy(pszVersion, W98SP1STR);
			*nVersion = W98SP1;
		}
		else if ((dwMinorVersion == 10) && (dwBuildNumber >= 2183))
		{
			::strcpy(pszVersion, W98SESTR);
			*nVersion = W98SE;
		}
		else if (dwMinorVersion == 90)
		{
			::strcpy(pszVersion, WMESTR);
			*nVersion = WME;
		}
	}
	else if (dwPlatformId == VER_PLATFORM_WIN32_NT)
	{
		if ((dwMajorVersion == 3) && (dwMinorVersion == 51))
		{
			::strcpy(pszVersion, WNT351STR);
			*nVersion = WNT351;
		}
		else if ((dwMajorVersion == 4) && (dwMinorVersion == 0))
		{
			::strcpy(pszVersion, WNT4STR);
			*nVersion = WNT4;
		}
		else if ((dwMajorVersion == 5) && (dwMinorVersion == 0))
		{
			::strcpy(pszVersion, W2KSTR);
			*nVersion = W2K;
		}
		else if ((dwMajorVersion == 5) && (dwMinorVersion == 1))
		{
			::strcpy(pszVersion, WXPSTR);
			*nVersion = WXP;
		}
		else if ((dwMajorVersion == 5) && (dwMinorVersion == 2))
		{
			::strcpy(pszVersion, W2003SERVERSTR);
			*nVersion = W2003SERVER;
		}
	}
	else if (dwPlatformId == VER_PLATFORM_WIN32_CE)
	{
		::strcpy(pszVersion, WCESTR);
		*nVersion = WCE;
	}

	return true;

#undef VER_PLATFORM_WIN32s
#undef VER_PLATFORM_WIN32_WINDOWS
#undef VER_PLATFORM_WIN32_NT
#undef VER_PLATFORM_WIN32_CE
}



const char* 
GetRegisterString(EXCEPTION_POINTERS* pExPtrs)
{
	static CHAR szBuff[8192] = "";

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
		return ( FALSE ) ;
	}

	g_pGetModuleBaseName =
		(GETMODULEBASENAMEW)GetProcAddress ( hInst ,
		"GetModuleBaseNameW" ) ;
	assert ( NULL != g_pGetModuleBaseName ) ;
	if ( NULL == g_pGetModuleBaseName )
	{
		TRACE ( "GetProcAddress failed on GetModuleBaseNameW!\n" ) ;
		return ( FALSE ) ;
	}

	g_pGetModuleFileNameEx =
		(GETMODULEFILENAMEEXW)GetProcAddress ( hInst ,
		"GetModuleFileNameExW" );
	assert ( NULL != g_pGetModuleFileNameEx ) ;
	if ( NULL == g_pGetModuleFileNameEx )
	{
		TRACE ( "GetProcAddress failed on GetModuleFileNameExW\n" ) ;
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
	do
	{
		szString += szBuff;
		szString += "\n";
		szBuff = GetNextStackTraceString ( dwOpt , pExPtrs ) ;
	}while ( NULL != szBuff ) ;

}


LONG __stdcall
CallStackDump(EXCEPTION_POINTERS* pExPtrs)
{
	std::string szDumpFileName;
	// 현재 실행 모듈을 알아낸다.
	CHAR DumpFileName[MAX_PATH];
	::GetModuleFileNameA(NULL, DumpFileName, MAX_PATH);

	CHAR drive[_MAX_DRIVE];
	CHAR dir[_MAX_DIR];
	CHAR fname[_MAX_FNAME];
	CHAR ext[_MAX_EXT];

	_splitpath( DumpFileName, drive, dir, fname, ext ); 

	// 확장자를 제거한 모듈 경로를 준비해둔다.
	szDumpFileName = drive;
	szDumpFileName += dir;
	szDumpFileName += fname;

	struct tm* now=NULL;
	time_t systemTime;

	time(&systemTime);  
	now=localtime(&systemTime);


	CHAR szTail[MAX_PATH];
	ZeroMemory(szTail, sizeof(CHAR) * MAX_PATH);

	sprintf(szTail, "_%04d년%02d월%02d일_%02d시%02d분%02d초",
		1900+now->tm_year, now->tm_mon+1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec);

	// 모듈명 + 시간정보로 저장할 파일 명을 만든다.
	szDumpFileName += szTail;

	// 먼저 콜스택과 모듈들을 문자열로 만든다.
	std::string szStackString ;

	NxGetCallStack(szStackString, pExPtrs, GSTSO_ALL);

	std::string szTimeStampLogFileName = szDumpFileName + ".txt";


	std::string szStack;
	szStack		 = "\n------------------------------------ Stack --------------------------------------------\n";

	std::string szRegister;
	szRegister		 = "\n------------------------------------ Register --------------------------------------------\n";
	szRegister += GetRegisterString(pExPtrs);


	// 스택과 로드한 모듈 정보를 텍스트 파일로 만든다.
	FILE* pFile = fopen(szTimeStampLogFileName.c_str(), "w");
	fprintf(pFile, "saved dump file to '%s'\n"
		"\n<fault reason>\n%s"
		"\n\n<user>\n%s"
		"\n\n<os>\n%s"
		"\n\n<cpu>\n%s"
		"\n\n<memory>\n%s\n",
		szTimeStampLogFileName.c_str(),
		GetFaultReason(pExPtrs),
		GetUserInfo(),
		GetOSInfo(),
		GetCpuInfo(),
		GetMemoryInfo()
		);

	fwrite(szRegister.c_str(), 1, szRegister.size(), pFile );

	fwrite(szStack.c_str(), 1, szStack.size(), pFile );
	fwrite(szStackString.c_str(), 1, szStackString.size(), pFile);
	fclose(pFile);


	// dmp 파일을 만든다.
	CHAR temp[2048] = "0";

	sprintf(temp, "Exception at 0x%08x\n\nCallstack Logging %s", pExPtrs->ExceptionRecord->ExceptionCode, szTimeStampLogFileName.c_str());
	MessageBoxA(NULL, temp, szDumpFileName.c_str(), MB_OK);


	return EXCEPTION_EXECUTE_HANDLER;

}

#endif