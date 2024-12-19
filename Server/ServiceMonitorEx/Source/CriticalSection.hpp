/*//===============================================================

	<< CRITICAL SECTION >>

	PRGM : milkji, B4nFter

	FILE : CRITICALSECTION.HPP
	DESC : 자동해제 기능을 가진 확장된 임계구역
	INIT BUILT DATE : 2005. 06. 02
	LAST BUILT DATE : 2005. 06. 02

	P.S.>	
		- DEBUG 모드 컴파일 : 임계구역 관련 API로 진입/해제 하는것 보다 3배 정도의 성능저하
		- RELEASE 모드 컴파일 : 임계구역 관련 API와 거의 성능차이 없음

	Copyrightⓒ 2002~2009 B4nFter (b4nfter@gmail.com) and Co-Writers. All Rights Reserved.

*///===============================================================


#pragma once


///////////////////////////////////////////////////////////////////
//	INCLUDE
///////////////////////////////////////////////////////////////////

#include "LOCKBASE.HPP"


///////////////////////////////////////////////////////////////////
//	DEFINE
///////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////
//	DECLARATION
///////////////////////////////////////////////////////////////////

//*---------------------------------------------------------------
// TYPE : CLASS
// NAME : CCriticalSection
//*---------------------------------------------------------------
// DESC : Critical Section Class
// PRGM : milkji, B4nFter
//*---------------------------------------------------------------
class CCriticalSection : public CLockBase
{
private:
	DISABLE_COPY_ASSIGN(CCriticalSection)	// 객체간 복사, 대입 금지

public:
	CCriticalSection();
	virtual ~CCriticalSection();

	DWORD Open(DWORD dwSpinCount = 0);
	VOID Close();

	BOOL IsOpen() const;

	VOID Lock(BOOL bDoCheck = FALSE) const;
	BOOL LockTry() const;
	VOID Unlock() const;

#if defined(_DEBUG)
	BOOL IsLock() const;
#endif // #if defined(_DEBUG)

private:
	VOID Reset();

private:
	mutable CRITICAL_SECTION m_crThis;
};


///////////////////////////////////////////////////////////////////
//	IMPLEMENTATION
///////////////////////////////////////////////////////////////////

//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CCriticalSection::CCriticalSection
//*---------------------------------------------------------------
// DESC : 생성자
// PARM :	1 . lpCriticalSection - 생성된 임계구역 객체의 포인터
// RETV : N/A
// PRGM : milkji, B4nFter
//*---------------------------------------------------------------
inline CCriticalSection::CCriticalSection()
{
	Reset();
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CCriticalSection::~CCriticalSection
//*---------------------------------------------------------------
// DESC : 소멸자
// PARM : N/A
// RETV : N/A
// PRGM : milkji, B4nFter
//*---------------------------------------------------------------
inline CCriticalSection::~CCriticalSection()
{
	Close();
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CCriticalSection::Open
//*---------------------------------------------------------------
// DESC : 객체 시작, 자원 초기화
// PARM :	1 . dwSpinCount - 
// RETV : NOERROR - 성공 / 그외 - 실패
// PRGM : milkji, B4nFter
//*---------------------------------------------------------------
#pragma warning(disable:4100)
inline DWORD CCriticalSection::Open(DWORD dwSpinCount)
{
	::memset(&m_crThis, '\0', sizeof(m_crThis));

#if ((WINVER >= 0x0500) && (_WIN32_WINNT >= 0x0403))
	if (dwSpinCount > 0) {
		/*
			P.S.>
				- 스핀카운트의 최상위 비트를 세트하면 ::EnterCriticalSection() 에 필요한 자원을 미리 할당
				- 이것은 상당량의 Non-Paged Pool 을 소비하므로 많은 수의 임계구역 생성 시 꺼야함
				- 이것은 오직 Windows Server 2000 에서만 동작함 (XP 이상에서는 제거됨)
		*/
		// dwSpinCount |= 0x80000000;	

		if (::InitializeCriticalSectionAndSpinCount(&m_crThis, dwSpinCount)) {
			m_bIsOpen = TRUE;
			return (NOERROR);
		}
	}
#endif	// #if ((WINVER >= 0x0500) && (_WIN32_WINNT >= 0x0500))

	DWORD dwRetVal;
	__try {
		::InitializeCriticalSection(&m_crThis);
		m_bIsOpen	= TRUE;
		dwRetVal = NOERROR;
	}
	__except(EXCEPTION_EXECUTE_HANDLER) 
	{
		dwRetVal = STATUS_NO_MEMORY;
	}
	return dwRetVal;
	
}
#pragma warning(default:4100)


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CCriticalSection::Close
//*---------------------------------------------------------------
// DESC : 객체 종료, 자원 해제
// PARM : N/A
// RETV : N/A
// PRGM : milkji, B4nFter
//*---------------------------------------------------------------
inline VOID CCriticalSection::Close()
{
	if (m_bIsOpen) {
		::DeleteCriticalSection(&m_crThis);
		Reset();
	}
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CCriticalSection::IsOpen
//*---------------------------------------------------------------
// DESC : 현재 객체가 준비되어 있는지 체크
// PARM : N/A
// RETV : TRUE - 맞음 / FALSE - 틀림
// PRGM : milkji, B4nFter
//*---------------------------------------------------------------
inline BOOL CCriticalSection::IsOpen() const
{
	return m_bIsOpen;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CCriticalSection::Lock
//*---------------------------------------------------------------
// DESC : 임계구역 진입
// PARM :	1 . bDoCheck - 
// RETV : N/A
// PRGM : milkji, B4nFter
//*---------------------------------------------------------------
#pragma warning(disable:4100)
inline VOID CCriticalSection::Lock(BOOL bDoCheck) const
{
	::EnterCriticalSection(&m_crThis);
}
#pragma warning(default:4100)


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CCriticalSection::LockTry
//*---------------------------------------------------------------
// DESC : 임계구역 진입 시도
// PARM : N/A
// RETV : TRUE - 성공 / FALSE - 실패
// PRGM : milkji, B4nFter
//*---------------------------------------------------------------
inline BOOL CCriticalSection::LockTry() const
{
#if	(WINVER >= 0x0500)
	if (::TryEnterCriticalSection(&m_crThis)) 
		return(TRUE);
	else 
		return(FALSE);
#else	// #if	(WINVER >= 0x0500)
	BASE_RETURN(FALSE);
#endif	// #if	(WINVER >= 0x0500)
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CCriticalSection::Unlock
//*---------------------------------------------------------------
// DESC : 임계구역 해제
// PARM : N/A
// RETV : N/A
// PRGM : milkji, B4nFter
//*---------------------------------------------------------------
inline VOID CCriticalSection::Unlock() const
{
	::LeaveCriticalSection(&m_crThis);
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CCriticalSection::IsLock
//*---------------------------------------------------------------
// DESC : 임계구역에 이미 진입해 있는지 체크
// PARM : N/A
// RETV : TRUE - 맞음 / FALSE - 틀림
// PRGM : milkji, B4nFter
//*---------------------------------------------------------------
#if defined(_DEBUG)
inline BOOL CCriticalSection::IsLock() const
{
	return((0 < m_crThis.RecursionCount) && (::GetCurrentThreadId() == HandleToUlong(m_crThis.OwningThread)));
}
#endif // #if defined(_DEBUG)


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CCriticalSection::Reset
//*---------------------------------------------------------------
// DESC : 객체 자원 초기화
// PARM : N/A
// RETV : N/A
// PRGM : milkji, B4nFter
//*---------------------------------------------------------------
inline VOID CCriticalSection::Reset()
{
	m_bIsOpen = FALSE;
}

