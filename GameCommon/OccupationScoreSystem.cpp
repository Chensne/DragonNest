
#include "Stdafx.h"
#include "OccupationScoreSystem.h"
#if defined( _GAMESERVER )
#include "DnActor.h"
#include "PvPGameMode.h"
#include "PvPOccupationMode.h"
#include "PvPOccupationSystem.h"
#include "DNGameRoom.h"
#include "DNUserSession.h"
#else // #if defined( _GAMESERVER )
#include "DnPlayerActor.h"
#include "DnMonsterActor.h"
#endif // #if defined( _GAMESERVER )

COccupationScoreSystem::COccupationScoreSystem()
{
}

COccupationScoreSystem::~COccupationScoreSystem()
{
}

UINT COccupationScoreSystem::GetOccupationAcquireScore(DnActorHandle hActor)
{
	if (!hActor || !hActor->IsPlayerActor())
		return 0;

	SMyOccupationCount * pScore = GetMyOccupationCount(hActor->GetName());
	if (pScore)
		return pScore->nAcquireCount;
	return 0;
}

UINT COccupationScoreSystem::GetOccupationStealScore(DnActorHandle hActor )
{
	if (!hActor || !hActor->IsPlayerActor())
		return 0;

	SMyOccupationCount * pScore = GetMyOccupationCount(hActor->GetName());
	if (pScore)
		return pScore->nStealAcquireCount;
	return 0;
}

void COccupationScoreSystem::OnDamage( DnActorHandle hActor, DnActorHandle hHitter, const INT64 iDamage  )
{
}

void COccupationScoreSystem::OnDie(DnActorHandle hActor, DnActorHandle hHitter)
{
	CPvPScoreSystem::OnDie(hActor, hHitter);
}

bool COccupationScoreSystem::OnTryAcquirePoint(DnActorHandle hActor, int AreaID, LOCAL_TIME Localtime)
{
	return false;
}

void COccupationScoreSystem::OnLeaveUser( DnActorHandle hActor )
{
	CPvPScoreSystem::OnLeaveUser(hActor);
	std::map<std::wstring, SMyOccupationCount>::iterator ii = m_mOccupationScore.find(hActor->GetName());
	if (ii != m_mOccupationScore.end())
		m_mOccupationScore.erase(ii);
}

#if defined( _GAMESERVER )
void COccupationScoreSystem::SendScore( CDNUserSession* pGameSession )
{
	CPvPScoreSystem::SendScore(pGameSession);
	_SendOccupationScore(pGameSession);
}
#endif // #if defined( _GAMESERVER )

COccupationScoreSystem::SMyOccupationCount * COccupationScoreSystem::GetMyOccupationCount(const WCHAR * pName)
{
	if (pName == NULL)
		return NULL;

	std::map<std::wstring, SMyOccupationCount>::iterator ii = m_mOccupationScore.find(pName);
	if (ii != m_mOccupationScore.end())
		return &(*ii).second;

	if (AddMyOccupationScore(pName))
		return GetMyOccupationCount(pName);
	return NULL;
}

bool COccupationScoreSystem::AddMyOccupationScore(const WCHAR * pName)
{
	if (pName == NULL) return false;

	SMyOccupationCount Score;
	std::wstring wstrName = pName;

	std::pair<std::map<std::wstring, SMyOccupationCount>::iterator, bool> iret = m_mOccupationScore.insert(std::make_pair(wstrName, Score));	
	return iret.second;
}

void COccupationScoreSystem::OnOccupationTeamScore(int nTeam, int nType)
{
	AddOccupationTeamScore(nTeam, nType);
}

void COccupationScoreSystem::AddOccupationTeamScore(DnActorHandle hActor, int nType)
{
	AddOccupationTeamScore(hActor->GetTeam(), nType);
}

#if defined( _GAMESERVER )
void COccupationScoreSystem::_SendOccupationScore(CDNUserSession* pSession)
{
	if( !pSession )
		return;

	CDNGameRoom* pGameRoom = pSession->GetGameRoom();
	if( !pGameRoom )
		return;

	if(m_mOccupationScore.size() > PvPCommon::Common::MaxPlayer)
	{
		_DANGER_POINT();
		return;
	}

	TPvPOccupationScore packet;
	memset(&packet, 0, sizeof(TPvPOccupationScore));

	for (int i = 0; i < PvPCommon::TeamIndex::Max; i++)
	{
		packet.TeamScoreInfo[i].nTeamID = (i == 0 ? PvPCommon::Team::A : PvPCommon::Team::B);
		GetOccupationTeamScore(packet.TeamScoreInfo[i].nTeamID, packet.TeamScoreInfo[i]);
	}

	for(std::map<std::wstring, SMyOccupationCount>::iterator ii = m_mOccupationScore.begin(); ii != m_mOccupationScore.end() ;ii++)
	{		
		CDNGameRoom::PartyStruct* pStruct =  pGameRoom->GetPartyData( const_cast<WCHAR*>((*ii).first.c_str()) );
		if( !pStruct || !pStruct->pSession )
			continue;

		packet.Info[packet.cCount].nSessionID = pStruct->pSession->GetSessionID();		
		packet.Info[packet.cCount].nTryAcquireCount = (*ii).second.nTryAcquireCount;
		packet.Info[packet.cCount].nAcquireCount = (*ii).second.nAcquireCount;
		packet.Info[packet.cCount].cBossKillCount = (*ii).second.cBossKillCount;
		packet.Info[packet.cCount].nStealAquireCount = (*ii).second.nStealAcquireCount;
		packet.cCount++;
	}

	pSession->AddSendData(SC_PVP, ePvP::SC_OCCUPATIONSCORE, reinterpret_cast<char*>(&packet), sizeof(packet) - sizeof(packet.Info) + (sizeof(TPvPOccupationScoreInfo) * packet.cCount));
}
#endif // #if defined( _GAMESERVER )