#pragma once

#include "Task.h"
#include "DnSkill.h"
#include "GameListener.h"
#include "MultiSingleton.h"
#include "DNGameRoom.h"
#include "DnSkillTreeSystem.h"

class CDnPlayerActor;

class CDnSkillTask : public CTask, 
					 public CMultiSingleton<CDnSkillTask, MAX_SESSION_COUNT>, 
					 public CGameListener, public TBoostMemoryPool< CDnSkillTask >
{
private:
	struct S_SKILL_LEVELUP_INFO : public TBoostMemoryPool<S_SKILL_LEVELUP_INFO>
	{
		DnActorHandle hActor;
		DnSkillHandle hSkill;
		int nSkillID;
		BYTE cUseSkillPoint;
		CDNUserSession* pSession;

		S_SKILL_LEVELUP_INFO( void ) : pSession( NULL ), nSkillID(0), cUseSkillPoint(0)
		{			
		};
	};

	list<S_SKILL_LEVELUP_INFO*> m_listWaitSkillLevelup;		// 액티브 스킬 시전 중에 레벨업이 오는 경우 큐에 넣었다가 끝났을 때 처리.

	//CDnSkillTreeSystem m_SkillTreeSystem;

public:
	CDnSkillTask( CDNGameRoom* pRoom );
	virtual ~CDnSkillTask(void);

protected:
	int OnRecvSkillMessage( CDNUserSession* pSession, int nSubCmd, char* pData, int nLen );

public:
	bool Initialize( void );

#if defined(PRE_ADD_SKILL_LEVELUP_RESERVATION)
	int UnLockSkillByMoney( CDNUserSession* pSession, int nSkillID );
#else
	bool UnLockSkillByMoney( CDNUserSession* pSession, int nSkillID );	
#endif
	bool UseSkillBook( CDNUserSession* pSession, int nItemID );

	virtual void Process( LOCAL_TIME LocalTime, float fDelta );
	virtual int OnDispatchMessage( CDNUserSession * pSession, int nMainCmd, int nSubCmd, char * pData, int iLen );
	virtual int OnRecvSkillLevelUpReq( CDNUserSession* pSession, CSSkillLevelUpReq* pPacket );
	virtual int OnRecvAcquireSkillReq( CDNUserSession* pSession, CSAcquireSkillReq* pPacket );
	virtual int OnRecvUnlockSkillByMoneyReq( CDNUserSession* pSession, CSUnlockSkillByMoneyReq* pPacket );
#if defined(PRE_ADD_SKILL_LEVELUP_RESERVATION)
	virtual int OnRecvReservationSkillListReq( CDNUserSession* pSession, CSReservationSkillListReq* pPacket );
#endif
#if defined(PRE_ADD_PRESET_SKILLTREE)
	virtual int OnRecvSkillSetList( CDNUserSession* pSession );
	virtual int OnRecvSaveSkillSet( CDNUserSession* pSession, CSSaveSkillSet *pPacket );
	virtual int OnRecvDeleteSkillSet( CDNUserSession* pSession, CSDeleteSkillSet *pPacket );
#endif	// #if defined(PRE_ADD_PRESET_SKILLTREE)
	int SkillLevelUp(CDNUserSession* pSession, int nSkillID, BYTE cUseSkillPoint);
	int AcquireSkill(CDNUserSession* pSession, int nSkillID);


	void GetSkillLevelList( int iSkillID, /*IN OUT*/ vector<int>& vlSkillLevelList, int iLevelDataType );

#if defined(PRE_ADD_SKILL_LEVELUP_LIMIT_BY_SP)
	bool IsAvailableSPByJob(std::vector<int>& jobHistory, vector<int>& needSPValues, CDnPlayerActor * pPlayerActor );
	bool IsAvailableSPByJob(std::vector<int>& jobHistory, vector<int>& needSPValues, int nIndex, CDnPlayerActor * pPlayerActor);
	void GetNeedSPValuesByJob(int nSkillID, std::vector<int>& nNeedSPValues);
#endif // PRE_ADD_SKILL_LEVELUP_LIMIT_BY_SP
};
