#include "Stdafx.h"

#ifdef PRE_ADD_AUTOUNPACK


#include "DnItemTask.h"

#include "DnInterface.h"
#include "DnMainMenuDlg.h"

#include "DnInvenTabDlg.h"
#include "DnInvenDlg.h"
#include "DnInvenCashDlg.h"
#include "DnInvenSlotDlg.h"

#include "DnCharmItemKeyDlg.h"

#include "DnAutoUnPackDlg.h"

#ifdef PRE_ADD_INSTANT_CASH_BUY
#include "DnInstantCashShopBuyDlg.h"
#include "DnCashShopTask.h"
#endif // PRE_ADD_INSTANT_CASH_BUY


CDnAutoUnPackDlg::CDnAutoUnPackDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback, bool bAutoCursor )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback, bAutoCursor ),
m_bUnPackIng( false ),
m_pInvenTabDlg(NULL),
m_pResigtedItem(NULL),
m_ItemClassID(0),
m_pCtrItemSlot( NULL ),
m_pStaticKeyCount( NULL ),
m_invenType(0),
m_ItemSerial(0)
{	
}


void CDnAutoUnPackDlg::ReleaseDlg()
{
	m_bUnPackIng = false;
	m_ItemClassID = 0;
	m_pResigtedItem = NULL;
}


void CDnAutoUnPackDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName("InvenBoxOpenDlg.ui").c_str(), bShow );
}


void CDnAutoUnPackDlg::InitialUpdate()
{
	m_pCtrItemSlot = GetControl<CDnItemSlotButton>( "ID_ITEM" );
	m_pStaticKeyCount = GetControl<CEtUIStatic>( "ID_TEXT_KEY" );
	m_pStaticKeyCount->SetText( std::wstring(L"") );
}


void CDnAutoUnPackDlg::Show( bool bShow )
{
	if( m_bShow == bShow )
		return;

	CEtUIDialog::Show( bShow );

	if( bShow )
	{
#ifdef PRE_ADD_INSTANT_CASH_BUY
		int nShowType = GetCashShopTask().GetInstantCashBuyEnableType( INSTANT_BUY_AUTOUNPACK );
		if( nShowType == INSTANT_BUY_HIDE )
		{
			GetControl<CEtUIButton>( "UI_BUTTON_BUYSHORTCUT" )->Show( false );
			GetControl<CEtUIStatic>( "ID_STATIC_CASHBASE" )->Show( false );
		}
		else if( nShowType == INSTANT_BUY_SHOW )
		{
			GetControl<CEtUIButton>( "UI_BUTTON_BUYSHORTCUT" )->Show( true );
			GetControl<CEtUIButton>( "UI_BUTTON_BUYSHORTCUT" )->Enable( true );
			GetControl<CEtUIStatic>( "ID_STATIC_CASHBASE" )->Show( true );
		}
		else if( nShowType == INSTANT_BUY_DISABLE )
		{
			GetControl<CEtUIButton>( "UI_BUTTON_BUYSHORTCUT" )->Show( true );
			GetControl<CEtUIButton>( "UI_BUTTON_BUYSHORTCUT" )->Enable( false );
			GetControl<CEtUIStatic>( "ID_STATIC_CASHBASE" )->Show( true );
		}
#endif // PRE_ADD_INSTANT_CASH_BUY	
	}
	else
	{
		EngUnpack();
	}	
}


void CDnAutoUnPackDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl("ID_OK") )
		{
			// 인벤토리슬롯이 남아있는지 확인.
			int needEmptyCount = (int)CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::CharmItemEmptySlotCount);
			
			// 일반인벤탭 개수확인.
			int nCurEmptySlotCount = 0;
			CDnInvenTabDlg *pInvenDlg = (CDnInvenTabDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::INVENTORY_DIALOG );
			if( pInvenDlg ) 
			{
				nCurEmptySlotCount = pInvenDlg->GetEmptySlotCount();

				// 인벤슬롯이 부족함.
				if( nCurEmptySlotCount < needEmptyCount )
				{
					wchar_t strMsg[256] = {0,};
					swprintf_s( strMsg, 256, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7052 ), needEmptyCount );
					GetInterface().MessageBox( strMsg, MB_OK );	
				}
				else
				{
					m_bUnPackIng = true;

					// 열기패킷전송.
					if( m_pResigtedItem && m_ItemSerial !=0 )
					{								
						GetItemTask().RequestAutoUnpackItem( m_ItemSerial, true );
					}
					/*else
					{
					//.. 아이템을 올려주세요.
					}*/				
				}
			}
		}
#ifdef PRE_ADD_INSTANT_CASH_BUY
		else if( IsCmdControl( "UI_BUTTON_BUYSHORTCUT" ) )
		{
			GetInterface().ShowInstantCashShopBuyDlg( true, INSTANT_BUY_AUTOUNPACK, this );
		}
