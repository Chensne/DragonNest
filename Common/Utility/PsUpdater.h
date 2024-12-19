

#pragma once


#include <TCHAR.H>
#include <WINDOWS.H>
#include <WININET.H>
#include <BASETSD.H>
#define	_CONVERSION_DONT_USE_THREAD_LOCALE	// ATL 7.0 부터 생긴 각 스레드별 코드페이지를 얻어오지 않고 기존 ATL 3.0 처럼 CP_THREAD_ACP 매크로 사용
#include <ATLCONV.H>		// A2T()
#include "HTTPCLIENT.H"
#include "CONSOLEOUTPUT.H"
#include "./BOOST/SHARED_PTR.HPP"
#include "Log.h"		// 테스트용 공용로그 임시사용

#pragma comment(lib, "MPR.LIB")		// WNetAddConnection2, WNetCancelConnection2


#define DF_PSUD_TMPBUFFSIZE			(1024)
#define DF_PSUD_VERSIONSIZE			(64)
#define DF_PSUD_ACCIDMAXLEN			(64)
#define DF_PSUD_PASSMAXLEN			(64)
#define DF_PSUD_DELFILEPREP			_T("Z_PSDEL")
#define DF_PSUD_URLINIFILE			_T("psurl.ini")
#define DF_PSUD_URLINIKEY			_T("update_info")
#define DF_PSUD_URLINIVAL_VERFILE	_T("ver_file")
#define DF_PSUD_URLINIVAL_VERURL	_T("ver_url")
#define DF_PSUD_URLINIVAL_DOWNURL	_T("down_url")
#define DF_PSUD_URLINIVAL_ACCID		_T("down_accid")
#define DF_PSUD_URLINIVAL_PASS		_T("down_pass")



class CPsUpdater
{

public:
	typedef	std::vector<_tstring>				TP_COPYFAILLIST;
	typedef	TP_COPYFAILLIST::iterator			TP_COPYFAILLIST_ITR;
	typedef	TP_COPYFAILLIST::const_iterator		TP_COPYFAILLIST_CTR;
	typedef boost::shared_ptr<CConsoleOutput>	TP_CONSOLEOUTPUT;

public:
	enum eNotifyType
	{
		eNotifyTypeUpdate,		// 업데이트
		eNotifyTypeCheck,		// 버전체크
		eNotifyTypeMax,
	};

public:
	CPsUpdater();
	~CPsUpdater();


	bool Open();
	void Close();
	bool IsOpen() const { return m_IsOpen; }
	
	bool IsSet() const { return(0 != m_CurPsVersionNo); }

	bool LoadVersion();
	bool DoCheck();
	void DoUpdate(eNotifyType pNotifyType);
	bool ParseVersion(LPCSTR pPsVersion, WORD& pYear, BYTE& pMonth, BYTE& pDay, WORD& pBuildNo);

	LPCTSTR GetVersion() const { return m_CurPsVersion; }

	DWORD DeleteOldFile(LPCTSTR pLocalPath = NULL);

	void AsyncUpdate();
	void UpdateProcess(LPBYTE pVersionBuff);

private:
	DWORD CopyNewFile(TP_COPYFAILLIST& pCopyFailList, TP_CONSOLEOUTPUT& pConsoleOutput, LPCTSTR pRemotePath, LPCTSTR pLocalPath = NULL);

	void ExecInternetFunc();

private:
	TCHAR m_PsVersionFileName[MAX_PATH];
	TCHAR m_PsVersionFileUrl[MAX_PATH];
	TCHAR m_PsDownloadRootUrl[MAX_PATH];
	TCHAR m_PsDownloadPathUrl[MAX_PATH];
	TCHAR m_PsDownloadAccountId[DF_PSUD_ACCIDMAXLEN];
	TCHAR m_PsDownloadPassword[DF_PSUD_PASSMAXLEN];
	TCHAR m_CurPsVersion[DF_PSUD_VERSIONSIZE];
	TCHAR m_NewPsVersion[DF_PSUD_VERSIONSIZE];
	UINT64 m_CurPsVersionNo;
	UINT64 m_NewPsVersionNo;

	bool m_IsOpen;

	HINTERNET m_hInternet;			
	HINTERNET m_hInternetConnection;
	bool m_IsUpdate;
};


extern CPsUpdater* g_PsUpdater;

