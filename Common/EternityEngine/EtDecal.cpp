#include "StdAfx.h"
#include "EtDecal.h"
#include "EtConvexVolume.h"
#include "EtBackBufferMng.h"
#include "EtOptionController.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

DECL_SMART_PTR_STATIC( CEtDecal, 100 )

CEtDecal::CEtDecal(void)
{
	m_nTechniqueIndex = 0;
	m_nSrcBlend = 0;
	m_nDestBlend = 0;
	m_bInitialize = true;
	m_bUseAddressUVWrap = false;
	m_bUseFixedUV = false;
	m_fAppearTime = 0.0f;
	m_fLifeTime = 0.0f;
	m_fProgressTime = 0.0f;
	m_nFrustumMask = 0;
}

CEtDecal::~CEtDecal(void)
{
	m_DecalStream.Clear();
	SAFE_RELEASE_SPTR( m_hMaterial );
}

void CEtDecal::Initialize( EtTextureHandle hTexture, float fX, float fZ, float fRadius, float fLifeTime, float fAppearTime, float fRotate, 
									EtColor &DecalColor, int nSrcBlend, int nDestBlend, CalcHeightFn *pHeightFunc, float fYHint )
{
	if( !m_hMaterial )
	{
		m_hMaterial = LoadResource( "Decal.fx", RT_SHADER );
	}
#ifdef PRE_FIX_MATERIAL_DUMP
	if( !m_hMaterial ) return;
#endif

	m_nTechniqueIndex = 0;
	m_fLifeTime = fLifeTime;
	m_fAppearTime = fAppearTime;
	m_fProgressTime = 0.f;

	int nTexIndex;
	nTexIndex = ( hTexture ) ? hTexture->GetMyIndex() : -1;
	AddCustomParam( m_vecCustomParam, EPT_TEX, m_hMaterial, "g_DiffuseTex", &nTexIndex );
	m_hDecalTexture = hTexture;
	m_nSrcBlend = nSrcBlend;
	m_nDestBlend = nDestBlend;	

	Update( fX, fZ, fRadius, fRotate, DecalColor, pHeightFunc, fYHint );
}

