#pragma once

#include "LogInfo.h"

////////////////////////////////////////////////////////////
// menu flags
#define MENU_FIND		(BYTE)0x01
#define MENU_CLEAR		(BYTE)0x02
#define MENU_OPEN		(BYTE)0x04
#define MENU_CLIPBOARD	(BYTE)0x08

#define MENU_LOG		(MENU_FIND | MENU_CLEAR)
#define MENU_FILE_LOG	(MENU_FIND | MENU_OPEN | MENU_CLIPBOARD)

#define MENU_ALL		(BYTE)0xff
////////////////////////////////////////////////////////////

class LogItem : public LogInfo
{
public:
	LogItem(const wstring& date, const wstring& type, const wstring& log, int idx)
		: LogInfo(date, type, log), index(idx)
	{
	}

	LogItem(const LogInfo& log, int idx)
		: LogInfo(log), index(idx)
	{
	}

public:
	int index;
};

class CLogList : public CListCtrl
{
	DECLARE_DYNAMIC(CLogList)

public:
	CLogList(BYTE flags);
	virtual ~CLogList();

protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnListClear();
	afx_msg void OnListFind();
	afx_msg void OnListOpen();
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

public:
	void InsertLog(LogInfo& log);
	void Pop();
	void Clear();
	bool OpenFile();

private:
	void SelectLine(int index);

private:
	BYTE m_MenuFlags;
	list<LogItem*> m_Logs;
	vector<int> m_FindLines;
	int m_LineIndex;
};
