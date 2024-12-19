#include "StdAfx.h"
#include "SubControlBaseView.h"


IMPLEMENT_DYNAMIC(CSubControlBaseView, CDialog)

CSubControlBaseView::CSubControlBaseView(EF_CONTROLTYPE pControlType, UINT nIDTemplate, CWnd* pParentWnd)
	: CDialog(nIDTemplate, pParentWnd), m_ControlType(pControlType), m_nIDTemplate(nIDTemplate)
{
	ASSERT(0 <= m_ControlType && EV_CTT_CNT > m_ControlType);
}

CSubControlBaseView::~CSubControlBaseView()
{
	
}


CWndDetachSafe::CWndDetachSafe(CWnd *pWnd, HWND hWnd) : m_Wnd(pWnd)
{
	ASSERT(NULL != pWnd);
	ASSERT(NULL != hWnd);

	pWnd->Attach(hWnd);	
}

CWndDetachSafe::~CWndDetachSafe()
{
	if (m_Wnd) {
		m_Wnd->Detach();
	}
}

