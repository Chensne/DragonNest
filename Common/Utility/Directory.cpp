

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
	_T('\0'),	// 종료 문자열
};


///////////////////////////////////////////////////////////////////
//	IMPLEMENTATION
///////////////////////////////////////////////////////////////////

//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CDirectory::CDirectory
//*---------------------------------------------------------------
// DESC : 생성자
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
// DESC : 소멸자
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
CDirectory::~CDirectory()
{
	// 객체 소멸 시 까지 자원해제가 않될 경우를 대비
	End();
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CDirectory::Reset
//*---------------------------------------------------------------
// DESC : 객체 자원 리셋
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
// P.S.>
//		- 핸들을 닫는것이 아니므로 주의
//*---------------------------------------------------------------
VOID CDirectory::Reset()
{
	m_hFind = INVALID_HANDLE_VALUE;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CDirectory::Begin
//*---------------------------------------------------------------
// DESC : 특정 경로의 특정 파일들 검색 개시
// PARM :	1 . lpszDirectoryFileName - 검색할 파일경로
// RETV : NOERROR - 성공 / 그외 - 실패
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
// DESC : CDirectory::Begin 호출 이후 계속 검색 수행
// PARM : N/A
// RETV : NOERROR - 성공 / 그외 - 실패
// PRGM : B4nFter
// P.S.>
//		- CDirectory::Begin 이 우선 호출되어야 함
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
// DESC : 파일 검색을 종료
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
// P.S.>
//		- CDirectory::Begin / CDirectory::Next 가 실패하더라도 호출 되어야 함
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
// DESC : 원본 디렉토리를 목적 디렉토리로 복사
// PARM :	1 . lpszSourceDirectoryName - 원본 디렉토리
//			2 . lpszTargetDirectoryName - 목적 디렉토리
//			4 . bOverWrite - 덮어쓰기 여부 (기본값 FALSE)
//			5 . bSubDirectory - 하부 디렉토리 탐색여부 (기본값 FALSE)
// RETV : NOERROR - 성공 / 그외 - 실패
// PRGM : B4nFter
//*---------------------------------------------------------------
DWORD CDirectory::Copy(LPCTSTR lpszSourceDirectoryName, LPCTSTR lpszTargetDirectoryName, BOOL bOverWrite, BOOL bSubDirectory)
{
	DN_ASSERT(NULL != lpszSourceDirectoryName,	"Invalid!");
	DN_ASSERT(NULL != lpszTargetDirectoryName,	"Invalid!");

	// 매개변수 체크
	if (!lpszSourceDirectoryName || !_tcscmp(lpszSourceDirectoryName, _T(""))) {
		DN_RETURN(1);
	}
	if (!lpszTargetDirectoryName || !_tcscmp(lpszTargetDirectoryName, _T(""))) {
		DN_RETURN(1);
	}

	// 디렉토리 유효성 체크
	INT iRetVal;
	iRetVal = _taccess(lpszSourceDirectoryName, 0);
	if (iRetVal) {
		// 해당 원본 디렉토리는 존재하지 않음
		DN_RETURN(static_cast<DWORD>(iRetVal));
	}
	iRetVal = _taccess(lpszTargetDirectoryName, 0);
	if (iRetVal) {
		// 해당 원본 디렉토리는 존재하지 않음
		DN_RETURN(static_cast<DWORD>(iRetVal));
	}

	TCHAR szSourceDirectoryNameOut[DF_DIR_MAX_PATH] = { _T('\0'), };
	TCHAR szTargetDirectoryNameOut[DF_DIR_MAX_PATH] = { _T('\0'), };
	DWORD dwRetVal = CheckFixDirectoryName(lpszSourceDirectoryName, lpszTargetDirectoryName, szSourceDirectoryNameOut, COUNT_OF(szSourceDirectoryNameOut), szTargetDirectoryNameOut, COUNT_OF(szTargetDirectoryNameOut));
	if (NOERROR != dwRetVal) {
		DN_RETURN(dwRetVal);
	}

	// 복사작업 진행 (작업 후 원본 유지)
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
// DESC : 원본 디렉토리를 목적 디렉토리로 이동
// PARM :	1 . lpszSourceDirectoryName - 원본 디렉토리
//			2 . lpszTargetDirectoryName - 목적 디렉토리
//			4 . bOverWrite - 덮어쓰기 여부 (기본값 FALSE)
//			5 . bSubDirectory - 하부 디렉토리 탐색여부 (기본값 FALSE)
// RETV : NOERROR - 성공 / 그외 - 실패
// PRGM : B4nFter
//*---------------------------------------------------------------
DWORD CDirectory::Move(LPCTSTR lpszSourceDirectoryName, LPCTSTR lpszTargetDirectoryName, BOOL bOverWrite, BOOL bSubDirectory)
{
	DN_ASSERT(NULL != lpszSourceDirectoryName,	"Invalid!");
	DN_ASSERT(NULL != lpszTargetDirectoryName,	"Invalid!");

	// 매개변수 체크
	if (!lpszSourceDirectoryName || !_tcscmp(lpszSourceDirectoryName, _T(""))) {
		DN_RETURN(1);
	}
	if (!lpszTargetDirectoryName || !_tcscmp(lpszTargetDirectoryName, _T(""))) {
		DN_RETURN(1);
	}

	// 디렉토리 유효성 체크
	INT iRetVal;
	iRetVal = ::_taccess(lpszSourceDirectoryName, 0);
	if (iRetVal) {
		// 해당 원본 디렉토리는 존재하지 않음
		DN_RETURN(static_cast<DWORD>(iRetVal));
	}
	iRetVal = ::_taccess(lpszTargetDirectoryName, 0);
	if (iRetVal) {
		// 해당 원본 디렉토리는 존재하지 않음
		DN_RETURN(static_cast<DWORD>(iRetVal));
	}

	TCHAR szSourceDirectoryNameOut[DF_DIR_MAX_PATH] = { _T('\0'), };
	TCHAR szTargetDirectoryNameOut[DF_DIR_MAX_PATH] = { _T('\0'), };
	DWORD dwRetVal = CheckFixDirectoryName(lpszSourceDirectoryName, lpszTargetDirectoryName, szSourceDirectoryNameOut, COUNT_OF(szSourceDirectoryNameOut), szTargetDirectoryNameOut, COUNT_OF(szTargetDirectoryNameOut));
	if (NOERROR != dwRetVal) {
		DN_RETURN(dwRetVal);
	}

	// 복사작업 진행 (작업 후 원본 삭제)
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

	// 삭제처리 → 디렉토리 내의 파일은 이미 CDirectory::CopyWork() 안에서 삭제되어 있을 것임
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
// DESC : 원본 디렉토리를 목적 디렉토리로 이름 변경
// PARM :	1 . lpszSourceDirectoryName - 원본 디렉토리
//			2 . lpszTargetDirectoryName - 목적 디렉토리
// RETV : NOERROR - 성공 / 그외 - 실패
// PRGM : B4nFter
//*---------------------------------------------------------------
DWORD CDirectory::Rename(LPCTSTR lpszSourceDirectoryName, LPCTSTR lpszTargetDirectoryName)
{
	DN_ASSERT(NULL != lpszSourceDirectoryName,	"Invalid!");
	DN_ASSERT(NULL != lpszTargetDirectoryName,	"Invalid!");
//	CHECK_STRING(lpszSourceDirectoryName,			DF_DIR_MAX_PATH);
//	CHECK_STRING(lpszTargetDirectoryName,			DF_DIR_MAX_PATH);

	// 매개변수 체크
	if (!lpszSourceDirectoryName || !_tcscmp(lpszSourceDirectoryName, _T(""))) {
		DN_RETURN(1);
	}
	// 매개변수 체크
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
// DESC : 원본 디렉토리를 제거
// PARM :	1 . lpszSourceDirectoryName - 원본 디렉토리
//			2 . bCheckFileExist - 파일이 존재하면 디렉토리를 제거하지 않을지 여부 (기본값 TRUE)
// RETV : NOERROR - 성공 / 그외 - 실패
// PRGM : B4nFter
//*---------------------------------------------------------------
DWORD CDirectory::Remove(LPCTSTR lpszSourceDirectoryName, BOOL bCheckFileExist)
{
	DN_ASSERT(NULL != lpszSourceDirectoryName,	"Invalid!");
//	CHECK_STRING(lpszSourceDirectoryName,			DF_DIR_MAX_PATH);

	// 매개변수 체크
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
		if (iSourceDirectoryNameLen < COUNT_OF(szSourceDirectoryName)-2) {		// NULL 문자 포함
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

	// 삭제처리 → 디렉토리 내의 파일은 이미 CDirectory::CopyWork() 안에서 삭제되어 있을 것임
	if (FALSE == ::RemoveDirectory(lpszSourceDirectoryName)) {
		DN_RETURN(::GetLastError());
	}

	return(NOERROR);
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CDirectory::CheckFixDirectoryName
//*---------------------------------------------------------------
// DESC : Copy / Move 를 수행할 수 있도록 원본 / 목적 디렉토리의 이름을 조정
// PARM :	1 . lpszSourceDirectoryNameIn - 원본 디렉토리 (입력)
//			2 . lpszTargetDirectoryNameIn - 목적 디렉토리 (입력)
//			3 . lpszSourceDirectoryNameOut - 원본 디렉토리 (출력)
//			4 . iSourceDirectoryNameOutSize - 원본 디렉토리 크기 (출력)
//			5 . lpszTargetDirectoryNameOut - 목적 디렉토리 (출력)
//			6 . iTargetDirectoryNameOutSize - 목적 디렉토리 크기 (출력)
// RETV : NOERROR - 성공 / 그외 - 실패
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

	// 매개변수 체크
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
		if (iSourceDirectoryNameLen < iSourceDirectoryNameOutSize-2) {	// NULL 문자 포함
			lpszSourceDirectoryNameOut[iSourceDirectoryNameLen]		= _T('\\');
			lpszSourceDirectoryNameOut[iSourceDirectoryNameLen+1]	= _T('\0');
			++iSourceDirectoryNameLen;
		}
		else {
			DN_RETURN(1);
		}
	}
	if (_T('\\') != lpszTargetDirectoryNameOut[iTargetDirectoryNameLen-1]) {
		if (iTargetDirectoryNameLen < iTargetDirectoryNameOutSize-2) {	// NULL 문자 포함
			lpszTargetDirectoryNameOut[iTargetDirectoryNameLen]		= _T('\\');
			lpszTargetDirectoryNameOut[iTargetDirectoryNameLen+1]	= _T('\0');
			++iTargetDirectoryNameLen;
		}
		else {
			DN_RETURN(1);
		}
	}
	
	CStringSet aSourceDirectoryName;	// 원본 디렉토리의 이름을 파싱하기 위한 문자열
	if (2 <= iSourceDirectoryNameLen) {
		if (_T(':') == lpszSourceDirectoryNameOut[1]) {
			// 드라이브 문자열을 나타내는 표시가 있다면 스킵
			aSourceDirectoryName = &lpszSourceDirectoryNameOut[2];
		}
		else {
			aSourceDirectoryName = lpszSourceDirectoryNameOut;
		}
	}
	else {
		aSourceDirectoryName = lpszSourceDirectoryNameOut;
	}

	CStringSet aSourceDirectoryToken1, aSourceDirectoryToken2;	// 원본 디렉토리의 이름을 담을 문자열
	BOOL bDoLoop = TRUE;
	INT nStrLength = aSourceDirectoryName.GetLength();
	INT nStrIndex = 0;

	do {
		// 원본 디렉토리 이름에서 토큰 추출
		aSourceDirectoryToken1 = aSourceDirectoryName.Tokenize(m_szSourceDirectorySeperator, nStrIndex);
		if (!aSourceDirectoryToken1.IsEmpty()) {
			aSourceDirectoryToken2 = aSourceDirectoryToken1;
		}
		if (nStrLength <= nStrIndex) {
			// 정상적인 파싱 종료
			break;
		}
	}
	while(bDoLoop);

	// 이름이 존재한다면 목적 디렉토리 이름 문자열에 추가
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
// DESC : Copy / Move 를 실제로 수행, 자신을 재귀호출
// PARM :	1 . lpszSourceDirectoryName - 원본 디렉토리
//			2 . lpszTargetDirectoryName - 목적 디렉토리
//			3 . bDoErase - 작업 수행 중 원본 파일/디렉토리 제거 여부 (TRUE - Move / FALSE - Copy)
//			5 . bOverWrite - 덮어쓰기 여부 (기본값 FALSE)
//			6 . bSubDirectory - 하부 디렉토리 탐색여부 (기본값 FALSE)
// RETV : NOERROR - 성공 / 그외 - 실패
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
	
	// 호출 시 매겨변수인 디렉토리 문자열 마무리를 포함한 체크가 보장됨을 전제로 함

	DWORD dwRetVal;

	// 목적 디렉토리 생성
	if (FALSE == ::CreateDirectory(lpszTargetDirectoryName, NULL)) {
		dwRetVal = ::GetLastError();
		if (ERROR_ALREADY_EXISTS != dwRetVal) {
			// 지정된 위치에 디렉토리를 생성할 수 없음
			DN_RETURN(dwRetVal);
		}
	}

	// 원본 디렉토리의 복사/이동 할 파일/디렉토리 들 탐색
	TCHAR szSourceDirectoryFindFiles[DF_DIR_MAX_PATH] = { _T('\0'), };
	STRCPY(szSourceDirectoryFindFiles, COUNT_OF(szSourceDirectoryFindFiles), lpszSourceDirectoryName);
	STRNCAT(szSourceDirectoryFindFiles, COUNT_OF(szSourceDirectoryFindFiles), _T("*.*"));
	CDirectory aDirectory;
	dwRetVal = aDirectory.Begin(szSourceDirectoryFindFiles);
	if (NOERROR != dwRetVal) {
		DN_RETURN(dwRetVal);
	}

	while(NOERROR == dwRetVal) {
		// 복사작업 수행
		if (aDirectory.IsDirectory()) {
			// 디렉토리

			if (bSubDirectory &&
				_T('.') != aDirectory.GetFindData().cFileName[0]
				) 
			{
				// 원본 디렉토리와 목적 디렉토리의 문자열을 재조정해 줌
				TCHAR szSourceDirectoryName[DF_DIR_MAX_PATH] = { _T('\0'), };
				TCHAR szTargetDirectoryName[DF_DIR_MAX_PATH] = { _T('\0'), };
				INT iDirectoryNameLen1, iDirectoryNameLen2;
				STRNCPY(szSourceDirectoryName, COUNT_OF(szSourceDirectoryName), lpszSourceDirectoryName);
				STRNCPY(szTargetDirectoryName, COUNT_OF(szTargetDirectoryName), lpszTargetDirectoryName);
				// 원본 디렉토리 문자열 조정
				iDirectoryNameLen1 = (INT)_tcslen(szSourceDirectoryName);
				STRCAT(szSourceDirectoryName, COUNT_OF(szSourceDirectoryName), aDirectory.GetFindData().cFileName);
				STRCAT(szSourceDirectoryName, COUNT_OF(szSourceDirectoryName), _T("\\"));
				iDirectoryNameLen2 = (INT)_tcslen(szSourceDirectoryName);
				DN_ASSERT(iDirectoryNameLen2 == iDirectoryNameLen1 + (INT)_tcslen(aDirectory.GetFindData().cFileName) + 1, "String Length Not Mach!");
				if (iDirectoryNameLen2 != iDirectoryNameLen1 + (INT)_tcslen(aDirectory.GetFindData().cFileName) + 1) {
					DN_RETURN(1);
				}
				// 목적 디렉토리 문자열 조정
				iDirectoryNameLen1 = (INT)_tcslen(szTargetDirectoryName);
				STRCAT(szTargetDirectoryName, COUNT_OF(szTargetDirectoryName), aDirectory.GetFindData().cFileName);
				STRCAT(szTargetDirectoryName, COUNT_OF(szTargetDirectoryName), _T("\\"));
				iDirectoryNameLen2 = (INT)_tcslen(szTargetDirectoryName);
				DN_ASSERT(iDirectoryNameLen2 == iDirectoryNameLen1 + (INT)_tcslen(aDirectory.GetFindData().cFileName) + 1, "String Length Not Mach!");
				if (iDirectoryNameLen2 != iDirectoryNameLen1 + (INT)_tcslen(aDirectory.GetFindData().cFileName) + 1) {
					DN_RETURN(1);
				}

				// 하위 디렉토리의 복사/이동 작업 진행
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

				// 삭제처리 → 디렉토리 내의 파일은 이미 CDirectory::CopyWork() 안에서 삭제되어 있을 것임
				if (bDoErase &&
					FALSE == ::RemoveDirectory(szSourceDirectoryName)
					)
				{
					DN_RETURN(::GetLastError());
				}
			}
		}
		else {
			// 파일

			// 원본 파일과 목적 파일의 문자열을 생성해 줌
			TCHAR szSourceFileName[DF_DIR_MAX_PATH] = { _T('\0'), };
			TCHAR szTargetFileName[DF_DIR_MAX_PATH] = { _T('\0'), };
			INT iFileNameLen1, iFileNameLen2;
			STRNCPY(szSourceFileName, COUNT_OF(szSourceFileName), lpszSourceDirectoryName);
			STRNCPY(szTargetFileName, COUNT_OF(szTargetFileName), lpszTargetDirectoryName);
			// 원본 파일 문자열 생성
			iFileNameLen1 = (INT)_tcslen(szSourceFileName);
			STRCAT(szSourceFileName, COUNT_OF(szSourceFileName), aDirectory.GetFindData().cFileName);
			iFileNameLen2 = (INT)_tcslen(szSourceFileName);
			DN_ASSERT(iFileNameLen2 == iFileNameLen1 + (INT)_tcslen(aDirectory.GetFindData().cFileName), "String Length Not Mach!");
			if (iFileNameLen2 != iFileNameLen1 + (INT)_tcslen(aDirectory.GetFindData().cFileName)) {
				DN_RETURN(1);
			}
			// 목표 파일 문자열 생성
			iFileNameLen1 = (INT)_tcslen(szTargetFileName);
			STRCAT(szTargetFileName, COUNT_OF(szTargetFileName), aDirectory.GetFindData().cFileName);
			iFileNameLen2 = (INT)_tcslen(szTargetFileName);
			DN_ASSERT(iFileNameLen2 == iFileNameLen1 + (INT)_tcslen(aDirectory.GetFindData().cFileName), "String Length Not Mach!");
			if (iFileNameLen2 != iFileNameLen1 + (INT)_tcslen(aDirectory.GetFindData().cFileName)) {
				DN_RETURN(1);
			}

			// 파일 복사
			DWORD dwCopyFlag = (bOverWrite)?(0):(COPY_FILE_FAIL_IF_EXISTS);
			if (FALSE == ::CopyFileEx(szSourceFileName, szTargetFileName, NULL, NULL, FALSE, dwCopyFlag)) {
				dwRetVal = ::GetLastError();
				if (ERROR_FILE_EXISTS != dwRetVal &&
					ERROR_ALREADY_EXISTS != dwRetVal
					)
				{
					// 지정된 위치에 디렉토리를 생성할 수 없음
					DN_RETURN(dwRetVal);
				}
			}
		
			// 삭제처리
			if (bDoErase &&
				FALSE == ::DeleteFile(szSourceFileName)
				)
			{
				DN_RETURN(::GetLastError());
			}
		}

		dwRetVal = aDirectory.Next();
		if (NOERROR != dwRetVal && ERROR_NO_MORE_FILES != dwRetVal) {
			// 에러 발생
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
// DESC : Remove 를 실제로 수행, 자신을 재귀호출
// PARM :	1 . lpszSourceDirectoryName - 원본 디렉토리
//			2 . bCheckFileExist - 파일이 존재하면 디렉토리를 제거하지 않을지 여부 (기본값 TRUE)
// RETV : NOERROR - 성공 / 그외 - 실패
// PRGM : B4nFter
//*---------------------------------------------------------------
DWORD CDirectory::RemoveWork(LPCTSTR lpszSourceDirectoryName, BOOL bCheckFileExist)
{
	DN_ASSERT(NULL != lpszSourceDirectoryName,	"Invalid!");
//	CHECK_STRING(lpszSourceDirectoryName,			DF_DIR_MAX_PATH);

	// 호출 시 매겨변수인 디렉토리 문자열 마무리를 포함한 체크가 보장됨을 전제로 함
	
	DWORD dwRetVal;

	// 원본 디렉토리의 복사/이동 할 파일/디렉토리 들 탐색
	TCHAR szSourceDirectoryFindFiles[DF_DIR_MAX_PATH] = { _T('\0'), };
	STRCPY(szSourceDirectoryFindFiles, COUNT_OF(szSourceDirectoryFindFiles), lpszSourceDirectoryName);
	STRNCAT(szSourceDirectoryFindFiles, COUNT_OF(szSourceDirectoryFindFiles), _T("*.*"));
	CDirectory aDirectory;
	dwRetVal = aDirectory.Begin(szSourceDirectoryFindFiles);
	if (NOERROR != dwRetVal) {
		DN_RETURN(dwRetVal);
	}

	while(NOERROR == dwRetVal) {
		// 복사작업 수행
		if (aDirectory.IsDirectory()) {
			// 디렉토리

			if (_T('.') != aDirectory.GetFindData().cFileName[0]) {
				// 원본 디렉토리와 목적 디렉토리의 문자열을 재조정해 줌
				TCHAR szSourceDirectoryName[DF_DIR_MAX_PATH] = { _T('\0'), };
				INT iDirectoryNameLen1, iDirectoryNameLen2;
				STRNCPY(szSourceDirectoryName, COUNT_OF(szSourceDirectoryName), lpszSourceDirectoryName);
				// 원본 디렉토리 문자열 조정
				iDirectoryNameLen1 = (INT)_tcslen(szSourceDirectoryName);
				STRCAT(szSourceDirectoryName, COUNT_OF(szSourceDirectoryName), aDirectory.GetFindData().cFileName);
				STRCAT(szSourceDirectoryName, COUNT_OF(szSourceDirectoryName), _T("\\"));
				iDirectoryNameLen2 = (INT)_tcslen(szSourceDirectoryName);
				DN_ASSERT(iDirectoryNameLen2 == iDirectoryNameLen1 + (INT)_tcslen(aDirectory.GetFindData().cFileName) + 1, "String Length Not Mach!");
				if (iDirectoryNameLen2 != iDirectoryNameLen1 + (INT)_tcslen(aDirectory.GetFindData().cFileName) + 1) {
					DN_RETURN(1);
				}

				// 하위 디렉토리의 복사/이동 작업 진행
				dwRetVal = RemoveWork(szSourceDirectoryName, bCheckFileExist);
				if (NOERROR != dwRetVal) {
					DN_RETURN(dwRetVal);
				}

				// 삭제처리 → 디렉토리 내의 파일은 이미 CDirectory::CopyWork() 안에서 삭제되어 있을 것임
				if (FALSE == ::RemoveDirectory(szSourceDirectoryName)) {
					DN_RETURN(::GetLastError());
				}
			}
		}
		else {
			// 파일

			if (bCheckFileExist) {
				// 이 옵션이 있으면 파일이 존재할 때 디렉토리를 제거하지 않음
				DN_RETURN(1);
			}

			// 원본 파일과 목적 파일의 문자열을 생성해 줌
			TCHAR szSourceFileName[DF_DIR_MAX_PATH] = { _T('\0'), };
			INT iFileNameLen1, iFileNameLen2;
			STRNCPY(szSourceFileName, COUNT_OF(szSourceFileName), lpszSourceDirectoryName);
			// 원본 파일 문자열 생성
			iFileNameLen1 = (INT)_tcslen(szSourceFileName);
			STRCAT(szSourceFileName, COUNT_OF(szSourceFileName), aDirectory.GetFindData().cFileName);
			iFileNameLen2 = (INT)_tcslen(szSourceFileName);
			DN_ASSERT(iFileNameLen2 == iFileNameLen1 + (INT)_tcslen(aDirectory.GetFindData().cFileName), "String Length Not Mach!");
			if (iFileNameLen2 != iFileNameLen1 + (INT)_tcslen(aDirectory.GetFindData().cFileName)) {
				DN_RETURN(1);
			}
			
			// 삭제처리
			if (FALSE == ::DeleteFile(szSourceFileName)) {
				DN_RETURN(::GetLastError());
			}
		}

		dwRetVal = aDirectory.Next();
		if (NOERROR != dwRetVal && ERROR_NO_MORE_FILES != dwRetVal) {
			// 에러 발생
			aDirectory.End();
			DN_RETURN(dwRetVal);
		}
	}

	aDirectory.End();

	return(NOERROR);
}

