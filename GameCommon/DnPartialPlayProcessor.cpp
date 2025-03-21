#include "StdAfx.h"
#include "DnActor.h"
#include "DnPartialPlayProcessor.h"
#include "DnStateBlow.h"
#if defined( PRE_FIX_71333 )
#include "DnMonsterActor.h"
#endif

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnPartialPlayProcessor::CDnPartialPlayProcessor()
: m_fMoveSpeed( 0.0f )
, m_iTime( 0 )
, m_iNowActionState( 0 )
, m_fStartActionLength( 0.0f )
, m_fLoopActionLength( 0.0f )
, m_fElapsedTime( 0.0f )
, m_fLoopActionElapsedTime( 0.0f )
, m_bPlayEnd( false )
#ifdef PRE_FIX_71333
, m_bLoopActionEnd( false )
#endif // PRE_FIX_71333
#ifdef PRE_FIX_68898
, m_isSkipEndAction( false )
#endif // PRE_FIX_68898
{
	m_iType = PARTIAL_PLAY_ANI; 
	m_fTimeLength = 0.0f;
}

CDnPartialPlayProcessor::CDnPartialPlayProcessor( DnActorHandle hActor, 
												  const char* pStartActionName, const char* pLoopActionName, 
												  const char* pEndActionName, 
												  int iTime, float fMoveSpeed ) : IDnSkillProcessor( hActor ),
																				  m_strStartActionName( pStartActionName ),
																				  m_strLoopActionName( pLoopActionName ),
																				  m_strEndActionName( pEndActionName ),
																				  m_iTime( iTime ),
																				  m_fMoveSpeed( fMoveSpeed ),
																				  //m_bStartedLastAction( false ),
																				  m_iNowActionState( -1 ),
																				  m_fLoopActionLength( 0.0f ),
																				  m_fElapsedTime( 0.0f ),
																				  m_fStartActionLength( 0.0f ),
																				  m_fLoopActionElapsedTime( 0.0f ),
																				  m_bPlayEnd( false )

#if defined(PRE_FIX_68898)
																				  , m_isSkipEndAction(false)
#endif // PRE_FIX_68898
#if defined( PRE_FIX_71333 )
																				  ,m_bLoopActionEnd(false)
#endif
{
	m_iType = PARTIAL_PLAY_ANI;
}

CDnPartialPlayProcessor::~CDnPartialPlayProcessor(void)
{
}


void CDnPartialPlayProcessor::_UpdatePlayTime( void )
{
	CEtActionBase::ActionElementStruct* pStartActionInfo = m_hHasActor->GetElement( m_strStartActionName.c_str() );
	CEtActionBase::ActionElementStruct* pEndActionInfo = m_hHasActor->GetElement( m_strEndActionName.c_str() );

	if( pStartActionInfo && pEndActionInfo )
	{
		// StartAction 은 현재 프레임에 맞춰서 끝까지 진행해야한다.
		float fFPS = m_hHasActor->CDnActionBase::GetFPS();
		m_fStartActionLength = (float)pStartActionInfo->dwLength / fFPS;
		m_fLoopActionLength = (float)m_iTime / 1000.0f;

		m_fTimeLength = m_fStartActionLength + m_fLoopActionLength + (float)pEndActionInfo->dwLength / fFPS;
	}
	else
	{
		// Note 한기: CBT 2차때 pStartActionInfo 를 찾지 못해 뻗은 케이스가 있어서 그런 상황에선 스킬을 아예 종료시킨다.
		// 일반적인 상황은 아니고 캐릭터가 유령이 되면 액션 셋이 바뀌기 때문에 액션을 못찾게 되는데,,
		// 남의 캐릭터가 스킬 썼다고 패킷이 왔을 당시 이미 남의 캐릭이 죽어 유령 상태인 
		// 경우(부활하지 못했을 수도. 아님 죽은 후 2초 정도 후에 스킬 쓴 패킷이 오거나.)가 되겠다.
		m_iNowActionState = END_ACTION;
		m_hHasActor->ReserveFinishSkill( m_hHasActor->GetProcessSkill() );

	}
}


void CDnPartialPlayProcessor::CopyFrom( IDnSkillProcessor* pProcessor )
{
	if( NULL == pProcessor )
		return;

	if( GetType() != pProcessor->GetType() )
		return;

	CDnPartialPlayProcessor* pSource = static_cast<CDnPartialPlayProcessor*>( pProcessor );
	m_strStartActionName = pSource->m_strStartActionName;
	m_strLoopActionName = pSource->m_strLoopActionName;
	m_strEndActionName = pSource->m_strEndActionName;
	m_fMoveSpeed = pSource->m_fMoveSpeed;
	m_iTime = pSource->m_iTime;

#if defined(PRE_FIX_68898)
	m_isSkipEndAction = pSource->m_isSkipEndAction;
#endif // PRE_FIX_68898
#if defined(PRE_FIX_71333 )
	if( m_hParentSkill && (m_hParentSkill->GetClassID() == 33175 || m_hParentSkill->GetClassID() == 33176) )
	{
		g_Log.Log(LogType::_PROFESSORK_SKILL, L"[CDnPartialPlayProcessor::CopyFrom -> Skill Probelm] SkillID:%d CurrentAction:%S Source->m_iTiem:%d\r\n", m_hParentSkill->GetClassID(), m_hHasActor->GetCurrentAction(), pSource->m_iTime);
	}
	if( pSource->m_iTime - m_iTime > 6000 )
	{
		g_Log.Log(LogType::_PROFESSORK_SKILL, L"[CDnPartialPlayProcessor::CopyFrom -> Time Problem] SkillID:%d CurrentAction:%S Source->m_iTiem:%d\r\n", m_hParentSkill->GetClassID(), m_hHasActor->GetCurrentAction(), pSource->m_iTime);
	}
#endif

}


void CDnPartialPlayProcessor::OnBegin( LOCAL_TIME LocalTime, float fDelta, DnSkillHandle hParentSkill )
{
	if( m_hHasActor )
	{
		m_iNowActionState = START_ACTION;
		m_fElapsedTime = 0.0f;
		m_fStartActionLength = 0.0f;
		m_fLoopActionLength = 0.0f;
		m_fLoopActionElapsedTime = 0.0f;
		m_bPlayEnd = false;

		m_hHasActor->SetMovable( false );
		m_hHasActor->SetActionQueue( m_strStartActionName.c_str() );

		_UpdatePlayTime();

#if defined(PRE_FIX_68898)
		m_isSkipEndAction = false;
#endif // PRE_FIX_68898
#if defined( PRE_FIX_71333 )
		m_bLoopActionEnd = false;
#endif
	}
}


