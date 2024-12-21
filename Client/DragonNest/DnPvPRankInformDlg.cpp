#include "Stdafx.h"

#ifdef PRE_ADD_PVPRANK_INFORM
#include "DnUIDefine.h"
#include "DnPvPRankInformDlg.h"
#include "DnActor.h"
#include "DnLocalPlayerActor.h"
#include "DnPartyTask.h"
#include "DnPvPRankInformListItemDlg.h"
#include "DnSimpleTooltipDlg.h"
#include "DnPVPLobbyVillageTask.h"
#include "TaskManager.h"

CDnPvPRankInformDlg::CDnPvPRankInformDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback, bool bAutoCursor)
	:CEtUIDialog(dialogType, pParentDialog, nID, pCallback, bAutoCursor)
{
	m_pUserName = NULL;
	m_pUserRank = NULL;
	m_pUserExp = NULL;

	m_pUserRankIcon = NULL;
	m_pRankInfoList= NULL;

	m_ScrollStartIndex = -1;
	m_UserExpCache = 0;
	m_NextLevelExpCache = 0;
}

CDnPvPRankInformDlg::~CDnPvPRankInformDlg()
{
	m_pRankInfoList->RemoveAllItems();
}

void CDnPvPRankInformDlg::InitialUpdate()
{
	m_pUserName = GetControl<CEtUIStatic>("ID_TEXT_NAME");
	m_pUserRank = GetControl<CEtUIStatic>("ID_TEXT_COLORANK");
	m_pUserExp = GetControl<CEtUIStatic>("ID_TEXT_EXP");
	m_pUserRankIcon = GetControl<CEtUITextureControl>("ID_TEXTUREL_RANK");
	m_pRankInfoList = GetControl<CEtUIListBoxEx>("ID_LISTBOXEX_RANK");
}

void CDnPvPRankInformDlg::Initialize(bool bShow)
{
	CEtUIDialog::Initialize(CEtResourceMng::GetInstance().GetFullName("RankGradeDlg.ui").c_str(), bShow);
}

void CDnPvPRankInformDlg::UpdateExpStatus()
{
	const TPvPGroup *pPvPInfo = NULL;
	CDnPVPLobbyVillageTask *pPVPLobbyTask = static_cast<CDnPVPLobbyVillageTask*>(CTaskManager::GetInstance().GetTask("PVPLobbyTask"));
	if (pPVPLobbyTask != NULL)
	{
		pPvPInfo = &(pPVPLobbyTask->GetUserInfo().sPvPInfo);
	}
	else if (CDnPartyTask::IsActive())
	{
		pPvPInfo = CDnPartyTask::GetInstance().GetPvPInfo();
	}

	if (pPvPInfo)
	{
		m_UserExpCache = pPvPInfo->uiXP;
		DNTableFileFormat* pPvPRankSox = GetDNTable(CDnTableDB::TPVPRANK);
		if (pPvPRankSox && pPvPInfo->cLevel != 0 && pPvPInfo->cLevel < PvPCommon::Common::MaxRank)
		{
			if (pPvPRankSox->IsExistItem(pPvPInfo->cLevel))
			{
				DNTableCell* pCell = pPvPRankSox->GetFieldFromLablePtr(pPvPInfo->cLevel + 1, "RankType");
				if (pCell)
				{
					int rankType = pCell->GetInteger();
					if (IsShowExpRankType(rankType))
					{
						pCell = pPvPRankSox->GetFieldFromLablePtr(pPvPInfo->cLevel + 1, "PvPRankEXP");
						if (pCell)
							m_NextLevelExpCache = pCell->GetInteger();
					}
				}
			}
		}

		std::wstring current = AtoCommaString(m_UserExpCache);
		std::wstring whole;
		if (m_NextLevelExpCache > 0)
			whole = AtoCommaString(m_NextLevelExpCache);

		if (whole.empty())
			m_ExpTooltipCache = FormatW(L"%s", current.c_str());
		else
			m_ExpTooltipCache = FormatW(L"%s/%s", current.c_str(), whole.c_str());
	}
}

