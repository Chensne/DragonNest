#include "StdAfx.h"
#include "EtRainDropSplash.h"
#include "EngineUtil.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

extern float g_fTotalElapsedTime;

CEtRainDropSplash::CEtRainDropSplash(void)
{
	m_fLifeTime = 0.2f;
	m_fInvLifeTime = 1.0f / m_fLifeTime;
	m_fCreationRadius = 1000.0f;
	m_nSplashPerSecond = 100;
	m_fRemainCreationTime = 0.0f;
	m_nBaseIndex = 0;
	m_nAllocSize = 0;
	m_nPositionParamIndex = 0;
}

CEtRainDropSplash::~CEtRainDropSplash(void)
{
}

void CEtRainDropSplash::Initialize( const char *pSplashTexture, float fSplashSize )
{
	int i, nIndexCount, nVertexCount;
	float fHalfSplashSize, fSplashHeight;
	EtVector3 *pvPosition;
	EtVector2 *pTexCoord;
	WORD *pwIndex, *pwVertexIndex;

	nVertexCount = MAX_RAIN_DROP_SPLASH_COUNT * 4;
	nIndexCount = MAX_RAIN_DROP_SPLASH_COUNT * 6;

	pvPosition = new EtVector3[ nVertexCount ];
	pTexCoord = new EtVector2[ nVertexCount ];
	pwVertexIndex = new WORD[ nVertexCount * 4 ];
	pwIndex = new WORD[ nIndexCount ];

	fHalfSplashSize = fSplashSize * 0.5f;
	fSplashHeight = fSplashSize / ( GetEtDevice()->Height() / ( float )GetEtDevice()->Width() );
	for( i = 0; i < MAX_RAIN_DROP_SPLASH_COUNT; i++ )
	{
		pvPosition[ i * 4 ] = EtVector3( -fHalfSplashSize, fSplashHeight, 0.0f );
		pvPosition[ i * 4 + 1 ] = EtVector3( fHalfSplashSize, fSplashHeight, 0.0f );
		pvPosition[ i * 4 + 2 ] = EtVector3( -fHalfSplashSize, 0.0f, 0.0f );
		pvPosition[ i * 4 + 3 ] = EtVector3( fHalfSplashSize, 0.0f, 0.0f );

		pTexCoord[ i * 4 ] = EtVector2( 0.0f, 0.0f );
		pTexCoord[ i * 4 + 1 ] = EtVector2( 1.0f, 0.0f );
		pTexCoord[ i * 4 + 2 ] = EtVector2( 0.0f, 1.0f );
		pTexCoord[ i * 4 + 3 ] = EtVector2( 1.0f, 1.0f );

		pwVertexIndex[ i * 4 * 4 ] = i;
		pwVertexIndex[ ( i * 4 + 1 ) * 4 ] = i;
		pwVertexIndex[ ( i * 4 + 2 ) * 4 ] = i;
		pwVertexIndex[ ( i * 4 + 3 ) * 4 ] = i;

		pwIndex[ i * 6 ] = i * 4;
		pwIndex[ i * 6 + 1 ] = i * 4 + 1;
		pwIndex[ i * 6 + 2 ] = i * 4 + 2;
		pwIndex[ i * 6 + 3 ] = i * 4 + 1;
		pwIndex[ i * 6 + 4 ] = i * 4 + 3;
		pwIndex[ i * 6 + 5 ] = i * 4 + 2;
	}

	CMemoryStream Stream;

	Stream.Initialize( pvPosition, nVertexCount * sizeof( EtVector3 ) );
	m_MeshStream.LoadVertexStream( &Stream, MST_POSITION, 0, nVertexCount );
	Stream.Initialize( pTexCoord, nVertexCount * sizeof( EtVector2 ) );
	m_MeshStream.LoadVertexStream( &Stream, MST_TEXCOORD, 0, nVertexCount );
	Stream.Initialize( pwVertexIndex, nVertexCount * sizeof( WORD ) * 4 );
	m_MeshStream.LoadVertexStream( &Stream, MST_BONEINDEX, 0, nVertexCount );
	Stream.Initialize( pwIndex, nIndexCount * sizeof( WORD ) );
	m_MeshStream.LoadIndexStream( &Stream, false, nIndexCount );

	delete [] pvPosition;
	delete [] pTexCoord;
	delete [] pwVertexIndex;
	delete [] pwIndex;

	int nTexIndex;

	CalcAllocSize();
	m_hMaterial = LoadResource( "RainDropSplash.fx", RT_SHADER );	
	m_hTexture = LoadResource( pSplashTexture, RT_TEXTURE );
	if( m_hTexture )
	{
		nTexIndex = m_hTexture->GetMyIndex();
		AddCustomParam( m_vecCustomParam, EPT_TEX, m_hMaterial, "g_RainDropSplashTex", &nTexIndex );
	}
	m_nPositionParamIndex = AddCustomParam( m_vecCustomParam, EPT_VECTOR_PTR, m_hMaterial, "g_RainDropSplashPos", NULL, MAX_RAIN_DROP_SPLASH_COUNT );
	AddCustomParam( m_vecCustomParam, EPT_FLOAT_PTR, m_hMaterial, "g_fInvLifeTime", &m_fInvLifeTime );
}

void CEtRainDropSplash::CalcAllocSize()
{
	m_nAllocSize = ( int )( m_nSplashPerSecond * m_fLifeTime * 2 );
	m_vecPosition.reserve( m_nAllocSize );
}

void CEtRainDropSplash::SetLifeTime( float fLifeTime )
{ 
	m_fLifeTime = fLifeTime;
	m_fInvLifeTime = 1.0f / m_fLifeTime;
	CalcAllocSize();
}

