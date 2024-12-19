#include "StdAfx.h"
#include "DnLevelUpGuideDlg.h"

#ifdef PRE_ADD_LEVELUP_GUIDE
#include "DnPlayerActor.h"
#include "DnSkillTask.h"
#include "DnInterface.h"
#include "DnCommonUtil.h"
#include "DnMainDlg.h"

#ifdef PRE_ADD_LEVELUP_REWARD_NOTIFIER
#include "DnItemTask.h"
#endif 

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnLevelUpGuideDlg::CDnLevelUpGuideDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
	: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback, false )
{
	m_pNewLevelText = NULL;
	m_pNewLevelCongratText = NULL;
	m_pNewLevelBorder = NULL;
	m_pNewLevelBackGround = NULL;

	m_pNewSkillBorder = NULL;
	m_pNewSkillBorder2 = NULL;
	m_pNewSkillBackGround = NULL;
	m_pNewSkillText = NULL;

	memset(m_pNewSkillSlotBtn, 0, sizeof(m_pNewSkillSlotBtn));

	m_State = eNone;
	m_Timer = 0.f;
	m_NewLevelCache = 0;
}

CDnLevelUpGuideDlg::~CDnLevelUpGuideDlg(void)
{
}

void CDnLevelUpGuideDlg::Initialize( bool bShow )
{
	CDnCustomDlg::Initialize( CEtResourceMng::GetInstance().GetFullName( "LevelGuideDlg.ui" ).c_str(), bShow );
}

void CDnLevelUpGuideDlg::InitialUpdate()
{
	m_pNewLevelBorder		= GetControl<CEtUIStatic>("ID_STATIC0");
	m_pNewLevelBackGround	= GetControl<CEtUIStatic>("ID_STATIC2");
	m_pNewLevelText			= GetControl<CEtUIStatic>("ID_TEXT0");
	m_pNewLevelCongratText	= GetControl<CEtUIStatic>("ID_TEXT1");

	m_pNewSkillBorder		= GetControl<CEtUIStatic>("ID_STATIC4");
	m_pNewSkillBorder2		= GetControl<CEtUIStatic>("ID_STATIC1");
	m_pNewSkillBackGround	= GetControl<CEtUIStatic>("ID_STATIC3");
	m_pNewSkillText			= GetControl<CEtUIStatic>("ID_TEXT3");

	int i = 0;
	std::string slotCtrlName;
	for (; i < _MAX_NEW_LEVELUP_SKILL_SLOT; ++i)
	{
		slotCtrlName = FormatA("ID_BT_SKILL%d", i);
		m_pNewSkillSlotBtn[i] = GetControl<CDnSkillSlotButton>(slotCtrlName.c_str());
	}

	for (i = 0; i < _MAX_NEW_LEVELUP_SKILL_SLOT_EVEN; ++i)
	{
		slotCtrlName = FormatA("ID_STATIC_POINT%d", i);
		m_pNewSkillEvenCountPos[i] = GetControl<CEtUIStatic>(slotCtrlName.c_str());
	}
}

void CDnLevelUpGuideDlg::ShowNewLevel(bool bShow)
{
	if (bShow)
	{
		std::wstring str;
		if (m_NewLevelCache <= 0)
			str = L"LEVEL UP!";	// note by kalliste : temporarily
		else
			str = FormatW(L"%s %d", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 64), m_NewLevelCache);	// UISTRING : ·¹º§
		m_pNewLevelText->SetText(str.c_str());
	}


	m_pNewLevelBorder->Show(bShow);
	m_pNewSkillBorder2->Show(bShow);
	m_pNewLevelBackGround->Show(bShow);
	m_pNewLevelText->Show(bShow);
	m_pNewLevelCongratText->Show(bShow);
}

// todo : send skill information over by argument
void CDnLevelUpGuideDlg::ShowNewSkill(bool bShow)
{
	m_pNewSkillBorder->Show(bShow);
	m_pNewSkillBorder2->Show(bShow);
	m_pNewSkillBackGround->Show(bShow);
	m_pNewSkillText->Show(bShow);

	int i = 0;
	std::string slotCtrlName;
	for (; i < _MAX_NEW_LEVELUP_SKILL_SLOT; ++i)
	{
		slotCtrlName = FormatA("ID_BT_SKILL%d", i);
		m_pNewSkillSlotBtn[i]->Show((bShow && m_pNewSkillSlotBtn[i]->IsEmptySlot() == false));
	}

	for (i = 0; i < _MAX_NEW_LEVELUP_SKILL_SLOT_EVEN; ++i)
	{
		slotCtrlName = FormatA("ID_STATIC_POINT%d", i);
		m_pNewSkillEvenCountPos[i]->Show(false);
	}
}

void CDnLevelUpGuideDlg::ShowOnLevelUp(bool bShow, int nNewLevel)
{
	m_NewLevelCache = bShow ? nNewLevel : 0;

	std::vector<int> newLevelSkillIds;
	
	if (CDnActor::s_hLocalActor)
	{
		CDnPlayerActor* pPlayerActor = static_cast<CDnPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
		if (pPlayerActor && CDnSkillTask::IsActive())
		{
			std::vector<int> jobHistory;
			pPlayerActor->GetJobHistory(jobHistory);
			CDnSkillTask::GetInstance().GetLevelUpSkillInfo(newLevelSkillIds, nNewLevel, jobHistory);

			int skillCount = (int)newLevelSkillIds.size();
			if (skillCount > _MAX_NEW_LEVELUP_SKILL_SLOT)
			{
				_ASSERT(0);
#ifndef _FINAL_BUILD
				std::wstring str = FormatW(L"Level Up New Skill Count Error : current - %d, max - %d", skillCount, _MAX_NEW_LEVELUP_SKILL_SLOT);
				GetInterface().AddChatMessage(CHATTYPE_SYSTEM, L"Debug", str.c_str(), false);
#endif
				skillCount = _MAX_NEW_LEVELUP_SKILL_SLOT;
			}

			if (skillCount > 0)
			{
				bool bEven = (skillCount % 2 != 0);
				int slotStartIndex = 0;

				int slotCenterIndex = ((bEven) ? _MAX_NEW_LEVELUP_SKILL_SLOT_EVEN : _MAX_NEW_LEVELUP_SKILL_SLOT) / 2;
				if (skillCount == 1)
				{
					slotStartIndex = slotCenterIndex;
				}
				else if (skillCount > 1)
				{
					int skillCenterIndex = (skillCount / 2);
					if (skillCenterIndex > slotCenterIndex)
					{
						_ASSERT(0);
						return;
					}

					slotStartIndex = slotCenterIndex - skillCenterIndex;
				}
				else
				{
					_ASSERT(0);
					return;
				}

				if (bEven)
				{
					int k = 0;
					for (; k < _MAX_NEW_LEVELUP_SKILL_SLOT_EVEN; ++k)
					{
						const SUICoord& coord = m_pNewSkillEvenCountPos[k]->GetBaseUICoord();
						m_pNewSkillSlotBtn[k]->SetPosition(coord.fX, coord.fY);
					}
				}
				else
				{
					int k = 0;
					for (; k < _MAX_NEW_LEVELUP_SKILL_SLOT; ++k)
					{
						const SUICoord& coord = m_pNewSkillSlotBtn[k]->GetBaseUICoord();
						m_pNewSkillSlotBtn[k]->SetPosition(coord.fX, coord.fY);
					}
				}

				CommonUtil::ClipNumber(slotStartIndex, 0, slotStartIndex);

				int i = 0, slotIdx = slotStartIndex;
				for (; i < skillCount; ++i)
				{
					DnSkillHandle hTempSkillHandle = CDnSkill::CreateSkill(CDnActor::Identity(), newLevelSkillIds[i], 1);
					m_pNewSkillSlotBtn[slotIdx]->SetItem((MIInventoryItem*)hTempSkillHandle.GetPointer(), 1);
					m_pNewSkillSlotBtn[slotIdx]->Show(false);
					m_SkillHandleList.push_back(hTempSkillHandle);
					slotIdx++;
					if (slotIdx >= _MAX_NEW_LEVELUP_SKILL_SLOT)
						break;
				}
			}
		}
	}

	Show(bShow);
}

void CDnLevelUpGuideDlg::Show(bool bShow)
{
	if (m_bShow == bShow)
		return;

	if (bShow)
	{
		SetFadeMode( CEtUIDialog::None );
		m_State = eShowNewLevel_Start;
	}
	else
	{
		m_Timer = _LEVELUP_SKILL_PRESENT_TIME;
		m_State = eShowNewSkill_Process;

		std::vector<DnSkillHandle>::iterator iter = m_SkillHandleList.begin();
		for (; iter != m_SkillHandleList.end();)
		{
			DnSkillHandle hCurHandle = (*iter);
			if (hCurHandle)
				hCurHandle->Release();
			iter = m_SkillHandleList.erase(iter);
		}

		m_SkillHandleList.clear();
	}

	CDnCustomDlg::Show(bShow);
}

void CDnLevelUpGuideDlg::Process(float fElapsedTime)
{
	CDnCustomDlg::Process(fElapsedTime);

	if (IsShow() == false || m_State == eNone)
		return;

	switch(m_State)
	{
	case eShowNewLevel_Start:
		{
			ShowNewSkill(false);
			ShowNewLevel(true);

			m_State = eShowNewLevel_Process;
			m_Timer = 0.f;
		}
		break;

	case eShowNewLevel_Process:
		{
			if (m_Timer >= _LEVELUP_PRESENT_TIME)
			{
				SetFadeMode( CEtUIDialog::CancelRender );
				ShowNewLevel(false);

				m_State = eShowNewLevel_End;
				m_Timer = 0.f;
			}
		}
		break;

	case eShowNewLevel_End:
		{
			if (m_Timer >= _LEVELUP_INTERVAL_TIME)
			{
				if (m_SkillHandleList.size() > 0)
					m_State = eShowNewSkill_Start;
				else
					m_State = eShowNewSkill_End;

				m_Timer = 0.f;
			}
		}
		break;

	case eShowNewSkill_Start:
		{
			ShowNewLevel(false);
			ShowNewSkill(true);
			if (GetInterface().GetMainBarDialog()) 
				GetInterface().GetMainBarDialog()->SetLevelUpSkillAlarm(true);

			m_State = eShowNewSkill_Process;
			m_Timer = 0.f;
		}
		break;

	case eShowNewSkill_Process:
		{
			if (m_Timer >= _LEVELUP_SKILL_PRESENT_TIME)
			{
				m_State = eShowNewSkill_End;
				m_Timer = 0.f;

				ShowNewLevel(false);
				ShowNewSkill(false);
			}
		}
		break;

	case eShowNewSkill_End:
		{
			if (m_Timer >= _LEVELUP_SKILL_PRESENT_TIME)
			{
				Show(false);
				m_State = eNone;
				m_Timer = 0.f;
			}
		}
		break;
	}

	m_Timer += fElapsedTime;
}

#endif // PRE_ADD_LEVELUP_GUIDE