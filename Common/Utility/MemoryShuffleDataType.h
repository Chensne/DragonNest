/*//===============================================================

	<< MEMORY-SHUFFLE DATA TYPE >>

	PRGM : B4nFter

	FILE : MEMORYSHUFFLEDATATYPE.H, MEMORYSHUFFLEDATATYPE.INL
	DESC : 
	INIT BUILT DATE : 2009. 09. 12
	LAST BUILT DATE : 2010. 07. 21

	P.S.>
		- 사용 전에 별도 문서로 설명되어있는 사용방법과 주의사항을 반드시 숙지해야 함

		- 주의사항에 소개되어있는 기본 데이터 타입 별 지원하지 않는 연산들에 대해서 컴파일 에러나 경고가 발생하지 않도록 템플릿 전문화 되어있으나 고의로 컴파일 에러나 경고를 원할 경우 본 헤더를 #include 하기 전 상단에 아래의 #define 을 선언할 것

			SW_MSDT_NOTINCLUDE_COMPILESAFETY

		- C++ 예약어 중 'export' 를 지원하는 컴파일러 사용 시 .h 와 .inl 을 .h 와 .cpp 의 형태로 분리하는 작업 필요 (참고 : http://www.comeaucomputing.com/4.0/docs/userman/export.html)

		- 지원하지 않는 데이터 타입 (2009.11.23> 포인터, 참조) 사용 시 컴파일 에러 발생 (의도적임)

		- 추후 C++ 예약어 'export' 지원 후 본 모듈의 구조변경에 대비한 링크 에러 'error LNK2001' 유발 기능 추가 필요

		- 예외상황 발생 시 오류체크를 위해 아래와 같은 기능들을 제공하니 지원을 원할 경우 본 헤더를 #include 하기 전 상단에 아래의 #define 을 선언할 것

			SW_MSDT_ERRORCHECK_ORIGINVALUE
				: TDATA 형의 복제 변수를 유지하며 모든 참조, 연산 시 동일하게 적용
				: 셔플 변수를 실제 값으로 변환하여 참조, 연산 직전에 복제 변수와 비교

		- 예외상황 발생 시 assert() 를 호출하고자 한다면 본 헤더를 #include 하기 전 상단에 아래의 #define 을 선언할 것

			SW_MSDT_ASSERT

		- 아래의 #define 은 MSDT 의 내부 동작을 확인할 목적으로 사용되며 대부분의 경우는 필요 없고 보안성을 낮추므로 실제 적용 시에는 반드시 제거, 비활성화 되어야 함

			SW_MSDT_TEST

	Copyrightⓒ 2002~2010 B4nFter (b4nfter@gmail.com) and Co-Writers. All Rights Reserved.

*///===============================================================


#pragma once


///////////////////////////////////////////////////////////////////
//	INCLUDE
///////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////
//	DEFINE
///////////////////////////////////////////////////////////////////

enum EF_MSDT_ALLOCTICKTERM						// 메모리 할당 간격
{
	EV_MSDT_ALTT_VERYFAST		= (1*1000),		// 극도의 보안이 요구되는 경우
	EV_MSDT_ALTT_FAST			= (5*1000),		// 보안이 우선되는 경우
	EV_MSDT_ALTT_MEDIUM			= (10*1000),	// 일반적인 경우
	EV_MSDT_ALTT_SLOW			= (30*1000),	// 성능이 우선되는 경우
	EV_MSDT_ALTT_VERYSLOW		= (60*1000),	// 극도의 성능이 요구되는 경우
};

enum EF_MSDT_EXCEPTIONCODE						// 예외 코드 (참고 : http://msdn.microsoft.com/en-us/library/het71c37.aspx)
{
	EV_MSDT_ECCD_NONE			= 0xE0000000,	// N/A
	EV_MSDT_ECCD_ALLOCFAIL		= 0xE0010000,	// 메모리 할당 실패
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
// DESC : 메모리 셔플 기본단위 클래스 (기본)
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
// DESC : 메모리 셔플의 동적 메모리 관리를 담당하는 할당자 클래스
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

	// 오류 체크용 변수
#if defined(SW_MSDT_ERRORCHECK_ORIGINVALUE)
	TDATA m_OriginalValue;
#endif	// SW_MSDT_ERRORCHECK_ORIGINVALUE

};


//*---------------------------------------------------------------
// TYPE : CLASS
// NAME : CMemoryShuffleSyncAuto
//*---------------------------------------------------------------
// DESC : 메모리 셔플 데이터 타입 연산 시 동기를 보장해주는 클래스
// PRGM : B4nFter
//*---------------------------------------------------------------
template<typename TALLOC, typename TUNIT>
class CMemoryShuffleSyncAuto
{
//	DISABLE_COPY_ASSIGN(CMemoryShuffleSyncAuto)	// 객체간 복사, 대입 금지

public:
	CMemoryShuffleSyncAuto(TALLOC* pAlloc);
	~CMemoryShuffleSyncAuto();

	TUNIT* Lock();

private:
	// 객체간 복사, 대입 금지
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
// DESC : 메모리 셔플 데이터 타입 클래스
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

	TDATA operator++();			// ++V (선치)
	TDATA operator++(int);		// V++ (후치)
	TDATA operator--();			// --V (선치)
	TDATA operator--(int);		// V-- (후치)

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
	__forceinline const CMemoryShuffleBaseUnit<TDATA>* GetPtr() const { return(m_Allocator.m_Unit); }		// 테스트를 위한 임시 메서드 !!! - 실제 사용 시엔 반드시 제거 요망
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
// DESC : 메모리 셔플 데이터 타입 클래스 (포인터 사용 금지)
// PRGM : B4nFter
//*---------------------------------------------------------------
template<typename TDATA>
class CMemoryShuffleDataType<TDATA*>
{

public:
	CMemoryShuffleDataType() {
		char aCompileError[-1];		// 컴파일 시점 에러 유발
	}
};


//*---------------------------------------------------------------
// TYPE : CLASS
// NAME : CMemoryShuffleDataType
//*---------------------------------------------------------------
// DESC : 메모리 셔플 데이터 타입 클래스 (포인터 사용 금지)
// PRGM : B4nFter
//*---------------------------------------------------------------
template<typename TDATA>
class CMemoryShuffleDataType<const TDATA*>
{

public:
	CMemoryShuffleDataType() {
		char aCompileError[-1];		// 컴파일 시점 에러 유발
	}
};


//*---------------------------------------------------------------
// TYPE : CLASS
// NAME : CMemoryShuffleDataType
//*---------------------------------------------------------------
// DESC : 메모리 셔플 데이터 타입 클래스 (참조 사용 금지)
// PRGM : B4nFter
//*---------------------------------------------------------------
template<typename TDATA>
class CMemoryShuffleDataType<TDATA&>
{

public:
	CMemoryShuffleDataType() {
		char aCompileError[-1];		// 컴파일 시점 에러 유발
	}
};


//*---------------------------------------------------------------
// TYPE : CLASS
// NAME : CMemoryShuffleDataType
//*---------------------------------------------------------------
// DESC : 메모리 셔플 데이터 타입 클래스 (참조 사용 금지)
// PRGM : B4nFter
//*---------------------------------------------------------------
template<typename TDATA>
class CMemoryShuffleDataType<const TDATA&>
{

public:
	CMemoryShuffleDataType() {
		char aCompileError[-1];		// 컴파일 시점 에러 유발
	}
};


#include "MEMORYSHUFFLEDATATYPE.INL"	// P.S.> 'export' 예약어가 지원되는 컴파일러에서 제거

