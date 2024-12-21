#include "stdafx.h"
#include "DnPVPTournamentRoomDlg.h"
#include "DnTableDB.h"
#include "DnInterface.h"
#include "PvPSendPacket.h"
#include "DnPVPTournamentDataMgr.h"
#include "DnPVPLobbyChatTabDlg.h"
#include "DnPVPLobbyVillageTask.h"
#include "DnPVPTournamentRoomMatchListDlg.h"
#include "DnPVPTournamentRoomUserListItemDlg.h"
#include "TaskManager.h"
#include "DnPVPRoomEditDlg.h"
#ifdef PRE_PVP_GAMBLEROOM
#include "DnInterfaceString.h"
#include "DnSimpleTooltipDlg.h"
#endif // PRE_PVP_GAMBLEROOM

#ifdef PRE_ADD_PVP_TOURNAMENT

CDnPVPTournamentRoomDlg::CDnPVPTournamentRoomDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback, bool bAutoCursor)
	: CDnCustomDlg(dialogType, pParentDialog, nID, pCallback , bAutoCursor)
{
	m_pExit = NULL;
	m_pTitle = NULL;
	m_pRoomIndexNum = NULL;
	m_pPasswordRoom = NULL;
	m_pMapName = NULL;
	m_pMiniMap = NULL;
	m_pUIPlayerNum = NULL;
	m_pUIGameMode = NULL;
	m_pMoveToObserver = NULL;
	m_pMoveToPlay = NULL;
	m_pEditPosition = NULL;
	m_pEditPositionComplete = NULL;
	m_pChangeMaster = NULL;
	m_pStart = NULL;
	m_pEditRoom = NULL;
	m_pWinCondition = NULL;
	m_pPVPMakeRoomDlg = NULL;
	m_pBreakIntoTitle = NULL;
	m_pDropItemTitle = NULL;

	m_pKick = NULL;
	m_pPermanentKick = NULL;
	m_pRoomName = NULL;
	m_pRoomStateStatic = NULL;
	m_pStaticModeInfo = NULL;

	int i = 0;
	for (; i < eSwitchMax; ++i)
	{
		m_pBreakIntoSwitch[i] = NULL;
		m_pDropItemSwitch[i] = NULL;
		m_pRevision[i] = NULL;
		m_pShowHp[i] = NULL;
		m_pReady[i] = NULL;
	}

	m_pMatchListDlg = NULL;
	m_pMatchUserListBox = NULL;
	m_pMatchListContainerListBox = NULL;
	m_pScrollBar = NULL;
	m_iScrollPos = -1;

	m_fOriContentDlgXPos = 0.f;
	m_fOriContentDlgYPos = 0.f;

	m_nBeepSound = -1;

#ifdef PRE_PVP_GAMBLEROOM
	m_pStaticGamblePrice = NULL;
#endif // PRE_PVP_GAMBLEROOM

	Reset();
}

CDnPVPTournamentRoomDlg::~CDnPVPTournamentRoomDlg(void)
{
	m_pMatchUserListBox->RemoveAllItems();
	SAFE_RELEASE_SPTR(m_hMiniMapImage);
	SAFE_DELETE(m_pPVPMakeRoomDlg);
	if (m_nBeepSound != -1)
		CEtSoundEngine::GetInstance().RemoveSound(m_nBeepSound);
}

void CDnPVPTournamentRoomDlg::InitialUpdate()
{
	m_pExit = GetControl<CEtUIButton>("ID_PVP_CANCEL");
	m_pTitle = GetControl<CEtUIStatic>("ID_TEXT_TOURNAMENT");

	m_pKick = GetControl<CEtUIButton>("ID_NORMAL_KICK");
	m_pKick->Enable(false);
	m_pPermanentKick = GetControl<CEtUIButton>( "ID_PERMAN_KICK" );
	m_pPermanentKick->Enable(false);

	m_pMoveToObserver = GetControl<CEtUIButton>("ID_PVP_OBSERVER");
	m_pMoveToPlay = GetControl<CEtUIButton>("ID_PVP_INGAME");
	m_pEditPosition = GetControl<CEtUIButton>("ID_TOURNAMENTEDIT");
	m_pEditPositionComplete = GetControl<CEtUIButton>("ID_CHANGE_GROUPLEADER");
	m_pEditPositionComplete->Show(false);
	m_pChangeMaster = GetControl<CEtUIButton>("ID_CHANGE_MASTER");
	m_pEditRoom = GetControl<CEtUIButton>("ID_EDIT");

	m_pRoomName = GetControl<CEtUIStatic>("ID_PVP_ROOMNAME");

	m_pPasswordRoom = GetControl<CEtUIStatic>("ID_PVP_SECRET");
	m_pPasswordRoom->Show(false);

	m_pRoomIndexNum = GetControl<CEtUIStatic>("ID_PVP_ROOMNUMBER");
	m_pMapName = GetControl<CEtUIStatic>("ID_PVP_MAPNAME");

	m_pMiniMap = GetControl<CEtUITextureControl>("ID_PVP_MAPIMG");
	m_pUIPlayerNum = GetControl<CEtUIStatic>("ID_PVP_MAP_REGULAR");
	m_pUIGameMode = GetControl<CEtUIStatic>("ID_PVP_MAP_MODE");

	m_pRoomStateStatic = GetControl<CEtUIStatic>("ID_PVP_MAP_STATE");
	m_pWinCondition = GetControl<CEtUIStatic>("ID_PVP_MAP_WINCONDITION");

	m_pBreakIntoTitle = GetControl<CEtUIStatic>("ID_STATIC6");
	m_pBreakIntoTitle->SetText(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 121124 )); // UISTRING : 랜덤순서

	m_pBreakIntoSwitch[eSwitchOn] = GetControl<CEtUIStatic>("ID_PVP_MAP_INTRUCTION");
	m_pBreakIntoSwitch[eSwitchOff] = GetControl<CEtUIStatic>("ID_PVP_MAP_NOINTRUCTION");
	m_pBreakIntoSwitch[eSwitchOn]->Show(false);
	m_pBreakIntoSwitch[eSwitchOff]->Show(false);

	m_pDropItemTitle = GetControl<CEtUIStatic>("ID_STATIC31");
	m_pDropItemTitle->Show(false);

	m_pDropItemSwitch[eSwitchOn] = GetControl<CEtUIStatic>("ID_PVP_ITEM");
	m_pDropItemSwitch[eSwitchOff] = GetControl<CEtUIStatic>("ID_PVP_NOITEM");
	m_pDropItemSwitch[eSwitchOn]->Show(false);
	m_pDropItemSwitch[eSwitchOff]->Show(false);

	m_pRevision[eSwitchOn] = GetControl<CEtUIStatic>("ID_PVP_FIT");
	m_pRevision[eSwitchOff] = GetControl<CEtUIStatic>("ID_PVP_NOFIT");
	m_pRevision[eSwitchOn]->Show(false);
	m_pRevision[eSwitchOff]->Show(false);

	m_pShowHp[eSwitchOn] = GetControl<CEtUIStatic>("ID_PVP_HP");
	m_pShowHp[eSwitchOff] = GetControl<CEtUIStatic>("ID_PVP_NOHP");
	m_pShowHp[eSwitchOn]->Show(false);
	m_pShowHp[eSwitchOff]->Show(false);

	m_pReady[eSwitchOn] = GetControl<CEtUIButton>("ID_PVP_READY_END");
	m_pReady[eSwitchOff] = GetControl<CEtUIButton>("ID_PVP_READY");

	m_pStart = GetControl<CEtUIButton>("ID_PVP_START");

	m_pMatchUserListBox	= GetControl<CEtUIListBoxEx>("ID_LISTBOXEX_LIST");
	m_pMatchListContainerListBox = GetControl<CEtUIListBox>("ID_LISTBOX_MATCH");
	m_pScrollBar = m_pMatchListContainerListBox->GetScrollBar();

	m_pPVPMakeRoomDlg = new CDnPVPRoomEditDlg( UI_TYPE_MODAL );
	m_pPVPMakeRoomDlg->Initialize( false );

	const char *szFileName = CDnTableDB::GetInstance().GetFileName(10007);
	if (strlen( szFileName ) > 0)
	{
		if (m_nBeepSound == -1)
			m_nBeepSound = CEtSoundEngine::GetInstance().LoadSound(CEtResourceMng::GetInstance().GetFullName(szFileName).c_str(), false, false);
	}

#ifdef PRE_ADD_PVP_HELP_MESSAGE
	for(int i = 0 ; i < E_PVP_RADIOBUTTON_MAX ; ++i)
		m_pRoomInfoButton[i] = GetControl<CEtUIRadioButton>( FormatA("ID_RBT_INFO%d",i).c_str() ); // 라디오 버튼

	m_pStaticModeInfo = GetControl<CEtUITextBox>("ID_TEXTBOX_MODEINFO"); // 모드 설명 백판
	m_pStaticModeInfo->Show(false);

	for(int i = 0 ; i < 2 ; ++i)
	{
		m_pStaticInfoBoard[i] = GetControl<CEtUIStatic>( FormatA("ID_STATIC_INFOBOARD%d",i).c_str() );
		m_pStaticInfoBoard[i]->Show(false);
	}

	for(int i = 0 ; i < 3 ; ++i)
		m_pStaticMapBoard[i] = GetControl<CEtUIStatic>( FormatA("ID_STATIC_MAPBOARD%d",i).c_str() );

	for(int i = 0 ; i < 8 ; ++i)
		m_pStaticTextMapInfo[i] = GetControl<CEtUIStatic>( FormatA("ID_TEXT_MAPINFO%d",i).c_str() ); // 인원,모드,상태~승리조건

	for(int i = 0 ; i < 5 ; ++i)
		m_pStaticMapLine[i] = GetControl<CEtUIStatic>( FormatA("ID_STATIC_MAPLINE%d",i).c_str() ); // 백판 라인

	m_pRoomInfoButton[E_PVP_RADIOBUTTON_ROOM_INFO]->SetChecked(true);
#endif

#ifdef PRE_PVP_GAMBLEROOM
	m_pStaticGamblePrice = GetControl<CEtUIStatic>( "ID_STATIC_PRIZE" );
	m_pStaticGamblePrice->Show( false );
#endif // PRE_PVP_GAMBLEROOM

	CDnCustomDlg::InitialUpdate();
}

void CDnPVPTournamentRoomDlg::Initialize(bool bShow)
{
	CDnCustomDlg::Initialize(CEtResourceMng::GetInstance().GetFullName("PvpTournamentReadyDlg.ui").c_str(), bShow);
}

void CDnPVPTournamentRoomDlg::ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg)
{
	CDnCustomDlg::ProcessCommand(nCommand, bTriggeredByUser, pControl, uMsg);

	SetCmdControlName( pControl->GetControlName() );

	if (nCommand == EVENT_BUTTON_CLICKED)
	{
		if (IsCmdControl("ID_PVP_CANCEL"))
		{
			GetInterface().GetPVPLobbyChatTabDlg()->RemoveObserver(0,true);
			SendPvPLeaveRoom();
			return;
		}

		if (IsCmdControl("ID_TOURNAMENTEDIT"))
		{
			CDnPVPTournamentDataMgr* pDataMgr = GetDataMgr();
			if (pDataMgr == NULL)
				return;

			pDataMgr->SetEditPositionState(true);
			ToggleEditPositionButton(false);

			m_pMatchListDlg->ResetSelect();

			return;
		}

		if (IsCmdControl("ID_CHANGE_GROUPLEADER"))
		{
			CDnPVPTournamentDataMgr* pDataMgr = GetDataMgr();
			if (pDataMgr == NULL)
				return;

			pDataMgr->SetEditPositionState(false);
			ToggleEditPositionButton(true);

			m_pMatchListDlg->ResetSelect();

			return;
		}

		if (IsCmdControl("ID_PVP_READY"))
		{
			SendPvPReady(TRUE);
			return;
		}

		if (IsCmdControl("ID_PVP_READY_END"))
		{
			SendPvPReady(FALSE);
			return;
		}

		if (IsCmdControl("ID_PVP_OBSERVER")) // 옵저버로 이동 !
		{
			SendPvPChangeTeam(PvPCommon::Team::Observer); // Observer = 1003입니다
			return;
		}

		if (IsCmdControl("ID_PVP_INGAME")) // 플레이어로 이동 !
		{
			SendPvPChangeTeam(PvPCommon::Team::A);
			return;
		}

		if (IsCmdControl("ID_CHANGE_MASTER"))
		{
			SendToServerChangeMaster();
			return;
		}

		if (IsCmdControl("ID_NORMAL_KICK"))
		{
			SendToServerKick(PvPCommon::BanType::Normal);
			return;
		}

		if (IsCmdControl("ID_PERMAN_KICK"))
		{
			SendToServerKick(PvPCommon::BanType::Permanence);
			return;
		}

		if (IsCmdControl("ID_PVP_START"))
		{
#ifdef _FINAL_BUILD
			SendPvPStart( PvPCommon::Check::AllCheck );
#else
			SendPvPStart(CGlobalValue::GetInstance().m_bPVPGameIgnoreCondition ? (PvPCommon::Check::AllCheck) : (PvPCommon::Check::CheckTimeOver|PvPCommon::Check::CheckScore));
#endif
			return;
		}

		if (IsCmdControl("ID_EDIT"))
		{
			CDnPVPTournamentDataMgr* pDataMgr = GetDataMgr();
			if (pDataMgr == NULL)
				return;
			const SBasicRoomInfo& info = pDataMgr->GetRoomInfo();

#ifdef PRE_MOD_PVP_ROOM_CREATE
			m_pPVPMakeRoomDlg->Show(true);
			m_pPVPMakeRoomDlg->SetMaxPlayerDownList(info.cMaxPlayerNum);
			m_pPVPMakeRoomDlg->SetMapAndModeDropDownList(info.uiMapIndex, info.uiGameModeID, m_pMapName->GetText());
			m_pPVPMakeRoomDlg->EnableGameModeDropDownList(false);
			m_pPVPMakeRoomDlg->EnableMapDropDownList(false);
			m_pPVPMakeRoomDlg->SetRoomName(info.roomName.c_str());
			m_pPVPMakeRoomDlg->SetPassword();
			m_pPVPMakeRoomDlg->SetInGameJoin(false);
			m_pPVPMakeRoomDlg->SetDropItem(false);
			m_pPVPMakeRoomDlg->SetGameWinObjectNum(m_pWinCondition->GetText());
			m_pPVPMakeRoomDlg->SetShowHp(info.bShowHp);
			m_pPVPMakeRoomDlg->SetRevision(info.bRevision);
			m_pPVPMakeRoomDlg->EnableRandomTeam(false);
			m_pPVPMakeRoomDlg->SetRandomTeamMode(info.bRandomTeam);
			m_pPVPMakeRoomDlg->SetEditState(true);
			m_pPVPMakeRoomDlg->SetMinMaxPlayerLevel(GetMinPlayerLevel(), GetMaxPlayerLevel());
			m_pPVPMakeRoomDlg->SetPlayerLevel(info.cMinLevel, info.cMaxLevel);
			m_pPVPMakeRoomDlg->SetRandomOrder(info.bRandomTeam);
#ifdef PRE_MOD_PVPOBSERVER
			m_pPVPMakeRoomDlg->SetObserver(m_bAllowObserver);
#endif

#else // PRE_MOD_PVP_ROOM_CREATE
			m_pPVPMakeRoomDlg->Show(true);
			m_pPVPMakeRoomDlg->SetMapDropDownList(info.uiMapIndex, m_pMapName->GetText(), info.cMaxPlayerNum);
			m_pPVPMakeRoomDlg->EnableGameModeDropDownList(false);
			m_pPVPMakeRoomDlg->EnableMapDropDownList(false);
			m_pPVPMakeRoomDlg->SetMaxPlayerDownList(info.cMaxPlayerNum);
			m_pPVPMakeRoomDlg->SetGameModeDownList(info.uiMapIndex, m_pUIGameMode->GetText());
			m_pPVPMakeRoomDlg->SetRoomName(info.roomName.c_str());
			m_pPVPMakeRoomDlg->SetPassword();
			m_pPVPMakeRoomDlg->SetInGameJoin(false);
			m_pPVPMakeRoomDlg->SetDropItem(false);
			m_pPVPMakeRoomDlg->SetGameWinObjectNum(m_pWinCondition->GetText());
			m_pPVPMakeRoomDlg->SetShowHp(info.bShowHp);
			m_pPVPMakeRoomDlg->SetRevision(info.bRevision);
			m_pPVPMakeRoomDlg->EnableRandomTeam(false);
			m_pPVPMakeRoomDlg->SetRandomTeamMode(info.bRandomTeam);
			m_pPVPMakeRoomDlg->SetEditState(true);
			m_pPVPMakeRoomDlg->SetMinMaxPlayerLevel(GetMinPlayerLevel(), GetMaxPlayerLevel());
			m_pPVPMakeRoomDlg->SetPlayerLevel(info.cMinLevel, info.cMaxLevel);
			m_pPVPMakeRoomDlg->SetRandomOrder(info.bRandomTeam);
#ifdef PRE_MOD_PVPOBSERVER
			m_pPVPMakeRoomDlg->SetObserver(m_bAllowObserver);
#endif

#endif // PRE_MOD_PVP_ROOM_CREATE
			
			return;
		}	
	}
	else if( nCommand == EVENT_RADIOBUTTON_CHANGED )
	{
#ifdef PRE_ADD_PVP_HELP_MESSAGE
		if( strstr(pControl->GetControlName() , "ID_RBT_INFO") )
		{
			int nTabID = static_cast<CEtUIRadioButton*>(pControl)->GetTabID();
			ChangeRoomInformation(nTabID);
		}
#endif
	}


}

