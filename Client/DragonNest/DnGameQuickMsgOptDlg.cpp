#include "StdAfx.h"
#include "DnGameQuickMsgOptDlg.h"
#include "DnGameRadioMsgOptDlg.h"
#include "DnGameMacroMsgOptDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnGameQuickMsgOptDlg::CDnGameQuickMsgOptDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnOptionTabDlg( dialogType, pParentDialog, nID, pCallback )
, m_pTabButtonRadioMsg(NULL)
, m_pTabButtonMacroMsg(NULL)
, m_pGameRadioMsgOptDlg(NULL)
, m_pGameMacroMsgOptDlg(NULL)
{
}

CDnGameQuickMsgOptDlg::~CDnGameQuickMsgOptDlg()
{
}

void CDnGameQuickMsgOptDlg::Initialize( bool bShow )
{
	BaseClass::Initialize( CEtResourceMng::GetInstance().GetFullName( "GameQuickMsgOptDlg.ui" ).c_str(), bShow );
}

void CDnGameQuickMsgOptDlg::InitialUpdate()
{
	m_pTabButtonRadioMsg = GetControl<CEtUIRadioButton>("ID_TAB_RADIO");
	m_pGameRadioMsgOptDlg = new CDnGameRadioMsgOptDlg( UI_TYPE_CHILD, this );
	m_pGameRadioMsgOptDlg->Initialize( false );
	AddTabDialog( m_pTabButtonRadioMsg, m_pGameRadioMsgOptDlg );

	m_pTabButtonMacroMsg = GetControl<CEtUIRadioButton>("ID_TAB_MACRO");
	m_pGameMacroMsgOptDlg = new CDnGameMacroMsgOptDlg( UI_TYPE_CHILD, this );
	m_pGameMacroMsgOptDlg->Initialize( false );
	AddTabDialog( m_pTabButtonMacroMsg, m_pGameMacroMsgOptDlg );

	GetControl<CEtUIStatic>("ID_STATIC_TEXT")->Show(false);
	GetControl<CEtUIStatic>("ID_STATIC_ICON")->Show(false);
}

void CDnGameQuickMsgOptDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl( "ID_BUTTON_RESET" ) )
		{
			if( m_pTabButtonMacroMsg->GetTabID() == GetCurrentTabID() && m_pGameMacroMsgOptDlg )
			{
				m_pGameMacroMsgOptDlg->ResetDefault();
			}
		}
	}
	else if( nCommand == EVENT_RADIOBUTTON_CHANGED )
	{
		if( m_pTabButtonRadioMsg->GetTabID() == GetCurrentTabID() )
			GetControl<CEtUIButton>("ID_BUTTON_RESET")->Show(false);
		else if( m_pTabButtonMacroMsg->GetTabID() == GetCurrentTabID() )
			GetControl<CEtUIButton>("ID_BUTTON_RESET")->Show(true);
	}

	BaseClass::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}