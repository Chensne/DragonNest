#pragma once

#include "Define.h"
#include "OXLayoutManager.h"


class CSubControlBaseView : public CDialog
{
	DECLARE_DYNAMIC(CSubControlBaseView)

public:
	explicit CSubControlBaseView(EF_CONTROLTYPE pControlType, UINT nIDTemplate, CWnd* pParentWnd = NULL);
	virtual ~CSubControlBaseView();

public:
	EF_CONTROLTYPE GetControlType() const { return m_ControlType; }
	UINT GetIdTemplate() const { return m_nIDTemplate; }

	virtual void UpdateData() = 0;

protected:
	COXLayoutManager m_LayoutManager;

private:
	EF_CONTROLTYPE m_ControlType;
	UINT m_nIDTemplate;
	
};


class CWndDetachSafe
{
public:
	CWndDetachSafe(CWnd *pWnd, HWND hWnd);
	~CWndDetachSafe();

private:
	CWnd *m_Wnd;

};
