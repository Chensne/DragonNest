#include "StdAfx.h"
#include "DnUnstableHolyWaterBlow.h"
#include "PvPZombieScoreSystem.h"

#if defined( _GAMESERVER )
#include "DNPvPGameRoom.h"
#include "PvPZombieMode.h"
#include "DNGameDataManager.h"
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


CDnUnstableHolyWaterBlow::CDnUnstableHolyWaterBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor )
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_219;
	SetValue( szValue );
	m_fValue = 0.0f;
	m_vecTransformActorList.clear();
	m_nHolyWaterChance = 0;

	SetHolyWaterInfo();
}


void CDnUnstableHolyWaterBlow::SetHolyWaterInfo()
{
	std::string str = m_StateBlow.szValue;

	std::vector<std::string> tokens;
	std::string delimiters = ",";
	TokenizeA(str, tokens, delimiters);

	m_nHolyWaterChance = atoi(tokens[0].c_str());

	str = tokens[1];

	std::vector<std::string> List_tokens;
	delimiters = ";";

	TokenizeA( str, List_tokens, delimiters);

	std::vector<std::string>::iterator iter = List_tokens.begin();
	for ( ; iter != List_tokens.end(); ++iter)
		m_vecTransformActorList.push_back(atoi(iter->c_str()));
	
}

CDnUnstableHolyWaterBlow::~CDnUnstableHolyWaterBlow(void)
{

}

void CDnUnstableHolyWaterBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
	__super::OnBegin(LocalTime, fDelta);
#if defined( _GAMESERVER )
	
	if( !m_hActor || !m_hActor->IsPlayerActor() )
		return;

	CDNGameRoom* pGameRoom = m_hActor->GetGameRoom();
	if( pGameRoom == NULL || pGameRoom->bIsZombieMode() == false )
		return;

	int iRandValue = _rand(GetRoom())%100;

	if( iRandValue < m_nHolyWaterChance )
	{
		m_hActor->OnAddStateBlowProcessAfterType( CDnActor::eStateBlowAfterProcessType::eDelZombie );
		// 성수 사용시 KillCount 늘려주기
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
	}
	else
	{
		int nTransformActorSize = (int)m_vecTransformActorList.size();
		if( nTransformActorSize > 0)
		{
			CDnPlayerActor *pPlayer = static_cast<CDnPlayerActor*>(m_hActor.GetPointer());
			int iRandValue = (_rand(GetRoom())%(nTransformActorSize));

			if(iRandValue >= 0 && iRandValue < nTransformActorSize )
			{
				pPlayer->ToggleTransformMode( true, m_vecTransformActorList[iRandValue] , true ); // 이전에 변신중이라도 상관없이 변신 시킴.

				CDNGameRoom* pGameRoom = m_hActor->GetGameRoom();

				if(pGameRoom)
				{
					TMonsterMutationData Data;
					
					if(g_pDataManager->GetMonsterMutationData(m_vecTransformActorList[iRandValue], Data))
					{
						// 랜덤 사이즈
						_fpreset();
						int nScale = 100;
						int nMin = Data.nSizeMin;
						int nMax = Data.nSizeMax;
						if( nMin <= nMax ) {
							nScale = ( nMin + ( _rand(pGameRoom)%( ( nMax + 1 ) - nMin ) ) );
						}
						float fScale = nScale/100.f;
						m_hActor->SetScale( EtVector3( fScale, fScale, fScale ) );

						_ASSERT( pGameRoom->bIsPvPRoom() );
						static_cast<CDNPvPGameRoom*>(pGameRoom)->SendSelectZombie( m_hActor, Data.nMutationID , true, false , nScale );
					}

				}
			}
		}
	}	
#endif // #if defined( _GAMESERVER )
}

void CDnUnstableHolyWaterBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	__super::Process( LocalTime, fDelta );

}

void CDnUnstableHolyWaterBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	__super::OnEnd(LocalTime, fDelta);

}

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnUnstableHolyWaterBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	szNewValue = szOrigValue;
}

void CDnUnstableHolyWaterBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	szNewValue = szOrigValue;
}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
