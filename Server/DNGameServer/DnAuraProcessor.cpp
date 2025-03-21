#include "StdAfx.h"
#include "DnAuraProcessor.h"
#include "DnActor.h"
#include "TaskManager.h"
#include "DnPartyTask.h"
#include "DNUserSession.h"
#include "DnGameRoom.h"
#include "DnBlow.h"
#include "DnGameTask.h"


CDnAuraProcessor::CDnAuraProcessor( DnActorHandle hActor, float fRange ) : IDnSkillProcessor( hActor ), 
																		   m_fRange( fRange ),
																		   m_fRangeSQ( fRange*fRange ),
																		   m_LastAuraCheckTime( 0 ),
																		   m_bAuraOff( false )
{

}

CDnAuraProcessor::~CDnAuraProcessor(void)
{

}


void CDnAuraProcessor::OnBegin( LOCAL_TIME LocalTime, float fDelta, DnSkillHandle hParentSkill  )
{
	m_LastAuraCheckTime = LocalTime;
	m_setPartyMemberInAura.clear();
	m_mapAuraMemberSEID.clear();
	m_bAuraOff = false;
}



void CDnAuraProcessor::Process( LOCAL_TIME LocalTime, float fDelta )
{
	CDnGameTask *pGameTask = (CDnGameTask *)CTaskManager::GetInstance(m_hHasActor->GetRoom()).GetTask( "GameTask" );
	if( pGameTask && pGameTask->GetGameTaskType() == GameTaskType::Farm )
		return;

	int nRefreshTime = 30;

#ifdef PRE_ADD_AURA_FOR_MONSTER
	if(m_hHasActor->IsMonsterActor()) // 몬스터는 스캔엑터 쓰니깐 리프래쉬 타임갭을 좀더 많이 늘리도록 하겠다.
		nRefreshTime = 600;
#endif

	// 주변 동료들을 모아서 새로 들어온 녀석은 효과줄 녀석들 셋팅
	if( LocalTime - m_LastAuraCheckTime > nRefreshTime )
	{
		//assert( m_hHasActor->IsEnabledAuraSkill() && "오라 프로세서 실행중에는 오라 스킬 발동 중이어야 함" );
		DnSkillHandle hAuraSkill = m_hHasActor->GetEnabledAuraSkill();

		if( NULL == hAuraSkill )
			return;

		// 몬스터가 오라 스킬을 쓰는 경우도 생겼으므로 플레이어인 경우엔 파티로 후보들을 모아주고, 
		// 몬스터인 경우는 ScanActor 를 쓸 수 밖에 없다.. (#15461)
		DNVector( DnActorHandle ) vlActors;
		if( m_hHasActor->IsPlayerActor() )
		{
			CDnPartyTask* pPartyTask = static_cast<CDnPartyTask*>(CTaskManager::GetInstancePtr(m_hHasActor->GetRoom())->GetTask( "PartyTask" ));
			if (pPartyTask)
			{
				vlActors.reserve( pPartyTask->GetUserCount() );
				for( int i = 0; i < (int)pPartyTask->GetUserCount(); ++i )
				{
					CDNGameRoom::PartyStruct* pPartyData = pPartyTask->GetPartyData( i );
					vlActors.push_back( pPartyData->pSession->GetActorHandle() );
				}
			}
		}
		else
		{
			CDnActor::ScanActor( m_hHasActor->GetRoom(), *(m_hHasActor->GetPosition()), m_fRange, vlActors );
		}


		set<DWORD> setPrevAuraMember = m_setPartyMemberInAura;
		m_setPartyMemberInAura.clear();

		for( int i = 0; i < (int)vlActors.size(); ++i )
		{
			DnActorHandle hActor = vlActors.at( i );

			// 유효하지 않은 객체는 무시.
			if( !hActor )
				continue;

			// 자기 자신은 패스..
			if( hActor != m_hHasActor )
			{
#if defined(PRE_FIX_55383)
				bool bSkip = false;
				CDnSkill::TargetTypeEnum skillTargetType = hAuraSkill->GetTargetType();
				switch(skillTargetType)
				{
				case CDnSkill::TargetTypeEnum::Self: //위에서 자신은 스킵 하는데. 일단 코드는 여기 적용 해놓는다.
					{
						bSkip = hActor != m_hHasActor;
					}
					break;
				case CDnSkill::TargetTypeEnum::Enemy: //적으로 설정 되어 있는데, 같은 팀이면 스킵
					{
						int nMyTeam = m_hHasActor->GetTeam();
						if (m_hHasActor->IsAppliedThisStateBlow(STATE_BLOW::BLOW_149))
							nMyTeam = m_hHasActor->GetOriginalTeam();

						int nTargetTeam = hActor->GetTeam();
						if (hActor->IsAppliedThisStateBlow(STATE_BLOW::BLOW_149))
							nTargetTeam = hActor->GetOriginalTeam();

						bSkip = nMyTeam == nTargetTeam;
					}
					break;
				case CDnSkill::TargetTypeEnum::Friend: //아군으로 설정 되어 있는데 다른 팀이면 스킵
					{
						int nMyTeam = m_hHasActor->GetTeam();
						if (m_hHasActor->IsAppliedThisStateBlow(STATE_BLOW::BLOW_149))
							nMyTeam = m_hHasActor->GetOriginalTeam();

						int nTargetTeam = hActor->GetTeam();
						if (hActor->IsAppliedThisStateBlow(STATE_BLOW::BLOW_149))
							nTargetTeam = hActor->GetOriginalTeam();

						bSkip = nMyTeam != nTargetTeam;
					}
					break;
				case CDnSkill::TargetTypeEnum::Party:	//일단 Friend와 동일 하게 처리 하고, 나중에 요청이 있으면 소환 몬스터 인경우는 Party가 아님으로 처리?
					{
						int nMyTeam = m_hHasActor->GetTeam();
						if (m_hHasActor->IsAppliedThisStateBlow(STATE_BLOW::BLOW_149))
							nMyTeam = m_hHasActor->GetOriginalTeam();

						int nTargetTeam = hActor->GetTeam();
						if (hActor->IsAppliedThisStateBlow(STATE_BLOW::BLOW_149))
							nTargetTeam = hActor->GetOriginalTeam();

						bSkip = nMyTeam != nTargetTeam;
					}
					break;
				case CDnSkill::TargetTypeEnum::All:
					bSkip = false;
					break;
				}

				if (bSkip == true)
					continue;
				
#else
				// 플레이어인 경우, pvp 에서는 모두 PartyStruct 에 있으므로 팀 구분이 필요하다.
				// 몬스터인 경우엔 scanactor 를 사용하므로 마찬가지.
				if( hActor->GetTeam() != m_hHasActor->GetTeam() 
#ifdef PRE_ADD_AURA_FOR_MONSTER
					&& hAuraSkill->GetTargetType() != CDnSkill::TargetTypeEnum::Enemy
					|| (hAuraSkill->GetTargetType() == CDnSkill::TargetTypeEnum::Enemy && hActor->GetTeam() == m_hHasActor->GetTeam())
#endif
					)
					continue;
#endif // PRE_FIX_55383

				// 같은 오라 스킬을 시전하고 있는 녀석은 제외.
				DnSkillHandle hIfNewMemberAuraSkill = hActor->GetEnabledAuraSkill();
				if( hIfNewMemberAuraSkill &&
					hIfNewMemberAuraSkill->GetClassID() == hAuraSkill->GetClassID() )
					continue;

				// 다른 캐릭터의 같은 오라 스킬의 효과가 적용된 대상은 제외한다. 오라 스킬 효과의 중첩을 허용할 경우 
				// 효과가 대체되어버린 경우 다른 액터의 AuraProcessor 엔 대상이 남아있는 상태가 되어 
				// 오라를 풀어버릴 경우 다른 캐릭터의 오라 영역에 있더라도 효과 적용이 안된다.
				bool bSameAuraSkillsBlow = false;
				DNVector(DnBlowHandle) vlhAppliedBlow;
				hActor->GetAllAppliedStateBlow( vlhAppliedBlow );
				DNVector(DnBlowHandle)::iterator iter = vlhAppliedBlow.begin();
				for( iter; iter != vlhAppliedBlow.end(); ++iter )
				{
					const CDnSkill::SkillInfo* pParentSkillInfo = (*iter)->GetParentSkillInfo();
					if( m_hHasActor != pParentSkillInfo->hSkillUser &&
						pParentSkillInfo->iSkillID == hAuraSkill->GetClassID() )
					{
						bSameAuraSkillsBlow = true;
						break;
					}
				}

				if( bSameAuraSkillsBlow )
					continue;

				// 몬스터인 경우엔 ScanActor 를 사용하기 때문에 범위 안에 있는 놈들만 검색되어 
				// 따로 거리체크를 할 필요 없다.
				bool bInRange = true;
				if( m_hHasActor->IsPlayerActor() )
				{
					bInRange = false;
					const EtVector3* pPos = hActor->GetPosition();
					const EtVector3* pMyPos = m_hHasActor->GetPosition();

					EtVector3 vDist = *pPos - *pMyPos;
					float fDistSQ = EtVec3LengthSq( &vDist );

					// 일정 거리 이하인 애들은 집어넣고,, 일단 거리는 임의대로 정해놓는다. (300)
					if( fDistSQ < m_fRangeSQ )
						bInRange = true;
				}
				
				if( bInRange )
					m_setPartyMemberInAura.insert( hActor->GetUniqueID() );
			}
		}

		// set 의 차집합으로 새로 들어간 녀석과 빠진 녀석들을 골라냄.
		vector<DWORD> vlNewMember;
		set_difference( m_setPartyMemberInAura.begin(), m_setPartyMemberInAura.end(), setPrevAuraMember.begin(), setPrevAuraMember.end(), 
						back_inserter(vlNewMember) );

		vector<DWORD> vlOutMember;
		set_difference( setPrevAuraMember.begin(), setPrevAuraMember.end(), m_setPartyMemberInAura.begin(), m_setPartyMemberInAura.end(),
						back_inserter(vlOutMember) );

		int iNumNewMember = (int)vlNewMember.size();
		for( int iMember = 0; iMember < iNumNewMember; ++iMember )
		{
			// 오라가 갖고 있는 target 상태효과 추가.
			DnActorHandle hNewMember = CDnActor::FindActorFromUniqueID( m_hHasActor->GetRoom(), vlNewMember.at(iMember) );
			if( hNewMember )
			{
				DWORD dwNumStateEffect = hAuraSkill->GetStateEffectCount();
				for( DWORD dwStateEffect = 0; dwStateEffect < dwNumStateEffect; ++dwStateEffect )
				{
					const CDnSkill::StateEffectStruct* pSE = hAuraSkill->GetStateEffectFromIndex( dwStateEffect );
#if defined(PRE_FIX_55383)
					bool bCanApply = false;
					switch(pSE->ApplyType)
					{
					case CDnSkill::ApplySelf:	//Self인경우 대상이 자신이어야 한다.
						{
							bCanApply = m_hHasActor == hNewMember;
						}
						break;
					case CDnSkill::ApplyTarget: //대상이 자신이 아니면 팀 상관 없이
						{
							bCanApply = m_hHasActor != hNewMember;
						}
						break;
					case CDnSkill::ApplyAll: //무조건
						{
							bCanApply = true;
						}
						break;
					case CDnSkill::ApplyEnemy: //팀이 다른 경우만
						{
							int nMyTeam = m_hHasActor->GetTeam();
							if (m_hHasActor->IsAppliedThisStateBlow(STATE_BLOW::BLOW_149))
								nMyTeam = m_hHasActor->GetOriginalTeam();

							int nTargetTeam = hNewMember->GetTeam();
							if (hNewMember->IsAppliedThisStateBlow(STATE_BLOW::BLOW_149))
								nTargetTeam = hNewMember->GetOriginalTeam();

							bCanApply = nMyTeam != nTargetTeam;
						}
						break;
					case CDnSkill::ApplyFriend: //팀이 같은 경우만..
						{
							int nMyTeam = m_hHasActor->GetTeam();
							if (m_hHasActor->IsAppliedThisStateBlow(STATE_BLOW::BLOW_149))
								nMyTeam = m_hHasActor->GetOriginalTeam();

							int nTargetTeam = hNewMember->GetTeam();
							if (hNewMember->IsAppliedThisStateBlow(STATE_BLOW::BLOW_149))
								nTargetTeam = hNewMember->GetOriginalTeam();

							bCanApply = nMyTeam == nTargetTeam;
						}
						break;
					}

					if (bCanApply == true)
					{
						int iID = hNewMember->CmdAddStateEffect( /*m_hHasActor,*/ hAuraSkill->GetInfo(), (STATE_BLOW::emBLOW_INDEX)pSE->nID, -1/*SE.nDurationTime*/, pSE->szValue.c_str() );
						m_mapAuraMemberSEID[ hNewMember->GetUniqueID() ].push_back( iID );
					}
#else
					if( CDnSkill::ApplyTarget == pSE->ApplyType )
					{
						int iID = hNewMember->CmdAddStateEffect( /*m_hHasActor,*/ hAuraSkill->GetInfo(), (STATE_BLOW::emBLOW_INDEX)pSE->nID, -1/*SE.nDurationTime*/, pSE->szValue.c_str() );
						m_mapAuraMemberSEID[ hNewMember->GetUniqueID() ].push_back( iID );
					}
#endif // PRE_FIX_55383
				}
			}
		}

		int iNumOutMember = (int)vlOutMember.size();
		for( int iMember = 0; iMember < iNumOutMember; ++iMember )
		{
			// 오라가 갖고 있는 상태효과 제거.
			DnActorHandle hOutMember = CDnActor::FindActorFromUniqueID( m_hHasActor->GetRoom(), vlOutMember.at(iMember) );
			if( hOutMember )
			{
				//// 같은 오라 스킬을 시전하고 있는 녀석은 신경쓰지 않는다.
				//DnSkillHandle hIfOutMemberAuraSkill = hOutMember->GetEnabledAuraSkill();
				//if( hIfOutMemberAuraSkill &&
				//	hIfOutMemberAuraSkill->GetClassID() == hAuraSkill->GetClassID() )
				//	continue;

				// 같은 오라 스킬을 시전하고 있더라도 걸려있는 버프는 내 오라로 걸어준 것일 수도 있으므로 
				// 상태효과 생성시에 부여된 아이디로 상태효과 제거.
				//DWORD dwNumStateEffect = hAuraSkill->GetStateEffectCount();
				//for( DWORD dwStateEffect = 0; dwStateEffect < dwNumStateEffect; ++dwStateEffect )
				//{
				//	const CDnSkill::StateEffectStruct* pSE = hAuraSkill->GetStateEffectFromIndex( dwStateEffect );
				//	if( CDnSkill::ApplyTarget == pSE->ApplyType )
				//		hOutMember->CmdRemoveStateEffect( (STATE_BLOW::emBLOW_INDEX)pSE->nID );
				//}

				vector<int>& vlBlowIDsToRemove = m_mapAuraMemberSEID[ hOutMember->GetUniqueID() ];
				for( int i = 0; i < (int)vlBlowIDsToRemove.size(); ++i )
					hOutMember->CmdRemoveStateEffectFromID( vlBlowIDsToRemove.at(i) );

				m_mapAuraMemberSEID.erase( hOutMember->GetUniqueID() );
			}
		}

		m_LastAuraCheckTime = LocalTime;
	}
}




void CDnAuraProcessor::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	if( m_setPartyMemberInAura.empty() )
		return;

	// 오라 스킬이라면 오라가 갖고 있는 상태효과 제거.
	set<DWORD>::iterator iter = m_setPartyMemberInAura.begin();

	//assert( m_hHasActor->IsEnabledAuraSkill() && "오라 프로세서 실행중에는 오라 스킬 발동 중이어야 함" );
	DnSkillHandle hAuraSkill = m_hHasActor->GetEnabledAuraSkill();

	if( hAuraSkill )
	{
		for( iter; iter != m_setPartyMemberInAura.end(); ++iter )
		{
			DnActorHandle hMember = CDnActor::FindActorFromUniqueID( m_hHasActor->GetRoom(), *iter );

			if( hMember )
			{
				vector<int>& vlBlowIDsToRemove = m_mapAuraMemberSEID[ hMember->GetUniqueID() ];
				for( int i = 0; i < (int)vlBlowIDsToRemove.size(); ++i )
					hMember->CmdRemoveStateEffectFromID( vlBlowIDsToRemove.at(i) );
			}
		}

		m_setPartyMemberInAura.clear();
		m_mapAuraMemberSEID.clear();
	}

	m_bAuraOff = true;
}
