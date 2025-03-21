#include "StdAfx.h"
#include "DnCurseBlow.h"
#include "DnSkill.h"

#if defined(_GAMESERVER)
#include "DnMonsterActor.h"
#endif // _GAMESERVER


#if !defined( USE_BOOST_MEMPOOL )
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
#endif // #if !defined( USE_BOOST_MEMPOOL )


CDnCurseBlow::CDnCurseBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor )
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_244;
	SetValue( szValue );
	m_fValue = 0.0f;
	
	m_fDamageRate = 0.0f;
	m_fDamage = 0.0f;
	m_fDelayTime = 0.0f;
	
	m_fCoolTime = 0.0f;
	m_bAtivate = false;
	m_nDamage = 0;

	std::string str = szValue;//"데미지%;데미지;DelayTime";
	std::vector<std::string> tokens;
	std::string delimiters = ";";

	//1. 상태효과 인덱스 리스트와 적용 레벨 구분
	TokenizeA(str, tokens, delimiters);
	if( 3 == tokens.size() )
	{
		m_fDamageRate = (float)atof(tokens[0].c_str());
		m_fDamage = (float)atof( tokens[1].c_str() );
		m_fDelayTime = (float)atof(tokens[2].c_str()) * 0.001f;
	}
#if defined(_GAMESERVER)
	else if( 4 == tokens.size() )
	{
		m_fDamageRate = (float)atof(tokens[0].c_str());
		m_fDamage = (float)atof( tokens[1].c_str() );
		m_fDelayTime = (float)atof(tokens[2].c_str()) * 0.001f;
		m_fCoolTime = (float)atof( tokens[3].c_str() );

		std::string::size_type delimiterindex = str.rfind( delimiters.c_str() );
		if( delimiterindex != std::string::npos )
		{
			str.erase( delimiterindex, str.length() - delimiterindex );
			SetValue( str.c_str() );
		}
	}
#endif // _GAMESERVER
	else
	{
		OutputDebug("%s InvalidValue %s\n", __FUNCTION__, szValue);
	}
}

CDnCurseBlow::~CDnCurseBlow(void)
{
}

void CDnCurseBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
	OutputDebug("%s \n", __FUNCTION__);
}

void CDnCurseBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	CDnBlow::Process( LocalTime, fDelta );
	
	if (m_fCoolTime > 0.0f)
	{
		m_fCoolTime -= fDelta;

		if (m_fCoolTime < 0.0f)
			m_fCoolTime = 0.0f;
	}
}

void CDnCurseBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	OutputDebug("%s \n", __FUNCTION__);
}

#if defined(_GAMESERVER)
bool CDnCurseBlow::CalcDuplicateValue( const char* szValue )
{
	float fValue = 0.0;
	
	std::string str = szValue;//"데미지%;데미지;DelayTime";
	std::vector<std::string> tokens;
	std::string delimiters = ";";

	float fDamageRate = 0.0f;
	float fDamage = 0.0f;
	float fDelayTime = 0.0f;

	TokenizeA(str, tokens, delimiters);
	if (3 == tokens.size())
	{
		fDamageRate = (float)atof(tokens[0].c_str());
		fDamage = (float)atof( tokens[1].c_str() );
		fDelayTime = (float)atof(tokens[2].c_str());
	}
	else
	{
		OutputDebug("%s InvalidValue %s\n", __FUNCTION__, szValue);
	}
	
	//데미지만 더한다??
	m_fDamageRate += fDamageRate;
	m_fDamage += fDamage;

	return true;
}

void CDnCurseBlow::OnAttackChange()
{
	if (m_fCoolTime <= 0.0f)
	{
		m_bAtivate = true;
		m_fCoolTime = m_fDelayTime;

		m_nDamage = CalcDamage();
		RequestDamage();
	}
	else
	{
		m_bAtivate = false;
		m_nDamage = 0;
	}
}

void CDnCurseBlow::RequestDamage()
{
	if (m_nDamage > 0)
	{
#if defined(PRE_FIX_61382)
		DnActorHandle hActor = CDnActor::GetOwnerActorHandle(m_hActor);
		if (hActor && hActor->IsDie() == false)
		{
			m_hActor->RequestDamageFromStateBlow(GetMySmartPtr(), m_nDamage);

			if (hActor->IsDie())
				hActor->Die(m_hSkillUser);
		}
#else
		if (m_hActor->IsDie() == false)
		{
			m_hActor->RequestDamageFromStateBlow(GetMySmartPtr(), m_nDamage);

			if( m_hActor->IsDie())
				m_hActor->Die( m_hSkillUser );
		}
#endif // PRE_FIX_61382
	}
}

int CDnCurseBlow::CalcDamage()
{
	int nDamage = 0;
	
	//스킬 시전자의 마법 공격력
	if (m_hSkillUser)
	{
		//최소/최대 마법공격력
		float fAttackPower = 0.0f;
		float fAttackMaxPower = (float)m_hSkillUser->GetAttackMMaxWithoutSkill();
		float fAttackMinPower = (float)m_hSkillUser->GetAttackMMinWithoutSkill();
		fAttackPower = (fAttackMinPower + fAttackMaxPower) * 0.5f;

		nDamage = (int)((fAttackPower * m_fDamageRate) + m_fDamage);
	}

	return nDamage;
}

//#59938
//소울게이트경우 발사체 발사에서 스킬 사용자가 소울게이트 자신으로 설정되어 있다.
//소울게이트가 사라진후 저주 상태효과에서 데미지를 설정해도 소울게이트가 없어져서
//데미지 표시가 되지 않는 현상이 발생됨.
//저주 상태효과 설정 될때 스킬 유저가 소환 몬스터 인경우 주인 액터로 변경 하도록 수정함.
void CDnCurseBlow::OnSetParentSkillInfo()
{
	//스킬 시전자의 마법 공격력
	if (m_ParentSkillInfo.hSkillUser)
	{
		m_hSkillUser = m_ParentSkillInfo.hSkillUser;

		if (m_hSkillUser->IsMonsterActor())
		{
			CDnMonsterActor* pMonsterActor = dynamic_cast<CDnMonsterActor*>(m_hSkillUser.GetPointer());
			if (pMonsterActor && pMonsterActor->IsSummonedMonster())
				m_hSkillUser = pMonsterActor->GetSummonerPlayerActor();
		}

		m_ParentSkillInfo.hSkillUser = m_hSkillUser;
	}
}
#endif

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnCurseBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	szNewValue = szOrigValue;
}

void CDnCurseBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{
	char szBuff[128] = {0, };

	szNewValue = szOrigValue;	
}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW
