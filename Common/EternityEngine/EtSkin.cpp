#include "StdAfx.h"
#include "EtSkin.h"
#include "EtMesh.h"
#include "EtLoader.h"
#include "EtOptionController.h"
#include "EtCustomParam.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CEtSkin::CEtSkin(void)
{
	m_nAlphaParamIndex = -1;
	m_pAdditionalCustomParam = NULL;	

	for( int i = 0;i < 5; i++) {
		m_EmmisiveColor[i] = EtColor(0,0,0,0);
	}
	m_pvecExternalParam = NULL;
	m_pvecMultiExternalParam = NULL;
}

CEtSkin::~CEtSkin(void)
{
	Clear();
}

void CEtSkin::Clear()
{
	// 스킨에 들어 있는 리소스들은 스킨 지워질때 바로 같이 지워져야 한다. WaitDeleteBuffer에 들어갈 필요 없다..
	int i, j;

	SAFE_RELEASE_SPTR( m_MeshHandle );
	// 맵툴 컴파일 시 이부분 주석처리 필요 ( Preview 속도때문에 )
#ifndef _TOOLCOMPILE
	for( i = 0; i < ( int )m_vecMaterialHandle.size(); i++ )
	{
		if( m_vecMaterialHandle[ i ] )
		{
			if( m_vecMaterialHandle[ i ]->Release() <= 0 )
			{
				delete m_vecMaterialHandle[ i ];
			}
		}
	}
#endif
	////////////////////////////////////////////////////////////////
	m_vecMaterialHandle.clear();
	if( m_hShadowMaterial )
	{
		if( m_hShadowMaterial->Release() <= 0 )
		{
			delete m_hShadowMaterial;
		}
		m_hShadowMaterial.Identity();
	}

	for( i = 0; i < ( int )m_vecCustomParam.size(); i++ )
	{
		for( j = 0; j < ( int )m_vecCustomParam[ i ].size(); j++ )
		{
			if( m_vecCustomParam[ i ][ j ].Type == EPT_TEX )
			{
				if( m_vecCustomParam[ i ][ j ].nTextureIndex == -1 ) 
				{
					continue;
				}
				EtResourceHandle hHandle = CEtResource::GetResource( m_vecCustomParam[ i ][ j ].nTextureIndex );
				if( hHandle )
				{
					if( hHandle->Release() <= 0 )
					{
						delete hHandle;
					}
				}
			}
		}
	}

	m_vecCustomParam.clear();
	m_vecSubSkinHeader.clear();
}

