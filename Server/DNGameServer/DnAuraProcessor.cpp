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
	if(m_hHasActor->IsMonsterActor()) // ���ʹ� ��ĵ���� ���ϱ� �������� Ÿ�Ӱ��� ���� ���� �ø����� �ϰڴ�.
		nRefreshTime = 600;
#endif

	// �ֺ� ������� ��Ƽ� ���� ���� �༮�� ȿ���� �༮�� ����
	if( LocalTime - m_LastAuraCheckTime > nRefreshTime )
	{
		//assert( m_hHasActor->IsEnabledAuraSkill() && "���� ���μ��� �����߿��� ���� ��ų �ߵ� ���̾�� ��" );
		DnSkillHandle hAuraSkill = m_hHasActor->GetEnabledAuraSkill();

		if( NULL == hAuraSkill )
			return;

		// ���Ͱ� ���� ��ų�� ���� ��쵵 �������Ƿ� �÷��̾��� ��쿣 ��Ƽ�� �ĺ����� ����ְ�, 
		// ������ ���� ScanActor �� �� �� �ۿ� ����.. (#15461)
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

			// ��ȿ���� ���� ��ü�� ����.
			if( !hActor )
				continue;

			// �ڱ� �ڽ��� �н�..
			if( hActor != m_hHasActor )
			{
#if defined(PRE_FIX_55383)
				bool bSkip = false;
				CDnSkill::TargetTypeEnum skillTargetType = hAuraSkill->GetTargetType();
				switch(skillTargetType)
				{
				case CDnSkill::TargetTypeEnum::Self: //������ �ڽ��� ��ŵ �ϴµ�. �ϴ� �ڵ�� ���� ���� �س��´�.
					{
						bSkip = hActor != m_hHasActor;
					}
					break;
				case CDnSkill::TargetTypeEnum::Enemy: //������ ���� �Ǿ� �ִµ�, ���� ���̸� ��ŵ
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
				case CDnSkill::TargetTypeEnum::Friend: //�Ʊ����� ���� �Ǿ� �ִµ� �ٸ� ���̸� ��ŵ
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
				case CDnSkill::TargetTypeEnum::Party:	//�ϴ� Friend�� ���� �ϰ� ó�� �ϰ�, ���߿� ��û�� ������ ��ȯ ���� �ΰ��� Party�� �ƴ����� ó��?
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
				// �÷��̾��� ���, pvp ������ ��� PartyStruct �� �����Ƿ� �� ������ �ʿ��ϴ�.
				// ������ ��쿣 scanactor �� ����ϹǷ� ��������.
				if( hActor->GetTeam() != m_hHasActor->GetTeam() 
#ifdef PRE_ADD_AURA_FOR_MONSTER
					&& hAuraSkill->GetTargetType() != CDnSkill::TargetTypeEnum::Enemy
					|| (hAuraSkill->GetTargetType() == CDnSkill::TargetTypeEnum::Enemy && hActor->GetTeam() == m_hHasActor->GetTeam())
#endif
					)
					continue;
#endif // PRE_FIX_55383

				// ���� ���� ��ų�� �����ϰ� �ִ� �༮�� ����.
				DnSkillHandle hIfNewMemberAuraSkill = hActor->GetEnabledAuraSkill();
				if( hIfNewMemberAuraSkill &&
					hIfNewMemberAuraSkill->GetClassID() == hAuraSkill->GetClassID() )
					continue;

				// �ٸ� ĳ������ ���� ���� ��ų�� ȿ���� ����� ����� �����Ѵ�. ���� ��ų ȿ���� ��ø�� ����� ��� 
				// ȿ���� ��ü�Ǿ���� ��� �ٸ� ������ AuraProcessor �� ����� �����ִ� ���°� �Ǿ� 
				// ���� Ǯ����� ��� �ٸ� ĳ������ ���� ������ �ִ��� ȿ�� ������ �ȵȴ�.
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

				// ������ ��쿣 ScanActor �� ����ϱ� ������ ���� �ȿ� �ִ� ��鸸 �˻��Ǿ� 
				// ���� �Ÿ�üũ�� �� �ʿ� ����.
				bool bInRange = true;
				if( m_hHasActor->IsPlayerActor() )
				{
					bInRange = false;
					const EtVector3* pPos = hActor->GetPosition();
					const EtVector3* pMyPos = m_hHasActor->GetPosition();

					EtVector3 vDist = *pPos - *pMyPos;
					float fDistSQ = EtVec3LengthSq( &vDist );

					// ���� �Ÿ� ������ �ֵ��� ����ְ�,, �ϴ� �Ÿ��� ���Ǵ�� ���س��´�. (300)
					if( fDistSQ < m_fRangeSQ )
						bInRange = true;
				}
				
				if( bInRange )
					m_setPartyMemberInAura.insert( hActor->GetUniqueID() );
			}
		}

		// set �� ���������� ���� �� �༮�� ���� �༮���� ���.
		vector<DWORD> vlNewMember;
		set_difference( m_setPartyMemberInAura.begin(), m_setPartyMemberInAura.end(), setPrevAuraMember.begin(), setPrevAuraMember.end(), 
						back_inserter(vlNewMember) );

		vector<DWORD> vlOutMember;
		set_difference( setPrevAuraMember.begin(), setPrevAuraMember.end(), m_setPartyMemberInAura.begin(), m_setPartyMemberInAura.end(),
						back_inserter(vlOutMember) );

		int iNumNewMember = (int)vlNewMember.size();
		for( int iMember = 0; iMember < iNumNewMember; ++iMember )
		{
			// ���� ���� �ִ� target ����ȿ�� �߰�.
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
					case CDnSkill::ApplySelf:	//Self�ΰ�� ����� �ڽ��̾�� �Ѵ�.
						{
							bCanApply = m_hHasActor == hNewMember;
						}
						break;
					case CDnSkill::ApplyTarget: //����� �ڽ��� �ƴϸ� �� ��� ����
						{
							bCanApply = m_hHasActor != hNewMember;
						}
						break;
					case CDnSkill::ApplyAll: //������
						{
							bCanApply = true;
						}
						break;
					case CDnSkill::ApplyEnemy: //���� �ٸ� ��츸
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
					case CDnSkill::ApplyFriend: //���� ���� ��츸..
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
			// ���� ���� �ִ� ����ȿ�� ����.
			DnActorHandle hOutMember = CDnActor::FindActorFromUniqueID( m_hHasActor->GetRoom(), vlOutMember.at(iMember) );
			if( hOutMember )
			{
				//// ���� ���� ��ų�� �����ϰ� �ִ� �༮�� �Ű澲�� �ʴ´�.
				//DnSkillHandle hIfOutMemberAuraSkill = hOutMember->GetEnabledAuraSkill();
				//if( hIfOutMemberAuraSkill &&
				//	hIfOutMemberAuraSkill->GetClassID() == hAuraSkill->GetClassID() )
				//	continue;

				// ���� ���� ��ų�� �����ϰ� �ִ��� �ɷ��ִ� ������ �� ����� �ɾ��� ���� ���� �����Ƿ� 
				// ����ȿ�� �����ÿ� �ο��� ���̵�� ����ȿ�� ����.
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

	// ���� ��ų�̶�� ���� ���� �ִ� ����ȿ�� ����.
	set<DWORD>::iterator iter = m_setPartyMemberInAura.begin();

	//assert( m_hHasActor->IsEnabledAuraSkill() && "���� ���μ��� �����߿��� ���� ��ų �ߵ� ���̾�� ��" );
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
