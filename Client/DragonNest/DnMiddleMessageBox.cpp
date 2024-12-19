#include "stdafx.h"
#include "DnMiddleMessageBox.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnMiddleMessageBox::CDnMiddleMessageBox( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnMessageBox( dialogType, pParentDialog, nID, pCallback )
{}

CDnMiddleMessageBox::~CDnMiddleMessageBox()
{}

void CDnMiddleMessageBox::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "MessageBox_Middle.ui" ).c_str(), bShow );
}