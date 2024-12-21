/*//===============================================================

	<< SAFE STRING MACROS >>

	FILE : SAFESTRINGMACROS.H
	DESC : 프로젝트 전반적으로 사용되는 안전문자열 CRT함수 관련 매크로들을 모아놓은 헤더파일
	INIT BUILT DATE : 2007. 03. 21
	LAST BUILT DATE : 2007. 05. 17

	Copyrightⓒ 2002~2009 B4nFter (b4nfter@gmail.com) and Co-Writers. All Rights Reserved.

*///===============================================================


#pragma once


///////////////////////////////////////////////////////////////////
//	INCLUDE
///////////////////////////////////////////////////////////////////

/*
	P.S.>
	- 본 헤더는 프로젝트 최상위 헤더에 포함되어야 함
	- 본 헤더의 프로젝트 포함 이전에 WINDOWS.H 등 기본헤더들이 이미 포함되어있다고 전제함 (따라서 본 헤더는 WINDOWS.H 등 기본헤더 제외시킴)
*/

// 아래 헤더들은 기존에는 추가되어있지 않는 것이고 안전문자열 CRT 함수와는 무관
#define	_CONVERSION_DONT_USE_THREAD_LOCALE	// ATL 7.0 부터 생긴 각 스레드별 코드페이지를 얻어오지 않고 기존 ATL 3.0 처럼 CP_THREAD_ACP 매크로 사용
#include <ATLBASE.H>	// ATL 7.0 문자열 변환 클래스와 매크로를 위함
#include <ATLCONV.H>


///////////////////////////////////////////////////////////////////
//	DEFINE
///////////////////////////////////////////////////////////////////

// 문자열 무결성 보증 매크로함수
#if defined(_DEBUG)
#define	CHECK_STRINGA(pMSG, pSZ)				DN_ASSERT((NULL != pMSG) && ('\0'  != pMSG[0]) && (0 < pSZ),		"Invalid string!");
#define	CHECK_STRINGW(pMSG, pSZ)				DN_ASSERT((NULL != pMSG) && (L'\0' != pMSG[0]) && (0 < pSZ),		"Invalid string!");
//#define	CHECK_STRINGA(pMSG, pSZ)			DN_ASSERT(!::IsBadStringPtrA(pMSG, pSZ) && ('\0'  != pMSG[0]) && (::strlen(pMSG) <= (size_t)pSZ),		"Invalid string!");
//#define	CHECK_STRINGW(pMSG, pSZ)			DN_ASSERT(!::IsBadStringPtrW(pMSG, pSZ) && (L'\0' != pMSG[0]) && (::wcslen(pMSG) <= (size_t)pSZ),		"Invalid string!");
#else	// #if defined(_DEBUG)
#define	CHECK_STRINGA(pMSG, pSZ)
#define	CHECK_STRINGW(pMSG, pSZ)
#endif	// #if defined(_DEBUG)
#define	MAKE_STRINGA(pSTR)						(pSTR[COUNT_OF(pSTR)-1] = '\0')
#define	MAKE_STRINGW(pSTR)						(pSTR[COUNT_OF(pSTR)-1] = L'\0')
#if defined(_UNICODE)
#define	CHECK_STRING							CHECK_STRINGW
#define	MAKE_STRING(pSTR)						MAKE_STRINGW(pSTR)
#else	// #if defined(_UNICODE)
#define	CHECK_STRING							CHECK_STRINGA
#define	MAKE_STRING(pSTR)						MAKE_STRINGA(pSTR)
#endif	// #if defined(_UNICODE)

