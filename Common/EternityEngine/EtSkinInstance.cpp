#include "StdAfx.h"
#include "EtSkinInstance.h"
#include "EtOptionController.h"
#include "EtMRTMng.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CEtSkinInstance::CEtSkinInstance(void)
{
	m_nAlphaParamIndex = -1;
	m_fDirLightAttenuation = 1.0f;
	m_bRenderAlphaTwoPass = false;
	m_bSkyBox = false;
	m_bSkipBakeDepth = false;
	m_RenderPriority = RP_NORMAL;
	m_fMeshRadius = 0.f;
	m_cDepthAlphaRef = 0x80;
}

CEtSkinInstance::~CEtSkinInstance(void)
{
}

int CEtSkinInstance::AddCustomParam( const char *pParamName )
{	
	int i, j;
	EtMaterialHandle hMaterial;
	EtParameterHandle hParameter;

	for( i = 0; i < ( int )m_vecCustomParamIndex.size(); i++ )
	{
		if( i >= (int)m_vecMaterialHandle.size() ) continue;
		hMaterial = m_vecMaterialHandle[ i ];
		hParameter = hMaterial->GetParameterByName( pParamName );
		for( j = 0; j < ( int )m_vecCustomParamIndex[ i ].size(); j++ )
		{
			int nCustomParamIndex = m_vecCustomParamIndex[ i ][ j ];
			if( nCustomParamIndex == -1 )
			{
				continue;
			}
			if( m_vecCustomParam[ i ][ nCustomParamIndex ].hParamHandle == hParameter )
			{
				return j;
			}
		}
	}

	bool bFindParam = false;
	std::vector< int > vecAddIndex;
	vecAddIndex.resize( m_vecCustomParamIndex.size() );
	for( i = 0; i < ( int )m_vecCustomParamIndex.size(); i++ )
	{
		vecAddIndex[ i ] = -1;
		if( ( i >= (int)m_vecMaterialHandle.size() ) || ( i >= (int)m_vecCustomParam.size() ) )
		{
			continue;
		}
		hMaterial = m_vecMaterialHandle[ i ];
		hParameter = hMaterial->GetParameterByName( pParamName );

		for( j = 0; j < ( int )m_vecCustomParam[ i ].size(); j++ )
		{
			if( m_vecCustomParam[ i ][ j ].hParamHandle == hParameter )
			{
				bFindParam = true;
				vecAddIndex[ i ] = j;
				break;
			}
		}
	}
	if( !bFindParam )
	{
		return -1;
	}

	for( i = 0; i < ( int )m_vecCustomParamIndex.size(); i++ )
	{
		m_vecCustomParamIndex[ i ].push_back( vecAddIndex[ i ] );
	}

	return ( int )m_vecCustomParamIndex[ 0 ].size() - 1;
}

bool CEtSkinInstance::SetCustomParam( int nParamIndex, void *pValue, int nSubMeshIndex )
{
	bool bResult = false;
	if( m_vecCustomParam.empty() || ( int )m_vecCustomParam[ 0 ].size() <= nParamIndex || nParamIndex < 0 )
	{
		ASSERT( 0 );
		return bResult;
	}	

	int i;
	for( i = 0; i < ( int )m_vecCustomParamIndex.size(); i++ )
	{
		bool bChange = false;
		if( nParamIndex >= (int)m_vecCustomParamIndex[ i ].size()  ) {
			continue;
		}
		int nCustomParamIndex = m_vecCustomParamIndex[ i ][ nParamIndex ];
		if( nCustomParamIndex == -1 )
		{
			continue;
		}
		switch( m_vecCustomParam[ i ][ nCustomParamIndex ].Type )
		{
		case EPT_INT:
			if( ( nSubMeshIndex == -1 ) || ( nSubMeshIndex == i ) )
			{
				if( m_vecCustomParam[ i ][ m_vecCustomParamIndex[ i ][ nParamIndex ] ].nInt != *( int * )pValue ) bChange = true;
				m_vecCustomParam[ i ][ m_vecCustomParamIndex[ i ][ nParamIndex ] ].nInt = *( int * )pValue;
			}
			break;
		case EPT_FLOAT:
			if( ( nSubMeshIndex == -1 ) || ( nSubMeshIndex == i ) )
			{
				if( m_vecCustomParam[ i ][ m_vecCustomParamIndex[ i ][ nParamIndex ] ].fFloat != *( float * )pValue ) bChange = true;
				m_vecCustomParam[ i ][ m_vecCustomParamIndex[ i ][ nParamIndex ] ].fFloat = *( float * )pValue;
				
			}
			break;
		case EPT_VECTOR:
			if( ( nSubMeshIndex == -1 ) || ( nSubMeshIndex == i ) )
			{
				if( memcmp(m_vecCustomParam[ i ][ m_vecCustomParamIndex[ i ][ nParamIndex ] ].fFloat4, pValue,  sizeof( float ) * 4) != 0 ) bChange = true;
				memcpy( m_vecCustomParam[ i ][ m_vecCustomParamIndex[ i ][ nParamIndex ] ].fFloat4, pValue, sizeof( float ) * 4 );
				m_vecCustomParam[ i ][ m_vecCustomParamIndex[ i ][ nParamIndex ] ].pUserStruct = NULL;
			}
			break;
		case EPT_TEX:
			if( ( nSubMeshIndex == -1 ) || ( nSubMeshIndex == i ) )
			{
				if( m_vecCustomParam[ i ][ m_vecCustomParamIndex[ i ][ nParamIndex ] ].nTextureIndex != *( int * )pValue ) bChange = true;
				m_vecCustomParam[ i ][ m_vecCustomParamIndex[ i ][ nParamIndex ] ].nTextureIndex = *( int * )pValue;				
			}
			break;
		default:
			ASSERT( 0 );
			break;
		}
		if( bChange ) {
			bResult = true;
			for( int j = 0; j < (int)m_vecCustomParam[ i ].size(); j++) {
				m_vecCustomParam[ i ][ j ].pUserStruct = NULL;
			}
		}
	}

	return bResult;
}

