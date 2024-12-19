/*//===============================================================

	<< CRITICAL SECTION >>

	PRGM : milkji, B4nFter

	FILE : CRITICALSECTION.HPP
	DESC : �ڵ����� ����� ���� Ȯ��� �Ӱ豸��
	INIT BUILT DATE : 2005. 06. 02
	LAST BUILT DATE : 2005. 06. 02

	P.S.>	
		- DEBUG ��� ������ : �Ӱ豸�� ���� API�� ����/���� �ϴ°� ���� 3�� ������ ��������
		- RELEASE ��� ������ : �Ӱ豸�� ���� API�� ���� �������� ����

	Copyright�� 2002~2009 B4nFter (b4nfter@gmail.com) and Co-Writers. All Rights Reserved.

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
	DISABLE_COPY_ASSIGN(CCriticalSection)	// ��ü�� ����, ���� ����

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
// DESC : ������
// PARM :	1 . lpCriticalSection - ������ �Ӱ豸�� ��ü�� ������
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
// DESC : �Ҹ���
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
// DESC : ��ü ����, �ڿ� �ʱ�ȭ
// PARM :	1 . dwSpinCount - 
// RETV : NOERROR - ���� / �׿� - ����
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
				- ����ī��Ʈ�� �ֻ��� ��Ʈ�� ��Ʈ�ϸ� ::EnterCriticalSection() �� �ʿ��� �ڿ��� �̸� �Ҵ�
				- �̰��� ��緮�� Non-Paged Pool �� �Һ��ϹǷ� ���� ���� �Ӱ豸�� ���� �� ������
				- �̰��� ���� Windows Server 2000 ������ ������ (XP �̻󿡼��� ���ŵ�)
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
// DESC : ��ü ����, �ڿ� ����
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
// DESC : ���� ��ü�� �غ�Ǿ� �ִ��� üũ
// PARM : N/A
// RETV : TRUE - ���� / FALSE - Ʋ��
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
// DESC : �Ӱ豸�� ����
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
// DESC : �Ӱ豸�� ���� �õ�
// PARM : N/A
// RETV : TRUE - ���� / FALSE - ����
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
// DESC : �Ӱ豸�� ����
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
// DESC : �Ӱ豸���� �̹� ������ �ִ��� üũ
// PARM : N/A
// RETV : TRUE - ���� / FALSE - Ʋ��
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
// DESC : ��ü �ڿ� �ʱ�ȭ
// PARM : N/A
// RETV : N/A
// PRGM : milkji, B4nFter
//*---------------------------------------------------------------
inline VOID CCriticalSection::Reset()
{
	m_bIsOpen = FALSE;
}

