#include "StdAfx.h"

#ifdef PRE_ADD_CHOICECUBE

#include "DnTableDB.h"
#include "DNTableFile.h"
#include "DNPacket.h"
#include "DnMainFrame.h"
#include "DnItemTask.h"
#include "DnCharInventory.h"
#include "DnCashInventory.h"
#include "DnPetInventory.h"
#include "DnInterface.h"
#include "DnMainMenuDlg.h"
#include "DnInvenTabDlg.h"
#include "DnChaosCubeStuffDlg.h"
#include "DnChaosCubeDlg.h"
#include "DnChoiceCubeDlg.h"

// Construct.
CDnChoiceCubeDlg::CDnChoiceCubeDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback, bool bAutoCursor )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback, bAutoCursor ),
m_pItemChaos(NULL), // 카오스아이템.
m_nChaosSerial(0),
m_CountPerPage(5), // 페이지당 슬롯개수.
m_crrPage(0), // 현재페이지.
m_maxPage(0), // 전체페이지.
m_signLight(1), // 선택재료슬롯 빛남처리 부호.
m_fAlphaColor(1.0f),
m_bSelfItemList(true), // 재료템과 같은 아이템 로딩여부.
m_SelectedSlotIndex(-1) // 선택한 재료슬롯 인덱스.
{
	m_pSlotBtnSelected = NULL; // 선택한재료슬롯.	
	m_pStaticLight = NULL; // 반짝임.
	m_pStaticTitle = NULL; // 타이틀바.
	m_pStaticHelp = NULL;  // 도움말.
	m_pStaticPage = NULL;  // Page/MaxPage.

	m_pBtnPrev = NULL;     // 이전.
	m_pBtnNext = NULL;     // 다음.


	m_pStaticLightResult = NULL;
	m_pStaticPageResult = NULL;
	m_pSlotBtnSelectedResult = NULL; // 선택한 결과물슬롯.

	m_pBtnMaterial = NULL; // 재료.
	m_pBtnProduct = NULL;  // 결과물.
	m_pBtnConfirm = NULL;  // 확인.

	m_pWheelRegionStuff = NULL; // 재료슬롯 휠 영역.
	m_pWheelRegionResult = NULL; // 결과물슬롯 휠 영역.

	m_crrPageResult = 0;
	m_maxPageResult = 0;

	m_SelectedSlotIndexResult = -1;
	m_fAlphaColorResult = 1.0f;

	m_fAlphaColorResult = 1.0f;
	m_signLightResult = 1;  
}


// ReleaseDlg()
void CDnChoiceCubeDlg::ReleaseDlg()
{
	m_vSlotBtnMaterials.clear(); // 재료슬롯들.
	m_vStaticSelect.clear(); // 재료슬롯 선택 Static.
	m_vNeedStuffCount.clear(); // 믹스에 필요한 재료개수.
	m_vStuffItemIDs.clear(); // 재료아이템.


	m_vSlotBtnMaterialsResult.clear(); // 재료슬롯들.
	m_vStaticSelectResult.clear(); // 재료슬롯 선택 Static.

	m_vResultItems.clear(); // 결과아이템들.

	m_vPeriodItems.clear(); // 결과슬롯에 기간제표시용 Static.

}


// Initialize()
void CDnChoiceCubeDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName("ChoiceBoxDlg.ui").c_str(), bShow );
}


