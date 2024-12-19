#include "StdAfx.h"
#include "DnGaugeDlg.h"
#include "DnGaugeFace.h"
#include "DnActor.h"
#include "DnLocalPlayerActor.h"
#include "GameOption.h"
#include "DnWorld.h"
#include "DnInterface.h"
#include "DnBlow.h"
#include "DnBuffUIMng.h"
#include "DnPartyTask.h"
#include "DnMonsterState.h"
#include "TaskManager.h"
#include "DnGameTask.h"
#include "DnItemTask.h"
#include "SystemSendPacket.h"
#include "DnUIString.h"
#include "DnSkillTask.h"
#include "DnPlayerCamera.h"

#ifdef _USE_VOICECHAT
#include "VoiceChatClient.h"
#include "DNVoiceChatTask.h"
#include "InputWrapper.h"
#endif

#ifdef PRE_ADD_VIP
#include "DnPopupVIPDlg.h"
#endif

#include "DnInterfaceString.h"
#include "DnContextMenuDlg.h"
#include "DnTradeTask.h"
#include "DnGuildTask.h"
#include "DnLifeConditionDlg.h"
#include "DnBubbleSystem.h"
#include "DnCommonUtil.h"

#include "DnMasterTask.h"
#include "DnSimpleTooltipDlg.h"

#ifdef PRE_MOD_NESTREBIRTH
#include "DnNestRebirthTooltipDlg.h"
#endif

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnGaugeDlg::CDnGaugeDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback)
	: CDnCustomDlg(dialogType, pParentDialog, nID, pCallback)
{
	m_nSessionID = 0;
	m_pFace = NULL;
	m_pStaticTextHP = NULL;
	m_pHPBar = NULL;
	m_pSPBar = NULL;
	m_pPlayerName = NULL;
	m_pPlayerNameInParty = NULL;
	m_pStaticHP = NULL;
	m_pStaticSP = NULL;
	m_FaceType = FT_PLAYER;
	m_pStaticGateReady = NULL;
	m_pStaticDamage = NULL;
	m_pStaticLoading = NULL;
	m_bDamage = false;
	m_fFaceTime = 0.0f;
	m_pCoinFree = NULL;
	m_pCoinCash = NULL;
	m_pPartyMaster = NULL;
	m_nMonsterGrade = CDnMonsterState::Minion;
	m_nStageConstructionLevel = 0;
	m_bRefreshStaticTextHP = false;
	m_pRebirthLimitBack = NULL;
#ifdef PRE_MOD_NESTREBIRTH
	m_pRebirthLimitCoinCount = NULL;
	m_pRebirthLimitCoinIcon = NULL;
	m_pRebirthLimitItemCount = NULL;
	m_pRebirthLimitItemIcon = NULL;
	m_nPrevUsableRebirthItemCount = -1;

	m_pNestRebirthTooltipDlg = NULL;
#endif
	m_pRebirthLimit = NULL;
	m_nPrevUsableCoinCount = -1;
	m_bLifeConditionDlg = true;

	int i = 0;
	for (; i < MAX_BUFF_SLOT; ++i)
	{
		m_pBuffCounter[i] = NULL;
		m_pBuffTextureCtrl[i] = NULL;
		m_pBubbleCoolTimeTextureCtrl[i] = NULL;
	}

	for (int i = 0; i < NUM_VOICE_STATE; ++i)
		m_pButtonVoice[i] = NULL;
	m_pButtonComplaint = NULL;

	m_pStaticPartyLootInfo = NULL;

	m_pStaticPartyItemLootRank = NULL;

	m_pButtonPartyQuit = NULL;

	m_pStaticItemRepair0 = NULL;
	m_pStaticItemRepair1 = NULL;
	m_pStaticItemRepair2 = NULL;

	m_pStaticVIPNormal = NULL;
	m_pStaticVIPExpire = NULL;
#ifdef PRE_ADD_VIP
	m_pPopupVIPDlg = NULL;
#endif
	m_pKeepPartyIcon = NULL;
	m_pKeepMaster = NULL;
	m_pKeepClassmate = NULL;
#ifdef PRE_ADD_IMPROVE_MASTER_OBSERVER
	m_pPartySelection = NULL;
#endif
#ifdef PRE_ADD_PVP_SHOW_PARTY_GAUGE
	m_pStaticShowPvpGauge = NULL;
	m_pCheckShowPvpGauge = NULL;
#endif
	m_pBuffUIMng = NULL;

	for (int itr = 0; itr < eMax_Icon; ++itr)
		m_pIconPosition[itr] = itr;
}

CDnGaugeDlg::~CDnGaugeDlg(void)
{
	for (int i = 0; i<BUFF_TEXTURE_COUNT; i++)
	{
		SAFE_RELEASE_SPTR(m_hSkillIconList[i]);
	}

	// 	SAFE_RELEASE_SPTR(m_hSkillIconLight);

	SAFE_DELETE(m_pBuffUIMng);
#ifdef PRE_ADD_VIP
	SAFE_DELETE(m_pPopupVIPDlg);
#endif
#ifdef PRE_MOD_NESTREBIRTH
	SAFE_DELETE(m_pNestRebirthTooltipDlg);
#endif
}

void CDnGaugeDlg::Initialize(const char *pFileName, GAUGE_FACE_TYPE FaceType, bool bShow)
{
	m_FaceType = FaceType;
	if (FaceType == FT_MINI_PLAYER)
		m_pBuffUIMng = new CDnBuffUIMng(MAX_BUFF_ONLY_SLOT, MAX_BUBBLE_ONLY_SLOT, CDnBuffUIMng::ePartyMember);
	else
		m_pBuffUIMng = new CDnBuffUIMng(MAX_BUFF_ONLY_SLOT, MAX_BUBBLE_ONLY_SLOT, CDnBuffUIMng::ePlayer);
	CEtUIDialog::Initialize(pFileName, bShow);

#ifdef PRE_ADD_VIP
	m_pPopupVIPDlg = new CDnPopupVIPDlg(UI_TYPE_TOP_MSG);
	m_pPopupVIPDlg->Initialize(false);
#endif

#ifdef PRE_MOD_NESTREBIRTH
	m_pNestRebirthTooltipDlg = new CDnNestRebirthTooltipDlg(UI_TYPE_TOP_MSG, this);
	m_pNestRebirthTooltipDlg->Initialize(false);
#endif
}

void CDnGaugeDlg::InitialUpdate()
{
	if (m_FaceType != FT_MONSTER_PARTS)
	{
		m_pFace = GetControl< CDnGaugeFace >("ID_PLAYER_FACE");
		m_pFace->SetFaceType(m_FaceType);
	}

	m_pHPBar = GetControl< CEtUIProgressBar >("ID_HP_GAUGE");

	if (m_FaceType == FT_ENEMY || m_FaceType == FT_MINI_ENEMY) {
		m_pSPBar = GetControl< CEtUIProgressBar >("ID_SUPER_GAUGE");
	}
	else if (m_FaceType != FT_MONSTER_PARTS) {
		m_pSPBar = GetControl< CEtUIProgressBar >("ID_MP_GAUGE");
	}

	if (m_FaceType == FT_ENEMY || m_FaceType == FT_MINI_ENEMY) {
		m_pStaticTextHP = GetControl< CEtUIStatic >("ID_TEXT_HP");
		m_pStaticTextHP->SetText(L"");
		m_pHPBar->UseLayer(true, 1, 0);
		m_pHPBar->UseShadeProcessBar(8, 7);
		m_pHPBar->UseGaugeEndElement(6);
	}

	m_pHPBar->SetSmooth(true);

	if (m_FaceType != FT_MONSTER_PARTS) {
		m_pSPBar->SetSmooth(true);
	}

	if (m_FaceType == FT_ENEMY) {
		CONTROL(Static, ID_STATIC_NESTBOSS)->Show(false);
		CONTROL(ProgressBar, ID_HP_NESTGAUGE)->Show(false);
		CONTROL(ProgressBar, ID_SUPER_NESTGAUGE)->Show(false);

		CONTROL(ProgressBar, ID_HP_NESTGAUGE)->UseLayer(true, 1, 0);
		CONTROL(ProgressBar, ID_HP_NESTGAUGE)->UseShadeProcessBar(8, 7);
		CONTROL(ProgressBar, ID_HP_NESTGAUGE)->UseGaugeEndElement(6);
	}

	m_pPlayerName = GetControl< CEtUIStatic >("ID_PLAYER_NAME");

	if (m_FaceType != FT_MONSTER_PARTS) {
		m_pStaticDamage = GetControl< CEtUIStatic >("ID_STATIC_DAMAGE");
	}


	if (m_FaceType == FT_PLAYER || m_FaceType == FT_MINI_PLAYER)
	{
		m_pStaticGateReady = GetControl< CEtUIStatic >("ID_GATE_READY");
		m_pStaticGateReady->Show(false);
		m_pStaticLoading = GetControl< CEtUIStatic >("ID_LOADING");
		m_pStaticLoading->Show(false);
		m_pPartyMaster = GetControl<CEtUIStatic>("ID_PARTY_MASTER_ICON");
		m_pPartyMaster->Show(false);
#ifdef PRE_ADD_IMPROVE_MASTER_OBSERVER
		m_pPartySelection = GetControl<CEtUIStatic>("ID_STATIC_SELECT");
		m_pPartySelection->Show(false);
#endif
	}

	if (m_FaceType == FT_PLAYER)
	{
		m_pStaticHP = GetControl< CEtUIStatic >("ID_STATIC_HP");
		m_pStaticHP->SetText(L"0/0");
		m_pStaticSP = GetControl< CEtUIStatic >("ID_STATIC_MP");
		m_pStaticSP->SetText(L"0/0");

		m_pCoinFree = GetControl< CEtUIStatic >("ID_COIN_FREE");
		m_pCoinCash = GetControl< CEtUIStatic >("ID_COIN_CASH");

		m_pRebirthLimitBack = GetControl< CEtUIStatic >("ID_STATIC_USE_COIN");
		m_pRebirthLimitBack->Show(false);

#ifdef PRE_MOD_NESTREBIRTH
		m_pRebirthLimitCoinCount = GetControl<CEtUIStatic>("ID_COIN_NEST");
		m_pRebirthLimitCoinCount->Show(false);

		m_pRebirthLimitItemCount = GetControl<CEtUIStatic>("ID_COIN_APPLE");
		m_pRebirthLimitItemCount->Show(false);

		m_pRebirthLimitCoinIcon = GetControl<CEtUIStatic>("ID_STATIC0");
		m_pRebirthLimitCoinIcon->Show(false);

		m_pRebirthLimitItemIcon = GetControl<CEtUIStatic>("ID_STATIC2");
		m_pRebirthLimitItemIcon->Show(false);
#endif
		m_pRebirthLimit = GetControl< CEtUIStatic >("ID_COIN_REMAIN");
		if (m_pRebirthLimit)
			m_pRebirthLimit->Show(false);

		m_pPlayerNameInParty = GetControl<CEtUIStatic>("ID_PLAYER_PARTYNAME");
		m_pPlayerNameInParty->Show(false);

		m_pStaticPartyLootInfo = GetControl<CEtUIStatic>("ID_STATIC_PROOTING");
		m_pStaticPartyLootInfo->Show(false);
		m_pButtonPartyQuit = GetControl<CEtUIButton>("ID_BUTTON_QUIT0");
		m_pButtonPartyQuit->Show(false);
		m_pStaticPartyItemLootRank = GetControl<CEtUIStatic>("ID_STATIC_RULE");
		m_pStaticPartyItemLootRank->Show(false);

		m_pStaticItemRepair0 = GetControl<CEtUIStatic>("ID_REPAIR0");
		m_pStaticItemRepair1 = GetControl<CEtUIStatic>("ID_REPAIR1");
		m_pStaticItemRepair2 = GetControl<CEtUIStatic>("ID_REPAIR2");
		m_pStaticItemRepair0->Show(false);
		m_pStaticItemRepair1->Show(false);
		m_pStaticItemRepair2->Show(false);
		m_pIconCoord[eRepair_Icon] = m_pStaticItemRepair0->GetBaseUICoord();

		m_pStaticVIPNormal = GetControl<CEtUIStatic>("ID_STATIC_VIP0");
		if (m_pStaticVIPNormal)
			m_pStaticVIPNormal->Show(false);

		m_pStaticVIPExpire = GetControl<CEtUIStatic>("ID_STATIC_VIP1");
		if (m_pStaticVIPExpire)
			m_pStaticVIPExpire->Show(false);

		m_pKeepPartyIcon = GetControl<CEtUIStatic>("ID_STATIC_FRIEND");
		if (m_pKeepPartyIcon)
		{
			m_pKeepPartyIcon->Show(false);
			m_pIconCoord[eParty_Icon] = m_pKeepPartyIcon->GetBaseUICoord();
		}
		m_pKeepMaster = GetControl<CEtUIStatic>("ID_STATIC_MASTER");
		m_pKeepClassmate = GetControl<CEtUIStatic>("ID_STATIC_PUPIL");
		if (m_pKeepMaster)
		{
			m_pKeepMaster->Show(false);
			m_pIconCoord[eMaster_Icon] = m_pKeepMaster->GetBaseUICoord();
		}
		if (m_pKeepClassmate)
		{
			m_pKeepClassmate->Show(false);
			m_pIconCoord[eClassmate_Icon] = m_pKeepClassmate->GetBaseUICoord();
		}
#ifdef PRE_ADD_PVP_SHOW_PARTY_GAUGE
		m_pStaticShowPvpGauge = GetControl<CEtUIStatic>("ID_TEXT_COLOPARTY");
		m_pCheckShowPvpGauge = GetControl<CEtUICheckBox>("ID_CHECKBOX_COLOPARTY");
#endif
	}

	if (m_FaceType == FT_PLAYER || m_FaceType == FT_MINI_PLAYER)
	{
		m_pButtonVoice[VoiceNone] = GetControl<CEtUIButton>("ID_BT_NON");
		m_pButtonVoice[VoiceReady] = GetControl<CEtUIButton>("ID_BT_ON");
		m_pButtonVoice[VoiceActive] = GetControl<CEtUIButton>("ID_BT_SPEAK");
		m_pButtonVoice[VoiceMute] = GetControl<CEtUIButton>("ID_BT_ON_NON");

		for (int i = 0; i < NUM_VOICE_STATE; ++i)
			m_pButtonVoice[i]->Show(false);

		m_hSkillIconList[0] = LoadResource(CEtResourceMng::GetInstance().GetFullName("BuffIcon01.dds").c_str(), RT_TEXTURE);
		m_hSkillIconList[1] = LoadResource(CEtResourceMng::GetInstance().GetFullName("BuffIcon02.dds").c_str(), RT_TEXTURE);
	}

	if (m_FaceType == FT_MINI_PLAYER) {
		int i = 0;
		std::string slotName[MAX_BUFF_SLOT_MINIGAUGE];
		std::string counterName[MAX_BUFF_SLOT_MINIGAUGE];
		for (; i < MAX_BUFF_SLOT_MINIGAUGE; ++i)
		{
			slotName[i] = FormatA("ID_BUFF_SLOT%d", i);
			counterName[i] = FormatA("ID_STATIC_SLOT%d", i);

			m_pBuffTextureCtrl[i] = GetControl<CEtUITextureControl>(slotName[i].c_str());
#ifdef PRE_ADD_BUFF_ADD_INFORMATION
			m_pBuffTextureCtrl[i]->SetTooltipRefresh(true);
			m_pBuffTextureCtrl[i]->SetToolTipBottomPosition(true);
			m_pBuffTextureCtrl[i]->SetTooltipWidthLimit(200.f);
#endif

			m_pBuffCounter[i] = GetControl<CEtUIStatic>(counterName[i].c_str());
			m_pIconCoord[eBuff_Icon1 + i] = m_pBuffCounter[i]->GetBaseUICoord();
		}

		m_pButtonComplaint = GetControl<CEtUIButton>("ID_BUTTON_COMPLAINT");
		m_pButtonComplaint->Show(false);
		m_pButtonComplaint->SetDisableTime(2.0f);
	}

#ifdef _USE_VOICECHAT
	if (m_FaceType == FT_PLAYER)
	{
		// 재로딩이 있을때마다 키설정값을 다시 불러온다.
		SetToggleMicHotKey(_ToVK(g_UIWrappingKeyData[IW_UI_TOGGLEMIC]));
	}
#endif

	if (m_FaceType == FT_PLAYER)
	{
		int i = 0;
		std::string slotName[MAX_BUFF_SLOT];
		std::string counterName[MAX_BUFF_SLOT];
		std::string coolTimeName[MAX_BUFF_SLOT];
		for (; i < MAX_BUFF_SLOT; ++i)
		{
			slotName[i] = FormatA("ID_BUFF_SLOT%d", i);
			counterName[i] = FormatA("ID_STATIC_SLOT%d", i);
			coolTimeName[i] = FormatA("ID_BUFF_COOLTIME%d", i);

			m_pBuffTextureCtrl[i] = GetControl<CEtUITextureControl>(slotName[i].c_str());
#ifdef PRE_ADD_BUFF_ADD_INFORMATION
			m_pBuffTextureCtrl[i]->SetTooltipRefresh(true);
			m_pBuffTextureCtrl[i]->SetToolTipBottomPosition(true);
			m_pBuffTextureCtrl[i]->SetTooltipWidthLimit(200.f);
#endif

			m_pBuffCounter[i] = GetControl<CEtUIStatic>(counterName[i].c_str());
			m_pBubbleCoolTimeTextureCtrl[i] = GetControl<CEtUITextureControl>(coolTimeName[i].c_str());

			m_pIconCoord[eBuff_Icon1 + i] = m_pBuffCounter[i]->GetBaseUICoord();
		}

		GetControl<CEtUIButton>("ID_BUTTON_PASSWORD")->Show(false);
		GetControl<CEtUIButton>("ID_BUTTON_LOCK")->Show(false);
		GetControl<CEtUIButton>("ID_BUTTON_UNLOCK")->Show(false);
	}

	m_dwDefaultColor = m_pPlayerName->GetTextColor();
}

void CDnGaugeDlg::Show(bool bShow)
{
	if (m_bShow == bShow)
		return;

	if (bShow)
	{
		if (m_FaceType == FT_ENEMY || m_FaceType == FT_MINI_ENEMY)
		{
			// 스테이지 난이도 기억.
			CDnGameTask* pGameTask = (CDnGameTask*)(CTaskManager::GetInstance().GetTask("GameTask"));
			if (pGameTask) m_nStageConstructionLevel = pGameTask->GetStageConstructionLevel();
		}

		if (m_FaceType == FT_ENEMY)
		{
			GetInterface().MoveOffsetStageInfoDlg(true);
		}
	}
	else
	{
		if (m_FaceType == FT_ENEMY)
		{
			GetInterface().MoveOffsetStageInfoDlg(false);
		}
	}

	CEtUIDialog::Show(bShow);
}


void CDnGaugeDlg::ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg)
{
	SetCmdControlName(pControl->GetControlName());

	if (nCommand == EVENT_BUTTON_CLICKED)
	{
#ifdef _USE_VOICECHAT
		if (IsCmdControl("ID_BT_NON"))
		{
		}
		else if (IsCmdControl("ID_BT_ON"))
		{
			if (m_FaceType == FT_MINI_PLAYER)
			{
				if (CDnPartyTask::IsActive()) {
					CDnPartyTask::PartyStruct *pMember = GetPartyTask().GetPartyDataFromMemberName(m_wszPlayerName.c_str());
					if (pMember) {
						GetVoiceChatTask().ReqUserMute(pMember->nSessionID, true);
						m_pButtonVoice[VoiceReady]->Show(false);
						m_pButtonVoice[VoiceMute]->Show(true);
					}
				}
			}
			else if (m_FaceType == FT_PLAYER)
			{
				GetVoiceChatTask().MuteMyMic(true);
				m_pButtonVoice[VoiceReady]->Show(false);
				m_pButtonVoice[VoiceMute]->Show(true);
			}
		}
		else if (IsCmdControl("ID_BT_SPEAK"))
		{
			if (m_FaceType == FT_MINI_PLAYER)
			{
				if (CDnPartyTask::IsActive()) {
					CDnPartyTask::PartyStruct *pMember = GetPartyTask().GetPartyDataFromMemberName(m_wszPlayerName.c_str());
					if (pMember) {
						GetVoiceChatTask().ReqUserMute(pMember->nSessionID, true);
						m_pButtonVoice[VoiceActive]->Show(false);
						m_pButtonVoice[VoiceMute]->Show(true);
					}
				}
			}
			else if (m_FaceType == FT_PLAYER)
			{
				GetVoiceChatTask().MuteMyMic(true);
				m_pButtonVoice[VoiceActive]->Show(false);
				m_pButtonVoice[VoiceMute]->Show(true);
			}
		}
		else if (IsCmdControl("ID_BT_ON_NON"))
		{
			if (m_FaceType == FT_MINI_PLAYER)
			{
				if (CDnPartyTask::IsActive()) {
					CDnPartyTask::PartyStruct *pMember = GetPartyTask().GetPartyDataFromMemberName(m_wszPlayerName.c_str());
					if (pMember) {
						GetVoiceChatTask().ReqUserMute(pMember->nSessionID, false);
						m_pButtonVoice[VoiceReady]->Show(true);
						m_pButtonVoice[VoiceMute]->Show(false);
					}
				}
			}
			else if (m_FaceType == FT_PLAYER)
			{
				GetVoiceChatTask().MuteMyMic(false);
				m_pButtonVoice[VoiceReady]->Show(true);
				m_pButtonVoice[VoiceMute]->Show(false);
			}
		}
		else if (IsCmdControl("ID_BUTTON_COMPLAINT"))
		{
			if (m_FaceType == FT_MINI_PLAYER)
			{
				if (CDnPartyTask::IsActive()) {
					CDnPartyTask::PartyStruct *pMember = GetPartyTask().GetPartyDataFromMemberName(m_wszPlayerName.c_str());
					if (pMember) {
						GetVoiceChatTask().ReqUserComplaint(pMember->nSessionID);
					}
				}
			}
		}
#endif //_USE_VOICECHAT 

		if (IsCmdControl("ID_BUTTON_QUIT0"))
		{
			if (m_FaceType == FT_PLAYER)
			{
				drag::Command(UI_DRAG_CMD_CANCEL);
				drag::ReleaseControl();

				if (GetInterface().IsShowPrivateMarketDlg()) return;
				CDnGameTask *pGameTask = (CDnGameTask *)CTaskManager::GetInstance().GetTask("GameTask");
				if (pGameTask && pGameTask->GetGameTaskType() == GameTaskType::Farm)
				{
					ToggleLifeCondition();
				}
				else
				{
					if (CDnWorld::GetInstance().GetMapType() == CDnWorld::MapTypeWorldMap)
					{
#ifdef PRE_FIX_GAMEQUIT_TO_VILLAGE
						::SendAbandonStage(true, false);
#else
						::SendAbandonStage(true);
#endif
					}
					else
					{
						GetPartyTask().ReqOutParty();
					}
					ResetKeepMasterInfo();
					// 1인팟으로 게임나가서 파티탈퇴를 여러번 누르니(페이드되면서.) 아예 마을로 와지지가 않는다.(검은 화면)
					// 그래서 한번 누르면 Disable로 바꾸도록 하겠다.
					m_pButtonPartyQuit->Enable(false);
					return;
				}
			}
		}
	}

#ifdef PRE_ADD_PVP_SHOW_PARTY_GAUGE
	if (nCommand == EVENT_CHECKBOX_CHANGED)
	{
		if (IsCmdControl("ID_CHECKBOX_COLOPARTY"))
		{
			GetInterface().ShowPartyPlayerGaugeList(m_pCheckShowPvpGauge->IsChecked());
		}
	}
#endif

	CEtUIDialog::ProcessCommand(nCommand, bTriggeredByUser, pControl, uMsg);
}


