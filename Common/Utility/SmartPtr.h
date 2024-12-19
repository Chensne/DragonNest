#pragma once

#include < vector >
#include <algorithm>
#include <windows.h>
#include "CriticalSection.h"

//#ifdef _DEBUG
#define SMARTPTR_VALID_CHECK
//#endif //_DEBUG

template < class T >
class CSmartPtr
{
public:
	CSmartPtr() 
	{	
		Identity();
	}
	CSmartPtr( _int64 nHandle, T *pInstance )
	{
		m_nHandle = nHandle;
		m_pInstance = pInstance;
	}
	template< class T2 >
	CSmartPtr( CSmartPtr< T2 > &Ptr ) 
	{
		m_nHandle = Ptr.m_nHandle;
		m_pInstance = reinterpret_cast< T * >( Ptr.m_pInstance );
	}
	~CSmartPtr() {}

public:
	union
	{
		_int64 m_nHandle;
		struct
		{
			int m_nIndex;
			int m_nID;
		};
	};
	T *m_pInstance;

public:
	void Identity() 
	{
		m_nHandle = 0xffffffffffffffff;
		m_pInstance = 0;
	}

	_int64 GetHandle() { return m_nHandle; }
	int GetIndex()	const { return m_nIndex; }
	int GetID()		{ return m_nID; }
	T *GetPointer() { return m_pInstance; }
	operator T*()
	{
		if( m_pInstance == NULL ) return false;
		if( !m_pInstance->IsValidSmartPtr( m_nHandle, m_pInstance ) )
		{
			return NULL;
		}
		return m_pInstance;
	}
	bool operator!() const
	{
		if( m_pInstance == NULL ) return true;
		if( !m_pInstance->IsValidSmartPtr( m_nHandle, m_pInstance ) )
		{
			return true;
		}

		return false;
	}
	T *operator->() const
	{
		if( m_pInstance )
		{
#ifdef SMARTPTR_VALID_CHECK
			if( !m_pInstance->IsValidSmartPtr( m_nHandle, m_pInstance ) )
			{
				ASSERT( 0 );
				// 강제 크래쉬 발생
				int* p = NULL;
				*p = 0xfefe;
			}
#endif
		}
		return( m_pInstance );
	}
	T &operator*()
	{
		if( m_pInstance )
		{
#ifdef SMARTPTR_VALID_CHECK
			if( !m_pInstance->IsValidSmartPtr( m_nHandle, m_pInstance ) )
			{
				ASSERT( 0 );
				// 강제 크래쉬 발생
				int* p = NULL;
				*p = 0xfefe;
			}
#endif
		}
		return( *m_pInstance );
	}

	template < class T2 >
	CSmartPtr< T > &operator = ( const CSmartPtr< T2 > &Sour )
	{
		m_nHandle = Sour.m_nHandle;
		m_pInstance = reinterpret_cast< T * >( Sour.m_pInstance );

		return *this;
	}
	template < class T2 >
	bool operator == ( const CSmartPtr< T2 > &Sour ) const
	{
		if( ( m_nHandle == Sour.m_nHandle ) && (m_pInstance == reinterpret_cast< T * >( Sour.m_pInstance ) ) )
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	template < class T2 >
	bool operator!= ( const CSmartPtr< T2 > &Sour ) const
	{ 
		return( !( *this == Sour ) );
	}
};

template < class T >
class CSmartPtrMng
{
public:
	static int s_nDefaultPoolSize;
	CSmartPtrMng( int nPoolSize = 1 )
	{
		Reset( ( nPoolSize == 1 ) ? s_nDefaultPoolSize : nPoolSize  );
	}
	~CSmartPtrMng()
	{
		// 외부에서 생성된 포인터들이기 때문에 CSmartPtrMng가 해제의 책임을 질필요는 없는 듯 하다.
		/*		int i;

		for( i = 0; i < ( int )m_vecSmartPtrs.size(); i++ )
		{
		if( m_vecSmartPtrs[ i ] )
		{
		delete m_vecSmartPtrs[ i ];
		}
		}*/
	}
	void Reset( int nPoolSize )
	{
		ScopeLock<CSyncLock> Lock(m_Lock, m_bUseLock);
		int i;

		m_vecSmartPtrs.resize( nPoolSize );
		m_vecEmptySlots.resize( nPoolSize );
		for( i = 0; i <nPoolSize ; i++ )
		{
			m_vecSmartPtrs[ i ] = CSmartPtr< T >( 0xffffffff00000000 + i, 0 );
			m_vecEmptySlots[ i ] = nPoolSize - i - 1;
		}
		m_vecUsedSlots.clear();
	}

protected:
	std::vector< CSmartPtr< T > > m_vecSmartPtrs;
	std::vector< int > m_vecEmptySlots;
	std::vector< int > m_vecUsedSlots;
	CSyncLock m_Lock;
	bool m_bUseLock;

public:
	void UseLock( bool bUse ) { m_bUseLock = bUse; }
	CSmartPtr< T > operator[] ( int nID )
	{
		ScopeLock<CSyncLock> Lock(m_Lock, m_bUseLock);
		return m_vecSmartPtrs[ nID ];
	}
	int BufferSize()
	{
		ScopeLock<CSyncLock> Lock(m_Lock, m_bUseLock);
		return ( int )m_vecSmartPtrs.size();
	}
	void SwapItemIndex( int nItemIndex1, int nItemIndex2 )
	{
		ScopeLock<CSyncLock> Lock(m_Lock, m_bUseLock);
		int nBackup;
		nBackup = m_vecUsedSlots[ nItemIndex1 ];
		m_vecUsedSlots[ nItemIndex1 ] = m_vecUsedSlots[ nItemIndex2 ];
		m_vecUsedSlots[ nItemIndex2 ] = nBackup;
	}
	int GetItemCount()
	{
		ScopeLock<CSyncLock> Lock(m_Lock, m_bUseLock);
		return ( int )m_vecUsedSlots.size();
	}
	int GetItemIndex( CSmartPtr< T > &SmartPtr )
	{
		ScopeLock<CSyncLock> Lock(m_Lock, m_bUseLock);
		int i;

		for( i = 0; i < ( int )m_vecUsedSlots.size(); i++ )
		{
			if( m_vecSmartPtrs[ m_vecUsedSlots[ i ] ] == SmartPtr )
			{
				return i;
			}
		}
		ASSERT( 0 && "이런일이 생기면 안된다" ); 
		return -1;
	}
	CSmartPtr< T > GetItem( int nItemIndex )
	{
		ScopeLock<CSyncLock> Lock(m_Lock, m_bUseLock);
		return m_vecSmartPtrs[ m_vecUsedSlots[ nItemIndex ] ];
	}

	CSmartPtr< T > &CreateHandle( T *pInstance )
	{
		ScopeLock<CSyncLock> Lock(m_Lock, m_bUseLock);

		ASSERT( pInstance );

		_int64 nHandle, nID;
		int nFreeSlotIndex;

		if( m_vecEmptySlots.empty() )
		{
			m_vecEmptySlots.push_back( ( int )m_vecSmartPtrs.size() );
			m_vecSmartPtrs.push_back( CSmartPtr< T >( 0, 0 ) );
		}

		nFreeSlotIndex = m_vecEmptySlots.back();
		m_vecEmptySlots.pop_back();
		nID = m_vecSmartPtrs[ nFreeSlotIndex ].GetID();
		nID++;
		nHandle = ( nID << 32 ) + nFreeSlotIndex;
		m_vecSmartPtrs[ nFreeSlotIndex ] = CSmartPtr< T >( nHandle, pInstance );
		m_vecUsedSlots.push_back( nFreeSlotIndex );

		return m_vecSmartPtrs[ nFreeSlotIndex ];
	}

	void DeleteHandle( CSmartPtr< T > hHandle )
	{
		ScopeLock<CSyncLock> Lock(m_Lock, m_bUseLock);

		ASSERT( hHandle.GetHandle() == m_vecSmartPtrs[ hHandle.GetIndex() ].GetHandle() );
		//		if( hHandle.GetIndex() < 0 || hHandle.GetIndex() >= (int)m_vecSmartPtrs.size() ) return;

		int nIndex;
		_int64 nID;
		std::vector< int >::iterator Iter;

		nIndex = hHandle.GetIndex();
		nID = hHandle.GetID();
		m_vecSmartPtrs[ nIndex ] = CSmartPtr< T >( nID << 32, 0 );
		Iter = std::find( m_vecUsedSlots.begin(), m_vecUsedSlots.end(), nIndex );
		if( Iter != m_vecUsedSlots.end() )
		{
			m_vecUsedSlots.erase( Iter );
		}
		m_vecEmptySlots.push_back( nIndex );
	}
	void PopItem( int nItemIndex )
	{
		ScopeLock<CSyncLock> Lock(m_Lock, m_bUseLock);
		if( ( nItemIndex >= ( int )m_vecUsedSlots.size() ) && ( nItemIndex < 0 ) )
		{
			return;
		}
		int nIndex = m_vecUsedSlots[ nItemIndex ];
		_int64 nID = m_vecSmartPtrs[ nIndex ].GetID();;
		m_vecSmartPtrs[ nIndex ] = CSmartPtr< T >( nID << 32, 0 );
		m_vecUsedSlots.erase( m_vecUsedSlots.begin() + nItemIndex );
		m_vecEmptySlots.push_back( nIndex );
	}
};

class CSyncLock;
template < class T >
class CSmartPtrBase
{
public:
	CSmartPtrBase( bool bUseMngLock = true ) : m_nSmartDepth(0)
	{
		ScopeLock<CSyncLock> Lock( s_SmartPtrLock );
		s_SmartPtrMng.UseLock( bUseMngLock );
		AddToList();
	}
	virtual ~CSmartPtrBase()
	{
		ScopeLock<CSyncLock> Lock( s_SmartPtrLock );
		DeleteFromList();
	}

private:
	static CSmartPtrMng< T > s_SmartPtrMng;	// 상속받은 클래스에서 직접 액세스 하지 못하도록 private로 선언.
	static CSmartPtr< T > s_Identity;

protected:
	CSmartPtr< T > m_MySmartPtr;
	int m_nSmartDepth;
public:
	static CSyncLock s_SmartPtrLock;

public:
	virtual int Release() 
	{ 
		delete this; 
		const int nNoRefCountNeed = 0; 
		return nNoRefCountNeed;
	}
	void AddToList() { 
		if( !m_MySmartPtr ) m_MySmartPtr = s_SmartPtrMng.CreateHandle( static_cast< T * >( this ) ); 
	}
	void DeleteFromList() { 
		if( m_MySmartPtr ) { s_SmartPtrMng.DeleteHandle( m_MySmartPtr ); m_MySmartPtr.Identity(); } 
	}

