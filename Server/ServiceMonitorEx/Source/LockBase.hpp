/*//===============================================================

	<< LOCK-BASE >>

	PRGM : B4nFter

	FILE : LOCKBASE.HPP
	DESC : Lock 클래스의 기본, 관리 클래스
	INIT BUILT DATE : 2007. 03. 12
	LAST BUILT DATE : 2007. 07. 15

	Copyrightⓒ 2002~2009 B4nFter (b4nfter@gmail.com) and Co-Writers. All Rights Reserved.

*///===============================================================


#pragma once


///////////////////////////////////////////////////////////////////
//	INCLUDE
///////////////////////////////////////////////////////////////////

#include "SYSBASEBASE.H"


///////////////////////////////////////////////////////////////////
//	DEFINE
///////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////
//	DECLARATION
///////////////////////////////////////////////////////////////////

//*---------------------------------------------------------------
// TYPE : CLASS
// NAME : CLockBase
//*---------------------------------------------------------------
// DESC : Lock 기본 클래스
// PRGM : B4nFter
//*---------------------------------------------------------------
class CLockBase
{
public:
	CLockBase();
	virtual ~CLockBase() { }

	virtual BOOL IsOpen() const = 0;

	virtual VOID Lock(BOOL bDoCheck = FALSE) const = 0;
	virtual VOID Unlock() const = 0;

#if defined(_DEBUG)
	virtual BOOL IsLock() const = 0;
#endif	// #if defined(_DEBUG)

protected:
	BOOL m_bIsOpen;
};


//*---------------------------------------------------------------
// TYPE : CLASS
// NAME : CLockAuto
//*---------------------------------------------------------------
// DESC : CLockBase 파생클래스의 자동 잠금/해제 관리 클래스
// PRGM : B4nFter
//*---------------------------------------------------------------
class CLockAuto
{
	DISABLE_COPY_ASSIGN(CLockAuto)	// 객체간 복사, 대입 금지

public:
	explicit CLockAuto(const CLockBase& lpLock);
	explicit CLockAuto(const CLockBase* lpLock);
	~CLockAuto();

private:
	const CLockBase* m_lpLock;
};


//*---------------------------------------------------------------
// TYPE : CLASS
// NAME : CLockAutoEx
//*---------------------------------------------------------------
// DESC : Lock() / Unlock() 메서드를 구현한 TLOCK 클래스 객체의 자동 잠금/해제 관리 클래스 (템플릿)
// PRGM : B4nFter
//*---------------------------------------------------------------
template<class TLOCK>
class CLockAutoEx
{
//	DISABLE_COPY_ASSIGN(CLockAutoEx)	// 객체간 복사, 대입 금지

public:
	explicit CLockAutoEx(const TLOCK& pLock);
	explicit CLockAutoEx(const TLOCK* pLock);
	~CLockAutoEx();

private:
	// 객체간 복사, 대입 금지
	CLockAutoEx(const CLockAutoEx<TLOCK>& pLock) : m_lpLock(pLock.m_lpLock)	{ }
#pragma warning (disable:4100)
	CLockAutoEx& operator=(const CLockAutoEx<TLOCK>& pLock)					{ return(*this); }
#pragma warning (default:4100)

private:
	const TLOCK* m_lpLock;
};


//*---------------------------------------------------------------
// TYPE : CLASS
// NAME : CUnlockAuto
//*---------------------------------------------------------------
// DESC : CLockBase 파생클래스의 자동 해제 관리 클래스
// PRGM : B4nFter
//*---------------------------------------------------------------
class CUnlockAuto
{
	DISABLE_COPY_ASSIGN(CUnlockAuto)	// 객체간 복사, 대입 금지

public:
	explicit CUnlockAuto(const CLockBase& lpLock);
	explicit CUnlockAuto(const CLockBase* lpLock);
	~CUnlockAuto();

private:
	const CLockBase* m_lpLock;
};


//*---------------------------------------------------------------
// TYPE : CLASS
// NAME : CUnlockAutoEx
//*---------------------------------------------------------------
// DESC : Unlock() 메서드를 구현한 TLOCK 클래스 객체의 자동 해제 관리 클래스
// PRGM : B4nFter
//*---------------------------------------------------------------
template<class TLOCK>
class CUnlockAutoEx
{
//	DISABLE_COPY_ASSIGN(CUnlockAutoEx)	// 객체간 복사, 대입 금지

public:
	explicit CUnlockAutoEx(const TLOCK& pLock);
	explicit CUnlockAutoEx(const TLOCK* pLock);
	~CUnlockAutoEx();

private:
	// 객체간 복사, 대입 금지
	CUnlockAutoEx(const CUnlockAutoEx<TLOCK>& pLock) : m_lpLock(pLock.m_lpLock)	{  }
#pragma warning (disable:4100)
	CUnlockAutoEx& operator=(const CUnlockAutoEx<TLOCK>& pLock)					{ return(*this); }
#pragma warning (default:4100)

private:
	const TLOCK* m_lpLock;
};


