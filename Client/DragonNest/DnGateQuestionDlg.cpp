#include "StdAfx.h"
#include "DnGateQuestionDlg.h"
#include "DnPartyTask.h"
#include "DnMainMenuDlg.h"
#include "DnInterface.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnGateQuestionDlg::CDnGateQuestionDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
	: CEtUIDialog( dialogType, pParentDialog, nID, pCallback, true )
	, m_pButtonStart(NULL)
	, m_pButtonCancel(NULL)
	, m_pStaticTitle(NULL)
	, m_pButtonPartyDlgOpen( NULL )
{
}

CDnGateQuestionDlg::~CDnGateQuestionDlg(void)
{
}

void CDnGateQuestionDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "PartyGateQuestion.ui" ).c_str(), bShow );
}

void CDnGateQuestionDlg::InitialUpdate()
{
	m_pButtonStart = GetControl<CEtUIButton>("ID_START");
	m_pButtonCancel = GetControl<CEtUIButton>("ID_CANCEL");
	m_pStaticTitle = GetControl<CEtUIStatic>("ID_STATIC_TITLE");
	m_pButtonPartyDlgOpen = GetControl<CEtUIButton>("ID_PARTY");
}

void CDnGateQuestionDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if (IsCmdControl("ID_PARTY"))
		{
			//GetInterface().GetMainMenuDialog(CDnMainMenuDlg::COMMUNITY_DIALOG)->Show(true);
			GetInterface().SwapPartyDialog();
		}

		Show(false);
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnGateQuestionDlg::Process( float fElapsedTime )
{
	CEtUIDialog::Process( fElapsedTime );

	if( IsShow() )
	{
		if( !GetInterface().IsEnableRender() && m_pButtonStart->IsEnable() )
		{
			CEtUIDialog::ProcessCommand( EVENT_BUTTON_CLICKED, false, m_pButtonStart, 0 );
			Show( false );
		}
	}
}

void CDnGateQuestionDlg::Show( bool bShow )
{
	if( m_bShow == bShow )
		return;

	if( bShow )
	{
		CDnMainMenuDlg *pMainMenuDlg = GetInterface().GetMainMenuDialog();
		if( pMainMenuDlg ) pMainMenuDlg->CloseMenuDialog();

		GetInterface().CloseGuildInviteReqDlg( true );
		GetInterface().CloseAcceptRequestDialog(true);

		GetInterface().ShowHelpKeyboardDialog( false );
		GetInterface().ShowMapMoveCashItemDlg( false, NULL, 0 );
		GetInterface().CloseItemUnsealDialog();
		GetInterface().ShowItemSealDialog( false );
		GetInterface().ShowGuildMarkCreateDialog( false );
#ifdef PRE_ADD_EXCHANGE_POTENTIAL
		GetInterface().ShowItemPotentialTransferDialog( false );
#endif
#ifdef PRE_ADD_PVP_VILLAGE_ACCESS
		GetInterface().ShowPVPVillageAccessDlg( false );
#endif
		
		EnableButtons();
	}

	CEtUIDialog::Show( bShow );
//	CDnMouseCursor::GetInstance().ShowCursor( bShow );
}

void CDnGateQuestionDlg::EnableButtons()
{
	bool bEnable(false);

	if( GetPartyTask().GetPartyRole() == CDnPartyTask::MEMBER )
	{
		bEnable = false;
	}
	else
	{
		bEnable = true;
	}

	m_pButtonStart->Enable(bEnable);
	m_pButtonCancel->Enable(bEnable);
	m_pButtonPartyDlgOpen->Enable((CDnWorld::GetInstance().GetMapType() == CDnWorld::MapTypeVillage) && 
		(GetPartyTask().GetPartyRole() == CDnPartyTask::SINGLE));
//	m_pButtonPartyDlgOpen->Enable(false);
}

void CDnGateQuestionDlg::SetTitleName( const wchar_t *wszTitle )
{
	m_pStaticTitle->SetText( wszTitle );
}