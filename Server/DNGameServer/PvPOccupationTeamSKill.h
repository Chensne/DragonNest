
#pragma once

class CDNPvPGameRoom;
class CPvPOccupactionSystem;
class CPvPOccupationTeam;

class CPvPOccupationTeamSkill : public TBoostMemoryPool<CPvPOccupationTeamSkill>
{
public:
	CPvPOccupationTeamSkill(CDNGameRoom * pGameRoom, CPvPOccupactionSystem * pSystem, CPvPOccupationTeam * pTeamSkill, int nTeam);
	~CPvPOccupationTeamSkill();

	bool IsSet() { return m_nSkillID <= 0 ? false : true; }
	void Reset() { _ClearValue(); }
	bool SetValue(TBattleGroundSkillInfo * pSkill, TBattleGroundEffectValue * pEffect, int nSlotIndex);
	int GetLevel() { return m_nSkillLevel; }
	int GetNeedResource() { return m_nUseResPoint; }
	int GetSlotIndex() { return m_nSlotIndex; }
	void SetSlotIndex(char cIndex) { m_nSlotIndex = cIndex; }
	int GetSkillID() { return m_nSkillID; }
	int GetDuplicateID() { return m_nDuplicate; }

	bool UseSkill(CDNUserSession * pSession, int &nPrevDuplicateID, int &nPreSkillID);
	bool RemoveSkillEffect();
	ULONG GetUseTick() { return m_nUseTick; }

private:
	CDNGameRoom * m_pGameRoom;
	CPvPOccupactionSystem * m_pOccupationSystem;
	CPvPOccupationTeam * m_pOccupationTeam;
	
	int m_nTeamID;
	int m_nSkillID;
	int m_nSkillLevel;
	int m_nSkillMaxLevel;
	int m_nNeedSkillPoint;			//ȹ����ʿ�����Ʈ
	int m_nUseResPoint;				//��ų���� �Ҹ� ������ ����Ʈ
	int m_nSkillType;				//CDnSkill::SkillTypeEnum		//������ ��ų�� ��ųŸ�Կ� �״� ������ ���� ������ �߰�Ȯ��� �ʿ��ұ� �ؼ� �о��
	int m_nSkillDurationType;		//CDnSkill::DurationTypeEnum
	int m_nSkillTargetType;			//CDnSkill::TargetTypeEnum
	int m_nEffectType[PvPCommon::Common::MaximumEffectCount];
	int m_nEffectApplyType[PvPCommon::Common::MaximumEffectCount];
	std::string m_strEffectValue[PvPCommon::Common::MaximumEffectCount];
	int m_nEffectValueDuration[PvPCommon::Common::MaximumEffectCount];
	int m_nSlotIndex;
	int m_nProcessType;				//IDnSkillProcessor::PLAY_ANI;
	int m_nCoolTime;
	std::string m_strActionName;
	CDnSkill::SkillInfo m_SkillInfo;
	int m_nDuplicate;

	//
	ULONG m_nUseTick;				//����ѽð�

	void _ClearValue();
};
