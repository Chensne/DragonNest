#include "StdAfx.h"
#include "EternityEngine.h"
#include "EtUI.h"
#include "BlindCaptionDlg.h"
#include "EtResourceMng.h"


CBlindCaptionDlg::CBlindCaptionDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
, m_pStaticCaption(NULL)
{
}

CBlindCaptionDlg::~CBlindCaptionDlg(void)
{
}

void CBlindCaptionDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "BlindCaptionDlg.ui" ).c_str(), bShow );
}

void CBlindCaptionDlg::InitialUpdate()
{
	m_pStaticCaption = static_cast<CEtUIStatic*>(GetControl("ID_CAPTION"));
}

void CBlindCaptionDlg::Show( bool bShow )
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

void CBlindCaptionDlg::SetCaption( const wchar_t *wszCaption )
{
	if( !m_pStaticCaption ) return;
	m_pStaticCaption->SetText( wszCaption );
}

void CBlindCaptionDlg::ClearCaption()
{
	if( !m_pStaticCaption ) return;
	m_pStaticCaption->ClearText();
}