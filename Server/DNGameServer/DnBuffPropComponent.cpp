#include "StdAfx.h"
#include "DnBuffPropComponent.h"
#include "DnActor.h"
#include "SMScanActor.h"
#include "boost/shared_ptr.hpp"
#include "DnMonsterActor.h"


CDnBuffPropComponent::CDnBuffPropComponent(void) : m_iTargetTeamType( -1 ), m_iMyTeam( -1 ), m_pGameRoom(NULL)
{
}

CDnBuffPropComponent::~CDnBuffPropComponent(void)
{
	// 버프 걸려있는 액터들 모두 날려줌.
	set<int>::iterator iter = m_setAffectingActors.begin();
	for( iter; iter != m_setAffectingActors.end(); ++iter )
	{
		DnActorHandle hActor = CDnActor::FindActorFromUniqueID( m_pGameRoom, *iter );
		if( hActor )
		{
			map<int, vector<int> >::iterator iterMap = m_mapAppliedStateEffect.find( hActor->GetUniqueID() );
			if( m_mapAppliedStateEffect.end() != iterMap )
			{
				const vector<int>& vlThisPropSEIDs = iterMap->second;

				int iNumIDs = (int)vlThisPropSEIDs.size();
				for( int i = 0; i < iNumIDs; ++i )
					hActor->CmdRemoveStateEffectFromID( vlThisPropSEIDs.at(i) );
			}
		}
	}
}


void CDnBuffPropComponent::Initialize( CMultiRoom* pRoom, int iTargetTeamType, int iSkillID, 
									   int iSkillLevel, const set<int>* pSetTargetActorIDs/* = NULL*/ )
{
	m_SkillComponent.Initialize( iSkillID, iSkillLevel );

	m_pGameRoom = pRoom;

	m_iTargetTeamType = iTargetTeamType;

	// 대상 팀 지정이 특정 액터 ID 대상일 때만 유효함.
	if( pSetTargetActorIDs )
	{
		_ASSERT( 3 == m_iTargetTeamType );
		m_setTargetActorIDs = *pSetTargetActorIDs;
	}
}


