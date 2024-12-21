#include "StdAfx.h"
#include "DnChangeWeightBlow.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


CDnChangeWeightBlow::CDnChangeWeightBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor ), m_fWeightDelta( 0.0f )
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_131;

	SetValue( szValue );
	m_fValue = (float)atof( szValue );
}

CDnChangeWeightBlow::~CDnChangeWeightBlow(void)
{
}


void CDnChangeWeightBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
	// ���� ���԰��� �޾Ƶд�. 
	// �������� ����ȿ���� ����� �� �����Ƿ� ��Ÿ���� �޾Ƽ� ���߿� ���� ��Ų��.
	float fNowWeightValue = m_hActor->GetWeight();
	m_fWeightDelta = fNowWeightValue - m_fValue;

	// ���Ӱ� ���԰��� ����.
	m_hActor->SetWeight( m_fValue );

	OutputDebug( "CDnChangeWeightBlow::OnBegin Value: Now: %2.2f, Dest: %2.2f, Delta: %2.2f\n", fNowWeightValue, m_fValue, m_fWeightDelta );
}


void CDnChangeWeightBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	float fNowWeightValue = m_hActor->GetWeight();
	m_hActor->SetWeight( fNowWeightValue + m_fWeightDelta );

	OutputDebug( "CDnChangeWeightBlow::OnEnd Value: Now: %2.2f, Dest: %2.2f, Delta: %2.2f\n", fNowWeightValue, m_fValue, m_fWeightDelta );
}

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnChangeWeightBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	//�ʿ��� �� ����
	float fValue[2];

	//////////////////////////////////////////////////////////////////////////
	//ù��° ��
	fValue[0] = (float)atof( szOrigValue );
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	//�ι�°
	fValue[1] = (float)atof( szAddValue );
	//////////////////////////////////////////////////////////////////////////

	//�� ���� ���Ѵ�.
	float fResultValue = fValue[0] + fValue[1];

	sprintf_s(szBuff, "%f", fResultValue);

	szNewValue = szBuff;
}

void CDnChangeWeightBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	//�ʿ��� �� ����
	float fValue[2];

	//////////////////////////////////////////////////////////////////////////
	//ù��° ��
	fValue[0] = (float)atof( szOrigValue );
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	//�ι�°
	fValue[1] = (float)atof( szAddValue );
	//////////////////////////////////////////////////////////////////////////

	//�� ���
	float fResultValue = fValue[0] - fValue[1];

	sprintf_s(szBuff, "%f", fResultValue);

	szNewValue = szBuff;
}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW