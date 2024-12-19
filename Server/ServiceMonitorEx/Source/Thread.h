/*//===============================================================

	<< THREAD >>

	PRGM : B4nFter

	FILE : THREAD.H, THREAD.CPP
	DESC : Thread ���� Ŭ����
	INIT BUILT DATE : 2006. 10. 25
	LAST BUILT DATE : 2007. 04. 29

	P.S.> 
		- 
		- 

	Copyright�� 2002~2009 B4nFter (b4nfter@gmail.com) and Co-Writers. All Rights Reserved.

*///===============================================================


#pragma once


///////////////////////////////////////////////////////////////////
//	INCLUDE
///////////////////////////////////////////////////////////////////

#include "SYSBASEBASE.H"
#include <PROCESS.H>
#include <SET>


///////////////////////////////////////////////////////////////////
//	DEFINE
///////////////////////////////////////////////////////////////////

typedef	UINT (__stdcall *LPFN_THREAD_PROC)(LPVOID);


///////////////////////////////////////////////////////////////////
//	DECLARATION
///////////////////////////////////////////////////////////////////

//*---------------------------------------------------------------
// TYPE : CLASS
// NAME : CThread
//*---------------------------------------------------------------
// DESC : Thread ���� Ŭ����
// PRGM : B4nFter
//*---------------------------------------------------------------
class CThread
{

private:
	typedef struct THREADNAMEINFO
	{
		DWORD  dwType;		// must be 0x1000
		LPCSTR szName;		// pointer to name (in user addr space)
		DWORD  dwThreadID;	// thread ID (-1=caller thread)
		DWORD  dwFlags;		// reserved for future use, must be zero
	} *LPTHREADNAMEINFO;

public:
	typedef VOID (*LPFN_PREP_PROC)(LPVOID);
	typedef VOID (*LPFN_POST_PROC)(LPVOID);

public:
	CThread();
	virtual ~CThread();

	BOOL Start(
		LPFN_THREAD_PROC lpfnThreadProc, 
		LPVOID lpParam = NULL, 
		LPCTSTR lpszName = NULL, 
		UINT uStackSize = 0, 
		BOOL bSuspend = FALSE, 
		BOOL bAutoEnd = TRUE,
		LPFN_PREP_PROC lpfnPrepProc = NULL,
		LPFN_POST_PROC lpfnPostProc = NULL,
		LPVOID lpPrepProcParam = NULL,
		LPVOID lpPostProcParam = NULL
		);
	VOID End();
	VOID Reset();

	HANDLE GetHandle() const { return m_hHandle; }
	UINT GetId() const { return m_uId; }

	VOID SetName(LPCTSTR lpszName);
	LPCTSTR GetName() const;

	LPVOID GetParam() const { return m_lpParam; }

	BOOL IsAutoEnd() const { return m_bAutoEnd; }
	LPFN_PREP_PROC GetPrepProc() const { return m_lpfnPrepProc; }
	LPFN_POST_PROC GetPostProc() const { return m_lpfnPostProc; }
	LPVOID GetPrepProcParam() const { return m_lpPrepProcParam; }
	LPVOID GetPostProcParam() const { return m_lpPostProcParam; }

	DWORD Suspend();
	DWORD Resume();
	BOOL Terminate(DWORD dwExitCode = 0);

	BOOL SetPriority(INT iPriority);
	BOOL SetAffinity(DWORD dwMask);

	DWORD GetTlsIndex();
	BOOL SetTlsValue(DWORD dwTlsIndex, LPVOID lpTlsValue);
	VOID FreeTlsIndex(DWORD dwTlsIndex);

public:
	static UINT __stdcall ThreadProc(LPVOID lpParam);
	static VOID SetName(LPCTSTR lpszName, DWORD pThreadID);

private:
	HANDLE m_hHandle;
	UINT m_uId;
	LPFN_THREAD_PROC m_lpfnThreadProc;
	LPVOID m_lpParam;
	TCHAR m_szName[MAX_PATH];
#pragma warning(disable:4251)
	std::set<DWORD> m_setTlsValue;
#pragma warning(default:4251)
	BOOL m_bAutoEnd;
	LPFN_PREP_PROC m_lpfnPrepProc;
	LPFN_POST_PROC m_lpfnPostProc;
	LPVOID m_lpPrepProcParam;
	LPVOID m_lpPostProcParam;

};

