#pragma once


#ifdef _DEBUG
template <bool T> struct ASSERT_STRUCT {};
template<> struct ASSERT_STRUCT<true> { static void static_assertion_failed() {} };
#define StaticAssert(expr) { const bool __b = (expr) ? true : false; ASSERT_STRUCT<__b>::static_assertion_failed(); }
#else
#define StaticAssert(expr) ((void)0)
#endif

#define CHECK_ALIGNMENT()  \
struct __alignment_static_test {char _;int __;double ___;}; \
	StaticAssert( sizeof(__alignment_static_test) == 16 ); \
	StaticAssert( __alignof(__alignment_static_test) == 8 );

#ifndef SAFE_DELETE
#define SAFE_DELETE(x) if(x) { delete x; x=NULL; }
#endif // SAFE_DELETE

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(x) if(x) { delete []x; x=NULL; }
#endif // SAFE_DELETE_ARRAY

#define _S _fnLoadString
CString _fnLoadString( UINT id );

BOOL KillProcess( LPCTSTR strProcessName );						// 프로세스 죽이기
BOOL FindProcessName( LPCTSTR strProcessName );					// 해당 이름의 프로세스가 있는지 확인하기
HRGN CreateRgnFromFile( HBITMAP hBmp, COLORREF color );			// 비트맵에서 Region 구하기