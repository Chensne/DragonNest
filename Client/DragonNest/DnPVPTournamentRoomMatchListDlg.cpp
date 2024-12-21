#include "stdafx.h"
#include "DnPVPTournamentRoomMatchListDlg.h"
#include "DnTableDB.h"
#include "DnInterface.h"
#include "PvPSendPacket.h"
#include "TaskManager.h"
#include "DnPVPLobbyChatTabDlg.h"
#include "DnPVPLobbyVillageTask.h"
#include "DnPVPTournamentDataMgr.h"
#include "DnPVPTournamentRoomDlg.h"
#include "PvPSendPacket.h"

#ifdef PRE_ADD_PVP_TOURNAMENT

void CDnPVPTournamentRoomMatchListDlg::SMatchListUI::SetCtrls(bool bLeftSide, int index, CEtUIDialog& parent)
{
	if (index < 0)
	{
		_ASSERT(0);
		return;
	}

	std::string ctrlName = FormatA("ID_TEXT_%sNAME%d", bLeftSide ? "LEFT" : "RIGHT", index);
	pNameStatic = parent.GetControl<CEtUIStatic>(ctrlName.c_str());

	ctrlName = FormatA("ID_STATIC_%sSELECT%d", bLeftSide ? "LEFT" : "RIGHT", index);
	pSelectStatic = parent.GetControl<CEtUIStatic>(ctrlName.c_str());

	ctrlName = FormatA("ID_STATIC_%sMASTER%d", bLeftSide ? "LEFT" : "RIGHT", index);
	pLeader = parent.GetControl<CEtUIStatic>(ctrlName.c_str());

	ctrlName = FormatA("ID_STATIC_%sREADY%d", bLeftSide ? "LEFT" : "RIGHT", index);
	pReady = parent.GetControl<CEtUIStatic>(ctrlName.c_str());

	ctrlName = FormatA("ID_STATIC_%sINGAME%d", bLeftSide ? "LEFT" : "RIGHT", index);
	pInGame = parent.GetControl<CEtUIStatic>(ctrlName.c_str());
}

void CDnPVPTournamentRoomMatchListDlg::SMatchListUI::ShowAll(bool bShow)
{
	pNameStatic->Show(bShow);
	pSelectStatic->Show(bShow);
	pLeader->Show(bShow);
	pInGame->Show(bShow);
	pReady->Show(bShow);
}

bool CDnPVPTournamentRoomMatchListDlg::SMatchListUI::IsEmpty() const
{
	if (pNameStatic)
	{
		const std::wstring& name = pNameStatic->GetText();
		return name.empty();
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////

CDnPVPTournamentRoomMatchListDlg::CDnPVPTournamentRoomMatchListDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback, bool bAutoCursor)
	: CDnCustomDlg(dialogType, pParentDialog, nID, pCallback, bAutoCursor)
{
	int i = 0;
	for (; i < eMatchModeByCount::eMODE_MAX; ++i)
	{
		m_pMatchListBase[i] = NULL;
	}

	m_CurrentModeCache = eMatchModeByCount::eMODE_MAX;
	m_pBaseDlg = NULL;
	m_CurrentSelectStaticIndex = -1;
}

CDnPVPTournamentRoomMatchListDlg::~CDnPVPTournamentRoomMatchListDlg(void)
{
}

void CDnPVPTournamentRoomMatchListDlg::InitialUpdate()
{
	std::string matchListBaseCtrlName;
	int i = 0;
	for (; i < eMatchModeByCount::eMODE_MAX; ++i)
	{
		int memberCount = CDnPVPTournamentDataMgr::GetMemberCountOfMode((eMatchModeByCount)i);
		matchListBaseCtrlName = FormatA("ID_STATIC_MATCH%d", memberCount);
		m_pMatchListBase[i] = GetControl<CEtUIStatic>(matchListBaseCtrlName.c_str());
	}

	if (m_pMatchListUI.empty() == false)
	{
		_ASSERT(0);
		return;
	}

	for (i = 0; i < MAX_MATCH_MEMBERCOUNT; ++i)
	{
		int ctrlListIndex = i;
		SMatchListUI unit;
		bool bLeftSide = true;
		if (i >= MAX_MATCH_SIDE_COUNT)
		{
			ctrlListIndex -= MAX_MATCH_SIDE_COUNT;
			bLeftSide = false;
		}

		unit.SetCtrls(bLeftSide, ctrlListIndex, *this);
		unit.ShowAll(false);
		m_pMatchListUI.push_back(unit);
	}
}

void CDnPVPTournamentRoomMatchListDlg::Initialize(bool bShow)
{
	CDnCustomDlg::Initialize(CEtResourceMng::GetInstance().GetFullName("PvpTournamentMatchDlg.ui").c_str(), bShow);
}

void CDnPVPTournamentRoomMatchListDlg::ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg)
{
	CDnCustomDlg::ProcessCommand(nCommand, bTriggeredByUser, pControl, uMsg);
}

void CDnPVPTournamentRoomMatchListDlg::Process(float fElapsedTime)
{
	CDnCustomDlg::Process(fElapsedTime);
}

void CDnPVPTournamentRoomMatchListDlg::ActivateMode(eMatchModeByCount modeType)
{
	if (modeType < 0 || modeType >= eMatchModeByCount::eMODE_MAX)
	{
		_ASSERT(0);
		return;
	}

	Reset();

	if (m_pMatchListUI.empty() == false)
	{
		CDnPVPTournamentDataMgr* pDataMgr = GetDataMgr();
		if (pDataMgr)
			pDataMgr->MakeMatchListIndexerByMode(modeType);
	}

	m_pMatchListBase[modeType]->Show(true);

	m_CurrentModeCache = modeType;
}

void CDnPVPTournamentRoomMatchListDlg::Show(bool bShow)
{
	if (bShow == m_bShow)
		return;

	CDnCustomDlg::Show(bShow);
}

void CDnPVPTournamentRoomMatchListDlg::Reset()
{
	int i = 0;
	for (; i < eMatchModeByCount::eMODE_MAX; ++i)
	{
		m_pMatchListBase[i]->Show(false);
	}

	for (i = 0; i < (int)m_pMatchListUI.size(); ++i)
	{
		SMatchListUI& curUI = m_pMatchListUI[i];
		curUI.pLeader->Show(false);
		curUI.pReady->Show(false);
		curUI.pInGame->Show(false);

		curUI.pNameStatic->ClearText();
		curUI.pSelectStatic->Show(false);
	}

	m_CurrentSelectStaticIndex = -1;
}

CDnPVPTournamentDataMgr* CDnPVPTournamentRoomMatchListDlg::GetDataMgr() const
{
	if (CDnBridgeTask::IsActive() == false)
		return NULL;

	return &(CDnBridgeTask::GetInstance().GetPVPTournamentDataMgr());
}

void CDnPVPTournamentRoomMatchListDlg::UpdateMatchListUI(const SMatchUserInfo& info)
{
	if (info.matchListUIIndex < 0 || info.matchListUIIndex >= (int)m_pMatchListUI.size())
	{
		_ASSERT(0);
		return;
	}

	const SMatchListUI& curListUI = m_pMatchListUI[info.matchListUIIndex];
	bool bIngame = CDnBaseRoomDlg::IsInGame(info.uiUserState);
	bool bMaster = CDnBaseRoomDlg::IsMaster(info.uiUserState);
	curListUI.pInGame->Show(bIngame);
	curListUI.pLeader->Show(bIngame == false && bMaster);
	curListUI.pReady->Show(bIngame == false && bMaster == false && CDnBaseRoomDlg::IsReady(info.uiUserState));
	curListUI.pNameStatic->SetText(info.playerName.c_str());
	curListUI.pNameStatic->Show(true);
	curListUI.pSelectStatic->Show(false);
}

void CDnPVPTournamentRoomMatchListDlg::ResetMatchListUI()
{
	int i = 0;
	for (i = 0; i < (int)m_pMatchListUI.size(); ++i)
	{
		SMatchListUI& curUI = m_pMatchListUI[i];
		curUI.pLeader->Show(false);
		curUI.pReady->Show(false);
		curUI.pInGame->Show(false);
		curUI.pNameStatic->ClearText();
		curUI.pSelectStatic->Show(false);
	}
}

float CDnPVPTournamentRoomMatchListDlg::GetCurrentListUIHeight() const
{
	if (m_CurrentModeCache != eMODE_MAX)
	{
		CEtUIStatic* pCurBase = m_pMatchListBase[m_CurrentModeCache];
		if (pCurBase)
			return pCurBase->GetBaseUICoord().fHeight;
	}

	return -1.f;
}

bool CDnPVPTournamentRoomMatchListDlg::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (m_pBaseDlg && m_pBaseDlg->IsMouseInDlg() == false)
		return false;

	switch(uMsg)
	{
	case WM_LBUTTONDOWN:
		{
			POINT MousePoint;
			float fMouseX, fMouseY;

			MousePoint.x = short( LOWORD( lParam ) );
			MousePoint.y = short( HIWORD( lParam ) );
			PointToFloat( MousePoint, fMouseX, fMouseY );

			if (m_pBaseDlg == NULL)
				return false;

			CDnPVPTournamentDataMgr* pDataMgr = GetDataMgr();
			if (pDataMgr == NULL)
				return false;

			if (pDataMgr->AmIMaster() == false)
				return false;

 			int i = 0;
 			for (; i < (int)m_pMatchListUI.size(); ++i)
 			{
				const SMatchListUI& elem = m_pMatchListUI[i];
				if (elem.pSelectStatic && elem.pSelectStatic->IsInside(fMouseX, fMouseY))
				{
					if (elem.IsEmpty() == false)
					{
						if (m_CurrentSelectStaticIndex == i)
						{
							elem.pSelectStatic->Show(false);
							m_CurrentSelectStaticIndex = -1;
						}
						else
						{
							if (m_CurrentSelectStaticIndex >= 0)
							{
								if (pDataMgr->IsStateEditPosition())
								{
									elem.pSelectStatic->Show(true);
									SendToServerSwapUserPosition(i);
								}
								else
								{
									if (m_CurrentSelectStaticIndex >= (int)m_pMatchListUI.size())
									{
										ResetSelect();
									}
									else
									{
										SMatchListUI& curMatch = m_pMatchListUI[m_CurrentSelectStaticIndex];
										if (curMatch.pSelectStatic)
											curMatch.pSelectStatic->Show(false);
										elem.pSelectStatic->Show(true);
										m_CurrentSelectStaticIndex = i;
									}
								}
							}
							else
							{
								elem.pSelectStatic->Show(true);
								m_CurrentSelectStaticIndex = i;
							}
						}
						break;
					}
				}
 			}
		}
		break;
	}

	return CEtUIDialog::MsgProc(hWnd, uMsg, wParam, lParam);
}

void CDnPVPTournamentRoomMatchListDlg::SendToServerSwapUserPosition(int uiIndex)
{
	if (m_CurrentSelectStaticIndex < 0)
		return;

	CDnPVPTournamentDataMgr* pDataMgr = GetDataMgr();
	if (pDataMgr == NULL)
		return;

	const SMatchUserInfo* pDestInfo = pDataMgr->GetSlotInfoByUIIndex(uiIndex);
	if (pDestInfo == NULL)
		return;

	const SMatchUserInfo* pSrcInfo = pDataMgr->GetSlotInfoByUIIndex(m_CurrentSelectStaticIndex);
	if (pSrcInfo == NULL)
		return;

	pDataMgr->HoldSwapPosition(true);

	SendPvPTournamentSwapPosition(pSrcInfo->commonIndex, pDestInfo->commonIndex);
}

void CDnPVPTournamentRoomMatchListDlg::SetBaseDlg(CDnPVPTournamentRoomDlg* pDlg)
{
	m_pBaseDlg = pDlg;
}

void CDnPVPTournamentRoomMatchListDlg::ResetSelect()
{
	int i = 0;
	for (i = 0; i < (int)m_pMatchListUI.size(); ++i)
	{
		SMatchListUI& curUI = m_pMatchListUI[i];
		curUI.pSelectStatic->Show(false);
	}

	m_CurrentSelectStaticIndex = -1;
}

#endif // PRE_ADD_PVP_TOURNAMENT