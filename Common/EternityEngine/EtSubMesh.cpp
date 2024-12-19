#include "StdAfx.h"
#include "EtSubMesh.h"
#include "EtAni.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CEtSubMesh::CEtSubMesh(void)
{
	m_nParentBoneIndex = -1;
	m_pPosition = NULL;
	m_pIndex = NULL;
	memset(&m_SubMeshHeader, 0, sizeof(m_SubMeshHeader));	
}

CEtSubMesh::~CEtSubMesh(void)
{
	SAFE_DELETE( m_pPosition );
	SAFE_DELETE( m_pIndex );
}

void CEtSubMesh::Clear()
{
	m_vecBoneName.clear();
}

void CEtSubMesh::LinkToAni( EtResourceHandle hResource )
{
	if( ( m_vecLinkIndex.size() > 0 ) || ( m_nParentBoneIndex != -1 ) )
	{
		return;
	}

	int i;
	EtAniHandle hAni;

	hAni = hResource;
	if( !m_vecBoneName.empty() )
	{
		m_vecLinkIndex.resize( m_vecBoneName.size() );
		for( i = 0; i < ( int )m_vecBoneName.size(); i++ )
		{
			m_vecLinkIndex[ i ] = hAni->GetBoneIndex( m_vecBoneName[ i ].c_str() );
		}
	}
	else
	{
		m_nParentBoneIndex = hAni->GetBoneIndex( m_SubMeshHeader.szParentName );
	}
}

void CEtSubMesh::LoadSubMesh( CStream *pStream )
{
	int i;

	m_MeshStream.Clear();

	pStream->Read( &m_SubMeshHeader, sizeof( SSubMeshHeader ) );
	pStream->Seek( SUBMESH_HEADER_RESERVED, SEEK_CUR );

	m_MeshStream.LoadIndexStream( pStream, m_SubMeshHeader.bUseStrip, m_SubMeshHeader.nFaceBufferSize );

	m_MeshStream.LoadVertexStream( pStream, MST_POSITION, 0, m_SubMeshHeader.nVertexCount );
	m_MeshStream.LoadVertexStream( pStream, MST_NORMAL, 0, m_SubMeshHeader.nVertexCount );
	for( i = 0; i < m_SubMeshHeader.nTextureChannel; i++ )
	{
		m_MeshStream.LoadVertexStream( pStream, MST_TEXCOORD, i, m_SubMeshHeader.nVertexCount );
	}
	if( m_SubMeshHeader.bUseVertexColor )
	{
		m_MeshStream.LoadVertexStream( pStream, MST_COLOR, 0, m_SubMeshHeader.nVertexCount );
	}
	if( m_SubMeshHeader.bUsePhysique )
	{
		m_MeshStream.LoadVertexStream( pStream, MST_BONEINDEX, 0, m_SubMeshHeader.nVertexCount );
		m_MeshStream.LoadVertexStream( pStream, MST_BONEWEIGHT, 0, m_SubMeshHeader.nVertexCount );

		int nBoneCount;
		char szBoneName[ 256 ];

		pStream->Read( &nBoneCount, sizeof( int ) );
		for( i = 0; i < nBoneCount; i++ )
		{
			pStream->Read( szBoneName, 256 );
			m_vecBoneName.push_back( szBoneName );
		}
	}
}

void CEtSubMesh::Draw( int nVertexDeclIndex, int nDrawStart, int nDrawCount )
{
	m_MeshStream.Draw( nVertexDeclIndex, nDrawStart, nDrawCount );
}

bool CEtSubMesh::TestLineToTriangle( EtVector3 &vPosition, EtVector3 &vDirection, EtMatrix &TransMat, float &fDist )
{
	if( m_pPosition == NULL )
	{
		m_pPosition = new EtVector3[ m_MeshStream.GetVertexCount() ];
		GetVertexStream( MST_POSITION, m_pPosition );
	}
	if( m_pIndex == NULL )
	{
		m_pIndex = new WORD[ m_MeshStream.GetIndexCount() ];
		m_MeshStream.GetIndexStream( m_pIndex );
	}

	int i, j, nIndexCount;
	bool bStrip;

	bStrip = m_MeshStream.IsStrip();
	nIndexCount = m_MeshStream.GetIndexCount();
	for( i = 0; i < nIndexCount; )
	{
		EtVector3 vTransformPoint[ 3 ];
		float fBary1, fBary2;

		if( i > nIndexCount - 3 )
		{
			break;
		}
		if( bStrip )
		{
			if( m_pIndex[ i + 2 ] == 0xffff )
			{
				i += 3;
			}
		}

		for( j = 0; j < 3; j++ )
		{
			EtVec3TransformCoord( vTransformPoint + j, m_pPosition + m_pIndex[ i + j ], &TransMat );
		}

		if( ::TestLineToTriangle( vPosition, vDirection, vTransformPoint[ 0 ], vTransformPoint[ 1 ], vTransformPoint[ 2 ], fDist, fBary1, fBary2 ) )
		{
			return true;
		}

		if( bStrip )
		{
			i++;
		}
		else
		{
			i += 3;
		}
	}

	return false;
}

bool CEtSubMesh::IsExistStream( MeshStreamType Type, int nStreamIndex )
{
	int nStride;

	if( m_MeshStream.FindVertexStream( Type, nStreamIndex, nStride ) )
	{
		return true;
	}
	else
	{
		return false;
	}
}

void CEtSubMesh::RemoveLinkInfo()
{
	m_vecBoneName.clear();
	m_vecLinkIndex.clear();
}

void CEtSubMesh::Assign( CEtSubMesh *pSubMesh, EtVector4 ScaleBias )
{
	m_SubMeshHeader = pSubMesh->m_SubMeshHeader;
	m_MeshStream.Assign( &pSubMesh->m_MeshStream, ScaleBias ) ;
	m_vecBoneName = pSubMesh->m_vecBoneName;
	m_vecLinkIndex = pSubMesh->m_vecLinkIndex;
	m_nParentBoneIndex = pSubMesh->m_nParentBoneIndex;	
}