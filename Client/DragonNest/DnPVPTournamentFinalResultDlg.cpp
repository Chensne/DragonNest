#include "stdafx.h"
#include "DnPVPTournamentFinalResultDlg.h"
#include "DnBridgeTask.h"
#include "SystemSendPacket.h"
#include "DnPVPTournamentDataMgr.h"
#include "DnPlayerActor.h"
#include "DnRevengeTask.h"
#include "TaskManager.h"
#include "DnInterface.h"
#include "DnGuildTask.h"
#include "DnItemTask.h"
#include "DnPVPTournamentFinalResultListDlg.h"
#include "DnPVPModeEndDlg.h"
#ifdef PRE_PVP_GAMBLEROOM
#include "DnInterfaceString.h"
#endif // PRE_PVP_GAMBLEROOM


#if !defined( USE_BOOST_MEMPOOL )
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
#endif // #if !defined( USE_BOOST_MEMPOOL )

#ifdef PRE_ADD_PVP_TOURNAMENT

void CDnPVPTournamentFinalResultDlg::SWinnersItem::ShowAll(bool bShow)
{
	pJobIcon->Show(bShow);
	pRankIcon->Show(bShow);
	pLevel->Show(bShow);
	pName->Show(bShow);
	pGuildMark->Show(bShow);
	pGuildName->Show(bShow);
	pKillCount->Show(bShow);
	pDeathCount->Show(bShow);
	pMedalCount->Show(bShow);
	pMedalIcon->Show(bShow);
	pItemSlot->Show(bShow);
}

CDnPVPTournamentFinalResultDlg::CDnPVPTournamentFinalResultDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback)
: CDnCustomDlg(dialogType, pParentDialog, nID, pCallback)
{
	m_pLosersListBox = NULL;
	m_pCloseButton = NULL;
	m_IsFinalResult = false;
	m_fTotal_ElapsedTime = 0.f;


#ifdef PRE_PVP_GAMBLEROOM
	m_bGamebleRoom = false;
#endif // PRE_PVP_GAMBLEROOM
}

CDnPVPTournamentFinalResultDlg::~CDnPVPTournamentFinalResultDlg()
{
	m_pLosersListBox->RemoveAllItems();

	SAFE_RELEASE_SPTR(m_hPVPMedalIconImage);
}

void CDnPVPTournamentFinalResultDlg::Initialize( bool bShow )
{
#ifdef PRE_PVP_GAMBLEROOM
	if( m_bGamebleRoom )
		CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName("PvpTournamentMatchResult_GDlg.ui").c_str(), bShow );
	else
		CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName("PvpTournamentMatchResultDlg.ui").c_str(), bShow );
#else // PRE_PVP_GAMBLEROOM
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName("PvpTournamentMatchResultDlg.ui").c_str(), bShow );
#endif // PRE_PVP_GAMBLEROOM
}

void CDnPVPTournamentFinalResultDlg::InitialUpdate()
{
	SAFE_RELEASE_SPTR(m_hPVPMedalIconImage);
	m_hPVPMedalIconImage = LoadResource(CEtResourceMng::GetInstance().GetFullName("Pvp_Medal.dds").c_str(), RT_TEXTURE);

	m_pLosersListBox = GetControl<CEtUIListBoxEx>("ID_LISTBOXEX_RANKLIST");
	m_pCloseButton = GetControl<CEtUIButton>("ID_BT_CLOSE");

	DNTableFileFormat* pSox = GetDNTable(CDnTableDB::TPVPGAMEMODE);
	if (pSox == NULL || CDnBridgeTask::IsActive() == false)
	{
		_ASSERT(0);
		return;
	}

	std::string ctrlName;
	int i = 0;
	for (; i < MAX_WINNERS_FOR_FINAL_RESULT_DLG; ++i)
	{
		ctrlName = FormatA("ID_SCORE_JOB%d", i + 1);
		m_WinnersUI[i].pJobIcon = GetControl<CDnJobIconStatic>(ctrlName.c_str());

		ctrlName = FormatA("ID_TEXTURE_RANK%d", i + 1);
		m_WinnersUI[i].pRankIcon = GetControl<CEtUITextureControl>(ctrlName.c_str());

		ctrlName = FormatA("ID_SCORE_LEVEL%d", i + 1);
		m_WinnersUI[i].pLevel = GetControl<CEtUIStatic>(ctrlName.c_str());

		ctrlName = FormatA("ID_SCORE_NAME%d", i + 1);
		m_WinnersUI[i].pName = GetControl<CEtUIStatic>(ctrlName.c_str());

		ctrlName = FormatA("ID_TEXTURE_MARK%d", i + 1);
		m_WinnersUI[i].pGuildMark = GetControl<CEtUITextureControl>(ctrlName.c_str());

		ctrlName = FormatA("ID_SCORE_GUILDNAME%d", i + 1);
		m_WinnersUI[i].pGuildName = GetControl<CEtUIStatic>(ctrlName.c_str());

		ctrlName = FormatA("ID_SCORE_KILLCOUND%d", i + 1);
		m_WinnersUI[i].pKillCount = GetControl<CEtUIStatic>(ctrlName.c_str());

		ctrlName = FormatA("ID_SCORE_DEATH%d", i + 1);
		m_WinnersUI[i].pDeathCount = GetControl<CEtUIStatic>(ctrlName.c_str());

#ifdef PRE_ADD_PVP_TOURNAMENT_WINNERXP
		ctrlName = FormatA("ID_SCORE_EX%d", i + 1);
		m_WinnersUI[i].pPVPXP = GetControl<CEtUIStatic>(ctrlName.c_str());
#endif

		ctrlName = FormatA("ID_SCORE_BLUE_MEDALICON%d", i + 1);
		m_WinnersUI[i].pMedalIcon = GetControl<CEtUITextureControl>(ctrlName.c_str());

		ctrlName = FormatA("ID_SCORE_MEDAL%d", i + 1);
		m_WinnersUI[i].pMedalCount = GetControl<CEtUIStatic>(ctrlName.c_str());

		ctrlName = FormatA("ID_BT_ITEM%d", i+1);
		m_WinnersUI[i].pItemSlot = GetControl<CDnItemSlotButton>(ctrlName.c_str());
		m_WinnersUI[i].pItemSlot->Show(false);

#ifdef PRE_PVP_GAMBLEROOM
		ctrlName = FormatA("ID_SCORE_PRIZE%d", i + 1);
		m_WinnersUI[i].pPrize = GetControl<CEtUIStatic>(ctrlName.c_str());
#endif // PRE_PVP_GAMBLEROOM
		ITEMCLSID itemId = ITEMCLSID_NONE;
		int count = 0;

		int itemRewardIndex = i+1;
		if (itemRewardIndex == 4)
			itemRewardIndex = 3;

		int tableId = CDnBridgeTask::GetInstance().GetPVPRoomStatus().m_nGameModeTableID;
		ctrlName = FormatA("_RankRewardItem%d", itemRewardIndex);
		DNTableCell* pCell = pSox->GetFieldFromLablePtr(tableId, ctrlName.c_str());
		if (pCell != NULL)
		{
			itemId = pCell->GetInteger();
			if (itemId != ITEMCLSID_NONE)
			{
				m_WinnersUI[i].pItemSlot->Show(true);

				ctrlName = FormatA("_RankRewardValue%d", itemRewardIndex);
				pCell = pSox->GetFieldFromLablePtr(tableId, ctrlName.c_str());
				if (pCell)
					count = pCell->GetInteger();

				if (CDnItemTask::IsActive())
				{
					TItem itemInfo;
					CDnItem::MakeItemInfo(itemId, count, itemInfo);

					SAFE_DELETE(m_WinnersUI[i].pItem);
					m_WinnersUI[i].pItem = CDnItemTask::GetInstance().CreateItem(itemInfo);

					if (m_WinnersUI[i].pItem)
						m_WinnersUI[i].pItemSlot->SetItem(m_WinnersUI[i].pItem, count);
				}
			}

		}
	}
}

void CDnPVPTournamentFinalResultDlg::FinalResultOpen()
{
	Show(false);
	m_IsFinalResult = true;
	UpdateUsers();

	CDnMouseCursor::GetInstance().ShowCursor( true, true );
}

void CDnPVPTournamentFinalResultDlg::Process(float fElapsedTime)
{
	CEtUIDialog::Process( fElapsedTime );

	if (m_IsFinalResult)
	{
		m_fTotal_ElapsedTime += fElapsedTime;

		if (m_fTotal_ElapsedTime > float(PvPCommon::Common::PvPFinishScoreOpenDelay))
		{
			if (!IsShow())
			{
				if (m_pCloseButton)
					m_pCloseButton->Show(true);

				CDnPVPModeEndDlg* pModeEndDlg = GetInterface().GetRespawnModeEnd();
				if (pModeEndDlg)
					pModeEndDlg->Show(false);

				UpdateUsers();
				CEtUIDialog::Show(true);	// 강제로 Show
			}

#ifdef _TEST_CODE_KAL
			int nRemainTime = 120 - int(m_fTotal_ElapsedTime);
#else
			int nRemainTime = PvPCommon::Common::PvPFinishAutoClose - int(m_fTotal_ElapsedTime);
#endif
			std::wstring closeMsg = FormatW(GetEtUIXML().GetUIString(CEtUIXML::idCategory1,121013), nRemainTime);
			if (m_pCloseButton)
				m_pCloseButton->SetText(closeMsg.c_str());
		}

		if (m_fTotal_ElapsedTime > float(PvPCommon::Common::PvPFinishAutoClose))
		{
			SendMovePvPGameToPvPLobby();
			m_IsFinalResult = false;
		}
	}
}

void CDnPVPTournamentFinalResultDlg::ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg /* = 0 */)
{
	SetCmdControlName(pControl->GetControlName());

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl("ID_BT_CLOSE" ) )
		{
			SendMovePvPGameToPvPLobby();
			return;
		}
	}

	CDnCustomDlg::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnPVPTournamentFinalResultDlg::AddUser(DnActorHandle hUser)
{
	if (hUser->GetTeam() == PvPCommon::Team::Observer)
		return;

	CDnPVPTournamentDataMgr* pDataMgr = GetDataMgr();
	if (pDataMgr == NULL)
	{
		_ASSERT(0);
		return;
	}

	if (hUser)
	{
		CDnPlayerActor* pPlayerActor = dynamic_cast<CDnPlayerActor*>(hUser.GetPointer());
		if (pPlayerActor)
		{
			SFinalReportUserInfo userInfo;
			userInfo.nSessionID = pPlayerActor->GetUniqueID();
			userInfo.nLevel = pPlayerActor->GetLevel();
			userInfo.cJobClassID = pPlayerActor->GetJobClassID();
			userInfo.cPVPlevel = pPlayerActor->GetPvPLevel();
			userInfo.wszUserName = pPlayerActor->GetName();
			if (pPlayerActor->IsJoinGuild())
				userInfo.GuildSelfView.Set(pPlayerActor->GetGuildSelfView());

#if defined( PRE_ADD_REVENGE )
			CDnRevengeTask * pRevengeTask = (CDnRevengeTask *)CTaskManager::GetInstance().GetTask( "RevengeTask" );
			if (NULL != pRevengeTask && CDnBridgeTask::GetInstance().GetSessionID() != userInfo.nSessionID)
				pRevengeTask->GetRevengeUserID(pPlayerActor->GetUniqueID(), userInfo.eRevengeUser);
#endif	// #if defined( PRE_ADD_REVENGE )

			pDataMgr->AddUserToFinalReportList(userInfo);

#ifdef PRE_WORLDCOMBINE_PVP
			GetInterface().UpdatePVPTournamentGameMatchUserList();
#endif // PRE_WORLDCOMBINE_PVP
		}
	}
}

void CDnPVPTournamentFinalResultDlg::RemoveUser(DnActorHandle hUser)
{
	CDnPVPTournamentDataMgr* pDataMgr = GetDataMgr();
	if (pDataMgr && hUser)
	{
		pDataMgr->RemoveUserFromFinalReportList(hUser->GetUniqueID());
		#ifdef PRE_WORLDCOMBINE_PVP
			GetInterface().UpdatePVPTournamentGameMatchUserList();
#endif // PRE_WORLDCOMBINE_PVP
	}
}

void CDnPVPTournamentFinalResultDlg::SetUserScore(int nSessionID, int nKOCount, int nKObyCount, int nCaptainKOCount, UINT uiKOP, UINT uiAssistP, UINT uiTotalP)
{
	CDnPVPTournamentDataMgr* pDataMgr = GetDataMgr();
	if (pDataMgr == NULL)
		return;

	std::vector<SFinalReportUserInfo>& reportList = pDataMgr->GetFinalReportUserList();
	std::vector<SFinalReportUserInfo>::iterator iter = reportList.begin();
	for (; iter != reportList.end(); ++iter)
	{
		SFinalReportUserInfo& report = (*iter);
		if( nSessionID == report.nSessionID )
		{
			report.uiKOCount = nKOCount;
			report.uiKObyCount = nKObyCount;
			report.uiKOP = uiKOP;
			report.uiAssistP = uiAssistP;
			report.uiTotalScore = uiTotalP;
			report.uiBossKOCount = nCaptainKOCount;
			return;
		}
	}
}

void CDnPVPTournamentFinalResultDlg::SetUserState(DnActorHandle hUser, int nState)
{
	CDnPVPTournamentDataMgr* pDataMgr = GetDataMgr();
	if (hUser && pDataMgr)
	{
		std::vector<SFinalReportUserInfo>& reportList = pDataMgr->GetFinalReportUserList();
		std::vector<SFinalReportUserInfo>::iterator iter = reportList.begin();
		for (; iter != reportList.end(); ++iter)
		{
			SFinalReportUserInfo& report = (*iter);
			if (hUser->GetUniqueID() == report.nSessionID && report.nState != MINE_S)
			{
				report.nState = nState;
				return;
			}
		}
	}
}

void CDnPVPTournamentFinalResultDlg::SetPVPXP(UINT nSessionID, UINT uiAddXPScore, UINT uiTotalXPScore, UINT uiMedalScore)
{
	CDnPVPTournamentDataMgr* pDataMgr = GetDataMgr();
	if (pDataMgr == NULL)
		return;

	std::vector<SFinalReportUserInfo>& reportList = pDataMgr->GetFinalReportUserList();
	std::vector<SFinalReportUserInfo>::iterator iter = reportList.begin();
	for (; iter != reportList.end(); ++iter)
	{
		SFinalReportUserInfo& report = (*iter);
		if (nSessionID == report.nSessionID)
		{
			report.uiXP = uiAddXPScore;
			report.uiMedal = uiMedalScore;
			report.uiTotalXP = uiTotalXPScore; 
			return;
		}
	}
}

void CDnPVPTournamentFinalResultDlg::UpdateUsers()
{
	CDnPVPTournamentDataMgr* pDataMgr = GetDataMgr();
	if (pDataMgr == NULL)
		return;

	pDataMgr->SortFinalReportUserList();

	std::wstring result;
	const std::vector<SFinalReportUserInfo>& finalReportInfoList = pDataMgr->GetFinalReportUserList();

	m_pLosersListBox->RemoveAllItems();

	int grade = LOSERS_FIRST_RANK - 1;
#ifdef PRE_PVP_GAMBLEROOM
	if( m_bGamebleRoom )
		grade = LOSERS_FIRST_RANK - 2;
#endif // PRE_PVP_GAMBLEROOM
	int prevTournamentStep = 0;
	int slotNumber = 0;
	std::vector<SFinalReportUserInfo>::const_iterator iter = finalReportInfoList.begin();
	for (; iter != finalReportInfoList.end(); ++iter, ++slotNumber)
	{
		const SFinalReportUserInfo& info = (*iter);
#ifdef PRE_PVP_GAMBLEROOM
		if( m_bGamebleRoom )
		{
			if( info.winnersRank == 1 || info.winnersRank == 2 )
			{
				UpdateWinnerUsers( info, info.winnersRank - 1 );
			}
			else
			{
				if( prevTournamentStep < info.tournamentStep )
				{
					prevTournamentStep = info.tournamentStep;
					++grade;
				}

				UpdateLoserUsers( info, grade );
			}

			continue;
		}
#endif // PRE_PVP_GAMBLEROOM
		if (info.winnersRank > 0)
		{
			UpdateWinnerUsers(info, info.winnersRank - 1);
		}
		else
		{
			if (prevTournamentStep < info.tournamentStep)
			{
				prevTournamentStep = info.tournamentStep;
				++grade;
			}

			UpdateLoserUsers(info, grade);
		}
	}
}

