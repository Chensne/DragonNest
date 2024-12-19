#include "StdAfx.h"
#include "EtVolumeFog.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CEtVolumeFog::CEtVolumeFog()
{
}

CEtVolumeFog::~CEtVolumeFog()
{
	Clear();
}

void CEtVolumeFog::Clear()
{
	SAFE_RELEASE_SPTR( m_hTexture );
}

void CEtVolumeFog::Render( float fElapsedTime )
{
	EtCameraHandle hCamera;
	EtMatrix *pCamMat;
	EtVector3 vOutput;

	hCamera = CEtCamera::GetActiveCamera();
	pCamMat = hCamera->GetInvViewMat();

	m_CamXVector = *(EtVector4*)&pCamMat->_11;
	m_CamYVector = *(EtVector4*)&pCamMat->_21;
	m_CamPos = *(EtVector4*)&pCamMat->_41;

	SRenderStackElement RenderElement;

	RenderElement.hMaterial = m_hMaterial;
	RenderElement.nTechniqueIndex = 0;
	EtMatrixIdentity(&RenderElement.WorldMat );

	RenderElement.nSaveMatIndex = -1;
	RenderElement.pvecCustomParam = &m_vecCustomParam;
	RenderElement.pRenderMeshStream = &m_MeshStream;
	RenderElement.nBakeDepthIndex = DT_NONE;

	SStateBlock RenderState;
	RenderState.AddRenderState( D3DRS_CULLMODE, CULL_NONE );
	RenderElement.nStateBlockIndex = CEtStateBlockManager::GetInstance().CreateStateBlock(&RenderState);
	GetCurRenderStack()->AddAlphaRenderElement( RenderElement );
}

void CEtVolumeFog::Initialize( const char *pFogTexture, EtVector3 Pos, float fRadius, float fDensity, float fHeight, D3DXCOLOR FogColor )
{
	int nIndexCount, nVertexCount;
	EtVector2 *pTexCoord;
	EtVector3 *pvPosition;
	WORD *pwIndex;

	int nCount = (int)__max( 5, ( fRadius * fRadius * 0.0001f * fDensity));
	nVertexCount = 4 * nCount;
	nIndexCount = 6 * nCount;	
	pvPosition = new EtVector3[ nVertexCount ];
	pTexCoord = new EtVector2[ nVertexCount ];
	pwIndex = new WORD[ nIndexCount ];	

	for( int i = 0; i < nCount; i++) {
		float fAngle = 2*D3DX_PI*(rand()%1024)/1023.f;
		float fDist = sqrtf((rand()%2048)/2047.f) * fRadius;
		EtVector3 vPos;
		vPos.x = cosf(fAngle)*fDist + Pos.x;
		vPos.y = Pos.y + (rand()%50) - 25;
		vPos.z = sinf(fAngle)*fDist + Pos.z;

		pvPosition[ i*4+0 ] = vPos;
		pvPosition[ i*4+1 ] = vPos;
		pvPosition[ i*4+2 ] = vPos;
		pvPosition[ i*4+3 ] = vPos;

		pTexCoord[ i*4+0 ] = EtVector2(0, 1);
		pTexCoord[ i*4+1 ] = EtVector2(0, 0);
		pTexCoord[ i*4+2 ] = EtVector2(1, 1);
		pTexCoord[ i*4+3 ] = EtVector2(1, 0);

		pwIndex[ i*6+0 ] = i*4+0;
		pwIndex[ i*6+1 ] = i*4+1;
		pwIndex[ i*6+2 ] = i*4+2;
		pwIndex[ i*6+3 ] = i*4+2;
		pwIndex[ i*6+4 ] = i*4+1;
		pwIndex[ i*6+5 ] = i*4+3;
	}


	CMemoryStream Stream;

	Stream.Initialize( pvPosition, nVertexCount * sizeof( EtVector3 ) );
	m_MeshStream.LoadVertexStream( &Stream, MST_POSITION, 0, nVertexCount );
	Stream.Initialize( pTexCoord, nVertexCount * sizeof( EtVector2 ) );
	m_MeshStream.LoadVertexStream( &Stream, MST_TEXCOORD, 0, nVertexCount );
	Stream.Initialize( pwIndex, nIndexCount * sizeof( WORD ) );
	m_MeshStream.LoadIndexStream( &Stream, false, nIndexCount );

	delete [] pvPosition;
	delete [] pTexCoord;
	delete [] pwIndex;

	m_hMaterial = LoadResource( "VolumeFog.fx", RT_SHADER );	
#ifdef PRE_FIX_MATERIAL_DUMP
	if( !m_hMaterial ) return;
#endif
	m_hTexture = LoadResource( pFogTexture, RT_TEXTURE );
	if( m_hTexture ) {
		int nTexIndex = m_hTexture->GetMyIndex();
		AddCustomParam( m_vecCustomParam, EPT_TEX, m_hMaterial, "g_VolumeFogTex", &nTexIndex );
	}
	AddCustomParam( m_vecCustomParam, EPT_VECTOR_PTR, m_hMaterial, "g_CamXVector", &m_CamXVector );
	AddCustomParam( m_vecCustomParam, EPT_VECTOR_PTR, m_hMaterial, "g_CamYVector", &m_CamYVector );	
	AddCustomParam( m_vecCustomParam, EPT_VECTOR_PTR, m_hMaterial, "g_CamPos", &m_CamPos );
	
	AddCustomParam( m_vecCustomParam, EPT_FLOAT, m_hMaterial, "g_fFogHeight", &fHeight );
	AddCustomParam( m_vecCustomParam, EPT_VECTOR, m_hMaterial, "g_VolumeFogColor", &FogColor);
}

EtVolumeFogHandle CEtVolumeFog::CreateVolumeFog( const char *pRainTexture , EtVector3 Pos, float fRadius, float fDensity, float fHeight, D3DXCOLOR FogColor )
{
	CEtVolumeFog *pRainEffect;

	pRainEffect = new CEtVolumeFog();
	pRainEffect->Initialize( pRainTexture, Pos, fRadius, fDensity, fHeight, FogColor );

	return pRainEffect->GetMySmartPtr();
}
