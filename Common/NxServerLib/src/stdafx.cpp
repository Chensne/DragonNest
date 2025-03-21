// stdafx.cpp : 표준 포함 파일만 들어 있는 소스 파일입니다.
// NxServer.pch는 미리 컴파일된 헤더가 됩니다.
// stdafx.obj에는 미리 컴파일된 형식 정보가 포함됩니다.

#include "stdafx.h"

// TODO: 필요한 추가 헤더는
// 이 파일이 아닌 STDAFX.H에서 참조합니다.


void 
NxTrace( LPCTSTR szMsg, ... )
{
#if defined(DEBUG) || defined(_DEBUG)
	TCHAR szBuffer[1024] = _T("0");

	va_list args;
	va_start(args, szMsg);
	StringCchVPrintf( szBuffer, 1023, szMsg, args );
	szBuffer[511] = _T('\0');
	va_end(args);

	OutputDebugString( szBuffer );
#endif

}

// 에러 메세지 박스
void 
NxMsgBox( LPCTSTR szMsg, ... )
{
	TCHAR szBuffer[1024] = _T("0");

	va_list args;
	va_start(args, szMsg);
	StringCchVPrintf( szBuffer, 1023, szMsg, args );
	szBuffer[511] = _T('\0');
	va_end(args);

	MessageBox( NULL, szBuffer, _T("Error"), MB_OK | MB_ICONERROR);
}



//----------------------------------------------------------------------------------------
void
NxWideStringToMultiString(CHAR* strDes, const WCHAR* wstrSource)
{
	//먼저 길이를 구한다.
	int nMultiByteLen = WideCharToMultiByte(CP_ACP, 0, wstrSource, -1, NULL, 0, NULL,NULL);

	WideCharToMultiByte(CP_ACP, 0, wstrSource, -1, strDes, nMultiByteLen, NULL, NULL);
}

//----------------------------------------------------------------------------------------
void
NxMultiStringToWideString(WCHAR* wstrDes , const CHAR* strSource )
{
	//먼저 길이를 구한다.
	int nMultiByteLen = MultiByteToWideChar(CP_ACP, 0, strSource, -1, NULL, 0);

	MultiByteToWideChar(CP_ACP, 0, strSource, -1, wstrDes, nMultiByteLen);
}