void CDnPVPTournamentFinalResultDlg::UpdateWinnerUsers(const SFinalReportUserInfo& info, int slotNumber)
{
	if (slotNumber >= MAX_WINNERS_FOR_FINAL_RESULT_DLG)
		return;

	SWinnersItem& current = m_WinnersUI[slotNumber];
	current.pJobIcon->SetIconID(info.cJobClassID);

	current.pDeathCount->SetInt64ToText(info.uiKObyCount);
#ifdef PRE_ADD_PVP_TOURNAMENT_WINNERXP
	current.pPVPXP->SetInt64ToText(info.uiXP);
#endif

	if (info.GuildSelfView.IsSet())
	{
		// 길드명
		current.pGuildName->SetText(info.GuildSelfView.wszGuildName);

		// 길드마크
		const TGuildView &GuildView = info.GuildSelfView;
		if (CDnGuildTask::IsActive() && GetGuildTask().IsShowGuildMark(GuildView))
		{
			EtTextureHandle hGuildMark = GetGuildTask().GetGuildMarkTexture(GuildView);
			current.pGuildMark->SetTexture(hGuildMark);
			current.pGuildMark->Show(true);
		}
	}

	current.pKillCount->SetInt64ToText(info.uiKOCount);
	current.pItemSlot->Show(info.uiKOCount > 0);

	std::wstring temp = FormatW(L"LV %d", info.nLevel);
	current.pLevel->SetText(temp.c_str());

	current.pName->SetText(info.wszUserName.c_str());

	//아이콘
	int iIconW,iIconH;
	int iU,iV;
	iIconW = GetInterface().GeticonWidth();
	iIconH = GetInterface().GeticonHeight();
	if (GetInterface().ConvertPVPGradeToUV(info.cPVPlevel, iU, iV))
	{
		current.pRankIcon->SetTexture(GetInterface().GetPVPIconTex(), iU, iV, iIconW, iIconH);
		current.pRankIcon->Show(true);
	}

	current.pMedalIcon->SetTexture(m_hPVPMedalIconImage, 0, 0, PVP_MEDAL_ICON_XSIZE, PVP_MEDAL_ICON_YSIZE);

	temp = FormatW(L"X %d", info.uiMedal);
	current.pMedalCount->SetText(temp.c_str());

#ifdef PRE_PVP_GAMBLEROOM
	if( m_bGamebleRoom && ( info.winnersRank == 1 || info.winnersRank == 2 ) )
	{
		std::wstring str;
		if( CDnBridgeTask::IsActive() )
		{
			const CDnPVPTournamentDataMgr* pDataMgr = &(CDnBridgeTask::GetInstance().GetPVPTournamentDataMgr());
			if( pDataMgr )
			{
				const SBasicRoomInfo& roomInfo = pDataMgr->GetRoomInfo();

				int nMoneyType = 0;
				if( roomInfo.cGambleType == PvPGambleRoom::Petal )
					nMoneyType = 4614;
				else if( roomInfo.cGambleType == PvPGambleRoom::Gold )
					nMoneyType = 507;

				int nTotalGamblePrice = roomInfo.cMaxPlayerNum * roomInfo.nGamblePrice;
				int nPrizeMoney = 0;

				if( info.winnersRank == 1 )
					nPrizeMoney = (int)( nTotalGamblePrice * 0.3f );
				else if( info.winnersRank == 2 )
					nPrizeMoney = (int)( nTotalGamblePrice * 0.2f );

				str = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8406 ), 
					DN_INTERFACE::UTIL::GetAddCommaString( nPrizeMoney ).c_str(), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nMoneyType ) );

				if( current.pPrize )
					current.pPrize->SetText( str.c_str() );
			}
		}
	}
#endif // PRE_PVP_GAMBLEROOM
}

void CDnPVPTournamentFinalResultDlg::UpdateLoserUsers(const SFinalReportUserInfo& info, int rank)
{
#ifdef PRE_PVP_GAMBLEROOM
	if( m_bGamebleRoom )
	{
		CDnPVPTournamentGambleFinalResultListDlg* pItemDlg = m_pLosersListBox->AddItem<CDnPVPTournamentGambleFinalResultListDlg>();
		if( pItemDlg )
			pItemDlg->SetInfo( info, rank );
	}
	else
	{
		CDnPVPTournamentFinalResultListDlg* pItemDlg = m_pLosersListBox->AddItem<CDnPVPTournamentFinalResultListDlg>();
		if( pItemDlg )
			pItemDlg->SetInfo( info, rank );
	}
#else // PRE_PVP_GAMBLEROOM
	CDnPVPTournamentFinalResultListDlg* pItemDlg = m_pLosersListBox->AddItem<CDnPVPTournamentFinalResultListDlg>();
	if (pItemDlg)
		pItemDlg->SetInfo(info, rank);
#endif // PRE_PVP_GAMBLEROOM
}

CDnPVPTournamentDataMgr* CDnPVPTournamentFinalResultDlg::GetDataMgr() const
{
	if (CDnBridgeTask::IsActive() == false)
		return NULL;

	return &(CDnBridgeTask::GetInstance().GetPVPTournamentDataMgr());
}

#endif	//#if defined(PRE_ADD_PVP_TOURNAMENT)