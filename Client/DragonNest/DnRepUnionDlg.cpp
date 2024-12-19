#include "StdAfx.h"
#include "DnRepUnionDlg.h"
#include "DnTableDB.h"
#include "DnQuestTask.h"
#include "TaskManager.h"
#include "DnCommonUtil.h"
#include "DnNpcReputationTabDlg.h"
#include "DnMainMenuDlg.h"
#include "DnIntegrateQuestNReputationDlg.h"

#define NPC_UNION_EMBLEM_UNIT_WIDTH				90
#define NPC_UNION_EMBLEM_UNIT_HEIGHT			80
#define NPC_UNION_EMBLEM_UNIT_COUNT_PER_LINE	4
#define NPC_FACE_TEXTURE_SIZE					64

void CDnRepUnionDlg::SUnionUnitUI::Reset()
{
	pName->ClearText();
	pPoint->ClearText();

	ShowAll(false);
}

void CDnRepUnionDlg::SUnionUnitUI::SetName(const std::wstring& name)
{
	pName->SetText(name.c_str());
}

void CDnRepUnionDlg::SUnionUnitUI::SetEmblem(EtTextureHandle hEmblem, int iconIdx)
{
	pEmblem->SetTexture(hEmblem, (iconIdx % NPC_UNION_EMBLEM_UNIT_COUNT_PER_LINE) * NPC_UNION_EMBLEM_UNIT_WIDTH, (iconIdx / NPC_UNION_EMBLEM_UNIT_COUNT_PER_LINE) * NPC_UNION_EMBLEM_UNIT_HEIGHT,
						NPC_UNION_EMBLEM_UNIT_WIDTH, NPC_UNION_EMBLEM_UNIT_HEIGHT);
}

void CDnRepUnionDlg::SUnionUnitUI::ShowSelect(bool bShow)
{
	pSelect->Show(bShow);
}

void CDnRepUnionDlg::SUnionUnitUI::SetPoint(UNIONPT_TYPE point)
{
	pPoint->SetInt64ToText(point);
}

void CDnRepUnionDlg::SUnionUnitUI::ShowPoint(bool bShow)
{
	pPoint->Show(bShow);
	pPointText->Show(bShow);
}

bool CDnRepUnionDlg::SUnionUnitUI::IsEmpty() const
{
	const std::wstring& str = pName->GetText();
	return (str.empty());
}

void CDnRepUnionDlg::SUnionUnitUI::ShowAll(bool bShow)
{
	pName->Show(bShow);
	pEmblem->Show(bShow);
	ShowPoint(bShow);
}

//////////////////////////////////////////////////////////////////////////
void CDnRepUnionDlg::SUnionMemberUI::ShowAll(bool bShow)
{
	pBase->Show(bShow);

	int i = 0;
	for (; i < NPC_UNION_MEMBER_COUNT_PER_PAGE; ++i)
	{
		pFace[i]->Show(bShow);
		pFavorValue[i]->Show(bShow);
	}
}

void CDnRepUnionDlg::SUnionMemberUI::ShowNpc(bool bShow, int idx)
{
	if (idx >= NPC_UNION_MEMBER_COUNT_PER_PAGE)
		return;

	pFace[idx]->Show(bShow);
	pFavorValue[idx]->Show(bShow);
}

void CDnRepUnionDlg::SUnionMemberUI::SetFace(EtTextureHandle hFace, int iconIdx, int arrayIndex)
{
	if (arrayIndex >= NPC_UNION_MEMBER_COUNT_PER_PAGE)
	{
		_ASSERT(0);
		return;
	}

	int faceCount = hFace->Width() / NPC_FACE_TEXTURE_SIZE;
	pFace[arrayIndex]->SetTexture(hFace, (iconIdx % faceCount) * NPC_FACE_TEXTURE_SIZE, (iconIdx / faceCount) * NPC_FACE_TEXTURE_SIZE,
								NPC_FACE_TEXTURE_SIZE, NPC_FACE_TEXTURE_SIZE);
}

bool CDnRepUnionDlg::SUnionMemberUI::IsShow() const
{
	return pBase->IsShow();
}

int CDnRepUnionDlg::SUnionMemberUI::GetNpcID(const std::string& ctrlName) const
{
	int i = 0;
	for (; i < NPC_UNION_MEMBER_COUNT_PER_PAGE; ++i)
	{
		if (ctrlName.compare(pFace[i]->GetControlName()) == 0)
			return npcID[i];
	}

	return -1;
}

int CDnRepUnionDlg::SUnionMemberUI::GetNpcIDByPos(float fX, float fY) const
{
	int i = 0;
	for (; i < NPC_UNION_MEMBER_COUNT_PER_PAGE; ++i)
	{
		if (pFace[i]->IsShow() && pFace[i]->IsInside(fX, fY))
			return npcID[i];
	}

	return -1;
}

//////////////////////////////////////////////////////////////////////////

CDnRepUnionDlg::CDnRepUnionDlg(UI_DIALOG_TYPE dialogType /* = UI_TYPE_FOCUS */, CEtUIDialog *pParentDialog /* = NULL */, 
													int nID /* = -1 */, CEtUICallback *pCallback /* = NULL */) :
													CEtUIDialog(dialogType, pParentDialog, nID, pCallback, true),
													m_pUnionExplainText(NULL),
													m_pUnionGroupPageCount(NULL),
													m_pUnionGroupBtnPrevPage(NULL),
													m_pUnionGroupBtnNextPage(NULL),
													m_MaxGroupPageCount(0),
													m_CurGroupPageCount(0),
													m_MaxMemberPageCount(0),
													m_CurMemberPageCount(0),
													m_pUnionMemberBtnNextPage(NULL),
													m_pUnionMemberBtnPrevPage(NULL),
													m_pUnionMemberPageCount(NULL)
{
}

CDnRepUnionDlg::~CDnRepUnionDlg(void)
{
	SAFE_RELEASE_SPTR(m_hNpcFaceTexture);
	SAFE_RELEASE_SPTR(m_hHideNpcFaceTexture);
	SAFE_RELEASE_SPTR(m_hEmblemTexture);
}

void CDnRepUnionDlg::Initialize(bool bShow)
{
#if defined(PRE_ADD_INTEGERATE_QUEST_REPUT)
	CEtUIDialog::Initialize(CEtResourceMng::GetInstance().GetFullName("Tab_ReputeGroupListDlg.ui").c_str(), bShow);
#else
	CEtUIDialog::Initialize(CEtResourceMng::GetInstance().GetFullName("ReputeGroupListDlg.ui").c_str(), bShow);
#endif // PRE_ADD_INTEGERATE_QUEST_REPUT
}

void CDnRepUnionDlg::InitialUpdate()
{
	int i = 0;
	std::string ctrlName;
	for (; i < NPC_UNION_REPUTATION_COUNT_PER_PAGE; ++i)
	{
		ctrlName = FormatA("ID_CLICK%d", i);
		m_UnionGroupUIs[i].pEventReceiver = GetControl<CEtUIStatic>(ctrlName.c_str());
		m_UnionGroupUIs[i].pEventReceiver->SetButton(true);

		ctrlName = FormatA("ID_SELECT%d", i);
		m_UnionGroupUIs[i].pSelect = GetControl<CEtUIStatic>(ctrlName.c_str());

		ctrlName = FormatA("ID_TEXTUREL_MARK%d", i);
		m_UnionGroupUIs[i].pEmblem = GetControl<CEtUITextureControl>(ctrlName.c_str());

		ctrlName = FormatA("ID_TEXT_GROUPNAME%d", i);
		m_UnionGroupUIs[i].pName = GetControl<CEtUIStatic>(ctrlName.c_str());

		ctrlName = FormatA("ID_TEXT_GROUPPOINT%d", i);
		m_UnionGroupUIs[i].pPointText = GetControl<CEtUIStatic>(ctrlName.c_str());

		ctrlName = FormatA("ID_TEXT_GROUPPOINTCOUNT%d", i);
		m_UnionGroupUIs[i].pPoint = GetControl<CEtUIStatic>(ctrlName.c_str());
	}

	m_UnionMemberUI.pBase	= GetControl<CEtUIStatic>("ID_STATIC_NPCLISTBG");
	for (i = 0; i < NPC_UNION_MEMBER_COUNT_PER_PAGE; ++i)
	{
		ctrlName = FormatA("ID_TEXTUREL_NPC%d", i);
		m_UnionMemberUI.pFace[i] = GetControl<CEtUITextureControl>(ctrlName.c_str());

		ctrlName = FormatA("ID_PROGRESSBAR_REPUTE%d", i);
		m_UnionMemberUI.pFavorValue[i] = GetControl<CEtUIProgressBar>(ctrlName.c_str());
	}

	m_pUnionGroupPageCount		= GetControl<CEtUIStatic>("ID_BT_PAGE");
	m_pUnionGroupBtnPrevPage	= GetControl<CEtUIButton>("ID_BT_PRIOR");
	m_pUnionGroupBtnNextPage	= GetControl<CEtUIButton>("ID_BT_NEXT");

	m_pUnionMemberPageCount		= GetControl<CEtUIStatic>("ID_TEXT_LISTPAGE");
	m_pUnionMemberBtnPrevPage	= GetControl<CEtUIButton>("ID_BT_LISTPRIOR");
	m_pUnionMemberBtnNextPage	= GetControl<CEtUIButton>("ID_BT_LISTNEXT");

	m_pUnionExplainText	= GetControl<CEtUITextBox>("ID_TEXTBOX_GROUPDETAIL");

	for (i = 0; i < eUFT_MAX; ++i)
	{
		ctrlName = FormatA("ID_RBT_TAB%d", i);
		m_pUnionTab[i] = GetControl<CEtUIRadioButton>(ctrlName.c_str());
	}

	SAFE_RELEASE_SPTR(m_hNpcFaceTexture);
	m_hNpcFaceTexture = LoadResource( "Repute_NPC.dds", RT_TEXTURE, true );

	SAFE_RELEASE_SPTR(m_hHideNpcFaceTexture);
	m_hHideNpcFaceTexture = LoadResource("ReputeGray_NPC.dds", RT_TEXTURE, true);

	SAFE_RELEASE_SPTR(m_hEmblemTexture);
	m_hEmblemTexture = LoadResource("Repute_Mark.dds", RT_TEXTURE, true);

#ifdef PRE_MOD_CORRECT_UISOUND_PLAY
	m_DlgInfo.bSound = false;
#endif 
}

void CDnRepUnionDlg::Reset()
{
	m_pUnionGroupPageCount->ClearText();
	m_MaxGroupPageCount = 0;
	m_CurGroupPageCount = 0;

	m_pUnionMemberPageCount->ClearText();
	m_MaxMemberPageCount = 0;
	m_CurMemberPageCount = 0;
	ShowMemberPageBtn(false);

	m_pUnionTab[eUFT_EXPLAIN]->SetChecked(true);
	m_pUnionTab[eUFT_MEMBER]->SetChecked(false);
	m_pUnionTab[eUFT_REWARD]->SetChecked(false);
}

CDnRepUnionDlg::eUnionFigureType CDnRepUnionDlg::GetCurUnionTabType() const
{
	int i = 0;
	for (; i < eUFT_MAX; ++i)
	{
		if (m_pUnionTab[i]->IsChecked())
			return (eUnionFigureType)i;
	}

	return eUFT_MAX;
}

const CDnRepUnionDlg::SUnionUnitUI* CDnRepUnionDlg::GetCurSelectedUnionUI() const
{
	int i = 0;
	for (; i < NPC_UNION_REPUTATION_COUNT_PER_PAGE; ++i)
	{
		if (m_UnionGroupUIs[i].IsSelected())
			return &(m_UnionGroupUIs[i]);
	}

	return NULL;
}

void CDnRepUnionDlg::Show(bool bShow)
{
	if (bShow)
	{
		UpdateAll(true);
	}

	CEtUIDialog::Show(bShow);
}

