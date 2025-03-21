#include "StdAfx.h"
#include "DnPartyKickDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnPartyKickDlg::CDnPartyKickDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
	: CEtUIDialog( dialogType, pParentDialog, nID, pCallback  )
{
}

CDnPartyKickDlg::~CDnPartyKickDlg(void)
{
}

void CDnPartyKickDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "PartyKickDlg.ui" ).c_str(), bShow );
}