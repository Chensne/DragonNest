// FileLogSplit.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "ServiceManagerEx.h"
#include "LogSplit.h"
#include "LogView.h"

IMPLEMENT_DYNAMIC(CLogSplit, CSplitterWnd)

CLogSplit::CLogSplit()
	: m_pLogView(NULL)
{
}

CLogSplit::~CLogSplit()
{
}

BEGIN_MESSAGE_MAP(CLogSplit, CSplitterWnd)
END_MESSAGE_MAP()

void CLogSplit::CreateSplit(CWnd* pParent, const RECT& rect, CCreateContext* pContext)
{
	CreateStatic(pParent, 2, 1);
	CreateView(1, 0, RUNTIME_CLASS(CLogView), CSize(rect.right, 0), pContext);
	SetRowInfo(0, rect.bottom / 3 * 2, 0);
	m_pLogView = (CLogView*)GetPane(1, 0);
}

void CLogSplit::ShowWnd(bool show)
{
	int cmd = (show) ? SW_SHOWNORMAL : SW_HIDE;

	if (m_pLogView)
		m_pLogView->ShowWindow(cmd);

	ShowWindow(cmd);
}

void CLogSplit::ResizeView(const RECT& rect)
{
	SetRowInfo(0, rect.bottom / 3 * 2, 0);
	RecalcLayout();
}

void CLogSplit::InsertLog(LogInfo& log)
{
	if (m_pLogView)
		m_pLogView->InsertLog(log);
}

void CLogSplit::SetCommand(const wstring& command)
{
	if (m_pLogView)
		m_pLogView->SetCommand(command);
}