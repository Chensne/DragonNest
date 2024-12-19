#include "StdAfx.h"

#ifdef PRE_ADD_START_POPUP_QUEUE

#include "DnStartPopupMgr.h"
#include "DnInterface.h"
#include "DnMainMenuDlg.h"
#include "DnEventTodayDlg.h"
#include "DnStampEventDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

bool CDnStartPopupMgr::SPopupUnit::IsSame(const SPopupUnit& unit) const
{
	return (type == unit.type && pPopupDlg == unit.pPopupDlg);
}

CDnStartPopupMgr::CDnStartPopupMgr()
{
	m_bShowing = false;
}

void CDnStartPopupMgr::RegisterTop(ePopupType type, CEtUIDialog* pDlg, const char* stringParam1)
{
	SPopupUnit unit;
	unit.type = type;
	unit.pPopupDlg = pDlg;
	if (stringParam1 != NULL)
		unit.stringParam = stringParam1;

	std::list<SPopupUnit>::const_iterator iter = m_Popups.begin();
	for (; iter != m_Popups.end(); ++iter)
	{
		const SPopupUnit& cur = (*iter);
		if (cur.IsSame(unit))
			return;
	}

	m_Popups.push_front(unit);
}

void CDnStartPopupMgr::RegisterLow(ePopupType type, CEtUIDialog* pDlg, const char* stringParam1)
{
	SPopupUnit unit;
	unit.type = type;
	unit.pPopupDlg = pDlg;
	if (stringParam1 != NULL)
		unit.stringParam = stringParam1;

	std::list<SPopupUnit>::const_iterator iter = m_Popups.begin();
	for (; iter != m_Popups.end(); ++iter)
	{
		const SPopupUnit& cur = (*iter);
		if (cur.IsSame(unit))
			return;
	}

	m_Popups.push_back(unit);
}

void CDnStartPopupMgr::Process(float fElapsedTime)
{
	if (m_bShowing)
	{
		if (m_Popups.size() > 0)
		{
			SPopupUnit& unit = m_Popups.front();
			if (unit.type == eNormal)
			{
				if (unit.pPopupDlg)
				{
					if (unit.pPopupDlg->IsShow() == false)
						PopAndShow();
				}
				else
				{
					PopAndShow();
				}
			}
			else if (unit.type == eEventToday || unit.type == eEventGuide)
			{
				CDnEventTodayDlg* pEventTodayDlg = static_cast<CDnEventTodayDlg*>(GetInterface().GetMainMenuDialog(CDnMainMenuDlg::EVENT_DIALOG));
				if (pEventTodayDlg)
				{
					if (pEventTodayDlg->IsShow() == false)
						PopAndShow();
				}
			}
			else if (unit.type == eStamp)
			{
				CDnStampEventDlg* pStampDlg = GetInterface().GetStampEventDlg();
				if (pStampDlg)
				{
					if (pStampDlg->IsShow() == false)
						PopAndShow();
				}
			}
		}
		else
		{
			m_bShowing = false;
		}
	}
}

void CDnStartPopupMgr::PopAndShow()
{
	if (m_Popups.size() <= 1)
	{
		StopAll();
		return;
	}

	m_Popups.pop_front();
	DoShow();
}

void CDnStartPopupMgr::StartShow()
{
	if (m_bShowing)
		return;

	m_bShowing = true;
	DoShow();
}

void CDnStartPopupMgr::DoShow()
{
	SPopupUnit& unit = m_Popups.front();
	if (unit.type == eNormal)
	{
		if (unit.pPopupDlg)
			unit.pPopupDlg->Show(true);
	}
	else if (unit.type == eEventToday)
	{
		GetInterface().ShowEventToday();
	}
	else if (unit.type == eStamp)
	{
		GetInterface().ShowStampDlg();
	}
	else if (unit.type == eEventGuide)
	{
		if (unit.stringParam.empty() == false)
		{
			CDnMainMenuDlg* pMainMenuDlg = GetInterface().GetMainMenuDialog();
			if (pMainMenuDlg)
				pMainMenuDlg->OnEventPopUp(unit.stringParam.c_str());
		}
	}
}

void CDnStartPopupMgr::StopAll()
{
	m_bShowing = false;
	m_Popups.clear();
}

#endif // PRE_ADD_START_POPUP_QUEUE