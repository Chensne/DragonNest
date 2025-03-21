#include "StdAfx.h"
#include "DnIntelligenceToPhysicAttackBlow.h"
#include "DnSkill.h"

#if defined(PRE_ADD_BUFF_STATE_LIMIT)
#include "DnBasicBlow.h"
#endif // PRE_ADD_BUFF_STATE_LIMIT

#if !defined( USE_BOOST_MEMPOOL )
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
#endif // #if !defined( USE_BOOST_MEMPOOL )

CDnIntelligenceToPhysicAttackBlow::CDnIntelligenceToPhysicAttackBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor )
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_165;
	SetValue( szValue );
	
	m_fValue = (float)atof( szValue );

#if defined(PRE_ADD_BUFF_STATE_LIMIT)
	m_pBasicBlow = NULL;
#else
	m_eRefreshState = CDnActorState::StateTypeEnum::ST_AttackP;
	m_bNeedRefreshState = false;
#endif // PRE_ADD_BUFF_STATE_LIMIT
}

CDnIntelligenceToPhysicAttackBlow::~CDnIntelligenceToPhysicAttackBlow(void)
{
#if defined(PRE_ADD_BUFF_STATE_LIMIT)
	SAFE_DELETE(m_pBasicBlow);
#endif // PRE_ADD_BUFF_STATE_LIMIT
}

void CDnIntelligenceToPhysicAttackBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
	__super::OnBegin(LocalTime, fDelta);

	if (!m_hActor)
		return;

	//힘
	int nIntelligence = m_hActor->GetIntelligence();
	int nPhysicAttack = (int)((float)nIntelligence * m_fValue);

#if defined(PRE_ADD_BUFF_STATE_LIMIT)
	
	char buff[64];
	sprintf_s(buff, "%d", nPhysicAttack);
	m_pBasicBlow = new CDnBasicBlow( m_hActor, buff );
	m_pBasicBlow->SetBlow( STATE_BLOW::BLOW_001 );		// 물리공격력 절대값.
	m_pBasicBlow->SetParentSkillInfo(&m_ParentSkillInfo);

	m_pBasicBlow->SetDurationTime(m_StateBlow.fDurationTime);
	m_pBasicBlow->OnBegin(LocalTime, fDelta);
	
#else
	m_State.SetAttackPMin( nPhysicAttack );
	m_State.SetAttackPMax( nPhysicAttack );
	m_eRefreshState = CDnActorState::StateTypeEnum::ST_AttackP;

	m_State.CalcValueType();
	m_hActor->AddBlowState( &m_State, GetAddBlowStateType() );

	m_bNeedRefreshState = (CDnActor::StateTypeEnum(-1) != m_eRefreshState);

	if( m_bNeedRefreshState )
	{
		// 임의로 설정한 상태효과는 부모스킬이 없다. post state 변경이 아니라 기본 state 변경이므로 감안해준다.
		// 부모 스킬이 있는 경우엔 패시브 스킬인지 구분해서 패시브인 경우엔 basic 스탯변경으로 감안해서 All Refresh 로 하고 나머지는 RefreshSkill 로 처리..
		if( NeedRefreshAll() )
		{
			m_hActor->RefreshState( CDnActorState::RefreshAll, m_eRefreshState );
		}
		else
		{
			m_hActor->RefreshState( CDnActorState::RefreshSkill, m_eRefreshState );
		}
	}
#endif // PRE_ADD_BUFF_STATE_LIMIT
}


void CDnIntelligenceToPhysicAttackBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	__super::Process(LocalTime, fDelta);
}


void CDnIntelligenceToPhysicAttackBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	__super::OnEnd(LocalTime, fDelta);

#if defined(PRE_ADD_BUFF_STATE_LIMIT)
	if (m_pBasicBlow)
		m_pBasicBlow->OnEnd(LocalTime, fDelta);
#else
	m_hActor->DelBlowState( &m_State );

	if( m_bNeedRefreshState )
	{
		// 패시브 스킬인지 구분해서 패시브인 경우엔 All Refresh 로 하고 나머지는 RefreshSkill 로 처리
		if( NeedRefreshAll() )
			m_hActor->RefreshState( CDnActorState::RefreshAll, m_eRefreshState );
		else
			m_hActor->RefreshState( CDnActorState::RefreshSkill, m_eRefreshState );
	}
#endif // PRE_ADD_BUFF_STATE_LIMIT
}


#if defined(PRE_ADD_BUFF_STATE_LIMIT)
#else
bool CDnIntelligenceToPhysicAttackBlow::NeedRefreshAll( void )
{
	// 임의로 설정한 상태효과는 부모스킬이 없다. post state 변경이 아니라 기본 state 변경이므로 감안해준다.
	// 부모 스킬이 있는 경우엔 패시브 스킬인지 구분해서 패시브인 경우엔 basic 스탯변경으로 감안해서 All Refresh 로 하고 나머지는 RefreshSkill 로 처리..
	return ( false == m_bHasParentSkill ||
		(CDnSkill::Passive == m_ParentSkillInfo.eSkillType && CDnSkill::Buff == m_ParentSkillInfo.eDurationType) );
}
#endif // PRE_ADD_BUFF_STATE_LIMIT

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnIntelligenceToPhysicAttackBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	//필요한 값 변수
	float fValue[2];

	//////////////////////////////////////////////////////////////////////////
	//첫번째 값
	fValue[0] = (float)atof( szOrigValue );
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	//두번째
	fValue[1] = (float)atof( szAddValue );
	//////////////////////////////////////////////////////////////////////////

	//두 값을 더한다.
	float fResultValue = fValue[0] + fValue[1];

	sprintf_s(szBuff, "%f", fResultValue);

	szNewValue = szBuff;
}

void CDnIntelligenceToPhysicAttackBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	//필요한 값 변수
	float fValue[2];

	//////////////////////////////////////////////////////////////////////////
	//첫번째 값
	fValue[0] = (float)atof( szOrigValue );
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	//두번째
	fValue[1] = (float)atof( szAddValue );
	//////////////////////////////////////////////////////////////////////////

	//값 계산
	float fResultValue = fValue[0] - fValue[1];

	sprintf_s(szBuff, "%f", fResultValue);

	szNewValue = szBuff;
}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW