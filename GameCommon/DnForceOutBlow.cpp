#include "StdAfx.h"
#include "DnForceOutBlow.h"

#include "DnSkill.h"
#ifdef _GAMESERVER
#include "DnHighlanderBlow.h"
#endif 

#if !defined( USE_BOOST_MEMPOOL )
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
#endif // #if !defined( USE_BOOST_MEMPOOL )

CDnForceOutBlow::CDnForceOutBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor )
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_204;
	SetValue( szValue );
	
	m_fValue = (float)atof(szValue);

#if defined(_GAMESERVER)
	AddCallBackType( SB_ONTARGETHIT );
#endif // _GAMESERVER
}

CDnForceOutBlow::~CDnForceOutBlow(void)
{

}


void CDnForceOutBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
	__super::OnBegin(LocalTime, fDelta);
}

void CDnForceOutBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	__super::Process( LocalTime, fDelta );
	
}


void CDnForceOutBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	__super::OnEnd(LocalTime, fDelta);

}


#if defined(_GAMESERVER)

void CDnForceOutBlow::OnTargetHit( DnActorHandle hTargetActor )
{
	if (!hTargetActor)
		return;

	if (hTargetActor->IsImmuned(m_StateBlow.emBlowIndex))
		return;

	//무게가 더 무거우면 스킵...
	float fTargetWeight = hTargetActor->GetWeight();
	if (fTargetWeight > m_fValue)
		return;

	hTargetActor->CheckDamageVelocity(m_hActor);

}

#endif

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnForceOutBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
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

void CDnForceOutBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
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
