#include "StdAfx.h"
#include "DnChangeMPConsumeRatioBlow.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnChangeMPConsumeRatioBlow::CDnChangeMPConsumeRatioBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor )
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_139;

	SetValue( szValue );
	m_fValue = (float)atof( szValue );

	// mp 소모시에 캐치해서 깍아준다.
	AddCallBackType( SB_ONUSEMP );
}

CDnChangeMPConsumeRatioBlow::~CDnChangeMPConsumeRatioBlow(void)
{

}

void CDnChangeMPConsumeRatioBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
	

	OutputDebug( "CDnChangeMPConsumeRatioBlow::OnBegin Value:%2.2f\n", m_fValue );
}


void CDnChangeMPConsumeRatioBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	CDnBlow::Process( LocalTime, fDelta );
}


void CDnChangeMPConsumeRatioBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	

	OutputDebug( "CDnChangeMPConsumeRatioBlow::OnEnd Value:%2.2f\n", m_fValue );
}

#ifdef _GAMESERVER
int CDnChangeMPConsumeRatioBlow::OnUseMP( int iMPDelta )
{
	// mp 소모는 서버에서 결정해서 쏴주도록 되어있으므로 여기서만 바꿔주면 클라이언트도 바뀐다.
	int iResult = int((float)iMPDelta * m_fValue);

	return iResult;
}
#endif 

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnChangeMPConsumeRatioBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
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

void CDnChangeMPConsumeRatioBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
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