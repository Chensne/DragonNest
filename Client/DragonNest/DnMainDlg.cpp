#include "StdAfx.h"
#include "DnMainDlg.h"
#include "DnMinimap.h"
#include "DnCamera.h"
#include "TaskManager.h"
#include "DnVillageTask.h"
#include "DnSkillTask.h"
#include "DnPartyTask.h"
#include "DnQuickSlotButton.h"
#include "DnQuickSlotDlg.h"
#include "DnPassiveSkillSlotDlg.h"
#include "DnItemTask.h"
#include "DnExpDlg.h"
#include "DnInterfaceString.h"
#include "DnTradeTask.h"
#include "DnMailDlg.h"
#include "DnMainMenuDlg.h"
#include "DnInterface.h"
#include "DnRadioMsgQuickSlotDlg.h"
#include "DnGestureQuickSlotDlg.h"
#include "DnPlayerActor.h"
#include "DnPopupFatigueDlg.h"
#include "DnTableDB.h"
#include "InputWrapper.h"
#include "DnLocalPlayerActor.h"
#include "DnCashShopTask.h"
#include "DnFadeInOutDlg.h"
#include "DnActor.h"
#include "DnLocalPlayerActor.h"
#include "DnLifeSkillPlantTask.h"
#include "DnCommonUtil.h"
#include "GameOption.h"
#ifdef PRE_ADD_SECONDARY_SKILL
#include "DnLifeSkillFishingTask.h"
#include "DnLifeSkillCookingTask.h"
#endif // PRE_ADD_SECONDARY_SKIL
#include "DnTimeEventTask.h"
#include "DnSystemDlg.h"
#include "DnPvPGameTask.h"
#include "PvPSendPacket.h"
#include "TradeSendPacket.h"
#include "DnGuildWarSkillSlotDlg.h"
#include "DnCommonTask.h"
#ifdef PRE_TEST_ANIMATION_UI
#include "DnAniTestDlg.h"
#endif
#include "DnPetTask.h"
#include "DnEventReceiverTabDlg.h"

#ifdef PRE_MOD_POTENTIAL_JEWEL_RENEWAL
#include "DnPotentialJewelDlg.h"
#endif

#ifdef PRE_ADD_STAMPSYSTEM
#include "DnWorldData.h"
#include "DnNPCActor.h"
#include "DnInterface.h"
#include "DnNPCArrowDlg.h"
#endif // PRE_ADD_STAMPSYSTEM

#ifdef PRE_ADD_DWC
#include "DnDWCTask.h"
#endif

#ifdef PRE_ADD_LEVELUP_REWARD_NOTIFIER
#include "DnInvenTabDlg.h"
#include "DnInvenCashDlg.h"
#endif


#include "DnMainBarMenuDlg.h"
#include "DnMainBar_MinimapDlg.h"
#include "DnMainBar_EventDlg.h"
//#include "DnMainBar_BubbleGaugeDlg.h"


#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

#define RIGHT_DEGREE_MAX	0.750491f
#define	LEFT_DEGREE_MAX		-3.893829f

#ifdef _ADD_MAINBAR_NEW

CDnMainDlg::CDnMainDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback)
	: CDnCustomDlg(dialogType, pParentDialog, nID, pCallback)
{
	m_pStaticSlotNumber = NULL;

	m_pExpDlg = NULL;
	m_pStaticBackLight = NULL;
	m_pStaticBackBlack = NULL;

	m_dwShowQuickDlgID = 0;
	m_dwPrevQuickDlgID = 0;
	m_dwOrignalShowQuickSlotDlgID = 0;
	m_dwOrignalPrevQuickSlotDlgID = 0;

	m_fDegree = 0.0f;
	m_pStaticExp = NULL;

	m_pStaticPost = NULL;
	m_pStaticCashShopGift = NULL;
	m_pStaticCashShopGiftBalloon = NULL;
	m_pStaticCashShopGiftText = NULL;
	m_pStaticHarvest = NULL;
	m_pStaticCheckAttendanceBalloon = NULL;
	m_pStaticCheckAttendanceText = NULL;
#ifdef PRE_ADD_LEVELUP_REWARD_NOTIFIER
	m_pStaticLevelUpNotifier = NULL;
#endif 
	m_bCheckAttendanceFirst = false;
	m_fCheckAttendanceAlarmTime = 0.0f;
	m_bNotifyCheckAttendance = false;

#ifdef PRE_REMOVE_MINISITE
#else
	m_pNewMiniSiteBtn = NULL;
#endif
	m_fMiniSiteAlarmTime = eMiniSiteBlinkTime;
	m_bNotifyCheckMiniSite = false;

	m_bCashShopGiftAlarm = false;
	m_fCashShopGiftAlarmTime = 0.f;

	m_bHarvestAlarm = false;
	m_fHarvestAlarmTime = 0.f;
	m_nHarvestCount = 0;

	m_bMailAlarm = false;
	m_fMailAlarmTime = 0.0f;

#ifdef PRE_ADD_LEVELUP_REWARD_NOTIFIER
	m_fLevelUpboxAlarmTime = 0.0f;
	m_bLevelUpBoxAlarm = false;
#endif 

	m_bPVP = false;
	m_pPassiveSkillSlotDlg = NULL;
	m_bShowOption = false;

	m_bLockQuickSlot = false;

	m_pPopupFTGDlg = NULL;
	m_pDnGuildWarSkillSlotDlg = NULL;

	memset(m_nMainDlgSoundIndex, 0, sizeof(m_nMainDlgSoundIndex));

	m_bEnableButtons = true;

	memset(m_pStaticTimeEventIcon, 0, sizeof(m_pStaticTimeEventIcon));
	m_pStaticTimeEventText = NULL;
	m_cUpdateEventType = -1;
	m_nTimeEventID = 0;
	m_fTimeEventHideDelta = 0.f;
	m_bShowTimeEventAlarm = true;

#ifdef PRE_ADD_LEVELUP_GUIDE
	m_pStaticLevelUpSkillAlarm = NULL;
	m_pStaticLevelUpSkillAlarmText = NULL;
	m_bLevelUpSkillAlarm = false;
	m_fLevelUpSkillAlarmTime = 0.f;
#endif

#ifdef PRE_TEST_ANIMATION_UI
	m_pAniTestBtn = NULL;
	m_pAniTestDlg = NULL;
#endif

#ifdef PRE_ADD_AUTO_DICE
	m_bShowDiceOption = false;
#endif
	m_nSkillSlotPage = 0;
#ifdef PRE_FIX_HOLD_COSTUMEMIX_INVEN
	m_HoldToggleShowDialogOnProcessButton = -1;
#endif
	m_pPCRoomFTGGauge = NULL;
	m_pStaticEventFTG = NULL;
	m_pStaticEventFTGFont = NULL;
	m_pStaticFTG = NULL;
	m_pStaticFTGFont = NULL;
	m_pStaticPCRoomFTG = NULL;
	m_pStaticPCRoomFTGFont = NULL;
	m_pStaticVIPFTG = NULL;
	m_pStaticVIPFTGFont = NULL;
	m_pStaticWeekFTG = NULL;
	m_pStaticWeekFTGFont = NULL;
	m_pVIPFTGGauge = NULL;
	m_pWeekFTGGauge = NULL;
	m_pEventFTGGauge = NULL;
	m_pFTGGauge = NULL;

	m_pFoodItem = NULL;
	m_pFoodItemSlot = NULL;
	m_pStaticFoodItemCount = NULL;

#ifdef PRE_ADD_MAIL_UI_IMPROVEMENTS
	m_pStaticMailText = NULL;
	m_pStaticMailBallon = NULL;
#endif
#ifdef PRE_ADD_REMOTE_QUEST
	m_bShowQuestNotify = false;
	m_pStaticQuestText = NULL;
	m_pStaticQuestBallon = NULL;
#endif // PRE_ADD_REMOTE_QUEST
#ifdef PRE_SPECIALBOX
	m_pButtonEventReceiverAlarm = NULL;
	m_pStaticEventReceiverBalloon = NULL;
	m_pStaticEventReceiverText = NULL;
	m_fEventReceiverAlarmTime = 0.f;
	m_bEventReceiverAlarm = false;
#endif

#ifdef PRE_MOD_APPELLATIONBOOK_RENEWAL
	m_pStaticNewAppellationAlarm = NULL;
	m_pStaticNewAppellationAlarmText = NULL;
#endif
	/*
	#if defined( PRE_ALTEIAWORLD_EXPLORE )
	m_pAlteaDiceIcon = NULL;
	m_bAlteaDiceShow = false;
	m_bAlteaDiceAlarm = false;
	m_fAlteaDiceAlarmTime = 0.0f;
	#endif	// #if defined( PRE_ALTEIAWORLD_EXPLORE )
	*/
#ifdef PRE_ADD_STAMPSYSTEM
	m_NPCArrowDlg = NULL;
	m_bSearching = false; // 찾는중
	m_SearchedNpcID = -1;
	m_camDir.x = m_camDir.y = m_camDir.z = 0.0f;
	m_vNPCPos.x = m_vNPCPos.y = m_vNPCPos.z = 0.0f;
	m_fRotDegree = 0.0f;
#endif // PRE_ADD_STAMPSYSTEM

	//New menu bar
	m_pMainBarMenuDlg = NULL;
	m_pLoveIcon = NULL;
	//m_pFarmPVPBtnDlg = NULL;
	m_pMainBar_MinimapDlg = NULL;
	m_pMainBar_EventDlg = NULL;
//	m_pMainBar_BubbleGaugeDlg = NULL;
}

CDnMainDlg::~CDnMainDlg(void)
{
	SAFE_RELEASE_SPTR(m_hBackLight);
	SAFE_RELEASE_SPTR(m_hBackBlack);
	SAFE_DELETE(m_pExpDlg);
	SAFE_DELETE(m_pPassiveSkillSlotDlg);
	SAFE_RELEASE_SPTR(m_hNorthTexture);
	SAFE_DELETE(m_pPopupFTGDlg);
	SAFE_DELETE(m_pDnGuildWarSkillSlotDlg);
#ifdef PRE_TEST_ANIMATION_UI
	SAFE_DELETE(m_pAniTestDlg);
#endif
	SAFE_DELETE(m_pFoodItem);
	SAFE_DELETE(m_pMainBarMenuDlg); //rlkt fix
									//	SAFE_DELETE( m_pFarmPVPBtnDlg );
	SAFE_DELETE(m_pMainBar_MinimapDlg);
	SAFE_DELETE(m_pMainBar_EventDlg);
//	SAFE_DELETE(m_pMainBar_BubbleGaugeDlg);
#ifdef PRE_ADD_STAMPSYSTEM
	FindComplete();
	SAFE_DELETE(m_NPCArrowDlg);
#endif // PRE_ADD_STAMPSYSTEM

}

void CDnMainDlg::Initialize(bool bShow)
{
	CEtUIDialog::Initialize(CEtResourceMng::GetInstance().GetFullName("MainBar_New.ui").c_str(), bShow);

	const char *szFileName = CDnTableDB::GetInstance().GetFileName(10033);
	if (strlen(szFileName) > 0)
		m_nMainDlgSoundIndex[eSOUND_MAILALARM] = CEtSoundEngine::GetInstance().LoadSound(CEtResourceMng::GetInstance().GetFullName(szFileName).c_str(), false, false);

	szFileName = CDnTableDB::GetInstance().GetFileName(10043);
	if (strlen(szFileName) > 0)
		m_nMainDlgSoundIndex[eSOUND_CASHSHOPGIFT] = CEtSoundEngine::GetInstance().LoadSound(CEtResourceMng::GetInstance().GetFullName(szFileName).c_str(), false, false);
}

void CDnMainDlg::InitialUpdate()
{
	/////////////////////////////////////////////////////////////////////////////
	// Note : QuickSlot Dialog 생성
	CDnQuickSlotDlg *pQuickSlotDlg(NULL);

	pQuickSlotDlg = new CDnQuickSlotDlg(UI_TYPE_CHILD, this, QUICKSLOT_DLG_00);
	pQuickSlotDlg->Initialize(false);
	m_QuickSlgDlgGroup.AddDialog(QUICKSLOT_DLG_00, pQuickSlotDlg);

	pQuickSlotDlg = new CDnQuickSlotDlg(UI_TYPE_CHILD, this, QUICKSLOT_DLG_01);
	pQuickSlotDlg->Initialize(false);
	m_QuickSlgDlgGroup.AddDialog(QUICKSLOT_DLG_01, pQuickSlotDlg);

	pQuickSlotDlg = new CDnQuickSlotDlg(UI_TYPE_CHILD, this, QUICKSLOT_SECOND_DLG_00);
	pQuickSlotDlg->Initialize(false);
	m_QuickSlgDlgGroup.AddDialog(QUICKSLOT_SECOND_DLG_00, pQuickSlotDlg);

	pQuickSlotDlg = new CDnQuickSlotDlg(UI_TYPE_CHILD, this, QUICKSLOT_SECOND_DLG_01);
	pQuickSlotDlg->Initialize(false);
	m_QuickSlgDlgGroup.AddDialog(QUICKSLOT_SECOND_DLG_01, pQuickSlotDlg);

	pQuickSlotDlg = new CDnQuickSlotDlg(UI_TYPE_CHILD, this, QUICKSLOT_EVENT);
	pQuickSlotDlg->Initialize(false);
	m_QuickSlgDlgGroup.AddDialog(QUICKSLOT_EVENT, pQuickSlotDlg);



	CDnLifeSkillQuickSlotDlg *pGestureQuickSlotDlg(NULL);
	pGestureQuickSlotDlg = new CDnLifeSkillQuickSlotDlg(UI_TYPE_CHILD, this, LIFESKILL_DLG);
	pGestureQuickSlotDlg->Initialize(false);
	m_QuickSlgDlgGroup.AddDialog(LIFESKILL_DLG, pGestureQuickSlotDlg);

	m_QuickSlgDlgGroup.ShowDialog(QUICKSLOT_DLG_00, true);

	m_pStaticSlotNumber = GetControl<CEtUIStatic>("ID_STATIC_SLOT");
	m_pStaticSlotNumber->SetText(L"1");
	m_pStaticSlotNumber->Enable(true);

	//PVP ICON
	//m_pFarmPVPBtnDlg = new CDnFarmPVPBtnDlg(UI_TYPE_CHILD, this);
	//m_pFarmPVPBtnDlg->Initialize(true);

	//Main Menu Bar
	m_pMainBarMenuDlg = new CDnMainBarMenuDlg(UI_TYPE_CHILD, this);
	m_pMainBarMenuDlg->Initialize(false);

	m_pMainBar_MinimapDlg = new CDnMainBar_MinimapDlg(UI_TYPE_CHILD, this);
	m_pMainBar_MinimapDlg->Initialize(true);

	m_pMainBar_EventDlg = new CDnMainBar_EventDlg(UI_TYPE_CHILD, this);
	m_pMainBar_EventDlg->Initialize(true);

//	m_pMainBar_BubbleGaugeDlg = new CDnMainBar_BubbleGaugeDlg(UI_TYPE_CHILD, this);
//	m_pMainBar_BubbleGaugeDlg->Initialize(true); //???

	m_pExpDlg = new CDnExpDlg(UI_TYPE_CHILD, this);
	m_pExpDlg->Initialize(false);

	m_pExpGauge[0] = GetControl<CDnExpGauge>("ID_EXP_GAUGE0");
	m_pExpGauge[1] = GetControl<CDnExpGauge>("ID_EXP_GAUGE1");
	m_pExpGauge[2] = GetControl<CDnExpGauge>("ID_EXP_GAUGE2");
	m_pExpGauge[3] = GetControl<CDnExpGauge>("ID_EXP_GAUGE3");
	m_pExpGauge[4] = GetControl<CDnExpGauge>("ID_EXP_GAUGE4");
	m_pStaticExp = GetControl<CEtUIStatic>("ID_STATIC_EXP");
	m_pExpGauge[0]->UseGaugeEndElement(1);
	m_pExpGauge[1]->UseGaugeEndElement(1);
	m_pExpGauge[2]->UseGaugeEndElement(1);
	m_pExpGauge[3]->UseGaugeEndElement(1);
	m_pExpGauge[4]->UseGaugeEndElement(1);

	m_pFTGGauge = GetControl<CDnExpGauge>("ID_FTG_GAUGE");
	m_pStaticFTGFont = GetControl<CEtUIStatic>("ID_STATIC_FTGFONT");
	m_pStaticFTG = GetControl<CEtUIStatic>("ID_STATIC_FTG");
	m_pFTGGauge->UseShadeProcessBar(3, 2);
	m_pFTGGauge->UseGaugeEndElement(1);

	m_pWeekFTGGauge = GetControl<CDnExpGauge>("ID_FWG_GAUGE");
	m_pStaticWeekFTGFont = GetControl<CEtUIStatic>("ID_STATIC_FWGFONT");
	m_pStaticWeekFTG = GetControl<CEtUIStatic>("ID_STATIC_FWG");
	m_pWeekFTGGauge->UseShadeProcessBar(3, 2);
	m_pWeekFTGGauge->UseGaugeEndElement(1);

	m_pPCRoomFTGGauge = GetControl<CDnExpGauge>("ID_FPG_GAUGE");
	m_pStaticPCRoomFTGFont = GetControl<CEtUIStatic>("ID_STATIC_FPGFONT");
	m_pStaticPCRoomFTG = GetControl<CEtUIStatic>("ID_STATIC_FPG");
	m_pPCRoomFTGGauge->UseShadeProcessBar(3, 2);
	m_pPCRoomFTGGauge->UseGaugeEndElement(1);

	m_pEventFTGGauge = GetControl<CDnExpGauge>("ID_FEG_GAUGE");
	m_pStaticEventFTGFont = GetControl<CEtUIStatic>("ID_STATIC_FEGFONT");
	m_pStaticEventFTG = GetControl<CEtUIStatic>("ID_STATIC_FEG");
	m_pEventFTGGauge->UseShadeProcessBar(3, 2);
	m_pEventFTGGauge->UseGaugeEndElement(1);

	m_pVIPFTGGauge = GetControl<CDnExpGauge>("ID_FVG_GAUGE");
	m_pStaticVIPFTGFont = GetControl<CEtUIStatic>("ID_STATIC_FVGFONT");
	m_pStaticVIPFTG = GetControl<CEtUIStatic>("ID_STATIC_FVG");

#ifdef PRE_ADD_VIP
	m_pVIPFTGGauge->UseShadeProcessBar(3, 2);
	m_pVIPFTGGauge->UseGaugeEndElement(1);
#else
	if (m_pVIPFTGGauge)
		m_pVIPFTGGauge->Show(false);
	if (m_pStaticVIPFTGFont)
		m_pStaticVIPFTGFont->Show(false);
	if (m_pStaticVIPFTG)
		m_pStaticVIPFTG->Show(false);
#endif

	m_pPopupFTGDlg = new CDnPopupFatigueDlg(UI_TYPE_TOP_MSG);	// 미니맵 외곽에 나오는 아이콘때문에 Child로 그리면 가려서 안된다.
	m_pPopupFTGDlg->Initialize(false);

	m_pDnGuildWarSkillSlotDlg = new CDnGuildWarSkillSlotDlg(UI_TYPE_CHILD, this);
	m_pDnGuildWarSkillSlotDlg->Initialize(false);

	m_hBackLight = LoadResource(CEtResourceMng::GetInstance().GetFullName("MainBarLight.dds").c_str(), RT_TEXTURE);
	m_hBackBlack = LoadResource(CEtResourceMng::GetInstance().GetFullName("MainBarBlack.dds").c_str(), RT_TEXTURE);

	m_pStaticBackLight = GetControl<CEtUIStatic>("ID_BACK_LIGHT");
	m_pStaticBackLight->Show(false);
	m_pStaticBackBlack = GetControl<CEtUIStatic>("ID_BACK_BLACK");
	m_pStaticBackBlack->Show(false);
	
#ifdef PRE_ADD_MAIL_UI_IMPROVEMENTS
	m_pStaticMailText = GetControl<CEtUIStatic>("ID_TEXT_MAIL");
	m_pStaticMailText->Show(false);
	m_pStaticMailBallon = GetControl<CEtUIStatic>("ID_STATIC_MAIL");
	m_pStaticMailBallon->Show(false);
#endif 
	m_pStaticPost = GetControl<CEtUIStatic>("ID_BT_POST");
	m_pStaticPost->Show(false);

	m_pStaticCashShopGift = GetControl<CEtUIStatic>("ID_BT_CPRESENT");
	m_pStaticCashShopGift->Show(false);

	//disabled.
	/*m_pStaticCashShopGiftBalloon = GetControl<CEtUIStatic>("ID_STATIC_CPCHAT");
	m_pStaticCashShopGiftBalloon->Show(false);
	m_pStaticCashShopGiftText = GetControl<CEtUIStatic>("ID_TEXT_CPCHAT");
	m_pStaticCashShopGiftText->Show(false);
	*/
#ifdef PRE_ADD_LEVELUP_REWARD_NOTIFIER
	m_pStaticLevelUpNotifier = GetControl<CEtUIButton>("ID_BT_LVPOST");
	m_pStaticLevelUpNotifier->Show(false);
#endif 
	m_pStaticHarvest = GetControl<CEtUIStatic>("ID_BT_LIFE");
	m_pStaticHarvest->Show(false);

	//Love icon
	//m_pLoveIcon = GetControl<CEtUIButton>("ID_BT_DNGIRL");
	//m_pLoveIcon->Show(true);

#ifdef PRE_ADD_REMOTE_QUEST
	//m_pStaticQuestText = GetControl<CEtUIStatic>("ID_TEXT_QUESTNOTICE");
	//m_pStaticQuestText->Show(false);
	m_pStaticQuestBallon = GetControl<CEtUIStatic>("ID_STATIC_QUESTNOTICE");
	m_pStaticQuestBallon->Show(false);
#endif // PRE_ADD_REMOTE_QUEST

	SButtonInfo buttonInfo;

	buttonInfo.pButton = GetControl<CDnMenuButton>("ID_CHAR_STATUS");
	buttonInfo.nDialogID = CDnMainMenuDlg::CHARSTATUS_DIALOG;
	m_vecButtonInfo.push_back(buttonInfo);

	buttonInfo.pButton = GetControl<CDnMenuButton>("ID_INVENTORY");
	buttonInfo.nDialogID = CDnMainMenuDlg::INVENTORY_DIALOG;
	m_vecButtonInfo.push_back(buttonInfo);

	buttonInfo.pButton = GetControl<CDnMenuButton>("ID_SKILL");
	buttonInfo.nDialogID = CDnMainMenuDlg::SKILL_DIALOG;
	m_vecButtonInfo.push_back(buttonInfo);

	buttonInfo.pButton = GetControl<CDnMenuButton>("ID_GUILD");
	buttonInfo.nDialogID = CDnMainMenuDlg::COMMUNITY_DIALOG;
	m_vecButtonInfo.push_back(buttonInfo);

	buttonInfo.pButton = GetControl<CDnMenuButton>("ID_BT_QUEST");
	buttonInfo.nDialogID = CDnMainMenuDlg::QUEST_DIALOG;
	m_vecButtonInfo.push_back(buttonInfo);

	buttonInfo.pButton = GetControl<CDnMenuButton>("ID_BT_MAP");
	buttonInfo.nDialogID = CDnMainMenuDlg::ZONEMAP_DIALOG;
	m_vecButtonInfo.push_back(buttonInfo);

	buttonInfo.pButton = GetControl<CDnMenuButton>("ID_BT_MISSION");
	buttonInfo.nDialogID = CDnMainMenuDlg::MISSION_DIALOG;
	m_vecButtonInfo.push_back(buttonInfo);

	buttonInfo.pButton = GetControl<CDnMenuButton>("ID_BT_SYSTEM");
	buttonInfo.nDialogID = CDnMainMenuDlg::SYSTEM_DIALOG;
	m_vecButtonInfo.push_back(buttonInfo);

	buttonInfo.pButton = GetControl<CDnMenuButton>("ID_BT_CHAT");
	buttonInfo.nDialogID = CDnMainMenuDlg::CHATROOM_DIALOG;
	m_vecButtonInfo.push_back(buttonInfo);

	//N  button
	buttonInfo.pButton = GetControl<CDnMenuButton>("ID_MENU");
	buttonInfo.nDialogID = CDnMainMenuDlg::MENUNEW_DIALOG;
	m_vecButtonInfo.push_back(buttonInfo);

#if defined(PRE_ADD_INTEGERATE_QUEST_REPUT)
	buttonInfo.pButton = GetControl<CDnMenuButton>("ID_BT_REPUTE");
	buttonInfo.nDialogID = CDnMainMenuDlg::InTEGRATE_QUESTNREPUTATION_DIALOG;
#ifdef PRE_ADD_NO_REPUTATION_DLG
	buttonInfo.pButton->Enable(false);
#endif
	m_vecButtonInfo.push_back(buttonInfo);
#else
#ifdef PRE_ADD_NPC_REPUTATION_SYSTEM
	buttonInfo.pButton = GetControl<CDnMenuButton>("ID_REPUTE");
	buttonInfo.nDialogID = CDnMainMenuDlg::NPC_REPUTATION_TAB_DIALOG;
#ifdef PRE_ADD_NO_REPUTATION_DLG
	buttonInfo.pButton->Enable(false);
#endif
	m_vecButtonInfo.push_back(buttonInfo);
#endif // #ifdef PRE_ADD_NPC_REPUTATION_SYSTEM
#endif // PRE_ADD_INTEGERATE_QUEST_REPUT

#if defined(PRE_ADD_INTEGERATE_EVENTUI)
	buttonInfo.pButton = GetControl<CDnMenuButton>("ID_BT_EVENT");
	buttonInfo.nDialogID = CDnMainMenuDlg::INTEGRATE_EVENT_DIALOG;
	m_vecButtonInfo.push_back(buttonInfo);
	GetControl<CDnMenuButton>("ID_EVENT")->Show(true);

#else
	buttonInfo.pButton = GetControl<CDnMenuButton>("ID_EVENT");
	buttonInfo.nDialogID = CDnMainMenuDlg::TIMEEVENT_DIALOG;
	m_vecButtonInfo.push_back(buttonInfo);
	GetControl<CDnMenuButton>("ID_EVENT")->Show(true);

	buttonInfo.pButton = GetControl<CDnMenuButton>("ID_CHECK");
	buttonInfo.nDialogID = CDnMainMenuDlg::ATTENDANCEEVENT_DIALOG;
	m_vecButtonInfo.push_back(buttonInfo);
	GetControl<CDnMenuButton>("ID_CHECK")->Show(false);
#endif // PRE_ADD_INTEGERATE_EVENTUI

#ifdef PRE_ADD_PVP_VILLAGE_ACCESS
	buttonInfo.pButton = GetControl<CDnMenuButton>("ID_BT_COLOSSEUM");
	buttonInfo.nDialogID = CDnMainMenuDlg::PVP_VILLAGE_ACCESS;
	m_vecButtonInfo.push_back(buttonInfo);
#endif

#if defined(PRE_ADD_MAILBOX_OPEN)
	buttonInfo.pButton = GetControl<CDnMenuButton>("ID_MAIL");
	buttonInfo.nDialogID = CDnMainMenuDlg::MAIL_DIALOG;
	m_vecButtonInfo.push_back(buttonInfo);
#endif // PRE_ADD_MAILBOX_OPEN

#ifdef PRE_ADD_PVP_RANKING
	buttonInfo.pButton = GetControl<CDnMenuButton>("ID_RANK");
	buttonInfo.nDialogID = CDnMainMenuDlg::PVP_RANK_DIALOG;
	m_vecButtonInfo.push_back(buttonInfo);
	//GetControl<CDnMenuButton>( "ID_RANK" )->Show( true );
#endif // PRE_ADD_PVP_RANKING

	m_pStaticCheckAttendanceBalloon = GetControl<CEtUIStatic>("ID_STATIC_CHECKBOX");
	m_pStaticCheckAttendanceBalloon->Show(false);
	m_pStaticCheckAttendanceText = GetControl<CEtUIStatic>("ID_TEXT_CHECK");
	m_pStaticCheckAttendanceText->Show(false);

#ifdef PRE_REMOVE_MINISITE
#else
	m_pNewMiniSiteBtn = GetControl<CEtUIButton>("ID_BT_MOVIE");
	m_pNewMiniSiteBtn->Show(true);
#endif
	/*
	#if defined( PRE_ALTEIAWORLD_EXPLORE )
	m_pAlteaDiceIcon = GetControl<CEtUIButton>( "ID_BT_ALTEA" );
	#endif	// #if defined( PRE_ALTEIAWORLD_EXPLORE )
	*/
	CDnMessageStatic *pMsgStatic(NULL);
	char szName[32] = { 0 };
	for (int i = 0; i<7; i++)
	{
		sprintf_s(szName, 32, "ID_MESSAGE_%02d", i);
		pMsgStatic = GetControl<CDnMessageStatic>(szName);
		m_vecStaticMessage.push_back(pMsgStatic);
	}

	m_pPassiveSkillSlotDlg = new CDnPassiveSkillSlotDlg(UI_TYPE_CHILD, this);
	m_pPassiveSkillSlotDlg->Initialize(true);

	m_bShowOption = true;
	ShowMinimapOption(false);
	GetControl<CEtUICheckBox>("ID_CHECKBOX_FUNCTIONNPC")->SetChecked(true);
	GetControl<CEtUICheckBox>("ID_CHECKBOX_NPC")->SetChecked(true);
	if (CDnMinimap::IsActive()) {
		GetMiniMap().ShowFunctionalNPC(true);
		GetMiniMap().ShowOtherNPC(true);
	}

	#ifdef PRE_ADD_AUTO_DICE
	m_bShowDiceOption = true;
	GetControl<CEtUICheckBox>("ID_CHECKBOX_DICE")->SetChecked( CGameOption::GetInstance().m_bEnableAutoDice );
	ShowAutoDiceOption(false);
	#endif
	
	//rlkt 17.06 
	m_hNorthTexture = LoadResource( CEtResourceMng::GetInstance().GetFullName( "Minimap_N.dds" ).c_str(), RT_TEXTURE );

	//ADD MISSING!
	SUIControlProperty sUIProperty;
	GetControl<CEtUIStatic>("ID_BT_MAP")->GetProperty(sUIProperty);
	strcpy(sUIProperty.szUIName, "ID_BT_CHAT");
	CEtUIDialog::CreateControl(&sUIProperty);
	strcpy(sUIProperty.szUIName, "ID_BT_REPUTE");
	CEtUIDialog::CreateControl(&sUIProperty);
	strcpy(sUIProperty.szUIName, "ID_BT_EVENT");
	CEtUIDialog::CreateControl(&sUIProperty);
	strcpy(sUIProperty.szUIName, "ID_BT_COLOSSEUM");
	CEtUIDialog::CreateControl(&sUIProperty);
	strcpy(sUIProperty.szUIName, "ID_BT_RANK");
	CEtUIDialog::CreateControl(&sUIProperty);
	strcpy(sUIProperty.szUIName, "ID_BT_MAP");
	CEtUIDialog::CreateControl(&sUIProperty);

	// 재로딩이 있을때마다 키설정값을 다시 불러온다.
	for (int i = 0; i < UIWrappingKeyIndex_Amount; ++i)
		SetMainUIHotKey(i, _ToVK(g_UIWrappingKeyData[i]));
	// 임시. 나중에 제대로 추가해야한다.
	//GetControl("ID_CHAT")->SetHotKey( 'C' );




	for (int i = 0; i<6; i++) {
		sprintf_s(szName, "ID_STATIC_TIMEEVENT%d", i);
		m_pStaticTimeEventIcon[i] = GetControl<CEtUIButton>(szName); //CEtUIStatic
	}
	m_pStaticTimeEventText = GetControl<CEtUIStatic>("ID_TEXT_TIME");

#ifdef PRE_ADD_LEVELUP_GUIDE
	m_pStaticLevelUpSkillAlarm = GetControl<CEtUIStatic>("ID_STATIC_SKILLUP");
	m_pStaticLevelUpSkillAlarm->Show(false);

	//fix skill ballon showing
	m_pStaticLevelUpSkillAlarmText = GetControl<CEtUIStatic>("ID_STATIC_SKILL");
	m_pStaticLevelUpSkillAlarmText->Show(false);

	m_pStaticLevelUpSkillAlarmText = GetControl<CEtUIStatic>("ID_STATIC_INVEN");
	m_pStaticLevelUpSkillAlarmText->Show(false);

	m_pStaticLevelUpSkillAlarmText = GetControl<CEtUIStatic>("ID_STATIC_COMMUNITY");
	m_pStaticLevelUpSkillAlarmText->Show(false);

	m_pStaticLevelUpSkillAlarmText = GetControl<CEtUIStatic>("ID_STATIC_MENU");
	m_pStaticLevelUpSkillAlarmText->Show(false);


#endif

#ifdef PRE_TEST_ANIMATION_UI
	m_pAniTestBtn = GetControl<CEtUIButton>("ID_TESTANI");
	m_pAniTestDlg = new CDnAniTestDlg(UI_TYPE_CHILD, this);
	m_pAniTestDlg->Initialize(false);
#endif

	m_pFoodItemSlot = GetControl<CDnItemSlotButton>("ID_BT_PETFOOD");
	m_pFoodItemSlot->Show(false);
	m_pStaticFoodItemCount = GetControl<CEtUIStatic>("ID_TEXT_PETFOOD");
	m_pStaticFoodItemCount->Show(false);

#ifdef PRE_SPECIALBOX
	m_pButtonEventReceiverAlarm = GetControl<CEtUIButton>("ID_BT_EVENTALARM");
	//m_pStaticEventReceiverBalloon = GetControl<CEtUIStatic>( "ID_STATIC_ALARM" );
	//m_pStaticEventReceiverText = GetControl<CEtUIStatic>( "ID_TEXT_ALARM" );
#endif

#ifdef PRE_MOD_APPELLATIONBOOK_RENEWAL
	m_pStaticNewAppellationAlarm = GetControl<CEtUIStatic>("ID_STATIC_APP");
	m_pStaticNewAppellationAlarm->Show(false);
	//m_pStaticNewAppellationAlarmText = GetControl<CEtUIStatic>("ID_TEXT_APP");
#endif	

#ifdef PRE_ADD_STAMPSYSTEM
	m_NPCArrowDlg = new CDnNPCArrowDlg();
	m_NPCArrowDlg->Initialize(false);
#endif // PRE_ADD_STAMPSYSTEM



}

void CDnMainDlg::Show(bool bShow)
{
	if (CDnPetTask::IsActive() && !GetPetTask().IsLoadPetConfig())
	{
		GetPetTask().LoadPetConfig();
		SetPetFoodInfo(GetPetTask().GetPetOption().m_nFoodItemID);
	}

	if (m_bShow == bShow)
		return;

	if (bShow)
	{
		m_QuickSlgDlgGroup.ShowDialog(m_dwShowQuickDlgID, true);
	}
	else
	{
		m_dwShowQuickDlgID = m_QuickSlgDlgGroup.GetShowDialogID();
	}

	CEtUIDialog::Show(bShow);
}

void CDnMainDlg::ShowQuickSlotChangeMessage()
{
	if (!CDnActor::s_hLocalActor) return;
	if (CDnActor::s_hLocalActor->GetLevel() > 15) return;

	WCHAR wszStr[128] = { 0, };
	swprintf_s(wszStr, GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 209), g_szKeyString[g_UIWrappingKeyData[IW_UI_QUICKSLOTCHANGE]]);
	GetInterface().AddChatMessage(CHATTYPE_SYSTEM, L"", wszStr, false);
}

void CDnMainDlg::ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg)
{
	SetCmdControlName(pControl->GetControlName());
	///RLKTTEST///
	m_pMainBarMenuDlg->ProcessCommand(nCommand, bTriggeredByUser, pControl, uMsg);


	if (!GetInterface().IsLockMainMenu()) {
		if (nCommand == EVENT_BUTTON_CLICKED)
		{
#ifdef PRE_ADD_SECONDARY_SKILL
			// 낚시, 요리 중일 경우 인벤토리 외 다른 창 사용 불가
			if (((CDnLifeSkillFishingTask::IsActive() && GetLifeSkillFishingTask().IsNowFishing())
				|| (CDnLifeSkillCookingTask::IsActive() && GetLifeSkillCookingTask().IsNowCooking()))
				&& !(IsCmdControl("ID_INVENTORY") || IsCmdControl("ID_COMMUNITY")))
				return;
#endif // PRE_ADD_SECONDARY_SKILL

			if (IsCmdControl("ID_BUTTON_SLOT_UP"))
			{
				if (m_QuickSlgDlgGroup.GetShowDialogID() != RADIOMSG_DLG &&
					m_QuickSlgDlgGroup.GetShowDialogID() != LIFESKILL_DLG)
				{
					ChangeQuickSlotUp();
					ShowQuickSlotChangeMessage();
				}
				return;
			}

			if (IsCmdControl("ID_BUTTON_SLOT_DOWN"))
			{
				if (m_QuickSlgDlgGroup.GetShowDialogID() != RADIOMSG_DLG &&
					m_QuickSlgDlgGroup.GetShowDialogID() != LIFESKILL_DLG)
				{
					ChangeQuickSlotDown();
					ShowQuickSlotChangeMessage();
				}
				return;
			}

			if (IsCmdControl("ID_BUTTON_DUMMY"))
			{
				// 제대로 구현될때까지 임시로 뺀다.
				return;

				// 별도의 라디오메세지 모드 bool값을 사용하지 않고, 그냥 있던 변수 m_dwShowQuickDlgID를 사용한다.
				// 만약 라디오메세지 모드에서 창이 show(false)되었다 show(true)되면 그냥 기본 퀵슬롯 상태로 되돌린다.
				if (m_QuickSlgDlgGroup.GetShowDialogID() != RADIOMSG_DLG)
				{
					m_dwShowQuickDlgID = m_QuickSlgDlgGroup.GetShowDialogID();
					m_QuickSlgDlgGroup.ShowDialog((DWORD)RADIOMSG_DLG, true);
					m_pStaticSlotNumber->SetText(L"V");
					GetControl<CEtUIButton>("ID_BUTTON_SLOT_UP")->Enable(false);
					GetControl<CEtUIButton>("ID_BUTTON_SLOT_DOWN")->Enable(false);
					ShowExpDialog(false);
					ShowFTGDialog(false);
				}
				else
				{
					m_QuickSlgDlgGroup.ShowDialog(m_dwShowQuickDlgID, true);
					m_pStaticSlotNumber->SetIntToText(m_dwShowQuickDlgID + 1);
					GetControl<CEtUIButton>("ID_BUTTON_SLOT_UP")->Enable(true);
					GetControl<CEtUIButton>("ID_BUTTON_SLOT_DOWN")->Enable(true);
					ShowExpDialog(false);
					ShowFTGDialog(false);
				}
				return;
			}


			if (IsCmdControl("ID_BUTTON_PLUS"))
			{
				if (CDnMinimap::IsActive() == true)
					GetMiniMap().Zoom(+1.0f);
				return;
			}

			if (IsCmdControl("ID_BUTTON_MINUS"))
			{
				if (CDnMinimap::IsActive() == true)
					GetMiniMap().Zoom(-1.0f);
				return;
			}

			if (IsCmdControl("ID_BUTTON_MAP"))
			{
				ShowMinimapOption(!m_bShowOption);
				return;
			}

#ifdef PRE_ADD_AUTO_DICE
			if (IsCmdControl("ID_BUTTON_DICE"))
			{
				ShowAutoDiceOption(!m_bShowDiceOption);
				return;
			}
#endif

#ifdef PRE_ADD_PVP_VILLAGE_ACCESS
			if (IsCmdControl("ID_COLOSSEUM"))
			{
#ifdef PRE_ADD_DWC
				if (GetDWCTask().IsDWCChar() == false)
				{
					if (CDnWorld::GetInstance().GetMapType() == CDnWorld::MapTypeVillage)
					{
						if (GetInterface().IsShowPVPVillageAccessDlg())
							GetInterface().ShowPVPVillageAccessDlg(false);
						else
							SendRequestPVPVillageAcess();
					}
				}
#else	// PRE_ADD_DWC
				if (CDnWorld::GetInstance().GetMapType() == CDnWorld::MapTypeVillage)
				{
					if (GetInterface().IsShowPVPVillageAccessDlg())
						GetInterface().ShowPVPVillageAccessDlg(false);
					else
						SendRequestPVPVillageAcess();
				}
#endif	// PRE_ADD_DWC
			}
#endif // PRE_ADD_PVP_VILLAGE_ACCESS

			CDnMainMenuDlg *pMainMenuDlg = GetInterface().GetMainMenuDialog();
			if (!pMainMenuDlg) return;

			if (GetInterface().IsShowPrivateMarketDlg()
				|| GetInterface().IsShowCostumeMixDlg()
				|| GetInterface().IsShowCostumeDesignMixDlg()
#ifdef PRE_ADD_COSRANDMIX
				|| GetInterface().IsShowCostumeRandomMixDlg()
#endif
				)
				return;

			if (GetInterface().GetFadeDlg() && (GetInterface().GetFadeDlg()->GetFadeMode() != CDnFadeInOutDlg::modeEnd))
				return;

			//rlkt_test pvp farm
			if (IsCmdControl("ID_BT_LADDER"))
			{
				if (CTaskManager::GetInstance().GetTask("VillageTask"))
				{
					GetInterface().MiddleMessageBox(9990100, MB_YESNO, 5001, CDnInterface::GetInstancePtr());
				}
				else {
					GetInterface().MiddleMessageBox(9990101);
				}
				return;
			}
			//

			if (IsCmdControl("ID_INVENTORY"))
			{
				pMainMenuDlg->ToggleShowDialog(CDnMainMenuDlg::INVENTORY_DIALOG);
				return;
			}

			if (IsCmdControl("ID_COMMUNITY"))
			{
				pMainMenuDlg->ToggleShowDialog(CDnMainMenuDlg::COMMUNITY_DIALOG);
				return;
			}

			if (IsCmdControl("ID_SKILL"))
			{
				pMainMenuDlg->ToggleShowDialog(CDnMainMenuDlg::SKILL_DIALOG);
				return;
			}

			if (IsCmdControl("ID_CHAR_STATUS"))
			{
				pMainMenuDlg->ToggleShowDialog(CDnMainMenuDlg::CHARSTATUS_DIALOG);
				return;
			}

			if (IsCmdControl("ID_QUEST"))
			{
				pMainMenuDlg->ToggleShowDialog(CDnMainMenuDlg::QUEST_DIALOG);
				return;
			}

			if (IsCmdControl("ID_BT_MISSION"))
			{
				pMainMenuDlg->ToggleShowDialog(CDnMainMenuDlg::MISSION_DIALOG);
				return;
			}

			if (IsCmdControl("ID_BT_DNGIRL"))
			{
				//GetInterface().GetLifeChannelDlg()->Show(true);			
				//GetInterface().MessageBox( L"We love you too!" );
				GetInterface().ShowGachaDialog(1);
				return;
			}
			//RLKT
			if (IsCmdControl("ID_STATIC_TIMEEVENT0") ||
				IsCmdControl("ID_STATIC_TIMEEVENT1") ||
				IsCmdControl("ID_STATIC_TIMEEVENT2") ||
				IsCmdControl("ID_STATIC_TIMEEVENT3") ||
				IsCmdControl("ID_STATIC_TIMEEVENT4") ||
				IsCmdControl("ID_STATIC_TIMEEVENT5"))
			{
				if (!CDnTimeEventTask::IsActive()) return;
				CDnTimeEventTask *pTask = &CDnTimeEventTask::GetInstance();

				pMainMenuDlg->ToggleShowDialog(CDnMainMenuDlg::INTEGRATE_EVENT_DIALOG);


				return;
			}
			if (IsCmdControl("ID_MENU"))
			{
				//if( !CDnTimeEventTask::IsActive() ) return;
				//CDnTimeEventTask *pTask = &CDnTimeEventTask::GetInstance();
				m_pMainBarMenuDlg->Show(true);
				//pMainMenuDlg->ToggleShowDialog( CDnMainMenuDlg::TIMEEVENT_DIALOG );


				return;
			}
#ifndef PRE_ADD_INTEGERATE_EVENTUI //else 
			if (IsCmdControl("ID_EVENT"))
			{
				if (!CDnTimeEventTask::IsActive()) return;
				CDnTimeEventTask *pTask = &CDnTimeEventTask::GetInstance();

				if (pTask && pTask->GetTimeEventCount() == 0)
					GetInterface().MessageBox(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 7736));
				else
					pMainMenuDlg->ToggleShowDialog(CDnMainMenuDlg::TIMEEVENT_DIALOG);

				return;
			}
			if (IsCmdControl("ID_CHECK"))
			{
				pMainMenuDlg->ToggleShowDialog(CDnMainMenuDlg::ATTENDANCEEVENT_DIALOG);
				m_bNotifyCheckAttendance = false;
				return;
			}
#endif // PRE_ADD_INTEGERATE_EVENTUI


#ifdef PRE_TEST_ANIMATION_UI
			if (IsCmdControl("ID_TESTANI"))
			{
				if (m_pAniTestDlg)
					m_pAniTestDlg->Show(!m_pAniTestDlg->IsShow());
				return;
			}
#endif

			if (IsCmdControl("ID_BT_MOVIE"))
			{
				CDnCommonTask* pCommonTask = static_cast<CDnCommonTask*>(CTaskManager::GetInstance().GetTask("CommonTask"));
				if (pCommonTask == NULL)
					return;

				float browserWidth = CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::DNTVBrowserWidth);
				float browserHeight = CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::DNTVBrowserHeight);

				std::string url(pCommonTask->GetMiniSiteURL());
				CDnInterface::GetInstance().OpenBrowser(url, browserWidth, browserHeight, CDnInterface::eBPT_CENTER, eGBT_GENERAL);
			}

#if defined(PRE_ADD_MAILBOX_OPEN)
			if (IsCmdControl("ID_MAIL"))
			{
				//메일함 열기..
				//마을에서만 가능..
				if (pMainMenuDlg == NULL ||
					CDnWorld::MapTypeVillage != CDnWorld::GetInstance().GetMapType())
					return;

				GetInterface().OpenMailDialogByShortCutKey();
			}
#endif // PRE_ADD_MAILBOX_OPEN



#ifdef PRE_SPECIALBOX
			if (IsCmdControl("ID_BT_EVENTALARM"))
			{
				if (CDnWorld::GetInstance().GetMapType() == CDnWorld::MapTypeVillage)
				{
					if (CDnEventReceiverTabDlg::CanRequestSpecialBoxInfo())
					{
						SendSpecialBoxList();
						CDnEventReceiverTabDlg::RequestDealy();
					}
					else
					{
						pMainMenuDlg->ToggleShowDialog(CDnMainMenuDlg::EVENT_RECEIVER_DIALOG);
					}
				}
				else
				{
					GetInterface().AddChatMessage(CHATTYPE_SYSTEM, L"", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 8226), false);
				}

				return;
			}
#endif

		} // Button End


	}

	CEtUIDialog::ProcessCommand(nCommand, bTriggeredByUser, pControl, uMsg);
}

//void CDnMainDlg::Render(float fElapsedTime)
//{
//	if( !IsShow() )
//		return;
//
//	DrawSprite( m_hBackBlack, SUICoord(0.0f, 0.0f, 1.0f, 1.0f), 0x7fffffff, m_pStaticBackBlack->GetUICoord() );
//	DrawSprite( m_hBackLight, SUICoord(0.0f, 0.0f, 1.0f, 1.0f), 0xffffffff, m_pStaticBackLight->GetUICoord() );
//
//	if( CDnMinimap::IsActive() )
//	{
//		SUICoord MinimapCoord, UVCoord;
//		MinimapCoord.SetPosition( (m_DlgInfo.DlgCoord.fWidth * 0.5f) - (78.0f/DEFAULT_UI_SCREEN_WIDTH), (6.0f/DEFAULT_UI_SCREEN_HEIGHT) );
//		MinimapCoord.SetSize( 156.0f/DEFAULT_UI_SCREEN_WIDTH, 156.0f/DEFAULT_UI_SCREEN_HEIGHT );
//		UVCoord.SetCoord( 0.0f, 0.0f, 1.0f, 1.0f );
//		//DrawSprite( CDnMinimap::GetInstance().GetTexture(), UVCoord, 0xdcffffff, MinimapCoord, -CalcCameraRotateValue() );
//		//DrawSprite( CDnMinimap::GetInstance().GetTexture(), UVCoord, 0xdcffffff, MinimapCoord );
//		DrawSprite( CDnMinimap::GetInstance().GetTexture(), UVCoord, 0xffffffff, MinimapCoord );
//	}
//
//	CEtUIDialog::Render( fElapsedTime );
//
//	if( CDnMinimap::IsActive() ) 
//	{
//		DrawMinimapOutInfo();
//	}
//
//	// Draw North Mark
//	SUICoord NorthCoord;
//	NorthCoord.SetPosition( (m_DlgInfo.DlgCoord.fWidth * 0.5f) - (11.0f/DEFAULT_UI_SCREEN_WIDTH), (-3.0f/DEFAULT_UI_SCREEN_HEIGHT) );
//	NorthCoord.SetSize( 22.0f/DEFAULT_UI_SCREEN_WIDTH, 22.0f/DEFAULT_UI_SCREEN_HEIGHT );
//	DrawSprite( m_hNorthTexture, SUICoord(0.0f, 0.0f, 1.0f, 1.0f), 0xffffffff, NorthCoord );
//
//	RenderEXP();
//	RenderFTG();
//	RenderWeekFTG();
//	RenderPCRoomFTG();
//	RenderEventFTG();
//#ifdef PRE_ADD_VIP
//	RenderVIPFTG();
//#endif
//	RenderFTGStatic();
//}
void CDnMainDlg::Render(float fElapsedTime)
{
	if (!IsShow())
		return;

	//? pt minimap
	//DrawSprite( m_hBackBlack, SUICoord(0.0f, 0.0f, 1.0f, 1.0f), 0x7fffffff, m_pStaticBackBlack->GetUICoord() );
	//DrawSprite( m_hBackLight, SUICoord(0.0f, 0.0f, 1.0f, 1.0f), 0xffffffff, m_pStaticBackLight->GetUICoord() );


	CEtUIDialog::Render(fElapsedTime);

	//
	m_pMainBar_MinimapDlg->Render(fElapsedTime);

	RenderEXP();
	RenderFTG();
	RenderWeekFTG();
	RenderPCRoomFTG();
	RenderEventFTG();
#ifdef PRE_ADD_VIP
	RenderVIPFTG();
#endif
	RenderFTGStatic();
}

bool CDnMainDlg::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (!IsShow())
	{
		return false;
	}

	bool bRet;
	bRet = CEtUIDialog::MsgProc(hWnd, uMsg, wParam, lParam);

	switch (uMsg)
	{
	case WM_LBUTTONDOWN:
	{
#if defined(PRE_ADD_68286)
		GetInterface().SetDisableChatTabMsgproc(true);
#endif // PRE_ADD_68286

		float fMouseX, fMouseY;
		POINT MousePoint;
		MousePoint.x = short(LOWORD(lParam));
		MousePoint.y = short(HIWORD(lParam));
		PointToFloat(MousePoint, fMouseX, fMouseY);
		//rlkt new ui movved to DNMainBar_MinimapDLG
		   if ( (GetControl<CEtUIButton>("ID_BUTTON_MAP")->IsInside( fMouseX, fMouseY) == false ) &&
		( GetControl<CEtUIStatic>("ID_MM_BACK")->IsInside( fMouseX, fMouseY) == false ) ) {
		if( m_bShowOption ) {
		ShowMinimapOption( false );
		}
		}
		#ifdef PRE_ADD_AUTO_DICE
		if ( (GetControl<CEtUIButton>("ID_BUTTON_DICE")->IsInside( fMouseX, fMouseY) == false ) &&
		( GetControl<CEtUIStatic>("ID_STATIC0")->IsInside( fMouseX, fMouseY) == false ) ) {
		if( m_bShowDiceOption ) {
		ShowAutoDiceOption( false );
		}
		}
		#endif
		
#if defined(PRE_ADD_68286)
		if (m_pStaticPost && m_pStaticPost->IsShow() && m_pStaticPost->IsInside(fMouseX, fMouseY) == true)
		{
			//메일함 열기..
			//마을에서만 가능..
			if (CDnWorld::MapTypeVillage == CDnWorld::GetInstance().GetMapType())
			{
#ifdef PRE_MOD_POTENTIAL_JEWEL_RENEWAL
				CDnPotentialJewelDlg* pPotentialJewelDlg = GetInterface().GetItemPotentialDlg();
				if (!pPotentialJewelDlg) return false;
				if (pPotentialJewelDlg->IsShow() == false)
				{
					GetInterface().OpenMailDialogByShortCutKey();

					//설정을 해 줘야 채팅창 활성화가 되지 않는다.
					GetInterface().SetDisableChatTabMsgproc(false);
					bRet = true;
				}
#else
				GetInterface().OpenMailDialogByShortCutKey();
				//설정을 해 줘야 채팅창 활성화가 되지 않는다.
				GetInterface().SetDisableChatTabMsgproc(false);
				bRet = true;
#endif // PRE_MOD_POTENTIAL_JEWEL_RENEWAL
			}
		}

		if (m_pStaticCashShopGift && m_pStaticCashShopGift->IsShow() && m_pStaticCashShopGift->IsInside(fMouseX, fMouseY) == true)
		{
			//마을에서만 가능..
			if (CDnWorld::MapTypeVillage == CDnWorld::GetInstance().GetMapType())
			{
#ifdef PRE_MOD_POTENTIAL_JEWEL_RENEWAL		
				CDnPotentialJewelDlg* pPotentialJewelDlg = GetInterface().GetItemPotentialDlg();
				if (!pPotentialJewelDlg) return false;
				if (pPotentialJewelDlg->IsShow() == false)
				{
					GetInterface().OpenCashShopRecvGiftWindow();
					//GetCashShopTask().RequestCashShopRecvGiftBasicInfoByShortCut();
					//설정을 해 줘야 채팅창 활성화가 되지 않는다.
					GetInterface().SetDisableChatTabMsgproc(false);
					bRet = true;
				}
#else
				GetInterface().OpenCashShopRecvGiftWindow();
				//GetCashShopTask().RequestCashShopRecvGiftBasicInfoByShortCut();
				//설정을 해 줘야 채팅창 활성화가 되지 않는다.
				GetInterface().SetDisableChatTabMsgproc(false);
				bRet = true;
#endif

			}
		}
#endif // PRE_ADD_68286
#ifdef PRE_ADD_REMOTE_QUEST
		if (m_pStaticQuestBallon && m_pStaticQuestBallon->IsShow() && m_pStaticQuestBallon->IsInside(fMouseX, fMouseY))
		{
#ifdef PRE_MOD_POTENTIAL_JEWEL_RENEWAL
			CDnPotentialJewelDlg* pPotentialJewelDlg = GetInterface().GetItemPotentialDlg();
			if (!pPotentialJewelDlg) return false;
			if (pPotentialJewelDlg->IsShow() == false)
			{
				CDnMainMenuDlg *pMainMenuDlg = GetInterface().GetMainMenuDialog();
				if (pMainMenuDlg && !pMainMenuDlg->IsOpenQuestDlg())
					pMainMenuDlg->ToggleShowDialog(CDnMainMenuDlg::QUEST_DIALOG);
			}
#else  // PRE_MOD_POTENTIAL_JEWEL_RENEWAL
			CDnMainMenuDlg *pMainMenuDlg = GetInterface().GetMainMenuDialog();
			if (pMainMenuDlg && !pMainMenuDlg->IsOpenQuestDlg())
				pMainMenuDlg->ToggleShowDialog(CDnMainMenuDlg::QUEST_DIALOG);
#endif // PRE_MOD_POTENTIAL_JEWEL_RENEWAL
		}
#endif // PRE_ADD_REMOTE_QUEST

#ifdef PRE_ADD_LEVELUP_REWARD_NOTIFIER
		if (CDnWorld::MapTypeVillage == CDnWorld::GetInstance().GetMapType())
		{
			if (m_pStaticLevelUpNotifier && m_pStaticLevelUpNotifier->IsShow() && m_pStaticLevelUpNotifier->IsInside(fMouseX, fMouseY))
			{
				GetInterface().GetMainMenuDialog()->ToggleShowDialog(CDnMainMenuDlg::INVENTORY_DIALOG);
				CDnInvenTabDlg* pInvenTabDlg = (CDnInvenTabDlg*)GetInterface().GetMainMenuDialog(CDnMainMenuDlg::INVENTORY_DIALOG);
				if (pInvenTabDlg)
				{
					CDnItem* pItem = CDnItemTask::GetInstance().GetExistLevelUpBox(CDnActor::s_hLocalActor->GetLevel());
					if (pItem)
					{
						pInvenTabDlg->GetCashInvenDlg()->RefreshInvenPageFromSlotIndex(pItem->GetSlotIndex());
						pInvenTabDlg->GetCashInvenDlg()->SetFocusBlink(pItem->GetSlotIndex(), 500, 2400);
						pInvenTabDlg->SetCheckedTab(ST_INVENTORY_CASH - 1);
					}
				}
			}
		}
#endif
	}
	break;
	case WM_MOUSEMOVE:
	{
		float fMouseX, fMouseY;
		POINT MousePoint;
		MousePoint.x = short(LOWORD(lParam));
		MousePoint.y = short(HIWORD(lParam));
		PointToFloat(MousePoint, fMouseX, fMouseY);

		bool bExpMouseEnter(false);

		for (int i = 0; i<5; i++)
		{
			if (m_pExpGauge[i]->IsMouseEnter())
			{
				bExpMouseEnter = true;
				break;
			}
		}

		if (!CDnMouseCursor::GetInstance().IsShowCursor()) bExpMouseEnter = false;
		ShowExpDialog(bExpMouseEnter, fMouseX, fMouseY);
		ShowMailNotifyTooltip(fMouseX, fMouseY);
		ShowHarvestNotifyTooltip(fMouseX, fMouseY);
#ifdef PRE_ADD_LEVELUP_REWARD_NOTIFIER
		ShowLevelUpBoxNotifierToolTip(fMouseX, fMouseY);
#endif 

		if (m_bCheckAttendanceFirst)
			ShowCheckAttendanceTooltip(fMouseX, fMouseY);

		bool bFTGMouseEnter(false);
		SUICoord uiCoord;
		m_pEventFTGGauge->GetUICoord(uiCoord);
		if (uiCoord.IsInside(fMouseX, fMouseY) && CDnMouseCursor::GetInstance().IsShowCursor())
			bFTGMouseEnter = true;

		ShowFTGDialog(bFTGMouseEnter, fMouseX, fMouseY);

#ifdef PRE_REMOVE_MINISITE
#else
		if (m_pNewMiniSiteBtn && m_pNewMiniSiteBtn->IsInside(fMouseX, fMouseY))
		{
			CDnCommonTask* pCommonTask = static_cast<CDnCommonTask*>(CTaskManager::GetInstance().GetTask("CommonTask"));
			if (pCommonTask != NULL)
			{
				std::wstring tooltipText;
				pCommonTask->GetMiniSiteTooltip(tooltipText);
				m_pNewMiniSiteBtn->SetTooltipText(tooltipText.c_str());
			}
		}
#endif
	}
	break;
	}

	return bRet;
}

float CDnMainDlg::CalcCameraRotateValue()
{
	EtVector3 vZDir, vCross;
	float fDot, fAngle;

	//vZDir = CDnCamera::GetActiveCamera()->GetMatEx()->m_vZAxis;
	vZDir.y = 0.0f;
	vZDir.x = 0.0f;
	vZDir.z = 1.0f;
	EtVec3Normalize(&vZDir, &vZDir);
	fDot = EtVec3Dot(&vZDir, &EtVector3(0.0f, 0.0f, 1.0f));
	m_fDegree = EtAcos(fDot);
	EtVec3Cross(&vCross, &EtVector3(0.0f, 0.0f, 1.0f), &vZDir);
	fAngle = EtToDegree(m_fDegree);

	if (vCross.y > 0.0f)
	{
		m_fDegree = -m_fDegree;
		return fAngle;
	}
	else
	{
		return -fAngle;
	}
}

bool CDnMainDlg::InitQuickSlot(int nQuickSlotIndex, MIInventoryItem *pItem)
{
	int nTabIndex = nQuickSlotIndex / QUICKSLOT_BUTTON_MAX;
	int nSlotIndex = nQuickSlotIndex % QUICKSLOT_BUTTON_MAX;

	ASSERT((nTabIndex >= QUICKSLOT_DLG_00) && (nTabIndex<QUICKSLOT_DLG_MAX));
	ASSERT((nSlotIndex >= 0) && (nSlotIndex<QUICKSLOT_BUTTON_MAX));

	if (!(nSlotIndex >= 0 && nSlotIndex < QUICKSLOT_BUTTON_MAX)) return false;
	if (!(nTabIndex >= QUICKSLOT_DLG_00 && nTabIndex < QUICKSLOT_DLG_MAX)) return false;

	CDnQuickSlotDlg *pCurQuickSlotDlg = static_cast<CDnQuickSlotDlg*>(m_QuickSlgDlgGroup.GetDialog(nTabIndex));
	if (!pCurQuickSlotDlg) return false;
	return pCurQuickSlotDlg->InitSlot(nSlotIndex, pItem);
}

void CDnMainDlg::SwapEventSlot(bool bTrue)
{
	if (bTrue)
	{
		m_dwOrignalShowQuickSlotDlgID = m_QuickSlgDlgGroup.GetShowDialogID();
		m_dwOrignalPrevQuickSlotDlgID = m_dwPrevQuickDlgID;

		m_dwShowQuickDlgID = QUICKSLOT_EVENT;
		m_QuickSlgDlgGroup.ShowDialog(QUICKSLOT_EVENT, true);
		m_pStaticSlotNumber->SetText(L"E");
		ShowExpDialog(false);
		ShowFTGDialog(false);

		CDnQuickSlotDlg *pCurQuickSlotDlg = static_cast<CDnQuickSlotDlg*>(m_QuickSlgDlgGroup.GetDialog(QUICKSLOT_EVENT));
		if (pCurQuickSlotDlg) pCurQuickSlotDlg->ResetAllSlot();

	}
	else
	{
		m_QuickSlgDlgGroup.ShowDialog(QUICKSLOT_EVENT, false);
		m_QuickSlgDlgGroup.ShowDialog(m_dwOrignalShowQuickSlotDlgID, true);
		m_dwPrevQuickDlgID = m_dwOrignalPrevQuickSlotDlgID;
		m_dwShowQuickDlgID = m_dwOrignalShowQuickSlotDlgID;

		m_pStaticSlotNumber->SetIntToText((m_dwOrignalShowQuickSlotDlgID - (m_nSkillSlotPage * QUICKSLOT_PAGE_COUNT)) + 1);
		ShowExpDialog(false);
		ShowFTGDialog(false);
	}
}

void CDnMainDlg::ChangeQuickSlotUp()
{
	if (m_bLockQuickSlot)
		return;

	int nCurDlgID = (int)m_QuickSlgDlgGroup.GetShowDialogID();

	if (!IsCurrentSkillPageIndex(++nCurDlgID))
		nCurDlgID = m_nSkillSlotPage * QUICKSLOT_PAGE_COUNT;

	m_dwPrevQuickDlgID = m_QuickSlgDlgGroup.GetShowDialogID();
	m_QuickSlgDlgGroup.ShowDialog((DWORD)nCurDlgID, true);

	m_pStaticSlotNumber->SetIntToText((nCurDlgID - (m_nSkillSlotPage * QUICKSLOT_PAGE_COUNT)) + 1);

	ShowExpDialog(false);
	ShowFTGDialog(false);
}

void CDnMainDlg::ChangeQuickSlotDown()
{
	if (m_bLockQuickSlot)
		return;

	int nCurDlgID = (int)m_QuickSlgDlgGroup.GetShowDialogID();
	if (!IsCurrentSkillPageIndex(--nCurDlgID))
		nCurDlgID = ((QUICKSLOT_SKILL_PAGE_COUNT * m_nSkillSlotPage) + QUICKSLOT_PAGE_COUNT) - 1;

	m_dwPrevQuickDlgID = m_QuickSlgDlgGroup.GetShowDialogID();
	m_QuickSlgDlgGroup.ShowDialog((DWORD)nCurDlgID, true);

	m_pStaticSlotNumber->SetIntToText((nCurDlgID - (m_nSkillSlotPage * QUICKSLOT_PAGE_COUNT)) + 1);

	ShowExpDialog(false);
	ShowFTGDialog(false);
}

void CDnMainDlg::ChangeQuickSlotButton(CDnSlotButton *pDragButton, CDnSlotButton *pPressedButton)
{
	if (!pDragButton || !pPressedButton)
		return;

	MIInventoryItem *pItem1 = pDragButton->GetItem();
	MIInventoryItem *pItem2 = pPressedButton->GetItem();


	if (!pItem1)
		return;

	int nTabIndex = ((CDnQuickSlotButton*)pPressedButton)->GetItemSlotIndex() / QUICKSLOT_BUTTON_MAX;
	int nSlotIndex = ((CDnQuickSlotButton*)pPressedButton)->GetItemSlotIndex() % QUICKSLOT_BUTTON_MAX;

	CDnQuickSlotDlg *pCurQuickSlotDlg = static_cast<CDnQuickSlotDlg*>(m_QuickSlgDlgGroup.GetDialog(nTabIndex));
	if (pCurQuickSlotDlg) pCurQuickSlotDlg->SetSlot(nSlotIndex, pItem1);

	if (pItem2)
	{
		nTabIndex = ((CDnQuickSlotButton*)pDragButton)->GetItemSlotIndex() / QUICKSLOT_BUTTON_MAX;
		nSlotIndex = ((CDnQuickSlotButton*)pDragButton)->GetItemSlotIndex() % QUICKSLOT_BUTTON_MAX;

		CDnQuickSlotDlg *pCurQuickSlotDlg = static_cast<CDnQuickSlotDlg*>(m_QuickSlgDlgGroup.GetDialog(nTabIndex));
		if (pCurQuickSlotDlg) pCurQuickSlotDlg->SetSlot(nSlotIndex, pItem2);
	}

	pDragButton->DisableSplitMode(true);
	pPressedButton->DisableSplitMode(true);

	drag::ReleaseControl();
}

bool CDnMainDlg::InitLifeSkillQuickSlot(int nSlotIndex, int nLifeSkillID, MIInventoryItem *pItem, MIInventoryItem::InvenItemTypeEnum ItemType)
{
	CDnLifeSkillQuickSlotDlg *pQuickSlotDlg = static_cast<CDnLifeSkillQuickSlotDlg*>(m_QuickSlgDlgGroup.GetDialog(LIFESKILL_DLG));
	return pQuickSlotDlg->InitSlot(nSlotIndex, nLifeSkillID, pItem, ItemType);
}

void CDnMainDlg::ResetLifeSkillQuickSlot(int nSlotIndex)
{
	CDnLifeSkillQuickSlotDlg *pQuickSlotDlg = static_cast<CDnLifeSkillQuickSlotDlg*>(m_QuickSlgDlgGroup.GetDialog(LIFESKILL_DLG));
	return pQuickSlotDlg->ResetSlot(nSlotIndex, false);
}

bool CDnMainDlg::ResetSecondarySkillQuickSlot(int nSecondarySkillID)
{
	CDnLifeSkillQuickSlotDlg *pQuickSlotDlg = static_cast<CDnLifeSkillQuickSlotDlg*>(m_QuickSlgDlgGroup.GetDialog(LIFESKILL_DLG));
	return pQuickSlotDlg->ResetSecondarySkillQuickSlot(nSecondarySkillID);
}

#ifdef PRE_SWAP_QUICKSLOT
void CDnMainDlg::SwapQuickSlot()
{
	// 현재는 두칸밖에 없으므로, 이 함수면 끝이다.
	ChangeQuickSlotUp();
}
#endif

int CDnMainDlg::GetEmptyQuickSlotIndex()
{
	int nSlotIndex = -1;
	CEtUIDialogGroup::DIALOG_VEC &vecQuickSlotDialog = m_QuickSlgDlgGroup.GetDialogList();
	for (int i = 0; i<(int)vecQuickSlotDialog.size() - 1; ++i)	// 마지막 퀵슬롯은 제스처 퀵슬롯이라 제외시킨다.
	{
		CEtUIDialogGroup::ID_DIALOG_PAIR &id_dlg_pair = vecQuickSlotDialog[i];
		if (!IsCurrentSkillPageIndex(id_dlg_pair.first))
			continue;

		nSlotIndex = static_cast<CDnQuickSlotDlg*>(id_dlg_pair.second)->FindEmptyQuickSlotIndex();
		if (nSlotIndex != -1) return nSlotIndex;
	}
	return -1;
}

int CDnMainDlg::GetSkillQuickSlotIndex(int nSkillID)
{
	int nSlotIndex = -1;
	CEtUIDialogGroup::DIALOG_VEC &vecQuickSlotDialog = m_QuickSlgDlgGroup.GetDialogList();
	for (int i = 0; i<(int)vecQuickSlotDialog.size() - 1; ++i)	// 마지막 퀵슬롯은 제스처 퀵슬롯이라 제외시킨다.
	{
		CEtUIDialogGroup::ID_DIALOG_PAIR &id_dlg_pair = vecQuickSlotDialog[i];
		if (!IsCurrentSkillPageIndex(id_dlg_pair.first))
			continue;

		nSlotIndex = static_cast<CDnQuickSlotDlg*>(id_dlg_pair.second)->FindSkillQuickSlotIndex(nSkillID);
		if (nSlotIndex != -1) return nSlotIndex;
	}
	return -1;
}

void CDnMainDlg::EnableQuickSlot(bool bEnable)
{
	CEtUIDialogGroup::DIALOG_VEC &vecQuickSlotDialog = m_QuickSlgDlgGroup.GetDialogList();
	for (int i = 0; i<(int)vecQuickSlotDialog.size(); ++i)
	{
		CEtUIDialogGroup::ID_DIALOG_PAIR &id_dlg_pair = vecQuickSlotDialog[i];
		CEtUIDialog* pDlg = id_dlg_pair.second;
		if (pDlg)
		{
			if (pDlg->GetDialogID() == LIFESKILL_DLG)
			{
				CDnLifeSkillQuickSlotDlg* pGestureDlg = static_cast<CDnLifeSkillQuickSlotDlg*>(pDlg);
				if (pGestureDlg)
					pGestureDlg->EnableQuickSlot(bEnable);
			}
			else
			{
				CDnQuickSlotDlg* pQuickSlotDlg = static_cast<CDnQuickSlotDlg*>(pDlg);
				if (pQuickSlotDlg)
					pQuickSlotDlg->EnableQuickSlot(bEnable);
			}
		}
	}
}

void CDnMainDlg::DrawMinimapOutInfo()
{
	std::vector<CDnMinimap::SOutInfo> &vecOutInfo = GetMiniMap().GetMiniMapOutInfo();
	if (vecOutInfo.empty() == false)
	{
		for (DWORD i = 0; i < vecOutInfo.size(); ++i)
		{
			CEtUIStatic *pIcon = NULL;
			switch (vecOutInfo[i].eIconType)
			{

			case CDnMinimap::emICON_INDEX::indexTraceQuest:
				pIcon = GetControl<CEtUIStatic>("ID_ICON_QUEST");
				break;

			case CDnMinimap::emICON_INDEX::indexAllyCaptain:
				pIcon = GetControl<CEtUIStatic>("ID_MAP_PARTY");
				break;

			case CDnMinimap::emICON_INDEX::indexEnemyCaptain:
				pIcon = GetControl<CEtUIStatic>("ID_MAP_ENEMY");
				break;
			}

			if (pIcon)
				DrawIcon(pIcon, vecOutInfo[i].fDegree, 72.f, 0.0025f);
		}
	}
}


void CDnMainDlg::DrawIcon(CEtUIControl *pControl, float fAngle, float fRadius, float fValue)
{
	SUIElement *pElement = pControl->GetElement(0);
	if (pElement)
	{
		SUICoord uiCoord;
		pControl->GetUICoord(uiCoord);

		float fCenX, fCenY;
		fCenX = (m_DlgInfo.DlgCoord.fWidth) * 0.5f;
		fCenY = 78.0f / DEFAULT_UI_SCREEN_HEIGHT;

		float fModifiedAngle = fAngle - 1.5707964f;

		EtVector2 vTemp;
		vTemp.x = cos(fModifiedAngle) * (fRadius / DEFAULT_UI_SCREEN_WIDTH);

		if (fModifiedAngle > RIGHT_DEGREE_MAX || fModifiedAngle < LEFT_DEGREE_MAX)
		{
			uiCoord.fY = m_DlgInfo.DlgCoord.fHeight - uiCoord.fHeight + fValue - (20.0f / DEFAULT_UI_SCREEN_HEIGHT);
		}
		else
		{
			vTemp.y = sin(fModifiedAngle) * (fRadius / DEFAULT_UI_SCREEN_HEIGHT);
			uiCoord.fY = vTemp.y + (fCenY - (uiCoord.fHeight*0.5f));
		}

		uiCoord.fX = vTemp.x + (fCenX - (uiCoord.fWidth*0.5f));
		uiCoord.SetSize(pElement->fTextureWidth * 1.25f, pElement->fTextureHeight * 1.25f);

		DrawSprite(pControl->GetTemplate().m_hTemplateTexture, pElement->TemplateUVCoord, 0xffffffff, uiCoord, EtToDegree(fAngle));
	}
}

void CDnMainDlg::ShowExpDialog(bool bShow, float fX, float fY)
{
	if (bShow)
	{
		if (CDnActor::s_hLocalActor)
		{
			CDnPlayerActor *pActor = (CDnPlayerActor *)CDnActor::s_hLocalActor.GetPointer();
			int nNextLevExp = pActor->GetNextLevelExperience();
			int nCurLevExp = pActor->GetCurrentLevelExperience();
			int nCurExp = pActor->GetExperience();

			if (nCurExp > nNextLevExp)
			{
				nCurExp = nNextLevExp;
			}

			nNextLevExp -= nCurLevExp;
			nCurExp -= nCurLevExp;

			m_pExpDlg->SetExp(nCurExp, nNextLevExp);
			ShowChildDialog(m_pExpDlg, true);

			SUICoord dlgCoord;
			m_pExpDlg->GetDlgCoord(dlgCoord);
			dlgCoord.fX = fX + GetXCoord() - (dlgCoord.fWidth / 2.0f);
			dlgCoord.fY = fY + GetYCoord() - dlgCoord.fHeight - 0.004f;
			m_pExpDlg->SetDlgCoord(dlgCoord);
		}
	}
	else
	{
		ShowChildDialog(m_pExpDlg, false);
	}
}

void CDnMainDlg::ShowMailNotifyTooltip(float fX, float fY)
{
#ifdef PRE_ADD_MAIL_UI_IMPROVEMENTS

	if (GetTradeTask().GetTradeMail().GetUnreadMailCount() <= 0)	// 메일을 다 읽었을 경우 툴팁을 출력할 필요가없다. 
		return;

	SButtonInfo* pMailInfo = GetMenuBotton(CDnMainMenuDlg::MAIL_DIALOG);

	if (pMailInfo && pMailInfo->pButton != NULL && pMailInfo->pButton->IsShow() != false)
	{
		if (pMailInfo->pButton->IsInside(fX, fY))
		{
			std::wstring str;
			if (m_bMailAlarm)
				str = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 1979);	// UISTRING : 새 메일이 도착했습니다.
			else
				str = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 1980);	// UISTRING : 읽지 않은 메일이 있습니다.

			ShowTooltipDlg(pMailInfo->pButton, true, str, 0xffffffff, true);
		}
		else
		{
			if (IsTooltipControl(pMailInfo->pButton))
				ShowTooltipDlg(pMailInfo->pButton, false);
		}
	}

#else
	if (m_pStaticPost == NULL || m_pStaticPost->IsShow() == false)
		return;

	if (m_pStaticPost->IsInside(fX, fY))
	{
		std::wstring str;
		if (m_bMailAlarm)
			str = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 1979);	// UISTRING : 새 메일이 도착했습니다.
		else
			str = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 1980);	// UISTRING : 읽지 않은 메일이 있습니다.

		ShowTooltipDlg(m_pStaticPost, true, str, 0xffffffff, true);
	}
	else
	{
		if (IsTooltipControl(m_pStaticPost))
			ShowTooltipDlg(m_pStaticPost, false);
	}
#endif 
}

void CDnMainDlg::ShowHarvestNotifyTooltip(float fX, float fY)
{
	if (m_pStaticHarvest == NULL)
		return;

	if (m_pStaticHarvest->IsShow() == false)
		return;

	if (m_pStaticHarvest->IsInside(fX, fY))
	{
		std::wstring str = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 7461);	// UISTRING : 농장창고에 회수하지 않은 물품이 있습니다. 농장창고를 확인해 보세요.

		ShowTooltipDlg(m_pStaticHarvest, true, str, 0xffffffff, true);
	}
	else
	{
		if (IsTooltipControl(m_pStaticHarvest))
			ShowTooltipDlg(m_pStaticHarvest, false);
	}
}

#ifdef PRE_ADD_LEVELUP_REWARD_NOTIFIER
void CDnMainDlg::ShowLevelUpBoxNotifierToolTip(float fx, float fy)
{
	if (m_pStaticLevelUpNotifier == NULL)
		return;

	if (!m_pStaticLevelUpNotifier->IsShow())
		return;

	if (m_pStaticLevelUpNotifier->IsInside(fx, fy))
	{
		std::wstring str = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 7766);	// UISTRING : 열 수 있는 레벨업 보상 상자를 가지고 있습니다.

		ShowTooltipDlg(m_pStaticLevelUpNotifier, true, str, 0xffffffff, true);
	}
	else
	{
		if (IsTooltipControl(m_pStaticLevelUpNotifier))
			ShowTooltipDlg(m_pStaticLevelUpNotifier, false);
	}
}
#endif 

void CDnMainDlg::ShowCheckButton(bool bShow)
{
	for (int i = 0; i<(int)m_vecButtonInfo.size(); i++)
	{
		if (m_vecButtonInfo[i].nDialogID == CDnMainMenuDlg::ATTENDANCEEVENT_DIALOG)
		{
			m_vecButtonInfo[i].pButton->Show(bShow);
			break;
		}
	}
}

void CDnMainDlg::ShowCheckAttendanceTooltip(float fX, float fY)
{
	CDnMenuButton* pCheckButton = GetControl<CDnMenuButton>("ID_CHECK");
	if (pCheckButton && pCheckButton->IsInside(fX, fY))
	{
		m_pStaticCheckAttendanceBalloon->Show(true);
		m_pStaticCheckAttendanceText->Show(true);
		m_bNotifyCheckAttendance = false;
	}
	else if (!m_bNotifyCheckAttendance)
	{
		m_pStaticCheckAttendanceBalloon->Show(false);
		m_pStaticCheckAttendanceText->Show(false);
	}
}

void CDnMainDlg::SetCheckAttendanceFirst(bool bCheckAttendanceFirst)
{
	m_bCheckAttendanceFirst = bCheckAttendanceFirst;
	CDnMenuButton* pCheckButton = GetControl<CDnMenuButton>("ID_CHECK");
	if (pCheckButton)
	{
		if (m_bCheckAttendanceFirst)
		{
			pCheckButton->SetBlink();
			m_fCheckAttendanceAlarmTime = 0.0f;
			m_bNotifyCheckAttendance = true;
			m_pStaticCheckAttendanceBalloon->Show(true);
			m_pStaticCheckAttendanceText->Show(true);

			// 출석당일 체크이면 시스템 메세지 출력
			CDnCommonTask* pCommonTask = static_cast<CDnCommonTask*>(CTaskManager::GetInstance().GetTask("CommonTask"));
			if (pCommonTask == NULL)
				return;

			std::map<CommonVariable::Type::eCode, CommonVariable::Data>::iterator iter = pCommonTask->GetVariableData().find(CommonVariable::Type::AttendanceEvent);
			if (iter == pCommonTask->GetVariableData().end())
				return;

			int nOngoingDay = static_cast<int>(iter->second.biValue >> 32);

			DNTableFileFormat*  pSox = GetDNTable(CDnTableDB::TATTENDANCEEVENT);
			if (pSox == NULL)
				return;

			for (int i = 0; i<pSox->GetItemCount(); i++)
			{
				int nTableID = pSox->GetItemID(i);

				bool bEnable = pSox->GetFieldFromLablePtr(nTableID, "_ON")->GetInteger() == 1 ? true : false;
				if (!bEnable)
					continue;

				int nAttendanceDate = pSox->GetFieldFromLablePtr(nTableID, "_Date_Count")->GetInteger();
				if (nAttendanceDate == nOngoingDay)	// 연속 출석일 당일 출석일 경우 체크
				{
					GetInterface().AddChatMessage(CHATTYPE_SYSTEM, L"", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 7735), false);
					break;
				}
			}
		}
		else
		{
			pCheckButton->ResetBlink();
			if (m_pStaticCheckAttendanceBalloon->IsShow())
				m_pStaticCheckAttendanceBalloon->Show(false);
			if (m_pStaticCheckAttendanceText->IsShow())
				m_pStaticCheckAttendanceText->Show(false);
			if (m_bNotifyCheckAttendance)
				m_bNotifyCheckAttendance = false;
		}
	}
}

void CDnMainDlg::UpdateAttendanceNotify(float fElapsedTime)
{
	if (m_bNotifyCheckAttendance)
	{
		if (m_fCheckAttendanceAlarmTime >= 10.f)
		{
			CDnMenuButton* pCheckButton = GetControl<CDnMenuButton>("ID_CHECK");
			if (pCheckButton)
				pCheckButton->ResetBlink();
			if (m_pStaticCheckAttendanceBalloon->IsShow())
				m_pStaticCheckAttendanceBalloon->Show(false);
			if (m_pStaticCheckAttendanceText->IsShow())
				m_pStaticCheckAttendanceText->Show(false);

			m_bNotifyCheckAttendance = false;
		}

		if (m_fCheckAttendanceAlarmTime != 100.0f)
			m_fCheckAttendanceAlarmTime += fElapsedTime;
	}
}

#ifdef PRE_MOD_POTENTIAL_JEWEL_RENEWAL
void CDnMainDlg::ProcessPotentialJewelButton()
{
	// 잠재력 부여 윈도우가 열렸을땐, 모든 버튼을 막는다.
	CDnPotentialJewelDlg* pPotentialJewelDlg = GetInterface().GetItemPotentialDlg();
	if (!pPotentialJewelDlg)
		return;

	bool bIsShow = pPotentialJewelDlg->IsShow();
	if (bIsShow)
		ToggleEnableAllButton(false);

	// 캐시샵 바로가기 버튼 막기.
	GetInterface().DisableCashShopMenuDlg(bIsShow);
}
#endif // PRE_MOD_POTENTIAL_JEWEL_RENEWAL

void CDnMainDlg::Process(float fElapsedTime)
{
	CEtUIDialog::Process(fElapsedTime);
	UpdateMailNotify(fElapsedTime);
	UpdateCashShopGiftNotify(fElapsedTime);
	UpdateHarvestNotify(fElapsedTime);
#ifdef PRE_ADD_LEVELUP_REWARD_NOTIFIER
	UpdateLevelUpBoxNotifier(fElapsedTime);
#endif 
#ifdef PRE_ADD_LEVELUP_GUIDE
	UpdateNewSkillNotify(fElapsedTime);
#endif
	UpdateMiniSiteNotify(fElapsedTime);
#ifdef PRE_SPECIALBOX
	UpdateSpecialBoxNotify(fElapsedTime);
#endif
#if defined( PRE_ALTEIAWORLD_EXPLORE )
	m_pMainBar_EventDlg->UpdateAlteaDice(fElapsedTime);
#endif	// #if defined( PRE_ALTEIAWORLD_EXPLORE )

	if (IsShow())
	{
		// Note : ID_MAP
		ProcessCommunityButton();
		ProcessSystemMenuButton();
		ProcessQuickSlot();
		ProcessZoneMapButton();
		ProcessButton(CDnMainMenuDlg::QUEST_DIALOG);
		ProcessButton(CDnMainMenuDlg::SKILL_DIALOG);
		ProcessButton(CDnMainMenuDlg::MISSION_DIALOG);
		ProcessButton(CDnMainMenuDlg::CHARSTATUS_DIALOG);
		ProcessButton(CDnMainMenuDlg::INVENTORY_DIALOG);
		ProcessChatRoomButton();

#ifdef PRE_ADD_NPC_REPUTATION_SYSTEM
#if !defined(PRE_ADD_NO_REPUTATION_DLG)
#ifdef PRE_ADD_INTEGERATE_QUEST_REPUT
		ProcessButton(CDnMainMenuDlg::InTEGRATE_QUESTNREPUTATION_DIALOG);
#else  // PRE_ADD_INTEGERATE_QUEST_REPUT
		ProcessButton(CDnMainMenuDlg::NPC_REPUTATION_TAB_DIALOG);
#endif // PRE_ADD_INTEGERATE_QUEST_REPUT
#endif	// PRE_ADD_NO_REPUTATION_DLG
#endif	// PRE_ADD_NPC_REPUTATION_SYSTEM

#ifdef PRE_ADD_INTEGERATE_EVENTUI
		ProcessButton(CDnMainMenuDlg::INTEGRATE_EVENT_DIALOG);
#endif // PRE_ADD_INTEGERATE_EVENTUI

		ProcessPVPModeButton();

#ifdef PRE_ADD_PVP_VILLAGE_ACCESS
		ProcessPVPVillageAccessDlg();
#endif // PRE_ADD_PVP_VILLAGE_ACCESS

		ProcessTimeEventAlarm(fElapsedTime);

#if defined(PRE_ADD_MAILBOX_OPEN)
		ProcessButton(CDnMainMenuDlg::MAIL_DIALOG);
#endif // PRE_ADD_MAILBOX_OPEN

#ifdef PRE_ADD_PVP_RANKING
		ProcessPvPRankButton();
#endif // PRE_ADD_PVP_RANKING

#ifdef PRE_MOD_POTENTIAL_JEWEL_RENEWAL
		ProcessPotentialJewelButton();
#endif // PRE_MOD_POTENTIAL_JEWEL_RENEWAL

		//rlkt_disabled
		/*	if( m_bCheckAttendanceFirst )
		{
		UpdateAttendanceNotify( fElapsedTime );

		CDnMenuButton* pBtn = GetControl<CDnMenuButton>("ID_CHECK");
		if (pBtn && pBtn->IsShow() == false)
		{
		CDnCommonTask* pCommonTask = static_cast<CDnCommonTask*>( CTaskManager::GetInstance().GetTask( "CommonTask" ) );
		if (pCommonTask && pCommonTask->HasVariableData(CommonVariable::Type::AttendanceEvent))
		pBtn->Show(true);
		}
		}

		if( GetControl<CDnMenuButton>( "ID_CHECK" )->IsShow() )
		{
		CDnCommonTask* pCommonTask = static_cast<CDnCommonTask*>( CTaskManager::GetInstance().GetTask( "CommonTask" ) );
		if( pCommonTask && m_bCheckAttendanceFirst != pCommonTask->GetCheckAttendanceFirst() )
		SetCheckAttendanceFirst( pCommonTask->GetCheckAttendanceFirst() );
		}
		*/
#ifdef PRE_ADD_STAMPSYSTEM
		if (m_bSearching && m_SearchedNpcID != -1)
		{
			RotetaNPCArrow();
		}
#endif // PRE_ADD_STAMPSYSTEM

#ifdef PRE_ADD_DWC
		// DWC캐릭은 캐시샵 버튼을 막는다
		GetInterface().DisableCashShopMenuDlg(GetDWCTask().IsDWCChar());
#ifdef PRE_ALTEIAWORLD_EXPLORE
		// DWC캐릭은 알테이아 주사위를 막는다
		if (GetDWCTask().IsDWCChar())
			m_pAlteaDiceIcon->Enable(false);
#endif // PRE_ALTEIAWORLD_EXPLORE
#endif // PRE_ADD_DWC
	}
}

//void CDnMainDlg::UpdateExpUp( float fElapsedTime )
//{
//	if( m_bExpUp )
//	{
//		SUICoord uiCoord;
//		m_pExpUp->GetUICoord( uiCoord );
//		uiCoord.fY -= 0.001f;
//		uiCoord.fHeight += 0.001f;
//		m_pExpUp->SetUICoord( uiCoord );
//
//		if( uiCoord.fHeight >= m_ExpCoord.fHeight )
//		{
//			m_bExpUp = false;
//			m_fExpUpTime = 1.0f;
//		}
//	}
//	else
//	{
//		if( m_fExpUpTime <= 0.0f )
//		{
//			m_pExpUp->Show(false);
//			m_fExpUpTime = 0.0f;
//		}
//		else
//		{
//			m_fExpUpTime -= fElapsedTime;
//		}
//	}
//}
//
//void CDnMainDlg::UpdateMoneyUp( float fElapsedTime )
//{
//	if( m_bMoneyUp )
//	{
//		SUICoord uiCoord;
//		m_pMoneyUp->GetUICoord( uiCoord );
//		uiCoord.fY -= 0.001f;
//		uiCoord.fHeight += 0.001f;
//		m_pMoneyUp->SetUICoord( uiCoord );
//
//		if( uiCoord.fHeight >= m_ExpCoord.fHeight )
//		{
//			m_bMoneyUp = false;
//			m_fMoneyUpTime = 1.0f;
//		}
//	}
//	else
//	{
//		if( m_fMoneyUpTime <= 0.0f )
//		{
//			m_pMoneyUp->Show(false);
//			m_fMoneyUpTime = 0.0f;
//		}
//		else
//		{
//			m_fMoneyUpTime -= fElapsedTime;
//		}
//	}
//}

void CDnMainDlg::UpdateHarvestNotify(float fElapsedTime)
{
	if (GetInterface().IsOpenBlind())
		return;

	if (m_bHarvestAlarm)
	{
		if (m_fHarvestAlarmTime >= 3.f)
		{
			m_bHarvestAlarm = false;
			m_pStaticHarvest->Show(false);
		}
		else
		{
			static int nTimeBack;
			int nBlinkTime = int(m_fHarvestAlarmTime*10.0f);

			if (((nBlinkTime % 3) == 0) && (nTimeBack != nBlinkTime))
			{
				nTimeBack = nBlinkTime;
				m_pStaticHarvest->Show(!m_pStaticHarvest->IsShow());
			}
		}

		m_fHarvestAlarmTime += fElapsedTime;
	}
	else
	{
		if (!CDnLifeSkillPlantTask::IsActive())
		{
			m_pStaticHarvest->Show(false);
		}
		else
		{
			if (GetLifeSkillPlantTask().GetWareHouseItemCount() > 0)
				m_pStaticHarvest->Show(true);
			else
				m_pStaticHarvest->Show(false);
		}
	}
}

#ifdef PRE_ADD_LEVELUP_REWARD_NOTIFIER
void CDnMainDlg::UpdateLevelUpBoxNotifier(float fElapsedTime)
{
	if (GetInterface().IsOpenBlind())
		return;

	if (m_bLevelUpBoxAlarm)
	{
		if (m_fLevelUpboxAlarmTime >= 1.8f)
		{
			m_bLevelUpBoxAlarm = false;
			m_fLevelUpboxAlarmTime = 0.0f;
			m_pStaticLevelUpNotifier->Show(true);
		}
		else
		{
			static int nTimeBack;
			int nBlinkTime = int(m_fLevelUpboxAlarmTime*10.0f);

			if (((nBlinkTime % 3) == 0) && (nTimeBack != nBlinkTime))
			{
				nTimeBack = nBlinkTime;
				m_pStaticLevelUpNotifier->Show(!m_pStaticLevelUpNotifier->IsShow());
			}
		}

		m_fLevelUpboxAlarmTime += fElapsedTime;
	}
}
#endif 

#ifdef PRE_ADD_LEVELUP_GUIDE
void CDnMainDlg::UpdateNewSkillNotify(float fElapsedTime)
{
	if (GetInterface().IsOpenBlind())
		return;

	if (m_bLevelUpSkillAlarm)
	{
		m_pStaticLevelUpSkillAlarm->Show(true);
		//m_pStaticLevelUpSkillAlarmText->Show(true);
		const float levelUpSkillAlarmMaxTime = 5.f;
		if (m_fLevelUpSkillAlarmTime >= levelUpSkillAlarmMaxTime)
		{
			m_fLevelUpSkillAlarmTime = 0.f;
			m_bLevelUpSkillAlarm = false;
			m_pStaticLevelUpSkillAlarm->Show(false);
			//m_pStaticLevelUpSkillAlarmText->Show(false);
		}

		m_fLevelUpSkillAlarmTime += fElapsedTime;
	}
}
#endif

#ifdef PRE_SPECIALBOX
void CDnMainDlg::UpdateSpecialBoxNotify(float fElapsedTime)
{
	return; //RLKT new ui
	if (GetInterface().IsOpenBlind())
		return;

	if (m_bEventReceiverAlarm)
	{
		//m_pStaticEventReceiverBalloon->Show(true);
		//m_pStaticEventReceiverText->Show(true);

		const float fEventSkillAlarmMaxTime = 5.f;
		if (m_fEventReceiverAlarmTime >= fEventSkillAlarmMaxTime)
		{
			m_fEventReceiverAlarmTime = 0.f;
			m_bEventReceiverAlarm = false;
			//m_pStaticEventReceiverBalloon->Show(false);
			//m_pStaticEventReceiverText->Show(false);
		}

		m_fEventReceiverAlarmTime += fElapsedTime;
	}
}
#endif

#ifdef PRE_ADD_REMOTE_QUEST
void CDnMainDlg::ShowQuestNotify(bool bShow, CDnQuestTree::eRemoteQuestState remoteQuestState)
{
	m_bShowQuestNotify = bShow;
	if (m_pStaticQuestBallon)
		m_pStaticQuestBallon->Show(bShow);

	/*if( m_pStaticQuestText )
	{
	if( remoteQuestState == CDnQuestTree::REMOTEQUEST_ASK )
	m_pStaticQuestText->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1200017 ) );
	else if( remoteQuestState == CDnQuestTree::REMOTEQUEST_COMPLETE )
	m_pStaticQuestText->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1200018 ) );

	m_pStaticQuestText->Show( bShow );
	}
	*/
}
#endif // PRE_ADD_REMOTE_QUEST

void CDnMainDlg::UpdateMailNotify(float fElapsedTime)
{
	if (GetInterface().IsOpenBlind())
		return;

	if (m_bMailAlarm)
	{
		if (m_fMailAlarmTime >= 3.f)
		{
			m_bMailAlarm = false;

#ifdef PRE_ADD_MAIL_UI_IMPROVEMENTS
			m_pStaticMailText->Show(false);
			m_pStaticMailBallon->Show(false);
#else
			m_pStaticPost->Show(false);	// 사용하지 않음으로 필요가 없다. 
#endif 
		}
		else
		{
			static int nTimeBack;
			int nBlinkTime = int(m_fMailAlarmTime*10.0f);
#ifdef PRE_ADD_MAIL_UI_IMPROVEMENTS
			m_pStaticMailText->Show(true);
			m_pStaticMailBallon->Show(true);
#endif

			if (((nBlinkTime % 3) == 0) && (nTimeBack != nBlinkTime))
			{
				nTimeBack = nBlinkTime;
#ifdef PRE_ADD_MAIL_UI_IMPROVEMENTS
				m_pStaticPost->Show(false);
#else
				m_pStaticPost->Show(!m_pStaticPost->IsShow());
#endif 

			}
		}

		m_fMailAlarmTime += fElapsedTime;
	}
	else
	{
		if (!CDnTradeTask::IsActive())
		{
#ifdef PRE_ADD_MAIL_UI_IMPROVEMENTS
#else
			m_pStaticPost->Show(false);
#endif
		}
		else
		{
			if (GetTradeTask().GetTradeMail().GetUnreadMailCount() > 0)
			{
#ifdef PRE_ADD_MAIL_UI_IMPROVEMENTS
				m_pStaticPost->Show(false);
#else
				m_pStaticPost->Show(true);
#endif
			}
			else
			{
#ifdef PRE_ADD_MAIL_UI_IMPROVEMENTS
#else
				m_pStaticPost->Show(false);
#endif 
			}
		}
	}
}

void CDnMainDlg::UpdateCashShopGiftNotify(float fElapsedTime)
{
	if (GetInterface().IsOpenBlind())
		return;

	if (m_bCashShopGiftAlarm)
	{
		if (m_fCashShopGiftAlarmTime >= 3.f)
		{
			m_bCashShopGiftAlarm = false;
			m_pStaticCashShopGift->Show(false);
			//m_pStaticCashShopGiftBalloon->Show(false);
			//m_pStaticCashShopGiftText->Show(false);
		}
		else
		{
			//m_pStaticCashShopGiftBalloon->Show( true );
			//_pStaticCashShopGiftText->Show(true);
			static int nTimeBack;
			int nBlinkTime = int(m_fCashShopGiftAlarmTime*10.0f);

			if (((nBlinkTime % 3) == 0) && (nTimeBack != nBlinkTime))
			{
				nTimeBack = nBlinkTime;
				m_pStaticCashShopGift->Show(!m_pStaticCashShopGift->IsShow());
			}
		}

		m_fCashShopGiftAlarmTime += fElapsedTime;
	}
	else
	{
		if (!CDnCashShopTask::IsActive())
		{
			m_pStaticCashShopGift->Show(false);
		}
		else
		{
			m_pStaticCashShopGift->Show(GetCashShopTask().GetGiftListCount() > 0);
		}
	}
}

void CDnMainDlg::ShowMiniSiteNotify(bool bShow, bool bBlink)
{
#ifdef PRE_REMOVE_MINISITE
#else
	m_bNotifyCheckMiniSite = bBlink;
	m_fMiniSiteAlarmTime = bBlink ? 0.f : eMiniSiteBlinkTime;
	m_pNewMiniSiteBtn->Show(bShow);
#endif
}

void CDnMainDlg::UpdateMiniSiteNotify(float fElapsedTime)
{
#ifdef PRE_REMOVE_MINISITE
	return;
#else
	if (GetInterface().IsOpenBlind())
		return;

	if (m_bNotifyCheckMiniSite)
	{
		if (m_fMiniSiteAlarmTime >= eMiniSiteBlinkTime)
		{
			if (m_pNewMiniSiteBtn->IsShow() == false)
				m_pNewMiniSiteBtn->Show(true);
		}
		else
		{
			m_fMiniSiteAlarmTime += fElapsedTime;

			static int nTimeBack;
			int nBlinkTime = int(m_fMiniSiteAlarmTime*10.0f);

			if (((nBlinkTime % 3) == 0) && (nTimeBack != nBlinkTime))
			{
				nTimeBack = nBlinkTime;
				m_pNewMiniSiteBtn->Show(!m_pNewMiniSiteBtn->IsShow());
			}
		}
	}
#endif // PRE_REMOVE_MINISITE
}

//void CDnMainDlg::ShowExpUp( int nExp )
//{
//	//if( nExp <= 0 )
//		return;
//
//	wchar_t szTemp[256]={0};
//	swprintf_s( szTemp, 256, L"+%d EXP", nExp );
//	m_pExpUp->SetText( szTemp );
//
//	SUICoord uiCoord;
//	uiCoord = m_ExpCoord;
//	uiCoord.fHeight = 0.0f;
//	m_pExpUp->SetUICoord( uiCoord );
//	m_pExpUp->Show(true);
//	m_bExpUp = true;
//}
//
//void CDnMainDlg::ShowMoneyUp( INT64 nCoin )
//{
//	//if( nCoin <= 0 )
//		return;
//
//	std::wstring strMoney;
//	DN_INTERFACE::UTIL::GetMoneyFormat( (int)nCoin, strMoney );
//	m_pMoneyUp->SetText( strMoney );
//
//	SUICoord uiCoord;
//	uiCoord = m_MoneyCoord;
//	uiCoord.fHeight = 0.0f;
//	m_pMoneyUp->SetUICoord( uiCoord );
//	m_pMoneyUp->Show(true);
//	m_bMoneyUp = true;
//}

int CDnMainDlg::GetQuickSlotTabIndex()
{
	return m_QuickSlgDlgGroup.GetShowDialogID();
}

void CDnMainDlg::OnRefreshQuickSlot()
{
	CEtUIDialogGroup::DIALOG_VEC &vecQuickSlotDialog = m_QuickSlgDlgGroup.GetDialogList();
	for (int i = 0; i<(int)vecQuickSlotDialog.size() - 1; ++i)	// 마지막 퀵슬롯은 제스처 퀵슬롯이라 제외시킨다.
	{
		CEtUIDialogGroup::ID_DIALOG_PAIR &id_dlg_pair = vecQuickSlotDialog[i];

		if (id_dlg_pair.second == NULL)
			continue;

		static_cast<CDnQuickSlotDlg*>(id_dlg_pair.second)->OnRefreshSlot();
	}
}

void CDnMainDlg::RefreshLifeSkillQuickSlot()
{
	CDnLifeSkillQuickSlotDlg *pLifeQuickSlotDlg = static_cast<CDnLifeSkillQuickSlotDlg*>(m_QuickSlgDlgGroup.GetDialog(LIFESKILL_DLG));
	if (pLifeQuickSlotDlg)
		pLifeQuickSlotDlg->RefreshVehicleItem();
}


void CDnMainDlg::OnMailNotify()
{
	m_bMailAlarm = true;
	m_fMailAlarmTime = 0.0f;
#ifdef PRE_ADD_MAIL_UI_IMPROVEMENTS
	m_pStaticPost->Show(false);
#else
	m_pStaticPost->Show(true);
#endif 

	if (m_nMainDlgSoundIndex[eSOUND_MAILALARM] != -1)
		CEtSoundEngine::GetInstance().PlaySound("2D", m_nMainDlgSoundIndex[eSOUND_MAILALARM]);

	GetInterface().AddChatMessage(CHATTYPE_SYSTEM, L"", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 1963), false);	// UISTRING : 새 우편이 도착했습니다.
}

void CDnMainDlg::OnHarvestNotify(int nCount)
{
	if (nCount > m_nHarvestCount)	//창고에 추가 되었을 경우에만 깜빡인다
	{
		m_bHarvestAlarm = true;
		m_fHarvestAlarmTime = 0.f;

		GetInterface().AddChatMessage(CHATTYPE_SYSTEM, L"", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 7486), false);	// UISTRING : 농장창고에 농작물이 추가되었습니다.
	}

	if (0 == nCount)
		m_pStaticHarvest->Show(false);
	else
		m_pStaticHarvest->Show(true);

	m_nHarvestCount = nCount;
}

void CDnMainDlg::OnCashShopGiftNotify(bool bSoundOnly)
{
	if (m_nMainDlgSoundIndex[eSOUND_CASHSHOPGIFT] != -1)
		CEtSoundEngine::GetInstance().PlaySound("2D", m_nMainDlgSoundIndex[eSOUND_CASHSHOPGIFT]);

	if (bSoundOnly)
		return;

	m_bCashShopGiftAlarm = true;
	m_fCashShopGiftAlarmTime = 0.0f;
	m_pStaticCashShopGift->Show(true);
	//m_pStaticCashShopGiftBalloon->Show(true);
	//m_pStaticCashShopGiftText->Show(true);

	GetInterface().AddChatMessage(CHATTYPE_SYSTEM, L"", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4668), false);	// UISTRING : 선물이 도착했습니다. 캐시샵 선물함에서 확인하세요.
}

#ifdef PRE_ADD_LEVELUP_REWARD_NOTIFIER
void CDnMainDlg::OnLevelUpBoxNotify(bool bShow)
{
	if (m_pStaticLevelUpNotifier == NULL)
		return;

	if (bShow)
	{
		if (m_pStaticLevelUpNotifier && m_pStaticLevelUpNotifier->IsShow())
		{
			m_bLevelUpBoxAlarm = false;
		}
		else
		{
			m_bLevelUpBoxAlarm = true;
		}
	}
	else
	{
		m_bLevelUpBoxAlarm = false;
	}

	m_pStaticLevelUpNotifier->Show(bShow);
}
#endif 

void CDnMainDlg::ToggleMenuButton(DWORD dwDialogID, bool bShow)
{
	for (int i = 0; i<(int)m_vecButtonInfo.size(); i++)
	{
		if (m_vecButtonInfo[i].nDialogID == dwDialogID)
		{
			m_vecButtonInfo[i].pButton->SetOpen(bShow);
			break;
		}
	}
}

#if defined(PRE_MOD_POTENTIAL_JEWEL_RENEWAL)
void CDnMainDlg::ToggleEnableAllButton(bool bEnable)
{
	for (int i = 0; i<(int)m_vecButtonInfo.size(); i++)
	{
		m_vecButtonInfo[i].pButton->Enable(bEnable);
	}
}
#endif // PRE_MOD_POTENTIAL_JEWEL_RENEWAL

//blondymarry start
void CDnMainDlg::ToggleEnableButton(DWORD dwDialogID, bool bEnable)
{
	for (int i = 0; i<(int)m_vecButtonInfo.size(); i++)
	{
		if (m_vecButtonInfo[i].nDialogID == dwDialogID)
		{
			m_vecButtonInfo[i].pButton->Enable(bEnable);
			break;
		}
	}
}
//blondymarry end

void CDnMainDlg::BlinkMenuButton(DWORD dwDialogID)
{
	for (int i = 0; i<(int)m_vecButtonInfo.size(); i++)
	{
		if (m_vecButtonInfo[i].nDialogID == dwDialogID)
		{
			m_vecButtonInfo[i].pButton->SetBlink();
			break;
		}
	}
}

void CDnMainDlg::CloseBlinkMenuButton(DWORD dwDialogID)
{
	for (int i = 0; i<(int)m_vecButtonInfo.size(); i++)
	{
		if (m_vecButtonInfo[i].nDialogID == dwDialogID)
		{
			m_vecButtonInfo[i].pButton->ResetBlink();
			break;
		}
	}
}

CDnMainDlg::SButtonInfo* CDnMainDlg::GetMenuBotton(DWORD dwDialogID)
{
	for (int i = 0; i<(int)m_vecButtonInfo.size(); i++)
	{
		if (m_vecButtonInfo[i].nDialogID == dwDialogID)
		{
			return &m_vecButtonInfo[i];
		}
	}

	return NULL;
}


void CDnMainDlg::RenderEXP()
{
	for (int i = 0; i<5; i++)
	{
		m_pExpGauge[i]->SetProgress(0.0f);
	}

	if (!CDnActor::s_hLocalActor)
		return;

	CDnPlayerActor *pActor = (CDnPlayerActor *)CDnActor::s_hLocalActor.GetPointer();
	int nCurLevExp = pActor->GetCurrentLevelExperience();
	int nNextLevExp = pActor->GetNextLevelExperience();
	int nCurExp = pActor->GetExperience();

	if (nCurExp > nNextLevExp)
	{
		nCurExp = nNextLevExp;
	}

	nNextLevExp -= nCurLevExp;
	nCurExp -= nCurLevExp;

	bool bSetExpGauge = true;
	int nLimitLevel = (int)CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::PlayerLevelLimit);
	if (CDnActor::s_hLocalActor && CDnActor::s_hLocalActor->GetLevel() == nLimitLevel) {
		CDnLocalPlayerActor *pActor = (CDnLocalPlayerActor *)CDnActor::s_hLocalActor.GetPointer();
		if (pActor->GetAccountLevel() < AccountLevel_Master) {
			m_pStaticExp->SetText(L"Max");
			bSetExpGauge = false;
		}
	}
	if (bSetExpGauge) {
		wchar_t szTemp[32] = { 0 };
		swprintf_s(szTemp, 32, L"%.2f%%", nNextLevExp ? ((float)nCurExp / (float)nNextLevExp*100.0f) : 0.0f);
		m_pStaticExp->SetText(szTemp);
	}

	int nMaxExp = nNextLevExp / 5;
	if (nMaxExp == 0) return;
	int nIndex = nCurExp / nMaxExp;
	int nExp = nCurExp % nMaxExp;

	for (int i = 0; i<nIndex; i++)
	{
		m_pExpGauge[i]->SetProgress(100.0f);
	}

	if (nIndex < 5)
	{
		m_pExpGauge[nIndex]->SetProgress((nExp*100.0f) / nMaxExp);
	}
}

void CDnMainDlg::RenderFTG()
{
	m_pFTGGauge->SetProgress(0.0f);

	if (!CDnItemTask::IsActive())
		return;

	int nCurFTG(0), nMaxFTG(0);
	int nTemp1, nTemp2, nTemp3, nTemp4, nTemp5, nTemp6;
	GetItemTask().GetFatigue(nCurFTG, nTemp1, nTemp2, nTemp3);
	GetItemTask().GetMaxFatigue(nMaxFTG, nTemp4, nTemp5, nTemp6);

	if (nCurFTG > nMaxFTG) nCurFTG = nMaxFTG;
	if (nMaxFTG == 0) return;
	m_pFTGGauge->SetProgress((nCurFTG*100.0f) / nMaxFTG);
}

void CDnMainDlg::RenderWeekFTG()
{
	m_pWeekFTGGauge->SetProgress(0.0f);

	if (!CDnItemTask::IsActive())
		return;

	int nCurFTG(0), nMaxFTG(0);
	int nTemp1, nTemp2, nTemp3, nTemp4, nTemp5, nTemp6;
	GetItemTask().GetFatigue(nTemp1, nCurFTG, nTemp2, nTemp3);
	GetItemTask().GetMaxFatigue(nTemp4, nMaxFTG, nTemp5, nTemp6);

	if (nCurFTG > nMaxFTG) nCurFTG = nMaxFTG;
	if (nMaxFTG == 0) return;
	m_pWeekFTGGauge->SetProgress((nCurFTG*100.0f) / nMaxFTG);
}

void CDnMainDlg::RenderPCRoomFTG()
{
	m_pPCRoomFTGGauge->SetProgress(0.0f);

	if (!CDnItemTask::IsActive())
		return;

	int nCurFTG(0), nMaxFTG(0);
	int nTemp1, nTemp2, nTemp3, nTemp4, nTemp5, nTemp6;
	GetItemTask().GetFatigue(nTemp1, nTemp2, nCurFTG, nTemp3);
	GetItemTask().GetMaxFatigue(nTemp4, nTemp5, nMaxFTG, nTemp6);

	if (nCurFTG > nMaxFTG) nCurFTG = nMaxFTG;
	if (nMaxFTG == 0) return;
	m_pPCRoomFTGGauge->SetProgress((nCurFTG*100.0f) / nMaxFTG);
}

void CDnMainDlg::RenderEventFTG()
{
	m_pEventFTGGauge->SetProgress(0.0f);

	if (!CDnItemTask::IsActive())
		return;

	int nCurFTG(0), nMaxFTG(0);
	int nTemp1, nTemp2, nTemp3, nTemp4, nTemp5, nTemp6;
	GetItemTask().GetFatigue(nTemp1, nTemp2, nTemp3, nCurFTG);
	GetItemTask().GetMaxFatigue(nTemp4, nTemp5, nTemp6, nMaxFTG);
	if (nCurFTG > nMaxFTG) nCurFTG = nMaxFTG;
	if (nMaxFTG == 0) return;
	m_pEventFTGGauge->SetProgress((nCurFTG*100.0f) / nMaxFTG);
}

#ifdef PRE_ADD_VIP
void CDnMainDlg::RenderVIPFTG()
{
	if (m_pVIPFTGGauge == NULL)
		return;

	m_pVIPFTGGauge->SetProgress(0.0f);

	if (!CDnItemTask::IsActive())
		return;

	int nCurFTG(0), nMaxFTG(0);
	nCurFTG = GetItemTask().GetFatigue(CDnItemTask::eVIPFTG);
	nMaxFTG = GetItemTask().GetMaxFatigue(CDnItemTask::eVIPFTG);

	if (nCurFTG > nMaxFTG) nCurFTG = nMaxFTG;
	if (nMaxFTG == 0) return;
	m_pVIPFTGGauge->SetProgress((nCurFTG*100.0f) / nMaxFTG);
}
#endif // PRE_ADD_VIP

void CDnMainDlg::RenderFTGStatic()
{
	if (!CDnItemTask::IsActive())
		return;

#ifdef PRE_ADD_VIP
	//	todo by kalliste : need refactoring statics -> vectorize(?) with type
	m_pStaticFTGFont->Show(false);
	m_pStaticPCRoomFTGFont->Show(false);
	m_pStaticEventFTGFont->Show(false);
	m_pStaticWeekFTGFont->Show(false);
	if (m_pStaticVIPFTGFont)
		m_pStaticVIPFTGFont->Show(false);

	m_pStaticFTG->Show(false);
	m_pStaticPCRoomFTG->Show(false);
	m_pStaticEventFTG->Show(false);
	m_pStaticWeekFTG->Show(false);
	if (m_pStaticVIPFTG)
		m_pStaticVIPFTG->Show(false);

	int nFatigues[CDnItemTask::eFTGMAX];
	int i = 0;
	for (; i < CDnItemTask::eFTGMAX; ++i)
		nFatigues[i] = GetItemTask().GetFatigue((CDnItemTask::eFatigueType)i);

	if (nFatigues[CDnItemTask::eEVENTFTG] > 0)
	{
		m_pStaticEventFTGFont->SetIntToText(nFatigues[CDnItemTask::eEVENTFTG]);
		m_pStaticEventFTGFont->Show(true);
		m_pStaticEventFTG->Show(true);
	}
	else if (nFatigues[CDnItemTask::ePCBANGFTG] > 0)
	{
		m_pStaticPCRoomFTGFont->SetIntToText(nFatigues[CDnItemTask::ePCBANGFTG]);
		m_pStaticPCRoomFTGFont->Show(true);
		m_pStaticPCRoomFTG->Show(true);
	}
	else if (m_pStaticVIPFTG && m_pStaticVIPFTGFont && nFatigues[CDnItemTask::eVIPFTG] > 0)
	{
		m_pStaticVIPFTGFont->SetIntToText(nFatigues[CDnItemTask::eVIPFTG]);
		m_pStaticVIPFTGFont->Show(true);
		m_pStaticVIPFTG->Show(true);
	}
	else if (nFatigues[CDnItemTask::eDAILYFTG] > 0)
	{
		m_pStaticFTGFont->SetIntToText(nFatigues[CDnItemTask::eDAILYFTG]);
		m_pStaticFTGFont->Show(true);
		m_pStaticFTG->Show(true);
	}
	else
	{
		m_pStaticWeekFTGFont->SetIntToText(nFatigues[CDnItemTask::eWEEKLYFTG]);
		m_pStaticWeekFTGFont->Show(true);
		m_pStaticWeekFTG->Show(true);
	}
#else // PRE_ADD_VIP
	int nTemp1, nTemp2, nTemp3, nTemp4;
	GetItemTask().GetFatigue(nTemp1, nTemp2, nTemp3, nTemp4);

	if (nTemp4 > 0)
	{
		m_pStaticFTGFont->Show(false);
		m_pStaticWeekFTGFont->Show(false);
		m_pStaticPCRoomFTGFont->Show(false);
		m_pStaticEventFTGFont->SetIntToText(nTemp4);
		m_pStaticEventFTGFont->Show(true);

		m_pStaticFTG->Show(false);
		m_pStaticWeekFTG->Show(false);
		m_pStaticPCRoomFTG->Show(false);
		m_pStaticEventFTG->Show(true);
	}
	else if (nTemp3 > 0)
	{
		m_pStaticFTGFont->Show(false);
		m_pStaticWeekFTGFont->Show(false);
		m_pStaticEventFTGFont->Show(false);
		m_pStaticPCRoomFTGFont->SetIntToText(nTemp3);
		m_pStaticPCRoomFTGFont->Show(true);

		m_pStaticFTG->Show(false);
		m_pStaticWeekFTG->Show(false);
		m_pStaticEventFTG->Show(false);
		m_pStaticPCRoomFTG->Show(true);
	}
	else if (nTemp1 > 0)
	{
		m_pStaticWeekFTGFont->Show(false);
		m_pStaticPCRoomFTGFont->Show(false);
		m_pStaticEventFTGFont->Show(false);
		m_pStaticFTGFont->SetIntToText(nTemp1);
		m_pStaticFTGFont->Show(true);

		m_pStaticWeekFTG->Show(false);
		m_pStaticPCRoomFTG->Show(false);
		m_pStaticEventFTG->Show(false);
		m_pStaticFTG->Show(true);
	}
	else
	{
		m_pStaticFTGFont->Show(false);
		m_pStaticPCRoomFTGFont->Show(false);
		m_pStaticEventFTGFont->Show(false);
		m_pStaticWeekFTGFont->SetIntToText(nTemp2);
		m_pStaticWeekFTGFont->Show(true);

		m_pStaticFTG->Show(false);
		m_pStaticPCRoomFTG->Show(false);
		m_pStaticEventFTG->Show(false);
		m_pStaticWeekFTG->Show(true);
	}
#endif // PRE_ADD_VIP
}

void CDnMainDlg::ShowFTGDialog(bool bShow, float fX, float fY)
{
	if (bShow)
	{
		if (CDnActor::s_hLocalActor && CDnItemTask::IsActive())
		{
#ifdef PRE_ADD_VIP
			int ftgs[CDnItemTask::eFTGMAX];
			memset(ftgs, 0, sizeof(ftgs));

			int i = 0;
			for (; i < CDnItemTask::eFTGMAX; ++i)
			{
				ftgs[i] = GetItemTask().GetFatigue((CDnItemTask::eFatigueType)i);
				m_pPopupFTGDlg->SetFatigue((CDnItemTask::eFatigueType)i, ftgs[i]);
			}
#else
			int nTemp1, nTemp2, nTemp3, nTemp4;
			GetItemTask().GetFatigue(nTemp1, nTemp2, nTemp3, nTemp4);

			m_pPopupFTGDlg->SetFatigue(nTemp1, nTemp2, nTemp3, nTemp4);
#endif // PRE_ADD_VIP
			m_pPopupFTGDlg->Show(true);

			SUICoord dlgCoord;
			m_pPopupFTGDlg->GetDlgCoord(dlgCoord);
			dlgCoord.fX = fX + GetXCoord() - (dlgCoord.fWidth / 2.0f);
			dlgCoord.fY = fY + GetYCoord() - dlgCoord.fHeight - 3.0f / DEFAULT_UI_SCREEN_HEIGHT;
			m_pPopupFTGDlg->SetDlgCoord(dlgCoord);
		}
	}
	else
	{
		m_pPopupFTGDlg->Show(false);
	}
}

void CDnMainDlg::AddMessage(const wchar_t *szMsg, DWORD dTextColor)
{
	for (int i = 5; i >= 0; i--)
	{
		if (m_vecStaticMessage[i]->IsShow())
		{
			m_vecStaticMessage[i + 1]->SetMsgTime(m_vecStaticMessage[i]->GetMsgTime());
			m_vecStaticMessage[i + 1]->SetTextWithEllipsis(m_vecStaticMessage[i]->GetText(), L"...");
			m_vecStaticMessage[i + 1]->SetTextColor(m_vecStaticMessage[i]->GetTextColor());
			m_vecStaticMessage[i + 1]->Show(true);

		}
	}

	m_vecStaticMessage[0]->SetMsgTime(5.0f);
	m_vecStaticMessage[0]->SetTextWithEllipsis(szMsg, L"...");
	m_vecStaticMessage[0]->Show(true);
	m_vecStaticMessage[0]->SetTextColor(dTextColor);
}

bool CDnMainDlg::SetQuickSlot(int nIndex, MIInventoryItem *pItem)
{
	CDnQuickSlotDlg *pQuickSlotDlg = (CDnQuickSlotDlg*)m_QuickSlgDlgGroup.GetShowDialog();
	if (pQuickSlotDlg)
	{
		return pQuickSlotDlg->SetSlot(nIndex, pItem);
	}

	return false;
}

bool CDnMainDlg::SetQuickSlot(int nQuickSlotIndex, CDnSkill *pSkill)
{
	int nTabIndex = nQuickSlotIndex / QUICKSLOT_BUTTON_MAX;
	int nSlotIndex = nQuickSlotIndex % QUICKSLOT_BUTTON_MAX;

	ASSERT((nTabIndex >= QUICKSLOT_DLG_00) && (nTabIndex<QUICKSLOT_DLG_MAX));
	ASSERT((nSlotIndex >= 0) && (nSlotIndex<QUICKSLOT_BUTTON_MAX));

	CDnQuickSlotDlg *pCurQuickSlotDlg = static_cast<CDnQuickSlotDlg*>(m_QuickSlgDlgGroup.GetDialog(nTabIndex));
	return pCurQuickSlotDlg->SetSlot(nSlotIndex, pSkill);
}

void CDnMainDlg::AddPassiveSkill(DnSkillHandle hSkill)
{
	m_pPassiveSkillSlotDlg->AddPassiveSkill(hSkill);
}

#if defined(PRE_FIX_61821)
void CDnMainDlg::ReplacePassiveSkill(DnSkillHandle hSkill)
{
	if (m_pPassiveSkillSlotDlg)
		m_pPassiveSkillSlotDlg->ReplacePassiveSkill(hSkill);
}
#endif // PRE_FIX_61821

void CDnMainDlg::ShowMinimapOption(bool bShow)
{
	if (m_bShowOption == bShow) {
		return;
	}
	m_bShowOption = bShow;
	GetControl<CEtUICheckBox>("ID_CHECKBOX_FUNCTIONNPC")->Show(bShow);
	GetControl<CEtUIStatic>("ID_STATIC_FUNCTIONNPC")->Show(bShow);
	GetControl<CEtUICheckBox>("ID_CHECKBOX_NPC")->Show(bShow);
	GetControl<CEtUIStatic>("ID_STATIC_NPC")->Show(bShow);
	GetControl<CEtUIStatic>("ID_MM_BACK")->Show(bShow);
}

#ifdef PRE_ADD_AUTO_DICE
void CDnMainDlg::ShowAutoDiceOption(bool bShow)
{
	if (m_bShowDiceOption == bShow) {
		return;
	}

	m_bShowDiceOption = bShow;
	GetControl<CEtUIStatic>("ID_TEXT_DICE")->Show(bShow);
	GetControl<CEtUIStatic>("ID_STATIC0")->Show(bShow);
	GetControl<CEtUICheckBox>("ID_CHECKBOX_DICE")->Show(bShow);
}

void CDnMainDlg::UpateAutoDiceOption(bool bReset)
{
	if (bReset)
		CGameOption::GetInstance().m_bEnableAutoDice = false;

	GetControl<CDnMenuButton>("ID_BUTTON_DICE")->SetOpen(CGameOption::GetInstance().m_bEnableAutoDice);
	GetControl<CEtUICheckBox>("ID_CHECKBOX_DICE")->SetChecked(CGameOption::GetInstance().m_bEnableAutoDice);
}
#endif


void CDnMainDlg::UpdateMinimapOption()
{
	m_pMainBar_MinimapDlg->UpdateMinimapOption();
}



void CDnMainDlg::ProcessSystemMenuButton()
{
	bool bEnable = true;
	if (CDnPartyTask::IsActive() && GetPartyTask().IsLocalActorEnterGateReady() && !GetPartyTask().IsGMTracing())
		bEnable = false;
	if (m_bEnableButtons == false)
		bEnable = false;
	if (CDnPartyTask::IsActive() && GetPartyTask().IsPartyProcessState(CDnPartyTask::PPS_ACCEPT_INVITE))
		bEnable = false;

	if (bEnable == false)
	{
		CDnMainMenuDlg *pMainMenuDlg = GetInterface().GetMainMenuDialog();
		if (pMainMenuDlg)
			pMainMenuDlg->CloseSystemDialogCompletely();
	}

	ToggleEnableButton(CDnMainMenuDlg::SYSTEM_DIALOG, bEnable);
}

void CDnMainDlg::ProcessCommunityButton()
{
	bool bEnable = true;

	if (CDnPartyTask::IsActive() && GetPartyTask().IsLocalActorEnterGateReady())
		bEnable = false;
	if (m_bEnableButtons == false)
		bEnable = false;
	if (CDnPartyTask::IsActive() && GetPartyTask().IsPartyProcessState(CDnPartyTask::PPS_ACCEPT_INVITE))
		bEnable = false;

#ifdef PRE_ADD_DWC
	// DWC 게임 도중에는 커뮤니티창을 열지 못하게 막는다.
	if (GetDWCTask().IsDWCPvPModePlaying())
		bEnable = false;
#endif

	if (bEnable == false)
	{
		CDnMainMenuDlg *pMainMenuDlg = GetInterface().GetMainMenuDialog();
		if (pMainMenuDlg && pMainMenuDlg->IsOpenDialog(CDnMainMenuDlg::COMMUNITY_DIALOG))
			pMainMenuDlg->ToggleShowDialog(CDnMainMenuDlg::COMMUNITY_DIALOG);
	}

	ToggleEnableButton(CDnMainMenuDlg::COMMUNITY_DIALOG, bEnable);
}

#ifdef PRE_ADD_PVP_RANKING
void CDnMainDlg::ProcessPvPRankButton()
{
	bool bEnable = false;
	if (GetInterface().GetInterfaceType() == CDnInterface::Village ||
		GetInterface().GetInterfaceType() == CDnInterface::PVPVillage
		)
		bEnable = true;

	ToggleEnableButton(CDnMainMenuDlg::PVP_RANK_DIALOG, bEnable);
}
#endif // PRE_ADD_PVP_RANKING

void CDnMainDlg::ProcessQuickSlot()
{
	// 배틀모드 전환쪽 코드 뒤에 콜백형태 등으로 구현할까 하다가,
	// 퀵슬롯 근처에 모든 처리를 두는게 나중에 관리하기 편할 듯 해서 그냥 프로세싱 부분에서 처리하기로 하겠다.
	// 패드 관련 코드는 여기서 구현한다.
	if (CDnActor::s_hLocalActor)
	{
		CDnPlayerActor *pActor = static_cast<CDnPlayerActor *>(CDnActor::s_hLocalActor.GetPointer());
		bool bBattleMode = pActor->IsBattleMode();

		if (!bBattleMode)
		{
			// 평화모드에서 제스처가 아니면 전환
			if (!m_QuickSlgDlgGroup.IsShowDialog(LIFESKILL_DLG))
			{
				m_dwPrevQuickDlgID = m_QuickSlgDlgGroup.GetShowDialogID();
				m_QuickSlgDlgGroup.ShowDialog(LIFESKILL_DLG, true);
				m_pStaticSlotNumber->SetText(L"G");
				ShowExpDialog(false);
				ShowFTGDialog(false);
			}
		}
		else if (m_QuickSlgDlgGroup.IsShowDialog(LIFESKILL_DLG))
		{
			// 전투모드에서 제스처가 보이면 전환
			m_QuickSlgDlgGroup.ShowDialog(m_dwPrevQuickDlgID, true);

			if (m_dwPrevQuickDlgID == QUICKSLOT_EVENT)
				m_pStaticSlotNumber->SetText(L"E");
			else
			{
				m_pStaticSlotNumber->SetIntToText((m_dwPrevQuickDlgID - (m_nSkillSlotPage * QUICKSLOT_PAGE_COUNT)) + 1);
			}

			ShowExpDialog(false);
			ShowFTGDialog(false);
		}
	}
}


void CDnMainDlg::ProcessZoneMapButton()
{
	bool bEnable = true;
	if (m_bPVP == true) bEnable = false;
	if (CDnPartyTask::IsActive() && GetPartyTask().IsLocalActorEnterGateReady()) bEnable = false;
	if (CDnWorld::GetInstance().GetMapType() == CDnWorld::MapTypeDungeon)
	{
		CDnGameTask * pGameTask = (CDnGameTask *)CTaskManager::GetInstance().GetTask("GameTask");
		if (!pGameTask || pGameTask->GetGameTaskType() == GameTaskType::Farm)
			bEnable = true;
		else
			bEnable = false;
	}

	if (m_bEnableButtons == false) bEnable = false;

	if (!bEnable) {
		CDnMainMenuDlg *pMainMenuDlg = GetInterface().GetMainMenuDialog();
		if (pMainMenuDlg && pMainMenuDlg->IsOpenDialog(CDnMainMenuDlg::ZONEMAP_DIALOG))
			pMainMenuDlg->ToggleShowDialog(CDnMainMenuDlg::ZONEMAP_DIALOG);
	}

	ToggleEnableButton(CDnMainMenuDlg::ZONEMAP_DIALOG, bEnable);
}

void CDnMainDlg::ProcessChatRoomButton()
{
	bool bEnable = true;
	if (CDnPartyTask::IsActive() && GetPartyTask().IsLocalActorEnterGateReady()) bEnable = false;

	if (m_bEnableButtons == false) bEnable = false;

	if (CDnInterface::IsActive() && GetInterface().IsPVP()) bEnable = false;

	if (!bEnable) {
		CDnMainMenuDlg *pMainMenuDlg = GetInterface().GetMainMenuDialog();
		if (pMainMenuDlg && pMainMenuDlg->IsOpenDialog(CDnMainMenuDlg::CHATROOM_DIALOG))
			pMainMenuDlg->ToggleShowDialog(CDnMainMenuDlg::CHATROOM_DIALOG);
	}

	ToggleEnableButton(CDnMainMenuDlg::CHATROOM_DIALOG, bEnable);
}

void CDnMainDlg::ProcessButton(DWORD dwDialogId)
{
	bool bEnable = true;
	if (CDnPartyTask::IsActive() && GetPartyTask().IsLocalActorEnterGateReady())
		bEnable = false;
	if (m_bEnableButtons == false)
		bEnable = false;
	if (CDnPartyTask::IsActive() && GetPartyTask().IsPartyProcessState(CDnPartyTask::PPS_ACCEPT_INVITE))
		bEnable = false;

	if (dwDialogId == CDnMainMenuDlg::CHARSTATUS_DIALOG || dwDialogId == CDnMainMenuDlg::INVENTORY_DIALOG)
	{
		if (CDnActor::s_hLocalActor)
		{
			CDnLocalPlayerActor *pActor = (CDnLocalPlayerActor *)CDnActor::s_hLocalActor.GetPointer();
			if (pActor->IsCannonMode())
			{
				bEnable = false;
			}
		}
	}

#if defined(PRE_ADD_MAILBOX_OPEN)
	if (dwDialogId == CDnMainMenuDlg::MAIL_DIALOG)
	{
		if (CDnWorld::MapTypeVillage != CDnWorld::GetInstance().GetMapType())
			bEnable = false;
	}
#endif // PRE_ADD_MAILBOX_OPEN

#ifdef PRE_FIX_HOLD_COSTUMEMIX_INVEN
	bool bNoToggleShowDialog = bEnable;
	if (m_HoldToggleShowDialogOnProcessButton >= 0 && m_HoldToggleShowDialogOnProcessButton == dwDialogId)
	{
		bNoToggleShowDialog = true;
	}

	if (bEnable == false && bNoToggleShowDialog == false)
#else
	if (bEnable == false)
#endif
	{
		CDnMainMenuDlg *pMainMenuDlg = GetInterface().GetMainMenuDialog();
		if (pMainMenuDlg && pMainMenuDlg->IsOpenDialog(dwDialogId))
			pMainMenuDlg->ToggleShowDialog(dwDialogId);
	}

	ToggleEnableButton(dwDialogId, bEnable);
}

void CDnMainDlg::ProcessPVPModeButton()
{
	bool bEnable = true;
	bool bEnalbeSkill = true;

	if (CDnActor::s_hLocalActor)
	{
		if (CDnActor::s_hLocalActor)
		{
			CDnPlayerActor *pPlayer = dynamic_cast<CDnPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
			if (pPlayer && pPlayer->IsObserver())
				bEnable = false;
			if (IsLockQuickSlot())
				bEnalbeSkill = false;
		}
	}

	switch (CDnWorld::GetInstance().GetMapType()) {
	case CDnWorld::MapTypeVillage:
	case CDnWorld::MapTypeWorldMap:
	{
		// ToggleEnableButton(CDnMainMenuDlg::COMMUNITY_DIALOG,!m_bPVP); // 이전코드인데 , ProcessCommuinity 버튼을 하면서 의미없는 코드였는듯  , 삭제.
		ToggleEnableButton(CDnMainMenuDlg::ZONEMAP_DIALOG, !m_bPVP);   // pvp마을에서는 특정 부분을 사용하지 못하게 설정.
	}
	break;
	case CDnWorld::MapTypeDungeon:
	{
		if (
			CDnBridgeTask::GetInstance().IsPvPGameMode(PvPCommon::GameMode::PvP_GuildWar) 		// 길드전 일때 맵 활성화 한다.
			|| CDnBridgeTask::GetInstance().IsPvPGameMode(PvPCommon::GameMode::PvP_Occupation) 	// 점령전 일때 맵 활성화 한다.
			)
		{
			ToggleEnableButton(CDnMainMenuDlg::ZONEMAP_DIALOG, m_bPVP);
		}
	}
	break;
	}

	ToggleEnableButton(CDnMainMenuDlg::QUEST_DIALOG, bEnable);
	ToggleEnableButton(CDnMainMenuDlg::SKILL_DIALOG, (bEnable && bEnalbeSkill));
}


#ifdef PRE_ADD_PVP_VILLAGE_ACCESS

void CDnMainDlg::ProcessPVPVillageAccessDlg()
{
	bool bEnable = m_bEnableButtons;

	if (CDnPartyTask::IsActive() && GetPartyTask().IsLocalActorEnterGateReady())
		bEnable = false;

	if (CDnWorld::GetInstance().GetMapType() != CDnWorld::MapTypeVillage)
		bEnable = false;

#ifdef PRE_PARTY_DB
	if (CDnActor::s_hLocalActor)
	{
		CDnLocalPlayerActor *pActor = static_cast<CDnLocalPlayerActor *>(CDnActor::s_hLocalActor.GetPointer());
		if (pActor)
		{
			if (CDnPartyTask::IsActive() && GetPartyTask().IsPartyMember(pActor->GetUniqueID()))
				bEnable = false;
		}
	}
#endif

#ifdef PRE_ADD_DWC
	bEnable = GetDWCTask().IsDWCChar();
#endif
	ToggleEnableButton(CDnMainMenuDlg::PVP_VILLAGE_ACCESS, !bEnable);
}

#endif

void CDnMainDlg::SetMainUIHotKey(int nUIWrappingKeyIndex, BYTE cVK)
{
	// 핫키를 임의로 바꿀 수 있는 기능이 없어서, 우선은 이런 식으로 처리한다.
	CDnQuickSlotDlg *pQuickSlotDlg = NULL;
	CDnLifeSkillQuickSlotDlg *pLifeSkillQuickSlotDlg = NULL;

	//if (m_pMainBarMenuDlg)
	//	m_pMainBarMenuDlg->SetButtonsHotKeys(nUIWrappingKeyIndex, cVK);

	bool bQuickSlot = false;
	char szName[32];
	switch (nUIWrappingKeyIndex)
	{
	case IW_UI_QUICKSLOTCHANGE:
		if (GetControl("ID_BUTTON_SLOT_UP"))	GetControl("ID_BUTTON_SLOT_UP")->SetHotKey(cVK);
		break;
	case IW_UI_QUICKSLOT1:	sprintf_s(szName, _countof(szName), "ID_QUICKSLOT1"); bQuickSlot = true; break;
	case IW_UI_QUICKSLOT2:	sprintf_s(szName, _countof(szName), "ID_QUICKSLOT2"); bQuickSlot = true; break;
	case IW_UI_QUICKSLOT3:	sprintf_s(szName, _countof(szName), "ID_QUICKSLOT3"); bQuickSlot = true; break;
	case IW_UI_QUICKSLOT4:	sprintf_s(szName, _countof(szName), "ID_QUICKSLOT4"); bQuickSlot = true; break;
	case IW_UI_QUICKSLOT5:	sprintf_s(szName, _countof(szName), "ID_QUICKSLOT5"); bQuickSlot = true; break;
	case IW_UI_QUICKSLOT6:	sprintf_s(szName, _countof(szName), "ID_QUICKSLOT6"); bQuickSlot = true; break;
	case IW_UI_QUICKSLOT7:	sprintf_s(szName, _countof(szName), "ID_QUICKSLOT7"); bQuickSlot = true; break;
	case IW_UI_QUICKSLOT8:	sprintf_s(szName, _countof(szName), "ID_QUICKSLOT8"); bQuickSlot = true; break;
	case IW_UI_QUICKSLOT9:	sprintf_s(szName, _countof(szName), "ID_QUICKSLOT9"); bQuickSlot = true; break;
	case IW_UI_QUICKSLOT10:	sprintf_s(szName, _countof(szName), "ID_QUICKSLOT10"); bQuickSlot = true; break;
#if defined( PRE_ADD_SKILLSLOT_EXTEND )
	case IW_UI_QUICKSLOT11:	sprintf_s(szName, _countof(szName), "ID_QUICKSLOT11"); bQuickSlot = true; break;
	case IW_UI_QUICKSLOT12:	sprintf_s(szName, _countof(szName), "ID_QUICKSLOT12"); bQuickSlot = true; break;
#endif	// #if defined( PRE_ADD_SKILLSLOT_EXTEND )
	case IW_UI_CHAR:		if (GetControl("ID_CHAR_STATUS")) GetControl("ID_CHAR_STATUS")->SetHotKey(cVK);	break;
	case IW_UI_COMMUNITY:	if (GetControl("ID_COMMUNITY")) GetControl("ID_COMMUNITY")->SetHotKey(cVK);	break;
	case IW_UI_INVEN:		if (GetControl("ID_INVENTORY")) GetControl("ID_INVENTORY")->SetHotKey(cVK);	break;
	case IW_UI_QUEST:		if (GetControl("ID_QUEST")) GetControl("ID_QUEST")->SetHotKey(cVK);		break;
	case IW_UI_SKILL:		if (GetControl("ID_SKILL")) GetControl("ID_SKILL")->SetHotKey(cVK);		break;
	case IW_UI_MISSION:		if (GetControl("ID_BT_MISSION")) GetControl("ID_BT_MISSION")->SetHotKey(cVK);		break;
	case IW_UI_MAP:			if (GetControl("ID_BT_MAP")) GetControl("ID_BT_MAP")->SetHotKey(cVK);			break;
	case IW_UI_CHAT:		if (GetControl("ID_BT_CHAT")) GetControl("ID_BT_CHAT")->SetHotKey(cVK);		break;
#if defined(PRE_ADD_NPC_REPUTATION_SYSTEM)
	case IW_UI_REPUTE:		if (GetControl("ID_BT_REPUTE")) GetControl("ID_BT_REPUTE")->SetHotKey(cVK);	break;
#endif
	case IW_UI_EVENT:		if (GetControl("ID_BT_EVENT")) GetControl("ID_BT_EVENT")->SetHotKey(cVK);	break;
	case IW_UI_USERHELP:	if (GetControl("ID_HELP")) GetControl("ID_HELP")->SetHotKey(cVK);	break;
#ifdef PRE_ADD_PVP_VILLAGE_ACCESS
	case IW_UI_PVP_VILLAGE_ACCESS: if (GetControl("ID_BT_COLOSSEUM")) GetControl("ID_BT_COLOSSEUM")->SetHotKey(cVK); break;
#endif
#ifdef PRE_ADD_PVP_RANKING
	case IW_UI_PVPRANK_OPEN: if (GetControl("ID_BT_6RANK")) GetControl("ID_BT_RANK")->SetHotKey(cVK); break;
#endif // PRE_ADD_PVP_RANKING
#ifdef PRE_ADD_MAIL_UI_IMPROVEMENTS	
	case IW_UI_MAILBOX_OPEN: if (GetControl("ID_MAIL")) GetControl("ID_MAIL")->SetHotKey(cVK); break;
#endif
		//	case IW_UI_MAP:			if (m_pMainBarMenuDlg && m_pMainBarMenuDlg->GetControl("ID_BT_MAP")) m_pMainBarMenuDlg->GetControl("ID_BT_MAP")->SetHotKey( cVK );			break;
		//	case IW_UI_CHAT:		if(m_pMainBarMenuDlg && m_pMainBarMenuDlg->GetControl("ID_BT_CHAT") ) m_pMainBarMenuDlg->GetControl("ID_BT_CHAT")->SetHotKey( cVK );		break;
		//#if defined(PRE_ADD_NPC_REPUTATION_SYSTEM)
		//	case IW_UI_REPUTE:		if(m_pMainBarMenuDlg && m_pMainBarMenuDlg->GetControl("ID_BT_REPUTE") ) m_pMainBarMenuDlg->GetControl("ID_BT_REPUTE")->SetHotKey( cVK );	break;
		//#endif
		//	case IW_UI_EVENT:		if(m_pMainBarMenuDlg && m_pMainBarMenuDlg->GetControl( "ID_BT_EVENT" ) ) m_pMainBarMenuDlg->GetControl("ID_BT_EVENT")->SetHotKey( cVK );	break;
		//	case IW_UI_USERHELP:	if(m_pMainBarMenuDlg && m_pMainBarMenuDlg->GetControl( "ID_BT_HELP" ) ) m_pMainBarMenuDlg->GetControl("ID_BT_HELP")->SetHotKey( cVK );	break;
		//#ifdef PRE_ADD_PVP_VILLAGE_ACCESS
		//	case IW_UI_PVP_VILLAGE_ACCESS: if(m_pMainBarMenuDlg && m_pMainBarMenuDlg->GetControl("ID_BT_COLOSSEUM")) m_pMainBarMenuDlg->GetControl("ID_BT_COLOSSEUM")->SetHotKey( cVK ); break;
		//#endif
		//#ifdef PRE_ADD_PVP_RANKING
		//	case IW_UI_PVPRANK_OPEN: if(m_pMainBarMenuDlg && m_pMainBarMenuDlg->GetControl("ID_BT_RANK") ) m_pMainBarMenuDlg->GetControl("ID_BT_RANK")->SetHotKey( cVK ); break;
		//#endif // PRE_ADD_PVP_RANKING
		//#ifdef PRE_ADD_MAIL_UI_IMPROVEMENTS	
		//	case IW_UI_MAILBOX_OPEN : if(GetControl("ID_MAIL")) GetControl("ID_MAIL")->SetHotKey( cVK ); break;
		//#endif
	case IW_UI_MENU: if (GetControl("ID_MENU")) GetControl("ID_MENU")->SetHotKey('N'); break; //RLKT CHANGE STATIC!
	}

	// 퀵슬롯의 경우 자식 다이얼로그에다가 설정해야한다.
	if (bQuickSlot)
	{
		CEtUIControl *pControl;
		for (int i = 0; i < m_QuickSlgDlgGroup.GetDialogCount(); ++i)
		{
			pControl = m_QuickSlgDlgGroup.GetDialogFromIndex(i)->GetControl(szName);
			if (pControl)
				pControl->SetHotKey(cVK);
			if (m_QuickSlgDlgGroup.GetDialogFromIndex(i)->GetDialogID() == LIFESKILL_DLG)
			{
				switch (nUIWrappingKeyIndex)
				{
#if defined( PRE_ADD_SKILLSLOT_EXTEND )
				case IW_UI_QUICKSLOT1:	sprintf_s(szName, _countof(szName), "ID_QUICKSLOT13"); break;
				case IW_UI_QUICKSLOT2:	sprintf_s(szName, _countof(szName), "ID_QUICKSLOT14"); break;
				case IW_UI_QUICKSLOT3:	sprintf_s(szName, _countof(szName), "ID_QUICKSLOT15"); break;
				case IW_UI_QUICKSLOT4:	sprintf_s(szName, _countof(szName), "ID_QUICKSLOT16"); break;
				case IW_UI_QUICKSLOT5:	sprintf_s(szName, _countof(szName), "ID_QUICKSLOT17"); break;
				case IW_UI_QUICKSLOT6:	sprintf_s(szName, _countof(szName), "ID_QUICKSLOT18"); break;
				case IW_UI_QUICKSLOT7:	sprintf_s(szName, _countof(szName), "ID_QUICKSLOT19"); break;
				case IW_UI_QUICKSLOT8:	sprintf_s(szName, _countof(szName), "ID_QUICKSLOT20"); break;
				case IW_UI_QUICKSLOT9:	sprintf_s(szName, _countof(szName), "ID_QUICKSLOT21"); break;
				case IW_UI_QUICKSLOT10:	sprintf_s(szName, _countof(szName), "ID_QUICKSLOT22"); break;
				case IW_UI_QUICKSLOT11:	sprintf_s(szName, _countof(szName), "ID_QUICKSLOT23"); break;
				case IW_UI_QUICKSLOT12:	sprintf_s(szName, _countof(szName), "ID_QUICKSLOT24"); break;
#else
				case IW_UI_QUICKSLOT1:	sprintf_s(szName, _countof(szName), "ID_QUICKSLOT11"); break;
				case IW_UI_QUICKSLOT2:	sprintf_s(szName, _countof(szName), "ID_QUICKSLOT12"); break;
				case IW_UI_QUICKSLOT3:	sprintf_s(szName, _countof(szName), "ID_QUICKSLOT13"); break;
				case IW_UI_QUICKSLOT4:	sprintf_s(szName, _countof(szName), "ID_QUICKSLOT14"); break;
				case IW_UI_QUICKSLOT5:	sprintf_s(szName, _countof(szName), "ID_QUICKSLOT15"); break;
				case IW_UI_QUICKSLOT6:	sprintf_s(szName, _countof(szName), "ID_QUICKSLOT16"); break;
				case IW_UI_QUICKSLOT7:	sprintf_s(szName, _countof(szName), "ID_QUICKSLOT17"); break;
				case IW_UI_QUICKSLOT8:	sprintf_s(szName, _countof(szName), "ID_QUICKSLOT18"); break;
				case IW_UI_QUICKSLOT9:	sprintf_s(szName, _countof(szName), "ID_QUICKSLOT19"); break;
				case IW_UI_QUICKSLOT10:	sprintf_s(szName, _countof(szName), "ID_QUICKSLOT20"); break;
#endif	// #if defined( PRE_ADD_SKILLSLOT_EXTEND )
				}
				pControl = m_QuickSlgDlgGroup.GetDialogFromIndex(i)->GetControl(szName);
				if (pControl)
					pControl->SetHotKey(cVK);
			}
		}
	}
}

void CDnMainDlg::ShowTimeEventAlarm(bool bShow)
{
	m_bShowTimeEventAlarm = bShow;
	for (int i = 0; i<6; i++) m_pStaticTimeEventIcon[i]->Show(bShow);
	m_pStaticTimeEventText->Show(bShow);
	if (bShow == false) m_fTimeEventHideDelta = 0.f;
}

void CDnMainDlg::UpdateTimeEventAlarm(char cType, int nLastEventID)
{
	if (!CDnTimeEventTask::IsActive()) return;

	m_cUpdateEventType = cType;
	m_nTimeEventID = nLastEventID;
	m_fTimeEventHideDelta = 10.f;

	switch (cType) {
	case 0: // 완료
		if (nLastEventID > 0) {
			WCHAR szTooltip[256] = L"";
			CDnTimeEventTask *pTask = &CDnTimeEventTask::GetInstance();
			CDnTimeEventTask::TimeEventInfoStruct *pTimeEvent = pTask->FindTimeEventInfo(nLastEventID);
			if (pTimeEvent)
				swprintf_s(szTooltip, GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4834), pTimeEvent->szTitle.c_str());
			m_pStaticTimeEventIcon[4]->SetTooltipText(szTooltip);
		}
		break;
	case -1: // 삭제
		break;
	}
}

void CDnMainDlg::ProcessTimeEventAlarm(float fElapsedTime)
{
	int nIconState = -1;
	bool bBlink = false;
	WCHAR szRemainTime[64] = L"";


	if (m_bShowTimeEventAlarm) {
		if (!CDnTimeEventTask::IsActive()) return;

		if (CDnWorld::GetInstance().GetMapType() == CDnWorld::MapTypeDungeon) {
			CDnGameTask *pGameTask = (CDnGameTask *)CTaskManager::GetInstance().GetTask("GameTask");
			if (pGameTask && !pGameTask->IsSyncComplete()) {
				for (int i = 0; i<6; i++) {
					m_pStaticTimeEventIcon[i]->SetBlink(false);
					m_pStaticTimeEventIcon[i]->Show(false);
				}
				m_pStaticTimeEventText->SetText(L"");
				return;
			}
		}
		if (m_fTimeEventHideDelta > 0.f) {
			m_fTimeEventHideDelta -= fElapsedTime;
			switch (m_cUpdateEventType) {
			case -1: nIconState = 5; break;
			case 0: nIconState = 4; break;
			}
			if (m_fTimeEventHideDelta <= 0.f) {
				m_cUpdateEventType = -1;
				m_nTimeEventID = 0;
				m_fTimeEventHideDelta = 0.f;
			}
		}
		else {
			CDnTimeEventTask *pTask = &CDnTimeEventTask::GetInstance();
			int nEventID = pTask->GetTimeEventAlarm();
			if (nEventID > 0) {
				CDnTimeEventTask::TimeEventInfoStruct *pTimeEvent = pTask->FindTimeEventInfo(nEventID);
				if (pTimeEvent) {
					float fProgress = (1.f / (float)(pTimeEvent->nMaintenanceTime / 1000)) * (float)(pTimeEvent->nRemainTime / 1000);
					if (fProgress > 0.75f) nIconState = 0;
					else if (fProgress > 0.5f) nIconState = 1;
					else if (fProgress > 0.25f) nIconState = 2;
					else if (fProgress > 0.f) nIconState = 3;
					else if (fProgress <= 0.f) nIconState = 4;

					int nValue = (int)(pTimeEvent->nRemainTime / 1000);
					int nHour = nValue / 3600;
					int nMin = (nValue % 3600) / 60;
					int nSec = nValue % 60;

					if (fProgress >= 0.f) {
						//swprintf_s( szRemainTime, L"%02d:%02d:%02d", nHour, nMin, nSec );
						_snwprintf_s(szRemainTime, _countof(szRemainTime), _TRUNCATE, L"%02d:%02d:%02d", nHour, nMin, nSec);
					}
					if (pTimeEvent->nRemainTime > 0 && (pTimeEvent->nRemainTime / 1000) < 60 * 5) bBlink = true;

					if (nIconState > -1 && fProgress > 0.f) {
						WCHAR szTooltip[1024] = L"";
						std::wstring szRewardStr;
						int nCount = (int)pTimeEvent->pVecRewardItemList.size();
						for (int i = 0; i<nCount; i++) {
							szRewardStr += pTimeEvent->pVecRewardItemList[i]->GetName();
							if (i != nCount - 1) szRewardStr += L", ";
						}
#ifdef _US
						_snwprintf_s(szTooltip, _countof(szTooltip), _TRUNCATE, GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4831), pTimeEvent->szTitle.c_str(), nHour, nMin, nSec,
							pTimeEvent->BeginTime.GetMonth(), pTimeEvent->BeginTime.GetDay(), pTimeEvent->BeginTime.GetYear(), pTimeEvent->BeginTime.GetHour(), pTimeEvent->BeginTime.GetMinute(),
							pTimeEvent->EndTime.GetMonth(), pTimeEvent->EndTime.GetDay(), pTimeEvent->EndTime.GetYear(), pTimeEvent->EndTime.GetHour(), pTimeEvent->EndTime.GetMinute(), szRewardStr.c_str());

#elif _RU
						_snwprintf_s(szTooltip, _countof(szTooltip), _TRUNCATE, GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4831), pTimeEvent->szTitle.c_str(), nHour, nMin, nSec,
							pTimeEvent->BeginTime.GetDay(), pTimeEvent->BeginTime.GetMonth(), pTimeEvent->BeginTime.GetYear(), pTimeEvent->BeginTime.GetHour(), pTimeEvent->BeginTime.GetMinute(),
							pTimeEvent->EndTime.GetDay(), pTimeEvent->EndTime.GetMonth(), pTimeEvent->EndTime.GetYear(), pTimeEvent->EndTime.GetHour(), pTimeEvent->EndTime.GetMinute(), szRewardStr.c_str());
#else // _US
						_snwprintf_s(szTooltip, _countof(szTooltip), _TRUNCATE, GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4831), pTimeEvent->szTitle.c_str(), nHour, nMin, nSec,
							pTimeEvent->BeginTime.GetYear(), pTimeEvent->BeginTime.GetMonth(), pTimeEvent->BeginTime.GetDay(), pTimeEvent->BeginTime.GetHour(), pTimeEvent->BeginTime.GetMinute(),
							pTimeEvent->EndTime.GetYear(), pTimeEvent->EndTime.GetMonth(), pTimeEvent->EndTime.GetDay(), pTimeEvent->EndTime.GetHour(), pTimeEvent->EndTime.GetMinute(), szRewardStr.c_str());
#endif // _US
						if (nIconState > -1) m_pStaticTimeEventIcon[nIconState]->SetTooltipText(szTooltip);
					}
				}
			}
		}
	}

	for (int i = 0; i<6; i++) {
		m_pStaticTimeEventIcon[i]->SetBlink((i == nIconState) ? bBlink : false);
		m_pStaticTimeEventIcon[i]->Show((i == nIconState) ? true : false);
	}
	m_pStaticTimeEventText->SetText(szRemainTime);
}

bool CDnMainDlg::IsShowGuildWarSkillSlotDlg()
{
	if (m_pDnGuildWarSkillSlotDlg)
		return m_pDnGuildWarSkillSlotDlg->IsShow();

	return false;
}

void CDnMainDlg::ShowGuildWarSkillSlotDlg(bool bShow, bool bFixedGuildSkill)
{
	if (m_pDnGuildWarSkillSlotDlg)
	{
		m_pDnGuildWarSkillSlotDlg->Show(bShow);
		m_pDnGuildWarSkillSlotDlg->SetFixedMode(bFixedGuildSkill);
	}
}

void CDnMainDlg::UseGuildWarSkill(int nIndex)
{
	if (m_pDnGuildWarSkillSlotDlg)
		m_pDnGuildWarSkillSlotDlg->UseGuildWarSkill(nIndex);
}

#ifdef PRE_TEST_ANIMATION_UI
bool CDnMainDlg::IsShowAniTestDlg() const
{
	return (m_pAniTestDlg && m_pAniTestDlg->IsShow());
}
#endif

#ifdef PRE_ADD_LEVELUP_GUIDE
void CDnMainDlg::SetLevelUpSkillAlarm(bool bSet)
{
	m_bLevelUpSkillAlarm = bSet;
	m_fLevelUpSkillAlarmTime = 0.f;

	BlinkMenuButton(CDnMainMenuDlg::SKILL_DIALOG);
}
#endif

#ifdef PRE_SPECIALBOX
void CDnMainDlg::SetEventReceiverAlarm(bool bNew)
{
	if (bNew == true)
	{
		m_fEventReceiverAlarmTime = 0.f;
		m_bEventReceiverAlarm = true;
		m_pButtonEventReceiverAlarm->SetBlink(true);
	}

	m_pButtonEventReceiverAlarm->Show(true);
	CDnEventReceiverTabDlg::ResetDelayTime();
}

void CDnMainDlg::CloseEventReceiverAlarm()
{
	m_fEventReceiverAlarmTime = 0.f;
	m_bEventReceiverAlarm = false;

	m_pButtonEventReceiverAlarm->Show(false);
	//m_pStaticEventReceiverBalloon->Show(false);
	//m_pStaticEventReceiverText->Show(false);
}
#endif


void CDnMainDlg::ChangeSkillSlotPage(int nPage)
{
	if (0 > nPage || nPage >= QUICKSLOT_SKILL_PAGE_COUNT)
		return;
	if (m_nSkillSlotPage == nPage)
		return;

	m_nSkillSlotPage = nPage;

	if (m_bLockQuickSlot)
		return;

	int nDefaultDlgID = QUICKSLOT_SKILL_PAGE_COUNT * nPage;
	m_dwPrevQuickDlgID = nDefaultDlgID;

	if (!m_QuickSlgDlgGroup.IsShowDialog(LIFESKILL_DLG))
	{
		m_QuickSlgDlgGroup.ShowDialog((DWORD)nDefaultDlgID, true);
		m_pStaticSlotNumber->SetIntToText((nDefaultDlgID - (m_nSkillSlotPage * QUICKSLOT_PAGE_COUNT)) + 1);
	}

	ShowExpDialog(false);
	ShowFTGDialog(false);
}

bool CDnMainDlg::IsCurrentSkillPageIndex(int nPageIndex)
{
	if (m_nSkillSlotPage < DualSkill::Type::Primary || m_nSkillSlotPage > DualSkill::Type::Secondary)
		return false;

	if ((nPageIndex >= ((m_nSkillSlotPage * QUICKSLOT_SKILL_PAGE_COUNT) + 0)) && nPageIndex < ((m_nSkillSlotPage * QUICKSLOT_SKILL_PAGE_COUNT) + QUICKSLOT_PAGE_COUNT))
		return true;

	return false;
}

#ifdef PRE_FIX_HOLD_COSTUMEMIX_INVEN
void CDnMainDlg::HoldToggleShowDialogOnProcessButton(bool bHold, DWORD dwDialogId)
{
	m_HoldToggleShowDialogOnProcessButton = bHold ? (int)dwDialogId : -1;
}
#endif

void CDnMainDlg::SetPetFoodInfo(int nPetFoodItemID)
{
	if (m_pFoodItemSlot == NULL)
		return;

	ResetPetFoodInfo();
	int nPetFoodItemCount = GetPetTask().GetPetFoodItemCount(nPetFoodItemID);
	if (nPetFoodItemCount == 0)
		return;

	TItemInfo itemInfo;
	if (CDnItem::MakeItemInfo(nPetFoodItemID, 1, itemInfo))
	{
		m_pFoodItem = GetItemTask().CreateItem(itemInfo);

		if (m_pFoodItem && m_pFoodItem->GetItemType() == ITEMTYPE_PET_FOOD)
		{
			m_pFoodItemSlot->SetItem(m_pFoodItem, CDnSlotButton::NO_COUNTABLE_RENDER);
			m_pFoodItemSlot->Show(true);
			WCHAR szPetFoodCount[12] = L"";
			swprintf_s(szPetFoodCount, L"%d", nPetFoodItemCount);
			m_pStaticFoodItemCount->SetText(szPetFoodCount);
			m_pStaticFoodItemCount->Show(true);
		}
	}
}

void CDnMainDlg::RefreshPetFoodCount()
{
	if (m_pFoodItem == NULL)
		return;

	int nPetFoodItemCount = GetPetTask().GetPetFoodItemCount(GetPetTask().GetPetOption().m_nFoodItemID);
	if (nPetFoodItemCount == 0)
	{
		ResetPetFoodInfo();
	}
	else if (nPetFoodItemCount > 0)
	{
		WCHAR szPetFoodCount[12] = L"";
		swprintf_s(szPetFoodCount, L"%d", nPetFoodItemCount);
		m_pStaticFoodItemCount->SetText(szPetFoodCount);
	}
}

void CDnMainDlg::ResetPetFoodInfo()
{
	SAFE_DELETE(m_pFoodItem);
	m_pFoodItemSlot->ResetSlot();
	m_pFoodItemSlot->Show(false);
	m_pStaticFoodItemCount->SetText(L"");
	m_pStaticFoodItemCount->Show(false);
}

#if defined(PRE_ADD_68286)
bool CDnMainDlg::IsAlarmIconClick(POINT& MousePoint)
{
	bool isAlarmIconClick = false;

	float fMouseX, fMouseY;
	PointToFloat(MousePoint, fMouseX, fMouseY);

	if (m_pStaticPost && m_pStaticPost->IsShow() && m_pStaticPost->IsInside(fMouseX, fMouseY) == true)
	{
		isAlarmIconClick = true;
	}

	if (m_pStaticCashShopGift && m_pStaticCashShopGift->IsShow() && m_pStaticCashShopGift->IsInside(fMouseX, fMouseY) == true)
	{
		isAlarmIconClick = true;
	}

	return isAlarmIconClick;
}
#endif // PRE_ADD_68286

#ifdef PRE_MOD_APPELLATIONBOOK_RENEWAL

void CDnMainDlg::ShowAppellationNotify(bool bShow)
{
	m_pStaticNewAppellationAlarm->Show(bShow);
	//m_pStaticNewAppellationAlarmText->Show(bShow);
}
#endif // PRE_MOD_APPELLATIONBOOK_RENEWAL

#ifdef PRE_ADD_STAMPSYSTEM
void CDnMainDlg::NpcTalk(int npcID)
{
	// #80220 - 현재 퀘스트 과제 버튼 클릭 시 해당 NPC와 대화를 해야만 안내 화살표가 사라지는 것을, 아무 NPC와 대화를 해도 사라지도록 수정합니다.
	//	if( !m_bSearching && m_SearchedNpcID != npcID )
	//		return;

	FindComplete();
}

void CDnMainDlg::NPCSearch(int mapID, int npcID)
{
	if (m_bSearching && m_SearchedNpcID == npcID)
		return;

	// 세인트헤이븐 에서만 작동.
	if (CGlobalInfo::GetInstance().m_nCurrentMapIndex != mapID)
	{
		m_SearchedNpcID = -1;
		m_bSearching = false;

		GetInterface().ShowCaptionDialog(CDnInterface::emCAPTION_TYPE::typeCaption2, 8320, textcolor::YELLOW); // "현재 위치에서는 NPC 안내가 불가능합니다"
		return;
	}


	m_SearchedNpcID = npcID;
	m_bSearching = false;

	CDnWorldData * pWorldData = CDnWorldDataManager::GetInstance().GetWorldData(CGlobalInfo::GetInstance().m_nCurrentMapIndex);
	if (!pWorldData)
		return;

	EtVector2 vPos(0.0f, 0.0f);
	int nSize = pWorldData->GetNpcCount();
	int i = 0;
	for (i = 0; i<nSize; ++i)
	{
		CDnWorldData::NpcData * pNpcData = pWorldData->GetNpcData(i);
		if (pNpcData->nNpcID == npcID)
		{
			m_bSearching = true;
			vPos = pNpcData->vPos;
			break;
		}
		if (i == nSize)
		{
			CDnNPCActor * pNpcActor = CDnNPCActor::FindNpcActorFromID(npcID);
			if (pNpcActor)
			{
				m_bSearching = true;
				vPos = EtVector2(pNpcActor->GetPosition()->x, pNpcActor->GetPosition()->z);
			}
		}
	}

	m_vNPCPos.x = vPos.x;
	m_vNPCPos.y = 0.0f;
	m_vNPCPos.z = vPos.y;

	if (m_bSearching)
	{
		GetInterface().ShowCaptionDialog(CDnInterface::emCAPTION_TYPE::typeCaption2, 8319, textcolor::YELLOW); // "퀘스트 NPC 안내를 시작합니다"

		m_NPCArrowDlg->Show(true);

		/*// test.
		DNTableFileFormat * pTable = GetDNTable( CDnTableDB::TNPC );
		if( pTable )
		{
		DNTableCell * pCell = pTable->GetFieldFromLablePtr( npcID, "_NameID" );
		if( pCell )
		GetInterface().AddChatMessage( eChatType::CHATTYPE_NORMAL, L"", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, pCell->GetInteger() ) );
		}	*/


	}

	else
	{
		// 세인트헤이븐 에서만 작동.	
		m_SearchedNpcID = -1;
		m_bSearching = false;

		//GetInterface().ShowCaptionDialog( CDnInterface::emCAPTION_TYPE::typeCaption2, 8320, textcolor::YELLOW ); // "현재 위치에서는 NPC 안내가 불가능합니다"		
		GetInterface().ShowCaptionDialog(CDnInterface::emCAPTION_TYPE::typeCaption2, L"세인트헤이븐에 존재하지 않는 NPC입니다 ( String ID 필요 )", textcolor::YELLOW); // 
	}

}

void CDnMainDlg::RotetaNPCArrow()
{
	if (!CDnActor::s_hLocalActor)
		return;

	DnCameraHandle hCamera = CDnCamera::GetActiveCamera();
	if (hCamera)
	{
		EtVector3 vZDir = hCamera->GetMatEx()->m_vZAxis;
		m_camDir = vZDir;

		vZDir.y = 0.0f;
		EtVec3Normalize(&vZDir, &vZDir);

		EtVector3 vCharPos = *CDnActor::s_hLocalActor->GetPosition();
		vCharPos.y = 0.0f;
		EtVector3 vDir = m_vNPCPos - vCharPos;

		// test.
		//GetInterface().AddChatMessage( eChatType::CHATTYPE_NORMAL, L"", FormatW( L"%f", EtVec3Length( &vDir ) ).c_str() );

		EtVector3 vNor;
		EtVec3Normalize(&vNor, &vDir);

		// 회전각.
		float fDegree = EtAcos(EtVec3Dot(&vZDir, &vNor));

		if (EtVec3Dot(&(hCamera->GetMatEx()->m_vXAxis), &vNor) < 0.0f)
			fDegree = -fDegree;

		m_NPCArrowDlg->GetStaticArrow()->SetRotate(EtToDegree(fDegree));
	}
}


void CDnMainDlg::FindComplete()
{
	m_SearchedNpcID = -1;
	m_bSearching = false; // 찾는중
	m_fRotDegree = 0.0f;

	if (m_NPCArrowDlg)
		m_NPCArrowDlg->Show(false);
}

#endif // PRE_ADD_STAMPSYSTEM

void CDnMainDlg::OnLevelUp(int nLevel)
{
#if defined( PRE_ALTEIAWORLD_EXPLORE )
	m_pMainBar_EventDlg->SetAlteaDiceIcon();
#endif	// #if defined( PRE_ALTEIAWORLD_EXPLORE )
}
void CDnMainDlg::SetAlteaDiceIcon()
{
	m_pMainBar_EventDlg->SetAlteaDiceIcon();
}

void CDnMainDlg::SetBubble(int Count, float fExpireTime, float fTotalTime)
{
//	if (m_pMainBar_BubbleGaugeDlg)
//		m_pMainBar_BubbleGaugeDlg->SetBubble(Count, fExpireTime, fTotalTime);
}



void CDnMainDlg::OnUICallbackProc(int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg)
{
	GetInterface().MessageBox(FormatW(L" got id = %d ", nID).c_str());

	if (nID == 1)
	{
		if (nCommand == EVENT_BUTTON_CLICKED)
		{
			if (strcmp(pControl->GetControlName(), "ID_YES") == 0)
			{
				//rlkt_test pvp farm travel
				GetInterface().MessageBox(L"Traveling to farm pvp!");
			}
		}
	}

}

#else
CDnMainDlg::CDnMainDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback)
	: CDnCustomDlg(dialogType, pParentDialog, nID, pCallback)
{
	m_pStaticSlotNumber = NULL;

	m_pExpDlg = NULL;
	m_pStaticBackLight = NULL;
	m_pStaticBackBlack = NULL;

	m_dwShowQuickDlgID = 0;
	m_dwPrevQuickDlgID = 0;
	m_dwOrignalShowQuickSlotDlgID = 0;
	m_dwOrignalPrevQuickSlotDlgID = 0;

	m_fDegree = 0.0f;
	m_pStaticExp = NULL;

	m_pStaticPost = NULL;
	m_pStaticCashShopGift = NULL;
	m_pStaticCashShopGiftBalloon = NULL;
	m_pStaticCashShopGiftText = NULL;
	m_pStaticHarvest = NULL;
	m_pStaticCheckAttendanceBalloon = NULL;
	m_pStaticCheckAttendanceText = NULL;
#ifdef PRE_ADD_LEVELUP_REWARD_NOTIFIER
	m_pStaticLevelUpNotifier = NULL;
#endif 
	m_bCheckAttendanceFirst = false;
	m_fCheckAttendanceAlarmTime = 0.0f;
	m_bNotifyCheckAttendance = false;

#ifdef PRE_REMOVE_MINISITE
#else
	m_pNewMiniSiteBtn = NULL;
#endif
	m_fMiniSiteAlarmTime = eMiniSiteBlinkTime;
	m_bNotifyCheckMiniSite = false;

	m_bCashShopGiftAlarm = false;
	m_fCashShopGiftAlarmTime = 0.f;

	m_bHarvestAlarm = false;
	m_fHarvestAlarmTime = 0.f;
	m_nHarvestCount = 0;

	m_bMailAlarm = false;
	m_fMailAlarmTime = 0.0f;

#ifdef PRE_ADD_LEVELUP_REWARD_NOTIFIER
	m_fLevelUpboxAlarmTime = 0.0f;
	m_bLevelUpBoxAlarm = false;
#endif 

	m_bPVP = false;
	m_pPassiveSkillSlotDlg = NULL;
	m_bShowOption = false;

	m_bLockQuickSlot = false;

	m_pPopupFTGDlg = NULL;
	m_pDnGuildWarSkillSlotDlg = NULL;

	memset(m_nMainDlgSoundIndex, 0, sizeof(m_nMainDlgSoundIndex));

	m_bEnableButtons = true;

	memset(m_pStaticTimeEventIcon, 0, sizeof(m_pStaticTimeEventIcon));
	m_pStaticTimeEventText = NULL;
	m_cUpdateEventType = -1;
	m_nTimeEventID = 0;
	m_fTimeEventHideDelta = 0.f;
	m_bShowTimeEventAlarm = true;

#ifdef PRE_ADD_LEVELUP_GUIDE
	m_pStaticLevelUpSkillAlarm = NULL;
	m_pStaticLevelUpSkillAlarmText = NULL;
	m_bLevelUpSkillAlarm = false;
	m_fLevelUpSkillAlarmTime = 0.f;
#endif

#ifdef PRE_TEST_ANIMATION_UI
	m_pAniTestBtn = NULL;
	m_pAniTestDlg = NULL;
#endif

#ifdef PRE_ADD_AUTO_DICE
	m_bShowDiceOption = false;
#endif
	m_nSkillSlotPage = 0;
#ifdef PRE_FIX_HOLD_COSTUMEMIX_INVEN
	m_HoldToggleShowDialogOnProcessButton = -1;
#endif
	m_pPCRoomFTGGauge = NULL;
	m_pStaticEventFTG = NULL;
	m_pStaticEventFTGFont = NULL;
	m_pStaticFTG = NULL;
	m_pStaticFTGFont = NULL;
	m_pStaticPCRoomFTG = NULL;
	m_pStaticPCRoomFTGFont = NULL;
	m_pStaticVIPFTG = NULL;
	m_pStaticVIPFTGFont = NULL;
	m_pStaticWeekFTG = NULL;
	m_pStaticWeekFTGFont = NULL;
	m_pVIPFTGGauge = NULL;
	m_pWeekFTGGauge = NULL;
	m_pEventFTGGauge = NULL;
	m_pFTGGauge = NULL;

	m_pFoodItem = NULL;
	m_pFoodItemSlot = NULL;
	m_pStaticFoodItemCount = NULL;

#ifdef PRE_ADD_MAIL_UI_IMPROVEMENTS
	m_pStaticMailText = NULL;
	m_pStaticMailBallon = NULL;
#endif
#ifdef PRE_ADD_REMOTE_QUEST
	m_bShowQuestNotify = false;
	m_pStaticQuestText = NULL;
	m_pStaticQuestBallon = NULL;
#endif // PRE_ADD_REMOTE_QUEST
#ifdef PRE_SPECIALBOX
	m_pButtonEventReceiverAlarm = NULL;
	m_pStaticEventReceiverBalloon = NULL;
	m_pStaticEventReceiverText = NULL;
	m_fEventReceiverAlarmTime = 0.f;
	m_bEventReceiverAlarm = false;
#endif

#ifdef PRE_MOD_APPELLATIONBOOK_RENEWAL
	m_pStaticNewAppellationAlarm = NULL;
	m_pStaticNewAppellationAlarmText = NULL;
#endif

#if defined( PRE_ALTEIAWORLD_EXPLORE )
	m_pAlteaDiceIcon = NULL;
	m_bAlteaDiceShow = false;
	m_bAlteaDiceAlarm = false;
	m_fAlteaDiceAlarmTime = 0.0f;
#endif	// #if defined( PRE_ALTEIAWORLD_EXPLORE )

#ifdef PRE_ADD_STAMPSYSTEM
	m_NPCArrowDlg = NULL;
	m_bSearching = false; // 찾는중
	m_SearchedNpcID = -1;
	m_camDir.x = m_camDir.y = m_camDir.z = 0.0f;
	m_vNPCPos.x = m_vNPCPos.y = m_vNPCPos.z = 0.0f;
	m_fRotDegree = 0.0f;
#endif // PRE_ADD_STAMPSYSTEM
}

CDnMainDlg::~CDnMainDlg(void)
{
	SAFE_RELEASE_SPTR(m_hBackLight);
	SAFE_RELEASE_SPTR(m_hBackBlack);
	SAFE_DELETE(m_pExpDlg);
	SAFE_DELETE(m_pPassiveSkillSlotDlg);
	SAFE_RELEASE_SPTR(m_hNorthTexture);
	SAFE_DELETE(m_pPopupFTGDlg);
	SAFE_DELETE(m_pDnGuildWarSkillSlotDlg);
#ifdef PRE_TEST_ANIMATION_UI
	SAFE_DELETE(m_pAniTestDlg);
#endif
	SAFE_DELETE(m_pFoodItem);

#ifdef PRE_ADD_STAMPSYSTEM
	FindComplete();
	SAFE_DELETE(m_NPCArrowDlg);
#endif // PRE_ADD_STAMPSYSTEM

}

void CDnMainDlg::Initialize(bool bShow)
{
	CEtUIDialog::Initialize(CEtResourceMng::GetInstance().GetFullName("MainBar.ui").c_str(), bShow);

	const char *szFileName = CDnTableDB::GetInstance().GetFileName(10033);
	if (strlen(szFileName) > 0)
		m_nMainDlgSoundIndex[eSOUND_MAILALARM] = CEtSoundEngine::GetInstance().LoadSound(CEtResourceMng::GetInstance().GetFullName(szFileName).c_str(), false, false);

	szFileName = CDnTableDB::GetInstance().GetFileName(10043);
	if (strlen(szFileName) > 0)
		m_nMainDlgSoundIndex[eSOUND_CASHSHOPGIFT] = CEtSoundEngine::GetInstance().LoadSound(CEtResourceMng::GetInstance().GetFullName(szFileName).c_str(), false, false);
}

void CDnMainDlg::InitialUpdate()
{
	/////////////////////////////////////////////////////////////////////////////
	// Note : QuickSlot Dialog 생성
	CDnQuickSlotDlg *pQuickSlotDlg(NULL);

	pQuickSlotDlg = new CDnQuickSlotDlg(UI_TYPE_CHILD, this, QUICKSLOT_DLG_00);
	pQuickSlotDlg->Initialize(false);
	m_QuickSlgDlgGroup.AddDialog(QUICKSLOT_DLG_00, pQuickSlotDlg);

	pQuickSlotDlg = new CDnQuickSlotDlg(UI_TYPE_CHILD, this, QUICKSLOT_DLG_01);
	pQuickSlotDlg->Initialize(false);
	m_QuickSlgDlgGroup.AddDialog(QUICKSLOT_DLG_01, pQuickSlotDlg);

	pQuickSlotDlg = new CDnQuickSlotDlg(UI_TYPE_CHILD, this, QUICKSLOT_SECOND_DLG_00);
	pQuickSlotDlg->Initialize(false);
	m_QuickSlgDlgGroup.AddDialog(QUICKSLOT_SECOND_DLG_00, pQuickSlotDlg);

	pQuickSlotDlg = new CDnQuickSlotDlg(UI_TYPE_CHILD, this, QUICKSLOT_SECOND_DLG_01);
	pQuickSlotDlg->Initialize(false);
	m_QuickSlgDlgGroup.AddDialog(QUICKSLOT_SECOND_DLG_01, pQuickSlotDlg);

	pQuickSlotDlg = new CDnQuickSlotDlg(UI_TYPE_CHILD, this, QUICKSLOT_EVENT);
	pQuickSlotDlg->Initialize(false);
	m_QuickSlgDlgGroup.AddDialog(QUICKSLOT_EVENT, pQuickSlotDlg);


	CDnLifeSkillQuickSlotDlg *pGestureQuickSlotDlg(NULL);
	pGestureQuickSlotDlg = new CDnLifeSkillQuickSlotDlg(UI_TYPE_CHILD, this, LIFESKILL_DLG);
	pGestureQuickSlotDlg->Initialize(false);
	m_QuickSlgDlgGroup.AddDialog(LIFESKILL_DLG, pGestureQuickSlotDlg);

	m_QuickSlgDlgGroup.ShowDialog(QUICKSLOT_DLG_00, true);

	m_pStaticSlotNumber = GetControl<CEtUIStatic>("ID_STATIC_SLOT");
	m_pStaticSlotNumber->SetText(L"1");
	m_pStaticSlotNumber->Enable(true);

	m_pExpDlg = new CDnExpDlg(UI_TYPE_CHILD, this);
	m_pExpDlg->Initialize(false);

	m_pExpGauge[0] = GetControl<CDnExpGauge>("ID_EXP_GAUGE0");
	m_pExpGauge[1] = GetControl<CDnExpGauge>("ID_EXP_GAUGE1");
	m_pExpGauge[2] = GetControl<CDnExpGauge>("ID_EXP_GAUGE2");
	m_pExpGauge[3] = GetControl<CDnExpGauge>("ID_EXP_GAUGE3");
	m_pExpGauge[4] = GetControl<CDnExpGauge>("ID_EXP_GAUGE4");
	m_pStaticExp = GetControl<CEtUIStatic>("ID_STATIC_EXP");
	m_pExpGauge[0]->UseGaugeEndElement(1);
	m_pExpGauge[1]->UseGaugeEndElement(1);
	m_pExpGauge[2]->UseGaugeEndElement(1);
	m_pExpGauge[3]->UseGaugeEndElement(1);
	m_pExpGauge[4]->UseGaugeEndElement(1);

	m_pFTGGauge = GetControl<CDnExpGauge>("ID_FTG_GAUGE");
	m_pStaticFTGFont = GetControl<CEtUIStatic>("ID_STATIC_FTGFONT");
	m_pStaticFTG = GetControl<CEtUIStatic>("ID_STATIC_FTG");
	m_pFTGGauge->UseShadeProcessBar(3, 2);
	m_pFTGGauge->UseGaugeEndElement(1);

	m_pWeekFTGGauge = GetControl<CDnExpGauge>("ID_FWG_GAUGE");
	m_pStaticWeekFTGFont = GetControl<CEtUIStatic>("ID_STATIC_FWGFONT");
	m_pStaticWeekFTG = GetControl<CEtUIStatic>("ID_STATIC_FWG");
	m_pWeekFTGGauge->UseShadeProcessBar(3, 2);
	m_pWeekFTGGauge->UseGaugeEndElement(1);

	m_pPCRoomFTGGauge = GetControl<CDnExpGauge>("ID_FPG_GAUGE");
	m_pStaticPCRoomFTGFont = GetControl<CEtUIStatic>("ID_STATIC_FPGFONT");
	m_pStaticPCRoomFTG = GetControl<CEtUIStatic>("ID_STATIC_FPG");
	m_pPCRoomFTGGauge->UseShadeProcessBar(3, 2);
	m_pPCRoomFTGGauge->UseGaugeEndElement(1);

	m_pEventFTGGauge = GetControl<CDnExpGauge>("ID_FEG_GAUGE");
	m_pStaticEventFTGFont = GetControl<CEtUIStatic>("ID_STATIC_FEGFONT");
	m_pStaticEventFTG = GetControl<CEtUIStatic>("ID_STATIC_FEG");
	m_pEventFTGGauge->UseShadeProcessBar(3, 2);
	m_pEventFTGGauge->UseGaugeEndElement(1);

	m_pVIPFTGGauge = GetControl<CDnExpGauge>("ID_FVG_GAUGE");
	m_pStaticVIPFTGFont = GetControl<CEtUIStatic>("ID_STATIC_FVGFONT");
	m_pStaticVIPFTG = GetControl<CEtUIStatic>("ID_STATIC_FVG");

#ifdef PRE_ADD_VIP
	m_pVIPFTGGauge->UseShadeProcessBar(3, 2);
	m_pVIPFTGGauge->UseGaugeEndElement(1);
#else
	if (m_pVIPFTGGauge)
		m_pVIPFTGGauge->Show(false);
	if (m_pStaticVIPFTGFont)
		m_pStaticVIPFTGFont->Show(false);
	if (m_pStaticVIPFTG)
		m_pStaticVIPFTG->Show(false);
#endif

	m_pPopupFTGDlg = new CDnPopupFatigueDlg(UI_TYPE_TOP_MSG);	// 미니맵 외곽에 나오는 아이콘때문에 Child로 그리면 가려서 안된다.
	m_pPopupFTGDlg->Initialize(false);

	m_pDnGuildWarSkillSlotDlg = new CDnGuildWarSkillSlotDlg(UI_TYPE_CHILD, this);
	m_pDnGuildWarSkillSlotDlg->Initialize(false);

	m_hBackLight = LoadResource(CEtResourceMng::GetInstance().GetFullName("MainBarLight.dds").c_str(), RT_TEXTURE);
	m_hBackBlack = LoadResource(CEtResourceMng::GetInstance().GetFullName("MainBarBlack.dds").c_str(), RT_TEXTURE);

	m_pStaticBackLight = GetControl<CEtUIStatic>("ID_BACK_LIGHT");
	m_pStaticBackLight->Show(false);
	m_pStaticBackBlack = GetControl<CEtUIStatic>("ID_BACK_BLACK");
	m_pStaticBackBlack->Show(false);
#ifdef PRE_ADD_MAIL_UI_IMPROVEMENTS
	m_pStaticMailText = GetControl<CEtUIStatic>("ID_TEXT_MAIL");
	m_pStaticMailText->Show(false);
	m_pStaticMailBallon = GetControl<CEtUIStatic>("ID_STATIC_MAIL");
	m_pStaticMailBallon->Show(false);
#endif 
	m_pStaticPost = GetControl<CEtUIStatic>("ID_STATIC_POST");
	m_pStaticPost->Show(false);

	m_pStaticCashShopGift = GetControl<CEtUIStatic>("ID_STATIC_CPRESENT");
	m_pStaticCashShopGift->Show(false);
	m_pStaticCashShopGiftBalloon = GetControl<CEtUIStatic>("ID_STATIC_CPCHAT");
	m_pStaticCashShopGiftBalloon->Show(false);
	m_pStaticCashShopGiftText = GetControl<CEtUIStatic>("ID_TEXT_CPCHAT");
	m_pStaticCashShopGiftText->Show(false);

#ifdef PRE_ADD_LEVELUP_REWARD_NOTIFIER
	m_pStaticLevelUpNotifier = GetControl<CEtUIButton>("ID_BT_LVPOST");
	m_pStaticLevelUpNotifier->Show(false);
#endif 
	m_pStaticHarvest = GetControl<CEtUIStatic>("ID_STATIC_LIFE");
	m_pStaticHarvest->Show(false);

#ifdef PRE_ADD_REMOTE_QUEST
	m_pStaticQuestText = GetControl<CEtUIStatic>("ID_TEXT_QUESTNOTICE");
	m_pStaticQuestText->Show(false);
	m_pStaticQuestBallon = GetControl<CEtUIStatic>("ID_STATIC_QUESTNOTICE");
	m_pStaticQuestBallon->Show(false);
#endif // PRE_ADD_REMOTE_QUEST

	SButtonInfo buttonInfo;
	buttonInfo.pButton = GetControl<CDnMenuButton>("ID_CHAR_STATUS");
	buttonInfo.nDialogID = CDnMainMenuDlg::CHARSTATUS_DIALOG;
	m_vecButtonInfo.push_back(buttonInfo);

	buttonInfo.pButton = GetControl<CDnMenuButton>("ID_INVENTORY");
	buttonInfo.nDialogID = CDnMainMenuDlg::INVENTORY_DIALOG;
	m_vecButtonInfo.push_back(buttonInfo);

	buttonInfo.pButton = GetControl<CDnMenuButton>("ID_SKILL");
	buttonInfo.nDialogID = CDnMainMenuDlg::SKILL_DIALOG;
	m_vecButtonInfo.push_back(buttonInfo);

	buttonInfo.pButton = GetControl<CDnMenuButton>("ID_COMMUNITY");
	buttonInfo.nDialogID = CDnMainMenuDlg::COMMUNITY_DIALOG;
	m_vecButtonInfo.push_back(buttonInfo);

	buttonInfo.pButton = GetControl<CDnMenuButton>("ID_QUEST");
	buttonInfo.nDialogID = CDnMainMenuDlg::QUEST_DIALOG;
	m_vecButtonInfo.push_back(buttonInfo);

	buttonInfo.pButton = GetControl<CDnMenuButton>("ID_MAP");
	buttonInfo.nDialogID = CDnMainMenuDlg::ZONEMAP_DIALOG;
	m_vecButtonInfo.push_back(buttonInfo);

	buttonInfo.pButton = GetControl<CDnMenuButton>("ID_MISSION");
	buttonInfo.nDialogID = CDnMainMenuDlg::MISSION_DIALOG;
	m_vecButtonInfo.push_back(buttonInfo);

	buttonInfo.pButton = GetControl<CDnMenuButton>("ID_SYSTEM");
	buttonInfo.nDialogID = CDnMainMenuDlg::SYSTEM_DIALOG;
	m_vecButtonInfo.push_back(buttonInfo);

	buttonInfo.pButton = GetControl<CDnMenuButton>("ID_CHAT");
	buttonInfo.nDialogID = CDnMainMenuDlg::CHATROOM_DIALOG;
	m_vecButtonInfo.push_back(buttonInfo);

#if defined(PRE_ADD_INTEGERATE_QUEST_REPUT)
	buttonInfo.pButton = GetControl<CDnMenuButton>("ID_REPUTE");
	buttonInfo.nDialogID = CDnMainMenuDlg::InTEGRATE_QUESTNREPUTATION_DIALOG;
#ifdef PRE_ADD_NO_REPUTATION_DLG
	buttonInfo.pButton->Enable(false);
#endif
	m_vecButtonInfo.push_back(buttonInfo);
#else
#ifdef PRE_ADD_NPC_REPUTATION_SYSTEM
	buttonInfo.pButton = GetControl<CDnMenuButton>("ID_REPUTE");
	buttonInfo.nDialogID = CDnMainMenuDlg::NPC_REPUTATION_TAB_DIALOG;
#ifdef PRE_ADD_NO_REPUTATION_DLG
	buttonInfo.pButton->Enable(false);
#endif
	m_vecButtonInfo.push_back(buttonInfo);
#endif // #ifdef PRE_ADD_NPC_REPUTATION_SYSTEM
#endif // PRE_ADD_INTEGERATE_QUEST_REPUT

#if defined(PRE_ADD_INTEGERATE_EVENTUI)
	buttonInfo.pButton = GetControl<CDnMenuButton>("ID_EVENT");
	buttonInfo.nDialogID = CDnMainMenuDlg::INTEGRATE_EVENT_DIALOG;
	m_vecButtonInfo.push_back(buttonInfo);
	GetControl<CDnMenuButton>("ID_EVENT")->Show(true);

#else
	buttonInfo.pButton = GetControl<CDnMenuButton>("ID_EVENT");
	buttonInfo.nDialogID = CDnMainMenuDlg::TIMEEVENT_DIALOG;
	m_vecButtonInfo.push_back(buttonInfo);
	GetControl<CDnMenuButton>("ID_EVENT")->Show(true);

	buttonInfo.pButton = GetControl<CDnMenuButton>("ID_CHECK");
	buttonInfo.nDialogID = CDnMainMenuDlg::ATTENDANCEEVENT_DIALOG;
	m_vecButtonInfo.push_back(buttonInfo);
	GetControl<CDnMenuButton>("ID_CHECK")->Show(false);
#endif // PRE_ADD_INTEGERATE_EVENTUI

#ifdef PRE_ADD_PVP_VILLAGE_ACCESS
	buttonInfo.pButton = GetControl<CDnMenuButton>("ID_COLOSSEUM");
	buttonInfo.nDialogID = CDnMainMenuDlg::PVP_VILLAGE_ACCESS;
	m_vecButtonInfo.push_back(buttonInfo);
#endif

#if defined(PRE_ADD_MAILBOX_OPEN)
	buttonInfo.pButton = GetControl<CDnMenuButton>("ID_MAIL");
	buttonInfo.nDialogID = CDnMainMenuDlg::MAIL_DIALOG;
	m_vecButtonInfo.push_back(buttonInfo);
#endif // PRE_ADD_MAILBOX_OPEN

#ifdef PRE_ADD_PVP_RANKING
	buttonInfo.pButton = GetControl<CDnMenuButton>("ID_RANK");
	buttonInfo.nDialogID = CDnMainMenuDlg::PVP_RANK_DIALOG;
	m_vecButtonInfo.push_back(buttonInfo);
	GetControl<CDnMenuButton>("ID_RANK")->Show(true);
#endif // PRE_ADD_PVP_RANKING

	m_pStaticCheckAttendanceBalloon = GetControl<CEtUIStatic>("ID_STATIC_CHECKBOX");
	m_pStaticCheckAttendanceBalloon->Show(false);
	m_pStaticCheckAttendanceText = GetControl<CEtUIStatic>("ID_TEXT_CHECK");
	m_pStaticCheckAttendanceText->Show(false);

#ifdef PRE_REMOVE_MINISITE
#else
	m_pNewMiniSiteBtn = GetControl<CEtUIButton>("ID_BT_MOVIE");
	m_pNewMiniSiteBtn->Show(true);
#endif

#if defined( PRE_ALTEIAWORLD_EXPLORE )
	m_pAlteaDiceIcon = GetControl<CEtUIButton>("ID_BT_ALTEA");
#endif	// #if defined( PRE_ALTEIAWORLD_EXPLORE )

	CDnMessageStatic *pMsgStatic(NULL);
	char szName[32] = { 0 };
	for (int i = 0; i<7; i++)
	{
		sprintf_s(szName, 32, "ID_MESSAGE_%02d", i);
		pMsgStatic = GetControl<CDnMessageStatic>(szName);
		m_vecStaticMessage.push_back(pMsgStatic);
	}

	m_pPassiveSkillSlotDlg = new CDnPassiveSkillSlotDlg(UI_TYPE_CHILD, this);
	m_pPassiveSkillSlotDlg->Initialize(true);

	m_bShowOption = true;
	ShowMinimapOption(false);
	GetControl<CEtUICheckBox>("ID_CHECKBOX_FUNCTIONNPC")->SetChecked(true);
	GetControl<CEtUICheckBox>("ID_CHECKBOX_NPC")->SetChecked(true);
	if (CDnMinimap::IsActive()) {
		GetMiniMap().ShowFunctionalNPC(true);
		GetMiniMap().ShowOtherNPC(true);
	}

#ifdef PRE_ADD_AUTO_DICE
	m_bShowDiceOption = true;
	GetControl<CEtUICheckBox>("ID_CHECKBOX_DICE")->SetChecked(CGameOption::GetInstance().m_bEnableAutoDice);
	ShowAutoDiceOption(false);
#endif

	m_hNorthTexture = LoadResource(CEtResourceMng::GetInstance().GetFullName("Minimap_N.dds").c_str(), RT_TEXTURE);

	// 재로딩이 있을때마다 키설정값을 다시 불러온다.
	for (int i = 0; i < UIWrappingKeyIndex_Amount; ++i)
		SetMainUIHotKey(i, _ToVK(g_UIWrappingKeyData[i]));
	// 임시. 나중에 제대로 추가해야한다.
	GetControl("ID_CHAT")->SetHotKey('C');

	for (int i = 0; i<6; i++) {
		sprintf_s(szName, "ID_STATIC_TIMEEVENT%d", i);
		m_pStaticTimeEventIcon[i] = GetControl<CEtUIStatic>(szName);
	}
	m_pStaticTimeEventText = GetControl<CEtUIStatic>("ID_TEXT_TIME");

#ifdef PRE_ADD_LEVELUP_GUIDE
	m_pStaticLevelUpSkillAlarm = GetControl<CEtUIStatic>("ID_STATIC_SKILL");
	m_pStaticLevelUpSkillAlarm->Show(false);
	m_pStaticLevelUpSkillAlarmText = GetControl<CEtUIStatic>("ID_TEXT_SKILL");
	m_pStaticLevelUpSkillAlarmText->Show(false);
#endif

#ifdef PRE_TEST_ANIMATION_UI
	m_pAniTestBtn = GetControl<CEtUIButton>("ID_TESTANI");
	m_pAniTestDlg = new CDnAniTestDlg(UI_TYPE_CHILD, this);
	m_pAniTestDlg->Initialize(false);
#endif

	m_pFoodItemSlot = GetControl<CDnItemSlotButton>("ID_BT_PETFOOD");
	m_pFoodItemSlot->Show(false);
	m_pStaticFoodItemCount = GetControl<CEtUIStatic>("ID_TEXT_PETFOOD");
	m_pStaticFoodItemCount->Show(false);

#ifdef PRE_SPECIALBOX
	m_pButtonEventReceiverAlarm = GetControl<CEtUIButton>("ID_BT_EVENTALARM");
	m_pStaticEventReceiverBalloon = GetControl<CEtUIStatic>("ID_STATIC_ALARM");
	m_pStaticEventReceiverText = GetControl<CEtUIStatic>("ID_TEXT_ALARM");
#endif

#ifdef PRE_MOD_APPELLATIONBOOK_RENEWAL
	m_pStaticNewAppellationAlarm = GetControl<CEtUIStatic>("ID_STATIC_APP");
	m_pStaticNewAppellationAlarmText = GetControl<CEtUIStatic>("ID_TEXT_APP");
#endif	

#ifdef PRE_ADD_STAMPSYSTEM
	m_NPCArrowDlg = new CDnNPCArrowDlg();
	m_NPCArrowDlg->Initialize(false);
#endif // PRE_ADD_STAMPSYSTEM

}

void CDnMainDlg::Show(bool bShow)
{
	if (CDnPetTask::IsActive() && !GetPetTask().IsLoadPetConfig())
	{
		GetPetTask().LoadPetConfig();
		SetPetFoodInfo(GetPetTask().GetPetOption().m_nFoodItemID);
	}

	if (m_bShow == bShow)
		return;

	if (bShow)
	{
		m_QuickSlgDlgGroup.ShowDialog(m_dwShowQuickDlgID, true);
	}
	else
	{
		m_dwShowQuickDlgID = m_QuickSlgDlgGroup.GetShowDialogID();
	}

	CEtUIDialog::Show(bShow);
}

void CDnMainDlg::ShowQuickSlotChangeMessage()
{
	if (!CDnActor::s_hLocalActor) return;
	if (CDnActor::s_hLocalActor->GetLevel() > 15) return;

	WCHAR wszStr[128] = { 0, };
	swprintf_s(wszStr, GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 209), g_szKeyString[g_UIWrappingKeyData[IW_UI_QUICKSLOTCHANGE]]);
	GetInterface().AddChatMessage(CHATTYPE_SYSTEM, L"", wszStr, false);
}

void CDnMainDlg::ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg)
{
	SetCmdControlName(pControl->GetControlName());

	if (!GetInterface().IsLockMainMenu()) {
		if (nCommand == EVENT_BUTTON_CLICKED)
		{
#ifdef PRE_ADD_SECONDARY_SKILL
			// 낚시, 요리 중일 경우 인벤토리 외 다른 창 사용 불가
			if (((CDnLifeSkillFishingTask::IsActive() && GetLifeSkillFishingTask().IsNowFishing())
				|| (CDnLifeSkillCookingTask::IsActive() && GetLifeSkillCookingTask().IsNowCooking()))
				&& !(IsCmdControl("ID_INVENTORY") || IsCmdControl("ID_COMMUNITY")))
				return;
#endif // PRE_ADD_SECONDARY_SKILL

			if (IsCmdControl("ID_BUTTON_SLOT_UP"))
			{
				if (m_QuickSlgDlgGroup.GetShowDialogID() != RADIOMSG_DLG &&
					m_QuickSlgDlgGroup.GetShowDialogID() != LIFESKILL_DLG)
				{
					ChangeQuickSlotUp();
					ShowQuickSlotChangeMessage();
				}
				return;
			}

			if (IsCmdControl("ID_BUTTON_SLOT_DOWN"))
			{
				if (m_QuickSlgDlgGroup.GetShowDialogID() != RADIOMSG_DLG &&
					m_QuickSlgDlgGroup.GetShowDialogID() != LIFESKILL_DLG)
				{
					ChangeQuickSlotDown();
					ShowQuickSlotChangeMessage();
				}
				return;
			}

			if (IsCmdControl("ID_BUTTON_DUMMY"))
			{
				// 제대로 구현될때까지 임시로 뺀다.
				return;

				// 별도의 라디오메세지 모드 bool값을 사용하지 않고, 그냥 있던 변수 m_dwShowQuickDlgID를 사용한다.
				// 만약 라디오메세지 모드에서 창이 show(false)되었다 show(true)되면 그냥 기본 퀵슬롯 상태로 되돌린다.
				if (m_QuickSlgDlgGroup.GetShowDialogID() != RADIOMSG_DLG)
				{
					m_dwShowQuickDlgID = m_QuickSlgDlgGroup.GetShowDialogID();
					m_QuickSlgDlgGroup.ShowDialog((DWORD)RADIOMSG_DLG, true);
					m_pStaticSlotNumber->SetText(L"V");
					GetControl<CEtUIButton>("ID_BUTTON_SLOT_UP")->Enable(false);
					GetControl<CEtUIButton>("ID_BUTTON_SLOT_DOWN")->Enable(false);
					ShowExpDialog(false);
					ShowFTGDialog(false);
				}
				else
				{
					m_QuickSlgDlgGroup.ShowDialog(m_dwShowQuickDlgID, true);
					m_pStaticSlotNumber->SetIntToText(m_dwShowQuickDlgID + 1);
					GetControl<CEtUIButton>("ID_BUTTON_SLOT_UP")->Enable(true);
					GetControl<CEtUIButton>("ID_BUTTON_SLOT_DOWN")->Enable(true);
					ShowExpDialog(false);
					ShowFTGDialog(false);
				}
				return;
			}

			if (IsCmdControl("ID_BUTTON_PLUS"))
			{
				if (CDnMinimap::IsActive() == true)
					GetMiniMap().Zoom(+1.0f);
				return;
			}

			if (IsCmdControl("ID_BUTTON_MINUS"))
			{
				if (CDnMinimap::IsActive() == true)
					GetMiniMap().Zoom(-1.0f);
				return;
			}

			if (IsCmdControl("ID_BUTTON_MAP"))
			{
				ShowMinimapOption(!m_bShowOption);
				return;
			}

#ifdef PRE_ADD_AUTO_DICE
			if (IsCmdControl("ID_BUTTON_DICE"))
			{
				ShowAutoDiceOption(!m_bShowDiceOption);
				return;
			}
#endif

#ifdef PRE_ADD_PVP_VILLAGE_ACCESS
			if (IsCmdControl("ID_COLOSSEUM"))
			{
#ifdef PRE_ADD_DWC
				if (GetDWCTask().IsDWCChar() == false)
				{
					if (CDnWorld::GetInstance().GetMapType() == CDnWorld::MapTypeVillage)
					{
						if (GetInterface().IsShowPVPVillageAccessDlg())
							GetInterface().ShowPVPVillageAccessDlg(false);
						else
							SendRequestPVPVillageAcess();
					}
				}
#else	// PRE_ADD_DWC
				if (CDnWorld::GetInstance().GetMapType() == CDnWorld::MapTypeVillage)
				{
					if (GetInterface().IsShowPVPVillageAccessDlg())
						GetInterface().ShowPVPVillageAccessDlg(false);
					else
						SendRequestPVPVillageAcess();
				}
#endif	// PRE_ADD_DWC
			}
#endif // PRE_ADD_PVP_VILLAGE_ACCESS

			CDnMainMenuDlg *pMainMenuDlg = GetInterface().GetMainMenuDialog();
			if (!pMainMenuDlg) return;

			if (GetInterface().IsShowPrivateMarketDlg()
				|| GetInterface().IsShowCostumeMixDlg()
				|| GetInterface().IsShowCostumeDesignMixDlg()
#ifdef PRE_ADD_COSRANDMIX
				|| GetInterface().IsShowCostumeRandomMixDlg()
#endif
				)
				return;

			if (GetInterface().GetFadeDlg() && (GetInterface().GetFadeDlg()->GetFadeMode() != CDnFadeInOutDlg::modeEnd))
				return;

			if (IsCmdControl("ID_INVENTORY"))
			{
				pMainMenuDlg->ToggleShowDialog(CDnMainMenuDlg::INVENTORY_DIALOG);
				return;
			}

			if (IsCmdControl("ID_COMMUNITY"))
			{
				pMainMenuDlg->ToggleShowDialog(CDnMainMenuDlg::COMMUNITY_DIALOG);
				return;
			}

			if (IsCmdControl("ID_SKILL"))
			{
				pMainMenuDlg->ToggleShowDialog(CDnMainMenuDlg::SKILL_DIALOG);
				return;
			}

			if (IsCmdControl("ID_CHAR_STATUS"))
			{
				pMainMenuDlg->ToggleShowDialog(CDnMainMenuDlg::CHARSTATUS_DIALOG);
				return;
			}

			if (IsCmdControl("ID_QUEST"))
			{
				pMainMenuDlg->ToggleShowDialog(CDnMainMenuDlg::QUEST_DIALOG);
				return;
			}

			if (IsCmdControl("ID_SYSTEM"))
			{
				pMainMenuDlg->ToggleShowDialog(CDnMainMenuDlg::SYSTEM_DIALOG);
				return;
			}

			if (IsCmdControl("ID_MAP"))
			{
				if (GetInterface().IsPVP())
					GetInterface().TogglePVPMapDlg();
				else
					pMainMenuDlg->ToggleShowDialog(CDnMainMenuDlg::ZONEMAP_DIALOG);

				return;
			}
			if (IsCmdControl("ID_MISSION"))
			{
				pMainMenuDlg->ToggleShowDialog(CDnMainMenuDlg::MISSION_DIALOG);
				return;
			}

			if (IsCmdControl("ID_CHAT"))
			{
				pMainMenuDlg->SetChatRoomFlag(uMsg == WM_KEYDOWN);
				pMainMenuDlg->ToggleShowDialog(CDnMainMenuDlg::CHATROOM_DIALOG);
				return;
			}

#ifdef PRE_ADD_NPC_REPUTATION_SYSTEM
			if (IsCmdControl("ID_REPUTE"))
			{
#ifdef PRE_ADD_NO_REPUTATION_DLG
#else
				pMainMenuDlg->ToggleShowDialog(CDnMainMenuDlg::NPC_REPUTATION_TAB_DIALOG);
#endif
				return;
			}
#endif // #ifdef PRE_ADD_NPC_REPUTATION_SYSTEM

#if defined(PRE_ADD_INTEGERATE_EVENTUI)
			if (IsCmdControl("ID_EVENT"))
			{
				if (!CDnTimeEventTask::IsActive()) return;
				CDnTimeEventTask *pTask = &CDnTimeEventTask::GetInstance();

				pMainMenuDlg->ToggleShowDialog(CDnMainMenuDlg::INTEGRATE_EVENT_DIALOG);

				return;
			}
#else
			if (IsCmdControl("ID_EVENT"))
			{
				if (!CDnTimeEventTask::IsActive()) return;
				CDnTimeEventTask *pTask = &CDnTimeEventTask::GetInstance();

				if (pTask && pTask->GetTimeEventCount() == 0)
					GetInterface().MessageBox(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 7736));
				else
					pMainMenuDlg->ToggleShowDialog(CDnMainMenuDlg::TIMEEVENT_DIALOG);

				return;
			}
			if (IsCmdControl("ID_CHECK"))
			{
				pMainMenuDlg->ToggleShowDialog(CDnMainMenuDlg::ATTENDANCEEVENT_DIALOG);
				m_bNotifyCheckAttendance = false;
				return;
			}
#endif // PRE_ADD_INTEGERATE_EVENTUI
#ifdef PRE_ADD_NO_HELP_DLG
#else
			if (IsCmdControl("ID_USERGUIDE"))
			{
				pMainMenuDlg->CloseMenuDialog();
				pMainMenuDlg->GetSystemDlg()->ToggleHelpDlg();
				return;
			}
#endif

#ifdef PRE_TEST_ANIMATION_UI
			if (IsCmdControl("ID_TESTANI"))
			{
				if (m_pAniTestDlg)
					m_pAniTestDlg->Show(!m_pAniTestDlg->IsShow());
				return;
			}
#endif

			if (IsCmdControl("ID_BT_MOVIE"))
			{
				CDnCommonTask* pCommonTask = static_cast<CDnCommonTask*>(CTaskManager::GetInstance().GetTask("CommonTask"));
				if (pCommonTask == NULL)
					return;

				float browserWidth = CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::DNTVBrowserWidth);
				float browserHeight = CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::DNTVBrowserHeight);

				std::string url(pCommonTask->GetMiniSiteURL());
				CDnInterface::GetInstance().OpenBrowser(url, browserWidth, browserHeight, CDnInterface::eBPT_CENTER, eGBT_GENERAL);
			}

#if defined(PRE_ADD_MAILBOX_OPEN)
			if (IsCmdControl("ID_MAIL"))
			{
				//메일함 열기..
				//마을에서만 가능..
				if (pMainMenuDlg == NULL ||
					CDnWorld::MapTypeVillage != CDnWorld::GetInstance().GetMapType())
					return;

				GetInterface().OpenMailDialogByShortCutKey();
			}
#endif // PRE_ADD_MAILBOX_OPEN

#ifdef PRE_ADD_PVP_RANKING
			if (IsCmdControl("ID_RANK"))
			{
				GetInterface().OpenPvPRankDlg();
			}
#endif

#ifdef PRE_SPECIALBOX
			if (IsCmdControl("ID_BT_EVENTALARM"))
			{
				if (CDnWorld::GetInstance().GetMapType() == CDnWorld::MapTypeVillage)
				{
					if (CDnEventReceiverTabDlg::CanRequestSpecialBoxInfo())
					{
						SendSpecialBoxList();
						CDnEventReceiverTabDlg::RequestDealy();
					}
					else
					{
						pMainMenuDlg->ToggleShowDialog(CDnMainMenuDlg::EVENT_RECEIVER_DIALOG);
					}
				}
				else
				{
					GetInterface().AddChatMessage(CHATTYPE_SYSTEM, L"", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 8226), false);
				}

				return;
			}
#endif

#if defined( PRE_ALTEIAWORLD_EXPLORE )
			if (IsCmdControl("ID_BT_ALTEA"))
			{
				if (CDnWorld::GetInstance().GetMapType() != CDnWorld::MapTypeVillage)
					return;
				pMainMenuDlg->ToggleShowDialog(CDnMainMenuDlg::ALTEA_TAB_DIALOG);
			}
#endif	// #if defined( PRE_ALTEIAWORLD_EXPLORE )
		} // Button End

		if (nCommand == EVENT_CHECKBOX_CHANGED) {
			if (IsCmdControl("ID_CHECKBOX_FUNCTIONNPC"))
			{
				if (CDnMinimap::IsActive()) {
					bool bChecked = GetControl<CEtUICheckBox>("ID_CHECKBOX_FUNCTIONNPC")->IsChecked();
					GetMiniMap().ShowFunctionalNPC(bChecked);
				}
			}
			if (IsCmdControl("ID_CHECKBOX_NPC"))
			{
				if (CDnMinimap::IsActive()) {
					bool bChecked = GetControl<CEtUICheckBox>("ID_CHECKBOX_NPC")->IsChecked();
					GetMiniMap().ShowOtherNPC(bChecked);
				}
			}

#ifdef PRE_ADD_AUTO_DICE
			if (IsCmdControl("ID_CHECKBOX_DICE"))
			{
				bool bChecked = GetControl<CEtUICheckBox>("ID_CHECKBOX_DICE")->IsChecked();
				CGameOption::GetInstance().m_bEnableAutoDice = bChecked;
				GetControl<CDnMenuButton>("ID_BUTTON_DICE")->SetOpen(bChecked);
			}
#endif
		}
	}

	CEtUIDialog::ProcessCommand(nCommand, bTriggeredByUser, pControl, uMsg);
}

void CDnMainDlg::Render(float fElapsedTime)
{
	if (!IsShow())
		return;

	DrawSprite(m_hBackBlack, SUICoord(0.0f, 0.0f, 1.0f, 1.0f), 0x7fffffff, m_pStaticBackBlack->GetUICoord());
	DrawSprite(m_hBackLight, SUICoord(0.0f, 0.0f, 1.0f, 1.0f), 0xffffffff, m_pStaticBackLight->GetUICoord());

	if (CDnMinimap::IsActive())
	{
		SUICoord MinimapCoord, UVCoord;
		MinimapCoord.SetPosition((m_DlgInfo.DlgCoord.fWidth * 0.5f) - (78.0f / DEFAULT_UI_SCREEN_WIDTH), (6.0f / DEFAULT_UI_SCREEN_HEIGHT));
		MinimapCoord.SetSize(156.0f / DEFAULT_UI_SCREEN_WIDTH, 156.0f / DEFAULT_UI_SCREEN_HEIGHT);
		UVCoord.SetCoord(0.0f, 0.0f, 1.0f, 1.0f);
		//DrawSprite( CDnMinimap::GetInstance().GetTexture(), UVCoord, 0xdcffffff, MinimapCoord, -CalcCameraRotateValue() );
		//DrawSprite( CDnMinimap::GetInstance().GetTexture(), UVCoord, 0xdcffffff, MinimapCoord );
		DrawSprite(CDnMinimap::GetInstance().GetTexture(), UVCoord, 0xffffffff, MinimapCoord);
	}

	CEtUIDialog::Render(fElapsedTime);

	if (CDnMinimap::IsActive())
	{
		DrawMinimapOutInfo();
	}

	// Draw North Mark
	SUICoord NorthCoord;
	NorthCoord.SetPosition((m_DlgInfo.DlgCoord.fWidth * 0.5f) - (11.0f / DEFAULT_UI_SCREEN_WIDTH), (-3.0f / DEFAULT_UI_SCREEN_HEIGHT));
	NorthCoord.SetSize(22.0f / DEFAULT_UI_SCREEN_WIDTH, 22.0f / DEFAULT_UI_SCREEN_HEIGHT);
	DrawSprite(m_hNorthTexture, SUICoord(0.0f, 0.0f, 1.0f, 1.0f), 0xffffffff, NorthCoord);

	RenderEXP();
	RenderFTG();
	RenderWeekFTG();
	RenderPCRoomFTG();
	RenderEventFTG();
#ifdef PRE_ADD_VIP
	RenderVIPFTG();
#endif
	RenderFTGStatic();
}

bool CDnMainDlg::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (!IsShow())
	{
		return false;
	}

	bool bRet;
	bRet = CEtUIDialog::MsgProc(hWnd, uMsg, wParam, lParam);

	switch (uMsg)
	{
	case WM_LBUTTONDOWN:
	{
#if defined(PRE_ADD_68286)
		GetInterface().SetDisableChatTabMsgproc(true);
#endif // PRE_ADD_68286

		float fMouseX, fMouseY;
		POINT MousePoint;
		MousePoint.x = short(LOWORD(lParam));
		MousePoint.y = short(HIWORD(lParam));
		PointToFloat(MousePoint, fMouseX, fMouseY);

		if ((GetControl<CEtUIButton>("ID_BUTTON_MAP")->IsInside(fMouseX, fMouseY) == false) &&
			(GetControl<CEtUIStatic>("ID_MM_BACK")->IsInside(fMouseX, fMouseY) == false)) {
			if (m_bShowOption) {
				ShowMinimapOption(false);
			}
		}
#ifdef PRE_ADD_AUTO_DICE
		if ((GetControl<CEtUIButton>("ID_BUTTON_DICE")->IsInside(fMouseX, fMouseY) == false) &&
			(GetControl<CEtUIStatic>("ID_STATIC0")->IsInside(fMouseX, fMouseY) == false)) {
			if (m_bShowDiceOption) {
				ShowAutoDiceOption(false);
			}
		}
#endif

#if defined(PRE_ADD_68286)
		if (m_pStaticPost && m_pStaticPost->IsShow() && m_pStaticPost->IsInside(fMouseX, fMouseY) == true)
		{
			//메일함 열기..
			//마을에서만 가능..
			if (CDnWorld::MapTypeVillage == CDnWorld::GetInstance().GetMapType())
			{
#ifdef PRE_MOD_POTENTIAL_JEWEL_RENEWAL
				CDnPotentialJewelDlg* pPotentialJewelDlg = GetInterface().GetItemPotentialDlg();
				if (!pPotentialJewelDlg) return false;
				if (pPotentialJewelDlg->IsShow() == false)
				{
					GetInterface().OpenMailDialogByShortCutKey();

					//설정을 해 줘야 채팅창 활성화가 되지 않는다.
					GetInterface().SetDisableChatTabMsgproc(false);
					bRet = true;
				}
#else
				GetInterface().OpenMailDialogByShortCutKey();
				//설정을 해 줘야 채팅창 활성화가 되지 않는다.
				GetInterface().SetDisableChatTabMsgproc(false);
				bRet = true;
#endif // PRE_MOD_POTENTIAL_JEWEL_RENEWAL
			}
		}

		if (m_pStaticCashShopGift && m_pStaticCashShopGift->IsShow() && m_pStaticCashShopGift->IsInside(fMouseX, fMouseY) == true)
		{
			//마을에서만 가능..
			if (CDnWorld::MapTypeVillage == CDnWorld::GetInstance().GetMapType())
			{
#ifdef PRE_MOD_POTENTIAL_JEWEL_RENEWAL		
				CDnPotentialJewelDlg* pPotentialJewelDlg = GetInterface().GetItemPotentialDlg();
				if (!pPotentialJewelDlg) return false;
				if (pPotentialJewelDlg->IsShow() == false)
				{
					GetInterface().OpenCashShopRecvGiftWindow();
					//GetCashShopTask().RequestCashShopRecvGiftBasicInfoByShortCut();
					//설정을 해 줘야 채팅창 활성화가 되지 않는다.
					GetInterface().SetDisableChatTabMsgproc(false);
					bRet = true;
				}
#else
				GetInterface().OpenCashShopRecvGiftWindow();
				//GetCashShopTask().RequestCashShopRecvGiftBasicInfoByShortCut();
				//설정을 해 줘야 채팅창 활성화가 되지 않는다.
				GetInterface().SetDisableChatTabMsgproc(false);
				bRet = true;
#endif

			}
		}
#endif // PRE_ADD_68286
#ifdef PRE_ADD_REMOTE_QUEST
		if (m_pStaticQuestBallon && m_pStaticQuestBallon->IsShow() && m_pStaticQuestBallon->IsInside(fMouseX, fMouseY))
		{
#ifdef PRE_MOD_POTENTIAL_JEWEL_RENEWAL
			CDnPotentialJewelDlg* pPotentialJewelDlg = GetInterface().GetItemPotentialDlg();
			if (!pPotentialJewelDlg) return false;
			if (pPotentialJewelDlg->IsShow() == false)
			{
				CDnMainMenuDlg *pMainMenuDlg = GetInterface().GetMainMenuDialog();
				if (pMainMenuDlg && !pMainMenuDlg->IsOpenQuestDlg())
					pMainMenuDlg->ToggleShowDialog(CDnMainMenuDlg::QUEST_DIALOG);
			}
#else  // PRE_MOD_POTENTIAL_JEWEL_RENEWAL
			CDnMainMenuDlg *pMainMenuDlg = GetInterface().GetMainMenuDialog();
			if (pMainMenuDlg && !pMainMenuDlg->IsOpenQuestDlg())
				pMainMenuDlg->ToggleShowDialog(CDnMainMenuDlg::QUEST_DIALOG);
#endif // PRE_MOD_POTENTIAL_JEWEL_RENEWAL
		}
#endif // PRE_ADD_REMOTE_QUEST

#ifdef PRE_ADD_LEVELUP_REWARD_NOTIFIER
		if (CDnWorld::MapTypeVillage == CDnWorld::GetInstance().GetMapType())
		{
			if (m_pStaticLevelUpNotifier && m_pStaticLevelUpNotifier->IsShow() && m_pStaticLevelUpNotifier->IsInside(fMouseX, fMouseY))
			{
				GetInterface().GetMainMenuDialog()->ToggleShowDialog(CDnMainMenuDlg::INVENTORY_DIALOG);
				CDnInvenTabDlg* pInvenTabDlg = (CDnInvenTabDlg*)GetInterface().GetMainMenuDialog(CDnMainMenuDlg::INVENTORY_DIALOG);
				if (pInvenTabDlg)
				{
					CDnItem* pItem = CDnItemTask::GetInstance().GetExistLevelUpBox(CDnActor::s_hLocalActor->GetLevel());
					if (pItem)
					{
						pInvenTabDlg->GetCashInvenDlg()->RefreshInvenPageFromSlotIndex(pItem->GetSlotIndex());
						pInvenTabDlg->GetCashInvenDlg()->SetFocusBlink(pItem->GetSlotIndex(), 500, 2400);
						pInvenTabDlg->SetCheckedTab(ST_INVENTORY_CASH - 1);
					}
				}
			}
		}
#endif
	}
	break;
	case WM_MOUSEMOVE:
	{
		float fMouseX, fMouseY;
		POINT MousePoint;
		MousePoint.x = short(LOWORD(lParam));
		MousePoint.y = short(HIWORD(lParam));
		PointToFloat(MousePoint, fMouseX, fMouseY);

		bool bExpMouseEnter(false);

		for (int i = 0; i<5; i++)
		{
			if (m_pExpGauge[i]->IsMouseEnter())
			{
				bExpMouseEnter = true;
				break;
			}
		}

		if (!CDnMouseCursor::GetInstance().IsShowCursor()) bExpMouseEnter = false;
		ShowExpDialog(bExpMouseEnter, fMouseX, fMouseY);
		ShowMailNotifyTooltip(fMouseX, fMouseY);
		ShowHarvestNotifyTooltip(fMouseX, fMouseY);
#ifdef PRE_ADD_LEVELUP_REWARD_NOTIFIER
		ShowLevelUpBoxNotifierToolTip(fMouseX, fMouseY);
#endif 

		if (m_bCheckAttendanceFirst)
			ShowCheckAttendanceTooltip(fMouseX, fMouseY);

		bool bFTGMouseEnter(false);
		SUICoord uiCoord;
		m_pEventFTGGauge->GetUICoord(uiCoord);
		if (uiCoord.IsInside(fMouseX, fMouseY) && CDnMouseCursor::GetInstance().IsShowCursor())
			bFTGMouseEnter = true;

		ShowFTGDialog(bFTGMouseEnter, fMouseX, fMouseY);

#ifdef PRE_REMOVE_MINISITE
#else
		if (m_pNewMiniSiteBtn && m_pNewMiniSiteBtn->IsInside(fMouseX, fMouseY))
		{
			CDnCommonTask* pCommonTask = static_cast<CDnCommonTask*>(CTaskManager::GetInstance().GetTask("CommonTask"));
			if (pCommonTask != NULL)
			{
				std::wstring tooltipText;
				pCommonTask->GetMiniSiteTooltip(tooltipText);
				m_pNewMiniSiteBtn->SetTooltipText(tooltipText.c_str());
			}
		}
#endif
	}
	break;
	}

	return bRet;
}

float CDnMainDlg::CalcCameraRotateValue()
{
	EtVector3 vZDir, vCross;
	float fDot, fAngle;

	//vZDir = CDnCamera::GetActiveCamera()->GetMatEx()->m_vZAxis;
	vZDir.y = 0.0f;
	vZDir.x = 0.0f;
	vZDir.z = 1.0f;
	EtVec3Normalize(&vZDir, &vZDir);
	fDot = EtVec3Dot(&vZDir, &EtVector3(0.0f, 0.0f, 1.0f));
	m_fDegree = EtAcos(fDot);
	EtVec3Cross(&vCross, &EtVector3(0.0f, 0.0f, 1.0f), &vZDir);
	fAngle = EtToDegree(m_fDegree);

	if (vCross.y > 0.0f)
	{
		m_fDegree = -m_fDegree;
		return fAngle;
	}
	else
	{
		return -fAngle;
	}
}

bool CDnMainDlg::InitQuickSlot(int nQuickSlotIndex, MIInventoryItem *pItem)
{
	int nTabIndex = nQuickSlotIndex / QUICKSLOT_BUTTON_MAX;
	int nSlotIndex = nQuickSlotIndex % QUICKSLOT_BUTTON_MAX;

	ASSERT((nTabIndex >= QUICKSLOT_DLG_00) && (nTabIndex<QUICKSLOT_DLG_MAX));
	ASSERT((nSlotIndex >= 0) && (nSlotIndex<QUICKSLOT_BUTTON_MAX));

	if (!(nSlotIndex >= 0 && nSlotIndex < QUICKSLOT_BUTTON_MAX)) return false;
	if (!(nTabIndex >= QUICKSLOT_DLG_00 && nTabIndex < QUICKSLOT_DLG_MAX)) return false;

	CDnQuickSlotDlg *pCurQuickSlotDlg = static_cast<CDnQuickSlotDlg*>(m_QuickSlgDlgGroup.GetDialog(nTabIndex));
	if (!pCurQuickSlotDlg) return false;
	return pCurQuickSlotDlg->InitSlot(nSlotIndex, pItem);
}

void CDnMainDlg::SwapEventSlot(bool bTrue)
{
	if (bTrue)
	{
		m_dwOrignalShowQuickSlotDlgID = m_QuickSlgDlgGroup.GetShowDialogID();
		m_dwOrignalPrevQuickSlotDlgID = m_dwPrevQuickDlgID;

		m_dwShowQuickDlgID = QUICKSLOT_EVENT;
		m_QuickSlgDlgGroup.ShowDialog(QUICKSLOT_EVENT, true);
		m_pStaticSlotNumber->SetText(L"E");
		ShowExpDialog(false);
		ShowFTGDialog(false);

		CDnQuickSlotDlg *pCurQuickSlotDlg = static_cast<CDnQuickSlotDlg*>(m_QuickSlgDlgGroup.GetDialog(QUICKSLOT_EVENT));
		if (pCurQuickSlotDlg) pCurQuickSlotDlg->ResetAllSlot();

	}
	else
	{
		m_QuickSlgDlgGroup.ShowDialog(QUICKSLOT_EVENT, false);
		m_QuickSlgDlgGroup.ShowDialog(m_dwOrignalShowQuickSlotDlgID, true);
		m_dwPrevQuickDlgID = m_dwOrignalPrevQuickSlotDlgID;
		m_dwShowQuickDlgID = m_dwOrignalShowQuickSlotDlgID;

		m_pStaticSlotNumber->SetIntToText((m_dwOrignalShowQuickSlotDlgID - (m_nSkillSlotPage * QUICKSLOT_PAGE_COUNT)) + 1);
		ShowExpDialog(false);
		ShowFTGDialog(false);
	}
}

void CDnMainDlg::ChangeQuickSlotUp()
{
	if (m_bLockQuickSlot)
		return;

	int nCurDlgID = (int)m_QuickSlgDlgGroup.GetShowDialogID();

	if (!IsCurrentSkillPageIndex(++nCurDlgID))
		nCurDlgID = m_nSkillSlotPage * QUICKSLOT_PAGE_COUNT;

	m_dwPrevQuickDlgID = m_QuickSlgDlgGroup.GetShowDialogID();
	m_QuickSlgDlgGroup.ShowDialog((DWORD)nCurDlgID, true);

	m_pStaticSlotNumber->SetIntToText((nCurDlgID - (m_nSkillSlotPage * QUICKSLOT_PAGE_COUNT)) + 1);

	ShowExpDialog(false);
	ShowFTGDialog(false);
}

void CDnMainDlg::ChangeQuickSlotDown()
{
	if (m_bLockQuickSlot)
		return;

	int nCurDlgID = (int)m_QuickSlgDlgGroup.GetShowDialogID();
	if (!IsCurrentSkillPageIndex(--nCurDlgID))
		nCurDlgID = ((QUICKSLOT_SKILL_PAGE_COUNT * m_nSkillSlotPage) + QUICKSLOT_PAGE_COUNT) - 1;

	m_dwPrevQuickDlgID = m_QuickSlgDlgGroup.GetShowDialogID();
	m_QuickSlgDlgGroup.ShowDialog((DWORD)nCurDlgID, true);

	m_pStaticSlotNumber->SetIntToText((nCurDlgID - (m_nSkillSlotPage * QUICKSLOT_PAGE_COUNT)) + 1);

	ShowExpDialog(false);
	ShowFTGDialog(false);
}

void CDnMainDlg::ChangeQuickSlotButton(CDnSlotButton *pDragButton, CDnSlotButton *pPressedButton)
{
	if (!pDragButton || !pPressedButton)
		return;

	MIInventoryItem *pItem1 = pDragButton->GetItem();
	MIInventoryItem *pItem2 = pPressedButton->GetItem();

	if (!pItem1)
		return;

	int nTabIndex = ((CDnQuickSlotButton*)pPressedButton)->GetItemSlotIndex() / QUICKSLOT_BUTTON_MAX;
	int nSlotIndex = ((CDnQuickSlotButton*)pPressedButton)->GetItemSlotIndex() % QUICKSLOT_BUTTON_MAX;

	CDnQuickSlotDlg *pCurQuickSlotDlg = static_cast<CDnQuickSlotDlg*>(m_QuickSlgDlgGroup.GetDialog(nTabIndex));
	if (pCurQuickSlotDlg) pCurQuickSlotDlg->SetSlot(nSlotIndex, pItem1);

	if (pItem2)
	{
		nTabIndex = ((CDnQuickSlotButton*)pDragButton)->GetItemSlotIndex() / QUICKSLOT_BUTTON_MAX;
		nSlotIndex = ((CDnQuickSlotButton*)pDragButton)->GetItemSlotIndex() % QUICKSLOT_BUTTON_MAX;

		CDnQuickSlotDlg *pCurQuickSlotDlg = static_cast<CDnQuickSlotDlg*>(m_QuickSlgDlgGroup.GetDialog(nTabIndex));
		if (pCurQuickSlotDlg) pCurQuickSlotDlg->SetSlot(nSlotIndex, pItem2);
	}

	pDragButton->DisableSplitMode(true);
	pPressedButton->DisableSplitMode(true);

	drag::ReleaseControl();
}

bool CDnMainDlg::InitLifeSkillQuickSlot(int nSlotIndex, int nLifeSkillID, MIInventoryItem *pItem, MIInventoryItem::InvenItemTypeEnum ItemType)
{
	CDnLifeSkillQuickSlotDlg *pQuickSlotDlg = static_cast<CDnLifeSkillQuickSlotDlg*>(m_QuickSlgDlgGroup.GetDialog(LIFESKILL_DLG));
	return pQuickSlotDlg->InitSlot(nSlotIndex, nLifeSkillID, pItem, ItemType);
}

void CDnMainDlg::ResetLifeSkillQuickSlot(int nSlotIndex)
{
	CDnLifeSkillQuickSlotDlg *pQuickSlotDlg = static_cast<CDnLifeSkillQuickSlotDlg*>(m_QuickSlgDlgGroup.GetDialog(LIFESKILL_DLG));
	return pQuickSlotDlg->ResetSlot(nSlotIndex, false);
}

bool CDnMainDlg::ResetSecondarySkillQuickSlot(int nSecondarySkillID)
{
	CDnLifeSkillQuickSlotDlg *pQuickSlotDlg = static_cast<CDnLifeSkillQuickSlotDlg*>(m_QuickSlgDlgGroup.GetDialog(LIFESKILL_DLG));
	return pQuickSlotDlg->ResetSecondarySkillQuickSlot(nSecondarySkillID);
}

#ifdef PRE_SWAP_QUICKSLOT
void CDnMainDlg::SwapQuickSlot()
{
	// 현재는 두칸밖에 없으므로, 이 함수면 끝이다.
	ChangeQuickSlotUp();
}
#endif

int CDnMainDlg::GetEmptyQuickSlotIndex()
{
	int nSlotIndex = -1;
	CEtUIDialogGroup::DIALOG_VEC &vecQuickSlotDialog = m_QuickSlgDlgGroup.GetDialogList();
	for (int i = 0; i<(int)vecQuickSlotDialog.size() - 1; ++i)	// 마지막 퀵슬롯은 제스처 퀵슬롯이라 제외시킨다.
	{
		CEtUIDialogGroup::ID_DIALOG_PAIR &id_dlg_pair = vecQuickSlotDialog[i];
		if (!IsCurrentSkillPageIndex(id_dlg_pair.first))
			continue;

		nSlotIndex = static_cast<CDnQuickSlotDlg*>(id_dlg_pair.second)->FindEmptyQuickSlotIndex();
		if (nSlotIndex != -1) return nSlotIndex;
	}
	return -1;
}

int CDnMainDlg::GetSkillQuickSlotIndex(int nSkillID)
{
	int nSlotIndex = -1;
	CEtUIDialogGroup::DIALOG_VEC &vecQuickSlotDialog = m_QuickSlgDlgGroup.GetDialogList();
	for (int i = 0; i<(int)vecQuickSlotDialog.size() - 1; ++i)	// 마지막 퀵슬롯은 제스처 퀵슬롯이라 제외시킨다.
	{
		CEtUIDialogGroup::ID_DIALOG_PAIR &id_dlg_pair = vecQuickSlotDialog[i];
		if (!IsCurrentSkillPageIndex(id_dlg_pair.first))
			continue;

		nSlotIndex = static_cast<CDnQuickSlotDlg*>(id_dlg_pair.second)->FindSkillQuickSlotIndex(nSkillID);
		if (nSlotIndex != -1) return nSlotIndex;
	}
	return -1;
}

void CDnMainDlg::EnableQuickSlot(bool bEnable)
{
	CEtUIDialogGroup::DIALOG_VEC &vecQuickSlotDialog = m_QuickSlgDlgGroup.GetDialogList();
	for (int i = 0; i<(int)vecQuickSlotDialog.size(); ++i)
	{
		CEtUIDialogGroup::ID_DIALOG_PAIR &id_dlg_pair = vecQuickSlotDialog[i];
		CEtUIDialog* pDlg = id_dlg_pair.second;
		if (pDlg)
		{
			if (pDlg->GetDialogID() == LIFESKILL_DLG)
			{
				CDnLifeSkillQuickSlotDlg* pGestureDlg = static_cast<CDnLifeSkillQuickSlotDlg*>(pDlg);
				if (pGestureDlg)
					pGestureDlg->EnableQuickSlot(bEnable);
			}
			else
			{
				CDnQuickSlotDlg* pQuickSlotDlg = static_cast<CDnQuickSlotDlg*>(pDlg);
				if (pQuickSlotDlg)
					pQuickSlotDlg->EnableQuickSlot(bEnable);
			}
		}
	}
}

void CDnMainDlg::DrawMinimapOutInfo()
{
	std::vector<CDnMinimap::SOutInfo> &vecOutInfo = GetMiniMap().GetMiniMapOutInfo();
	if (vecOutInfo.empty() == false)
	{
		for (DWORD i = 0; i < vecOutInfo.size(); ++i)
		{
			CEtUIStatic *pIcon = NULL;
			switch (vecOutInfo[i].eIconType)
			{

			case CDnMinimap::emICON_INDEX::indexTraceQuest:
				pIcon = GetControl<CEtUIStatic>("ID_ICON_QUEST");
				break;

			case CDnMinimap::emICON_INDEX::indexAllyCaptain:
				pIcon = GetControl<CEtUIStatic>("ID_MAP_PARTY");
				break;

			case CDnMinimap::emICON_INDEX::indexEnemyCaptain:
				pIcon = GetControl<CEtUIStatic>("ID_MAP_ENEMY");
				break;
			}

			if (pIcon)
				DrawIcon(pIcon, vecOutInfo[i].fDegree, 72.f, 0.0025f);
		}
	}
}


void CDnMainDlg::DrawIcon(CEtUIControl *pControl, float fAngle, float fRadius, float fValue)
{
	SUIElement *pElement = pControl->GetElement(0);
	if (pElement)
	{
		SUICoord uiCoord;
		pControl->GetUICoord(uiCoord);

		float fCenX, fCenY;
		fCenX = (m_DlgInfo.DlgCoord.fWidth) * 0.5f;
		fCenY = 78.0f / DEFAULT_UI_SCREEN_HEIGHT;

		float fModifiedAngle = fAngle - 1.5707964f;

		EtVector2 vTemp;
		vTemp.x = cos(fModifiedAngle) * (fRadius / DEFAULT_UI_SCREEN_WIDTH);

		if (fModifiedAngle > RIGHT_DEGREE_MAX || fModifiedAngle < LEFT_DEGREE_MAX)
		{
			uiCoord.fY = m_DlgInfo.DlgCoord.fHeight - uiCoord.fHeight + fValue - (20.0f / DEFAULT_UI_SCREEN_HEIGHT);
		}
		else
		{
			vTemp.y = sin(fModifiedAngle) * (fRadius / DEFAULT_UI_SCREEN_HEIGHT);
			uiCoord.fY = vTemp.y + (fCenY - (uiCoord.fHeight*0.5f));
		}

		uiCoord.fX = vTemp.x + (fCenX - (uiCoord.fWidth*0.5f));
		uiCoord.SetSize(pElement->fTextureWidth * 1.25f, pElement->fTextureHeight * 1.25f);

		DrawSprite(pControl->GetTemplate().m_hTemplateTexture, pElement->TemplateUVCoord, 0xffffffff, uiCoord, EtToDegree(fAngle));
	}
}

void CDnMainDlg::ShowExpDialog(bool bShow, float fX, float fY)
{
	if (bShow)
	{
		if (CDnActor::s_hLocalActor)
		{
			CDnPlayerActor *pActor = (CDnPlayerActor *)CDnActor::s_hLocalActor.GetPointer();
			int nNextLevExp = pActor->GetNextLevelExperience();
			int nCurLevExp = pActor->GetCurrentLevelExperience();
			int nCurExp = pActor->GetExperience();

			if (nCurExp > nNextLevExp)
			{
				nCurExp = nNextLevExp;
			}

			nNextLevExp -= nCurLevExp;
			nCurExp -= nCurLevExp;

			m_pExpDlg->SetExp(nCurExp, nNextLevExp);
			ShowChildDialog(m_pExpDlg, true);

			SUICoord dlgCoord;
			m_pExpDlg->GetDlgCoord(dlgCoord);
			dlgCoord.fX = fX + GetXCoord() - (dlgCoord.fWidth / 2.0f);
			dlgCoord.fY = fY + GetYCoord() - dlgCoord.fHeight - 0.004f;
			m_pExpDlg->SetDlgCoord(dlgCoord);
		}
	}
	else
	{
		ShowChildDialog(m_pExpDlg, false);
	}
}

void CDnMainDlg::ShowMailNotifyTooltip(float fX, float fY)
{
#ifdef PRE_ADD_MAIL_UI_IMPROVEMENTS

	if (GetTradeTask().GetTradeMail().GetUnreadMailCount() <= 0)	// 메일을 다 읽었을 경우 툴팁을 출력할 필요가없다. 
		return;

	SButtonInfo* pMailInfo = GetMenuBotton(CDnMainMenuDlg::MAIL_DIALOG);

	if (pMailInfo && pMailInfo->pButton != NULL && pMailInfo->pButton->IsShow() != false)
	{
		if (pMailInfo->pButton->IsInside(fX, fY))
		{
			std::wstring str;
			if (m_bMailAlarm)
				str = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 1979);	// UISTRING : 새 메일이 도착했습니다.
			else
				str = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 1980);	// UISTRING : 읽지 않은 메일이 있습니다.

			ShowTooltipDlg(pMailInfo->pButton, true, str, 0xffffffff, true);
		}
		else
		{
			if (IsTooltipControl(pMailInfo->pButton))
				ShowTooltipDlg(pMailInfo->pButton, false);
		}
	}

#else
	if (m_pStaticPost == NULL || m_pStaticPost->IsShow() == false)
		return;

	if (m_pStaticPost->IsInside(fX, fY))
	{
		std::wstring str;
		if (m_bMailAlarm)
			str = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 1979);	// UISTRING : 새 메일이 도착했습니다.
		else
			str = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 1980);	// UISTRING : 읽지 않은 메일이 있습니다.

		ShowTooltipDlg(m_pStaticPost, true, str, 0xffffffff, true);
	}
	else
	{
		if (IsTooltipControl(m_pStaticPost))
			ShowTooltipDlg(m_pStaticPost, false);
	}
#endif 
}

void CDnMainDlg::ShowHarvestNotifyTooltip(float fX, float fY)
{
	if (m_pStaticHarvest == NULL)
		return;

	if (m_pStaticHarvest->IsShow() == false)
		return;

	if (m_pStaticHarvest->IsInside(fX, fY))
	{
		std::wstring str = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 7461);	// UISTRING : 농장창고에 회수하지 않은 물품이 있습니다. 농장창고를 확인해 보세요.

		ShowTooltipDlg(m_pStaticHarvest, true, str, 0xffffffff, true);
	}
	else
	{
		if (IsTooltipControl(m_pStaticHarvest))
			ShowTooltipDlg(m_pStaticHarvest, false);
	}
}

#ifdef PRE_ADD_LEVELUP_REWARD_NOTIFIER
void CDnMainDlg::ShowLevelUpBoxNotifierToolTip(float fx, float fy)
{
	if (m_pStaticLevelUpNotifier == NULL)
		return;

	if (!m_pStaticLevelUpNotifier->IsShow())
		return;

	if (m_pStaticLevelUpNotifier->IsInside(fx, fy))
	{
		std::wstring str = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 7766);	// UISTRING : 열 수 있는 레벨업 보상 상자를 가지고 있습니다.

		ShowTooltipDlg(m_pStaticLevelUpNotifier, true, str, 0xffffffff, true);
	}
	else
	{
		if (IsTooltipControl(m_pStaticLevelUpNotifier))
			ShowTooltipDlg(m_pStaticLevelUpNotifier, false);
	}
}
#endif 

void CDnMainDlg::ShowCheckButton(bool bShow)
{
	for (int i = 0; i<(int)m_vecButtonInfo.size(); i++)
	{
		if (m_vecButtonInfo[i].nDialogID == CDnMainMenuDlg::ATTENDANCEEVENT_DIALOG)
		{
			m_vecButtonInfo[i].pButton->Show(bShow);
			break;
		}
	}
}

void CDnMainDlg::ShowCheckAttendanceTooltip(float fX, float fY)
{
	CDnMenuButton* pCheckButton = GetControl<CDnMenuButton>("ID_CHECK");
	if (pCheckButton && pCheckButton->IsInside(fX, fY))
	{
		m_pStaticCheckAttendanceBalloon->Show(true);
		m_pStaticCheckAttendanceText->Show(true);
		m_bNotifyCheckAttendance = false;
	}
	else if (!m_bNotifyCheckAttendance)
	{
		m_pStaticCheckAttendanceBalloon->Show(false);
		m_pStaticCheckAttendanceText->Show(false);
	}
}

void CDnMainDlg::SetCheckAttendanceFirst(bool bCheckAttendanceFirst)
{
	m_bCheckAttendanceFirst = bCheckAttendanceFirst;
	CDnMenuButton* pCheckButton = GetControl<CDnMenuButton>("ID_CHECK");
	if (pCheckButton)
	{
		if (m_bCheckAttendanceFirst)
		{
			pCheckButton->SetBlink();
			m_fCheckAttendanceAlarmTime = 0.0f;
			m_bNotifyCheckAttendance = true;
			m_pStaticCheckAttendanceBalloon->Show(true);
			m_pStaticCheckAttendanceText->Show(true);

			// 출석당일 체크이면 시스템 메세지 출력
			CDnCommonTask* pCommonTask = static_cast<CDnCommonTask*>(CTaskManager::GetInstance().GetTask("CommonTask"));
			if (pCommonTask == NULL)
				return;

			std::map<CommonVariable::Type::eCode, CommonVariable::Data>::iterator iter = pCommonTask->GetVariableData().find(CommonVariable::Type::AttendanceEvent);
			if (iter == pCommonTask->GetVariableData().end())
				return;

			int nOngoingDay = static_cast<int>(iter->second.biValue >> 32);

			DNTableFileFormat*  pSox = GetDNTable(CDnTableDB::TATTENDANCEEVENT);
			if (pSox == NULL)
				return;

			for (int i = 0; i<pSox->GetItemCount(); i++)
			{
				int nTableID = pSox->GetItemID(i);

				bool bEnable = pSox->GetFieldFromLablePtr(nTableID, "_ON")->GetInteger() == 1 ? true : false;
				if (!bEnable)
					continue;

				int nAttendanceDate = pSox->GetFieldFromLablePtr(nTableID, "_Date_Count")->GetInteger();
				if (nAttendanceDate == nOngoingDay)	// 연속 출석일 당일 출석일 경우 체크
				{
					GetInterface().AddChatMessage(CHATTYPE_SYSTEM, L"", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 7735), false);
					break;
				}
			}
		}
		else
		{
			pCheckButton->ResetBlink();
			if (m_pStaticCheckAttendanceBalloon->IsShow())
				m_pStaticCheckAttendanceBalloon->Show(false);
			if (m_pStaticCheckAttendanceText->IsShow())
				m_pStaticCheckAttendanceText->Show(false);
			if (m_bNotifyCheckAttendance)
				m_bNotifyCheckAttendance = false;
		}
	}
}

void CDnMainDlg::UpdateAttendanceNotify(float fElapsedTime)
{
	if (m_bNotifyCheckAttendance)
	{
		if (m_fCheckAttendanceAlarmTime >= 10.f)
		{
			CDnMenuButton* pCheckButton = GetControl<CDnMenuButton>("ID_CHECK");
			if (pCheckButton)
				pCheckButton->ResetBlink();
			if (m_pStaticCheckAttendanceBalloon->IsShow())
				m_pStaticCheckAttendanceBalloon->Show(false);
			if (m_pStaticCheckAttendanceText->IsShow())
				m_pStaticCheckAttendanceText->Show(false);

			m_bNotifyCheckAttendance = false;
		}

		if (m_fCheckAttendanceAlarmTime != 100.0f)
			m_fCheckAttendanceAlarmTime += fElapsedTime;
	}
}

#ifdef PRE_MOD_POTENTIAL_JEWEL_RENEWAL
void CDnMainDlg::ProcessPotentialJewelButton()
{
	// 잠재력 부여 윈도우가 열렸을땐, 모든 버튼을 막는다.
	CDnPotentialJewelDlg* pPotentialJewelDlg = GetInterface().GetItemPotentialDlg();
	if (!pPotentialJewelDlg)
		return;

	bool bIsShow = pPotentialJewelDlg->IsShow();
	if (bIsShow)
		ToggleEnableAllButton(false);

	// 캐시샵 바로가기 버튼 막기.
	GetInterface().DisableCashShopMenuDlg(bIsShow);
}
#endif // PRE_MOD_POTENTIAL_JEWEL_RENEWAL

void CDnMainDlg::Process(float fElapsedTime)
{
	CEtUIDialog::Process(fElapsedTime);
	UpdateMailNotify(fElapsedTime);
	UpdateCashShopGiftNotify(fElapsedTime);
	UpdateHarvestNotify(fElapsedTime);
#ifdef PRE_ADD_LEVELUP_REWARD_NOTIFIER
	UpdateLevelUpBoxNotifier(fElapsedTime);
#endif 
#ifdef PRE_ADD_LEVELUP_GUIDE
	UpdateNewSkillNotify(fElapsedTime);
#endif
	UpdateMiniSiteNotify(fElapsedTime);
#ifdef PRE_SPECIALBOX
	UpdateSpecialBoxNotify(fElapsedTime);
#endif
#if defined( PRE_ALTEIAWORLD_EXPLORE )
	UpdateAlteaDice(fElapsedTime);
#endif	// #if defined( PRE_ALTEIAWORLD_EXPLORE )

	if (IsShow())
	{
		// Note : ID_MAP
		ProcessCommunityButton();
		ProcessSystemMenuButton();
		ProcessQuickSlot();
		ProcessZoneMapButton();
		ProcessButton(CDnMainMenuDlg::QUEST_DIALOG);
		ProcessButton(CDnMainMenuDlg::SKILL_DIALOG);
		ProcessButton(CDnMainMenuDlg::MISSION_DIALOG);
		ProcessButton(CDnMainMenuDlg::CHARSTATUS_DIALOG);
		ProcessButton(CDnMainMenuDlg::INVENTORY_DIALOG);
		ProcessChatRoomButton();

#ifdef PRE_ADD_NPC_REPUTATION_SYSTEM
#if !defined(PRE_ADD_NO_REPUTATION_DLG)
#ifdef PRE_ADD_INTEGERATE_QUEST_REPUT
		ProcessButton(CDnMainMenuDlg::InTEGRATE_QUESTNREPUTATION_DIALOG);
#else  // PRE_ADD_INTEGERATE_QUEST_REPUT
		ProcessButton(CDnMainMenuDlg::NPC_REPUTATION_TAB_DIALOG);
#endif // PRE_ADD_INTEGERATE_QUEST_REPUT
#endif	// PRE_ADD_NO_REPUTATION_DLG
#endif	// PRE_ADD_NPC_REPUTATION_SYSTEM

#ifdef PRE_ADD_INTEGERATE_EVENTUI
		ProcessButton(CDnMainMenuDlg::INTEGRATE_EVENT_DIALOG);
#endif // PRE_ADD_INTEGERATE_EVENTUI

		ProcessPVPModeButton();

#ifdef PRE_ADD_PVP_VILLAGE_ACCESS
		ProcessPVPVillageAccessDlg();
#endif // PRE_ADD_PVP_VILLAGE_ACCESS

		ProcessTimeEventAlarm(fElapsedTime);

#if defined(PRE_ADD_MAILBOX_OPEN)
		ProcessButton(CDnMainMenuDlg::MAIL_DIALOG);
#endif // PRE_ADD_MAILBOX_OPEN

#ifdef PRE_ADD_PVP_RANKING
		ProcessPvPRankButton();
#endif // PRE_ADD_PVP_RANKING

#ifdef PRE_MOD_POTENTIAL_JEWEL_RENEWAL
		ProcessPotentialJewelButton();
#endif // PRE_MOD_POTENTIAL_JEWEL_RENEWAL

		if (m_bCheckAttendanceFirst)
		{
			UpdateAttendanceNotify(fElapsedTime);

			CDnMenuButton* pBtn = GetControl<CDnMenuButton>("ID_CHECK");
			if (pBtn && pBtn->IsShow() == false)
			{
				CDnCommonTask* pCommonTask = static_cast<CDnCommonTask*>(CTaskManager::GetInstance().GetTask("CommonTask"));
				if (pCommonTask && pCommonTask->HasVariableData(CommonVariable::Type::AttendanceEvent))
					pBtn->Show(true);
			}
		}

		if (GetControl<CDnMenuButton>("ID_CHECK")->IsShow())
		{
			CDnCommonTask* pCommonTask = static_cast<CDnCommonTask*>(CTaskManager::GetInstance().GetTask("CommonTask"));
			if (pCommonTask && m_bCheckAttendanceFirst != pCommonTask->GetCheckAttendanceFirst())
				SetCheckAttendanceFirst(pCommonTask->GetCheckAttendanceFirst());
		}

#ifdef PRE_ADD_STAMPSYSTEM
		if (m_bSearching && m_SearchedNpcID != -1)
		{
			RotetaNPCArrow();
		}
#endif // PRE_ADD_STAMPSYSTEM

#ifdef PRE_ADD_DWC
		// DWC캐릭은 캐시샵 버튼을 막는다
		GetInterface().DisableCashShopMenuDlg(GetDWCTask().IsDWCChar());
#ifdef PRE_ALTEIAWORLD_EXPLORE
		// DWC캐릭은 알테이아 주사위를 막는다
		if (GetDWCTask().IsDWCChar())
			m_pAlteaDiceIcon->Enable(false);
#endif // PRE_ALTEIAWORLD_EXPLORE
#endif // PRE_ADD_DWC
	}
}

//void CDnMainDlg::UpdateExpUp( float fElapsedTime )
//{
//	if( m_bExpUp )
//	{
//		SUICoord uiCoord;
//		m_pExpUp->GetUICoord( uiCoord );
//		uiCoord.fY -= 0.001f;
//		uiCoord.fHeight += 0.001f;
//		m_pExpUp->SetUICoord( uiCoord );
//
//		if( uiCoord.fHeight >= m_ExpCoord.fHeight )
//		{
//			m_bExpUp = false;
//			m_fExpUpTime = 1.0f;
//		}
//	}
//	else
//	{
//		if( m_fExpUpTime <= 0.0f )
//		{
//			m_pExpUp->Show(false);
//			m_fExpUpTime = 0.0f;
//		}
//		else
//		{
//			m_fExpUpTime -= fElapsedTime;
//		}
//	}
//}
//
//void CDnMainDlg::UpdateMoneyUp( float fElapsedTime )
//{
//	if( m_bMoneyUp )
//	{
//		SUICoord uiCoord;
//		m_pMoneyUp->GetUICoord( uiCoord );
//		uiCoord.fY -= 0.001f;
//		uiCoord.fHeight += 0.001f;
//		m_pMoneyUp->SetUICoord( uiCoord );
//
//		if( uiCoord.fHeight >= m_ExpCoord.fHeight )
//		{
//			m_bMoneyUp = false;
//			m_fMoneyUpTime = 1.0f;
//		}
//	}
//	else
//	{
//		if( m_fMoneyUpTime <= 0.0f )
//		{
//			m_pMoneyUp->Show(false);
//			m_fMoneyUpTime = 0.0f;
//		}
//		else
//		{
//			m_fMoneyUpTime -= fElapsedTime;
//		}
//	}
//}

void CDnMainDlg::UpdateHarvestNotify(float fElapsedTime)
{
	if (GetInterface().IsOpenBlind())
		return;

	if (m_bHarvestAlarm)
	{
		if (m_fHarvestAlarmTime >= 3.f)
		{
			m_bHarvestAlarm = false;
			m_pStaticHarvest->Show(false);
		}
		else
		{
			static int nTimeBack;
			int nBlinkTime = int(m_fHarvestAlarmTime*10.0f);

			if (((nBlinkTime % 3) == 0) && (nTimeBack != nBlinkTime))
			{
				nTimeBack = nBlinkTime;
				m_pStaticHarvest->Show(!m_pStaticHarvest->IsShow());
			}
		}

		m_fHarvestAlarmTime += fElapsedTime;
	}
	else
	{
		if (!CDnLifeSkillPlantTask::IsActive())
		{
			m_pStaticHarvest->Show(false);
		}
		else
		{
			if (GetLifeSkillPlantTask().GetWareHouseItemCount() > 0)
				m_pStaticHarvest->Show(true);
			else
				m_pStaticHarvest->Show(false);
		}
	}
}

#ifdef PRE_ADD_LEVELUP_REWARD_NOTIFIER
void CDnMainDlg::UpdateLevelUpBoxNotifier(float fElapsedTime)
{
	if (GetInterface().IsOpenBlind())
		return;

	if (m_bLevelUpBoxAlarm)
	{
		if (m_fLevelUpboxAlarmTime >= 1.8f)
		{
			m_bLevelUpBoxAlarm = false;
			m_fLevelUpboxAlarmTime = 0.0f;
			m_pStaticLevelUpNotifier->Show(true);
		}
		else
		{
			static int nTimeBack;
			int nBlinkTime = int(m_fLevelUpboxAlarmTime*10.0f);

			if (((nBlinkTime % 3) == 0) && (nTimeBack != nBlinkTime))
			{
				nTimeBack = nBlinkTime;
				m_pStaticLevelUpNotifier->Show(!m_pStaticLevelUpNotifier->IsShow());
			}
		}

		m_fLevelUpboxAlarmTime += fElapsedTime;
	}
}
#endif 

#ifdef PRE_ADD_LEVELUP_GUIDE
void CDnMainDlg::UpdateNewSkillNotify(float fElapsedTime)
{
	if (GetInterface().IsOpenBlind())
		return;

	if (m_bLevelUpSkillAlarm)
	{
		m_pStaticLevelUpSkillAlarm->Show(true);
		m_pStaticLevelUpSkillAlarmText->Show(true);
		const float levelUpSkillAlarmMaxTime = 5.f;
		if (m_fLevelUpSkillAlarmTime >= levelUpSkillAlarmMaxTime)
		{
			m_fLevelUpSkillAlarmTime = 0.f;
			m_bLevelUpSkillAlarm = false;
			m_pStaticLevelUpSkillAlarm->Show(false);
			m_pStaticLevelUpSkillAlarmText->Show(false);
		}

		m_fLevelUpSkillAlarmTime += fElapsedTime;
	}
}
#endif

#ifdef PRE_SPECIALBOX
void CDnMainDlg::UpdateSpecialBoxNotify(float fElapsedTime)
{
	if (GetInterface().IsOpenBlind())
		return;

	if (m_bEventReceiverAlarm)
	{
		m_pStaticEventReceiverBalloon->Show(true);
		m_pStaticEventReceiverText->Show(true);

		const float fEventSkillAlarmMaxTime = 5.f;
		if (m_fEventReceiverAlarmTime >= fEventSkillAlarmMaxTime)
		{
			m_fEventReceiverAlarmTime = 0.f;
			m_bEventReceiverAlarm = false;
			m_pStaticEventReceiverBalloon->Show(false);
			m_pStaticEventReceiverText->Show(false);
		}

		m_fEventReceiverAlarmTime += fElapsedTime;
	}
}
#endif

#ifdef PRE_ADD_REMOTE_QUEST
void CDnMainDlg::ShowQuestNotify(bool bShow, CDnQuestTree::eRemoteQuestState remoteQuestState)
{
	m_bShowQuestNotify = bShow;
	if (m_pStaticQuestBallon)
		m_pStaticQuestBallon->Show(bShow);

	if (m_pStaticQuestText)
	{
		if (remoteQuestState == CDnQuestTree::REMOTEQUEST_ASK)
			m_pStaticQuestText->SetText(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 1200017));
		else if (remoteQuestState == CDnQuestTree::REMOTEQUEST_COMPLETE)
			m_pStaticQuestText->SetText(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 1200018));

		m_pStaticQuestText->Show(bShow);
	}
}
#endif // PRE_ADD_REMOTE_QUEST

void CDnMainDlg::UpdateMailNotify(float fElapsedTime)
{
	if (GetInterface().IsOpenBlind())
		return;

	if (m_bMailAlarm)
	{
		if (m_fMailAlarmTime >= 3.f)
		{
			m_bMailAlarm = false;

#ifdef PRE_ADD_MAIL_UI_IMPROVEMENTS
			m_pStaticMailText->Show(false);
			m_pStaticMailBallon->Show(false);
#else
			m_pStaticPost->Show(false);	// 사용하지 않음으로 필요가 없다. 
#endif 
		}
		else
		{
			static int nTimeBack;
			int nBlinkTime = int(m_fMailAlarmTime*10.0f);
#ifdef PRE_ADD_MAIL_UI_IMPROVEMENTS
			m_pStaticMailText->Show(true);
			m_pStaticMailBallon->Show(true);
#endif

			if (((nBlinkTime % 3) == 0) && (nTimeBack != nBlinkTime))
			{
				nTimeBack = nBlinkTime;
#ifdef PRE_ADD_MAIL_UI_IMPROVEMENTS
				m_pStaticPost->Show(false);
#else
				m_pStaticPost->Show(!m_pStaticPost->IsShow());
#endif 

			}
		}

		m_fMailAlarmTime += fElapsedTime;
	}
	else
	{
		if (!CDnTradeTask::IsActive())
		{
#ifdef PRE_ADD_MAIL_UI_IMPROVEMENTS
#else
			m_pStaticPost->Show(false);
#endif
		}
		else
		{
			if (GetTradeTask().GetTradeMail().GetUnreadMailCount() > 0)
			{
#ifdef PRE_ADD_MAIL_UI_IMPROVEMENTS
				m_pStaticPost->Show(false);
#else
				m_pStaticPost->Show(true);
#endif
			}
			else
			{
#ifdef PRE_ADD_MAIL_UI_IMPROVEMENTS
#else
				m_pStaticPost->Show(false);
#endif 
			}
		}
	}
}

void CDnMainDlg::UpdateCashShopGiftNotify(float fElapsedTime)
{
	if (GetInterface().IsOpenBlind())
		return;

	if (m_bCashShopGiftAlarm)
	{
		if (m_fCashShopGiftAlarmTime >= 3.f)
		{
			m_bCashShopGiftAlarm = false;
			m_pStaticCashShopGift->Show(false);
			m_pStaticCashShopGiftBalloon->Show(false);
			m_pStaticCashShopGiftText->Show(false);
		}
		else
		{
			m_pStaticCashShopGiftBalloon->Show(true);
			m_pStaticCashShopGiftText->Show(true);
			static int nTimeBack;
			int nBlinkTime = int(m_fCashShopGiftAlarmTime*10.0f);

			if (((nBlinkTime % 3) == 0) && (nTimeBack != nBlinkTime))
			{
				nTimeBack = nBlinkTime;
				m_pStaticCashShopGift->Show(!m_pStaticCashShopGift->IsShow());
			}
		}

		m_fCashShopGiftAlarmTime += fElapsedTime;
	}
	else
	{
		if (!CDnCashShopTask::IsActive())
		{
			m_pStaticCashShopGift->Show(false);
		}
		else
		{
			m_pStaticCashShopGift->Show(GetCashShopTask().GetGiftListCount() > 0);
		}
	}
}

void CDnMainDlg::ShowMiniSiteNotify(bool bShow, bool bBlink)
{
#ifdef PRE_REMOVE_MINISITE
#else
	m_bNotifyCheckMiniSite = bBlink;
	m_fMiniSiteAlarmTime = bBlink ? 0.f : eMiniSiteBlinkTime;
	m_pNewMiniSiteBtn->Show(bShow);
#endif
}

void CDnMainDlg::UpdateMiniSiteNotify(float fElapsedTime)
{
#ifdef PRE_REMOVE_MINISITE
	return;
#else
	if (GetInterface().IsOpenBlind())
		return;

	if (m_bNotifyCheckMiniSite)
	{
		if (m_fMiniSiteAlarmTime >= eMiniSiteBlinkTime)
		{
			if (m_pNewMiniSiteBtn->IsShow() == false)
				m_pNewMiniSiteBtn->Show(true);
		}
		else
		{
			m_fMiniSiteAlarmTime += fElapsedTime;

			static int nTimeBack;
			int nBlinkTime = int(m_fMiniSiteAlarmTime*10.0f);

			if (((nBlinkTime % 3) == 0) && (nTimeBack != nBlinkTime))
			{
				nTimeBack = nBlinkTime;
				m_pNewMiniSiteBtn->Show(!m_pNewMiniSiteBtn->IsShow());
			}
		}
	}
#endif // PRE_REMOVE_MINISITE
}

//void CDnMainDlg::ShowExpUp( int nExp )
//{
//	//if( nExp <= 0 )
//		return;
//
//	wchar_t szTemp[256]={0};
//	swprintf_s( szTemp, 256, L"+%d EXP", nExp );
//	m_pExpUp->SetText( szTemp );
//
//	SUICoord uiCoord;
//	uiCoord = m_ExpCoord;
//	uiCoord.fHeight = 0.0f;
//	m_pExpUp->SetUICoord( uiCoord );
//	m_pExpUp->Show(true);
//	m_bExpUp = true;
//}
//
//void CDnMainDlg::ShowMoneyUp( INT64 nCoin )
//{
//	//if( nCoin <= 0 )
//		return;
//
//	std::wstring strMoney;
//	DN_INTERFACE::UTIL::GetMoneyFormat( (int)nCoin, strMoney );
//	m_pMoneyUp->SetText( strMoney );
//
//	SUICoord uiCoord;
//	uiCoord = m_MoneyCoord;
//	uiCoord.fHeight = 0.0f;
//	m_pMoneyUp->SetUICoord( uiCoord );
//	m_pMoneyUp->Show(true);
//	m_bMoneyUp = true;
//}

int CDnMainDlg::GetQuickSlotTabIndex()
{
	return m_QuickSlgDlgGroup.GetShowDialogID();
}

void CDnMainDlg::OnRefreshQuickSlot()
{
	CEtUIDialogGroup::DIALOG_VEC &vecQuickSlotDialog = m_QuickSlgDlgGroup.GetDialogList();
	for (int i = 0; i<(int)vecQuickSlotDialog.size() - 1; ++i)	// 마지막 퀵슬롯은 제스처 퀵슬롯이라 제외시킨다.
	{
		CEtUIDialogGroup::ID_DIALOG_PAIR &id_dlg_pair = vecQuickSlotDialog[i];

		if (id_dlg_pair.second == NULL)
			continue;

		static_cast<CDnQuickSlotDlg*>(id_dlg_pair.second)->OnRefreshSlot();
	}
}

void CDnMainDlg::RefreshLifeSkillQuickSlot()
{
	CDnLifeSkillQuickSlotDlg *pLifeQuickSlotDlg = static_cast<CDnLifeSkillQuickSlotDlg*>(m_QuickSlgDlgGroup.GetDialog(LIFESKILL_DLG));
	if (pLifeQuickSlotDlg)
		pLifeQuickSlotDlg->RefreshVehicleItem();
}


void CDnMainDlg::OnMailNotify()
{
	m_bMailAlarm = true;
	m_fMailAlarmTime = 0.0f;
#ifdef PRE_ADD_MAIL_UI_IMPROVEMENTS
	m_pStaticPost->Show(false);
#else
	m_pStaticPost->Show(true);
#endif 

	if (m_nMainDlgSoundIndex[eSOUND_MAILALARM] != -1)
		CEtSoundEngine::GetInstance().PlaySound("2D", m_nMainDlgSoundIndex[eSOUND_MAILALARM]);

	GetInterface().AddChatMessage(CHATTYPE_SYSTEM, L"", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 1963), false);	// UISTRING : 새 우편이 도착했습니다.
}

void CDnMainDlg::OnHarvestNotify(int nCount)
{
	if (nCount > m_nHarvestCount)	//창고에 추가 되었을 경우에만 깜빡인다
	{
		m_bHarvestAlarm = true;
		m_fHarvestAlarmTime = 0.f;

		GetInterface().AddChatMessage(CHATTYPE_SYSTEM, L"", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 7486), false);	// UISTRING : 농장창고에 농작물이 추가되었습니다.
	}

	if (0 == nCount)
		m_pStaticHarvest->Show(false);
	else
		m_pStaticHarvest->Show(true);

	m_nHarvestCount = nCount;
}

void CDnMainDlg::OnCashShopGiftNotify(bool bSoundOnly)
{
	if (m_nMainDlgSoundIndex[eSOUND_CASHSHOPGIFT] != -1)
		CEtSoundEngine::GetInstance().PlaySound("2D", m_nMainDlgSoundIndex[eSOUND_CASHSHOPGIFT]);

	if (bSoundOnly)
		return;

	m_bCashShopGiftAlarm = true;
	m_fCashShopGiftAlarmTime = 0.0f;
	m_pStaticCashShopGift->Show(true);
	m_pStaticCashShopGiftBalloon->Show(true);
	m_pStaticCashShopGiftText->Show(true);

	GetInterface().AddChatMessage(CHATTYPE_SYSTEM, L"", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4668), false);	// UISTRING : 선물이 도착했습니다. 캐시샵 선물함에서 확인하세요.
}

#ifdef PRE_ADD_LEVELUP_REWARD_NOTIFIER
void CDnMainDlg::OnLevelUpBoxNotify(bool bShow)
{
	if (m_pStaticLevelUpNotifier == NULL)
		return;

	if (bShow)
	{
		if (m_pStaticLevelUpNotifier && m_pStaticLevelUpNotifier->IsShow())
		{
			m_bLevelUpBoxAlarm = false;
		}
		else
		{
			m_bLevelUpBoxAlarm = true;
		}
	}
	else
	{
		m_bLevelUpBoxAlarm = false;
	}

	m_pStaticLevelUpNotifier->Show(bShow);
}
#endif 

void CDnMainDlg::ToggleMenuButton(DWORD dwDialogID, bool bShow)
{
	for (int i = 0; i<(int)m_vecButtonInfo.size(); i++)
	{
		if (m_vecButtonInfo[i].nDialogID == dwDialogID)
		{
			m_vecButtonInfo[i].pButton->SetOpen(bShow);
			break;
		}
	}
}

#if defined(PRE_MOD_POTENTIAL_JEWEL_RENEWAL)
void CDnMainDlg::ToggleEnableAllButton(bool bEnable)
{
	for (int i = 0; i<(int)m_vecButtonInfo.size(); i++)
	{
		m_vecButtonInfo[i].pButton->Enable(bEnable);
	}
}
#endif // PRE_MOD_POTENTIAL_JEWEL_RENEWAL

//blondymarry start
void CDnMainDlg::ToggleEnableButton(DWORD dwDialogID, bool bEnable)
{
	for (int i = 0; i<(int)m_vecButtonInfo.size(); i++)
	{
		if (m_vecButtonInfo[i].nDialogID == dwDialogID)
		{
			m_vecButtonInfo[i].pButton->Enable(bEnable);
			break;
		}
	}
}
//blondymarry end

void CDnMainDlg::BlinkMenuButton(DWORD dwDialogID)
{
	for (int i = 0; i<(int)m_vecButtonInfo.size(); i++)
	{
		if (m_vecButtonInfo[i].nDialogID == dwDialogID)
		{
			m_vecButtonInfo[i].pButton->SetBlink();
			break;
		}
	}
}

void CDnMainDlg::CloseBlinkMenuButton(DWORD dwDialogID)
{
	for (int i = 0; i<(int)m_vecButtonInfo.size(); i++)
	{
		if (m_vecButtonInfo[i].nDialogID == dwDialogID)
		{
			m_vecButtonInfo[i].pButton->ResetBlink();
			break;
		}
	}
}

CDnMainDlg::SButtonInfo* CDnMainDlg::GetMenuBotton(DWORD dwDialogID)
{
	for (int i = 0; i<(int)m_vecButtonInfo.size(); i++)
	{
		if (m_vecButtonInfo[i].nDialogID == dwDialogID)
		{
			return &m_vecButtonInfo[i];
		}
	}

	return NULL;
}


void CDnMainDlg::RenderEXP()
{
	for (int i = 0; i<5; i++)
	{
		m_pExpGauge[i]->SetProgress(0.0f);
	}

	if (!CDnActor::s_hLocalActor)
		return;

	CDnPlayerActor *pActor = (CDnPlayerActor *)CDnActor::s_hLocalActor.GetPointer();
	int nCurLevExp = pActor->GetCurrentLevelExperience();
	int nNextLevExp = pActor->GetNextLevelExperience();
	int nCurExp = pActor->GetExperience();

	if (nCurExp > nNextLevExp)
	{
		nCurExp = nNextLevExp;
	}

	nNextLevExp -= nCurLevExp;
	nCurExp -= nCurLevExp;

	bool bSetExpGauge = true;
	int nLimitLevel = (int)CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::PlayerLevelLimit);
	if (CDnActor::s_hLocalActor && CDnActor::s_hLocalActor->GetLevel() == nLimitLevel) {
		CDnLocalPlayerActor *pActor = (CDnLocalPlayerActor *)CDnActor::s_hLocalActor.GetPointer();
		if (pActor->GetAccountLevel() < AccountLevel_Master) {
			m_pStaticExp->SetText(L"Max");
			bSetExpGauge = false;
		}
	}
	if (bSetExpGauge) {
		wchar_t szTemp[32] = { 0 };
		swprintf_s(szTemp, 32, L"%.2f%%", nNextLevExp ? ((float)nCurExp / (float)nNextLevExp*100.0f) : 0.0f);
		m_pStaticExp->SetText(szTemp);
	}

	int nMaxExp = nNextLevExp / 5;
	if (nMaxExp == 0) return;
	int nIndex = nCurExp / nMaxExp;
	int nExp = nCurExp % nMaxExp;

	for (int i = 0; i<nIndex; i++)
	{
		m_pExpGauge[i]->SetProgress(100.0f);
	}

	if (nIndex < 5)
	{
		m_pExpGauge[nIndex]->SetProgress((nExp*100.0f) / nMaxExp);
	}
}

void CDnMainDlg::RenderFTG()
{
	m_pFTGGauge->SetProgress(0.0f);

	if (!CDnItemTask::IsActive())
		return;

	int nCurFTG(0), nMaxFTG(0);
	int nTemp1, nTemp2, nTemp3, nTemp4, nTemp5, nTemp6;
	GetItemTask().GetFatigue(nCurFTG, nTemp1, nTemp2, nTemp3);
	GetItemTask().GetMaxFatigue(nMaxFTG, nTemp4, nTemp5, nTemp6);

	if (nCurFTG > nMaxFTG) nCurFTG = nMaxFTG;
	if (nMaxFTG == 0) return;
	m_pFTGGauge->SetProgress((nCurFTG*100.0f) / nMaxFTG);
}

void CDnMainDlg::RenderWeekFTG()
{
	m_pWeekFTGGauge->SetProgress(0.0f);

	if (!CDnItemTask::IsActive())
		return;

	int nCurFTG(0), nMaxFTG(0);
	int nTemp1, nTemp2, nTemp3, nTemp4, nTemp5, nTemp6;
	GetItemTask().GetFatigue(nTemp1, nCurFTG, nTemp2, nTemp3);
	GetItemTask().GetMaxFatigue(nTemp4, nMaxFTG, nTemp5, nTemp6);

	if (nCurFTG > nMaxFTG) nCurFTG = nMaxFTG;
	if (nMaxFTG == 0) return;
	m_pWeekFTGGauge->SetProgress((nCurFTG*100.0f) / nMaxFTG);
}

void CDnMainDlg::RenderPCRoomFTG()
{
	m_pPCRoomFTGGauge->SetProgress(0.0f);

	if (!CDnItemTask::IsActive())
		return;

	int nCurFTG(0), nMaxFTG(0);
	int nTemp1, nTemp2, nTemp3, nTemp4, nTemp5, nTemp6;
	GetItemTask().GetFatigue(nTemp1, nTemp2, nCurFTG, nTemp3);
	GetItemTask().GetMaxFatigue(nTemp4, nTemp5, nMaxFTG, nTemp6);

	if (nCurFTG > nMaxFTG) nCurFTG = nMaxFTG;
	if (nMaxFTG == 0) return;
	m_pPCRoomFTGGauge->SetProgress((nCurFTG*100.0f) / nMaxFTG);
}

void CDnMainDlg::RenderEventFTG()
{
	m_pEventFTGGauge->SetProgress(0.0f);

	if (!CDnItemTask::IsActive())
		return;

	int nCurFTG(0), nMaxFTG(0);
	int nTemp1, nTemp2, nTemp3, nTemp4, nTemp5, nTemp6;
	GetItemTask().GetFatigue(nTemp1, nTemp2, nTemp3, nCurFTG);
	GetItemTask().GetMaxFatigue(nTemp4, nTemp5, nTemp6, nMaxFTG);
	if (nCurFTG > nMaxFTG) nCurFTG = nMaxFTG;
	if (nMaxFTG == 0) return;
	m_pEventFTGGauge->SetProgress((nCurFTG*100.0f) / nMaxFTG);
}

#ifdef PRE_ADD_VIP
void CDnMainDlg::RenderVIPFTG()
{
	if (m_pVIPFTGGauge == NULL)
		return;

	m_pVIPFTGGauge->SetProgress(0.0f);

	if (!CDnItemTask::IsActive())
		return;

	int nCurFTG(0), nMaxFTG(0);
	nCurFTG = GetItemTask().GetFatigue(CDnItemTask::eVIPFTG);
	nMaxFTG = GetItemTask().GetMaxFatigue(CDnItemTask::eVIPFTG);

	if (nCurFTG > nMaxFTG) nCurFTG = nMaxFTG;
	if (nMaxFTG == 0) return;
	m_pVIPFTGGauge->SetProgress((nCurFTG*100.0f) / nMaxFTG);
}
#endif // PRE_ADD_VIP

void CDnMainDlg::RenderFTGStatic()
{
	if (!CDnItemTask::IsActive())
		return;

#ifdef PRE_ADD_VIP
	//	todo by kalliste : need refactoring statics -> vectorize(?) with type
	m_pStaticFTGFont->Show(false);
	m_pStaticPCRoomFTGFont->Show(false);
	m_pStaticEventFTGFont->Show(false);
	m_pStaticWeekFTGFont->Show(false);
	if (m_pStaticVIPFTGFont)
		m_pStaticVIPFTGFont->Show(false);

	m_pStaticFTG->Show(false);
	m_pStaticPCRoomFTG->Show(false);
	m_pStaticEventFTG->Show(false);
	m_pStaticWeekFTG->Show(false);
	if (m_pStaticVIPFTG)
		m_pStaticVIPFTG->Show(false);

	int nFatigues[CDnItemTask::eFTGMAX];
	int i = 0;
	for (; i < CDnItemTask::eFTGMAX; ++i)
		nFatigues[i] = GetItemTask().GetFatigue((CDnItemTask::eFatigueType)i);

	if (nFatigues[CDnItemTask::eEVENTFTG] > 0)
	{
		m_pStaticEventFTGFont->SetIntToText(nFatigues[CDnItemTask::eEVENTFTG]);
		m_pStaticEventFTGFont->Show(true);
		m_pStaticEventFTG->Show(true);
	}
	else if (nFatigues[CDnItemTask::ePCBANGFTG] > 0)
	{
		m_pStaticPCRoomFTGFont->SetIntToText(nFatigues[CDnItemTask::ePCBANGFTG]);
		m_pStaticPCRoomFTGFont->Show(true);
		m_pStaticPCRoomFTG->Show(true);
	}
	else if (m_pStaticVIPFTG && m_pStaticVIPFTGFont && nFatigues[CDnItemTask::eVIPFTG] > 0)
	{
		m_pStaticVIPFTGFont->SetIntToText(nFatigues[CDnItemTask::eVIPFTG]);
		m_pStaticVIPFTGFont->Show(true);
		m_pStaticVIPFTG->Show(true);
	}
	else if (nFatigues[CDnItemTask::eDAILYFTG] > 0)
	{
		m_pStaticFTGFont->SetIntToText(nFatigues[CDnItemTask::eDAILYFTG]);
		m_pStaticFTGFont->Show(true);
		m_pStaticFTG->Show(true);
	}
	else
	{
		m_pStaticWeekFTGFont->SetIntToText(nFatigues[CDnItemTask::eWEEKLYFTG]);
		m_pStaticWeekFTGFont->Show(true);
		m_pStaticWeekFTG->Show(true);
	}
#else // PRE_ADD_VIP
	int nTemp1, nTemp2, nTemp3, nTemp4;
	GetItemTask().GetFatigue(nTemp1, nTemp2, nTemp3, nTemp4);

	if (nTemp4 > 0)
	{
		m_pStaticFTGFont->Show(false);
		m_pStaticWeekFTGFont->Show(false);
		m_pStaticPCRoomFTGFont->Show(false);
		m_pStaticEventFTGFont->SetIntToText(nTemp4);
		m_pStaticEventFTGFont->Show(true);

		m_pStaticFTG->Show(false);
		m_pStaticWeekFTG->Show(false);
		m_pStaticPCRoomFTG->Show(false);
		m_pStaticEventFTG->Show(true);
	}
	else if (nTemp3 > 0)
	{
		m_pStaticFTGFont->Show(false);
		m_pStaticWeekFTGFont->Show(false);
		m_pStaticEventFTGFont->Show(false);
		m_pStaticPCRoomFTGFont->SetIntToText(nTemp3);
		m_pStaticPCRoomFTGFont->Show(true);

		m_pStaticFTG->Show(false);
		m_pStaticWeekFTG->Show(false);
		m_pStaticEventFTG->Show(false);
		m_pStaticPCRoomFTG->Show(true);
	}
	else if (nTemp1 > 0)
	{
		m_pStaticWeekFTGFont->Show(false);
		m_pStaticPCRoomFTGFont->Show(false);
		m_pStaticEventFTGFont->Show(false);
		m_pStaticFTGFont->SetIntToText(nTemp1);
		m_pStaticFTGFont->Show(true);

		m_pStaticWeekFTG->Show(false);
		m_pStaticPCRoomFTG->Show(false);
		m_pStaticEventFTG->Show(false);
		m_pStaticFTG->Show(true);
	}
	else
	{
		m_pStaticFTGFont->Show(false);
		m_pStaticPCRoomFTGFont->Show(false);
		m_pStaticEventFTGFont->Show(false);
		m_pStaticWeekFTGFont->SetIntToText(nTemp2);
		m_pStaticWeekFTGFont->Show(true);

		m_pStaticFTG->Show(false);
		m_pStaticPCRoomFTG->Show(false);
		m_pStaticEventFTG->Show(false);
		m_pStaticWeekFTG->Show(true);
	}
#endif // PRE_ADD_VIP
}

void CDnMainDlg::ShowFTGDialog(bool bShow, float fX, float fY)
{
	if (bShow)
	{
		if (CDnActor::s_hLocalActor && CDnItemTask::IsActive())
		{
#ifdef PRE_ADD_VIP
			int ftgs[CDnItemTask::eFTGMAX];
			memset(ftgs, 0, sizeof(ftgs));

			int i = 0;
			for (; i < CDnItemTask::eFTGMAX; ++i)
			{
				ftgs[i] = GetItemTask().GetFatigue((CDnItemTask::eFatigueType)i);
				m_pPopupFTGDlg->SetFatigue((CDnItemTask::eFatigueType)i, ftgs[i]);
			}
#else
			int nTemp1, nTemp2, nTemp3, nTemp4;
			GetItemTask().GetFatigue(nTemp1, nTemp2, nTemp3, nTemp4);

			m_pPopupFTGDlg->SetFatigue(nTemp1, nTemp2, nTemp3, nTemp4);
#endif // PRE_ADD_VIP
			m_pPopupFTGDlg->Show(true);

			SUICoord dlgCoord;
			m_pPopupFTGDlg->GetDlgCoord(dlgCoord);
			dlgCoord.fX = fX + GetXCoord() - (dlgCoord.fWidth / 2.0f);
			dlgCoord.fY = fY + GetYCoord() - dlgCoord.fHeight - 3.0f / DEFAULT_UI_SCREEN_HEIGHT;
			m_pPopupFTGDlg->SetDlgCoord(dlgCoord);
		}
	}
	else
	{
		m_pPopupFTGDlg->Show(false);
	}
}

void CDnMainDlg::AddMessage(const wchar_t *szMsg, DWORD dTextColor)
{
	for (int i = 5; i >= 0; i--)
	{
		if (m_vecStaticMessage[i]->IsShow())
		{
			m_vecStaticMessage[i + 1]->SetMsgTime(m_vecStaticMessage[i]->GetMsgTime());
			m_vecStaticMessage[i + 1]->SetTextWithEllipsis(m_vecStaticMessage[i]->GetText(), L"...");
			m_vecStaticMessage[i + 1]->SetTextColor(m_vecStaticMessage[i]->GetTextColor());
			m_vecStaticMessage[i + 1]->Show(true);

		}
	}

	m_vecStaticMessage[0]->SetMsgTime(5.0f);
	m_vecStaticMessage[0]->SetTextWithEllipsis(szMsg, L"...");
	m_vecStaticMessage[0]->Show(true);
	m_vecStaticMessage[0]->SetTextColor(dTextColor);
}

bool CDnMainDlg::SetQuickSlot(int nIndex, MIInventoryItem *pItem)
{
	CDnQuickSlotDlg *pQuickSlotDlg = (CDnQuickSlotDlg*)m_QuickSlgDlgGroup.GetShowDialog();
	if (pQuickSlotDlg)
	{
		return pQuickSlotDlg->SetSlot(nIndex, pItem);
	}

	return false;
}

bool CDnMainDlg::SetQuickSlot(int nQuickSlotIndex, CDnSkill *pSkill)
{
	int nTabIndex = nQuickSlotIndex / QUICKSLOT_BUTTON_MAX;
	int nSlotIndex = nQuickSlotIndex % QUICKSLOT_BUTTON_MAX;

	ASSERT((nTabIndex >= QUICKSLOT_DLG_00) && (nTabIndex<QUICKSLOT_DLG_MAX));
	ASSERT((nSlotIndex >= 0) && (nSlotIndex<QUICKSLOT_BUTTON_MAX));

	CDnQuickSlotDlg *pCurQuickSlotDlg = static_cast<CDnQuickSlotDlg*>(m_QuickSlgDlgGroup.GetDialog(nTabIndex));
	return pCurQuickSlotDlg->SetSlot(nSlotIndex, pSkill);
}

void CDnMainDlg::AddPassiveSkill(DnSkillHandle hSkill)
{
	m_pPassiveSkillSlotDlg->AddPassiveSkill(hSkill);
}

#if defined(PRE_FIX_61821)
void CDnMainDlg::ReplacePassiveSkill(DnSkillHandle hSkill)
{
	if (m_pPassiveSkillSlotDlg)
		m_pPassiveSkillSlotDlg->ReplacePassiveSkill(hSkill);
}
#endif // PRE_FIX_61821

void CDnMainDlg::ShowMinimapOption(bool bShow)
{
	if (m_bShowOption == bShow) {
		return;
	}
	m_bShowOption = bShow;
	GetControl<CEtUICheckBox>("ID_CHECKBOX_FUNCTIONNPC")->Show(bShow);
	GetControl<CEtUIStatic>("ID_STATIC_FUNCTIONNPC")->Show(bShow);
	GetControl<CEtUICheckBox>("ID_CHECKBOX_NPC")->Show(bShow);
	GetControl<CEtUIStatic>("ID_STATIC_NPC")->Show(bShow);
	GetControl<CEtUIStatic>("ID_MM_BACK")->Show(bShow);
}

#ifdef PRE_ADD_AUTO_DICE
void CDnMainDlg::ShowAutoDiceOption(bool bShow)
{
	if (m_bShowDiceOption == bShow) {
		return;
	}

	m_bShowDiceOption = bShow;
	GetControl<CEtUIStatic>("ID_TEXT_DICE")->Show(bShow);
	GetControl<CEtUIStatic>("ID_STATIC0")->Show(bShow);
	GetControl<CEtUICheckBox>("ID_CHECKBOX_DICE")->Show(bShow);
}

void CDnMainDlg::UpateAutoDiceOption(bool bReset)
{
	if (bReset)
		CGameOption::GetInstance().m_bEnableAutoDice = false;

	GetControl<CDnMenuButton>("ID_BUTTON_DICE")->SetOpen(CGameOption::GetInstance().m_bEnableAutoDice);
	GetControl<CEtUICheckBox>("ID_CHECKBOX_DICE")->SetChecked(CGameOption::GetInstance().m_bEnableAutoDice);
}
#endif


void CDnMainDlg::UpdateMinimapOption()
{
	bool bShowFunctionNpc = GetControl<CEtUICheckBox>("ID_CHECKBOX_FUNCTIONNPC")->IsChecked();
	bool bShowStaticNpc = GetControl<CEtUICheckBox>("ID_CHECKBOX_NPC")->IsChecked();

	if (CDnMinimap::IsActive()) {
		GetMiniMap().ShowFunctionalNPC(bShowFunctionNpc);
		GetMiniMap().ShowOtherNPC(bShowStaticNpc);
	}
}



void CDnMainDlg::ProcessSystemMenuButton()
{
	bool bEnable = true;
	if (CDnPartyTask::IsActive() && GetPartyTask().IsLocalActorEnterGateReady() && !GetPartyTask().IsGMTracing())
		bEnable = false;
	if (m_bEnableButtons == false)
		bEnable = false;
	if (CDnPartyTask::IsActive() && GetPartyTask().IsPartyProcessState(CDnPartyTask::PPS_ACCEPT_INVITE))
		bEnable = false;

	if (bEnable == false)
	{
		CDnMainMenuDlg *pMainMenuDlg = GetInterface().GetMainMenuDialog();
		if (pMainMenuDlg)
			pMainMenuDlg->CloseSystemDialogCompletely();
	}

	ToggleEnableButton(CDnMainMenuDlg::SYSTEM_DIALOG, bEnable);
}

void CDnMainDlg::ProcessCommunityButton()
{
	bool bEnable = true;

	if (CDnPartyTask::IsActive() && GetPartyTask().IsLocalActorEnterGateReady())
		bEnable = false;
	if (m_bEnableButtons == false)
		bEnable = false;
	if (CDnPartyTask::IsActive() && GetPartyTask().IsPartyProcessState(CDnPartyTask::PPS_ACCEPT_INVITE))
		bEnable = false;

#ifdef PRE_ADD_DWC
	// DWC 게임 도중에는 커뮤니티창을 열지 못하게 막는다.
	if (GetDWCTask().IsDWCPvPModePlaying())
		bEnable = false;
#endif

	if (bEnable == false)
	{
		CDnMainMenuDlg *pMainMenuDlg = GetInterface().GetMainMenuDialog();
		if (pMainMenuDlg && pMainMenuDlg->IsOpenDialog(CDnMainMenuDlg::COMMUNITY_DIALOG))
			pMainMenuDlg->ToggleShowDialog(CDnMainMenuDlg::COMMUNITY_DIALOG);
	}

	ToggleEnableButton(CDnMainMenuDlg::COMMUNITY_DIALOG, bEnable);
}

#ifdef PRE_ADD_PVP_RANKING
void CDnMainDlg::ProcessPvPRankButton()
{
	bool bEnable = false;
	if (GetInterface().GetInterfaceType() == CDnInterface::Village ||
		GetInterface().GetInterfaceType() == CDnInterface::PVPVillage
		)
		bEnable = true;

	ToggleEnableButton(CDnMainMenuDlg::PVP_RANK_DIALOG, bEnable);
}
#endif // PRE_ADD_PVP_RANKING

void CDnMainDlg::ProcessQuickSlot()
{
	// 배틀모드 전환쪽 코드 뒤에 콜백형태 등으로 구현할까 하다가,
	// 퀵슬롯 근처에 모든 처리를 두는게 나중에 관리하기 편할 듯 해서 그냥 프로세싱 부분에서 처리하기로 하겠다.
	// 패드 관련 코드는 여기서 구현한다.
	if (CDnActor::s_hLocalActor)
	{
		CDnPlayerActor *pActor = static_cast<CDnPlayerActor *>(CDnActor::s_hLocalActor.GetPointer());
		bool bBattleMode = pActor->IsBattleMode();

		if (!bBattleMode)
		{
			// 평화모드에서 제스처가 아니면 전환
			if (!m_QuickSlgDlgGroup.IsShowDialog(LIFESKILL_DLG))
			{
				m_dwPrevQuickDlgID = m_QuickSlgDlgGroup.GetShowDialogID();
				m_QuickSlgDlgGroup.ShowDialog(LIFESKILL_DLG, true);
				m_pStaticSlotNumber->SetText(L"G");
				ShowExpDialog(false);
				ShowFTGDialog(false);
			}
		}
		else if (m_QuickSlgDlgGroup.IsShowDialog(LIFESKILL_DLG))
		{
			// 전투모드에서 제스처가 보이면 전환
			m_QuickSlgDlgGroup.ShowDialog(m_dwPrevQuickDlgID, true);

			if (m_dwPrevQuickDlgID == QUICKSLOT_EVENT)
				m_pStaticSlotNumber->SetText(L"E");
			else
			{
				m_pStaticSlotNumber->SetIntToText((m_dwPrevQuickDlgID - (m_nSkillSlotPage * QUICKSLOT_PAGE_COUNT)) + 1);
			}

			ShowExpDialog(false);
			ShowFTGDialog(false);
		}
	}
}


void CDnMainDlg::ProcessZoneMapButton()
{
	bool bEnable = true;
	if (m_bPVP == true) bEnable = false;
	if (CDnPartyTask::IsActive() && GetPartyTask().IsLocalActorEnterGateReady()) bEnable = false;
	if (CDnWorld::GetInstance().GetMapType() == CDnWorld::MapTypeDungeon)
	{
		CDnGameTask * pGameTask = (CDnGameTask *)CTaskManager::GetInstance().GetTask("GameTask");
		if (!pGameTask || pGameTask->GetGameTaskType() == GameTaskType::Farm)
			bEnable = true;
		else
			bEnable = false;
	}

	if (m_bEnableButtons == false) bEnable = false;

	if (!bEnable) {
		CDnMainMenuDlg *pMainMenuDlg = GetInterface().GetMainMenuDialog();
		if (pMainMenuDlg && pMainMenuDlg->IsOpenDialog(CDnMainMenuDlg::ZONEMAP_DIALOG))
			pMainMenuDlg->ToggleShowDialog(CDnMainMenuDlg::ZONEMAP_DIALOG);
	}

	ToggleEnableButton(CDnMainMenuDlg::ZONEMAP_DIALOG, bEnable);
}

void CDnMainDlg::ProcessChatRoomButton()
{
	bool bEnable = true;
	if (CDnPartyTask::IsActive() && GetPartyTask().IsLocalActorEnterGateReady()) bEnable = false;

	if (m_bEnableButtons == false) bEnable = false;

	if (CDnInterface::IsActive() && GetInterface().IsPVP()) bEnable = false;

	if (!bEnable) {
		CDnMainMenuDlg *pMainMenuDlg = GetInterface().GetMainMenuDialog();
		if (pMainMenuDlg && pMainMenuDlg->IsOpenDialog(CDnMainMenuDlg::CHATROOM_DIALOG))
			pMainMenuDlg->ToggleShowDialog(CDnMainMenuDlg::CHATROOM_DIALOG);
	}

	ToggleEnableButton(CDnMainMenuDlg::CHATROOM_DIALOG, bEnable);
}

void CDnMainDlg::ProcessButton(DWORD dwDialogId)
{
	bool bEnable = true;
	if (CDnPartyTask::IsActive() && GetPartyTask().IsLocalActorEnterGateReady())
		bEnable = false;
	if (m_bEnableButtons == false)
		bEnable = false;
	if (CDnPartyTask::IsActive() && GetPartyTask().IsPartyProcessState(CDnPartyTask::PPS_ACCEPT_INVITE))
		bEnable = false;

	if (dwDialogId == CDnMainMenuDlg::CHARSTATUS_DIALOG || dwDialogId == CDnMainMenuDlg::INVENTORY_DIALOG)
	{
		if (CDnActor::s_hLocalActor)
		{
			CDnLocalPlayerActor *pActor = (CDnLocalPlayerActor *)CDnActor::s_hLocalActor.GetPointer();
			if (pActor->IsCannonMode())
			{
				bEnable = false;
			}
		}
	}

#if defined(PRE_ADD_MAILBOX_OPEN)
	if (dwDialogId == CDnMainMenuDlg::MAIL_DIALOG)
	{
		if (CDnWorld::MapTypeVillage != CDnWorld::GetInstance().GetMapType())
			bEnable = false;
	}
#endif // PRE_ADD_MAILBOX_OPEN

#ifdef PRE_FIX_HOLD_COSTUMEMIX_INVEN
	bool bNoToggleShowDialog = bEnable;
	if (m_HoldToggleShowDialogOnProcessButton >= 0 && m_HoldToggleShowDialogOnProcessButton == dwDialogId)
	{
		bNoToggleShowDialog = true;
	}

	if (bEnable == false && bNoToggleShowDialog == false)
#else
	if (bEnable == false)
#endif
	{
		CDnMainMenuDlg *pMainMenuDlg = GetInterface().GetMainMenuDialog();
		if (pMainMenuDlg && pMainMenuDlg->IsOpenDialog(dwDialogId))
			pMainMenuDlg->ToggleShowDialog(dwDialogId);
	}

	ToggleEnableButton(dwDialogId, bEnable);
}

void CDnMainDlg::ProcessPVPModeButton()
{
	bool bEnable = true;
	bool bEnalbeSkill = true;

	if (CDnActor::s_hLocalActor)
	{
		if (CDnActor::s_hLocalActor)
		{
			CDnPlayerActor *pPlayer = dynamic_cast<CDnPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
			if (pPlayer && pPlayer->IsObserver())
				bEnable = false;
			if (IsLockQuickSlot())
				bEnalbeSkill = false;
		}
	}

	switch (CDnWorld::GetInstance().GetMapType()) {
	case CDnWorld::MapTypeVillage:
	case CDnWorld::MapTypeWorldMap:
	{
		// ToggleEnableButton(CDnMainMenuDlg::COMMUNITY_DIALOG,!m_bPVP); // 이전코드인데 , ProcessCommuinity 버튼을 하면서 의미없는 코드였는듯  , 삭제.
		ToggleEnableButton(CDnMainMenuDlg::ZONEMAP_DIALOG, !m_bPVP);   // pvp마을에서는 특정 부분을 사용하지 못하게 설정.
	}
	break;
	case CDnWorld::MapTypeDungeon:
	{
		if (
			CDnBridgeTask::GetInstance().IsPvPGameMode(PvPCommon::GameMode::PvP_GuildWar) 		// 길드전 일때 맵 활성화 한다.
			|| CDnBridgeTask::GetInstance().IsPvPGameMode(PvPCommon::GameMode::PvP_Occupation) 	// 점령전 일때 맵 활성화 한다.
			)
		{
			ToggleEnableButton(CDnMainMenuDlg::ZONEMAP_DIALOG, m_bPVP);
		}
	}
	break;
	}

	ToggleEnableButton(CDnMainMenuDlg::QUEST_DIALOG, bEnable);
	ToggleEnableButton(CDnMainMenuDlg::SKILL_DIALOG, (bEnable && bEnalbeSkill));
}


#ifdef PRE_ADD_PVP_VILLAGE_ACCESS

void CDnMainDlg::ProcessPVPVillageAccessDlg()
{
	bool bEnable = m_bEnableButtons;

	if (CDnPartyTask::IsActive() && GetPartyTask().IsLocalActorEnterGateReady())
		bEnable = false;

	if (CDnWorld::GetInstance().GetMapType() != CDnWorld::MapTypeVillage)
		bEnable = false;

#ifdef PRE_PARTY_DB
	if (CDnActor::s_hLocalActor)
	{
		CDnLocalPlayerActor *pActor = static_cast<CDnLocalPlayerActor *>(CDnActor::s_hLocalActor.GetPointer());
		if (pActor)
		{
			if (CDnPartyTask::IsActive() && GetPartyTask().IsPartyMember(pActor->GetUniqueID()))
				bEnable = false;
		}
	}
#endif

#ifdef PRE_ADD_DWC
	bEnable = GetDWCTask().IsDWCChar();
#endif
	ToggleEnableButton(CDnMainMenuDlg::PVP_VILLAGE_ACCESS, !bEnable);
}

#endif

void CDnMainDlg::SetMainUIHotKey(int nUIWrappingKeyIndex, BYTE cVK)
{
	// 핫키를 임의로 바꿀 수 있는 기능이 없어서, 우선은 이런 식으로 처리한다.
	CDnQuickSlotDlg *pQuickSlotDlg = NULL;
	CDnLifeSkillQuickSlotDlg *pLifeSkillQuickSlotDlg = NULL;

	bool bQuickSlot = false;
	char szName[32];
	switch (nUIWrappingKeyIndex)
	{
	case IW_UI_QUICKSLOTCHANGE:
		if (GetControl("ID_BUTTON_SLOT_UP"))	GetControl("ID_BUTTON_SLOT_UP")->SetHotKey(cVK);
		break;
	case IW_UI_QUICKSLOT1:	sprintf_s(szName, _countof(szName), "ID_QUICKSLOT1"); bQuickSlot = true; break;
	case IW_UI_QUICKSLOT2:	sprintf_s(szName, _countof(szName), "ID_QUICKSLOT2"); bQuickSlot = true; break;
	case IW_UI_QUICKSLOT3:	sprintf_s(szName, _countof(szName), "ID_QUICKSLOT3"); bQuickSlot = true; break;
	case IW_UI_QUICKSLOT4:	sprintf_s(szName, _countof(szName), "ID_QUICKSLOT4"); bQuickSlot = true; break;
	case IW_UI_QUICKSLOT5:	sprintf_s(szName, _countof(szName), "ID_QUICKSLOT5"); bQuickSlot = true; break;
	case IW_UI_QUICKSLOT6:	sprintf_s(szName, _countof(szName), "ID_QUICKSLOT6"); bQuickSlot = true; break;
	case IW_UI_QUICKSLOT7:	sprintf_s(szName, _countof(szName), "ID_QUICKSLOT7"); bQuickSlot = true; break;
	case IW_UI_QUICKSLOT8:	sprintf_s(szName, _countof(szName), "ID_QUICKSLOT8"); bQuickSlot = true; break;
	case IW_UI_QUICKSLOT9:	sprintf_s(szName, _countof(szName), "ID_QUICKSLOT9"); bQuickSlot = true; break;
	case IW_UI_QUICKSLOT10:	sprintf_s(szName, _countof(szName), "ID_QUICKSLOT10"); bQuickSlot = true; break;
#if defined( PRE_ADD_SKILLSLOT_EXTEND )
	case IW_UI_QUICKSLOT11:	sprintf_s(szName, _countof(szName), "ID_QUICKSLOT11"); bQuickSlot = true; break;
	case IW_UI_QUICKSLOT12:	sprintf_s(szName, _countof(szName), "ID_QUICKSLOT12"); bQuickSlot = true; break;
#endif	// #if defined( PRE_ADD_SKILLSLOT_EXTEND )
	case IW_UI_CHAR:		if (GetControl("ID_CHAR_STATUS")) GetControl("ID_CHAR_STATUS")->SetHotKey(cVK);	break;
	case IW_UI_COMMUNITY:	if (GetControl("ID_COMMUNITY")) GetControl("ID_COMMUNITY")->SetHotKey(cVK);	break;
	case IW_UI_INVEN:		if (GetControl("ID_INVENTORY")) GetControl("ID_INVENTORY")->SetHotKey(cVK);	break;
	case IW_UI_QUEST:		if (GetControl("ID_QUEST")) GetControl("ID_QUEST")->SetHotKey(cVK);		break;
	case IW_UI_SKILL:		if (GetControl("ID_SKILL")) GetControl("ID_SKILL")->SetHotKey(cVK);		break;
	case IW_UI_MISSION:		if (GetControl("ID_MISSION")) GetControl("ID_MISSION")->SetHotKey(cVK);		break;
	case IW_UI_MAP:			if (GetControl("ID_MAP")) GetControl("ID_MAP")->SetHotKey(cVK);			break;
	case IW_UI_CHAT:		if (GetControl("ID_CHAT")) GetControl("ID_CHAT")->SetHotKey(cVK);		break;
#if defined(PRE_ADD_NPC_REPUTATION_SYSTEM)
	case IW_UI_REPUTE:		if (GetControl("ID_REPUTE")) GetControl("ID_REPUTE")->SetHotKey(cVK);	break;
#endif
	case IW_UI_EVENT:		if (GetControl("ID_EVENT")) GetControl("ID_EVENT")->SetHotKey(cVK);	break;
	case IW_UI_USERHELP:	if (GetControl("ID_USERGUIDE")) GetControl("ID_USERGUIDE")->SetHotKey(cVK);	break;
#ifdef PRE_ADD_PVP_VILLAGE_ACCESS
	case IW_UI_PVP_VILLAGE_ACCESS: if (GetControl("ID_COLOSSEUM")) GetControl("ID_COLOSSEUM")->SetHotKey(cVK); break;
#endif
#ifdef PRE_ADD_PVP_RANKING
	case IW_UI_PVPRANK_OPEN: if (GetControl("ID_RANK")) GetControl("ID_RANK")->SetHotKey(cVK); break;
#endif // PRE_ADD_PVP_RANKING
#ifdef PRE_ADD_MAIL_UI_IMPROVEMENTS	
	case IW_UI_MAILBOX_OPEN: if (GetControl("ID_MAIL")) GetControl("ID_MAIL")->SetHotKey(cVK); break;
#endif
	}

	// 퀵슬롯의 경우 자식 다이얼로그에다가 설정해야한다.
	if (bQuickSlot)
	{
		CEtUIControl *pControl;
		for (int i = 0; i < m_QuickSlgDlgGroup.GetDialogCount(); ++i)
		{
			pControl = m_QuickSlgDlgGroup.GetDialogFromIndex(i)->GetControl(szName);
			if (pControl)
				pControl->SetHotKey(cVK);
			if (m_QuickSlgDlgGroup.GetDialogFromIndex(i)->GetDialogID() == LIFESKILL_DLG)
			{
				switch (nUIWrappingKeyIndex)
				{
#if defined( PRE_ADD_SKILLSLOT_EXTEND )
				case IW_UI_QUICKSLOT1:	sprintf_s(szName, _countof(szName), "ID_QUICKSLOT13"); break;
				case IW_UI_QUICKSLOT2:	sprintf_s(szName, _countof(szName), "ID_QUICKSLOT14"); break;
				case IW_UI_QUICKSLOT3:	sprintf_s(szName, _countof(szName), "ID_QUICKSLOT15"); break;
				case IW_UI_QUICKSLOT4:	sprintf_s(szName, _countof(szName), "ID_QUICKSLOT16"); break;
				case IW_UI_QUICKSLOT5:	sprintf_s(szName, _countof(szName), "ID_QUICKSLOT17"); break;
				case IW_UI_QUICKSLOT6:	sprintf_s(szName, _countof(szName), "ID_QUICKSLOT18"); break;
				case IW_UI_QUICKSLOT7:	sprintf_s(szName, _countof(szName), "ID_QUICKSLOT19"); break;
				case IW_UI_QUICKSLOT8:	sprintf_s(szName, _countof(szName), "ID_QUICKSLOT20"); break;
				case IW_UI_QUICKSLOT9:	sprintf_s(szName, _countof(szName), "ID_QUICKSLOT21"); break;
				case IW_UI_QUICKSLOT10:	sprintf_s(szName, _countof(szName), "ID_QUICKSLOT22"); break;
				case IW_UI_QUICKSLOT11:	sprintf_s(szName, _countof(szName), "ID_QUICKSLOT23"); break;
				case IW_UI_QUICKSLOT12:	sprintf_s(szName, _countof(szName), "ID_QUICKSLOT24"); break;
#else
				case IW_UI_QUICKSLOT1:	sprintf_s(szName, _countof(szName), "ID_QUICKSLOT11"); break;
				case IW_UI_QUICKSLOT2:	sprintf_s(szName, _countof(szName), "ID_QUICKSLOT12"); break;
				case IW_UI_QUICKSLOT3:	sprintf_s(szName, _countof(szName), "ID_QUICKSLOT13"); break;
				case IW_UI_QUICKSLOT4:	sprintf_s(szName, _countof(szName), "ID_QUICKSLOT14"); break;
				case IW_UI_QUICKSLOT5:	sprintf_s(szName, _countof(szName), "ID_QUICKSLOT15"); break;
				case IW_UI_QUICKSLOT6:	sprintf_s(szName, _countof(szName), "ID_QUICKSLOT16"); break;
				case IW_UI_QUICKSLOT7:	sprintf_s(szName, _countof(szName), "ID_QUICKSLOT17"); break;
				case IW_UI_QUICKSLOT8:	sprintf_s(szName, _countof(szName), "ID_QUICKSLOT18"); break;
				case IW_UI_QUICKSLOT9:	sprintf_s(szName, _countof(szName), "ID_QUICKSLOT19"); break;
				case IW_UI_QUICKSLOT10:	sprintf_s(szName, _countof(szName), "ID_QUICKSLOT20"); break;
#endif	// #if defined( PRE_ADD_SKILLSLOT_EXTEND )
				}
				pControl = m_QuickSlgDlgGroup.GetDialogFromIndex(i)->GetControl(szName);
				if (pControl)
					pControl->SetHotKey(cVK);
			}
		}
	}
}

void CDnMainDlg::ShowTimeEventAlarm(bool bShow)
{
	m_bShowTimeEventAlarm = bShow;
	for (int i = 0; i<6; i++) m_pStaticTimeEventIcon[i]->Show(bShow);
	m_pStaticTimeEventText->Show(bShow);
	if (bShow == false) m_fTimeEventHideDelta = 0.f;
}

void CDnMainDlg::UpdateTimeEventAlarm(char cType, int nLastEventID)
{
	if (!CDnTimeEventTask::IsActive()) return;

	m_cUpdateEventType = cType;
	m_nTimeEventID = nLastEventID;
	m_fTimeEventHideDelta = 10.f;

	switch (cType) {
	case 0: // 완료
		if (nLastEventID > 0) {
			WCHAR szTooltip[256] = L"";
			CDnTimeEventTask *pTask = &CDnTimeEventTask::GetInstance();
			CDnTimeEventTask::TimeEventInfoStruct *pTimeEvent = pTask->FindTimeEventInfo(nLastEventID);
			if (pTimeEvent)
				swprintf_s(szTooltip, GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4834), pTimeEvent->szTitle.c_str());
			m_pStaticTimeEventIcon[4]->SetTooltipText(szTooltip);
		}
		break;
	case -1: // 삭제
		break;
	}
}

void CDnMainDlg::ProcessTimeEventAlarm(float fElapsedTime)
{
	int nIconState = -1;
	bool bBlink = false;
	WCHAR szRemainTime[64] = L"";


	if (m_bShowTimeEventAlarm) {
		if (!CDnTimeEventTask::IsActive()) return;

		if (CDnWorld::GetInstance().GetMapType() == CDnWorld::MapTypeDungeon) {
			CDnGameTask *pGameTask = (CDnGameTask *)CTaskManager::GetInstance().GetTask("GameTask");
			if (pGameTask && !pGameTask->IsSyncComplete()) {
				for (int i = 0; i<6; i++) {
					m_pStaticTimeEventIcon[i]->SetBlink(false);
					m_pStaticTimeEventIcon[i]->Show(false);
				}
				m_pStaticTimeEventText->SetText(L"");
				return;
			}
		}
		if (m_fTimeEventHideDelta > 0.f) {
			m_fTimeEventHideDelta -= fElapsedTime;
			switch (m_cUpdateEventType) {
			case -1: nIconState = 5; break;
			case 0: nIconState = 4; break;
			}
			if (m_fTimeEventHideDelta <= 0.f) {
				m_cUpdateEventType = -1;
				m_nTimeEventID = 0;
				m_fTimeEventHideDelta = 0.f;
			}
		}
		else {
			CDnTimeEventTask *pTask = &CDnTimeEventTask::GetInstance();
			int nEventID = pTask->GetTimeEventAlarm();
			if (nEventID > 0) {
				CDnTimeEventTask::TimeEventInfoStruct *pTimeEvent = pTask->FindTimeEventInfo(nEventID);
				if (pTimeEvent) {
					float fProgress = (1.f / (float)(pTimeEvent->nMaintenanceTime / 1000)) * (float)(pTimeEvent->nRemainTime / 1000);
					if (fProgress > 0.75f) nIconState = 0;
					else if (fProgress > 0.5f) nIconState = 1;
					else if (fProgress > 0.25f) nIconState = 2;
					else if (fProgress > 0.f) nIconState = 3;
					else if (fProgress <= 0.f) nIconState = 4;

					int nValue = (int)(pTimeEvent->nRemainTime / 1000);
					int nHour = nValue / 3600;
					int nMin = (nValue % 3600) / 60;
					int nSec = nValue % 60;

					if (fProgress >= 0.f) {
						//swprintf_s( szRemainTime, L"%02d:%02d:%02d", nHour, nMin, nSec );
						_snwprintf_s(szRemainTime, _countof(szRemainTime), _TRUNCATE, L"%02d:%02d:%02d", nHour, nMin, nSec);
					}
					if (pTimeEvent->nRemainTime > 0 && (pTimeEvent->nRemainTime / 1000) < 60 * 5) bBlink = true;

					if (nIconState > -1 && fProgress > 0.f) {
						WCHAR szTooltip[1024] = L"";
						std::wstring szRewardStr;
						int nCount = (int)pTimeEvent->pVecRewardItemList.size();
						for (int i = 0; i<nCount; i++) {
							szRewardStr += pTimeEvent->pVecRewardItemList[i]->GetName();
							if (i != nCount - 1) szRewardStr += L", ";
						}
#ifdef _US
						_snwprintf_s(szTooltip, _countof(szTooltip), _TRUNCATE, GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4831), pTimeEvent->szTitle.c_str(), nHour, nMin, nSec,
							pTimeEvent->BeginTime.GetMonth(), pTimeEvent->BeginTime.GetDay(), pTimeEvent->BeginTime.GetYear(), pTimeEvent->BeginTime.GetHour(), pTimeEvent->BeginTime.GetMinute(),
							pTimeEvent->EndTime.GetMonth(), pTimeEvent->EndTime.GetDay(), pTimeEvent->EndTime.GetYear(), pTimeEvent->EndTime.GetHour(), pTimeEvent->EndTime.GetMinute(), szRewardStr.c_str());

#elif _RU
						_snwprintf_s(szTooltip, _countof(szTooltip), _TRUNCATE, GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4831), pTimeEvent->szTitle.c_str(), nHour, nMin, nSec,
							pTimeEvent->BeginTime.GetDay(), pTimeEvent->BeginTime.GetMonth(), pTimeEvent->BeginTime.GetYear(), pTimeEvent->BeginTime.GetHour(), pTimeEvent->BeginTime.GetMinute(),
							pTimeEvent->EndTime.GetDay(), pTimeEvent->EndTime.GetMonth(), pTimeEvent->EndTime.GetYear(), pTimeEvent->EndTime.GetHour(), pTimeEvent->EndTime.GetMinute(), szRewardStr.c_str());
#else // _US
						_snwprintf_s(szTooltip, _countof(szTooltip), _TRUNCATE, GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4831), pTimeEvent->szTitle.c_str(), nHour, nMin, nSec,
							pTimeEvent->BeginTime.GetYear(), pTimeEvent->BeginTime.GetMonth(), pTimeEvent->BeginTime.GetDay(), pTimeEvent->BeginTime.GetHour(), pTimeEvent->BeginTime.GetMinute(),
							pTimeEvent->EndTime.GetYear(), pTimeEvent->EndTime.GetMonth(), pTimeEvent->EndTime.GetDay(), pTimeEvent->EndTime.GetHour(), pTimeEvent->EndTime.GetMinute(), szRewardStr.c_str());
#endif // _US
						if (nIconState > -1) m_pStaticTimeEventIcon[nIconState]->SetTooltipText(szTooltip);
					}
				}
			}
		}
	}

	for (int i = 0; i<6; i++) {
		m_pStaticTimeEventIcon[i]->SetBlink((i == nIconState) ? bBlink : false);
		m_pStaticTimeEventIcon[i]->Show((i == nIconState) ? true : false);
	}
	m_pStaticTimeEventText->SetText(szRemainTime);
}

bool CDnMainDlg::IsShowGuildWarSkillSlotDlg()
{
	if (m_pDnGuildWarSkillSlotDlg)
		return m_pDnGuildWarSkillSlotDlg->IsShow();

	return false;
}

void CDnMainDlg::ShowGuildWarSkillSlotDlg(bool bShow, bool bFixedGuildSkill)
{
	if (m_pDnGuildWarSkillSlotDlg)
	{
		m_pDnGuildWarSkillSlotDlg->Show(bShow);
		m_pDnGuildWarSkillSlotDlg->SetFixedMode(bFixedGuildSkill);
	}
}

void CDnMainDlg::UseGuildWarSkill(int nIndex)
{
	if (m_pDnGuildWarSkillSlotDlg)
		m_pDnGuildWarSkillSlotDlg->UseGuildWarSkill(nIndex);
}

#ifdef PRE_TEST_ANIMATION_UI
bool CDnMainDlg::IsShowAniTestDlg() const
{
	return (m_pAniTestDlg && m_pAniTestDlg->IsShow());
}
#endif

#ifdef PRE_ADD_LEVELUP_GUIDE
void CDnMainDlg::SetLevelUpSkillAlarm(bool bSet)
{
	m_bLevelUpSkillAlarm = bSet;
	m_fLevelUpSkillAlarmTime = 0.f;

	BlinkMenuButton(CDnMainMenuDlg::SKILL_DIALOG);
}
#endif

#ifdef PRE_SPECIALBOX
void CDnMainDlg::SetEventReceiverAlarm(bool bNew)
{
	if (bNew == true)
	{
		m_fEventReceiverAlarmTime = 0.f;
		m_bEventReceiverAlarm = true;
		m_pButtonEventReceiverAlarm->SetBlink(true);
	}

	m_pButtonEventReceiverAlarm->Show(true);
	CDnEventReceiverTabDlg::ResetDelayTime();
}

void CDnMainDlg::CloseEventReceiverAlarm()
{
	m_fEventReceiverAlarmTime = 0.f;
	m_bEventReceiverAlarm = false;

	m_pButtonEventReceiverAlarm->Show(false);
	m_pStaticEventReceiverBalloon->Show(false);
	m_pStaticEventReceiverText->Show(false);
}
#endif


void CDnMainDlg::ChangeSkillSlotPage(int nPage)
{
	if (0 > nPage || nPage >= QUICKSLOT_SKILL_PAGE_COUNT)
		return;
	if (m_nSkillSlotPage == nPage)
		return;

	m_nSkillSlotPage = nPage;

	if (m_bLockQuickSlot)
		return;

	int nDefaultDlgID = QUICKSLOT_SKILL_PAGE_COUNT * nPage;
	m_dwPrevQuickDlgID = nDefaultDlgID;

	if (!m_QuickSlgDlgGroup.IsShowDialog(LIFESKILL_DLG))
	{
		m_QuickSlgDlgGroup.ShowDialog((DWORD)nDefaultDlgID, true);
		m_pStaticSlotNumber->SetIntToText((nDefaultDlgID - (m_nSkillSlotPage * QUICKSLOT_PAGE_COUNT)) + 1);
	}

	ShowExpDialog(false);
	ShowFTGDialog(false);
}

bool CDnMainDlg::IsCurrentSkillPageIndex(int nPageIndex)
{
	if (m_nSkillSlotPage < DualSkill::Type::Primary || m_nSkillSlotPage > DualSkill::Type::Secondary)
		return false;

	if ((nPageIndex >= ((m_nSkillSlotPage * QUICKSLOT_SKILL_PAGE_COUNT) + 0)) && nPageIndex < ((m_nSkillSlotPage * QUICKSLOT_SKILL_PAGE_COUNT) + QUICKSLOT_PAGE_COUNT))
		return true;

	return false;
}

#ifdef PRE_FIX_HOLD_COSTUMEMIX_INVEN
void CDnMainDlg::HoldToggleShowDialogOnProcessButton(bool bHold, DWORD dwDialogId)
{
	m_HoldToggleShowDialogOnProcessButton = bHold ? (int)dwDialogId : -1;
}
#endif

void CDnMainDlg::SetPetFoodInfo(int nPetFoodItemID)
{
	if (m_pFoodItemSlot == NULL)
		return;

	ResetPetFoodInfo();
	int nPetFoodItemCount = GetPetTask().GetPetFoodItemCount(nPetFoodItemID);
	if (nPetFoodItemCount == 0)
		return;

	TItemInfo itemInfo;
	if (CDnItem::MakeItemInfo(nPetFoodItemID, 1, itemInfo))
	{
		m_pFoodItem = GetItemTask().CreateItem(itemInfo);

		if (m_pFoodItem && m_pFoodItem->GetItemType() == ITEMTYPE_PET_FOOD)
		{
			m_pFoodItemSlot->SetItem(m_pFoodItem, CDnSlotButton::NO_COUNTABLE_RENDER);
			m_pFoodItemSlot->Show(true);
			WCHAR szPetFoodCount[12] = L"";
			swprintf_s(szPetFoodCount, L"%d", nPetFoodItemCount);
			m_pStaticFoodItemCount->SetText(szPetFoodCount);
			m_pStaticFoodItemCount->Show(true);
		}
	}
}

void CDnMainDlg::RefreshPetFoodCount()
{
	if (m_pFoodItem == NULL)
		return;

	int nPetFoodItemCount = GetPetTask().GetPetFoodItemCount(GetPetTask().GetPetOption().m_nFoodItemID);
	if (nPetFoodItemCount == 0)
	{
		ResetPetFoodInfo();
	}
	else if (nPetFoodItemCount > 0)
	{
		WCHAR szPetFoodCount[12] = L"";
		swprintf_s(szPetFoodCount, L"%d", nPetFoodItemCount);
		m_pStaticFoodItemCount->SetText(szPetFoodCount);
	}
}

void CDnMainDlg::ResetPetFoodInfo()
{
	SAFE_DELETE(m_pFoodItem);
	m_pFoodItemSlot->ResetSlot();
	m_pFoodItemSlot->Show(false);
	m_pStaticFoodItemCount->SetText(L"");
	m_pStaticFoodItemCount->Show(false);
}

#if defined(PRE_ADD_68286)
bool CDnMainDlg::IsAlarmIconClick(POINT& MousePoint)
{
	bool isAlarmIconClick = false;

	float fMouseX, fMouseY;
	PointToFloat(MousePoint, fMouseX, fMouseY);

	if (m_pStaticPost && m_pStaticPost->IsShow() && m_pStaticPost->IsInside(fMouseX, fMouseY) == true)
	{
		isAlarmIconClick = true;
	}

	if (m_pStaticCashShopGift && m_pStaticCashShopGift->IsShow() && m_pStaticCashShopGift->IsInside(fMouseX, fMouseY) == true)
	{
		isAlarmIconClick = true;
	}

	return isAlarmIconClick;
}
#endif // PRE_ADD_68286

#ifdef PRE_MOD_APPELLATIONBOOK_RENEWAL

void CDnMainDlg::ShowAppellationNotify(bool bShow)
{
	m_pStaticNewAppellationAlarm->Show(bShow);
	m_pStaticNewAppellationAlarmText->Show(bShow);
}
#endif // PRE_MOD_APPELLATIONBOOK_RENEWAL

#if defined( PRE_ALTEIAWORLD_EXPLORE )
void CDnMainDlg::SetAlteaDiceIcon()
{
	m_bAlteaDiceShow = false;

	CDnVillageTask* pVillageTask = (CDnVillageTask*)(CTaskManager::GetInstance().GetTask("VillageTask"));
	if (NULL == pVillageTask || CVillageClientSession::WorldVillage != pVillageTask->GetVillageType())
		return;

	DNTableFileFormat * pSox = GetDNTable(CDnTableDB::TALTEIATIME);
	if (NULL == pSox)
		return;

	CTimeSet tCurTime;
	for (int itr = 0; itr < pSox->GetItemCount(); ++itr)
	{
		int nItemID = pSox->GetItemID(itr);

		char *szPeriod = pSox->GetFieldFromLablePtr(nItemID, "_EventPeriod")->GetString();
		std::vector<string> vEventPeriod;
		TokenizeA(szPeriod, vEventPeriod, ":");
		if (vEventPeriod.size() != 2) continue;

		CTimeSet tBeginSet(vEventPeriod[0].c_str(), true);
		CTimeSet tEndSet(vEventPeriod[1].c_str(), true);

		if (tCurTime.GetTimeT64_LC() < tEndSet.GetTimeT64_LC()
			&& tCurTime.GetTimeT64_LC() > tBeginSet.GetTimeT64_LC())
		{
			m_bAlteaDiceShow = true;
			break;
		}
	}

	if (false == m_bAlteaDiceShow)
		return;

	m_bAlteaDiceShow = false;

	if (!CDnActor::s_hLocalActor)
		return;

	const int nLevel = CDnActor::s_hLocalActor->GetLevel();
	const int nLimitLevel = (int)CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::AlteiaWorldEnterLimitLevel);

	if (nLimitLevel <= nLevel)
		m_bAlteaDiceShow = true;
}

void CDnMainDlg::SetAlteaDiceAlarm()
{
	m_bAlteaDiceAlarm = true;
	m_fAlteaDiceAlarmTime = 0.0f;
}

void CDnMainDlg::UpdateAlteaDice(float fElapsedTime)
{
	if (true == m_bAlteaDiceShow)
	{
		if (m_bAlteaDiceAlarm)
		{
			if (3.0f <= m_fAlteaDiceAlarmTime)
			{
				m_bAlteaDiceAlarm = false;
				m_pAlteaDiceIcon->Show(false);
			}
			else
			{
				static int nTimeBack = 0;
				int nBlinkTime = int(m_fAlteaDiceAlarmTime * 10.0f);

				if (((nBlinkTime % 3) == 0) && (nTimeBack != nBlinkTime))
				{
					nTimeBack = nBlinkTime;
					m_pAlteaDiceIcon->Show(!m_pAlteaDiceIcon->IsShow());
				}
				m_fAlteaDiceAlarmTime += fElapsedTime;
			}
		}
		else
			m_pAlteaDiceIcon->Show(true);
	}
	else
		m_pAlteaDiceIcon->Show(false);
}
#endif	// #if defined( PRE_ALTEIAWORLD_EXPLORE )

#ifdef PRE_ADD_STAMPSYSTEM
void CDnMainDlg::NpcTalk(int npcID)
{
	// #80220 - 현재 퀘스트 과제 버튼 클릭 시 해당 NPC와 대화를 해야만 안내 화살표가 사라지는 것을, 아무 NPC와 대화를 해도 사라지도록 수정합니다.
	//	if( !m_bSearching && m_SearchedNpcID != npcID )
	//		return;

	FindComplete();
}

void CDnMainDlg::NPCSearch(int mapID, int npcID)
{
	if (m_bSearching && m_SearchedNpcID == npcID)
		return;

	// 세인트헤이븐 에서만 작동.
	if (CGlobalInfo::GetInstance().m_nCurrentMapIndex != mapID)
	{
		m_SearchedNpcID = -1;
		m_bSearching = false;

		GetInterface().ShowCaptionDialog(CDnInterface::emCAPTION_TYPE::typeCaption2, 8320, textcolor::YELLOW); // "현재 위치에서는 NPC 안내가 불가능합니다"
		return;
	}


	m_SearchedNpcID = npcID;
	m_bSearching = false;

	CDnWorldData * pWorldData = CDnWorldDataManager::GetInstance().GetWorldData(CGlobalInfo::GetInstance().m_nCurrentMapIndex);
	if (!pWorldData)
		return;

	EtVector2 vPos(0.0f, 0.0f);
	int nSize = pWorldData->GetNpcCount();
	int i = 0;
	for (i = 0; i<nSize; ++i)
	{
		CDnWorldData::NpcData * pNpcData = pWorldData->GetNpcData(i);
		if (pNpcData->nNpcID == npcID)
		{
			m_bSearching = true;
			vPos = pNpcData->vPos;
			break;
		}
		if (i == nSize)
		{
			CDnNPCActor * pNpcActor = CDnNPCActor::FindNpcActorFromID(npcID);
			if (pNpcActor)
			{
				m_bSearching = true;
				vPos = EtVector2(pNpcActor->GetPosition()->x, pNpcActor->GetPosition()->z);
			}
		}
	}

	m_vNPCPos.x = vPos.x;
	m_vNPCPos.y = 0.0f;
	m_vNPCPos.z = vPos.y;

	if (m_bSearching)
	{
		GetInterface().ShowCaptionDialog(CDnInterface::emCAPTION_TYPE::typeCaption2, 8319, textcolor::YELLOW); // "퀘스트 NPC 안내를 시작합니다"

		m_NPCArrowDlg->Show(true);

		/*// test.
		DNTableFileFormat * pTable = GetDNTable( CDnTableDB::TNPC );
		if( pTable )
		{
		DNTableCell * pCell = pTable->GetFieldFromLablePtr( npcID, "_NameID" );
		if( pCell )
		GetInterface().AddChatMessage( eChatType::CHATTYPE_NORMAL, L"", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, pCell->GetInteger() ) );
		}	*/


	}

	else
	{
		// 세인트헤이븐 에서만 작동.	
		m_SearchedNpcID = -1;
		m_bSearching = false;

		//GetInterface().ShowCaptionDialog( CDnInterface::emCAPTION_TYPE::typeCaption2, 8320, textcolor::YELLOW ); // "현재 위치에서는 NPC 안내가 불가능합니다"		
		GetInterface().ShowCaptionDialog(CDnInterface::emCAPTION_TYPE::typeCaption2, L"세인트헤이븐에 존재하지 않는 NPC입니다 ( String ID 필요 )", textcolor::YELLOW); // 
	}

}

void CDnMainDlg::RotetaNPCArrow()
{
	if (!CDnActor::s_hLocalActor)
		return;

	DnCameraHandle hCamera = CDnCamera::GetActiveCamera();
	if (hCamera)
	{
		EtVector3 vZDir = hCamera->GetMatEx()->m_vZAxis;
		m_camDir = vZDir;

		vZDir.y = 0.0f;
		EtVec3Normalize(&vZDir, &vZDir);

		EtVector3 vCharPos = *CDnActor::s_hLocalActor->GetPosition();
		vCharPos.y = 0.0f;
		EtVector3 vDir = m_vNPCPos - vCharPos;

		// test.
		//GetInterface().AddChatMessage( eChatType::CHATTYPE_NORMAL, L"", FormatW( L"%f", EtVec3Length( &vDir ) ).c_str() );

		EtVector3 vNor;
		EtVec3Normalize(&vNor, &vDir);

		// 회전각.
		float fDegree = EtAcos(EtVec3Dot(&vZDir, &vNor));

		if (EtVec3Dot(&(hCamera->GetMatEx()->m_vXAxis), &vNor) < 0.0f)
			fDegree = -fDegree;

		m_NPCArrowDlg->GetStaticArrow()->SetRotate(EtToDegree(fDegree));
	}
}


void CDnMainDlg::FindComplete()
{
	m_SearchedNpcID = -1;
	m_bSearching = false; // 찾는중
	m_fRotDegree = 0.0f;

	if (m_NPCArrowDlg)
		m_NPCArrowDlg->Show(false);
}

#endif // PRE_ADD_STAMPSYSTEM

void CDnMainDlg::OnLevelUp(int nLevel)
{
#if defined( PRE_ALTEIAWORLD_EXPLORE )
	SetAlteaDiceIcon();
#endif	// #if defined( PRE_ALTEIAWORLD_EXPLORE )
}

#endif