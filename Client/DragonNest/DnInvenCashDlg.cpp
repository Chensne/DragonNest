#include "StdAfx.h"
#include "DnInvenCashDlg.h"
#include "DnInvenSlotDlg.h"
#include "DnInven.h"
#include "DnItemTask.h"
#include "GameOption.h"
#ifdef PRE_ADD_COSTUMEMIX_TOOLTIP
#include "DnInterface.h"
#include "DnMainMenuDlg.h"
#include "DnCostumeRandomMixDlg.h"
#endif 

#ifdef PRE_ADD_CASHINVENTAB
#include "DnInvenCashTabDlg.h"
#endif // PRE_ADD_CASHINVENTAB

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


#ifdef PRE_ADD_CASHINVENTAB	
CDnInvenCashDlg::STabInfo::~STabInfo()
{
	SAFE_DELETE_PVEC( vecInvenSlotDlg );
};
#endif // PRE_ADD_CASHINVENTAB

CDnInvenCashDlg::CDnInvenCashDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
, m_nCurPage(0)
, m_nMaxPage(0)
, m_pButtonPageGroupPrev( NULL )
, m_pButtonPageGroupNext( NULL )
, m_pButtonPagePrev( NULL )
, m_pButtonPageNext( NULL )
#ifdef PRE_ADD_CASHINVENTAB
,m_pInvenCashTabDlg(NULL)
, m_newItemTabIndex(-1)
, m_crrTabType(0)
#endif // PRE_ADD_CASHINVENTAB
{
	memset( m_pButtonPage, 0, sizeof(m_pButtonPage) );
}

CDnInvenCashDlg::~CDnInvenCashDlg(void)
{

#ifdef PRE_ADD_CASHINVENTAB	
	SAFE_DELETE( m_pInvenCashTabDlg );
	SAFE_DELETE_PVEC( m_pVecTypeInvenSlotDlg );
	m_mapType.clear();	
#endif // PRE_ADD_CASHINVENTAB

	SAFE_DELETE_PVEC( m_pVecInvenSlotDlg );
}

void CDnInvenCashDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "InvenCashDlg.ui" ).c_str(), bShow );
}

void CDnInvenCashDlg::InitialUpdate()
{
	m_pButtonPagePrev = GetControl<CEtUIButton>("ID_BUTTON_PGUP");
	m_pButtonPageNext = GetControl<CEtUIButton>("ID_BUTTON_PGDN");
	m_pButtonPageGroupPrev = GetControl<CEtUIButton>("ID_BUTTON_WPGUP");
	m_pButtonPageGroupNext = GetControl<CEtUIButton>("ID_BUTTON_WPGDN");
	m_pButtonPagePrev->Enable(false);
	m_pButtonPageNext->Enable(false);
	m_pButtonPageGroupPrev->Enable(false);
	m_pButtonPageGroupNext->Enable(false);

	char szControlName[32];
	for( int i = 0; i < NUM_PAGE_PER_GROUP; ++i )
	{
		sprintf_s(szControlName, _countof(szControlName), "ID_BUTTON_PAGE%d", i);
		m_pButtonPage[i] = GetControl<CEtUIButton>(szControlName);
	}

	// 최소한 한개의 페이지는 필요하니 한개만 미리 만들어둔다.
	CDnInvenSlotDlg *pInvenSlotDlg = new CDnInvenSlotDlg( UI_TYPE_CHILD, this );	
#ifdef PRE_ADD_CASHINVENTAB
		pInvenSlotDlg->Initialize( true, 0, ITEM_MAX, "InvenCashItemSlotDlg.ui" );	
		pInvenSlotDlg->Show( false );
		pInvenSlotDlg->SetSlotType( ITEM_SLOT_TYPE::ST_INVENTORY_CASH );
		//pInvenSlotDlg->MoveDialog( 0.0f, -7.0f/DEFAULT_UI_SCREEN_HEIGHT );
#else
		pInvenSlotDlg->Initialize( true, 0, ITEM_MAX );
		pInvenSlotDlg->MoveDialog( 0.0f, -24.0f/DEFAULT_UI_SCREEN_HEIGHT );
#endif // PRE_ADD_CASHINVENTAB
	
	m_pVecInvenSlotDlg.push_back( pInvenSlotDlg );


#ifdef PRE_ADD_CASHINVENTAB
	m_crrTabType = ETABTYPE::ETABTYPE_COSTUME; // 현재탭타입.
	m_pVecTypeInvenSlotDlg.resize( ETABTYPE::ETABTYPE_MAX );	
	
	int i = ETABTYPE::ETABTYPE_COSTUME;
	for( ; i<ETABTYPE::ETABTYPE_MAX; ++i )
	{
		CDnInvenSlotDlg * pInvenSlotDlg = new CDnInvenSlotDlg( UI_TYPE_CHILD, this );
		pInvenSlotDlg->Initialize( ( i>0 ? false : true ), 0, ITEM_MAX, "InvenCashItemSlotDlg.ui" );
		//pInvenSlotDlg->MoveDialog( 0.0f, -7.0f/DEFAULT_UI_SCREEN_HEIGHT );
		pInvenSlotDlg->SetSlotType( ITEM_SLOT_TYPE::ST_INVENTORY_CASH );
		
		STabInfo * pTabInfo = new STabInfo;
		pTabInfo->crrPage = 0;
		pTabInfo->vecInvenSlotDlg.push_back( pInvenSlotDlg );

		m_pVecTypeInvenSlotDlg[ i ] = pTabInfo;
	}


	DNTableFileFormat * pSox = GetDNTable( CDnTableDB::TCASHINVENTAB );
	if( pSox )
	{
		int size = pSox->GetItemCount();
		for( int i=0; i<size; ++i )
		{
			int id = pSox->GetItemID( i );

			DNTableCell * pCell = pSox->GetFieldFromLablePtr( id, "_Tab_Type" );
			if( !pCell )
				continue;
			int tabType = pCell->GetInteger();

			pCell = pSox->GetFieldFromLablePtr( id, "_Item_Type" );
			if( !pCell )
				continue;
			int itemType = pCell->GetInteger();
			
			m_mapType.insert( make_pair( itemType, tabType ) );
		}
	}


	m_pInvenCashTabDlg = new CDnInvenCashTabDlg( UI_DIALOG_TYPE::UI_TYPE_CHILD, this );
	m_pInvenCashTabDlg->Initialize( true );

#endif // PRE_ADD_CASHINVENTAB

}

