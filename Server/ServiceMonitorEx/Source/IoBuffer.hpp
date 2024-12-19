/*//===============================================================

	<< I/O BUFFER >>

	PRGM : B4nFter

	FILE : IOBUFFER.HPP
	DESC : 기본 I/O 버퍼
	INIT BUILT DATE : 2005. 11. 21
	LAST BUILT DATE : 2005. 11. 29

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
// NAME : CIoBuffer
//*---------------------------------------------------------------
// DESC : 기본 I/O 버퍼
// PRGM : B4nFter
//*---------------------------------------------------------------
class CIoBuffer
{

private:
	LPBYTE m_lpbtBuffer;
	CIoBuffer* m_lpIoBufferNextNode;	// 버퍼 연결리스트를 구성하는 경우에 사용가능한 참조 포인터
	INT m_iBufferSize;
	INT m_iBufferHeadPtr;	// 버퍼에서 받은 데이터를 소모한 위치를 나타내는 포인터
	INT m_iBufferTailPtr;	// 버퍼에서 받은 데이터의 마지막 위치를 나타내는 포인터
	BOOL m_bLockState;		// 버퍼의 잠금상태 (필요한 경우만 사용)
	INT m_iType;			// 버퍼의 타입 (필요한 경우만 사용)
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
	INT GetFreeBufferSize() { return m_iBufferSize - m_iBufferTailPtr; }	// 데이터가 들어있지 않은 빈 버퍼의 크기를 리턴

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
// DESC : 생성자
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
// DESC : 소멸자
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
// DESC : 객체 관련 자원들을 초기화
// PARM :	1 . uBufferSize - 동적으로 생성할 버퍼의 크기
//			2 . lpIoBufferNode - 버퍼객체와 연결될 외부 버퍼의 포인터 (필요한 경우만 사용)
// RETV : TRUE - 성공 / FALSE - 실패
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
// DESC : 객체 관련 자원들을 해제
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
// DESC : 객체관련 자원 초기화
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
// DESC : 버퍼를 특정 데이터로 세팅
// PARM :	1 . lpbtBuffer - 세팅할 버퍼
//			2 . iBufferSize - 세팅할 버퍼의 크기
// RETV : TRUE - 성공 / FALSE - 실패
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
// DESC : 사용 가능한 버퍼의 시작 포인터와 사용 가능한 크기를 반환
// PARM :	1 . iBufferSize - 사용 가능한 버퍼의 크기를 반환받기 위한 변수
// RETV : 사용 가능한 버퍼의 시작 포인터
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