//*---------------------------------------------------------------
// TYPE : CLASS
// NAME : CUnlockUndo
//*---------------------------------------------------------------
// DESC : CLockBase 파생클래스의 자동 해제/잠금 관리 클래스
// PRGM : B4nFter
//*---------------------------------------------------------------
class CUnlockUndo
{
	DISABLE_COPY_ASSIGN(CUnlockUndo)	// 객체간 복사, 대입 금지

public:
	explicit CUnlockUndo(const CLockBase& lpLock);
	explicit CUnlockUndo(const CLockBase* lpLock);
	~CUnlockUndo();

private:
	const CLockBase* m_lpLock;
};


//*---------------------------------------------------------------
// TYPE : CLASS
// NAME : CUnlockUndoEx
//*---------------------------------------------------------------
// DESC : Lock() / Unlock() 메서드를 구현한 TLOCK 클래스 객체의 자동 해제/잠금 관리 클래스 (템플릿)
// PRGM : B4nFter
//*---------------------------------------------------------------
template<class TLOCK>
class CUnlockUndoEx
{
//	DISABLE_COPY_ASSIGN(CUnlockUndoEx)	// 객체간 복사, 대입 금지

public:
	explicit CUnlockUndoEx(const TLOCK& pLock);
	explicit CUnlockUndoEx(const TLOCK* pLock);
	~CUnlockUndoEx();

private:
	// 객체간 복사, 대입 금지
	CUnlockUndoEx(const CUnlockUndoEx<TLOCK>& pLock) : m_lpLock(pLock.m_lpLock)	{  }
#pragma warning (disable:4100)
	CUnlockUndoEx& operator=(const CUnlockUndoEx<TLOCK>& pLock)					{  return(*this); }
#pragma warning (default:4100)

private:
	const TLOCK* m_lpLock;
};


///////////////////////////////////////////////////////////////////
//	IMPLEMENTATION
///////////////////////////////////////////////////////////////////