void CEtDecal::Update( float fX, float fZ, float fRadius, float fRotate, EtColor &DecalColor, CalcHeightFn *pHeightFunc, float fYHint , float fHintThreshold )
{
	int nTileScale = 1;
	SGraphicOption Option;
	GetEtOptionController()->GetGraphicOption( Option );
	if( !Option.bUseSplatting ) {
		nTileScale = 2;
	}


	EtTerrainHandle hTerrainArea;
	int nTileX, nTileZ, nSizeX, nSizeZ;

	hTerrainArea = CEtTerrainArea::GetTerrainArea( fX, fZ );

	float fTileSize = 50.f;
	if( hTerrainArea )
	{
		fTileSize = hTerrainArea->GetTerrainInfo()->fTileSize;	
	}

	m_DecalColor = DecalColor;
	AddCustomParam( m_vecCustomParam, EPT_VECTOR_PTR, m_hMaterial, "g_DecalColor", &m_DecalColor );

	float fTileX = ( fX - fRadius ) / fTileSize;
	float fTileZ = ( fZ - fRadius ) / fTileSize;
	if( fTileX < 0.0f ) fTileX -= 1.0f;
	if( fTileZ < 0.0f ) fTileZ -= 1.0f;
	nTileX = (int)fTileX;
	nTileZ = (int)fTileZ;
	nSizeX = (int)(fRadius * 2.0f / fTileSize + 1.0f);
	nSizeZ = (int)(fRadius * 2.0f / fTileSize + 1.0f);

	EtMatrix TransMat;
	EtVector3 vTransU, vTransV;
	EtMatrix FixedUVMat;
	EtMatrixIdentity( &FixedUVMat );

	EtMatrixIdentity( &TransMat );
	TransMat._11 = 1.0f / ( fRadius * 2.0f );
	TransMat._22 = TransMat._11;
	TransMat._33 = 0.0f;
	TransMat._41 = ( fRadius - fX ) / ( fRadius * 2.0f );
	TransMat._42 = ( fRadius - fZ ) / ( fRadius * 2.0f );
	TransMat._43 = 0.0f;
	if( fRotate != 0.0f )
	{
		EtMatrix RotateMat, MoveMat;

		EtMatrixTranslation( &MoveMat, -0.5f, -0.5f, 0.0f );
		EtMatrixRotationZ( &RotateMat, EtToRadian( fRotate ) );
		EtMatrixMultiply( &RotateMat, &MoveMat, &RotateMat );
		EtMatrixTranslation( &MoveMat, 0.5f, 0.5f, 0.0f );
		EtMatrixMultiply( &RotateMat, &RotateMat, &MoveMat );
		EtMatrixMultiply( &TransMat, &TransMat, &RotateMat );
		FixedUVMat = RotateMat;
	}
	vTransU.x = TransMat._11;
	vTransU.y = TransMat._21;
	vTransU.z = TransMat._41;
	vTransV.x = TransMat._12;
	vTransV.y = TransMat._22;
	vTransV.z = TransMat._42;

	int i, j;
	int nVertexCount, nIndexCount;
	EtVector3 *pVertex;
	EtVector2 *pTexCoord;
	float *pAlpha;
	WORD *pIndexBuf;
	CMemoryStream Stream;

	nVertexCount = ( nSizeX + 1 ) * ( nSizeZ + 1 );
	nIndexCount = nSizeX * nSizeZ * 6;
	pVertex = new EtVector3[ nVertexCount ];
	pTexCoord = new EtVector2[ nVertexCount ];
	pAlpha = new float[ nVertexCount ];
	pIndexBuf = ( WORD * )pVertex;
	m_BoundingBox.Reset();
	
	bool bFixedY = false;
	float fFixedY = 0.f;
	if( pHeightFunc ) {
		float fMinY = FLT_MAX;
		float fMaxY = -FLT_MAX;
		for( i = 0; i <= nSizeZ; i++ ) {
			for( j = 0; j <= nSizeX; j++ ) {
				float fPointX, fPointZ;				
				fPointX = ( nTileX + j ) * fTileSize;
				fPointZ = ( nTileZ + i ) * fTileSize;				
 				float fY = pHeightFunc->GetHeight( fPointX, fYHint, fPointZ, nTileScale );
				if( fY > fMaxY ) fMaxY = fY;
				if( fY < fMinY ) fMinY = fY;
			}
		}
		if( fMaxY-fMinY > fHintThreshold ) {		// 차이가 너무 크면 힌트Y 값과 가까운 값을 사용한다.
			bFixedY = true;			
			if( fabsf( fMaxY - fYHint )  <  fabsf( fMinY - fYHint ) ) {
				fFixedY = fMaxY;
			}
			else {
				fFixedY = fMinY;				
			}
		}
	}

	for( i = 0; i <= nSizeZ; i++ )
	{
		for( j = 0; j <= nSizeX; j++ )
		{
			int nIndex;
			float fPointX, fPointZ;

			static float fBiasY = 1.0f;

			nIndex = ( nSizeX + 1 ) * i + j;
			fPointX = ( nTileX + j ) * fTileSize;
			fPointZ = ( nTileZ + i ) * fTileSize;
			if( hTerrainArea ) {
				if( bFixedY ) {
					pVertex[ nIndex ] = EtVector3( fPointX, fFixedY + fBiasY, fPointZ );
				}
				else if( pHeightFunc ) {
					pVertex[ nIndex ] = EtVector3( fPointX, pHeightFunc->GetHeight( fPointX, fYHint, fPointZ, nTileScale ) + fBiasY, fPointZ );
				}
				else {
					pVertex[ nIndex ] = EtVector3( fPointX, hTerrainArea->GetLandHeight( fPointX, fPointZ, NULL, nTileScale ) + fBiasY, fPointZ ); 
				}
			}
			else {
				pVertex[ nIndex ] = EtVector3( fPointX, fBiasY, fPointZ ); 
			}
			if( !m_bUseFixedUV ) {
				pTexCoord[ nIndex ].x = pVertex[ nIndex ].x * vTransU.x + pVertex[ nIndex ].z * vTransU.y + vTransU.z;
				pTexCoord[ nIndex ].y = 1.f - (pVertex[ nIndex ].x * vTransV.x + pVertex[ nIndex ].z * vTransV.y + vTransV.z);
			}
			else {
				float fBaseU = (float)j / nSizeX;
				float fBaseV = (float)i / nSizeZ;
				pTexCoord[ nIndex ].x = fBaseU * FixedUVMat._11 + fBaseV * FixedUVMat._21 + FixedUVMat._41;
				pTexCoord[ nIndex ].y = 1.0f - (fBaseU * FixedUVMat._12 + fBaseV * FixedUVMat._22 + FixedUVMat._42);
			}
			EtVec3Maximize( &m_BoundingBox.Max, &m_BoundingBox.Max, pVertex + nIndex );
			EtVec3Minimize( &m_BoundingBox.Min, &m_BoundingBox.Min, pVertex + nIndex );
		}
	}
	const float fDiffThreshold = 350.f;
	for( i = 0; i <= nSizeZ; i++ )
	{
		for( j = 0; j <= nSizeX; j++ )
		{
			int nIndex;
			nIndex = ( nSizeX + 1 ) * i + j;
			pAlpha[ nIndex ] = 1.0f;

			if( i == 0 || i == nSizeZ || j == 0 || j == nSizeX )
				continue;

			// 상하, 좌우, 대각선 모두 체크해야 완전히 검사 다 하는거다.
			if( abs( pVertex[nIndex-nSizeX-1].y - pVertex[nIndex].y ) > fDiffThreshold ||
				abs( pVertex[nIndex+nSizeX+1].y - pVertex[nIndex].y ) > fDiffThreshold ||
				abs( pVertex[nIndex-1].y - pVertex[nIndex].y ) > fDiffThreshold ||
				abs( pVertex[nIndex+1].y - pVertex[nIndex].y ) > fDiffThreshold ||
				abs( pVertex[nIndex-nSizeX-1-1].y - pVertex[nIndex].y ) > fDiffThreshold ||
				abs( pVertex[nIndex-nSizeX-1+1].y - pVertex[nIndex].y ) > fDiffThreshold ||
				abs( pVertex[nIndex+nSizeX+1-1].y - pVertex[nIndex].y ) > fDiffThreshold ||
				abs( pVertex[nIndex+nSizeX+1+1].y - pVertex[nIndex].y ) > fDiffThreshold )
				pAlpha[ nIndex ] = 0.0f;
		}
	}
	if( m_DecalStream.GetVertexCount() < nVertexCount ) {
		m_DecalStream.Clear();
	}
	Stream.Initialize( pVertex, sizeof( EtVector3 ) * nVertexCount );
	m_DecalStream.LoadVertexStream( &Stream, MST_POSITION, 0, nVertexCount );
	Stream.Initialize( pTexCoord, sizeof( EtVector2 ) * nVertexCount );
	m_DecalStream.LoadVertexStream( &Stream, MST_TEXCOORD, 0, nVertexCount );
	Stream.Initialize( pAlpha, sizeof( float ) * nVertexCount );
	m_DecalStream.LoadVertexStream( &Stream, MST_DEPTH, 0, nVertexCount );
	for( i = 0; i < nSizeZ; i++ )
	{
		for( j = 0; j < nSizeX; j++ )
		{
			int nIndex, nBaseVertex;

			nIndex = ( nSizeX * i + j ) * 6;
			nBaseVertex = i * ( nSizeX + 1 ) + j;
			pIndexBuf[ nIndex ] = nBaseVertex;
			pIndexBuf[ nIndex + 1 ] = nBaseVertex + nSizeX + 1;
			pIndexBuf[ nIndex + 2 ] = nBaseVertex + nSizeX + 2;
			pIndexBuf[ nIndex + 3 ] = nBaseVertex;
			pIndexBuf[ nIndex + 4 ] = nBaseVertex + nSizeX + 2;
			pIndexBuf[ nIndex + 5 ] = nBaseVertex + 1;
		}
	}
	Stream.Initialize( pIndexBuf, sizeof( WORD ) * nIndexCount );
	m_DecalStream.LoadIndexStream( &Stream, false, nIndexCount );

	delete [] pVertex;
	delete [] pTexCoord;
	delete [] pAlpha;
}

