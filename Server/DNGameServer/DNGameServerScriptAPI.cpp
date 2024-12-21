#include "Stdafx.h"
#include "DNGameServerScriptAPI.h"
#include "DNGameServerManager.h"
#include "DNRUDPGameServer.h"
#include "DNUserSession.h"
#include "DNGameRoom.h"
#include "DnWorld.h"
#include "DnWorldGrid.h"
#include "DnWorldSector.h"
#include "DnWorldProp.h"
#include "DnWorldActProp.h"
#include "DnWorldBrokenProp.h"
#include "EtWorldEventControl.h"
#include "EtWorldEventArea.h"
#include "DNNpc.h"
#include "DNGameDataManager.h"
#include "TaskManager.h"
#include "DnGameTask.h"
#include "DnPlayerActor.h"
#include "DnLuaAPIDefine.h"
#include "DnMonsterActor.h"
#include "DNReplaceString.h"
#include "EtUIXML.h"
#include "PerfCheck.h"
#include "DnTriggerElement.h"
#include "DnTriggerObject.h"
#include "DnNPCActor.h"
#include "DNLogConnection.h"
#include "DnItemTask.h"
#include "MAActorProp.h"
#include "DnDLGameTask.h"
#include "DNQuestManager.h"
#include "DnWorldNpcProp.h"
#include "DnPartyTask.h"
#include "TimeSet.h"
#include "DnStateBlow.h"
#include "PvPGameMode.h"
#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
#include "ReputationSystemRepository.h"
#include "NpcReputationProcessor.h"
#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
#include "DNAggroSystem.h"
#include "DNFarmUserSession.h"
#include "DNUserItem.h"
#include "DnWorld.h"
#include "DNScriptAPICommon.h"
#include "DNMissionSystem.h"
#include "DnWorldKeepOperationProp.h"
#if defined(PRE_ADD_POSITIONHACK_POS_LOG)
#include "DnPlayerSpeedHackChecker.h"
#endif	// #if defined(PRE_ADD_POSITIONHACK_POS_LOG)
#if !defined(_FINAL_BUILD)
extern CLog g_ScriptLog;
#endif	// #if !defined(_FINAL_BUILD)

int CheckInvenForQuestReward(CDNUserBase* pUser, const TQuestReward& QuestReward, const bool bSelectedArray[])
{
	return DNScriptAPI::CheckInvenForQuestReward(pUser, QuestReward, bSelectedArray);
}

void DefAllAPIFunc(lua_State* pLuaState)
{
	DefineLuaAPI(pLuaState);

	//-- npc & quest
	LUA_TINKER_DEF(pLuaState, api_npc_NextTalk);
	LUA_TINKER_DEF(pLuaState, api_npc_NextScript);

	LUA_TINKER_DEF(pLuaState, api_npc_SetParamString);
	LUA_TINKER_DEF(pLuaState, api_npc_SetParamInt);
	LUA_TINKER_DEF(pLuaState, api_log_AddLog);
	LUA_TINKER_DEF(pLuaState, api_log_UserLog);
	LUA_TINKER_DEF(pLuaState, api_quest_DumpQuest);

	LUA_TINKER_DEF(pLuaState, api_quest_AddHuntingQuest);
	LUA_TINKER_DEF(pLuaState, api_quest_AddQuest);
	LUA_TINKER_DEF(pLuaState, api_quest_UserHasQuest);
	LUA_TINKER_DEF(pLuaState, api_quest_CompleteQuest);
	LUA_TINKER_DEF(pLuaState, api_quest_MarkingCompleteQuest);
	LUA_TINKER_DEF(pLuaState, api_quest_IsMarkingCompleteQuest);


	LUA_TINKER_DEF(pLuaState, api_npc_GetNpcIndex);
	LUA_TINKER_DEF(pLuaState, api_quest_SetQuestStepAndJournalStep);
	LUA_TINKER_DEF(pLuaState, api_quest_SetQuestStep);
	LUA_TINKER_DEF(pLuaState, api_quest_GetQuestStep);
	LUA_TINKER_DEF(pLuaState, api_quest_SetJournalStep);
	LUA_TINKER_DEF(pLuaState, api_quest_GetJournalStep);
	LUA_TINKER_DEF(pLuaState, api_quest_SetQuestMemo);
	LUA_TINKER_DEF(pLuaState, api_quest_GetQuestMemo);

	LUA_TINKER_DEF(pLuaState, api_quest_SetCountingInfo);
	LUA_TINKER_DEF(pLuaState, api_quest_ClearCountingInfo);
	LUA_TINKER_DEF(pLuaState, api_quest_IsAllCompleteCounting);
	LUA_TINKER_DEF(pLuaState, api_user_CheckInvenForAddItem);
	LUA_TINKER_DEF(pLuaState, api_user_CheckInvenForAddItemList);
	LUA_TINKER_DEF(pLuaState, api_user_AddItem);
	LUA_TINKER_DEF(pLuaState, api_user_DelItem);
	LUA_TINKER_DEF(pLuaState, api_user_AllDelItem);
	LUA_TINKER_DEF(pLuaState, api_user_HasItem);

	LUA_TINKER_DEF(pLuaState, api_user_GetUserClassID);
	LUA_TINKER_DEF(pLuaState, api_user_GetUserJobID);
	LUA_TINKER_DEF(pLuaState, api_user_GetUserLevel);
	LUA_TINKER_DEF(pLuaState, api_user_GetUserInvenBlankCount);
	LUA_TINKER_DEF(pLuaState, api_quest_GetUserQuestInvenBlankCount);

	LUA_TINKER_DEF(pLuaState, api_user_AddExp);
	LUA_TINKER_DEF(pLuaState, api_user_AddCoin);
	LUA_TINKER_DEF(pLuaState, api_user_PlayCutScene);
	LUA_TINKER_DEF(pLuaState, api_ui_OpenWareHouse);
	LUA_TINKER_DEF(pLuaState, api_ui_OpenShop);
	LUA_TINKER_DEF(pLuaState, api_ui_OpenSkillShop);
	LUA_TINKER_DEF(pLuaState, api_ui_OpenMarket);
	LUA_TINKER_DEF(pLuaState, api_ui_OpenCompoundEmblem);
	LUA_TINKER_DEF(pLuaState, api_ui_OpenUpgradeJewel);
	LUA_TINKER_DEF(pLuaState, api_ui_OpenMailBox);
	LUA_TINKER_DEF(pLuaState, api_ui_OpenDisjointItem);
	LUA_TINKER_DEF(pLuaState, api_ui_OpenCompoundItem);
	LUA_TINKER_DEF(pLuaState, api_ui_OpenCompound2Item);
	LUA_TINKER_DEF(pLuaState, api_ui_OpenCashShop);
	LUA_TINKER_DEF(pLuaState, api_ui_OpenGuildMgrBox);
	LUA_TINKER_DEF(pLuaState, api_ui_OpenGacha_JP);
	LUA_TINKER_DEF(pLuaState, api_ui_OpenGiveNpcPresent);
	LUA_TINKER_DEF(pLuaState, api_ui_OpenFarmWareHouse);
#if defined(PRE_ADD_OPEN_QUEST_TEXTURE_DIALOG)
	LUA_TINKER_DEF(pLuaState, api_ui_OpenTextureDialog);
	LUA_TINKER_DEF(pLuaState, api_ui_CloseTextureDialog);
#endif
	

	LUA_TINKER_DEF(pLuaState, api_user_UserMessage);
	LUA_TINKER_DEF(pLuaState, api_quest_AddSymbolItem);
	LUA_TINKER_DEF(pLuaState, api_quest_DelSymbolItem);
	LUA_TINKER_DEF(pLuaState, api_quest_HasSymbolItem);

	LUA_TINKER_DEF(pLuaState, api_quest_CheckQuestInvenForAddItem);
	LUA_TINKER_DEF(pLuaState, api_quest_CheckQuestInvenForAddItemList);
	LUA_TINKER_DEF(pLuaState, api_quest_AddQuestItem);
	LUA_TINKER_DEF(pLuaState, api_quest_DelQuestItem);
	LUA_TINKER_DEF(pLuaState, api_quest_AllDelQuestItem);
	LUA_TINKER_DEF(pLuaState, api_quest_HasQuestItem);

	LUA_TINKER_DEF(pLuaState, api_ui_OpenGuildCreate);
	LUA_TINKER_DEF(pLuaState, api_ui_OpenQuestReward);
	LUA_TINKER_DEF(pLuaState, api_quest_RewardQuestUser);
	LUA_TINKER_DEF(pLuaState, api_user_GetPartymemberCount);
	LUA_TINKER_DEF(pLuaState, api_user_IsPartymember);
	LUA_TINKER_DEF(pLuaState, api_user_ChangeMap);
	LUA_TINKER_DEF(pLuaState, api_user_GetStageConstructionLevel);
	LUA_TINKER_DEF(pLuaState, api_user_GetMapIndex);
	LUA_TINKER_DEF(pLuaState, api_user_GetLastStageClearRank);
	LUA_TINKER_DEF(pLuaState, api_user_EnoughCoin);
	LUA_TINKER_DEF(pLuaState, api_user_GetCoin);
	LUA_TINKER_DEF(pLuaState, api_user_DelCoin);
	LUA_TINKER_DEF(pLuaState, api_user_IsMissionGained);
	LUA_TINKER_DEF(pLuaState, api_user_IsMissionAchieved);
	LUA_TINKER_DEF(pLuaState, api_user_HasItemWarehouse);
	LUA_TINKER_DEF(pLuaState, api_user_HasItemEquip);
	LUA_TINKER_DEF(pLuaState, api_guild_GetGuildMemberRole);
	LUA_TINKER_DEF(pLuaState, api_quest_IncQuestMemo);
	LUA_TINKER_DEF(pLuaState, api_quest_DecQuestMemo);
	LUA_TINKER_DEF(pLuaState, api_user_SetUserJobID);
	LUA_TINKER_DEF(pLuaState, api_user_IsJobInSameLine);
	LUA_TINKER_DEF(pLuaState, api_user_HasCashItem);
	LUA_TINKER_DEF(pLuaState, api_user_HasCashItemEquip);
	LUA_TINKER_DEF(pLuaState, api_quest_IncCounting);
	LUA_TINKER_DEF(pLuaState, api_quest_IsPlayingQuestMaximum);
	LUA_TINKER_DEF(pLuaState, api_quest_ForceCompleteQuest);
	LUA_TINKER_DEF(pLuaState, api_user_ChangePos);
	LUA_TINKER_DEF(pLuaState, api_ui_OpenUpgradeItem);
	LUA_TINKER_DEF(pLuaState, api_user_RequestEnterPVP);

	LUA_TINKER_DEF(pLuaState, api_npc_GetFavorPoint);
	LUA_TINKER_DEF(pLuaState, api_npc_GetMalicePoint);
	LUA_TINKER_DEF(pLuaState, api_npc_GetFavorPercent);
	LUA_TINKER_DEF(pLuaState, api_npc_GetMalicePercent);
	LUA_TINKER_DEF(pLuaState, api_npc_AddFavorPoint);
	LUA_TINKER_DEF(pLuaState, api_npc_AddMalicePoint);
	LUA_TINKER_DEF(pLuaState, api_npc_SendSelectedPresent);
	LUA_TINKER_DEF(pLuaState, api_npc_Rage);
	LUA_TINKER_DEF(pLuaState, api_npc_Disappoint);

	LUA_TINKER_DEF(pLuaState, api_user_ResetSkill);
	LUA_TINKER_DEF(pLuaState, api_user_SetSecondJobSkill);
	LUA_TINKER_DEF(pLuaState, api_user_ClearSecondJobSkill);
	LUA_TINKER_DEF(pLuaState, api_user_RepairItem);
	LUA_TINKER_DEF(pLuaState, api_ui_OpenExchangeEnchant);

	//-- 트리거 
	LUA_TINKER_DEF(pLuaState, api_trigger_GetPartyCount);
	LUA_TINKER_DEF(pLuaState, api_trigger_GetPartyActor);
	LUA_TINKER_DEF(pLuaState, api_trigger_IsInsidePlayerToEventArea);
	LUA_TINKER_DEF(pLuaState, api_trigger_SetActionToProp);
	LUA_TINKER_DEF(pLuaState, api_trigger_ShowDungeonClear);
	LUA_TINKER_DEF(pLuaState, api_trigger_SetPermitGate);
	LUA_TINKER_DEF(pLuaState, api_trigger_GetMonsterAreaLiveCount);
	LUA_TINKER_DEF(pLuaState, api_trigger_RebirthMonster);
	LUA_TINKER_DEF(pLuaState, api_trigger_GenerationMonster);
	LUA_TINKER_DEF(pLuaState, api_trigger_GenerationMonsterGroup);
	LUA_TINKER_DEF(pLuaState, api_trigger_CmdMoveMonster);
	LUA_TINKER_DEF(pLuaState, api_trigger_CmdRandMoveMonster);
	LUA_TINKER_DEF(pLuaState, api_trigger_CheckMonsterIsInsideArea);
	LUA_TINKER_DEF(pLuaState, api_trigger_CheckMonsterLessHP);
	LUA_TINKER_DEF(pLuaState, api_trigger_CheckInsideAreaMonsterLessHP);
	
	LUA_TINKER_DEF(pLuaState, api_trigger_GenerationNpc);
	LUA_TINKER_DEF(pLuaState, api_trigger_DestroyNpc);	
	LUA_TINKER_DEF(pLuaState, api_trigger_SetGameSpeed);
	LUA_TINKER_DEF(pLuaState, api_trigger_ShowProp);
	LUA_TINKER_DEF(pLuaState, api_trigger_IsAllDieMonster);
	LUA_TINKER_DEF(pLuaState, api_trigger_GetLastAimPropHandle);	
	LUA_TINKER_DEF(pLuaState, api_trigger_GetLastAimPropActor);
	LUA_TINKER_DEF(pLuaState, api_trigger_IsBrokenProp);
	LUA_TINKER_DEF(pLuaState, api_trigger_GetLastBrokenPropActor);
	LUA_TINKER_DEF(pLuaState, api_trigger_IsProp);
	LUA_TINKER_DEF(pLuaState, api_trigger_DestroyMonsterArea);
	LUA_TINKER_DEF(pLuaState, api_trigger_DestroyAllMonster);
	LUA_TINKER_DEF(pLuaState, api_trigger_SetMonsterAIState);
	LUA_TINKER_DEF(pLuaState, api_trigger_PlayCutSceneByTrigger);
	LUA_TINKER_DEF(pLuaState, api_trigger_GetGameTime);
	LUA_TINKER_DEF(pLuaState, api_trigger_Delay);
	LUA_TINKER_DEF(pLuaState, api_trigger_GetBattleModeCount);
	LUA_TINKER_DEF(pLuaState, api_trigger_GetActionToProp);
	LUA_TINKER_DEF(pLuaState, api_trigger_ChangeMapByTrigger);

	LUA_TINKER_DEF(pLuaState, api_trigger_GetTalkNpcClassID);
	LUA_TINKER_DEF(pLuaState, api_trigger_GetTalkNpcActor);
	LUA_TINKER_DEF(pLuaState, api_trigger_GetTalkNpcTargetActor);
	LUA_TINKER_DEF(pLuaState, api_trigger_EnableOperatorProp);
	LUA_TINKER_DEF(pLuaState, api_trigger_AllUserEnableOperatorProp);
	LUA_TINKER_DEF(pLuaState, api_trigger_SetActionToPropActor);
	LUA_TINKER_DEF(pLuaState, api_trigger_EnableTriggerElement);

	LUA_TINKER_DEF(pLuaState, api_trigger_SetActionToActor);
	LUA_TINKER_DEF(pLuaState, api_trigger_EnableOperatorNpc);
	LUA_TINKER_DEF(pLuaState, api_trigger_EnableTriggerObject);
	LUA_TINKER_DEF(pLuaState, api_trigger_EnableMonsterNoDamage);

	LUA_TINKER_DEF(pLuaState, api_trigger_WarpActor);
	LUA_TINKER_DEF(pLuaState, api_trigger_WarpActorFromEventArea);
	LUA_TINKER_DEF(pLuaState, api_trigger_IsInsidePropToEventArea);
	LUA_TINKER_DEF(pLuaState, api_trigger_ChangeTeamFromEventArea);
	LUA_TINKER_DEF(pLuaState, api_trigger_GetUserLevelByTrigger);
	LUA_TINKER_DEF(pLuaState, api_trigger_IsDieActor);
	LUA_TINKER_DEF(pLuaState, api_trigger_GetMonsterActorFromEventArea);
	LUA_TINKER_DEF(pLuaState, api_trigger_DropItemToEventArea);
	LUA_TINKER_DEF(pLuaState, api_trigger_DropItemToActor);
	LUA_TINKER_DEF(pLuaState, api_trigger_SetForceAggroToMonsterArea);
	LUA_TINKER_DEF(pLuaState, api_trigger_SetForceAggroToProp);
	LUA_TINKER_DEF(pLuaState, api_trigger_ShowChatBalloonToMonsterArea);
	LUA_TINKER_DEF(pLuaState, api_trigger_ShowChatBalloonToActor);
	LUA_TINKER_DEF(pLuaState, api_trigger_ShowChatBalloonToProp);
	LUA_TINKER_DEF(pLuaState, api_trigger_HideChatBalloonToMonsterArea);
	LUA_TINKER_DEF(pLuaState, api_trigger_HideChatBalloonToProp);
	LUA_TINKER_DEF(pLuaState, api_trigger_ShowExposureInfo);
	LUA_TINKER_DEF(pLuaState, api_trigger_GenerationMonsterSet);
	LUA_TINKER_DEF(pLuaState, api_trigger_GenerationMonsterSetInEventArea);
	LUA_TINKER_DEF(pLuaState, api_trigger_DLChangeRound);
	LUA_TINKER_DEF(pLuaState, api_trigger_DLGetRound);
	LUA_TINKER_DEF(pLuaState, api_trigger_DLGetTotalRound);
	LUA_TINKER_DEF(pLuaState, api_trigger_DLGetStartFloor);
	LUA_TINKER_DEF(pLuaState, api_trigger_DLRequestChallenge);
	LUA_TINKER_DEF(pLuaState, api_trigger_DLResponseChallenge);
	LUA_TINKER_DEF(pLuaState, api_trigger_DLSetStartRound);
	LUA_TINKER_DEF(pLuaState, api_trigger_EnableDungeonClearWarpQuestFlag);
	LUA_TINKER_DEF(pLuaState, api_trigger_IsEnableDungeonClearWarp);
	LUA_TINKER_DEF(pLuaState, api_trigger_IsExistNotAchieveMissionPartyActor);
	LUA_TINKER_DEF(pLuaState, api_trigger_GetChangeActionPlayerActor);
	LUA_TINKER_DEF(pLuaState, api_trigger_GetActionToPlayerActor);
	LUA_TINKER_DEF(pLuaState, api_trigger_EnablePropNoDamage);

	LUA_TINKER_DEF(pLuaState, api_trigger_GetTriggerEventSignalEventAreaHandle);
	LUA_TINKER_DEF(pLuaState, api_trigger_GetTriggerEventSignalActorHandle);
	LUA_TINKER_DEF(pLuaState, api_trigger_GetTriggerEventSignalEventID);
	LUA_TINKER_DEF(pLuaState, api_trigger_GetTriggerEventSignalTeam);
	LUA_TINKER_DEF(pLuaState, api_trigger_GetMonsterAreaTotalCount);
	LUA_TINKER_DEF(pLuaState, api_trigger_HoldPartyDice);
	LUA_TINKER_DEF(pLuaState, api_trigger_ReleasePartyDice);
	LUA_TINKER_DEF(pLuaState, api_trigger_UpdateNestTryCount);
	LUA_TINKER_DEF(pLuaState, api_trigger_UpdateUserNestTryCount);
	LUA_TINKER_DEF(pLuaState, api_trigger_GetMonsterClassID);
	LUA_TINKER_DEF(pLuaState, api_trigger_EnableDungeonClearWarpAlarm);

	LUA_TINKER_DEF(pLuaState, api_trigger_SetHPFromEventArea);
	LUA_TINKER_DEF(pLuaState, api_trigger_SetHP);
	LUA_TINKER_DEF(pLuaState, api_trigger_AddHPAbsoluteFromEventArea);
	LUA_TINKER_DEF(pLuaState, api_trigger_AddHPRatioFromEventArea);
	LUA_TINKER_DEF(pLuaState, api_trigger_AddHPAbsolute);
	LUA_TINKER_DEF(pLuaState, api_trigger_AddHPRatio);
	LUA_TINKER_DEF(pLuaState, api_trigger_GetTeamFromEventArea);
	LUA_TINKER_DEF(pLuaState, api_trigger_GetTeam);
	LUA_TINKER_DEF(pLuaState, api_trigger_EnableOperatorNpcFromPropNpc);
	LUA_TINKER_DEF(pLuaState, api_trigger_SetHPRatioFromEventArea);
	LUA_TINKER_DEF(pLuaState, api_trigger_SetHPRatio);
	LUA_TINKER_DEF(pLuaState, api_trigger_IgnoreAggroFromEventArea);
	LUA_TINKER_DEF(pLuaState, api_trigger_GetPropDurability);
	LUA_TINKER_DEF(pLuaState, api_trigger_LastAimPropActorMessage);
	LUA_TINKER_DEF(pLuaState, api_trigger_BuffFromEventArea);
	LUA_TINKER_DEF(pLuaState, api_trigger_ActorBuffFromEventArea);
	LUA_TINKER_DEF(pLuaState, api_trigger_NoDuplicateBuffFromEventArea);
	LUA_TINKER_DEF(pLuaState, api_trigger_Start_DragonNest);
	LUA_TINKER_DEF(pLuaState, api_trigger_SetUsableRebirthCoin);
	LUA_TINKER_DEF(pLuaState, api_trigger_GetMonsterHPPercent_FromEventArea);
	LUA_TINKER_DEF(pLuaState, api_trigger_UseSkillFromEventArea);
	LUA_TINKER_DEF(pLuaState, api_trigger_UseSkill);
	LUA_TINKER_DEF(pLuaState, api_trigger_NotUseManaSkill);
	LUA_TINKER_DEF(pLuaState, api_trigger_AllUser_AddVehicleItem);
	LUA_TINKER_DEF(pLuaState, api_trigger_PlayerNameShowBlindCaption);
	LUA_TINKER_DEF(pLuaState, api_trigger_InvenToEquip);
	LUA_TINKER_DEF(pLuaState, api_trigger_UnRideVehicle);
	LUA_TINKER_DEF(pLuaState, api_trigger_ChangeMyBGM);
	LUA_TINKER_DEF(pLuaState, api_trigger_FileTableBGMOff);
	LUA_TINKER_DEF(pLuaState, api_trigger_RadioMsg);
	LUA_TINKER_DEF(pLuaState, api_trigger_RadioImage);
	LUA_TINKER_DEF(pLuaState, api_trigger_GetPvPGameModePlayTime);

	LUA_TINKER_DEF(pLuaState, api_trigger_DropGroupItemToEventArea);
	LUA_TINKER_DEF(pLuaState, api_trigger_DropGroupItemToActor);
	LUA_TINKER_DEF(pLuaState, api_trigger_TargetActorShowNpc);
	LUA_TINKER_DEF(pLuaState, api_trigger_TargetActorShowProp);
	LUA_TINKER_DEF(pLuaState, api_trigger_TriggerVariableTable);
	LUA_TINKER_DEF(pLuaState, api_trigger_GetPvPGameMode);
	LUA_TINKER_DEF(pLuaState, api_trigger_CmdKeepOperationProp);
	LUA_TINKER_DEF(pLuaState, api_trigger_ActivateMission);
	LUA_TINKER_DEF(pLuaState, api_trigger_SetDungeonClearRound);
	LUA_TINKER_DEF(pLuaState, api_trigger_BreakIntoUserCheck);
	LUA_TINKER_DEF(pLuaState, api_trigger_DungeonStartTimeAttack);
	LUA_TINKER_DEF(pLuaState, api_trigger_DungeonCheckTimeAttack);
	LUA_TINKER_DEF(pLuaState, api_trigger_DungeonStopTimeAttack);
	LUA_TINKER_DEF(pLuaState, api_trigger_RequestDungeonFail);
	LUA_TINKER_DEF(pLuaState, api_trigger_Special_RebirthItem);
	LUA_TINKER_DEF(pLuaState, api_trigger_GameWarpDungeonClearToLeader);
	LUA_TINKER_DEF(pLuaState, api_trigger_console_WriteLine);
	LUA_TINKER_DEF(pLuaState, api_trigger_GetPartyCountbyRoom);
	LUA_TINKER_DEF(pLuaState, api_trigger_GetPartyActorbyRoom);
	LUA_TINKER_DEF(pLuaState, api_trigger_GetRacingStartTime);
	LUA_TINKER_DEF(pLuaState, api_trigger_GetRacingGoalTime);
	LUA_TINKER_DEF(pLuaState, api_trigger_GetCurrentWeather);
	LUA_TINKER_DEF(pLuaState, api_trigger_StartDungeonGateTime);
	LUA_TINKER_DEF(pLuaState, api_trigger_EndDungeonGateTime);
	LUA_TINKER_DEF(pLuaState, api_trigger_StageRemainClearCount);	//스테이지의 남은 클리어 카운트
	LUA_TINKER_DEF(pLuaState, api_trigger_EnableRideVehicle);
	LUA_TINKER_DEF(pLuaState, api_trigger_AllMonsterUseSkill);	
	LUA_TINKER_DEF(pLuaState, api_trigger_RequireTotalComboCount);
	LUA_TINKER_DEF(pLuaState, api_trigger_RequireTotalComboReset);
	LUA_TINKER_DEF(pLuaState, api_trigger_GetPartyInClassCount);
	LUA_TINKER_DEF(pLuaState, api_trigger_Set_AlteiaWorld);
	LUA_TINKER_DEF(pLuaState, api_tirgger_Set_UserHPSP);
	LUA_TINKER_DEF(pLuaState, api_trigger_Set_ItemUseCount);
	//rlkt_test
	LUA_TINKER_DEF(pLuaState, api_trigger_CheckJobChangeProcess);
	LUA_TINKER_DEF(pLuaState, api_ui_OpenJobChange);
	LUA_TINKER_DEF(pLuaState, api_trigger_Emergence_GetValue);
	LUA_TINKER_DEF(pLuaState, api_trigger_Emergence_Begin);
	LUA_TINKER_DEF(pLuaState, api_trigger_Emergence_CheckSelectedEvent);

	LUA_TINKER_DEF(pLuaState, api_trigger_CheckForceMoveComebackRoom);

	
	

	
}

// desc : 인벤토리의 특정 빈슬롯에 아이템을 세팅 (P.S.> api_user_CheckInvenForAddItemList 에서 사용됨)
bool __SetBlankInventorySlot(TInvenItemCnt pTempInven[INVENTORYMAX], const TItem& Item, int iIndex)
{
	DN_ASSERT(NULL != pTempInven,	"Invalid!");
	DN_ASSERT(0 != Item.nItemID,			"Invalid!");
	DN_ASSERT(0 < Item.wCount,			"Invalid!");
	DN_ASSERT(-1 < iIndex,			"Invalid!");

	if (pTempInven[iIndex].nItemID != 0) {
		return false;
	}

	pTempInven[iIndex].Set(Item.nItemID, Item.wCount, Item.bSoulbound, Item.cSealCount );

	return true;
}

// desc : 인벤토리의 빈슬롯들에 아이템을 특정 개수만큼 세팅 (P.S.> api_user_CheckInvenForAddItemList 에서 사용됨)
bool __SetBlankInventorySlotCount(TInvenItemCnt pTempInven[INVENTORYMAX], const TItem& Item, int nOverlapCount, int nCount /* 채울 슬롯개수 */)
{
	DN_ASSERT(NULL != pTempInven,	"Invalid!");
	DN_ASSERT(0 != Item.nItemID,			"Invalid!");
	DN_ASSERT(0 < Item.wCount,			"Invalid!");
	DN_ASSERT(0 < nOverlapCount,	"Invalid!");
	DN_ASSERT(0 < nCount,			"Invalid!");

	// !!! 주의 - 본 함수 호출 전에 체크가 확실히 되었음을 전제하고 세팅하는 부분만 수행

	int nBundle = Item.wCount / nOverlapCount;
	int nRemain = Item.wCount % nOverlapCount;

	for (int iIndex = 0 ; INVENTORYMAX > iIndex ; ++iIndex) 
	{
		if (0 < nBundle) {
			if (pTempInven[iIndex].nItemID == 0) {
				pTempInven[iIndex].Set(Item.nItemID, nOverlapCount, Item.bSoulbound, Item.cSealCount );
				--nBundle;
				continue;
			}
		}

		if (0 < nRemain) 
		{
			if( CDNUserItem::bIsSameItem( &Item, &pTempInven[iIndex] ) && nOverlapCount >= pTempInven[iIndex].wCount + nRemain )
			{
				pTempInven[iIndex].wCount += nRemain;
				nRemain = 0;
				continue;
			}
		}
	}

	if (0 < nBundle) 
	{
		return false;
	}
	if (0 < nRemain) 
	{
		for (int iIndex = 0 ; INVENTORYMAX > iIndex ; ++iIndex) 
		{
			if (pTempInven[iIndex].nItemID == 0) {
				pTempInven[iIndex].Set( Item.nItemID, nRemain, Item.bSoulbound, Item.cSealCount );
				nRemain = 0;
				break;
			}
		}
	}
	if (0 < nRemain) {
		return false;
	}

	return true;
}

// desc : 인벤토리의 특정 슬롯에 아이템을 중첩 세팅 (P.S.> api_user_CheckInvenForAddItemList 에서 사용됨)
bool __SetOverlapInventorySlot(TInvenItemCnt pTempInven[INVENTORYMAX], const TItem& Item, int nOverlapCount, int iIndex)
{
	DN_ASSERT(NULL != pTempInven,	"Invalid!");
	DN_ASSERT(0 != Item.nItemID,			"Invalid!");
	DN_ASSERT(0 < Item.wCount,			"Invalid!");
	DN_ASSERT(0 < nOverlapCount,	"Invalid!");
	DN_ASSERT(-1 < iIndex,			"Invalid!");

	if( CDNUserItem::bIsDifferentItem( &Item, &pTempInven[iIndex] ) || nOverlapCount < pTempInven[iIndex].wCount + Item.wCount )
	{
		return false;
	}

	pTempInven[iIndex].wCount += Item.wCount;

	return true;
}

// desc : 인벤토리에 빈슬롯이 몇 개나 있는지 개수 찾기 (P.S.> api_user_CheckInvenForAddItemList 에서 사용됨)
int __FindBlankInventorySlot(TInvenItemCnt pTempInven[INVENTORYMAX], int nTempInvenCount)
{
	DN_ASSERT(NULL != pTempInven,								"Invalid!");
	DN_ASSERT(CHECK_RANGE(nTempInvenCount, 0, INVENTORYMAX),	"Invalid!");

	for (int iIndex = 0 ; nTempInvenCount > iIndex ; ++iIndex) 
	{
		if (pTempInven[iIndex].nItemID == 0) {
			return iIndex;
		}
	}

	return -1;
}

// desc : 인벤토리에 빈슬롯 찾기 (P.S.> api_user_CheckInvenForAddItemList 에서 사용됨)
int __FindBlankInventorySlotCount(TInvenItemCnt pTempInven[INVENTORYMAX], int nTempInvenCount)
{
	DN_ASSERT(NULL != pTempInven,								"Invalid!");
	DN_ASSERT(CHECK_RANGE(nTempInvenCount, 0, INVENTORYMAX),	"Invalid!");

	int nCount = 0;
	for (int iIndex = 0 ; nTempInvenCount > iIndex ; ++iIndex) {
		if (pTempInven[iIndex].nItemID == 0) {
			++nCount;
		}
	}

	return nCount;
}

// desc : 인벤토리에 셀 수 있는 아이템 중 남은 공간에 wCount 넣을 수 있는 Index 찾기 (P.S.> api_user_CheckInvenForAddItemList 에서 사용됨)
int __FindOverlapInventorySlot(TInvenItemCnt pTempInven[INVENTORYMAX], int nTempInvenCount, const TItem& Item )
{
	DN_ASSERT(NULL != pTempInven,								"Invalid!");
	DN_ASSERT(CHECK_RANGE(nTempInvenCount, 0, INVENTORYMAX),	"Invalid!");

	if ((Item.nItemID <= 0) || (Item.wCount <= 0)) 
		DN_RETURN(-1);

	TItemData *pItemData = NULL;
	for (int iIndex = 0 ; nTempInvenCount > iIndex ; ++iIndex) 
	{
		if (pTempInven[iIndex].nItemID == 0) {
			// 빈칸이면 패스
			continue;
		}

		pItemData = g_pDataManager->GetItemData(pTempInven[iIndex].nItemID);
		if (!pItemData) continue;

		if (pItemData->nOverlapCount == 1) continue;			// 겹치지 않으면 패스
		if( CDNUserItem::bIsDifferentItem( &Item, &pTempInven[iIndex] ) ) continue;	// 같은 아이템이 아님

		if (pItemData->nOverlapCount >= pTempInven[iIndex].wCount + Item.wCount) 
		{
			return iIndex;
		}
	}

	return -1;
}

// desc : 인벤토리에 아이템을 넣을 수 있는 공간이 충분한지 검사 (P.S.> api_user_CheckInvenForAddItemList 에서 사용됨)
bool __IsValidSpaceInventorySlotAndSet(TInvenItemCnt pTempInven[INVENTORYMAX], int nTempInvenCount, const TItem& Item )
{
	DN_ASSERT(CHECK_RANGE(nTempInvenCount, 0, INVENTORYMAX),	"Invalid!");

	if ((Item.nItemID <= 0) || (Item.wCount <= 0)) DN_RETURN(false);

	TItemData *pItem = g_pDataManager->GetItemData(Item.nItemID);
	if (!pItem) return false;

	int nCount = 0;
	if (pItem->nOverlapCount == 1){		// 겹치지 않는 아이템
		nCount = __FindBlankInventorySlotCount(pTempInven, nTempInvenCount);
		if (Item.wCount > nCount) {
			// 빈슬롯이랑 비교하기
			return false;
		}

		return(__SetBlankInventorySlotCount(pTempInven, Item, 1, nCount));
	}
	else 
	{
		// 겹쳐지는 아이템 (인벤에 동일 아이템들이 몇개씩 남아있어도 신경안쓰고 깔끔하게 넣는 경우만 생각하쟈)
		if (pItem->nOverlapCount < Item.wCount){		// 한번에 겹치는 양보다 많을경우
			int nBundle = Item.wCount / pItem->nOverlapCount;
			if ((Item.wCount % pItem->nOverlapCount) > 0) nBundle++;

			nCount = __FindBlankInventorySlotCount(pTempInven, nTempInvenCount);
			if (nBundle > nCount) {
				// 묶음이랑 빈슬롯이랑 비교하기
				return false;
			}

			return(__SetBlankInventorySlotCount(pTempInven, Item, pItem->nOverlapCount, nCount));
		}
		else {
			int nBlank = __FindOverlapInventorySlot(pTempInven, nTempInvenCount, Item );	// 기존 아이템중에 한방에 들어갈 아이템이 있는지 
			if (nBlank < 0){
				nBlank = __FindBlankInventorySlot(pTempInven, nTempInvenCount);
				if (nBlank < 0) {
					return false;
				}

				return(__SetBlankInventorySlot(pTempInven, Item, nBlank));
			}

			return(__SetOverlapInventorySlot(pTempInven, Item, pItem->nOverlapCount, nBlank));
		}
	}

	return true;
}

// desc : 퀘스트 인벤토리의 특정 빈슬롯에 아이템을 세팅 (P.S.> api_quest_CheckQuestInvenForAddItemList 에서 사용됨)
bool __SetBlankQuestInventorySlot(TQuestItemCnt pTempQuestInven[QUESTINVENTORYMAX], int nItemID, short wCount, int iIndex)
{
	DN_ASSERT(NULL != pTempQuestInven,	"Invalid!");
	DN_ASSERT(0 != nItemID,				"Invalid!");
	DN_ASSERT(0 < wCount,				"Invalid!");
	DN_ASSERT(-1 < iIndex,				"Invalid!");

	if (pTempQuestInven[iIndex].nItemID != 0) {
		return false;
	}

	pTempQuestInven[iIndex].Set(nItemID, wCount);

	return true;
}

// desc : 퀘스트 인벤토리의 빈슬롯들에 아이템을 특정 개수만큼 세팅 (P.S.> api_quest_CheckQuestInvenForAddItemList 에서 사용됨)
bool __SetBlankQuestInventorySlotCount(TQuestItemCnt pTempQuestInven[QUESTINVENTORYMAX], int nItemID, short wCount, int nOverlapCount, int nCount /* 채울 슬롯개수 */)
{
	DN_ASSERT(NULL != pTempQuestInven,	"Invalid!");
	DN_ASSERT(0 != nItemID,				"Invalid!");
	DN_ASSERT(0 < wCount,				"Invalid!");
	DN_ASSERT(0 < nOverlapCount,		"Invalid!");
	DN_ASSERT(0 < nCount,				"Invalid!");

	// !!! 주의 - 본 함수 호출 전에 체크가 확실히 되었음을 전제하고 세팅하는 부분만 수행

	int nBundle = wCount / nOverlapCount;
	int nRemain = wCount % nOverlapCount;

	for (int iIndex = 0 ; QUESTINVENTORYMAX > iIndex ; ++iIndex) {
		if (0 < nBundle) {
			if (pTempQuestInven[iIndex].nItemID == 0) {
				pTempQuestInven[iIndex].Set(nItemID, nOverlapCount);
				--nBundle;
				continue;
			}
		}

		if (0 < nRemain) {
			if (pTempQuestInven[iIndex].nItemID == nItemID &&
				nOverlapCount >= pTempQuestInven[iIndex].wCount + nRemain
				)
			{
				pTempQuestInven[iIndex].wCount += nRemain;
				nRemain = 0;
				continue;
			}
		}
	}

	if (0 < nBundle) {
		return false;
	}
	if (0 < nRemain) {
		for (int iIndex = 0 ; QUESTINVENTORYMAX > iIndex ; ++iIndex) {
			if (pTempQuestInven[iIndex].nItemID == 0) {
				pTempQuestInven[iIndex].Set(nItemID, nRemain);
				nRemain = 0;
				break;
			}
		}
	}
	if (0 < nRemain) {
		return false;
	}

	return true;
}

// desc : 퀘스트 인벤토리의 특정 슬롯에 아이템을 중첩 세팅 (P.S.> api_quest_CheckQuestInvenForAddItemList 에서 사용됨)
bool __SetOverlapQuestInventorySlot(TQuestItemCnt pTempQuestInven[QUESTINVENTORYMAX], int nItemID, short wCount, int nOverlapCount, int iIndex)
{
	DN_ASSERT(NULL != pTempQuestInven,	"Invalid!");
	DN_ASSERT(0 != nItemID,				"Invalid!");
	DN_ASSERT(0 < wCount,				"Invalid!");
	DN_ASSERT(0 < nOverlapCount,		"Invalid!");
	DN_ASSERT(-1 < iIndex,				"Invalid!");

	if (pTempQuestInven[iIndex].nItemID != nItemID || 
		nOverlapCount < pTempQuestInven[iIndex].wCount + wCount
		)
	{
		return false;
	}

	pTempQuestInven[iIndex].wCount += wCount;

	return true;
}

// desc : 퀘스트 인벤토리에 빈슬롯이 몇 개나 있는지 개수 찾기 (P.S.> api_quest_CheckQuestInvenForAddItemList 에서 사용됨)
int __FindBlankQuestInventorySlotCount(TQuestItemCnt pTempQuestInven[QUESTINVENTORYMAX])
{
	DN_ASSERT(NULL != pTempQuestInven,	"Invalid!");

	int nCount = 0;
	for (int iIndex = 0 ; QUESTINVENTORYMAX > iIndex ; ++iIndex) {
		if (pTempQuestInven[iIndex].nItemID == 0) {
			++nCount;
		}
	}

	return nCount;
}

// desc : 퀘스트 인벤토리에 빈슬롯 찾기 (P.S.> api_quest_CheckQuestInvenForAddItemList 에서 사용됨)
int __FindBlankQuestInventorySlot(TQuestItemCnt pTempQuestInven[QUESTINVENTORYMAX])
{
	DN_ASSERT(NULL != pTempQuestInven,	"Invalid!");

	for (int iIndex = 0 ; QUESTINVENTORYMAX > iIndex ; ++iIndex) {
		if (pTempQuestInven[iIndex].nItemID == 0) {
			return iIndex;
		}
	}

	return -1;
}

// desc : 퀘스트 인벤토리에 셀 수 있는 아이템 중 남은 공간에 wCount 넣을 수 있는 Index 찾기 (P.S.> api_quest_CheckQuestInvenForAddItemList 에서 사용됨)
int __FindOverlapQuestInventorySlot(TQuestItemCnt pTempQuestInven[QUESTINVENTORYMAX], int nItemID, short wCount)
{
	DN_ASSERT(NULL != pTempQuestInven,	"Invalid!");

	if ((nItemID <= 0) || (wCount <= 0)) DN_RETURN(-1);

	bool boFlag = false;
	TItemData *pQuestItem = NULL;
	for (int iIndex = 0 ; QUESTINVENTORYMAX > iIndex ; ++iIndex) {
		if (pTempQuestInven[iIndex].nItemID == 0) continue;	// 빈칸이면 패스

		pQuestItem = g_pDataManager->GetItemData(pTempQuestInven[iIndex].nItemID);
		if (!pQuestItem) continue;

		if (pQuestItem->nOverlapCount == 1) continue;				// 겹치지 않으면 패스
		if (pTempQuestInven[iIndex].nItemID != nItemID) continue;	// 같은 아이템이 아니다

		if (pQuestItem->nOverlapCount >= pTempQuestInven[iIndex].wCount + wCount){
			return iIndex;
		}
	}

	return -1;
}

// desc : 퀘스트 인벤토리에 아이템을 넣을 수 있는 공간이 충분한지 검사 (P.S.> api_quest_CheckQuestInvenForAddItemList 에서 사용됨)
bool __IsValidSpaceQuestInventorySlotAndSet(TQuestItemCnt pTempQuestInven[QUESTINVENTORYMAX], int nItemID, short wCount)
{
	DN_ASSERT(NULL != pTempQuestInven,	"Invalid!");

	if ((nItemID <= 0) || (wCount <= 0)) DN_RETURN(false);

	TItemData *pItem = g_pDataManager->GetItemData(nItemID);
	if (!pItem) return false;

	int nCount = 0;
	if (pItem->nOverlapCount == 1){		// 겹치지 않는 아이템
		nCount = __FindBlankQuestInventorySlotCount(pTempQuestInven);
		if (wCount > nCount) {
			// 빈슬롯이랑 비교하기
			return false;
		}

		return(__SetBlankQuestInventorySlotCount(pTempQuestInven, nItemID, wCount, 1, nCount));
	}
	else {	// 겹치는 아이템
		int nRemain = 0, nBlank = 0, nBundle = 0;

		int nLocalCount = __FindBlankQuestInventorySlotCount(pTempQuestInven);	// 빈 슬롯개수

		if (pItem->nOverlapCount >= wCount){
			nRemain = wCount;
		}
		else {
			nBundle = wCount / pItem->nOverlapCount;
			nRemain = wCount % pItem->nOverlapCount;

			if (nBundle > nLocalCount) return false;	// 공간부족
		}

		if (nRemain > 0){
			nBlank = __FindOverlapQuestInventorySlot(pTempQuestInven, nItemID, nRemain);	// 짜투리가 들어갈 공간이 있는지
			if (nBlank < 0){	// 짜투리 공간은 없다
				if ((nBundle + 1) > nLocalCount) return false;	// 한칸의 여유가 더 있는지...
			}
		}

		return(__SetBlankQuestInventorySlotCount(pTempQuestInven, nItemID, wCount, pItem->nOverlapCount, nLocalCount));
	}

	return true;
}

void api_log_AddLog(CDNGameRoom *pRoom, const char* szLog)
{
	DN_ASSERT(NULL != pRoom,	"Invalid!");
	DNScriptAPI::api_log_AddLog(szLog);
}

void api_log_UserLog(CDNGameRoom *pRoom, UINT nUserObjectID, const char* szLog)
{
	DN_ASSERT(NULL != pRoom,	"Invalid!");

#ifndef _FINAL_BUILD
	CDNUserSession * pUser = pRoom->GetUserSession(nUserObjectID);
	if ( !pUser ) return;

	DNScriptAPI::api_log_UserLog(pUser, szLog);
#endif // _FINAL_BUILD
}

void api_quest_DumpQuest(CDNGameRoom *pRoom, UINT nUserObjectID)
{
	DN_ASSERT(NULL != pRoom,	"Invalid!");

#ifndef _FINAL_BUILD
	CDNUserSession * pUser = pRoom->GetUserSession(nUserObjectID);
	if ( !pUser ) return;
	
	DNScriptAPI::api_quest_DumpQuest(pUser);
#endif // _FINAL_BUILD
}

void api_npc_NextTalk(CDNGameRoom *pRoom, UINT nUserSessionID,  UINT nNpcObjectID , const char* szTalkIndex, const char* szTargetFile)
{
	DN_ASSERT(NULL != pRoom, "Invalid!");

	CDNUserSession* pUser = pRoom->GetUserSession(nUserSessionID);
	if ( !pUser ) return;

	DNScriptAPI::api_npc_NextTalk(pUser, nNpcObjectID, szTalkIndex, szTargetFile);
}

void api_npc_NextScript(CDNGameRoom *pRoom, UINT nUserSessionID, UINT nNpcObjectID, const char* szTalkIndex, const char* szTargetFile)
{
	DN_ASSERT(NULL != pRoom,	"Invalid!");

	CDNUserSession * pUser = pRoom->GetUserSession(nUserSessionID);
	if ( !pUser ) return;

	DnActorHandle hActor = CDnActor::FindActorFromUniqueID( pUser->GetGameRoom(), nNpcObjectID );
	if( !hActor || !hActor->IsNpcActor() )
		return;

	CDnNPCActor* pNpc = static_cast<CDnNPCActor*>(hActor.GetPointer());

	DNScriptAPI::api_npc_NextScript(pUser, pNpc, nNpcObjectID, szTalkIndex, szTargetFile);
}

int api_npc_SetParamString(CDNGameRoom *pRoom, UINT nUserObjectID, const char* szParamKey, int nParamID)
{
	DN_ASSERT(NULL != pRoom,	"Invalid!");

	CDNUserSession * pUser = pRoom->GetUserSession(nUserObjectID);
	if ( !pUser ) return -1;

	return DNScriptAPI::api_npc_SetParamString(pUser, szParamKey, nParamID);
}

int api_npc_SetParamInt(CDNGameRoom *pRoom, UINT nUserObjectID, const char* szParamKey, int nValue)
{
	DN_ASSERT(NULL != pRoom,	"Invalid!");

	CDNUserSession * pUser = pRoom->GetUserSession(nUserObjectID);
	if ( !pUser ) return -1;

	return DNScriptAPI::api_npc_SetParamInt(pUser, szParamKey, nValue);
}

int api_quest_AddHuntingQuest(CDNGameRoom *pRoom, UINT nUserObjectID, int nQuestID, int nQuestStep, int nJournalStep, int nCountingSlot, int nCountingType, int nCountingIndex, int nTargetCnt)
{
	DN_ASSERT(NULL != pRoom,	"Invalid!");

	CDNUserSession * pUser = pRoom->GetUserSession(nUserObjectID);
	if ( !pUser ) return -1;

	return DNScriptAPI::api_quest_AddHuntingQuest(pUser, nQuestID, nQuestStep, nJournalStep, nCountingSlot, nCountingType, nCountingIndex, nTargetCnt);
}

int api_quest_AddQuest(CDNGameRoom *pRoom, UINT nUserObjectID, int nQuestID, int nQuestType)
{
	DN_ASSERT(NULL != pRoom,	"Invalid!");

	CDNUserSession * pUser = pRoom->GetUserSession(nUserObjectID);
	if ( !pUser ) return -1;

	return DNScriptAPI::api_quest_AddQuest(pUser, nQuestID, nQuestType);
}

int api_quest_CompleteQuest(CDNGameRoom *pRoom, UINT nUserObjectID, int nQuestID, bool bDelPlayList)
{
	DN_ASSERT(NULL != pRoom,	"Invalid!");

	CDNUserSession * pUser = pRoom->GetUserSession(nUserObjectID);
	if ( !pUser ) return -1;

	return DNScriptAPI::api_quest_CompleteQuest(pUser, nQuestID, bDelPlayList);
}

int api_quest_MarkingCompleteQuest(CDNGameRoom *pRoom, UINT nUserObjectID, int nQuestID)
{
	// 보상퀘스트의 마킹을 api_quest_CompleteQuest으로 통합하기 위해 더이상 이 API는 사용하지 않는다.
	return 1;
}

int api_quest_IsMarkingCompleteQuest(CDNGameRoom *pRoom, UINT nUserObjectID, int nQuestID)
{
	DN_ASSERT(NULL != pRoom,	"Invalid!");

	CDNUserSession* pUser = pRoom->GetUserSession(nUserObjectID);
	if ( !pUser ) 
		return -1;

	return DNScriptAPI::api_quest_IsMarkingCompleteQuest(pUser, nQuestID);
}

int api_quest_UserHasQuest(CDNGameRoom *pRoom, UINT nUserObjectID, int nQuestID)
{
	DN_ASSERT(NULL != pRoom,	"Invalid!");

	CDNUserSession * pUser = pRoom->GetUserSession(nUserObjectID);
	if ( !pUser ) 
		return -1;

	return DNScriptAPI::api_quest_UserHasQuest(pUser, nQuestID);
}

int api_quest_GetPlayingQuestCnt(CDNGameRoom *pRoom, UINT nUserObjectID)
{
	DN_ASSERT(NULL != pRoom,	"Invalid!");

	CDNUserSession * pUser = pRoom->GetUserSession(nUserObjectID);
	if ( !pUser ) 
		return -1;

	return DNScriptAPI::api_quest_GetPlayingQuestCnt(pUser);
}

int api_npc_GetNpcIndex(CDNGameRoom *pRoom, UINT nUserObjectID, UINT nNpcObjID)
{
	DN_ASSERT(NULL != pRoom,	"Invalid!");

	CDNUserSession * pUser = pRoom->GetUserSession(nUserObjectID);

	if ( !pUser ) 
		return -1;

	DnActorHandle hActor = CDnActor::FindActorFromUniqueID( pUser->GetGameRoom(), nNpcObjID );
	if( !hActor || !hActor->IsNpcActor() )
		return -2;

	CDnNPCActor* pNpc = static_cast<CDnNPCActor*>(hActor.GetPointer());
	
	return DNScriptAPI::api_npc_GetNpcIndex(pNpc);
}

int api_quest_SetQuestStepAndJournalStep(CDNGameRoom *pRoom, UINT nUserObjectID, int nQuestID, short nQuestStep, int nJournalStep)
{
	CDNUserSession * pUser = pRoom->GetUserSession(nUserObjectID);
	if ( !pUser ) return -1;

	return DNScriptAPI::api_quest_SetQuestStepAndJournalStep(pUser, nQuestID, nQuestStep, nJournalStep);
}

int api_quest_SetQuestStep(CDNGameRoom *pRoom, UINT nUserObjectID, int nQuestID, short nQuestStep)
{
	DN_ASSERT(NULL != pRoom,	"Invalid!");

	CDNUserSession * pUser = pRoom->GetUserSession(nUserObjectID);
	if ( !pUser ) return -1;

	return DNScriptAPI::api_quest_SetQuestStep(pUser, nQuestID, nQuestStep);
}

int api_quest_GetQuestStep(CDNGameRoom *pRoom, UINT nUserObjectID, int nQuestID)
{
	DN_ASSERT(NULL != pRoom,	"Invalid!");

	CDNUserSession * pUser = pRoom->GetUserSession(nUserObjectID);
	if ( !pUser ) 
		return -1;

	return DNScriptAPI::api_quest_GetQuestStep(pUser, nQuestID);
}

int api_quest_SetJournalStep(CDNGameRoom *pRoom, UINT nUserObjectID, int nQuestID, int nJournalStep)
{
	DN_ASSERT(NULL != pRoom,	"Invalid!");

	CDNUserSession * pUser = pRoom->GetUserSession(nUserObjectID);
	if ( !pUser ) return -1;

	return DNScriptAPI::api_quest_SetJournalStep(pUser, nQuestID, nJournalStep);
}

int api_quest_GetJournalStep(CDNGameRoom *pRoom, UINT nUserObjectID, int nQuestID)
{
	DN_ASSERT(NULL != pRoom,	"Invalid!");

	CDNUserSession * pUser = pRoom->GetUserSession(nUserObjectID);
	if ( !pUser ) 
		return -1;

	return DNScriptAPI::api_quest_GetJournalStep(pUser, nQuestID);
}

int api_quest_SetQuestMemo(CDNGameRoom *pRoom, UINT nUserObjectID, int nQuestID, char nMemoIndex, int iVal)
{
	DN_ASSERT(NULL != pRoom,	"Invalid!");

	CDNUserSession * pUser = pRoom->GetUserSession(nUserObjectID);
	if ( !pUser ) return -1;

	return DNScriptAPI::api_quest_SetQuestMemo(pUser, nQuestID, nMemoIndex, iVal);
}

int api_quest_GetQuestMemo(CDNGameRoom *pRoom, UINT nUserObjectID, int nQuestID, char nMemoIndex)
{
	DN_ASSERT(NULL != pRoom,	"Invalid!");

	CDNUserSession * pUser = pRoom->GetUserSession(nUserObjectID);
	if ( !pUser ) {
		return -1;
	}

	return DNScriptAPI::api_quest_GetQuestMemo(pUser, nQuestID, nMemoIndex);
}

int api_quest_SetCountingInfo(CDNGameRoom *pRoom, UINT nUserObjectID, int nQuestID, int nSlot, int nCountingType, int nCountingIndex, int nTargetCnt)
{
	DN_ASSERT(NULL != pRoom,	"Invalid!");

	CDNUserSession * pUser = pRoom->GetUserSession(nUserObjectID);
	if ( !pUser ) return -1;
#if defined(_CH)
	if (pUser->GetFCMState() != FCMSTATE_NONE){
		pUser->SendQuestResult(ERROR_QUEST_FCM);
		return -1;
	}
#endif
	return DNScriptAPI::api_quest_SetCountingInfo(pUser, nQuestID, nSlot, nCountingType, nCountingIndex, nTargetCnt);
}

int api_quest_ClearCountingInfo(CDNGameRoom *pRoom, UINT nUserObjectID, int nQuestID)
{
	DN_ASSERT(NULL != pRoom,	"Invalid!");

	CDNUserSession * pUser = pRoom->GetUserSession(nUserObjectID);
	if ( !pUser ) 
		return -1;

	return DNScriptAPI::api_quest_ClearCountingInfo(pUser, nQuestID);
}

int api_quest_IsAllCompleteCounting(CDNGameRoom *pRoom, UINT nUserObjectID, int nQuestID)
{
	DN_ASSERT(NULL != pRoom,	"Invalid!");

	CDNUserSession * pUser = pRoom->GetUserSession(nUserObjectID);
	if ( !pUser ) 
		return -1;

	return DNScriptAPI::api_quest_IsAllCompleteCounting(pUser, nQuestID);
}

int api_user_CheckInvenForAddItem(CDNGameRoom *pRoom, UINT nUserObjectID, int nItemIndex, int nItemCnt)
{
	DN_ASSERT(NULL != pRoom,	"Invalid!");

	CDNUserSession* pUser = pRoom->GetUserSession(nUserObjectID);
	if (!pUser) {
		return -1;
	}

	return DNScriptAPI::api_user_CheckInvenForAddItem(pUser, nItemIndex, nItemCnt);
}

int api_user_CheckInvenForAddItemList(CDNGameRoom *pRoom, UINT nUserObjectID, lua_tinker::table ItemTable)
{
	DN_ASSERT(NULL != pRoom,	"Invalid!");

	CDNUserSession* pUser = pRoom->GetUserSession(nUserObjectID);
	if (!pUser) {
		return -1;
	}

	return DNScriptAPI::api_user_CheckInvenForAddItemList(pUser, ItemTable);
}

int api_user_AddItem(CDNGameRoom *pRoom, UINT nUserObjectID, int nItemIndex, int nItemCnt, int nQuestID)
{
	DN_ASSERT(NULL != pRoom,	"Invalid!");

	CDNUserSession * pUser = pRoom->GetUserSession(nUserObjectID);

	if ( !pUser ) 
		return -1;

#if defined(_CH)
	if (pUser->GetFCMState() != FCMSTATE_NONE){
		pUser->SendQuestResult(ERROR_QUEST_FCM);
		return -1;
	}
#endif

	CDNUserItem* pUserItem = pUser->GetItem();
	if ( !pUserItem )
		return -2;

	TItem ResultItem;
	if( CDNUserItem::MakeItemStruct(nItemIndex,ResultItem) == false )
		return -2;
	// 일단은 임시로 빈공간만 확인한다. 
	// 인벤에 빈공간이 없으면 
	if ( !pUserItem->IsValidSpaceInventorySlot(nItemIndex, nItemCnt, ResultItem.bSoulbound, ResultItem.cSealCount, ResultItem.bEternity) ) 
	{
		return -3;
	}

	bool bResult = pUserItem->AddInventoryByQuest(nItemIndex, nItemCnt, nQuestID, 0);	// 아이템에 따라서 randomseed 를 어떻게 넣을껀지 결정해주시길!

#if defined( PRE_ENABLE_QUESTCHATLOG )
	if ( bResult )
	{
		// 임시로 실제로 주진 않고 유저 채팅창에만 찍어준다.
		std::wstring wszMsg;
		wszMsg = FormatW( L"아이템 지급 : id %d count : %d" , nItemIndex, nItemCnt );
		pUser->SendChat(CHATTYPE_NORMAL, (int)wszMsg.size()*sizeof(WCHAR), L"", (WCHAR*)wszMsg.c_str());
	}
	else
	{
		// 임시로 실제로 주진 않고 유저 채팅창에만 찍어준다.
		std::wstring wszMsg;
		wszMsg = FormatW( L"아이템 지급 실패 : id %d count : %d" , nItemIndex, nItemCnt );
		pUser->SendChat(CHATTYPE_NORMAL, (int)wszMsg.size()*sizeof(WCHAR), L"", (WCHAR*)wszMsg.c_str());
	}
#endif // #if defined( PRE_ENABLE_QUESTCHATLOG )

	return 1;
}

int api_user_DelItem(CDNGameRoom *pRoom, UINT nUserObjectID, int nItemIndex, int nItemCnt, int nQuestID)
{
	DN_ASSERT(NULL != pRoom,	"Invalid!");

	CDNUserSession * pUser = pRoom->GetUserSession(nUserObjectID);
	if ( !pUser ) 
		return -1;

	return DNScriptAPI::api_user_DelItem(pUser, nItemIndex, nItemCnt, nQuestID);
}

int api_user_AllDelItem(CDNGameRoom *pRoom, UINT nUserObjectID, int nItemIndex)
{
	DN_ASSERT(NULL != pRoom,	"Invalid!");

	CDNUserSession * pUser = pRoom->GetUserSession(nUserObjectID);
	if ( !pUser ) 
		return -1;

	return DNScriptAPI::api_user_AllDelItem(pUser, nItemIndex);
}

int api_user_HasItem(CDNGameRoom *pRoom, UINT nUserObjectID, int nItemIndex, int nItemCnt)
{
	DN_ASSERT(NULL != pRoom,	"Invalid!");

	CDNUserSession * pUser = pRoom->GetUserSession(nUserObjectID);
	if ( !pUser ) 
		return -1;

	return DNScriptAPI::api_user_HasItem(pUser, nItemIndex, nItemCnt);
}

int api_user_GetUserClassID(CDNGameRoom *pRoom, UINT nUserObjectID)
{
	DN_ASSERT(NULL != pRoom,	"Invalid!");

	CDNUserSession * pUser = pRoom->GetUserSession(nUserObjectID);
	if ( !pUser ) return -1;

	return DNScriptAPI::api_user_GetUserClassID(pUser);
}

int api_user_GetUserJobID(CDNGameRoom *pRoom, UINT nUserObjectID)
{
	DN_ASSERT(NULL != pRoom,	"Invalid!");

	CDNUserSession * pUser = pRoom->GetUserSession(nUserObjectID);
	if ( !pUser ) return -1;

	return DNScriptAPI::api_user_GetUserJobID(pUser);
}

int api_user_GetUserLevel(CDNGameRoom *pRoom, UINT nUserObjectID)
{
	DN_ASSERT(NULL != pRoom,	"Invalid!");

	CDNUserSession * pUser = pRoom->GetUserSession(nUserObjectID);
	if ( !pUser ) return -1;

	return DNScriptAPI::api_user_GetUserLevel(pUser);
}

int api_user_GetUserInvenBlankCount(CDNGameRoom *pRoom, UINT nUserObjectID)
{
	DN_ASSERT(NULL != pRoom,	"Invalid!");

	CDNUserSession * pUser = pRoom->GetUserSession(nUserObjectID);
	if ( !pUser ) 
		return -1;

	return DNScriptAPI::api_user_GetUserInvenBlankCount(pUser);
}

int api_quest_GetUserQuestInvenBlankCount(CDNGameRoom *pRoom, UINT nUserObjectID)
{
	DN_ASSERT(NULL != pRoom,	"Invalid!");

	CDNUserSession * pUser = pRoom->GetUserSession(nUserObjectID);
	if ( !pUser ) 
		return -1;

	return DNScriptAPI::api_quest_GetUserQuestInvenBlankCount(pUser);
}

/*
desc   : 유저에게 경험치를 추가한다.
param  : 유저인덱스, 경험치
return : -1 => 유저를 찾을수 없음, 
*/
int api_user_AddExp(CDNGameRoom *pRoom, UINT nUserObjectID, int nQuestID, int nAddExp)
{
	DN_ASSERT(NULL != pRoom,	"Invalid!");

	CDNUserSession * pUser = pRoom->GetUserSession(nUserObjectID);

	if ( !pUser ) 
		return -1;

#if defined(_CH)
	if (pUser->GetFCMState() != FCMSTATE_NONE){
		pUser->SendQuestResult(ERROR_QUEST_FCM);
		return -1;
	}
#endif

	DnActorHandle hActor = pUser->GetActorHandle();
	if ( !hActor || !hActor->IsPlayerActor() )
		return -1;

	CDnPlayerActor* pPlayerActor = static_cast<CDnPlayerActor*>(hActor.GetPointer());
	TExpData ExpData;	
	ExpData.set( (float)nAddExp );
	pPlayerActor->CmdAddExperience( ExpData, DBDNWorldDef::CharacterExpChangeCode::Quest, nQuestID );

	int nExp = pPlayerActor->GetExperience();

#ifndef _FINAL_BUILD
	std::wstring wszMsg;
	wszMsg = FormatW( L"경험치추가 : %d 최종 : %d" , nAddExp, nExp);
	pUser->SendChat(CHATTYPE_NORMAL, (int)wszMsg.size()*sizeof(WCHAR), L"", (WCHAR*)wszMsg.c_str());
#endif

	return 1;
}

/*
desc   : 유저에게 돈을 추가한다.
param  : 유저인덱스, 돈
return : -1 => 유저를 찾을수 없음, 
*/
int api_user_AddCoin(CDNGameRoom *pRoom, UINT nUserObjectID, int nAddCoin, int nQuestID)
{
	DN_ASSERT(NULL != pRoom,	"Invalid!");

	CDNUserSession * pUser = pRoom->GetUserSession(nUserObjectID);

	if ( !pUser ) 
		return -1;

#if defined(_CH)
	if (pUser->GetFCMState() != FCMSTATE_NONE){
		pUser->SendQuestResult(ERROR_QUEST_FCM);
		return -1;
	}
#endif

	DnActorHandle hActor = pUser->GetActorHandle();
	if ( !hActor || !hActor->IsPlayerActor() ) 
		return -1;

	CDnPlayerActor* pPlayerActor = static_cast<CDnPlayerActor*>(hActor.GetPointer());

	pPlayerActor->CmdAddCoin(nAddCoin, DBDNWorldDef::CoinChangeCode::QuestReward, nQuestID);

#ifndef _FINAL_BUILD
	std::wstring wszMsg;
	wszMsg = FormatW( L"코인추가 : %d 최종 : %lld", nAddCoin, pUser->GetCoin());
	pUser->SendChat(CHATTYPE_NORMAL, (int)wszMsg.size()*sizeof(WCHAR), L"", (WCHAR*)wszMsg.c_str());
#endif

	return 1;
}

int api_user_PlayCutScene(CDNGameRoom *pRoom, UINT nUserObjectID, UINT nNpcObjectID, int nCutSceneTableID, bool bIgnoreFadeIn)
{
	DN_ASSERT(NULL != pRoom,	"Invalid!");

	CDNUserSession * pUser = pRoom->GetUserSession(nUserObjectID);

	if ( !pUser ) return -1;

	return DNScriptAPI::api_user_PlayCutScene(pUser, nNpcObjectID, nCutSceneTableID, bIgnoreFadeIn);
}

int api_ui_OpenWareHouse(CDNGameRoom *pRoom, UINT nUserObjectID, int iItemID/*=0*/ )
{
	DN_ASSERT(NULL != pRoom,	"Invalid!");

	CDNUserSession* pUser = pRoom->GetUserSession(nUserObjectID);
	if ( !pUser ) 
		return -1;

	return DNScriptAPI::api_ui_OpenWareHouse(pUser,iItemID);
}

/*
desc   : 유저에게 상점를 열라고 알린다.
param  : 유저인덱스, 상점 ID
return : -1 => 유저를 찾을수 없음, -2 면 샵이 없음. 1이면 성공
*/
int api_ui_OpenShop(CDNGameRoom *pRoom, UINT nUserObjectID, int nShopID, Shop::Type::eCode Type/*=Shop::Type::Normal*/ )
{
	DN_ASSERT(NULL != pRoom,	"Invalid!");

#if defined (PRE_MOD_GAMESERVERSHOP)
	CDNUserSession* pUser = pRoom->GetUserSession(nUserObjectID);
	if ( !pUser ) 
		return -1;

	return DNScriptAPI::api_ui_OpenShop(pUser, nShopID, Type);
#else
/*
	CDNUserSession* pUser = pRoom->GetUserSession(nUserObjectID);
	if ( !pUser ) 
		return -1;

	std::wstring wszLog;
	wszLog = FormatW( L"샵열기");
	pUser->SendChat(CHATTYPE_NORMAL, (int)wszLog.size()*sizeof(WCHAR), L"", (WCHAR*)wszLog.c_str());

	TShopData *pShopData = g_pDataManager->GetShopData(nShopID);
	if ( !pShopData )
		return -2;

	pUser->m_nShopID = nShopID;
	pUser->SendShopList(pShopData);

	pUser->SetCalledNpcResponse(true, false);	// 보통 클라이언트에 보내는 UI 개설 요청 이후 대화로 이어지지 않기 때문에 에러가 아닌데도 본의아니게 NPC 응답여부 체크에 걸리게 되므로 대화한 것으로 처리해 줌

	return 1;
*/

	return -2;
#endif
}



/*
desc   : 유저에게 상점를 열라고 알린다.
param  : 유저인덱스, 스킬 상점 ID
return : -1 => 유저를 찾을수 없음, -2 면 샵이 없음. 1이면 성공
*/
int api_ui_OpenSkillShop(CDNGameRoom *pRoom, UINT nUserObjectID, int nSkillShopID)
{
	DN_ASSERT(NULL != pRoom,	"Invalid!");
	CDNUserSession* pUser = pRoom->GetUserSession(nUserObjectID);
	if ( !pUser ) 
		return -1;

#if defined (PRE_MOD_GAMESERVERSHOP)
	return DNScriptAPI::api_ui_OpenSkillShop(pUser);
#else
/*
	CDNUserSession* pUser = pRoom->GetUserSession(nUserObjectID);
	if ( !pUser ) 
		return -1;

	std::wstring wszLog;
	wszLog = FormatW( L"스킬샵열기");
	pUser->SendChat(CHATTYPE_NORMAL, (int)wszLog.size()*sizeof(WCHAR), L"", (WCHAR*)wszLog.c_str());

	TSkillShopData *pShopData = g_pDataManager->GetSkillShopData(nSkillShopID);
	if ( !pShopData )
		return -2;


	pUser->m_nShopID = nSkillShopID;
	pUser->SendSkillShopList(pShopData);

	pUser->SetCalledNpcResponse(true, false);	// 보통 클라이언트에 보내는 UI 개설 요청 이후 대화로 이어지지 않기 때문에 에러가 아닌데도 본의아니게 NPC 응답여부 체크에 걸리게 되므로 대화한 것으로 처리해 줌

	return 1;
*/
	return -2;
#endif
}



/*
desc   : 유저에게 무인상점을 열라고 알린다.
param  : 유저인덱스
return : -1 => 유저를 찾을수 없음, -2 면 샵이 없음. 1이면 성공
*/
int api_ui_OpenMarket(CDNGameRoom *pRoom, UINT nUserObjectID)
{
	DN_ASSERT(NULL != pRoom,	"Invalid!");

/*
	CDNUserSession* pUser = pRoom->GetUserSession(nUserObjectID);
	if ( !pUser ) 
		return -1;
#ifndef _FINAL_BUILD
	std::wstring wszLog;
	wszLog = FormatW( L"무인상점 열기");
	pUser->SendChat(CHATTYPE_NORMAL, (int)wszLog.size()*sizeof(WCHAR), L"", (WCHAR*)wszLog.c_str());
#endif

	pUser->SetCalledNpcResponse(true, false);	// 보통 클라이언트에 보내는 UI 개설 요청 이후 대화로 이어지지 않기 때문에 에러가 아닌데도 본의아니게 NPC 응답여부 체크에 걸리게 되므로 대화한 것으로 처리해 줌

	return pUser->QueryDefaultMarketList();
*/

	return -2;
}

int api_ui_OpenCompoundEmblem(CDNGameRoom *pRoom, UINT nUserObjectID)
{
	DN_ASSERT(NULL != pRoom,	"Invalid!");

	CDNUserSession* pUser = pRoom->GetUserSession(nUserObjectID);
	if ( !pUser ) 
		return -1;

	return DNScriptAPI::api_ui_OpenCompoundEmblem(pUser);
}

int api_ui_OpenUpgradeJewel(CDNGameRoom *pRoom, UINT nUserObjectID)
{
	DN_ASSERT(NULL != pRoom,	"Invalid!");

	CDNUserSession* pUser = pRoom->GetUserSession(nUserObjectID);
	if ( !pUser ) 
		return -1;

	return DNScriptAPI::api_ui_OpenUpgradeJewel(pUser);
}

int api_ui_OpenMailBox(CDNGameRoom *pRoom, UINT nUserObjectID)
{
	DN_ASSERT(NULL != pRoom,	"Invalid!");

	CDNUserSession* pUser = pRoom->GetUserSession(nUserObjectID);
	if ( !pUser ) 
		return -1;

	return DNScriptAPI::api_ui_OpenMailBox(pUser);
}

/*
desc   : 유저에게 농장창고를 열라고 시킨다.
param  : 유저인덱스
return : -1 => 유저를 찾을수 없음, 1이면 성공
*/
int api_ui_OpenFarmWareHouse(CDNGameRoom *pRoom, UINT nUserObjectID)
{
	DN_ASSERT(NULL != pRoom,	"Invalid!");

	CDNUserSession* pUser = pRoom->GetUserSession(nUserObjectID);
	if ( !pUser ) 
		return -1;
	if( !pRoom->bIsFarmRoom() )
		return -1;

#ifndef _FINAL_BUILD
	std::wstring wszLog;
	wszLog = FormatW( L"농장창고 열기");
	pUser->SendChat(CHATTYPE_NORMAL, (int)wszLog.size()*sizeof(WCHAR), L"", (WCHAR*)wszLog.c_str());
#endif

	static_cast<CDNFarmUserSession*>(pUser)->ClearFarmWareHouseItem();
	pUser->SendOpenFarmWareHouse();

	pUser->SetCalledNpcResponse(true, false);	// 보통 클라이언트에 보내는 UI 개설 요청 이후 대화로 이어지지 않기 때문에 에러가 아닌데도 본의아니게 NPC 응답여부 체크에 걸리게 되므로 대화한 것으로 처리해 줌

	return 1;

}

int api_ui_OpenDisjointItem(CDNGameRoom *pRoom, UINT nUserObjectID)
{
	DN_ASSERT(NULL != pRoom,	"Invalid!");

	CDNUserSession* pUser = pRoom->GetUserSession(nUserObjectID);
	if ( !pUser ) 
		return -1;

	return DNScriptAPI::api_ui_OpenDisjointItem(pUser);
}

int api_ui_OpenCompoundItem(CDNGameRoom *pRoom, UINT nUserObjectID, int nCompoundShopID)
{
	DN_ASSERT(NULL != pRoom,	"Invalid!");

	CDNUserSession* pUser = pRoom->GetUserSession(nUserObjectID);
	if ( !pUser ) 
		return -1;

	return DNScriptAPI::api_ui_OpenCompoundItem(pUser, nCompoundShopID);
}

int api_ui_OpenCompound2Item( CDNGameRoom *pRoom, UINT nUserObjectID, int nCompoundGroupID, int iItemID/*=0*/ )
{

	DN_ASSERT(NULL != pRoom,	"Invalid!");

	CDNUserSession* pUser = pRoom->GetUserSession(nUserObjectID);
	if ( !pUser ) 
		return -1;

#ifndef _FINAL_BUILD
	std::wstring wszLog;
	wszLog = FormatW( L"아이템 합성창 열기");
	pUser->SendChat(CHATTYPE_NORMAL, (int)wszLog.size()*sizeof(WCHAR), L"", (WCHAR*)wszLog.c_str());
#endif

	pUser->SendOpenCompound2Item(nCompoundGroupID, iItemID );

	pUser->SetCalledNpcResponse(true, false);	// 보통 클라이언트에 보내는 UI 개설 요청 이후 대화로 이어지지 않기 때문에 에러가 아닌데도 본의아니게 NPC 응답여부 체크에 걸리게 되므로 대화한 것으로 처리해 줌

	pUser->SetRemoteItemCompoundItemID( iItemID );

	return 1;
}

int api_ui_OpenCashShop(CDNGameRoom *pRoom, UINT nUserObjectID)
{
	DN_ASSERT(NULL != pRoom,	"Invalid!");

	CDNUserSession* pUser = pRoom->GetUserSession(nUserObjectID);
	if ( !pUser ) 
		return -1;

	return DNScriptAPI::api_ui_OpenCashShop(pUser);
}

int api_ui_OpenGuildMgrBox(CDNGameRoom *pRoom, UINT nUserObjectID, int nGuildMgrNo)
{
	DN_ASSERT(NULL != pRoom,	"Invalid!");

	CDNUserSession* pUser = pRoom->GetUserSession(nUserObjectID);
	if ( !pUser ) 
		return -1;

	return DNScriptAPI::api_ui_OpenGuildMgrBox(pUser, nGuildMgrNo);
}

int api_ui_OpenGacha_JP(CDNGameRoom *pRoom,  UINT nUserObjectID, int nGachaShopID)
{
#ifdef PRE_ADD_GACHA_JAPAN
	CDNUserSession* pUser = pRoom->GetUserSession(nUserObjectID);
	if ( !pUser ) 
		return -1;

	return DNScriptAPI::api_ui_OpenGacha_JP(pUser, nGachaShopID);
#else	// PRE_ADD_GACHA_JAPAN
	return -3;
#endif	// PRE_ADD_GACHA_JAPAN
}

int api_ui_OpenGiveNpcPresent( CDNGameRoom* pRoom, UINT nUserObjectID, int nNpcID )
{
#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
	CDNUserSession* pUser = pRoom->GetUserSession( nUserObjectID );
	if( !pUser )
		return -1;

	return DNScriptAPI::api_ui_OpenGiveNpcPresent(pUser, nNpcID);
#else
	return -1;
#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
}

int api_user_UserMessage(CDNGameRoom *pRoom, UINT nUserObjectID, int nType, int nBaseStringIdx, lua_tinker::table ParamTable)
{
	DN_ASSERT(NULL != pRoom,	"Invalid!");

	CDNUserSession* pUser = pRoom->GetUserSession(nUserObjectID);
	if ( !pUser ) return -1;

	return DNScriptAPI::api_user_UserMessage(pUser, nType, nBaseStringIdx, ParamTable);
}

/*
desc   : 프랍을 클릭한 유저에대 한 메세지를 출력한다
param  : 유저 핸들,  스트링ID
return : -1 => 유저를 찾을수 없음, -2 타입이 없음 -3 메세지가 너무 김 -4 룸이 없음
*/
int api_trigger_LastAimPropActorMessage( CDNGameRoom *pRoom, int nType, int nActorHandle, int nBaseStringIdx, lua_tinker::table ParamTable )
{
	if( !pRoom ) return -4;

	DnActorHandle hActor = CDnActor::FindActorFromUniqueID( pRoom, nActorHandle );
	if( !hActor ) return -1;

	std::vector<DNReplaceString::DynamicReplaceStringInfo> ReplaceParamList;

#if defined(_SERVER) &&defined(PRE_ADD_MULTILANGUAGE)
	MultiLanguage::SupportLanguage::eSupportLanguage eLangCode;
	
	CDNUserSession * pUser;
	for( DWORD itr = 0; itr < pRoom->GetUserCount(); ++itr )
	{
		eLangCode = MultiLanguage::eDefaultLanguage;

		pUser = pRoom->GetUserData(itr);
		if(pUser == NULL) continue;

		eLangCode = pUser->m_eSelectedLanguage;

		WCHAR wszBaseString[512];
		swprintf_s( wszBaseString, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nBaseStringIdx, eLangCode ), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, hActor->GetNameUIStringIndex(), eLangCode ) );

		std::wstring wszString = std::wstring(wszBaseString);

		// 먼저 파라미터 테이블에 값이 있는지 확인 하고
		lua_tinker::table t = ParamTable.get<lua_tinker::table>(1);
		const char* __szKey = t.get<const char*>(1);

		// 있으면 
		if ( __szKey )
		{
			for ( int i = 1 ; i <= ParamTable.getSize() ; i++ )
			{
				lua_tinker::table pTable = ParamTable.get<lua_tinker::table>(i);
				char* szKey = pTable.get<char*>(1);
				int nValueType = pTable.get<int>(2);
				int nValue = pTable.get<int>(3);

				DNReplaceString::DynamicReplaceStringInfo param;
				ToWideString(szKey, param.szKey);
				param.cValueType = (char)nValueType;
				param.nValue = nValue;
				ReplaceParamList.push_back(param);
			}
		}

		g_ReplaceString.Relpace( wszString, ReplaceParamList, eLangCode );

		if( wszString.size() > 256 )
			return -3;

		switch(nType)
		{
		case CHATTYPE_NORMAL:
		case CHATTYPE_PARTY:
		case CHATTYPE_PRIVATE:
		case CHATTYPE_GUILD:
		case CHATTYPE_CHANNEL:
		case CHATTYPE_SYSTEM:
			break;
		default:
			return -2;
		}
		
		pUser->SendChat( (eChatType)nType, (int)wszString.size()*sizeof(WCHAR), L"", wszString.c_str() );
	}
#else		//#if defined(_SERVER) &&defined(PRE_ADD_MULTILANGUAGE)
	WCHAR wszBaseString[512];
	swprintf_s( wszBaseString, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nBaseStringIdx ), hActor->GetName() );

	std::wstring wszString = std::wstring(wszBaseString);

	// 먼저 파라미터 테이블에 값이 있는지 확인 하고
	lua_tinker::table t = ParamTable.get<lua_tinker::table>(1);
	const char* __szKey = t.get<const char*>(1);

	// 있으면 
	if ( __szKey )
	{
		for ( int i = 1 ; i <= ParamTable.getSize() ; i++ )
		{
			lua_tinker::table pTable = ParamTable.get<lua_tinker::table>(i);
			char* szKey = pTable.get<char*>(1);
			int nValueType = pTable.get<int>(2);
			int nValue = pTable.get<int>(3);

			DNReplaceString::DynamicReplaceStringInfo param;
			ToWideString(szKey, param.szKey);
			param.cValueType = (char)nValueType;
			param.nValue = nValue;
			ReplaceParamList.push_back(param);
		}
	}

	g_ReplaceString.Relpace( wszString, ReplaceParamList );

	if( wszString.size() > 256 )
		return -3;

	switch(nType)
	{
	case CHATTYPE_NORMAL:
	case CHATTYPE_PARTY:
	case CHATTYPE_PRIVATE:
	case CHATTYPE_GUILD:
	case CHATTYPE_CHANNEL:
	case CHATTYPE_SYSTEM:
		break;
	default:
		return -2;
	}

	CDNUserSession * pUser;
	for( DWORD itr = 0; itr < pRoom->GetUserCount(); ++itr )
	{
		pUser = pRoom->GetUserData(itr);
		if( pUser )
			pUser->SendChat( (eChatType)nType, (int)wszString.size()*sizeof(WCHAR), L"", wszString.c_str() );
	}
#endif		//#if defined(_SERVER) &&defined(PRE_ADD_MULTILANGUAGE)		

	return 1;
}

int api_quest_AddSymbolItem(CDNGameRoom *pRoom, UINT nUserObjectID, int nItemID, short wCount)
{
	DN_ASSERT(NULL != pRoom,	"Invalid!");

	CDNUserSession* pUser = pRoom->GetUserSession(nUserObjectID);
	if ( !pUser ) 
		return -1;

	return DNScriptAPI::api_quest_AddSymbolItem(pUser, nItemID, wCount);
}

int api_quest_DelSymbolItem(CDNGameRoom *pRoom, UINT nUserObjectID, int nItemID, short wCount)
{
	DN_ASSERT(NULL != pRoom,	"Invalid!");

	CDNUserSession* pUser = pRoom->GetUserSession(nUserObjectID);
	if ( !pUser ) 
		return -1;

	return DNScriptAPI::api_quest_DelSymbolItem(pUser, nItemID, wCount);
}

int api_quest_HasSymbolItem(CDNGameRoom *pRoom, UINT nUserObjectID, int nItemIndex, int nItemCnt)
{
	DN_ASSERT(NULL != pRoom,	"Invalid!");

	CDNUserSession* pUser = pRoom->GetUserSession(nUserObjectID);
	if ( !pUser ) 
		return -1;

	return DNScriptAPI::api_quest_HasSymbolItem(pUser, nItemIndex, nItemCnt);
}

int api_quest_CheckQuestInvenForAddItem(CDNGameRoom *pRoom, UINT nUserObjectID, int nItemIndex, int nItemCnt)
{
	DN_ASSERT(NULL != pRoom,	"Invalid!");

	CDNUserSession* pUser = pRoom->GetUserSession(nUserObjectID);
	if (!pUser) {
		return -1;
	}

	return DNScriptAPI::api_quest_CheckQuestInvenForAddItem(pUser, nItemIndex, nItemCnt);
}

int api_quest_CheckQuestInvenForAddItemList(CDNGameRoom *pRoom, UINT nUserObjectID, lua_tinker::table ItemTable)
{
	DN_ASSERT(NULL != pRoom,	"Invalid!");

	CDNUserSession* pUser = pRoom->GetUserSession(nUserObjectID);
	if (!pUser) {
		return -1;
	}

	return DNScriptAPI::api_quest_CheckQuestInvenForAddItemList(pUser, ItemTable);
}

int api_quest_AddQuestItem(CDNGameRoom *pRoom, UINT nUserObjectID, int nItemID, short wCount, int nQuestID)
{
	DN_ASSERT(NULL != pRoom,	"Invalid!");

	CDNUserSession* pUser = pRoom->GetUserSession(nUserObjectID);
	if ( !pUser ) 
		return -1;

	return DNScriptAPI::api_quest_AddQuestItem(pUser, nItemID, wCount, nQuestID);
}

int api_quest_DelQuestItem(CDNGameRoom *pRoom, UINT nUserObjectID, int nItemID, short wCount, int nQuestID)
{
	DN_ASSERT(NULL != pRoom,	"Invalid!");

	CDNUserSession* pUser = pRoom->GetUserSession(nUserObjectID);
	if ( !pUser ) 
		return -1;

	return DNScriptAPI::api_quest_DelQuestItem(pUser, nItemID, wCount, nQuestID);
}

int api_quest_AllDelQuestItem(CDNGameRoom *pRoom, UINT nUserObjectID, int nItemID )
{
	DN_ASSERT(NULL != pRoom,	"Invalid!");

	CDNUserSession* pUser = pRoom->GetUserSession(nUserObjectID);
	if ( !pUser ) 
		return -1;

	return DNScriptAPI::api_quest_AllDelQuestItem(pUser, nItemID);
}

int api_quest_HasQuestItem(CDNGameRoom *pRoom, UINT nUserObjectID, int nItemIndex, int nItemCnt)
{
	DN_ASSERT(NULL != pRoom,	"Invalid!");

	CDNUserSession* pUser = pRoom->GetUserSession(nUserObjectID);
	if ( !pUser ) 
		return -1;

	return DNScriptAPI::api_quest_HasQuestItem(pUser, nItemIndex, nItemCnt);
}

/*
desc   : 유저에게 길드 생성창을 띄우라고 알린다.
param  : 유저인덱스
return : -1 => 유저를 찾을수 없음, 1이면 성공
*/
int api_ui_OpenGuildCreate(CDNGameRoom *pRoom, UINT nUserObjectID)
{
	DN_ASSERT(NULL != pRoom,	"Invalid!");

	CDNUserSession* pUser = pRoom->GetUserSession(nUserObjectID);
	if ( !pUser ) 
		return -1;

	// N/A

	return 1;
}

/*
desc   : 유저에게 퀘스트 보상창을 띄우라고 알린다.
param  : 유저인덱스, 보상테이블  bActivate 가 true 면 고를수 있고 이면 그냥 보는 용도는 false로 쓴다.
return : -1 => 유저를 찾을수 없음, 1이면 성공
*/
int api_ui_OpenQuestReward(CDNGameRoom *pRoom, UINT nUserObjectID, int nRewardTableIndex, bool bActivate)
{
	DN_ASSERT(NULL != pRoom,	"Invalid!");

	CDNUserSession* pUser = pRoom->GetUserSession(nUserObjectID);
	if ( !pUser ) 
		return -1;

	pUser->SendOpenQuestReward(nRewardTableIndex, bActivate);

	pUser->SetCalledNpcResponse(true, false);	// 보통 클라이언트에 보내는 UI 개설 요청 이후 대화로 이어지지 않기 때문에 에러가 아닌데도 본의아니게 NPC 응답여부 체크에 걸리게 되므로 대화한 것으로 처리해 줌

	return 1;
}

int api_quest_RewardQuestUser(CDNGameRoom *pRoom, UINT nUserObjectID, int nRewardTableIndex, int nQuestID, int nRewardCheck)
{
	DN_ASSERT(NULL != pRoom,	"Invalid!");

	CDNUserSession* pUser = pRoom->GetUserSession(nUserObjectID);
	if (!pUser) {
		return -1;
	}

	return DNScriptAPI::api_quest_RewardQuestUser(pUser, nRewardTableIndex, nQuestID, nRewardCheck);
}

void api_user_ChangeMap(CDNGameRoom *pRoom, UINT nUserObjectID, int nMapIndex, int nGateNo)
{
	DN_ASSERT(NULL != pRoom,	"Invalid!");
	if( !pRoom )
		return;

	CDNUserSession *pUser = pRoom->GetUserSession(nUserObjectID);
	if( !pUser ) return;
#if defined(_CH)
	if (pUser->GetFCMState() != FCMSTATE_NONE){
		pUser->SendQuestResult(ERROR_QUEST_FCM);
		return;
	}
#endif

	int nPermitCnt = g_pDataManager->GetMapPermitPartyCount(nMapIndex);
	if (nPermitCnt > 0 && nPermitCnt < (int)pRoom->GetUserCountWithoutGM())
		return;

	CDnGameTask *pTask = (CDnGameTask *)pUser->GetGameRoom()->GetTaskMng()->GetTask( "GameTask" );
	if( !pTask ) return;

	pRoom->RequestChangeMapFromTrigger( nMapIndex, nGateNo );
}

