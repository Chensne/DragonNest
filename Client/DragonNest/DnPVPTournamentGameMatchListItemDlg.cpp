#include "stdafx.h"
#include "DnPVPTournamentGameMatchListItemDlg.h"
#include "DnPVPTournamentGameMatchListDlg.h"
#include "DnTableDB.h"
#include "DnInterface.h"
#include "TaskManager.h"
#include "DnPVPTournamentDataMgr.h"

#ifdef PRE_ADD_PVP_TOURNAMENT

void CDnPVPTournamentGameMatchListItemDlg::SGameMatchListUI::SetCtrls(bool bLeftSide, int index, CEtUIDialog& parent)
{
	if (index < 0)
	{
		_ASSERT(0);
		return;
	}

	(*this).bLeftSide = bLeftSide;
	std::string leftOrRight = bLeftSide ? "LEFT" : "RIGHT";
	std::string ctrlName = FormatA("ID_%s_LISTON%d", leftOrRight.c_str(), index);
	pSelectStatic = parent.GetControl<CEtUIStatic>(ctrlName.c_str());

	ctrlName = FormatA("ID_TEXT_%s_NAME%d", leftOrRight.c_str(), index);
	pNameStatic = parent.GetControl<CEtUIStatic>(ctrlName.c_str());

	ctrlName = FormatA("ID_%s_MASTER%d", leftOrRight.c_str(), index);
	pLeader = parent.GetControl<CEtUIStatic>(ctrlName.c_str());
	pLeader->Show(false);

	ctrlName = FormatA("ID_%s_CLASS%d", leftOrRight.c_str(), index);
	pJobIcon = parent.GetControl<CDnJobIconStatic>(ctrlName.c_str());

	ctrlName = FormatA("ID_%s_PLAYLIST%d", leftOrRight.c_str(), index);
	pCurrentUserStatic = parent.GetControl<CEtUIStatic>(ctrlName.c_str());
}

void CDnPVPTournamentGameMatchListItemDlg::SGameMatchListUI::ShowAll(bool bShow)
{
	pNameStatic->Show(bShow);
	pSelectStatic->Show(bShow);
	pLeader->Show(false);
	pJobIcon->Show(bShow);
	pCurrentUserStatic->Show(bShow);
}

