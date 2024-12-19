#include "StdAfx.h"
#include "DnDummyModalDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnDummyModalDlg::CDnDummyModalDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
	: CEtUIDialog( dialogType, pParentDialog, nID, pCallback, true )
{
}

void CDnDummyModalDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize(bShow);
}