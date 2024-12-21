#include "stdafx.h"
#include "DnUIDefine.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

void PvPTournamentUIDef::SBasicRoomInfo::Reset()
{
	uiMapIndex = 0;
	uiGameModeID = 0;
	uiRoomState = 0;
	uiWinCondition = 0;
	uiRoomIndex = 0;
	nEventRoomID = 0;

	cMaxLevel = 0;
	cMinLevel = 0;
	cMaxPlayerNum = 0;

	bBreakIntoFlag = false;
	bDropItem = false;
	bRandomTeam = false;

	bShowHp = false;
	bRevision = false;

	roomName = L"";
#ifdef PRE_WORLDCOMBINE_PVP
	nWorldPvPRoomType = 0;
#endif // PRE_WORLDCOMBINE_PVP

#ifdef PRE_PVP_GAMBLEROOM
	cGambleType = PvPGambleRoom::NoneType;
	nGamblePrice = 0;
#endif // PRE_PVP_GAMBLEROOM
}

void PvPTournamentUIDef::SBasicRoomInfo::Set(const PvPCommon::RoomInfo& info)
{
	uiMapIndex = info.uiMapIndex;
	uiGameModeID = info.uiGameModeTableID;
	bBreakIntoFlag = (info.unRoomOptionBit & PvPCommon::RoomOption::BreakInto) ? true : false;
	bDropItem = (info.unRoomOptionBit & PvPCommon::RoomOption::DropItem) ? true : false;
	bShowHp = (info.unRoomOptionBit & PvPCommon::RoomOption::ShowHP) ? true : false;
	bRevision = (info.unRoomOptionBit & PvPCommon::RoomOption::NoRegulation) ? true : false;
	uiRoomState = info.cRoomState;
	uiRoomIndex = info.uiIndex;
	cMaxPlayerNum = info.cMaxUserCount;
	bRandomTeam = (info.unRoomOptionBit & PvPCommon::RoomOption::RandomTeam) ? true : false;
	nEventRoomID = info.nEventID;

	cMinLevel = info.cMinLevel;
	cMaxLevel = info.cMaxLevel;

	uiWinCondition = info.uiWinCondition;

	roomName = info.wszBuffer;
#ifdef PRE_WORLDCOMBINE_PVP
	nWorldPvPRoomType = info.nWorldPvPRoomType;
#endif // PRE_WORLDCOMBINE_PVP
#ifdef PRE_PVP_GAMBLEROOM
	cGambleType = info.cGambleType;
	nGamblePrice = info.nGamblePrice;
#endif // PRE_PVP_GAMBLEROOM
}

PvPTournamentUIDef::SMatchUserInfo::SMatchUserInfo()
{
	commonIndex = -1;
	matchListUIIndex = -1;
	slotState = CDnBaseRoomDlg::Open;
	nJobID = 0;
	cLevel = 0;
	cPVPLevel = 0;
	uiUserState = 0;
	uiSessionID = 0;
	bIsNeedUpdate = false;
	bShow = false;
#if defined( PRE_ADD_REVENGE )
	uiRevengeUserID = 0;
#endif	// #if defined( PRE_ADD_REVENGE )
}

PvPTournamentUIDef::SGameMatchUserInfo::SGameMatchUserInfo()
{
	bCurrentMatchUser = false;
	step = -1;
	bWin = false;
	jobId = 0;
	sessionId = 0;
}

PvPTournamentUIDef::SFinalReportUserInfo::SFinalReportUserInfo()
{
	nSessionID = 0;
	nLevel = 0;
	nState = 0;
	cJobClassID = 0;
	cPVPlevel = 0;
	uiKOCount = 0;
	uiKObyCount = 0;
	uiKOP = 0;
	uiAssistP = 0;
	uiTotalScore = 0;
	uiXP = 0;
	uiTotalXP = 0;
	uiMedal = 0;
	uiBossKOCount = 0;
#if defined( PRE_ADD_REVENGE )
	eRevengeUser = Revenge::RevengeTarget::eRevengeTarget_None;
#endif	// #if defined( PRE_ADD_REVENGE )
	tournamentStep = 256;
	winnersRank = 0;
}