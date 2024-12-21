#include "StdAfx.h"
#include "DnDarkTransformationBlow.h"
#ifdef _CLIENT
#include "DnLocalPlayerActor.h"
#endif
#include "SmartPtrDef.h"

CDnDarkTransformationBlow::CDnDarkTransformationBlow(DnActorHandle hActor, const char* szValue) : CDnBlow(hActor)
{
	m_StateBlow.emBlowIndex = STATE_BLOW::BLOW_298;

	m_hCurrentActor = hActor;
	m_nTransformActorID = 0;
	m_nOriginalActorID = 0;

	SetValue(szValue);

	std::string str = szValue;
	std::vector<std::string> tokens;

	TokenizeA(str, tokens, ";");

	if (tokens.size() > 0)
	{
		m_nTransformActorID = atoi(tokens[0].c_str());
		m_nWeapon[0] = atoi(tokens[1].c_str());
		m_nWeapon[1] = atoi(tokens[2].c_str());
		m_nUnk = atoi(tokens[3].c_str());
	}
#if defined(_GAMESERVER)
	m_bOrderUseSkill = false;
	m_nTransformSkillID = 0;

	if (tokens.size() > 1)
		m_nTransformSkillID = atoi(tokens[1].c_str());
#endif

}

CDnDarkTransformationBlow::~CDnDarkTransformationBlow(void)
{

}

void CDnDarkTransformationBlow::OnBegin(LOCAL_TIME LocalTime, float fDelta)
{

#ifdef PRE_ADD_TRANSFORM_MONSTER_ACTOR
	if (m_hActor)
	{
		m_nOriginalActorID = m_hActor->GetActorTableID();

#ifdef _CLIENT
		m_hActor->TransformToActorID(m_nTransformActorID);
#endif
			for (int i = 0; i < 2; i++)
			{
#ifdef _CLIENT
				m_hBackupWeapon[i] = m_hCurrentActor->GetWeapon(i);

				DnWeaponHandle hWeapon = CDnWeapon::CreateWeapon(m_nWeapon[i], 0);
				m_hCurrentActor->AttachWeapon(hWeapon, i, true);
#endif

#ifdef _GAMESERVER
				DnWeaponHandle hWeapon = CDnWeapon::CreateWeapon(GetRoom(), m_nWeapon[i], 0);
				m_hCurrentActor->AttachWeapon(hWeapon, i, true);
#endif
			}
		}

#endif

}

void CDnDarkTransformationBlow::Process(LOCAL_TIME LocalTime, float fDelta)
{
#ifdef PRE_ADD_TRANSFORM_MONSTER_ACTOR
#if defined(_GAMESERVER)
	if (m_bOrderUseSkill == true)
	{
		if (m_nTransformSkillID > 0)
		{
			if (m_hActor)
				m_hCurrentActor->UseSkill(m_nTransformSkillID, false);
		}
		m_bOrderUseSkill = false;
	}
#endif
#endif
}

void CDnDarkTransformationBlow::OnEnd(LOCAL_TIME LocalTime, float fDelta)
{
	if (m_hActor)
	{
#ifdef _CLIENT
		m_hActor->TransformToNormal();

		//backup weapon
		for (int i = 0; i<CDnWeapon::EquipSlot_Amount; i++) {
			if (m_hBackupWeapon[i])
			{
				m_hCurrentActor->AttachWeapon(m_hBackupWeapon[i], i, false);
			}
		}
#endif
	}
	
	// 몬스터는 AI및 기타 여러가지 상황이 있기때문에 
	// 변신되는 상황을 AI 에서 관리하도록 합니다.
	// 종료될때 돌아오지 않도록 설정합니다.
	// 이 부분은 앞으로 변경될 수 있습니다.
}
