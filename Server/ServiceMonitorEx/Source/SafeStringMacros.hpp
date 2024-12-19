/*//===============================================================

	<< SAFE STRING MACROS >>

	PRGM : B4nFter, milkji

	FILE : SAFESTRINGMACROS.HPP
	DESC : System Base 라이브러리 전반적으로 사용되는 안전문자열 CRT함수 관련 매크로들을 모아놓은 헤더파일
	INIT BUILT DATE : 2007. 03. 21
	LAST BUILT DATE : 2007. 05. 17

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

// 문자열 무결성 보증 매크로함수
#if defined(_DEBUG)
#define	CHECK_STRINGA(pMSG, pSZ)				BASE_ASSERT((NULL != pMSG) && ('\0'  != pMSG[0]) && (0 < pSZ),		"Invalid string!");
#define	CHECK_STRINGW(pMSG, pSZ)				BASE_ASSERT((NULL != pMSG) && (L'\0' != pMSG[0]) && (0 < pSZ),		"Invalid string!");
//#define	CHECK_STRINGA(pMSG, pSZ)			BASE_ASSERT(!::IsBadStringPtrA(pMSG, pSZ) && ('\0'  != pMSG[0]) && (::strlen(pMSG) <= (size_t)pSZ),		"Invalid string!");
//#define	CHECK_STRINGW(pMSG, pSZ)			BASE_ASSERT(!::IsBadStringPtrW(pMSG, pSZ) && (L'\0' != pMSG[0]) && (::wcslen(pMSG) <= (size_t)pSZ),		"Invalid string!");
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
#if defined(_DEBUG)
	#if	(_MSC_VER >= 1400)	// VS.NET 2005 이상
		#define	STRCPYA(pDEST, pSTR, pDESTSZ)			BASE_ASSERT(0 == ::strcpy_s(pDEST, pDESTSZ, pSTR),					"Invalid!")
		#define	STRCPYW(pDEST, pSTR, pDESTSZ)			BASE_ASSERT(0 == ::wcscpy_s(pDEST, pDESTSZ, pSTR),					"Invalid!")
		#define	STRNCPYA(pDEST, pSTR, pDESTSZ)			BASE_ASSERT(0 == ::strncpy_s(pDEST, pDESTSZ, pSTR, pDESTSZ),		"Invalid!")
		#define	STRNCPYW(pDEST, pSTR, pDESTSZ)			BASE_ASSERT(0 == ::wcsncpy_s(pDEST, pDESTSZ, pSTR, pDESTSZ),		"Invalid!")
		#define	STRCATA(pDEST, pSTR, pDESTSZ)			BASE_ASSERT(0 == ::strcat_s(pDEST, pDESTSZ, pSTR),					"Invalid!")
		#define	STRCATW(pDEST, pSTR, pDESTSZ)			BASE_ASSERT(0 == ::wcscat_s(pDEST, pDESTSZ, pSTR),					"Invalid!")
		#define	STRNCATA(pDEST, pSTR, pDESTSZ)			BASE_ASSERT(0 == ::strncat_s(pDEST, pDESTSZ, pSTR, pDESTSZ),		"Invalid!")
		#define	STRNCATW(pDEST, pSTR, pDESTSZ)			BASE_ASSERT(0 == ::wcsncat_s(pDEST, pDESTSZ, pSTR, pDESTSZ),		"Invalid!")
		#define	STRUPA(pSTR, pSTRSZ)					BASE_ASSERT(0 == ::_strupr_s(pSTR, pSTRSZ),							"Invalid!");
		#define	STRUPW(pSTR, pSTRSZ)					BASE_ASSERT(0 == ::_wcsupr_s(pSTR, pSTRSZ),							"Invalid!");
		#define	STRLWA(pSTR, pSTRSZ)					BASE_ASSERT(0 == ::_strlwr_s(pSTR, pSTRSZ),							"Invalid!");
		#define	STRLWW(pSTR, pSTRSZ)					BASE_ASSERT(0 == ::_wcslwr_s(pSTR, pSTRSZ),							"Invalid!");
		#define	SPRINTFA								::sprintf_s
		#define	SPRINTFW								::swprintf_s
		#define	SNPRINTFA								::_snprintf_s
		#define	SNPRINTFW								::_snwprintf_s
		#define	VSNPRINTFA(pDEST, pDESTSZ, pFMT, pARG)	::_vsnprintf_s(pDEST, pDESTSZ, pDESTSZ-1, pFMT, pARG)
		#define	VSNPRINTFW(pDEST, pDESTSZ, pFMT, pARG)	::_vsnwprintf_s(pDEST, pDESTSZ, pDESTSZ-1, pFMT, pARG)
// 		#define	VSNPRINTFA(pDEST, pDESTSZ, pFMT, pARG)	BASE_ASSERT(0 < ::_vsnprintf_s(pDEST, pDESTSZ, pDESTSZ-1, pFMT, pARG),	"Invalid!")
// 		#define	VSNPRINTFW(pDEST, pDESTSZ, pFMT, pARG)	BASE_ASSERT(0 < ::_vsnwprintf_s(pDEST, pDESTSZ, pDESTSZ-1, pFMT, pARG),	"Invalid!")
		#define	SSCANFA									::sscanf
		#define	SSCANFW									::swscanf
	#else
		#define	STRCPYA(pDEST, pSTR, pDESTSZ)			BASE_ASSERT(SUCCEEDED(::StringCchCopyA(pDEST, pDESTSZ, pSTR)),				"Insufficient destination")
		#define	STRCPYW(pDEST, pSTR, pDESTSZ)			BASE_ASSERT(SUCCEEDED(::StringCchCopyW(pDEST, pDESTSZ, pSTR)),				"Insufficient destination")
		#define	STRNCPYA(pDEST, pSTR, pDESTSZ)			BASE_ASSERT(SUCCEEDED(::StringCchCopyNA(pDEST, pDESTSZ, pSTR, pDESTSZ)),	"Insufficient destination")
		#define	STRNCPYW(pDEST, pSTR, pDESTSZ)			BASE_ASSERT(SUCCEEDED(::StringCchCopyNW(pDEST, pDESTSZ, pSTR, pDESTSZ)),	"Insufficient destination")
		#define	STRCATA(pDEST, pSTR, pDESTSZ)			BASE_ASSERT(SUCCEEDED(::StringCchCatA(pDEST, pDESTSZ, pSTR)),				"Insufficient destination")
		#define	STRCATW(pDEST, pSTR, pDESTSZ)			BASE_ASSERT(SUCCEEDED(::StringCchCatW(pDEST, pDESTSZ, pSTR)),				"Insufficient destination")
		#define	STRNCATA(pDEST, pSTR, pDESTSZ)			BASE_ASSERT(SUCCEEDED(::StringCchCatNA(pDEST, pDESTSZ, pSTR, pDESTSZ)),		"Insufficient destination")
		#define	STRNCATW(pDEST, pSTR, pDESTSZ)			BASE_ASSERT(SUCCEEDED(::StringCchCatNW(pDEST, pDESTSZ, pSTR, pDESTSZ)),		"Insufficient destination")
		#define	STRUPA(pSTR, pSTRSZ)					::_strupr(pSTR)
		#define	STRUPW(pSTR, pSTRSZ)					::_wcsupr(pSTR)
		#define	STRLWA(pSTR, pSTRSZ)					::_strlwr(pSTR)
		#define	STRLWW(pSTR, pSTRSZ)					::_wcslwr(pSTR)
		#define	SPRINTFA								::sprintf
		#define	SPRINTFW								::swprintf
		#define	SNPRINTFA								::StringCchPrintfA
		#define	SNPRINTFW								::StringCchPrintfW
		#define	VSNPRINTFA(pDEST, pDESTSZ, pFMT, pARG)	BASE_ASSERT(SUCCEEDED(::StringCchVPrintfA(pDEST, pDESTSZ, pFMT, pARG)),		"Insufficient destination")
		#define	VSNPRINTFW(pDEST, pDESTSZ, pFMT, pARG)	BASE_ASSERT(SUCCEEDED(::StringCchVPrintfW(pDEST, pDESTSZ, pFMT, pARG)),		"Insufficient destination")
		#define	SSCANFA									::sscanf
		#define	SSCANFW									::swscanf
	#endif
	#define	STRLENA(pSTR, pSTRSZ)						CDebugSet::StrLenA(pSTR, pSTRSZ)
	#define	STRLENW(pSTR, pSTRSZ)						CDebugSet::StrLenW(pSTR, pSTRSZ)
	#define	FOPENA(pPATH, pMODE)						CDebugSet::FOpenA(pPATH, pMODE)
	#define	FOPENW(pPATH, pMODE)						CDebugSet::FOpenW(pPATH, pMODE)
	#define SPLITPATHA(pPATH, pDRV, pDRVSZ, pDIR, pDIRSZ, pFNM, pFNMSZ, pEXT, pEXTSZ)	::_splitpath_s(pPATH, pDRV, pDRVSZ, pDIR, pDIRSZ, pFNM, pFNMSZ, pEXT, pEXTSZ)
	#define SPLITPATHW(pPATH, pDRV, pDRVSZ, pDIR, pDIRSZ, pFNM, pFNMSZ, pEXT, pEXTSZ)	::_wsplitpath_s(pPATH, pDRV, pDRVSZ, pDIR, pDIRSZ, pFNM, pFNMSZ, pEXT, pEXTSZ)
#else	// _DEBUG
	#define	STRCPYA(pDEST, pSTR, pDESTSZ)				::strcpy(pDEST, pSTR)
	#define	STRCPYW(pDEST, pSTR, pDESTSZ)				::wcscpy(pDEST, pSTR)
	#define	STRNCPYA(pDEST, pSTR, pDESTSZ)				::strncpy(pDEST, pSTR, pDESTSZ);pDEST[pDESTSZ-1]='\0';
	#define	STRNCPYW(pDEST, pSTR, pDESTSZ)				::wcsncpy(pDEST, pSTR, pDESTSZ);pDEST[pDESTSZ-1]=L'\0';
	#define	STRCATA(pDEST, pSTR, pDESTSZ)				::strcat(pDEST, pSTR)
	#define	STRCATW(pDEST, pSTR, pDESTSZ)				::wcscat(pDEST, pSTR)
	#define	STRNCATA(pDEST, pSTR, pDESTSZ)				::strncat(pDEST, pSTR, pDESTSZ);pDEST[pDESTSZ-1]='\0';
	#define	STRNCATW(pDEST, pSTR, pDESTSZ)				::wcsncat(pDEST, pSTR, pDESTSZ);pDEST[pDESTSZ-1]=L'\0';
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

	#define	STRLENA(pSTR, pSTRSZ)						::strlen(pSTR)
	#define	STRLENW(pSTR, pSTRSZ)						::wcslen(pSTR)
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
	#define	STRLEN		STRLENW
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
	#define	STRLEN		STRLENA
	#define	FOPEN		FOPENA
	#define SPLITPATH	SPLITPATHA
#endif	// _UNICODE

