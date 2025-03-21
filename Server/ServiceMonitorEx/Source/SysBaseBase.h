/*//===============================================================

	<< SYSTEM BASE HEADER >>

	PRGM : B4nFter

	FILE : SYSBASEBASE.H
	DESC : System Base 라이브러리의 대부분의 구성 컴포넌트들이 공통적으로 갖추어야할 매크로와 헤더 선언등을 모아놓은 헤더
	INIT BUILT DATE : 2005. 10. 15
	LAST BUILT DATE : 2005. 10. 15

	P.S.> 
		- 내부 컴포턴트들만을 위한 헤더로 내부에서만 INCLUDE 하여 사용

	Copyrightⓒ 2002~2009 B4nFter (b4nfter@gmail.com) and Co-Writers. All Rights Reserved.

*///===============================================================


#pragma once


///////////////////////////////////////////////////////////////////
//	LINK OPTION
///////////////////////////////////////////////////////////////////

// warning 제거
#if defined(_MSC_VER) && (_MSC_VER <= 1200)		//  for VS 6.0 or less version.
	#pragma warning(disable:4786)
	#pragma warning(disable:4251)
#elif defined(_MSC_VER) && (_MSC_VER > 1200)	//  for VS 7.0 or more version.
	#pragma warning(disable:4251)
#endif	// _MSC_VER

// DLL용 링크접두사 선언
#if defined(N4A_EXPORT)
#define N4A_DECLSPEC			__declspec(dllexport)
#define N4A_DECLSPEC_EXC		extern"C" __declspec(dllexport)		// C타입 링크관계 사용 (다른 C++타입 링크관계 함수의 함수포인터로 들어가는 경우 오류 주의)
#elif defined(N4A_IMPORT)
#define N4A_DECLSPEC			__declspec(dllimport)
#define N4A_DECLSPEC_EXC		extern"C" __declspec(dllimport)		// C타입 링크관계 사용 (다른 C++타입 링크관계 함수의 함수포인터로 들어가는 경우 오류 주의)
#else	// !defined(N4A_EXPORT) && !defined(N4A_IMPORT)
#define N4A_DECLSPEC			
#define N4A_DECLSPEC_EXC			
#endif	// !defined(N4A_EXPORT) && !defined(N4A_IMPORT)


///////////////////////////////////////////////////////////////////
//	INCLUDE
///////////////////////////////////////////////////////////////////

//#pragma warning (disable:4995)	// !!! 임시 - 안전 문자열 CRT함수 관련 경고 끔
#pragma warning (disable:4996)		// !!! 임시 - 안전 문자열 CRT함수 관련 경고 끔
#pragma warning (disable:4819)		// !!! 임시 - 코드페이지 처리 관련 경고 끔

/*
	<윈도우 버전 설정>
		Minimum system required				Macros to define
		------------------------			------------------------
		Windows Vista						WINVER>=0x0600/_WIN32_WINNT>=0x0600
		Windows Server 2003 family			WINVER>=0x0502/_WIN32_WINNT>=0x0502
		Windows XP							WINVER>=0x0501/_WIN32_WINNT>=0x0501
		Windows 2000						WINVER>=0x0500/_WIN32_WINNT>=0x0500
		Windows NT 4.0						WINVER>=0x0400/_WIN32_WINNT>=0x0400
		Windows Me							WINVER>=0x0500/_WIN32_WINDOWS=0x0500
		Windows 98							WINVER>=0x0410/_WIN32_WINDOWS>=0x0410
		Windows 95							WINVER>=0x0400/_WIN32_WINDOWS>=0x0400 
		
		http://msdn.microsoft.com/library/default.asp?url=/library/en-us/winprog/winprog/using_the_windows_headers.asp

	<인터넷 익스플로어러 버전 설정>
		Minimum system required				Macros to define
		------------------------			------------------------
		Internet Explorer 7.0				_WIN32_IE>=0x0700
		Internet Explorer 6.0 SP2			_WIN32_IE>=0x0603
		Internet Explorer 6.0 SP1			_WIN32_IE>=0x0601
		Internet Explorer 6.0				_WIN32_IE>=0x0600
		Internet Explorer 5.5				_WIN32_IE>=0x0550
		Internet Explorer 5.01				_WIN32_IE>=0x0501
		Internet Explorer 5.0, 5.0a, 5.0b	_WIN32_IE>=0x0500
		Internet Explorer 4.01				_WIN32_IE>=0x0401
		Internet Explorer 4.0				_WIN32_IE>=0x0400
		Internet Explorer 3.0, 3.01, 3.02	_WIN32_IE>=0x0300

	P.S.>
		- MBCS 사용 시 Windows 98 부터, _UNICODE 사용 시 Windows Server 2003 부터 지원
		- 버전을 나타내는 매크로가 여럿 있지만 WINVER는 항상 있으므로 WINVER 사용
		- WINVER 사용 시 Windows 98 이 Windows NT 4.0 보다 크므로 Windows Server 2000 을 기준으로 함
		- ODBC 는 자체 헤더에 별도 지정 (미구현)
*/

#if defined(_MBCS)
	#if defined(_WIN32_WINDOWS)
		#undef	_WIN32_WINDOWS
	#endif	// #if defined(_WIN32_WINDOWS)
	#if !defined(_WIN32_WINDOWS)
		#define	_WIN32_WINDOWS	0x0410
	#endif	// #if !defined(_WIN32_WINDOWS)

	#if defined(_WIN32_WINNT) && (_WIN32_WINNT < 0x0502)
		#undef	_WIN32_WINNT
	#endif	// #if defined(_WIN32_WINNT) && (_WIN32_WINNT < 0x0502)
	#if !defined(_WIN32_WINNT)
		#define	_WIN32_WINNT	0x0502
	#endif	// #if !defined(_WIN32_WINNT)

	#if defined(WINVER)
		#undef	WINVER
	#endif	// #if defined(WINVER)
	#if !defined(WINVER)
		#define	WINVER	0x0410
	#endif	// #if !defined(WINVER)
#else	// #if defined(_MBCS)
	#if defined(_WIN32_WINNT) && (_WIN32_WINNT < 0x0502)
		#undef	_WIN32_WINNT
	#endif	// #if defined(_WIN32_WINNT) && (_WIN32_WINNT < 0x0502)
	#if !defined(_WIN32_WINNT)
		#define	_WIN32_WINNT	0x0502
	#endif	// #if !defined(_WIN32_WINNT)

	#if defined(WINVER) && (WINVER < 0x0502)
		#undef	WINVER
	#endif	// #if defined(WINVER) && (WINVER < 0x0502)
	#if !defined(WINVER)
		#define	WINVER	0x0502
	#endif	// #if !defined(WINVER)
#endif	// #if defined(_MBCS)

// Windows 의 min/max 대신 STL 의 min/max 사용
#if !defined(NOMINMAX)
#define NOMINMAX   
#endif	// NOMINMAX

// C4996 경고 방지
#if !defined(_DEBUG)
#define _CRT_SECURE_NO_DEPRECATE
#endif	// _DEBUG

//#define _HAS_ITERATOR_DEBUGGING	0		// DEBUG 모드일 때 STL 경고 무시 (기본값 1)
//#define EXCEPT_EXTERNALLIB				// 서버프로젝트 이외의 외부 프로젝트에서 파일단위로 포함하여 사용할 경우 '프로젝트 속성 → 링커 → 입력' 에서 라이브러리의 상대경로를 추가하여 사용
#include <AFX.H>
#include <WINSOCK2.H>
#include <MSWSOCK.H>
#include <WINDOWS.H>
#include <TCHAR.H>
#include <STDIO.H>
#include <STDLIB.H>
#include <BASETSD.H>	// WIN64 데이터타입 변환
#include <STRING>
#include <LMERR.H>
#define	_CONVERSION_DONT_USE_THREAD_LOCALE	// ATL 7.0 부터 생긴 각 스레드별 코드페이지를 얻어오지 않고 기존 ATL 3.0 처럼 CP_THREAD_ACP 매크로 사용
#include <ATLBASE.H>	// ATL 7.0 문자열 변환 클래스와 매크로를 위함
#include <ATLCONV.H>

//#include "SWITCH.H"		// SWITCH 가 내부 기본헤더를 포함하는 모듈 뿐 아니라 바로 추가되는 모든 모듈에도 영향을 줄 수 있도록 함
#include "COMMONMACROS.HPP"
#include "BASESET.HPP"
#include "DEBUGSET.HPP"
#include "SAFESTRINGMACROS.HPP"

// DWORD 형 리턴값 타입정의 - 차후 적당한 파일로 이동
#define HASERROR				((DWORD)0x00000001L)	// NOERROR 의 반대의미 (에러있음)
#define OLEERROR				((HRESULT)0x80040000L)	// HRESULT 에러 (OLE_E_FIRST) (P.S.> S_FALSE 는 성공을 의미 → SUCCEEDED(), FAILED() 에서 에러로 취급되지 않음)

