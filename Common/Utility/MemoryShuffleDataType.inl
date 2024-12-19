

///////////////////////////////////////////////////////////////////
//	INCLUDE
///////////////////////////////////////////////////////////////////

//#include "MEMORYSHUFFLEDATATYPE.H"	// P.S.> 'export' 예약어가 지원되는 컴파일러에서 사용
#include <WINDOWS.H>
#include <BASETSD.H>
#include <NEW>			// placement new


///////////////////////////////////////////////////////////////////
//	DEFINE
///////////////////////////////////////////////////////////////////

#define DF_MSDT_LOCKSIGN		(0x6A035984)	// 동기 체크 상수 (P.S.> 절대 0 으로 지정해선 않됨)	
#define FIX_VIOLATION_BY_FREE // new 로 생성한 객체를 free() 로 제거하면서 생기는 메모리영역 침범 수정.

#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }

///////////////////////////////////////////////////////////////////
//	DECLARATION
///////////////////////////////////////////////////////////////////

//*---------------------------------------------------------------
// TYPE : CLASS
// NAME : CMemoryShuffleDervUnit
//*---------------------------------------------------------------
// DESC : 메모리 셔플 기본단위 클래스 (파생)
// PRGM : B4nFter
//*---------------------------------------------------------------
template<typename TDATA>
class CMemoryShuffleDervUnit : public CMemoryShuffleBaseUnit<TDATA>
{

private:
	mutable char m_Data[sizeof(TDATA)];

public:
	CMemoryShuffleDervUnit();
	virtual ~CMemoryShuffleDervUnit();

	TDATA GetValue() const;
	void SetValue(TDATA pData);

private:
	void GetXor(char const * pIn, char* pOut, unsigned int pSize, unsigned int pKey) const;
	
};


///////////////////////////////////////////////////////////////////
//	IMPLEMENTATION
///////////////////////////////////////////////////////////////////

