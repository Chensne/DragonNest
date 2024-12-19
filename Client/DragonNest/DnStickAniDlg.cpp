#include "StdAfx.h"
#include "DnStickAniDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

CDnStickAniDlg::CDnStickAniDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
	: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
	, m_pAnimation(NULL)
{
}

CDnStickAniDlg::~CDnStickAniDlg(void)
{
}

void CDnStickAniDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "StickAniDlg.ui" ).c_str(), bShow );
}

void CDnStickAniDlg::InitialUpdate()
{
	m_pAnimation = GetControl<CEtUIAnimation>("ID_ANIMATION");
}

void CDnStickAniDlg::Show( bool bShow ) 
{
	if( m_bShow == bShow )
		return;

	if( bShow )
	{
		m_pAnimation->Play();
	}
	else
	{
		m_pAnimation->Stop();
	}

	CEtUIDialog::Show( bShow );
}
