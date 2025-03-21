#include "StdAfx.h"
#include "EtSaveMat.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CEtSaveMat g_EtSaveMat;
CSyncLock CEtSaveMat::s_SaveMatLock;

void CEtSaveMat::Clear() 
{ 
	ScopeLock< CSyncLock > Lock( s_SaveMatLock );

	m_nCurrentStartIndex += ( int )m_vecSaveMat.size();
	if( m_nCurrentStartIndex > INT_MAX / 2 )
	{
		m_nCurrentStartIndex = 0;
	}
	m_vecSaveMat.clear(); 
	m_vecTransMat.clear();
	m_vecTransCount.clear();
}

bool CEtSaveMat::IsValidIndex( int nIndex )
{
	if( ( nIndex < m_nCurrentStartIndex ) || ( nIndex >= m_nCurrentStartIndex + ( int )m_vecSaveMat.size() ) )
	{
		return false;
	}

	return true;
}

int CEtSaveMat::LockMatrix( int nCount, EtMatrix **pLockedMatrix )
{
	// 두군데에서 동시에 불리는 경우 있다..
	ScopeLock< CSyncLock > Lock( s_SaveMatLock );

	int nOriginSize = ( int )m_vecSaveMat.size();
	int nResult = nOriginSize + m_nCurrentStartIndex;
	m_vecSaveMat.resize( nOriginSize + nCount );
	m_vecTransMat.resize( nOriginSize + nCount );
	m_vecTransCount.resize( nOriginSize + nCount );
	m_vecTransCount[ nOriginSize ] = nCount;
	*pLockedMatrix = &m_vecSaveMat[ nOriginSize ];
	return nResult;
}

// 물렌더링 시에는 m_bSaveTransMat 을 false 로 해서 다음번에 재계산하도록 한다.
void CEtSaveMat::Transform( int nIndex , EtMatrix &TransMat )
{
	int &nBoneCount = m_vecTransCount[ nIndex - m_nCurrentStartIndex ];
	ASSERT( nBoneCount != 0 );
	if( nBoneCount > 0 ) {
		for( int i = 0; i < nBoneCount; i++) {
			int index = i + nIndex - m_nCurrentStartIndex;
			EtMatrixMultiply( &m_vecTransMat[ index ] , &m_vecSaveMat[ index ], &TransMat);
		}
		if( m_bSaveTransMat ) {
			nBoneCount = -nBoneCount;	// marking 
		}
	}
}

void CEtSaveMat::FlushBuffer()
{
	ScopeLock< CSyncLock > Lock( s_SaveMatLock );

	SAFE_DELETE_VEC( m_vecSaveMat );
	SAFE_DELETE_VEC( m_vecTransMat );
	SAFE_DELETE_VEC( m_vecTransCount );
	m_nCurrentStartIndex = 0;
}