#include "stdafx.h"
#include "DnPackageBoxResultDlg.h"
#include "DnItemTask.h"
#include "DnLocalPlayerActor.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

#if defined( PRE_ADD_EASYGAMECASH )

CDnPackageBoxResultDlg::CDnPackageBoxResultDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog * pParentDialog, int nID, CEtUICallback * pCallback )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback, true )
, m_pPriorButton( NULL )
, m_pNextButton( NULL )
, m_pQuestionStatic( NULL )
, m_nSelectPage( 1 )
, m_nMaxSelectPage( 1 )
#ifdef PRE_ADD_GETITEM_WITH_RANDOMVALUE
, m_nSoundIndex( -1 )
#endif 
{
	SecureZeroMemory( m_pPageButton, sizeof( CEtUIButton *) * ePackageBox_Page_Count );
}

CDnPackageBoxResultDlg::~CDnPackageBoxResultDlg()
{
	RefreshControl();
}

void CDnPackageBoxResultDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "PackageBox.ui" ).c_str(), bShow );
}

void CDnPackageBoxResultDlg::InitialUpdate()
{
	char szUIName[256] = {0,};
	for( int itr = 0; itr < ePackageBox_Item_Count; ++itr )
	{
		sprintf_s( szUIName, _countof(szUIName), "ID_ITEM%d", itr );
		m_pPackageList[itr].m_pItemSlot = GetControl<CDnItemSlotButton>( szUIName );

		sprintf_s( szUIName, _countof(szUIName), "ID_TEXT_NAME%d", itr );
		m_pPackageList[itr].m_pItemName = GetControl<CEtUIStatic>( szUIName );
	}

	for( int itr = 0; itr < ePackageBox_Page_Count; ++itr )
	{
		sprintf_s( szUIName, _countof(szUIName), "ID_BT_PAGE%d", itr );
		m_pPageButton[itr] = GetControl<CEtUIButton>( szUIName );
	}

	m_pPriorButton = GetControl<CEtUIButton>( "ID_BT_PRIOR" );
	m_pNextButton = GetControl<CEtUIButton>( "ID_BT_NEXT" );
	m_pQuestionStatic = GetControl<CEtUIStatic>( "ID_STATIC0" );

#ifdef PRE_ADD_SMARTMOVE_PACKAGEBOX
	m_SmartMove.SetControl(GetControl<CEtUIButton>("ID_BT_OK"));
#endif

#ifdef PRE_ADD_GETITEM_WITH_RANDOMVALUE
	m_nSoundIndex = CEtSoundEngine::GetInstance().LoadSound( "UI_CharmItemConfirm.ogg", false, false );
#endif
}

void CDnPackageBoxResultDlg::Show( bool bShow )
{
	if( m_bShow == bShow )
		return;

#ifdef PRE_ADD_SMARTMOVE_PACKAGEBOX
	if (bShow)
	{
		m_SmartMove.MoveCursor();
		SetPackageBoxItem();
#ifdef PRE_ADD_GETITEM_WITH_RANDOMVALUE
		if (m_nSoundIndex != -1)
			CEtSoundEngine::GetInstance().PlaySound( "2D", m_nSoundIndex );
#endif 
	}
	else
	{
		m_SmartMove.ReturnCursor();
	}
#else
	if( true == bShow )
		SetPackageBoxItem();
#endif

	CEtUIDialog::Show( bShow );
}

void CDnPackageBoxResultDlg::Process( float fElapsedTime )
{
	CEtUIDialog::Process( fElapsedTime );
}

void CDnPackageBoxResultDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl("ID_BT_OK") || IsCmdControl("ID_BT_CLOSE") )
		{
			Show(false);
			return;
		}
		else if( IsCmdControl("ID_BT_PRIOR") )
		{
			PrevPage();
		}
		else if( IsCmdControl("ID_BT_NEXT") )
		{
			NextPage();
		}
		else if( strstr( pControl->GetControlName(), "ID_BT_PAGE" ) )
		{
			int nSlotIndex;
			sscanf_s( pControl->GetControlName(), "ID_BT_PAGE%d", &nSlotIndex );

			m_nSelectPage = nSlotIndex + 1;
			SelectPage( m_nSelectPage );
		}
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

bool CDnPackageBoxResultDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
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
		}
		break;
	}

	return CEtUIDialog::MsgProc( hWnd, uMsg, wParam, lParam );
}
#ifdef PRE_ADD_GETITEM_WITH_RANDOMVALUE
void CDnPackageBoxResultDlg::SetInvenData( const BYTE cInvenType, const short sInvenIndex, TRangomGiveItemData* pItemlist, int nitemCount )
{
	ClearPackageList();
	if( nitemCount > 0 ) {
		for( int i = 0; i < nitemCount; ++i )
			m_vPackageItem.push_back( boost::make_tuple<int, int, int>( pItemlist[i].nItemID, pItemlist[i].nCount, pItemlist[i].nPeriod ) );
		return ;
	}
	ProcessPerminPlayer();
}
#endif 

#ifdef PRE_FIX_MAKECHARMITEM
void CDnPackageBoxResultDlg::SetInvenData( const BYTE cInvenType, const short sInvenIndex, const INT64& nCharmItemSerial )
#else
void CDnPackageBoxResultDlg::SetInvenData( const BYTE cInvenType, const short sInvenIndex )
#endif
{
	ClearPackageList();
#ifdef PRE_FIX_MAKECHARMITEM
	GetItemTask().MakeCharmItemList( m_vPackageItem, sInvenIndex, cInvenType, nCharmItemSerial );
#else
	GetItemTask().MakeCharmItemList( m_vPackageItem, sInvenIndex, cInvenType );
#endif
	ProcessPerminPlayer();
}