// 안전문자열 CRT함수 관련 매크로함수
//#if defined(_DEBUG)
//#if !defined( _FINAL_BUILD )
#if 1
	//#if	(_MSC_VER >= 1400)	// VS.NET 2005 이상
	#if 1
		#define	STRCPYA(pDEST, pDESTSZ, pSTR)			{ int error_t = ::strcpy_s(pDEST, pDESTSZ, pSTR );				DN_ASSERT( 0 == error_t, "Invalid!"); }
		#define	STRCPYW(pDEST, pDESTSZ, pSTR)			{ int error_t = ::wcscpy_s(pDEST, pDESTSZ, pSTR);				DN_ASSERT( 0 == error_t, "Invalid!"); }
		#define	STRNCPYA(pDEST, pDESTSZ, pSTR)			{ int error_t = ::strncpy_s(pDEST, pDESTSZ, pSTR, pDESTSZ);		DN_ASSERT( 0 == error_t, "Invalid!"); }
		#define	STRNCPYW(pDEST, pDESTSZ, pSTR)			{ int error_t = ::wcsncpy_s(pDEST, pDESTSZ, pSTR, pDESTSZ);		DN_ASSERT( 0 == error_t, "Invalid!"); }
		#define	STRCATA(pDEST, pDESTSZ, pSTR)			{ int error_t = ::strcat_s(pDEST, pDESTSZ, pSTR);				DN_ASSERT( 0 == error_t, "Invalid!"); }
		#define	STRCATW(pDEST, pDESTSZ, pSTR)			{ int error_t = ::wcscat_s(pDEST, pDESTSZ, pSTR);				DN_ASSERT( 0 == error_t, "Invalid!"); }
		#define	STRNCATA(pDEST, pDESTSZ, pSTR)			{ int error_t = ::strncat_s(pDEST, pDESTSZ, pSTR, pDESTSZ);		DN_ASSERT( 0 == error_t, "Invalid!"); }
		#define	STRNCATW(pDEST, pDESTSZ, pSTR)			{ int error_t = ::wcsncat_s(pDEST, pDESTSZ, pSTR, pDESTSZ);		DN_ASSERT( 0 == error_t, "Invalid!"); }
		#define	STRUPA(pSTR, pSTRSZ)					{ int error_t = ::_strupr_s(pSTR, pSTRSZ);						DN_ASSERT( 0 == error_t, "Invalid!"); }
		#define	STRUPW(pSTR, pSTRSZ)					{ int error_t = ::_wcsupr_s(pSTR, pSTRSZ);						DN_ASSERT( 0 == error_t, "Invalid!"); }
		#define	STRLWA(pSTR, pSTRSZ)					{ int error_t = ::_strlwr_s(pSTR, pSTRSZ);						DN_ASSERT( 0 == error_t, "Invalid!"); }
		#define	STRLWW(pSTR, pSTRSZ)					{ int error_t = ::_wcslwr_s(pSTR, pSTRSZ);						DN_ASSERT( 0 == error_t, "Invalid!"); }
		#define	SPRINTFA								::sprintf_s
		#define	SPRINTFW								::swprintf_s
		#define	SNPRINTFA								::_snprintf_s
		#define	SNPRINTFW								::_snwprintf_s
		#define	VSNPRINTFA(pDEST, pDESTSZ, pFMT, pARG)	::_vsnprintf_s(pDEST, pDESTSZ, pDESTSZ-1, pFMT, pARG)
		#define	VSNPRINTFW(pDEST, pDESTSZ, pFMT, pARG)	::_vsnwprintf_s(pDEST, pDESTSZ, pDESTSZ-1, pFMT, pARG)
