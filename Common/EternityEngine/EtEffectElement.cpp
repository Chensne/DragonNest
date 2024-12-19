#include "StdAfx.h"
#include "EtEffectElement.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CEtEffectElement::CEtEffectElement()
{
	m_pElementData = NULL;
	m_bPrevShow = false;
	m_bShow = false;
	m_fAlpha = 1.f;
	EtMatrixIdentity( &m_WorldMat );
	EtMatrixIdentity( &m_PrevWorldMat );
	m_nEffectTick = 0;
}

CEtEffectElement::~CEtEffectElement()
{
}

void CEtEffectElement::Initialize( CEtElementData *pElementData )
{
	m_pElementData = pElementData;
}

void CEtEffectElement::Process( EtMatrix &WorldMat, int nEffectTick, float fScaleVal )
{
	m_bPrevShow = m_bShow;
	m_bShow = m_pElementData->Process( nEffectTick, WorldMat, fScaleVal );
	if( !m_bShow )
	{
		return;
	}

	m_PrevWorldMat = m_WorldMat;
	m_WorldMat = *m_pElementData->GetWorldMat();
	m_Color = *m_pElementData->GetColor();
	m_Color.a *= m_fAlpha;
	m_nEffectTick = nEffectTick;
}

///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////


CEtEffectMeshElement::CEtEffectMeshElement()
{
	m_nAmbientIndex = -1;;
	m_nDiffuseIndex = -1;
	m_pSkinInstance = NULL;
	m_nFXTime = 0;
}

CEtEffectMeshElement::~CEtEffectMeshElement()
{
	SAFE_DELETE( m_pSkinInstance );
	SAFE_RELEASE_SPTR( m_hSkin );
}

void CEtEffectMeshElement::Initialize( CEtElementData *pElementData )
{
	CEtEffectElement::Initialize( pElementData );

	m_hSkin = LoadResource( pElementData->GetFileName(), RT_SKIN );
	if( !m_hSkin ) {
		return;
	}
	if( m_pSkinInstance == NULL )
	{
		m_pSkinInstance = new CEtSkinInstance();
	}
	m_pSkinInstance->CopySkinInfo( m_hSkin );
	m_pSkinInstance->SetBlendOP( ( EtBlendOP )( ( CEtMeshElementData * )m_pElementData )->GetBlendOP() );
	m_pSkinInstance->SetSrcBlend( ( EtBlendMode )( ( CEtMeshElementData * )m_pElementData )->GetSrcBlend() );
	m_pSkinInstance->SetDestBlend( ( EtBlendMode )( ( CEtMeshElementData * )m_pElementData )->GetDestBlend() );
	m_pSkinInstance->SetCullMode( ( ( CEtMeshElementData * )m_pElementData )->GetCullMode() ? true : false );
	m_pSkinInstance->SetUVTiling( ( ( CEtMeshElementData * )m_pElementData )->GetTilingMode() ? true : false );
	
	m_nAmbientIndex = m_pSkinInstance->AddCustomParam( "g_MaterialAmbient" );
	m_nDiffuseIndex = m_pSkinInstance->AddCustomParam( "g_MaterialDiffuse" );
	m_nFXTime = m_pSkinInstance->AddCustomParam("g_fFXElapsedTime");

	m_pSkinInstance->SetSkipBakeDepth( (( CEtMeshElementData * )m_pElementData )->GetZWriteMode() ? false : true );
	m_pSkinInstance->SetMeshRadius( 0.0f );
}

void CEtEffectMeshElement::Render()
{
	if( !m_bShow ) {
		return;
	}
	if( !m_pSkinInstance ) {
		return;
	}
	if( m_nAmbientIndex != -1 ) {
		m_pSkinInstance->SetCustomParam( m_nAmbientIndex, &m_Color, -1 );
	}
	if( m_nDiffuseIndex != -1 ) {
		m_pSkinInstance->SetCustomParam( m_nDiffuseIndex, &m_Color, -1 );
	}
	if( m_nFXTime != -1 ) {
		float fTime = m_nEffectTick / 60.f;
		m_pSkinInstance->SetCustomParam( m_nFXTime, &fTime, -1 );
	}
	DNVector(CEtLight *) vecInfluenceLight;
	m_pSkinInstance->Render( m_WorldMat, m_WorldMat, m_Color.a, vecInfluenceLight, false, -1 );
}

void CEtEffectMeshElement::RenderImmediate()
{
	if( !m_bShow ) {
		return;
	}
	if( !m_pSkinInstance ) {
		return;
	}
	if( m_nAmbientIndex != -1 ) {
		m_pSkinInstance->SetCustomParam( m_nAmbientIndex, &m_Color, -1 );
	}
	if( m_nDiffuseIndex != -1 ) {
		m_pSkinInstance->SetCustomParam( m_nDiffuseIndex, &m_Color, -1 );
	}
	if( m_nFXTime != -1 ) {
		float fTime = m_nEffectTick / 60.f;
		m_pSkinInstance->SetCustomParam( m_nFXTime, &fTime, -1 );
	}
	DNVector(CEtLight *) vecInfluenceLight;
	m_pSkinInstance->RenderImmediate( m_WorldMat, m_WorldMat, m_Color.a, vecInfluenceLight, false, false, -1 );

	bool bAlphaTwoPass = m_pSkinInstance->IsAlphaTwoPass();
	m_pSkinInstance->SetRenderAlphaTwoPass( true );
	m_pSkinInstance->RenderImmediate( m_WorldMat, m_WorldMat, m_Color.a, vecInfluenceLight, false, true, -1 );
	m_pSkinInstance->SetRenderAlphaTwoPass( bAlphaTwoPass );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CEtEffectBillboardEffectElement::CEtEffectBillboardEffectElement()
{
	m_bNeedSetRenderType = false;
	m_RenderType = (RenderType)0;
	m_bNeedSetCull = false;
	m_bEnableCull = false;
}

CEtEffectBillboardEffectElement::~CEtEffectBillboardEffectElement()
{
	SAFE_RELEASE_SPTR( m_hBillboardEffectData );
	SAFE_RELEASE_SPTR( m_hBillboardEffect );
}

void CEtEffectBillboardEffectElement::Initialize( CEtElementData *pElementData )
{
	CEtEffectElement::Initialize( pElementData );

	m_hBillboardEffectData = LoadResource( pElementData->GetFileName(), RT_PARTICLE );
}

void CEtEffectBillboardEffectElement::Process( EtMatrix &WorldMat, int nEffectTick, float fScaleVal )
{
	CEtEffectElement::Process( WorldMat, nEffectTick, 1.f );

	if( !m_hBillboardEffectData ) {
		return;
	}

	if( !IsShow() ) {
		if( !m_hBillboardEffect ) return;
		if( ( ( CEtBillboardEffectElementData * )m_pElementData )->IsStop() ) {
			m_hBillboardEffect->Show( false );
		}
		else {
			if( m_hBillboardEffect ) {
				m_hBillboardEffect->StopPlay();
			}
		}
		return;
	}

	if( !m_hBillboardEffect ) {
		m_hBillboardEffect = (new CEtBillboardEffect())->GetMySmartPtr();
		m_hBillboardEffect->Initialize( m_hBillboardEffectData, NULL );
		m_hBillboardEffect->EnableLoop( ( ( CEtBillboardEffectElementData * )m_pElementData )->IsLoop() ? true : false );
		m_hBillboardEffect->EnableTracePos( ( ( CEtBillboardEffectElementData * )m_pElementData )->IsIterate() ? true : false );
		if( m_bNeedSetRenderType ) {
			m_hBillboardEffect->SetRenderType( m_RenderType );
			m_bNeedSetRenderType = false;
		}
		if( m_bNeedSetCull ) {
			m_hBillboardEffect->EnableCull( m_bEnableCull );
			m_bNeedSetCull = false;
		}
	}
	if( m_hBillboardEffect ) {
		m_hBillboardEffect->Show( true );
		m_hBillboardEffect->SetWorldMat( &m_WorldMat );
		m_hBillboardEffect->SetColor( &m_Color );
		m_hBillboardEffect->SetScale( fScaleVal );
	}
}

void CEtEffectBillboardEffectElement::SetRenderType( RenderType Type )
{
	if( m_hBillboardEffect ) {
		m_hBillboardEffect->SetRenderType( Type );
	}
	else {
		m_bNeedSetRenderType = true;
		m_RenderType = Type;
	}
}

void CEtEffectBillboardEffectElement::RenderImmediate()
{
	if( m_hBillboardEffect )
	{
		m_hBillboardEffect->RenderImmediate();
	}
}

void CEtEffectBillboardEffectElement::EnableCull( bool bEnable )
{
	if( m_hBillboardEffect ) {
		m_hBillboardEffect->EnableCull( bEnable );
	}
	else {
		m_bNeedSetCull = true;
		m_bEnableCull = bEnable;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

CEtEffectPointLightElement::CEtEffectPointLightElement()
{
	m_LightInfo.Type = LT_POINT;
}

CEtEffectPointLightElement::~CEtEffectPointLightElement()
{
	SAFE_RELEASE_SPTR( m_hLight );
}

void CEtEffectPointLightElement::Initialize( CEtElementData *pElementData )
{
	CEtEffectElement::Initialize( pElementData );

	CEtLight *pLight;

	pLight = new CEtLight();
	pLight->Initialize( &m_LightInfo );
	m_hLight = pLight->GetMySmartPtr();
	m_hLight->Enable( false );
}

void CEtEffectPointLightElement::Process( EtMatrix &WorldMat, int nEffectTick, float fScaleVal )
{
	CEtEffectElement::Process( WorldMat, nEffectTick, fScaleVal );

	if( !IsShow() ) {
		m_hLight->Enable( false );
		return;
	}

	memcpy( &m_LightInfo.Position, &m_WorldMat._41, sizeof( EtVector3 ) );
	m_LightInfo.fRange = ( ( CEtPointLightElementData * )m_pElementData )->GetRange();
	m_LightInfo.Diffuse = m_Color;
	m_LightInfo.Specular = m_Color;
	m_hLight->SetLightInfo( &m_LightInfo );
	m_hLight->Enable( true );
}
