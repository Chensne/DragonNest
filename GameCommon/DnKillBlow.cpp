#include "StdAfx.h"
#include "DnKillBlow.h"
#include "DnActor.h"

#ifdef _GAMESERVER
#include "DNUserSendManager.h"
#endif

#if !defined( USE_BOOST_MEMPOOL )
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
#endif // #if !defined( USE_BOOST_MEMPOOL )

CDnKillBlow::CDnKillBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor )
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_233;
	SetValue( szValue );
}

CDnKillBlow::~CDnKillBlow(void)
{
}

void CDnKillBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
#ifdef _GAMESERVER
	if( !m_hActor->IsDie() )
		m_hActor->RequestKillAfterProcessStateBlow(m_ParentSkillInfo.hSkillUser);

	SetState( STATE_BLOW::STATE_END );
#endif
}

void CDnKillBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	__super::OnEnd(LocalTime, fDelta);
}

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnKillBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{	
}

void CDnKillBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{	
}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