void CEtDecal::GetExtent( EtVector3 &Origin, EtVector3 &Extent )
{
	Origin = ( m_BoundingBox.Max + m_BoundingBox.Min ) / 2;
	Extent = ( m_BoundingBox.Max - m_BoundingBox.Min ) / 2;
}

bool CEtDecal::Process( float fElapsedTime )
{
	if( !m_bInitialize ) {
		m_fLifeTime -= fElapsedTime;	
	}	

	if( m_fProgressTime < m_fAppearTime ) {
		float fAlpha = m_fProgressTime / m_fAppearTime;
		m_DecalColor.a = fAlpha;
	}
	m_fProgressTime += fElapsedTime;

	if( m_fLifeTime < 0.0f )
	{
		return true;
	}
	if( !m_bInitialize && m_fLifeTime <= 0.5f ) {
		float fAlpha = (m_fLifeTime / 0.5f);
		m_DecalColor.a = fAlpha;
	}

	m_bInitialize = false;

	return false;
}

void CEtDecal::Render()
{
	if( m_DecalStream.GetIndexCount() == 0 )
	{
		return;
	}

	SRenderStackElement RenderElement;
	EtMatrix WorldMat;

	EtMatrixIdentity( &WorldMat );
	RenderElement.hMaterial = m_hMaterial;
	RenderElement.nTechniqueIndex = m_nTechniqueIndex;
	RenderElement.WorldMat = WorldMat;
	RenderElement.nSaveMatIndex = -1;
	RenderElement.pvecCustomParam = &m_vecCustomParam;
	RenderElement.pRenderMeshStream = &m_DecalStream;
	RenderElement.renderPriority = IsPointLight() ? RP_HIGH : RP_ABOVE_NORMAL;
	RenderElement.nBakeDepthIndex = DT_NONE;

	int nDiffuseSamplerIndex = 0;
	int nParamIndex = m_hMaterial->GetParameterByName( "g_DiffuseTex" );
	for( int i = 0; i < m_hMaterial->GetParameterCount(); ++i ) {
		if( i == nParamIndex )
			break;
		if( m_hMaterial->GetParameterType( i ) == D3DXPT_TEXTURE2D ) {
			++nDiffuseSamplerIndex;
		}
	}
	SStateBlock RenderStateBlock;
	RenderStateBlock.AddSamplerState( D3DSAMP_ADDRESSU, m_bUseAddressUVWrap ? TADDRESS_WRAP : TADDRESS_CLAMP, nDiffuseSamplerIndex );
	RenderStateBlock.AddSamplerState( D3DSAMP_ADDRESSV, m_bUseAddressUVWrap ? TADDRESS_WRAP : TADDRESS_CLAMP, nDiffuseSamplerIndex );
	RenderStateBlock.AddSamplerState( D3DSAMP_BORDERCOLOR, 0 );

	if( m_nSrcBlend != 0 ) RenderStateBlock.AddRenderState( D3DRS_SRCBLEND, m_nSrcBlend);
	if( m_nDestBlend != 0 ) 	RenderStateBlock.AddRenderState( D3DRS_DESTBLEND, m_nDestBlend);
	RenderElement.nStateBlockIndex = CEtStateBlockManager::GetInstance().CreateStateBlock( &RenderStateBlock );

	if( IsPointLight() ) {
		GetCurRenderStack()->AddUseBackBufferRenderElement( RenderElement );
	}
	else {
		GetCurRenderStack()->AddAlphaRenderElement( RenderElement );
	}
}

