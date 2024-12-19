#include "StdAfx.h"
#include "EtTerrain.h"
#include "EtConvexVolume.h"
#include "EtCollisionFunc.h"
#include "EtLoader.h"
#include "EtOptionController.h"
#include "EtRenderStack.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

using namespace EternityEngine;

char *CEtTerrain::s_szTerrainMaterial[ 6 ] =
{
	"LayeredTerrain.fx",
	"LayeredCliffTerrain.fx",
	"LowDetailTerrain.fx",
	"LayeredTerrainOpti.fx",
	"LayeredCliffTerrainOpti.fx",
	"LayeredCliffTerrainLow.fx",
};

CEtTerrain::CEtTerrain(void)
{
	m_bEnable = false;
	m_bDrawGrid = false;
	m_dwGridColor = 0xffffffff;
	m_BoundingBox.Reset();

	m_nTechniqueIndex = 0;
	SetTextureLayerCount( 4 );

	m_nBlockOffsetX = 0;
	m_nBlockOffsetY = 0;

	m_nMaterialName = 0;
	m_nRenderUniqueID = -1;

	m_nBakeDepthType = DT_OPAQUE;

	m_nStride = 0;
	m_nStrideVert = 0;
}

CEtTerrain::~CEtTerrain(void)
{
	Clear();
}

void CEtTerrain::Clear()
{
	int i;

	SAFE_RELEASE_SPTR( m_hMaterial );	
	SAFE_RELEASE_SPTR( m_hLightMap );

	for( i = 0; i < ( int )m_vecCustomParam.size(); i++ )
	{
		if( m_vecCustomParam[ i ].Type == EPT_TEX )
		{
			EtResourceHandle hHandle;

			if( m_vecCustomParam[ i ].nTextureIndex != -1 )
			{
				hHandle = CEtResource::GetResource( m_vecCustomParam[ i ].nTextureIndex );
				if( hHandle && hHandle->GetRefCount() > 0 ) {
					SAFE_RELEASE_SPTR( hHandle );				
				}
			}
		}
	}
	m_vecCustomParam.clear();
	m_MeshStream.Clear();
}

void CEtTerrain::SetTextureLayerCount( int nCount )
{
	int i;

	m_vecTextureDist.resize( nCount );
	m_vecTextureHandle.resize( nCount );
	m_vecTextureRotation.resize( nCount );
	m_vecTextureRotationParam.resize( nCount * 2 );
	for( i = 0; i < nCount; i++ )
	{
		m_vecTextureHandle[ i ].Identity();
		m_vecTextureDist[ i ] = FLT_MAX;

		m_vecTextureRotation[ i ] = 0.0f;
		// cos, sin 값 두개를 저장하는 것인데, 기본값은 cos 0, sin 0 이다.
		m_vecTextureRotationParam[ i*2 ] = 1.0f;
		m_vecTextureRotationParam[ i*2+1 ] = 0.0f;
	}
}

void CEtTerrain::Initialize()
{
	Clear();
	CreateMaterial( s_szTerrainMaterial[ m_nMaterialName ] );
	GenerateVertexBuffer();
	GenerateTexureCoord();
	CalcBoundingBox();
#if 0
	int i, j;
	int nLayer1 = 0;
	int nLayer2 = 0;
	int nLayer3 = 0;
	int nLayer4 = 0;
	for( i = 0; i < ( m_TerrainInfo.nSizeY + 1 ); i++ )
	{
		for( j = 0; j < ( m_TerrainInfo.nSizeX + 1 ); j++ )
		{
			DWORD dwValue = GetLayerValue( j, i );
			if( dwValue & 0xff000000 )
			{
				nLayer1++;
			}
			if( dwValue & 0xff0000 )
			{
				nLayer2++;
			}
			if( dwValue & 0xff00 )
			{
				nLayer3++;
			}
			if( dwValue & 0xff )
			{
				nLayer4++;
			}
		}
	}
	OutputDebug( "Layer Count %d %d %d %d\n", nLayer1, nLayer2, nLayer3, nLayer4 );
#endif
}

