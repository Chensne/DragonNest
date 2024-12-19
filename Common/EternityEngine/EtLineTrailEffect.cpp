#include "StdAfx.h"
#include "EtLineTrailEffect.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

EtIndexBuffer *CEtLineTrailEffect::s_pCommonIndexBuffer = NULL;
EtMaterialHandle CEtLineTrailEffect::s_hMaterial;

CEtLineTrailEffect::CEtLineTrailEffect(void)
{
	m_bModify = false;
	m_LineColor = EtColor( 1.0f, 1.0f, 1.0f, 1.0f );
	m_bShow = true;
	m_fElapsedTime = 0.f;
	m_fCurrentStartTime = 0.0f;
	m_fInverseLifeTime = 0.0f;
	m_fLifeTime = 0.0f;
	m_fLineWidth = 0.0f;
	m_fMinSegment = 0.0f;
}

CEtLineTrailEffect::~CEtLineTrailEffect(void)
{
}

void CEtLineTrailEffect::Initialize( EtTextureHandle hTexture, float fLifeTime, float fLineWidth, float fMinSegment )
{
	m_fLifeTime = fLifeTime;
	m_fLineWidth = fLineWidth;
	m_fMinSegment = fMinSegment;

	CreateVertexBuffer();

	int nTexIndex;
	nTexIndex = hTexture->GetMyIndex();
	AddCustomParam( m_vecCustomParam, EPT_TEX, s_hMaterial, "g_LineTrailTexture", &nTexIndex );
	AddCustomParam( m_vecCustomParam, EPT_VECTOR_PTR, s_hMaterial, "g_LineTrailColor", &m_LineColor );
	AddCustomParam( m_vecCustomParam, EPT_FLOAT, s_hMaterial, "g_fLineTrailWidth", &m_fLineWidth );
	AddCustomParam( m_vecCustomParam, EPT_FLOAT_PTR, s_hMaterial, "g_fLineTrailInverseLifeTime", &m_fInverseLifeTime );
	AddCustomParam( m_vecCustomParam, EPT_FLOAT_PTR, s_hMaterial, "g_fLineTrailStartTime", &m_fCurrentStartTime );
	
}

void CEtLineTrailEffect::CreateVertexBuffer()
{
	int nFaceCount;

	m_MeshStream.CreateVertexStream( MST_POSITION, 0, MAX_LINE_TRAIL_VERTEX );
	m_MeshStream.CreateVertexStream( MST_NORMAL, 0, MAX_LINE_TRAIL_VERTEX );
	m_MeshStream.CreateVertexStream( MST_TEXCOORD, 0, MAX_LINE_TRAIL_VERTEX );

	nFaceCount = ( MAX_LINE_TRAIL_VERTEX - 2 );
	m_MeshStream.SetIndexBuffer( s_pCommonIndexBuffer, nFaceCount * 3 );
}

void CEtLineTrailEffect::CopyVertexBuffer()
{
	CMemoryStream Stream;
	int nVertexCount;

	nVertexCount = min( ( int )m_vecPosition.size(), MAX_LINE_TRAIL_VERTEX );
	if( nVertexCount <= 0 )
	{
		return;
	}
	Stream.Initialize( &m_vecPosition[ 0 ], nVertexCount * sizeof( EtVector3 ) );
	m_MeshStream.LoadVertexStream( &Stream, MST_POSITION, 0, nVertexCount );
	Stream.Initialize( &m_vecTangent[ 0 ], nVertexCount * sizeof( EtVector3 ) );
	m_MeshStream.LoadVertexStream( &Stream, MST_NORMAL, 0, nVertexCount );
	Stream.Initialize( &m_vecTime[ 0 ], nVertexCount * sizeof( EtVector2 ) );
	m_MeshStream.LoadVertexStream( &Stream, MST_TEXCOORD, 0, nVertexCount );
}

void CEtLineTrailEffect::AddPoint( EtVector3 &Point )
{
	if( !m_vecPosition.empty() )
	{
		int i;
		float fCurTime, fLastTime;

		fLastTime = m_vecTime[ m_vecTime.size() - 1 ].x;	
		fCurTime = m_fElapsedTime;
		if( ( fCurTime - fLastTime ) < 0.01666f )	// 60분의 1초보다 빨리 들어온 포인트는 생략한다
		{
			return;
		}

		int nLastIndex, nInterpolation;
		float fLength, fElapsedTime;
		EtVector3 Tangent;

		nLastIndex = ( int )m_vecPosition.size() - 2;
		Tangent = Point - m_vecPosition[ nLastIndex ];
		fLength = EtVec3Length( &Tangent );
		m_vecTangent[ nLastIndex ] = Tangent;
		m_vecTangent[ nLastIndex + 1 ] = Tangent;
		nInterpolation = min( ( int )( fLength / m_fMinSegment ) + 1, 5 );
		fElapsedTime = fCurTime - fLastTime;

		m_vecPosition.resize( m_vecPosition.size() + nInterpolation * 2 );
		m_vecTangent.resize( m_vecPosition.size() );
		m_vecTime.resize( m_vecPosition.size() );

		for( i = 1; i <= nInterpolation; i++ )
		{
			int nCurIndex;
			float fWeight;

			fWeight = i / ( float )nInterpolation;
			nCurIndex = nLastIndex + i * 2;
			EtVec3Hermite( &m_vecPosition[ nCurIndex ], &m_vecPosition[ nLastIndex ], &m_vecTangent[ nLastIndex ], &Point, &Tangent, fWeight );
			m_vecPosition[ nCurIndex + 1 ] = m_vecPosition[ nCurIndex ];

			m_vecTangent[ nCurIndex ] = Tangent;
			m_vecTangent[ nCurIndex + 1 ] = Tangent;

			m_vecTime[ nCurIndex ].x = m_vecTime[ nLastIndex ].x + fElapsedTime * fWeight;
			m_vecTime[ nCurIndex + 1 ].x = m_vecTime[ nCurIndex ].x;
			m_vecTime[ nCurIndex ].y = 0.0f;
			m_vecTime[ nCurIndex + 1 ].y = 1.0f;
		}
		m_bModify = true;
	}
	else
	{
		EtVector2 Time;
		m_fElapsedTime = 0.f;

		m_vecPosition.push_back( Point );
		m_vecPosition.push_back( Point );
		m_vecTangent.push_back( Point );
		m_vecTangent.push_back( Point );
		Time.x = 0.0f;
		Time.y = 0.0f;
		m_vecTime.push_back( Time );
		Time.y = 1.0f;
		m_vecTime.push_back( Time );
	}
}

void CEtLineTrailEffect::Render( float fElapsedTime )
{
	m_fElapsedTime += fElapsedTime;

	int i, nEraseEnd;
	float fCurTime = m_fElapsedTime;
	nEraseEnd = -1;
	for( i = 0; i < ( int )m_vecTime.size(); i += 2 )
	{
		if( fCurTime - m_vecTime[ i ].x > m_fLifeTime )
		{
			nEraseEnd = i + 2;
		}
		else
		{
			break;
		}
	}
	if( nEraseEnd != -1 )
	{
		m_vecPosition.erase( m_vecPosition.begin(), m_vecPosition.begin() + nEraseEnd );
		m_vecTangent.erase( m_vecTangent.begin(), m_vecTangent.begin() + nEraseEnd );
		m_vecTime.erase( m_vecTime.begin(), m_vecTime.begin() + nEraseEnd );
		m_bModify = true;
	}

	if( m_bShow == true )
		return;
	
	if( m_vecPosition.size() <= 2 )
		return;

	if( m_bModify )
	{
		CopyVertexBuffer();
		m_bModify = false;
	}
	m_fCurrentStartTime = m_vecTime[ 0 ].x;
	m_fInverseLifeTime = 1.0f / ( m_vecTime[ m_vecTime.size() - 1 ].x - m_fCurrentStartTime );

	SRenderStackElement RenderElement;
	EtMatrix WorldMat;

	EtMatrixIdentity( &WorldMat );
	RenderElement.hMaterial = s_hMaterial;
	RenderElement.nTechniqueIndex = 0;
	RenderElement.WorldMat = WorldMat;
	RenderElement.nSaveMatIndex = -1;
	RenderElement.pvecCustomParam = &m_vecCustomParam;
	RenderElement.pRenderMeshStream = &m_MeshStream;
	RenderElement.nDrawCount = ( int )m_vecPosition.size() - 2;
	RenderElement.nBakeDepthIndex = DT_NONE;
	GetCurRenderStack()->AddAlphaRenderElement( RenderElement );
}

void CEtLineTrailEffect::InitializeEffect()
{
	s_hMaterial = LoadResource( "LineTrail.fx", RT_SHADER );
	if( s_pCommonIndexBuffer == NULL )
	{
		int i, nQuadCount, nFaceCount;
		WORD *pIndexBuffer;
		CMemoryStream Stream;

		nFaceCount = ( MAX_LINE_TRAIL_VERTEX - 2 );
		nQuadCount = MAX_LINE_TRAIL_VERTEX / 2 - 1;
		s_pCommonIndexBuffer = GetEtDevice()->CreateIndexBuffer( nFaceCount * 3 * sizeof( WORD ) );
		s_pCommonIndexBuffer->Lock( 0, nFaceCount * 3 * sizeof( WORD ), ( void ** )&pIndexBuffer, 0 );
		for( i = 0; i < nQuadCount; i++ )
		{
			pIndexBuffer[ i * 6 ] = i * 2;
			pIndexBuffer[ i * 6 + 1 ] = i * 2 + 2;
			pIndexBuffer[ i * 6 + 2 ] = i * 2 + 1;
			pIndexBuffer[ i * 6 + 3 ] = i * 2 + 1;
			pIndexBuffer[ i * 6 + 4 ] = i * 2 + 2;
			pIndexBuffer[ i * 6 + 5 ] = i * 2 + 3;
		}
		s_pCommonIndexBuffer->Unlock();
	}
}

void CEtLineTrailEffect::FinializeEffect()
{
	SAFE_RELEASE_SPTR( s_hMaterial );
	SAFE_RELEASE( s_pCommonIndexBuffer );
}
