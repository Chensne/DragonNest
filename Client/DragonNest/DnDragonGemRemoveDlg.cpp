#include "stdafx.h"

#ifdef PRE_ADD_DRAGON_GEM
#include "DnDragonGemRemoveDlg.h"

CDnDragonGemRemoveDlg::CDnDragonGemRemoveDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog(dialogType, pParentDialog, nID, pCallback)
{
}

CDnDragonGemRemoveDlg::~CDnDragonGemRemoveDlg()
{
}

void CDnDragonGemRemoveDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize(CEtResourceMng::GetInstance().GetFullName("invenitemjewelcleandlg.ui").c_str(), bShow);
}

void CDnDragonGemRemoveDlg::InitialUpdate()
{

}

void CDnDragonGemRemoveDlg::Show(bool bShow)
{
	if (bShow == m_bShow)
		return;

	CEtUIDialog::Show(bShow);
}

bool CDnDragonGemRemoveDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if(!IsShow())
		return false;

	return CEtUIDialog::MsgProc(hWnd, uMsg, wParam, lParam);
}

void CDnDragonGemRemoveDlg::Process( float fElapsedTime )
{
	CEtUIDialog::Process(fElapsedTime);
}

void CDnDragonGemRemoveDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName(pControl->GetControlName());

	CEtUIDialog::ProcessCommand(nCommand, bTriggeredByUser, pControl, uMsg);
}

#endif // PRE_ADD_DRAGON_GEM