void CEtTerrain::CreateMaterial( const char *pEffectName )
{
	int i;
	char szParamName[ 64 ];

	m_hMaterial = LoadResource( pEffectName, RT_SHADER, true );
#ifdef PRE_FIX_MATERIAL_DUMP
	if( !m_hMaterial ) return;
#endif

	for( i = 0; i < ( int )m_vecTextureDist.size(); i++ )
	{
		if( m_vecTextureDist[ i ] == FLT_MAX )
		{
			m_vecTextureDist[ i ] = m_TerrainInfo.fTileSize / m_TerrainInfo.fTextureDistance;
		}
	}
	AddCustomParam( m_vecCustomParam, EPT_VECTOR_PTR, m_hMaterial, "g_fTextureDistance", &m_vecTextureDist[ 0 ] );
	AddCustomParam( m_vecCustomParam, EPT_FLOAT_PTR, m_hMaterial, "g_fTileSize", &m_TerrainInfo.fTileSize );
	AddCustomParam( m_vecCustomParam, EPT_VECTOR_PTR, m_hMaterial, "g_WorldOffset", &m_TerrainInfo.TerrainOffset );
	AddCustomParam( m_vecCustomParam, EPT_VECTOR_PTR, m_hMaterial, "g_fTextureRotate12", &m_vecTextureRotationParam[ 0 ] );
	AddCustomParam( m_vecCustomParam, EPT_VECTOR_PTR, m_hMaterial, "g_fTextureRotate34", &m_vecTextureRotationParam[ 4 ] );
	for( i = 0; i < 4; i++ )
	{
		int nTexIndex = -1;
		sprintf( szParamName, "g_LayerTex%d", i + 1 );
		AddCustomParam( m_vecCustomParam, EPT_TEX, m_hMaterial, szParamName, &nTexIndex );
		SetTextureParam( i );
	}
}

void CEtTerrain::SetTerrainInfo( STerrainInfo *pInfo )
{
	memcpy( &m_TerrainInfo, pInfo, sizeof( STerrainInfo ) );
	CheckLayerCount();
}

void CEtTerrain::GenerateTexureCoord()
{
}

void CEtTerrain::GeneratePosition( EtVector3 * pBuffer )
{
	CMemoryStream *pStream;
	int nVertexCount;

	nVertexCount = ( m_TerrainInfo.nSizeX + 1 ) * ( m_TerrainInfo.nSizeY + 1 );
	CalcPosition( pBuffer, nVertexCount );
	pStream = new CMemoryStream( pBuffer, nVertexCount * sizeof( EtVector3 ) );
	m_MeshStream.LoadVertexStream( pStream, MST_POSITION, 0, nVertexCount );
	delete pStream;
}

void CEtTerrain::GenerateNormal( EtVector3 * pBuffer )
{
	CMemoryStream *pStream;
	int nVertexCount;

	nVertexCount = ( m_TerrainInfo.nSizeX + 1 ) * ( m_TerrainInfo.nSizeY + 1 );
	CalcNormal( pBuffer, nVertexCount );
	pStream = new CMemoryStream( pBuffer, nVertexCount * sizeof( EtVector3 ) );
	m_MeshStream.LoadVertexStream( pStream, MST_NORMAL, 0, nVertexCount );
	delete pStream;
}

DWORD CEtTerrain::ConvertLayerValue( DWORD dwLayer )
{
	DWORD dwRet = 0;
	float fValue1, fValue2, fValue3, fValue4;
	float fValue12, fValue123, fValue1234;

	fValue1 = ( dwLayer >> 24 ) / 255.0f;
	fValue2 = ( ( dwLayer >> 16 ) & 0xff ) / 255.0f;
	fValue3 = ( ( dwLayer >> 8 ) & 0xff ) / 255.0f;
	fValue4 = ( dwLayer & 0xff ) / 255.0f;

	fValue12 = fValue1 + fValue2;
	fValue123 = fValue12 + fValue3;
	fValue1234 = fValue123 + fValue4;

	if( fValue12 != 0.0f )
	{
		dwRet |= ( ( DWORD )( fValue2 / ( fValue12 ) * 255 ) & 0xff ) << 24;
	}
	if( fValue123 != 0.0f )
	{
		dwRet |= ( ( DWORD )( fValue3 / ( fValue123 ) * 255 ) & 0xff ) << 16;
	}
	if( fValue1234 != 0.0f )
	{
		dwRet |= ( ( DWORD )( fValue4 / ( fValue1234 ) * 255 ) & 0xff ) << 8;
	}

	return dwRet;
}

void CEtTerrain::GenerateLayer( DWORD *pBuffer )
{
	CMemoryStream *pStream;
	int i, j, nVertexCount;

	nVertexCount = ( m_TerrainInfo.nSizeX + 1 ) * ( m_TerrainInfo.nSizeY + 1 );
	for( i = 0; i <= m_TerrainInfo.nSizeY; i++ )
	{
		int nVertexOffset;

		nVertexOffset = VertexOffset( 0, i );
		for( j = 0; j <= m_TerrainInfo.nSizeX; j++ )
		{
			pBuffer[ nVertexOffset + j ] = ConvertLayerValue( GetLayerValue( j, i ) );
		}
	}
	pStream = new CMemoryStream( pBuffer, nVertexCount * sizeof( DWORD ) );
	m_MeshStream.LoadVertexStream( pStream, MST_COLOR, 0, nVertexCount );
	delete pStream;
}

void CEtTerrain::GenerateVertexBuffer()
{
	EtVector3 *pBuffer;
	int nVertexCount;

	nVertexCount = ( m_TerrainInfo.nSizeX + 1 ) * ( m_TerrainInfo.nSizeY + 1 );

	pBuffer = new EtVector3[ nVertexCount ];
	GeneratePosition( pBuffer );
	GenerateNormal( pBuffer );
	GenerateLayer( ( DWORD *)pBuffer );

	delete [] pBuffer;
}

void CEtTerrain::CalcPosition( EtVector3 *pPosition, int nCount )
{
	int i, j;
	int nSizeX, nSizeY;

	nSizeX = m_TerrainInfo.nSizeX + 1;
	nSizeY = m_TerrainInfo.nSizeY + 1;
	for( i = 0; i < nSizeY; i++ )
	{
		for( j = 0; j < nSizeX; j++ )
		{
			pPosition[ i * nSizeX + j ].x = j * m_TerrainInfo.fTileSize;
			pPosition[ i * nSizeX + j ].y = GetHeight( j, i );
			pPosition[ i * nSizeX + j ].z = i * m_TerrainInfo.fTileSize;
			if( pPosition[ i * nSizeX + j ].y > m_BoundingBox.Max.y )
			{
				m_BoundingBox.Max.y = pPosition[ i * nSizeX + j ].y;
			}
			if( pPosition[ i * nSizeX + j ].y < m_BoundingBox.Min.y )
			{
				m_BoundingBox.Min.y = pPosition[ i * nSizeX + j ].y;
			}
		}
	}
}

void CEtTerrain::CalcNormal( EtVector3 *pNormal, int nCount )
{
	int i, j;
	int nSizeX, nSizeY;

	nSizeX = m_TerrainInfo.nSizeX + 1;
	nSizeY = m_TerrainInfo.nSizeY + 1;
	for( i = 0; i < nSizeY; i++ )
	{
		for( j = 0; j < nSizeX; j++ )
		{
			pNormal[ i * nSizeX + j ] = GetVertexNormal( j, i );
		}
	}
}

EtVector3 CEtTerrain::GetVertexNormal( int nIndexX, int nIndexY )
{
	EtVector3 Return;

	Return.x = ( GetHeight( nIndexX - 1, nIndexY ) - GetHeight( nIndexX + 1, nIndexY ) ) / ( m_TerrainInfo.fTileSize * 2 );
	Return.y = 1.414f * m_TerrainInfo.fHeightMultiply;
	Return.z = ( GetHeight( nIndexX, nIndexY - 1 ) - GetHeight( nIndexX, nIndexY + 1 ) ) / ( m_TerrainInfo.fTileSize * 2 );
	EtVec3Normalize( &Return, &Return );

	return Return;
}

void CEtTerrain::UpdateHeight()
{
	int nVertexCount;
	EtVector3 *pBuffer;

	nVertexCount = ( m_TerrainInfo.nSizeX + 1 ) * ( m_TerrainInfo.nSizeY + 1 );
	pBuffer = new EtVector3[ nVertexCount ];
	GeneratePosition( pBuffer );
	delete [] pBuffer;
}

void CEtTerrain::UpdateLayer()
{
	int nVertexCount;
	DWORD *pBuffer;

	nVertexCount = ( m_TerrainInfo.nSizeX + 1 ) * ( m_TerrainInfo.nSizeY + 1 );
	pBuffer = new DWORD[ nVertexCount ];
	GenerateLayer( pBuffer );
	delete [] pBuffer;
}

void CEtTerrain::UpdateNormal()
{
	int nVertexCount;
	EtVector3 *pBuffer;

	nVertexCount = ( m_TerrainInfo.nSizeX + 1 ) * ( m_TerrainInfo.nSizeY + 1 );
	pBuffer = new EtVector3[ nVertexCount ];
	GenerateNormal( pBuffer );
	delete [] pBuffer;
}

void CEtTerrain::UpdateAll()
{
	int nVertexCount;
	EtVector3 *pBuffer;

	nVertexCount = ( m_TerrainInfo.nSizeX + 1 ) * ( m_TerrainInfo.nSizeY + 1 );
	pBuffer = new EtVector3[ nVertexCount ];
	GeneratePosition( pBuffer );
	GenerateNormal( pBuffer );
	GenerateLayer( ( DWORD * )pBuffer );
	GenerateTexureCoord();
	delete [] pBuffer;
}

void CEtTerrain::Render()
{
	SRenderStackElement RenderElement;
	EtMatrix WorldMat;
	EtCameraHandle hCamera;
	EtVector3 vDist;

	EtMatrixTranslation( &WorldMat, m_TerrainInfo.TerrainOffset.x, m_TerrainInfo.TerrainOffset.y, m_TerrainInfo.TerrainOffset.z );

	hCamera = CEtCamera::GetActiveCamera();
	EtVector3 vCamDir = (*hCamera->GetDirection());
	RenderElement.fDist = EtVec3Dot( (EtVector3*)&WorldMat._41, &vCamDir);

	RenderElement.hMaterial = m_hMaterial;
	RenderElement.nTechniqueIndex = m_nTechniqueIndex;
	RenderElement.WorldMat = WorldMat;
	RenderElement.PrevWorldMat = WorldMat;
	RenderElement.nSaveMatIndex = -1;
	RenderElement.pvecCustomParam = &m_vecCustomParam;
	RenderElement.pRenderMeshStream = &m_MeshStream;
	RenderElement.nDiffuseTexIndex = ( m_vecTextureHandle[0] ) ? m_vecTextureHandle[ 0 ]->GetMyIndex() : -1;
	RenderElement.nBakeDepthIndex = m_nBakeDepthType;
//	RenderElement.renderPriority = RP_HIGH;		//  Overdraw 문제는 해결 됬으므로, 부하가 큰 지형부터 렌더링 큐에 넣어서, GPU Idle time 을 줄인다.
	m_nRenderUniqueID = GetCurRenderStack()->AddNormalRenderElement( RenderElement, m_nRenderUniqueID );

	if( m_bDrawGrid )
	{
		int i, j;

		for( i = 0; i < m_TerrainInfo.nSizeY; i++ )
		{
			for( j = 0; j < m_TerrainInfo.nSizeX; j++ )
			{
				DrawRect( j, i, m_dwGridColor );
			}
		}
	}
}

void CEtTerrain::RenderWater( int index )
{
	SRenderStackElement RenderElement;
	EtMatrix WorldMat;
	EtCameraHandle hCamera;
	EtVector3 vDist;

	EtMatrixTranslation( &WorldMat, m_TerrainInfo.TerrainOffset.x, m_TerrainInfo.TerrainOffset.y, m_TerrainInfo.TerrainOffset.z );

	hCamera = CEtCamera::GetActiveCamera();
	EtVector3 vCamDir = (*hCamera->GetDirection());
	vCamDir.y = 0.f;
	RenderElement.fDist = EtVec3Dot( &EtVector3(WorldMat._41, 0, WorldMat._43) , &vCamDir);

	RenderElement.hMaterial = m_hMaterial;
	RenderElement.nTechniqueIndex = m_nTechniqueIndex;
	RenderElement.WorldMat = WorldMat;
	RenderElement.PrevWorldMat = WorldMat;
	RenderElement.nSaveMatIndex = -1;
	RenderElement.pvecCustomParam = &m_vecCustomParam;
	RenderElement.pRenderMeshStream = &m_MeshStream;
	RenderElement.renderPriority = RP_HIGH;
	GetCurRenderStack()->AddWaterRenderElement( index, RenderElement );

}

void CEtTerrain::GetExtent( EtVector3 &Origin, EtVector3 &Extent )
{
	Origin = ( m_BoundingBox.Max + m_BoundingBox.Min ) / 2;
	Extent = ( m_BoundingBox.Max - m_BoundingBox.Min ) / 2;
}

void CEtTerrain::CalcBoundingBox()
{
	m_BoundingBox.Max.x = m_TerrainInfo.TerrainOffset.x + m_TerrainInfo.nSizeX * m_TerrainInfo.fTileSize;
	m_BoundingBox.Max.z = m_TerrainInfo.TerrainOffset.z + m_TerrainInfo.nSizeY * m_TerrainInfo.fTileSize;

	m_BoundingBox.Min.x = m_TerrainInfo.TerrainOffset.x;
	m_BoundingBox.Min.z = m_TerrainInfo.TerrainOffset.z;

	m_BoundingBox.Max.y += 1.0f;
	m_BoundingBox.Min.y -= 1.0f;
}

float CEtTerrain::Pick( EtVector3 &Origin, EtVector3 &Direction, EtVector3 &PickPos )
{
	int nTileX, nTileY, nDirX, nDirY;
	EtVector2 Origin2D, Direction2D;
	float fTileSize, fDistX, fDistY, fRet;

	fTileSize = m_TerrainInfo.fTileSize;
	if( Direction.x >= 0.0f )
	{
		nDirX = 1;
		Direction2D.x = Direction.x;
		Origin2D.x = Origin.x - m_TerrainInfo.TerrainOffset.x;
	}
	else
	{
		nDirX = -1;
		Direction2D.x = -Direction.x;
		Origin2D.x = m_TerrainInfo.nSizeX * fTileSize - ( Origin.x - m_TerrainInfo.TerrainOffset.x );
	}
	if( Direction.z >= 0.0f )
	{
		nDirY = 1;
		Direction2D.y = Direction.z;
		Origin2D.y = Origin.z - m_TerrainInfo.TerrainOffset.z;
	}
	else
	{
		nDirY = -1;
		Direction2D.y = -Direction.z;
		Origin2D.y = m_TerrainInfo.nSizeY * fTileSize - ( Origin.z - m_TerrainInfo.TerrainOffset.z );
	}

	while( 1 )
	{
		nTileX = ( int )( Origin2D.x / fTileSize );
		if( nDirX < 0 )
		{
			nTileX = m_TerrainInfo.nSizeX - nTileX - 1;
		}
		nTileY = ( int )( Origin2D.y / fTileSize );
		if( nDirY < 0 )
		{
			nTileY = m_TerrainInfo.nSizeY - nTileY - 1;
		}
		if( !IsInTerrain( nTileX, nTileY ) )
		{
			break;
		}
		fRet = TestLintToTile( Origin, Direction, nTileX, nTileY );
		if( ( fRet != FLT_MAX ) && ( fRet >= 0.0f ) )
		{
			PickPos = Origin + Direction * fRet;
	 		return fRet;
		}

		fDistX = ( ( ( int )( Origin2D.x / fTileSize ) + 1 ) * fTileSize - Origin2D.x ) / Direction2D.x;
		fDistY = ( ( ( int )( Origin2D.y / fTileSize ) + 1 ) * fTileSize - Origin2D.y ) / Direction2D.y;
		if( fDistX >= fDistY )
		{
			Origin2D += fDistY * Direction2D;
		}
		else if( fDistX < fDistY )
		{
			Origin2D += fDistX * Direction2D;
		}
	}

	return FLT_MAX;
}