// 		#define	VSNPRINTFA(pDEST, pDESTSZ, pFMT, pARG)	DN_ASSERT(0 < ::_vsnprintf_s(pDEST, pDESTSZ, pDESTSZ-1, pFMT, pARG),	"Invalid!")
// 		#define	VSNPRINTFW(pDEST, pDESTSZ, pFMT, pARG)	DN_ASSERT(0 < ::_vsnwprintf_s(pDEST, pDESTSZ, pDESTSZ-1, pFMT, pARG),	"Invalid!")
		#define	SSCANFA									::sscanf
		#define	SSCANFW									::swscanf
	#else
		#define	STRCPYA(pDEST, pDESTSZ, pSTR)			{ HRESULT hResult = ::StringCchCopyA(pDEST, pDESTSZ, pSTR));			DN_ASSERT( SUCCEEDED(hResult) ), "Insufficient destination"); }
		#define	STRCPYW(pDEST, pDESTSZ, pSTR)			{ HRESULT hResult = ::StringCchCopyW(pDEST, pDESTSZ, pSTR));			DN_ASSERT( SUCCEEDED(hResult) ), "Insufficient destination"); }
		#define	STRNCPYA(pDEST, pDESTSZ, pSTR)			{ HRESULT hResult = ::StringCchCopyNA(pDEST, pDESTSZ, pSTR, pDESTSZ));	DN_ASSERT( SUCCEEDED(hResult) ), "Insufficient destination"); }
		#define	STRNCPYW(pDEST, pDESTSZ, pSTR)			{ HRESULT hResult = ::StringCchCopyNW(pDEST, pDESTSZ, pSTR, pDESTSZ));	DN_ASSERT( SUCCEEDED(hResult) ), "Insufficient destination"); }
		#define	STRCATA(pDEST, pDESTSZ, pSTR)			{ HRESULT hResult = ::StringCchCatA(pDEST, pDESTSZ, pSTR));				DN_ASSERT( SUCCEEDED(hResult) ), "Insufficient destination"); }
		#define	STRCATW(pDEST, pDESTSZ, pSTR)			{ HRESULT hResult = ::StringCchCatW(pDEST, pDESTSZ, pSTR));				DN_ASSERT( SUCCEEDED(hResult) ), "Insufficient destination"); }
		#define	STRNCATA(pDEST, pDESTSZ, pSTR)			{ HRESULT hResult = ::StringCchCatNA(pDEST, pDESTSZ, pSTR, pDESTSZ));	DN_ASSERT( SUCCEEDED(hResult) ), "Insufficient destination"); }
		#define	STRNCATW(pDEST, pDESTSZ, pSTR)			{ HRESULT hResult = ::StringCchCatNW(pDEST, pDESTSZ, pSTR, pDESTSZ));	DN_ASSERT( SUCCEEDED(hResult) ), "Insufficient destination"); }
		#define	STRUPA(pSTR, pSTRSZ)					::_strupr(pSTR)
		#define	STRUPW(pSTR, pSTRSZ)					::_wcsupr(pSTR)
		#define	STRLWA(pSTR, pSTRSZ)					::_strlwr(pSTR)
		#define	STRLWW(pSTR, pSTRSZ)					::_wcslwr(pSTR)
		#define	SPRINTFA								::sprintf
		#define	SPRINTFW								::swprintf
		#define	SNPRINTFA								::StringCchPrintfA
		#define	SNPRINTFW								::StringCchPrintfW
		#define	VSNPRINTFA(pDEST, pDESTSZ, pFMT, pARG)	{ HRESULT hResult = ::StringCchVPrintfA(pDEST, pDESTSZ, pFMT, pARG));	DN_ASSERT( SUCCEEDED(hResult) ), "Insufficient destination"); }
		#define	VSNPRINTFW(pDEST, pDESTSZ, pFMT, pARG)	{ HRESULT hResult = ::StringCchVPrintfW(pDEST, pDESTSZ, pFMT, pARG));	DN_ASSERT( SUCCEEDED(hResult) ), "Insufficient destination"); }
		#define	SSCANFA									::sscanf
		#define	SSCANFW									::swscanf
	#endif
// 	#define	STRLENA(pSTR, pSTRSZ)						CDebugUtil::StrLenA(pSTR, pSTRSZ)
// 	#define	STRLENW(pSTR, pSTRSZ)						CDebugUtil::StrLenW(pSTR, pSTRSZ)
	#define	FOPENA(pPATH, pMODE)						CDebugUtil::FOpenA(pPATH, pMODE)
	#define	FOPENW(pPATH, pMODE)						CDebugUtil::FOpenW(pPATH, pMODE)
	#define SPLITPATHA(pPATH, pDRV, pDRVSZ, pDIR, pDIRSZ, pFNM, pFNMSZ, pEXT, pEXTSZ)	::_splitpath_s(pPATH, pDRV, pDRVSZ, pDIR, pDIRSZ, pFNM, pFNMSZ, pEXT, pEXTSZ)
	#define SPLITPATHW(pPATH, pDRV, pDRVSZ, pDIR, pDIRSZ, pFNM, pFNMSZ, pEXT, pEXTSZ)	::_wsplitpath_s(pPATH, pDRV, pDRVSZ, pDIR, pDIRSZ, pFNM, pFNMSZ, pEXT, pEXTSZ)
