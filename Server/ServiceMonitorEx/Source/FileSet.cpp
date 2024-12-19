

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
// DESC : ������
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
// DESC : �Ҹ���
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
// DESC : ���ϰ��� �ڿ� �ʱ�ȭ
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
// P.S.>
//		- �ڵ��� �ݴ°��� �ƴϹǷ� ����
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
// DESC : ������ ���� �ڿ��� �Ҵ�
// PARM :	1 . lpszFileName - ������ �̸��� ������ ����/��� ���
//			2 . bIsReadOnly - �б����� ����
//			3 . bIsAppend - ���Ͽ� �����͸� �߰��ϴ� ������ ���� ���� (bIsReadOnly == FALSE �� ���� ��ȿ)
//			4 . bIsReadShare - �б���� ����
// RETV : NOERROR - ���� / �׿� - ����
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
// DESC : ������ ���� �ڿ��� �Ҵ�
// PARM :	1 . lpszFileName - ������ �̸��� ������ ����/��� ���
//			2 . dwDesiredAccess - ���ٸ��
//			3 . dwShareMode - �������
//			4 . dwCreationDisposition - ������ȹ
//			5 . dwFlagsAndAttributes - �÷���, �Ӽ� (�ټ����� ����)
//			6 . lpSecurityAttributes - ���ȼӼ�
//			7 . hTemplateFile - ���ø� ���� �ڵ�
// RETV : NOERROR - ���� / �׿� - ����
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
// DESC : �����ڵ��� �ݰ� ��ü�ڿ� ����
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
// DESC : ���� �������� ������ ��������� �����͸� ��� ó���ϰ� ���
// PARM : N/A
// RETV : TRUE - ���� / FALSE - ����
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
// DESC : ���ϰ�ü�� ���� �����ִ��� ���� ��ȯ
// PARM : N/A
// RETV : TRUE - ���� / FALSE - ����
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
// DESC : ���� �������� ������ ��ü��θ� ����
// PARM :	1 . lpszBuffer - ���ڿ� ��¹���
//			2 . nBufferSize - ���ڿ� ��¹����� ũ�� 
// RETV : TRUE - ���� / FALSE - ����
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

#if	(_MSC_VER >= 1400)	// VS.NET 2005 �̻�
	return (_tcsncpy_s(lpszBuffer, nBufferSize, m_szFileFullPath, _TRUNCATE)?(TRUE):(FALSE));
#else	// #if	(_MSC_VER >= 1400)
	return (_tcsncpy(lpszBuffer, m_szFileFullPath, nBufferSize)?(TRUE):(FALSE));
#endif	// #if	(_MSC_VER >= 1400)
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CFileSet::GetDrive
//*---------------------------------------------------------------
// DESC : ���� �������� ������ ����̺긦 ����
// PARM :	1 . lpszBuffer - ���ڿ� ��¹���
//			2 . nBufferSize - ���ڿ� ��¹����� ũ�� 
// RETV : TRUE - ���� / FALSE - ����
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

#if	(_MSC_VER >= 1400)	// VS.NET 2005 �̻�
	return (!_tsplitpath_s(m_szFileFullPath, lpszBuffer, nBufferSize, NULL, 0, NULL, 0, NULL, 0)?(TRUE):(FALSE));
#else	// #if	(_MSC_VER >= 1400)
	return (!_tsplitpath(m_szFileFullPath, lpszBuffer, NULL, NULL, NULL)?(TRUE):(FALSE));
#endif	// #if	(_MSC_VER >= 1400)
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CFileSet::GetPath
//*---------------------------------------------------------------
// DESC : ���� �������� ������ ��θ� ����
// PARM :	1 . lpszBuffer - ���ڿ� ��¹���
//			2 . nBufferSize - ���ڿ� ��¹����� ũ�� 
// RETV : TRUE - ���� / FALSE - ����
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