bool CEtTerrain::IsInTerrain( int nX, int nY )
{
	if( ( nX < 0 ) || ( nX >= m_TerrainInfo.nSizeX ) )
	{
		return false;
	}
	if( ( nY < 0 ) || ( nY >= m_TerrainInfo.nSizeY ) )
	{
		return false;
	}

	return true;
}

float CEtTerrain::TestLintToTile( EtVector3 &Origin, EtVector3 &Direction, int nTileX, int nTileY )
{
	float fTileSize;
	float fDist1, fDist2, fBary1, fBary2;
	EtVector3 V1, V2, V3, ExtraOrigin;

	fTileSize = m_TerrainInfo.fTileSize;

	ExtraOrigin = Origin - m_TerrainInfo.TerrainOffset;

	V1 = EtVector3( nTileX * fTileSize, GetHeight( nTileX, nTileY ), nTileY * fTileSize );
	nTileY++;
	V2 = EtVector3( nTileX * fTileSize, GetHeight( nTileX, nTileY ), nTileY * fTileSize );
	nTileX++;
	V3 = EtVector3( nTileX * fTileSize, GetHeight( nTileX, nTileY ), nTileY * fTileSize );
	TestLineToTriangle( ExtraOrigin, Direction, V1, V2, V3, fDist1, fBary1, fBary2 );

	nTileY--;
	V2 = EtVector3( nTileX * fTileSize, GetHeight( nTileX, nTileY ), nTileY * fTileSize );
	TestLineToTriangle( ExtraOrigin, Direction, V1, V3, V2, fDist2, fBary1, fBary2 );

	return min( fDist1, fDist2 );
}

SCustomParam *CEtTerrain::FindEffectParam( EffectParamType Type, int nIndex )
{
	int i, nCount;

	nCount = 0;
	for( i = 0; i < ( int )m_vecCustomParam.size(); i++ )
	{
		if( m_vecCustomParam[ i ].Type == Type )
		{
			if( nCount == nIndex )
			{
				return &m_vecCustomParam[ i ];
			}
			nCount++;
		}
	}

	return NULL;
}

const char *CEtTerrain::GetTextureName( int nTexIndex )
{
	int nFindTexIndex;
	SCustomParam *pParam;

	pParam = FindEffectParam( EPT_TEX, nTexIndex );
	if( pParam == NULL )
	{
		return NULL;
	}
	nFindTexIndex = pParam->nTextureIndex;
	if( nFindTexIndex == -1 )
	{
		return NULL;
	}

	EtTextureHandle hTexture;

	hTexture = CEtResource::GetResource( nFindTexIndex );

//	return hTexture->GetFullName();
	return hTexture->GetFileName();
}

