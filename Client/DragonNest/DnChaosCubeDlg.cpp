#include "StdAfx.h"

#ifdef PRE_ADD_CHAOSCUBE

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
#include "DnChaosCubeResultDlg.h"
#include "DnChaosCubeProgressDlg.h"
#include "DnChaosCubeStuffDlg.h"
#include "DnChaosCubeDlg.h"


// Construct.
CDnChaosCubeDlg::CDnChaosCubeDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback, bool bAutoCursor )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback, bAutoCursor ),
//m_pChaosCubeProgressDlg(NULL),
//m_pChaosCubeResultDlg(NULL),
//m_pChildDlg(NULL),
m_pItemChaos(NULL), // 카오스아이템.
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
	m_pBtnMaterial = NULL; // 재료.
	m_pBtnProduct = NULL;  // 결과물.
	m_pBtnConfirm = NULL;  // 확인.
}


// ReleaseDlg()
void CDnChaosCubeDlg::ReleaseDlg()
{
	m_vSlotBtnMaterials.clear(); // 재료슬롯들.
	m_vStaticSelect.clear(); // 재료슬롯 선택 Static.
	m_vNeedStuffCount.clear(); // 믹스에 필요한 재료개수.
	m_vStuffItemIDs.clear(); // 재료아이템.
	
//	SAFE_DELETE( m_pChildDlg );
//	SAFE_DELETE( m_pChaosCubeProgressDlg );
//	SAFE_DELETE( m_pChaosCubeResultDlg );
}


// Initialize()
void CDnChaosCubeDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName("MixBoxDlg.ui").c_str(), bShow );
}


// InitialUpdate()
void CDnChaosCubeDlg::InitialUpdate()
{
	//// Dialog - 재료Dlg or 결과물.
	//m_pChildDlg = new CDnChaosCubeStuffDlg( UI_TYPE_CHILD_MODAL, this );  
	//m_pChildDlg->Initialize( false );

	//// Dialog - 프로그레스창.	
	//m_pChaosCubeProgressDlg = new CDnChaosCubeProgressDlg( UI_TYPE_CHILD_MODAL, this );
	//m_pChaosCubeProgressDlg->Initialize( false );

	//// Dialog - 획득결과물.
	//m_pChaosCubeResultDlg = new CDnChaosCubeResultDlg( UI_TYPE_CHILD_MODAL, this );
	//m_pChaosCubeResultDlg->Initialize( false );

	m_pSlotBtnSelected = GetControl<CDnItemSlotButton>("ID_ITEM0"); // 선택한재료슬롯.
	m_pStaticLight = GetControl<CEtUIStatic>("ID_STATIC_LIGHT");    // 반짝임.

	m_pStaticPage = GetControl<CEtUIStatic>("ID_TEXT_PAGE"); // Page/MaxPage.

	// 재료슬롯들.
	char buf[64] = {0,};
	m_vSlotBtnMaterials.resize( m_CountPerPage );	
	for( int i=0; i<m_CountPerPage; ++i )
	{
		sprintf_s( buf, 64, "ID_ITEM%d", i+1 );
		m_vSlotBtnMaterials[ i ] = GetControl<CDnItemSlotButton>( buf );
	}

	// 재료슬롯 선택 Static.
	m_vStaticSelect.resize( m_CountPerPage );	
	for( int i=0; i<m_CountPerPage; ++i )
	{
		sprintf_s( buf, 64, "ID_SELECT%d", i+1 );
		m_vStaticSelect[ i ] = GetControl<CEtUIStatic>( buf );
	}	
	
	m_pBtnMaterial = GetControl<CEtUIButton>("ID_BT_ITEMLIST"); // 재료.
	m_pBtnProduct = GetControl<CEtUIButton>("ID_BT_RESULT");    // 결과물.


	m_pStaticTitle = GetControl<CEtUIStatic>("ID_TEXT_TITLE"); // Title.
	m_pStaticHelp = GetControl<CEtUIStatic>("ID_TEXT_ASK"); // Ask. 

	m_pBtnConfirm = GetControl<CEtUIButton>("ID_BT_OK"); // 확인.
}


// Show()
void CDnChaosCubeDlg::Show( bool bShow )
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


void CDnChaosCubeDlg::Process( float fElapsedTime )
{
	CEtUIDialog::Process( fElapsedTime );

	// 선택된 재료템이 있는경우에 빛나는 애니메이션처리.
	if( m_pSlotBtnSelected->GetItem() )
	{		
		D3DXCOLOR color( m_pStaticLight->GetTextureColor() );
		
		if( m_fAlphaColor <= 0.3f )
			m_signLight = 1;
		if( m_fAlphaColor >= 1.0f )
			m_signLight = -1;

		m_fAlphaColor += fElapsedTime * m_signLight;
		color.a = m_fAlphaColor;
		
		m_pStaticLight->SetTextureColor( color );
	}

}