void CEtSkinInstance::RestoreCustomParam( int nParamIndex, int nSubMeshIndex )
{
	if( ( int )m_vecCustomParam.size() <= nParamIndex )
	{
		ASSERT( 0 );
		return;
	}

	int i;

	for( i = 0; i < ( int )m_vecCustomParamIndex.size(); i++ )
	{
		if( nParamIndex >= ( int )m_vecCustomParamIndex[i].size() ) continue;
		if( ( nSubMeshIndex == -1 ) || ( nSubMeshIndex == i ) )
		{
			if( nParamIndex == -1 ) {
				for( DWORD j=0; j<m_vecCustomParamIndex[ i ].size(); j++ ) 
				{
					int nCustomParamIndex = m_vecCustomParamIndex[ i ][ j ];
					if( nCustomParamIndex == -1 )
					{
						continue;
					}
					m_vecCustomParam[ i ][ nCustomParamIndex ] = m_hSkin->m_vecCustomParam[ i ][ m_vecCustomParamIndex[ i ][ j ] ];
				}
			}
			else
			{
				int nCustomParamIndex = m_vecCustomParamIndex[ i ][ nParamIndex ];
				if( nCustomParamIndex != -1 )
				{
					m_vecCustomParam[ i ][ m_vecCustomParamIndex[ i ][ nParamIndex ] ] = m_hSkin->m_vecCustomParam[ i ][ m_vecCustomParamIndex[ i ][ nParamIndex ] ];
				}
			}
		}
	}
}

