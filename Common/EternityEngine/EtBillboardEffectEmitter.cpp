#include "StdAfx.h"
#include "EtBillboardEffectEmitter.h"
#include "EngineUtil.h"
#include "EtBackBufferMng.h"
#include "EtMRTMng.h"

CEtBillboardEffectEmitter::CEtBillboardEffectEmitter()
{
	InitializeEmitter();
	m_bDraw = true;
}

CEtBillboardEffectEmitter::~CEtBillboardEffectEmitter()
{
	FinalizeEmitter();
	Clear();
}

void CEtBillboardEffectEmitter::InitializeEmitter()
{
	memset( &m_EmitterInfo, 0, sizeof( SEmitter ) );
	for( int i = 0; i < DATA_TYPE_AMOUNT; i++ )
		m_pDataContainer[ i ] = new CEtEffectDataContainer();
	memset( m_ScaleData, 0, sizeof( m_ScaleData ) );
	m_nMaxEffectLifeTime = 0;
}

void CEtBillboardEffectEmitter::FinalizeEmitter()
{
	for( int i = 0; i < DATA_TYPE_AMOUNT; i++ )
	{
		SAFE_DELETE( m_pDataContainer[ i ]);
	}
	SAFE_RELEASE_SPTR( m_hTexture );
}

void CEtBillboardEffectEmitter::Clear()
{
	m_EmitterMeshStream.Clear();
}

void CEtBillboardEffectEmitter::LoadEmitter( CStream *pStream, int nVersion )
{
	int i;

	switch( nVersion )
	{
	case EMITTER_VER:
		pStream->Read( &m_EmitterInfo, sizeof( SEmitter ) );
		pStream->Seek( EMITTER_STRUCT_RESERVED, SEEK_CUR );
		break;
	case EMITTER_VER_10:	LoadVersion_10( pStream );	m_EmitterInfo.nOptionCountMid = m_EmitterInfo.nOptionCountLow = m_EmitterInfo.nGenerateCount; break;
	case EMITTER_VER_11:	LoadVersion_11( pStream );	m_EmitterInfo.nOptionCountMid = m_EmitterInfo.nOptionCountLow = m_EmitterInfo.nGenerateCount; break;
#ifdef  _ADD_EFFECT_EMITTER_0xD
	case EMITTER_VER_13:	LoadVersion_13( pStream );	m_EmitterInfo.nOptionCountMid = m_EmitterInfo.nOptionCountLow = m_EmitterInfo.nGenerateCount; break;
#endif
	default:
		ASSERT( 0 && "Invalid EmitterVersion" );
		pStream->Seek( 1024, SEEK_CUR );
		break;
	}

	for( i = 0; i < DATA_TYPE_AMOUNT; i++ ) 
	{
		m_pDataContainer[ i ]->ResetData();
		m_pDataContainer[ i ]->Load( pStream );
	}

	if( strlen( m_EmitterInfo.szTextureName ) > 0 )
	{
		// 세이브할때 잘못되서 풀네임이 들어간 경우가 있다.. 우선 이렇게 처리하고 원인을 찾아보자
		// by mapping
		char szFullName[ _MAX_PATH ];
		_GetFullFileName( szFullName, _countof(szFullName), m_EmitterInfo.szTextureName );
		LoadTexture( szFullName );
	}
}

void CEtBillboardEffectEmitter::SaveEmitter( CStream *pStream )
{
	char cDummy[ EMITTER_STRUCT_RESERVED ];

	memset( cDummy, 0, EMITTER_STRUCT_RESERVED );
	pStream->Write( &m_EmitterInfo, sizeof( SEmitter ) );
	pStream->Write( cDummy, EMITTER_STRUCT_RESERVED );

	for( int i = 0; i < DATA_TYPE_AMOUNT; i++ )
	{
		m_pDataContainer[ i ]->Save( pStream );
	}
}

void CEtBillboardEffectEmitter::LoadVersion_10( CStream *pStream )
{
	// 구조체에서 뒷부분으로 멤버가 추가되기만 하면 이렇게 하면 Read로 한번에 읽을 수 있다.
	// 하지만, 사이에 있는 멤버가 사라질 수도 있으니 바뀌게 되면 직접 하나하나 읽는거로 바꿔야한다.
	// 아직까진 그런일이 없을 듯 하니.. 만약 그렇게 되면 UI쪽처럼 멤버 하나하나 읽는 루틴 바꾸기로 한다.
	pStream->Read( &m_EmitterInfo, sizeof( SEmitter_10 ) );
	pStream->Seek( EMITTER_STRUCT_RESERVED_10, SEEK_CUR );
}

