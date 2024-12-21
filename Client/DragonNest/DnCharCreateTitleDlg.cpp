#include "StdAfx.h"
#include "DnCharCreateTitleDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

#ifndef PRE_MOD_SELECT_CHAR

CDnCharCreateTitleDlg::CDnCharCreateTitleDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback  )
{
}

CDnCharCreateTitleDlg::~CDnCharCreateTitleDlg(void)
{
}

void CDnCharCreateTitleDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "CharCreateTitle.ui" ).c_str(), bShow );
}

#endif // PRE_MOD_SELECT_CHAR