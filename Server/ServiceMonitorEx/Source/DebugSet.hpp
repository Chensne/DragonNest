/*//===============================================================

	<< DEBUG >>

	PRGM : B4nFter, milkji

	FILE : DEBUGSET.HPP
	DESC : 디버깅을 위한 메서드 모음
	INIT BUILT DATE : 2005. 04. 28
	LAST BUILT DATE : 2006. 03. 07

	Copyrightⓒ 2002~2009 B4nFter (b4nfter@gmail.com) and Co-Writers. All Rights Reserved.

*///===============================================================


#pragma once


///////////////////////////////////////////////////////////////////
//	INCLUDE
///////////////////////////////////////////////////////////////////

// P.S.> SYSBASEBASE.H 에 의해 참조됨 (따라서 본 헤더는 WINDOWS.H 등 기본헤더 제외시킴)

#if defined(_DEBUG)
#include <STRSAFE.H>
#include <CRTDBG.H>		// DCRT를 위해서 가장 마지막에 include 되어야 함
#endif	// _DEBUG


///////////////////////////////////////////////////////////////////
//	DEFINE
///////////////////////////////////////////////////////////////////

/*
	<VISUAL STUDIO 버전 설정>
		Minimum system required				Macros to define
		------------------------			------------------------
		Visual Studio.net 9.0 (2008)		_MSC_VER 15.00 (1500)
		Visual Studio.net 8.0 (2005)		_MSC_VER 14.00 (1400)
		Visual Studio.net 7.1 (2003)		_MSC_VER 13.10 (1310)
		Visual Studio.net 7.0				_MSC_VER 13.00 (1300)
		Visual Studio 6.0					_MSC_VER 12.00 (1200)
		Visual Studio 5.0					_MSC_VER 10.00 (1000)
*/

/*
	<메모리 상태값 설명 (WIN32 기준)>

	0xCDCDCDCD	: HEAP 에 할당되었으나 초기화 되지 않음
	0xDDDDDDDD	: HEAP 에서 해제된 영역
	0xFDFDFDFD	: HEAP 메모리의 경계에 자동으로 위치하는 'NoMansLand' 울타리로 절대 덮어씌여져선 않됨. 만약 덮어쓴다면 아마도 배열 경계의 끝을 넘은것임
	0xCCCCCCCC	: STACK 에 위치되었으나 초기화 되지 않음
*/
#if defined(_X86_)
#define DF_MEMSTATE_CDCD	((DWORD_PTR)(0xCDCDCDCD))
#define DF_MEMSTATE_DDDD	((DWORD_PTR)(0xDDDDDDDD))
#define DF_MEMSTATE_FDFD	((DWORD_PTR)(0xFDFDFDFD))
#define DF_MEMSTATE_CCCC	((DWORD_PTR)(0xCCCCCCCC))
#elif defined(_AMD64_)
#define DF_MEMSTATE_CDCD	((DWORD_PTR)(0xCDCDCDCDCDCDCDCD))
#define DF_MEMSTATE_DDDD	((DWORD_PTR)(0xDDDDDDDDDDDDDDDD))
#define DF_MEMSTATE_FDFD	((DWORD_PTR)(0xFDFDFDFDFDFDFDFD))
#define DF_MEMSTATE_CCCC	((DWORD_PTR)(0xCCCCCCCCCCCCCCCC))
#elif defined(_IA64_)
#pragma FORCE COMPILE ABORT!
#else	// !_X86 && !_AMD64_ && !_IA64_
#pragma FORCE COMPILE ABORT!
#endif	// !_X86 && !_AMD64_ && !_IA64_

// 컴파일 주석 매크로 함수
// P.S.>
/*
	- 컴파일 타임에 트레이스 창에 남는 주석
	- 파일명과 줄번호를 출력하며 클릭 시 이동
	- #pragma COMPILE_COMMENT("주석내용") 로 기술
*/
#define __COMPILE_COMMENTX__(pMSG)		#pMSG
#define __COMPILE_COMMENT__(pMSG)		__COMPILE_COMMENTX__(pMSG) 
#define	COMPILE_COMMENT(pCMT)			message(__FILE__ "(" __COMPILE_COMMENT__(__LINE__) ") : "pCMT)

// 디버깅 관련 매크로 함수들
#if defined(_DEBUG)
	#if !defined(_CRTDBG_MAP_ALLOC)
		#define	_CRTDBG_MAP_ALLOC
	#endif	// _CRTDBG_MAP_ALLOC
	#if defined(MFC_VER)
		#define malloc(s)							_malloc_dbg(s, _NORMAL_BLOCK, __FILE__, __LINE__)
		#define calloc(c, s)						_calloc_dbg(c, s, _NORMAL_BLOCK, __FILE__, __LINE__)
		#define realloc(p, s)						_realloc_dbg(p, s, _NORMAL_BLOCK, __FILE__, __LINE__)
		#define _expand(p, s)						_expand_dbg(p, s, _NORMAL_BLOCK, __FILE__, __LINE__)
		#define free(p)								_free_dbg(p, _NORMAL_BLOCK)
		#define _msize(p)							_msize_dbg(p, _NORMAL_BLOCK)
		#define _aligned_malloc(s, a)				_aligned_malloc_dbg(s, a, __FILE__, __LINE__)
		#define _aligned_realloc(p, s, a)			_aligned_realloc_dbg(p, s, a, __FILE__, __LINE__)
		#define _aligned_offset_malloc(s, a, o)		_aligned_offset_malloc_dbg(s, a, o, __FILE__, __LINE__)
		#define _aligned_offset_realloc(p, s, a, o)	_aligned_offset_realloc_dbg(p, s, a, o, __FILE__, __LINE__)
		#define _aligned_free(p)					_aligned_free_dbg(p)
	#endif	// MFC_VER
	#define BASE_TRACE					CDebugSet::Trace
	#define BASE_MESSAGEBOX				CDebugSet::MessageBox
	#define	BASE_CHECK_MEMORYLEAK()		{																\
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);									\
		_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);												\
	)
	#define	BASE_ASSERT_CMP(pEXP)		{typedef char DUM[(pEXP)? 1: -1];}		// COMPILE-TIME (배열의 크기가 0 이하가 될 수 없다는 점을 이용, 조건이 틀리면 C2118 ('첨자가 음수입니다.') 에러 발생)
	#define	BASE_ASSERT(pEXP, pMSG)		{																\
		BOOL bEXP = static_cast<BOOL>(pEXP);															\
		if ((FALSE == bEXP) && CBaseSet::IsWindowStationVisible()) {								\
			DWORD dwLastError = ::GetLastError();														\
			/* Retry Button => 1, Abort => 종료, Ignore => -1*/											\
			INT	iRetVal = CDebugSet::CrtDbgReport(__FILE__, __LINE__, (const CHAR*)pMSG, #pEXP);		\
			::SetLastError(dwLastError);																\
			if (iRetVal == 1) {																			\
				::__debugbreak(); /*_asm { int 3 },	::_CrtDbgBreak();*/									\
			}																							\
		}																								\
	}
	#define BASE_VERIFY(pEXP, pTRUEVAL, pMSG)		BASE_ASSERT((pEXP) == (pTRUEVAL), pMSG)
	#define	BASE_RETURN(pEXP)						{BASE_ASSERT(0, "BASE_RETURN!");return(pEXP);}
	#define	BASE_RETURN_NONE						{BASE_ASSERT(0, "BASE_RETURN!");return;}
	#define	BASE_BREAK								{BASE_ASSERT(0, "BASE_CONTINUE!");break;}
	#define	BASE_CONTINUE							{BASE_ASSERT(0, "BASE_CONTINUE!");continue;}
	#define	BASE_LEAVE								{BASE_ASSERT(0, "BASE_LEAVE!");__leave;}
#else	// _DEBUG
	#define BASE_TRACE								/##/	// Release 모드 컴파일 시 한줄 주석으로 인하여 'C1075' (짝이 되기전에 파일이 끝남) 오류가 발생할 수 있으니 주의
	#define BASE_MESSAGEBOX							/##/	// Release 모드 컴파일 시 한줄 주석으로 인하여 'C1075' (짝이 되기전에 파일이 끝남) 오류가 발생할 수 있으니 주의
	#define	BASE_CHECK_MEMORYLEAK					/##/	// Release 모드 컴파일 시 한줄 주석으로 인하여 'C1075' (짝이 되기전에 파일이 끝남) 오류가 발생할 수 있으니 주의
	#define	BASE_ASSERT_CMP(pEXP)
	#define	BASE_ASSERT(pEXP, pMSG)
	#define BASE_VERIFY(pEXP, pTRUEVAL, pMSG)		((void)(pEXP))
	#define	BASE_RETURN(pEXP)						return(pEXP)
	#define	BASE_RETURN_NONE						return
	#define	BASE_BREAK								break
	#define	BASE_CONTINUE							continue
	#define	BASE_LEAVE								__leave
#endif	// _DEBUG

// HEAP 체크 관련 매크로 함수들
// P.S.>
/*
		- DCRT의 HEAP 체크를 활성화 하기 위해서는 반드시 BASE_DCRT_ON()을 한번 호출해야 함
		- CRTDBG_CHECK_ALWAYS_DF는 new/delete마다 모든 HEAP 메모리를 검사하고 검증하므로 Overhead가 너무 크므로 활성화시키지 않음
		- DCRT용 함수를 사용할려면 반드시 _CrtSetDbgFlag()를 호출해야 함
*/
#if defined(_DEBUG)
	#if (1 == BASE_DCRT_LEVEL)
		#define	BASE_DCRT_ON()		::_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF)
	#elif (2 == BASE_DCRT_LEVEL)
		#define	BASE_DCRT_ON()		::_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_DELAY_FREE_MEM_DF |		\
													 _CRTDBG_LEAK_CHECK_DF)
	#elif (3 == BASE_DCRT_LEVEL)
		#define	BASE_DCRT_ON()		::_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_DELAY_FREE_MEM_DF |		\
													 _CRTDBG_LEAK_CHECK_DF| _CRTDBG_CHECK_ALWAYS_DF)
	#else
		#define	BASE_DCRT_ON()
	#endif

	#if (0 < BASE_DCRT_LEVEL)
		#define	BASE_DCRT_OFF()		::_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF)
		#define	BASE_HEAP_CHECK()	BASE_ASSERT(::_CrtCheckMemory(),					"Under/Overwrite or Invalid Access!")
		#define	BASE_HEAP_PTR(pPTR)	BASE_ASSERT(!::_CrtIsValidHeapPointer(pPTR),		"Invalid Heap Pointer")
		#define	BASE_HEAP_LEAK()	BASE_ASSERT(!::_CrtDumpMemoryLeaks(),				"Memory Leakage!")
		#define	BASE_CODE(pCODE)	BASE_ASSERT(!::IsBadCodePtr((FARPROC)pCODE),		"Bad Code!");
	#else
		#define	BASE_DCRT_OFF()
		#define	BASE_HEAP_CHECK()		
		#define	BASE_HEAP_PTR(pPTR)
		#define	BASE_HEAP_LEAK()
		#define	BASE_CODE(pCODE)	BASE_ASSERT(NULL != pCODE,							"Bad Code!");
	#endif

	// Desc	: null도 check주고, pSZ가 0인지도 확인해 준다.
	#if (3 > BASE_DCRT_LEVEL)
		#define	BASE_READ(pMSG, pSZ)	BASE_ASSERT((NULL != pMSG) && (0 < pSZ),		"Can`t Read!");
		#define	BASE_WRITE(pBUF, pSZ)	BASE_ASSERT((NULL != pBUF) && (0 < pSZ),		"Can`t Write!");
		#define	BASE_STRINGA(pMSG, pSZ)	BASE_ASSERT((NULL != pMSG) && ('\0'  != pMSG[0]) && (0 < pSZ),										"Invalid String!");
		#define	BASE_STRINGW(pMSG, pSZ)	BASE_ASSERT((NULL != pMSG) && (L'\0' != pMSG[0]) && (0 < pSZ),										"Invalid String!");
	#else
		#define	BASE_READ(pMSG, pSZ)	BASE_ASSERT(!::IsBadReadPtr(pMSG, pSZ),			"Can`t Read!");
		#define	BASE_WRITE(pBUF, pSZ)	BASE_ASSERT(!::IsBadWritePtr(pBUF, pSZ),		"Can`t Write!");
		#define	BASE_STRINGA(pMSG, pSZ)	BASE_ASSERT(!::IsBadStringPtrA(pMSG, pSZ) && ('\0'  != pMSG[0]) && (::strlen(pMSG) <= (size_t)pSZ),	"Invalid String!");
		#define	BASE_STRINGW(pMSG, pSZ)	BASE_ASSERT(!::IsBadStringPtrW(pMSG, pSZ) && (L'\0' != pMSG[0]) && (::wcslen(pMSG) <= (size_t)pSZ),	"Invalid String!");
	#endif
#else	// _DEBUG
	#define	BASE_DCRT_ON()
	#define	BASE_DCRT_OFF()

	#define	BASE_HEAP_CHECK()
	#define	BASE_HEAP_PTR(pPTR)
	#define	BASE_HEAP_LEAK()
	
	#define	BASE_READ(pMSG, pSZ)
	#define	BASE_WRITE(pBUF, pSZ)
	#define	BASE_STRINGA(pMSG, pSZ)
	#define	BASE_STRINGW(pMSG, pSZ)
	#define	BASE_CODE(pCODE)
#endif	// _DEBUG

#if defined(_UNICODE)
#define BASE_STRING		BASE_STRINGW
#else	// _UNICODE
#define BASE_STRING		BASE_STRINGA
#endif	// _UNICODE


// 반복문 체크 매크로 함수들
#if defined(_DEBUG)
#define	_UNIQUE_VAR(pVAR)							__XONLY__VAR__ ## pVAR
#define	_WHILE(pCOND, pLIMIT, pVAR)																		\
	INT _UNIQUE_VAR(pVAR) = pLIMIT+1;																	\
	while((pCOND) &&																					\
	((--_UNIQUE_VAR(pVAR)>=0)?true:(_ASSERTE(!"Infinite loop!"),false)))
#define	WHILE(pCOND, pLIMIT)																			\
	_WHILE(pCOND, pLIMIT, __COUNTER__)
#define	_FOR(pINIT, pCOND, pLOOP, pLIMIT, pVAR)															\
	INT _UNIQUE_VAR(pVAR) = pLIMIT+1;																	\
	for (pINIT;																							\
	((pCOND)&&((--_UNIQUE_VAR(pVAR)>=0)?true:(_ASSERTE(!"Infinite loop!"),false)));						\
	pLOOP)
#define	FOR(pINIT, pCOND, pLOOP, pLIMIT)																\
	_FOR(pINIT, pCOND, pLOOP, pLIMIT, __COUNTER__)
#else	// _DEBUG
#define	WHILE(pCOND, pLIMIT)						while((pCOND))
#define	FOR(pINIT, pCOND, pLOOP, pLIMIT)			for (pINIT; pCOND; pLOOP)
#endif	// _DEBUG
#define	WHILE_INFINITE								for (;;)

