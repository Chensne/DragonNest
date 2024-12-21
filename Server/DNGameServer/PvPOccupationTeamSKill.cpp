
#include "Stdafx.h"
#include "DNPvPGameRoom.h"
#include "PvPOccupationTeamSKill.h"
#include "PvPOccupationSystem.h"
#include "DNUserSession.h"
#include "DnSkill.h"
#include "DnGameTask.h"
#include "TaskManager.h"
#include "DnPvPGameTask.h"
#include "IDnSkillProcessor.h"
#include "PvPOccupationTeam.h"

CPvPOccupationTeamSkill::CPvPOccupationTeamSkill(CDNGameRoom * pGameRoom, CPvPOccupactionSystem * pSystem, CPvPOccupationTeam * pTeamSkill, int nTeam) : \
m_pGameRoom(pGameRoom), m_pOccupationSystem(pSystem), m_pOccupationTeam(pTeamSkill), m_nTeamID(nTeam), m_nUseTick(0), m_nDuplicate(0)
{
	_ClearValue();
}

CPvPOccupationTeamSkill::~CPvPOccupationTeamSkill()
{
	_ClearValue();
}

bool CPvPOccupationTeamSkill::SetValue(TBattleGroundSkillInfo * pSkill, TBattleGroundEffectValue * pEffect, int nSlotIndex)
{
	if (pSkill == NULL || pEffect == NULL)
		return false;

	m_nSkillID = pSkill->nSkillID;
	m_nSkillLevel = pEffect->nSkillLevel;
	m_nSkillMaxLevel = pSkill->nSkillMaxLevel;
	m_nNeedSkillPoint = pEffect->nNeedSkillPoint;			//획득시필요포인트
	m_nUseResPoint = pEffect->nUseResPoint;				//스킬사용시 소모 점령전 포인트
	m_nSkillType = pSkill->nSkillType;				//CDnSkill::SkillTypeEnum		//점령전 스킬은 스킬타입에 그닥 영향을 받지 않지만 추가확장시 필요할까 해서 읽어둠
	m_nSkillDurationType = pSkill->nSkillDurationType;		//CDnSkill::DurationTypeEnum
	m_nSkillTargetType = pSkill->nSkillTargetType;			//CDnSkill::TargetTypeEnum

	for (int i = 0; i < PvPCommon::Common::MaximumEffectCount; i++)
	{
		m_nEffectType[i] = pSkill->nEffectType[i];
		m_nEffectApplyType[i] = pSkill->nEffectApplyType[i];	
		m_strEffectValue[i] = pEffect->strEffectValue[i];
		m_nEffectValueDuration[i] = pEffect->nEffectValueDuration[i];
	}

	m_nSlotIndex = nSlotIndex;
	m_nProcessType = pSkill->nProcess;
	//if (m_nProcessType == IDnSkillProcessor::PLAY_ANI)
	m_nCoolTime = pEffect->nCoolTime;
	m_strActionName = pEffect->strActionName;
	m_nDuplicate = pSkill->nCanDuplicate;

	std::vector<CDnSkill::StateEffectStruct> vecSkillEffectList;
	CDnSkill::CreateBattleGroundSkillInfo( m_nSkillID, m_nSkillLevel, m_SkillInfo, vecSkillEffectList );
	m_SkillInfo.bIsGuildSkill = true;
	return true;
}

bool CPvPOccupationTeamSkill::UseSkill(CDNUserSession * pSession, int &nPrevDuplicateID, int &nPreSkillID)
{
	if (m_pGameRoom == NULL || pSession == NULL || !pSession->GetActorHandle())
		return false;

	ULONG nCurTick = timeGetTime();
	if (m_nUseTick + m_nCoolTime > nCurTick)
		return false;
	
	switch (m_nSkillDurationType)
	{
	case CDnSkill::DurationTypeEnum::Buff:
		{
			if (m_nSkillTargetType != PvPCommon::OccupationSkillTargetType::Team)
				return false;

			if (m_pOccupationTeam == NULL)
				return false;

			if (m_pOccupationTeam->GetPrevDuplicateID() == GetDuplicateID())
			{
				if (m_pOccupationTeam->RemovePrevSkillEffect() == false)
					_DANGER_POINT();
			}

			CDNUserSession * pUser;
			for (int i = 0; i < (int)m_pGameRoom->GetUserCount(); i++)
			{
				 pUser = m_pGameRoom->GetUserData(i);
				 if (pUser && pSession->GetTeam() == pUser->GetTeam())
				 {					 
					 for (int j = 0; j < PvPCommon::Common::MaximumEffectCount; j++)
					 {
						 if (m_nEffectType[j] == STATE_BLOW::emBLOW_INDEX::BLOW_NONE)
							 continue;

						 if (m_nEffectApplyType[j] == PvPCommon::OccuaptionSkillEffectTartgetType::Team)
							 static_cast<CDNPvPGameRoom*>(m_pGameRoom)->CmdPvPOccupationAddStateEffect(&m_SkillInfo, pUser->GetActorHandle(), m_nEffectType[j], m_nEffectValueDuration[j], m_strEffectValue[j].c_str(), true);
					 }
				 }
			}

			nPrevDuplicateID = GetDuplicateID();
			nPreSkillID = GetSkillID();
			break;
		}

	case CDnSkill::DurationTypeEnum::Instantly:
		{
			break;
		}

	default:
		{
			_DANGER_POINT();
			return false;
		}
	}

	if (pSession->GetActorHandle())
		pSession->GetActorHandle()->CmdAction(m_strActionName.c_str());

	m_nUseTick = nCurTick;
	return true;
}

bool CPvPOccupationTeamSkill::RemoveSkillEffect()
{
	if (m_pOccupationTeam == NULL)
		return false;

	switch (m_nSkillDurationType)
	{
	case CDnSkill::DurationTypeEnum::Buff:
		{
			CDNUserSession * pUser;
			for (int i = 0; i < (int)m_pGameRoom->GetUserCount(); i++)
			{
				pUser = m_pGameRoom->GetUserData(i);
				if (pUser && m_pOccupationTeam->GetTeamID() == pUser->GetTeam())
				{					 
					for (int j = 0; j < PvPCommon::Common::MaximumEffectCount; j++)
					{
						if (m_nEffectType[j] == STATE_BLOW::emBLOW_INDEX::BLOW_NONE)
							continue;

						if (m_nEffectApplyType[j] == PvPCommon::OccuaptionSkillEffectTartgetType::Team)
							static_cast<CDNPvPGameRoom*>(m_pGameRoom)->CmdPvPOccupationRemoveStateEffect(pUser->GetActorHandle(), m_nEffectType[j]);
					}
				}
			}
			break;
		}

	default:
		{
			_DANGER_POINT();
			return false;
		}
	}
	return true;
}

void CPvPOccupationTeamSkill::_ClearValue()
{
	m_nSkillID = 0;
	m_nSkillLevel = 0;
	m_nSkillMaxLevel = 0;
	m_nNeedSkillPoint = 0;			//획득시필요포인트
	m_nUseResPoint = 0;				//스킬사용시 소모 점령전 포인트
	m_nSkillType = 0;				//CDnSkill::SkillTypeEnum		//점령전 스킬은 스킬타입에 그닥 영향을 받지 않지만 추가확장시 필요할까 해서 읽어둠
	m_nSkillDurationType = 0;		//CDnSkill::DurationTypeEnum
	m_nSkillTargetType = 0;			//CDnSkill::TargetTypeEnum
	memset(m_nEffectType, 0, sizeof(m_nEffectType));
	memset(m_nEffectApplyType, 0, sizeof(m_nEffectApplyType));
	for (int i = 0; i < PvPCommon::Common::MaximumEffectCount; i++)
		m_strEffectValue[i].clear();
	memset(m_nEffectValueDuration, 0, sizeof(m_nEffectValueDuration));
	m_nSlotIndex = -1;
	m_nProcessType = -1;
	m_nCoolTime = 0;
	m_strActionName.clear();
}
