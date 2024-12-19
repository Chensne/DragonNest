#include "StdAfx.h"
#include "DnRtContinueMPIncBlow.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

const LOCAL_TIME RT_MP_INC_INTERVAL = 2000;


CDnRtContinueMPIncBlow::CDnRtContinueMPIncBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor )
#ifdef _GAMESERVER
, m_IntervalChecker( hActor, GetMySmartPtr() )
#endif
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_014;
	SetValue( szValue );

	std::vector<string> vlTokens;
	string strArgument( szValue );
	TokenizeA( strArgument, vlTokens, ";" );

	m_bShowValue = false;
	if( vlTokens.size() > 1 ) {
		m_fValue = (float)atof( vlTokens[0].c_str() );
		m_bShowValue = ( strstr( vlTokens[1].c_str(), "true" ) || strstr( vlTokens[1].c_str(), "1" ) ) ? true : false;
	}
	else m_fValue = (float)atof( szValue );
}

CDnRtContinueMPIncBlow::~CDnRtContinueMPIncBlow(void)
{
}


void CDnRtContinueMPIncBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
#ifdef _GAMESERVER
	m_IntervalChecker.OnBegin( LocalTime, RT_MP_INC_INTERVAL );
#endif

	OutputDebug( "CDnRtContinueMPIncBlow::OnBegin, Value:%2.2f (MP : %d ) \n", m_fValue, m_hActor->GetSP());
}

bool CDnRtContinueMPIncBlow::OnCustomIntervalProcess( void )
{
#ifdef _GAMESERVER
	// MP 회복 패킷 보냄. (음수일수도 있음)
	int iMaxMP = m_hActor->GetMaxSP();
	int iNowMP = m_hActor->GetSP();
	int iDelta = int((float)iMaxMP*m_fValue);
	int iResultMP = iNowMP+iDelta;

	if( iResultMP > iMaxMP )
	{
		iDelta = iMaxMP - iNowMP;
		iResultMP = iMaxMP;
	}
	else
	if( iResultMP < 0 )
	{
		iDelta = iDelta - iResultMP;
		iResultMP = 0;
	}

	if( 0 != iDelta )
	{
		m_hActor->SetSP( iResultMP );
		m_hActor->RequestHPMPDelta( CDnState::ElementEnum_Amount, iDelta, m_hActor->GetUniqueID(), true, m_bShowValue );
	}

#endif
	return true;
}

void CDnRtContinueMPIncBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	CDnBlow::Process( LocalTime, fDelta );

#ifdef _GAMESERVER
	m_IntervalChecker.Process( LocalTime, fDelta );
#endif
}

void CDnRtContinueMPIncBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
#ifdef _GAMESERVER
	m_IntervalChecker.OnEnd( LocalTime, fDelta );
#endif

	OutputDebug( "CDnRtContinueMPIncBlow::OnEnd, Value:%2.2f (MP : %d ) \n", m_fValue, m_hActor->GetSP());
}

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnRtContinueMPIncBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	//파싱에 필요한 변수 선언
	std::vector<string> vlTokens[2];
	string strArgument[2];

	int nValueCount = 0;

	//필요한 값 변수
	float fValue[2];
	bool bShowValue[2];


	//////////////////////////////////////////////////////////////////////////
	//첫번째 문자열 파싱.
	strArgument[0] = szOrigValue;
	TokenizeA( strArgument[0], vlTokens[0], ";" );

	bShowValue[0] = true;
	if( vlTokens[0].size() > 1 ) {
		fValue[0] = (float)atof( vlTokens[0][0].c_str() );
		bShowValue[0] = ( strstr( vlTokens[0][1].c_str(), "true" ) || strstr( vlTokens[0][1].c_str(), "1" ) ) ? true : false;

		nValueCount = 2;
	}
	else 
	{
		fValue[0] = (float)atof( szOrigValue );

		nValueCount = 1;
	}
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	//두번째 문자열 파싱
	strArgument[1] = szOrigValue;
	TokenizeA( strArgument[1], vlTokens[1], ";" );

	bShowValue[1] = true;
	if( vlTokens[1].size() > 1 ) {
		fValue[1] = (float)atof( vlTokens[1][0].c_str() );
		bShowValue[1] = ( strstr( vlTokens[1][1].c_str(), "true" ) || strstr( vlTokens[1][1].c_str(), "1" ) ) ? true : false;
	}
	else 
	{
		fValue[1] = (float)atof( szOrigValue );
	}
	//////////////////////////////////////////////////////////////////////////

	//두 값을 더한다.
	float fResultValue = fValue[0] + fValue[1];
	bool fResultShow = (bShowValue[0] || bShowValue[1]);

	if (nValueCount == 2)
	{
		sprintf_s(szBuff, "%f;%s", fResultValue, fResultShow ? "true" : "false");
	}
	else
	{
		sprintf_s(szBuff, "%f", fResultValue);
	}

	szNewValue = szBuff;
}

void CDnRtContinueMPIncBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	//파싱에 필요한 변수 선언
	std::vector<string> vlTokens[2];
	string strArgument[2];

	int nValueCount = 0;

	//필요한 값 변수
	float fValue[2];
	bool bShowValue[2];


	//////////////////////////////////////////////////////////////////////////
	//첫번째 문자열 파싱.
	strArgument[0] = szOrigValue;
	TokenizeA( strArgument[0], vlTokens[0], ";" );

	bShowValue[0] = true;
	if( vlTokens[0].size() > 1 ) {
		fValue[0] = (float)atof( vlTokens[0][0].c_str() );
		bShowValue[0] = ( strstr( vlTokens[0][1].c_str(), "true" ) || strstr( vlTokens[0][1].c_str(), "1" ) ) ? true : false;

		nValueCount = 2;
	}
	else 
	{
		fValue[0] = (float)atof( szOrigValue );

		nValueCount = 1;
	}
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	//두번째 문자열 파싱
	strArgument[1] = szAddValue;
	TokenizeA( strArgument[1], vlTokens[1], ";" );

	bShowValue[1] = true;
	if( vlTokens[1].size() > 1 ) {
		fValue[1] = (float)atof( vlTokens[1][0].c_str() );
		bShowValue[1] = ( strstr( vlTokens[1][1].c_str(), "true" ) || strstr( vlTokens[1][1].c_str(), "1" ) ) ? true : false;
	}
	else 
	{
		fValue[1] = (float)atof( szOrigValue );
	}
	//////////////////////////////////////////////////////////////////////////

	//두 값을 계산.
	float fResultValue = fValue[0] - fValue[1];
	bool fResultShow = (bShowValue[0] || bShowValue[1]);

	if (nValueCount == 2)
	{
		sprintf_s(szBuff, "%f;%s", fResultValue, fResultShow ? "true" : "false");
	}
	else
	{
		sprintf_s(szBuff, "%f", fResultValue);
	}

	szNewValue = szBuff;
}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW