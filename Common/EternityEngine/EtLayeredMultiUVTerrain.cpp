#include "StdAfx.h"
#include "EtLoader.h"
#include "EtLayeredMultiUVTerrain.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

extern CSyncLock *g_pEtRenderLock;

CEtCliffTerrain::CEtCliffTerrain(void)
{
	m_nTechniqueIndex = 0;
	SetTextureLayerCount( 4 );
	m_nMaterialName = 1;
}

CEtCliffTerrain::~CEtCliffTerrain(void)
{
}

void CEtCliffTerrain::GenerateTexureCoord()
{
	int i, j;
	EtVector2 *pTexCoord1, *pTexCoord2;
	DWORD *pTerrainAlpha;
	float *pValueX, *pValueZ;
	int nVertexCount;

	nVertexCount = ( m_TerrainInfo.nSizeX + 1 ) * ( m_TerrainInfo.nSizeY + 1 );
	pTexCoord1 = new EtVector2[ nVertexCount ];
	pTexCoord2 = new EtVector2[ nVertexCount ];
	pTerrainAlpha = new DWORD[ nVertexCount ];
	pValueX = new float[ nVertexCount ];
	pValueZ = new float[ nVertexCount ];
	for( i = 0; i <= m_TerrainInfo.nSizeY; i++ )
	{
		for( j = 0; j <= m_TerrainInfo.nSizeX; j++ )
		{
			int nCoordOffset;
			EtVector3 Normal;

			nCoordOffset = VertexOffset( j, i );
			Normal = GetVertexNormal( j, i );

			pValueX[ nCoordOffset ] = CalcAddOffsetX( &Normal );
			pValueZ[ nCoordOffset ] = CalcAddOffsetZ( &Normal );
			pTerrainAlpha[ nCoordOffset ] = CalcBlendWeight( &Normal );
		}
	}
	for( i = 0; i <= m_TerrainInfo.nSizeY; i++ )
	{
		for( j = 0; j <= m_TerrainInfo.nSizeX; j++ )
		{
			int nCoordOffset, nCopyOffset;

			nCoordOffset = VertexOffset( j, i );
			if( pValueX[ nCoordOffset ] == 0.0f )
			{
				pValueX[ nCoordOffset ] = 1.0f;
				nCopyOffset = -1;
				if( ( j < m_TerrainInfo.nSizeX - 2 ) && ( pValueX[ nCoordOffset + 2 ] != 0.0f ) )
				{
					nCopyOffset = nCoordOffset + 2;
				}
				else if( ( j < m_TerrainInfo.nSizeX - 1 ) && ( pValueX[ nCoordOffset + 1 ] != 0.0f ) )
				{
					nCopyOffset = nCoordOffset + 1;
				}
				else if( ( j != 0 ) && ( pValueX[ nCoordOffset - 1 ] != 0.0f ) )
				{
					nCopyOffset = nCoordOffset - 1;
				}
				if( nCopyOffset != -1 )
				{
					pValueX[ nCoordOffset ] = pValueX[ nCopyOffset ];
					pTerrainAlpha[ nCoordOffset ] = pTerrainAlpha[ nCopyOffset ];
				}
			}
			if( pValueZ[ nCoordOffset ] == 0.0f )
			{
				pValueZ[ nCoordOffset ] = 1.0f;
				nCopyOffset = -1;
				if( ( i < m_TerrainInfo.nSizeY - 2 ) && ( pValueZ[ nCoordOffset + m_TerrainInfo.nSizeX + 2 ] != 0.0f ) )
				{
					nCopyOffset = nCoordOffset + m_TerrainInfo.nSizeX + 2;
					pValueZ[ nCoordOffset ] = pValueZ[ nCoordOffset + m_TerrainInfo.nSizeX + 2 ];
					pTerrainAlpha[ nCoordOffset ] = pTerrainAlpha[ nCoordOffset + m_TerrainInfo.nSizeX + 2 ];
				}
				else if( ( i < m_TerrainInfo.nSizeY - 1 ) && ( pValueZ[ nCoordOffset + m_TerrainInfo.nSizeX + 1 ] != 0.0f ) )
				{
					nCopyOffset = nCoordOffset + m_TerrainInfo.nSizeX + 1;
				}
				else if( ( i != 0 ) && ( pValueZ[ nCoordOffset - m_TerrainInfo.nSizeX - 1 ] != 0.0f ) )
				{
					nCopyOffset = nCoordOffset - m_TerrainInfo.nSizeX - 1;
				}
				if( nCopyOffset != -1 )
				{
					pValueZ[ nCoordOffset ] = pValueZ[ nCopyOffset ];
					pTerrainAlpha[ nCoordOffset ] = pTerrainAlpha[ nCopyOffset ];
				}
			}
		}
	}

	float fTextureDistance;

	fTextureDistance = m_TerrainInfo.fTileSize / m_vecTextureDist[ 3 ];
	for( i = 0; i <= m_TerrainInfo.nSizeY; i++ )
	{
		for( j = 0; j <= m_TerrainInfo.nSizeX; j++ )
		{
			int nCoordOffset;
			float fAddCoord, fValue;

			nCoordOffset = VertexOffset( j, i );
			fValue = pValueX[ nCoordOffset ];
			fAddCoord = fValue * GetHeight( j, i ) * m_TerrainInfo.fHeightMultiply / fTextureDistance;
			pTexCoord1[ nCoordOffset ].x = ( m_TerrainInfo.TerrainOffset.z + i * m_TerrainInfo.fTileSize ) 
				/ fTextureDistance;
			pTexCoord1[ nCoordOffset ].y = ( ( m_TerrainInfo.TerrainOffset.x + j * m_TerrainInfo.fTileSize ) 
				/ fTextureDistance - fAddCoord ) * fValue;

			fValue = -pValueZ[ nCoordOffset ];
			fAddCoord = fValue * GetHeight( j, i ) * m_TerrainInfo.fHeightMultiply / fTextureDistance;
			pTexCoord2[ nCoordOffset ].x = ( m_TerrainInfo.TerrainOffset.x + j * m_TerrainInfo.fTileSize ) 
				/ fTextureDistance;
			pTexCoord2[ nCoordOffset ].y = ( ( m_TerrainInfo.TerrainOffset.z + i * m_TerrainInfo.fTileSize ) 
				/ fTextureDistance - fAddCoord ) * fValue;
		}
	}

	CMemoryStream *pStream;	

	pStream = new CMemoryStream( pTexCoord1, nVertexCount * sizeof( EtVector2 ) );
	m_MeshStream.LoadVertexStream( pStream, MST_TEXCOORD, 0, nVertexCount );
	pStream->Initialize( pTexCoord2, nVertexCount * sizeof( EtVector2 ) );
	m_MeshStream.LoadVertexStream( pStream, MST_TEXCOORD, 1, nVertexCount );
	pStream->Initialize( pTerrainAlpha, nVertexCount * sizeof( DWORD ) );
	m_MeshStream.LoadVertexStream( pStream, MST_COLOR, 1, nVertexCount );

	delete [] pValueX;
	delete [] pValueZ;
	delete [] pTexCoord1;
	delete [] pTexCoord2;
	delete [] pTerrainAlpha;
	delete pStream;
}

DWORD CEtCliffTerrain::CalcBlendWeight( EtVector3 *pNormal )
{
	DWORD dwRet;
	EtVector2 Weight, BlendWeight;

	Weight.x = fabs( EtVec3Dot( pNormal, &EtVector3( 0.0f, 0.0f, 1.0f ) ) );
	if( Weight.x == 0 )
	{
		return 0xff000000;
	}
	Weight.y = fabs( EtVec3Dot( pNormal, &EtVector3( 1.0f, 0.0f, 0.0f ) ) );
	if( Weight.y == 0 )
	{
		return 0x00ff0000;
	}
	BlendWeight.x = max( 0.0f, Weight.x - TERRAIN_BLEND_THRESH_HOLD );
	BlendWeight.y = max( 0.0f, Weight.y - TERRAIN_BLEND_THRESH_HOLD );
	if( BlendWeight.x + BlendWeight.y <= 0.0f )
	{
		if( Weight.x >= Weight.y )
		{
			BlendWeight.x = 1.0f;
		}
		else
		{
			BlendWeight.y = 1.0f;
		}
	}
	else
	{
		float fAddBlend;

		fAddBlend = BlendWeight.x + BlendWeight.y;
		BlendWeight.x /= fAddBlend;
		BlendWeight.y /= fAddBlend;
	}

	dwRet = ( ( DWORD )( BlendWeight.y * 255 ) << 24 ) + ( ( DWORD )( BlendWeight.x * 255 ) << 16 );

	return dwRet;
}

float CEtCliffTerrain::CalcAddOffsetX( EtVector3 *pFaceNormal )
{
	float fDot1, fDot2;

	if( EtVec3Dot( &EtVector3( 0.0f, 1.0f, 0.0f ), pFaceNormal ) > 0.995f )
	{
		return 0.0f;
	}
	fDot1 = EtVec3Dot( &EtVector3( -1.0f, 0.0f, 0.0f ), pFaceNormal );
	fDot2 = EtVec3Dot( &EtVector3( 1.0f, 0.0f, 0.0f ), pFaceNormal );

	if( fDot1 > fDot2 )
	{
		return -1.0f;
	}
	else if( fDot1 < fDot2 )
	{
		return 1.0f;
	}
	else
	{
		return 0.0f;
	}
}


float CEtCliffTerrain::CalcAddOffsetZ( EtVector3 *pFaceNormal )
{
	float fDot1, fDot2;

	if( EtVec3Dot( &EtVector3( 0.0f, 1.0f, 0.0f ), pFaceNormal ) > 0.995f )
	{
		return 0.0f;
	}
	fDot1 = EtVec3Dot( &EtVector3( 0.0f, 0.0f, -1.0f ), pFaceNormal );
	fDot2 = EtVec3Dot( &EtVector3( 0.0f, 0.0f, 1.0f ), pFaceNormal );
	if( fDot1 > fDot2 )
	{
		return 1.0f;
	}
	else if( fDot1 < fDot2 )
	{
		return -1.0f;
	}
	else
	{
		return 0.0f;
	}
}


/////////////////////////////////////////////////////////////////////////////////////////
// CEtDetailCliffTerrain
/////////////////////////////////////////////////////////////////////////////////////////
CEtDetailCliffTerrain::CEtDetailCliffTerrain()
{
	m_nTechniqueIndex = 1;
	SetTextureLayerCount( 8 );
}

CEtDetailCliffTerrain::~CEtDetailCliffTerrain()
{
}

