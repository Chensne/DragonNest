#include "StdAfx.h"
#include "EtCustomMeshStream.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CEtCustomMeshStream::CEtCustomMeshStream(void)
{
	m_pVB = NULL;
	m_pLockedVB = NULL;
	m_pIB = NULL;
	m_pwLockedIB = NULL;
	m_nIndexCount = 0;
	m_nVertexCount = 0;
	m_nStride = 0;

	m_nVertexBufferSize = 0;
	m_nIndexBufferSize = 0;

	m_Type = PT_TRIANGLELIST;
}

CEtCustomMeshStream::~CEtCustomMeshStream(void)
{	
	Clear();
}

void CEtCustomMeshStream::Clear()
{
	ReleaseVB();
	ReleaseIB();
	
	m_nIndexCount = 0;
	m_nVertexCount = 0;
	m_nStride = 0;
	m_nVertexBufferSize = 0;
	m_nIndexBufferSize = 0;

	m_Type = PT_TRIANGLELIST;
}

char *CEtCustomMeshStream::LockVB()
{
	if( m_pLockedVB )
	{
		return m_pLockedVB;
	}
	m_pVB->Lock( 0, m_nVertexBufferSize, ( void ** )&m_pLockedVB, 0 );
	return m_pLockedVB;
}

void CEtCustomMeshStream::UnlockVB()
{
	if( m_pLockedVB )
	{
		m_pVB->Unlock();
		m_pLockedVB = NULL;
	}
}

void CEtCustomMeshStream::ReleaseVB()
{
	UnlockVB();
	SAFE_RELEASE( m_pVB );
}

WORD *CEtCustomMeshStream::LockIB()
{
	if( m_pwLockedIB )
	{
		return m_pwLockedIB;
	}
	m_pIB->Lock( 0, m_nIndexBufferSize, ( void ** )&m_pwLockedIB, 0 );
	return m_pwLockedIB;
}

void CEtCustomMeshStream::UnlockIB()
{
	if( m_pwLockedIB )
	{
		m_pIB->Unlock();
		m_pwLockedIB = NULL;
	}
}

void CEtCustomMeshStream::ReleaseIB()
{
	UnlockIB();
	SAFE_RELEASE( m_pIB);
}

void CEtCustomMeshStream::LoadVertexStream( CStream *pStream, int nVertexCount, int nStride )
{
	bool bCreate = true;

	if( m_nVertexBufferSize >= nVertexCount * nStride )
	{
		bCreate = false;
	}
	if( bCreate )
	{
		ReleaseVB();
		m_nVertexBufferSize = nVertexCount * nStride;
		m_pVB = GetEtDevice()->CreateVertexBuffer( m_nVertexBufferSize, 0 );
	}
	m_nVertexCount = nVertexCount;
	m_nStride = nStride;

	char *pBuffer = LockVB();
	pStream->Read( pBuffer, nStride * nVertexCount );
}

void CEtCustomMeshStream::AddVertexStream( CStream *pStream, int nVertexCount, int nStride )
{
	bool bCreate = true;

	if( ( m_nStride != 0 ) && ( m_nStride != nStride ) )
	{
		ASSERT( 0 && "Invalid Usage Dynamic VertexBuffer" );
		return;
	}
	if( m_nVertexBufferSize >= m_nVertexCount * m_nStride + nVertexCount * nStride )
	{
		bCreate = false;
	}

	char *pTargetBuf;
	if( bCreate )
	{
		m_nVertexBufferSize = m_nVertexCount * m_nStride + nVertexCount * nStride;
		EtVertexBuffer *pNewVB = GetEtDevice()->CreateVertexBuffer( m_nVertexBufferSize, 0 );
		if( m_nVertexCount > 0 )
		{
			char *pSourBuf = LockVB();
			pNewVB->Lock( 0, m_nVertexBufferSize, ( void ** )&pTargetBuf, 0 );
			memcpy( pTargetBuf, pSourBuf, m_nVertexCount * m_nStride );
			pNewVB->Unlock();
		}
		ReleaseVB();
		m_pVB = pNewVB;
	}

	pTargetBuf = LockVB();
	if( !pTargetBuf ) {
		return;
	}
	pStream->Read( pTargetBuf + m_nVertexCount * m_nStride, nStride * nVertexCount );
	m_nVertexCount += nVertexCount;
	m_nStride = nStride;
}

