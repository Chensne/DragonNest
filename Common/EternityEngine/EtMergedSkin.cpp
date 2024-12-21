#include "StdAfx.h"
#include "EtSkin.h"
#include "EtMesh.h"
#include "EtLoader.h"
#include "EtCustomParam.h"
#include "EtMergedSkin.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

#define MASK_TEXTURE_SCALE 2
CEtMergedSkin::CEtMergedSkin()
{	
	m_bDeleteImmediate = true;

	m_nTextureWidth = 512;
	m_nTextureHeight = 512;
} 

CEtMergedSkin::~CEtMergedSkin()
{
	// EtSkin 디스트럭터에서 지워주기 때문에 여기서 지울 필요 없다.
//	SAFE_RELEASE_SPTR( m_hCombineTexture );
//	SAFE_RELEASE_SPTR( m_hCombineMaskTexture );
	SAFE_RELEASE_SPTR( m_MeshHandle );
}

EtVector4 CEtMergedSkin::MergeTexture( EtTextureHandle hTexture, EtTextureHandle hMaskTexture, int nMergeOrder, RECT TextureRect )
{
	EtVector4 ScaleBias;
	ScaleBias.x = ( TextureRect.right - TextureRect.left ) / ( float )m_nTextureWidth;
	ScaleBias.y = ( TextureRect.bottom - TextureRect.top ) / ( float )m_nTextureHeight;
	ScaleBias.z = TextureRect.left / ( float )m_nTextureWidth;;
	ScaleBias.w = TextureRect.top / ( float )m_nTextureHeight;

	if( ( m_hCombineTexture ) && ( m_hCombineTexture->GetTexturePtr() ) && ( hTexture ) && ( hTexture->GetTexturePtr() ) )
	{
		EtSurface *pCombineSurface = m_hCombineTexture->GetSurfaceLevel();
		EtSurface *pSourSurface = hTexture->GetSurfaceLevel();
		D3DXLoadSurfaceFromSurface( pCombineSurface, NULL, &TextureRect, pSourSurface, NULL, NULL, D3DX_DEFAULT, 0 );
	}

	if( ( m_hCombineMaskTexture ) && ( m_hCombineMaskTexture->GetTexturePtr() ) && ( hMaskTexture ) && ( hMaskTexture->GetTexturePtr() ) )
	{
		TextureRect.left /= MASK_TEXTURE_SCALE;
		TextureRect.right /= MASK_TEXTURE_SCALE;
		TextureRect.top /= MASK_TEXTURE_SCALE;
		TextureRect.bottom /= MASK_TEXTURE_SCALE;
		EtSurface *pCombineSurface = m_hCombineMaskTexture->GetSurfaceLevel();
		EtSurface *pSourSurface = hMaskTexture->GetSurfaceLevel();
		D3DXLoadSurfaceFromSurface( pCombineSurface, NULL, &TextureRect, pSourSurface, NULL, NULL, D3DX_DEFAULT, 0 );
	}

	return ScaleBias;
}

