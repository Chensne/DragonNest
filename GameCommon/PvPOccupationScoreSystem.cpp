
#include "Stdafx.h"
#include "PvPOccupationScoreSystem.h"

#if defined(_CLIENT)
#include "DnTableDB.h"
#include "DnBridgeTask.h"
#endif	// #if defined(_CLIENT)

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

CPvPOccupationScoreSystem::CPvPOccupationScoreSystem()
{
	m_nAcquireScore = 1;
	m_nStealScore = 1;
	m_nKillScore = 1;
}

CPvPOccupationScoreSystem::~CPvPOccupationScoreSystem()
{
}

#if defined (_GAMESERVER)
bool CPvPOccupationScoreSystem::InitializeOccupationSystem(TBattleGourndModeInfo * pModeInfo)
{
	if (pModeInfo == NULL) return false;
	m_nAcquireScore = pModeInfo->nCaptureScore;
	m_nStealScore = pModeInfo->nStealScore;
	m_nKillScore = pModeInfo->nKillScore;

	return true;
}
#endif		//#if defined (_GAMESERVER)

#if defined(_CLIENT)
void CPvPOccupationScoreSystem::InitializeOccupationSystem()
{
	DNTableFileFormat*  pSox = GetDNTable( CDnTableDB::TPVPGAMEMODE );
	int nBattleGroundID = pSox->GetFieldFromLablePtr( CDnBridgeTask::GetInstance().GetPVPRoomStatus().m_nGameModeTableID , "_BattleGroundID" )->GetInteger();;

	pSox = GetDNTable( CDnTableDB::TBATTLEGROUNDMODE );
	if( !pSox ) return;

	m_nAcquireScore = pSox->GetFieldFromLablePtr(nBattleGroundID, "_Conquest_Score")->GetInteger();
	m_nStealScore = pSox->GetFieldFromLablePtr(nBattleGroundID, "_Conquest_StealScore")->GetInteger();
	m_nKillScore = pSox->GetFieldFromLablePtr(nBattleGroundID, "_Conquest_KillScore")->GetInteger();
}
#endif	// #if defined(_CLIENT)

bool CPvPOccupationScoreSystem::GetOccupationTeamScore(int nTeam, int &nTeamScore)
{
	for (std::vector <STeamOccupationScore>::iterator ii = m_vOccupationScoreTeam.begin(); ii != m_vOccupationScoreTeam.end(); ii++)
	{
		if ((*ii).nTeamID == nTeam)
		{
			nTeamScore = (*ii).nKillScore + (*ii).nAcquireScore + (*ii).nStealAquireScore;
			return true;
		}
	}
	return false;
}

bool CPvPOccupationScoreSystem::GetOccupationTeamScore(int nTeam, TPvPOccupationTeamScoreInfo &TeamScore)
{
	for (std::vector <STeamOccupationScore>::iterator ii = m_vOccupationScoreTeam.begin(); ii != m_vOccupationScoreTeam.end(); ii++)
	{
		if ((*ii).nTeamID == nTeam)
		{			
			TeamScore.nTeamKillScore = (*ii).nKillScore;
			TeamScore.nTeamAcquireScore = (*ii).nAcquireScore;
			TeamScore.nTeamStealAcquireScore = (*ii).nStealAquireScore;
			return true;
		}
	}
	return false;;
}

UINT CPvPOccupationScoreSystem::GetMyOccupationScore(const WCHAR * pName)
{
	UINT nScore = 0;
	COccupationScoreSystem::SMyOccupationCount * pMyOccupation = GetMyOccupationCount(pName);
	if (pMyOccupation)
	{
		nScore = pMyOccupation->nAcquireCount * m_nAcquireScore;
		nScore += pMyOccupation->nStealAcquireCount * m_nStealScore;
	}
	
	nScore += GetKillCount(pName) * m_nKillScore;
	return nScore;
}

bool CPvPOccupationScoreSystem::OnAcquirePoint(int nTeam, const WCHAR * pName, bool bSnatched, bool bBreakInto)
{
	if (pName)
	{
		SMyOccupationCount * pScore = GetMyOccupationCount(pName);
		if (pScore)
		{
			int nPointType;
			if (bSnatched)
			{
				pScore->nStealAcquireCount++;
				nPointType = PvPCommon::OccupationScoreType::StealAcquire;
			}
			else
			{
				pScore->nAcquireCount++;
				nPointType = PvPCommon::OccupationScoreType::Acquire;
			}

			if( !bBreakInto )
				AddOccupationTeamScore(nTeam, nPointType);			
			return true;
		}
	}
	return false;
}

void CPvPOccupationScoreSystem::AddOccupationTeamScore(int nTeam, int nType)
{
	std::vector <STeamOccupationScore>::iterator ii;
	for (ii = m_vOccupationScoreTeam.begin(); ii != m_vOccupationScoreTeam.end(); ii++)
	{
		if ((*ii).nTeamID == nTeam)
		{
			if (nType == PvPCommon::OccupationScoreType::Kill) (*ii).nKillScore += m_nKillScore;
			else if (nType == PvPCommon::OccupationScoreType::Acquire) (*ii).nAcquireScore += m_nAcquireScore;
			else if (nType == PvPCommon::OccupationScoreType::StealAcquire) (*ii).nStealAquireScore += m_nStealScore;
			return;
		}
	}

	STeamOccupationScore Score;
	Score.nTeamID = nTeam;

	if (nType == PvPCommon::OccupationScoreType::Kill) Score.nKillScore += m_nKillScore;
	else if (nType == PvPCommon::OccupationScoreType::Acquire) Score.nAcquireScore += m_nAcquireScore;
	else if (nType == PvPCommon::OccupationScoreType::StealAcquire) Score.nStealAquireScore += m_nStealScore;

	m_vOccupationScoreTeam.push_back(Score);
}