void CEtCustomMeshStream::LoadIndexStream( CStream *pStream, int nIndexCount )
{
	bool bCreate = true;

	if( m_nIndexBufferSize >= ( int )( nIndexCount * sizeof( WORD ) ) )
	{
		bCreate = false;
	}
	if( bCreate )
	{
		ReleaseIB();
		m_nIndexBufferSize = nIndexCount * sizeof( WORD );
		m_pIB = GetEtDevice()->CreateIndexBuffer( m_nIndexBufferSize );
	}

	m_nIndexCount = nIndexCount;

	WORD *pwBuffer;

	pwBuffer = LockIB();
	pStream->Read( pwBuffer, sizeof( WORD ) * nIndexCount );
}

void CEtCustomMeshStream::AddIndexStream( CStream *pStream, int nIndexCount )
{
	bool bCreate = true;

	if( m_nIndexBufferSize >= ( int )( ( nIndexCount + m_nIndexCount ) * sizeof( WORD ) ) )
	{
		bCreate = false;
	}
	WORD *pTargetBuf;
	if( bCreate )
	{
		m_nIndexBufferSize = ( m_nIndexCount + nIndexCount ) * sizeof( WORD );
		EtIndexBuffer *pNewIB = GetEtDevice()->CreateIndexBuffer( m_nIndexBufferSize );
		if( m_nIndexCount > 0 )
		{
			WORD *pSourBuf = LockIB();
			pNewIB->Lock( 0, m_nIndexBufferSize, ( void ** )&pTargetBuf, 0 );
			memcpy( pTargetBuf, pSourBuf, m_nIndexCount * sizeof( WORD ) );
			pNewIB->Unlock();
		}
		ReleaseIB();
		m_pIB = pNewIB;
	}

	pTargetBuf = LockIB();
	pStream->Read( pTargetBuf + m_nIndexCount, nIndexCount * sizeof( WORD ) );
	m_nIndexCount += nIndexCount;
}

void CEtCustomMeshStream::Draw( int nVertexDeclIndex, int nDrawStart, int nDrawCount )
{
	if( !m_pVB )
	{
		return;
	}

	if( nVertexDeclIndex != -1 )
	{
		GetEtDevice()->SetVertexDeclaration( nVertexDeclIndex );
	}

	UnlockVB();
	UnlockIB();

	GetEtDevice()->SetStreamSource( 0, m_pVB, m_nStride );
	if( m_pIB )
	{
		switch( m_Type )
		{
			case PT_POINTLIST:
				if( nDrawCount == 0 )
				{
					nDrawCount = m_nIndexCount;
				}
				break;
			case PT_LINELIST:
				if( nDrawCount == 0 )
				{
					nDrawCount = m_nIndexCount / 2;
				}
				nDrawStart = nDrawStart * 2;
				break;
			case PT_LINESTRIP:
				if( nDrawCount == 0 )
				{
					nDrawCount = m_nIndexCount - 1;
				}
				break;
			case PT_TRIANGLELIST:
				if( nDrawCount == 0 )
				{
					nDrawCount = m_nIndexCount / 3;
				}
				nDrawStart = nDrawStart * 3;
				break;
			case PT_TRIANGLESTRIP:
				if( nDrawCount == 0 )
				{
					nDrawCount = m_nIndexCount - 2;
				}
				break;
			case PT_TRIANGLEFAN:
				if( nDrawCount == 0 )
				{
					nDrawCount = m_nIndexCount - 2;
				}
				nDrawStart = 0;
				break;
		}

		GetEtDevice()->SetIndexBuffer( m_pIB );
		GetEtDevice()->DrawIndexedPrimitive( m_Type, 0, m_nVertexCount, nDrawStart, nDrawCount );
	}
	else
	{
		switch( m_Type )
		{
			case PT_POINTLIST:
				if( nDrawCount == 0 )
				{
					nDrawCount = m_nVertexCount;
				}
				break;
			case PT_LINELIST:
				if( nDrawCount == 0 )
				{
					nDrawCount = m_nVertexCount / 2;
				}
				nDrawStart = nDrawStart * 2;
				break;
			case PT_LINESTRIP:
				if( nDrawCount == 0 )
				{
					nDrawCount = m_nVertexCount - 1;
				}
				break;
			case PT_TRIANGLELIST:
				if( nDrawCount == 0 )
				{
					nDrawCount = m_nVertexCount / 3;
				}
				nDrawStart = nDrawStart * 3;
				break;
			case PT_TRIANGLESTRIP:
				if( nDrawCount == 0 )
				{
					nDrawCount = m_nVertexCount - 2;
				}
				break;
			case PT_TRIANGLEFAN:
				if( nDrawCount == 0 )
				{
					nDrawCount = m_nVertexCount - 2;
				}
				nDrawStart = 0;
				break;
		}
		GetEtDevice()->DrawPrimitive( m_Type, nDrawStart, nDrawCount );
	}
}
