/*//===============================================================

	<< BIT SET EX (1) >>

	PRGM : B4nFter

	FILE : BITSETEX1.HPP
	DESC : ��Ʈ���� Ŭ���� ���� (����, ũ������, �ӵ�����)
	INIT BUILT DATE : 2004. 06. 06
	LAST BUILT DATE : 2008. 05. 20

	Copyright�� 2002~2009 B4nFter (b4nfter@gmail.com) and Co-Writers. All Rights Reserved.

	P.S.>
		- �Ϲ� ��Ʈ����� ��Ʈ �÷��� ������ ���ÿ� �����ϵ��� ������ �� (�޼��� ������ Ŭ���� ���� ����)
		- TTYPE Ÿ�Ը�ŭ�� ���ѵ� ũ�⸦ ������ ��� ������ �ܼ�ȭ�Ǿ� �ӵ� ����

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
// DESC : ��Ʈ���� Ŭ���� (����, ũ������, �ӵ�����)
// PRGM : B4nFter
//*---------------------------------------------------------------
template<INT TMAX, class TTYPE>
class CBitSetEx1
{
public:
	typedef TTYPE TP_TYPE;

public:
	explicit CBitSetEx1(TTYPE pBitSet = static_cast<TTYPE>(0));

	// ��Ʈ ���� �޼���
	BOOL Get(ULONG pBitPos);
	BOOL IsSet() const;
	VOID Set(ULONG pBitPos, BOOL pBitVal);
	VOID SetAll(BOOL pBitVal);
	UINT GetMax() const { return TMAX; }
	UINT GetBitSize() const { return sizeof(TP_TYPE)*CHAR_BIT; }

	// �÷��� ���� �޼��� (�÷α��� ���۰��� 1���� �̾�� ��)
	BOOL IsSet(TTYPE pBitSet) const;
	BOOL IsReset(TTYPE pBitSet) const;
	BOOL IsSetOther(TTYPE pBitSet) const;	// ������ �÷��� ���� �÷��� ���� SET ����
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
// DESC : ������ (�����ʿ�)
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
// DESC : Ư�� ��ġ�� ��Ʈ�� ���� ����
// PARM : N/A
// RETV : Ư�� ��ġ�� ��Ʈ�� �� (0/1)
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
// DESC : ��Ʈ������ ���� 0 �� �ƴ��� üũ
// PARM : N/A
// RETV : TRUE - ���� / FALSE - ����
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
// DESC : ��Ʈ������ Ư�� ��Ʈ�� 0 �� �ƴ��� üũ
// PARM :	1 . pBitSet - Ư�� ��
// RETV : TRUE - ���� / FALSE - ����
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
// DESC : ��Ʈ������ Ư�� ��Ʈ�� 0 ���� üũ
// PARM :	1 . pBitSet - Ư�� ��
// RETV : TRUE - ���� / FALSE - ����
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
// DESC : ��Ʈ������ Ư�� ��Ʈ�� ������ ������ ���� 0 �� �ƴ��� üũ
// PARM :	1 . pBitSet - Ư�� ��
// RETV : TRUE - ���� / FALSE - ����
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
// DESC : ��Ʈ���� ��ü�� Ư�� ��Ʈ�� ����
// PARM :	1 . pBitSet - Ư�� ��
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
// DESC : ��Ʈ���� ��ü�� 1 �� ����
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
// DESC : Ư�� ��ġ�� ��Ʈ�� Ư�� ��Ʈ�� ����
// PARM :	1 . pBitPos - ��Ʈ ��ġ
//			2 . pBitVal - ������ ��Ʈ��
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
// DESC : ��Ʈ���� ��ü�� Ư�� ��Ʈ�� ����
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
// DESC : ��Ʈ������ �ʱ�ȭ
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
// DESC : ��Ʈ������ Ư�� ��Ʈ������ ���� �ݴ�� ����
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
// DESC : ��Ʈ������ ���� ���� ����
// PARM : N/A
// RETV : TTYPE Ÿ���� ���� ��
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
// DESC : Ư�� ��Ʈ�� ���� ����
// PARM : N/A
// RETV : Ư�� ��Ʈ�� ��
// PRGM : B4nFter
//*---------------------------------------------------------------
template<INT TMAX, class TTYPE>
inline BOOL CBitSetEx1<TMAX, TTYPE>::operator[](ULONG pBitPos)
{
	return Get(pBitPos);
}

