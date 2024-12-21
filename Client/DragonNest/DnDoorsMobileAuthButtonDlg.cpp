#include "StdAfx.h"

#ifdef PRE_ADD_DOORS

#include "DnDoorsMobileAuthButtonDlg.h"
#include "DnDoorsMobileMessageBoxDlg.h"
#include "DnInterface.h"
#include "TaskManager.h"
#include "DnBridgeTask.h"
#include "DnFadeInOutDlg.h"

CDnDoorsMobileAuthButtonDlg::CDnDoorsMobileAuthButtonDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback)
	: CEtUIDialog(dialogType, pParentDialog, nID, pCallback)
{
	m_pAuthBtn = NULL;
	m_pAuthMessageBox = NULL;
}

CDnDoorsMobileAuthButtonDlg::~CDnDoorsMobileAuthButtonDlg(void)
{
	SAFE_DELETE(m_pAuthMessageBox);
}

void CDnDoorsMobileAuthButtonDlg::Initialize(bool bShow)
{
	CEtUIDialog::Initialize(CEtResourceMng::GetInstance().GetFullName("MobileBTDlg.ui").c_str(), bShow);
	m_pAuthMessageBox = new CDnDoorsMobileMessageBoxDlg(UI_TYPE_MODAL, this);
	m_pAuthMessageBox->Initialize(false);
	m_pAuthMessageBox->SetParentAuthDlg(this);
}

void CDnDoorsMobileAuthButtonDlg::InitialUpdate()
{
	m_pAuthBtn = GetControl<CEtUIButton>("ID_BT_MOBILE");
#ifdef PRE_REMOVE_DOORS_UITEMP
	if (m_pAuthBtn)
		m_pAuthBtn->Show(false);
#endif
}

void CDnDoorsMobileAuthButtonDlg::Process(float fElapsedTime)
{
	if( m_bShow ) {
		CDnLoginTask *pTask = (CDnLoginTask *)CTaskManager::GetInstance().GetTask( "LoginTask" );
		if( pTask ) {
			bool bEnable = true;
			if( pTask->IsRequestWait() ) bEnable = false;
			if( GetInterface().GetFadeDlg() && GetInterface().GetFadeDlg()->GetFadeMode() != CDnFadeInOutDlg::modeEnd ) bEnable = false;
			m_pAuthBtn->Enable( bEnable );
		}
	}
	CEtUIDialog::Process( fElapsedTime );
}

void CDnDoorsMobileAuthButtonDlg::ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg)
{
	SetCmdControlName(pControl->GetControlName());

	if (nCommand == EVENT_BUTTON_CLICKED)
	{
		if (IsCmdControl("ID_BT_MOBILE"))
		{
#ifdef PRE_REMOVE_DOORS_UITEMP
#else
			m_pAuthBtn->Enable(false);

			SendDoorsMobileReqAuthFlag();
#endif
		}
	}
}

void CDnDoorsMobileAuthButtonDlg::ShowDoorsMobileMsgBoxDlg(bool bShow, DoorsMobileMsgBoxDlgType type)
{
#ifdef PRE_REMOVE_DOORS_UITEMP
	m_pAuthBtn->Show(false);
#else
	if (m_pAuthMessageBox == NULL)
		return;

	if (type == eDMMsgBox_CancelAuth)
		m_pAuthMessageBox->ResetType(eDMMsgBox_CancelAuth);
	m_pAuthMessageBox->Show(bShow);

	if (bShow == false)
		m_pAuthBtn->Enable(true);
#endif
}

void CDnDoorsMobileAuthButtonDlg::OnSetDoorsAuthMobileMsgBox(const WCHAR* pKey)
{
	if (m_pAuthMessageBox == NULL || pKey == NULL || pKey[0] == ' ')
		return;

	m_pAuthMessageBox->ResetType(eDMMsgBox_Auth);
	m_pAuthMessageBox->SetAuthData(pKey);
}

void CDnDoorsMobileAuthButtonDlg::EnableAuthButton(bool bEnable)
{
#ifdef PRE_REMOVE_DOORS_UITEMP
#else
	if (m_pAuthBtn)
		m_pAuthBtn->Enable(bEnable);
#endif
}

void CDnDoorsMobileAuthButtonDlg::OnCloseMessageBox()
{
#ifdef PRE_REMOVE_DOORS_UITEMP
#else
	m_pAuthBtn->Enable(true);
#endif
}

#endif // PRE_ADD_DOORS