void CDnPvPRankInformDlg::SetBasicInfo()
{
	if (CDnActor::s_hLocalActor)
	{
		CDnLocalPlayerActor* pLocalActor = static_cast<CDnLocalPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
		if (pLocalActor)
		{
			m_UserInfo.userName = pLocalActor->GetName();
			m_pUserName->SetText(pLocalActor->GetName());

			m_UserInfo.userRank = pLocalActor->GetPvPLevel();
			const WCHAR* pPvPLevelName = pLocalActor->GetPvPLevelName();
			if (pPvPLevelName && pPvPLevelName[0] != '\0')
				m_pUserRank->SetText(pPvPLevelName);
			GetInterface().SetPvPClassIcon(m_pUserRankIcon, pLocalActor->GetPvPLevel());

			UpdateExpStatus();
		}
	}
	else
	{
		CDnPVPLobbyVillageTask *pPVPLobbyTask = static_cast<CDnPVPLobbyVillageTask*>(CTaskManager::GetInstance().GetTask("PVPLobbyTask"));
		if (pPVPLobbyTask == NULL)
			return;

		const SCEnter& userInfo = pPVPLobbyTask->GetUserInfo();
		m_UserInfo.userName = userInfo.wszCharacterName;
		m_pUserName->SetText(m_UserInfo.userName.c_str());

		m_UserInfo.userRank = userInfo.sPvPInfo.cLevel;

		DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TPVPRANK );
		if (pSox)
		{
			if (pSox->IsExistItem(userInfo.sPvPInfo.cLevel))
			{
				int nUIString = pSox->GetFieldFromLablePtr(userInfo.sPvPInfo.cLevel, "PvPRankUIString")->GetInteger();
				const WCHAR* pPvPLevelName = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, nUIString);
				if (pPvPLevelName && pPvPLevelName[0] != '\0')
					m_pUserRank->SetText(pPvPLevelName);
			}
		}

		GetInterface().SetPvPClassIcon(m_pUserRankIcon, userInfo.sPvPInfo.cLevel);

		UpdateExpStatus();
	}
}

bool CDnPvPRankInformDlg::IsShowExpRankType(const int& rankType) const
{
	return (rankType != 1 && rankType != 2);
}

void CDnPvPRankInformDlg::Show(bool bShow)
{
#ifdef _TEST_CODE_KAL
	m_UserInfo.Clear();
	m_RankInfos.clear();

	m_NextLevelExpCache = -1;
	m_UserExpCache = -1;
#endif

	if (bShow)
	{
		if (m_UserInfo.IsEmpty())
			SetBasicInfo();

		if (m_RankInfos.empty())
			MakeRankInfos();

		int myGradeIndex = -1;

		std::wstring percentageString;
		if (m_NextLevelExpCache > 0)
		{
			int expPercentage = int((float(m_UserExpCache) / float(m_NextLevelExpCache)) * 100.f);
			percentageString = FormatW(L"%d%%", expPercentage);
		}
		else
		{
			percentageString = AtoCommaString(m_UserExpCache);
		}

		m_pUserExp->SetText(percentageString.c_str());

		std::vector<PvPRankInfoUIDef::SRankUnit>::const_iterator iter = m_RankInfos.begin();
		int i = 0;
		for (; iter != m_RankInfos.end(); ++iter)
		{
			const PvPRankInfoUIDef::SRankUnit& curUnit = (*iter);
			CDnPvPRankInformListItemDlg* pListItemDlg = m_pRankInfoList->AddItem<CDnPvPRankInformListItemDlg>();
			if (pListItemDlg)
			{
				pListItemDlg->SetData(curUnit);

				if (m_UserInfo.userRank == curUnit.level)
					myGradeIndex = i;
				++i;
			}
		}

		if (m_pRankInfoList->GetSize() > 0)
		{
			if (m_ScrollStartIndex < 0)
			{
				CDnPvPRankInformListItemDlg* pListItemDlg = m_pRankInfoList->GetItem<CDnPvPRankInformListItemDlg>(0);
				if (pListItemDlg)
				{
					float fListItemHeight = pListItemDlg->GetCurrentListUIHeight();
					if (fListItemHeight > 0)
					{
						const SUICoord& listUICoord = m_pRankInfoList->GetBaseUICoord();
						int itemCountInPage = int(listUICoord.fHeight / fListItemHeight);
						m_ScrollStartIndex = int(itemCountInPage * 0.5f);
					}
				}
			}

			CEtUIScrollBar* pScrollBar = m_pRankInfoList->GetScrollBar();
			if (pScrollBar && myGradeIndex >= 0)
			{
				if (myGradeIndex < m_ScrollStartIndex)
					myGradeIndex = 0;
				else
					myGradeIndex -= m_ScrollStartIndex;

				pScrollBar->SetTrackPos(myGradeIndex);
			}
		}
	}
	else
	{
		m_pRankInfoList->RemoveAllItems();
	}

	CEtUIDialog::Show(bShow);
}

void CDnPvPRankInformDlg::Clear()
{
	m_UserInfo.Clear();
	m_RankInfos.clear();

	m_NextLevelExpCache = -1;
	m_UserExpCache = -1;
}