// InitialUpdate()
void CDnChoiceCubeDlg::InitialUpdate()
{

	//---  재료슬롯들  ---//
	m_pSlotBtnSelected = GetControl<CDnItemSlotButton>("ID_ITEM0"); // 선택한재료슬롯.
	m_pStaticLight = GetControl<CEtUIStatic>("ID_STATIC_LIGHT0");    // 반짝임.
		
	m_pStaticPage = GetControl<CEtUIStatic>("ID_TEXT_PAGE0"); // Page/MaxPage.

	// 재료슬롯들.
	char buf[64] = {0,};
	m_vSlotBtnMaterials.resize( m_CountPerPage );	
	for( int i=0; i<m_CountPerPage; ++i )
	{
		sprintf_s( buf, 64, "ID_SAUCEITEM%d", i );
		m_vSlotBtnMaterials[ i ] = GetControl<CDnItemSlotButton>( buf );
	}

	// 재료슬롯 선택 Static.
	m_vStaticSelect.resize( m_CountPerPage );	
	for( int i=0; i<m_CountPerPage; ++i )
	{
		sprintf_s( buf, 64, "ID_SAUCESELECT%d", i );
		m_vStaticSelect[ i ] = GetControl<CEtUIStatic>( buf );
	}	


	//---  결과물슬롯들  ---//
	m_pSlotBtnSelectedResult = GetControl<CDnItemSlotButton>("ID_ITEM1"); // 선택한 결과물슬롯.
	m_pStaticLightResult = GetControl<CEtUIStatic>("ID_STATIC_LIGHT1"); // ID_STATIC_LIGHT_RESULT
	m_pStaticPageResult = GetControl<CEtUIStatic>("ID_TEXT_PAGE1"); // Page/MaxPage.
	
		
	// 슬롯들.	
	m_vSlotBtnMaterialsResult.resize( m_CountPerPage );	
	for( int i=0; i<m_CountPerPage; ++i )
	{
		sprintf_s( buf, 64, "ID_RESULTITEM%d", i );
		m_vSlotBtnMaterialsResult[ i ] = GetControl<CDnItemSlotButton>( buf );
	}

	// 슬롯 선택 Static.
	m_vStaticSelectResult.resize( m_CountPerPage );	
	for( int i=0; i<m_CountPerPage; ++i )
	{
		sprintf_s( buf, 64, "ID_RESULTSELECT%d", i );
		m_vStaticSelectResult[ i ] = GetControl<CEtUIStatic>( buf );
	}	

	// 결과슬롯에 기간제표시용 Static.
	m_vPeriodItems.resize( m_CountPerPage ); 
	for( int i=0; i<m_CountPerPage; ++i )
	{
		sprintf_s( buf, 64, "ID_STATIC_TIME%d", i );
		m_vPeriodItems[ i ] = GetControl<CEtUIStatic>( buf );
	}

	m_pBtnMaterial = GetControl<CEtUIButton>("ID_BT_LIST0"); // 재료.
	m_pBtnProduct = GetControl<CEtUIButton>("ID_BT_LIST1");    // 결과물.


	m_pStaticTitle = GetControl<CEtUIStatic>("ID_TEXT_TITLE"); // Title.
	m_pStaticHelp = GetControl<CEtUIStatic>("ID_TEXT_ASK0"); // Ask. 

	m_pBtnConfirm = GetControl<CEtUIButton>("ID_OK"); // 확인.

	m_pWheelRegionStuff = GetControl<CEtUIStatic>("ID_STATIC7");  // 재료슬롯 휠 영역.
	m_pWheelRegionResult = GetControl<CEtUIStatic>("ID_STATIC8"); // 결과물슬롯 휠 영역.
}


// Show()
void CDnChoiceCubeDlg::Show( bool bShow )
{
	if( m_bShow == bShow )
		return;

	CEtUIDialog::Show( bShow );

	// Open.
	if( bShow )
	{
		m_pBtnConfirm->Enable( false ); // 확인버튼 비활성화.
		OpenDlg();
	}

	// Close.
	else
	{
		// Dlg Close시에 정리.
		CloseDlg();		
	}
}


void CDnChoiceCubeDlg::Process( float fElapsedTime )
{
	CEtUIDialog::Process( fElapsedTime );
	
	// 선택된 재료템이 있는경우에 빛나는 애니메이션처리.
	if( m_pSlotBtnSelected->GetItem() || m_pSlotBtnSelectedResult->GetItem() )
	{		
		D3DXCOLOR color( m_pStaticLight->GetTextureColor() );

		if( m_fAlphaColor <= 0.3f )
			m_signLight = 1;
		if( m_fAlphaColor >= 1.0f )
			m_signLight = -1;

		m_fAlphaColor += fElapsedTime * m_signLight;
		color.a = m_fAlphaColor;

		// 재료슬롯.
		if( m_pSlotBtnSelected->GetItem() )
			m_pStaticLight->SetTextureColor( color );

		// 결과슬롯.
		if( m_pSlotBtnSelectedResult->GetItem() && m_pStaticLightResult->IsShow() )
			m_pStaticLightResult->SetTextureColor( color );
	}

}


bool CDnChoiceCubeDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsShow() )
	{
		return false;
	}

	switch( uMsg )
	{
	case WM_MOUSEWHEEL:
		{			
			if( IsMouseInDlg() )
			{
				POINT MousePoint;
				MousePoint.x = short( LOWORD( lParam ) );
				MousePoint.y = short( HIWORD( lParam ) );

				float fMouseX, fMouseY;
				ScreenToClient( hWnd, &MousePoint );
				PointToFloat( MousePoint, fMouseX, fMouseY );
				lParam = MAKELPARAM( MousePoint.x, MousePoint.y );

				UINT uLines;
				SystemParametersInfo( SPI_GETWHEELSCROLLLINES, 0, &uLines, 0 );
				int nScrollAmount = int( ( short )HIWORD( wParam ) ) / WHEEL_DELTA * uLines;

				// 재료슬롯영역.
				SUICoord coord;
				coord = m_pWheelRegionStuff->GetUICoord();
				if( coord.IsInside( fMouseX, fMouseY ) )				
				{
					if( nScrollAmount > 0 )
					{
						if( m_crrPage > 0 )
							ProcessCommand( EVENT_BUTTON_CLICKED, false, GetControl<CEtUIButton>("ID_BT_PRIOR0"), 0 );
					}
					else if( nScrollAmount < 0 )
					{
						if( m_crrPage < m_maxPage )
							ProcessCommand( EVENT_BUTTON_CLICKED, false, GetControl<CEtUIButton>("ID_BT_NEXT0"), 0 );
					}
				}

				// 결과물슬롯영역.
				else
				{
					coord = m_pWheelRegionResult->GetUICoord();
					if( coord.IsInside( fMouseX, fMouseY ) )					
					{
						if( nScrollAmount > 0 )
						{
							if( m_crrPageResult > 0 )
								ProcessCommand( EVENT_BUTTON_CLICKED, false, GetControl<CEtUIButton>("ID_BT_PRIOR1"), 0 );
						}
						else if( nScrollAmount < 0 )
						{
							if( m_crrPageResult < m_maxPageResult )
								ProcessCommand( EVENT_BUTTON_CLICKED, false, GetControl<CEtUIButton>("ID_BT_NEXT1"), 0 );
						}
					}
				}
				
				CEtUIDialogBase::StaticMsgProc( CDnMainFrame::GetInstance().GetHWnd(), WM_MOUSEMOVE, 0, (LPARAM)lParam );

				return true;
			}
		}
		break;
	}

	return CEtUIDialog::MsgProc( hWnd, uMsg, wParam, lParam );
}


// ProcessCommand()
void CDnChoiceCubeDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl * pControl, UINT uMsg )
{

	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{

		if( strstr( pControl->GetControlName(), "ID_SAUCEITEM" ) )
		{
			// 재료아이템선택.
			SelectionStuffItem( pControl->GetControlName() );			
		}

		if( strstr( pControl->GetControlName(), "ID_RESULTITEM" ) )
		{
			// 결과물아이템선택.
			SelectionResultItem( pControl->GetControlName() );
		}

		// 이전.
		else if( IsCmdControl("ID_BT_PRIOR0") )
		{
			--m_crrPage;
			if( m_crrPage < 0 )
				m_crrPage = m_maxPage;

			// 재료슬롯 갱신.
			RefreshStuffItemSlot();
		}

		// 다음.
		else if( IsCmdControl("ID_BT_NEXT0") )
		{
			++m_crrPage;
			if( m_crrPage > m_maxPage )
				m_crrPage = 0;

			// 재료슬롯 갱신.
			RefreshStuffItemSlot();
		}


		// 결과슬롯 이전.
		else if( IsCmdControl("ID_BT_PRIOR1") )
		{
			--m_crrPageResult;
			if( m_crrPageResult < 0 )
				m_crrPageResult = m_maxPageResult;
			
			// 결과물슬롯 갱신.
			RefreshResultItemSlot();
		}

		// 결과슬롯 다음.
		else if( IsCmdControl("ID_BT_NEXT1") )
		{
			++m_crrPageResult;
			if( m_crrPageResult > m_maxPageResult )
				m_crrPageResult = 0;

			// 결과물슬롯 갱신.
			RefreshResultItemSlot();
		}



		// 재료창열기.
		else if( IsCmdControl("ID_BT_LIST0") )
		{
			CDnChaosCubeStuffDlg * pStuffDlg = GetInterface().GetChaosCubeStuffDlg();
			if( pStuffDlg )
			{
				pStuffDlg->SetDlgMode( CDnChaosCubeDlg::EChildDlgMode::STUFF_CHOICE, m_pItemChaos->GetTypeParam(0), m_bSelfItemList );
				pStuffDlg->Show( true );
			}
		}

		// 결과창열기.
		else if( IsCmdControl("ID_BT_LIST1") )
		{
			CDnChaosCubeStuffDlg * pStuffDlg = GetInterface().GetChaosCubeStuffDlg();
			if( pStuffDlg )
			{
				pStuffDlg->SetDlgMode( CDnChaosCubeDlg::EChildDlgMode::PRODUCT_CHOICE, m_pItemChaos->GetTypeParam(0), m_bSelfItemList );
				pStuffDlg->Show( true );
			}
		}

		// 닫기.
		else if( IsCmdControl("ID_BT_CLOSE") || IsCmdControl("ID_CANCEL") )
			Show( false );

		// 믹스시작.
		else if( IsCmdControl("ID_OK") )
		{
			// 인벤토리슬롯이 남아있는지 확인.
			int needEmptyCount = (int)CGlobalWeightIntTable::GetInstance().GetValue(CGlobalWeightIntTable::ChaosCubeEmptySlotCount);

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
					GetInterface().MessageBox( 7040, MB_YESNO, 1, this ); // "선택한 재료가 사라지고 선택한 아이템이 획득됩니다. 정말로 초이스큐브를 작동하시겠습니까?"
				}
			}					
		}

	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}


void CDnChoiceCubeDlg::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nID == 1 )
	{
		if( nCommand == EVENT_BUTTON_CLICKED )
		{
			if( IsCmdControl( "ID_YES" ) )
			{
				m_pBtnConfirm->Enable( false );
				MixStart(); // 믹스시작.
			}
		}
	}

}


// Dlg Close시에 정리.
void CDnChoiceCubeDlg::CloseDlg()
{
	m_crrPage = 0;
	m_crrPageResult = 0;

	// 재료선택슬롯 초기화.
	m_pSlotBtnSelected->ResetSlot();

	// 재료슬롯 초기화.
	int size = (int)m_vSlotBtnMaterials.size();
	for( int i=0; i<size; ++i )
		m_vSlotBtnMaterials[ i ]->ResetSlot();

	// 인벤에서 검색한 재료아이템들.
	std::map< int, std::vector< CDnItem * > >::iterator it = m_mapStuffItems.begin();
	for( ; it != m_mapStuffItems.end(); ++it )
		(*it).second.clear();
	m_mapStuffItems.clear();	

	// Hide - 재료슬롯 선택 Static.
	size = (int)m_vStaticSelect.size();
	for( int i=0; i<size; ++i )
		m_vStaticSelect[ i ]->Show( false ); 
	

	// 결과슬롯 초기화.
	m_pSlotBtnSelectedResult->ResetSlot();
	size = (int)m_vSlotBtnMaterialsResult.size();
	for( int i=0; i<size; ++i )
		m_vSlotBtnMaterialsResult[ i ]->ResetSlot();

	// Hide - 결과물슬롯 선택 Static.
	size = (int)m_vStaticSelectResult.size();
	for( int i=0; i<size; ++i )
		m_vStaticSelectResult[ i ]->Show( false ); 
	

	// 결과물선택슬롯 빛남 숨김.
	m_pStaticLightResult->Show( false );

	// Close ChildDlg.
	//m_pChildDlg->Show( false );
	CDnChaosCubeStuffDlg * pDlg = GetInterface().GetChaosCubeStuffDlg();
	if( pDlg )
		pDlg->Show( false );
}


void CDnChoiceCubeDlg::OpenDlg()
{
	if( !m_pItemChaos )
		return;

	m_pStaticHelp->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7019 ) ); // "카오스큐브에 넣을 재료를 선택해 주세요."

	int mixNumber = m_pItemChaos->GetTypeParam( 0 );
	int mixType = m_pItemChaos->GetTypeParam( 1 );

	switch( mixType )
	{
	case 0 :
		m_bSelfItemList = true;
		m_pBtnMaterial->Enable( true );
		m_pBtnProduct->Enable( true );
		break;

	case 1:
		m_bSelfItemList = true;
		m_pBtnMaterial->Enable( true );
		m_pBtnProduct->Enable( false );
		break;

	case 2:
		m_bSelfItemList = false;
		m_pBtnMaterial->Enable( true );
		m_pBtnProduct->Enable( true );
		break;

	case 3:
		m_bSelfItemList = false;
		m_pBtnMaterial->Enable( true );
		m_pBtnProduct->Enable( false );
		break;
	}


	// 믹스박스번호에 해당하는 재료아이템들을 얻어낸다.	
	DNTableFileFormat * pTable = GetDNTable( CDnTableDB::TCHAOSCUBESTUFF );
	if( !pTable )
		return;

	m_vStuffItemIDs.clear();

	int size = pTable->GetItemCount();
	for( int i=0; i<size; ++i )
	{
		int id = pTable->GetItemID( i );
		DNTableCell * pCell = pTable->GetFieldFromLablePtr( id, "_ChaoscubeNum" );
		if( pCell )
		{	
			if( mixNumber == pCell->GetInteger() )
			{
				pCell = pTable->GetFieldFromLablePtr( id, "_ItemID" );

				if( pCell )
				{
					int itemID = pCell->GetInteger();
					m_vStuffItemIDs.push_back( itemID );

					pCell = pTable->GetFieldFromLablePtr( id, "_Count" );
					if( pCell )
						m_vNeedStuffCount.insert( std::map< int, int >::value_type( itemID, pCell->GetInteger() ) );
				}
			}
		}
	}


	// 결과물 슬롯들.
	m_vResultItems.clear();
	pTable = GetDNTable( CDnTableDB::TCHAOSCUBERESULT );
	if( !pTable )
		return;

	size = pTable->GetItemCount();
	for( int i=0; i<size; ++i )
	{
		int id = pTable->GetItemID( i );
		DNTableCell * pCell = pTable->GetFieldFromLablePtr( id, "_ChaoscubeNum" );
		if( pCell )
		{
			if( mixNumber == pCell->GetInteger() )
			{
				pCell = pTable->GetFieldFromLablePtr( id, "_ItemID" );
				if( pCell )
				{
					CDnItem * pNewItem = NULL;
					int itemID = pCell->GetInteger();

					// 개수.
					pCell = pTable->GetFieldFromLablePtr( id, "_Count" );
					if( pCell )
					{
						pNewItem = CDnItem::CreateItem( itemID, 0 );
						pNewItem->SetOverlapCount( pCell->GetInteger() );

						// 기간제.
						pCell = pTable->GetFieldFromLablePtr( id, "_Period" );
						if( pCell )
						{
							SResultItem aNewItem;
							aNewItem.pItem = pNewItem;
							aNewItem.nPeriod = pCell->GetInteger();		

							// 골드.
							pCell = pTable->GetFieldFromLablePtr( id, "_Gold" );
							if( pCell )
								aNewItem.nGold = (INT64)( pCell->GetInteger() );

							m_vResultItems.push_back( aNewItem );
						}						

					}
				}
			}
		}
	}


	// 아이템목록갱신.
	UpdateItemList(); 

}


// 아이템목록갱신.
void CDnChoiceCubeDlg::UpdateItemList()
{	

	// 재료슬롯 -------------//
	if( !m_vStuffItemIDs.empty() )
	{			
		m_pStaticHelp->SetTextColor( textcolor::WHITE );
		m_pStaticHelp->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7019 ) ); // "카오스큐브에 넣을 재료를 선택해 주세요."

		m_pItemChaos = GetItemTask().GetCashInventory().FindItemFromSerialID( m_nChaosSerial );

		// 아이템없음.
		if( m_pItemChaos == NULL )
		{
			Show( false );
			return;
		}


		m_mapStuffItems.clear();

		CDnItemTask & pItemTask = GetItemTask();

		const int size = 3;
		CDnInventory *arrInven[ size ] = { &pItemTask.GetCharInventory(), &pItemTask.GetCashInventory(), &pItemTask.GetPetInventory() };
		CDnInventory * pInven;

		int stuffItemCnt = 0;
		int stuffItemCount = (int)m_vStuffItemIDs.size();
		for( int i=0; i<stuffItemCount; ++i )
		{		
			// 인벤토리에서 재료검색.
			std::vector< CDnItem * > vResult;
			for( int k=0; k<size; ++k )
			{
				pInven = arrInven[ k ];
				int tot = pInven->ScanItemFromID( m_vStuffItemIDs[ i ], &vResult );

				// 찾았음.
				if( !vResult.empty() )
				{
					++stuffItemCnt;
					m_mapStuffItems.insert( std::map< int, std::vector< CDnItem * > >::value_type( m_vStuffItemIDs[ i ], vResult ) ); // map 에 재료템들 저장.
					break;
				}
			}
		}

		if( stuffItemCnt >= m_CountPerPage )
			--stuffItemCnt;
		m_maxPage = stuffItemCnt / m_CountPerPage;
	}

	// 재료슬롯 갱신.
	RefreshStuffItemSlot();



	// 결과물슬롯 -----------//
	//m_crrPageResult = 0;
	if( m_vResultItems.empty() )
	{
		m_crrPageResult = m_maxPageResult = 0;		
	}
	else
	{
		int size = (int)m_vResultItems.size();
		if( size >= m_CountPerPage )
			--size;
		m_maxPageResult = size / m_CountPerPage;
	}
	
	// 결과물슬롯 갱신.
	RefreshResultItemSlot();

}


// 재료슬롯에 재료등록.
void CDnChoiceCubeDlg::RefreshStuffItemSlot()
{
	// 재료선택표시 숨김.
	for( int i=0; i<m_CountPerPage; ++i )
		m_vStaticSelect[ i ]->Show( false );

	// 재료슬롯 초기화.
	int size = (int)m_vSlotBtnMaterials.size();
	for( int i=0; i<size; ++i )
	{
		m_vSlotBtnMaterials[ i ]->ResetSlot();
		m_vSlotBtnMaterials[ i ]->SetSlotType( ST_ITEM_NONE );
	}
	
	if( m_mapStuffItems.empty() )
	{
		m_pStaticHelp->SetTextColor( textcolor::RED );
		m_pStaticHelp->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7033 ) ); // "본 카오스 큐브에 넣을 수 있는 재료를 가지고 있지 않습니다."
		return;
	}

	// 현재페이지에 해당하는 재료들을 슬롯에 등록한다.	
	int i = 0;
	size = m_crrPage * m_CountPerPage;
	std::map< int, std::vector< CDnItem * > >::iterator it = m_mapStuffItems.begin();
	while( i < size )
	{
		++it;
		++i;

		if( it == m_mapStuffItems.end() )
		{			
			return;
		}		
	}

	for( i=0; i<m_CountPerPage; ++i )
	{
		std::vector< CDnItem * > & vItems = (*it).second;

		// 개수.
		int tot = 0;
		for( int k=0; k<(int)vItems.size(); ++k )
			tot += vItems[ k ]->GetOverlapCount();

		// 슬롯에 등록.
		m_vSlotBtnMaterials[ i ]->SetItem( vItems[0], tot );

		// 재료개수가 모자란 경우 개수폰트색상을 RED로 변경.		
		std::map< int, int >::iterator itNC = m_vNeedStuffCount.find( vItems[0]->GetClassID() );
		if( itNC != m_vNeedStuffCount.end() )
		{
			if( tot < (*itNC).second )
			{
				m_vSlotBtnMaterials[ i ]->SetSlotType( ST_SET_FONTCOLOR );
				m_vSlotBtnMaterials[ i ]->SetFontColor( D3DCOLOR_RGBA(255,0,0,255) );
			}
		}	

		++it;
		if( it == m_mapStuffItems.end() )
			break;
	}

	// 페이지표시.
	wchar_t str[128];
	swprintf_s( str, 10, L"%d / %d", m_crrPage+1, m_maxPage+1 );
	m_pStaticPage->SetText( str );

}

// 재료아이템선택.
void CDnChoiceCubeDlg::SelectionStuffItem( const char * strCtlName )
{
	// 선택표시.
	std::string str( strCtlName );
	std::string strNumber = str.substr( strlen("ID_SAUCEITEM"), 1 );
	int idx = atoi( strNumber.c_str() );
	if( idx >= 0 && idx < m_CountPerPage )
	{
		// 선택부분 이외는 선택표시 숨김.
		for( int i=0; i<m_CountPerPage; ++i )
			m_vStaticSelect[ i ]->Show( false );

		MIInventoryItem * pSlotItem = m_vSlotBtnMaterials[ idx ]->GetItem();
		if( !pSlotItem )
			return;

		CDnItem * pSelectedItem = static_cast< CDnItem * >( pSlotItem );
		if( pSelectedItem )
		{
			m_SelectedSlotIndex = -1;			

			// 필요한 재료개수 확인.
			int totItemCount = m_vSlotBtnMaterials[ idx ]->GetRenderCount(); // 인벤의 개수.

			std::map< int, int >::iterator it = m_vNeedStuffCount.find( pSelectedItem->GetClassID() );
			if( it != m_vNeedStuffCount.end() )
			{
				int needCount = (*it).second; // 필요 재료개수.

				// 재료가 부족함.
				if( totItemCount < needCount )
				{
					m_pBtnConfirm->Enable( false ); // 확인버튼 활성화.
					GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7022) ); // "최소 수량이 부족한 재료는 카오스큐브에 넣을 수 없습니다."
				}

				// OK.
				else
				{
					m_vStaticSelect[ idx ]->Show( true );	

					// 선택한 아이템을 선택재료슬롯에 등록.	
					m_pSlotBtnSelected->SetItem( pSelectedItem, needCount );		

					// 빛남초기화.
					m_fAlphaColor = 1.0f;

					// 결과아이템선택여부 - 
					// 7034 : "획득하고 싶은 아이템을 결과물 목록에서 선택해 주세요."
					// 7035 : "결과물 아이템을 확인하시고 확인 버튼을 클릭해 주세요."
					m_pStaticHelp->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 
						m_pSlotBtnSelectedResult->GetItem() == NULL ? 7034 : 7035 ) );					

					// 선택한 재료슬롯 인덱스.
					m_SelectedSlotIndex = idx;

					if( m_pSlotBtnSelectedResult->GetItem() != NULL )
					{
						// 재료아이템과 결과물아이템이 같은지 비교.
						if( CompareStuffnResult() )
							m_pBtnConfirm->Enable( true ); // 확인버튼 활성화.						
					}
					
				}
			}			
		}		
	}
}

// 결과물슬롯에 결과물등록.
void CDnChoiceCubeDlg::RefreshResultItemSlot()
{
	// 결과슬롯들 -------------//


	// 선택표시 숨김.
	for( int i=0; i<m_CountPerPage; ++i )
		m_vStaticSelectResult[ i ]->Show( false );

	// 슬롯 초기화.
	int size = (int)m_vSlotBtnMaterialsResult.size();
	for( int i=0; i<size; ++i )
	{
		m_vSlotBtnMaterialsResult[ i ]->ResetSlot();
		m_vSlotBtnMaterialsResult[ i ]->SetSlotType( ST_ITEM_NONE );
	}

	if( !m_vResultItems.empty() )
	{
		CDnItem * pResultItem = NULL;
		int slotIndex = 0;
		int itemSize = (int)m_vResultItems.size();

		int size = (int)m_vResultItems.size();
		int nBegin = m_crrPageResult * m_CountPerPage;
		for( int i=nBegin; i<nBegin+m_CountPerPage; ++i, ++slotIndex )
		{
			if( i < itemSize )
			{
				SResultItem & rResultItem = m_vResultItems[ i ];				
				m_vSlotBtnMaterialsResult[ slotIndex ]->SetItem( rResultItem.pItem, rResultItem.pItem->GetOverlapCount() );

				// 기간제아이템의 경우 - 
				m_vPeriodItems[ slotIndex ]->Show( ( rResultItem.nPeriod > 0 ? true : false ) ); //아이콘표시.
				if( rResultItem.nPeriod )
				{
					std::wstring strTooltip = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4706 ), rResultItem.nPeriod );
					m_vSlotBtnMaterialsResult[ slotIndex ]->SetTooltipText( strTooltip.c_str() );
				}
				else
				{
					m_vSlotBtnMaterialsResult[ slotIndex ]->ClearTooltipText();
				}

				// 골드아이템의 경우 툴팁에 골드 출력.
				if( rResultItem.pItem->GetClassID() == 1073750029 )
				{
					// 골드획득 - 0인 단위는 출력하지 않음. (ex> 1골드0실버1쿠퍼 => 1골드1쿠퍼 ).
					if( rResultItem.nGold > 0 )
					{
						std::wstring str;
						GoldToString( str, rResultItem.nGold,
							GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1766 ),
							GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1781 ),		
							GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1783 ) );
						m_vSlotBtnMaterialsResult[ slotIndex ]->SetTooltipText( str.c_str() );	
					}
					else
					{
						m_vSlotBtnMaterialsResult[ slotIndex ]->ClearTooltipText();
					}
				}
			}
		}

	}

	// 페이지표시.	
	wchar_t str[128] = {0,};
	swprintf_s( str, 10, L"%d / %d", m_crrPageResult+1, m_maxPageResult+1 );
	m_pStaticPageResult->SetText( str );	

}


// 결과물아이템선택.
void CDnChoiceCubeDlg::SelectionResultItem( const char * strCtlName )
{
	// 선택표시.
	std::string str( strCtlName );
	std::string strNumber = str.substr( strlen("ID_RESULTITEM"), 1 );
	int idx = atoi( strNumber.c_str() );
	if( idx >= 0 && idx < m_CountPerPage )
	{
		// 선택부분 이외는 선택표시 숨김.
		for( int i=0; i<m_CountPerPage; ++i )
			m_vStaticSelectResult[ i ]->Show( false );

		MIInventoryItem * pSlotItem = m_vSlotBtnMaterialsResult[ idx ]->GetItem();
		if( !pSlotItem )
			return;

		CDnItem * pSelectedItem = static_cast< CDnItem * >( pSlotItem );
		if( pSelectedItem )
		{
			m_SelectedSlotIndexResult = -1;					
			
			m_vStaticSelectResult[ idx ]->Show( true );	

			// 선택한 아이템을 선택재료슬롯에 등록.	
			m_pSlotBtnSelectedResult->SetItem( pSelectedItem, pSelectedItem->GetOverlapCount() );		

			// 빛남초기화.
			m_fAlphaColorResult = 1.0f;

			//m_pStaticHelp->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7020 ) ); // "선택한 재료를 확인하고 확인 버튼을 클릭해주세요. 정해진 개수의 재료가 새로운 아이템으로 변화됩니다."

			// 선택한 재료슬롯 인덱스.
			m_SelectedSlotIndexResult = idx;

			if( m_pSlotBtnSelected->GetItem() != NULL )
			{
				m_pStaticLightResult->Show( true );

				// 재료아이템과 결과물아이템이 같은지 비교.
				if( CompareStuffnResult() )
					m_pBtnConfirm->Enable( true ); // 확인버튼 활성화.				
			}
		}
	}

}


// 믹스시작.
void CDnChoiceCubeDlg::MixStart()
{
	CDnItem * pSelectedItem = static_cast< CDnItem * >( m_pSlotBtnSelected->GetItem() );
	CDnItem * pSelectedItemResult = static_cast< CDnItem * >( m_pSlotBtnSelectedResult->GetItem() );
	if( !pSelectedItem || !pSelectedItemResult )
		return;

	CSChaosCubeRequest packet;
	memset( packet.ChaosItem, 0, sizeof(TChaosItem) * MAX_CHAOSCUBE_STUFF );
	
	if( m_pItemChaos->IsCashItem() )
		packet.cCubeInvenType = ITEMPOSITION_CASHINVEN;
	else
		packet.cCubeInvenType = ITEMPOSITION_INVEN;	
	packet.sCubeInvenIndex = m_pItemChaos->GetSlotIndex();
	packet.biCubeInvenSerial = m_pItemChaos->GetSerialID();
	packet.nChoiceItemID = pSelectedItemResult->GetClassID();

	// 필요한 재료 개수.
	std::map< int, int >::iterator it = m_vNeedStuffCount.find( pSelectedItem->GetClassID() );
	if( it != m_vNeedStuffCount.end() )
	{
		int needCount = (*it).second; // 필요 재료개수.
		packet.nCount = 0;

		std::map< int, std::vector< CDnItem * > >::iterator mapIt = m_mapStuffItems.find( pSelectedItem->GetClassID() );
		if( mapIt != m_mapStuffItems.end() )
		{		
			// 인벤타입 - 탈것.
			CDnItem * pItem = GetItemTask().FindItem( pSelectedItem->GetClassID(), ST_INVENTORY_VEHICLE ); 
			if( pItem )
				packet.cInvenType = ITEMPOSITION_VEHICLE;
			else
			{
				// 인벤 - 캐시.
				if( pSelectedItem->IsCashItem() )
					packet.cInvenType = ITEMPOSITION_CASHINVEN; 
				else
				{
					// 인벤 - 일반.
					pItem = GetItemTask().FindItem( pSelectedItem->GetClassID(), ST_INVENTORY ); 
					if( pItem )
						packet.cInvenType = ITEMPOSITION_INVEN;
					else
					{
						// 인벤타입에러.
						ASSERT( 0 && "Error - Inven Type." );
						return;
					}
				}
			}		

			std::vector< CDnItem * > & rVec = (*mapIt).second;
			int size = (int)rVec.size();
			for( int i=0; i<size; ++i )
			{
				++packet.nCount;

				CDnItem * stuffItem = rVec[ i ];				

				packet.ChaosItem[ i ].biInvenSerial = stuffItem->GetSerialID();
				packet.ChaosItem[ i ].sInvenIndex = stuffItem->GetSlotIndex();

				int count = stuffItem->GetOverlapCount();

				needCount -= count;

				// 더필요하다 다음아이템도 계속진행.
				if( needCount > 0 )
				{
					packet.ChaosItem[ i ].wCount = count;
				}

				else if( needCount == 0 )
				{
					packet.ChaosItem[ i ].wCount = count;
					break;	
				}

				else if( needCount < 0 )
				{
					packet.ChaosItem[ i ].wCount = needCount + count;
					break;
				}
			}// for.

			GetItemTask().RequestChaosCube( packet );
		}
	}

}	