void CDnGaugeDlg::Process(float fElapsedTime)
{
	CEtUIDialog::Process(fElapsedTime);

	if (m_bDamage)
	{
		if (m_fFaceTime >= 1.8f)
		{
			m_bDamage = false;
			m_pStaticDamage->SetBlink(false);
		}
		else if (m_fFaceTime >= 1.5f)
		{
			m_pStaticDamage->Show(false);
		}

		m_fFaceTime += fElapsedTime;
	}
	else
	{
		if (m_pStaticDamage)
		{
			m_pStaticDamage->SetBlink(false);
			m_pStaticDamage->Show(false);
		}
	}

	if (m_pStaticHP) m_pStaticHP->Show(CGameOption::GetInstance().bStateBarInsideCount);
	if (m_pStaticSP) m_pStaticSP->Show(CGameOption::GetInstance().bStateBarInsideCount);

	if (IsShow() && m_FaceType == FT_PLAYER)
	{
		m_pPlayerName->Show(m_pPartyMaster->IsShow() == false);
		m_pPlayerNameInParty->Show(m_pPartyMaster->IsShow());

		RefreshRebirth();

#ifdef PRE_ADD_VIP
		RefreshVIP();
#endif
		if (CDnPartyTask::IsActive()) {
			m_pButtonPartyQuit->Enable((CDnPartyTask::GetInstance().GetEnteredGateIndex() == -1) ? true : false);
		}
	}
}

void CDnGaugeDlg::RenderBubbleRemainTime(float fElapsedTime)
{
	// 	if (m_FaceType == FT_PLAYER)
	// 	{
	// 		if (CDnActor::s_hLocalActor)
	// 		{
	// 			CDnLocalPlayerActor* pLocalActor = static_cast<CDnLocalPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
	// 			if (pLocalActor)
	// 			{
	// 				BubbleSystem::CDnBubbleSystem* pBubbleSystem = pLocalActor->GetBubbleSystem();
	// 				if (pBubbleSystem == NULL)
	// 					return;
	// 
	// 				std::vector<BubbleSystem::CDnBubbleSystem::S_BUBBLE_INFO> bubbleInfos;
	// 				pBubbleSystem->GetAllAppliedBubbles(bubbleInfos);
	// 
	// 				std::vector<BubbleSystem::CDnBubbleSystem::S_BUBBLE_INFO>::const_iterator iter = bubbleInfos.begin();
	// 				for (; iter != bubbleInfos.end(); ++iter)
	// 				{
	// 					const BubbleSystem::CDnBubbleSystem::S_BUBBLE_INFO& info = (*iter);
	// 				}
	// 			}
	// 		}
	// 	}
}

void CDnGaugeDlg::Render(float fElapsedTime)
{
	if (GetInterface().IsOpenBlind())
		return;

	if (IsShow())
	{
		RefreshIcon();

		RenderBubbleRemainTime(fElapsedTime);
	}

	CEtUIDialog::Render(fElapsedTime);
}

void CDnGaugeDlg::SetFaceID(int nID)
{
	if (!m_pFace) return;
	m_pFace->SetFaceID(nID);
}

int CDnGaugeDlg::GetFaceID()
{
	if (!m_pFace) return -1;
	return m_pFace->GetFaceID();
}

void CDnGaugeDlg::SetFaceType(GAUGE_FACE_TYPE Type)
{
	if (!m_pFace) return;
	m_pFace->SetFaceType(Type);
}

void CDnGaugeDlg::UpdatePortrait(EtObjectHandle hHandle, bool bDelete, bool bBoss, int nAniIndex, float fFrame)
{
	if (!m_pFace) return;
	m_pFace->UpdatePortrait(hHandle, bDelete, bBoss, nAniIndex, fFrame);
}

bool CDnGaugeDlg::IsUpdatePortrait()
{
	if (!m_pFace) return false;
	return m_pFace->IsUpdatePortrait();
}

void CDnGaugeDlg::BackupPortrait()
{
	if (!m_pFace) return;
	m_pFace->BackupPortrait();
}

int CDnGaugeDlg::GetSeparateCount()
{
	int nSeparateCount = 1;
	switch (m_nMonsterGrade) {
	case CDnMonsterState::Named:
		nSeparateCount = 1;
		break;
	case CDnMonsterState::Boss:
		nSeparateCount = (m_nStageConstructionLevel <= 1) ? 1 : m_nStageConstructionLevel;
		break;
	case CDnMonsterState::BossHP4:
		nSeparateCount = 4;
		break;
	case CDnMonsterState::NestBoss:
		nSeparateCount = 5;
		break;
	case CDnMonsterState::NestBoss8:
		nSeparateCount = 20;
		break;
	}

	return nSeparateCount;
}

void CDnGaugeDlg::SetHP(float fHP)
{
	if (!m_pHPBar) return;

	// 네스트보스의 경우엔 둘 이상 절대 안나온다고 하니 네스트보스이면서 MINI_ENEMY인 상황은 발생하지 않을 것이다.
	if (m_FaceType == FT_ENEMY || m_FaceType == FT_MINI_ENEMY) {
		int nSeparateCount = GetSeparateCount();

		int nLayerCount = EtMin(nSeparateCount - 1, (int)(fHP * nSeparateCount));
		if (m_nPrevLayerCount != nLayerCount || m_bRefreshStaticTextHP) {
			//			int nSeparateCount = GetSeparateCount();
			switch (nSeparateCount) {
			case 1: m_pHPBar->UseLayer(true, 1, 0); break;
			case 2: m_pHPBar->UseLayer(true, 2, 0, 1); break;
			case 3: m_pHPBar->UseLayer(true, 3, 0, 1, 2); break;
			case 4: m_pHPBar->UseLayer(true, 4, 0, 1, 2, 3); break;
			case 5: m_pHPBar->UseLayer(true, 5, 0, 1, 2, 3, 4); break;
			case 20: m_pHPBar->UseLayer(true, 20, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4); break;
			}

			if (nLayerCount == 0) {
				m_pStaticTextHP->SetText(L"");
			}
			else {
				m_pStaticTextHP->SetText(FormatW(L"X%d", nLayerCount + 1));
			}
			m_bRefreshStaticTextHP = false;
		}
		m_nPrevLayerCount = nLayerCount;
		m_pHPBar->SetProgress(100.0f * fHP);
	}
	else {
		m_pHPBar->SetProgress(100.0f * fHP);
	}
}

void CDnGaugeDlg::SetHP(INT64 nCurHP, INT64 nMaxHP, eGagueMode eType)
{
	if (!m_pStaticHP) return;
	if (nMaxHP <= 0) nMaxHP = 1;

	wchar_t szTemp[256] = L"";

	switch (eType)
	{
	case eGagueMode::Percentage:
		swprintf_s(szTemp, 256, L"%I64d/%d%%  ", (INT64)((float)nCurHP / (float)nMaxHP * 100), 100);
		break;
	case eGagueMode::Protected:
		swprintf_s(szTemp, 256, L" -- / --    ");
		nCurHP = 100;
		nMaxHP = 100;
		break;

	case eGagueMode::Normal:
	default:
		swprintf_s(szTemp, 256, L"%I64d/%I64d", nCurHP, nMaxHP);
		break;
	}

	m_pStaticHP->SetText(szTemp);
	SetHP((float)nCurHP / (float)nMaxHP);
}

void CDnGaugeDlg::SetSP(float fSP)
{
	if (!m_pSPBar) return;
	m_pSPBar->SetProgress(fSP * 100.0f);
}

void CDnGaugeDlg::SetSP(int nCurSP, int nMaxSP, eGagueMode eType)
{
	if (!m_pStaticSP) return;
	if (nMaxSP <= 0) nMaxSP = 1;

	wchar_t szTemp[256] = L"";

	//------------------------------------------------------
	//[debug]  솰柬Sp긴뺏
	if (nCurSP <= 0)
	{
		nCurSP = nMaxSP;
	}
	//------------------------------------------------------


	switch (eType)
	{
	case eGagueMode::Percentage:
		swprintf_s(szTemp, 256, L"%d/%d%%  ", (int)((float)nCurSP / (float)nMaxSP * 100), 100);
		break;
	case eGagueMode::Protected:
		swprintf_s(szTemp, 256, L" -- / --    ");
		nCurSP = 100;
		nMaxSP = 100;
		break;

	case eGagueMode::Normal:
	default:
		swprintf_s(szTemp, 256, L"%d/%d", nCurSP, nMaxSP);
		break;
	}

	m_pStaticSP->SetText(szTemp);
	SetSP((float)nCurSP / (float)nMaxSP);
}

#ifdef PRE_WORLDCOMBINE_PARTY
void CDnGaugeDlg::SetPlayerName(int nLevel, LPCWSTR pwszName, DWORD dwColor, int nServerID)
{
	if (!m_pPlayerName)
		return;
	if (m_FaceType == FT_PLAYER && m_pPlayerNameInParty == NULL)
		return;

	std::wstring temp, serverName;
	bool bWithServerName = IsPlayerNameWithServerName(nServerID);
	if (bWithServerName)
	{
		std::wstring serverNameTemp;
		CDnInterface::GetInstance().GetServerNameByIndex(serverNameTemp, nServerID);
		serverName = FormatW(L"(%s)", serverNameTemp.c_str());
	}

#ifdef PRE_MOD_CONSTANT_TEXT_CONVERT_TO_TABLE_TEXT
	if (nLevel > 0)
		temp = FormatW(L"%s %d %s", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 7888), nLevel, pwszName); // UISTRING : Lv. 
	else
		temp = FormatW(L"%s", pwszName);
#else
	if (nLevel > 0)
		temp = FormatW(L"Lv. %d %s", nLevel, pwszName);
	else
		temp = FormatW(L"%s", pwszName);

#endif

	if (bWithServerName)
		temp += serverName;

	DWORD textColor = (dwColor == -1) ? m_dwDefaultColor : dwColor;

	if (m_FaceType == FT_PLAYER)
	{
		if (m_pPlayerNameInParty->IsShow())
		{
			m_pPlayerNameInParty->SetTextColor(textColor);
			m_pPlayerNameInParty->SetText(temp.c_str());
			m_pPlayerName->ClearText();
		}
		else
		{
			m_pPlayerName->SetTextColor(textColor);
			m_pPlayerName->SetText(temp.c_str());
			m_pPlayerNameInParty->ClearText();
		}
	}
	else if (m_FaceType == FT_MINI_PLAYER)
	{
		m_pPlayerName->ClearText();
		m_pPlayerName->SetTextColor(textColor);
		m_pPlayerName->SetText(temp.c_str());

		if (bWithServerName)
			m_pPlayerName->SetPartColorText(serverName.c_str(), textcolor::RED, D3DCOLOR_ARGB(0, 255, 255, 255));
	}
	else
	{
		m_pPlayerName->SetTextColor(textColor);
		m_pPlayerName->SetText(temp.c_str());
	}

	m_wszPlayerName = pwszName;
}

bool CDnGaugeDlg::IsPlayerNameWithServerName(int nServerID) const
{
	return ((nServerID != Party::Constants::INVALID_SERVERID) && CDnPartyTask::IsActive() && CDnPartyTask::GetInstance().IsPartyType(_WORLDCOMBINE_PARTY));
}
#else // PRE_WORLDCOMBINE_PARTY
void CDnGaugeDlg::SetPlayerName(int nLevel, LPCWSTR pwszName, DWORD dwColor)
{
	if (!m_pPlayerName) return;
	if (m_FaceType == FT_PLAYER && m_pPlayerNameInParty == NULL)
		return;

	wchar_t wszTemp[80] = { 0 };

#ifdef PRE_MOD_CONSTANT_TEXT_CONVERT_TO_TABLE_TEXT
	if (nLevel > 0)
	{
		swprintf_s(wszTemp, L"%s %d %s", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 7888), nLevel, pwszName); // UISTRING : Lv. 
	}
	else
	{
		swprintf_s(wszTemp, 80, L"%s", pwszName);
	}
#else
	if (nLevel > 0)
	{
		swprintf_s(wszTemp, 80, L"Lv. %d %s", nLevel, pwszName);
	}
	else
	{
		swprintf_s(wszTemp, 80, L"%s", pwszName);
	}
#endif

	DWORD textColor = (dwColor == -1) ? m_dwDefaultColor : dwColor;

	if (m_FaceType == FT_PLAYER)
	{
		if (m_pPlayerNameInParty->IsShow())
		{
			m_pPlayerNameInParty->SetTextColor(textColor);
			m_pPlayerNameInParty->SetText(wszTemp);
			m_pPlayerName->ClearText();
		}
		else
		{
			m_pPlayerName->SetTextColor(textColor);
			m_pPlayerName->SetText(wszTemp);
			m_pPlayerNameInParty->ClearText();
		}
	}
	else
	{
		m_pPlayerName->SetTextColor(textColor);
		m_pPlayerName->SetText(wszTemp);
	}

	m_wszPlayerName = pwszName;
}
#endif // PRE_WORLDCOMBINE_PARTY

void CDnGaugeDlg::UpdateRebirthCoin()
{
	if (!m_pCoinFree)		return;
	if (!m_pCoinCash)		return;

	if (CDnItemTask::IsActive() == false)
	{
		_ASSERT(0);
		return;
	}

	int coinCount[CDnItemTask::eCOINMAX];
	int i = 0, freeCoinSum = 0;
	for (; i < CDnItemTask::eCOINMAX; ++i)
	{
		coinCount[i] = GetItemTask().GetRebirthCoin((CDnItemTask::eRebirthCoinType)i);
		if (coinCount[i] == ILLEGAL_REBIRTH_COIN_COUNT)
		{
			_ASSERT(0);
			continue;
		}

		if (i < CDnItemTask::eFREECOIN_MAX)
			freeCoinSum += coinCount[i];
	}

	bool bVIPMode = false;
#ifdef PRE_ADD_VIP
	const CDnVIPDataMgr* pMgr = GetInterface().GetLocalPlayerVIPDataMgr();
	if (pMgr && pMgr->IsVIPMode() == true)
		bVIPMode = true;
#endif	// #ifdef PRE_ADD_VIP

#if defined _CH || defined _TEST_CODE_KAL
	m_pCoinFree->SetIntToText(freeCoinSum);
	std::wstring str;
	str = FormatW(L"%d", freeCoinSum);

	str += FormatW(L"(%s", GetItemTask().GetRebirthCoinString(CDnItemTask::eDAILYCOIN));
	for (i = 1; i < CDnItemTask::eFREECOIN_MAX; ++i)
	{
#ifdef PRE_ADD_VIP
		if ((bVIPMode == false && i == CDnItemTask::eVIPCOIN) || (i == CDnItemTask::eVIPCOIN && pMgr->IsVIP() == false))
			continue;
#endif	// #ifdef PRE_ADD_VIP

		if (i == CDnItemTask::ePCBANGCOIN && GetItemTask().GetRebirthCoin(CDnItemTask::ePCBANGCOIN) <= 0)
			continue;

		const WCHAR* pCoinString = GetItemTask().GetRebirthCoinString((CDnItemTask::eRebirthCoinType)i);
		if (pCoinString && pCoinString[0] != '\0')
			str += FormatW(L" + %s", pCoinString);
	}
	str += L")";
#ifndef _US
	m_pCoinFree->SetTooltipText(str.c_str());
#endif
	m_pCoinCash->SetIntToText(coinCount[CDnItemTask::eCASHCOIN]);
#else
	m_pCoinFree->SetIntToText(freeCoinSum);
	std::wstring str;
	str = FormatW(L"%d", coinCount[CDnItemTask::eDAILYCOIN]);
	for (i = 1; i < CDnItemTask::eFREECOIN_MAX; ++i)
	{
		if (bVIPMode == false && i == CDnItemTask::eVIPCOIN)
			continue;
#ifdef _KRAZ
		if (i == CDnItemTask::ePCBANGCOIN)
			continue;
#endif

		std::wstring temp;
		const WCHAR* pCoinString = GetItemTask().GetRebirthCoinString((CDnItemTask::eRebirthCoinType)i);
		if (pCoinString == NULL || pCoinString[0] == '\0')
			temp = FormatW(L"%d", coinCount[i]);
		else
			temp = FormatW(L"%d(%s)", coinCount[i], pCoinString);
		str += FormatW(L" + %s", temp.c_str());
	}
#ifndef _US
	m_pCoinFree->SetTooltipText(str.c_str());
#endif
	m_pCoinCash->SetIntToText(coinCount[CDnItemTask::eCASHCOIN]);
#endif // _CH
}

void CDnGaugeDlg::ProcessBuffs(DnActorHandle hActor)
{
	if (m_pBuffUIMng == NULL)
	{
		_ASSERT(0);
		return;
	}

	int iNumAppliedStateBlow = hActor->GetNumAppliedStateBlow();
	if (hActor->IsDie() || iNumAppliedStateBlow <= 0)
	{
		m_pBuffUIMng->ClearLists();

		int i = 0;
		for (; i < MAX_BUFF_SLOT; ++i)
		{
			if (m_pBuffTextureCtrl[i])
				m_pBuffTextureCtrl[i]->Show(false);
			if (m_pBuffCounter[i])
				m_pBuffCounter[i]->Show(false);
		}
	}

	m_pBuffUIMng->Process(hActor);

	int j = 0;
	for (; j < MAX_BUFF_SLOT; ++j)
	{
		if (m_pBuffTextureCtrl[j])
			m_pBuffTextureCtrl[j]->Show(false);
		if (m_pBuffCounter[j])
			m_pBuffCounter[j]->Show(false);
		if (m_pBubbleCoolTimeTextureCtrl[j])
			m_pBubbleCoolTimeTextureCtrl[j]->Show(false);
	}

	//	todo : Refactoring by kalliste
	int slotIdx = 0;

	ProcessBubbleList(m_pBuffUIMng->GetBubbleSlotList(), slotIdx);      // 버블
	ProcessBuffList(m_pBuffUIMng->GetBuffSlotList(), slotIdx);          // 일반 버프
	ProcessBuffList(m_pBuffUIMng->GetLowPriorityList(), slotIdx, true);// 추가관리되는 우선순위 낮은 버프 < 네임드아이템 >
}

void CDnGaugeDlg::ProcessBuffList(CDnBuffUIMng::BUFFLIST& buffSlotList, int &slotIdx, bool bUseTooltip)
{
	CDnBuffUIMng::BUFFLIST::iterator iter = buffSlotList.begin();
	CDnBuffUIMng::BUFFLIST::iterator nextIter;
	for (; iter != buffSlotList.end();)
	{
		CDnBuffUIMng::SBuffUnit& unit = *iter;
		nextIter = iter;
		++nextIter;


		if (unit.bUpdated)
		{
			if (unit.texturePageIdx >= BUFF_TEXTURE_COUNT)
			{
				_ASSERT(0);
				return;
			}

			if (m_pBuffTextureCtrl[slotIdx])
			{
				m_pBuffTextureCtrl[slotIdx]->SetTexture(m_hSkillIconList[unit.texturePageIdx], int(unit.textureUV.fX),
					int(unit.textureUV.fY), int(unit.textureUV.fWidth), int(unit.textureUV.fHeight));
				m_pBuffTextureCtrl[slotIdx]->Show(true);
#ifdef PRE_ADD_BUFF_ADD_INFORMATION
				if (bUseTooltip && m_pBuffTextureCtrl[slotIdx]->IsMouseEnter())
					ApplyBuffToolTip(unit.skillID, unit.skillLevelID, unit.duration, m_pBuffTextureCtrl[slotIdx]);
				else if (!m_pBuffTextureCtrl[slotIdx]->GetTooltipText().empty())
					m_pBuffTextureCtrl[slotIdx]->ClearTooltipText();
#endif
			}

			std::wstring counterStr;
			counterStr = FormatW(_T("%d"), int(unit.duration));

#if defined(PRE_FIX_48494)
			//자신의 Aura인 경우만..
			if (unit.bOwner == true && unit.buffType == CDnSkill::DurationTypeEnum::Aura)
				counterStr = L"∞";
#endif // PRE_FIX_48494

			if (m_pBuffCounter[slotIdx])
			{
				m_pBuffCounter[slotIdx]->SetText(counterStr.c_str());
				m_pBuffCounter[slotIdx]->Show(true);
			}

			unit.bUpdated = false;
			++slotIdx;
		}
		else
		{
			buffSlotList.erase(iter);

			if (m_pBuffTextureCtrl[slotIdx])
				m_pBuffTextureCtrl[slotIdx]->Show(false);
			if (m_pBuffCounter[slotIdx])
				m_pBuffCounter[slotIdx]->Show(false);
			if (m_pBubbleCoolTimeTextureCtrl[slotIdx])
				m_pBubbleCoolTimeTextureCtrl[slotIdx]->Show(false);
		}

		iter = nextIter;
	}
}

void CDnGaugeDlg::ProcessBubbleList(CDnBuffUIMng::BUBBLELIST& bubbleSlotList, int &slotIdx)
{
	CDnBuffUIMng::BUBBLELIST::iterator bbIter = bubbleSlotList.begin();
	CDnBuffUIMng::BUBBLELIST::iterator bbNextIter;
	for (; bbIter != bubbleSlotList.end();)
	{
		CDnBuffUIMng::SBubbleUnit& unit = *bbIter;
		bbNextIter = bbIter;
		++bbNextIter;

		if (unit.bUpdated)
		{
			if (unit.texturePageIdx >= BUFF_TEXTURE_COUNT)
			{
				_ASSERT(0);
				return;
			}

			// set bubble texture
			if (m_pBuffTextureCtrl[slotIdx])
			{
				m_pBuffTextureCtrl[slotIdx]->SetTexture(m_hSkillIconList[unit.texturePageIdx], int(unit.textureUV.fX),
					int(unit.textureUV.fY), int(unit.textureUV.fWidth), int(unit.textureUV.fHeight));
				m_pBuffTextureCtrl[slotIdx]->Show(true);
			}

			// process bubble cooltime
			SUICoord uvCoord, uiCoord, uiBaseCoord;
			CEtUITextureControl* pCoolTimeCtrl = m_pBubbleCoolTimeTextureCtrl[slotIdx];

			if (pCoolTimeCtrl)
			{
				const float& maxDuration = unit.duration;

				m_pBuffUIMng->GetTextureUV(uvCoord, BUBBLE_COOLTIME_TEXTURE_IDX);
				float uvHeight = (uvCoord.fHeight) * (1.f - (unit.remainTime / maxDuration));
				uvCoord.fY = uvCoord.fY + (uvCoord.fHeight - uvHeight);
				CommonUtil::ClipNumber(uvHeight, 0.f, uvCoord.fHeight);
				uvCoord.fHeight = uvHeight;

				pCoolTimeCtrl->GetUICoord(uiCoord);
				uiBaseCoord = pCoolTimeCtrl->GetBaseUICoord();
				float uiHeight = (uiBaseCoord.fHeight) * (1.f - (unit.remainTime / maxDuration));
				uiCoord.fY = uiBaseCoord.fY + (uiBaseCoord.fHeight - uiHeight);
				CommonUtil::ClipNumber(uiHeight, 0.f, uiBaseCoord.fHeight);
				uiCoord.fHeight = uiHeight;

				//OutputDebug("[BUBBLE] Texture Control UI - fx:%f fY:%f fWidth:%f fHeight:%f\n", uvCoord.fX, uiCoord.fY, uvCoord.fWidth, uiCoord.fHeight);

				pCoolTimeCtrl->SetTexture(m_hSkillIconList[unit.texturePageIdx], int(uvCoord.fX), int(uvCoord.fY), int(uvCoord.fWidth), int(uvCoord.fHeight));
				pCoolTimeCtrl->SetTextureColor(0x80800000);
				pCoolTimeCtrl->SetUICoord(uiCoord);
				pCoolTimeCtrl->Show(true);
			}

			//	process bubble counter
			std::wstring counterStr;
			counterStr = FormatW(_T("%d"), int(unit.bubbleCount));
			if (m_pBuffCounter[slotIdx])
			{
				m_pBuffCounter[slotIdx]->SetText(counterStr.c_str());
				m_pBuffCounter[slotIdx]->Show(true);
			}

			unit.bUpdated = false;
			++slotIdx;
		}
		else
		{
			bubbleSlotList.erase(bbIter);

			if (m_pBuffTextureCtrl[slotIdx])
				m_pBuffTextureCtrl[slotIdx]->Show(false);
			if (m_pBuffCounter[slotIdx])
				m_pBuffCounter[slotIdx]->Show(false);
			if (m_pBubbleCoolTimeTextureCtrl[slotIdx])
				m_pBubbleCoolTimeTextureCtrl[slotIdx]->Show(false);
		}

		bbIter = bbNextIter;
	}

}


#ifdef PRE_ADD_NAMEDITEM_SYSTEM
void CDnGaugeDlg::ProcessVillagePartyBuff()
{
	if (!m_pBuffUIMng)
		return;

	for (int j = 0; j < MAX_BUFF_SLOT; j++)
	{
		if (m_pBuffTextureCtrl[j])
			m_pBuffTextureCtrl[j]->Show(false);
		if (m_pBuffCounter[j])
			m_pBuffCounter[j]->Show(false);
		if (m_pBubbleCoolTimeTextureCtrl[j])
			m_pBubbleCoolTimeTextureCtrl[j]->Show(false);
	}

	if (CDnActor::s_hLocalActor && CDnActor::s_hLocalActor->GetUniqueID() == m_nSessionID) // 공격대일경우 자신도 포함됩니다.
	{
		CDnLocalPlayerActor *pLocalActor = static_cast<CDnLocalPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
		int iNumUsedSourceItem = pLocalActor->GetNumEffectSkill();
		for (int EffectIndex = 0; EffectIndex < iNumUsedSourceItem; EffectIndex++)
		{
			const CDnLocalPlayerActor::S_EFFECT_SKILL* pUsedSourceItem = pLocalActor->GetEffectSkillFromIndex(EffectIndex);
			if (pUsedSourceItem)
			{
				AddVillagePartyBuffTexture(
					EffectIndex,
					pUsedSourceItem->iItemID,
					pUsedSourceItem->iSkillID,
					float(pUsedSourceItem->nEffectSkillLeftTime / 1000),
					pUsedSourceItem->bEternity);
			}
		}
	}
	else
	{
		CDnPartyTask *pPartyTask = (CDnPartyTask*)CTaskManager::GetInstance().GetTask("PartyTask");
		if (pPartyTask)
		{
			const CDnPartyTask::PartyStruct *pPartyStruct = pPartyTask->GetPartyDataFromSessionID(m_nSessionID);
			if (pPartyStruct)
			{
				for (DWORD EffectIndex = 0; EffectIndex< pPartyStruct->vecEffestSkillData.size(); EffectIndex++)
				{
					AddVillagePartyBuffTexture(
						EffectIndex,
						pPartyStruct->vecEffestSkillData[EffectIndex].nItemID,
						pPartyStruct->vecEffestSkillData[EffectIndex].nSkillID,
						pPartyStruct->vecEffestSkillData[EffectIndex].fRemainTime,
						pPartyStruct->vecEffestSkillData[EffectIndex].bEternity
					);
				}
			}
		}
	}
}

void CDnGaugeDlg::AddVillagePartyBuffTexture(int nControIndex, int nItemID, int nSkillID, float fRemainTime, bool bEternity)
{
	DNTableFileFormat* pItemSox = GetDNTable(CDnTableDB::TITEM);
	DNTableFileFormat* pSkillSox = GetDNTable(CDnTableDB::TSKILL);
	DNTableFileFormat* pSkillLevelSox = GetDNTable(CDnTableDB::TSKILLLEVEL);

	if ((pItemSox && pItemSox->IsExistItem(nItemID)))
	{
		int nSkillTableID = pItemSox->GetFieldFromLablePtr(nItemID, "_SkillID")->GetInteger();
		eItemTypeEnum eType = (eItemTypeEnum)pItemSox->GetFieldFromLablePtr(nItemID, "_Type")->GetInteger();

		if ((CDnItem::IsEffectSkillItem(eType) == false))
			return;

		if (pSkillSox && pSkillSox->IsExistItem(nSkillTableID) && pSkillLevelSox)
		{
			int nSkillLevel = pItemSox->GetFieldFromLablePtr(nItemID, "_SkillLevel")->GetInteger();
			int nBuffIconIndex = pSkillSox->GetFieldFromLablePtr(nSkillTableID, "_BuffIconImageIndex")->GetInteger();
			int iSkillLevelTableID = -1;

			vector<int> vlSkillLevelList;
			GetSkillTask().GetSkillLevelList(nSkillTableID, vlSkillLevelList, CDnSkill::PVE);
			if (false == vlSkillLevelList.empty())
			{
				for (int i = 0; i < (int)vlSkillLevelList.size(); ++i)
				{
					int iNowLevel = pSkillLevelSox->GetFieldFromLablePtr(vlSkillLevelList.at(i), "_SkillLevel")->GetInteger();
					if (iNowLevel == nSkillLevel)
					{
						iSkillLevelTableID = vlSkillLevelList.at(i);
						break;
					}
				}
			}

			if (m_pBuffTextureCtrl[nControIndex])
			{
				int nBuffIconIndex = pSkillSox->GetFieldFromLablePtr(nSkillID, "_BuffIconImageIndex")->GetInteger();
				int nIconTextureindex = nBuffIconIndex / BUFF_TEXTURE_ICON_COUNT;

				if (nIconTextureindex >= BUFF_TEXTURE_COUNT)
					return;

				SUICoord TextureUV;
				m_pBuffUIMng->GetTextureUV(TextureUV, nBuffIconIndex);

				m_pBuffTextureCtrl[nControIndex]->SetTexture(m_hSkillIconList[nIconTextureindex], int(TextureUV.fX),
					int(TextureUV.fY), int(TextureUV.fWidth), int(TextureUV.fHeight));
				m_pBuffTextureCtrl[nControIndex]->Show(true);
#ifdef PRE_ADD_BUFF_ADD_INFORMATION
				if (m_pBuffTextureCtrl[nControIndex]->IsMouseEnter())
					ApplyBuffToolTip(nSkillID, iSkillLevelTableID, fRemainTime, m_pBuffTextureCtrl[nControIndex]);
				else if (!m_pBuffTextureCtrl[nControIndex]->GetTooltipText().empty())
					m_pBuffTextureCtrl[nControIndex]->ClearTooltipText();
#endif
			}

			if (m_pBuffCounter[nControIndex])
			{
				std::wstring counterStr;
				counterStr = FormatW(L"%d", int(fRemainTime));

				if (bEternity == true || fRemainTime > 6000.f || fRemainTime < 0)
					counterStr = L"∞";

				m_pBuffCounter[nControIndex]->SetText(counterStr);
				m_pBuffCounter[nControIndex]->Show(true);
			}
		}
	}
}

#endif


// 버블에 대한 디스크립션 정보 아직없음 테이블 추가후 찍어야함 , 현재는 네임드 아이템만 사용할 계획.
void CDnGaugeDlg::ApplyBuffToolTip(int nSkilID, int nSkillLevelID, float fDuration, CEtUITextureControl* pBuffTextureCtrl)
{
#ifdef PRE_ADD_BUFF_ADD_INFORMATION
	if (nSkilID > -1 && nSkillLevelID > -1)
	{
		DNTableFileFormat*  pSkillTable = GetTableDB().GetTable(CDnTableDB::TSKILL);
		DNTableFileFormat*  pSkillLevelTable = GetTableDB().GetTable(CDnTableDB::TSKILLLEVEL);

		if (pSkillTable && pSkillLevelTable)
		{
			std::wstring wszDescription;

			int nSkillExplainStringID = pSkillLevelTable->GetFieldFromLablePtr(nSkillLevelID, "_SkillExplanationID")->GetInteger();
			std::wstring msg;
			char *szParam = pSkillLevelTable->GetFieldFromLablePtr(nSkillLevelID, "_SkillExplanationIDParam")->GetString();
			MakeUIStringUseVariableParam(msg, nSkillExplainStringID, szParam);

			wszDescription += FormatW(L"#j%s#d", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, pSkillTable->GetFieldFromLablePtr(nSkilID, "_NameID")->GetInteger()));
			wszDescription += L"\n";
			wszDescription += msg;

			if (fDuration > 0)
			{
				std::wstring wszDuration;
				CDnItem::MakeItemDurationString((int)(fDuration * 1000), wszDuration);
				wszDescription += FormatW(L"\n#s%s:%s#d", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 5158), wszDuration.c_str());
			}

			pBuffTextureCtrl->SetTooltipText(wszDescription.c_str());
		}
	}
#endif
}

void CDnGaugeDlg::RefreshDurabilityIcon()
{
	m_pStaticItemRepair0->Show(false);
	m_pStaticItemRepair1->Show(false);
	m_pStaticItemRepair2->Show(false);

	CDnItemTask::EquipDurabilityStateEnum eResult = GetItemTask().GetEquipDurabilityState();
	switch (eResult)
	{
	case CDnItemTask::Durability_None:
		return;
	case CDnItemTask::Durability_Yellow:
		m_pStaticItemRepair0->Show(true);
		break;
	case CDnItemTask::Durability_Orange:
		m_pStaticItemRepair1->Show(true);
		break;
	case CDnItemTask::Durability_Red:
		m_pStaticItemRepair2->Show(true);
		break;
	}
}

