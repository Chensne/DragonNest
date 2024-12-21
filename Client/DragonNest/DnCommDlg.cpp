#include "StdAfx.h"
#include "DnCommDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnCommDlg::CDnCommDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
	: CEtUIDialog( dialogType, pParentDialog, nID, pCallback  )
	, m_fDelayTime(0.0f)
#ifdef PRE_ADD_OPEN_QUEST_TEXTURE_DIALOG
	, m_bAutoCloseDialog(false)
	, m_DlgType(eCommDlg_TYPE_COMMON)
#endif
{
}

CDnCommDlg::~CDnCommDlg(void)
{
}

void CDnCommDlg::CallBackFunc( DWORD dwID, DWORD dwSetID )
{
	if( !m_pCallback ) return;
	m_pCallback->OnUICallbackProc( dwID, dwSetID, NULL );
}