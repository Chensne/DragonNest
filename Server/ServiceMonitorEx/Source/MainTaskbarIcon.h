
#pragma once

#include "OXTaskbarIcon.h"


class CMainTaskbarIcon : public COXTaskbarIcon
{
public:
	CMainTaskbarIcon() { };
	void DoPopupMenu();
	// pointer to window to send messages
	CWnd* m_PopupOwner;
	
	//{{AFX_VIRTUAL(CMainTaskbarIcon)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CMainTaskbarIcon)
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

