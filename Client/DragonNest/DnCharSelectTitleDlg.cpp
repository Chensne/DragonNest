#include "StdAfx.h"
#include "DnCharSelectTitleDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

#ifndef PRE_MOD_SELECT_CHAR

CDnCharSelectTitleDlg::CDnCharSelectTitleDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
	: CEtUIDialog( dialogType, pParentDialog, nID, pCallback  )
{
}

CDnCharSelectTitleDlg::~CDnCharSelectTitleDlg(void)
{
}

void CDnCharSelectTitleDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "CharSelectTitleDlg.ui" ).c_str(), bShow );
}

#endif // PRE_MOD_SELECT_CHAR