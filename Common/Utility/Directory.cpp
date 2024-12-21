

///////////////////////////////////////////////////////////////////
//	INCLUDE
///////////////////////////////////////////////////////////////////

#include "STDAFX.H"
#include "DIRECTORY.H"
#include "COMMONMACROS.H"
#include "BASESET.H"
#include "DEBUGUTIL.H"
#include "SAFESTRINGMACROS.H"
#include "STRINGSET.H"
#include <IO.H>		// _access

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

const TCHAR CDirectory::m_szSourceDirectorySeperator[3] = {
	_T('\\'),
	_T(':'),
	_T('\0'),	// ���� ���ڿ�
};


///////////////////////////////////////////////////////////////////
//	IMPLEMENTATION
///////////////////////////////////////////////////////////////////

//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CDirectory::CDirectory
//*---------------------------------------------------------------
// DESC : ������
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
CDirectory::CDirectory()
{
	Reset();
};


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CDirectory::~CDirectory
//*---------------------------------------------------------------
// DESC : �Ҹ���
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
CDirectory::~CDirectory()
{
	// ��ü �Ҹ� �� ���� �ڿ������� �ʵ� ��츦 ���
	End();
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CDirectory::Reset
//*---------------------------------------------------------------
// DESC : ��ü �ڿ� ����
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
// P.S.>
//		- �ڵ��� �ݴ°��� �ƴϹǷ� ����
//*---------------------------------------------------------------
VOID CDirectory::Reset()
{
	m_hFind = INVALID_HANDLE_VALUE;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CDirectory::Begin
//*---------------------------------------------------------------
// DESC : Ư�� ����� Ư�� ���ϵ� �˻� ����
// PARM :	1 . lpszDirectoryFileName - �˻��� ���ϰ��
// RETV : NOERROR - ���� / �׿� - ����
// PRGM : B4nFter
//*---------------------------------------------------------------
DWORD CDirectory::Begin(LPCTSTR lpszDirectoryFileName)
{
	DN_ASSERT(!IsOpen(),					"Already Called!");
//	CHECK_STRING(lpszDirectoryFileName,		DF_DIR_MAX_PATH);

	if (INVALID_HANDLE_VALUE != m_hFind) {
		return(1);
	}


	m_hFind = ::FindFirstFile(lpszDirectoryFileName, &m_stFindData);
	if (INVALID_HANDLE_VALUE != m_hFind) {
		return(NOERROR);
	}
	else {
		return(::GetLastError());
	}
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CDirectory::Next
//*---------------------------------------------------------------
// DESC : CDirectory::Begin ȣ�� ���� ��� �˻� ����
// PARM : N/A
// RETV : NOERROR - ���� / �׿� - ����
// PRGM : B4nFter
// P.S.>
//		- CDirectory::Begin �� �켱 ȣ��Ǿ�� ��
//*---------------------------------------------------------------
DWORD CDirectory::Next()
{
	DN_ASSERT(IsOpen(),	"Must Call CDirectory::Begin() First!");

	if (::FindNextFile(m_hFind, &m_stFindData)) {
		return(NOERROR);
	}
	else {
		return(::GetLastError());
	}
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CDirectory::End
//*---------------------------------------------------------------
// DESC : ���� �˻��� ����
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
// P.S.>
//		- CDirectory::Begin / CDirectory::Next �� �����ϴ��� ȣ�� �Ǿ�� ��
//*---------------------------------------------------------------
VOID CDirectory::End()
{
	if (INVALID_HANDLE_VALUE != m_hFind) {
		::FindClose(m_hFind);
		m_hFind = INVALID_HANDLE_VALUE;
	}
	Reset();
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CDirectory::Copy
//*---------------------------------------------------------------
// DESC : ���� ���丮�� ���� ���丮�� ����
// PARM :	1 . lpszSourceDirectoryName - ���� ���丮
//			2 . lpszTargetDirectoryName - ���� ���丮
//			4 . bOverWrite - ����� ���� (�⺻�� FALSE)
//			5 . bSubDirectory - �Ϻ� ���丮 Ž������ (�⺻�� FALSE)
// RETV : NOERROR - ���� / �׿� - ����
// PRGM : B4nFter
//*---------------------------------------------------------------
DWORD CDirectory::Copy(LPCTSTR lpszSourceDirectoryName, LPCTSTR lpszTargetDirectoryName, BOOL bOverWrite, BOOL bSubDirectory)
{
	DN_ASSERT(NULL != lpszSourceDirectoryName,	"Invalid!");
	DN_ASSERT(NULL != lpszTargetDirectoryName,	"Invalid!");

	// �Ű����� üũ
	if (!lpszSourceDirectoryName || !_tcscmp(lpszSourceDirectoryName, _T(""))) {
		DN_RETURN(1);
	}
	if (!lpszTargetDirectoryName || !_tcscmp(lpszTargetDirectoryName, _T(""))) {
		DN_RETURN(1);
	}

	// ���丮 ��ȿ�� üũ
	INT iRetVal;
	iRetVal = _taccess(lpszSourceDirectoryName, 0);
	if (iRetVal) {
		// �ش� ���� ���丮�� �������� ����
		DN_RETURN(static_cast<DWORD>(iRetVal));
	}
	iRetVal = _taccess(lpszTargetDirectoryName, 0);
	if (iRetVal) {
		// �ش� ���� ���丮�� �������� ����
		DN_RETURN(static_cast<DWORD>(iRetVal));
	}

	TCHAR szSourceDirectoryNameOut[DF_DIR_MAX_PATH] = { _T('\0'), };
	TCHAR szTargetDirectoryNameOut[DF_DIR_MAX_PATH] = { _T('\0'), };
	DWORD dwRetVal = CheckFixDirectoryName(lpszSourceDirectoryName, lpszTargetDirectoryName, szSourceDirectoryNameOut, COUNT_OF(szSourceDirectoryNameOut), szTargetDirectoryNameOut, COUNT_OF(szTargetDirectoryNameOut));
	if (NOERROR != dwRetVal) {
		DN_RETURN(dwRetVal);
	}

	// �����۾� ���� (�۾� �� ���� ����)
	dwRetVal = CopyWork(
		szSourceDirectoryNameOut, 
		szTargetDirectoryNameOut, 
		FALSE,
		bOverWrite, 
		bSubDirectory
		);
	if (NOERROR != dwRetVal) {
		DN_RETURN(dwRetVal);
	}

	return(NOERROR);
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CDirectory::Move
//*---------------------------------------------------------------
// DESC : ���� ���丮�� ���� ���丮�� �̵�
// PARM :	1 . lpszSourceDirectoryName - ���� ���丮
//			2 . lpszTargetDirectoryName - ���� ���丮
//			4 . bOverWrite - ����� ���� (�⺻�� FALSE)
//			5 . bSubDirectory - �Ϻ� ���丮 Ž������ (�⺻�� FALSE)
// RETV : NOERROR - ���� / �׿� - ����
// PRGM : B4nFter
//*---------------------------------------------------------------
DWORD CDirectory::Move(LPCTSTR lpszSourceDirectoryName, LPCTSTR lpszTargetDirectoryName, BOOL bOverWrite, BOOL bSubDirectory)
{
	DN_ASSERT(NULL != lpszSourceDirectoryName,	"Invalid!");
	DN_ASSERT(NULL != lpszTargetDirectoryName,	"Invalid!");

	// �Ű����� üũ
	if (!lpszSourceDirectoryName || !_tcscmp(lpszSourceDirectoryName, _T(""))) {
		DN_RETURN(1);
	}
	if (!lpszTargetDirectoryName || !_tcscmp(lpszTargetDirectoryName, _T(""))) {
		DN_RETURN(1);
	}

	// ���丮 ��ȿ�� üũ
	INT iRetVal;
	iRetVal = ::_taccess(lpszSourceDirectoryName, 0);
	if (iRetVal) {
		// �ش� ���� ���丮�� �������� ����
		DN_RETURN(static_cast<DWORD>(iRetVal));
	}
	iRetVal = ::_taccess(lpszTargetDirectoryName, 0);
	if (iRetVal) {
		// �ش� ���� ���丮�� �������� ����
		DN_RETURN(static_cast<DWORD>(iRetVal));
	}

	TCHAR szSourceDirectoryNameOut[DF_DIR_MAX_PATH] = { _T('\0'), };
	TCHAR szTargetDirectoryNameOut[DF_DIR_MAX_PATH] = { _T('\0'), };
	DWORD dwRetVal = CheckFixDirectoryName(lpszSourceDirectoryName, lpszTargetDirectoryName, szSourceDirectoryNameOut, COUNT_OF(szSourceDirectoryNameOut), szTargetDirectoryNameOut, COUNT_OF(szTargetDirectoryNameOut));
	if (NOERROR != dwRetVal) {
		DN_RETURN(dwRetVal);
	}

	// �����۾� ���� (�۾� �� ���� ����)
	dwRetVal = CopyWork(
		szSourceDirectoryNameOut, 
		szTargetDirectoryNameOut, 
		TRUE, 
		bOverWrite, 
		bSubDirectory
		);
	if (NOERROR != dwRetVal) {
		DN_RETURN(dwRetVal);
	}

	// ����ó�� �� ���丮 ���� ������ �̹� CDirectory::CopyWork() �ȿ��� �����Ǿ� ���� ����
	if (FALSE == ::RemoveDirectory(lpszSourceDirectoryName) &&
		bSubDirectory
		)
	{
		DN_RETURN(::GetLastError());
	}

	return(NOERROR);
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CDirectory::Rename
//*---------------------------------------------------------------
// DESC : ���� ���丮�� ���� ���丮�� �̸� ����
// PARM :	1 . lpszSourceDirectoryName - ���� ���丮
//			2 . lpszTargetDirectoryName - ���� ���丮
// RETV : NOERROR - ���� / �׿� - ����
// PRGM : B4nFter
//*---------------------------------------------------------------
DWORD CDirectory::Rename(LPCTSTR lpszSourceDirectoryName, LPCTSTR lpszTargetDirectoryName)
{
	DN_ASSERT(NULL != lpszSourceDirectoryName,	"Invalid!");
	DN_ASSERT(NULL != lpszTargetDirectoryName,	"Invalid!");
//	CHECK_STRING(lpszSourceDirectoryName,			DF_DIR_MAX_PATH);
//	CHECK_STRING(lpszTargetDirectoryName,			DF_DIR_MAX_PATH);

	// �Ű����� üũ
	if (!lpszSourceDirectoryName || !_tcscmp(lpszSourceDirectoryName, _T(""))) {
		DN_RETURN(1);
	}
	// �Ű����� üũ
	if (!lpszTargetDirectoryName || !_tcscmp(lpszTargetDirectoryName, _T(""))) {
		DN_RETURN(1);
	}

	if (FALSE == ::MoveFile(lpszSourceDirectoryName, lpszTargetDirectoryName)) {
		return(::GetLastError());
	}
	return(NOERROR);
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CDirectory::Remove
//*---------------------------------------------------------------
// DESC : ���� ���丮�� ����
// PARM :	1 . lpszSourceDirectoryName - ���� ���丮
//			2 . bCheckFileExist - ������ �����ϸ� ���丮�� �������� ������ ���� (�⺻�� TRUE)
// RETV : NOERROR - ���� / �׿� - ����
// PRGM : B4nFter
//*---------------------------------------------------------------
DWORD CDirectory::Remove(LPCTSTR lpszSourceDirectoryName, BOOL bCheckFileExist)
{
	DN_ASSERT(NULL != lpszSourceDirectoryName,	"Invalid!");
//	CHECK_STRING(lpszSourceDirectoryName,			DF_DIR_MAX_PATH);

	// �Ű����� üũ
	if (!lpszSourceDirectoryName || !_tcscmp(lpszSourceDirectoryName, _T(""))) {
		DN_RETURN(1);
	}

	TCHAR szSourceDirectoryName[DF_DIR_MAX_PATH] = { _T('\0'), };
	STRNCPY(szSourceDirectoryName, COUNT_OF(szSourceDirectoryName), lpszSourceDirectoryName);
	INT iSourceDirectoryNameLen = (INT)_tcslen(szSourceDirectoryName);
	DN_ASSERT(0 < iSourceDirectoryNameLen,	"Invalid!");
	DN_ASSERT(COUNT_OF(szSourceDirectoryName)-1 >= iSourceDirectoryNameLen,	"Invalid!");
	if (0 >= iSourceDirectoryNameLen) {
		DN_RETURN(1);
	}

	if (_T('\\') != szSourceDirectoryName[iSourceDirectoryNameLen-1]) {
		if (iSourceDirectoryNameLen < COUNT_OF(szSourceDirectoryName)-2) {		// NULL ���� ����
			szSourceDirectoryName[iSourceDirectoryNameLen]		= _T('\\');
			szSourceDirectoryName[iSourceDirectoryNameLen+1]	= _T('\0');
			++iSourceDirectoryNameLen;
		}
		else {
			DN_RETURN(1);
		}
	}

	DWORD dwRetVal = RemoveWork(szSourceDirectoryName, bCheckFileExist);
	if (NOERROR != dwRetVal) {
		DN_RETURN(dwRetVal);
	}

	// ����ó�� �� ���丮 ���� ������ �̹� CDirectory::CopyWork() �ȿ��� �����Ǿ� ���� ����
	if (FALSE == ::RemoveDirectory(lpszSourceDirectoryName)) {
		DN_RETURN(::GetLastError());
	}

	return(NOERROR);
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CDirectory::CheckFixDirectoryName
//*---------------------------------------------------------------
// DESC : Copy / Move �� ������ �� �ֵ��� ���� / ���� ���丮�� �̸��� ����
// PARM :	1 . lpszSourceDirectoryNameIn - ���� ���丮 (�Է�)
//			2 . lpszTargetDirectoryNameIn - ���� ���丮 (�Է�)
//			3 . lpszSourceDirectoryNameOut - ���� ���丮 (���)
//			4 . iSourceDirectoryNameOutSize - ���� ���丮 ũ�� (���)
//			5 . lpszTargetDirectoryNameOut - ���� ���丮 (���)
//			6 . iTargetDirectoryNameOutSize - ���� ���丮 ũ�� (���)
// RETV : NOERROR - ���� / �׿� - ����
// PRGM : B4nFter
//*---------------------------------------------------------------
DWORD CDirectory::CheckFixDirectoryName(
	LPCTSTR lpszSourceDirectoryNameIn, 
	LPCTSTR lpszTargetDirectoryNameIn, 
	LPTSTR lpszSourceDirectoryNameOut, 
	const INT iSourceDirectoryNameOutSize,
	LPTSTR lpszTargetDirectoryNameOut,
	const INT iTargetDirectoryNameOutSize
	)
{
	DN_ASSERT(NULL != lpszSourceDirectoryNameIn,	"Invalid!");
	DN_ASSERT(NULL != lpszTargetDirectoryNameIn,	"Invalid!");
	DN_ASSERT(NULL != lpszSourceDirectoryNameOut,	"Invalid!");
	DN_ASSERT(0 < iSourceDirectoryNameOutSize,	"Invalid!");
	DN_ASSERT(NULL != lpszTargetDirectoryNameOut,	"Invalid!");
	DN_ASSERT(0 < iTargetDirectoryNameOutSize,	"Invalid!");
//	CHECK_STRING(lpszSourceDirectoryNameIn,			DF_DIR_MAX_PATH);
//	CHECK_STRING(lpszTargetDirectoryNameIn,			DF_DIR_MAX_PATH);

	// �Ű����� üũ
	if (!lpszSourceDirectoryNameIn || !_tcscmp(lpszSourceDirectoryNameIn, _T(""))) {
		DN_RETURN(1);
	}
	if (!lpszTargetDirectoryNameIn || !_tcscmp(lpszTargetDirectoryNameIn, _T(""))) {
		DN_RETURN(1);
	}
	if (!lpszSourceDirectoryNameOut || !lpszTargetDirectoryNameOut) {
		DN_RETURN(1);
	}

	INT iSourceDirectoryNameLen = (INT)_tcslen(lpszSourceDirectoryNameIn);
	DN_ASSERT(0 < iSourceDirectoryNameLen,	"Invalid!");
	DN_ASSERT(iSourceDirectoryNameOutSize-1 >= iSourceDirectoryNameLen,	"Invalid!");
	if (0 >= iSourceDirectoryNameLen) {
		DN_RETURN(1);
	}
	STRNCPY(lpszSourceDirectoryNameOut, iSourceDirectoryNameOutSize, lpszSourceDirectoryNameIn);
	INT iTargetDirectoryNameLen = (INT)_tcslen(lpszTargetDirectoryNameIn);
	DN_ASSERT(0 < iTargetDirectoryNameLen,	"Invalid!");
	DN_ASSERT(iTargetDirectoryNameOutSize-1 >= iTargetDirectoryNameLen,	"Invalid!");
	if (0 >= iTargetDirectoryNameLen) {
		DN_RETURN(1);
	}
	STRNCPY(lpszTargetDirectoryNameOut, iTargetDirectoryNameOutSize, lpszTargetDirectoryNameIn);

	if (_T('\\') != lpszSourceDirectoryNameOut[iSourceDirectoryNameLen-1]) {
		if (iSourceDirectoryNameLen < iSourceDirectoryNameOutSize-2) {	// NULL ���� ����
			lpszSourceDirectoryNameOut[iSourceDirectoryNameLen]		= _T('\\');
			lpszSourceDirectoryNameOut[iSourceDirectoryNameLen+1]	= _T('\0');
			++iSourceDirectoryNameLen;
		}
		else {
			DN_RETURN(1);
		}
	}
	if (_T('\\') != lpszTargetDirectoryNameOut[iTargetDirectoryNameLen-1]) {
		if (iTargetDirectoryNameLen < iTargetDirectoryNameOutSize-2) {	// NULL ���� ����
			lpszTargetDirectoryNameOut[iTargetDirectoryNameLen]		= _T('\\');
			lpszTargetDirectoryNameOut[iTargetDirectoryNameLen+1]	= _T('\0');
			++iTargetDirectoryNameLen;
		}
		else {
			DN_RETURN(1);
		}
	}
	
	CStringSet aSourceDirectoryName;	// ���� ���丮�� �̸��� �Ľ��ϱ� ���� ���ڿ�
	if (2 <= iSourceDirectoryNameLen) {
		if (_T(':') == lpszSourceDirectoryNameOut[1]) {
			// ����̺� ���ڿ��� ��Ÿ���� ǥ�ð� �ִٸ� ��ŵ
			aSourceDirectoryName = &lpszSourceDirectoryNameOut[2];
		}
		else {
			aSourceDirectoryName = lpszSourceDirectoryNameOut;
		}
	}
	else {
		aSourceDirectoryName = lpszSourceDirectoryNameOut;
	}

	CStringSet aSourceDirectoryToken1, aSourceDirectoryToken2;	// ���� ���丮�� �̸��� ���� ���ڿ�
	BOOL bDoLoop = TRUE;
	INT nStrLength = aSourceDirectoryName.GetLength();
	INT nStrIndex = 0;

	do {
		// ���� ���丮 �̸����� ��ū ����
		aSourceDirectoryToken1 = aSourceDirectoryName.Tokenize(m_szSourceDirectorySeperator, nStrIndex);
		if (!aSourceDirectoryToken1.IsEmpty()) {
			aSourceDirectoryToken2 = aSourceDirectoryToken1;
		}
		if (nStrLength <= nStrIndex) {
			// �������� �Ľ� ����
			break;
		}
	}
	while(bDoLoop);

	// �̸��� �����Ѵٸ� ���� ���丮 �̸� ���ڿ��� �߰�
	if (!aSourceDirectoryToken2.IsEmpty()) {
		INT iTargetDirectoryNameLen1 = (INT)_tcslen(lpszTargetDirectoryNameOut);
		STRNCAT(lpszTargetDirectoryNameOut, iTargetDirectoryNameOutSize, aSourceDirectoryToken2.Get());
		STRNCAT(lpszTargetDirectoryNameOut, iTargetDirectoryNameOutSize, _T("\\"));
		INT iTargetDirectoryNameLen2 = (INT)_tcslen(lpszTargetDirectoryNameOut);
		DN_ASSERT(iTargetDirectoryNameLen2 == iTargetDirectoryNameLen1 + aSourceDirectoryToken2.GetLength() + 1,	"String Length Not Mach!");
		if (iTargetDirectoryNameLen2 != iTargetDirectoryNameLen1 + aSourceDirectoryToken2.GetLength() + 1) {
			DN_RETURN(1);
		}
	}

	return(NOERROR);
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CDirectory::CopyWork
//*---------------------------------------------------------------
// DESC : Copy / Move �� ������ ����, �ڽ��� ���ȣ��
// PARM :	1 . lpszSourceDirectoryName - ���� ���丮
//			2 . lpszTargetDirectoryName - ���� ���丮
//			3 . bDoErase - �۾� ���� �� ���� ����/���丮 ���� ���� (TRUE - Move / FALSE - Copy)
//			5 . bOverWrite - ����� ���� (�⺻�� FALSE)
//			6 . bSubDirectory - �Ϻ� ���丮 Ž������ (�⺻�� FALSE)
// RETV : NOERROR - ���� / �׿� - ����
// PRGM : B4nFter
//*---------------------------------------------------------------
DWORD CDirectory::CopyWork(
	LPCTSTR lpszSourceDirectoryName, 
	LPCTSTR lpszTargetDirectoryName, 
	BOOL bDoErase, 
	BOOL bOverWrite, 
	BOOL bSubDirectory
   )
{
	DN_ASSERT(NULL != lpszSourceDirectoryName,	"Invalid!");
	DN_ASSERT(NULL != lpszTargetDirectoryName,	"Invalid!");
//	CHECK_STRING(lpszSourceDirectoryName,			DF_DIR_MAX_PATH);
//	CHECK_STRING(lpszTargetDirectoryName,			DF_DIR_MAX_PATH);
	
	// ȣ�� �� �Űܺ����� ���丮 ���ڿ� �������� ������ üũ�� ������� ������ ��

	DWORD dwRetVal;

	// ���� ���丮 ����
	if (FALSE == ::CreateDirectory(lpszTargetDirectoryName, NULL)) {
		dwRetVal = ::GetLastError();
		if (ERROR_ALREADY_EXISTS != dwRetVal) {
			// ������ ��ġ�� ���丮�� ������ �� ����
			DN_RETURN(dwRetVal);
		}
	}

	// ���� ���丮�� ����/�̵� �� ����/���丮 �� Ž��
	TCHAR szSourceDirectoryFindFiles[DF_DIR_MAX_PATH] = { _T('\0'), };
	STRCPY(szSourceDirectoryFindFiles, COUNT_OF(szSourceDirectoryFindFiles), lpszSourceDirectoryName);
	STRNCAT(szSourceDirectoryFindFiles, COUNT_OF(szSourceDirectoryFindFiles), _T("*.*"));
	CDirectory aDirectory;
	dwRetVal = aDirectory.Begin(szSourceDirectoryFindFiles);
	if (NOERROR != dwRetVal) {
		DN_RETURN(dwRetVal);
	}

	while(NOERROR == dwRetVal) {
		// �����۾� ����
		if (aDirectory.IsDirectory()) {
			// ���丮

			if (bSubDirectory &&
				_T('.') != aDirectory.GetFindData().cFileName[0]
				) 
			{
				// ���� ���丮�� ���� ���丮�� ���ڿ��� �������� ��
				TCHAR szSourceDirectoryName[DF_DIR_MAX_PATH] = { _T('\0'), };
				TCHAR szTargetDirectoryName[DF_DIR_MAX_PATH] = { _T('\0'), };
				INT iDirectoryNameLen1, iDirectoryNameLen2;
				STRNCPY(szSourceDirectoryName, COUNT_OF(szSourceDirectoryName), lpszSourceDirectoryName);
				STRNCPY(szTargetDirectoryName, COUNT_OF(szTargetDirectoryName), lpszTargetDirectoryName);
				// ���� ���丮 ���ڿ� ����
				iDirectoryNameLen1 = (INT)_tcslen(szSourceDirectoryName);
				STRCAT(szSourceDirectoryName, COUNT_OF(szSourceDirectoryName), aDirectory.GetFindData().cFileName);
				STRCAT(szSourceDirectoryName, COUNT_OF(szSourceDirectoryName), _T("\\"));
				iDirectoryNameLen2 = (INT)_tcslen(szSourceDirectoryName);
				DN_ASSERT(iDirectoryNameLen2 == iDirectoryNameLen1 + (INT)_tcslen(aDirectory.GetFindData().cFileName) + 1, "String Length Not Mach!");
				if (iDirectoryNameLen2 != iDirectoryNameLen1 + (INT)_tcslen(aDirectory.GetFindData().cFileName) + 1) {
					DN_RETURN(1);
				}
				// ���� ���丮 ���ڿ� ����
				iDirectoryNameLen1 = (INT)_tcslen(szTargetDirectoryName);
				STRCAT(szTargetDirectoryName, COUNT_OF(szTargetDirectoryName), aDirectory.GetFindData().cFileName);
				STRCAT(szTargetDirectoryName, COUNT_OF(szTargetDirectoryName), _T("\\"));
				iDirectoryNameLen2 = (INT)_tcslen(szTargetDirectoryName);
				DN_ASSERT(iDirectoryNameLen2 == iDirectoryNameLen1 + (INT)_tcslen(aDirectory.GetFindData().cFileName) + 1, "String Length Not Mach!");
				if (iDirectoryNameLen2 != iDirectoryNameLen1 + (INT)_tcslen(aDirectory.GetFindData().cFileName) + 1) {
					DN_RETURN(1);
				}

				// ���� ���丮�� ����/�̵� �۾� ����
				dwRetVal = CopyWork(
					szSourceDirectoryName, 
					szTargetDirectoryName, 
					bDoErase, 
					bOverWrite, 
					bSubDirectory
					);
				if (NOERROR != dwRetVal) {
					DN_RETURN(dwRetVal);
				}

				// ����ó�� �� ���丮 ���� ������ �̹� CDirectory::CopyWork() �ȿ��� �����Ǿ� ���� ����
				if (bDoErase &&
					FALSE == ::RemoveDirectory(szSourceDirectoryName)
					)
				{
					DN_RETURN(::GetLastError());
				}
			}
		}
		else {
			// ����

			// ���� ���ϰ� ���� ������ ���ڿ��� ������ ��
			TCHAR szSourceFileName[DF_DIR_MAX_PATH] = { _T('\0'), };
			TCHAR szTargetFileName[DF_DIR_MAX_PATH] = { _T('\0'), };
			INT iFileNameLen1, iFileNameLen2;
			STRNCPY(szSourceFileName, COUNT_OF(szSourceFileName), lpszSourceDirectoryName);
			STRNCPY(szTargetFileName, COUNT_OF(szTargetFileName), lpszTargetDirectoryName);
			// ���� ���� ���ڿ� ����
			iFileNameLen1 = (INT)_tcslen(szSourceFileName);
			STRCAT(szSourceFileName, COUNT_OF(szSourceFileName), aDirectory.GetFindData().cFileName);
			iFileNameLen2 = (INT)_tcslen(szSourceFileName);
			DN_ASSERT(iFileNameLen2 == iFileNameLen1 + (INT)_tcslen(aDirectory.GetFindData().cFileName), "String Length Not Mach!");
			if (iFileNameLen2 != iFileNameLen1 + (INT)_tcslen(aDirectory.GetFindData().cFileName)) {
				DN_RETURN(1);
			}
			// ��ǥ ���� ���ڿ� ����
			iFileNameLen1 = (INT)_tcslen(szTargetFileName);
			STRCAT(szTargetFileName, COUNT_OF(szTargetFileName), aDirectory.GetFindData().cFileName);
			iFileNameLen2 = (INT)_tcslen(szTargetFileName);
			DN_ASSERT(iFileNameLen2 == iFileNameLen1 + (INT)_tcslen(aDirectory.GetFindData().cFileName), "String Length Not Mach!");
			if (iFileNameLen2 != iFileNameLen1 + (INT)_tcslen(aDirectory.GetFindData().cFileName)) {
				DN_RETURN(1);
			}

			// ���� ����
			DWORD dwCopyFlag = (bOverWrite)?(0):(COPY_FILE_FAIL_IF_EXISTS);
			if (FALSE == ::CopyFileEx(szSourceFileName, szTargetFileName, NULL, NULL, FALSE, dwCopyFlag)) {
				dwRetVal = ::GetLastError();
				if (ERROR_FILE_EXISTS != dwRetVal &&
					ERROR_ALREADY_EXISTS != dwRetVal
					)
				{
					// ������ ��ġ�� ���丮�� ������ �� ����
					DN_RETURN(dwRetVal);
				}
			}
		
			// ����ó��
			if (bDoErase &&
				FALSE == ::DeleteFile(szSourceFileName)
				)
			{
				DN_RETURN(::GetLastError());
			}
		}

		dwRetVal = aDirectory.Next();
		if (NOERROR != dwRetVal && ERROR_NO_MORE_FILES != dwRetVal) {
			// ���� �߻�
			aDirectory.End();
			DN_RETURN(dwRetVal);
		}
	}

	aDirectory.End();

	return(NOERROR);
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CDirectory::RemoveWork
//*---------------------------------------------------------------
// DESC : Remove �� ������ ����, �ڽ��� ���ȣ��
// PARM :	1 . lpszSourceDirectoryName - ���� ���丮
//			2 . bCheckFileExist - ������ �����ϸ� ���丮�� �������� ������ ���� (�⺻�� TRUE)
// RETV : NOERROR - ���� / �׿� - ����
// PRGM : B4nFter
//*---------------------------------------------------------------
DWORD CDirectory::RemoveWork(LPCTSTR lpszSourceDirectoryName, BOOL bCheckFileExist)
{
	DN_ASSERT(NULL != lpszSourceDirectoryName,	"Invalid!");
//	CHECK_STRING(lpszSourceDirectoryName,			DF_DIR_MAX_PATH);

	// ȣ�� �� �Űܺ����� ���丮 ���ڿ� �������� ������ üũ�� ������� ������ ��
	
	DWORD dwRetVal;

	// ���� ���丮�� ����/�̵� �� ����/���丮 �� Ž��
	TCHAR szSourceDirectoryFindFiles[DF_DIR_MAX_PATH] = { _T('\0'), };
	STRCPY(szSourceDirectoryFindFiles, COUNT_OF(szSourceDirectoryFindFiles), lpszSourceDirectoryName);
	STRNCAT(szSourceDirectoryFindFiles, COUNT_OF(szSourceDirectoryFindFiles), _T("*.*"));
	CDirectory aDirectory;
	dwRetVal = aDirectory.Begin(szSourceDirectoryFindFiles);
	if (NOERROR != dwRetVal) {
		DN_RETURN(dwRetVal);
	}

	while(NOERROR == dwRetVal) {
		// �����۾� ����
		if (aDirectory.IsDirectory()) {
			// ���丮

			if (_T('.') != aDirectory.GetFindData().cFileName[0]) {
				// ���� ���丮�� ���� ���丮�� ���ڿ��� �������� ��
				TCHAR szSourceDirectoryName[DF_DIR_MAX_PATH] = { _T('\0'), };
				INT iDirectoryNameLen1, iDirectoryNameLen2;
				STRNCPY(szSourceDirectoryName, COUNT_OF(szSourceDirectoryName), lpszSourceDirectoryName);
				// ���� ���丮 ���ڿ� ����
				iDirectoryNameLen1 = (INT)_tcslen(szSourceDirectoryName);
				STRCAT(szSourceDirectoryName, COUNT_OF(szSourceDirectoryName), aDirectory.GetFindData().cFileName);
				STRCAT(szSourceDirectoryName, COUNT_OF(szSourceDirectoryName), _T("\\"));
				iDirectoryNameLen2 = (INT)_tcslen(szSourceDirectoryName);
				DN_ASSERT(iDirectoryNameLen2 == iDirectoryNameLen1 + (INT)_tcslen(aDirectory.GetFindData().cFileName) + 1, "String Length Not Mach!");
				if (iDirectoryNameLen2 != iDirectoryNameLen1 + (INT)_tcslen(aDirectory.GetFindData().cFileName) + 1) {
					DN_RETURN(1);
				}

				// ���� ���丮�� ����/�̵� �۾� ����
				dwRetVal = RemoveWork(szSourceDirectoryName, bCheckFileExist);
				if (NOERROR != dwRetVal) {
					DN_RETURN(dwRetVal);
				}

				// ����ó�� �� ���丮 ���� ������ �̹� CDirectory::CopyWork() �ȿ��� �����Ǿ� ���� ����
				if (FALSE == ::RemoveDirectory(szSourceDirectoryName)) {
					DN_RETURN(::GetLastError());
				}
			}
		}
		else {
			// ����

			if (bCheckFileExist) {
				// �� �ɼ��� ������ ������ ������ �� ���丮�� �������� ����
				DN_RETURN(1);
			}

			// ���� ���ϰ� ���� ������ ���ڿ��� ������ ��
			TCHAR szSourceFileName[DF_DIR_MAX_PATH] = { _T('\0'), };
			INT iFileNameLen1, iFileNameLen2;
			STRNCPY(szSourceFileName, COUNT_OF(szSourceFileName), lpszSourceDirectoryName);
			// ���� ���� ���ڿ� ����
			iFileNameLen1 = (INT)_tcslen(szSourceFileName);
			STRCAT(szSourceFileName, COUNT_OF(szSourceFileName), aDirectory.GetFindData().cFileName);
			iFileNameLen2 = (INT)_tcslen(szSourceFileName);
			DN_ASSERT(iFileNameLen2 == iFileNameLen1 + (INT)_tcslen(aDirectory.GetFindData().cFileName), "String Length Not Mach!");
			if (iFileNameLen2 != iFileNameLen1 + (INT)_tcslen(aDirectory.GetFindData().cFileName)) {
				DN_RETURN(1);
			}
			
			// ����ó��
			if (FALSE == ::DeleteFile(szSourceFileName)) {
				DN_RETURN(::GetLastError());
			}
		}

		dwRetVal = aDirectory.Next();
		if (NOERROR != dwRetVal && ERROR_NO_MORE_FILES != dwRetVal) {
			// ���� �߻�
			aDirectory.End();
			DN_RETURN(dwRetVal);
		}
	}

	aDirectory.End();

	return(NOERROR);
}

