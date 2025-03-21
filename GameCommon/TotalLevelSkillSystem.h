#if defined(PRE_ADD_TOTAL_LEVEL_SKILL)
#pragma once
#include "Singleton.h"

class CDnTotalLevelSkillSystem
{
public:
	CDnTotalLevelSkillSystem(DnActorHandle hActor);
	virtual ~CDnTotalLevelSkillSystem();

	enum _TotalLevelSkillType
	{
		TotalLevelSkill_Type_None = -1,
		TotalLevelSkill_Type_Attack = 0,
		TotalLevelSkill_Type_Defence,
		TotalLevelSkill_Type_Ability,
	};

	enum _TotalLevelSkillSlotIndex
	{
		TotalLevelSkill_Slot_0,
		TotalLevelSkill_Slot_1,
		TotalLevelSkill_Slot_2,
		TotalLevelSkill_Slot_3,		//캐시 슬롯.
		TotallevelSkill_Slot_Count,
	};

	enum _ErrorCode
	{
		TotalLevelSystem_Error_None = 0,
		TotalLevelSystem_Error_NotUsable,
		TotalLevelSystem_Error_NotActivateSlot,
		TotalLevelSystem_Error_SameSkillType,
	};

	struct _TotalLevelSkillInfo
	{
		int m_TotalLevelLimit;
		int m_ActorLevelLimit;
		int m_SkillType;
		int m_SkillTreeSlotIndex;

		int m_SkillID;
	};

	struct _TotalLevelSkillSlotInfo
	{
		int m_LevelLimit;
		int m_SlotIndex;
		bool m_isCashSlot;
		__time64_t m_ExpireDate;
	};

protected:
	DnActorHandle m_hActor;													//통합 레벨 스킬 액터 주인

	vector<DnSkillHandle> m_vlTotalLevelSkillList;
	
	std::map<int, _TotalLevelSkillInfo> m_TotalLevelSkillInfoList;			//통합 레벨 스킬 정보
	std::map<int, _TotalLevelSkillSlotInfo> m_TotalLevelSlotInfoList;		//통합 레벨 스킬 슬롯 정보

	std::map<int, bool> m_TotalLevelSkillSlotActivate;						//통합 레벨 슬롯 활성화 정보
	std::map<int, DnSkillHandle> m_ActiveTotalLevelSkillList;				//통합 레벨 스킬 등록 정보

	int m_TotalLevel;	//통합 레벨 현재 수치

	void RemoveTotalLevelSkillFromList(int slotIndex);

public:
	std::map<int, bool>& GetSlotActivateList() { return m_TotalLevelSkillSlotActivate; }
	_TotalLevelSkillSlotInfo* GetSlotInfo(int nSlotIndex);

	vector<DnSkillHandle>& GetTotalLevelSkillList() { return m_vlTotalLevelSkillList; }
	_TotalLevelSkillInfo* GetTotalLevelSkillInfo(int nSkillID);

	void LoadTotalLevelSkillList();
	void LoadTotalLevelSkillSlotInfo();

	DnSkillHandle FindTotalLevelSkill( int nSkillTableID );
	bool IsTotalLevelSkill( int nSkillTableID );

	bool IsActivateSlot(int nSlotIndex);

	//통합 레벨 스킬 등록..
	void AddTotalLevelSkill(int slotIndex, DnSkillHandle hSkill, bool isInitialize = false);
	//통합 레벨 스킬 제거
	void RemoveTotallevelSkill(int slotIndex);
	//등록된 통합 레벨 스킬을 반환
	DnSkillHandle GetActivateTotalLevelSkill(int slotIndex);

	//통합 레벨 스킬 슬롯 활성화 설정.
	void ActivateTotalLevelSkillSlot(int slotIndex, bool bActivate);
	//캐쉬 슬롯 활성화 설정..
	void ActivateTotalLevelSkillCashSlot(int nSlotIndex, bool bActivate, __time64_t tExpireDate);

	void SetTotalLevel(int totalLevel);
	int GetTotalLevel() { return m_TotalLevel; }

	//스킬 등록 가능 한지 판단.
	int CanAddSkill(int slotIndex, DnSkillHandle hSkill);

	//통합 레벨 스킬이 활성화 되었는지..
	bool IsUsableSkill(DnSkillHandle hSkill);

	void UpdateTotalLevel();

	void Process(LOCAL_TIME localTime, float fDelta);

	int FindEmptySlot();

	int FindEmptyNormalSlot(int nSkillID);
	int FindEmptyCashSlot(int nSkillID);
	int FindEmptySlot(int nSkillID);
	
};

#endif // PRE_ADD_TOTAL_LEVEL_SKILL