#else	// _DEBUG
	#define	STRCPYA(pDEST, pDESTSZ, pSTR)				::strcpy(pDEST, pSTR)
	#define	STRCPYW(pDEST, pDESTSZ, pSTR)				::wcscpy(pDEST, pSTR)
	#define	STRNCPYA(pDEST, pDESTSZ, pSTR)				::strncpy(pDEST, pSTR, pDESTSZ);pDEST[pDESTSZ-1]='\0';
	#define	STRNCPYW(pDEST, pDESTSZ, pSTR)				::wcsncpy(pDEST, pSTR, pDESTSZ);pDEST[pDESTSZ-1]=L'\0';
	#define	STRCATA(pDEST, pDESTSZ, pSTR)				::strcat(pDEST, pSTR)
	#define	STRCATW(pDEST, pDESTSZ, pSTR)				::wcscat(pDEST, pSTR)
	#define	STRNCATA(pDEST, pDESTSZ, pSTR)				::strncat(pDEST, pSTR, pDESTSZ);pDEST[pDESTSZ-1]='\0';
	#define	STRNCATW(pDEST, pDESTSZ, pSTR)				::wcsncat(pDEST, pSTR, pDESTSZ);pDEST[pDESTSZ-1]=L'\0';
	#define	STRUPA(pSTR, pSTRSZ)						::_strupr(pSTR)
	#define	STRUPW(pSTR, pSTRSZ)						::_wcsupr(pSTR)
	#define	STRLWA(pSTR, pSTRSZ)						::_strlwr(pSTR)
	#define	STRLWW(pSTR, pSTRSZ)						::_wcslwr(pSTR)
	// Warn	: null termination 이 안됨
	#define	SPRINTFA									::sprintf
	#define	SPRINTFW									::swprintf
	#define	SNPRINTFA									::_snprintf
	#define	SNPRINTFW									::_snwprintf
	#define	VSNPRINTFA(pDEST,pDESTSZ,pFMT,pARG)			::_vsnprintf(pDEST, pDESTSZ, pFMT, pARG);pDEST[pDESTSZ-1]='\0';
	#define	VSNPRINTFW(pDEST,pDESTSZ,pFMT,pARG)			::_vsnwprintf(pDEST, pDESTSZ, pFMT, pARG);pDEST[pDESTSZ-1]=L'\0';
	#define	SSCANFA										::sscanf
	#define	SSCANFW										::swscanf

// 	#define	STRLENA(pSTR, pSTRSZ)						::strlen(pSTR)
// 	#define	STRLENW(pSTR, pSTRSZ)						::wcslen(pSTR)
	#define	FOPENA(pPATH, pMODE)						::fopen(pPATH, pMODE)
	#define	FOPENW(pPATH, pMODE)						::_wfopen(pPATH, pMODE)
	#define SPLITPATHA(pPATH, pDRV, pDRVSZ, pDIR, pDIRSZ, pFNM, pFNMSZ, pEXT, pEXTSZ)	::_splitpath(pPATH, pDRV, pDIR, pFNM, pEXT)
	#define SPLITPATHW(pPATH, pDRV, pDRVSZ, pDIR, pDIRSZ, pFNM, pFNMSZ, pEXT, pEXTSZ)	::_wsplitpath(pPATH, pDRV, pDIR, pFNM, pEXT)
#endif	// _DEBUG

#if defined(_UNICODE)
	#define	STRCPY		STRCPYW
	#define	STRNCPY		STRNCPYW
	#define	STRCAT		STRCATW
	#define	STRNCAT		STRNCATW
	#define	STRUP		STRUPW
	#define	STRLW		STRLWW
	#define	SPRINTF		SPRINTFW
	#define	SNPRINTF	SNPRINTFW
	#define	VSNPRINTF	VSNPRINTFW
	#define	SSCANF		SSCANFW
//	#define	STRLEN		STRLENW
	#define	FOPEN		FOPENW
	#define SPLITPATH	SPLITPATHW
#else	// _UNICODE
	#define	STRCPY		STRCPYA
	#define	STRNCPY		STRNCPYA
	#define	STRCAT		STRCATA
	#define	STRNCAT		STRNCATA
	#define	STRUP		STRUPA
	#define	STRLW		STRLWA
	#define	SPRINTF		SPRINTFA
	#define	SNPRINTF	SNPRINTFA
	#define	VSNPRINTF	VSNPRINTFA
	#define	SSCANF		SSCANFA
//	#define	STRLEN		STRLENA
	#define	FOPEN		FOPENA
	#define SPLITPATH	SPLITPATHA
#endif	// _UNICODE