#if	(_MSC_VER >= 1400)	// VS.NET 2005 �̻�
	return (!_tsplitpath_s(m_szFileFullPath, NULL, 0, lpszBuffer, nBufferSize, NULL, 0, NULL, 0)?(TRUE):(FALSE));
#else	// #if	(_MSC_VER >= 1400)
	return (!_tsplitpath(m_szFileFullPath, NULL, lpszBuffer, NULL, NULL)?(TRUE):(FALSE));
#endif	// #if	(_MSC_VER >= 1400)
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CFileSet::GetName
//*---------------------------------------------------------------
// DESC : ���� �������� ������ �̸��� ���� (Ȯ���� ����)
// PARM :	1 . lpszBuffer - ���ڿ� ��¹���
//			2 . nBufferSize - ���ڿ� ��¹����� ũ�� 
// RETV : TRUE - ���� / FALSE - ����
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

#if	(_MSC_VER >= 1400)	// VS.NET 2005 �̻�
	return (!_tsplitpath_s(m_szFileFullPath, NULL, 0, NULL, 0, lpszBuffer, nBufferSize, NULL, 0)?(TRUE):(FALSE));
#else	// #if	(_MSC_VER >= 1400)
	return (!_tsplitpath(m_szFileFullPath, NULL, NULL, lpszBuffer, NULL)?(TRUE):(FALSE));
#endif	// #if	(_MSC_VER >= 1400)
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CFileSet::GetExt
//*---------------------------------------------------------------
// DESC : ���� �������� ������ Ȯ���ڸ� ����
// PARM :	1 . lpszBuffer - ���ڿ� ��¹���
//			2 . nBufferSize - ���ڿ� ��¹����� ũ�� 
// RETV : TRUE - ���� / FALSE - ����
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

#if	(_MSC_VER >= 1400)	// VS.NET 2005 �̻�
	return (!_tsplitpath_s(m_szFileFullPath, NULL, 0, NULL, 0, NULL, 0, lpszBuffer, nBufferSize)?(TRUE):(FALSE));
#else	// #if	(_MSC_VER >= 1400)
	return (!_tsplitpath(m_szFileFullPath, NULL, NULL, NULL, lpszBuffer)?(TRUE):(FALSE));
#endif	// #if	(_MSC_VER >= 1400)
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CFileSet::GetTitle
//*---------------------------------------------------------------
// DESC : ���� �������� ������ �̸� + Ȯ���ڸ� ����
// PARM :	1 . lpszBuffer - ���ڿ� ��¹���
//			2 . nBufferSize - ���ڿ� ��¹����� ũ�� 
// RETV : TRUE - ���� / FALSE - ����
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
#if	(_MSC_VER >= 1400)	// VS.NET 2005 �̻�
	INT iRetVal = _tsplitpath_s(m_szFileFullPath, NULL, 0, NULL, 0, szFileName, _countof(szFileName), szExtName, _countof(szExtName));
#else	// #if	(_MSC_VER >= 1400)
	INT iRetVal = _tsplitpath(m_szFileFullPath, NULL, NULL, szFileName, szExtName);
#endif	// #if	(_MSC_VER >= 1400)
	if (iRetVal) {
		return FALSE;
	}

