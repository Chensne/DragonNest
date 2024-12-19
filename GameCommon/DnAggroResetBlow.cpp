#include "stdafx.h"
#include "DnAggroResetBlow.h"
#include "DnMonsterActor.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


CDnAggroResetBlow::CDnAggroResetBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor )
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_148;
	SetValue( szValue );
	m_fValue = (float)atof( szValue );

	// 0 : 어그로 리셋(다시 원래 어그로 값으로 복구 안됨)
	// 1 : 어그로 리셋(다시 원래 어그로 값으로 복구 됨)
	// 1인 경우 서버에서 어그로 없는 상태로 취급 하고, 상태효과 제거 되면 원래 상태로 취급 되므로
	// 어그로 변경 하지 않는다.
	// 0인 경우는 그냥 어그로 리셋 시키면 됨.
	m_iType = (int)m_fValue;
}

CDnAggroResetBlow::~CDnAggroResetBlow(void)
{

}

void CDnAggroResetBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
#ifdef _GAMESERVER
	if(m_hActor->IsMonsterActor() )
	{
		if (m_iType == 0)
			static_cast<CDnMonsterActor*>( m_hActor.GetPointer() )->ResetAggro( m_ParentSkillInfo.hSkillUser );
	}
	else
	{
		OutputDebug( "CDnAggroResetBlow::OnBegin - 몬스터 액터에게만 유효합니다. 대상이 잘못 지정됨.\n" );
		SetState( STATE_BLOW::STATE_END );
	}
#else
	if( m_hActor->IsMonsterActor() == false )
	{
		SetState( STATE_BLOW::STATE_END );
	}
#endif

	OutputDebug( "CDnAggroResetBlow::OnBegin\n");
}


void CDnAggroResetBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	CDnBlow::Process( LocalTime, fDelta );
}


void CDnAggroResetBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
#if defined(_GAMESERVER)
	if(m_hActor && m_hActor->IsMonsterActor() )
	{
		//Type : 0인 경우 끝날때도 어그로 리셋 시켜준다...
		if (m_iType == 0)
			static_cast<CDnMonsterActor*>( m_hActor.GetPointer() )->ResetAggro(m_ParentSkillInfo.hSkillUser);
	}
#endif // _GAMESERVER

	OutputDebug( "CDnAggroResetBlow::OnEnd\n");
}

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnAggroResetBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	szNewValue = szOrigValue;
}

void CDnAggroResetBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	szNewValue = szOrigValue;
}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
