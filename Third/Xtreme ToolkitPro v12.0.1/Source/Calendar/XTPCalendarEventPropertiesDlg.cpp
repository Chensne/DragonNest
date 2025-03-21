// XTPCalendarEventPropertiesDlg.cpp: implementation of the CXTPCalendarEventPropertiesDlg.
//
// This file is a part of the XTREME CALENDAR MFC class library.
// (c)1998-2008 Codejock Software, All Rights Reserved.
//
// THIS SOURCE FILE IS THE PROPERTY OF CODEJOCK SOFTWARE AND IS NOT TO BE
// RE-DISTRIBUTED BY ANY MEANS WHATSOEVER WITHOUT THE EXPRESSED WRITTEN
// CONSENT OF CODEJOCK SOFTWARE.
//
// THIS SOURCE CODE CAN ONLY BE USED UNDER THE TERMS AND CONDITIONS OUTLINED
// IN THE XTREME TOOLKIT PRO LICENSE AGREEMENT. CODEJOCK SOFTWARE GRANTS TO
// YOU (ONE SOFTWARE DEVELOPER) THE LIMITED RIGHT TO USE THIS SOFTWARE ON A
// SINGLE COMPUTER.
//
// CONTACT INFORMATION:
// support@codejock.com
// http://www.codejock.com
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Resource.h"

#include "Common/XTPResourceManager.h"
#include "Common/XTPVC50Helpers.h"

#include "XTPCalendarEventPropertiesDlg.h"
#include "XTPCalendarEventRecurrenceDlg.h"
#include "XTPCalendarOccurSeriesChooseDlg.h"

#include "XTPCalendarUtils.h"
#include "XTPCalendarDefines.h"
#include "XTPCalendarData.h"
#include "XTPCalendarControl.h"
#include "XTPCalendarView.h"
#include "XTPCalendarViewDay.h"
#include "XTPCalendarEventLabel.h"
#include "XTPCalendarThemeOffice2007.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CXTPCalendarEventPropertiesDlg, CDialog)
/////////////////////////////////////////////////////////////////////////////
// CXTPCalendarEventPropertiesDlg dialog

BOOL XTPCalendarIsThemeOffice2007(CXTPCalendarControl* pCalendar)
{
	ASSERT(pCalendar);

	if (!pCalendar || !pCalendar->GetTheme())
		return FALSE;

	CXTPCalendarThemeOffice2007* ptheme2007 = DYNAMIC_DOWNCAST(CXTPCalendarThemeOffice2007, pCalendar->GetTheme());

	return ptheme2007 != NULL;
}

CXTPCalendarEventCategoryComboBox::CXTPCalendarEventCategoryComboBox()
{
	m_nLastKey = 0;
	m_pCategories = NULL;
}

CXTPCalendarEventCategoryComboBox::~CXTPCalendarEventCategoryComboBox()
{
	CMDTARGET_RELEASE(m_pCategories);
}

BEGIN_MESSAGE_MAP(CXTPCalendarEventCategoryComboBox, CComboBox)
	//{{AFX_MSG_MAP(CXTPSyntaxEditColorComboBox)
	ON_CONTROL_REFLECT(CBN_SELENDOK, OnSelChange)
	ON_CONTROL_REFLECT(CBN_DROPDOWN, OnDropDown)
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CXTPCalendarEventCategoryComboBox::Init(CXTPCalendarEventCategories* pCategories, CXTPCalendarEventCategoryIDs* pChecked)
{
	ResetContent();

	if (!pCategories || !pChecked)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	m_pCategories = pCategories;
	CMDTARGET_ADDREF(m_pCategories);

	int nCount = m_pCategories->GetCount();
	for (int i = 0; i < nCount; i++)
	{
		CXTPCalendarEventCategory* pCat = m_pCategories->GetAt(i);
		ASSERT(pCat);
		if (pCat)
		{
			int nIdx = AddString(pCat->GetName());
			SetItemDataPtr(nIdx, (void*)pCat);
		}
	}

	m_arChecked.RemoveAll();
	m_arChecked.Copy(*pChecked);

	return TRUE;
}

void CXTPCalendarEventCategoryComboBox::GetChecked(CXTPCalendarEventCategoryIDs* pChecked, BOOL* pbChanged)
{
	if (!pChecked)
		return;

	if (pbChanged)
	{
		int nCount = m_arChecked.GetSize();
		if (nCount == pChecked->GetSize())
		{
			for (int i = 0; i < nCount; i++)
			{
				if (m_arChecked[i] != pChecked->GetAt(i))
				{
					*pbChanged |= TRUE;
					break;
				}
			}
		}
		else
		{
			*pbChanged |= TRUE;
		}
	}

	pChecked->RemoveAll();
	pChecked->Copy(m_arChecked);
}

void CXTPCalendarEventCategoryComboBox::MeasureItem(LPMEASUREITEMSTRUCT lpMIS)
{
	ASSERT(lpMIS->CtlType == ODT_COMBOBOX);
	lpMIS->itemHeight = (::GetSystemMetrics(SM_CYVTHUMB)-::GetSystemMetrics(SM_CYEDGE)) + 4;
	lpMIS->itemWidth = 0;
}

void CXTPCalendarEventCategoryComboBox::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
	CDC*  pDC       = CDC::FromHandle(lpDIS->hDC);
	UINT  itemState = lpDIS->itemState;
	UINT  itemID    = lpDIS->itemID;
	CRect rcItem    = lpDIS->rcItem;

	if (!m_pCategories)
		return;

	BOOL bDisabled = ((itemState & ODS_DISABLED) == ODS_DISABLED);

	if((itemState & ODS_COMBOBOXEDIT) == ODS_COMBOBOXEDIT)
	{
		if (bDisabled)
		{
			pDC->SetTextColor(GetXtremeColor(COLOR_GRAYTEXT));
			pDC->SetBkColor(GetXtremeColor(COLOR_3DFACE));
			pDC->FillSolidRect(&rcItem, GetXtremeColor(COLOR_3DFACE));
		}

		BOOL bCtrlFocused = GetFocus() == this;
		if (bCtrlFocused)
		{
			pDC->FillSolidRect(&rcItem, GetXtremeColor(COLOR_HIGHLIGHT));
			pDC->DrawFocusRect(&rcItem);
		}

		int nOffset = 0;
		int nCount = m_arChecked.GetSize();
		for (int i = 0; i < nCount; i++)
		{
			CXTPCalendarEventCategory* pCat = m_pCategories->Find(m_arChecked.GetAt(i));
			ASSERT(pCat);
			if (pCat)
			{
				// Draw category color
				CRect rcColor = rcItem;
				rcColor.left += nOffset;
				rcColor.right = rcColor.left + rcColor.Height();
				rcColor.DeflateRect(2, 2, 2, 2);

				nOffset += rcColor.Height() + 2;

				if (bCtrlFocused)
				{
					CRect rcColor2 = rcColor;
					rcColor2.InflateRect(1, 1, 1, 1);
					CXTPCalendarTheme::DrawRoundRect(pDC, rcColor2, RGB(255, 255, 255), TRUE, //NULL);
						CBrush::FromHandle((HBRUSH)GetStockObject(LTGRAY_BRUSH)));
				}

				CXTPCalendarTheme::DrawRoundRect(pDC, rcColor, pCat->GetBorderColor(), FALSE,
											 pCat->GetBackgroundColor());
			}
		}

		return;
	}

	if (itemID == (UINT)-1)
		return;

	CXTPCalendarEventCategory* pCat = (CXTPCalendarEventCategory*)GetItemDataPtr(itemID);
	ASSERT(pCat);
	if (!pCat)
		return;

	BOOL bFocus = ((itemState & ODS_FOCUS) == ODS_FOCUS);
	int nFIdx = m_arChecked.Find(pCat->GetID());

	pDC->SetBkColor(GetXtremeColor(COLOR_WINDOW));

	// draw focus rectangle.
	if (bFocus)
	{
		pDC->SetTextColor(GetXtremeColor(COLOR_HIGHLIGHTTEXT));
		pDC->FillSolidRect(&rcItem, GetXtremeColor(COLOR_HIGHLIGHT));

		pDC->DrawFocusRect(&rcItem);
	}
	else if (nFIdx >= 0)
	{
		pDC->SetTextColor(GetXtremeColor(COLOR_BTNTEXT));
		pDC->FillSolidRect(&rcItem, GetXtremeColor(COLOR_BTNFACE));
	}
	else
	{
		pDC->SetTextColor(GetXtremeColor(COLOR_WINDOWTEXT));
		pDC->FillSolidRect(&rcItem, GetXtremeColor(COLOR_WINDOW));
	}


	// Draw category color
	CRect rcColor = rcItem;
	rcColor.right = rcColor.left + rcColor.Height();
	rcColor.DeflateRect(3, 3, 3, 3);

	if (nFIdx >= 0)
	{
		CRect rcColor2 = rcColor;
		rcColor2.InflateRect(1, 1, 1, 1);
		CXTPCalendarTheme::DrawRoundRect(pDC, rcColor2, RGB(255, 255, 255), TRUE, //NULL);
				CBrush::FromHandle((HBRUSH)GetStockObject(LTGRAY_BRUSH)));
	}

	CXTPCalendarTheme::DrawRoundRect(pDC, rcColor, pCat->GetBorderColor(), FALSE,
									 pCat->GetBackgroundColor());

	// draw text.
	CRect rText = rcItem;
	rText.left = rcColor.right;
	rText.DeflateRect(4, 2, 2, 1);

	pDC->SetBkMode(TRANSPARENT);
	pDC->DrawText(pCat->GetName(), rText, DT_LEFT | DT_VCENTER| DT_SINGLELINE);
}

int CXTPCalendarEventCategoryComboBox::GetLBCurSel() const
{
	if (GetDroppedState() == TRUE)
	{
		HWND hWnd = ::FindWindow(_T("ComboLBox"), NULL);
		if (::IsWindow(hWnd))
		{
			return (int)::SendMessage(hWnd, LB_GETCURSEL, 0, 0);
		}
	}
	return LB_ERR;
}

void CXTPCalendarEventCategoryComboBox::RedrawLB(BOOL bUpdateNow)
{
	if (GetDroppedState() == TRUE)
	{
		HWND hWnd = ::FindWindow(_T("ComboLBox"), NULL);
		if (::IsWindow(hWnd))
		{
			if (bUpdateNow)
				::UpdateWindow(hWnd);
			else
				::InvalidateRect(hWnd, NULL, FALSE);
		}
	}
}

void CXTPCalendarEventCategoryComboBox::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_SPACE && GetDroppedState())
	{
		_OnSelChange(GetLBCurSel());

		Invalidate(FALSE);
		RedrawLB();

		m_nLastKey = nChar;

		return;
	}

	if (nChar == VK_RETURN && m_nLastKey == VK_SPACE)
	{
		ShowDropDown(FALSE);
		Invalidate(FALSE);

		return;
	}

	m_nLastKey = nChar;

	CComboBox::OnKeyDown(nChar, nRepCnt, nFlags);
}
void CXTPCalendarEventCategoryComboBox::OnDropDown()
{
	SetCurSel(CB_ERR);
	m_nLastKey = 0;
}

void CXTPCalendarEventCategoryComboBox::OnSelChange()
{
	if (GetDroppedState())
		_OnSelChange(GetCurSel());
}

void CXTPCalendarEventCategoryComboBox::_OnSelChange(int nItemIndex)
{
	if (!m_pCategories || nItemIndex == CB_ERR || GetDroppedState() == FALSE)
		return;

	CXTPCalendarEventCategory* pCat = (CXTPCalendarEventCategory*)GetItemDataPtr(nItemIndex);
	ASSERT(pCat);
	if (!pCat)
		return;

	int nFIdx = m_arChecked.Find(pCat->GetID());

	if (nFIdx < 0)
		m_arChecked.InsertAt(0, pCat->GetID());
	else
		m_arChecked.RemoveAt(nFIdx);
}

/////////////////////////////////////////////////////////////////////////////
CXTPCalendarEventLabelComboBox::CXTPCalendarEventLabelComboBox()
{
	m_pLabels = NULL;
}

CXTPCalendarEventLabelComboBox::~CXTPCalendarEventLabelComboBox()
{
	CMDTARGET_RELEASE(m_pLabels);
}

void CXTPCalendarEventLabelComboBox::Init(CXTPCalendarEventLabels*  pLabels)
{
	ResetContent();

	ASSERT(pLabels);
	if (!pLabels)
		return;

	m_pLabels = pLabels;
	CMDTARGET_ADDREF(m_pLabels);

	for (int i = 0; i < m_pLabels->GetCount(); i++)
	{
		CXTPCalendarEventLabel* pLbl = m_pLabels->GetAt(i);
		ASSERT(pLbl);
		if (!pLbl)
			continue;
		int nIdx = AddString(pLbl->m_strName);
		SetItemData(nIdx, (DWORD_PTR)pLbl->m_nLabelID);
	}
}

int CXTPCalendarEventLabelComboBox::GetLabelID() const
{
	if (GetCurSel() == CB_ERR)
		return XTP_IDS_CALENDAR_EVENT_LABEL_NONE;

	return (int)GetItemData(GetCurSel());
}

void CXTPCalendarEventLabelComboBox::SetLabelID(int nID)
{
	for (int i = 0; i < GetCount(); i++)
	{
		if (nID == (int)GetItemData(i))
		{
			SetCurSel(i);
			return;
		}
	}

	SetCurSel(CB_ERR);
}

void CXTPCalendarEventLabelComboBox::MeasureItem(LPMEASUREITEMSTRUCT lpMIS)
{
	ASSERT(lpMIS->CtlType == ODT_COMBOBOX);
	lpMIS->itemHeight = (::GetSystemMetrics(SM_CYVTHUMB)-::GetSystemMetrics(SM_CYEDGE));
	lpMIS->itemWidth = 0;
}

