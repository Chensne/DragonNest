
#include "StdAfx.h"
#include "EtShadowMap.h"
#include "EtLiSPShadowMap.h"
#include "EtTrapezoidShadowMap.h"
#include "EtSprite.h"
#include "EtDrawQuad.h"
#include "EtOptionController.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CEtShadowMap *g_pEtShadowMap = NULL;

CEtShadowMap::CEtShadowMap(void)
{
	m_fShadowRange = 500.0f;
	m_fDistanceLightAt = 500.0f;
	m_ShadowColor = EtColor( 0.0f, 0.0f, 0.0f, 1.0f );
	m_ShadowMapFormat = FMT_UNKNOWN;

	EtMatrixIdentity(&m_LightViewMat);
	D3DXMatrixTranslation(&m_LightProjMat, 0.0f, 0.0f, -10000.0f );
	m_LightProjMat._11 = m_LightProjMat._22 = m_LightProjMat._33 = 0.f;
	m_LightProjDepthMat = m_LightProjMat;
	m_bEnable = true;

	SGraphicOption Option;
	GetEtOptionController()->GetGraphicOption( Option );
	m_bUseSoftShadow = Option.bSoftShadow;

	m_nVertexDecl = -1;
	m_pBackupDepthTarget = NULL;
	m_pBackupRenderTarget = NULL;
}

CEtShadowMap::~CEtShadowMap(void)
{
	Clear();
}

void CEtShadowMap::Clear()
{
	SAFE_RELEASE_SPTR( m_hShadowMap );
	SAFE_RELEASE_SPTR( m_hShadowMapDepth );
	SAFE_RELEASE_SPTR( m_hShadowMaterial );
	SAFE_RELEASE_SPTR( m_hBlurShadowMap );
}