void CDnPackageBoxResultDlg::NextPage()
{
	if( m_nMaxSelectPage > m_nSelectPage )
	{
		++m_nSelectPage;
		SelectPage( m_nSelectPage );
	}
}

void CDnPackageBoxResultDlg::PrevPage()
{
	if( 1 < m_nSelectPage )
	{
		--m_nSelectPage;
		SelectPage( m_nSelectPage );
	}
}

void CDnPackageBoxResultDlg::RefreshControl()
{
	for( int itr = 0; itr < ePackageBox_Item_Count; ++itr )
	{
		CDnItem * pItem = (CDnItem *)m_pPackageList[itr].m_pItemSlot->GetItem();
		if( NULL != pItem )
			SAFE_DELETE( pItem );

		m_pPackageList[itr].m_pItemSlot->Show( false );
		m_pPackageList[itr].m_pItemName->SetText( L"" );
	}

	for( int itr = 0; itr < ePackageBox_Page_Count; ++itr )
		m_pPageButton[ itr ]->Enable( (m_nSelectPage - 1) != itr );
}

void CDnPackageBoxResultDlg::ClearPackageList()
{
	m_vPackageItem.clear();
	m_nSelectPage = 1;
	m_nMaxSelectPage = 1;

	for( int itr = 0; itr < ePackageBox_Page_Count; ++itr )
	{
		m_pPageButton[itr]->SetText( L"" );
		m_pPageButton[itr]->Show( false );
	}
}

void CDnPackageBoxResultDlg::SetPackageBoxItem()
{
	m_nMaxSelectPage = (int)m_vPackageItem.size() / ePackageBox_Page_Count;

	if( 0 != m_vPackageItem.size() % ePackageBox_Page_Count )
		++m_nMaxSelectPage;

	for( int itr = 0; itr < m_nMaxSelectPage; ++itr )
	{
		if( ePackageBox_Page_Count <= itr )
			break;

		m_pPageButton[itr]->SetIntToText( itr + 1 );
		m_pPageButton[itr]->Show( true );
	}

	SelectPage( m_nSelectPage );
}

void CDnPackageBoxResultDlg::SelectPage( const int nSelectPage )
{
	RefreshControl();

	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TITEM );
	if( NULL == pSox )
		return;

	DWORD nItemIndex = (nSelectPage - 1) * ePackageBox_Max_1Page_Count;

	for( int itr = 0; itr < ePackageBox_Max_1Page_Count; ++itr )
	{
		if( m_vPackageItem.size() <= nItemIndex )
			break;

		CDnItem * pItem = NULL;
		int nItemID = m_vPackageItem[nItemIndex].get<0>();

		eItemTypeEnum Type = CDnItem::GetItemType( nItemID );

		if( eItemTypeEnum::ITEMTYPE_PARTS == Type )
			pItem = CDnParts::CreateParts( nItemID, 0 );
		else
			pItem = CDnItem::CreateItem( nItemID, 0 );

		if( NULL == pItem )
			continue;

		int nIconIndex = -1;
		eItemRank itemRank = ITEMRANK_D;
		eItemTypeEnum itemType = ITEMTYPE_NORMAL;

		const DNTableCell* pIconImageField = pSox->GetFieldFromLablePtr( nItemID, "_IconImageIndex" );
		if (pIconImageField)
			nIconIndex = pIconImageField->GetInteger();

		const DNTableCell* pRankField = pSox->GetFieldFromLablePtr( nItemID, "_Rank" );
		if (pRankField)
			itemRank = (eItemRank)pRankField->GetInteger();

		const DNTableCell* pItmeTypeField = pSox->GetFieldFromLablePtr( nItemID, "_Type" );
		if (pItmeTypeField)
			itemType = (eItemTypeEnum)pItmeTypeField->GetInteger();

		pItem->ChangeIconIndex(nIconIndex);
		pItem->ChangeRank(itemRank);
		pItem->ChangeItemType(itemType);

		m_pPackageList[itr].m_pItemSlot->Show( true );
		m_pPackageList[itr].m_pItemSlot->SetItem( pItem, m_vPackageItem[nItemIndex].get<1>() );

		if( 0 != m_vPackageItem[nItemIndex].get<2>() )
		{
			WCHAR wszDay[256] = {0,};
			WCHAR wszTitle[256] = {0,};
			swprintf_s( wszDay, _countof(wszDay), GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 1916), m_vPackageItem[nItemIndex].get<2>() );	// UISTRING : %dÀÏ
			swprintf_s( wszTitle, _countof(wszTitle), L"%s (%s)", pItem->GetName(), wszDay );
			m_pPackageList[itr].m_pItemName->SetText( wszTitle );
		}
		else
			m_pPackageList[itr].m_pItemName->SetText( pItem->GetName() );

		++nItemIndex;
	}
}

void CDnPackageBoxResultDlg::ProcessPerminPlayer()
{
	std::vector<int> nVecJobList;
	((CDnPlayerActor*)CDnActor::s_hLocalActor.GetPointer())->GetJobHistory( nVecJobList );

	std::vector<boost::tuple<int, int, int>>::iterator Itor = m_vPackageItem.begin();
	while( Itor != m_vPackageItem.end() )
	{
		if( false == CDnItem::IsPermitPlayer( Itor->get<0>(), nVecJobList ) )
		{
			Itor = m_vPackageItem.erase( Itor );
			if( m_vPackageItem.end() == Itor )
				return;
		}
		else
			++Itor;
	}
}
#endif	// #if defined( PRE_ADD_EASYGAMECASH )