void CEtBillboardEffectEmitter::LoadVersion_11( CStream *pStream )
{
	pStream->Read( &m_EmitterInfo, sizeof( SEmitter_11 ) );
	pStream->Seek( EMITTER_STRUCT_RESERVED_11, SEEK_CUR );
}

void CEtBillboardEffectEmitter::LoadVersion_13( CStream *pStream )
{
    //TODO(Cussrro):속潼栗都댄轎，써뭐댕鬼꼇寧鈴
	pStream->Read( &m_EmitterInfo, sizeof( SEmitter_13 ) );
	pStream->Seek( EMITTER_STRUCT_RESERVED_13, SEEK_CUR );
}

void CEtBillboardEffectEmitter::LoadTexture( const char *pFileName )
{
	SAFE_RELEASE_SPTR( m_hTexture );

	m_hTexture = LoadResource( pFileName, RT_TEXTURE );
	if( !m_hTexture ) {
		ASSERT( 0 && "Texture Not Found" );
		return;
	}
	//strcpy( m_EmitterInfo.szTextureName, m_hTexture->GetFileName() );
	strncpy_s(m_EmitterInfo.szTextureName, _countof(m_EmitterInfo.szTextureName), m_hTexture->GetFileName(), _TRUNCATE);
}

void CEtBillboardEffectEmitter::CreateEmitterBuffer()
{
	int i, j;
	EtVector3 RotationAxis;
	EtMatrix RotationMat;
	float fRotateAngle;

	Clear();
	CreateDataContainer();

	if( m_EmitterInfo.bUseTimeRandomSeed ) srand( GetTickCount() );
	else srand( m_EmitterInfo.nRandomSeed );

	int nBillboardEffectCount = m_EmitterInfo.nGenerateCount;
	if( CEtBillboardEffect::GetEffectCountOption() == 1 ) nBillboardEffectCount = m_EmitterInfo.nOptionCountMid;
	else if( CEtBillboardEffect::GetEffectCountOption() == 2 ) nBillboardEffectCount = m_EmitterInfo.nOptionCountLow;

	m_vecBeginningTime.resize( nBillboardEffectCount );
	if( m_vecBeginningTime.empty() ) {
		EnableDraw( false );
		return;
	}

	EtVec3Cross( &RotationAxis, &EtVector3( 0.0f, 1.0f, 0.0f ), 
		&EtVector3( m_EmitterInfo.vInitiateLaunch.x, m_EmitterInfo.vInitiateLaunch.y, m_EmitterInfo.vInitiateLaunch.z ) );
	fRotateAngle = EtAcos( EtVec3Dot( &EtVector3( 0.0f, 1.0f, 0.0f ), 
		&EtVector3( m_EmitterInfo.vInitiateLaunch.x, m_EmitterInfo.vInitiateLaunch.y, m_EmitterInfo.vInitiateLaunch.z ) ) );

	float fffRotateAngle = acosf( EtVec3Dot( &EtVector3( 0.0f, 1.0f, 0.0f ), 
		&EtVector3( m_EmitterInfo.vInitiateLaunch.x, m_EmitterInfo.vInitiateLaunch.y, m_EmitterInfo.vInitiateLaunch.z ) ) );
	ASSERT( fffRotateAngle == fRotateAngle );
	EtMatrixRotationAxis( &RotationMat, &RotationAxis, fRotateAngle );

	EtVector4 *pDirection;
	EtVector2 *pBeginningTime;
	short *pVertexIndex;
	EtVector3 *pStartOffset;
	float *pSizeAdjustRate;

	pDirection = new EtVector4[ nBillboardEffectCount * 4 ];
	pBeginningTime = new EtVector2[ nBillboardEffectCount * 4 ];
	pVertexIndex = new short[ nBillboardEffectCount * 4 * 4 ];
	pStartOffset = new EtVector3[ nBillboardEffectCount * 4 ];
	pSizeAdjustRate = new float[ nBillboardEffectCount * 4 ];

	EtVector3 vReservedLaunchVec1( 1.0f, 1.0f, 1.0f );
	EtVector3 vReservedLaunchVec2( -1.0f, -1.0f, -1.0f );
	EtVec3Normalize( &vReservedLaunchVec1, &vReservedLaunchVec1 );
	EtVec3Normalize( &vReservedLaunchVec2, &vReservedLaunchVec2 );

	for( i = 0; i < nBillboardEffectCount; i++ )
	{
		EtVector3 vStartOffset;
		EtVector3 vNormal;
		vNormal.x = RandomNumberInRange( -1.0f, 1.0f );
		vNormal.y = RandomNumberInRange( -1.0f, 1.0f );
		vNormal.z = RandomNumberInRange( -1.0f, 1.0f );
		EtVec3Normalize( &vNormal, &vNormal );
		EtVector3 vBaseSize, vBaseNormal;
		vBaseSize.x = RandomNumberInRange( m_EmitterInfo.vStartPosInnerRadius.x, m_EmitterInfo.vStartPosOuterRadius.x );
		vBaseSize.y = RandomNumberInRange( m_EmitterInfo.vStartPosInnerRadius.y, m_EmitterInfo.vStartPosOuterRadius.y );
		vBaseSize.z = RandomNumberInRange( m_EmitterInfo.vStartPosInnerRadius.z, m_EmitterInfo.vStartPosOuterRadius.z );
		EtVec3Normalize( &vBaseNormal, &vBaseSize );
		vBaseNormal.x = vBaseNormal.x * vNormal.x;
		vBaseNormal.y = vBaseNormal.y * vNormal.y;
		vBaseNormal.z = vBaseNormal.z * vNormal.z;
		EtVec3Normalize( &vBaseNormal, &vBaseNormal );
		vStartOffset.x = vBaseNormal.x * vBaseSize.x;
		vStartOffset.y = vBaseNormal.y * vBaseSize.y;
		vStartOffset.z = vBaseNormal.z * vBaseSize.z;

		EtVector4 LaunchVec;
		int nRandRot;
		EtVector3 vLaunchTest = m_EmitterInfo.vInitiateLaunch;
		if( vLaunchTest == vReservedLaunchVec1 )
		{
			LaunchVec.x = vBaseNormal.x;
			LaunchVec.y = vBaseNormal.y;
			LaunchVec.z = vBaseNormal.z;
		}
		else if( vLaunchTest == vReservedLaunchVec2 )
		{
			LaunchVec.x = -vBaseNormal.x;
			LaunchVec.y = -vBaseNormal.y;
			LaunchVec.z = -vBaseNormal.z;
		}
		else
		{
			float fRange = m_EmitterInfo.vInitiateLaunch.w;
			if( m_EmitterInfo.vInitiateLaunch.w > ET_PI/2.0f && ( fRange = RandomNumberInRange( 0, m_EmitterInfo.vInitiateLaunch.w ) ) > ET_PI/2.0f )
			{
				EtVector3 vBase( 0.0f, 1.0f, 0.0f );
				EtVector3 vTempRotationAxis;
				EtMatrix matTempRotation;
				vTempRotationAxis.x = RandomNumberInRange( -sin( ET_PI/2.0f ), sin( ET_PI/2.0f ) );
				vTempRotationAxis.y = 0.0f;
				vTempRotationAxis.z = RandomNumberInRange( -sin( ET_PI/2.0f ), sin( ET_PI/2.0f ) );
				EtVec3Normalize( &vTempRotationAxis, &vTempRotationAxis );
				EtMatrixRotationAxis( &matTempRotation, &vTempRotationAxis, fRange );
				EtVec3TransformNormal( ( EtVector3 * )&LaunchVec, &vBase, &matTempRotation );
			}
			else
			{
				LaunchVec.x = RandomNumberInRange( -sin( fRange ), sin( fRange ) );
				LaunchVec.y = RandomNumberInRange( -sin( fRange ), sin( fRange ) ) + 1;
				LaunchVec.z = RandomNumberInRange( -sin( fRange ), sin( fRange ) );
			}
			EtVec3TransformNormal( ( EtVector3 * )&LaunchVec, ( EtVector3 * )&LaunchVec, &RotationMat );
			EtVec3Normalize( ( EtVector3 * )&LaunchVec, ( EtVector3 * )&LaunchVec );
		}

		LaunchVec.w = RandomNumberInRange( m_EmitterInfo.fSpeedVelocityMin, m_EmitterInfo.fSpeedVelocityMax );

		m_vecBeginningTime[ i ] = ( m_EmitterInfo.nBeginningTime / ( float )m_nMaxEffectLifeTime ) / nBillboardEffectCount * i;

		float fSizeAdjustRate = RandomNumberInRange( (float)-m_EmitterInfo.nSizeAdjustRate, (float)m_EmitterInfo.nSizeAdjustRate ) / 100.0f;

		nRandRot = rand() % 2;
		for( j = 0; j < 4; j++ )
		{
			pDirection[ i * 4 + j ] = LaunchVec;
			pBeginningTime[ i * 4 + j ].x = m_vecBeginningTime[ i ];
			pVertexIndex[ ( i * 4 + j ) * 4 ] = j;
			pVertexIndex[ ( i * 4 + j ) * 4 + 1 ] = ( short )( m_vecBeginningTime[ i ] * TRACE_POS_COUNT );	// Iterate Position Index 셋팅
			if( ( m_EmitterInfo.bUseRandomSpin ) || ( nRandRot ) )
			{
				pBeginningTime[ i * 4 + j ].y = 1.0f;
			}
			else
			{
				pBeginningTime[ i * 4 + j ].y = -1.0f;
			}
			pStartOffset[ i * 4 + j ] = vStartOffset;
			pSizeAdjustRate[ i * 4 + j ] = fSizeAdjustRate;
		}
	}

	CMemoryStream *pStream;

	pStream = new CMemoryStream( pDirection, sizeof( EtVector4 ) * nBillboardEffectCount * 4 );
	m_EmitterMeshStream.LoadVertexStream( pStream, MST_BONEWEIGHT, 0, nBillboardEffectCount * 4 );
	pStream->Initialize( pBeginningTime, sizeof( float ) * nBillboardEffectCount * 4 * 2 );
	m_EmitterMeshStream.LoadVertexStream( pStream, MST_TEXCOORD, 0, nBillboardEffectCount * 4 );
	pStream->Initialize( pVertexIndex, sizeof( short ) * nBillboardEffectCount * 4 * 4 );
	m_EmitterMeshStream.LoadVertexStream( pStream, MST_BONEINDEX, 0, nBillboardEffectCount * 4 );
	pStream->Initialize( pStartOffset, sizeof( EtVector3 ) * nBillboardEffectCount * 4 );
	m_EmitterMeshStream.LoadVertexStream( pStream, MST_NORMAL, 0, nBillboardEffectCount * 4 );
	pStream->Initialize( pSizeAdjustRate, sizeof( float ) * nBillboardEffectCount * 4 );
	m_EmitterMeshStream.LoadVertexStream( pStream, MST_DEPTH, 0, nBillboardEffectCount * 4 );

	WORD *pIndex;

	pIndex = ( WORD * )pDirection;
	for( i = 0; i < nBillboardEffectCount; i++ )
	{
		pIndex[ i * 6 ] = i * 4;
		pIndex[ i * 6 + 1 ] = i * 4 + 1;
		pIndex[ i * 6 + 2 ] = i * 4 + 2;
		pIndex[ i * 6 + 3 ] = i * 4 + 2;
		pIndex[ i * 6 + 4 ] = i * 4 + 1;
		pIndex[ i * 6 + 5 ] = i * 4 + 3;
	}
	pStream->Initialize( pIndex, nBillboardEffectCount * 3 * 2 * sizeof( WORD ) );
	m_EmitterMeshStream.LoadIndexStream( pStream, false, nBillboardEffectCount * 3 * 2 );

	delete pStream;
	delete [] pDirection;
	delete [] pBeginningTime;
	delete [] pVertexIndex;
	delete [] pStartOffset;
	delete [] pSizeAdjustRate;
}

