#include "StdAfx.h"


#ifdef PRE_ADD_CHAOSCUBE

#include "DnTableDB.h"
#include "DNTableFile.h"
#include "DnMainFrame.h"
#include "DnItem.h"
#include "DnItemTask.h"
#include "DnChaosCubeDlg.h"
#include "DnChaosCubeStuffDlg.h"

// Construct.
CDnChaosCubeStuffDlg::CDnChaosCubeStuffDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback, bool bAutoCursor )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback, bAutoCursor ),
m_DlgMode( CDnChaosCubeDlg::EChildDlgMode::NONE ),
m_CountPerPage(5), // 페이지당 슬롯개수.
m_crrPage(0), // 현재페이지.
m_maxPage(0),  // 전체페이지.
m_nStuffItemID(-1), // 재료아이템 아이디.
m_nChaosNum(0)
{
	m_pStaticTitle  = NULL; // 타이틀바.
	m_pStaticHelp = NULL;  // 도움말.
	m_pStaticPage = NULL;  // Page/MaxPage.

	m_pBtnPrev = NULL;     // 이전.
	m_pBtnNext = NULL;     // 다음.
}


// ReleaseDlg()
void CDnChaosCubeStuffDlg::ReleaseDlg()
{
	m_mapGold.clear(); // 결과 골드량.
	m_vItems.clear(); // 아이템목록.
	m_vSlotBtns.clear();   // 아이템슬롯들.
	m_vSlotStatics.clear();//아이템슬롯 Static.
}


// Initialize()
void CDnChaosCubeStuffDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName("MixBoxListDlg.ui").c_str(), bShow );
}


// InitialUpdate()
void CDnChaosCubeStuffDlg::InitialUpdate()
{
	m_pStaticTitle = GetControl<CEtUIStatic>("ID_TEXT_TITLE");
	m_pStaticHelp  = GetControl<CEtUIStatic>("ID_TEXT_ASK");
	m_pStaticPage  = GetControl<CEtUIStatic>("ID_TEXT_PAGE");

	// 재료슬롯들.
	char buf[64] = {0,};
	m_vSlotBtns.resize( m_CountPerPage );
	m_vSlotStatics.resize( m_CountPerPage );
	for( int i=0; i<m_CountPerPage; ++i )
	{
		sprintf_s( buf, 64, "ID_ITEM%d", i );
		m_vSlotBtns[ i ] = GetControl<CDnItemSlotButton>( buf );

		sprintf_s( buf, 64, "ID_TEXT_NAME%d", i );
		m_vSlotStatics[ i ] = GetControl<CEtUIStatic>( buf );
	}

}


// Show()
void CDnChaosCubeStuffDlg::Show( bool bShow )
{
	if( m_bShow == bShow )
		return;

	CEtUIDialog::Show( bShow );

	// Open.
	if( bShow )
	{
		// 재료창.
		if( m_DlgMode == CDnChaosCubeDlg::EChildDlgMode::STUFF )
			m_pStaticTitle->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7028 ) ); // "카오스큐브  재료목록"

		// 결과물창.
		else if( m_DlgMode == CDnChaosCubeDlg::EChildDlgMode::PRODUCT )
			m_pStaticTitle->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7027 ) ); // "카오스큐브 결과물"

#ifdef PRE_ADD_CHOICECUBE
		// 재료_초이스 창.
		else if( m_DlgMode == CDnChaosCubeDlg::EChildDlgMode::STUFF_CHOICE )
			m_pStaticTitle->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7028 ) ); // "초이스큐브 재료목록"

		// 결과물_초이스 창.
		else if( m_DlgMode == CDnChaosCubeDlg::EChildDlgMode::PRODUCT_CHOICE )
			m_pStaticTitle->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7027 ) ); // "초이스큐브 결과물"
#endif

		// 아이템슬롯 갱신.
		RefreshList();
	}

	// Close.
	else
	{
		m_crrPage = 0;
	}
}


bool CDnChaosCubeStuffDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
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
void CDnChaosCubeStuffDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl * pControl, UINT uMsg )
{

	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		// 이전.
		if( IsCmdControl("ID_BT_PRIOR") )
		{
			--m_crrPage;
			if( m_crrPage < 0 )
				m_crrPage = m_maxPage;

			// 아이템슬롯 갱신.
			RefreshList();
		}

		// 다음.
		else if( IsCmdControl("ID_BT_NEXT") )
		{
			++m_crrPage;
			if( m_crrPage > m_maxPage )
				m_crrPage = 0;

			// 아이템슬롯 갱신.
			RefreshList();
		}

		else if( IsCmdControl("ID_BT_CLOSE") || IsCmdControl("ID_BT_CANCEL") )
			Show( false );
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}



// 믹스박스번호에 해당하는 재료아이템들을 얻어낸다.	
//void CDnChaosCubeStuffDlg::LoadItems( int nChaosNum, int nItemID, bool bSelf )
void CDnChaosCubeStuffDlg::LoadItems( int nChaosNum, bool bSelf )
{
	m_vItems.clear();

	DNTableFileFormat * pTable = NULL;

	char * strCellName = "_ChaoscubeNum";

	// Table.
	if( m_DlgMode == CDnChaosCubeDlg::EChildDlgMode::STUFF )		// 재료
		pTable = GetDNTable( CDnTableDB::TCHAOSCUBESTUFF );
	else if( m_DlgMode == CDnChaosCubeDlg::EChildDlgMode::PRODUCT ) // 결과물.
		pTable = GetDNTable( CDnTableDB::TCHAOSCUBERESULT );

#ifdef PRE_ADD_CHOICECUBE
	else if( m_DlgMode == CDnChaosCubeDlg::EChildDlgMode::STUFF_CHOICE ) // 재료_초이스.
		pTable = GetDNTable( CDnTableDB::TCHAOSCUBESTUFF );
	
	else if( m_DlgMode == CDnChaosCubeDlg::EChildDlgMode::PRODUCT_CHOICE ) // 결과물_초이스.
		pTable = GetDNTable( CDnTableDB::TCHAOSCUBERESULT );

#endif


	if( !pTable )
		return;	

	CDnItem * pItem = NULL;
	DNTableCell * pCell = NULL;

	int size = pTable->GetItemCount();
	for( int i=0; i<size; ++i )
	{
		int id = pTable->GetItemID( i );
		DNTableCell * pCell = pTable->GetFieldFromLablePtr( id, "_ChaoscubeNum" );
		if( pCell && ( pCell->GetInteger() != nChaosNum ) )
			continue;
			
		pCell = pTable->GetFieldFromLablePtr( id, "_ItemID" );
		if( pCell )
		{	
			/*// 재료아이템과 같은 아이템 로딩여부.
			if( bSelf == false && pCell->GetInteger() == nItemID )
				continue;*/

			TItemInfo info;
			if( CDnItem::MakeItemInfo( pCell->GetInteger(), 1, info ) )
			{
				pItem = CDnItemTask::GetInstance().CreateItem( info );
				if( pItem )
				{
					pCell = pTable->GetFieldFromLablePtr( id, "_Count" );
					if( pCell )
					{
						pItem->SetOverlapCount( pCell->GetInteger() );
						m_vItems.push_back( pItem );
					}
				}
			}
		}

		// 결과 골드량.
#ifdef PRE_ADD_CHOICECUBE
		if( m_DlgMode == CDnChaosCubeDlg::EChildDlgMode::PRODUCT || 
			m_DlgMode == CDnChaosCubeDlg::EChildDlgMode::PRODUCT_CHOICE ) // 결과물.			
#else
		if( m_DlgMode == CDnChaosCubeDlg::EChildDlgMode::PRODUCT ) // 결과물.
#endif	
		{
			pCell = pTable->GetFieldFromLablePtr( id, "_Gold" );
			if( pCell )
				m_mapGold.insert( std::map< int, INT64 >::value_type( i, (INT64)pCell->GetInteger() ) );
		}


	}

	// MaxPage.
	if( m_vItems.empty() )
		m_maxPage = 0;
	else
	{
		int stuffSize = (int)m_vItems.size();
		if( stuffSize >= m_CountPerPage )
			--stuffSize;
		m_maxPage = stuffSize / m_CountPerPage;
	}		

}


// 재료창 or 결과물창.
//void CDnChaosCubeStuffDlg::SetDlgMode( int mode, int nItemID, int nChaosNum, bool bSelf )
void CDnChaosCubeStuffDlg::SetDlgMode( int mode, int nChaosNum, bool bSelf )
{ 		
	//if(	m_DlgMode == mode && m_nChaosNum == nChaosNum )
	//	return;

	//m_nStuffItemID = nItemID; // 재료아이템 아이디.

	m_DlgMode = mode;
	m_nChaosNum = nChaosNum;

	// Load.
	//LoadItems( nChaosNum, nItemID, bSelf );
	LoadItems( nChaosNum, bSelf );

	// 재료창.
	if( m_DlgMode == CDnChaosCubeDlg::EChildDlgMode::STUFF )
		m_pStaticHelp->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7031 ) ); // "본 카오스큐브를 작동시키려면 아래 아이템 중 하나가 필요합니다."

	// 결과물창.
	else if( m_DlgMode == CDnChaosCubeDlg::EChildDlgMode::PRODUCT )		
		m_pStaticHelp->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, bSelf ? 7025 : 7026 ) ); // "본 카오스큐브에서는 아래 목록의 아이템 중 하나가 나타납니다.", "본 카오스큐브에서는 아래 목록 중, 넣은 재료를 제외한 아이템 중 하나가 나타납니다."

#ifdef PRE_ADD_CHOICECUBE
	// 재료_초이스 창.
	else if( m_DlgMode == CDnChaosCubeDlg::EChildDlgMode::STUFF_CHOICE )
		m_pStaticHelp->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7038 ) ); // "본 초이스 큐브에서는 아래 목록의 아이템 중 하나를 선택할 수 있습니다."

	// 결과물_초이스 창.
	else if( m_DlgMode == CDnChaosCubeDlg::EChildDlgMode::PRODUCT_CHOICE )
		m_pStaticHelp->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7039 ) ); // "본 초이스 큐브에서는 아래 목록 중, 넣은 재료를 제외한 아이템 중 하나를 선택할 수 있습니다."
#endif

}


// 아이템목록 페이징처리.
void CDnChaosCubeStuffDlg::RefreshList()
{
	// 아이템슬롯들 초기화.
	int size = (int)m_vSlotBtns.size();
	for( int i=0; i<size; ++i )
	{
		m_vSlotBtns[ i ]->ResetSlot();	
		m_vSlotStatics[ i ]->SetText( std::wstring(L"") );
	}
		
	size = m_crrPage * m_CountPerPage;		

	// 현재페이지에 해당하는 아이템들을 슬롯에 등록한다.	
	for( int i=0; i<m_CountPerPage; ++i )
	{
		unsigned int index = size + i;
		if( index >= m_vItems.size() )
			break;

		CDnItem * pItem = m_vItems[ index ];
		// 슬롯에 등록.
		m_vSlotBtns[ i ]->SetItem( pItem, pItem->GetOverlapCount() );

		// 골드인 경우.
		if( pItem->GetClassID() == 1073750029 ) 
		{
			std::map< int, INT64 >::iterator it = m_mapGold.find( i );
			if( it != m_mapGold.end() )
			{
				std::wstring str;
				GoldToString( str, (*it).second,
					GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1766 ),
					GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1781 ),		
					GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1783 ) );
				m_vSlotStatics[ i ]->SetText( str );
			}
		}
		// 골드가아닌 아이템의 경우.
		else
			m_vSlotStatics[ i ]->SetText( pItem->GetName() );
	}

	// 페이지표시.
	wchar_t str[10];
	swprintf_s( str, 10, L"%d / %d", m_crrPage+1, m_maxPage+1 );
	m_pStaticPage->SetText( str );

}

#endif