int CEtSkin::LoadResource( CStream *pStream )
{
	ASSERT( pStream && "Invalid Resource Stream( Skin )" );
	if( pStream == NULL )
	{
		return ETERR_INVALIDRESOURCESTREAM;
	}

	int i, j, nRet;
	ASSERT( !m_hShadowMaterial );
	ASSERT( !m_MeshHandle );

	Clear();
	m_hShadowMaterial = ::LoadResource( "Shadow.fx", RT_SHADER, true );
	pStream->Read( &m_SkinHeader, sizeof( SSkinFileHeader ) );
	pStream->Seek( SKIN_HEADER_RESERVED, SEEK_CUR );

	if( strstr( m_SkinHeader.szHeaderString, SKIN_FILE_STRING ) == NULL ) return ETERR_FILENOTFOUND;

	nRet = LoadMesh( m_SkinHeader.szMeshName );

	if( nRet != ET_OK )
	{
		ASSERT( false );
		return nRet;
	}	

	m_vecCustomParam.resize( m_SkinHeader.nSubMeshCount );
	m_vecSubSkinHeader.resize( m_SkinHeader.nSubMeshCount );
	m_DiffuseTexIndexList.resize( m_SkinHeader.nSubMeshCount );
	m_IsAlphaTextureList.resize( m_SkinHeader.nSubMeshCount );
	for( i = 0; i < m_SkinHeader.nSubMeshCount; i++ )
	{
		m_DiffuseTexIndexList[ i ] = -1;
		m_IsAlphaTextureList[ i ] = FALSE;
		int nParamCount, nNameLength;
		char szParamName[ 256 ];
		SCustomParam CustomParam;

		pStream->Read( &m_vecSubSkinHeader[ i ], sizeof( SSubSkinHeader ) );
		pStream->Seek( SUB_SKIN_HEADER_RESERVED, SEEK_CUR );
		nRet = LoadEffect( m_vecSubSkinHeader[ i ].szEffectName );
		if( nRet != ET_OK )
		{
			return nRet;
		}

		pStream->Read( &nParamCount, sizeof( int ) );
		for( j = 0; j < nParamCount; j++ )
		{
			CustomParam.nVariableCount = 1;
			pStream->Read( &nNameLength, sizeof( int ) );
			pStream->Read( szParamName, nNameLength );
			CustomParam.hParamHandle = m_vecMaterialHandle[ i ]->GetParameterByName( szParamName );
			pStream->Read( &CustomParam.Type, sizeof( EffectParamType ) );
			if( stricmp( szParamName, "g_MaterialAmbient" ) == 0 )
			{
				m_nAlphaParamIndex = ( int )m_vecCustomParam[ i ].size();
			}
			switch( CustomParam.Type )
			{
			case EPT_INT:
				pStream->Read( &CustomParam.nInt, sizeof( int ) );
				break;
			case EPT_FLOAT:
				pStream->Read( &CustomParam.fFloat, sizeof( float ) );
				break;
			case EPT_VECTOR:
				pStream->Read( &CustomParam.fFloat4, sizeof( float ) * 4 );
				break;
			case EPT_TEX:
				{
					char szTexName[ 256 ];
					pStream->Read( &nNameLength, sizeof( int ) );
					pStream->Read( szTexName, nNameLength );
					CustomParam.nTextureIndex = LoadTexture( szTexName, m_vecMaterialHandle[ i ]->GetTextureType( CustomParam.hParamHandle ) );
					if( stricmp( szParamName, "g_DiffuseTex" ) == 0 || stricmp( szParamName, "g_DiffuseVolumeTex" ) == 0 )
					{
						m_DiffuseTexIndexList[ i ] = CustomParam.nTextureIndex;
						if( m_DiffuseTexIndexList[ i ] == -1 ) break;

						EtTextureHandle hTexture = CEtResource::GetResource( m_DiffuseTexIndexList[ i ] );
						if( hTexture->IsAlphaTexture() ) {	
							m_IsAlphaTextureList[ i ] = TRUE;
						}
					}
				}
				break;
			}
			m_vecCustomParam[ i ].push_back( CustomParam );
		}
	}

	CheckTechnique();
	CheckMeshStream();

	return ET_OK;
}

int CEtSkin::Save( const char *pFileName )
{
	CFileStream Stream( pFileName, CFileStream::OPEN_WRITE );

	if( !Stream.IsValid() )
	{
		return ETERR_FILECREATEFAIL;
	}

	SaveSkin( &Stream );

	return ET_OK;
}

int CEtSkin::SaveSkin( CStream *pStream )
{
	char cReserved[ 1024 ];
	int i, j;

	pStream->Write( &m_SkinHeader, sizeof( SSkinFileHeader ) );
	memset( cReserved, 0, 1024 );
	pStream->Write( cReserved, SKIN_HEADER_RESERVED );

	for( i = 0; i < m_SkinHeader.nSubMeshCount; i++ )
	{
		char szParamName[ 256 ];
		int nParamCount, nNameLength;
		SCustomParam CustomParam;

		pStream->Write( &m_vecSubSkinHeader[ i ], sizeof( SSubSkinHeader ) );
		pStream->Write( cReserved, SUB_SKIN_HEADER_RESERVED );
		nParamCount = ( int )m_vecCustomParam[ i ].size();
		pStream->Write( &nParamCount, sizeof( int ) );
		for( j = 0; j < nParamCount; j++ )
		{
			CustomParam = m_vecCustomParam[ i ][ j ];
			strcpy( szParamName, m_vecMaterialHandle[ i ]->GetParameterName( CustomParam.hParamHandle ) );
			nNameLength = ( int )strlen( szParamName ) + 1;
			pStream->Write( &nNameLength, sizeof( int ) );
			pStream->Write( szParamName, nNameLength );
			pStream->Write( &CustomParam.Type, sizeof( EffectParamType ) );
			switch( CustomParam.Type )
			{
			case EPT_INT:
				pStream->Write( &CustomParam.nInt, sizeof( int ) );
				break;
			case EPT_FLOAT:
				pStream->Write( &CustomParam.fFloat, sizeof( float ) );
				break;
			case EPT_VECTOR:
				pStream->Write( &CustomParam.fFloat4, sizeof( float ) * 4 );
				break;
			case EPT_TEX:
				if( CustomParam.nTextureIndex == -1 ) {					
					//MessageBox(0, "텍스쳐가 빠진것이 있으니 확인 바래요.", "경고", MB_OK);
					MessageBox(0, " Missing textures found ", "Warning", MB_OK);
				}
				else {
					strcpy( szParamName, CEtResource::GetResource( CustomParam.nTextureIndex )->GetFileName() );			
				}
				nNameLength = ( int )strlen( szParamName ) + 1;
				pStream->Write( &nNameLength, sizeof( int ) );
				pStream->Write( szParamName, nNameLength );
				break;
			}
		}
	}

	return ET_OK;
}