void CDnPartialPlayProcessor::Process( LOCAL_TIME LocalTime, float fDelta )
{
	if( !m_hHasActor )
		return;

	if( m_bPlayEnd )
		return;

	// 시작과 끝 액션은 끝까지 해야하므로 프레임 변경에 의한 지속시간을 누적으로 처리해야한다.
	if( m_iNowActionState == START_ACTION ||
		m_iNowActionState == END_ACTION )
	{
		float fFPS = m_hHasActor->CDnActionBase::GetFPS();
		m_fElapsedTime += fDelta * (fFPS/60.0f);
	}
	else
	{
		m_fElapsedTime += fDelta;
	}

	_UpdatePlayTime();

	// 시작 액션이 끝나면 루핑 액션으로 넘어감
	switch( m_iNowActionState )
	{
		case START_ACTION:
			if( m_fElapsedTime >= m_fStartActionLength )
				m_iNowActionState = LOOP_ACTION;
			break;

		// 루핑 액션인 경우 Z 축으로 이동 시키고 끝나는 시점 찝어냄
		case LOOP_ACTION:
			//m_fLoopActionLength -= fDelta;
			//m_fLoopActionElapsedTime += fDelta * (fFPS/60.0f);
			m_fLoopActionElapsedTime += fDelta;		// 루프 액션은 기존대로 frame 변경에 대한 영향을 받지 않도록 처리. #
			
			// #24949
			if( 0 == m_hHasActor->GetCantXZMoveSEReferenceCount() )
			{
				m_hHasActor->MoveZ( m_fMoveSpeed * fDelta );
			}

			//if( m_fLoopActionLength <= 0.0f )
			if( m_fLoopActionLength < m_fLoopActionElapsedTime )
			{
				//m_fLoopActionLength = 0.0f;

				if( false == m_hHasActor->IsDie() )
				{
					m_hHasActor->SetMovable( false );
#if defined(PRE_FIX_68898)
#if defined( PRE_FIX_71333 )
					if(m_isSkipEndAction == false)
					{
						m_bLoopActionEnd = m_hHasActor->SetActionQueue( m_strEndActionName.c_str() );
#if defined( _GAMESERVER )
						if( !m_bLoopActionEnd )
						{
							if( m_hHasActor->IsMonsterActor() )
							{
								CDnMonsterActor* pMonster = (CDnMonsterActor *)m_hHasActor.GetPointer();
								if( pMonster->GetGrade() >= CDnMonsterState::Boss )
									g_Log.Log(LogType::_PROFESSORK_SKILL, L"[CDnPartialPlayProcessor::Process] Loop Action SetActionQueue Fail MonsterID:%d StartActionName:%S EndActionName:%S CurrentAction:%S \r\n", pMonster->GetMonsterClassID(), m_strStartActionName.c_str(), m_strEndActionName.c_str(), m_hHasActor->GetCurrentAction());
							}							
						}
#endif	// #if defined( _GAMESERVER )
					}	
					else
					{
						m_bLoopActionEnd = true;
#if defined( _GAMESERVER )
						if( m_hHasActor->IsMonsterActor() )
						{
							CDnMonsterActor* pMonster = (CDnMonsterActor *)m_hHasActor.GetPointer();
							g_Log.Log(LogType::_PROFESSORK_SKILL, L"[CDnPartialPlayProcessor::Process] m_isSkipEndAction Error MonsterID:%d StartActionName:%S EndActionName:%S CurrentAction:%S \r\n",pMonster->GetMonsterClassID(), m_strStartActionName.c_str(), m_strEndActionName.c_str(), m_hHasActor->GetCurrentAction());
						}
#endif	// #if defined( _GAMESERVER )
					}
#else	// #if defined( PRE_FIX_71333 )
					if (m_isSkipEndAction == false)
						m_hHasActor->SetActionQueue( m_strEndActionName.c_str() );
#endif	// #if defined( PRE_FIX_71333 )
#else	// #if defined(PRE_FIX_68898)
					m_hHasActor->SetActionQueue( m_strEndActionName.c_str() );
#endif // PRE_FIX_68898
					m_iNowActionState = END_ACTION;
				}
			}
			break;

		case END_ACTION:
			// 종료 액션 진행 중...
			if( m_fTimeLength < m_fElapsedTime )
			{
				m_bPlayEnd = true;
				m_fTimeLength = 0.0;
			}
			break;
	}

	// 공격이 끊겼을 경우엔 중단시킨다.
	// 스탠드 액션 변경 상태효과가 있으면 loop 중에 중지 되지 않는다.
	if( false == m_hHasActor->GetStateBlow()->IsApplied( STATE_BLOW::BLOW_121 ) )
	{
		if( false == m_hHasActor->IsAttack() )
		{
			m_bPlayEnd = true;
			m_fTimeLength = 0.0f;
		}
	}
}



