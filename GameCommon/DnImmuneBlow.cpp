#include "StdAfx.h"
#include "DnImmuneBlow.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnImmuneBlow::CDnImmuneBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor ), 
																			m_fResistPercent( 0 )
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_077;
	SetValue( szValue );
	//m_nValue = atoi( szValue );

	m_fValue = 0.0f;
	m_fResistPercent = 0.0f;
	m_fReduceTimeValue = 1.0f;

	std::string str = szValue;
	std::vector<std::string> tokens;
	std::string delimiters = ";";

	TokenizeA(str, tokens, delimiters);
	if (tokens.size() >= 2)
	{
		m_fValue = (float)atof(tokens[0].c_str());
		m_fResistPercent = (float)atof(tokens[1].c_str());

		if (tokens.size() == 3)
			m_fReduceTimeValue = (float)atof(tokens[2].c_str()) * 0.01f;
	}
	else
	{
		OutputDebug("%s Invalidvalue.... %s\n", __FUNCTION__, szValue);
	}
}

CDnImmuneBlow::~CDnImmuneBlow(void)
{

}

void CDnImmuneBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{

}

void CDnImmuneBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	// 이 녀석은 면역 말고 하는 게 없음..	
	CDnBlow::Process( LocalTime, fDelta );
}

bool CDnImmuneBlow::IsImmuned( const CDnSkill::SkillInfo* pParentSkillInfo, STATE_BLOW::emBLOW_INDEX BlowIndex )
{
	if( !pParentSkillInfo || !m_hActor )
		return false;

	bool bResult = false;

#ifdef _GAMESERVER
	// 디버프인 상태효과만 면역 가능. (#17801) // #57038 parentSkill타입이 Buff이지만 Debuff를 거는경우도 생김. 팀체크 추가.
	bool bIsSameTeam = false;
	bIsSameTeam = pParentSkillInfo->iSkillUserTeam == m_hActor->GetTeam();

	if( CDnSkill::Debuff == pParentSkillInfo->eDurationType || !bIsSameTeam )
	{
		if( (int)m_fValue == BlowIndex )
		{
			int iProb = int(m_fResistPercent * 10000.0f);
			if( _rand(GetRoom()) % 1000000 <= iProb )
				bResult = true;
		}
	}
#endif

	return bResult;
};


void CDnImmuneBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{

}

void CDnImmuneBlow::OnDuplicate( const STATE_BLOW& StateBlowInfo )
{

}

bool CDnImmuneBlow::IsImmuned( STATE_BLOW::emBLOW_INDEX BlowIndex )
{
	bool bResult = false;

#ifdef _GAMESERVER
	if( (int)m_fValue == BlowIndex )
	{
		int iProb = int(m_fResistPercent * 10000.0f);
		if( _rand(GetRoom())%1000000 <= iProb )
			bResult = true;
	}
#endif

	return bResult;
};

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnImmuneBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	//파싱에 필요한 변수 선언
	std::vector<string> vlTokens[2];
	string strArgument[2];

	int nValueCount = 0;

	//필요한 값 변수
	float fValue[2] = {0.0f, };
	float fResistPercent[2] = {0.0f, };


	//////////////////////////////////////////////////////////////////////////
	//첫번째 문자열 파싱.
	strArgument[0] = szOrigValue;
	TokenizeA( strArgument[0], vlTokens[0], ";" );

	if( vlTokens[0].size() == 2 ) {
		fValue[0] = (float)atof( vlTokens[0][0].c_str() );
		fResistPercent[0] = (float)atof( vlTokens[0][1].c_str() );
	}
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	//두번째 문자열 파싱
	strArgument[1] = szAddValue;
	TokenizeA( strArgument[1], vlTokens[1], ";" );

	if( vlTokens[1].size() == 2 ) {
		fValue[1] = (float)atof( vlTokens[1][0].c_str() );
		fResistPercent[1] = (float)atof( vlTokens[1][1].c_str() );
	}
	//////////////////////////////////////////////////////////////////////////

	//두 값을 더한다.
	float fResultValue = fValue[0] + fValue[1];
	float fResultResistPercent = fResistPercent[0] + fResistPercent[1];

	sprintf_s(szBuff, "%f;%f", fResultValue, fResultResistPercent);
	
	szNewValue = szBuff;
}

void CDnImmuneBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	//파싱에 필요한 변수 선언
	std::vector<string> vlTokens[2];
	string strArgument[2];

	int nValueCount = 0;

	//필요한 값 변수
	float fValue[2] = {0.0f, };
	float fResistPercent[2] = {0.0f, };


	//////////////////////////////////////////////////////////////////////////
	//첫번째 문자열 파싱.
	strArgument[0] = szOrigValue;
	TokenizeA( strArgument[0], vlTokens[0], ";" );

	if( vlTokens[0].size() == 2 ) {
		fValue[0] = (float)atof( vlTokens[0][0].c_str() );
		fResistPercent[0] = (float)atof( vlTokens[0][1].c_str() );
	}
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	//두번째 문자열 파싱
	strArgument[1] = szAddValue;
	TokenizeA( strArgument[1], vlTokens[1], ";" );

	if( vlTokens[1].size() == 2 ) {
		fValue[1] = (float)atof( vlTokens[1][0].c_str() );
		fResistPercent[1] = (float)atof( vlTokens[1][1].c_str() );
	}
	//////////////////////////////////////////////////////////////////////////

	//두 값을 더한다.
	float fResultValue = fValue[0] - fValue[1];
	float fResultResistPercent = fResistPercent[0] - fResistPercent[1];

	sprintf_s(szBuff, "%f;%f", fResultValue, fResultResistPercent);

	szNewValue = szBuff;
}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