void CXTPCalendarEventLabelComboBox::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
	CDC*  pDC       = CDC::FromHandle(lpDIS->hDC);
	UINT  itemState = lpDIS->itemState;
	UINT  itemID    = lpDIS->itemID;
	CRect rcItem    = lpDIS->rcItem;

	if (itemID == (UINT)-1 || !m_pLabels)
	{
		return;
	}

	int nLabelID = (int)GetItemData(itemID);
	CXTPCalendarEventLabel* pLbl = m_pLabels->Find(nLabelID);
	if (!pLbl)
		return;

	BOOL bDisabled = ((itemState & ODS_DISABLED) == ODS_DISABLED);
	BOOL bSelected = ((itemState & ODS_SELECTED) == ODS_SELECTED);
	BOOL bFocus    = ((itemState & ODS_FOCUS)    == ODS_FOCUS);

	// draw background.
	if (bDisabled)
	{
		pDC->SetTextColor(GetXtremeColor(COLOR_GRAYTEXT));
		pDC->SetBkColor(GetXtremeColor(COLOR_3DFACE));
		pDC->FillSolidRect(&rcItem, GetXtremeColor(COLOR_3DFACE));
	}
	else
	{
		if (bSelected)
		{
			pDC->SetTextColor(GetXtremeColor(COLOR_HIGHLIGHTTEXT));
			pDC->SetBkColor(GetXtremeColor(COLOR_WINDOW));
			pDC->FillSolidRect(&rcItem, GetXtremeColor(COLOR_HIGHLIGHT));
		}
		else
		{
			pDC->SetTextColor(GetXtremeColor(COLOR_WINDOWTEXT));
			pDC->SetBkColor(GetXtremeColor(COLOR_WINDOW));
			pDC->FillSolidRect(&rcItem, GetXtremeColor(COLOR_WINDOW));
		}

		// draw focus rectangle.
		if (bFocus)
		{
			pDC->DrawFocusRect(&rcItem);
		}
	}

	// determine the size of the color rectangle.
	CRect rColor(rcItem);
	rColor.DeflateRect(2,2);
	rColor.right = rColor.left + rColor.Height();
	rColor.bottom = rColor.top + rColor.Height();

	// draw color rectangle.
	pDC->FillSolidRect(rColor,
		bDisabled? GetXtremeColor(COLOR_3DFACE): pLbl->m_clrColor);

	pDC->Draw3dRect(rColor,
		GetXtremeColor(bDisabled? COLOR_GRAYTEXT: COLOR_WINDOWTEXT),
		GetXtremeColor(bDisabled? COLOR_GRAYTEXT: COLOR_WINDOWTEXT));

	// determine the size of the text display.
	CRect rText(rColor);
	rText.top -= 2;
	rText.bottom = rText.top + (::GetSystemMetrics(SM_CYVTHUMB)-::GetSystemMetrics(SM_CYEDGE));
	rText.left = rText.right + 4;
	rText.right = rcItem.right;

	// draw text.
	pDC->SetBkMode(TRANSPARENT);
	pDC->DrawText(pLbl->m_strName, rText, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
}

/////////////////////////////////////////////////////////////////////////////
CXTPCalendarEventPropertiesDlg::CXTPCalendarEventPropertiesDlg(CXTPCalendarControl* pControl,
										 BOOL bAddEvent, CWnd* pParent,
										 CXTPCalendarEvent* pEditingEvent, UINT nIDTemplate)
{
	InitModalIndirect(XTPResourceManager()->LoadDialogTemplate(nIDTemplate), pParent);

	m_nAllDayEventSet = -1;

	m_bAddEvent = bAddEvent;

	m_bAllDayEvent = FALSE;
	m_bPrivate = FALSE;
	m_bMeeting = FALSE;
	m_bReminder = FALSE;
	m_nBusyStatus = xtpCalendarBusyStatusUnknown;

	m_bOccurrence = TRUE;

	CXTPCalendarData* pData = pControl ? pControl->GetDataProvider() : NULL;
	if (!pControl || !pData)
	{
		ASSERT(FALSE);
		return;
	}
	m_ptrControl = pControl;

	m_bCategoriesEnabled = XTPCalendarIsThemeOffice2007(m_ptrControl);

	m_ptrEditingEvent.SetPtr(pEditingEvent, TRUE);
	if (!pEditingEvent)
	{
		m_ptrEditingEvent = pData->CreateNewEvent();
		if (m_ptrEditingEvent)
		{
			CXTPCalendarView* pView = m_ptrControl->GetActiveView();
			ASSERT(pView);

			if (pView)
			{
				COleDateTime dtSelBegin, dtSelEnd;
				BOOL bAllDayEvent = FALSE;
				int nGroupIndex = 0;

				if (!pView->GetSelection(&dtSelBegin, &dtSelEnd, &bAllDayEvent, &nGroupIndex))
				{
					dtSelBegin = COleDateTime::GetCurrentTime();
					dtSelEnd = dtSelBegin + COleDateTimeSpan(0,1, 0,0);
					nGroupIndex = 0;
				}
				UINT uSchID = XTP_SAFE_GET3(pView, GetViewDay_(0), GetViewGroup_(nGroupIndex), GetScheduleID(), XTP_CALENDAR_UNKNOWN_SCHEDULE_ID);

				m_ptrEditingEvent->SetStartTime(min(dtSelBegin, dtSelEnd));
				m_ptrEditingEvent->SetEndTime(max(dtSelBegin, dtSelEnd));
				m_ptrEditingEvent->SetScheduleID(uSchID);

				m_nAllDayEventSet = bAllDayEvent ? 1 : 0;

				if (!bAllDayEvent)
					m_ptrEditingEvent->SetBusyStatus(xtpCalendarBusyStatusBusy);
			}
		}
	}

	if (m_ptrEditingEvent)
	{
		m_dtStartDate = m_ptrEditingEvent->GetStartTime();
		m_dtStartTime = m_dtStartDate;

		m_dtEndDate = m_ptrEditingEvent->GetEndTime();
		m_dtEndTime = m_dtEndDate;

		if (m_ptrEditingEvent->IsAllDayEvent() && CXTPCalendarUtils::IsZeroTime(m_dtEndDate))
		{
			AddEndDate_ResetTime(m_dtStartDate, m_dtEndDate, -1);
		}
	}


}


#if _MSC_VER < 1200
AFX_INLINE void DDX_DateTimeCtrl(CDataExchange* pDX, int nIDC, COleDateTime& value)
{
	HWND hWndCtrl = pDX->PrepareCtrl(nIDC);
	if (pDX->m_bSaveAndValidate)
		CXTPDateTimeHelper::GetDateTimeCtrlTime(hWndCtrl, value);
	else
		CXTPDateTimeHelper::SetDateTimeCtrlTime(hWndCtrl, value);
}
#endif

void CXTPCalendarEventPropertiesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, XTP_IDC_COMBO_SCHEDULEID, m_ctrlScheduleIDCB);
	DDX_Control(pDX, XTP_IDC_SCHEDULEID_TEXT, m_ctrlScheduleLabel);
	DDX_Control(pDX, XTP_IDC_COMBO_REMINDER, m_ctrlReminderCB);
	DDX_Control(pDX, XTP_IDC_COMBO_BUSYSTATE, m_ctrlBusyStatusCB);
	DDX_Text(pDX, XTP_IDC_EDIT_SUBJECT, m_strSubject);
	DDX_Text(pDX, XTP_IDC_EDIT_LOCATION, m_strLocation);
	DDX_Text(pDX, XTP_IDC_EDIT_BODY, m_strBody);
	DDX_DateTimeCtrl(pDX, XTP_IDC_DTP_START_DATE, m_dtStartDate);
	DDX_DateTimeCtrl(pDX, XTP_IDC_DTP_START_TIME, m_dtStartTime);
	DDX_DateTimeCtrl(pDX, XTP_IDC_DTP_END_DATE, m_dtEndDate);
	DDX_DateTimeCtrl(pDX, XTP_IDC_DTP_END_TIME, m_dtEndTime);

	DDX_CBIndex(pDX, XTP_IDC_COMBO_BUSYSTATE, m_nBusyStatus);

	DDX_Check(pDX, XTP_IDC_CHECK_ALLDAY_EVENT, m_bAllDayEvent);
	DDX_Check(pDX, XTP_IDC_CHECK_PRIVATE, m_bPrivate);
	DDX_Check(pDX, XTP_IDC_CHECK_MEETING, m_bMeeting);
	DDX_Check(pDX, XTP_IDC_CHECK_REMINDER, m_bReminder);
}


BEGIN_MESSAGE_MAP(CXTPCalendarEventPropertiesDlg, CDialog)
	ON_WM_CREATE()
	ON_BN_CLICKED(XTP_IDC_CHECK_REMINDER, OnCheckReminder)
	ON_BN_CLICKED(XTP_IDC_BUTTON_RECURRECNCE, OnBnClickedButtonRecurrecnce)
	ON_BN_CLICKED(XTP_IDC_CHECK_ALLDAY_EVENT, OnBnClickedCheckAlldayEvent)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXTPCalendarEventPropertiesDlg message handlers

