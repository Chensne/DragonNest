#include "StdAfx.h"
#include "DnHolyWaterBlow.h"
#include "PvPZombieScoreSystem.h"

#if defined( _GAMESERVER )
#include "DNPvPGameRoom.h"
#include "PvPZombieMode.h"
#include "DNUserSession.h"
#include "DnPlayerActor.h"
#include "DNMissionSystem.h"
#else
#include "DnPvPGameTask.h"
#include "TaskManager.h"
#endif // #if defined( _GAMESERVER )
#if !defined( USE_BOOST_MEMPOOL )
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
#endif // #if !defined( USE_BOOST_MEMPOOL )


CDnHolyWaterBlow::CDnHolyWaterBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor )
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_181;
	SetValue( szValue );

	m_fValue = 0.0f;

}

CDnHolyWaterBlow::~CDnHolyWaterBlow(void)
{

}

void CDnHolyWaterBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
	__super::OnBegin(LocalTime, fDelta);

#if defined( _GAMESERVER )
	if( !m_hActor || !m_hActor->IsPlayerActor() )
		return;
	CDNGameRoom* pGameRoom = m_hActor->GetGameRoom();
	if( pGameRoom == NULL || pGameRoom->bIsZombieMode() == false )
		return;
	m_hActor->OnAddStateBlowProcessAfterType( CDnActor::eStateBlowAfterProcessType::eDelZombie );
	// 성수 사용시 KillCount 늘려주기.
	CPvPZombieScoreSystem* pScoreSystem = static_cast<CPvPZombieScoreSystem *>(pGameRoom->GetPvPGameMode()->GetScoreSystem());
	if( pScoreSystem)			
		pScoreSystem->OnDie(m_hActor, GetParentSkillInfo()->hSkillUser);

	CDnPlayerActor *pPlayer = static_cast<CDnPlayerActor *>(((DnActorHandle)GetParentSkillInfo()->hSkillUser).GetPointer());

	if( pPlayer )
	{
		pPlayer->GetUserSession()->AddGhoulScore(GhoulMode::PointType::HolyWaterUse);	
		pPlayer->GetUserSession()->GetEventSystem()->OnEvent( EventSystem::OnKillPlayer, 2, EventSystem::PvPGameMode, PvPCommon::GameMode::PvP_Zombie_Survival,
			EventSystem::HolyWaterUse, pPlayer->GetUserSession()->GetTotalGhoulScore(GhoulMode::PointType::HolyWaterUse));			
		// 클라에 KillCount 늘려주기
		for( UINT i=0 ;i<pGameRoom->GetUserCount() ; ++i )
		{
			CDNUserSession* pSession = pGameRoom->GetUserData(i);
			if( pSession )
				pSession->SendPvPHolyWaterKillCount( m_hActor->GetSessionID() , pPlayer->GetUserSession()->GetSessionID() );
		}
	}
#endif // #if defined( _GAMESERVER )
}

void CDnHolyWaterBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	__super::Process( LocalTime, fDelta );

}


void CDnHolyWaterBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	__super::OnEnd(LocalTime, fDelta);

}

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnHolyWaterBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	szNewValue = szOrigValue;
}

void CDnHolyWaterBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	szNewValue = szOrigValue;
}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
