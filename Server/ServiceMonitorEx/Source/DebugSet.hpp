/*//===============================================================

	<< DEBUG >>

	PRGM : B4nFter, milkji

	FILE : DEBUGSET.HPP
	DESC : ������� ���� �޼��� ����
	INIT BUILT DATE : 2005. 04. 28
	LAST BUILT DATE : 2006. 03. 07

	Copyright�� 2002~2009 B4nFter (b4nfter@gmail.com) and Co-Writers. All Rights Reserved.

*///===============================================================


#pragma once


///////////////////////////////////////////////////////////////////
//	INCLUDE
///////////////////////////////////////////////////////////////////

// P.S.> SYSBASEBASE.H �� ���� ������ (���� �� ����� WINDOWS.H �� �⺻��� ���ܽ�Ŵ)

#if defined(_DEBUG)
#include <STRSAFE.H>
#include <CRTDBG.H>		// DCRT�� ���ؼ� ���� �������� include �Ǿ�� ��
#endif	// _DEBUG


///////////////////////////////////////////////////////////////////
//	DEFINE
///////////////////////////////////////////////////////////////////

/*
	<VISUAL STUDIO ���� ����>
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
	<�޸� ���°� ���� (WIN32 ����)>

	0xCDCDCDCD	: HEAP �� �Ҵ�Ǿ����� �ʱ�ȭ ���� ����
	0xDDDDDDDD	: HEAP ���� ������ ����
	0xFDFDFDFD	: HEAP �޸��� ��迡 �ڵ����� ��ġ�ϴ� 'NoMansLand' ��Ÿ���� ���� ��������� �ʵ�. ���� ����ٸ� �Ƹ��� �迭 ����� ���� ��������
	0xCCCCCCCC	: STACK �� ��ġ�Ǿ����� �ʱ�ȭ ���� ����
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

// ������ �ּ� ��ũ�� �Լ�
// P.S.>
/*
	- ������ Ÿ�ӿ� Ʈ���̽� â�� ���� �ּ�
	- ���ϸ�� �ٹ�ȣ�� ����ϸ� Ŭ�� �� �̵�
	- #pragma COMPILE_COMMENT("�ּ�����") �� ���
*/
#define __COMPILE_COMMENTX__(pMSG)		#pMSG
#define __COMPILE_COMMENT__(pMSG)		__COMPILE_COMMENTX__(pMSG) 
#define	COMPILE_COMMENT(pCMT)			message(__FILE__ "(" __COMPILE_COMMENT__(__LINE__) ") : "pCMT)

// ����� ���� ��ũ�� �Լ���
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
	#define	BASE_ASSERT_CMP(pEXP)		{typedef char DUM[(pEXP)? 1: -1];}		// COMPILE-TIME (�迭�� ũ�Ⱑ 0 ���ϰ� �� �� ���ٴ� ���� �̿�, ������ Ʋ���� C2118 ('÷�ڰ� �����Դϴ�.') ���� �߻�)
	#define	BASE_ASSERT(pEXP, pMSG)		{																\
		BOOL bEXP = static_cast<BOOL>(pEXP);															\
		if ((FALSE == bEXP) && CBaseSet::IsWindowStationVisible()) {								\
			DWORD dwLastError = ::GetLastError();														\
			/* Retry Button => 1, Abort => ����, Ignore => -1*/											\
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
	#define BASE_TRACE								/##/	// Release ��� ������ �� ���� �ּ����� ���Ͽ� 'C1075' (¦�� �Ǳ����� ������ ����) ������ �߻��� �� ������ ����
	#define BASE_MESSAGEBOX							/##/	// Release ��� ������ �� ���� �ּ����� ���Ͽ� 'C1075' (¦�� �Ǳ����� ������ ����) ������ �߻��� �� ������ ����
	#define	BASE_CHECK_MEMORYLEAK					/##/	// Release ��� ������ �� ���� �ּ����� ���Ͽ� 'C1075' (¦�� �Ǳ����� ������ ����) ������ �߻��� �� ������ ����
	#define	BASE_ASSERT_CMP(pEXP)
	#define	BASE_ASSERT(pEXP, pMSG)
	#define BASE_VERIFY(pEXP, pTRUEVAL, pMSG)		((void)(pEXP))
	#define	BASE_RETURN(pEXP)						return(pEXP)
	#define	BASE_RETURN_NONE						return
	#define	BASE_BREAK								break
	#define	BASE_CONTINUE							continue
	#define	BASE_LEAVE								__leave
#endif	// _DEBUG

// HEAP üũ ���� ��ũ�� �Լ���
// P.S.>
/*
		- DCRT�� HEAP üũ�� Ȱ��ȭ �ϱ� ���ؼ��� �ݵ�� BASE_DCRT_ON()�� �ѹ� ȣ���ؾ� ��
		- CRTDBG_CHECK_ALWAYS_DF�� new/delete���� ��� HEAP �޸𸮸� �˻��ϰ� �����ϹǷ� Overhead�� �ʹ� ũ�Ƿ� Ȱ��ȭ��Ű�� ����
		- DCRT�� �Լ��� ����ҷ��� �ݵ�� _CrtSetDbgFlag()�� ȣ���ؾ� ��
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

	// Desc	: null�� check�ְ�, pSZ�� 0������ Ȯ���� �ش�.
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


// �ݺ��� üũ ��ũ�� �Լ���
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

// ��ü ����, ���Թ��� ��ũ�ε� (��� �� ���ٷ��� ����ؾ� ��)
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

// �� ��ȯ, ���Ȯ�� ���� Ÿ��, ��ũ�� �Լ���
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

// ��Ÿ ��ũ�� �Լ���
#if defined(_DEBUG)
#if !defined(NEW)
//#define NEW	::new(_CLIENT_BLOCK, __FILE__, __LINE__)	// new �� �����Ҵ� ���� �������� ���� �ڵ��� ����, ���� ���� ��µ� (_CLIENT_BLOCK �� �Ҵ� �޸��� ���� 16��Ʈ�� Client Block ID �� ������ �� �� ID���� �޸𸮸� ������ �� �ֵ��� �ϴ� ���)
#define NEW		::new(_NORMAL_BLOCK, __FILE__, __LINE__)	// new �� �����Ҵ� ���� �������� ���� �ڵ��� ����, ���� ���� ��µ�
#endif	// #if defined(NEW)
#define UNUSED(pVAL)				// ������� �ʴ� ������ ��� RELEASE ����� �� ����Ǿ� C4100 ��� �߻����� ����
#else	// _DEBUG
#if !defined(NEW)
#define NEW		::new(std::nothrow)
#endif	// #if defined(NEW)
#define UNUSED(pVAL) pVAL			// ������� �ʴ� ������ ��� RELEASE ����� �� ����Ǿ� C4100 ��� �߻����� ����
#endif	// _DEBUG
#define UNUSED_ALWAYS(pVAL) pVAL	// ������� �ʴ� ������ ��� ������ �ɼǰ� �����ϰ� ����Ǿ� C4100 ��� �߻����� ����


///////////////////////////////////////////////////////////////////
//	DECLARATION
///////////////////////////////////////////////////////////////////

//*---------------------------------------------------------------
// TYPE : CLASS
// NAME : CDebugSet
//*---------------------------------------------------------------
// DESC : System Base Library �� ������� ���� �޼��� ���� Ŭ������ ��� static ��� �޼���� �̷����
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
// DESC : ����� ������ TRACE �� ����ϴ� �޼���
// PARM :	1 . lpszFormat - �������
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
#if	(_MSC_VER >= 1400)	// VS.NET 2005 �̻�
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
// DESC : ����� ������ �޽��� �ڽ� �� ����ϴ� �޼���
// PARM :	1 . hWnd - ǥ���� ������ �ڵ�
//			2 . uType - ���� �÷���
//			3 . lpszCaption - Ÿ��Ʋ
//			4 . lpszFormat - �������
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
#if	(_MSC_VER >= 1400)	// VS.NET 2005 �̻�
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
// DESC : �α������� ���λ簡 �Ǵ� ���α׷� �̸��� ������
// PARM :	1 . lpszOutBuffer - ���α׷� �̸��� ����� ����
//			2 . nOutBufferSize - ���α׷� �̸��� ����� ������ ũ��
//			3 . lpszProgamName - ����ڰ� ���Ƿ� ������ ���α׷� �̸�
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
inline VOID CDebugSet::SetProgramName(LPTSTR lpszOutBuffer, INT nOutBufferSize, LPCTSTR lpszProgamName)
{
	if (!lpszProgamName) {
		TCHAR szDrive[MAX_PATH], szDir[MAX_PATH], szFileName[MAX_PATH], szFileExt[MAX_PATH];

		// ���������� �̸��� ����
		::GetModuleFileName(NULL, lpszOutBuffer, nOutBufferSize);

#if	(_MSC_VER >= 1400)	// VS.NET 2005 �̻�
		_tsplitpath_s(lpszOutBuffer, szDrive, szDir, szFileName, szFileExt);
		_tcsncpy_s(lpszOutBuffer, nOutBufferSize, szFileName, _TRUNCATE);
#else	// #if	(_MSC_VER >= 1400)
		_tsplitpath(lpszOutBuffer, szDrive, szDir, szFileName, szFileExt);
		_tcsncpy(lpszOutBuffer, szFileName, nOutBufferSize);
#endif	// #if	(_MSC_VER >= 1400)
	}
	else {
#if	(_MSC_VER >= 1400)	// VS.NET 2005 �̻�
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
// DESC : ���� �������� ���α׷��� �̸��� ��ȯ
// PARM : N/A
// RETV : ���� �������� ���α׷��� �̸�
// PRGM : B4nFter
//*---------------------------------------------------------------
inline LPCTSTR CDebugSet::GetProgramName()
{
	static TCHAR szProgramName[MAX_PATH] = { _T('\0'), };

	if (_T('\0') == szProgramName[0]) {
		TCHAR szDrive[MAX_PATH], szDir[MAX_PATH], szFileName[MAX_PATH], szFileExt[MAX_PATH];

		// ���������� �̸��� ����
		::GetModuleFileName(NULL, szProgramName, COUNT_OF(szProgramName));

#if	(_MSC_VER >= 1400)	// VS.NET 2005 �̻�
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
// DESC : ���� �������� ���α׷��� ��θ� ��ȯ
// PARM : N/A
// RETV : ���� �������� ���α׷��� ���
// PRGM : B4nFter
//*---------------------------------------------------------------
inline LPCTSTR CDebugSet::GetProgramPath()
{
	static TCHAR szProgramPath[MAX_PATH] = { _T('\0'), };

	if (_T('\0') == szProgramPath[0]) {
		TCHAR szDrive[MAX_PATH], szDir[MAX_PATH], szFileName[MAX_PATH], szFileExt[MAX_PATH];

		// ���������� �̸��� ����
		::GetModuleFileName(NULL, szProgramPath, COUNT_OF(szProgramPath));

#if	(_MSC_VER >= 1400)	// VS.NET 2005 �̻�
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
	// ���̰� 0�ΰ��� �������� ó���Ѵ�.
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
	// ���̰� 0�ΰ��� �������� ó���Ѵ�.
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
#if	(_MSC_VER >= 1400)	// VS.NET 2005 �̻�
	FILE*	aFp = NULL;
	::fopen_s(&aFp, pPath, pMode);
	return(aFp);
#else	// #if	(_MSC_VER >= 1400)
	return(::fopen(pPath, pMode));
#endif	// #if	(_MSC_VER >= 1400)
}


inline FILE* CDebugSet::FOpenW(LPCWSTR pPath, LPCWSTR pMode)
{
#if	(_MSC_VER >= 1400)	// VS.NET 2005 �̻�
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
		if (!::isspace((*pMSG++)&0xFF))		// 0xFF => DCRT ������ assert ������
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
		if (!::iswspace((*pMSG++)&0xFFFF))	// 0xFFFF => DCRT ������ assert ������
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
//	BASE_ASSERT(0 <= pMinus,	"Invalid parameter!");	//pMinus�� ���� ���̴�.
	return((pMinus <= 0)? FALSE:(pBase<(pBase-pMinus)));
}


template<typename TTYPE, typename TVAL>
inline BOOL CDebugSet::IsAssignable(const TTYPE /*pType*/, const TVAL pVal)
{
	// singed�� unsinged�� ���ϴ� ��쿣 ������ �߻��ϹǷ� �Ҵ��� �������� Ȯ���ϴ� ���� �Ϲ������� INT64�� �ϸ� ��� ���� �ذ�� �� ��
	return((std::numeric_limits<TTYPE>::min() <= static_cast<INT64>(pVal)) &&
		(static_cast<INT64>(pVal) <= std::numeric_limits<TTYPE>::max()));
}


template<typename TTYPE, typename TVAL>
inline VOID CDebugSet::Assign(TTYPE& pType, const TVAL pVal)
{
	BASE_ASSERT(IsAssignable(pType, pVal),	"Invalid!");
	pType = static_cast<TTYPE>(pVal);
}

