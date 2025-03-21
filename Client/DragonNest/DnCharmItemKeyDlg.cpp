#include "StdAfx.h"
#include "DnCharmItemKeyDlg.h"
#include "DnItem.h"
#include "DnInterface.h"

#include "DnItemTask.h"
#include "DnMainFrame.h"
#include "DnTableDB.h"
#include "DnMainMenuDlg.h"
#include "DnInvenTabDlg.h"
#include "DnMailDlg.h"
#include "ItemSendPacket.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnCharmItemKeyDlg::CDnCharmItemKeyDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback, true )
, m_cInvenType(0)
, m_sInvenIndex(0)
, m_biInvenSerial(0)
, m_nTypeParam(0)
, m_cKeyInvenIndex(0)
, m_nKeyItemID(0)
, m_biKeyItemSerial(0)
, m_pButtonPagePrev(NULL)
, m_pButtonPageNext(NULL)
, m_pStaticPage(NULL)
, m_nCurPage(0)
, m_nMaxPage(0)
, m_nNeedEmptySlotCount(0)
, m_pButtonOK( NULL )
{
}

CDnCharmItemKeyDlg::~CDnCharmItemKeyDlg(void)
{
	SAFE_DELETE_PVEC(m_vecItem);
}

void CDnCharmItemKeyDlg::Initialize( bool bShow )
{
 	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "InvenBoxDlg.ui" ).c_str(), bShow );
}

void CDnCharmItemKeyDlg::InitialUpdate()
{
	char szTemp[32]={0};
	for( int i=0; i<NUM_SLOT; i++ )
	{
		sprintf_s( szTemp, 32, "ID_ITEM%d", i+1 );
		CDnItemSlotButton *pItemSlotButton = GetControl<CDnItemSlotButton>(szTemp);
		pItemSlotButton->SetSlotIndex( i );
		m_vecSlotButton.push_back( pItemSlotButton );

		sprintf_s( szTemp, 32, "ID_SELECT%d", i+1 );
		CEtUIStatic *pSelect = GetControl<CEtUIStatic>(szTemp);
		pSelect->Show( false );
		m_vecSelect.push_back( pSelect );
	}
	m_vecItem.resize(NUM_SLOT);

	m_pButtonPagePrev = GetControl<CEtUIButton>("ID_BT_PRIOR");
	m_pButtonPageNext = GetControl<CEtUIButton>("ID_BT_NEXT");
	m_pStaticPage = GetControl<CEtUIStatic>("ID_TEXT_PAGE");
	m_nCurPage = 0;
	m_pButtonOK = GetControl<CEtUIButton>("ID_BT_OK");

	m_SmartMove.SetControl( m_pButtonOK );

	m_nNeedEmptySlotCount = (int)CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::CharmItemEmptySlotCount );
}

void CDnCharmItemKeyDlg::Show( bool bShow ) 
{ 
	CEtUIDialog::Show( bShow );

	if( bShow )
	{
		m_SmartMove.MoveCursor();
	}
	else
	{
		m_SmartMove.ReturnCursor();
		for( int i = 0; i < NUM_SLOT; ++i )
		{
			SAFE_DELETE( m_vecItem[i] );
			m_vecSlotButton[i]->ResetSlot();
			m_vecSelect[i]->Show( false );
		}
		m_pButtonOK->Enable( false );
	}
}

void CDnCharmItemKeyDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl("ID_BT_OK") )
		{
			if( !CheckInvenEmptySlot() )
			{
				WCHAR wszTemp[256] = {0,};
				swprintf_s( wszTemp, _countof(wszTemp), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7052 ), m_nNeedEmptySlotCount );
				GetInterface().MessageBox( wszTemp, MB_OK );
				if( m_nTypeParam == 2 ) Show( false );
				return;
			}
			RequestUseCharmItem();
			Show( false );
			return;
		}
		else if( IsCmdControl("ID_BT_CANCEL") || IsCmdControl("ID_BT_CLOSE") )
		{
			Show(false);
			return;
		}
		else if( IsCmdControl("ID_BT_PRIOR" ) )
		{
			PrevPage();
			UpdateKeySlot();
			return;
		}
		else if( IsCmdControl("ID_BT_NEXT" ) )
		{
			NextPage();
			UpdateKeySlot();
			return;
		}
		else if( strstr( pControl->GetControlName(), "ID_ITEM" ) )
		{
			CDnSlotButton *pDragButton = ( CDnSlotButton * )drag::GetControl();
			CDnSlotButton *pPressedButton = ( CDnItemSlotButton * )pControl;

			if( pDragButton )
			{
				pDragButton->DisableSplitMode(true);
				drag::ReleaseControl();
			}
			else
			{
				CDnItem *pPressedItem = static_cast<CDnItem *>(pPressedButton->GetItem());
				if( pPressedItem )
				{
					int nIndex = pPressedButton->GetSlotIndex();
					for( int i = 0; i < NUM_SLOT; ++i )
						m_vecSelect[i]->Show( false );
					m_vecSelect[nIndex]->Show( true );
					m_pButtonOK->Enable( true );

					if( m_vecItem[nIndex] )
					{
						m_cKeyInvenIndex = m_vecItem[nIndex]->GetSlotIndex();
						m_nKeyItemID = m_vecItem[nIndex]->GetClassID();
						m_biKeyItemSerial = m_vecItem[nIndex]->GetSerialID();

#ifdef PRE_ADD_GETITEM_WITH_RANDOMVALUE
						m_CharmKeyInfo.cKeyIndexIndex = m_cKeyInvenIndex;
						m_CharmKeyInfo.nKeyItemID = m_nKeyItemID;
						m_CharmKeyInfo.biKeyItemSerial = m_biKeyItemSerial;

#endif 
					}
				}
			}
			return;
		}
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}


bool CDnCharmItemKeyDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
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
					if( m_nCurPage > 0 )
						ProcessCommand( EVENT_BUTTON_CLICKED, false, GetControl<CEtUIButton>("ID_BT_PRIOR"), 0 );
				}
				else if( nScrollAmount < 0 )
				{
					if( m_nCurPage < m_nMaxPage )
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

void CDnCharmItemKeyDlg::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( nID == MESSAGEBOX_OPENBOX )
		{
			if( IsCmdControl( "ID_YES" ) )
			{
				RequestUseCharmItem();
			}
		}
	}
}


#ifdef PRE_ADD_AUTOUNPACK
// AutoUnPackDlg 가 열려있는 경우 bDirectly 를 True 로 설정하여 확인메세지박스가 뜨지않도록 한다.
void CDnCharmItemKeyDlg::SetCharmItem( CDnItem *pCharmItem, bool bDirectly )
{
	if( !pCharmItem ) return;
	if( !CDnItemTask::IsActive() ) return;

	m_cInvenType = pCharmItem->IsCashItem() ? ITEMPOSITION_CASHINVEN : ITEMPOSITION_INVEN;
	m_sInvenIndex = pCharmItem->GetSlotIndex();
	m_biInvenSerial = pCharmItem->GetSerialID();
	m_nTypeParam = pCharmItem->GetTypeParam(1);			 
	m_cKeyInvenIndex = 0;
	m_nKeyItemID = 0;
	m_biKeyItemSerial = 0;
	m_vecKeyItem.clear();

	if( m_nTypeParam == 1 || m_nTypeParam == 2 )

	{
#ifdef PRE_ADD_GETITEM_WITH_RANDOMVALUE
		if( ! CheckInvenNeedExtraSlot( pCharmItem) )
			return;
#endif 

		// 열쇠 필요한 경우
		std::vector<int> vecKeyID;
		pCharmItem->GetCharmItemKeyID( vecKeyID );

		m_vecKeyItem.clear();
		for( int i = 0; i < (int)vecKeyID.size(); ++i )
		{
			GetItemTask().GetCharInventory().ScanItemFromID( vecKeyID[i], &m_vecKeyItem );
			GetItemTask().GetCashInventory().ScanItemFromID( vecKeyID[i], &m_vecKeyItem );
		}
		if( m_vecKeyItem.empty() )
		{
			//Show( false );	// 어차피 모달이라 다른 참아이템 연상태에서 또 열지는 못할테니 Show( false )호출할 필요없다.
			GetInterface().MessageBox( 4560, MB_OK );
			return;
		}

		m_nCurPage = 0;
		m_nMaxPage = ((int)m_vecKeyItem.size()-1) / NUM_SLOT;
		UpdateKeyList();
		Show( true );

		// 제일 처음 창이 열릴땐 선택할 수 있는 키가 있을 경우 자동으로 하나 선택해준다.
		if( m_vecItem.size() && m_vecSlotButton.size() && m_vecItem[0] && m_vecSlotButton[0] )
			ProcessCommand( EVENT_BUTTON_CLICKED, false, m_vecSlotButton[0], 0 );

		// 2번의 경우엔 바로 OK버튼 누르는걸로 해서 열쇠리스트창이 안보이게 한다. 
		// 자동열기창이 열려있는경우에도 수행.
		if( m_nTypeParam == 2 || bDirectly )
		{
			// #53419 다이아몬드 드림드롭 관련 문제.
			// : 열쇠가 자동으로 선택되는 경우에 key정보 가 설정되지않아서 문제발생.
			m_nKeyItemID = m_vecKeyItem[ 0 ]->GetClassID();
			m_biKeyItemSerial = m_vecItem[ 0 ]->GetSerialID();

			ProcessCommand( EVENT_BUTTON_CLICKED, false, m_pButtonOK, 0 );	
		}
	}
	else if( m_nTypeParam == 0 )
	{
		// 열쇠 필요없는 경우
		//Show( false );
		if( !CheckInvenEmptySlot() )
		{
			WCHAR wszTemp[256] = {0,};
			swprintf_s( wszTemp, _countof(wszTemp), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7052 ), m_nNeedEmptySlotCount );
			GetInterface().MessageBox( wszTemp, MB_OK );
			return;
		}

		if( bDirectly )
			RequestUseCharmItem();
#ifdef PRE_ADD_GETITEM_WITH_RANDOMVALUE
		else if( ! bDirectly && CheckInvenNeedExtraSlot( pCharmItem ) )
			GetInterface().MessageBox( 7053, MB_YESNO, MESSAGEBOX_OPENBOX, this, false ); 
#else
		else
			GetInterface().MessageBox( 7053, MB_YESNO, MESSAGEBOX_OPENBOX, this, false ); 
#endif 
	}
}
#else
void CDnCharmItemKeyDlg::SetCharmItem( CDnItem *pCharmItem )
{
	if( !pCharmItem ) return;
	if( !CDnItemTask::IsActive() ) return;

	m_cInvenType = pCharmItem->IsCashItem() ? ITEMPOSITION_CASHINVEN : ITEMPOSITION_INVEN;
	m_sInvenIndex = pCharmItem->GetSlotIndex();
	m_biInvenSerial = pCharmItem->GetSerialID();
	m_nTypeParam = pCharmItem->GetTypeParam(1);
	m_cKeyInvenIndex = 0;
	m_nKeyItemID = 0;
	m_biKeyItemSerial = 0;

	m_vecKeyItem.clear();
	if( m_nTypeParam == 1 || m_nTypeParam == 2 )
	{
		// 열쇠 필요한 경우
		std::vector<int> vecKeyID;
		pCharmItem->GetCharmItemKeyID( vecKeyID );

		m_vecKeyItem.clear();
		for( int i = 0; i < (int)vecKeyID.size(); ++i )
		{
			GetItemTask().GetCharInventory().ScanItemFromID( vecKeyID[i], &m_vecKeyItem );
			GetItemTask().GetCashInventory().ScanItemFromID( vecKeyID[i], &m_vecKeyItem );
		}
		if( m_vecKeyItem.empty() )
		{
			//Show( false );	// 어차피 모달이라 다른 참아이템 연상태에서 또 열지는 못할테니 Show( false )호출할 필요없다.
			GetInterface().MessageBox( 4560, MB_OK );
			return;
		}

		m_nCurPage = 0;
		m_nMaxPage = ((int)m_vecKeyItem.size()-1) / NUM_SLOT;
		UpdateKeyList();
		Show( true );

		// 제일 처음 창이 열릴땐 선택할 수 있는 키가 있을 경우 자동으로 하나 선택해준다.
		if( m_vecItem.size() && m_vecSlotButton.size() && m_vecItem[0] && m_vecSlotButton[0] )
			ProcessCommand( EVENT_BUTTON_CLICKED, false, m_vecSlotButton[0], 0 );

		// 2번의 경우엔 바로 OK버튼 누르는걸로 해서 열쇠리스트창이 안보이게 한다.
		if( m_nTypeParam == 2 )
		{
			// #53419 다이아몬드 드림드롭 관련 문제.
			// : 열쇠가 자동으로 선택되는 경우에 key정보 가 설정되지않아서 문제발생.
			m_nKeyItemID = m_vecKeyItem[ 0 ]->GetClassID();
			m_biKeyItemSerial = m_vecItem[ 0 ]->GetSerialID();

			ProcessCommand( EVENT_BUTTON_CLICKED, false, m_pButtonOK, 0 );
		}
	}
	else if( m_nTypeParam == 0 )
		// 열쇠 필요없는 경우
		//Show( false );

		if( !CheckInvenEmptySlot() )
		{
			WCHAR wszTemp[256] = {0,};
			swprintf_s( wszTemp, _countof(wszTemp), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7052 ), m_nNeedEmptySlotCount );
			GetInterface().MessageBox( wszTemp, MB_OK );
			return;
		}

		GetInterface().MessageBox( 7053, MB_YESNO, MESSAGEBOX_OPENBOX, this, false );
	}
}


#endif

void CDnCharmItemKeyDlg::OnRecvRequestCharmItem( char cInvenType, short sInvenIndex, INT64 biInvenSerial )
{
	if( cInvenType != m_cInvenType || sInvenIndex != m_sInvenIndex || biInvenSerial != m_biInvenSerial )
		return;
	
	GetInterface().ShowCharmItemProgressDialog( true, m_cInvenType, m_sInvenIndex, m_biInvenSerial, m_cKeyInvenIndex, m_nKeyItemID, m_biKeyItemSerial );
}

void CDnCharmItemKeyDlg::UpdateKeyList()
{
	UpdatePage();
	UpdateKeySlot();
}

void CDnCharmItemKeyDlg::UpdatePage()
{
	wchar_t wszPage[10]={0};
	swprintf_s( wszPage, 10, L"%d/%d", m_nCurPage+1, m_nMaxPage+1 );
	m_pStaticPage->SetText( wszPage );

	if( m_nCurPage == m_nMaxPage )
		m_pButtonPageNext->Enable(false);
	else
		m_pButtonPageNext->Enable(true);

	if( m_nCurPage == 0 )
		m_pButtonPagePrev->Enable(false);
	else
		m_pButtonPagePrev->Enable(true);
}

void CDnCharmItemKeyDlg::UpdateKeySlot()
{
	for( int i = 0; i < NUM_SLOT; ++i )
	{
		SAFE_DELETE( m_vecItem[i] );
		m_vecSlotButton[i]->ResetSlot();
		m_vecSelect[i]->Show( false );
	}

	int nOffset = m_nCurPage * NUM_SLOT;

	// 필요한 것들만 보여준다.
	for( int i = nOffset; i < (int)m_vecKeyItem.size(); ++i )
	{
		if( i >= nOffset + NUM_SLOT )
			break;

		TItemInfo itemInfo;
		m_vecKeyItem[i]->GetTItemInfo(itemInfo);
		CDnItem *pNewItem = GetItemTask().CreateItem( itemInfo );
		if( pNewItem->IsCashItem() )
		{
			pNewItem->SetSlotIndex( m_vecKeyItem[i]->GetSlotIndex() );
			pNewItem->SetEternityItem( m_vecKeyItem[i]->IsEternityItem() );
			if( !m_vecKeyItem[i]->IsEternityItem() )
				pNewItem->SetExpireDate( *m_vecKeyItem[i]->GetExpireDate() );
		}
		m_vecItem[i-nOffset] = pNewItem;
		m_vecSlotButton[i-nOffset]->SetItem(pNewItem, CDnSlotButton::ITEM_ORIGINAL_COUNT);
	}
	m_pButtonOK->Enable( false );
}