void CDnRepUnionDlg::UpdateAll(bool bIsInit)
{
	if (bIsInit)
		Reset();

	DNTableFileFormat*  pUnionTable = GetDNTable(CDnTableDB::TUNION);
	if (pUnionTable == NULL)
	{
		_ASSERT(0);
		return;
	}

	CDnQuestTask* pQuestTask = static_cast<CDnQuestTask*>(CTaskManager::GetInstance().GetTask( "QuestTask" ));
	if (pQuestTask == NULL)
		return;

	CReputationSystemRepository* pReputationRepos = pQuestTask->GetReputationRepository();
	if (pReputationRepos == NULL)
		return;

	m_MaxGroupPageCount = pUnionTable->GetItemCount() / NPC_UNION_REPUTATION_COUNT_PER_PAGE;

	int i = 0;
	int tableIndex = m_CurGroupPageCount * NPC_UNION_REPUTATION_COUNT_PER_PAGE;
	for (; i < NPC_UNION_REPUTATION_COUNT_PER_PAGE; ++i, ++tableIndex)
	{
		m_UnionGroupUIs[i].Reset();

		int tableItemID = pUnionTable->GetItemID(tableIndex);
		if (tableItemID >= 0)
		{
			m_UnionGroupUIs[i].tableItemID = tableItemID;
			m_UnionGroupUIs[i].unionID = pUnionTable->GetFieldFromLablePtr(tableItemID, "_UnionID")->GetInteger();

			int unionEmblemIconIdx = pUnionTable->GetFieldFromLablePtr(tableItemID, "_UnionIconID")->GetInteger();
			m_UnionGroupUIs[i].SetEmblem(m_hEmblemTexture, unionEmblemIconIdx);

			int unionNameStringNum = pUnionTable->GetFieldFromLablePtr(tableItemID, "_UnionName")->GetInteger();
			m_UnionGroupUIs[i].SetName(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, unionNameStringNum));

			if (m_UnionGroupUIs[i].unionID >= NpcReputation::UnionType::Max)
			{
				_ASSERT(0);
			}
			else
			{
				UNIONPT_TYPE unionPoint = pReputationRepos->GetUnionPoint(m_UnionGroupUIs[i].unionID);
				if (unionPoint >= 0)
				{
					m_UnionGroupUIs[i].SetPoint(unionPoint);
					m_UnionGroupUIs[i].ShowPoint(true);
				}
			}

			m_UnionGroupUIs[i].ShowAll(true);
		}
	}

	UpdateGroupPageBtn();

	UpdateMemberPage(true);

	UpdateFigurePage();
}

void CDnRepUnionDlg::UpdateGroupPageBtn()
{
	std::wstring pageString = FormatW(L"%d / %d", m_CurGroupPageCount + 1, m_MaxGroupPageCount + 1);
	m_pUnionGroupPageCount->SetText(pageString.c_str());

	m_pUnionGroupBtnNextPage->Enable(m_MaxGroupPageCount > m_CurGroupPageCount);
	m_pUnionGroupBtnPrevPage->Enable(m_CurGroupPageCount > 0);
}

void CDnRepUnionDlg::ShowMemberPageBtn(bool bShow)
{
	m_pUnionMemberPageCount->Show(bShow);
	m_pUnionMemberBtnPrevPage->Show(bShow);
	m_pUnionMemberBtnNextPage->Show(bShow);
}

void CDnRepUnionDlg::UpdateMemberPageBtn()
{
	bool bValid = false;
	const SUnionUnitUI* pUnionSelected = GetCurSelectedUnionUI();
	if (pUnionSelected)
	{
		std::map<int, std::vector<SUnionMemberInfo> >::const_iterator uiIter = m_UnionNpcList.find(pUnionSelected->unionID);
		if (uiIter != m_UnionNpcList.end())
		{
			const std::vector<SUnionMemberInfo>& infoList = (*uiIter).second;
			m_MaxMemberPageCount = (int)infoList.size() / NPC_UNION_MEMBER_COUNT_PER_PAGE;
			if (m_CurMemberPageCount > m_MaxMemberPageCount)
				m_CurMemberPageCount = 0;

			bValid = true;
		}
	}

	if (bValid == false)
		m_CurMemberPageCount = m_MaxMemberPageCount = 0;

	std::wstring str = FormatW(L"%d / %d", m_CurMemberPageCount + 1, m_MaxMemberPageCount + 1);
	m_pUnionMemberPageCount->SetText(str.c_str());

	m_pUnionMemberBtnNextPage->Enable(m_MaxMemberPageCount > m_CurMemberPageCount);
	m_pUnionMemberBtnPrevPage->Enable(m_CurMemberPageCount > 0);
}

bool CDnRepUnionDlg::SortByQuestOpened(const SUnionMemberInfo& a, const SUnionMemberInfo& b)
{
	return (a.bIsOpened > b.bIsOpened);
}

bool CDnRepUnionDlg::SortByNpcIndex(const SUnionMemberInfo& a, const SUnionMemberInfo& b)
{
	return (a.npcId < b.npcId);
}

void CDnRepUnionDlg::ProcessPage(bool bNext)
{
	m_CurMemberPageCount = bNext ? m_CurMemberPageCount + 1 : m_CurMemberPageCount - 1;
	CommonUtil::ClipNumber(m_CurMemberPageCount, 0, m_MaxMemberPageCount);

	UpdateMemberPage(false);
}

void CDnRepUnionDlg::UpdateMemberPage(bool bMakeMemberList)
{
	DNTableFileFormat*  pReputeTable = GetDNTable(CDnTableDB::TREPUTE);
	if (pReputeTable == NULL)
	{
		_ASSERT(0);
		return;
	}

	CDnQuestTask* pQuestTask = static_cast<CDnQuestTask*>(CTaskManager::GetInstance().GetTask( "QuestTask" ));	
	if( !pQuestTask ) 
		return;

	m_UnionMemberUI.ShowAll(false);

	if (bMakeMemberList || m_UnionNpcList.empty())
	{
		m_UnionNpcList.clear();

		int index = 0;
		int tableCount = pReputeTable->GetItemCount();
		for (; index < tableCount; ++index)
		{
			int tableId = pReputeTable->GetItemID(index);
			int npcUnionId = pReputeTable->GetFieldFromLablePtr(tableId, "_UnionID")->GetInteger();

			SUnionMemberInfo info;
			info.tableId = tableId;
			info.npcId = pReputeTable->GetFieldFromLablePtr(tableId, "_NpcID")->GetInteger();

			bool bCompleletedOpenQuest = false;
			std::vector<std::string> tokens;

			std::string szReputeClearQuestString = pReputeTable->GetFieldFromLablePtr(tableId, "_QuestID2")->GetString();
			TokenizeA(szReputeClearQuestString, tokens, ",");

			std::vector<std::string>::iterator questIter = tokens.begin();
			for (; questIter != tokens.end(); ++questIter)
			{
				std::string& token = (*questIter);
				int questId = atoi(token.c_str());

#ifdef PRE_ADD_REPUTATION_EXPOSURE
				// 미션 클리어 유무를 체크한다.
				bool bIsClearMission = false;
				int  nMissionID		 = pReputeTable->GetFieldFromLablePtr(tableId, "_Mission")->GetInteger();
				if(  nMissionID > 0 )
				{
					CDnMissionTask::MissionInfoStruct* pMissionInfo = GetMissionTask().GetMissionFromArrayIndex(nMissionID - 1);
					if(pMissionInfo) 
						bIsClearMission = pMissionInfo->bAchieve;
				}

				if (bIsClearMission)
#else
				if (pQuestTask->IsClearQuest(questId))
#endif
				{
					bCompleletedOpenQuest = true;
					break;
				}
			}

			info.bIsOpened = bCompleletedOpenQuest;

			std::map<int, std::vector<SUnionMemberInfo> >::iterator iter = m_UnionNpcList.find(npcUnionId);
			if (iter != m_UnionNpcList.end())
			{
				std::vector<SUnionMemberInfo>& curList = (*iter).second;
				curList.push_back(info);
			}
			else
			{
				std::vector<SUnionMemberInfo> memberInfo;
				memberInfo.push_back(info);

				m_UnionNpcList.insert(std::make_pair(npcUnionId, memberInfo));
			}
		}

		std::map<int, std::vector<SUnionMemberInfo> >::iterator unionIter = m_UnionNpcList.begin();
		for (; unionIter != m_UnionNpcList.end(); ++unionIter)
		{
			std::vector<SUnionMemberInfo>& memberList = (*unionIter).second;

			std::sort(memberList.begin(), memberList.end(), SortByNpcIndex);
			std::sort(memberList.begin(), memberList.end(), SortByQuestOpened);
		}
	}

	UpdateMemberPageBtn();

	CReputationSystemRepository* pReputationRepos = pQuestTask->GetReputationRepository();
	if (pReputationRepos == NULL)
		return;

	DNTableFileFormat*  pNpcTable = GetDNTable( CDnTableDB::TNPC );
	if (pNpcTable == NULL)
		return;

	m_UnionMemberUI.ShowBase(true);

	const SUnionUnitUI* pUnionSelected = GetCurSelectedUnionUI();
	if (pUnionSelected)
	{
		std::map<int, std::vector<SUnionMemberInfo> >::const_iterator uiIter = m_UnionNpcList.find(pUnionSelected->unionID);
		if (uiIter != m_UnionNpcList.end())
		{
			const std::vector<SUnionMemberInfo>& memberInfoList = (*uiIter).second;

			int currentIndex = m_CurMemberPageCount * NPC_UNION_MEMBER_COUNT_PER_PAGE;
			int index = 0;
			for (; index < NPC_UNION_MEMBER_COUNT_PER_PAGE; ++index)
			{
				if (currentIndex < (int)memberInfoList.size())
				{
					const SUnionMemberInfo& info = memberInfoList.at(currentIndex);

					m_UnionMemberUI.npcNameStringID[index] = pNpcTable->GetFieldFromLablePtr(info.npcId, "_NameID")->GetInteger();

					m_UnionMemberUI.npcID[index] = pReputeTable->GetFieldFromLablePtr(info.tableId, "_NpcID")->GetInteger();

					int npcIconIdx = pReputeTable->GetFieldFromLablePtr(info.tableId, "_NpcIcon")->GetInteger();
					m_UnionMemberUI.SetFace(info.bIsOpened ? m_hNpcFaceTexture : m_hHideNpcFaceTexture, npcIconIdx, index);

					int favorPercent = 0;
					if (info.bIsOpened)
						favorPercent = pReputationRepos->GetNpcReputationPercent(info.npcId, IReputationSystem::NpcFavor);
					m_UnionMemberUI.pFavorValue[index]->SetProgress((float)favorPercent);

					++currentIndex;

					m_UnionMemberUI.ShowNpc(true, index);
				}
			}
		}
	}

	ShowMemberPageBtn(GetCurUnionTabType() == eUFT_MEMBER);
}