int CEtSkin::LoadMesh( const char *pMeshName )
{
	m_MeshHandle = ::LoadResource( pMeshName, RT_MESH, true );
	if( !m_MeshHandle )
	{		
		ASSERT( 0 && "Mesh File Not Found!!!" );
		return ETERR_MESHNOTFOUND;
	}

	if( m_MeshHandle->GetSubMeshCount() != m_SkinHeader.nSubMeshCount )
	{
		ASSERT( 0 && "스킨과 메쉬파일의 서브메시 갯수가 다릅니다. 확인해주세요!!!" );
		return -1;
	}

	return ET_OK;
}

int CEtSkin::LoadEffect( const char *pEffectName )
{
	EtResourceHandle hHandle;	

	hHandle = ::LoadResource( pEffectName, RT_SHADER, true );
	if( !hHandle )
	{
		ASSERT( 0 && "fx File Not Found!!!" );
		return ETERR_EFFECTNOTFOUND;
	}
	m_vecMaterialHandle.push_back( hHandle );

	return ET_OK;
}

int CEtSkin::LoadTexture( const char *pTextureName, EtTextureType Type )
{
	EtResourceHandle hHandle;

	hHandle = ::LoadResource( pTextureName, RT_TEXTURE, true );
	if( !hHandle )
	{
		ASSERT( 0 && "Texture File Not Found!!!" );
		return -1;
	}

	return hHandle->GetMyIndex();
}

void CEtSkin::CheckTechnique()
{
	int i;

	m_vecTehiniqueIndex.resize( m_SkinHeader.nSubMeshCount );
	for( i = 0; i < m_SkinHeader.nSubMeshCount; i++ )
	{
		if( m_MeshHandle->GetSubMesh( i )->IsExistStream( MST_BONEINDEX, 0 ) )
		{
			m_vecTehiniqueIndex[ i ] = 1;
		}
		else
		{
			m_vecTehiniqueIndex[ i ] = 0;
		}
	}
}

void CEtSkin::CheckMeshStream()
{
	int i;

	for( i = 0; i < m_SkinHeader.nSubMeshCount; i++ )
	{
		if( m_vecMaterialHandle[ i ]->UseTangentSpace() )
		{
			m_MeshHandle->GetSubMesh( i )->GenerateTangentSpace();
		}
	}
}

EtMaterialHandle CEtSkin::GetCurrentMaterial( int nMtlIndex )
{
	if( m_hExternalMaterial ) {
		return m_hExternalMaterial;
	}
	else {
		return m_vecMaterialHandle[ nMtlIndex ];
	}
}

void CEtSkin::SetExternalMaterial( EtMaterialHandle hMaterial, std::vector< SCustomParam > *pvecParam, std::vector< std::vector< SCustomParam > > *pvecMultiParam )
{
	m_hExternalMaterial = hMaterial;
	m_pvecExternalParam = pvecParam;
	m_pvecMultiExternalParam = pvecMultiParam;
}

void CEtSkin::ClearExternalMaterial()
{
	m_hExternalMaterial.Identity();
	m_pvecExternalParam = NULL;
	m_pvecMultiExternalParam = NULL;
}

void CEtSkin::SetEffect( int nSubMeshIndex, int nEffectIndex, SCustomParam *pParam )
{
	ASSERT( m_vecCustomParam[ nSubMeshIndex ][ nEffectIndex ].hParamHandle == pParam->hParamHandle );
	ASSERT( m_vecCustomParam[ nSubMeshIndex ][ nEffectIndex ].Type == pParam->Type );

	switch( m_vecCustomParam[ nSubMeshIndex ][ nEffectIndex ].Type ) 
	{
	case EPT_INT:
		m_vecCustomParam[ nSubMeshIndex ][ nEffectIndex ].nInt = pParam->nInt;
		break;
	case EPT_FLOAT:
		m_vecCustomParam[ nSubMeshIndex ][ nEffectIndex ].fFloat = pParam->fFloat;
		break;
	case EPT_VECTOR:
		memcpy( m_vecCustomParam[ nSubMeshIndex][ nEffectIndex ].fFloat4, pParam->fFloat4, sizeof( pParam->fFloat4 ) );
		break;
	}
}

