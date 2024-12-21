#include "StdAfx.h"
#include "DnInterface.h"
#include "DnActor.h"
#include "DnTableDB.h"
#include "InputWrapper.h"
#include "DnMinimap.h"
#include "DnLocalPlayerActor.h"
#include "DnMonsterActor.h"
#include "GameSendPacket.h"
#include "DnMainFrame.h"

// Task Include
#include "DnTitleTask.h"
#include "DnGameTask.h"
#include "DnVillageTask.h"
#include "TaskManager.h"
#include "DnPartyTask.h"
#include "DnTradeTask.h"
#include "DnTradePrivateMarket.h"
#include "DnItemTask.h"
#include "DnFriendTask.h"
#include "DnBridgeTask.h"
#include "DnCutSceneTask.h"
#include "DnCommonTask.h"
#include "DnRadioMsgTask.h"
#include "DnSkillTask.h"
#include "DnGuildTask.h"
#include "DnRestraintTask.h"
#include "DNVoiceChatTask.h"
#include "DnAuthTask.h"
// Dialog Include
#include "DnInterfaceString.h"
#include "DnUIString.h"
#include "DnMainDlg.h"
#include "DnGaugeDlg.h"
#include "DnLoginDlg.h"
#include "DnChatTabDlg.h"
#include "DnMessageBox.h"
#include "DnBigMessageBox.h"
#include "DnMiddleMessageBox.h"
#include "DnChatOption.h"
#include "DnTooltipDlg.h"
#include "DnNpcDlg.h"
#include "DnCharStatusDlg.h"
#include "DnNoticeDlg.h"
#include "DnCharCreateDlg.h"
#include "DnCharCreateDarkDlg.h"
#include "DnCharCreateBackDlg.h"
#ifdef PRE_MOD_SELECT_CHAR
#include "DnCharLoginTitleDlg.h"
#include "DnCharCreatePlayerInfo.h"
#include "DnCharCreateSelectDlg.h"
#include "DnCharRotateDlg.h"
#include "DnCharGestureDlg.h"
#else // PRE_MOD_SELECT_CHAR
#include "DnCharCreateTitleDlg.h"
#endif // PRE_MOD_SELECT_CHAR

#ifdef PRE_ADD_CHARSELECT_PREVIEW_CASHITEM
#include "DnCharSelectCostumePreviewDlg.h"
#endif // PRE_ADD_CHARSELECT_PREVIEW_CASHITEM

#include "DnCharCreateServerNameDlg.h"
#include "DnCharSelectDlg.h"
#include "DnPlayerInfoDlg.h"
#include "DnCharDeleteWaitDlg.h"
#include "DnSkillTabDlg.h"
#include "DnSkillTreeDlg.h"
#include "DnSkillTreeContentDlg.h"
#include "DnGateQuestionDlg.h"
#include "DnCaptionDlg_01.h"
#include "DnCaptionDlg_02.h"
#include "DnCaptionDlg_03.h"
#include "DnStaffrollDlg.h"
#include "DnCaptionDlg_04.h"
#include "DnCaptionDlg_05.h"
#include "DnCaptionDlg_06.h"
#ifdef PRE_ADD_NEWCOMEBACK
#include "DnCaptionDlg_07.h" // PRE_ADD_NEWCOMEBACK
#endif // PRE_ADD_NEWCOMEBACK
#include "DnFadeInOutDlg.h"
#include "DnAssertDlg.h"
#include "DnDamageMng.h"
#include "DnComboMng.h"
#include "DnChainMng.h"
#include "DnCpJudgeMng.h"
#include "DnCpScoreMng.h"
#include "DnCpDownMng.h"
#ifdef PRE_ADD_CRAZYDUC_UI
#include "DnScoreMng.h"
#endif 
#include "DnServerListDlg.h"
#include "DnWaitUserDlg.h"
#include "DnChannelListDlg.h"
#include "DnSimpleTooltipDlg.h"
#include "DnCustomTooltipDlg.h"
#include "DnDirectDlg.h"
#include "DnInspectPlayerDlg.h"
#include "DnAcceptDlg.h"
#include "DnPrivateChatDlg.h"
#include "DnStickAniDlg.h"
#include "DnEnemyGaugeMng.h"
#include "DnDungeonEnterDlg.h"
#include "DnEventDungeonEnterDlg.h"
#include "DnMainMenuDlg.h"
#include "DnMessageManager.h"
#include "DnSkillStoreTabDlg.h"
#include "DnTextureDlgMng.h"
#include "DnTextBoxDlgMng.h"
#include "DnItemAlarmDlg.h"
#include "GameOption.h"
#include "MASingleBody.h"
#include "DnInvenTabDlg.h"
#include "DnSystemDlg.h"
#include "DnChannelMoveDlg.h"
#include "DnCharCreateBackDlg.h"
#include "DnMissionFailDlg.h"
#include "DnDungeonClearMoveDlg.h"
#include "DnRebirthCaptionDlg.h"
#include "DnRebirthFailDlg.h"
#include "DnStageClearReportDlg.h"
#include "DnStageClearDlg.h"
#include "DnPrivateMarketReqDlg.h"
#include "DnAcceptRequestDlg.h"
#include "DnRandomItemDlg.h"
#include "DnMovieDlg.h"
#include "DnMovieProcessDlg.h"
#include "DnDungeonInfoDlg.h"
#include "DnNpcTalkReturnDlg.h"

#include "DnPVPRoomListDlg.h"
#include "DnPVPGameRoomDlg.h"
#include "DnPVPLobbyVillageTask.h"
#include "DnPVPModeEndDlg.h"
#include "DnRespawnGauageDlg.h"
#include "DnRespawnModeHUD.h"
#include "DnPVPLobbyChatTabDlg.h"
#include "DnPVPGameResultDlg.h"
#include "DnPVPZoneMapDlg.h"
#include "DnGuildZoneMapDlg.h"
#include "DnPvpGuildPropInfoDlg.h"
#include "DnPvPGameTask.h"
#include "DnRoundModeHUD.h"
#include "DnPvPKilledMeDlg.h"
#include "DnPvPRacingResultDlg.h"

#include "DnIndividualRespawnModeHUD.h"
#include "DnIndividualRoundModeHUD.h"
#include "DnPVPGameResultAllDlg.h"
#include "DnGuildWarResultDlg.h"

#include "DnStateBlow.h"
#include "DnMissionAchieveAlarmDlg.h"
#include "DnMissionGainAlarmDlg.h"
#include "DnProgressDlg.h"
#include "DnProgressMBox.h"
#include "DnSlideCaptionDlg.h"
#include "DnGameTipDlg.h"
#include "DnGuildCreateDlg.h"
#include "DnGuildYesNoDlg.h"
#include "DnGuildInviteReqDlg.h"
#include "DnGuildMarkCreateDlg.h"
#include "DnDarklairDungeonEnterDlg.h"
#include "DnDarklairRoundCountDlg.h"
#include "DnDarklairInfoDlg.h"
#include "DnDarklairClearReportDlg.h"
#include "DnDarklairClearRankDlg.h"
#include "DnDarklairClearRewardDlg.h"
#include "DnDarkLairRankBoardDlg.h"
#include "DnHelpKeyboardDlg.h"
#include "DnDungeonOpenNoticeDlg.h"
#include "DnSecurityKBDDlg.h"
#include "DnSecurityNUMPADChangeDlg.h"
#include "DnSecurityNUMPADCreateDlg.h"
#include "DnSecurityNUMPADCheckDlg.h"
#include "DnSecondPassCheck.h"
#include "DnSecurityNUMPADDeleteDlg.h"

#include "DnWorldMsgDlg.h"
#include "DnSkillResetDlg.h"

#include "DnNameLinkMng.h"
#include "DnItemChoiceDlg.h"
#include "DnStageClearMoveDlg.h"
#include "DnItemUnSealDlg.h"
#include "DnItemSealDlg.h"

#include "DnCashShopMenuDlg.h"

#ifdef PRE_ADD_CASHSHOP_RENEWAL
#include "DnCashShopRenewalDlg.h"
#else
#include "DnCashShopDlg.h"
#endif // PRE_ADD_CASHSHOP_RENEWAL

#include "GameOptionSendPacket.h"
#include "DnStoreConfirmExDlg.h"

#include "DnInvenDlg.h"
#include "DNIsolate.h"

#include "DnMODdlgBase.h"
#include "DnPotentialJewelDlg.h"
#include "DnHelpAlarmDlg.h"
#include "DnEnchantJewelDlg.h"
#include "DnPetalTokenDlg.h"
#include "DnAppellationGainDlg.h"
#include "DnBossAlertDlg.h"

#include "DnSDOAService.h"
#include "DnAllKillModeHUD.h"

#include "DnPVPLadderSystemDlg.h"
#include "DnPVPLadderTabDlg.h"
#include "DnPVPLadderRankBoardDlg.h"
#if defined(PRE_ADD_DWC)
#include "DnPVPDWCTabDlg.h"
#include "DnDWCTask.h"
#include "DnDWCCharCreateDlg.h"
#include "DnDwcCreateTeamDlg.h"
#include "DnDwcInviteReqDlg.h"
#include "DnDWCMarkTooltipDlg.h"
#endif

#ifdef PRE_FIX_ESCAPE
#include "DnEscapeDlg.h"
#endif

#include "DnCharPetDlg.h"
#include "DnPetOptionDlg.h"

// 일본 전용 가챠폰
#ifdef PRE_ADD_GACHA_JAPAN
#include "DnGachaDlg.h"
#include "DnGachaRouletteDlg.h"
#endif

#include "DnContextMenuDlg.h"
#include "DnCostumeMixDlg.h"
#include "DnCostumeDesignMixDlg.h"
#ifdef PRE_ADD_COSRANDMIX
#include "DnCostumeRandomMixDlg.h"
#endif
#include "DnChatRoomDlg.h"
#include "DnChatRoomPassWordDlg.h"
#include "DnCharmItemKeyDlg.h"
#include "DnCharmItemProgressDlg.h"
#include "DnCharmItemResultDlg.h"
#include "DnPackageBoxResultDlg.h"
#include "DnPackageBoxProgressDlg.h"
#include "DnVoiceChatPremiumOptDlg.h"

#ifdef PRE_ADD_AUTOUNPACK
#include "DnAutoUnPackResultDlg.h"
#include "DnAutoUnPackDlg.h"
#endif

#ifdef PRE_ADD_CASHREMOVE
#include "DnInvenCashItemRemoveDlg.h"
#endif
#include "DnMasterTask.h"

#include "DnLifeSkillFishingDlg.h"
#include "DnPartyListDlg.h"
#include "DnLifeChannelDlg.h"
#include "DnLifeSkillPlantTask.h"
#include "DnLifeConditionDlg.h"
#include "DnLifeTooltipDlg.h"
#include "DnLifeSlotDlg.h"

#include "DnGuildWarPreliminaryStatusDlg.h"
#include "DnGuildWarPreliminaryResultDlg.h"
#include "DnGuildWarFinalTournamentListDlg.h"
#include "DnGuildWarTask.h"

#include "DnGuildWarSituationMng.h"
#include "DnGuildWarModeHUD.h"
#include "DnGuildWarSkillSlotDlg.h"
#include "DnGuildWarBossGauge.h"
#include "DnOccupationModeHUD.h"
#include "DnGuildWarRoomDlg.h"
#include "DnGuildRenameDlg.h"
#include "DnCharRenameDlg.h"

#ifdef PRE_ADD_PVP_TOURNAMENT
#include "DnPVPTournamentRoomDlg.h"
#include "DnPVPTournamentRoomMatchListDlg.h"
#include "DnPVPTournamentModeHud.h"
#include "DnPVPTournamentGameMatchListDlg.h"
#include "DnPVPTournamentGameMatchListItemDlg.h"
#include "DnPVPTournamentFinalResultDlg.h"
#endif

#ifdef PRE_WORLDCOMBINE_PVP
#include "DnWorldPVPRoomStartDlg.h"
#endif // PRE_WORLDCOMBINE_PVP

#ifdef PRE_ADD_CASHSHOP_REFUND_CL
#include "DnCashShopRefundInvenDlg.h"
#endif

#include "DnDurabilityGaugeMng.h"
#include "DnCashShopTask.h"
#include "DnRepUnionMarkTooltip.h"

#include "DnPetNamingDlg.h"
#include "DnPetExtendPeriodDlg.h"

#ifdef PRE_ADD_CHANGEJOB_CASHITEM
#include "DnChangeJobCashItemDlg.h"
#endif // #ifdef PRE_ADD_CHANGEJOB_CASHITEM

#if defined(_CH) && defined(_AUTH)
#include "DnSDOUADlg.h"
#endif
#if defined(_TH) && defined(_AUTH)
#include "DnTHOTPDlg.h"
#endif	// _TH && _AUTH
#if defined(_US) && defined(_AUTH)
#include "DnNexonService.h"
#endif // _US & _AUTH

#include "DnPartsMonsterActor.h"

#include "DnLifeUnionMarkTooltip.h"
#include "DnGuildWarMarkTooltipDlg.h"
#include "DnGuildRewardMarkTooltipDlg.h"

#include "DnGuildWarFinalWinNoticeDlg.h"
#include "DnProgressSimpleMessageBox.h"

#ifdef PRE_ADD_LEVELUP_GUIDE
#include "DnLevelUpGuideDlg.h"
#endif

#if defined(PRE_ADD_REMOVE_PREFIX)
#include "DnRemovePrefixDlg.h"
#endif // PRE_ADD_REMOVE_PREFIX

#include "DnHardCoreModeTimeAttackDlg.h"
#include "DnTimeOutFailDlg.h"

#ifdef PRE_ADD_STAGE_LIMIT_INTERFACE
#include "DnDungeonLimitDlg.h"
#endif

#include "DnPetAddSkillDlg.h"
#include "DnPetExpandSkillSlotDlg.h"
#ifdef PRE_ADD_PET_EXTEND_PERIOD
#include "DnBuyPetExtendPeriodDlg.h"
#endif // PRE_ADD_PET_EXTEND_PERIOD
#include "DnNestInfoDlg.h"
#include "DnMapMoveCashItemDlg.h"

#include "DnGuildLevelUpAlarmDlg.h"

#ifdef PRE_ADD_EXCHANGE_POTENTIAL
#include "DnPotentialTransferDlg.h"
#endif

#include "DnPGStorageTabDlg.h"

#ifdef PRE_ADD_CHAOSCUBE
#include "DnChaosCubeDlg.h"
#include "DnChaosCubeProgressDlg.h"
#include "DnChaosCubeResultDlg.h"
#include "DnChaosCubeStuffDlg.h"
#endif

#ifdef PRE_ADD_CHOICECUBE
#include "DnChoiceCubeDlg.h"
#endif

#include "DnWorldZoneSelectDlg.h"
#ifdef PRE_ADD_BESTFRIEND
#include "DnBestFriendRegistrationDlg.h"
#include "DnIdentifyBestFriendDlg.h"
#include "DnBestFriendProgressDlg.h"
#include "DnBestFriendRewardDlg.h"
#include "DnBestFriendRewardProgressDlg.h"
#include "DnBestFriendMemoDlg.h"
#endif

#ifdef PRE_ADD_INSTANT_CASH_BUY
#include "DnInstantCashShopBuyDlg.h"
#endif // PRE_ADD_INSTANT_CASH_BUY

#ifdef PRE_PRIVATECHAT_CHANNEL
#include "DnPrivateChannelDlg.h"
#endif // PRE_PRIVATECHAT_CHANNEL

#ifdef PRE_ADD_DOORS
#include "DnDoorsMobileAuthButtonDlg.h"
#endif

#if defined( PRE_ADD_REVENGE )
#include "DnPVPRevengeMessageBoxDlg.h"
#endif	// #if defined( PRE_ADD_REVENGE )

#ifdef PRE_ADD_PVP_RANKING
#include "DnPvPRankDlg.h"
#endif // PRE_ADD_PVP_RANKING

#ifdef PRE_ADD_DIRECT_BUY_UPGRADEITEM
#include "DnMarketDirectBuyDlg.h"
#endif

#ifdef PRE_ADD_NEWCOMEBACK
#include "DnComeBackRewardDlg.h"
#include "DnComeBackMsgDlg.h"
#endif // PRE_ADD_NEWCOMEBACK

#ifdef PRE_ADD_ACTIVEMISSION
#include "DnAcMissionAlarmDlg.h"
#endif // PRE_ADD_ACTIVEMISSION

#ifdef PRE_ADD_GETITEM_WITH_RANDOMVALUE
#include "DnCharmItemKeyDlg.h"
#endif 

#if defined( PRE_ADD_STAGE_CLEAR_ADD_REWARD )
#include "DnDungeonExtraRewardDlg.h"
#endif	// PRE_ADD_STAGE_CLEAR_ADD_REWARD

#ifdef PRE_ADD_FADE_TRIGGER
#include "DnFadeCaptionDlg.h"
#endif // PRE_ADD_FADE_TRIGGER

#ifdef PRE_ADD_PVP_COMBOEXERCISE
#include "DnComboExerciseModeHUD.h"
#endif // PRE_ADD_PVP_COMBOEXERCISE

#ifdef PRE_ADD_ALTHEIA_NEWCAPTION_TRIGGER
#include "DnImageBlindDlg.h"
#endif // PRE_ADD_ALTHEIA_NEWCAPTION_TRIGGER

#ifdef PRE_MOD_POTENTIAL_JEWEL_RENEWAL
#include "DnPotentialJewelCleanDlg.h"
#include "DnPotentialJewel.h"
#endif // PRE_MOD_POTENTIAL_JEWEL_RENEWAL

#include "DnDungeonSynchroDlg.h"
#include "DnAlteaBoardDlg.h"
#include "DnAlteaClearDlg.h"

#ifdef PRE_ADD_STAMPSYSTEM
#include "DnStampEventAlarmDlg.h"
#endif // PRE_ADD_STAMPSYSTEM

#ifdef PRE_ADD_GAMEQUIT_REWARD
#include "DnGameQuitRewardDlg.h"
#include "DNGameQuitNextTimeRewardDlg.h"
#include "DNGameQuitNextTimeRewardComboDlg.h"
#endif // PRE_ADD_GAMEQUIT_REWARD

#ifdef PRE_ADD_PVPRANK_INFORM
#include "DnPvPRankInformDlg.h"
#endif

#ifdef PRE_ADD_CRAZYDUC_UI
#include "DnMODCustom3Dlg.h"
#endif 

#ifdef PRE_ADD_MAINQUEST_UI
#include "DnMainQuestDlg.h"
#include "DnNpcTalkCamera.h"
#endif

#ifdef PRE_ADD_DRAGON_GEM
#include "DnDragonGemEquipDlg.h"
#include "DnDragonGemRemoveDlg.h"
#endif

#include "ArabicText.h"
#include "DnRebirthSystem.h"//rlkt_rebirth

#include "FarmSendPacket.h" //farmpvp

//new login ui
#include "DnCharCreateSetupDlg.h"
#include "DnCharCreateSetupGestureDlg.h"
#include "DnCharCreateNameDlg.h"
#include "DnCharCreateSetupBackDlg.h"

//rlkt_test change job
#include "DnJobChangeDlg.h"

//rlkt_repair
#include "DnDungeonClearRepairDlg.h"

//rlkt_disjoint
#include "DnItemDisjointResultDlg.h"

//rlkt_Instant Specialzation
#include "DnInstantSpecializationDlg.h"

using namespace EternityEngine;

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

#define LOGO_DELAY_TIME		3.f

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


namespace
{
	const int g_cGoldItemID = 1073750029; // 골드아이템.
};


CDnInterface::CDnInterface(void)
	: CInputReceiver( true )
{
	m_Type = Title;
	m_szTaskName = "TitleTask";
	m_bEnableRender = true;
	m_bProcessMainProcess = false;

	// Note : UI
	//
	m_pMessageBox = NULL;
	m_pMessageBoxTextBox = NULL;
	m_pBigMessageBox = NULL;
	m_pMiddleMessageBox = NULL;
	m_pAssertDlg = NULL;
	m_pSimpleTooltipDlg = NULL;
	m_pCustomTooltipDlg = NULL;
	m_pAcceptDlg = NULL;
	m_pFadeDlg = NULL;
	m_pFadeForBlowDlg = NULL;
	m_pBlindDlg = NULL;
	m_pTextureDlgMng = NULL;
	m_pTextBoxDlgMng = NULL;
	m_CurAcceptReqDlg = eNONE;
	m_pSecurityKBDDlg = NULL;
	m_pSecurityChangeDlg= NULL;
	m_pSecurityCheckDlg= NULL;
	m_pSecurityCreateDlg= NULL;
	m_pSecondPassCheck =NULL;
	m_pSecurityDeleteDlg = NULL ;

#ifdef PRE_ADD_NEWCOMEBACK
	m_pComeBackRewardDlg = NULL;
	m_pComebackMsgDlg = NULL;
#endif // PRE_ADD_NEWCOMEBACK

#ifdef PRE_ADD_PVP_RANKING
	m_pPvPRankDlg = NULL;
#endif // PRE_ADD_PVP_RANKING

#ifdef PRE_ADD_ACTIVEMISSION
	m_pActiveMissionDlg = NULL;
#endif // PRE_ADD_ACTIVEMISSION

#ifdef PRE_ADD_STAMPSYSTEM
	m_pStampEventAlarmDlg = NULL;
#endif // PRE_ADD_STAMPSYSTEM

#ifdef PRE_ADD_GAMEQUIT_REWARD
	m_pGameQuitRewardDlg = NULL;
	m_pGameQuitNextTimeRewardDlg = NULL;
	m_pGameQuitNextTimeRewardComboDlg = NULL;
#endif // PRE_ADD_GAMEQUIT_REWARD

	// Note : Login
	//
	m_State = CDnLoginTask::LoginStateEnum::RequestIDPass;

	m_pLoginDlg = NULL;
	m_pCharSelectDlg = NULL;
	m_pCharCreateDlg = NULL;
	m_pCharCreateDarkDlg = NULL;
	m_pCharCreateBackDlg = NULL;
	m_pCharCreateServerNameDlg = NULL;
	m_pServerListDlg = NULL;
	m_pWaitUserDlg = NULL;
	m_pChannelListDlg = NULL;
	m_pStaffrollDlg = NULL;
#ifdef PRE_ADD_DOORS
	m_pDoorsMobileAuthDlg = NULL;
#endif
#if defined(_CH) && defined(_AUTH)
	m_pSDOUADlg = NULL;
#endif
#if defined(_TH) && defined(_AUTH)
	m_pTHOTPDlg = NULL;
#endif	// _TH && _AUTH

#ifdef PRE_MOD_SELECT_CHAR
	m_pCharLoginTitleDlg = NULL;
	m_pCharCreatePlayerInfo = NULL;
	m_pCharCreateSelectDlg = NULL;
	m_pCharRotateDlg = NULL;
	m_pCharGestureDlg = NULL;

#ifdef _ADD_NEWLOGINUI
	m_pCharCreateSetupDlg = NULL;
	m_pCharCreateSetupGestureDlg = NULL;
	m_pCharCreateNameDlg = NULL;
	m_pCharCreateSetupBackDlg = NULL;
#endif

	//
	m_pJobChangeDlg = NULL;
	//rlkt_disjoint
	m_pItemDisjointResultDlg = NULL;
	m_pInstantSpecializationDlg = NULL;

#else // PRE_MOD_SELECT_CHAR
	m_pCharCreateTitleDlg = NULL;
	SecureZeroMemory( m_pSlotInfoDlg, sizeof(m_pSlotInfoDlg) );
	SecureZeroMemory( m_pSlotDeleteWaitDlg, sizeof(m_pSlotDeleteWaitDlg) );
#endif // PRE_MOD_SELECT_CHAR

#ifdef PRE_ADD_CHARSELECT_PREVIEW_CASHITEM 
	m_pCharSelectPreviewCostumeDlg = NULL;
#endif //PRE_ADD_CHARSELECT_PREVIEW_CASHITEM

#ifdef PRE_ADD_DWC
	m_pDWCCharCreateDlg = NULL;	
#endif


	// Note : Game, Village
	InitCommonDlg();

	// Note : Village

	// Note : Game
	m_pDamageMng = NULL;
	m_pComboMng = NULL;
	m_pChainMng = NULL;
	m_pCpJudgeMng = NULL;
	m_pCpScoreMng = NULL;
	m_pCpDownMng = NULL;
	m_pEnemyGaugeMng = NULL;
	m_pStickAniDlg = NULL;
	m_pMissionFailDlg = NULL;
	m_pDungeonMoveDlg = NULL;
	m_pRebirthCaptionDlg = NULL;
	m_pRebirthFailDlg = NULL;
	m_pDungeonEnterDlg = NULL;
	m_pDungeonNestEnterDlg = NULL;
	m_pDnHardCoreModeTimeAttackDlg = NULL;
	m_pTimeOutFailDlg = NULL;
#ifdef PRE_ADD_STAGE_LIMIT_INTERFACE
	m_pDungeonLimitDlg = NULL;
#endif
	m_pDungeonSynchroDlg = NULL;

	m_pEventDungeonEnterDlg = NULL;
	m_pStageClearReportDlg = NULL;
	m_pStageClearDlg = NULL;
	m_pRandomItemDlg = NULL;
	m_pMovieDlg = NULL;
	m_pDungeonInfoDlg = NULL;
	m_pNameLinkMng = NULL;
	m_pNameLinkToolTipDlg = NULL;
#if defined( PRE_ADD_STAGE_CLEAR_ADD_REWARD )
	m_pDungeonExtraRewardDlg = NULL;
#endif	// PRE_ADD_STAGE_CLEAR_ADD_REWARD

	m_pItemChoiceDlg = NULL;
	m_pStageClearMoveDlg = NULL;
	m_pStageClearRepairDlg = NULL;

	m_pBossAlertDlg = NULL;
	m_pMovieDlg = NULL;
	m_pLifeTooltipDlg = NULL;
	m_pLifeConditionDlg = NULL;
	m_pLifeSlotDlg = NULL;
#ifdef PRE_ADD_GACHA_JAPAN
	m_pGachaDlg = NULL;
	m_pGachaItemNameRouletteDlg = NULL;
	m_pGachaItemStatRouletteDlg = NULL;
#endif

	//pvp game
	m_pRespawnGauageDlg = NULL;
	m_pPVPModeEndDlg = NULL;
	m_pModeHUD = NULL;
	m_pPVPGameResultDlg = NULL;
	m_pPVPZoneMapDlg = NULL;
	m_pGuildWarZoneMapDlg = NULL;
	m_pGuildPropInfoDlg = NULL;
	m_pPvPKilledMeDlg = NULL;
	m_pPvPRacingResultDlg = NULL;

#ifdef PRE_ADD_PVP_TOURNAMENT
	m_pPVPTournamentGMatchListDlg = NULL;
	m_pPVPTournamentGMatchItemDlg = NULL;
	m_pPVPTournamentFinalResultDlg = NULL;
#endif
#ifdef PRE_WORLDCOMBINE_PVP
	m_pWorldPVPRoomStartDlg = NULL;
#endif // PRE_WORLDCOMBINE_PVP
	// Darklair
	m_pDarkLairEnterDlg = NULL;
	m_pDarkLairRoundCountDlg = NULL;
	m_pDarkLairInfoDlg = NULL;
	m_pDarkLairClearReportDlg = NULL;
	m_pDarkLairClearRankDlg = NULL;
	m_pDarkLairClearRewardDlg = NULL;
	m_pDarkLairRankBoardDlg = NULL;
	m_pPVPLadderRankBoardDlg = NULL;
	m_pPVPGameResultAllDlg = NULL;
	m_pGuildWarResultDlg = NULL;
	m_pLifeChannelDlg = NULL;


	// Guild War game
	m_pGuildWarPreliminaryStatusDlg = NULL;
	m_pGuildWarPreliminaryResultDlg = NULL;
	m_pGuildWarFinalTournamentListDlg = NULL;

	m_pGuildWarSituation = NULL;
	m_pGuildWarBossGaugeDlg = NULL;

	m_nCurLogoIndex = 0;

	m_bEnableCountDown = false;
	m_fCountDownDelta = 0.f;

	m_bIsOpenNpcDlg = false;
	m_nAccumulationCPCache = 0;

	m_pWorldDataMng = new CDnWorldDataManager;
	m_pWorldDataMng->Initialize( CEtResourceMng::GetInstance().GetFullPath( "MapData" ).c_str() );

	m_ePVPRoomState = ePVP_Room;

	m_pDurabilityGaugeMng = NULL;

	// 10회 이상 로드 되는것들은 Cache에 등록해놓는다.
	CEtResourceMng::GetInstance().AddCacheMemoryStream( CEtResourceMng::GetInstance().GetFullName( "ComboDlg.ui" ).c_str() );
	CEtResourceMng::GetInstance().AddCacheMemoryStream( CEtResourceMng::GetInstance().GetFullName( "DamageDlg.ui" ).c_str() );
	CEtResourceMng::GetInstance().AddCacheMemoryStream( CEtResourceMng::GetInstance().GetFullName( "EnemyGaugeDlg.ui" ).c_str() );
	CEtResourceMng::GetInstance().AddCacheMemoryStream( CEtResourceMng::GetInstance().GetFullName( "InvenItemSepDlg.ui" ).c_str() );
	CEtResourceMng::GetInstance().AddCacheMemoryStream( CEtResourceMng::GetInstance().GetFullName( "InvenItemSlotDlg.ui" ).c_str() );
	CEtResourceMng::GetInstance().AddCacheMemoryStream( CEtResourceMng::GetInstance().GetFullName( "StoreConfirmDlg.ui" ).c_str() );
	CEtResourceMng::GetInstance().AddCacheMemoryStream( CEtResourceMng::GetInstance().GetFullName( "StoreConfirmExDlg.ui" ).c_str() );
	CEtResourceMng::GetInstance().AddCacheMemoryStream( CEtResourceMng::GetInstance().GetFullName( "StoreSlotDlg.ui" ).c_str() );

//blondymarry start
	InitPVPLobbyDlg();

	m_hPVPIconImage[0] = LoadResource( CEtResourceMng::GetInstance().GetFullName( "Pvp_Rank.dds" ).c_str(), RT_TEXTURE );
	m_hPVPIconImage[1] = LoadResource( CEtResourceMng::GetInstance().GetFullName( "Pvp_Rank_Big.dds" ).c_str(), RT_TEXTURE );
//blondymarry end

	CFileNameString szGuildMarkFileName;
	bool bExistFile;
	m_vecGuildMarkTextureHandle.resize(GUILDMARK_TEXTURE_COUNT);
	char szTemp[32];
	for( int i = 0; i < GUILDMARK_TEXTURE_COUNT; ++i )
	{
		sprintf_s( szTemp, 32, "GuildMark%02d.dds", i+1 );
		szGuildMarkFileName = CEtResourceMng::GetInstance().GetFullName( szTemp, &bExistFile );
		if( !bExistFile ) continue;
		m_vecGuildMarkTextureHandle[i] = LoadResource( szGuildMarkFileName.c_str(), RT_TEXTURE );
	}

#ifdef PRE_ADD_VIP
	m_hVIPIconImage = LoadResource( CEtResourceMng::GetInstance().GetFullName( "Icon_VIP.dds" ).c_str(), RT_TEXTURE );
#endif
	m_hPartyMemberMarkerImage = LoadResource( CEtResourceMng::GetInstance().GetFullName( "PartyMemberMarker.dds" ).c_str(), RT_TEXTURE );
	m_hGuildPreliminariesImage = LoadResource( CEtResourceMng::GetInstance().GetFullName( "Guild_Preliminaries.dds" ).c_str(), RT_TEXTURE );
	m_hStigmaActorMarketImage = LoadResource( CEtResourceMng::GetInstance().GetFullName( "StigmaActorMarker.dds" ).c_str(), RT_TEXTURE );
	m_hPointMarkImage = LoadResource( CEtResourceMng::GetInstance().GetFullName( "Pvp_PointMark.dds" ).c_str(), RT_TEXTURE );
	m_hPointMarkTailImage = LoadResource( CEtResourceMng::GetInstance().GetFullName( "Pvp_PointMarkTail.dds" ).c_str(), RT_TEXTURE );
	CEtUIDialog::SetAutoCursorCallback( CDnInterface::OnAutoCursorCallback );
	m_FadeState = eFS_NONE;
	m_pCashShopMenuDlg = NULL;
	m_pCashShopDlg = NULL;

	m_fPartyPlayerGaugeOffset = 0.0f;
	m_fEnemyGaugeOffset = 0.0f;

	m_vec2MonsterPartsGaugeOffset = EtVector2(0.f,0.f);

	SecureZeroMemory( m_nCompareInfo, sizeof(m_nCompareInfo) );

#ifdef PRE_SWAP_QUICKSLOT
	m_bSwapState = false;
#endif

#ifdef PRE_ADD_AIM_ASSIST_BUTTON
	m_bAimKeyState = false;
	m_fAimMoveDelay = 0.0f;
#endif
	m_bLockMainMenu = false;
	m_pProgressSimpleMsgBox = NULL;
#ifdef PRE_ADD_LEVELUP_GUIDE
	m_pLevelUpGuildeDlg = NULL;
#endif

	m_bOpenFarmChannel = false;

	m_bJoypadPushed = false;

#ifdef PRE_ADD_COMEBACK
	m_bComeback = false;
#endif

#ifdef PRE_MOD_BROWSER
	m_DisableDlgDelayTime = 0.f;
#endif

#ifdef PRE_ADD_NEWCOMEBACK
	m_bComebackUser = false;
#endif // PRE_ADD_NEWCOMEBACK

#ifdef PRE_ADD_PVP_RANKING
	m_pPvPRankDlg = NULL;
#endif // PRE_ADD_PVP_RANKING


#ifdef PRE_ADD_PVP_COMBOEXERCISE
	m_TimeSummonMonster = 0; // 콤보연습모드에서 몬스터소환 시간제한.
#endif // PRE_ADD_PVP_COMBOEXERCISE

#ifdef PRE_MOD_POTENTIAL_JEWEL_RENEWAL
	m_bIsRemovePotentialDlg = false;
#endif // PRE_MOD_POTENTIAL_JEWEL_RENEWAL

#if defined( PRE_ALTEIAWORLD_EXPLORE )
	m_pAlteaBoardDlg = NULL;
	m_pAlteaClearDlg = NULL;
#endif	// #if defined( PRE_ALTEIAWORLD_EXPLORE )

#ifdef PRE_ADD_PART_SWAP_RESTRICT_TRIGGER
	m_bIsPartSwapRestrict = false;
#endif

#ifdef PRE_ADD_RETURN_VILLAGE_SHOWNAME
	m_nVillage = 0;
#endif 

#ifdef PRE_ADD_PVPRANK_INFORM
	m_pPvPRankInformDlg = NULL;
#endif

#ifdef PRE_ADD_CRAZYDUC_UI
	m_pScoreMng = NULL;
#endif 
}

CDnInterface::~CDnInterface(void)
{
	SAFE_DELETE( m_pWorldDataMng );
	for( int i=0; i<2; i++ )
		SAFE_RELEASE_SPTR(m_hPVPIconImage[i]);

#ifdef PRE_ADD_VIP
	SAFE_RELEASE_SPTR( m_hVIPIconImage );
#endif
	SAFE_RELEASE_SPTRVEC( m_vecGuildMarkTextureHandle );
	SAFE_RELEASE_SPTR( m_hPartyMemberMarkerImage );
	SAFE_RELEASE_SPTR( m_hGuildPreliminariesImage );
	SAFE_RELEASE_SPTR( m_hStigmaActorMarketImage );
	SAFE_RELEASE_SPTR( m_hPointMarkImage );
	SAFE_RELEASE_SPTR( m_hPointMarkTailImage );
	// Note : 최종적으로 interface가 소멸될때 finalize를 호출해준다.
	//		다른 task에서 호출하지만 만약을 위해 한번 더 호출한다.
	//
	{
		Finalize( m_Type );

		switch( m_Type )
		{
		case Village:		
		case Game:
		case PVPGame:
		case PVPVillage:	
		case DLGame:
		case Farm:
			Finalize( Common );
			break;
		}
	}

	SAFE_DELETE(m_pNameLinkMng);
	m_cLastDungeonEnterDialogType = 0;

	PostFinalize();
}

void CDnInterface::Finalize( InterfaceTypeEnum Type )
{
	static void (CDnInterface::*fp[InterfaceTypeMax])() =
	{
		&CDnInterface::FinalizeTitle,
		&CDnInterface::FinalizeLogin,
		&CDnInterface::FinalizeVillage,
		&CDnInterface::FinalizePVPVillage,
		&CDnInterface::FinalizeGame,
		&CDnInterface::FinalizePVPGame,
		&CDnInterface::FinalizeDLGame,
		&CDnInterface::FinalizeFarmGame,
		&CDnInterface::FinalizeCommon,		
	};

	if (m_pDirectDlg)	m_pDirectDlg->Show(false);

	(this->*fp[Type])();

	CEtUIDialog::s_pMouseEnterControl = NULL;

	// Finalize가 호출될때마다 현재 태스크가 유효한지를 확인해서 m_pTask값을 Refresh한다.
	if( CTaskManager::IsActive() && m_szTaskName.size() && Type != Common )
	{
		bool bInvalid = false;
		CTask *pTask = CTaskManager::GetInstance().GetTask( m_szTaskName.c_str() );
		if( pTask == NULL ) bInvalid = true;
		if( pTask != NULL && pTask->m_bDestroyTask ) bInvalid = true;
		if( bInvalid )
		{
			m_pTask = NULL;
			m_szTaskName.clear();
		}
	}
}

void CDnInterface::FinalizeTitle()
{
	SAFE_DELETE_PVEC(m_pVecLogoList);

	OutputDebug( "CDnInterface::FinalizeTitle\n" );
}

void CDnInterface::FinalizeLogin()
{
#ifdef PRE_MOD_SELECT_CHAR
	SAFE_DELETE( m_pCharLoginTitleDlg );
	SAFE_DELETE( m_pCharCreatePlayerInfo );
	SAFE_DELETE( m_pCharCreateSelectDlg );
	SAFE_DELETE(m_pCharRotateDlg);
	SAFE_DELETE(m_pCharGestureDlg);

#ifdef _ADD_NEWLOGINUI
	SAFE_DELETE( m_pCharCreateSetupDlg );
	SAFE_DELETE( m_pCharCreateSetupGestureDlg );
	SAFE_DELETE( m_pCharCreateNameDlg );
	SAFE_DELETE( m_pCharCreateSetupBackDlg );
#endif //_ADD_NEWLOGINUI

#else // PRE_MOD_SELECT_CHAR
	for( int i=0; i<SHOWCHARNUMPERONEPAGE; i++ )
	{
		SAFE_DELETE( m_pSlotInfoDlg[i] );
		SAFE_DELETE( m_pSlotDeleteWaitDlg[i] );
	}
	SAFE_DELETE( m_pCharCreateTitleDlg );
	SAFE_DELETE_PVEC( m_pVecCreateInfoList );
#endif // PRE_MOD_SELECT_CHAR

#ifdef PRE_ADD_DWC
	SAFE_DELETE(m_pDWCCharCreateDlg);
#endif

#ifdef PRE_ADD_CHARSELECT_PREVIEW_CASHITEM 
	SAFE_DELETE(m_pCharSelectPreviewCostumeDlg);
#endif

	SAFE_DELETE( m_pLoginDlg );
	SAFE_DELETE( m_pCharSelectDlg );
	SAFE_DELETE( m_pCharCreateDlg );
	SAFE_DELETE( m_pCharCreateBackDlg );
	SAFE_DELETE( m_pCharCreateDarkDlg );
	SAFE_DELETE( m_pCharCreateServerNameDlg );
	SAFE_DELETE( m_pServerListDlg );
	SAFE_DELETE( m_pWaitUserDlg );
	SAFE_DELETE( m_pChannelListDlg );
	SAFE_DELETE( m_pSecondPassCheck );
	SAFE_DELETE( m_pStaffrollDlg );
#ifdef PRE_ADD_DOORS
	SAFE_DELETE( m_pDoorsMobileAuthDlg );
#endif
#if defined(_CH) && defined(_AUTH)
	SAFE_DELETE( m_pSDOUADlg );
#endif
#if defined(_TH) && defined(_AUTH)
	SAFE_DELETE( m_pTHOTPDlg );
#endif	// _TH && _AUTH


	SAFE_DELETE( m_pTooltipDlg ); // ComeBack - 로그인창에서 툴팁표시.

	OutputDebug( "CDnInterface::FinalizeLogin\n" );
}
//blondymarry start
void CDnInterface::FinalizeVillage()
{

#ifdef PRE_ADD_PVP_VILLAGE_ACCESS
	SAFE_DELETE (m_pPVPLadderTabDlg);
	SAFE_DELETE (m_pPVPRoomListDlg);
#endif

#ifdef PRE_ADD_PVP_RANKING
	SAFE_DELETE( m_pPvPRankDlg );
#endif // PRE_ADD_PVP_RANKING

#ifdef PRE_ADD_PVPRANK_INFORM
	SAFE_DELETE(m_pPvPRankInformDlg);
#endif

	OutputDebug( "CDnInterface::FinalizeVillage\n" );
}
//blondymarry end

void CDnInterface::FinalizePVPVillage()
{
	SAFE_DELETE(m_pPVPRoomListDlg);
	SAFE_DELETE( m_pPVPGameRoomDlg );
	SAFE_DELETE( m_pGuildWarRoomDlg );

#ifdef PRE_ADD_PVP_TOURNAMENT
	SAFE_DELETE(m_pPVPTournamentRoomDlg);
	SAFE_DELETE(m_pPVPTournamentMatchListDlg);
#endif
	SAFE_DELETE (m_pPVPLadderSystemDlg);
	SAFE_DELETE (m_pPVPLadderTabDlg);
#if defined( PRE_ADD_REVENGE )
	SAFE_DELETE( m_pPVPRevengeMessageBox );
#endif	//	#if defined( PRE_ADD_REVENGE )

#ifdef PRE_ADD_PVP_RANKING
	SAFE_DELETE( m_pPvPRankDlg );
#endif // PRE_ADD_PVP_RANKING

#if defined(PRE_ADD_DWC)
	SAFE_DELETE(m_pPVPDWCTablDlg);
#endif // PRE_ADD_DWC

#ifdef PRE_ADD_PVPRANK_INFORM
	SAFE_DELETE(m_pPvPRankInformDlg);
#endif

	m_pPVPChatDlg->Show( false );
/*
	if( m_pChatDlg )
	{
		for( DWORD i=0; i<m_pVecCommonDialogList.size(); i++ ) 
		{
			if( m_pVecCommonDialogList[i] ==  m_pChatDlg )
			{
				m_pVecCommonDialogList.erase(m_pVecCommonDialogList.begin() +i );				
			}
		}

		bool isPVP =  m_pChatDlg->GetGameMode();

		SAFE_DELETE(m_pChatDlg);

		m_pChatDlg = new CDnChatTabDlg( UI_TYPE_FOCUS );
		m_pChatDlg->Initialize( false );
		m_pChatDlg->SetPassMessageToChild( true );
		m_pChatDlg->SetChatGameMode(isPVP);
		AddCommonDialogList(m_pChatDlg);

		m_pChatDlg->s_plistDialogFocus.pop_back();
		m_pChatDlg->s_plistDialogFocus.push_front(m_pChatDlg);
		m_pChatDlg->ShowEx(true);
		m_pChatDlg->ShowEx(false);
 
	}
*/

    OutputDebug( "CDnInterface::FinalizePVPVillage\n" );
}

void CDnInterface::FinalizeGame()
{
	SAFE_DELETE( m_pDamageMng );
	SAFE_DELETE( m_pComboMng );
	SAFE_DELETE( m_pChainMng );
	SAFE_DELETE( m_pCpJudgeMng );
	SAFE_DELETE( m_pCpScoreMng );
	SAFE_DELETE( m_pCpDownMng );
	SAFE_DELETE( m_pEnemyGaugeMng );
#ifdef PRE_ADD_CRAZYDUC_UI
	SAFE_DELETE( m_pScoreMng );
#endif
	SAFE_DELETE( m_pMissionFailDlg );
	SAFE_DELETE( m_pRebirthCaptionDlg );
	SAFE_DELETE( m_pRebirthFailDlg );
	SAFE_DELETE( m_pDungeonMoveDlg );
	SAFE_DELETE( m_pStickAniDlg );
	SAFE_DELETE( m_pDungeonEnterDlg );
	SAFE_DELETE( m_pDungeonNestEnterDlg );
	SAFE_DELETE( m_pDnHardCoreModeTimeAttackDlg );
	SAFE_DELETE( m_pTimeOutFailDlg );
#ifdef PRE_ADD_STAGE_LIMIT_INTERFACE
	SAFE_DELETE( m_pDungeonLimitDlg );
#endif
	SAFE_DELETE( m_pDungeonSynchroDlg );
	SAFE_DELETE( m_pEventDungeonEnterDlg );
	SAFE_DELETE( m_pStageClearReportDlg );
	SAFE_DELETE( m_pStageClearDlg );
	SAFE_DELETE( m_pDungeonInfoDlg );
	SAFE_DELETE( m_pItemChoiceDlg );
	SAFE_DELETE( m_pStageClearMoveDlg );
	SAFE_DELETE( m_pStageClearRepairDlg );

#if defined( PRE_ADD_STAGE_CLEAR_ADD_REWARD )
	SAFE_DELETE( m_pDungeonExtraRewardDlg );
#endif	// PRE_ADD_STAGE_CLEAR_ADD_REWARD
	
	for( DWORD i=0; i<(int)m_pVecEnemyGaugeList.size(); i++ ) 
	{
		SAFE_DELETE( m_pVecEnemyGaugeList[i].pGaugeDlg )
		m_pVecEnemyGaugeList[i].hActor.Identity();
	}

	for( DWORD i=0; i<(int)m_pVecMonsterPartsGuageList.size(); i++ ) 
	{
		SAFE_DELETE( m_pVecMonsterPartsGuageList[i] )
	}
	SAFE_DELETE_VEC( m_pVecMonsterPartsGuageList );

	SAFE_DELETE_VEC( m_pVecEnemyGaugeList );
	SAFE_DELETE( m_pMODDlg );
	SAFE_DELETE( m_pBossAlertDlg );
	SAFE_DELETE( m_pDurabilityGaugeMng );

#if defined( PRE_ALTEIAWORLD_EXPLORE )
	SAFE_DELETE( m_pAlteaBoardDlg );
	SAFE_DELETE( m_pAlteaClearDlg );
#endif	// #if defined( PRE_ALTEIAWORLD_EXPLORE )

#if defined(PRE_ADD_PART_SWAP_RESTRICT_TRIGGER)
	m_bIsPartSwapRestrict = false;
#endif // PRE_ADD_PART_SWAP_RESTRICT_TRIGGER

	OutputDebug( "CDnInterface::FinalizeGame\n" );
}
//blondy
void CDnInterface::FinalizePVPGame()
{
#ifdef PRE_ADD_PVP_COMBOEXERCISE
	m_TimeSummonMonster = 0;
#endif // PRE_ADD_PVP_COMBOEXERCISE

	if( m_pChatDlg )
		m_pChatDlg->SetChatGameMode( false );	// pvp chat mode 가 풀리지 않는 버그 때문에 주석 해제합니다. by kalliste

	FinalizeGame();
	SAFE_DELETE(m_pRespawnGauageDlg);
	SAFE_DELETE(m_pPVPModeEndDlg);
	SAFE_DELETE(m_pModeHUD);
	SAFE_DELETE(m_pPVPGameResultDlg);
	SAFE_DELETE(m_pPVPZoneMapDlg);
	SAFE_DELETE(m_pGuildWarZoneMapDlg);
	SAFE_DELETE(m_pGuildPropInfoDlg);
	SAFE_DELETE( m_pPvPKilledMeDlg );
	SAFE_DELETE( m_pPvPRacingResultDlg );
	SAFE_DELETE(m_pPVPGameResultAllDlg);
	SAFE_DELETE(m_pGuildWarSituation);
	SAFE_DELETE(m_pGuildWarBossGaugeDlg);
	SAFE_DELETE(m_pGuildWarResultDlg);

#if defined( PRE_REMOVE_GUILD_WAR_UI )
	CDnSkillTreeDlg* pSkillTreeDlg = static_cast<CDnSkillTreeDlg*>( GetInterface().GetMainMenuDialog( CDnMainMenuDlg::SKILL_DIALOG ) );
	if( pSkillTreeDlg )
		pSkillTreeDlg->SetGuildSkillMode( true, false );
#endif	// #if defined( PRE_REMOVE_GUILD_WAR_UI )

	if( m_pMainBar )
		m_pMainBar->ShowGuildWarSkillSlotDlg( false, false );

	CEtUIDialog* pDnNotifierDialog = GetInterface().GetMainMenuDialog( CDnMainMenuDlg::NOTIFIER_DIALOG );
	if( pDnNotifierDialog )
		pDnNotifierDialog->Show( true );

	if( CDnGuildWarTask::IsActive() )
	{
		GetGuildWarTask().SetGuildWarState( PvPCommon::OccupationSystemState::End );
		GetGuildWarTask().ResetGuildWarSkillPoint();
	}

#ifdef PRE_ADD_PVP_TOURNAMENT
	SAFE_DELETE(m_pPVPTournamentGMatchListDlg);
	SAFE_DELETE(m_pPVPTournamentGMatchItemDlg);
	SAFE_DELETE(m_pPVPTournamentFinalResultDlg);
#endif
#ifdef PRE_WORLDCOMBINE_PVP
	SAFE_DELETE( m_pWorldPVPRoomStartDlg );
#endif // PRE_WORLDCOMBINE_PVP
	OutputDebug( "CDnInterface::FinalizePvPGame()" );
}
//blondy end

void CDnInterface::FinalizeDLGame()
{
	FinalizeGame();
	SAFE_DELETE( m_pDarkLairEnterDlg );
	SAFE_DELETE( m_pDarkLairRoundCountDlg );
	SAFE_DELETE( m_pDarkLairInfoDlg );
	SAFE_DELETE( m_pDarkLairClearReportDlg );
	SAFE_DELETE( m_pDarkLairClearRankDlg );
	SAFE_DELETE( m_pDarkLairClearRewardDlg );
}

void CDnInterface::FinalizeFarmGame()
{
	FinalizeGame();
	SAFE_DELETE( m_pLifeTooltipDlg );
	SAFE_DELETE( m_pLifeConditionDlg );
	SAFE_DELETE( m_pLifeSlotDlg );

	m_pPlayerGaugeDlg->FinalizeFarm();
}

void CDnInterface::FinalizeCommon()
{
	for( DWORD i=0; i<(int)m_pVecPartyPlayerGaugeList.size(); i++ ) 
	{
		SAFE_DELETE( m_pVecPartyPlayerGaugeList[i].pGaugeDlg );
		m_pVecPartyPlayerGaugeList[i].hActor.Identity();
	}
	SAFE_DELETE_VEC( m_pVecPartyPlayerGaugeList );

#ifdef PRE_ADD_IMPROVE_MASTER_OBSERVER
	for( DWORD i=0; i<(int)m_pVecPartyPlayerRightGageList.size(); i++ ) 
	{
		SAFE_DELETE( m_pVecPartyPlayerRightGageList[i].pGaugeDlg );
		m_pVecPartyPlayerRightGageList[i].hActor.Identity();
	}
	SAFE_DELETE_VEC( m_pVecPartyPlayerRightGageList );
#endif

	SAFE_DELETE_PVEC( m_pVecCommonDialogList );
	SAFE_DELETE_VEC( m_pVecOpenBaseDialogList);

	//rlkt_test
	SAFE_DELETE (m_pJobChangeDlg );
	SAFE_DELETE(m_pItemDisjointResultDlg);
	InitCommonDlg();

	OutputDebug( "CDnInterface::FinalizeCommon\n" );
}

void CDnInterface::InitCommonDlg()
{
	m_pMainBar = NULL;
	m_pMainMenuDlg = NULL;
	m_pChatDlg = NULL;
	m_pTooltipDlg = NULL;
	m_pNpcDlg = NULL;
	m_pNoticeDlg = NULL;
	m_pPlayerGaugeDlg = NULL;
	m_pCaptionDlg_01 = NULL;
	m_pCaptionDlg_02 = NULL;
	m_pCaptionDlg_03 = NULL;
	m_pCaptionDlg_04 = NULL;
	m_pCaptionDlg_05 = NULL;
	m_pCaptionDlg_06 = NULL;
#ifdef PRE_ADD_NEWCOMEBACK
	m_pCaptionDlg_07 = NULL; // PRE_ADD_NEWCOMEBACK
#endif // PRE_ADD_NEWCOMEBACK
	m_pItemAlarmDlg = NULL;
	m_pDirectDlg = NULL;
	m_pInspectPlayerDlg = NULL;
	m_pGuildCreateDlg = NULL;
	m_pGuildYesNoDlg = NULL;
	m_pGuildInviteReqDlg = NULL;
	m_pPrivateChatDlg = NULL;
	m_pGateQuestionDlg = NULL;
	m_pChannelMoveDlg = NULL;
	m_pPrivateMarketReqDlg = NULL;
	m_pAcceptRequestDlg = NULL;
	m_pProgressDlg = NULL;
	m_pProgressMBox = NULL;
	m_pMovieProcessDlg = NULL;
	m_pFocusTypeMovieProcessDlg = NULL;
	m_pMissionAchieveAlarmDlg = NULL;
	m_pMissionGainAlarmDlg = NULL;
	m_pSlideCaptionDlg = NULL;
	m_pGameTipDlg = NULL;
	m_pHelpKeyboardDlg = NULL;
	m_pDungeonOpenNoticeDlg = NULL;
	m_pSplitConfirmExDlg = NULL;
	m_pCashShopMenuDlg = NULL;
	m_pWorldMsgDlg = NULL;
	m_pSkillResetConfirmDlg = NULL;
	m_pItemUnsealDlg = NULL;
	m_pItemSealDlg = NULL;
	m_pItemPotentialDlg = NULL;
	m_pItemPetalTokenDlg = NULL;
	m_pItemAppellationGainDlg =  NULL;
	m_pMODDlg = NULL;
	m_pHelpAlarmDlg = NULL;
	m_pBossAlertDlg = NULL;
	m_pNameLinkToolTipDlg = NULL;
	m_pNpcTalkReturnDlg = NULL;
	m_pEnchantJewelDlg = NULL;
	m_pGuildMarkCreateDlg = NULL;
#ifdef PRE_MOD_POTENTIAL_JEWEL_RENEWAL
	m_pItemPotentialCleanDlg = NULL;
#endif // PRE_MOD_POTENTIAL_JEWEL_RENEWAL

#ifdef PRE_ADD_GACHA_JAPAN
	m_pGachaDlg = NULL;
#endif // #ifdef PRE_ADD_GACHA_JAPAN
	m_pChatRoomDlg = NULL;
	m_pChatRoomPassWordDlg = NULL;
	m_pCharmItemKeyDlg = NULL;
	m_pCharmItemProgressDlg = NULL;
	m_pCharmItemResultDlg = NULL;
#if defined( PRE_ADD_EASYGAMECASH )
	m_pPackageBoxResultDlg = NULL;
	m_pPackageBoxProgressDlg = NULL;
#endif	// #if defined( PRE_ADD_EASYGAMECASH )

#ifdef PRE_ADD_AUTOUNPACK
	m_pAutoUnPackDlg = NULL;
	m_pAutoUnPackResultDlg = NULL;
#endif

#ifdef PRE_ADD_CASHREMOVE
	m_pCashItemRemoveDlg = NULL;
#endif

#ifdef PRE_ADD_CHAOSCUBE
	m_pChaosCubeDlg = NULL;
	m_pChaosCubeResultDlg = NULL;
	m_pChaosCubeStuffDlg = NULL;
	m_pChaosCubeProgressDlg = NULL;
#endif

#ifdef PRE_ADD_CHOICECUBE
	m_pChaosCubeDlg = NULL;
#endif

#ifdef PRE_ADD_BESTFRIEND
	m_pBFRegistrationDlg = NULL;
	m_pIdentifyBF = NULL;
	m_pBFProgressDlg = NULL;
	m_pBFRewardDlg = NULL;
	m_pBFRewardProgressDlg = NULL;
	m_pBFMemoDlg = NULL;
	m_serialBF = 0;
#endif

	m_pVoiceChatPremiumOptDlg = NULL;
	m_pDarkLairRankBoardDlg = NULL;
	m_pPVPLadderRankBoardDlg = NULL;


	// Guild War game
	m_pGuildWarPreliminaryStatusDlg = NULL;
	m_pGuildWarPreliminaryResultDlg = NULL;
	m_pGuildWarFinalTournamentListDlg = NULL;

	m_pGuildRenameDlg = NULL;
	m_pCharRenameDlg = NULL;
	m_pLifeSkillFishingDlg = NULL;
	m_pPetNamingDlg = NULL;
	m_pPetExtendPeriodDlg = NULL;
	m_pPetAddSkillDlg = NULL;
	m_pPetExpandSkillSlotDlg = NULL;
#ifdef PRE_ADD_PET_EXTEND_PERIOD
	m_pBuyPetExtendPeriodDlg = NULL;
#endif // PRE_ADD_PET_EXTEND_PERIOD
#ifdef PRE_ADD_CHANGEJOB_CASHITEM
	m_pChangeJobDlg = NULL;
#endif // #ifdef PRE_ADD_CHANGEJOB_CASHITEM
	m_pDnGuildWarFinalWinNoticeDlg = NULL;

#if defined(PRE_ADD_REMOVE_PREFIX)
	m_pRemovePrefixDlg = NULL;
#endif // PRE_ADD_REMOVE_PREFIX
	m_pNestInfoDlg = NULL;
	m_pGuildLevelUpAlarmDlg = NULL;
#ifdef PRE_ADD_EXCHANGE_POTENTIAL
	m_pPotentialTransferDlg = NULL;
#endif

#ifdef PRE_PRIVATECHAT_CHANNEL
	m_pDnPrivateChannelInviteDlg = NULL;
	m_pDnPrivateChannelPasswordDlg = NULL;
#ifdef PRE_ADD_PRIVATECHAT_CHANNEL
	m_pDnPrivateChannelPasswordChnageDlg = NULL;
#endif // PRE_ADD_PRIVATECHAT_CHANNEL
#endif // PRE_PRIVATECHAT_CHANNEL

#ifdef PRE_FIX_ESCAPE
	m_pEscapeDlg = NULL;
#endif

	m_pWorldZoneSelectDlg = NULL;

#ifdef PRE_ADD_INSTANT_CASH_BUY
	m_pDnInstantCashShopBuyDlg = NULL;
#endif // PRE_ADD_INSTANT_CASH_BUY

#ifdef PRE_ADD_NEWCOMEBACK
	m_pComeBackRewardDlg = NULL;
	m_pComebackMsgDlg = NULL;
#endif // PRE_ADD_NEWCOMEBACK

#ifdef PRE_ADD_PVP_RANKING
	m_pPvPRankDlg = NULL;
#endif // PRE_ADD_PVP_RANKING

	m_pMapMoveCashItemDlg = NULL;

#ifdef PRE_ADD_DIRECT_BUY_UPGRADEITEM
	m_pMarketDirectBuyDlg = NULL;
#endif

#ifdef PRE_ADD_ALTHEIA_NEWCAPTION_TRIGGER
	m_ImageBlindDlg = NULL;
#endif

#ifdef PRE_ADD_STAMPSYSTEM
	m_pStampEventAlarmDlg = NULL;
#endif // PRE_ADD_STAMPSYSTEM

#ifdef PRE_ADD_GAMEQUIT_REWARD
	m_pGameQuitRewardDlg = NULL;
	m_pGameQuitNextTimeRewardDlg = NULL;
	m_pGameQuitNextTimeRewardComboDlg = NULL;
#endif // PRE_ADD_GAMEQUIT_REWARD

#ifdef PRE_ADD_FADE_TRIGGER
	m_pFadeCaptionDlg = NULL;
#endif // PRE_ADD_FADE_TRIGGER

#ifdef PRE_ADD_DWC
	m_pDWCTeamCreateDlg = NULL;
	m_pDWCInviteReqDlg = NULL;
#endif // PRE_ADD_DWC

#ifdef PRE_ADD_PVPRANK_INFORM
	m_pPvPRankInformDlg = NULL;
#endif

#ifdef PRE_ADD_MAINQUEST_UI
	m_pMainQuestDlg = NULL;
#endif

#ifdef PRE_ADD_DRAGON_GEM
	m_pDragonGemEquipDlg  = NULL;
	m_pDragonGemRemoveDlg = NULL;
#endif


	//rlktInstant JOb
	m_pInstantSpecializationDlg = NULL;
}

//blondymarry start
void CDnInterface::InitPVPLobbyDlg()
{
	m_pPVPRoomListDlg = NULL;
	m_pPVPGameRoomDlg = NULL;
	m_pGuildWarRoomDlg = NULL;
#ifdef PRE_ADD_PVP_TOURNAMENT
	m_pPVPTournamentRoomDlg = NULL;
	m_pPVPTournamentMatchListDlg = NULL;
#endif
	m_pPVPChatDlg = NULL;
	m_pPVPLadderSystemDlg = NULL;
	m_pPVPLadderTabDlg = NULL;

#if defined( PRE_ADD_REVENGE )
	m_pPVPRevengeMessageBox = NULL;
#endif	// #if defined( PRE_ADD_REVENGE )

#ifdef PRE_ADD_PVP_RANKING
	m_pPvPRankDlg = NULL;
#endif // PRE_ADD_PVP_RANKING

#if defined(PRE_ADD_DWC)
	m_pPVPDWCTablDlg = NULL;
#endif // PRE_ADD_DWC

#ifdef PRE_ADD_PVPRANK_INFORM
	m_pPvPRankInformDlg = NULL;
#endif
}
//blondymarry end

void CDnInterface::PreInitialize()
{
	if( !m_pMessageBox )
	{
		m_pMessageBox = new CDnMessageBox( UI_TYPE_MODAL );
		m_pMessageBox->Initialize( false );
	}

	if( !m_pMessageBoxTextBox )
	{
		m_pMessageBoxTextBox = new CDnMessageBoxTextBox( UI_TYPE_MODAL );
		m_pMessageBoxTextBox->Initialize( false );
	}

	if( !m_pBigMessageBox )
	{
		m_pBigMessageBox = new CDnBigMessageBox( UI_TYPE_MODAL );
		m_pBigMessageBox->Initialize( false );
	}

	if( !m_pMiddleMessageBox )
	{
		m_pMiddleMessageBox = new CDnMiddleMessageBox( UI_TYPE_MODAL );
		m_pMiddleMessageBox->Initialize( false );
	}

	if( !m_pAssertDlg )
	{
		m_pAssertDlg = new CDnAssertDlg( UI_TYPE_MODAL );
		m_pAssertDlg->Initialize( false );
	}

	if( !m_pTextureDlgMng )
	{
		m_pTextureDlgMng = new CDnTextureDlgMng;
	}

	if( !m_pTextBoxDlgMng )
	{
		m_pTextBoxDlgMng = new CDnTextBoxDlgMng;
	}

	if( !m_pAcceptDlg )
	{
		m_pAcceptDlg = new CDnAcceptDlg( UI_TYPE_TOP_MSG );
		m_pAcceptDlg->Initialize( false );
	}

	if( !m_pSimpleTooltipDlg )
	{
		m_pSimpleTooltipDlg = new CDnSimpleTooltipDlg( UI_TYPE_MOST_TOP );
		m_pSimpleTooltipDlg->Initialize( false );
		CEtUIDialog::SetTooltipDlg( m_pSimpleTooltipDlg );
	}

	if( !m_pBlindDlg )
	{
		m_pBlindDlg = new CDnBlindDlg( UI_TYPE_BOTTOM, NULL );
		m_pBlindDlg->Initialize( false );
	}

	if( !m_pFadeForBlowDlg )
	{
		m_pFadeForBlowDlg = new CDnFadeForBlowDlg( UI_TYPE_BOTTOM );
		m_pFadeForBlowDlg->Initialize( true );
		CEtUIDialog::s_pFadeDlg = m_pFadeForBlowDlg;
	}

	if( !m_pFadeDlg )
	{
		m_pFadeDlg = new CDnFadeInOutDlg( UI_TYPE_MOST_TOP, NULL, FADE_DIALOG);
		m_pFadeDlg->Initialize( true );
	}

#ifdef PRE_ADD_FADE_TRIGGER
	if( !m_pFadeCaptionDlg )
	{
		m_pFadeCaptionDlg = new CDnFadeCaptionDlg( UI_TYPE_MOST_TOP, NULL );
		m_pFadeCaptionDlg->Initialize( false );
	}
#endif // PRE_ADD_FADE_TRIGGER

	if (!m_pProgressSimpleMsgBox)
	{
		m_pProgressSimpleMsgBox = new CDnProgressSimpleMessageBox(UI_TYPE_MOST_TOP);
		m_pProgressSimpleMsgBox->Initialize(false);
	}

#ifdef PRE_ADD_LEVELUP_GUIDE
	if (!m_pLevelUpGuildeDlg)
	{
		m_pLevelUpGuildeDlg = new CDnLevelUpGuideDlg(UI_TYPE_MOST_TOP);
		m_pLevelUpGuildeDlg->Initialize(false);
	}
#endif

	if( !m_pSecurityKBDDlg )
	{
		m_pSecurityKBDDlg = new CDnSecurityKBDDlg( UI_TYPE_MODAL );
		m_pSecurityKBDDlg->Initialize( false  );
	}

	if( !m_pSecurityChangeDlg )
	{
		m_pSecurityChangeDlg = new CDnSecurityNUMPADChangeDlg( UI_TYPE_MODAL);
		m_pSecurityChangeDlg->Initialize( false );
	}

	if( !m_pSecurityCheckDlg )
	{
		m_pSecurityCheckDlg = new CDnSecurityNUMPADCheckDlg( UI_TYPE_MODAL) ;
		m_pSecurityCheckDlg->Initialize(  false );
	}

	if( !m_pSecurityCreateDlg )
	{
		m_pSecurityCreateDlg = new CDnSecurityNUMPADCreateDlg( UI_TYPE_MODAL );
		m_pSecurityCreateDlg->Initialize( false  );
	}
	if( !m_pSecurityDeleteDlg )
	{
		m_pSecurityDeleteDlg = new CDnSecurityNUMPADDeleteDlg( UI_TYPE_MODAL );
		m_pSecurityDeleteDlg->Initialize( false );
	}

#ifdef PRE_ADD_ACTIVEMISSION
	if( !m_pActiveMissionDlg )
	{
		m_pActiveMissionDlg = new CDnAcMissionAlarmDlg( UI_TYPE_FOCUS );
		m_pActiveMissionDlg->Initialize( false );
	}	
#endif // PRE_ADD_ACTIVEMISSION

	MakeCpShowList();

	if (!m_pNameLinkMng)
		m_pNameLinkMng = new CDnNameLinkMng();

	int i = CONTEXTM_MIN;
	for (; i < CONTEXTM_MAX; ++i)
	{
		CDnContextMenuDlg* pContextMenuDlg = NULL;
		if (i == CONTEXTM_PARTY)		pContextMenuDlg = new CDnPartyContextMenuDlg(UI_TYPE_MODAL, NULL, (eContextMenuType)i);	// todo by kalliste : make factory	
		else if (i == CONTEXTM_CHAT)	pContextMenuDlg = new CDnChatContextMenuDlg(UI_TYPE_MODAL, NULL, (eContextMenuType)i);
		else
		{
			_ASSERT(0);
			break;
		}
		pContextMenuDlg->Initialize(false);
		m_pContextMenuDlgList.insert(std::make_pair(i, pContextMenuDlg));
	}

	OutputDebug( "CDnInterface::PreInitialize\n" );
}

void CDnInterface::PostFinalize()
{
	SAFE_DELETE(m_pMessageBox);
	SAFE_DELETE(m_pMessageBoxTextBox);
	SAFE_DELETE(m_pBigMessageBox );
	SAFE_DELETE(m_pMiddleMessageBox);
	SAFE_DELETE(m_pAssertDlg);
	SAFE_DELETE(m_pAcceptDlg);
	SAFE_DELETE(m_pFadeForBlowDlg);
	SAFE_DELETE(m_pFadeDlg);
	SAFE_DELETE(m_pBlindDlg);
	SAFE_DELETE(m_pTextureDlgMng);
	SAFE_DELETE(m_pTextBoxDlgMng);
	SAFE_DELETE(m_pSimpleTooltipDlg);
	SAFE_DELETE(m_pProgressSimpleMsgBox);
#ifdef PRE_ADD_LEVELUP_GUIDE
	SAFE_DELETE(m_pLevelUpGuildeDlg);
#endif
	SAFE_DELETE(m_pSecurityKBDDlg);
	SAFE_DELETE(m_pSecurityCreateDlg);
	SAFE_DELETE(m_pSecurityCheckDlg);
	SAFE_DELETE(m_pSecurityChangeDlg);
	SAFE_DELETE( m_pSecurityDeleteDlg );
#ifdef PRE_ADD_FADE_TRIGGER
	SAFE_DELETE( m_pFadeCaptionDlg );
#endif // PRE_ADD_FADE_TRIGGER

#ifdef PRE_ADD_ACTIVEMISSION
	SAFE_DELETE( m_pActiveMissionDlg );
#endif // PRE_ADD_ACTIVEMISSION
	if (m_pContextMenuDlgList.empty() == false)
	{
		std::map<int, CDnContextMenuDlg*>::iterator iter = m_pContextMenuDlgList.begin();
		for (; iter != m_pContextMenuDlgList.end(); ++iter)
		{
			CDnContextMenuDlg* pDlg = (*iter).second;
			if (pDlg)
				SAFE_DELETE(pDlg);
		}

		m_pContextMenuDlgList.clear();
	}

	OutputDebug( "CDnInterface::PostFinalize\n" );
}

void CDnInterface::Initialize( InterfaceTypeEnum Type, CTask *pTask )
{
	switch( Type )
	{
	case Village:

		//인터페이스 이니셜라이징이 구조가 잘못되있음 기본적으로 이니셜라이징까지 모두 가야 하는데 중간에 리턴하는 경우가 있음
		InitializeVillage( pTask );
		CloseLobbyDialog();
	case Game:
	case PVPGame:
	case DLGame:
	case Farm:
		{
			CloseBlind();
			ClearListDialog();
			RefreshPartyGateInfo();
			OpenBaseDialog();
			CloseAllMainMenuDialog();

		}
		break;
	case Title:
	case Login:
		{
			CloseAcceptDialog();
			ClosePrivateMarketReqDialog();
			EtInterface::g_bEtUIRender = true;
			CEtCustomRender::s_bEnableCustomRender = true;
		}
		break;
	case PVPVillage:
		CloseBlind();
		ClearListDialog();
		CloseAllMainMenuDialog();
		CloseMessageBox();
		EtInterface::g_bEtUIRender = true;
		CEtCustomRender::s_bEnableCustomRender = true;
		break;
	}	

	if( m_Type == Type )
	{
		OutputDebug( "CDnInterface::Initialize, m_Type == Type\n" );
		return;
	}

	InterfaceTypeEnum PrevType(m_Type);

	if((m_Type==Village) || (m_Type==Game) || (m_Type==PVPVillage) || (m_Type==PVPGame) || (m_Type==DLGame) || (m_Type==Farm))
	{
		PrevType = Common;
	}

	m_Type = Type;
	m_pTask = pTask;
	m_szTaskName = m_pTask->GetTaskName();

	static void (CDnInterface::*fp[InterfaceTypeMax])( CTask *pTask ) =
	{
		&CDnInterface::InitializeTitle,
		&CDnInterface::InitializeLogin,
		&CDnInterface::InitializeVillage,
		&CDnInterface::InitializePVPVillage,
		&CDnInterface::InitializeGame,
		&CDnInterface::InitializePVPGame,
		&CDnInterface::InitializeDLGame,
		&CDnInterface::InitializeFarmGame,
		&CDnInterface::InitializeCommon,
	};

	switch( m_Type )
	{
	case Title:
	case Login:	
		if( PrevType == Common )
		{
			// Note : Village, Game에서 나오면 Common을 Finalize해준다.
			//
			Finalize( Common );
		}
		break;
	case Village:
	case Game:
	case PVPGame:
	case PVPVillage:
	case DLGame:
	case Farm:
		if( PrevType != Common )
		{
			// Note : Village, Game이 아니면 Common을 Initialize한다.
			//
			(this->*fp[InterfaceTypeEnum::Common])(pTask);
		}
		break;
	}

	(this->*fp[m_Type])(pTask);

	DisableCashShopMenuDlg(false);

#ifdef PRE_MOD_SYSTEM_STATE
	m_SystemStateMgr.Clear();
#endif
}

//blondymarry start
void CDnInterface::SetPVPVillageMenu( bool isPVP  )
{
	if(m_pMainBar)
		m_pMainBar->SetPVPMenu( isPVP );
}
//blondymarry end

void CDnInterface::InitializeTitle( CTask *pTask )
{
	CEtUIDialog *pDlg = NULL;

	pDlg = new CEtUIDialog( UI_TYPE_TOP );
	pDlg->Initialize( CEtResourceMng::GetInstance().GetFullName( "CompanyLogo.ui" ).c_str(), false );
	m_pVecLogoList.push_back( pDlg );

	pDlg = NULL;

	pDlg = new CEtUIDialog( UI_TYPE_TOP );
	pDlg->Initialize( CEtResourceMng::GetInstance().GetFullName( "NoticeLogo.ui" ).c_str(), false );
	m_pVecLogoList.push_back( pDlg );

	m_nCurLogoIndex = -1;
	m_fLogoDelta = 0.f;

	OutputDebug( "CDnInterface::InitializeTitle\n" );
}

void CDnInterface::InitializeLogin( CTask *pTask )
{
	m_pLoginDlg = new CDnLoginDlg;
	m_pLoginDlg->Initialize( false );
#ifndef PRE_MOD_SELECT_CHAR
	m_pCharSelectDlg = new CDnCharSelectDlg;
	m_pCharSelectDlg->Initialize( false );
#endif // PRE_MOD_SELECT_CHAR
	m_pCharCreateDlg = new CDnCharCreateDlg;
	m_pCharCreateDlg->Initialize( false );

	m_pCharCreateBackDlg = new CDnCharCreateBackDlg;
	m_pCharCreateBackDlg->Initialize( false );

	m_pCharCreateServerNameDlg = new CDnCharCreateServerNameDlg;
	m_pCharCreateServerNameDlg->Initialize( false );

	m_pServerListDlg = new CDnServerListDlg;
	m_pServerListDlg->Initialize( false );	

	m_pWaitUserDlg = new CDnWaitUserDlg( UI_TYPE_MODAL );
	m_pWaitUserDlg->Initialize( false );

	m_pChannelListDlg = new CDnChannelListDlg;
	m_pChannelListDlg->Initialize( false );
	m_pSecondPassCheck = new CDnSecondPassCheck( UI_TYPE_MODAL );
	m_pSecondPassCheck->Initialize( false );

	m_pStaffrollDlg = new CDnStaffrollDlg;
	m_pStaffrollDlg->Initialize( false );
#if defined(_KRAZ) || defined(_WORK)
	m_pStaffrollDlg->Show( false );
#endif

#ifdef PRE_ADD_DOORS
	m_pDoorsMobileAuthDlg = new CDnDoorsMobileAuthButtonDlg;
	m_pDoorsMobileAuthDlg->Initialize(false);
	#ifdef PRE_REMOVE_DOORS_UITEMP
	m_pDoorsMobileAuthDlg->Show(false);
	#endif
#endif

#if defined(_CH) && defined(_AUTH)
	m_pSDOUADlg = new CDnSDOUADlg( UI_TYPE_MODAL );
	m_pSDOUADlg->Initialize( false );
#endif

#if defined(_TH) && defined(_AUTH)
	m_pTHOTPDlg = new CDnTHOTPDlg( UI_TYPE_MODAL );
	m_pTHOTPDlg->Initialize( false );
#endif	// _TH && _AUTH

#ifdef PRE_MOD_SELECT_CHAR
	m_pCharLoginTitleDlg = new CDnCharLoginTitleDlg;
	m_pCharLoginTitleDlg->Initialize( false );
	m_pCharCreatePlayerInfo = new CDnCharCreatePlayerInfo;
	m_pCharCreatePlayerInfo->Initialize( false );
	m_pCharCreateSelectDlg = new CDnCharCreateSelectDlg;
	m_pCharCreateSelectDlg->Initialize( false );
	m_pCharRotateDlg = new CDnCharRotateDlg;
	m_pCharRotateDlg->Initialize( false );
	m_pCharGestureDlg = new CDnCharGestureDlg;
	m_pCharGestureDlg->Initialize( false );
	m_pCharSelectDlg = new CDnCharSelectDlg;
	m_pCharSelectDlg->Initialize( false );

#ifdef _ADD_NEWLOGINUI
    m_pCharCreateSetupDlg = new CDnCharCreateSetupDlg;
    m_pCharCreateSetupDlg->Initialize(false);
    m_pCharCreateSetupGestureDlg = new CDnCharCreateSetupGestureDlg;
    m_pCharCreateSetupGestureDlg->Initialize(false);
    m_pCharCreateNameDlg = new CDnCharCreateNameDlg;
    m_pCharCreateNameDlg->Initialize(false);
    m_pCharCreateSetupBackDlg = new CDnCharCreateSetupBackDlg;
    m_pCharCreateSetupBackDlg->Initialize(false);
#endif

#else // PRE_MOD_SELECT_CHAR
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TDEFAULTCREATE );
	DNTableFileFormat* pActorSOX = GetDNTable( CDnTableDB::TACTOR );
	int nActorCount = pSox->GetItemCount();

	CEtUIDialog *pInfoDlg(NULL);
	char szLabel[32]={0};
	int nClassID(0);

	for( int i=1; i<=nActorCount; i++ ) 
	{
		nClassID = pSox->GetFieldFromLablePtr( i, "_ClassID" )->GetInteger();
		sprintf_s( szLabel, 32, "%sClassInfo.ui", pActorSOX->GetFieldFromLablePtr( nClassID, "_StaticName" )->GetString() );
		pInfoDlg = new CEtUIDialog( UI_TYPE_TOP );
		pInfoDlg->Initialize( CEtResourceMng::GetInstance().GetFullName( szLabel ).c_str(), false );
		m_pVecCreateInfoList.push_back( pInfoDlg );
	}

	m_pCharCreateTitleDlg = new CDnCharCreateTitleDlg;
	m_pCharCreateTitleDlg->Initialize( false );

	for( int i=0; i<SHOWCHARNUMPERONEPAGE; i++ )
	{
		m_pSlotInfoDlg[i] = new CDnPlayerInfoDlg( UI_TYPE_TOP, NULL, PLAYERINFO_DIALOG+i, dynamic_cast<CEtUICallback*>(pTask) );
		m_pSlotInfoDlg[i]->Initialize( false );

		m_pSlotDeleteWaitDlg[i] = new CDnCharDeleteWaitDlg( UI_TYPE_BOTTOM_MSG, NULL, PLAYERDELETEWAIT_DIALOG+i, dynamic_cast<CEtUICallback*>(pTask) );
		m_pSlotDeleteWaitDlg[i]->Initialize( false );
	}
#endif // PRE_MOD_SELECT_CHAR

#ifdef PRE_ADD_CHARSELECT_PREVIEW_CASHITEM 
	m_pCharSelectPreviewCostumeDlg = new CDnCharSelectCostumePreviewDlg;
	m_pCharSelectPreviewCostumeDlg->Initialize( false );
#endif //PRE_ADD_CHARSELECT_PREVIEW_CASHITEM

#ifdef PRE_ADD_DWC
	m_pDWCCharCreateDlg = new CDnDWCCharCreateDlg;
	m_pDWCCharCreateDlg->Initialize( false );
#endif // PRE_ADD_DWC

	//rlkt_dark
	m_pCharCreateDarkDlg = new CDnCharCreateDarkDlg;
	m_pCharCreateDarkDlg->Initialize( false );

	// ComeBack - 로그인창에서 툴팁표시.
	m_pTooltipDlg = new CDnTooltipDlg( UI_TYPE_TOP_MSG );
	m_pTooltipDlg->Initialize( false );

#ifdef PRE_ADD_GAMEQUIT_REWARD
	m_pGameQuitNextTimeRewardDlg = new CDnGameQuitNextTimeRewardDlg( UI_TYPE_MODAL, NULL, -1, NULL, true );
	m_pGameQuitNextTimeRewardDlg->Initialize( false );
	AddCommonDialogList( m_pGameQuitNextTimeRewardDlg );
#endif // PRE_ADD_GAMEQUIT_REWARD

	OutputDebug( "CDnInterface::InitializeLogin\n" );
}

//#ifndef _FINAL_BUILD
#include "DnActorClassDefine.h"
void CDnInterface::_TestUIDump( CTask *pTask )
{
//#if defined(_DEBUG) || defined(_RDEBUG)
	CEtUIDialog::s_bUITest = true;
//#endif

	FILE *fp = fopen( "uitest.txt", "wt" );
	fclose( fp );

	OutputDebug( "=========== _TEST_UI_DUMP - Start ===========\n" );

	DnActorHandle hPlayer = CreateActor( 1, true );
	CDnActor::s_hLocalActor = hPlayer;

	// UI 덤프나는지 테스트할땐
	// 아래 주석 풀고, public으로 설정 후
	// DnTitleTask에서 //GetInterface()._TestUIDump( this ); 찾아 주석 풀고 F5로 시작하면 된다.
	CDnBridgeTask::GetInstance().InitializeDefaultTask();

	CGameOption::GetInstance().m_bSmartMove = false;
	CGameOption::GetInstance().m_bSmartMoveMainMenu = false;

	OutputDebug( "_TEST_UI_DUMP - Start : InitializeVillage\n" );
	InitializeVillage( pTask );
	OutputDebug( "_TEST_UI_DUMP - End : InitializeVillage\n" );

	OutputDebug( "_TEST_UI_DUMP - Start : InitializeCommon\n" );
	InitializeCommon( pTask );
	OutputDebug( "_TEST_UI_DUMP - End : InitializeCommon\n" );

	OutputDebug( "_TEST_UI_DUMP - Start : FinalVillage\n" );
	Finalize( Village );
	OutputDebug( "_TEST_UI_DUMP - End : FinalVillage\n" );

	OutputDebug( "_TEST_UI_DUMP - Start : InitializeGame\n" );
	InitializeGame( pTask );
	OutputDebug( "_TEST_UI_DUMP - End : InitializeGame\n" );

	OutputDebug( "_TEST_UI_DUMP - Start : FinalizeGame\n" );
	Finalize( Game );
	OutputDebug( "_TEST_UI_DUMP - End : FinalizeGame\n" );

	OutputDebug( "_TEST_UI_DUMP - Start : InitializeVillage\n" );
	InitializeVillage( pTask );
	OutputDebug( "_TEST_UI_DUMP - End : InitializeVillage\n" );

	OutputDebug( "_TEST_UI_DUMP - Start : FinalVillage\n" );
	Finalize( Village );
	OutputDebug( "_TEST_UI_DUMP - End : FinalVillage\n" );

	OutputDebug( "_TEST_UI_DUMP - Start : InitializePVPVillage\n" );
	InitializePVPVillage( pTask );
	OutputDebug( "_TEST_UI_DUMP - End : InitializePVPVillage\n" );

	OutputDebug( "_TEST_UI_DUMP - Start : FinalPVPVillage\n" );
	Finalize( PVPVillage );
	OutputDebug( "_TEST_UI_DUMP - End : FinalPVPVillage\n" );

	OutputDebug( "_TEST_UI_DUMP - Start : CreatePvpGameTask\n" );
	CDnGameTask	*pTestGameTask = new CDnPvPGameTask();
	OutputDebug( "_TEST_UI_DUMP - End : CreatePvpGameTask\n" );

	OutputDebug( "_TEST_UI_DUMP - Start : InitializePVPGame\n" );
	InitializePVPGame( pTestGameTask );
	OutputDebug( "_TEST_UI_DUMP - End : InitializePVPGame\n" );

	OutputDebug( "_TEST_UI_DUMP - Start : FinalizePVPGame\n" );
	Finalize( PVPGame );
	OutputDebug( "_TEST_UI_DUMP - End : FinalizePVPGame\n" );

	SAFE_DELETE( pTestGameTask );

	OutputDebug( "_TEST_UI_DUMP - Start : InitializeDLGame\n" );
	InitializeDLGame( pTask );
	OutputDebug( "_TEST_UI_DUMP - End : InitializeDLGame\n" );

	OutputDebug( "_TEST_UI_DUMP - Start : FinalizeDLGame\n" );
	Finalize( DLGame );
	OutputDebug( "_TEST_UI_DUMP - End : FinalizeDLGame\n" );

	OutputDebug( "_TEST_UI_DUMP - Start : InitializeFarmGame\n" );
	InitializeFarmGame( pTask );
	OutputDebug( "_TEST_UI_DUMP - End : InitializeFarmGame\n" );

	OutputDebug( "_TEST_UI_DUMP - Start : FinalizeFarmGame\n" );
	Finalize( Farm );
	OutputDebug( "_TEST_UI_DUMP - End : FinalizeFarmGame\n" );

	OutputDebug( "_TEST_UI_DUMP - Start : FinalizeCommon\n" );
	Finalize( Common );
	OutputDebug( "_TEST_UI_DUMP - End : FinalizeCommon\n" );

	SAFE_RELEASE_SPTR( hPlayer );

	// 브레이크 걸릴테니 Shift+F5로 종료.
	//DebugBreak();
	// /uitest 설정으로 빼면서 이렇게 할필요 없다.

	OutputDebug( "=========== _TEST_UI_DUMP - End ===========\n" );

//#if defined(_DEBUG) || defined(_RDEBUG)
	CEtUIDialog::s_bUITest = false;
//#endif
}
//#endif	// #ifndef _FINAL_BUILD

void CDnInterface::InitializeVillage( CTask *pTask )
{
	CDnVillageTask *pVillageTask = NULL;
	pVillageTask = dynamic_cast<CDnVillageTask *>(pTask);
	if( pVillageTask )
		SetPVPVillageMenu( ( pVillageTask->GetVillageType() == CDnVillageTask::PvPVillage ) ? true : false );

#ifdef PRE_ADD_PVP_VILLAGE_ACCESS
	if(pVillageTask && pVillageTask->GetVillageType() != CDnVillageTask::PvPLobbyVillage)
	{
		if( !m_pPVPRoomListDlg )
		{
			m_pPVPRoomListDlg = new CDnPVPRoomListDlg( UI_TYPE_FOCUS , NULL , -1 , NULL , true ); 
			m_pPVPRoomListDlg->EnableVillageAccessMode(true);
			m_pPVPRoomListDlg->Initialize(false);
		}

		if(!m_pPVPLadderTabDlg && m_pPVPRoomListDlg )
		{
			m_pPVPLadderTabDlg = new CDnPVPLadderTabDlg( UI_TYPE_FOCUS );
			m_pPVPLadderTabDlg->Initialize(false);
			m_pPVPLadderTabDlg->EnableVillageAccessMode(true);
		}
	
#ifdef PRE_ADD_COMEBACK
		if( m_bComeback == false )
		{
			CDnBridgeTask *pTask = (CDnBridgeTask *)CTaskManager::GetInstance().GetTask( "BridgeTask" );
			m_bComeback = pTask->GetComeback();
		}
#endif
	}
#endif

#ifdef PRE_ADD_PVP_RANKING
	if( m_pPvPRankDlg == NULL )
	{
		m_pPvPRankDlg = new CDnPvPRankDlg( UI_TYPE_FOCUS, NULL, -1, m_pMainMenuDlg, true );
		m_pPvPRankDlg->Initialize( false );	
		//AddCommonDialogList( m_pPvPRankDlg );
	}
#endif // PRE_ADD_PVP_RANKING

#ifdef PRE_ADD_STAGE_USECOUNT_ITEM
	CDnCharInventory& Inventory = GetItemTask().GetCharInventory();
	Inventory.RefreshInventory();
#endif

#ifdef PRE_ADD_PVPRANK_INFORM
	if (m_pPvPRankInformDlg == NULL)
	{
		m_pPvPRankInformDlg = new CDnPvPRankInformDlg(UI_TYPE_MODAL, NULL, -1, NULL, true);
		m_pPvPRankInformDlg->Initialize(false);
	}
#endif

	OutputDebug( "CDnInterface::InitializeVillage\n" );
}

void CDnInterface::InitializePVPVillage( CTask *pTask )
{
	if( !m_pPVPRoomListDlg )
	{
		m_pPVPRoomListDlg = new CDnPVPRoomListDlg( UI_TYPE_FOCUS ); 
		m_pPVPRoomListDlg->Initialize(true);
	}

	if(!m_pPVPGameRoomDlg)
	{
		m_pPVPGameRoomDlg = new CDnPVPGameRoomDlg();
		m_pPVPGameRoomDlg->Initialize(false);
	}
	if(!m_pGuildWarRoomDlg)
	{
		m_pGuildWarRoomDlg = new CDnGuildWarRoomDlg();
		m_pGuildWarRoomDlg->Initialize(false);
	}
#ifdef PRE_ADD_PVP_TOURNAMENT
	if (m_pPVPTournamentRoomDlg == NULL)
	{
		m_pPVPTournamentRoomDlg = new CDnPVPTournamentRoomDlg();
		m_pPVPTournamentRoomDlg->Initialize(false);
	}

	if (m_pPVPTournamentMatchListDlg == NULL)
	{
		m_pPVPTournamentMatchListDlg = new CDnPVPTournamentRoomMatchListDlg();
		m_pPVPTournamentMatchListDlg->Initialize(false);
	}

	if (m_pPVPTournamentRoomDlg && m_pPVPTournamentMatchListDlg)
	{
		m_pPVPTournamentRoomDlg->SetMatchListDlg(m_pPVPTournamentMatchListDlg);
		m_pPVPTournamentMatchListDlg->SetBaseDlg(m_pPVPTournamentRoomDlg);
	}
#endif

	if(!m_pPVPLadderSystemDlg)
	{
		m_pPVPLadderSystemDlg = new CDnPVPLadderSystemDlg( UI_TYPE_FOCUS );
		m_pPVPLadderSystemDlg->Initialize(false);
	}

	if(!m_pPVPLadderTabDlg)
	{
		m_pPVPLadderTabDlg = new CDnPVPLadderTabDlg( UI_TYPE_FOCUS );
#if defined(PRE_ADD_DWC)
		m_pPVPLadderTabDlg->Initialize(false);
#else
		m_pPVPLadderTabDlg->Initialize(true);
#endif
	}

#if defined(PRE_ADD_DWC)
	if(!m_pPVPDWCTablDlg)
	{
		m_pPVPDWCTablDlg = new CDnPVPDWCTabDlg(UI_TYPE_FOCUS);
		m_pPVPDWCTablDlg->Initialize(false);
	}
#endif // #if defined(PRE_ADD_DWC)

#if defined( PRE_ADD_REVENGE )
	if( NULL == m_pPVPRevengeMessageBox )
	{
		m_pPVPRevengeMessageBox = new CDnPVPRevengeMessageBoxDlg( UI_TYPE_MODAL );
		m_pPVPRevengeMessageBox->Initialize( false );
		m_pPVPRevengeMessageBox->Show( false );
	}
#endif	// #if defined( PRE_ADD_REVENGE )

#ifdef PRE_ADD_PVP_RANKING
	if( m_pPvPRankDlg == NULL )
	{
		m_pPvPRankDlg = new CDnPvPRankDlg( UI_TYPE_MODAL );
		m_pPvPRankDlg->Initialize( false );	
		//AddCommonDialogList( m_pPvPRankDlg );
	}
#endif // PRE_ADD_PVP_RANKING

#ifdef PRE_ADD_PVPRANK_INFORM
	if (m_pPvPRankInformDlg == NULL)
	{
		m_pPvPRankInformDlg = new CDnPvPRankInformDlg(UI_TYPE_MODAL);
		m_pPvPRankInformDlg->Initialize(false);
	}
#endif

	HideCommonDialog();
	m_pPVPChatDlg->Show( true );

	CDnMouseCursor::GetInstance().ShowCursor(true);	
    OutputDebug( "CDnInterface::InitializePVPVillage\n" );
}

void CDnInterface::InitializeCommon( CTask *pTask )
{
	CreatePlayerGauge();

	m_pMainBar = new CDnMainDlg( UI_TYPE_BOTTOM_MSG );
	m_pMainBar->Initialize( true );
	AddCommonDialogList(m_pMainBar);
	m_pVecOpenBaseDialogList.push_back(m_pMainBar);

	//rlkt_instant spec
	m_pInstantSpecializationDlg = new CDnInstantSpecializationDlg(UI_TYPE_FOCUS);
	m_pInstantSpecializationDlg->Initialize(false);
	m_pInstantSpecializationDlg->Show(true);
	m_pVecOpenBaseDialogList.push_back(m_pInstantSpecializationDlg);


	m_pChatDlg = new CDnChatTabDlg( UI_TYPE_FOCUS );
	m_pChatDlg->Initialize( false );
	m_pChatDlg->SetPassMessageToChild( true );
	AddCommonDialogList(m_pChatDlg);

	// 채팅내용을 유지하기위해 pvp드나들때 복사할까 하다가
	// 그냥 항상 생성된 상태로 유지하면서 show/hide 시키는게 훨씬 간단해서 이렇게 하기로 한다.
	m_pPVPChatDlg = new CDnPVPLobbyChatTabDlg( UI_TYPE_FOCUS );
	m_pPVPChatDlg->Initialize( false );
	m_pPVPChatDlg->SetPassMessageToChild( true );
	AddCommonDialogList(m_pPVPChatDlg);

	m_pMainMenuDlg = new CDnMainMenuDlg( UI_TYPE_FOCUS );
	m_pMainMenuDlg->Initialize( true );
	AddCommonDialogList(m_pMainMenuDlg);
	m_pVecOpenBaseDialogList.push_back(m_pMainMenuDlg);

	// Note : 여기에 임시로 넣어둠.
	//
	m_pMainMenuDlg->SetQuestSumInfoDlg();

#ifdef PRE_ADD_PVP_RANKING
	if( m_pPvPRankDlg )
	{
		m_pPvPRankDlg->SetCallback( m_pMainMenuDlg );
	}
#endif // PRE_ADD_PVP_RANKING

#ifdef PRE_REMOVE_CASHSHOP
	m_pCashShopMenuDlg = new CDnCashShopMenuDlg(UI_TYPE_FOCUS);
	m_pCashShopMenuDlg->Initialize(false);
#else
	m_pCashShopMenuDlg = new CDnCashShopMenuDlg(UI_TYPE_FOCUS);
	m_pCashShopMenuDlg->Initialize(true);
	AddCommonDialogList(m_pCashShopMenuDlg);
#endif

	m_pNpcDlg = new CDnNpcDlg(  UI_TYPE_FOCUS, NULL, NPC_DIALOG );
	m_pNpcDlg->Initialize( false );
	AddCommonDialogList(m_pNpcDlg);

	m_pNoticeDlg = new CDnNoticeDlg;
	m_pNoticeDlg->Initialize( false );
	AddCommonDialogList(m_pNoticeDlg);

	m_pGateQuestionDlg = new CDnGateQuestionDlg( UI_TYPE_MODAL, NULL, GATEQUESTION_DIALOG );
	m_pGateQuestionDlg->Initialize( false );
	AddCommonDialogList(m_pGateQuestionDlg);

	m_pCaptionDlg_01 = new CDnCaptionDlg_01( UI_TYPE_TOP );
	m_pCaptionDlg_01->Initialize( true );
	AddCommonDialogList(m_pCaptionDlg_01);

	m_pCaptionDlg_02 = new CDnCaptionDlg_02( UI_TYPE_TOP );
	m_pCaptionDlg_02->Initialize( true );
	AddCommonDialogList(m_pCaptionDlg_02);

	m_pCaptionDlg_03 = new CDnCaptionDlg_03( UI_TYPE_TOP );
	m_pCaptionDlg_03->Initialize( true );
	AddCommonDialogList(m_pCaptionDlg_03);

	m_pCaptionDlg_04 = new CDnCaptionDlg_04( UI_TYPE_TOP );
	m_pCaptionDlg_04->Initialize( true );
	AddCommonDialogList(m_pCaptionDlg_04);

	m_pCaptionDlg_05 = new CDnCaptionDlg_05(UI_TYPE_TOP);
	m_pCaptionDlg_05->Initialize(false);
	AddCommonDialogList(m_pCaptionDlg_05);

	m_pCaptionDlg_06 = new CDnCaptionDlg_06(UI_TYPE_TOP);
	m_pCaptionDlg_06->Initialize(false);
	AddCommonDialogList(m_pCaptionDlg_06);

	#ifdef PRE_ADD_NEWCOMEBACK
	m_pCaptionDlg_07 = new CDnCaptionDlg_07(UI_TYPE_TOP);
	m_pCaptionDlg_07->Initialize(false);
	AddCommonDialogList(m_pCaptionDlg_07);
	#endif // PRE_ADD_NEWCOMEBACK
	m_pItemAlarmDlg = new CDnItemAlarmDlg( UI_TYPE_TOP );
	m_pItemAlarmDlg->Initialize( false );
	AddCommonDialogList(m_pItemAlarmDlg);

	m_pDirectDlg = new CDnDirectDlg( UI_TYPE_MODAL );
	m_pDirectDlg->Initialize( false );
	AddCommonDialogList(m_pDirectDlg);

	m_pInspectPlayerDlg = new CDnInspectPlayerDlg( UI_TYPE_FOCUS );
	m_pInspectPlayerDlg->Initialize( false );
	AddCommonDialogList(m_pInspectPlayerDlg);

	m_pChannelMoveDlg = new CDnChannelMoveDlg( UI_TYPE_BOTTOM_MSG );
	m_pChannelMoveDlg->Initialize( true );
	AddCommonDialogList(m_pChannelMoveDlg);

	m_pTooltipDlg = new CDnTooltipDlg( UI_TYPE_TOP_MSG );
	m_pTooltipDlg->Initialize( false );
	AddCommonDialogList(m_pTooltipDlg);

	m_pNameLinkToolTipDlg = new CDnTooltipDlg( UI_TYPE_BOTTOM_MSG );
	m_pNameLinkToolTipDlg->Initialize(false);
	m_pNameLinkToolTipDlg->SetMode(CDnTooltipDlg::TOOLTIP_NAMELINK);
	AddCommonDialogList(m_pNameLinkToolTipDlg);

	m_pPrivateMarketReqDlg = new CDnPrivateMarketReqDlg( UI_TYPE_TOP_MSG );
	m_pPrivateMarketReqDlg->Initialize( false );
	AddCommonDialogList(m_pPrivateMarketReqDlg);

	m_pProgressDlg = new CDnProgressDlg( UI_TYPE_TOP_MSG );
	m_pProgressDlg->Initialize(false);
	AddCommonDialogList(m_pProgressDlg);

	m_pProgressMBox = new CDnProgressMBox( UI_TYPE_TOP_MSG );
	m_pProgressMBox->Initialize(false);
	AddCommonDialogList(m_pProgressMBox);

	m_pRandomItemDlg = new CDnRandomItemDlg( UI_TYPE_MODAL );
	m_pRandomItemDlg->Initialize( false );
	AddCommonDialogList(m_pRandomItemDlg);

	m_pMovieDlg = new CDnMovieDlg();
	m_pMovieDlg->Initialize( false );
	AddCommonDialogList( m_pMovieDlg );

	m_pNpcTalkReturnDlg = new CDnNpcTalkReturnDlg();
	m_pNpcTalkReturnDlg->Initialize( false );
	AddCommonDialogList( m_pNpcTalkReturnDlg );

	m_pMovieProcessDlg = new CDnMovieProcessDlg( UI_TYPE_MODAL );
	m_pMovieProcessDlg->Initialize( false );
	AddCommonDialogList( m_pMovieProcessDlg );

	m_pFocusTypeMovieProcessDlg = new CDnMovieProcessDlg( UI_TYPE_FOCUS );
	m_pFocusTypeMovieProcessDlg->Initialize( false );
	AddCommonDialogList( m_pFocusTypeMovieProcessDlg );

	m_pMissionAchieveAlarmDlg = new CDnMissionAchieveAlarmDlg( UI_TYPE_TOP );
	m_pMissionAchieveAlarmDlg->Initialize( false );
	AddCommonDialogList( m_pMissionAchieveAlarmDlg );

	m_pMissionGainAlarmDlg = new CDnMissionGainAlarmDlg( UI_TYPE_TOP );
	m_pMissionGainAlarmDlg->Initialize( false );
	AddCommonDialogList( m_pMissionGainAlarmDlg );

	m_pSlideCaptionDlg = new CDnSlideCaptionDlg( UI_TYPE_MOST_TOP );
	m_pSlideCaptionDlg->Initialize( false );
	AddCommonDialogList( m_pSlideCaptionDlg );

	m_pGameTipDlg = new CDnGameTipDlg( UI_TYPE_MOST_TOP );
	m_pGameTipDlg->Initialize( false );
	AddCommonDialogList( m_pGameTipDlg );

	m_pItemUnsealDlg = new CDnItemUnSealDlg( UI_TYPE_FOCUS );
	m_pItemUnsealDlg->Initialize(false);
	AddCommonDialogList(m_pItemUnsealDlg);

	m_pItemSealDlg = new CDnItemSealDlg( UI_TYPE_FOCUS );
	m_pItemSealDlg->Initialize(false);
	AddCommonDialogList(m_pItemSealDlg);

	m_pGuildCreateDlg = new CDnGuildCreateDlg;
	m_pGuildCreateDlg->Initialize( false );
	AddCommonDialogList(m_pGuildCreateDlg);

	m_pGuildYesNoDlg = new CDnGuildYesNoDlg;
	m_pGuildYesNoDlg->Initialize( false );
	AddCommonDialogList(m_pGuildYesNoDlg);

	m_pHelpKeyboardDlg = new CDnHelpKeyboardDlg( UI_TYPE_MODAL );
	m_pHelpKeyboardDlg->Initialize( false );
	AddCommonDialogList( m_pHelpKeyboardDlg );

	m_pPrivateChatDlg = new CDnPrivateChatDlg( UI_TYPE_TOP_MSG );
	m_pPrivateChatDlg->Initialize( false );
	AddCommonDialogList(m_pPrivateChatDlg);

	m_pGuildInviteReqDlg = new CDnGuildInviteReqDlg( UI_TYPE_TOP_MSG );
	m_pGuildInviteReqDlg->Initialize( false );
	AddCommonDialogList(m_pGuildInviteReqDlg);

	m_pAcceptRequestDlg = new CDnAcceptRequestDlg( UI_TYPE_TOP_MSG );
	m_pAcceptRequestDlg->Initialize( false );
	AddCommonDialogList(m_pAcceptRequestDlg);

	m_pDungeonOpenNoticeDlg = new CDnDungeonOpenNoticeDlg( UI_TYPE_TOP_MSG );
	m_pDungeonOpenNoticeDlg->Initialize( false );
	AddCommonDialogList( m_pDungeonOpenNoticeDlg );

	m_pSplitConfirmExDlg = new CDnStoreConfirmExDlg(UI_TYPE_MODAL);
	m_pSplitConfirmExDlg->Initialize(false);
	AddCommonDialogList(m_pSplitConfirmExDlg);

#ifdef PRE_ADD_CASHSHOP_RENEWAL
	m_pCashShopDlg = new CDnCashShopRenewalDlg(UI_TYPE_FOCUS);
#else
	m_pCashShopDlg = new CDnCashShopDlg(UI_TYPE_FOCUS);	
#endif // PRE_ADD_CASHSHOP_RENEWAL
	m_pCashShopDlg->Initialize(false);
	AddCommonDialogList( m_pCashShopDlg );

	m_pWorldMsgDlg = new CDnWorldMsgDlg( UI_TYPE_TOP_MSG );
	m_pWorldMsgDlg->Initialize( false );
	AddCommonDialogList( m_pWorldMsgDlg );

	m_pSkillResetConfirmDlg = new CDnSkillResetDlg( UI_TYPE_MODAL );
	m_pSkillResetConfirmDlg->Initialize( false );
	AddCommonDialogList( m_pSkillResetConfirmDlg );

#ifdef PRE_ADD_GACHA_JAPAN
	m_pGachaDlg = new CDnGachaDlg( UI_TYPE_FOCUS );
	m_pGachaDlg->Initialize( false );

	// 이 다이얼로그의 삭제는 GachaDlg 소멸자에서 직접 처리합니다.
	m_pGachaItemNameRouletteDlg = new CDnGachaRouletteDlg( UI_TYPE_SELF );
	m_pGachaItemNameRouletteDlg->Initialize( false );
	m_pGachaItemStatRouletteDlg = new CDnGachaRouletteDlg( UI_TYPE_SELF );
	m_pGachaItemStatRouletteDlg->Initialize( false );
	m_pGachaDlg->SetRouletteDlg( m_pGachaItemNameRouletteDlg, m_pGachaItemStatRouletteDlg );

	AddCommonDialogList( m_pGachaDlg );
#endif

	m_pChatRoomDlg = new CDnChatRoomDlg( UI_TYPE_MODAL );
	m_pChatRoomDlg->Initialize( false );
	AddCommonDialogList( m_pChatRoomDlg );

	m_pChatRoomPassWordDlg = new CDnChatRoomPassWordDlg( UI_TYPE_MODAL );
	m_pChatRoomPassWordDlg->Initialize( false );
	AddCommonDialogList( m_pChatRoomPassWordDlg );

	m_pItemPotentialDlg = new CDnPotentialJewelDlg( UI_TYPE_FOCUS );
	m_pItemPotentialDlg->Initialize(false);
	AddCommonDialogList(m_pItemPotentialDlg);

#ifdef PRE_MOD_POTENTIAL_JEWEL_RENEWAL
	m_pItemPotentialCleanDlg = new CDnPotentialJewelCleanDlg( UI_TYPE_FOCUS );
	m_pItemPotentialCleanDlg->Initialize(false);
	AddCommonDialogList(m_pItemPotentialCleanDlg);
#endif // PRE_MOD_POTENTIAL_JEWEL_RENEWAL

	m_pItemPetalTokenDlg = new CDnPetalTokenDlg( UI_TYPE_MODAL );
	m_pItemPetalTokenDlg->Initialize(false);
	AddCommonDialogList(m_pItemPetalTokenDlg);

	m_pItemAppellationGainDlg = new CDnAppellationGainDlg( UI_TYPE_MODAL );
	m_pItemAppellationGainDlg->Initialize( false );
	AddCommonDialogList(m_pItemAppellationGainDlg);

	m_pCharmItemKeyDlg = new CDnCharmItemKeyDlg( UI_TYPE_MODAL );
	m_pCharmItemKeyDlg->Initialize( false );
	AddCommonDialogList( m_pCharmItemKeyDlg );

	m_pCharmItemProgressDlg = new CDnCharmItemProgressDlg( UI_TYPE_MODAL );
	m_pCharmItemProgressDlg->Initialize( false );
	AddCommonDialogList( m_pCharmItemProgressDlg );

	m_pCharmItemResultDlg = new CDnCharmItemResultDlg( UI_TYPE_MODAL );
	m_pCharmItemResultDlg->Initialize( false );
	AddCommonDialogList( m_pCharmItemResultDlg );

#if defined( PRE_ADD_EASYGAMECASH )
	m_pPackageBoxResultDlg = new CDnPackageBoxResultDlg( UI_TYPE_MODAL );
	m_pPackageBoxResultDlg->Initialize( false );
	AddCommonDialogList( m_pPackageBoxResultDlg );

	m_pPackageBoxProgressDlg = new CDnPackageBoxProgressDlg( UI_TYPE_MODAL );
	m_pPackageBoxProgressDlg->Initialize( false );
	AddCommonDialogList( m_pPackageBoxProgressDlg );
#endif	// #if defined( PRE_ADD_EASYGAMECASH )

#ifdef PRE_ADD_AUTOUNPACK
	m_pAutoUnPackDlg = new CDnAutoUnPackDlg();
	m_pAutoUnPackDlg->Initialize( false );
	AddCommonDialogList( m_pAutoUnPackDlg );

	m_pAutoUnPackResultDlg = new CDnAutoUnPackResultDlg( UI_TYPE_MODAL );
	m_pAutoUnPackResultDlg->Initialize( false );
	AddCommonDialogList( m_pAutoUnPackResultDlg );
#endif

#ifdef PRE_ADD_CASHREMOVE
	m_pCashItemRemoveDlg = new CDnInvenCashItemRemoveDlg( UI_TYPE_MODAL );
	m_pCashItemRemoveDlg->Initialize( false );
	AddCommonDialogList( m_pCashItemRemoveDlg );
#endif

#ifdef PRE_ADD_CHAOSCUBE
	m_pChaosCubeDlg = new CDnChaosCubeDlg( UI_TYPE_MODAL );
	m_pChaosCubeDlg->Initialize( false );
	AddCommonDialogList( m_pChaosCubeDlg );

	// Dialog - 획득결과물.
	m_pChaosCubeResultDlg = new CDnChaosCubeResultDlg( UI_TYPE_MODAL );
	m_pChaosCubeResultDlg->Initialize( false );
	AddCommonDialogList( m_pChaosCubeResultDlg );


	// Dialog - 재료Dlg or 결과물.
	m_pChaosCubeStuffDlg = new CDnChaosCubeStuffDlg( UI_TYPE_MODAL );  
	m_pChaosCubeStuffDlg->Initialize( false );
	AddCommonDialogList( m_pChaosCubeStuffDlg );

	// Dialog - 프로그레스창.	
	m_pChaosCubeProgressDlg = new CDnChaosCubeProgressDlg( UI_TYPE_MODAL );
	m_pChaosCubeProgressDlg->Initialize( false );
	AddCommonDialogList( m_pChaosCubeProgressDlg );

#endif

#ifdef PRE_ADD_CHOICECUBE
	m_pChoiceCubeDlg = new CDnChoiceCubeDlg( UI_TYPE_MODAL );
	m_pChoiceCubeDlg->Initialize( false );
	AddCommonDialogList( m_pChoiceCubeDlg );
#endif

#ifdef PRE_ADD_BESTFRIEND
	
	// 절친등록서 Dlg.
	m_pBFRegistrationDlg = new CDnBFRegistrationDlg( UI_TYPE_MODAL );
	m_pBFRegistrationDlg->Initialize( false );
	AddCommonDialogList( m_pBFRegistrationDlg );

	// 절친확인 Dlg.
	m_pIdentifyBF = new CDnIdentifyBestFriendDlg( UI_TYPE_MODAL );
	m_pIdentifyBF->Initialize( false );
	AddCommonDialogList( m_pIdentifyBF );

	// 절친수락 ProgressDlg.
	m_pBFProgressDlg = new CDnBestFriendProgressDlg( UI_TYPE_MODAL );
	m_pBFProgressDlg->Initialize( false );
	AddCommonDialogList( m_pBFProgressDlg );

	// 보상아이템Dlg.
	m_pBFRewardDlg = new CDnBestFriendRewardDlg( UI_TYPE_MODAL );
	m_pBFRewardDlg->Initialize( false );
	AddCommonDialogList( m_pBFRewardDlg );

	// 보상아이템ProgressDlg.
	m_pBFRewardProgressDlg = new CDnBestFriendRewardProgressDlg( UI_TYPE_MODAL );
	m_pBFRewardProgressDlg->Initialize( false );
	AddCommonDialogList( m_pBFRewardProgressDlg );

	// 메모Dlg.
	m_pBFMemoDlg = new CDnBestFriendMemoDlg( UI_TYPE_MODAL );
	m_pBFMemoDlg->Initialize( false );
	AddCommonDialogList( m_pBFMemoDlg );

#endif

	m_pDarkLairRankBoardDlg = new CDnDarkLairRankBoardDlg();
	m_pDarkLairRankBoardDlg->Initialize( false );
	AddCommonDialogList( m_pDarkLairRankBoardDlg );

	m_pPVPLadderRankBoardDlg = new CDnPVPLadderRankBoardDlg();
	m_pPVPLadderRankBoardDlg->Initialize( false );
	AddCommonDialogList( m_pPVPLadderRankBoardDlg );

#ifdef PRE_FIX_ESCAPE
	m_pEscapeDlg = new CDnEscapeDlg( UI_TYPE_TOP_MSG );
	m_pEscapeDlg->Initialize(false);
	AddCommonDialogList(m_pEscapeDlg);
#endif

#ifdef PRE_REMOVE_GUILD_WAR_UI
	// Guild War game
	m_pGuildWarPreliminaryStatusDlg = new CDnGuildWarPreliminaryStatusDlg();
	m_pGuildWarPreliminaryStatusDlg->Initialize( false );
	AddCommonDialogList( m_pGuildWarPreliminaryStatusDlg );

	m_pGuildWarPreliminaryResultDlg = new CDnGuildWarPreliminaryResultDlg();
	m_pGuildWarPreliminaryResultDlg->Initialize( false );
	AddCommonDialogList( m_pGuildWarPreliminaryResultDlg );

	m_pGuildWarFinalTournamentListDlg = new CDnGuildWarFinalTournamentListDlg();
	m_pGuildWarFinalTournamentListDlg->Initialize( false );
	AddCommonDialogList( m_pGuildWarFinalTournamentListDlg );
#endif // PRE_REMOVE_GUILD_WAR_UI

#ifdef _USE_VOICECHAT
	m_pVoiceChatPremiumOptDlg = new CDnVoiceChatPremiumOptDlg( UI_TYPE_MODAL );
	m_pVoiceChatPremiumOptDlg->Initialize( false );
	AddCommonDialogList( m_pVoiceChatPremiumOptDlg );
#endif

	m_pLifeSkillFishingDlg = new CDnLifeSkillFishingDlg( UI_TYPE_FOCUS );
	m_pLifeSkillFishingDlg->Initialize( false );
	AddCommonDialogList( m_pLifeSkillFishingDlg );

	m_pLifeChannelDlg = new CDnLifeChannelDlg( UI_TYPE_MODAL );
	m_pLifeChannelDlg->Initialize( false );
	AddCommonDialogList( m_pLifeChannelDlg );

	m_pGuildRenameDlg = new CDnGuildRenameDlg( UI_TYPE_MODAL );
	m_pGuildRenameDlg->Initialize( false );
	AddCommonDialogList( m_pGuildRenameDlg  );

	m_pCharRenameDlg = new CDnCharRenameDlg( UI_TYPE_MODAL );
	m_pCharRenameDlg->Initialize( false );
	AddCommonDialogList( m_pCharRenameDlg );

	m_pEnchantJewelDlg = new CDnEnchantJewelDlg();
	m_pEnchantJewelDlg->Initialize(false);
	AddCommonDialogList(m_pEnchantJewelDlg);

	m_pGuildMarkCreateDlg = new CDnGuildMarkCreateDlg( UI_TYPE_MODAL );
	m_pGuildMarkCreateDlg->Initialize( false );
	AddCommonDialogList( m_pGuildMarkCreateDlg );

	m_pHelpAlarmDlg = new CDnHelpAlarmDlg( UI_TYPE_TOP_MSG );
	m_pHelpAlarmDlg->Initialize( false );
	AddCommonDialogList( m_pHelpAlarmDlg );

	m_pRepUnionMarkTooltipDlg = new CDnRepUnionMarkTooltipDlg( UI_TYPE_TOP_MSG );
	m_pRepUnionMarkTooltipDlg->Initialize( false );
	AddCommonDialogList(m_pRepUnionMarkTooltipDlg);

#if defined(PRE_ADD_VIP_FARM)
	m_pLifeUnionMarkTooltipDlg = new CDnLifeUnionMarkTooltipDlg( UI_TYPE_TOP_MSG );
	m_pLifeUnionMarkTooltipDlg->Initialize( false );
	AddCommonDialogList(m_pLifeUnionMarkTooltipDlg);
#endif

	m_pGuildWarMarkTooltipDlg = new CDnGuildWarMarkTooltipDlg(UI_TYPE_TOP_MSG);
	m_pGuildWarMarkTooltipDlg->Initialize(false);
	AddCommonDialogList(m_pGuildWarMarkTooltipDlg);

	m_pCustomTooltipDlg = new CDnCustomTooltipDlg( UI_TYPE_TOP_MSG );
	m_pCustomTooltipDlg->Initialize( false );
	AddCommonDialogList(m_pCustomTooltipDlg);

	m_pGuildRewardMarkTooltipDlg = new CDnGuildRewardMarkTooltipDlg(UI_TYPE_TOP_MSG);
	m_pGuildRewardMarkTooltipDlg->Initialize(false);
	AddCommonDialogList(m_pGuildRewardMarkTooltipDlg);

	m_pPetNamingDlg = new CDnPetNamingDlg( UI_TYPE_MODAL );
	m_pPetNamingDlg->Initialize( false );
	AddCommonDialogList( m_pPetNamingDlg );

	m_pPetExtendPeriodDlg = new CDnPetExtendPeriodDlg();
	m_pPetExtendPeriodDlg->Initialize( false );
	AddCommonDialogList( m_pPetExtendPeriodDlg );
	m_pPetAddSkillDlg = new CDnPetAddSkillDlg( UI_TYPE_MODAL );
	m_pPetAddSkillDlg->Initialize( false );
	AddCommonDialogList( m_pPetAddSkillDlg );

	m_pPetExpandSkillSlotDlg = new CDnPetExpandSkillSlotDlg( UI_TYPE_MODAL );
	m_pPetExpandSkillSlotDlg->Initialize( false );
	AddCommonDialogList( m_pPetExpandSkillSlotDlg );
#ifdef PRE_ADD_PET_EXTEND_PERIOD
	m_pBuyPetExtendPeriodDlg = new CDnBuyPetExtendPeriodDlg( UI_TYPE_MODAL );
	m_pBuyPetExtendPeriodDlg->Initialize( false );
	AddCommonDialogList( m_pBuyPetExtendPeriodDlg );
#endif // PRE_ADD_PET_EXTEND_PERIOD
#ifdef PRE_ADD_CHANGEJOB_CASHITEM
	m_pChangeJobDlg = new CDnChangeJobCashItemDlg( UI_TYPE_MODAL );
	m_pChangeJobDlg->Initialize( false );
	AddCommonDialogList( m_pChangeJobDlg );
#endif // #ifdef PRE_ADD_CHAGEJOB_CASHITEM

	m_pDnGuildWarFinalWinNoticeDlg = new CDnGuildWarFinalWinNoticeDlg( UI_TYPE_FOCUS );
	m_pDnGuildWarFinalWinNoticeDlg->Initialize( false );
	AddCommonDialogList( m_pDnGuildWarFinalWinNoticeDlg );

#if defined(PRE_ADD_REMOVE_PREFIX)
	m_pRemovePrefixDlg = new CDnRemovePrefixDlg(UI_TYPE_FOCUS);
	m_pRemovePrefixDlg->Initialize(false);
	AddCommonDialogList(m_pRemovePrefixDlg);
#endif // PRE_ADD_REMOVE_PREFIX

	m_pNestInfoDlg = new CDnNestInfoDlg(UI_TYPE_MODAL);
	m_pNestInfoDlg->Initialize(false);
	AddCommonDialogList(m_pNestInfoDlg);

	m_pMapMoveCashItemDlg = new CDnMapMoveCashItemDlg( UI_TYPE_MODAL );
	m_pMapMoveCashItemDlg->Initialize( false );
	AddCommonDialogList( m_pMapMoveCashItemDlg );

	m_pGuildLevelUpAlarmDlg = new CDnGuildLevelUpAlarmDlg( UI_TYPE_TOP );
	m_pGuildLevelUpAlarmDlg->Initialize( false );
	AddCommonDialogList( m_pGuildLevelUpAlarmDlg );

#ifdef PRE_ADD_EXCHANGE_POTENTIAL
	m_pPotentialTransferDlg = new CDnPotentialTransferDlg( UI_TYPE_FOCUS );
	m_pPotentialTransferDlg->Initialize( false );
	AddCommonDialogList( m_pPotentialTransferDlg );
#endif

	m_pWorldZoneSelectDlg = new CDnWorldZoneSelectDlg( UI_TYPE_MODAL, NULL, GATE_MAP_INDEX_SELECT_DIALOG );
	m_pWorldZoneSelectDlg->Initialize( false );
	AddCommonDialogList( m_pWorldZoneSelectDlg );

#ifdef PRE_ADD_INSTANT_CASH_BUY
	m_pDnInstantCashShopBuyDlg = new CDnInstantCashShopBuyDlg( UI_TYPE_MODAL );
	m_pDnInstantCashShopBuyDlg->Initialize( false );
	AddCommonDialogList( m_pDnInstantCashShopBuyDlg );
#endif // PRE_ADD_INSTANT_CASH_BUY

#ifdef PRE_PRIVATECHAT_CHANNEL
	m_pDnPrivateChannelInviteDlg = new CDnPrivateChannelInviteDlg( UI_TYPE_MODAL );
	m_pDnPrivateChannelInviteDlg->Initialize( false );
	AddCommonDialogList( m_pDnPrivateChannelInviteDlg );

	m_pDnPrivateChannelPasswordDlg = new CDnPrivateChannelPasswordDlg( UI_TYPE_MODAL );
	m_pDnPrivateChannelPasswordDlg->Initialize( false );
	AddCommonDialogList( m_pDnPrivateChannelPasswordDlg );
#ifdef PRE_ADD_PRIVATECHAT_CHANNEL
	m_pDnPrivateChannelPasswordChnageDlg = new CDnPrivateChannelPasswordChangeDlg( UI_TYPE_MODAL );
	m_pDnPrivateChannelPasswordChnageDlg->Initialize( false );
	AddCommonDialogList( m_pDnPrivateChannelPasswordChnageDlg );
#endif // PRE_ADD_PRIVATECHAT_CHANNEL
#endif // PRE_PRIVATECHAT_CHANNEL

#ifdef PRE_ADD_DIRECT_BUY_UPGRADEITEM
	m_pMarketDirectBuyDlg = new CDnMarketDirectBuyDlg( UI_TYPE_MODAL );
	m_pMarketDirectBuyDlg->Initialize(false);
	AddCommonDialogList( m_pMarketDirectBuyDlg );
#endif

#ifdef PRE_ADD_ALTHEIA_NEWCAPTION_TRIGGER
	m_ImageBlindDlg = new CDnImageBlindDlg( UI_TYPE_MODAL );
	m_ImageBlindDlg->Initialize(false);
	AddCommonDialogList( m_ImageBlindDlg );
#endif 

#ifdef PRE_ADD_STAMPSYSTEM
	m_pStampEventAlarmDlg = new CDnStampEventAlarmDlg( UI_TYPE_BOTTOM_MSG );
	m_pStampEventAlarmDlg->Initialize( false );
	AddCommonDialogList( m_pStampEventAlarmDlg );
#endif // PRE_ADD_STAMPSYSTEM

#ifdef PRE_ADD_GAMEQUIT_REWARD
	m_pGameQuitRewardDlg = new CDnGameQuitRewardDlg( UI_TYPE_MODAL, NULL, -1, NULL, true );
	m_pGameQuitRewardDlg->Initialize( false );
	AddCommonDialogList( m_pGameQuitRewardDlg );
#endif // PRE_ADD_GAMEQUIT_REWARD

#ifdef PRE_MOD_BROWSER
	m_InternetBrowserMgr.Initialize();
#endif

#ifdef PRE_ADD_DWC
	m_pDWCTeamCreateDlg = new CDnDwcCreateTeamDlg();
	m_pDWCTeamCreateDlg->Initialize(false);
	AddCommonDialogList(m_pDWCTeamCreateDlg);

	m_pDWCInviteReqDlg = new CDnDwcInviteReqDlg( UI_TYPE_TOP_MSG );
	m_pDWCInviteReqDlg->Initialize( false );
	AddCommonDialogList(m_pDWCInviteReqDlg);

	m_pDWCMarkToolTipDlg = new CDnDWCMarkTooltipDlg( UI_TYPE_TOP_MSG );
	m_pDWCMarkToolTipDlg->Initialize(false);
	AddCommonDialogList(m_pDWCMarkToolTipDlg);
#endif // PRE_ADD_DWC

#ifdef PRE_ADD_MAINQUEST_UI
	m_pMainQuestDlg = new CDnMainQuestDlg( UI_TYPE_FOCUS, NULL, MAINQUEST_DIALOG );
	m_pMainQuestDlg->Initialize(false);
	AddCommonDialogList(m_pMainQuestDlg);
#endif

#ifdef PRE_ADD_DRAGON_GEM
	m_pDragonGemEquipDlg = new CDnDragonGemEquipDlg( UI_TYPE_FOCUS );
	m_pDragonGemEquipDlg->Initialize(false);
	AddCommonDialogList(m_pDragonGemEquipDlg);

	m_pDragonGemRemoveDlg = new CDnDragonGemRemoveDlg( UI_TYPE_FOCUS );
	m_pDragonGemRemoveDlg->Initialize(false);
	AddCommonDialogList(m_pDragonGemRemoveDlg);
#endif // PRE_ADD_DRAGON_GEM

	//rlkt_test
	m_pJobChangeDlg = new CDnJobChangeDlg;
	m_pJobChangeDlg->Initialize(false);

	m_pItemDisjointResultDlg = new CDnItemDisjointResultDlg;
	m_pItemDisjointResultDlg->Initialize(false);


	OutputDebug( "CDnInterface::InitializeCommon\n" );
}

void CDnInterface::InitializeGame( CTask *pTask )
{
	CreateMonsterGauge();
	CreateMonsterPartsGauge();

	m_pDamageMng = new CDnDamageMng;
	m_pComboMng = new CDnComboMng;
	m_pChainMng = new CDnChainMng;
	m_pEnemyGaugeMng = new CDnEnemyGaugeMng;
	m_pCpDownMng = new CDnCpDownMng;
	m_pCpScoreMng = new CDnCpScoreMng;
	m_pCpScoreMng->Initialize();
	m_pCpJudgeMng = new CDnCpJudgeMng;
	m_pCpJudgeMng->Initialize();

#ifdef PRE_ADD_CRAZYDUC_UI	
	m_pScoreMng = new CDnScoreMng;
#endif 
	m_pDungeonEnterDlg = new CDnDungeonEnterDlg( UI_TYPE_BOTTOM_MSG, NULL, DUNGEON_ENTER_DIALOG );
	m_pDungeonEnterDlg->Initialize( false );

	m_pDungeonNestEnterDlg = new CDnDungeonEnterDlg( UI_TYPE_BOTTOM_MSG, NULL, DUNGEON_ENTER_DIALOG );
	m_pDungeonNestEnterDlg->SetDungeonEnterType( CDnDungeonEnterDlg::DUNGEONENTER_TYPE_NEST );
	m_pDungeonNestEnterDlg->Initialize( false );

	m_pDnHardCoreModeTimeAttackDlg = new CDnHardCoreModeTimeAttackDlg( UI_TYPE_BOTTOM_MSG );
	m_pDnHardCoreModeTimeAttackDlg->Initialize( false );

	m_pTimeOutFailDlg = new CDnTimeOutFailDlg( UI_TYPE_TOP );
	m_pTimeOutFailDlg->Initialize( false );

#ifdef PRE_ADD_STAGE_LIMIT_INTERFACE
	m_pDungeonLimitDlg = new CDnDungeonLimitDlg( UI_TYPE_FOCUS );
	m_pDungeonLimitDlg->Initialize( false );
#endif

	m_pDungeonSynchroDlg = new CDnDungeonSynchroDlg( UI_TYPE_BOTTOM_MSG );
	m_pDungeonSynchroDlg->Initialize( false );

	m_pEventDungeonEnterDlg = new CDnEventDungeonEnterDlg( UI_TYPE_BOTTOM_MSG, NULL, DUNGEON_ENTER_DIALOG );
	m_pEventDungeonEnterDlg->Initialize( false );

	m_pStageClearDlg = new CDnStageClearDlg( UI_TYPE_BOTTOM_MSG );
	m_pStageClearDlg->Initialize( false );

	m_pStageClearReportDlg = new CDnStageClearReportDlg( UI_TYPE_BOTTOM_MSG );
	m_pStageClearReportDlg->Initialize( false );
	
	m_pMissionFailDlg = new CDnMissionFailDlg( UI_TYPE_TOP );
	m_pMissionFailDlg->Initialize( false );

	m_pRebirthCaptionDlg = new CDnRebirthCaptionDlg( UI_TYPE_BOTTOM_MSG );
	m_pRebirthCaptionDlg->Initialize( false );

	m_pRebirthFailDlg = new CDnRebirthFailDlg( UI_TYPE_BOTTOM_MSG );
	m_pRebirthFailDlg->Initialize( false );

	m_pDungeonMoveDlg = new CDnDungeonClearMoveDlg( UI_TYPE_MODAL, NULL, DUNGEON_MOVE_DIALOG, this );
	m_pDungeonMoveDlg->Initialize( false );

	// 원래는 UI_TYPE_TOP 이었는데, 다른 창들보다 아래로 가기 원해서 UI_TYPE_BOTTOM 로 처리하겠다.
	m_pStickAniDlg = new CDnStickAniDlg( UI_TYPE_BOTTOM );
	m_pStickAniDlg->Initialize( false );

	// 스테이지안에 들어가서는 ChannelMoveDlg대신 StageInfoDlg가 뜨게된다.
	// 유저입력 받을필요 없는 보여주기용 다이얼로그니 MSG 안받도록 한다.
	m_pDungeonInfoDlg = new CDnDungeonInfoDlg( UI_TYPE_BOTTOM_MSG );
	m_pDungeonInfoDlg->Initialize( false );

	m_pItemChoiceDlg = new CDnItemChoiceDlg(UI_TYPE_FOCUS);
	m_pItemChoiceDlg->Initialize(false);

	m_pStageClearMoveDlg = new CDnStageClearMoveDlg(UI_TYPE_BOTTOM_MSG);
	m_pStageClearMoveDlg->Initialize(false);
	
	m_pStageClearRepairDlg = new CDnDungeonClearRepairDlg(UI_TYPE_BOTTOM_MSG);
	m_pStageClearRepairDlg->Initialize(false);

	m_pBossAlertDlg = new CDnBossAlertDlg;
	m_pBossAlertDlg->Initialize(false);

#if defined( PRE_ADD_STAGE_CLEAR_ADD_REWARD )
	m_pDungeonExtraRewardDlg = new CDnDungeonExtraRewardDlg( UI_TYPE_MODAL );
	m_pDungeonExtraRewardDlg->Initialize( false );
#endif	// PRE_ADD_STAGE_CLEAR_ADD_REWARD

	m_pDurabilityGaugeMng = new CDnDurabilityGaugeMng();

#if defined( PRE_ALTEIAWORLD_EXPLORE )
	m_pAlteaClearDlg = new CDnAlteaClearDlg( UI_TYPE_MODAL );
	m_pAlteaClearDlg->Initialize( false );

	Initialize_AlteaBoard();
	SetAlteaDiceIcon();
#endif	// #if defined( PRE_ALTEIAWORLD_EXPLORE )

	OutputDebug( "CDnInterface::InitializeGame\n" );
}

void CDnInterface::InitializePVPGame( CTask *pTask )
{
	InitializeGame( pTask );

	if( m_pChatDlg )
		m_pChatDlg->SetChatGameMode( true );

	m_pPVPModeEndDlg = new CDnPVPModeEndDlg( UI_TYPE_TOP_MSG );
	m_pPVPModeEndDlg->Initialize( false );

	m_pRespawnGauageDlg = new CDnRespawnGauageDlg( UI_TYPE_TOP );
	m_pRespawnGauageDlg->Initialize( false );

	CDnPvPGameTask * PvPGameTask  = dynamic_cast<CDnPvPGameTask *>(pTask);
	
	if( PvPGameTask )
	{
		switch( PvPGameTask->GetGameMode())
		{
		case PvPCommon::GameMode::PvP_Respawn:
			m_pModeHUD = new CDnRespawnModeHUD( UI_TYPE_BOTTOM );
			m_pModeHUD->Initialize( true );
			break;
		case PvPCommon::GameMode::PvP_Round:
			m_pModeHUD = new CDnRoundModeHUD( UI_TYPE_BOTTOM );
			m_pModeHUD->Initialize( true );
			break;
		case PvPCommon::GameMode::PvP_Captain:
			m_pModeHUD = new CDnRoundModeHUD( UI_TYPE_BOTTOM );
			m_pModeHUD->Initialize( true );
			break;
		case PvPCommon::GameMode::PvP_IndividualRespawn:
			m_pModeHUD = new CDnIndividualRespawnModeHUD( UI_TYPE_BOTTOM );
			m_pModeHUD->Initialize( true );
			break;
		case PvPCommon::GameMode::PvP_Zombie_Survival:
			m_pModeHUD = new CDnIndividualRoundModeHUD( UI_TYPE_BOTTOM );
			m_pModeHUD->Initialize( true );
			if(m_pChatDlg) m_pChatDlg->ResetChatMode();
			break;
		case PvPCommon::GameMode::PvP_GuildWar :
			m_pModeHUD = new CDnGuildWarModeHUD( UI_TYPE_BOTTOM );
			m_pModeHUD->Initialize( true );
			break;
		case PvPCommon::GameMode::PvP_Occupation:
			m_pModeHUD = new CDnOccupationModeHUD( UI_TYPE_BOTTOM );
			m_pModeHUD->Initialize( true );
			break;
		case PvPCommon::GameMode::PvP_AllKill:
	#ifdef _TEST_CODE_KAL
			m_pModeHUD = new CDnPVPTournamentModeHUD(UI_TYPE_BOTTOM);
	#else
			m_pModeHUD = new CDnAllKillModeHUD( UI_TYPE_BOTTOM );
	#endif
			m_pModeHUD->Initialize( true );
			break;
#if defined( PRE_ADD_RACING_MODE )
		case PvPCommon::GameMode::PvP_Racing:
			m_pModeHUD = new CDnIndividualRespawnModeHUD( UI_TYPE_BOTTOM );
			m_pModeHUD->Initialize( true );
			break;
#endif	// #if defined( PRE_ADD_RACING_MODE )

#ifdef PRE_ADD_PVP_TOURNAMENT
		case PvPCommon::GameMode::PvP_Tournament:
			{
				if (m_pPVPTournamentGMatchListDlg == NULL)
				{
					m_pPVPTournamentGMatchListDlg = new CDnPVPTournamentGameMatchListDlg(UI_TYPE_TOP_MSG);
					m_pPVPTournamentGMatchListDlg->Initialize(false);
				}

				if (m_pPVPTournamentGMatchItemDlg == NULL)
				{
					if (CDnBridgeTask::IsActive())
					{
						CDnPVPTournamentDataMgr& mgr = CDnBridgeTask::GetInstance().GetPVPTournamentDataMgr();
						PvPTournamentUIDef::eMatchModeByCount mode = mgr.GetCurrentMode();

						switch(mode)
						{
						case eMODE_4: m_pPVPTournamentGMatchItemDlg = new CDnPVPTournamentGameMatchListItemDlg_4User(UI_TYPE_TOP); break;
						case eMODE_8: m_pPVPTournamentGMatchItemDlg = new CDnPVPTournamentGameMatchListItemDlg_8User(UI_TYPE_TOP); break;
						case eMODE_16: m_pPVPTournamentGMatchItemDlg = new CDnPVPTournamentGameMatchListItemDlg_16User(UI_TYPE_TOP); break;
						case eMODE_32: m_pPVPTournamentGMatchItemDlg = new CDnPVPTournamentGameMatchListItemDlg_32User(UI_TYPE_TOP); break;
						default:
							_ASSERT(0);
						}
						if (m_pPVPTournamentGMatchItemDlg)
							m_pPVPTournamentGMatchItemDlg->Initialize(false);
					}
				}

				if (m_pPVPTournamentGMatchListDlg && m_pPVPTournamentGMatchItemDlg)
				{
					m_pPVPTournamentGMatchListDlg->SetGameMatchListItemDlg(m_pPVPTournamentGMatchItemDlg);
					m_pPVPTournamentGMatchItemDlg->SetBaseDlg(m_pPVPTournamentGMatchListDlg);
				}

				m_pModeHUD = new CDnPVPTournamentModeHUD( UI_TYPE_BOTTOM );
				m_pModeHUD->Initialize( true );
			}
			break;
#endif

#if defined(PRE_ADD_PVP_COMBOEXERCISE)
		case PvPCommon::GameMode::PvP_ComboExercise:
			m_pModeHUD = new CDnComboExerciseModeHUD( UI_TYPE_BOTTOM );
			m_pModeHUD->Initialize( true );
			break;
#endif	// PRE_ADD_PVP_COMBOEXERCISE

		default:
			m_pModeHUD = new CDnPVPBaseHUD( UI_TYPE_BOTTOM );
			m_pModeHUD->Initialize( true );
			break;
		}
	}	

	if(PvPGameTask)
	{
		if(PvPGameTask->GetGameMode() == PvPCommon::GameMode::PvP_Captain)
		{
			m_pPVPGameResultDlg = new CDnPVPGameResultDlg( UI_TYPE_TOP_MSG );
			m_pPVPGameResultDlg->Initialize( false );
			m_pPVPGameResultDlg->EnableCaptainMode(true); // 대장전은 킬(대장) 을 보여줍니다.
		}
		else if(PvPGameTask->GetGameMode() == PvPCommon::GameMode::PvP_IndividualRespawn)
		{
			m_pPVPGameResultAllDlg = new CDnPVPGameResultAllDlg(UI_TYPE_TOP_MSG);
			m_pPVPGameResultAllDlg->Initialize(false);
		}
		else if(PvPGameTask->GetGameMode() == PvPCommon::GameMode::PvP_Zombie_Survival)
		{
			m_pPVPGameResultAllDlg = new CDnPVPGameResultAllDlg(UI_TYPE_TOP_MSG);
			m_pPVPGameResultAllDlg->EnableAssistMode(true);
			m_pPVPGameResultAllDlg->Initialize(false);
			m_pPVPGameResultAllDlg->SetBoardName(GetEtUIXML().GetUIString( CEtUIXML::idCategory1,120074));
		}
		else if(PvPGameTask->GetGameMode() == PvPCommon::GameMode::PvP_AllKill)
		{
			m_pPVPGameResultDlg = new CDnPVPGameResultDlg( UI_TYPE_TOP_MSG );
			m_pPVPGameResultDlg->Initialize( false );
			m_pPVPGameResultDlg->DisableAssist(true);
		}
		else if(PvPGameTask->GetGameMode() == PvPCommon::GameMode::PvP_GuildWar)
		{
			m_pGuildWarResultDlg = new CDnGuildWarResultDlg(UI_TYPE_TOP_MSG);
			m_pGuildWarResultDlg->Initialize( false );

			m_pGuildWarSituation = new CDnGuildWarSituationMng;
			m_pGuildWarSituation->Initialize();

			m_pGuildWarZoneMapDlg = new CDnGuildWarZoneMapDlg(UI_TYPE_BOTTOM_MSG);
			m_pGuildWarZoneMapDlg->Initialize( false );

			m_pGuildPropInfoDlg = new CDnPvpGuildPropInfoDlg(UI_TYPE_BOTTOM_MSG);
			m_pGuildPropInfoDlg->Initialize( false );

			m_pGuildWarBossGaugeDlg = new CDnGuildWarBossGaugeDlg(UI_TYPE_BOTTOM_MSG);
			m_pGuildWarBossGaugeDlg->Initialize(false);
		}
		else if(PvPGameTask->GetGameMode() == PvPCommon::GameMode::PvP_Occupation)
		{
			m_pPVPGameResultDlg = new CDnPVPGameResultDlg( UI_TYPE_TOP_MSG );
			m_pPVPGameResultDlg->Initialize( false );
			m_pPVPGameResultDlg->EnableOccupationMode(true);

			m_pGuildWarZoneMapDlg = new CDnGuildWarZoneMapDlg(UI_TYPE_BOTTOM_MSG);
			m_pGuildWarZoneMapDlg->Initialize( false );
		}
#if defined( PRE_ADD_RACING_MODE )
		else if(PvPGameTask->GetGameMode() == PvPCommon::GameMode::PvP_Racing)
		{
			m_pPvPRacingResultDlg = new CDnPvPRacingResultDlg( UI_TYPE_TOP_MSG );
			m_pPvPRacingResultDlg->Initialize( false );
		}
#endif	// #if defined( PRE_ADD_RACING_MODE )
#ifdef PRE_ADD_PVP_TOURNAMENT
		else if (PvPGameTask->GetGameMode() == PvPCommon::GameMode::PvP_Tournament)
		{
			m_pPVPTournamentFinalResultDlg = new CDnPVPTournamentFinalResultDlg();
#ifdef PRE_PVP_GAMBLEROOM
			bool bGambleMode = false;
			if( CDnBridgeTask::IsActive() )
			{
				const CDnPVPTournamentDataMgr* pDataMgr = &(CDnBridgeTask::GetInstance().GetPVPTournamentDataMgr());
				if( pDataMgr )
				{
					const SBasicRoomInfo& roomInfo = pDataMgr->GetRoomInfo();
					if( roomInfo.cGambleType == PvPGambleRoom::Petal || roomInfo.cGambleType == PvPGambleRoom::Gold )
						bGambleMode = true;
				}
			}
			m_pPVPTournamentFinalResultDlg->SetGamebleRoom( bGambleMode );
#endif // PRE_PVP_GAMBLEROOM
			m_pPVPTournamentFinalResultDlg->Initialize(false);
		}
#endif

#ifdef PRE_ADD_PVP_COMBOEXERCISE
		else if(PvPGameTask->GetGameMode() == PvPCommon::GameMode::PvP_ComboExercise)
		{
			m_pPVPGameResultAllDlg = new CDnPVPGameResultAllDlg(UI_TYPE_TOP_MSG);
			m_pPVPGameResultAllDlg->Initialize(false);
			m_pPVPGameResultAllDlg->SetComboExerciseMode();
		}
#endif // PRE_ADD_PVP_COMBOEXERCISE

		else
		{
			m_pPVPGameResultDlg = new CDnPVPGameResultDlg( UI_TYPE_TOP_MSG );
			m_pPVPGameResultDlg->Initialize( false );
			if(PvPGameTask->IsLadderMode())
				m_pPVPGameResultDlg->EnableLadderMode(true);
		}
	}

	m_pPVPZoneMapDlg = new CDnPVPZoneMapDlg( UI_TYPE_TOP_MSG );
	m_pPVPZoneMapDlg->Initialize( false );

	m_pPvPKilledMeDlg = new CDnPvPKilledMeDlg( UI_TYPE_TOP_MSG );
	m_pPvPKilledMeDlg->Initialize( false );

#ifdef PRE_WORLDCOMBINE_PVP
	m_pWorldPVPRoomStartDlg = new CDnWorldPVPRoomStartDlg( UI_TYPE_TOP_MSG );
	m_pWorldPVPRoomStartDlg->Initialize( false );
#endif // PRE_WORLDCOMBINE_PVP
}

void CDnInterface::InitializeDLGame( CTask *pTask )
{
	InitializeGame( pTask );

	m_pDarkLairEnterDlg = new CDnDarklairDungeonEnterDlg( UI_TYPE_BOTTOM_MSG, NULL );
	m_pDarkLairEnterDlg->Initialize( false );

	m_pDarkLairRoundCountDlg = new CDnDarklairRoundCountDlg( UI_TYPE_BOTTOM );
	m_pDarkLairRoundCountDlg->Initialize( false );

	m_pDarkLairInfoDlg = new CDnDarklairInfoDlg( UI_TYPE_TOP );
	m_pDarkLairInfoDlg->Initialize( false );

	m_pDarkLairClearReportDlg = new CDnDarklairClearReportDlg( UI_TYPE_BOTTOM_MSG );
	m_pDarkLairClearReportDlg->Initialize( false );

	m_pDarkLairClearRankDlg = new CDnDarklairClearRankDlg( UI_TYPE_BOTTOM_MSG );
	m_pDarkLairClearRankDlg->Initialize( false );

	m_pDarkLairClearRewardDlg = new CDnDarklairClearRewardDlg( UI_TYPE_BOTTOM_MSG );
	m_pDarkLairClearRewardDlg->Initialize( false );

	SAFE_DELETE( m_pDungeonInfoDlg );
	SAFE_DELETE( m_pStageClearReportDlg );
	SAFE_DELETE( m_pStageClearDlg );
#if defined( PRE_ADD_STAGE_CLEAR_ADD_REWARD )
	SAFE_DELETE( m_pDungeonExtraRewardDlg );
#endif	// PRE_ADD_STAGE_CLEAR_ADD_REWARD
}

void CDnInterface::InitializeFarmGame( CTask *pTask )
{
	InitializeGame( pTask );

	m_pLifeTooltipDlg = new CDnLifeTooltipDlg;
	m_pLifeTooltipDlg->Initialize( true );

	m_pLifeConditionDlg = new CDnLifeConditionDlg;
	m_pLifeConditionDlg->Initialize( true );

	m_pLifeSlotDlg = new CDnLifeSlotDlg;
	m_pLifeSlotDlg->Initialize( false );

	m_pPlayerGaugeDlg->InitializeFarm();

	SAFE_DELETE( m_pStageClearReportDlg );
	SAFE_DELETE( m_pStageClearDlg );
#if defined( PRE_ADD_STAGE_CLEAR_ADD_REWARD )
	SAFE_DELETE( m_pDungeonExtraRewardDlg );
#endif	// PRE_ADD_STAGE_CLEAR_ADD_REWARD
}

void CDnInterface::CreateMonsterGauge()
{
	m_pVecEnemyGaugeList.clear();

	CDnGaugeDlg *pGauge = new CDnGaugeDlg( UI_TYPE_BOTTOM_MSG );
	pGauge->Initialize( CEtResourceMng::GetInstance().GetFullName( "EnemyGauge.ui" ).c_str(), FT_ENEMY, false );
	AddMonsterGaugeList( pGauge );

	m_fEnemyGaugeOffset = pGauge->Height() + 5.0f / DEFAULT_UI_SCREEN_HEIGHT;

	for( DWORD i=0; i<eMiniGaugeSize::eMonsterGaugeSize; i++ )
	{
		pGauge = new CDnGaugeDlg( UI_TYPE_BOTTOM_MSG );
		pGauge->Initialize( CEtResourceMng::GetInstance().GetFullName( "MiniEnemyGauge.ui" ).c_str(), FT_MINI_ENEMY, false );
		pGauge->MoveDialog( 0.0f, m_fEnemyGaugeOffset + ( i * pGauge->Height() ) );
		AddMonsterGaugeList( pGauge );
	}
}


void CDnInterface::CreateMonsterPartsGauge()
{
	m_pVecMonsterPartsGuageList.clear();
	CDnGaugeDlg *pGauge = NULL;

	m_vec2MonsterPartsGaugeOffset.y = m_fEnemyGaugeOffset;
	m_vec2MonsterPartsGaugeOffset.x = 0.f;

	for( DWORD i=0; i< eMiniGaugeSize::eMonsterPartsGaugeSize; i++ ) 
	{
		pGauge = new CDnGaugeDlg( UI_TYPE_BOTTOM_MSG );
		pGauge->Initialize( CEtResourceMng::GetInstance().GetFullName( "EnemyPartsGauge.ui" ).c_str(), FT_MONSTER_PARTS, false );
		pGauge->SetProgressIndex( CDnGaugeDlg::eProgressBarType::HPBar, 3 , true );

		if( i < ( eMiniGaugeSize::eMonsterPartsGaugeSize / 2 ) ) // 우측 상단 두개
			pGauge->MoveDialog( 0.0f , m_vec2MonsterPartsGaugeOffset.y + ( (i % 2) * pGauge->Height() ) );
		else 
			pGauge->MoveDialog( m_vec2MonsterPartsGaugeOffset.x - pGauge->Width() , m_vec2MonsterPartsGaugeOffset.y + ( (i % 2)  * pGauge->Height() ) );

		m_pVecMonsterPartsGuageList.push_back( pGauge );
	}
}

void CDnInterface::CreatePlayerGauge()
{
	m_pPlayerGaugeDlg = new CDnGaugeDlg( UI_TYPE_BOTTOM_MSG );
	m_pPlayerGaugeDlg->Initialize( CEtResourceMng::GetInstance().GetFullName( "PlayerGauge.ui" ).c_str(), FT_PLAYER, true );

	m_pPlayerGaugeDlg->SetFaceID( CDnActor::s_hLocalActor->GetClassID() - 1 );
#ifdef PRE_WORLDCOMBINE_PARTY
	m_pPlayerGaugeDlg->SetPlayerName(CDnActor::s_hLocalActor->GetLevel(), CDnActor::s_hLocalActor->GetName(), GAUGE_PLAYERNAME_NO_COLOR, Party::Constants::INVALID_SERVERID);
#else
	m_pPlayerGaugeDlg->SetPlayerName( CDnActor::s_hLocalActor->GetLevel(), CDnActor::s_hLocalActor->GetName() );
#endif
	AddCommonDialogList(m_pPlayerGaugeDlg);
	m_pVecOpenBaseDialogList.push_back(m_pPlayerGaugeDlg);

	m_fPartyPlayerGaugeOffset = m_pPlayerGaugeDlg->Height() + 6.0f / DEFAULT_UI_SCREEN_HEIGHT;

	m_pVecPartyPlayerGaugeList.clear();

	for( DWORD i=0; i<eMiniGaugeSize::eMiniPlayerGaugeSize; i++ )
	{
		CDnGaugeDlg *pGauge = new CDnGaugeDlg( UI_TYPE_BOTTOM_MSG );
		pGauge->Initialize( CEtResourceMng::GetInstance().GetFullName( "MiniPlayerGauge.ui" ).c_str(), FT_MINI_PLAYER, false );
		pGauge->MoveDialog( (i / MAX_8RAID_GROUP_MEMBER) * pGauge->Width(), m_fPartyPlayerGaugeOffset + ( (i % MAX_8RAID_GROUP_MEMBER) * pGauge->Height()) );
		AddPartyPlayerGaugeList( pGauge , eMiniPartyPlayerGaugeType::eLeftType );
	}

#ifdef PRE_ADD_IMPROVE_MASTER_OBSERVER
	m_pVecPartyPlayerRightGageList.clear();

	for( DWORD i=0; i<eMiniGaugeSize::eMiniPlayerGaugeSize; i++ )
	{
		CDnGaugeDlg *pGauge = new CDnGaugeDlg( UI_TYPE_BOTTOM_MSG );
		pGauge->Initialize( CEtResourceMng::GetInstance().GetFullName( "MiniRightPlayerGauge.ui" ).c_str(), FT_MINI_PLAYER, false );
		pGauge->MoveDialog( (i / MAX_8RAID_GROUP_MEMBER) * -pGauge->Width(), m_fPartyPlayerGaugeOffset + ( (i % MAX_8RAID_GROUP_MEMBER) * pGauge->Height()) );
		AddPartyPlayerGaugeList( pGauge , eMiniPartyPlayerGaugeType::eRighType );
	}
#endif

}

void CDnInterface::AddPartyPlayerGaugeList( CDnGaugeDlg *pGauge , eMiniPartyPlayerGaugeType eType )	
{ 
	if( eType == eMiniPartyPlayerGaugeType::eLeftType )
	{
		PartyPlayerGauge sPartyPlayerGauge;
		sPartyPlayerGauge.pGaugeDlg = pGauge;
		m_pVecPartyPlayerGaugeList.push_back( sPartyPlayerGauge );
	}
#ifdef PRE_ADD_IMPROVE_MASTER_OBSERVER	
	if( eType == eMiniPartyPlayerGaugeType::eRighType )
	{

		PartyPlayerGauge sPartyPlayerGauge;
		sPartyPlayerGauge.pGaugeDlg = pGauge;
		m_pVecPartyPlayerRightGageList.push_back( sPartyPlayerGauge );
	}
#endif

}

bool CDnInterface::IsShowPartyPlayerGaugeList() 
{
	if( !m_pVecPartyPlayerGaugeList.empty() )
	{
		if( m_pVecPartyPlayerGaugeList[0].pGaugeDlg->IsShow() )
			return true;
	}
#ifdef PRE_ADD_IMPROVE_MASTER_OBSERVER
	if( !m_pVecPartyPlayerRightGageList.empty() )
	{
		if( m_pVecPartyPlayerRightGageList[0].pGaugeDlg->IsShow() )
			return true;
	}
#endif

	return false;
}

void CDnInterface::ShowPartyPlayerGaugeList( bool bShow )
{
	for( DWORD i=0; i<m_pVecPartyPlayerGaugeList.size(); i++ )
	{
		if( m_pVecPartyPlayerGaugeList[i].hActor )
			m_pVecPartyPlayerGaugeList[i].pGaugeDlg->Show( bShow );
	}

#ifdef PRE_ADD_IMPROVE_MASTER_OBSERVER
	for( DWORD i=0; i<m_pVecPartyPlayerRightGageList.size(); i++ )
	{
		if( m_pVecPartyPlayerRightGageList[i].hActor )
			m_pVecPartyPlayerRightGageList[i].pGaugeDlg->Show( bShow );
	}
#endif

}

void CDnInterface::ResetPartyPlayerGaugeList()
{
	for( DWORD i=0; i<m_pVecPartyPlayerGaugeList.size(); i++ )
	{
		m_pVecPartyPlayerGaugeList[i].hActor.Identity();
		m_pVecPartyPlayerGaugeList[i].pGaugeDlg->Show(false);
	}

#ifdef PRE_ADD_IMPROVE_MASTER_OBSERVER
	for( DWORD i=0; i<m_pVecPartyPlayerRightGageList.size(); i++ )
	{
		m_pVecPartyPlayerRightGageList[i].hActor.Identity();
		m_pVecPartyPlayerRightGageList[i].pGaugeDlg->Show(false);
	}
#endif
}

#ifdef PRE_ADD_PVP_SHOW_PARTY_GAUGE
void CDnInterface::ShowPvpPartyControl(bool bShow)
{
	if( m_pPlayerGaugeDlg )
		m_pPlayerGaugeDlg->ShowPvpPartyGauge(bShow);
}
#endif

void CDnInterface::ResetPartyPlayerGaugeAttributes( bool bMaster, bool bSelection )
{
	for( DWORD i=0; i<m_pVecPartyPlayerGaugeList.size(); i++ )
	{
		if( bMaster )
			m_pVecPartyPlayerGaugeList[i].pGaugeDlg->SetPartyMaster( false );
#ifdef PRE_ADD_IMPROVE_MASTER_OBSERVER
		if( bSelection )
			m_pVecPartyPlayerGaugeList[i].pGaugeDlg->SetPartySelection( false );
#endif
	}

#ifdef PRE_ADD_IMPROVE_MASTER_OBSERVER
	for( DWORD i=0; i<m_pVecPartyPlayerRightGageList.size(); i++ )
	{
		if( bMaster )
			m_pVecPartyPlayerRightGageList[i].pGaugeDlg->SetPartyMaster( false );
		if( bSelection )
			m_pVecPartyPlayerRightGageList[i].pGaugeDlg->SetPartySelection( false );
	}
#endif

}

void CDnInterface::SetPartyPlayerGaugeAttributes( UINT nSessionID, bool bEnable, bool bMaster, bool bSelect )
{
	for( DWORD i=0; i<m_pVecPartyPlayerGaugeList.size(); i++ )
	{
		if( m_pVecPartyPlayerGaugeList[i].hActor )
		{
			if( m_pVecPartyPlayerGaugeList[i].hActor->GetUniqueID() == nSessionID )
			{
				if( bMaster )
					m_pVecPartyPlayerGaugeList[i].pGaugeDlg->SetPartyMaster( bEnable );
#ifdef PRE_ADD_IMPROVE_MASTER_OBSERVER
				if( bSelect )
					m_pVecPartyPlayerGaugeList[i].pGaugeDlg->SetPartySelection( bEnable );
#endif
				return;
			}
		}
	}
#ifdef PRE_ADD_IMPROVE_MASTER_OBSERVER
	for( DWORD i=0; i<m_pVecPartyPlayerRightGageList.size(); i++ )
	{
		if( m_pVecPartyPlayerRightGageList[i].hActor) 
		{
			if(m_pVecPartyPlayerRightGageList[i].hActor->GetUniqueID() == nSessionID )
			{
				if( bMaster )
					m_pVecPartyPlayerRightGageList[i].pGaugeDlg->SetPartyMaster( bEnable );
				if( bSelect )
					m_pVecPartyPlayerRightGageList[i].pGaugeDlg->SetPartySelection( bEnable );
				return;
			}
		}
	}
#endif
}

void CDnInterface::UpdatePartyPlayerGaugePortrait( UINT nSessionID )
{
	for( DWORD i=0; i<m_pVecPartyPlayerGaugeList.size(); i++ )
	{
		if( m_pVecPartyPlayerGaugeList[i].hActor && m_pVecPartyPlayerGaugeList[i].hActor->IsPlayerActor() )
		{
			if( nSessionID > 0 && nSessionID != m_pVecPartyPlayerGaugeList[i].hActor->GetUniqueID() )
				continue;

			CDnPlayerActor *pPlayerActor = static_cast<CDnPlayerActor*>(m_pVecPartyPlayerGaugeList[i].hActor.GetPointer());
			std::string strTargetAction = pPlayerActor->IsSwapSingleSkin() ? "Stand" : "Normal_Stand";

			if( pPlayerActor->GetObjectHandle() )
			{
				int nAniIndex = pPlayerActor->GetCachedAniIndex( strTargetAction.c_str() );
				if( nAniIndex != -1 ) 
				{
					int nFaceID = pPlayerActor->IsSwapSingleSkin() ? pPlayerActor->GetSingleSkinActorID() - 1 : pPlayerActor->GetClassID() -1;
					m_pVecPartyPlayerGaugeList[i].pGaugeDlg->SetFaceID( nFaceID );
					m_pVecPartyPlayerGaugeList[i].pGaugeDlg->UpdatePortrait( m_pVecPartyPlayerGaugeList[i].hActor->GetObjectHandle(), false, false, nAniIndex, 0.0f );
				}
			}
		}
	}

#ifdef PRE_ADD_IMPROVE_MASTER_OBSERVER
	for( DWORD i=0; i<m_pVecPartyPlayerRightGageList.size(); i++ )
	{
		if( m_pVecPartyPlayerRightGageList[i].hActor && m_pVecPartyPlayerRightGageList[i].hActor->IsPlayerActor() )
		{
			if( nSessionID > 0 && nSessionID != m_pVecPartyPlayerRightGageList[i].hActor->GetUniqueID() )
				continue;

			CDnPlayerActor *pPlayerActor = static_cast<CDnPlayerActor*>(m_pVecPartyPlayerRightGageList[i].hActor.GetPointer());
			std::string strTargetAction = pPlayerActor->IsSwapSingleSkin() ? "Stand" : "Normal_Stand";

			if( pPlayerActor->GetObjectHandle() )
			{
				int nAniIndex = pPlayerActor->GetCachedAniIndex( strTargetAction.c_str() );
				if( nAniIndex != -1 ) 
				{
					int nFaceID = pPlayerActor->IsSwapSingleSkin() ? pPlayerActor->GetSingleSkinActorID() - 1 : pPlayerActor->GetClassID() -1;
					m_pVecPartyPlayerRightGageList[i].pGaugeDlg->SetFaceID( nFaceID );
					m_pVecPartyPlayerRightGageList[i].pGaugeDlg->UpdatePortrait( m_pVecPartyPlayerRightGageList[i].hActor->GetObjectHandle(), false, false, nAniIndex, 0.0f );
				}
			}
		}
	}
#endif
}

#ifdef PRE_ADD_IMPROVE_MASTER_OBSERVER

void CDnInterface::InsertPvpPlayerGauge( int nIndex , DnActorHandle hActor , eMiniPartyPlayerGaugeType eType )
{
	if( nIndex >= eMiniGaugeSize::eMiniPlayerGaugeSize || nIndex < 0 )
		return;

	if( !hActor || ( hActor && !hActor->IsPlayerActor() ) )
		return;

	PartyPlayerGauge *pGaugeInfo = NULL;

	if( eType == eMiniPartyPlayerGaugeType::eLeftType )
	{
		pGaugeInfo = &m_pVecPartyPlayerGaugeList[nIndex];
	}
	else if(eType == eMiniPartyPlayerGaugeType::eRighType )
	{
		pGaugeInfo = &m_pVecPartyPlayerRightGageList[nIndex];
	}

	if( pGaugeInfo && pGaugeInfo->hActor != hActor )
	{
		pGaugeInfo->hActor = hActor;

		if( pGaugeInfo->pGaugeDlg )
		{
			pGaugeInfo->pGaugeDlg->SetFaceID( hActor->GetClassID() - 1 ); //rlkt_fix
			//pGaugeInfo->pGaugeDlg->SetPartyIconIndex(hActor->pGaugeDlg->nVecJobHistoryList[hActor->pGaugeDlg->nVecJobHistoryList.size() - 1]); //damn :))
			//pGaugeInfo->pGaugeDlg->SetPartyIconIndex(hActor->OnGetJobClassID());
			pGaugeInfo->pGaugeDlg->SetSessionID( hActor->GetUniqueID() );

#ifdef PRE_WORLDCOMBINE_PARTY
			pGaugeInfo->pGaugeDlg->SetPlayerName( hActor->GetLevel() , hActor->GetName() , GAUGE_PLAYERNAME_NO_COLOR, Party::Constants::INVALID_SERVERID );
#else
			pGaugeInfo->pGaugeDlg->SetPlayerName( hActor->GetLevel() , hActor->GetName() );
#endif
			
			CDnPlayerActor *pPlayerActor = static_cast<CDnPlayerActor*>( hActor.GetPointer());
			std::string strTargetAction = pPlayerActor->IsSwapSingleSkin() ? "Stand" : "Normal_Stand";

			if( pPlayerActor->GetObjectHandle() )
			{
				int nAniIndex = pPlayerActor->GetCachedAniIndex( strTargetAction.c_str() );
				if( nAniIndex != -1 ) 
				{
					int nFaceID = pPlayerActor->IsSwapSingleSkin() ? pPlayerActor->GetSingleSkinActorID() - 1 : pPlayerActor->GetClassID() -1;
					pGaugeInfo->pGaugeDlg->SetFaceID( nFaceID );
					pGaugeInfo->pGaugeDlg->UpdatePortrait( hActor->GetObjectHandle(), false, false, nAniIndex, 0.0f );
				}
			} 
			
		}

		if( pGaugeInfo->hActor && pGaugeInfo->hActor->IsPlayerActor() )
		{
			CDnPlayerActor *pPlayer = static_cast<CDnPlayerActor*>(pGaugeInfo->hActor.GetPointer());
			bool bIsCaptaion = pPlayer->IsCaptainMode();
			SetPartyPlayerGaugeAttributes( pPlayer->GetUniqueID() , bIsCaptaion , true , false );
		}
	}
}

void CDnInterface::RemovePvpPlayerGauge( int nIndex, eMiniPartyPlayerGaugeType eType )
{
	if( nIndex >= eMiniGaugeSize::eMiniPlayerGaugeSize || nIndex < 0 )
		return;

	PartyPlayerGauge *pGaugeInfo = NULL;

	if( eType == eMiniPartyPlayerGaugeType::eLeftType )
	{
		pGaugeInfo = &m_pVecPartyPlayerGaugeList[nIndex];
	}
	else if(eType == eMiniPartyPlayerGaugeType::eRighType )
	{
		pGaugeInfo = &m_pVecPartyPlayerRightGageList[nIndex];
	}

	if( pGaugeInfo )
	{
		pGaugeInfo->hActor.Identity();
		pGaugeInfo->pGaugeDlg->Show( false );
		pGaugeInfo->pGaugeDlg->SetPartyMaster( false );
		pGaugeInfo->pGaugeDlg->SetPartySelection( false );
	}
}

void CDnInterface::RefreshPvpPlayerGauge()
{
	CDnGameTask* pGameTask = (CDnGameTask*)CTaskManager::GetInstance().GetTask( "GameTask" );
	if( pGameTask && pGameTask->GetGameTaskType() == GameTaskType::PvP ) 
	{
		CDnPvPGameTask *pPvpGameTask = (CDnPvPGameTask*)pGameTask;

		switch( pPvpGameTask->GetGameMode() )
		{
		case PvPCommon::GameMode::PvP_GuildWar:
		case PvPCommon::GameMode::PvP_Racing:
			return;
		default:
			break;
		}


		if( CDnActor::s_hLocalActor )
		{
			if( CDnActor::s_hLocalActor->GetTeam() == PvPCommon::Team::Observer && 
				static_cast<CDnLocalPlayerActor*>(CDnActor::s_hLocalActor.GetPointer())->IsDeveloperAccountLevel() == false)
			{
				return;
			}

			bool bIsShowPartyGauge = IsShowPartyPlayerGaugeList();
			int nLeftIndex = 0;
			int nRightIndex = 0;

			for( DWORD i=0; i<CDnPartyTask::GetInstance().GetPartyCount(); i++ ) 
			{
				CDnPartyTask::PartyStruct *pInfo = CDnPartyTask::GetInstance().GetPartyData(i);
				if( pInfo && pInfo->hActor )
				{
					if( pInfo->hActor->GetTeam() == PvPCommon::Team::Observer )
						continue;

					if( CDnActor::s_hLocalActor->GetTeam() == PvPCommon::Team::Observer )
					{
						if( pPvpGameTask->GetGameMode() == PvPCommon::GameMode::PvP_Tournament || 
#ifdef PRE_ADD_PVP_TOURNAMENT
							pPvpGameTask->GetGameMode() == PvPCommon::GameMode::PvP_AllKill 
#endif
							)
						{
							if( pPvpGameTask->IsMatchedBattlePlayer( pInfo->nSessionID ) == false )
								continue;
						}
						if( pPvpGameTask->GetGameMode() == PvPCommon::GameMode::PvP_IndividualRespawn )
						{
							InsertPvpPlayerGauge( 
								nLeftIndex < eMiniGaugeSize::eMiniPlayerGaugeSize ? nLeftIndex : ( nLeftIndex - eMiniGaugeSize::eMiniPlayerGaugeSize ), 
								pInfo->hActor,
								nLeftIndex < eMiniGaugeSize::eMiniPlayerGaugeSize ? eMiniPartyPlayerGaugeType::eLeftType : eMiniPartyPlayerGaugeType::eRighType );

							nLeftIndex++;
						}
						else
						{
							if( pInfo->hActor->GetTeam() == PvPCommon::Team::A )
							{
								InsertPvpPlayerGauge( nLeftIndex , pInfo->hActor , eMiniPartyPlayerGaugeType::eLeftType );
								nLeftIndex++;
							}
							else if( pInfo->hActor->GetTeam() == PvPCommon::Team::B )
							{
								InsertPvpPlayerGauge( nRightIndex , pInfo->hActor , eMiniPartyPlayerGaugeType::eRighType );
								nRightIndex++;
							}
						}
					} 
					else
					{
						if( pPvpGameTask->IsShowPartyGauge() == true )
						{
							if( pInfo->hActor->GetTeam() == CDnActor::s_hLocalActor->GetTeam() && pInfo->hActor->GetUniqueID() != CDnActor::s_hLocalActor->GetUniqueID() )
							{
								InsertPvpPlayerGauge( nLeftIndex , pInfo->hActor , eMiniPartyPlayerGaugeType::eLeftType );
								nLeftIndex++;
							}
						}
					}
				}
			}

			for( int i=nLeftIndex; i<eMiniGaugeSize::eMiniPlayerGaugeSize; i++ )
				RemovePvpPlayerGauge( i , eMiniPartyPlayerGaugeType::eLeftType );

			for( int i=nRightIndex; i<eMiniGaugeSize::eMiniPlayerGaugeSize; i++ )
				RemovePvpPlayerGauge( i , eMiniPartyPlayerGaugeType::eRighType );

#ifdef PRE_ADD_PVP_SHOW_PARTY_GAUGE
			ShowPartyPlayerGaugeList( m_pPlayerGaugeDlg->CheckShowPvpPartyGauge() );
#else
			ShowPartyPlayerGaugeList( CDnActor::s_hLocalActor->GetTeam() == PvPCommon::Team::Observer ? bIsShowPartyGauge : true );
#endif
		}
	}
}

#endif

void CDnInterface::AddMonsterGaugeList( CDnGaugeDlg *pGauge )
{
	EnemyGauge sEnemyGauge;
	sEnemyGauge.pGaugeDlg = pGauge;
	m_pVecEnemyGaugeList.push_back( sEnemyGauge );
}

void CDnInterface::RefreshPlayerDurabilityIcon()
{
	if( m_pPlayerGaugeDlg )
		m_pPlayerGaugeDlg->RefreshDurabilityIcon();
}

void CDnInterface::ShowEnemyGauge( DnActorHandle hActor, bool bCrossHair )
{
	CDnMonsterActor *pActor = dynamic_cast<CDnMonsterActor *>(hActor.GetPointer());
	if( !pActor ) return;


	if( pActor->GetGrade() >= CDnMonsterState::Named )
	{
		if( !bCrossHair )
		{
			CDnGameTask * pGameTask = (CDnGameTask *)CTaskManager::GetInstance().GetTask( "GameTask" );
			if( pGameTask && GameTaskType::PvP == pGameTask->GetGameTaskType() && eGuildWar_Room == m_ePVPRoomState )
				ShowGuildWarBossGauge( hActor );
			else
				ShowBossGauge( hActor );
		}
//		return;
	}

	// 투명 상태효과 걸린 몬스터는 게이지 보여주지 않는다.
	if( pActor->GetStateBlow()->IsApplied( STATE_BLOW::BLOW_073 ) )
		return;

	if( m_pEnemyGaugeMng ) m_pEnemyGaugeMng->SetEnenmy( hActor );
}

void CDnInterface::HideEnemyGauge( DnActorHandle hActor )
{
	CDnMonsterActor *pActor = dynamic_cast<CDnMonsterActor *>(hActor.GetPointer());
	if( !pActor ) return;
	if( pActor->GetGrade() >= CDnMonsterState::Named ) return;

	if( m_pEnemyGaugeMng ) m_pEnemyGaugeMng->DeleteEnemy( hActor );
}

void CDnInterface::ShowPlayerGauge( DnActorHandle hActor , bool bShow )
{
	CDnPlayerActor *pActor = dynamic_cast<CDnPlayerActor *>(hActor.GetPointer());
	if( !pActor ) return;	

	if( m_pEnemyGaugeMng ) {
		if( bShow )
			m_pEnemyGaugeMng->SetPlayer( hActor );
		else
			m_pEnemyGaugeMng->DeletePlayer( hActor );
	}
}

#ifdef PRE_ADD_SHOW_MONACTION
bool CDnInterface::IsShowEnemyGauge(DnActorHandle hActor) const
{
	if (m_pEnemyGaugeMng)
		return m_pEnemyGaugeMng->IsEnemyGaugeInList(hActor);
	return false;
}
#endif


void CDnInterface::ProcessMonsterPartsGauge( DnActorHandle hActor )
{
	for(DWORD i=0; i<m_pVecMonsterPartsGuageList.size() ; i++ )
		m_pVecMonsterPartsGuageList[i]->Show(false);

	if(hActor && hActor->IsPartsMonsterActor())
	{
		CDnPartsMonsterActor* pPartsMonsterActor = dynamic_cast<CDnPartsMonsterActor*>(hActor.GetPointer());
		if(pPartsMonsterActor)
		{
			int nMaxPartsIndex = pPartsMonsterActor->GetPartsSize();

			if(nMaxPartsIndex > PartsMonster::Common::UI::Max_PartsSize)
				nMaxPartsIndex = PartsMonster::Common::UI::Max_PartsSize;

			for(int i=0;i<nMaxPartsIndex; i++)
			{
				if(!m_pVecMonsterPartsGuageList[i])
					break;

				MonsterParts *pMonsterParts = pPartsMonsterActor->GetPartsByIndex(i);
				if(!pMonsterParts || !pMonsterParts->IsEnalbeUIParts())
					continue;

				float fHP = 0.f;

				if( pMonsterParts->GetMaxHP() > 0 )
				{
					fHP = pMonsterParts->GetHP() / ( float )pMonsterParts->GetMaxHP();
				}

				m_pVecMonsterPartsGuageList[i]->SetHP( fHP );
#ifdef PRE_WORLDCOMBINE_PARTY
				m_pVecMonsterPartsGuageList[i]->SetPlayerName(-1,pMonsterParts->GetPartsName().c_str(), GAUGE_PLAYERNAME_NO_COLOR, Party::Constants::INVALID_SERVERID);
#else
				m_pVecMonsterPartsGuageList[i]->SetPlayerName(-1,pMonsterParts->GetPartsName().c_str());
#endif
				m_pVecMonsterPartsGuageList[i]->Show(true);
			}
		}
	}
}

void CDnInterface::HideMonsterPartsGauge()
{
	for( int i=0; i<((int)m_pVecMonsterPartsGuageList.size()); i++ )
	{
		if(m_pVecMonsterPartsGuageList[i])
			m_pVecMonsterPartsGuageList[i]->Show(false);
	}
}


void CDnInterface::ProcessPlayerGauge()
{
	if( !m_pPlayerGaugeDlg )
		return;

	DnActorHandle hActor = CDnActor::s_hLocalActor;
	if( !hActor ) 
		return;

	CDnGaugeDlg::eGagueMode eGaugeMode = CDnGaugeDlg::eGagueMode::Normal;

	CDnLocalPlayerActor *pPlayer = static_cast<CDnLocalPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
	if( pPlayer->IsFollowObserver() && pPlayer->GetFollowObserverUniqueID() != 0 )
	{
		CDnActor *pFollowObserver = CDnActor::FindActorFromUniqueID( pPlayer->GetFollowObserverUniqueID());
		if( pFollowObserver )
		{
			hActor = pFollowObserver->GetActorHandle();

			if(IsLadderMode())
				eGaugeMode = CDnGaugeDlg::eGagueMode::Protected;
			else
				eGaugeMode = CDnGaugeDlg::eGagueMode::Percentage;
		}			
	}	

	m_pPlayerGaugeDlg->SetHP( hActor->GetHP(), hActor->GetMaxHP() , eGaugeMode );
	m_pPlayerGaugeDlg->SetSP( hActor->GetSP(), hActor->GetMaxSP() , eGaugeMode );

#ifdef PRE_WORLDCOMBINE_PARTY
	m_pPlayerGaugeDlg->SetPlayerName( hActor->GetLevel(), hActor->GetName(), GAUGE_PLAYERNAME_NO_COLOR, Party::Constants::INVALID_SERVERID );
#else
	m_pPlayerGaugeDlg->SetPlayerName( hActor->GetLevel(), hActor->GetName() );
#endif
	m_pPlayerGaugeDlg->ProcessBuffs(hActor);

	
	CDnItemTask *pTask = (CDnItemTask *)CTaskManager::GetInstance().GetTask( "ItemTask" );
	if( pTask )
		m_pPlayerGaugeDlg->UpdateRebirthCoin();
}

#ifdef PRE_ADD_BUFF_ADD_INFORMATION
void CDnInterface::ProcessVillagePartyPlayerGauge()
{
	for( int i=0; i<(int)m_pVecPartyPlayerGaugeList.size(); i++ )
	{
		if( m_pVecPartyPlayerGaugeList[i].pGaugeDlg->GetSessionID() > 0 )
			m_pVecPartyPlayerGaugeList[i].pGaugeDlg->ProcessVillagePartyBuff();
	}
}
#endif

void CDnInterface::ProcessPartyPlayerGauge()
{
	DnActorHandle hActor;
	float fHP(0.0f), fSP(0.0f);

	for( int i=0; i<(int)m_pVecPartyPlayerGaugeList.size(); i++ )
	{
		hActor = m_pVecPartyPlayerGaugeList[i].hActor;
		if( !hActor ) continue;
		if( !m_pVecPartyPlayerGaugeList[i].pGaugeDlg->IsShow() ) continue;

		if( hActor->GetMaxHP() == 0 ) fHP = 0.f;
		else fHP = hActor->GetHP() / ( float )hActor->GetMaxHP();

		if( hActor->GetMaxSP() == 0 ) fSP = 0.f;
		else fSP = hActor->GetSP() / ( float )hActor->GetMaxSP();

		m_pVecPartyPlayerGaugeList[i].pGaugeDlg->SetHP( fHP );
		m_pVecPartyPlayerGaugeList[i].pGaugeDlg->SetSP( fSP );
		m_pVecPartyPlayerGaugeList[i].pGaugeDlg->ProcessBuffs(hActor);
	}

#ifdef PRE_ADD_IMPROVE_MASTER_OBSERVER
	for( int i=0; i<(int)m_pVecPartyPlayerRightGageList.size(); i++ )
	{
		hActor = m_pVecPartyPlayerRightGageList[i].hActor;
		if( !hActor ) continue;
		if( !m_pVecPartyPlayerRightGageList[i].pGaugeDlg->IsShow() ) continue;

		if( hActor->GetMaxHP() == 0 ) fHP = 0.f;
		else fHP = hActor->GetHP() / ( float )hActor->GetMaxHP();

		if( hActor->GetMaxSP() == 0 ) fSP = 0.f;
		else fSP = hActor->GetSP() / ( float )hActor->GetMaxSP();

		m_pVecPartyPlayerRightGageList[i].pGaugeDlg->SetHP( fHP );
		m_pVecPartyPlayerRightGageList[i].pGaugeDlg->SetSP( fSP );
		m_pVecPartyPlayerRightGageList[i].pGaugeDlg->ProcessBuffs(hActor);
	}
#endif

}

void CDnInterface::ProcessMonsterGauge()
{
	float fHP;

	// Note : 보스 게이지
	//
	for (int i = 0; i<(int)m_pVecEnemyGaugeList.size(); i++)
	{
		DnActorHandle hActor = m_pVecEnemyGaugeList[i].hActor;
		bool bPartsBodyCheck = false;
#ifdef PRE_ADD_CHECK_PART_MONSTER
		if (m_pVecEnemyGaugeList[0].hActor)
			bPartsBodyCheck = m_pVecEnemyGaugeList[0].hActor->IsEnablePartsHPDisplay();
#else
		if (m_pVecEnemyGaugeList[0].hActor)
			bPartsBodyCheck = m_pVecEnemyGaugeList[0].hActor->IsPartsMonsterActor();
#endif // PRE_ADD_CHECK_PART_MONSTER

		if (!(hActor) || hActor->IsDie() || (i != 0 && bPartsBodyCheck))
		{
			HideBossGauge(i);
		}
		else
		{
			if (hActor->GetMaxHP() == 0)
			{
				fHP = 0.f;
			}
			else
			{
				fHP = hActor->GetHP() / (float)hActor->GetMaxHP();
			}

			m_pVecEnemyGaugeList[i].pGaugeDlg->SetHP(fHP);

			m_pVecEnemyGaugeList[i].pGaugeDlg->SetSP(hActor->GetMaxSuperArmor() <= 50.0f ? 0.0f : ((float)hActor->GetCurrentSuperArmor() / hActor->GetMaxSuperArmor()));	// 슈퍼아머			
			m_pVecEnemyGaugeList[i].pGaugeDlg->SetFaceID(hActor->GetClassID() - 1);
#ifdef PRE_WORLDCOMBINE_PARTY
			m_pVecEnemyGaugeList[i].pGaugeDlg->SetPlayerName(hActor->GetLevel(), hActor->GetName(), GAUGE_PLAYERNAME_NO_COLOR, Party::Constants::INVALID_SERVERID);
#else
			m_pVecEnemyGaugeList[i].pGaugeDlg->SetPlayerName(hActor->GetLevel(), hActor->GetName());
#endif

			CDnMonsterActor *pActor = dynamic_cast<CDnMonsterActor *>(hActor.GetPointer());
			if (pActor) {
				m_pVecEnemyGaugeList[i].pGaugeDlg->SetMonsterGrade(pActor->GetGrade());
			}

			if (hActor->GetMaxSuperArmor() <= 50.0f) {
				m_pVecEnemyGaugeList[i].pGaugeDlg->SetImmediateChange(false);
			}

			int nMaxSuperArmor = hActor->GetMaxSuperArmor();
			if (nMaxSuperArmor <= 50)
				m_pVecEnemyGaugeList[i].pGaugeDlg->SetProgressIndex(CDnGaugeDlg::eProgressBarType::SPBar, 0);
			else if (nMaxSuperArmor <= 200)
				m_pVecEnemyGaugeList[i].pGaugeDlg->SetProgressIndex(CDnGaugeDlg::eProgressBarType::SPBar, 1);
			else if (nMaxSuperArmor <= 600)
				m_pVecEnemyGaugeList[i].pGaugeDlg->SetProgressIndex(CDnGaugeDlg::eProgressBarType::SPBar, 2);
			else if (nMaxSuperArmor <= 1800)
				m_pVecEnemyGaugeList[i].pGaugeDlg->SetProgressIndex(CDnGaugeDlg::eProgressBarType::SPBar, 3);
			else
				m_pVecEnemyGaugeList[i].pGaugeDlg->SetProgressIndex(CDnGaugeDlg::eProgressBarType::SPBar, 4);

			if (i == 0 && hActor->IsPartsMonsterActor())
				ProcessMonsterPartsGauge(hActor);
			else if (i == 0 && !hActor->IsPartsMonsterActor())
				HideMonsterPartsGauge();
		}
	}
}

void CDnInterface::UpdateBossPortrait(int nIndex)
{
	if ((int)m_pVecEnemyGaugeList.size() <= nIndex) return;
	if (!m_pVecEnemyGaugeList[nIndex].hActor) return;
	if (!m_pVecEnemyGaugeList[nIndex].hActor->GetObjectHandle()) return;

	m_pVecEnemyGaugeList[nIndex].pGaugeDlg->SetFaceID(m_pVecEnemyGaugeList[nIndex].hActor->GetClassID() - 1);
	EtObjectHandle hObject = CreateStaticObject(m_pVecEnemyGaugeList[nIndex].hActor->GetObjectHandle()->GetSkinFileName());
	if (!hObject) return;
	m_pVecEnemyGaugeList[nIndex].pGaugeDlg->UpdatePortrait(hObject, true, true);
}

void CDnInterface::ShowGuildWarBossGauge(DnActorHandle hActor)
{
	DnMonsterActorHandle hMonsterActor = (DnMonsterActorHandle)hActor;
	if (m_pGuildWarBossGaugeDlg->SetActor(hMonsterActor))
		m_pGuildWarBossGaugeDlg->Show(true);
}

void CDnInterface::ShowBossGauge(DnActorHandle hActor)
{
	for (int i = 0; i<(int)m_pVecEnemyGaugeList.size(); i++)
	{
		if (m_pVecEnemyGaugeList[i].hActor == hActor)
		{
			if (i == 0 && m_pVecEnemyGaugeList[i].pGaugeDlg->IsShow()) {
				return;
			}
			HideBossGauge(i);
			break;
		}
	}

	for (int i = (int)m_pVecEnemyGaugeList.size() - 1; i>0; i--)
	{
		if (m_pVecEnemyGaugeList[i - 1].pGaugeDlg->IsShow())
		{
			if (m_pVecEnemyGaugeList[i].hActor != m_pVecEnemyGaugeList[i - 1].hActor) {
				m_pVecEnemyGaugeList[i].hActor = m_pVecEnemyGaugeList[i - 1].hActor;
				m_pVecEnemyGaugeList[i].pGaugeDlg->SetImmediateChange(true);
				m_pVecEnemyGaugeList[i].pGaugeDlg->SetImmediateChange(false);
				UpdateBossPortrait(i);
			}
			m_pVecEnemyGaugeList[i].pGaugeDlg->Show(true);
		}
	}

	if (!m_pVecEnemyGaugeList.empty()) {
		if (m_pVecEnemyGaugeList[0].hActor != hActor) {
			m_pVecEnemyGaugeList[0].hActor = hActor;
			m_pVecEnemyGaugeList[0].pGaugeDlg->SetImmediateChange(true);
			m_pVecEnemyGaugeList[0].pGaugeDlg->SetImmediateChange(false);
			UpdateBossPortrait(0);
		}
		m_pVecEnemyGaugeList[0].pGaugeDlg->Show(true);
	}
}

void CDnInterface::HideBossGauge(int nIndex)
{
	m_pVecEnemyGaugeList[nIndex].hActor.Identity();
	m_pVecEnemyGaugeList[nIndex].pGaugeDlg->Show(false);

	for (int i = nIndex; i<((int)m_pVecEnemyGaugeList.size()) - 1; i++)
	{
		if (m_pVecEnemyGaugeList[i + 1].pGaugeDlg->IsShow())
		{
			if (m_pVecEnemyGaugeList[i].hActor != m_pVecEnemyGaugeList[i + 1].hActor) {
				m_pVecEnemyGaugeList[i].hActor = m_pVecEnemyGaugeList[i + 1].hActor;
				m_pVecEnemyGaugeList[i].pGaugeDlg->SetImmediateChange(true);
				m_pVecEnemyGaugeList[i].pGaugeDlg->SetImmediateChange(false);
				UpdateBossPortrait(i);
			}
			m_pVecEnemyGaugeList[i].pGaugeDlg->Show(true);
			HideBossGauge(i + 1);
		}
	}

	if (nIndex == 0)
		HideMonsterPartsGauge();
}

void CDnInterface::SelectShortestTime_AcceptRequestDlg(eAcceptReqType type, float& shortest_time, float comparee_time)
{
	if (comparee_time != _INVALID_TIME_VALUE)
	{
		if (shortest_time == _INVALID_TIME_VALUE)	{ shortest_time = comparee_time; m_CurAcceptReqDlg = type; }
		else if (shortest_time > comparee_time)		m_CurAcceptReqDlg = type;
	}
}

#ifdef PRE_FIX_QUICKPVP_EXCLUSIVEREQ
void CDnInterface::ClearAcceptReqDlg(bool bReject /*= false*/)
{
	m_CurAcceptReqDlg = eNONE;
	GetInstance().CloseAcceptRequestDialog(bReject);
}
#else
void CDnInterface::ClearAcceptReqDlg()
{
	m_CurAcceptReqDlg = eNONE;
	GetInstance().CloseAcceptRequestDialog();
}
#endif

void CDnInterface::ProcessAcceptRequestDialog()
{
	if (m_CurAcceptReqDlg != eNONE)
	{
		switch(m_CurAcceptReqDlg)
		{
		case eTRADE:
			{
				float fExchange = GetTradeTask().GetTradePrivateMarket().GetShortestAcceptTime();
				if (fExchange != _INVALID_TIME_VALUE)	{ GetTradeTask().GetTradePrivateMarket().OpenAcceptRequestDialog(); }
				else									{ ClearAcceptReqDlg(); }
			}
			break;
		case ePARTY:
			{
				float fParty = GetPartyTask().GetShortestAcceptTime();
				if (fParty != _INVALID_TIME_VALUE)		{ GetPartyTask().OpenAcceptRequestDialog(); }
				else									{ ClearAcceptReqDlg(); }
			}
			break;
		case eMASTER:
			{
				float fMaster = GetMasterTask().GetShortestAcceptTime();
				if (fMaster != _INVALID_TIME_VALUE)		{ GetMasterTask().OpenAcceptRequestDialog(); }
				else									{ ClearAcceptReqDlg(); }
			}
			break;
#ifdef PRE_ADD_QUICK_PVP
		case eDUEL:
			{
				CDnCommonTask *pCommonTask = static_cast<CDnCommonTask *>(CTaskManager::GetInstance().GetTask("CommonTask"));
				if (pCommonTask)
				{
					float fAccept = pCommonTask->GetQuickPvPShortestAcceptTime();
					if (fAccept != _INVALID_TIME_VALUE)		{ pCommonTask->OpenQuickPvPAcceptRequestDialog(); }
					else									{ ClearAcceptReqDlg(); }
				}
			}
			break;
#endif
		default:
			ClearAcceptReqDlg();
			break;
		}
	}
	else
	{
		float shortest	= _INVALID_TIME_VALUE;
		if( CDnTradeTask::IsActive() )	SelectShortestTime_AcceptRequestDlg(eTRADE, shortest, GetTradeTask().GetTradePrivateMarket().GetShortestAcceptTime());
		if( CDnPartyTask::IsActive() )	SelectShortestTime_AcceptRequestDlg(ePARTY, shortest, GetPartyTask().GetShortestAcceptTime());
		if( CDnMasterTask::IsActive() )	SelectShortestTime_AcceptRequestDlg(eMASTER, shortest, GetMasterTask().GetShortestAcceptTime());
#ifdef PRE_ADD_QUICK_PVP
		CDnCommonTask *pCommonTask = static_cast<CDnCommonTask *>(CTaskManager::GetInstance().GetTask("CommonTask"));
		if (pCommonTask)
		{
			SelectShortestTime_AcceptRequestDlg(eDUEL, shortest, pCommonTask->GetQuickPvPShortestAcceptTime());
		}
#endif
	}
}

bool CDnInterface::Process( LOCAL_TIME LocalTime, float fDelta )
{
	ProcessInput( LocalTime, fDelta );

	static bool (CDnInterface::*fp[InterfaceTypeMax])(LOCAL_TIME LocalTime, float fDelta) =
	{
		&CDnInterface::ProcessTitle,
		&CDnInterface::ProcessLogin,
		&CDnInterface::ProcessVillage,
		&CDnInterface::ProcessPVPVillage,
		&CDnInterface::ProcessGame,
		&CDnInterface::ProcessPVPGame,
		&CDnInterface::ProcessDLGame,
		&CDnInterface::ProcessFarmGame,
	};

	m_bProcessMainProcess = true;

#ifdef PRE_MOD_BROWSER
	ProcessDisableDlgs(fDelta);
	m_InternetBrowserMgr.ProcessBrowser();
#endif
#ifdef PRE_ADD_START_POPUP_QUEUE
	m_StartPopupMgr.Process(fDelta);
#endif

	return (this->*fp[m_Type])(LocalTime, fDelta);
}

void CDnInterface::PostProcess( LOCAL_TIME LocalTime, float fDelta )
{
	ProcessCountDown( fDelta );
	m_bProcessMainProcess = false;
}

bool CDnInterface::ProcessTitle( LOCAL_TIME LocalTime, float fDelta )
{
	if( m_nCurLogoIndex >= (int)m_pVecLogoList.size() ) 
		return false;

	m_fLogoDelta -= fDelta;

	if( m_fLogoDelta <= 0.f ) 
	{
		m_nCurLogoIndex++;
		if( m_nCurLogoIndex >= (int)m_pVecLogoList.size() ) 
			return false;

		if( m_nCurLogoIndex > 0 )
		{
			CEtUIDialog* pDlg = m_pVecLogoList[m_nCurLogoIndex - 1];
			if ( pDlg == NULL )
			{
				::MessageBox(NULL,L"error",NULL,0);
			}
			pDlg->Show( false );
		}

		m_fLogoDelta = LOGO_DELAY_TIME;
		OutputDebug("m_nCurLogoIndex %d %d\n", m_pVecLogoList.size(), m_nCurLogoIndex);
		CEtUIDialog* pDlg = m_pVecLogoList[m_nCurLogoIndex];
		if ( pDlg == NULL )
		{
			
			::MessageBox(NULL,L"error",NULL,0);
		}
		pDlg->Show( true );

	}

	if( m_nCurLogoIndex < (int)m_pVecLogoList.size() ) 
	{
		float fFadeDelta = LOGO_DELAY_TIME * 0.15f;

		if( m_fLogoDelta > LOGO_DELAY_TIME - fFadeDelta ) 
		{
			float fAlpha = ( 1.f / fFadeDelta ) * ( LOGO_DELAY_TIME - m_fLogoDelta );
			m_pVecLogoList[m_nCurLogoIndex]->SetRenderColor(D3DCOLOR_ARGB( (BYTE)( fAlpha * 255 ), 255, 255, 255 ));
		}
		else if( m_fLogoDelta < fFadeDelta ) 
		{
			float fAlpha = ( 1.f / fFadeDelta ) * ( m_fLogoDelta );
			m_pVecLogoList[m_nCurLogoIndex]->SetRenderColor(D3DCOLOR_ARGB( (BYTE)( fAlpha * 255 ), 255, 255, 255 ));
		}
		else 
		{
			m_pVecLogoList[m_nCurLogoIndex]->SetRenderColor(D3DCOLOR_ARGB( 255, 255, 255, 255 ));
		}
	}

	if( m_pTextureDlgMng ) m_pTextureDlgMng->Process( fDelta );
	if( m_pTextBoxDlgMng ) m_pTextBoxDlgMng->Process( fDelta );

	return true;
}

bool CDnInterface::ProcessLogin( LOCAL_TIME LocalTime, float fDelta )
{
	if( m_pTextureDlgMng ) m_pTextureDlgMng->Process( fDelta );
	if( m_pTextBoxDlgMng ) m_pTextBoxDlgMng->Process( fDelta );
	return true;
}

bool CDnInterface::ProcessVillage( LOCAL_TIME LocalTime, float fDelta )
{
	ProcessDefaultGameUI( LocalTime, fDelta );

#ifdef PRE_ADD_BUFF_ADD_INFORMATION
	ProcessVillagePartyPlayerGauge();
#endif

	//CDnVillageTask *pTask;
	//pTask = dynamic_cast<CDnVillageTask *>(m_pTask);

	return true;
}

bool CDnInterface::ProcessGame( LOCAL_TIME LocalTime, float fDelta )
{
	ProcessDefaultGameUI( LocalTime, fDelta );
	ProcessPartyPlayerGauge();
	ProcessMonsterGauge();

	// Note : 몬스터 게이지
	//
	if( m_pDamageMng ) m_pDamageMng->Process( fDelta );
	if( m_pComboMng ) m_pComboMng->Process( fDelta );
	if( m_pChainMng ) m_pChainMng->Process( fDelta );
	if( m_pEnemyGaugeMng ) m_pEnemyGaugeMng->Process( fDelta );
	if (m_pDurabilityGaugeMng) m_pDurabilityGaugeMng->Process( fDelta );

	CDnGameTask * pGameTask = (CDnGameTask *)CTaskManager::GetInstance().GetTask( "GameTask" );
	if( pGameTask && pGameTask->GetGameTaskType() != GameTaskType::Farm )
	{
		if( m_pCpJudgeMng ) m_pCpJudgeMng->Process( fDelta );
		if( m_pCpScoreMng ) m_pCpScoreMng->Process( fDelta );
		if( m_pCpDownMng ) m_pCpDownMng->Process(fDelta);
#ifdef PRE_ADD_CRAZYDUC_UI
		if( m_pScoreMng ) m_pScoreMng->Process( fDelta );
#endif 
	}

	return true;
}

bool CDnInterface::ProcessPVPGame( LOCAL_TIME LocalTime, float fDelta )
{
	ProcessDefaultGameUI( LocalTime, fDelta );
	ProcessMonsterGauge();

#ifdef PRE_ADD_IMPROVE_MASTER_OBSERVER
	ProcessPartyPlayerGauge();
#endif

	// Note : 몬스터 게이지
	if( m_pDamageMng ) m_pDamageMng->Process( fDelta );
	if( m_pComboMng ) m_pComboMng->Process( fDelta );
	if( m_pChainMng ) m_pChainMng->Process( fDelta );
	if( m_pEnemyGaugeMng ) m_pEnemyGaugeMng->Process( fDelta );

	if( m_pGuildWarSituation )	m_pGuildWarSituation->Process( fDelta );
	if( m_pGuildWarZoneMapDlg ) m_pGuildWarZoneMapDlg->Process( fDelta );
	if( m_pGuildPropInfoDlg )	m_pGuildPropInfoDlg->Process( fDelta );

	return true;
}

bool CDnInterface::ProcessPVPVillage( LOCAL_TIME LocalTime, float fDelta )
{
	return ProcessDefaultGameUI( LocalTime, fDelta );
}

bool CDnInterface::ProcessDefaultGameUI( LOCAL_TIME LocalTime, float fDelta )
{
	ProcessPlayerGauge();
	ProcessAcceptRequestDialog();

	m_pTextureDlgMng->Process( fDelta );
	m_pTextBoxDlgMng->Process( fDelta );

#ifdef PRE_SWAP_QUICKSLOT
	ProcessSwapQuickSlot();
#endif

	return true;
}

#ifdef PRE_SWAP_QUICKSLOT
void CDnInterface::ProcessSwapQuickSlot()
{
	if( GetChatDialog() && !GetChatDialog()->IsEditBoxFocus() )
	{
		DnActorHandle hActor = CDnActor::s_hLocalActor;
		if( !hActor ) return;

		CDnPlayerActor *pActor = static_cast<CDnPlayerActor *>(CDnActor::s_hLocalActor.GetPointer());
		if( !pActor->IsBattleMode() ) return;

		if( IsPushKey( IW( IW_TOGGLEMOUSE ) ) )
		{
			if( m_bSwapState == false ) {
				if( m_pMainBar ) m_pMainBar->SwapQuickSlot();
				m_bSwapState = true;
			}
		}
		else if( !IsPushKey( IW( IW_TOGGLEMOUSE ) ) )
		{
			if( m_bSwapState == true ) {
				if( m_pMainBar ) m_pMainBar->SwapQuickSlot();
				m_bSwapState = false;
			}
		}
	}
}
#endif

bool CDnInterface::ProcessDLGame( LOCAL_TIME LocalTime, float fDelta )
{
	bool bResult = ProcessGame( LocalTime, fDelta );
	if( !bResult ) return false;
	return true;
}

bool CDnInterface::ProcessFarmGame( LOCAL_TIME LocalTime, float fDelta )
{
	bool bResult = ProcessGame( LocalTime, fDelta );
	if( !bResult ) return false;
	return true;
}

void CDnInterface::ProcessInput( LOCAL_TIME LocalTime, float fDelta )
{
	if(IsPVP() )
		ProcessPVPInput();

#ifdef PRE_ADD_AIM_ASSIST_BUTTON
	ProcessAimAssistFindTargetUI( LocalTime, fDelta );
	ProcessAimAssistMoveNearUI( LocalTime, fDelta );
#endif
}

void CDnInterface::ProcessPVPInput()
{
	if( GetChatDialog() && !GetChatDialog()->IsEditBoxFocus() && !focus::GetControl() )
	{
#ifdef PRE_ADD_PVP_TOURNAMENT
		if (IsPVP() && m_pPVPTournamentGMatchListDlg)
			return;
#endif

		if( IsPushKey( g_UIWrappingKeyData[IW_PVPTAB] ) )	//&& !(GetKeyState( VK_LMENU )&0x80) ) 탭이었을때의 알트 검사
		{
			if( IsPVP() && m_pPVPGameResultDlg )
			{
				bool bShowResultDlg = true;
				if(IsLadderMode() && (m_bEnableCountDown || CDnLocalPlayerActor::IsLockInput()))
					bShowResultDlg = false;
				m_pPVPGameResultDlg->Show(bShowResultDlg);
			}
			else if(IsPVP() && m_pPVPGameResultAllDlg)
			{
				m_pPVPGameResultAllDlg->Show(true);
			}
			else if(IsPVP() && m_pGuildWarResultDlg)
			{
				m_pGuildWarResultDlg->Show(true);
			}
#if defined( PRE_ADD_RACING_MODE )
			else if( IsPVP() && m_pPvPRacingResultDlg )
			{
				//m_pPvPRacingResultDlg->Show( true );
			}
#endif	// #if defined( PRE_ADD_RACING_MODE )
		}
		else if( !IsPushKey( g_UIWrappingKeyData[IW_PVPTAB] ) )
		{
			if( IsPVP() && m_pPVPGameResultDlg )
			{
				m_pPVPGameResultDlg->Show(false);
			}
			else if(IsPVP() && m_pPVPGameResultAllDlg )
			{
				m_pPVPGameResultAllDlg->Show(false);
			}
			else if(IsPVP() && m_pGuildWarResultDlg )
			{
				m_pGuildWarResultDlg->Show(false);
			}
#if defined( PRE_ADD_RACING_MODE )
			else if( IsPVP() && m_pPvPRacingResultDlg )
			{
				//m_pPvPRacingResultDlg->Show( false );
			}
#endif	// #if defined( PRE_ADD_RACING_MODE )
		}
		if( IsPushKey( g_UIWrappingKeyData[IW_UI_SITUATION] ) )
		{
			if( eGuildWar_Room == m_ePVPRoomState && m_pGuildWarSituation )
				m_pGuildWarSituation->SituationShow( true );
		}
		else if( !IsPushKey( g_UIWrappingKeyData[IW_UI_SITUATION] ) )
		{
			if( eGuildWar_Room == m_ePVPRoomState && m_pGuildWarSituation )
				m_pGuildWarSituation->SituationShow( false );
		}
	}
}

#ifdef PRE_ADD_AIM_ASSIST_BUTTON

void CDnInterface::ProcessAimAssistFindTargetUI( LOCAL_TIME LocalTime, float fDelta )
{
	if( m_pFadeDlg && (m_pFadeDlg->GetFadeMode() != CDnFadeInOutDlg::modeEnd) )
		return;

	if( !CDnMouseCursor::GetInstance().IsShowCursor() )
		return;

	RECT rcMouse;
	std::vector<CEtUIControl *> vecControl;
	std::vector<EtVector2> vecControlPos;
	UI_CONTROL_TYPE eType[] = { UI_CONTROL_BUTTON, UI_CONTROL_RADIOBUTTON, UI_CONTROL_CHECKBOX,
								UI_CONTROL_COMBOBOX, UI_CONTROL_LISTBOX, UI_CONTROL_LISTBOXEX,
								UI_CONTROL_TREECONTROL, UI_CONTROL_QUESTTREECONTROL,
								UI_CONTROL_TEXTBOX, UI_CONTROL_HTMLTEXTBOX };
	CEtUIDialog *pDialog = NULL;
	CEtUIControl *pControl = NULL;
	float fScreenWidth = 1.0f;
	float fScreenHeight = 1.0f;

	if( m_bAimKeyState == false )
	{
		if( IsPushKey( IW( IW_AIMASSIST ) ) )
		{
			if( m_pNpcDlg && m_pNpcDlg->IsShow() && (!m_pMessageBox->IsShow() && !m_pBigMessageBox->IsShow() && !m_pMiddleMessageBox) )
			{
				m_pNpcDlg->GetSmartMovePos( vecControlPos );
				fScreenWidth = m_pNpcDlg->GetScreenWidth();
				fScreenHeight = m_pNpcDlg->GetScreenHeight();
			}
			else
			{
				if( CEtUIDialogBase::StaticFindControl( vecControl, _countof(eType), eType, true ) && vecControl.size() )
				{
					for( int i = 0; i < (int)vecControl.size(); ++i )
					{
						pControl = vecControl[i];
						if( pControl ) pDialog = pControl->GetParent();
						if( !pDialog ) continue;

						fScreenWidth = pDialog->GetScreenWidth();
						fScreenHeight = pDialog->GetScreenHeight();
						pControl->FindInputPos( vecControlPos );
					}
				}
			}

			if( vecControlPos.size() )
			{
				float fMinDist = FLT_MAX;
				float fX = 0.0f;
				float fY = 0.0f;
				for( int i = 0; i < (int)vecControlPos.size(); ++i )
				{
					float fMovePosX = vecControlPos[i].x;
					float fMovePosY = vecControlPos[i].y;
					float fDist = (CEtUIDialog::s_fScreenMouseX-fMovePosX)*(CEtUIDialog::s_fScreenMouseX-fMovePosX) + (CEtUIDialog::s_fScreenMouseY-fMovePosY)*(CEtUIDialog::s_fScreenMouseY-fMovePosY);
					if( fDist < fMinDist )
					{
						fMinDist = fDist;
						fX = fMovePosX;
						fY = fMovePosY;
					}
				}
				m_nLockCursorPosX = (int)(fScreenWidth * fX);
				m_nLockCursorPosY = (int)(fScreenHeight * fY);
				CDnMouseCursor::GetInstance().SetCursorPos( m_nLockCursorPosX, m_nLockCursorPosY );

				POINT pt;
				pt.x = m_nLockCursorPosX;
				pt.y = m_nLockCursorPosY;
				ClientToScreen( CDnMainFrame::GetInstance().GetHWnd(), &pt );
				rcMouse.left = rcMouse.right = pt.x;
				rcMouse.top = rcMouse.bottom = pt.y;
				//CDnMouseCursor::GetInstance().ClipCursor( &rcMouse );
				ClipCursor( &rcMouse );
			}

			m_bAimKeyState = true;
		}
	}
	else if( m_bAimKeyState == true )
	{
		// DIK_SCROLL, DIK_LMENU, DIK_CAPSLOCK, DIK_NUMLOCK 부터 일반 DIK_A~Z까지 전부다 누르고 있다 떼야 !IsPushKey(키) 에서 컬리는데
		// DIK_PAUSE 키만 누르고 있는 상태에서도 !IsPushKey( DIK_PAUSE ) 에서 true가 된다.
		// 다이렉트 샘플 예제에서 테스트 해보니 계속 Pause키를 누르고 있어도 바로 입력이 풀리는 것처럼 된다.(다운 후 즉시 업도 같이 오는 듯)
		// AsyncKeyState도 그렇고 마찬가지로 안되길래 메세지 프로시저에서 확인해보니 누르자마다 KEY_DOWN오고 바로 KEY_UP이 온다.
		// 그래서 결국 Pause키를 사용하지 않기로 했다.
		if( !IsPushKey( IW( IW_AIMASSIST ) ) )
		{
			m_bAimKeyState = false;
			m_fAimMoveDelay = 0.0f;
			//CDnMouseCursor::GetInstance().ClipCursor( NULL );
			ClipCursor( NULL );
		}
	}
}

void CDnInterface::ProcessAimAssistMoveNearUI( LOCAL_TIME LocalTime, float fDelta )
{
	if( m_pFadeDlg && (m_pFadeDlg->GetFadeMode() != CDnFadeInOutDlg::modeEnd) )
		return;

	if( !CDnMouseCursor::GetInstance().IsShowCursor() )
		return;

	RECT rcMouse;
	std::vector<CEtUIControl *> vecControl;
	std::vector<EtVector2> vecControlPos;
	UI_CONTROL_TYPE eType[] = { UI_CONTROL_BUTTON, UI_CONTROL_RADIOBUTTON, UI_CONTROL_CHECKBOX,
								UI_CONTROL_COMBOBOX, UI_CONTROL_LISTBOX, UI_CONTROL_LISTBOXEX,
								UI_CONTROL_TREECONTROL, UI_CONTROL_QUESTTREECONTROL,
								UI_CONTROL_TEXTBOX, UI_CONTROL_HTMLTEXTBOX };
	CEtUIDialog *pDialog = NULL;
	CEtUIControl *pControl = NULL;
	float fScreenWidth = 1.0f;
	float fScreenHeight = 1.0f;

	const float fMove = 1.0f;
	const float fDelay = 0.4f;
	char szBuf[128] = {0,};
	if( m_bAimKeyState == true )
	{
		if( m_fAimMoveDelay > 0.0f )
		{
			m_fAimMoveDelay -= fDelta;
		}
		else
		{
			int nDirIndex = 0;
			bool bMouseMoved = false;
			D3DXVECTOR3 vecMouseMove = GetMouseVariation();
			if( abs(vecMouseMove.x) > abs(vecMouseMove.y) )
			{
				if( abs(vecMouseMove.x) > fMove )
				{
					bMouseMoved = true;
					if( vecMouseMove.x >= 0.0f ) nDirIndex = 0;
					else nDirIndex = 1;
				}
			}
			else
			{
				if( abs(vecMouseMove.y) > fMove )
				{
					bMouseMoved = true;
					if( vecMouseMove.y >= 0.0f ) nDirIndex = 2;
					else nDirIndex = 3;
				}
			}

			if( IsJoypad() )
			{
				if( IsPushJoypadButton( IW_PAD( IW_LOOKLEFT ) ) )
				{
					bMouseMoved = true;
					nDirIndex = 1;
				}
				else if( IsPushJoypadButton( IW_PAD( IW_LOOKRIGHT ) ) )
				{
					bMouseMoved = true;
					nDirIndex = 0;
				}
				else if( IsPushJoypadButton( IW_PAD( IW_LOOKUP ) ) )
				{
					bMouseMoved = true;
					nDirIndex = 3;
				}
				else if( IsPushJoypadButton( IW_PAD( IW_LOOKDOWN ) ) )
				{
					bMouseMoved = true;
					nDirIndex = 2;
				}
			}

			if( bMouseMoved )
			{
				m_fAimMoveDelay = fDelay;

				EtVector2 vAxisDir[4] = {
					EtVector2(1.0f, 0.0f),
					EtVector2(-1.0f, 0.0f),
					EtVector2(0.0f, 1.0f),
					EtVector2(0.0f, -1.0f),
				};

				bool bNpcDlg = false;
				if( m_pNpcDlg && m_pNpcDlg->IsShow() )
				{
					m_pNpcDlg->GetSmartMovePos( vecControlPos );
					fScreenWidth = m_pNpcDlg->GetScreenWidth();
					fScreenHeight = m_pNpcDlg->GetScreenHeight();
					bNpcDlg = true;
				}
				else
				{
					if( CEtUIDialogBase::StaticFindControl( vecControl, _countof(eType), eType, true ) && vecControl.size() )
					{
						for( int i = 0; i < (int)vecControl.size(); ++i )
						{
							pControl = vecControl[i];
							if( pControl ) pDialog = pControl->GetParent();
							if( !pDialog ) continue;

							fScreenWidth = pDialog->GetScreenWidth();
							fScreenHeight = pDialog->GetScreenHeight();
							pControl->FindInputPos( vecControlPos );
						}
					}
				}

				if( vecControlPos.size() )
				{
					float fMinDist = FLT_MAX;
					float fX = 0.0f;
					float fY = 0.0f;
					for( int j = 0; j < 2; ++j )	// npcDlg일때는 90도로 먼저 검사 후 만약 나오지 않았을때 180도로 검사해야해서 두번 돈다.
					{
						if( !bNpcDlg && j == 1 ) continue;
						if( fMinDist != FLT_MAX ) continue;

						for( int i = 0; i < (int)vecControlPos.size(); ++i )
						{
							float fMovePosX = vecControlPos[i].x;
							float fMovePosY = vecControlPos[i].y;

							EtVector2 vDir = EtVector2(fMovePosX-CEtUIDialog::s_fScreenMouseX, fMovePosY-CEtUIDialog::s_fScreenMouseY);
							EtVec2Normalize( &vDir, &vDir );
							float fDir = acos( EtVec2Dot( &vAxisDir[nDirIndex], &vDir ) );
							if( fDir > (ET_PI / (j == 0 ? 4.0f : 2.05f)) )
								continue;

							if( (int)(fScreenWidth * fMovePosX) == m_nLockCursorPosX && (int)(fScreenHeight * fMovePosY) == m_nLockCursorPosY )
								continue;

							float fDist = (CEtUIDialog::s_fScreenMouseX-fMovePosX)*(CEtUIDialog::s_fScreenMouseX-fMovePosX) + (CEtUIDialog::s_fScreenMouseY-fMovePosY)*(CEtUIDialog::s_fScreenMouseY-fMovePosY);
							if( fDist < fMinDist )
							{
								fMinDist = fDist;
								fX = fMovePosX;
								fY = fMovePosY;
							}
						}
					}

					if( fMinDist != FLT_MAX )
					{
						m_nLockCursorPosX = (int)(fScreenWidth * fX);
						m_nLockCursorPosY = (int)(fScreenHeight * fY);

						POINT pt;
						pt.x = m_nLockCursorPosX;
						pt.y = m_nLockCursorPosY;
						ClientToScreen( CDnMainFrame::GetInstance().GetHWnd(), &pt );
						rcMouse.left = rcMouse.right = pt.x;
						rcMouse.top = rcMouse.bottom = pt.y;
						//CDnMouseCursor::GetInstance().ClipCursor( &rcMouse );
						ClipCursor( &rcMouse );

						CDnMouseCursor::GetInstance().SetCursorPos( m_nLockCursorPosX, m_nLockCursorPosY );
					}
				}
			}
		}
	}
}
#endif

void CDnInterface::OnInputReceive( int nReceiverState, LOCAL_TIME LocalTime )
{
	if( nReceiverState & IR_JOYPAD_DOWN )
	{
		if( m_pFadeDlg && (m_pFadeDlg->GetFadeMode() != CDnFadeInOutDlg::modeEnd) )
			return;

		// IsPushKey로 검사할 경우
		// 차일드 모달 띄워둔 상태에서 Ctrl+특수키로 IME 전환할때 커서비활성모드로 바뀐 후
		// 해당 차일드 모달 창을 닫지 않는이상 커서모드로 바꿀 방법이 없게된다.
		// 그래서 이렇게 GetEventKey로 처리해야한다.
		// 참고로, IsPushKey, ReleaseKey로는 해결이 안된다.
		if( IsPushJoypadButton( IW_PAD(IW_TOGGLEMOUSE) ) )
		{
			if( GetInterfaceType() != Title && GetInterfaceType() != Login && GetInterfaceType() != PVPVillage)
			{
				if (CDnLocalPlayerActor::IsLockInput() == false && m_pCashShopDlg->IsShow() == false)
				{
					bool bShowCursor = CDnMouseCursor::GetInstance().IsShowCursor();
					CDnMouseCursor::GetInstance().ShowCursor( !bShowCursor, true );

					drag::Command(UI_DRAG_CMD_CANCEL);
					drag::ReleaseControl();

					focus::ReleaseControl();

					ReleaseJoypadButton( IW_PAD(IW_TOGGLEMOUSE) );
				}
			}
		}

		if( IsPushJoypadButton( IW_UI(IW_SYSTEM) ) && !(GetKeyState( VK_LCONTROL )&0x80) )	// 컨트롤+Esc로 안열리게 해달라고 한다.
		{
			bool bApplyEscapeUI = true;
			if( CTaskManager::GetInstance().GetTask( "CutSceneTask" ) ) bApplyEscapeUI = false;

			if( bApplyEscapeUI ) {
				if( EtInterface::drag::IsValid() )
				{
					EtInterface::drag::ReleaseControl();
				}
				else
				{
					if( GetChatDialog() && GetChatDialog()->IsShow() )
					{
					}
					else if( m_pMainMenuDlg )
					{
						if( !m_pMainMenuDlg->IsOpenDialog() && (!m_pBlindDlg || m_pBlindDlg->GetBlindMode() == CDnBlindDlg::modeClosed) )
						{
							m_pMainMenuDlg->ShowSystemDialog( true );
						}

						if(IsPVPLobby())
						{
							if( m_pPVPRoomListDlg->IsShow() )
								m_pPVPRoomListDlg->OpenExitDlg();							

						}

					}
					else if( IsOpenBlind() )
					{
						CDnLocalPlayerActor::LockInput(false);
						CDnCommonTask *pCommonTask = (CDnCommonTask *)CTaskManager::GetInstance().GetTask( "CommonTask" );
						if( pCommonTask ) pCommonTask->EndNpcTalk();
					}
					else
					{
#if defined(_CH) && defined(_AUTH)
						if( m_Type == InterfaceTypeEnum::Login )
						{
							if( m_pLoginDlg && m_pLoginDlg->IsShow() )
								CDnSDOAService::GetInstance().Show( false );
						}
#endif	// #if defined(_CH) && defined(_AUTH)

// #69613 - 메세지변경 처리 제거.
//#ifdef PRE_ADD_NEWCOMEBACK
//						CDnBridgeTask *pTask = (CDnBridgeTask *)CTaskManager::GetInstance().GetTask( "BridgeTask" );
//						if( pTask->GetComeback() )
//							MessageBox( 4948, MB_YESNO, MESSAGEBOX_21, CDnInterface::GetInstancePtr() ); // "게임에 접속을 하지 않고 종료하면, 재접속 시에는 귀환자 보상을 받을 수 없습니다. 종료하시겠습니까?"
//						else
//							MessageBox( MESSAGEBOX_21, MB_YESNO, MESSAGEBOX_21, this );
//#else

#ifdef PRE_ADD_GAMEQUIT_REWARD
						CTask * _pTask = CTaskManager::GetInstance().GetTask( "CommonTask" );
						if( _pTask )
							static_cast<CDnCommonTask *>(_pTask)->SendLogOutMessage();
#else
						MessageBox( MESSAGEBOX_21, MB_YESNO, MESSAGEBOX_21, this );
#endif // PRE_ADD_GAMEQUIT_REWARD
//#endif // PRE_ADD_NEWCOMEBACK	


					}
				}
				ReleaseJoypadButton( IW_UI(IW_SYSTEM) );
			}
		}

		if( IsPushJoypadButton( IW_UI(IW_UI_HELP) ) && !(GetKeyState( VK_LMENU )&0x80) )
		{
			// 타이틀도 아니고, 로그인도 아니고, pvp로비도 아니고,
			if( GetInterfaceType() != Title && GetInterfaceType() != Login && GetInterfaceType() != PVPVillage )
			{
				if( GetChatDialog() && !GetChatDialog()->IsEditBoxFocus() && IsShowCashShopDlg() == false )
				{
					if( m_pHelpKeyboardDlg && 
						!CTaskManager::GetInstance().GetTask( "CutSceneTask" ) && 
						!IsOpenBlind() && 
						!m_pGateQuestionDlg->IsShow() 
						&& !m_pWorldZoneSelectDlg->IsShow()
						)
					{
						m_pHelpKeyboardDlg->SetJoypadPush( true );
						m_pHelpKeyboardDlg->Show( true );
					}
				}
			}
			ReleaseJoypadButton( IW_UI(IW_UI_HELP) );
			return;
		}

		if( !EtInterface::g_bEtUIRender )
		{
			// 타이틀도 아니고, 로그인도 아니고, pvp로비도 아니고,
			if( GetInterfaceType() != Title && GetInterfaceType() != Login && GetInterfaceType() != PVPVillage )
			{
				// UI렌더링이 안될때도 퀵슬롯 관련은 작동해야한다고 한다.
				if( !m_pMainBar ) return;
				if( IsPushJoypadButton( IW_UI( IW_UI_QUICKSLOT1 ) ) || IsPushJoypadButton( IW_UI( IW_UI_QUICKSLOT2 ) ) ||
					IsPushJoypadButton( IW_UI( IW_UI_QUICKSLOT3 ) ) || IsPushJoypadButton( IW_UI( IW_UI_QUICKSLOT4 ) ) ||
					IsPushJoypadButton( IW_UI( IW_UI_QUICKSLOT5 ) ) || IsPushJoypadButton( IW_UI( IW_UI_QUICKSLOT6 ) ) ||
					IsPushJoypadButton( IW_UI( IW_UI_QUICKSLOT7 ) ) || IsPushJoypadButton( IW_UI( IW_UI_QUICKSLOT8 ) ) ||
					IsPushJoypadButton( IW_UI( IW_UI_QUICKSLOT9 ) ) || IsPushJoypadButton( IW_UI( IW_UI_QUICKSLOT10 ) ) ||
					IsPushJoypadButton( IW_UI( IW_UI_QUICKSLOTCHANGE ) ) )
					m_bJoypadPushed = true;
			}
			return;
		}
	}

	if( nReceiverState & IR_KEY_DOWN  ) 
	{
		if( m_pFadeDlg && (m_pFadeDlg->GetFadeMode() != CDnFadeInOutDlg::modeEnd) )
			return;

		// IsPushKey로 검사할 경우
		// 차일드 모달 띄워둔 상태에서 Ctrl+특수키로 IME 전환할때 커서비활성모드로 바뀐 후
		// 해당 차일드 모달 창을 닫지 않는이상 커서모드로 바꿀 방법이 없게된다.
		// 그래서 이렇게 GetEventKey로 처리해야한다.
		// 참고로, IsPushKey, ReleaseKey로는 해결이 안된다.
		bool bToggleMouse = false;
		for( DWORD i=0; i<GetEventKeyCount(); i++ ) {
			BYTE cKey = GetEventKey(i);
			if( cKey == IW( IW_TOGGLEMOUSE ) ) bToggleMouse = true;
		}
		if( bToggleMouse )
		{
			if( GetInterfaceType() != Title && GetInterfaceType() != Login && GetInterfaceType() != PVPVillage)
			{
				if (CDnLocalPlayerActor::IsLockInput() == false && m_pCashShopDlg->IsShow() == false)
				{
					bool bShowCursor = CDnMouseCursor::GetInstance().IsShowCursor();
					CDnMouseCursor::GetInstance().ShowCursor( !bShowCursor, true );

					drag::Command(UI_DRAG_CMD_CANCEL);
					drag::ReleaseControl();

					focus::ReleaseControl();

					/*
					if( bShowCursor == false )
					{
					CDnLocalPlayerActor *pPlayer = dynamic_cast<CDnLocalPlayerActor*>((CDnActor*)CDnActor::s_hLocalActor);
					if( pPlayer && pPlayer->IsMove() && pPlayer->IsMovable() ) 
					{
					pPlayer->CmdStop( "Stand" );
					}
					}
					*/
				}
			}
		}

#ifdef PRE_ADD_OVERLAP_SETEFFECT
		if(m_pTooltipDlg)
		{
			if( m_pTooltipDlg->IsShow() && IsPushKey(DIK_LALT) ) {
				m_pTooltipDlg->PressAlt(true);
			}
		}
#endif

#ifdef PRE_ADD_DRAGON_GEM
	#ifndef _FINALBUILD
		if( IsPushKey(DIK_A) )
		{
			// Test용으로 만든 치트키
			if(m_pTooltipDlg && m_pTooltipDlg->IsShow())
			{
				//m_pTooltipDlg->ShowDragonGemToolTip();
			}
		}
	#endif
#endif // PRE_ADD_DRAGON_GEM

		if( IsPushKey( DIK_LCONTROL ) && IsPushKey( DIK_SYSRQ ) )
		{
			// 타이틀도 아니고, 로그인도 아니고, pvp로비도 아니고,
			if( GetInterfaceType() != Title && GetInterfaceType() != Login && GetInterfaceType() != PVPVillage )
			{
				// 컷신플레이중도 아닐때만 UI숨기기 가능.
				if( !CTaskManager::GetInstance().GetTask( "CutSceneTask" ) )
				{
					m_bEnableRender = !m_bEnableRender;
					EtInterface::g_bEtUIRender = m_bEnableRender;
					CEtCustomRender::s_bEnableCustomRender = m_bEnableRender;
					ReleasePushKey( DIK_SYSRQ );

					CDnMouseCursor::GetInstance().ShowCursor( m_bEnableRender, true );
				}
			}
		}

		// 라디오메세지의 경우 UI렌더링이 안되도 상관없이 동작해야한다.
		bool bUsableRadioMsg = true;
#if defined(_JP)
		if( GetChatDialog() && GetChatDialog()->IsEditBoxFocus() )
			bUsableRadioMsg = false;
#endif

#ifdef PRE_ADD_PVP_COMBOEXERCISE
		// PvPGameMode - 콤보연습모드의 경우에는 "F1" 키가 몬스터 소환으로 사용된다.
		if( IsPushKey( DIK_F1 ) )
		{			
			CTask * pTask = CTaskManager::GetInstance().GetTask("GameTask");
			if( pTask )
			{
				CDnPvPGameTask * pPvPGameTask = dynamic_cast< CDnPvPGameTask * >( pTask );
				if( pPvPGameTask && (pPvPGameTask->GetGameMode() == PvPCommon::GameMode::PvP_ComboExercise) && pPvPGameTask->IsPvPComboModeMaster() )
				{
					bUsableRadioMsg = false;				

					if( m_TimeSummonMonster == 0 || ( pPvPGameTask->GetLocalTime() > m_TimeSummonMonster + 1000 ) )
					{
						m_TimeSummonMonster = pPvPGameTask->GetLocalTime();
						pPvPGameTask->SummonDummyMonster();
					}
				}
			}
		}		
#endif // PRE_ADD_PVP_COMBOEXERCISE

		if( CDnRadioMsgTask::IsActive() && !IsOpenBlind() && bUsableRadioMsg )
		{
			// 키셋팅 개선 들어가기 전까진 간단하게 처리한다.
			int nIndex = 0;
			if( IsPushKey( DIK_F1 ) ) nIndex = 1;
			else if( IsPushKey( DIK_F2 ) ) nIndex = 2;
			else if( IsPushKey( DIK_F3 ) ) nIndex = 3;
			else if( IsPushKey( DIK_F4 ) ) nIndex = 4;
			else if( IsPushKey( DIK_F5 ) ) nIndex = 5;
			else if( IsPushKey( DIK_F6 ) ) nIndex = 6;
			else if( IsPushKey( DIK_F7 ) ) nIndex = 7;
			else if( IsPushKey( DIK_F8 ) ) nIndex = 8;
			else if( IsPushKey( DIK_F9 ) ) nIndex = 9;
			else if( IsPushKey( DIK_F10 ) ) nIndex = 10;

			bool bIsUseGuildWarSkill = false;
			CDnMainDlg *pMainBarDlg = GetInterface().GetMainBarDialog();
			if( pMainBarDlg && pMainBarDlg->IsShowGuildWarSkillSlotDlg() && nIndex < 9 )
				bIsUseGuildWarSkill = true;

			if( !bIsUseGuildWarSkill )
			{
				// 라디오메세지
				int nRadioMsgID = nIndex;
				if( nRadioMsgID )
					GetRadioMsgTask().UseRadioMsg( nRadioMsgID );
			}
		}

		if( !EtInterface::g_bEtUIRender )
		{
			// 타이틀도 아니고, 로그인도 아니고, pvp로비도 아니고,
			if( GetInterfaceType() != Title && GetInterfaceType() != Login && GetInterfaceType() != PVPVillage )
			{
				// UI렌더링이 안될때도 퀵슬롯 관련은 작동해야한다고 한다.
				if( !m_pMainBar ) return;
				if( IsPushKey( g_UIWrappingKeyData[IW_UI_QUICKSLOT1] ) || IsPushKey( g_UIWrappingKeyData[IW_UI_QUICKSLOT2] ) ||
					IsPushKey( g_UIWrappingKeyData[IW_UI_QUICKSLOT3] ) || IsPushKey( g_UIWrappingKeyData[IW_UI_QUICKSLOT4] ) ||
					IsPushKey( g_UIWrappingKeyData[IW_UI_QUICKSLOT5] ) || IsPushKey( g_UIWrappingKeyData[IW_UI_QUICKSLOT6] ) ||
					IsPushKey( g_UIWrappingKeyData[IW_UI_QUICKSLOT7] ) || IsPushKey( g_UIWrappingKeyData[IW_UI_QUICKSLOT8] ) ||
					IsPushKey( g_UIWrappingKeyData[IW_UI_QUICKSLOT9] ) || IsPushKey( g_UIWrappingKeyData[IW_UI_QUICKSLOT10] ) ||
					IsPushKey( g_UIWrappingKeyData[IW_UI_QUICKSLOTCHANGE] ) )
					m_pMainBar->MsgProc( m_pMainBar->GetHWnd(), WM_KEYDOWN, 0, 0 );
			}
			return;
		}

		if( IsPushKey( DIK_RETURN ) || IsPushKey( DIK_NUMPADENTER ) )
		{
			bool bChatRoom = false;
			if( m_pChatRoomDlg && m_pChatRoomDlg->IsShow() ) bChatRoom = true;
			if( GetChatDialog() && !bChatRoom )
			{
				GetChatDialog()->ShowEx(true);
			}
		}

		if( IsPushKey( DIK_ESCAPE ) && !(GetKeyState( VK_LCONTROL )&0x80) )	// 컨트롤+Esc로 안열리게 해달라고 한다.
		{
			if(CDnActor::s_hLocalActor)
			{
				CDnLocalPlayerActor *pPlayer = dynamic_cast<CDnLocalPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());

				if(pPlayer && pPlayer->IsCannonMode() && !IsOpenBlind()) 
				{
					pPlayer->ExitCannonMode();
					return;
				}
			}

			bool bApplyEscapeUI = true;
			if( CTaskManager::GetInstance().GetTask( "CutSceneTask" ) ) bApplyEscapeUI = false;

			if( bApplyEscapeUI ) {
				if( EtInterface::drag::IsValid() )
				{
					EtInterface::drag::ReleaseControl();
				}
				else
				{
					if( GetChatDialog() && GetChatDialog()->IsShow() )
					{
					}
					else if( m_pMainMenuDlg )
					{
						if( !m_pMainMenuDlg->IsOpenDialog() && (!m_pBlindDlg || m_pBlindDlg->GetBlindMode() == CDnBlindDlg::modeClosed) )
						{
/* //rlkt_escape_fix
#ifdef PRE_ADD_MAINQUEST_UI
							CDnCommonTask* pCommonTask = static_cast<CDnCommonTask*>( CTaskManager::GetInstance().GetTask( "CommonTask" ) );
							if(pCommonTask)
							{
								DnCameraHandle		hNpcCam		= pCommonTask->GetNpcTalkCamera();
								CDnNpcTalkCamera*	pNpcTalkCam = static_cast<CDnNpcTalkCamera*>(hNpcCam.GetPointer());
								if(pNpcTalkCam)
								{
									if(pCommonTask->IsRequestNpcTalk)
									if( pNpcTalkCam->GetWorkState() == CDnNpcTalkCamera::TalkNone ||
										pNpcTalkCam->GetWorkState() == CDnNpcTalkCamera::RestoreCamera )
									{
										m_pMainMenuDlg->ShowSystemDialog( true );
									}
								}
								else
								{
									m_pMainMenuDlg->ShowSystemDialog( true );
								}
							}							
#else*/
							m_pMainMenuDlg->ShowSystemDialog( true );
//#endif // PRE_ADD_MAINQUEST_UI
						}

						if(IsPVPLobby())
						{
							if( m_pPVPRoomListDlg->IsShow() )
								m_pPVPRoomListDlg->OpenExitDlg();							

						}

					}
					else if( IsOpenBlind() )
					{
						CDnLocalPlayerActor::LockInput(false);
						CDnCommonTask *pCommonTask = (CDnCommonTask *)CTaskManager::GetInstance().GetTask( "CommonTask" );
						if( pCommonTask ) pCommonTask->EndNpcTalk();
					}
					else
					{
#if defined(_CH) && defined(_AUTH)
						if( m_Type == InterfaceTypeEnum::Login )
						{
							if( m_pLoginDlg && m_pLoginDlg->IsShow() )
								CDnSDOAService::GetInstance().Show( false );
						}
#endif	// #if defined(_CH) && defined(_AUTH)

// #69613 - 메세지변경 처리 제거.
//#ifdef PRE_ADD_NEWCOMEBACK
//						CDnBridgeTask *pTask = (CDnBridgeTask *)CTaskManager::GetInstance().GetTask( "BridgeTask" );
//						if( pTask->GetComeback() )
//							MessageBox( 4948, MB_YESNO, MESSAGEBOX_21, CDnInterface::GetInstancePtr() ); // "게임에 접속을 하지 않고 종료하면, 재접속 시에는 귀환자 보상을 받을 수 없습니다. 종료하시겠습니까?"
//						else
//							MessageBox( MESSAGEBOX_21, MB_YESNO, MESSAGEBOX_21, this );
//#else

#ifdef PRE_ADD_GAMEQUIT_REWARD
						CTask * _pTask = CTaskManager::GetInstance().GetTask( "CommonTask" );
						if( _pTask )
							static_cast<CDnCommonTask *>(_pTask)->SendLogOutMessage();
#else
						MessageBox( MESSAGEBOX_21, MB_YESNO, MESSAGEBOX_21, this );
#endif // PRE_ADD_GAMEQUIT_REWARD
//#endif // PRE_ADD_NEWCOMEBACK					

					}
				}
				ReleasePushKey( DIK_ESCAPE );
			}
		}

		//if( IsPushKey( DIK_LSHIFT ) && IsPushKey( DIK_R ) )
		if( IsPushKey( g_UIWrappingKeyData[IW_CHATREPLY] ) )
		{
			if( GetChatDialog() && !GetChatDialog()->IsEditBoxFocus() )
			{
				std::wstring strLastName;
				if( GetChatDialog()->GetLastPrivateName( strLastName ) )
				{
					GetChatDialog()->ShowEx(true);
					GetChatDialog()->SetPrivateName( strLastName );
					//ReleasePushKey( DIK_R );
					ReleasePushKey( g_UIWrappingKeyData[IW_CHATREPLY] );

					// 한글입력모드에서 Shift+R 누를 경우 WM_IME_STARTCOMPOSITION 없이 ㄲ가 입력되게 된다.
					// 캔슬처리로 취소.
					// T로 단축키가 바뀌면서 필요없게 되었으나 우선은 그냥 두겠다.
					if( GetChatDialog()->GetIMEEditBox() )
						GetChatDialog()->GetIMEEditBox()->CancelIMEComposition();
				}
			}
		}

#ifdef _TEST_CODE_KAL
 		if( IsPushKey( DIK_F6 ) )
 		{
//			m_pMainMenuDlg->ToggleShowDialog(CDnMainMenuDlg::CASHSHOP_DIALOG);
// 			SetCpJudge(CpType_SuperArmorAttack, 1234);
// 			SetCpJudge(CpType_BrokenShield, 1234);
// 			SetCpJudge(CpType_Genocide, 1234);
// 			OpenMailDialog(false, _T(""), _T(""));
//			OpenProgressDialog(_T("TEST"), 10.f, 10.f, true, true);
// 			BeginCountDown(10.f, _T("TEST"), eProgress_Always);
// 			m_bEnableCountDown = true;
		}
#else
		//if( IsPushKey( DIK_F9 ) )
		//{
		//	if( m_pBlindDlg && m_pBlindDlg->GetBlindMode() == CDnBlindDlg::modeClosed )
		//	{
		//		//AddSlideCaptionMessage( L"드드드", 0xFFFFFFFF, 50.0f );
		//		//AddSlideCaptionMessage( L"라라라라라라라라라라", 0xFFFF0000, 60.0f );
		//		//AddSlideCaptionMessage( L"124315415", 0xFFFF0000, 30.0f );
		//	}
		//}

#ifndef _FINAL_BUILD
		//if( IsPushKey( DIK_F10 ) )
		//{
		//	if( m_pBlindDlg && m_pBlindDlg->GetBlindMode() == CDnBlindDlg::modeClosed )
		//	{
		//		//DelSlideCaptionMessage( L"드드드" );
		//		//CloseSlideCaptionMessage();
		//		//ShowGuildMgrBox( 0, true );
		//	}
		//}

		//if( IsPushKey( DIK_F11 ) )
		//{
		//	if( m_pBlindDlg && m_pBlindDlg->GetBlindMode() == CDnBlindDlg::modeClosed )
		//	{
		//		//ShowGuildMgrBox( 1, true );
		//	}
		//}
#endif

#endif // _TEST_CODE_KAL

		//if( IsOpenModalDialog() )
		// OnInputReceive까지 온 거 자체가 모달 다이얼로그가 하나도 열려있지 않은 상태다.
		if( IsPushKey( g_UIWrappingKeyData[IW_UI_HELP] ) && !(GetKeyState( VK_LMENU )&0x80) )
		{
			// 타이틀도 아니고, 로그인도 아니고, pvp로비도 아니고,
			if( GetInterfaceType() != Title && GetInterfaceType() != Login && GetInterfaceType() != PVPVillage )
			{
				if( GetChatDialog() && !GetChatDialog()->IsEditBoxFocus() && IsShowCashShopDlg() == false)
				{
					if( m_pHelpKeyboardDlg && !CTaskManager::GetInstance().GetTask( "CutSceneTask" ) && !IsOpenBlind() )
					{
						m_pHelpKeyboardDlg->Show( true );
					}
				}
			}
			ReleasePushKey( g_UIWrappingKeyData[IW_UI_HELP] );
			return;
		}

#ifdef _DEBUG
		if( IsPushKey( DIK_F3 ) )
		{
		}

		if( IsPushKey( DIK_F4 ) )
		{
		}

		if( IsPushKey( DIK_LCONTROL ) && IsPushKey( DIK_R ) )
		{
			// Note : UI 다시 로딩.
			//
			ReInitialize();
		}
#endif
	}

	if (nReceiverState & IR_KEY_UP)
	{
#ifdef PRE_ADD_PVP_TOURNAMENT
		if (IsPVP() && m_pPVPTournamentGMatchListDlg)
		{
			if (GetChatDialog() && !GetChatDialog()->IsEditBoxFocus())
			{
				for (DWORD i=0; i<GetEventKeyCount(); i++)
				{
					BYTE cKey = GetEventKey(i);
					if (cKey == g_UIWrappingKeyData[IW_PVPTAB])
					{
						m_pPVPTournamentGMatchListDlg->Show(!m_pPVPTournamentGMatchListDlg->IsShow());
						break;
					}
				}
			}
		}
#endif

#ifdef PRE_ADD_OVERLAP_SETEFFECT
		if(m_pTooltipDlg)
		{
			if( m_pTooltipDlg->IsShow() && IsPushKey(DIK_LALT) == false) {
				m_pTooltipDlg->PressAlt(false);
			}
		}
#endif

#ifdef PRE_ADD_DRAGON_GEM
	#ifndef _FINALBUILD
		if( IsPushKey(DIK_A) )
		{
			// Test용으로 만든 치트키
			if(m_pTooltipDlg && m_pTooltipDlg->IsShow())
			{
				//m_pTooltipDlg->CloseDragonGemToolTip();
			}
		}
	#endif
#endif // PRE_ADD_DRAGON_GEM
	}

}

void CDnInterface::RefreshPartyList()
{
	if( !m_pMainMenuDlg ) return;
	m_pMainMenuDlg->RefreshPartyList();
}

void CDnInterface::RefreshPartyMemberList()
{
	if( !m_pMainMenuDlg ) return;
	m_pMainMenuDlg->RefreshPartyMemberList();
}

void CDnInterface::RefreshPartyInfoList()
{
	if( !m_pMainMenuDlg ) return;
	m_pMainMenuDlg->RefreshPartyInfoList();
}

void CDnInterface::ResetPartyListSort()
{
	CDnCommunityDlg *pCommunityDlg = (CDnCommunityDlg*)GetMainMenuDialog(CDnMainMenuDlg::COMMUNITY_DIALOG);
	if (pCommunityDlg)
	{
		CDnPartyListDlg* pPartyListDlg = pCommunityDlg->GetPartyListDialog();
		if (pPartyListDlg)
			pPartyListDlg->ResetSort(true);
	}
}

#ifdef PRE_PARTY_DB
const WCHAR* CDnInterface::GetPartyListSearchWord()
{
	CDnCommunityDlg *pCommunityDlg = (CDnCommunityDlg*)GetMainMenuDialog(CDnMainMenuDlg::COMMUNITY_DIALOG);
	if (pCommunityDlg)
	{
		CDnPartyListDlg* pPartyListDlg = pCommunityDlg->GetPartyListDialog();
		if (pPartyListDlg)
			return pPartyListDlg->GetCurrentSearchWord();
	}

	return NULL;
}
#endif

void CDnInterface::RefreshPartyGateInfo()
{
	//m_pMainMenuDlg->RefreshPartyInfoList();
	TCHAR szName[128]={0};

	//blondy
	if( IsPVP() || IsFarm() )
	{
		m_pPlayerGaugeDlg->ShowLoading( false );
		m_pPlayerGaugeDlg->ShowGateReady( false );
		return;
	}
	//blondy end

	for( DWORD i=0; i<CDnPartyTask::GetInstance().GetPartyCount(); i++ ) 
	{
		CDnPartyTask::PartyStruct *pInfo = CDnPartyTask::GetInstance().GetPartyData(i);

		//DWORD dwTextColor = -1;
		bool bShowGateReady(false);
		bool bShowLoading(false);

		// 운영자난입은 파티 UI 표시 안함
		if( pInfo->bGMTrace )
			continue;

		if( pInfo->nEnteredGateIndex == -1 ) 
		{
			//wsprintf( szName, pInfo->wszCharacterName );
		}
		else 
		{
			//wsprintf( szName, L"Ready Gate - %d", pInfo->nEnteredGateIndex );
			//dwTextColor = textcolor::GREENYELLOW;

			// Note : Ready 이미지 보여줌
			//
			bShowGateReady = true;
		}
		if( CDnWorld::GetInstance().GetMapType() == CDnWorld::MapTypeDungeon ||
			CDnWorld::GetInstance().GetMapType() == CDnWorld::MapTypeWorldMap ) {
			if( pInfo->bSync == false ) {
				bShowLoading = true;
			}
		}

		if( ( CDnActor::s_hLocalActor && __wcsicmp_l( pInfo->wszCharacterName, CDnActor::s_hLocalActor->GetName() ) == NULL ) ||
			CDnPartyTask::GetInstance().GetPartyRole() == CDnPartyTask::SINGLE )
		{

			if( m_pPlayerGaugeDlg ) {
				m_pPlayerGaugeDlg->ShowGateReady( bShowGateReady );
				m_pPlayerGaugeDlg->ShowLoading( bShowLoading );
			}

			if (CDnPartyTask::GetInstance().IsPartyType(_RAID_PARTY_8) == false)
				continue;
		}

		if (pInfo->cMemberIndex >= (int)m_pVecPartyPlayerGaugeList.size())
		{
			_ASSERT("CDnInterface::RefreshPartyGateInfo() ::nPos >= (int)m_pVecPartyPlayerGaugeList.size() ");
			continue;
		}

		for( int i=0; i<(int)m_pVecPartyPlayerGaugeList.size(); i++ )
		{
			if (m_pVecPartyPlayerGaugeList[i].pGaugeDlg)
			{
				CDnGaugeDlg* pDlg = m_pVecPartyPlayerGaugeList[i].pGaugeDlg;
				const std::wstring& name = pDlg->GetName();
				if (name.compare(pInfo->wszCharacterName) == 0)
				{
					pDlg->ShowGateReady( bShowGateReady );
					pDlg->ShowLoading( bShowLoading );
				}
			}
		}
#if 0
		m_pVecPartyPlayerGaugeList[pInfo->cMemberIndex].pGaugeDlg->ShowGateReady( bShowGateReady );
		m_pVecPartyPlayerGaugeList[pInfo->cMemberIndex].pGaugeDlg->ShowLoading( bShowLoading );
#endif
	}
}

void CDnInterface::RefreshPartyGuage()
{
	if( IsPVP() || IsFarm() ) 
		return;

	for( int i=0; i<(int)m_pVecPartyPlayerGaugeList.size(); i++ )
	{
		m_pVecPartyPlayerGaugeList[i].pGaugeDlg->Show(false);
		m_pVecPartyPlayerGaugeList[i].pGaugeDlg->ShowGateReady( false );
		m_pVecPartyPlayerGaugeList[i].pGaugeDlg->ShowLoading( false );
		m_pVecPartyPlayerGaugeList[i].pGaugeDlg->SetEmpty();
	}

	if (m_pPlayerGaugeDlg) {
		m_pPlayerGaugeDlg->SetPartyMaster(false);
		m_pPlayerGaugeDlg->ShowPartyQuitButton(false);
	}

	if( !CDnPartyTask::IsActive() )
		return;

	if (m_pPlayerGaugeDlg)
	{
		const wchar_t* pLootRuleText = CDnPartyTask::GetInstance().GetPartyLootRuleString(CDnPartyTask::GetInstance().GetPartyLootRule());
		m_pPlayerGaugeDlg->ShowPartyLootInfo(pLootRuleText != NULL, pLootRuleText);

		const wchar_t* pLootRankText = CDnPartyTask::GetInstance().GetPartyLootItemRankString(CDnPartyTask::GetInstance().GetPartyLootItemRank());
		std::wstring jobDiceAndLootRankText;
		if (pLootRankText != NULL)
			jobDiceAndLootRankText = FormatW(L"%s, %s", CDnPartyTask::GetInstance().IsJobDice() ? GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3410 ) : GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3411 ), pLootRankText);
		m_pPlayerGaugeDlg->ShowPartyItemLootRank(CDnPartyTask::GetInstance().GetPartyIndex() != -1, jobDiceAndLootRankText.c_str());
	}

	if( CDnPartyTask::GetInstance().GetPartyRole() == CDnPartyTask::SINGLE ) 
		return;

	int nPartyPalyerGaugeIndex(0);
#ifdef PRE_FIX_CLIENT_MEMOPTIMIZE
	std::vector<int> nVecUsingGaugeList;
#endif

	for( DWORD i=0; i<GetPartyTask().GetPartyCount(); i++ )
	{
		if( (int)m_pVecPartyPlayerGaugeList.size() <= nPartyPalyerGaugeIndex )
			break;

		CDnPartyTask::PartyStruct *pInfo = GetPartyTask().GetPartyData(i);
		if( !pInfo ) continue;

		// 운영자난입은 파티 UI 표시 안함
		if( pInfo->bGMTrace )
			continue;

		if( pInfo->nSessionID == CDnBridgeTask::GetInstance().GetSessionID() ) 
		{
			if (m_pPlayerGaugeDlg)
			{
				m_pPlayerGaugeDlg->SetPartyMaster(pInfo->bPartyMaster);
				m_pPlayerGaugeDlg->ShowPartyQuitButton(true);

				// 운영자 난입에 의해 싱글이었다가 파티상태가 된거라면 탈퇴버튼 안보여야한다.
				if( GetPartyTask().IsSingleToPartyByGMTrace() )
					m_pPlayerGaugeDlg->ShowPartyQuitButton(false);
			}
			else
			{
				_ASSERT(0);
			}
#ifdef PRE_WORLDCOMBINE_PARTY
			if (CDnPartyTask::GetInstance().IsPartyTypeNeedRaidPartyTypeUI() == false)
				continue;
#else
			if (CDnPartyTask::GetInstance().IsPartyType(_RAID_PARTY_8) == false)
				continue;
#endif
		}
		if( pInfo->hActor ) {
			((CDnPlayerActor*)pInfo->hActor.GetPointer())->SetPartyLeader( pInfo->bPartyMaster );
		}

#ifdef PRE_WORLDCOMBINE_PARTY
		if (CDnPartyTask::GetInstance().IsPartyTypeNeedRaidPartyTypeUI())
#else
		if (CDnPartyTask::GetInstance().IsPartyType(_RAID_PARTY_8))
#endif
		{
			if (pInfo->cMemberIndex < 0 || pInfo->cMemberIndex >= m_pVecPartyPlayerGaugeList.size())
				continue;

			nPartyPalyerGaugeIndex = pInfo->cMemberIndex;
		}

		//rlkt_test 19.06 fix new party face id!
		m_pVecPartyPlayerGaugeList[nPartyPalyerGaugeIndex].pGaugeDlg->SetFaceID( pInfo->cClassID - 1 );

		nVecUsingGaugeList.push_back( nPartyPalyerGaugeIndex );

		
		CDnGaugeDlg* pCurGaugeDlg = m_pVecPartyPlayerGaugeList[nPartyPalyerGaugeIndex].pGaugeDlg;
		if (pCurGaugeDlg)
		{
			
			// 아래에 액터가 안들어있기 때문에,
			//MAPartsBody *pPartsBody = dynamic_cast< MAPartsBody * >( pInfo->hActor.GetPointer() );
			// PartyStruct 내용안에 있는 걸로 간단하게 액터를 만든 후 넘기도록 한다.
			// 그런데 그냥 지역변수로 만들면, 다음번 프레임에서 RTT돌릴때 이미 삭제되어있을테니, 멤버로해서 유지되도록 한다.
			//
			// 문제는 액터를 통째로 생성하기때문에 예전보다 조금 느려진건데, 이 함수는 상당히 빈번하게 호출된다는 것이다.
			// 그래서 갱신될때마다 느려지는 걸 방지하기 위해 이전값을 가지고 있다가 비교하도록 하겠다.
			CDnRenderAvatarNew &TempAvatar = m_TempAvatar[nPartyPalyerGaugeIndex];
			if( TempAvatar.GetActorID() != pInfo->cClassID ) {
				TempAvatar.ResetActor( false );
				TempAvatar.SetActor( pInfo->cClassID, false );
				TempAvatar.SetRenderFrameCount( 0 );	// 직접 RTT에 쓸게 아니고, 액터만 들고있을거라서 0으로 설정한다. RTT에서 동작하지 않도록.

				// 액터가 바뀌면, 아래값 전부 초기화
				m_nCompareInfo[nPartyPalyerGaugeIndex][0] = m_nCompareInfo[nPartyPalyerGaugeIndex][1] = m_nCompareInfo[nPartyPalyerGaugeIndex][2] = m_nCompareInfo[nPartyPalyerGaugeIndex][3] = 0;
			}

			if( m_nCompareInfo[nPartyPalyerGaugeIndex][0] != pInfo->nFaceID ) {
				TempAvatar.AttachItem( pInfo->nFaceID );
				m_nCompareInfo[nPartyPalyerGaugeIndex][0] = pInfo->nFaceID;
			}
			if( m_nCompareInfo[nPartyPalyerGaugeIndex][1] != pInfo->nHairID ) {
				TempAvatar.AttachItem( pInfo->nHairID );
				m_nCompareInfo[nPartyPalyerGaugeIndex][1] = pInfo->nHairID;
			}
			if( m_nCompareInfo[nPartyPalyerGaugeIndex][2] != pInfo->nHelmetID ) {
				// 장착하고 있다가 해제할때 0으로 오는데, 문제는 렌더아바타 특성상 Detach를 지원하지 않는다는 것이다.
				// 헬멧의 경우에 캐시아이템, 일반아이템 쓰고 있다가 캐시를 벗으면 일반으로 덮어야하기때문에 별도로 장착해제시킨다.
				if( pInfo->nHelmetID > 0 && CDnItem::IsCashItem( pInfo->nHelmetID ) == false )
				{
					MAPartsBody *pPartsBody = dynamic_cast<MAPartsBody *>( TempAvatar.GetActor().GetPointer() );
					if( pPartsBody )
					{
						pPartsBody->DetachParts( CDnParts::Helmet );
						pPartsBody->DetachCashParts( CDnParts::CashHelmet );
					}
				}
				TempAvatar.AttachItem( pInfo->nHelmetID );
				m_nCompareInfo[nPartyPalyerGaugeIndex][2] = pInfo->nHelmetID;
			}
			if( m_nCompareInfo[nPartyPalyerGaugeIndex][3] != pInfo->nEarringID ) {
				// 헬멧과 마찬가지.
				TempAvatar.AttachItem( pInfo->nEarringID );
				m_nCompareInfo[nPartyPalyerGaugeIndex][3] = pInfo->nEarringID;
			}

			MAPartsBody *pPartsBody = dynamic_cast<MAPartsBody *>( TempAvatar.GetActor().GetPointer() );
			if( pPartsBody )
			{
				pPartsBody->SetPartsColor( MAPartsBody::SkinColor, pInfo->dwSkinColor );
				pPartsBody->SetPartsColor( MAPartsBody::EyeColor, pInfo->dwEyeColor );
				pPartsBody->SetPartsColor( MAPartsBody::HairColor, pInfo->dwHairColor );

				// 그래서 이렇게 해제한다.
				if( pInfo->nHelmetID == 0 )
				{
					pPartsBody->DetachParts( CDnParts::Helmet );
					pPartsBody->DetachCashParts( CDnParts::CashHelmet );
				}
				if( pInfo->nEarringID == 0 )
				{
					pPartsBody->DetachParts( CDnParts::Earring );
					pPartsBody->DetachCashParts( CDnParts::CashEarring );
				}
			}
			if( ( pPartsBody ) && ( pPartsBody->GetObjectHandle() ) )
			{
				int nAniIndex = TempAvatar.GetActor()->GetCachedAniIndex( "Normal_Stand" );
				if( nAniIndex != -1 )
				{
					// 파티원의 경우에 FaceTatoo텍스처가 셋팅안되서 검게 나올때가 종종 생긴다. 그래서 강제로 한번만 더 호출해주기로 한다.
					pPartsBody->ForceUpdateFaceDeco();
					pCurGaugeDlg->UpdatePortrait( pPartsBody->GetObjectHandle(), false, false, nAniIndex, 0.0f );
				}
			}

			

			//rlkt_ok!
			//pCurGaugeDlg->SetPartyIconIndex(pInfo->nVecJobHistoryList[pInfo->nVecJobHistoryList.size()-1]); //damn :))

			//pCurGaugeDlg->UpdatePortrait();
			pCurGaugeDlg->SetSessionID( pInfo->nSessionID );
#ifdef PRE_WORLDCOMBINE_PARTY
			pCurGaugeDlg->SetPlayerName( pInfo->cLevel, pInfo->wszCharacterName, GAUGE_PLAYERNAME_NO_COLOR, pInfo->nWorldSetID );
#else
			pCurGaugeDlg->SetPlayerName( pInfo->cLevel, pInfo->wszCharacterName );
#endif
			pCurGaugeDlg->SetHP( 1.f );
			pCurGaugeDlg->SetSP( 1.f );
			pCurGaugeDlg->Show(true);
			pCurGaugeDlg->SetPartyMaster(pInfo->bPartyMaster);
		}
		m_pVecPartyPlayerGaugeList[nPartyPalyerGaugeIndex].hActor = pInfo->hActor;

#ifdef PRE_WORLDCOMBINE_PARTY
		if (CDnPartyTask::GetInstance().IsPartyTypeNeedRaidPartyTypeUI() == false)
#else
		if (CDnPartyTask::GetInstance().IsPartyType(_RAID_PARTY_8) == false)
#endif
			nPartyPalyerGaugeIndex++;
	}

#ifdef PRE_FIX_CLIENT_MEMOPTIMIZE
	for( int i=0; i<PARTYCOUNTMAX; i++ ) {
		if( std::find( nVecUsingGaugeList.begin(), nVecUsingGaugeList.end(), i ) != nVecUsingGaugeList.end() ) continue;
		m_TempAvatar[i].ResetActor( false );
	}
#endif
}

void CDnInterface::RefreshGuildWarSituation()
{
	if( !CDnPartyTask::IsActive() )
		return;

	if( m_pGuildWarSituation )
		m_pGuildWarSituation->Refresh();
}

void CDnInterface::RefreshPartyVoiceButtonMode( UINT nSessionID, bool bAvailable, bool bMute )
{
	if( IsPVP() || IsFarm() ) 
		return;

	int nPartyPalyerGaugeIndex(0);

	for( DWORD i=0; i<GetPartyTask().GetPartyCount(); i++ )
	{
		if( (int)m_pVecPartyPlayerGaugeList.size() <= nPartyPalyerGaugeIndex )
			break;

		CDnPartyTask::PartyStruct *pInfo = GetPartyTask().GetPartyData(i);
		if( !pInfo ) continue;

		// 자신이면 continue
		if( pInfo->nSessionID == CDnBridgeTask::GetInstance().GetSessionID() ) continue;

		if( pInfo->nSessionID == nSessionID )
		{
			CDnGaugeDlg* pCurGaugeDlg = m_pVecPartyPlayerGaugeList[nPartyPalyerGaugeIndex].pGaugeDlg;
			if (pCurGaugeDlg)
			{
#ifdef _USE_VOICECHAT
				pCurGaugeDlg->UpdateVoiceButtonMode(bAvailable, bMute);
#endif
			}
		}
		nPartyPalyerGaugeIndex++;
	}
}

void CDnInterface::RefreshPartySpeaking( UINT nSessionID, bool bSpeaking )
{
	if( IsPVP() || IsFarm() ) 
		return;

	int nPartyPalyerGaugeIndex(0);

	for( DWORD i=0; i<GetPartyTask().GetPartyCount(); i++ )
	{
		if( (int)m_pVecPartyPlayerGaugeList.size() <= nPartyPalyerGaugeIndex )
			break;

		CDnPartyTask::PartyStruct *pInfo = GetPartyTask().GetPartyData(i);
		if( !pInfo ) continue;

		// 자신이면 continue
		if( pInfo->nSessionID == CDnBridgeTask::GetInstance().GetSessionID() ) continue;

		if( pInfo->nSessionID == nSessionID )
		{
			CDnGaugeDlg* pCurGaugeDlg = m_pVecPartyPlayerGaugeList[nPartyPalyerGaugeIndex].pGaugeDlg;
			if (pCurGaugeDlg)
			{
#ifdef _USE_VOICECHAT
				pCurGaugeDlg->UpdateVoiceSpeaking(bSpeaking);
#endif
			}
		}
		nPartyPalyerGaugeIndex++;
	}
}

void CDnInterface::HideVoiceButtonMode()
{
#ifdef _USE_VOICECHAT
	if( m_pPlayerGaugeDlg )
		m_pPlayerGaugeDlg->HideVoiceButtonMode();

	for( DWORD i = 0; i<m_pVecPartyPlayerGaugeList.size(); i++ )
		m_pVecPartyPlayerGaugeList[i].pGaugeDlg->HideVoiceButtonMode();
#endif
}

void CDnInterface::SetPartyInfoLeader( int nLeaderIndex )
{
	if( !m_pMainMenuDlg ) return;
	if( nLeaderIndex < 0 ) return;

	m_pMainMenuDlg->SetPartyInfoLeader( nLeaderIndex );
}

void CDnInterface::SetPartyCreateDifficulties(const BYTE* pDifficulties)
{
	if( !m_pMainMenuDlg ) return;
	m_pMainMenuDlg->SetPartyCreateDifficulties(pDifficulties);
}

void CDnInterface::OnPartyJoin()
{
	if( !m_pMainMenuDlg ) return;

	m_pMainMenuDlg->OnPartyJoin();
}

void CDnInterface::SwapPartyDialog()
{
	if( !m_pMainMenuDlg ) return;
	m_pMainMenuDlg->SwapPartyDialog();
}

int CDnInterface::GetPartyListStartIndex()
{
	return m_pMainMenuDlg->GetPartyListStartIndex();
}

void CDnInterface::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg )
{
	switch( nID ) 
	{
		//rlkt_rebirth
	case MESSAGE_BOX_RLKT_REBIRTH:
	{
			if (nCommand == EVENT_BUTTON_CLICKED)
			{
				if (strcmp(pControl->GetControlName(), "ID_YES") == 0)
				{
					CDnRebirthSystem::GetInstance().OnPressOK();
					OutputDebug("[%s] On YES Click!", __FUNCTION__);
				}
				else if (strcmp(pControl->GetControlName(), "ID_NO") == 0)
				{
					OutputDebug("[%s] On NO Click!", __FUNCTION__);
				}
			}
	}
	break;

	case MESSAGE_BOX_RLKT_FARMPVP:
	{
			if (nCommand == EVENT_BUTTON_CLICKED)
			{
				if (strcmp(pControl->GetControlName(), "ID_YES") == 0)
				{
					Farm::Send::SendReqFarmInfo();
					//GetInterface().MessageBox(L"Traveling to farm pvp!");
				}
			}
	}
	break;

	case MESSAGEBOX_21:
		{
			if( nCommand == EVENT_BUTTON_CLICKED ) 
			{
				if( strcmp( pControl->GetControlName(), "ID_YES" ) == 0 ) 
				{
					if( CDnLoadingTask::IsActive() ) CDnLoadingTask::GetInstance().EnableBackgroundLoading( false );
					CTaskManager::GetInstance().RemoveAllTask( false );
				}
#if defined(_CH) && defined(_AUTH)
				else
				{
					if( m_Type == InterfaceTypeEnum::Login )
					{
						if( m_pLoginDlg && m_pLoginDlg->IsShow() )
						{
							CDnSDOAService::GetInstance().Show( true );
							m_pLoginDlg->ShowSDOALoginDialog( true );	// OA창의 x버튼으로 닫을 경우 다시 로그인창을 열어줘야한다.
						}
					}
				}
#endif	// #if defined(_CH) && defined(_AUTH)
			}
		}
		break;
	case DUNGEON_MOVE_DIALOG:
		if( nCommand == EVENT_BUTTON_CLICKED )
		{
			if( strcmp( pControl->GetControlName(), "ID_TO_WORLD" ) == 0 ) 
			{
				TerminateStageClearWarpStandBy();
				SendWarpDungeon( false );
			}
			else if( strcmp( pControl->GetControlName(), "ID_TO_VILLAGE" ) == 0 )
			{
				TerminateStageClearWarpStandBy();
				SendWarpDungeon( true );
			}
		}
		break;	

/*	case FADE_DIALOG:
		{
			if( nCommand == EVENT_FADE_COMPLETE )
			{
				switch(m_FadeState)
				{
				case eFS_CASHSHOP_OPEN_FO:
				case eFS_CASHSHOP_CLOSE_FO:
					{
						m_pMainMenuDlg->ToggleShowDialog(CDnMainMenuDlg::CASHSHOP_DIALOG);
						m_FadeState = eFS_CASHSHOP_OPEN_FI;
					}
					break; 

				case eFS_CASHSHOP_OPEN_FI:
					{
						m_FadeState = eFS_NONE;
					}
					break;
				}
			}
		}
		break;
		*/
	}
}

// Chat
void CDnInterface::AddChatMessage( eChatType eType, LPCWSTR wszFromCharName, LPCWSTR wszChatMsg, bool bShowCaption, bool isAppend, bool hasColor, DWORD colorValue, DWORD bgColorValue )
{
	bool bUseSendMsg = false;
	if( CDnLoadingTask::IsActive() && CDnLoadingTask::GetInstance().GetThreadID() == GetCurrentThreadId() ) bUseSendMsg = true;
	if( bUseSendMsg )
	{
		// 로딩쓰레드에서 로딩 후 채팅창에 시스템메세지같은거 띄울때
		// 쓰레드에서 직접 텍스트 추가하면 텍스트박스에서 뻑날 수 있으니, 자신에게 패킷 되돌려 메인쓰레드에서 처리되게 한다.
		if( CClientSessionManager::IsActive() ) CClientSessionManager::GetInstance().LockProcessDispatchMessage( true );
		SCChat ChatMsg;
		memset(&ChatMsg, 0, sizeof(SCChat));
		ChatMsg.eType = eType;
		ChatMsg.nLen = (short)wcslen( wszChatMsg );
		_wcscpy( ChatMsg.wszFromCharacterName, _countof(ChatMsg.wszFromCharacterName), wszFromCharName, (int)wcslen(wszFromCharName) );
		_wcscpy( ChatMsg.wszChatMsg, _countof(ChatMsg.wszChatMsg), wszChatMsg, (int)wcslen(wszChatMsg) );
		int iLen = (int)(sizeof(SCChat) - sizeof(ChatMsg.wszChatMsg) + (ChatMsg.nLen * 2));
		if( CClientSessionManager::IsActive() ) CClientSessionManager::GetInstance().RecvPacket( SC_CHAT, eChat::SC_CHATMSG, (char*)&ChatMsg, iLen, 0 );
		if( CClientSessionManager::IsActive() ) CClientSessionManager::GetInstance().LockProcessDispatchMessage( false );
		return;
	}

	LogWnd::Log(1,_T("[%s] %s"), wszFromCharName, wszChatMsg);

	// 욕설필터링을 Send하기전에 처리하니 치트도 자꾸 필터링된다고 해서 받았을때 필터링하는걸로 바꾼다.
	std::wstring wszTempMsg = wszChatMsg;
#ifndef _FINAL_BUILD
	bool bCheckDebugMessage = false;
	if( eType == CHATTYPE_NORMAL && __wcsicmp_l( wszFromCharName, L"Debug" ) == 0 ) bCheckDebugMessage = true;
	if( bCheckDebugMessage == false )
#endif
	{
		PROFILE_TICK_TEST_BLOCK_START( "CheckChat Filter" );

		if( eType == CHATTYPE_NORMAL	 || eType == CHATTYPE_PARTY || eType == CHATTYPE_PRIVATE  || eType == CHATTYPE_GUILD ||
			eType == CHATTYPE_CHANNEL	 || eType == CHATTYPE_WORLD || eType == CHATTYPE_TEAMCHAT || eType == CHATTYPE_CHATROOM ||
			eType == CHATTYPE_RAIDNOTICE
#ifdef PRE_PRIVATECHAT_CHANNEL
			|| eType == CHATTYPE_PRIVATECHANNEL
#endif // PRE_PRIVATECHAT_CHANNEL
#ifdef PRE_ADD_DOORS
			|| eType == CHATTYPE_PRIVATE_MOBILE
#endif
#ifdef PRE_ADD_DWC
			|| eType == CHATTYPE_DWC_TEAM
#endif
			)
		{
			DN_INTERFACE::UTIL::CheckChat( wszTempMsg, '*' );
		}
		PROFILE_TICK_TEST_BLOCK_END();
	}
	wszChatMsg = wszTempMsg.c_str();

	if( eType == CHATTYPE_NORMAL || eType == CHATTYPE_PRIVATE || eType == CHATTYPE_GUILD || eType == CHATTYPE_SYSTEM || eType == CHATTYPE_CHATROOM ||
		eType == CHATTYPE_GM || eType == CHATTYPE_PRIVATE_GM 
#ifdef PRE_ADD_DOORS
		|| eType == CHATTYPE_PRIVATE_MOBILE
#endif
#ifdef PRE_ADD_DWC
		|| eType == CHATTYPE_DWC_TEAM
#endif
		)
	{
		if( m_pChatRoomDlg && m_pChatRoomDlg->IsShow() )
		{
			m_pChatRoomDlg->AddChatMessage( eType, wszFromCharName, wszChatMsg );
			return;
		}
	}

	CDnGameTask * pGameTask = (CDnGameTask *)CTaskManager::GetInstance().GetTask( "GameTask" );
	if( pGameTask && pGameTask->GetGameTaskType() == GameTaskType::Farm && eType == CHATTYPE_PARTY_GM )
		eType = CHATTYPE_GM;

	if( !GetChatDialog() ) return;
	int nChatType(CHAT_NORMAL);

	switch( eType )
	{
	case CHATTYPE_SYSTEM:
		GetChatDialog()->AddSystemMessage( wszFromCharName, wszChatMsg, bShowCaption );
		GetChatDialog(true)->AddSystemMessage( wszFromCharName, wszChatMsg, false );	// modified by kalliste
		return;
	case CHATTYPE_NOTICE:	GetChatDialog()->AddNoticeMessage( wszFromCharName, wszChatMsg ); return;
	case CHATTYPE_DICE:		
		{
			if (GetIsolateTask().IsBlackList(wszFromCharName))
				return;

			std::wstring result;

			std::vector<std::wstring> tokens;
			TokenizeW( wszChatMsg, tokens, L"//" );

			int value = _wtoi(tokens[1].c_str());
			int range = _wtoi(tokens[2].c_str());

			if( !CDnActor::s_hLocalActor )
				return;

			if (!_tcscmp(tokens[0].c_str(), CDnActor::s_hLocalActor->GetName()))
				result = FormatW(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3581 ), value, range);
			else
				result = FormatW(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3582 ), tokens[0].c_str(), value, range);

			GetChatDialog()->AddSystemMessage( L"", result.c_str(), false);
		}
		return;

	// GM의 경우 별도처리.
	case CHATTYPE_GM:			GetChatDialog()->AddChatMessage( CHAT_NORMAL, wszFromCharName, wszChatMsg, false, false, 0xFFFFFFFF, D3DCOLOR_ARGB(0x80,0x22,0x8B,0x22)); return;
	case CHATTYPE_PARTY_GM:		GetChatDialog()->AddChatMessage( CHAT_PARTY, wszFromCharName, wszChatMsg, false, false, 0xFFFFFFFF, D3DCOLOR_ARGB(0x80,0x22,0x8B,0x22)); return;
	case CHATTYPE_PRIVATE_GM:	GetChatDialog()->AddChatMessage( CHAT_PRIVATE, wszFromCharName, wszChatMsg, false, false, 0xFFFFFFFF, D3DCOLOR_ARGB(0x80,0x22,0x8B,0x22)); return;

	// 블라인드 메시지 별도 처리.
	case CHATTYPE_BLIND:		GetBlindDialog()->SetCaption( wszChatMsg, 5000 );	return;
	}

	switch( eType )
	{
	case CHATTYPE_NORMAL:		nChatType = CHAT_NORMAL;	break;
	case CHATTYPE_PRIVATE:		nChatType = CHAT_PRIVATE;	break;
	case CHATTYPE_PARTY:		nChatType = CHAT_PARTY;		break;
	case CHATTYPE_GUILD:		nChatType = CHAT_GUILD;		break;
	case CHATTYPE_CHANNEL:		nChatType = CHAT_CHANNEL;	break;
	case CHATTYPE_WORLD:		nChatType = CHAT_WORLD;		break;
#ifdef PRE_PRIVATECHAT_CHANNEL
	case CHATTYPE_PRIVATECHANNEL:	nChatType = CHAT_PRIVATE_CHANNEL;	break;
#endif // PRE_PRIVATECHAT_CHANNEL
	case CHATTYPE_TEAMCHAT:			nChatType = CHAT_PARTY; break;
#ifdef PRE_ADD_DOORS
	case CHATTYPE_PRIVATE_MOBILE:	nChatType = CHAT_PRIVATE; break;
#endif
#ifdef PRE_ADD_DOORS_GUILDCHAT_DISCONNECT
	case CHATTYPE_GUILD_MOBILE:		nChatType = CHAT_GUILD; break;
#endif
#ifdef PRE_ADD_DWC
	case CHATTYPE_DWC_TEAM:			nChatType = CHAT_GUILD; break;
#endif
#ifdef PRE_ADD_WORLD_MSG_RED
	case CHATTYPE_WORLD_POPMSG:		nChatType = CHAT_WORLD_RED; break;
#endif // PRE_ADD_WORLD_MSG_RED
	case CHATTYPE_RAIDNOTICE:
		{
			nChatType = CHAT_RAIDNOTICE;
			int soundIndex = CDnPartyTask::GetInstance().GetPartySoundIndex(CDnPartyTask::REQUESTREADY);

			if( CDnBridgeTask::GetInstance().IsPvPGameMode( PvPCommon::GameMode::PvP_GuildWar ) )
			{
				CDnPartyTask::PartyStruct * pData = GetPartyTask().GetPartyDataFromMemberName( wszFromCharName );

				if( pData )
				{
					if (CDnLocalPlayerActor::s_hLocalActor)
					{
						CDnLocalPlayerActor *pLocalActor = dynamic_cast<CDnLocalPlayerActor *>(CDnLocalPlayerActor::s_hLocalActor.GetPointer());
						if( pLocalActor )
						{
							if( pLocalActor->GetTeam() == pData->usTeam )
								CEtSoundEngine::GetInstance().PlaySound("2D", soundIndex);
						}
					}
				}
			}
			else
				CEtSoundEngine::GetInstance().PlaySound("2D", soundIndex);
		}
		break;

	default:
		ASSERT(0&&"CDnInterface::AddChatMessage");
		break;
	}

#ifdef PRE_ADD_DOORS
	#ifdef PRE_ADD_DOORS_GUILDCHAT_DISCONNECT
	bool bDoorsMobile = false;
	if (eType == CHATTYPE_PRIVATE_MOBILE || eType == CHATTYPE_GUILD_MOBILE)
		bDoorsMobile = true;
	#else
	bool bDoorsMobile = (eType == CHATTYPE_PRIVATE_MOBILE);
	#endif
	GetChatDialog()->AddChatMessage( nChatType, wszFromCharName, wszChatMsg, isAppend, hasColor, colorValue, bgColorValue, bDoorsMobile);
	#ifndef _FINAL_BUILD
	if( bCheckDebugMessage ) return;
	#endif

	GetChatDialog(true)->AddChatMessage( nChatType, wszFromCharName, wszChatMsg, isAppend, hasColor, colorValue, bgColorValue, bDoorsMobile );
#else // PRE_ADD_DOORS
	GetChatDialog()->AddChatMessage( nChatType, wszFromCharName, wszChatMsg, isAppend, hasColor, colorValue, bgColorValue );

	#ifndef _FINAL_BUILD
	if( bCheckDebugMessage ) return;
	#endif

	GetChatDialog(true)->AddChatMessage( nChatType, wszFromCharName, wszChatMsg, isAppend, hasColor, colorValue, bgColorValue );
#endif // PRE_ADD_DOORS
}

void CDnInterface::SendChatMessage( eChatType type, const std::wstring& fromCharName, std::wstring& chatMsg)
{
	if( !GetChatDialog() ) return;
	int nChatType(CHAT_NORMAL);

	switch( type )
	{
	case CHATTYPE_NORMAL:		nChatType = CHAT_NORMAL;	break;
	case CHATTYPE_PRIVATE:		nChatType = CHAT_PRIVATE;	break;
	case CHATTYPE_PARTY:		nChatType = CHAT_PARTY;		break;
	case CHATTYPE_GUILD:		nChatType = CHAT_GUILD;		break;
	case CHATTYPE_CHANNEL:		nChatType = CHAT_CHANNEL;	break;
	case CHATTYPE_WORLD:		nChatType = CHAT_WORLD;		break;
#ifdef PRE_PRIVATECHAT_CHANNEL
	case CHATTYPE_PRIVATECHANNEL:	nChatType = CHAT_PRIVATE_CHANNEL;	break;
#endif // PRE_PRIVATECHAT_CHANNEL
	case CHATTYPE_TEAMCHAT:		nChatType = CHAT_PARTY;		break;
	default:
		ASSERT(0&&"CDnInterface::SendChatMessage");
		break;
	}


	GetChatDialog()->SendChat(nChatType, chatMsg, fromCharName);
}

void CDnInterface::AddNoticeMessage(const WCHAR * pMsg, int nShowSec)
{
	//준후씨 여기서 nShowSec이 0보다 크면 슬라이드쇼? ㅋㅋㅋㅋ입니다 초단위이고 받은 순간부터 시간만큼 보여주면 됩니다.
	if( nShowSec == 0)
		AddChatMessage( CHATTYPE_NOTICE, L"", pMsg, false );
	else
		AddSlideCaptionMessage( pMsg, textcolor::WHITE, (float)nShowSec );
}

//void CDnInterface::AddWorldSystemMessage( char cType, LPCWSTR wszFromCharName, int nID, int nValue )
void CDnInterface::AddWorldSystemMessage( SCWorldSystemMsg *pPacket )
{
	char cType = pPacket->cType;
	LPCWSTR wszFromCharName = pPacket->wszFromCharacterName;
	int nID = pPacket->nID;
	int nValue = pPacket->nValue;

	WCHAR wszStr[256];
	CDnItem *pItem = NULL;

	switch( cType )
	{
	case WORLDCHATTYPE_ENCHANTITEM:
		{
			DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TITEM );
			if( !pSox ) return;
			std::wstring wszItemName;
			int nNameID = pSox->GetFieldFromLablePtr( nID, "_NameID" )->GetInteger();
			char *szParam = pSox->GetFieldFromLablePtr( nID, "_NameIDParam" )->GetString();
			MakeUIStringUseVariableParam( wszItemName, nNameID, szParam );
			wsprintf( wszStr, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8012 ), wszFromCharName, nValue, wszItemName.c_str() );
			GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", wszStr, false );
		}
		break;

	case WORLDCHATTYPE_MISSION:
		{
			DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TMISSION );
			if( !pSox ) return;
			int nItemID = pSox->GetItemID( nID );
			int nUIStringID = pSox->GetFieldFromLablePtr( nItemID, "_TitleNameID" )->GetInteger();
			std::wstring wszTitleName;
			char *szParam = pSox->GetFieldFromLablePtr( nItemID, "_TitleNameIDParam" )->GetString();
			MakeUIStringUseVariableParam( wszTitleName, nUIStringID, szParam );
			wsprintf( wszStr, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7129 ), wszFromCharName, wszTitleName.c_str() );
			GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", wszStr, false );
		}
		break;
	case WORLDCHATTYPE_CHARMITEM:
		{
			// 골드아이템의 경우 - 아이템명이 아닌 골드를 출력해야함.
			if( nID == g_cGoldItemID )
			{
				// 골드획득 - 0인 단위는 출력하지 않음. (ex> 1골드0실버1쿠퍼 => 1골드1쿠퍼 ).
				if( nValue > 0 )
				{	
					std::wstring strText;
					wchar_t str[256] = {0,};

					// 골드.
					INT64 coin = nValue / 10000;
					if( coin > 0 )
					{
						swprintf_s( str, _countof(str), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1766 ), coin );
						strText.assign( str );
					}

					// 실버.
					coin = (nValue % 10000) / 100;
					if( coin > 0 )
					{
						strText.append( L" " );
						swprintf_s( str, _countof(str), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1781 ), coin );			
						strText.append( str );
					}

					// 코퍼.
					coin = nValue % 100;
					if( coin > 0 )
					{
						strText.append( L" " );
						swprintf_s( str, _countof(str), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1783 ), coin );			
						strText.append( str );
					}
				
					_wcscpy( str, _countof(str), strText.c_str(), (int)wcslen(strText.c_str()) );
					wsprintf( wszStr, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7057 ), wszFromCharName, str );
				}
			}

			// 아이템.
			else
			{
#ifdef PRE_FIX_68828
				DNTableFileFormat* pItemSox = GetDNTable(CDnTableDB::TITEM);
				DNTableFileFormat* pSox = GetDNTable(CDnTableDB::TCHARMITEM);
				if (pSox == NULL || pItemSox == NULL)
					return;

				std::wstring wszItemName;
				DNTableCell* pMsgTypeCell = pSox->GetFieldFromLablePtr(nID, "_MsgType");
				DNTableCell* pItemIDCell = pSox->GetFieldFromLablePtr(nID, "_ItemID");
				if (pMsgTypeCell && pItemIDCell)
				{
					int itemId = pItemIDCell->GetInteger();
					if (itemId <= 0)
					{
						_ASSERT(0);
						return;
					}

					DNTableCell* pItemNameCell = pItemSox->GetFieldFromLablePtr(itemId, "_NameID" );
					DNTableCell* pItemNameParamCell = pItemSox->GetFieldFromLablePtr(itemId, "_NameIDParam");
					if (pItemNameCell == NULL || pItemNameParamCell == NULL)
					{
						_ASSERT(0);
						return;
					}

					int nNameID = pItemNameCell->GetInteger();
					char *szParam = pItemNameParamCell->GetString();
					MakeUIStringUseVariableParam(wszItemName, nNameID, szParam);

					eCharmItemSystemMsgType type = (eCharmItemSystemMsgType)pMsgTypeCell->GetInteger();
					if (type == eTYPE_NORMAL)
						wsprintf( wszStr, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7057 ), wszFromCharName, wszItemName.c_str() );
					else if (type == eTYPE_TREASURE)
						wsprintf( wszStr, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7047 ), wszFromCharName, wszItemName.c_str() );
				}
#else
				DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TITEM );
				if( !pSox ) return;
				std::wstring wszItemName;
				int nNameID = pSox->GetFieldFromLablePtr( nID, "_NameID" )->GetInteger();
				char *szParam = pSox->GetFieldFromLablePtr( nID, "_NameIDParam" )->GetString();
				MakeUIStringUseVariableParam( wszItemName, nNameID, szParam );
				wsprintf( wszStr, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7057 ), wszFromCharName, wszItemName.c_str() );
#endif
			}

			GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", wszStr, false );
		}
		break;
	case WORLDCHATTYPE_DONATION:
		{			
			wsprintf( wszStr, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7737 ), wszFromCharName );
			GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", wszStr, false );
		}
		break;
	case WORLDCHATTYPE_NAMEDITEM:
		{
			DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TITEM );
			if( !pSox ) return;
			std::wstring wszItemName;
			wchar_t szClass[256]={0};
			int nNameID = pSox->GetFieldFromLablePtr( nID, "_NameID" )->GetInteger();
			char *szParam = pSox->GetFieldFromLablePtr( nID, "_NameIDParam" )->GetString();
			MakeUIStringUseVariableParam( wszItemName, nNameID, szParam );
			swprintf_s( szClass, 256, L"%s", DN_INTERFACE::STRING::GetJobString( nValue ) );
			wsprintf( wszStr, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 100076), szClass, wszFromCharName, wszItemName.c_str() );
			GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", wszStr , true );

			int nSoundIndex = -1;
			nSoundIndex = CEtSoundEngine::GetInstance().LoadSound( CEtResourceMng::GetInstance().GetFullName( "UI_Named Item.ogg" ).c_str(), false, false );
			if( nSoundIndex != -1 ) 
				CEtSoundEngine::GetInstance().PlaySound( "2D", nSoundIndex );
		}
		break;

#ifdef PRE_ADD_CHAOSCUBE
	case WORLDCHATTYPE_CHAOSITEM:
		{
			wchar_t wszStr[256] = {0,};
			
			// 골드아이템의 경우 - 아이템명이 아닌 골드를 출력해야함.
			if( nID == g_cGoldItemID )
			{
				// 골드획득 - 0인 단위는 출력하지 않음. (ex> 1골드0실버1쿠퍼 => 1골드1쿠퍼 ).
				if( nValue > 0 )
				{
					std::wstring str;
					GoldToString( str, nValue,
								  GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1766 ),
								  GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1781 ),		
								  GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1783 ) );			
					wsprintf( wszStr, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7030 ), wszFromCharName, str.c_str() );
				}
			}

			// 아이템.
			else
			{
				DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TITEM );
				if( !pSox ) 
					return;

				std::wstring wszItemName;
				int nNameID = pSox->GetFieldFromLablePtr( nID, "_NameID" )->GetInteger();
				char *szParam = pSox->GetFieldFromLablePtr( nID, "_NameIDParam" )->GetString();
				MakeUIStringUseVariableParam( wszItemName, nNameID, szParam );
				wsprintf( wszStr, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7030 ), wszFromCharName, wszItemName.c_str() );
			}			
			
			GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", wszStr, false ); // "%s님의 카오스큐브에서 희귀아이템이 나타났습니다(%s)"
			
		}
		break;
#endif
			
#ifdef PRE_ADD_CHOICECUBE
	case WORLDCHATTYPE_CHOICEITEM:
		{
			wchar_t wszStr[256] = {0,};

			// 골드아이템의 경우 - 아이템명이 아닌 골드를 출력해야함.
			if( nID == g_cGoldItemID )
			{
				// 골드획득 - 0인 단위는 출력하지 않음. (ex> 1골드0실버1쿠퍼 => 1골드1쿠퍼 ).
				if( nValue > 0 )
				{
					std::wstring str;
					GoldToString( str, nValue,
						GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1766 ),
						GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1781 ),		
						GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1783 ) );			
					wsprintf( wszStr, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7030 ), wszFromCharName, str.c_str() );
				}
			}

			// 아이템.
			else
			{
				DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TITEM );
				if( !pSox ) 
					return;

				std::wstring wszItemName;
				int nNameID = pSox->GetFieldFromLablePtr( nID, "_NameID" )->GetInteger();
				char *szParam = pSox->GetFieldFromLablePtr( nID, "_NameIDParam" )->GetString();
				MakeUIStringUseVariableParam( wszItemName, nNameID, szParam );
				wsprintf( wszStr, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7037 ), wszFromCharName, wszItemName.c_str() );
			}			

			GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", wszStr, false ); // "%s님의 카오스큐브에서 희귀아이템이 나타났습니다(%s)"

		}
		break;
#endif

#ifdef PRE_ADD_BESTFRIEND
	case WORLDCHATTYPE_BESTFRIEND:
		{
			wchar_t strMessage[512] = {0,};
			swprintf_s( strMessage, 512, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4336 ), pPacket->wszFromCharacterName, pPacket->wszToCharacterName ); // "%s님과 %s님께서 서로 절친으로 등록되었습니다."
			GetInterface().ShowCaptionDialog( emCAPTION_TYPE::typeCaption3, strMessage, textcolor::YELLOW );	
		}
		break;
#endif // PRE_ADD_BESTFRIEND
#if defined( PRE_ADD_DIRECTNBUFF )
	case WORLDCHATTYPE_DIRECTPARTYBUFF:
		{	
			DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TITEM );
			if( !pSox ) return;
			std::wstring wszItemName;
			int nNameID = pSox->GetFieldFromLablePtr( pPacket->nID, "_NameID" )->GetInteger();
			int nSkillID = pSox->GetFieldFromLablePtr( pPacket->nID, "_SkillID" )->GetInteger();
			char *szParam = pSox->GetFieldFromLablePtr( pPacket->nID, "_NameIDParam" )->GetString();
			MakeUIStringUseVariableParam( wszItemName, nNameID, szParam );

			DNTableFileFormat* pSkillSox = GetDNTable( CDnTableDB::TSKILL );
			if( !pSkillSox ) return;
			int iStringID = pSkillSox->GetFieldFromLablePtr( nSkillID, "_NameID" )->GetInteger();
			std::wstring wszSkillName = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, iStringID );

			std::wstring wstMessage; // "%s" 님이소유한 "%s" 아이템으로 인하여 "%s" 효과가 발동되었습니다"
			wstMessage = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 100084 ), pPacket->wszFromCharacterName, wszItemName.c_str() , wszSkillName.c_str() ); 
			GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", wstMessage.c_str() , true );
		}
		break;
#endif

#ifdef PRE_FIX_68828
	case WORLDCHATTYPE_COSMIX:
		{
			DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TITEM );
			if( !pSox ) return;
			std::wstring wszItemName;
			int nNameID = pSox->GetFieldFromLablePtr( nID, "_NameID" )->GetInteger();
			char *szParam = pSox->GetFieldFromLablePtr( nID, "_NameIDParam" )->GetString();
			MakeUIStringUseVariableParam( wszItemName, nNameID, szParam );
			wsprintf( wszStr, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7057 ), wszFromCharName, wszItemName.c_str() );

			GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", wszStr, false );
		}
		break;

	case WORLDCHATTYPE_COSMIX888:
	{
		DNTableFileFormat* pSox = GetDNTable(CDnTableDB::TITEM);
		if (!pSox) return;
		std::wstring wszItemName;
		int nNameID = pSox->GetFieldFromLablePtr(nID, "_NameID")->GetInteger();
		char *szParam = pSox->GetFieldFromLablePtr(nID, "_NameIDParam")->GetString();
		MakeUIStringUseVariableParam(wszItemName, nNameID, szParam);
		wsprintf(wszStr, GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 99999968), wszFromCharName, wszItemName.c_str());

		GetInterface().AddChatMessage(CHATTYPE_SYSTEM, L"", wszStr, false);
	}
	break;
#endif
	}
}

void CDnInterface::AddServerMessage( char cType, char cStringType, int nMsgIndex, char cCaptionType, BYTE cFadeTime )
{
	WCHAR wszStr[256];
	CDnItem *pItem = NULL;
	float fFadeTime = (float)cFadeTime;
	if( fFadeTime == 0)
		fFadeTime = 8.0f;

	switch( cType )
	{
	case CHATTYPE_SYSTEM:
		{			
			wsprintf( wszStr, GetEtUIXML().GetUIString( (CEtUIXML::emCategoryId)cStringType, nMsgIndex ) );
			GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", wszStr, false );
		}
		break;		
	}
	if( cCaptionType != -1 )
	{
		GetInterface().ShowCaptionDialog( (emCAPTION_TYPE)cCaptionType,  wszStr, textcolor::YELLOW, fFadeTime);
	}
}

void CDnInterface::SetCharStatusSlotEvent()
{
	CDnCharStatusDlg *pDialog = (CDnCharStatusDlg*)GetMainMenuDialog( CDnMainMenuDlg::CHARSTATUS_DIALOG );
	if( pDialog ) pDialog->SetSlotEvent();
}

void CDnInterface::ChangeState( CDnLoginTask::LoginStateEnum state, bool bHideOnly )
{
	CDnLoginTask::LoginStateEnum PrevState = m_State;
	m_State = state;

	if( PrevState != m_State )
	{
		switch( PrevState )
		{
		case CDnLoginTask::RequestIDPass:
			m_pLoginDlg->Show( false );
			m_pStaffrollDlg->Show( false );
#ifdef PRE_ADD_DOORS
			m_pDoorsMobileAuthDlg->Show(false);
#endif
			break;
		case CDnLoginTask::CharSelect:
			m_pCharCreateServerNameDlg->Show( false );
			m_pCharSelectDlg->Show( false );

#ifndef PRE_MOD_SELECT_CHAR
			ShowSlotInfoDlg( false );
			ShowSlotDeleteWaitDlg( false );
#endif // PRE_MOD_SELECT_CHAR
			break;
		case CDnLoginTask::CharCreate_SelectClassDark://rlkt_dark
			m_pCharSelectDlg->Show( false );
			m_pCharCreateDarkDlg->Show( false );
			m_pCharCreateBackDlg->Show( false );
			m_pCharCreateSelectDlg->Show( false );
			m_pCharCreateServerNameDlg->Show( false );
			break;
		case CDnLoginTask::CharCreate_SelectClass:
			m_pCharCreateBackDlg->Show( false );
#ifdef PRE_MOD_SELECT_CHAR
			m_pCharCreateSelectDlg->Show( false );
#else // PRE_MOD_SELECT_CHAR
			m_pCharCreateTitleDlg->Show( false );
#endif // PRE_MOD_SELECT_CHAR
			m_pCharCreateServerNameDlg->Show( false );
			break;
		case CDnLoginTask::CharCreate_ChangeParts:
#ifdef _ADD_NEWLOGINUI
			m_pCharCreateSetupDlg->Show(false);
			m_pCharCreateSetupGestureDlg->Show(false);
			m_pCharCreateNameDlg->Show(false);
			m_pCharCreateSetupBackDlg->Show(false); //_ADD_RLKT
			m_pCharCreateBackDlg->InitialUpdate(); //_ADD_RLKT
#else
			m_pCharCreateDlg->Show(false);
#endif
#ifdef PRE_MOD_SELECT_CHAR
			m_pCharCreateSelectDlg->ResetButton();
#else // PRE_MOD_SELECT_CHAR
			m_pCharCreateTitleDlg->Show(false);
			ShowCreateInfoList( false );
#endif // PRE_MOD_SELECT_CHAR
			m_pCharCreateServerNameDlg->Show( false );
			break;
		case CDnLoginTask::ServerList:
			m_pServerListDlg->Show( false );
			m_pStaffrollDlg->Show( false );
#ifdef PRE_ADD_DOORS
			m_pDoorsMobileAuthDlg->Show(false);
#endif
			break;
		case CDnLoginTask::ChannelList:
			m_pChannelListDlg->Show( false );
			break;
#ifdef PRE_ADD_DWC
		case CDnLoginTask::CharCreate_DWC:
			m_pDWCCharCreateDlg->Show(false);
			break;
#endif
		}
	}
	
	if( bHideOnly ) return;

#ifdef PRE_MOD_SELECT_CHAR
	if( m_State != CDnLoginTask::CharSelect && 
		m_State != CDnLoginTask::CharCreate_SelectClass &&
		m_State != CDnLoginTask::CharCreate_ChangeParts )
		ShowCharLoginTitleDlg( false );
#endif // PRE_MOD_SELECT_CHAR

	// 빌리지나 게임안에서 캐릭선택으로 오는 경우에만 m_nSelectedServerIndex값이 -1 이다.
	CDnBridgeTask *pBridgeTask = (CDnBridgeTask *)CTaskManager::GetInstance().GetTask( "BridgeTask" );

	switch( m_State ) 
	{
	case CDnLoginTask::RequestIDPass:
		{
#if !((defined(_KR) || defined(_KRAZ) || defined(_JP) || defined(_RU)) && defined(_AUTH))
			m_pLoginDlg->Show( true );
#elif defined(_US) && defined(_AUTH)
			DnNexonService* pDnNexonService = static_cast<DnNexonService*>( g_pServiceSetup );
			if( pDnNexonService && pDnNexonService->IsStandAloneMode() )
				m_pLoginDlg->Show( true );
#elif defined(_TW) && defined(_AUTH)
			DnTwnService* pDnTwnService = static_cast<DnTwnService*>( g_pServiceSetup );
			if( pDnTwnService && !pDnTwnService->IsWebLogin() )
				m_pLoginDlg->Show( true );
#endif // _KR
#if defined(_KRAZ) || defined(_WORK)
			m_pStaffrollDlg->Show( true );
#endif
#ifdef PRE_ADD_DOORS
			m_pDoorsMobileAuthDlg->Show(false);
#endif
		}
		break;

	case CDnLoginTask::CharSelect:
		if( pBridgeTask ) m_pCharCreateServerNameDlg->SetServerName( pBridgeTask->GetCurrentServerName() );
		m_pCharCreateServerNameDlg->Show( true );
		m_pCharSelectDlg->Show( true );

#ifdef PRE_MOD_SELECT_CHAR
		ShowCharLoginTitleDlg( true, 78 );
#else // PRE_MOD_SELECT_CHAR
		ShowSlotInfoDlg( true );
		ShowSlotDeleteWaitDlg( true );
#endif // PRE_MOD_SELECT_CHAR
		__time64_t tSecondAuthResetDate;
		tSecondAuthResetDate = GetDnAuthTask().GetSecondAuthResetDate();
		if( tSecondAuthResetDate != -1 )
		{
			std::wstring wszStrTime;
			DN_INTERFACE::STRING::GetDayText( wszStrTime, tSecondAuthResetDate );

			WCHAR wzStrTmp[1024]={0,};
			swprintf_s( wzStrTmp, _countof(wzStrTmp), L"%s %s", wszStrTime.c_str(), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 6476 ) );
			GetInterface().MessageBox( wzStrTmp );
		}
		m_pStaffrollDlg->Show(false);
#ifdef PRE_ADD_DOORS
		m_pDoorsMobileAuthDlg->Show(false);
#endif
		break;
	case CDnLoginTask::CharCreate_SelectClass:
#ifndef PRE_MOD_SELECT_CHAR
		m_pCharCreateBackDlg->Show( true );
		m_pCharCreateTitleDlg->Show( true );
#endif // PRE_MOD_SELECT_CHAR
		m_pCharCreateServerNameDlg->Show( true );
		break;
	case CDnLoginTask::CharCreate_ChangeParts:
#ifndef PRE_MOD_SELECT_CHAR
		m_pCharCreateDlg->Show( true );
		m_pCharCreateTitleDlg->Show( true );
#endif // PRE_MOD_SELECT_CHAR
		m_pCharCreateServerNameDlg->Show( true );
		break;
	case CDnLoginTask::ServerList:
		{
#if defined(_KRAZ) || defined(_WORK)
			m_pStaffrollDlg->Show( true );
#endif
#ifdef PRE_ADD_DOORS
	#ifdef PRE_REMOVE_DOORS_UITEMP
	#else
			m_pDoorsMobileAuthDlg->EnableAllControl(true);
			m_pDoorsMobileAuthDlg->Show(true);
#endif
#endif
		}
		break;
#ifdef PRE_ADD_DWC
	case CDnLoginTask::CharCreate_DWC:
		{			
			//m_pDWCCharCreateDlg->Show( true );
			//GetInterface().FadeDialog(0xFF000000, 0x00000000, 1.0f, NULL );
		}
		break;
#endif // PRE_ADD_DWC
	}
}

void CDnInterface::ShowChannelList(bool bShow)
{
	if( m_pChannelListDlg ) m_pChannelListDlg->Show(bShow);
}

void CDnInterface::ShowServerList(bool bShow)
{
	if( m_pServerListDlg ) m_pServerListDlg->Show( bShow );
}

#ifdef PRE_MOD_SELECT_CHAR
void CDnInterface::SetClassHairColor( int nClassIndex )
{
#ifdef _ADD_NEWLOGINUI
	if (m_pCharCreateSetupDlg)
		m_pCharCreateSetupDlg->SetClassHairColor(nClassIndex + 1);
#else
	if( m_pCharCreateDlg )
		m_pCharCreateDlg->SetClassHairColor( nClassIndex+1 );
#endif
}
#else // PRE_MOD_SELECT_CHAR
void CDnInterface::ShowCreateInfoList( bool bShow )
{
	for( int i=0; i<(int)m_pVecCreateInfoList.size(); i++ )
	{
		m_pVecCreateInfoList[i]->Show(bShow);
	}
}

void CDnInterface::ShowCreateInfoList( bool bShow, int nClassIndex )
{
	CEtUIDialog *pDialog = m_pVecCreateInfoList[nClassIndex];
	if( pDialog ) pDialog->Show( bShow );
	m_pCharCreateDlg->SetClassHairColor( nClassIndex+1 );
}
#endif // PRE_MOD_SELECT_CHAR

int CDnInterface::GetCharSetupSelectedServerIndex()
{
#ifdef _ADD_NEWLOGINUI
    if (m_pCharCreateSetupDlg)
        return m_pCharCreateSetupDlg->GetSelectedServerID();
#endif // _ADD_NEWLOGINUI

	return -1;
}

void CDnInterface::EnableCharSelectDlgControl( bool bEnable, const char *sControlName )
{
	CEtUIControl *pControl = m_pCharSelectDlg->GetControl( sControlName );
	if( pControl ) pControl->Enable( bEnable );
}

bool CDnInterface::IsEnableCharSelectDlgControl( const char *szControlName )
{
	CEtUIControl *pControl = m_pCharSelectDlg->GetControl( szControlName );
	if( pControl ) return pControl->IsEnable();
	return false;
}

void CDnInterface::EnableCharCreateBackDlgControl( bool bEnable )
{
#ifdef _ADD_NEWLOGINUI
	m_pCharCreateNameDlg->EnableCharCreateBackDlgControl(bEnable);
#else
	m_pCharCreateDlg->EnableCharCreateBackDlgControl( bEnable );
#endif
}

void CDnInterface::ShowServerSelectDlg( bool bShow )
{
	m_pServerListDlg->Show( bShow );
}

void CDnInterface::SetWaitUserProperty( LPCWSTR  wszServerName, UINT nWaitUserCount, ULONG nEstimateTime )
{	
	m_pWaitUserDlg->SetProperty( wszServerName, nWaitUserCount, nEstimateTime );
}

void CDnInterface::ShowWaitUserDlg( bool bShow )
{
	m_pWaitUserDlg->Show( bShow );
}

void CDnInterface::ShowCharCreateDlg( bool bShow )
{
#ifdef _ADD_NEWLOGINUI			
	m_pCharCreateSetupDlg->Show(true);
	m_pCharCreateSetupGestureDlg->Show(true);
	m_pCharCreateNameDlg->Show(true);
	m_pCharCreateSetupBackDlg->Show(true);
#else
	m_pCharCreateDlg->Show( bShow );
#endif
#ifndef PRE_MOD_SELECT_CHAR
	ShowCreateInfoList( bShow );
#endif // PRE_MOD_SELECT_CHAR
}

#ifdef PRE_ADD_CHARSELECT_PREVIEW_CASHITEM
void CDnInterface::ResetPreviewCostumeRadioButton()
{
	if(m_pCharSelectPreviewCostumeDlg)
		m_pCharSelectPreviewCostumeDlg->ResetRadioButton();
}

void CDnInterface::ShowPreviewCostumeDlg(bool bShow)
{
	if(m_pCharSelectPreviewCostumeDlg)
		m_pCharSelectPreviewCostumeDlg->Show(bShow);
}
#endif // PRE_ADD_CHARSELECT_PREVIEW_CASHITEM

#ifdef PRE_MOD_SELECT_CHAR

void CDnInterface::ShowCharCreateServerNameDlg( bool bShow )
{
	if( m_pCharCreateServerNameDlg )
		m_pCharCreateServerNameDlg->Show( bShow );
}

void CDnInterface::ShowCharLoginTitleDlg( bool bShow, int nTitleString )
{
	if( m_pCharLoginTitleDlg )
	{
		m_pCharLoginTitleDlg->Show( bShow );

		if( bShow )
			m_pCharLoginTitleDlg->SetTitle( nTitleString );
	}
}

#ifdef PRE_ADD_NEWCOMEBACK
void CDnInterface::SetComebackUser( bool bComeback )
{
	if( m_pCharRotateDlg )
			m_pCharRotateDlg->SetComebackUser( bComeback );
}
#endif // PRE_ADD_NEWCOMEBACK

#ifdef PRE_ADD_DWC
void CDnInterface::SetDWCCharSelect( bool bDWCSelect )
{
	if(m_pCharRotateDlg)
		m_pCharRotateDlg->SetDWCCharSelect(bDWCSelect);
}

void CDnInterface::OpenDwcTeamCreateDlg()
{
	if( GetDWCTask().HasDWCTeam() ) 
	{
		MessageBox(120232); // 대상은 이미 팀에 소속되어 있습니다.
		return;
	}

	if(m_pDWCTeamCreateDlg)
	{
		m_pDWCTeamCreateDlg->Show(true);
		OpenNpcTalkReturnDlg();
	}
}

void CDnInterface::CloseDwcTeamCreateDlg()
{
	if(m_pDWCTeamCreateDlg)
	{
		m_pDWCTeamCreateDlg->Show(false);
		CloseNpcTalkReturnDlg();
	}
}

bool CDnInterface::IsOpenDwcTeamCreateDlg()
{
	if(m_pDWCTeamCreateDlg)
	{
		return m_pDWCTeamCreateDlg->IsShow();
	}

	return false;
}

void CDnInterface::OpenDWCInviteReqDlg( LPCWSTR wszTitleName, LPCWSTR wszCharacterName, float fTime, int nID, CEtUICallback *pCall )
{
	if( !m_pDWCInviteReqDlg ) return;
	m_pDWCInviteReqDlg->SetInfo( wszTitleName, wszCharacterName, fTime, nID, pCall );
	m_pDWCInviteReqDlg->SetElapsedTime( fTime );
	m_pDWCInviteReqDlg->Show( true );
}

void CDnInterface::CloseDWCInviteReqDlg( bool bSendReject )
{
	if( !m_pDWCInviteReqDlg ) return;
	if( bSendReject ) m_pDWCInviteReqDlg->SendReject();
	m_pDWCInviteReqDlg->Show( false );
}

void CDnInterface::SetDWCRankBoardInfo(SCGetDWCRankPage* pInfo)
{
	if( m_pPvPRankDlg )
		m_pPvPRankDlg->SetListDWC( pInfo );
}

void CDnInterface::ShowDWCRankBoardInfo(bool bShow)
{
	if( m_pPvPRankDlg )
		m_pPvPRankDlg->Show(bShow);
}

void CDnInterface::SetFindDWCRankBoardInfo(SCGetDWCFindRank* pInfo)
{
	if( m_pPvPRankDlg )
		m_pPvPRankDlg->SetFindListDWC( pInfo );
}

#endif // PRE_ADD_DWC

void CDnInterface::ShowCharCreatePlayerInfo( bool bShow, int nJobClassID )
{
	if( m_pCharCreatePlayerInfo == NULL )
		return;

	if( bShow )
	{
		DNTableFileFormat* pDefaultSox = GetDNTable( CDnTableDB::TDEFAULTCREATE );
		if( pDefaultSox == NULL ) return;
		int nJobExplanationID; 
		std::string szFileName;
		//rlkt_dark
	/*	if (nJobClassID == 8)
		{
			nJobExplanationID = pDefaultSox->GetFieldFromLablePtr(nJobClassID + 2, "_ExplanationUIStringID")->GetInteger();
			szFileName = (char*)pDefaultSox->GetFieldFromLablePtr(nJobClassID + 2, "_ClassMovieFile")->GetString();
		}else{
			nJobExplanationID = pDefaultSox->GetFieldFromLablePtr(nJobClassID + 1, "_ExplanationUIStringID")->GetInteger();
			szFileName = (char*)pDefaultSox->GetFieldFromLablePtr(nJobClassID + 1, "_ClassMovieFile")->GetString();
		}*/

		nJobExplanationID = pDefaultSox->GetFieldFromLablePtr(nJobClassID + 1, "_ExplanationUIStringID")->GetInteger();
		szFileName = (char*)pDefaultSox->GetFieldFromLablePtr(nJobClassID + 1, "_ClassMovieFile")->GetString();
		m_pCharCreatePlayerInfo->Show( false );
		m_pCharCreatePlayerInfo->Show( true );
		int ClassType = pDefaultSox->GetFieldFromLablePtr(nJobClassID + 1, "_Type")->GetInteger();
		if (ClassType == 0)
		{
			m_pCharCreatePlayerInfo->SetJobID(nJobClassID, nJobExplanationID);
		}
		else if (ClassType == 1 || ClassType == 2) //Dark job + light job
		{
			int DarkJobID = pDefaultSox->GetFieldFromLablePtr(nJobClassID + 1, "_JobCode1")->GetInteger();
			m_pCharCreatePlayerInfo->SetJobID(DarkJobID, nJobExplanationID, true); //dark class :)
		}
		m_pCharCreatePlayerInfo->PlayMovie( szFileName );
	}
	else
	{
		m_pCharCreatePlayerInfo->Show( false );
	}
}

void CDnInterface::ShowCharCreateDarkDlg( bool bShow )
{
	if( m_pCharCreateDarkDlg )
		m_pCharCreateDarkDlg->Show( bShow );
}

void CDnInterface::ShowCharCreateSelectDlg( bool bShow )
{
	if( m_pCharCreateSelectDlg )
		m_pCharCreateSelectDlg->Show( bShow );
}

void CDnInterface::ResetCharSelectDlgButton()
{
	if( m_pCharCreateSelectDlg )
		m_pCharCreateSelectDlg->ResetButton();
}

bool CDnInterface::IsShowCharCreatePlayerInfo()
{
	if( m_pCharCreatePlayerInfo && m_pCharCreatePlayerInfo->IsShow() )
		return true;

	return false;
}

void CDnInterface::ShowCharCreateBackDlg( bool bShow )
{
	if( m_pCharCreateBackDlg )
		m_pCharCreateBackDlg->Show( bShow );
}

void CDnInterface::ShowCharRotateDlg( bool bShow )
{
	if( m_pCharRotateDlg )
		m_pCharRotateDlg->Show( bShow );
}

void CDnInterface::SetCharCreateSelect( int nClassIndex )
{
	if( m_pCharCreateSelectDlg )
		m_pCharCreateSelectDlg->SetCharCreateSelect( nClassIndex );
}

void CDnInterface::SetFocusCharIndex( int nClassIndex )
{
	if( m_pCharCreateSelectDlg )
		m_pCharCreateSelectDlg->SetFocusCharIndex( nClassIndex );
}

void CDnInterface::ShowCharGestureDlg( bool bShow, bool bCharCreate )
{
#ifdef _ADD_NEWLOGINUI
	if (m_pCharCreateSetupGestureDlg)
	{
	//	m_pCharCreateSetupGestureDlg->SetCharCreateDlg(bCharCreate);
		m_pCharCreateSetupGestureDlg->Show(bShow);
	}
#else
	if( m_pCharGestureDlg )
	{
		m_pCharGestureDlg->SetCharCreateDlg( bCharCreate );
		m_pCharGestureDlg->Show( bShow );
	}
#endif
}	

void CDnInterface::ShowCharOldGestureDlg(bool bShow, bool bCharCreate)
{
	if( m_pCharGestureDlg )
	{
		m_pCharGestureDlg->SetCharCreateDlg( bCharCreate );
		m_pCharGestureDlg->Show( bShow );
	}
}	
#else // PRE_MOD_SELECT_CHAR

void CDnInterface::ShowSlotInfoDlg( bool bShow, BYTE CharIndex, DnActorHandle hActor, LPCTSTR szMapName )
{
	if( !m_pSlotInfoDlg[CharIndex] ) return;
	static EtVector2 vInfoPos[4] = 
	{ 
		EtVector2( 0.25f, 0.2f ),  
		EtVector2( 0.50f, 0.21f ), 
		EtVector2( 0.00f, 0.18f ), 
		EtVector2( 0.75f, 0.19f ) 
	};

	m_pSlotInfoDlg[CharIndex]->Show(bShow);
	if( !hActor ) return;
	m_pSlotInfoDlg[CharIndex]->SetInfomation( hActor->GetName(), szMapName, hActor->GetLevel(), ((CDnPlayerActor*)hActor.GetPointer())->GetJobClassID() );

	SUICoord Coord;
	m_pSlotInfoDlg[CharIndex]->GetDlgCoord( Coord );
	Coord.fX = vInfoPos[CharIndex].x;
	Coord.fY = vInfoPos[CharIndex].y;
	m_pSlotInfoDlg[CharIndex]->SetDlgCoord( Coord );
}

void CDnInterface::ShowSlotInfoDlg( bool bShow )
{
	for( int i=0; i<SHOWCHARNUMPERONEPAGE; i++ ) {
		if( m_pSlotInfoDlg[i] )
			m_pSlotInfoDlg[i]->Show(bShow);
	}
}

void CDnInterface::ShowSlotDeleteWaitDlg( bool bShow )
{
	for( int i=0; i<SHOWCHARNUMPERONEPAGE; i++ ) {
		if( m_pSlotDeleteWaitDlg[i] )
			m_pSlotDeleteWaitDlg[i]->Show(bShow);
	}
}

void CDnInterface::ShowSlotDeleteWaitDlg( bool bShow, BYTE CharIndex, __time64_t RemainTime )
{
	if( !m_pSlotDeleteWaitDlg[CharIndex] ) return;
	static EtVector2 vInfoPos[4] = 
	{ 
		EtVector2( 0.30f, 0.7f ),  
		EtVector2( 0.55f, 0.71f ), 
		EtVector2( 0.05f, 0.68f ), 
		EtVector2( 0.80f, 0.69f ) 
	};

	m_pSlotDeleteWaitDlg[CharIndex]->Show( bShow );
	m_pSlotDeleteWaitDlg[CharIndex]->SetRemainTime( RemainTime );

	SUICoord Coord;
	m_pSlotDeleteWaitDlg[CharIndex]->GetDlgCoord( Coord );
	Coord.fX = vInfoPos[CharIndex].x;
	Coord.fY = vInfoPos[CharIndex].y;
	m_pSlotDeleteWaitDlg[CharIndex]->SetDlgCoord( Coord );
}

void CDnInterface::ShowSlotBlockDlg( bool bShow )
{
	for( int i=0; i<SHOWCHARNUMPERONEPAGE; i++ )
	{
		if( m_pSlotDeleteWaitDlg[i] )
			m_pSlotDeleteWaitDlg[i]->ShowSlotBlock( bShow );
	}
}

void CDnInterface::ShowSlotBlockDlg( bool bShow, BYTE CharIndex )
{
	if( !m_pSlotDeleteWaitDlg[CharIndex] ) return;
	static EtVector2 vInfoPos[4] = 
	{ 
		EtVector2( 0.30f, 0.7f ),  
		EtVector2( 0.55f, 0.71f ), 
		EtVector2( 0.05f, 0.68f ), 
		EtVector2( 0.80f, 0.69f ) 
	};

	m_pSlotDeleteWaitDlg[CharIndex]->ShowSlotBlock( bShow );

	SUICoord Coord;
	m_pSlotDeleteWaitDlg[CharIndex]->GetDlgCoord( Coord );
	Coord.fX = vInfoPos[CharIndex].x;
	Coord.fY = vInfoPos[CharIndex].y;
	m_pSlotDeleteWaitDlg[CharIndex]->SetDlgCoord( Coord );
}

#endif // PRE_MOD_SELECT_CHAR

void CDnInterface::ShowCashShopDlg(bool bShow)
{
#ifdef PRE_REMOVE_CASHSHOP
	return;
#endif

	if (m_pCashShopDlg == NULL)
	{
		_ASSERT(0);
		return;
	}

	if ((bShow && m_pCashShopDlg->IsShow()) ||
		(bShow == false && m_pCashShopDlg->IsShow() == false)
		)
		return;

	if (bShow)
	{
		if (GetChatDialog())
		{
			GetChatDialog()->ShowEx(false);
		}

		CEtUIDialogBase::CloseAllDialog();
		CloseNpcDialog();
		CDnLocalPlayerActor::LockInput( true );
		FadeDialog( 0x00000000, 0xff000000, CGlobalInfo::GetInstance().m_fFadeDelta, m_pCashShopDlg );
#ifdef PRE_REMOVE_CASHSHOP
#else
		if (m_pCashShopMenuDlg)
			m_pCashShopMenuDlg->Show(!bShow);
#endif
	}
	else
	{
		CDnLocalPlayerActor::LockInput( false );
		FadeDialog( 0x00000000, 0xff000000, CGlobalInfo::GetInstance().m_fFadeDelta, m_pCashShopDlg );
	}
}

bool CDnInterface::OpenAcceptDialog( LPCWSTR wszMessage, float fTotalTime, float fElapsedTime, int nID, CEtUICallback *pCall )
{
	ASSERT( m_pAcceptDlg&&"CDnInterface::OpenAcceptDialog, m_pAcceptDlg is NULL!" );

	if( m_pAcceptDlg->IsShow() )
		return false;

	m_pAcceptDlg->SetInfo( wszMessage, fTotalTime, fElapsedTime, nID, pCall );
	m_pAcceptDlg->Show(true);

	return true;
}

bool CDnInterface::OpenAcceptDialog( int nMsgIndex, float fTotalTime, float fElapsedTime, int nID, CEtUICallback *pCall )
{
	return OpenAcceptDialog( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nMsgIndex ), fTotalTime, fElapsedTime, nID, pCall );
}

void CDnInterface::CloseAcceptDialog()
{
	if( !m_pAcceptDlg ) return;
	m_pAcceptDlg->Show(false);
}

bool CDnInterface::IsOpenAcceptDialog()
{
	if( !m_pAcceptDlg ) return false;
	return m_pAcceptDlg->IsShow();
}

bool CDnInterface::OpenPrivateMarketReqDialog( LPCWSTR wszMessage, float fTime, int nID, CEtUICallback *pCall )
{
	ASSERT( m_pPrivateMarketReqDlg&&"CDnInterface::OpenAcceptDialog, m_pAcceptDlg is NULL!" );

	if( m_pPrivateMarketReqDlg->IsShow() )
		return false;

	m_pPrivateMarketReqDlg->SetInfo( wszMessage, fTime, nID, pCall );
	m_pPrivateMarketReqDlg->Show(true);

	return true;
}

void CDnInterface::ClosePrivateMarketReqDialog()
{
	if( !m_pPrivateMarketReqDlg ) return;
	m_pPrivateMarketReqDlg->Show(false);
}

bool CDnInterface::IsOpenPrivateMarketReqDialog()
{
	if( !m_pPrivateMarketReqDlg ) return false;
	return m_pPrivateMarketReqDlg->IsShow();
}

void CDnInterface::OpenAcceptRequestDialog( LPCWSTR wszMessage, float fTotalTime, float fRemainTime, bool bAccept, int type, int nID, CEtUICallback *pCall )
{
	ASSERT( m_pAcceptRequestDlg&&"CDnInterface::OpenAcceptRequestDialog, m_pAcceptRequestDlg is NULL!" );

	// 중복 열림에 대한 처리는 각각의 여는 프로세스(개인거래 태스크, 파티 태스크 등)에 맡긴다.
	//if( m_pAcceptRequestDlg->IsShow() )
	//	return false;

	m_pAcceptRequestDlg->SetInfo( wszMessage, fTotalTime, bAccept, (CDnAcceptRequestDlg::eAcceptRequestType)type, nID, pCall );
	m_pAcceptRequestDlg->SetElapsedTime( fRemainTime );
	m_pAcceptRequestDlg->Show(true);
}

void CDnInterface::CloseAcceptRequestDialog(bool bReject /*= false*/)
{
	if (!m_pAcceptRequestDlg || m_pAcceptRequestDlg->IsShow() == false)
		return;

	if (bReject)
		m_pAcceptRequestDlg->OnTimeIsUp();
	m_pAcceptRequestDlg->Show(false);

	CEtUIDialog* pDnNotifierDialog = GetInterface().GetMainMenuDialog( CDnMainMenuDlg::NOTIFIER_DIALOG );
	if( pDnNotifierDialog )
		pDnNotifierDialog->Show( true );
}

#ifdef PRE_ADD_QUICK_PVP
void CDnInterface::TimeUpAcceptRequestDialog()
{
	if (!m_pAcceptRequestDlg || m_pAcceptRequestDlg->IsShow() == false)
		return;

	m_pAcceptRequestDlg->OnTimeIsUp();
}
#endif

bool CDnInterface::IsOpenAcceptRequestDialog()
{
	if( !m_pAcceptRequestDlg ) return false;
	return m_pAcceptRequestDlg->IsShow();
}

void CDnInterface::OpenProgressDialog(LPCWSTR wszMessage, float fTotalTime, float fRemainTime, bool bManualTimeCtrl, bool bManualClose, int nID, CEtUICallback *pCall)
{
	if (m_pProgressDlg)
	{
		m_pProgressDlg->SetInfo( wszMessage, fTotalTime, fRemainTime, bManualTimeCtrl, bManualClose, nID, pCall );
		m_pProgressDlg->Show(true);
	}
	else
	{
		_ASSERT(0);
	}
}

bool CDnInterface::IsOpenProgressDialog()
{
	if( !m_pProgressDlg ) return false;
	return m_pProgressDlg->IsShow();
}

void CDnInterface::ForceCloseProgressDialog()
{
	if (m_pProgressDlg)
	{
		m_pProgressDlg->SetManualClose(false);
		m_pProgressDlg->Show(false);
	}
}

void CDnInterface::ProgressMBox( LPCWSTR wszMessage, float fTotalTime, float fRemainTime, bool bManual, int nID, CEtUICallback *pCall )
{
	if (m_pProgressMBox)
	{
		m_pProgressMBox->SetMessageBox( wszMessage, fTotalTime, fRemainTime, bManual, nID, pCall );
		m_pProgressMBox->Show(true);
	}
	else 
	{
		_ASSERT(0);
	}
}

bool CDnInterface::IsOpenProgressMBox()
{
	if( !m_pProgressMBox ) return false;
	return m_pProgressMBox->IsShow();
}

void CDnInterface::SetProgressMBoxCallback(CEtUICallback* pCall)
{
	m_pProgressMBox->SetCallback(pCall);
}

void CDnInterface::MessageBox( int nMsgIndex, UINT uType, int nID, CEtUICallback *pCall, bool bTopMost, bool bSmartMoveNoButton, bool bNoBtn, bool bNoOverwrite )
{
	std::wstring strMsg = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nMsgIndex );

#ifndef _FINAL_BUILD
	if( GetAsyncKeyState( VK_LSHIFT )&0x8000 || CGlobalValue::GetInstance().m_bMessageBoxWithMsgID )
	{
		std::wstring strTemp;
		strTemp = FormatW( L"\nClientMsgID : %d", nMsgIndex );
		strMsg.append( strTemp.c_str() );

		WCHAR wszIndex[32] = {0,};
		swprintf_s( wszIndex, _countof(wszIndex), L"%d", nMsgIndex );
		CEtUILineEditBox::CopyStringToClipboard( wszIndex );
		CGlobalValue::GetInstance().m_bMessageBoxWithMsgID = false;
	}
#endif

	MessageBox( strMsg.c_str(), uType, nID, pCall, bTopMost, bSmartMoveNoButton, bNoBtn, bNoOverwrite );
}

void CDnInterface::MessageBox( LPCWSTR pwszMessage, UINT uType, int nID, CEtUICallback *pCall, bool bTopMost, bool bSmartMoveNoButton, bool bNoBtn, bool bNoOverwrite )
{
	m_pMessageBox->SetMessageBox( m_Type, pwszMessage, uType, nID, pCall, bTopMost, bSmartMoveNoButton, bNoBtn, bNoOverwrite );
	m_pMessageBox->Show(true);
}

void CDnInterface::MiddleMessageBox( int nMsgIndex, UINT uType, int nID, CEtUICallback *pCall, bool bTopMost, bool bSmartMoveNoButton, bool bNoBtn, bool bNoOverwrite )
{
	MiddleMessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nMsgIndex ), uType, nID, pCall, bTopMost, bSmartMoveNoButton, bNoBtn, bNoOverwrite );
}

void CDnInterface::MiddleMessageBox( LPCWSTR pwszMessage, UINT uType, int nID, CEtUICallback *pCall, bool bTopMost, bool bSmartMoveNoButton, bool bNoBtn, bool bNoOverwrite )
{
	m_pMiddleMessageBox->SetMessageBox( m_Type, pwszMessage, uType, nID, pCall, bTopMost, bSmartMoveNoButton, bNoBtn, bNoOverwrite );
	m_pMiddleMessageBox->Show( true );
}

void CDnInterface::MessageBoxTextBox( LPCWSTR pwszMessage, UINT uType, int nID, CEtUICallback *pCall, bool bTopMost, bool bSmartMoveNoButton, bool bNoBtn, bool bNoOverwrite )
{
	m_pMessageBoxTextBox->SetMessageBox( m_Type, pwszMessage, uType, nID, pCall, bTopMost, bSmartMoveNoButton, bNoBtn, bNoOverwrite );
	m_pMessageBoxTextBox->Show(true);
}

void CDnInterface::BigMessageBox( int nMsgIndex, UINT uType, int nID, CEtUICallback *pCall, bool bTopMost, bool bSmartMoveNoButton, bool bNoBtn, bool bNoOverwrite )
{
	BigMessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nMsgIndex ), uType, nID, pCall, bTopMost, bSmartMoveNoButton, bNoBtn, bNoOverwrite );
}

void CDnInterface::BigMessageBox( LPCWSTR pwszMessage, UINT uType, int nID, CEtUICallback *pCall, bool bTopMost, bool bSmartMoveNoButton, bool bNoBtn, bool bNoOverwrite )
{
	m_pBigMessageBox->SetMessageBox( m_Type, pwszMessage, uType, nID, pCall, bTopMost, bSmartMoveNoButton, bNoBtn, bNoOverwrite );
	m_pBigMessageBox->Show(true);
}

void CDnInterface::GetServerMessage(std::wstring& msg, int nMsgIndex) const
{
	msg = GetEtUIXML().GetUIString( CEtUIXML::idCategory2, nMsgIndex );
	if( msg.empty() )
	{
		wchar_t wszTemp[128] = {0};
		swprintf_s( wszTemp, _countof(wszTemp), L"[%d]", nMsgIndex );
		msg = wszTemp;
	}
}

void CDnInterface::ServerMessageBox( int nMsgIndex, UINT uType, int nID, CEtUICallback *pCall, bool bTopMost, bool bSmartMoveNoButton, bool bNoBtn, bool bNoOverwrite )
{
	std::wstring strMsg = GetEtUIXML().GetUIString( CEtUIXML::idCategory2, nMsgIndex );
	if( strMsg.empty() )
	{
		wchar_t wszTemp[128] = {0};
		swprintf_s( wszTemp, _countof(wszTemp), L"[%d]", nMsgIndex );
		strMsg = wszTemp;
	}

#ifndef _FINAL_BUILD
	if( GetAsyncKeyState( VK_LSHIFT )&0x8000 || CGlobalValue::GetInstance().m_bMessageBoxWithMsgID )
	{
		std::wstring strTemp;
		strTemp = FormatW( L"\nServerMsgID : %d", nMsgIndex );
		strMsg.append( strTemp.c_str() );

		WCHAR wszIndex[32] = {0,};
		swprintf_s( wszIndex, _countof(wszIndex), L"%d", nMsgIndex );
		CEtUILineEditBox::CopyStringToClipboard( wszIndex );
		CGlobalValue::GetInstance().m_bMessageBoxWithMsgID = false;
	}
#endif

	MessageBox( strMsg.c_str(), uType, nID, pCall, bTopMost, bSmartMoveNoButton, bNoBtn, bNoOverwrite );
}

void CDnInterface::CloseMessageBox()
{
	if( m_pMessageBox )
		m_pMessageBox->Show( false );
	if( m_pMessageBoxTextBox )
		m_pMessageBoxTextBox->Show( false );
	if( m_pBigMessageBox )
		m_pBigMessageBox->Show( false );
	if( m_pMiddleMessageBox )
		m_pMiddleMessageBox->Show( false );
}

void CDnInterface::OpenNpcDialog( CEtUICallback *pCallback )
{
	// Note : 이미 창고나 상점이 열려있으면 그 뒤에 오는 NPC대화는 무시한다.
	//
	if( m_pMainMenuDlg )
	{
		if( m_pMainMenuDlg->IsOpenDialog(CDnMainMenuDlg::PLAYER_GUILD_STORAGE_DIALOG) )	return;
		if( m_pMainMenuDlg->IsOpenDialog(CDnMainMenuDlg::STORE_DIALOG) )			return;
		if( m_pMainMenuDlg->IsOpenDialog(CDnMainMenuDlg::SKILL_STORE_DIALOG) )		return;
		if( m_pMainMenuDlg->IsOpenDialog(CDnMainMenuDlg::MARKET_DIALOG) )			return;
		if( m_pMainMenuDlg->IsOpenDialog(CDnMainMenuDlg::MAIL_DIALOG) )				return;
		if( m_pMainMenuDlg->IsOpenDialog(CDnMainMenuDlg::ITEM_DISJOINT_DIALOG) )	return;
		if( m_pMainMenuDlg->IsOpenDialog(CDnMainMenuDlg::PLATE_MAIN_DIALOG) )return;
		if( m_pMainMenuDlg->IsOpenDialog(CDnMainMenuDlg::ITEM_UPGRADE_DIALOG) )		return;
		if( m_pMainMenuDlg->IsOpenDialog(CDnMainMenuDlg::ITEM_COMPOUND_DIALOG) )	return;
		if( m_pMainMenuDlg->IsWithDrawGlyphOpen() )	return;
#ifdef PRE_ADD_DONATION
		if( m_pMainMenuDlg->IsOpenDialog(CDnMainMenuDlg::CONTRIBUTION_DIALOG) )		return;
#endif
		CDnSkillTreeDlg* pSkillTreeDlg = static_cast<CDnSkillTreeDlg*>( GetInterface().GetMainMenuDialog( CDnMainMenuDlg::SKILL_DIALOG ) );
		if(pSkillTreeDlg && pSkillTreeDlg->IsShow() && pSkillTreeDlg->IsUnlockByMoneyMode())
			return;
	}
	if( m_pStageClearReportDlg && m_pStageClearReportDlg->IsShow() ) return;
	if( m_pStageClearDlg && m_pStageClearDlg->IsShow() ) return;

#ifdef PRE_ADD_GACHA_JAPAN
	if( m_pGachaDlg && m_pGachaDlg->IsShow() ) return;
#endif

	// NPC관련 길드창이 열려있어도 마찬가지
	if( IsShowGuildMgrBox() ) return;

	// DWC Team 생성창
#ifdef PRE_ADD_DWC
	if( IsOpenDwcTeamCreateDlg() ) return;
#endif // PRE_ADD_DWC

	// 그 외 블라인드된 상태로 열리는 창들..
#ifdef PRE_ADD_GACHA_JAPAN
	if( m_pGachaDlg && m_pGachaDlg->IsShow() )	return;
#endif // PRE_ADD_GACHA_JAPAN

#ifdef PRE_FIX_49403
	// #49403 특수 순서에서, 돈을 소모했음에도 불구하고, 비행선을 탈 수 없다.
	// 현재 거래요청or수락 상태 이면 거래취소패킷 전송.
	CDnTradePrivateMarket & rPrivateMarket = GetTradeTask().GetTradePrivateMarket();	
#ifdef PRE_FIX_CANCELTRADE
	rPrivateMarket.ClearTradeUserInfoList();
#else
	// 거래중이면 거래취소.
	if( rPrivateMarket.GetCurTradeUserID() > 0 )
		GetTradeTask().GetTradePrivateMarket().RequestPrivateMarketCancel(); // 거래수락취소.

	// 거래 요청중 이면 요청취소.
	else if( rPrivateMarket.GetTradePlayerSessionID() > 0 )
		GetTradeTask().GetTradePrivateMarket().SendExchangeRequestCancel( rPrivateMarket.GetTradePlayerSessionID() ); // 거래요청취소.	
#endif // PRE_FIX_CANCELTRADE

#endif
	GetMasterTask().CloseAcceptDlg();
	ShowMiniMap( false );
	CEtUIDialogBase::CloseAllDialog();

	// 사용자 인풋에 의해 npc창이 열릴때 HideNextBlindOpened를 강제로 해제시킨다. 
	m_pNpcDlg->HideNextBlindOpened(false);
	m_pNpcDlg->SetCallback( pCallback );
//	if( m_pBlindDlg->IsShow() ) return;
	m_pBlindDlg->SetBillBoardColor( 0x00000000 );
	m_pBlindDlg->AddBlindCallBack( m_pNpcDlg );
	m_pBlindDlg->AddBlindCallBack( ((CDnLocalPlayerActor*)CDnActor::s_hLocalActor.GetPointer()) );
	m_pBlindDlg->Show(true);

	m_pChatDlg->ShowEx(false);

#ifdef PRE_ADD_MAINQUEST_UI
	m_pMainQuestDlg->SetOpenMainQuestDlgFlag(false);
#endif
}

#if defined(PRE_ADD_MAILBOX_OPEN) || defined(PRE_ADD_68286)
void CDnInterface::OpenMailDialogByShortCutKey()
{
	// Note : 이미 창고나 상점이 열려있으면 그 뒤에 오는 NPC대화는 무시한다.
	//
	if( m_pMainMenuDlg )
	{
		if( m_pMainMenuDlg->IsOpenDialog(CDnMainMenuDlg::PLAYER_GUILD_STORAGE_DIALOG) )	return;
		if( m_pMainMenuDlg->IsOpenDialog(CDnMainMenuDlg::STORE_DIALOG) )			return;
		if( m_pMainMenuDlg->IsOpenDialog(CDnMainMenuDlg::SKILL_STORE_DIALOG) )		return;
		if( m_pMainMenuDlg->IsOpenDialog(CDnMainMenuDlg::MARKET_DIALOG) )			return;
		if( m_pMainMenuDlg->IsOpenDialog(CDnMainMenuDlg::MAIL_DIALOG) )				return;
		if( m_pMainMenuDlg->IsOpenDialog(CDnMainMenuDlg::ITEM_DISJOINT_DIALOG) )	return;
		if( m_pMainMenuDlg->IsOpenDialog(CDnMainMenuDlg::PLATE_MAIN_DIALOG) )return;
		if( m_pMainMenuDlg->IsOpenDialog(CDnMainMenuDlg::ITEM_UPGRADE_DIALOG) )		return;
		if( m_pMainMenuDlg->IsOpenDialog(CDnMainMenuDlg::ITEM_COMPOUND_DIALOG) )	return;

		if( m_pMainMenuDlg->IsWithDrawGlyphOpen() )	return;

#ifdef PRE_ADD_DONATION
		if( m_pMainMenuDlg->IsOpenDialog(CDnMainMenuDlg::CONTRIBUTION_DIALOG) )		return;
#endif

		CDnSkillTreeDlg* pSkillTreeDlg = static_cast<CDnSkillTreeDlg*>( GetInterface().GetMainMenuDialog( CDnMainMenuDlg::SKILL_DIALOG ) );
		if( pSkillTreeDlg && pSkillTreeDlg->IsShow() && pSkillTreeDlg->IsUnlockByMoneyMode() )
			return;
	}
	if( m_pStageClearReportDlg && m_pStageClearReportDlg->IsShow() )	return;
	if( m_pStageClearDlg && m_pStageClearDlg->IsShow() )			return;

#ifdef PRE_ADD_GACHA_JAPAN
	if( m_pGachaDlg && m_pGachaDlg->IsShow() )			return;
#endif

	// NPC관련 길드창이 열려있어도 마찬가지
	if( IsShowGuildMgrBox() ) return;

	// DWC Team 생성창
#ifdef PRE_ADD_DWC
	if( IsOpenDwcTeamCreateDlg() ) return;
#endif // PRE_ADD_DWC

	// 그 외 블라인드된 상태로 열리는 창들..
#ifdef PRE_ADD_GACHA_JAPAN
	if( m_pGachaDlg && m_pGachaDlg->IsShow() )	return;
#endif // PRE_ADD_GACHA_JAPAN

#ifdef PRE_FIX_49403
	// #49403 특수 순서에서, 돈을 소모했음에도 불구하고, 비행선을 탈 수 없다.
	// 현재 거래요청or수락 상태 이면 거래취소패킷 전송.
	CDnTradePrivateMarket & rPrivateMarket = GetTradeTask().GetTradePrivateMarket();	

	
	//#69850
	//거래중 우편함 알림 아이콘으로 우편함을 열때, 거래 취소를 보내고, 우편함을 열면, 인벤창이 닫힘, 우편함도 비정상 동작.
	//거래중일 경우 우편함 열리지 않도록함.
	if (rPrivateMarket.GetCurTradeUserID() != 0)
		return;

// #ifdef PRE_FIX_CANCELTRADE
// 	rPrivateMarket.ClearTradeUserInfoList();
// #else
// 	// 거래중이면 거래취소.
// 	if( rPrivateMarket.GetCurTradeUserID() > 0 )
// 		GetTradeTask().GetTradePrivateMarket().RequestPrivateMarketCancel();	 // 거래수락취소.
// 
// 	// 거래 요청중 이면 요청취소.
// 	else if( rPrivateMarket.GetTradePlayerSessionID() > 0 )
// 		GetTradeTask().GetTradePrivateMarket().SendExchangeRequestCancel( rPrivateMarket.GetTradePlayerSessionID() ); // 거래요청취소.	
// #endif // PRE_FIX_CANCELTRADE

#endif
	GetMasterTask().CloseAcceptDlg();
	ShowMiniMap( false );
	CEtUIDialogBase::CloseAllDialog();

	// 사용자 인풋에 의해 npc창이 열릴때 HideNextBlindOpened를 강제로 해제시킨다. 
	m_pNpcDlg->HideNextBlindOpened(false);
	//m_pNpcDlg->SetCallback( pCallback );
	//	if( m_pBlindDlg->IsShow() ) return;
	m_pBlindDlg->SetBillBoardColor( 0x00000000 );
	
	//여기서 추가 되면 NPC대화창이 활성화 되버린다.
	//여기 등록은 안 하고, CDnNpcDlg::OnBlindOpen() 에서 호출하는 함수들 호출해준다.
	m_pNpcDlg->SetSkipOpen(true);
	m_pBlindDlg->AddBlindCallBack( m_pNpcDlg );	
	if (CDnActor::s_hLocalActor)
		m_pBlindDlg->AddBlindCallBack( ((CDnLocalPlayerActor*)CDnActor::s_hLocalActor.GetPointer()) );
	m_pBlindDlg->Show(true);

	m_pChatDlg->ShowEx(false);

	//NPCTalkReturnDlg 예외 처리 설정..
	m_pNpcTalkReturnDlg->SetSkipOpen(true);

	//////////////////////////////////////////////////////////////////////////
	CDnLocalPlayerActor::SetTakeNpcUID(0xffffffff);
	OpenMailDialog(false, _T(""), _T(""));

	//여기서 머리위 아이콘 표시하고, 서버로 패킷 전송...
	if (CDnActor::s_hLocalActor)
		((CDnLocalPlayerActor*)CDnActor::s_hLocalActor.GetPointer())->OpenMailBoxByShortCutKey();
}
#endif // PRE_ADD_MAILBOX_OPEN

// NPC 대화 끝나면 이 함수 호출된다.
void CDnInterface::CloseNpcDialog()
{
	if( m_pNpcDlg ) 
	{
		m_pNpcDlg->Close();
	}

	// Note : 창고나 상점등이 열릴때는 레터박스를 유지한다.
	//
	if( m_pMainMenuDlg )
	{
		if( m_pMainMenuDlg->IsOpenDialog(CDnMainMenuDlg::PLAYER_GUILD_STORAGE_DIALOG) )	return;
		if( m_pMainMenuDlg->IsOpenDialog(CDnMainMenuDlg::STORE_DIALOG) )			return;
		if( m_pMainMenuDlg->IsOpenDialog(CDnMainMenuDlg::SKILL_STORE_DIALOG) )		return;
		if( m_pMainMenuDlg->IsOpenDialog(CDnMainMenuDlg::MARKET_DIALOG) )			return;
		if( m_pMainMenuDlg->IsOpenDialog(CDnMainMenuDlg::MAIL_DIALOG) )				return;
		if( m_pMainMenuDlg->IsOpenDialog(CDnMainMenuDlg::ITEM_DISJOINT_DIALOG) )	return;
		if( m_pMainMenuDlg->IsOpenDialog(CDnMainMenuDlg::PLATE_MAIN_DIALOG) )		return;
		if( m_pMainMenuDlg->IsOpenDialog(CDnMainMenuDlg::ITEM_UPGRADE_DIALOG) )		return;
		if( m_pMainMenuDlg->IsOpenDialog(CDnMainMenuDlg::ITEM_COMPOUND_DIALOG) )	return;
		if( m_pMainMenuDlg->IsOpenDialog(CDnMainMenuDlg::ITEM_COMPOUND2_DIALOG) )	return;
		if( m_pMainMenuDlg->IsOpenDialog(CDnMainMenuDlg::LIFESTORAGE_DIALOG) )		return;
#ifdef PRE_ADD_DONATION
		if( m_pMainMenuDlg->IsOpenDialog(CDnMainMenuDlg::CONTRIBUTION_DIALOG) )		return;
#endif
		if( m_pMainMenuDlg->IsOpenDialog(CDnMainMenuDlg::INVENTORY_DIALOG) )		return;
		if( m_pMainMenuDlg->IsWithDrawGlyphOpen() )									return;
		CDnSkillTreeDlg* pSkillTreeDlg = static_cast<CDnSkillTreeDlg*>( GetInterface().GetMainMenuDialog( CDnMainMenuDlg::SKILL_DIALOG ) );
		if(pSkillTreeDlg && pSkillTreeDlg->IsShow() && pSkillTreeDlg->IsUnlockByMoneyMode()) return;
	}

	if (CDnCashShopTask::GetInstance().IsOpenGeneralBrowser()) return;

	// NPC관련 길드창이 열려있어도 마찬가지
	if( IsShowGuildMgrBox() ) return;

	// DWC Team 생성창
#ifdef PRE_ADD_DWC
	if( IsOpenDwcTeamCreateDlg() ) return;
#endif // PRE_ADD_DWC

	// 그 외 블라인드된 상태로 열리는 창들..
#ifdef PRE_ADD_GACHA_JAPAN
	if( m_pGachaDlg && m_pGachaDlg->IsShow() )	return;
#endif // PRE_ADD_GACHA_JAPAN

	if( m_pDarkLairRankBoardDlg && ( m_pDarkLairRankBoardDlg->IsRankInfoRequest() || m_pDarkLairRankBoardDlg->IsShow() ) ) return;
	if( m_pPVPLadderRankBoardDlg && (m_pPVPLadderRankBoardDlg->IsRankInfoRequest() || m_pPVPLadderRankBoardDlg->IsShow() ) ) return;

	if( m_pGuildWarPreliminaryStatusDlg && m_pGuildWarPreliminaryStatusDlg->IsShow() )		return;
	if( m_pGuildWarPreliminaryResultDlg && m_pGuildWarPreliminaryResultDlg->IsShow() )		return;
	if( m_pGuildWarFinalTournamentListDlg && m_pGuildWarFinalTournamentListDlg->IsShow() )	return;

	if( m_bOpenFarmChannel ) return;


	if( m_pBlindDlg )
	{
		m_pBlindDlg->Show(false);
	}

#ifdef PRE_ADD_QUEST_BACK_DIALOG_BUTTON
	GetInterface().CloseNpcTalkReturnDlg();
#endif

#ifdef PRE_ADD_STAMPSYSTEM
	if( GetInterface().GetInterfaceType() == CDnInterface::Village )
		GetInterface().AddStamp(false, true);
#endif // PRE_ADD_STAMPSYSTEM

}

void CDnInterface::ShowGateQuestionDialog( bool bShow, std::wstring &strGateName, CEtUICallback *pCallback )
{
	if( !m_pGateQuestionDlg ) return;
	CloseMessageBox();

	m_pGateQuestionDlg->SetCallback( pCallback );
	m_pGateQuestionDlg->SetTitleName( strGateName.c_str() );
	m_pGateQuestionDlg->Show(bShow);
}

void CDnInterface::ShowWorldZoneSelectDialog( bool bShow, const BYTE cGateNo, CEtUICallback *pCallback )
{
	if( NULL == m_pWorldZoneSelectDlg )
		return;

	CloseMessageBox();

	m_pWorldZoneSelectDlg->SetCallback( pCallback );
	m_pWorldZoneSelectDlg->SetMapInfo( cGateNo );
	m_pWorldZoneSelectDlg->Show( bShow );
}

bool CDnInterface::IsShowWorldZoneSelectDialog()
{
	if( NULL == m_pWorldZoneSelectDlg )
		return false;

	return m_pWorldZoneSelectDlg->IsShow();
}

BYTE CDnInterface::GetWorldZoneSelectIndex()
{
	if( NULL == m_pWorldZoneSelectDlg )
		return 0;

	return m_pWorldZoneSelectDlg->GetSelectIndex();
}

void CDnInterface::SetWorldZoneSelectIndex( const BYTE cSelectIndex )
{
	if( NULL == m_pWorldZoneSelectDlg )
		return;

	m_pWorldZoneSelectDlg->SetSelectIndex( cSelectIndex );
}

bool CDnInterface::IsFarm()
{
	CDnGameTask * pGameTask = (CDnGameTask *)CTaskManager::GetInstance().GetTask( "GameTask" );

	if( pGameTask && pGameTask->GetGameTaskType() == GameTaskType::Farm )
		return true;

	return false;
}

void CDnInterface::ShowFarmGateQuestionDialog( bool bShow, SCFarmInfo * pFarmInfo, CEtUICallback *pCallback )
{
	if( !m_pLifeChannelDlg ) return;
	CloseMessageBox();

	if( pFarmInfo )
		m_pLifeChannelDlg->AddChannelList(pFarmInfo);
	m_pLifeChannelDlg->SetCallback( pCallback );
	m_pLifeChannelDlg->Show( bShow );
}

void CDnInterface::UpdateFarmPlantedInfo( SCFarmPlantedInfo * pPlantedInfo )
{
	if( !m_pLifeChannelDlg ) return;

	if( pPlantedInfo )
		m_pLifeChannelDlg->UpdateFarmPlantedInfo( pPlantedInfo );
}

void CDnInterface::RefreshLifeStorage( SCFarmWareHouseList * pData)
{
	if( m_pMainMenuDlg )
		m_pMainMenuDlg->RefreshLifeStorage(pData);
}

void CDnInterface::RequestTakeWareHouseItem( int nSlotIndex )
{
	if( m_pMainMenuDlg )
		m_pMainMenuDlg->RequestTakeWareHouseItem( nSlotIndex );
}

void CDnInterface::OnRecvTakeWareHouseItem(INT64 biUniqueID)
{
	if( m_pMainMenuDlg )
		m_pMainMenuDlg->OnRecvTakeWareHouseItem(biUniqueID);
}

void CDnInterface::SetFarmTooltip( CDnLifeSkillPlantTask::SPlantTooltip & info )
{
	m_pLifeTooltipDlg->SetInfo( info );
	m_pLifeTooltipDlg->AddRenderCount();
}

void CDnInterface::ChangeState(int nAreaIndex, int nItemID, int eAreaState, float fTime, float fWater, char * szHarvestIconName)
{
	m_pLifeConditionDlg->ChangeState( nAreaIndex, nItemID, eAreaState, fTime, fWater, szHarvestIconName );
	m_pMainMenuDlg->RefreshZoneMapFarmPlant();
}

void CDnInterface::FarmConditionRefresh()
{
	m_pLifeConditionDlg->RefreshPage();
}

CDnLifeTooltipDlg * CDnInterface::GetLifeTooltipDlg()
{
	return m_pLifeTooltipDlg;
}

CDnLifeConditionDlg * CDnInterface::GetLifeConditionDlg()
{
	return m_pLifeConditionDlg;
}

CDnLifeChannelDlg * CDnInterface::GetLifeChannelDlg()
{
	return m_pLifeChannelDlg;
}

void CDnInterface::OpenLifeSlotDialog()
{
	if( m_pLifeSlotDlg )
		m_pLifeSlotDlg->Show( true );
}

void CDnInterface::CloseLifeSlotDialog()
{
	if( m_pLifeSlotDlg )
		m_pLifeSlotDlg->Show( false );
}

void CDnInterface::OpenFarmChannel()
{
	m_bOpenFarmChannel = true;	
}

void CDnInterface::CloseFarmChannel()
{
	m_bOpenFarmChannel = false;
}

void CDnInterface::OpenStageClearDialog()
{
	m_pStageClearDlg->SetCallback( m_pStageClearReportDlg->GetCallBack() );
	m_pStageClearReportDlg->SetCallback( NULL );
	m_pStageClearReportDlg->Show( false );

	m_pBlindDlg->RemoveBlindCallback( m_pStageClearReportDlg );
	m_pBlindDlg->AddBlindCallBack( m_pStageClearDlg );

	m_pStageClearDlg->Show( true );
}

void CDnInterface::CloseStageClearDialog(bool bLeader)
{
	ShowMiniMap( true );

	CEtUIDialogBase::CloseAllDialog();
	CloseNpcDialog();

	if( m_pStageClearDlg ) {
		m_pStageClearDlg->SetCallback( NULL );
		m_pStageClearDlg->Show(false);
	}

	m_pBlindDlg->Show(false);
}

void CDnInterface::OpenItemChoiceDialog(const TItem& itemInfo, float totalTime, const DWORD& dropItemUniqueID)
{
	if (m_pItemChoiceDlg == NULL)
		return;

#ifdef PRE_ADD_AUTO_DICE
	if(CGameOption::GetInstance().m_bEnableAutoDice == true)
	{
		if(CDnPartyTask::IsActive())
			GetPartyTask().ReqPartyJoinGetReversionItem(true);
		
		return;
	}
#endif

	m_pItemChoiceDlg->Open(itemInfo, totalTime, dropItemUniqueID);

	if (m_pStageClearMoveDlg && m_pStageClearMoveDlg->IsShow())		ShowStageClearMoveDialog(false);

	if (m_pDungeonMoveDlg && m_pDungeonMoveDlg->IsShow())
		ShowDungeonMoveDlg(false, false);
}

void CDnInterface::CloseItemChoiceDialog()
{
	if (m_pItemChoiceDlg)
	{
		m_pItemChoiceDlg->Close();
		CDnPartyTask *pPartyTask = (CDnPartyTask *)CTaskManager::GetInstance().GetTask( "PartyTask" );
		if (pPartyTask)
			pPartyTask->SetReversionItemSharing(false);
	}
}

bool CDnInterface::IsShowItemChoiceDialog() const
{
	if (m_pItemChoiceDlg)
		return m_pItemChoiceDlg->IsShow();

	return false;
}

void CDnInterface::ShowStageClearMoveDialog(bool bShow)
{
	if (m_pStageClearMoveDlg && m_pStageClearRepairDlg)
	{
		m_pStageClearMoveDlg->Show(bShow);
		m_pStageClearRepairDlg->Show(bShow);

		if (bShow)
			m_pStageClearRepairDlg->Calculate();
	}
}

void CDnInterface::TerminateStageClearWarpStandBy()
{
	CDnPartyTask *pPartyTask = (CDnPartyTask *)CTaskManager::GetInstance().GetTask( "PartyTask" );
	if (pPartyTask && pPartyTask->GetPartyState() == CDnPartyTask::STAGE_CLEAR_WARP_STANDBY)
	{
		if (IsShowItemChoiceDialog())
			CloseItemChoiceDialog();

		pPartyTask->SetPartyState(CDnPartyTask::NORMAL);
	}
}

bool CDnInterface::OpenItemUnsealDialog(const MIInventoryItem* pItem, CDnSlotButton *pSlotButton)
{
	if (pItem == NULL)
		return false;

	if (pItem->GetType() == MIInventoryItem::Item)
	{
		if( m_pItemUnsealDlg->IsShow() )
			m_pItemUnsealDlg->Show( false );
		const CDnItem* pConvItem = static_cast<const CDnItem*>(pItem);
		if (m_pItemUnsealDlg->SetInfo(pConvItem, pSlotButton) == false)
			return false;
		m_pItemUnsealDlg->Show(true);
	}
	else
	{
		_ASSERT(0);	// Note : Item Type이 아닌 것도 개봉이 가능해질 경우 처리
	}

	return true;
}

bool CDnInterface::CloseItemUnsealDialog()
{
	m_pItemUnsealDlg->Show(false);
	return true;
}

void CDnInterface::ShowItemSealDialog( bool bShow, CDnItem *pSealItem )
{
	if( !m_pItemSealDlg ) return;
	if( bShow ) m_pItemSealDlg->SetSealItem( pSealItem );
	m_pItemSealDlg->Show( bShow );
}

void CDnInterface::ShowItemPotentialDialog( bool bShow, CDnItem *pPotentialItem )
{
	if( !m_pItemPotentialDlg ) return;

#ifdef PRE_MOD_POTENTIAL_JEWEL_RENEWAL
	if( !m_pItemPotentialCleanDlg ) return;

	// 월드맵에서, 강제이동이나 컷신 등등 다이얼로그가 강제종료되는 상황을 막기 위해 제한(#71351, #71757 잠재력 부여 기능 개선)
	if(CDnWorld::GetInstance().GetMapType() != CDnWorld::MapTypeVillage )
	{
		MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3797 ) );
		return;
	}

	m_bIsRemovePotentialDlg = dynamic_cast<CDnPotentialJewel*>(pPotentialItem)->IsRemovePotentialItem();
	if(m_bIsRemovePotentialDlg)
	{
		// 소멸의 큐브
		if(bShow) m_pItemPotentialCleanDlg->SetPotentialItem(pPotentialItem);
		m_pItemPotentialCleanDlg->Show(bShow);
	}
	else
	{
		// 일반 코드.
		if(bShow) m_pItemPotentialDlg->SetPotentialItem(pPotentialItem);
		m_pItemPotentialDlg->Show(bShow);
	}
#else
	if( bShow ) m_pItemPotentialDlg->SetPotentialItem( pPotentialItem );
	m_pItemPotentialDlg->Show( bShow );
#endif // PRE_MOD_POTENTIAL_JEWEL_RENEWAL
}

void CDnInterface::ShowChatRoomDialog( bool bShow )
{
	if( !m_pChatRoomDlg ) return;
	if( bShow ) CloseAllMainMenuDialog();
	m_pChatRoomDlg->Show( bShow );
}

void CDnInterface::ShowChatRoomPassWordDlg( bool bShow, int nID, CEtUICallback *pCall )
{
	if( !m_pChatRoomPassWordDlg ) return;
	if( bShow ) {
		m_pChatRoomPassWordDlg->SetCallback( pCall );
		m_pChatRoomPassWordDlg->SetDialogID( nID );
	}
	m_pChatRoomPassWordDlg->Show( bShow );
}

LPCWSTR CDnInterface::GetChatRoomPassWord()
{
	if( !m_pChatRoomPassWordDlg ) return NULL;
	return m_pChatRoomPassWordDlg->GetPassword();
}

void CDnInterface::ShowCharmItemKeyDialog( bool bShow, CDnItem *pCharmItem )
{
	if( !m_pCharmItemKeyDlg ) return;
#ifdef PRE_ADD_AUTOUNPACK
	if( bShow )
	{
		// 자동열기창이 열려있는 경우.		
		if( GetInterface().IsOpenAutoUnPackDlg() )
			m_pCharmItemKeyDlg->SetCharmItem( pCharmItem, true );
		else
			m_pCharmItemKeyDlg->SetCharmItem( pCharmItem );
	}
		
#else
	if( bShow ) m_pCharmItemKeyDlg->SetCharmItem( pCharmItem );
#endif
	else m_pCharmItemKeyDlg->Show( bShow );
}

void CDnInterface::ShowCharmItemProgressDialog( bool bShow, char cInvenType, short sInvenIndex, INT64 biInvenSerial, BYTE cKeyInvenIndex, int nKeyItemID, INT64 biKeyItemSerial )
{
	if( !m_pCharmItemProgressDlg ) return;
	float fTime = CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::CharmItemOpenTime );
	if( fTime < 1000.0f ) 
		fTime = 1000.0f;
	fTime *= 0.001f;

#if defined(PRE_FIX_43986)
	float fDelayTime = CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::CharmItempDisplayDelayTime);
#endif // PRE_FIX_43986
	
#ifdef PRE_ADD_AUTOUNPACK 
	// 개당열기지연시간 %.
	if( GetInterface().IsOpenAutoUnPackDlg() )
		fTime *= GetInterface().GetAutoUnPackDlg()->GetUnPackTime();
#endif

#if defined(PRE_FIX_43986)	
	m_pCharmItemProgressDlg->ShowEx( bShow, cInvenType, sInvenIndex, biInvenSerial, cKeyInvenIndex, nKeyItemID, biKeyItemSerial, fTime, fDelayTime * 0.001f );	
#else
	m_pCharmItemProgressDlg->ShowEx( bShow, cInvenType, cInvenIndex, biInvenSerial, cKeyInvenIndex, nKeyItemID, biKeyItemSerial, fTime );
#endif // PRE_FIX_43986
}

void CDnInterface::ShowCharmItemResultDialog( bool bShow, int nItemID, int nCount, int nPeriod )
{
	if( !m_pCharmItemResultDlg ) return;

	if( nItemID == 0 )
	{
		ASSERT( 0 && "획득한게 아무것도없다. 서버오류!" );
		return;
	}

	// 골드획득.
	INT64 nGold = 0;
	if( nItemID == g_cGoldItemID )
	{
		nGold = nCount;
		INT64 nCoin = GetItemTask().GetCoin();
		if( COINMAX < ( nCoin + nGold ) )
		{			
			ServerMessageBox( 498 ); // "상자에서 획득할 수 있는 골드량보다 여유가 부족합니다. 골드량을 줄이고 다시 시도해 주세요."
			return;
		}		
	}

#ifdef PRE_ADD_AUTOUNPACK

	// 자동열기창이 열려있는 경우.	
	if( GetInterface().IsOpenAutoUnPackDlg() )
	{	
		m_pAutoUnPackResultDlg->SetResultItem( nItemID, nCount, nPeriod, nGold );	
		m_pAutoUnPackResultDlg->Show( bShow );
	}
	else
	{	
		if( bShow ) m_pCharmItemResultDlg->SetResultItem( nItemID, nCount, nPeriod, nGold );	
		m_pCharmItemResultDlg->Show( bShow );
	}
#else	
	if( bShow ) m_pCharmItemResultDlg->SetResultItem( nItemID, nCount, nPeriod, nGold );	

	m_pCharmItemResultDlg->Show( bShow );
#endif

}

void CDnInterface::ShowGuildRenameDlg(bool bShow, char cInvenType, BYTE cInvenIndex, INT64 biItemSerial)
{
	if( !m_pGuildRenameDlg ) return;
	if( bShow ) m_pGuildRenameDlg->SetInfo(cInvenType, cInvenIndex, biItemSerial);
	m_pGuildRenameDlg->Show( bShow );
}

void CDnInterface::ShowCharRenameDlg( bool bShow, CDnItem *pItem )
{
	if( !m_pCharRenameDlg ) return;

	m_pCharRenameDlg->SetCharRenameItem( pItem );
	m_pCharRenameDlg->Show( bShow );
}

void CDnInterface::ShowEnchantJewelDialog( bool bShow, CDnItem *pItem )
{
	if( !m_pEnchantJewelDlg ) return;
	if( bShow ) m_pEnchantJewelDlg->SetEnchantJewelItem( pItem );
	m_pEnchantJewelDlg->Show( bShow );
}

void CDnInterface::ShowGuildMarkCreateDialog( bool bShow, CDnItem *pItem )
{
	if( !m_pGuildMarkCreateDlg ) return;
	if( bShow ) m_pGuildMarkCreateDlg->SetGuildMarkItem( pItem );
	m_pGuildMarkCreateDlg->Show( bShow );
}

void CDnInterface::SetLastTryUseMapMoveCashItem( CDnItem* pUsedItem )
{
	if( !m_pMapMoveCashItemDlg ) 
		return;

	// 마을 리스트 패킷 받고 다이얼로그가 열리므로.. 
	if( false == m_pMapMoveCashItemDlg->IsShow() )
		m_pMapMoveCashItemDlg->SetUsedItem( pUsedItem );
}

void CDnInterface::ShowMapMoveCashItemDlg( bool bShow, const WarpVillage::WarpVillageInfo* pVillageList, int iListCount )
{
	if( bShow )
	{
		if( !m_pMapMoveCashItemDlg ) 
			return;

		m_pMapMoveCashItemDlg->SetVillageList( pVillageList, iListCount );
	}

	m_pMapMoveCashItemDlg->Show( bShow );
}

void CDnInterface::ResetMapMoveOKBtn( void )
{
	if( !m_pMapMoveCashItemDlg ) 
		return;

	m_pMapMoveCashItemDlg->ResetOKBtn();
}

bool CDnInterface::IsShowMapMoveCashItemDlg( void )
{
	bool bResult = false;
	if( NULL != m_pMapMoveCashItemDlg )
	{
		if( m_pMapMoveCashItemDlg->IsShow() )
			bResult = true;
	}

	return bResult;
}

EtTextureHandle CDnInterface::GetGuildMarkIconTex( int nIndex )
{
	EtTextureHandle hIconTexture;
	if( nIndex < (int)m_vecGuildMarkTextureHandle.size() && m_vecGuildMarkTextureHandle[ nIndex ] )
		hIconTexture = m_vecGuildMarkTextureHandle[ nIndex ];
	return hIconTexture;
}

void CDnInterface::ShowVoiceChatPremiumOptDialog(bool bShow, bool bDisableApplyButton)
{
#ifdef _USE_VOICECHAT
	if (!m_pVoiceChatPremiumOptDlg) return;
	m_pVoiceChatPremiumOptDlg->Show(bShow);

	// 캐시샵같은데서 열때는 강제로 적용버튼 비활성 시킨다.
	if (bDisableApplyButton) {
		m_pVoiceChatPremiumOptDlg->DisableApplyButton();
		if (m_pTooltipDlg) m_pTooltipDlg->HideTooltip();
	}
#endif
}

#ifdef PRE_ADD_EXCHANGE_POTENTIAL
void CDnInterface::ShowItemPotentialTransferDialog( bool bShow )
{
	if( !m_pPotentialTransferDlg ) return;
	m_pPotentialTransferDlg->Show( bShow );
}
#endif

void CDnInterface::OpenStageClearReportDialog( CEtUICallback *pCallback )
{
	ShowMiniMap( false );

	CloseRebirthCaptionDlg();
	CloseRebirthFailDlg();

	// 스테이지클리어 결과창이 열릴때 거래중이었다면 인벤토리가 닫히면서 취소요청을 보내게 된다.
	CEtUIDialogBase::CloseAllDialog();
	CloseNpcDialog();

	m_pStageClearReportDlg->SetCallback( pCallback );

	m_pBlindDlg->RemoveBlindCallback( m_pNpcDlg );
	m_pBlindDlg->SetBillBoardColor( 0x50000000 );
	m_pBlindDlg->AddBlindCallBack( m_pStageClearReportDlg );
	m_pBlindDlg->AddBlindCallBack( ((CDnLocalPlayerActor*)CDnActor::s_hLocalActor.GetPointer()) );
	m_pBlindDlg->Show(true);


#ifdef PRE_ADD_ACTIVEMISSION
	RefreshActiveMissionEventMark( 0, std::wstring(L""), false, false );
#endif // PRE_ADD_ACTIVEMISSION

}

void CDnInterface::OpenDungeonEnterDialog( CDnWorld::DungeonGateStruct *pGateInfo, CEtUICallback *pCallback )
{
	ShowMiniMap( false );
	CEtUIDialogBase::CloseAllDialog();
	
	CDnDungeonEnterDlg *pDlg = NULL;
	switch( pGateInfo->SetType ) 
	{
		case CDnWorld::DungeonGateStruct::DungeonSetType::Defaualt: 
			pDlg = m_pDungeonEnterDlg; 
			break;
		case CDnWorld::DungeonGateStruct::DungeonSetType::Event: 
			pDlg = m_pEventDungeonEnterDlg; 
			break;
		case CDnWorld::DungeonGateStruct::DungeonSetType::Nest:
			pDlg = m_pDungeonNestEnterDlg; 
			break;
	}

	if( pDlg == NULL )
		return;

	m_cLastDungeonEnterDialogType = (char)pGateInfo->SetType;
 
	pDlg->SetGateName( pGateInfo->szMapName );
	pDlg->SetGateInfo( pGateInfo );
	pDlg->SetCallback( pCallback );

	m_pBlindDlg->SetBillBoardColor( 0x50000000 );
	m_pBlindDlg->AddBlindCallBack( pDlg );
	m_pBlindDlg->AddBlindCallBack( ((CDnLocalPlayerActor*)CDnActor::s_hLocalActor.GetPointer()) );
	m_pBlindDlg->Show(true);
}

void CDnInterface::OpenDarkLairEnterDialog( CDnWorld::DungeonGateStruct *pGateInfo, CEtUICallback *pCallback )
{
	ShowMiniMap( false );
	CEtUIDialogBase::CloseAllDialog();

//	m_pDarkLairEnterDlg->SetGateName( pGateInfo->szMapName );
//	m_pDarkLairEnterDlg->SetGateInfo( pGateInfo );
	m_pDarkLairEnterDlg->Refresh( pGateInfo );
	m_pDarkLairEnterDlg->SetCallback( pCallback );

	m_cLastDungeonEnterDialogType = CDnWorld::DungeonGateStruct::DungeonSetType::DarkLair;

	m_pBlindDlg->SetBillBoardColor( 0x50000000 );
	m_pBlindDlg->AddBlindCallBack( m_pDarkLairEnterDlg );
	m_pBlindDlg->AddBlindCallBack( ((CDnLocalPlayerActor*)CDnActor::s_hLocalActor.GetPointer()) );
	m_pBlindDlg->Show(true);
}

void CDnInterface::OpenDarkLariRoundCountDialog( int nRound, bool bBoss, bool bFinalRound )
{
	if( m_pDarkLairRoundCountDlg ) m_pDarkLairRoundCountDlg->Show( nRound, bBoss, bFinalRound );
	if( m_pDarkLairInfoDlg ) m_pDarkLairInfoDlg->RefreshRound( nRound, bBoss, bFinalRound );
}

void CDnInterface::OpenDarkLairClearReportDialog( CEtUICallback *pCallback )
{
	ShowMiniMap( false );

	CEtUIDialogBase::CloseAllDialog();
	CloseNpcDialog();

	m_pDarkLairClearReportDlg->SetCallback( pCallback );

	m_pBlindDlg->RemoveBlindCallback( m_pNpcDlg );
	m_pBlindDlg->SetBillBoardColor( 0x50000000 );
	m_pBlindDlg->AddBlindCallBack( m_pDarkLairClearReportDlg );
	m_pBlindDlg->AddBlindCallBack( ((CDnLocalPlayerActor*)CDnActor::s_hLocalActor.GetPointer()) );
	m_pBlindDlg->Show(true);
}

void CDnInterface::OpenDarkLairClearRankDialog()
{
	m_pDarkLairClearReportDlg->Show( false );
	m_pDarkLairClearRankDlg->SetCallback( m_pDarkLairClearReportDlg->GetCallBack() );
	m_pDarkLairClearReportDlg->SetCallback( NULL );

	m_pBlindDlg->RemoveBlindCallback( m_pDarkLairClearReportDlg );
	m_pBlindDlg->AddBlindCallBack( m_pDarkLairClearRankDlg );

	m_pDarkLairClearRankDlg->Show( true );
}

void CDnInterface::OpenDarkLairClearRewardItemDialog()
{
	m_pDarkLairClearRankDlg->Show( false );
	m_pDarkLairClearRewardDlg->SetCallback( m_pDarkLairClearRankDlg->GetCallBack() );
	m_pDarkLairClearRankDlg->SetCallback( NULL );

	m_pBlindDlg->RemoveBlindCallback( m_pDarkLairClearRankDlg );
	m_pBlindDlg->AddBlindCallBack( m_pDarkLairClearRewardDlg );

	m_pDarkLairClearRewardDlg->Show( true );
}

void CDnInterface::CloseDarkLairClearRewardItemDialog()
{
	ShowMiniMap( true );

	CEtUIDialogBase::CloseAllDialog();
	CloseNpcDialog();

	if( m_pDarkLairClearRewardDlg ) {
		m_pDarkLairClearRewardDlg->SetCallback( NULL );
		m_pDarkLairClearRewardDlg->Show(false);
	}

	m_pBlindDlg->Show(false);
}

void CDnInterface::RequestPVPLadderRankInfo()
{
	if( m_pPVPLadderRankBoardDlg )
		m_pPVPLadderRankBoardDlg->SetRankInfoRequest( true );
}

void CDnInterface::OpenPVPLadderRankBoardDialog(SCPvPLadderRankBoard* pData)
{
	if( m_pPVPLadderRankBoardDlg == NULL )
		return;

	m_pPVPLadderRankBoardDlg->SetPVPLadderRankInfo(pData);
	m_pPVPLadderRankBoardDlg->Show( true );
	OpenNpcTalkReturnDlg();
}

#if defined( PRE_ADD_REVENGE )
void CDnInterface::ShowPVPRevengeDlg( const std::wstring wszRevengeRequestUser )
{
	if( NULL != m_pPVPRevengeMessageBox )
	{
		m_pPVPRevengeMessageBox->SetMessageBox( wszRevengeRequestUser );
		m_pPVPRevengeMessageBox->Show( true );
	}
}
#endif	// #if defined( PRE_ADD_REVENGE )

void CDnInterface::RequestDarkLairRankInfo()
{
	if( m_pDarkLairRankBoardDlg )
		m_pDarkLairRankBoardDlg->SetRankInfoRequest( true );
}

void CDnInterface::OpenDarkRareRankBoardDialog( TDLRankHistoryPartyInfo* pBestInfo, TDLRankHistoryPartyInfo* pHistoryInfo )
{
	if( m_pDarkLairRankBoardDlg == NULL || pBestInfo == NULL ||  pHistoryInfo == NULL )
		return;

	m_pDarkLairRankBoardDlg->SetDarkLairRankInfo( pBestInfo, pHistoryInfo );
	m_pDarkLairRankBoardDlg->Show( true );

	OpenNpcTalkReturnDlg();
}

#if defined(PRE_ADD_CHALLENGE_DARKLAIR)
void CDnInterface::SetDarkRareRankBoardMapIndex( const int nMapIndex )
{
	if( m_pDarkLairRankBoardDlg == NULL )
		return;

	m_pDarkLairRankBoardDlg->SetDarkLairRankMapIndex( nMapIndex );
}
#endif	// #if defined(PRE_ADD_CHALLENGE_DARKLAIR)

void CDnInterface::OpenGuildWarPresentCondition()
{
	if( m_pGuildWarPreliminaryStatusDlg == NULL )
		return;

	m_pGuildWarPreliminaryStatusDlg->Show( true );

	OpenNpcTalkReturnDlg();
}

void CDnInterface::OpenGuildWarPreliminaryResult()
{
	if( m_pGuildWarPreliminaryResultDlg == NULL )
		return;

	m_pGuildWarPreliminaryResultDlg->Show( true );

	OpenNpcTalkReturnDlg();
}

void CDnInterface::OpenGuildWarFinalTournamentList()
{
	if( m_pGuildWarFinalTournamentListDlg == NULL )
		return;

	m_pGuildWarFinalTournamentListDlg->Show( true );

	OpenNpcTalkReturnDlg();
}

void CDnInterface::SetGuildWarPreliminaryResult()
{
	if( m_pGuildWarPreliminaryResultDlg && m_pGuildWarPreliminaryResultDlg->IsShow() )
	{
		m_pGuildWarPreliminaryResultDlg->SetGuildWarPreliminaryResultDlg();
	}
}

void CDnInterface::SetGuildWarFinalTournamentList()
{
	if( m_pGuildWarFinalTournamentListDlg && m_pGuildWarFinalTournamentListDlg->IsShow() )
	{
		m_pGuildWarFinalTournamentListDlg->SetTournamentUI();
	}
}

void CDnInterface::SetGuildWarSkillUI( UINT uiMyPVPState, bool bResetGuildWarSkillList )
{
#if defined( PRE_REMOVE_GUILD_WAR_UI )
	if( !CDnActor::s_hLocalActor ) return;

	if( bResetGuildWarSkillList )
		GetSkillTask().LoadGuildWarSkillList( (PvPCommon::Team::eTeam)CDnActor::s_hLocalActor->GetTeam() );

	CDnSkillTreeDlg* pSkillTreeDlg = static_cast<CDnSkillTreeDlg*>( GetInterface().GetMainMenuDialog( CDnMainMenuDlg::SKILL_DIALOG ) );
	if( pSkillTreeDlg )
	{
		if( uiMyPVPState & PvPCommon::UserState::GuildWarCaptain )
			pSkillTreeDlg->SetGuildSkillMode( true, true );
		else
			pSkillTreeDlg->SetGuildSkillMode( true, false );

		if( pSkillTreeDlg->IsShow() )
		{
			pSkillTreeDlg->UpdateView();
			pSkillTreeDlg->UpdateGuildWarSkillTreeContent();
		}
	}

	if( m_pMainBar )
	{
		if( uiMyPVPState & PvPCommon::UserState::GuildWarCaptain )
			m_pMainBar->ShowGuildWarSkillSlotDlg( true, false );
		else if( uiMyPVPState & PvPCommon::UserState::GuildWarSedcondCaptain )
			m_pMainBar->ShowGuildWarSkillSlotDlg( true, true );
		else
			m_pMainBar->ShowGuildWarSkillSlotDlg( false, false );

		if( bResetGuildWarSkillList )
		{
			CDnGuildWarSkillSlotDlg* pGuildWarSkillSlotDlg = m_pMainBar->GetGuildWarSkillSlotDialog();
			if( pGuildWarSkillSlotDlg )
				pGuildWarSkillSlotDlg->ResetAllSlot();
		}
	}

	if( uiMyPVPState & PvPCommon::UserState::GuildWarCaptain || uiMyPVPState & PvPCommon::UserState::GuildWarSedcondCaptain )
	{
		CEtUIDialog* pDnNotifierDialog = GetInterface().GetMainMenuDialog( CDnMainMenuDlg::NOTIFIER_DIALOG );
		if( pDnNotifierDialog )
			pDnNotifierDialog->Show( false );
	}
#endif	// #if defined( PRE_REMOVE_GUILD_WAR_UI )
}

// Second Authenticate
void CDnInterface::OpenSecurityKBDDlg(int State)
{
	// 0 번은 ID  1번은 패스워드 
	if( State == 0 ) 
	{
		m_pSecurityKBDDlg->Show(true , 0 );
	}
	else if ( State == 1 )
	{
		m_pSecurityKBDDlg->Show(true , 1 );
	}
}
bool CDnInterface::OpenSecondAuthDlg(int nChannelID)
{
	CDnAuthTask *pAuthTask = (CDnAuthTask *)CTaskManager::GetInstance().GetTask("AuthTask");
	if(!pAuthTask) return FALSE; 

	// 2차 비번이 없으면 무조건 띄움니다. 서버에서 보여줘 메세지를 주면
	if( !pAuthTask->GetSecondAuthPW() && pAuthTask->GetSecondAuthNotifyShow() )
	{
		m_pSecondPassCheck->SetSelectChannel(nChannelID);
		m_pSecondPassCheck->Show(true);
		return TRUE; 
	}
	return FALSE;
}

void CDnInterface::OpenSecurityPassCheck()
{
#if defined(PRE_ADD_23829)
	// [2010/12/07 semozz]
	// 2차 비밀번호 확인 창은 서버 정보를 통해서 보여줄지 말지를 결정한다.
	// SCGameOptionNotifySecondAuthInfo의 bSecondAuthPassFlag값을 저장해 놓는다.

	CDnAuthTask *pAuthTask = (CDnAuthTask *)CTaskManager::GetInstance().GetTask("AuthTask");
	//pAuthTask가 없으면?
	if (!pAuthTask)
		return;

	// 플래그가 설정되어 있으면 2차 비밀번호 확인창을
	if (pAuthTask->GetSecondAuthNotifyShow())
	{
		if (m_pSecondPassCheck)
			m_pSecondPassCheck->Show(true);
	}
	// 설정되어 있지 않으면 채널 리스트창을 보여 준다.
	else
	{
		CDnLoginTask *pLoginTask = (CDnLoginTask *)CTaskManager::GetInstance().GetTask("LoginTask");
		if (pLoginTask)
			pLoginTask->ChangeState(CDnLoginTask::LoginStateEnum::ChannelList);
	}
#else
	if (m_pSecondPassCheck)
		m_pSecondPassCheck->Show(true);
#endif
}

void CDnInterface::OpenSecurityChangeDlg()
{
	m_pSecurityChangeDlg->Show(true);
}

void CDnInterface::OpenSecurityDelete()
{
	m_pSecurityDeleteDlg->Show(true);
}

bool CDnInterface::IsShowSecurityCheckDlg()
{
	return m_pSecurityCheckDlg->IsShow();
}

bool CDnInterface::IsShowSecurityChangeDlg()
{
	return m_pSecurityChangeDlg->IsShow();
}

void CDnInterface::OpenSecurityCheckDlg(InterfaceTypeEnum Type /* = InterfaceTypeEnum::Game  */, int AuthType /* = 2 */)
{
#ifdef PRE_ADD_DWC
	if(GetDWCTask().IsDWCRankSession() == false && GetDWCTask().IsDWCChar()) // DWC시즌이 아니라면
	{
		// mid: 콜로대회 기간이 아닙니다. 해당 캐릭은 사용할수없습니다.
		GetInterface().MessageBox(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 120270));
		return;
	}
#endif

	//AuthType 
	CDnAuthTask *pAuthTask = (CDnAuthTask *)CTaskManager::GetInstance().GetTask("AuthTask");
	if(!pAuthTask) return;

	if( Type == InterfaceTypeEnum::Login )
	{
		if(pAuthTask->GetSecondAuthPW()) 
		{
			m_pSecurityCheckDlg->Show(true , AuthType );
		}
		else 
		{
			OpenSecurityPassCheck();
		}
	}
}

void CDnInterface::OpenSecurityCreateDlg()
{
	m_pSecurityCreateDlg->Show(true);
}

//blondy
void CDnInterface::ShowPVPModeEndDialog( byte cGaemeMode , bool bShow , byte cResult , bool FinalRound )
{
	if( m_pPVPModeEndDlg)
	{
		m_pPVPModeEndDlg->Show( bShow );

		if( bShow )
			m_pPVPModeEndDlg->PlayResultSound( cResult  );
	}

}

void CDnInterface::OpenpRespawnGauageDlg( PCWSTR pwszMessage, float fTime, bool bText )
{
	if( m_pRespawnGauageDlg)
	{
		m_pRespawnGauageDlg->Show( true );
		m_pRespawnGauageDlg->SetInfo( pwszMessage , fTime  );
		m_pRespawnGauageDlg->SetSpaceText( bText );
	}
}

void CDnInterface::ShowRespawnGauageDlg(bool bShow)
{
	if(m_pRespawnGauageDlg)
	{
		m_pRespawnGauageDlg->Show(bShow);
		m_pRespawnGauageDlg->SetSpaceText( bShow );
	}
}

//blondy end
void CDnInterface::OpenBlind()
{
	if( !m_pBlindDlg ) return;

	if( m_pNpcDlg && m_pNpcDlg->IsShow() )
	{
		m_bIsOpenNpcDlg = true;
		m_pNpcDlg->Show( false );
		return;
	}

	ShowMiniMap( false );

	if( GetChatDialog() ) {
		GetChatDialog()->SetIgnoreShowFunc( true );
		GetChatDialog(true)->SetIgnoreShowFunc( true );
	}
	CEtUIDialogBase::CloseAllDialog();

	// 채팅 다이얼로그의 show설정은 다시 true로 되돌린다.
	// 이렇게 해야 ChatTabDlg::ShowEx에서 m_groupTabDialog.GetShowDialog이 가능해진다.
	ShowChatDialog();

	m_pBlindDlg->SetBillBoardColor( 0x00000000 );
	m_pBlindDlg->Show(true);
	m_pBlindDlg->AddBlindCallBack( ((CDnLocalPlayerActor*)CDnActor::s_hLocalActor.GetPointer()) );
}

void CDnInterface::CloseBlind( bool bShowBaseDialog )
{
#ifdef PRE_ADD_QUEST_BACK_DIALOG_BUTTON
	DnCameraHandle		 hCurrentCam		= CDnCamera::GetActiveCamera();
	CDnNpcTalkReturnDlg* pNpcTalkReturnDlg  = GetInterface().GetNpcTalkReturnDlg();
	if( pNpcTalkReturnDlg == NULL || hCurrentCam == NULL ) return;

	if( hCurrentCam->GetCameraType() == CDnCamera::CameraTypeEnum::NpcTalkCamera && pNpcTalkReturnDlg->GetButtonToggle() )
	{
		return;
	}
#endif

	if( !m_pBlindDlg ) return;

	if( bShowBaseDialog ) {
		if( m_bIsOpenNpcDlg )
		{
			m_bIsOpenNpcDlg = false;
			if( m_pNpcDlg ) m_pNpcDlg->Show( true );
			return;
		}

		OpenBaseDialog();
	}

	m_pBlindDlg->Show(false);

	// 돌아가기 버튼도 종료
	CloseNpcTalkReturnDlg();

#ifdef PRE_FIX_SUMMON_ACTOR_HPBAR 
	// 현재 화면에 이미 렌더링 되고 있는 액터들을 모아 둠
	deque<DnActorHandle> dqhLiveActors;
	int iNumLiveActor = (int)CDnActor::s_pVecProcessList.size();
	for(int iActor = 0; iActor < iNumLiveActor; ++iActor)
	{
		DnActorHandle hLiveActor = CDnActor::s_pVecProcessList.at(iActor)->GetMySmartPtr();
		if(hLiveActor->IsNpcActor() && !hLiveActor->IsShow())
			continue;
		dqhLiveActors.push_back( hLiveActor );
	}
	if(!dqhLiveActors.empty())
	{
		for(int i = 0 ; i < (int)dqhLiveActors.size() ; ++i)
		{
			DnActorHandle hActor = dqhLiveActors.at(i);
			if(!hActor) continue;
			if( hActor->IsDie()) continue;
			GetInterface().ShowEnemyGauge(hActor, true);
		}
	}
#endif // PRE_FIX_SUMMON_ACTOR_HPBAR


#ifdef PRE_ADD_STAMPSYSTEM
	if( GetInterface().GetInterfaceType() == CDnInterface::Village )
		GetInterface().AddStamp(false, true);
#endif // PRE_ADD_STAMPSYSTEM

}

void CDnInterface::SetSelectDungeonInfo( int nMapIndex, int nDifficult )
{
	switch( m_cLastDungeonEnterDialogType ) 
	{
		case CDnWorld::DungeonGateStruct::DungeonSetType::Defaualt:
			if( m_pDungeonEnterDlg ) m_pDungeonEnterDlg->SetSelectDungeonInfo( nMapIndex, nDifficult );
			break;
		case CDnWorld::DungeonGateStruct::DungeonSetType::Event: 
			if( m_pEventDungeonEnterDlg ) m_pEventDungeonEnterDlg->SetSelectDungeonInfo( nMapIndex, nDifficult );
			break;
		case CDnWorld::DungeonGateStruct::DungeonSetType::DarkLair:
			if( m_pDarkLairEnterDlg ) m_pDarkLairEnterDlg->SetSelectDungeonInfo( nMapIndex, nDifficult );
			break;
		case CDnWorld::DungeonGateStruct::DungeonSetType::Nest:
			if( m_pDungeonNestEnterDlg ) m_pDungeonNestEnterDlg->SetSelectDungeonInfo( nMapIndex, nDifficult );
			break;
	}
}

#if defined( PRE_ADD_STAGE_CLEAR_ADD_REWARD )
void CDnInterface::OpenDungeonExtraRewardDlg( DWORD dwPropIndex )
{
	if( NULL == m_pDungeonExtraRewardDlg )
		return;
	
	m_pDungeonExtraRewardDlg->Show( true );
	m_pDungeonExtraRewardDlg->SetPropIndex( dwPropIndex );
}
#endif	// PRE_ADD_STAGE_CLEAR_ADD_REWARD

void CDnInterface::OpenBaseDialog()
{
	ShowMiniMap( true );

	for( int i=0; i<(int)m_pVecOpenBaseDialogList.size(); i++ )
	{
		m_pVecOpenBaseDialogList[i]->Show(true);
	}

	if( m_Type == Farm && m_pLifeConditionDlg )
	{
		if( m_pPlayerGaugeDlg->IsLifeCondition() )
			m_pLifeConditionDlg->Show( true );
		GetLifeSkillPlantTask().OpenBaseDialog();
		m_pLifeTooltipDlg->Show( true );
	}

	ShowChatDialog();

	if( m_pMainMenuDlg )
	{
		m_pMainMenuDlg->SetQuestSumInfoDlg();
	}

	RefreshStageInfoDlg( false );
	RefreshPartyGuage();
	RefreshGuildWarSituation();

	if( GetInterface().GetInterfaceType() == CDnInterface::Game )
	{
		CDnGameTask* pTask = (CDnGameTask*)CTaskManager::GetInstance().GetTask( "GameTask" );
		DNTableFileFormat*  pSox = GetDNTable( CDnTableDB::TMAP );
		if( pTask && pSox )
		{
			int nCurMapIndex = pTask->GetGameMapIndex();
			CDnWorld::MapSubTypeEnum MapSubType = (CDnWorld::MapSubTypeEnum)pSox->GetFieldFromLablePtr( nCurMapIndex, "_MapSubType" )->GetInteger();
		
			if( MapSubType == CDnWorld::MapSubTypeNest && pTask->GetStageConstructionLevel() == 3 )	// 하드코어 모드 타이머 표시
			{
				ShowHardCoreModeTimeAttackDlg( true );
			}
		}
	}
}

void CDnInterface::RefreshPlateDialog()
{
	if( !CDnActor::s_hLocalActor )	return;
	if( !m_pMainBar )				return;

	m_pMainBar->OnRefreshQuickSlot();
	((CDnCharStatusDlg *)(m_pMainMenuDlg->GetMainDialog( CDnMainMenuDlg::CHARSTATUS_DIALOG )))->RefreshGlyphEquip();
}

void CDnInterface::RefreshGlyphCover()
{
	if( !CDnActor::s_hLocalActor )	return;
	if( !m_pMainBar )				return;

	m_pMainBar->OnRefreshQuickSlot();
	((CDnCharStatusDlg *)(m_pMainMenuDlg->GetMainDialog( CDnMainMenuDlg::CHARSTATUS_DIALOG )))->RefreshGlyphCover();
}

void CDnInterface::OnRefreshLocalPlayerStatus()
{
	if(m_pMainMenuDlg)
	{
		((CDnCharStatusDlg *)(m_pMainMenuDlg->GetMainDialog( CDnMainMenuDlg::CHARSTATUS_DIALOG )))->OnRefreshPlayerStatus();
	}
}

void CDnInterface::RefreshLadderInfo()
{
	if(m_pMainMenuDlg)
	{
		((CDnCharStatusDlg *)(m_pMainMenuDlg->GetMainDialog( CDnMainMenuDlg::CHARSTATUS_DIALOG )))->RefreshLadderInfo();
	}
}

void CDnInterface::ChangeSkillPage(int nPage)
{
	if( !CDnActor::s_hLocalActor )	return;
	if( !m_pMainBar )				return;

	m_pMainBar->ChangeSkillSlotPage(nPage);

	CDnSkillTreeDlg *pSkillTreeDlg = (CDnSkillTreeDlg*)GetMainMenuDialog( CDnMainMenuDlg::SKILL_DIALOG );
	if( pSkillTreeDlg ) 
		pSkillTreeDlg->SelectSkillPage(nPage);

}

void CDnInterface::RefreshSkillDialog()
{
	if( !CDnActor::s_hLocalActor )	return;
	if( !m_pMainBar )				return;

	//CDnSkillTabDlg *pSkillDlg = (CDnSkillTabDlg*)GetMainMenuDialog( CDnMainMenuDlg::SKILL_DIALOG );
	//if( !pSkillDlg ) return;
	CDnSkillTreeDlg *pSkillTreeDlg = (CDnSkillTreeDlg*)GetMainMenuDialog( CDnMainMenuDlg::SKILL_DIALOG );
	if( !pSkillTreeDlg ) return;

	CDnSkillStoreTabDlg *pSkillStoreDlg = (CDnSkillStoreTabDlg*)GetMainMenuDialog( CDnMainMenuDlg::SKILL_STORE_DIALOG );
	if( !pSkillStoreDlg ) return;

	pSkillTreeDlg->UpdateView();
		
	//pSkillTreeDlg->ResetAllItem();

	// TODO: 추후에 job 까지 고려되어 셋팅해야 함..
	//CDnSkillTreeContentDlg* pSkillTreeContentDlg = dynamic_cast<CDnSkillTreeContentDlg*>(pSkillTreeDlg->GetContentDialog());
	//if( pSkillTreeContentDlg )
	//{
		//CDnSkillTask* pSkillTask = static_cast<CDnSkillTask*>( CTaskManager::GetInstance().GetTask( "SkillTask" ) );
		//pSkillTreeContentDlg->ClearSkillSlot();
		//for( int i = 0; i < MAX_SKILL_SLOT_COUNT; i++ )
		//{
		//	CDnSkillTask::S_SKILLTREE_SLOT_INFO SlotInfo;
		//	pSkillTask->GetSkillTreeSlotInfo( 1, i, SlotInfo );
		//	pSkillTreeContentDlg->AddSkillSlot( SlotInfo );
		//}
		//pSkillTreeContentDlg->UpdateSkillSlot();
	//}

	m_pMainBar->OnRefreshQuickSlot();
	pSkillStoreDlg->OnRefreshSlot();
}

#ifdef PRE_MOD_SKILLRESETITEM
void CDnInterface::OpenSkillResetConfirmDlg(CDnItem* pItem, int iRewardSP)
{
	if (!m_pSkillResetConfirmDlg)
		return;

	if (pItem == NULL)
		return;

	m_pSkillResetConfirmDlg->Show(true);

	eItemPositionType invenType = pItem->IsCashItem() ? ITEMPOSITION_CASHINVEN : ITEMPOSITION_INVEN;
	m_pSkillResetConfirmDlg->UpdateContent(pItem->GetSerialID(), pItem->GetTypeParam(0), pItem->GetTypeParam(1), iRewardSP, invenType, pItem->GetSlotIndex());
}
#else
void CDnInterface::OpenSkillResetConfirmDlg( INT64 biItemSerial, int iStartJobDegree, int iEndJobDegree, int iRewardSP )
{
	if( !m_pSkillResetConfirmDlg ) return;
	m_pSkillResetConfirmDlg->Show( true );
	m_pSkillResetConfirmDlg->UpdateContent( biItemSerial, iStartJobDegree, iEndJobDegree, iRewardSP );
}
#endif

void CDnInterface::CloseSkillResetConfirmDlg()
{
	if( !m_pSkillResetConfirmDlg ) return;
	m_pSkillResetConfirmDlg->Show( false );
}

#ifdef PRE_ADD_CHANGEJOB_CASHITEM
void CDnInterface::ShowChangeJobDialog( bool bShow, CDnItem* pChangeJobItem )
{
	if( m_pChangeJobDlg )
	{
		m_pChangeJobDlg->Show( true );
		m_pChangeJobDlg->SetItem( pChangeJobItem );
	}
}

bool CDnInterface::IsShowChangeJobDialog() const
{
	if( m_pChangeJobDlg )
		return m_pChangeJobDlg->IsShow();

	return false;
}
#endif // #ifdef PRE_ADD_CHANGEJOB_CASHITEM

void CDnInterface::DelPrivateName( LPCWSTR wszPrivateName )
{
	if( !GetChatDialog() ) return;
	GetChatDialog()->DelPrivateName( wszPrivateName );
}

bool CDnInterface::IsFocusEditBox()
{
	bool bOpenChat = false;
	/*
	if( m_pChatDlg && m_pChatDlg->IsEditBoxFocus() ) return false;
	if(  ) return true;
	*/
	return CEtUIEditBox::IsFocusEditBox();
}

#ifdef PRE_ADD_GACHA_JAPAN
// Gacha
void CDnInterface::ShowGachaDialog( int nGachaIndex )
{
	if( m_pGachaDlg )
	{
		m_pGachaDlg->SetGachaIndex( nGachaIndex );
		m_pGachaDlg->Show( true );
	}

	// 인벤창도 같이 띄워준다.
	m_pMainMenuDlg->Show( true );
	m_pMainMenuDlg->OpenInvenDialog();

	OpenNpcTalkReturnDlg();
}


void CDnInterface::CloseGachaDialog( void )
{
	if( m_pGachaDlg )
		m_pGachaDlg->Show( false );

	m_pMainMenuDlg->CloseInvenDialog();
}


bool CDnInterface::IsShowGachaShopDlg( void )
{
	return (m_pGachaDlg && m_pGachaDlg->IsShow());
}


void CDnInterface::OnRecvGachaRes_JP( const SCRefreshCashInven* pPacket )
{
	if( m_pGachaDlg )
		m_pGachaDlg->OnRecvGachaResult( pPacket );
}
#endif // PRE_ADD_GACHA_JAPAN

void CDnInterface::ShowCostumeMixDlg(bool bShow, bool bCloseInvenTab)
{
	if (m_pMainMenuDlg == NULL || m_pMainBar == NULL)
		return;

	m_pMainMenuDlg->ShowCostumeMixDialog(bShow, bCloseInvenTab);
	m_pMainBar->EnableQuickSlot(!bShow);
}

bool CDnInterface::IsShowCostumeMixDlg() const
{
	return (m_pMainMenuDlg->IsOpenDialog(CDnMainMenuDlg::ITEM_COSTUMEMIX_DIALOG));
}

CDnCostumeMixDlg* CDnInterface::GetCostumeMixDlg() const
{
	if (!m_pMainMenuDlg)
		return NULL;
	return static_cast<CDnCostumeMixDlg*>(m_pMainMenuDlg->GetMainDialog(CDnMainMenuDlg::ITEM_COSTUMEMIX_DIALOG));
}

void CDnInterface::ShowCostumeDesignMixDlg(bool bShow, bool bCloseInvenTab)
{
	if (m_pMainMenuDlg == NULL || m_pMainBar == NULL)
		return;

	m_pMainMenuDlg->ShowCostumMergeDialog(bShow, bCloseInvenTab);
	m_pMainBar->EnableQuickSlot(!bShow);
}

bool CDnInterface::IsShowCostumeDesignMixDlg() const
{
	return (m_pMainMenuDlg->IsOpenDialog(CDnMainMenuDlg::ITEM_COSTUME_DMIX_DIALOG));
}

CDnCostumeDesignMixDlg* CDnInterface::GetCostumeDesignMixDlg() const
{
	if (!m_pMainMenuDlg)
		return NULL;
	return static_cast<CDnCostumeDesignMixDlg*>(m_pMainMenuDlg->GetMainDialog(CDnMainMenuDlg::ITEM_COSTUME_DMIX_DIALOG));
}

#ifdef PRE_ADD_COSRANDMIX
void CDnInterface::ShowCostumeRandomMixDlg(bool bShow, bool bCloseInvenTab)
{
	if (m_pMainMenuDlg == NULL || m_pMainBar == NULL)
		return;

#ifdef PRE_ADD_COSRANDMIX_ACCESSORY
	if (m_pMainMenuDlg->IsShow() == false)
		m_pMainMenuDlg->Show(true);
#endif

	m_pMainMenuDlg->ShowCostumeRandomMixDialog(bShow, bCloseInvenTab);
	m_pMainBar->EnableQuickSlot(!bShow);
}

bool CDnInterface::IsShowCostumeRandomMixDlg() const
{
	return (m_pMainMenuDlg->IsOpenDialog(CDnMainMenuDlg::ITEM_COSTUME_RANDOMMIX_DIALOG));
}

CDnCostumeRandomMixDlg* CDnInterface::GetCostumeRandomMixDlg() const
{
	if (!m_pMainMenuDlg)
		return NULL;

	return static_cast<CDnCostumeRandomMixDlg*>(m_pMainMenuDlg->GetMainDialog(CDnMainMenuDlg::ITEM_COSTUME_RANDOMMIX_DIALOG));
}

#ifdef PRE_MOD_COSRANDMIX_NPC_CLOSE
void CDnInterface::ResetCostumeRandomMixDlg()
{
	if (IsShowCostumeRandomMixDlg())
	{
		CDnCostumeRandomMixDlg* pDlg = GetCostumeRandomMixDlg();
		if (pDlg)
			pDlg->ClearAndInitializeMix();
	}
}
#endif // PRE_MOD_COSRANDMIX_NPC_CLOSE

#endif

void CDnInterface::ShowCaptionDialog( emCAPTION_TYPE emCaptionType, LPCWSTR wszMsg, DWORD dwColor, float fFadeTime )
{
#ifdef PRE_FIX_SG_NEWLINECHAR
	std::wstring tempStr(wszMsg);
	if (tempStr.length() > 0 && tempStr[tempStr.length() - 1] == '\n')
		tempStr.replace(tempStr.end() - 1, tempStr.end(), L"");

	if( emCaptionType == typeCaption1 )
	{
		if( m_pCaptionDlg_01 ) m_pCaptionDlg_01->SetCaption( tempStr.c_str(), dwColor, fFadeTime );
	}
	else if( emCaptionType == typeCaption2 )
	{
		bool bBottomPos = false;
		if( m_pNpcDlg && m_pNpcDlg->IsShow() ) bBottomPos = true;

		if( m_pCaptionDlg_02 ) m_pCaptionDlg_02->SetCaption( tempStr.c_str(), dwColor, fFadeTime, bBottomPos );
	}
	else if( emCaptionType == typeCaption3 )
	{
		if( m_pCaptionDlg_03 ) m_pCaptionDlg_03->AddCaption( tempStr.c_str(), dwColor, fFadeTime );
	}
	else if( emCaptionType == typeCaption4 )
	{
		if( m_pCaptionDlg_04 ) m_pCaptionDlg_04->SetCaption( tempStr.c_str(), dwColor, fFadeTime , m_pCaptionDlg_04->SecondCaption );
	}
	else if( emCaptionType == typeCaption5 )
	{
		if( m_pCaptionDlg_05 ) m_pCaptionDlg_05->AddCaption(tempStr.c_str(), dwColor);
	}
	else if( emCaptionType == typeCaption6 )
	{
		if( m_pCaptionDlg_06 ) m_pCaptionDlg_06->SetCaption( tempStr.c_str(), dwColor, fFadeTime );
	}
	else
		ASSERT(0&&"CDnInterface::ShowCaptionDialog");
#else // PRE_FIX_SG_NEWLINECHAR
	if( emCaptionType == typeCaption1 )
	{
		if( m_pCaptionDlg_01 ) m_pCaptionDlg_01->SetCaption( wszMsg, dwColor, fFadeTime );
	}
	else if( emCaptionType == typeCaption2 )
	{
		bool bBottomPos = false;
		if( m_pNpcDlg && m_pNpcDlg->IsShow() ) bBottomPos = true;

		if( m_pCaptionDlg_02 ) m_pCaptionDlg_02->SetCaption( wszMsg, dwColor, fFadeTime, bBottomPos );
	}
	else if( emCaptionType == typeCaption3 )
	{
		if( m_pCaptionDlg_03 ) m_pCaptionDlg_03->AddCaption( wszMsg, dwColor, fFadeTime );
	}
	else if( emCaptionType == typeCaption4 )
	{
		if( m_pCaptionDlg_04 ) m_pCaptionDlg_04->SetCaption( wszMsg , dwColor, fFadeTime , m_pCaptionDlg_04->SecondCaption );
	}
	else if( emCaptionType == typeCaption5 )
	{
		if( m_pCaptionDlg_05 ) m_pCaptionDlg_05->AddCaption(wszMsg, dwColor);
	}
	else if( emCaptionType == typeCaption6 )
	{
		if( m_pCaptionDlg_06 ) m_pCaptionDlg_06->SetCaption( wszMsg, dwColor, fFadeTime );
	}
	else
		ASSERT(0&&"CDnInterface::ShowCaptionDialog");
#endif // PRE_FIX_SG_NEWLINECHAR
}

void CDnInterface::ShowCaptionDialog( emCAPTION_TYPE emCaptionType, int nIndex, DWORD dwColor, float fFadeTime )
{
	ShowCaptionDialog( emCaptionType, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nIndex ), dwColor, fFadeTime );
}

#ifdef PRE_ADD_NEWCOMEBACK
void CDnInterface::ShowCaptionDialog2( emCAPTION_TYPE emCaptionType, std::vector< int > & vecStrIdx, DWORD dwColor, float fFadeTime )
{
	if( emCaptionType == typeCaption7 && m_pCaptionDlg_07 )
	{		
		std::vector< const wchar_t * > vecStr( vecStrIdx.size() );
		int size = (int)vecStr.size();
		for( int i=0; i<size; ++i )
			vecStr[ i ] = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, vecStrIdx[ i ] );
		m_pCaptionDlg_07->SetCaption( vecStr, dwColor, fFadeTime );
		vecStr.clear();
	}
}
#endif // PRE_ADD_NEWCOMEBACK

void CDnInterface::CloseCaptionDialog( emCAPTION_TYPE emCaptionType )
{
	if( emCaptionType == typeCaption1 )
	{
		if( m_pCaptionDlg_01 ) m_pCaptionDlg_01->CloseCaption();
	}
	else if( emCaptionType == typeCaption2 )
	{
		if( m_pCaptionDlg_02 ) m_pCaptionDlg_02->CloseCaption();
	}
	else if( emCaptionType == typeCaption3 )
	{
		if( m_pCaptionDlg_03 ) m_pCaptionDlg_03->CloseCaption();
	}
	else if( emCaptionType == typeCaption4 )
	{
		if( m_pCaptionDlg_04 ) m_pCaptionDlg_04->CloseCaption();
	}
	else if (emCaptionType == typeCaption5)
	{
		if (m_pCaptionDlg_05)
			m_pCaptionDlg_05->CloseCaption();
	}
#ifdef PRE_ADD_NEWCOMEBACK
	else if( emCaptionType == typeCaption7 ) // PRE_ADD_NEWCOMEBACK
	{
		if( m_pCaptionDlg_07 )
			m_pCaptionDlg_07->CloseCaption();
	}
#endif // PRE_ADD_NEWCOMEBACK
	else
		ASSERT(0&&"CDnInterface::CloseCaptionDialog");
}

void CDnInterface::CloseCaptionDialog()
{
	CloseCaptionDialog( typeCaption1 );
	CloseCaptionDialog( typeCaption2 );
	CloseCaptionDialog( typeCaption3 );
	CloseCaptionDialog( typeCaption4 );
	CloseCaptionDialog( typeCaption5 );
}

void CDnInterface::AddSlideCaptionMessage( LPCWSTR wszMsg, DWORD dwColor, float fShowTime )
{
	// 현재 서버에서 스트링 길때 스트링버퍼에 아무 값 안넣고 보내는 경우가 있어서, 임시로 체크하겠다.
	if( wszMsg[0] == 20742 && wszMsg[1] == 20742 && wszMsg[2] == 20742 && wszMsg[3] == 20742 )
		return;

	if( m_pSlideCaptionDlg )
	{
		m_pSlideCaptionDlg->AddCaption( wszMsg, dwColor, fShowTime );
	}
}

void CDnInterface::CloseSlideCaptionMessage( bool bForce )
{
	if( m_pSlideCaptionDlg ) m_pSlideCaptionDlg->CloseCaption( bForce );
}

void CDnInterface::DelSlideCaptionMessage( LPCWSTR wszMsg )
{
	if( m_pSlideCaptionDlg ) m_pSlideCaptionDlg->DelCaption( wszMsg );
}

void CDnInterface::DelSlideCaptionMessage( int nIndex )
{
	if( m_pSlideCaptionDlg ) m_pSlideCaptionDlg->DelCaption( nIndex );
}

void CDnInterface::AddGameTipMessage( LPCWSTR wszMsg, DWORD dwColor, float fShowTime )
{
	if( m_pGameTipDlg ) m_pGameTipDlg->AddCaption( wszMsg, dwColor, fShowTime );
}

void CDnInterface::CloseGameTipMessage()
{
	if( m_pGameTipDlg ) m_pGameTipDlg->CloseCaption( false );
}

void CDnInterface::ShowSkillAlarmDialog( LPCWSTR wszMsg, DnSkillHandle hSkill, DWORD dwColor, float fFadeTime )
{
	if( m_pItemAlarmDlg )
	{
		m_pItemAlarmDlg->SetSkillAlarm( wszMsg, hSkill, dwColor, fFadeTime );
		m_pItemAlarmDlg->Show( true );
	}
	
	if( m_pMainBar ) m_pMainBar->BlinkMenuButton( CDnMainMenuDlg::SKILL_DIALOG );
}
void CDnInterface::ShowItemAlarmDialog( LPCWSTR wszMsg, CDnItem *pItem, int nCount, DWORD dwColor, float fFadeTime )
{
	if( m_pItemAlarmDlg )
	{
		// 아이템이 있다면 아이템의 ShowAlarm속성을 확인하고,
		// 아이템이 없으면 그냥 글만 출력하는 걸로 생각하고 NULL을 넘긴다.
		if( (pItem && pItem->IsShowAlarmDialog()) || (pItem == NULL) )
		{
			m_pItemAlarmDlg->SetItemAlarm( wszMsg, pItem, nCount, dwColor, fFadeTime );
			m_pItemAlarmDlg->Show( true );
		}
	}

	if( m_pMainBar ) m_pMainBar->BlinkMenuButton( CDnMainMenuDlg::INVENTORY_DIALOG );

	// 알람이 뜨는 것과 New표시가 뜨는 건 이제 완전히 다르게 되었다. 함께 처리하지 않는다.
	//pItem->SetNewGain( true );
}

void CDnInterface::ShowSymbolAlarmDialog( LPCWSTR wszMsg, CDnItem *pItem, int nCount, DWORD dwColor, float fFadeTime )
{
	if( m_pItemAlarmDlg ) 
	{
		m_pItemAlarmDlg->SetSymbolAlarm( wszMsg, pItem, nCount, dwColor, fFadeTime );
		m_pItemAlarmDlg->Show( true );
	}

	if( m_pMainBar ) m_pMainBar->BlinkMenuButton( CDnMainMenuDlg::INVENTORY_DIALOG );

	AddMessageItem( pItem->GetName(), nCount );
}

void CDnInterface::ShowPlantAlarmDialog(std::vector<CDnItem *> & vPlant, DWORD dwColor, float fFadeTime )
{
	if( m_pItemAlarmDlg )
	{
		m_pItemAlarmDlg->SetPlantAlarm( vPlant, dwColor, fFadeTime );
		m_pItemAlarmDlg->Show( true );
	}

	for( int itr = 0; itr < (int)vPlant.size(); ++itr )
		AddMessageItem( vPlant[itr]->GetName(), 1 );
}

void CDnInterface::ShowMissionAlarmDialog( CDnMissionTask::MissionInfoStruct *pInfo, float fFadeTime )
{
	if( pInfo->bAchieve ) {
		if( m_pMissionAchieveAlarmDlg )
		{
			m_pMissionAchieveAlarmDlg->SetMissionAlarm( pInfo, fFadeTime );
			m_pMissionAchieveAlarmDlg->Show( true );
		}
	}
	else {
		if( m_pMissionGainAlarmDlg )
		{
			m_pMissionGainAlarmDlg->SetMissionAlarm( pInfo, fFadeTime );
			m_pMissionGainAlarmDlg->Show( true );
		}
	}

	if( m_pMainBar ) m_pMainBar->BlinkMenuButton( CDnMainMenuDlg::MISSION_DIALOG );
}

#ifdef PRE_ADD_INSTANT_CASH_BUY
void CDnInterface::ShowInstantCashShopBuyDlg( bool bShow, eInstantCashShopBuyType eBuyType, CEtUICallback* pParentCallback )
{
	if( m_pDnInstantCashShopBuyDlg )
	{
		if( bShow )
		{
			if( m_pDnInstantCashShopBuyDlg->SetBuyType( eBuyType, pParentCallback ) )
				m_pDnInstantCashShopBuyDlg->Show( bShow );
		}
		else
		{
			m_pDnInstantCashShopBuyDlg->Show( bShow );
		}
	}
}
#endif // PRE_ADD_INSTANT_CASH_BUY

#ifdef PRE_ADD_DIRECT_BUY_UPGRADEITEM
void CDnInterface::SetDirectBuyUpgradeItemDlg(SCMarketMiniList* pPacket)
{
	if(m_pMarketDirectBuyDlg)
		m_pMarketDirectBuyDlg->RecvOpenMarketPacket(pPacket);
}
#endif

void CDnInterface::ShowMapCaption( int nMapID )
{
	if( nMapID < 1 )
	{
		ASSERT(0&&"CDnInterface::ShowMapCaption");
		return;
	}

	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TMAP );
	if( !pSox ) return;

	int nMapNameID = pSox->GetFieldFromLablePtr( nMapID, "_MapNameID" )->GetInteger();

	ShowCaptionDialog( typeCaption2, nMapNameID, textcolor::ORANGE );


#ifdef PRE_ADD_CASHSHOPBANNER_RENEWAL
	ShowIngameBanner( true );
#endif // PRE_ADD_CASHSHOPBANNER_RENEWAL

#ifdef PRE_ADD_STAMPSYSTEM
	CEtUIDialog * pDlg = GetMainMenuDialog( CDnMainMenuDlg::INTEGRATE_EVENT_DIALOG );
	if( pDlg )
	{
		if( m_pStampEventAlarmDlg )
			m_pStampEventAlarmDlg->SetAlarmState( false, true );		
	}
#endif // PRE_ADD_STAMPSYSTEM

#ifdef PRE_ADD_COMEBACK

#ifdef PRE_ADD_DWC
	// DWC캐릭은 귀환자 캡션을 보지 않는다
	if( m_bComeback && GetDWCTask().IsDWCChar() == false )
#else // PRE_ADD_DWC
	if( m_bComeback )
#endif // PRE_ADD_DWC
	{
		m_bComeback = false;

#ifdef PRE_ADD_NEWCOMEBACK
		std::vector< int > vecStrIdx;
		vecStrIdx.push_back( 7884 ); // "★★용자님의 귀환을 환영합니다!★★"
		vecStrIdx.push_back( 4949 ); // "지금 바로 ' I '키를 눌러 인벤토리 창을 열고 캐시 탭을 확인해 보세요!"
		GetInterface().ShowCaptionDialog2( CDnInterface::emCAPTION_TYPE::typeCaption7, vecStrIdx, textcolor::YELLOW ); 
		vecStrIdx.clear();
		// "지금 바로 ' I '키를 눌러 인벤토리 창을 열고 캐시 탭을 확인해 보세요!"
		if( CDnActor::s_hLocalActor )
		{
			CDnLocalPlayerActor * pLocalActor = dynamic_cast< CDnLocalPlayerActor * >( CDnActor::s_hLocalActor.GetPointer() );
			if( pLocalActor )
				pLocalActor->ShowComebackUserEffect();

			int BGMIndex = CEtSoundEngine::GetInstance().LoadSound( CEtResourceMng::GetInstance().GetFullName( "DRAGON NEST_BGM_Cashshop.ogg" ).c_str(), false, true );
			EtSoundChannelHandle hBGM = CEtSoundEngine::GetInstance().PlaySound__("BGM", BGMIndex, true, true);
		}

#else
		const wchar_t * str = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 7739);
		GetInterface().ShowCaptionDialog( CDnInterface::emCAPTION_TYPE::typeCaption2, 
								          str, textcolor::YELLOW ); // "복귀 유저 보상이 주어졌습니다. 지금 우편함 혹은 캐시샵 선물함을 확인해 주세요."

		GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", str, false );

#endif // PRE_ADD_NEWCOMEBACK

		CDnBridgeTask *pTask = (CDnBridgeTask *)CTaskManager::GetInstance().GetTask( "BridgeTask" );
		pTask->SetComeback( false );
	}
#endif

#ifdef PRE_ADD_GAMEQUIT_REWARD
	ShowNewbieGameQuitRewardMessage();	
#endif // PRE_ADD_GAMEQUIT_REWARD

}

#ifdef PRE_ADD_GAMEQUIT_REWARD
void CDnInterface::ShowNewbieGameQuitRewardMessage()
{
	CTask * pTask = CTaskManager::GetInstance().GetTask( "BridgeTask" );
	if( pTask )
	{
		CDnBridgeTask * pBridgeTask = (CDnBridgeTask *)pTask;
		if( pBridgeTask->GetNewbieRewardMessage() )
		{
			pTask = CTaskManager::GetInstance().GetTask( "CommonTask" );
			if( pTask )
				GetInterface().MessageBox( 7957, MB_OK, 7957, static_cast<CDnCommonTask*>(pTask) );
			pBridgeTask->EndNewbieRewardMessage();
		}
	}
}
#endif // PRE_ADD_GAMEQUIT_REWARD

void CDnInterface::FadeDialog( DWORD srcColor, DWORD destColor, float fFadeTime, CEtUICallback *pCallback, bool bForceApplySrcColor )
{
	ClearTextureWindow();
	ClearAlarmWindow();
	EndCountDown();
	CloseCaptionDialog();
//	CloseNpcDialog();
	if( m_pNpcDlg )
	{
		// 만약 npcDlg가 보여진 상태라면, 아마도 npcDlg에서 맵이동 선택지를 선택해 이동되는 형태일거다.
		// 이런 상황에선 다음번에 오는 CDnNpcDlg::OnBlindOpened에서 Show되지 않도록 플래그 처리를 해둔다.
		if( m_pNpcDlg->IsShow() )
			m_pNpcDlg->HideNextBlindOpened();
		m_pNpcDlg->Close();
	}
	if( m_pDungeonOpenNoticeDlg ) m_pDungeonOpenNoticeDlg->Show( false );

	// npc대화도중에 맵이동 시키는 경우때문에 블라인드 콜백에서 npcDlg를 제거시킨다.
	// 전부 Clear해버리면 CDnNotifyArrowDialog 도 빠질때가 있어서
	// 블라인드 닫힌 후 보여야할 화면가운데 화살표가 보이지 않게 된다. 그래서 npcDlg하나만 제거해야한다.
	//
	// 아무래도 이렇게 직접 Remove하니 CDnNpcDlg::OnBlindClosed 함수가 호출되지 않으면서 문제가 많이 발생한다.
	// 이렇게 처리하지 않고 위에서 npcDlg 상태를 보고 처리하는 걸로 하겠다.
	//if( m_pBlindDlg && m_pNpcDlg ) m_pBlindDlg->RemoveBlindCallback( m_pNpcDlg );

	m_pFadeDlg->SetCallback( pCallback );

	m_pFadeDlg->SetFadeColor( srcColor, destColor, bForceApplySrcColor );
	m_pFadeDlg->SetFadeTime( fFadeTime );
	m_pFadeDlg->SetFadeMode( CDnFadeInOutDlg::modeBegin );

	// 채널이동 등에 대해선 처리했지만, 그 외 경우엔 Fade에서 처리하려 했다.
	// 그런데 이상하게 페이드 중에 가는 메세지들에 대해서 제대로 패킷이 안가는거 같아,
	// 채널이동, 빌리지에서 게임시작다이얼로그 등에서 직접 처리하기로 했다.
	// 그래도 혹시 모르니, 처리.
	if( CDnTradeTask::IsActive() ) GetTradeTask().GetTradePrivateMarket().ClearTradeUserInfoList();
#ifdef PRE_MOD_INTEG_SYSTEM_STATE
	if( CDnPartyTask::IsActive() ) GetPartyTask().ClearInviteInfoList(true);
#else
	if( CDnPartyTask::IsActive() ) GetPartyTask().ClearInviteInfoList();
#endif
#ifdef PRE_FIX_QUICKPVP_EXCLUSIVEREQ
	CDnCommonTask *pTask = static_cast<CDnCommonTask*>(CTaskManager::GetInstance().GetTask( "CommonTask" ));
	if (pTask)
		pTask->SetRequestingQuickPvP(false);
#endif

	if (srcColor == 0x00000000 && destColor == 0xff000000) // fade out
	{
		ForceCloseProgressDialog();
		DisableCashShopMenuDlg(true);
	}
}

void CDnInterface::FadeForBlow( float fFadeTime, DWORD dwColor )
{
	if( !m_pFadeForBlowDlg ) return;
	m_pFadeForBlowDlg->SetFadeForBlow( fFadeTime, dwColor );
}

void CDnInterface::OpenAssert( const wchar_t *szMsg, CEtUICallback *pCallback )
{
	if( !m_pAssertDlg ) return;
	m_pAssertDlg->SetInfo( m_Type, szMsg, pCallback );
	m_pAssertDlg->Show(true);
}

void CDnInterface::OnDamage( DnActorHandle hActor )
{	

	if( CDnActor::s_hLocalActor == hActor )
	{
		if( m_pPlayerGaugeDlg )
		{
			m_pPlayerGaugeDlg->OnDamage();
		}
		return;
	}

	// PVP에서는 처리하지 않는다 
	if( !IsPVP() && !IsFarm() )
	{
		for( int i=0; i<(int)m_pVecPartyPlayerGaugeList.size(); i++ )
		{
			if( m_pVecPartyPlayerGaugeList[i].hActor == hActor )
			{
				if( m_pVecPartyPlayerGaugeList[i].pGaugeDlg )
				{
					m_pVecPartyPlayerGaugeList[i].pGaugeDlg->OnDamage();
				}
				return;
			}
		}
	}

	for( int i=0; i<(int)m_pVecEnemyGaugeList.size(); i++ )
	{
		if( m_pVecEnemyGaugeList[i].hActor == hActor )
		{
			if( m_pVecEnemyGaugeList[i].pGaugeDlg )
			{
				m_pVecEnemyGaugeList[i].pGaugeDlg->OnDamage();
			}
			return;
		}
	}
}

void CDnInterface::SetDamage( EtVector3 vPos, int nDamage, bool bCritical, bool bCriticalRes, bool bEnemy, bool bMine, int hasElement, bool bStageLimit )
{
	if( nDamage <= 0 ) return;
	if( !m_pDamageMng ) return;
	if( !EtInterface::g_bEtUIRender ) return;

	m_pDamageMng->SetDamage( vPos, nDamage, bEnemy, bMine, bCritical, hasElement, bStageLimit );

#ifndef _FINAL_BUILD
	if(CGlobalValue::GetInstance().m_bShowDPS && bMine)
	{
		CGlobalValue::GetInstance().m_nSumDPS += nDamage;
	}
#endif


	if( bCriticalRes && bMine )
	{
		if( CGameOption::GetInstance().bMyCombatInfo ) {
			m_pDamageMng->SetCriticalRes( vPos );
		}		
	}
}

#ifdef PRE_ADD_CRAZYDUC_UI
void CDnInterface::SetCrazyDucScore( EtVector3& vPos, int nMonsterID  )
{
	if( nMonsterID <= 0 ) return;
	if( !m_pScoreMng ) return;
		
	DNTableFileFormat * pEventMonsterTable = GetTableDB().GetTable( CDnTableDB::TEVENTMONSTER ); 
	if( pEventMonsterTable && ! pEventMonsterTable->IsExistItem(nMonsterID ) ) 
		return;
	
	int nScore = pEventMonsterTable->GetFieldFromLablePtr( nMonsterID, "_CompleteExperience" )->GetInteger();
	const int nValue = 900190; 

	m_pScoreMng->SetScore( vPos, nScore, nMonsterID - nValue ); 
}
#endif

void CDnInterface::SetCpUp( int nValue )
{
	m_nAccumulationCPCache += nValue;
	//OutputDebug("[CLEAR POINT] CpUp:%d\n", nValue);
}

void CDnInterface::SetCpJudge(eCpJudgeType type, int nValue)
{
	//OutputDebug("[CLEAR POINT] judgeType:%d value:%d\n", type, nValue);
	if (IsCpShow(type) == false)
	{
		SetCpUp(nValue);
	}
	else
	{
		if( !m_pCpJudgeMng ) return;
		m_pCpJudgeMng->SetCpJudge(type, nValue);
	}
}

void CDnInterface::SetCpDown(int nValue)
{
	if (m_pCpDownMng == NULL || m_pCpScoreMng == NULL)	return;
	m_pCpDownMng->SetCpDown(nValue);
	m_nAccumulationCPCache += nValue;
	if (m_nAccumulationCPCache < 0)
		m_nAccumulationCPCache = 0;

	m_pCpScoreMng->OnCpDown();
}

void CDnInterface::ResetCp()
{
	m_nAccumulationCPCache = 0;
}

void CDnInterface::MakeCpShowList()
{
	int i = 0;
	for (; i < CpType_MAX; ++i)
	{
		if ((eCpJudgeType)i == CpType_None ||
			(eCpJudgeType)i == CpType_SkillScore ||
			(eCpJudgeType)i == CpType_Combo
			|| (eCpJudgeType)i == CpType_AssistKillMonster
			)
		{
			continue;
		}

		m_CpShowList.push_back((eCpJudgeType)i);
	}
}

bool CDnInterface::IsCpShow(eCpJudgeType type) const
{
	std::vector<eCpJudgeType>::const_iterator iter = m_CpShowList.begin();
	for (; iter != m_CpShowList.end(); ++iter)
	{
		const eCpJudgeType& judge = *iter;
		if (judge == type)
			return true;
	}
	return false;
}

SUICoord CDnInterface::GetCpScoreDlgCoord() const
{
	return m_pCpScoreMng->GetCpScoreDlgCoord();
}

SUICoord CDnInterface::GetCpScoreDlgBaseCoord() const
{
	return m_pCpScoreMng->GetCpScoreDlgBaseCoord();
}

void CDnInterface::SetRecovery( EtVector3 vPos, int nHP, int nSP, bool bEnemy, bool bMine )
{
	if( !m_pDamageMng ) return;
	m_pDamageMng->SetRecovery( vPos, nHP, nSP, bEnemy, bMine );
#ifdef _RDEBUG
	g_Log.Log(LogType::_FILELOG, "[DMGERR] CDnInterface::SetRecovery() : vPos(%f, %f, %f) / nHP(%d) / nSP(%d) / enemy(%d) / mine(%d)\n", vPos.x, vPos.y, vPos.z, nHP, nSP, bEnemy, bMine);
#endif
}

void CDnInterface::SetResist( EtVector3 vPos )
{
	if( !m_pDamageMng ) return;
	m_pDamageMng->SetResist( vPos );
}

#ifdef PRE_ADD_DECREASE_EFFECT
void CDnInterface::SetDecreaseEffect( EtVector3 vPos )
{
	if( !m_pDamageMng ) return;
	m_pDamageMng->SetDecreaseEffect( vPos );
}
#endif // PRE_ADD_DECREASE_EFFECT

void CDnInterface::SetCombo( int nCombo, int nDelay )
{
	if( nCombo <= 0 ) return;
#if defined(_US) || defined(_SG) || defined(_TH) || defined(_ID) || defined(_RU)// #47229 _TH 추가. 태국도 1 Combo X 추가합니다. #57019 _ID 추가
	if( nCombo <= 1 ) return;	// #30115 미국, 싱가폴은 1 Combo 하지 말라고 한다.
#endif
	if( !m_pDamageMng ) return;

	m_pComboMng->SetCombo( nCombo, nDelay );
}

void CDnInterface::SetChain( int nChain )
{
	//if( nChain <= 0 ) return;
	//m_pChainMng->SetChain( nChain );
}

void CDnInterface::ClearServerList()
{
	if( !m_pServerListDlg ) return;
	m_pServerListDlg->ClearServerList();
}

void CDnInterface::AddServerList( int nIndex, LPCWSTR szServerName, float fUserRatio, int nCharCount )
{
	if( !m_pServerListDlg ) return;
	m_pServerListDlg->AddServerList( nIndex, szServerName, fUserRatio, nCharCount );

#ifdef PRE_WORLDCOMBINE_PARTY
	AddServerListToCache(nIndex, szServerName);
#endif
}

#ifdef PRE_WORLDCOMBINE_PARTY
void CDnInterface::AddServerListToCache(int nIndex, LPCWSTR szServerName)
{
	std::map<int, std::wstring>::iterator iter = m_ServerListCache.find(nIndex);
	if (iter == m_ServerListCache.end())
		m_ServerListCache.insert(std::make_pair(nIndex, std::wstring(szServerName)));
}

void CDnInterface::GetServerNameByIndex(std::wstring& serverName, int nIndex) const
{
	std::map<int, std::wstring>::const_iterator iter = m_ServerListCache.find(nIndex);
	if (iter != m_ServerListCache.end())
		serverName = (*iter).second;
}
#endif

#ifndef PRE_MOD_SELECT_CHAR
std::wstring CDnInterface::GetServerName( int nIndex )
{
	if( !m_pServerListDlg ) return std::wstring(L"");
	return m_pServerListDlg->GetServerName( nIndex );
}

int CDnInterface::GetServerIndex( LPCWSTR szServerName )
{
	if( !m_pServerListDlg ) return -1;
	return m_pServerListDlg->GetServerIndex( szServerName );
}
#endif // PRE_MOD_SELECT_CHAR

void CDnInterface::AddChannelList( sChannelInfo *pChannelInfo )
{
	if( GetInterfaceType() == CDnInterface::Login )
	{
		if( !m_pChannelListDlg ) return;
		m_pChannelListDlg->AddChannelList( pChannelInfo );
	}
	else
	{
		if( !m_pMainMenuDlg ) return;
		m_pMainMenuDlg->AddChannelList( pChannelInfo );
	}
}

//blondy
void CDnInterface::AddLobbyList( sRoomInfo * RoomInfo , UINT uiMaxPage)
{
	if( m_pPVPRoomListDlg )
		m_pPVPRoomListDlg->AddLobbyList( RoomInfo , uiMaxPage );
}

void CDnInterface::RefreshRoomList()
{
	if( m_pPVPRoomListDlg )
		m_pPVPRoomListDlg->RefreshRoomList();
}

void CDnInterface::SetFirstRoom( bool isFirst )
{
	if( m_pPVPRoomListDlg )
	{
		m_pPVPRoomListDlg->ClearvecRoomInfo(); 
		m_pPVPRoomListDlg->SetFirstRoom( isFirst );
	}
}
//blondy end


void CDnInterface::SetDefaultServerList()
{
	if( !m_pServerListDlg ) return;
	m_pServerListDlg->SetDefaultList();
}

void CDnInterface::SetDefaultChannelList()
{
	if( GetInterfaceType() == CDnInterface::Login )
	{
		if( !m_pChannelListDlg ) return;
		m_pChannelListDlg->SetDefaultList();
	}
	else
	{
		if( !m_pMainMenuDlg ) return;
		CDnSystemDlg *pSystemDlg = (CDnSystemDlg*)m_pMainMenuDlg->GetMainDialog( CDnMainMenuDlg::SYSTEM_DIALOG );
		if( pSystemDlg )
		{
			pSystemDlg->SetDefaultList();
		}
	}
}

void CDnInterface::OpenDirectDialog( DWORD dwSessionID, int nUserLevel, const wchar_t *wszJobName, const wchar_t *wszName )
{
	m_pDirectDlg->SetInfo( dwSessionID, nUserLevel, wszJobName, wszName );
	m_pDirectDlg->Show( true );	
}

void CDnInterface::SetDirectDialogPermission( UINT nSessionID, char *pCommunityOption, TPARTYID PartyID )
{
	m_pDirectDlg->SetPermission(nSessionID, pCommunityOption, PartyID);
}

void CDnInterface::CloseDirectDialog()
{
	if( m_pDirectDlg ) m_pDirectDlg->Show( false );
}

void CDnInterface::OpenInspectPlayerInfoDialog( DWORD dwSessionID )
{ 
	m_pInspectPlayerDlg->Show(true); 
	m_pInspectPlayerDlg->SetSessionID( dwSessionID );
}

void CDnInterface::CloseInspectPlayerInfoDialog()
{ 
	if( m_pInspectPlayerDlg ) m_pInspectPlayerDlg->Show(false);
}

void CDnInterface::ShowGuildMgrBox( int nGuildMgrNo, bool bShow )
{
	if( nGuildMgrNo == GuildMgrBox_Amount && bShow == false ) {
		// 모두 닫는다.
		ShowGuildCreateDlg( false );
		ShowGuildDismissDlg( false );
		ShowGuildLeaveDlg( false );
		ShowGuildWarEnrollDlg( false );
		return;
	}

	// 스크립트에서 호출하는 길드NPC 다이얼로그 번호는 아래와 같다.
	// 0 : 길드창설
	// 1 : 길드해산
	// 2 : 길드탈퇴
	// 3 : 길드포인트. 없어져서 안씀. 값만 유지.
	// 4 : 길드레벨업. 없어져서 안씀. 값만 유지.
	// 5 : 길드전신청
	switch( nGuildMgrNo )
	{
	case 0:	ShowGuildCreateDlg( bShow );	break;
	case 1:	ShowGuildDismissDlg( bShow );	break;
	case 2:	ShowGuildLeaveDlg( bShow );		break;
	case 5:	ShowGuildWarEnrollDlg( bShow );	break;
	}
}

void CDnInterface::ShowGuildCreateDlg( bool bShow )
{
	if( !m_pGuildCreateDlg ) return;
	m_pGuildCreateDlg->Show( bShow );
	if( bShow ) OpenNpcTalkReturnDlg();
}

void CDnInterface::ShowGuildDismissDlg( bool bShow )
{
	if( !m_pGuildYesNoDlg ) return;
	m_pGuildYesNoDlg->SetGuildYesNoType( CDnGuildYesNoDlg::GuildYesNo_Dismiss );
	m_pGuildYesNoDlg->Show( bShow );
	if( bShow ) OpenNpcTalkReturnDlg();
}

void CDnInterface::ShowGuildLeaveDlg( bool bShow )
{
	if( !m_pGuildYesNoDlg ) return;
	m_pGuildYesNoDlg->SetGuildYesNoType( CDnGuildYesNoDlg::GuildYesNo_Leave );
	m_pGuildYesNoDlg->Show( bShow );
	if( bShow ) OpenNpcTalkReturnDlg();
}

void CDnInterface::ShowGuildWarEnrollDlg( bool bShow )
{
	if( !m_pGuildYesNoDlg ) return;
	m_pGuildYesNoDlg->SetGuildYesNoType( CDnGuildYesNoDlg::GuildYesNo_GuildWarEnroll );
	m_pGuildYesNoDlg->Show( bShow );
	if( bShow ) OpenNpcTalkReturnDlg();
}

void CDnInterface::ShowGuildWarFinalWinNoticeDlg( bool bShow, const std::wstring strMsg )
{
	if( m_pDnGuildWarFinalWinNoticeDlg )
	{
		m_pDnGuildWarFinalWinNoticeDlg->SetFianlWinNoticeMsg( strMsg );
		m_pDnGuildWarFinalWinNoticeDlg->Show( bShow );
	}
}

bool CDnInterface::IsShowGuildMgrBox()
{
	if( m_pGuildCreateDlg && m_pGuildCreateDlg->IsShow() ) return true;
	if( m_pGuildYesNoDlg && m_pGuildYesNoDlg->IsShow() ) return true;
	return false;
}

void CDnInterface::OpenGuildInviteReqDlg( LPCWSTR wszGuildName, LPCWSTR wszCharacterName, float fTime, int nID, CEtUICallback *pCall )
{
	if( !m_pGuildInviteReqDlg ) return;
	m_pGuildInviteReqDlg->SetInfo( wszGuildName, wszCharacterName, fTime, nID, pCall );
	m_pGuildInviteReqDlg->SetElapsedTime( fTime );
	m_pGuildInviteReqDlg->Show( true );
}

void CDnInterface::CloseGuildInviteReqDlg( bool bSendReject )
{
	if( !m_pGuildInviteReqDlg ) return;
	if( bSendReject ) m_pGuildInviteReqDlg->SendReject();
	m_pGuildInviteReqDlg->Show( false );
}

bool CDnInterface::IsShowGuildInviteReqDlg()
{
	if( m_pGuildInviteReqDlg && m_pGuildInviteReqDlg->IsShow() ) return true;
	return false;
}

void CDnInterface::ShowGuildLevelUpAlarmDlg( bool bShow, int nLevel, float fFadeTime )
{
	if( !m_pGuildLevelUpAlarmDlg ) return;
	if( bShow ) m_pGuildLevelUpAlarmDlg->SetInfo( nLevel, fFadeTime );
	m_pGuildLevelUpAlarmDlg->Show( bShow );
}

void CDnInterface::OpenPrivateChatDialog( LPCWSTR wszPrivateName )
{
	if( !m_pPrivateChatDlg ) return;
	m_pPrivateChatDlg->SetPrivateName( wszPrivateName );
	m_pPrivateChatDlg->Show(true);
}

void CDnInterface::OpenGuildChatDialog( LPCWSTR wszGuildName )
{
	if( !m_pPrivateChatDlg ) return;
	m_pPrivateChatDlg->SetGuildName( wszGuildName );
	m_pPrivateChatDlg->Show(true);
}

void CDnInterface::OpenStorageDialog( int iRemoteItemID, bool bGuildStorageOpened /*= false*/ )
{
	if( !m_pMainMenuDlg ) return;
	m_pMainMenuDlg->Show( true );
	m_pMainMenuDlg->OpenStorageDialog( iRemoteItemID, bGuildStorageOpened );

	OpenNpcTalkReturnDlg();
}

#ifdef PRE_ADD_PVPRANK_INFORM
void CDnInterface::OpenStoreDialog(int nType, int nShopID)
#else
void CDnInterface::OpenStoreDialog(int nType)
#endif
{
	if( !m_pMainMenuDlg ) return;

	// 거래 제재 검사
	if( GetRestraintTask().CheckRestraint( _RESTRAINTTYPE_TRADE, true )
#ifdef PRE_ADD_DWC
		&& GetDWCTask().IsDWCChar() == false
#endif
		)
	{
		return;
	}

	m_pMainMenuDlg->Show( true );
#ifdef PRE_ADD_PVPRANK_INFORM
	m_pMainMenuDlg->OpenStoreDialog(nType, nShopID);
#else
	m_pMainMenuDlg->OpenStoreDialog(nType);
#endif
	
	OpenNpcTalkReturnDlg();
}

void CDnInterface::OpenMailDialog(bool bWrite, const std::wstring& receiver, const std::wstring& title)
{
	if (!m_pMainMenuDlg)
		return;
	m_pMainMenuDlg->Show(true);
	m_pMainMenuDlg->OpenMailDialog(bWrite, receiver.c_str(), title.c_str());

	OpenNpcTalkReturnDlg();
}

void CDnInterface::OpenGlyphLiftDialog()
{
	if( !m_pMainMenuDlg )
		return;

	m_pMainMenuDlg->Show( true );
	m_pMainMenuDlg->OpenGlyphLiftDialog();

	OpenNpcTalkReturnDlg();
}

void CDnInterface::OpenInvenDialog()
{
	if( !m_pMainMenuDlg )
		return;

	m_pMainMenuDlg->Show( true );
	m_pMainMenuDlg->OpenInvenDialog();

	OpenNpcTalkReturnDlg();
}

#ifdef PRE_ADD_COMPARETOOLTIP
// #69087
bool CDnInterface::IsOpenInvenDialog()
{
	if( !m_pMainMenuDlg )
		return false;

	return m_pMainMenuDlg->IsOpenDialog( CDnMainMenuDlg::INVENTORY_DIALOG );	
}
#endif // PRE_ADD_COMPARETOOLTIP

void CDnInterface::OpenSkillStoreDialog()
{
	if( !m_pMainMenuDlg ) return;

	m_pMainMenuDlg->Show( true );
	m_pMainMenuDlg->OpenSkillStoreDialog();

	OpenNpcTalkReturnDlg();
}

void CDnInterface::OpenMarketDialog()
{
	if( !m_pMainMenuDlg ) return;

	// 거래 제재 검사
	if( GetRestraintTask().CheckRestraint( _RESTRAINTTYPE_TRADE, true ) )
		return;

	if( false == m_pMainMenuDlg->IsOpenDialog( CDnMainMenuDlg::MARKET_DIALOG ) )
	{
		m_pMainMenuDlg->Show( true );
		m_pMainMenuDlg->OpenMarketDialog();
	}
}

void CDnInterface::OpenItemDisjointDialog()
{
	if( !m_pMainMenuDlg ) return;

	// 거래 제재 검사
	if( GetRestraintTask().CheckRestraint( _RESTRAINTTYPE_TRADE, true ) )
		return;

	m_pMainMenuDlg->Show( true );
	m_pMainMenuDlg->OpenItemDisjointDialog();

	OpenNpcTalkReturnDlg();
}

void CDnInterface::OpenPlateMainDialog()
{
	if( !m_pMainMenuDlg ) return;

	// 거래 제재 검사
	if( GetRestraintTask().CheckRestraint( _RESTRAINTTYPE_TRADE, true ) )
		return;

	m_pMainMenuDlg->Show( true );
	m_pMainMenuDlg->OpenPlateMainDialog();

	OpenNpcTalkReturnDlg();
}

void CDnInterface::OpenItemUpgradeDialog( int nRemoteItemID )
{
	if( !m_pMainMenuDlg ) return;

	// 거래 제재 검사
	if( GetRestraintTask().CheckRestraint( _RESTRAINTTYPE_TRADE, true ) )
		return;

	m_pMainMenuDlg->Show( true );
	m_pMainMenuDlg->OpenItemUpgradeDialog( nRemoteItemID );

#ifdef PRE_ADD_EQUIPED_ITEM_ENCHANT
	m_pMainMenuDlg->SetEquippedItemUpgradeDlg();
#endif

	OpenNpcTalkReturnDlg();
}

#ifdef PRE_ADD_EXCHANGE_ENCHANT
void CDnInterface::OpenItemUpgradeExchangeDialog()
{
	if (m_pMainMenuDlg == NULL)
		return;

	// 거래 제재 검사
	if (GetRestraintTask().CheckRestraint(_RESTRAINTTYPE_TRADE, true))
		return;

	m_pMainMenuDlg->Show(true);
	m_pMainMenuDlg->OpenItemUpgradeExchangeDialog();
}
#endif

void CDnInterface::OpenItemCompoundDialog( int nCompoundShopID )
{
	if( !m_pMainMenuDlg ) return;

	// 거래 제재 검사
	if( GetRestraintTask().CheckRestraint( _RESTRAINTTYPE_TRADE, true ) )
		return;

	m_pMainMenuDlg->Show( true );
	m_pMainMenuDlg->OpenItemCompoundDialog( nCompoundShopID );

	OpenNpcTalkReturnDlg();
}

void CDnInterface::OpenItemCompound2Dialog( int nCompoundGroupID, int nRemoteItemID )
{
	if( !m_pMainMenuDlg ) return;

	// 거래 제재 검사
	if( GetRestraintTask().CheckRestraint( _RESTRAINTTYPE_TRADE, true ) )
		return;

	m_pMainMenuDlg->Show( true );
	m_pMainMenuDlg->OpenItemCompound2Dialog( nCompoundGroupID, nRemoteItemID );
}

void CDnInterface::ShowStickAniDialog( bool bShow )
{
	if( !m_pStickAniDlg ) return;
	m_pStickAniDlg->Show(bShow);
}

void CDnInterface::ToggleButtonMouseCursor( bool bMouseCursor )
{
	if( !m_pMainMenuDlg )
		return;

	m_pMainMenuDlg->ToggleButtonCtrl( bMouseCursor?2:1 );
}

void CDnInterface::ToggleButtonBattleMode( bool bBattleMode )
{
	if( !m_pMainMenuDlg )return;
	m_pMainMenuDlg->ToggleButtonZ( bBattleMode?1:2 );
}

void CDnInterface::ClearListDialog()
{
	// Note : 콤보, 체인, 데미지 다이얼로그를 모두 삭제한다.
	//
	if( m_pDamageMng ) m_pDamageMng->Clear();
	if( m_pComboMng ) m_pComboMng->Clear();
	if( m_pChainMng ) m_pChainMng->Clear();
	if( m_pEnemyGaugeMng ) m_pEnemyGaugeMng->Clear();
#ifdef PRE_ADD_CRAZYDUC_UI
	if( m_pScoreMng ) m_pScoreMng->Clear();
#endif 
}

void CDnInterface::ReInitialize()
{
	static void (CDnInterface::*fp[InterfaceTypeMax])() =
	{
		&CDnInterface::ReInitializeTitle,
		&CDnInterface::ReInitializeLogin,
		&CDnInterface::ReInitializeVillage,
		&CDnInterface::ReInitializePVPVillage,
		&CDnInterface::ReInitializeGame,
		&CDnInterface::ReInitializePVPGame,
		&CDnInterface::ReInitializeDLGame,
		&CDnInterface::ReInitializeFarmGame,
		&CDnInterface::ReInitializeCommon,
	};

	EtInterface::g_bEtUIMsgProc = false;
	EtInterface::g_bEtUIProcess = false;
	EtInterface::g_bEtUIRender = false;

	(this->*fp[m_Type])();

	EtInterface::g_bEtUIMsgProc = true;
	EtInterface::g_bEtUIProcess = true;
	EtInterface::g_bEtUIRender = true;
}

void CDnInterface::ReInitializeTitle()
{
}

void CDnInterface::ReInitializeLogin()
{
	FinalizeLogin();

	PostFinalize();
	PreInitialize();

	InitializeLogin( m_pTask );

	ChangeState( m_State );
	CDnMouseCursor::GetInstance().ShowCursor( true );
}

void CDnInterface::ReInitializeVillage()
{
	FinalizeCommon();
	FinalizeVillage();

	PostFinalize();
	PreInitialize();

	InitializeVillage( m_pTask );
	InitializeCommon( m_pTask );

	OpenBaseDialog();
}

void CDnInterface::ReInitializeGame()
{
	FinalizeCommon();
	FinalizeGame();

	PostFinalize();
	PreInitialize();

	InitializeGame( m_pTask );
	InitializeCommon( m_pTask );

	OpenBaseDialog();
}

void CDnInterface::ReInitializeCommon()
{
	FinalizeCommon();

	PostFinalize();
	PreInitialize();

	InitializeCommon( m_pTask );

	OpenBaseDialog();
}

void CDnInterface::ReInitializePVPGame()
{
	FinalizeCommon();
	FinalizeGame();

	PostFinalize();
	PreInitialize();

	InitializePVPGame( m_pTask );
	InitializeCommon( m_pTask );

	OpenBaseDialog();
}

void CDnInterface::ReInitializeDLGame()
{
	FinalizeCommon();
	FinalizeGame();

	PostFinalize();
	PreInitialize();

	InitializeDLGame( m_pTask );
	InitializeCommon( m_pTask );

	OpenBaseDialog();
}

void CDnInterface::ReInitializeFarmGame()
{
	FinalizeCommon();
	FinalizeGame();

	PostFinalize();
	PreInitialize();

	InitializeFarmGame( m_pTask );
	InitializeCommon( m_pTask );

	OpenBaseDialog();
}

void CDnInterface::ReInitializePVPVillage()
{
	FinalizeCommon();
	FinalizeGame();

	PostFinalize();
	PreInitialize();

	InitializePVPVillage( m_pTask );
	InitializeCommon( m_pTask );

	OpenBaseDialog();
}

void CDnInterface::ShowCutSceneBlindDialog( bool bShow )
{
	if( bShow )
	{
		if( m_pNpcDlg && m_pNpcDlg->IsShow() )
		{
			m_bIsOpenNpcDlg = true;
			if( m_pNpcDlg ) m_pNpcDlg->Show( false );
			return;
		}

		ShowMiniMap( false );
		CEtUIDialogBase::CloseAllDialog();

		// 채팅 다이얼로그의 show설정은 다시 true로 되돌린다.
		// 이렇게 해야 ChatTabDlg::ShowEx에서 m_groupTabDialog.GetShowDialog이 가능해진다.
		ShowChatDialog();
	}
	else
	{
		if( m_bIsOpenNpcDlg )
		{
			m_bIsOpenNpcDlg = false;
			if( m_pNpcDlg ) m_pNpcDlg->Show( true );
			return;
		}

		OpenBaseDialog();
	}

	m_pBlindDlg->SetBillBoardColor( 0x00000000 );
	m_pBlindDlg->AddBlindCallBack( ((CDnLocalPlayerActor*)CDnActor::s_hLocalActor.GetPointer()) );
	m_pBlindDlg->Show( bShow );
}

DWORD CDnInterface::DrawTextureWindow( EtTextureHandle hTextureHandle, float fX, float fY, float fDelayTime, DWORD dwSetID, CEtUICallback *pCallback )
{
	if( !m_pTextureDlgMng ) return 0;
	return m_pTextureDlgMng->ShowTextureWindow( hTextureHandle, fX, fY, fDelayTime, dwSetID, pCallback );
}

DWORD CDnInterface::DrawTextureWindow( EtTextureHandle hTextureHandle, float fX, float fY, int nPos, float fDelayTime, DWORD dwSetID, CEtUICallback *pCallback )
{
	if( !m_pTextureDlgMng ) return 0;
	return m_pTextureDlgMng->ShowTextureWindow( hTextureHandle, fX, fY, nPos, fDelayTime, dwSetID, pCallback );
}

#ifdef PRE_ADD_OPEN_QUEST_TEXTURE_DIALOG
DWORD CDnInterface::DrawTextureWindow( EtTextureHandle hTextureHandle, float fX, float fY, int nPos, float fDelayTime, DWORD dwSetID, bool bAutoCloseDialog, CEtUICallback *pCallback)
{
	if( !m_pTextureDlgMng ) return 0;
	return m_pTextureDlgMng->ShowTextureWindow( hTextureHandle, fX, fY, nPos, fDelayTime, dwSetID, bAutoCloseDialog, pCallback);
}

DWORD CDnInterface::ClearNpcTalkTextureWindow()
{
	if( !m_pTextureDlgMng ) return 0;
	m_pTextureDlgMng->ClearNpcTalkTextureWindow();
	return 1;
}
#endif

void CDnInterface::ClearTextureWindow()
{
	if( !m_pTextureDlgMng ) return;
	m_pTextureDlgMng->Clear();
}

void CDnInterface::CloseTextureWindow( DWORD dwID, bool bSetID )
{
	if( !m_pTextureDlgMng ) return;
	m_pTextureDlgMng->CloseDlg( dwID, bSetID );
}

DWORD CDnInterface::ShowAlarmWindow( int nType, float fX, float fY, float fWidth, float fDelayTime, LPCWSTR wszStr, DWORD dwSetID, CEtUICallback *pCallback )
{
	if( !m_pTextBoxDlgMng ) return 0;
	return m_pTextBoxDlgMng->ShowTextWindow( wszStr, fX, fY, fWidth, fDelayTime, dwSetID, pCallback );
}

DWORD CDnInterface::ShowAlarmWindow( int nType, float fX, float fY, int nPos, int nFormat, float fDelayTime, LPCWSTR wszStr, DWORD dwSetID, CEtUICallback *pCallback )
{
	if( !m_pTextBoxDlgMng ) return 0;
	return m_pTextBoxDlgMng->ShowTextWindow( wszStr, fX, fY, nPos, nFormat, fDelayTime, dwSetID, pCallback );
}

DWORD CDnInterface::ShowCountDownAlarmWindow( int nType, float fX, float fY, int nPos, int nFormat, float fDelayTime, int nCountDown, DWORD dwSetID, CEtUICallback *pCallback )
{
	if( !m_pTextBoxDlgMng ) return 0;
	return m_pTextBoxDlgMng->ShowCountDownWindow( nCountDown, fX, fY, nPos, nFormat, fDelayTime, dwSetID, pCallback );
}

void CDnInterface::ClearAlarmWindow()
{
	if( !m_pTextBoxDlgMng ) return;
	m_pTextBoxDlgMng->Clear();
}

void CDnInterface::CloseAlarmWindow( DWORD dwID, bool bSetID )
{
	if( !m_pTextBoxDlgMng ) return;
	m_pTextBoxDlgMng->CloseDlg( dwID, bSetID );
}

void CDnInterface::BeginCountDown( int nSec, LPCWSTR wszString, emCountDownType type )
{
	m_bEnableCountDown	= true;
	m_fCountDownDelta	= (float)nSec;
	m_szCountDownString = wszString;
	m_CountDownType		= type;

	if (m_CountDownType == eProgress ||
		m_CountDownType == eProgress_Always)
		OpenProgressDialog(wszString, (float)nSec, (float)nSec, true, true);
}

#ifdef PRE_FIX_ESCAPE
void CDnInterface::ShowEscapeDlg( bool bShow )
{
	if(IsPVP())
	{
		m_pEscapeDlg->Show(bShow);
	}
	else if( CDnActor::s_hLocalActor )
	{
		CDnLocalPlayerActor *pLocalActor = static_cast<CDnLocalPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
		pLocalActor->CmdEscape();
	}
}
#endif

void CDnInterface::EndCountDown()
{
	m_bEnableCountDown = false;
	m_fCountDownDelta = 0.f;
}

void CDnInterface::ProcessCountDown( float fDelta )
{
	if( !m_bEnableCountDown ) return;

	int nPrevSec = (int)m_fCountDownDelta + 1;
	m_fCountDownDelta -= fDelta;
	int nCurSec = (int)m_fCountDownDelta + 1;

	if( !m_bProcessMainProcess && 
		m_CountDownType != eProgress_Always)
		return;

	if( nCurSec != nPrevSec ) {
		if( m_CountDownType == eNumber_Prefix )
		{
#if defined(PRE_ADD_ENGLISH_STRING)
			std::wstring szUIString = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1000020 ), nCurSec );	// after %d sec.

			std::wstring szOutputMessage = FormatW( L"%s %s", m_szCountDownString.c_str(), szUIString.c_str() );
			ShowCaptionDialog( CDnInterface::typeCaption2, szOutputMessage.c_str(), EtInterface::textcolor::ORANGE );
#else
			std::wstring szStr = FormatW( L"%d %s %s", nCurSec, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 906 ), m_szCountDownString.c_str() );
			ShowCaptionDialog( CDnInterface::typeCaption2, szStr.c_str(), EtInterface::textcolor::ORANGE );
#endif	// #if defined(PRE_ADD_ENGLISH_STRING)
		}
		else if( m_CountDownType == eNumber_Only )
		{
			std::wstring szStr = FormatW( m_szCountDownString.c_str(), nCurSec );
			ShowCaptionDialog( CDnInterface::typeCaption2, szStr.c_str(), EtInterface::textcolor::ORANGE );
		}
		else if( m_CountDownType == eCaptain)
		{
#if defined(PRE_ADD_ENGLISH_STRING)
			std::wstring szUIString = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1000020 ), nCurSec );	// after %d sec.

			std::wstring szOutputMessage = FormatW( L"%s %s", m_szCountDownString.c_str(), szUIString.c_str() );
			ShowCaptionDialog( CDnInterface::typeCaption4, szOutputMessage.c_str(), EtInterface::textcolor::ORANGE );
#else
			std::wstring szStr = FormatW( L"%d %s %s", nCurSec, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 906 ), m_szCountDownString.c_str() );
			ShowCaptionDialog( CDnInterface::typeCaption4, szStr.c_str(), EtInterface::textcolor::ORANGE );
#endif	// #if defined(PRE_ADD_ENGLISH_STRING)
		}
	}

	if (m_CountDownType == eProgress ||
		m_CountDownType == eProgress_Always)
	{
		if( m_pProgressDlg ) m_pProgressDlg->SetElapsedTime(m_fCountDownDelta);
	}

	if( m_fCountDownDelta <= 0.f ) {
		if (m_CountDownType == eNumber_Prefix)
		{
			ShowCaptionDialog( CDnInterface::typeCaption2, m_szCountDownString.c_str(), EtInterface::textcolor::ORANGE );
		}
		else if(m_CountDownType == eCaptain)
		{
			ShowCaptionDialog( CDnInterface::typeCaption4, m_szCountDownString.c_str(), EtInterface::textcolor::ORANGE );
		}
		EndCountDown();
	}
}

void CDnInterface::CloseAllMainMenuDialog()
{
	if( !m_pMainMenuDlg ) return;
#ifdef PRE_ADD_GACHA_JAPAN
	GetInterface().CloseGachaDialog();
#endif // PRE_ADD_GACHA_JAPAN
	m_pMainMenuDlg->CloseMenuDialog();
	CloseModalDialog();
	CloseItemUnsealDialog();
	ShowItemSealDialog( false );
	ShowGuildMgrBox( CDnInterface::GuildMgrBox_Amount, false );
	if( m_pDarkLairRankBoardDlg ) m_pDarkLairRankBoardDlg->Show( false );
	if( m_pPVPLadderRankBoardDlg ) m_pPVPLadderRankBoardDlg->Show( false );
#ifdef PRE_ADD_PVP_VILLAGE_ACCESS
	if( m_pPVPLadderTabDlg ) m_pPVPLadderTabDlg->Show(false);
	if( m_pPVPRoomListDlg ) m_pPVPRoomListDlg->Show(false);
#endif
	if( m_pGuildWarPreliminaryStatusDlg && m_pGuildWarPreliminaryStatusDlg->IsShow() )	m_pGuildWarPreliminaryStatusDlg->Show( false );
	if( m_pGuildWarPreliminaryResultDlg && m_pGuildWarPreliminaryResultDlg->IsShow() )	m_pGuildWarPreliminaryResultDlg->Show( false );
	if( m_pGuildWarFinalTournamentListDlg && m_pGuildWarFinalTournamentListDlg->IsShow() )	m_pGuildWarFinalTournamentListDlg->Show( false );
#ifdef PRE_ADD_CHANGEJOB_CASHITEM
	if( m_pChangeJobDlg && m_pChangeJobDlg->IsShow() ) m_pChangeJobDlg->Show( false );
#endif // #ifdef PRE_ADD_CHANGEJOB_CASHITEM
#ifdef PRE_ADD_EXCHANGE_POTENTIAL
	GetInterface().ShowItemPotentialTransferDialog( false );
#endif

	if( GetChatDialog() && GetChatDialog()->IsShow())
		GetChatDialog()->ShowEx(false);

	//Close change Dialog:D
	if (m_pJobChangeDlg && m_pJobChangeDlg->IsShow())
		m_pJobChangeDlg->Show(false);

}

void CDnInterface::CloseModalDialog()
{
	// 스킬업다이얼로그, 금전입력창, Confirm창 등은
	// 렌더링 순서 및 여러가지 이유로 Child_modal을 하지 못했던 창들인데,
	// 파티장의 채널이동으로 메인메뉴는 Dninterface::Initialize에서 닫히지만 위 모달들은 안닫힌다.
	// 그래서 이렇게 직접 메인메뉴가 닫힐때 호출해서 닫기로 하겠다.
	CEtUIDialogBase::CloseModalDialog();
}

//blondy
void CDnInterface::CloseLobbyDialog()
{
	if( m_pPVPRoomListDlg )
	{
		m_pPVPRoomListDlg->Reset();
		m_pPVPRoomListDlg->Show(false);
		m_pPVPLadderTabDlg->Show(false);
#if defined(PRE_ADD_DWC)
		if(m_pPVPDWCTablDlg) 
			m_pPVPDWCTablDlg->Show(false);
#endif
	}
}

void CDnInterface::CloseGameRoomDialog()
{
	if( m_pGuildWarRoomDlg )
	{
		m_pGuildWarRoomDlg->Reset();
		m_pGuildWarRoomDlg->Show(false);
	}
	if( m_pPVPGameRoomDlg )
	{
		m_pPVPGameRoomDlg->Reset();
		m_pPVPGameRoomDlg->Show(false);
	}
	#ifdef PRE_ADD_PVP_TOURNAMENT
	if (m_pPVPTournamentRoomDlg)
	{
		m_pPVPTournamentRoomDlg->Reset();
		m_pPVPTournamentRoomDlg->Show(false);
	}
	#endif
}
//blondy end

void CDnInterface::UpdateMyPortrait()
{
	if( !CDnActor::s_hLocalActor ) return;
	CDnPlayerActor *pPlayerActor = static_cast<CDnPlayerActor*>( CDnActor::s_hLocalActor.GetPointer() );
	
	std::string strTargetAction = "Normal_Stand";

#ifdef PRE_FIX_PROTRAIT_FOR_GHOST
	if( pPlayerActor->IsSwapSingleSkin() )
	{
		strTargetAction = "Stand";
	}
	else if( pPlayerActor->IsGhost() || pPlayerActor->GetHP() <= 0  ) 
	{
		return;
	}
#else
	if( pPlayerActor->IsGhost() || pPlayerActor->GetHP() <= 0  ) 
		return;
#endif

	// 초상화에는 살펴보기나, 캐시샵 프리뷰와 달리 RenderAvatar를 사용하지 않는다.
	// 파티원 게이지나, 보스얼굴엔 애니메이션도 안하기 때문에,
	// 자신얼굴은 자신캐릭터에서 얻어다 쓰고, 파티원은 최소한의 정보로 임시액터 만들어 사용한다.
	// 자신얼굴 찍을때도 임시액터만들어 쓸 수도 있지만, 굳이 로컬액터가 살아있는데, 이렇게 할 필요는 없다고 본다.
	// RenderAvatar를 사용하지 않기 때문에 파티클은 나오지 않는다.
	if( CDnActor::s_hLocalActor->GetObjectHandle() )
	{
		int nAniIndex = CDnActor::s_hLocalActor->GetCachedAniIndex( strTargetAction.c_str() );
		if( nAniIndex != -1 ) 
		{
			if( m_pPlayerGaugeDlg ) 
			{
				if( pPlayerActor->IsSwapSingleSkin() || m_pPlayerGaugeDlg->GetFaceID() != (CDnActor::s_hLocalActor->GetClassID() -1) )
				{
					int nFaceID = CDnActor::s_hLocalActor->GetClassID() -1;
#ifdef PRE_FIX_PROTRAIT_FOR_GHOST
				if( pPlayerActor->IsSwapSingleSkin() ) 
						nFaceID = pPlayerActor->GetSingleSkinActorID() - 1;
#endif
					m_pPlayerGaugeDlg->SetFaceID( nFaceID );
				}

				m_pPlayerGaugeDlg->UpdatePortrait( CDnActor::s_hLocalActor->GetObjectHandle(), false, false, nAniIndex, 0.0f );
			}
		}
	}
}

void CDnInterface::UpdateTargetPortrait(DnActorHandle hActor)
{
	if( !CDnActor::s_hLocalActor || !hActor) return;
	CDnLocalPlayerActor *pPlayerActor = static_cast<CDnLocalPlayerActor*>( hActor.GetPointer() );

	std::string strTargetAction = "Normal_Stand";

#ifdef PRE_FIX_PROTRAIT_FOR_GHOST
	if( pPlayerActor->IsSwapSingleSkin() ) 
		strTargetAction = "Stand";
#else
	if( pPlayerActor->IsGhost() || pPlayerActor->GetHP() <= 0 ) 
		return;
#endif

	
	if( hActor->GetObjectHandle() )
	{
		int nAniIndex = hActor->GetCachedAniIndex( strTargetAction.c_str() );
		if( nAniIndex != -1 ) 
		{
			int nFaceID = pPlayerActor->GetClassID() -1;
#ifdef PRE_FIX_PROTRAIT_FOR_GHOST
			if( pPlayerActor->IsSwapSingleSkin() ) 
				nFaceID = pPlayerActor->GetSingleSkinActorID() - 1;
#endif

			m_pPlayerGaugeDlg->SetFaceID( nFaceID );
			m_pPlayerGaugeDlg->UpdatePortrait( hActor->GetObjectHandle(), false, false, nAniIndex, 0.0f );
		}
	}
}

void CDnInterface::BackupMyPortrait()
{
	if( !CDnActor::s_hLocalActor ) return;
	if( m_pPlayerGaugeDlg ) m_pPlayerGaugeDlg->BackupPortrait();
}


EtTextureHandle CDnInterface::CloneMyPortrait()
{
	EtTextureHandle hTexture = m_pPlayerGaugeDlg->GetPortraitTexture();
	if( !hTexture ) return CEtTexture::Identity();
//	EtBaseTexture *pBaseTexture = hTexture->GetTexturePtr();
	hTexture->AddRef();
//	pBaseTexture->AddRef();
	return hTexture;
}

void CDnInterface::ShowMiniMap( bool bShow )
{
	if( CDnMinimap::IsActive() )
	{
		GetMiniMap().Show(bShow);
	}
}

CEtUIDialog *CDnInterface::GetMainMenuDialog( DWORD dwDialogID )
{
	if( !m_pMainMenuDlg )
	{
		CDebugSet::ToLogFile( "CDnInterface::GetMainMenuDialog, m_pMainMenuDlg is NULL!" );
		return NULL;
	}

	return m_pMainMenuDlg->GetMainDialog( dwDialogID );
}

bool CDnInterface::IsEmptySlot( DWORD dwDialogID, int nSubSlotIndex )
{
	CDnInventoryDlg *pInvenDlg = dynamic_cast<CDnInventoryDlg*>(GetMainMenuDialog( dwDialogID ));

	if( NULL == pInvenDlg &&
		CDnMainMenuDlg::PLAYER_GUILD_STORAGE_DIALOG == dwDialogID )
	{
		CDnPGStorageTabDlg* pPGStorageDlg = dynamic_cast<CDnPGStorageTabDlg*>(GetMainMenuDialog(dwDialogID));
		if( pPGStorageDlg )
		{
			return pPGStorageDlg->IsEmptySlot();
		}
	}

	if( !pInvenDlg ) return false;

	switch( dwDialogID ) {
		case CDnMainMenuDlg::INVENTORY_DIALOG:
			{
				CDnInvenTabDlg *pTabDlg = dynamic_cast<CDnInvenTabDlg *>(pInvenDlg);
				if( !pTabDlg ) break;
				CDnInvenDlg *pDlg = pTabDlg->GetInvenDlg( (ITEM_SLOT_TYPE)nSubSlotIndex );
				if( !pDlg ) break;
				return pDlg->IsEmptySlot();
			}
			break;
		default: break;
	}
	return pInvenDlg->IsEmptySlot();
}

int CDnInterface::GetEmptySlot( DWORD dwDialogID, int nSubSlotIndex )
{
	CDnInventoryDlg *pInvenDlg = dynamic_cast<CDnInventoryDlg*>(GetMainMenuDialog( dwDialogID ));

	if( NULL == pInvenDlg &&
		CDnMainMenuDlg::PLAYER_GUILD_STORAGE_DIALOG == dwDialogID )
	{
		CDnPGStorageTabDlg* pPGStorageDlg = dynamic_cast<CDnPGStorageTabDlg*>(GetMainMenuDialog(dwDialogID));
		if( pPGStorageDlg )
		{
			return pPGStorageDlg->GetEmptySlot();
		}
	}

	if( !pInvenDlg ) return -1;

	switch( dwDialogID ) {
		case CDnMainMenuDlg::INVENTORY_DIALOG:
			{
				CDnInvenTabDlg *pTabDlg = dynamic_cast<CDnInvenTabDlg *>(pInvenDlg);
				if( !pTabDlg ) break;
				CDnInvenDlg *pDlg = pTabDlg->GetInvenDlg( (ITEM_SLOT_TYPE)nSubSlotIndex );
				if( !pDlg ) break;
				return pDlg->GetEmptySlot();
			}
			break;
		default: break;
	}
	return pInvenDlg->GetEmptySlot();
}

void CDnInterface::AddMessageCoin( INT64 nCoin )
{
	if( !m_pMainBar )	return;
	if( nCoin <= 0 )	return;

	std::wstring strCoin;
	DN_INTERFACE::UTIL::GetMoneyFormatUseStr( nCoin, strCoin );

	wchar_t szTemp[128] = {0};
	swprintf_s( szTemp, _countof(szTemp), L"[%s] %s", strCoin.c_str(), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 100046 ) );
	m_pMainBar->AddMessage(szTemp);
}

void CDnInterface::AddMessageItem( const wchar_t *szItemName, int nCount )
{
	if( !m_pMainBar ) return;
	if( nCount <= 0 ) return;

	wchar_t szTemp[128] = {0};

	if( nCount > 1 )
	{
		swprintf_s( szTemp, _countof(szTemp), L"[%s %d%s] %s", szItemName, nCount, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1728 ), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3548 ) );
	}
	else
	{
		swprintf_s( szTemp, _countof(szTemp), L"[%s] %s", szItemName, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3548 ) );
	}
	
	m_pMainBar->AddMessage(szTemp);
}

void CDnInterface::AddMessageExp( int Exp, const wchar_t *szStrArg )
{
	if( !m_pMainBar ) return;

	wchar_t szTemp[128] = {0};
	if( !szStrArg )
		swprintf_s( szTemp, _countof(szTemp), L"[+%d] %s", Exp, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 100044 ) );
	else
#if defined(PRE_ADD_ENGLISH_STRING)
		swprintf_s( szTemp, _countof(szTemp), L"%s %s", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 100044 ), szStrArg );
#else
		swprintf_s( szTemp, _countof(szTemp), L"%s %s", szStrArg, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 100044 ) );
#endif
	m_pMainBar->AddMessage(szTemp);
}

void CDnInterface::AddMessageSkill( const wchar_t *szSkillName )
{
	if( !m_pMainBar ) return;

	wchar_t szTemp[128] = {0};
	swprintf_s( szTemp, _countof(szTemp), L"[%s] %s", szSkillName, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 100045 ) );
	m_pMainBar->AddMessage(szTemp);
}

void CDnInterface::AddMessageText( const wchar_t *szText , DWORD dTextcolor)
{
	if( !m_pMainBar ) return;

	m_pMainBar->AddMessage(szText , dTextcolor);
}

void CDnInterface::SetChannelID( int nChannelID )
{
	if( !m_pChannelMoveDlg )	return;
	m_pChannelMoveDlg->SetChannelID( nChannelID );
}

void CDnInterface::RefreshStageInfoDlg( bool bRefreshInfo )
{
	if( m_pChannelMoveDlg ) m_pChannelMoveDlg->Show( false );
	if( m_pDungeonInfoDlg ) m_pDungeonInfoDlg->Show( false );
	if( m_pDarkLairInfoDlg ) m_pDarkLairInfoDlg->Show( false );

	bool bShowChannelMove = false;
	bool bShowDungeonInfo = false;
	bool bShowDarklairInfo = false;
	bool bShowCashShopButton = false;

	if( CDnWorld::GetInstance().GetMapType() == CDnWorld::MapTypeVillage ) {
		bShowCashShopButton = true;
		bShowChannelMove = true;
	}
	else {
		CDnGameTask *pTask = (CDnGameTask *)CTaskManager::GetInstance().GetTask( "GameTask" );
		if( pTask ) {
			switch( pTask->GetGameTaskType() ) {
				case GameTaskType::Normal:
				case GameTaskType::Farm:
					switch( CDnWorld::GetInstance().GetMapType() ) {
						case CDnWorld::MapTypeWorldMap: bShowChannelMove = true; break;
						case CDnWorld::MapTypeDungeon: bShowDungeonInfo = true; break;
					}
					break;
				case GameTaskType::PvP:
					break;
				case GameTaskType::DarkLair:
					switch( CDnWorld::GetInstance().GetMapType() ) {
						case CDnWorld::MapTypeWorldMap: bShowChannelMove = true; break;
						case CDnWorld::MapTypeDungeon: bShowDarklairInfo = true; break;
					}
					break;
			}
		}
	}
	if( bShowChannelMove && m_pChannelMoveDlg ) {
		m_pChannelMoveDlg->Show( true );
		if( bRefreshInfo ) m_pChannelMoveDlg->RefreshMapInfo();
	}
	if( bShowDungeonInfo && m_pDungeonInfoDlg ) {
		m_pDungeonInfoDlg->Show( true );
		if( bRefreshInfo ) m_pDungeonInfoDlg->RefreshInfo();
	}
	/*if( bShowDarklairInfo && m_pDarkLairInfoDlg ) {
		m_pDarkLairInfoDlg->Show( true );
		if( bRefreshInfo ) m_pDarkLairInfoDlg->RefreshInfo();
	}*/
	//rlkt_test

#ifdef PRE_REMOVE_CASHSHOP
#else
	if (m_pCashShopMenuDlg)
	{
		m_pCashShopMenuDlg->Show(bShowCashShopButton);
	}
#endif
}

void CDnInterface::RefreshRepUnionMembershipMark(const std::vector<SUnionMembershipData>& membershipList)
{
	DNTableFileFormat*  pUnionBenefitTable = GetDNTable(CDnTableDB::TREPUTEBENEFIT);
	if (pUnionBenefitTable == NULL)
		return;

	std::vector<int> markItemIdList;

	std::vector<SUnionMembershipData>::const_iterator iter = membershipList.begin();
	for (; iter != membershipList.end(); ++iter)
	{
		const SUnionMembershipData& data = (*iter);
		markItemIdList.push_back(data.itemId);
	}

	if (m_pChannelMoveDlg)
		m_pChannelMoveDlg->SetUnionMarks(markItemIdList);
	if (m_pDungeonInfoDlg)
		m_pDungeonInfoDlg->SetUnionMarks(markItemIdList);
}

void CDnInterface::ShowRepUnionMarkTooltip(int tableIdx, float fMouseX, float fMouseY)
{
	if (m_pRepUnionMarkTooltipDlg == NULL)
		return;

	m_pRepUnionMarkTooltipDlg->SetMarkTooltip(tableIdx);
	m_pRepUnionMarkTooltipDlg->AdjustPosition(fMouseX, fMouseY + ( CDnMouseCursor::GetInstance().GetCursorHeight() * 3/4 ));
	m_pRepUnionMarkTooltipDlg->Show(true);
}

void CDnInterface::CloseRepUnionMarkTooltip()
{
	if (m_pRepUnionMarkTooltipDlg->IsShow())
		m_pRepUnionMarkTooltipDlg->Show(false);
}

#if defined(PRE_ADD_VIP_FARM)
void CDnInterface::ShowLifeUnionMarkTooltip( float fMouseX, float fMouseY )
{
	if( NULL == m_pLifeUnionMarkTooltipDlg )
		return;

	m_pLifeUnionMarkTooltipDlg->SetMarkTooltip();
	m_pLifeUnionMarkTooltipDlg->SetPosition( fMouseX, fMouseY + ( CDnMouseCursor::GetInstance().GetCursorHeight() * 3/4 ) );
	m_pLifeUnionMarkTooltipDlg->Show( true );
}

void CDnInterface::CloseLifeUnionMarkTooltip()
{
	if( m_pLifeUnionMarkTooltipDlg->IsShow() )
		m_pLifeUnionMarkTooltipDlg->Show( false );
}
#endif

#if defined(PRE_ADD_DWC)
void CDnInterface::RefreshDWCMark(bool bShow, bool bNew)
{
	if(m_pChannelMoveDlg)
		m_pChannelMoveDlg->SetDWCMark(bShow, bNew);
}

void CDnInterface::ShowDWCMarkTooltip(float fMouseX, float fMouseY)
{
	if (m_pDWCMarkToolTipDlg == NULL || CDnDWCTask::IsActive() == false)
		return;
	
	if(GetDWCTask().IsDWCRankSession() == false)
	{
		CloseDWCMarkTooltip();
		return;
	}

	bool bRet = m_pDWCMarkToolTipDlg->SetMarkTooltip();
	if(bRet)
	{
		m_pDWCMarkToolTipDlg->AdjustPosition(fMouseX, fMouseY + ( CDnMouseCursor::GetInstance().GetCursorHeight() * 3/4 ) );
		m_pDWCMarkToolTipDlg->Show(bRet);
	}
}

void CDnInterface::CloseDWCMarkTooltip()
{
	if (m_pDWCMarkToolTipDlg->IsShow())
		m_pDWCMarkToolTipDlg->Show(false);
}

#endif // PRE_ADD_DWC

void CDnInterface::RefreshGuildWarMark(bool bShow, bool bNew)
{
	if (m_pChannelMoveDlg)
		m_pChannelMoveDlg->SetGuildWarMark(bShow, bNew);
	if (m_pDungeonInfoDlg)
		m_pDungeonInfoDlg->SetGuildWarMark(bShow, bNew);
}

void CDnInterface::ShowGuildWarMarkTooltip(float fMouseX, float fMouseY)
{
	if (m_pGuildWarMarkTooltipDlg == NULL || CDnGuildWarTask::IsActive() == false)
		return;

	eGuildWarStepType warStep = CDnGuildWarTask::GetInstance().GetCurrentGuildWarEventTimeType();
	if (warStep == GUILDWAR_STEP_NONE || warStep == GUILDWAR_STEP_END)
	{
		m_pGuildWarMarkTooltipDlg->Show(false);
		return;
	}

	bool bRet = m_pGuildWarMarkTooltipDlg->SetMarkTooltip();
	if (bRet)
	{
		m_pGuildWarMarkTooltipDlg->AdjustPosition(fMouseX, fMouseY + ( CDnMouseCursor::GetInstance().GetCursorHeight() * 3/4 ) );
		m_pGuildWarMarkTooltipDlg->Show(bRet);
	}
}

void CDnInterface::CloseGuildWarMarkTooltip()
{
	if (m_pGuildWarMarkTooltipDlg->IsShow())
		m_pGuildWarMarkTooltipDlg->Show(false);
}

void CDnInterface::MoveOffsetStageInfoDlg( bool bMove )
{
	// IsShow검사를 하지 않는건 나중에 Show되는 순서가 바뀌더라도 제대로 동작하도록 하기 위함이다.
	// 그냥 두 InfoDlg에 같이 적용하는건 어쩔 수 없는 것으로 우선 처리.

	if( m_pDungeonInfoDlg )
		m_pDungeonInfoDlg->MoveOffset( bMove );

	if( m_pDarkLairInfoDlg )
		m_pDarkLairInfoDlg->MoveOffset( bMove );
}

void CDnInterface::OpenNotifyArrowDialog(EtVector3 vTarget, LOCAL_TIME nTime, bool bImmediately )
{
	if( !m_pMainMenuDlg ) return;
	
	m_pMainMenuDlg->OpenNotifyArrowDialog(vTarget, nTime, bImmediately);
}


void CDnInterface::CloseNotifyArrowDialog()
{
	if( !m_pMainMenuDlg ) return;

	m_pMainMenuDlg->CloseNotifyArrowDialog();
}

void CDnInterface::SetCutSceneCaption( const wchar_t *wszCaption )
{
	if( !m_pBlindDlg ) return;
	m_pBlindDlg->SetCaption( wszCaption );
}

void CDnInterface::ClearCutSceneCaption()
{
	if( !m_pBlindDlg ) return;
	m_pBlindDlg->ClearCaption();
}

void CDnInterface::ShowChatDialog()
{
	if( !GetChatDialog() ) return;
	GetChatDialog()->ShowChatDlg();
}

void CDnInterface::OpenMissionFailDialog( bool bTimeOut, bool bCanWarpWorldZone )
{
	ShowMiniMap( false );
	CEtUIDialogBase::CloseAllDialog();

	m_pDungeonMoveDlg->EnableWarpWorldZone( bCanWarpWorldZone );

	m_pBlindDlg->SetBillBoardColor( 0x00000000 );
	
	if( bTimeOut )
		m_pBlindDlg->AddBlindCallBack( m_pTimeOutFailDlg );
	else
		m_pBlindDlg->AddBlindCallBack( m_pMissionFailDlg );

	m_pBlindDlg->Show(true);
}

#ifdef PRE_ADD_STAGE_LIMIT_INTERFACE
void CDnInterface::ShowDungeonLimitDlg( bool bShow, int nMaxIndex )
{
	if( m_pDungeonLimitDlg )
	{
		m_pDungeonLimitDlg->Show( bShow );
		if( bShow == true )
			m_pDungeonLimitDlg->SetDungeonLimitInfo( nMaxIndex );
	}
}
#endif

void CDnInterface::OpenMissionFailMoveDialog()
{
	if( !m_pDungeonMoveDlg ) return;

	m_pDungeonMoveDlg->SetMissionFailMode(true);
#ifdef PRE_ADD_RETURN_VILLAGE_SHOWNAME
	m_pDungeonMoveDlg->SetReturnVillage( m_nVillage );
#endif 

#ifdef PRE_WORLDCOMBINE_PARTY
	if (CDnPartyTask::IsActive())
		m_pDungeonMoveDlg->EnableWarpWorldZone(!CDnPartyTask::GetInstance().IsPartyType(_WORLDCOMBINE_PARTY));
	else
		m_pDungeonMoveDlg->EnableWarpWorldZone(true);
#endif
	m_pDungeonMoveDlg->Show(true);
}

void CDnInterface::ShowDungeonMoveDlg(bool bShow, bool bMissionFailMode)
{
	if (m_pDungeonMoveDlg == NULL)
		return;

	m_pDungeonMoveDlg->SetMissionFailMode(bMissionFailMode);
#ifdef PRE_ADD_RETURN_VILLAGE_SHOWNAME
	m_pDungeonMoveDlg->SetReturnVillage( m_nVillage );
#endif 
#ifdef PRE_WORLDCOMBINE_PARTY
	if (CDnPartyTask::IsActive())
		m_pDungeonMoveDlg->EnableWarpWorldZone(!CDnPartyTask::GetInstance().IsPartyType(_WORLDCOMBINE_PARTY));
	else
		m_pDungeonMoveDlg->EnableWarpWorldZone(true);
#else
	m_pDungeonMoveDlg->EnableWarpWorldZone( true );
#endif
	m_pDungeonMoveDlg->Show(bShow);
}

bool CDnInterface::IsShowRebirthCationDlg()
{
	if( !m_pRebirthCaptionDlg ) return false;
	return m_pRebirthCaptionDlg->IsShow();
}

bool CDnInterface::IsShowRebirthFailDlg()
{
	if( !m_pRebirthFailDlg ) return false;
	return m_pRebirthFailDlg->IsShow();
}

void CDnInterface::OpenRebirthCaptionDlg()
{
	if( !m_pRebirthCaptionDlg ) return;

	m_pRebirthCaptionDlg->Show( true );
}

void CDnInterface::OpenRebirthFailDlg()
{
	if( !m_pRebirthFailDlg ) return;

	m_pRebirthFailDlg->Show( true );
}

void CDnInterface::CloseRebirthCaptionDlg()
{
	if( !m_pRebirthCaptionDlg ) return;
	m_pRebirthCaptionDlg->Show( false );
}

void CDnInterface::CloseRebirthFailDlg()
{
	if( !m_pRebirthFailDlg ) return;
	m_pRebirthFailDlg->Show( false );
}

void CDnInterface::RefreshRebirthInterface()
{
	CDnPartyTask *pTask = (CDnPartyTask *)CTaskManager::GetInstance().GetTask( "PartyTask" );
	CDnGameTask *pGameTask = (CDnGameTask *)CTaskManager::GetInstance().GetTask( "GameTask" );

	if( !pTask || !pGameTask )
		return;

	switch( pGameTask->GetGameTaskType() ) 
	{
//		case GameTaskType::DarkLair:
		case GameTaskType::PvP:
			return;
	}

	if( pTask->GetPartyRole() == CDnPartyTask::LEADER || pTask->GetPartyRole() == CDnPartyTask::SINGLE ) 
	{
		if( !CDnPartyTask::GetInstance().IsPartyMemberAlive() )
		{
			if (pGameTask->GetGameTaskType() == GameTaskType::DarkLair)
			{
				if (pTask->GetPartyState() != CDnPartyTask::NORMAL)
				{
					CloseRebirthFailDlg();
					return;
				}
			}
			GetInterface().OpenRebirthFailDlg();
		}
		else {
			if( pGameTask ) {
				switch( pGameTask->GetGameTaskType() ) {
					case GameTaskType::DarkLair:
						return;
				}
			}
			GetInterface().CloseRebirthFailDlg();
		}
	}

}

bool CDnInterface::IsOpenBlind()
{
	if( m_pBlindDlg && (m_pBlindDlg->GetBlindMode() != CDnBlindDlg::modeClosed) )
	{
		return true;
	}

	return false;
}

bool CDnInterface::IsShowPrivateMarketDlg()
{
	if( !m_pMainMenuDlg ) return false;

	CDnInvenTabDlg *pInvenDlg = (CDnInvenTabDlg*)m_pMainMenuDlg->GetMainDialog( CDnMainMenuDlg::INVENTORY_DIALOG );
	if( !pInvenDlg ) return false;

	return pInvenDlg->IsShowPrivateMarketDialog();
}

bool CDnInterface::IsShowCashShopDlg() const
{
	if (m_pCashShopDlg == NULL)
		return false;

	return m_pCashShopDlg->IsShow();
}

void CDnInterface::OnAutoCursorCallback( bool bShow )
{
	CDnMouseCursor::GetInstance().ShowCursor( bShow );
}

void CDnInterface::OnLostDevice()
{

}

void CDnInterface::OnResetDevice()
{
	SUICoord dlgCoord;

	for( DWORD i = 1; i<m_pVecEnemyGaugeList.size(); i++ )
	{
		m_pVecEnemyGaugeList[i].pGaugeDlg->GetDlgCoord( dlgCoord );
		{
			dlgCoord.fY = m_fEnemyGaugeOffset + ( (i-1) * dlgCoord.fHeight );
		}
		m_pVecEnemyGaugeList[i].pGaugeDlg->SetDlgCoord( dlgCoord );
	}

	for( DWORD i = 0; i<m_pVecMonsterPartsGuageList.size(); i++ )
	{
		if( i < ( eMiniGaugeSize::eMonsterPartsGaugeSize / 2 ) )
		{
			m_pVecMonsterPartsGuageList[i]->GetDlgCoord( dlgCoord );
			{
				dlgCoord.fY = m_vec2MonsterPartsGaugeOffset.y + ( i * dlgCoord.fHeight );
			}
			m_pVecMonsterPartsGuageList[i]->SetDlgCoord( dlgCoord );
		}
		else
		{
			m_pVecMonsterPartsGuageList[i]->GetDlgCoord( dlgCoord );
			{
				dlgCoord.fY = m_vec2MonsterPartsGaugeOffset.y + ( i-2 * dlgCoord.fHeight );
				dlgCoord.fX = m_vec2MonsterPartsGaugeOffset.x - dlgCoord.fWidth;
			}
			m_pVecMonsterPartsGuageList[i]->SetDlgCoord( dlgCoord );
		}
	}

	for( DWORD i = 0; i<m_pVecPartyPlayerGaugeList.size(); i++ )
	{
		m_pVecPartyPlayerGaugeList[i].pGaugeDlg->GetDlgCoord( dlgCoord );
		{
			dlgCoord.fX = (i / MAX_8RAID_GROUP_MEMBER) * dlgCoord.fWidth;
			dlgCoord.fY = m_fPartyPlayerGaugeOffset + ((i % MAX_8RAID_GROUP_MEMBER) * dlgCoord.fHeight);
		}
		m_pVecPartyPlayerGaugeList[i].pGaugeDlg->SetDlgCoord( dlgCoord );
	}

#ifdef PRE_ADD_IMPROVE_MASTER_OBSERVER
	for( DWORD i = 0; i<m_pVecPartyPlayerRightGageList.size(); i++ )
	{
		m_pVecPartyPlayerRightGageList[i].pGaugeDlg->GetDlgCoord( dlgCoord );
		{
			dlgCoord.fX = (i / MAX_8RAID_GROUP_MEMBER) * dlgCoord.fWidth;
			dlgCoord.fY = m_fPartyPlayerGaugeOffset + ((i % MAX_8RAID_GROUP_MEMBER) * dlgCoord.fHeight);
		}
		m_pVecPartyPlayerRightGageList[i].pGaugeDlg->SetDlgCoord( dlgCoord );
	}
#endif

	// 자신의 얼굴은 소셜애니메이션이 실행될때마다 새로 업뎃하지만, 리셋될때 바로 하는게 정석이므로,
	UpdateMyPortrait();

	// 파티원 얼굴은 처리해야할게 많아서 아래 함수를 부르는 것으로 처리
	RefreshPartyGuage();
	RefreshGuildWarSituation();
	// 보스.
	for( int i=0; i<(int)m_pVecEnemyGaugeList.size(); i++ )
		UpdateBossPortrait( i );
}

//blondy
void CDnInterface::ChageLobbyState( emLobbyState state )
{
	if( !m_pTask ) return;

	CDnPVPLobbyVillageTask *pTask;
	pTask = dynamic_cast<CDnPVPLobbyVillageTask *>(m_pTask);
	//CDnBridgeTask *pBridgeTask = (CDnBridgeTask *)CTaskManager::GetInstance().GetTask( "BridgeTask" );

	if(!pTask)
	{
		ErrorLog("ChageLobbyState()::Not PVPVillageTask!");
		return;
	}

	switch( state )
	{
	case emLobbyState::InLobby:
		m_pPVPRoomListDlg->Show(true);
		
#if defined(PRE_ADD_DWC)
		m_pPVPLadderTabDlg->Show(!GetDWCTask().IsDWCChar());
		m_pPVPDWCTablDlg->Show(GetDWCTask().IsDWCChar());
#else
		m_pPVPLadderTabDlg->Show(true);
#endif // PRE_ADD_DWC
		m_pPVPChatDlg->ShowObserverDlg(false);
		CloseGameRoomDialog();
		return;
	case emLobbyState::InGameRoom:
		{
#ifdef PRE_ADD_PVP_TOURNAMENT
			SwitchPVPRoomDlg();
#else	
		if( eGuildWar_Room == m_ePVPRoomState )
		{
			m_pGuildWarRoomDlg->Show(true);
			m_pPVPGameRoomDlg->Show(false);
		}
		else
		{
			m_pPVPGameRoomDlg->Show(true);
			m_pGuildWarRoomDlg->Show(false);
		}	
#endif // PRE_ADD_PVP_TOURNAMENT

		m_pPVPChatDlg->ShowObserverDlg(true);
		CloseLobbyDialog();
		}
		return;
	}

	ErrorLog("ChageLobbyState()::Lobby not Chage ");
}

#ifdef PRE_ADD_PVP_TOURNAMENT
void CDnInterface::SwitchPVPRoomDlg()
{
	bool bRoomState[eRoomMax];
	memset(bRoomState, 0, sizeof(bRoomState));

	if (m_ePVPRoomState < ePVP_Room || m_ePVPRoomState >= eRoomMax)
	{
		_ASSERT(0);
		return;
	}

	bRoomState[m_ePVPRoomState] = true;

	m_pGuildWarRoomDlg->Show(bRoomState[eGuildWar_Room]);
	m_pPVPGameRoomDlg->Show(bRoomState[ePVP_Room]);
	m_pPVPTournamentRoomDlg->Show(bRoomState[eTournament_Room]);
}
#endif

CDnBaseRoomDlg *CDnInterface::GetGameRoomDlg()
{
	if( !m_pTask )
	{
		return NULL;
	}

	if( !IsPVPLobby() || (!m_pPVPGameRoomDlg && !m_pGuildWarRoomDlg) )
	{
		return NULL;
	}

#ifdef PRE_ADD_PVP_TOURNAMENT
	switch(m_ePVPRoomState)
	{
	case ePVP_Room:
		return m_pPVPGameRoomDlg;

	case eGuildWar_Room:
		return m_pGuildWarRoomDlg;

	case eTournament_Room:
		return m_pPVPTournamentRoomDlg;
	}

	return NULL;
#else
	return eGuildWar_Room == m_ePVPRoomState ? (CDnBaseRoomDlg *)m_pGuildWarRoomDlg : (CDnBaseRoomDlg *)m_pPVPGameRoomDlg;
#endif
}

CDnGuildWarRoomDlg *CDnInterface::GetGuildWarRoomDlg()
{
	if( !m_pTask ) return NULL;

	CDnPVPLobbyVillageTask *pTask;
	pTask = dynamic_cast<CDnPVPLobbyVillageTask *>(m_pTask);

	if(!pTask || !m_pGuildWarRoomDlg )
	{
		ErrorLog("ChageLobbyState()::Not PVPVillageTask or m_pGuildWarRoomDlg = NULL");
		return NULL;
	}

	return m_pGuildWarRoomDlg;
}

void CDnInterface::SelectPVPRoom( ePVPRoomState eMode )
{
	m_ePVPRoomState = eMode;
}

void CDnInterface::ShowHelpKeyboardDialog( bool bShow )
{
	if( m_pHelpKeyboardDlg )
		m_pHelpKeyboardDlg->Show( bShow );
}

void CDnInterface::HideCommonDialog()
{
	for( int i = 0; i < ( int )m_pVecCommonDialogList.size(); i++ )
	{
		if(m_pVecCommonDialogList[i])
			m_pVecCommonDialogList[ i ]->Show( false );
	}
}

bool CDnInterface::IsPVP()
{
	CDnGameTask *pGameTask = (CDnGameTask *)CTaskManager::GetInstance().GetTask( "GameTask" );

	if( pGameTask && pGameTask->GetGameTaskType() == GameTaskType::PvP )
		return true;
	return false;
}

bool CDnInterface::IsLadderMode()
{
	CDnGameTask *pGameTask = (CDnGameTask*)CTaskManager::GetInstance().GetTask( "GameTask" );
	if( pGameTask && pGameTask->GetGameTaskType() == GameTaskType::PvP) 
	{
		if( pGameTask && ((CDnPvPGameTask*)pGameTask)->IsLadderMode() )
			return true;
	}
	return false;
}

bool CDnInterface::IsPVPLobby()
{
	CDnPVPLobbyVillageTask *pPVPVillageTask = (CDnPVPLobbyVillageTask *)CTaskManager::GetInstance().GetTask( "PVPLobbyTask" );
	return pPVPVillageTask?true:false;
}

void CDnInterface::AddPVPGameUer( DnActorHandle hUser)
{
	if(IsPVP())
	{
		if( m_pPVPGameResultDlg && hUser )
		{
			// 옵져버일경우에는 좌측에 A팀 우측에 B팀 표시
			bool bIsLeft = false;
			if( !CDnActor::s_hLocalActor || CDnActor::s_hLocalActor->GetTeam() == PvPCommon::Team::Observer )
				bIsLeft = (hUser->GetTeam() == PvPCommon::Team::A) ? true : false;
			// 일반적인 경우에는 내 팀이 왼쪽 상대팀이 오른쪽
			else
				bIsLeft = ( CDnActor::s_hLocalActor->GetTeam() == hUser->GetTeam() ) ? true : false;

			m_pPVPGameResultDlg->AddUser( hUser , bIsLeft );
			if( CDnBridgeTask::GetInstance().IsPvPGameMode( PvPCommon::GameMode::PvP_Occupation ) && m_pGuildWarZoneMapDlg )
				m_pGuildWarZoneMapDlg->AddUser( hUser, false );
		}
		else if(m_pPVPGameResultAllDlg && hUser)
		{
			m_pPVPGameResultAllDlg->AddUser(hUser);
		}
		else if(m_pGuildWarResultDlg && m_pGuildWarZoneMapDlg && hUser)
		{
			m_pGuildWarResultDlg->AddUser(hUser);
			m_pGuildWarZoneMapDlg->AddUser(hUser, true);
		}
#if defined( PRE_ADD_RACING_MODE )
		else if( m_pPvPRacingResultDlg && hUser )
		{
			m_pPvPRacingResultDlg->AddUser(hUser);
		}
#endif	// #if defined( PRE_ADD_RACING_MODE )
#ifdef PRE_ADD_PVP_TOURNAMENT
		else if (m_pPVPTournamentFinalResultDlg && hUser)
		{
			m_pPVPTournamentFinalResultDlg->AddUser(hUser);
		}
#endif
	}	
}

void CDnInterface::SetPVPGameUerState( DnActorHandle hUser, int iState )
{
	if(IsPVP())
	{
		if( m_pPVPGameResultDlg && hUser )
		{
			m_pPVPGameResultDlg->SetUserState( hUser , iState );
		}
		else if(m_pPVPGameResultAllDlg && hUser)
		{
			m_pPVPGameResultAllDlg->SetUserState(hUser , iState);
		}
		else if(m_pGuildWarResultDlg && hUser)
		{
			m_pGuildWarResultDlg->SetUserState(hUser , iState);
		}
#if defined( PRE_ADD_RACING_MODE )
		else if( m_pPvPRacingResultDlg && hUser )
		{
			m_pPvPRacingResultDlg->SetUserState( hUser, iState );
		}
#endif	// #if defined( PRE_ADD_RACING_MODE )
	}	
}

void CDnInterface::RestartRound()
{
	if(IsPVP())
	{
		if( m_pPVPGameResultDlg )
		{
			m_pPVPGameResultDlg->RestartRound();
		}
		else if(m_pPVPGameResultAllDlg )
		{
			m_pPVPGameResultAllDlg->RestartRound();
		}
		else if( m_pGuildWarResultDlg )
		{
			m_pGuildWarResultDlg->RestartRound();
		}
	}
}


void CDnInterface::SetPVPGameScore( int nSessionID, int nKOCount, int nKObyCount,int nBossKOCount,UINT uiKOP, UINT uiAssistP, UINT uiTotalP, UINT uiOccupation )
{
	if(IsPVP())
	{
		if( m_pPVPGameResultDlg )
		{
			m_pPVPGameResultDlg->SetUserScore( nSessionID, nKOCount, nKObyCount, nBossKOCount , uiKOP, uiAssistP, uiTotalP ); 
		}
		else if(m_pPVPGameResultAllDlg)
		{
			m_pPVPGameResultAllDlg->SetUserScore( nSessionID, nKOCount, nKObyCount, uiKOP, uiAssistP, uiTotalP ); 
		}
		else if(m_pGuildWarResultDlg)
		{
			m_pGuildWarResultDlg->SetUserScore( nSessionID, nKOCount, nKObyCount, uiKOP, uiAssistP, uiTotalP, uiOccupation ); 
		}
#ifdef PRE_ADD_PVP_TOURNAMENT
		else if (m_pPVPTournamentFinalResultDlg)
		{
			m_pPVPTournamentFinalResultDlg->SetUserScore(nSessionID, nKOCount, nKObyCount, uiKOP, uiAssistP, uiTotalP, uiOccupation);
		}
#endif
	}	
}

void CDnInterface::SetPvPGameOccupationScore( int nSessionID, UINT uiOccupation, UINT uiOccupationSteal )
{
	if( CDnBridgeTask::GetInstance().IsPvPGameMode(PvPCommon::GameMode::PvP_Occupation) )
		m_pPVPGameResultDlg->SetOccupationScore( nSessionID, uiOccupation, uiOccupationSteal );
}

void CDnInterface::SetPvPRacingScore( int nSessionID, DWORD dwLapTime )
{
#if defined( PRE_ADD_RACING_MODE )
		if( m_pPvPRacingResultDlg )
		{
			m_pPvPRacingResultDlg->SetUserScore( nSessionID, dwLapTime );
		}
#endif	// #if defined( PRE_ADD_RACING_MODE )	
}

void CDnInterface::RemovePVPGameUer( DnActorHandle hUser )
{
	if(IsPVP())
	{
		if( m_pPVPGameResultDlg && hUser )
		{
			m_pPVPGameResultDlg->RemoveUser( hUser );
		}
		else if(m_pPVPGameResultAllDlg && hUser)
		{
			m_pPVPGameResultAllDlg->RemoveUser(hUser);
		}
		else if(m_pGuildWarResultDlg && hUser)
		{
			m_pGuildWarResultDlg->RemoveUser(hUser);
			m_pGuildWarSituation->RemoveUser(hUser);
		}
#if defined( PRE_ADD_RACING_MODE )
		else if( m_pPvPRacingResultDlg && hUser )
		{
			m_pPvPRacingResultDlg->RemoveUser( hUser );
		}
#endif	// #if defined( PRE_ADD_RACING_MODE )
#ifdef PRE_ADD_PVP_TOURNAMENT
		else if (m_pPVPTournamentFinalResultDlg && hUser)
		{
			m_pPVPTournamentFinalResultDlg->RemoveUser(hUser);
		}
#endif
	}	
}


void CDnInterface::OpenRandomItemDlg( char cInvenType, BYTE cInvenIndex, INT64 biInvenSerial )
{
	if( m_pRandomItemDlg ) {
		m_pRandomItemDlg->ShowEx( true, cInvenType, cInvenIndex, biInvenSerial, 2.f );
	}
}

void CDnInterface::OpenRandomItemResultDlg( char cInvenType, int nItemID, char cLevel, int nItemCount )
{
	if( m_pRandomItemDlg ) {
		m_pRandomItemDlg->Show( false );
	}

	const char *szMovieName[] = {
		"RandomItem_Bad.avi",
		"RandomItem_Normal.avi",
		"RandomItem_Good.avi",
		"RandomItem_VeryGood.avi",
	};

	//	OpenMovieDlg( szMovieName[ cLevel - 1 ], 5.f );
	CDnItem *pItem = NULL;
#if defined(PRE_FIX_73183)
	bool bCash = CDnItem::IsCashItem( nItemID );
	if( bCash )
		pItem = GetItemTask().GetCashInventory().FindItem( nItemID );
	else
		pItem = GetItemTask().GetCharInventory().FindItem( nItemID );
#else
	switch( cInvenType )
	{
	case ITEMPOSITION_INVEN:		pItem = GetItemTask().GetCharInventory().FindItem( nItemID );	break;
	case ITEMPOSITION_CASHINVEN:	pItem = GetItemTask().GetCashInventory().FindItem( nItemID );	break;
	}
#endif //

	if( pItem ) {
		ShowItemAlarmDialog( pItem->GetName(), pItem, nItemCount );
	}
}

void CDnInterface::OpenMovieDlg( const char *szFileName, float fTimer )
{
	if( !m_pMovieDlg ) return;
	m_pMovieDlg->Show( true );
	m_pMovieDlg->PlayMovie( szFileName, fTimer );
}

void CDnInterface::CloseMovieDlg()
{
	if( !m_pMovieDlg ) return;
	m_pMovieDlg->Show( false );
}

void CDnInterface::OpenMovieProcessDlg( const char *szMovieFileName, float fTimer, LPCWSTR wszStaticMsg, int nID , CEtUICallback *pCall, bool bShowButton )
{
	if( m_pMovieProcessDlg )
	{
		// 타이머 지나면 알아서 안보일거다.
		m_pMovieProcessDlg->SetInfo( wszStaticMsg, fTimer, nID, pCall, bShowButton );
		m_pMovieProcessDlg->Show( true );
	}

	/*
	if( m_pMovieDlg )
	{
		m_pMovieDlg->Show( true );
		m_pMovieDlg->PlayMovie( szMovieFileName, fTimer );
	}
	*/
}

void CDnInterface::CloseMovieProcessDlg( bool bDisableFlag )
{
	if( bDisableFlag )
		m_pMovieProcessDlg->DisableFlag();

	if( m_pMovieProcessDlg )
		m_pMovieProcessDlg->Show( false );

	if( m_pMovieDlg )
		m_pMovieDlg->Show( false );
}

void CDnInterface::OpenFocusTypeMovieProcessDlg( const char *szMovieFileName, float fTimer, LPCWSTR wszStaticMsg, int nID , CEtUICallback *pCall )
{
	if( m_pFocusTypeMovieProcessDlg )
	{
		// 타이머 지나면 알아서 안보일거다.
		m_pFocusTypeMovieProcessDlg->SetInfo( wszStaticMsg, fTimer, nID, pCall );
		m_pFocusTypeMovieProcessDlg->Show( true );
	}
}

void CDnInterface::CloseFocusTypeMovieProcessDlg( bool bDisableFlag )
{
	if( bDisableFlag )
		m_pFocusTypeMovieProcessDlg->DisableFlag();

	if( m_pFocusTypeMovieProcessDlg )
		m_pFocusTypeMovieProcessDlg->Show( false );

	if( m_pMovieDlg )
		m_pMovieDlg->Show( false );
}

void CDnInterface::OpenMovieAlarmDlg( const char *szMovieFileName, float fTimer, int nItemID, int nEnchantLevel, char cOption, int nFailMsgID )
{
	// MovieProcess는 모달로 띄운게 하나라도 있어서 툴팁이 안뜨는데, 이건 툴팁이 뜬다.
	// 그런데 동영상은 직접 텍스처를 건드린거라 툴팁이 동영상에 가려진다.
	// 그냥 movieDlg를 모달로 해야하나..
	// 나중에 이상하다고 하면 그때 바꾸던지 해야겠다.

	if( nFailMsgID == 0 )
	{
		// 템이 생성되고 결과를 알리는거니 인벤토리에서 찾아도 된다.
		CDnItem *pItem = CDnItemTask::GetInstance().GetCharInventory().FindItem( nItemID, cOption );
		if( pItem )
		{
			// 강화 되어있다면,
			WCHAR wszMsg[256];
			if( nEnchantLevel > 0 )
				swprintf_s(wszMsg, _countof(wszMsg), L"+%d %s", nEnchantLevel, pItem->GetName() );
			else
				swprintf_s(wszMsg, _countof(wszMsg), L"%s", pItem->GetName() );

			// pItem->GetOverlapCount() 으로 수량얻는거 버그 생길 가능성 있다.
			// 중첩되는 아이템으로 나올 경우 장비류와 달리 실제 얻은 개수와 다를 수 있기때문이다.
			// 문장보옥, 조합에서는 항상 1개의 아이템만을 만들 수 있으니 강제로 1로 설정하도록 하겠다.
			ShowItemAlarmDialog( wszMsg, pItem, 1, textcolor::WHITE, fTimer );
		}
	}
	else
	{
		WCHAR wszMsg[256];
		swprintf_s(wszMsg, _countof(wszMsg), L"\n\n%s", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nFailMsgID ) );

		// 아이템ID가 0일때는 획득 아이템이 없을때다.
		ShowItemAlarmDialog( wszMsg, NULL, 0, textcolor::WHITE, fTimer );
	}

	/*
	if( m_pMovieDlg )
	{
		m_pMovieDlg->Show( true );
		m_pMovieDlg->PlayMovie( szMovieFileName, fTimer );
	}
	*/
}

void CDnInterface::TogglePVPMapDlg()
{
	/*if( m_pPVPZoneMapDlg )
	{
		m_pPVPZoneMapDlg->SetMapIndex(-1);
		m_pPVPZoneMapDlg->Show(!m_pPVPZoneMapDlg->IsShow());
	}*/
	if( m_pGuildWarZoneMapDlg )
	{
		m_pGuildWarZoneMapDlg->Show( !m_pGuildWarZoneMapDlg->IsShow() );
	}
}

void CDnInterface::ToggleShowAllPVPPlayerInMap( bool bShow )
{
	if( m_pPVPZoneMapDlg )
		m_pPVPZoneMapDlg->SetAllPlayerShow( bShow );

	if( CDnMinimap::IsActive() )
	{
		GetMiniMap().SetAllPlayerShow( bShow );
	}	
}


void CDnInterface::OpenFinalResultDlg()
{
	CloseMessageBox();
	CloseAllMainMenuDialog();

	if( m_pPVPGameResultDlg )
	{
		m_pPVPGameResultDlg->FinalResultOpen();	
	}
	else if(m_pPVPGameResultAllDlg)
	{
		m_pPVPGameResultAllDlg->FinalResultOpen();
	}
	else if(m_pGuildWarResultDlg)
	{
		m_pGuildWarResultDlg->FinalResultOpen();
	}
#if defined( PRE_ADD_RACING_MODE )
	else if( m_pPvPRacingResultDlg )
	{
		m_pPvPRacingResultDlg->FinalResultOpen();
	}
#endif	// #if defined( PRE_ADD_RACING_MODE )
#ifdef PRE_ADD_PVP_TOURNAMENT
	else if (m_pPVPTournamentFinalResultDlg)
	{
		m_pPVPTournamentFinalResultDlg->FinalResultOpen();
	}
#endif
}

void CDnInterface::OpenNpcTalkReturnDlg()
{
	if( !m_pNpcTalkReturnDlg ) return;

	m_pNpcTalkReturnDlg->Show( true );
}

void CDnInterface::CloseNpcTalkReturnDlg()
{
	if( !m_pNpcTalkReturnDlg ) return;

	m_pNpcTalkReturnDlg->Show( false );
}
#ifdef PRE_MOD_ITEM_COMPOUND_DIALOG_RENEWAL
void CDnInterface::ShowNpcTalkReturnDlgButton( bool bShow )
{
	if( !m_pNpcTalkReturnDlg ) return;

	m_pNpcTalkReturnDlg->ShowBackButton( bShow );
}
#endif

#ifdef PRE_MOD_PVP_LADDER_XP
void CDnInterface::SetPVPXP( UINT nSessionID, UINT uiAddXPScore, UINT uiTotalXPScore, UINT uiAddMedalScore, UINT uiAddLadderXPScore )
#else // PRE_MOD_PVP_LADDER_XP
void CDnInterface::SetPVPXP( UINT nSessionID, UINT uiAddXPScore, UINT uiTotalXPScore, UINT uiAddMedalScore )
#endif // PRE_MOD_PVP_LADDER_XP
{
	if(m_pPVPGameResultAllDlg)
	{
		m_pPVPGameResultAllDlg->SetPVPXP( nSessionID, uiAddXPScore,uiTotalXPScore,uiAddMedalScore );
	}
	if(m_pGuildWarResultDlg)
	{
		m_pGuildWarResultDlg->SetPVPXP( nSessionID, uiAddXPScore,uiTotalXPScore,uiAddMedalScore );
	}
#ifdef PRE_MOD_PVP_LADDER_XP
	if( m_pPVPGameResultDlg )
	{
		if( IsLadderMode() )
			m_pPVPGameResultDlg->SetPVPXP( nSessionID, uiAddXPScore, uiTotalXPScore, uiAddMedalScore, uiAddLadderXPScore );
		else
			m_pPVPGameResultDlg->SetPVPXP( nSessionID, uiAddXPScore, uiTotalXPScore, uiAddMedalScore );
	}
#else // PRE_MOD_PVP_LADDER_XP
	if( m_pPVPGameResultDlg )
		m_pPVPGameResultDlg->SetPVPXP( nSessionID, uiAddXPScore, uiTotalXPScore, uiAddMedalScore );
#endif // PRE_MOD_PVP_LADDER_XP

#if defined( PRE_ADD_RACING_MODE )
	if( m_pPvPRacingResultDlg )
	{
		m_pPvPRacingResultDlg->SetPVPXP( nSessionID, uiAddXPScore, uiTotalXPScore, uiAddMedalScore );
	}
#endif	// #if defined( PRE_ADD_RACING_MODE )
#ifdef PRE_ADD_PVP_TOURNAMENT
	if (m_pPVPTournamentFinalResultDlg)
	{
		m_pPVPTournamentFinalResultDlg->SetPVPXP(nSessionID, uiAddXPScore, uiTotalXPScore, uiAddMedalScore);
	}
#endif
}

CDnChatTabDlg *CDnInterface::GetChatDialog( bool bOtherChatDlg )
{
	if( !m_pPVPChatDlg )
		return m_pChatDlg;

	bool bPVP = IsPVPLobby();
	bPVP ^= bOtherChatDlg;
	return bPVP ? m_pPVPChatDlg : m_pChatDlg;
}

void CDnInterface::ShowPvPKilledMeDlg( bool bShow, WCHAR *wszName, int nKillCount )
{
	if( !m_pPvPKilledMeDlg ) return;

	if( bShow ) {
		m_pPvPKilledMeDlg->Refresh( wszName, nKillCount );
	}
	m_pPvPKilledMeDlg->Show( bShow );
}

void CDnInterface::ShowOccupationWarnning( DnActorHandle hActor, int nStringID, bool bMapAlarm, bool bReaction, bool bMessage, bool bTeamCheck, int nSoundID )
{
	if( !CDnActor::s_hLocalActor ) return;
	CDnLocalPlayerActor * pLocalActor = (CDnLocalPlayerActor *)(CDnActor::s_hLocalActor.GetPointer());
	if( !m_pGuildWarZoneMapDlg || !hActor || !pLocalActor )	return;

	bool bChecked = bTeamCheck ? (pLocalActor->GetTeam() == hActor->GetTeam()) : true;

	if( bMapAlarm && bChecked && m_pGuildWarZoneMapDlg )
		m_pGuildWarZoneMapDlg->AddAlarm( hActor );

	if( bReaction && bChecked )
		pLocalActor->GuildWarAlarm();

	if( bMessage && bChecked )
	{
		if( CDnBridgeTask::GetInstance().IsPvPGameMode( PvPCommon::GameMode::PvP_GuildWar ) && m_pGuildPropInfoDlg )
			m_pGuildPropInfoDlg->ShowInfo( hActor, nStringID );
		if( CDnBridgeTask::GetInstance().IsPvPGameMode( PvPCommon::GameMode::PvP_Occupation ) )
		{
			WCHAR wszString[512];
			swprintf_s( wszString, _countof(wszString), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nStringID ), hActor->GetName() );

			DWORD dwColor = textcolor::WHITE;
			if( PvPCommon::Team::Observer == pLocalActor->GetTeam() )
			{
				if( PvPCommon::Team::A == hActor->GetTeam() )
					dwColor = textcolor::PVP_F_PLAYER;
				else if( PvPCommon::Team::B == hActor->GetTeam() )
					dwColor = textcolor::PVP_E_PLAYER;
			}
			else
			{
				if( hActor->GetTeam() == pLocalActor->GetTeam() )
					dwColor = textcolor::PVP_F_PLAYER;
				else
					dwColor = textcolor::PVP_E_PLAYER;
			}

			AddMessageText( wszString, dwColor );
		}
	}

	if( -1 != nSoundID && bChecked )
		CEtSoundEngine::GetInstance().PlaySound( "2D", nSoundID, false );
}

void CDnInterface::OpenWorldMsgDialog( int nType, INT64 biItemSerial )
{
	if (CDnPartyTask::GetInstance().IsLocalActorEnterGateReady())
		return;

	// 캐시아이템 월드대화. 마을에서만 사용 가능.
	if( nType != 1 && CDnWorld::GetInstance().GetMapType() != CDnWorld::MapTypeVillage )
	{
		GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 100301 ) );
		return;
	}

	if( !m_pWorldMsgDlg ) return;
	if( m_pWorldMsgDlg ) {
		m_pWorldMsgDlg->SetInfo( nType, biItemSerial );
		m_pWorldMsgDlg->Show( true );
	}
}

bool CDnInterface::IsOpenWorldMsgDialog() const
{
	return m_pWorldMsgDlg->IsShow();
}

CDnWorldMsgDlg* CDnInterface::GetWorldMsgDialog()
{
	return m_pWorldMsgDlg;
}

void CDnInterface::CloseWorldMsgDialog()
{
	if (m_pWorldMsgDlg == NULL)
		return;

	if (IsOpenWorldMsgDialog())
		m_pWorldMsgDlg->Show(false);
}

void CDnInterface::ApplyUIHotKey()
{
	// 패킷와서 g_WrappingData에 값 갱신되고 나서 호출
	for( int i = 0; i < UIWrappingKeyIndex_Amount; ++i )
		SetMainUIHotKey( i, _ToVK(g_UIWrappingKeyData[i]) );
	SetRebirthCaptionHotKey( _ToVK(g_WrappingKeyData[IW_REBIRTH]) );
#ifdef _USE_VOICECHAT
	SetToggleMicHotKey( _ToVK(g_UIWrappingKeyData[IW_UI_TOGGLEMIC]) );
#endif
}

void CDnInterface::SetMainUIHotKey( int nUIWrappingKeyIndex, BYTE cVK )
{
	if( !m_pMainBar ) return;
	m_pMainBar->SetMainUIHotKey( nUIWrappingKeyIndex, cVK );
}

void CDnInterface::SetRebirthCaptionHotKey( BYTE cVK )
{
	if( !m_pRebirthCaptionDlg ) return;
	m_pRebirthCaptionDlg->SetRebirthCaptionHotKey( cVK );
}

#ifdef _USE_VOICECHAT
void CDnInterface::SetToggleMicHotKey( BYTE cVK )
{
	if( !m_pPlayerGaugeDlg ) return;
	m_pPlayerGaugeDlg->SetToggleMicHotKey( cVK );
}
#endif

void CDnInterface::ShowDungeonOpenNoticeDialog( bool bShow )
{
	if( !m_pDungeonOpenNoticeDlg ) return;
	if( !bShow ) {
		m_pDungeonOpenNoticeDlg->Show( bShow );
		return;
	}

	CDnCommonTask *pTask = (CDnCommonTask *)CTaskManager::GetInstance().GetTask( "CommonTask" );
	if( !pTask ) return;
	if( pTask->GetDungeonOpenNoticeCount() == 0 ) return;

	m_pDungeonOpenNoticeDlg->Show( bShow );

	pTask->ClearDungeonOpenNotice();
}

void CDnInterface::OpenStoreConfirmExDlg(CDnSlotButton* pFromSlot, emSTORE_CONFIRM_TYPE type, CEtUICallback* pCallback)
{
	if (m_pSplitConfirmExDlg == NULL || pFromSlot ==NULL)
		return;

	m_pSplitConfirmExDlg->Reset();
	m_pSplitConfirmExDlg->SetItem(pFromSlot, type);
	m_pSplitConfirmExDlg->SetCallback(pCallback);
	m_pSplitConfirmExDlg->Show(true);
}

void CDnInterface::EnableStoreConfirmExDlg(bool bEnable)
{
	if (m_pSplitConfirmExDlg == NULL || m_pSplitConfirmExDlg->IsShow() == false)
		return;

	m_pSplitConfirmExDlg->EnalbleButtons(bEnable);
}

void CDnInterface::CloseStoreConfirmExDlg()
{
	m_pSplitConfirmExDlg->Show(false);
}

void CDnInterface::ShowEventToday()
{
	if( m_pMainMenuDlg ) m_pMainMenuDlg->OpenEventToday();
}

void CDnInterface::CloseEventToday()
{
	if( m_pMainMenuDlg ) m_pMainMenuDlg->CloseEventToday();
}

void CDnInterface::OnPartyJoinFailed()
{
#ifdef PRE_MOD_REQ_JOIN_PARTY_ANSWER_MSG_APP
	if( GetDirectDlg() && GetDirectDlg()->IsShow() )
		GetDirectDlg()->OnPartyJoinFailed();
	else
	{
		if( !m_pMainMenuDlg ) return;
		m_pMainMenuDlg->OnPartyJoinFailed();
	}
#else
	if( !m_pMainMenuDlg ) return;
	m_pMainMenuDlg->OnPartyJoinFailed();
#endif 
}


void CDnInterface::CloseNameLinkToolTipDlg()
{
	if (m_pNameLinkToolTipDlg && m_pNameLinkToolTipDlg->IsShow())
		m_pNameLinkToolTipDlg->Show(false);
}

#ifdef PRE_ADD_VIP
const CDnVIPDataMgr* CDnInterface::GetLocalPlayerVIPDataMgr() const
{
	if (CDnLocalPlayerActor::s_hLocalActor)
	{
		CDnLocalPlayerActor *pLocalActor = static_cast<CDnLocalPlayerActor *>(CDnLocalPlayerActor::s_hLocalActor.GetPointer());
		if (pLocalActor != NULL)
			return &(pLocalActor->GetVIPDataMgr());
	}

	return NULL;
}

CDnVIPDataMgr* CDnInterface::GetLocalPlayerVIPDataMgr()
{
	if (CDnLocalPlayerActor::s_hLocalActor)
	{
		CDnLocalPlayerActor *pLocalActor = static_cast<CDnLocalPlayerActor *>(CDnLocalPlayerActor::s_hLocalActor.GetPointer());
		if (pLocalActor != NULL)
			return &(pLocalActor->GetVIPDataMgr());
	}

	return NULL;
}
#endif // PRE_ADD_VIP

bool CDnInterface::SetNameLinkChat(const CDnItem& item)
{
	CDnNameLinkMng* pNameLinkMng = GetNameLinkMng();
	if (pNameLinkMng == NULL)
	{
		_ASSERT(0);
		return false;
	}

	std::wstring itemNameLink;
	CDnNameLinkMng& nameLinkMng = *pNameLinkMng;
	nameLinkMng.MakeNameLinkString_Item(itemNameLink, item);

	if (m_pWorldMsgDlg && m_pWorldMsgDlg->IsShow())
	{
		m_pWorldMsgDlg->AppendChatEditBox(itemNameLink.c_str(), false);
		return true;
	}

	if( m_pChatRoomDlg && m_pChatRoomDlg->IsShow() )
	{
		m_pChatRoomDlg->AppendChatEditBox(itemNameLink.c_str(), false);
		return true;
	}

	CDnChatTabDlg* pChatDlg = GetChatDialog();
	if (pChatDlg)
	{
		if (pChatDlg->IsShow() == false)
			pChatDlg->ShowChatDlg();

		pChatDlg->AppendChatEditBox(itemNameLink.c_str(), false);
	}

	return true;
}

CDnContextMenuDlg* CDnInterface::GetContextMenuDlg(eContextMenuType type)
{
	std::map<int, CDnContextMenuDlg*>::iterator iter = m_pContextMenuDlgList.find(type);
	if (iter != m_pContextMenuDlgList.end())
	{
		CDnContextMenuDlg* pDlg = (*iter).second;
		return pDlg;
	}

	return NULL;
}

void CDnInterface::RegisterMsgListenDlg(int protocol, CEtUIDialog* pDlg)
{
	std::multimap<int, CEtUIDialog*>::iterator iter = m_pMsgListenDlgList.lower_bound(protocol);
	for (; iter != m_pMsgListenDlgList.upper_bound(protocol);)
	{
		CEtUIDialog* pCurDlg = (*iter).second;
		if (pCurDlg)
		{
			if (pCurDlg == pDlg)
				return;

			++iter;
		}
		else
		{
			iter = m_pMsgListenDlgList.erase(iter);
		}
	}

	m_pMsgListenDlgList.insert(std::make_pair(protocol, pDlg));
}

void CDnInterface::HandleMsgListenDlg(int protocol, char* pData)
{
	std::multimap<int, CEtUIDialog*>::iterator iter = m_pMsgListenDlgList.lower_bound(protocol);
	for (; iter != m_pMsgListenDlgList.upper_bound(protocol);)
	{
		CEtUIDialog* pCurDlg = (*iter).second;
		if (pCurDlg)
			pCurDlg->OnHandleMsg(protocol, pData);
		iter = m_pMsgListenDlgList.erase(iter);
	}
}

void CDnInterface::InitializeMODDialog( int nModTableID )
{
	FinalizeMODDialog();

	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TMODDLG );
	if( !pSox ) return;
	if( !pSox->IsExistItem( nModTableID ) ) return;

	CDnMODDlgBase::MODDlgType Type = (CDnMODDlgBase::MODDlgType)pSox->GetFieldFromLablePtr( nModTableID, "_Type" )->GetInteger();
	std::string szFileName = (char*)pSox->GetFieldFromLablePtr( nModTableID, "_FileName" )->GetString();

	m_pMODDlg = CDnMODDlgBase::CreateMODDlg( Type, szFileName.c_str() );
	m_pMODDlg->Initialize( true );
	
#ifdef PRE_ADD_CRAZYDUC_UI
	
	CDnMODCustom3Dlg* pMod3Custom3Dlg = static_cast<CDnMODCustom3Dlg*>( m_pMODDlg );
	CDnGameTask * pGameTask = (CDnGameTask*)(CTaskManager::GetInstance().GetTask("GameTask"));
	if( Type == CDnMODDlgBase::TimerAndCounter && pMod3Custom3Dlg && pGameTask && pGameTask->IsShowMODDialog() )
	{
		pMod3Custom3Dlg->Play( true );
		pMod3Custom3Dlg->SetTimer( pGameTask->GetTimeAttackInfo().nRemain, pGameTask->GetTimeAttackInfo().nOrigin );
	}

#endif 
}

void CDnInterface::FinalizeMODDialog()
{
	SAFE_DELETE( m_pMODDlg );
}

void CDnInterface::ShowMODDialog( bool bShow )
{
	if( m_pMODDlg ) m_pMODDlg->Show( bShow );
}

#ifdef PRE_ADD_MONSTER_PARTS_UI_TRIGGER
void CDnInterface::SetLinkMODValue( CEtTriggerElement *pElement, int nIndex, int nDefineValueIndex, int nGaugeType, int nPartsIndex )
{
	if( !m_pMODDlg ) return;

	m_pMODDlg->LinkValue( pElement, nIndex, nDefineValueIndex, nGaugeType, nPartsIndex );
}
#else
void CDnInterface::SetLinkMODValue( CEtTriggerElement *pElement, int nIndex, int nDefineValueIndex )
{
	if( !m_pMODDlg ) return;

	m_pMODDlg->LinkValue( pElement, nIndex, nDefineValueIndex );
}
#endif 

void CDnInterface::ShowTimeEventDialog( bool bShow )
{
	if( !m_pMainMenuDlg ) return;

	if( bShow ) m_pMainMenuDlg->Show( true );
	m_pMainMenuDlg->ShowTimeEventDialog( bShow );
}

void CDnInterface::ShowHelpAlarmDialog( int nHelpTableID )
{
	if( !m_pHelpAlarmDlg ) return;

	m_pHelpAlarmDlg->SetAlarm( nHelpTableID );
	m_pHelpAlarmDlg->Show( true );
}

void CDnInterface::RefreshMasterListDialog()
{
	if( m_pMainMenuDlg )
		m_pMainMenuDlg->RefreshMasterListDialog();
}

void CDnInterface::RefreshMasterSimpleInfo()
{
	if( m_pMainMenuDlg )
		m_pMainMenuDlg->RefreshMasterSimpleInfo();
}

void CDnInterface::RefreshMasterCharacterInfo()
{
	if( m_pMainMenuDlg )
		m_pMainMenuDlg->RefreshMasterCharacterInfo();
}

void CDnInterface::RefreshMasterClassmate()
{
	if( m_pMainMenuDlg )
		m_pMainMenuDlg->RefreshMasterClassmate();
}

void CDnInterface::RefreshPupilList()
{
	if( m_pMainMenuDlg )
		m_pMainMenuDlg->RefreshPupilList();
}


void CDnInterface::RefreshMyMasterInfo()
{
	if( m_pMainMenuDlg )
		m_pMainMenuDlg->RefreshMyMasterInfo();
}

void CDnInterface::RefreshClassmateInfo()
{
	if( m_pMainMenuDlg )
		m_pMainMenuDlg->RefreshClassmateInfo();
}

void CDnInterface::RefreshPupilLeave()
{
	if( m_pMainMenuDlg )
		m_pMainMenuDlg->RefreshPupilLeave();
}

void CDnInterface::RefreshRespectPoint()
{
	if( m_pMainMenuDlg )
		m_pMainMenuDlg->RefreshRespectPoint();
}

void CDnInterface::ResetKeepMasterInfo()
{
	if( m_pPlayerGaugeDlg )
		m_pPlayerGaugeDlg->ResetKeepMasterInfo();
}

bool CDnInterface::IsShowItemUnsealDialog() const
{
	if (m_pItemUnsealDlg)
		return m_pItemUnsealDlg->IsShow();
	return false;
}

bool CDnInterface::IsShowItemSealDialog() const
{
	if (m_pItemSealDlg)
		return m_pItemSealDlg->IsShow();
	return false;
}

#ifdef PRE_MOD_ITEM_COMPOUND_DIALOG_RENEWAL
void CDnInterface::ForceShowCashShopMenuDlg(bool bShow )
{
	if (m_pCashShopMenuDlg ) m_pCashShopMenuDlg->Show( bShow );
}
#endif 

void CDnInterface::DisableCashShopMenuDlg(bool bDisable)
{
	if (m_pCashShopMenuDlg && m_pCashShopMenuDlg->IsShow())
		m_pCashShopMenuDlg->Disable(bDisable);
}

#ifdef PRE_MOD_CREATE_CHAR
void CDnInterface::SetCharCreatePartsName( CDnParts::PartsTypeEnum PartsIndex, std::wstring strPartsName )
{
#ifdef _ADD_NEWLOGINUI
	if (m_pCharCreateSetupDlg)
		m_pCharCreateSetupDlg->SetCharCreatePartsName(PartsIndex, strPartsName);
#else
	if( m_pCharCreateDlg )
		m_pCharCreateDlg->SetCharCreatePartsName( PartsIndex, strPartsName );
#endif
}
#endif // PRE_MOD_CREATE_CHAR

#ifndef PRE_MOD_SELECT_CHAR
void CDnInterface::ShowCharSelectArrowLeft( bool bShow )
{
	if( m_pCharSelectDlg ) m_pCharSelectDlg->ShowCharSelectArrowLeft( bShow );
}

void CDnInterface::ShowCharSelectArrowRight( bool bShow )
{
	if( m_pCharSelectDlg ) m_pCharSelectDlg->ShowCharSelectArrowRight( bShow ); 
}
#endif // PRE_MOD_SELECT_CHAR

DWORD CDnInterface::GetCharNameColor(const WCHAR* pName) const
{
	if (CDnFriendTask::GetInstance().GetFriendInfo(pName)) 
		return textcolor::FRIENDNAME;
	if (CDnMasterTask::GetInstance().IsMasterPlayer(pName))
		return textcolor::MASTERNAME;
	if (CDnGuildTask::GetInstance().GetGuildMemberFromName(pName))
		return textcolor::GUILDMEMBERNAME;
	if (CDnIsolateTask::GetInstance().IsBlackList(pName))
		return textcolor::ISOLATENAME;
	return textcolor::WHITE;
}

void CDnInterface::RefreshQuestSummaryInfo()
{
	if( m_pMainMenuDlg )
	{
		m_pMainMenuDlg->EnalbeQuestSumPriority(CGameOption::GetInstance().m_bEnableQuestNotifyPriority ? (CDnWorld::GetInstance().GetMapType() == CDnWorld::MapTypeDungeon) : false);
		m_pMainMenuDlg->RefreshQuestSumInfo();
	}
}


void CDnInterface::ShowPetalTokenDialog( bool bShow, CDnItem *pItem )
{
	if( !m_pItemPetalTokenDlg ) return;

	m_pItemPetalTokenDlg->SetPetalTokenItem( pItem );
	m_pItemPetalTokenDlg->Show( bShow );
}

void CDnInterface::ShowAppellationGainDialog( bool bShow, CDnItem *pItem )
{
	if( !m_pItemAppellationGainDlg ) return;

	m_pItemAppellationGainDlg->SetAppellationGainItem( pItem );
	m_pItemAppellationGainDlg->Show( bShow );
}


#if defined( PRE_ADD_EASYGAMECASH )
void CDnInterface::ShowPackageProgressDlg( char cInvenType, BYTE cInvenIndex, INT64 biInvenSerial )
{
	if( NULL == m_pPackageBoxProgressDlg || NULL == m_pPackageBoxResultDlg )
		return;

	float fTime = CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::AllGiveCharmItemOpenTime);

	m_pPackageBoxProgressDlg->ShowEx( true, cInvenType, cInvenIndex, biInvenSerial, fTime * 0.001f );
#ifdef PRE_FIX_MAKECHARMITEM
	m_pPackageBoxResultDlg->SetInvenData( cInvenType, cInvenIndex, biInvenSerial );
#else
	m_pPackageBoxResultDlg->SetInvenData( cInvenType, cInvenIndex );
#endif
}

#ifdef PRE_ADD_GETITEM_WITH_RANDOMVALUE
void CDnInterface::SetPackageBoxInfo( char cInvenType, short sInvenIndex, TRangomGiveItemData* pItemlist, int nitemCount )
{
	m_pPackageBoxResultDlg->SetInvenData( cInvenType, sInvenIndex, pItemlist, nitemCount );
}

void CDnInterface::ShowRandomCharmItemProgressDialog( bool bShow, char cInvenType, short sInvenIndex, INT64 biInvenSerial, TRangomGiveItemData* pItemlist, int nitemCount )
{	
	// 프로그레스바는 CharmItemProgressDlg 를 사용하고 결과창은 PackageBox 를 사용
	GetInterface().SetPackageBoxInfo( cInvenType, sInvenIndex, pItemlist, nitemCount );
	
	CDnCharmItemKeyDlg::SCharmKeyInfo keyInfo = GetInterface().GetCharmItemKeyDlg()->GetCharmKeyInfo();
	GetInterface().ShowCharmItemProgressDialog( bShow, cInvenType, sInvenIndex, biInvenSerial, keyInfo.cKeyIndexIndex, keyInfo.nKeyItemID, keyInfo.biKeyItemSerial );
}
#endif 

void CDnInterface::ShowPackageBoxDlg( bool bShow )
{
	if( NULL == m_pPackageBoxProgressDlg || NULL == m_pPackageBoxResultDlg )
		return;

	m_pPackageBoxProgressDlg->Show( false );
	m_pPackageBoxResultDlg->Show( bShow );
}
#endif	// #if defined( PRE_ADD_EASYGAMECASH )

void CDnInterface::ShowLifeStorageDialog( bool bShow )
{
	if( !m_pMainMenuDlg )
		return;

	m_pMainMenuDlg->Show(true);
	m_pMainMenuDlg->ShowLifeStorageDialog( bShow );

	OpenNpcTalkReturnDlg();
}

#ifdef PRE_ADD_COOKING_SYSTEM
void CDnInterface::OpenCookingDialog( bool bClearSelectedRecipe )
{
	if( !m_pMainMenuDlg ) return;

	// 거래 제재 검사
	if( GetRestraintTask().CheckRestraint( _RESTRAINTTYPE_TRADE, true ) )
		return;

	m_pMainMenuDlg->Show( true );
	m_pMainMenuDlg->OpenCookingDialog( bClearSelectedRecipe );
}

void CDnInterface::CloseCookingDialog()
{
	if( m_pMainMenuDlg )
		m_pMainMenuDlg->CloseCookingDialog();
}

bool CDnInterface::IsGateQuestionDlgShow()
{
	if( m_pGateQuestionDlg && m_pGateQuestionDlg->IsShow() )
		return true;

	return false;
}
#endif // PRE_ADD_COOKING_SYSTEM

CDnDungeonEnterDlg *CDnInterface::GetCurrentDungeonEnterDialog()
{
	if( m_pDungeonEnterDlg && m_pDungeonEnterDlg->IsShow() ) return m_pDungeonEnterDlg;
	if( m_pEventDungeonEnterDlg && m_pEventDungeonEnterDlg->IsShow() ) return m_pEventDungeonEnterDlg;
	if( m_pDungeonNestEnterDlg && m_pDungeonNestEnterDlg->IsShow() ) return m_pDungeonNestEnterDlg;
	return NULL;
}

void CDnInterface::ShowBossAlertDialog( bool bShow, WCHAR *wszName, const char *szImageFileName )
{
	if( !m_pBossAlertDlg ) return;
	if( bShow ) {
		m_pBossAlertDlg->SetBoss( wszName, szImageFileName );
	}
	m_pBossAlertDlg->Show( bShow );
}

#ifdef PRE_ADD_CASHSHOP_REFUND_CL
CDnCashShopRefundInvenDlg* CDnInterface::GetCashShopRefundInven()
{
	return m_pCashShopDlg->GetRefundInven();
}
#endif


void CDnInterface::ShowDurabilityGauge(DnActorHandle hActor, int nStateBlowID, float fDurability, bool bShow)
{
	if (m_pDurabilityGaugeMng)
		m_pDurabilityGaugeMng->ShowDurability(hActor, nStateBlowID, fDurability, bShow);
}

#if defined(PRE_ADD_23829)
void CDnInterface::Show2ndPassCreateButton(bool bShow)
{
	if (m_pCharSelectDlg)
		dynamic_cast<CDnCharSelectDlg*>(m_pCharSelectDlg)->Show2ndPassCreateButton(bShow);
}
#endif

#ifdef PRE_ADD_GUILD_EASYSYSTEM
// idea from http://support.microsoft.com/kb/242308/en-us  
ULONG CDnInterface::ProcIDFromWnd(HWND hwnd) // 윈도우 핸들로 프로세스 아이디 얻기  
{  
	ULONG idProc;  
	GetWindowThreadProcessId( hwnd, &idProc );  
	return idProc;  
}  

HWND CDnInterface::GetWinHandle(ULONG pid) // 프로세스 아이디로 윈도우 핸들 얻기  
{  
	HWND tempHwnd = FindWindow(NULL,NULL); // 최상위 윈도우 핸들 찾기  

	while( tempHwnd != NULL )  
	{  
		if( GetParent(tempHwnd) == NULL ) // 최상위 핸들인지 체크, 버튼 등도 핸들을 가질 수 있으므로 무시하기 위해  
			if( pid == ProcIDFromWnd(tempHwnd) )  
				return tempHwnd;  
		tempHwnd = GetWindow(tempHwnd, GW_HWNDNEXT); // 다음 윈도우 핸들 찾기  
	}  
	return NULL;  
}

void CDnInterface::OpenSimpleBrowser(const std::wstring& wUrl)
{
	BOOL result;

	RECT rt={0, 0, 800, 600};
	GetWindowRect(CDnMainFrame::GetInstance().GetHWnd(), &rt);

	STARTUPINFO si;  
	ZeroMemory(&si,sizeof(si));  
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USEPOSITION | STARTF_USESIZE | STARTF_USESHOWWINDOW;
	si.dwXSize = rt.right - rt.left;
	si.dwYSize = rt.bottom - rt.top;
	si.dwX = rt.left;
	si.dwY = rt.top;

	si.wShowWindow = SW_SHOWDEFAULT;

	PROCESS_INFORMATION pi;  
	WCHAR appPath[255];

	HKEY hkey;
	LONG ReturnValue=RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\IEXPLORE.EXE", 0, KEY_ALL_ACCESS, &hkey);
	if(ReturnValue == ERROR_SUCCESS)
	{
		DWORD dwType;
		DWORD dwSize = 200;
		
		LONG lReturn = RegQueryValueEx (hkey, L"", NULL, &dwType, (BYTE *)appPath, &dwSize);
				
		if(lReturn == ERROR_SUCCESS )
		{
			RegCloseKey(hkey);
		}
	}
	
	std::wstring command = appPath;
	command += L" ";				// 공백
	command += L"\"";				// "시작
	command += wUrl;				// url 주소
	command += L"\"";				// "끝
	
	result = CreateProcess(NULL, (LPTSTR)command.c_str(), NULL, NULL, true, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi);

	DWORD errorID = 0;
	if (result == FALSE)
	{
		errorID = GetLastError();
	}
	else
	{
		HANDLE hProcess = pi.hProcess;
		DWORD dResult = WaitForSingleObject( pi.hProcess, 5 * 100 );

		HWND hWnd = GetWinHandle(pi.dwProcessId); // 프로세스 아이디로 윈도우 핸들 얻기  
		if (hWnd != NULL)
		{
			WINDOWINFO winInfo;
			GetWindowInfo(hWnd, &winInfo);
			AdjustWindowRectEx(&rt, winInfo.dwStyle, false, winInfo.dwExStyle);

			BOOL bRet = SetWindowPos(hWnd, HWND_TOP, rt.left, rt.top, rt.right - rt.left, rt.bottom - rt.top, SWP_NOSIZE);
			if (bRet == 0)
			{
				// todo by kalliste
			}
		}
	}
}
#endif // PRE_ADD_GUILD_EASYSYSTEM

void CDnInterface::OpenBrowser(std::string& url, float fPageWidth, float fPageHeight, eBrowserPosType posType, eGeneralBrowserType type)
{
#ifdef PRE_ADD_GUILD_EASYSYSTEM
	if (type == eGBT_SIMPLE)
	{
		std::wstring wUrl = L"";
		wUrl.assign(url.begin(), url.end());
		OpenSimpleBrowser(wUrl);
	}
	else
	{
		if (posType == eBPT_CENTER)
		{
			if (CDnCashShopTask::IsActive() == false)
				return;

			SUICoord coord;
			coord.fX = (CEtDevice::GetInstance().Width() - fPageWidth) * 0.5f;
			coord.fY = (CEtDevice::GetInstance().Height() - fPageHeight) * 0.5f;
			coord.fWidth = fPageWidth;
			coord.fHeight = fPageHeight;

			if (type == eGBT_GENERAL)
			{
				OpenBlind();
				CDnLocalPlayerActor::LockInput(true);
			}

			CDnCashShopTask::GetInstance().OpenBrowser(url, coord, type, L"");
		}
		else
		{
			_ASSERT(0);
		}
	}
#else
	if (posType == eBPT_CENTER)
	{
		if (CDnCashShopTask::IsActive() == false)
			return;

		SUICoord coord;
		coord.fX = (CEtDevice::GetInstance().Width() - fPageWidth) * 0.5f;
		coord.fY = (CEtDevice::GetInstance().Height() - fPageHeight) * 0.5f;
		coord.fWidth = fPageWidth;
		coord.fHeight = fPageHeight;

		if (type == eGBT_GENERAL)
		{
			OpenBlind();
			CDnLocalPlayerActor::LockInput(true);
		}

		CDnCashShopTask::GetInstance().OpenBrowser(url, coord, type, L"");
	}
	else
	{
		_ASSERT(0);
	}
#endif

	return;
}

#ifdef PRE_MOD_BROWSER
void CDnInterface::OnCloseBrowser(eBrowserType type)
{
	if (type == eBT_NPCBROWSER)
	{
		CloseNpcDialog();
	}
	else if (type != eGBT_SIMPLE)
	{
		CloseBlind(true);
		CDnLocalPlayerActor::LockInput(false);
	}
}

void CDnInterface::OnWndProcForInternetBrowser(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	m_InternetBrowserMgr.WndProc(hWnd, message, wParam, lParam);
}
#else
void CDnInterface::OnCloseBrowser(eGeneralBrowserType type)
{
	if (type == eGBT_NPCBROWSER)
	{
		CloseNpcDialog();
	}
#ifdef PRE_ADD_GUILD_EASYSYSTEM
	else if (type != eGBT_SIMPLE)
#else
	else
#endif
	{
		CloseBlind(true);
		CDnLocalPlayerActor::LockInput(false);
	}
}
#endif // PRE_MOD_BROWSER

bool CDnInterface::IsShowChannelDlg() const
{
	CDnSystemDlg *pSystemDlg = (CDnSystemDlg*)m_pMainMenuDlg->GetMainDialog( CDnMainMenuDlg::SYSTEM_DIALOG );
	if( pSystemDlg )
		return pSystemDlg->IsOpenChannelDlg();

	return false;
}

void CDnInterface::ShowPetExtendPeriodDlg( CDnItem* pItem, bool bShow )
{
	if( m_pPetExtendPeriodDlg )
	{
		m_pPetExtendPeriodDlg->SetExtendPeriodItem( pItem );
		m_pPetExtendPeriodDlg->Show( bShow );
	}
}

void CDnInterface::ShowPetAddSkillDlg( CDnItem* pItem, bool bShow )
{
	if( m_pPetAddSkillDlg )
	{
		m_pPetAddSkillDlg->SetSkillItem( pItem );
		m_pPetAddSkillDlg->Show( bShow );
	}
}

void CDnInterface::ShowPetExpandSkillSlotDlg( CDnItem* pItem, bool bShow )
{
	if( m_pPetExpandSkillSlotDlg )
	{
		m_pPetExpandSkillSlotDlg->SetExpandSkillSlotItem( pItem );
		m_pPetExpandSkillSlotDlg->Show( bShow );
	}
}

#ifdef PRE_ADD_PET_EXTEND_PERIOD
void CDnInterface::ShowBuyPetExtendPeriodItemDlg( bool bShow, INT64 nPetSerialID )
{
	if( m_pBuyPetExtendPeriodDlg )
	{
		m_pBuyPetExtendPeriodDlg->SetPetSerialID( nPetSerialID );
		m_pBuyPetExtendPeriodDlg->Show( bShow );
	}
}
#endif // PRE_ADD_PET_EXTEND_PERIOD

void CDnInterface::RefreshPetFoodCount()
{
	if( m_pMainBar )
	{
		m_pMainBar->RefreshPetFoodCount();
	}

	CDnPetOptionDlg* pPetOptionDlg = NULL;
	CDnCharStatusDlg* pCharStatusDlg = (CDnCharStatusDlg *)GetMainMenuDialog( CDnMainMenuDlg::CHARSTATUS_DIALOG );
	if( pCharStatusDlg )
	{
		CDnCharPetDlg* pCharPetDlg = pCharStatusDlg->GetPetDlg();
		if( pCharPetDlg )
		{
			pPetOptionDlg = pCharPetDlg->GetPetOptionDlg();
			if( pPetOptionDlg )
				pPetOptionDlg->RefreshPetFoodCount();
		}
	}
}

bool CDnInterface::IsFading() const
{
	if( m_pFadeDlg && (m_pFadeDlg->GetFadeMode() != CDnFadeInOutDlg::modeEnd) )
		return true;

	return false;
}

void CDnInterface::ProgressSimpleMsgBox(LPCWSTR pwszMessage, float heartBeatMsgSec, bool bLoadingAni, int nID, CEtUICallback *pCall, float aniSecForRound)
{
	if (m_pProgressSimpleMsgBox == NULL)
		return;

	if (aniSecForRound > 0.f)
		m_pProgressSimpleMsgBox->SetMessageBox(pwszMessage, heartBeatMsgSec, aniSecForRound, nID, pCall);
	else
		m_pProgressSimpleMsgBox->SetMessageBox(pwszMessage, heartBeatMsgSec, bLoadingAni, nID, pCall);

	m_pProgressSimpleMsgBox->Show(true);
}

CDnProgressSimpleMessageBox* CDnInterface::GetProgressSimpleMsgBox() const
{
	return m_pProgressSimpleMsgBox;
}

void CDnInterface::CloseProgressSimpleMsgBox()
{
	if (m_pProgressSimpleMsgBox)
		m_pProgressSimpleMsgBox->Show(false);
}

#ifdef PRE_ADD_LEVELUP_GUIDE
void CDnInterface::ShowLevelUpGuide(bool bShow, int nNewLevel)
{
	if (m_pLevelUpGuildeDlg->IsShow())
		m_pLevelUpGuildeDlg->Show(false);

	m_pLevelUpGuildeDlg->ShowOnLevelUp(bShow, nNewLevel);
}
#endif

#if defined(PRE_ADD_REMOVE_PREFIX)
void CDnInterface::ShowRemovePrefixDialog( bool bShow, CDnItem *pItem )
{
	if( !m_pRemovePrefixDlg ) return;
	if( bShow ) m_pRemovePrefixDlg->SetItem( pItem );
	m_pRemovePrefixDlg->Show( bShow );
}
#endif // PRE_ADD_REMOVE_PREFIX

void CDnInterface::ShowHardCoreModeTimeAttackDlg( bool bShow )
{
	if( m_pDnHardCoreModeTimeAttackDlg )
		m_pDnHardCoreModeTimeAttackDlg->Show( bShow );
}

void CDnInterface::SetHardCoreModeRemainTime( int nOriginTime, int nRemainTime )
{
	if( m_pDnHardCoreModeTimeAttackDlg )
		m_pDnHardCoreModeTimeAttackDlg->SetRemainTime( nOriginTime, nRemainTime );
}

void CDnInterface::ResetHardCoreModeTimer()
{
	if( m_pDnHardCoreModeTimeAttackDlg )
		m_pDnHardCoreModeTimeAttackDlg->ResetTimer();
}

void CDnInterface::SetHardCoreModeHurryUpTime( int nTime )
{
	if( m_pDnHardCoreModeTimeAttackDlg )
		m_pDnHardCoreModeTimeAttackDlg->SetHurryTime( nTime );
}

void CDnInterface::JoypadPostProcess()
{
	if( m_bJoypadPushed )
	{
		if( IsPushJoypadButton( IW_UI( IW_UI_QUICKSLOT1 ) ) || IsPushJoypadButton( IW_UI( IW_UI_QUICKSLOT2 ) ) ||
			IsPushJoypadButton( IW_UI( IW_UI_QUICKSLOT3 ) ) || IsPushJoypadButton( IW_UI( IW_UI_QUICKSLOT4 ) ) ||
			IsPushJoypadButton( IW_UI( IW_UI_QUICKSLOT5 ) ) || IsPushJoypadButton( IW_UI( IW_UI_QUICKSLOT6 ) ) ||
			IsPushJoypadButton( IW_UI( IW_UI_QUICKSLOT7 ) ) || IsPushJoypadButton( IW_UI( IW_UI_QUICKSLOT8 ) ) ||
			IsPushJoypadButton( IW_UI( IW_UI_QUICKSLOT9 ) ) || IsPushJoypadButton( IW_UI( IW_UI_QUICKSLOT10 ) ) ||
			IsPushJoypadButton( IW_UI( IW_UI_QUICKSLOTCHANGE ) ) )
		{
			m_pMainBar->MsgProc( m_pMainBar->GetHWnd(), WM_KEYDOWN, 0 , 0 );
		}
	}

	m_bJoypadPushed = false;
}

#if defined(PRE_ADD_WEEKLYEVENT)
void CDnInterface::RefreshWeeklyEventMark(bool bShow, bool bNew)
{
	if (m_pChannelMoveDlg)
		m_pChannelMoveDlg->SetWeeklyEventMark(bShow, bNew);
	if (m_pDungeonInfoDlg)
		m_pDungeonInfoDlg->SetWeeklyEventMark(bShow, bNew);
}

void CDnInterface::ShowWeeklyEventMarkTooltip(float fMouseX, float fMouseY)
{
	CDnCommonTask *pTask = static_cast<CDnCommonTask *>(CTaskManager::GetInstance().GetTask( "CommonTask" ));
	if( m_pCustomTooltipDlg && !m_pCustomTooltipDlg->IsShow() && pTask )	
	{
		m_pCustomTooltipDlg->SetPosition( fMouseX, fMouseY + ( CDnMouseCursor::GetInstance().GetCursorHeight() * 3/4 )  );
		m_pCustomTooltipDlg->SetWeeklyEventText();
		m_pCustomTooltipDlg->Show( true );
	}
}

void CDnInterface::CloseWeeklyEventMarkTooltip()
{
	if( m_pCustomTooltipDlg )
	{
		m_pCustomTooltipDlg->GetTooltipTextBox()->ClearText();
		m_pCustomTooltipDlg->Show( false );
	}
}

#endif


void CDnInterface::RefreshGuildRewardMark(bool bShow, bool bNew)
{
	if (m_pChannelMoveDlg)
		m_pChannelMoveDlg->SetGuildRewardMark(bShow, bNew);
 	if (m_pDungeonInfoDlg)
 		m_pDungeonInfoDlg->SetGuildRewardMark(bShow, bNew);
}

bool CDnInterface::IsMarkedGuildReward()
{
	bool isMarked = false;

	if (m_pChannelMoveDlg && m_pChannelMoveDlg->IsShow())
		isMarked = m_pChannelMoveDlg->IsMarkGroupTypeInInfoList(MARK_GUILDREWARD_ALERT);

	if (isMarked == false && m_pDungeonInfoDlg && m_pDungeonInfoDlg->IsShow())
		isMarked = m_pDungeonInfoDlg->IsMarkGroupTypeInInfoList(MARK_GUILDREWARD_ALERT);

	return isMarked;
}

void CDnInterface::ShowGuildRewardMarkTooltip(float fMouseX, float fMouseY)
{
	if (m_pGuildRewardMarkTooltipDlg == NULL || CDnGuildWarTask::IsActive() == false)
		return;

	bool bRet = m_pGuildRewardMarkTooltipDlg->SetMarkTooltip();
	if (bRet)
	{
		m_pGuildRewardMarkTooltipDlg->AdjustPosition(fMouseX, fMouseY + ( CDnMouseCursor::GetInstance().GetCursorHeight() * 3/4 ) );
		m_pGuildRewardMarkTooltipDlg->Show(bRet);
	}
}

void CDnInterface::CloseGuildRewardMarkTooltip()
{
	if (m_pGuildRewardMarkTooltipDlg->IsShow())
		m_pGuildRewardMarkTooltipDlg->Show(false);
}

void CDnInterface::UpdateGuildRewardInfo()
{
	if (m_pGuildRewardMarkTooltipDlg)
		m_pGuildRewardMarkTooltipDlg->SetMarkTooltip();
}

void CDnInterface::ShowPlayerGuildInfo(bool bShow)
{
	if (m_pInspectPlayerDlg && m_pInspectPlayerDlg->IsShow())
		m_pInspectPlayerDlg->SwapGuildInfoDlg(bShow);
}

#ifdef PRE_ADD_DONATION
void CDnInterface::OpenContributionDialog( bool bShow )
{	
	if( !m_pMainMenuDlg ) return;

	m_pMainMenuDlg->Show( bShow );
	m_pMainMenuDlg->OpenContributionDialog( bShow );
	
	if( bShow )
		OpenNpcTalkReturnDlg();		
	else
		CloseNpcDialog();		
}

bool CDnInterface::IsOpenDonation()
{
	if( !m_pMainMenuDlg ) 
		return false;

	return m_pMainMenuDlg->IsOpenDonation();
}
#endif


#ifdef PRE_ADD_CHAOSCUBE

bool CDnInterface::IsOpenChaosCubeDlg()
{
	return m_pChaosCubeDlg->IsShow();
}

void CDnInterface::OpenChaosCubeDialog( bool bShow, CDnItem * pItem )
{
	m_pChaosCubeDlg->SetChaoseItem( pItem );
	m_pChaosCubeDlg->Show( bShow );
}

void CDnInterface::OpenChaosCubeProgressDlg( bool bShow, SCChaosCubeRequest * pData )
{	
	//m_pChaosCubeDlg->OpenProgressDlg( bShow, pData );
	m_pChaosCubeProgressDlg->SetRequestData( pData, CDnChaosCubeProgressDlg::ECUBETYPE::ECUBETYPE_CHAOS );
	m_pChaosCubeProgressDlg->Show( bShow );
}

void CDnInterface::OpenResultDlg( bool bShow, int nItemID, int nCount )
{
	m_pChaosCubeResultDlg->SetResultItem( nItemID, nCount );
	m_pChaosCubeResultDlg->Show( bShow );
}

void CDnInterface::UpdateChaosCubeDlg()
{
	m_pChaosCubeDlg->UpdateItemList();
}

void CDnInterface::MixComplete()
{
	m_pChaosCubeDlg->MixComplete();
}

void CDnInterface::RecvResultItemComplet()
{
	m_pChaosCubeDlg->RecvResultItemComplet();
}

void CDnInterface::RecvMixStart( bool bShow )
{
	m_pChaosCubeDlg->RecvMixStart( bShow);
}

void CDnInterface::OpenStuffDlg( bool bShow, int nItemID, int nCount )
{	
	m_pChaosCubeStuffDlg->Show( bShow );
}

void CDnInterface::OpenProgressDlg( bool bShow, int nItemID, int nCount )
{
	m_pChaosCubeProgressDlg->Show( bShow );
}

#endif


#ifdef PRE_ADD_CHOICECUBE

void CDnInterface::RecvChoiceResultItemComplet()
{
	m_pChoiceCubeDlg->RecvResultItemComplet();
}

void CDnInterface::RecvChoiceMixStart( bool bShow )
{
	m_pChoiceCubeDlg->RecvMixStart( bShow);
}

bool CDnInterface::IsOpenChoiceCubeDlg()
{
	return m_pChoiceCubeDlg->IsShow();
}

void CDnInterface::UpdateChoiceCubeDlg()
{
	m_pChoiceCubeDlg->UpdateItemList();
}

void CDnInterface::OpenChoiceCubeDialog( bool bShow, CDnItem * pItem )
{
	m_pChoiceCubeDlg->SetChaoseItem( pItem );
	m_pChoiceCubeDlg->Show( bShow );
}

void CDnInterface::OpenChoiceCubeProgressDlg( bool bShow, SCChaosCubeRequest * pData )
{		
	m_pChaosCubeProgressDlg->SetRequestData( pData, CDnChaosCubeProgressDlg::ECUBETYPE::ECUBETYPE_CHOICE );
	m_pChaosCubeProgressDlg->Show( bShow );
}

#endif


#ifdef PRE_ADD_AUTOUNPACK
void CDnInterface::OpenAutoUnPack( INT64 sn, bool bShow, int nTime )
{
	// 개당열기지연시간 % 설정.
	m_pAutoUnPackDlg->Show( bShow );
	m_pAutoUnPackDlg->SetUnPackTime( nTime );
	m_pAutoUnPackDlg->SetItemSerial( sn );

	/*if( m_pMainMenuDlg )
		m_pMainMenuDlg->OpenAutoUnPackDialog( bShow, nTime );*/
}

bool CDnInterface::IsOpenAutoUnPackDlg(){
	return m_pAutoUnPackDlg->IsShow();
}


void CDnInterface::UnPacking()
{
	m_pAutoUnPackDlg->UnPacking();
}

#endif //PRE_ADD_AUTOUNPACK


#ifdef PRE_ADD_BESTFRIEND

bool CDnInterface::IsOpenBFRegistrationDlg()
{
	if( m_pBFRegistrationDlg )
		return m_pBFRegistrationDlg->IsShow();
	return false;
}

// 절친등록서 아이템 Serial..
INT64 CDnInterface::GetBFserial()
{
	if( m_pBFRegistrationDlg )
		return m_pBFRegistrationDlg->GetSerial();

	return 0;
}

void CDnInterface::OpenBFRegistrationDlg( bool bShow, INT64 serial )
{
	if( m_pBFRegistrationDlg )
	{
		m_pBFRegistrationDlg->SetSerial( serial );
		m_pBFRegistrationDlg->Show( bShow );	
	}
}

void CDnInterface::OpenIdentifyBF( bool bShow )
{
	if( m_pIdentifyBF )
		m_pIdentifyBF->Show( bShow );
}

// 절친확인정보.
void CDnInterface::SetSearchBF( BestFriend::SCSearch * pData )
{
	if( m_pIdentifyBF )
		m_pIdentifyBF->SetSearchBF( pData );
}

// 절친수락 ProgressDlg.
void CDnInterface::OpenBFProgressDlg( bool bShow )
{
	if( m_pBFProgressDlg )
		m_pBFProgressDlg->Show( bShow );	
}

void CDnInterface::SetBFProgressData( BestFriend::SCRegistReq * pData, bool bOpen )
{
	if( m_pBFProgressDlg )
		m_pBFProgressDlg->SetData( pData, bOpen );
}


// 절친정보.
void CDnInterface::SetBFData( struct TBestFriendInfo & info )
{
	CDnCommunityDlg * pCommunityDlg = static_cast< CDnCommunityDlg * >( GetMainMenuDialog( CDnMainMenuDlg::COMMUNITY_DIALOG ) );
	if( pCommunityDlg )
	{
		m_strNameBF.assign( info.wszName );
		m_serialBF = info.biItemSerial;
		pCommunityDlg->SetBFData( info );	
	}
}

const wchar_t * CDnInterface::GetNameBF()
{
	return m_strNameBF.c_str();
}


// 보상아이템Dlg.
void CDnInterface::OpenBFRewardDlg( bool bShow, INT64 giftSerial, int typeParam1 )
{
	if( m_pBFRewardDlg )
	{
		m_pBFRewardDlg->SetGiftInfo( giftSerial, typeParam1 );
		m_pBFRewardDlg->Show( bShow );
	}
}

// 보상아이템ProgressDlg.
void CDnInterface::OpenBFRewardProgressDlg( bool bShow, INT64 giftSerial )
{
	if( m_pBFRewardProgressDlg )
	{
		m_pBFRewardProgressDlg->SetData( giftSerial, m_pBFRewardDlg->GetItemID() );
		m_pBFRewardProgressDlg->Show( true );
	}
}

// 보상아이템지급완료.
void CDnInterface::SendRewardComplet( int nItemID )
{
	if( m_pBFRewardDlg )
		m_pBFRewardDlg->SendRewardComplet( nItemID );
}

// 메모Dlg.
void CDnInterface::OpenBFMemo( bool bShow )
{
	if( m_pBFMemoDlg )
		m_pBFMemoDlg->Show( bShow );
}

// 메모수신.
void CDnInterface::SetMemoBF( BestFriend::SCEditMemo * sMemo )
{
	CDnCommunityDlg * pCommunityDlg = static_cast< CDnCommunityDlg * >( m_pMainMenuDlg->GetMainDialog( CDnMainMenuDlg::COMMUNITY_DIALOG ) );
	if( pCommunityDlg )
	{
		pCommunityDlg->SetMemo( sMemo->bFromMe, sMemo->wszMemo );
	}
}

// 절친파기 or 파기취소.
void CDnInterface::BrokeupOrCancelBF( bool bCancel, WCHAR * strName )
{
	CDnCommunityDlg * pCommunityDlg = static_cast< CDnCommunityDlg * >( m_pMainMenuDlg->GetMainDialog( CDnMainMenuDlg::COMMUNITY_DIALOG ) );
	if( pCommunityDlg )
	{	
		int strIdx = 0;
		if( bCancel )
			strIdx = 4341; // "%s님이 절친파기를 신청하셨습니다."
		else
			strIdx = 4343; // "%s님이 절친파기를 취소하셨습니다."

		wchar_t str[256] = {0,};
		swprintf_s( str, 256, GetEtUIXML().GetUIString(CEtUIXML::idCategory1, strIdx), strName );
		GetInterface().ShowCaptionDialog( CDnInterface::emCAPTION_TYPE::typeCaption3, str, textcolor::YELLOW ); 

		pCommunityDlg->BrokeupOrCancelBF( bCancel, strName );
	}
}

// 절친해제.
void CDnInterface::DestroyBF()
{
	CDnCommunityDlg * pCommunityDlg = static_cast< CDnCommunityDlg * >( m_pMainMenuDlg->GetMainDialog( CDnMainMenuDlg::COMMUNITY_DIALOG ) );
	if( pCommunityDlg )
	{
		wchar_t str[256] = {0,};
		swprintf_s( str, 256, GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4342), GetNameBF() ); // "%s님과 절친이 파기되었습니다."
		GetInterface().ShowCaptionDialog( CDnInterface::emCAPTION_TYPE::typeCaption3, str, textcolor::YELLOW ); 

		pCommunityDlg->DestroyBF();
	}
}



#endif //PRE_ADD_BESTFRIEND


bool CDnInterface::IsShowGateReady()
{
	if( m_pPlayerGaugeDlg )
		return m_pPlayerGaugeDlg->IsShowGateReady();
	return false;
}

#ifdef PRE_ADD_CASHREMOVE
// 캐시아이템제거.
void CDnInterface::OpenCashItemRemoveDialog( bool bShow, MIInventoryItem * pItem )
{
	if( m_pCashItemRemoveDlg )
	{
		m_pCashItemRemoveDlg->SetDlgType( CDnInvenCashItemRemoveDlg::DLGTYPE::DLGTYPE_REMOVE );
		m_pCashItemRemoveDlg->SetItemSlot( pItem );			
		m_pCashItemRemoveDlg->Show( bShow );		
	}
}


// 캐시아이템복구.
void CDnInterface::OpenCashItemRestoreDialog( bool bShow, MIInventoryItem * pItem )
{
	if( m_pCashItemRemoveDlg )
	{
		m_pCashItemRemoveDlg->SetDlgType( CDnInvenCashItemRemoveDlg::DLGTYPE::DLGTYPE_RESTORE );
		m_pCashItemRemoveDlg->SetItemSlot( pItem );		
		m_pCashItemRemoveDlg->Show( bShow );		
	}
}


// 제거 - 캐쉬아이템삭제 수락.
void CDnInterface::RemoveCashItemAccept()
{
	CDnInvenTabDlg * pInvenDlg = static_cast< CDnInvenTabDlg * >(GetMainMenuDialog( CDnMainMenuDlg::INVENTORY_DIALOG ));
	pInvenDlg->RemoveCashItemAccept();
}

// 제거 - 캐쉬아이템삭제 취소.
void CDnInterface::RemoveCashItemCancel()
{
	CDnInvenTabDlg * pInvenDlg = static_cast< CDnInvenTabDlg * >(GetMainMenuDialog( CDnMainMenuDlg::INVENTORY_DIALOG ));
	pInvenDlg->RemoveCashItemCancel();
}

#endif


#ifdef PRE_ADD_PVP_VILLAGE_ACCESS

bool CDnInterface::IsShowPVPVillageAccessDlg()
{
	if( m_pPVPRoomListDlg && m_pPVPLadderTabDlg && ( m_pPVPRoomListDlg->IsShow() || m_pPVPLadderTabDlg->IsShow() ) )
		return true;

	return false;
}

void CDnInterface::ShowPVPVillageAccessDlg(bool bShow)
{
	if( bShow == true && IsGateQuestionDlgShow() )
		return;

	if( m_pPVPRoomListDlg && m_pPVPLadderTabDlg && CDnActor::s_hLocalActor )
	{
		if(bShow)
			m_pMainMenuDlg->CloseMenuDialog();

		if( m_pPVPRoomListDlg->IsShow() != bShow )
		{
			CDnCommonTask *pTask = static_cast<CDnCommonTask *>(CTaskManager::GetInstance().GetTask( "CommonTask" ));
			if( pTask )			
				pTask->SendWindowState( bShow ? WINDOW_BLIND : WINDOW_NONE );
		}

		m_pPVPRoomListDlg->Show(bShow);
		m_pPVPLadderTabDlg->Show(bShow);
#ifdef PRE_ADD_COLOSSEUM_BEGINNER
		m_pPVPLadderTabDlg->UpdateAndSelectTabProperly();
#endif // #ifdef PRE_ADD_COLOSSEUM_BEGINNER

		CEtUIDialog* pDnNotifierDialog = GetInterface().GetMainMenuDialog( CDnMainMenuDlg::NOTIFIER_DIALOG );
		if( pDnNotifierDialog )
			pDnNotifierDialog->Show( !bShow );

#ifdef PRE_ADD_NOTIFY_ITEM_COMPOUND
		CEtUIDialog* pDnCompoundNotifierDialog = GetInterface().GetMainMenuDialog( CDnMainMenuDlg::ITEM_COMPOUND_NOTIFY_DIALOG );
		if( pDnCompoundNotifierDialog )
			pDnCompoundNotifierDialog->Show( !bShow );
#endif

#ifdef PRE_ADD_CASHSHOPBANNER_RENEWAL
		CEtUIDialog* pIngameBannerDlg = GetInterface().GetMainMenuDialog( CDnMainMenuDlg::CASHSHOP_INGAMEBANNER_DIALOG );
		if( pIngameBannerDlg )
		{
			if( GetInterface().GetInterfaceType() != CDnInterface::Village )
				bShow = true;
			pIngameBannerDlg->Show( !bShow );
		}
#endif // PRE_ADD_CASHSHOPBANNER_RENEWAL

	}
}
#endif



// 장착중인가?
bool CDnInterface::IsEquipped( CDnItem * pItem, bool bCash )
{
	if( m_pMainMenuDlg )
		return m_pMainMenuDlg->IsEquipped( pItem, bCash );

	return false;
}

#ifdef PRE_MOD_BROWSER
void CDnInterface::DisableAllDlgs(bool bEnable, const std::wstring& text, float delayTime)
{
	if (bEnable)
	{
		if (delayTime <= 0.f)
		{
			CloseMessageBox();
			MessageBox(text.c_str(), MB_OK, MESSAGEBOX_DISABLE_ALL, this, true, false, true, false);
		}
		else
		{
			m_DisableDlgDelayTime = delayTime;
			m_DisableDlgStringCache = text;
		}
	}
	else
	{

		CloseMessageBox();
		m_DisableDlgDelayTime = 0.f;
		m_DisableDlgStringCache.clear();
	}
}

void CDnInterface::ProcessDisableDlgs(float fElapsedTime)
{
	if (m_DisableDlgDelayTime > 0.f)
	{
		m_DisableDlgDelayTime -= fElapsedTime;
		if (m_DisableDlgDelayTime <= 0.f)
		{
			DisableAllDlgs(true, m_DisableDlgStringCache);
			m_DisableDlgStringCache.clear();
			m_DisableDlgDelayTime = 0.f;
		}
	}
}
#endif // PRE_MOD_BROWSER

#ifdef PRE_ADD_DOORS
void CDnInterface::OnSetDoorsCancelAuthMobilMsgBox(const SCDoorsCancelAuth& data)
{
	m_pDoorsMobileAuthDlg->ShowDoorsMobileMsgBoxDlg(false, eDMMsgBox_CancelAuth);

	if (data.nRetCode != ERROR_NONE)
	{
		ServerMessageBox(data.nRetCode);
	}
	else
	{
		CDnBridgeTask *pTask = static_cast<CDnBridgeTask *>(CTaskManager::GetInstance().GetTask( "BridgeTask" ));
		if (pTask)
			pTask->SetDoorsMobileAuthUser(false);
		GetInterface().MessageBox(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 6483)); // UISTRING : 모바일 인증이 해제되었습니다.
	}
}

void CDnInterface::OnSetDoorsAuthMobileMsgBox(const SCDoorsGetAuthKey& data)
{
	if (data.nRetCode != ERROR_NONE)
	{
		ServerMessageBox(data.nRetCode);
		if (m_pDoorsMobileAuthDlg)
			m_pDoorsMobileAuthDlg->EnableAuthButton(true);
	}
	else
	{
		if (m_pDoorsMobileAuthDlg)
		{
			m_pDoorsMobileAuthDlg->OnSetDoorsAuthMobileMsgBox(data.wszAuthKey);
			m_pDoorsMobileAuthDlg->ShowDoorsMobileMsgBoxDlg(true, eDMMsgBox_Auth);
		}
		else
		{
			GetInterface().MessageBox(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 6486)); // UISTRING : 모바일 인증 기능을 사용할 수 없습니다
			if (m_pDoorsMobileAuthDlg)
				m_pDoorsMobileAuthDlg->EnableAuthButton(true);
		}
	}
}

void CDnInterface::OnSetDoorsAuthFlag(const SCDoorsGetAuthFlag& data)
{
	if (data.bFlag)
		m_pDoorsMobileAuthDlg->ShowDoorsMobileMsgBoxDlg(true, eDMMsgBox_CancelAuth);
	else
		SendDoorsMobileReqAuthKey();
}
#endif


#ifdef PRE_ADD_COMBINEDSHOP_PERIOD
// 상점Dlg.
bool CDnInterface::IsOpenStore()
{
	if( m_pMainMenuDlg )
		return m_pMainMenuDlg->IsOpenStore();
	return false;
}
#endif // PRE_ADD_COMBINEDSHOP_PERIOD

#if defined(PRE_ADD_SERVER_WAREHOUSE)
bool CDnInterface::IsOpenStorage()
{
	if( m_pMainMenuDlg )
		return m_pMainMenuDlg->IsOpenStorage();
	return false;
}
#endif //#if defined(PRE_ADD_SERVER_WAREHOUSE)

#ifdef PRE_ADD_QUEST_BACK_DIALOG_BUTTON
void CDnInterface::ShowQuestReturnButton(bool bIsShow)
{
	(bIsShow)? OpenNpcTalkReturnDlg() : CloseNpcTalkReturnDlg();
}
#endif


#ifdef PRE_MOD_PVPOBSERVER	
bool CDnInterface::IsEventRoom()
{
	if( GetInterface().GetGameRoomDlg())
		return GetInterface().GetGameRoomDlg()->IsEventRoom();
	
	return false;
}

void CDnInterface::ResetPvpObserverJoinFlag()
{
	if( m_pPVPRoomListDlg )
		m_pPVPRoomListDlg->ResetObserverJoinFlag();
}

#endif // PRE_MOD_PVPOBSERVER


#if defined(PRE_ADD_TOTAL_LEVEL_SKILL)
void CDnInterface::UpdateTotalLevelSkill()
{
	CDnSkillTreeDlg* pSkillTreeDlg = static_cast<CDnSkillTreeDlg*>( GetInterface().GetMainMenuDialog( CDnMainMenuDlg::SKILL_DIALOG ) );
	if (pSkillTreeDlg)
		pSkillTreeDlg->UpdateTotalLevelSkillTreeContent();
}
void CDnInterface::AddTotalLevelSkill(int nSlotIndex, DnSkillHandle hSkill)
{
	CDnSkillTreeDlg* pSkillTreeDlg = static_cast<CDnSkillTreeDlg*>( GetInterface().GetMainMenuDialog( CDnMainMenuDlg::SKILL_DIALOG ) );

	if (pSkillTreeDlg && hSkill)
		pSkillTreeDlg->AddTotalLevelSkill(nSlotIndex, hSkill);
}
void CDnInterface::RemoveTotalLevelSkill(int nSlotIndex)
{
	CDnSkillTreeDlg* pSkillTreeDlg = static_cast<CDnSkillTreeDlg*>( GetInterface().GetMainMenuDialog( CDnMainMenuDlg::SKILL_DIALOG ) );

	if (pSkillTreeDlg)
		pSkillTreeDlg->RemoveTotalLevelSkill(nSlotIndex);
}

void CDnInterface::ActivateTotalLevelSkillSlot(int nSlotIndex, bool bActivate)
{
	CDnSkillTreeDlg* pSkillTreeDlg = static_cast<CDnSkillTreeDlg*>( GetInterface().GetMainMenuDialog( CDnMainMenuDlg::SKILL_DIALOG ) );

	if (pSkillTreeDlg)
		pSkillTreeDlg->ActivateTotalLevelSkillSlot(nSlotIndex, bActivate);
}

void CDnInterface::ActivateTotalLevelSkillCashSlot(int nSlotIndex, bool bActivate, __time64_t tExpireDate)
{
	CDnSkillTreeDlg* pSkillTreeDlg = static_cast<CDnSkillTreeDlg*>( GetInterface().GetMainMenuDialog( CDnMainMenuDlg::SKILL_DIALOG ) );

	if (pSkillTreeDlg)
		pSkillTreeDlg->ActivateTotalLevelSkillCashSlot(nSlotIndex, bActivate, tExpireDate);
}
#endif // PRE_ADD_TOTAL_LEVEL_SKILL

bool CDnInterface::IsSkipInteraction()
{
	//상점
	if (IsOpenStore() == true) return true;

	//창고
	if( m_pMainMenuDlg->IsOpenDialog(CDnMainMenuDlg::PLAYER_GUILD_STORAGE_DIALOG) )	return true;

	//분해
	if( m_pMainMenuDlg->IsOpenDialog(CDnMainMenuDlg::ITEM_DISJOINT_DIALOG) )	return true;

	//우편
	if( m_pMainMenuDlg->IsOpenDialog(CDnMainMenuDlg::MAIL_DIALOG) )	return true;

	//거래소
	if( m_pMainMenuDlg->IsOpenDialog(CDnMainMenuDlg::MARKET_DIALOG) )	return true;

	//강화
	if( m_pMainMenuDlg->IsOpenDialog(CDnMainMenuDlg::ITEM_UPGRADE_DIALOG) )	return true;

	//코스튬 합성기
	if( m_pMainMenuDlg->IsOpenDialog(CDnMainMenuDlg::ITEM_COSTUMEMIX_DIALOG) )	return true;
	if( m_pMainMenuDlg->IsOpenDialog(CDnMainMenuDlg::ITEM_COSTUME_DMIX_DIALOG) )	return true;
#if defined(PRE_ADD_COSRANDMIX)
	if( m_pMainMenuDlg->IsOpenDialog(CDnMainMenuDlg::ITEM_COSTUME_RANDOMMIX_DIALOG) )	return true;
#endif // PRE_ADD_COSRANDMIX

	//청약철회
	//담기창
	// 이 녀석들은 아이템 슬롯 타입/캐쉬아이템 여부로 걸러 질듯함.

	return false;

}


CEtUIDialog * CDnInterface::GetCommonDialog( int ID )
{
	CEtUIDialog * pDlg = NULL;
	for( int i = 0; i < ( int )m_pVecCommonDialogList.size(); i++ )
	{
		pDlg = m_pVecCommonDialogList[i];
		if( pDlg && pDlg->GetDialogID() == ID )
			return pDlg;
	}

	return NULL;
}

// CommonDialogList 에서 제거.
bool CDnInterface::RemoveCommonDialog( int ID )
{
	CEtUIDialog * pDlg = NULL;
	std::vector<CEtUIDialog*>::iterator it = m_pVecCommonDialogList.begin();
	for( ; it != m_pVecCommonDialogList.end(); ++it )
	{
		pDlg = (*it);
		if( pDlg && pDlg->GetDialogID() == ID )
		{
			m_pVecCommonDialogList.erase( it );
			return true;
		}
	}
	
	return false;
}

#ifdef PRE_ADD_PVP_RANKING

// 화면 우측 알리미창들 - 미션,퀘스트,제작 등..
void CDnInterface::OpenRightAlramDlg( bool bShow )
{
	// 오른쪽의 알림창들 - 제작,퀘스트,미션 등..
	if( bShow )
		m_pMainMenuDlg->CloseMenuDialog();

	CEtUIDialog* pDnNotifierDialog = GetInterface().GetMainMenuDialog( CDnMainMenuDlg::NOTIFIER_DIALOG );
	if( pDnNotifierDialog )
		pDnNotifierDialog->Show( !bShow );

#ifdef PRE_ADD_NOTIFY_ITEM_COMPOUND
	CEtUIDialog* pDnCompoundNotifierDialog = GetInterface().GetMainMenuDialog( CDnMainMenuDlg::ITEM_COMPOUND_NOTIFY_DIALOG );
	if( pDnCompoundNotifierDialog )
		pDnCompoundNotifierDialog->Show( !bShow );
#endif

#ifdef PRE_ADD_CASHSHOPBANNER_RENEWAL
	CEtUIDialog * pIngameBannerDlg = GetInterface().GetMainMenuDialog( CDnMainMenuDlg::CASHSHOP_INGAMEBANNER_DIALOG );
	if( pIngameBannerDlg )
	{
		if( GetInterface().GetInterfaceType() != CDnInterface::Village )
			bShow = true;
		pIngameBannerDlg->Show( !bShow );	
	}
#endif // PRE_ADD_CASHSHOPBANNER_RENEWAL

}

bool CDnInterface::IsShowPvPRankDlg()
{
	if( m_pPvPRankDlg )
		return m_pPvPRankDlg->IsShow();
	return false;
}

void CDnInterface::ClosePvPRankDlg()
{
	if( m_pPvPRankDlg )
		m_pPvPRankDlg->Show( false );

}

void CDnInterface::OpenPvPRankDlg()
{	
	if( m_pPvPRankDlg &&
		( GetInterface().GetInterfaceType() == CDnInterface::Village || 
		  GetInterface().GetInterfaceType() == PVPVillage ) 
	)
	{	
		m_pMainMenuDlg->ToggleShowDialog( CDnMainMenuDlg::PVP_RANK_DIALOG );
		m_pPvPRankDlg->Show( !m_pPvPRankDlg->IsShow() );
	}	
}

void CDnInterface::SetInfoMyRankColosseum( TPvPRankingDetail * pInfo )
{
	/*if( m_pMainMenuDlg )
	{
		CEtUIDialog * pDlg = m_pMainMenuDlg->GetMainDialog( CDnMainMenuDlg::PVP_RANK_DIALOG );
		if( pDlg )
			static_cast<CDnPvPRankDlg *>( pDlg )->SetInfoMyRankColosseum( pInfo );
	}*/
	if( m_pPvPRankDlg )
		m_pPvPRankDlg->SetInfoMyRankColosseum( pInfo );
}


void CDnInterface::SetInfoMyRankLadder( TPvPLadderRankingDetail * pInfo )
{
	/*if( m_pMainMenuDlg )
	{
		CEtUIDialog * pDlg = m_pMainMenuDlg->GetMainDialog( CDnMainMenuDlg::PVP_RANK_DIALOG );
		if( pDlg )
			static_cast<CDnPvPRankDlg *>( pDlg )->SetInfoMyRankLadder( pInfo );
	}*/
	if( m_pPvPRankDlg )
		m_pPvPRankDlg->SetInfoMyRankLadder( pInfo );
}


void CDnInterface::SetInfoColosseum( TPvPRankingDetail * pInfo )
{
	/*if( m_pMainMenuDlg )
	{
		CEtUIDialog * pDlg = m_pMainMenuDlg->GetMainDialog( CDnMainMenuDlg::PVP_RANK_DIALOG );
		if( pDlg )
			static_cast<CDnPvPRankDlg *>( pDlg )->SetInfoColosseum( pInfo );
	}*/
	if( m_pPvPRankDlg )
		m_pPvPRankDlg->SetInfoColosseum( pInfo );
}

void CDnInterface::SetInfoLadder( TPvPLadderRankingDetail * pInfo )
{
	/*if( m_pMainMenuDlg )
	{
		CEtUIDialog * pDlg = m_pMainMenuDlg->GetMainDialog( CDnMainMenuDlg::PVP_RANK_DIALOG );
		if( pDlg )
			static_cast<CDnPvPRankDlg *>( pDlg )->SetInfoLadder( pInfo );
	}*/
	if( m_pPvPRankDlg )
		m_pPvPRankDlg->SetInfoLadder( pInfo );
}

void CDnInterface::SetListColosseum( SCPvPRankList * pInfo )
{
	/*if( m_pMainMenuDlg )
	{
		CEtUIDialog * pDlg = m_pMainMenuDlg->GetMainDialog( CDnMainMenuDlg::PVP_RANK_DIALOG );
		if( pDlg )
			static_cast<CDnPvPRankDlg *>( pDlg )->SetListColosseum( pInfo );
	}*/
	if( m_pPvPRankDlg )
		m_pPvPRankDlg->SetListColosseum( pInfo );
}

void CDnInterface::SetListLadder( SCPvPLadderRankList * pInfo )
{
	/*if( m_pMainMenuDlg )
	{
		CEtUIDialog * pDlg = m_pMainMenuDlg->GetMainDialog( CDnMainMenuDlg::PVP_RANK_DIALOG );
		if( pDlg )
			static_cast<CDnPvPRankDlg *>( pDlg )->SetListLadder( pInfo );
	}*/
	if( m_pPvPRankDlg )
		m_pPvPRankDlg->SetListLadder( pInfo );
}

#endif // PRE_ADD_PVP_RANKING


// 콜로세움등급아이콘.
void CDnInterface::SetPvPClassIcon( CEtUITextureControl * pControl, BYTE pvpLevel )
{
	if( pControl == NULL )
		return;

	//아이콘
	int iIconW,iIconH;
	int iU,iV;
	iIconW = GeticonWidth();
	iIconH = GeticonHeight();
	if( ConvertPVPGradeToUV( pvpLevel, iU, iV ))
	{
		pControl->SetTexture( GetPVPIconTex(), iU, iV, iIconW, iIconH );		

		// 등급명.		
		DNTableFileFormat*  pTable = GetTableDB().GetTable( CDnTableDB::TPVPRANK );
		if( pTable == NULL )
			return;

		DNTableCell * pCell = NULL;
		int size = pTable->GetItemCount();
		for( int i=0; i<size; ++i )
		{
			int nID = pTable->GetItemID( i );
			pCell = pTable->GetFieldFromLablePtr( nID, "PvPRank" );
			if( pCell && pCell->GetInteger() == pvpLevel )
			{
				pCell = pTable->GetFieldFromLablePtr( nID, "PvPRankUIString" );
				if( pCell )
					pControl->SetTooltipText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, pCell->GetInteger() ) );
			}
		}

	}
}

#ifdef PRE_ADD_PVP_TOURNAMENT
void CDnInterface::UpdatePVPTournamentGameMatchUserList()
{
	if (m_pPVPTournamentGMatchListDlg)
		m_pPVPTournamentGMatchListDlg->Update();
}
#endif


#ifdef PRE_ADD_NEWCOMEBACK
void CDnInterface::OpenComebackRewardDlg( bool bShow )
{
	if( m_pComeBackRewardDlg == NULL )
	{
		m_pComeBackRewardDlg = new CDnComeBackRewardDlg( UI_TYPE_MODAL );		
		m_pComeBackRewardDlg->Initialize( false );
		AddCommonDialogList( m_pComeBackRewardDlg );
	}
	m_pComeBackRewardDlg->Show( bShow );	
}

void CDnInterface::SelectedCharIndex( int nIndex )
{
	if( m_pComeBackRewardDlg )
		m_pComeBackRewardDlg->SetSelectedChar( nIndex );
}


// 귀환자 최초접속시 뜨는 환영메세지 Dlg.
void CDnInterface::OpenComebackMsgDlg( bool bShow )
{
	if( m_pComebackMsgDlg == NULL )
	{
		m_pComebackMsgDlg = new CDnComeBackMsgDlg( UI_TYPE_MODAL );
		m_pComebackMsgDlg->Initialize( false );
		AddCommonDialogList( m_pComeBackRewardDlg );
	}
	m_pComebackMsgDlg->Show( bShow );
}

#endif // PRE_ADD_NEWCOMEBACK

#if defined(PRE_ADD_68286)
void CDnInterface::SetDisableChatTabMsgproc(bool isDisable)
{
	if (m_pChatDlg)
		m_pChatDlg->SetPassMessageToChild( isDisable );
}

bool CDnInterface::IsAlarmIconClick(POINT& MousePoint)
{
	bool isAlarmIconClick = false;

	if (m_pChatDlg)
		isAlarmIconClick = m_pMainBar->IsAlarmIconClick(MousePoint);

	return isAlarmIconClick;
}
#endif // PRE_ADD_68286


#ifdef PRE_ADD_ACTIVEMISSION
void CDnInterface::ShowActiveAlarmDlg( bool bShow, std::wstring & str, float fadeTime )
{
	if( m_pActiveMissionDlg )
	{
		if( bShow )
			m_pActiveMissionDlg->SetAlarm( str, fadeTime );
		m_pActiveMissionDlg->Show( bShow );
	}
}

void CDnInterface::RefreshActiveMissionEventMark( int acMissionID, std::wstring & str, bool bShow, bool bNew)
{
	if (m_pChannelMoveDlg)
		m_pChannelMoveDlg->SetActiveMissionEventMark( acMissionID, str, bShow, bNew );
	if (m_pDungeonInfoDlg)
		m_pDungeonInfoDlg->SetActiveMissionEventMark( acMissionID, str, bShow, bNew );
}

void CDnInterface::ShowActiveMissionEventMarkTooltip( std::wstring & str, bool bShow, float fMouseX, float fMouseY )
{
	CDnCommonTask *pTask = static_cast<CDnCommonTask *>(CTaskManager::GetInstance().GetTask( "CommonTask" ));
	if( bShow && m_pCustomTooltipDlg && !m_pCustomTooltipDlg->IsShow() && pTask )	
	{
		m_pCustomTooltipDlg->SetPosition( fMouseX, fMouseY + ( CDnMouseCursor::GetInstance().GetCursorHeight() * 3/4 )  );
		m_pCustomTooltipDlg->SetActiveMissionText( str );
		m_pCustomTooltipDlg->Show( true );
	}

}

void CDnInterface::CloseActiveMissionEventMarkTooltip()
{
	if( m_pCustomTooltipDlg )
	{
		m_pCustomTooltipDlg->GetTooltipTextBox()->ClearText();
		m_pCustomTooltipDlg->Show( false );
	}
}

#endif // PRE_ADD_ACTIVEMISSION=======

#ifdef PRE_ADD_68286
void CDnInterface::OpenCashShopRecvGiftWindow()
{
	// Note :
	//
	if( m_pMainMenuDlg )
	{
		if( m_pMainMenuDlg->IsOpenDialog(CDnMainMenuDlg::PLAYER_GUILD_STORAGE_DIALOG) )	return;
		if( m_pMainMenuDlg->IsOpenDialog(CDnMainMenuDlg::STORE_DIALOG) )			return;
		if( m_pMainMenuDlg->IsOpenDialog(CDnMainMenuDlg::SKILL_STORE_DIALOG) )		return;
		if( m_pMainMenuDlg->IsOpenDialog(CDnMainMenuDlg::MARKET_DIALOG) )			return;
		if( m_pMainMenuDlg->IsOpenDialog(CDnMainMenuDlg::MAIL_DIALOG) )				return;
		if( m_pMainMenuDlg->IsOpenDialog(CDnMainMenuDlg::ITEM_DISJOINT_DIALOG) )	return;
		if( m_pMainMenuDlg->IsOpenDialog(CDnMainMenuDlg::PLATE_MAIN_DIALOG) )return;
		if( m_pMainMenuDlg->IsOpenDialog(CDnMainMenuDlg::ITEM_UPGRADE_DIALOG) )		return;
		if( m_pMainMenuDlg->IsOpenDialog(CDnMainMenuDlg::ITEM_COMPOUND_DIALOG) )	return;

		if( m_pMainMenuDlg->IsWithDrawGlyphOpen() )	return;

#ifdef PRE_ADD_DONATION
		if( m_pMainMenuDlg->IsOpenDialog(CDnMainMenuDlg::CONTRIBUTION_DIALOG) )		return;
#endif

		CDnSkillTreeDlg* pSkillTreeDlg = static_cast<CDnSkillTreeDlg*>( GetInterface().GetMainMenuDialog( CDnMainMenuDlg::SKILL_DIALOG ) );
		if( pSkillTreeDlg && pSkillTreeDlg->IsShow() && pSkillTreeDlg->IsUnlockByMoneyMode() )
			return;
	}
	if( m_pStageClearReportDlg && m_pStageClearReportDlg->IsShow() )	return;
	if( m_pStageClearDlg && m_pStageClearDlg->IsShow() )			return;

#ifdef PRE_ADD_GACHA_JAPAN
	if( m_pGachaDlg && m_pGachaDlg->IsShow() )			return;
#endif

	// NPC관련 길드창이 열려있어도 마찬가지
	if( IsShowGuildMgrBox() ) return;

	// DWC Team 생성창
#ifdef PRE_ADD_DWC
	if( IsOpenDwcTeamCreateDlg() ) return;
#endif // PRE_ADD_DWC

	// 그 외 블라인드된 상태로 열리는 창들..
#ifdef PRE_ADD_GACHA_JAPAN
	if( m_pGachaDlg && m_pGachaDlg->IsShow() )	return;
#endif // PRE_ADD_GACHA_JAPAN

#ifdef PRE_FIX_49403
	// #49403 특수 순서에서, 돈을 소모했음에도 불구하고, 비행선을 탈 수 없다.
	// 현재 거래요청or수락 상태 이면 거래취소패킷 전송.
	CDnTradePrivateMarket & rPrivateMarket = GetTradeTask().GetTradePrivateMarket();	

	//#69850
	//거래중 우편함 알림 아이콘으로 우편함을 열때, 거래 취소를 보내고, 우편함을 열면, 인벤창이 닫힘, 우편함도 비정상 동작.
	//거래중일 경우 우편함 열리지 않도록함.
	if (rPrivateMarket.GetCurTradeUserID() != 0)
		return;
#endif

	GetCashShopTask().RequestCashShopRecvGiftBasicInfoByShortCut();
}
#endif // PRE_ADD_68286

#ifdef PRE_WORLDCOMBINE_PVP
void CDnInterface::SetWorldPVPStartButtonInfo( bool bShow, UINT uiPvPIndex )
{
	if( m_pWorldPVPRoomStartDlg )
	{
		m_pWorldPVPRoomStartDlg->SetPVPIndex( uiPvPIndex );
		m_pWorldPVPRoomStartDlg->Show( bShow );
	}
}

void CDnInterface::ChangePVPScoreTeam( int nSession, int nTeam )
{
	if( m_pPVPGameResultDlg ) 
		m_pPVPGameResultDlg->ChangePVPScoreTeam( nSession, nTeam ); 
}
#endif // PRE_WORLDCOMBINE_PVP

#ifdef PRE_ADD_ALTHEIA_NEWCAPTION_TRIGGER
void CDnInterface::OpenImageBlind()
{
	if( !m_ImageBlindDlg ) return;

	if( m_pNpcDlg && m_pNpcDlg->IsShow() )
	{
		m_bIsOpenNpcDlg = true;
		m_pNpcDlg->Show( false );
		return;
	}

	CDnLocalPlayerActor::ShowCrosshair(false);
	//CDnLocalPlayerActor::LockInput(true);
	ShowMiniMap( false );
	CEtUIDialogBase::CloseAllDialog();
	m_ImageBlindDlg->Show(true);
}

void CDnInterface::CloseImageBlind( bool bShowBaseDialog )
{
#ifdef PRE_ADD_QUEST_BACK_DIALOG_BUTTON
	DnCameraHandle		 hCurrentCam		= CDnCamera::GetActiveCamera();
	CDnNpcTalkReturnDlg* pNpcTalkReturnDlg  = GetInterface().GetNpcTalkReturnDlg();
	if( pNpcTalkReturnDlg == NULL || hCurrentCam == NULL ) return;

	if( hCurrentCam->GetCameraType() == CDnCamera::CameraTypeEnum::NpcTalkCamera && pNpcTalkReturnDlg->GetButtonToggle() )
	{
		return;
	}
#endif

	if( !m_ImageBlindDlg ) return;

	if( bShowBaseDialog ) 
	{
		if( m_bIsOpenNpcDlg )
		{
			m_bIsOpenNpcDlg = false;
			if( m_pNpcDlg ) m_pNpcDlg->Show( true );
			return;
		}
		OpenBaseDialog();
	}

	m_ImageBlindDlg->CloseBlind();
	//CDnLocalPlayerActor::LockInput(false);
	CDnLocalPlayerActor::ShowCrosshair(true);
}
#endif // PRE_ADD_ALTHEIA_NEWCAPTION_TRIGGER

#ifdef PRE_ADD_CASHSHOP_ACTOZ
void CDnInterface::ShowConfirmRefund( CDnItem * pItem )
{
	if( m_pCashShopDlg )
	{
		m_pCashShopDlg->ShowConfirmRefund( pItem );
	}
}
#endif // PRE_ADD_CASHSHOP_ACTOZ

void CDnInterface::ShowDungeonSynchroDialog( bool bShow, bool bSynchro )
{
	if( m_pDungeonSynchroDlg )
	{
		m_pDungeonSynchroDlg->SetSynchro( bSynchro );
		m_pDungeonSynchroDlg->Show( bShow );
	}
}


#ifdef PRE_ADD_STAMPSYSTEM
void CDnInterface::NPCSearch( int mapID, int npcID )
{
	if( m_pMainBar )
		m_pMainBar->NPCSearch( mapID, npcID );
}

void CDnInterface::ChangeWorldMap()
{		
	// 퀘스트NPC화살표.
	if( m_pMainBar )
		m_pMainBar->FindComplete();
}

// 이벤트창.
void CDnInterface::ShowStampDlg()
{	
	// 만렙인 경우에만 스탬프창을 연다.	
	if( CDnActor::s_hLocalActor )
	{
		int nLimitLevel = (int)CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::PlayerLevelLimit );
		if( CDnActor::s_hLocalActor->GetLevel() < nLimitLevel )
			return;
	}

	if( m_pMainMenuDlg )
		m_pMainMenuDlg->ShowIntegrateEventDialog( true );
}

// 도전과제 하나 완료.
void CDnInterface::AddStamp( bool bOne, bool bForce )
{
	if( m_pStampEventAlarmDlg )
		m_pStampEventAlarmDlg->SetAlarmState( bOne, bForce );
}

void CDnInterface::ShowStampAlarmDlg( bool bShow )
{
	if( m_pStampEventAlarmDlg )
		m_pStampEventAlarmDlg->Show( bShow );	
}

CDnStampEventDlg * CDnInterface::GetStampEventDlg()
{
	if( m_pMainMenuDlg )
		return m_pMainMenuDlg->GetStampEventDlg();

	return NULL;
}

// 퀘스트창 일일퀘스트탭 열기.
void CDnInterface::ShowDailyQuestTab()
{
	if( m_pMainMenuDlg )
		m_pMainMenuDlg->ShowDailyQuestTab();		
}

#endif // PRE_ADD_STAMPSYSTEM

#if defined( PRE_ALTEIAWORLD_EXPLORE )
void CDnInterface::Initialize_AlteaBoard()
{
	CDnGameTask * pGameTask = (CDnGameTask*)(CTaskManager::GetInstance().GetTask("GameTask"));
	if( NULL == pGameTask )
		return;

	if( false == GetAlteaTask().IsAlteaWorldMap( pGameTask->GetGameMapIndex() ) )
		return;

	m_pAlteaBoardDlg = new CDnAlteaBoardDlg( UI_TYPE_MODAL );
	m_pAlteaBoardDlg->Initialize( false );
	//m_pAlteaBoardDlg->Show( true );
}

void CDnInterface::Show_AlteaBoard( bool bShow )
{
	if( NULL == m_pAlteaBoardDlg )
		return;

	m_pAlteaBoardDlg->Show( bShow );
}

void CDnInterface::SetAlteaDiceIcon()
{
	m_pMainBar->SetAlteaDiceIcon();
}

void CDnInterface::ShowAlteaClearDlg( bool bClear )
{
	if( NULL == m_pAlteaClearDlg )
		return;

	m_pAlteaClearDlg->Show_ClearDlg( bClear );
	m_pAlteaClearDlg->Show( true );
}
#endif	// #if defined( PRE_ALTEIAWORLD_EXPLORE )

#ifdef PRE_ADD_GAMEQUIT_REWARD

// 아이템즉시지급 Dlg.
bool CDnInterface::ShowGameQuitRewardDlg( bool bShow, int type )
{
	if( m_pGameQuitRewardDlg )
	{
		m_pGameQuitRewardDlg->SetRewardType( type );	
		m_pGameQuitRewardDlg->Show( bShow );

		if( bShow == false )
		{
			return m_pGameQuitRewardDlg->ShowNewbieNextTimeReward();
		}
	}

	return false;
}

void CDnInterface::RecvReward()
{
	if( m_pGameQuitRewardDlg )
		m_pGameQuitRewardDlg->RecvReward();
}

// 나중에 받을수 있는 아이템 Dlg.
void CDnInterface::ShowGameQuitNextTimeRewardDlg( bool bShow, int type, bool bCharSelect, int nLevel, int nClass )
{
	if( m_pGameQuitNextTimeRewardDlg )
	{
		// 캐릭터선택창에서 호출.
		if( bCharSelect )
			m_pGameQuitNextTimeRewardDlg->SetNextTimeRewardType( type, bCharSelect, nLevel, nClass );

		// 게임안에서 호출.
		else
			m_pGameQuitNextTimeRewardDlg->SetNextTimeRewardType( type );

		m_pGameQuitNextTimeRewardDlg->Show( bShow );
	}
}

// 신규계정 게임종료보상 캐릭터선택 Dlg.	
void CDnInterface::ShowGameQuitRewardComboDlg( bool bShow )
{
	if( m_pGameQuitNextTimeRewardComboDlg == NULL )
	{
		m_pGameQuitNextTimeRewardComboDlg = new CDnGameQuitNextTimeRewardComboDlg( UI_TYPE_MODAL );		
		m_pGameQuitNextTimeRewardComboDlg->Initialize( false );
		AddCommonDialogList( m_pGameQuitNextTimeRewardComboDlg );
	}
	m_pGameQuitNextTimeRewardComboDlg->Show( bShow );	
}

// 캐릭터목록초기화.
void CDnInterface::ResetCharacterListGameQuitRewardComboDlg()
{
	if( m_pGameQuitNextTimeRewardComboDlg )
		m_pGameQuitNextTimeRewardComboDlg->ResetComboBox();
}

// 캐릭터선택차에서 선택한 캐릭터 Index.
void CDnInterface::SelectedGameQuitRewardCharIndex( int charIndex )
{
	if( m_pGameQuitNextTimeRewardComboDlg )
		m_pGameQuitNextTimeRewardComboDlg->SelectedGameQuitRewardCharIndex( charIndex );
}

#endif // PRE_ADD_GAMEQUIT_REWARD

#ifdef PRE_ADD_FADE_TRIGGER
void CDnInterface::ShowFadeScreenCaption( int nUIStringIndex, int nDelay )
{
	if( m_pFadeCaptionDlg )
	{
		SUICoord capCoord;
		m_pFadeCaptionDlg->GetDlgCoord( capCoord );
		capCoord.fY = 0.5f - ( capCoord.fHeight / 2.0f );
		m_pFadeCaptionDlg->SetDlgCoord( capCoord );

		m_pFadeCaptionDlg->SetCaption( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nUIStringIndex ), nDelay / 1000.0f );
	}
}
#endif // PRE_ADD_FADE_TRIGGER

#ifdef PRE_ADD_PVPRANK_INFORM
void CDnInterface::ShowPVPRankInformDlg(bool bShow)
{
	if (m_pPvPRankInformDlg == NULL)
		return;

	m_pPvPRankInformDlg->Show(bShow);
}
#endif

#ifdef PRE_ADD_CASHSHOPBANNER_RENEWAL
void CDnInterface::ShowIngameBanner( bool bShow )
{
	if( m_pMainMenuDlg )
	{
		m_pMainMenuDlg->ShowIngameBanner( bShow );
	}
}
#endif // PRE_ADD_CASHSHOPBANNER_RENEWAL

#ifdef PRE_ADD_MAINQUEST_UI
void CDnInterface::ShowMainQuestDlg(CEtUICallback *pCallback/* = NULL*/)
{	
	if(pCallback) m_pMainQuestDlg->SetCallback(pCallback);
	m_pMainQuestDlg->Start();
}

void CDnInterface::CloseMainQuestDlg()
{
	/*
	두가지 상황이 있다.
	1. 종료버튼 클릭 
	-> 메인퀘스트UI 를 닫는다. 끝

	2. 돌아가기 버튼 
	-> 2.1 메인퀘스트UI를 닫는다.
	-> 2.2 NpcDlg를 다시 연다.
	*/
	m_pMainQuestDlg->End();
}

bool CDnInterface::IsOpenMainQuestDlg() 
{ 
	if(m_pMainQuestDlg) 
		return m_pMainQuestDlg->IsOpenMainQuestDlg(); 

	return false;
}
#endif // PRE_ADD_MAINQUEST_UI


//void CDnInterface::SetBubble(int Count,float fExpireTime, float fTotalTime)
//{
	//if(m_pMainBar)
//		m_pMainBar->SetBubble(Count,fExpireTime,fTotalTime);
//} 큼폭�쓱�


void CDnInterface::SetLatency(int Latency)
{
	if(m_pDungeonInfoDlg)
		m_pDungeonInfoDlg->SetLatency(Latency);
}

void CDnInterface::ShowRebirthDialog()
{
	if (CDnRebirthSystem::GetInstancePtr())
	{
		OutputDebug("CDnRebirthSystem::OpenRebirthWindow()");
		CDnRebirthSystem::GetInstance().OpenRebirthWindow();
	}else{
		OutputDebug("CDnRebirthSystem::CreateInstance()");
		CDnRebirthSystem::CreateInstance();
		this->ShowRebirthDialog();
	}
}

void CDnInterface::ShowJobChangeUI()
{
	if (m_pJobChangeDlg)
	{
		CloseAllMainMenuDialog();
		//m_pJobChangeDlg->SetCallback(m_pJobChangeDlg->GetCallBack());
		m_pJobChangeDlg->InitClass(CDnActor::s_hLocalActor->OnGetJobClassID()); // GetActorHandle()->GetJobClassID());
		m_pJobChangeDlg->Show(true);
	}
}

void CDnInterface::AddNewDisjointResult(SCItemDisjointResNew* pPacket) {
	if (m_pItemDisjointResultDlg && pPacket)
	{
		m_pItemDisjointResultDlg->OnRecvDropList(pPacket);
		m_pItemDisjointResultDlg->Show(true);
	}
}


void CDnInterface::ShowInstantSpecializationBtn(bool bShow)
{
	if (m_pInstantSpecializationDlg)
		m_pInstantSpecializationDlg->Show(bShow);
}