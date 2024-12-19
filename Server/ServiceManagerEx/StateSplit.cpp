#include "stdafx.h"
#include "ServiceManagerEx.h"
#include "StateSplit.h"
#include "DefaultStateView.h"
#include "StateView.h"

IMPLEMENT_DYNAMIC(CStateSplit, CSplitterWnd)

BEGIN_MESSAGE_MAP(CStateSplit, CSplitterWnd)
END_MESSAGE_MAP()

CStateSplit::CStateSplit()
	: m_pDefaultStateView(NULL), m_pStateView(NULL)
{

}

CStateSplit::~CStateSplit()
{
}

void CStateSplit::CreateSplit(CWnd* pParent, int paneId, const RECT& rect, CCreateContext* pContext)
{
	CreateStatic(pParent, 2, 1, WS_CHILD | WS_VISIBLE | WS_BORDER, paneId);
	CreateView(0, 0, RUNTIME_CLASS(CStateView), CSize(0, rect.bottom / 3 * 2 / 3 * 1), pContext);
	CreateView(1, 0, RUNTIME_CLASS(CDefaultStateView), CSize(0, 0), pContext);

	m_pStateView = (CStateView*)GetPane(0, 0);
	m_pDefaultStateView = (CDefaultStateView*)GetPane(1, 0);
}

void CStateSplit::ShowWnd(bool show)
{
	int cmd = (show) ? SW_SHOWNORMAL : SW_HIDE;

	ShowWindow(cmd);

	if (m_pStateView)
		m_pStateView->ShowWindow(cmd);

	if (m_pDefaultStateView)
		m_pDefaultStateView->ShowWindow(cmd);
}

void CStateSplit::RefreshView()
{
	if (m_pStateView)
		m_pStateView->RefreshView();

	if (m_pDefaultStateView)
		m_pDefaultStateView->RefreshView();
}

void CStateSplit::ResizeView(const RECT& rect)
{
	SetRowInfo(0, rect.bottom / 3 * 2 / 3 * 1, 0);
	RecalcLayout();
}