bool CEtMergedSkin::Assign( EtSkinHandle hSkin, RECT TextureRect )
{
	m_SkinHeader = hSkin->m_SkinHeader;
	m_vecSubSkinHeader = hSkin->m_vecSubSkinHeader;
	m_vecMaterialHandle = hSkin->m_vecMaterialHandle;
	int i, nSize;
	nSize = (int)m_vecMaterialHandle.size();
	for( i = 0; i < nSize; i++) {
		m_vecMaterialHandle[i]->AddRef();
	}
	m_vecCustomParam = hSkin->m_vecCustomParam;
	m_IsAlphaTextureList = hSkin->m_IsAlphaTextureList;

	m_hCombineTexture = CEtTexture::CreateNormalTexture( m_nTextureWidth, m_nTextureHeight, FMT_DXT1, USAGE_DEFAULT, POOL_MANAGED );
	if( !m_hCombineTexture || m_hCombineTexture->GetTexturePtr() == NULL ) {
		return false;
	}

	EtParameterHandle hEmissiveParam = m_vecMaterialHandle[ 0 ]->GetParameterByName( "g_EmissiveColor" );
	EtParameterHandle hPowerParam = m_vecMaterialHandle[ 0 ]->GetParameterByName( "g_EmissivePower" );
	EtParameterHandle hRangeParam = m_vecMaterialHandle[ 0 ]->GetParameterByName( "g_EmissivePowerRange" );
	EtParameterHandle hAniSpeedParam = m_vecMaterialHandle[ 0 ]->GetParameterByName( "g_EmissiveAniSpeed" );

	int *pTextureIndex = NULL, *pMaskTextureIndex = NULL;
	if( !m_vecCustomParam.empty() ) {
		for( int j = 0; j < ( int )m_vecCustomParam[ 0 ].size(); j++ )
		{
			if( m_vecCustomParam[ 0 ][ j ].Type == EPT_TEX )
			{
				if( m_vecCustomParam[ 0 ][ j ].nTextureIndex != -1 ) {
					if( !pTextureIndex ) {
						pTextureIndex = &m_vecCustomParam[ 0 ][ j ].nTextureIndex;
					}
					else {
						ASSERT( pMaskTextureIndex == NULL );
						pMaskTextureIndex = &m_vecCustomParam[ 0 ][ j ].nTextureIndex;					
					}
				}
			}
			else if( hEmissiveParam != -1 && m_vecCustomParam[ 0 ][ j ].hParamHandle == hEmissiveParam ) {
				memcpy(&m_EmissiveParam[ 0 ].Color, m_vecCustomParam[ 0 ][ j ].fFloat4, sizeof(float)*4);
				m_vecCustomParam[ 0 ][ j ].nVariableCount = 1;
			}
			else if( hPowerParam != -1 && m_vecCustomParam[ 0 ][ j ].hParamHandle == hPowerParam ) {
				m_EmissiveParam[ 0 ].fPower = m_vecCustomParam[ 0 ][ j ].fFloat;
			}
			else if( hRangeParam != -1 && m_vecCustomParam[ 0 ][ j ].hParamHandle == hRangeParam ) {
				m_EmissiveParam[ 0 ].fRange = m_vecCustomParam[ 0 ][ j ].fFloat;
			}
			else if( hAniSpeedParam != -1 && m_vecCustomParam[ 0 ][ j ].hParamHandle == hAniSpeedParam ) {
				m_EmissiveParam[ 0 ].fAniSpeed = m_vecCustomParam[ 0 ][ j ].fFloat;
			}
		}
	}

	EtTextureHandle hTexture, hMaskTexture;

	if( pTextureIndex ) {
		hTexture = CEtResource::GetSmartPtr( *pTextureIndex );
	}
	if( !hTexture ) return false;

	if( pMaskTextureIndex ) {
		hMaskTexture = CEtResource::GetSmartPtr( *pMaskTextureIndex );
		m_hCombineMaskTexture = CEtTexture::CreateNormalTexture( m_nTextureWidth / MASK_TEXTURE_SCALE, m_nTextureHeight / MASK_TEXTURE_SCALE, FMT_DXT1, USAGE_DEFAULT, POOL_MANAGED );
		if( !m_hCombineMaskTexture || m_hCombineMaskTexture->GetTexturePtr() == NULL ) {
			return false;
		}

		int nStride;
		char *pPtr = ( char * )m_hCombineMaskTexture->Lock( nStride );
		if( pPtr )
		{
			// dxt1 이라서 압축되 있어서.. Height 나누기 4 해줬다.
			memset( pPtr, 0, nStride * m_hCombineMaskTexture->Height() / 4 );
		}
		m_hCombineMaskTexture->Unlock();
	}

	EtVector4 ScaleBias = MergeTexture( hTexture, hMaskTexture, 0, TextureRect );
	if( pTextureIndex ) {
		*pTextureIndex = m_hCombineTexture->GetMyIndex();
	}
	if( pMaskTextureIndex ) {
		*pMaskTextureIndex = m_hCombineMaskTexture->GetMyIndex();
	}

	m_nAlphaParamIndex = hSkin->m_nAlphaParamIndex;
	m_DiffuseTexIndexList.resize(1);
	m_DiffuseTexIndexList[0] = m_hCombineTexture->GetMyIndex();

	m_vecTehiniqueIndex = hSkin->m_vecTehiniqueIndex;

	m_hShadowMaterial = hSkin->m_hShadowMaterial;
	hSkin->m_hShadowMaterial->AddRef();

	m_MeshHandle = (new CEtMesh)->GetMySmartPtr();
	m_MeshHandle->Assign( hSkin->m_MeshHandle, ScaleBias );

	return true;
}

#ifdef PRE_MOD_WARRIOR_RIGHTHAND
void CEtMergedSkin::Merge( EtSkinHandle hSkin, RECT TextureRect, CEtSkinInstance *pSkinInstance )
#else
void CEtMergedSkin::Merge( EtSkinHandle hSkin, RECT TextureRect )
#endif	// #ifdef PRE_MOD_WARRIOR_RIGHTHAND
{
	if( !hSkin->GetMeshHandle() || hSkin->GetMeshHandle()->GetSubMeshCount() == 0 ) return;

	EtParameterHandle hSourceEmissiveParam = hSkin->m_vecMaterialHandle[ 0 ]->GetParameterByName( "g_EmissiveColor" );
	EtParameterHandle hSourcePowerParam = m_vecMaterialHandle[ 0 ]->GetParameterByName( "g_EmissivePower" );
	EtParameterHandle hSourceRangeParam = m_vecMaterialHandle[ 0 ]->GetParameterByName( "g_EmissivePowerRange" );
	EtParameterHandle hSourceAniSpeedParam = m_vecMaterialHandle[ 0 ]->GetParameterByName( "g_EmissiveAniSpeed" );

	int nSourceEmissiveParamIndex = -1;
	int nSourcePowerParamIndex = -1;
	int nSourceRangeParamIndex = -1;
	int nSourceAniSpeedParamIndex = -1;

	int nSourceTextureIndex = -1, nSourceMaskTextureIndex = -1;
	if( !m_vecCustomParam.empty() ) {
		for( int j = 0; j < ( int )m_vecCustomParam[ 0 ].size(); j++ )
		{
			if( m_vecCustomParam[ 0 ][ j ].Type == EPT_TEX )
			{
				if( m_vecCustomParam[ 0 ][ j ].nTextureIndex != -1 ) {
					if( nSourceTextureIndex == -1 ) {
						nSourceTextureIndex = m_vecCustomParam[ 0 ][ j ].nTextureIndex;
					}
					else {
						ASSERT( nSourceMaskTextureIndex == -1 );
						nSourceMaskTextureIndex = m_vecCustomParam[ 0 ][ j ].nTextureIndex;
					}
				}
			}
			else if( hSourceEmissiveParam != -1 && m_vecCustomParam[ 0 ][ j ].hParamHandle == hSourceEmissiveParam ) {				
				nSourceEmissiveParamIndex = j;
			}
			else if( hSourcePowerParam != -1 && m_vecCustomParam[ 0 ][ j ].hParamHandle == hSourcePowerParam ) {				
				nSourcePowerParamIndex = j;
			}
			else if( hSourceRangeParam != -1 && m_vecCustomParam[ 0 ][ j ].hParamHandle == hSourceRangeParam ) {				
				nSourceRangeParamIndex = j;
			}
			else if( hSourceAniSpeedParam != -1 && m_vecCustomParam[ 0 ][ j ].hParamHandle == hSourceAniSpeedParam ) {				
				nSourceAniSpeedParamIndex = j;
			}
		}
	}
	if( nSourceTextureIndex == -1 ) {
		return;
	}

	EtParameterHandle hEmissiveParam = hSkin->m_vecMaterialHandle[ 0 ]->GetParameterByName( "g_EmissiveColor" );
	EtParameterHandle hPowerParam = hSkin->m_vecMaterialHandle[ 0 ]->GetParameterByName( "g_EmissivePower" );
	EtParameterHandle hRangeParam = hSkin->m_vecMaterialHandle[ 0 ]->GetParameterByName( "g_EmissivePowerRange" );
	EtParameterHandle hAniSpeedParam = hSkin->m_vecMaterialHandle[ 0 ]->GetParameterByName( "g_EmissiveAniSpeed" );

	int nDestTextureIndex = -1, nDestMaskTextureIndex = -1;
	int nMergeOrder = 0;
	if( nSourceEmissiveParamIndex != -1 ) {
		nMergeOrder = m_vecCustomParam[ 0 ][ nSourceEmissiveParamIndex ].nVariableCount;
	}
	if( !hSkin->m_vecCustomParam.empty() ) {
		for( int j = 0; j < ( int )hSkin->m_vecCustomParam[ 0 ].size(); j++ )
		{
			if( hSkin->m_vecCustomParam[ 0 ][ j ].Type == EPT_TEX )
			{
				if( hSkin->m_vecCustomParam[ 0 ][ j ].nTextureIndex != -1 ) {
					if( nDestTextureIndex == -1 ) {
						nDestTextureIndex = hSkin->m_vecCustomParam[ 0 ][ j ].nTextureIndex;
					}
					else {
						ASSERT( nDestMaskTextureIndex == -1 );
						nDestMaskTextureIndex = hSkin->m_vecCustomParam[ 0 ][ j ].nTextureIndex;
					}
				}
			}
			else if( nSourceEmissiveParamIndex != -1 && hEmissiveParam != -1 && hSkin->m_vecCustomParam[ 0 ][ j ].hParamHandle == hEmissiveParam ) {
				memcpy(&m_EmissiveParam[ nMergeOrder ].Color, hSkin->m_vecCustomParam[ 0 ][ j ].fFloat4, sizeof(EtColor) );
				m_vecCustomParam[ 0 ][ nSourceEmissiveParamIndex ].nVariableCount++;
				m_vecCustomParam[ 0 ][ nSourceEmissiveParamIndex ].pPointer = (void *)m_EmissiveParam;
			}
			else if( nSourcePowerParamIndex != -1 && hPowerParam != -1 && hSkin->m_vecCustomParam[ 0 ][ j ].hParamHandle == hPowerParam ) {
				m_EmissiveParam[ nMergeOrder ].fPower = hSkin->m_vecCustomParam[ 0 ][ j ].fFloat;
				m_vecCustomParam[ 0 ][ nSourcePowerParamIndex ].nVariableCount++;
			}
			else if( nSourceRangeParamIndex != -1 && hRangeParam != -1 && hSkin->m_vecCustomParam[ 0 ][ j ].hParamHandle == hRangeParam ) {
				m_EmissiveParam[ nMergeOrder ].fRange = hSkin->m_vecCustomParam[ 0 ][ j ].fFloat;
				m_vecCustomParam[ 0 ][ nSourceRangeParamIndex ].nVariableCount++;
			}
			else if( nSourceAniSpeedParamIndex != -1 && hAniSpeedParam != -1 && hSkin->m_vecCustomParam[ 0 ][ j ].hParamHandle == hAniSpeedParam ) {
				m_EmissiveParam[ nMergeOrder ].fAniSpeed = hSkin->m_vecCustomParam[ 0 ][ j ].fFloat;
				m_vecCustomParam[ 0 ][ nSourceAniSpeedParamIndex ].nVariableCount++;
			}
		}
	}

	if( nDestTextureIndex == -1 ) {
		return;
	}



	EtTextureHandle hTexture = CEtResource::GetSmartPtr( nDestTextureIndex );
	EtTextureHandle hMaskTexture;

	if( !m_hCombineTexture || !hTexture ) return;
	if( nDestMaskTextureIndex != -1 ) {
		hMaskTexture = CEtResource::GetSmartPtr( nDestMaskTextureIndex );
	}

	EtVector4 ScaleBias = MergeTexture( hTexture, hMaskTexture, nMergeOrder, TextureRect );

	std::map< std::string, int > BoneSets;

	int j, nBoneSize;

	std::vector<std::string> BoneNames;

	for( j = 0; j < hSkin->GetMeshHandle()->GetSubMeshCount(); j++) {
		if( hSkin->IsAlphaTexture( j ) ) {		// 알파텍스쳐가 단 하나라도 있으면 TRUE 
			m_IsAlphaTextureList[ 0 ] = TRUE;
		}
		for( int k = 0; k < (int)hSkin->GetMeshHandle()->GetSubMesh(j)->GetBoneName().size(); k++ ) {
			BoneNames.push_back( hSkin->GetMeshHandle()->GetSubMesh(j)->GetBoneName()[k] );
		}		
	}
	nBoneSize = (int)BoneNames.size();
	for( j = 0; j < nBoneSize; j++) {
		BoneSets.insert( std::make_pair(BoneNames[j], 0) );
	}

	std::vector<std::string> *pThisBoneNames = NULL;
	if( GetMeshHandle()->GetSubMeshCount() > 0 ) {
		pThisBoneNames = &GetMeshHandle()->GetSubMesh(0)->GetBoneName();
	}

	if( pThisBoneNames ) {
		nBoneSize = (int)pThisBoneNames->size();
		for( j = 0; j < nBoneSize; j++) {
			BoneSets.insert( std::make_pair( (*pThisBoneNames)[j], 0) );
		}
	}

	std::map< std::string, int >::iterator it = BoneSets.begin();
	int nCount = 0;
	while( it != BoneSets.end() ) {
		it->second = nCount++;
		++it;
	}

	std::vector< int > boneRemapTable;

	if( pThisBoneNames ) {
		for( j = 0; j < nBoneSize; j++) {
			int nIndex = BoneSets[ (*pThisBoneNames)[j] ] ;
			boneRemapTable.push_back( nIndex );
		}
		GetMeshHandle()->GetSubMesh(0)->GetMeshStream()->RemapBoneIndices( boneRemapTable );
	}

	GetMeshHandle()->GetSubMesh(0)->RemoveLinkInfo();	

	if( pThisBoneNames ) {
		it = BoneSets.begin();
		while( it != BoneSets.end() ) {
			(*pThisBoneNames).push_back( it->first );
			++it;
		}
	}

	for( j = 0; j < hSkin->GetMeshHandle()->GetSubMeshCount(); j++) {

#ifdef PRE_MOD_WARRIOR_RIGHTHAND
		if( pSkinInstance && pSkinInstance->GetSkinRenderInfo( j ) && !pSkinInstance->GetSkinRenderInfo( j )->bShowSubmesh ) continue;
#endif	// #ifdef PRE_MOD_WARRIOR_RIGHTHAND

		nBoneSize = (int)hSkin->GetMeshHandle()->GetSubMesh(j)->GetBoneName().size();
		boneRemapTable.clear();
		for( int k = 0; k < nBoneSize; k++) {
			assert(BoneSets.count( hSkin->GetMeshHandle()->GetSubMesh(j)->GetBoneName()[k]) != 0);
			int nIndex = BoneSets[ hSkin->GetMeshHandle()->GetSubMesh(j)->GetBoneName()[k] ] ;
			boneRemapTable.push_back( nIndex );
		}

		GetMeshHandle()->GetSubMesh(0)->GetMeshStream()->Attach( hSkin->GetMeshHandle()->GetSubMesh(j)->GetMeshStream(), boneRemapTable, ScaleBias , nMergeOrder);	
	}
}

void CEtMergedSkin::BuildMipmap()
{
	if( m_hCombineTexture )
	{
		m_hCombineTexture->BuildMipmap();
	}
}