void CDnGaugeDlg::SetEmpty()
{
	if (!m_pFace) return;
	m_pFace->SetFaceID(-1);
	m_wszPlayerName = L"";
	m_nSessionID = 0;

	int j = 0;
	for (; j < MAX_BUFF_SLOT; ++j)
	{
		if (m_pBuffTextureCtrl[j])
			m_pBuffTextureCtrl[j]->Show(false);
		if (m_pBuffCounter[j])
			m_pBuffCounter[j]->Show(false);
	}
}

bool CDnGaugeDlg::Empty()
{
	if (m_pFace->GetFaceID() == -1)
		return true;
	else
		return false;
}

void CDnGaugeDlg::OnDamage()
{
	ASSERT(m_pFace&&"CDnGaugeDlg::OnDamage");

	m_bDamage = true;
	m_fFaceTime = 0.0f;

	if (m_pStaticDamage)
	{
		m_pStaticDamage->Show(true);
		m_pStaticDamage->SetBlink(true);
	}

	//m_pFace->OnDamage();
}

void CDnGaugeDlg::ShowGateReady(bool bShow)
{
	m_pStaticGateReady->Show(bShow);
}

void CDnGaugeDlg::ShowLoading(bool bShow)
{
	m_pStaticLoading->Show(bShow);
}

bool CDnGaugeDlg::IsShowGateReady()
{
	if (m_pStaticGateReady)
		return m_pStaticGateReady->IsShow();
	return false;
}

void CDnGaugeDlg::SetImmediateChange(bool bHP)
{
	if (bHP) {
		if (m_pHPBar) m_pHPBar->SetImmediateChange();
	}
	else {
		if (m_pSPBar) m_pSPBar->SetImmediateChange();
	}
}

void CDnGaugeDlg::SetProgressIndex(int nType, int nIndex, bool bUse)
{
	switch (nType)
	{
	case eProgressBarType::HPBar:
	{
		if (m_pHPBar)
		{
			m_pHPBar->SetProgressIndex(nIndex);
			m_pHPBar->UseProgressIndex(bUse);
		}
	}
	break;
	case eProgressBarType::SPBar:
	{
		if (m_pSPBar)
		{
			m_pSPBar->SetProgressIndex(nIndex);
			m_pSPBar->UseProgressIndex(bUse);
		}
	}
	break;
	}
}

EtTextureHandle CDnGaugeDlg::GetPortraitTexture()
{
	if (!m_pFace) return CEtTexture::Identity();
	return m_pFace->GetPortraitTexture();
}

#ifdef PRE_ADD_IMPROVE_MASTER_OBSERVER
void CDnGaugeDlg::SetPartySelection(bool bSelect)
{
	m_pPartySelection->Show(bSelect);
}
#endif

void CDnGaugeDlg::SetPartyMaster(bool bMaster)
{
	m_pPartyMaster->Show(bMaster);
}

#ifdef _USE_VOICECHAT
void CDnGaugeDlg::UpdateVoiceButtonMode(bool bVoiceAvailable, bool bMute)
{
	if (m_FaceType == FT_PLAYER || m_FaceType == FT_MINI_PLAYER)
	{
		for (int i = 0; i < NUM_VOICE_STATE; ++i)
			m_pButtonVoice[i]->Show(false);
	}

	if (m_FaceType == FT_MINI_PLAYER && m_pButtonComplaint)
		m_pButtonComplaint->Show(false);

	int nCurState = -1;
	if (m_FaceType == FT_PLAYER)
	{
		if (CDnPartyTask::IsActive() && GetPartyTask().GetPartyCount() > 1)	// 2인 이상일때만 처리
		{
			if (CGameOption::GetInstance().m_bVoiceChat)	// 클라이언트단 다시 한번 검사
			{
				if (CDnVoiceChatTask::IsActive() && GetVoiceChatTask().IsInitialized() && bVoiceAvailable)
				{
					nCurState = VoiceNone;

					if (CVoiceChatClient::IsActive() && CVoiceChatClient::GetInstance().IsExistMic())
						nCurState = VoiceReady;
				}
			}
		}
	}
	else if (m_FaceType == FT_MINI_PLAYER)
	{
		if (CDnVoiceChatTask::IsActive() && GetVoiceChatTask().IsInitialized())
		{
			if (CGameOption::GetInstance().m_bVoiceChat)
			{
				if (!bVoiceAvailable)
				{
					nCurState = VoiceNone;
				}
				else
				{
					nCurState = bMute ? VoiceMute : VoiceReady;
					//m_pButtonComplaint->Show( true );
				}
			}
		}
	}

	if (nCurState != -1)
		m_pButtonVoice[nCurState]->Show(true);
}

void CDnGaugeDlg::UpdateVoiceSpeaking(bool bSpeak)
{
	if (m_FaceType != FT_PLAYER && m_FaceType != FT_MINI_PLAYER)
		return;

	if (m_pButtonVoice[VoiceReady]->IsShow() || m_pButtonVoice[VoiceActive]->IsShow())
	{
		m_pButtonVoice[VoiceReady]->Show(!bSpeak);
		m_pButtonVoice[VoiceActive]->Show(bSpeak);
	}
}

void CDnGaugeDlg::HideVoiceButtonMode()
{
	if (m_FaceType == FT_PLAYER || m_FaceType == FT_MINI_PLAYER)
	{
		for (int i = 0; i < NUM_VOICE_STATE; ++i)
			m_pButtonVoice[i]->Show(false);
	}
}

void CDnGaugeDlg::SetToggleMicHotKey(BYTE cVK)
{
	// 핫키를 임의로 바꿀 수 있는 기능이 없어서, 우선은 이런 식으로 처리한다.
	m_pButtonVoice[VoiceReady]->SetHotKey(cVK);
	m_pButtonVoice[VoiceActive]->SetHotKey(cVK);
	m_pButtonVoice[VoiceMute]->SetHotKey(cVK);
}
#endif

void CDnGaugeDlg::SetMonsterGrade(int nMonsterGrade)
{
	if (m_nMonsterGrade == nMonsterGrade) return;

	if (m_FaceType == FT_ENEMY || m_FaceType == FT_MINI_ENEMY)
		m_nMonsterGrade = nMonsterGrade;

	if (m_FaceType == FT_ENEMY) {
		CEtUIProgressBar *pOtherHPBar = NULL;
		CEtUIProgressBar *pOtherSPBar = NULL;
		CEtUIStatic *pOtherTextHP = NULL;
		bool bNestBoss = (nMonsterGrade >= CDnMonsterState::NestBoss);

		CONTROL(Static, ID_STATIC_BOSS)->Show(!bNestBoss);
		CONTROL(Static, ID_STATIC_NESTBOSS)->Show(bNestBoss);

		if (!bNestBoss) {
			m_pStaticTextHP = CONTROL(Static, ID_TEXT_HP);
			pOtherTextHP = CONTROL(Static, ID_TEXT_NESTHP);
			m_pHPBar = CONTROL(ProgressBar, ID_HP_GAUGE);
			pOtherHPBar = CONTROL(ProgressBar, ID_HP_NESTGAUGE);
			m_pSPBar = CONTROL(ProgressBar, ID_SUPER_GAUGE);
			pOtherSPBar = CONTROL(ProgressBar, ID_SUPER_NESTGAUGE);
		}
		else {
			m_pStaticTextHP = CONTROL(Static, ID_TEXT_NESTHP);
			pOtherTextHP = CONTROL(Static, ID_TEXT_HP);
			m_pHPBar = CONTROL(ProgressBar, ID_HP_NESTGAUGE);
			pOtherHPBar = CONTROL(ProgressBar, ID_HP_GAUGE);
			m_pSPBar = CONTROL(ProgressBar, ID_SUPER_NESTGAUGE);
			pOtherSPBar = CONTROL(ProgressBar, ID_SUPER_GAUGE);
		}
		pOtherTextHP->SetText(L"");
		m_pHPBar->Show(true);
		pOtherHPBar->Show(false);
		m_pSPBar->Show(true);
		pOtherSPBar->Show(false);
	}

	// 등급이 바뀌었다는건 네스트보스였다가 보스 혹은 보스였다가 네스트보스용으로 바뀐것을 의미한다.
	// 이 경우엔 SetHP함수에서 꼭 피표시 스태틱(X5같은)을 갱신해야한다.
	m_bRefreshStaticTextHP = true;
}

void CDnGaugeDlg::ShowPartyLootInfo(bool bShow, const wchar_t* pInfoString)
{
	if (pInfoString && pInfoString[0] != '\0')
		m_pStaticPartyLootInfo->SetText(pInfoString);
	m_pStaticPartyLootInfo->Show(bShow);
}

void CDnGaugeDlg::ShowPartyQuitButton(bool bShow)
{
	m_pButtonPartyQuit->Show(bShow);
	if (bShow) m_pButtonPartyQuit->Enable(true);
}

void CDnGaugeDlg::ShowPartyItemLootRank(bool bShow, const wchar_t* pInfoString)
{
	if (pInfoString && pInfoString[0] != '\0')
		m_pStaticPartyItemLootRank->SetText(pInfoString);
	m_pStaticPartyItemLootRank->Show(bShow);
}

void CDnGaugeDlg::RefreshRebirth()
{
	if (CDnItemTask::IsActive()) {
#ifdef PRE_MOD_NESTREBIRTH
		int nUsableCoin = CDnItemTask::GetInstance().GetUsableRebirthCoin();

		bool bIsShowNestRebirthGauge = false;
		bool bIsShowRebirthLimitGauge = false;
		if (CDnWorld::IsActive() && CDnWorld::GetInstance().GetMapType() == CDnWorld::MapTypeDungeon)
		{
			if (CDnWorld::GetInstance().GetDragonNestType() != eDragonNestType::None)
			{
				if (CommonUtil::IsInfiniteRebirthOnDungeon() == false)
					bIsShowNestRebirthGauge = true;
			}
			else if (CommonUtil::IsInfiniteRebirthOnDungeon() == false)
			{
				bIsShowRebirthLimitGauge = true;
			}
		}

		if (bIsShowNestRebirthGauge)
		{
			if (m_pRebirthLimit)
				m_pRebirthLimit->Show(false);
			m_pRebirthLimitBack->Show(true);
			m_pRebirthLimitCoinIcon->Show(true);
			m_pRebirthLimitItemIcon->Show(true);

			int nLocalPlayerUsableRebirthCoin = CDnItemTask::GetInstance().GetRebirthCoinCount();
			if (nLocalPlayerUsableRebirthCoin > nUsableCoin)
				nLocalPlayerUsableRebirthCoin = nUsableCoin;

			const std::wstring& limitText = m_pRebirthLimitCoinCount->GetText();
			if (m_nPrevUsableCoinCount != nUsableCoin || limitText.empty())
				m_pRebirthLimitCoinCount->SetIntToText(nUsableCoin);
			m_nPrevUsableCoinCount = nUsableCoin;

			m_pRebirthLimitCoinCount->Show(true);
			m_pRebirthLimitItemCount->Show(true);

			if (CDnItemTask::GetInstance().IsOnChangeRebirthCoinOrItem())
			{
				int nRebirthItemCount = CDnItemTask::GetInstance().GetRebirthItemCount_MineOnly();
				int nRebirthItemCountLimit = CDnItemTask::GetInstance().GetSpecialRebirthItemCount();
				if (nRebirthItemCount > nRebirthItemCountLimit)
					nRebirthItemCount = nRebirthItemCountLimit;
				m_pRebirthLimitItemCount->SetIntToText(nRebirthItemCount);

				CDnItemTask::GetInstance().SetUpdateRebirthCoinOrItem(false);

				if (m_pNestRebirthTooltipDlg)
					m_pNestRebirthTooltipDlg->SetNestRebirthTooltip(nLocalPlayerUsableRebirthCoin, nUsableCoin, nRebirthItemCount);
			}
		}
		else if (bIsShowRebirthLimitGauge)
		{
			m_pRebirthLimitCoinCount->Show(false);
#ifdef PRE_FIX_REBIRTHCOIN_REMAIN
			const std::wstring& limitText = m_pRebirthLimitCoinCount->GetText();
			if (limitText.empty() == false)
				m_pRebirthLimitCoinCount->ClearText();
#endif
			m_pRebirthLimitItemCount->Show(false);
			m_pRebirthLimitCoinIcon->Show(false);
			m_pRebirthLimitItemIcon->Show(false);

			m_pRebirthLimitBack->Show(true);
			if (m_pRebirthLimit)
				m_pRebirthLimit->Show(true);
			if (m_nPrevUsableCoinCount != nUsableCoin)
			{
				WCHAR wszStr[64] = { 0, };
				if (nUsableCoin > 0)
					swprintf_s(wszStr, GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 110), nUsableCoin);
				else
					swprintf_s(wszStr, GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 111));
				if (m_pRebirthLimit)
					m_pRebirthLimit->SetText(wszStr);
			}

			m_nPrevUsableCoinCount = nUsableCoin;
		}
		else
		{
			m_pRebirthLimitBack->Show(false);
			m_pRebirthLimitCoinCount->Show(false);
#ifdef PRE_FIX_REBIRTHCOIN_REMAIN
			const std::wstring& limitText = m_pRebirthLimitCoinCount->GetText();
			if (limitText.empty() == false)
				m_pRebirthLimitCoinCount->ClearText();
#endif
			m_pRebirthLimitItemCount->Show(false);
			m_pRebirthLimitCoinIcon->Show(false);
			m_pRebirthLimitItemIcon->Show(false);
			if (m_pRebirthLimit)
				m_pRebirthLimit->Show(false);

			m_nPrevUsableCoinCount = -1;
		}
