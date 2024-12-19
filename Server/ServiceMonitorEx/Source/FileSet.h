/*//===============================================================

	<< FILE SET >>

	FILE : FILESET.H, FILESET.CPP
	DESC : 파일 관리 클래스
	INIT BUILT DATE : 2007. 05. 02
	LAST BUILT DATE : 2007. 05. 14

	Copyrightⓒ 2002~2009 B4nFter (b4nfter@gmail.com) and Co-Writers. All Rights Reserved.

*///===============================================================


#pragma once


///////////////////////////////////////////////////////////////////
//	INCLUDE
///////////////////////////////////////////////////////////////////

#include "SYSBASEBASE.H"
#include "VARARG.H"
#include <MEMORY>
#include <LIMITS.H>

#pragma comment(lib, "VERSION.LIB")


///////////////////////////////////////////////////////////////////
//	DEFINE
///////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////
//	DECLARATION
///////////////////////////////////////////////////////////////////

//*---------------------------------------------------------------
// TYPE : CLASS
// NAME : CFileSet
//*---------------------------------------------------------------
// DESC : 파일 관리 클래스
// PRGM : B4nFter
//*---------------------------------------------------------------
class CFileSet
{
public:
	enum EF_BOMTYPE {			// BOM (Byte Order Mark) 타입
		EV_BT_UTF8,				// BOM - UTF-8
		EV_BT_UTF16_LE,			// BOM - UTF-16/UCS-2 (리틀엔디언)
		EV_BT_UTF16_BE,			// BOM - UTF-16/UCS-2 (빅엔디언)
// 		EV_BT_UTF32_LE,			// BOM - UTF-32/UCS-4 (리틀엔디언)		// P.S.> 필요 시 추가
// 		EV_BT_UTF32_BE,			// BOM - UTF-32/UCS-4 (빅엔디언)		// P.S.> 필요 시 추가
		EV_BT_NONE,
	};
	typedef struct FILEBOMTYPEDAT
	{
		EF_BOMTYPE m_eType;
		BYTE m_btData[UCHAR_MAX];
		BYTE m_nSize;
		FILEBOMTYPEDAT(EF_BOMTYPE eType, const CVarArgA<UCHAR_MAX>* pData, BYTE nSize) {
// 			BASE_ASSERT(CHECK_LIMIT(eType, EV_BT_NONE),	"Invalid!");
// 			BASE_ASSERT(NULL != pData,					"Invalid!");
// 			BASE_ASSERT(0 < nSize,						"Invalid!");
			m_eType = eType;
			::memcpy(m_btData, pData->GetMsg(), nSize);
			m_nSize = nSize;
		}
	} *LPFILEBOMTYPEDAT;

public:
	CFileSet();
	virtual ~CFileSet();

	operator HANDLE() const;

public:
	DWORD Open(
		LPCTSTR lpszFileName, 
		BOOL bIsReadOnly = TRUE, 
		BOOL bIsAppend = FALSE, 
		BOOL bIsReadShare = TRUE
		);
	DWORD Open(
		LPCTSTR lpszFileName,	
		DWORD dwDesiredAccess, 
		DWORD dwShareMode, 
		DWORD dwCreationDisposition, 
		DWORD dwFlagsAndAttributes, 
		LPSECURITY_ATTRIBUTES lpSecurityAttributes = NULL,
		HANDLE hTemplateFile = NULL
		);
	virtual VOID Close();
	virtual BOOL Flush();
	BOOL IsOpen() const;

	virtual BOOL GetFullPath(/*out*/ LPTSTR lpszBuffer, DWORD nBufferSize) const;
	virtual BOOL GetDrive(/*out*/ LPTSTR lpszBuffer, DWORD nBufferSize) const;
	virtual BOOL GetPath(/*out*/ LPTSTR lpszBuffer, DWORD nBufferSize) const;
	virtual BOOL GetName(/*out*/ LPTSTR lpszBuffer, DWORD nBufferSize) const;
	virtual BOOL GetExt(/*out*/ LPTSTR lpszBuffer, DWORD nBufferSize) const;
	virtual BOOL GetTitle(/*out*/ LPTSTR lpszBuffer, DWORD nBufferSize) const;

	virtual DWORD GetPosition(/*out*/ ULONGLONG& u64FilePositon) const;
	virtual DWORD GetTime(/*out*/ LPFILETIME lpstCreateFileTime, /*out*/ LPFILETIME lpstAccessFileTime, /*out*/ LPFILETIME lpstWriteFileTime) const;

	virtual DWORD Seek(DWORD dwMoveMethod, LONG lOffsetLow, LONG lOffsetHigh = 0, /*out*/ ULONGLONG* lpu64Offset = NULL);
	virtual DWORD SetLength(ULONGLONG dwFileLength);
	virtual DWORD GetLength(/*out*/ ULONGLONG& dwFileLength) const;

	virtual DWORD Read(/*out*/ LPVOID lpBuffer, /*int,out*/ DWORD& nBufferSize);
	virtual DWORD Write(LPCVOID lpBuffer, /*int,out*/ DWORD& nBufferSize);

	virtual DWORD LockRange(ULONGLONG uOffset, ULONGLONG uNumberOfBytes);
	virtual DWORD UnlockRange(ULONGLONG uOffset, ULONGLONG uNumberOfBytes);

	virtual EF_BOMTYPE SkipBOM();
	virtual DWORD WriteBOM(EF_BOMTYPE eBomType);

public:
	static DWORD GetTime(LPCTSTR lpszFileName, /*out*/ LPFILETIME lpstCreateFileTime, /*out*/ LPFILETIME lpstAccessFileTime, /*out*/ LPFILETIME lpstWriteFileTime);
	static DWORD GetVersion(LPCTSTR lpszFileName, /*out*/ DWORD& dwFileVersion1, /*out*/ DWORD& dwFileVersion2, /*out*/ DWORD& dwFileVersion3, /*out*/ DWORD& dwFileVersion4);
	static DWORD GetAttributes(LPCTSTR lpszFileName);
	static BOOL SetAttributes(LPCTSTR lpszFileName, DWORD dwFileAttributes);

	static BOOL Copy(LPCTSTR lpszSrcFileName, LPCTSTR lpszTgtFileName, BOOL pIsFailIfExist);
	static BOOL Remove(LPCTSTR lpszFileName);
	static BOOL Rename(LPCTSTR lpszOldFileName, LPCTSTR lpszNewFileName);

	static BOOL GetTempName(LPCTSTR lpPathName, LPCTSTR lpPrefixString, UINT uUnique, /*out*/ LPTSTR lpTempFileName);

	static LPFILEBOMTYPEDAT GetBomTypeDat(EF_BOMTYPE pBomType);

private:
	VOID Reset();

private:
	HANDLE m_hFile;
	TCHAR m_szFileFullPath[MAX_PATH];

#pragma warning(disable:4251)
	static FILEBOMTYPEDAT m_stBomTypeDat[EV_BT_NONE];
#pragma warning(default:4251)
};