void CEtShadowMap::Initialize( ShadowQuality Quality, ShadowType Type )
{
	switch( Quality )
	{
	case SQ_HIGH:
		m_nShadowMapSize = m_bUseSoftShadow ? 1024 : 2048;
		break;
	case SQ_NORMAL:
		m_nShadowMapSize = 1024;
		break;
	case SQ_NONE:
		m_nShadowMapSize = 512;
		m_bEnable = false;
		break;
	default:
		m_nShadowMapSize = 1024;
		m_bEnable = false;
		break;
	}

	EtFormat ShadowFormat;

	m_ShadowType = Type;
	if( m_ShadowType == ST_SHADOWMAP )
	{
		ShadowFormat = FMT_R5G6B5;
	}
	else if( m_ShadowType == ST_DEPTHSHADOWMAP )
	{
		ShadowFormat = FMT_R32F;
	}
	else
	{
		ShadowFormat = FMT_UNKNOWN;
	}
	if( ( !m_hShadowMap ) || ( m_hShadowMap->Width() != m_nShadowMapSize ) || ( ShadowFormat != m_ShadowMapFormat ) )
	{
		SAFE_RELEASE_SPTR( m_hShadowMap );
		SAFE_RELEASE_SPTR( m_hBlurShadowMap );
		SAFE_RELEASE_SPTR( m_hShadowMapDepth );
		m_ShadowMapFormat = ShadowFormat;
		m_hShadowMap = CEtTexture::CreateRenderTargetTexture( m_nShadowMapSize, m_nShadowMapSize, ShadowFormat );
		m_hShadowMap->SetFileName( "EtShadowMap" );
		if( m_bUseSoftShadow ) {
			m_hBlurShadowMap = CEtTexture::CreateRenderTargetTexture( 3 * m_nShadowMapSize / 4, 3 * m_nShadowMapSize / 4, ShadowFormat );
			m_hBlurShadowMap->SetFileName( "EtBlurShadowMap" );
		}
		if( m_ShadowType == ST_DEPTHSHADOWMAP )
		{
			m_hShadowMapDepth = CEtDepth::CreateDepthStencil( m_nShadowMapSize, m_nShadowMapSize );
			m_hShadowMapDepth->SetFileName( "EtShadowMapDepth" );
		}
	}

	if( m_bUseSoftShadow ) {
		D3DVERTEXELEMENT9 VertexElement[] = {	{0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
																	{0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
																	D3DDECL_END() };
		m_nVertexDecl = GetEtDevice()->CreateVertexDeclaration( VertexElement );
	}

	ResetRenderTarget();
}

void CEtShadowMap::LoadMaterial()
{
	if( m_bUseSoftShadow && !m_hShadowMaterial && CEtResourceMng::IsActive() ) {
		m_hShadowMaterial = ::LoadResource( "Shadow.fx", RT_SHADER, true );
		int nTexIndex = m_hShadowMap->GetMyIndex();
		::AddCustomParam( m_vecCustomParam, EPT_TEX, m_hShadowMaterial, "g_ShadowMapTex", &nTexIndex );
	}
}

void CEtShadowMap::CalcShadowMat()
{	
	EtMatrix *pInvViewMat;
	SLightInfo *pLightInfo;
	EtVector3 Eye, At, CamDir;

	pLightInfo = CEtLight::GetShadowCastDirLightInfo();
	pInvViewMat = CEtCamera::GetActiveCamera()->GetInvViewMat();
	CamDir = *( EtVector3 * )&pInvViewMat->_31;
	CamDir.y = 0.0f;
	EtVec3Normalize( &CamDir, &CamDir );
	At = *( EtVector3 * )&pInvViewMat->_41 + CamDir * m_fDistanceLightAt;
	Eye = At - pLightInfo->Direction * 10000.0f;
	EtMatrixLookAtLH( &m_LightViewMat, &Eye, &At, &EtVector3( 0.0f, 1.0f, 0.0f ) );
	EtMatrixOrthoLH( &m_LightProjMat, m_fShadowRange * 2.0f, m_fShadowRange * 2.0f, 10.0f, 100000.0f );
	m_LightProjDepthMat = m_LightProjMat;
}

void CEtShadowMap::BeginShadow()
{
	m_pBackupRenderTarget = GetEtDevice()->GetRenderTarget();
	m_pBackupDepthTarget = GetEtDevice()->GetDepthStencilSurface();
	GetEtDevice()->SetRenderTarget( m_hShadowMap->GetSurfaceLevel() );
	
	if( m_ShadowType == ST_DEPTHSHADOWMAP )
	{
		//GetEtDevice()->SetCullMode( CULL_CW );
		GetEtDevice()->SetDepthStencilSurface( m_hShadowMapDepth->GetDepthBuffer() );
	}
	else if( m_ShadowType == ST_SHADOWMAP )
	{
		GetEtDevice()->EnableZ( false );
		GetEtDevice()->SetDepthStencilSurface( NULL );		
	}	
	GetEtDevice()->ClearBuffer( 0xFFFFFFFF, 1.0f, 0, true, true, false);

}

void CEtShadowMap::EndShadow()
{
	if( m_ShadowType == ST_DEPTHSHADOWMAP )
	{
		//GetEtDevice()->SetCullMode( CULL_CCW );
	}
	else if( m_ShadowType == ST_SHADOWMAP )
	{
		GetEtDevice()->EnableZ( true );
	}

	if( m_bUseSoftShadow ) {
		DownSampling();
	}

	GetEtDevice()->SetRenderTarget( m_pBackupRenderTarget );
	GetEtDevice()->SetDepthStencilSurface( m_pBackupDepthTarget );
}

void CEtShadowMap::ClearBuffer()
{
	LoadMaterial();
	if( !m_hShadowMaterial ) return;

	GetEtDevice()->EnableZ( false );
	
	int nPasses = 0;	
	m_hShadowMaterial->SetTechnique( 3 );
	m_hShadowMaterial->BeginEffect( nPasses );
	m_hShadowMaterial->BeginPass( 0 );
	m_hShadowMaterial->CommitChanges();

	float fDepth = 0.999f;
	STextureVertex Vertices[4];
	Vertices[ 0 ].Position = EtVector3( -1, 1, fDepth );
	Vertices[ 0 ].TexCoordinate = EtVector2( 0, 0 );
	Vertices[ 1 ].Position = EtVector3( 1, 1, fDepth );
	Vertices[ 1 ].TexCoordinate = EtVector2( 1, 0 );
	Vertices[ 2 ].Position = EtVector3( -1, -1, fDepth);
	Vertices[ 2 ].TexCoordinate = EtVector2( 0, 1 );
	Vertices[ 3 ].Position = EtVector3( 1, -1, fDepth );
	Vertices[ 3 ].TexCoordinate = EtVector2( 1, 1);

	GetEtDevice()->SetVertexDeclaration( m_nVertexDecl );
	GetEtDevice()->DrawPrimitiveUP( PT_TRIANGLESTRIP, 2, Vertices, sizeof( STextureVertex ) );	

	m_hShadowMaterial->EndPass();
	m_hShadowMaterial->EndEffect();
	GetEtDevice()->EnableZ( true );
}
 
void CEtShadowMap::DownSampling()
{
	LoadMaterial();
	if( !m_hShadowMaterial ) return;
	int nPasses = 0;	

	GetEtDevice()->EnableZ( false );
	GetEtDevice()->SetRenderTarget( m_hBlurShadowMap->GetSurfaceLevel() );
	GetEtDevice()->SetDepthStencilSurface( NULL );

	m_hShadowMaterial->SetTechnique( 2 );
	m_hShadowMaterial->BeginEffect( nPasses );
	m_hShadowMaterial->BeginPass( 0 );
	m_hShadowMaterial->SetCustomParamList( m_vecCustomParam );
	m_hShadowMaterial->CommitChanges();

	STextureVertex Vertices[4];
	Vertices[ 0 ].Position = EtVector3( -1, 1, 0 );
	Vertices[ 0 ].TexCoordinate = EtVector2( 0, 0 );
	Vertices[ 1 ].Position = EtVector3( 1, 1, 0 );
	Vertices[ 1 ].TexCoordinate = EtVector2( 1, 0 );
	Vertices[ 2 ].Position = EtVector3( -1, -1, 0);
	Vertices[ 2 ].TexCoordinate = EtVector2( 0, 1 );
	Vertices[ 3 ].Position = EtVector3( 1, -1, 0 );
	Vertices[ 3 ].TexCoordinate = EtVector2( 1, 1);

	GetEtDevice()->SetVertexDeclaration( m_nVertexDecl );
	GetEtDevice()->DrawPrimitiveUP( PT_TRIANGLESTRIP, 2, Vertices, sizeof( STextureVertex ) );	

	m_hShadowMaterial->EndPass();
	m_hShadowMaterial->EndEffect();
	GetEtDevice()->EnableZ( true );
}

void CEtShadowMap::ResetShadowMap()
{
	m_pBackupRenderTarget = GetEtDevice()->GetRenderTarget();
	m_pBackupDepthTarget = GetEtDevice()->GetDepthStencilSurface();
	GetEtDevice()->SetRenderTarget( m_hShadowMap->GetSurfaceLevel() );
	if( m_hShadowMapDepth ) {
		GetEtDevice()->SetDepthStencilSurface( m_hShadowMapDepth->GetDepthBuffer() );
	}
	GetEtDevice()->ClearBuffer( 0xffffffff, 1.0f, 0 );
	GetEtDevice()->SetRenderTarget( m_pBackupRenderTarget );
	GetEtDevice()->SetDepthStencilSurface( m_pBackupDepthTarget );
	EtMatrixIdentity(&m_LightViewMat);
	D3DXMatrixTranslation(&m_LightProjMat, 0.0f, 0.0f, -10000.0f );
	m_LightProjMat._11 = m_LightProjMat._22 = m_LightProjMat._33 = 0.f;
	m_LightProjDepthMat = m_LightProjMat;
}

void CEtShadowMap::DebugRender()
{
	SUICoord UVCoord, ScreenCoord;
	CEtSprite::GetInstance().Begin( 0 );
	UVCoord.SetCoord( 0.0f, 0.0f, 1.0f, 1.0f );
	ScreenCoord.SetCoord( 0.55f, 0.65f, 0.2f, 0.3f );	
	CEtSprite::GetInstance().DrawSprite( (EtTexture*)m_hBlurShadowMap->GetTexturePtr(), m_hBlurShadowMap->Width(), m_hBlurShadowMap->Height(),
					UVCoord, 0xffffffff, ScreenCoord, 0.0f );
	CEtSprite::GetInstance().End();
}

EtBaseTexture * CEtShadowMap::GetShadowMapTexture() 
{
	if( m_bUseSoftShadow ) {
		if( m_hBlurShadowMap ) {
			return m_hBlurShadowMap->GetTexturePtr(); 
		}
		else {
			return NULL;
		}
	}
	else {
		if( m_hShadowMap ) {
			return m_hShadowMap->GetTexturePtr(); 
		}
		else {
			return NULL;
		}
	}
}

void CEtShadowMap::Bake( CEtRenderStack *pStack )
{
	if( m_bEnable ) {
		BeginShadow();
		CalcShadowMat();
		pStack->RenderBlock( CEtRenderStack::PASS_SHADOW );
		EndShadow();
	}
	else {
		pStack->ClearElement( CEtRenderStack::PASS_SHADOW );
	}
}

void CEtShadowMap::ResetRenderTarget()
{
	m_pBackupRenderTarget = GetEtDevice()->GetRenderTarget();
	m_pBackupDepthTarget = GetEtDevice()->GetDepthStencilSurface();
	GetEtDevice()->SetRenderTarget( m_hShadowMap->GetSurfaceLevel() );
	if( m_hShadowMapDepth )
		GetEtDevice()->SetDepthStencilSurface( m_hShadowMapDepth->GetDepthBuffer() );
	GetEtDevice()->ClearBuffer( 0xFFFFFFFF, 1.0f, 0, true, true, false);
	GetEtDevice()->SetRenderTarget( m_pBackupRenderTarget );
	GetEtDevice()->SetDepthStencilSurface( m_pBackupDepthTarget );
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////
CEtShadowMap *GetEtShadowMap()
{ 
	return g_pEtShadowMap; 
}

void DeleteShadowMap() 
{ 
	SAFE_DELETE( g_pEtShadowMap ); 
}

void CreateSimpleSahdowMap( ShadowQuality Quality, ShadowType Type ) 
{
	SAFE_DELETE( g_pEtShadowMap ); 
	g_pEtShadowMap = new CEtTrapezoidShadowMap;
	g_pEtShadowMap->Initialize( Quality, Type );
}
