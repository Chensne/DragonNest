#include "stdafx.h"
#include "ServiceMonitor.h"
#include "MainTaskbarIcon.h"
#include "MainFrm.h"

#include "resource.h"		// main symbols

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainTaskbarIcon

BEGIN_MESSAGE_MAP(CMainTaskbarIcon, COXTaskbarIcon)
	//{{AFX_MSG_MAP(CMainTaskbarIcon)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CMainTaskbarIcon::OnRButtonUp(UINT nFlags, CPoint point) 
{
	UNREFERENCED_PARAMETER(nFlags);
	UNREFERENCED_PARAMETER(point);
	// on right click show menu
	DoPopupMenu();
}

void CMainTaskbarIcon::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	UNREFERENCED_PARAMETER(nFlags);
	UNREFERENCED_PARAMETER(point);

	// on double click restore or minimize the position of window on screen

	WINDOWPLACEMENT aWindowPlacement;

	CMainFrame* aMainFrame = static_cast<CServiceMonitorApp*>(::AfxGetApp())->GetMainFrame();

	BOOL aRetVal = ::GetWindowPlacement(aMainFrame->GetSafeHwnd(), &aWindowPlacement);
	if (aRetVal) {
		if (SW_MINIMIZE == aWindowPlacement.showCmd ||
			SW_SHOWMINIMIZED == aWindowPlacement.showCmd ||
			SW_FORCEMINIMIZE == aWindowPlacement.showCmd
			)
		{
			::PostMessage(m_PopupOwner->m_hWnd, WM_COMMAND, ID_POPUP_RESTORE, (LPARAM)m_hWnd);
		}
		else {
			::PostMessage(m_PopupOwner->m_hWnd, WM_COMMAND, ID_POPUP_MINIMIZE, (LPARAM)m_hWnd);
		}
	}
	else {
		ASSERT(0);
	}
}

void CMainTaskbarIcon::DoPopupMenu()
{
	CMenu aPopMenu;
	VERIFY(aPopMenu.LoadMenu(IDR_TASKBAR_POPUP));

	// set default item
	MENUITEMINFO aItemInfo = { sizeof(MENUITEMINFO) };
	aItemInfo.fMask = MIIM_STATE;
	aItemInfo.fState = MFS_DEFAULT;
	::SetMenuItemInfo(aPopMenu.m_hMenu, ID_POPUP_RESTORE, FALSE, &aItemInfo);
	
	CPoint aPosMouse;
	GetCursorPos(&aPosMouse);
	
	// show menu
	m_PopupOwner->SendMessage(WM_NCACTIVATE, FALSE);
	aPopMenu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, aPosMouse.x, aPosMouse.y, m_PopupOwner);
}

