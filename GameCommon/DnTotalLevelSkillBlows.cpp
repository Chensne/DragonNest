#include "StdAfx.h"
#include "DnTotalLevelSkillBlows.h"

#if defined(_GAMESERVER)
#include "DnPlayerActor.h"
#include "SecondarySkillRepository.h"
#include "SecondarySkill.h"
#include "DNUserSession.h"
#include "DNGameDataManager.h"
#endif // _GAMESERVER

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

//////////////////////////////////////////////////////////////////////////
//���ݿ�
CDnAddCriticalRateBlow::CDnAddCriticalRateBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor )
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_267;

	SetValue( szValue );

	m_nCoolTime = 0;
	m_fLeftCoolTime = 0.0f;	
	m_bActivated = false;
	m_fCriticalRate = 0.0f;

	SetValue(szValue);
	m_fValue = 0.0f;	
}
CDnAddCriticalRateBlow::~CDnAddCriticalRateBlow(void)
{

}

void CDnAddCriticalRateBlow::SetInfo(const char* szValue)
{
	std::string str = szValue;//"ũ��Ƽ���������ӽð�(1/1000��);ũ��Ƽ��������ġ(0.0~1.0)";
	std::vector<std::string> tokens;
	std::string delimiters = ";";

	//1. ����
	TokenizeA(str, tokens, delimiters);

	if (tokens.size() != 2)
	{
		OutputDebug("%s :: %s --> ����ȿ�� ���� ���� �ʿ�!!!\n", __FUNCTION__, szValue);
	}
	else
	{
		m_nCoolTime = atoi(tokens[0].c_str());
		m_fCriticalRate = (float)atof(tokens[1].c_str());
	}
}

#if defined(_GAMESERVER)
void CDnAddCriticalRateBlow::ApplyCriticalIncBlow()
{
	char szParam[128] = {0, };
	_snprintf_s(szParam, _countof(szParam), _TRUNCATE, "%f", m_fCriticalRate);

	m_hActor->CmdAddStateEffect(&m_ParentSkillInfo, STATE_BLOW::BLOW_251, m_nCoolTime, szParam);

	m_bActivated = true;
	m_fLeftCoolTime = m_nCoolTime * 0.001f;
}
#endif // _GAMESERVER

void CDnAddCriticalRateBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
	__super::OnBegin(LocalTime, fDelta);

	OutputDebug("%s\n", __FUNCTION__);
}
void CDnAddCriticalRateBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	__super::Process(LocalTime, fDelta);

	if (m_bActivated == true)
	{
		m_fLeftCoolTime -= fDelta;

		if (m_fLeftCoolTime <= 0.0f)
		{
			m_fLeftCoolTime = 0.0f;
			m_bActivated = false;
		}
	}
}
void CDnAddCriticalRateBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	__super::OnEnd(LocalTime, fDelta);

	OutputDebug("%s\n", __FUNCTION__);
}

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnAddCriticalRateBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{

}
void CDnAddCriticalRateBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{

}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW


//////////////////////////////////////////////////////////////////////////
//����
CDnDamageChagneBlow::CDnDamageChagneBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor )
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_268;

	SetValue( szValue );

	m_nCoolTime = 0;
	m_fLeftCoolTime = 0.0f;	
	m_bActivated = false;
	m_fDamagRate = 0.0f;
	m_fLimitHPRate = 0.0f;

	SetValue(szValue);
	m_fValue = 0.0f;
}
CDnDamageChagneBlow::~CDnDamageChagneBlow(void)
{

}

void CDnDamageChagneBlow::SetInfo(const char* szValue)
{
	std::string str = szValue;//"���������� ���ӽð�(1/1000��);����������(0.0~1.0)";
	std::vector<std::string> tokens;
	std::string delimiters = ";";

	//1. ����
	TokenizeA(str, tokens, delimiters);

	if (tokens.size() != 2)
	{
		OutputDebug("%s :: %s --> ����ȿ�� ���� ���� �ʿ�!!!\n", __FUNCTION__, szValue);
	}
	else
	{
		m_nCoolTime = atoi(tokens[0].c_str());
		m_fDamagRate = (float)atof(tokens[1].c_str());
	}
}

#if defined(_GAMESERVER)
void CDnDamageChagneBlow::ApplyDamageChange()
{
	m_bActivated = true;
	m_fLeftCoolTime = m_nCoolTime * 0.001f;
}
#endif // _GAMESERVER

void CDnDamageChagneBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
	__super::OnBegin(LocalTime, fDelta);

	OutputDebug("%s\n", __FUNCTION__);
}
void CDnDamageChagneBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	__super::OnBegin(LocalTime, fDelta);

	OutputDebug("%s\n", __FUNCTION__);
}
void CDnDamageChagneBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	__super::OnBegin(LocalTime, fDelta);

	OutputDebug("%s\n", __FUNCTION__);
}

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnDamageChagneBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{

}
void CDnDamageChagneBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{

}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW


//////////////////////////////////////////////////////////////////////////
//��ɿ� ����ȿ����...
//1. ��ɽ� ����ġ �߰� n%����
CDnExpIncBlow::CDnExpIncBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor )
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_255;

	SetValue( szValue );

	m_fValue = (float)atof(szValue);
}
CDnExpIncBlow::~CDnExpIncBlow(void)
{

}

void CDnExpIncBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
	__super::OnBegin(LocalTime, fDelta);

	OutputDebug("%s\n", __FUNCTION__);
}
void CDnExpIncBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	__super::Process(LocalTime, fDelta);

}
void CDnExpIncBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	__super::OnEnd(LocalTime, fDelta);

	OutputDebug("%s\n", __FUNCTION__);
}

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnExpIncBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{

}
void CDnExpIncBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{

}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW


//2. �� ����ġ n%����
CDnPetExpIncBlow::CDnPetExpIncBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor )
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_256;

	SetValue( szValue );

	m_fValue = (float)atof(szValue);
}
CDnPetExpIncBlow::~CDnPetExpIncBlow(void)
{

}

void CDnPetExpIncBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
	__super::OnBegin(LocalTime, fDelta);

	OutputDebug("%s\n", __FUNCTION__);
}
void CDnPetExpIncBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	__super::Process(LocalTime, fDelta);


}
void CDnPetExpIncBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	__super::OnEnd(LocalTime, fDelta);

	OutputDebug("%s\n", __FUNCTION__);
}

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnPetExpIncBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{

}
void CDnPetExpIncBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{

}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW


//3. �������� ĳ���� �̵��ӵ� n%����
CDnMoveSpeedIncBlow::CDnMoveSpeedIncBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor )
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_257;

	SetValue( szValue );

	m_fValue = (float)atof(szValue);
}
CDnMoveSpeedIncBlow::~CDnMoveSpeedIncBlow(void)
{

}

void CDnMoveSpeedIncBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
	__super::OnBegin(LocalTime, fDelta);

	OutputDebug("%s\n", __FUNCTION__);
}
void CDnMoveSpeedIncBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	__super::Process(LocalTime, fDelta);

}
void CDnMoveSpeedIncBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	__super::OnEnd(LocalTime, fDelta);

	OutputDebug("%s\n", __FUNCTION__);
}

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnMoveSpeedIncBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{

}
void CDnMoveSpeedIncBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{

}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW


//4. Ż���� �̵� �Ӱ� n%����
CDnVehicleMoveSpeedBlow::CDnVehicleMoveSpeedBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor )
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_258;

	SetValue( szValue );

	m_fValue = (float)atof(szValue);
}
CDnVehicleMoveSpeedBlow::~CDnVehicleMoveSpeedBlow(void)
{

}

void CDnVehicleMoveSpeedBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
	__super::OnBegin(LocalTime, fDelta);

	OutputDebug("%s\n", __FUNCTION__);
}
void CDnVehicleMoveSpeedBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	__super::Process(LocalTime, fDelta);


}
void CDnVehicleMoveSpeedBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	__super::OnEnd(LocalTime, fDelta);

	OutputDebug("%s\n", __FUNCTION__);
}

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnVehicleMoveSpeedBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{

}
void CDnVehicleMoveSpeedBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{

}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW


//5. ĳ���� ����� ������ ������ �ʴ� ��ų(����ø�)
CDnIgnoreDurationBlow::CDnIgnoreDurationBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor )
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_259;

	SetValue( szValue );

	m_fValue = (float)atof(szValue);
}
CDnIgnoreDurationBlow::~CDnIgnoreDurationBlow(void)
{

}

void CDnIgnoreDurationBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
	__super::OnBegin(LocalTime, fDelta);

	OutputDebug("%s\n", __FUNCTION__);
}
void CDnIgnoreDurationBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	__super::Process(LocalTime, fDelta);


}
void CDnIgnoreDurationBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	__super::OnEnd(LocalTime, fDelta);

	OutputDebug("%s\n", __FUNCTION__);
}

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnIgnoreDurationBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{

}
void CDnIgnoreDurationBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{

}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW


//6. NPC���� ������ �� ȣ���� 10%����
CDnNpcReputationIncBlow::CDnNpcReputationIncBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor )
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_260;

	SetValue( szValue );

	m_fValue = (float)atof(szValue);
}
CDnNpcReputationIncBlow::~CDnNpcReputationIncBlow(void)
{

}

void CDnNpcReputationIncBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
	__super::OnBegin(LocalTime, fDelta);

	OutputDebug("%s\n", __FUNCTION__);
}
void CDnNpcReputationIncBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	__super::Process(LocalTime, fDelta);


}
void CDnNpcReputationIncBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	__super::OnEnd(LocalTime, fDelta);

	OutputDebug("%s\n", __FUNCTION__);
}

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnNpcReputationIncBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{

}
void CDnNpcReputationIncBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{

}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW


//7. �ݷμ��� �޴� n�� �߰� ȹ��
CDnAddMedalBlow::CDnAddMedalBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor )
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_261;

	SetValue( szValue );
	m_fValue = 0.0f;

	m_nAddMedal = 0;
	m_nLimitMedal = 0;

	std::string str = szValue;
	std::vector<std::string> tokens;
	TokenizeA(str, tokens, ";" );

	if (tokens.size() == 2)
	{
		m_nAddMedal = atoi(tokens[0].c_str());
		m_nLimitMedal = atoi(tokens[1].c_str());
	}
	else
	{
		OutputDebug("%s Invalid Value %s\n ", __FUNCTION__, szValue);
	}
}
CDnAddMedalBlow::~CDnAddMedalBlow(void)
{

}

void CDnAddMedalBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
	__super::OnBegin(LocalTime, fDelta);

	OutputDebug("%s\n", __FUNCTION__);
}
void CDnAddMedalBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	__super::Process(LocalTime, fDelta);


}
void CDnAddMedalBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	__super::OnEnd(LocalTime, fDelta);

	OutputDebug("%s\n", __FUNCTION__);
}

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnAddMedalBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{

}
void CDnAddMedalBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{

}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW


//8. ���� ��� �ð� n% ����
CDnFarmingTimeBlow::CDnFarmingTimeBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor )
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_262;

	SetValue( szValue );

	m_fValue = (float)atof(szValue);
}
CDnFarmingTimeBlow::~CDnFarmingTimeBlow(void)
{

}

void CDnFarmingTimeBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
	__super::OnBegin(LocalTime, fDelta);

	OutputDebug("%s\n", __FUNCTION__);
}
void CDnFarmingTimeBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	__super::Process(LocalTime, fDelta);


}
void CDnFarmingTimeBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	__super::OnEnd(LocalTime, fDelta);

	OutputDebug("%s\n", __FUNCTION__);
}

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnFarmingTimeBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{

}
void CDnFarmingTimeBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{

}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW


//9. ���� ���� ������ 1% ����
CDnLadderStoreSaleBlow::CDnLadderStoreSaleBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor )
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_263;

	SetValue( szValue );

	m_fValue = (float)atof(szValue);
}
CDnLadderStoreSaleBlow::~CDnLadderStoreSaleBlow(void)
{

}

void CDnLadderStoreSaleBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
	__super::OnBegin(LocalTime, fDelta);

	OutputDebug("%s\n", __FUNCTION__);
}
void CDnLadderStoreSaleBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	__super::Process(LocalTime, fDelta);


}
void CDnLadderStoreSaleBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	__super::OnEnd(LocalTime, fDelta);

	OutputDebug("%s\n", __FUNCTION__);
}

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnLadderStoreSaleBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{

}
void CDnLadderStoreSaleBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{

}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW

//10. �ڵ� ���� �ð� n�� ����
CDnAutoFishingTimeBlow::CDnAutoFishingTimeBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor )
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_264;

	SetValue( szValue );

	m_fValue = (float)atof(szValue);
}
CDnAutoFishingTimeBlow::~CDnAutoFishingTimeBlow(void)
{

}

void CDnAutoFishingTimeBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
	__super::OnBegin(LocalTime, fDelta);

	OutputDebug("%s\n", __FUNCTION__);
}
void CDnAutoFishingTimeBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	__super::Process(LocalTime, fDelta);


}
void CDnAutoFishingTimeBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	__super::OnEnd(LocalTime, fDelta);

	OutputDebug("%s\n", __FUNCTION__);
}

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnAutoFishingTimeBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{

}
void CDnAutoFishingTimeBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{

}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW


//11. �丮 ���� n% ����
CDnCookingExpIncBlow::CDnCookingExpIncBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor )
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_265;

	SetValue( szValue );

	m_fValue = (float)atof(szValue);

	m_AddExpValue = 0;
}
CDnCookingExpIncBlow::~CDnCookingExpIncBlow(void)
{

}

void CDnCookingExpIncBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
	__super::OnBegin(LocalTime, fDelta);

	OutputDebug("%s\n", __FUNCTION__);
}
void CDnCookingExpIncBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	__super::Process(LocalTime, fDelta);


}
void CDnCookingExpIncBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	__super::OnEnd(LocalTime, fDelta);

	OutputDebug("%s\n", __FUNCTION__);
}

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnCookingExpIncBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{

}
void CDnCookingExpIncBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{

}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW


//12. ��ȭ ȹ�淮 n%����
CDnGoldIncBlow::CDnGoldIncBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor )
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_266;

	SetValue( szValue );

	m_fValue = (float)atof(szValue);
}
CDnGoldIncBlow::~CDnGoldIncBlow(void)
{

}

void CDnGoldIncBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
	__super::OnBegin(LocalTime, fDelta);

	OutputDebug("%s\n", __FUNCTION__);
}
void CDnGoldIncBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	__super::Process(LocalTime, fDelta);


}
void CDnGoldIncBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	__super::OnEnd(LocalTime, fDelta);

	OutputDebug("%s\n", __FUNCTION__);
}

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnGoldIncBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{

}
void CDnGoldIncBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{

}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW

//13. �ñر� ��Ÿ�� ����
CDnGlobalCoolTimeBlow::CDnGlobalCoolTimeBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor )
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_269;

	SetValue( szValue );

	m_fValue = (float)atof(szValue);
}

CDnGlobalCoolTimeBlow::~CDnGlobalCoolTimeBlow(void)
{

}

void CDnGlobalCoolTimeBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
	__super::OnBegin(LocalTime, fDelta);

	//�̹� ���� �� �۷ι� ��Ÿ�� �ð� ����..
#if defined( PRE_ADD_TOTAL_LEVEL_SKILL )
	if (m_hActor)
		m_hActor->UpdateGlobalCoolTime(m_fValue);
#endif // PRE_ADD_TOTAL_LEVEL_SKILL

	OutputDebug("%s\n", __FUNCTION__);
}

void CDnGlobalCoolTimeBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	__super::OnBegin(LocalTime, fDelta);

}

void CDnGlobalCoolTimeBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	__super::OnEnd(LocalTime, fDelta);

	//�̹� ���� �� �۷ι� ��Ÿ�� �ð� ����..
#if defined(PRE_ADD_TOTAL_LEVEL_SKILL)
	if (m_hActor)
		m_hActor->ResetGlobalCoolTime(m_fValue);
#endif // PRE_ADD_TOTAL_LEVEL_SKILL

	OutputDebug("%s\n", __FUNCTION__);
}

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnGlobalCoolTimeBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{

}
void CDnGlobalCoolTimeBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{

}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW

//////////////////////////////////////////////////////////////////////////
//��ȯ�� �ý��� ���� ����ȿ��
//��Ƽ���� ����� n%���
CDnPartyDropRateBlow::CDnPartyDropRateBlow( DnActorHandle hActor, const char* szValue ) : CDnBlow( hActor )
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_270;

	SetValue( szValue );

	m_fValue = (float)atof(szValue);
}
CDnPartyDropRateBlow::~CDnPartyDropRateBlow(void)
{

}

void CDnPartyDropRateBlow::OnBegin( LOCAL_TIME LocalTime, float fDelta )
{
	__super::OnBegin(LocalTime, fDelta);

	OutputDebug("%s\n", __FUNCTION__);
}
void CDnPartyDropRateBlow::Process( LOCAL_TIME LocalTime, float fDelta )
{
	__super::Process(LocalTime, fDelta);

}
void CDnPartyDropRateBlow::OnEnd( LOCAL_TIME LocalTime, float fDelta )
{
	__super::OnEnd(LocalTime, fDelta);

	OutputDebug("%s\n", __FUNCTION__);
}

#if defined(PRE_ADD_PREFIX_SYSTE_RENEW)
void CDnPartyDropRateBlow::AddStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{

}
void CDnPartyDropRateBlow::RemoveStateEffectValue(const char* szOrigValue, const char* szAddValue, std::string& szNewValue)
{

}
#endif // PRE_ADD_PREFIX_SYSTE_RENEW