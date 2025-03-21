#include "StdAfx.h"
#include "MAFaceAniBase.h"
#include "DnTableDB.h"
#include "MAActorRenderBase.h"
#ifdef PRE_FIX_MEMOPT_EXT
#include "DnCommonUtil.h"
#endif

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

MAFaceAniBase::MAFaceAniBase()
{
	m_LocalTime = 0;
	m_AniFrame = 0;
	m_nCurrentSocialActionIndex = -1;

	m_fFps = s_fDefaultFps;
	m_bUseMaskTexture = false;
}

MAFaceAniBase::~MAFaceAniBase()
{
	FreeFaceAni();
}

void MAFaceAniBase::FreeFaceAni()
{
	for( DWORD i=0; i<m_pVecSocialList.size(); i++ ) {
		for( DWORD j=0; j<m_pVecSocialList[i]->pVecList.size(); j++ ) {
			SAFE_RELEASE_SPTR( m_pVecSocialList[i]->pVecList[j]->hTexture );
			SAFE_RELEASE_SPTR( m_pVecSocialList[i]->pVecList[j]->hMaskTexture );
		}
		SAFE_DELETE_PVEC( m_pVecSocialList[i]->pVecList );
	}
	SAFE_DELETE_PVEC( m_pVecSocialList );
	SAFE_DELETE_VEC( m_szMapSearch );
}

bool MAFaceAniBase::Initialize( int nClassID )
{
	FreeFaceAni();

	DNTableFileFormat* pFaceSox = GetDNTable( CDnTableDB::TFACE );
	DNTableFileFormat* pSocialSox = GetDNTable( CDnTableDB::TSOCIAL );

	if( !pSocialSox->IsExistItem( nClassID ) ) return false;
	m_bUseMaskTexture = ( pSocialSox->GetFieldFromLablePtr( nClassID, "_UseMaskTexture" )->GetInteger() ) ? true : false;
	char szLabel[32];
	SocialStruct *pSocialStruct;
	std::string szName;
	int nFaceAniIndex;

	for( DWORD i=0; i<50; i++ ) {
		sprintf_s( szLabel, "_Name%d", i + 1 );
		szName = pSocialSox->GetFieldFromLablePtr( nClassID, szLabel )->GetString();
		sprintf_s( szLabel, "_FaceID%d", i + 1 );
		nFaceAniIndex = pSocialSox->GetFieldFromLablePtr( nClassID, szLabel )->GetInteger();

		if( szName.empty() || nFaceAniIndex < 1) continue;
		if( !pFaceSox->IsExistItem( nFaceAniIndex ) ) continue;

		pSocialStruct = new SocialStruct;
		pSocialStruct->szName = szName;

		LoadFaceAni( nFaceAniIndex, pSocialStruct );

		m_pVecSocialList.push_back( pSocialStruct );
		m_szMapSearch.insert( make_pair( szName, (int)m_pVecSocialList.size() - 1 ) );
	}
	SetFaceAction( m_szPrevSocialAction.c_str() );

	return true;
}

void MAFaceAniBase::LoadFaceAni( int nTableID, SocialStruct *pSocial )
{
	DNTableFileFormat* pFaceSox = GetDNTable( CDnTableDB::TFACE );
#ifdef PRE_FIX_MEMOPT_EXT
	DNTableFileFormat*  pFileNameSox = GetDNTable( CDnTableDB::TFILE );
	if (!pFileNameSox)
	{
		_ASSERT(0);
		return;
	}
#endif

	int nCount = pFaceSox->GetFieldFromLablePtr( nTableID, "_TotalFrame" )->GetInteger();
	char szLabel[32];
	std::string szTextureName;
	float fFrame;
	float fTotalFrame = 0;
	FrameStruct *pStruct;
	char szFileName[256];
	char szExt[32];
	char szTemp[256];
	for( int i=0; i<nCount; i++ ) {
		sprintf_s( szLabel, "_TextureName%d", i + 1 );
#ifdef PRE_FIX_MEMOPT_EXT
		CommonUtil::GetFileNameFromFileEXT(szTextureName, pFaceSox, nTableID, szLabel, pFileNameSox);
#else
		szTextureName = pFaceSox->GetFieldFromLablePtr( nTableID, szLabel )->GetString();
#endif
		sprintf_s( szLabel, "_Delay%d", i + 1 );
		fFrame = ( pFaceSox->GetFieldFromLablePtr( nTableID, szLabel )->GetInteger() / 1000.f ) * 60.f;
		if( szTextureName.empty() ) continue;

		if( fFrame <= 0.f ) fFrame = 100000.f;
		fTotalFrame += fFrame;

		pStruct = new FrameStruct;
		pStruct->hTexture = EternityEngine::LoadTexture( szTextureName.c_str() );
		/*
#ifdef PRE_FIX_CLIENT_MEMOPTIMIZE
		if( pStruct->hTexture ) pStruct->hTexture->SetDeleteImmediate( true );
#endif
		*/
		if( m_bUseMaskTexture ) {
			_GetFileName( szFileName, _countof(szFileName), szTextureName.c_str() );
			_GetExt( szExt, _countof(szExt), szTextureName.c_str() );
			sprintf_s( szTemp, "%s_AvatarFace.%s", szFileName, szExt );
			pStruct->hMaskTexture = EternityEngine::LoadTexture( szTemp );
			/*
#ifdef PRE_FIX_CLIENT_MEMOPTIMIZE
			if( pStruct->hMaskTexture ) pStruct->hMaskTexture->SetDeleteImmediate( true );
#endif
			*/
		}
		pStruct->fFrame = fTotalFrame;
		pSocial->pVecList.push_back( pStruct );
	}
	pSocial->fTotalFrame = fTotalFrame;
}