void CDnBuffPropComponent::OnMessage( const boost::shared_ptr<IStateMessage>& pMessage )
{
	const boost::shared_ptr<CSMScanActor> pScanActorMsg = boost::static_pointer_cast<CSMScanActor>(pMessage);
	set<int> setScanActor = pScanActorMsg->GetScanActorSet();

	// 새로 추가된 액터.
	vector<int> vlhNewActors;
	set_difference( setScanActor.begin(), setScanActor.end(), 
					m_setAffectingActors.begin(), m_setAffectingActors.end(), 
					back_inserter(vlhNewActors) );

	// 바깥으로 나간 액터.
	vector<int> vlhOutActors;
	set_difference( m_setAffectingActors.begin(), m_setAffectingActors.end(), 
					setScanActor.begin(), setScanActor.end(),
					back_inserter(vlhOutActors) );

	// 추가된 액터에게 효과 추가.
	bool bUpdated = false;
	if( false == vlhNewActors.empty() )
	{
		vector<int>::iterator iter = vlhNewActors.begin();
		for( iter; iter != vlhNewActors.end(); ++iter )
		{
			// 타겟 타입 별로 처리함. 지속시간은 영구적으로 해놓고 바깥으로 벗어나면 삭제시켜줌.
			// npc 는 제외
			DnActorHandle hActor = CDnActor::FindActorFromUniqueID( m_pGameRoom, *iter );
			if( hActor && hActor->GetActorType() < CDnActorState::Npc )
			{
				// 특정 액터 대상일 땐 ID 맞는 애들만 처리한다.
				bool bTargetActor = false;
				if( 3 == m_iTargetTeamType )
				{
					if( m_setTargetActorIDs.end() != m_setTargetActorIDs.find(hActor->GetClassID()) )
					{
						bTargetActor = true;
					}
				}
				else
				{
					bTargetActor = true;
				}

				if( false == bTargetActor )
					continue;

				// 상태이상 Add 시에 지속 구분 인덱스 처리 필요.
				// 해당 Actor의 지속효과 구분 인덱스를 구분하여 성공한 경우에 상태이상 추가 시킴.
				map<int, bool> mapDuplicateResult;
				const CDnSkill::SkillInfo& HasSkillInfo = m_SkillComponent.GetSkillInfo();
				CDnSkill::CanApply eResult = CDnSkill::CanApplySkillStateEffect( &HasSkillInfo, hActor, HasSkillInfo.iSkillID, HasSkillInfo.iLevel, 
																				 HasSkillInfo.iSkillDuplicateMethod, 
																				 HasSkillInfo.iDuplicateCount, mapDuplicateResult );

				if( CDnSkill::CanApply::Fail != eResult )
				{
					vector<int> vlAddedSEIDs;
					int iNumStateEffect = m_SkillComponent.GetNumStateEffect();
					for( int iStateEffect = 0; iStateEffect < iNumStateEffect; ++iStateEffect  )
					{
						int iActorType = hActor->GetActorType();
						const CDnSkill::StateEffectStruct* pStateEffectInfo = m_SkillComponent.GetStateEffectInfo( iStateEffect );
						bool bCheckTeam = false;
						if( 3 != m_iTargetTeamType )		// 특정 액터 ID 대상이 아닐 때.
						{
							if( -1 != m_iMyTeam )
							{								
								bCheckTeam = (2 == m_iTargetTeamType) ||		// all
											 (m_iTargetTeamType == 0 && (m_iMyTeam == hActor->GetTeam()) ) ||	// friend
											 (m_iTargetTeamType == 1 && (m_iMyTeam != hActor->GetTeam()) );		// enemy
							}
							else
							{
								// #45096 플레이어가 소환한 몬스터는 플레이어 팀으로 판단.
								if( hActor->IsMonsterActor() )
								{
									CDnMonsterActor* pMonsterActor = static_cast<CDnMonsterActor*>(hActor.GetPointer());
									DnActorHandle hSummonerPlayer = pMonsterActor->GetSummonerPlayerActor();
									if( hSummonerPlayer )
										iActorType = hSummonerPlayer->GetActorType();
								}

								bCheckTeam = (2 == m_iTargetTeamType) ||		// all
									(m_iTargetTeamType == 0 && iActorType <= CDnActorState::Reserved6 || iActorType == CDnActorState::Vehicle ) ||	// player
											 (m_iTargetTeamType == 1 && iActorType > CDnActorState::Reserved6 );	// monster
							}
						}
						else
						{
							bCheckTeam = true;
						}

						if( bCheckTeam )
						{
							// 같은 스킬 중첩일 경우엔 스킬 효과 중에 확률 체크하는 것들은 이미 CanApplySkillStateEffect 에서 확률체크되고
							// 통과된 상태이다. 따라서 여기선 확률 체크 된건지 확인하고 된거라면 다시 확률 체크 안하도록 함수 호출 해준다.
							bool bAllowAddThisSE = true;
							bool bCheckCanBegin = true;
							if( CDnSkill::CanApply::ApplyDuplicateSameSkill == eResult )
							{
								map<int, bool>::iterator iter = mapDuplicateResult.find( pStateEffectInfo->nID );
								// 맵에 없는 경우 현재 액터가 상태효과에 걸려있지 않으므로 그냥 정상적으로 상태효과 추가 루틴 실행.
								if( mapDuplicateResult.end() != iter )
								{
									// 같은 스킬의 확률있는 상태효과가 현재 걸려있어서 CanAdd 를 호출해보았으나 실패했음.
									// 이런 경우엔 상태효과 추가하지 않는다.
									if( false == (iter->second) )
										bAllowAddThisSE = false;
									else
										// 이미 CanAdd 를 통과한 상태이므로 CmdAddStateEffect 호출 시 따로 체크하지 않도록 해준다.
										bCheckCanBegin = false;
								}
							}

							int iID = hActor->CmdAddStateEffect( &m_SkillComponent.GetSkillInfo(), (STATE_BLOW::emBLOW_INDEX)pStateEffectInfo->nID, 
																 -1, pStateEffectInfo->szValue.c_str(), false, bCheckCanBegin );
							if( -1 != iID )
								vlAddedSEIDs.push_back( iID );
						}
					}

					if( false == vlAddedSEIDs.empty() )
						m_mapAppliedStateEffect.insert( make_pair(hActor->GetUniqueID(), vlAddedSEIDs) );
				}
			}
		}

		bUpdated = true;
	}

	// 바깥으로 나간 액터에게 효과 제거.
	if( false == vlhOutActors.empty() )
	{
		vector<int>::iterator iter = vlhOutActors.begin();
		for( iter; iter != vlhOutActors.end(); ++iter )
		{
			DnActorHandle hActor = CDnActor::FindActorFromUniqueID( m_pGameRoom, *iter );
			if( hActor && hActor->GetActorType() < CDnActorState::Npc )
			{
				map<int, vector<int> >::iterator iterMap = m_mapAppliedStateEffect.find( hActor->GetUniqueID() );
				if( m_mapAppliedStateEffect.end() != iterMap )
				{
					const vector<int>& vlThisPropSEIDs = iterMap->second;

					int iNumIDs = (int)vlThisPropSEIDs.size();
					for( int i = 0; i < iNumIDs; ++i )
						hActor->CmdRemoveStateEffectFromID( vlThisPropSEIDs.at(i) );

					m_mapAppliedStateEffect.erase( iterMap );
				}
			}
		}

		bUpdated = true;
	}

	if( bUpdated )
		m_setAffectingActors = setScanActor;
}

void CDnBuffPropComponent::RemoveAffectedStateEffects( void )
{
	set<int>::iterator iter = m_setAffectingActors.begin();
	for( iter; iter != m_setAffectingActors.end(); ++iter )
	{
		map<int, vector<int> >::iterator iterMap = m_mapAppliedStateEffect.find( *iter );
		if( m_mapAppliedStateEffect.end() != iterMap )
		{
			DnActorHandle hActor = CDnActor::FindActorFromUniqueID( m_pGameRoom, *iter );
			if( hActor )
			{
				vector<int>& vlBlowsToRemove = iterMap->second;
				for( int i = 0; i < (int)vlBlowsToRemove.size(); ++i )
				{
					int iBlowID = vlBlowsToRemove.at( i );
					hActor->CmdRemoveStateEffectFromID( iBlowID );
				}
			}
		}
	}

	m_setAffectingActors.clear();
	m_mapAppliedStateEffect.clear();
}