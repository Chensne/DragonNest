#include "StdAfx.h"
#include "DnChangeActionSetBlow.h"
#include "IDnSkillProcessor.h"
#include "DnChangeActionStrProcessor.h"
#include "DnApplySEWhenActionSetBlowEnabledProcessor.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


CDnChangeActionSetBlow::CDnChangeActionSetBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor ),
																							  m_pChangeActionStrProcessor( NULL ),
																							  m_bEnable( false ),
																							  m_bChangedActionBegun( false )
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_129;

	_ASSERT( strlen(szValue) < STATE_BLOW_ARGUMENT_MAX_SIZE );
	SetValue( szValue );

	// 인자값은 특정 액션을 취하고 있을 때만 액션 이름을 바꾸도록 하고 싶을 때..
	// 해당 액션 이름을 받아둔다.
	// 이 액션을 취하고 있을 때만 액션 대체가 CDnActor::SetActionQueue() 함수에서 일어난다.
	if( 0 < strlen(szValue) && (strcmp(szValue, "0") != 0) )
		m_strTriggerAction.assign( szValue );
}

CDnChangeActionSetBlow::~CDnChangeActionSetBlow(void)
{

}

void CDnChangeActionSetBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
	DnSkillHandle hParentSkill = m_ParentSkillInfo.hSkillUser->FindSkill( m_ParentSkillInfo.iSkillID );
	m_bEnable = false;
	m_strCancleAction.clear();
	_ASSERT( hParentSkill );
	if( hParentSkill )
	{
		m_pChangeActionStrProcessor = static_cast<CDnChangeActionStrProcessor*>( hParentSkill->GetProcessor( IDnSkillProcessor::CHANGE_ACTIONSTR ) );
		OutputDebug( "CDnChangeActionSetBlow::OnBegin\n" );
	}
	else
	{
		SetState( STATE_BLOW::STATE_END );
		OutputDebug( "CDnChangeActionSetBlow::OnBegin Fail!! Can't find Parent Skill Object!!\n" );
	}
}


void CDnChangeActionSetBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	CDnBlow::Process( LocalTime, fDelta );
}


void CDnChangeActionSetBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
#ifdef _GAMESERVER
	// #35037 상태효과 종료될 때 바로 부여된 상태효과들과 강제 속성 셋팅을 리셋해준다.
	OnNotChangeAction();
#endif // _GAMESERVER
	OutputDebug( "CDnChangeActionSetBlow::OnEnd\n" );
}


CDnChangeActionStrProcessor* CDnChangeActionSetBlow::GetChangeActionStrProcessor( void )
{
	if( m_bEnable )
	{
		// 덤프가 계속 발생해서 멤버변수 되신 실제 포인터를 리턴하게 변경해본다.
		CDnChangeActionStrProcessor* pProcessor = NULL;
		if( m_ParentSkillInfo.hSkillUser )
		{
			DnSkillHandle hParentSkill = m_ParentSkillInfo.hSkillUser->FindSkill( m_ParentSkillInfo.iSkillID );
			if( hParentSkill )
				pProcessor = static_cast<CDnChangeActionStrProcessor*>( hParentSkill->GetProcessor( IDnSkillProcessor::CHANGE_ACTIONSTR ) );
		}

		// 현재 Processor 는 없는데 멤버변수는 Valid 하다면 이게 맞는거임. 로그 박아봄
		if( pProcessor == NULL && m_pChangeActionStrProcessor )
		{
			g_Log.Log(LogType::_ERROR, L"CDnChangeActionSetBlow::GetChangeActionStrProcessor Invalid Call!!" );
		}

		return pProcessor;


		//return m_pChangeActionStrProcessor;
	}
	else
		return NULL;
}

void CDnChangeActionSetBlow::UpdateEnable( const char* pPrevAction, const char* pCurrentAction )
{
	if( strcmp(pPrevAction, pCurrentAction) == 0 )
		return;

	// #40128 부모스킬의 사용가능 무기 정보를 얻어와서 체크.
	if( m_ParentSkillInfo.hSkillUser )
	{
		DnSkillHandle hParentSkill = m_ParentSkillInfo.hSkillUser->FindSkill( m_ParentSkillInfo.iSkillID );
		if( hParentSkill && !hParentSkill->IsSatisfyWeapon() )
		{
			m_bEnable = false;
			return;
		}
	}

	if( m_strTriggerAction.empty() )
	{
		m_bEnable = true;
	}
	else
	if( pCurrentAction && 0 < strlen(pCurrentAction) &&
		pPrevAction && 0 < strlen(pPrevAction) )
	{
		// 현재 바뀐 액션이 트리거 액션과 같다면 활성화 시점.
		if( false == m_bEnable && m_strTriggerAction == pCurrentAction )
		{
			m_bEnable = true;
			m_strCancleAction = pPrevAction;
		}
		else
		if( m_strCancleAction == pCurrentAction )
		{
			m_bEnable = false;
		}
	}
}


#ifdef _GAMESERVER
void CDnChangeActionSetBlow::OnChangeAction( void )
{
	if( false == m_bChangedActionBegun )
	{
		// 액션 변경시에 상태효과 적용 발현타입이 있다면 함수 호출해줌.
		CDnApplySEWhenActionSetBlowEnabledProcessor* pProcessor = NULL;
		DnSkillHandle hParentSkill = m_ParentSkillInfo.hSkillUser->FindSkill( m_ParentSkillInfo.iSkillID );
		if( hParentSkill )
		{
			pProcessor = static_cast<CDnApplySEWhenActionSetBlowEnabledProcessor*>( hParentSkill->GetProcessor( IDnSkillProcessor::APPLY_SE_WHEN_ACTIONSET_ENABLED ) );
			if( pProcessor )
				pProcessor->OnChangeActionBegin();
		}

		m_bChangedActionBegun = true;
	}
}


void CDnChangeActionSetBlow::OnNotChangeAction( void )
{
	if( m_bChangedActionBegun )
	{
		// 액션 변경시에 상태효과 적용 발현타입이 있다면 함수 호출해줌.
		CDnApplySEWhenActionSetBlowEnabledProcessor* pProcessor = NULL;
		DnSkillHandle hParentSkill = m_ParentSkillInfo.hSkillUser->FindSkill( m_ParentSkillInfo.iSkillID );
		if( hParentSkill )
		{
			pProcessor = static_cast<CDnApplySEWhenActionSetBlowEnabledProcessor*>( hParentSkill->GetProcessor( IDnSkillProcessor::APPLY_SE_WHEN_ACTIONSET_ENABLED ) );
			if( pProcessor )
				pProcessor->OnChangeActionSetBlowEnd();
		}

		m_bChangedActionBegun = false;
	}
}
#endif

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnChangeActionSetBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	szNewValue = szOrigValue;
}

void CDnChangeActionSetBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	szNewValue = szOrigValue;
}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW