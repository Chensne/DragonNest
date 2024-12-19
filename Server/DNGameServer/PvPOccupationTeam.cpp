
#include "Stdafx.h"
#include "PvPOccupationSystem.h"
#include "PvPOccupationTeam.h"
#include "DNPvPGameRoom.h"
#include "EtWorldEventArea.h"
#include "PvPGameMode.h"
#include "PvPOccupationTeamSKill.h"

CPvPOccupationTeam::CPvPOccupationTeam(CDNGameRoom * pGameRoom, CPvPGameMode * pGameMode, int nTeam, UINT nLimit, int nMaxPoint, int nBossID, UINT nGuildDBID, int nQualifyingScore) : \
m_pGameRoom(pGameRoom), m_nTeamID(nTeam), m_nTeamResource(0), m_nTeamResourceLimit(nLimit)\
, m_nTeamBossID(nBossID), m_pGameMode(pGameMode), m_nMaxSkillPoint(nMaxPoint), m_nCurSkillPoint(nMaxPoint), \
m_nGuildDBID(nGuildDBID), m_nQualifyingScore(nQualifyingScore), m_nPrevDuplicateID(0), m_nPrevUsingSkillID(0)
{
	m_vhBossActor.clear();
	m_nTeamResource = 0;
	m_nTeamResourceSum = 0;
}

CPvPOccupationTeam::~CPvPOccupationTeam()
{
	m_vhBossActor.clear();
	for (std::map <int, CPvPOccupationTeamSkill*>::iterator ii = m_mOccupationTeamSkill.begin(); ii != m_mOccupationTeamSkill.end(); ii++)
		SAFE_DELETE((*ii).second);
}

void CPvPOccupationTeam::InitializeOccupationTeam(int nBossID, CEtWorldEventArea * pArea/* = NULL*/)
{
	m_vhBossActor.clear();
	if (pArea)
		m_OBB = *pArea->GetOBB();
}

void CPvPOccupationTeam::Process(LOCAL_TIME LocalTime, float fDelta)
{
}

void CPvPOccupationTeam::GetTeamSkillInfo(PvPCommon::OccupationSkillSlotInfo &Info)
{
	ULONG nCurTick = timeGetTime();

	Info.nTeam = GetTeamID();

	int i = 0;
	for (std::map <int, CPvPOccupationTeamSkill*>::iterator ii = m_mOccupationTeamSkill.begin(); ii != m_mOccupationTeamSkill.end(); ii++)
	{
		Info.cSlotIndex[i] = (*ii).second->GetSlotIndex();
		Info.nSkillID[i] = (*ii).second->GetSkillID();
		Info.nSKillUseElapsedTick[i] = (*ii).second->GetUseTick() == 0 ? 0 : nCurTick - (*ii).second->GetUseTick();
		i++;

		if (i >= QUICKSLOTMAX)
			return;
	}
}

bool CPvPOccupationTeam::IsBoss(DnActorHandle hActor)
{
	if (m_nTeamBossID <= 0) return false;

	std::vector <DnActorHandle>::iterator ii;
	for (ii = m_vhBossActor.begin(); ii != m_vhBossActor.end(); ii++)
	{
		if ((*ii) && (*ii) == hActor)
			return true;
	}
	return false;
}

bool CPvPOccupationTeam::IsBossDie()
{
	if (m_nTeamBossID <= 0) return false;

	std::vector <DnActorHandle>::iterator ii;
	for (ii = m_vhBossActor.begin(); ii != m_vhBossActor.end(); ii++)
	{
		if ((*ii) && (*ii)->IsDie())
			return true;
	}
	return false;
}

void CPvPOccupationTeam::SetBossHandle(DnActorHandle hBoss)
{
	if (hBoss)
	{
		std::vector <DnActorHandle>::iterator ii;
		for (ii = m_vhBossActor.begin(); ii != m_vhBossActor.end(); ii++)
		{
			if ((*ii) == hBoss)
				return;
		}

		if (ii == m_vhBossActor.end())
			m_vhBossActor.push_back(hBoss);
		return;
	}
	_DANGER_POINT();
}

bool CPvPOccupationTeam::AcquirePoint(int nAreaID)
{
	std::list <int>::iterator ii = std::find(m_lOwnedAreaList.begin(), m_lOwnedAreaList.end(), nAreaID);
	if (ii != m_lOwnedAreaList.end())
	{
		_DANGER_POINT();
		return false;
	}
	m_lOwnedAreaList.push_back(nAreaID);
	return true;
}

bool CPvPOccupationTeam::SnatchedPoint(int nAreaID)
{
	std::list <int>::iterator ii = std::find(m_lOwnedAreaList.begin(), m_lOwnedAreaList.end(), nAreaID);
	if (ii == m_lOwnedAreaList.end())
	{
		_DANGER_POINT();
		return false;
	}
	m_lOwnedAreaList.erase(ii);
	return true;
}