void CDnInvenCashDlg::Show( bool bShow )
{
	if( m_bShow == bShow )
		return;

	if( bShow )
	{
		RefreshPageControl();
		RefreshCashInvenSlot();
#ifdef PRE_ADD_CASHINVENTAB
		RefreshCashInvenTypeSlot();
#endif // PRE_ADD_CASHINVENTAB

	}

	else
	{
#ifdef PRE_ADD_COSTUMEMIX_TOOLTIP
		CDnCostumeRandomMixDlg* pCostumeRandomMixDlg = static_cast<CDnCostumeRandomMixDlg*>(GetInterface().GetMainMenuDialog(CDnMainMenuDlg::ITEM_COSTUME_RANDOMMIX_DIALOG));
		if( pCostumeRandomMixDlg )
			pCostumeRandomMixDlg->ResetSameItemFromCashInven();
#endif // PRE_ADD_COSTUMEMIX_TOOLTIP

	}

	CEtUIDialog::Show( bShow );
}

void CDnInvenCashDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl("ID_BUTTON_PGUP" ) )
		{
			PrevPage();
			return;
		}
		else if( IsCmdControl( "ID_BUTTON_PGDN" ) )
		{
			NextPage();
			return;
		}
		else if( IsCmdControl( "ID_BUTTON_WPGUP" ) )
		{
			PrevGroupPage();
			return;
		}
		else if( IsCmdControl( "ID_BUTTON_WPGDN" ) )
		{
			NextGroupPage();
			return;
		}
		else if( strstr( pControl->GetControlName(), "ID_BUTTON_PAGE" ) )
		{
			const std::wstring& wszPage = pControl->GetText();
			if( !wszPage.empty() )
			{
				int nPage = _wtoi( wszPage.c_str() );

#ifdef PRE_ADD_CASHINVENTAB
				STabInfo * pTab = m_pVecTypeInvenSlotDlg[ m_crrTabType ];
				pTab->crrPage = nPage - 1;
#endif // PRE_ADD_CASHINVENTAB

				m_nCurPage = nPage-1;
				RefreshPageControl();
				RefreshCashInvenSlot();

#ifdef PRE_ADD_CASHINVENTAB
				RefreshCashInvenTypeSlot();
#endif // PRE_ADD_CASHINVENTAB

				return;
			}
		}
	}
	

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

bool CDnInvenCashDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsShow() )
	{
		return false;
	}

	switch( uMsg )
	{
	case WM_MOUSEWHEEL:
		{
#ifdef PRE_ADD_CASHINVENTAB

			STabInfo * pTab = m_pVecTypeInvenSlotDlg[ m_crrTabType ];
			int crrPage = pTab->crrPage;

			if( (int)pTab->vecInvenSlotDlg.size() <= crrPage )
				break;

			if( IsMouseInDlg() || (pTab->vecInvenSlotDlg[crrPage] && pTab->vecInvenSlotDlg[crrPage]->IsMouseInDlg()) )
			{
				UINT uLines;
				SystemParametersInfo( SPI_GETWHEELSCROLLLINES, 0, &uLines, 0 );
				int nScrollAmount = int( ( short )HIWORD( wParam ) ) / WHEEL_DELTA * uLines;
				if( nScrollAmount > 0 )
				{
					PrevPage();
				}
				else if( nScrollAmount < 0 )
				{
					NextPage();
				}
			}
#else
			if( (int)m_pVecInvenSlotDlg.size() <= m_nCurPage )
				break;

			if( IsMouseInDlg() || (m_pVecInvenSlotDlg[m_nCurPage] && m_pVecInvenSlotDlg[m_nCurPage]->IsMouseInDlg()) )
			{
				UINT uLines;
				SystemParametersInfo( SPI_GETWHEELSCROLLLINES, 0, &uLines, 0 );
				int nScrollAmount = int( ( short )HIWORD( wParam ) ) / WHEEL_DELTA * uLines;
				if( nScrollAmount > 0 )
				{
					PrevPage();
				}
				else if( nScrollAmount < 0 )
				{
					NextPage();
				}
			}
#endif // PRE_ADD_CASHINVENTAB

		}
		break;
	}

	return CEtUIDialog::MsgProc( hWnd, uMsg, wParam, lParam );
}

void CDnInvenCashDlg::SetSlotType( ITEM_SLOT_TYPE slotType )
{
	for( int i = 0; i < (int)m_pVecInvenSlotDlg.size(); ++i )
		m_pVecInvenSlotDlg[i]->SetSlotType( slotType );
}

#ifdef PRE_ADD_CASHINVENTAB
void CDnInvenCashDlg::CheckSlotIndex( int nSlotIndex, bool bInsert, MIInventoryItem * _pItem )
#else
void CDnInvenCashDlg::CheckSlotIndex( int nSlotIndex, bool bInsert )
#endif // PRE_ADD_CASHINVENTAB
{

#ifdef PRE_ADD_CASHINVENTAB

	CDnItem * pItem = static_cast<CDnItem*>( _pItem );
	eItemTypeEnum itemType = pItem->GetItemType();	

	int tabIdx = ETABTYPE::ETABTYPE_ETC; // CashInvenTab 에 없는 ItemType은 '기타' 탭으로 설정.
	std::map< int, int >::iterator it = m_mapType.find( itemType );
	if( it != m_mapType.end() )
	{
		int n = it->second - 1;
		tabIdx = (int)m_pVecTypeInvenSlotDlg.size() > n ? n : ETABTYPE::ETABTYPE_ETC;
	}

	STabInfo * pTabInfo = m_pVecTypeInvenSlotDlg[ tabIdx ];
	m_newItemTabIndex = tabIdx;

	// 아이템추가.
	if( bInsert )
	{
		++pTabInfo->maxItem;

		int slotIdx = 0;
		bool bIsEmpty = false;		
		CDnInvenSlotDlg * pSlotDlg = NULL;
		int size = (int)pTabInfo->vecInvenSlotDlg.size();		
		for( int i=0; i<size; ++i )
		{
			pSlotDlg = pTabInfo->vecInvenSlotDlg[i];
			slotIdx = pSlotDlg->GetEmptySlot();
			if( slotIdx != -1 )
			{			
				bIsEmpty = true;
				break;
			}
		}

		// 빈자리가 있다. 빈자리에 추가한다.
		if( bIsEmpty )
		{
			pSlotDlg->SetItemByIndex( pItem, slotIdx );
		}

		// 빈자리가 없다. InvenSlotDlg 을 추가한다.
		else
		{
			CDnInvenSlotDlg *pInvenSlotDlg = new CDnInvenSlotDlg( UI_TYPE_CHILD, this );
			//pInvenSlotDlg->Initialize( false, (int)pTabInfo->vecInvenSlotDlg.size(), ITEM_MAX );
			pInvenSlotDlg->Initialize( false, 0, ITEM_MAX, "InvenCashItemSlotDlg.ui" );
			pInvenSlotDlg->SetSlotType( ST_INVENTORY_CASH );
			
			slotIdx = 0;		

			// 아무래도 거래소처럼 이동된 상태에서 새로 생성해야할 때가 있어서 첫 페이지의 위치와 정렬속성를 따라가는 형태로 하겠다.
			if( !pTabInfo->vecInvenSlotDlg.empty() )
			{
				UIAllignHoriType HoriType;
				UIAllignVertType VertType;
				SUICoord DlgCoord;
				pTabInfo->vecInvenSlotDlg[0]->GetAllignType( HoriType, VertType );
				pTabInfo->vecInvenSlotDlg[0]->GetDlgCoord( DlgCoord );
				pInvenSlotDlg->SetAllignType( HoriType, VertType );
				pInvenSlotDlg->SetDlgCoord( DlgCoord );			
			}
			else
			{
				//pInvenSlotDlg->MoveDialog( 0.0f, -7.0f/DEFAULT_UI_SCREEN_HEIGHT );
			}

			pSlotDlg->SetItemByIndex( pItem, slotIdx );
			pTabInfo->vecInvenSlotDlg.push_back( pInvenSlotDlg );
		}

		RefreshPageControl( true );
	}

	// 아이템제거.
	else if( pItem != NULL )
	{
		--pTabInfo->maxItem;
		if( pTabInfo->maxItem < 0 )
			pTabInfo->maxItem = 0;

		CDnInvenSlotDlg * pSlotDlg = NULL;
		int size = (int)pTabInfo->vecInvenSlotDlg.size();		
		for( int i=0; i<size; ++i )
		{
			pSlotDlg = pTabInfo->vecInvenSlotDlg[i];
			
			const std::vector< CDnQuickSlotButton* > & vSlots = pSlotDlg->GetSlotButtons();
			int cnt = (int)vSlots.size();
			for( int x=0; x<cnt; ++x )
			{
				CDnItem * pSlotItem = (CDnItem*)vSlots[ x ]->GetItem();
				if( pSlotItem && pSlotItem->GetSerialID() == pItem->GetSerialID() )
				{
					vSlots[ x ]->ResetSlot();
					vSlots[ x ]->OnRefreshTooltip();
				}
			}
		}

		RefreshPageControl();
		RefreshCashInvenTypeSlot();
	}

#endif // PRE_ADD_CASHINVENTAB





	if( bInsert )
	{
		// 현재 슬롯을 표현하기 위해 필요한 페이지 개수
		int nNumPage = (nSlotIndex / ITEM_MAX) + 1;
		if( (int)m_pVecInvenSlotDlg.size() < nNumPage )
		{
			bool bOrigSmartMove = CGameOption::GetInstance().m_bSmartMove;
			CGameOption::GetInstance().m_bSmartMove = false;
			int nNewCount = nNumPage - (int)m_pVecInvenSlotDlg.size();
			for( int i = 0; i < nNewCount; ++i )
			{
				CDnInvenSlotDlg *pInvenSlotDlg = new CDnInvenSlotDlg( UI_TYPE_CHILD, this );
#ifdef PRE_ADD_CASHINVENTAB			
				pInvenSlotDlg->Initialize( false, (int)m_pVecInvenSlotDlg.size(), ITEM_MAX, "InvenCashItemSlotDlg.ui" );
#else
				pInvenSlotDlg->Initialize( false, (int)m_pVecInvenSlotDlg.size(), ITEM_MAX );
#endif // PRE_ADD_CASHINVENTAB
				pInvenSlotDlg->SetSlotType( ST_INVENTORY_CASH );

				// 아무래도 거래소처럼 이동된 상태에서 새로 생성해야할 때가 있어서 첫 페이지의 위치와 정렬속성를 따라가는 형태로 하겠다.
				if( !m_pVecInvenSlotDlg.empty() )
				{
					UIAllignHoriType HoriType;
					UIAllignVertType VertType;
					SUICoord DlgCoord;
					m_pVecInvenSlotDlg[0]->GetAllignType( HoriType, VertType );
					m_pVecInvenSlotDlg[0]->GetDlgCoord( DlgCoord );
					pInvenSlotDlg->SetAllignType( HoriType, VertType );
					pInvenSlotDlg->SetDlgCoord( DlgCoord );
				}
				else
				{
#ifdef PRE_ADD_CASHINVENTAB	
					//pInvenSlotDlg->MoveDialog( 0.0f, -7.0f/DEFAULT_UI_SCREEN_HEIGHT );
#else
					pInvenSlotDlg->MoveDialog( 0.0f, -24.0f/DEFAULT_UI_SCREEN_HEIGHT );
#endif // #ifdef PRE_ADD_CASHINVENTAB	
				}

#ifdef PRE_ADD_CASHINVENTAB
				pInvenSlotDlg->Show( false );
#endif // PRE_ADD_CASHINVENTAB

				m_pVecInvenSlotDlg.push_back( pInvenSlotDlg );
			}
			CGameOption::GetInstance().m_bSmartMove = bOrigSmartMove;
			RefreshPageControl( true );
		}
	}
	else
	{
		// 현재 인벤토리의 마지막 아이템을 보여주기 위해 필요한 페이지 개수를 구해와 페이지 개수 설정.
		int nLastCashInvenSlotIndex = 0;
		std::map<int,CDnItem*>::reverse_iterator iter = GetItemTask().GetCashInventory().GetInventoryItemList().rbegin();
		if( iter != GetItemTask().GetCashInventory().GetInventoryItemList().rend() )
			nLastCashInvenSlotIndex = iter->first;
		int nMaxPage = nLastCashInvenSlotIndex / ITEM_MAX;

		// 현재 초기화하는 슬롯과 마지막 슬롯 인덱스가 같다면, 맨 마지막에 있는 아이템을 삭제하는거다.
		// (아이템을 삭제할때 슬롯을 먼저 초기화시키고 인벤토리 리스트에서 빼기때문에 인벤토리 내용엔 아직 삭제될게 남아있어서 이렇게 처리하는 거다.)
		if( nSlotIndex == nLastCashInvenSlotIndex )
		{
			// 이럴땐 뒤쪽에서 두번째 위치하는 아이템의 슬롯인덱스를 구해서,
			int nLastCashInvenSlotIndex2 = 0;
			std::map<int,CDnItem*>::reverse_iterator iter2 = GetItemTask().GetCashInventory().GetInventoryItemList().rbegin();
			if( iter2 != GetItemTask().GetCashInventory().GetInventoryItemList().rend() )
			{
				++iter2;
				if( iter2 != GetItemTask().GetCashInventory().GetInventoryItemList().rend() )
				{
					nLastCashInvenSlotIndex2 = iter2->first;
					int nMaxPage2 = nLastCashInvenSlotIndex2 / ITEM_MAX;
					if( nMaxPage > nMaxPage2 )
					{
						nMaxPage = nMaxPage2;
						m_nMaxPage = nMaxPage;
					}
				}
			}
		}

		// 해당 슬롯을 표현하기 위해 필요한 페이지 개수
		int nNumPage = nMaxPage+1;

		if( (int)m_pVecInvenSlotDlg.size() > nNumPage )
		{
			int nDelCount = (int)m_pVecInvenSlotDlg.size() - nNumPage;
			for( int i = 0; i < nDelCount; ++i )
				SAFE_DELETE( m_pVecInvenSlotDlg[nNumPage+i] );
			for( int i = 0; i < nDelCount; ++i )
				m_pVecInvenSlotDlg.pop_back();

			RefreshPageControl();
			RefreshCashInvenSlot();
		}
	}
}

//void CDnInvenCashDlg::CheckSlotIndex( int nSlotIndex, bool bInsert )
//{
//	if( bInsert )
//	{
//		// 현재 슬롯을 표현하기 위해 필요한 페이지 개수
//		int nNumPage = (nSlotIndex / ITEM_MAX) + 1;
//		if( (int)m_pVecInvenSlotDlg.size() < nNumPage )
//		{
//			bool bOrigSmartMove = CGameOption::GetInstance().m_bSmartMove;
//			CGameOption::GetInstance().m_bSmartMove = false;
//			int nNewCount = nNumPage - (int)m_pVecInvenSlotDlg.size();
//			for( int i = 0; i < nNewCount; ++i )
//			{
//				CDnInvenSlotDlg *pInvenSlotDlg = new CDnInvenSlotDlg( UI_TYPE_CHILD, this );
//				pInvenSlotDlg->Initialize( false, (int)m_pVecInvenSlotDlg.size(), ITEM_MAX );
//				pInvenSlotDlg->SetSlotType( ST_INVENTORY_CASH );
//
//				// 아무래도 거래소처럼 이동된 상태에서 새로 생성해야할 때가 있어서 첫 페이지의 위치와 정렬속성를 따라가는 형태로 하겠다.
//				if( !m_pVecInvenSlotDlg.empty() )
//				{
//					UIAllignHoriType HoriType;
//					UIAllignVertType VertType;
//					SUICoord DlgCoord;
//					m_pVecInvenSlotDlg[0]->GetAllignType( HoriType, VertType );
//					m_pVecInvenSlotDlg[0]->GetDlgCoord( DlgCoord );
//					pInvenSlotDlg->SetAllignType( HoriType, VertType );
//					pInvenSlotDlg->SetDlgCoord( DlgCoord );
//				}
//				else
//				{
//					pInvenSlotDlg->MoveDialog( 0.0f, -24.0f/DEFAULT_UI_SCREEN_HEIGHT );
//				}
//				m_pVecInvenSlotDlg.push_back( pInvenSlotDlg );
//			}
//			CGameOption::GetInstance().m_bSmartMove = bOrigSmartMove;
//			RefreshPageControl( true );
//		}
//	}
//	else
//	{
//		// 현재 인벤토리의 마지막 아이템을 보여주기 위해 필요한 페이지 개수를 구해와 페이지 개수 설정.
//		int nLastCashInvenSlotIndex = 0;
//		std::map<int,CDnItem*>::reverse_iterator iter = GetItemTask().GetCashInventory().GetInventoryItemList().rbegin();
//		if( iter != GetItemTask().GetCashInventory().GetInventoryItemList().rend() )
//			nLastCashInvenSlotIndex = iter->first;
//		int nMaxPage = nLastCashInvenSlotIndex / ITEM_MAX;
//
//		// 현재 초기화하는 슬롯과 마지막 슬롯 인덱스가 같다면, 맨 마지막에 있는 아이템을 삭제하는거다.
//		// (아이템을 삭제할때 슬롯을 먼저 초기화시키고 인벤토리 리스트에서 빼기때문에 인벤토리 내용엔 아직 삭제될게 남아있어서 이렇게 처리하는 거다.)
//		if( nSlotIndex == nLastCashInvenSlotIndex )
//		{
//			// 이럴땐 뒤쪽에서 두번째 위치하는 아이템의 슬롯인덱스를 구해서,
//			int nLastCashInvenSlotIndex2 = 0;
//			std::map<int,CDnItem*>::reverse_iterator iter2 = GetItemTask().GetCashInventory().GetInventoryItemList().rbegin();
//			if( iter2 != GetItemTask().GetCashInventory().GetInventoryItemList().rend() )
//			{
//				++iter2;
//				if( iter2 != GetItemTask().GetCashInventory().GetInventoryItemList().rend() )
//				{
//					nLastCashInvenSlotIndex2 = iter2->first;
//					int nMaxPage2 = nLastCashInvenSlotIndex2 / ITEM_MAX;
//					if( nMaxPage > nMaxPage2 )
//					{
//						nMaxPage = nMaxPage2;
//						m_nMaxPage = nMaxPage;
//					}
//				}
//			}
//		}
//
//		// 해당 슬롯을 표현하기 위해 필요한 페이지 개수
//		int nNumPage = nMaxPage+1;
//
//		if( (int)m_pVecInvenSlotDlg.size() > nNumPage )
//		{
//			int nDelCount = (int)m_pVecInvenSlotDlg.size() - nNumPage;
//			for( int i = 0; i < nDelCount; ++i )
//				SAFE_DELETE( m_pVecInvenSlotDlg[nNumPage+i] );
//			for( int i = 0; i < nDelCount; ++i )
//				m_pVecInvenSlotDlg.pop_back();
//
//			RefreshPageControl();
//			RefreshCashInvenSlot();
//		}
//	}
//}


void CDnInvenCashDlg::SetItem( MIInventoryItem *pItem )
{
	ASSERT(pItem&&"CDnInvenCashDlg::SetItem");
	if( !pItem ) return;

	int nSlotIndex = pItem->GetSlotIndex();
	int nSlotPage = nSlotIndex / ITEM_MAX;

	// 새로운 아이템이 들어올때마다 페이지를 갱신한다.
#ifdef PRE_ADD_CASHINVENTAB
	CheckSlotIndex( nSlotIndex, true, pItem );
#else
	CheckSlotIndex( nSlotIndex, true );
#endif // PRE_ADD_CASHINVENTAB

	if( (int)m_pVecInvenSlotDlg.size() <= nSlotPage )
	{
		ASSERT(0&&"CDnInvenCashDlg::SetItem");
		return;
	}

	m_pVecInvenSlotDlg[nSlotPage]->SetItem( pItem );
}

void CDnInvenCashDlg::ResetSlot( int nSlotIndex )
{
	int nSlotPage = nSlotIndex / ITEM_MAX;

	if( (int)m_pVecInvenSlotDlg.size() <= nSlotPage )
	{
		ASSERT(0&&"CDnInvenCashDlg::ResetSlot");
		return;
	}

#ifdef PRE_ADD_CASHINVENTAB
	CDnItem * pItem = m_pVecInvenSlotDlg[nSlotPage]->GetSlotItem( nSlotIndex );
#endif // PRE_ADD_CASHINVENTAB

	m_pVecInvenSlotDlg[nSlotPage]->ResetSlot( nSlotIndex );

	// 아이템이 삭제될때 페이지를 갱신한다.
#ifdef PRE_ADD_CASHINVENTAB
	CheckSlotIndex( nSlotIndex, false, pItem );
#else
	CheckSlotIndex( nSlotIndex, false );
#endif // PRE_ADD_CASHINVENTAB
	
}

#ifdef PRE_ADD_LEVELUP_REWARD_NOTIFIER
void CDnInvenCashDlg::RefreshInvenPageFromSlotIndex( int nSlotIndex )
{
	int nSlotPage = nSlotIndex / ITEM_MAX;

	if( m_nMaxPage < nSlotPage ) 
		m_nCurPage = m_nMaxPage;
	else	
		m_nCurPage = nSlotPage;

	RefreshPageControl();
	RefreshCashInvenSlot();
}
void CDnInvenCashDlg::SetFocusBlink( int nSlotIndex, int nBlinkTime, int nEndTime )
{
	int nSlotPage = nSlotIndex / ITEM_MAX;
	
	if ( m_nMaxPage < nSlotPage ) 
		m_nCurPage = m_nMaxPage;
	else	
		m_nCurPage = nSlotPage;

	CDnQuickSlotButton* pFindSlotButton(NULL);
	const std::vector< CDnQuickSlotButton* > vQuickSlotButtons = m_pVecInvenSlotDlg[nSlotPage]->GetSlotButtons();
	for ( int i = 0; i < (int)vQuickSlotButtons.size(); ++i )
	{
		if ( vQuickSlotButtons[i]->GetSlotIndex() == nSlotIndex )
		{
			pFindSlotButton = vQuickSlotButtons[i];	
			break;
		}
	}
	if ( pFindSlotButton )
	{
		pFindSlotButton->SetBlink( true );
		pFindSlotButton->SetBlendTime( nBlinkTime ); 
		pFindSlotButton->EndBlinkTime( nEndTime );
	}
	
}

#endif

void CDnInvenCashDlg::RefreshPageControl( bool bCheckMaxPage )
{

#ifdef PRE_ADD_CASHINVENTAB

	int maxPage = GetCrrMaxPage();
	STabInfo * pTab = m_pVecTypeInvenSlotDlg[ m_crrTabType ];		

	// 현재 페이지가 맥스 페이지를 넘었다면 유효한 범위 안으로 갱신
	if( pTab->crrPage > maxPage )
		pTab->crrPage = maxPage;
	if( pTab->crrPage < 0 ) pTab->crrPage = 0;

	// 현재 페이지에 따라 페이지 그룹을 설정하고(페이지 그룹이 0이면 1,2,3,4,5 페이지가 보이고 1이면 6,7,8,9,10 페이지가 보인다.)
	int nPageGroup = pTab->crrPage / NUM_PAGE_PER_GROUP;
	int nMaxPageGroup = maxPage / NUM_PAGE_PER_GROUP;

	// 페이지 그룹과 현재 페이지를 바탕으로 페이지 컨트롤 설정.
	for( int i = 0; i < NUM_PAGE_PER_GROUP; ++i )
	{
		m_pButtonPage[i]->ClearText();
		int nPage = nPageGroup*NUM_PAGE_PER_GROUP+i;
		if( nPage <= maxPage )
		{
			m_pButtonPage[i]->SetIntToText( nPage+1 );
			m_pButtonPage[i]->Enable( pTab->crrPage != nPage );
		}
	}

	m_pButtonPageNext->Enable( !(pTab->crrPage == maxPage) );
	m_pButtonPagePrev->Enable( !(pTab->crrPage==0) );
	m_pButtonPageGroupNext->Enable( !(nPageGroup == nMaxPageGroup) );
	m_pButtonPageGroupPrev->Enable( !(nPageGroup==0) );

	//if( bCheckMaxPage )
	//{
	//	// 맥스 페이지 설정하고(페이지가 한개면 0, 두개면 1인 최고 인덱스 저장이다.)
	//	int nLastCashInvenSlotIndex = 0;
	//	std::map<int,CDnItem*>::reverse_iterator iter = GetItemTask().GetCashInventory().GetInventoryItemList().rbegin();
	//	if( iter != GetItemTask().GetCashInventory().GetInventoryItemList().rend() )
	//		nLastCashInvenSlotIndex = iter->first;
	//	m_nMaxPage = nLastCashInvenSlotIndex / ITEM_MAX;
	//}

#else 

	if( bCheckMaxPage )
	{
		// 맥스 페이지 설정하고(페이지가 한개면 0, 두개면 1인 최고 인덱스 저장이다.)
		int nLastCashInvenSlotIndex = 0;
		std::map<int,CDnItem*>::reverse_iterator iter = GetItemTask().GetCashInventory().GetInventoryItemList().rbegin();
		if( iter != GetItemTask().GetCashInventory().GetInventoryItemList().rend() )
			nLastCashInvenSlotIndex = iter->first;
		m_nMaxPage = nLastCashInvenSlotIndex / ITEM_MAX;
	}

	// 현재 페이지가 맥스 페이지를 넘었다면 유효한 범위 안으로 갱신
	if( m_nCurPage > m_nMaxPage )
		m_nCurPage = m_nMaxPage;
	if( m_nCurPage < 0 ) m_nCurPage = 0;

	// 현재 페이지에 따라 페이지 그룹을 설정하고(페이지 그룹이 0이면 1,2,3,4,5 페이지가 보이고 1이면 6,7,8,9,10 페이지가 보인다.)
	int nPageGroup = m_nCurPage / NUM_PAGE_PER_GROUP;
	int nMaxPageGroup = m_nMaxPage / NUM_PAGE_PER_GROUP;

	// 페이지 그룹과 현재 페이지를 바탕으로 페이지 컨트롤 설정.
	for( int i = 0; i < NUM_PAGE_PER_GROUP; ++i )
	{
		m_pButtonPage[i]->ClearText();
		int nPage = nPageGroup*NUM_PAGE_PER_GROUP+i;
		if( nPage <= m_nMaxPage )
		{
			m_pButtonPage[i]->SetIntToText( nPage+1 );
			m_pButtonPage[i]->Enable( m_nCurPage != nPage );
		}
	}

	m_pButtonPageNext->Enable( !(m_nCurPage == m_nMaxPage) );
	m_pButtonPagePrev->Enable( !(m_nCurPage==0) );
	m_pButtonPageGroupNext->Enable( !(nPageGroup == nMaxPageGroup) );
	m_pButtonPageGroupPrev->Enable( !(nPageGroup==0) );

#endif // PRE_ADD_CASHINVENTAB

}

void CDnInvenCashDlg::RefreshCashInvenSlot()
{
#ifdef PRE_ADD_CASHINVENTAB
    // ...
#else
	for( int i = 0; i < (int)m_pVecInvenSlotDlg.size(); ++i )
		m_pVecInvenSlotDlg[i]->Show( i == m_nCurPage );
#endif // PRE_ADD_CASHINVENTAB

}

void CDnInvenCashDlg::NextPage()
{

#ifdef PRE_ADD_CASHINVENTAB

	STabInfo * pTab = m_pVecTypeInvenSlotDlg[ m_crrTabType ];
	int crrPage = pTab->crrPage;
	int size = (int)pTab->vecInvenSlotDlg.size();

	if( pTab->crrPage < size )
	{	
		++pTab->crrPage;
		if( pTab->crrPage >= size )
			pTab->crrPage = size - 1;
		RefreshPageControl();
		RefreshCashInvenTypeSlot();
	}

#else

	if( m_nCurPage < m_nMaxPage ) {
		m_nCurPage++;
		if( m_nCurPage > m_nMaxPage ) m_nCurPage = m_nMaxPage;
		RefreshPageControl();
		RefreshCashInvenSlot();
	}

#endif // PRE_ADD_CASHINVENTAB

}

void CDnInvenCashDlg::PrevPage()
{

#ifdef PRE_ADD_CASHINVENTAB
	STabInfo * pTab = m_pVecTypeInvenSlotDlg[ m_crrTabType ];	
	if( pTab->crrPage > 0 )
	{	
		--pTab->crrPage;
		if( pTab->crrPage < 0 )
			pTab->crrPage = 0;
		RefreshPageControl();
		RefreshCashInvenTypeSlot();
	}

#else

	if( m_nCurPage > 0 ) {
		m_nCurPage--;
		if( m_nCurPage < 0 ) m_nCurPage = 0;
		RefreshPageControl();
		RefreshCashInvenSlot();
	}

#endif // PRE_ADD_CASHINVENTAB

}

void CDnInvenCashDlg::NextGroupPage()
{
#ifdef PRE_ADD_CASHINVENTAB
	STabInfo * pTab = m_pVecTypeInvenSlotDlg[ m_crrTabType ];

	int _nPageGroup = pTab->crrPage / NUM_PAGE_PER_GROUP;
	int _nMaxPageGroup = GetCrrMaxPage() / NUM_PAGE_PER_GROUP;

	if( _nPageGroup < _nMaxPageGroup )
	{
		_nPageGroup++;
		if( _nPageGroup > _nMaxPageGroup ) _nPageGroup = _nMaxPageGroup;
		m_nCurPage = _nPageGroup * NUM_PAGE_PER_GROUP;
		RefreshPageControl();
		RefreshCashInvenTypeSlot();
	}

#endif // PRE_ADD_CASHINVENTAB

	int nPageGroup = m_nCurPage / NUM_PAGE_PER_GROUP;
	int nMaxPageGroup = m_nMaxPage / NUM_PAGE_PER_GROUP;

	if( nPageGroup < nMaxPageGroup ) {
		nPageGroup++;
		if( nPageGroup > nMaxPageGroup ) nPageGroup = nMaxPageGroup;
		m_nCurPage = nPageGroup * NUM_PAGE_PER_GROUP;
		RefreshPageControl();
		RefreshCashInvenSlot();
	}
}

void CDnInvenCashDlg::PrevGroupPage()
{

#ifdef PRE_ADD_CASHINVENTAB
	STabInfo * pTab = m_pVecTypeInvenSlotDlg[ m_crrTabType ];

	int _nPageGroup = pTab->crrPage / NUM_PAGE_PER_GROUP;
	if( _nPageGroup > 0 )
	{
		_nPageGroup--;
		if( _nPageGroup < 0 ) _nPageGroup = 0;
		pTab->crrPage = _nPageGroup * NUM_PAGE_PER_GROUP;
		RefreshPageControl();
		RefreshCashInvenTypeSlot();
	}

#endif // PRE_ADD_CASHINVENTAB

	int nPageGroup = m_nCurPage / NUM_PAGE_PER_GROUP;
	int nMaxPageGroup = m_nMaxPage / NUM_PAGE_PER_GROUP;

	if( nPageGroup > 0 ) {
		nPageGroup--;
		if( nPageGroup < 0 ) nPageGroup = 0;
		m_nCurPage = nPageGroup * NUM_PAGE_PER_GROUP;
		RefreshPageControl();
		RefreshCashInvenSlot();
	}
}

void CDnInvenCashDlg::ReleaseNewGain()
{
	if( !CDnItemTask::IsActive() )
		return;

	if( GetItemTask().GetCashInventory().GetInventoryItemList().empty() )
		return;

	CDnItem *pItem(NULL);
	CDnCashInventory::INVENTORY_MAP_ITER iter = GetItemTask().GetCashInventory().GetInventoryItemList().begin();
	for( ; iter != GetItemTask().GetCashInventory().GetInventoryItemList().end(); ++iter )
	{
		pItem = iter->second;
		if( !pItem ) continue;
		pItem->SetNewGain( false );
	}
}

int CDnInvenCashDlg::GetRegisteredItemCount()
{
	if( !CDnItemTask::IsActive() )
		return 0;

	if( GetItemTask().GetCashInventory().GetInventoryItemList().empty() )
		return 0;

	int nCount = 0;
	for( int i = 0; i < (int)m_pVecInvenSlotDlg.size(); ++i )
		nCount += m_pVecInvenSlotDlg[i]->GetRegisteredSlotCount();

	return nCount;
}

void CDnInvenCashDlg::MoveDialog( float fX, float fY ) 
{
	// 거래소에서 인벤토리 일괄적으로 이동시켜 작게 보여주는 기능때문에 이렇게 별도로 처리한다.
	CEtUIDialog::MoveDialog( fX, fY );
	for( int i = 0; i < (int)m_pVecInvenSlotDlg.size(); ++i )
		m_pVecInvenSlotDlg[i]->MoveDialog( fX, fY );

#ifdef PRE_ADD_CASHINVENTAB
	int size = (int)m_pVecTypeInvenSlotDlg.size();
	for( int i=0; i<size; ++i )
	{
		STabInfo * pTab = m_pVecTypeInvenSlotDlg[ i ];
		for( int x = 0; x < (int)pTab->vecInvenSlotDlg.size(); ++x )
			pTab->vecInvenSlotDlg[x]->MoveDialog( fX, fY );
	}

	if( m_pInvenCashTabDlg )
		m_pInvenCashTabDlg->MoveDialog( fX, fY );
#endif // PRE_ADD_CASHINVENTAB

}

#ifdef PRE_ADD_CASHINVENTAB

void CDnInvenCashDlg::RefreshCashInvenTypeSlot()
{
	STabInfo * pTab = m_pVecTypeInvenSlotDlg[ m_crrTabType ];
	for( int i = 0; i < (int)pTab->vecInvenSlotDlg.size(); ++i )
		pTab->vecInvenSlotDlg[i]->Show( i == pTab->crrPage );
}


void CDnInvenCashDlg::ChangeTab( int idx )
{
	if( m_crrTabType == idx )
		return;

	int preIdx = m_crrTabType;
	m_crrTabType = idx;

	//// 전체.
	//if( m_crrTabType == ETABTYPE::ETABTYPE_COSTUME )
	//{
	//	// 전체 탭 보임.
	//	RefreshCashInvenSlot();		
	//	
	//	// 이전 탭 숨김.
	//	STabInfo * pp = m_pVecTypeInvenSlotDlg[ preIdx ];
	//	pp->vecInvenSlotDlg[ pp->crrPage ]->Show( false );
	//}

	//// 
	//else
	//{
	//	// 전체 탭 숨김.
	//	m_pVecInvenSlotDlg[ m_nCurPage ]->Show( false );
	//
	//	// 선택 탭 보임.
	//	STabInfo * pp = m_pVecTypeInvenSlotDlg[ m_crrTabType ];
	//	RefreshCashInvenSlot( pp->vecInvenSlotDlg, pp->crrPage );
	//}

	// 이전 탭 숨김.
	STabInfo * pp = m_pVecTypeInvenSlotDlg[ preIdx ];
	pp->vecInvenSlotDlg[ pp->crrPage ]->Show( false );

	// 선택 탭 보임.
	pp = m_pVecTypeInvenSlotDlg[ m_crrTabType ];

	RefreshPageControl();
	RefreshCashInvenTypeSlot();

}

int CDnInvenCashDlg::GetCrrMaxPage()
{
	int maxPage = 0;
	STabInfo * pTab = m_pVecTypeInvenSlotDlg[ m_crrTabType ];		
	
	// 마지막슬롯인덱스 구하기.
	int nLastCashInvenSlotIndex = 0;
	bool bEnd = false;		

	int size = (int)pTab->vecInvenSlotDlg.size() - 1;
	for( int i=size; i>=0; --i )
	{
		CDnInvenSlotDlg * pSlotDlg = pTab->vecInvenSlotDlg[ i ] ;

		const std::vector< CDnQuickSlotButton* > & vSlots = pSlotDlg->GetSlotButtons();
		int cnt = (int)vSlots.size() - 1;
		int last = ITEM_MAX;
		for( int x=cnt; x>=0; --x )
		{
			if( vSlots[ x ]->IsEmptySlot() )
				last = x;
			else
			{
				//nLastCashInvenSlotIndex = i * ITEM_MAX + last;
				nLastCashInvenSlotIndex = i * ITEM_MAX + x;
				bEnd = true;
				break;
			}
		}

		if( bEnd )
			break;
	}

	maxPage = ( nLastCashInvenSlotIndex == 0 ? 0 : (nLastCashInvenSlotIndex / ITEM_MAX) );

	return maxPage;
}

void CDnInvenCashDlg::AddNewItem()
{
	
	if( m_newItemTabIndex == -1 )
		return;

	if( m_pInvenCashTabDlg )
		m_pInvenCashTabDlg->AddNewItem( m_newItemTabIndex );
	
	m_newItemTabIndex = -1;
}

void CDnInvenCashDlg::SetAllignTypeCashTab( UIAllignHoriType Hori, UIAllignVertType Vert )
{
	int size = (int)m_pVecTypeInvenSlotDlg.size();
	for( int i=0; i<size; ++i )
	{
		STabInfo * pTab = m_pVecTypeInvenSlotDlg[ i ];
		for( int x = 0; x < (int)pTab->vecInvenSlotDlg.size(); ++x )
			pTab->vecInvenSlotDlg[x]->SetAllignType( Hori, Vert );
	}

	if( m_pInvenCashTabDlg )
		m_pInvenCashTabDlg->SetAllignType( Hori, Vert );
}


CDnItem * CDnInvenCashDlg::GetCashInvenItemBySN( INT64 SN )
{
	int size = (int)m_pVecInvenSlotDlg.size();
	for( int i=0; i<size; ++i )
	{
		const std::vector< CDnQuickSlotButton* > vQuickSlotButtons = m_pVecInvenSlotDlg[ i ]->GetSlotButtons();
		for( int x = 0; x < (int)vQuickSlotButtons.size(); ++x )
		{
			MIInventoryItem * miItem = vQuickSlotButtons[x]->GetItem();
			if( miItem )
			{
				CDnItem * pItem = static_cast<CDnItem*>( miItem );
				if( pItem->GetSerialID() == SN )
					return pItem;
			}
		}
	}

	return NULL;
}


#endif // PRE_ADD_CASHINVENTAB