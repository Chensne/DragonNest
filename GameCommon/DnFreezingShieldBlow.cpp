#include "StdAfx.h"
#include "DnFreezingShieldBlow.h"
#if defined( _GAMESERVER )
#include "DnActor.h"
#endif // #if defined( _GAMESERVER )

#if !defined( USE_BOOST_MEMPOOL )
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
#endif // #if !defined( USE_BOOST_MEMPOOL )



CDnFreezingShieldBlow::CDnFreezingShieldBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor )
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_173;
	SetValue( szValue );
	
	m_nDuration = 0;

	SetInfo(szValue);

#if defined(_GAMESERVER)
	AddCallBackType( SB_ONDEFENSEATTACK );
#endif // _GAMESERVER
}

CDnFreezingShieldBlow::~CDnFreezingShieldBlow(void)
{

}

void CDnFreezingShieldBlow::SetInfo(const char* szValue)
{
	std::string str = szValue;//"[마법공격력비율;공격범위;결빙지속시간][결빙설정값]";
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
			m_fRate = (float)atof( infoTokens[0].c_str() );
			m_fLimitRadius = (float)atof( infoTokens[1].c_str() );
			m_nFreezingTime = atoi(infoTokens[2].c_str());
		}
		else
		{
			m_fRate = 0.0f;
			m_fLimitRadius = 0.0f;
			m_nFreezingTime = 0;

			OutputDebug("%s Invalid Value!!!!\n", __FUNCTION__);
		}

		//3. 상태효과 설정값
		m_strStateAttribute = tokens[1];
	}
}


void CDnFreezingShieldBlow::OnBegin(LOCAL_TIME LocalTime, float fDelta )
{
	__super::OnBegin(LocalTime, fDelta);

	if (m_hActor)
	{
		//액터의 마법공격력 * 비율 => 방어막의 내구도를 결정한다..
		m_nDuration = int((float)m_hActor->GetAttackMMax() * m_fRate);
	}
	
	OutputDebug( "CDnFreezingShieldBlow::OnBegin, 마법공격력 : %d , 비율 : %f, 내구도 : %d\n", m_hActor->GetAttackMMax(), m_fRate, m_nDuration );
}



void CDnFreezingShieldBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	CDnBlow::Process( LocalTime, fDelta );

#ifdef _GAMESERVER
	//서버쪽에서는 지속시간동안 유지 또는 방어막이 0되기전가지 계속 유지
	if( STATE_BLOW::STATE_END == m_StateBlow.emBlowState || m_nDuration <= 0 )
	{
		// 클라에게 상태효과 제거하란 패킷만 보내고 서버에서는 순리대로 blow 를 해제시킨다.
		m_hActor->CmdRemoveStateEffect( m_StateBlow.emBlowIndex, false );
		SetState( STATE_BLOW::STATE_END );
	}
#else
	//클라이언트는 계속 유지 되고, 서버 패킷으로 끝나도록한다...
	SetState(STATE_BLOW::STATE_DURATION);
#endif
}


void CDnFreezingShieldBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{

#if defined(_GAMESERVER)
	if (m_hActor && !m_hActor->IsDie())
	{
		DNVector(DnActorHandle) hVecList;

		m_hActor->ScanActor( m_hActor->GetRoom(), *m_hActor->GetPosition(), m_fLimitRadius, hVecList );

		// 41번 상태 효과에서 이펙트 3번 사용 [2011/03/07 semozz]
		char szBuff[128] = {0, };
		_snprintf_s(szBuff, _countof(szBuff), _TRUNCATE, "%d", 3 );
		m_ParentSkillInfo.szEffectOutputIDs = szBuff;
		m_ParentSkillInfo.szEffectOutputIDToClient = szBuff;

		for( DWORD i=0; i<hVecList.size(); i++ ) {
			if( !hVecList[i]->IsHittable( m_hActor, LocalTime, NULL ) ) 
				continue;

			if (hVecList[i]->GetTeam() == m_hActor->GetTeam())
				continue;

			hVecList[i]->CmdAddStateEffect(&m_ParentSkillInfo, STATE_BLOW::BLOW_041, m_nFreezingTime, m_strStateAttribute.c_str(), true);
		}
	}
#endif // _GAMESERVER
	OutputDebug( "CDnFreezingShieldBlow::OnEnd\n");
}


#ifdef _GAMESERVER
bool CDnFreezingShieldBlow::OnDefenseAttack( DnActorHandle hHitter, CDnState* pAttackerState, CDnDamageBase::SHitParam &HitParam, bool bHitSuccess )
{
	//  #25797 히트 시그널에 bIgnoreParring 이 켜져 있으면 발동하지 않는다.
	if( HitParam.bIgnoreParring )
		return false;

#if defined(PRE_FIX_BLOCK_CONDITION)
	//블럭의 발동이 fDamage가 0이라도 적이라면 블럭 발동 가능 하도록..
	//아군인 경우 기존 처럼 fDamage 가0이면 블럭 되지 않도록 함.
	if (IsCanBlock(hHitter, m_hActor, HitParam) == false)
		return false;
#else
	// hit percent 가 0% 일 경우 발동하지 않는 것으로 됨. (#21175)
	if( 0.0f == HitParam.fDamage )
		return false;
#endif // PRE_FIX_BLOCK_CONDITION

#if defined(PRE_ADD_49166)
	// 피격자가 디버프 (Freezing / FrameStop) 상태효과가 적용되어 있는 경우는 블럭 발동 되지 않도록..
	if (IsInVaildBlockCondition(m_hActor) == true)
		return false;
#endif // PRE_ADD_49166

	bool bResult = false;

	float fDamage = m_hActor->PreCalcDamage(hHitter, HitParam);
	
	if( m_nDuration >= fDamage )
	{
		m_nDuration -= (int)fDamage;

		//맞는 동작 하지 않도록한다.
		HitParam.szActionName.assign( "" );	
		
		bResult = true;
	}
	else
	{
		//남은 내구도보다 데미지가 더 크면 내구도는 0으로 하고
		//피격 동작 하도록.
		m_nDuration = 0;
	}

	return bResult;
}
#endif


#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnFreezingShieldBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	std::string strValue[2];
	std::vector<std::string> tokens[2];
	std::string delimiters = "[]";


	float fRate[2] = {0.0f, };
	float fLimitRadius[2] = {0.0f, };
	int nFreezingTime[2] = {0, };
	
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
		if (5 == infoTokens.size())
		{
			fRate[0] = (float)atof(infoTokens[0].c_str());
			fLimitRadius[0] = (float)atof(infoTokens[1].c_str());
			nFreezingTime[0] = atoi(infoTokens[2].c_str());
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
		if (5 == infoTokens.size())
		{
			fRate[1] = (float)atof(infoTokens[0].c_str());
			fLimitRadius[1] = (float)atof(infoTokens[1].c_str());
			nFreezingTime[1] = atoi(infoTokens[2].c_str());
		}
		else
			OutputDebug("%s :: %s --> 상태효과 정보 점검 필요!!!\n", __FUNCTION__, szAddValue);

		//3. 상태효과 설정값
		szArgment[1] = tokens[1][1];
	}


	float fResultRate = fRate[0] + fRate[1];
	float fResultLimitRadius = max(fLimitRadius[0], fLimitRadius[1]);
	int nResultFreezingTime = max(nFreezingTime[0], nFreezingTime[1]);
	
	sprintf_s(szBuff, "[%f;%f;%d;][%s]", fResultRate, fResultLimitRadius, nResultFreezingTime, szArgment[0].c_str());

	szNewValue = szBuff;
}

void CDnFreezingShieldBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	std::string strValue[2];
	std::vector<std::string> tokens[2];
	std::string delimiters = "[]";


	float fRate[2] = {0.0f, };
	float fLimitRadius[2] = {0.0f, };
	int nFreezingTime[2] = {0, };

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
		if (5 == infoTokens.size())
		{
			fRate[0] = (float)atof(infoTokens[0].c_str());
			fLimitRadius[0] = (float)atof(infoTokens[1].c_str());
			nFreezingTime[0] = atoi(infoTokens[2].c_str());
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
		if (5 == infoTokens.size())
		{
			fRate[1] = (float)atof(infoTokens[0].c_str());
			fLimitRadius[1] = (float)atof(infoTokens[1].c_str());
			nFreezingTime[1] = atoi(infoTokens[2].c_str());
		}
		else
			OutputDebug("%s :: %s --> 상태효과 정보 점검 필요!!!\n", __FUNCTION__, szAddValue);

		//3. 상태효과 설정값
		szArgment[1] = tokens[1][1];
	}


	float fResultRate = fRate[0] - fRate[1];
	float fResultLimitRadius = min(fLimitRadius[0], fLimitRadius[1]);
	int nResultFreezingTime = min(nFreezingTime[0], nFreezingTime[1]);

	sprintf_s(szBuff, "[%f;%f;%d;][%s]", fResultRate, fResultLimitRadius, nResultFreezingTime, szArgment[0].c_str());

	szNewValue = szBuff;
}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
