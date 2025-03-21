#include "StdAfx.h"
#include "DnPlateMixViewDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnPlateMixViewDlg::CDnPlateMixViewDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback )
, m_pStaticProb(NULL)
, m_pItem(NULL)
, m_pItemSlotButton(NULL)
, m_pStaticItemQ(NULL)
{
}

CDnPlateMixViewDlg::~CDnPlateMixViewDlg(void)
{
	SAFE_DELETE( m_pItem );
}

void CDnPlateMixViewDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "PlateMixViewDlg.ui" ).c_str(), bShow );
}

void CDnPlateMixViewDlg::InitialUpdate()
{
	m_pStaticProb = GetControl<CEtUIStatic>("ID_PROB");
	m_pItemSlotButton = GetControl<CDnItemSlotButton>("ID_ITEMSLOT");
	m_pStaticItemQ = GetControl<CEtUIStatic>("ID_ITEM_Q");
}

void CDnPlateMixViewDlg::Show( bool bShow )
{
	if( m_bShow == bShow )
		return;

	if( bShow )
	{

	}
	else
	{
		m_pStaticProb->Show( false );

		SAFE_DELETE( m_pItem );
		m_pItemSlotButton->ResetSlot();
		m_pStaticItemQ->Show( false );
	}

	CEtUIDialog::Show( bShow );
}

void CDnPlateMixViewDlg::SetInfo( int nProb, int nItemID )
{
	if( nProb != 0 )
	{
		WCHAR wszProb[8];
		swprintf_s(wszProb, _countof(wszProb), L"%d%%", nProb );
		m_pStaticProb->SetText( wszProb );
		m_pStaticProb->Show( true );
	}

	if( nItemID != 0 )
	{
		SAFE_DELETE( m_pItem );
		TItemInfo itemInfo;
		if( CDnItem::MakeItemInfo( nItemID, 1, itemInfo ) == false ) return;
		m_pItem = GetItemTask().CreateItem( itemInfo );
		m_pItemSlotButton->SetItem(m_pItem, CDnSlotButton::ITEM_ORIGINAL_COUNT);
		m_pStaticItemQ->Show( false );
	}
	else
	{
		SAFE_DELETE( m_pItem );
		m_pItemSlotButton->ResetSlot();
		m_pStaticItemQ->Show( true );
	}
}