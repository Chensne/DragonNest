#include "StdAfx.h"
#include "DnPotentialTransferInfoItemDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

#ifdef PRE_ADD_EXCHANGE_POTENTIAL

CDnPotentialTransferInfoItemDlg::CDnPotentialTransferInfoItemDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
, m_pStaticList0(NULL)
, m_pStaticList1(NULL)
, m_pStaticList2(NULL)
{
}

CDnPotentialTransferInfoItemDlg::~CDnPotentialTransferInfoItemDlg(void)
{
}

void CDnPotentialTransferInfoItemDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "InvenPotentialOptionListDlg.ui" ).c_str(), bShow );
}

void CDnPotentialTransferInfoItemDlg::InitialUpdate()
{
	m_pStaticList0 = GetControl<CEtUIStatic>( "ID_TEXT_ITEM0" );
	m_pStaticList1 = GetControl<CEtUIStatic>( "ID_TEXT_ITEM1" );
	m_pStaticList2 = GetControl<CEtUIStatic>( "ID_TEXT_ITEM2" );
}

void CDnPotentialTransferInfoItemDlg::SetInfo( const WCHAR *wszText1, const WCHAR *wszText2, const WCHAR *wszText3, DWORD dwColor )
{
	m_pStaticList0->SetText( wszText1 );
	m_pStaticList1->SetText( wszText2 );
	m_pStaticList2->SetText( wszText3 );

	m_pStaticList0->SetTextColor( dwColor );
	m_pStaticList1->SetTextColor( dwColor );
}

#endif