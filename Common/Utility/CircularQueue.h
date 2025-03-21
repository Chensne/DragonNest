#pragma once

template < class T >
class CCircularQueue
{
public:
	CCircularQueue( int nQueueSize )
	{
		m_nQueueSize = nQueueSize;
		m_pQueueData = new T[ nQueueSize ];
		m_nDataCount = 0;
		m_nHead = 0;
		m_nTail = 0;
	}
	virtual ~CCircularQueue()
	{
		if( m_pQueueData )
		{
			delete [] m_pQueueData;
			m_pQueueData = NULL;
		}
	}

private:
	T *m_pQueueData;
	int m_nQueueSize;
	int m_nDataCount;
	int m_nHead;
	int m_nTail;

public:
	bool Push( T &Data )
	{
		if( m_nDataCount >= m_nQueueSize )
		{
			return false;
		}

		m_pQueueData[ m_nTail ] = Data;
		m_nTail = ( m_nTail + 1 ) % m_nQueueSize;
		m_nDataCount++;

		return true;
	}

	bool Pop( T &Data )
	{
		if( m_nDataCount <= 0 )
		{
			return false;
		}

		Data = m_pQueueData[ m_nHead ];
		m_nHead = ( m_nHead + 1) % m_nQueueSize;
		m_nDataCount--;

		return true;
	}

	bool IsEmpty() { return m_nDataCount == 0; }
	int Size() { return m_nDataCount; }

	T operator[] ( int nIndex )
	{
		ASSERT( nIndex < m_nDataCount );

		int nNewIndex;

		nNewIndex = ( m_nHead + nIndex ) % m_nQueueSize;

		return m_pQueueData[ nNewIndex ];
	}
};