// 객체 복사, 대입방지 매크로들 (사용 시 접근레벨 명시해야 함)
#define	DISABLE_COPY_ASSIGN(pCLS)											\
		private:															\
			pCLS(const pCLS##&)												\
			{																\
				BASE_ASSERT(0, "Don`t use Copy Constructor!");				\
			}																\
			pCLS##& operator=(const pCLS##&)								\
			{																\
				BASE_ASSERT(0, "Don`t use Assignment operator!");			\
				return(*this);												\
			}
#define	DISABLE_COPY_ASSIGN_DEFAULT(pCLS)									\
		public:																\
			pCLS()															\
			{																\
				/*BASE_ASSERT(0, "Don`t use Default constructor!");*/		\
			}																\
		private:															\
			pCLS(const pCLS##&)												\
			{																\
				BASE_ASSERT(0, "Don`t use Copy constructor!");				\
			}																\
			pCLS##& operator=(const pCLS##&)								\
			{																\
				BASE_ASSERT(0, "Don`t use Assignment operator!");			\
				return(*this);												\
			}

// 형 변환, 상속확인 관련 타입, 매크로 함수들
template<typename TDERV, typename TBASE>
class CCnvChker
{
private:
	typedef	char							TP_SMALL;
	typedef class { TP_SMALL DUM[2];}		TP_BIG;
	static  TP_SMALL	Test(TBASE);
	static  TP_BIG		Test(...);
	static  TDERV		MakeDerv();
public:
#pragma warning (disable:4244)	//possible loss of data
	enum {EV_ISCONV = (sizeof(Test(MakeDerv())) == sizeof(TP_SMALL))};
#pragma warning (default:4244)
	enum {EV_ISSAME = false};
};
template<typename TDERV>
class CCnvChker<TDERV, TDERV>
{
public:
	enum {EV_ISCONV = true};
	enum {EV_ISSAME = true};
};

#define	IS_SUPER_CLASS(pDERV, pBASE)	\
	(CCnvChker<const pDERV*, const pBASE*>::EV_ISCONV && !CCnvChker<const pDERV*, const void*>::EV_ISSAME)
#define	IS_SAME_CLASS(pDERV, pBASE)		\
	(IS_SUPER_CLASS(pDERV, pBASE) && CCnvChker<const pDERV, const pBASE>::EV_ISSAME)

// 기타 매크로 함수들
#if defined(_DEBUG)
#if !defined(NEW)
//#define NEW	::new(_CLIENT_BLOCK, __FILE__, __LINE__)	// new 로 동적할당 이후 해제되지 않은 코드의 파일, 라인 정보 출력됨 (_CLIENT_BLOCK 은 할당 메모리의 하위 16비트에 Client Block ID 를 설정한 후 이 ID별로 메모리를 추적할 수 있도록 하는 기능)
#define NEW		::new(_NORMAL_BLOCK, __FILE__, __LINE__)	// new 로 동적할당 이후 해제되지 않은 코드의 파일, 라인 정보 출력됨
#endif	// #if defined(NEW)
#define UNUSED(pVAL)				// 사용하지 않는 변수의 경우 RELEASE 모드일 때 선언되어 C4100 경고가 발생하지 않음
#else	// _DEBUG
#if !defined(NEW)
#define NEW		::new(std::nothrow)
#endif	// #if defined(NEW)
#define UNUSED(pVAL) pVAL			// 사용하지 않는 변수의 경우 RELEASE 모드일 때 선언되어 C4100 경고가 발생하지 않음
#endif	// _DEBUG
#define UNUSED_ALWAYS(pVAL) pVAL	// 사용하지 않는 변수의 경우 컴파일 옵션과 무관하게 선언되어 C4100 경고가 발생하지 않음


///////////////////////////////////////////////////////////////////
//	DECLARATION
///////////////////////////////////////////////////////////////////

//*---------------------------------------------------------------
// TYPE : CLASS
// NAME : CDebugSet
//*---------------------------------------------------------------
// DESC : System Base Library 의 디버깅을 위한 메서드 모음 클래스로 모두 static 멤버 메서드로 이루어짐
// PRGM : B4nFter
//*---------------------------------------------------------------
class CDebugSet
{
public:
	static VOID Trace(LPCTSTR lpszFormat, ...);
	static VOID MessageBox(HWND hWnd, UINT uType, LPCTSTR lpszCaption, LPCTSTR lpszFormat, ...);
	static VOID SetProgramName(LPTSTR lpszOutBuffer, INT nOutBufferSize, LPCTSTR lpszProgamName = NULL);
	static LPCTSTR GetProgramName();
	static LPCTSTR GetProgramPath();
#if defined(_DEBUG)
	inline static INT CrtDbgReport(LPCSTR lpszFileName, INT iLineNo, LPCSTR lpszMsg, LPCSTR lpszReason);
#endif	// _DEBUG

	static size_t StrLenA(LPCSTR pStr, DWORD pStrSz);
	static size_t StrLenW(LPCWSTR pStr, DWORD pStrSz);
	static FILE* FOpenA(LPCSTR pPath, LPCSTR pMode);
	static FILE* FOpenW(LPCWSTR pPath, LPCWSTR pMode);
	static BOOL IsSpaceAllA(LPCSTR pMSG);
	static BOOL IsSpaceAllW(LPCWSTR pMSG);

	template<typename TTYPE>
	static TTYPE TypeMax(const TTYPE /*pVar*/);
	template<typename TTYPE>
	static TTYPE TypeMin(const TTYPE /*pVar*/);
	template<typename TTYPE>
	static BOOL IsOverflow(const TTYPE pBase, const TTYPE pPlus);
	template<typename TTYPE>
	static BOOL IsUnderflow(const TTYPE pBase, const TTYPE pMinus);
	template<typename TTYPE, typename TVAL>
	static BOOL IsAssignable(const TTYPE /*pType*/, const TVAL pVal);
	template<typename TTYPE, typename TVAL>
	VOID Assign(TTYPE& pType, const TVAL pVal);
};


///////////////////////////////////////////////////////////////////
//	IMPLEMENTATION
///////////////////////////////////////////////////////////////////

//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CDebugSet::Trace
//*---------------------------------------------------------------
// DESC : 디버깅 정보를 TRACE 로 출력하는 메서드
// PARM :	1 . lpszFormat - 출력형식
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
inline VOID CDebugSet::Trace(LPCTSTR lpszFormat, ...)
{
	_ASSERTE(NULL != lpszFormat);

	if (!CBaseSet::IsWindowStationVisible()) {
		return;
	}

	TCHAR szBuffer[1024];
	va_list	szArgs;

	va_start(szArgs, lpszFormat);
#if	(_MSC_VER >= 1400)	// VS.NET 2005 이상
	_vsntprintf_s(szBuffer, COUNT_OF(szBuffer), _TRUNCATE, lpszFormat, szArgs);
#else	// #if	(_MSC_VER >= 1400)
	#pragma warning (disable:4995)
	_vsntprintf(szBuffer, COUNT_OF(szBuffer), lpszFormat, szArgs);
	#pragma warning (default:4995)
#endif	// #if	(_MSC_VER >= 1400)
	va_end(szArgs);

	::OutputDebugString(szBuffer);
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CDebugSet::MessageBox
//*---------------------------------------------------------------
// DESC : 디버깅 정보를 메시지 박스 로 출력하는 메서드
// PARM :	1 . hWnd - 표시할 윈도우 핸들
//			2 . uType - 생성 플래그
//			3 . lpszCaption - 타이틀
//			4 . lpszFormat - 출력형식
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
inline VOID CDebugSet::MessageBox(HWND hWnd, UINT uType, LPCTSTR lpszCaption, LPCTSTR lpszFormat, ...)
{
	_ASSERTE(NULL != lpszCaption);
	_ASSERTE(NULL != lpszFormat);

	if (!CBaseSet::IsWindowStationVisible()) {
		return;
	}

	TCHAR szBuffer[1024];
	va_list	szArgs;

	va_start(szArgs, lpszFormat);
#if	(_MSC_VER >= 1400)	// VS.NET 2005 이상
	_vsntprintf_s(szBuffer, COUNT_OF(szBuffer), _TRUNCATE, lpszFormat, szArgs);
#else	// #if	(_MSC_VER >= 1400)
	#pragma warning (disable:4995)
	_vsntprintf(szBuffer, COUNT_OF(szBuffer), lpszFormat, szArgs);
	#pragma warning (default:4995)
#endif	// #if	(_MSC_VER >= 1400)
	va_end(szArgs);

	::MessageBox(hWnd, szBuffer, lpszCaption, uType | MB_SERVICE_NOTIFICATION);
}


#if defined(_DEBUG)
inline INT CDebugSet::CrtDbgReport(LPCSTR lpszFileName, INT iLineNo, LPCSTR lpszMsg, LPCSTR lpszReason)
{
	_ASSERTE(NULL != lpszFileName);
	_ASSERTE(NULL != lpszMsg);
	_ASSERTE(NULL != lpszReason);

	return(::_CrtDbgReport(_CRT_ERROR, lpszFileName, iLineNo, NULL, "%s\n(Reason:%s)", lpszMsg, lpszReason));
}
#endif	// _DEBUG


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CDebugSet::SetProgramName
//*---------------------------------------------------------------
// DESC : 로그파일의 접두사가 되는 프로그램 이름을 세팅함
// PARM :	1 . lpszOutBuffer - 프로그램 이름을 출력할 버퍼
//			2 . nOutBufferSize - 프로그램 이름을 출력할 버퍼의 크기
//			3 . lpszProgamName - 사용자가 임의로 지정할 프로그램 이름
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
inline VOID CDebugSet::SetProgramName(LPTSTR lpszOutBuffer, INT nOutBufferSize, LPCTSTR lpszProgamName)
{
	if (!lpszProgamName) {
		TCHAR szDrive[MAX_PATH], szDir[MAX_PATH], szFileName[MAX_PATH], szFileExt[MAX_PATH];

		// 실행파일의 이름을 구함
		::GetModuleFileName(NULL, lpszOutBuffer, nOutBufferSize);

#if	(_MSC_VER >= 1400)	// VS.NET 2005 이상
		_tsplitpath_s(lpszOutBuffer, szDrive, szDir, szFileName, szFileExt);
		_tcsncpy_s(lpszOutBuffer, nOutBufferSize, szFileName, _TRUNCATE);
#else	// #if	(_MSC_VER >= 1400)
		_tsplitpath(lpszOutBuffer, szDrive, szDir, szFileName, szFileExt);
		_tcsncpy(lpszOutBuffer, szFileName, nOutBufferSize);
#endif	// #if	(_MSC_VER >= 1400)
	}
	else {
#if	(_MSC_VER >= 1400)	// VS.NET 2005 이상
		_tcsncpy_s(lpszOutBuffer, nOutBufferSize, lpszProgamName, _TRUNCATE);
#else	// #if	(_MSC_VER >= 1400)
		_tcsncpy(lpszOutBuffer, lpszProgamName, nOutBufferSize);
#endif	// #if	(_MSC_VER >= 1400)
	}
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CDebugSet::GetProgramName
//*---------------------------------------------------------------
// DESC : 현재 실행중인 프로그램의 이름을 반환
// PARM : N/A
// RETV : 현재 실행중인 프로그램의 이름
// PRGM : B4nFter
//*---------------------------------------------------------------
inline LPCTSTR CDebugSet::GetProgramName()
{
	static TCHAR szProgramName[MAX_PATH] = { _T('\0'), };

	if (_T('\0') == szProgramName[0]) {
		TCHAR szDrive[MAX_PATH], szDir[MAX_PATH], szFileName[MAX_PATH], szFileExt[MAX_PATH];

		// 실행파일의 이름을 구함
		::GetModuleFileName(NULL, szProgramName, COUNT_OF(szProgramName));

#if	(_MSC_VER >= 1400)	// VS.NET 2005 이상
		_tsplitpath_s(szProgramName, szDrive, szDir, szFileName, szFileExt);
		_tcsncpy_s(szProgramName, COUNT_OF(szProgramName), szFileName, _TRUNCATE);
#else	// #if	(_MSC_VER >= 1400)
		_tsplitpath(szProgramName, szDrive, szDir, szFileName, szFileExt);
		_tcsncpy(szProgramName, szFileName, COUNT_OF(szProgramName));
#endif	// #if	(_MSC_VER >= 1400)
	}
	return szProgramName;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CDebugSet::GetProgramPath
//*---------------------------------------------------------------
// DESC : 현재 실행중인 프로그램의 경로를 반환
// PARM : N/A
// RETV : 현재 실행중인 프로그램의 경로
// PRGM : B4nFter
//*---------------------------------------------------------------
inline LPCTSTR CDebugSet::GetProgramPath()
{
	static TCHAR szProgramPath[MAX_PATH] = { _T('\0'), };

	if (_T('\0') == szProgramPath[0]) {
		TCHAR szDrive[MAX_PATH], szDir[MAX_PATH], szFileName[MAX_PATH], szFileExt[MAX_PATH];

		// 실행파일의 이름을 구함
		::GetModuleFileName(NULL, szProgramPath, COUNT_OF(szProgramPath));

#if	(_MSC_VER >= 1400)	// VS.NET 2005 이상
		_tsplitpath_s(szProgramPath, szDrive, szDir, szFileName, szFileExt);
		_sntprintf_s(szProgramPath, COUNT_OF(szProgramPath), _T("%s%s"), szDrive, szDir);
#else	// #if	(_MSC_VER >= 1400)
		_tsplitpath(szProgramPath, szDrive, szDir, szFileName, szFileExt);
#pragma warning(disable:4995)
		_sntprintf(szProgramPath, _T("%s%s"), szDrive, szDir);
#pragma warning(default:4995)
#endif	// #if	(_MSC_VER >= 1400)
	}
	return szProgramPath;
}


#if defined(_DEBUG)
inline size_t CDebugSet::StrLenA(LPCSTR pStr, DWORD pStrSz)
{
	// 길이가 0인것은 정상으로 처리한다.
	size_t	aRv = 0;
	if ('\0' != *pStr)
	{
		if (!SUCCEEDED(::StringCchLengthA(pStr, ((STRSAFE_MAX_CCH<pStrSz)? STRSAFE_MAX_CCH:pStrSz), &aRv)))
		{
			BASE_ASSERT(0, "Invalid String!");
		}
	}
	return(aRv);
}


inline size_t CDebugSet::StrLenW(LPCWSTR pStr, DWORD pStrSz)
{
	// 길이가 0인것은 정상으로 처리한다.
	size_t	aRv = 0;
	if (L'\0' != *pStr)
	{
		if (!SUCCEEDED(::StringCchLengthW(pStr, ((STRSAFE_MAX_CCH<pStrSz)? STRSAFE_MAX_CCH:pStrSz), &aRv)))
		{
			BASE_ASSERT(0, "Invalid String!");
		}
	}
	return(aRv);
}


inline FILE* CDebugSet::FOpenA(LPCSTR pPath, LPCSTR pMode)
{
#if	(_MSC_VER >= 1400)	// VS.NET 2005 이상
	FILE*	aFp = NULL;
	::fopen_s(&aFp, pPath, pMode);
	return(aFp);
#else	// #if	(_MSC_VER >= 1400)
	return(::fopen(pPath, pMode));
#endif	// #if	(_MSC_VER >= 1400)
}


inline FILE* CDebugSet::FOpenW(LPCWSTR pPath, LPCWSTR pMode)
{
#if	(_MSC_VER >= 1400)	// VS.NET 2005 이상
	FILE*	aFp = NULL;
	::_wfopen_s(&aFp, pPath, pMode);
	return(aFp);
#else	// #if	(_MSC_VER >= 1400)
	return(::_wfopen(pPath, pMode));
#endif	// #if	(_MSC_VER >= 1400)
}
#endif	// _DEBUG


inline BOOL CDebugSet::IsSpaceAllA(LPCSTR pMSG)
{
	BASE_ASSERT(NULL != pMSG,	"Invalid!");

	while('\0' != (*pMSG))
	{
		if (!::isspace((*pMSG++)&0xFF))		// 0xFF => DCRT 내부의 assert 방지용
		{
			return FALSE;
		}
	}
	return TRUE;
}


inline BOOL CDebugSet::IsSpaceAllW(LPCWSTR pMSG)
{
	BASE_ASSERT(NULL != pMSG,	"Invalid!");

	while(L'\0' != (*pMSG))
	{
		if (!::iswspace((*pMSG++)&0xFFFF))	// 0xFFFF => DCRT 내부의 assert 방지용
		{
			return FALSE;
		}
	}
	return TRUE;
}

#if defined(max)
#undef max
#endif	// max
#if defined(min)
#undef min
#endif	// min

template<typename TTYPE>
inline TTYPE CDebugSet::TypeMax(const TTYPE /*pVar*/)
{
	return(std::numeric_limits<TTYPE>::max());
}


template<typename TTYPE>
inline TTYPE CDebugSet::TypeMin(const TTYPE /*pVar*/)
{
	return(std::numeric_limits<TTYPE>::min());
}


template<typename TTYPE>
inline BOOL CDebugSet::IsOverflow(const TTYPE pBase, const TTYPE pPlus)
{
//	BASE_ASSERT(0 <= pPlus,	"Invalid parameter!");
	return((pPlus <= 0)? FALSE:((pBase+pPlus)<pBase));
}


template<typename TTYPE>
inline BOOL CDebugSet::IsUnderflow(const TTYPE pBase, const TTYPE pMinus)
{
//	BASE_ASSERT(0 <= pMinus,	"Invalid parameter!");	//pMinus는 빼는 값이다.
	return((pMinus <= 0)? FALSE:(pBase<(pBase-pMinus)));
}


template<typename TTYPE, typename TVAL>
inline BOOL CDebugSet::IsAssignable(const TTYPE /*pType*/, const TVAL pVal)
{
	// singed와 unsinged를 비교하는 경우엔 오류가 발생하므로 할당이 가능한지 확인하는 경우는 일반적으로 INT64로 하면 모든 것이 해결될 듯 함
	return((std::numeric_limits<TTYPE>::min() <= static_cast<INT64>(pVal)) &&
		(static_cast<INT64>(pVal) <= std::numeric_limits<TTYPE>::max()));
}


template<typename TTYPE, typename TVAL>
inline VOID CDebugSet::Assign(TTYPE& pType, const TVAL pVal)
{
	BASE_ASSERT(IsAssignable(pType, pVal),	"Invalid!");
	pType = static_cast<TTYPE>(pVal);
}

