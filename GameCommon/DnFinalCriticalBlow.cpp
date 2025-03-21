#include "StdAfx.h"
#include "DnFinalCriticalBlow.h"

#ifdef _CLIENT
#include "DnInterface.h"
#endif

#if !defined( USE_BOOST_MEMPOOL )
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
#endif // #if !defined( USE_BOOST_MEMPOOL )


CDnFinalCriticalBlow::CDnFinalCriticalBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor )
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_251;
	SetValue( szValue );

	m_fValue = (float)atof(szValue);
}

CDnFinalCriticalBlow::~CDnFinalCriticalBlow(void)
{

}


void CDnFinalCriticalBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
	__super::OnBegin(LocalTime, fDelta);

#if defined(PRE_ADD_CHAR_STATUS_SECOND_RENEW) && defined (PRE_FIX_CHARSTATUS_REFRESH) && defined(_CLIENT)
	// 살짝 애매한 부분인데 CharStatus는 기본적으로 스텟이 갱신될때 표현을 해주는데 이상태효과같은경우는 스텟을 안건들이니 리프래쉬 기준을 잡을수없다.
	// 일단은 이런식으로 표현되는 상태효과는 잘없기때문에 이상태효과로 걸어두는데 나중에 이런경우가 많아지면 DnBlow레벨로 낮추어서 갱신하도록 설정해야할듯 싶습니다.
	if( CDnActor::s_hLocalActor && m_hActor == CDnActor::s_hLocalActor )
		GetInterface().OnRefreshLocalPlayerStatus();
#endif

}

void CDnFinalCriticalBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	__super::Process( LocalTime, fDelta );
}


void CDnFinalCriticalBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	__super::OnEnd(LocalTime, fDelta);

#if defined(PRE_ADD_CHAR_STATUS_SECOND_RENEW) && defined (PRE_FIX_CHARSTATUS_REFRESH) && defined(_CLIENT)
	if( m_hActor == CDnActor::s_hLocalActor )
		GetInterface().OnRefreshLocalPlayerStatus();
#endif

}

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnFinalCriticalBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	szNewValue = szOrigValue;
}

void CDnFinalCriticalBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	szNewValue = szOrigValue;
}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
