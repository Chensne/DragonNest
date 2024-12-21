#include "StdAfx.h"
#include "DnNormalMonsterActor.h"
#include "DnTableDB.h"
#include "DnCommonUtil.h"
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


CDnNormalMonsterActor::CDnNormalMonsterActor( int nClassID, bool bProcess )
: TDnMonsterActor( nClassID, bProcess )
{

}

CDnNormalMonsterActor::~CDnNormalMonsterActor()
{
}

void CDnNormalMonsterActor::Process( LOCAL_TIME LocalTime, float fDelta )
{
	TDnMonsterActor< MASingleBody, MASingleDamage >::Process( LocalTime, fDelta );
}

void CDnNormalMonsterActor::OnDamage( CDnDamageBase *pHitter, SHitParam &HitParam )
{
	TDnMonsterActor< MASingleBody, MASingleDamage >::OnDamage( pHitter, HitParam );
}

void CDnNormalMonsterActor::SetPosition( EtVector3 &vPos )
{
	TDnMonsterActor< MASingleBody, MASingleDamage >::SetPosition( vPos );
	m_pMovement->ValidateCurCell();
}

#ifdef PRE_ADD_TRANSFORM_MONSTER_ACTOR
void CDnNormalMonsterActor::SwapActor( int nActorIndex )
{
	if( m_nSwapActorID == nActorIndex ) return;
	m_nSwapActorID = nActorIndex;

	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TACTOR );

#ifdef PRE_FIX_MEMOPT_EXT
	DNTableFileFormat*  pFileNameSox = GetDNTable( CDnTableDB::TFILE );
	std::string szSkinName, szAniName, szActName;
	CommonUtil::GetFileNameFromFileEXT(szSkinName, pSox, m_nSwapActorID, "_SkinName", pFileNameSox);
	CommonUtil::GetFileNameFromFileEXT(szAniName, pSox, m_nSwapActorID, "_AniName", pFileNameSox);
	CommonUtil::GetFileNameFromFileEXT(szActName, pSox, m_nSwapActorID, "_ActName", pFileNameSox);
#else
	std::string szSkinName = pSox->GetFieldFromLablePtr( m_nSwapActorID, "_SkinName" )->GetString();
	std::string szAniName = pSox->GetFieldFromLablePtr( m_nSwapActorID, "_AniName" )->GetString();
	std::string szActName = pSox->GetFieldFromLablePtr( m_nSwapActorID, "_ActName" )->GetString();
#endif

	SAFE_RELEASE_SPTR( GetObjectHandle() );
	FreeAction();

	LoadSkin( CEtResourceMng::GetInstance().GetFullName( szSkinName ).c_str(), CEtResourceMng::GetInstance().GetFullName( szAniName ).c_str() );
	LoadAction( CEtResourceMng::GetInstance().GetFullName( szActName ).c_str() );

	if( m_hObject ) 
	{
		m_hObject->SetCollisionGroup( COLLISION_GROUP_DYNAMIC( 1 ) );
		m_hObject->SetTargetCollisionGroup( COLLISION_GROUP_STATIC( 1 ) | COLLISION_GROUP_DYNAMIC( 2 ) | COLLISION_GROUP_DYNAMIC( 3 ) );
	}

	if( IsExistAction( "Summon_On" ) == true )
		SetAction( "Summon_On", 0.f, 0.f );
	else
		SetAction( "Stand", 0.f, 0.f );

#ifdef _SHADOW_TEST
	if( m_hShadowActor && m_hShadowActor->GetObjectHandle() )
	{
		MASingleBody *pSingleBody = dynamic_cast<MASingleBody *>(m_hShadowActor->GetActorHandle().GetPointer());
		if( pSingleBody )
		{
			SAFE_RELEASE_SPTR( m_hShadowActor->GetObjectHandle() );
			m_hShadowActor->FreeAction();

			pSingleBody->LoadSkin( CEtResourceMng::GetInstance().GetFullName( szSkinName ).c_str(), CEtResourceMng::GetInstance().GetFullName( szAniName ).c_str() );
			m_hShadowActor->LoadAction( CEtResourceMng::GetInstance().GetFullName( szActName ).c_str() );

			if( m_hShadowActor->GetObjectHandle() ) {
				m_hShadowActor->GetObjectHandle()->SetCollisionGroup( COLLISION_GROUP_DYNAMIC( 1 ) );
				m_hShadowActor->GetObjectHandle()->SetTargetCollisionGroup( COLLISION_GROUP_STATIC( 1 ) | COLLISION_GROUP_DYNAMIC( 2 ) | COLLISION_GROUP_DYNAMIC( 3 ) );
			}
		}

		if( IsExistAction( "Summon_On" ) == true )
			SetAction( "Summon_On", 0.f, 0.f );
		else
			SetAction( "Stand", 0.f, 0.f );
	}
#endif

}
#endif