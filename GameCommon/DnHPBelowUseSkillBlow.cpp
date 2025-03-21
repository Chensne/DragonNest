#include "StdAfx.h"
#include "DnHPBelowUseSkillBlow.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


#ifdef PRE_ADD_HPBELOWDOSKILLBLOW

CDnHPBelowUseSkillBlow::CDnHPBelowUseSkillBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor )
, m_nHP( 0 )
, m_nSkillIndex( 0 )
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_281;

	SetValue( szValue );

	string strValue( szValue );
	int iSemiColonIndex = (int)strValue.find_first_of( ';', 0 );
	bool bValidArgument = (string::npos != iSemiColonIndex);
	_ASSERT( bValidArgument && "281 상태효과 효과 수치가 잘못 되었습니다. 세미콜론을 찾을 수가 없음" );

	if( bValidArgument )
	{
		string strHP = strValue.substr( 0, iSemiColonIndex );
		string strSkill = strValue.substr( iSemiColonIndex+1, strValue.length() );

		m_nHP = (int)atoi( strHP.c_str() );
		m_nSkillIndex = (int)atoi( strSkill.c_str() );
	}

	AddCallBackType( SB_ONCALCDAMAGE );
}

CDnHPBelowUseSkillBlow::~CDnHPBelowUseSkillBlow(void)
{
}

void CDnHPBelowUseSkillBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
	OutputDebug( "CDnHPBelowUseSkillBlow::OnBegin Value:%2.2f\n", m_fValue );
}

void CDnHPBelowUseSkillBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	CDnBlow::Process( LocalTime, fDelta );
}

void CDnHPBelowUseSkillBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	OutputDebug( "CDnHPBelowUseSkillBlow::OnEnd Value:%2.2f\n", m_fValue );
}

#ifdef _GAMESERVER
float CDnHPBelowUseSkillBlow::CalcDamage( float fOriginalDamage )
{
	float fResult = 0.0f;
	int iNowHP = (int)m_hActor->GetHP();

	if( iNowHP - (int)fOriginalDamage <= m_nHP )
	{
		if( m_nSkillIndex > 0 )
		{
			if( m_hActor )
				m_hActor->UseSkill( m_nSkillIndex, false );
		}
	}

	return fResult;
}

float CDnHPBelowUseSkillBlow::OnCalcDamage( float fOriginalDamage, CDnDamageBase::SHitParam& HitParam )
{
	return CalcDamage( fOriginalDamage );
}
#endif // _GAMESERVER

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnHPBelowUseSkillBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
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

void CDnHPBelowUseSkillBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
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

#endif // PRE_ADD_HPBELOWDOSKILLBLOW