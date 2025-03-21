#include "Stdafx.h"
#include "ActorEternity.h"
#include "SaveMesh.h"

#include "AtgStrip.h"

CSaveMesh::CSaveMesh()
{
	m_pNode = NULL;
	m_pMesh = NULL;
	m_pMtl = NULL;
}

CSaveMesh::~CSaveMesh()
{
}

void CSaveMesh::Initialize( INode *pNode, Mesh *pMesh, Mtl *pMtl, std::vector< std::string > *pBoneList, int nMatCount, 
						   int nMatIndex, int nMAXVertexCount )
{
	m_pNode = pNode;
	m_pMesh = pMesh;
	m_pMtl = pMtl;
	m_pBoneList = pBoneList;
	m_nMatCount = nMatCount;
	m_nMatIndex = nMatIndex;
	m_nMAXVertexCount = nMAXVertexCount;

	m_nTextureChannelCount = GetTextureChannelCount( pMtl );
	if( m_nTextureChannelCount <= 0 )
	{
		m_nTextureChannelCount = 1;
	}
}

void CSaveMesh::SetMeshName( const char *pszMeshName, const char *pszParentName )
{
	strcpy( m_szMeshName, pszMeshName );
	strcpy( m_szParentName, pszParentName );
}

int CSaveMesh::BuildSaveMesh( bool bUseStrip )
{
	GetVertexList( m_pMesh, m_LocalVertexList, m_nMatCount, m_nMatIndex );
	if( m_LocalVertexList.empty() )
	{
		return 0;
	}
	m_FinalFaceList.resize( m_LocalVertexList.size() );
	ReduceSameVertex( m_LocalVertexList, m_FinalFaceList, m_FinalVertexList );

	if( m_FinalVertexList.size() > 65535 )
	{
		MessageBox( NULL, "버텍스 갯수가 65535개가 넘습니다. 정상적으로 보이지 않을겁니다.", NULL, MB_OK );
	}
	else if( bUseStrip )
	{
		int i;
		DWORD nStripIndexCount;
		WORD *pStripIndices, *pVertexPermutation;
		std::vector< SLocalVertex > StripVertexList;

		TriStripper::MakeStrips( m_FinalFaceList.size() / 3, &m_FinalFaceList[ 0 ], &nStripIndexCount, &pStripIndices );
		TriStripper::ComputeVertexPermutation( nStripIndexCount, pStripIndices, m_FinalVertexList.size(), &pVertexPermutation );

		m_FinalFaceList.clear();
		m_FinalFaceList.resize( nStripIndexCount );
		std::copy( pStripIndices, pStripIndices + nStripIndexCount, m_FinalFaceList.begin() );
		delete [] pStripIndices;

		StripVertexList.resize( m_FinalVertexList.size() );
		for( i = 0; i < m_FinalVertexList.size(); i++ )
		{
			StripVertexList[ i ] = m_FinalVertexList[ pVertexPermutation[ i ] ];
		}
		std::copy( StripVertexList.begin(), StripVertexList.end(), m_FinalVertexList.begin() );
		StripVertexList.clear();
		delete [] pVertexPermutation;
	}

	return ( int )m_FinalFaceList.size();
}

int CSaveMesh::WriteMesh( FILE *fp, std::vector< SMAXVertex > &SystemVertex, std::vector< std::string > &BoneList, bool bUseStrip, 
						 bool bPhysique, bool bExportVertexColor )
{
	int i, j;
	SSubMeshHeader SubMeshHeader;

	memset( &SubMeshHeader, 0, sizeof( SSubMeshHeader ) );
	SubMeshHeader.nVertexCount = m_FinalVertexList.size();
	SubMeshHeader.bUseStrip = bUseStrip;
	if( SubMeshHeader.nVertexCount > 65535 )
	{
		SubMeshHeader.bUseStrip = false;		// vertex 갯수가 65535 넘어가면 스트립 아니다.. 정상적으로 보이지도 않을텐데 뭐..
	}
	SubMeshHeader.nFaceCount = m_FinalFaceList.size();
	SubMeshHeader.bExportVertexColor = bExportVertexColor;
	SubMeshHeader.nTextureChannel = m_nTextureChannelCount;
	if( SubMeshHeader.nTextureChannel <= 0 )
	{
		SubMeshHeader.nTextureChannel = 1;
	}
	SubMeshHeader.bUsePhysique = bPhysique;
	strcpy( SubMeshHeader.szName, m_szMeshName );
	strcpy( SubMeshHeader.szParentName, m_szParentName );
	fwrite( &SubMeshHeader, sizeof( SSubMeshHeader ), 1, fp );

	fwrite( &m_FinalFaceList[ 0 ], sizeof( WORD ), m_FinalFaceList.size(), fp );	// Index Buffer

	for( i = 0; i < m_FinalVertexList.size(); i++ )		// Position
	{
		fwrite( &SystemVertex[ m_FinalVertexList[ i ].nVertexIndex ].Vertex, sizeof( Point3 ), 1, fp );
	}
	for( i = 0; i < m_FinalVertexList.size(); i++ )		// Normal
	{
		fwrite( &m_FinalVertexList[ i ].FaceNormal, sizeof( Point3 ), 1, fp );
	}
	for( i = 0; i < SubMeshHeader.nTextureChannel; i++ )
	{
		for( j = 0; j < m_FinalVertexList.size(); j++ )		// Texture Coord
		{
			fwrite( m_FinalVertexList[ j ].fU + i, sizeof( float ), 1, fp );
			fwrite( m_FinalVertexList[ j ].fV + i, sizeof( float ), 1, fp );
		}
	}
	if( bExportVertexColor )
	{
		for( i = 0; i < m_FinalVertexList.size(); i++ )		// Normal
		{
			fwrite( &SystemVertex[ m_FinalVertexList[ i ].dwVertexColorIndex ], sizeof( DWORD ), 1, fp );
		}
	}
	if( bPhysique )
	{
		int nBoneAllocCount, nBoneIndex;
		std::vector< int > BoneAlloc;
		std::vector< std::string > szBoneAlloc;

		nBoneAllocCount = 0;
		BoneAlloc.resize( BoneList.size() );
		for( i = 0; i < ( int )BoneAlloc.size(); i++ )
		{
			BoneAlloc[ i ] = -1;
		}

		for( i = 0; i < m_FinalVertexList.size(); i++ )		// BoneIndex
		{
			int j;
			for( j = 0; j < 4; j++ )
			{
				nBoneIndex = SystemVertex[ m_FinalVertexList[ i ].nVertexIndex ].nBone[ j ];
				if( nBoneIndex == -1 )
				{
					nBoneIndex = 0;
				}
				else
				{
					if( BoneAlloc[ nBoneIndex ] == -1 )
					{
						BoneAlloc[ nBoneIndex ] = nBoneAllocCount;
						szBoneAlloc.push_back( BoneList[ nBoneIndex ] );
						nBoneAllocCount++;
					}
				}
				short nFinalIndex = BoneAlloc[ nBoneIndex ];
				if( ( nFinalIndex == -1 ) || ( nFinalIndex >= BoneList.size() ) )
				{
					nFinalIndex = 0;
				}
				fwrite( &nFinalIndex, sizeof( short ), 1, fp );
			}
		}
		for( i = 0; i < m_FinalVertexList.size(); i++ )		// BoneIndex
		{
			fwrite( &SystemVertex[ m_FinalVertexList[ i ].nVertexIndex ].fWeight, sizeof( float ), 4, fp );
		}

		char szBoneName[ 256 ];
		if( nBoneAllocCount > 50 )
		{
			MessageBox( NULL, "Bone 갯수가 50개가 넘습니다. 메쉬를 분할해주세요", NULL, MB_OK );
		}
		fwrite( &nBoneAllocCount, sizeof( int ), 1, fp );
		for( i = 0; i < nBoneAllocCount; i++)
		{
			memset( szBoneName, 0, 256 );
			strcpy( szBoneName, szBoneAlloc[ i ].c_str() );
			fwrite( szBoneName, 256, 1, fp );
		}
	}

	return 1;
}