void CPvPOccupationTeam::GainPoint(int nGain)
{
	if (m_nTeamResource + nGain >= m_nTeamResourceLimit)
	{
		int nGap = m_nTeamResourceLimit - (m_nTeamResource);
		m_nTeamResource = m_nTeamResourceLimit;
		if (nGap > 0)
			m_nTeamResourceSum += nGap;
	}
	else
	{
		m_nTeamResource += nGain;
		m_nTeamResourceSum += nGain;
	}

	if (m_pGameMode->GetOccupationSystem())
		m_pGameMode->GetOccupationSystem()->SendOccupationTeamState(GetTeamID(), GetTeamResource(), GetTeamResourceSum());
}

void CPvPOccupationTeam::ClearTeamSkill()
{
	_CrealSkill();
}

bool CPvPOccupationTeam::AddTeamSkill(int nSlotIndex, TBattleGroundSkillInfo * pSkill, TBattleGroundEffectValue * pEffect)
{
	if (pSkill == NULL || pEffect == NULL)
		return false;

	if (m_mOccupationTeamSkill.size() >= QUICKSLOTMAX)
		return false;

	CPvPOccupationTeamSkill * pTeamSkill = GetTeamSkill(pSkill->nSkillID);
	if (pTeamSkill)
	{
		if (pTeamSkill->GetLevel() >= pEffect->nSkillLevel)		//³·Àº ·¹º§Àº È¹µæºÒ°¡
			return false;
	}

	if (m_nCurSkillPoint < pEffect->nNeedSkillPoint)		//Æ÷ÀÎÆ®°¡ ¸ðÁö¶óµµ È¹µæºÒ°¡
		return false;

	if (pTeamSkill == NULL)
	{
		if (m_pGameRoom && m_pGameMode->GetOccupationSystem())
		{
			pTeamSkill = new CPvPOccupationTeamSkill(m_pGameRoom, m_pGameMode->GetOccupationSystem(), this, GetTeamID());
			if (pTeamSkill == NULL)
				return false;

			if (pTeamSkill->SetValue(pSkill, pEffect, nSlotIndex))
			{
				m_nCurSkillPoint -= pEffect->nNeedSkillPoint;
				m_mOccupationTeamSkill.insert(std::make_pair(pTeamSkill->GetSkillID(), pTeamSkill));
				return true;
			}
		}
	}

	if (pTeamSkill != NULL)
		SAFE_DELETE(pTeamSkill);
	return false;
}

bool CPvPOccupationTeam::UseSkill(CDNUserSession * pSession, int nSkillID)
{
	if (pSession == NULL)
		return false;

	CPvPOccupationTeamSkill * pTeamSkill = GetTeamSkill(nSkillID);
	if (pTeamSkill == NULL)
		return false;

	if (m_nTeamResource >= (UINT)pTeamSkill->GetNeedResource())
	{
		if (pTeamSkill->UseSkill(pSession, m_nPrevDuplicateID, m_nPrevUsingSkillID))
		{
			m_nTeamResource -= pTeamSkill->GetNeedResource();
			return true;
		}
	}
	return false;
}

bool CPvPOccupationTeam::SwapSkillIndex(char cFrom, char cTo)
{
	CPvPOccupationTeamSkill * pFromSkill = GetTeamSkillBySlotIndex(cFrom);
	if (pFromSkill == NULL)
		return false;

	CPvPOccupationTeamSkill * pToSkill = GetTeamSkillBySlotIndex(cTo);
	if (pToSkill == NULL)
		return false;

	pFromSkill->SetSlotIndex(cTo);
	pToSkill->SetSlotIndex(cFrom);
	return true;
}

bool CPvPOccupationTeam::RemovePrevSkillEffect()
{
	CPvPOccupationTeamSkill * pSkill = GetTeamSkill(GetPrevUsingSkillID());
	if (pSkill == NULL)
		return false;

	if (pSkill->RemoveSkillEffect())
	{
		m_nPrevDuplicateID = 0;
		m_nPrevUsingSkillID = 0;
		return true;
	}
	return false;
}

void CPvPOccupationTeam::_CrealSkill()
{
	for (std::map <int, CPvPOccupationTeamSkill*>::iterator ii = m_mOccupationTeamSkill.begin(); ii != m_mOccupationTeamSkill.end(); ii++)
		(*ii).second->Reset();
}

CPvPOccupationTeamSkill * CPvPOccupationTeam::GetTeamSkill(int nSkillID)
{
	std::map <int, CPvPOccupationTeamSkill*>::iterator ii = m_mOccupationTeamSkill.find(nSkillID);
	if (ii != m_mOccupationTeamSkill.end())
		return (*ii).second;
	return NULL;
}

CPvPOccupationTeamSkill * CPvPOccupationTeam::GetTeamSkillBySlotIndex(char cIndex)
{
	for (std::map <int, CPvPOccupationTeamSkill*>::iterator ii = m_mOccupationTeamSkill.begin(); ii != m_mOccupationTeamSkill.end(); ii++)
	{
		if ((*ii).second->GetSlotIndex() == cIndex)
			return (*ii).second;
	}
	return NULL;
}

