#include "StdAfx.h"
#include "DnChangeHealingRatioBlow.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


CDnChangeHealingRatioBlow::CDnChangeHealingRatioBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor )
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_141;

	// 이 상태효과는 따로 값을 쓰는 것은 없다..
	SetValue( szValue );
	m_fValue = (float)atof( szValue );

	// 실제로 이 클래스에서 하는 일은 없고, CDnActor::RequestHPMPDelta() 함수에서 
	// hp 가 회복될 때만 이 상태효과의 수치를 참고해서 처리한다.

}

CDnChangeHealingRatioBlow::~CDnChangeHealingRatioBlow(void)
{

}

void CDnChangeHealingRatioBlow::OnSetParentSkillInfo()
{

#if defined( _GAMESERVER )
	CalcHealValueLimit( m_StateBlow.emBlowIndex, m_fValue );
#endif

}

void CDnChangeHealingRatioBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{


	OutputDebug( "CDnChangeHealingRatioBlow::OnBegin Value:%2.2f\n", m_fValue );
}


void CDnChangeHealingRatioBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	CDnBlow::Process( LocalTime, fDelta );
}


void CDnChangeHealingRatioBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{


	OutputDebug( "CDnChangeHealingRatioBlow::OnEnd Value:%2.2f\n", m_fValue );
}

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnChangeHealingRatioBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
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

void CDnChangeHealingRatioBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
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