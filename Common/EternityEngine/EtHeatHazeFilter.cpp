#include "StdAfx.h"
#include "EtHeatHazeFilter.h"
#include "EtBackBufferMng.h"
#include "EtDrawQuad.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CEtHeatHazeFilter::CEtHeatHazeFilter()
{
	m_Type = SF_HEATHAZE;
	m_fElapsedTime = 0.0f;
	m_fDistortionPower = 1.0f;
	m_fWaveSpeed = 5.0f;
}

CEtHeatHazeFilter::~CEtHeatHazeFilter()
{
	Clear();
}

void CEtHeatHazeFilter::Clear()
{
	SAFE_RELEASE_SPTR( m_hTexture );
	CEtPostProcessFilter::Clear();
}

void CEtHeatHazeFilter::Initialize()
{
	Clear();

	m_hMaterial = LoadResource( "HeatHazeFilter.fx", RT_SHADER );
	if( !m_hMaterial ) return;

	int nBackBufferIndex = GetEtBackBufferMng()->GetBackBufferIndex();
	AddCustomParam( m_vecCustomParam, EPT_TEX, m_hMaterial, "g_BackBuffer", &nBackBufferIndex );
	AddCustomParam( m_vecCustomParam, EPT_FLOAT_PTR, m_hMaterial, "g_fElapsedTime", &m_fElapsedTime );
	AddCustomParam( m_vecCustomParam, EPT_FLOAT_PTR, m_hMaterial, "g_fDistortionPower", &m_fDistortionPower );	
}

void CEtHeatHazeFilter::SetTexture( char *szFileName )
{
	if( !m_hMaterial ) return;
	SAFE_RELEASE_SPTR( m_hTexture );
	m_hTexture = EternityEngine::LoadTexture( szFileName );
	if( !m_hTexture ) return;
	int nTexIndex = m_hTexture->GetMyIndex();
	AddCustomParam( m_vecCustomParam, EPT_TEX, m_hMaterial, "g_PerturbationTexture", &nTexIndex );
}

void CEtHeatHazeFilter::Render( float fElapsedTime )
{
	if( !m_hMaterial ) return;
	m_fElapsedTime += fElapsedTime * m_fWaveSpeed;
	GetEtDevice()->RestoreRenderTarget();

	int nPasses = 0;
	int nTechnique = 0;
	m_hMaterial->SetTechnique( nTechnique );
	m_hMaterial->SetCustomParamList( m_vecCustomParam );
	m_hMaterial->BeginEffect( nPasses );
	m_hMaterial->BeginPass( 0 );

	EtCameraHandle hCamera;
	EtMatrix *pProjMat;
	EtVector3 vOutput;

	hCamera = CEtCamera::GetActiveCamera();
	pProjMat = hCamera->GetProjMat();
	EtVec3TransformCoord( &vOutput, &EtVector3( 0.0, 0.0f, 1000.0f ), pProjMat );
	GetEtDevice()->SetVertexShader( NULL );	// Use Fixed Pipeline
	DrawQuad( &EtVector2( 0, 0 ), &EtVector2( 1, 1 ), &EtVector2( 0, 0 ), &EtVector2( 1, 1 ), vOutput.z );
	m_hMaterial->EndPass();
	m_hMaterial->EndEffect();
}