void CEtBillboardEffectEmitter::SetEffectParam( EtMaterialHandle hMaterial )
{
	if( !IsDraw() ) return;

	int nTexIndex;
	float fParam;

	m_vecCustomParam.clear();
	fParam = m_EmitterInfo.nLifeDuration / BILLBOARD_EFFECT_FRAME;
	AddCustomParam( m_vecCustomParam, EPT_FLOAT, hMaterial, "g_fLifeTime", &fParam );
	fParam = m_nMaxEffectLifeTime / ( float )m_EmitterInfo.nLifeDuration;
	AddCustomParam( m_vecCustomParam, EPT_FLOAT, hMaterial, "g_fMultiOriginalTime", &fParam );
	AddCustomParam( m_vecCustomParam, EPT_FLOAT, hMaterial, "g_fResistTime", &m_EmitterInfo.fOpposeTime );
	AddCustomParam( m_vecCustomParam, EPT_FLOAT, hMaterial, "g_fResistScale", &m_EmitterInfo.fOpposeScaleValue );
	fParam = EtToRadian( m_EmitterInfo.fZRotateStartValue );
	AddCustomParam( m_vecCustomParam, EPT_FLOAT, hMaterial, "g_fRotateStart", &fParam );
	fParam = EtToRadian( m_EmitterInfo.fZRotateEndValue ) - EtToRadian( m_EmitterInfo.fZRotateStartValue );
	AddCustomParam( m_vecCustomParam, EPT_FLOAT, hMaterial, "g_fRotateRange", &fParam );
	AddCustomParam( m_vecCustomParam, EPT_FLOAT, hMaterial, "g_fScaleTable", m_ScaleData, EMITTER_TABLE_COUNT );
	AddCustomParam( m_vecCustomParam, EPT_VECTOR, hMaterial, "g_fColorTable", m_ColorData, EMITTER_TABLE_COUNT );	
	AddCustomParam( m_vecCustomParam, EPT_INT, hMaterial, "g_nUseRotateByDir", &m_EmitterInfo.nUseRotateByDir );

	if( m_hTexture ) {
		nTexIndex = m_hTexture->GetMyIndex();
		AddCustomParam( m_vecCustomParam, EPT_TEX, hMaterial, "g_DiffuseTex", &nTexIndex );
	}
	if( m_EmitterInfo.bUseBumpEffectTexture )
	{
		nTexIndex = GetEtBackBufferMng()->GetBackBufferIndex();
		AddCustomParam( m_vecCustomParam, EPT_TEX, hMaterial, "g_BackBuffer", &nTexIndex );
	}

	if( CEtRenderStack::IsDepthRender() ) {
		nTexIndex = CEtMRTMng::GetInstance().GetDepthTarget()->GetMyIndex();	
		AddCustomParam( m_vecCustomParam, EPT_TEX, hMaterial, "g_DepthTex", &nTexIndex );
	}
}