void CDnRepUnionDlg::UpdateFigurePage()
{
	DNTableFileFormat*  pUnionTable = GetDNTable(CDnTableDB::TUNION);
	if (pUnionTable == NULL)
	{
		_ASSERT(0);
		return;
	}

	m_UnionMemberUI.ShowAll(false);
	m_pUnionExplainText->ClearText();
	m_pUnionExplainText->Show(false);

	eUnionFigureType type = GetCurUnionTabType();
	const SUnionUnitUI* pUnionSelected = GetCurSelectedUnionUI();
	if (pUnionSelected)
	{
		std::wstring text;
		if (type == eUFT_EXPLAIN)
		{
			int explainStringNum = pUnionTable->GetFieldFromLablePtr(pUnionSelected->tableItemID, "_UnionInfoID")->GetInteger();
			m_pUnionExplainText->SetText(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, explainStringNum));
			m_pUnionExplainText->Show(true);

			ShowMemberPageBtn(false);
		}
		else if (type == eUFT_REWARD)
		{
			int explainStringNum = pUnionTable->GetFieldFromLablePtr(pUnionSelected->tableItemID, "_UnionRewardInfoID")->GetInteger();
			m_pUnionExplainText->SetText(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, explainStringNum));
			m_pUnionExplainText->Show(true);

			ShowMemberPageBtn(false);
		}
		else if (type == eUFT_MEMBER)
		{
			UpdateMemberPage(false);
		}
	}

	m_pUnionTab[eUFT_EXPLAIN]->Enable(pUnionSelected != NULL);
	m_pUnionTab[eUFT_MEMBER]->Enable(pUnionSelected != NULL);
	m_pUnionTab[eUFT_REWARD]->Enable(pUnionSelected != NULL);
}

void CDnRepUnionDlg::ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg /*= 0*/)
{
	CEtUIDialog::ProcessCommand(nCommand, bTriggeredByUser, pControl, uMsg);
	SetCmdControlName(pControl->GetControlName());

	if (nCommand == EVENT_BUTTON_CLICKED)
	{
		if( IsCmdControl( "ID_BT_LISTPRIOR" ) )
		{
			ProcessPage(false);
		}
		if( IsCmdControl( "ID_BT_LISTNEXT" ) )
		{
			ProcessPage(true);
		}
		if (strstr(pControl->GetControlName(), "ID_CLICK"))
		{
			const SUnionUnitUI* pUnit = GetCurSelectedUnionUI();
			int i = 0;
			for (; i < NPC_UNION_REPUTATION_COUNT_PER_PAGE; ++i)
			{
				bool bSelect = (m_UnionGroupUIs[i].IsEmpty() == false && pControl == m_UnionGroupUIs[i].pEventReceiver);
				if (bSelect && &(m_UnionGroupUIs[i]) != pUnit)
					m_CurMemberPageCount = 0;

				m_UnionGroupUIs[i].ShowSelect(bSelect);
			}

			UpdateFigurePage();
		}
	}
	else if (nCommand == EVENT_RADIOBUTTON_CHANGED && bTriggeredByUser)
	{
		UpdateFigurePage();
	}
}