void CEtDetailCliffTerrain::CreateMaterial( const char *pEffectName )
{
	CEtCliffTerrain::CreateMaterial( pEffectName );

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

/////////////////////////////////////////////////////////////////////////////////////////
// CEtLowDetailTerrain
/////////////////////////////////////////////////////////////////////////////////////////
CEtLowDetailTerrain::CEtLowDetailTerrain()
{
	m_nMaterialName = 0;
}

CEtLowDetailTerrain::~CEtLowDetailTerrain()
{
}

void CEtLowDetailTerrain::Clear()
{
	SAFE_RELEASE_SPTR( m_hEntireMap );
	CEtTerrain::Clear();	
}

void CEtLowDetailTerrain::InitializeTerrain()
{
	SAFE_RELEASE_SPTR( m_hMaterial ) ;
	m_hMaterial = LoadResource( "LowDetailTerrain.fx", RT_SHADER, true );
	SAFE_RELEASE_SPTR( m_hLightMap );

	for( int i = 0; i < ( int )m_vecCustomParam.size(); i++ ) {
		if( m_vecCustomParam[ i ].Type == EPT_TEX ) {
			EtResourceHandle hHandle;
			if( m_vecCustomParam[ i ].nTextureIndex != -1 ) {
				hHandle = CEtResource::GetResource( m_vecCustomParam[ i ].nTextureIndex );
				SAFE_RELEASE_SPTR( hHandle );
			}
		}
	}
	m_vecCustomParam.clear();
	m_MeshStream.Clear();

	int nVertexCount = ( m_TerrainInfo.nSizeX / 2 + 1 ) * ( m_TerrainInfo.nSizeY / 2 + 1 );
	EtVector3 *pPosition = new EtVector3[ nVertexCount ];
	
	int i, j;
	int nSizeX, nSizeY;
	nSizeX = m_TerrainInfo.nSizeX / 2 + 1;
	nSizeY = m_TerrainInfo.nSizeY / 2 + 1;
	for( i = 0; i < nSizeY; i++ )
	{
		for( j = 0; j < nSizeX; j++ )
		{
			pPosition[ i * nSizeX + j ].x = j * m_TerrainInfo.fTileSize * 2;
			pPosition[ i * nSizeX + j ].y = GetHeight( j * 2, i * 2 );
			pPosition[ i * nSizeX + j ].z = i * m_TerrainInfo.fTileSize * 2;
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

	CMemoryStream *pStream = new CMemoryStream( pPosition, nVertexCount * sizeof( EtVector3 ) );
	m_MeshStream.LoadVertexStream( pStream, MST_POSITION, 0, nVertexCount );
	delete pStream;	
	delete [] pPosition;

	int nTexIndex = -1;
	nTexIndex = m_hEntireMap->GetMyIndex();
	AddCustomParam( m_vecCustomParam, EPT_TEX, m_hMaterial, "g_EntireTex", &nTexIndex );

	int nMapSize = m_hEntireMap->Width() * 2;
	EtVector4 TerrainBlockSize;
	TerrainBlockSize.x = ( m_TerrainInfo.nSizeX + m_TerrainInfo.nSizeX * 2.0f / nMapSize ) * m_TerrainInfo.fTileSize;
	TerrainBlockSize.y = ( m_TerrainInfo.nSizeY + m_TerrainInfo.nSizeY * 2.0f / nMapSize ) * m_TerrainInfo.fTileSize;
	AddCustomParam( m_vecCustomParam, EPT_VECTOR, m_hMaterial, "g_TerrainBlockSize", &TerrainBlockSize );

	EtVector4 vPixelSize;
	vPixelSize.x = 1.0f / nMapSize;
	vPixelSize.y = 1.0f / nMapSize;
	AddCustomParam( m_vecCustomParam, EPT_VECTOR, m_hMaterial, "g_fPixelSize", &vPixelSize );
}

void CEtLowDetailTerrain::BakeLowDetailMap()
{
	//const int TEXTURE_SIZE = 512;
	// #40097 클라이언트 메모리 최적화 작업에 따라 64사이즈로 처리.
	const int TEXTURE_SIZE = 64;

	m_hEntireMap = CEtTexture::CreateRenderTargetTexture( TEXTURE_SIZE, TEXTURE_SIZE, FMT_A8R8G8B8 );

	EtSurface *pBackupRenderTarget = GetEtDevice()->GetRenderTarget();
	EtSurface *pBackupDepthTarget = GetEtDevice()->GetDepthStencilSurface();
	GetEtDevice()->SetRenderTarget( m_hEntireMap->GetSurfaceLevel() );
	GetEtDevice()->SetDepthStencilSurface( NULL);
	GetEtDevice()->ClearBuffer( 0xffffffff, 1.0f, 0, true, false, false);

	GetEtDevice()->BeginScene();
	GetEtDevice()->EnableZ( false );
	GetEtDevice()->SetCullMode( CULL_NONE );

	SCameraInfo CamInfo;
	CamInfo.Type = CT_ORTHOGONAL;
	CamInfo.Target = CT_RENDERTARGET_NO_GENERATE_BACKBUFFER;
	CamInfo.fWidth = TEXTURE_SIZE;
	CamInfo.fHeight = TEXTURE_SIZE;
	CamInfo.fViewWidth = m_TerrainInfo.fTileSize*m_TerrainInfo.nSizeX;
	CamInfo.fViewHeight = m_TerrainInfo.fTileSize*m_TerrainInfo.nSizeY;

	EtCameraHandle hCamera = EternityEngine::CreateCamera( &CamInfo );
	hCamera->LookAt( EtVector3(0,5000,0), EtVector3(0,0,0), EtVector3( 0.0f, 0.0f, 1.0f ) );
	EtMatrix MatScale, *ViewMat;
	EtMatrixScaling(&MatScale, 1.f, -1.f, 1.f);
	ViewMat = hCamera->GetViewMat();
	EtMatrixMultiply(ViewMat, ViewMat, &MatScale);

	EtCameraHandle hCameraBackup = CEtCamera::GetActiveCamera();
	hCamera->Activate();
	EtViewPort m_BackupViewport;
	GetEtDevice()->GetViewport( &m_BackupViewport );
	EtViewPort terrainViewport;
	terrainViewport.X = 0;
	terrainViewport.Y = 0;
	terrainViewport.Width = TEXTURE_SIZE;
	terrainViewport.Height = TEXTURE_SIZE;
	terrainViewport.MinZ = 0;
	terrainViewport.MaxZ = 1;
	GetEtDevice()->SetViewport( &terrainViewport );
	//////////////////////////////////////////////////////////////////////////////////////////////
	EtMatrix WorldMat;
	EtMatrixTranslation(&WorldMat, -(m_TerrainInfo.fTileSize*m_TerrainInfo.nSizeX/2), 0, -(m_TerrainInfo.fTileSize*m_TerrainInfo.nSizeY/2) );

	CEtLight::SetDirLightAttenuation( 1.f );
	m_hMaterial->SetTechnique( m_nTechniqueIndex );
	int nPasses;
	m_hMaterial->BeginEffect( nPasses );
	m_hMaterial->BeginPass( 0 );
	m_hMaterial->SetGlobalParams();

	m_hMaterial->SetWorldMatParams( &WorldMat, &WorldMat );
	m_hMaterial->SetCustomParamList( m_vecCustomParam );
	m_hMaterial->CommitChanges();
	m_MeshStream.Draw( m_hMaterial->GetVertexDeclIndex( m_nTechniqueIndex, 0 ) );
	m_hMaterial->EndPass();
	m_hMaterial->EndEffect();
	///////////////////////////////////////////////////////////////////////////////////////
	GetEtDevice()->SetViewport( &m_BackupViewport );
	GetEtDevice()->SetRenderTarget( pBackupRenderTarget );
	GetEtDevice()->SetDepthStencilSurface( pBackupDepthTarget );
	GetEtDevice()->EndScene();
	SAFE_RELEASE_SPTR( hCamera );
	if( hCameraBackup ) 
	{
		hCameraBackup->Activate();
	}	

	GetEtDevice()->EnableZ( true );

	m_hEntireMap->RemoveRenderTarget();
	
	InitializeTerrain();
}

/////////////////////////////////////////////////////////////////////////////////////////
// CEtTerrainOpti
/////////////////////////////////////////////////////////////////////////////////////////
CEtTerrainOpti::CEtTerrainOpti(void)
{
	m_bProcessLayer = false;
	m_nBakeDepthType = DT_TERRAIN;
	memset(m_nLayerConvertTable, 0, sizeof(m_nLayerConvertTable));
}

CEtTerrainOpti::~CEtTerrainOpti(void)
{
}

void CEtTerrainOpti::CheckLayerCount()
{
	if( m_bProcessLayer )
	{
		return;
	}

	int i, j;
	int nLayerCount[ 4 ];

	memset( nLayerCount, 0, sizeof( int ) * 4 );
	for( i = 0; i < ( m_TerrainInfo.nSizeY + 1 ); i++ )
	{
		for( j = 0; j < ( m_TerrainInfo.nSizeX + 1 ); j++ )
		{
			DWORD dwValue = GetLayerValue( j, i );
			if( dwValue & 0xff000000 )
			{
				nLayerCount[ 0 ]++;
			}
			if( dwValue & 0xff0000 )
			{
				nLayerCount[ 1 ]++;
			}
			if( dwValue & 0xff00 )
			{
				nLayerCount[ 2 ]++;
			}
			if( dwValue & 0xff )
			{
				nLayerCount[ 3 ]++;
			}
		}
	}

	int nCurLayer = 0;
	int nAvailableCount = 0;
	for( i = 0; i < 4; i++ )
	{
		if( nLayerCount[ i ] > 0 )
		{
			m_nLayerConvertTable[ i ] = nCurLayer;
			nCurLayer++;
			nAvailableCount++;
		}
		else
		{
			m_nLayerConvertTable[ i ] = -1;
		}
	}

	for( i = 0; i < ( m_TerrainInfo.nSizeY + 1 ); i++ )
	{
		for( j = 0; j < ( m_TerrainInfo.nSizeX + 1 ); j++ )
		{
			DWORD dwValue = GetLayerValue( j, i );
			EtColor TestColor( dwValue );
			if( m_nLayerConvertTable[ 2 ] == -1 )
			{
				( ( char * )&dwValue )[ 1 ] = ( ( char * )&dwValue )[ 0 ];
			}
			if( m_nLayerConvertTable[ 1 ] == -1 )
			{
				( ( char * )&dwValue )[ 2 ] = ( ( char * )&dwValue )[ 1 ];
				( ( char * )&dwValue )[ 1 ] = ( ( char * )&dwValue )[ 0 ];
			}
			if( m_nLayerConvertTable[ 0 ] == -1 )
			{
				( ( char * )&dwValue )[ 3 ] = ( ( char * )&dwValue )[ 2 ];
				( ( char * )&dwValue )[ 2 ] = ( ( char * )&dwValue )[ 1 ];
				( ( char * )&dwValue )[ 1 ] = ( ( char * )&dwValue )[ 0 ];
			}
			SetLayerValue( j, i, dwValue );
		}
	}

	m_nTechniqueIndex = nAvailableCount - 1;
	// 절벽 지형 사용하고 있지 않으면 쉐이더 바꿔준다.
	if( nLayerCount[ 3 ] == 0 )
	{
		m_nMaterialName = 3;
	}
	else
	{
		m_nMaterialName = 4;
	}
	m_bProcessLayer = true;
}

bool CEtTerrainOpti::SetTexture( int nTexIndex, const char *pTexName )
{
	if( m_nLayerConvertTable[ nTexIndex ] == -1 )
	{
		return true;
	}
	return CEtTerrain::SetTexture( m_nLayerConvertTable[ nTexIndex ], pTexName );
}

void CEtTerrainOpti::GenerateTexureCoord()
{
	if( m_nMaterialName == 3 )
	{
		CEtTerrain::GenerateTexureCoord();
	}
	else
	{
		CEtCliffTerrain::GenerateTexureCoord();
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
// CEtLowSplatTerrain
/////////////////////////////////////////////////////////////////////////////////////////
CEtLowSplatTerrain::CEtLowSplatTerrain( void )
{
	m_nTechniqueIndex = 0;

	// 외부에서 참조하는 곳이 많아서 여기서 선언은 4장으로 하지만
	// 실제 사용하는 것은 2장이다.
	SetTextureLayerCount( 4 );
	m_nMaterialName = 5;
}

CEtLowSplatTerrain::~CEtLowSplatTerrain( void )
{
	
}

void CEtLowSplatTerrain::CreateMaterial( const char* pEffectName )
{
	// 스플래팅 알파 테이블 뒤져서 가장 많이 사용되는 텍스쳐 2장을 찾아서 2장만 바른다.
	char szParamName[ 64 ] = { 0 };
	m_hMaterial = LoadResource( pEffectName, RT_SHADER, true );

	// 텍스쳐는 2장만..
	for( int i = 0; i < 2; i++ )
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
	for( int i = 0; i < 2; i++ )
	{
		int nTexIndex = -1;
		sprintf( szParamName, "g_LayerTex%d", i + 1 );
		AddCustomParam( m_vecCustomParam, EPT_TEX, m_hMaterial, szParamName, &nTexIndex );
		SetTextureParam( i );
	}
}

bool CEtLowSplatTerrain::SetTexture( int nTexIndex, const char *pTexName )
{
	if( nTexIndex <= 2  )
	{
		return __super::SetTexture( nTexIndex, pTexName );
	}

	return false;
}

void CEtLowSplatTerrain::SetTextureDistance( int nTexLayer, float fDistance )
{
	if( nTexLayer <= 2  )
		__super::SetTextureDistance( nTexLayer, fDistance );
}

void CEtLowSplatTerrain::GenerateVertexBuffer( void )
{
	// 버텍스 갯수를 절반만 사용함에 따라 관련 데이터들도 맞춰준다.
	int nVertexCount = ( m_TerrainInfo.nSizeX / 2 + 1 ) * ( m_TerrainInfo.nSizeY / 2 + 1 );
	EtVector3 *pPositionNormalBuffer = new EtVector3[ nVertexCount ];

	int i, j;
	int nSizeX, nSizeY;
	nSizeX = m_TerrainInfo.nSizeX / 2 + 1;
	nSizeY = m_TerrainInfo.nSizeY / 2 + 1;
	for( i = 0; i < nSizeY; i++ )
	{
		for( j = 0; j < nSizeX; j++ )
		{
			pPositionNormalBuffer[ i * nSizeX + j ].x = j * m_TerrainInfo.fTileSize * 2;
			pPositionNormalBuffer[ i * nSizeX + j ].y = GetHeight( j * 2, i * 2 );
			pPositionNormalBuffer[ i * nSizeX + j ].z = i * m_TerrainInfo.fTileSize * 2;
			if( pPositionNormalBuffer[ i * nSizeX + j ].y > m_BoundingBox.Max.y )
			{
				m_BoundingBox.Max.y = pPositionNormalBuffer[ i * nSizeX + j ].y;
			}
			if( pPositionNormalBuffer[ i * nSizeX + j ].y < m_BoundingBox.Min.y )
			{
				m_BoundingBox.Min.y = pPositionNormalBuffer[ i * nSizeX + j ].y;
			}
		}
	}

	CMemoryStream *pStream = new CMemoryStream( pPositionNormalBuffer, nVertexCount * sizeof( EtVector3 ) );
	m_MeshStream.LoadVertexStream( pStream, MST_POSITION, 0, nVertexCount );
	delete pStream;

	SecureZeroMemory( pPositionNormalBuffer, sizeof(EtVector3)*nVertexCount );
	nSizeX = m_TerrainInfo.nSizeX/2 + 1;
	nSizeY = m_TerrainInfo.nSizeY/2 + 1;
	for( i = 0; i < nSizeY; i++ )
	{
		for( j = 0; j < nSizeX; j++ )
		{
			int iBufferPos = i * nSizeX + j;
			if( iBufferPos < nVertexCount )
			{
				pPositionNormalBuffer[ iBufferPos ] = GetVertexNormal( j, i );
			}
		}
	}
	pStream = new CMemoryStream( pPositionNormalBuffer, nVertexCount * sizeof( EtVector3 ) );
	m_MeshStream.LoadVertexStream( pStream, MST_NORMAL, 0, nVertexCount );
	delete pStream;
	delete [] pPositionNormalBuffer;

	DWORD* pBuffer = new DWORD[ nVertexCount+1 ];
	for( i = 0; i <= m_TerrainInfo.nSizeY; i += 2 )
	{
		int nVertexOffset = (i/2) * ( m_TerrainInfo.nSizeX/2 + 1 );
		for( j = 0; j <= m_TerrainInfo.nSizeX; j += 2 )
		{
			int iLayerValue = 0;
			if( ( j + m_nBlockOffsetX >= m_nStride ) || ( i + m_nBlockOffsetY >= m_nStrideVert ) )
			{
				iLayerValue = 0;
			}
			else
			{
				iLayerValue = m_TerrainInfo.pLayerDensity[ i * m_nStride + j ]; 
			}

			int iBufferPos = nVertexOffset + j/2;
			if( iBufferPos < nVertexCount+1 )
			{
				pBuffer[ iBufferPos ] = ConvertLayerValue( iLayerValue );
			}
		}
	}
	pStream = new CMemoryStream( pBuffer, (nVertexCount+1) * sizeof( DWORD ) );
	m_MeshStream.LoadVertexStream( pStream, MST_COLOR, 0, nVertexCount );
	delete pStream;
	delete [] pBuffer;	
}