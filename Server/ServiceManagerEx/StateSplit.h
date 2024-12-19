#pragma once

class CDefaultStateView;
class CStateView;

class CStateSplit : public CSplitterWnd
{
	DECLARE_DYNAMIC(CStateSplit)

public:
	CStateSplit();
	virtual ~CStateSplit();

protected:
	DECLARE_MESSAGE_MAP()

public:
	void CreateSplit(CWnd* pParent, int paneId, const RECT& rect, CCreateContext* pContext);
	void ShowWnd(bool show);
	void RefreshView();
	void ResizeView(const RECT& rect);

public:
	CDefaultStateView* GetDefaultStateView() { return m_pDefaultStateView; }
	CStateView* GetStateView() { return m_pStateView; }

private:
	CDefaultStateView* m_pDefaultStateView;
	CStateView* m_pStateView;
};


