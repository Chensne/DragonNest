/*//===============================================================

	<< MEMORY MAP >>

	FILE : MEMORYMAP.H, MEMORYMAP.CPP
	DESC : Memory Map 관리 클래스
	INIT BUILT DATE : 2005. 12. 11
	LAST BUILT DATE : 2005. 12. 11

	P.S.> 
		- 일반적으로 존재하는 파일에 접근하여 읽기만 할 목적으로 제작된 메모리 맵
		- 물리적인 파일을 새로 만들거나 크기가 0인 경우 CMemoryMap::Open 의 ::CreateFileMapping 에서 에러가 발생할 수 있으므로 파일크기 확인

	Copyrightⓒ 2002~2009 B4nFter (b4nfter@gmail.com) and Co-Writers. All Rights Reserved.

*///===============================================================


#pragma once


///////////////////////////////////////////////////////////////////
//	INCLUDE
///////////////////////////////////////////////////////////////////

#include <WINDOWS.H>


///////////////////////////////////////////////////////////////////
//	DEFINE
///////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////
//	DECLARATION
///////////////////////////////////////////////////////////////////

//*---------------------------------------------------------------
// TYPE : CLASS
// NAME : CMemoryMap
//*---------------------------------------------------------------
// DESC : Memory Map 관리 클래스
// PRGM : B4nFter
//*---------------------------------------------------------------
class CMemoryMap
{

public:
	CMemoryMap();
	~CMemoryMap();

	BOOL Open(
		LPCTSTR lpszFileName, 
		DWORD dwFileOffsetHigh = 0, 
		DWORD dwFileOffsetLow = 0, 
		SIZE_T dwNumberOfBytesToMap = 0,
		LPCTSTR lpszMemoryMapName = NULL, 
		DWORD dwDesiredAccess = GENERIC_READ, 
		DWORD dwCreationDisposition = OPEN_EXISTING
		);
	VOID Close();

	BOOL ChangeMapView(
		DWORD dwFileOffsetHigh = 0, 
		DWORD dwFileOffsetLow = 0, 
		SIZE_T dwNumberOfBytesToMap = 0
		);
	BOOL Flush();

	LPVOID GetBaseAddress();
	UINT64 GetSize();

private:
	HANDLE m_hFile;
	HANDLE m_hMemoryMap;

	LPVOID m_lpBaseAddr;
	DWORD m_dwSizeLow;
	DWORD m_dwSizeHigh;
};