// 카오스아이템.
void CDnChoiceCubeDlg::SetChaoseItem( class CDnItem * pItem )
{
	m_pItemChaos = pItem;

	if( m_pItemChaos == NULL )
		return;

	m_nChaosSerial = m_pItemChaos->GetSerialID();

	memcpy_s( m_pItemChaos, sizeof(CDnItem), pItem, sizeof(CDnItem) );		
	m_nChaosSerial = m_pItemChaos->GetSerialID();


	//
	//m_pStaticTitle = GetControl<CEtUIStatic>("ID_TEXT_TITLE"); // Title.
	m_pStaticTitle->SetText( m_pItemChaos->GetName() );

}


// 믹스완료.
void CDnChoiceCubeDlg::MixComplete()
{
	m_pStaticHelp->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7019 ) ); // "카오스큐브에 넣을 재료를 선택해 주세요."
}


// 결과아이템 수령완료.
void CDnChoiceCubeDlg::RecvResultItemComplet()
{
	// 재료.
	if( m_SelectedSlotIndex != -1 )
	{
		char buf[10];
		std::string str( "ID_SAUCEITEM" );
		str.append( itoa(m_SelectedSlotIndex,buf,10) );

		SelectionStuffItem( str.c_str() );
	}

	// 결과.
	if( m_SelectedSlotIndexResult != -1 )
	{
		char buf[10];
		std::string str( "ID_RESULTITEM" );
		str.append( itoa(m_SelectedSlotIndexResult,buf,10) );

		SelectionResultItem( str.c_str() );
	}

}

// 믹스시작 응답.
void CDnChoiceCubeDlg::RecvMixStart( bool bEnableBtn )
{
	m_pBtnConfirm->Enable( bEnableBtn );
}


int CDnChoiceCubeDlg::GetResultItemID()
{	
	CDnItem * pR = static_cast< CDnItem * >( m_pSlotBtnSelectedResult->GetItem() );
	if( pR )
		return pR->GetClassID();

	return 0;
}


// 재료아이템과 결과물아이템이 같은지 비교.
bool CDnChoiceCubeDlg::CompareStuffnResult()
{
	int pStuffID = m_pSlotBtnSelected->GetItem()->GetClassID();
	int pResultID = m_pSlotBtnSelectedResult->GetItem()->GetClassID();
	
	bool bSame = ( pStuffID == pResultID );
	int typeParam2 = m_pItemChaos->GetTypeParam( 1 );

	bool bRes = true;

	// 같은아이템.
	if( bSame )
	{
		bRes = (typeParam2 == 0 || typeParam2 == 1) ? true : false; // 같은거 가능일때 true, 아니면 false.
	}

	// 메세지처리 & 결과물슬롯제거.
	if( !bRes )
	{
		m_pSlotBtnSelectedResult->ResetSlot();
		m_vStaticSelectResult[ m_SelectedSlotIndexResult ]->Show( false ); // 선택슬롯표시 숨김.
		m_pStaticLightResult->Show( false );
		GetInterface().MessageBox( 7041, MB_OK ); // "본 큐브에서는 재료와 결과물을 동일하게 선택할 수 없습니다."
	}
	else
	{
		m_pStaticLightResult->Show( true );
	}

	return bRes;
}


// 아이템의 기간제여부( 반환값 : 기간 ).
int CDnChoiceCubeDlg::GetPeriodItemByID( int nItemID )
{
	int size = (int)m_vResultItems.size();
	for( int i=0; i<size; ++i )
	{
		SResultItem & rItem = m_vResultItems[ i ];
		if( rItem.pItem->GetClassID() == nItemID )
			return rItem.nPeriod;
	}

	return 0;
}


#endif
