#include "StdAfx.h"
#include "DnPartyMemberDlg.h"
#include "DnPartyTask.h"
#include "DnContextMenuDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnPartyMemberDlg::CDnPartyMemberDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback)
	: CDnCustomDlg(dialogType, pParentDialog, nID, pCallback)
{
	m_nCurSelectIndex = -1;
	m_pSelectBar = NULL;
}

void CDnPartyMemberDlg::InitialUpdate()
{
	m_pSelectBar = GetControl<CEtUIStatic>("ID_SELECT_BAR");
	m_pSelectBar->Show(false);
}

void CDnPartyMemberDlg::Process(float fElapsedTime)
{
	CEtUIDialog::Process(fElapsedTime);

	ProcessInvitaion();
}

void CDnPartyMemberDlg::ProcessInvitaion()
{
	if (CDnPartyTask::IsActive() && GetPartyTask().IsPartyMemberInviting())
	{
		if (ShowInvitationCtrl(true))
			m_bPartyMemberInviting = true;
	}
	else
	{
		if (m_bPartyMemberInviting)
		{
			ShowInvitationCtrl(false);
			m_bPartyMemberInviting = false;
		}
	}
}

bool CDnPartyMemberDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsShow() )
		return false;

	switch( uMsg )
	{
	case WM_LBUTTONDOWN:
	case WM_LBUTTONDBLCLK:
		{
			POINT MousePoint;
			float fMouseX, fMouseY;

			MousePoint.x = short( LOWORD( lParam ) );
			MousePoint.y = short( HIWORD( lParam ) );
			PointToFloat( MousePoint, fMouseX, fMouseY );

			int nFindSelect = FindInsideItem( fMouseX, fMouseY );
			if( nFindSelect > -1 )
			{
				m_nCurSelectIndex = nFindSelect;
				UpdateSelectBar();
			}
		}
		break;
	}

	return CEtUIDialog::MsgProc( hWnd, uMsg, wParam, lParam );
}

void CDnPartyMemberDlg::ShowPartyContextMenu(bool bShow, float fX, float fY, const SPartyMemberData& memberInfo)
{
	CDnContextMenuDlg* pDlg = GetInterface().GetContextMenuDlg(CDnInterface::CONTEXTM_PARTY);
	if (pDlg)
	{
		if (bShow)
		{
			CDnPartyContextMenuDlg::SContextMenuPartyInfo info;
			info.dwSessionID = memberInfo.GetSessionID();
			info.name = memberInfo.GetName();
			info.level = memberInfo.GetUserLevel();
			info.job = memberInfo.GetJobClass();

			pDlg->SetInfo(&info);
			pDlg->SetPosition(fX, fY);
		}
		pDlg->Show(bShow);
	}
}

void CDnPartyMemberDlg::UpdateSelectBar()
{
	SUICoord uiCoordNumber, uiCoordSelectBar;

	if (GetCurSelectIndex() < 0 || GetCurSelectIndex() >= NORMPARTYCOUNTMAX)
		return;

	CEtUIControl* pCtrl = GetCurSelectCtrl();
	if (pCtrl == NULL)
		return;

	pCtrl->GetUICoord(uiCoordNumber);
	m_pSelectBar->GetUICoord(uiCoordSelectBar);
#ifdef PRE_PARTY_DB
	uiCoordSelectBar.fX = uiCoordNumber.fX;
	uiCoordSelectBar.fY = uiCoordNumber.fY;
#else
	uiCoordSelectBar.fX = uiCoordNumber.fX-0.005f;//-0.016f;
	uiCoordSelectBar.fY = uiCoordNumber.fY-0.005f;//-0.018f;
#endif
	m_pSelectBar->SetUICoord(uiCoordSelectBar);
	m_pSelectBar->Show(true);
}


void CDnPartyMemberDlg::ResetCurSelectIndex()
{
	m_nCurSelectIndex = -1;
	m_pSelectBar->Show(false);
}