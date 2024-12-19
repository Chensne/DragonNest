// GetFontFile.cpp
//
// Copyright (C) 2001 Hans Dietrich
//
// This software is released into the public domain.  
// You are free to use it in any way you like.
//
// This software is provided "as is" with no expressed 
// or implied warranty.  I accept no liability for any 
// damage or loss of business that this software may cause. 
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GetFontFile.h"
#include "GetNameValue.h"
#include "SundriesFunc.h"
#include "DebugSet.h"

///////////////////////////////////////////////////////////////////////////////
// GetFontFile
//
// Note:  This is *not* a foolproof method for finding the name of a font file.
//        If a font has been installed in a normal manner, and if it is in
//        the Windows "Font" directory, then this method will probably work.
//        It will probably work for most screen fonts and TrueType fonts.
//        However, this method might not work for fonts that are created 
//        or installed dynamically, or that are specific to a particular
//        device, or that are not installed into the font directory.

BOOL GetFontFile(LPCTSTR lpszFontName, CString& strDisplayName, CString& strFontFile)
{
	ASSERT(lpszFontName && lpszFontName[0] != 0);

	_TCHAR szName[2 * MAX_PATH];
	_TCHAR szData[2 * MAX_PATH];

	CString strFont;
	int nOsType = GetOSVersionType();
	switch( nOsType )
	{
	case 1:																					// 1 : Windows 95
	case 2:																					// 2 : Windows 98, 98SE
	case 3:	strFont = _T("Software\\Microsoft\\Windows\\CurrentVersion\\Fonts"); break;		// 3 : Windows ME
	case 4:																					// 4 : Windows NT
	case 5:																					// 5 : Windows 2000
	case 6:																					// 6 : Windows XP
	case 7:																					// 7 : Windows Server 2003
	case 8:																					// 8 : Windows Vista
	case 9:	strFont = _T("Software\\Microsoft\\Windows NT\\CurrentVersion\\Fonts"); break;	// 9 : Windows 7
	default:
		CDebugSet::ToLogFile( "GetFontFile, Unknow OS Version Type!" );
		break;
	}

	strFontFile.Empty();

	BOOL bResult = FALSE;

	while (GetNextNameValue(HKEY_LOCAL_MACHINE, strFont, szName, szData) == ERROR_SUCCESS)
	{
		if ( strstr(szName, lpszFontName) != NULL )
		{
			TRACE(_T("found font\n"));
			strDisplayName = szName;
			strFontFile = szData;
			bResult = TRUE;
			break;
		}
		// 일부 폰트들은 레지에 등록된 이름과 폰트페이스네임이 다르다.
		// (폰트를 열었을때 맨 위에 적혀있는 이름과 서체 이름이 다를때)
		// 그래서 실제 폰트파일 이름으로도 검사하도록 하겠다.
		else if( strstr(szData, lpszFontName) != NULL )
		{
			TRACE(_T("found font\n"));
			strDisplayName = szName;
			strFontFile = szData;
			bResult = TRUE;
			break;
		}

		strFont.Empty();	// this will get next value, same key
	}

	GetNextNameValue(HKEY_LOCAL_MACHINE, NULL, NULL, NULL);	// close the registry key

	return bResult;
}