void CEtSkinInstance::CopySkinInfo( EtSkinHandle hSkin )
{
	int i, j;
	SSkinRenderInfo SkinRenderInfo;

	m_hSkin = hSkin;

	m_hShadowMaterial = hSkin->m_hShadowMaterial;
	m_MeshHandle = hSkin->m_MeshHandle;	
	if( !m_MeshHandle ) {
		return;
	}

	EtVector3 vExtent, vOrigin;
	m_MeshHandle->GetExtent(vOrigin, vExtent);
	vExtent.y = 0.f;
	m_fMeshRadius = EtVec3Length( &vExtent );

	m_nAlphaParamIndex = hSkin->m_nAlphaParamIndex;
	m_vecMaterialHandle.resize( hSkin->m_vecMaterialHandle.size() );
	m_vecSkinRenderInfo.resize( m_MeshHandle->GetSubMeshCount() );
	m_vecCustomParam.resize( m_MeshHandle->GetSubMeshCount() );
	m_vecCustomParamIndex.resize( m_MeshHandle->GetSubMeshCount() );
	m_vecShadowParam.resize( m_MeshHandle->GetSubMeshCount() );
	m_vecRenderUniqueID.resize( m_MeshHandle->GetSubMeshCount() );

	std::copy( hSkin->m_vecMaterialHandle.begin(), hSkin->m_vecMaterialHandle.end(), m_vecMaterialHandle.begin() );

	SkinRenderInfo.BlendOP = BLENDOP_ADD;
	SkinRenderInfo.SrcBlend = BLEND_SRCALPHA;
	SkinRenderInfo.DestBlend = BLEND_INVSRCALPHA;
	SkinRenderInfo.bTwoSide = false;
	SkinRenderInfo.bUVTiling = false;
	for( i = 0; i < m_MeshHandle->GetSubMeshCount(); i++ ) 
	{
		m_vecRenderUniqueID[ i ] = -1;
		SkinRenderInfo.nTechniqueIndex = hSkin->m_vecTehiniqueIndex[ i ];
		SkinRenderInfo.bEnableAlphablend = hSkin->m_vecSubSkinHeader[ i ].bEnableAlphablend;
		SkinRenderInfo.fAlphaValue = hSkin->m_vecSubSkinHeader[ i ].fAlphaValue;
		SkinRenderInfo.bShowSubmesh = true;
		m_vecSkinRenderInfo[ i ] = SkinRenderInfo;

		m_vecCustomParam[ i ].resize( hSkin->m_vecCustomParam[ i ].size() );
		for( j = 0; j < ( int )hSkin->m_vecCustomParam[ i ].size(); j++ )
		{
			m_vecCustomParam[ i ][ j ] = hSkin->m_vecCustomParam[ i ][ j ];
		}

		//::AddCustomParam( m_vecShadowParam[ i ], EPT_TEX, m_hShadowMaterial, "g_DiffuseTex", (void*)&hSkin->GetDiffuseTexIndex( i ) );
		bool bNormalDiffuseTexture = true;
		if( hSkin->GetDiffuseTexIndex( i ) != -1 )
		{
			EtTextureHandle hTexture = CEtResource::GetResource( m_hSkin->GetDiffuseTexIndex( i ) );
			if( hTexture && hTexture->GetTextureType() == ETTEXTURE_VOLUME ) bNormalDiffuseTexture = false;
		}
		if( bNormalDiffuseTexture )
			::AddCustomParam( m_vecShadowParam[ i ], EPT_TEX, m_hShadowMaterial, "g_DiffuseTex", (void*)&hSkin->GetDiffuseTexIndex( i ) );
		else
			::AddCustomParam( m_vecShadowParam[ i ], EPT_TEX, m_hShadowMaterial, "g_DiffuseVolumeTex", (void*)&hSkin->GetDiffuseTexIndex( i ) );
	}

	for( i = 0; i < ( int )m_vecMaterialHandle.size(); i++ ) {
		if( strstr( m_vecMaterialHandle[ i ]->GetFileName(), "skybox" ) ) {
			m_bSkyBox = true;
			if( m_vecMaterialHandle[ i ]->GetParameterByName( "g_DepthTex" ) != -1 ) {
				int nDepthTexIndex = CEtMRTMng::GetInstance().GetDepthTarget()->GetMyIndex();
				::AddCustomParam( m_vecCustomParam[ i ], EPT_TEX, m_vecMaterialHandle[ i ], "g_DepthTex", &nDepthTexIndex );
			}
			break;
		}
	}
}

void CEtSkinInstance::SetBlendOP( EtBlendOP BlendOP, int nSubmeshIndex )
{
	if( nSubmeshIndex == -1 ) {
		for( int i = 0; i < ( int )m_vecSkinRenderInfo.size(); i++ ) {
			m_vecSkinRenderInfo[ i ].BlendOP = BlendOP;
		}
	}
	else {
		m_vecSkinRenderInfo[ nSubmeshIndex ].BlendOP = BlendOP;
	}
}

void CEtSkinInstance::SetSrcBlend( EtBlendMode SrcBlend, int nSubmeshIndex )
{
	if( nSubmeshIndex == -1 ) {
		for( int i = 0; i < ( int )m_vecSkinRenderInfo.size(); i++ ) {
			m_vecSkinRenderInfo[ i ].SrcBlend = SrcBlend;
		}
	}
	else {
		m_vecSkinRenderInfo[ nSubmeshIndex ].SrcBlend = SrcBlend;
	}
}

void CEtSkinInstance::SetDestBlend( EtBlendMode DestBlend, int nSubmeshIndex )
{
	if( nSubmeshIndex == -1 ) {
		for( int i = 0; i < ( int )m_vecSkinRenderInfo.size(); i++ ) {
			m_vecSkinRenderInfo[ i ].DestBlend = DestBlend;
		}
	}
	else {
		m_vecSkinRenderInfo[ nSubmeshIndex ].DestBlend = DestBlend;
	}
}

void CEtSkinInstance::SetCullMode( bool bTwoSide, int nSubMeshIndex )
{
	if( nSubMeshIndex == -1 ) {
		for( int i = 0; i < ( int )m_vecSkinRenderInfo.size(); i++ ) {
			m_vecSkinRenderInfo[ i ].bTwoSide = bTwoSide;
		}
	}
	else {
		m_vecSkinRenderInfo[ nSubMeshIndex ].bTwoSide = bTwoSide;
	}	
}

void CEtSkinInstance::SetUVTiling( bool bUVTiling, int nSubMeshIndex )
{
	if( nSubMeshIndex == -1 ) {
		for( int i = 0; i < ( int )m_vecSkinRenderInfo.size(); i++ ) {
			m_vecSkinRenderInfo[ i ].bUVTiling = bUVTiling;
		}
	}
	else {
		m_vecSkinRenderInfo[ nSubMeshIndex ].bUVTiling = bUVTiling;
	}	
}

void CEtSkinInstance::EnableAlphaBlend( bool bEnable, int nSubMeshIndex )
{
	if( nSubMeshIndex == -1 ) 
	{
		for( int i = 0; i < ( int )m_vecSkinRenderInfo.size(); i++ ) 
		{
			m_vecSkinRenderInfo[ i ].bEnableAlphablend = bEnable;
		}
	}
	else 
	{
		m_vecSkinRenderInfo[ nSubMeshIndex ].bEnableAlphablend = bEnable;
	}	
}

void CEtSkinInstance::Render( EtMatrix &WorldMat, EtMatrix &PrevWorldMat, float fObjectAlpha, DNVector(CEtLight *) &vecInfluenceLight, bool bShadowReceive, int nSaveMatIndex )
{
	int i, nCurTechnique;
	SRenderStackElement RenderElement;
	EtCameraHandle hCamera;
	
	std::copy( vecInfluenceLight.begin(), vecInfluenceLight.end(), RenderElement.pInfluenceLight );
	RenderElement.fDirLightAttenuation = m_fDirLightAttenuation;

	hCamera = CEtCamera::GetActiveCamera();
	EtVector3 vCamDir = (*hCamera->GetDirection());
	vCamDir.y = 0.f;
	RenderElement.fDist = EtVec3Dot( &EtVector3(WorldMat._41, 0, WorldMat._43) , &vCamDir);

	if( m_bSkyBox )
	{
		RenderElement.fDist = hCamera->GetCameraFar();
	}

	if( !m_MeshHandle ) return;

	for( i = 0; i < ( int )m_MeshHandle->GetSubMeshCount(); i++ )
	{
		if( i >= (int)m_vecSkinRenderInfo.size()  ) {
			assert( false );
			continue;
		}
		if( m_vecSkinRenderInfo[ i ].bShowSubmesh == false )
		{
			continue;
		}
		float fAlpha;
		fAlpha = fObjectAlpha * m_vecSkinRenderInfo[ i ].fAlphaValue;

		if( ( nSaveMatIndex != -1 ) && ( m_vecSkinRenderInfo[ i ].nTechniqueIndex == 1 ) )
		{
			nCurTechnique = 1;
		}
		else
		{
			nCurTechnique = 0;
		}
		if( ( bShadowReceive ) && ( GetEtOptionController()->IsEnableDynamicShadow() ) && ( nCurTechnique + 2 < m_vecMaterialHandle[ i ]->GetTechniqueCount() ) )
		{
			nCurTechnique += 2;
		}
		RenderElement.nBakeDepthIndex = DT_NORMAL;
		RenderElement.hMaterial = m_vecMaterialHandle[ i ];
		RenderElement.nTechniqueIndex = nCurTechnique;
		RenderElement.WorldMat = WorldMat;
		RenderElement.PrevWorldMat = PrevWorldMat;
		RenderElement.nSaveMatIndex = nSaveMatIndex;
		RenderElement.pvecCustomParam = &m_vecCustomParam[ i ];
		RenderElement.pRenderSubMesh = m_MeshHandle->GetSubMesh( i );
		RenderElement.nDiffuseTexIndex = m_hSkin->GetDiffuseTexIndex( i );		
		RenderElement.renderPriority = m_RenderPriority;

		if( m_bSkyBox )
		{
			RenderElement.nBakeDepthIndex = DT_SKYBOX;
			RenderElement.renderPriority = RP_HIGH;
		}
		else if( !m_hSkin->IsAlphaTexture( i ) ) 
		{
			RenderElement.nBakeDepthIndex = DT_OPAQUE;
		}

		if( RenderElement.nDiffuseTexIndex != -1 ) {
			EtTextureHandle hTexture = CEtResource::GetResource( RenderElement.nDiffuseTexIndex );
			if( hTexture && hTexture->GetTextureType() == ETTEXTURE_VOLUME ) {
				RenderElement.nBakeDepthIndex = DT_VOLUME;
			}
		}

		if( m_bSkipBakeDepth ) {
			RenderElement.nBakeDepthIndex = DT_NONE;
		}

		// #48453 알파가 0.001 이하라면 거의 보이지 않는건데, 이럴 경우엔 Depth를 굽지 않게 해서
		// PointLightDecal 같은거 그릴때 어색하게 되는걸 수정합니다.
		if( fObjectAlpha <= 0.001f )
			RenderElement.nBakeDepthIndex = DT_NONE;

		SStateBlock RenderStateBlock;
		RenderStateBlock.AddRenderState( D3DRS_BLENDOP, m_vecSkinRenderInfo[ i ].BlendOP );
		RenderStateBlock.AddRenderState( D3DRS_SRCBLEND, m_vecSkinRenderInfo[ i ].SrcBlend );
		RenderStateBlock.AddRenderState( D3DRS_DESTBLEND, m_vecSkinRenderInfo[ i ].DestBlend );

		if( m_vecSkinRenderInfo[ i ].bTwoSide  ) 
		{
			RenderStateBlock.AddRenderState( D3DRS_CULLMODE, CULL_NONE );
		}
		if( m_vecSkinRenderInfo[ i ].bUVTiling ) {
			RenderStateBlock.AddSamplerState( D3DSAMP_ADDRESSU, TADDRESS_CLAMP );
			RenderStateBlock.AddSamplerState( D3DSAMP_ADDRESSV, TADDRESS_CLAMP );
		}
		// Hair처럼 테두리를 예쁘게 뽑기위한 객체는 ZWrite 해줘야 한다..
		if( ( fAlpha >= 1.0f ) && ( m_vecSkinRenderInfo[ i ].bEnableAlphablend ) && !m_bSkyBox )
		{
			RenderStateBlock.AddRenderState( D3DRS_ZWRITEENABLE, true );
		}

		RenderElement.nStateBlockIndex = CEtStateBlockManager::GetInstance().CreateStateBlock( &RenderStateBlock );		

		if( m_vecMaterialHandle[ i ]->IsUseBackBuffer() ) 
		{
			
			if( ( m_nAlphaParamIndex >= 0 ) && ( m_nAlphaParamIndex < ( int )m_vecCustomParam[ i ].size() ) )
			{
				m_vecCustomParam[ i ][ m_nAlphaParamIndex ].fFloat4[ 3 ] = 1.0f;
			}
			GetCurRenderStack()->AddUseBackBufferRenderElement( RenderElement );
		}
		else if( ( fAlpha < 1.0f ) || ( m_vecSkinRenderInfo[ i ].bEnableAlphablend ) )
		{
			if( ( m_nAlphaParamIndex >= 0 ) && ( m_nAlphaParamIndex < ( int )m_vecCustomParam[ i ].size() ) )
			{
				m_vecCustomParam[ i ][ m_nAlphaParamIndex ].fFloat4[ 3 ] = fAlpha;
				if( fAlpha <= FLT_EPSILON ) {
					return;
				}
			}
			if( m_bRenderAlphaTwoPass )
			{
//				RenderElement.renderPriority = ( ( int )m_RenderPriority  < ( int )RP_ABOVE_NORMAL ) ? m_RenderPriority : RP_ABOVE_NORMAL;
				RenderElement.fDist -= m_fMeshRadius;

				SStateBlock RenderStateBlockInner;
				// fAlpha가 1보다 크고 알파 켜져 있으면 Hair 처럼 테두리를 예쁘게 뽑기위한 렌더링이다..
				if( fAlpha >= 1.0f )
				{
					RenderStateBlockInner.AddRenderState( D3DRS_ZWRITEENABLE, TRUE );
					RenderStateBlockInner.AddRenderState( D3DRS_ALPHAREF, 254 );
					RenderStateBlockInner.AddRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
					RenderStateBlockInner.AddRenderState( D3DRS_ALPHATESTENABLE, TRUE );
					RenderStateBlockInner.AddRenderState( D3DRS_BLENDOP, m_vecSkinRenderInfo[ i ].BlendOP );
					RenderStateBlockInner.AddRenderState( D3DRS_SRCBLEND, m_vecSkinRenderInfo[ i ].SrcBlend );
					RenderStateBlockInner.AddRenderState( D3DRS_DESTBLEND, m_vecSkinRenderInfo[ i ].DestBlend );
					RenderElement.nStateBlockIndex = CEtStateBlockManager::GetInstance().CreateStateBlock( &RenderStateBlockInner );
					GetCurRenderStack()->AddAlphaRenderElement( RenderElement );

					RenderStateBlockInner.Clear();
					RenderStateBlockInner.AddRenderState( D3DRS_ZWRITEENABLE, FALSE );
					RenderStateBlockInner.AddRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
					RenderStateBlockInner.AddRenderState( D3DRS_ALPHAREF, 0 );
					RenderStateBlockInner.AddRenderState( D3DRS_ZFUNC, D3DCMP_LESS );
					RenderStateBlockInner.AddRenderState( D3DRS_BLENDOP, m_vecSkinRenderInfo[ i ].BlendOP );
					RenderStateBlockInner.AddRenderState( D3DRS_SRCBLEND, m_vecSkinRenderInfo[ i ].SrcBlend );
					RenderStateBlockInner.AddRenderState( D3DRS_DESTBLEND, m_vecSkinRenderInfo[ i ].DestBlend );
					RenderElement.nStateBlockIndex = CEtStateBlockManager::GetInstance().CreateStateBlock( &RenderStateBlockInner );	
					GetCurRenderStack()->AddAlphaRenderElement( RenderElement );
				}
				else
				{
					RenderElement.bIsTwoPassAlpha = true;
					RenderStateBlockInner.AddRenderState( D3DRS_BLENDOP, m_vecSkinRenderInfo[ i ].BlendOP );
					RenderStateBlockInner.AddRenderState( D3DRS_SRCBLEND, m_vecSkinRenderInfo[ i ].SrcBlend );
					RenderStateBlockInner.AddRenderState( D3DRS_DESTBLEND, m_vecSkinRenderInfo[ i ].DestBlend );
					RenderElement.nStateBlockIndex = CEtStateBlockManager::GetInstance().CreateStateBlock( &RenderStateBlockInner );				
					GetCurRenderStack()->AddAlphaRenderElement( RenderElement );
				}
			}
			else 
			{
				RenderElement.fDist -= m_fMeshRadius;

				if( m_bSkyBox )
					GetCurRenderStack()->AddUseSkyBoxAlphaRenderElement( RenderElement );
				else
					GetCurRenderStack()->AddAlphaRenderElement( RenderElement );
			}
		}
		else
		{
			if( ( m_nAlphaParamIndex >= 0 ) && ( m_nAlphaParamIndex < ( int )m_vecCustomParam[ i ].size() ) )
			{
				m_vecCustomParam[ i ][ m_nAlphaParamIndex ].fFloat4[ 3 ] = 1.0f;
			}
			RenderElement.fDist += m_fMeshRadius;
			m_vecRenderUniqueID[ i ] = GetCurRenderStack()->AddNormalRenderElement( RenderElement, m_vecRenderUniqueID[ i ] );
		}
	}
}