#else // PRE_MOD_NESTREBIRTH
		int nMaxRebirthCoin = CDnItemTask::GetInstance().GetMaxUsableRebirthCoin();
		int nUsableCoin = CDnItemTask::GetInstance().GetUsableRebirthCoin();

		if (CDnWorld::IsActive() && CDnWorld::GetInstance().GetMapType() != CDnWorld::MapTypeDungeon) nMaxRebirthCoin = -1;

		if (nMaxRebirthCoin > 0) {
			m_pRebirthLimitBack->Show(true);
			m_pRebirthLimit->Show(true);
			if (m_nPrevUsableCoinCount != nUsableCoin) {
				WCHAR wszStr[64] = { 0, };
				if (nUsableCoin > 0)
					swprintf_s(wszStr, GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 110), nUsableCoin);
				else swprintf_s(wszStr, GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 111));
				m_pRebirthLimit->SetText(wszStr);
			}
			m_nPrevUsableCoinCount = nUsableCoin;
		}
		else {
			m_pRebirthLimitBack->Show(false);
			m_pRebirthLimit->Show(false);
			m_nPrevUsableCoinCount = -1;
		}
#endif // PRE_MOD_NESTREBIRTH
	}
}

#ifdef PRE_ADD_VIP
void CDnGaugeDlg::RefreshVIP()
{
	const CDnVIPDataMgr* pMgr = GetInterface().GetLocalPlayerVIPDataMgr();
	if (pMgr == NULL)
		return;

	const CDnVIPDataMgr& mgr = *pMgr;
	if (mgr.IsVIP())
	{
		bool bGreenSignal = false;
		if (mgr.IsVIPAutoPay())
		{
			bGreenSignal = true;
		}
		else
		{
			if (mgr.IsVIPNearExpireDate() == false)
				bGreenSignal = true;
		}

		m_pStaticVIPExpire->Show(bGreenSignal == false);
		m_pStaticVIPNormal->Show(bGreenSignal);
	}
	else
	{
		m_pStaticVIPExpire->Show(false);
		m_pStaticVIPNormal->Show(false);
	}
}

void CDnGaugeDlg::ShowPopupVIPDlg(bool bShow, float fX, float fY)
{
	if (bShow)
	{
		m_pPopupVIPDlg->Show(true);

		SUICoord dlgCoord;
		m_pPopupVIPDlg->GetDlgCoord(dlgCoord);
		dlgCoord.fX = fX;
		dlgCoord.fY = fY;
		m_pPopupVIPDlg->SetDlgCoord(dlgCoord);
	}
	else
	{
		m_pPopupVIPDlg->Show(false);
	}
}
#endif // PRE_ADD_VIP

bool CDnGaugeDlg::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (!IsShow())
	{
		return false;
	}

	switch (uMsg)
	{
	case WM_MOUSEMOVE:
	{
		float fMouseX, fMouseY;
		POINT MousePoint;
		MousePoint.x = short(LOWORD(lParam));
		MousePoint.y = short(HIWORD(lParam));
		PointToFloat(MousePoint, fMouseX, fMouseY);

#ifdef PRE_ADD_VIP
		if (m_pStaticVIPNormal && m_pPopupVIPDlg &&
			(m_pStaticVIPNormal->IsShow() || m_pStaticVIPExpire->IsShow()))
		{
			bool bShowPopup = false;
			SUICoord uiCoord;
			m_pStaticVIPNormal->GetUICoord(uiCoord);
			m_pPopupVIPDlg->SetInfo(uiCoord);

			if (uiCoord.IsInside(fMouseX, fMouseY))
				bShowPopup = true;
			ShowPopupVIPDlg(bShowPopup, fMouseX, fMouseY);
		}
#endif // PRE_ADD_VIP
		if (m_pKeepMaster && m_pKeepMaster->IsShow())
		{
			GetScreenMouseMovePoints(fMouseX, fMouseY);
			fMouseX -= GetXCoord();
			fMouseY -= GetYCoord();
			SUICoord uiCoord;
			m_pKeepMaster->GetUICoord(uiCoord);
			CDnSimpleTooltipDlg* pSimpleTooltipDlg = GetInterface().GetSimpleTooltipDialog();
			if (pSimpleTooltipDlg)
			{
				if (uiCoord.IsInside(fMouseX, fMouseY))
				{
					pSimpleTooltipDlg->ShowTooltipDlg(m_pKeepMaster, true, m_strMasterBonusText, 0xffffffff, true);
					pSimpleTooltipDlg->SetBottomPosition(true);
				}
			}
		}
#ifdef PRE_MOD_NESTREBIRTH

#ifdef PRE_FIX_NONESTTOOLTIP_NODRAGONNEST
		if (m_pRebirthLimitBack && m_pRebirthLimitBack->IsShow() &&
			m_pRebirthLimit && m_pRebirthLimit->IsShow() == false)
#else
		if (m_pRebirthLimitBack && m_pRebirthLimitBack->IsShow())
#endif
		{
			SUICoord uiCoord;
			m_pRebirthLimitBack->GetUICoord(uiCoord);
			if (uiCoord.IsInside(fMouseX, fMouseY))
				ShowNestRebirthTooltipDlg(fMouseX, fMouseY);
			else
				CloseNestRebirthTooltipDlg();
		}
#endif
	}
	break;

	case WM_RBUTTONDOWN:
	{
		POINT MousePoint;
		float fMouseX, fMouseY;

		MousePoint.x = short(LOWORD(lParam));
		MousePoint.y = short(HIWORD(lParam));
		PointToFloat(MousePoint, fMouseX, fMouseY);

		if (m_FaceType == FT_MINI_PLAYER && IsMouseInDlg())
		{
			if (CDnPartyTask::IsActive())
			{
				CDnPartyTask::PartyStruct *pMember = GetPartyTask().GetPartyDataFromMemberName(m_wszPlayerName.c_str());
#ifdef PRE_MOD_SHOW_CHAR_INFO_IN_VILLAGE
				if (pMember)	// Stage -> Village 파티갱신이 되었을때 항상 Struct->hActor 는 NULL 이다.  
								// 그리고 hActor 의 활용은 ShowPartyContextMenu() 에선 필요하지 않음으로 조건을 제외시킨다.  
#else 
				if (pMember && pMember->hActor)
#endif 
				{

#ifdef PRE_ADD_IMPROVE_MASTER_OBSERVER
					if (CDnActor::s_hLocalActor && pMember->hActor)
					{
						CDnGameTask *pGameTask = (CDnGameTask *)CTaskManager::GetInstance().GetTask("GameTask");
						if (pGameTask && pGameTask->GetGameTaskType() == GameTaskType::PvP)
						{
							CDnLocalPlayerActor* pLocalActor = static_cast<CDnLocalPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
							if (pLocalActor && pLocalActor->IsObserver())
							{
								DnCameraHandle hCamera = CDnCamera::GetActiveCamera();
								if (hCamera && hCamera->GetCameraType() == CDnCamera::PlayerCamera)
								{
									CDnPlayerCamera *pPlayerCamera = static_cast<CDnPlayerCamera*>(hCamera.GetPointer());
									pPlayerCamera->DetachActor();
									pPlayerCamera->AttachActor(pMember->hActor);
								}

								pLocalActor->SetFollowObserver(true, pMember->nSessionID);
								GetInterface().ResetPartyPlayerGaugeAttributes(false, true);
								GetInterface().SetPartyPlayerGaugeAttributes(pMember->nSessionID, true, false, true);
								break;
							}
							break;
						}
					}
#endif

					float dlgPosX, dlgPosY;
					GetPosition(dlgPosX, dlgPosY);
					dlgPosX += fMouseX;
					dlgPosY += fMouseY;
					ShowPartyContextMenu(true, dlgPosX, dlgPosY, *pMember);
				}
			}
		}
	}
	break;
	}

	return CDnCustomDlg::MsgProc(hWnd, uMsg, wParam, lParam);
}

#ifdef PRE_ADD_BEGINNERGUILD
void CDnGaugeDlg::UpdateKeepPartyInfo(int nKeepPartyRate, int nBestFriendRate, int nBeginnerGuildBonusRate)
#else
void CDnGaugeDlg::UpdateKeepPartyInfo(int nKeepPartyRate, int nBestFriendRate)
#endif
{
	CDnGameTask *pGameTask = (CDnGameTask *)CTaskManager::GetInstance().GetTask("GameTask");
	if (pGameTask && (pGameTask->GetGameTaskType() == GameTaskType::Farm || pGameTask->GetGameTaskType() == GameTaskType::PvP))
	{
		m_pKeepPartyIcon->Show(false);
		return;
	}

#ifdef PRE_ADD_BEGINNERGUILD
	if (nKeepPartyRate == 0 && nBestFriendRate == 0 && nBeginnerGuildBonusRate == 0) {
		m_pKeepPartyIcon->Show(false);
	}
	else {
		m_pKeepPartyIcon->Show(true);

		WCHAR wszStr[256];
		swprintf_s(wszStr, GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 1390), nKeepPartyRate, nBeginnerGuildBonusRate, nBestFriendRate);
		m_pKeepPartyIcon->SetTooltipText(wszStr);
	}
#else
	if (nKeepPartyRate == 0 && nBestFriendRate == 0) {
		m_pKeepPartyIcon->Show(false);
	}
	else {
		m_pKeepPartyIcon->Show(true);

		WCHAR wszStr[256];
		swprintf_s(wszStr, GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 1369), nKeepPartyRate, nBestFriendRate);
		m_pKeepPartyIcon->SetTooltipText(wszStr);
	}
#endif
}

int CDnGaugeDlg::SetKeepMasterTooltip(std::wstring& strText, int nExpBonusRate, int nMemberCount, int nStrIndex)
{
	if (nMemberCount == 0)
		return 0;

	strText += FormatW(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 7290), nExpBonusRate);
	strText += L"\n";

	CDnPartyTask::PartyStruct *pPartyStruct(NULL);
	DWORD dwPartyCount = GetPartyTask().GetPartyCount();

	int nFindMember = 0;
	for (DWORD i = 0; i<dwPartyCount; i++)
	{
		pPartyStruct = GetPartyTask().GetPartyData(i);
		if (!pPartyStruct) continue;

		const TMasterSystemSimpleInfo & SimpleInfo = GetMasterTask().GetSimpleInfo();

		for (BYTE j = 0; j<nMemberCount; j++)
		{
			if (__wcsicmp_l(SimpleInfo.OppositeInfo[j].wszCharName, pPartyStruct->wszCharacterName) == 0)
			{
				int nMasterFavorBonusExp = GetMasterFavorBonusExp(SimpleInfo.OppositeInfo[j].nFavorPoint);
				strText += FormatW(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, nStrIndex), pPartyStruct->wszCharacterName, SimpleInfo.OppositeInfo[j].nFavorPoint, nMasterFavorBonusExp);
				strText += L"\n";
				nFindMember++;
			}
		}
	}

	return nFindMember;
}

int CDnGaugeDlg::GetMasterFavorBonusExp(int nFavorPoint)
{
	DNTableFileFormat* pSoxMasterSysFeel = GetDNTable(CDnTableDB::TMASTERSYSFEEL);
	if (pSoxMasterSysFeel == NULL)
	{
		ErrorLog("pSoxMasterSysFeel table Not found!!");
		return 0;
	}

	int nMasterFavorBonusExp = 0;
	for (int i = 0; i<pSoxMasterSysFeel->GetItemCount(); ++i)
	{
		int nTableID = pSoxMasterSysFeel->GetItemID(i);
		int nMaxFeel = pSoxMasterSysFeel->GetFieldFromLablePtr(nTableID, "_MaxFeel")->GetInteger();
		if (nFavorPoint <= nMaxFeel)
		{
			nMasterFavorBonusExp = pSoxMasterSysFeel->GetFieldFromLablePtr(nTableID, "_AddExp")->GetInteger();
			break;
		}
	}

	return nMasterFavorBonusExp;
}

void CDnGaugeDlg::UpdateKeepMasterInfo(int nMasterCount, int nPupilCount, int nClassmateCount)
{
	if (nMasterCount == 0 && nPupilCount == 0)
		m_pKeepMaster->Show(false);
	else
	{
		m_pKeepMaster->Show(true);
		m_strMasterBonusText = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 7289);
		m_strMasterBonusText += L"\n";

		int nFindMember = 0;
		const TMasterSystemSimpleInfo & SimpleInfo = GetMasterTask().GetSimpleInfo();

		if (nMasterCount != 0)
		{
			nFindMember = SetKeepMasterTooltip(m_strMasterBonusText, static_cast<int>(CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::MasterSystem_MasterCountExpBonusRate) * 100), SimpleInfo.iMasterCount, 7292);
		}
		else if (nPupilCount != 0)
		{
			nFindMember = SetKeepMasterTooltip(m_strMasterBonusText, MasterSystem::Reward::PupilCountExpBonusRate, SimpleInfo.iPupilCount, 7291);
			m_strMasterBonusText += GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 7293);
			m_strMasterBonusText += L"\n";
			m_strMasterBonusText += GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 7294);

			if (nFindMember > 0)
			{
				m_strMasterBonusText += L"\n";
				m_strMasterBonusText += FormatW(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 7295), nFindMember, nFindMember);
			}
		}
	}

	if (nClassmateCount == 0)
		m_pKeepClassmate->Show(false);
	else
	{
		m_pKeepClassmate->Show(true);

		WCHAR wszString[256];
		swprintf_s(wszString, _countof(wszString), GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 7261), static_cast<int>(CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::MasterSystem_ClassmateCountExpBonusRate) * 100) * nClassmateCount);
		m_pKeepClassmate->SetTooltipText(wszString);
	}
}

void CDnGaugeDlg::ResetKeepMasterInfo()
{
	m_pKeepMaster->Show(false);
	m_pKeepClassmate->Show(false);
}

void CDnGaugeDlg::ShowPartyContextMenu(bool bShow, float fX, float fY, const CDnPartyTask::PartyStruct& memberInfo)
{
	CDnContextMenuDlg* pDlg = GetInterface().GetContextMenuDlg(CDnInterface::CONTEXTM_PARTY);
	if (pDlg)
	{
		if (bShow)
		{
			CDnPartyContextMenuDlg::SContextMenuPartyInfo info;
			info.dwSessionID = memberInfo.nSessionID;
			info.name = memberInfo.wszCharacterName;
			int nJob = memberInfo.cClassID;
			if (!memberInfo.nVecJobHistoryList.empty())
				nJob = memberInfo.nVecJobHistoryList[memberInfo.nVecJobHistoryList.size() - 1];
			info.job = nJob;
			info.level = memberInfo.cLevel;

			pDlg->SetInfo(&info);
			pDlg->SetPosition(fX, fY);
		}
		pDlg->Show(bShow);
	}
}

void CDnGaugeDlg::RefreshIcon()
{
	if (m_FaceType != FT_PLAYER)
		return;

	int nIconIndex = 0;

	if (m_pKeepPartyIcon->IsShow())
		++nIconIndex;

	if (m_pStaticItemRepair0->IsShow() || m_pStaticItemRepair1->IsShow() || m_pStaticItemRepair2->IsShow())
	{
		if (nIconIndex != m_pIconPosition[eRepair_Icon])
		{
			m_pStaticItemRepair0->SetPosition(m_pIconCoord[nIconIndex].fX, m_pIconCoord[nIconIndex].fY);
			m_pStaticItemRepair1->SetPosition(m_pIconCoord[nIconIndex].fX, m_pIconCoord[nIconIndex].fY);
			m_pStaticItemRepair2->SetPosition(m_pIconCoord[nIconIndex].fX, m_pIconCoord[nIconIndex].fY);
			m_pIconPosition[eRepair_Icon] = nIconIndex;
		}
		++nIconIndex;
	}

	if (m_pKeepMaster->IsShow())
	{
		if (nIconIndex != m_pIconPosition[eMaster_Icon])
		{
			m_pKeepMaster->SetPosition(m_pIconCoord[nIconIndex].fX, m_pIconCoord[nIconIndex].fY);
			m_pIconPosition[eMaster_Icon] = nIconIndex;
		}
		++nIconIndex;
	}

	if (m_pKeepClassmate->IsShow())
	{
		if (nIconIndex != m_pIconPosition[eClassmate_Icon])
		{
			m_pKeepClassmate->SetPosition(m_pIconCoord[nIconIndex].fX, m_pIconCoord[nIconIndex].fY);
			m_pIconPosition[eClassmate_Icon] = nIconIndex;
		}
		++nIconIndex;
	}

	for (int itr = 0; itr < MAX_BUFF_SLOT; ++itr)
	{
		if (m_pBuffCounter[itr]->IsShow())
		{
			if (nIconIndex != m_pIconPosition[eBuff_Icon1 + itr])
			{
				if (m_pBuffCounter[itr])
				{
					float fUIGap = (m_pBuffCounter[itr]->GetBaseUICoord().fWidth - m_pIconCoord[nIconIndex].fWidth) / 2;
					m_pBuffCounter[itr]->SetPosition(m_pIconCoord[nIconIndex].fX - fUIGap, m_pBuffCounter[itr]->GetBaseUICoord().fY);
				}
				if (m_pBuffTextureCtrl[itr])
				{
					m_pBuffTextureCtrl[itr]->SetPosition(m_pIconCoord[nIconIndex].fX, m_pBuffTextureCtrl[itr]->GetBaseUICoord().fY);
				}

				SUICoord bubbleUICoord;
				if (m_pBubbleCoolTimeTextureCtrl[itr])
				{
					m_pBubbleCoolTimeTextureCtrl[itr]->GetUICoord(bubbleUICoord);
					m_pBubbleCoolTimeTextureCtrl[itr]->SetPosition(m_pIconCoord[nIconIndex].fX, bubbleUICoord.fY);
				}
				m_pIconPosition[eBuff_Icon1 + itr] = nIconIndex;
			}
			++nIconIndex;
		}
	}

	/*if( eIndex < eMax_Custom_Icon )
	{
	switch( eIndex )
	{
	case eRepair_Icon :
	if( !m_pKeepPartyIcon->IsShow() && m_pIconPosition[eRepair_Icon] != eParty_Icon )
	{
	m_pStaticItemRepair0->SetPosition( m_pIconCoord[eParty_Icon] );
	m_pStaticItemRepair1->SetPosition( m_pIconCoord[eParty_Icon] );
	m_pStaticItemRepair2->SetPosition( m_pIconCoord[eParty_Icon] );
	}
	else if( m_pKeepPartyIcon->IsShow() && m_pIconCoord[eRepair_Icon] != eRepair_Icon )
	{
	m_pStaticItemRepair0->SetPosition( m_pIconCoord[eRepair_Icon] );
	m_pStaticItemRepair1->SetPosition( m_pIconCoord[eRepair_Icon] );
	m_pStaticItemRepair2->SetPosition( m_pIconCoord[eRepair_Icon] );
	}
	break;
	}
	}*/
}

void CDnGaugeDlg::InitializeFarm()
{
	m_pButtonPartyQuit->SetText(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 7459));	//현황 닫기
	m_pButtonPartyQuit->Show(true);
}

void CDnGaugeDlg::FinalizeFarm()
{
	m_pButtonPartyQuit->SetText(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 3517));	//파티 탈퇴
}

void CDnGaugeDlg::ToggleLifeCondition()
{
	m_bLifeConditionDlg = !GetInterface().GetLifeConditionDlg()->IsShow();

	GetInterface().GetLifeConditionDlg()->Show(m_bLifeConditionDlg);
	GetInterface().GetLifeConditionDlg()->SetShow(m_bLifeConditionDlg);

	if (m_bLifeConditionDlg)
		m_pButtonPartyQuit->SetText(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 7459));	//현황 닫기
	else
		m_pButtonPartyQuit->SetText(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 7460));	//현황 보기
}

#ifdef PRE_MOD_NESTREBIRTH
void CDnGaugeDlg::ShowNestRebirthTooltipDlg(float fMouseX, float fMouseY)
{
	if (m_pNestRebirthTooltipDlg == NULL)
		return;

	m_pNestRebirthTooltipDlg->AdjustPosition(fMouseX, fMouseY);
	m_pNestRebirthTooltipDlg->Show(true);
}

void CDnGaugeDlg::CloseNestRebirthTooltipDlg()
{
	m_pNestRebirthTooltipDlg->Show(false);
}
#endif

#ifdef PRE_ADD_PVP_SHOW_PARTY_GAUGE
void CDnGaugeDlg::ShowPvpPartyGauge(bool bShow)
{
	m_pStaticShowPvpGauge->Show(bShow);
	m_pCheckShowPvpGauge->Show(bShow);
	m_pCheckShowPvpGauge->SetChecked(bShow, false);
}

bool CDnGaugeDlg::CheckShowPvpPartyGauge()
{
	return m_pCheckShowPvpGauge->IsChecked();
}
#endif