#endif // PRE_ADD_INSTANT_CASH_BUY
		if( IsCmdControl("ID_CANCEL") || IsCmdControl("ID_CLOSE") )
		{
			m_bUnPackIng = false;
			Show( false );
		}
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}


// 자동열기 아이템등록.
// : 현재 인벤토리탭에 속한 인자와 동일한 아이템을 모두 찾아내어 등록한다.
bool CDnAutoUnPackDlg::RegisterItem( CDnItem * pItem )
{	 
	// 아이템 타입 검사.
	if( pItem && pItem->GetItemType() != ITEMTYPE_CHARM )
	{
		GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8051 ), MB_OK );
		return false;
	}

	bool bResult = true;

	m_pResigtedItem = NULL; // 자동열기 등록아이템.

	// 한 인벤탭에 속한 모든 SlotDlg들 - 인자의 아이템을 모든 SlotDlg에서 찾아내야함.
	if( m_pInvenTabDlg == NULL )
		m_pInvenTabDlg = static_cast<CDnInvenTabDlg * >( GetInterface().GetMainMenuDialog()->GetMainDialog( CDnMainMenuDlg::INVENTORY_DIALOG ) );
	
	std::vector< CDnInvenSlotDlg * > vSlotDlg;
	CDnInvenTabDlg::eInvenTabType invenType = m_pInvenTabDlg->GetEnableInvenType();

	// 아이템이 위치한 인벤타입저장.
	if( pItem )
		m_invenType = (int)invenType; 
	
	// 기존에 등록했던 아이템 계속열기.
	else
		invenType = (CDnInvenTabDlg::eInvenTabType)m_invenType;
	

	// [캐쉬탭] 은 다른 클래스를 사용.
	if( invenType == CDnInvenTabDlg::eInvenTabType::eCASH ) 
	{
		CDnInvenCashDlg * pInvenCashDlg = m_pInvenTabDlg->GetCashInvenDlg();		
		vSlotDlg = pInvenCashDlg->GetInvenSlotDlgList();
	}

	// [캐쉬탭이외의 탭] 들.
	else 
	{
		// InvenTabDlg 에서 사용하는 탭타입과  InvenDlg 에서 사용하는 탭타입이 달라 매칭시킴.
		ITEM_SLOT_TYPE invenDlgSlotType = ITEM_SLOT_TYPE::ST_ITEM_NONE;
		switch( invenType )
		{
		case CDnInvenTabDlg::eInvenTabType::eCOMMON :	
			invenDlgSlotType = ITEM_SLOT_TYPE::ST_INVENTORY; break;

		case CDnInvenTabDlg::eInvenTabType::eQUEST :	
			invenDlgSlotType = ITEM_SLOT_TYPE::ST_INVENTORY_QUEST; break;
		}

		CDnInvenDlg * pInvenDlg = m_pInvenTabDlg->GetInvenDlg( invenDlgSlotType );
		vSlotDlg = pInvenDlg->GetInvenSlotDlgList();		
	}
	
	if( !vSlotDlg.empty() )
	{
		int nTotCount = 0; // 해당아이템의 전체개수.
		CDnInvenSlotDlg * pSlotDlg = NULL;
		int size = (int)vSlotDlg.size();
		for( int i=0; i<size; ++i )
		{
			pSlotDlg = vSlotDlg[ i ];
			if( pSlotDlg == NULL )
				continue;

			// 아이템을 등록하면 해당슬롯버튼의 아이템과 동일한 아이템을 슬롯창에서 모두 찾아서 등록한다.
			const std::vector< CDnQuickSlotButton* > & vSlotButtons = pSlotDlg->GetSlotButtons();
			CDnQuickSlotButton * pSlotBtn = NULL;
			CDnItem * pSlotItem = NULL;

			int nItemID = pItem ? pItem->GetClassID() : m_ItemClassID;
			size_t size = vSlotButtons.size();
			for( size_t i=0; i<size; ++i )
			{
				pSlotBtn = vSlotButtons.at( i );
				if( !pSlotBtn || !pSlotBtn->GetItem() )
					continue;

				pSlotItem = static_cast< CDnItem * >( pSlotBtn->GetItem() );
				if( nItemID == pSlotItem->GetClassID() )
				{
					// 인벤슬롯에서 선택한경우에는 pItem 값이 있고, 자동열기에서 자동선택한 경우에는 pItem 이 NULL 로 보내온다. 
					// NULL 일때 인벤슬롯에 존재하는 item 으로 설정한다. 이렇게해야 캐쉬 serialnumber 가 존재한다.
					if( pItem == NULL )
						pItem = pSlotItem;

					nTotCount += pSlotItem->GetOverlapCount(); // 슬롯에속한 같은아이템의 총개수.
				}
			}
		}
		
		// SlotButton에 아이템설정.
		if( nTotCount > 0 )
		{
			m_pResigtedItem = pItem;
			m_ItemClassID = pItem->GetClassID();
			m_pCtrItemSlot->SetItem( pItem, nTotCount );	

			// 열쇠가 필요한 경우 열쇠 개수 출력.
			SetKeyItemCount( pItem );

			bResult = true;
		}	

		// 모두 소모했음, 자동열기중단.
		else
		{
			bResult = false;
		}
		
	}

	return bResult;
}