void CXTPCalendarEventPropertiesDlg::OnOK()
{
	UpdateData();

	CString strReminder;
	m_ctrlReminderCB.GetWindowText(strReminder);
	int nReminderMinutes = CXTPCalendarUtils::ParceTimeDuration_min(strReminder);

	if (nReminderMinutes == INT_MIN)
	{
		m_ctrlReminderCB.SetFocus();
		XTPResourceManager()->ShowMessageBox(XTP_IDS_CALENDAR_WRONG_VALUE);
		return;
	}

	BOOL bChanged = FALSE;

	if (m_ptrEditingEvent->GetRecurrenceState() == xtpCalendarRecurrenceOccurrence)
	{
		VERIFY(m_ptrEditingEvent->MakeAsRException());
	}

	if (m_strSubject.Compare(m_ptrEditingEvent->GetSubject()))
	{
		m_ptrEditingEvent->SetSubject(m_strSubject);
		bChanged = TRUE;
	}
	if (m_strLocation.Compare(m_ptrEditingEvent->GetLocation()))
	{
		m_ptrEditingEvent->SetLocation(m_strLocation);
		bChanged = TRUE;
	}
	if (m_strBody.Compare(m_ptrEditingEvent->GetBody()))
	{
		m_ptrEditingEvent->SetBody(m_strBody);
		bChanged = TRUE;
	}

	if (m_bOccurrence)
	{
		COleDateTime dtStartTime(CXTPCalendarUtils::UpdateTime(m_dtStartDate,
															   m_dtStartTime));
		COleDateTime dtEndTime(CXTPCalendarUtils::UpdateTime(m_dtEndDate, m_dtEndTime));

		if (m_bAllDayEvent && CXTPCalendarUtils::IsZeroTime(m_dtEndDate))
		{
			AddEndDate_ResetTime(m_dtStartDate, dtEndTime, 1);
		}

		if (dtStartTime != m_ptrEditingEvent->GetStartTime())
		{
			m_ptrEditingEvent->SetStartTime(dtStartTime);
			bChanged = TRUE;
		}
		if (dtEndTime != m_ptrEditingEvent->GetEndTime())
		{
			m_ptrEditingEvent->SetEndTime(dtEndTime);
			bChanged = TRUE;
		}

		if (m_bAllDayEvent != m_ptrEditingEvent->IsAllDayEvent())
		{
			m_ptrEditingEvent->SetAllDayEvent(m_bAllDayEvent);
			bChanged = TRUE;
		}
	}
	else
	{
		//m_ptrEditingEvent->SetAllDayEvent(FALSE);
		m_ptrEditingEvent->SetAllDayEvent(m_bAllDayEvent);
	}

	if (m_nBusyStatus != m_ptrEditingEvent->GetBusyStatus())
	{
		m_ptrEditingEvent->SetBusyStatus(m_nBusyStatus);
		bChanged = TRUE;
	}

	if (m_bCategoriesEnabled)
	{
		m_ctrlCategories.GetChecked(m_ptrEditingEvent->GetCategories(), &bChanged);
	}
	else
	{
		if (m_ctrlLabelCB.GetLabelID() != m_ptrEditingEvent->GetLabelID())
		{
			m_ptrEditingEvent->SetLabelID(m_ctrlLabelCB.GetLabelID());
			bChanged = TRUE;
		}
	}

	if (m_bPrivate != m_ptrEditingEvent->IsPrivate())
	{
		m_ptrEditingEvent->SetPrivate(m_bPrivate);
		bChanged = TRUE;
	}

	if (m_bMeeting != m_ptrEditingEvent->IsMeeting())
	{
		m_ptrEditingEvent->SetMeeting(m_bMeeting);
		bChanged = TRUE;
	}

	if (m_bReminder != m_ptrEditingEvent->IsReminder())
	{
		m_ptrEditingEvent->SetReminder(m_bReminder);
		bChanged = TRUE;
	}

	if (m_bReminder)
	{
		if (nReminderMinutes != m_ptrEditingEvent->GetReminderMinutesBeforeStart())
		{
			m_ptrEditingEvent->SetReminderMinutesBeforeStart(nReminderMinutes);
			bChanged = TRUE;
		}
	}

	int nSelSch = m_ctrlScheduleIDCB.GetCurSel();
	if (nSelSch != CB_ERR)
	{
		UINT uSchID = (UINT)m_ctrlScheduleIDCB.GetItemData(nSelSch);
		if (uSchID != m_ptrEditingEvent->GetScheduleID())
		{
			m_ptrEditingEvent->SetScheduleID(uSchID);
			bChanged = TRUE;
		}
	}

	// Update event in data storage
	CXTPCalendarControl::CUpdateContext updateContext(m_ptrControl, xtpCalendarUpdateLayout | xtpCalendarUpdateRedraw);

	CXTPCalendarData* pData = m_ptrEditingEvent->GetDataProvider();
	if (m_bAddEvent)
	{
		VERIFY(pData->AddEvent(m_ptrEditingEvent));
	} else {
		VERIFY(pData->ChangeEvent(m_ptrEditingEvent));
	}

	m_ptrControl->Populate();

	CDialog::OnOK();
}

void CXTPCalendarEventPropertiesDlg::OnCancel()
{
	CDialog::OnCancel();
}

void CXTPCalendarEventPropertiesDlg::UpdateControlsFromEvent()
{
	ASSERT(m_ptrEditingEvent);

	m_strSubject = m_ptrEditingEvent->GetSubject();
	m_strLocation = m_ptrEditingEvent->GetLocation();
	m_strBody = m_ptrEditingEvent->GetBody();

	if (m_bCategoriesEnabled)
	{
		if (m_ptrEditingEvent->GetDataProvider())
			m_ctrlCategories.Init(m_ptrEditingEvent->GetDataProvider()->GetEventCategories(),
								  m_ptrEditingEvent->GetCategories());
	}
	else
	{
		if (m_ptrEditingEvent->GetDataProvider())
			m_ctrlLabelCB.Init(m_ptrEditingEvent->GetDataProvider()->GetLabelList());

		m_ctrlLabelCB.SetLabelID(m_ptrEditingEvent->GetLabelID());
	}


	if (m_nAllDayEventSet < 0)
	{
		m_bAllDayEvent = m_ptrEditingEvent->IsAllDayEvent();
	}
	else
	{
		m_bAllDayEvent = m_nAllDayEventSet != 0;

		if (m_bAllDayEvent && CXTPCalendarUtils::IsZeroTime(m_dtEndDate))
		{
			AddEndDate_ResetTime(m_dtStartDate, m_dtEndDate, -1);
		}
	}

	m_bPrivate = m_ptrEditingEvent->IsPrivate();
	m_bMeeting = m_ptrEditingEvent->IsMeeting();
	m_nBusyStatus = m_ptrEditingEvent->GetBusyStatus();

	m_bReminder = m_ptrEditingEvent->IsReminder();

	int nMinutes = m_ptrEditingEvent->GetReminderMinutesBeforeStart();
	CString strReminder = CXTPCalendarUtils::FormatTimeDuration(nMinutes, FALSE);

	m_ctrlReminderCB.SetWindowText(strReminder);

	//-----------------------------------------------
	CXTPCalendarSchedules* pSchedules = m_ptrEditingEvent->GetDataProvider()->GetSchedules();
	ASSERT(pSchedules);

	BOOL bSchEnabled = pSchedules && pSchedules->GetCount();

	m_ctrlScheduleIDCB.EnableWindow(bSchEnabled);
	m_ctrlScheduleLabel.EnableWindow(bSchEnabled);

	if (pSchedules)
	{
		m_ctrlScheduleIDCB.ResetContent();

		int nIdx = m_ctrlScheduleIDCB.AddString(_T("-"));
		m_ctrlScheduleIDCB.SetItemData(nIdx, (DWORD_PTR)XTP_CALENDAR_UNKNOWN_SCHEDULE_ID);

		int nCount = pSchedules->GetCount();
		for (int nSchNr = 0; nSchNr < nCount; nSchNr++)
		{
			CXTPCalendarSchedule* pSch = pSchedules->GetAt(nSchNr);
			ASSERT(pSch);
			if (pSch)
			{
				nIdx = m_ctrlScheduleIDCB.AddString(pSch->GetName());
				m_ctrlScheduleIDCB.SetItemData(nIdx, (DWORD_PTR)pSch->GetID());
			}
		}

		UINT uSchID = m_ptrEditingEvent->GetScheduleID();
		nCount = m_ctrlScheduleIDCB.GetCount();

		if (m_bAddEvent && nCount)
		{
			m_ctrlScheduleIDCB.SetCurSel(0);
		}

		for (int nSchIDCS = 0; nSchIDCS < nCount; nSchIDCS++)
		{
			if (m_ctrlScheduleIDCB.GetItemData(nSchIDCS) == uSchID)
			{
				m_ctrlScheduleIDCB.SetCurSel(nSchIDCS);
				break;
			}
		}
	}

	UpdateData(FALSE);


}