void CDnPartialPlayProcessor::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	// 중간에 끊기는 경우 라스트 액션 그냥 재생해 줌.
	if( END_ACTION != m_iNowActionState )
	{
		if( m_hHasActor && !m_hHasActor->IsDie() )
		{
			// 행동 불가일 때는 공격, 스킬사용 못함
			// 따라서 여기서 임시로 해제 했다가 다시 걸어주도록 하자.
			bool bCantAction = false;
			if( (m_hHasActor->GetStateEffect() & CDnActorState::Cant_AttackAction) == CDnActorState::Cant_AttackAction ) 
			{
				bCantAction = true;
				m_hHasActor->SetStateEffect( m_hHasActor->GetStateEffect() & ~CDnActorState::Cant_AttackAction );
			}

			// 맞았을 때는 그냥 히트 액션 실행
			if( false == m_hHasActor->IsHit() )
			{
				m_hHasActor->SetMovable( false );
#if defined(PRE_FIX_68898)
#if defined( PRE_FIX_71333 )
				if (m_isSkipEndAction == false)
				{
					m_bLoopActionEnd = m_hHasActor->SetActionQueue( m_strEndActionName.c_str() );
					if( !m_bLoopActionEnd )
					{
						// 루프 액션이 끝나고 엔드액션을 넣을때 어떤 이유로든 실패가 되면 루프가 안끝나게 되므로 강제로 엔드액션을 실행시킴
						m_hHasActor->SetAction( m_strEndActionName.c_str(), 0, 3 );		
					}
#if defined( _GAMESERVER )
					if( !m_bLoopActionEnd )
					{
						if( m_hHasActor->IsMonsterActor() )
						{
							CDnMonsterActor* pMonster = (CDnMonsterActor *)m_hHasActor.GetPointer();
							if( pMonster->GetGrade() >= CDnMonsterState::Boss )
								g_Log.Log(LogType::_PROFESSORK_SKILL, L"[CDnPartialPlayProcessor::OnEnd] End Action SetActionQueue Fail MonsterID:%d StartActionName:%S EndActionName:%S CurrentAction:%S \r\n", pMonster->GetMonsterClassID(), m_strStartActionName.c_str(), m_strEndActionName.c_str(), m_hHasActor->GetCurrentAction());
						}
						
					}
#endif	// #if defined( _GAMESERVER )
				}
#if defined( _GAMESERVER )
				else
				{
					if( m_hHasActor->IsMonsterActor() )
					{
						CDnMonsterActor* pMonster = (CDnMonsterActor *)m_hHasActor.GetPointer();
						g_Log.Log(LogType::_PROFESSORK_SKILL, L"[CDnPartialPlayProcessor::Process] m_isSkipEndAction Error MonsterID:%d StartActionName:%S EndActionName:%S CurrentAction:%S \r\n",pMonster->GetMonsterClassID(), m_strStartActionName.c_str(), m_strEndActionName.c_str(), m_hHasActor->GetCurrentAction());
					}
				}				
#endif	// #if defined( _GAMESERVER )
#else
				if (m_isSkipEndAction == false)
					m_hHasActor->SetActionQueue( m_strEndActionName.c_str() );

				m_isSkipEndAction = false;
#endif
#else
				m_hHasActor->SetActionQueue( m_strEndActionName.c_str() );
#endif // PRE_FIX_68898
			}

			if( bCantAction )
			{
				m_hHasActor->SetStateEffect( m_hHasActor->GetStateEffect() | CDnActorState::Cant_AttackAction );
			}
		}
	}

#if defined( PRE_FIX_71333 )
	if( END_ACTION == m_iNowActionState )
	{
		if( !m_bLoopActionEnd )
		{
			// 루프 액션이 끝나고 엔드액션을 넣을때 어떤 이유로든 실패가 되면 루프가 안끝나게 되므로 강제로 엔드액션을 실행시킴

			m_hHasActor->SetAction( m_strEndActionName.c_str(), 0, 3 );		
		}
	}
	m_bLoopActionEnd = false;
#endif	
}