bool CDnRepUnionDlg::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if( !IsShow() )
	{
		return false;
	}

	bool bRet = CEtUIDialog::MsgProc( hWnd, uMsg, wParam, lParam );
	switch( uMsg )
	{
	case WM_MOUSEMOVE:
		{
			float fMouseX = 0;
			float fMouseY = 0;
			POINT MousePoint;
			MousePoint.x = short( LOWORD( lParam ) );
			MousePoint.y = short( HIWORD( lParam ) );
			PointToFloat( MousePoint, fMouseX, fMouseY );

			bool bMouseEnter = false;
			for (int i = 0; i < NPC_UNION_MEMBER_COUNT_PER_PAGE; i++)
			{
				if (m_UnionMemberUI.IsShow())
				{
					if (m_UnionMemberUI.pFace[i]->IsShow() && 
						m_UnionMemberUI.pFace[i]->IsInside(fMouseX, fMouseY) && 
						i < NPC_UNION_MEMBER_COUNT_PER_PAGE)
						ShowTooltipDlg(m_UnionMemberUI.pFace[i], true, m_UnionMemberUI.npcNameStringID[i]);
				}
			}
		}
	case WM_MOUSEWHEEL:
		{
			//다이얼로그 위에 있어야 하고, 정렬콤보 박스가 포커스가 아닐때
			if ( IsMouseInDlg() )
			{
				float fMouseX = 0;
				float fMouseY = 0;
				POINT MousePoint;
				MousePoint.x = short( LOWORD( lParam ) );
				MousePoint.y = short( HIWORD( lParam ) );
				PointToFloat( MousePoint, fMouseX, fMouseY );

				bool bMouseEnter = false;
				for (int i = 0; i < NPC_UNION_MEMBER_COUNT_PER_PAGE; i++)
				{
					if (m_UnionMemberUI.IsShow())
					{
						UINT uLines;
						SystemParametersInfo( SPI_GETWHEELSCROLLLINES, 0, &uLines, 0 );
						int nScrollAmount = int( ( short )HIWORD( wParam ) ) / WHEEL_DELTA * uLines;

						//휠을 내릴때 - : 다음 페이지, 올릴때 + : 이전 페이지
						if ( nScrollAmount != 0 )
						{
							ProcessPage(nScrollAmount < 0);
							ShowTooltipDlg(NULL, false, 0);
						}
					}
				}
			}
		}
		break;

	case WM_LBUTTONUP:
		{
			float fMouseX = 0;
			float fMouseY = 0;
			POINT MousePoint;
			MousePoint.x = short( LOWORD( lParam ) );
			MousePoint.y = short( HIWORD( lParam ) );
			PointToFloat( MousePoint, fMouseX, fMouseY );

			if (m_UnionMemberUI.IsShow())
			{
				int npcId = m_UnionMemberUI.GetNpcIDByPos(fMouseX, fMouseY);
				if (npcId > -1)
				{
#ifdef PRE_ADD_INTEGERATE_QUEST_REPUT
					CDnIntegrateQuestNReputationDlg* npcRepTabDlg = static_cast<CDnIntegrateQuestNReputationDlg*>(CDnInterface::GetInstance().GetMainMenuDialog(CDnMainMenuDlg::InTEGRATE_QUESTNREPUTATION_DIALOG));
					if(npcRepTabDlg)
						npcRepTabDlg->SetReputeNpcUnionPage(npcId);
#else
					CDnNpcReputationTabDlg* npcRepTabDlg = static_cast<CDnNpcReputationTabDlg*>(CDnInterface::GetInstance().GetMainMenuDialog(CDnMainMenuDlg::NPC_REPUTATION_TAB_DIALOG));
					if(npcRepTabDlg)
						npcRepTabDlg->OnSelectNpcInUnionDlg(npcId);
#endif
					
				}
			}
		}
		break;
	}

	return bRet;
}