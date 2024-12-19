#include "StdAfx.h"
#include "DnCaptionDlg_02.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnCaptionDlg_02::CDnCaptionDlg_02( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
	: CDnCaptionDlg( dialogType, pParentDialog, nID, pCallback )
{
	m_fShowRatio = 0.65f;
}

CDnCaptionDlg_02::~CDnCaptionDlg_02(void)
{
}

void CDnCaptionDlg_02::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "CaptionDlg_02.ui" ).c_str(), bShow );
}

void CDnCaptionDlg_02::InitialUpdate()
{
	m_pCaption = GetControl<CEtUIStatic>("ID_CAPTION");

	m_pCaption->GetUICoord( m_uiDefaultPos );
	GetControl<CEtUIStatic>("ID_BOTTOMPOS")->GetUICoord( m_uiBottomPos );
}

void CDnCaptionDlg_02::SetCaption( LPCWSTR wszMsg, DWORD dwColor, float fFadeTime, bool bBottomPos )
{
	if( !bBottomPos )
		m_pCaption->SetUICoord( m_uiDefaultPos );
	else
		m_pCaption->SetUICoord( m_uiBottomPos );

	CDnCaptionDlg::SetCaption( wszMsg, dwColor, fFadeTime );
}