#include "StdAfx.h"
#include "DnHelpKeyboardTabDlg.h"

CDnHelpKeyboardTabDlg::CDnHelpKeyboardTabDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
{
}

CDnHelpKeyboardTabDlg::~CDnHelpKeyboardTabDlg()
{
}

void CDnHelpKeyboardTabDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( m_szUIFileName ).c_str(), bShow );
}

void CDnHelpKeyboardTabDlg::SetUIFileName( const char *szFileName )
{
	m_szUIFileName = szFileName;
}