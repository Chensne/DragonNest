

///////////////////////////////////////////////////////////////////
//	INCLUDE
///////////////////////////////////////////////////////////////////

#include "STDAFX.H"
#include "MEMORYMAP.H"
#include "COMMONMACROS.H"
#include "BASESET.H"
#include "DEBUGUTIL.H"
#include "SAFESTRINGMACROS.H"


#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

///////////////////////////////////////////////////////////////////
//	IMPLEMENTATION
///////////////////////////////////////////////////////////////////

//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryMap::CMemoryMap
//*---------------------------------------------------------------
// DESC : 생성자
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
CMemoryMap::CMemoryMap() :
	m_hFile(INVALID_HANDLE_VALUE),m_hMemoryMap(INVALID_HANDLE_VALUE),
	m_lpBaseAddr(0), m_dwSizeLow(0), m_dwSizeHigh(0)
{
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryMap::~CMemoryMap
//*---------------------------------------------------------------
// DESC : 소멸자
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
CMemoryMap::~CMemoryMap()
{
	Close();
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryMap::Open
//*---------------------------------------------------------------
// DESC : Memory Map으로 사용할 파일을 접근, 개방
// PARM :	1 . lpszFileName - Memory Map으로 사용할 파일의 이름
//			2 . dwFileOffsetHigh - 접근하고자 하는 파일위치 (상위DWORD)
//			3 . dwFileOffsetLow - 접근하고자 하는 파일위치 (하위DWORD)
//			4 . dwNumberOfBytesToMap - 매핑하고자 하는 바이트 수
//			5 . lpszMemoryMapName - Memory Map의 이름
//			6 . dwDesiredAccess - 접근하고자 하는 모드
//			7 . dwCreationDisposition - 파일을 생성할 방식 (ex> CREATE_ALWAYS, OPEN_EXISTING 등)
// RETV : TRUE - 성공 / FALSE - 실패
// PRGM : B4nFter
//*---------------------------------------------------------------
BOOL CMemoryMap::Open(
					  LPCTSTR lpszFileName, 
					  DWORD dwFileOffsetHigh,  
					  DWORD dwFileOffsetLow, 
					  SIZE_T dwNumberOfBytesToMap,
					  LPCTSTR /*lpszMemoryMapName*/, 
					  DWORD dwDesiredAccess, 
					  DWORD dwCreationDisposition
					  )
{
	if (m_hFile != INVALID_HANDLE_VALUE) {
		return FALSE;
	}

	m_hFile = ::CreateFile(lpszFileName, dwDesiredAccess, FILE_SHARE_READ, 0, dwCreationDisposition, FILE_ATTRIBUTE_NORMAL, 0);
	if (m_hFile == INVALID_HANDLE_VALUE) {
		DN_TRACE(_T("m_hFile == INVALID_HANDLE_VALUE : %d - CMemoryMap::Open()"), GetLastError());
		return FALSE;
	}

	m_hMemoryMap = ::CreateFileMapping(m_hFile, 0, PAGE_WRITECOPY, 0, 0, 0);
	if (!m_hMemoryMap) {
		DN_TRACE(_T("!m_hMemoryMap : %d - CMemoryMap::Open()"), GetLastError());
		return FALSE;
	}

	m_lpBaseAddr = ::MapViewOfFile(m_hMemoryMap, FILE_MAP_COPY, dwFileOffsetHigh, dwFileOffsetLow, dwNumberOfBytesToMap);
	if (!m_lpBaseAddr) {
		DN_TRACE(_T("!m_lpBaseAddr : %d - CMemoryMap::Open()"), GetLastError());
		return FALSE;
	}

	m_dwSizeLow = ::GetFileSize(m_hFile, &m_dwSizeHigh);

	return TRUE;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryMap::Close
//*---------------------------------------------------------------
// DESC : Memory Map으로 사용할 파일을 닫음
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
VOID CMemoryMap::Close()
{
	if (m_lpBaseAddr != 0) {
		::UnmapViewOfFile(m_lpBaseAddr);
		m_lpBaseAddr = 0;
	}

	if (m_hMemoryMap != INVALID_HANDLE_VALUE) {
		::CloseHandle(m_hMemoryMap);
		m_hMemoryMap = INVALID_HANDLE_VALUE;
	}

	if (m_hFile != INVALID_HANDLE_VALUE)	{
		::CloseHandle(m_hFile);
		m_hFile = INVALID_HANDLE_VALUE;
	}

	m_dwSizeLow = 0;
	m_dwSizeHigh = 0;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryMap::ChangeMapView
//*---------------------------------------------------------------
// DESC : Memory Map의 뷰의 위치만 변경
// PARM :	1 . dwFileOffsetHigh - 접근하고자 하는 파일위치 (상위DWORD)
//			2 . dwFileOffsetLow - 접근하고자 하는 파일위치 (하위DWORD)
//			3 . dwNumberOfBytesToMap - 매핑하고자 하는 바이트 수
// RETV : TRUE - 성공 / FALSE - 실패
// PRGM : B4nFter
//*---------------------------------------------------------------
BOOL CMemoryMap::ChangeMapView(
	DWORD dwFileOffsetHigh , 
	DWORD dwFileOffsetLow, 
	SIZE_T dwNumberOfBytesToMap
	)
{
	if (m_hFile == INVALID_HANDLE_VALUE) {
		return FALSE;
	}
	if (!m_hMemoryMap) {
		return FALSE;
	}
	if (m_lpBaseAddr != 0) {
		::UnmapViewOfFile(m_lpBaseAddr);
		m_lpBaseAddr = 0;
	}

	m_lpBaseAddr = ::MapViewOfFile(m_hMemoryMap, FILE_MAP_COPY, dwFileOffsetHigh, dwFileOffsetLow, dwNumberOfBytesToMap);
	if (!m_lpBaseAddr) {
		DN_TRACE(_T("!m_lpBaseAddr : %d - CMemoryMap::ChangeMapView()"), GetLastError());
		return FALSE;
	}

	return TRUE;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryMap::Flush
//*---------------------------------------------------------------
// DESC : Memory Map의 버퍼를 비움 (I/O 처리)
// PARM : N/A
// RETV : TRUE - 성공 / FALSE - 실패
// PRGM : B4nFter
//*---------------------------------------------------------------
BOOL CMemoryMap::Flush()
{
	if (::FlushViewOfFile(m_lpBaseAddr, 0) == FALSE) {
		DN_TRACE(_T("::FlushViewOfFile(m_lpBaseAddr, 0) == FALSE : %d - CMemoryMap::Open()"), GetLastError());
		return FALSE;
	}

	return TRUE;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryMap::GetBaseAddress
//*---------------------------------------------------------------
// DESC : Memory Map 파일의 기본 주소를 얻음
// PARM : N/A
// RETV : Memory Map 파일의 기본 주소
// PRGM : B4nFter
//*---------------------------------------------------------------
LPVOID CMemoryMap::GetBaseAddress()
{
	return m_lpBaseAddr;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryMap::GetSize
//*---------------------------------------------------------------
// DESC : Memory Map 파일의 크기를 얻음
// PARM : N/A
// RETV : Memory Map 파일의 크기 (INT64)
// PRGM : B4nFter
//*---------------------------------------------------------------
UINT64 CMemoryMap::GetSize()
{
	return MAKELONGLONG(m_dwSizeLow, m_dwSizeHigh);
}