bool CDnPVPTournamentGameMatchListItemDlg::SGameMatchListUI::IsEmpty() const
{
	if (pNameStatic)
	{
		const std::wstring& name = pNameStatic->GetText();
		return name.empty();
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////

CDnPVPTournamentGameMatchListItemDlg::CDnPVPTournamentGameMatchListItemDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback, bool bAutoCursor)
	: CDnCustomDlg(dialogType, pParentDialog, nID, pCallback, true)
{
	m_pBaseDlg = NULL;
	m_pBackground = NULL;
	m_pTrophySelect = NULL;
	m_pLeftFinalRoundWinLine = NULL;
	m_pRightFinalRoundWinLine = NULL;
}

CDnPVPTournamentGameMatchListItemDlg::~CDnPVPTournamentGameMatchListItemDlg()
{
}

void CDnPVPTournamentGameMatchListItemDlg::InitialUpdate()
{
	const int maxUserCount = GetMaxUserCount();
	const int halfUserCount = maxUserCount / 2;

	int i = 0;
	std::string ctrlName;
	for (; i < maxUserCount; ++i)
	{
		int index = i;
		bool bLeft = true;
		if (halfUserCount <= i)
		{
			bLeft = false;
			index = i - halfUserCount;
		}

		SGameMatchListUI temp;
		temp.SetCtrls(bLeft, index, (*this));
		m_pMatchListUI.push_back(temp);
	}

	const int maxRoundCount = GetMaxRoundCount();
	const int halfRoundCount = maxRoundCount / 2;
	int lineCount = maxUserCount;
	int halfLineCount = lineCount / 2;

	for (i = 0; i < maxRoundCount; ++i)
	{
		std::vector<CEtUIStatic*> leftUnit;
		std::vector<CEtUIStatic*> rightUnit;

		int j = 0;
		for (; j < lineCount; ++j)
		{
			bool bLeft = true;
			int index = j;
			if (halfLineCount <= j)
			{
				bLeft = false;
				index = j - halfLineCount;
			}

			std::string ctrlName;
			if (i == 0)
				ctrlName = FormatA("ID_LINE_%s_%d", bLeft ? "L" : "R", index + 1);
			else
				ctrlName = FormatA("ID_LINE_%s%d_%d", bLeft ? "L" : "R", i + 1, index + 1);
			CEtUIStatic* curStatic = GetControl<CEtUIStatic>(ctrlName.c_str());
			if (bLeft)
				leftUnit.push_back(curStatic);
			else
				rightUnit.push_back(curStatic);
		}

		m_LeftAdvanceLine.push_back(leftUnit);
		m_RightAdvanceLine.push_back(rightUnit);

		lineCount /= 2;
		halfLineCount = lineCount / 2;
	}

	m_pLeftFinalRoundWinLine = GetControl<CEtUIStatic>("ID_LINE_L_WIN");
	m_pRightFinalRoundWinLine = GetControl<CEtUIStatic>("ID_LINE_R_WIN");
	m_pLeftFinalRoundWinLine->Show(false);
	m_pRightFinalRoundWinLine->Show(false);

	m_pBackground = GetControl<CEtUIStatic>("ID_STATIC_SIZE");
	m_pTrophySelect = GetControl<CEtUIStatic>("ID_LINE_TROPHY");
#ifdef PRE_ADD_PVP_TOURNAMENT_ADDITIONAL
	m_pTrophySelect->Show(false);
#else
	m_pTrophySelect->Show(true);
#endif
}

#ifdef _TEST_CODE_KAL
void CDnPVPTournamentGameMatchListItemDlg::Show(bool bShow)
{
	if (m_bShow == bShow)
		return;

	Update();

	CDnCustomDlg::Show(bShow);
}
#endif

void CDnPVPTournamentGameMatchListItemDlg::Reset()
{
	int i = 0;
	for (; i < (int)m_pMatchListUI.size(); ++i)
	{
		SGameMatchListUI& matchUI = m_pMatchListUI[i];
		matchUI.ShowAll(false);
	}

	int leftLineCount = (int)m_LeftAdvanceLine.size();
	int rightLineCount = (int)m_RightAdvanceLine.size();
	if (leftLineCount != rightLineCount)
	{
		_ASSERT(0);
		return;
	}

	for (i = 0; i < leftLineCount; ++i)
	{
		std::vector<CEtUIStatic*>& innerLeft = m_LeftAdvanceLine[i];
		std::vector<CEtUIStatic*>& innerRight = m_RightAdvanceLine[i];
		int j = 0;
		for (; j < (int)innerLeft.size(); ++j)
		{
			CEtUIStatic* pCurLeftStatic = innerLeft[j];
			if (pCurLeftStatic)
				pCurLeftStatic->Show(false);

			CEtUIStatic* pCurRightStatic = innerRight[j];
			if (pCurRightStatic)
				pCurRightStatic->Show(false);
		}
	}

	m_pLeftFinalRoundWinLine->Show(false);
	m_pRightFinalRoundWinLine->Show(false);
}

void CDnPVPTournamentGameMatchListItemDlg::Update()
{
	CDnPVPTournamentDataMgr* pMgr = GetDataMgr();
	if( pMgr == NULL )
	{
		_ASSERT(0);
		return;
	}

	Reset();

	const std::vector<SGameMatchUserInfo>& gameMatchUserList = pMgr->GetGameMatchUserList();
#ifndef _WORK
	if ((int)gameMatchUserList.size() > GetMaxUserCount())
		return;
#endif

#ifdef PRE_WORLDCOMBINE_PVP
	if( pMgr->GetRoomInfo().nWorldPvPRoomType && static_cast<int>( gameMatchUserList.size() ) == 0 )
	{
		std::vector<SFinalReportUserInfo>& finalReportUserList = pMgr->GetFinalReportUserList();
		int maxUserCount = static_cast<int>( finalReportUserList.size() );

		for( int i=0; i<maxUserCount; ++i )
		{
			SFinalReportUserInfo& info = finalReportUserList[i];
			if( i >= static_cast<int>( m_pMatchListUI.size() ) )
				break;

			SGameMatchListUI& curUserUI = m_pMatchListUI[i];
			curUserUI.pNameStatic->SetText( info.wszUserName );
			curUserUI.pNameStatic->Show( true );

			curUserUI.pJobIcon->SetIconID( info.cJobClassID );
			curUserUI.pJobIcon->Show( true );
		}
		return;
	}
#endif // PRE_WORLDCOMBINE_PVP

	const int maxUserCount = (int)gameMatchUserList.size();
	const int halfModeUserCount = GetMaxUserCount() / 2;
	int i = 0;
	for (; i < maxUserCount; ++i)
	{
		const SGameMatchUserInfo& info = gameMatchUserList[i];
		if (i >= (int)m_pMatchListUI.size())
			break;

		SGameMatchListUI& curUserUI = m_pMatchListUI[i];
		curUserUI.pNameStatic->SetText(info.charName.c_str());
		curUserUI.pJobIcon->SetIconID(info.jobId);
		curUserUI.pCurrentUserStatic->Show(info.bCurrentMatchUser);
#ifdef PRE_ADD_PVP_TOURNAMENT_ADDITIONAL
		curUserUI.pSelectStatic->Show(true);
#else
		curUserUI.pSelectStatic->Show(false);
#endif
		curUserUI.pNameStatic->Show(true);
		curUserUI.pJobIcon->Show(true);

		int curStep = info.step;

		int j = 1;
		for (; j <= curStep; ++j)
		{
			int index = j - 1;
			if (j == curStep && info.bWin == false)
				break;

			if (index == m_LeftAdvanceLine.size() || index == m_RightAdvanceLine.size())
			{
				if (curUserUI.bLeftSide == false)
					m_pRightFinalRoundWinLine->Show(true);
				else
					m_pLeftFinalRoundWinLine->Show(true);
#ifdef PRE_ADD_PVP_TOURNAMENT_ADDITIONAL
				m_pTrophySelect->Show(true);
#endif

				continue;
			}
			else if ((int)m_LeftAdvanceLine.size() < index || (int)m_RightAdvanceLine.size() < index)
			{
				break;
			}

			int tempIndex = i;
			if (curUserUI.bLeftSide == false)
			{
				if (tempIndex < halfModeUserCount)
				{
					_ASSERT(0);
					continue;
				}

				tempIndex -= halfModeUserCount;
			}

			int terminalIndex = tempIndex / (int)std::pow((double)2, j - 1);

			int lineListSize = curUserUI.bLeftSide ? (int)m_LeftAdvanceLine[index].size() : (int)m_RightAdvanceLine[index].size();
			if (terminalIndex >= lineListSize)
				continue;

			CEtUIStatic* pCurLine = curUserUI.bLeftSide ? m_LeftAdvanceLine[index][terminalIndex] : m_RightAdvanceLine[index][terminalIndex];
			if (pCurLine)
				pCurLine->Show(true);
		}
	}

	for (i = 0; i < maxUserCount; ++i)
	{
		const SGameMatchUserInfo& info = gameMatchUserList[i];
		if (i >= (int)m_pMatchListUI.size())
			break;

		SGameMatchListUI& curUserUI = m_pMatchListUI[i];
		if (info.bWin)
		{
			curUserUI.pNameStatic->SetTextColor(textcolor::WHITE);
#ifdef PRE_ADD_PVP_TOURNAMENT_ADDITIONAL
#else
			curUserUI.pSelectStatic->Show(true);
#endif
		}
		else
		{
			DWORD textColor = textcolor::DARKGRAY;
			if (info.step <= 0)
				textColor = textcolor::WHITE;
#ifdef PRE_ADD_PVP_TOURNAMENT_ADDITIONAL
			else
				curUserUI.pSelectStatic->Show(false);
#endif
			curUserUI.pNameStatic->SetTextColor(textColor);
		}
	}
}

CDnPVPTournamentDataMgr* CDnPVPTournamentGameMatchListItemDlg::GetDataMgr() const
{
	if (CDnBridgeTask::IsActive() == false)
		return NULL;

	return &(CDnBridgeTask::GetInstance().GetPVPTournamentDataMgr());
}

float CDnPVPTournamentGameMatchListItemDlg::GetCurrentListUIHeight() const
{
	if (m_pBackground)
		return m_pBackground->GetBaseUICoord().fHeight;

	return -1.f;
}

//////////////////////////////////////////////////////////////////////////

CDnPVPTournamentGameMatchListItemDlg_4User::CDnPVPTournamentGameMatchListItemDlg_4User(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback)
	:CDnPVPTournamentGameMatchListItemDlg(dialogType, pParentDialog, nID, pCallback, true)
{
}

void CDnPVPTournamentGameMatchListItemDlg_4User::Initialize(bool bShow)
{
	CDnCustomDlg::Initialize(CEtResourceMng::GetInstance().GetFullName("PvpTournamentMatch4TreeDlg.ui").c_str(), bShow);
}

//////////////////////////////////////////////////////////////////////////

CDnPVPTournamentGameMatchListItemDlg_8User::CDnPVPTournamentGameMatchListItemDlg_8User(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback)
	:CDnPVPTournamentGameMatchListItemDlg(dialogType, pParentDialog, nID, pCallback, true)
{
}

void CDnPVPTournamentGameMatchListItemDlg_8User::Initialize(bool bShow)
{
	CDnCustomDlg::Initialize(CEtResourceMng::GetInstance().GetFullName("PvpTournamentMatch8TreeDlg.ui").c_str(), bShow);
}

//////////////////////////////////////////////////////////////////////////

CDnPVPTournamentGameMatchListItemDlg_16User::CDnPVPTournamentGameMatchListItemDlg_16User(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback)
	:CDnPVPTournamentGameMatchListItemDlg(dialogType, pParentDialog, nID, pCallback, true)
{
}

void CDnPVPTournamentGameMatchListItemDlg_16User::Initialize(bool bShow)
{
	CDnCustomDlg::Initialize(CEtResourceMng::GetInstance().GetFullName("PvpTournamentMatch16TreeDlg.ui").c_str(), bShow);
}

//////////////////////////////////////////////////////////////////////////
CDnPVPTournamentGameMatchListItemDlg_32User::CDnPVPTournamentGameMatchListItemDlg_32User(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback)
	:CDnPVPTournamentGameMatchListItemDlg(dialogType, pParentDialog, nID, pCallback, true)
{
}

void CDnPVPTournamentGameMatchListItemDlg_32User::Initialize(bool bShow)
{
	CDnCustomDlg::Initialize(CEtResourceMng::GetInstance().GetFullName("PvpTournamentMatch32TreeDlg.ui").c_str(), bShow);
}

#endif // PRE_ADD_PVP_TOURNAMENT