bool CEtTerrain::SetTexture( int nTexIndex, const char *pTexName )
{
	ASSERT( nTexIndex < ( int )m_vecTextureHandle.size() );

	EtTextureHandle hTexture, hLoadTex;

	hLoadTex = LoadResource( pTexName, RT_TEXTURE );
	if( !hLoadTex )
	{
		return false;
	}

	m_vecTextureHandle[ nTexIndex ] = hLoadTex;
	SetTextureParam( nTexIndex );

	return true;
}

void CEtTerrain::SetTextureParam( int nTexIndex )
{
	int nFindTexIndex;
	SCustomParam *pParam;
	EtTextureHandle hTexture;

	if( !m_vecTextureHandle[ nTexIndex ] )
	{
		return;
	}

	pParam = FindEffectParam( EPT_TEX, nTexIndex );
	if( pParam == NULL )
	{
		return;
	}

	nFindTexIndex = pParam->nTextureIndex;
	if( nFindTexIndex != -1 )
	{
		hTexture = CEtResource::GetResource( nFindTexIndex );
		if( hTexture && hTexture->GetRefCount() > 0 ) {
			SAFE_RELEASE_SPTR( hTexture );			
		}
	}
	pParam->nTextureIndex = m_vecTextureHandle[ nTexIndex ]->GetMyIndex();
}

const char *CEtTerrain::GetTextureSemanticName( int nTexIndex )
{
	SCustomParam *pParam;

	pParam = FindEffectParam( EPT_TEX, nTexIndex );
	if( pParam == NULL )
	{
		return NULL;
	}

	return m_hMaterial->GetSemantic( pParam->hParamHandle );
}

void CEtTerrain::SetTextureDistance( int nTexLayer, float fDistance )
{
	ASSERT( nTexLayer < ( int )m_vecTextureDist.size() );

	m_vecTextureDist[ nTexLayer ] = m_TerrainInfo.fTileSize / fDistance;
}

float CEtTerrain::GetTextureDistance( int nTexLayer )
{
	ASSERT( nTexLayer < ( int )m_vecTextureDist.size() );

	return m_TerrainInfo.fTileSize / m_vecTextureDist[ nTexLayer ];
}

void CEtTerrain::SetTextureRotation( int nTexLayer, float fRotate )
{
	ASSERT( nTexLayer < ( int )m_vecTextureRotation.size() );

	m_vecTextureRotation[ nTexLayer ] = fRotate;
	float fRotateRadian = EtToRadian( fRotate );
	m_vecTextureRotationParam[ nTexLayer * 2 ] = cosf( fRotateRadian );
	m_vecTextureRotationParam[ nTexLayer * 2 + 1 ] = sinf( fRotateRadian );
}

float CEtTerrain::GetTextureRotation( int nTexLayer )
{
	ASSERT( nTexLayer < ( int )m_vecTextureRotation.size() );

	return m_vecTextureRotation[ nTexLayer ];
}

void CEtTerrain::DrawRect( int nX, int nY, DWORD dwColor )
{
	EtVector3 Start, End;

	Start.x = m_TerrainInfo.TerrainOffset.x + nX * m_TerrainInfo.fTileSize;
	Start.y = GetHeight( nX, nY ) + 1.0f;
	Start.z = m_TerrainInfo.TerrainOffset.z + nY * m_TerrainInfo.fTileSize;
	nX++;
	End.x = m_TerrainInfo.TerrainOffset.x + nX * m_TerrainInfo.fTileSize;
	End.y = GetHeight( nX, nY ) + 1.0f;
	End.z = m_TerrainInfo.TerrainOffset.z + nY * m_TerrainInfo.fTileSize;
	DrawLine3D( Start, End, dwColor );

	nY++;
	End.x = m_TerrainInfo.TerrainOffset.x + nX * m_TerrainInfo.fTileSize;
	End.y = GetHeight( nX, nY ) + 1.0f;
	End.z = m_TerrainInfo.TerrainOffset.z + nY * m_TerrainInfo.fTileSize;
	DrawLine3D( Start, End, dwColor );

	nX--;
	nY--;
	End.x = m_TerrainInfo.TerrainOffset.x + nX * m_TerrainInfo.fTileSize;
	End.y = GetHeight( nX, nY ) + 1.0f;
	End.z = m_TerrainInfo.TerrainOffset.z + nY * m_TerrainInfo.fTileSize;
	DrawLine3D( Start, End, dwColor );

	nY++;
	End.x = m_TerrainInfo.TerrainOffset.x + nX * m_TerrainInfo.fTileSize;
	End.y = GetHeight( nX, nY ) + 1.0f;
	End.z = m_TerrainInfo.TerrainOffset.z + nY * m_TerrainInfo.fTileSize;
	DrawLine3D( Start, End, dwColor );

	nX++;
	End.x = m_TerrainInfo.TerrainOffset.x + nX * m_TerrainInfo.fTileSize;
	End.y = GetHeight( nX, nY ) + 1.0f;
	End.z = m_TerrainInfo.TerrainOffset.z + nY * m_TerrainInfo.fTileSize;
	DrawLine3D( Start, End, dwColor );
}

void CEtTerrain::SetLightMap( EtTextureHandle hLightMap )
{
	SAFE_RELEASE_SPTR( m_hLightMap );
	m_hLightMap = hLightMap;

	EtVector4 TerrainBlockSize;
	EtVector4 vPixelSize;
	int nTexIndex = -1;
	nTexIndex = m_hLightMap->GetMyIndex();
	AddCustomParam( m_vecCustomParam, EPT_TEX, m_hMaterial, "g_LightMap", &nTexIndex );
	TerrainBlockSize.x = ( m_TerrainInfo.nSizeX + m_TerrainInfo.nSizeX * 2.0f / m_hLightMap->Width() ) * m_TerrainInfo.fTileSize;
	TerrainBlockSize.y = ( m_TerrainInfo.nSizeY + m_TerrainInfo.nSizeY * 2.0f / m_hLightMap->Height() ) * m_TerrainInfo.fTileSize;
	AddCustomParam( m_vecCustomParam, EPT_VECTOR, m_hMaterial, "g_TerrainBlockSize", &TerrainBlockSize );
	vPixelSize.x = 1.0f / m_hLightMap->Width();
	vPixelSize.y = 1.0f / m_hLightMap->Height();
	AddCustomParam( m_vecCustomParam, EPT_VECTOR, m_hMaterial, "g_fPixelSize", &vPixelSize );
}

EtTextureHandle CEtTerrain::GetLightMap()
{
	return m_hLightMap ? m_hLightMap : CEtTexture::GetWhiteTexture();
}
/////////////////////////////////////////////////////////////////////////////////////////
// CEtDetailTerrain
/////////////////////////////////////////////////////////////////////////////////////////
CEtDetailTerrain::CEtDetailTerrain()
{
	m_nTechniqueIndex = 1;
	SetTextureLayerCount( 8 );
}

CEtDetailTerrain::~CEtDetailTerrain()
{
}

void CEtDetailTerrain::CreateMaterial( const char *pEffectName )
{
	CEtTerrain::CreateMaterial( pEffectName );

	int i;
	char szParamName[ 64 ];

	AddCustomParam( m_vecCustomParam, EPT_VECTOR_PTR, m_hMaterial, "g_fTextureDistance2", &m_vecTextureDist[ 4 ] );
	for( i = 0; i < 4; i++ )
	{
		int nTexIndex = -1;
		sprintf( szParamName, "g_LayerFarTex%d", i + 1 );
		AddCustomParam( m_vecCustomParam, EPT_TEX, m_hMaterial, szParamName, &nTexIndex );
		SetTextureParam( i + 4 );
	}
}
 