/*
desc   : 해당 유저의 파티원수를 리턴한다.
param  : 유저인덱스
return : -1 => 유저를 찾을수 없음, 그외는 파티원수 (주의 !!!> 파티에 가입되지 않은 사용자도 1 을 반환)
*/
int api_user_GetPartymemberCount(CDNGameRoom *pRoom, UINT nUserObjectID)
{
	DN_ASSERT(NULL != pRoom,	"Invalid!");

	CDNUserSession * pUser = pRoom->GetUserSession(nUserObjectID);

	if ( !pUser ) 
		return -1;

	CDNGameRoom* pGameRoom = pUser->GetGameRoom();
	if ( !pGameRoom )
		return -1;

	return pGameRoom->GetUserCount();
}

/*
desc   : 해당 유저가 파티에 가입된 상태인지 체크한다.
param  : 유저인덱스
return : -1 => 유저를 찾을수 없음, 0 => 파티에 가입 않되어있음, 1 => 파티에 가입 되어있음
*/
int api_user_IsPartymember(CDNGameRoom *pRoom, UINT nUserObjectID)
{
	DN_ASSERT(NULL != pRoom,	"Invalid!");

	CDNUserSession * pUser = pRoom->GetUserSession(nUserObjectID);

	if ( !pUser ) 
		return -1;

	CDNGameRoom* pGameRoom = pUser->GetGameRoom();
	if ( !pGameRoom )
		return -1;

	if (0 != pGameRoom->GetPartyIndex()) {
		return 1;
	}

	// 파티에 가입하지 않은 상태이면 0 을 반환
	return 0;
}

/*
desc   : 해당 유저의 스테이지 난이도를 구함
param  : 유저인덱스
return : 0 ~ 4 => 던전 난이도 (0:쉬움 / 1:보통 / 2:어려움 / 3:마스터 : 4:어비스), 그 외는 실패 (기본 -1)
*/
int api_user_GetStageConstructionLevel(CDNGameRoom *pRoom, UINT nUserObjectID)
{
	DN_ASSERT(NULL != pRoom,	"Invalid!");

	CDNUserSession * pUser = pRoom->GetUserSession(nUserObjectID);

	if (!pUser) {
		return -1;
	}

	CDNGameRoom* pGameRoom = pUser->GetGameRoom();
	if ( !pGameRoom ) {
		return -1;
	}

	return pGameRoom->GetGameTask()->GetStageDifficulty();
}

int api_user_GetMapIndex(CDNGameRoom *pRoom, UINT nUserObjectID)
{
	DN_ASSERT(NULL != pRoom,	"Invalid!");

	CDNUserSession * pUser = pRoom->GetUserSession(nUserObjectID);
	if (!pUser) {
		return -1;
	}

	return DNScriptAPI::api_user_GetMapIndex(pUser);
}

int api_user_GetLastStageClearRank(CDNGameRoom *pRoom, UINT nUserObjectID)
{
	DN_ASSERT(NULL != pRoom,	"Invalid!");

	CDNUserSession * pUser = pRoom->GetUserSession(nUserObjectID);
	if (!pUser) {
		return -1;
	}

	return DNScriptAPI::api_user_GetLastStageClearRank(pUser);
}

int api_user_EnoughCoin(CDNGameRoom *pRoom, UINT nUserObjectID, int nCoin )
{
	DN_ASSERT(NULL != pRoom,	"Invalid!");

	CDNUserSession * pUser = pRoom->GetUserSession(nUserObjectID);

	if (!pUser) {
		return -1;
	}

	DnActorHandle hActor = pUser->GetActorHandle();
	if (!hActor || !hActor->IsPlayerActor() ) 
		return -1;

	return DNScriptAPI::api_user_EnoughCoin(pUser, nCoin);
}

int api_user_GetCoin(CDNGameRoom *pRoom, UINT nUserObjectID)
{
	DN_ASSERT(NULL != pRoom,	"Invalid!");

	CDNUserSession * pUser = pRoom->GetUserSession(nUserObjectID);

	if (!pUser) {
		return -1;
	}

	DnActorHandle hActor = pUser->GetActorHandle();
	if (!hActor || !hActor->IsPlayerActor() ) 
		return -1;

	return DNScriptAPI::api_user_GetCoin(pUser);
}

/*
desc   : 유저에게 돈을 감소한다.
param  : 유저인덱스, 돈 (0 초과)
return : -1 => 유저를 찾을수 없음, 1 => 성공
*/
int api_user_DelCoin(CDNGameRoom *pRoom, UINT nUserObjectID, int nDelCoin)
{
	DN_ASSERT(NULL != pRoom,	"Invalid!");
	DN_ASSERT(0 < nDelCoin,		"Invalid!");

	CDNUserSession * pUser = pRoom->GetUserSession(nUserObjectID);
	if (!pUser) return -1;

#if defined(_CH)
	if (pUser->GetFCMState() != FCMSTATE_NONE){
		pUser->SendQuestResult(ERROR_QUEST_FCM);
		return -1;
	}
#endif

	DnActorHandle hActor = pUser->GetActorHandle();
	if ( !hActor || !hActor->IsPlayerActor() ) 
		return -1;

	CDnPlayerActor* pPlayerActor = static_cast<CDnPlayerActor*>(hActor.GetPointer());

	pPlayerActor->CmdAddCoin((-nDelCoin), DBDNWorldDef::CoinChangeCode::Use, 0);

#ifndef _FINAL_BUILD
	std::wstring wszMsg;
	wszMsg = FormatW( L"코인감소 : %d 최종 : %lld" ,nDelCoin , pUser->GetCoin());
	pUser->SendChat(CHATTYPE_NORMAL, (int)wszMsg.size()*sizeof(WCHAR), L"", (WCHAR*)wszMsg.c_str());
#endif

	return 1;
}

int api_user_IsMissionGained(CDNGameRoom *pRoom, UINT nUserObjectID, int nMissionIndex)
{
	DN_ASSERT(NULL != pRoom,							"Invalid!");
	DN_ASSERT(CHECK_LIMIT(nMissionIndex, MISSIONMAX),	"Invalid!");

	CDNUserSession * pUser = pRoom->GetUserSession(nUserObjectID);
	if (!pUser) return -1;

	return DNScriptAPI::api_user_IsMissionGained(pUser, nMissionIndex);
}

int api_user_IsMissionAchieved(CDNGameRoom *pRoom, UINT nUserObjectID, int nMissionIndex)
{
	DN_ASSERT(NULL != pRoom,							"Invalid!");

	CDNUserSession * pUser = pRoom->GetUserSession(nUserObjectID);
	if (!pUser) return -1;

	return DNScriptAPI::api_user_IsMissionAchieved(pUser, nMissionIndex);
}

int api_user_HasItemWarehouse(CDNGameRoom *pRoom, UINT nUserObjectID, int nItemIndex, int nItemCnt)
{
	DN_ASSERT(NULL != pRoom,	"Invalid!");

	CDNUserSession * pUser = pRoom->GetUserSession(nUserObjectID);
	if (!pUser) {
		return -1;
	}

	return DNScriptAPI::api_user_HasItemWarehouse(pUser, nItemIndex, nItemCnt);
}

int api_user_HasItemEquip(CDNGameRoom *pRoom, UINT nUserObjectID, int nItemIndex)
{
	DN_ASSERT(NULL != pRoom,	"Invalid!");

	CDNUserSession * pUser = pRoom->GetUserSession(nUserObjectID);
	if (!pUser) {
		return -1;
	}

	return DNScriptAPI::api_user_HasItemEquip(pUser, nItemIndex);
}

int api_guild_GetGuildMemberRole(CDNGameRoom *pRoom, UINT nUserObjectID)
{
	DN_ASSERT(NULL != pRoom,	"Invalid!");

	CDNUserSession * pUser = pRoom->GetUserSession(nUserObjectID);
	if (!pUser) return -1;

	return DNScriptAPI::api_guild_GetGuildMemberRole(pUser);
}

int api_quest_IncQuestMemo(CDNGameRoom *pRoom, UINT nUserObjectID, int nQuestID, char nMemoIndex)
{
	CDNUserSession * pUser = pRoom->GetUserSession(nUserObjectID);
	if ( !pUser ) {
		return LONG_MIN;
	}

	return DNScriptAPI::api_quest_IncQuestMemo(pUser, nQuestID, nMemoIndex);
}

int api_quest_DecQuestMemo(CDNGameRoom *pRoom, UINT nUserObjectID, int nQuestID, char nMemoIndex)
{
	DN_ASSERT(NULL != pRoom,	"Invalid!");

	CDNUserSession * pUser = pRoom->GetUserSession(nUserObjectID);
	if ( !pUser ) {
		return LONG_MAX;
	}

	return DNScriptAPI::api_quest_DecQuestMemo(pUser, nQuestID, nMemoIndex);
}

int api_user_SetUserJobID(CDNGameRoom *pRoom, UINT nUserObjectID, int nJobID)
{
	DN_ASSERT(NULL != pRoom,	"Invalid!");

	CDNUserSession * pUser = pRoom->GetUserSession(nUserObjectID);
	if ( !pUser ) {
		return -1;
	}

	return DNScriptAPI::api_user_SetUserJobID(pUser, nJobID);
}

int api_user_IsJobInSameLine(CDNGameRoom *pRoom, UINT nUserObjectID, int nBaseJobID)
{
	DN_ASSERT(NULL != pRoom,	"Invalid!");

	CDNUserSession * pUser = pRoom->GetUserSession(nUserObjectID);
	if ( !pUser ) {
		return -1;
	}

	return DNScriptAPI::api_user_IsJobInSameLine(pUser, nBaseJobID);
}

int api_user_HasCashItem(CDNGameRoom *pRoom, UINT nUserObjectID, int nItemIndex, int nItemCnt)
{
	DN_ASSERT(NULL != pRoom,	"Invalid!");

	CDNUserSession * pUser = pRoom->GetUserSession(nUserObjectID);
	if ( !pUser ) {
		return -1;
	}

	return DNScriptAPI::api_user_HasCashItem(pUser, nItemIndex, nItemCnt);
}

int api_user_HasCashItemEquip(CDNGameRoom *pRoom, UINT nUserObjectID, int nItemIndex)
{
	DN_ASSERT(NULL != pRoom,	"Invalid!");

	CDNUserSession * pUser = pRoom->GetUserSession(nUserObjectID);
	if ( !pUser ) {
		return -1;
	}

	return DNScriptAPI::api_user_HasCashItemEquip(pUser, nItemIndex);
}

int api_quest_IncCounting(CDNGameRoom *pRoom, UINT nUserObjectID, int nCountingType, int nCountingIndex)
{
	DN_ASSERT(NULL != pRoom,	"Invalid!");

	CDNUserSession * pUser = pRoom->GetUserSession(nUserObjectID);
	if (!pUser) {
		return -1;
	}

	return DNScriptAPI::api_quest_IncCounting(pUser, nCountingType, nCountingIndex);
}

int api_quest_IsPlayingQuestMaximum(CDNGameRoom *pRoom, UINT nUserObjectID)
{
	DN_ASSERT(NULL != pRoom,	"Invalid!");

	CDNUserSession * pUser = pRoom->GetUserSession(nUserObjectID);
	if (!pUser) {
		return -1;
	}

	return DNScriptAPI::api_quest_IsPlayingQuestMaximum(pUser);
}

int api_quest_ForceCompleteQuest(CDNGameRoom *pRoom, UINT nUserObjectID, int nQuestID, int nQuestCode, int bDoMark, int bDoDelete, int bDoRepeat)
{
	CDNUserSession * pUser = pRoom->GetUserSession(nUserObjectID);
	if (!pUser) {
		return -1;
	}

	return DNScriptAPI::api_quest_ForceCompleteQuest(pUser, nQuestID, nQuestCode, bDoMark, bDoDelete, bDoRepeat);
}

/*
desc   : 특정 사용자의 위치를 이동 시킨다.
param  : 유저인덱스, X 좌표, Y 좌표, Z 좌표
return : -1 : 유저를 찾을수 없음, 1 성공
*/
int api_user_ChangePos(CDNGameRoom *pRoom, UINT nUserObjectID, int nX, int nY, int nZ, int nLookX, int nLookY)
{
	CDNUserSession * pUser = pRoom->GetUserSession(nUserObjectID);
	if (!pUser) {
		return -1;
	}
#if defined(_CH)
	if (pUser->GetFCMState() != FCMSTATE_NONE){
		pUser->SendQuestResult(ERROR_QUEST_FCM);
		return -1;
	}
#endif

#ifndef _FINAL_BUILD
	wstring wszString = FormatW(L"마을에서만 사용 가능한 기능입니다.\r\n");
	pUser->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
#endif	// _FINAL_BUILD

	return 1;
}


int api_ui_OpenUpgradeItem(CDNGameRoom *pRoom, UINT nUserObjectID, int iItemID/*=0*/ )
{
	CDNUserSession * pUser = pRoom->GetUserSession(nUserObjectID);
	if (!pUser) {
		return -1;
	}

	pUser->SendOpenUpgradeItem( iItemID );
	pUser->SetCalledNpcResponse(true, false);	// 보통 클라이언트에 보내는 UI 개설 요청 이후 대화로 이어지지 않기 때문에 에러가 아닌데도 본의아니게 NPC 응답여부 체크에 걸리게 되므로 대화한 것으로 처리해 줌
	pUser->SetRemoteEnchantItemID( iItemID );

	return 1;
}

int api_user_RequestEnterPVP(CDNGameRoom *pRoom, UINT nUserObjectID)
{
	CDNUserSession * pUser = pRoom->GetUserSession(nUserObjectID);
	if (!pUser) {
		return -1;
	}

#ifndef _FINAL_BUILD
	wstring wszString = FormatW(L"마을에서만 사용 가능한 기능입니다.\r\n");
	pUser->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
#endif	// _FINAL_BUILD

	return 1;
}

int api_npc_GetFavorPoint( CDNGameRoom *pRoom, UINT uiUserObjectID, int iNpcID )
{
#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
	CDNUserSession* pUser = pRoom->GetUserSession(uiUserObjectID);
	if (!pUser)
		return -1;

	return DNScriptAPI::api_npc_GetFavorPoint(pUser, iNpcID);
#else
	return -1;
#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
}

int api_npc_GetMalicePoint( CDNGameRoom *pRoom, UINT uiUserObjectID, int iNpcID )
{
#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
	CDNUserSession* pUser = pRoom->GetUserSession(uiUserObjectID);
	if( !pUser )
		return -1;

	return DNScriptAPI::api_npc_GetMalicePoint(pUser, iNpcID);
#else
	return -1;
#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
}

int api_npc_GetFavorPercent( CDNGameRoom *pRoom, UINT uiUserObjectID, int iNpcID )
{
#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
	CDNUserSession* pUser = pRoom->GetUserSession(uiUserObjectID);
	if( !pUser )
		return -1;

	return DNScriptAPI::api_npc_GetFavorPercent(pUser, iNpcID);
#else
	return -1;
#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
}

int api_npc_GetMalicePercent( CDNGameRoom *pRoom, UINT uiUserObjectID, int iNpcID )
{
#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
	CDNUserSession* pUser = pRoom->GetUserSession(uiUserObjectID);
	if( !pUser )
		return -1;

	return DNScriptAPI::api_npc_GetMalicePercent(pUser, iNpcID);
#else
	return -1;
#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
}

int api_npc_AddFavorPoint( CDNGameRoom *pRoom, UINT uiUserObjectID, int iNpcID, int val )
{
#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
	CDNUserSession* pUser = pRoom->GetUserSession(uiUserObjectID);
	if( !pUser )
		return -1;

	return DNScriptAPI::api_npc_AddFavorPoint(pUser, iNpcID, val);
#else
	return -1;
#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
}

int api_npc_AddMalicePoint( CDNGameRoom *pRoom, UINT uiUserObjectID, int iNpcID, int val )
{
#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
	CDNUserSession* pUser = pRoom->GetUserSession(uiUserObjectID);
	if( !pUser )
		return -1;

	return DNScriptAPI::api_npc_AddMalicePoint(pUser, iNpcID, val);
#else
	return -1;
#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
}

int api_npc_SendSelectedPresent( CDNGameRoom* pRoom, UINT uiUserObjectID, int iNpcID )
{
#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
	CDNUserSession* pUser = pRoom->GetUserSession(uiUserObjectID);
	if( !pUser )
		return -1;

	return DNScriptAPI::api_npc_SendSelectedPresent(pUser, iNpcID);
#else
	return -1;
#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
}

int api_npc_Rage( CDNGameRoom* pRoom, UINT uiUserObjectID, int iNpcID )
{
#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
	CDNUserSession* pUser = pRoom->GetUserSession(uiUserObjectID);
	if( !pUser )
		return -1;

	return DNScriptAPI::api_npc_Rage(pUser, iNpcID);
#else
	return -1;
#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
}

int api_npc_Disappoint( CDNGameRoom* pRoom, UINT uiUserObjectID, int iNpcID )
{
#if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
	CDNUserSession* pUser = pRoom->GetUserSession(uiUserObjectID);
	if( !pUser )
		return -1;

	return DNScriptAPI::api_npc_Disappoint(pUser, iNpcID);
#else 
	return -1;
#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
}

int api_user_ResetSkill(CDNGameRoom *pRoom, UINT nUserObjectID)
{
	CDNUserSession * pUser = pRoom->GetUserSession(nUserObjectID);
	if (!pUser) {
		return -1;
	}

#ifndef _FINAL_BUILD
	wstring wszString = FormatW(L"마을에서만 사용 가능한 기능입니다.\r\n");
	pUser->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
#endif	// _FINAL_BUILD

	return 1;
}

int api_user_SetSecondJobSkill(CDNGameRoom *pRoom, UINT nUserObjectID, int iSecondChangeJobID)
{
	CDNUserSession * pSession = pRoom->GetUserSession(nUserObjectID);
	if (!pSession) {
		return -1;
	}

	CDnPlayerActor* pPlayerActor = pSession->GetPlayerActor();
	if( pPlayerActor )
	{
		if( pPlayerActor->CanChangeJob( iSecondChangeJobID ) )
		{
			// 각 직업별로 들어가는 2차 전직 스킬들의 ID
			switch( iSecondChangeJobID )
			{
					// 소드마스터 -> 글래디에이터
				case 23:
					{
						// 임시 스킬 추가에 대해서 클라로 패킷도 보냄..
						pPlayerActor->AddTempSkill( 301 );	// 이베이젼 슬래쉬
						pPlayerActor->AddTempSkill( 302 );	// 트리플 슬래쉬 ex
						pPlayerActor->AddTempSkill( 303 );	// 피니쉬 어택
					}
					break;

					// 소드마스터 -> 워로드
				case 24:
					{
						pPlayerActor->AddTempSkill( 351 );	// 문 블레이드 댄스
						pPlayerActor->AddTempSkill( 352 );	// 문라이트 스플린터 ex
						pPlayerActor->AddTempSkill( 353 );	// 플래쉬 스탠스
					}
					break;

					// 머셔너리 -> 바바리안
				case 25:
					{
						pPlayerActor->AddTempSkill( 501 );	// 어펜드 히트
						pPlayerActor->AddTempSkill( 502 );	// 스톰프 ex
						pPlayerActor->AddTempSkill( 503 );	// 본 크래쉬
					}
					break;

					// 머셔너리 -> 디스트로이어
				case 26:
					{
						pPlayerActor->AddTempSkill( 551 );	// 브레이킹 포인트
						pPlayerActor->AddTempSkill( 552 );	// 플라잉 스윙 ex
						pPlayerActor->AddTempSkill( 553 );	// 메일스트롬 하울
					}
					break;

					// 보우마스터 -> 스나이퍼
				case 29:
					{
						pPlayerActor->AddTempSkill( 1301 );	// 크리티컬 브레이크
						pPlayerActor->AddTempSkill( 1302 );	// 차지샷 ex
						pPlayerActor->AddTempSkill( 1303 );	// 치팅 포인트
					}
					break;

					// 보우마스터 -> 아틸러리
				case 30:
					{
						pPlayerActor->AddTempSkill( 1351 );	// 매지컬 브리즈
						pPlayerActor->AddTempSkill( 1352 );	// 익스텐션 애로우 ex
						pPlayerActor->AddTempSkill( 1353 );	// 데토네이팅 애로우
					}
					break;

					// 아크로뱃 -> 템페스트
				case 31:
					{
						pPlayerActor->AddTempSkill( 1501 );	// 이베이드
						pPlayerActor->AddTempSkill( 1502 );	// 킥 앤 샷 ex
						pPlayerActor->AddTempSkill( 1503 );	// 허리케인 댄스
					}
					break;

					// 아크로뱃 -> 윈드워커
				case 32:
					{
						pPlayerActor->AddTempSkill( 1551 );	// 쇼타임
						pPlayerActor->AddTempSkill( 1552 );	// 스파이럴 킥 ex
						pPlayerActor->AddTempSkill( 1553 );	// 라이징 스톰
					}
					break;

					// 엘리멘탈로드 -> 셀레아나
				case 35:
					{
						pPlayerActor->AddTempSkill( 2301 );	// 익스플로전
						pPlayerActor->AddTempSkill( 2302 );	// 파이어볼 ex
						pPlayerActor->AddTempSkill( 2303 );	// 롤링 라바
					}
					break;

					// 엘리멘탈로드 -> 엘레스트라
				case 36:
					{
						pPlayerActor->AddTempSkill( 2351 );	// 아이스 배리어
						pPlayerActor->AddTempSkill( 2352 );	// 아이스 소드 ex
						pPlayerActor->AddTempSkill( 2353 );	// 아이시클 인젝션									}
						break;

						// 포스유저 -> 스매셔
				case 37:
					{
						pPlayerActor->AddTempSkill( 2501 );	// 아케인
						pPlayerActor->AddTempSkill( 2502 );	// 리니어 레이 ex
						pPlayerActor->AddTempSkill( 2503 );	// 오비틀 레이저
					}
					break;

					// 포스유저 -> 마제스티
				case 38:
					{
						pPlayerActor->AddTempSkill( 2551 );	// 스틸 매직
						pPlayerActor->AddTempSkill( 2552 );	// 그라비티볼 ex
						pPlayerActor->AddTempSkill( 2553 );	// 스위치 그라비티
					}
					break;

					// 팔라딘 -> 가디언
				case 41:
					{
						pPlayerActor->AddTempSkill( 3301 );	// 오토 블록 ex
						pPlayerActor->AddTempSkill( 3302 );	// 가디언 포스
						pPlayerActor->AddTempSkill( 3303 );	// 저스티스 크래쉬
					}
					break;

					// 팔라딘 -> 크루세이더
				case 42:
					{
						pPlayerActor->AddTempSkill( 3351 );	// 저지스 파워
						pPlayerActor->AddTempSkill( 3352 );	// 홀리 크로스 ex
						pPlayerActor->AddTempSkill( 3353 );	// 저지먼트 해머
					}
					break;

					// 프리스트 -> 세인트
				case 43:
					{
						pPlayerActor->AddTempSkill( 3501 );	// 홀리 쉴드
						pPlayerActor->AddTempSkill( 3502 );	// 렐릭 오브 라이트닝 ex
						pPlayerActor->AddTempSkill( 3503 );	// 쇼크 오브 렐릭
					}
					break;

					// 프리스트 -> 인퀴지터
				case 44:
					{
						pPlayerActor->AddTempSkill( 3551 );	// 쇼크 트랜지션
						pPlayerActor->AddTempSkill( 3552 );	// 라이트닝 볼트 ex
						pPlayerActor->AddTempSkill( 3553 );	// 컨스크레이션
					}
					break;

					// 엔지니어 -> 슈팅스타
				case 47:
					{
						pPlayerActor->AddTempSkill( 4301 );	// 스플래쉬
						pPlayerActor->AddTempSkill( 4302 ); // 핑퐁밤 EX
						pPlayerActor->AddTempSkill( 4303 ); // 알프레도 빔
					}
					break;

					// 엔지니어 -> 기어 마스터
				case 48:
					{
						pPlayerActor->AddTempSkill( 4351 ); // 체인소우 타워
						pPlayerActor->AddTempSkill( 4352 );	// 메카덕 EX
						pPlayerActor->AddTempSkill( 4353 ); // 빅 메카 붐버
					}
					break;

					// 알케미스트 -> 어뎁트
				case 50:
					{
						pPlayerActor->AddTempSkill( 4501 ); // C2H5OH
						pPlayerActor->AddTempSkill( 4502 ); // 마그마 펀치 EX
						pPlayerActor->AddTempSkill( 4503 ); // 아이스 빔
					}
					break;

					// 알케미스트 -> 피지션
				case 51:
					{
						pPlayerActor->AddTempSkill( 4551 ); // 러브 바이러스
						pPlayerActor->AddTempSkill( 4552 ); // 인젝터 EX
						pPlayerActor->AddTempSkill( 4553 );	// 힐 샤워
					}
					break;
					// 스크리머 -> 다크서머너
				case 55:
					{
						pPlayerActor->AddTempSkill( 5301 ); // 새디즘 플레저
						pPlayerActor->AddTempSkill( 5302 ); // 팬텀 클로 EX
						pPlayerActor->AddTempSkill( 5303 );	// 카오스 포메이션
					}
					break;
					//스크리머 -> 소울이터
				case 56:
					{
						pPlayerActor->AddTempSkill( 5401 ); // 스펙터 오프 페인
						pPlayerActor->AddTempSkill( 5402 ); // 스피릿 페이퍼 EX
						pPlayerActor->AddTempSkill( 5403 );	// 소울 스크림
					}
					break;
					//댄서 -> 블레이드댄서
				case 58:
					{
						pPlayerActor->AddTempSkill( 5501 ); // 스콜 플레이커
						pPlayerActor->AddTempSkill( 5505 ); // 거스트 디멘시아 
						pPlayerActor->AddTempSkill( 5502 ); // 그레이즈 댄서 ex
					}
					break;
					//댄서 -> 스피릿댄서
				case 59:
					{
						pPlayerActor->AddTempSkill( 5601 ); // 와이드 스팅어
						pPlayerActor->AddTempSkill( 5605 ); // 프라이 토르
						pPlayerActor->AddTempSkill( 5602 ); // 스토커 ex
					}
					break;
				}
			}

			// 먼저 임시 스킬부터 추가하고 임시 전직하도록 클라에 보내줘야 클라에서 제대로 스킬트리가
			// 초기화 된다.
			pPlayerActor->SendTempJobChange( iSecondChangeJobID );
		}
	}

#ifndef _FINAL_BUILD
	wstring wszString = FormatW(L"게임서버에서만 사용 가능한 기능입니다.\r\n");
	pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
#endif	// _FINAL_BUILD

	return 1;
}

int api_user_ClearSecondJobSkill(CDNGameRoom *pRoom, UINT nUserObjectID)
{
	CDNUserSession * pSession = pRoom->GetUserSession(nUserObjectID);
	if (!pSession) {
		return -1;
	}

	CDnPlayerActor* pPlayerActor = pSession->GetPlayerActor();
	if( pPlayerActor )
	{
		// 클라쪽으로도 패킷 나감.
		pPlayerActor->RemoveAllTempSkill();
		pPlayerActor->EndAddTempSkillAndSendRestoreTempJobChange();
	}

#ifndef _FINAL_BUILD
	wstring wszString = FormatW(L"게임서버에서만 사용 가능한 기능입니다.\r\n");
	pSession->SendChat( CHATTYPE_NORMAL, (int)wszString.size()*sizeof(WCHAR), L"", (WCHAR*)wszString.c_str() );
#endif	// _FINAL_BUILD

	return 1;
}

int api_user_RepairItem(CDNGameRoom *pRoom, UINT nUserObjectID, int iAllRepair)
{
#if defined( PRE_ADD_REPAIR_NPC )
	CDNUserSession* pUser = pRoom->GetUserSession(nUserObjectID);
	if( !pUser )
		return -1;

	return DNScriptAPI::api_user_RepairItem(pUser, iAllRepair);
#else
	return -1;
#endif // #if defined( PRE_ADD_NPC_REPUTATION_SYSTEM )
}

int api_ui_OpenExchangeEnchant(CDNGameRoom *pRoom, UINT nUserObjectID)
{
	DN_ASSERT(NULL != pRoom,	"Invalid!");

	CDNUserSession* pUser = pRoom->GetUserSession(nUserObjectID);
	if (!pUser) 
		return -1;

	return DNScriptAPI::api_ui_OpenExchangeEnchant(pUser);
}

//-------------------------------------------------------------------------------------------------------
// trigger 

int api_trigger_GetPartyCount( CDNGameRoom *pRoom )
{
	if( !pRoom ) return 0;
	return pRoom->GetUserCount();
}

int api_trigger_GetPartyActor( CDNGameRoom *pRoom, int nPartyIndex )
{
	if(!pRoom) return 0;

	CDNUserSession * pSession = pRoom->GetUserData( nPartyIndex - 1 );
	if(!pSession) return 0;

	return pSession->GetSessionID();
}

bool api_trigger_IsInsidePlayerToEventArea( CDNGameRoom *pRoom, int nActorHandle, int nEventAreaHandle )
{
	if(!pRoom) return false;

	DnActorHandle hActor = CDnActor::FindActorFromUniqueID( pRoom, nActorHandle );
	if( !hActor ) return false;

//	if( hActor->IsDie() ) return false;
	std::vector<CEtWorldEventArea *> VecArea;
	pRoom->GetWorld()->FindEventAreaFromCreateUniqueID( nEventAreaHandle, &VecArea );
	if( VecArea.empty() ) return false;

	return VecArea[0]->CheckArea( *hActor->GetPosition() );
}

void api_trigger_SetActionToProp( CDNGameRoom *pRoom, int nPropHandle, const char *szAction )
{
	if(!pRoom) return;

	std::vector<CEtWorldProp *> VecProp;
	pRoom->GetWorld()->FindPropFromCreateUniqueID( nPropHandle, &VecProp );

	if( VecProp.empty() ) return;

	int nPropType = ((CDnWorldProp*)VecProp[0])->GetPropType();
	switch( nPropType ) {
		case PTE_Static:
		case PTE_Camera:
			return;
		default:
			{
				CDnWorldActProp *pProp = static_cast<CDnWorldActProp *>(VecProp[0]);
				pProp->CmdAction( szAction );
			}
			break;
	}
}

void api_trigger_ShowDungeonClear( CDNGameRoom *pRoom, bool bClear, bool bIgnoreRewardItem )
{
	if(!pRoom) return;
	CDnGameTask *pTask = (CDnGameTask *)pRoom->GetTaskMng()->GetTask( "GameTask" );
	if( !pTask ) return;

	PROFILE_TICK_TEST( pTask->RequestDungeonClear( bClear, CDnActor::Identity(), bIgnoreRewardItem ) );
}

void api_trigger_SetPermitGate( CDNGameRoom *pRoom, int nGateIndex, int nOpenClose )
{
	if(!pRoom) return;
	CDnGameTask *pTask = (CDnGameTask *)pRoom->GetTaskMng()->GetTask( "GameTask" );
	if( !pTask ) return;

	EWorldEnum::PermitGateEnum PermitFlag;
	if( nOpenClose == 0 ) PermitFlag = EWorldEnum::PermitClose;
	else PermitFlag = EWorldEnum::PermitEnter;

	pTask->RequestChangeGateInfo( (char)nGateIndex, PermitFlag );

}

int api_trigger_GetMonsterAreaLiveCount( CDNGameRoom *pRoom, int nEventAreaHandle )
{
	if(!pRoom) return 0;
	CDnGameTask *pTask = (CDnGameTask *)pRoom->GetTaskMng()->GetTask( "GameTask" );
	if( !pTask ) return 0;

	return pTask->GetBirthAreaLiveCount( nEventAreaHandle );
}

int api_trigger_GetMonsterAreaTotalCount( CDNGameRoom *pRoom, int nEventAreaHandle )
{
	if(!pRoom) return 0;
	CDnGameTask *pTask = (CDnGameTask *)pRoom->GetTaskMng()->GetTask( "GameTask" );
	if( !pTask ) return 0;

	return pTask->GetBirthAreaTotalCount( nEventAreaHandle );
}

void api_trigger_RebirthMonster( CDNGameRoom *pRoom, int nEventAreaHandle )
{
	PROFILE_TICK_TEST_BLOCK_START( "api_trigger_RebirthMonster" );
	// 이함수는 좀 느리다.. 나중에 미리 태이블 만들어놓던지 쓰지 말던지..
	if(!pRoom) return;
	CDnGameTask *pTask = (CDnGameTask *)pRoom->GetTaskMng()->GetTask( "GameTask" );
	if( !pTask ) return;

	std::vector<CEtWorldEventArea *> VecArea;
	pRoom->GetWorld()->FindEventAreaFromCreateUniqueID( nEventAreaHandle, &VecArea );
	if( VecArea.empty() ) return;

	switch( VecArea[0]->GetControl()->GetUniqueID() ) {
		case ETE_UnitArea:
			{
				char szAreaName[64] = { 0, };
				int nMonsterTableID = -1;
				sscanf_s( VecArea[0]->GetName(), "%s %d", szAreaName, sizeof(szAreaName), &nMonsterTableID );
				UnitAreaStruct *pStruct = (UnitAreaStruct *)VecArea[0]->GetData();

				if( !pStruct->vCount ) return;

				if( stricmp( szAreaName, "monster" ) == NULL ) {
					api_trigger_GenerationMonster( pRoom, nEventAreaHandle, nMonsterTableID, (int)pStruct->vCount->x, (int)pStruct->vCount->y, 1 );
				}
				else if( stricmp( szAreaName, "monstergroup" ) == NULL ) {
					api_trigger_GenerationMonsterGroup( pRoom, nEventAreaHandle, nMonsterTableID, (int)pStruct->vCount->x, (int)pStruct->vCount->y, 1 );
				}
				else {
					DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TACTOR );
					int nActorTableID = pSox->GetItemIDFromField( "_StaticName", szAreaName );
					if( nActorTableID == -1 ) return;

					UnitAreaStruct *pLocalStruct = (UnitAreaStruct *)VecArea[0]->GetData();
					int nCount = 1;
					if( pLocalStruct->vCount ) {
						nCount = (int)pLocalStruct->vCount->x + _rand(pRoom)%( ( (int)pLocalStruct->vCount->y + 1 ) - (int)pLocalStruct->vCount->x );
					}

					EtVector3 vPos;
					SOBB Box;
					Box = *VecArea[0]->GetOBB();

					for( int i=0; i<nCount; i++ ) {

						if (pStruct->MonsterSpawn == 0)
							vPos = pTask->GetGenerationRandomPosition( &Box );
						else
							vPos = Box.Center;

						pTask->RequestGenerationMonsterFromActorID( nMonsterTableID, nActorTableID, vPos, EtVector3( 0.f, 0.f, 0.f ), EtVector3( 0.f, 0.f, 0.f ), &Box, nEventAreaHandle );
					}
				}
			}
			break;
		case ETE_MonsterSetArea:
			{
				MonsterSetAreaStruct *pLocalStruct = (MonsterSetAreaStruct *)VecArea[0]->GetData();
				api_trigger_GenerationMonsterSet( pRoom, nEventAreaHandle, pLocalStruct->nSetID, pLocalStruct->nPosID, 1, false );
			}
			break;
	}
	PROFILE_TICK_TEST_BLOCK_END();
}

void api_trigger_GenerationMonster( CDNGameRoom *pRoom, int nEventAreaHandle, int nMonsterTableID, int nMin, int nMax, int nTeamSetting )
{
	PROFILE_TICK_TEST_BLOCK_START( "api_trigger_GenerationMonster" );
	if(!pRoom) return;
	CDnGameTask *pTask = (CDnGameTask *)pRoom->GetTaskMng()->GetTask( "GameTask" );
	if( !pTask ) return;

	std::vector<CEtWorldEventArea *> VecArea;
	pRoom->GetWorld()->FindEventAreaFromCreateUniqueID( nEventAreaHandle, &VecArea );
	if( VecArea.empty() ) return;

	int nMonsterSpawn = pTask->GetMonsterSpawnType(nEventAreaHandle);

	int nTemp = nMax - nMin;
	if( nTemp < 1 ) nTemp = 1;
	int nCount = nMin + ( _rand(pRoom)%nTemp );

	EtVector3 vPos;
	SOBB Box = *VecArea[0]->GetOBB();

	for( int i=0; i<nCount; i++ ) {

		if (nMonsterSpawn == 0)
			vPos = pTask->GetGenerationRandomPosition( &Box );
		else
			vPos = Box.Center;

		pTask->RequestGenerationMonsterFromMonsterID( nMonsterTableID, vPos, EtVector3( 0.f, 0.f, 0.f ), EtVector3( 0.f, 0.f, 0.f ), &Box, nEventAreaHandle, nTeamSetting );
	}
	PROFILE_TICK_TEST_BLOCK_END();
}

void api_trigger_GenerationMonsterGroup( CDNGameRoom *pRoom, int nEventAreaHandle, int nMonsterGroupID, int nMin, int nMax, int nTeamSetting )
{
	PROFILE_TICK_TEST_BLOCK_START( "api_trigger_GenerationMonsterGroup" );
	if(!pRoom) return;
	CDnGameTask *pTask = (CDnGameTask *)pRoom->GetTaskMng()->GetTask( "GameTask" );
	if( !pTask ) return;

	std::vector<CEtWorldEventArea *> VecArea;
	pRoom->GetWorld()->FindEventAreaFromCreateUniqueID( nEventAreaHandle, &VecArea );
	if( VecArea.empty() ) return;

	int nTemp = nMax - nMin;
	if( nTemp < 1 ) nTemp = 1;
	int nCount = nMin + ( _rand(pRoom)%nTemp );

	EtVector3 vPos;
	SOBB Box = *VecArea[0]->GetOBB();

	pTask->RequestGenerationMonsterFromMonsterGroupID( nMonsterGroupID, nCount, Box, nEventAreaHandle, NULL, nTeamSetting );
	PROFILE_TICK_TEST_BLOCK_END();
}

void api_trigger_GenerationMonsterSet( CDNGameRoom *pRoom, int nEventAreaHandle, int nSetID, int nPosID, int nTeamSetting, bool bResetReference )
{
	PROFILE_TICK_TEST_BLOCK_START( "api_trigger_GenerationMonsterSet" );
	if(!pRoom) return;
	CDnGameTask *pTask = (CDnGameTask *)pRoom->GetTaskMng()->GetTask( "GameTask" );
	if( !pTask ) return;

	std::vector<CEtWorldEventArea *> VecArea;
	pRoom->GetWorld()->FindEventAreaFromCreateUniqueID( nEventAreaHandle, &VecArea );
	if( VecArea.empty() ) return;

	EtVector3 vPos;
	SOBB Box = *VecArea[0]->GetOBB();

	if( nSetID == 0 || nPosID == 0 ) {
		if( VecArea[0]->GetControl()->GetUniqueID() == ETE_MonsterSetArea ) {
			MonsterSetAreaStruct *pStruct = (MonsterSetAreaStruct *)VecArea[0]->GetData();
			if( nSetID == 0 ) nSetID = pStruct->nSetID;
			if( nPosID == 0 ) nPosID = pStruct->nPosID;
		}
		else {
			if( nSetID == 0 || nPosID == 0 ) return;
		}
	}

	pTask->RequestGenerationMonsterFromSetMonsterID( nSetID, nPosID, Box, nEventAreaHandle, NULL, nTeamSetting, bResetReference );
	PROFILE_TICK_TEST_BLOCK_END();
}

void api_trigger_GenerationMonsterSetInEventArea( CDNGameRoom *pRoom, int nEventAreaHandle, int nChangeSetID, int nChangePosID, int nTeamSetting, bool bResetReference )
{
	PROFILE_TICK_TEST_BLOCK_START( "api_trigger_GenerationMonsterSetInEventArea" );
	if(!pRoom) return;
	CDnGameTask *pTask = (CDnGameTask *)pRoom->GetTaskMng()->GetTask( "GameTask" );
	if( !pTask ) return;

	std::vector<CEtWorldEventArea *> VecArea;
	pRoom->GetWorld()->FindEventAreaFromCreateUniqueID( nEventAreaHandle, &VecArea );
	if( VecArea.empty() ) return;

	std::vector<CEtWorldEventArea *> VecArea2;
	pRoom->GetWorld()->ScanEventArea( *VecArea[0]->GetOBB(), &VecArea2 );
	for( DWORD i=0; i<VecArea2.size(); i++ ) {
		if( VecArea2[i]->GetControl()->GetUniqueID() != ETE_MonsterSetArea ||
			VecArea2[i]->GetCreateUniqueID() == nEventAreaHandle ) {
			VecArea2.erase( VecArea2.begin () + i );
			i--;
		}
	}
	if( VecArea2.empty() ) return;

	pTask->RequestGenerationMonsterFromSetMonsterID( nChangeSetID, nChangePosID, VecArea2, NULL, nTeamSetting, bResetReference );
	PROFILE_TICK_TEST_BLOCK_END();
}

void api_trigger_CmdMoveMonster( CDNGameRoom* pRoom, int nEventAreaHandle, int nTargetEventAreaHandle )
{
	api_trigger_CmdRandMoveMonster( pRoom, nEventAreaHandle, nTargetEventAreaHandle, 100 );
}

void api_trigger_CmdRandMoveMonster( CDNGameRoom* pRoom, int nEventAreaHandle, int nTargetEventAreaHandle, UINT uiMoveFrontRate )
{
	if(!pRoom) return;
	CDnGameTask *pTask = (CDnGameTask *)pRoom->GetTaskMng()->GetTask( "GameTask" );
	if( !pTask ) return;
	if( !pRoom->GetWorld() ) return;

	DNVector(DnActorHandle) hVecList;
	pTask->GetBirthAreaLiveActors( nEventAreaHandle, hVecList );
	if( hVecList.empty() ) return;

	std::vector<CEtWorldEventArea*> VecArea;
	pRoom->GetWorld()->FindEventAreaFromCreateUniqueID( nTargetEventAreaHandle, &VecArea );
	if( VecArea.empty() ) return;

	for( DWORD i=0; i<hVecList.size(); i++ ) 
	{
		if( !hVecList[i] )
			continue;
		if( !hVecList[i]->IsMonsterActor() ) continue;
		CDnMonsterActor* pMonster = static_cast<CDnMonsterActor*>(hVecList[i].GetPointer());

		pMonster->SetNaviDestination( VecArea[0]->GetOBB(), uiMoveFrontRate );
	}
}

bool api_trigger_CheckMonsterIsInsideArea( CDNGameRoom* pRoom, int nEventAreaHandle, int nTargetEventAreaHandle )
{
	if(!pRoom) return false;
	CDnGameTask *pTask = (CDnGameTask *)pRoom->GetTaskMng()->GetTask( "GameTask" );
	if( !pTask ) return false;

	return pTask->CheckMonsterIsInsideArea( nEventAreaHandle, nTargetEventAreaHandle );
}

bool api_trigger_CheckMonsterLessHP( CDNGameRoom* pRoom, int iMonsterID, int iHP )
{
	if( !pRoom )
		return false;
	CDnGameTask* pGameTask = static_cast<CDnGameTask*>(pRoom->GetTaskMng()->GetTask("GameTask") );
	if( !pGameTask )
		return false;

	return pGameTask->CheckMonsterLessHP( iMonsterID, iHP );
}

bool api_trigger_CheckInsideAreaMonsterLessHP( CDNGameRoom* pRoom, int iAreaUniqueID, int iHP )
{
	if( !pRoom )
		return false;
	CDnGameTask* pGameTask = static_cast<CDnGameTask*>(pRoom->GetTaskMng()->GetTask("GameTask") );
	if( !pGameTask )
		return false;

	return pGameTask->CheckInsideAreaMonsterLessHP( iAreaUniqueID, iHP );
}

int api_trigger_GenerationNpc(CDNGameRoom *pRoom, int nEventAreaHandle, int nNpcTableID)
{
	if(!pRoom) return -1;
	CDnGameTask *pTask = (CDnGameTask *)pRoom->GetTaskMng()->GetTask( "GameTask" );
	if( !pTask ) return -1;

	std::vector<CEtWorldEventArea *> VecArea;
	pRoom->GetWorld()->FindEventAreaFromCreateUniqueID( nEventAreaHandle, &VecArea );

	if( VecArea.empty() ) return -1;

	EtVector3 vPos;
	CEtWorldEventArea* pArea = VecArea[0];
	
	vPos.x = pArea->GetMin()->x + ((int)(pArea->GetMax()->x - pArea->GetMin()->x) / 2);
	vPos.z = pArea->GetMin()->z + ((int)(pArea->GetMax()->z - pArea->GetMin()->z) / 2);
	vPos.y = pRoom->GetWorld()->GetHeight( vPos );

	pTask->RequestGenerationNpc(nNpcTableID, vPos.x , vPos.y, vPos.z, pArea->GetRotate(), nEventAreaHandle );
	
	return 1;
}



/*
desc   : EventArea 에 NPC 를 제거한다.
param  : 이벤트영역 id
return : -1 => 이벤트영역 찾을수 없음,  1이면 성공
*/
int api_trigger_DestroyNpc(CDNGameRoom *pRoom, int nEventAreaHandle)
{
	if(!pRoom) return -1;
	CDnGameTask *pTask = (CDnGameTask *)pRoom->GetTaskMng()->GetTask( "GameTask" );
	if( !pTask ) return -1;

	pTask->RequestDestroyNpc(nEventAreaHandle);

	
	return 1;
}


void api_trigger_SetGameSpeed( CDNGameRoom *pRoom, float fPlaySpeed, DWORD dwDelay )
{
	if( !pRoom ) return;
	CDnGameTask *pTask = (CDnGameTask *)pRoom->GetTaskMng()->GetTask( "GameTask" );
	if( !pTask ) return;

	pTask->RequestChangeGameSpeed( fPlaySpeed, dwDelay );
}

void api_trigger_ShowProp( CDNGameRoom *pRoom, int nPropHandle, bool bShow )
{
	std::vector<CEtWorldProp *> VecProp;
	pRoom->GetWorld()->FindPropFromCreateUniqueID( nPropHandle, &VecProp );

	if( VecProp.empty() ) return;

	CDnWorldProp *pProp = static_cast<CDnWorldProp *>( VecProp[0] );
	if( !pProp ) return;

#ifdef PRE_FIX_PROP_SHOW_BY_TRIGGER
	bool bResult = pProp->SetShowInfo(PropDef::Option::eTrigger, bShow ? PropDef::Option::True : PropDef::Option::False);
	if (bResult == false)
	{
		DN_ASSERT(bResult, "Invalid! api_trigger_ShowProp");
		return;
	}
#endif
	pProp->CmdShow( bShow );
}

bool api_trigger_IsAllDieMonster( CDNGameRoom *pRoom, bool bCheckTeam )
{
	bool bAllDie = true;
	DnActorHandle hActor;
	for( DWORD i=0; i<CDnActor::s_pVecProcessList[pRoom->GetRoomID()].size(); i++ ) {
		hActor = CDnActor::s_pVecProcessList[pRoom->GetRoomID()][i]->GetMySmartPtr();
		if( !hActor ) continue;
		if( !hActor->IsMonsterActor() ) continue;
		if( bCheckTeam && hActor->GetTeam() == 0 ) continue;
		
		if( !hActor->IsDie() ) {
			bAllDie = false;
			break;
		}
	}
	return bAllDie;
}

int api_trigger_GetLastAimPropHandle( CDNGameRoom *pRoom, CEtTriggerElement *pElement )
{
	if(!pRoom) return -1;
	if( !pElement ) return -1;
	CEtTriggerObject *pObject = pElement->GetTriggerObject();
	if( !pObject ) return -1;

	return pObject->GetEventValueStore( "LastOperationProp" );

	/*
	if( !pRoom->GetWorld()->GetLastOperationProp() ) return -1;
	return pRoom->GetWorld()->GetLastOperationProp()->GetCreateUniqueID();
	*/
}

int api_trigger_GetLastAimPropActor( CDNGameRoom *pRoom, CEtTriggerElement *pElement )
{
	if(!pRoom) return -1;
	if( !pElement ) return -1;
	CEtTriggerObject *pObject = pElement->GetTriggerObject();
	if( !pObject ) return -1;

	return pObject->GetEventValueStore( "LastOperationActor" );

	/*
	if( !pRoom->GetWorld()->GetLastOperationActor() ) return -1;
	return pRoom->GetWorld()->GetLastOperationActor()->GetUniqueID();
	*/

}

bool api_trigger_IsProp( CDNGameRoom *pRoom, int nPropHandle )
{
	if(!pRoom) return false;

	std::vector<CEtWorldProp *> VecProp;
	pRoom->GetWorld()->FindPropFromCreateUniqueID( nPropHandle, &VecProp );

	if( VecProp.empty() ) return false;

	CDnWorldProp *pProp = static_cast<CDnWorldProp *>( VecProp[0] );
	if( !pProp ) return false;

	return pProp->IsShow();
}

bool api_trigger_IsBrokenProp( CDNGameRoom *pRoom, int nPropHandle )
{
	if(!pRoom) return false;

	std::vector<CEtWorldProp *> VecProp;
	pRoom->GetWorld()->FindPropFromCreateUniqueID( nPropHandle, &VecProp );

	if( VecProp.empty() ) return false;

	// ClassID 가 안맞으면 안부셔졌음.
	switch( ((CDnWorldProp*)VecProp[0])->GetPropType() ) {
		case PTE_Broken:
		case PTE_BrokenDamage:
		case PTE_HitMoveDamageBroken:
		case PTE_BuffBroken:
		case PTE_ShooterBroken:
			{
				CDnWorldBrokenProp *pProp = static_cast<CDnWorldBrokenProp *>( VecProp[0] );
				return pProp->IsBroken();
			}
			break;
	}
	return false;
}

int api_trigger_GetLastBrokenPropActor( CDNGameRoom *pRoom, CEtTriggerElement *pElement )
{
	if(!pRoom) return -1;
	if( !pElement ) return -1;
	CEtTriggerObject *pObject = pElement->GetTriggerObject();
	if( !pObject ) return -1;

	return pObject->GetEventValueStore( "LastBrokenPropActor" );
}


void api_trigger_DestroyMonsterArea( CDNGameRoom *pRoom, int nEventAreaHandle, int nDropType )
{
	if(!pRoom) return;
	CDnGameTask *pTask = (CDnGameTask *)pRoom->GetTaskMng()->GetTask( "GameTask" );
	if( !pTask ) return;

	DNVector(DnActorHandle) hVecList;
	pTask->GetBirthAreaLiveActors( nEventAreaHandle, hVecList );

	bool bDropItem = false;
	bool bAddExp = false;
	switch( nDropType ) {
		case 1: bDropItem = true; break; // 아이템 드랍
		case 2: bAddExp = true; break; // 경험치
		case 3:	bDropItem = bAddExp = true; // 경험치 + 아이템
	}

	for( DWORD i=0; i<hVecList.size(); i++ ) {
		if( !hVecList[i]->IsMonsterActor() ) continue;

		/*
		if( bAddExp ) {
			((CDnMonsterActor*)hVecList[i].GetPointer())->EnableDropItem( false );
			((CDnMonsterActor*)hVecList[i].GetPointer())->OnDie( DnActorHandle() );
		}
		*/
		hVecList[i]->CmdSuicide( bDropItem, bAddExp );
	}
}

void api_trigger_DestroyAllMonster( CDNGameRoom *pRoom, bool bDropItem, int nTeam )
{
	if(!pRoom) return;
	CDnGameTask *pTask = (CDnGameTask *)pRoom->GetTaskMng()->GetTask( "GameTask" );
	if( !pTask ) return;

	pTask->RequestDestroyAllMonster( bDropItem, nTeam );
}

void api_trigger_SetMonsterAIState( CDNGameRoom *pRoom, int nEventAreaHandle, int nType )
{
	if(!pRoom) return;
	CDnGameTask *pTask = (CDnGameTask *)pRoom->GetTaskMng()->GetTask( "GameTask" );
	if( !pTask ) return;

	DNVector(DnActorHandle) hVecList;
	pTask->GetBirthAreaLiveActors( nEventAreaHandle, hVecList );

	CDnMonsterActor *pMonster;
	for( DWORD i=0; i<hVecList.size(); i++ ) 
	{
		if( !hVecList[i] || !hVecList[i]->IsMonsterActor() )
			continue;
		pMonster = static_cast<CDnMonsterActor*>(hVecList[i].GetPointer());
		pMonster->SetAIState( (MAAiReceiver::AIState)nType );
	}
}

void api_trigger_PlayCutSceneByTrigger( CDNGameRoom *pRoom, int nCutSceneTableID, bool bFadeIn, int nQuestIndex, int nQuestStep )
{
	if(!pRoom) return;
	CDnGameTask *pTask = (CDnGameTask *)pRoom->GetTaskMng()->GetTask( "GameTask" );
	if( !pTask ) return;

	pTask->RequestPlayCutScene( nCutSceneTableID, bFadeIn, nQuestIndex, nQuestStep );
}

int api_trigger_GetGameTime( CDNGameRoom *pRoom )
{
	if(!pRoom) return 0;
	CDnGameTask *pTask = (CDnGameTask *)pRoom->GetTaskMng()->GetTask( "GameTask" );
	if( !pTask ) return 0;
	if( !pTask->IsSyncComplete() ) return 0;
	return (int)( pTask->GetLocalTime() / 1000 );
}

bool api_trigger_Delay( CDNGameRoom *pRoom, CEtTriggerElement *pElement, int nDelay )
{
	if( !pElement ) return true;
	if( !pRoom ) return true;
	CDnGameTask *pTask = (CDnGameTask *)pRoom->GetTaskMng()->GetTask( "GameTask" );
	if( !pTask ) return true;
	if( !pTask->IsSyncComplete() ) return false;
	if( pElement->GetLastExcuteTime() == -1 ) pElement->SetLastExcuteTime( pTask->GetLocalTime() );

#if !defined( PRE_TRIGGER_TEST )
	if( (DWORD)pTask->GetLocalTime() - pElement->GetLastExcuteTime() < nDelay ) return false;
#endif // #if defined( PRE_TRIGGER_TEST )

	return true;
}

int api_trigger_GetBattleModeCount( CDNGameRoom *pRoom, int nEventAreaHandle )
{
	if(!pRoom) return 0;
	CDnGameTask *pTask = (CDnGameTask *)pRoom->GetTaskMng()->GetTask( "GameTask" );
	if( !pTask ) return 0;

	DNVector(DnActorHandle) hVecList;
	pTask->GetBirthAreaLiveActors( nEventAreaHandle, hVecList );

	int nCount = 0;
	CDnMonsterActor *pActor;
	for( DWORD i=0; i<hVecList.size(); i++ ) 
	{
		if( !hVecList[i] || !hVecList[i]->IsMonsterActor() )
			continue;
		pActor = static_cast<CDnMonsterActor*>(hVecList[i].GetPointer());
		if( pActor->GetAggroTarget() ) 
			nCount++;
	}
	return nCount;
}

char *api_trigger_GetActionToProp( CDNGameRoom *pRoom, int nPropHandle )
{
	if(!pRoom) return "";

	std::vector<CEtWorldProp *> VecProp;
	pRoom->GetWorld()->FindPropFromCreateUniqueID( nPropHandle, &VecProp );

	if( VecProp.empty() ) return "";

	int nPropType = ((CDnWorldProp*)VecProp[0])->GetPropType();
	switch( nPropType ) 
	{
		case PTE_Static:
		case PTE_Camera:
			break;
		default:
		{
			CDnWorldActProp *pProp = static_cast<CDnWorldActProp *>(VecProp[0]);
			return (char*)pProp->GetCurrentAction();
		}
	}

	return "";
}

void api_trigger_ChangeMapByTrigger( CDNGameRoom *pRoom, int nMapIndex, int nGateNo )
{
	if(!pRoom) return;
	CDnGameTask *pTask = (CDnGameTask *)pRoom->GetTaskMng()->GetTask( "GameTask" );
	if( !pTask ) return;

#if defined( PRE_TRIGGER_TEST )
	return;
#endif // #if defined( PRE_TRIGGER_TEST )

	pTask->RequestChangeMap( nMapIndex, nGateNo );
}

int api_trigger_GetTalkNpcClassID( CDNGameRoom *pRoom, CEtTriggerElement *pElement )
{
	if(!pRoom) return -1;
	if( !pElement ) return -1;
	CEtTriggerObject *pObject = pElement->GetTriggerObject();
	if( !pObject ) return -1;

	int nValue = pObject->GetEventValueStore( "LastTalkNpc" );

	DnActorHandle hActor = CDnActor::FindActorFromUniqueID( pRoom, nValue );
	if( !hActor ) return -1;
	CDnNPCActor *pNpc = (CDnNPCActor *)hActor.GetPointer();
	return pNpc->GetNpcClassID();
}

int api_trigger_GetTalkNpcActor( CDNGameRoom *pRoom, CEtTriggerElement *pElement )
{
	/*
	if(!pRoom) return -1;

	DnActorHandle hNpc = pRoom->GetWorld()->GetLastTalkNpc();
	if( !hNpc ) return -1;

	return hNpc->GetUniqueID();
	*/
	if(!pRoom) return -1;
	if( !pElement ) return -1;
	CEtTriggerObject *pObject = pElement->GetTriggerObject();
	if( !pObject ) return -1;

	return pObject->GetEventValueStore( "LastTalkNpc" );

}

int api_trigger_GetTalkNpcTargetActor( CDNGameRoom *pRoom, CEtTriggerElement *pElement )
{
	/*
	if(!pRoom) return -1;

	DnActorHandle hActor = pRoom->GetWorld()->GetLastTalkNpcPlayer();
	if( !hActor ) return -1;

	return hActor->GetUniqueID();
	*/
	if(!pRoom) return -1;
	if( !pElement ) return -1;
	CEtTriggerObject *pObject = pElement->GetTriggerObject();
	if( !pObject ) return -1;

	return pObject->GetEventValueStore( "LastTalkActor" );

}

void api_trigger_AllUserEnableOperatorProp( CDNGameRoom *pRoom, int nPropHandle, bool bEnable )
{
	if(!pRoom) return;

	std::vector<CEtWorldProp *> VecProp;
	pRoom->GetWorld()->FindPropFromCreateUniqueID( nPropHandle, &VecProp );

	if( VecProp.empty() ) return;

	((CDnWorldProp*)VecProp[0])->EnableOperator( bEnable );
}

void api_trigger_EnableOperatorProp( CDNGameRoom *pRoom, int nPropHandle, int nActorHandle, bool bEnable )
{
	if(!pRoom) return;

	std::vector<CEtWorldProp *> VecProp;
	pRoom->GetWorld()->FindPropFromCreateUniqueID( nPropHandle, &VecProp );

	if( VecProp.empty() ) return;

	DnActorHandle hActor;
	if( nActorHandle == 0 || nActorHandle == -1 ) {
		for( DWORD i=0; i<pRoom->GetUserCount(); i++ ) {
			hActor = pRoom->GetUserData(i)->GetActorHandle();
			((CDnWorldProp*)VecProp[0])->CmdEnableOperator( hActor, bEnable );
		}
	}
	else {
		hActor = CDnActor::FindActorFromUniqueID( pRoom, nActorHandle );
		if( !hActor ) return;

		((CDnWorldProp*)VecProp[0])->CmdEnableOperator( hActor, bEnable );
	}
}

void api_trigger_SetActionToPropActor( CDNGameRoom *pRoom, int nPropHandle, int nActorHandle, const char *szAction )
{
	if(!pRoom) return;

	std::vector<CEtWorldProp *> VecProp;
	pRoom->GetWorld()->FindPropFromCreateUniqueID( nPropHandle, &VecProp );

	if( VecProp.empty() ) return;

	CDnWorldActProp *pProp = NULL;
	int nPropType = ((CDnWorldProp*)VecProp[0])->GetPropType();
	switch( nPropType ) {
		case PTE_Static:
		case PTE_Camera:
			return;
		default:
			pProp = static_cast<CDnWorldActProp *>(VecProp[0]);
			break;
	}
	if( !pProp ) return;

	DnActorHandle hActor = CDnActor::FindActorFromUniqueID( pRoom, nActorHandle );
	if( !hActor ) return;

	pProp->CmdAction( hActor, szAction );
}

void api_trigger_EnableTriggerElement( CDNGameRoom *pRoom, CEtTriggerElement *pElement, bool bEnable )
{
	if( !pRoom ) return;
	if( !pElement ) return;
	pElement->SetEnable( bEnable );
}

void api_trigger_SetActionToActor( CDNGameRoom *pRoom, int nEventAreaHandle, const char *szAction, int nLoopCount, float fBlendFrame )
{
	if(!pRoom) return;
	CDnGameTask *pTask = (CDnGameTask *)pRoom->GetTaskMng()->GetTask( "GameTask" );
	if( !pTask ) return;

	DNVector(DnActorHandle) hVecList;
	pTask->GetBirthAreaLiveActors( nEventAreaHandle, hVecList );

	for( DWORD i=0; i<hVecList.size(); i++ ) {
		DnActorHandle hActor = hVecList[i];
		hActor->CmdAction( szAction, nLoopCount, fBlendFrame );
	}
}

void api_trigger_EnableOperatorNpc( CDNGameRoom *pRoom, int nEventAreaHandle, int nActorHandle, bool bEnable )
{
	if(!pRoom) return;
	CDnGameTask *pTask = (CDnGameTask *)pRoom->GetTaskMng()->GetTask( "GameTask" );
	if( !pTask ) return;

	DNVector(DnActorHandle) hVecList;
	pTask->GetBirthAreaLiveActors( nEventAreaHandle, hVecList );

	for( DWORD i=0; i<hVecList.size(); i++ ) {
		DnActorHandle hNpc = hVecList[i];
		if( !hNpc || !hNpc->IsNpcActor() ) 
			continue;

		CDnNPCActor *pNpc = static_cast<CDnNPCActor *>(hNpc.GetPointer());

		DnActorHandle hActor;
		if( nActorHandle == 0 || nActorHandle == -1 ) {
			for( DWORD j=0; j<pRoom->GetUserCount(); j++ ) {
				hActor = pRoom->GetUserData(j)->GetActorHandle();
				pNpc->CmdEnableOperator( hActor, bEnable );
			}
		}
		else {
			hActor = CDnActor::FindActorFromUniqueID( pRoom, nActorHandle );
			if( !hActor ) continue;

			pNpc->CmdEnableOperator( hActor, bEnable );
		}
	}
}

void api_trigger_EnableOperatorNpcFromPropNpc( CDNGameRoom *pRoom, int nPropHandle, int nActorHandle, bool bEnable )
{
	if(!pRoom) return;
	CDnGameTask *pTask = (CDnGameTask *)pRoom->GetTaskMng()->GetTask( "GameTask" );
	if( !pTask ) return;

	std::vector<CEtWorldProp *> pVecList;
	CDnWorld::GetInstance(pRoom).FindPropFromCreateUniqueID( nPropHandle, &pVecList );
	if( pVecList.empty() ) return;

	for( DWORD i=0; i<pVecList.size(); i++ ) {
		CDnWorldProp *pProp = (CDnWorldProp *)pVecList[i];
		if( !pProp || pProp->GetPropType() != PTE_Npc ) continue;

		DnActorHandle hNpc = ((CDnWorldNpcProp*)pProp)->GetActorHandle();
		if( !hNpc || !hNpc->IsNpcActor() ) continue;

		CDnNPCActor *pNpc = static_cast<CDnNPCActor *>(hNpc.GetPointer());

		DnActorHandle hActor;
		if( nActorHandle == 0 || nActorHandle == -1 ) {
			for( DWORD j=0; j<pRoom->GetUserCount(); j++ ) {
				hActor = pRoom->GetUserData(j)->GetActorHandle();
				pNpc->CmdEnableOperator( hActor, bEnable );
			}
		}
		else {
			hActor = CDnActor::FindActorFromUniqueID( pRoom, nActorHandle );
			if( !hActor ) continue;

			pNpc->CmdEnableOperator( hActor, bEnable );
		}
	}
}

void api_trigger_EnableTriggerObject( CDNGameRoom *pRoom, const char *szTriggerObjectName, bool bEnable )
{
	if(!pRoom) return;
	CDnGameTask *pTask = (CDnGameTask *)pRoom->GetTaskMng()->GetTask( "GameTask" );
	if( !pTask ) return;

	pRoom->GetWorld()->EnableTriggerObject( szTriggerObjectName, bEnable );
}


void api_trigger_EnableMonsterNoDamage( CDNGameRoom *pRoom, int nEventAreaHandle, bool bEnable )
{
	if(!pRoom) return;
	CDnGameTask *pTask = (CDnGameTask *)pRoom->GetTaskMng()->GetTask( "GameTask" );
	if( !pTask ) return;

	DNVector(DnActorHandle) hVecList;
	pTask->GetBirthAreaLiveActors( nEventAreaHandle, hVecList );

	CDnMonsterActor *pMonster;
	for( DWORD i=0; i<hVecList.size(); i++ ) 
	{
		if( !hVecList[i] || !hVecList[i]->IsMonsterActor() )
			continue;
		pMonster = static_cast<CDnMonsterActor*>(hVecList[i].GetPointer());
		pMonster->EnableNoDamage( bEnable );
	}
}

void api_trigger_WarpActor( CDNGameRoom *pRoom, int nActorHandle, int nTargetEventAreaHandle )
{
	if(!pRoom) return;
	CDnGameTask *pTask = (CDnGameTask *)pRoom->GetTaskMng()->GetTask( "GameTask" );
	if( !pTask ) return;

	DnActorHandle hActor = CDnActor::FindActorFromUniqueID( pRoom, nActorHandle );
	if( !hActor ) return;

	std::vector<CEtWorldEventArea *> VecArea;
	pRoom->GetWorld()->FindEventAreaFromCreateUniqueID( nTargetEventAreaHandle, &VecArea );
	if( VecArea.empty() ) return;
#if defined(PRE_ADD_POSITIONHACK_POS_LOG)
	if( hActor->IsPlayerActor() && pRoom->bIsGuildWarMode()) // 프랍을 클릭했을때 InvalidCount가 있으면 좌표를 남기도록 로그를 추가합니다.
	{
		CDnPlayerActor* pPlayerActor = static_cast<CDnPlayerActor*>(hActor.GetPointer());
		if( pPlayerActor && pPlayerActor->GetPlayerSpeedHackChecker() )
			((CDnPlayerSpeedHackChecker*)pPlayerActor->GetPlayerSpeedHackChecker())->CheckInvalidActorAndPositionLog();
	}
#endif	// #if defined(PRE_ADD_POSITIONHACK_POS_LOG)
	SOBB *pOBB = VecArea[0]->GetOBB();
	hActor->CmdWarp( pOBB->Center, EtVec3toVec2( pOBB->Axis[2] ), NULL, true );
}

void api_trigger_WarpActorFromEventArea( CDNGameRoom *pRoom, int nMonsterEventAreaHandle, int nTargetEventAreaHandle )
{
	if(!pRoom) return;
	CDnGameTask *pTask = (CDnGameTask *)pRoom->GetTaskMng()->GetTask( "GameTask" );
	if( !pTask ) return;

	std::vector<CEtWorldEventArea *> VecArea;
	pRoom->GetWorld()->FindEventAreaFromCreateUniqueID( nTargetEventAreaHandle, &VecArea );
	if( VecArea.empty() ) return;

	SOBB *pOBB = VecArea[0]->GetOBB();

	DNVector(DnActorHandle) hVecList;
	pTask->GetBirthAreaLiveActors( nMonsterEventAreaHandle, hVecList );
	for( DWORD i=0; i<hVecList.size(); i++ ) {
		if( hVecList[i] ) hVecList[i]->CmdWarp( pOBB->Center, EtVec3toVec2( pOBB->Axis[2] ), NULL, true );
	}
}


bool api_trigger_IsInsidePropToEventArea( CDNGameRoom* pRoom, int nPropHandle, int nTargetEventAreaHandle )
{
	if(!pRoom) return false;
	CDnGameTask *pTask = (CDnGameTask *)pRoom->GetTaskMng()->GetTask( "GameTask" );
	if( !pTask ) return false;

	std::vector<CEtWorldProp *> VecProp;
	CDnWorld::GetInstance(pRoom).FindPropFromCreateUniqueID( nPropHandle, &VecProp );
	if( VecProp.empty() ) return false;

	std::vector<CEtWorldEventArea *> VecArea;
	pRoom->GetWorld()->FindEventAreaFromCreateUniqueID( nTargetEventAreaHandle, &VecArea );
	if( VecArea.empty() ) return false;

	SOBB *pOBB = VecArea[0]->GetOBB();
	
	// 정확하게 박스충돌 체크
	SOBB PropOBB;
	SSphere Sphere;
	VecProp[0]->GetBoundingBox( PropOBB );
	return TestOBBToOBB( *pOBB, PropOBB );

	// 간단하게 인사이드 체크
//	return pOBB->IsInside( ((CDnWorldProp*)VecProp[0])->GetMatEx()->m_vPosition );
}

void api_trigger_ChangeTeamFromEventArea( CDNGameRoom *pRoom, int nEventAreaHandle, int nTeam )
{
	if(!pRoom) return;
	CDnGameTask *pTask = (CDnGameTask *)pRoom->GetTaskMng()->GetTask( "GameTask" );
	if( !pTask ) return;

	DNVector(DnActorHandle) hVecList;
	pTask->GetBirthAreaLiveActors( nEventAreaHandle, hVecList );

	for( DWORD i=0; i<hVecList.size(); i++ ) 
	{
		if( !hVecList[i] || !hVecList[i]->IsMonsterActor() )
			continue;

		CDnMonsterActor *pMonster = static_cast<CDnMonsterActor *>(hVecList[i].GetPointer());
		pMonster->CmdChangeTeam( nTeam );
		pMonster->SetAggroTarget( CDnActor::Identity() );
	}
}

bool api_trigger_IsDieActor( CDNGameRoom *pRoom, int nActorHandle )
{
	if(!pRoom) return true;

	DnActorHandle hActor = CDnActor::FindActorFromUniqueID( pRoom, nActorHandle );
	if( !hActor ) return true;

	return hActor->IsDie();
}

int api_trigger_GetUserLevelByTrigger( CDNGameRoom *pRoom, int nActorHandle )
{
	if(!pRoom) return 0;

	DnActorHandle hActor = CDnActor::FindActorFromUniqueID( pRoom, nActorHandle );
	if( !hActor ) return 0;

	return hActor->GetLevel();
}

int api_trigger_GetMonsterActorFromEventArea( CDNGameRoom *pRoom, int nEventAreaHandle, int nIndex )
{
	if( !pRoom ) return 0;
	CDnGameTask *pTask = (CDnGameTask *)pRoom->GetTaskMng()->GetTask( "GameTask" );
	if( !pTask ) return 0;

	DNVector(DnActorHandle) hVecResult;
	pTask->GetBirthAreaLiveActors( nEventAreaHandle, hVecResult );
	nIndex--;

	if( nIndex < 0 || nIndex >= (int)hVecResult.size() ) return 0;
	return hVecResult[nIndex]->GetUniqueID();
}

void api_trigger_DropItemToEventArea( CDNGameRoom *pRoom, int nEventAreaHandle, int nItemID, int nCount, DWORD dwOwnerActorHandle )
{
	if( !pRoom ) return;

#if defined( PRE_FIX_48517 )
	// dwOwnerActorHandle이 0인 경우에는 소유권없음이므로 무조건 드랍
	if( dwOwnerActorHandle > 0 && !pRoom->GetUserSession( dwOwnerActorHandle ) )
		return;
#endif
	CDnItemTask *pTask = (CDnItemTask *)pRoom->GetTaskMng()->GetTask( "ItemTask" );
	CDnGameTask *pGameTask = (CDnGameTask *)pRoom->GetTaskMng()->GetTask( "GameTask" );
	if( !pTask ) return;
	if( !pGameTask ) return;

	std::vector<CEtWorldEventArea *> pVecList;
	CDnWorld::GetInstance(pRoom).FindEventAreaFromCreateUniqueID( nEventAreaHandle, &pVecList );
	if( pVecList.empty() ) return;

	SOBB *pOBB = pVecList[0]->GetOBB();
	EtVector3 vPos;

	int nMonsterSpawn = pGameTask->GetMonsterSpawnType(nEventAreaHandle);

	if (nMonsterSpawn == 0)
		vPos = pGameTask->GetGenerationRandomPosition( pOBB );
	else
		vPos = pOBB->Center;	

	if( nCount > 0 ) {
		pTask->RequestDropItem( STATIC_INSTANCE_(CDnDropItem::s_dwUniqueCount)++, vPos, nItemID, _roomrand(pRoom), nCount, 0, ( dwOwnerActorHandle == 0 ) ? -1 : dwOwnerActorHandle );
	}
	else {
		DNVector(CDnItem::DropItemStruct) VecResult;
		CDnDropItem::CalcDropItemList( pRoom, nItemID, VecResult );
		for( DWORD i=0; i<VecResult.size(); i++ ) {
#if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
			pTask->RequestDropItem( VecResult[i].dwUniqueID, vPos, VecResult[i].nItemID, VecResult[i].nSeed, VecResult[i].nCount, (short)( ( 360 / VecResult.size() ) * i ), ( dwOwnerActorHandle == 0 ) ? -1 : dwOwnerActorHandle, VecResult[i].nEnchantID );
#else	// #if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
			pTask->RequestDropItem( VecResult[i].dwUniqueID, vPos, VecResult[i].nItemID, VecResult[i].nSeed, VecResult[i].nCount, (short)( ( 360 / VecResult.size() ) * i ), ( dwOwnerActorHandle == 0 ) ? -1 : dwOwnerActorHandle );
#endif	// #if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
		}
	}
}

void api_trigger_DropItemToActor( CDNGameRoom *pRoom, int nActorHandle, int nItemID, int nCount, DWORD dwOwnerActorHandle )
{
	if( !pRoom ) return;
	CDnItemTask *pTask = (CDnItemTask *)pRoom->GetTaskMng()->GetTask( "ItemTask" );
	CDnGameTask *pGameTask = (CDnGameTask *)pRoom->GetTaskMng()->GetTask( "GameTask" );
	if( !pTask ) return;
	if( !pGameTask ) return;

	DnActorHandle hActor = CDnActor::FindActorFromUniqueID( pRoom, nActorHandle );
	if( !hActor ) return;

	EtVector3 vPos = *hActor->GetPosition();

	if( nCount > 0 ) {
		pTask->RequestDropItem( STATIC_INSTANCE_(CDnDropItem::s_dwUniqueCount)++, vPos, nItemID, _roomrand(pRoom), nCount, 0, ( dwOwnerActorHandle == 0 ) ? -1 : dwOwnerActorHandle );
	}
	else {
		DNVector(CDnItem::DropItemStruct) VecResult;
		CDnDropItem::CalcDropItemList( pRoom, nItemID, VecResult );
		for( DWORD i=0; i<VecResult.size(); i++ ) {
#if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
			pTask->RequestDropItem( VecResult[i].dwUniqueID, vPos, VecResult[i].nItemID, VecResult[i].nSeed, VecResult[i].nCount, (short)( ( 360 / VecResult.size() ) * i ), ( dwOwnerActorHandle == 0 ) ? -1 : dwOwnerActorHandle, VecResult[i].nEnchantID );
#else	// #if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
			pTask->RequestDropItem( VecResult[i].dwUniqueID, vPos, VecResult[i].nItemID, VecResult[i].nSeed, VecResult[i].nCount, (short)( ( 360 / VecResult.size() ) * i ), ( dwOwnerActorHandle == 0 ) ? -1 : dwOwnerActorHandle );
#endif	// #if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
		}
	}
}

void api_trigger_DropGroupItemToEventArea( CDNGameRoom *pRoom, int nEventAreaHandle, int nItemID, DWORD dwOwnerActorHandle )
{
	if( !pRoom ) return;
	CDnItemTask *pTask = (CDnItemTask *)pRoom->GetTaskMng()->GetTask( "ItemTask" );
	CDnGameTask *pGameTask = (CDnGameTask *)pRoom->GetTaskMng()->GetTask( "GameTask" );
	if( !pTask ) return;
	if( !pGameTask ) return;

	std::vector<CEtWorldEventArea *> pVecList;
	CDnWorld::GetInstance(pRoom).FindEventAreaFromCreateUniqueID( nEventAreaHandle, &pVecList );
	if( pVecList.empty() ) return;

	SOBB *pOBB = pVecList[0]->GetOBB();
	EtVector3 vPos;

	int nMonsterSpawn = pGameTask->GetMonsterSpawnType(nEventAreaHandle);

	if (nMonsterSpawn == 0)
		vPos = pGameTask->GetGenerationRandomPosition( pOBB );
	else
		vPos = pOBB->Center;

	DNVector(CDnItem::DropItemStruct) VecResult;
	CDnDropItem::CalcDropItemList( pRoom, pGameTask->GetStageDifficulty(), nItemID, VecResult );
	for( DWORD i=0; i<VecResult.size(); i++ ) {
#if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
		pTask->RequestDropItem( VecResult[i].dwUniqueID, vPos, VecResult[i].nItemID, VecResult[i].nSeed, VecResult[i].nCount, (short)( ( 360 / VecResult.size() ) * i ), ( dwOwnerActorHandle == 0 ) ? -1 : dwOwnerActorHandle, VecResult[i].nEnchantID );
#else	// #if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
		pTask->RequestDropItem( VecResult[i].dwUniqueID, vPos, VecResult[i].nItemID, VecResult[i].nSeed, VecResult[i].nCount, (short)( ( 360 / VecResult.size() ) * i ), ( dwOwnerActorHandle == 0 ) ? -1 : dwOwnerActorHandle );
#endif	// #if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
	}
}

void api_trigger_DropGroupItemToActor( CDNGameRoom *pRoom, int nActorHandle, int nItemID, DWORD dwOwnerActorHandle )
{
	if( !pRoom ) return;
	CDnItemTask *pTask = (CDnItemTask *)pRoom->GetTaskMng()->GetTask( "ItemTask" );
	CDnGameTask *pGameTask = (CDnGameTask *)pRoom->GetTaskMng()->GetTask( "GameTask" );
	if( !pTask ) return;
	if( !pGameTask ) return;

	DnActorHandle hActor = CDnActor::FindActorFromUniqueID( pRoom, nActorHandle );
	if( !hActor ) return;

	EtVector3 vPos = *hActor->GetPosition();

	DNVector(CDnItem::DropItemStruct) VecResult;
	CDnDropItem::CalcDropItemList( pRoom, pGameTask->GetStageDifficulty(), nItemID, VecResult );
	for( DWORD i=0; i<VecResult.size(); i++ ) {
#if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
		pTask->RequestDropItem( VecResult[i].dwUniqueID, vPos, VecResult[i].nItemID, VecResult[i].nSeed, VecResult[i].nCount, (short)( ( 360 / VecResult.size() ) * i ), ( dwOwnerActorHandle == 0 ) ? -1 : dwOwnerActorHandle, VecResult[i].nEnchantID );
#else	// #if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
		pTask->RequestDropItem( VecResult[i].dwUniqueID, vPos, VecResult[i].nItemID, VecResult[i].nSeed, VecResult[i].nCount, (short)( ( 360 / VecResult.size() ) * i ), ( dwOwnerActorHandle == 0 ) ? -1 : dwOwnerActorHandle );
#endif	// #if defined(PRE_ADD_STAGE_CLEAR_ENCHANT_REWARD)
	}
}

void api_trigger_ShowChatBalloonToMonsterArea( CDNGameRoom *pRoom, int nEventAreaHandle, int nUIStringIndex )
{
	if( !pRoom ) return;
	CDnGameTask *pTask = (CDnGameTask *)pRoom->GetTaskMng()->GetTask( "GameTask" );
	if( !pTask ) return;

	DNVector(DnActorHandle) hVecList;
	pTask->GetBirthAreaLiveActors( nEventAreaHandle, hVecList );
	if( hVecList.empty() ) return;

	if( !CEtUIXML::IsActive() )
		return;

#if defined(PRE_ADD_MULTILANGUAGE)
	for( DWORD i=0; i<hVecList.size(); i++ ) {
		if( hVecList[i] ) {
			hVecList[i]->CmdChatBalloon( nUIStringIndex );
		}
	}
#else		//#if defined(PRE_ADD_MULTILANGUAGE)
	std::wstring wszString = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nUIStringIndex );
	if( wszString.empty() ) return;

	for( DWORD i=0; i<hVecList.size(); i++ ) {
		if( hVecList[i] ) {
			hVecList[i]->CmdChatBalloon( wszString.c_str() );
		}
	}
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)
}

void api_trigger_ShowChatBalloonToActor( CDNGameRoom *pRoom, int nActorHandle, int nUIStringIndex )
{
	if( !pRoom ) return;
	CDnGameTask *pTask = (CDnGameTask *)pRoom->GetTaskMng()->GetTask( "GameTask" );
	if( !pTask ) return;

	DnActorHandle hActor = CDnActor::FindActorFromUniqueID( pRoom, nActorHandle );
	if( !hActor ) return;

#if defined(PRE_ADD_MULTILANGUAGE)
	hActor->CmdChatBalloon( nUIStringIndex );
#else		//#if defined(PRE_ADD_MULTILANGUAGE)
	std::wstring wszString = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nUIStringIndex );
	if( wszString.empty() ) return;

	hActor->CmdChatBalloon( wszString.c_str() );
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)
}

void api_trigger_ShowChatBalloonToProp( CDNGameRoom *pRoom, int nPropHandle, int nUIStringIndex )
{
	if( !pRoom ) return;
	CDnGameTask *pTask = (CDnGameTask *)pRoom->GetTaskMng()->GetTask( "GameTask" );
	if( !pTask ) return;

	std::vector<CEtWorldProp *> pVecList;
	CDnWorld::GetInstance(pRoom).FindPropFromCreateUniqueID( nPropHandle, &pVecList );
	if( pVecList.empty() ) return;

	CDnWorldProp *pProp = static_cast<CDnWorldProp *>( pVecList[0] );
	if( !pProp ) return;

	pProp->CmdChatBalloon( nUIStringIndex );
}

void api_trigger_HideChatBalloonToMonsterArea( CDNGameRoom *pRoom, int nEventAreaHandle )
{
	if( !pRoom ) return;
	CDnGameTask *pTask = (CDnGameTask *)pRoom->GetTaskMng()->GetTask( "GameTask" );
	if( !pTask ) return;

	DNVector(DnActorHandle) hVecList;
	pTask->GetBirthAreaLiveActors( nEventAreaHandle, hVecList );
	if( hVecList.empty() ) return;

	if( !CEtUIXML::IsActive() )
		return;

#if defined(PRE_ADD_MULTILANGUAGE)
	for( DWORD i=0; i<hVecList.size(); i++ ) {
		if( hVecList[i] ) {
			hVecList[i]->CmdChatBalloon( 0 );
		}
	}
#else		//#if defined(PRE_ADD_MULTILANGUAGE)
	for( DWORD i=0; i<hVecList.size(); i++ ) {
		if( hVecList[i] ) {
			hVecList[i]->CmdChatBalloon( L"HIDE_TRIGGER_CHATBALLOON" );
		}
	}
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)
}

void api_trigger_HideChatBalloonToProp( CDNGameRoom *pRoom, int nPropHandle )
{
	// Hide함수를 따로 만드는 대신 Show함수에다가 UIStringIndex가 0일때 하이드 하는거로 하려다가,
	// 기획자분이 Hide함수가 따로 있는게 편하다고 하셔서 이렇게 별도로 만들어둡니다.
	if( !pRoom ) return;
	CDnGameTask *pTask = (CDnGameTask *)pRoom->GetTaskMng()->GetTask( "GameTask" );
	if( !pTask ) return;

	std::vector<CEtWorldProp *> pVecList;
	CDnWorld::GetInstance(pRoom).FindPropFromCreateUniqueID( nPropHandle, &pVecList );
	if( pVecList.empty() ) return;

	CDnWorldProp *pProp = static_cast<CDnWorldProp *>( pVecList[0] );
	if( !pProp ) return;

	pProp->CmdChatBalloon( 0 );
}

void api_trigger_SetForceAggroToMonsterArea( CDNGameRoom *pRoom, int nEventAreaHandle, int nTargetEventArea, int nValue )
{
	if( !pRoom ) return;
	CDnGameTask *pTask = (CDnGameTask *)pRoom->GetTaskMng()->GetTask( "GameTask" );
	if( !pTask ) return;

	DNVector(DnActorHandle) hVecList;
	DNVector(DnActorHandle) hVecTargetList;

	pTask->GetBirthAreaLiveActors( nEventAreaHandle, hVecList );
	pTask->GetBirthAreaLiveActors( nTargetEventArea, hVecTargetList );

	if( hVecList.empty() || hVecTargetList.empty() ) return;

	for( DWORD i=0; i<hVecList.size(); i++ ) 
	{
		if( hVecList[i] && hVecList[i]->IsMonsterActor() ) 
		{
			CDnMonsterActor *pMonster = static_cast<CDnMonsterActor *>(hVecList[i].GetPointer());
			pMonster->GetAggroSystem()->AddAggro( hVecTargetList[_rand(pRoom)%hVecTargetList.size()], nValue, true );
		}
	}
}

void api_trigger_SetForceAggroToProp( CDNGameRoom *pRoom, int nEventAreaHandle, int nPropHandle, int nValue )
{
	if( !pRoom ) return;
	CDnGameTask *pTask = (CDnGameTask *)pRoom->GetTaskMng()->GetTask( "GameTask" );
	if( !pTask ) return;

	DNVector(DnActorHandle) hVecList;

	pTask->GetBirthAreaLiveActors( nEventAreaHandle, hVecList );

	if( hVecList.empty() ) return;

	std::vector<CEtWorldProp *> pVecTargetList;
	CDnWorld::GetInstance(pRoom).FindPropFromCreateUniqueID( nPropHandle, &pVecTargetList );
	if( pVecTargetList.empty() ) return;

	MAActorProp *pActorProp = dynamic_cast<MAActorProp *>(pVecTargetList[0]);
	if( !pActorProp ) return;

	DnActorHandle hTargetActor = pActorProp->GetActorHandle();
	if( !hTargetActor ) return;

	for( DWORD i=0; i<hVecList.size(); i++ ) 
	{
		if( hVecList[i] && hVecList[i]->IsMonsterActor() ) 
		{
			CDnMonsterActor *pMonster = static_cast<CDnMonsterActor *>(hVecList[i].GetPointer());
			pMonster->GetAggroSystem()->AddAggro( hTargetActor, nValue, true );
		}
	}
}

void api_trigger_ShowExposureInfo( CDNGameRoom *pRoom, int nEventAreaHandle, bool bShow )
{
	if( !pRoom ) return;
	CDnGameTask *pTask = (CDnGameTask *)pRoom->GetTaskMng()->GetTask( "GameTask" );
	if( !pTask ) return;

	DNVector(DnActorHandle) hVecList;

	pTask->GetBirthAreaLiveActors( nEventAreaHandle, hVecList );

	if( hVecList.empty() ) return;

	for( DWORD i=0; i<hVecList.size(); i++ ) {
		if( hVecList[i] ) {
			hVecList[i]->CmdShowExposureInfo( bShow );
		}
	}
}

void api_trigger_DLChangeRound( CDNGameRoom *pRoom, bool bBoss, int iNextTotalRound/*=0*/ )
{
	if( !pRoom ) return;
	CDnGameTask *pTask = (CDnGameTask *)pRoom->GetTaskMng()->GetTask( "GameTask" );
	if( !pTask ) return;
	if( pTask->GetGameTaskType() != GameTaskType::DarkLair ) return;

	CDnDLGameTask *pDLTask = (CDnDLGameTask *)pTask;
#ifdef PRE_MOD_DARKLAIR_RECONNECT
	pDLTask->UpdateRound( iNextTotalRound, bBoss );
#else // PRE_MOD_DARKLAIR_RECONNECT
	pDLTask->UpdateRound( iNextTotalRound );
#endif // PRE_MOD_DARKLAIR_RECONNECT
}

int api_trigger_DLGetRound( CDNGameRoom *pRoom )
{
	if( !pRoom ) return 0;
	CDnGameTask *pTask = (CDnGameTask *)pRoom->GetTaskMng()->GetTask( "GameTask" );
	if( !pTask ) return 0;
	if( pTask->GetGameTaskType() != GameTaskType::DarkLair ) return 0;
	return ((CDnDLGameTask*)pTask)->GetRound();
}

int api_trigger_DLGetTotalRound( CDNGameRoom *pRoom )
{
	if( !pRoom ) return 0;
	CDnGameTask *pTask = (CDnGameTask *)pRoom->GetTaskMng()->GetTask( "GameTask" );
	if( !pTask ) return 0;
	if( pTask->GetGameTaskType() != GameTaskType::DarkLair ) return 0;
	return ((CDnDLGameTask*)pTask)->GetTotalRound();
}

int api_trigger_DLGetStartFloor( CDNGameRoom *pRoom )
{
	if( !pRoom ) 
		return 0;
	CDnGameTask *pTask = (CDnGameTask *)pRoom->GetTaskMng()->GetTask( "GameTask" );
	if( !pTask ) 
		return 0;
	if( pTask->GetGameTaskType() != GameTaskType::DarkLair ) 
		return 0;
	
	return static_cast<CDnDLGameTask*>(pTask)->GetStartFloor();
}

int api_trigger_DLRequestChallenge( CDNGameRoom *pRoom )
{
	if( !pRoom ) 
		return 0;
	CDnGameTask *pTask = (CDnGameTask *)pRoom->GetTaskMng()->GetTask( "GameTask" );
	if( !pTask ) 
		return 0;
	if( pTask->GetGameTaskType() != GameTaskType::DarkLair ) 
		return 0;
	
	CDnDLGameTask* pDLGameTask = static_cast<CDnDLGameTask*>(pTask);

	pDLGameTask->ChangeDungeonClearState( CDnGameTask::DCS_DLRequestChallenge );
	return 1;
}

int api_trigger_DLResponseChallenge( CDNGameRoom *pRoom )
{
	if( !pRoom ) 
		return 0;
	CDnGameTask *pTask = (CDnGameTask *)pRoom->GetTaskMng()->GetTask( "GameTask" );
	if( !pTask ) 
		return 0;
	if( pTask->GetGameTaskType() != GameTaskType::DarkLair ) 
		return 0;

	CDnDLGameTask* pDLGameTask = static_cast<CDnDLGameTask*>(pTask);
	return pDLGameTask->GetChallengeResponse();
}

void api_trigger_DLSetStartRound( CDNGameRoom *pRoom, int iRound )
{
	if( !pRoom ) 
		return;
	CDnGameTask *pTask = (CDnGameTask *)pRoom->GetTaskMng()->GetTask( "GameTask" );
	if( !pTask ) 
		return;
	if( pTask->GetGameTaskType() != GameTaskType::DarkLair ) 
		return;

	CDnDLGameTask* pDLGameTask = static_cast<CDnDLGameTask*>(pTask);
	pDLGameTask->SetStartRound( iRound );

	return;
}

void api_trigger_EnableDungeonClearWarpQuestFlag(CDNGameRoom* pRoom)
{
	if( !pRoom ) return;
	CDnGameTask *pTask = (CDnGameTask *)pRoom->GetTaskMng()->GetTask( "GameTask" );
	if( !pTask ) return;

	pTask->EnableDungeonClearWarpQuestFlag();
}

bool api_trigger_IsEnableDungeonClearWarp(CDNGameRoom* pRoom)
{
	if( !pRoom ) return false;
	CDnGameTask *pTask = (CDnGameTask *)pRoom->GetTaskMng()->GetTask( "GameTask" );
	if( !pTask ) return false;

	return (pTask->GetDungeonClearState() == CDnGameTask::DCS_WarpStandBy);
}

bool api_trigger_IsExistNotAchieveMissionPartyActor( CDNGameRoom *pRoom, int nMissionArrayIndex )
{
	if( !pRoom ) return false;
	TMissionData *pData = g_pDataManager->GetMissionData(nMissionArrayIndex);
	if( !pData || !pData->bActivate ) return false;

	for( DWORD i=0; i<pRoom->GetUserCount(); i++ ) {
		CDNUserSession *pSession = pRoom->GetUserData(i);
		if( !pSession ) continue;

		if( GetBitFlag( pSession->GetMissionData()->MissionAchieve, nMissionArrayIndex ) == false ) return true;
	}
	return false;
}

int api_trigger_GetChangeActionPlayerActor( CDNGameRoom *pRoom, CEtTriggerElement *pElement )
{
	if(!pRoom) return -1;
	if( !pElement ) return -1;
	CEtTriggerObject *pObject = pElement->GetTriggerObject();
	if( !pObject ) return -1;

	return pObject->GetEventValueStore( "ChangeActionPlayer" );
}

char *api_trigger_GetActionToPlayerActor( CDNGameRoom *pRoom, CEtTriggerElement *pElement )
{
	if(!pRoom) return "";
	if( !pElement ) return "";
	CEtTriggerObject *pObject = pElement->GetTriggerObject();
	if( !pObject ) return "";

	int nSessionID = pObject->GetEventValueStore( "ChangeActionPlayer" );
	int nIndex;
	CDNGameRoom::PartyStruct *pStruct = pRoom->GetPartyDatabySessionID( nSessionID, nIndex );
	if( !pStruct ) return "";
	if( !pStruct->pSession ) return "";
	if( !pStruct->pSession->GetActorHandle() ) return "";
	return (char*)pStruct->pSession->GetActorHandle()->GetCurrentAction();

}

void api_trigger_EnablePropNoDamage( CDNGameRoom *pRoom, int nPropHandle, bool bEnable )
{
	if( !pRoom ) return;

	std::vector<CEtWorldProp *> VecProp;
	pRoom->GetWorld()->FindPropFromCreateUniqueID( nPropHandle, &VecProp );

	if( VecProp.empty() ) return;

	CDnWorldProp *pProp = static_cast<CDnWorldProp *>( VecProp[0] );
	if( !pProp ) return;
	switch( pProp->GetPropType() ) {
		case PTE_Broken:
		case PTE_BrokenDamage:
		case PTE_HitMoveDamageBroken:
		case PTE_BuffBroken:
		case PTE_ShooterBroken:
			((CDnWorldBrokenProp*)pProp)->EnableNoDamage( bEnable );
			break;
		default:
			return;
	}
}

int api_trigger_GetTriggerEventSignalEventAreaHandle( CDNGameRoom *pRoom, CEtTriggerElement *pElement )
{
	if(!pRoom) return -1;
	if( !pElement ) return -1;
	CEtTriggerObject *pObject = pElement->GetTriggerObject();
	if( !pObject ) return -1;

	return pObject->GetEventValueStore( "EventArea" );
}

int api_trigger_GetTriggerEventSignalActorHandle( CDNGameRoom *pRoom, CEtTriggerElement *pElement )
{
	if(!pRoom) return -1;
	if( !pElement ) return -1;
	CEtTriggerObject *pObject = pElement->GetTriggerObject();
	if( !pObject ) return -1;

	return pObject->GetEventValueStore( "ActorHandle" );
}

int api_trigger_GetTriggerEventSignalEventID( CDNGameRoom *pRoom, CEtTriggerElement *pElement )
{
	if(!pRoom) return -1;
	if( !pElement ) return -1;
	CEtTriggerObject *pObject = pElement->GetTriggerObject();
	if( !pObject ) return -1;

	return pObject->GetEventValueStore( "EventID" );
}

int api_trigger_GetTriggerEventSignalTeam( CDNGameRoom *pRoom, CEtTriggerElement *pElement )
{
	if(!pRoom) return -1;
	if( !pElement ) return -1;
	CEtTriggerObject *pObject = pElement->GetTriggerObject();
	if( !pObject ) return -1;

	int nSessionID = pObject->GetEventValueStore( "ActorHandle" );

	CDNUserSession * pUser = pRoom->GetUserSession(nSessionID);

	if( !pUser )
		return -1;

	return pUser->GetTeam();
}

void api_trigger_HoldPartyDice(CDNGameRoom* pRoom)
{
	if( !pRoom ) return;
	CDnPartyTask::GetInstance(pRoom).HoldSharingReversionItem();
}

void api_trigger_ReleasePartyDice(CDNGameRoom* pRoom)
{
	if( !pRoom ) return;
	CDnPartyTask::GetInstance(pRoom).ReleaseSharingReversionItem();
}

void api_trigger_UpdateNestTryCount(CDNGameRoom *pRoom, int iForceMapIndex/*=0*/ )
{
	if( !pRoom ) return;
	CDnGameTask *pTask = (CDnGameTask *)pRoom->GetTaskMng()->GetTask( "GameTask" );
	if( !pTask ) return;

	pTask->RequestNestDungeonClear( iForceMapIndex );
}

void api_trigger_UpdateUserNestTryCount(CDNGameRoom *pRoom, int nActorHandle )
{
	if( !pRoom ) return;
	CDnGameTask *pTask = (CDnGameTask *)pRoom->GetTaskMng()->GetTask( "GameTask" );
	if( !pTask ) return;

	DnActorHandle hActor = CDnActor::FindActorFromUniqueID( pRoom, nActorHandle );
	if( !hActor ) 
		return;

	pTask->RequestNestDungeonClear( hActor );
}

int api_trigger_GetMonsterClassID( CDNGameRoom *pRoom, int nActorHandle )
{
	if( !pRoom ) return 0;
	CDnGameTask *pTask = (CDnGameTask *)pRoom->GetTaskMng()->GetTask( "GameTask" );
	if( !pTask ) return 0;

	DnActorHandle hActor = CDnActor::FindActorFromUniqueID( pRoom, nActorHandle );
	if( !hActor ) return 0;
	if( !hActor->IsMonsterActor() ) return 0;
	return ((CDnMonsterActor*)hActor.GetPointer())->GetMonsterClassID();
}

void api_trigger_EnableDungeonClearWarpAlarm( CDNGameRoom *pRoom, bool bEnable )
{
	if( !pRoom ) return;
	CDnGameTask *pTask = (CDnGameTask *)pRoom->GetTaskMng()->GetTask( "GameTask" );
	if( !pTask ) return;

	pTask->RequestEnableDungeonClearWarpAlarm( bEnable );
}

void api_trigger_SetHPFromEventArea( CDNGameRoom *pRoom, int nEventAreaHandle, int nValue )
{
	if( !pRoom ) return;
	CDnGameTask *pTask = (CDnGameTask *)pRoom->GetTaskMng()->GetTask( "GameTask" );
	if( !pTask ) return;

	DNVector(DnActorHandle) hVecList;
	pTask->GetBirthAreaLiveActors( nEventAreaHandle, hVecList );

	for( DWORD i=0; i<hVecList.size(); i++ ) {
		DnActorHandle hActor = hVecList[i];
		if( !hActor ) continue;

		hActor->CmdRefreshHPSP( nValue, hActor->GetSP() );
	}
}

void api_trigger_SetHP( CDNGameRoom *pRoom, int nActorHandle, int nValue )
{
	if( !pRoom ) return;

	DnActorHandle hActor = CDnActor::FindActorFromUniqueID( pRoom, nActorHandle );
	if( !hActor ) return;

	hActor->CmdRefreshHPSP( nValue, hActor->GetSP() );
}

void api_trigger_SetHPRatioFromEventArea( CDNGameRoom *pRoom, int nEventAreaHandle, float fValue )
{
	if( !pRoom ) return;
	CDnGameTask *pTask = (CDnGameTask *)pRoom->GetTaskMng()->GetTask( "GameTask" );
	if( !pTask ) return;

	DNVector(DnActorHandle) hVecList;
	pTask->GetBirthAreaLiveActors( nEventAreaHandle, hVecList );

	for( DWORD i=0; i<hVecList.size(); i++ ) {
		DnActorHandle hActor = hVecList[i];
		if( !hActor ) continue;

		INT64 nResult = (INT64)( hActor->GetMaxHP() * fValue );
		hActor->CmdRefreshHPSP( nResult, hActor->GetSP() );
//		printf("[RLKT_DEBUG][%s][ActorID: %d] CurHP: %lld ResultHP: %lld modValue: %.6f SP: %d \n", __FUNCTION__, hActor->GetClassID(), hActor->GetMaxHP(), nResult, fValue, hActor->GetSP());
	}
}

void api_trigger_SetHPRatio( CDNGameRoom *pRoom, int nActorHandle, float fValue )
{
	if( !pRoom ) return;

	DnActorHandle hActor = CDnActor::FindActorFromUniqueID( pRoom, nActorHandle );
	if( !hActor ) return;

	INT64 nResult = (INT64)( hActor->GetMaxHP() * fValue );
	hActor->CmdRefreshHPSP( nResult, hActor->GetSP() );
}

void api_trigger_AddHPAbsoluteFromEventArea( CDNGameRoom *pRoom, int nEventAreaHandle, int nValue )
{
	if( !pRoom ) return;
	CDnGameTask *pTask = (CDnGameTask *)pRoom->GetTaskMng()->GetTask( "GameTask" );
	if( !pTask ) return;

	DNVector(DnActorHandle) hVecList;
	pTask->GetBirthAreaLiveActors( nEventAreaHandle, hVecList );

	for( DWORD i=0; i<hVecList.size(); i++ ) {
		DnActorHandle hActor = hVecList[i];
		if( !hActor ) continue;

		INT64 nResult = hActor->GetHP() + nValue;
		if( nResult > hActor->GetMaxHP() ) nResult = hActor->GetMaxHP();
		if( nResult < 1 ) nResult = 1;
		hActor->CmdRefreshHPSP( nResult, hActor->GetSP() );
	}
}

void api_trigger_AddHPRatioFromEventArea( CDNGameRoom *pRoom, int nEventAreaHandle, float fValue )
{
	if( !pRoom ) return;
	CDnGameTask *pTask = (CDnGameTask *)pRoom->GetTaskMng()->GetTask( "GameTask" );
	if( !pTask ) return;

	DNVector(DnActorHandle) hVecList;
	pTask->GetBirthAreaLiveActors( nEventAreaHandle, hVecList );

	for( DWORD i=0; i<hVecList.size(); i++ ) {
		DnActorHandle hActor = hVecList[i];
		if( !hActor ) continue;

		INT64 nResult = hActor->GetHP() + (INT64)( hActor->GetMaxHP() * fValue );
		if( nResult > hActor->GetMaxHP() ) nResult = hActor->GetMaxHP();
		if( nResult < 1 ) nResult = 1;
		hActor->CmdRefreshHPSP( nResult, hActor->GetSP() );
	}
}

void api_trigger_AddHPAbsolute( CDNGameRoom *pRoom, int nActorHandle, int nValue )
{
	if( !pRoom ) return;

	DnActorHandle hActor = CDnActor::FindActorFromUniqueID( pRoom, nActorHandle );
	if( !hActor ) return;

	INT64 nResult = hActor->GetHP() + nValue;
	if( nResult > hActor->GetMaxHP() ) nResult = hActor->GetMaxHP();
	if( nResult < 1 ) nResult = 1;
	hActor->CmdRefreshHPSP( nResult, hActor->GetSP() );
}

void api_trigger_AddHPRatio( CDNGameRoom *pRoom, int nActorHandle, float fValue )
{
	if( !pRoom ) return;

	DnActorHandle hActor = CDnActor::FindActorFromUniqueID( pRoom, nActorHandle );
	if( !hActor ) return;

	INT64 nResult = hActor->GetHP() + (INT64)( hActor->GetMaxHP() * fValue );
	if( nResult > hActor->GetMaxHP() ) nResult = hActor->GetMaxHP();
	if( nResult < 1 ) nResult = 1;
	hActor->CmdRefreshHPSP( nResult, hActor->GetSP() );
}

int api_trigger_GetTeamFromEventArea( CDNGameRoom *pRoom, int nEventAreaHandle )
{
	if( !pRoom ) return -1;
	CDnGameTask *pTask = (CDnGameTask *)pRoom->GetTaskMng()->GetTask( "GameTask" );
	if( !pTask ) return -1;

	DNVector(DnActorHandle) hVecList;
	pTask->GetBirthAreaLiveActors( nEventAreaHandle, hVecList );

	for( DWORD i=0; i<hVecList.size(); i++ ) {
		if( !hVecList[i] ) continue;
		return hVecList[i]->GetTeam();
	}
	return -1;
}

int api_trigger_GetTeam( CDNGameRoom *pRoom, int nActorHandle )
{
	if( !pRoom ) return -1;

	DnActorHandle hActor = CDnActor::FindActorFromUniqueID( pRoom, nActorHandle );
	if( !hActor ) return -1;

	return hActor->GetTeam();
}

void api_trigger_IgnoreAggroFromEventArea( CDNGameRoom *pRoom, int nEventAreaHandle, int nValue )
{
	if( !pRoom ) return;
	CDnGameTask *pTask = (CDnGameTask *)pRoom->GetTaskMng()->GetTask( "GameTask" );
	if( !pTask ) return;

	DNVector(DnActorHandle) hVecList;
	pTask->GetBirthAreaLiveActors( nEventAreaHandle, hVecList );

	for( DWORD i=0; i<hVecList.size(); i++ ) {
		if( !hVecList[i] ) continue;
		if( !hVecList[i]->GetAggroSystem() ) continue;
		hVecList[i]->GetAggroSystem()->SetIgnoreFlag( (nValue == 0) ? false : true );
	}
}

int api_trigger_GetPropDurability( CDNGameRoom *pRoom, int nPropHandle )
{
	if(!pRoom) return 0;

	std::vector<CEtWorldProp *> VecProp;
	pRoom->GetWorld()->FindPropFromCreateUniqueID( nPropHandle, &VecProp );

	if( VecProp.empty() ) return 0;

	CDnWorldProp *pProp = static_cast<CDnWorldProp *>( VecProp[0] );
	if( !pProp ) return 0;

	int nPropType = pProp->GetPropType();
	switch( nPropType ) {
		case PTE_Broken:
		case PTE_BrokenDamage:
		case PTE_BuffBroken:
		case PTE_HitMoveDamageBroken:
		case PTE_ShooterBroken:
			return ((CDnWorldBrokenProp*)pProp)->GetDurability();
		default:
			break;
	}
	return 0;
}

void api_trigger_NotUseManaSkill( CDNGameRoom *pRoom, int nActorHandle )
{
	DnActorHandle hActor = CDnActor::FindActorFromUniqueID( pRoom, nActorHandle );

	if( !hActor ) return;

	if( hActor->IsPlayerActor() )
		static_cast<CDnPlayerActor*>(hActor.GetPointer())->OnBattleToggle( false );
	hActor->CmdAddStateEffect( NULL, STATE_BLOW::BLOW_078, -1, "", true );
}

bool api_trigger_NoDuplicateBuffFromEventArea( CDNGameRoom *pRoom, int nActorHandle, int nSkillID, int nSkillLevel, int nDelay)
{
	PROFILE_TICK_TEST_BLOCK_START( "api_trigger_NoDuplicateBuffFromEventArea" );

	if( !pRoom ) return false;

	CDnSkill::SkillInfo sSkillInfo;
	std::vector<CDnSkill::StateEffectStruct> vecSkillEffectList;
	CDnSkill::CreateSkillInfo( nSkillID, nSkillLevel, sSkillInfo, vecSkillEffectList );

	sSkillInfo.bIgnoreImmune = true;

	DnActorHandle hActor = CDnActor::FindActorFromUniqueID( pRoom, nActorHandle );
	DnActorHandle hVehicle = CDnActor::FindActorFromUniqueID( pRoom, nActorHandle + 1);
	if( !hActor ) return false;

	sSkillInfo.hSkillUser = hActor;

	hActor->ClearTriggerBuff();

	if( hVehicle )
		hVehicle->ClearTriggerBuff();

	for( int itr = 0; itr < (int)vecSkillEffectList.size(); ++itr )
	{
		int nID = hActor->CmdAddStateEffect( &sSkillInfo, (STATE_BLOW::emBLOW_INDEX)vecSkillEffectList[itr].nID, nDelay, 
			vecSkillEffectList[itr].szValue.c_str(), false, true );

		hActor->AddTriggerBuff( nID );

		if( hActor->IsPlayerActor() )
			static_cast<CDnPlayerActor*>(hActor.GetPointer())->SendRemoveStateEffectGraphic( (STATE_BLOW::emBLOW_INDEX)vecSkillEffectList[itr].nID );

		if( hVehicle )
		{
			nID = hVehicle->CmdAddStateEffect( &sSkillInfo, (STATE_BLOW::emBLOW_INDEX)vecSkillEffectList[itr].nID, nDelay, 
				vecSkillEffectList[itr].szValue.c_str(), false, true );

			hVehicle->AddTriggerBuff( nID );
		}
	}

	PROFILE_TICK_TEST_BLOCK_END();

	return true;
}

void api_trigger_BuffFromEventArea( CDNGameRoom *pRoom, int nActorHandle, int nSkillID, int nSkillLevel, int nDelay)
{
	PROFILE_TICK_TEST_BLOCK_START( "api_trigger_BuffFromEventArea" );

	if( !pRoom ) return;

	CDnSkill::SkillInfo sSkillInfo;
	std::vector<CDnSkill::StateEffectStruct> vecSkillEffectList;
	CDnSkill::CreateSkillInfo( nSkillID, nSkillLevel, sSkillInfo, vecSkillEffectList );

	sSkillInfo.bIgnoreImmune = true;

	DnActorHandle hActor = CDnActor::FindActorFromUniqueID( pRoom, nActorHandle );
	DnActorHandle hVehicle = CDnActor::FindActorFromUniqueID( pRoom, nActorHandle + 1);
	if( !hActor ) return;

	sSkillInfo.hSkillUser = hActor;

	for( int itr = 0; itr < (int)vecSkillEffectList.size(); ++itr )
	{
		hActor->CmdAddStateEffect( &sSkillInfo, (STATE_BLOW::emBLOW_INDEX)vecSkillEffectList[itr].nID, nDelay, 
			vecSkillEffectList[itr].szValue.c_str(), false, true );

		if( hActor->IsPlayerActor() )
			static_cast<CDnPlayerActor*>(hActor.GetPointer())->SendRemoveStateEffectGraphic( (STATE_BLOW::emBLOW_INDEX)vecSkillEffectList[itr].nID );

		if( hVehicle )
		{
			hVehicle->CmdAddStateEffect( &sSkillInfo, (STATE_BLOW::emBLOW_INDEX)vecSkillEffectList[itr].nID, nDelay, 
				vecSkillEffectList[itr].szValue.c_str(), false, true );
		}
	}

	PROFILE_TICK_TEST_BLOCK_END();
}

void api_trigger_ActorBuffFromEventArea( CDNGameRoom *pRoom, int nActorHandle, int nEventAreaHandle, int nSkillID, int nSkillLevel, int nDelay, bool bSummonInclude )
{
	PROFILE_TICK_TEST_BLOCK_START( "api_trigger_ActorBuffFromEventArea" );

	if( NULL == pRoom )
		return;

	CDnSkill::SkillInfo sSkillInfo;
	std::vector<CDnSkill::StateEffectStruct> vecSkillEffectList;
	CDnSkill::CreateSkillInfo( nSkillID, nSkillLevel, sSkillInfo, vecSkillEffectList );

	sSkillInfo.bIgnoreImmune = true;

	DnActorHandle hActor = CDnActor::FindActorFromUniqueID( pRoom, nActorHandle );
	if( NULL == hActor )
		return;

	const std::list<DnMonsterActorHandle> & listSummmonMonster = hActor->GetSummonedMonsterList();

	for( int itr = 0; itr < (int)vecSkillEffectList.size(); ++itr )
	{
		sSkillInfo.hSkillUser = hActor;

		hActor->CmdAddStateEffect( &sSkillInfo, (STATE_BLOW::emBLOW_INDEX)vecSkillEffectList[itr].nID, nDelay, 
			vecSkillEffectList[itr].szValue.c_str(), false, true );

		if( bSummonInclude && false == listSummmonMonster.empty() )
		{
			std::list<DnMonsterActorHandle>::const_iterator iter = listSummmonMonster.begin();
			for( iter; iter != listSummmonMonster.end(); ++iter )
			{
				DnMonsterActorHandle hMonster = (*iter);

				if( NULL == hMonster && false == api_trigger_IsInsidePlayerToEventArea( pRoom, hMonster->GetClassID(), nEventAreaHandle) )
					continue;

				sSkillInfo.hSkillUser = hMonster;

				hMonster->CmdAddStateEffect( &sSkillInfo, (STATE_BLOW::emBLOW_INDEX)vecSkillEffectList[itr].nID, nDelay, 
					vecSkillEffectList[itr].szValue.c_str(), false, true );
			}
		}
	}

	PROFILE_TICK_TEST_BLOCK_END();
}

void api_trigger_Start_DragonNest( CDNGameRoom* pRoom, int nType, int nRebirthCount )
{
	if( !pRoom )
		return;

	// 서버쪽에도 설정이 되어야 할듯... [2011/01/12 semozz]
	CDnWorld *pWorld = pRoom->GetWorld();
	if (pWorld)
		pWorld->SetDragonNestType((eDragonNestType)nType);

	for( UINT i=0 ; i<pRoom->GetUserCount() ; ++i )
	{
		CDNUserSession* pUser = pRoom->GetUserData(i);
		if( pUser )
		{
			// 씨드래곤네스트에서 부활 카운트 수는 이 정보값으로 변경한다 [2011/01/12 semozz]
			CDNGameRoom::PartyStruct* pParty = pRoom->GetPartyData(pUser);
			if (pParty)
			{
				pParty->nUsableRebirthCoin = nRebirthCount;
			}
			pUser->SendStartDragonNest( (eDragonNestType)nType, nRebirthCount );
		}
	}
}

void api_trigger_SetUsableRebirthCoin( CDNGameRoom* pRoom, int nActorHandle, int nUsableRebirthCoin )
{
	if( !pRoom )
		return;

	CDNUserSession* pSession = pRoom->GetUserSession(nActorHandle);
	if( pSession == NULL )
		return;

	CDNGameRoom::PartyStruct* pStruct = pRoom->GetPartyData( pSession );
	if( pStruct == NULL )
		return;

	pStruct->nUsableRebirthCoin = nUsableRebirthCoin;
	
	pSession->SendRebirthCoin( ERROR_NONE, nUsableRebirthCoin, _REBIRTH_SELF, pSession->GetSessionID() );
}

int api_trigger_GetMonsterHPPercent_FromEventArea( CDNGameRoom* pRoom, int nEventAreaHandle )
{
	if( !pRoom )
		return -1;

	CDnGameTask* pGameTask = static_cast<CDnGameTask*>(pRoom->GetTaskMng()->GetTask("GameTask") );
	if( !pGameTask )
		return -1;

	return pGameTask->GetInsideAreaMonsterHPPercent( nEventAreaHandle );
}


void api_trigger_UseSkillFromEventArea( CDNGameRoom *pRoom, int nEventAreaHandle, int nSkillID )
{
	if( !pRoom ) return;
	CDnGameTask *pTask = (CDnGameTask *)pRoom->GetTaskMng()->GetTask( "GameTask" );
	if( !pTask ) return;

	DNVector(DnActorHandle) hVecList;
	pTask->GetBirthAreaLiveActors( nEventAreaHandle, hVecList );

	for( DWORD i=0; i<hVecList.size(); i++ ) {
		if( !hVecList[i] ) continue;
		if( hVecList[i]->IsDie() ) continue;
		if( !hVecList[i]->IsMonsterActor() ) continue;
		CDnMonsterActor *pMonster = (CDnMonsterActor*)hVecList[i].GetPointer();
		
		if (pMonster)
		{
			DnSkillHandle hSkill = pMonster->FindSkill(nSkillID);
			if (hSkill)
				hSkill->SetIgnoreImmnune(true);

			pMonster->UseSkill( nSkillID );
		}
	}
}

void api_trigger_UseSkill( CDNGameRoom *pRoom, int nActorHandle, int nSkillID )
{
	if( !pRoom ) return;

	DnActorHandle hActor = CDnActor::FindActorFromUniqueID( pRoom, nActorHandle );
	if( !hActor ) return;
	if( hActor->IsDie() ) return;
	if( !hActor->IsMonsterActor() ) return;

	CDnMonsterActor *pMonster = (CDnMonsterActor*)hActor.GetPointer();
	pMonster->UseSkill( nSkillID );
}

void api_trigger_AllUser_AddVehicleItem(CDNGameRoom *pRoom, char* pszStr, bool bDBSave )
{
	if( pszStr == NULL )
		return;

	std::string					strString(pszStr);
	std::vector<std::string>	vSplit;
	std::vector<int>			vItemID;

	boost::algorithm::split( vSplit, strString, boost::algorithm::is_any_of(",") );

	for( UINT i=0 ; i<vSplit.size() ; ++i )
	{
		vItemID.push_back( atoi(vSplit[i].c_str()) );
		const TItemData* pItemData = g_pDataManager->GetItemData( vItemID[i] );
		if( pItemData == NULL )
		{
			_ASSERT(0);
			return;
		}
		
		if( pItemData->nType != ITEMTYPE_VEHICLE )
		{
			_ASSERT(0);
			return;
		}
	}

	if( vItemID.empty() )
		return;

	::srand( timeGetTime() );
	
	for( DWORD i=0; i<pRoom->GetUserCount(); i++ ) 
	{
		CDNUserSession *pSession = pRoom->GetUserData(i);
		if( !pSession ) 
			continue;		
#if defined(PRE_ADD_RACING_MODE)
		if( pRoom->GetPvPGameMode() && !pRoom->GetPvPGameMode()->bIsPlayingUser(pSession->GetActorHandle()) )
			continue;		
#endif

		int iItemID = vItemID[::rand()%vItemID.size()];
		pSession->GetItem()->CreateCashInvenItem( iItemID, 1, bDBSave ? DBDNWorldDef::AddMaterializedItem::Trigger : 0 );
	}
}

void api_trigger_PlayerNameShowBlindCaption( CDNGameRoom *pRoom, int nUIStringIndex, int nDelay, int nActorHandle )
{
	DnActorHandle Actor = CDnActor::FindActorFromUniqueID( pRoom, nActorHandle );

	if( !Actor )
		return;

	WCHAR wszString[512];
#if defined(PRE_ADD_MULTILANGUAGE)
	//이건 먼지 몰겠다
	CDNUserSession * pUser;
	for( DWORD itr = 0; itr < pRoom->GetUserCount(); ++itr )
	{
		memset(wszString, 0, sizeof(wszString));
		pUser = pRoom->GetUserData(itr);
		if( pUser )
		{
			swprintf_s( wszString, _countof(wszString), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nUIStringIndex, pUser->m_eSelectedLanguage ), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, Actor->GetNameUIStringIndex(), pUser->m_eSelectedLanguage ) );
			pUser->SendChat( eChatType::CHATTYPE_BLIND, _countof(wszString), L"", wszString );
		}
	}
#else		//#if defined(PRE_ADD_MULTILANGUAGE)
	swprintf_s( wszString, _countof(wszString), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nUIStringIndex ), Actor->GetName() );
	CDNUserSession * pUser;
	for( DWORD itr = 0; itr < pRoom->GetUserCount(); ++itr )
	{
		pUser = pRoom->GetUserData(itr);
		if( pUser )
			pUser->SendChat( eChatType::CHATTYPE_BLIND, _countof(wszString), L"", wszString );
	}
#endif		//#if defined(PRE_ADD_MULTILANGUAGE)
}

void api_trigger_InvenToEquip( CDNGameRoom* pRoom, UINT nUserObjectID, int iItemID, bool bShootMode, bool bBackup/*=false*/ )
{
	CDNUserSession * pUser = pRoom->GetUserSession(nUserObjectID);
	if ( !pUser ) 
		return;

	const TItemData* pItemData = g_pDataManager->GetItemData( iItemID );
	if( pItemData == NULL )
		return;
	// 트리거로 캐시템은 못하게 한다
	if( pItemData->IsCash )
		return;

	int iInvenIndex = pUser->GetItem()->FindInventorySlot( iItemID, 1 );
	if( iInvenIndex < 0 )
		return;
	int iDestIndex = pUser->GetItem()->GetEquipSlotIndex( iItemID );
	if( iDestIndex < 0 )
		return;

	const TItem* pItem = pUser->GetItem()->GetInventory( iInvenIndex );
	if( pItem == NULL )
		return;

	CSMoveItem TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.cMoveType			= MoveType_InvenToEquip;
	TxPacket.cSrcIndex			= static_cast<BYTE>(iInvenIndex);
	TxPacket.biSrcItemSerial	= pItem->nSerial;
	TxPacket.cDestIndex			= static_cast<BYTE>(iDestIndex);
	TxPacket.wCount				= 1;

	if( bBackup == true )
	{
		const TItem* pOrgEquip = pUser->GetItem()->GetEquip( TxPacket.cDestIndex );

		// 이미 백업설정이 되었으면
		if( pUser->m_BackupEquipInfo.first == 0 )
			pUser->m_BackupEquipInfo = std::make_pair( pOrgEquip?pOrgEquip->nSerial:0, pItem->nSerial );
		else
			pUser->m_BackupEquipInfo.second = pItem->nSerial;
	}
	else
	{
		pUser->m_BackupEquipInfo = std::make_pair(0,0);
	}


	DnActorHandle hActor = pUser->GetActorHandle();
	if( hActor && hActor->IsPlayerActor() )
	{
		CDnPlayerActor *pPlayer = static_cast<CDnPlayerActor*>(hActor.GetPointer());
		if( pPlayer )
		{
			pPlayer->SetWeaponViewOrder( 0, false );

			pUser->GetItem()->SetMoveItemCheckGameMode( false );
			pRoom->OnDispatchMessage( pUser, CS_ITEM, eItem::CS_MOVEITEM, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket), 0 );
			pUser->GetItem()->SetMoveItemCheckGameMode( true );

			// ShootMode
			const TItem* pEquip = pUser->GetItem()->GetEquip( TxPacket.cDestIndex );
			if( pEquip && pEquip->nItemID == iItemID )
			{
				pPlayer->CmdShootMode( bShootMode );
			}
		}
	}
}

void api_trigger_UnRideVehicle( CDNGameRoom* pRoom, UINT nUserObjectID )
{
	if( !pRoom ) 
		return;

	CDNUserSession* pSession = pRoom->GetUserSession(nUserObjectID);
	if( pSession == NULL )
		return;

	TVehicle* pVehicle = pSession->GetItem()->GetVehicleEquip();
	if( pVehicle == NULL )
		return;

	CSMoveCashItem TxPacket;
	memset( &TxPacket, 0, sizeof(TxPacket) );

	TxPacket.cMoveType			= MoveType_VehicleBodyToVehicleInven;
	TxPacket.cCashEquipIndex	= Vehicle::Slot::Body;
	TxPacket.biEquipItemSerial	= pVehicle->Vehicle[Vehicle::Slot::Body].nSerial;
	TxPacket.biCashInvenSerial	= 0;
	TxPacket.wCount				= 1;

	pRoom->OnDispatchMessage( pSession, CS_ITEM, eItem::CS_MOVECASHITEM, reinterpret_cast<char*>(&TxPacket), sizeof(TxPacket), 0 );
}

void api_trigger_ChangeMyBGM( CDNGameRoom * pRoom, int nActorHandle, int nBGM, int nFadeDelta )
{
	CDNUserSession* pSession = pRoom->GetUserSession(nActorHandle);
	if( pSession == NULL )
		return;

	pSession->SendTriggerChangeMyBGM(nBGM, nFadeDelta);
}

void api_trigger_FileTableBGMOff( CDNGameRoom * pRoom, int nActorHandle )
{
	if( !pRoom )
		return;

	CDNUserSession* pSession = pRoom->GetUserSession(nActorHandle);
	if( pSession == NULL )
		return;

	pSession->SendTriggerFileTableBGMOff();
}

void api_trigger_RadioMsg( CDNGameRoom * pRoom, int nActorHandle, int nRadioID )
{
	CDNUserSession* pSession = pRoom->GetUserSession(nActorHandle);
	if( pSession == NULL )
		return;

	pSession->SendUseRadio(nActorHandle, nRadioID);
}

void api_trigger_RadioImage( CDNGameRoom * pRoom, int nActorHandle, int nFileID, DWORD nTime )
{
	CDNUserSession* pSession = pRoom->GetUserSession(nActorHandle);
	if( pSession == NULL )
		return;

	for( DWORD i=0; i<pRoom->GetUserCount(); i++ ) 
	{
		pSession = pRoom->GetUserData(i);

		if( pSession )
			pSession->SendTriggerRadioImage( nActorHandle, nFileID, nTime );
	}
}

int api_trigger_GetPvPGameModePlayTime( CDNGameRoom * pRoom )
{
	if( !pRoom )
		return 0;

	if( !pRoom->GetPvPGameMode() )
		return 0;

	return static_cast<int>(pRoom->GetPvPGameMode()->GetSelectPlayTime() - pRoom->GetPvPGameMode()->GetTimeOver());
}

void api_trigger_TargetActorShowNpc( CDNGameRoom * pRoom, int nActorHandle, int nEventAreaHandle, bool bShow )
{
	if( !pRoom ) 
		return;

	CDnGameTask *pTask = (CDnGameTask *)pRoom->GetTaskMng()->GetTask( "GameTask" );
	if( !pTask ) 
		return;

	CDNUserSession * pSession = pRoom->GetUserSession( nActorHandle );
	if( !pSession ) 
		return;

	DNVector(DnActorHandle) vecActor;
	pTask->GetBirthAreaLiveActors( nEventAreaHandle, vecActor );
	if( vecActor.empty() ) 
		return;

	if (bShow)
	{
		std::vector<CEtWorldEventArea *> VecArea;
		pRoom->GetWorld()->FindEventAreaFromCreateUniqueID( nEventAreaHandle, &VecArea );
		if( VecArea.empty() ) 
			return;

		EtVector3 vPos;
		CEtWorldEventArea* pArea = VecArea[0];

		for ( int itr = 0 ; itr < (int)vecActor.size() ; ++itr )
		{
			DnActorHandle hActor = vecActor[itr];
			if( !hActor ) 
				continue;

			if (!hActor->IsNpcActor())
				continue;

			hActor->Show(true);
			pTask->SendActorShow(pSession, hActor);
		}
	}
	else
	{
		for ( int itr = 0 ; itr < (int)vecActor.size() ; ++itr )
		{
			DnActorHandle hActor = vecActor[itr];
			if( !hActor ) 
				continue;

			if (!hActor->IsNpcActor())
				continue;

			hActor->Show(false);
			pTask->SendActorHide(pSession, hActor);
		}
	}

}

void api_trigger_TargetActorShowProp( CDNGameRoom * pRoom, int nActorHandle, int nPropHandle, bool bShow )
{
	if(!pRoom) 
		return;

	CDnGameTask *pTask = (CDnGameTask *)pRoom->GetTaskMng()->GetTask( "GameTask" );
	if( !pTask ) 
		return;

	CDNUserSession * pSession = pRoom->GetUserSession( nActorHandle );
	if( !pSession ) 
		return;

	std::vector<CEtWorldProp *> pVecList;
	CDnWorld::GetInstance(pRoom).FindPropFromCreateUniqueID( nPropHandle, &pVecList );
	if( pVecList.empty() ) 
		return;


	for( DWORD i=0; i<pVecList.size(); i++ ) 
	{
		CDnWorldProp *pProp = (CDnWorldProp *)pVecList[i];
		if( !pProp || pProp->GetPropType() != PTE_Npc ) 
			continue;

		pProp->CmdShow (bShow, pSession);
	}
}

int  api_trigger_TriggerVariableTable( CDNGameRoom * pRoom, int nItemID )
{
	if( !pRoom ) return -1;

	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TTRIGGERVARIABLETABLE );

	if( !pSox ) return -1;

	return pSox->GetFieldFromLablePtr( nItemID, "_Value" )->GetInteger();
}

int  api_trigger_GetPvPGameMode( CDNGameRoom * pRoom )
{
	if( !pRoom ||
		!pRoom->GetPvPGameMode() ||
		!pRoom->GetPvPGameMode()->GetPvPGameModeTable() )
		return -1;

	return pRoom->GetPvPGameMode()->GetPvPGameModeTable()->uiGameMode;
}

void api_trigger_CmdKeepOperationProp( CDNGameRoom * pRoom, int nPropHandle, int nActorHandle, int nOperationTime, int nStringID, const char *szAction )
{
	if( NULL == pRoom )
		return;

	CDnGameTask *pTask = (CDnGameTask *)pRoom->GetTaskMng()->GetTask( "GameTask" );
	if( NULL == pTask )
		return;

	CDNUserSession * pSession = pRoom->GetUserSession( nActorHandle );
	if( NULL == pSession )
		return;

	std::vector<CEtWorldProp *> pVecList;
	CDnWorld::GetInstance(pRoom).FindPropFromCreateUniqueID( nPropHandle, &pVecList );
	if( pVecList.empty() ) return;

	for( DWORD i=0; i<pVecList.size(); i++ ) 
	{
		CDnWorldKeepOperationProp *pProp = (CDnWorldKeepOperationProp *)pVecList[i];
		if( !pProp || pProp->GetPropType() != PTE_KeepOperation ) continue;

		// 여기서 프랍 실행 시작 알려주시면 됩니다.
		pProp->OnClickKeepProp( pSession, nOperationTime, nStringID, szAction );
	}

	return;
}

void api_trigger_ActivateMission( CDNGameRoom * pRoom, int nActorHandle, int nMissionID)
{
	if( NULL == pRoom )
		return;

	CDnGameTask *pTask = (CDnGameTask *)pRoom->GetTaskMng()->GetTask( "GameTask" );
	if( NULL == pTask )
		return;

	CDNUserSession * pSession = pRoom->GetUserSession( nActorHandle );
	if( NULL == pSession )
		return;

	pSession->GetEventSystem()->OnEvent(EventSystem::OnTrigger, 1, EventSystem::MissionID, nMissionID);
}

void api_trigger_SetDungeonClearRound( CDNGameRoom * pRoom, int iRound )
{
#if defined( PRE_ADD_36870 )
	if( pRoom == NULL )
		return;
	pRoom->SetDungeonClearRound( iRound );
#endif // #if defined( PRE_ADD_36870 )
	return;
}

bool api_trigger_BreakIntoUserCheck( CDNGameRoom* pRoom )
{
	if( !pRoom )
		return false;
	return !pRoom->bIsExistBreakIntoUser();
}

bool api_trigger_DungeonStartTimeAttack( CDNGameRoom* pRoom, int iMin, int iSec )
{
	if( !pRoom )
		return false;
	CDnGameTask* pGameTask = pRoom->GetGameTask();
	if( !pGameTask )
		return false;

	pGameTask->StartTimeAttack( iMin, iSec );
	pGameTask->SyncTimeAttack();
	return true;
}

int api_trigger_DungeonCheckTimeAttack( CDNGameRoom* pRoom )
{
	if( !pRoom )
		return 0;
	CDnGameTask* pGameTask = pRoom->GetGameTask();
	if( !pGameTask )
		return 0;
	
	return pGameTask->GetRemainTimeAttackSec();
}

void api_trigger_DungeonStopTimeAttack( CDNGameRoom* pRoom )
{
	if( !pRoom )
		return;
	CDnGameTask* pGameTask = pRoom->GetGameTask();
	if( !pGameTask )
		return;
	
	pGameTask->StopTimeAttack();
}

void api_trigger_RequestDungeonFail( CDNGameRoom* pRoom, bool bTimeOut/*=false*/ )
{
	if( !pRoom )
		return;

	CDnGameTask* pGameTask = pRoom->GetGameTask();
	if( pGameTask == NULL )
		return;

	pGameTask->RequestDungeonFailed(true,bTimeOut);
}

void api_trigger_Special_RebirthItem( CDNGameRoom* pRoom, int nType, int nITemID, int nRebirthItemCount )
{
	if( !pRoom )
		return;

	CDnWorld *pWorld = pRoom->GetWorld();
	if (pWorld)
		pWorld->SetDragonNestType((eDragonNestType)nType);

	// 부활 아이템과 횟수 지정
	for( UINT i=0 ; i<pRoom->GetUserCount() ; ++i )
	{
		CDNUserSession* pUser = pRoom->GetUserData(i);
		if( pUser )
		{
			
			CDNGameRoom::PartyStruct* pParty = pRoom->GetPartyData(pUser);
			int nRebirthCount = 0;
			if (pParty)
			{
				nRebirthCount = pParty->nUsableRebirthCoin;
				pParty->ReverseItemList.push_back( nITemID );				
				pParty->nUsableRebirthItemCoin += nRebirthItemCount;
			}

			pUser->SendStartDragonNest( (eDragonNestType)nType, nRebirthCount );
			pUser->SendSpecialRebirthItem( nITemID, pParty->nUsableRebirthItemCoin );
		}
	}
}

void api_trigger_GameWarpDungeonClearToLeader(CDNGameRoom* pRoom, bool bIgnoreClear/*=false*/)
{
	if (!pRoom)
		return;

	CDnGameTask* pGameTask = pRoom->GetGameTask();
	if (!pGameTask)
		return;

	if (!bIgnoreClear)
	{		
		pRoom->AddDungeonPlayTime( timeGetTime() - pGameTask->GetStageCreateTime() );
		for( DWORD i=0; i<pRoom->GetUserCount(); i++ ) 
		{
			// StageClear 로그
			CDNGameRoom::PartyStruct* pParty = pRoom->GetPartyData(i);
			if (pParty && pParty->pSession)
			{
				pParty->pSession->SetLastStageClearRank(0);
				pParty->pSession->CheckAndSendStageClearLog(true);
			}
		}		
	}

	pGameTask->EnableGameWarpDungeonClearToLeader();
	pGameTask->SendGameWarpDungeonClearToLeader();
}

void api_trigger_console_WriteLine( char * szOutputString, const char * s0, const char * s1, const char * s2, const char * s3, const char * s4, const char * s5,
								   const char * s6, const char * s7, const char * s8, const char * s9 )
{
#if defined(_WORK)
	if( NULL == szOutputString || NULL == s0 || NULL == s1 || NULL == s2 || NULL == s3 || NULL == s4 || NULL == s5 || NULL == s6 || NULL == s7 || NULL == s8 || NULL == s9 )
		return;

	char szPrintString[512] = {0,};

	sprintf_s( szPrintString, _countof( szPrintString ), szOutputString, s0, s1, s2, s3, s4, s5, s6, s7, s8, s9 );

	g_Log.Log( LogType::_NORMAL, szPrintString );
#endif
}

int api_trigger_GetPartyCountbyRoom( CDNGameRoom* pRoom )
{
#if defined( PRE_FIX_49129 )
	if (!pRoom)
		return 0;
	return pRoom->GetFirstUserCount();
#else
	return 0;
#endif
}
int api_trigger_GetPartyActorbyRoom( CDNGameRoom* pRoom, int nFirstPartyIndex )
{
#if defined( PRE_FIX_49129 )
	if (!pRoom)
		return 0;

	if( nFirstPartyIndex >= (int)(pRoom->GetFirstUserCount()) )
		return 0;

	CDNGameRoom::PartyFirstStruct* PartyFirst = pRoom->GetFirstPartyData((DWORD)nFirstPartyIndex);
	if( !PartyFirst )
		return 0;

	CDNUserSession * pSession = pRoom->GetUserSessionByCharDBID( PartyFirst->biCharacterDBID );
	if( NULL == pSession )
		return 0;
	return pSession->GetSessionID();
#else
	return 0;
#endif
}

// 모든 유저의 출발 시간 기록..
int api_trigger_GetRacingStartTime( CDNGameRoom * pRoom )
{
	if( !pRoom )
		return 0;

	if( !pRoom->GetPvPGameMode()->bIsRacingMode() )
		return 0;	
#if defined(PRE_ADD_RACING_MODE)
	pRoom->GetPvPGameMode()->SetRacingStart();
#endif

	return 0;
}
int api_trigger_GetRacingGoalTime( CDNGameRoom * pRoom, const int nActorHandle )
{
	if( !pRoom )
		return 0;

	if( !pRoom->GetPvPGameMode()->bIsRacingMode() )
		return 0;	

	CDNUserSession * pSession = pRoom->GetUserSession( (UINT)nActorHandle );
	if( !pSession )
		return 0;
#if defined(PRE_ADD_RACING_MODE)
	pRoom->GetPvPGameMode()->SetRacingEnd( (UINT)nActorHandle );
#endif

	return 0;
}

int api_trigger_GetCurrentWeather( CDNGameRoom* pRoom )
{
#if defined (PRE_ADD_DESERTDRAGON)
	if (!pRoom || !pRoom->GetWorld())
		return -1;

	return pRoom->GetWorld()->GetCurrentWeather();
#else
	return -1;
#endif
}

void api_trigger_StartDungeonGateTime( CDNGameRoom* pRoom, int nGateID )
{
#if defined( PRE_ADD_58761 )
	if (!pRoom)
		return;

	return pRoom->StartDungeonGateTime( nGateID );
#endif
}
void api_trigger_EndDungeonGateTime( CDNGameRoom* pRoom, int nGateID )
{
#if defined( PRE_ADD_58761 )
	if (!pRoom)
		return;

	return pRoom->EndDungeonGateTime( nGateID, true );
#endif
}

int api_trigger_StageRemainClearCount( CDNGameRoom* pRoom, UINT nUserObjectID, int nMapIndex )
{
#if defined(PRE_ADD_62072)
	if (!pRoom)
		return 0;

	CDNUserSession * pUser = pRoom->GetUserSession(nUserObjectID);
	if ( !pUser ) 
		return 0;

	int nMapID = nMapIndex;
	const TMapInfo *pMapInfo = g_pDataManager->GetMapInfo(nMapID);
	if(!pMapInfo) return 0;

	int nExpandNestClearCount = 0;
	int nPCBangExpandNestClearCount = 0;

	if ( pMapInfo->MapSubType == EWorldEnum::MapSubTypeNest && pMapInfo->bExpandable){
		nExpandNestClearCount = pUser->m_nExpandNestClearCount;
		nPCBangExpandNestClearCount = g_pDataManager->GetPCBangNestClearCount(pUser->GetPCBangGrade(), nMapID);		
	}
	int nRemainCount = (pMapInfo->nMaxClearCount + nExpandNestClearCount + nPCBangExpandNestClearCount) - pUser->GetNestClearTotalCount( nMapID );
	if( nRemainCount < 0 ) return 0;
	return nRemainCount;
#endif
	return 0;
}

void api_trigger_EnableRideVehicle( CDNGameRoom * pRoom, UINT nActorHandle, bool bEnableRide )
{
	if( !pRoom )
		return;

	CDNUserSession * pUser = pRoom->GetUserSession( nActorHandle );
	if ( !pUser ) 
		return;

	CDnPlayerActor * pPlayerActor = pUser->GetPlayerActor();
	if( !pPlayerActor )
		return;

	pPlayerActor->SetForceEnableRide( bEnableRide );
}

void api_trigger_AllMonsterUseSkill( CDNGameRoom * pRoom, int nSkillID, int nSkillLevel, int nDelay, bool bEternity )
{
	PROFILE_TICK_TEST_BLOCK_START( "api_trigger_AllMonsterUseSkill" );

	if( !pRoom )
		return;

	CDnSkill::SkillInfo sSkillInfo;
	std::vector<CDnSkill::StateEffectStruct> vecSkillEffectList;
	CDnSkill::CreateSkillInfo( nSkillID, nSkillLevel, sSkillInfo, vecSkillEffectList );
	sSkillInfo.bIgnoreImmune = true;

	DNVector(DnActorHandle) vecActorList;
	CDnActor::GetActorList( pRoom, vecActorList );

	for( DWORD itr = 0; itr < vecActorList.size(); ++itr )
	{
		DnActorHandle hActor = vecActorList[itr];

		if( !hActor || hActor->IsDie() || !hActor->IsMonsterActor() )
			continue;

		DnMonsterActorHandle hMonsterActor = hActor;

		if( !hMonsterActor || hMonsterActor->GetSummonerPlayerActor() )
			continue;

		sSkillInfo.hSkillUser = hActor;
		for( int nEffectIndex = 0; nEffectIndex < (int)vecSkillEffectList.size(); ++nEffectIndex )
		{
			hActor->CmdAddStateEffect( &sSkillInfo, (STATE_BLOW::emBLOW_INDEX)vecSkillEffectList[nEffectIndex].nID, nDelay, 
				vecSkillEffectList[nEffectIndex].szValue.c_str(), false, true, bEternity );
		}
	}

	PROFILE_TICK_TEST_BLOCK_END();
}

bool api_trigger_RequireTotalComboCount( CDNGameRoom * pRoom, UINT nActorHandle, int nTotalComboCount )
{
	if( NULL == pRoom )
		return false;

	CDNUserSession* pSession = pRoom->GetUserSession( nActorHandle );
	if( NULL == pSession )
		return false;

	CDnPlayerActor * pPlayer = pSession->GetPlayerActor();
	if( NULL == pPlayer )
		return false;

	if( pPlayer->GetTotalComboCount() >= nTotalComboCount )
		return true;

	return false;
}

void api_trigger_RequireTotalComboReset( CDNGameRoom * pRoom, UINT nActorHandle )
{
	if( NULL == pRoom )
		return;

	CDNUserSession* pSession = pRoom->GetUserSession( nActorHandle );
	if( NULL == pSession )
		return;

	CDnPlayerActor * pPlayer = pSession->GetPlayerActor();
	if( NULL == pPlayer )
		return;

	pPlayer->ResetTotalComboCount();
}

int api_trigger_GetPartyInClassCount( CDNGameRoom* pRoom, int nClassID )
{
	if( NULL == pRoom )
		return 0;
	
	return pRoom->GetPartyUserClassCount(nClassID);
}

void api_trigger_Set_AlteiaWorld( CDNGameRoom* pRoom, bool bFlag )
{
#if defined( PRE_ALTEIAWORLD_EXPLORE )
	if( NULL == pRoom )
		return;

	pRoom->SetAlteiaWorld(bFlag);
#endif
	return;
}

#if defined(PRE_ADD_OPEN_QUEST_TEXTURE_DIALOG)
int api_ui_OpenTextureDialog(CDNGameRoom* pRoom, UINT nUserObjectID, int nFileIndex, float fX, float fY, int nPos, int nTime, int nDialogIndex, bool bAutoCloseDialog)
{
	DN_ASSERT(NULL != pRoom,	"Invalid!");
	CDNUserSession* pUser = pRoom->GetUserSession(nUserObjectID);
	if ( !pUser ) 
		return -1;

	SCOpenTextureDialog sDlgData;
	sDlgData.fX = fX;
	sDlgData.fY = fY;
	sDlgData.nTime = nTime;
	sDlgData.nPosition = nPos;
	sDlgData.nFileIndex = nFileIndex;
	sDlgData.nDialogIndex = nDialogIndex;
	sDlgData.bAutoCloseDialog = bAutoCloseDialog;

	return DNScriptAPI::api_Open_Texture_Dialog(pUser, sDlgData);
}

int api_ui_CloseTextureDialog(CDNGameRoom* pRoom, UINT nUserObjectID, int nDlgID)
{
	DN_ASSERT(NULL != pRoom,	"Invalid!");
	CDNUserSession* pUser = pRoom->GetUserSession(nUserObjectID);
	if ( !pUser ) 
		return -1;

	return DNScriptAPI::api_Close_Texture_Dialog(pUser, nDlgID);
}
#endif

void api_tirgger_Set_UserHPSP( CDNGameRoom * pRoom, UINT nActorHandle, int nHP_Ratio, int nSP_Ratio )
{
	if( NULL == pRoom )
		return;

	CDNUserSession* pSession = pRoom->GetUserSession( nActorHandle );
	if( NULL == pSession )
		return;

	CDnPlayerActor * pPlayer = pSession->GetPlayerActor();
	if( NULL == pPlayer )
		return;

	INT64 nHP = (INT64)( pPlayer->GetMaxHP() *( nHP_Ratio / 100.f ) );
	if( nHP > pPlayer->GetMaxHP() )
		nHP = pPlayer->GetMaxHP();
	else if( nHP < 1 )
		nHP = 1;

	int nSP = (int)( pPlayer->GetMaxSP() *( nSP_Ratio / 100.f ) );
	if( nSP > pPlayer->GetMaxSP() )
		nSP = pPlayer->GetMaxSP();
	else if( nSP < 1 )
		nSP = 1;

	pPlayer->CmdRefreshHPSP( nHP, nSP );
}

void api_trigger_Set_ItemUseCount( CDNGameRoom* pRoom, int nItemID, int nUseCount )
{
#if defined( PRE_ADD_STAGE_USECOUNT_ITEM )
	if( !pRoom )
		return;
	
	for( UINT i=0 ; i<pRoom->GetUserCount() ; ++i )
	{
		CDNUserSession* pUser = pRoom->GetUserData(i);
		if( pUser )
		{
			CDNGameRoom::PartyStruct* pParty = pRoom->GetPartyData(pUser);			
			if (pParty)
			{				
				pParty->UseLimitItem.insert(make_pair(nItemID, nUseCount));				
			}			
			pUser->SendStageUseLimitItem( nItemID, nUseCount );
		}
	}
#endif
}

//rlkt_test
bool api_trigger_CheckJobChangeProcess(CDNGameRoom* pRoom, UINT nActorHandle)
{
	if (!pRoom)
		return false;


	//rlkt_placeholder!
	return true;
} 

int api_ui_OpenJobChange(CDNGameRoom* pRoom, UINT nUserObjectID)
{
	if (!pRoom)
		return false;

	CDNUserSession* pUser = pRoom->GetUserSession(nUserObjectID);
	if (!pUser)
		return -1;

	return DNScriptAPI::api_Open_ChangeJobDialog(pUser); //(pRoom, nUserObjectID);
}

int api_trigger_Emergence_Begin(CDNGameRoom* pRoom, int nEventAreaHandle)
{
	return 1;
}

int api_trigger_Emergence_GetValue(CDNGameRoom* pRoom, UINT nValueType)
{

	//OutputDebug("%s nValueType:%d", __FUNCTION__, nValueType);
	//rlkt_placeholder.
	
	if(nValueType == 1)
	{
		return 0;
	}

	return 0;
}

bool api_trigger_Emergence_CheckSelectedEvent(CDNGameRoom* pRoom, int nEventAreaHandle)
{
	return false;
}

bool api_trigger_CheckForceMoveComebackRoom(CDNGameRoom* pRoom)
{
	return false;
}