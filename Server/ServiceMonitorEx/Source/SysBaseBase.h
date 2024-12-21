/*//===============================================================

	<< SYSTEM BASE HEADER >>

	PRGM : B4nFter

	FILE : SYSBASEBASE.H
	DESC : System Base ���̺귯���� ��κ��� ���� ������Ʈ���� ���������� ���߾���� ��ũ�ο� ��� ������� ��Ƴ��� ���
	INIT BUILT DATE : 2005. 10. 15
	LAST BUILT DATE : 2005. 10. 15

	P.S.> 
		- ���� ������Ʈ�鸸�� ���� ����� ���ο����� INCLUDE �Ͽ� ���

	Copyright�� 2002~2009 B4nFter (b4nfter@gmail.com) and Co-Writers. All Rights Reserved.

*///===============================================================


#pragma once


///////////////////////////////////////////////////////////////////
//	LINK OPTION
///////////////////////////////////////////////////////////////////

// warning ����
#if defined(_MSC_VER) && (_MSC_VER <= 1200)		//  for VS 6.0 or less version.
	#pragma warning(disable:4786)
	#pragma warning(disable:4251)
#elif defined(_MSC_VER) && (_MSC_VER > 1200)	//  for VS 7.0 or more version.
	#pragma warning(disable:4251)
#endif	// _MSC_VER

// DLL�� ��ũ���λ� ����
#if defined(N4A_EXPORT)
#define N4A_DECLSPEC			__declspec(dllexport)
#define N4A_DECLSPEC_EXC		extern"C" __declspec(dllexport)		// CŸ�� ��ũ���� ��� (�ٸ� C++Ÿ�� ��ũ���� �Լ��� �Լ������ͷ� ���� ��� ���� ����)
#elif defined(N4A_IMPORT)
#define N4A_DECLSPEC			__declspec(dllimport)
#define N4A_DECLSPEC_EXC		extern"C" __declspec(dllimport)		// CŸ�� ��ũ���� ��� (�ٸ� C++Ÿ�� ��ũ���� �Լ��� �Լ������ͷ� ���� ��� ���� ����)
#else	// !defined(N4A_EXPORT) && !defined(N4A_IMPORT)
#define N4A_DECLSPEC			
#define N4A_DECLSPEC_EXC			
#endif	// !defined(N4A_EXPORT) && !defined(N4A_IMPORT)


///////////////////////////////////////////////////////////////////
//	INCLUDE
///////////////////////////////////////////////////////////////////

//#pragma warning (disable:4995)	// !!! �ӽ� - ���� ���ڿ� CRT�Լ� ���� ��� ��
#pragma warning (disable:4996)		// !!! �ӽ� - ���� ���ڿ� CRT�Լ� ���� ��� ��
#pragma warning (disable:4819)		// !!! �ӽ� - �ڵ������� ó�� ���� ��� ��

/*
	<������ ���� ����>
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

	<���ͳ� �ͽ��÷ξ ���� ����>
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
		- MBCS ��� �� Windows 98 ����, _UNICODE ��� �� Windows Server 2003 ���� ����
		- ������ ��Ÿ���� ��ũ�ΰ� ���� ������ WINVER�� �׻� �����Ƿ� WINVER ���
		- WINVER ��� �� Windows 98 �� Windows NT 4.0 ���� ũ�Ƿ� Windows Server 2000 �� �������� ��
		- ODBC �� ��ü ����� ���� ���� (�̱���)
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

// Windows �� min/max ��� STL �� min/max ���
#if !defined(NOMINMAX)
#define NOMINMAX   
#endif	// NOMINMAX

// C4996 ��� ����
#if !defined(_DEBUG)
#define _CRT_SECURE_NO_DEPRECATE
#endif	// _DEBUG

//#define _HAS_ITERATOR_DEBUGGING	0		// DEBUG ����� �� STL ��� ���� (�⺻�� 1)
//#define EXCEPT_EXTERNALLIB				// ����������Ʈ �̿��� �ܺ� ������Ʈ���� ���ϴ����� �����Ͽ� ����� ��� '������Ʈ �Ӽ� �� ��Ŀ �� �Է�' ���� ���̺귯���� ����θ� �߰��Ͽ� ���
#include <AFX.H>
#include <WINSOCK2.H>
#include <MSWSOCK.H>
#include <WINDOWS.H>
#include <TCHAR.H>
#include <STDIO.H>
#include <STDLIB.H>
#include <BASETSD.H>	// WIN64 ������Ÿ�� ��ȯ
#include <STRING>
#include <LMERR.H>
#define	_CONVERSION_DONT_USE_THREAD_LOCALE	// ATL 7.0 ���� ���� �� �����庰 �ڵ��������� ������ �ʰ� ���� ATL 3.0 ó�� CP_THREAD_ACP ��ũ�� ���
#include <ATLBASE.H>	// ATL 7.0 ���ڿ� ��ȯ Ŭ������ ��ũ�θ� ����
#include <ATLCONV.H>

//#include "SWITCH.H"		// SWITCH �� ���� �⺻����� �����ϴ� ��� �� �ƴ϶� �ٷ� �߰��Ǵ� ��� ��⿡�� ������ �� �� �ֵ��� ��
#include "COMMONMACROS.HPP"
#include "BASESET.HPP"
#include "DEBUGSET.HPP"
#include "SAFESTRINGMACROS.HPP"

// DWORD �� ���ϰ� Ÿ������ - ���� ������ ���Ϸ� �̵�
#define HASERROR				((DWORD)0x00000001L)	// NOERROR �� �ݴ��ǹ� (��������)
#define OLEERROR				((HRESULT)0x80040000L)	// HRESULT ���� (OLE_E_FIRST) (P.S.> S_FALSE �� ������ �ǹ� �� SUCCEEDED(), FAILED() ���� ������ ��޵��� ����)

