#include "StdAfx.h"
#include "DnRebirthDlg.h"
#include "DnPartyTask.h"
#include "DnInterface.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnRebirthDlg::CDnRebirthDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
	: CEtUIDialog( dialogType, pParentDialog, nID, pCallback  )
	, m_pButtonCoin(NULL)
	, m_pStaticCoin(NULL)
{
}

CDnRebirthDlg::~CDnRebirthDlg(void)
{
}

void CDnRebirthDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "RebirthDlg.ui" ).c_str(), bShow );
}

void CDnRebirthDlg::InitialUpdate()
{
	m_pButtonCoin = GetControl<CEtUIButton>("ID_BUTTON_COIN");
	m_pStaticCoin = GetControl<CEtUIStatic>("ID_STATIC_COIN");
}

void CDnRebirthDlg::Show( bool bShow )
{
	if( m_bShow == bShow )
		return;

	CEtUIDialog::Show( bShow );
	//CDnMouseCursor::GetInstance().ShowCursor( bShow );
}

void CDnRebirthDlg::SetRebirthCoin( int nCoin )
{
	wchar_t wszTemp[128] = {0};
	swprintf_s( wszTemp, _countof(wszTemp), L"%s : %d°³", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 911 ), nCoin );
	m_pStaticCoin->SetText( wszTemp );
}

void CDnRebirthDlg::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg )
{
	if( m_pCallback )
	{
		m_pCallback->OnUICallbackProc( GetDialogID(), EVENT_BUTTON_CLICKED, NULL, 0 );
	}
}