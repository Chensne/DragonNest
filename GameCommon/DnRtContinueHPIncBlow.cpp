#include "StdAfx.h"
#include "DnRtContinueHPIncBlow.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

const LOCAL_TIME RT_HP_INC_INTERVAL = 2000;


CDnRtContinueHPIncBlow::CDnRtContinueHPIncBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor )
#ifdef _GAMESERVER
																							  , m_IntervalChecker( hActor, GetMySmartPtr() )
#endif
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_012;
	SetValue( szValue );

	std::vector<string> vlTokens;
	string strArgument( szValue );
	TokenizeA( strArgument, vlTokens, ";" );

	m_bShowValue = true;
	if( vlTokens.size() > 1 ) {
		m_fValue = (float)atof( vlTokens[0].c_str() );
		m_bShowValue = ( strstr( vlTokens[1].c_str(), "true" ) || strstr( vlTokens[1].c_str(), "1" ) ) ? true : false;
	}
	else 
	{
		m_fValue = (float)atof( szValue );
	}
}

CDnRtContinueHPIncBlow::~CDnRtContinueHPIncBlow(void)
{
}

void CDnRtContinueHPIncBlow::OnSetParentSkillInfo()
{

#if defined( _GAMESERVER )
	CalcHealValueLimit( m_StateBlow.emBlowIndex, m_fValue );
#endif

}


void CDnRtContinueHPIncBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
#ifdef _GAMESERVER
	m_IntervalChecker.OnBegin( LocalTime, RT_HP_INC_INTERVAL );
#endif

	OutputDebug( "CDnRtContinueHPIncBlow::OnBegin, Value:%2.2f (HP : %d ) \n", m_fValue, m_hActor->GetHP());
}

#ifdef _GAMESERVER
bool CDnRtContinueHPIncBlow::OnCustomIntervalProcess( void )
{
	// HP 회복 패킷 보냄. (음수일수도 있음)
	INT64 iMaxHP = m_hActor->GetMaxHP();
	INT64 iNowHP = m_hActor->GetHP();
	INT64 iDelta = INT64((float)iMaxHP*m_fValue);
	INT64 iResultHP = iNowHP+iDelta;

	if( iResultHP > iMaxHP )
	{
		iDelta = iMaxHP - iNowHP;
		iResultHP = iMaxHP;
	}
	else
	if( iResultHP < 0 )
	{
		iDelta = iDelta - iResultHP;
		iResultHP = 0;
	}

	if( 0 != iDelta )
	{
		m_hActor->SetHP( iResultHP );
		m_hActor->RequestHPMPDelta( CDnState::ElementEnum_Amount, iDelta, m_hActor->GetUniqueID(), false, m_bShowValue );

		if (m_hActor->IsDie())
			m_hActor->Die(m_ParentSkillInfo.hSkillUser);
	}
	
	return true;
}
#endif

void CDnRtContinueHPIncBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	CDnBlow::Process( LocalTime, fDelta );

#ifdef _GAMESERVER
	m_IntervalChecker.Process( LocalTime, fDelta );
#endif
}

void CDnRtContinueHPIncBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
#ifdef _GAMESERVER
	m_IntervalChecker.OnEnd( LocalTime, fDelta );
#endif

	OutputDebug( "CDnRtContinueHPIncBlow::OnEnd, Value:%2.2f (HP : %d ) \n", m_fValue, m_hActor->GetHP());
}

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnRtContinueHPIncBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
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

void CDnRtContinueHPIncBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
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