void CDnPVPTournamentRoomDlg::SendToServerKick(PvPCommon::BanType::eBanType banType)
{
	const CDnPVPTournamentDataMgr* pDataMgr = GetDataMgr();
	if (pDataMgr == NULL || m_pMatchListDlg == NULL)
	{
		_ASSERT(0);
		return;
	}

	int uiIndex = m_pMatchListDlg->GetCurrentSelectUserUIIndex();
	if (uiIndex == -1)
	{
		if (GetInterface().GetPVPLobbyChatTabDlg())
		{
			CEtUIListBoxEx* pUI = GetInterface().GetPVPLobbyChatTabDlg()->GetObserverListBox();
			if (pUI)
			{
				if (GetInterface().GetPVPLobbyChatTabDlg()->GetObserverNumber() > 0) // 관전자가 없는데 강퇴 시키면 안됩니다. 
				{
					if (GetInterface().GetPVPLobbyChatTabDlg()->IsSelectedListBox())
					{
						int selectIndex = pUI->GetSelectedIndex();
						if (selectIndex >= 0 && selectIndex < GetInterface().GetPVPLobbyChatTabDlg()->GetObserverNumber())
						{
							int SelectedSessionID = GetInterface().GetPVPLobbyChatTabDlg()->GetSessionIDFromList(selectIndex);
							SendPvPBan(SelectedSessionID, banType);
							GetInterface().GetPVPLobbyChatTabDlg()->RemoveObserver(SelectedSessionID);
							return;
						}
					}
				}
			}
			// 관전자를 강퇴 해줍니다.
		}

		HandlePVPTournamentError(ePTERR_CANT_KICK_NO_SELECT);
		return;
	}

	const SMatchUserInfo* pInfo = pDataMgr->GetSlotInfoByUIIndex(uiIndex);
	if (pInfo == NULL || pInfo->uiSessionID == CDnBridgeTask::GetInstance().GetSessionID())
	{
		HandlePVPTournamentError(ePTERR_CANT_KICK_SELF);
		return;
	}

	SendPvPBan(pInfo->uiSessionID, banType);
}

void CDnPVPTournamentRoomDlg::SendToServerChangeMaster()
{
	const CDnPVPTournamentDataMgr* pDataMgr = GetDataMgr();
	if (pDataMgr == NULL || m_pMatchListDlg == NULL)
	{
		_ASSERT(0);
		return;
	}

	int uiIndex = m_pMatchListDlg->GetCurrentSelectUserUIIndex();
	if (uiIndex == -1)
	{
		//HandlePVPTournamentError(ePTERR_CANT_CHANGEMASTER_NO_SELECT);
		return;
	}

	const SMatchUserInfo* pInfo = pDataMgr->GetSlotInfoByUIIndex(uiIndex);
	if (pInfo == NULL || pInfo->uiSessionID == CDnBridgeTask::GetInstance().GetSessionID())
	{
		//HandlePVPTournamentError(ePTERR_CANT_CHANGEMASTER_SELF);
		return;
	}

	SendPvPChangeCaptain(pInfo->uiSessionID, PvPCommon::CaptainType::eCode::Captain);
}

void CDnPVPTournamentRoomDlg::ChangeTeam(UINT uiSessionID, int cTeam, char cTeamSlotIndex)
{
	int nPlayerTeamIndex = -1;

	const CDnPVPTournamentDataMgr* pDataMgr = GetDataMgr();
	if (pDataMgr == NULL)
	{
		_ASSERT(0);
		return;
	}

	//먼저 찾고
	int iSlotIndex = -1;
	int iPlayerIndex = -1;

	CDnPVPLobbyChatTabDlg* pLobbyChatDlg = GetInterface().GetPVPLobbyChatTabDlg();
	if (pLobbyChatDlg == NULL)
	{
		_ASSERT(0);
		return;
	}

	bool IsObserver = false;
	int i = 0;
	for (; i < pLobbyChatDlg->GetObserverNumber();i++)
	{
		if (pLobbyChatDlg->GetSessionIDFromList(i) == uiSessionID)
			IsObserver = true; // 세션아이디가 옵져버 리스트중에 하나일때 = 옵져버 -> 플레이어 전환
	}

	if (!IsObserver) // 일반 이동 처리 / 일반 - > 옵져버 이동
	{
		const SMatchUserInfo* pInfo = pDataMgr->GetSlotInfoBySessionID(uiSessionID);
		if (pInfo != NULL && pInfo->IsEmpty() == false) // 기본 슬롯중에 찾았다 = 나는 슬롯 플레이어다
		{
			SMatchUserInfo copyInfo = (*pInfo);
			RemovePlayer(uiSessionID, PvPCommon::LeaveType::Normal);

			if (cTeam != (PvPCommon::Team::Observer))
			{
				InsertPlayer(cTeam,
					copyInfo.uiUserState,
					uiSessionID,
					copyInfo.nJobID,
					copyInfo.cLevel,
					copyInfo.cPVPLevel,
					copyInfo.playerName.c_str(),
					copyInfo.commonIndex);
			}
			else if(cTeam == (PvPCommon::Team::Observer))
			{
				pLobbyChatDlg->InsertObserver(cTeam,
					copyInfo.uiUserState,
					uiSessionID , 
					copyInfo.nJobID,
					copyInfo.cLevel,
					copyInfo.cPVPLevel,
					copyInfo.playerName.c_str());
			}

			UpdateSlot(false);
			UpdateFunctionButtons();
		}
	}
	else // 옵져버 - > 플레이어 이동
	{

		CDnPVPLobbyChatTabDlg::sObserverSlot sSlot = pLobbyChatDlg->GetObserverSlotFromSessionID(uiSessionID); // 옵져버에 저장되어있는 정보
		pLobbyChatDlg->RemoveObserver(uiSessionID,false);  // 옵져버 슬롯에서 지워줌

		InsertPlayer(cTeam ,
			sSlot.uiUserState,	
			sSlot.uiSessionID , 
			sSlot.iJobID ,
			sSlot.cLevel ,
			sSlot.cPVPLevel ,
			sSlot.wszPlayerName,
			cTeamSlotIndex
			); // 플레이어로 토스

		UpdateSlot(false);
		UpdateFunctionButtons();

		CEtUIListBoxEx* pUI = pLobbyChatDlg->GetObserverListBox();
#ifdef PRE_MOD_PVPOBSERVER
		if (pUI->GetSelectedIndex() == -1 && (CGlobalInfo::GetInstance().m_cLocalAccountLevel < eAccountLevel::AccountLevel_New))
#else
		if (pUI->GetSelectedIndex() == -1)
#endif // PRE_MOD_PVPOBSERVER
		{
			m_pKick->Enable(false);
			m_pPermanentKick->Enable(false);
		}
	}
}

//	todo by kalliste : refactor with CDnPVPGameRoomDlg::PlaySound
void CDnPVPTournamentRoomDlg::PlaySound(byte cType)
{
	if (m_nBeepSound!= -1)
		CEtSoundEngine::GetInstance().PlaySound("2D", m_nBeepSound, false);
}

void CDnPVPTournamentRoomDlg::HandlePVPTournamentError(ePVPTournamentError error, bool bShowCode)
{
	std::wstring whole, errString;
	switch(error)
	{
	case ePTERR_CANT_KICK_SELF:
		{
			errString = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 3928); // UISTRING : 자기자신은 추방할 수 없습니다
		}
		break;

	case ePTERR_CANT_KICK_NO_SELECT:
		{
			errString = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 120212); // UISTRING : 추방할 캐릭터를 선택해 주세요
		}
		break;

	case ePTERR_CANT_CHANGEMASTER_NO_SELECT:
		{
			errString = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 120213); // UISTRING : 변경할 방장을 선택해 주세요
		}
		break;

	case ePTERR_CANT_CHANGEMASTER_SELF:
		{
			errString = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 120214); // UISTRING : 자기자신을 방장위임할 수 없습니다
		}
		break;

	default:
		{
			errString = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 120215); // UISTRING : 토너먼트 동작 오류입니다
			bShowCode = true;
		}
		break;
	}

	if (bShowCode)
		whole = FormatW(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 120216), errString.c_str(), error);	// UISTRING : %s(코드번호:%d)
	else
		whole = errString;

	GetInterface().MessageBox(whole.c_str(), MB_OK);
}

void CDnPVPTournamentRoomDlg::Process(float fElapsedTime)
{
	CDnCustomDlg::Process(fElapsedTime);
}

void CDnPVPTournamentRoomDlg::Show(bool bShow)
{
	if (m_bShow == bShow)
		return;

	if (m_pMatchListDlg)
	{
		m_pMatchListDlg->Show(bShow);
	}

#ifdef PRE_ADD_PVP_HELP_MESSAGE
	for(int i = 0 ; i < E_PVP_RADIOBUTTON_MAX ; ++i)
	{
		if(m_pRoomInfoButton[i])
		{
			m_pRoomInfoButton[i]->SetChecked(false);
			if(i == m_nRadioButtonIndex)
				m_pRoomInfoButton[i]->SetChecked(true);
		}
	}
#endif // PRE_ADD_PVP_HELP_MESSAGE

	CDnCustomDlg::Show(bShow);
}

void CDnPVPTournamentRoomDlg::Reset()
{
#ifdef PRE_MOD_PVPOBSERVER
	m_bEnterObserver = false;
	m_bAllowObserver = false;
	m_bEventRoom = false;
#endif // PRE_MOD_PVPOBSERVER

#ifdef PRE_ADD_PVP_HELP_MESSAGE
	m_nRadioButtonIndex = E_PVP_RADIOBUTTON_ROOM_INFO;
#endif
}

void CDnPVPTournamentRoomDlg::SetRoomUI()
{
	const CDnPVPTournamentDataMgr* pDataMgr = GetDataMgr();
	if (pDataMgr == NULL)
	{
		_ASSERT(0);
		return;
	}

	const SBasicRoomInfo& roomInfo = pDataMgr->GetRoomInfo();

	// 타이틀
	std::wstring titleString;
	int memberCount = CDnPVPTournamentDataMgr::GetMemberCountOfMode(pDataMgr->GetCurrentMode());
#ifdef PRE_PVP_GAMBLEROOM
	if( roomInfo.cGambleType != PvPGambleRoom::NoneType )
		titleString = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8413 ), memberCount ); // UISTRING : 토너먼트 %d강전 (도박모드)
	else
		titleString = FormatW(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 120161), memberCount); // UISTRING : 토너먼트 %d강전
#else // PRE_PVP_GAMBLEROOM
	titleString = FormatW(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 120161), memberCount); // UISTRING : 토너먼트 %d강전
#endif // PRE_PVP_GAMBLEROOM
	m_pTitle->SetText(titleString.c_str());

	// 방이름
	m_pRoomName->SetText(roomInfo.roomName.c_str());

	// 룸넘버
	std::wstring roomIndexString;
	roomIndexString = FormatW(L"%d. ", roomInfo.uiRoomIndex);
	m_pRoomIndexNum->SetText(roomIndexString.c_str());

	// 맵이름
	DNTableFileFormat* pMapSox = GetDNTable(CDnTableDB::TPVPMAP);
	if (!pMapSox)
	{
		ErrorLog("CDnPVPGameRoomDlg::RoomSetting:: PVP Map table Not found!!\n");
		return;
	}
	int nMapNameUIString = 0;
	int pvpMapTableId = pMapSox->GetItemIDFromField("MapTableID", roomInfo.uiMapIndex);
	DNTableCell* pMapNameCell = pMapSox->GetFieldFromLablePtr(pvpMapTableId , "MapNameUIstring");
	if (pMapNameCell == NULL)
	{
		ErrorLog("CDnPVPGameRoomDlg::RoomSetting:: PvP MapTable MapNameUIstring NOT FOUND!!\n");
		return;
	}
	nMapNameUIString = pMapNameCell->GetInteger();
	const wchar_t * wszMapName = NULL;
	wszMapName = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, nMapNameUIString);
	if (wszMapName)
		m_pMapName->SetText(wszMapName);

	// 맵이미지
	DNTableCell* pMapImageTextureNameCell = pMapSox->GetFieldFromLablePtr(pvpMapTableId , "MapImage");
	if (pMapImageTextureNameCell == NULL)
	{
		ErrorLog("CDnPVPGameRoomDlg::RoomSetting:: PvP MapTable MapImage NOT FOUND!!\n");
		return;
	}
	SAFE_RELEASE_SPTR(m_hMiniMapImage);
	std::string szMiniTextureName = pMapImageTextureNameCell->GetString();
	if (szMiniTextureName.c_str())
		m_hMiniMapImage = LoadResource(CEtResourceMng::GetInstance().GetFullName(szMiniTextureName.c_str()).c_str(), RT_TEXTURE);
	if (m_hMiniMapImage)
		m_pMiniMap->SetTexture(m_hMiniMapImage, 0, 0, m_hMiniMapImage->Width() , m_hMiniMapImage->Height());

	// 모드
	DNTableFileFormat* pPvPGameModeSox = GetDNTable(CDnTableDB::TPVPGAMEMODE);
	if (!pPvPGameModeSox)
	{
		ErrorLog("CDnPVPGameRoomDlg::RoomSetting:: gamemode table Not found!!\n");
		return;
	}
	int nGameModeUIString = 0;
	DNTableCell* pGameModeUIStringCell = pPvPGameModeSox->GetFieldFromLablePtr(roomInfo.uiGameModeID, "GameModeUIString");
	if (pGameModeUIStringCell == NULL)
	{
		ErrorLog("CDnPVPGameRoomDlg::RoomSetting:: gamemode table GameModeUIString field Not found!!\n");
		return;
	}
	nGameModeUIString = pGameModeUIStringCell->GetInteger();
	const WCHAR * wszModeName = NULL;
	wszModeName = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, nGameModeUIString);
	if (wszModeName)
		m_pUIGameMode->SetText(wszModeName);

	std::wstring wszWinCon;
	DNTableCell* pWinConditionCell = pPvPGameModeSox->GetFieldFromLablePtr(roomInfo.uiGameModeID, "WinCondition_UIString");
	if (pWinConditionCell)
	{
		int winConditionStringIndex = pWinConditionCell->GetInteger();
		wszWinCon = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, winConditionStringIndex);
	}
	m_pWinCondition->SetText(wszWinCon.c_str());

	// 상태 셋팅
	SetRoomStateUI();

	m_pBreakIntoSwitch[eSwitchOn]->Show(roomInfo.bRandomTeam);
	m_pBreakIntoSwitch[eSwitchOff]->Show(!roomInfo.bRandomTeam);

	// 보정
	m_pRevision[eSwitchOn]->Show(!roomInfo.bRevision);
	m_pRevision[eSwitchOff]->Show(roomInfo.bRevision);

	// 체력표시
	m_pShowHp[eSwitchOn]->Show(roomInfo.bShowHp);
	m_pShowHp[eSwitchOff]->Show(!roomInfo.bShowHp);

	m_iScrollPos = 0;
	m_fOriContentDlgXPos = m_fOriContentDlgYPos = 0.f;
}

void CDnPVPTournamentRoomDlg::RoomSetting(PvPCommon::RoomInfo* pInfo, bool IsModified)
{
#ifdef PRE_MOD_PVPOBSERVER
	m_bAllowObserver = pInfo->unRoomOptionBit & PvPCommon::RoomOption::AllowObserver ? true : false;
	m_bEventRoom = pInfo->bExtendObserver;
#endif // PRE_MOD_PVPOBSERVER

	CDnPVPTournamentDataMgr* pDataMgr = GetDataMgr();
	if (pDataMgr == NULL || pInfo == NULL)
	{
		_ASSERT(0);
		return;
	}

	if (IsModified)
		pDataMgr->ResetRoomInfo();
	else
		pDataMgr->Reset();

	pDataMgr->SetRoomInfo(*pInfo);
	const SBasicRoomInfo& basicRoomInfo = pDataMgr->GetRoomInfo();

	SetRoomUI();

	eMatchModeByCount currentMode = pDataMgr->GetCurrentMode();
	if (currentMode == eMODE_MAX)
	{
		_ASSERT(0);
		return;
	}

	if (IsModified == false)
	{
		if (m_pMatchListDlg)
			m_pMatchListDlg->ActivateMode(currentMode);
	}

	std::wstring gameStatusRoomName = FormatW(L"%d. %s", basicRoomInfo.uiRoomIndex, basicRoomInfo.roomName.c_str());
	CDnBridgeTask::GetInstance().SetPVPGameStatus(gameStatusRoomName, basicRoomInfo.uiWinCondition, basicRoomInfo.uiGameModeID, pInfo->uiPlayTimeSec, basicRoomInfo.cMaxPlayerNum, basicRoomInfo.bShowHp, basicRoomInfo.bRevision);

#ifdef PRE_ADD_PVP_HELP_MESSAGE
	if(IsModified)
	{
		ChangeRoomInformation(E_PVP_RADIOBUTTON_ROOM_INFO);
		if(m_pRoomInfoButton[m_nRadioButtonIndex])
			m_pRoomInfoButton[m_nRadioButtonIndex]->SetChecked(true);
	}
#endif

#ifdef PRE_PVP_GAMBLEROOM
	std::wstring str;
	if( pInfo->cGambleType == PvPGambleRoom::Petal )
	{
		int nWinnerPrizeMoney = (int)( pInfo->cMaxUserCount * pInfo->nGamblePrice * 0.3f );
		m_pStaticGamblePrice->Show( true );
		str = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8402 ), DN_INTERFACE::UTIL::GetAddCommaString( nWinnerPrizeMoney ).c_str(), 
						GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4614 ) );
		m_pStaticGamblePrice->SetText( str.c_str() );
	}
	else if( pInfo->cGambleType == PvPGambleRoom::Gold )
	{
		int nWinnerPrizeMoney = (int)( pInfo->cMaxUserCount * pInfo->nGamblePrice * 0.3f );
		m_pStaticGamblePrice->Show( true );
		str = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8402 ), DN_INTERFACE::UTIL::GetAddCommaString( nWinnerPrizeMoney ).c_str(), 
						GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 507 ) );
		m_pStaticGamblePrice->SetText( str.c_str() );
	}
	else
	{
		m_pStaticGamblePrice->Show( false );
	}
#endif // PRE_PVP_GAMBLEROOM
}

void CDnPVPTournamentRoomDlg::SetRoomStateUI()
{
	const CDnPVPTournamentDataMgr* pDataMgr = GetDataMgr();
	if (pDataMgr == NULL)
	{
		_ASSERT(0);
		return;
	}

	int nRoomStateUIString = 120028;
	if (pDataMgr->IsSyncingRoom())
		nRoomStateUIString = 120029;
	if (pDataMgr->IsStartingRoom())
		nRoomStateUIString = 120029;
	if (pDataMgr->IsPlayingRoom())
		nRoomStateUIString = 120030;

	const WCHAR* wszRoomState = NULL; 
	wszRoomState = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, nRoomStateUIString);
	m_pPasswordRoom->Show(pDataMgr->IsPasssWordRoom());

	if (wszRoomState)
		m_pRoomStateStatic->SetText(wszRoomState);
}

CDnPVPTournamentDataMgr* CDnPVPTournamentRoomDlg::GetDataMgr() const
{
	if (CDnBridgeTask::IsActive() == false)
		return NULL;

	return &(CDnBridgeTask::GetInstance().GetPVPTournamentDataMgr());
}

void CDnPVPTournamentRoomDlg::SetMatchListDlg(CDnPVPTournamentRoomMatchListDlg* pDlg)
{
	m_pMatchListDlg = pDlg;
}

void CDnPVPTournamentRoomDlg::InsertPlayer(int team, UINT uiUserState, UINT uiSessionID, int iJobID, BYTE cLevel, BYTE cPVPLevel, const WCHAR * szPlayerName, char cPosition)
{
	int i = 0;

	CDnPVPTournamentDataMgr* pDataMgr = GetDataMgr();
	if (pDataMgr == NULL)
	{
		_ASSERT(0);
		return;
	}

	SMatchUserInfo info;
	info.commonIndex = cPosition;
	info.nJobID = iJobID;
	info.cLevel = cLevel;
	info.uiUserState = uiUserState;
	info.cPVPLevel = cPVPLevel;
	info.playerName = szPlayerName;
	info.uiSessionID = uiSessionID;
	info.slotState = InPlayer;
	info.bIsNeedUpdate = true;

	bool bSet = pDataMgr->SetInfo(info);
	if (bSet == false)
	{
		ErrorLog("CDnPVPGameRoomDlg::InsertPlayer:: Not insert player because no empty slot.");
		return;
	}

	UpdateSlot(false);
	UpdateUserListBox();

	UpdateFunctionButtons();
}

#ifdef PRE_MOD_PVPOBSERVER
void CDnPVPTournamentRoomDlg::InsertObserver(UINT uiUserState, UINT uiSessionID)
{
	if (CDnBridgeTask::GetInstance().GetSessionID() == uiSessionID)
		SetUserState(uiSessionID, uiUserState);
}
#endif // PRE_MOD_PVPOBSERVER

void CDnPVPTournamentRoomDlg::ToggleEditPositionButton(bool bEnableEditPosition)
{
	m_pEditPosition->Show(bEnableEditPosition);
	m_pEditPositionComplete->Show(!bEnableEditPosition);
}

void CDnPVPTournamentRoomDlg::ToggleReadyButton(bool bEnableReady)
{
	m_pReady[eSwitchOn]->Show(bEnableReady);
	m_pReady[eSwitchOff]->Show(!bEnableReady);
}

void CDnPVPTournamentRoomDlg::ToggleObserverButton(bool bEnableObserver)
{
	m_pMoveToObserver->Show(bEnableObserver);
	m_pMoveToPlay->Show(!bEnableObserver);
}

void CDnPVPTournamentRoomDlg::UpdateFunctionButtons()
{
	int nLocalUserSessionID = CDnBridgeTask::GetInstance().GetSessionID();

	CDnPVPTournamentDataMgr* pDataMgr = GetDataMgr();
	if (pDataMgr == NULL)
	{
		_ASSERT(0);
		return;
	}

	UINT myState = 0;
	const SMatchUserInfo* pLocalUserInfo = pDataMgr->GetSlotInfoBySessionID(nLocalUserSessionID);
	if (pLocalUserInfo == NULL)
	{
		CDnPVPLobbyChatTabDlg* pChatTabDlg = GetInterface().GetPVPLobbyChatTabDlg();
		if (pChatTabDlg != NULL)
		{
			CDnPVPLobbyChatTabDlg::sObserverSlot sSlot = pChatTabDlg->GetObserverSlotFromSessionID(nLocalUserSessionID);
			myState = sSlot.uiUserState;
		}
		else
		{
			return;
		}
	}
	else
	{
		myState = pLocalUserInfo->uiUserState;
	}

	bool bMaster = IsMaster(myState);
	UpdateButtonsByMasterOrNot(*pDataMgr, bMaster);
	UpdateButtonsByState(*pDataMgr, myState);
}

void CDnPVPTournamentRoomDlg::UpdateButtonsByMasterOrNot(const CDnPVPTournamentDataMgr& mgr, bool bMaster)
{
	m_pMatchListDlg->ResetSelect();

	m_pKick->Show(bMaster);
	m_pPermanentKick->Show(bMaster);
	m_pChangeMaster->Show(bMaster);

	m_pStart->Show(bMaster || mgr.IsPlayingRoom());

	if (bMaster)
	{
		ToggleEditPositionButton(true);

		m_pMoveToObserver->Show(false);
		m_pMoveToPlay->Show(false);

		m_pReady[eSwitchOn]->Show(false);
		m_pReady[eSwitchOff]->Show(false);
	}
	else
	{
		m_pEditPosition->Show(false);
		m_pEditPositionComplete->Show(false);
	}

	m_pEditRoom->Show(bMaster);
}

void CDnPVPTournamentRoomDlg::UpdateButtonsByState(CDnPVPTournamentDataMgr& mgr, const UINT& userState)
{
	bool bMaster = IsMaster(userState);
	bool bReadyState = IsReady(userState);

	bool bIsPlayerObserver = false;
	bool bIsEnableMoveToObserver = false;
	bool bIsEnableMoveToPlay = false;
	bool bObserverState = IsEnableObserver();

	CDnPVPLobbyChatTabDlg* pLobbyChatDlg = GetInterface().GetPVPLobbyChatTabDlg();
	if (pLobbyChatDlg == NULL)
		return;

	int i = 0;
	for(; i < pLobbyChatDlg->GetObserverNumber();i++)
	{
		int nLocalUserSessionID = CDnBridgeTask::GetInstance().GetSessionID();
		if(pLobbyChatDlg->GetSessionIDFromList(i) == nLocalUserSessionID)
			bIsPlayerObserver = true;
	}

	bool bEnableEdit = false;
	bool bEnableChangeMaster = false;
	bool bEnableEditPosition = false;
	bool bEnableStart = mgr.IsEnableStartGame();
	bool bEnableExit = true;
	bool bEnableReady = false;

	if (mgr.IsReadyRoom())
	{
		bEnableEdit = true;
		bEnableChangeMaster = true;
		bEnableEditPosition = true;
		bEnableReady = (bMaster == false && bIsPlayerObserver == false);
		bIsEnableMoveToObserver = (bMaster == false && bReadyState == false);
#ifdef PRE_MOD_PVPOBSERVER				
		bIsEnableMoveToPlay = (!bMaster && IsEnableMoveToPlay());
		if (m_bEventRoom && bMaster)
			bIsEnableMoveToPlay = false;
#endif // PRE_MOD_PVPOBSERVER
	}
	else if (mgr.IsSyncingRoom())
	{
		bEnableExit = true;
		bEnableReady = false;
	}
	else if (mgr.IsStartingRoom())
	{
		bEnableChangeMaster = false;
		bEnableEdit = false;
		bEnableEditPosition = false;
		bEnableExit = false;
		bEnableStart = false;
		bEnableReady = false;
	}
	else if (mgr.IsPlayingRoom())
	{
		bEnableExit = true;
		bIsEnableMoveToObserver = true;
		bIsEnableMoveToPlay = false;
		bEnableStart = bIsPlayerObserver;
	}

#ifdef PRE_PVP_GAMBLEROOM
	const SBasicRoomInfo& roomInfo = mgr.GetRoomInfo();
	if( mgr.GetEventRoomIndex() > 0 || roomInfo.cGambleType != PvPGambleRoom::NoneType )
#else // PRE_PVP_GAMBLEROOM
	if( mgr.GetEventRoomIndex() > 0 )
#endif // PRE_PVP_GAMBLEROOM
	{
		bEnableReady = false;
		bIsEnableMoveToObserver = false;
		bIsEnableMoveToPlay = false;
	}

	m_pStart->Enable(bEnableStart);

	if (bMaster)
	{
		const SBasicRoomInfo& info = mgr.GetRoomInfo();
		if (info.bRandomTeam)
			bEnableEditPosition = false;

		m_pKick->Enable(bEnableEdit);
		m_pPermanentKick->Enable(bEnableEdit);
		m_pChangeMaster->Enable(bEnableChangeMaster);

		m_pEditPosition->Enable(bEnableEditPosition);
		m_pEditPositionComplete->Enable(bEnableEditPosition);

		if (bEnableEditPosition)
		{
			ToggleEditPositionButton(mgr.IsStateEditPosition() == false);
		}
		else
		{
			mgr.SetEditPositionState(false);
			ToggleEditPositionButton(mgr.IsStateEditPosition() == false);
		}

	}
	else
	{
		m_pMoveToPlay->Show(bIsPlayerObserver);
		m_pMoveToObserver->Show(!bIsPlayerObserver && (bMaster == false) && !IsGroupCaptain(userState));

		m_pMoveToObserver->Enable(bIsEnableMoveToObserver);
		m_pMoveToPlay->Enable((m_bEventRoom && m_bEnterObserver) ? false : bIsEnableMoveToPlay);

		if (m_pStart->IsShow() && m_pStart->IsEnable())
		{
			m_pReady[eSwitchOn]->Show(false);
			m_pReady[eSwitchOff]->Show(false);
		}
		else
		{
			if (mgr.IsPlayingRoom())
			{
				m_pReady[eSwitchOn]->Show(false);
				m_pReady[eSwitchOff]->Show(false);
			}
			else
			{
				ToggleReadyButton(bReadyState);
				m_pReady[eSwitchOn]->Enable(bEnableReady);
				m_pReady[eSwitchOff]->Enable(bEnableReady);
			}
		}
	}

	m_pExit->Enable(bEnableExit);
}

bool CDnPVPTournamentRoomDlg::IsEnableObserver() const
{
	CDnPVPLobbyChatTabDlg* pChatTabDlg = GetInterface().GetPVPLobbyChatTabDlg();
	if (pChatTabDlg == NULL)
	{
		_ASSERT(0);
		return false;
	}

	CDnPVPTournamentDataMgr* pDataMgr = GetDataMgr();
	if (pDataMgr == NULL)
	{
		_ASSERT(0);
		return false;
	}

	if (pDataMgr->GetCurrentMemberCount() < 2 || m_bEventRoom || !m_bAllowObserver)
		return false;

#ifdef PRE_MOD_PVPOBSERVER
	int nPlayerCnt = m_bEventRoom ? PvPCommon::Common::ExtendMaxObserverPlayer : PvPCommon::Common::MaxObserverPlayer;
	return (pChatTabDlg->GetObserverNumber() < nPlayerCnt); // 운영자의 경우 옵저버 16명.
#else
	return (pChatTabDlg->GetObserverNumber() < PvPCommon::Common::MaxObserverPlayer); // 현재는 4명까지만
#endif // PRE_MOD_PVPOBSERVER

}

bool CDnPVPTournamentRoomDlg::IsEnableMoveToPlay() const
{
	CDnPVPTournamentDataMgr* pDataMgr = GetDataMgr();
	if (pDataMgr == NULL)
	{
		_ASSERT(0);
		return false;
	}

	int maxUserCount = CDnPVPTournamentDataMgr::GetMemberCountOfMode(pDataMgr->GetCurrentMode());
	if (maxUserCount == eConstance::INVALID_MEMBERCOUNT)
	{
		_ASSERT(0);
		return false;
	}

	if (pDataMgr->GetCurrentMemberCount() >= maxUserCount)
		return false;

	return true;
}

void CDnPVPTournamentRoomDlg::SetUserState(UINT uiSessionID, UINT uiUserState, bool IsOuterCall)
{
	bool bIsMe = false;
	bool bIsChangeMaster =  false;
	std::wstring masterName;

	CDnPVPTournamentDataMgr* pDataMgr = GetDataMgr();
	if (pDataMgr == NULL)
	{
		_ASSERT(0);
		return;
	}

	const SMatchUserInfo* pUserInfo = pDataMgr->GetSlotInfoBySessionID(uiSessionID);
	if (pUserInfo && pUserInfo->slotState == InPlayer)
	{
		SMatchUserInfo matchInfo;
		matchInfo = *pUserInfo;

		if (!IsMaster(matchInfo.uiUserState) && IsMaster(uiUserState))
		{
			bIsChangeMaster= true;
			masterName = matchInfo.playerName;
			if (CDnBridgeTask::GetInstance().GetSessionID() == uiSessionID)	// 자기 자신일때
				bIsMe = true;
		}

		matchInfo.uiUserState = uiUserState;
		matchInfo.bIsNeedUpdate = true;
		if (pDataMgr->SetInfo(matchInfo) == false)
		{
			_ASSERT(0);
			return;
		}
	}

	if (IsOuterCall)
	{
		//님이 방장 됐음.
		if (bIsMe && bIsChangeMaster)
		{
			GetInterface().MessageBox(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 120132), MB_OK);
		}
		else if(bIsChangeMaster)
		{
			std::wstring temp;
			temp = FormatW(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 120131), masterName.c_str());

			GetInterface().AddChatMessage(CHATTYPE_SYSTEM, L"",temp.c_str());
		}
	}

	UpdateSlot(false);
	UpdateFunctionButtons();
}

void CDnPVPTournamentRoomDlg::UpdateSlot(bool bForceAll)
{
	CDnPVPTournamentDataMgr* pDataMgr = GetDataMgr();
	if (pDataMgr == NULL)
	{
		_ASSERT(0);
		return;
	}

	m_pMatchListDlg->ResetMatchListUI();

	const std::vector<SMatchUserInfo>& matchUserInfoList = pDataMgr->GetMatchUserList();
	int i = 0;
	for (; i < (int)matchUserInfoList.size(); ++i)
	{
		const SMatchUserInfo& curInfo = matchUserInfoList[i];
		if (curInfo.IsEmpty() == false)
			m_pMatchListDlg->UpdateMatchListUI(curInfo);
	}

	UpdateUserListBox();

	const SBasicRoomInfo& roomInfo = pDataMgr->GetRoomInfo();
	std::wstring playerNumString;
	playerNumString = FormatW(L"%d/%d", pDataMgr->GetCurrentMemberCount(), roomInfo.cMaxPlayerNum);
	m_pUIPlayerNum->SetText(playerNumString.c_str());
}

