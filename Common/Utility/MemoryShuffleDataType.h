/*//===============================================================

	<< MEMORY-SHUFFLE DATA TYPE >>

	PRGM : B4nFter

	FILE : MEMORYSHUFFLEDATATYPE.H, MEMORYSHUFFLEDATATYPE.INL
	DESC : 
	INIT BUILT DATE : 2009. 09. 12
	LAST BUILT DATE : 2010. 07. 21

	P.S.>
		- ��� ���� ���� ������ ����Ǿ��ִ� ������� ���ǻ����� �ݵ�� �����ؾ� ��

		- ���ǻ��׿� �Ұ��Ǿ��ִ� �⺻ ������ Ÿ�� �� �������� �ʴ� ����鿡 ���ؼ� ������ ������ ��� �߻����� �ʵ��� ���ø� ����ȭ �Ǿ������� ���Ƿ� ������ ������ ��� ���� ��� �� ����� #include �ϱ� �� ��ܿ� �Ʒ��� #define �� ������ ��

			SW_MSDT_NOTINCLUDE_COMPILESAFETY

		- C++ ����� �� 'export' �� �����ϴ� �����Ϸ� ��� �� .h �� .inl �� .h �� .cpp �� ���·� �и��ϴ� �۾� �ʿ� (���� : http://www.comeaucomputing.com/4.0/docs/userman/export.html)

		- �������� �ʴ� ������ Ÿ�� (2009.11.23> ������, ����) ��� �� ������ ���� �߻� (�ǵ�����)

		- ���� C++ ����� 'export' ���� �� �� ����� �������濡 ����� ��ũ ���� 'error LNK2001' ���� ��� �߰� �ʿ�

		- ���ܻ�Ȳ �߻� �� ����üũ�� ���� �Ʒ��� ���� ��ɵ��� �����ϴ� ������ ���� ��� �� ����� #include �ϱ� �� ��ܿ� �Ʒ��� #define �� ������ ��

			SW_MSDT_ERRORCHECK_ORIGINVALUE
				: TDATA ���� ���� ������ �����ϸ� ��� ����, ���� �� �����ϰ� ����
				: ���� ������ ���� ������ ��ȯ�Ͽ� ����, ���� ������ ���� ������ ��

		- ���ܻ�Ȳ �߻� �� assert() �� ȣ���ϰ��� �Ѵٸ� �� ����� #include �ϱ� �� ��ܿ� �Ʒ��� #define �� ������ ��

			SW_MSDT_ASSERT

		- �Ʒ��� #define �� MSDT �� ���� ������ Ȯ���� �������� ���Ǹ� ��κ��� ���� �ʿ� ���� ���ȼ��� ���߹Ƿ� ���� ���� �ÿ��� �ݵ�� ����, ��Ȱ��ȭ �Ǿ�� ��

			SW_MSDT_TEST

	Copyright�� 2002~2010 B4nFter (b4nfter@gmail.com) and Co-Writers. All Rights Reserved.

*///===============================================================


#pragma once


///////////////////////////////////////////////////////////////////
//	INCLUDE
///////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////
//	DEFINE
///////////////////////////////////////////////////////////////////

enum EF_MSDT_ALLOCTICKTERM						// �޸� �Ҵ� ����
{
	EV_MSDT_ALTT_VERYFAST		= (1*1000),		// �ص��� ������ �䱸�Ǵ� ���
	EV_MSDT_ALTT_FAST			= (5*1000),		// ������ �켱�Ǵ� ���
	EV_MSDT_ALTT_MEDIUM			= (10*1000),	// �Ϲ����� ���
	EV_MSDT_ALTT_SLOW			= (30*1000),	// ������ �켱�Ǵ� ���
	EV_MSDT_ALTT_VERYSLOW		= (60*1000),	// �ص��� ������ �䱸�Ǵ� ���
};

enum EF_MSDT_EXCEPTIONCODE						// ���� �ڵ� (���� : http://msdn.microsoft.com/en-us/library/het71c37.aspx)
{
	EV_MSDT_ECCD_NONE			= 0xE0000000,	// N/A
	EV_MSDT_ECCD_ALLOCFAIL		= 0xE0010000,	// �޸� �Ҵ� ����
};

#if defined(PRE_MEMORY_SHUFFLE)
#define MSDT_DECL(pTYPE)			CMemoryShuffleDataType<pTYPE>	// USING MSDT
#define MSDT_SETTERM(pMSDT,pTIME)	pMSDT.SetAllocTickTerm(pTIME)	// USING MSDT
#else	// _DEBUG
#define MSDT_DECL(pTYPE)			pTYPE							// NOT-USING MSDT
#define MSDT_SETTERM(pMSDT,pTIME)									// NOT-USING MSDT
#endif	// _DEBUG

#if defined(SW_MSDT_ASSERT)
#define MSDT_ASSERT(pCOND)		if (!(pCOND)) { DebugBreak(); }
#define MSDT_RETURN(pRETV)		DebugBreak(); return(pRETV);
#define MSDT_RETURN_NONE		DebugBreak(); return;
#else	// #if defined(SW_MSDT_ASSERT)
#define MSDT_ASSERT(pCOND)		
#define MSDT_RETURN(pRETV)		return(pRETV);
#define MSDT_RETURN_NONE		return;
#endif	// #if defined(SW_MSDT_ASSERT)


///////////////////////////////////////////////////////////////////
//	FORWARD DECLARATION
///////////////////////////////////////////////////////////////////

template<typename TDATA>
class CMemoryShuffleBaseUnit;

template<typename TDATA>
class CMemoryShuffleAllocator;

template<typename TDATA>
class CMemoryShuffleDataType;


///////////////////////////////////////////////////////////////////
//	DECLARATION
///////////////////////////////////////////////////////////////////

//*---------------------------------------------------------------
// TYPE : CLASS
// NAME : CMemoryShuffleBaseUnit
//*---------------------------------------------------------------
// DESC : �޸� ���� �⺻���� Ŭ���� (�⺻)
// PRGM : B4nFter
//*---------------------------------------------------------------
template<typename TDATA>
class CMemoryShuffleBaseUnit
{

public:
	CMemoryShuffleBaseUnit();
	virtual ~CMemoryShuffleBaseUnit();

	virtual TDATA GetValue() const = 0;
	virtual void SetValue(TDATA pData) = 0;

	unsigned long GetLastTick() const;
	void SetLastTick(unsigned long pLastTick);
		
private:
	mutable unsigned long m_LastTick;

};


//*---------------------------------------------------------------
// TYPE : CLASS
// NAME : CMemoryShuffleAllocator
//*---------------------------------------------------------------
// DESC : �޸� ������ ���� �޸� ������ ����ϴ� �Ҵ��� Ŭ����
// PRGM : B4nFter
//*---------------------------------------------------------------
template<typename TDATA>
class CMemoryShuffleAllocator
{

private:
	mutable CMemoryShuffleBaseUnit<TDATA>* m_Unit;

public:
	friend class CMemoryShuffleDataType<TDATA>;

public:
	CMemoryShuffleAllocator();
	virtual ~CMemoryShuffleAllocator();

	bool IsOpen() const;

	bool Shuffle(CMemoryShuffleBaseUnit<TDATA>*& pUnit) const;

 	CMemoryShuffleBaseUnit<TDATA>* Lock();
 	void Unlock(const CMemoryShuffleBaseUnit<TDATA>* pUnit) const;

	unsigned long GetTickTerm() const;
	void SetTickTerm(unsigned long pTickTerm = EV_MSDT_ALTT_FAST);

#if defined(SW_MSDT_ERRORCHECK_ORIGINVALUE)
	const TDATA GetOriginalValue() const { return m_OriginalValue; }
#endif	// SW_MSDT_ERRORCHECK_ORIGINVALUE

private:
	bool Create();
	bool Create(const TDATA& pData);
	bool Create(const CMemoryShuffleDataType<TDATA>& pMsdt);

	void Release();

private:
	mutable unsigned long m_TickTerm;

public:
	volatile mutable long m_Lock;

	// ���� üũ�� ����
#if defined(SW_MSDT_ERRORCHECK_ORIGINVALUE)
	TDATA m_OriginalValue;
#endif	// SW_MSDT_ERRORCHECK_ORIGINVALUE

};


//*---------------------------------------------------------------
// TYPE : CLASS
// NAME : CMemoryShuffleSyncAuto
//*---------------------------------------------------------------
// DESC : �޸� ���� ������ Ÿ�� ���� �� ���⸦ �������ִ� Ŭ����
// PRGM : B4nFter
//*---------------------------------------------------------------
template<typename TALLOC, typename TUNIT>
class CMemoryShuffleSyncAuto
{
//	DISABLE_COPY_ASSIGN(CMemoryShuffleSyncAuto)	// ��ü�� ����, ���� ����

public:
	CMemoryShuffleSyncAuto(TALLOC* pAlloc);
	~CMemoryShuffleSyncAuto();

	TUNIT* Lock();

private:
	// ��ü�� ����, ���� ����
	CMemoryShuffleSyncAuto(const CMemoryShuffleSyncAuto<TALLOC, TUNIT>& pSyncAuto) : m_Alloc(pSyncAuto.m_Alloc)		{ MSDT_ASSERT(0); /* "Can't be Called!" */ }
#pragma warning (disable:4100)
	CMemoryShuffleSyncAuto& operator=(const CMemoryShuffleSyncAuto<TALLOC, TUNIT>& pSyncAuto)						{ MSDT_ASSERT(0); /* "Can't be Called!" */ return(*this); }
#pragma warning (default:4100)

private:
	TALLOC* m_Alloc;
	TUNIT* m_Unit;

};