//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleBaseUnit::CMemoryShuffleBaseUnit
//*---------------------------------------------------------------
// DESC : 생성자
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
/* export */ template<typename TDATA>
inline CMemoryShuffleBaseUnit<TDATA>::CMemoryShuffleBaseUnit() : m_LastTick(0)
{
	
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleBaseUnit::~CMemoryShuffleBaseUnit
//*---------------------------------------------------------------
// DESC : 소멸자
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
/* export */ template<typename TDATA>
inline CMemoryShuffleBaseUnit<TDATA>::~CMemoryShuffleBaseUnit()
{
	
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleBaseUnit::GetLastTick
//*---------------------------------------------------------------
// DESC : 동적 메모리가 마지막 셔플링된 시점을 반환
// PARM : N/A
// RETV : 기존 셔플링 시점
// PRGM : B4nFter
//*---------------------------------------------------------------
/* export */ template<typename TDATA>
inline unsigned long CMemoryShuffleBaseUnit<TDATA>::GetLastTick() const
{
	return m_LastTick;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleBaseUnit::SetLastTick
//*---------------------------------------------------------------
// DESC : 동적 메모리가 마지막 셔플링된 시점을 지정
// PARM :	1 . pLastTick - 신규 셔플링 시점
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
/* export */ template<typename TDATA>
inline void CMemoryShuffleBaseUnit<TDATA>::SetLastTick(unsigned long pLastTick)
{
	m_LastTick = pLastTick;
}


//================================================================

//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDervUnit::CMemoryShuffleDervUnit
//*---------------------------------------------------------------
// DESC : 생성자
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
/* export */ template<typename TDATA>
inline CMemoryShuffleDervUnit<TDATA>::CMemoryShuffleDervUnit()
{
	::memset(m_Data, 0, sizeof(m_Data));
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDervUnit::~CMemoryShuffleDervUnit
//*---------------------------------------------------------------
// DESC : 소멸자
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
/* export */ template<typename TDATA>
inline CMemoryShuffleDervUnit<TDATA>::~CMemoryShuffleDervUnit()
{
	
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDervUnit::GetValue
//*---------------------------------------------------------------
// DESC : 기존 보유값을 반환
// PARM : N/A
// RETV : 기존 보유값
// PRGM : B4nFter
//*---------------------------------------------------------------
/* export */ template<typename TDATA>
inline TDATA CMemoryShuffleDervUnit<TDATA>::GetValue() const
{
	TDATA aData;

	GetXor(m_Data, reinterpret_cast<char*>(&aData), sizeof(TDATA), PtrToUint(this));

	return aData;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDervUnit::SetValue
//*---------------------------------------------------------------
// DESC : 신규 대입값을 지정
// PARM :	1 . pData - 신규 대입값
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
/* export */ template<typename TDATA>
inline void CMemoryShuffleDervUnit<TDATA>::SetValue(TDATA pData)
{
	GetXor(reinterpret_cast<char*>(&pData), m_Data, sizeof(TDATA), PtrToUint(this));
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDervUnit::GetXor
//*---------------------------------------------------------------
// DESC : 특정값을 변환키를 사용하여 XOR 연산함
// PARM :	1 . pIn - 변환할 특정값을 가진 변수
//			2 . pOut - 변환 결과를 출력할 변수
//			2 . pSize - 특정값의 크기
//			3 . pKey - 변환키
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
/* export */ template<typename TDATA>
inline void CMemoryShuffleDervUnit<TDATA>::GetXor(char const * pIn, char* pOut, unsigned int pSize, unsigned int pKey) const
{
	// P.S.> 추후 필요한 경우에 pKey 를 변환하거나 XOR 방식을 바꾸는 방법도 있을 수 있을 것임

	unsigned int const * aIn = reinterpret_cast<unsigned int const *>(pIn);	// ++ 연산에 l-value 가 필요하므로 임시 변수를 완전히 없앨 수는 없음 (C2105)
	unsigned int* aOut = reinterpret_cast<unsigned int*>(pOut);				// ++ 연산에 l-value 가 필요하므로 임시 변수를 완전히 없앨 수는 없음 (C2105)
	unsigned int aQtSize = pSize >> 2;
	for (; aQtSize ; --aQtSize) {
		*aOut++ = ((*aIn++) ^ pKey);
	}

	pIn = reinterpret_cast<char const *>(aIn);
	pOut = reinterpret_cast<char*>(aOut);
	pSize = (pSize & 0x03);
	for (; pSize ; --pSize) {
		*pOut++ = ((*pIn++) ^ reinterpret_cast<char*>(&pKey)[pSize]);
	}
}


//================================================================

//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleAllocator::CMemoryShuffleAllocator
//*---------------------------------------------------------------
// DESC : 생성자
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
/* export */ template<typename TDATA>
inline CMemoryShuffleAllocator<TDATA>::CMemoryShuffleAllocator() : m_TickTerm(0), m_Unit(NULL), m_Lock(DF_MSDT_LOCKSIGN)
{
	// 오류 체크용 변수
#if defined(SW_MSDT_ERRORCHECK_ORIGINVALUE)
	m_OriginalValue = TDATA();
#endif	// SW_MSDT_ERRORCHECK_ORIGINVALUE
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleAllocator::~CMemoryShuffleAllocator
//*---------------------------------------------------------------
// DESC : 소멸자
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
/* export */ template<typename TDATA>
inline CMemoryShuffleAllocator<TDATA>::~CMemoryShuffleAllocator()
{
	Release();
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleAllocator::IsOpen
//*---------------------------------------------------------------
// DESC : 할당자 사용가능 여부 반환
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
/* export */ template<typename TDATA>
inline bool CMemoryShuffleAllocator<TDATA>::IsOpen() const
{
	return(0 < m_TickTerm);
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleAllocator::Create
//*---------------------------------------------------------------
// DESC : 동적 메모리를 최초로 생성
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
// P.S.>
//		- 본 메서드는 기존에 할당받은 동적 메모리가 없고 동기화가 필요없는 상황에서 호출됨을 전제함
//*---------------------------------------------------------------
/* export */ template<typename TDATA>
inline bool CMemoryShuffleAllocator<TDATA>::Create()
{
	MSDT_ASSERT(NULL == m_Unit);
	MSDT_ASSERT(0 == m_TickTerm);

	char* aUnit = static_cast<char*>(malloc(sizeof(CMemoryShuffleDervUnit<TDATA>)));
	if (!aUnit) {
		m_TickTerm = 0;
		MSDT_RETURN(false);
	}

	m_Unit = new (aUnit) CMemoryShuffleDervUnit<TDATA>;		// placement new 는 std::bad_alloc 을 발생시키지 않으므로 결과적으로 예외를 발생시키지 않음 ?
	MSDT_ASSERT(NULL != m_Unit);

//#if defined(SW_MSDT_ERRORCHECK_ORIGINVALUE)
	m_Unit->SetValue(TDATA());	// 0 초기화 함
//#endif	// SW_MSDT_ERRORCHECK_ORIGINVALUE

	SetTickTerm();

	return true;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleAllocator::Create
//*---------------------------------------------------------------
// DESC : 동적 메모리를 최초로 생성하고 특정 값으로 초기화
// PARM :	1 . pData - 초기화에 사용할 값
// RETV : N/A
// PRGM : B4nFter
// P.S.>
//		- 본 메서드는 기존에 할당받은 동적 메모리가 없고 동기화가 필요없는 상황에서 호출됨을 전제함
//*---------------------------------------------------------------
/* export */ template<typename TDATA>
inline bool CMemoryShuffleAllocator<TDATA>::Create(const TDATA& pData)
{
	if (!Create()) {
		return false;
	}

#if defined(SW_MSDT_ERRORCHECK_ORIGINVALUE)
	m_OriginalValue = pData;
#endif	// SW_MSDT_ERRORCHECK_ORIGINVALUE

	m_Unit->SetValue(pData);

	return true;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleAllocator::Create
//*---------------------------------------------------------------
// DESC : 동적 메모리를 최초로 생성하고 특정 객체로 초기화
// PARM :	1 . pMsdt - 초기화에 사용할 객체
// RETV : N/A
// PRGM : B4nFter
// P.S.>
//		- 본 메서드는 기존에 할당받은 동적 메모리가 없고 동기화가 필요없는 상황에서 호출됨을 전제함
//*---------------------------------------------------------------
/* export */ template<typename TDATA>
inline bool CMemoryShuffleAllocator<TDATA>::Create(const CMemoryShuffleDataType<TDATA>& pMsdt)
{
	if (!Create()) {
		return false;
	}

#if defined(SW_MSDT_ERRORCHECK_ORIGINVALUE)
	m_OriginalValue = static_cast<TDATA>(const_cast<CMemoryShuffleDataType<TDATA>&>(pMsdt));
#endif	// SW_MSDT_ERRORCHECK_ORIGINVALUE

	m_Unit->SetValue(static_cast<TDATA>(const_cast<CMemoryShuffleDataType<TDATA>&>(pMsdt)));

	return true;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleAllocator::Release
//*---------------------------------------------------------------
// DESC : 기존의 동적 메모리를 해제하고 할당자를 사용불가로 지정
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
/* export */ template<typename TDATA>
inline void CMemoryShuffleAllocator<TDATA>::Release()
{
	if (m_Unit) {
//		m_Unit->~TDATA();	// placement new 사용으로 인한 명시적인 소멸자 호출 (소멸자가 없는 TDATA 에 대해 컴파일 오류 발생)
		
#ifdef FIX_VIOLATION_BY_FREE
		SAFE_DELETE( m_Unit );
#else
		free(reinterpret_cast<char*>(m_Unit));
		m_Unit = NULL;
#endif

		
	}
	m_TickTerm = 0;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleAllocator::Shuffle
//*---------------------------------------------------------------
// DESC : 동적 메모리를 지정된 시간 간격에 따라 다른 동적 메모리로 변환
// PARM :	1 . pUnit - 메모리 셔플링을 수행할 기본단위 객체
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
/* export */ template<typename TDATA>
inline bool CMemoryShuffleAllocator<TDATA>::Shuffle(CMemoryShuffleBaseUnit<TDATA>*& pUnit) const
{
	MSDT_ASSERT(NULL != pUnit);

	// P.S.> 본 메서드에 진입한 시기에는 CMemoryShuffleAllocator<TDATA> 객체의 동기가 보장되어야 함

	CMemoryShuffleDervUnit<TDATA>* aUnit1 = static_cast<CMemoryShuffleDervUnit<TDATA>*>(pUnit);
	
	unsigned long aCurTick = ::GetTickCount();	// 해상도가 낮아도 무관 ?
	if (m_TickTerm < ((aCurTick >= aUnit1->GetLastTick())?(aCurTick - aUnit1->GetLastTick()):(ULONG_MAX - aUnit1->GetLastTick() + aCurTick))) {
		TDATA aData = aUnit1->GetValue();

		char* aUnit2 = static_cast<char*>(malloc(sizeof(CMemoryShuffleDervUnit<TDATA>)));	// 메모리 해제 이후에 다시 할당받으면 같은 주소가 될 가능성이 있으므로 이를 방지하기 위해 할당을 우선 받음
		if (!aUnit2) {
			m_TickTerm = 0;
			::RaiseException(EV_MSDT_ECCD_ALLOCFAIL, 0, 0, NULL);	// 할당에 실패하면 즉시 확인하는 것이 좋으므로 예외를 발생시킴
			MSDT_RETURN(false);
		}

//		aUnit1->~TDATA();	// placement new 사용으로 인한 명시적인 소멸자 호출 (소멸자가 없는 TDATA 에 대해 컴파일 오류 발생)
		
#ifdef FIX_VIOLATION_BY_FREE
		SAFE_DELETE( aUnit1 );
#else
		free(reinterpret_cast<char*>(aUnit1));
#endif
		
		aUnit1 = new (aUnit2) CMemoryShuffleDervUnit<TDATA>;	// placement new 는 std::bad_alloc 을 발생시키지 않으므로 결과적으로 예외를 발생시키지 않음 ?
		MSDT_ASSERT(NULL != aUnit1);

		aUnit1->SetValue(aData);
		aUnit1->SetLastTick(aCurTick);

		pUnit = aUnit1;
	}

	return true;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleAllocator::GetTickTerm
//*---------------------------------------------------------------
// DESC : 기존 메모리 셔플링 간격 반환
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
/* export */ template<typename TDATA>
inline unsigned long CMemoryShuffleAllocator<TDATA>::GetTickTerm() const
{
	return m_TickTerm;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleAllocator::SetTickTerm
//*---------------------------------------------------------------
// DESC : 신규 메모리 셔플링 간격 지정
// PARM :	1 . pTickTerm - 메모리 셔플링 간격
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
/* export */ template<typename TDATA>
inline void CMemoryShuffleAllocator<TDATA>::SetTickTerm(unsigned long pTickTerm)
{
	MSDT_ASSERT(0 < pTickTerm);

	m_TickTerm = pTickTerm;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleAllocator::Lock
//*---------------------------------------------------------------
// DESC : 할당자를 잠금상태로 변환
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
/* export */ template<typename TDATA>
inline CMemoryShuffleBaseUnit<TDATA>* CMemoryShuffleAllocator<TDATA>::Lock()
{
	long aLock = 0;

	do {
		aLock = InterlockedExchange(&m_Lock, 0);
	} while(DF_MSDT_LOCKSIGN != aLock);


	CMemoryShuffleDervUnit<TDATA>* aUnit = const_cast<CMemoryShuffleDervUnit<TDATA>*>(static_cast<CMemoryShuffleDervUnit<TDATA>*>(m_Unit));
	MSDT_ASSERT(NULL != aUnit);

	m_Unit = NULL;	// 반드시 지정할 필요는 없으나 보안성을 다소나마 높히기 위함

	return aUnit;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleAllocator::Unlock
//*---------------------------------------------------------------
// DESC : 할당자의 잠금상태를 해제
// PARM :	1 . pUnit - 잠금상태를 해제하는데 사용할 기본단위 객체
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
/* export */ template<typename TDATA>
inline void CMemoryShuffleAllocator<TDATA>::Unlock(const CMemoryShuffleBaseUnit<TDATA>* pUnit) const
{
	MSDT_ASSERT(NULL == m_Unit);				// Lock 시 m_Unit 를 NULL 지정 시 체크
	MSDT_ASSERT(NULL != pUnit);

	if (!pUnit) {
		MSDT_RETURN_NONE;
	}


	m_Unit = const_cast<CMemoryShuffleDervUnit<TDATA>*>(static_cast<const CMemoryShuffleDervUnit<TDATA>*>(pUnit));	// 이전처럼 포인터 교체 자체의 원자성이 보장 (InterlockedExchangePointer) 되는 것이 아니므로 동기 해제 전에 포인터를 교체해야 함

	long aLock = InterlockedExchange(&m_Lock, DF_MSDT_LOCKSIGN);
	MSDT_ASSERT(DF_MSDT_LOCKSIGN != aLock);
}


//================================================================

//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleSyncAuto::CMemoryShuffleSyncAuto
//*---------------------------------------------------------------
// DESC : 생성자
// PARM :	1 . pAlloc - TALLOC 클래스 객체포인터
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
/* export */ template<typename TALLOC, typename TUNIT>
inline CMemoryShuffleSyncAuto<TALLOC, TUNIT>::CMemoryShuffleSyncAuto(TALLOC* pAlloc) : m_Alloc(pAlloc), m_Unit(NULL)
{
	MSDT_ASSERT(NULL != m_Alloc);
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleSyncAuto::~CMemoryShuffleSyncAuto
//*---------------------------------------------------------------
// DESC : 소멸자
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
/* export */ template<typename TALLOC, typename TUNIT>
inline CMemoryShuffleSyncAuto<TALLOC, TUNIT>::~CMemoryShuffleSyncAuto()
{
	if (m_Alloc && m_Unit) {
		m_Alloc->Unlock(m_Unit);
		m_Alloc = NULL;
		m_Unit = NULL;
	}
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleSyncAuto::Lock
//*---------------------------------------------------------------
// DESC : 보유한 할당자를 잠금상태로 변환
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
/* export */ template<typename TALLOC, typename TUNIT>
inline TUNIT* CMemoryShuffleSyncAuto<TALLOC, TUNIT>::Lock()
{
	MSDT_ASSERT(NULL != m_Alloc);
	MSDT_ASSERT(NULL == m_Unit);

	m_Unit = m_Alloc->Lock();
	if (!m_Unit) {
		MSDT_RETURN(NULL);
	}

	// P.S.> 본 메서드에서 LOCK 된 이후에는 동기가 보장되며 경쟁상태 발생 시 대기중인 스레드의 LOCK 시도에 의해 CPU 점유율이 증가할 수 있으므로 최대한 신속한 처리 후 UNLOCK 이 요구됨

	if (!m_Alloc->Shuffle(m_Unit)) {
		MSDT_RETURN(NULL);
	}

	return m_Unit;
}


//================================================================

//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDataType::CMemoryShuffleDataType
//*---------------------------------------------------------------
// DESC : 생성자
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
/* export */ template<typename TDATA>
inline CMemoryShuffleDataType<TDATA>::CMemoryShuffleDataType()
{
	if (!m_Allocator.Create()) {
		::RaiseException(EV_MSDT_ECCD_ALLOCFAIL, 0, 0, NULL);	// 생성자에서 할당에 실패하면 별도의 오류조치가 불가능하므로 예외를 발생시킴
	}
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDataType::CMemoryShuffleDataType
//*---------------------------------------------------------------
// DESC : 생성자
// PARM :	1 . pData - 초기화에 사용할 값
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
/* export */ template<typename TDATA>
inline CMemoryShuffleDataType<TDATA>::CMemoryShuffleDataType(const TDATA& pData)
{
	if (!m_Allocator.Create(pData)) {
		::RaiseException(EV_MSDT_ECCD_ALLOCFAIL, 0, 0, NULL);	// 생성자에서 할당에 실패하면 별도의 오류조치가 불가능하므로 예외를 발생시킴
	}
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDataType::CMemoryShuffleDataType
//*---------------------------------------------------------------
// DESC : 생성자
// PARM :	1 . pMsdt - 초기화에 사용할 객체
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
/* export */ template<typename TDATA>
inline CMemoryShuffleDataType<TDATA>::CMemoryShuffleDataType(const CMemoryShuffleDataType<TDATA>& pMsdt)
{
	if (!m_Allocator.Create(pMsdt)) {
		::RaiseException(EV_MSDT_ECCD_ALLOCFAIL, 0, 0, NULL);	// 생성자에서 할당에 실패하면 별도의 오류조치가 불가능하므로 예외를 발생시킴
	}
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDataType::~CMemoryShuffleDataType
//*---------------------------------------------------------------
// DESC : 소멸자
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
/* export */ template<typename TDATA>
inline CMemoryShuffleDataType<TDATA>::~CMemoryShuffleDataType()
{
	
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDataType::GetAllocator
//*---------------------------------------------------------------
// DESC : 할당자 객체 참조 반환
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
/* export */ template<typename TDATA>
inline CMemoryShuffleAllocator<TDATA>& CMemoryShuffleDataType<TDATA>::GetAllocator()
{
	return m_Allocator;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDataType::GetAllocTickTerm
//*---------------------------------------------------------------
// DESC : 기존 메모리 셔플링 간격 반환
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
/* export */ template<typename TDATA>
inline unsigned long CMemoryShuffleDataType<TDATA>::GetAllocTickTerm() const
{
	m_Allocator.GetTickTerm();
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDataType::SetAllocTickTerm
//*---------------------------------------------------------------
// DESC : 신규 메모리 셔플링 간격 지정
// PARM :	1 . pTickTerm - 메모리 셔플링 간격
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
/* export */ template<typename TDATA>
inline void CMemoryShuffleDataType<TDATA>::SetAllocTickTerm(unsigned long pTickTerm)
{
	m_Allocator.SetTickTerm(pTickTerm);
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDataType::operator TDATA
//*---------------------------------------------------------------
// DESC : 형변환 연산자 구현
// PARM : N/A
// RETV : 현재 보유값
// PRGM : B4nFter
//*---------------------------------------------------------------
/* export */ template<typename TDATA>
inline CMemoryShuffleDataType<TDATA>::operator TDATA const ()
{
	TP_SYNCAUTO aSyncAuto(&m_Allocator);

	CMemoryShuffleBaseUnit<TDATA>* aUnit = aSyncAuto.Lock();
	if (!aUnit) {
		MSDT_RETURN(TDATA());
	}

	return(aUnit->GetValue());
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDataType::operator=
//*---------------------------------------------------------------
// DESC : 대입 연산자 구현
// PARM :	1 . pData - 연산 대입값
// RETV : 연산 결과값
// PRGM : B4nFter
//*---------------------------------------------------------------
/* export */ template<typename TDATA>
inline TDATA CMemoryShuffleDataType<TDATA>::operator=(const TDATA& pData)
{
	TP_SYNCAUTO aSyncAuto(&m_Allocator);

	CMemoryShuffleBaseUnit<TDATA>* aUnit = aSyncAuto.Lock();
	if (!aUnit) {
		MSDT_RETURN(TDATA());
	}

#if defined(SW_MSDT_ERRORCHECK_ORIGINVALUE)
	m_Allocator.m_OriginalValue = pData;
#endif	// SW_MSDT_ERRORCHECK_ORIGINVALUE

	aUnit->SetValue(pData);

	return pData;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDataType::operator=
//*---------------------------------------------------------------
// DESC : 대입 연산자 구현
// PARM :	1 . pMsdt - 연산 대입객체
// RETV : 객체 참조반환
// PRGM : B4nFter
//*---------------------------------------------------------------
/* export */ template<typename TDATA>
inline CMemoryShuffleDataType<TDATA>& CMemoryShuffleDataType<TDATA>::operator=(const CMemoryShuffleDataType<TDATA>& pMsdt)
{
	TDATA aData = static_cast<TDATA>(const_cast<CMemoryShuffleDataType<TDATA>&>(pMsdt));

	TP_SYNCAUTO aSyncAuto(&m_Allocator);

	CMemoryShuffleBaseUnit<TDATA>* aUnit = aSyncAuto.Lock();
	if (!aUnit) {
		MSDT_RETURN(*this);
	}

#if defined(SW_MSDT_ERRORCHECK_ORIGINVALUE)
	m_Allocator.m_OriginalValue = aData;
#endif	// SW_MSDT_ERRORCHECK_ORIGINVALUE

	aUnit->SetValue(aData);

	return(*this);
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDataType::operator++
//*---------------------------------------------------------------
// DESC : ++ (선치) 증감 연산자 구현
// PARM : N/A
// RETV : 연산 결과값
// PRGM : B4nFter
//*---------------------------------------------------------------
/* export */ template<typename TDATA>
inline TDATA CMemoryShuffleDataType<TDATA>::operator++()
{
	TP_SYNCAUTO aSyncAuto(&m_Allocator);

	CMemoryShuffleBaseUnit<TDATA>* aUnit = aSyncAuto.Lock();
	if (!aUnit) {
		MSDT_RETURN(TDATA());
	}

	TDATA aData = aUnit->GetValue() + 1;

#if defined(SW_MSDT_ERRORCHECK_ORIGINVALUE)
	++m_Allocator.m_OriginalValue;
#endif	// SW_MSDT_ERRORCHECK_ORIGINVALUE

	aUnit->SetValue(aData);

	return aData;
}


#if !defined(SW_MSDT_NOTINCLUDE_COMPILESAFETY)
//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDataType::operator++
//*---------------------------------------------------------------
// DESC : ++ (선치) 증감 연산자 구현 (bool 타입에 특화됨)
// PARM : N/A
// RETV : 연산 결과값
// PRGM : B4nFter
// P.S.>
//		- 원래 bool 데이터 타입에서 지원하지 않는 연산이므로 기본값만 반환 (연산 자체를 사용하지 않도록 할 것)
//*---------------------------------------------------------------
/* export */ template<>
inline bool CMemoryShuffleDataType<bool>::operator++()
{
	MSDT_ASSERT(0);		// "Can't be Called!"

	return(bool());
}
#endif	// SW_MSDT_NOTINCLUDE_COMPILESAFETY


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDataType::operator++
//*---------------------------------------------------------------
// DESC : ++ (후치) 증감 연산자 구현
// PARM : N/A
// RETV : 연산 결과값
// PRGM : B4nFter
//*---------------------------------------------------------------
/* export */ template<typename TDATA>
inline TDATA CMemoryShuffleDataType<TDATA>::operator++(int)
{
	TP_SYNCAUTO aSyncAuto(&m_Allocator);

	CMemoryShuffleBaseUnit<TDATA>* aUnit = aSyncAuto.Lock();
	if (!aUnit) {
		MSDT_RETURN(TDATA());
	}

	TDATA aData = aUnit->GetValue();

#if defined(SW_MSDT_ERRORCHECK_ORIGINVALUE)
	++m_Allocator.m_OriginalValue;
#endif	// SW_MSDT_ERRORCHECK_ORIGINVALUE

	aUnit->SetValue(aData + 1);

	return aData;
}


#if !defined(SW_MSDT_NOTINCLUDE_COMPILESAFETY)
//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDataType::operator++
//*---------------------------------------------------------------
// DESC : ++ (후치) 증감 연산자 구현 (bool 타입에 특화됨)
// PARM : N/A
// RETV : 연산 결과값
// PRGM : B4nFter
// P.S.>
//		- 원래 bool 데이터 타입에서 지원하지 않는 연산이므로 기본값만 반환 (연산 자체를 사용하지 않도록 할 것)
//*---------------------------------------------------------------
/* export */ template<>
inline bool CMemoryShuffleDataType<bool>::operator++(int)
{
	MSDT_ASSERT(0);		// "Can't be Called!"

	return(bool());
}
#endif	// SW_MSDT_NOTINCLUDE_COMPILESAFETY


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDataType::operator--
//*---------------------------------------------------------------
// DESC : -- (선치) 증감 연산자 구현
// PARM : N/A
// RETV : 연산 결과값
// PRGM : B4nFter
//*---------------------------------------------------------------
/* export */ template<typename TDATA>
inline TDATA CMemoryShuffleDataType<TDATA>::operator--()
{
	TP_SYNCAUTO aSyncAuto(&m_Allocator);

	CMemoryShuffleBaseUnit<TDATA>* aUnit = aSyncAuto.Lock();
	if (!aUnit) {
		MSDT_RETURN(TDATA());
	}

	TDATA aData = aUnit->GetValue() - 1;

#if defined(SW_MSDT_ERRORCHECK_ORIGINVALUE)
	--m_Allocator.m_OriginalValue;
#endif	// SW_MSDT_ERRORCHECK_ORIGINVALUE

	aUnit->SetValue(aData);

	return aData;
}


#if !defined(SW_MSDT_NOTINCLUDE_COMPILESAFETY)
//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDataType::operator--
//*---------------------------------------------------------------
// DESC : -- (선치) 증감 연산자 구현 (bool 타입에 특화됨)
// PARM : N/A
// RETV : 연산 결과값
// PRGM : B4nFter
// P.S.>
//		- 원래 bool 데이터 타입에서 지원하지 않는 연산이므로 기본값만 반환 (연산 자체를 사용하지 않도록 할 것)
//*---------------------------------------------------------------
/* export */ template<>
inline bool CMemoryShuffleDataType<bool>::operator--()
{
	MSDT_ASSERT(0);		// "Can't be Called!"

	return(bool());
}
#endif	// SW_MSDT_NOTINCLUDE_COMPILESAFETY


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDataType::operator--
//*---------------------------------------------------------------
// DESC : -- (후치) 증감 연산자 구현
// PARM : N/A
// RETV : 연산 결과값
// PRGM : B4nFter
//*---------------------------------------------------------------
/* export */ template<typename TDATA>
inline TDATA CMemoryShuffleDataType<TDATA>::operator--(int)
{
	TP_SYNCAUTO aSyncAuto(&m_Allocator);

	CMemoryShuffleBaseUnit<TDATA>* aUnit = aSyncAuto.Lock();
	if (!aUnit) {
		MSDT_RETURN(TDATA());
	}

	TDATA aData = aUnit->GetValue();

#if defined(SW_MSDT_ERRORCHECK_ORIGINVALUE)
	--m_Allocator.m_OriginalValue;
#endif	// SW_MSDT_ERRORCHECK_ORIGINVALUE

	aUnit->SetValue(aData - 1);

	return aData;
}


#if !defined(SW_MSDT_NOTINCLUDE_COMPILESAFETY)
//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDataType::operator--
//*---------------------------------------------------------------
// DESC : -- (후치) 증감 연산자 구현 (bool 타입에 특화됨)
// PARM : N/A
// RETV : 연산 결과값
// PRGM : B4nFter
// P.S.>
//		- 원래 bool 데이터 타입에서 지원하지 않는 연산이므로 기본값만 반환 (연산 자체를 사용하지 않도록 할 것)
//*---------------------------------------------------------------
/* export */ template<>
inline bool CMemoryShuffleDataType<bool>::operator--(int)
{
	MSDT_ASSERT(0);		// "Can't be Called!"

	return(bool());
}
#endif	// SW_MSDT_NOTINCLUDE_COMPILESAFETY


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDataType::operator+=
//*---------------------------------------------------------------
// DESC : += 복합 대입 연산자 구현
// PARM :	1 . pData - 연산 인자값
// RETV : 연산 결과값
// PRGM : B4nFter
//*---------------------------------------------------------------
/* export */ template<typename TDATA>
inline TDATA CMemoryShuffleDataType<TDATA>::operator+=(const TDATA& pData)
{
	TP_SYNCAUTO aSyncAuto(&m_Allocator);

	CMemoryShuffleBaseUnit<TDATA>* aUnit = aSyncAuto.Lock();
	if (!aUnit) {
		MSDT_RETURN(TDATA());
	}

	TDATA aData = aUnit->GetValue() + pData;

#if defined(SW_MSDT_ERRORCHECK_ORIGINVALUE)
	m_Allocator.m_OriginalValue += pData;
#endif	// SW_MSDT_ERRORCHECK_ORIGINVALUE

	aUnit->SetValue(aData);

	return aData;
}


#if !defined(SW_MSDT_NOTINCLUDE_COMPILESAFETY)
//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDataType::operator+=
//*---------------------------------------------------------------
// DESC : += 복합 대입 연산자 구현 (bool 타입에 특화됨)
// PARM :	1 . pData - 연산 인자값
// RETV : 연산 결과값
// PRGM : B4nFter
// P.S.>
//		- 원래 bool 데이터 타입에서 지원하지 않는 연산이므로 기본값만 반환 (연산 자체를 사용하지 않도록 할 것)
//*---------------------------------------------------------------
/* export */ template<>
inline bool CMemoryShuffleDataType<bool>::operator+=(const bool& pData)
{
	MSDT_ASSERT(0);		// "Can't be Called!"

	return(bool());
}
#endif	// SW_MSDT_NOTINCLUDE_COMPILESAFETY


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDataType::operator-=
//*---------------------------------------------------------------
// DESC : -= 복합 대입 연산자 구현
// PARM :	1 . pData - 연산 인자값
// RETV : 연산 결과값
// PRGM : B4nFter
//*---------------------------------------------------------------
/* export */ template<typename TDATA>
inline TDATA CMemoryShuffleDataType<TDATA>::operator-=(const TDATA& pData)
{
	TP_SYNCAUTO aSyncAuto(&m_Allocator);

	CMemoryShuffleBaseUnit<TDATA>* aUnit = aSyncAuto.Lock();
	if (!aUnit) {
		MSDT_RETURN(TDATA());
	}

	TDATA aData = aUnit->GetValue() - pData;

#if defined(SW_MSDT_ERRORCHECK_ORIGINVALUE)
	m_Allocator.m_OriginalValue -= pData;
#endif	// SW_MSDT_ERRORCHECK_ORIGINVALUE

	aUnit->SetValue(aData);

	return aData;
}


#if !defined(SW_MSDT_NOTINCLUDE_COMPILESAFETY)
//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDataType::operator-=
//*---------------------------------------------------------------
// DESC : -= 복합 대입 연산자 구현 (bool 타입에 특화됨)
// PARM :	1 . pData - 연산 인자값
// RETV : 연산 결과값
// PRGM : B4nFter
// P.S.>
//		- 원래 bool 데이터 타입에서 지원하지 않는 연산이므로 기본값만 반환 (연산 자체를 사용하지 않도록 할 것)
//*---------------------------------------------------------------
/* export */ template<>
inline bool CMemoryShuffleDataType<bool>::operator-=(const bool& pData)
{
	MSDT_ASSERT(0);		// "Can't be Called!"

	return(bool());
}
#endif	// SW_MSDT_NOTINCLUDE_COMPILESAFETY


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDataType::operator*=
//*---------------------------------------------------------------
// DESC : *= 복합 대입 연산자 구현
// PARM :	1 . pData - 연산 인자값
// RETV : 연산 결과값
// PRGM : B4nFter
//*---------------------------------------------------------------
/* export */ template<typename TDATA>
inline TDATA CMemoryShuffleDataType<TDATA>::operator*=(const TDATA& pData)
{
	TP_SYNCAUTO aSyncAuto(&m_Allocator);

	CMemoryShuffleBaseUnit<TDATA>* aUnit = aSyncAuto.Lock();
	if (!aUnit) {
		MSDT_RETURN(TDATA());
	}

	TDATA aData = aUnit->GetValue() * pData;

#if defined(SW_MSDT_ERRORCHECK_ORIGINVALUE)
	m_Allocator.m_OriginalValue *= pData;
#endif	// SW_MSDT_ERRORCHECK_ORIGINVALUE

	aUnit->SetValue(aData);

	return aData;
}


#if !defined(SW_MSDT_NOTINCLUDE_COMPILESAFETY)
//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDataType::operator*=
//*---------------------------------------------------------------
// DESC : *= 복합 대입 연산자 구현 (bool 타입에 특화됨)
// PARM :	1 . pData - 연산 인자값
// RETV : 연산 결과값
// PRGM : B4nFter
// P.S.>
//		- 원래 bool 데이터 타입에서 지원하지 않는 연산이므로 기본값만 반환 (연산 자체를 사용하지 않도록 할 것)
//*---------------------------------------------------------------
/* export */ template<>
inline bool CMemoryShuffleDataType<bool>::operator*=(const bool& pData)
{
	MSDT_ASSERT(0);		// "Can't be Called!"

	return(bool());
}
#endif	// SW_MSDT_NOTINCLUDE_COMPILESAFETY


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDataType::operator/=
//*---------------------------------------------------------------
// DESC : /= 복합 대입 연산자 구현
// PARM :	1 . pData - 연산 인자값
// RETV : 연산 결과값
// PRGM : B4nFter
//*---------------------------------------------------------------
/* export */ template<typename TDATA>
inline TDATA CMemoryShuffleDataType<TDATA>::operator/=(const TDATA& pData)
{
	TP_SYNCAUTO aSyncAuto(&m_Allocator);

	CMemoryShuffleBaseUnit<TDATA>* aUnit = aSyncAuto.Lock();
	if (!aUnit) {
		MSDT_RETURN(TDATA());
	}

	TDATA aData = aUnit->GetValue() / pData;

#if defined(SW_MSDT_ERRORCHECK_ORIGINVALUE)
	m_Allocator.m_OriginalValue /= pData;
#endif	// SW_MSDT_ERRORCHECK_ORIGINVALUE

	aUnit->SetValue(aData);

	return aData;
}


#if !defined(SW_MSDT_NOTINCLUDE_COMPILESAFETY)
//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDataType::operator/=
//*---------------------------------------------------------------
// DESC : /= 복합 대입 연산자 구현 (bool 타입에 특화됨)
// PARM :	1 . pData - 연산 인자값
// RETV : 연산 결과값
// PRGM : B4nFter
// P.S.>
//		- 원래 bool 데이터 타입에서 지원하지 않는 연산이므로 기본값만 반환 (연산 자체를 사용하지 않도록 할 것)
//*---------------------------------------------------------------
/* export */ template<>
inline bool CMemoryShuffleDataType<bool>::operator/=(const bool& pData)
{
	MSDT_ASSERT(0);		// "Can't be Called!"

	return(bool());
}
#endif	// SW_MSDT_NOTINCLUDE_COMPILESAFETY


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDataType::operator%=
//*---------------------------------------------------------------
// DESC : %= 복합 대입 연산자 구현
// PARM :	1 . pData - 연산 인자값
// RETV : 연산 결과값
// PRGM : B4nFter
//*---------------------------------------------------------------
/* export */ template<typename TDATA>
inline TDATA CMemoryShuffleDataType<TDATA>::operator%=(const TDATA& pData)
{
	TP_SYNCAUTO aSyncAuto(&m_Allocator);

	CMemoryShuffleBaseUnit<TDATA>* aUnit = aSyncAuto.Lock();
	if (!aUnit) {
		MSDT_RETURN(TDATA());
	}

	TDATA aData = aUnit->GetValue() % pData;

#if defined(SW_MSDT_ERRORCHECK_ORIGINVALUE)
	m_Allocator.m_OriginalValue %= pData;
#endif	// SW_MSDT_ERRORCHECK_ORIGINVALUE

	aUnit->SetValue(aData);

	return aData;
}


#if !defined(SW_MSDT_NOTINCLUDE_COMPILESAFETY)
//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDataType::operator%=
//*---------------------------------------------------------------
// DESC : %= 복합 대입 연산자 구현 (bool 타입에 특화됨)
// PARM :	1 . pData - 연산 인자값
// RETV : 연산 결과값
// PRGM : B4nFter
// P.S.>
//		- 원래 bool 데이터 타입에서 지원하지 않는 연산이므로 기본값만 반환 (연산 자체를 사용하지 않도록 할 것)
//*---------------------------------------------------------------
/* export */ template<>
inline bool CMemoryShuffleDataType<bool>::operator%=(const bool& pData)
{
	MSDT_ASSERT(0);		// "Can't be Called!"

	return(bool());
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDataType::operator%=
//*---------------------------------------------------------------
// DESC : %= 복합 대입 연산자 구현 (float 타입에 특화됨)
// PARM :	1 . pData - 연산 인자값
// RETV : 연산 결과값
// PRGM : B4nFter
// P.S.>
//		- 원래 실수형 데이터 타입에서 지원하지 않는 연산이므로 기본값만 반환 (연산 자체를 사용하지 않도록 할 것)
//*---------------------------------------------------------------
/* export */ template<>
inline float CMemoryShuffleDataType<float>::operator%=(const float& pData)
{
	MSDT_ASSERT(0);		// "Can't be Called!"

	return(float());
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDataType::operator%=
//*---------------------------------------------------------------
// DESC : %= 복합 대입 연산자 구현 (double 타입에 특화됨)
// PARM :	1 . pData - 연산 인자값
// RETV : 연산 결과값
// PRGM : B4nFter
// P.S.>
//		- 원래 실수형 데이터 타입에서 지원하지 않는 연산이므로 기본값만 반환 (연산 자체를 사용하지 않도록 할 것)
//*---------------------------------------------------------------
/* export */ template<>
inline double CMemoryShuffleDataType<double>::operator%=(const double& pData)
{
	MSDT_ASSERT(0);		// "Can't be Called!"

	return(double());
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDataType::operator%=
//*---------------------------------------------------------------
// DESC : %= 복합 대입 연산자 구현 (long double 타입에 특화됨)
// PARM :	1 . pData - 연산 인자값
// RETV : 연산 결과값
// PRGM : B4nFter
// P.S.>
//		- 원래 실수형 데이터 타입에서 지원하지 않는 연산이므로 기본값만 반환 (연산 자체를 사용하지 않도록 할 것)
//*---------------------------------------------------------------
/* export */ template<>
inline long double CMemoryShuffleDataType<long double>::operator%=(const long double& pData)
{
	MSDT_ASSERT(0);		// "Can't be Called!"

	return(long double());
}
#endif	// SW_MSDT_NOTINCLUDE_COMPILESAFETY


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDataType::operator&=
//*---------------------------------------------------------------
// DESC : &= 복합 대입 연산자 구현
// PARM :	1 . pData - 연산 인자값
// RETV : 연산 결과값
// PRGM : B4nFter
//*---------------------------------------------------------------
/* export */ template<typename TDATA>
inline TDATA CMemoryShuffleDataType<TDATA>::operator&=(const TDATA& pData)
{
	TP_SYNCAUTO aSyncAuto(&m_Allocator);

	CMemoryShuffleBaseUnit<TDATA>* aUnit = aSyncAuto.Lock();
	if (!aUnit) {
		MSDT_RETURN(TDATA());
	}

	TDATA aData = aUnit->GetValue() & pData;

#if defined(SW_MSDT_ERRORCHECK_ORIGINVALUE)
	m_Allocator.m_OriginalValue &= pData;
#endif	// SW_MSDT_ERRORCHECK_ORIGINVALUE

	aUnit->SetValue(aData);

	return aData;
}


#if !defined(SW_MSDT_NOTINCLUDE_COMPILESAFETY)
//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDataType::operator&=
//*---------------------------------------------------------------
// DESC : &= 복합 대입 연산자 구현 (float 타입에 특화됨)
// PARM :	1 . pData - 연산 인자값
// RETV : 연산 결과값
// PRGM : B4nFter
// P.S.>
//		- 원래 실수형 데이터 타입에서 지원하지 않는 연산이므로 기본값만 반환 (연산 자체를 사용하지 않도록 할 것)
//*---------------------------------------------------------------
/* export */ template<>
inline float CMemoryShuffleDataType<float>::operator&=(const float& pData)
{
	MSDT_ASSERT(0);		// "Can't be Called!"

	return(float());
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDataType::operator&=
//*---------------------------------------------------------------
// DESC : &= 복합 대입 연산자 구현 (double 타입에 특화됨)
// PARM :	1 . pData - 연산 인자값
// RETV : 연산 결과값
// PRGM : B4nFter
// P.S.>
//		- 원래 실수형 데이터 타입에서 지원하지 않는 연산이므로 기본값만 반환 (연산 자체를 사용하지 않도록 할 것)
//*---------------------------------------------------------------
/* export */ template<>
inline double CMemoryShuffleDataType<double>::operator&=(const double& pData)
{
	MSDT_ASSERT(0);		// "Can't be Called!"

	return(double());
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDataType::operator&=
//*---------------------------------------------------------------
// DESC : &= 복합 대입 연산자 구현 (long double 타입에 특화됨)
// PARM :	1 . pData - 연산 인자값
// RETV : 연산 결과값
// PRGM : B4nFter
// P.S.>
//		- 원래 실수형 데이터 타입에서 지원하지 않는 연산이므로 기본값만 반환 (연산 자체를 사용하지 않도록 할 것)
//*---------------------------------------------------------------
/* export */ template<>
inline long double CMemoryShuffleDataType<long double>::operator&=(const long double& pData)
{
	MSDT_ASSERT(0);		// "Can't be Called!"

	return(long double());
}
#endif	// SW_MSDT_NOTINCLUDE_COMPILESAFETY


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDataType::operator|=
//*---------------------------------------------------------------
// DESC : |= 복합 대입 연산자 구현
// PARM :	1 . pData - 연산 인자값
// RETV : 연산 결과값
// PRGM : B4nFter
//*---------------------------------------------------------------
/* export */ template<typename TDATA>
inline TDATA CMemoryShuffleDataType<TDATA>::operator|=(const TDATA& pData)
{
	TP_SYNCAUTO aSyncAuto(&m_Allocator);

	CMemoryShuffleBaseUnit<TDATA>* aUnit = aSyncAuto.Lock();
	if (!aUnit) {
		MSDT_RETURN(TDATA());
	}

	TDATA aData = aUnit->GetValue() | pData;

#if defined(SW_MSDT_ERRORCHECK_ORIGINVALUE)
	m_Allocator.m_OriginalValue |= pData;
#endif	// SW_MSDT_ERRORCHECK_ORIGINVALUE

	aUnit->SetValue(aData);

	return aData;
}


#if !defined(SW_MSDT_NOTINCLUDE_COMPILESAFETY)
//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDataType::operator|=
//*---------------------------------------------------------------
// DESC : |= 복합 대입 연산자 구현 (float 타입에 특화됨)
// PARM :	1 . pData - 연산 인자값
// RETV : 연산 결과값
// PRGM : B4nFter
// P.S.>
//		- 원래 실수형 데이터 타입에서 지원하지 않는 연산이므로 기본값만 반환 (연산 자체를 사용하지 않도록 할 것)
//*---------------------------------------------------------------
/* export */ template<>
inline float CMemoryShuffleDataType<float>::operator|=(const float& pData)
{
	MSDT_ASSERT(0);		// "Can't be Called!"

	return(float());
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDataType::operator|=
//*---------------------------------------------------------------
// DESC : |= 복합 대입 연산자 구현 (double 타입에 특화됨)
// PARM :	1 . pData - 연산 인자값
// RETV : 연산 결과값
// PRGM : B4nFter
// P.S.>
//		- 원래 실수형 데이터 타입에서 지원하지 않는 연산이므로 기본값만 반환 (연산 자체를 사용하지 않도록 할 것)
//*---------------------------------------------------------------
/* export */ template<>
inline double CMemoryShuffleDataType<double>::operator|=(const double& pData)
{
	MSDT_ASSERT(0);		// "Can't be Called!"

	return(double());
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDataType::operator|=
//*---------------------------------------------------------------
// DESC : |= 복합 대입 연산자 구현 (long double 타입에 특화됨)
// PARM :	1 . pData - 연산 인자값
// RETV : 연산 결과값
// PRGM : B4nFter
// P.S.>
//		- 원래 실수형 데이터 타입에서 지원하지 않는 연산이므로 기본값만 반환 (연산 자체를 사용하지 않도록 할 것)
//*---------------------------------------------------------------
/* export */ template<>
inline long double CMemoryShuffleDataType<long double>::operator|=(const long double& pData)
{
	MSDT_ASSERT(0);		// "Can't be Called!"

	return(long double());
}
#endif	// SW_MSDT_NOTINCLUDE_COMPILESAFETY


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDataType::operator^=
//*---------------------------------------------------------------
// DESC : ^= 복합 대입 연산자 구현
// PARM :	1 . pData - 연산 인자값
// RETV : 연산 결과값
// PRGM : B4nFter
//*---------------------------------------------------------------
/* export */ template<typename TDATA>
inline TDATA CMemoryShuffleDataType<TDATA>::operator^=(const TDATA& pData)
{
	TP_SYNCAUTO aSyncAuto(&m_Allocator);

	CMemoryShuffleBaseUnit<TDATA>* aUnit = aSyncAuto.Lock();
	if (!aUnit) {
		MSDT_RETURN(TDATA());
	}

	TDATA aData = aUnit->GetValue() ^ pData;

#if defined(SW_MSDT_ERRORCHECK_ORIGINVALUE)
	m_Allocator.m_OriginalValue ^= pData;
#endif	// SW_MSDT_ERRORCHECK_ORIGINVALUE

	aUnit->SetValue(aData);

	return aData;
}


#if !defined(SW_MSDT_NOTINCLUDE_COMPILESAFETY)
//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDataType::operator^=
//*---------------------------------------------------------------
// DESC : ^= 복합 대입 연산자 구현 (float 타입에 특화됨)
// PARM :	1 . pData - 연산 인자값
// RETV : 연산 결과값
// PRGM : B4nFter
// P.S.>
//		- 원래 실수형 데이터 타입에서 지원하지 않는 연산이므로 기본값만 반환 (연산 자체를 사용하지 않도록 할 것)
//*---------------------------------------------------------------
/* export */ template<>
inline float CMemoryShuffleDataType<float>::operator^=(const float& pData)
{
	MSDT_ASSERT(0);		// "Can't be Called!"

	return(float());
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDataType::operator^=
//*---------------------------------------------------------------
// DESC : ^= 복합 대입 연산자 구현 (double 타입에 특화됨)
// PARM :	1 . pData - 연산 인자값
// RETV : 연산 결과값
// PRGM : B4nFter
// P.S.>
//		- 원래 실수형 데이터 타입에서 지원하지 않는 연산이므로 기본값만 반환 (연산 자체를 사용하지 않도록 할 것)
//*---------------------------------------------------------------
/* export */ template<>
inline double CMemoryShuffleDataType<double>::operator^=(const double& pData)
{
	MSDT_ASSERT(0);		// "Can't be Called!"

	return(double());
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDataType::operator^=
//*---------------------------------------------------------------
// DESC : ^= 복합 대입 연산자 구현 (long double 타입에 특화됨)
// PARM :	1 . pData - 연산 인자값
// RETV : 연산 결과값
// PRGM : B4nFter
// P.S.>
//		- 원래 실수형 데이터 타입에서 지원하지 않는 연산이므로 기본값만 반환 (연산 자체를 사용하지 않도록 할 것)
//*---------------------------------------------------------------
/* export */ template<>
inline long double CMemoryShuffleDataType<long double>::operator^=(const long double& pData)
{
	MSDT_ASSERT(0);		// "Can't be Called!"

	return(long double());
}
#endif	// SW_MSDT_NOTINCLUDE_COMPILESAFETY

