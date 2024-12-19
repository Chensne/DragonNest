/*//===============================================================

	<< COMMON MACROS >>

	PRGM : B4nFter, milkji

	FILE : COMMONMACROS.HPP
	DESC : System Base 라이브러리 전반적으로 사용되는 일반적인 매크로들을 모아놓은 헤더파일
	INIT BUILT DATE : 2005. 10. 15
	LAST BUILT DATE : 2007. 11. 19

	Copyrightⓒ 2002~2009 B4nFter (b4nfter@gmail.com) and Co-Writers. All Rights Reserved.

*///===============================================================


#pragma once


///////////////////////////////////////////////////////////////////
//	INCLUDE
///////////////////////////////////////////////////////////////////

// P.S.> SYSBASEBASE.H 에 의해 참조됨 (따라서 본 헤더는 WINDOWS.H 등 기본헤더 제외시킴)


///////////////////////////////////////////////////////////////////
//	DEFINE
///////////////////////////////////////////////////////////////////

// 객체 해제 관련 매크로함수
#if !defined(SAFE_DELETE)
	#define SAFE_DELETE(p)						{ if (p) { delete(p); (p) = NULL; } }
#endif	// SAFE_DELETE
#if !defined(SAFE_DELETEARRAY)
	#define SAFE_DELETEARRAY(p)					{ if (p) { delete[](p); (p) = NULL; } }
#endif	// SAFE_DELETEARRAY
#if !defined(SAFE_DELETEOBJECT)
	#define SAFE_DELETEOBJECT(p)				{ if (p) { ::DeleteObject(p); (p) = NULL; } }
#endif	// SAFE_DELETEOBJECT
#if !defined(SAFE_RELEASE)
	#define SAFE_RELEASE(p)						{ if (p) { (p)->Release(); (p) = NULL; } }
#endif	// SAFE_RELEASE
#if !defined(SAFE_CLOSEHANDLE)
	#define SAFE_CLOSEHANDLE(p)					{ if (p !=INVALID_HANDLE_VALUE) { ::CloseHandle(p); (p) = INVALID_HANDLE_VALUE; } }
#endif	// SAFE_CLOSEHANDLE
#if !defined(SAFE_CLOSESOCKET)
	#define SAFE_CLOSESOCKET(p)					{ if (p !=INVALID_SOCKET) { ::closesocket(p); (p) = INVALID_SOCKET; } }
#endif	// SAFE_CLOSESOCKET
#if !defined(SAFE_FCLOSE)
	#define SAFE_FCLOSE(p)						{ if (p) { ::fclose(p); (p) = NULL; } }
#endif	// SAFE_FCLOSE
#if !defined(SAFE_DESTROYWINDOW)
	#define SAFE_DESTROYWINDOW(p)				{ if (p) { ::DestroyWindow(p); (p) = NULL; } }
#endif	// SAFE_DESTROYWINDOW
#if !defined(SAFE_DESTROYMENU)
	#define SAFE_DESTROYMENU(p)					{ if (p) { ::DestroyMenu(p); (p) = NULL; } }
#endif	// SAFE_DESTROYMENU
#if !defined(SAFE_FREE)
	#define SAFE_FREE(p)						{ if (p) { ::free(p); (p) = NULL; } }
#endif	// SAFE_FREE


// 비교 매크로함수
#if !defined(MIN)
	#define MIN(M_A, M_B)						(((M_A)>(M_B))	?	(M_B):(M_A))
#endif	// MIN
#if !defined(MAX)
	#define MAX(M_A, M_B)						(((M_A)>(M_B))	?	(M_A):(M_B))
#endif	// MAX
#if !defined(EQUARL)
	#define EQUARL(M_A, M_B)					(((M_A)==(M_B))	?	TRUE:FALSE;)
#endif	// EQUARL


// 범위특정 매크로함수
#define CHECK_LIMIT(C_VALUE, C_MAX)				(C_VALUE<0?FALSE:C_VALUE>(C_MAX-1)?FALSE:TRUE)		// 0 이상 C_MAX 미만
#define CHECK_LIMIT_NONZERO(C_VALUE, C_MAX)		(C_VALUE<=0?FALSE:C_VALUE>(C_MAX-1)?FALSE:TRUE)		// 0 초과 C_MAX 미만
#define	CHECK_RANGE(C_VALUE, C_MIN, C_MAX)		(C_VALUE<C_MIN?FALSE:C_VALUE>C_MAX?FALSE:TRUE)		// C_MIN 이상 C_MAX 미만


// 범위제한 매크로함수
#define CUT_LIMIT(C_VALUE, C_MIN, C_MAX)		(C_VALUE<C_MIN?C_MIN:C_VALUE>C_MAX?C_MAX:C_VALUE)


// 배열 관련 매크로함수
#define COUNT_OF(ARRAY)							(sizeof(ARRAY)/sizeof(ARRAY[0]))


// 숫자조합 관련 매크로함수
#define SWAP(LT, RT)							((LT)^=((RT)^=((LT)^=(RT))))
#define LOLONG(l)								((DWORD)((UINT64)(l) & 0xFFFFFFFF))
#define HILONG(l)								((DWORD)((UINT64)(l) >> 32))
#define MAKELONGLONG(A, B)						((INT64)(((DWORD)((UINT64)(A) & 0xFFFFFFFF)) | ((INT64)((DWORD)((UINT64)(B) & 0xFFFFFFFF))) << 32))


// 문자열 관련 매크로함수
#define CONV_STRINGA(pNM)						(#pNM)
#define CONV_STRINGW(pNM)						(L#pNM)
#if defined(_UNICODE)
#define CONV_STRING								CONV_STRINGW
#else	// _UNICODE
#define CONV_STRING								CONV_STRINGA
#endif	// _UNICODE


// 컴파일 모드 관련 매크로함수
#if defined(_DEBUG)
#define	MODE_DBG(pDBG)							pDBG
#if	(_MSC_VER >= 1400)
#define	MODE_DBG_EX(pMST, pDBG)					pMST, pDBG
#else	// #if	(_MSC_VER >= 1400)
#define	MODE_DBG_EX(pMST, pDBG)					pMST
#endif	//#if	(_MSC_VER >= 1400)
#define	MODE_CMP(pDBG, pREL)					pDBG
#else	// _DEBUG
#define	MODE_DBG(pDBG)					
#define	MODE_DBG_EX(pMST, pDBG)					pMST
//#define	MODE_DBG_EX(pMST, pDBG)					pMST, pDBG
#define	MODE_CMP(pDBG, pREL)					pREL
#endif	// _DEBUG

#if defined(_MT)
#define	MODE_MT(pSG, pMT)						pMT
#else	// _MT
#define	MODE_MT(pSG, pMT)						pSG
#endif	// _MT

#if defined(_UNICODE)
#define	MODE_CHARSET(pMBCS, pUNCD)				pUNCD
#else	// _UNICODE
#define	MODE_CHARSET(pMBCS, pUNCD)				pMBCS
#endif	// _UNICODE


// 표준 문자열 관련
#if defined(_UNICODE)
#define	_tstring				std::wstring
#else	// _UNICODE
#define	_tstring				std::string
#endif	// _UNICODE


// 표준 파일입출력 관련
#if defined(_UNICODE)
#define	_tifstream				std::wifstream
#define	_tofstream				std::wofstream
#else	// _UNICODE
#define	_tifstream				std::ifstream
#define	_tofstream				std::ofstream
#endif	// _UNICODE


// 숫자 → 문자열변환 (USES_CONVERSION 을 전방에 미리 선언해야 함 - ATLDBCLI.H)
#define	I2A(I) (\
	(_lpa = (LPCSTR)alloca(12)),\
	(_itoa(I, (LPSTR)_lpa, 10)),\
	(_lpa))

#define	I2W(I) (\
	(_lpw = (LPCWSTR)alloca(24)),\
	(_itow(I, (LPWSTR)_lpw, 10)),\
	(_lpw))

#if defined(_UNICODE)
	#define	I2T					I2W
#else	// _UNICODE
	#define	I2T					I2A
#endif	// _UNICODE


// 64비트 관련 (X64 - _AMD64_ / IA64 - _IA64_)
#if defined(_AMD64_) || defined(_IA64_)
typedef	DWORD64					PTRTYPE;
#else	// _AMD64_ / _IA64_
typedef	DWORD					PTRTYPE;
#endif	// _AMD64_ / _IA64_