bool CDnChaosCubeDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsShow() )
	{
		return false;
	}

	switch( uMsg )
	{
	case WM_MOUSEWHEEL:
		{
			POINT MousePoint;
			MousePoint.x = short( LOWORD( lParam ) );
			MousePoint.y = short( HIWORD( lParam ) );

			ScreenToClient( hWnd, &MousePoint );
			lParam = MAKELPARAM( MousePoint.x, MousePoint.y );

			if( IsMouseInDlg() )
			{
				UINT uLines;
				SystemParametersInfo( SPI_GETWHEELSCROLLLINES, 0, &uLines, 0 );
				int nScrollAmount = int( ( short )HIWORD( wParam ) ) / WHEEL_DELTA * uLines;
				if( nScrollAmount > 0 )
				{
					if( m_crrPage > 0 )
						ProcessCommand( EVENT_BUTTON_CLICKED, false, GetControl<CEtUIButton>("ID_BT_PRIOR"), 0 );
				}
				else if( nScrollAmount < 0 )
				{
					if( m_crrPage < m_maxPage )
						ProcessCommand( EVENT_BUTTON_CLICKED, false, GetControl<CEtUIButton>("ID_BT_NEXT"), 0 );
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
void CDnChaosCubeDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl * pControl, UINT uMsg )
{

	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
			
		if( strstr( pControl->GetControlName(), "ID_ITEM" ) )
		{
			// 재료아이템선택.
			SelectionStuffItem( pControl->GetControlName() );			
		}

		// 이전.
		else if( IsCmdControl("ID_BT_PRIOR") )
		{
			--m_crrPage;
			if( m_crrPage < 0 )
				m_crrPage = m_maxPage;

			// 재료슬롯 갱신.
			RefreshStuffItemSlot();
		}

		// 다음.
		else if( IsCmdControl("ID_BT_NEXT") )
		{
			++m_crrPage;
			if( m_crrPage > m_maxPage )
				m_crrPage = 0;

			// 재료슬롯 갱신.
			RefreshStuffItemSlot();
		}

		// 재료창열기.
		else if( IsCmdControl("ID_BT_ITEMLIST") )
		{
			//if( m_pSlotBtnSelected->GetItem() )
			//{
			//	m_pChildDlg->SetDlgMode( EChildDlgMode::STUFF, m_pSlotBtnSelected->GetItem()->GetClassID(), m_pItemChaos->GetTypeParam(0), m_bSelfItemList );
			//	m_pChildDlg->Show( true );			
			//}
//			m_pChildDlg->SetDlgMode( EChildDlgMode::STUFF, m_pItemChaos->GetTypeParam(0), m_bSelfItemList );
//			m_pChildDlg->Show( true );			
			GetInterface().GetChaosCubeStuffDlg()->SetDlgMode( EChildDlgMode::STUFF, m_pItemChaos->GetTypeParam(0), m_bSelfItemList );
			GetInterface().GetChaosCubeStuffDlg()->Show( true );
		}
	
		// 결과창열기.
		else if( IsCmdControl("ID_BT_RESULT") )
		{
			//if( m_pSlotBtnSelected->GetItem() )
			//{
			//	m_pChildDlg->SetDlgMode( EChildDlgMode::PRODUCT, m_pSlotBtnSelected->GetItem()->GetClassID(), m_pItemChaos->GetTypeParam(0), m_bSelfItemList );
			//	m_pChildDlg->Show( true );			
			//}
			//m_pChildDlg->SetDlgMode( EChildDlgMode::PRODUCT, m_pItemChaos->GetTypeParam(0), m_bSelfItemList );
			//m_pChildDlg->Show( true );
			GetInterface().GetChaosCubeStuffDlg()->SetDlgMode( EChildDlgMode::PRODUCT, m_pItemChaos->GetTypeParam(0), m_bSelfItemList );
			GetInterface().GetChaosCubeStuffDlg()->Show( true );
		}

		// 닫기.
		else if( IsCmdControl("ID_BT_CLOSE") || IsCmdControl("ID_BT_CANCEL") )
			Show( false );

		// 믹스시작.
		else if( IsCmdControl("ID_BT_OK") )
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
					GetInterface().MessageBox( 7032, MB_YESNO, 1, this ); // "선택한 재료가 사라지고 새로운 아이템이 획득됩니다. 정말로 카오스큐브를 작동하시겠습니까?"			
				}
			}					
		}
		
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}


