#include "StdAfx.h"
#include "DnPVPChatDlg.h"
#include "DnChatOptDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnPVPChatDlg::CDnPVPChatDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnChatDlg( dialogType, pParentDialog, nID, pCallback )
{
	

}


CDnPVPChatDlg::~CDnPVPChatDlg(void)
{
	SAFE_DELETE( m_pChatOptDlg );
}

void CDnPVPChatDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "PvpChatDialog.ui" ).c_str(), bShow );
}