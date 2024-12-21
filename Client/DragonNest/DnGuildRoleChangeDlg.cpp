#include "StdAfx.h"
#include "DnGuildRoleChangeDlg.h"
#include "DnGuildTask.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnGuildRoleChangeDlg::CDnGuildRoleChangeDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
, m_pButtonOK(NULL)
, m_pName(NULL)
, m_pComboBox(NULL)
, m_nCharacterDBID(0)
{
}

CDnGuildRoleChangeDlg::~CDnGuildRoleChangeDlg(void)
{
}

void CDnGuildRoleChangeDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "GuildGradeDlg.ui" ).c_str(), bShow );
}

void CDnGuildRoleChangeDlg::InitialUpdate()
{
	m_pButtonOK = GetControl<CEtUIButton>("ID_OK");
	m_pName = GetControl<CEtUIStatic>("ID_NAME");
	m_pComboBox = GetControl<CEtUIComboBox>("ID_COMBOBOX_GRADE");

	m_pComboBox->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3734 ), NULL, GUILDROLE_TYPE_SUBMASTER );
	m_pComboBox->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3735 ), NULL, GUILDROLE_TYPE_SENIOR );
	m_pComboBox->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3736 ), NULL, GUILDROLE_TYPE_REGULAR );
	m_pComboBox->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3737 ), NULL, GUILDROLE_TYPE_JUNIOR );
}

void CDnGuildRoleChangeDlg::Show( bool bShow ) 
{
	if( m_bShow == bShow )
		return;

	if( bShow )
	{
		InitControl();
	}

	CEtUIDialog::Show( bShow );
}

void CDnGuildRoleChangeDlg::InitControl()
{
	if( !CDnGuildTask::IsActive() ) return;
	TGuildMember *pMember = GetGuildTask().GetGuildMemberFromCharacterDBID( m_nCharacterDBID );
	if( pMember ) {
		m_pName->SetText( pMember->wszCharacterName );
		m_pComboBox->SetSelectedByValue( pMember->btGuildRole );
	}
	m_pButtonOK->Enable( false );
}

void CDnGuildRoleChangeDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	TGuildMember *pCurMember = GetGuildTask().GetGuildMemberFromCharacterDBID( m_nCharacterDBID );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl("ID_OK") )
		{
			if( !pCurMember ) return;

			int iInt1 = 0;
			if( m_pComboBox->GetSelectedValue( iInt1 ) )
				GetGuildTask().RequestChangeGuildMemberInfo(GUILDMEMBUPDATE_TYPE_ROLE, iInt1, 0, 0, 0, pCurMember->nAccountDBID, pCurMember->nCharacterDBID );
			Show(false);
			return;
		}
		else if( IsCmdControl("ID_CANCEL") || IsCmdControl("ID_BUTTON_CLOSE") )
		{
			Show(false);
			return;
		}
	}
	else if( nCommand == EVENT_COMBOBOX_SELECTION_CHANGED && bTriggeredByUser ) 
	{
		if( IsCmdControl("ID_COMBOBOX_GRADE") )
		{
			if( !pCurMember ) return;

			int iInt1 = 0;
			if( m_pComboBox->GetSelectedValue( iInt1 ) ) {
				if( iInt1 != pCurMember->btGuildRole )
					m_pButtonOK->Enable( true );
				else
					m_pButtonOK->Enable( false );
			}
			return;
		}
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}