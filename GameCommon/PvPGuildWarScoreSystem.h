
#pragma once

#include "OccupationScoreSystem.h"

class CPvPGuildWarScoreSystem : public COccupationScoreSystem, public TBoostMemoryPool<CPvPGuildWarScoreSystem>
{
public:

	CPvPGuildWarScoreSystem();
	virtual ~CPvPGuildWarScoreSystem();

	//Get	
	virtual bool GetOccupationTeamScore(int nTeam, int &nTeamScore);
	virtual bool GetOccupationTeamScore(int nTeam, TPvPOccupationTeamScoreInfo &TeamScore);

	//On
	virtual void OnDamage(DnActorHandle hActor, DnActorHandle hHitter, const INT64 iDamage);
	virtual void OnDie(DnActorHandle hActor, DnActorHandle hHitter);
	virtual bool OnAcquirePoint(int nTeam, const WCHAR * pName, bool bSnatched = false, bool bBreakInto = false);

#ifdef _CLIENT
	virtual bool IsRedBossKill()	{ return m_bRedBossKill; }
	virtual bool IsBlueBossKill()	{ return m_bBlueBossKill; }	

	int GetDoorID( int nTeam, int nCount );
#endif	//#ifdef _CLIENT	

protected:
#ifdef _CLIENT
	int	m_BossID[PvPCommon::TeamIndex::Max];
	int m_DoorID[PvPCommon::TeamIndex::Max][PvPCommon::Common::DoorCount];
	int m_BossString[PvPCommon::TeamIndex::Max];
	int m_DoorDamageString[PvPCommon::TeamIndex::Max][PvPCommon::Common::DoorCount];
	int m_DoorDieString[PvPCommon::TeamIndex::Max][PvPCommon::Common::DoorCount];
	LOCAL_TIME m_BossDamageTime[PvPCommon::TeamIndex::Max];
	LOCAL_TIME m_DoorDamageTime[PvPCommon::TeamIndex::Max][PvPCommon::Common::DoorCount];

	int m_nDamageSound;

	bool m_bRedBossKill;
	bool m_bBlueBossKill;
#endif

	virtual void AddOccupationTeamScore(int nTeam, int nType);
};