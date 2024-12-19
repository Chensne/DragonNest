/*//===============================================================

	<< ACCESS >>

	FILE : ACCESS.H, ACCESS.CPP
	DESC : 특정 자원이나 객체 공유 시 배타적 잠금을 사용하지 참조 정보만으로 접근하고자 할 때 참조 정보를 관리
	INIT BUILT DATE : 2007. 07. 25
	LAST BUILT DATE : 2007. 07. 29

	Copyrightⓒ 2002~2009 B4nFter (b4nfter@gmail.com) and Co-Writers. All Rights Reserved.

*///===============================================================


#pragma once


///////////////////////////////////////////////////////////////////
//	INCLUDE
///////////////////////////////////////////////////////////////////

#include "CRITICALSECTION.H"
#if defined(_DEBUG)
#include <MAP>
#define	DF_DEBUG_COUNT	(1000)	// Access/Detach 무결성 체크 시 Access 개수 허용범위
#endif // _DEBUG


///////////////////////////////////////////////////////////////////
//	DECLARATION
///////////////////////////////////////////////////////////////////

//*---------------------------------------------------------------
// TYPE : CLASS
// NAME : CAccess
//*---------------------------------------------------------------
// DESC : 참조 정보 관리 클래스
// PRGM : B4nFter
//*---------------------------------------------------------------
class CAccess
{
private:
//	DISABLE_COPY_ASSIGN(CAccess)	// 객체간 복사, 대입 금지

public:
	typedef	CSyncLock						TP_LOCK;
	typedef	ScopeLock<CSyncLock>			TP_LOCKAUTO;
#if defined(_DEBUG)
	typedef std::map<DWORD, DWORD>			TP_DEBUGLIST;
	typedef TP_DEBUGLIST::iterator			TP_DEBUGLIST_ITR;
	typedef TP_DEBUGLIST::const_iterator	TP_DEBUGLIST_CTR;
#endif // _DEBUG

public:
	CAccess();
	~CAccess();

	DWORD Open(BOOL pDoEnable = FALSE);
	VOID Close();
	BOOL IsOpen() const { return m_bOpen; }

#if defined(_DEBUG)
	static DWORD OpenDebug();
//	MODE_DBG(static DWORD OpenDebug());
	static DWORD GetAccessCountByThread();
//	MODE_DBG(static DWORD GetAccessCountByThread());
#endif // _DEBUG

	VOID Reset();
	VOID ResetNoLock();

	DWORD GetAccessCount() const;
	DWORD GetAccessCountNoLock() const;

	BOOL CanAttach();
	BOOL CanAttachNoLock();

	BOOL IsAttach() const;
	BOOL Attach();
//	BOOL AttachTry();
	VOID AttachMust();

	BOOL Detach();
	BOOL DetachMust();

	VOID AttachMain();
	VOID DetachMain();

	BOOL IsEnable() const { return m_bEnable; }
	VOID DisableAttach();
	VOID DisableAttachNoLock();

	TP_LOCK* GetLock() const { return &m_Lock; }

	VOID Lock() const;
	VOID Unlock() const;
//	BOOL LockTry() const;
#if defined(_DEBUG)
	BOOL IsLock() const;
#endif // _DEBUG

private:
	DWORD m_dwAccessCount;
	BOOL m_bEnable;
	BOOL m_bOpen;
	mutable TP_LOCK m_Lock;
#if defined(_DEBUG)
#pragma warning(disable:4251)
	static TP_DEBUGLIST m_Debug;
//	MODE_DBG(static TP_DEBUGLIST m_Debug);
#pragma warning(default:4251)
	static TP_LOCK m_DebugLock;
//	MODE_DBG(static TP_LOCK m_DebugLock);
#endif // _DEBUG
};


//*---------------------------------------------------------------
// TYPE : CLASS
// NAME : CDetachAuto
//*---------------------------------------------------------------
// DESC : CAccess 파생클래스의 자동 해제를 관리하는 클래스
// PRGM : B4nFter
//*---------------------------------------------------------------
class CDetachAuto
{
//	DISABLE_COPY_ASSIGN(CDetachAuto)	// 객체간 복사, 대입 금지

public:
	explicit CDetachAuto(CAccess& lpAccess);
	explicit CDetachAuto(CAccess* lpAccess);
	~CDetachAuto();

/*
private:
	// 객체간 복사, 대입 금지
	CDetachAuto(const CDetachAuto& pAccess) : m_lpAccess(pAccess.m_lpAccess)	{ DN_ASSERT(0, "Can't be Called!"); }
#pragma warning (disable:4100)
	CDetachAuto& operator=(const CDetachAuto& pLock)							{ DN_ASSERT(0, "Can't be Called!"); return(*this); }
#pragma warning (default:4100)
*/

private:
	CAccess* m_lpAccess;
};


//*---------------------------------------------------------------
// TYPE : CLASS
// NAME : CDetachAutoEx
//*---------------------------------------------------------------
// DESC : Detach() 메서드를 구현된 TACCESS 클래스 객체의 자동 해제를 관리하는 클래스
// PRGM : B4nFter
//*---------------------------------------------------------------
template<class TACCESS>
class CDetachAutoEx
{
//	DISABLE_COPY_ASSIGN(CDetachAutoEx)	// 객체간 복사, 대입 금지

public:
	explicit CDetachAutoEx(TACCESS& pAccess);
	explicit CDetachAutoEx(TACCESS* pAccess);
	~CDetachAutoEx();

private:
	// 객체간 복사, 대입 금지
	CDetachAutoEx(const CDetachAutoEx<TACCESS>& pAccess) : m_lpAccess(pAccess.m_lpAccess)	{ DN_ASSERT(0, "Can't be Called!"); }
#pragma warning (disable:4100)
	CDetachAutoEx& operator=(const CDetachAutoEx<TACCESS>& pLock)							{ DN_ASSERT(0, "Can't be Called!"); return(*this); }
#pragma warning (disable:4100)

private:
	TACCESS* m_lpAccess;
};


///////////////////////////////////////////////////////////////////
//	IMPLEMENTATION
///////////////////////////////////////////////////////////////////

//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CAccess::IsOpen
//*---------------------------------------------------------------
// DESC : CAccess 객체가 사용 가능한지 여부 반환
// PARM : N/A
// RETV : TRUE - 성공 / FALSE - 실패
// PRGM : B4nFter
//*---------------------------------------------------------------
/*
inline BOOL CAccess::IsOpen() const
{
	return(m_Lock.IsOpen());
}
*/


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CAccess::IsAttach
//*---------------------------------------------------------------
// DESC : CAccess 객체가 Attach() 되었는지 여부 반환
// PARM : N/A
// RETV : TRUE - 성공 / FALSE - 실패
// PRGM : B4nFter
//*---------------------------------------------------------------
inline BOOL CAccess::IsAttach() const
{
//	DN_ASSERT(IsOpen(),		"Invalid!");
	TP_LOCKAUTO aLockAuto(m_Lock);
	return(0 < m_dwAccessCount);
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CDetachAuto::CDetachAuto
//*---------------------------------------------------------------
// DESC : 생성자
// PARM :	1 . lpAccess - CAccess 클래스 객체참조
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
inline CDetachAuto::CDetachAuto(CAccess& lpAccess) : m_lpAccess(&lpAccess)
{
//	DN_ASSERT(NULL != m_lpAccess,		"Invalid!");
//	DN_ASSERT(m_lpAccess->IsAttach(),	"Not Attached!");	// Detach()에서 체크됨
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CDetachAuto::CDetachAuto
//*---------------------------------------------------------------
// DESC : 생성자
// PARM :	1 . lpAccess - CAccess 클래스 객체포인터
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
inline CDetachAuto::CDetachAuto(CAccess* lpAccess) : m_lpAccess(lpAccess)
{
//	DN_ASSERT(NULL != m_lpAccess,		"Invalid!");
//	DN_ASSERT(m_lpAccess->IsAttach(),	"Not Attached!");	// Detach()에서 체크됨
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CDetachAuto::~CDetachAuto
//*---------------------------------------------------------------
// DESC : 소멸자
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
inline CDetachAuto::~CDetachAuto()
{
//	DN_ASSERT(NULL != m_lpAccess,	"Invalid!");
	if (m_lpAccess) {
		m_lpAccess->Detach();
	}
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CDetachAutoEx::CDetachAutoEx
//*---------------------------------------------------------------
// DESC : 생성자
// PARM :	1 . pAccess - TACCESS 클래스 객체참조
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
template<class TACCESS>
inline CDetachAutoEx<TACCESS>::CDetachAutoEx(TACCESS& pAccess) : m_lpAccess(&pAccess)
{
//	DN_ASSERT(NULL != m_lpAccess,		"Invalid!");
//	DN_ASSERT(m_lpAccess->IsAttach(),	"Not Attached!");	// Detach()에서 체크됨

	if (m_lpAccess) {
		m_lpAccess->Attach();
	}
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CDetachAutoEx::CDetachAutoEx
//*---------------------------------------------------------------
// DESC : 생성자
// PARM :	1 . pAccess - TACCESS 클래스 객체포인터
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
template<class TACCESS>
inline CDetachAutoEx<TACCESS>::CDetachAutoEx(TACCESS* pAccess) : m_lpAccess(pAccess)
{
//	DN_ASSERT(NULL != m_lpAccess,		"Invalid!");
//	DN_ASSERT(m_lpAccess->IsAttach(),	"Not Attached!");	// Detach()에서 체크됨

	if (m_lpAccess) {
		m_lpAccess->Attach();
	}
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CDetachAutoEx::~CDetachAutoEx
//*---------------------------------------------------------------
// DESC : 소멸자
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
template<class TACCESS>
inline CDetachAutoEx<TACCESS>::~CDetachAutoEx()
{
//	DN_ASSERT(NULL != m_lpAccess,	"Invalid!");
	if (m_lpAccess) {
		m_lpAccess->Detach();
	}
}