// 자동열기시작.
void CDnAutoUnPackDlg::UnPacking()
{	
	if( m_pResigtedItem )
		GetInterface().ShowCharmItemKeyDialog( true, m_pResigtedItem );
}


// 열기요청결과( 실제열기전에 받는결과 )
void CDnAutoUnPackDlg::OnRecvRequestCharmItem( char cInvenType, short sInvenIndex, INT64 biInvenSerial )
{
	GetInterface().GetCharmItemKeyDlg()->OnRecvRequestCharmItem( cInvenType, sInvenIndex, biInvenSerial );
}

// 이전상태 복원.
void CDnAutoUnPackDlg::PreStateRestore()
{
	if( m_ItemClassID == 0 )
		return;

	// 자동열기에서 자동선택한 경우에는 pItem 을 NULL 로 보낸다.
	// NULL 일때 m_ItemClassID 에 해당하는 아이템을 인벤슬롯에서 찾아설정한다. 이렇게해야 서버에서의 무결성검사를 통과할 수 있다.
	m_pResigtedItem = NULL;

	// ItemSlotButton 갱신.
	RegisterItem( m_pResigtedItem );	
}

void CDnAutoUnPackDlg::ReadyUnPack()
{
	if( m_ItemClassID == 0 )
		return;

	// 자동열기에서 자동선택한 경우에는 pItem 을 NULL 로 보낸다.
	// NULL 일때 m_ItemClassID 에 해당하는 아이템을 인벤슬롯에서 찾아설정한다. 이렇게해야 서버에서의 무결성검사를 통과할 수 있다.
	m_pResigtedItem = NULL;

	// ItemSlotButton 갱신.
	if( RegisterItem( m_pResigtedItem ) )
	{
		// 자동열기시작.
		UnPacking();
	}

	// 자동열기중단.
	else
	{
		Show( false );
	}
}

// KeyItem 개수 설정.
void CDnAutoUnPackDlg::SetKeyItemCount( CDnItem * pItem )
{
	if( !pItem ) 
		return;
	if( !CDnItemTask::IsActive() ) 
		return;
	
	int nTypeParam = pItem->GetTypeParam( 1 );

	// 열쇠 필요 O.
	if( nTypeParam == 1 || nTypeParam == 2 )
	{
		
		std::vector<CDnItem*> vecKeyItem;
		std::vector<int> vecKeyID;
		pItem->GetCharmItemKeyID( vecKeyID );

		
		for( int i = 0; i < (int)vecKeyID.size(); ++i )
		{
			GetItemTask().GetCharInventory().ScanItemFromID( vecKeyID[i], &vecKeyItem );
			GetItemTask().GetCashInventory().ScanItemFromID( vecKeyID[i], &vecKeyItem );
		}

		// 열쇠개수설정.		
		wchar_t buf[64] = {0,};
		int nKeyCnt = 0;
		if( !vecKeyItem.empty() )
		{
			int size = (int)vecKeyItem.size();
			for( int i=0; i<size; ++i )
				nKeyCnt += vecKeyItem[ i ]->GetOverlapCount();
		}			
		swprintf_s( buf, 64, GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4874), nKeyCnt );
		m_pStaticKeyCount->SetText( std::wstring(buf) );

		vecKeyItem.clear();
		vecKeyID.clear();
	}

	// 열쇠 필요 X.
	else
	{
		m_pStaticKeyCount->SetText( std::wstring(L"") );
	}

}


// 개당열기지연시간 % 설정.
void CDnAutoUnPackDlg::SetUnPackTime( int nTime )
{	
	if( nTime <= 0 )
		nTime = 100;

	m_fUnPackTimePerOwn = (float)nTime / 100.0f;
}


// 자동열기 중단.
void CDnAutoUnPackDlg::EngUnpack()
{
	m_ItemSerial = 0;
	m_invenType = 0;
	m_bUnPackIng = false;
	m_ItemClassID = 0;
	m_pResigtedItem = NULL;
	m_pCtrItemSlot->ResetSlot();
	m_pStaticKeyCount->SetText( std::wstring(L"") );
}

#ifdef PRE_ADD_INSTANT_CASH_BUY
void CDnAutoUnPackDlg::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg )
{
	if( nID == INSTANT_CASHSHOP_BUY_DIALOG )
	{
		CDnItem* pItem = static_cast<CDnItem*>( m_pCtrItemSlot->GetItem() );
		if( pItem )
			SetKeyItemCount( pItem );
	}
}
#endif // PRE_ADD_INSTANT_CASH_BUY

#endif