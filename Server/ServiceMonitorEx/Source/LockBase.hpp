/*//===============================================================

	<< LOCK-BASE >>

	PRGM : B4nFter

	FILE : LOCKBASE.HPP
	DESC : Lock Ŭ������ �⺻, ���� Ŭ����
	INIT BUILT DATE : 2007. 03. 12
	LAST BUILT DATE : 2007. 07. 15

	Copyright�� 2002~2009 B4nFter (b4nfter@gmail.com) and Co-Writers. All Rights Reserved.

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
// DESC : Lock �⺻ Ŭ����
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
// DESC : CLockBase �Ļ�Ŭ������ �ڵ� ���/���� ���� Ŭ����
// PRGM : B4nFter
//*---------------------------------------------------------------
class CLockAuto
{
	DISABLE_COPY_ASSIGN(CLockAuto)	// ��ü�� ����, ���� ����

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
// DESC : Lock() / Unlock() �޼��带 ������ TLOCK Ŭ���� ��ü�� �ڵ� ���/���� ���� Ŭ���� (���ø�)
// PRGM : B4nFter
//*---------------------------------------------------------------
template<class TLOCK>
class CLockAutoEx
{
//	DISABLE_COPY_ASSIGN(CLockAutoEx)	// ��ü�� ����, ���� ����

public:
	explicit CLockAutoEx(const TLOCK& pLock);
	explicit CLockAutoEx(const TLOCK* pLock);
	~CLockAutoEx();

private:
	// ��ü�� ����, ���� ����
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
// DESC : CLockBase �Ļ�Ŭ������ �ڵ� ���� ���� Ŭ����
// PRGM : B4nFter
//*---------------------------------------------------------------
class CUnlockAuto
{
	DISABLE_COPY_ASSIGN(CUnlockAuto)	// ��ü�� ����, ���� ����

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
// DESC : Unlock() �޼��带 ������ TLOCK Ŭ���� ��ü�� �ڵ� ���� ���� Ŭ����
// PRGM : B4nFter
//*---------------------------------------------------------------
template<class TLOCK>
class CUnlockAutoEx
{
//	DISABLE_COPY_ASSIGN(CUnlockAutoEx)	// ��ü�� ����, ���� ����

public:
	explicit CUnlockAutoEx(const TLOCK& pLock);
	explicit CUnlockAutoEx(const TLOCK* pLock);
	~CUnlockAutoEx();

private:
	// ��ü�� ����, ���� ����
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
// DESC : CLockBase �Ļ�Ŭ������ �ڵ� ����/��� ���� Ŭ����
// PRGM : B4nFter
//*---------------------------------------------------------------
class CUnlockUndo
{
	DISABLE_COPY_ASSIGN(CUnlockUndo)	// ��ü�� ����, ���� ����

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
// DESC : Lock() / Unlock() �޼��带 ������ TLOCK Ŭ���� ��ü�� �ڵ� ����/��� ���� Ŭ���� (���ø�)
// PRGM : B4nFter
//*---------------------------------------------------------------
template<class TLOCK>
class CUnlockUndoEx
{
//	DISABLE_COPY_ASSIGN(CUnlockUndoEx)	// ��ü�� ����, ���� ����

public:
	explicit CUnlockUndoEx(const TLOCK& pLock);
	explicit CUnlockUndoEx(const TLOCK* pLock);
	~CUnlockUndoEx();

private:
	// ��ü�� ����, ���� ����
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
// DESC : ������
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
// DESC : ������
// PARM :	1 . lpLock - CLockBase �� �Ļ�Ŭ���� ��ü����
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
// DESC : ������
// PARM :	1 . lpLock - CLockBase �� �Ļ�Ŭ���� ��ü������
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
// DESC : �Ҹ���
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
// DESC : ������
// PARM :	1 . pLock - TLOCK Ŭ���� ��ü����
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
// DESC : ������
// PARM :	1 . pLock - TLOCK Ŭ���� ��ü������
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
// DESC : �Ҹ���
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
// DESC : ������
// PARM :	1 . lpLock - CLockBase �� �Ļ�Ŭ���� ��ü����
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
// DESC : ������
// PARM :	1 . lpLock - CLockBase �� �Ļ�Ŭ���� ��ü������
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
// DESC : �Ҹ���
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
// DESC : ������
// PARM :	1 . pLock - TLOCK Ŭ���� ��ü����
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
// DESC : ������
// PARM :	1 . pLock - TLOCK Ŭ���� ��ü������
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
// DESC : �Ҹ���
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
// DESC : ������
// PARM :	1 . lpLock - CLockBase �� �Ļ�Ŭ���� ��ü����
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
// DESC : ������
// PARM :	1 . lpLock - CLockBase �� �Ļ�Ŭ���� ��ü������
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
// DESC : �Ҹ���
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
// DESC : ������
// PARM :	1 . pLock - TLOCK Ŭ���� ��ü����
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
// DESC : ������
// PARM :	1 . pLock - TLOCK Ŭ���� ��ü������
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
// DESC : �Ҹ���
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

