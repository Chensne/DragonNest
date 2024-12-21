#include "StdAfx.h"
#include "DnDarklairRequestChallengeDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnDarklairRequestChallengeDlg::CDnDarklairRequestChallengeDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnMessageBox( dialogType, pParentDialog, nID, pCallback )
{
}

CDnDarklairRequestChallengeDlg::~CDnDarklairRequestChallengeDlg()
{
}

void CDnDarklairRequestChallengeDlg::InitialUpdate()
{
	CDnMessageBox::InitialUpdate();

	SetTitle( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 830 ) );
	m_pButtonYes->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 831 ) );
}

void CDnDarklairRequestChallengeDlg::Show( bool bShow )
{ 
	if( bShow == m_bShow )
		return;

	if( bShow )
	{
		SetTitle( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 830 ) );
	}

	CDnMessageBox::Show( bShow );
}

void CDnDarklairRequestChallengeDlg::EnableButton( bool bEnableButton )
{
	m_pButtonYes->Enable( bEnableButton );
	m_pButtonNo->Enable( bEnableButton );
}

