

///////////////////////////////////////////////////////////////////
//	INCLUDE
///////////////////////////////////////////////////////////////////

//#include "MEMORYSHUFFLEDATATYPE.H"	// P.S.> 'export' ���� �����Ǵ� �����Ϸ����� ���
#include <WINDOWS.H>
#include <BASETSD.H>
#include <NEW>			// placement new


///////////////////////////////////////////////////////////////////
//	DEFINE
///////////////////////////////////////////////////////////////////

#define DF_MSDT_LOCKSIGN		(0x6A035984)	// ���� üũ ��� (P.S.> ���� 0 ���� �����ؼ� �ʵ�)	
#define FIX_VIOLATION_BY_FREE // new �� ������ ��ü�� free() �� �����ϸ鼭 ����� �޸𸮿��� ħ�� ����.

#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }

///////////////////////////////////////////////////////////////////
//	DECLARATION
///////////////////////////////////////////////////////////////////

//*---------------------------------------------------------------
// TYPE : CLASS
// NAME : CMemoryShuffleDervUnit
//*---------------------------------------------------------------
// DESC : �޸� ���� �⺻���� Ŭ���� (�Ļ�)
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
// DESC : ������
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
// DESC : �Ҹ���
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
// DESC : ���� �޸𸮰� ������ ���ø��� ������ ��ȯ
// PARM : N/A
// RETV : ���� ���ø� ����
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
// DESC : ���� �޸𸮰� ������ ���ø��� ������ ����
// PARM :	1 . pLastTick - �ű� ���ø� ����
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
// DESC : ������
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
// DESC : �Ҹ���
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
// DESC : ���� �������� ��ȯ
// PARM : N/A
// RETV : ���� ������
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
// DESC : �ű� ���԰��� ����
// PARM :	1 . pData - �ű� ���԰�
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
// DESC : Ư������ ��ȯŰ�� ����Ͽ� XOR ������
// PARM :	1 . pIn - ��ȯ�� Ư������ ���� ����
//			2 . pOut - ��ȯ ����� ����� ����
//			2 . pSize - Ư������ ũ��
//			3 . pKey - ��ȯŰ
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
/* export */ template<typename TDATA>
inline void CMemoryShuffleDervUnit<TDATA>::GetXor(char const * pIn, char* pOut, unsigned int pSize, unsigned int pKey) const
{
	// P.S.> ���� �ʿ��� ��쿡 pKey �� ��ȯ�ϰų� XOR ����� �ٲٴ� ����� ���� �� ���� ����

	unsigned int const * aIn = reinterpret_cast<unsigned int const *>(pIn);	// ++ ���꿡 l-value �� �ʿ��ϹǷ� �ӽ� ������ ������ ���� ���� ���� (C2105)
	unsigned int* aOut = reinterpret_cast<unsigned int*>(pOut);				// ++ ���꿡 l-value �� �ʿ��ϹǷ� �ӽ� ������ ������ ���� ���� ���� (C2105)
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
// DESC : ������
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
/* export */ template<typename TDATA>
inline CMemoryShuffleAllocator<TDATA>::CMemoryShuffleAllocator() : m_TickTerm(0), m_Unit(NULL), m_Lock(DF_MSDT_LOCKSIGN)
{
	// ���� üũ�� ����
#if defined(SW_MSDT_ERRORCHECK_ORIGINVALUE)
	m_OriginalValue = TDATA();
#endif	// SW_MSDT_ERRORCHECK_ORIGINVALUE
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleAllocator::~CMemoryShuffleAllocator
//*---------------------------------------------------------------
// DESC : �Ҹ���
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
// DESC : �Ҵ��� ��밡�� ���� ��ȯ
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
// DESC : ���� �޸𸮸� ���ʷ� ����
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
// P.S.>
//		- �� �޼���� ������ �Ҵ���� ���� �޸𸮰� ���� ����ȭ�� �ʿ���� ��Ȳ���� ȣ����� ������
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

	m_Unit = new (aUnit) CMemoryShuffleDervUnit<TDATA>;		// placement new �� std::bad_alloc �� �߻���Ű�� �����Ƿ� ��������� ���ܸ� �߻���Ű�� ���� ?
	MSDT_ASSERT(NULL != m_Unit);

//#if defined(SW_MSDT_ERRORCHECK_ORIGINVALUE)
	m_Unit->SetValue(TDATA());	// 0 �ʱ�ȭ ��
//#endif	// SW_MSDT_ERRORCHECK_ORIGINVALUE

	SetTickTerm();

	return true;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleAllocator::Create
//*---------------------------------------------------------------
// DESC : ���� �޸𸮸� ���ʷ� �����ϰ� Ư�� ������ �ʱ�ȭ
// PARM :	1 . pData - �ʱ�ȭ�� ����� ��
// RETV : N/A
// PRGM : B4nFter
// P.S.>
//		- �� �޼���� ������ �Ҵ���� ���� �޸𸮰� ���� ����ȭ�� �ʿ���� ��Ȳ���� ȣ����� ������
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
// DESC : ���� �޸𸮸� ���ʷ� �����ϰ� Ư�� ��ü�� �ʱ�ȭ
// PARM :	1 . pMsdt - �ʱ�ȭ�� ����� ��ü
// RETV : N/A
// PRGM : B4nFter
// P.S.>
//		- �� �޼���� ������ �Ҵ���� ���� �޸𸮰� ���� ����ȭ�� �ʿ���� ��Ȳ���� ȣ����� ������
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
// DESC : ������ ���� �޸𸮸� �����ϰ� �Ҵ��ڸ� ���Ұ��� ����
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
/* export */ template<typename TDATA>
inline void CMemoryShuffleAllocator<TDATA>::Release()
{
	if (m_Unit) {
//		m_Unit->~TDATA();	// placement new ������� ���� ������� �Ҹ��� ȣ�� (�Ҹ��ڰ� ���� TDATA �� ���� ������ ���� �߻�)
		
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
// DESC : ���� �޸𸮸� ������ �ð� ���ݿ� ���� �ٸ� ���� �޸𸮷� ��ȯ
// PARM :	1 . pUnit - �޸� ���ø��� ������ �⺻���� ��ü
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
/* export */ template<typename TDATA>
inline bool CMemoryShuffleAllocator<TDATA>::Shuffle(CMemoryShuffleBaseUnit<TDATA>*& pUnit) const
{
	MSDT_ASSERT(NULL != pUnit);

	// P.S.> �� �޼��忡 ������ �ñ⿡�� CMemoryShuffleAllocator<TDATA> ��ü�� ���Ⱑ ����Ǿ�� ��

	CMemoryShuffleDervUnit<TDATA>* aUnit1 = static_cast<CMemoryShuffleDervUnit<TDATA>*>(pUnit);
	
	unsigned long aCurTick = ::GetTickCount();	// �ػ󵵰� ���Ƶ� ���� ?
	if (m_TickTerm < ((aCurTick >= aUnit1->GetLastTick())?(aCurTick - aUnit1->GetLastTick()):(ULONG_MAX - aUnit1->GetLastTick() + aCurTick))) {
		TDATA aData = aUnit1->GetValue();

		char* aUnit2 = static_cast<char*>(malloc(sizeof(CMemoryShuffleDervUnit<TDATA>)));	// �޸� ���� ���Ŀ� �ٽ� �Ҵ������ ���� �ּҰ� �� ���ɼ��� �����Ƿ� �̸� �����ϱ� ���� �Ҵ��� �켱 ����
		if (!aUnit2) {
			m_TickTerm = 0;
			::RaiseException(EV_MSDT_ECCD_ALLOCFAIL, 0, 0, NULL);	// �Ҵ翡 �����ϸ� ��� Ȯ���ϴ� ���� �����Ƿ� ���ܸ� �߻���Ŵ
			MSDT_RETURN(false);
		}

//		aUnit1->~TDATA();	// placement new ������� ���� ������� �Ҹ��� ȣ�� (�Ҹ��ڰ� ���� TDATA �� ���� ������ ���� �߻�)
		
#ifdef FIX_VIOLATION_BY_FREE
		SAFE_DELETE( aUnit1 );
#else
		free(reinterpret_cast<char*>(aUnit1));
#endif
		
		aUnit1 = new (aUnit2) CMemoryShuffleDervUnit<TDATA>;	// placement new �� std::bad_alloc �� �߻���Ű�� �����Ƿ� ��������� ���ܸ� �߻���Ű�� ���� ?
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
// DESC : ���� �޸� ���ø� ���� ��ȯ
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
// DESC : �ű� �޸� ���ø� ���� ����
// PARM :	1 . pTickTerm - �޸� ���ø� ����
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
// DESC : �Ҵ��ڸ� ��ݻ��·� ��ȯ
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

	m_Unit = NULL;	// �ݵ�� ������ �ʿ�� ������ ���ȼ��� �ټҳ��� ������ ����

	return aUnit;
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleAllocator::Unlock
//*---------------------------------------------------------------
// DESC : �Ҵ����� ��ݻ��¸� ����
// PARM :	1 . pUnit - ��ݻ��¸� �����ϴµ� ����� �⺻���� ��ü
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
/* export */ template<typename TDATA>
inline void CMemoryShuffleAllocator<TDATA>::Unlock(const CMemoryShuffleBaseUnit<TDATA>* pUnit) const
{
	MSDT_ASSERT(NULL == m_Unit);				// Lock �� m_Unit �� NULL ���� �� üũ
	MSDT_ASSERT(NULL != pUnit);

	if (!pUnit) {
		MSDT_RETURN_NONE;
	}


	m_Unit = const_cast<CMemoryShuffleDervUnit<TDATA>*>(static_cast<const CMemoryShuffleDervUnit<TDATA>*>(pUnit));	// ����ó�� ������ ��ü ��ü�� ���ڼ��� ���� (InterlockedExchangePointer) �Ǵ� ���� �ƴϹǷ� ���� ���� ���� �����͸� ��ü�ؾ� ��

	long aLock = InterlockedExchange(&m_Lock, DF_MSDT_LOCKSIGN);
	MSDT_ASSERT(DF_MSDT_LOCKSIGN != aLock);
}


//================================================================

//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleSyncAuto::CMemoryShuffleSyncAuto
//*---------------------------------------------------------------
// DESC : ������
// PARM :	1 . pAlloc - TALLOC Ŭ���� ��ü������
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
// DESC : �Ҹ���
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
// DESC : ������ �Ҵ��ڸ� ��ݻ��·� ��ȯ
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

	// P.S.> �� �޼��忡�� LOCK �� ���Ŀ��� ���Ⱑ ����Ǹ� ������� �߻� �� ������� �������� LOCK �õ��� ���� CPU �������� ������ �� �����Ƿ� �ִ��� �ż��� ó�� �� UNLOCK �� �䱸��

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
// DESC : ������
// PARM : N/A
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
/* export */ template<typename TDATA>
inline CMemoryShuffleDataType<TDATA>::CMemoryShuffleDataType()
{
	if (!m_Allocator.Create()) {
		::RaiseException(EV_MSDT_ECCD_ALLOCFAIL, 0, 0, NULL);	// �����ڿ��� �Ҵ翡 �����ϸ� ������ ������ġ�� �Ұ����ϹǷ� ���ܸ� �߻���Ŵ
	}
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDataType::CMemoryShuffleDataType
//*---------------------------------------------------------------
// DESC : ������
// PARM :	1 . pData - �ʱ�ȭ�� ����� ��
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
/* export */ template<typename TDATA>
inline CMemoryShuffleDataType<TDATA>::CMemoryShuffleDataType(const TDATA& pData)
{
	if (!m_Allocator.Create(pData)) {
		::RaiseException(EV_MSDT_ECCD_ALLOCFAIL, 0, 0, NULL);	// �����ڿ��� �Ҵ翡 �����ϸ� ������ ������ġ�� �Ұ����ϹǷ� ���ܸ� �߻���Ŵ
	}
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDataType::CMemoryShuffleDataType
//*---------------------------------------------------------------
// DESC : ������
// PARM :	1 . pMsdt - �ʱ�ȭ�� ����� ��ü
// RETV : N/A
// PRGM : B4nFter
//*---------------------------------------------------------------
/* export */ template<typename TDATA>
inline CMemoryShuffleDataType<TDATA>::CMemoryShuffleDataType(const CMemoryShuffleDataType<TDATA>& pMsdt)
{
	if (!m_Allocator.Create(pMsdt)) {
		::RaiseException(EV_MSDT_ECCD_ALLOCFAIL, 0, 0, NULL);	// �����ڿ��� �Ҵ翡 �����ϸ� ������ ������ġ�� �Ұ����ϹǷ� ���ܸ� �߻���Ŵ
	}
}


//*---------------------------------------------------------------
// TYPE : FUNCTION
// NAME : CMemoryShuffleDataType::~CMemoryShuffleDataType
//*---------------------------------------------------------------
// DESC : �Ҹ���
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
// DESC : �Ҵ��� ��ü ���� ��ȯ
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
// DESC : ���� �޸� ���ø� ���� ��ȯ
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
// DESC : �ű� �޸� ���ø� ���� ����
// PARM :	1 . pTickTerm - �޸� ���ø� ����
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
// DESC : ����ȯ ������ ����
// PARM : N/A
// RETV : ���� ������
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
// DESC : ���� ������ ����
// PARM :	1 . pData - ���� ���԰�
// RETV : ���� �����
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
// DESC : ���� ������ ����
// PARM :	1 . pMsdt - ���� ���԰�ü
// RETV : ��ü ������ȯ
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
// DESC : ++ (��ġ) ���� ������ ����
// PARM : N/A
// RETV : ���� �����
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
// DESC : ++ (��ġ) ���� ������ ���� (bool Ÿ�Կ� Ưȭ��)
// PARM : N/A
// RETV : ���� �����
// PRGM : B4nFter
// P.S.>
//		- ���� bool ������ Ÿ�Կ��� �������� �ʴ� �����̹Ƿ� �⺻���� ��ȯ (���� ��ü�� ������� �ʵ��� �� ��)
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
// DESC : ++ (��ġ) ���� ������ ����
// PARM : N/A
// RETV : ���� �����
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
// DESC : ++ (��ġ) ���� ������ ���� (bool Ÿ�Կ� Ưȭ��)
// PARM : N/A
// RETV : ���� �����
// PRGM : B4nFter
// P.S.>
//		- ���� bool ������ Ÿ�Կ��� �������� �ʴ� �����̹Ƿ� �⺻���� ��ȯ (���� ��ü�� ������� �ʵ��� �� ��)
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
// DESC : -- (��ġ) ���� ������ ����
// PARM : N/A
// RETV : ���� �����
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
// DESC : -- (��ġ) ���� ������ ���� (bool Ÿ�Կ� Ưȭ��)
// PARM : N/A
// RETV : ���� �����
// PRGM : B4nFter
// P.S.>
//		- ���� bool ������ Ÿ�Կ��� �������� �ʴ� �����̹Ƿ� �⺻���� ��ȯ (���� ��ü�� ������� �ʵ��� �� ��)
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
// DESC : -- (��ġ) ���� ������ ����
// PARM : N/A
// RETV : ���� �����
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
// DESC : -- (��ġ) ���� ������ ���� (bool Ÿ�Կ� Ưȭ��)
// PARM : N/A
// RETV : ���� �����
// PRGM : B4nFter
// P.S.>
//		- ���� bool ������ Ÿ�Կ��� �������� �ʴ� �����̹Ƿ� �⺻���� ��ȯ (���� ��ü�� ������� �ʵ��� �� ��)
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
// DESC : += ���� ���� ������ ����
// PARM :	1 . pData - ���� ���ڰ�
// RETV : ���� �����
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
// DESC : += ���� ���� ������ ���� (bool Ÿ�Կ� Ưȭ��)
// PARM :	1 . pData - ���� ���ڰ�
// RETV : ���� �����
// PRGM : B4nFter
// P.S.>
//		- ���� bool ������ Ÿ�Կ��� �������� �ʴ� �����̹Ƿ� �⺻���� ��ȯ (���� ��ü�� ������� �ʵ��� �� ��)
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
// DESC : -= ���� ���� ������ ����
// PARM :	1 . pData - ���� ���ڰ�
// RETV : ���� �����
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
// DESC : -= ���� ���� ������ ���� (bool Ÿ�Կ� Ưȭ��)
// PARM :	1 . pData - ���� ���ڰ�
// RETV : ���� �����
// PRGM : B4nFter
// P.S.>
//		- ���� bool ������ Ÿ�Կ��� �������� �ʴ� �����̹Ƿ� �⺻���� ��ȯ (���� ��ü�� ������� �ʵ��� �� ��)
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
// DESC : *= ���� ���� ������ ����
// PARM :	1 . pData - ���� ���ڰ�
// RETV : ���� �����
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
// DESC : *= ���� ���� ������ ���� (bool Ÿ�Կ� Ưȭ��)
// PARM :	1 . pData - ���� ���ڰ�
// RETV : ���� �����
// PRGM : B4nFter
// P.S.>
//		- ���� bool ������ Ÿ�Կ��� �������� �ʴ� �����̹Ƿ� �⺻���� ��ȯ (���� ��ü�� ������� �ʵ��� �� ��)
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
// DESC : /= ���� ���� ������ ����
// PARM :	1 . pData - ���� ���ڰ�
// RETV : ���� �����
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
// DESC : /= ���� ���� ������ ���� (bool Ÿ�Կ� Ưȭ��)
// PARM :	1 . pData - ���� ���ڰ�
// RETV : ���� �����
// PRGM : B4nFter
// P.S.>
//		- ���� bool ������ Ÿ�Կ��� �������� �ʴ� �����̹Ƿ� �⺻���� ��ȯ (���� ��ü�� ������� �ʵ��� �� ��)
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
// DESC : %= ���� ���� ������ ����
// PARM :	1 . pData - ���� ���ڰ�
// RETV : ���� �����
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
// DESC : %= ���� ���� ������ ���� (bool Ÿ�Կ� Ưȭ��)
// PARM :	1 . pData - ���� ���ڰ�
// RETV : ���� �����
// PRGM : B4nFter
// P.S.>
//		- ���� bool ������ Ÿ�Կ��� �������� �ʴ� �����̹Ƿ� �⺻���� ��ȯ (���� ��ü�� ������� �ʵ��� �� ��)
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
// DESC : %= ���� ���� ������ ���� (float Ÿ�Կ� Ưȭ��)
// PARM :	1 . pData - ���� ���ڰ�
// RETV : ���� �����
// PRGM : B4nFter
// P.S.>
//		- ���� �Ǽ��� ������ Ÿ�Կ��� �������� �ʴ� �����̹Ƿ� �⺻���� ��ȯ (���� ��ü�� ������� �ʵ��� �� ��)
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
// DESC : %= ���� ���� ������ ���� (double Ÿ�Կ� Ưȭ��)
// PARM :	1 . pData - ���� ���ڰ�
// RETV : ���� �����
// PRGM : B4nFter
// P.S.>
//		- ���� �Ǽ��� ������ Ÿ�Կ��� �������� �ʴ� �����̹Ƿ� �⺻���� ��ȯ (���� ��ü�� ������� �ʵ��� �� ��)
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
// DESC : %= ���� ���� ������ ���� (long double Ÿ�Կ� Ưȭ��)
// PARM :	1 . pData - ���� ���ڰ�
// RETV : ���� �����
// PRGM : B4nFter
// P.S.>
//		- ���� �Ǽ��� ������ Ÿ�Կ��� �������� �ʴ� �����̹Ƿ� �⺻���� ��ȯ (���� ��ü�� ������� �ʵ��� �� ��)
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
// DESC : &= ���� ���� ������ ����
// PARM :	1 . pData - ���� ���ڰ�
// RETV : ���� �����
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
// DESC : &= ���� ���� ������ ���� (float Ÿ�Կ� Ưȭ��)
// PARM :	1 . pData - ���� ���ڰ�
// RETV : ���� �����
// PRGM : B4nFter
// P.S.>
//		- ���� �Ǽ��� ������ Ÿ�Կ��� �������� �ʴ� �����̹Ƿ� �⺻���� ��ȯ (���� ��ü�� ������� �ʵ��� �� ��)
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
// DESC : &= ���� ���� ������ ���� (double Ÿ�Կ� Ưȭ��)
// PARM :	1 . pData - ���� ���ڰ�
// RETV : ���� �����
// PRGM : B4nFter
// P.S.>
//		- ���� �Ǽ��� ������ Ÿ�Կ��� �������� �ʴ� �����̹Ƿ� �⺻���� ��ȯ (���� ��ü�� ������� �ʵ��� �� ��)
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
// DESC : &= ���� ���� ������ ���� (long double Ÿ�Կ� Ưȭ��)
// PARM :	1 . pData - ���� ���ڰ�
// RETV : ���� �����
// PRGM : B4nFter
// P.S.>
//		- ���� �Ǽ��� ������ Ÿ�Կ��� �������� �ʴ� �����̹Ƿ� �⺻���� ��ȯ (���� ��ü�� ������� �ʵ��� �� ��)
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
// DESC : |= ���� ���� ������ ����
// PARM :	1 . pData - ���� ���ڰ�
// RETV : ���� �����
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
// DESC : |= ���� ���� ������ ���� (float Ÿ�Կ� Ưȭ��)
// PARM :	1 . pData - ���� ���ڰ�
// RETV : ���� �����
// PRGM : B4nFter
// P.S.>
//		- ���� �Ǽ��� ������ Ÿ�Կ��� �������� �ʴ� �����̹Ƿ� �⺻���� ��ȯ (���� ��ü�� ������� �ʵ��� �� ��)
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
// DESC : |= ���� ���� ������ ���� (double Ÿ�Կ� Ưȭ��)
// PARM :	1 . pData - ���� ���ڰ�
// RETV : ���� �����
// PRGM : B4nFter
// P.S.>
//		- ���� �Ǽ��� ������ Ÿ�Կ��� �������� �ʴ� �����̹Ƿ� �⺻���� ��ȯ (���� ��ü�� ������� �ʵ��� �� ��)
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
// DESC : |= ���� ���� ������ ���� (long double Ÿ�Կ� Ưȭ��)
// PARM :	1 . pData - ���� ���ڰ�
// RETV : ���� �����
// PRGM : B4nFter
// P.S.>
//		- ���� �Ǽ��� ������ Ÿ�Կ��� �������� �ʴ� �����̹Ƿ� �⺻���� ��ȯ (���� ��ü�� ������� �ʵ��� �� ��)
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
// DESC : ^= ���� ���� ������ ����
// PARM :	1 . pData - ���� ���ڰ�
// RETV : ���� �����
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
// DESC : ^= ���� ���� ������ ���� (float Ÿ�Կ� Ưȭ��)
// PARM :	1 . pData - ���� ���ڰ�
// RETV : ���� �����
// PRGM : B4nFter
// P.S.>
//		- ���� �Ǽ��� ������ Ÿ�Կ��� �������� �ʴ� �����̹Ƿ� �⺻���� ��ȯ (���� ��ü�� ������� �ʵ��� �� ��)
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
// DESC : ^= ���� ���� ������ ���� (double Ÿ�Կ� Ưȭ��)
// PARM :	1 . pData - ���� ���ڰ�
// RETV : ���� �����
// PRGM : B4nFter
// P.S.>
//		- ���� �Ǽ��� ������ Ÿ�Կ��� �������� �ʴ� �����̹Ƿ� �⺻���� ��ȯ (���� ��ü�� ������� �ʵ��� �� ��)
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
// DESC : ^= ���� ���� ������ ���� (long double Ÿ�Կ� Ưȭ��)
// PARM :	1 . pData - ���� ���ڰ�
// RETV : ���� �����
// PRGM : B4nFter
// P.S.>
//		- ���� �Ǽ��� ������ Ÿ�Կ��� �������� �ʴ� �����̹Ƿ� �⺻���� ��ȯ (���� ��ü�� ������� �ʵ��� �� ��)
//*---------------------------------------------------------------
/* export */ template<>
inline long double CMemoryShuffleDataType<long double>::operator^=(const long double& pData)
{
	MSDT_ASSERT(0);		// "Can't be Called!"

	return(long double());
}
#endif	// SW_MSDT_NOTINCLUDE_COMPILESAFETY

