#include "stdafx.h"
#include "ServiceManagerEx.h"
#include "MainSplit.h"
#include "ManagerView.h"
#include "StateView.h"

IMPLEMENT_DYNAMIC(CMainSplit, CSplitterWnd)

BEGIN_MESSAGE_MAP(CMainSplit, CSplitterWnd)
END_MESSAGE_MAP()

CMainSplit::CMainSplit()
	: m_pManagerView(NULL)
{
}

CMainSplit::~CMainSplit()
{
}

void CMainSplit::CreateSplit(CWnd* pParent, int paneId, const RECT& rect, CCreateContext* pContext)
{
	CreateStatic(pParent, 1, 2, WS_CHILD | WS_VISIBLE | WS_BORDER, paneId);
	CreateView(0, 0, RUNTIME_CLASS(CManagerView), CSize(rect.right / 3 * 2, 0), pContext);
	m_pManagerView = (CManagerView*)GetPane(0, 0);
}

void CMainSplit::ShowWnd(bool show)
{
	int cmd = (show) ? SW_SHOWNORMAL : SW_HIDE;

	ShowWindow(cmd);

	if (m_pManagerView)
		m_pManagerView->ShowWindow(cmd);
}

void CMainSplit::BuildView()
{
	if (m_pManagerView)
		m_pManagerView->BuildView();
}

void CMainSplit::RefreshView()
{
	if (m_pManagerView)
		m_pManagerView->RefreshView();
}

void CMainSplit::SetManagerViewType(ViewType type)
{
	if (m_pManagerView)
		m_pManagerView->SetViewType(type);
}

void CMainSplit::ResizeView(const RECT& rect)
{
	SetColumnInfo(0, rect.right / 3 * 2, 0);
	RecalcLayout();
}

void CMainSplit::Update()
{
	static DWORD prevTick = ::GetTickCount();
	DWORD curTick = ::GetTickCount();
	if (curTick - prevTick < 100)
		return;

	prevTick = curTick;

	if (m_pManagerView)
		m_pManagerView->Update();
}

void CMainSplit::OnPatchStart()
{
	if (!m_pManagerView)
	{
		ASSERT(0);
		return;
	}

	m_pManagerView->OnPatchStart();
}

void CMainSplit::OnPatchProgress(int id, const wchar_t* key, unsigned long progress, unsigned long progressMax)
{
	if (!m_pManagerView)
	{
		ASSERT(0);
		return;
	}

	m_pManagerView->OnPatchProgress(id, key, progress, progressMax);
}

void CMainSplit::OnPatchEnd(bool succeeded)
{
	if (!m_pManagerView)
	{
		ASSERT(0);
		return;
	}

	m_pManagerView->OnPatchEnd(succeeded);
}

void CMainSplit::OnUnzipProgress(int id, const wchar_t* filename, unsigned long progress, unsigned long progressMax)
{
	if (!m_pManagerView)
	{
		ASSERT(0);
		return;
	}

	m_pManagerView->OnUnzipProgress(id, filename, progress, progressMax);
}

void CMainSplit::OnWorldMaxUser(int id, int maxUser)
{
	if (!m_pManagerView)
	{
		ASSERT(0);
		return;
	}

	m_pManagerView->OnWorldMaxUser(id, maxUser);
}

void CMainSplit::OnPatchFail(int id, const wchar_t* msg)
{
	if (!m_pManagerView)
	{
		ASSERT(0);
		return;
	}

	m_pManagerView->OnPatchFail(id, msg);
}

void CMainSplit::OnPatchCompleted(int id)
{
	if (!m_pManagerView)
	{
		ASSERT(0);
		return;
	}

	m_pManagerView->OnPatchCompleted(id);
}

void CMainSplit::OnCommandPatch()
{
	m_pManagerView->OnCommandPatch();
}