void CDnChaosCubeDlg::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg )
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
void CDnChaosCubeDlg::CloseDlg()
{
	m_crrPage = 0;

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
	{
		m_vStaticSelect[ i ]->Show( false ); 
	}

	// Close ChildDlg.
	//m_pChildDlg->Show( false );
	CDnChaosCubeStuffDlg * pDlg = GetInterface().GetChaosCubeStuffDlg();
	if( pDlg )
		pDlg->Show( false );
}


void CDnChaosCubeDlg::OpenDlg()
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
	
	// 아이템목록갱신.
	UpdateItemList(); 
	
}


// 아이템목록갱신.
void CDnChaosCubeDlg::UpdateItemList()
{	
	if( !m_vStuffItemIDs.empty() )
	{			
		m_pStaticHelp->SetTextColor( textcolor::WHITE );
		m_pStaticHelp->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7019 ) ); // "카오스큐브에 넣을 재료를 선택해 주세요."

		m_pItemChaos = GetItemTask().GetCashInventory().FindItemFromSerialID( m_nChaosSerial );

		// 아이템없음.
		if( m_pItemChaos == NULL )
		{
			m_pItemChaos = GetItemTask().GetCharInventory().FindItemFromSerialID( m_nChaosSerial );
			if( m_pItemChaos == NULL )
			{
				Show( false );
				return;
			}
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
}


// 재료슬롯에 재료등록.
void CDnChaosCubeDlg::RefreshStuffItemSlot()
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
	wchar_t str[10];
	swprintf_s( str, 10, L"%d / %d", m_crrPage+1, m_maxPage+1 );
	m_pStaticPage->SetText( str );
}


// 재료아이템선택.
void CDnChaosCubeDlg::SelectionStuffItem( const char * strCtlName )
{
	// 선택표시.
	std::string str( strCtlName );
	std::string strNumber = str.substr( strlen("ID_ITEM"), 1 );
	int idx = atoi( strNumber.c_str() );
	if( idx > 0 && idx <= m_CountPerPage )
	{
		// 선택부분 이외는 선택표시 숨김.
		for( int i=0; i<m_CountPerPage; ++i )
			m_vStaticSelect[ i ]->Show( false );
	
		// 선택부분의 재료아이템이 존재하는 경우에 선택표시 출력.
		--idx;

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

					m_pStaticHelp->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7020 ) ); // "선택한 재료를 확인하고 확인 버튼을 클릭해주세요. 정해진 개수의 재료가 새로운 아이템으로 변화됩니다."

					// 선택한 재료슬롯 인덱스.
					m_SelectedSlotIndex = idx;

					m_pBtnConfirm->Enable( true ); // 확인버튼 활성화.
				}
			}			
		}		
	}
}


// 믹스시작.
void CDnChaosCubeDlg::MixStart()
{
	CDnItem * pSelectedItem = static_cast< CDnItem * >( m_pSlotBtnSelected->GetItem() );
	if( !pSelectedItem )
		return;

	CSChaosCubeRequest packet;
	memset( packet.ChaosItem, 0, sizeof(TChaosItem) * MAX_CHAOSCUBE_STUFF );

	if( m_pItemChaos->IsCashItem() )
		packet.cCubeInvenType = ITEMPOSITION_CASHINVEN;
	else
		packet.cCubeInvenType = ITEMPOSITION_INVEN;	
	packet.sCubeInvenIndex = m_pItemChaos->GetSlotIndex();
	packet.biCubeInvenSerial = m_pItemChaos->GetSerialID();

	
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


// Open ProgressDlg.
//void CDnChaosCubeDlg::OpenProgressDlg( bool bShow, SCChaosCubeRequest * pData )
//{
//	m_pChaosCubeProgressDlg->SetRequestData( pData );
//	m_pChaosCubeProgressDlg->Show( bShow );
//}

//// Open ResultDlg;
//void CDnChaosCubeDlg::OpenResultDlg( bool bShow, int nItemID, int nCount )
//{
//	m_pChaosCubeResultDlg->SetResultItem( nItemID, nCount );
//	m_pChaosCubeResultDlg->Show( bShow );
//}


// 카오스아이템.
void CDnChaosCubeDlg::SetChaoseItem( class CDnItem * pItem )
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
void CDnChaosCubeDlg::MixComplete()
{
	m_pStaticHelp->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7019 ) ); // "카오스큐브에 넣을 재료를 선택해 주세요."
}


// 결과아이템 수령완료.
void CDnChaosCubeDlg::RecvResultItemComplet()
{
	if( m_SelectedSlotIndex != -1 )
	{
		char buf[10];
		std::string str( "ID_ITEM" );
		str.append( itoa(m_SelectedSlotIndex+1,buf,10) );

		SelectionStuffItem( str.c_str() );
	}
	
}

// 믹스시작 응답.
void CDnChaosCubeDlg::RecvMixStart( bool bEnableBtn )
{
	m_pBtnConfirm->Enable( bEnableBtn );
}


#endif
