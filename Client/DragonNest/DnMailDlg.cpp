#include "StdAfx.h"
#include "DnMailDlg.h"
#include "DNCommonDef.h"
#include "DnTradeTask.h"
#include "DnMainMenuDlg.h"
#include "DnMailReadDlg.h"
#include "DnMailWriteDlg.h"
#include "DnInterface.h"
#include "DnMailListDlg.h"
#include "DnLocalPlayerActor.h"
#ifdef PRE_ADD_SHORTCUT_HELP_DIALOG
#include "DnSystemDlg.h"
#endif

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnMailDlg::CDnMailDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
	: CEtUITabDialog( dialogType, pParentDialog, nID, pCallback, true )
{
	m_pMailListDlg = NULL;
	m_pMailReadDlg = NULL;
	m_pMailWriteDlg = NULL;

	m_pMailListTab = NULL;
	m_pMailReadTab = NULL;
	m_pMailWriteTab = NULL;

	m_bLock = false;
}

void CDnMailDlg::Initialize(bool bShow)
{
	CEtUIDialog::Initialize(CEtResourceMng::GetInstance().GetFullName("MailDlg.ui").c_str(), bShow);

	if( CDnTradeTask::IsActive() )
	{
		GetTradeTask().GetTradeMail().SetMailDialog( this );
	}
	else
	{
		CDebugSet::ToLogFile( "CDnMailDlg::Initialize, 메일 다이얼로그가 만들어 지기 전에 거래 테스크가 생성되어야 합니다." );
	}
}

void CDnMailDlg::InitialUpdate()
{
	m_pMailListTab = GetControl<CEtUIRadioButton>("ID_RADIOBUTTON_LIST");
	m_pMailListDlg = new CDnMailListDlg(UI_TYPE_CHILD, this);
	m_pMailListDlg->Initialize(false);
	AddTabDialog(m_pMailListTab, m_pMailListDlg);

	m_pMailReadTab = GetControl<CEtUIRadioButton>("ID_RADIOBUTTON_READ");
	m_pMailReadDlg = new CDnMailReadDlg(UI_TYPE_CHILD, this);
	m_pMailReadDlg->Initialize(false);
	AddTabDialog(m_pMailReadTab, m_pMailReadDlg);

	m_pMailWriteTab = GetControl<CEtUIRadioButton>("ID_RADIOBUTTON_WRITE");
	m_pMailWriteDlg = new CDnMailWriteDlg(UI_TYPE_CHILD, this);
	m_pMailWriteDlg->Initialize(false);
	AddTabDialog(m_pMailWriteTab, m_pMailWriteDlg);

	SetCheckedTab(m_pMailListTab->GetTabID());
}

void CDnMailDlg::Show(bool bShow)
{
	if (m_bShow == bShow)
		return;
	CEtUITabDialog::Show(bShow);

	if (m_pMailWriteDlg == NULL ||
		m_pMailReadDlg == NULL ||
		m_pMailListDlg == NULL)
	{
		_ASSERT(0);
		return;
	}

	m_pMailWriteDlg->Clear(true);
	m_pMailReadDlg->Clear();
	m_pMailListDlg->Clear();

	if (m_bShow)
	{
		LockDlgs(false);	// todo : 문제 있을 수 있음. 서버 처리 중일 때, 메일 닫았다가 다시 열면 다시 패킷을 보낼 수 있으므로. 닫는 곳에서 막는 것 생각해보기. by kalliste
		OpenMailListDialog();

		GetTradeTask().GetTradeMail().RequestMailBox(1);
		GetTradeTask().GetTradeMail().ClearCurReadMailInfoCache();

		DWORD nUID = CDnLocalPlayerActor::GetTakeNpcUID();
		DnActorHandle hNpc = CDnActor::FindActorFromUniqueID(nUID);
		if( hNpc )
		{
			hNpc->SetActionQueue( "Open" );
		}
	}
	else
	{
		if( drag::IsValid() )
		{
			CDnSlotButton *pDragButton = static_cast<CDnSlotButton*>(drag::GetControl());
			if (pDragButton)
			{
				pDragButton->DisableSplitMode(false);
				drag::ReleaseControl();
			}
		}

		DWORD nUID = CDnLocalPlayerActor::GetTakeNpcUID();
		DnActorHandle hNpc = CDnActor::FindActorFromUniqueID(nUID);
		if( hNpc )
		{
			hNpc->SetActionQueue( "Close" );
		}

		GetInterface().CloseBlind();
	}

}