//*---------------------------------------------------------------
// TYPE : CLASS
// NAME : CMemoryShuffleDataType
//*---------------------------------------------------------------
// DESC : �޸� ���� ������ Ÿ�� Ŭ����
// PRGM : B4nFter
//*---------------------------------------------------------------
template<typename TDATA>
class CMemoryShuffleDataType
{

public:
	CMemoryShuffleDataType();
	CMemoryShuffleDataType(const TDATA& pData);
	CMemoryShuffleDataType(const CMemoryShuffleDataType<TDATA>& pMsdt);
	virtual ~CMemoryShuffleDataType();

	operator TDATA const ();
// 	operator TDATA* const ();	// not support
// 	operator TDATA& const ();	// not support

	TDATA operator=(const TDATA& pData);
	CMemoryShuffleDataType<TDATA>& operator=(const CMemoryShuffleDataType<TDATA>& pMsdt);

	TDATA operator++();			// ++V (��ġ)
	TDATA operator++(int);		// V++ (��ġ)
	TDATA operator--();			// --V (��ġ)
	TDATA operator--(int);		// V-- (��ġ)

	TDATA operator+=(const TDATA& pData);
	TDATA operator-=(const TDATA& pData);
	TDATA operator*=(const TDATA& pData);
	TDATA operator/=(const TDATA& pData);
	TDATA operator%=(const TDATA& pData);
	TDATA operator&=(const TDATA& pData);
	TDATA operator|=(const TDATA& pData);
	TDATA operator^=(const TDATA& pData);

	unsigned long GetAllocTickTerm() const;
	void SetAllocTickTerm(unsigned long pTickTerm);

#if defined(SW_MSDT_TEST)
	__forceinline const CMemoryShuffleBaseUnit<TDATA>* GetPtr() const { return(m_Allocator.m_Unit); }		// �׽�Ʈ�� ���� �ӽ� �޼��� !!! - ���� ��� �ÿ� �ݵ�� ���� ���
#endif	// SW_MSDT_TEST

private:
	typedef	CMemoryShuffleSyncAuto<CMemoryShuffleAllocator<TDATA>, CMemoryShuffleBaseUnit<TDATA>>	TP_SYNCAUTO;

	CMemoryShuffleAllocator<TDATA>& GetAllocator();

private:
	CMemoryShuffleAllocator<TDATA> m_Allocator;

};


//*---------------------------------------------------------------
// TYPE : CLASS
// NAME : CMemoryShuffleDataType
//*---------------------------------------------------------------
// DESC : �޸� ���� ������ Ÿ�� Ŭ���� (������ ��� ����)
// PRGM : B4nFter
//*---------------------------------------------------------------
template<typename TDATA>
class CMemoryShuffleDataType<TDATA*>
{

public:
	CMemoryShuffleDataType() {
		char aCompileError[-1];		// ������ ���� ���� ����
	}
};


//*---------------------------------------------------------------
// TYPE : CLASS
// NAME : CMemoryShuffleDataType
//*---------------------------------------------------------------
// DESC : �޸� ���� ������ Ÿ�� Ŭ���� (������ ��� ����)
// PRGM : B4nFter
//*---------------------------------------------------------------
template<typename TDATA>
class CMemoryShuffleDataType<const TDATA*>
{

public:
	CMemoryShuffleDataType() {
		char aCompileError[-1];		// ������ ���� ���� ����
	}
};


//*---------------------------------------------------------------
// TYPE : CLASS
// NAME : CMemoryShuffleDataType
//*---------------------------------------------------------------
// DESC : �޸� ���� ������ Ÿ�� Ŭ���� (���� ��� ����)
// PRGM : B4nFter
//*---------------------------------------------------------------
template<typename TDATA>
class CMemoryShuffleDataType<TDATA&>
{

public:
	CMemoryShuffleDataType() {
		char aCompileError[-1];		// ������ ���� ���� ����
	}
};


//*---------------------------------------------------------------
// TYPE : CLASS
// NAME : CMemoryShuffleDataType
//*---------------------------------------------------------------
// DESC : �޸� ���� ������ Ÿ�� Ŭ���� (���� ��� ����)
// PRGM : B4nFter
//*---------------------------------------------------------------
template<typename TDATA>
class CMemoryShuffleDataType<const TDATA&>
{

public:
	CMemoryShuffleDataType() {
		char aCompileError[-1];		// ������ ���� ���� ����
	}
};


#include "MEMORYSHUFFLEDATATYPE.INL"	// P.S.> 'export' ���� �����Ǵ� �����Ϸ����� ����

