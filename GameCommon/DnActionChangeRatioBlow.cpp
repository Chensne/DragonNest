#include "StdAfx.h"
#include "DnActionChangeRatioBlow.h"

#if !defined( USE_BOOST_MEMPOOL )
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
#endif // #if !defined( USE_BOOST_MEMPOOL )


CDnActionChangeRatioBlow::CDnActionChangeRatioBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor )
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_206;
	SetValue( szValue );

	m_fValue = 0.0f;

	std::string str = szValue;//"비율;액션";
	std::vector<std::string> tokens;
	std::string delimiters = ";";

	//1. 상태효과 인덱스 리스트와 적용 레벨 구분
	TokenizeA(str, tokens, delimiters);
	if (2 == tokens.size())
	{
		m_fRate = (float)atof( tokens[0].c_str() );
		m_strActionName = tokens[1];
	}
	else
	{
		m_fRate = 0.0f;
		m_strActionName = "";
	}

#if defined(_GAMESERVER)
	AddCallBackType(SB_ONTARGETHIT);
#endif // _GAMESERVER
}

CDnActionChangeRatioBlow::~CDnActionChangeRatioBlow(void)
{

}

void CDnActionChangeRatioBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
	__super::OnBegin(LocalTime, fDelta);

#if defined(_GAMESERVER)
	OnChangeAction();
#endif // _GAMESERVER
}

void CDnActionChangeRatioBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	__super::Process( LocalTime, fDelta );
	
}


void CDnActionChangeRatioBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	__super::OnEnd(LocalTime, fDelta);

}

#if defined(_GAMESERVER)
// float CDnActionChangeRatioBlow::OnCalcDamage( float fOriginalDamage, CDnDamageBase::SHitParam& HitParam )
// {
// 	OnChangeAction(m_hActor);
// }

void CDnActionChangeRatioBlow::OnChangeAction()
{
	//확률 계산..
	bool bExecuteable = rand() % 10000 <= (m_fRate * 10000.0f);

	//////////////////////////////////////////////////////////////////////////
	//왁스경우 Debuff가 아니라 상태효과 추가시점에 면역처리가 안됨..
	//그래서 여기서 면역 확인 해서 면역이면 처리 안되도록한다.
	if (m_hActor->IsImmuned(m_StateBlow.emBlowIndex))
		bExecuteable = false;
	//////////////////////////////////////////////////////////////////////////

	if (m_hActor && bExecuteable && !m_strActionName.empty())
	{
		m_hActor->CmdAction(m_strActionName.c_str(), 0, 3.0f, true, true, false);

		// #37325 [왁스맞고 일어나지못함] 다운이 포함된 액션일 경우 <m_fDownDelta 가 포함되어야 하지만 일반적으로 Hit에 의해 계산될때는 설정이 되나 , 
		// 이경우에는 m_fDownDelta 가 설정이 안되는 경우가있다. 0이면 ProcessDown() 함수가 동작이 안한다, 다운을 포함한 액션일경우에 다운델타를 체크 없을경우 넣어준다.
		if((m_hActor->GetState() & CDnActorState::Down) && m_hActor->GetDownRemainDelta() <= 0) 
			m_hActor->SetDownRemainDelta(0.1f); // 일단 예외적인 경우니 바로 일어나게 한다.


		// #37553
		//몬스터 액터인경우는 클라이언트로 CdmAction에서 패킷을 보내지만
		//플레이어 액터인경우는 클라이언트로 패킷을 보내지 않는다. 그래서
		//여기서 SC_ACTION_CHANGE패킷을 전송한다.
		if (!m_hActor->IsMonsterActor())
		{
			int	nActionIndex = m_hActor->GetElementIndex( m_strActionName.c_str() );
			m_hActor->RequestActionChange(nActionIndex);
		}
		
	}
}
#endif // _GAMESERVER


#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnActionChangeRatioBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	//파싱에 필요한 변수 선언
	std::vector<string> vlTokens[2];
	string strArgument[2];

	//필요한 값 변수
	float fRate[2] = {0.0f, };
	std::string strActionName[2];

	//////////////////////////////////////////////////////////////////////////
	//첫번째 문자열 파싱.
	strArgument[0] = szOrigValue;
	TokenizeA( strArgument[0], vlTokens[0], ";" );

	if( vlTokens[0].size() == 2 ) 
	{
		fRate[0] = (float)atof( vlTokens[0][0].c_str() );
		strActionName[0] = vlTokens[0][1];
	}
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	//두번째 문자열 파싱
	strArgument[1] = szAddValue;
	TokenizeA( strArgument[1], vlTokens[1], ";" );

	if( vlTokens[1].size() == 2 ) 
	{
		fRate[1] = (float)atof( vlTokens[1][0].c_str() );
		strActionName[1] = vlTokens[1][1];
	}
	//////////////////////////////////////////////////////////////////////////

	//두 값을 더한다.
	float fResultRate = fRate[0] + fRate[1];

	sprintf_s(szBuff, "%f;%s", fResultRate, strActionName[0].c_str());

	szNewValue = szBuff;
}

void CDnActionChangeRatioBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	//파싱에 필요한 변수 선언
	std::vector<string> vlTokens[2];
	string strArgument[2];

	//필요한 값 변수
	float fRate[2] = {0.0f, };
	std::string strActionName[2];
	
	//////////////////////////////////////////////////////////////////////////
	//첫번째 문자열 파싱.
	strArgument[0] = szOrigValue;
	TokenizeA( strArgument[0], vlTokens[0], ";" );

	if( vlTokens[0].size() == 2 ) 
	{
		fRate[0] = (float)atof( vlTokens[0][0].c_str() );
		strActionName[0] = vlTokens[0][1];
	}
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	//두번째 문자열 파싱
	strArgument[1] = szAddValue;
	TokenizeA( strArgument[1], vlTokens[1], ";" );

	if( vlTokens[1].size() == 2 ) 
	{
		fRate[1] = (float)atof( vlTokens[1][0].c_str() );
		strActionName[1] = vlTokens[1][1];
	}
	//////////////////////////////////////////////////////////////////////////

	//두 값을 더한다.
	float fResultRate = fRate[0] - fRate[1];

	sprintf_s(szBuff, "%f;%s", fResultRate, strActionName[0].c_str());

	szNewValue = szBuff;
}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
