/*//===============================================================

	<< BIT SET EX (1) >>

	PRGM : B4nFter

	FILE : BITSETEX1.HPP
	DESC : 비트집합 클래스 구현 (정적, 크기제한, 속도빠름)
	INIT BUILT DATE : 2004. 06. 06
	LAST BUILT DATE : 2008. 05. 20

	Copyrightⓒ 2002~2009 B4nFter (b4nfter@gmail.com) and Co-Writers. All Rights Reserved.

	P.S.>
		- 일반 비트연산과 비트 플래그 연산을 동시에 지원하도록 디자인 됨 (메서드 구분은 클래스 선언 참조)
		- TTYPE 타입만큼의 제한된 크기를 가지는 대신 연산이 단순화되어 속도 빠름

*///===============================================================


#pragma once


///////////////////////////////////////////////////////////////////
//	INCLUDE
///////////////////////////////////////////////////////////////////

#include "COMMONMACROS.H"
#include "BASESET.H"
#include "DEBUGUTIL.H"
#include "SAFESTRINGMACROS.H"


///////////////////////////////////////////////////////////////////
//	DEFINE
///////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////
//	DECLARATION
///////////////////////////////////////////////////////////////////

//*---------------------------------------------------------------
// TYPE : CLASS
// NAME : CBitSetEx1
//*---------------------------------------------------------------
// DESC : 비트집합 클래스 (정적, 크기제한, 속도빠름)
// PRGM : B4nFter
//*---------------------------------------------------------------
template<INT TMAX, class TTYPE>
class CBitSetEx1
{
public:
	typedef TTYPE TP_TYPE;

public:
	explicit CBitSetEx1(TTYPE pBitSet = static_cast<TTYPE>(0));

	// 비트 연산 메서드
	BOOL Get(ULONG pBitPos);
	BOOL IsSet() const;
	VOID Set(ULONG pBitPos, BOOL pBitVal);
	VOID SetAll(BOOL pBitVal);
	UINT GetMax() const { return TMAX; }
	UINT GetBitSize() const { return sizeof(TP_TYPE)*CHAR_BIT; }

	// 플래그 연산 메서드 (플로그의 시작값이 1부터 이어야 함)
	BOOL IsSet(TTYPE pBitSet) const;
	BOOL IsReset(TTYPE pBitSet) const;
	BOOL IsSetOther(TTYPE pBitSet) const;	// 지정된 플래그 외의 플래그 들의 SET 여부
	VOID Set(TTYPE pBitSet);
	VOID Set();
	VOID Reset(TTYPE pBitSet);
	VOID Reset();
	TP_TYPE GetRaw() const;

	BOOL operator[](ULONG pBitPos);

private:
	TTYPE m_BitSet;
};


///////////////////////////////////////////////////////////////////
//	IMPLEMENTATION
///////////////////////////////////////////////////////////////////

//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CBitSetEx1::CBitSetEx1
//*---------------------------------------------------------------
// DESC : 생성자 (인자필요)
// PARM :	1 . init size
//			2 . init value
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
template<INT TMAX, class TTYPE>
inline CBitSetEx1<TMAX, TTYPE>::CBitSetEx1(TTYPE pBitSet) : m_BitSet(pBitSet)
{
	DN_ASSERT(!CDebugUtil::IsOverflow(static_cast<TTYPE>(0), static_cast<TTYPE>(TMAX)),	"Overflow!");
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CBitSetEx1::Get
//*---------------------------------------------------------------
// DESC : 특정 위치의 비트의 값을 얻음
// PARM : N/A
// RETV : 특정 위치의 비트의 값 (0/1)
// PRGM : B4nFter
//*---------------------------------------------------------------
template<INT TMAX, class TTYPE>
inline BOOL CBitSetEx1<TMAX, TTYPE>::Get(ULONG pBitPos)
{
	if (!CHECK_LIMIT(pBitPos, GetBitSize())) {
		DN_RETURN(FALSE);
	}

	return (m_BitSet >> pBitPos) & 1;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CBitSetEx1::IsSet
//*---------------------------------------------------------------
// DESC : 비트집합의 값이 0 이 아닌지 체크
// PARM : N/A
// RETV : TRUE - 성공 / FALSE - 실패
// PRGM : B4nFter
//*---------------------------------------------------------------
template<INT TMAX, class TTYPE>
inline BOOL CBitSetEx1<TMAX, TTYPE>::IsSet() const
{
	return (0 != m_BitSet);
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CBitSetEx1::IsSet
//*---------------------------------------------------------------
// DESC : 비트집합의 특정 비트가 0 이 아닌지 체크
// PARM :	1 . pBitSet - 특정 값
// RETV : TRUE - 성공 / FALSE - 실패
// PRGM : B4nFter
//*---------------------------------------------------------------
template<INT TMAX, class TTYPE>
inline BOOL CBitSetEx1<TMAX, TTYPE>::IsSet(TTYPE pBitSet) const
{
	DN_ASSERT((0 != pBitSet) && (pBitSet <= TMAX),	"Invalid!");
	return (0 != (pBitSet & m_BitSet));
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CBitSetEx1::IsReset
//*---------------------------------------------------------------
// DESC : 비트집합의 특정 비트가 0 인지 체크
// PARM :	1 . pBitSet - 특정 값
// RETV : TRUE - 성공 / FALSE - 실패
// PRGM : B4nFter
//*---------------------------------------------------------------
template<INT TMAX, class TTYPE>
inline BOOL CBitSetEx1<TMAX, TTYPE>::IsReset(TTYPE pBitSet) const
{
	DN_ASSERT((0 != pBitSet) && (pBitSet <= TMAX),	"Invalid!");
	return (0 == (pBitSet & m_BitSet));
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CBitSetEx1::IsSetOther
//*---------------------------------------------------------------
// DESC : 비트집합의 특정 비트를 제외한 나머지 값이 0 이 아닌지 체크
// PARM :	1 . pBitSet - 특정 값
// RETV : TRUE - 성공 / FALSE - 실패
// PRGM : B4nFter
//*---------------------------------------------------------------
template<INT TMAX, class TTYPE>
inline BOOL CBitSetEx1<TMAX, TTYPE>::IsSetOther(TTYPE pBitSet) const
{
	DN_ASSERT((0 != pBitSet) && (pBitSet <= TMAX),	"Invalid!");
	return (0 != ((~pBitSet) & m_BitSet));
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CBitSetEx1::Set
//*---------------------------------------------------------------
// DESC : 비트집합 전체를 특정 비트로 세팅
// PARM :	1 . pBitSet - 특정 값
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
template<INT TMAX, class TTYPE>
inline VOID CBitSetEx1<TMAX, TTYPE>::Set(TTYPE pBitSet)
{
	DN_ASSERT((0 != pBitSet) && (pBitSet <= TMAX),	"Invalid!");
	m_BitSet |= (pBitSet);
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CBitSetEx1::Set
//*---------------------------------------------------------------
// DESC : 비트집합 전체를 1 로 세팅
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
template<INT TMAX, class TTYPE>
inline VOID CBitSetEx1<TMAX, TTYPE>::Set()
{
	SetAll(1);
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CBitSetEx1::Set
//*---------------------------------------------------------------
// DESC : 특정 위치의 비트를 특정 비트로 지정
// PARM :	1 . pBitPos - 비트 위치
//			2 . pBitVal - 지정할 비트값
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
template<INT TMAX, class TTYPE>
inline VOID CBitSetEx1<TMAX, TTYPE>::Set(ULONG pBitPos, BOOL pBitVal)
{
	if (!CHECK_LIMIT(pBitPos, GetBitSize())) {
		DN_ASSERT(0,	"Invalid!");
		return;
	}

	if (pBitVal) {
		m_BitSet |= (static_cast<TTYPE>(1) << pBitPos);
	}
	else {
		m_BitSet &= (~(static_cast<TTYPE>(1) << pBitPos));
	}
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CBitSetEx1::SetAll
//*---------------------------------------------------------------
// DESC : 비트집합 전체를 특정 비트로 세팅
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
template<INT TMAX, class TTYPE>
inline VOID CBitSetEx1<TMAX, TTYPE>::SetAll(BOOL pBitVal)
{
	if (pBitVal) {
		::memset(&m_BitSet, 0xFF, sizeof(m_BitSet));
	}
	else {
		::memset(&m_BitSet, 0x00, sizeof(m_BitSet));
	}
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CBitSetEx1::Reset
//*---------------------------------------------------------------
// DESC : 비트집합을 초기화
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
template<INT TMAX, class TTYPE>
VOID CBitSetEx1<TMAX, TTYPE>::Reset()
{
	m_BitSet = static_cast<TTYPE>(0);
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CBitSetEx1::Reset
//*---------------------------------------------------------------
// DESC : 비트집합을 특정 비트집합의 값과 반대로 세팅
// PARM : 
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
template<INT TMAX, class TTYPE>
VOID CBitSetEx1<TMAX, TTYPE>::Reset(TTYPE pBitSet)
{
	DN_ASSERT((0 != pBitSet) && (pBitSet <= TMAX),	"Invalid!");
	m_BitSet &= ~(pBitSet);
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CBitSetEx1::GetRaw
//*---------------------------------------------------------------
// DESC : 비트집합의 실제 값을 얻음
// PARM : N/A
// RETV : TTYPE 타입의 실제 값
// PRGM : B4nFter
//*---------------------------------------------------------------
template<INT TMAX, class TTYPE>
typename CBitSetEx1<TMAX, TTYPE>::TP_TYPE CBitSetEx1<TMAX, TTYPE>::GetRaw() const
{
	return m_BitSet;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CBitSetEx1::operator[]
//*---------------------------------------------------------------
// DESC : 특정 비트의 값을 얻음
// PARM : N/A
// RETV : 특정 비트의 값
// PRGM : B4nFter
//*---------------------------------------------------------------
template<INT TMAX, class TTYPE>
inline BOOL CBitSetEx1<TMAX, TTYPE>::operator[](ULONG pBitPos)
{
	return Get(pBitPos);
}