void CDnMailDlg::RefreshMailBoxList(const std::vector<CDnTradeMail::SMailBoxInfo>& mailList)
{
	if (m_pMailListDlg)
	 	m_pMailListDlg->RefreshList(mailList);
	else
		_ASSERT(0);
}

void CDnMailDlg::RefreshMailReadDialog(const CDnTradeMail::SReadMailInfo& info)
{
	if (m_pMailReadDlg)
		m_pMailReadDlg->Update(info);
	else
		_ASSERT(0);

//	OpenMailReadDialog();
}

void CDnMailDlg::OpenMailReadDialog()
{
	if (m_pMailReadDlg == NULL || m_pMailReadDlg->IsUpdated() == false)
	{
		_ASSERT(0);
		return;
	}

	int tabId = m_pMailReadTab->GetTabID();

	m_groupTabDialog.ShowDialog(GetCurrentTabID(), false);
	m_groupTabDialog.ChangeDialog(tabId, m_pMailReadDlg);
	m_groupTabDialog.ShowDialog(tabId, true);

	LockDlgs(false);

	SetCheckedTab(tabId);
}

void CDnMailDlg::OpenMailWriteDialog(const wchar_t *wszName, const wchar_t* wszTitle, bool bAfterMailSend)
{
	if (m_pMailWriteDlg == NULL || m_pMailWriteTab == NULL)
	{
		_ASSERT(0);
		return;
	}

	m_pMailWriteDlg->Clear(bAfterMailSend == false);
	m_pMailWriteDlg->SetName(wszName);
	m_pMailWriteDlg->SetTitle(wszTitle);

	int tabId = m_pMailWriteTab->GetTabID();
	m_groupTabDialog.ShowDialog(GetCurrentTabID(), false);
	m_groupTabDialog.ChangeDialog(tabId, m_pMailWriteDlg);
	m_groupTabDialog.ShowDialog(tabId, true);

	LockDlgs(false);

	SetCheckedTab(tabId);
}

void CDnMailDlg::OpenMailListDialog()
{
	if (m_pMailListTab == NULL)
	{
		_ASSERT(0);
		return;
	}

	int tabId = m_pMailListTab->GetTabID();

	m_groupTabDialog.ShowDialog(GetCurrentTabID(), false);
	m_groupTabDialog.ChangeDialog(tabId, m_pMailListDlg);
	m_groupTabDialog.ShowDialog(tabId, true);

	LockDlgs(false);

	SetCheckedTab(tabId);
}

void CDnMailDlg::Process( float fElapsedTime )
{
	if( !CDnTradeTask::IsActive() ) return;

	if (m_pMailReadTab != NULL && m_pMailReadDlg != NULL)
		m_pMailReadTab->Enable(m_pMailReadDlg->IsUpdated());

	CEtUITabDialog::Process(fElapsedTime);
}

void CDnMailDlg::ClearMailReadDialog()
{
	if (m_pMailReadDlg)
		m_pMailReadDlg->Clear();
	else
		_ASSERT(0);
}