void CXTPCalendarEventPropertiesDlg::UpdateControlsState()
{
	if (GetDlgItem(XTP_IDC_DTP_START_DATE))
	{
		GetDlgItem(XTP_IDC_DTP_START_DATE)->ShowWindow(m_bOccurrence ? SW_SHOW : SW_HIDE);
	}
	if (GetDlgItem(XTP_IDC_DTP_START_TIME))
	{
		GetDlgItem(XTP_IDC_DTP_START_TIME)->ShowWindow(m_bOccurrence && !m_bAllDayEvent ? SW_SHOW : SW_HIDE);
	}
	if (GetDlgItem(XTP_IDC_DTP_END_DATE))
	{
		GetDlgItem(XTP_IDC_DTP_END_DATE)->ShowWindow(m_bOccurrence ? SW_SHOW : SW_HIDE);
	}
	if (GetDlgItem(XTP_IDC_DTP_END_TIME))
	{
		GetDlgItem(XTP_IDC_DTP_END_TIME)->ShowWindow(m_bOccurrence && !m_bAllDayEvent ? SW_SHOW : SW_HIDE);
	}
	if (GetDlgItem(XTP_IDC_START_TEXT))
	{
		GetDlgItem(XTP_IDC_START_TEXT)->ShowWindow(m_bOccurrence ? SW_SHOW : SW_HIDE);
	}
	if (GetDlgItem(XTP_IDC_END_TEXT))
	{
		GetDlgItem(XTP_IDC_END_TEXT)->ShowWindow(m_bOccurrence ? SW_SHOW : SW_HIDE);
	}

	m_ctrlReminderCB.EnableWindow(m_bReminder);
}

void CXTPCalendarEventPropertiesDlg::InitReminderComboBox()
{
	m_ctrlReminderCB.ResetContent();

	m_ctrlReminderCB.AddString(LoadString(XTP_IDS_CALENDAR_MINUTES_0));
	m_ctrlReminderCB.AddString(LoadString(XTP_IDS_CALENDAR_MINUTES_1));
	m_ctrlReminderCB.AddString(LoadString(XTP_IDS_CALENDAR_MINUTES_5));
	m_ctrlReminderCB.AddString(LoadString(XTP_IDS_CALENDAR_MINUTES_10));
	m_ctrlReminderCB.AddString(LoadString(XTP_IDS_CALENDAR_MINUTES_15));
}

void CXTPCalendarEventPropertiesDlg::InitBusyStatusComboBox()
{
	m_ctrlBusyStatusCB.ResetContent();

	m_ctrlBusyStatusCB.AddString(LoadString(XTP_IDS_CALENDAR_TIME_STATUS_FREE));
	m_ctrlBusyStatusCB.AddString(LoadString(XTP_IDS_CALENDAR_TIME_STATUS_TENTATIVE));
	m_ctrlBusyStatusCB.AddString(LoadString(XTP_IDS_CALENDAR_TIME_STATUS_BUSY));
	m_ctrlBusyStatusCB.AddString(LoadString(XTP_IDS_CALENDAR_TIME_STATUS_OUTOFOFFICE));
}


BOOL CXTPCalendarEventPropertiesDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	if (m_bCategoriesEnabled)
	{
		m_ctrlCategories.SubclassDlgItem(XTP_IDC_COMBO_LABEL, this);

		if (GetDlgItem(XTP_IDC_LABEL))
		{
			CString str = XTPResourceManager()->LoadString(XTP_IDS_CALENDAR_CATEGORIES_LABEL);
			GetDlgItem(XTP_IDC_LABEL)->SetWindowText(str + _T(":"));
		}
	}
	else
	{
		m_ctrlLabelCB.SubclassDlgItem(XTP_IDC_COMBO_LABEL, this);
	}

	InitReminderComboBox();

	InitBusyStatusComboBox();

	UpdateControlsFromEvent();
	UpdateControlsState();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CXTPCalendarEventPropertiesDlg::OnBnClickedButtonRecurrecnce()
{
	UpdateData();

	BOOL bWasNotRecur = FALSE;
	BOOL bUpdateFromEvent = FALSE;

	CXTPCalendarEventPtr ptrMasterEvent = m_ptrEditingEvent;
	int nRState = ptrMasterEvent->GetRecurrenceState();

	if (nRState != xtpCalendarRecurrenceMaster)
	{
		ptrMasterEvent = m_ptrEditingEvent->CloneEvent();

		if (nRState == xtpCalendarRecurrenceNotRecurring)
		{
			VERIFY(ptrMasterEvent->MakeEventAsRecurrence());
			bWasNotRecur = TRUE;

			CXTPCalendarRecurrencePatternPtr ptrPattern;
			ptrPattern = ptrMasterEvent->GetRecurrencePattern();

			COleDateTime dtStartDateTime(CXTPCalendarUtils::UpdateTime(m_dtStartDate, m_dtStartTime));
			COleDateTime dtEndDateTime(CXTPCalendarUtils::UpdateTime(m_dtEndDate, m_dtEndTime));

			COleDateTimeSpan spDuration = dtEndDateTime- dtStartDateTime;

			ptrPattern->SetStartTime(CXTPCalendarUtils::ResetDate(m_dtStartTime));
			ptrPattern->SetDurationMinutes((int) spDuration.GetTotalMinutes());

			ptrPattern->SetPatternStartDate(CXTPCalendarUtils::ResetTime(m_dtStartDate));

			ptrMasterEvent->UpdateRecurrence(ptrPattern);
		}
		else
		{
			bUpdateFromEvent = TRUE;

			ASSERT(nRState == xtpCalendarRecurrenceOccurrence ||
				   nRState == xtpCalendarRecurrenceException);

			CXTPCalendarRecurrencePatternPtr ptrPattern;
			ptrPattern = m_ptrEditingEvent->GetRecurrencePattern();
			ptrMasterEvent = ptrPattern->GetMasterEvent();
		}
	}

	CXTPCalendarEventRecurrenceDlg dlg(ptrMasterEvent, this);
	dlg.m_bDisableRemove = bWasNotRecur;

	int nRes = (int)dlg.DoModal();
	if (nRes == IDOK || nRes == XTP_CALENDAR_DLGRESULT_REMOVE_RECURRENCE)
	{
		m_ptrEditingEvent = ptrMasterEvent;

		m_bOccurrence = m_ptrEditingEvent->GetRecurrenceState() ==
						xtpCalendarRecurrenceNotRecurring;

		if (bUpdateFromEvent)
		{
			UpdateData();
			UpdateControlsFromEvent();
		}

		UpdateControlsState();
	}
}

int CXTPCalendarEventPropertiesDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	int nType = m_ptrEditingEvent->GetRecurrenceState();

	if (nType == xtpCalendarRecurrenceOccurrence || nType == xtpCalendarRecurrenceException)
	{
		CXTPCalendarOccurSeriesChooseDlg dlg(0, XTP_IDS_CALENDAR_OCURR_SERIES_OPEN);
		dlg.SetEvent(m_ptrEditingEvent);

		if (dlg.DoModal() != IDOK)
			return -1;

		m_bOccurrence = dlg.m_bOccur;
		if (!m_bOccurrence)
		{
			CXTPCalendarRecurrencePatternPtr ptrPatternRO;
			ptrPatternRO = m_ptrEditingEvent->GetRecurrencePattern();
			CXTPCalendarEventPtr ptrMasterRO = ptrPatternRO->GetMasterEvent();

			m_ptrEditingEvent = ptrMasterRO->CloneEvent();
		}
	}

	return 0;
}

void CXTPCalendarEventPropertiesDlg::OnBnClickedCheckAlldayEvent()
{
	UpdateData();
	UpdateControlsState();

	if (m_bAllDayEvent && CXTPCalendarUtils::IsZeroTime(m_dtEndTime))
	{
		AddEndDate_ResetTime(m_dtStartDate, m_dtEndDate, -1);
		UpdateData(FALSE);
	}
	else if (!m_bAllDayEvent && CXTPCalendarUtils::IsZeroTime(m_dtEndTime))
	{
		AddEndDate_ResetTime(m_dtStartDate, m_dtEndDate, 1);
		UpdateData(FALSE);
	}
}


void CXTPCalendarEventPropertiesDlg::OnCheckReminder()
{
	UpdateData();
	UpdateControlsState();
}

void CXTPCalendarEventPropertiesDlg::AddEndDate_ResetTime(COleDateTime dtStartDate, COleDateTime& rdtEndDate, int nDays)
{
	COleDateTimeSpan spDiff = CXTPCalendarUtils::ResetTime(rdtEndDate) -
								CXTPCalendarUtils::ResetTime(dtStartDate);
	if (nDays > 0 || nDays < 0 && (DWORD)(double)spDiff >= (DWORD)(-1 * nDays) )
	{
		rdtEndDate = CXTPCalendarUtils::ResetTime(rdtEndDate) + COleDateTimeSpan(nDays,0,0,0);
	}
}
