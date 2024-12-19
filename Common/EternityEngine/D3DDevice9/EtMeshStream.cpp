#include "StdAfx.h"
#include "EtMeshStream.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CEtMeshStream::CEtMeshStream(void)
{
	m_pIB = NULL;
	m_bStrip = false;
	m_bGenerateTangentSpace = false;

	m_bDeleteVB = true;
	m_bDeleteIB = true;

	m_Usage = USAGE_DEFAULT;
	m_Pool = POOL_MANAGED;
	m_pPointSpriteTexture = NULL;

	m_nIndexCount = 0;
	m_nVertexCount = 0;
	m_pPointSpriteTexture = NULL;
}

CEtMeshStream::~CEtMeshStream(void)
{
	Clear();
}

void CEtMeshStream::Clear()
{
	int i;

	// 외부에서 생성해서 셋팅한 버퍼들은 여기서 삭제하지 않는다.
	if( m_bDeleteIB )
	{
		SAFE_RELEASE( m_pIB );
	}
	if( m_bDeleteVB )
	{
		for( i = 0; i < ( int )m_vecElements.size(); i++ )
		{
			SAFE_RELEASE( m_vecElements[ i ].m_pVB );
		}
		SAFE_DELETE_VEC( m_vecElements );
	}
}

EtVertexBuffer *CEtMeshStream::FindVertexStream( MeshStreamType Type, int nStreamIndex, int &nStride )
{
	int i, nSize;

	nSize = ( int )m_vecElements.size();
	for( i = 0; i < nSize; i++ )
	{
		if( ( m_vecElements[ i ].Type == Type ) && ( m_vecElements[ i ].nStreamIndex == nStreamIndex ) )
		{
			nStride = m_vecElements[ i ].nStride;
			return m_vecElements[ i ].m_pVB;
		}
	}

	return NULL;
}

void CEtMeshStream::GenerateTangentSpace()
{
	if( m_bGenerateTangentSpace )
	{
		return;
	}

	int i, nStride;
	EtVertexBuffer *pTangentVB, *pBinormalVB, *pPositionVB, *pNormalVB, *pTextureVB;
	SStreamElement Element;
	EtVector3 *pPosition, *pNormal, *pTangent, *pBinormal;
	EtVector2 *pTexture;
	WORD *pFace;

	pTangentVB = FindVertexStream( MST_TANGENT, 0, nStride );
	if( !pTangentVB )
	{
		nStride = sizeof( EtVector3 );
		pTangentVB = GetEtDevice()->CreateVertexBuffer( m_nVertexCount * nStride, MST_TANGENT, m_Usage, m_Pool );
		Element.m_pVB = pTangentVB;
		Element.nStreamIndex = 0;
		Element.nStride = nStride;
		Element.Type = MST_TANGENT;
		m_vecElements.push_back( Element );
	}
	pBinormalVB = FindVertexStream( MST_BINORMAL, 0, nStride );
	if( !pBinormalVB )
	{
		nStride = sizeof( EtVector3 );
		pBinormalVB = GetEtDevice()->CreateVertexBuffer( m_nVertexCount * nStride, MST_BINORMAL, m_Usage, m_Pool );
		Element.m_pVB = pBinormalVB;
		Element.nStreamIndex = 0;
		Element.nStride = nStride;
		Element.Type = MST_BINORMAL;
		m_vecElements.push_back( Element );
	}

	pPositionVB = FindVertexStream( MST_POSITION, 0, nStride );
	if( !pPositionVB )
	{
		ASSERT( 0 && "Position Vertex Stream Not Found" );
		return;
	}
	pNormalVB = FindVertexStream( MST_NORMAL, 0, nStride );
	if( !pNormalVB )
	{
		ASSERT( 0 && "Normal Vertex Stream Not Found" );
		return;
	}
	pTextureVB = FindVertexStream( MST_TEXCOORD, 0, nStride );
	if( !pTextureVB )
	{
		ASSERT( 0 && "Texture Coordinate Vertex Stream Not Found" );
		return;
	}

	pTangentVB->Lock( 0, m_nVertexCount * nStride, ( void ** )&pTangent, 0 );
	pBinormalVB->Lock( 0, m_nVertexCount * nStride, ( void ** )&pBinormal, 0 );
	pPositionVB->Lock( 0, m_nVertexCount * nStride, ( void ** )&pPosition, 0 );
	pNormalVB->Lock( 0, m_nVertexCount * nStride, ( void ** )&pNormal, 0 );
	pTextureVB->Lock( 0, m_nVertexCount * nStride, ( void ** )&pTexture, 0 );
	m_pIB->Lock( 0, sizeof( WORD ) * m_nIndexCount, ( void ** )&pFace, 0 );

	for( i = 0; i < m_nIndexCount; )
	{
		int nVert1, nVert2, nVert3;
		EtVector3 PVec, QVec;
		float s1, s2, t1, t2, fValue;
		EtVector3 CalcTangent, CalcBinormal;

		if( m_bStrip )
		{
			if( i > m_nIndexCount - 3 )
			{
				break;
			}
			if( pFace[ i + 2 ] == 0xffff )
			{
				i += 3;
			}
		}

		nVert1 = pFace[ i ];
		nVert2 = pFace[ i + 1 ];
		nVert3 = pFace[ i + 2 ];

		PVec = pPosition[ nVert2 ] - pPosition[ nVert1 ];
		QVec = pPosition[ nVert3 ] - pPosition[ nVert1 ];

		s1 = pTexture[ nVert2 ].x - pPosition[ nVert1 ].x;
		t1 = pTexture[ nVert2 ].y - pPosition[ nVert1 ].y;
		s2 = pTexture[ nVert3 ].x - pPosition[ nVert1 ].x;
		t2 = pTexture[ nVert3 ].y - pPosition[ nVert1 ].y;

		fValue = s1 * t2 - s2 * t1;
		if( fabsf( fValue ) <= 0.0001f )
		{
			fValue = 1.0f;
		}
		else
		{
			fValue = 1.0f / fValue;
		}

		CalcTangent = fValue * ( t2 * PVec - t1 * QVec );
		CalcBinormal = fValue * ( s1 * QVec - s2 * PVec );

		pTangent[ nVert1 ] += CalcTangent;
		pBinormal[ nVert1 ] += CalcBinormal;
		pTangent[ nVert2 ] += CalcTangent;
		pBinormal[ nVert2 ] += CalcBinormal;
		pTangent[ nVert3 ] += CalcTangent;
		pBinormal[ nVert3 ] += CalcBinormal;
		if( m_bStrip )
		{
			i++;
		}
		else
		{
			i += 3;
		}
	}

	for( i = 0; i < m_nVertexCount; i++ )
	{
		EtVector3 CalcTangent, CalcBinormal;
		float fLenTangent, fLenBinormal;

		EtVec3Normalize( pTangent + i, pTangent + i );
		EtVec3Normalize( pBinormal + i, pBinormal + i );

		CalcTangent = pTangent[ i ] -  ( EtVec3Dot( pNormal + i , pTangent + i )  * pNormal[ i ] );
		CalcBinormal = pBinormal[ i ] - ( EtVec3Dot( pNormal + i , pBinormal + i ) * pNormal[ i ] ) 
			- ( EtVec3Dot( &CalcTangent, pBinormal + i ) * CalcTangent );
		EtVec3Normalize( pTangent + i, &CalcTangent );
		EtVec3Normalize( pBinormal + i, &CalcBinormal );

		fLenTangent = EtVec3Length( pTangent + i );
		fLenBinormal = EtVec3Length( pBinormal + i );;
		if( ( fLenTangent <= 0.001f ) || ( fLenBinormal <= 0.001f ) )
		{	
			if( fLenTangent > 0.5f)
			{
				EtVec3Cross( pBinormal + i, pNormal + i, pTangent + i );
			}
			else if( fLenBinormal > 0.5 )
			{
				EtVec3Cross( pTangent + i , pBinormal + i, pNormal + i );
			}
			else
			{
				EtVector3 XAxis( 1.0f , 0.0f , 0.0f);
				EtVector3 YAxis( 0.0f , 1.0f , 0.0f);
				EtVector3 StartAxis;

				if( EtVec3Dot( &XAxis, pNormal + i ) < EtVec3Dot( &YAxis, pNormal + i ) )
				{
					StartAxis = XAxis;
				}
				else
				{
					StartAxis = YAxis;
				}
				EtVec3Cross( pTangent + i, pNormal + i, &StartAxis );
				EtVec3Cross( pBinormal + i, pNormal + i, pTangent + i );
			}
		}
		else
		{
			if( EtVec3Dot( pBinormal + i, pTangent + i )  > 0.999f )
			{
				D3DXVec3Cross( pBinormal + i, pNormal + i, pTangent + i );
			}
		}
	}

	pTangentVB->Unlock();
	pBinormalVB->Unlock();
	pPositionVB->Unlock();
	pNormalVB->Unlock();
	pTextureVB->Unlock();
	m_pIB->Unlock();

	m_bGenerateTangentSpace = true;
}

void CEtMeshStream::CreateVertexStream( MeshStreamType Type, int nStreamIndex, int nVertexCount )
{
	DWORD dwFVF;
	SStreamElement Element;
	int nStride;

	switch( Type )
	{
		case MST_POSITION:
			nStride = sizeof( EtVector3 );
			dwFVF = D3DFVF_XYZ;
			break;
		case MST_NORMAL:
			nStride = sizeof( EtVector3 );
			dwFVF = D3DFVF_NORMAL;
			break;
		case MST_TEXCOORD:
			nStride = sizeof( float ) * 2;
			dwFVF = D3DFVF_TEX0 + nStreamIndex;
			dwFVF <<= D3DFVF_TEXCOUNT_SHIFT;
			break;
		case MST_COLOR:
			nStride = sizeof( DWORD );
			dwFVF = D3DFVF_DIFFUSE;
			break;
		case MST_BONEINDEX:
			nStride = sizeof( short ) * 4;	
			dwFVF = 0;						// FVF 뭘로 해야 하지?
			break;
		case MST_BONEWEIGHT:
			nStride = sizeof( float ) * 4;	
			dwFVF = 0;						// FVF 뭘로 해야 하지?
			break;
		case MST_DEPTH:
			nStride = sizeof( float );
			dwFVF = 0;
			break;
		case MST_PSIZE:
			nStride = sizeof( float );
			dwFVF = D3DFVF_PSIZE;
			break;
		default:
			ASSERT( 0 && "Invalid Stream Type!!!" );
			return;
	}

	m_nVertexCount = nVertexCount;
	Element.Type = Type;
	Element.m_pVB = GetEtDevice()->CreateVertexBuffer( nVertexCount * nStride, dwFVF, m_Usage, m_Pool );
	Element.nStride = nStride;
	Element.nStreamIndex = nStreamIndex;
	m_vecElements.push_back( Element );
}

void CEtMeshStream::LoadVertexStream( CStream *pStream, MeshStreamType Type, int nStreamIndex, int nVertexCount )
{
	int nStride;
	SStreamElement Element;

	Element.m_pVB = FindVertexStream( Type, nStreamIndex, nStride );
	if( !Element.m_pVB )
	{
		CreateVertexStream( Type, nStreamIndex, nVertexCount );
		Element.m_pVB = FindVertexStream( Type, nStreamIndex, nStride );
	}

	if( pStream && Element.m_pVB )
	{
		void *pBuffer = NULL;
		Element.m_pVB->Lock( 0, nStride * nVertexCount, &pBuffer, 0 );
		if( pBuffer ) 
			pStream->Read( pBuffer, nStride * nVertexCount );
		Element.m_pVB->Unlock();
	}
	else if( pStream && Element.m_pVB == NULL )
	{
		pStream->Seek( nStride * nVertexCount, SEEK_CUR );
	}
}

void CEtMeshStream::LoadIndexStream( CStream *pStream, bool bStrip, int nIndexCount )
{
	void *pBuffer = NULL;

	m_bStrip = bStrip;
	if( !m_pIB )
	{
		m_pIB = GetEtDevice()->CreateIndexBuffer( sizeof( WORD ) * nIndexCount, m_Usage, m_Pool );
	}

	if( pStream && m_pIB )
	{
		m_pIB->Lock( 0, sizeof( WORD ) * nIndexCount, &pBuffer, 0 );
		if( pBuffer ) 
			pStream->Read( pBuffer, sizeof( WORD ) * nIndexCount );
		m_pIB->Unlock();
	}
	else if( pStream && m_pIB == NULL )
	{
		pStream->Seek( sizeof( WORD ) * nIndexCount, SEEK_CUR );
	}

	m_nIndexCount = nIndexCount;
}

int CEtMeshStream::GetVertexStream( MeshStreamType Type, void *pDest, int nStreamIndex )
{
	int nStride;
	EtVertexBuffer *pVB;
	void *pLockBuffer = NULL;

	pVB = FindVertexStream( Type, nStreamIndex, nStride );
	if( !pVB )
	{
		ASSERT( 0 && "Not Find Mesh Stream" );
		return 0;
	}
	pVB->Lock( 0, nStride * m_nVertexCount, &pLockBuffer, 0 );
	if( pLockBuffer )
		memcpy( pDest, pLockBuffer, nStride * m_nVertexCount );
	pVB->Unlock();

	return nStride;
}

void CEtMeshStream::SetVertexStream( MeshStreamType Type, void *pSource, int nStreamIndex )
{
	int nStride;
	EtVertexBuffer *pVB;
	void *pLockBuffer = NULL;

	pVB = FindVertexStream( Type, nStreamIndex, nStride );
	if( !pVB )
	{
		ASSERT( 0 && "Invalid Mesh Stream Type" );
		return;
	}
	pVB->Lock( 0, nStride * m_nVertexCount, &pLockBuffer, 0 );
	if( pLockBuffer )
		memcpy( pLockBuffer, pSource, nStride * m_nVertexCount );
	pVB->Unlock();
}

void CEtMeshStream::GetIndexStream( WORD *pDest )
{
	void *pLockBuffer = NULL;
	if( m_pIB ) {
		m_pIB->Lock( 0, sizeof( WORD ) * m_nIndexCount, &pLockBuffer, D3DLOCK_READONLY );
		if( pLockBuffer != NULL )
			memcpy( pDest, pLockBuffer, sizeof( WORD ) * m_nIndexCount );
		m_pIB->Unlock();
	}
}

void CEtMeshStream::SetIndexStream( WORD *pSource )
{
	void *pLockBuffer = NULL;
	if( m_pIB ) {
		m_pIB->Lock( 0, sizeof( WORD ) * m_nIndexCount, &pLockBuffer, 0 );
		if( pLockBuffer ) {
			memcpy( pLockBuffer, pSource, sizeof( WORD ) * m_nIndexCount );
		}
		m_pIB->Unlock();
	}
}

void *CEtMeshStream::Lock( MeshStreamType Type, int nStreamIndex, DWORD dwFlag, int &nStride )
{
	EtVertexBuffer *pVB;
	void *pLockBuffer = NULL;

	pVB = FindVertexStream( Type, nStreamIndex, nStride );
	if( !pVB )
	{
		ASSERT( 0 && "Invalid Mesh Stream Type" );
		return NULL;
	}
	pVB->Lock( 0, nStride * m_nVertexCount, &pLockBuffer, dwFlag );

	return pLockBuffer;
}

void CEtMeshStream::Unlock( MeshStreamType Type, int nStreamIndex )
{
	EtVertexBuffer *pVB;
	int nStride;

	pVB = FindVertexStream( Type, nStreamIndex, nStride );
	if( !pVB )
	{
		ASSERT( 0 && "Invalid Mesh Stream Type" );
		return;
	}
	pVB->Unlock();
}

void CEtMeshStream::SetIndexBuffer( EtIndexBuffer *pBuffer, int nIndexCount ) 
{ 
	if( m_bDeleteIB ) {
		SAFE_RELEASE( m_pIB );
	}
	m_pIB = pBuffer; 
	m_bDeleteIB = false; 
	m_nIndexCount = nIndexCount;
}

