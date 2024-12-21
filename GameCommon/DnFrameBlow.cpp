#include "StdAfx.h"
#include "DnFrameBlow.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnFrameBlow::CDnFrameBlow(DnActorHandle hActor, const char *szValue)
	: CDnBlow(hActor)
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_025;
	SetValue(szValue);
	m_fValue = (float)atof(szValue);
}

CDnFrameBlow::~CDnFrameBlow(void)
{
}

void CDnFrameBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
	m_hActor->UpdateFPS();

	OutputDebug( "CDnFrameBlow::OnBegin, Value:%f \n", m_fValue );
}

void CDnFrameBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{ 
	// ���ѽð��� ������ ���� ���� ��� STATE_DURATION ���� �Ǿ��ִ� ��찡 �־ UpdateFPS() ���� ��ȿ�� ����ȿ����
	// �з��Ǿ� ������ �ӵ����� ����Ǿ� ������ ��찡 ����.
	SetState( STATE_BLOW::STATE_END );
	
	m_hActor->UpdateFPS();

	OutputDebug( "CDnFrameBlow::OnEnd\n" );
}

void CDnFrameBlow::OnDuplicate( const STATE_BLOW& StateBlowInfo )
{
	m_hActor->UpdateFPS();
}


#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnFrameBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
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

void CDnFrameBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
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