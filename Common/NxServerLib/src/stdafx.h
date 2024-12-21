// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이 
// 들어 있는 포함 파일입니다.
//
#include <winsock2.h>
#include <windows.h>
#include <iostream>
#include <tchar.h>

// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.
#include <string>
#include <strsafe.h>

typedef std::basic_string< TCHAR > tstring;


//-------------------------------------------------------------------------------------------------
// 매크로 
#ifndef SAFE_DELETE
#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#endif    

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#endif    

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }
#endif



// 디버그 출력창에 찍어준다.
void NxTrace( LPCTSTR szMsg, ... );

// 에러 메세지 박스
void NxMsgBox( LPCTSTR szMsg, ... );


// WCHAR to CHAR
void NxWideStringToMultiString(CHAR* strDes, const WCHAR* wstrSource);

// CHAR to WCHAR
void NxMultiStringToWideString(WCHAR* wstrDes , const CHAR* strSource);