void CEtMeshStream::Draw( int nVertexDeclIndex, int nDrawStart, int nDrawCount )
{
	int i, nElementCount;
	EtVertexDecl *pDecl;	
	D3DVERTEXELEMENT9 Decl[ 32 ];

	if( nDrawStart < 0 )
	{
		nDrawStart = 0;
	}
	if( nVertexDeclIndex != -1 )
	{
		GetEtDevice()->SetVertexDeclaration( nVertexDeclIndex );
	}
	pDecl = GetEtDevice()->GetVertexDeclaration( nVertexDeclIndex );
	if( !pDecl )	{
		return;
	}
	GetEtDevice()->GetVertexElement( nVertexDeclIndex, Decl, ( UINT * )&nElementCount );
	
	for( i = 0; i < nElementCount; i++ )
	{
		EtVertexBuffer *pBuffer;
		int nStride;

		if( Decl[ i ].Stream == 0xff )
		{
			continue;
		}
		pBuffer = FindVertexStream( ( MeshStreamType )Decl[ i ].Usage, ( int )Decl[ i ].UsageIndex, nStride );
		if( pBuffer == NULL ) return;
		GetEtDevice()->SetStreamSource( Decl[ i ].Stream, pBuffer, nStride );
	}

	if( m_pIB ) 
	{
		GetEtDevice()->SetIndexBuffer( m_pIB );
	}

	if( m_pPointSpriteTexture ) 
	{
		GetEtDevice()->SetTexture( 0, m_pPointSpriteTexture );
		GetEtDevice()->SetPixelShader( NULL );
		GetEtDevice()->SetRenderState( D3DRS_POINTSPRITEENABLE, TRUE );
		GetEtDevice()->DrawPrimitive( PT_POINTLIST, 0, m_nVertexCount);
		GetEtDevice()->SetRenderState( D3DRS_POINTSPRITEENABLE, FALSE );
		GetEtDevice()->SetTexture( 0, NULL);		
	}
	else if( m_bStrip )	// 스트립일때는 DrawCount를 측정 못해서.. 적용안한다. 전체만 드로우 된다..
	{
		if( nDrawCount == 0 )
		{
			nDrawCount = m_nIndexCount - 2;
		}
		else
		{
			nDrawCount = nDrawCount + 2;
		}
		GetEtDevice()->DrawIndexedPrimitive( PT_TRIANGLESTRIP, 0, m_nVertexCount, 0, nDrawCount );
	}
	else
	{
		if( nDrawCount == 0 )
		{
			nDrawCount = m_nIndexCount / 3;
		}
		GetEtDevice()->DrawIndexedPrimitive( PT_TRIANGLELIST, 0, m_nVertexCount, nDrawStart * 3, nDrawCount );
	}
}

void CEtMeshStream::DrawInstancing( int nVertexDeclIndex, EtVertexBuffer *pVB, int nStartIndex, int nInstanceCount )
{
	int i, nElementCount;
	EtVertexDecl *pDecl;	
	D3DVERTEXELEMENT9 Decl[ 32 ];

	if( nVertexDeclIndex != -1 )
	{
		GetEtDevice()->SetVertexDeclaration( nVertexDeclIndex );
	}
	pDecl = GetEtDevice()->GetVertexDeclaration( nVertexDeclIndex );
	if( !pDecl ) {
		return;
	}
	GetEtDevice()->GetVertexElement( nVertexDeclIndex, Decl, ( UINT * )&nElementCount );

	int nMatrixStream = 0;
	for( i = 0; i < nElementCount; i++ )
	{
		if( Decl[i].UsageIndex != 0 ) {
			nMatrixStream = Decl[i].Stream;
			break;
		}
		EtVertexBuffer *pBuffer;
		int nStride;
		pBuffer = FindVertexStream( ( MeshStreamType )Decl[ i ].Usage, ( int )Decl[ i ].UsageIndex, nStride );
		ASSERT( pBuffer != NULL );
		GetEtDevice()->SetStreamSource( Decl[ i ].Stream, pBuffer, nStride );
		GetEtDevice()->SetStreamSourceFreq( Decl[ i ].Stream, D3DSTREAMSOURCE_INDEXEDDATA | nInstanceCount );
	}
	if( m_pIB ) 
	{
		GetEtDevice()->SetIndexBuffer( m_pIB );
	}

	GetEtDevice()->SetStreamSource( nMatrixStream,  pVB, sizeof(EtMatrix), sizeof(EtMatrix) * nStartIndex );	
	GetEtDevice()->SetStreamSourceFreq( nMatrixStream, D3DSTREAMSOURCE_INSTANCEDATA | 1ul );
	
	if( m_bStrip )	// 스트립일때는 DrawCount를 측정 못해서.. 적용안한다. 전체만 드로우 된다..
	{
		GetEtDevice()->DrawIndexedPrimitive( PT_TRIANGLESTRIP, 0, m_nVertexCount, 0, m_nIndexCount - 2 );
	}
	else
	{
		int nDrawCount = m_nIndexCount / 3;
		GetEtDevice()->DrawIndexedPrimitive( PT_TRIANGLELIST, 0, m_nVertexCount, 0, nDrawCount );
	}

	for( i = 0; i <= nMatrixStream; i++) {
		GetEtDevice()->SetStreamSourceFreq( i, 1 );
	}
}

