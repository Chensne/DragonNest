#include "StdAfx.h"
#include "DnNormalMonsterActor.h"
#include "DNGameDataManager.h"

const int g_nNaviUpdateDelta = 1000;

CDnNormalMonsterActor::CDnNormalMonsterActor( CMultiRoom *pRoom, int nClassID )
: TDnMonsterActor( pRoom, nClassID )
{
//	MAWalkMovement::Initialize( this, &m_Cross, &m_vStaticPosition );
	m_nLastUpdateNaviTime = 0;
	
}

CDnNormalMonsterActor::~CDnNormalMonsterActor()
{

}

MAMovementBase* CDnNormalMonsterActor::CreateMovement()
{
	MAMovementBase* pMovement = new MAWalkMovementNav();
	return pMovement;
}

void CDnNormalMonsterActor::Process( LOCAL_TIME LocalTime, float fDelta )
{
	TDnMonsterActor< MASingleBody, MASingleDamage >::Process( LocalTime, fDelta );

	if( m_pMovement->IsNaviMode() && m_pMovement->GetNaviType() == MAMovementBase::NaviType::eTarget )
	{
		m_nLastUpdateNaviTime -= static_cast<LOCAL_TIME>(fDelta*1000.0f);
		if( m_nLastUpdateNaviTime < 0 )
			UpdateNaviMode();
	}
}

void CDnNormalMonsterActor::UpdateNaviMode()
{
	if( !m_pMovement->IsNaviMode() )
		return;

	if ( !m_pMovement->GetNaviTarget() || m_pMovement->GetNaviTarget()->IsDie() )
	{
		m_pMovement->ResetNaviMode();
		return;
	}

	CmdMoveNavi( m_pMovement->GetNaviTarget(), m_pMovement->GetNaviTargetMinDistance(), m_pMovement->GetNaviTargetActionName(), -1);
}

void CDnNormalMonsterActor::CmdMoveNavi( DnActorHandle hActor, float fMinDistance, const char *szActionName, int nLoopCount, float fBlendFrame )
{
	TDnMonsterActor< MASingleBody, MASingleDamage >::CmdMoveNavi( hActor, fMinDistance, szActionName, nLoopCount, fBlendFrame );
	if( m_hActor->IsNaviMode() )
		m_nLastUpdateNaviTime = g_nNaviUpdateDelta;
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
	g_pDataManager->GetFileNameFromFileEXT(szSkinName, pSox, m_nSwapActorID, "_SkinName");
	g_pDataManager->GetFileNameFromFileEXT(szAniName, pSox, m_nSwapActorID, "_AniName");
	g_pDataManager->GetFileNameFromFileEXT(szActName, pSox, m_nSwapActorID, "_ActName");
#else
	std::string szSkinName = pSox->GetFieldFromLablePtr( m_nSwapActorID, "_SkinName" )->GetString();
	std::string szAniName = pSox->GetFieldFromLablePtr( m_nSwapActorID, "_AniName" )->GetString();
	std::string szActName = pSox->GetFieldFromLablePtr( m_nSwapActorID, "_ActName" )->GetString();
#endif

	SAFE_RELEASE_SPTR( GetObjectHandle() );
	FreeAction();

	LoadSkin( CEtResourceMng::GetInstance().GetFullName( szSkinName ).c_str(), CEtResourceMng::GetInstance().GetFullName( szAniName ).c_str() );
	LoadAction( CEtResourceMng::GetInstance().GetFullName( szActName ).c_str() );

	if( m_hObject ) {
		m_hObject->SetCollisionGroup( COLLISION_GROUP_DYNAMIC( 1 ) );
		m_hObject->SetTargetCollisionGroup( COLLISION_GROUP_STATIC( 1 ) | COLLISION_GROUP_DYNAMIC( 2 ) | COLLISION_GROUP_DYNAMIC( 3 ) );
	}

	if( IsExistAction( "Summon_On" ) == true )
		SetAction( "Summon_On", 0.f, 0.f );
	else
		SetAction( "Stand", 0.f, 0.f );
}
#endif