void CDnCharmItemKeyDlg::PrevPage()
{
	--m_nCurPage;
	if( m_nCurPage < 0 )
		m_nCurPage = 0;

	UpdatePage();
}

void CDnCharmItemKeyDlg::NextPage()
{
	++m_nCurPage;
	if( m_nCurPage > m_nMaxPage )
		m_nCurPage = m_nMaxPage;

	UpdatePage();
}

bool CDnCharmItemKeyDlg::CheckInvenEmptySlot()
{
	// 빈슬롯 확인. 캐시는 어차피 유동적이니 일반 인벤만 확인하면 된다.
	int nCurEmptySlotCount = 0;
	CDnInvenTabDlg *pInvenDlg = (CDnInvenTabDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::INVENTORY_DIALOG );
	if( pInvenDlg ) nCurEmptySlotCount = pInvenDlg->GetEmptySlotCount();
	if( nCurEmptySlotCount < m_nNeedEmptySlotCount )
		return false;
	return true;
}

void CDnCharmItemKeyDlg::RequestUseCharmItem()
{
	// 우편함 열고있는 상태에서 랜덤아이템 여는 루틴때문에 이렇게 한다.
	CDnMailDlg *pMailDlg = (CDnMailDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::MAIL_DIALOG );
	if( pMailDlg && pMailDlg->IsShow() )
		pMailDlg->LockDlgs( true );

	SendRequestCharmItem( m_cInvenType, m_sInvenIndex, m_biInvenSerial, m_cKeyInvenIndex, m_nKeyItemID, m_biKeyItemSerial );
}

#ifdef PRE_ADD_GETITEM_WITH_RANDOMVALUE
bool CDnCharmItemKeyDlg::CheckInvenNeedExtraSlot( CDnItem* pCharmItem )
{
	int nCharmItemKeyID =  0;
	//int nMin = 0;
	int nMax = 0;
		
	// nMax 값만 알아도 되지만 추후 nMin값을 필요로 하게 될때 대비해서 우선 주석처리 해둔다. 
	if( pCharmItem->GetItemType() == ITEMTYPE_CHARMRANDOM )
	{	
		TItemInfo iteminfo;
		pCharmItem->GetTItemInfo( iteminfo );
		int nItemID = iteminfo.Item.nItemID;

		DNTableFileFormat* pSox  = GetDNTable( CDnTableDB::TITEM );
		if( !pSox || !pSox->IsExistItem(nItemID) )
			return false;

		nCharmItemKeyID = pSox->GetFieldFromLablePtr( nItemID, "_TypeParam3" )->GetInteger();

		DNTableFileFormat* pSoxCharm = GetDNTable( CDnTableDB::TCHARMCOUNT );
		if( ! pSoxCharm || ! pSoxCharm->IsExistItem( nCharmItemKeyID ) )
			return false;

		//nMin = pSoxCharm->GetFieldFromLablePtr( nCharmItemKeyID, "_CharmCount_Min" )->GetInteger();
		nMax = pSoxCharm->GetFieldFromLablePtr( nCharmItemKeyID, "_CharmCount_Max" )->GetInteger();

		int nCurEmptySlotCount = 0;
		CDnInvenTabDlg *pInvenDlg = (CDnInvenTabDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::INVENTORY_DIALOG );
		if( pInvenDlg ) nCurEmptySlotCount = pInvenDlg->GetEmptySlotCount();

		if( nCurEmptySlotCount < nMax )
		{
			WCHAR wszTemp[256] = {0,};
			swprintf_s( wszTemp, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1996 ), nMax );
			GetInterface().MessageBox( wszTemp, MB_OK );
			return false;
		}	
	}

	return true;
}
#endif 