void CDnPVPTournamentRoomDlg::UpdateUserListBox()
{
	m_pMatchUserListBox->RemoveAllItems();

	CDnPVPTournamentDataMgr* pDataMgr = GetDataMgr();
	if (pDataMgr == NULL)
	{
		_ASSERT(0);
		return;
	}

	const std::vector<SMatchUserInfo>& userList = pDataMgr->GetMatchUserList();
	int i = 0;
	for (; i < (int)userList.size(); ++i)
	{
		const SMatchUserInfo& info = userList[i];
		CDnPVPTournamentRoomUserListItemDlg* pItemDlg = m_pMatchUserListBox->AddItem<CDnPVPTournamentRoomUserListItemDlg>();
		if (pItemDlg)
			pItemDlg->SetInfo(info);
	}
}

void CDnPVPTournamentRoomDlg::_SetContentOffset()
{
	if (m_pMatchListDlg)
	{
		SUIDialogInfo matchListDlgInfo;
		m_pMatchListDlg->GetDlgInfo(matchListDlgInfo);

		if (0.f == m_fOriContentDlgXPos && 0.f == m_fOriContentDlgYPos)
		{
			m_fOriContentDlgXPos = matchListDlgInfo.DlgCoord.fX;
			m_fOriContentDlgYPos = matchListDlgInfo.DlgCoord.fY;
		}

		const SUICoord& matchListBoxCoord = m_pMatchListContainerListBox->GetUICoord();
		float fPageSize = matchListBoxCoord.fHeight / GetScreenHeightRatio() * GetEtDevice()->Height();
		m_pScrollBar->SetPageSize((int)fPageSize);

		// 다이얼로그는 다이얼로그 스케일 값도 같이 곱해지기 때문에 정확한 다이얼로그 사이즈를 구하기 위해서 같이 고려되어야 한다.
		float currentListUIHeight = m_pMatchListDlg->GetCurrentListUIHeight();
		if (currentListUIHeight < 0.f)
		{
			_ASSERT(0);
			return;
		}

		m_pScrollBar->SetTrackRange(0, int(currentListUIHeight * (float)GetEtDevice()->Height() / (float)DEFAULT_UI_SCREEN_HEIGHT * (float)DEFAULT_UI_SCREEN_HEIGHT) );
		if (m_iScrollPos != -1)
		{
			m_pScrollBar->SetTrackPos(m_iScrollPos);
			m_iScrollPos = -1;
		}

		int iPos = m_pScrollBar->GetTrackPos();
		float fNowYOffsetRatio = (float)iPos / fPageSize;
		float fHeight = matchListBoxCoord.fHeight / GetScreenHeightRatio();

		m_pMatchListDlg->SetPosition(m_fOriContentDlgXPos, m_fOriContentDlgYPos - fNowYOffsetRatio * fHeight);
	}
}

void CDnPVPTournamentRoomDlg::Render( float fElapsedTime )
{
	CEtUIDialog::Render( fElapsedTime );

	if (IsShow())
	{
		// 스크롤바의 위치에 따라 다이얼로그의 위치를 옮겨줌.
		_SetContentOffset();

		// 스크롤 바를 위해 생성해 놓은 리스트 박스의 너비가 content 사이즈다.
		// 이걸로 뷰포트 잡으면 됨.
		RECT oldRect, newRect;
		GetEtDevice()->GetScissorRect(&oldRect);

		const SUICoord& ContentListBoxCoord = m_pMatchListContainerListBox->GetUICoord();
		newRect.left = int((ContentListBoxCoord.fX+GetXCoord()) / GetScreenWidthRatio() * GetEtDevice()->Width());
		newRect.top = int((ContentListBoxCoord.fY+GetYCoord()) / GetScreenHeightRatio() * GetEtDevice()->Height());
		newRect.right = newRect.left + int(ContentListBoxCoord.fWidth / GetScreenWidthRatio() * GetEtDevice()->Width());
		newRect.bottom = newRect.top + int(ContentListBoxCoord.fHeight / GetScreenHeightRatio() * GetEtDevice()->Height());

		CEtSprite::GetInstance().Flush();
		GetEtDevice()->SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE);
		GetEtDevice()->SetScissorRect(&newRect);

		m_pMatchListDlg->Show(true);
		m_pMatchListDlg->Render(fElapsedTime);
		m_pMatchListDlg->Show(false);

		CEtSprite::GetInstance().Flush();
		GetEtDevice()->SetScissorRect(&oldRect);
		GetEtDevice()->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
	}
}

void CDnPVPTournamentRoomDlg::OnSwapPosition(const int& srcPos, const int& destPos)
{
	CDnPVPTournamentDataMgr* pDataMgr = GetDataMgr();
	if (pDataMgr == NULL)
	{
		_ASSERT(0);
		return;
	}

	ResetHoldSwapPosition(*pDataMgr);

	if (srcPos == destPos || srcPos == -1 || destPos == -1)
	{
		return;
	}

	if (pDataMgr->SwapSlotByCommonIndex(srcPos, destPos) == false)
	{
		_ASSERT(0);
		return;
	}

	const SMatchUserInfo* pSrcInfo = pDataMgr->GetSlotInfoByCommonIndex(srcPos);
	const SMatchUserInfo* pDestInfo = pDataMgr->GetSlotInfoByCommonIndex(destPos);
	if (pSrcInfo == NULL || pDestInfo == NULL)
	{
		_ASSERT(0);
		return;
	}

	m_pMatchListDlg->UpdateMatchListUI(*pSrcInfo);
	m_pMatchListDlg->UpdateMatchListUI(*pDestInfo);

	std::wstring msg = FormatW(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 120164), pDestInfo->playerName.c_str(), pSrcInfo->playerName.c_str()); // UISTRING : %s님과 %s님의 자리가 변경되었습니다
	GetInterface().AddChatMessage(CHATTYPE_SYSTEM, L"", msg.c_str(), false);
}

void CDnPVPTournamentRoomDlg::ResetHoldSwapPosition(CDnPVPTournamentDataMgr& mgr)
{
	mgr.HoldSwapPosition(false);
	m_pMatchListDlg->ResetSelect();
}

void CDnPVPTournamentRoomDlg::RemovePlayer(UINT uiSessionID, PvPCommon::LeaveType::eLeaveType eType)
{
	std::wstring msg;

	CDnPVPTournamentDataMgr* pDataMgr = GetDataMgr();
	if (pDataMgr == NULL)
	{
		_ASSERT(0);
		return;
	}

	const SMatchUserInfo* pInfo = pDataMgr->GetSlotInfoBySessionID(uiSessionID);
	if (pInfo != NULL)
	{
		int nUIString = 0;
		if (eType == PvPCommon::LeaveType::Ban)
			nUIString = 121061;
		else if (eType == PvPCommon::LeaveType::PermanenceBan)
			nUIString = 121062;

		if (nUIString)
		{
			msg = FormatW(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, nUIString), pInfo->playerName.c_str());
			GetInterface().AddChatMessage(CHATTYPE_SYSTEM, L"", msg.c_str(), false);
		}

		pDataMgr->RemoveInfo(uiSessionID);

		UpdateSlot(false);
		UpdateUserListBox();

		UpdateFunctionButtons();

		return;
	}

	ErrorLog("CDnPVPGameRoomDlg::RemovePlayer:: SessionID Not Found %d",uiSessionID);
}

void CDnPVPTournamentRoomDlg::SetRoomState(UINT roomState)
{
	CDnPVPTournamentDataMgr* pDataMgr = GetDataMgr();
	if (pDataMgr == NULL)
	{
		_ASSERT(0);
		return;
	}

	pDataMgr->SetRoomState(roomState);
	SetRoomStateUI();

	UpdateFunctionButtons();
}

int CDnPVPTournamentRoomDlg::GetMaxPlayerLevel() const
{
	int temp = 0;

	const CDnPVPTournamentDataMgr* pDataMgr = GetDataMgr();
	if (pDataMgr == NULL)
	{
		_ASSERT(0);
		return 0;
	}

	const std::vector<SMatchUserInfo>& userList = pDataMgr->GetMatchUserList();
	int i = 0;
	for (; i < (int)userList.size(); ++i)
	{
		const SMatchUserInfo& info = userList[i];
		if (temp < (int)info.cLevel)
			temp = info.cLevel;
	}

	return temp;
}

int CDnPVPTournamentRoomDlg::GetMinPlayerLevel() const
{
	int temp = 200;

	const CDnPVPTournamentDataMgr* pDataMgr = GetDataMgr();
	if (pDataMgr == NULL)
	{
		_ASSERT(0);
		return 200;
	}

	const std::vector<SMatchUserInfo>& userList = pDataMgr->GetMatchUserList();
	int i = 0;
	for (; i < (int)userList.size(); ++i)
	{
		const SMatchUserInfo& info = userList[i];
		if (temp > (int)info.cLevel)
			temp = info.cLevel;
	}

	return temp;
}

#ifdef PRE_ADD_PVP_HELP_MESSAGE
void CDnPVPTournamentRoomDlg::ChangeRoomInformation(int nTabID)
{
	m_nRadioButtonIndex = nTabID;
	
	if(nTabID == E_PVP_RADIOBUTTON_ROOM_INFO)
	{
		bool bShow = true;

		//--------------------------
		// 맵 관련 컨트롤
		if(m_pUIGameMode)
			m_pUIGameMode->Show(bShow);
		if(m_pUIPlayerNum)
			m_pUIPlayerNum->Show(bShow);
		if(m_pMiniMap)
			m_pMiniMap->Show(bShow);
		if(m_pMapName)
			m_pMapName->Show(bShow);
		if(m_pRoomStateStatic)
			m_pRoomStateStatic->Show(bShow);
		
		const CDnPVPTournamentDataMgr* pDataMgr = GetDataMgr();
		if (pDataMgr == NULL)
		{
			_ASSERT(0);
			return;
		}
		const SBasicRoomInfo& roomInfo = pDataMgr->GetRoomInfo();

		m_pWinCondition->Show(bShow);

		m_pDropItemSwitch[eSwitchOn]->Show(roomInfo.bDropItem);
		m_pDropItemSwitch[eSwitchOff]->Show(!roomInfo.bDropItem);
		

		m_pBreakIntoSwitch[eSwitchOn]->Show(roomInfo.bRandomTeam);
		m_pBreakIntoSwitch[eSwitchOff]->Show(!roomInfo.bRandomTeam);

		// 보정
		m_pRevision[eSwitchOn]->Show(!roomInfo.bRevision);
		m_pRevision[eSwitchOff]->Show(roomInfo.bRevision);

		// 체력표시
		m_pShowHp[eSwitchOn]->Show(roomInfo.bShowHp);
		m_pShowHp[eSwitchOff]->Show(!roomInfo.bShowHp);


		for(int i = 0 ; i < 3 ; ++i)
		{
			if(m_pStaticMapBoard[i])
				m_pStaticMapBoard[i]->Show(bShow);
		}

		for(int i = 0 ; i < 8 ; ++i)
		{
			if(m_pStaticTextMapInfo[i])
				m_pStaticTextMapInfo[i]->Show(bShow);
		}

		for(int i = 0 ; i < 5 ; ++i)
		{
			if(m_pStaticMapLine[i])
				m_pStaticMapLine[i]->Show(bShow);
		}


		//--------------------------
		// 모드 관련 컨트롤
		for(int i = 0 ; i < 2 ; ++i)
		{
			// 모드설명 백판
			if(m_pStaticInfoBoard[i])
				m_pStaticInfoBoard[i]->Show(!bShow);
		}

		m_pStaticModeInfo->ClearText();
		m_pStaticModeInfo->Show(!bShow);
	}
	else if(nTabID == E_PVP_RADIOBUTTON_MODE_INFO)
	{
		bool bShow = false;

		//--------------------------
		// 맵 관련 컨트롤
		if(m_pUIGameMode)
			m_pUIGameMode->Show(bShow);
		if(m_pUIPlayerNum)
			m_pUIPlayerNum->Show(bShow);
		if(m_pMiniMap)
			m_pMiniMap->Show(bShow);
		if(m_pMapName)
			m_pMapName->Show(bShow);
		if(m_pRoomStateStatic)
			m_pRoomStateStatic->Show(bShow);

		for(int i = 0 ; i < eSwitchMax ; ++i)
		{
			m_pBreakIntoSwitch[i]->Show(bShow);
			m_pDropItemSwitch[i]->Show(bShow);
			m_pDropItemSwitch[i]->Show(bShow);
			m_pRevision[i]->Show(bShow);
			m_pShowHp[i]->Show(bShow);
		}

		m_pWinCondition->Show(bShow);

		for(int i = 0 ; i < 3 ; ++i)
		{
			if(m_pStaticMapBoard[i])
				m_pStaticMapBoard[i]->Show(bShow);
		}

		for(int i = 0 ; i < 8 ; ++i)
		{
			if(m_pStaticTextMapInfo[i])
				m_pStaticTextMapInfo[i]->Show(bShow);
		}

		for(int i = 0 ; i < 5 ; ++i)
		{
			if(m_pStaticMapLine[i])
				m_pStaticMapLine[i]->Show(bShow);
		}


		//--------------------------
		// 모드 관련 컨트롤
		for(int i = 0 ; i < 2 ; ++i)
		{
			// 모드설명 백판
			if(m_pStaticInfoBoard[i])
				m_pStaticInfoBoard[i]->Show(!bShow);
		}

		m_pStaticModeInfo->ClearText();
		if(nTabID == E_PVP_RADIOBUTTON_MODE_INFO)
		{
			const CDnPVPTournamentDataMgr* pDataMgr = GetDataMgr();
			if (pDataMgr == NULL)
			{
				_ASSERT(0);
				return;
			}
			const SBasicRoomInfo& roomInfo = pDataMgr->GetRoomInfo();

			// 툴팁 설정
			DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TPVPGAMEMODE );
			if (!pSox) return;

			int nToolTipStringID = pSox->GetFieldFromLablePtr(roomInfo.uiGameModeID , "_TooltipUIString")->GetInteger();
			std::vector<std::wstring> vecString;
			TokenizeW_NewLine(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nToolTipStringID ), vecString, L"\\n" );

			for(int i = 0 ; i < (int)vecString.size() ; ++i)
			{
				m_pStaticModeInfo->AddText(vecString[i].c_str());
			}
		}

		// 모드설명 Text Static
		m_pStaticModeInfo->Show(!bShow);
	}
}

#endif

#ifdef PRE_PVP_GAMBLEROOM

bool CDnPVPTournamentRoomDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsShow() )
		return false;

	switch( uMsg )
	{
	case WM_MOUSEMOVE:
		{
			POINT MousePoint;
			MousePoint.x = short( LOWORD( lParam ) );
			MousePoint.y = short( HIWORD( lParam ) );

			float fMouseX, fMouseY;
			PointToFloat( MousePoint, fMouseX, fMouseY );

			CDnSimpleTooltipDlg* pSimpleTooltTip = GetInterface().GetSimpleTooltipDialog();
			if( pSimpleTooltTip == NULL )
				break;

			if( m_pStaticGamblePrice->IsShow() && m_pStaticGamblePrice->GetUICoord().IsInside( fMouseX, fMouseY ) )
			{
				std::wstring str;
				const CDnPVPTournamentDataMgr* pDataMgr = GetDataMgr();
				if( pDataMgr )
				{
					const SBasicRoomInfo& roomInfo = pDataMgr->GetRoomInfo();
				
					int nMoneyType = 0;
					if( roomInfo.cGambleType == PvPGambleRoom::Petal )
						nMoneyType = 4614;
					else if( roomInfo.cGambleType == PvPGambleRoom::Gold )
						nMoneyType = 507;

					int nTotalGamblePrice = roomInfo.cMaxPlayerNum * roomInfo.nGamblePrice;
					int nWinnerPrizeMoney = (int)( nTotalGamblePrice * 0.3f );
					int nSecondPrizeMoney = (int)( nTotalGamblePrice * 0.2f );

					str = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8403 ), 
									DN_INTERFACE::UTIL::GetAddCommaString( nTotalGamblePrice ).c_str(), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nMoneyType ),
									DN_INTERFACE::UTIL::GetAddCommaString( nWinnerPrizeMoney ).c_str(), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nMoneyType ),
									DN_INTERFACE::UTIL::GetAddCommaString( nSecondPrizeMoney ).c_str(), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nMoneyType ) );
				}

				if( str.length() > 0 )
					pSimpleTooltTip->ShowTooltipDlg( m_pStaticGamblePrice, true, str, 0xffffffff, true );

				break;
			}
		}
		break;
	}

	return CEtUIDialog::MsgProc( hWnd, uMsg, wParam, lParam );
}

#endif // PRE_PVP_GAMBLEROOM

#endif // PRE_ADD_PVP_TOURNAMENT