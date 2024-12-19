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
m_pItemChaos(NULL), // ī����������.
m_nChaosSerial(0),
m_CountPerPage(5), // �������� ���԰���.
m_crrPage(0), // ����������.
m_maxPage(0), // ��ü������.
m_signLight(1), // ������ώ�� ����ó�� ��ȣ.
m_fAlphaColor(1.0f),
m_bSelfItemList(true), // ����۰� ���� ������ �ε�����.
m_SelectedSlotIndex(-1) // ������ ��ώ�� �ε���.
{
	m_pSlotBtnSelected = NULL; // ��������ώ��.	
	m_pStaticLight = NULL; // ��¦��.
	m_pStaticTitle = NULL; // Ÿ��Ʋ��.
	m_pStaticHelp = NULL;  // ����.
	m_pStaticPage = NULL;  // Page/MaxPage.

	m_pBtnPrev = NULL;     // ����.
	m_pBtnNext = NULL;     // ����.


	m_pStaticLightResult = NULL;
	m_pStaticPageResult = NULL;
	m_pSlotBtnSelectedResult = NULL; // ������ ���������.

	m_pBtnMaterial = NULL; // ���.
	m_pBtnProduct = NULL;  // �����.
	m_pBtnConfirm = NULL;  // Ȯ��.

	m_pWheelRegionStuff = NULL; // ��ώ�� �� ����.
	m_pWheelRegionResult = NULL; // ��������� �� ����.

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
	m_vSlotBtnMaterials.clear(); // ��ώ�Ե�.
	m_vStaticSelect.clear(); // ��ώ�� ���� Static.
	m_vNeedStuffCount.clear(); // �ͽ��� �ʿ��� ��ᰳ��.
	m_vStuffItemIDs.clear(); // ��������.


	m_vSlotBtnMaterialsResult.clear(); // ��ώ�Ե�.
	m_vStaticSelectResult.clear(); // ��ώ�� ���� Static.

	m_vResultItems.clear(); // ��������۵�.

	m_vPeriodItems.clear(); // ������Կ� �Ⱓ��ǥ�ÿ� Static.

}


// Initialize()
void CDnChoiceCubeDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName("ChoiceBoxDlg.ui").c_str(), bShow );
}


// InitialUpdate()
void CDnChoiceCubeDlg::InitialUpdate()
{

	//---  ��ώ�Ե�  ---//
	m_pSlotBtnSelected = GetControl<CDnItemSlotButton>("ID_ITEM0"); // ��������ώ��.
	m_pStaticLight = GetControl<CEtUIStatic>("ID_STATIC_LIGHT0");    // ��¦��.
		
	m_pStaticPage = GetControl<CEtUIStatic>("ID_TEXT_PAGE0"); // Page/MaxPage.

	// ��ώ�Ե�.
	char buf[64] = {0,};
	m_vSlotBtnMaterials.resize( m_CountPerPage );	
	for( int i=0; i<m_CountPerPage; ++i )
	{
		sprintf_s( buf, 64, "ID_SAUCEITEM%d", i );
		m_vSlotBtnMaterials[ i ] = GetControl<CDnItemSlotButton>( buf );
	}

	// ��ώ�� ���� Static.
	m_vStaticSelect.resize( m_CountPerPage );	
	for( int i=0; i<m_CountPerPage; ++i )
	{
		sprintf_s( buf, 64, "ID_SAUCESELECT%d", i );
		m_vStaticSelect[ i ] = GetControl<CEtUIStatic>( buf );
	}	


	//---  ��������Ե�  ---//
	m_pSlotBtnSelectedResult = GetControl<CDnItemSlotButton>("ID_ITEM1"); // ������ ���������.
	m_pStaticLightResult = GetControl<CEtUIStatic>("ID_STATIC_LIGHT1"); // ID_STATIC_LIGHT_RESULT
	m_pStaticPageResult = GetControl<CEtUIStatic>("ID_TEXT_PAGE1"); // Page/MaxPage.
	
		
	// ���Ե�.	
	m_vSlotBtnMaterialsResult.resize( m_CountPerPage );	
	for( int i=0; i<m_CountPerPage; ++i )
	{
		sprintf_s( buf, 64, "ID_RESULTITEM%d", i );
		m_vSlotBtnMaterialsResult[ i ] = GetControl<CDnItemSlotButton>( buf );
	}

	// ���� ���� Static.
	m_vStaticSelectResult.resize( m_CountPerPage );	
	for( int i=0; i<m_CountPerPage; ++i )
	{
		sprintf_s( buf, 64, "ID_RESULTSELECT%d", i );
		m_vStaticSelectResult[ i ] = GetControl<CEtUIStatic>( buf );
	}	

	// ������Կ� �Ⱓ��ǥ�ÿ� Static.
	m_vPeriodItems.resize( m_CountPerPage ); 
	for( int i=0; i<m_CountPerPage; ++i )
	{
		sprintf_s( buf, 64, "ID_STATIC_TIME%d", i );
		m_vPeriodItems[ i ] = GetControl<CEtUIStatic>( buf );
	}

	m_pBtnMaterial = GetControl<CEtUIButton>("ID_BT_LIST0"); // ���.
	m_pBtnProduct = GetControl<CEtUIButton>("ID_BT_LIST1");    // �����.


	m_pStaticTitle = GetControl<CEtUIStatic>("ID_TEXT_TITLE"); // Title.
	m_pStaticHelp = GetControl<CEtUIStatic>("ID_TEXT_ASK0"); // Ask. 

	m_pBtnConfirm = GetControl<CEtUIButton>("ID_OK"); // Ȯ��.

	m_pWheelRegionStuff = GetControl<CEtUIStatic>("ID_STATIC7");  // ��ώ�� �� ����.
	m_pWheelRegionResult = GetControl<CEtUIStatic>("ID_STATIC8"); // ��������� �� ����.
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
		m_pBtnConfirm->Enable( false ); // Ȯ�ι�ư ��Ȱ��ȭ.
		OpenDlg();
	}

	// Close.
	else
	{
		// Dlg Close�ÿ� ����.
		CloseDlg();		
	}
}


void CDnChoiceCubeDlg::Process( float fElapsedTime )
{
	CEtUIDialog::Process( fElapsedTime );
	
	// ���õ� ������� �ִ°�쿡 ������ �ִϸ��̼�ó��.
	if( m_pSlotBtnSelected->GetItem() || m_pSlotBtnSelectedResult->GetItem() )
	{		
		D3DXCOLOR color( m_pStaticLight->GetTextureColor() );

		if( m_fAlphaColor <= 0.3f )
			m_signLight = 1;
		if( m_fAlphaColor >= 1.0f )
			m_signLight = -1;

		m_fAlphaColor += fElapsedTime * m_signLight;
		color.a = m_fAlphaColor;

		// ��ώ��.
		if( m_pSlotBtnSelected->GetItem() )
			m_pStaticLight->SetTextureColor( color );

		// �������.
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

				// ��ώ�Կ���.
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

				// ��������Կ���.
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
			// �������ۼ���.
			SelectionStuffItem( pControl->GetControlName() );			
		}

		if( strstr( pControl->GetControlName(), "ID_RESULTITEM" ) )
		{
			// ����������ۼ���.
			SelectionResultItem( pControl->GetControlName() );
		}

		// ����.
		else if( IsCmdControl("ID_BT_PRIOR0") )
		{
			--m_crrPage;
			if( m_crrPage < 0 )
				m_crrPage = m_maxPage;

			// ��ώ�� ����.
			RefreshStuffItemSlot();
		}

		// ����.
		else if( IsCmdControl("ID_BT_NEXT0") )
		{
			++m_crrPage;
			if( m_crrPage > m_maxPage )
				m_crrPage = 0;

			// ��ώ�� ����.
			RefreshStuffItemSlot();
		}


		// ������� ����.
		else if( IsCmdControl("ID_BT_PRIOR1") )
		{
			--m_crrPageResult;
			if( m_crrPageResult < 0 )
				m_crrPageResult = m_maxPageResult;
			
			// ��������� ����.
			RefreshResultItemSlot();
		}

		// ������� ����.
		else if( IsCmdControl("ID_BT_NEXT1") )
		{
			++m_crrPageResult;
			if( m_crrPageResult > m_maxPageResult )
				m_crrPageResult = 0;

			// ��������� ����.
			RefreshResultItemSlot();
		}



		// ���â����.
		else if( IsCmdControl("ID_BT_LIST0") )
		{
			CDnChaosCubeStuffDlg * pStuffDlg = GetInterface().GetChaosCubeStuffDlg();
			if( pStuffDlg )
			{
				pStuffDlg->SetDlgMode( CDnChaosCubeDlg::EChildDlgMode::STUFF_CHOICE, m_pItemChaos->GetTypeParam(0), m_bSelfItemList );
				pStuffDlg->Show( true );
			}
		}

		// ���â����.
		else if( IsCmdControl("ID_BT_LIST1") )
		{
			CDnChaosCubeStuffDlg * pStuffDlg = GetInterface().GetChaosCubeStuffDlg();
			if( pStuffDlg )
			{
				pStuffDlg->SetDlgMode( CDnChaosCubeDlg::EChildDlgMode::PRODUCT_CHOICE, m_pItemChaos->GetTypeParam(0), m_bSelfItemList );
				pStuffDlg->Show( true );
			}
		}

		// �ݱ�.
		else if( IsCmdControl("ID_BT_CLOSE") || IsCmdControl("ID_CANCEL") )
			Show( false );

		// �ͽ�����.
		else if( IsCmdControl("ID_OK") )
		{
			// �κ��丮������ �����ִ��� Ȯ��.
			int needEmptyCount = (int)CGlobalWeightIntTable::GetInstance().GetValue(CGlobalWeightIntTable::ChaosCubeEmptySlotCount);

			// �Ϲ��κ��� ����Ȯ��.
			int nCurEmptySlotCount = 0;
			CDnInvenTabDlg *pInvenDlg = (CDnInvenTabDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::INVENTORY_DIALOG );
			if( pInvenDlg ) 
			{
				nCurEmptySlotCount = pInvenDlg->GetEmptySlotCount();

				// �κ������� ������.
				if( nCurEmptySlotCount < needEmptyCount )
				{
					wchar_t strMsg[256] = {0,};
					swprintf_s( strMsg, 256, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7052 ), needEmptyCount );
					GetInterface().MessageBox( strMsg, MB_OK );	
				}
				else
				{
					GetInterface().MessageBox( 7040, MB_YESNO, 1, this ); // "������ ��ᰡ ������� ������ �������� ȹ��˴ϴ�. ������ ���̽�ť�긦 �۵��Ͻðڽ��ϱ�?"
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
				MixStart(); // �ͽ�����.
			}
		}
	}

}


// Dlg Close�ÿ� ����.
void CDnChoiceCubeDlg::CloseDlg()
{
	m_crrPage = 0;
	m_crrPageResult = 0;

	// ��ἱ�ý��� �ʱ�ȭ.
	m_pSlotBtnSelected->ResetSlot();

	// ��ώ�� �ʱ�ȭ.
	int size = (int)m_vSlotBtnMaterials.size();
	for( int i=0; i<size; ++i )
		m_vSlotBtnMaterials[ i ]->ResetSlot();

	// �κ����� �˻��� �������۵�.
	std::map< int, std::vector< CDnItem * > >::iterator it = m_mapStuffItems.begin();
	for( ; it != m_mapStuffItems.end(); ++it )
		(*it).second.clear();
	m_mapStuffItems.clear();	

	// Hide - ��ώ�� ���� Static.
	size = (int)m_vStaticSelect.size();
	for( int i=0; i<size; ++i )
		m_vStaticSelect[ i ]->Show( false ); 
	

	// ������� �ʱ�ȭ.
	m_pSlotBtnSelectedResult->ResetSlot();
	size = (int)m_vSlotBtnMaterialsResult.size();
	for( int i=0; i<size; ++i )
		m_vSlotBtnMaterialsResult[ i ]->ResetSlot();

	// Hide - ��������� ���� Static.
	size = (int)m_vStaticSelectResult.size();
	for( int i=0; i<size; ++i )
		m_vStaticSelectResult[ i ]->Show( false ); 
	

	// ��������ý��� ���� ����.
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

	m_pStaticHelp->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7019 ) ); // "ī����ť�꿡 ���� ��Ḧ ������ �ּ���."

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


	// �ͽ��ڽ���ȣ�� �ش��ϴ� �������۵��� ����.	
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


	// ����� ���Ե�.
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

					// ����.
					pCell = pTable->GetFieldFromLablePtr( id, "_Count" );
					if( pCell )
					{
						pNewItem = CDnItem::CreateItem( itemID, 0 );
						pNewItem->SetOverlapCount( pCell->GetInteger() );

						// �Ⱓ��.
						pCell = pTable->GetFieldFromLablePtr( id, "_Period" );
						if( pCell )
						{
							SResultItem aNewItem;
							aNewItem.pItem = pNewItem;
							aNewItem.nPeriod = pCell->GetInteger();		

							// ���.
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


	// �����۸�ϰ���.
	UpdateItemList(); 

}


// �����۸�ϰ���.
void CDnChoiceCubeDlg::UpdateItemList()
{	

	// ��ώ�� -------------//
	if( !m_vStuffItemIDs.empty() )
	{			
		m_pStaticHelp->SetTextColor( textcolor::WHITE );
		m_pStaticHelp->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7019 ) ); // "ī����ť�꿡 ���� ��Ḧ ������ �ּ���."

		m_pItemChaos = GetItemTask().GetCashInventory().FindItemFromSerialID( m_nChaosSerial );

		// �����۾���.
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
			// �κ��丮���� ���˻�.
			std::vector< CDnItem * > vResult;
			for( int k=0; k<size; ++k )
			{
				pInven = arrInven[ k ];
				int tot = pInven->ScanItemFromID( m_vStuffItemIDs[ i ], &vResult );

				// ã����.
				if( !vResult.empty() )
				{
					++stuffItemCnt;
					m_mapStuffItems.insert( std::map< int, std::vector< CDnItem * > >::value_type( m_vStuffItemIDs[ i ], vResult ) ); // map �� ����۵� ����.
					break;
				}
			}
		}

		if( stuffItemCnt >= m_CountPerPage )
			--stuffItemCnt;
		m_maxPage = stuffItemCnt / m_CountPerPage;
	}

	// ��ώ�� ����.
	RefreshStuffItemSlot();



	// ��������� -----------//
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
	
	// ��������� ����.
	RefreshResultItemSlot();

}


// ��ώ�Կ� �����.
void CDnChoiceCubeDlg::RefreshStuffItemSlot()
{
	// ��ἱ��ǥ�� ����.
	for( int i=0; i<m_CountPerPage; ++i )
		m_vStaticSelect[ i ]->Show( false );

	// ��ώ�� �ʱ�ȭ.
	int size = (int)m_vSlotBtnMaterials.size();
	for( int i=0; i<size; ++i )
	{
		m_vSlotBtnMaterials[ i ]->ResetSlot();
		m_vSlotBtnMaterials[ i ]->SetSlotType( ST_ITEM_NONE );
	}
	
	if( m_mapStuffItems.empty() )
	{
		m_pStaticHelp->SetTextColor( textcolor::RED );
		m_pStaticHelp->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7033 ) ); // "�� ī���� ť�꿡 ���� �� �ִ� ��Ḧ ������ ���� �ʽ��ϴ�."
		return;
	}

	// ������������ �ش��ϴ� ������ ���Կ� ����Ѵ�.	
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

		// ����.
		int tot = 0;
		for( int k=0; k<(int)vItems.size(); ++k )
			tot += vItems[ k ]->GetOverlapCount();

		// ���Կ� ���.
		m_vSlotBtnMaterials[ i ]->SetItem( vItems[0], tot );

		// ��ᰳ���� ���ڶ� ��� ������Ʈ������ RED�� ����.		
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

	// ������ǥ��.
	wchar_t str[128];
	swprintf_s( str, 10, L"%d / %d", m_crrPage+1, m_maxPage+1 );
	m_pStaticPage->SetText( str );

}

// �������ۼ���.
void CDnChoiceCubeDlg::SelectionStuffItem( const char * strCtlName )
{
	// ����ǥ��.
	std::string str( strCtlName );
	std::string strNumber = str.substr( strlen("ID_SAUCEITEM"), 1 );
	int idx = atoi( strNumber.c_str() );
	if( idx >= 0 && idx < m_CountPerPage )
	{
		// ���úκ� �ܴ̿� ����ǥ�� ����.
		for( int i=0; i<m_CountPerPage; ++i )
			m_vStaticSelect[ i ]->Show( false );

		MIInventoryItem * pSlotItem = m_vSlotBtnMaterials[ idx ]->GetItem();
		if( !pSlotItem )
			return;

		CDnItem * pSelectedItem = static_cast< CDnItem * >( pSlotItem );
		if( pSelectedItem )
		{
			m_SelectedSlotIndex = -1;			

			// �ʿ��� ��ᰳ�� Ȯ��.
			int totItemCount = m_vSlotBtnMaterials[ idx ]->GetRenderCount(); // �κ��� ����.

			std::map< int, int >::iterator it = m_vNeedStuffCount.find( pSelectedItem->GetClassID() );
			if( it != m_vNeedStuffCount.end() )
			{
				int needCount = (*it).second; // �ʿ� ��ᰳ��.

				// ��ᰡ ������.
				if( totItemCount < needCount )
				{
					m_pBtnConfirm->Enable( false ); // Ȯ�ι�ư Ȱ��ȭ.
					GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7022) ); // "�ּ� ������ ������ ���� ī����ť�꿡 ���� �� �����ϴ�."
				}

				// OK.
				else
				{
					m_vStaticSelect[ idx ]->Show( true );	

					// ������ �������� ������ώ�Կ� ���.	
					m_pSlotBtnSelected->SetItem( pSelectedItem, needCount );		

					// �����ʱ�ȭ.
					m_fAlphaColor = 1.0f;

					// ��������ۼ��ÿ��� - 
					// 7034 : "ȹ���ϰ� ���� �������� ����� ��Ͽ��� ������ �ּ���."
					// 7035 : "����� �������� Ȯ���Ͻð� Ȯ�� ��ư�� Ŭ���� �ּ���."
					m_pStaticHelp->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 
						m_pSlotBtnSelectedResult->GetItem() == NULL ? 7034 : 7035 ) );					

					// ������ ��ώ�� �ε���.
					m_SelectedSlotIndex = idx;

					if( m_pSlotBtnSelectedResult->GetItem() != NULL )
					{
						// �������۰� ������������� ������ ��.
						if( CompareStuffnResult() )
							m_pBtnConfirm->Enable( true ); // Ȯ�ι�ư Ȱ��ȭ.						
					}
					
				}
			}			
		}		
	}
}

// ��������Կ� ��������.
void CDnChoiceCubeDlg::RefreshResultItemSlot()
{
	// ������Ե� -------------//


	// ����ǥ�� ����.
	for( int i=0; i<m_CountPerPage; ++i )
		m_vStaticSelectResult[ i ]->Show( false );

	// ���� �ʱ�ȭ.
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

				// �Ⱓ���������� ��� - 
				m_vPeriodItems[ slotIndex ]->Show( ( rResultItem.nPeriod > 0 ? true : false ) ); //������ǥ��.
				if( rResultItem.nPeriod )
				{
					std::wstring strTooltip = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4706 ), rResultItem.nPeriod );
					m_vSlotBtnMaterialsResult[ slotIndex ]->SetTooltipText( strTooltip.c_str() );
				}
				else
				{
					m_vSlotBtnMaterialsResult[ slotIndex ]->ClearTooltipText();
				}

				// ���������� ��� ������ ��� ���.
				if( rResultItem.pItem->GetClassID() == 1073750029 )
				{
					// ���ȹ�� - 0�� ������ ������� ����. (ex> 1���0�ǹ�1���� => 1���1���� ).
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

	// ������ǥ��.	
	wchar_t str[128] = {0,};
	swprintf_s( str, 10, L"%d / %d", m_crrPageResult+1, m_maxPageResult+1 );
	m_pStaticPageResult->SetText( str );	

}


// ����������ۼ���.
void CDnChoiceCubeDlg::SelectionResultItem( const char * strCtlName )
{
	// ����ǥ��.
	std::string str( strCtlName );
	std::string strNumber = str.substr( strlen("ID_RESULTITEM"), 1 );
	int idx = atoi( strNumber.c_str() );
	if( idx >= 0 && idx < m_CountPerPage )
	{
		// ���úκ� �ܴ̿� ����ǥ�� ����.
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

			// ������ �������� ������ώ�Կ� ���.	
			m_pSlotBtnSelectedResult->SetItem( pSelectedItem, pSelectedItem->GetOverlapCount() );		

			// �����ʱ�ȭ.
			m_fAlphaColorResult = 1.0f;

			//m_pStaticHelp->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7020 ) ); // "������ ��Ḧ Ȯ���ϰ� Ȯ�� ��ư�� Ŭ�����ּ���. ������ ������ ��ᰡ ���ο� ���������� ��ȭ�˴ϴ�."

			// ������ ��ώ�� �ε���.
			m_SelectedSlotIndexResult = idx;

			if( m_pSlotBtnSelected->GetItem() != NULL )
			{
				m_pStaticLightResult->Show( true );

				// �������۰� ������������� ������ ��.
				if( CompareStuffnResult() )
					m_pBtnConfirm->Enable( true ); // Ȯ�ι�ư Ȱ��ȭ.				
			}
		}
	}

}


// �ͽ�����.
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

	// �ʿ��� ��� ����.
	std::map< int, int >::iterator it = m_vNeedStuffCount.find( pSelectedItem->GetClassID() );
	if( it != m_vNeedStuffCount.end() )
	{
		int needCount = (*it).second; // �ʿ� ��ᰳ��.
		packet.nCount = 0;

		std::map< int, std::vector< CDnItem * > >::iterator mapIt = m_mapStuffItems.find( pSelectedItem->GetClassID() );
		if( mapIt != m_mapStuffItems.end() )
		{		
			// �κ�Ÿ�� - Ż��.
			CDnItem * pItem = GetItemTask().FindItem( pSelectedItem->GetClassID(), ST_INVENTORY_VEHICLE ); 
			if( pItem )
				packet.cInvenType = ITEMPOSITION_VEHICLE;
			else
			{
				// �κ� - ĳ��.
				if( pSelectedItem->IsCashItem() )
					packet.cInvenType = ITEMPOSITION_CASHINVEN; 
				else
				{
					// �κ� - �Ϲ�.
					pItem = GetItemTask().FindItem( pSelectedItem->GetClassID(), ST_INVENTORY ); 
					if( pItem )
						packet.cInvenType = ITEMPOSITION_INVEN;
					else
					{
						// �κ�Ÿ�Կ���.
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

				// ���ʿ��ϴ� ���������۵� �������.
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


// ī����������.
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


// �ͽ��Ϸ�.
void CDnChoiceCubeDlg::MixComplete()
{
	m_pStaticHelp->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7019 ) ); // "ī����ť�꿡 ���� ��Ḧ ������ �ּ���."
}


// ��������� ���ɿϷ�.
void CDnChoiceCubeDlg::RecvResultItemComplet()
{
	// ���.
	if( m_SelectedSlotIndex != -1 )
	{
		char buf[10];
		std::string str( "ID_SAUCEITEM" );
		str.append( itoa(m_SelectedSlotIndex,buf,10) );

		SelectionStuffItem( str.c_str() );
	}

	// ���.
	if( m_SelectedSlotIndexResult != -1 )
	{
		char buf[10];
		std::string str( "ID_RESULTITEM" );
		str.append( itoa(m_SelectedSlotIndexResult,buf,10) );

		SelectionResultItem( str.c_str() );
	}

}

// �ͽ����� ����.
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


// �������۰� ������������� ������ ��.
bool CDnChoiceCubeDlg::CompareStuffnResult()
{
	int pStuffID = m_pSlotBtnSelected->GetItem()->GetClassID();
	int pResultID = m_pSlotBtnSelectedResult->GetItem()->GetClassID();
	
	bool bSame = ( pStuffID == pResultID );
	int typeParam2 = m_pItemChaos->GetTypeParam( 1 );

	bool bRes = true;

	// ����������.
	if( bSame )
	{
		bRes = (typeParam2 == 0 || typeParam2 == 1) ? true : false; // ������ �����϶� true, �ƴϸ� false.
	}

	// �޼���ó�� & �������������.
	if( !bRes )
	{
		m_pSlotBtnSelectedResult->ResetSlot();
		m_vStaticSelectResult[ m_SelectedSlotIndexResult ]->Show( false ); // ���ý���ǥ�� ����.
		m_pStaticLightResult->Show( false );
		GetInterface().MessageBox( 7041, MB_OK ); // "�� ť�꿡���� ���� ������� �����ϰ� ������ �� �����ϴ�."
	}
	else
	{
		m_pStaticLightResult->Show( true );
	}

	return bRes;
}


// �������� �Ⱓ������( ��ȯ�� : �Ⱓ ).
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