void CDnMailDlg::ClearMailReadAttach()
{
	if (m_pMailReadDlg == NULL)
	{
		_ASSERT(0);
		return;
	}

	m_pMailReadDlg->ClearAttach();

	CDnTradeMail& mailTask = GetTradeTask().GetTradeMail();
	int readAttachMailId = m_pMailReadDlg->GetMailID();
	CDnTradeMail::SMailBoxInfo* pInfo = mailTask.FindMailBoxInfo(readAttachMailId);
	if (pInfo)
	{
		pInfo->bItem = false;
		m_pMailListDlg->RefreshList(mailTask.GetMailBoxInfoList());
	}
	else
	{
		mailTask.RequestMailBox(mailTask.GetCurrentPage());
	}
}

void CDnMailDlg::RemoveReadDialogAttachItem(INT64 attachItemSerial, bool bHasCoin)
{
	if (m_pMailReadDlg == NULL)
	{
		_ASSERT(0);
		return;
	}

	int restAttachItemCount = m_pMailReadDlg->RemoveAttachItem(attachItemSerial);
	if (restAttachItemCount < 0)
	{
		GetInterface().MessageBox(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1990 )); // UISTRING : 메일을 읽어오는데 실패했습니다. 메일함을 다시 열어주세요.
		return;
	}

	CDnTradeMail& mailTask = GetTradeTask().GetTradeMail();
	int readAttachMailId = m_pMailReadDlg->GetMailID();
	CDnTradeMail::SMailBoxInfo* pInfo = mailTask.FindMailBoxInfo(readAttachMailId);
	if (pInfo)
	{
		pInfo->bItem = (restAttachItemCount > 0 || bHasCoin);
		m_pMailListDlg->RefreshList(mailTask.GetMailBoxInfoList());
	}
	else
	{
		mailTask.RequestMailBox(mailTask.GetCurrentPage());
	}
}

bool CDnMailDlg::IsShowWriteDlg() const
{
	if (m_pMailWriteDlg == NULL)
	{
		_ASSERT(0);
		return false;
	}

	return (m_pMailWriteDlg->IsShow());
}

bool CDnMailDlg::AttachItemToWriteDlg(CDnSlotButton* pFromSlot, bool bItemSplit)
{
	if (m_pMailWriteDlg == NULL)
	{
		_ASSERT(0);
		return false;
	}

	return m_pMailWriteDlg->AddAutoAttachItem(pFromSlot, bItemSplit);
}

void CDnMailDlg::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl( "ID_CLOSE_DIALOG" ) )
		{
			Show(false);
			return;
		}
	}
}

void CDnMailDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg)
{
	SetCmdControlName( pControl->GetControlName() );
	
	if( nCommand == EVENT_BUTTON_CLICKED )
	{
#ifdef PRE_ADD_SHORTCUT_HELP_DIALOG
	if( IsCmdControl("ID_BT_SMALLHELP") )
	{
		CDnMainMenuDlg* pMainMenuDlg = GetInterface().GetMainMenuDialog();
		CDnSystemDlg* pSystemDlg = pMainMenuDlg->GetSystemDlg();
		if(pSystemDlg)
			pSystemDlg->ShowChoiceHelpDlg(HELP_SHORTCUT_MAIL);
	}
#endif
	}

	CEtUITabDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnMailDlg::LockDlgs(bool bLock)
{
	m_bLock = bLock;

	if (m_pMailWriteDlg == NULL ||
		m_pMailReadDlg == NULL ||
		m_pMailListDlg == NULL ||
		m_pMailListTab == NULL ||
		m_pMailReadTab == NULL ||
		m_pMailWriteTab == NULL)
	{
		_ASSERT(0);
		return;
	}

	if (bLock)
	{
		m_pMailWriteDlg->Lock();
		m_pMailReadDlg->Lock();
		m_pMailListDlg->Lock();

		m_pMailListTab->Enable(false);
		m_pMailReadTab->Enable(false);
		m_pMailWriteTab->Enable(false);
	}
	else
	{
		m_pMailWriteDlg->UnLock();
		m_pMailReadDlg->UnLock();
		m_pMailListDlg->UnLock();

		m_pMailListTab->Enable(true);
		m_pMailReadTab->Enable(true);
		m_pMailWriteTab->Enable(true);
	}
}