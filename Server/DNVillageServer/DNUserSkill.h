#pragma once

#include "DnSkillTreeSystem.h"

class CDNUserSession;
class CDNUserSkill
{
private:
	CDNUserSession* m_pUserSession;
	TSkillGroup *m_pSkillData;

	typedef map<int, TSkill> TMapSkillList;

	// 스킬트리에서 사용하는 현재 스킬 보유 정보
	TMapSkillList m_SkillList[DualSkill::Type::MAX];
	vector<CDnSkillTreeSystem::S_POSSESSED_SKILL_INFO> m_vlPossessedSkill[DualSkill::Type::MAX];

	vector<int> m_vlWaitingSkillIDsToReset;

public:
	CDNUserSkill(CDNUserSession *pUserObj);
	virtual ~CDNUserSkill(void);

	void LoadUserData();
	void SaveUserData(TUserData *pUserData);

	void AddSkill(int nSkillID, int iLevel = 1 );
	void DelSkill(int nSkillID);
	bool FindSkill(int nSkillID);
#if defined(PRE_ADD_SKILL_LEVELUP_RESERVATION)
	void ReservationSkillList(CSReservationSkillListReq* pUnlockSkillByMoneyReq);
	int SkillLevelUp( int nSkillID, int nUsePoint=0);
	int UnLockSkillByMoney( int nSkillID );
	int AcquireSkill( int nSkillID );
#else
	void SkillLevelUp( int nSkillID, int nUsePoint );
	void UnLockSkillByMoney( int nSkillID );
	void AcquireSkill( int nSkillID );
#endif

	bool UseSkillBook( int nItemID );
	void UnLockSkill( int nSkillID );

	void CheatAcquireSkill( int nSkillID );

	void ResetSkill( int nSkillPage );
	void ResetAllSkill( int nSkillPage );

	void WaitForUseSkillResetCashItemFromDBServer( vector<int>& vlSkillIDsToReset );
	bool IsWaitSkillResetDBRes( void ) { return (false == m_vlWaitingSkillIDsToReset.empty()); };
	void OnResponseSkillResetCashItemFromDBServer( bool bSuccess );

	// 해당 직업의 스킬들을 모아줌.
	void GatherThisJobSkill( BYTE cJob, IN OUT vector<int>& vlResult );

	int GetSkillLevel( int nSkillID );
	void SetSkillLevel( int nSkillID, int nLevel, int nLogCode, bool bSendSkillLevelUp = true );

	int GetLevelUpSkillPoint( int nPrevLevel, int nCurLevel );

	const vector<CDnSkillTreeSystem::S_POSSESSED_SKILL_INFO>& GetPossessedSkillInfo() { return m_vlPossessedSkill[GetCurrentSkillPage()]; };

	virtual void OnAttachEquip( TItem *pItem );
	virtual void OnDetachEquip( TItem *pItem );
	
	bool IsExclusiveSkill( int nSkillID, int nExclusiveID );
	int GetUsedSPByJob( int nJobID );
	int GetAvailSkillPointByJob( int nSkillID );

#ifdef PRE_ADD_SP_REVISION
#ifdef _FINAL_BUILD
	void CheckAndRevisionSkillPoint( bool bSendRevisionSPToClient = true );
#endif // #ifdef _FINAL_BUILD
#endif // #ifdef PRE_ADD_SP_REVISION

#ifdef PRE_ADD_CHANGEJOB_CASHITEM
	void OnResponseChangeJobCode( USHORT wTotalSkillPoint );
#endif // #ifdef PRE_ADD_CHANGEJOB_CASHITEM

	bool HasSameGlobalIDSkill( int iGlobalSkillGroupID );

	void ApplyExpendedSkillPage( int nSkillPage );
	int GetCurrentSkillPage();

#if defined(PRE_ADD_SKILL_LEVELUP_LIMIT_BY_SP)
	void GetJobHistory(BYTE *cJobArray, vector<int>& jobHistory);
	bool IsAvailableSPByJob(vector<int>& jobHistory, vector<int>& needSPValues);
	bool IsAvailableSPByJob(std::vector<int>& jobHistory, vector<int>& needSPValues, int nIndex);
#endif // PRE_ADD_SKILL_LEVELUP_LIMIT_BY_SP
};