void CDnPvPRankInformDlg::MakeRankInfos()
{
	DNTableFileFormat* pPvPRankSox = GetDNTable(CDnTableDB::TPVPRANK);
	if (pPvPRankSox == NULL)
		return;
	DNTableFileFormat* pPvPRewardSox = GetDNTable(CDnTableDB::TPVPRANKREWARD);
	if (pPvPRewardSox == NULL)
		return;

	PvPRankInfoUIDef::SRankUnit unit;
	int i = 0;
	int count = pPvPRankSox->GetItemCount();
	for (; i < count; ++i)
	{
		unit.Clear();

		int tableID = pPvPRankSox->GetItemID(i);
		DNTableCell* pCell = pPvPRankSox->GetFieldFromLablePtr(tableID, "PvPRank");
		if (pCell == NULL)
			continue;
		unit.level = pCell->GetInteger();

		if (m_UserInfo.userRank > unit.level)
		{
			unit.lineColor = 0xFF7b7D7A;
		}
		else if (m_UserInfo.userRank == unit.level)
		{
			unit.lineColor = 0xFFEA993D;
		}
		else
		{
			unit.lineColor = 0xFFFFFFFF;
			if (i == count - 1)
				unit.lineColor = 0xFFFF0000;
		}

		pCell = pPvPRankSox->GetFieldFromLablePtr(tableID, "PvPRankUIString");
		if (pCell == NULL)
			continue;
		unit.name = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, pCell->GetInteger());

		pCell = pPvPRankSox->GetFieldFromLablePtr(tableID, "RankType");
		if (pCell == NULL)
			continue;
		int rankType = pCell->GetInteger();
		if (IsShowExpRankType(rankType))
		{
			int nextRankExpIndex = i - 1;

			if (nextRankExpIndex >= 0)
			{
				int nextRankExpTableID = pPvPRankSox->GetItemID(nextRankExpIndex);
				pCell = pPvPRankSox->GetFieldFromLablePtr(nextRankExpTableID, "PvPRankEXP");
				if (pCell != NULL)
					unit.needExp = pCell->GetInteger();
			}
		}

		pCell = pPvPRewardSox->GetFieldFromLablePtr(unit.level, "_PVPRankID");
		if (pCell == NULL)
			continue;

		int j = 0;
		PvPRankInfoUIDef::SRankRewardItemUnit rewardItemUnit;
		for (; j < PvPRankInfoUIDef::eMAX_REWARD_ITEM_COUNT; ++j)
		{
			std::string str = FormatA("_RewardItemID%d", j + 1);
			pCell = pPvPRewardSox->GetFieldFromLablePtr(unit.level, str.c_str());
			if (pCell == NULL)
				continue;
			rewardItemUnit.id = pCell->GetInteger();

			str = FormatA("_Count%d", j + 1);
			pCell = pPvPRewardSox->GetFieldFromLablePtr(unit.level, str.c_str());
			if (pCell == NULL)
				continue;
			rewardItemUnit.count = pCell->GetInteger();

			unit.rewards.push_back(rewardItemUnit);
		}

		m_RankInfos.push_back(unit);
	}
}

void CDnPvPRankInformDlg::ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg)
{
	SetCmdControlName(pControl->GetControlName());

	if (nCommand == EVENT_BUTTON_CLICKED)
	{
		if (IsCmdControl("ID_BT_CLOSE") || IsCmdControl("ID_BT_CANCEL"))
			Show(false);
		return;
	}

	CEtUIDialog::ProcessCommand(nCommand, bTriggeredByUser, pControl, uMsg);
}

bool CDnPvPRankInformDlg::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if( !IsShow() )
		return false;

	bool bRet;
	bRet = CEtUIDialog::MsgProc( hWnd, uMsg, wParam, lParam );

	switch( uMsg )
	{
	case WM_MOUSEMOVE:
		{
			float fMouseX, fMouseY;
			POINT MousePoint;
			MousePoint.x = short( LOWORD( lParam ) );
			MousePoint.y = short( HIWORD( lParam ) );
			PointToFloat( MousePoint, fMouseX, fMouseY );

			if (m_pRankInfoList->IsInside(fMouseX, fMouseY))
			{
				if (m_pRankInfoList->IsFocus() == false)
					RequestFocus(m_pRankInfoList);
			}
			else
			{
				if (m_pRankInfoList->IsFocus())
					focus::ReleaseControl();

				CDnSimpleTooltipDlg* pSimpleTooltipDlg = GetInterface().GetSimpleTooltipDialog();
				if (pSimpleTooltipDlg && m_pUserExp)
				{
					SUICoord uiCoord;
					m_pUserExp->GetUICoord(uiCoord);

					if (uiCoord.IsInside(fMouseX, fMouseY))
					{
						pSimpleTooltipDlg->ShowTooltipDlg(m_pUserExp, true, m_ExpTooltipCache, 0xffffffff, true);
					}
				}
			}
		}
		break;
	}

	return bRet;
}

#endif // PRE_ADD_PVPRANK_INFORM