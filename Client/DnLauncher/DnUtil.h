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

BOOL ClientDeleteFile( LPCTSTR strFileName );					// 클라이언트에서 해당 파일 삭제하기
BOOL KillProcess( LPCTSTR strProcessName );						// 프로세스 죽이기
BOOL FindProcessName( LPCTSTR strProcessName );					// 해당 이름의 프로세스가 있는지 확인하기
HRGN CreateRgnFromFile( HBITMAP hBmp, COLORREF color );			// 비트맵에서 Region 구하기

BOOL RunProcess( LPCTSTR strProcessName );
BOOL KillMyProcess( LPCTSTR strEXEName );						// 자기자신의 프로세스만을 종료시킨다.

// DirectX 버전 구하기
HRESULT GetDXVersion( DWORD* pdwDirectXVersion, TCHAR* strDirectXVersion, int cchDirectXVersion );
HRESULT GetDirectXVersionViaDxDiag( DWORD* pdwDirectXVersionMajor, DWORD* pdwDirectXVersionMinor, TCHAR* pcDirectXVersionLetter );
HRESULT GetDirectXVersionViaFileVersions( DWORD* pdwDirectXVersionMajor, DWORD* pdwDirectXVersionMinor, TCHAR* pcDirectXVersionLetter );
ULARGE_INTEGER MakeInt64( WORD a, WORD b, WORD c, WORD d );
int CompareLargeInts( ULARGE_INTEGER ullParam1, ULARGE_INTEGER ullParam2 );
HRESULT GetFileVersion( TCHAR* szPath, ULARGE_INTEGER* pllFileVersion );
void EnumDisplayMode( std::vector<POINT>& vecDisplayMode );

void ErrorMessageBoxLog( UINT uiStringID );
void ErrorMessageBoxLog( CString strString );

HRESULT DownloadToFile( LPCTSTR strDownloadUrl, LPCTSTR strSaveFilePath );
BOOL IsExistFile( HINTERNET hHttpConnection );
BOOL IsExistFile( LPCTSTR strDownloadUrl );