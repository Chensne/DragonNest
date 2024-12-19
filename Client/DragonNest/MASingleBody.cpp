#include "StdAfx.h"
#include "MASingleBody.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

#define BLANK_TEXTURE "BlankTexture"

MASingleBody::MASingleBody()
{
}

MASingleBody::~MASingleBody()
{
	FreeSkin();
}

bool MASingleBody::LoadSkin( const char *szSkinName, const char *szAniName, bool bTwoPassAlpha )
{
	m_szSkinFileName = szSkinName;
	m_szAniFileName = szAniName;

	if( m_hObject ) return true;
	m_hObject = EternityEngine::CreateAniObject( szSkinName, szAniName );
	if( !m_hObject ) return false;
	m_hObject->EnableShadowCast( true );
	m_hObject->EnableShadowReceive( false );
	m_hObject->GetSkinInstance()->SetRenderAlphaTwoPass( bTwoPassAlpha );

	m_vecFaceSubMeshIndex.clear();
	std::string szSubMesh;
	for( int i=0; i<m_hObject->GetSubMeshCount(); i++ ) {
		szSubMesh = m_hObject->GetSubMeshName(i);
		ToLowerA( szSubMesh );
		if( strstr( szSubMesh.c_str(), "_head" ) || strstr( szSubMesh.c_str(), "_face" ) ) {
			m_vecFaceSubMeshIndex.push_back( i );
		}
	}

	return true;
}

void MASingleBody::FreeSkin()
{
	// 초기에 설정된 대로 에미시브 텍스처를 돌려놓는다. 이렇게 하고 삭제하면 바꿨던 에미시브 다 삭제 가능.
	if( !m_szCurrentEmissiveTexture.empty() )
	{
		int nIndex = m_hObject->AddCustomParam( "g_EmissiveTex" );
		m_hObject->RestoreCustomParam( nIndex );
		if( m_szCurrentEmissiveTexture != BLANK_TEXTURE ) SAFE_RELEASE_SPTR( m_hCurrentEmissiveTexture );
		m_szCurrentEmissiveTexture = "";
	}

	SAFE_RELEASE_SPTR( m_hObject );
}

bool MASingleBody::ChangeSocialTexture( int nSocialIndex, int nFrameIndex )
{
	if( !m_hObject || m_vecFaceSubMeshIndex.empty() ) return false;

	bool bResult[2] = { false, };
	EtTextureHandle hTexture = m_pVecSocialList[nSocialIndex]->pVecList[nFrameIndex]->hTexture;
	int nTextureIndex = -1;
	if( hTexture ) {
		nTextureIndex = hTexture.GetIndex();
		int nCustomParam = m_hObject->AddCustomParam( "g_DiffuseTex" );
		if( nCustomParam != -1 ) {
			for( int i = 0; i < (int)m_vecFaceSubMeshIndex.size(); ++i )
				bResult[0] = m_hObject->SetCustomParam( nCustomParam, &nTextureIndex, m_vecFaceSubMeshIndex[i] );
		}
	}

	hTexture = m_pVecSocialList[nSocialIndex]->pVecList[nFrameIndex]->hMaskTexture;
	nTextureIndex = -1;
	if( hTexture ) {
		nTextureIndex = hTexture.GetIndex();
		int nCustomParam = m_hObject->AddCustomParam( "g_MaskTex" );
		if( nCustomParam != -1 ) {
			for( int i = 0; i < (int)m_vecFaceSubMeshIndex.size(); ++i )
				bResult[1] = m_hObject->SetCustomParam( nCustomParam, &nTextureIndex, m_vecFaceSubMeshIndex[i] );
		}
	}

	return ( bResult[0] ) ? true : false;	// 싱글바디는 기본적으로 MaskTex 안썼으니 0번만 검사.
}