void CEtMeshStream::Attach( CEtMeshStream *pMeshStream,  std::vector<int> &BoneRemapTable, EtVector4 ScaleBias, int nMergeOrder )
{
	int nSize;
	nSize = (int)m_vecElements.size();
	int nNewVertexCount = (m_nVertexCount + pMeshStream->m_nVertexCount);
	for( int i = 0; i < nSize; i++) {

		int nStride = m_vecElements[ i ].nStride;
		BYTE *pVertexBuffer = new BYTE[ nStride * pMeshStream->m_nVertexCount ];
		BYTE *pCurrVertexBuffer = new BYTE[ nStride * m_nVertexCount ];
		memset(pCurrVertexBuffer, 0, nStride*m_nVertexCount);
		pMeshStream->GetVertexStream( m_vecElements[ i ].Type, pVertexBuffer, m_vecElements[ i ].nStreamIndex );

		void *pPtr = NULL;
		m_vecElements[ i ].m_pVB->Lock( 0, nStride * m_nVertexCount, &pPtr, 0 );
		if( pPtr ) {
			memcpy( pCurrVertexBuffer, pPtr, nStride * m_nVertexCount );
		}
		m_vecElements[ i ].m_pVB->Unlock();

		EtVertexBuffer *pVB = GetEtDevice()->CreateVertexBuffer( nNewVertexCount * nStride, 0, m_Usage, m_Pool );

		void *pDestBuffer = NULL;
		pVB->Lock(0, nNewVertexCount * nStride, &pDestBuffer, 0);
		if( pDestBuffer ) {
			memcpy( pDestBuffer, pCurrVertexBuffer, nStride*m_nVertexCount);

			if( m_vecElements[ i ].Type == MST_TEXCOORD ) {
				for( int j = 0; j < pMeshStream->m_nVertexCount; j++) {
					((EtVector2*)pDestBuffer)[m_nVertexCount+j].x = float(nMergeOrder) + ((EtVector2*)pVertexBuffer)[j].x * ScaleBias.x + ScaleBias.z;
					((EtVector2*)pDestBuffer)[m_nVertexCount+j].y = ((EtVector2*)pVertexBuffer)[j].y * ScaleBias.y + ScaleBias.w;
				}
			}
			else {
				memcpy( ((BYTE*)pDestBuffer) + nStride * m_nVertexCount, pVertexBuffer, nStride*pMeshStream->m_nVertexCount);
			}
		}
		pVB->Unlock();
		delete [] pVertexBuffer;
		delete [] pCurrVertexBuffer;
		SAFE_RELEASE( m_vecElements[ i ].m_pVB );
		m_vecElements[ i ].m_pVB = pVB;
	}


	if( pMeshStream->m_bStrip ) {

		int nNewIndexCount = m_nIndexCount + (pMeshStream->m_nIndexCount-2)*3;
		WORD *pIndices = new WORD[ nNewIndexCount ];
		GetIndexStream( pIndices );
		WORD *pAttachIndices = new WORD[ pMeshStream->m_nIndexCount ];
		pMeshStream->GetIndexStream( pAttachIndices );

		for( int i = 0; i < pMeshStream->m_nIndexCount-2; i++) {
			if( (i&1)==0) {
				pIndices[m_nIndexCount+i*3+0] = pAttachIndices[i] + m_nVertexCount;
				pIndices[m_nIndexCount+i*3+1] = pAttachIndices[i+1] + m_nVertexCount;
				pIndices[m_nIndexCount+i*3+2] = pAttachIndices[i+2] + m_nVertexCount;
			}
			else {
				pIndices[m_nIndexCount+i*3+0] = pAttachIndices[i+1] + m_nVertexCount;
				pIndices[m_nIndexCount+i*3+1] = pAttachIndices[i] + m_nVertexCount;
				pIndices[m_nIndexCount+i*3+2] = pAttachIndices[i+2] + m_nVertexCount;
			}
		}

		m_nIndexCount = nNewIndexCount;
		SAFE_RELEASE( m_pIB );
		m_pIB = GetEtDevice()->CreateIndexBuffer( sizeof( WORD ) * m_nIndexCount, m_Usage, m_Pool );
		SetIndexStream( pIndices );
		delete [] pIndices;
		delete [] pAttachIndices;
	}
	else {
		int nNewIndexCount = m_nIndexCount + pMeshStream->m_nIndexCount;
		WORD *pIndices = new WORD[ nNewIndexCount ];
		GetIndexStream( pIndices );
		pMeshStream->GetIndexStream( pIndices + m_nIndexCount );
		for( int i = m_nIndexCount; i < nNewIndexCount; i++) {
			pIndices[i] += m_nVertexCount;
		}
		m_nIndexCount = nNewIndexCount;
		SAFE_RELEASE( m_pIB );
		m_pIB = GetEtDevice()->CreateIndexBuffer( sizeof( WORD ) * m_nIndexCount, m_Usage, m_Pool );
		SetIndexStream( pIndices );
		delete [] pIndices;
	}	

	int nOldVertexCount = m_nVertexCount;
	m_nVertexCount = nNewVertexCount;

	if( !BoneRemapTable.empty() ) {
		WORD *pBoneIndices = new WORD[m_nVertexCount*4];
		if( GetVertexStream( MST_BONEINDEX, pBoneIndices, 0 ) != 0 ) {
			int nMaxIndex = -1;
			for( int i = nOldVertexCount; i < m_nVertexCount; i++) {
				if( pBoneIndices[i*4+0] != 65535 ) nMaxIndex = EtMax( nMaxIndex, pBoneIndices[i*4+0] );
				if( pBoneIndices[i*4+1] != 65535 ) nMaxIndex = EtMax( nMaxIndex, pBoneIndices[i*4+1] );
				if( pBoneIndices[i*4+2] != 65535 ) nMaxIndex = EtMax( nMaxIndex, pBoneIndices[i*4+2] );
				if( pBoneIndices[i*4+3] != 65535 ) nMaxIndex = EtMax( nMaxIndex, pBoneIndices[i*4+3] );
			}

			if( (int)BoneRemapTable.size() > nMaxIndex ) {
				for( int i = nOldVertexCount; i < m_nVertexCount; i++) {
					if( pBoneIndices[i*4+0] != 65535 ) pBoneIndices[i*4+0] = BoneRemapTable[ pBoneIndices[i*4+0] ];
					if( pBoneIndices[i*4+1] != 65535 ) pBoneIndices[i*4+1] = BoneRemapTable[ pBoneIndices[i*4+1] ];
					if( pBoneIndices[i*4+2] != 65535 ) pBoneIndices[i*4+2] = BoneRemapTable[ pBoneIndices[i*4+2] ];
					if( pBoneIndices[i*4+3] != 65535 ) pBoneIndices[i*4+3] = BoneRemapTable[ pBoneIndices[i*4+3] ];		
				}
			}
			SetVertexStream( MST_BONEINDEX, pBoneIndices, 0 );
		}
		delete [] pBoneIndices;
	}

}

