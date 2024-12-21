

///////////////////////////////////////////////////////////////////
//	INCLUDE
///////////////////////////////////////////////////////////////////

#include "STDAFX.H"
#include "FILESET.H"


///////////////////////////////////////////////////////////////////
//	IMPLEMENTATION
///////////////////////////////////////////////////////////////////

#pragma warning (disable:4238)
CFileSet::FILEBOMTYPEDAT CFileSet::m_stBomTypeDat[EV_BT_NONE] = {
	CFileSet::FILEBOMTYPEDAT(EV_BT_UTF8,		&CVarArgA<UCHAR_MAX>("%c%c%c",		0xEF,0xBB,0xBF),		3),
	CFileSet::FILEBOMTYPEDAT(EV_BT_UTF16_LE,	&CVarArgA<UCHAR_MAX>("%c%c",		0xFF,0xFE),				2),
	CFileSet::FILEBOMTYPEDAT(EV_BT_UTF16_BE,	&CVarArgA<UCHAR_MAX>("%c%c",		0xFE,0xFF),				2),
// 	CFileSet::FILEBOMTYPEDAT(EV_BT_UTF32_LE,	&CVarArgA<UCHAR_MAX>("%c%c%c%c",	0xFF,0xFE,0x00,0x00),	4),
// 	CFileSet::FILEBOMTYPEDAT(EV_BT_UTF32_BE,	&CVarArgA<UCHAR_MAX>("%c%c%c%c",	0x00,0x00,0xFE,0xFF),	4),
};
#pragma warning (default:4238)


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CFileSet::CFileSet
//*---------------------------------------------------------------
// DESC : 생성자
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
CFileSet::CFileSet()
{
	Reset();
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CFileSet::~CFileSet
//*---------------------------------------------------------------
// DESC : 소멸자
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
CFileSet::~CFileSet()
{
	Close();
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CFileSet::Reset
//*---------------------------------------------------------------
// DESC : 파일관련 자원 초기화
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
// P.S.>
//		- 핸들을 닫는것이 아니므로 주의
//*---------------------------------------------------------------
VOID CFileSet::Reset()
{
	m_hFile = INVALID_HANDLE_VALUE;
	::memset(m_szFileFullPath, 0, sizeof(m_szFileFullPath));
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CFileSet::Open
//*---------------------------------------------------------------
// DESC : 파일을 열고 자원을 할당
// PARM :	1 . lpszFileName - 파일의 이름을 포함한 절대/상대 경로
//			2 . bIsReadOnly - 읽기전용 여부
//			3 . bIsAppend - 파일에 데이터를 추가하는 것으로 할지 여부 (bIsReadOnly == FALSE 일 때만 유효)
//			4 . bIsReadShare - 읽기공유 여부
// RETV : NOERROR - 성공 / 그외 - 실패
// PRGM : B4nFter
//*---------------------------------------------------------------
DWORD CFileSet::Open(LPCTSTR lpszFileName, BOOL bIsReadOnly, BOOL bIsAppend, BOOL bIsReadShare)
{
	BASE_ASSERT(FALSE == IsOpen(),		"Invalid!");
	BASE_ASSERT(NULL != lpszFileName,	"Invalid!");

	DWORD dwDesiredAccess;
	DWORD dwShareMode;
	DWORD dwCreationDisposition;
	DWORD dwFlagsAndAttributes;

	if (bIsReadOnly) {
		dwDesiredAccess = GENERIC_READ;
		dwShareMode = FILE_SHARE_READ;
		dwCreationDisposition = OPEN_EXISTING;
		dwFlagsAndAttributes = FILE_FLAG_SEQUENTIAL_SCAN | FILE_ATTRIBUTE_READONLY;
		if (!bIsReadShare) { 
			dwShareMode |= FILE_SHARE_WRITE;
		}
	}
	else {
		dwDesiredAccess = GENERIC_READ | GENERIC_WRITE;
		dwShareMode = FILE_SHARE_READ;
		dwFlagsAndAttributes = FILE_ATTRIBUTE_NORMAL;
		if (bIsAppend) {
			dwCreationDisposition = OPEN_ALWAYS;
		}
		else {
			dwCreationDisposition = CREATE_NEW;
		}
	}

	DWORD dwRetVal = Open(lpszFileName, dwDesiredAccess, dwShareMode, dwCreationDisposition, dwFlagsAndAttributes, NULL, NULL);
	if (NOERROR != dwRetVal) {
		return dwRetVal;
	}

	if (!bIsReadOnly) {
		if (bIsAppend) {
			dwRetVal = Seek(FILE_END, 0, 0);
		}
		else {
			if (FALSE == ::SetEndOfFile(m_hFile)) {
				dwRetVal = ::GetLastError();
			}
		}
	}
	
	return dwRetVal;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CFileSet::Open
//*---------------------------------------------------------------
// DESC : 파일을 열고 자원을 할당
// PARM :	1 . lpszFileName - 파일의 이름을 포함한 절대/상대 경로
//			2 . dwDesiredAccess - 접근모드
//			3 . dwShareMode - 공유모드
//			4 . dwCreationDisposition - 생성계획
//			5 . dwFlagsAndAttributes - 플래그, 속성 (다수지정 가능)
//			6 . lpSecurityAttributes - 보안속성
//			7 . hTemplateFile - 템플릿 파일 핸들
// RETV : NOERROR - 성공 / 그외 - 실패
// PRGM : B4nFter
//*---------------------------------------------------------------
DWORD CFileSet::Open(
  LPCTSTR lpszFileName,	
  DWORD dwDesiredAccess, 
  DWORD dwShareMode, 
  DWORD dwCreationDisposition, 
  DWORD dwFlagsAndAttributes, 
  LPSECURITY_ATTRIBUTES lpSecurityAttributes,
  HANDLE hTemplateFile
  )
{
	BASE_ASSERT(FALSE == IsOpen(),		"Invalid!");
	BASE_ASSERT(NULL != lpszFileName,	"Invalid!");
	if (NULL == lpszFileName) {
		return HASERROR;
	}

	m_hFile = ::CreateFile(lpszFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
	if (INVALID_HANDLE_VALUE == m_hFile) {
		return(::GetLastError());
	}

	TCHAR szFileFullPath[MAX_PATH] = { _T('\0') };
	STRNCPY(szFileFullPath, lpszFileName, _countof(szFileFullPath));
	LPTSTR lpszFilePart;
	DWORD dwRetVal = ::GetFullPathName(szFileFullPath, _countof(m_szFileFullPath), m_szFileFullPath, &lpszFilePart);
	if (0 == dwRetVal) {
		return(::GetLastError());
	}

	return NOERROR;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CFileSet::Close
//*---------------------------------------------------------------
// DESC : 파일핸들을 닫고 객체자원 정리
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
VOID CFileSet::Close()
{
	if (INVALID_HANDLE_VALUE != m_hFile) {
		::CloseHandle(m_hFile);
		Reset();
	}
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CFileSet::Flush
//*---------------------------------------------------------------
// DESC : 현재 접근중인 파일의 쓰기버퍼의 데이터를 즉시 처리하고 비움
// PARM : N/A
// RETV : TRUE - 성공 / FALSE - 실패
// PRGM : B4nFter
//*---------------------------------------------------------------
BOOL CFileSet::Flush()
{
	BASE_ASSERT(TRUE == IsOpen(),	"Invalid!");
	if (FALSE == IsOpen()) {
		return FALSE;
	}

	return(::FlushFileBuffers(m_hFile));
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CFileSet::IsOpen
//*---------------------------------------------------------------
// DESC : 파일객체가 현재 열려있는지 여부 반환
// PARM : N/A
// RETV : TRUE - 성공 / FALSE - 실패
// PRGM : B4nFter
//*---------------------------------------------------------------
BOOL CFileSet::IsOpen() const
{
	return(INVALID_HANDLE_VALUE != m_hFile);
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CFileSet::GetFullPath
//*---------------------------------------------------------------
// DESC : 현재 접근중인 파일의 전체경로를 구함
// PARM :	1 . lpszBuffer - 문자열 출력버퍼
//			2 . nBufferSize - 문자열 출력버퍼의 크기 
// RETV : TRUE - 성공 / FALSE - 실패
// PRGM : B4nFter
//*---------------------------------------------------------------
BOOL CFileSet::GetFullPath(/*out*/ LPTSTR lpszBuffer, DWORD nBufferSize) const
{
	BASE_ASSERT(TRUE == IsOpen(),					"Invalid!");
	BASE_ASSERT(NULL != lpszBuffer,					"Invalid!");
	BASE_ASSERT(0 != nBufferSize,					"Invalid!");
	BASE_ASSERT(_T('\0') != m_szFileFullPath[0],	"Invalid!");
	if (FALSE == IsOpen() ||
		NULL == lpszBuffer ||
		0 == nBufferSize ||
		!_tcsncmp(m_szFileFullPath, _T(""), _countof(m_szFileFullPath))
		)
	{
		return FALSE;
	}

#if	(_MSC_VER >= 1400)	// VS.NET 2005 이상
	return (_tcsncpy_s(lpszBuffer, nBufferSize, m_szFileFullPath, _TRUNCATE)?(TRUE):(FALSE));
#else	// #if	(_MSC_VER >= 1400)
	return (_tcsncpy(lpszBuffer, m_szFileFullPath, nBufferSize)?(TRUE):(FALSE));
#endif	// #if	(_MSC_VER >= 1400)
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CFileSet::GetDrive
//*---------------------------------------------------------------
// DESC : 현재 접근중인 파일의 드라이브를 구함
// PARM :	1 . lpszBuffer - 문자열 출력버퍼
//			2 . nBufferSize - 문자열 출력버퍼의 크기 
// RETV : TRUE - 성공 / FALSE - 실패
// PRGM : B4nFter
//*---------------------------------------------------------------
BOOL CFileSet::GetDrive(/*out*/ LPTSTR lpszBuffer, DWORD nBufferSize) const
{
	BASE_ASSERT(TRUE == IsOpen(),						"Invalid!");
	BASE_ASSERT(NULL != lpszBuffer,					"Invalid!");
	BASE_ASSERT(0 != nBufferSize,						"Invalid!");
	BASE_ASSERT(_T('\0') != m_szFileFullPath[0],		"Invalid!");
	if (FALSE == IsOpen() ||
		NULL == lpszBuffer ||
		0 == nBufferSize ||
		!_tcsncmp(m_szFileFullPath, _T(""), _countof(m_szFileFullPath))
		)
	{
		return FALSE;
	}

#if	(_MSC_VER >= 1400)	// VS.NET 2005 이상
	return (!_tsplitpath_s(m_szFileFullPath, lpszBuffer, nBufferSize, NULL, 0, NULL, 0, NULL, 0)?(TRUE):(FALSE));
#else	// #if	(_MSC_VER >= 1400)
	return (!_tsplitpath(m_szFileFullPath, lpszBuffer, NULL, NULL, NULL)?(TRUE):(FALSE));
#endif	// #if	(_MSC_VER >= 1400)
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CFileSet::GetPath
//*---------------------------------------------------------------
// DESC : 현재 접근중인 파일의 경로를 구함
// PARM :	1 . lpszBuffer - 문자열 출력버퍼
//			2 . nBufferSize - 문자열 출력버퍼의 크기 
// RETV : TRUE - 성공 / FALSE - 실패
// PRGM : B4nFter
//*---------------------------------------------------------------
BOOL CFileSet::GetPath(/*out*/ LPTSTR lpszBuffer, DWORD nBufferSize) const
{
	BASE_ASSERT(TRUE == IsOpen(),						"Invalid!");
	BASE_ASSERT(NULL != lpszBuffer,					"Invalid!");
	BASE_ASSERT(0 != nBufferSize,						"Invalid!");
	BASE_ASSERT(_T('\0') != m_szFileFullPath[0],		"Invalid!");
	if (FALSE == IsOpen() ||
		NULL == lpszBuffer ||
		0 == nBufferSize ||
		!_tcsncmp(m_szFileFullPath, _T(""), _countof(m_szFileFullPath))
		)
	{
		return FALSE;
	}

#if	(_MSC_VER >= 1400)	// VS.NET 2005 이상
	return (!_tsplitpath_s(m_szFileFullPath, NULL, 0, lpszBuffer, nBufferSize, NULL, 0, NULL, 0)?(TRUE):(FALSE));
#else	// #if	(_MSC_VER >= 1400)
	return (!_tsplitpath(m_szFileFullPath, NULL, lpszBuffer, NULL, NULL)?(TRUE):(FALSE));
#endif	// #if	(_MSC_VER >= 1400)
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CFileSet::GetName
//*---------------------------------------------------------------
// DESC : 현재 접근중인 파일의 이름을 구함 (확장자 제외)
// PARM :	1 . lpszBuffer - 문자열 출력버퍼
//			2 . nBufferSize - 문자열 출력버퍼의 크기 
// RETV : TRUE - 성공 / FALSE - 실패
// PRGM : B4nFter
//*---------------------------------------------------------------
BOOL CFileSet::GetName(/*out*/ LPTSTR lpszBuffer, DWORD nBufferSize) const
{
	BASE_ASSERT(TRUE == IsOpen(),						"Invalid!");
	BASE_ASSERT(NULL != lpszBuffer,					"Invalid!");
	BASE_ASSERT(0 != nBufferSize,						"Invalid!");
	BASE_ASSERT(_T('\0') != m_szFileFullPath[0],		"Invalid!");
	if (FALSE == IsOpen() ||
		NULL == lpszBuffer ||
		0 == nBufferSize ||
		!_tcsncmp(m_szFileFullPath, _T(""), _countof(m_szFileFullPath))
		)
	{
		return FALSE;
	}

#if	(_MSC_VER >= 1400)	// VS.NET 2005 이상
	return (!_tsplitpath_s(m_szFileFullPath, NULL, 0, NULL, 0, lpszBuffer, nBufferSize, NULL, 0)?(TRUE):(FALSE));
#else	// #if	(_MSC_VER >= 1400)
	return (!_tsplitpath(m_szFileFullPath, NULL, NULL, lpszBuffer, NULL)?(TRUE):(FALSE));
#endif	// #if	(_MSC_VER >= 1400)
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CFileSet::GetExt
//*---------------------------------------------------------------
// DESC : 현재 접근중인 파일의 확장자를 구함
// PARM :	1 . lpszBuffer - 문자열 출력버퍼
//			2 . nBufferSize - 문자열 출력버퍼의 크기 
// RETV : TRUE - 성공 / FALSE - 실패
// PRGM : B4nFter
//*---------------------------------------------------------------
BOOL CFileSet::GetExt(/*out*/ LPTSTR lpszBuffer, DWORD nBufferSize) const
{
	BASE_ASSERT(TRUE == IsOpen(),						"Invalid!");
	BASE_ASSERT(NULL != lpszBuffer,					"Invalid!");
	BASE_ASSERT(0 != nBufferSize,						"Invalid!");
	BASE_ASSERT(_T('\0') != m_szFileFullPath[0],		"Invalid!");
	if (FALSE == IsOpen() ||
		NULL == lpszBuffer ||
		0 == nBufferSize ||
		!_tcsncmp(m_szFileFullPath, _T(""), _countof(m_szFileFullPath))
		)
	{
		return FALSE;
	}

#if	(_MSC_VER >= 1400)	// VS.NET 2005 이상
	return (!_tsplitpath_s(m_szFileFullPath, NULL, 0, NULL, 0, NULL, 0, lpszBuffer, nBufferSize)?(TRUE):(FALSE));
#else	// #if	(_MSC_VER >= 1400)
	return (!_tsplitpath(m_szFileFullPath, NULL, NULL, NULL, lpszBuffer)?(TRUE):(FALSE));
#endif	// #if	(_MSC_VER >= 1400)
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CFileSet::GetTitle
//*---------------------------------------------------------------
// DESC : 현재 접근중인 파일의 이름 + 확장자를 구함
// PARM :	1 . lpszBuffer - 문자열 출력버퍼
//			2 . nBufferSize - 문자열 출력버퍼의 크기 
// RETV : TRUE - 성공 / FALSE - 실패
// PRGM : B4nFter
//*---------------------------------------------------------------
BOOL CFileSet::GetTitle(/*out*/ LPTSTR lpszBuffer, DWORD nBufferSize) const
{
	BASE_ASSERT(TRUE == IsOpen(),						"Invalid!");
	BASE_ASSERT(NULL != lpszBuffer,					"Invalid!");
	BASE_ASSERT(0 != nBufferSize,						"Invalid!");
	BASE_ASSERT(_T('\0') != m_szFileFullPath[0],		"Invalid!");
	if (FALSE == IsOpen() ||
		NULL == lpszBuffer ||
		0 == nBufferSize ||
		!_tcsncmp(m_szFileFullPath, _T(""), _countof(m_szFileFullPath))
		)
	{
		return FALSE;
	}

	TCHAR szFileName[MAX_PATH] = { _T('\0'), };
	TCHAR szExtName[MAX_PATH] = { _T('\0'), };
#if	(_MSC_VER >= 1400)	// VS.NET 2005 이상
	INT iRetVal = _tsplitpath_s(m_szFileFullPath, NULL, 0, NULL, 0, szFileName, _countof(szFileName), szExtName, _countof(szExtName));
#else	// #if	(_MSC_VER >= 1400)
	INT iRetVal = _tsplitpath(m_szFileFullPath, NULL, NULL, szFileName, szExtName);
#endif	// #if	(_MSC_VER >= 1400)
	if (iRetVal) {
		return FALSE;
	}

#if	(_MSC_VER >= 1400)	// VS.NET 2005 이상
	iRetVal = _stprintf_s(lpszBuffer, nBufferSize, _T("%s%s"), szFileName, szExtName);
#else	// #if	(_MSC_VER >= 1400)
	iRetVal = _sntprintf(lpszBuffer, nBufferSize, _T("%s%s"), szFileName, szExtName);
#endif	// #if	(_MSC_VER >= 1400)
	if (iRetVal < 0) {
		return FALSE;
	}

	return TRUE;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CFileSet::Seek
//*---------------------------------------------------------------
// DESC : 현재 접근중인 파일의 특정 위치에 파일포인터를 위치시킴
// PARM :	1 . dwMoveMethod - 파일포인터 이동방법
/*
				FILE_BEGIN : 파일의 특정위치를 파일의 처음부터 찾음
				FILE_CURRENT : 파일의 특정위치를 파일의 현재 위치부터 찾음
				FILE_END : 파일의 특정위치를 파일의 끝부터 찾음
*/
//			2 . lOffsetLow - 파일포인터를 이동시킬 상대위치 (하위 LONG)
//			3 . lOffsetHigh - 파일포인터를 이동시킬 상대위치 (상위 LONG)
//			4 . lpu64Offset - 이동된 파일포인터의 위치값
// RETV : NOERROR - 성공 / 그외 - 실패
// PRGM : B4nFter
//*---------------------------------------------------------------
DWORD CFileSet::Seek(DWORD dwMoveMethod, LONG lOffsetLow, LONG lOffsetHigh, ULONGLONG* lpu64Offset)
{
	BASE_ASSERT(TRUE == IsOpen(),		"Invalid!");
	if (FALSE == IsOpen()) {
		return HASERROR;
	}

	LPLONG lpOffsetHigh = NULL;
	if (lOffsetHigh > 0) {
		lpOffsetHigh = &lOffsetHigh;
	}

	DWORD dwOffsetLow = ::SetFilePointer(m_hFile, lOffsetLow, lpOffsetHigh, dwMoveMethod);
	if (INVALID_SET_FILE_POINTER == dwOffsetLow) {
		DWORD dwErrNo = ::GetLastError();
		if (NO_ERROR != dwErrNo) {
			return dwErrNo;
		}
	}
	if (lpu64Offset) {
		if (lpOffsetHigh) {
			*lpu64Offset = MAKELONGLONG(dwOffsetLow, *lpOffsetHigh);
		}
		else {
			*lpu64Offset = MAKELONGLONG(dwOffsetLow, 0);
		}
	}
	return NOERROR;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CFileSet::SetLength
//*---------------------------------------------------------------
// DESC : 현재 접근중인 파일의 크기를 특정 수치만큼 강제조정
// PARM :	1 . dwFileLength - 조정할 파일의 크기
// RETV : NOERROR - 성공 / 그외 - 실패
// PRGM : B4nFter
//*---------------------------------------------------------------
DWORD CFileSet::SetLength(ULONGLONG dwFileLength)
{
	BASE_ASSERT(IsOpen(),				"Invalid!");

	DWORD dwRetVal = Seek(FILE_BEGIN, LOLONG(dwFileLength), HILONG(dwFileLength));
	if (NOERROR != dwRetVal) {
		return dwRetVal;
	}

	if (!::SetEndOfFile(m_hFile)) {
		dwRetVal = ::GetLastError();
		if (::GetLastError() != NO_ERROR) {
			return dwRetVal;
		}
	}

	return NOERROR;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CFileSet::GetLength
//*---------------------------------------------------------------
// DESC : 현재 접근중인 파일의 크기를 얻어옴
// PARM :	1 . dwFileLength - 얻은 파일의 크기
// RETV : NOERROR - 성공 / 그외 - 실패
// PRGM : B4nFter
//*---------------------------------------------------------------
DWORD CFileSet::GetLength(/*out*/ ULONGLONG& dwFileLength) const
{
	BASE_ASSERT(IsOpen(),				"Invalid!");

	ULARGE_INTEGER unFileSize;
	unFileSize.LowPart = ::GetFileSize(m_hFile, &unFileSize.HighPart);
	if (unFileSize.LowPart == INVALID_FILE_SIZE) {
		DWORD dwRetVal = ::GetLastError();
		if (NO_ERROR != dwRetVal) {
			return dwRetVal;
		}
	}

	dwFileLength = unFileSize.QuadPart;

	return NOERROR;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CFileSet::Read
//*---------------------------------------------------------------
// DESC : 현재 접근중인 파일의 현재 파일포인터부터 데이터를 읽음
// PARM :	1 . lpBuffer - 데이터를 읽을 버퍼포인터
//			2 . nBufferSize - 버퍼의 크기
// RETV : NOERROR - 성공 / ERROR_HANDLE_EOF - 파일읽기종료 / 그외 - 실패
// PRGM : B4nFter
//*---------------------------------------------------------------
DWORD CFileSet::Read(/*out*/ LPVOID lpBuffer, /*int,out*/ DWORD& nBufferSize)
{
	BASE_ASSERT(TRUE == IsOpen(),		"Invalid!");
	BASE_ASSERT(NULL != lpBuffer,		"Invalid!");
	BASE_ASSERT(0 != nBufferSize,		"Invalid!");
	if (FALSE == IsOpen() ||
		NULL == lpBuffer ||
		0 == nBufferSize
		)
	{
		return HASERROR;
	}
	
	DWORD dwRetVal = ::ReadFile(m_hFile, lpBuffer, nBufferSize, &nBufferSize, NULL);
	if (0 == dwRetVal) {
		return(::GetLastError());
	}

	return NOERROR;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CFileSet::Write
//*---------------------------------------------------------------
// DESC : 현재 접근중인 파일의 현재 파일포인터부터 데이터를 기록
// PARM :	1 . lpBuffer - 기록할 데이터를 가진 버퍼포인터
//			2 . nBufferSize - 버퍼의 크기
// RETV : NOERROR - 성공 / 그외 - 실패
// PRGM : B4nFter
//*---------------------------------------------------------------
DWORD CFileSet::Write(LPCVOID lpBuffer, /*int,out*/ DWORD& nBufferSize)
{
	BASE_ASSERT(TRUE == IsOpen(),		"Invalid!");
	BASE_ASSERT(NULL != lpBuffer,		"Invalid!");
	BASE_ASSERT(0 != nBufferSize,		"Invalid!");
	if (FALSE == IsOpen() ||
		NULL == lpBuffer ||
		0 == nBufferSize
		)
	{
		return HASERROR;
	}

	DWORD dwRetVal = ::WriteFile(m_hFile, lpBuffer, nBufferSize, &nBufferSize, NULL);
	if (0 == dwRetVal) {
		return(::GetLastError());
	}

	return NOERROR;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CFileSet::LockRange
//*---------------------------------------------------------------
// DESC : 현재 접근중인 파일의 특정 위치를 다른 프로세스들이 접근하지 못하도록 배타적 잠김을 설정
// PARM :	1 . uOffset - 파일의 시작위치로 부터의 상대위치
//			2 . uNumberOfBytes - 잠김을 수행할 바이트 수
// RETV : NOERROR - 성공 / 그외 - 실패
// PRGM : B4nFter
//*---------------------------------------------------------------
DWORD CFileSet::LockRange(ULONGLONG uOffset, ULONGLONG uNumberOfBytes)
{
	BASE_ASSERT(IsOpen(),				"Invalid!");
	BASE_ASSERT(0 != uNumberOfBytes,	"Invalid!");

	ULARGE_INTEGER unULargeInterger;
	ULARGE_INTEGER unCount;

	unULargeInterger.QuadPart = uOffset;
	unCount.QuadPart = uNumberOfBytes;
	if (!::LockFile(m_hFile, unULargeInterger.LowPart, unULargeInterger.HighPart, unCount.LowPart, unCount.HighPart)) {
		return (::GetLastError());
	}

	return NOERROR;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CFileSet::SkipBOM
//*---------------------------------------------------------------
// DESC : 문서파일의 인코딩형태를 나타내는 BOM (Byte Order Mark) 가 존재하면 넘김
// PARM : N/A
// RETV : BOM 타입 (EV_BT_NONE 이면 속해있지 않음)
// PRGM : B4nFter
// P.S.>
//		- 필연적으로 파일포인터는 맨 앞으로 이동되고 이후에 BOM의 크기만큼 이동, 최종적으로 파일포인터는 BOM 뒤에 위치
//		- BOM이 없는 경우 파일포인터는 맨 앞으로 이동
//*---------------------------------------------------------------
CFileSet::EF_BOMTYPE CFileSet::SkipBOM()
{
	BASE_ASSERT(IsOpen(),				"Invalid!");

	if (!IsOpen()) {
		return EV_BT_NONE;
	}

	BYTE btData[UCHAR_MAX];
	DWORD dwSize, dwRetVal;
	for (DWORD dwIndex = 0 ; dwIndex < EV_BT_NONE ; ++dwIndex) {
		dwRetVal = Seek(FILE_BEGIN, 0, 0);
		if (NOERROR != dwRetVal) {
			BASE_CONTINUE;
		}
		dwSize = m_stBomTypeDat[dwIndex].m_nSize;
		dwRetVal = Read(btData, dwSize);
		if (NOERROR != dwRetVal) {
			BASE_CONTINUE;
		}
		if (dwSize != m_stBomTypeDat[dwIndex].m_nSize) {
			continue;
		}
		if (!::memcmp(btData, m_stBomTypeDat[dwIndex].m_btData, m_stBomTypeDat[dwIndex].m_nSize)) {
			return m_stBomTypeDat[dwIndex].m_eType;
		}
	}

	return EV_BT_NONE;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CFileSet::WriteBOM
//*---------------------------------------------------------------
// DESC : 파일의 맨 앞에 특정 BOM (Byte Order Mark) 을 남김
// PARM :	1 . eBomType - 쓰기를 원하는 BOM 타입
// RETV : NOERROR - 성공 / 그외 - 실패
// PRGM : B4nFter
//*---------------------------------------------------------------
DWORD CFileSet::WriteBOM(CFileSet::EF_BOMTYPE eBomType)
{
	BASE_ASSERT(IsOpen(),								"Invalid!");
	BASE_ASSERT(CHECK_LIMIT(eBomType, EV_BT_NONE),	"Invalid!");

	if (!IsOpen() ||
		!CHECK_LIMIT(eBomType, EV_BT_NONE)
		)
	{
		return HASERROR;
	}

	DWORD dwRetVal = Seek(FILE_BEGIN, 0, 0);
	if (NOERROR != dwRetVal) {
		BASE_RETURN(dwRetVal);
	}

	DWORD dwSize = m_stBomTypeDat[eBomType].m_nSize;
	dwRetVal = Write(m_stBomTypeDat[eBomType].m_btData, dwSize);
	if (NOERROR != dwRetVal) {
		BASE_RETURN(dwRetVal);
	}
	if (dwSize != m_stBomTypeDat[eBomType].m_nSize) {
		BASE_RETURN(HASERROR);
	}

	return NOERROR;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CFileSet::UnlockRange
//*---------------------------------------------------------------
// DESC : 이전에 Lock 되었던 파일의 특정 위치의 배타적 잠김을 해제
// PARM :	1 . uOffset - 파일의 시작위치로 부터의 상대위치
//			2 . uNumberOfBytes - 잠김해제를 수행할 바이트 수
// RETV : NOERROR - 성공 / 그외 - 실패
// PRGM : B4nFter
//*---------------------------------------------------------------
DWORD CFileSet::UnlockRange(ULONGLONG uOffset, ULONGLONG uNumberOfBytes)
{
	BASE_ASSERT(IsOpen(),				"Invalid!");
	BASE_ASSERT(0 != uNumberOfBytes,	"Invalid!");

	ULARGE_INTEGER unULargeInterger;
	ULARGE_INTEGER unCount;

	unULargeInterger.QuadPart = uOffset;
	unCount.QuadPart = uNumberOfBytes;
	if (!::UnlockFile(m_hFile, unULargeInterger.LowPart, unULargeInterger.HighPart, unCount.LowPart, unCount.HighPart)) {
		return (::GetLastError());
	}

	return NOERROR;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CFileSet::GetPosition
//*---------------------------------------------------------------
// DESC : 현재 접근중인 파일의 파일포인터 위치를 얻어옴
// PARM :	1 . u64FilePositon - 파일포인터 위치를 얻어올 변수
// RETV : NOERROR - 성공 / 그외 - 실패
// PRGM : B4nFter
//*---------------------------------------------------------------
DWORD CFileSet::GetPosition(/*out*/ ULONGLONG& u64FilePositon) const
{
	BASE_ASSERT(IsOpen(),				"Invalid!");

	LARGE_INTEGER unLargeInteger;
	unLargeInteger.QuadPart = 0;
	unLargeInteger.LowPart = ::SetFilePointer(m_hFile, unLargeInteger.LowPart, &unLargeInteger.HighPart , FILE_CURRENT);
	if (unLargeInteger.LowPart == (DWORD)-1) {
		DWORD dwErrNo = ::GetLastError();
		if (NO_ERROR != dwErrNo) {
			return dwErrNo;
		}
	}

	u64FilePositon = unLargeInteger.QuadPart;

	return NOERROR;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CFileSet::GetTime
//*---------------------------------------------------------------
// DESC : 현재 접근중인 파일의 파일시간을 얻어옴
// PARM :	1 . lpstCreateFileTime - 생성시간
//			2 . lpstAccessFileTime - 마지막 접근시간
//			3 . lpstWriteFileTime - 마지막 기록시간
// RETV : NOERROR - 성공 / 그외 - 실패
// PRGM : B4nFter
//*---------------------------------------------------------------
DWORD CFileSet::GetTime(/*out*/ LPFILETIME lpstCreateFileTime, /*out*/ LPFILETIME lpstAccessFileTime, /*out*/ LPFILETIME lpstWriteFileTime) const
{
	BASE_ASSERT(NULL != lpstCreateFileTime,		"Invalid!");
	BASE_ASSERT(NULL != lpstAccessFileTime,		"Invalid!");
	BASE_ASSERT(NULL != lpstWriteFileTime,		"Invalid!");

	DWORD dwRetVal = 0;
	if (INVALID_HANDLE_VALUE != m_hFile)
	{
		if (!::GetFileTime(m_hFile, lpstCreateFileTime, lpstAccessFileTime, lpstWriteFileTime))
		{
			dwRetVal = ::GetLastError();
			BASE_ASSERT(0, "Error!");
		}
//		DN_VERIFY(::CloseHandle(m_hFile), TRUE,	"error!");
	}
	else
	{
		BASE_RETURN(HASERROR);
	}

	return(dwRetVal);
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CFileSet::GetTime
//*---------------------------------------------------------------
// DESC : 특정 경로의 파일의 파일시간을 얻어옴 (정적 메서드)
// PARM :	1 . lpszFileName - 파일의 이름을 포함한 절대/상대 경로
//			2 . lpstCreateFileTime - 생성시간
//			3 . lpstAccessFileTime - 마지막 접근시간
//			4 . lpstWriteFileTime - 마지막 기록시간
// RETV : NOERROR - 성공 / 그외 - 실패
// PRGM : B4nFter
//*---------------------------------------------------------------
DWORD CFileSet::GetTime(LPCTSTR lpszFileName, /*out*/ LPFILETIME lpstCreateFileTime, /*out*/ LPFILETIME lpstAccessFileTime, /*out*/ LPFILETIME lpstWriteFileTime)
{
	BASE_ASSERT(NULL != lpstCreateFileTime,		"Invalid!");
	BASE_ASSERT(NULL != lpstCreateFileTime,		"Invalid!");
	BASE_ASSERT(NULL != lpstAccessFileTime,		"Invalid!");
	BASE_ASSERT(NULL != lpstWriteFileTime,		"Invalid!");

	DWORD dwRetVal = 0;
	HANDLE hFile = ::CreateFile(lpszFileName, FILE_READ_ATTRIBUTES, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE != hFile)
	{
		if (!::GetFileTime(hFile, lpstCreateFileTime, lpstAccessFileTime, lpstWriteFileTime))
		{
			dwRetVal = ::GetLastError();
			BASE_ASSERT(0, "Error!");
		}
		BASE_VERIFY(::CloseHandle(hFile), TRUE,	"Error!");
	}
	else
	{
		BASE_RETURN(HASERROR);
	}

	return(dwRetVal);
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CFileSet::GetVersion
//*---------------------------------------------------------------
// DESC : 특정 경로의 파일의 버전정보를 얻어옴 (정적 메서드)
// PARM :	1 . lpszFileName - 파일의 이름을 포함한 절대/상대 경로
//			2 . dwFileVersion1 - 버전정보 1
//			3 . dwFileVersion2 - 버전정보 2
//			4 . dwFileVersion3 - 버전정보 3
//			5 . dwFileVersion4 - 버전정보 4
// RETV : NOERROR - 성공 / 그외 - 실패 (파일 바이너리에 버전 리소스가 없으면 ERROR_RESOURCE_TYPE_NOT_FOUND (1813L) 반환)
// PRGM : B4nFter
// P.S.>
//		- 파일 바이너리에 버전 리소스가 이미 존재해야 함
//*---------------------------------------------------------------
DWORD CFileSet::GetVersion(LPCTSTR lpszFileName, DWORD& dwFileVersion1, DWORD& dwFileVersion2, DWORD& dwFileVersion3, DWORD& dwFileVersion4)
{
	DWORD dwHandle= 0;
	DWORD dwFileVersionInfoSize = ::GetFileVersionInfoSize(lpszFileName, &dwHandle);
	if (0 == dwFileVersionInfoSize)
	{
		return (::GetLastError());	// Version Resource가 없으면 ERROR_RESOURCE_TYPE_NOT_FOUND (1813L)
	}

	std::auto_ptr<BYTE>	aVerBuf(NEW BYTE[dwFileVersionInfoSize]);
	if (NULL == aVerBuf.get())
	{
		return ERROR_NOT_ENOUGH_MEMORY;
	}

	DWORD	dwRetVal = 0;
	if (TRUE == ::GetFileVersionInfo(lpszFileName, dwHandle, dwFileVersionInfoSize, aVerBuf.get()))
	{
		UINT uLength    = 0;
		VS_FIXEDFILEINFO* lpVsFixedFileInfo = NULL;
		if (TRUE == ::VerQueryValue(aVerBuf.get(), _T("\\"), (LPVOID*)&lpVsFixedFileInfo, &uLength))
		{
			BASE_ASSERT(sizeof(VS_FIXEDFILEINFO) == uLength,	"Error!");
			dwFileVersion1	= HIWORD(lpVsFixedFileInfo->dwFileVersionMS);
			dwFileVersion2	= LOWORD(lpVsFixedFileInfo->dwFileVersionMS);
			dwFileVersion3	= HIWORD(lpVsFixedFileInfo->dwFileVersionLS);
			dwFileVersion4	= LOWORD(lpVsFixedFileInfo->dwFileVersionLS);
		}
		else
		{
			dwRetVal = ERROR_NOT_FOUND;	// 1168L
		}
	}
	else
	{
		dwRetVal = ::GetLastError();
	}

	return dwRetVal;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CFileSet::GetAttributes
//*---------------------------------------------------------------
// DESC : 특정 경로의 파일의 속성정보를 가져옴 (정적 메서드)
// PARM :	1 . lpszFileName - 파일의 이름을 포함한 절대/상대 경로
// RETV : 파일의 속성 : 성공 / INVALID_FILE_ATTRIBUTES : 실패
// PRGM : B4nFter
//*---------------------------------------------------------------
DWORD CFileSet::GetAttributes(LPCTSTR lpszFileName)
{
	BASE_ASSERT(NULL != lpszFileName,		"Invalid!");
	return (::GetFileAttributes(lpszFileName));
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CFileSet::SetAttributes
//*---------------------------------------------------------------
// DESC : 특정 경로의 파일의 속성을 지정 (정적 메서드)
// PARM :	1 . lpszFileName - 파일의 이름을 포함한 절대/상대 경로
//			2 . dwFileAttributes - 파일 속성
/*
				FILE_ATTRIBUTE_READONLY             0x00000001
				FILE_ATTRIBUTE_HIDDEN               0x00000002
				FILE_ATTRIBUTE_SYSTEM               0x00000004
				FILE_ATTRIBUTE_DIRECTORY            0x00000010
				FILE_ATTRIBUTE_ARCHIVE              0x00000020
				FILE_ATTRIBUTE_DEVICE               0x00000040
				FILE_ATTRIBUTE_NORMAL               0x00000080
				FILE_ATTRIBUTE_TEMPORARY            0x00000100
				FILE_ATTRIBUTE_SPARSE_FILE          0x00000200
				FILE_ATTRIBUTE_REPARSE_POINT        0x00000400
				FILE_ATTRIBUTE_COMPRESSED           0x00000800
				FILE_ATTRIBUTE_OFFLINE              0x00001000
				FILE_ATTRIBUTE_NOT_CONTENT_INDEXED  0x00002000
				FILE_ATTRIBUTE_ENCRYPTED            0x00004000
*/
// RETV : TRUE - 성공 / FALSE - 실패
// PRGM : B4nFter
//*---------------------------------------------------------------
BOOL CFileSet::SetAttributes(LPCTSTR lpszFileName, DWORD dwFileAttributes)
{
	BASE_ASSERT(NULL != lpszFileName,			"Invalid!");
	return (::SetFileAttributes(lpszFileName, dwFileAttributes));
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CFileSet::Remove
//*---------------------------------------------------------------
// DESC : 특정 경로의 파일을 제거 (정적 메서드)
// PARM :	1 . lpszSrcFileName - 원본 파일
//			2 . lpszTgtFileName - 목적 파일
//			3 . pIsFailIfExist - 기존에 파일이 있으면 실패로 처리할지 여부 (TRUE:실패 / FALSE:성공(덮어씀))
// RETV : TRUE - 성공 / FALSE - 실패
// PRGM : B4nFter
//*---------------------------------------------------------------
BOOL CFileSet::Copy(LPCTSTR lpszSrcFileName, LPCTSTR lpszTgtFileName, BOOL pIsFailIfExist)
{
	BASE_ASSERT(NULL != lpszSrcFileName,	"Invalid!");
	BASE_ASSERT(NULL != lpszTgtFileName,	"Invalid!");

	return (::CopyFile(lpszSrcFileName, lpszTgtFileName, pIsFailIfExist));
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CFileSet::Remove
//*---------------------------------------------------------------
// DESC : 특정 경로의 파일을 제거 (정적 메서드)
// PARM :	1 . lpszFileName - 파일의 이름을 포함한 절대/상대 경로
// RETV : TRUE - 성공 / FALSE - 실패
// PRGM : B4nFter
//*---------------------------------------------------------------
BOOL CFileSet::Remove(LPCTSTR lpszFileName)
{
	BASE_ASSERT(NULL != lpszFileName,			"Invalid!");

	return (::DeleteFile(lpszFileName));
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CFileSet::Rename
//*---------------------------------------------------------------
// DESC : 특정 경로의 파일의 이름과 경로를 변경 (정적 메서드)
// PARM :	1 . lpszOldFileName - 이전 파일의 이름을 포함한 절대/상대 경로
//			2 . lpszNewFileName - 신규 파일의 이름을 포함한 절대/상대 경로
// RETV : TRUE - 성공 / FALSE - 실패
// PRGM : B4nFter
//*---------------------------------------------------------------
BOOL CFileSet::Rename(LPCTSTR lpszOldFileName, LPCTSTR lpszNewFileName)
{
	BASE_ASSERT(NULL != lpszOldFileName,		"Invalid!");
	BASE_ASSERT(NULL != lpszNewFileName,		"Invalid!");
	
	return (::MoveFile(lpszOldFileName, lpszNewFileName));
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CFileSet::GetTempName
//*---------------------------------------------------------------
// DESC : 특정 경로와 접두사, 숫자를 기준으로 임시파일 이름을 생성 (정적 메서드)
// PARM :	1 . lpPathName - 기준 경로
//			2 . lpPrefixString - 기준 파일 접두사
//			3 . uUnique - 기준 숫자
//			4 . lpTempFileName - 임시 파일이름을 얻어올 버퍼
// RETV : TRUE - 성공 / FALSE - 실패
// PRGM : B4nFter
//*---------------------------------------------------------------
BOOL CFileSet::GetTempName(LPCTSTR lpPathName, LPCTSTR lpPrefixString, UINT uUnique, /*out*/ LPTSTR lpTempFileName)
{
	BASE_ASSERT(NULL != lpPathName,				"Invalid!");
	BASE_ASSERT(NULL != lpPrefixString,			"Invalid!");
	BASE_ASSERT(NULL != lpTempFileName,			"Invalid!");
	if (NULL == lpPathName) { return FALSE; }
	if (NULL == lpPrefixString) { return FALSE; }
	if (NULL == lpTempFileName) { return FALSE; }

	UINT uRetVal = ::GetTempFileName(lpPathName, lpPrefixString, uUnique, lpTempFileName);
	if (0 == uRetVal) {
		return FALSE;
	}

	return TRUE;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CFileSet::GetBomTypeDat
//*---------------------------------------------------------------
// DESC : 특정 BOM 타입에 대한 정보를 얻음
// PARM :	1 . pBomType - 특정 BOM 타입에 대한 EF_BOMTYPE 열거형 값
// RETV : 특정 BOM 타입에 대한 정보 포인터
// PRGM : B4nFter
//*---------------------------------------------------------------
CFileSet::LPFILEBOMTYPEDAT CFileSet::GetBomTypeDat(EF_BOMTYPE pBomType)
{
	if (!CHECK_LIMIT(pBomType, EV_BT_NONE)) {
		BASE_RETURN(NULL);
	}

	return &m_stBomTypeDat[pBomType];
}