Point3 CSaveMesh::GetFaceNormal( Mesh *pMesh, int nFaceNum, RVertex *pRVertex )
{
	int i, numNormals;
	Face *pFace = &pMesh->faces[ nFaceNum ];
	DWORD smGroup = pFace->smGroup;
	Point3 FaceNormal;

	// Is normal specified
	// SPCIFIED is not currently used, but may be used in future versions.
	if( pRVertex->rFlags & SPECIFIED_NORMAL )
	{
		FaceNormal = pRVertex->rn.getNormal();
	}
	// If normal is not specified it's only available if the face belongs
	// to a smoothing group
	else
	{
		numNormals = pRVertex->rFlags & NORCT_MASK;
		if( numNormals && smGroup )
		{
			// If there is only one vertex is found in the rn member.
			if( numNormals == 1 )
			{
				FaceNormal = pRVertex->rn.getNormal();
			}
			else
			{
				// If two or more vertices are there you need to step through them
				// and find the vertex with the same smoothing group as the current face.
				// You will find multiple normals in the ern member.
				for( i = 0; i < numNormals; i++ )
				{
					if( pRVertex->ern[ i ].getSmGroup() & smGroup )
					{
						FaceNormal = pRVertex->ern[ i ].getNormal();
					}
				}
			}
		}
		else
		{
			// Get the normal from the Face if no smoothing groups are there
			FaceNormal = pMesh->getFaceNormal( nFaceNum );
		}
	}

	return FaceNormal;
}

bool TMNegParity( Matrix3 &TM )
{
	return ( DotProd( CrossProd( TM.GetRow( 0 ), TM.GetRow( 1 ) ), TM.GetRow( 2 ) ) < 0.0 ) ? 1 : 0;
}

void CSaveMesh::GetVertexList( Mesh *pMesh, std::vector< SLocalVertex > &VertexList, int nMatCount, int nMatID )
{
	int i, j, k, nFaceCount;
	Face *pTriFace;
	TVFace*	pTexturedTriFace;
	SLocalVertex LocalVertex;
	Matrix3 NormalTM;
	bool bNegScale;

	NormalTM = m_pNode->GetObjTMAfterWSM( 0 );
	bNegScale = TMNegParity( NormalTM );
//	NormalTM.RotateZ( 3.141592654f );
	NormalTM.SetRow( 3, Point3( 0, 0, 0 ) );

	nFaceCount = pMesh->getNumFaces();
	for( i = 0; i < nFaceCount; i++ )
	{
		pTriFace = pMesh->faces + i;
		if( pTriFace->getMatID() % nMatCount != nMatID )
		{
			continue;
		}
		for( j = 0; j < 3; j++ )
		{
			UVVert UV;
			Point3 FaceNormal;
			int nFaceIndex;

			if( bNegScale )
			{
				nFaceIndex = j;
			}
			else
			{
				nFaceIndex = 2 - j;
			}

			LocalVertex.nVertexIndex = pTriFace->getVert( nFaceIndex );
			LocalVertex.nSmothGroup = pTriFace->getSmGroup();
			for( k = 0; k < m_nTextureChannelCount; k++ )
			{
				if( pMesh->mapSupport( k + 1 ) ) {
					pTexturedTriFace = pMesh->mapFaces( k + 1 ) + i;
					UV = pMesh->mapVerts( k + 1)[ pTexturedTriFace->getTVert( nFaceIndex ) ];
					LocalVertex.fU[ k ] = UV.x;
					LocalVertex.fV[ k ] = 1.0f - UV.y;
				}
				else {
					LocalVertex.fU[ k ] = 0.f;
					LocalVertex.fV[ k ] = 0.f;
				}
			}
			LocalVertex.nCheckFlag = 0;
			LocalVertex.bUseFaceNormal = false;
			FaceNormal = Normalize( NormalTM * GetFaceNormal( pMesh, i, pMesh->getRVertPtr( pTriFace->getVert( nFaceIndex ) ) ) );
			LocalVertex.FaceNormal.x = FaceNormal.x;
			LocalVertex.FaceNormal.y = FaceNormal.z;
			LocalVertex.FaceNormal.z = FaceNormal.y;

			if( pMesh->vcFace )
			{
				LocalVertex.dwVertexColorIndex = pMesh->vcFace[ i ].t[ nFaceIndex ];
			}
			else
			{
				LocalVertex.dwVertexColorIndex = 0;
			}

			VertexList.push_back( LocalVertex );
		}
	}
}

