#include "StdAfx.h"
#include "DnCaptionDlg_01.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnCaptionDlg_01::CDnCaptionDlg_01( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
	: CDnCaptionDlg( dialogType, pParentDialog, nID, pCallback )
{
	m_fShowRatio = 0.75f;
}

CDnCaptionDlg_01::~CDnCaptionDlg_01(void)
{
}

void CDnCaptionDlg_01::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "CaptionDlg_01.ui" ).c_str(), bShow );
}