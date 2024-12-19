#include "StdAfx.h"
#include "DnAddStateOnHitBlow.h"
#if defined( _GAMESERVER )
#include "DnPlayerActor.h"
#endif // #if defined( _GAMESERVER )

#if !defined( USE_BOOST_MEMPOOL )
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
#endif // #if !defined( USE_BOOST_MEMPOOL )



CDnAddStateOnHitBlow::CDnAddStateOnHitBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor )
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_178;
	SetValue( szValue );
	
	SetInfo(szValue);

#ifdef _GAMESERVER
	AddCallBackType( SB_ONCALCDAMAGE );

	m_bCalcProb = false;
	m_bOnDamaged = false;
#endif
}

CDnAddStateOnHitBlow::~CDnAddStateOnHitBlow(void)
{

}



void CDnAddStateOnHitBlow::OnBegin(LOCAL_TIME LocalTime, float fDelta )
{
	__super::OnBegin(LocalTime, fDelta);
	OutputDebug( "CDnBlockBlow::OnBegin, Value:%d \n", (int)m_fValue );
}



void CDnAddStateOnHitBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	__super::Process( LocalTime, fDelta );

#if defined(_GAMESERVER)
	m_bCalcProb = m_bOnDamaged = false;
#endif // _GAMESERVER
}


void CDnAddStateOnHitBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	__super::OnEnd(LocalTime, fDelta);

}

void CDnAddStateOnHitBlow::SetInfo(const char* szValue)
{
	std::string str = szValue;//"[확률;DestStateIndex;상태효과지속시간][상태효과설정값]";
	std::vector<std::string> tokens;
	std::string delimiters = "[]";

	//1. 구분
	TokenizeA(str, tokens, delimiters);

	if (tokens.size() != 2)
	{
		OutputDebug("%s :: %s --> 상태효과 정보 점검 필요!!!\n", __FUNCTION__, szValue);
	}
	else
	{
		//2. 기본 정보
		std::string defaultInfo = tokens[0];
		std::vector<std::string> infoTokens;
		delimiters = ";";

		TokenizeA(defaultInfo, infoTokens, delimiters);
		if (3 == infoTokens.size())
		{
			m_fRate = m_fValue = (float)atof(infoTokens[0].c_str());
			m_nDestStateBlowIndex = atoi(infoTokens[1].c_str());
			m_nStateDurationTime = atoi(infoTokens[2].c_str());
		}
		else
			OutputDebug("%s :: %s --> 상태효과 정보 점검 필요!!!\n", __FUNCTION__, szValue);

		//3. 상태효과 설정값
		m_strStateAttribute = tokens[1];
	}
}


#ifdef _GAMESERVER
bool CDnAddStateOnHitBlow::CalcProb()
{
	bool bExecuteable = false;
	bExecuteable = rand() % 10000 <= (m_fRate * 10000.0f);
	return bExecuteable;
}

float CDnAddStateOnHitBlow::OnCalcDamage( float fOriginalDamage, CDnDamageBase::SHitParam& HitParam )
{
	if( fOriginalDamage == 0.f )
		return 0.0f;

	if (false == m_bCalcProb)
	{
		m_bOnDamaged = CalcProb();
		m_bCalcProb = true;
	}

	if (m_bOnDamaged)
	{
		m_hActor->CmdAddStateEffect(&m_ParentSkillInfo, (STATE_BLOW::emBLOW_INDEX)m_nDestStateBlowIndex, m_nStateDurationTime, m_strStateAttribute.c_str());
	}

	return 0.0f;
}
#endif

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnAddStateOnHitBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	std::string strValue[2];
	std::vector<std::string> tokens[2];
	std::string delimiters = "[]";


	float fRate[2] = {0.0f, };
	int nDestStateBlowIndex[2] = {0, };
	int nStateDurationTime[2] = {0, };
	std::string szArgment[2];


	strValue[0] = szOrigValue;
	//1. 구분
	TokenizeA(strValue[0], tokens[0], delimiters);

	if (tokens[0].size() != 2)
	{
		OutputDebug("%s :: %s --> 상태효과 정보 점검 필요!!!\n", __FUNCTION__, szOrigValue);
	}
	else
	{
		//2. 기본 정보
		std::string defaultInfo = tokens[0][0];
		std::vector<std::string> infoTokens;
		delimiters = ";";

		TokenizeA(defaultInfo, infoTokens, delimiters);
		if (3 == infoTokens.size())
		{
			fRate[0] = (float)atof(infoTokens[0].c_str());
			nDestStateBlowIndex[0] = atoi(infoTokens[1].c_str());
			nStateDurationTime[0] = atoi(infoTokens[2].c_str());
		}
		else
			OutputDebug("%s :: %s --> 상태효과 정보 점검 필요!!!\n", __FUNCTION__, szOrigValue);

		//3. 상태효과 설정값
		szArgment[0] = tokens[0][1];
	}

	//////////////////////////////////////////////////////////////////////////
	strValue[1] = szAddValue;
	//1. 구분
	TokenizeA(strValue[1], tokens[1], delimiters);

	if (tokens[1].size() != 2)
	{
		OutputDebug("%s :: %s --> 상태효과 정보 점검 필요!!!\n", __FUNCTION__, szAddValue);
	}
	else
	{
		//2. 기본 정보
		std::string defaultInfo = tokens[1][0];
		std::vector<std::string> infoTokens;
		delimiters = ";";

		TokenizeA(defaultInfo, infoTokens, delimiters);
		if (3 == infoTokens.size())
		{
			fRate[1] = (float)atof(infoTokens[0].c_str());
			nDestStateBlowIndex[1] = atoi(infoTokens[1].c_str());
			nStateDurationTime[1] = atoi(infoTokens[2].c_str());
		}
		else
			OutputDebug("%s :: %s --> 상태효과 정보 점검 필요!!!\n", __FUNCTION__, szAddValue);

		//3. 상태효과 설정값
		szArgment[1] = tokens[1][1];
	}


	float fResultRate = fRate[0] + fRate[1];
	int nResultDestStateBlowIndex = nDestStateBlowIndex[0];
	int nResultStateDurationTime = max(nStateDurationTime[0], nStateDurationTime[1]);

	sprintf_s(szBuff, "[%f;%d;%d][%s]", fResultRate, nResultDestStateBlowIndex, nResultStateDurationTime, szArgment[0].c_str());

	szNewValue = szBuff;
}

void CDnAddStateOnHitBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	std::string strValue[2];
	std::vector<std::string> tokens[2];
	std::string delimiters = "[]";


	float fRate[2] = {0.0f, };
	int nDestStateBlowIndex[2] = {0, };
	int nStateDurationTime[2] = {0, };
	std::string szArgment[2];


	strValue[0] = szOrigValue;
	//1. 구분
	TokenizeA(strValue[0], tokens[0], delimiters);

	if (tokens[0].size() != 2)
	{
		OutputDebug("%s :: %s --> 상태효과 정보 점검 필요!!!\n", __FUNCTION__, szOrigValue);
	}
	else
	{
		//2. 기본 정보
		std::string defaultInfo = tokens[0][0];
		std::vector<std::string> infoTokens;
		delimiters = ";";

		TokenizeA(defaultInfo, infoTokens, delimiters);
		if (3 == infoTokens.size())
		{
			fRate[0] = (float)atof(infoTokens[0].c_str());
			nDestStateBlowIndex[0] = atoi(infoTokens[1].c_str());
			nStateDurationTime[0] = atoi(infoTokens[2].c_str());
		}
		else
			OutputDebug("%s :: %s --> 상태효과 정보 점검 필요!!!\n", __FUNCTION__, szOrigValue);

		//3. 상태효과 설정값
		szArgment[0] = tokens[0][1];
	}

	//////////////////////////////////////////////////////////////////////////
	strValue[1] = szAddValue;
	//1. 구분
	TokenizeA(strValue[1], tokens[1], delimiters);

	if (tokens[1].size() != 2)
	{
		OutputDebug("%s :: %s --> 상태효과 정보 점검 필요!!!\n", __FUNCTION__, szAddValue);
	}
	else
	{
		//2. 기본 정보
		std::string defaultInfo = tokens[1][0];
		std::vector<std::string> infoTokens;
		delimiters = ";";

		TokenizeA(defaultInfo, infoTokens, delimiters);
		if (3 == infoTokens.size())
		{
			fRate[1] = (float)atof(infoTokens[0].c_str());
			nDestStateBlowIndex[1] = atoi(infoTokens[1].c_str());
			nStateDurationTime[1] = atoi(infoTokens[2].c_str());
		}
		else
			OutputDebug("%s :: %s --> 상태효과 정보 점검 필요!!!\n", __FUNCTION__, szAddValue);

		//3. 상태효과 설정값
		szArgment[1] = tokens[1][1];
	}


	float fResultRate = fRate[0] - fRate[1];
	int nResultDestStateBlowIndex = nDestStateBlowIndex[0];
	int nResultStateDurationTime = min(nStateDurationTime[0], nStateDurationTime[1]);

	sprintf_s(szBuff, "[%f;%d;%d][%s]", fResultRate, nResultDestStateBlowIndex, nResultStateDurationTime, szArgment[0].c_str());

	szNewValue = szBuff;
}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW

