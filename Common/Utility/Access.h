/*//===============================================================

	<< ACCESS >>

	FILE : ACCESS.H, ACCESS.CPP
	DESC : Ư�� �ڿ��̳� ��ü ���� �� ��Ÿ�� ����� ������� ���� ���������� �����ϰ��� �� �� ���� ������ ����
	INIT BUILT DATE : 2007. 07. 25
	LAST BUILT DATE : 2007. 07. 29

	Copyright�� 2002~2009 B4nFter (b4nfter@gmail.com) and Co-Writers. All Rights Reserved.

*///===============================================================


#pragma once


///////////////////////////////////////////////////////////////////
//	INCLUDE
///////////////////////////////////////////////////////////////////

#include "CRITICALSECTION.H"
#if defined(_DEBUG)
#include <MAP>
#define	DF_DEBUG_COUNT	(1000)	// Access/Detach ���Ἲ üũ �� Access ���� ������
#endif // _DEBUG


///////////////////////////////////////////////////////////////////
//	DECLARATION
///////////////////////////////////////////////////////////////////

//*---------------------------------------------------------------
// TYPE : CLASS
// NAME : CAccess
//*---------------------------------------------------------------
// DESC : ���� ���� ���� Ŭ����
// PRGM : B4nFter
//*---------------------------------------------------------------
class CAccess
{
private:
//	DISABLE_COPY_ASSIGN(CAccess)	// ��ü�� ����, ���� ����

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
// DESC : CAccess �Ļ�Ŭ������ �ڵ� ������ �����ϴ� Ŭ����
// PRGM : B4nFter
//*---------------------------------------------------------------
class CDetachAuto
{
//	DISABLE_COPY_ASSIGN(CDetachAuto)	// ��ü�� ����, ���� ����

public:
	explicit CDetachAuto(CAccess& lpAccess);
	explicit CDetachAuto(CAccess* lpAccess);
	~CDetachAuto();

/*
private:
	// ��ü�� ����, ���� ����
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
// DESC : Detach() �޼��带 ������ TACCESS Ŭ���� ��ü�� �ڵ� ������ �����ϴ� Ŭ����
// PRGM : B4nFter
//*---------------------------------------------------------------
template<class TACCESS>
class CDetachAutoEx
{
//	DISABLE_COPY_ASSIGN(CDetachAutoEx)	// ��ü�� ����, ���� ����

public:
	explicit CDetachAutoEx(TACCESS& pAccess);
	explicit CDetachAutoEx(TACCESS* pAccess);
	~CDetachAutoEx();

private:
	// ��ü�� ����, ���� ����
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
// DESC : CAccess ��ü�� ��� �������� ���� ��ȯ
// PARM : N/A
// RETV : TRUE - ���� / FALSE - ����
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
// DESC : CAccess ��ü�� Attach() �Ǿ����� ���� ��ȯ
// PARM : N/A
// RETV : TRUE - ���� / FALSE - ����
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
// DESC : ������
// PARM :	1 . lpAccess - CAccess Ŭ���� ��ü����
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
inline CDetachAuto::CDetachAuto(CAccess& lpAccess) : m_lpAccess(&lpAccess)
{
//	DN_ASSERT(NULL != m_lpAccess,		"Invalid!");
//	DN_ASSERT(m_lpAccess->IsAttach(),	"Not Attached!");	// Detach()���� üũ��
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CDetachAuto::CDetachAuto
//*---------------------------------------------------------------
// DESC : ������
// PARM :	1 . lpAccess - CAccess Ŭ���� ��ü������
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
inline CDetachAuto::CDetachAuto(CAccess* lpAccess) : m_lpAccess(lpAccess)
{
//	DN_ASSERT(NULL != m_lpAccess,		"Invalid!");
//	DN_ASSERT(m_lpAccess->IsAttach(),	"Not Attached!");	// Detach()���� üũ��
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CDetachAuto::~CDetachAuto
//*---------------------------------------------------------------
// DESC : �Ҹ���
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
// DESC : ������
// PARM :	1 . pAccess - TACCESS Ŭ���� ��ü����
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
template<class TACCESS>
inline CDetachAutoEx<TACCESS>::CDetachAutoEx(TACCESS& pAccess) : m_lpAccess(&pAccess)
{
//	DN_ASSERT(NULL != m_lpAccess,		"Invalid!");
//	DN_ASSERT(m_lpAccess->IsAttach(),	"Not Attached!");	// Detach()���� üũ��

	if (m_lpAccess) {
		m_lpAccess->Attach();
	}
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CDetachAutoEx::CDetachAutoEx
//*---------------------------------------------------------------
// DESC : ������
// PARM :	1 . pAccess - TACCESS Ŭ���� ��ü������
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
template<class TACCESS>
inline CDetachAutoEx<TACCESS>::CDetachAutoEx(TACCESS* pAccess) : m_lpAccess(pAccess)
{
//	DN_ASSERT(NULL != m_lpAccess,		"Invalid!");
//	DN_ASSERT(m_lpAccess->IsAttach(),	"Not Attached!");	// Detach()���� üũ��

	if (m_lpAccess) {
		m_lpAccess->Attach();
	}
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CDetachAutoEx::~CDetachAutoEx
//*---------------------------------------------------------------
// DESC : �Ҹ���
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

