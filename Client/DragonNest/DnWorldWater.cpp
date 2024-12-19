#include "StdAfx.h"
#include "DnWorldSector.h"
#include "DnWorldWater.h"
#include "DnWorldWaterRiver.h"
#include "EtWater.h"
#include "PerfCheck.h"
#include "EtRenderStack.h"
#include "DnTableDB.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnWorldWater::CDnWorldWater( CEtWorldSector *pSector )
: CEtWorldWater( pSector )

{
	m_pMesh = NULL;
	m_nRenderUniqueID = -1;
	m_nScale = 1;
}

CDnWorldWater::~CDnWorldWater()
{
	SAFE_DELETE( m_pMesh );
	SAFE_RELEASE_SPTR( m_hAlphaTexture );
	SAFE_RELEASE_SPTR( m_hMaterial );
	std::map<std::string, EtTextureHandle>::iterator it;
	for( it = m_MapUsingTexture.begin(); it!=m_MapUsingTexture.end(); it++ ) {
		SAFE_RELEASE_SPTR( it->second );
	}
	m_MapUsingTexture.clear();
	GetEtWater()->Clear();
}

bool CDnWorldWater::Load( const char *szFileName )
{
	bool bResult = CEtWorldWater::Load( szFileName );
	if( bResult ) {
		int nCount = ( ( m_bEnable ) ? 1 : 0 ) + (int)m_pVecRiver.size();
		GetEtWater()->Initialize( nCount );

		for( DWORD i=0; i<m_pVecRiver.size(); i++ ) {
			((CDnWorldWaterRiver*)m_pVecRiver[i])->RefreshWaterInfo();
		}

		if( !m_bEnable ) return true;
		CreateWater();
		RefreshWaterInfo();

		// 뭐 일단 생성 후 필요는 없으므로 지워주게 해놓차.
		SAFE_DELETEA( m_pAlphaTable );
		SAFE_DELETE_PVEC( m_pVecCustomParamList );
	}
	return bResult;
}

CEtWorldWaterRiver *CDnWorldWater::AllocRiver()
{
	return new CDnWorldWaterRiver( this );
}

void CDnWorldWater::Render( LOCAL_TIME LocalTime )
{
	if( m_bEnable ) {
		SRenderStackElement RenderElement;
		EtMatrix WorldMat;

		EtMatrixIdentity( &WorldMat );
		RenderElement.hMaterial = m_hMaterial;
		RenderElement.nTechniqueIndex = 0;
		RenderElement.WorldMat = WorldMat;
		RenderElement.PrevWorldMat = WorldMat;
		RenderElement.nSaveMatIndex = -1;
		RenderElement.pvecCustomParam = &m_VecCustomParam;
		RenderElement.pRenderMeshStream = m_pMesh;

		RenderElement.renderPriority = RP_HIGH;
		RenderElement.nBakeDepthIndex = DT_OPAQUE;
		GetCurRenderStack()->AddAlphaRenderElement( RenderElement );
	}
	for( DWORD i=0; i<m_pVecRiver.size(); i++ ) {
		((CDnWorldWaterRiver*)m_pVecRiver[i])->Render( LocalTime );
	}
}

void CDnWorldWater::CreateWater()
{
	m_pMesh = new CEtMeshStream;	

	m_fGridSize = EtMin( (float)m_pSector->GetParentGrid()->GetGridWidth() * 100.f ,
								(float)m_pSector->GetParentGrid()->GetGridHeight() * 100.f );

	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TMAP );

	int nMapIndex = CGlobalInfo::GetInstance().m_nCurrentMapIndex;
	m_nScale = 1;
	if( pSox->IsExistItem( nMapIndex ) ) {
		int nScale = pSox->GetFieldFromLablePtr( nMapIndex, "_WaterScale" )->GetInteger();
		if( nScale > 1 ) {
			m_nScale = nScale;
		}
	}

	GenerationVertex();
	GenerationIndex();
	GenerationNormal();
	GenerationTexCoord();
	GenerationShader();
	GenerationTexture();
}

void CDnWorldWater::GenerationVertex()
{
	EtVector3 vOffset = *m_pSector->GetOffset();
	float fWidth = (float)m_pSector->GetParentGrid()->GetGridWidth() * 100.f * (m_nScale*2-1);
	float fHeight = (float)m_pSector->GetParentGrid()->GetGridHeight() * 100.f * (m_nScale*2-1);

	int nWidthCount = (int)( ( fWidth / m_fGridSize ) + 1.f ) ;
	int nHeightCount = (int)( ( fHeight / m_fGridSize ) + 1.f ) ;

	int nSize = nWidthCount * nHeightCount;
	EtVector3 *pVertex = new EtVector3[ nSize ];

	for( int j=0; j<nHeightCount; j++ ) {
		for( int i=0; i<nWidthCount; i++ ) {
			pVertex[ j * nWidthCount + i ].x =  vOffset.x - ( fWidth / 2.f ) + ( i * m_fGridSize );
			pVertex[ j * nWidthCount + i ].y = GetWaterLevel();
			pVertex[ j * nWidthCount + i ].z =  vOffset.z - ( fHeight / 2.f ) + ( j * m_fGridSize );
		}
	}

	CMemoryStream Stream( pVertex, sizeof(EtVector3) * nSize );
	m_pMesh->LoadVertexStream( &Stream, MST_POSITION, 0, nSize );
	SAFE_DELETEA( pVertex );
}

void CDnWorldWater::GenerationIndex()
{
	if( !m_pMesh ) return;

	float fWidth = (float)m_pSector->GetParentGrid()->GetGridWidth() * 100.f * (m_nScale*2-1);
	float fHeight = (float)m_pSector->GetParentGrid()->GetGridHeight() * 100.f *( m_nScale*2-1);

	int nWidthCount = (int)( ( fWidth / m_fGridSize ) );
	int nHeightCount = (int)( ( fHeight / m_fGridSize ) ) ;

	int nSize = nWidthCount * nHeightCount * 6;
	WORD *pIndex = new WORD[ nSize ];

	int nOffset;
	for( int j=0; j<nHeightCount; j++ ) {
		for( int i=0; i<nWidthCount; i++ ) {
			nOffset = (j*6) * nWidthCount + (i*6);

			pIndex[nOffset] = ( j * ( nWidthCount + 1 ) ) + i;
			pIndex[nOffset + 1] = ( (j+1) * ( nWidthCount + 1 ) ) + i;
			pIndex[nOffset + 2] = ( (j+1) * ( nWidthCount + 1 ) ) + (i+1);
			pIndex[nOffset + 3] = ( j * ( nWidthCount + 1 ) ) + i;
			pIndex[nOffset + 4] = ( (j+1) * ( nWidthCount + 1 ) ) + (i+1);
			pIndex[nOffset + 5] = ( j * ( nWidthCount + 1 ) ) + (i+1);
		}
	}

	CMemoryStream Stream( pIndex, sizeof(WORD) * nSize );
	m_pMesh->LoadIndexStream( &Stream, false, nSize );

	SAFE_DELETEA( pIndex );
}

void CDnWorldWater::GenerationNormal()
{
	if( !m_pMesh ) return;

	EtVector3 vOffset = *m_pSector->GetOffset();
	float fWidth = (float)m_pSector->GetParentGrid()->GetGridWidth() * 100.f * (m_nScale*2-1);
	float fHeight = (float)m_pSector->GetParentGrid()->GetGridHeight() * 100.f * (m_nScale*2-1);

	int nWidthCount = (int)( ( fWidth / m_fGridSize ) + 1.f );
	int nHeightCount = (int)( ( fHeight / m_fGridSize ) + 1.f );

	int nSize = nWidthCount * nHeightCount;
	EtVector3 *pVertex = new EtVector3[ nSize ];

	for( int j=0; j<nHeightCount; j++ ) {
		for( int i=0; i<nWidthCount; i++ ) {
			pVertex[ j * nWidthCount + i ] = EtVector3( 0.f, 1.f, 0.f );
		}
	}

	CMemoryStream Stream( pVertex, sizeof(EtVector3) * nSize );
	m_pMesh->LoadVertexStream( &Stream, MST_NORMAL, 0, nSize );
	SAFE_DELETEA( pVertex );
}

void CDnWorldWater::GenerationTexCoord()
{
	if( !m_pMesh ) return;

	float fWidth = (float)m_pSector->GetParentGrid()->GetGridWidth() * 100.f * (m_nScale*2-1);
	float fHeight = (float)m_pSector->GetParentGrid()->GetGridHeight() * 100.f * (m_nScale*2-1);

	int nWidthCount = (int)( ( fWidth / m_fGridSize ) + 1.f );
	int nHeightCount = (int)( ( fHeight / m_fGridSize ) + 1.f );

	int nSize = nWidthCount * nHeightCount;
	EtVector2 *pUV = new EtVector2[ nSize ];

	for( int j=0; j<nHeightCount; j++ ) {
		for( int i=0; i<nWidthCount; i++ ) {
			pUV[ j * nWidthCount + i ].x = (m_nScale*2-1)*( m_vUVRatio.x / (float)(nWidthCount-1) ) * (float)i - (m_nScale-1);
			pUV[ j * nWidthCount + i ].y = (m_nScale*2-1)*( m_vUVRatio.y / (float)(nHeightCount-1) ) * (float)j - (m_nScale-1);
		}
	}

	CMemoryStream Stream( pUV, sizeof(EtVector2) * nSize );
	m_pMesh->LoadVertexStream( &Stream, MST_TEXCOORD, 0, nSize );

	for( int j=0; j<nHeightCount; j++ ) {
		for( int i=0; i<nWidthCount; i++ ) {
			pUV[ j * nWidthCount + i ].x = (m_nScale*2-1)*( 1.f / (float)(nWidthCount-1) ) * (float)i - (m_nScale-1);
			pUV[ j * nWidthCount + i ].y = (m_nScale*2-1)*( 1.f / (float)(nHeightCount-1) ) * (float)j - (m_nScale-1);
		}
	}

	Stream.Initialize( pUV, sizeof(EtVector2) * nSize );
	m_pMesh->LoadVertexStream( &Stream, MST_TEXCOORD, 1, nSize );


	SAFE_DELETEA( pUV );
}

void CDnWorldWater::GenerationShader()
{
	m_hMaterial = LoadResource( m_szShaderName.c_str(), RT_SHADER );
	if( !m_hMaterial ) return;

	for( DWORD i=0; i<m_pVecCustomParamList.size(); i++ ) {
		CustomParamStruct *pStruct = m_pVecCustomParamList[i];
		switch( pStruct->nType ) {
			case EPT_INT: 
				AddCustomParam( m_VecCustomParam, (EffectParamType)pStruct->nType, m_hMaterial, pStruct->szName.c_str(), &pStruct->nInt );
				break;
			case EPT_FLOAT:
				AddCustomParam( m_VecCustomParam, (EffectParamType)pStruct->nType, m_hMaterial, pStruct->szName.c_str(), &pStruct->fFloat );
				break;
			case EPT_VECTOR:
				AddCustomParam( m_VecCustomParam, (EffectParamType)pStruct->nType, m_hMaterial, pStruct->szName.c_str(), &pStruct->fFloat4 );
				break;
			case EPT_TEX:
				{
					std::map<std::string, EtTextureHandle>::iterator it = m_MapUsingTexture.find( m_pVecCustomParamList[i]->szName );
					if( it != m_MapUsingTexture.end() ) {
						SAFE_RELEASE_SPTR( it->second );
						m_MapUsingTexture.erase( it );
					}
					EtTextureHandle hTexture = LoadResource( pStruct->szTexName, RT_TEXTURE );
					int nTexIndex = hTexture.GetIndex();
					m_MapUsingTexture.insert( make_pair( pStruct->szName, hTexture ) );
					if( nTexIndex == -1 ) {
						nTexIndex = CEtTexture::GetBlankTexture().GetIndex();
					}
					AddCustomParam( m_VecCustomParam, (EffectParamType)pStruct->nType, m_hMaterial, pStruct->szName.c_str(), &nTexIndex );
				}
				break;
		}
	}

	AddCustomParam( m_VecCustomParam, EPT_MATRIX_PTR, m_hMaterial, "g_LastViewMat", GetEtWater()->GetWaterBakeViewMat() );
}


void CDnWorldWater::GenerationTexture()
{	
#ifdef PRE_FIX_MATERIAL_DUMP
	if( !m_hMaterial ) return;
#endif

	m_hAlphaTexture = CEtTexture::CreateNormalTexture( m_nAlphaTableWidth, m_nAlphaTableHeight, FMT_A8R8G8B8, USAGE_DEFAULT, POOL_MANAGED );
	if( m_hAlphaTexture ) {
		int nTexIndex = m_hAlphaTexture.GetIndex();
		AddCustomParam( m_VecCustomParam, EPT_TEX, m_hMaterial, "g_TransparencyTex", &nTexIndex );
	}

	int nStride;
	float fValue;
	DWORD *pPtr = (DWORD*)m_hAlphaTexture->Lock( nStride );

	for( int j=0; j<m_nAlphaTableHeight; j++ ) {
		for( int i=0; i<m_nAlphaTableWidth; i++ ) {
			fValue = m_pAlphaTable[j*m_nAlphaTableWidth+i] * m_fRevisionAlpha;
			if( fValue < 0.f ) fValue = 0.f;
			if( fValue > 255.f ) fValue = 255.f;
			*pPtr = D3DCOLOR_ARGB( (BYTE)fValue, 0, 0, 0 );
			pPtr++;
		}
	}

	m_hAlphaTexture->Unlock();

//	m_hAlphaTexture->ChangeFormat( FMT_DXT5 );
}

int CDnWorldWater::GetWaterIndex( void *pWater )
{
	int nIndex = 0;
	if( m_bEnable ) {
		nIndex++;
		if( pWater == this ) return 0;
	}
	for( DWORD i=0; i<m_pVecRiver.size(); i++ ) {
		if( m_pVecRiver[i] == pWater ) return nIndex + i;
	}
	return -1;
}

void CDnWorldWater::RefreshWaterInfo()
{
	if( !m_hMaterial ) return;
	int nIndex = GetWaterIndex( this );
	int nTexIndex = -1;
	if( nIndex != -1 ) {
		GetEtWater()->SetWaterHeight( nIndex, m_fWaterLevel );
		GetEtWater()->SetWaterIgnoreBake( nIndex, false );

		EtTextureHandle hTexture = GetEtWater()->GetWaterMapTexture( nIndex );
		nTexIndex = hTexture.GetIndex();
	}
	AddCustomParam( m_VecCustomParam, EPT_TEX, m_hMaterial, "g_WaterMapTex", &nTexIndex );
}