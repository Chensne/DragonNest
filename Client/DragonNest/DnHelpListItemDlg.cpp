#include "StdAfx.h"
#include "DnHelpListItemDlg.h"

CDnHelpListItemDlg::CDnHelpListItemDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback )
{
	m_pStaticText = NULL;
	m_nLinkHelpTableID = 0;
}

CDnHelpListItemDlg::~CDnHelpListItemDlg()
{
}

void CDnHelpListItemDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "HelpListDlg.ui" ).c_str(), bShow );
}

void CDnHelpListItemDlg::InitialUpdate()
{
	m_pStaticText = GetControl<CEtUIStatic>("ID_TEXT_LIST");
}

void CDnHelpListItemDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnHelpListItemDlg::Process( float fElapsedTime )
{
	CEtUIDialog::Process( fElapsedTime );
}

void CDnHelpListItemDlg::SetInfo( CDnHelpDlg::KeywordStruct *pStruct )
{
	m_pStaticText->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, pStruct->nKeywordStringID ));
	m_nLinkHelpTableID = pStruct->nLinkHelpTableID;
}