void CEtSkinInstance::RenderImmediate( EtMatrix &WorldMat, EtMatrix &PrevWorldMat, float fObjectAlpha, DNVector(CEtLight *) &vecInfluenceLight, bool bShadowReceive, bool bAlphaPass, int nSaveMatIndex )
{
	int i, nCurTechnique;

	if( !m_MeshHandle ) 
	{
		return;
	}
	// 일단 포인트 라이트 안먹게 수정합니다.. by mapping
/*	if( !vecInfluenceLight.empty() )
	{
		CEtLight::SetInfluenceLight( &vecInfluenceLight[ 0 ] );
	}*/
	//CEtLight::SetDirLightAttenuation( m_fDirLightAttenuation );
	CEtLight::SetDirLightAttenuation( 1.0f );
	for( i = 0; i < ( int )m_MeshHandle->GetSubMeshCount(); i++ )
	{
		if( ( i >= ( int )m_vecSkinRenderInfo.size() ) || ( m_vecSkinRenderInfo[ i ].bShowSubmesh == false ) )
		{
			continue;
		}
		float fAlpha = fObjectAlpha * m_vecSkinRenderInfo[ i ].fAlphaValue;

		if( bAlphaPass )
		{
			if( ( fAlpha >= 1.0f ) && ( !m_vecSkinRenderInfo[ i ].bEnableAlphablend ) )
			{
				continue;
			}
			if( fAlpha <= FLT_EPSILON )
			{
				continue;
			}
			if( ( m_nAlphaParamIndex >= 0 ) && ( m_nAlphaParamIndex < ( int )m_vecCustomParam[ i ].size() ) )
			{
				m_vecCustomParam[ i ][ m_nAlphaParamIndex ].fFloat4[ 3 ] = fAlpha;
			}
		}
		else
		{
			if( ( fAlpha < 1.0f ) || ( m_vecSkinRenderInfo[ i ].bEnableAlphablend ) )
			{
				continue;
			}
			if( ( m_nAlphaParamIndex >= 0 ) && ( m_nAlphaParamIndex < ( int )m_vecCustomParam[ i ].size() ) )
			{
				m_vecCustomParam[ i ][ m_nAlphaParamIndex ].fFloat4[ 3 ] = 1.0f;
			}
		}

		if( ( nSaveMatIndex != -1 ) && ( m_vecSkinRenderInfo[ i ].nTechniqueIndex == 1 ) )
		{
			nCurTechnique = 1;
		}
		else
		{
			nCurTechnique = 0;
		}
		if( ( bShadowReceive ) && ( GetEtOptionController()->IsEnableDynamicShadow() ) && ( nCurTechnique + 2 < m_vecMaterialHandle[ i ]->GetTechniqueCount() ) )
		{
			nCurTechnique += 2;
		}

		CEtMaterial *pCurMaterial = m_vecMaterialHandle[ i ].GetPointer();
		CEtSubMesh *pSubmesh = m_MeshHandle->GetSubMesh( i );

		if( ( pCurMaterial == NULL ) || ( pSubmesh == NULL ) )
		{
			continue;
		}
		pCurMaterial->SetTechnique( nCurTechnique );
		int nPasses = 0;
		pCurMaterial->BeginEffect( nPasses );
		pCurMaterial->BeginPass( 0 );
		pCurMaterial->SetGlobalParams();
		if( nSaveMatIndex != -1 )
		{
			pCurMaterial->SetWorldMatArray( &WorldMat, nSaveMatIndex, pSubmesh->GetLinkCount(), ( pSubmesh->GetLinkCount() > 0 ) ? pSubmesh->GetLinkIndex() : NULL );
		}
		pCurMaterial->SetWorldMatParams( &WorldMat, &PrevWorldMat );
		pCurMaterial->SetCustomParamList( m_vecCustomParam[ i ] );
		pCurMaterial->CommitChanges();
		if( ( m_bRenderAlphaTwoPass ) && ( bAlphaPass ) )
		{
			int nRef = EtClamp( (int)( ( 255.f * fAlpha ) - 0.5f ), 0, 255);
			bool bZWrite = GetEtDevice()->EnableZWrite( true );
			DWORD dwAlphaRef = GetEtDevice()->SetAlphaRef( nRef );
			EtBlendOP nBlendOP = GetEtDevice()->SetBlendOP( m_vecSkinRenderInfo[ i ].BlendOP );
			EtBlendMode nSrcBlend = GetEtDevice()->SetSrcBlend( m_vecSkinRenderInfo[ i ].SrcBlend );
			EtBlendMode nDestBlend = GetEtDevice()->SetDestBlend( m_vecSkinRenderInfo[ i ].DestBlend );
			pSubmesh->Draw( pCurMaterial->GetVertexDeclIndex( nCurTechnique, 0 ) );

			GetEtDevice()->EnableZWrite( false );
			GetEtDevice()->SetAlphaRef( 0 );
			EtCmpFunc ZFunc = GetEtDevice()->SetZFunc( CF_LESS );
			pSubmesh->Draw( pCurMaterial->GetVertexDeclIndex( nCurTechnique, 0 ) );

			GetEtDevice()->EnableZWrite( bZWrite );
			GetEtDevice()->SetAlphaRef( dwAlphaRef );
			GetEtDevice()->SetBlendOP( nBlendOP );
			GetEtDevice()->SetSrcBlend( nSrcBlend );
			GetEtDevice()->SetDestBlend( nDestBlend );
			GetEtDevice()->SetZFunc( ZFunc );
		}
		else
		{
			pSubmesh->Draw( pCurMaterial->GetVertexDeclIndex( nCurTechnique, 0 ) );
		}
		pCurMaterial->EndPass();
		pCurMaterial->EndEffect();
	}
}

void CEtSkinInstance::RenderShadow( EtMatrix &WorldMat, int nSaveMatIndex )
{
	int i;
	SRenderStackElement RenderElement;

	RenderElement.hMaterial = m_hShadowMaterial;

	if( !m_MeshHandle ) return;
	for( i = 0; i < ( int )m_MeshHandle->GetSubMeshCount(); i++ )
	{
		if( i >= (int)m_vecSkinRenderInfo.size()  ) {
			assert( false );
			continue;
		}
		if( m_vecSkinRenderInfo[ i ].bShowSubmesh == false )
		{
			continue;
		}
		RenderElement.WorldMat = WorldMat;
		RenderElement.nSaveMatIndex = nSaveMatIndex;
		if( ( nSaveMatIndex != -1 ) && ( m_vecSkinRenderInfo[ i ].nTechniqueIndex == 1 ) )
		{
			RenderElement.nTechniqueIndex = 1;
		}
		else
		{
			RenderElement.nTechniqueIndex = 0;
		}
		if( m_hSkin->GetDiffuseTexIndex( i ) != -1 )
		{
			EtTextureHandle hTexture = CEtResource::GetResource( m_hSkin->GetDiffuseTexIndex( i ) );
			if( hTexture && hTexture->GetTextureType() == ETTEXTURE_VOLUME ) RenderElement.nTechniqueIndex += 4;
		}
		RenderElement.pvecCustomParam = &m_vecShadowParam[ i ];
		RenderElement.pRenderSubMesh = m_MeshHandle->GetSubMesh( i );
		GetCurRenderStack()->AddShadowRenderElement( RenderElement );
	}
}

void CEtSkinInstance::RenderWater( int index, EtMatrix &WorldMat, EtMatrix &PrevWorldMat, float fObjectAlpha, DNVector(CEtLight *) &vecInfluenceLight, bool bShadowReceive, int nSaveMatIndex )
{
	int i, nCurTechnique;
	SRenderStackElement RenderElement;
	EtCameraHandle hCamera;
	EtVector3 vDist;

	if( !m_MeshHandle ) return;
	for( i = 0; i < ( int )m_MeshHandle->GetSubMeshCount(); i++ )
	{
		if( i >= (int)m_vecSkinRenderInfo.size()  ) {
			assert( false );
			continue;
		}

		if( m_vecSkinRenderInfo[ i ].bShowSubmesh == false )
		{
			continue;
		}
		float fAlpha;
		fAlpha = fObjectAlpha * m_vecSkinRenderInfo[ i ].fAlphaValue;

		if( ( fAlpha < 1.0f ) || ( m_vecSkinRenderInfo[ i ].bEnableAlphablend ) )
		{
			if( m_bSkyBox )
			{
				EtMaterialHandle hMaterial = m_hSkin->GetMaterialHandle( i );
				if( ( hMaterial ) && ( 0 == strcmp( hMaterial->GetFileName(), "skyboxcloud.fx" ) ) )
					RenderElement.renderPriority = RP_BELOW_NORMAL;

				RenderElement.bIsAlphaSky = true;
			}
		}

		if( ( nSaveMatIndex != -1 ) && ( m_vecSkinRenderInfo[ i ].nTechniqueIndex == 1 ) )
		{
			nCurTechnique = 1;
		}
		else
		{
			nCurTechnique = 0;
		}

		RenderElement.hMaterial = m_vecMaterialHandle[ i ];
		RenderElement.nTechniqueIndex = nCurTechnique;
		RenderElement.WorldMat = WorldMat;
		RenderElement.PrevWorldMat = PrevWorldMat;
		RenderElement.nSaveMatIndex = nSaveMatIndex;
		RenderElement.pvecCustomParam = &m_vecCustomParam[ i ];
		RenderElement.pRenderSubMesh = m_MeshHandle->GetSubMesh( i );

		SStateBlock RenderStateBlock;
		RenderStateBlock.AddRenderState( D3DRS_BLENDOP, m_vecSkinRenderInfo[ i ].BlendOP );
		RenderStateBlock.AddRenderState( D3DRS_SRCBLEND, m_vecSkinRenderInfo[ i ].SrcBlend );
		RenderStateBlock.AddRenderState( D3DRS_DESTBLEND, m_vecSkinRenderInfo[ i ].DestBlend );

		RenderElement.nStateBlockIndex = CEtStateBlockManager::GetInstance().CreateStateBlock( &RenderStateBlock );

		if( ( m_nAlphaParamIndex >= 0 ) && ( m_nAlphaParamIndex < ( int )m_vecCustomParam[ i ].size() ) )
		{
			m_vecCustomParam[ i ][ m_nAlphaParamIndex ].fFloat4[ 3 ] = 1.0f;
		}

		GetCurRenderStack()->AddWaterRenderElement( index, RenderElement );

	}
}