void CEtRainDropSplash::SetSplashPerSecond( int nSplash )
{ 
	m_nSplashPerSecond = nSplash; 
	CalcAllocSize();
}

void CEtRainDropSplash::Process( float fElapsedTime )
{
	int i, nCreationCount;

	for( i = m_nBaseIndex; i < ( int )m_vecPosition.size(); i++ )
	{
		if( m_vecPosition[ i ].w >= ( g_fTotalElapsedTime - m_fLifeTime ) )
		{
			m_nBaseIndex = i;
			break;
		}
	}

	EtCameraHandle hCamera;
	EtVector3 *pvCamPosition, vCenterPosition;
	EtMatrix *pInvViewMat;

	hCamera = CEtCamera::GetActiveCamera();
	pvCamPosition = hCamera->GetPosition();
	pInvViewMat = hCamera->GetInvViewMat();
	vCenterPosition.x = pvCamPosition->x + pInvViewMat->_31 * ( m_fCreationRadius * 1.0f );
	vCenterPosition.y = pvCamPosition->y + pInvViewMat->_32 * ( m_fCreationRadius * 1.0f );
	vCenterPosition.z = pvCamPosition->z + pInvViewMat->_33 * ( m_fCreationRadius * 1.0f );

	m_fRemainCreationTime += fElapsedTime;
	nCreationCount = ( int )( m_fRemainCreationTime * m_nSplashPerSecond );
	m_fRemainCreationTime -= nCreationCount / ( float )m_nSplashPerSecond;
	for( i = 0; i < nCreationCount; i++ )
	{
		EtVector4 vSplashPosition;
		EtTerrainHandle hTerrain;

		if( ( int )m_vecPosition.size() >= m_nAllocSize )
		{
			if( m_nBaseIndex <= 0 )
			{
				break;
			}
			else
			{
				m_vecPosition.erase( m_vecPosition.begin(), m_vecPosition.begin() + m_nBaseIndex );
				m_nBaseIndex = 0;
			}
		}

		vSplashPosition.x = RandomNumberInRange( -m_fCreationRadius, m_fCreationRadius ) + vCenterPosition.x;
		vSplashPosition.y = 0.0f;
		vSplashPosition.z = RandomNumberInRange( -m_fCreationRadius, m_fCreationRadius ) + vCenterPosition.z;
		hTerrain = CEtTerrainArea::GetTerrainArea( vSplashPosition.x, vSplashPosition.z );
		if( hTerrain )
		{
			vSplashPosition.y = hTerrain->GetLandHeight( vSplashPosition.x, vSplashPosition.z, NULL );
		}
		vSplashPosition.w = g_fTotalElapsedTime;

		m_vecPosition.push_back( vSplashPosition );
	}
}

void CEtRainDropSplash::Render( float fElapsedTime )
{
	Process( fElapsedTime );

	EtMatrix WorldMat;
	int nDrawCount, nCurBaseIndex;
	EtCameraHandle hCamera;

	nDrawCount = ( int )m_vecPosition.size() - m_nBaseIndex;

	hCamera = CEtCamera::GetActiveCamera();
	memcpy( &WorldMat, hCamera->GetInvViewMat(), sizeof( EtMatrix ) );
	WorldMat._21 = 0.0f;
	WorldMat._22 = 1.0f;
	WorldMat._23 = 0.0f;
	WorldMat._41 = 0.0f;
	WorldMat._42 = 0.0f;
	WorldMat._43 = 0.0f;

	nCurBaseIndex = m_nBaseIndex;
	while( nDrawCount > 0 )
	{
		int nDraw;
		if( nDrawCount > MAX_RAIN_DROP_SPLASH_COUNT )
		{
			nDraw = MAX_RAIN_DROP_SPLASH_COUNT;
		}
		else
		{
			nDraw = nDrawCount;
		}

		m_vecCustomParam[ m_nPositionParamIndex ].nVariableCount = nDraw;
		if( nDraw <= 1 )
		{
			m_vecCustomParam[ m_nPositionParamIndex ].nVariableCount = 2;
		}

		if( (int)m_vecPosition.capacity() < m_vecCustomParam[ m_nPositionParamIndex ].nVariableCount + nCurBaseIndex ) {
			_ASSERT( false );
			m_vecPosition.reserve( m_vecCustomParam[ m_nPositionParamIndex ].nVariableCount + nCurBaseIndex );
		}

		m_vecCustomParam[ m_nPositionParamIndex ].pPointer = &m_vecPosition[ nCurBaseIndex ];

		SRenderStackElement RenderElement;

		RenderElement.hMaterial = m_hMaterial;
		RenderElement.nTechniqueIndex = 0;
		RenderElement.WorldMat = WorldMat;
		RenderElement.nSaveMatIndex = -1;
		RenderElement.pvecCustomParam = &m_vecCustomParam;
		RenderElement.pRenderMeshStream = &m_MeshStream;
		RenderElement.nDrawCount = nDraw * 2;
		RenderElement.nBakeDepthIndex = DT_NONE;
		GetCurRenderStack()->AddAlphaRenderElement( RenderElement );

		nDrawCount -= nDraw;
		nCurBaseIndex += nDraw;
	}
}

EtRainDropSplashHandle CEtRainDropSplash::CreateRainDropSplash( const char *pSplashTexture, float fSplashSize )
{
	CEtRainDropSplash *pSplashEffect;

	pSplashEffect = new CEtRainDropSplash();
	pSplashEffect->Initialize( pSplashTexture, fSplashSize );

	return pSplashEffect->GetMySmartPtr();
}