void CEtBillboardEffectEmitter::CreateDataContainer()
{
	int i;
	int nColorKeyCount, nAlphaKeyCount, nScaleKeyCount;

	nColorKeyCount = ( int )m_pDataContainer[ COLOR_DATA_TYPE ]->GetDataCount();
	nAlphaKeyCount = ( int )m_pDataContainer[ ALPHA_DATA_TYPE ]->GetDataCount();
	nScaleKeyCount = ( int )m_pDataContainer[ SCALE_DATA_TYPE ]->GetDataCount();

	if( !nColorKeyCount || !nAlphaKeyCount || !nScaleKeyCount )
	{
		for( i = 0; i < EMITTER_TABLE_COUNT; i++ )
		{
			m_ColorData[ i ].r = 1.0f;
			m_ColorData[ i ].g = 1.0f;
			m_ColorData[ i ].b = 1.0f;
			m_ColorData[ i ].a = 1.0f;

			m_ScaleData[ i ] = 1.0f;
		}
		return;
	}

	for( i = 0; i < EMITTER_TABLE_COUNT; i++ ) 
	{
		float fKey = i * ( 1.f / ( EMITTER_TABLE_COUNT - 1 ) );

		m_ColorData[ i ].r = m_pDataContainer[ COLOR_DATA_TYPE ]->GetInterpolationVector3( fKey )->x;
		m_ColorData[ i ].g = m_pDataContainer[ COLOR_DATA_TYPE ]->GetInterpolationVector3( fKey )->y;
		m_ColorData[ i ].b = m_pDataContainer[ COLOR_DATA_TYPE ]->GetInterpolationVector3( fKey )->z;
		m_ColorData[ i ].a = *m_pDataContainer[ ALPHA_DATA_TYPE ]->GetInterpolationFloat( fKey );
		m_ScaleData[ i ] = *m_pDataContainer[ SCALE_DATA_TYPE ]->GetInterpolationFloat( fKey );
	}
}