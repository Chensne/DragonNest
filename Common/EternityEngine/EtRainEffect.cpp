#include "StdAfx.h"
#include "EtRainEffect.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CEtRainEffect::CEtRainEffect(void)
{
	m_fRainSpeed = 1.0f;
	m_fRaniStretchValue = 1.0f;
	m_vCurUp = EtVector3( 0.0f, 1.0f, 0.0f );
	m_vVertexOffset = EtVector4( 0.0f, 0.0f, 0.0f, 0.0f );
	m_vRainDir = EtVector3( 0.0f, -1.0f, 0.0f );
	m_vMovement = EtVector3( 0.0f, 0.0f, 0.0f );
	m_vPrevMoveDir = EtVector3( 0.0f, 0.0f, 0.0f );
	EtMatrixIdentity( &m_RotationMat );

	m_nLayerCount = 1;
	m_fRainSpeedDecreaseRate = 0.9f;

	// 외부 파라메터로 빼놓지 않고 상수 형태로 쓴다..
	// 필요하면 외부 파라메터로 빼도록 하자.
	m_fMaxRainAngle = 30.0f;
	m_fInterpolationSpeed = 300.0f;
	m_fRainOffset = 0.0f;
}

CEtRainEffect::~CEtRainEffect(void)
{
}

void CEtRainEffect::SetRainDir( EtVector3 &vDir )
{
	m_vRainDir = vDir;

	EtVector3 vCross, vInverseDir, vWorldUp;

	vInverseDir = -vDir;
	vWorldUp = EtVector3( 0.0f, 1.0f, 0.0f );
	EtVec3Cross( &vCross, &vWorldUp, &vInverseDir );
	EtMatrixRotationAxis( &m_RotationMat, &vCross, EtAcos( EtVec3Dot( &vWorldUp, &vInverseDir ) ) );
}

void CEtRainEffect::Initialize( const char *pRainTexture )
{
	int i, nIndexCount, nVertexCount;
	EtVector3 *pvPosition;
	EtVector2 *pTexCoord;
	WORD *pwIndex;

	nVertexCount = ( RAIN_CYLINDER_SIDE_COUNT + 1 ) * 2;
	nIndexCount = RAIN_CYLINDER_SIDE_COUNT * 2 + 2;

	pvPosition = new EtVector3[ nVertexCount ];
	pTexCoord = new EtVector2[ nVertexCount ];
	pwIndex = new WORD[ nIndexCount ];
	for( i = 0; i < nVertexCount; i += 2 )
	{
		float fRadian, fWeight;

		fWeight = ( 1.0f - ( i / ( nVertexCount - 2.0f ) ) );
		fRadian = ET_PI * 2.0f * fWeight;
		pvPosition[ i ] = EtVector3( sin( fRadian ) * RAIN_CYLINDER_RADIUS, RAIN_CYLINDER_HEIGHT, cos( fRadian ) * RAIN_CYLINDER_RADIUS );
		pvPosition[ i + 1 ] = EtVector3( sin( fRadian ) * RAIN_CYLINDER_RADIUS, -RAIN_CYLINDER_HEIGHT, cos( fRadian ) * RAIN_CYLINDER_RADIUS );

		pTexCoord[ i ] = EtVector2( fWeight * 4.0f, 0.0f );
		pTexCoord[ i + 1 ] = EtVector2( fWeight * 4.0f, 10.0f );
	}
	for( i = 0; i < nIndexCount; i++ )
	{
		pwIndex[ i ] = i;
	}

	CMemoryStream Stream;

	Stream.Initialize( pvPosition, nVertexCount * sizeof( EtVector3 ) );
	m_MeshStream.LoadVertexStream( &Stream, MST_POSITION, 0, nVertexCount );
	Stream.Initialize( pTexCoord, nVertexCount * sizeof( EtVector2 ) );
	m_MeshStream.LoadVertexStream( &Stream, MST_TEXCOORD, 0, nVertexCount );
	Stream.Initialize( pwIndex, nIndexCount * sizeof( WORD ) );
	m_MeshStream.LoadIndexStream( &Stream, true, nIndexCount );

	delete [] pvPosition;
	delete [] pTexCoord;
	delete [] pwIndex;

	int nTexIndex;

	m_hMaterial = LoadResource( "Rainfall.fx", RT_SHADER );	
	m_hTexture = LoadResource( pRainTexture, RT_TEXTURE );
	if( m_hTexture )
	{
		nTexIndex = m_hTexture->GetMyIndex();
		AddCustomParam( m_vecCustomParam, EPT_TEX, m_hMaterial, "g_RainfallTex", &nTexIndex );
	}
	AddCustomParam( m_vecCustomParam, EPT_FLOAT_PTR, m_hMaterial, "g_fRainSpeed", &m_fRainSpeed );
	AddCustomParam( m_vecCustomParam, EPT_FLOAT_PTR, m_hMaterial, "g_fRainStretchValue", &m_fRaniStretchValue );
	AddCustomParam( m_vecCustomParam, EPT_FLOAT_PTR, m_hMaterial, "g_fRainOffset", &m_fRainOffset );
	AddCustomParam( m_vecCustomParam, EPT_VECTOR_PTR, m_hMaterial, "g_RainVertexOffset", &m_vVertexOffset );
}

void CEtRainEffect::SetMovement( EtVector3 &vMovement )
{
	EtVector3 vMoveDir;
	float fMoveDist;

	m_vMovement = vMovement;
	fMoveDist = EtVec3Length( &m_vMovement );
	if( fMoveDist > 0.0f )
	{
		vMoveDir = m_vMovement / fMoveDist;
		if( EtVec3Dot( &vMoveDir, &m_vPrevMoveDir ) > 0.965925f )	// 15도 이하로 차이나면 같은 방향으로 움직이는 걸로 간주해서 예전 방향대로 움직인다.
		{
			m_vMovement = m_vPrevMoveDir * fMoveDist;
		}
		else
		{
			m_vPrevMoveDir = vMoveDir;
		}
	}
}

void CEtRainEffect::CalcRotationMat( float fElapsedTime )
{
	EtVector3 vTargetUp, vWorldUp;
	float fCurDegree, fTargetDegree, fDiffDegree;

	vTargetUp = m_vMovement / fElapsedTime;
	vTargetUp.y = 980.0f * m_fRainSpeed;
	EtVec3Normalize( &vTargetUp, &vTargetUp );

	vWorldUp = EtVector3( 0.0f, 1.0f, 0.0f );
	fCurDegree = EtToDegree( EtAcos( EtVec3Dot( &vWorldUp, &m_vCurUp ) ) );
	fTargetDegree = EtToDegree( EtAcos( EtVec3Dot( &vWorldUp, &vTargetUp ) ) );
	if( fTargetDegree > m_fMaxRainAngle )
	{
		EtVector3 vUp;
		EtMatrix AxisMat;

		EtVec3Cross( &vUp, &vWorldUp, &vTargetUp );
		EtMatrixRotationAxis( &AxisMat, &vUp, EtToRadian( m_fMaxRainAngle ) );
		EtVec3TransformNormal( &vTargetUp, &vWorldUp, &AxisMat );
		fTargetDegree = m_fMaxRainAngle;
	}

	fDiffDegree = fabs( fCurDegree - fTargetDegree ) / fElapsedTime;

	if( fDiffDegree > m_fInterpolationSpeed )
	{
		EtVec3Lerp( &vTargetUp, &m_vCurUp, &vTargetUp, m_fInterpolationSpeed / fDiffDegree );
		EtVec3Normalize( &vTargetUp, &vTargetUp );
	}

	m_vCurUp = vTargetUp;
}

void CEtRainEffect::RenderRainCylinder( EtVector3 &vPosition, EtVector3 &vScale )
{
	EtMatrix WorldMat, ScaleMat;

	EtMatrixTranslation( &WorldMat, vPosition.x, vPosition.y, vPosition.z );
	EtMatrixScaling( &ScaleMat, vScale.x, vScale.y, vScale.z );
	EtMatrixMultiply( &WorldMat, &ScaleMat, &WorldMat );

	SRenderStackElement RenderElement;

	RenderElement.hMaterial = m_hMaterial;
	RenderElement.nTechniqueIndex = 0;
	RenderElement.WorldMat = WorldMat;
	RenderElement.nSaveMatIndex = -1;
	RenderElement.pvecCustomParam = &m_vecCustomParam;
	RenderElement.pRenderMeshStream = &m_MeshStream;
	RenderElement.nBakeDepthIndex = DT_NONE;
	GetCurRenderStack()->AddAlphaRenderElement( RenderElement );
}

void CEtRainEffect::Render( float fElapsedTime )
{
	int i;
	EtCameraHandle hCamera;
	EtVector3 *pvCamPosition, vFinalUp;
	float fRainSpeed;

	hCamera = CEtCamera::GetActiveCamera();
	pvCamPosition = hCamera->GetPosition();

	CalcRotationMat( fElapsedTime );

	fRainSpeed = m_fRainSpeed;
	EtVec3TransformNormal( &vFinalUp, &m_vCurUp, &m_RotationMat );
	m_vVertexOffset.x = vFinalUp.x * 500.0f;
	m_vVertexOffset.y = vFinalUp.y * 500.0f;
	m_vVertexOffset.z = vFinalUp.z * 500.0f;
	m_vVertexOffset.y = 0.0f;

	float fRainSpeedTable[ MAX_RAIN_CYLINDER_COUNT ];
	float fCylinderScale[ MAX_RAIN_CYLINDER_COUNT ] = { 1.0f, 4.0f, 6.0f, 8.0f };

	fRainSpeedTable[ 0 ] = m_fRainSpeed;
	for( i = 1; i < MAX_RAIN_CYLINDER_COUNT; i++ )
	{
		fRainSpeedTable[ i ] = fRainSpeedTable[ i - 1 ] * m_fRainSpeedDecreaseRate;
	}
	for( i = m_nLayerCount - 1; i >= 0; i-- )
	{
		m_fRainOffset = i * 0.25f;
		m_fRainSpeed = fRainSpeedTable[ i ];
		if( i == 0 )
		{
			GetEtDevice()->EnableZ( false );
		}
		RenderRainCylinder( *pvCamPosition, EtVector3( fCylinderScale[ i ], fCylinderScale[ i ], fCylinderScale[ i ] ) );
		GetEtDevice()->EnableZ( true );
	}

	m_fRainSpeed = fRainSpeed;
}

EtRainHandle CEtRainEffect::CreateRainEffect( const char *pRainTexture )
{
	CEtRainEffect *pRainEffect;

	pRainEffect = new CEtRainEffect();
	pRainEffect->Initialize( pRainTexture );

	return pRainEffect->GetMySmartPtr();
}