void CEtSkin::SetTexture( int nSubMeshIndex, int nEffectIndex, const char *pTexName )
{
	ASSERT( nSubMeshIndex < ( int )m_vecCustomParam.size() );

	if( m_vecCustomParam[ nSubMeshIndex ][ nEffectIndex ].Type != EPT_TEX ) 
	{
		return;
	}
	if( m_vecCustomParam[ nSubMeshIndex ][ nEffectIndex ].nTextureIndex != -1 ) 
	{
		EtTextureHandle hTexture;
		hTexture = CEtResource::GetResource( m_vecCustomParam[ nSubMeshIndex ][ nEffectIndex ].nTextureIndex );
		SAFE_RELEASE_SPTR( hTexture );
	}
	m_vecCustomParam[ nSubMeshIndex ][ nEffectIndex ].nTextureIndex = LoadTexture( pTexName, 
		m_vecMaterialHandle[ nSubMeshIndex ]->GetTextureType( m_vecCustomParam[ nSubMeshIndex ][ nEffectIndex ].hParamHandle ) );
}

void CEtSkin::SetAlphaValue( int nSubMeshIndex, float fAlphaValue )
{
	ASSERT( nSubMeshIndex < ( int )m_vecCustomParam.size() );

	m_vecSubSkinHeader[ nSubMeshIndex ].fAlphaValue = fAlphaValue;
}

float CEtSkin::GetAlphaValue( int nSubMeshIndex )
{
	ASSERT( nSubMeshIndex < ( int )m_vecCustomParam.size() );

	return m_vecSubSkinHeader[ nSubMeshIndex ].fAlphaValue;
}

void CEtSkin::EnableAlphaBlend( int nSubMeshIndex, bool bEnable )
{
	ASSERT( nSubMeshIndex < ( int )m_vecCustomParam.size() );

	m_vecSubSkinHeader[ nSubMeshIndex ].bEnableAlphablend = bEnable;
}

bool CEtSkin::IsEnableAlphaBlend( int nSubMeshIndex )
{
	ASSERT( nSubMeshIndex < ( int )m_vecCustomParam.size() );

	return m_vecSubSkinHeader[ nSubMeshIndex ].bEnableAlphablend;
}

void CEtSkin::RenderSubMesh( int nSubMeshIndex, EtMatrix &WorldMat, EtMatrix &PrevWorldMat, bool bShadowReceive, int nSaveMatIndex, float fAlpha )
{
	int nCurTechnique, nParentBoneIndex;
	EtMaterialHandle hMaterial;

	hMaterial = GetCurrentMaterial( nSubMeshIndex );
	if( ( nSaveMatIndex != -1 ) && ( m_vecTehiniqueIndex[ nSubMeshIndex ] == 1 ) )
	{
		nCurTechnique = 1;
	}
	else
	{
		nCurTechnique = 0;
	}
	if( m_nAlphaParamIndex != -1 )
	{
		m_vecCustomParam[ nSubMeshIndex ][ m_nAlphaParamIndex ].fFloat4[ 3 ] = fAlpha;
	}

	hMaterial->SetTechnique( nCurTechnique );
	hMaterial->SetGlobalParams();

	nParentBoneIndex = m_MeshHandle->GetSubMesh( nSubMeshIndex )->GetParentBoneIndex();
	if( ( nParentBoneIndex != -1 ) && ( nSaveMatIndex != -1 ) )
	{
		hMaterial->SetWorldMatParams( &WorldMat, &PrevWorldMat, nSaveMatIndex + nParentBoneIndex );
	}
	else
	{
		hMaterial->SetWorldMatParams( &WorldMat, &PrevWorldMat );
	}
	if( ( bShadowReceive ) && ( GetEtOptionController()->IsEnableDynamicShadow() ) )
	{
		nCurTechnique += 2;
		if( nCurTechnique >= hMaterial->GetTechniqueCount() )
		{
			nCurTechnique -= 2;
		}
	}
	if( nCurTechnique % 2 == 1 )
	{
		// 중복셋팅인지 확인 요망 by mapping
		if( m_MeshHandle->GetSubMesh( nSubMeshIndex )->GetLinkCount() ) 
		{
			hMaterial->SetWorldMatArray( &WorldMat, nSaveMatIndex, 
				m_MeshHandle->GetSubMesh( nSubMeshIndex )->GetLinkCount(), m_MeshHandle->GetSubMesh( nSubMeshIndex )->GetLinkIndex() );
		}
	}

	SetCustomParams( nSubMeshIndex );	

	int i, nPasses;
	hMaterial->BeginEffect( nPasses );
	for( i = 0; i < nPasses; i++ )
	{
		hMaterial->BeginPass( i );
		hMaterial->CommitChanges();
		m_MeshHandle->Draw( nSubMeshIndex, hMaterial->GetVertexDeclIndex( nCurTechnique, i ) );
		hMaterial->EndPass();
	}
	hMaterial->EndEffect();
}


void CEtSkin::Render( EtMatrix &WorldMat, EtMatrix &PrevWorldMat, float fObjectAlpha, bool bShadowReceive, std::vector< bool > *pvecShowSubmesh, CEtCustomParam *pCustomParam, int nSaveMatIndex )
{
	if( !m_MeshHandle )
	{
		return;
	}

	int i;

	m_pAdditionalCustomParam = pCustomParam;
	for( i = 0; i < ( int )m_MeshHandle->GetSubMeshCount(); i++ )
	{
		if( ( pvecShowSubmesh ) && ( ( *pvecShowSubmesh )[ i ] == false ) )
		{
			continue;
		}
		float fAlpha;
		fAlpha = fObjectAlpha * m_vecSubSkinHeader[ i ].fAlphaValue;
		if( ( fAlpha < 1.0f ) || ( m_vecSubSkinHeader[ i ].bEnableAlphablend ) )
		{
			continue;
		}
		RenderSubMesh( i, WorldMat, PrevWorldMat, bShadowReceive, nSaveMatIndex, 1.0f );
	}
	m_pAdditionalCustomParam = NULL;
}

void CEtSkin::RenderAlpha( EtMatrix &WorldMat, EtMatrix &PrevWorldMat, float fObjectAlpha, bool bShadowReceive, std::vector< bool > *pvecShowSubmesh, CEtCustomParam *pCustomParam, int nSaveMatIndex )
{
	if( !m_MeshHandle )
	{
		return;
	}

	int i;

	m_pAdditionalCustomParam = pCustomParam;
	for( i = 0; i < ( int )m_MeshHandle->GetSubMeshCount(); i++ )
	{
		if( ( pvecShowSubmesh ) && ( ( *pvecShowSubmesh )[ i ] == false ) )
		{
			continue;
		}
		float fAlpha;
		fAlpha = fObjectAlpha * m_vecSubSkinHeader[ i ].fAlphaValue;
		if( ( fAlpha >= 1.0f ) && ( m_vecSubSkinHeader[ i ].bEnableAlphablend == false ) )
		{
			continue;
		}
		RenderSubMesh( i, WorldMat, PrevWorldMat, bShadowReceive, nSaveMatIndex, fAlpha );
	}
	m_pAdditionalCustomParam = NULL;
}

void CEtSkin::RenderShadow( EtMatrix &WorldMat, EtMatrix &PrevWorldMat, std::vector< bool > *pvecShowSubmesh, int nSaveMatIndex )
{
	if( !m_MeshHandle )
	{
		return;
	}

	SetExternalMaterial( m_hShadowMaterial, NULL );
	int i;
	for( i = 0; i < ( int )m_MeshHandle->GetSubMeshCount(); i++ )
	{
		if( ( pvecShowSubmesh ) && ( ( *pvecShowSubmesh )[ i ] == false ) )
		{
			continue;
		}
		RenderSubMesh( i, WorldMat, PrevWorldMat, false, nSaveMatIndex, 1.0f );
	}
	ClearExternalMaterial();
}

void CEtSkin::SetCustomParams( int nMaterialIndex )
{
	if( m_hExternalMaterial )
	{
		if( m_pvecMultiExternalParam ) 
		{
			m_hExternalMaterial->SetCustomParamList( (*m_pvecMultiExternalParam)[nMaterialIndex] );
		}
		else if( m_pvecExternalParam )
		{
			m_hExternalMaterial->SetCustomParamList( *m_pvecExternalParam );
		}
	}
	else
	{
		m_vecMaterialHandle[ nMaterialIndex ]->SetCustomParamList( m_vecCustomParam[ nMaterialIndex ] );
		if( m_pAdditionalCustomParam )
		{
			m_vecMaterialHandle[ nMaterialIndex ]->SetCustomParamList( m_pAdditionalCustomParam->GetCustomParamList( nMaterialIndex ) );
		}
	}
}