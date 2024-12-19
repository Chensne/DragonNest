#include "StdAfx.h"

#ifdef PRE_ADD_PVP_TOURNAMENT

#include "DnCommonDef.h"
#include "DnPVPTournamentDataMgr.h"
#include "DnBaseRoomDlg.h"
#include "DnBridgeTask.h"
#include "DnCommonUtil.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

bool Compare_PVPTournamentStep(const SFinalReportUserInfo& a, const SFinalReportUserInfo& b)
{
	return a.tournamentStep < b.tournamentStep;
}

CDnPVPTournamentDataMgr::CDnPVPTournamentDataMgr()
{
	m_RoomInfo.Reset();
	m_bHoldSwapPosition = false;
	m_bEditPositionState = true;
	m_bReadyState = false;
}

void CDnPVPTournamentDataMgr::SetRoomInfo(PvPCommon::RoomInfo& info)
{
	m_RoomInfo.Reset();
	m_RoomInfo.Set(info);
}

void CDnPVPTournamentDataMgr::SetRoomState(UINT roomState)
{
	m_RoomInfo.uiRoomState = roomState;
}

bool CDnPVPTournamentDataMgr::IsReadyRoom() const
{
	return (m_RoomInfo.uiRoomState == PvPCommon::RoomState::None || m_RoomInfo.uiRoomState == PvPCommon::RoomState::Password);
}

bool CDnPVPTournamentDataMgr::IsPasssWordRoom() const
{
	return (m_RoomInfo.uiRoomState & PvPCommon::RoomState::Password) ? true : false;
}

bool CDnPVPTournamentDataMgr::IsSyncingRoom() const
{
	return (m_RoomInfo.uiRoomState & PvPCommon::RoomState::Syncing) ? true : false;
}

bool CDnPVPTournamentDataMgr::IsStartingRoom() const
{
	return (m_RoomInfo.uiRoomState & PvPCommon::RoomState::Starting) ? true : false;
}

bool CDnPVPTournamentDataMgr::IsPlayingRoom() const
{
	return (m_RoomInfo.uiRoomState & PvPCommon::RoomState::Playing) ? true : false;
}

bool CDnPVPTournamentDataMgr::IsInGamePlayingRoom() const
{
	return (IsPlayingRoom() || IsStartingRoom() || IsSyncingRoom());
}

int CDnPVPTournamentDataMgr::GetEventRoomIndex() const
{
	return m_RoomInfo.nEventRoomID;
}

int CDnPVPTournamentDataMgr::GetMemberCountOfMode(eMatchModeByCount modeType)
{
	switch(modeType)
	{
	case eMODE_4: return 4;
	case eMODE_8: return 8;
	case eMODE_16: return 16;
	case eMODE_32: return 32;
	default: return eConstance::INVALID_MEMBERCOUNT;
	}

	return eConstance::INVALID_MEMBERCOUNT;
}

eMatchModeByCount CDnPVPTournamentDataMgr::GetModeByMemberCount(int memberCount)
{
	switch(memberCount)
	{
	case 4: return eMODE_4;
	case 8: return eMODE_8;
	case 16: return eMODE_16;
	case 32: return eMODE_32;
	default: return eMODE_MAX;
	}

	return eMODE_MAX;
}

eMatchModeByCount CDnPVPTournamentDataMgr::GetCurrentMode() const
{
	return GetModeByMemberCount(m_RoomInfo.cMaxPlayerNum);
}

int CDnPVPTournamentDataMgr::GetMatchListUIStartIndexByCount(eMatchModeByCount mode) const
{
	switch(mode)
	{
	case eMODE_4: return 3;
	case eMODE_8: return 2;
	case eMODE_16:
	case eMODE_32:
		return 0;
	}

	return -1;
}

bool CDnPVPTournamentDataMgr::MakeMatchListIndexerByMode(eMatchModeByCount mode)
{
	m_MatchListIndexer.clear();

	const int memberCount = GetMemberCountOfMode(mode);
	if (memberCount == eConstance::INVALID_MEMBERCOUNT)
	{
		_ASSERT(0);
		return false;
	}

	const int nUIStartIndex = GetMatchListUIStartIndexByCount(mode);
	if (nUIStartIndex < 0 || nUIStartIndex >= MAX_MATCH_MEMBERCOUNT)
	{
		_ASSERT(0);
		return false;
	}

	int nUIIndex = nUIStartIndex;
	std::vector<int> tempHalfIndexList;
	const int nHalfSideMemberCount = (memberCount / 2);
	int i = 0;
	for (; i < memberCount; ++i)
	{
		if (i >= nHalfSideMemberCount)
		{
			int nIndexOfHalfIndexList = (i - nHalfSideMemberCount);
			if (nIndexOfHalfIndexList < 0 || nIndexOfHalfIndexList >= (int)tempHalfIndexList.size())
			{
				_ASSERT(0);
				return false;
			}

			nUIIndex = tempHalfIndexList[nIndexOfHalfIndexList] + MAX_MATCH_SIDE_COUNT;
		}
		else
		{
			tempHalfIndexList.push_back(nUIIndex);
		}

		m_MatchListIndexer.push_back(nUIIndex);
		nUIIndex++;
	}

	return true;
}

const SMatchUserInfo* CDnPVPTournamentDataMgr::GetSlotInfoByCommonIndex(int commonIndex) const
{
	int i = 0;
	for (; i < (int)m_MatchUserList.size(); ++i)
	{
		const SMatchUserInfo& info = m_MatchUserList[i];
		if (info.commonIndex == commonIndex)
			return (&info);
	}

	return NULL;
}

bool CDnPVPTournamentDataMgr::SetInfo(const SMatchUserInfo& info)
{
	int i = 0;
	bool bSet = false;
	for (; i < (int)m_MatchUserList.size(); ++i)
	{
		SMatchUserInfo& insertInfo = m_MatchUserList[i];
		if (insertInfo.commonIndex == info.commonIndex)
		{
			insertInfo = info;
			insertInfo.matchListUIIndex = GetUIIndexByCommonIndex(info.commonIndex);
			return true;
		}
	}

	SMatchUserInfo addInfo;
	addInfo = info;
	addInfo.matchListUIIndex = GetUIIndexByCommonIndex(info.commonIndex);
	m_MatchUserList.push_back(addInfo);

	return true;
}

const SMatchUserInfo* CDnPVPTournamentDataMgr::GetSlotInfoBySessionID(UINT uiSessionID) const
{
	std::vector<SMatchUserInfo>::const_iterator iter = m_MatchUserList.begin();
	for (; iter != m_MatchUserList.end(); ++iter)
	{
		const SMatchUserInfo& info = (*iter);
		if (info.uiSessionID == uiSessionID)
			return &info;
	}

	return NULL;
}

void CDnPVPTournamentDataMgr::GetNeedUpdateMatchInfoIndexes(std::vector<int>& indexList) const
{
	int i = 0;
	for (; i < (int)m_MatchUserList.size(); ++i)
	{
		const SMatchUserInfo& info = m_MatchUserList[i];
		if (info.bIsNeedUpdate)
			indexList.push_back(i);
	}
}

int CDnPVPTournamentDataMgr::GetUIIndexByCommonIndex(int commonIndex) const
{
	if (commonIndex < 0 || commonIndex >= (int)m_MatchListIndexer.size())
		return -1;

	return m_MatchListIndexer[commonIndex];
}

int CDnPVPTournamentDataMgr::GetCurrentMemberCount() const
{
	return (int)m_MatchUserList.size();
}

const SMatchUserInfo* CDnPVPTournamentDataMgr::GetSlotInfoByUIIndex(int matchUIIndex) const
{
	int i = 0;
	for (; i < (int)m_MatchListIndexer.size(); ++i)
	{
		const int& curUIIndex = m_MatchListIndexer[i];
		if (curUIIndex == matchUIIndex)
			return GetSlotInfoByCommonIndex(i);
	}

	return NULL;
}

bool CDnPVPTournamentDataMgr::SwapSlotByCommonIndex(const int& sourceIndex, const int& destIndex)
{
	if (sourceIndex < 0 || sourceIndex >= (int)m_MatchListIndexer.size() ||
		destIndex < 0 || destIndex >= (int)m_MatchListIndexer.size() ||
		sourceIndex == destIndex)
	{
		return false;
	}

	int swapIndex[2];
	memset(swapIndex, -1, sizeof(swapIndex));

	int i = 0;
	for (; i < (int)m_MatchUserList.size(); ++i)
	{
		SMatchUserInfo& info = m_MatchUserList[i];
		if (info.commonIndex == sourceIndex)
			swapIndex[0] = i;
		else if (info.commonIndex == destIndex)
			swapIndex[1] = i;

		if (swapIndex[0] >= 0 && swapIndex[1] >= 0)
			break;
	}

	if (swapIndex[0] < 0 || swapIndex[1] < 0 || (swapIndex[0] == swapIndex[1]))
	{
		_ASSERT(0);
		ErrorLog("[PVPTOURNAMENT] SwapSlotByCommonIndex Error : srcIndexFromServer(%d) destIndexFromServer(%d) \
				 MatchuserList.size(%d), clientSwapSrcIdx(%d) clientSwapDestIdx(%d)", sourceIndex, destIndex, (int)m_MatchUserList.size(),
				 swapIndex[0], swapIndex[1]);
		return false;
	}

	std::swap(m_MatchUserList[swapIndex[0]].commonIndex, m_MatchUserList[swapIndex[1]].commonIndex);
	std::swap(m_MatchUserList[swapIndex[0]].matchListUIIndex, m_MatchUserList[swapIndex[1]].matchListUIIndex);

	return true;
}

void CDnPVPTournamentDataMgr::HoldSwapPosition(bool bHold)
{
	m_bHoldSwapPosition = bHold;
}

void CDnPVPTournamentDataMgr::Reset()
{
	m_RoomInfo.Reset();
	m_MatchUserList.clear();
	m_MatchListIndexer.clear();
	SetEditPositionState(false);
	m_GameMatchUserList.clear();
	m_FinalReportUserList.clear();

	HoldSwapPosition(false);
}

void CDnPVPTournamentDataMgr::ResetRoomInfo()
{
	m_RoomInfo.Reset();
	SetEditPositionState(false);

	HoldSwapPosition(false);
}

bool CDnPVPTournamentDataMgr::IsEnableStartGame() const
{
	if (AmIMaster())
	{
		eMatchModeByCount mode = GetCurrentMode();
#ifdef _WORK
		int memberCount = (int)m_MatchUserList.size();
#else
		int memberCount = GetMemberCountOfMode(mode);
#endif
		if (memberCount == m_MatchUserList.size() && IsAllPlayerReady())
			return true;
	}

	return false;
}

bool CDnPVPTournamentDataMgr::IsAllPlayerReady() const
{
	std::vector<SMatchUserInfo>::const_iterator iter = m_MatchUserList.begin();
	for (; iter != m_MatchUserList.end(); ++iter)
	{
		const SMatchUserInfo& curInfo = (*iter);
		if (CDnBaseRoomDlg::IsMaster(curInfo.uiUserState) == false && CDnBaseRoomDlg::IsReady(curInfo.uiUserState) == false)
			return false;
	}

	return true;
}

bool CDnPVPTournamentDataMgr::AmIMaster() const
{
	UINT localUserSessionID = CDnBridgeTask::GetInstance().GetSessionID();
	const SMatchUserInfo* pInfo = GetSlotInfoBySessionID(localUserSessionID);
	if (pInfo == NULL)
	{
		_ASSERT(0);
		return false;
	}

	return CDnBaseRoomDlg::IsMaster(pInfo->uiUserState);
}

bool CDnPVPTournamentDataMgr::RemoveInfo(const UINT sessionID)
{
	std::vector<SMatchUserInfo>::iterator iter = m_MatchUserList.begin();
	for (; iter != m_MatchUserList.end(); ++iter)
	{
		SMatchUserInfo& info = (*iter);
		if (info.uiSessionID == sessionID)
		{
			m_MatchUserList.erase(iter);
			return true;
		}
	}

	return false;
}

void CDnPVPTournamentDataMgr::SetGameMatchUserList(const SCPvPTournamentMatchList& packet)
{
	m_GameMatchUserList.clear();
	
	int memberCount = GetMemberCountOfMode(GetCurrentMode());
	double wholeStep = CommonUtil::logWithBase((double)memberCount, 2);

	int i = 0;
	for (; i < packet.nCount; ++i)
	{
		SGameMatchUserInfo gameMatchInfo;
		const TPvPTournamentUserInfo& info = packet.sTournamentUserInfo[i];
		gameMatchInfo.charName = info.wszCharName;
		gameMatchInfo.bWin = info.bWin;
		gameMatchInfo.jobId = info.cJob;
		gameMatchInfo.sessionId = info.uiSessionID;
		if (info.cTournamentStep <= 0)
			gameMatchInfo.step = 0;
		else
			gameMatchInfo.step = int(wholeStep - CommonUtil::logWithBase((double)info.cTournamentStep, 2)) + 1;

		m_GameMatchUserList.push_back(gameMatchInfo);

		SetFinalReportUser_TournamentStep(info.uiSessionID, info.cTournamentStep);
	}
}

const std::vector<SGameMatchUserInfo>& CDnPVPTournamentDataMgr::GetGameMatchUserList() const
{
	return m_GameMatchUserList;
}

void CDnPVPTournamentDataMgr::ClearGameMatchCurrentUser()
{
	int i = 0;
	int userListSize = (int)m_GameMatchUserList.size();
	for (; i < userListSize; ++i)
	{
		SGameMatchUserInfo& current = m_GameMatchUserList[i];
		current.bCurrentMatchUser = false;
	}
}

void CDnPVPTournamentDataMgr::SetGameMatchCurrentUser(const std::vector<UINT>& sessionIds)
{
	int i = 0;
	int userListSize = (int)m_GameMatchUserList.size();
	for (; i < userListSize; ++i)
	{
		SGameMatchUserInfo& current = m_GameMatchUserList[i];
		current.bCurrentMatchUser = false;

		int j = 0;
		for (; j < (int)sessionIds.size(); ++j)
		{
			if (sessionIds[j] == current.sessionId)
			{
				current.bCurrentMatchUser = true;
				break;
			}
		}
	}
}

bool CDnPVPTournamentDataMgr::IsInBattlePlayer(const UINT& sessionID) const
{
	int i = 0;
	int userListSize = (int)m_GameMatchUserList.size();
	for (; i < userListSize; ++i)
	{
		const SGameMatchUserInfo& current = m_GameMatchUserList[i];
		if (current.sessionId == sessionID)
			return current.bCurrentMatchUser;
	}

	return false;
}

void CDnPVPTournamentDataMgr::AddUserToFinalReportList(const SFinalReportUserInfo& info)
{
	SFinalReportUserInfo* pInfo = GetFinalReportUser(info.nSessionID);
	if (pInfo)
	{
		*pInfo = info;
		return;
	}

	m_FinalReportUserList.push_back(info);
}

void CDnPVPTournamentDataMgr::RemoveUserFromFinalReportList(UINT sessionID)
{
	std::vector<SFinalReportUserInfo>::iterator iter = m_FinalReportUserList.begin();
	for (; iter != m_FinalReportUserList.end(); ++iter)
	{
		SFinalReportUserInfo& curInfo = (*iter);
		if (sessionID == curInfo.nSessionID)
		{
			m_FinalReportUserList.erase(iter);
			return;
		}
	}
}

void CDnPVPTournamentDataMgr::SetFinalReportUser_TournamentStep(UINT sessionID, int tournamentStep)
{
	SFinalReportUserInfo* pUserInfo = GetFinalReportUser(sessionID);
	if (pUserInfo == NULL)
		return;
	pUserInfo->tournamentStep = (tournamentStep <= 0) ? 256 : tournamentStep;
}

SFinalReportUserInfo* CDnPVPTournamentDataMgr::GetFinalReportUser(UINT sessionID)
{
	std::vector<SFinalReportUserInfo>::iterator iter = m_FinalReportUserList.begin();
	for (; iter != m_FinalReportUserList.end(); ++iter)
	{
		SFinalReportUserInfo& curInfo = (*iter);
		if (curInfo.nSessionID == sessionID)
			return &(curInfo);
	}

	return NULL;
}

void CDnPVPTournamentDataMgr::SortFinalReportUserList()
{
	std::sort(m_FinalReportUserList.begin(), m_FinalReportUserList.end(), Compare_PVPTournamentStep);
}

void CDnPVPTournamentDataMgr::SetFinalReportUser_WinnersRank(UINT* winnersSessionId, int count)
{
	if (count <= 0 || winnersSessionId == NULL)
	{
		_ASSERT(0);
		return;
	}

	if (m_FinalReportUserList.empty())
		return;

	int i = 0;
	for (; i < count; ++i)
	{
		UINT curSessionId = winnersSessionId[i];

		if (curSessionId != 0)
		{
			SFinalReportUserInfo* pCurrentInfo = GetFinalReportUser(curSessionId);
			if (pCurrentInfo == NULL)
				continue;

			pCurrentInfo->winnersRank = i + 1;
		}
	}
}

#endif // PRE_ADD_PVP_TOURNAMENT