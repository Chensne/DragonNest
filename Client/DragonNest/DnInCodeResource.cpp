#include "stdAfx.h"
#include "DnInCodeResource.h"
#include "DnTableDB.h"
#include "DNTableFile.h"
#include "DnEtcObject.h"
#ifdef PRE_FIX_MEMOPT_EXT
#include "DnCommonUtil.h"
#endif
#ifdef PRE_ADD_CASH_COSTUME_AURA
#include "DnItem.h"
#endif

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 
CDnInCodeResource::CDnInCodeResource()
{
}

CDnInCodeResource::~CDnInCodeResource()
{
	Finalize();
}

void CDnInCodeResource::Initialize()
{
	Finalize();

	if (CDnTableDB::IsActive())
		m_szPlayerCommonEffectName = CDnTableDB::GetInstance().GetFileName( 2 );
	m_hPlayerCommonEffect = (new CDnEtcObject(false))->GetMySmartPtr();
	m_hPlayerCommonEffect->Initialize( NULL, NULL, m_szPlayerCommonEffectName.c_str() );

	DNTableFileFormat* pActorSox = GetDNTable( CDnTableDB::TACTOR );
#ifdef PRE_FIX_MEMOPT_EXT
	DNTableFileFormat* pFileNameSox = GetDNTable( CDnTableDB::TFILE );
	if (pFileNameSox == NULL)
	{
		_ASSERT(0);
		return;
	}
#endif
	for( int i=0; i<10; i++ ) {
		int nItemID = 500 + i;
		if( !pActorSox->IsExistItem( nItemID ) ) continue;
#ifdef PRE_FIX_MEMOPT_EXT
		std::string szSkinName, szAniName, szActName;
		CommonUtil::GetFileNameFromFileEXT(szSkinName, pActorSox, nItemID, "_SkinName");
		CommonUtil::GetFileNameFromFileEXT(szAniName, pActorSox, nItemID, "_AniName");
		CommonUtil::GetFileNameFromFileEXT(szActName, pActorSox, nItemID, "_ActName");
#else
		std::string szSkinName = pActorSox->GetFieldFromLablePtr( nItemID, "_SkinName" )->GetString();
		std::string szAniName = pActorSox->GetFieldFromLablePtr( nItemID, "_AniName" )->GetString();
		std::string szActName = pActorSox->GetFieldFromLablePtr( nItemID, "_ActName" )->GetString();
#endif
		m_hGhostCommon[i] = (new CDnEtcObject(false))->GetMySmartPtr();
		if( !m_hGhostCommon[i]->Initialize( szSkinName.c_str(), szAniName.c_str(), szActName.c_str() ) ) {
			SAFE_RELEASE_SPTR( m_hGhostCommon[i] );
		}
	}


	for( int i = 0; i < EtcIndex_Amount; ++i ) {
		std::string szFileName = CDnTableDB::GetInstance().GetFileName( 21+i );	// 21번부터 시작.
		if( !szFileName.empty() ) {
			m_hHeadEffect[i] = (new CDnEtcObject(false))->GetMySmartPtr();
			std::string szSkinName = szFileName + ".skn";
			std::string szAniName = szFileName + ".ani";
			std::string szActName = szFileName + ".act";
			if( !m_hHeadEffect[i]->Initialize( szSkinName.c_str(), szAniName.c_str(), szActName.c_str() ) ) {
				SAFE_RELEASE_SPTR( m_hHeadEffect[i] );
			}
		}
	}

	m_hNullWhiteTexture = EternityEngine::LoadTexture( "NullWhite.dds" );
}

void CDnInCodeResource::Finalize()
{
	SAFE_RELEASE_SPTR( m_hPlayerCommonEffect );
	for( int i=0; i<10; i++ ) {
		SAFE_RELEASE_SPTR( m_hGhostCommon[i] );
	}
	for( int i=0; i<EtcIndex_Amount; i++ ) {
		SAFE_RELEASE_SPTR( m_hHeadEffect[i] );
	}
	SAFE_RELEASE_SPTR( m_hNullWhiteTexture );
}


DnEtcHandle CDnInCodeResource::CreatePlayerCommonEffect()
{
	DnEtcHandle hHandle = (new CDnEtcObject)->GetMySmartPtr();
	hHandle->Initialize( NULL, NULL, m_szPlayerCommonEffectName.c_str() );
	return hHandle;
}

DnEtcHandle CDnInCodeResource::CreateHeadEffect( eHeadEffectEtcIndex eType )
{
	DnEtcHandle hHandle = (new CDnEtcObject)->GetMySmartPtr();

	std::string szFileName = CDnTableDB::GetInstance().GetFileName( 21+eType );	// 21번부터 시작.
	if( !szFileName.empty() ) {
		std::string szSkinName = szFileName + ".skn";
		std::string szAniName = szFileName + ".ani";
		std::string szActName = szFileName + ".act";
		if( !hHandle->Initialize( szSkinName.c_str(), szAniName.c_str(), szActName.c_str() ) ) {
			SAFE_RELEASE_SPTR( hHandle );
		}
	}
	else {
		_ASSERT(0&&"존재하지않는 헤드이펙트의 생성을 요청했습니다.");
		SAFE_RELEASE_SPTR( hHandle );
	}
	return hHandle;
}

#ifdef PRE_ADD_CASH_COSTUME_AURA
DnEtcHandle CDnInCodeResource::CreateCostumeAura(int nItemID)
{
	DnEtcHandle hHandle = (new CDnEtcObject)->GetMySmartPtr();

	DNTableFileFormat* pSox;
	switch(CDnItem::GetItemType(nItemID))
	{
	case ITEMTYPE_WEAPON:
		{
			pSox = GetDNTable( CDnTableDB::TWEAPON );
		}
		break;

	case ITEMTYPE_PARTS:
		{
			pSox = GetDNTable( CDnTableDB::TPARTS );
		}
		break;
	}
	
	if(!pSox)
	{
		_ASSERT(0 && "Table Load Fail");
		SAFE_RELEASE_SPTR( hHandle );
		return hHandle;
	}

	std::string szEffectActName;
	int nActName = pSox->GetFieldFromLablePtr(nItemID, "_SetEffect_ActName")->GetInteger();
	if(nActName)
	{
		szEffectActName = CDnTableDB::GetInstance().GetFileName( nActName );
	}

	if( !szEffectActName.empty() )
	{
		if( !hHandle->Initialize( NULL, NULL, szEffectActName.c_str()) )
			SAFE_RELEASE_SPTR( hHandle );
	}
	else
	{
		_ASSERT(0 && "존재하지 않는 코스튬오오라 이펙트 생성을 요청했습니다.");
		SAFE_RELEASE_SPTR( hHandle );
	}

	return hHandle;
}
#endif