bool MASingleBody::ChangeEmissiveTexture( std::string &szTextureName )
{
	if( !m_hObject ) return false;
	EtSkinHandle hSkin = m_hObject->GetSkin();
	if( !hSkin ) return false;
	bool bUseEmissiveTexture = false;
	int nSubMeshCount = hSkin->GetMaterialCount();
	for( int i = 0; i < nSubMeshCount; i++ )
	{
		EtMaterialHandle hMaterial = hSkin->GetMaterialHandle( i );
		if( ( hMaterial ) && ( strstr( hMaterial->GetFileName(), "emissive" ) ) )
		{
			bUseEmissiveTexture = true;
			break;
		}
	}
	if( !bUseEmissiveTexture ) return false;

	int nIndex = m_hObject->AddCustomParam( "g_EmissiveTex" );
	if( nIndex == -1 ) return false;

	if( szTextureName.empty() ) szTextureName = BLANK_TEXTURE;
	if( m_szCurrentEmissiveTexture == szTextureName ) return false;

	EtTextureHandle hTexture;
	if( szTextureName == BLANK_TEXTURE ) hTexture = CEtTexture::GetBlankTexture();
	else hTexture = EternityEngine::LoadTexture( szTextureName.c_str() );
	if( !hTexture ) return false;
	int nTextureIndex = hTexture.GetIndex();
	if( m_szCurrentEmissiveTexture != BLANK_TEXTURE ) SAFE_RELEASE_SPTR( m_hCurrentEmissiveTexture );
	m_szCurrentEmissiveTexture = szTextureName;
	m_hCurrentEmissiveTexture = hTexture;
	bool bResult = m_hObject->SetCustomParam( nIndex, &nTextureIndex );
	return bResult;
}

bool MASingleBody::GetEmissiveConstants( float &fEmissivePower, float &fEmissivePowerRange, float &fEmissiveAniSpeed )
{
	if( !m_hObject ) return false;
	EtSkinHandle hSkin = m_hObject->GetSkin();
	if( !hSkin ) return false;
	int nSubMeshCount = hSkin->GetMaterialCount();
	for( int i = 0; i < nSubMeshCount; i++ )
	{
		EtMaterialHandle hMaterial = hSkin->GetMaterialHandle( i );
		if( ( hMaterial ) && ( strstr( hMaterial->GetFileName(), "emissive" ) ) )
		{
			int nIndex = 0;
			int nParamCount = hSkin->GetEffectCount( i );
			SCustomParam *pParam = NULL;
			EtParameterHandle hParam1, hParam2, hParam3;
			hParam1 = hMaterial->GetParameterByName( "g_EmissivePower" );
			hParam2 = hMaterial->GetParameterByName( "g_EmissivePowerRange" );
			hParam3 = hMaterial->GetParameterByName( "g_EmissiveAniSpeed" );
			for( int j = 0; j < nParamCount; j++ )
			{
				pParam = m_hObject->GetCustomParam( i, j );
				if( pParam->hParamHandle == hParam1 )
					fEmissivePower = pParam->fFloat;
				if( pParam->hParamHandle == hParam2 )
					fEmissivePowerRange = pParam->fFloat;
				if( pParam->hParamHandle == hParam3 )
					fEmissiveAniSpeed = pParam->fFloat;
			}
			return true;
		}
	}
	return false;
}

bool MASingleBody::SetEmissiveConstants( float fEmissivePower, float fEmissivePowerRange, float fEmissiveAniSpeed )
{
	if( !m_hObject ) return false;
	EtSkinHandle hSkin = m_hObject->GetSkin();
	if( !hSkin ) return false;

	int nCustomParam = m_hObject->AddCustomParam( "g_EmissivePower" );
	if( nCustomParam != -1 ) m_hObject->SetCustomParam( nCustomParam, &fEmissivePower );
	nCustomParam = m_hObject->AddCustomParam( "g_EmissivePowerRange" );
	if( nCustomParam != -1 ) m_hObject->SetCustomParam( nCustomParam, &fEmissivePowerRange );
	nCustomParam = m_hObject->AddCustomParam( "g_EmissiveAniSpeed" );
	if( nCustomParam != -1 ) m_hObject->SetCustomParam( nCustomParam, &fEmissiveAniSpeed );
	return true;
}