#include "StdAfx.h"
#include "DnPVPAllKilledDlg.h"
#include "DnInterface.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnPvPAllKilledDlg::CDnPvPAllKilledDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
{
}

CDnPvPAllKilledDlg::~CDnPvPAllKilledDlg()
{
}

void CDnPvPAllKilledDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "PvpAllKilledDlg.ui" ).c_str(), bShow );
}