void CEtMeshStream::RemapBoneIndices( std::vector<int> &BoneRemapTable )
{
	if( BoneRemapTable.empty() ) return;

	WORD *pBoneIndices = new WORD[m_nVertexCount*4];
	int nStride = GetVertexStream( MST_BONEINDEX, pBoneIndices, 0 );
	if( nStride ) {
		for( int i = 0; i < m_nVertexCount; i++) {
			if( pBoneIndices[i*4+0] != 65535 ) pBoneIndices[i*4+0] = BoneRemapTable[ pBoneIndices[i*4+0] ];
			if( pBoneIndices[i*4+1] != 65535 ) pBoneIndices[i*4+1] = BoneRemapTable[ pBoneIndices[i*4+1] ];
			if( pBoneIndices[i*4+2] != 65535 ) pBoneIndices[i*4+2] = BoneRemapTable[ pBoneIndices[i*4+2] ];
			if( pBoneIndices[i*4+3] != 65535 ) pBoneIndices[i*4+3] = BoneRemapTable[ pBoneIndices[i*4+3] ];		
		}
		SetVertexStream( MST_BONEINDEX, pBoneIndices, 0 );
	}
	delete [] pBoneIndices;
}

void CEtMeshStream::Assign( CEtMeshStream *pMeshStream, EtVector4 ScaleBias )
{
	m_bStrip = pMeshStream->m_bStrip;
	m_bGenerateTangentSpace = pMeshStream->m_bGenerateTangentSpace;
	m_nIndexCount = pMeshStream->m_nIndexCount;
	m_nVertexCount = pMeshStream->m_nVertexCount;
	m_bDeleteVB = pMeshStream->m_bDeleteVB;
	m_bDeleteIB = pMeshStream->m_bDeleteIB;
	m_Usage = pMeshStream->m_Usage;
	m_Pool = pMeshStream->m_Pool;

	WORD *pIndices = new WORD[ m_nIndexCount ];
	pMeshStream->GetIndexStream( pIndices );
	
	ASSERT( m_pIB == NULL );
	SAFE_RELEASE( m_pIB );

	if( m_bStrip ) {
		int nOldIndexCount = m_nIndexCount;
		m_nIndexCount = (m_nIndexCount -2)*3;
		
		m_pIB = GetEtDevice()->CreateIndexBuffer( sizeof( WORD ) * m_nIndexCount, m_Usage, m_Pool );

		WORD *pNewIndices = new WORD[ m_nIndexCount ];

		for( int i = 0; i < nOldIndexCount-2; i++) {
			if( (i&1)==0) {
				pNewIndices[i*3+0] = pIndices[i];
				pNewIndices[i*3+1] = pIndices[i+1];
				pNewIndices[i*3+2] = pIndices[i+2];
			}
			else {
				pNewIndices[i*3+0] = pIndices[i+1];
				pNewIndices[i*3+1] = pIndices[i];
				pNewIndices[i*3+2] = pIndices[i+2];
			}
		}
		SetIndexStream( pNewIndices );
		delete [] pNewIndices;
		m_bStrip = false;
	}
	else {
		m_pIB = GetEtDevice()->CreateIndexBuffer( sizeof( WORD ) * m_nIndexCount, m_Usage, m_Pool );
		SetIndexStream( pIndices );
	}
	delete [] pIndices;

	int i, nSize;
	nSize = (int)pMeshStream->m_vecElements.size();
	for( i = 0; i < nSize; i++) {
		if( !pMeshStream->m_vecElements[ i ].m_pVB ) continue;

		int nStride = pMeshStream->m_vecElements[ i ].nStride;
		BYTE *pVertexBuffer = new BYTE[ nStride * m_nVertexCount ];
		memset( pVertexBuffer, 0, nStride * m_nVertexCount );
		SStreamElement StreamElement;
		StreamElement.nStreamIndex = pMeshStream->m_vecElements[ i ].nStreamIndex;
		StreamElement.nStride = pMeshStream->m_vecElements[ i ].nStride;
		StreamElement.Type = pMeshStream->m_vecElements[ i ].Type;

		void *pPtr = NULL;
		pMeshStream->m_vecElements[ i ].m_pVB->Lock( 0, nStride * m_nVertexCount, &pPtr, 0 );
		if( pPtr )
			memcpy( pVertexBuffer, pPtr, nStride * m_nVertexCount );
		pMeshStream->m_vecElements[ i ].m_pVB->Unlock();

		StreamElement.m_pVB = GetEtDevice()->CreateVertexBuffer( m_nVertexCount * nStride, 0, m_Usage, m_Pool );
		pPtr = NULL;
		StreamElement.m_pVB->Lock( 0, nStride * m_nVertexCount, &pPtr, 0);		
		if( pPtr ) {
			if( StreamElement.Type == MST_TEXCOORD ) {
				for( int j = 0; j < m_nVertexCount; j++) {
					((EtVector2*)pPtr)[j].x = ((EtVector2*)pVertexBuffer)[j].x * ScaleBias.x + ScaleBias.z;
					((EtVector2*)pPtr)[j].y = ((EtVector2*)pVertexBuffer)[j].y * ScaleBias.y + ScaleBias.w;
				}
			}
			else {
				memcpy( pPtr, pVertexBuffer, nStride * m_nVertexCount );
			}
		}
		StreamElement.m_pVB->Unlock();

		m_vecElements.push_back( StreamElement );

		delete [] pVertexBuffer;
	}	
}
