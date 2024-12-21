// LogList.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ServiceManagerEx.h"
#include "LogList.h"
#include "Log.h"
#include "LogFindDlg.h"
#include "LogBuilder.h"
#include "ServiceManager.h"
#include "ErrorCode.h"
#include <sstream>

extern CServiceManager* g_pServiceManager;

IMPLEMENT_DYNAMIC(CLogList, CListCtrl)

CLogList::CLogList(BYTE flags)
	: m_MenuFlags(flags), m_LineIndex(0)
{
}

CLogList::~CLogList()
{
	for each (const LogItem* p in m_Logs)
	{
		SAFE_DELETE(p);
	}
	m_Logs.clear();
	m_FindLines.clear();
	m_LineIndex = 0;
}

BEGIN_MESSAGE_MAP(CLogList, CListCtrl)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, &CLogList::OnNMCustomdraw)
	ON_COMMAND(ID_LIST_CLEAR, &CLogList::OnListClear)
	ON_COMMAND(ID_LIST_FIND, &CLogList::OnListFind)
	ON_COMMAND(ID_LIST_OPEN, &CLogList::OnListOpen)
	ON_WM_CONTEXTMENU()
	ON_WM_CHAR()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

void CLogList::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>(pNMHDR);
	*pResult = 0;

	if (pLVCD->nmcd.dwDrawStage == CDDS_PREPAINT)
	{
		*pResult = CDRF_NOTIFYITEMDRAW;
	}
	else if (pLVCD->nmcd.dwDrawStage == CDDS_ITEMPREPAINT)
	{
		LogInfo* pLog = (LogInfo*)pLVCD->nmcd.lItemlParam;
		if (pLog)
		{
			const ViewConfig* pViewConfig = ((CServiceManagerExApp*)::AfxGetApp())->GetConfigEx().GetViewConfig(L"Log");
			if (pViewConfig)
			{
				pLVCD->clrText = pViewConfig->GetFontColor(pLog->type);
				pLVCD->clrTextBk = pViewConfig->GetBgColor();
			}
			else
			{
				pLVCD->clrText = RGB(192, 192, 192);
				pLVCD->clrTextBk = RGB(0, 0, 0);
			}
		}

		*pResult = CDRF_DODEFAULT;
	}
}

void CLogList::OnListClear()
{
	if (IDYES != ::AfxMessageBox(L"Are you sure to clear list?", MB_ICONQUESTION | MB_YESNO))
		return;

	Clear();
}

void CLogList::OnListFind()
{
	CLogFindDlg dlg;
	dlg.DoModal();

	wstring findWhat = dlg.GetFindWhat();
	BYTE flags = dlg.GetOptionFlags();

	m_FindLines.clear();
	m_LineIndex = 0;

	for each (const LogItem* pLog in m_Logs)
	{
		wstring word = pLog->text;
		if ((flags & MATCH_CASE) != MATCH_CASE)
		{
			transform(findWhat.begin(), findWhat.end(), findWhat.begin(), towlower);
			transform(pLog->text.begin(), pLog->text.end(), word.begin(), towlower);
		}

		if (wstring::npos != word.find(findWhat))
			m_FindLines.push_back(pLog->index);
	}

	if (m_FindLines.empty())
	{
		CString message;
		message.Format(L"The following specified text was not found:\n\n%s", findWhat.c_str());
		::AfxMessageBox(message, MB_ICONASTERISK | MB_OK);
		SetFocus();
		return;
	}

	SelectLine(m_LineIndex);
}

void CLogList::OnListOpen()
{
	OpenFile();
}

void CLogList::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	CMenu menu;
	menu.LoadMenu(IDR_MENU_LOG);
	CMenu* pPopup = menu.GetSubMenu(0);

	if ((m_MenuFlags & MENU_FIND) != MENU_FIND)
		pPopup->DeleteMenu(ID_LIST_FIND, MF_BYCOMMAND);

	if ((m_MenuFlags & MENU_CLEAR) != MENU_CLEAR)
		pPopup->DeleteMenu(ID_LIST_CLEAR, MF_BYCOMMAND);

	if ((m_MenuFlags & MENU_OPEN) != MENU_OPEN)
		pPopup->DeleteMenu(ID_LIST_OPEN, MF_BYCOMMAND);

	pPopup->DeleteMenu(ID_LIST_CLIPBOARD, MF_BYCOMMAND);

	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
	menu.DestroyMenu();
}

void CLogList::Pop()
{
	if (m_Logs.empty())
		return;

	delete m_Logs.front();
	m_Logs.pop_front();
	DeleteItem(0);
}

void CLogList::Clear()
{
	for each (const LogItem* p in m_Logs)
	{
		delete p;
	}
	m_Logs.clear();

	DeleteAllItems();
	m_FindLines.clear();
	m_LineIndex = 0;
}

void CLogList::InsertLog(LogInfo& log)
{
	int count = GetItemCount();
	LogItem* pItem = new LogItem(log, count);

	LV_ITEM lvItem;
	lvItem.mask = LVIF_TEXT | LVIF_PARAM;
	lvItem.iItem = count;
	lvItem.iSubItem = 0;
	lvItem.pszText = const_cast<wchar_t*>(log.date.c_str());
	lvItem.lParam = (LPARAM)pItem;

	InsertItem(&lvItem);
	SetItemText(count, 1, log.type.c_str());

	size_t pos = log.text.rfind('\n');
	if (pos != wstring::npos)
		log.text = log.text.substr(0, pos);

	pos = log.text.rfind('\r');
	if (pos != wstring::npos)
		log.text = log.text.substr(0, pos);

	SetItemText(count, 2, log.text.c_str());
	EnsureVisible(count, FALSE);

	m_Logs.push_back(pItem);
}

void CLogList::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (nChar == 6)
	{
		OnListFind();
	}

	CListCtrl::OnChar(nChar, nRepCnt, nFlags);
}

void CLogList::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (!m_FindLines.empty())
	{
		switch (nChar)
		{
		case VK_F3:
			{
				if (m_LineIndex > 0)
					--m_LineIndex;

				SelectLine(m_LineIndex);
			}
			break;

		case VK_F4:
			{
				if (m_LineIndex < (int)m_FindLines.size() - 1)
					++m_LineIndex;

				SelectLine(m_LineIndex);
			}
			break;
		}
	}

	CListCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}

bool CLogList::OpenFile()
{
	CTime time;

	wchar_t filter[] = L"Log (ServiceManager*.log, ExceptionReport*.log)|ServiceManager*.log;ExceptionReport*.log|";
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, filter);
	if (IDOK != dlg.DoModal())
		return false;

	CString path = dlg.GetPathName();
	CLogBuilder builder;

	try
	{
		log_error error = builder.Load(path.GetBuffer());
		if (error == error_size_overflow)
		{
			AfxMessageBox(L"Cannot open file, because file is too large.", MB_ICONASTERISK);
			return false;
		}
	}
	catch (...)
	{
		return false;
	}

	Clear();

	const vector<LogInfo*>& logs = builder.GetLogs();
	for each (LogInfo* pLog in logs)
	{
		InsertLog(*pLog);
	}

	AfxGetMainWnd()->SetWindowText(L"ServiceManagerEx - " + path);

	return true;
}

void CLogList::SelectLine(int index)
{
	ASSERT(index >= 0 && index < (int)m_FindLines.size());
	int line = m_FindLines[index];

	SetItemState(-1, 0, LVIS_SELECTED | LVIS_FOCUSED);
	SetItemState(line, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
	EnsureVisible(line, FALSE);
	SetFocus();
}
