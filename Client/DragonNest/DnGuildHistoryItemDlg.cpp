#include "StdAfx.h"
#include "DnGuildHistoryItemDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnGuildHistoryItemDlg::CDnGuildHistoryItemDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
, m_pStaticList0(NULL)
, m_pStaticList1(NULL)
{
}

CDnGuildHistoryItemDlg::~CDnGuildHistoryItemDlg(void)
{
}

void CDnGuildHistoryItemDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "GuildHistoryListDlg.ui" ).c_str(), bShow );
}

void CDnGuildHistoryItemDlg::InitialUpdate()
{
	m_pStaticList0 = GetControl<CEtUIStatic>( "ID_HIS_LIST0" );
	m_pStaticList1 = GetControl<CEtUIStatic>( "ID_HIS_LIST1" );	
}

void CDnGuildHistoryItemDlg::SetInfo( const WCHAR *wszText1, const WCHAR *wszText2 )
{
	m_pStaticList0->SetText( wszText1 );
	m_pStaticList1->SetText( wszText2 );
}