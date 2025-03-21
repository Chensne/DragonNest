#include "Stdafx.h"

#ifdef PRE_ADD_CASHSHOP_ACTOZ

#include "DnTableDB.h"

#include "DnCashShopTask.h"
#include "DnCashShopConfirmRefundDlg.h"


#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnCashShopConfirmRefundDlg::CDnCashShopConfirmRefundDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback, bool bAutoCursor )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback, bAutoCursor )
,m_pSlotBtn(NULL)
,m_pStaticName(NULL)
{

}

CDnCashShopConfirmRefundDlg::~CDnCashShopConfirmRefundDlg()
{

}

void CDnCashShopConfirmRefundDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName("CSRefundInvenMessage_refund.ui").c_str(), bShow );
}

void CDnCashShopConfirmRefundDlg::InitialUpdate()
{
	m_pSlotBtn = GetControl<CDnItemSlotButton>("ID_BT_ITEM");
	m_pStaticName = GetControl<CEtUIStatic>("ID_TEXT_NAME");
}

void CDnCashShopConfirmRefundDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		// 확인.
		if( IsCmdControl("ID_BT_OK") )
		{
			// 캐시인벤으로 이동요청.
			MoveCashInventory();

			Show( false );
		}

		// 취소.
		if( IsCmdControl("ID_BT_CANCEL") || IsCmdControl("ID_BT_CLOSE") )
		{			
			Show( false );
		}
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnCashShopConfirmRefundDlg::Show( bool bShow )
{
	if( m_bShow == bShow )
		return;

	CEtUIDialog::Show( bShow );

	if( bShow )
	{

	}
	else
	{
		if( m_pSlotBtn )
			m_pSlotBtn->ResetSlot();
	}
}


void CDnCashShopConfirmRefundDlg::SetItem( CDnItem * pItem )
{
	if( m_pSlotBtn && m_pStaticName )
	{
		std::wstring str( pItem->GetName() );

		DNTableFileFormat * pTable = GetDNTable( CDnTableDB::TCASHCOMMODITY );
		DNTableCell * pCell = NULL;
	
		const SCashShopItemInfo* pInfo = GetCashShopTask().GetItemInfo( pItem->GetCashItemSN() );
		if( pInfo && !pInfo->nameString.empty() )
			str.assign( pInfo->nameString );
		
		if( str == L"" || str.size() < 1 )
		{
			pCell = pTable->GetFieldFromLablePtr( pItem->GetSerialID(), "_NameID" );
			if( pCell )
				str.assign( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, pCell->GetInteger() ) );
			else
				str.assign( pItem->GetName() );

			// 이상태에서도 Name 이 비어있다면.. ItemTable까지 간다.
			if( str == L"" || str.size() < 1 )
			{			
				pCell = pTable->GetFieldFromLablePtr( pItem->GetSerialID(), "_ItemID01" );
				if( pCell )
				{
					int itemID = pCell->GetInteger();

					DNTableFileFormat * pItemTable = GetDNTable( CDnTableDB::TITEM );
					if( pItemTable )
					{
						DNTableCell * pItemCell = pItemTable->GetFieldFromLablePtr( itemID, "_NameID" );
						if( pItemCell && pItemCell->GetInteger() != 0 )
							str.assign( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, pItemCell->GetInteger() ) );
					}
				}			
			}
		}

		int count = pItem->GetOverlapCount();
		pCell = pTable->GetFieldFromLablePtr( pItem->GetSerialID(), "_Count" );
		if( pCell )
			count = pCell->GetInteger();
		m_pSlotBtn->SetItem( pItem, count );


		if( !pItem->IsEternityItem() )
		{
			if( pTable )
			{
				// 기간제인 경우 기간표시.
				pCell = pTable->GetFieldFromLablePtr( pItem->GetSerialID(), "_Period" );
				if( pCell )
				{	
					if( pCell->GetInteger() > 0 )
						str.append( FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4613 ), pCell->GetInteger() ) ); // "(%d일)"
				}
			}				
		}
		m_pStaticName->SetText( str );
	}
}


// 캐시인벤으로 이동요청.
void CDnCashShopConfirmRefundDlg::MoveCashInventory()
{
	if( m_pSlotBtn )
	{
		const CDnItem* pItem = static_cast<CDnItem*>( m_pSlotBtn->GetItem() );
		bool bPackage = CDnCashShopTask::GetInstance().IsPackageItem(pItem->GetCashItemSN());
		CDnCashShopTask::GetInstance().RequestCashShopWithdrawFromRefundInven(pItem->GetSerialID(), bPackage);
	}
}

#endif // PRE_ADD_CASHSHOP_ACTOZ