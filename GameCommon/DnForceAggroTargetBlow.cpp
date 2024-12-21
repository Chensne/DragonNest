
#include "StdAfx.h"
#include "DnForceAggroTargetBlow.h"

#ifdef _GAMESERVER
#include "DNAggroSystem.h"
#include "DnMonsterActor.h"
#include "DnStateBlow.h"
#endif

// #60235 [데저트 드래곤] 어그로 대상에게 StateEffect 표시 기능

CDnForceAggroTargetBlow::CDnForceAggroTargetBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor )
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_254;
	SetValue( szValue );

#ifdef _GAMESERVER
	m_bFindRandomAggroTarget = false;
	m_bFindPlayerActorOnly = false;
	m_fForceIncreaseAggroValue = 0.f;
	m_nServerGraphicBlowID = -1;
	m_tLastRefreshTime = 0;
	m_tLastFindTargetTime = 0;
	m_nTargetEffectIndex = 0;
	m_hTargetActor.Identity();


	std::string str = m_StateBlow.szValue;
	std::vector<std::string> tokens;
	std::string delimiters = ";";
	TokenizeA(str, tokens, delimiters);

	if( tokens.size() == 4 )
	{
		m_bFindRandomAggroTarget = atoi( tokens[0].c_str() ) == 0 ? true : false;
		m_bFindPlayerActorOnly = atoi( tokens[1].c_str() ) == 0 ? true : false;
		m_fForceIncreaseAggroValue = (float)atof( tokens[2].c_str() );
		m_nTargetEffectIndex = atoi( tokens[3].c_str() );
	}
#endif

}

CDnForceAggroTargetBlow::~CDnForceAggroTargetBlow(void)
{
#if defined(_GAMESERVER)
	if( m_hTargetActor && m_nServerGraphicBlowID != -1 )
	{
		m_hTargetActor->CmdRemoveStateEffectFromID( m_nServerGraphicBlowID );
	}
#endif
}

void CDnForceAggroTargetBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
	CDnBlow::OnBegin( LocalTime, fDelta );

#ifdef _GAMESERVER
	ForceAggroTarget();
#endif

}


void CDnForceAggroTargetBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	CDnBlow::OnEnd( LocalTime , fDelta );

#ifdef _GAMESERVER
	if( m_hActor && m_hActor->IsMonsterActor() )
	{
		CDnMonsterActor* pMonsterActor = static_cast<CDnMonsterActor*>( m_hActor.GetPointer() );
		if( pMonsterActor && pMonsterActor->GetAggroSystem() )
		{
			pMonsterActor->GetAggroSystem()->SetIgnoreFlag( false );
		}

		if( m_hTargetActor ) // DetachGraphic Effect 
		{
			if( m_nTargetEffectIndex > 0 )
			{
				m_hTargetActor->CmdRemoveStateEffectFromID( m_nServerGraphicBlowID );
			}
		}
	}
#endif

}



void CDnForceAggroTargetBlow::Process(LOCAL_TIME LocalTime, float fDelta )
{   
	CDnBlow::Process( LocalTime, fDelta );

#ifdef _GAMESERVER
	// Target die or some kind of error process 
	if( m_hActor && m_hActor->IsMonsterActor() )
	{
		if( m_hActor->IsDie() )
		{
			if( m_hTargetActor )
			{
				m_hTargetActor->CmdRemoveStateEffectFromID( m_nServerGraphicBlowID );
			}
			m_nServerGraphicBlowID = -1;
			SetState(STATE_BLOW::STATE_END);
			return;
		}

		CDnMonsterActor* pMonsterActor = static_cast<CDnMonsterActor*>( m_hActor.GetPointer() );
		if( pMonsterActor && pMonsterActor->GetAggroSystem() && pMonsterActor->GetAIBase() )
		{
			std::list<CDNAggroSystem::AggroStruct>& aggroList = pMonsterActor->GetAggroSystem()->GetAggroList();

			if( !aggroList.empty() )
			{
				if( m_hTargetActor )
				{
					CDNAggroSystem::AggroStruct *Info = pMonsterActor->GetAggroSystem()->GetAggroStruct( m_hTargetActor );
					if( !Info )
					{
						if( !m_hTargetActor->IsDie() )
						{
							if( m_nTargetEffectIndex > 0 )
							{
								m_hTargetActor->CmdRemoveStateEffectFromID( m_nServerGraphicBlowID );
								m_hTargetActor = CDnActor::Identity();
							}
						}

						if( LocalTime - m_tLastRefreshTime > 1000 ) 
						{
							ForceAggroTarget();
							m_tLastRefreshTime = LocalTime;
						}
					}
				}
				else
				{
					if( LocalTime - m_tLastRefreshTime > 1000 )
					{
						ForceAggroTarget();
						m_tLastRefreshTime = LocalTime;
					}
				}
			}
			else
			{
				if( m_fForceIncreaseAggroValue == 0 )
				{
					pMonsterActor->GetAggroSystem()->SetIgnoreFlag( false );
					m_hTargetActor = CDnActor::Identity();
				}
			}

			
			if( m_fForceIncreaseAggroValue != 0 ) 
			{
				if( LocalTime - m_tLastFindTargetTime > 1000 )
				{
					// 스킬 사용중인경우 Ai가 동작하지않기때문에 스킬 사용중에도 타겟을 변경할수 있도록 설정합니다.
					FindAggroTargetWhenProcessSkill();
					m_tLastFindTargetTime = LocalTime;
				}
			}
			else if( !m_hTargetActor || ( m_hTargetActor && m_hTargetActor->IsDie() ) )
			{
				if( LocalTime - m_tLastFindTargetTime > 1000 )
				{
					ForceAggroTarget();
					m_tLastFindTargetTime = LocalTime;
				}
			}
		}
	}
#endif

}



#ifdef _GAMESERVER
void CDnForceAggroTargetBlow::ForceAggroTarget()
{
	if( m_hActor && m_hActor->IsMonsterActor() )
	{
		bool bSuccess = false;

		CDnMonsterActor* pMonsterActor = static_cast<CDnMonsterActor*>( m_hActor.GetPointer() );
		if( pMonsterActor && pMonsterActor->GetAggroSystem() && pMonsterActor->GetAIBase() )
		{
			if( m_fForceIncreaseAggroValue == 0 )
			{
				pMonsterActor->GetAggroSystem()->SetIgnoreFlag( false );
			}

			if( m_bFindRandomAggroTarget )
			{
				DnActorHandle hResultActor;
				hResultActor = FindRandomAggroTarget();

				if( hResultActor )
				{
					if( pMonsterActor->GetAggroTarget() )
					{
						CDNAggroSystem::AggroStruct *Info = pMonsterActor->GetAggroSystem()->GetAggroStruct( pMonsterActor->GetAggroTarget() );

						if( pMonsterActor->GetAggroTarget() == hResultActor )
						{
							pMonsterActor->GetAggroSystem()->AddAggro( hResultActor , (int)(Info->iAggro * m_fForceIncreaseAggroValue) , true );
						}
						else
						{
							pMonsterActor->SetAggroTarget( hResultActor , (int)(Info->iAggro * (1 + m_fForceIncreaseAggroValue)) );
						}
					}

					pMonsterActor->GetAIBase()->ChangeTarget( hResultActor , false );

					m_hTargetActor = hResultActor;
					bSuccess = true;
				}
			}
			else // m_bFindRandomAggroTarget == false
			{
				if( pMonsterActor->GetAggroTarget() )
				{
					CDNAggroSystem::AggroStruct *Info = pMonsterActor->GetAggroSystem()->GetAggroStruct( pMonsterActor->GetAggroTarget() );
					pMonsterActor->GetAggroSystem()->AddAggro(  pMonsterActor->GetAggroTarget() , (int)(Info->iAggro * m_fForceIncreaseAggroValue) , true );
					m_hTargetActor = pMonsterActor->GetAggroTarget();
					bSuccess = true;
				}
				else
				{
					bool bIsProvocationTarget = false;
					m_hTargetActor	= pMonsterActor->GetAggroSystem()->OnGetAggroTarget( bIsProvocationTarget );
					pMonsterActor->GetAIBase()->ChangeTarget( m_hTargetActor , false );

					if( pMonsterActor->GetAggroTarget() )
					{
						CDNAggroSystem::AggroStruct *Info = pMonsterActor->GetAggroSystem()->GetAggroStruct( pMonsterActor->GetAggroTarget() );
						pMonsterActor->GetAggroSystem()->AddAggro(  pMonsterActor->GetAggroTarget() , (int)(Info->iAggro * m_fForceIncreaseAggroValue) , true );
					}

					bSuccess = true;
				}
			}

			if( m_fForceIncreaseAggroValue == 0 )
			{
				pMonsterActor->GetAggroSystem()->SetIgnoreFlag( true );
			}

			if( bSuccess && m_hTargetActor )
			{
				// AttachGraphic Effect 
				if( m_nTargetEffectIndex > 0 )
				{
					std::string strEffect = FormatA( "%d" , m_nTargetEffectIndex );
					m_nServerGraphicBlowID = m_hTargetActor->CmdAddStateEffect( GetParentSkillInfo() , STATE_BLOW::BLOW_100, (int)(GetDurationTime() * 1000) , strEffect.c_str() );
				}
			}
		}
	}
}

DnActorHandle CDnForceAggroTargetBlow::FindRandomAggroTarget()
{
	DnActorHandle hResultActor;

	CDnMonsterActor* pMonsterActor = static_cast<CDnMonsterActor*>( m_hActor.GetPointer() );
	if( pMonsterActor && pMonsterActor->GetAggroSystem() && pMonsterActor->GetAIBase() )
	{
		std::list<CDNAggroSystem::AggroStruct>& aggroList = pMonsterActor->GetAggroSystem()->GetAggroList();
		if( !aggroList.empty() )
		{
			if( m_bFindPlayerActorOnly )
			{
				std::vector<DnActorHandle> vecActorList;
				for( std::list<CDNAggroSystem::AggroStruct>::iterator itor = aggroList.begin() ; itor != aggroList.end() ; ++itor )
				{
					if( (*itor).hActor && (*itor).hActor->IsPlayerActor() )
						vecActorList.push_back( (*itor).hActor );
				}

				if( !vecActorList.empty() )
				{
					int nRandomIndex = (int)( _rand(m_hActor->GetRoom()) % vecActorList.size());
					hResultActor = vecActorList[nRandomIndex];
				}
			}
			else
			{
				int nRandomIndex = (int)( _rand(m_hActor->GetRoom()) % aggroList.size());
				std::list<CDNAggroSystem::AggroStruct>::iterator itor = aggroList.begin();
				std::advance( itor , nRandomIndex );
				hResultActor = (*itor).hActor;
			}
		}
	}

	return hResultActor;
}

void CDnForceAggroTargetBlow::FindAggroTargetWhenProcessSkill()
{
	CDnMonsterActor* pMonsterActor = static_cast<CDnMonsterActor*>( m_hActor.GetPointer() );
	if( pMonsterActor && pMonsterActor->GetAggroSystem() && pMonsterActor->GetAIBase() )
	{
		if( m_hActor->IsProcessSkill() )
		{
			bool bIsProvocationTarget = false;

			DnActorHandle hActor = pMonsterActor->GetAggroSystem()->OnGetAggroTarget( bIsProvocationTarget );
			if( hActor && m_hTargetActor != hActor )
			{
				if( m_nTargetEffectIndex > 0 )
				{
					if( m_hTargetActor )
					{
						m_hTargetActor->CmdRemoveStateEffectFromID( m_nServerGraphicBlowID );
					}
				}
				pMonsterActor->GetAIBase()->ChangeTarget( hActor , false );
				m_hTargetActor = hActor;

				std::string strEffect = FormatA( "%d" , m_nTargetEffectIndex );
				m_nServerGraphicBlowID = m_hTargetActor->CmdAddStateEffect( GetParentSkillInfo() , STATE_BLOW::BLOW_100, (int)(GetDurationTime() * 1000) , strEffect.c_str() );
			}
		}
	}
}

#endif




#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnForceAggroTargetBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	szNewValue = szOrigValue;
}

void CDnForceAggroTargetBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	szNewValue = szOrigValue;
}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
