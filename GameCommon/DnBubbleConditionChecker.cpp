#include "StdAfx.h"
#include "DnBubbleConditionChecker.h"
#include "DnObserverEventMessage.h"
#include "DnObservable.h"
#include "DnBubbleSystem.h"
#include "DNGameRoom.h"
#include "DnActor.h"

namespace BubbleSystem
{

// 각 조건 체크 타입별로 객체 생성.
IDnConditionChecker* IDnConditionChecker::Create( int iConditionCheckerType, const char* pArgument )
{
	IDnConditionChecker* pConditionChecker = NULL;

	switch( iConditionCheckerType )
	{
		case BUBBLE_CONDITION::ON_USE_SKILL:
			pConditionChecker = new CDnUseSkill;
			break;

		case BUBBLE_CONDITION::BLOCK_SUCCESS:
			pConditionChecker = new CDnBlockSuccess;
			break;
		
		case BUBBLE_CONDITION::BUBBLE_COUNT_GREATER:
			pConditionChecker = new CDnBubbleCountGreater;
			break;

		case BUBBLE_CONDITION::BUBBLE_COUNT_UPDATED:
			pConditionChecker = new CDnBubbleCountUpdated;
			break;

		case BUBBLE_CONDITION::OWN_THIS_SKILL:
			pConditionChecker = new CDnOwnThisSkill;
			break;

		case BUBBLE_CONDITION::PARRING_SUCCESS:
			pConditionChecker = new CDnParringSuccess;
			break;

		case BUBBLE_CONDITION::COOLTIME_PARRING_SUCCESS:
			pConditionChecker = new CDnCooltimeParringSuccess;
			break;

		case BUBBLE_CONDITION::DO_NORMAL_ATTACK:
			pConditionChecker = new CDnDoNormalAttack;
			break;

		case BUBBLE_CONDITION::PLAYER_KILL_TARGET_ON_GHOUL_MODE:
			pConditionChecker = new CDnPlayerKillTargetOnGhoulMode;
			break;

		case BUBBLE_CONDITION::ON_USE_SKILL_WITH_SPECIFIC_SKILLLEVEL:
			pConditionChecker = new CDnUseSkillWithSpecificSkillLevel;
			break;

		case BUBBLE_CONDITION::PROBABILITY:
			pConditionChecker = new CDnProbability;
			break;

		//Master Dark Avenger incrase bubble on critical damage !
		case BUBBLE_CONDITION::ONCRITICALHIT:
			pConditionChecker = new CDnOnCriticalHit;
			break;

		default:
			return NULL;
	}

	pConditionChecker->Initialize( pArgument );

	return pConditionChecker;
}

// CDnUseSkill 
void CDnUseSkill::Initialize( const char* pArgument )
{
	if( NULL == pArgument )
		return;

	m_iSkillID = atoi( pArgument );
}

bool CDnUseSkill::IsSatisfy( CDnBubbleSystem* /*pBubbleSystem*/, ::IDnObserverNotifyEvent* pEvent )
{
	bool bResult = false;

	if( EVENT_BUBBLE_ON_USE_SKILL == pEvent->GetEventType() )
	{
		// 이 조건 체크 객체에서 요구하는 스킬 ID 와 일치함.
		if( static_cast<CDnUseSkillMessage*>( pEvent )->GetSkillID() == m_iSkillID )
		{
			bResult = true;
		}
	}

	return bResult;
}
//////////////////////////////////////////////////////////////////////////


// CDnBlockSuccess
void CDnBlockSuccess::Initialize( const char* pArgument )
{
	if( NULL == pArgument )
		return;

	m_iSkillID = atoi( pArgument );
}

bool CDnBlockSuccess::IsSatisfy( CDnBubbleSystem* /*pBubbleSystem*/, ::IDnObserverNotifyEvent* pEvent )
{
	bool bResult = false;

	// 블록 성공 메시지이고,
	if( EVENT_BUBBLE_BLOCK_SUCCESS == pEvent->GetEventType() )
	{
		// 이 조건 체크 객체에서 요구하는 스킬 ID 와 일치함.
		if( static_cast<CDnBlockSuccessMessage*>( pEvent )->GetSkillID() == m_iSkillID )
		{
			bResult = true;
		}
	}

	return bResult;
}
//////////////////////////////////////////////////////////////////////////


// CDnBubbleCountGreater
void CDnBubbleCountGreater::Initialize( const char* pArgument )
{
	if( NULL == pArgument )
		return;

	// 스트링을 공백단위로 분할한 후
	std::vector<string> vlTokens;
	string strArgument( pArgument );
	TokenizeA( strArgument, vlTokens, ";" );

	_ASSERT( 2 == (int)vlTokens.size() );

	m_iBubbleTypeID = atoi( vlTokens.at(0).c_str() );
	m_iBasisCount = atoi( vlTokens.at(1).c_str() );
}

bool CDnBubbleCountGreater::IsSatisfy( CDnBubbleSystem* pBubbleSystem, ::IDnObserverNotifyEvent* /*pEvent*/ )
{
	bool bResult = false;

	// 특정 버블의 갯수가 이 갯수 이상일 때.
	if( m_iBasisCount <= pBubbleSystem->GetBubbleCountByTypeID( m_iBubbleTypeID ) )
	{
		bResult = true;
	}

	return bResult;
}
//////////////////////////////////////////////////////////////////////////

// CDnBubbleCountUpdated
void CDnBubbleCountUpdated::Initialize( const char* pArgument )
{
	if( NULL == pArgument )
		return;

	m_iBubbleTypeID = atoi( pArgument );
}

bool CDnBubbleCountUpdated::IsSatisfy( CDnBubbleSystem* /*pBubbleSystem*/, ::IDnObserverNotifyEvent* pEvent )
{
	bool bResult = false;

	if( EVENT_BUBBLE_COUNT_UPDATED == pEvent->GetEventType() )
	{
		// 이 조건 체크 객체에서 요구하는 스킬 ID 와 일치함.
		if( static_cast<CDnBubbleCountUpdatedMessage*>( pEvent )->GetBubbleTypeID() == m_iBubbleTypeID )
		{
			bResult = true;
		}
	}

	return bResult;
}
//////////////////////////////////////////////////////////////////////////

// CDnOwnThisSkill
void CDnOwnThisSkill::Initialize( const char* pArgument )
{
	if( NULL == pArgument )
		return;

	m_iSkillID = atoi( pArgument );
}

bool CDnOwnThisSkill::IsSatisfy( CDnBubbleSystem* pBubbleSystem, ::IDnObserverNotifyEvent* pEvent )
{
	bool bResult = false;

	// 이 조건 체크 객체에서 요구하는 스킬 ID 와 일치함.
	DnActorHandle hActor = pBubbleSystem->GetHasActor();
	if( hActor && hActor->FindSkill( m_iSkillID ) )
	{
		bResult = true;
	}

	return bResult;
}
//////////////////////////////////////////////////////////////////////////

// CDnParringSuccess
void CDnParringSuccess::Initialize( const char* pArgument )
{
	if( NULL == pArgument )
		return;

	m_iSkillID = atoi( pArgument );
}

bool CDnParringSuccess::IsSatisfy( CDnBubbleSystem* /*pBubbleSystem*/, ::IDnObserverNotifyEvent* pEvent )
{
	bool bResult = false;

	// 블록 성공 메시지이고,
	if( EVENT_BUBBLE_PARRING_SUCCESS == pEvent->GetEventType() )
	{
		// 이 조건 체크 객체에서 요구하는 스킬 ID 와 일치함.
		if( static_cast<CDnParringSuccessMessage*>( pEvent )->GetSkillID() == m_iSkillID )
		{
			bResult = true;
		}
	}

	return bResult;
}
//////////////////////////////////////////////////////////////////////////

// CDnCooltimeParringSuccess
void CDnCooltimeParringSuccess::Initialize( const char* pArgument )
{
	if( NULL == pArgument )
		return;

	m_iSkillID = atoi( pArgument );
}

bool CDnCooltimeParringSuccess::IsSatisfy( CDnBubbleSystem* /*pBubbleSystem*/, ::IDnObserverNotifyEvent* pEvent )
{
	bool bResult = false;

	// 블록 성공 메시지이고,
	if( EVENT_BUBBLE_COOLTIME_PARRING_SUCCESS == pEvent->GetEventType() )
	{
		// 이 조건 체크 객체에서 요구하는 스킬 ID 와 일치함.
		if( static_cast<CDnCooltimeParringSuccessMessage*>( pEvent )->GetSkillID() == m_iSkillID )
		{
			bResult = true;
		}
	}

	return bResult;
}
//////////////////////////////////////////////////////////////////////////

// CDnCooltimeParringSuccess
void CDnDoNormalAttack::Initialize( const char* pArgument )
{
	// 인자 받는 것 없음.
	if( NULL == pArgument )
		return;

}

bool CDnDoNormalAttack::IsSatisfy( CDnBubbleSystem* pBubbleSystem, ::IDnObserverNotifyEvent* pEvent )
{
	bool bResult = false;

	DnActorHandle hActor = pBubbleSystem->GetHasActor();
	if( EVENT_ONCHANGEACTION == pEvent->GetEventType() )
	{
		// TODO: 평타 구분하는 코드가 여기 말고 두 군데 정도 더 있는데 함수로 빼자.
		if( false == hActor->IsProcessSkill() &&
			false == hActor->IsAppliedThisStateBlow(STATE_BLOW::BLOW_121) &&
			false == hActor->IsAppliedThisStateBlow(STATE_BLOW::BLOW_129) )
		{
			bResult = true;
		}
	}

	return bResult;
}
//////////////////////////////////////////////////////////////////////////

// CDnCooltimeParringSuccess
void CDnPlayerKillTargetOnGhoulMode::Initialize( const char* pArgument )
{
	// 인자 받는 것 없음.
	if( NULL == pArgument )
		return;

}

bool CDnPlayerKillTargetOnGhoulMode::IsSatisfy( CDnBubbleSystem* pBubbleSystem, ::IDnObserverNotifyEvent* pEvent )
{
	bool bResult = false;

	DnActorHandle hActor = pBubbleSystem->GetHasActor();
	if( EVENT_PLAYER_KILL_TARGET == pEvent->GetEventType() )
	{

		// 구울 모드면 true.
		if(hActor && hActor->GetGameRoom()->bIsZombieMode())
			bResult = true;
	}

	return bResult;
}
/////////////////////////////////////////////////////////////////////////

// CDnUseSkillWithSpecificSkillLevel 
void CDnUseSkillWithSpecificSkillLevel::Initialize( const char* pArgument )
{
	if( NULL == pArgument )
		return;

	vector<string> vlArgs;
	TokenizeA( pArgument, vlArgs, ";" );

	if( 2 == (int)vlArgs.size() )
	{
		m_iSkillID = atoi( vlArgs.at( 0 ).c_str() );
		m_iSkillLevel = atoi( vlArgs.at( 1 ).c_str() );
	}
}

bool CDnUseSkillWithSpecificSkillLevel::IsSatisfy( CDnBubbleSystem* /*pBubbleSystem*/, ::IDnObserverNotifyEvent* pEvent )
{
	bool bResult = false;

	if( EVENT_BUBBLE_ON_USE_SKILL == pEvent->GetEventType() )
	{
		// 이 조건 체크 객체에서 요구하는 스킬 ID 와 일치함.
		CDnUseSkillMessage* pMsg = static_cast<CDnUseSkillMessage*>( pEvent );
		if( pMsg->GetSkillID() == m_iSkillID &&
			pMsg->GetSkillLevel() == m_iSkillLevel )
		{
			bResult = true;
		}
	}

	return bResult;
}
//////////////////////////////////////////////////////////////////////////

// CDnProbability 
void CDnProbability::Initialize( const char* pArgument )
{
	if( NULL == pArgument )
		return;
	
	// 0.0 ~ 1.0 범위
	m_fProbability = (float)atof( pArgument );
	m_fProbability *= 10000.0f;
}

bool CDnProbability::IsSatisfy( CDnBubbleSystem* pBubbleSystem, ::IDnObserverNotifyEvent* /*pEvent*/ )
{
	bool bResult = false;

	float fRandomResult = float(_rand(pBubbleSystem->GetHasActor()->GetRoom()) % 10000);
	bResult = fRandomResult <= m_fProbability;

	return bResult;
}
//////////////////////////////////////////////////////////////////////////



// CDnProbability 
void CDnOnCriticalHit::Initialize(const char* pArgument)
{
	if (NULL == pArgument)
		return;
	
	m_vecCheckSkills.clear();

	std::vector<std::string> tempVecSkillList;
	TokenizeA(pArgument, tempVecSkillList, ";");

	if (tempVecSkillList.size() == 0)
		return;

	for (DWORD i = 0; i < tempVecSkillList.size(); i++)
		m_vecCheckSkills.push_back(atoi(tempVecSkillList[i].c_str()));

}

bool CDnOnCriticalHit::IsSatisfy(CDnBubbleSystem* pBubbleSystem, ::IDnObserverNotifyEvent* pEvent)
{
	bool bResult = false;

	if (EVENT_ONCRITICALHIT == pEvent->GetEventType())
	{
		CDnOnCriticalHitMessage* pMsg = static_cast<CDnOnCriticalHitMessage*>(pEvent);
		if (!pMsg)
			return false;

		int nSkillID = pMsg->GetSkillID();
		printf("%s - Before PMSG UsageSkill: %d \n", __FUNCTION__, nSkillID);
		if(nSkillID > 0)
		{
			if (std::find(m_vecCheckSkills.begin(), m_vecCheckSkills.end(), nSkillID) != m_vecCheckSkills.end())
			{
				printf("%s - OK ADD BUBBLES skill = %d \n",__FUNCTION__, nSkillID);
				bResult = true;
			}
		}
	}

	return bResult;
}

//////////////////////////////////////////////////////////////////////////


}