#pragma once
#include <Windows.h>
#include "AssertX.h"
#include "SmartPtr.h"
#include "StringUtil.h"
#ifndef SAFE_DELETE
#define SAFE_DELETE(x) if(x) { delete x; x=NULL; }
#endif // SAFE_DELETE

#ifndef SAFE_DELETEA
#define SAFE_DELETEA(x) if(x) { delete []x; x=NULL; }
#endif // SAFE_DELETEA

#ifndef SAFE_DELETE_VEC
template <class T>
void SAFE_DELETE_VEC( T &vecDel, bool bClearly = true ) 
{ 
	vecDel.clear();  
	if( bClearly ) {
		vecDel.swap( T() );	// 이렇게 해야 capacity 가 0 이된다.. x.swap(x) 는 아무 동작안함.
	}
} 
#endif // SAFE_DELETE_VEC

#ifndef SAFE_DELETE_VECF
#define SAFE_DELETE_VECF(x, f) \
	for( DWORD MACRO_0 = 0; MACRO_0<x.size(); MACRO_0++ ) { \
		f; \
	} \
	SAFE_DELETE_VEC(x)
#endif // SAFE_DELETE_VECF

#ifndef SAFE_DELETE_PVEC
#define SAFE_DELETE_PVEC(x) \
	for( DWORD MACRO_0 = 0; MACRO_0<x.size(); MACRO_0++ ) { \
		if( x[MACRO_0] ) delete x[MACRO_0]; \
	} \
	SAFE_DELETE_VEC(x)
#endif // SAFE_DELETE_PVEC

#ifndef SAFE_DELETE_PVECF
#define SAFE_DELETE_PVECF(x,f) \
	for( DWORD MACRO_0 = 0; MACRO_0<x.size(); MACRO_0++ ) { \
		f; \
		if( x[MACRO_0] ) delete x[MACRO_0]; \
	} \
	SAFE_DELETE_VEC(x)
#endif // SAFE_DELETE_PVECF

#ifndef SAFE_DELETE_MAP
#define SAFE_DELETE_MAP(x) SAFE_DELETE_VEC(x)
#endif //SAFE_DELETE_MAP

#ifndef SAFE_DELETE_MAPF
#define SAFE_DELETE_MAPF(t,x,f) \
	for( t::iterator t##_iter=x.begin(); t##_iter != x.end(); t##_iter++ ) { \
		f; \
	} \
	SAFE_DELETE_MAP(x)
#endif //SAFE_DELETE_MAPF

#ifndef SAFE_DELETE_PMAP
#define SAFE_DELETE_PMAP(t, x) \
	for( t::iterator t##_iter=x.begin(); t##_iter != x.end(); t##_iter++ ) { \
		SAFE_DELETE( t##_iter->second ); \
	} \
	SAFE_DELETE_MAP(x)
#endif //SAFE_DELETE_PMAP

#ifndef SAFE_DELETEA_PMAP
#define SAFE_DELETEA_PMAP(t, x) \
	for( t::iterator t##_iter=x.begin(); t##_iter != x.end(); t##_iter++ ) { \
	SAFE_DELETEA( t##_iter->second ); \
	} \
	SAFE_DELETE_MAP(x)
#endif //SAFE_DELETE_PMAP

#ifndef SAFE_DELETE_PMAPF
#define SAFE_DELETE_PMAPF(t, x, f) \
	for( t::iterator t##_iter=x.begin(); t##_iter != x.end(); t##_iter++ ) { \
		f; \
		SAFE_DELETE( t##_iter->second ); \
	} \
	SAFE_DELETE_MAP(x)
#endif //SAFE_DELETE_PMAPF

#ifndef SAFE_DELETE_AVEC
#define SAFE_DELETE_AVEC(x) \
	for( DWORD MACRO_0 = 0; MACRO_0<x.size(); MACRO_0++ ) { \
		if( x[MACRO_0] ) delete []x[MACRO_0]; \
	} \
	SAFE_DELETE_VEC(x)
#endif // SAFE_DELETE_AVEC

#ifndef SAFE_DELETE_AVECF
#define SAFE_DELETE_AVECF(x,f) \
	for( DWORD MACRO_0 = 0; MACRO_0<x.size(); MACRO_0++ ) { \
		f; \
		if( x[MACRO_0] ) delete []x[MACRO_0]; \
	} \
	SAFE_DELETE_VEC(x)
#endif // SAFE_DELETE_AVECF


#ifndef SAFE_RELEASE
//#define _CHECK_D3D_LEAK
#ifdef _CHECK_D3D_LEAK
void __ADD_D3D_RES( void *pPtr, char *szFile, int nLine );
void REMOVE_D3D_RES( void *pPtr );
#define ADD_D3D_RES( x ) __ADD_D3D_RES( x, __FILE__, __LINE__ )
#define SAFE_RELEASE( x )		\
	if( x ) {			\
		REMOVE_D3D_RES( (x) );	\
		x->Release();	\
		x = NULL;	\
	}
#else
#define SAFE_RELEASE(x) if(x) { x->Release(); x=NULL; }
#define ADD_D3D_RES(x)	__noop
#endif
#else
#define ADD_D3D_RES(x)	__noop
#endif

#ifndef SAFE_RELEASE_PVEC
#define SAFE_RELEASE_PVEC(x) \
	for( DWORD MACRO_0 = 0; MACRO_0<x.size(); MACRO_0++ ) { \
		SAFE_RELEASE( x[MACRO_0] ); \
	} \
	SAFE_DELETE_VEC(x)
#endif // SAFE_DELETE_PVEC

#ifndef SAFE_RELEASE_PVECF
#define SAFE_RELEASE_PVECF(x,f) \
	for( DWORD MACRO_0 = 0; MACRO_0<x.size(); MACRO_0++ ) { \
		f; \
		SAFE_RELEASE( x[MACRO_0] ); \
	} \
	SAFE_DELETE_VEC(x)
#endif // SAFE_DELETE_PVECF


#ifndef ERASE_VEC
#define ERASE_VEC(vec, x) \
	for( int MACRO_0 = 0; MACRO_0<(int)vec.size(); MACRO_0++ ) { \
		if( vec[MACRO_0] == x ) { \
			vec.erase( vec.begin() + MACRO_0 ); \
			break; \
		} \
	}
#endif // ERASE_VEC

#ifndef SAFE_RELEASE_SPTR
#define SAFE_RELEASE_SPTR(x) if(x) { x->Release(); x.Identity(); }
#endif //SAFE_RELEASE_SPTR

#ifndef SAFE_RELEASE_SPTRVEC
#define SAFE_RELEASE_SPTRVEC(x) \
	for( DWORD MACRO_0 = 0; MACRO_0<x.size(); MACRO_0++ ) { \
		if( x[MACRO_0] ) { \
			SAFE_RELEASE_SPTR( x[MACRO_0] ); \
		} \
	} \
	SAFE_DELETE_VEC( x )
#endif //SAFE_RELEASE_SPTRVEC

#ifndef SAFE_RELEASE_SPTRVECF
#define SAFE_RELEASE_SPTRVECF(x,f) \
	for( DWORD MACRO_0 = 0; MACRO_0<x.size(); MACRO_0++ ) { \
		f; \
		if( x[MACRO_0] ) { \
			SAFE_RELEASE_SPTR( x[MACRO_0] ); \
		} \
	} \
	SAFE_DELETE_VEC( x )
#endif //SAFE_RELEASE_SPTRVECF


#ifndef SAFE_CLOSE_HANDLE
#define SAFE_CLOSE_HANDLE(x) { if( x != INVALID_HANDLE_VALUE ) { CloseHandle( x ); x = INVALID_HANDLE_VALUE; } }
#endif //SAFE_CLOSE_HANDLE


// FileName String Function
const char *_GetPath( char *pBuffer, int iCharBufferSize, const char *pPath );
const char *_GetFullFileName( char *pBuffer, int iCharBufferSize, const char *pString );
const char *_GetExt( char *pBuffer, int iCharBufferSize, const char *pString );
const char *_GetFileName( char *pBuffer, int iCharBufferSize, const char *pString );
const char *_GetOriginalRandomBaseName( char *pBuffer, const char *pString );

// String Function
char *_CopyString( const char *szStr ); // Allocation 한 char 배열포인터를 리턴한다.
const char *_GetSubStrByCount( int nCount, const char *szSor, char cCharacter = '|' ); // cCharacter 로 구분된 스트링들을 파싱하여 리턴해준다. 더이상 없으면 NULL 리턴.
std::string _GetSubStrByCountSafe( int nCount, char *szSor, char cCharacter );
int _GetNumSubStr( char *szSor, char cCharacter );

// Help Debug Function
void _OutputDebug( const char *szString, ... );
void OutputDebugFunc( const char *szString, ... );
void SetOutputDebugFuncPtr( void (__stdcall *Func)(const char*, ...) );
extern void (__stdcall *s_pDefaultOutputDebugFunc)(const char*, ...);

// Directory Search Function
int FindFileInDirectory( const char *pFindDir, const char *pFileName, char *pTargetBuffer );
void FindFileListInDirectory( const char *szFolderName, const char *szExt, std::vector<CFileNameString> &szVecResult, bool bIncludeFolder = false, bool bSearchSubDir = false, bool bIncludeFolderName = false );
void FindFileListInDirectory( const char *szFolderName, const char *szExt, std::vector<std::string> &szVecResult, bool bIncludeFolder = false, bool bSearchSubDir = false, bool bIncludeFolderName = false );
void GetFullPath( const char *pSource, char *pTarget );
void _FindFolder( const char *szFolderName, std::vector<CFileNameString> &szVecResult, bool bSearchSubDir = false, char *szRootPathName = NULL );
void _FindFolder( const char *szFolderName, std::vector<std::string> &szVecResult, bool bSearchSubDir = false, char *szRootPathName = NULL );
const char *FindFileName( const char *pFullName );
bool IsFullName( const char *pFileName );

// File Function
bool _CopyFile( const char *pSource, const char *pTarget, bool bExistOverwrite = true, bool bMoveFile = false );
void DeleteFolder( const char *pFolder );
void CreateFolder( const char *pFolder );

// Registry Function
LONG GetRegistryNumber(HKEY hKey, char *pSubKey, char *pValueName, DWORD &dwValue);
LONG SetRegistryNumber(HKEY hKey, char *pSubKey, char *pValueName, DWORD dwValue);
LONG GetRegistryString(HKEY hKey, char *pSubKey, char *pValueName, char *pValue, int nMaxLength = 0 );
LONG SetRegistryString(HKEY hKey, char *pSubKey, char *pValueName, char *pValue );
LONG GetRegistryBinary(HKEY hKey, char *pSubKey, char *pValueName, void *pValue, int &nSize );
LONG SetRegistryBinary(HKEY hKey, char *pSubKey, char *pValueName, void *pValue, int nSize );

LONG DeleteRegistry( HKEY hKey, char *pSubKey );
LONG AddRegistryKey( HKEY hKey, char *pSubKey, char *pValueName );

// Simple Function
template<class X> void _Swap( X &A, X &B )
{
	X Temp = A;
	A = B;
	B = Temp;
}

class CStream;
void ReadStdString( std::string &szStr, CStream *pStream );
void WriteStdString( std::string &szStr, CStream *pStream );

void SetBitFlag( char *pBuffer, int nArrayIndex, bool bFlag );
bool GetBitFlag( char *pBuffer, int nArrayIndex );
int GetDateValue( int nYear, int nMonth, int nDay );
// Note : 윈도우 타입 얻기
//		아래와 같은 리턴값을 가집니다.
//      -1: Failed
//      1 : Windows 95
//      2 : Windows 98, 98SE
//      3 : Windows ME
//      4 : Windows NT
//      5 : Windows 2000
//      6 : Windows XP
//		7 : Windows Server 2003
//		8 : Windows Vista
int GetOSVersionType();

unsigned long GetHashCode( const char* String );
INT64 GetHashCode64( const char* String );
bool ExecCommand( char *pCommand, std::string *pszOutput );

// 걍 넣은숫자 리턴함수인데.. 뒤에 인덱스를 2개씩 짝지어서 넣으면 해당 인덱스일 경우에 바꿔서 리턴
int GetValue2ExceptionSwap( int nValue, int nSwapValueCount, ... ); 
/*RLKT  20.07.2016 open output debug! */
/*#ifdef _FINAL_BUILD
#define OutputDebug( fmt, ... ) do { } while( false )
#else*/
#define OutputDebug( fmt, ... ) OutputDebugFunc( fmt, __VA_ARGS__ )
//#endif

bool DnLocalTime_s( struct tm * _Tm, const time_t * _Time );

#define ThreadDelay() Sleep(0);

DWORD CalcDecreaseColor( DWORD OrigColor, DWORD DescColor );

UINT64 GetFreeDiskSpaceAsByte(const WCHAR * pwszRootName = NULL);
UINT GetFreeDiskSpaceAsMegaByte(const WCHAR * pwszRootName = NULL);
UINT GetFreeDiskSpaceAsGigaByte(const WCHAR * pwszRootName = NULL);

#define GetTupleValue(t,idx) std::tr1::get<idx>(t)