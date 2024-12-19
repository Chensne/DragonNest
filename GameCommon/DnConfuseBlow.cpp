#include "StdAfx.h"
#include "DnConfuseBlow.h"
#include "DnStateBlow.h"
#if defined(_CLIENT)
#include "InputDevice.h"
#include "DnLocalPlayerActor.h"
#endif // _CLIENT


#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnConfuseBlow::CDnConfuseBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor )
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_225;
	SetValue( szValue );

	m_OrigInverseInputMode = 0;

	//szValue값 설정
	m_fValue = (float)atof(szValue);

	//1 -> 키보드 & 마우스 입력 반전
	//2 -> 키보드 입력 반전
	//3 -> 마우스 입력 반전
}

CDnConfuseBlow::~CDnConfuseBlow(void)
{

}


void CDnConfuseBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
	__super::OnBegin(LocalTime, fDelta);

#if defined(_CLIENT)
	if (m_hActor && m_hActor->IsPlayerActor())
	{
		CDnLocalPlayerActor* pLocalPlayerActor = dynamic_cast<CDnLocalPlayerActor*>(m_hActor.GetPointer());
		if (pLocalPlayerActor)
		{
			CInputDevice::ms_InverseModeCount++;

			//원래 InverseMode값 저장 해놓고...
			m_OrigInverseInputMode = CInputDevice::GetInverseMode();
			CInputDevice::SetInverseMode((int)m_fValue);
		}
	}
#endif // _CLIENT

	OutputDebug( "%s\n", __FUNCTION__ );
}


void CDnConfuseBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	__super::Process( LocalTime, fDelta );
}


void CDnConfuseBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	__super::OnEnd(LocalTime, fDelta);

#if defined(_CLIENT)
	if (m_hActor && m_hActor->IsPlayerActor())
	{
		CDnLocalPlayerActor* pLocalPlayerActor = dynamic_cast<CDnLocalPlayerActor*>(m_hActor.GetPointer());
		if (pLocalPlayerActor)
		{
			//현재 모드값 가져 온다.
			int nCurrentMode = CInputDevice::GetInverseMode();

			//원래 저장시점의 Mode값이 현재 Mode값과 다르면 다른 상태효과에서 그 이전 값으로 돌려놨다.
			if (m_OrigInverseInputMode != 0 && nCurrentMode != m_OrigInverseInputMode)
				m_OrigInverseInputMode = nCurrentMode;

			//원래 InverseMode값으로 바꿔놓는다.
			CInputDevice::SetInverseMode(m_OrigInverseInputMode);

			CInputDevice::ms_InverseModeCount--;
		}
	}
#endif // _CLIENT

	OutputDebug( "%s\n", __FUNCTION__ );
}

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnConfuseBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };


	szNewValue = szOrigValue;
}

void CDnConfuseBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	szNewValue = szOrigValue;
}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW