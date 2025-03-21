// Note : Simple List Memory Pool -_-;;;;
//		나중에 인터페이스쪽에서 사용하기 위해 추가.
//
#pragma once

#include <list>
#include <algorithm>

template<typename TYPE>
class CSLMemPool
{
protected:
	struct DeleteObject
	{
		template<typename TYPE>
		void operator() ( const TYPE *ptr ) const
		{
			ASSERT(ptr&&"CMemPool<TYPE>::operator()");
			delete ptr;
		}
	};

	typedef typename std::list<TYPE*>			MEMPOOL_LIST;
	typedef typename MEMPOOL_LIST::iterator		MEMPOOL_LIST_ITER;

	MEMPOOL_LIST m_listMemPool;

public:
	CSLMemPool();
	~CSLMemPool();

public:
	// Note : 메모리를 할당한다.
	//		풀에 있다면 풀에 있는 메모리를 반환하고 아니면 new한다.
	TYPE* Allocate();
	// Note : 메모리풀에 반환한다. 객체는 이전 값을 가진다.
	//		풀에 반환하기 전 또는 후에 초기화를 하고 사용해야 한다.
	void Deallocate( TYPE* ptr );
	// Note : 메모리풀에 반환한다. 단 객체를 생성자로 초기화 후 반환.
	void DeallocateEx( TYPE* ptr );

public:
	DWORD GetAmount()	{ return static_cast<DWORD>(m_listMemPool.size()); }
	void Clear()		{ std::for_each ( m_listMemPool.begin(), m_listMemPool.end(), DeleteObject() ); }
};

template<typename TYPE>
inline CSLMemPool<TYPE>::CSLMemPool()
{
}

template<typename TYPE>
inline CSLMemPool<TYPE>::~CSLMemPool()
{
	Clear();
}

template<typename TYPE>
inline TYPE* CSLMemPool<TYPE>::Allocate()
{
	if( m_listMemPool.empty() )
	{
		return new TYPE;
	}

	TYPE* ptr = *m_listMemPool.begin();
	m_listMemPool.pop_front();

	ASSERT(ptr&&"CMemPool<TYPE>::Allocate");
	return ptr;
}

template<typename TYPE>
inline void CSLMemPool<TYPE>::Deallocate( TYPE* ptr )
{
	ASSERT(ptr&&"CMemPool<TYPE>::Deallocate");
	m_listMemPool.push_back ( ptr );
}

template<typename TYPE>
inline void CSLMemPool<TYPE>::DeallocateEx( TYPE* ptr )
{
	ASSERT(ptr&&"CMemPool<TYPE>::DeallocateEx");
	*ptr = TYPE();
	m_listMemPool.push_back ( ptr );
}