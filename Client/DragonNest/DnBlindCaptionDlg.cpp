#include "StdAfx.h"
#include "DnBlindCaptionDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnBlindCaptionDlg::CDnBlindCaptionDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
	: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
	, m_pStaticCaption(NULL)
{
}

CDnBlindCaptionDlg::~CDnBlindCaptionDlg(void)
{
}

void CDnBlindCaptionDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "BlindCaptionDlg.ui" ).c_str(), bShow );
}

void CDnBlindCaptionDlg::InitialUpdate()
{
	m_pStaticCaption = GetControl<CEtUIStatic>("ID_CAPTION");
}

void CDnBlindCaptionDlg::Show( bool bShow )
{ 
	if( m_bShow == bShow )
		return;

	if( bShow )
	{
	}
	else
	{
		m_pStaticCaption->ClearText();
	}

	CEtUIDialog::Show( bShow );
}

void CDnBlindCaptionDlg::SetCaption( const wchar_t *wszCaption )
{
	if( !m_pStaticCaption ) return;
	m_pStaticCaption->SetText( wszCaption );
}

void CDnBlindCaptionDlg::ClearCaption()
{
	if( !m_pStaticCaption ) return;
	m_pStaticCaption->ClearText();
}