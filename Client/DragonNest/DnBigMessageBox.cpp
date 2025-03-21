#include "stdafx.h"
#include "DnBigMessageBox.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnBigMessageBox::CDnBigMessageBox( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnMessageBox( dialogType, pParentDialog, nID, pCallback )
{}

CDnBigMessageBox::~CDnBigMessageBox()
{}

void CDnBigMessageBox::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "MessageBox_Big.ui" ).c_str(), bShow );
}