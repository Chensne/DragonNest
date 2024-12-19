#include "StdAfx.h"
#include "DnCaptionDlg_07.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnCaptionDlg_07::CDnCaptionDlg_07( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnCaptionExDlg( dialogType, pParentDialog, nID, pCallback )
{
	m_fShowRatio = 0.75f;
}

CDnCaptionDlg_07::~CDnCaptionDlg_07(void)
{
}

void CDnCaptionDlg_07::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "CaptionDlg_07.ui" ).c_str(), bShow );
}