

///////////////////////////////////////////////////////////////////
//	INCLUDE
///////////////////////////////////////////////////////////////////

#include "STDAFX.H"
#include "THREAD.H"


///////////////////////////////////////////////////////////////////
//	IMPLEMENTATION
///////////////////////////////////////////////////////////////////

//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CThread::CThread
//*---------------------------------------------------------------
// DESC : 생성자
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
CThread::CThread()
{
	Reset();
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CThread::~CThread
//*---------------------------------------------------------------
// DESC : 소멸자
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
CThread::~CThread()
{
	
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CThread::Start
//*---------------------------------------------------------------
// DESC : 스레드를 실행
// PARM :	1 . lpfnThreadProc - 스레드 프로시저 포인터
//			2 . lpParam - 스레드에 지정할 인자값 (기본값 NULL)
//			3 . lpszName - 스레드 이름 (기본값 NULL)
//			4 . uStackSize - 스레드 스택크기 (기본값 0 → 1MB)
//			5 . bSuspend - 생성 직후 지연여부 (기본값 FALSE)
//			6 . bAutoEnd - 종료 시 자동 자원해제를 할지 여부 (기본값 TRUE, FALSE 일 경우 사용자가 직접 CThread::End() 호출)
//			7 . m_lpPrepProc - 스레드 시작 시 실행되는 루틴 (기본값 NULL)
//			8 . m_lpPostProc - 스레드 종료 시 실행되는 루틴 (기본값 NULL)
//			9 . m_lpPrepProcParam - 스레드 시작 루틴에 전달되는 인자 (기본값 NULL)
//			10 . m_lpPostProcParam - 스레드 종료 루틴에 전달되는 인자 (기본값 NULL)
// RETV : TRUE - 성공 / FALSE - 실패
// PRGM : B4nFter
//*---------------------------------------------------------------
BOOL CThread::Start(
	LPFN_THREAD_PROC lpfnThreadProc, 
	LPVOID lpParam, 
	LPCTSTR lpszName, 
	UINT uStackSize, 
	BOOL bSuspend, 
	BOOL bAutoEnd,
	LPFN_PREP_PROC lpfnPrepProc,
	LPFN_POST_PROC lpfnPostProc,
	LPVOID lpPrepProcParam,
	LPVOID lpPostProcParam
	)
{
	if (m_hHandle) {
		return FALSE;
	}

	if (!lpfnThreadProc) {
		return FALSE;
	}

	m_lpfnThreadProc = lpfnThreadProc;
	m_lpParam = lpParam;
	m_bAutoEnd = bAutoEnd;
	m_lpfnPrepProc = lpfnPrepProc;
	m_lpfnPostProc = lpfnPostProc;
	m_lpPrepProcParam = lpPrepProcParam;
	m_lpPostProcParam = lpPostProcParam;

	if (lpszName) {
		STRNCPY(m_szName, lpszName, COUNT_OF(m_szName));
	}

	m_hHandle = (HANDLE)::_beginthreadex(NULL, uStackSize, ThreadProc, (LPVOID)(this), (bSuspend)?(CREATE_SUSPENDED):(0), &m_uId);
	if (!m_hHandle) {
		return FALSE;
	}

	return TRUE;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CThread::Reset
//*---------------------------------------------------------------
// DESC : 스레드 객체의 정보를 리셋
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
VOID CThread::End()
{
	if (DF_MEMSTATE_DDDD == (DWORD_PTR)m_hHandle) {
		// HEAP 에서 해제된 경우 별도 종료 없어야 함
		return;
	}

	if (m_hHandle) {
		DWORD dwExitCode = 0;
		::GetExitCodeThread(m_hHandle, &dwExitCode);
		if (STILL_ACTIVE == dwExitCode) {
			// 스레드 종료 시에는 TLS를 명시적으로 해제하지 않음 (STL 자원이 미리 해제됨으로 인한 Access Violation 예외를 피하기 위함, 시스템에 해제 맡김?)
			std::set<DWORD>::iterator it = m_setTlsValue.begin();
			for (; it != m_setTlsValue.end() ; ++it) {
				if ((*it) != TLS_OUT_OF_INDEXES) {
					::TlsFree(*it);
				}
			}
			m_setTlsValue.clear();
		}
		::CloseHandle(m_hHandle);
	}

	Reset();
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CThread::Reset
//*---------------------------------------------------------------
// DESC : 스레드 객체의 정보를 리셋
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
VOID CThread::Reset()
{
	m_hHandle = NULL;
	m_uId = 0;
	m_lpfnThreadProc = NULL;
	m_lpParam = NULL;
	m_szName[0] = _T('\0');
	m_bAutoEnd = TRUE;
	m_lpfnPrepProc = NULL;
	m_lpfnPostProc = NULL;
	m_lpPrepProcParam = NULL;
	m_lpPostProcParam = NULL;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CThread::ThreadProc
//*---------------------------------------------------------------
// DESC : 스레드의 메인 실행루틴, CThread:m_lpfnThreadProc 를 실행
// PARM :	1 . lpParam - 스레드에 지정할 인자값
// RETV : 스레드의 종료코드
// PRGM : B4nFter
//*---------------------------------------------------------------
UINT __stdcall CThread::ThreadProc(LPVOID lpParam)
{
	CThread* lpThread =	(CThread*)lpParam;

	if (!lpThread || !lpThread->m_lpfnThreadProc) {
		return 0;
	}

	if ((DF_MEMSTATE_DDDD != ((DWORD_PTR)(lpThread->m_hHandle))) &&
		::_tcsncmp(lpThread->m_szName, _T(""), COUNT_OF(lpThread->m_szName))
		)
	{
		lpThread->SetName(lpThread->m_szName);
	}

	if ((DF_MEMSTATE_DDDD != ((DWORD_PTR)(lpThread->m_hHandle))) &&
		lpThread->GetPrepProc()
		)
	{
		lpThread->GetPrepProc()(lpThread->GetPrepProcParam());
	}

	UINT iRetVal = 0;
	if ((DF_MEMSTATE_DDDD != ((DWORD_PTR)(lpThread->m_hHandle))) &&
		lpThread->m_lpfnThreadProc
		)
	{
		iRetVal = lpThread->m_lpfnThreadProc(lpThread->GetParam());
	}

	if ((DF_MEMSTATE_DDDD != ((DWORD_PTR)(lpThread->m_hHandle))) &&
		lpThread->GetPostProc()
		)
	{
		lpThread->GetPostProc()(lpThread->GetPostProcParam());
	}

	if ((DF_MEMSTATE_DDDD != ((DWORD_PTR)(lpThread->m_hHandle))) &&
		lpThread->IsAutoEnd()
		)
	{
		lpThread->End();
	}

	return iRetVal;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CThread::SetName
//*---------------------------------------------------------------
// DESC : 현재 스레드 이름을 지정
// PARM :	1 . lpszName - 스레드 이름
// RETV : N/A
// PRGM : B4nFter
// P.S.>
//		- 스레드를 생성한 후 지정해야 함 (CThread::Start() 에서 이름 저장, CThread::ThreadProc() 에서 이름 지정)
//		- 스레드 이름 표시는 Debug → Thread 대화창에서 확인 가능
//*---------------------------------------------------------------
VOID CThread::SetName(LPCTSTR lpszName)
{
	SetName(lpszName, (DWORD)(-1));
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CThread::SetName
//*---------------------------------------------------------------
// DESC : 특정 스레드 이름을 지정
// PARM :	1 . lpszName - 스레드 이름
//			2 . pThreadID - 스레드 ID
// RETV : N/A
// PRGM : B4nFter
// P.S.>
//		- 스레드를 생성한 후 지정해야 함 (CThread::Start() 에서 이름 저장, CThread::ThreadProc() 에서 이름 지정)
//		- 스레드 이름 표시는 Debug → Thread 대화창에서 확인 가능
//*---------------------------------------------------------------
VOID CThread::SetName(LPCTSTR lpszName, DWORD pThreadID)
{
	if (!lpszName) {
		BASE_RETURN_NONE;
	}

	USES_CONVERSION;

	THREADNAMEINFO stThreadNameInfo;
	stThreadNameInfo.dwType = 0x1000;
	stThreadNameInfo.szName = T2CA(lpszName);
	stThreadNameInfo.dwThreadID = pThreadID;	
	stThreadNameInfo.dwFlags = 0;
	__try
	{
		::RaiseException(0x406D1388, 0, sizeof(THREADNAMEINFO)/sizeof(DWORD), (const ULONG_PTR*)&stThreadNameInfo);
	}
	__except(EXCEPTION_CONTINUE_EXECUTION)
	{
		NULL;
	}
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CThread::GetName
//*---------------------------------------------------------------
// DESC : 스레드 이름을 얻어옴
// PARM : N/A
// RETV : 스레드 이름
// PRGM : B4nFter
//*---------------------------------------------------------------
LPCTSTR CThread::GetName() const
{
	return m_szName;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CThread::Suspend
//*---------------------------------------------------------------
// DESC : 스레드의 Suspend 상태를 해제
// PARM : N/A
// RETV : 성공:previous suspend count / 실패:0xFFFFFFFF
// PRGM : B4nFter
//*---------------------------------------------------------------
DWORD CThread::Suspend()
{
	if (!m_hHandle) {
		return (DWORD)(0xFFFFFFFF);
	}
	return ::SuspendThread(m_hHandle);
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CThread::Resume
//*---------------------------------------------------------------
// DESC : 스레드의 Suspend 상태를 해제
// PARM : N/A
// RETV : 성공:previous suspend count / 실패:0xFFFFFFFF
// PRGM : B4nFter
//*---------------------------------------------------------------
DWORD CThread::Resume()
{
	if (!m_hHandle) {
		return (DWORD)(0xFFFFFFFF);
	}
	return ::ResumeThread(m_hHandle);
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CThread::Terminate
//*---------------------------------------------------------------
// DESC : 스레드를 강제 종료함
// PARM :	1 . dwExitCode - Thread 종료코드
// RETV : TRUE - 성공 / FALSE - 실패
// PRGM : B4nFter
//*---------------------------------------------------------------
BOOL CThread::Terminate(DWORD dwExitCode)
{
	if (!m_hHandle) {
		return FALSE;
	}
	return ::TerminateThread(m_hHandle, dwExitCode);
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CThread::SetPriority
//*---------------------------------------------------------------
// DESC : 스레드 우선순위를 지정
// PARM :	1 . iPriority - 우선순위 (1~31까지, 번호가 높을 수록 우선순위 높음)
// RETV : TRUE - 성공 / FALSE - 실패
// PRGM : B4nFter
// P.S.>
//		- MSDN 의 Scheduling Priorities 참고
//*---------------------------------------------------------------
BOOL CThread::SetPriority(INT iPriority)
{
	if (!m_hHandle) {
		return FALSE;
	}
	return ::SetThreadPriority(m_hHandle, iPriority);
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CThread::SetAffinity
//*---------------------------------------------------------------
// DESC : 스레드 친화도를 지정
// PARM :	1 . dwMask - 친화도 마스크 (친화도를 설정하고자 하는 CPU순서에 해당하는 비트들을 1로 셋)
// RETV : TRUE - 성공 / FALSE - 실패
// PRGM : B4nFter
//*---------------------------------------------------------------
BOOL CThread::SetAffinity(DWORD dwMask)
{
	if (!m_hHandle) {
		return FALSE;
	}
	return (BOOL)::SetThreadAffinityMask(m_hHandle, dwMask);
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CThread::GetTlsIndex
//*---------------------------------------------------------------
// DESC : Thread Local Storage 를 할당하고 할당된 공간의 인덱스를 가져옴
// PARM : N/A
// RETV : 할당된 TLS의 인덱스
// PRGM : B4nFter
//*---------------------------------------------------------------
DWORD CThread::GetTlsIndex()
{
	DWORD dwTlsIndex = ::TlsAlloc();
	if (dwTlsIndex != TLS_OUT_OF_INDEXES) {
		m_setTlsValue.insert(dwTlsIndex);
	}
	return dwTlsIndex;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CThread::SetTlsValue
//*---------------------------------------------------------------
// DESC : Thread Local Storage 의 특정 인덱스에 값을 지정
// PARM :	1 . dwTlsIndex - TLS 인덱스
//			2 . lpTlsValue - TLS 인덱스 공간에 지정할 값
// RETV : TRUE - 성공 / FALSE - 실패
// PRGM : B4nFter
//*---------------------------------------------------------------
BOOL CThread::SetTlsValue(DWORD dwTlsIndex, LPVOID lpTlsValue)
{
	std::set<DWORD>::iterator it = m_setTlsValue.find(dwTlsIndex);
	if (it != m_setTlsValue.end()) {
		return ::TlsSetValue(dwTlsIndex, lpTlsValue);
	}
	return FALSE;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CThread::FreeTlsIndex
//*---------------------------------------------------------------
// DESC : Thread Local Storage 의 특정 인덱스를 해제
// PARM :	1 . dwTlsIndex - TLS 인덱스
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
VOID CThread::FreeTlsIndex(DWORD dwTlsIndex)
{
	if (dwTlsIndex == TLS_OUT_OF_INDEXES) {
		return;
	}

	std::set<DWORD>::iterator it = m_setTlsValue.find(dwTlsIndex);
	if (it != m_setTlsValue.end()) {
		m_setTlsValue.erase(dwTlsIndex);
	}
}