#if	(_MSC_VER >= 1400)	// VS.NET 2005 �̻�
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
// DESC : ���� �������� ������ Ư�� ��ġ�� ���������͸� ��ġ��Ŵ
// PARM :	1 . dwMoveMethod - ���������� �̵����
/*
				FILE_BEGIN : ������ Ư����ġ�� ������ ó������ ã��
				FILE_CURRENT : ������ Ư����ġ�� ������ ���� ��ġ���� ã��
				FILE_END : ������ Ư����ġ�� ������ ������ ã��
*/
//			2 . lOffsetLow - ���������͸� �̵���ų �����ġ (���� LONG)
//			3 . lOffsetHigh - ���������͸� �̵���ų �����ġ (���� LONG)
//			4 . lpu64Offset - �̵��� ������������ ��ġ��
// RETV : NOERROR - ���� / �׿� - ����
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
// DESC : ���� �������� ������ ũ�⸦ Ư�� ��ġ��ŭ ��������
// PARM :	1 . dwFileLength - ������ ������ ũ��
// RETV : NOERROR - ���� / �׿� - ����
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
// DESC : ���� �������� ������ ũ�⸦ ����
// PARM :	1 . dwFileLength - ���� ������ ũ��
// RETV : NOERROR - ���� / �׿� - ����
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
// DESC : ���� �������� ������ ���� ���������ͺ��� �����͸� ����
// PARM :	1 . lpBuffer - �����͸� ���� ����������
//			2 . nBufferSize - ������ ũ��
// RETV : NOERROR - ���� / ERROR_HANDLE_EOF - �����б����� / �׿� - ����
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
// DESC : ���� �������� ������ ���� ���������ͺ��� �����͸� ���
// PARM :	1 . lpBuffer - ����� �����͸� ���� ����������
//			2 . nBufferSize - ������ ũ��
// RETV : NOERROR - ���� / �׿� - ����
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
// DESC : ���� �������� ������ Ư�� ��ġ�� �ٸ� ���μ������� �������� ���ϵ��� ��Ÿ�� ����� ����
// PARM :	1 . uOffset - ������ ������ġ�� ������ �����ġ
//			2 . uNumberOfBytes - ����� ������ ����Ʈ ��
// RETV : NOERROR - ���� / �׿� - ����
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
// DESC : ���������� ���ڵ����¸� ��Ÿ���� BOM (Byte Order Mark) �� �����ϸ� �ѱ�
// PARM : N/A
// RETV : BOM Ÿ�� (EV_BT_NONE �̸� �������� ����)
// PRGM : B4nFter
// P.S.>
//		- �ʿ������� ���������ʹ� �� ������ �̵��ǰ� ���Ŀ� BOM�� ũ�⸸ŭ �̵�, ���������� ���������ʹ� BOM �ڿ� ��ġ
//		- BOM�� ���� ��� ���������ʹ� �� ������ �̵�
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
// DESC : ������ �� �տ� Ư�� BOM (Byte Order Mark) �� ����
// PARM :	1 . eBomType - ���⸦ ���ϴ� BOM Ÿ��
// RETV : NOERROR - ���� / �׿� - ����
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
// DESC : ������ Lock �Ǿ��� ������ Ư�� ��ġ�� ��Ÿ�� ����� ����
// PARM :	1 . uOffset - ������ ������ġ�� ������ �����ġ
//			2 . uNumberOfBytes - ��������� ������ ����Ʈ ��
// RETV : NOERROR - ���� / �׿� - ����
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
// DESC : ���� �������� ������ ���������� ��ġ�� ����
// PARM :	1 . u64FilePositon - ���������� ��ġ�� ���� ����
// RETV : NOERROR - ���� / �׿� - ����
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
// DESC : ���� �������� ������ ���Ͻð��� ����
// PARM :	1 . lpstCreateFileTime - �����ð�
//			2 . lpstAccessFileTime - ������ ���ٽð�
//			3 . lpstWriteFileTime - ������ ��Ͻð�
// RETV : NOERROR - ���� / �׿� - ����
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
// DESC : Ư�� ����� ������ ���Ͻð��� ���� (���� �޼���)
// PARM :	1 . lpszFileName - ������ �̸��� ������ ����/��� ���
//			2 . lpstCreateFileTime - �����ð�
//			3 . lpstAccessFileTime - ������ ���ٽð�
//			4 . lpstWriteFileTime - ������ ��Ͻð�
// RETV : NOERROR - ���� / �׿� - ����
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
// DESC : Ư�� ����� ������ ���������� ���� (���� �޼���)
// PARM :	1 . lpszFileName - ������ �̸��� ������ ����/��� ���
//			2 . dwFileVersion1 - �������� 1
//			3 . dwFileVersion2 - �������� 2
//			4 . dwFileVersion3 - �������� 3
//			5 . dwFileVersion4 - �������� 4
// RETV : NOERROR - ���� / �׿� - ���� (���� ���̳ʸ��� ���� ���ҽ��� ������ ERROR_RESOURCE_TYPE_NOT_FOUND (1813L) ��ȯ)
// PRGM : B4nFter
// P.S.>
//		- ���� ���̳ʸ��� ���� ���ҽ��� �̹� �����ؾ� ��
//*---------------------------------------------------------------
DWORD CFileSet::GetVersion(LPCTSTR lpszFileName, DWORD& dwFileVersion1, DWORD& dwFileVersion2, DWORD& dwFileVersion3, DWORD& dwFileVersion4)
{
	DWORD dwHandle= 0;
	DWORD dwFileVersionInfoSize = ::GetFileVersionInfoSize(lpszFileName, &dwHandle);
	if (0 == dwFileVersionInfoSize)
	{
		return (::GetLastError());	// Version Resource�� ������ ERROR_RESOURCE_TYPE_NOT_FOUND (1813L)
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
// DESC : Ư�� ����� ������ �Ӽ������� ������ (���� �޼���)
// PARM :	1 . lpszFileName - ������ �̸��� ������ ����/��� ���
// RETV : ������ �Ӽ� : ���� / INVALID_FILE_ATTRIBUTES : ����
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
// DESC : Ư�� ����� ������ �Ӽ��� ���� (���� �޼���)
// PARM :	1 . lpszFileName - ������ �̸��� ������ ����/��� ���
//			2 . dwFileAttributes - ���� �Ӽ�
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
// RETV : TRUE - ���� / FALSE - ����
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
// DESC : Ư�� ����� ������ ���� (���� �޼���)
// PARM :	1 . lpszSrcFileName - ���� ����
//			2 . lpszTgtFileName - ���� ����
//			3 . pIsFailIfExist - ������ ������ ������ ���з� ó������ ���� (TRUE:���� / FALSE:����(���))
// RETV : TRUE - ���� / FALSE - ����
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
// DESC : Ư�� ����� ������ ���� (���� �޼���)
// PARM :	1 . lpszFileName - ������ �̸��� ������ ����/��� ���
// RETV : TRUE - ���� / FALSE - ����
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
// DESC : Ư�� ����� ������ �̸��� ��θ� ���� (���� �޼���)
// PARM :	1 . lpszOldFileName - ���� ������ �̸��� ������ ����/��� ���
//			2 . lpszNewFileName - �ű� ������ �̸��� ������ ����/��� ���
// RETV : TRUE - ���� / FALSE - ����
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
// DESC : Ư�� ��ο� ���λ�, ���ڸ� �������� �ӽ����� �̸��� ���� (���� �޼���)
// PARM :	1 . lpPathName - ���� ���
//			2 . lpPrefixString - ���� ���� ���λ�
//			3 . uUnique - ���� ����
//			4 . lpTempFileName - �ӽ� �����̸��� ���� ����
// RETV : TRUE - ���� / FALSE - ����
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
// DESC : Ư�� BOM Ÿ�Կ� ���� ������ ����
// PARM :	1 . pBomType - Ư�� BOM Ÿ�Կ� ���� EF_BOMTYPE ������ ��
// RETV : Ư�� BOM Ÿ�Կ� ���� ���� ������
// PRGM : B4nFter
//*---------------------------------------------------------------
CFileSet::LPFILEBOMTYPEDAT CFileSet::GetBomTypeDat(EF_BOMTYPE pBomType)
{
	if (!CHECK_LIMIT(pBomType, EV_BT_NONE)) {
		BASE_RETURN(NULL);
	}

	return &m_stBomTypeDat[pBomType];
}