	static void SwapItemIndex( int nItemIndex1, int nItemIndex2 ) { s_SmartPtrMng.SwapItemIndex( nItemIndex1, nItemIndex2 ); }
	// 실제로 할당돼 있는 slot의 갯수를 리턴한다.
	static int GetItemCount() { 
		return s_SmartPtrMng.GetItemCount(); 
	}			
	// 실제 할당돼 있는 slot의 리스트에서 가져온다. 여기의 nItemIndex는 오브젝트 삽입삭제에 의해서 변경될 수 있으므로 주의
	static CSmartPtr< T >GetItem( int nItemIndex ) { 
		return s_SmartPtrMng.GetItem( nItemIndex ); 
	}	

	int GetMyItemIndex() { return s_SmartPtrMng.GetItemIndex( m_MySmartPtr ); }
	int GetMyIndex() { return m_MySmartPtr.GetIndex(); }
	CSmartPtr< T > GetMySmartPtr() { return m_MySmartPtr; }
	// 전체 리스트에서 아이템을 가져온다. 여기의 nIndex는 오브젝트 삽입삭제와 상관없이 불변이다. 위의 GetItem() 함수와 구별해서 사용.
	static CSmartPtr< T > GetSmartPtr( int nIndex ) { 
		if( nIndex < 0 || nIndex >= s_SmartPtrMng.BufferSize() ) return Identity();
		return s_SmartPtrMng[ nIndex ]; 
	}

	static void Reset() { s_SmartPtrMng.Reset( s_SmartPtrMng.BufferSize() ); }

	static bool IsValidSmartPtr( _int64 nHandle, T *pInstnace )
	{
		if( nHandle == -1 ) return false;
		if( pInstnace == NULL ) return false;
		CSmartPtr< T > CheckSmartPtr;
		int nIndex = (int)( nHandle & 0xffffffff );
		if( nIndex < 0 || nIndex >= s_SmartPtrMng.BufferSize() )
		{
			return false;
		}
		CheckSmartPtr = s_SmartPtrMng[ nIndex ];
		if( CheckSmartPtr.GetHandle() != nHandle )
		{
			return false;
		}
		if( CheckSmartPtr.GetPointer() != pInstnace )
		{
			return false;
		}

		return true;
	}

	static void DeleteAllObject()
	{
		ScopeLock<CSyncLock> Lock( s_SmartPtrLock );
		while( s_SmartPtrMng.GetItemCount() )
		{
			CSmartPtr< T > hHandle;
			hHandle = s_SmartPtrMng.GetItem( 0 );
			if( hHandle )
			{
				int nCount = s_SmartPtrMng.GetItemCount();
				hHandle->Release();
				if( nCount == s_SmartPtrMng.GetItemCount() ) {
					s_SmartPtrMng.PopItem( 0 );
				}
			}
			else
			{
				s_SmartPtrMng.PopItem( 0 );
			}
		}
	}
	static CSmartPtr< T > Identity() { return s_Identity; }
};

#define DECL_SMART_PTR_STATIC( Name, Count) \
	int CSmartPtrMng< Name >::s_nDefaultPoolSize = Count; \
	CSyncLock CSmartPtrBase< Name >::s_SmartPtrLock; \
	CSmartPtrMng< Name > CSmartPtrBase< Name >::s_SmartPtrMng( Count ); CSmartPtr< Name > CSmartPtrBase< Name >::s_Identity;
