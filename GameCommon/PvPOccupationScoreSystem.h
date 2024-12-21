
#pragma once

#include "OccupationScoreSystem.h"

class CPvPOccupationScoreSystem : public COccupationScoreSystem, public TBoostMemoryPool<CPvPOccupationScoreSystem>
{
public:
	CPvPOccupationScoreSystem();
	virtual ~CPvPOccupationScoreSystem();

#if defined (_GAMESERVER)
	virtual bool InitializeOccupationSystem(TBattleGourndModeInfo * pModeInfo);
#endif		//#if defined (_GAMESERVER)
#if defined(_CLIENT)
	virtual void InitializeOccupationSystem();
#endif	// #if defined(_CLIENT)

	//Get	
	virtual bool GetOccupationTeamScore(int nTeam, int &nTeamScore);
	virtual bool GetOccupationTeamScore(int nTeam, TPvPOccupationTeamScoreInfo &TeamScore);
	virtual UINT GetMyOccupationScore(const WCHAR * pName);

	//On
	virtual bool OnAcquirePoint(int nTeam, const WCHAR * pName, bool bSnatched = false, bool bBreakInto = false);

private:
	int m_nAcquireScore;
	int m_nStealScore;
	int m_nKillScore;

	virtual void AddOccupationTeamScore(int nTeam, int nType);
};
