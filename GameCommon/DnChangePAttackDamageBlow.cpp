#include "StdAfx.h"
#include "DnChangePAttackDamageBlow.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


CDnChangePAttackDamageBlow::CDnChangePAttackDamageBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor )
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_134;

	SetValue( szValue );
	m_fValue = (float)atof( szValue );
}

CDnChangePAttackDamageBlow::~CDnChangePAttackDamageBlow(void)
{
}

void CDnChangePAttackDamageBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
	OutputDebug( "CDnChangePAttackDamageBlow::OnBegin\n" );
}


void CDnChangePAttackDamageBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	CDnBlow::Process( LocalTime, fDelta );
}


void CDnChangePAttackDamageBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	OutputDebug( "CDnChangePAttackDamageBlow::OnEnd\n" );
}


#ifdef _GAMESERVER
void CDnChangePAttackDamageBlow::OnDuplicate( const STATE_BLOW& StateBlowInfo )
{
	// 시간은 넘어온 값으로 대체됨
	m_StateBlow.fDurationTime = StateBlowInfo.fDurationTime;

	const char* szTempValue = StateBlowInfo.szValue.c_str();
	SetValue( szTempValue );
	float fDuplicateValue = (float)atof( szTempValue );

	if( 1.0f < fDuplicateValue )
		fDuplicateValue -= 1.0f;

	m_fValue += fDuplicateValue;
}
#endif

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnChangePAttackDamageBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	//필요한 값 변수
	float fValue[2];

	//////////////////////////////////////////////////////////////////////////
	//첫번째 값
	fValue[0] = (float)atof( szOrigValue );
	//////////////////////////////////////////////////////////////////////////

	//수치가 0.75 --> 25%만 적용이 됨. 두번째 수치는 - 1.0을 해서 더해야 수치가 맞음
	//0.75, 0.8두 설정을 더할때
	//최종 값은 0.75 + (0.8 - 1.0) = 0.55
	//////////////////////////////////////////////////////////////////////////
	//두번째
	fValue[1] = (float)atof( szAddValue ) - 1.0f;
	//////////////////////////////////////////////////////////////////////////

	//두 값을 더한다.
	float fResultValue = fValue[0] + fValue[1];

	sprintf_s(szBuff, "%f", fResultValue);

	szNewValue = szBuff;
}

void CDnChangePAttackDamageBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
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