void MAFaceAniBase::SetFaceAction( const char *szAniName )
{
	int nIndex = GetSocialIndex( szAniName );
	if( nIndex == -1 ) return;

	SocialStruct *pStruct = m_pVecSocialList[nIndex];
	m_nCurrentSocialActionIndex = nIndex;
	if( !m_szSocialAction.empty() ) m_szPrevSocialAction = m_szSocialAction;
	m_szSocialAction = szAniName;
	m_AniFrame = m_LocalTime;
}

void MAFaceAniBase::Process( LOCAL_TIME LocalTime, float fDelta )
{
	m_LocalTime = LocalTime;
	if( m_nCurrentSocialActionIndex == -1 ) return;
	if( m_nCurrentSocialActionIndex >= (int)m_pVecSocialList.size() ) return;

	SocialStruct *pStruct = m_pVecSocialList[m_nCurrentSocialActionIndex];
	float fFrame = ( ( m_LocalTime - m_AniFrame ) / 1000.f ) * m_fFps;
	if( fFrame >= (float)pStruct->fTotalFrame ) {
		ChangeSocialTexture( m_nCurrentSocialActionIndex, (int)pStruct->pVecList.size() - 1 );
		if( !m_szSocialAction.empty() ) m_szPrevSocialAction = m_szSocialAction;
		m_szSocialAction.clear();
		m_nCurrentSocialActionIndex = -1;
	}
	else {
		int nCurFrame = 0;
		for( nCurFrame=0; nCurFrame < (int)pStruct->pVecList.size(); nCurFrame++ ) {
			if( fFrame < pStruct->pVecList[nCurFrame]->fFrame ) break;
		}
		ChangeSocialTexture( m_nCurrentSocialActionIndex, nCurFrame );
	}
}

bool MAFaceAniBase::IsExistFaceAni( const char *szAniName )
{
	std::map<std::string, int>::iterator it = m_szMapSearch.find( szAniName );
	if( it == m_szMapSearch.end() ) return false;
	return true;
}

int MAFaceAniBase::GetSocialIndex( const char *szAniName )
{
	std::map<std::string, int>::iterator it = m_szMapSearch.find( szAniName );
	if( it == m_szMapSearch.end() ) return -1;

	return it->second;

}
/*
void MAFaceAniBase::ChangeSocialTexture( int nSocialIndex, int nFrameIndex )
{
	if( !m_pRenderBase ) return;

	DnPartsHandle hBodyParts = GetParts( CDnParts::Face );
	EtAniObjectHandle hBodyObject = hBodyParts->GetObjectHandle();

	if( hBodyObject ) {
		int iCustomParam = hBodyObject->AddCustomParam( "g_DiffuseTex" );

		if( NULL == s_aSocialTextureHandle[ SocialEnum ] )
			s_aSocialTextureHandle[ SocialEnum ] = EternityEngine::LoadTexture(s_aSocialTextureName[ SocialEnum ]);

		int iTexture = s_aSocialTextureHandle[ SocialEnum ]->GetMyIndex();
		hBodyObject->SetCustomParam( iCustomParam, &iTexture );
	}
}
*/