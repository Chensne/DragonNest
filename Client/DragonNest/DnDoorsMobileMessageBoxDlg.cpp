#include "StdAfx.h"

#ifdef PRE_ADD_DOORS

#include "DnDoorsMobileMessageBoxDlg.h"
#include "DnInterface.h"
#include "DnBridgeTask.h"
#include "LoginSendPacket.h"
#include "DnTableDB.h"
#include "DnDoorsMobileAuthButtonDlg.h"

CDnDoorsMobileMessageBoxDlg::CDnDoorsMobileMessageBoxDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback)
: CEtUIDialog(dialogType, pParentDialog, nID, pCallback)
{
	m_pAuthOrCancelAuthBtn = NULL;
	m_pKeyStatic = NULL;
	m_pKeyTitleStatic = NULL;
	m_pTimerStatic = NULL;
	m_pCloseBtn = NULL;
	m_fTimer = 0.f;
	m_PrevTimeCheck = 0;
	m_Type = eDMMsgBox_Auth;
	m_pParentAuthDlg = NULL;
}

CDnDoorsMobileMessageBoxDlg::~CDnDoorsMobileMessageBoxDlg(void)
{
}

void CDnDoorsMobileMessageBoxDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize(CEtResourceMng::GetInstance().GetFullName("MobileMessageBoxDlg.ui").c_str(), bShow);
}

void CDnDoorsMobileMessageBoxDlg::InitialUpdate()
{
	m_pAuthOrCancelAuthBtn = GetControl<CEtUIButton>("ID_BT_KEY");
	m_pKeyStatic = GetControl<CEtUIStatic>("ID_TEXT_KEY");
	m_pKeyTitleStatic = GetControl<CEtUIStatic>("ID_TEXT_KEYTITLE");
	m_pTimerStatic = GetControl<CEtUIStatic>("ID_TEXT_TIME");
	m_pCloseBtn = GetControl<CEtUIButton>("ID_BT_CANCEL");
}

void CDnDoorsMobileMessageBoxDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if (nCommand == EVENT_BUTTON_CLICKED)
	{
		if (IsCmdControl( "ID_BT_KEY" ))
		{
			if (IsType() == eDMMsgBox_CancelAuth)
			{
				m_pAuthOrCancelAuthBtn->Enable(false);
				SendDoorsMobileReqCancelAuth();
			}

			m_pCloseBtn->Enable(false);
		}
		else if (IsCmdControl("ID_BT_CANCEL") || IsCmdControl("ID_BT_CLOSE"))
		{
			if (m_pCloseBtn->IsEnable())
			{
				if (m_pParentAuthDlg)
					m_pParentAuthDlg->OnCloseMessageBox();
				Show(false);
			}
		}
	}

	CEtUIDialog::ProcessCommand(nCommand, bTriggeredByUser, pControl, uMsg);
}

void CDnDoorsMobileMessageBoxDlg::Process(float fElapsedTime)
{
	if (m_Type == eDMMsgBox_Auth)
	{
		if (m_fTimer > 0.f)
		{
			m_fTimer -= fElapsedTime;
			if (m_fTimer <= 0)
			{
				SendDoorsMobileReqAuthKey();

				m_fTimer = 0.f;
				std::wstring str = FormatW(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 6481), 0);
				m_pTimerStatic->SetText(str.c_str());
			}

			if (m_PrevTimeCheck != (int)m_fTimer)
			{
				m_PrevTimeCheck = (int)m_fTimer;
				std::wstring str = FormatW(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 6481), m_PrevTimeCheck);
				m_pTimerStatic->SetText(str.c_str());
			}
		}
		else
		{
			m_fTimer = 0.f;
		}
	}

	CEtUIDialog::Process(fElapsedTime);
}

void CDnDoorsMobileMessageBoxDlg::ResetType(DoorsMobileMsgBoxDlgType type)
{
	m_Type = type;

	if (m_pAuthOrCancelAuthBtn == NULL || m_pKeyStatic == NULL || m_pTimerStatic == NULL)
		return;

	m_pCloseBtn->Enable(true);

	if (type == eDMMsgBox_Auth)
	{
		m_pAuthOrCancelAuthBtn->Show(false);
		m_pKeyTitleStatic->Show(true);

		m_pKeyStatic->ClearText();
		m_pTimerStatic->Show(true);
		m_pTimerStatic->ClearText();
	}
	else if (type == eDMMsgBox_CancelAuth)
	{
		m_pAuthOrCancelAuthBtn->SetText(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 6478)); // UISTRING : 인증 해제 하기
		m_pAuthOrCancelAuthBtn->Show(true);
		m_pAuthOrCancelAuthBtn->Enable(true);
		m_pKeyTitleStatic->Show(false);

		m_pKeyStatic->SetText(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 6482)); // UISTRING : 인증상태입니다.
		m_pTimerStatic->Show(false);
	}
}

void CDnDoorsMobileMessageBoxDlg::SetAuthData(const WCHAR* pKey)
{
	if (m_Type == eDMMsgBox_CancelAuth)
	{
		_ASSERT(0);
		return;
	}

	m_pKeyStatic->SetText(pKey);
	m_fTimer = CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::DoorsMobile_AuthRefreshSec);
	m_PrevTimeCheck = 0;
}

void CDnDoorsMobileMessageBoxDlg::SetParentAuthDlg(CDnDoorsMobileAuthButtonDlg* pParentDlg)
{
	m_pParentAuthDlg = pParentDlg;
}

#endif // PRE_ADD_DOORS