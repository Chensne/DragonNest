#include "StdAfx.h"
#include "DnGuildStorageHistoryItemDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnGuildStorageHistoryItemDlg::CDnGuildStorageHistoryItemDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
, m_pStaticList0(NULL)
, m_pStaticList1(NULL)
, m_pStaticList2(NULL)
, m_pStaticList3(NULL)
, m_pStaticList4(NULL)
{
}

CDnGuildStorageHistoryItemDlg::~CDnGuildStorageHistoryItemDlg(void)
{
}

void CDnGuildStorageHistoryItemDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "GuildStorageDescListDlg.ui" ).c_str(), bShow );
}

void CDnGuildStorageHistoryItemDlg::InitialUpdate()
{
	m_pStaticList0 = GetControl<CEtUIStatic>( "ID_LIST0" );
	m_pStaticList1 = GetControl<CEtUIStatic>( "ID_LIST1" );
	m_pStaticList2 = GetControl<CEtUIStatic>( "ID_LIST2" );
	m_pStaticList3 = GetControl<CEtUIStatic>( "ID_LIST3" );
	m_pStaticList4 = GetControl<CEtUIStatic>( "ID_LIST4" );
}

void CDnGuildStorageHistoryItemDlg::SetInfo( const WCHAR *wszText1, const WCHAR *wszText2, const WCHAR *wszText3, const WCHAR *wszText4, const WCHAR *wszText5 )
{
	m_pStaticList0->SetText( wszText1 );
	m_pStaticList1->SetText( wszText2 );
	m_pStaticList2->SetText( wszText3 );
	m_pStaticList3->SetText( wszText4 );
	m_pStaticList4->SetText( wszText5 );
}