void CEtDecal::RenderDecalList( int nMask, float fElapsedTime )
{
	ScopeLock<CSyncLock> Lock( s_SmartPtrLock );

	int i, nCount;
	EtDecalHandle hHandle;
	std::vector< EtDecalHandle > vecDeleteList;

	nCount = GetItemCount();
	for( i = 0; i < nCount; i++ )
	{
		hHandle = GetItem( i );
		if( !hHandle )
		{
			continue;
		}
		if( hHandle->Process( fElapsedTime ) )
		{
			vecDeleteList.push_back( hHandle );
			continue;
		}
		if( hHandle->GetFrustumMask() & nMask )
		{
			hHandle->Render();
		}
	}

	nCount = ( int )vecDeleteList.size();
	for( i = 0; i < nCount; i++ )
	{
		SAFE_RELEASE_SPTR( vecDeleteList[ i ] );
	}
}

void CEtDecal::ProcessDeleteDecalList( float fElapsedTime )
{
	ScopeLock<CSyncLock> Lock( s_SmartPtrLock );

	int i, nCount;
	EtDecalHandle hHandle;
	std::vector< EtDecalHandle > vecDeleteList;

	nCount = GetItemCount();
	for( i = 0; i < nCount; i++ )
	{
		hHandle = GetItem( i );
		if( !hHandle )
		{
			continue;
		}
		if( hHandle->Process( fElapsedTime ) )
		{
			vecDeleteList.push_back( hHandle );
			continue;
		}
	}

	nCount = ( int )vecDeleteList.size();
	for( i = 0; i < nCount; i++ )
	{
		SAFE_RELEASE_SPTR( vecDeleteList[ i ] );
	}
}