void CSaveMesh::ReduceSameVertex( std::vector< SLocalVertex > &VertexList, std::vector< WORD > &FaceList, std::vector< SLocalVertex > &ReduceList )
{
	int i, j, k, t;
	bool bBreak;
	std::vector< std::vector< int > > vecVertexIndex;

	vecVertexIndex.resize( m_nMAXVertexCount );
	for( i = 0; i < VertexList.size(); i++ )
	{
		vecVertexIndex[ VertexList[ i ].nVertexIndex ].push_back( i );
	}
	for( i = 0; i < m_nMAXVertexCount; i++ )
	{
		int nSize;

		nSize = vecVertexIndex[ i ].size();
		if( nSize == 1 )
		{
			ReduceList.push_back( VertexList[ vecVertexIndex[ i ][ 0 ] ] );
			FaceList[ vecVertexIndex[ i ][ 0 ] ] = ReduceList.size() - 1;
		}
		else if( nSize <= 0 )
		{
			continue;
		}
		for( j = 0; j < nSize; j++ )
		{
			if( VertexList[ vecVertexIndex[ i ][ j ] ].nCheckFlag == 1 )
			{
				continue;
			}
			ReduceList.push_back( VertexList[ vecVertexIndex[ i ][ j ] ] );
			FaceList[ vecVertexIndex[ i ][ j ] ] = ReduceList.size() - 1;
			for( k = j + 1; k < nSize; k++ )
			{
				if( VertexList[ vecVertexIndex[ i ][ j ] ].nSmothGroup != VertexList[ vecVertexIndex[ i ][ k ] ].nSmothGroup )
				{
					continue;
				}
				bBreak = false;
				for( t = 0; t < m_nTextureChannelCount; t++ )
				{
					if( VertexList[ vecVertexIndex[ i ][ j ] ].fU[ t ] != VertexList[ vecVertexIndex[ i ][ k ] ].fU[ t ] )
					{
						bBreak = true;
						continue;
					}
					if( VertexList[ vecVertexIndex[ i ][ j ] ].fV[ t ] != VertexList[ vecVertexIndex[ i ][ k ] ].fV[ t ] )
					{
						bBreak = true;
						continue;
					}
				}
				if( bBreak )
				{
					continue;
				}
				VertexList[ vecVertexIndex[ i ][ k ] ].nCheckFlag = 1;
				FaceList[ vecVertexIndex[ i ][ k ] ] = ReduceList.size() - 1;
			}
		}
	}


/*	int i, j;
	int nVertexCount;

	nVertexCount = VertexList.size();
	for( i = 0; i < nVertexCount; i++ )
	{
		if( VertexList[ i ].nCheckFlag == 0 )
		{
			FaceList[ i ] = ReduceList.size();
			ReduceList.push_back( VertexList[ i ] );
			for( j = i + 1; j < nVertexCount; j++ )
			{
				if( VertexList[ i ].nVertexIndex != VertexList[ j ].nVertexIndex )
				{
					continue;
				}
				if( VertexList[ i ].nSmothGroup != VertexList[ j ].nSmothGroup )
				{
					ReduceList[ ReduceList.size() - 1 ].bUseFaceNormal = true;
					VertexList[ j ].bUseFaceNormal = true;
					continue;
				}
				if( VertexList[ i ].fU != VertexList[ j ].fU )
				{
					continue;
				}
				if( VertexList[ i ].fV != VertexList[ j ].fV )
				{
					continue;
				}
				VertexList[ j ].nCheckFlag = 1;
				FaceList[ j ] = ReduceList.size() - 1;
			}
		}
	}*/
}