//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CLockBase::CLockBase
//*---------------------------------------------------------------
// DESC : 생성자
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
inline CLockBase::CLockBase() : m_bIsOpen(NULL)
{
	
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CLockAuto::CLockAuto
//*---------------------------------------------------------------
// DESC : 생성자
// PARM :	1 . lpLock - CLockBase 의 파생클래스 객체참조
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
inline CLockAuto::CLockAuto(const CLockBase& lpLock) : m_lpLock(&lpLock)
{
	if (m_lpLock) 
	{
		m_lpLock->Lock();
	}
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CLockAuto::CLockAuto
//*---------------------------------------------------------------
// DESC : 생성자
// PARM :	1 . lpLock - CLockBase 의 파생클래스 객체포인터
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
inline CLockAuto::CLockAuto(const CLockBase* lpLock) : m_lpLock(lpLock)
{
	if (m_lpLock) 
	{
		m_lpLock->Lock();
	}
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CLockAuto::~CLockAuto
//*---------------------------------------------------------------
// DESC : 소멸자
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
inline CLockAuto::~CLockAuto()
{
	if (m_lpLock) {
		m_lpLock->Unlock();
		m_lpLock = NULL;
	}
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CLockAutoEx::CLockAutoEx
//*---------------------------------------------------------------
// DESC : 생성자
// PARM :	1 . pLock - TLOCK 클래스 객체참조
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
template<class TLOCK>
inline CLockAutoEx<TLOCK>::CLockAutoEx(const TLOCK& pLock) : m_lpLock(&pLock)
{
	if (m_lpLock) 
	{
		m_lpLock->Lock();
	}
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CLockAutoEx::CLockAutoEx
//*---------------------------------------------------------------
// DESC : 생성자
// PARM :	1 . pLock - TLOCK 클래스 객체포인터
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
template<class TLOCK>
inline CLockAutoEx<TLOCK>::CLockAutoEx(const TLOCK* pLock) : m_lpLock(pLock)
{
	if (m_lpLock) 
	{
		m_lpLock->Lock();
	}
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CLockAutoEx::~CLockAutoEx
//*---------------------------------------------------------------
// DESC : 소멸자
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
template<class TLOCK>
inline CLockAutoEx<TLOCK>::~CLockAutoEx()
{
	if (m_lpLock) {
		m_lpLock->Unlock();
		m_lpLock = NULL;
	}
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CUnlockAuto::CUnlockAuto
//*---------------------------------------------------------------
// DESC : 생성자
// PARM :	1 . lpLock - CLockBase 의 파생클래스 객체참조
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
inline CUnlockAuto::CUnlockAuto(const CLockBase& lpLock) : m_lpLock(&lpLock)
{

}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CUnlockAuto::CUnlockAuto
//*---------------------------------------------------------------
// DESC : 생성자
// PARM :	1 . lpLock - CLockBase 의 파생클래스 객체포인터
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
inline CUnlockAuto::CUnlockAuto(const CLockBase* lpLock) : m_lpLock(lpLock)
{

}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CUnlockAuto::~CUnlockAuto
//*---------------------------------------------------------------
// DESC : 소멸자
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
inline CUnlockAuto::~CUnlockAuto()
{
	if (m_lpLock) {
		m_lpLock->Unlock();
		m_lpLock = NULL;
	}
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CUnlockAutoEx::CUnlockAutoEx
//*---------------------------------------------------------------
// DESC : 생성자
// PARM :	1 . pLock - TLOCK 클래스 객체참조
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
template<class TLOCK>
inline CUnlockAutoEx<TLOCK>::CUnlockAutoEx(const TLOCK& pLock) : m_lpLock(&pLock)
{

}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CUnlockAutoEx::CUnlockAutoEx
//*---------------------------------------------------------------
// DESC : 생성자
// PARM :	1 . pLock - TLOCK 클래스 객체포인터
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
template<class TLOCK>
inline CUnlockAutoEx<TLOCK>::CUnlockAutoEx(const TLOCK* pLock) : m_lpLock(pLock)
{
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CUnlockAutoEx::~CUnlockAutoEx
//*---------------------------------------------------------------
// DESC : 소멸자
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
template<class TLOCK>
inline CUnlockAutoEx<TLOCK>::~CUnlockAutoEx()
{
	if (m_lpLock) {
		m_lpLock->Unlock();
		m_lpLock = NULL;
	}
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CUnlockUndo::CUnlockUndo
//*---------------------------------------------------------------
// DESC : 생성자
// PARM :	1 . lpLock - CLockBase 의 파생클래스 객체참조
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
inline CUnlockUndo::CUnlockUndo(const CLockBase& lpLock) : m_lpLock(&lpLock)
{
	if (m_lpLock) 
	{
		m_lpLock->Unlock();
	}
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CUnlockUndo::CUnlockUndo
//*---------------------------------------------------------------
// DESC : 생성자
// PARM :	1 . lpLock - CLockBase 의 파생클래스 객체포인터
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
inline CUnlockUndo::CUnlockUndo(const CLockBase* lpLock) : m_lpLock(lpLock)
{
	if (m_lpLock) 
	{
		m_lpLock->Unlock();
	}
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CUnlockUndo::~CUnlockUndo
//*---------------------------------------------------------------
// DESC : 소멸자
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
inline CUnlockUndo::~CUnlockUndo()
{
	if (m_lpLock) {
		m_lpLock->Lock();
		m_lpLock = NULL;
	}
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CUnlockUndoEx::CUnlockUndoEx
//*---------------------------------------------------------------
// DESC : 생성자
// PARM :	1 . pLock - TLOCK 클래스 객체참조
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
template<class TLOCK>
inline CUnlockUndoEx<TLOCK>::CUnlockUndoEx(const TLOCK& pLock) : m_lpLock(&pLock)
{
	if (m_lpLock) 
	{
		m_lpLock->Unlock();
	}
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CUnlockUndoEx::CUnlockUndoEx
//*---------------------------------------------------------------
// DESC : 생성자
// PARM :	1 . pLock - TLOCK 클래스 객체포인터
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
template<class TLOCK>
inline CUnlockUndoEx<TLOCK>::CUnlockUndoEx(const TLOCK* pLock) : m_lpLock(pLock)
{
	if (m_lpLock) {
		m_lpLock->Unlock();
	}
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CUnlockUndoEx::~CUnlockUndoEx
//*---------------------------------------------------------------
// DESC : 소멸자
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
template<class TLOCK>
inline CUnlockUndoEx<TLOCK>::~CUnlockUndoEx()
{
	if (m_lpLock) {
		m_lpLock->Lock();
		m_lpLock = NULL;
	}
}