void CEtDecal::ClearFrustumMask()
{
	ScopeLock<CSyncLock> Lock( s_SmartPtrLock );

	int i, nCount;

	nCount = GetItemCount();
	for( i = 0; i < nCount; i++ )
	{
		GetItem( i )->SetFrustumMask( 0 );
	}
}

void CEtDecal::MaskFrustumDecalList( CEtConvexVolume *pFrustum, int nFrustumMask )
{
	ScopeLock<CSyncLock> Lock( s_SmartPtrLock );

	int i, nCount;

	nCount = GetItemCount();
	for( i = 0; i < nCount; i++ )
	{
		EtVector3 Origin, Extent;
		EtDecalHandle hHandle;
		hHandle = GetItem( i );
		hHandle->GetExtent( Origin, Extent );
		if( pFrustum->TesToBox( Origin, Extent ) )
		{
			hHandle->AddFrustumMask( nFrustumMask );
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// CEtPointLightDecal
///////////////////////////////////////////////////////////////////////////////////////////////
CEtPointLightDecal::CEtPointLightDecal(void)
{
}

CEtPointLightDecal::~CEtPointLightDecal(void)
{
}

void CEtPointLightDecal::Initialize( SLightInfo &LightInfo, float fRadius, float fLifeTime )
{
	if( !m_hMaterial )
	{
		m_hMaterial = LoadResource( "Decal.fx", RT_SHADER );
	}
#ifdef PRE_FIX_MATERIAL_DUMP
	if( !m_hMaterial ) return;
#endif
	m_fLifeTime = fLifeTime;
	m_nTechniqueIndex = 1;

	int nTexIndex;
	nTexIndex = GetEtBackBufferMng()->GetBackBufferIndex();
	AddCustomParam( m_vecCustomParam, EPT_TEX, m_hMaterial, "g_DiffuseTex", &nTexIndex );

	EtMatrixIdentity(&m_MatBias);
	m_MatBias._11 = 0.5f;
	m_MatBias._22 = -0.5f;
	m_MatBias._41 = 0.5f + 0.5f / GetEtDevice()->Width();
	m_MatBias._42 = 0.5f + 0.5f / GetEtDevice()->Height();
	AddCustomParam( m_vecCustomParam, EPT_MATRIX_PTR, m_hMaterial, "g_BiasMat", &m_MatBias );
	Update( LightInfo, fRadius );
}

void CEtPointLightDecal::Update( SLightInfo &LightInfo, float fRadius )
{
	EtTerrainHandle hTerrainArea;
	STerrainInfo *pTerrainInfo;
	int nTileX, nTileZ, nSizeX, nSizeZ;
	float fX, fZ;

	fX = LightInfo.Position.x;
	fZ = LightInfo.Position.z;
	hTerrainArea = CEtTerrainArea::GetTerrainArea( fX, fZ );
	if( !hTerrainArea )
	{
		return;
	}

	pTerrainInfo = hTerrainArea->GetTerrainInfo();
	nTileX = ( int )( ( fX - fRadius ) / pTerrainInfo->fTileSize ) - 1;
	nTileZ = ( int )( ( fZ - fRadius ) / pTerrainInfo->fTileSize ) - 1;
	nSizeX = ( int )( ( fX + fRadius) / pTerrainInfo->fTileSize ) - nTileX;
	nSizeZ = ( int )( ( fZ + fRadius) / pTerrainInfo->fTileSize ) - nTileZ;

	int i, j;
	int nVertexCount, nIndexCount;
	EtVector3 *pVertex;
	EtColor *pColor;
	WORD *pIndexBuf;
	CMemoryStream Stream;

	nVertexCount = ( nSizeX + 1 ) * ( nSizeZ + 1 );
	nIndexCount = nSizeX * nSizeZ * 6;
	pVertex = new EtVector3[ nVertexCount ];
	pColor = new EtColor[ nVertexCount ];
	pIndexBuf = ( WORD * )pVertex;
	m_BoundingBox.Reset();
	for( i = 0; i <= nSizeZ; i++ )
	{
		for( j = 0; j <= nSizeX; j++ )
		{
			int nIndex;
			float fPointX, fPointZ;
			EtVector3 vNormal;

			nIndex = ( nSizeX + 1 ) * i + j;
			fPointX = ( nTileX + j ) * pTerrainInfo->fTileSize;
			fPointZ = ( nTileZ + i ) * pTerrainInfo->fTileSize;
			pVertex[ nIndex ] = EtVector3( fPointX, hTerrainArea->GetLandHeight( fPointX, fPointZ, &vNormal ) + 1.0f, fPointZ ); 
			CalcPointLight( pColor[ nIndex ], LightInfo, pVertex[ nIndex ], vNormal );
			EtVec3Maximize( &m_BoundingBox.Max, &m_BoundingBox.Max, pVertex + nIndex );
			EtVec3Minimize( &m_BoundingBox.Min, &m_BoundingBox.Min, pVertex + nIndex );
		}
	}
	Stream.Initialize( pVertex, sizeof( EtVector3 ) * nVertexCount );
	m_DecalStream.LoadVertexStream( &Stream, MST_POSITION, 0, nVertexCount );
	Stream.Initialize( pColor, sizeof( EtColor ) * nVertexCount );
	m_DecalStream.LoadVertexStream( &Stream, MST_BONEWEIGHT, 0, nVertexCount );
	for( i = 0; i < nSizeZ; i++ )
	{
		for( j = 0; j < nSizeX; j++ )
		{
			int nIndex, nBaseVertex;

			nIndex = ( nSizeX * i + j ) * 6;
			nBaseVertex = i * ( nSizeX + 1 ) + j;
			pIndexBuf[ nIndex ] = nBaseVertex;
			pIndexBuf[ nIndex + 1 ] = nBaseVertex + nSizeX + 1;
			pIndexBuf[ nIndex + 2 ] = nBaseVertex + nSizeX + 2;
			pIndexBuf[ nIndex + 3 ] = nBaseVertex;
			pIndexBuf[ nIndex + 4 ] = nBaseVertex + nSizeX + 2;
			pIndexBuf[ nIndex + 5 ] = nBaseVertex + 1;
		}
	}
	Stream.Initialize( pIndexBuf, sizeof( WORD ) * nIndexCount );
	m_DecalStream.LoadIndexStream( &Stream, false, nIndexCount );

	delete [] pVertex;
	delete [] pColor;
}

void CEtPointLightDecal::CalcPointLight( EtColor &OutColor, SLightInfo &LightInfo, EtVector3 &vPosition, EtVector3 &vNormal )
{
	float fLength, fAttenuation;
	EtVector3 vLightDir;

	vLightDir = LightInfo.Position - vPosition;
	fLength = EtVec3Length( &vLightDir );
	vLightDir /= fLength;
	fAttenuation = max( 0.0f, 1.0f - ( fLength / LightInfo.fRange ) );
	OutColor = LightInfo.Diffuse * max( 0.0f , EtVec3Dot( &vNormal, &vLightDir ) ) * fAttenuation;

	OutColor.r += 1.0f;
	OutColor.g += 1.0f;
	OutColor.b += 1.0f;
}
