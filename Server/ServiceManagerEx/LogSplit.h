#pragma once

#include "LogInfo.h"

class CLogView;

class CLogSplit : public CSplitterWnd
{
	DECLARE_DYNAMIC(CLogSplit)

public:
	CLogSplit();
	virtual ~CLogSplit();

protected:
	DECLARE_MESSAGE_MAP()

public:
	void CreateSplit(CWnd* pParent, const RECT& rect, CCreateContext* pContext);
	void ShowWnd(bool show);
	void ResizeView(const RECT& rect);
	void InsertLog(LogInfo& log);
	void SetCommand(const wstring& command);

public:
	CLogViewDisplayer* CreateDisplayer();

private:
	CLogView* m_pLogView;
};


