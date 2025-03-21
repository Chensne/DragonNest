#pragma once
#include <list>
#include <vector>
#include <algorithm>

//----------------------------------------------------------------------------
class SyncObject
{
public:
	SyncObject();
	~SyncObject();

	void		Lock();
	void		Unlock();

private:
	CRITICAL_SECTION	m_cs;
};

//----------------------------------------------------------------------------
template < class _T >
class SyncObjectGuard
{
public:
	SyncObjectGuard( _T& LockObj )
		: m_MonitorObj( &LockObj )
	{
		m_MonitorObj->Lock();
	}

	~SyncObjectGuard()
	{
		m_MonitorObj->Unlock();
	}

private:
	_T*		m_MonitorObj;
};


template<typename _T, int RESERVE_SIZE = 0 >
class MemPool
{
public:
	MemPool()
		: m_pReserveData(NULL), m_nReserveSize(0), m_nUsedReserveSize(0), m_nInitCapacity(0), m_nTotalAllocatedSize(0)
	{
		Guard<SyncObject>  Sync(m_CS);
		if ( RESERVE_SIZE > 0 )
		{
			Reserve( RESERVE_SIZE );
		}
	}

	virtual ~MemPool()
	{
		SyncObjectGuard<SyncObject> Sync(m_CS);
		Destroy();
	}

	// 예약공간을 할당한다.
	void	Reserve(int nSize)
	{
		SyncObjectGuard<SyncObject> Sync(m_CS);
		m_nUsedReserveSize = 0;
		m_nReserveSize = nSize;

		if ( m_pReserveData )
		{
			delete[] m_pReserveData;
			m_pReserveData = NULL;
		}

		if ( m_nReserveSize > 0 )
		{
			m_pReserveData = new BYTE[sizeof(_T)*m_nReserveSize];
		}
	}

	void InitCapacity(UINT nCapacity)
	{
		SyncObjectGuard<SyncObject> Sync(m_CS);
		m_nInitCapacity = nCapacity;

		m_AllocateData.reserve(m_nInitCapacity);
		m_FreeData.reserve(m_nInitCapacity);
	}

	// 예약공간 제거
	void	Destroy()
	{
		SyncObjectGuard<SyncObject> Sync(m_CS);
		if ( m_pReserveData )
		{
			delete[] m_pReserveData;
			m_pReserveData = NULL;
		}

		m_nUsedReserveSize = 0;
		m_nReserveSize = 0;

		std::for_each(m_AllocateData.begin(), m_AllocateData.end(), __DeleteAllocatedData );

		m_AllocateData.clear();
		m_FreeData.clear();

	}

	_T*	Alloc()
	{
		SyncObjectGuard<SyncObject> Sync(m_CS);
		++m_nTotalAllocatedSize;
		_T* pData = NULL;

		if ( m_FreeData.empty() )
		{
			// 예약공간에 남아있다면 
			if ( m_nUsedReserveSize < m_nReserveSize )
			{
				pData = ((_T*)m_pReserveData) + (m_nUsedReserveSize++);
			}

			// 없다면 생성해서 준다.
			else
			{
				pData = __AllocateData();
				m_AllocateData.push_back(pData);
			}

			return pData;
		}


		pData = m_FreeData.back();
		m_FreeData.pop_back();
		return pData;

	}

	inline void Free(_T* pData)
	{
		SyncObjectGuard<SyncObject> Sync(m_CS);
		--m_nTotalAllocatedSize;
		m_FreeData.push_back(pData);

	}

	inline DWORD GetAllocatedCount()
	{
		SyncObjectGuard<SyncObject> Sync(m_CS);
		return m_nTotalAllocatedSize;
	}

private:

	static _T* __AllocateData()
	{
		return (_T*)::operator new(sizeof(_T));
	}

	static void __DeleteAllocatedData(_T* pData)
	{
		::operator delete(pData);          
	}

private:
	// 미리 할당한 공간 
	BYTE*	m_pReserveData;

	// 추가 할당분 
	std::list<_T*>   m_AllocateData;
	std::vector<_T*> m_FreeData;

	// 예약공간 크기
	DWORD m_nReserveSize;
	// 사용한 예약공간 크기 
	DWORD m_nUsedReserveSize;
	// 추가 할당분을 위한 예약공간초기화 크기..-_-
	DWORD m_nInitCapacity;
	// 전체 Alloc 갯수
	DWORD m_nTotalAllocatedSize;

	// 멀티쓰레드 락
	SyncObject	m_CS;

};


template < class _T, int RESERVE_SIZE = 0 >
class ObjectPool
{
	typedef MemPool<_T, RESERVE_SIZE>		SELF_TYPE;

public:
	ObjectPool()
	{

	}

	virtual ~ObjectPool()
	{

	}

	static _T* Alloc()
	{
		return ms_Pool.Alloc();
	}

	static void Free(_T* pData)
	{
		ms_Pool.Free(pData);
	}

	void * operator new(size_t mem_size)
	{
		return ms_Pool.Alloc();
	}

	void operator delete(void* pT)
	{
		ms_Pool.Free((_T*)pT);
	}

	static SELF_TYPE& GetSharedPool()
	{
		return ms_Pool;
	}

protected:
	static SELF_TYPE ms_Pool;
};

template <class T, int RESERVE_SIZE> 
MemPool<T, RESERVE_SIZE>  ObjectPool<T, RESERVE_SIZE>::ms_Pool;

