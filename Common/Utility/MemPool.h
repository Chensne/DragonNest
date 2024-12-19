
#pragma once

//LFH
class CLfhHeap
{
public:
	CLfhHeap() { m_hLfhHeap = NULL; }
	~CLfhHeap();

	static CLfhHeap * GetInstance();
	bool InitPool();

	inline void * _Alloc(size_t size)
	{
		return HeapAlloc(m_hLfhHeap, 0, size);
	}
	inline void _DeAlloc(void * memblock)
	{
		HeapFree(m_hLfhHeap, 0, memblock);
	}

private:
	HANDLE m_hLfhHeap;
};

/*#####################################################################################################################
	Boost MemPool
#####################################################################################################################*/

#include "./boost/pool/pool_alloc.hpp"

#if !defined(_TOOLCOMPILE)
#define USE_BOOST_MEMPOOL

template< typename T, typename TMutex=boost::details::pool::default_mutex >
class TBoostMemoryPool
{
public:
	static void* operator new( size_t )
	{
		return boost::fast_pool_allocator<T,boost::default_user_allocator_new_delete,TMutex>::allocate( 1 );
	}
	static void operator delete( void* p )
	{
		boost::fast_pool_allocator<T,boost::default_user_allocator_new_delete,TMutex>::deallocate( (boost::fast_pool_allocator<T>::pointer)p, 1 );
	}
	static void* operator new[]( size_t size )
	{
		return boost::fast_pool_allocator<T,boost::default_user_allocator_new_delete,TMutex>::allocate( size );
	}
	static void operator delete[]( void* p, size_t size )
	{
		boost::fast_pool_allocator<T,boost::default_user_allocator_new_delete,TMutex>::deallocate( (boost::fast_pool_allocator<T>::pointer)p, size );
	}
};

#define DNVector(T)		std::vector<T>
#define DNList(T)		std::list<T>
#define DNFastVector(T)	std::vector<T>
#define DNFastList(T)	std::list<T>

#else

#define DNVector(T)		std::vector<T>
#define DNList(T)		std::list<T>
#define DNFastVector(T)	std::vector<T>
#define DNFastList(T)	std::list<T>

template< typename T, typename TMutex=boost::details::pool::default_mutex >
class TBoostMemoryPool
{
};

#endif
