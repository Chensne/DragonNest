#pragma once

#include "LogInfo.h"

// CLogView 뷰입니다.
class CLogList;
class CCommander;

class CLogView : public CScrollView
{
	DECLARE_DYNCREATE(CLogView)

protected:
	CLogView();
	virtual ~CLogView();

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void OnDraw(CDC* pDC);      // 이 뷰를 그리기 위해 재정의되었습니다.
	virtual void OnInitialUpdate();     // 생성된 후 처음입니다.

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnSize(UINT nType, int cx, int cy);

protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

public:
	void InsertLog(LogInfo& log);
	void FlushLog();
	void SetCommand(const wstring& command);

private:
	CLogList* m_pList;
	vector<LogInfo> m_Logs;
	CCommander* m_pCmdLine;
	CButton* m_pCmdBtn;
};


