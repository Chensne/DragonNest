#include "stdafx.h"
#include "DnMiniPlayerGuildWarTabDlg.h"

#if !defined( USE_BOOST_MEMPOOL )
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
#endif // #if !defined( USE_BOOST_MEMPOOL )

CDnMiniPlayerGuildWarTabDlg::CDnMiniPlayerGuildWarTabDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback ), m_pPartyName(NULL)
{}

CDnMiniPlayerGuildWarTabDlg::~CDnMiniPlayerGuildWarTabDlg()
{}

void CDnMiniPlayerGuildWarTabDlg::Initialize(bool bShow)
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "MiniPlayerGuildWarTab.ui" ).c_str(), bShow );
}

void CDnMiniPlayerGuildWarTabDlg::InitialUpdate()
{
	m_pPartyName = GetControl<CEtUIStatic>( "ID_TEXT_PARTY" );
}

void CDnMiniPlayerGuildWarTabDlg::Show(bool bShow)
{
	if( bShow == m_bShow )
		return;

	CEtUIDialog::Show( bShow );
}

void CDnMiniPlayerGuildWarTabDlg::SetPartyName( int nParytNumber )
{
	WCHAR wszName[256];
	swprintf_s( wszName, _countof(wszName), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 126219 ), nParytNumber );	// %d ÆÄÆ¼
	m_pPartyName->SetText( wszName );
}
