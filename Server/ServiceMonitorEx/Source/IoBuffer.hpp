/*//===============================================================

	<< I/O BUFFER >>

	PRGM : B4nFter

	FILE : IOBUFFER.HPP
	DESC : �⺻ I/O ����
	INIT BUILT DATE : 2005. 11. 21
	LAST BUILT DATE : 2005. 11. 29

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
// NAME : CIoBuffer
//*---------------------------------------------------------------
// DESC : �⺻ I/O ����
// PRGM : B4nFter
//*---------------------------------------------------------------
class CIoBuffer
{

private:
	LPBYTE m_lpbtBuffer;
	CIoBuffer* m_lpIoBufferNextNode;	// ���� ���Ḯ��Ʈ�� �����ϴ� ��쿡 ��밡���� ���� ������
	INT m_iBufferSize;
	INT m_iBufferHeadPtr;	// ���ۿ��� ���� �����͸� �Ҹ��� ��ġ�� ��Ÿ���� ������
	INT m_iBufferTailPtr;	// ���ۿ��� ���� �������� ������ ��ġ�� ��Ÿ���� ������
	BOOL m_bLockState;		// ������ ��ݻ��� (�ʿ��� ��츸 ���)
	INT m_iType;			// ������ Ÿ�� (�ʿ��� ��츸 ���)
	LPVOID m_lpParam;

public:
	CIoBuffer();
	virtual ~CIoBuffer();

	BOOL Initialize(INT uBufferSize, CIoBuffer* lpIoBufferNextNode = NULL);
	VOID Finalize();
	VOID Reset();

	CIoBuffer* GetIoBufferNextNode() { return m_lpIoBufferNextNode;	}
	VOID SetIoBufferNextNode(CIoBuffer* lpIoBufferNode) { m_lpIoBufferNextNode = lpIoBufferNode; }

	BOOL GetLockState() { return m_bLockState; }
	VOID SetLockState(BOOL bLockState) { m_bLockState = bLockState; }

	LPBYTE GetBuffer() { return m_lpbtBuffer; }
	INT GetBufferSize() { return m_iBufferSize; }

	BOOL SetBuffer(LPBYTE lpbtBuffer, INT iBufferSize);

	INT GetBufferHeadPtr() { return m_iBufferHeadPtr; }
	VOID SetBufferHeadPtr(INT iHeadBufferPtr) { m_iBufferHeadPtr = iHeadBufferPtr; }
	VOID AddBufferHeadPtr(INT iHeadBufferPtr) { m_iBufferHeadPtr += iHeadBufferPtr; }
	INT& RefBufferHeadPtr() { return m_iBufferHeadPtr; }

	INT GetBufferTailPtr() { return m_iBufferTailPtr; }
	VOID SetBufferTailPtr(INT iTailBufferPtr) { m_iBufferTailPtr = iTailBufferPtr; }
	VOID AddBufferTailPtr(INT iTailBufferPtr) { m_iBufferTailPtr += iTailBufferPtr; }
	INT& RefBufferTailPtr() { return m_iBufferTailPtr; }

	inline LPBYTE GetFreeBuffer(INT& iBufferSize);
	INT GetFreeBufferSize() { return m_iBufferSize - m_iBufferTailPtr; }	// �����Ͱ� ������� ���� �� ������ ũ�⸦ ����

	INT GetType() { return m_iType; }
	VOID SetType(INT iType) { m_iType = iType; }

	LPVOID GetParam() { return m_lpParam; }
	VOID SetParam(LPVOID lpParam) { m_lpParam = lpParam; }
};


///////////////////////////////////////////////////////////////////
//	IMPLEMENTATION
///////////////////////////////////////////////////////////////////

//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CIoBuffer::CIoBuffer
//*---------------------------------------------------------------
// DESC : ������
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
inline CIoBuffer::CIoBuffer()
{
	m_lpbtBuffer = NULL;
	m_lpIoBufferNextNode = NULL;
	m_iBufferSize = 0;
	m_iType = -1;
	m_lpParam = NULL;
	Reset();
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CIoBuffer::~CIoBuffer
//*---------------------------------------------------------------
// DESC : �Ҹ���
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
inline CIoBuffer::~CIoBuffer()
{
	Finalize();
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CIoBuffer::Initialize
//*---------------------------------------------------------------
// DESC : ��ü ���� �ڿ����� �ʱ�ȭ
// PARM :	1 . uBufferSize - �������� ������ ������ ũ��
//			2 . lpIoBufferNode - ���۰�ü�� ����� �ܺ� ������ ������ (�ʿ��� ��츸 ���)
// RETV : TRUE - ���� / FALSE - ����
// PRGM : B4nFter
//*---------------------------------------------------------------
inline BOOL CIoBuffer::Initialize(INT uBufferSize, CIoBuffer* lpIoBufferNextNode)
{
	if (!uBufferSize || m_lpbtBuffer) {
		return FALSE;
	}

	m_lpbtBuffer = NEW BYTE[uBufferSize];
	m_lpIoBufferNextNode = lpIoBufferNextNode;
	m_iBufferSize = uBufferSize;
	Reset();

	return TRUE;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CIoBuffer::Finalize
//*---------------------------------------------------------------
// DESC : ��ü ���� �ڿ����� ����
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
inline VOID CIoBuffer::Finalize()
{
	SAFE_DELETEARRAY(m_lpbtBuffer);
	m_iBufferSize = 0;
	m_iBufferHeadPtr = 0;
	m_iBufferTailPtr = 0;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CIoBuffer::Reset
//*---------------------------------------------------------------
// DESC : ��ü���� �ڿ� �ʱ�ȭ
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
inline VOID CIoBuffer::Reset()
{
	m_iBufferHeadPtr = 0;
	m_iBufferTailPtr = 0;
	m_bLockState = FALSE;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CIoBuffer::SetBuffer
//*---------------------------------------------------------------
// DESC : ���۸� Ư�� �����ͷ� ����
// PARM :	1 . lpbtBuffer - ������ ����
//			2 . iBufferSize - ������ ������ ũ��
// RETV : TRUE - ���� / FALSE - ����
// PRGM : B4nFter
//*---------------------------------------------------------------
inline BOOL CIoBuffer::SetBuffer(LPBYTE lpbtBuffer, INT iBufferSize)
{
	if (!lpbtBuffer || 0 >= iBufferSize || !m_lpbtBuffer) {
		BASE_RETURN(FALSE);
	}
	if (GetFreeBufferSize() < iBufferSize) {
		BASE_RETURN(FALSE);
	}
	
	::memcpy_s(m_lpbtBuffer + m_iBufferTailPtr, GetFreeBufferSize(), lpbtBuffer, iBufferSize);
	AddBufferTailPtr(iBufferSize);

	return TRUE;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CIoBuffer::GetFreeBuffer
//*---------------------------------------------------------------
// DESC : ��� ������ ������ ���� �����Ϳ� ��� ������ ũ�⸦ ��ȯ
// PARM :	1 . iBufferSize - ��� ������ ������ ũ�⸦ ��ȯ�ޱ� ���� ����
// RETV : ��� ������ ������ ���� ������
// PRGM : B4nFter
//*---------------------------------------------------------------
inline LPBYTE CIoBuffer::GetFreeBuffer(INT& iBufferSize)
{
	if (m_lpbtBuffer) {
		iBufferSize = m_iBufferSize - m_iBufferTailPtr;
		return m_lpbtBuffer + m_iBufferTailPtr;
	}

	return NULL;
}

