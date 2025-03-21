#include "StdAfx.h"
#include "DnInvenPetDlg.h"
#include "DnInvenSlotDlg.h"
#include "DnInven.h"
#include "DnItemTask.h"
#include "GameOption.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnInvenPetDlg::CDnInvenPetDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
, m_nCurPage( 0 )
, m_nMaxPage( 0 )
, m_pButtonPageGroupPrev( NULL )
, m_pButtonPageGroupNext( NULL )
, m_pButtonPagePrev( NULL )
, m_pButtonPageNext( NULL )
{
	memset( m_pButtonPage, 0, sizeof(m_pButtonPage) );
}

CDnInvenPetDlg::~CDnInvenPetDlg(void)
{
	SAFE_DELETE_PVEC( m_pVecInvenSlotDlg );
}

void CDnInvenPetDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "InvenCashDlg.ui" ).c_str(), bShow );
}

void CDnInvenPetDlg::InitialUpdate()
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
	pInvenSlotDlg->Initialize( true, 0, ITEM_MAX );
	pInvenSlotDlg->MoveDialog( 0.0f, -24.0f/DEFAULT_UI_SCREEN_HEIGHT );
	m_pVecInvenSlotDlg.push_back( pInvenSlotDlg );
}

void CDnInvenPetDlg::Show( bool bShow )
{
	if( m_bShow == bShow )
		return;

	if( bShow )
	{
		RefreshPageControl();
		RefreshPetInvenSlot();
	}

	CEtUIDialog::Show( bShow );
}

void CDnInvenPetDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
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
				m_nCurPage = nPage-1;
				RefreshPageControl();
				RefreshPetInvenSlot();
				return;
			}
		}
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

bool CDnInvenPetDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsShow() )
	{
		return false;
	}

	switch( uMsg )
	{
	case WM_MOUSEWHEEL:
		{
			if( (int)m_pVecInvenSlotDlg.size() < m_nCurPage )
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
		}
		break;
	}

	return CEtUIDialog::MsgProc( hWnd, uMsg, wParam, lParam );
}

void CDnInvenPetDlg::SetSlotType( ITEM_SLOT_TYPE slotType )
{
	for( int i = 0; i < (int)m_pVecInvenSlotDlg.size(); ++i )
		m_pVecInvenSlotDlg[i]->SetSlotType( slotType );
}

void CDnInvenPetDlg::CheckSlotIndex( int nSlotIndex, bool bInsert )
{
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
				pInvenSlotDlg->Initialize( false, (int)m_pVecInvenSlotDlg.size(), ITEM_MAX );
				pInvenSlotDlg->SetSlotType( ST_INVENTORY_VEHICLE );

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
					pInvenSlotDlg->MoveDialog( 0.0f, -24.0f/DEFAULT_UI_SCREEN_HEIGHT );
				}
				m_pVecInvenSlotDlg.push_back( pInvenSlotDlg );
			}
			CGameOption::GetInstance().m_bSmartMove = bOrigSmartMove;
			RefreshPageControl( true );
		}
	}
	else
	{
		// 현재 인벤토리의 마지막 아이템을 보여주기 위해 필요한 페이지 개수를 구해와 페이지 개수 설정.
		int nLastPetInvenSlotIndex = 0;
		std::map<int,CDnItem*>::reverse_iterator iter = GetItemTask().GetPetInventory().GetInventoryItemList().rbegin();
		if( iter != GetItemTask().GetPetInventory().GetInventoryItemList().rend() )
			nLastPetInvenSlotIndex = iter->first;
		int nMaxPage = nLastPetInvenSlotIndex / ITEM_MAX;

		// 현재 초기화하는 슬롯과 마지막 슬롯 인덱스가 같다면, 맨 마지막에 있는 아이템을 삭제하는거다.
		// (아이템을 삭제할때 슬롯을 먼저 초기화시키고 인벤토리 리스트에서 빼기때문에 인벤토리 내용엔 아직 삭제될게 남아있어서 이렇게 처리하는 거다.)
		if( nSlotIndex == nLastPetInvenSlotIndex )
		{
			// 이럴땐 뒤쪽에서 두번째 위치하는 아이템의 슬롯인덱스를 구해서,
			int nLastPetInvenSlotIndex2 = 0;
			std::map<int,CDnItem*>::reverse_iterator iter2 = GetItemTask().GetPetInventory().GetInventoryItemList().rbegin();
			if( iter2 != GetItemTask().GetPetInventory().GetInventoryItemList().rend() )
			{
				++iter2;
				if( iter2 != GetItemTask().GetPetInventory().GetInventoryItemList().rend() )
				{
					nLastPetInvenSlotIndex2 = iter2->first;
					int nMaxPage2 = nLastPetInvenSlotIndex2 / ITEM_MAX;
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
			RefreshPetInvenSlot();
		}
	}
}

void CDnInvenPetDlg::SetItem( MIInventoryItem *pItem )
{
	ASSERT(pItem&&"CDnInvenPetDlg::SetItem");
	if( !pItem ) return;

	int nSlotIndex = pItem->GetSlotIndex();
	int nSlotPage = nSlotIndex / ITEM_MAX;

	// 새로운 아이템이 들어올때마다 페이지를 갱신한다.
	CheckSlotIndex( nSlotIndex, true );

	if( (int)m_pVecInvenSlotDlg.size() <= nSlotPage )
	{
		ASSERT(0&&"CDnInvenPetDlg::SetItem");
		return;
	}

	m_pVecInvenSlotDlg[nSlotPage]->SetItem( pItem );
}

void CDnInvenPetDlg::ResetSlot( int nSlotIndex )
{
	int nSlotPage = nSlotIndex / ITEM_MAX;

	if( (int)m_pVecInvenSlotDlg.size() <= nSlotPage )
	{
		ASSERT(0&&"CDnInvenPetDlg::ResetSlot");
		return;
	}

	m_pVecInvenSlotDlg[nSlotPage]->ResetSlot( nSlotIndex );

	// 아이템이 삭제될때 페이지를 갱신한다.
	CheckSlotIndex( nSlotIndex, false );
}

void CDnInvenPetDlg::RefreshPageControl( bool bCheckMaxPage )
{
	if( bCheckMaxPage )
	{
		// 맥스 페이지 설정하고(페이지가 한개면 0, 두개면 1인 최고 인덱스 저장이다.)
		int nLastPetInvenSlotIndex = 0;
		std::map<int,CDnItem*>::reverse_iterator iter = GetItemTask().GetPetInventory().GetInventoryItemList().rbegin();

		if( iter != GetItemTask().GetPetInventory().GetInventoryItemList().rend() )
			nLastPetInvenSlotIndex = iter->first;
		m_nMaxPage = nLastPetInvenSlotIndex / ITEM_MAX;
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
}

void CDnInvenPetDlg::RefreshPetInvenSlot()
{
	for( int i = 0; i < (int)m_pVecInvenSlotDlg.size(); ++i )
		m_pVecInvenSlotDlg[i]->Show( i == m_nCurPage );
}

void CDnInvenPetDlg::NextPage()
{
	if( m_nCurPage < m_nMaxPage ) {
		m_nCurPage++;
		if( m_nCurPage > m_nMaxPage ) m_nCurPage = m_nMaxPage;
		RefreshPageControl();
		RefreshPetInvenSlot();
	}
}

void CDnInvenPetDlg::PrevPage()
{
	if( m_nCurPage > 0 ) {
		m_nCurPage--;
		if( m_nCurPage < 0 ) m_nCurPage = 0;
		RefreshPageControl();
		RefreshPetInvenSlot();
	}
}

void CDnInvenPetDlg::NextGroupPage()
{
	int nPageGroup = m_nCurPage / NUM_PAGE_PER_GROUP;
	int nMaxPageGroup = m_nMaxPage / NUM_PAGE_PER_GROUP;

	if( nPageGroup < nMaxPageGroup ) {
		nPageGroup++;
		if( nPageGroup > nMaxPageGroup ) nPageGroup = nMaxPageGroup;
		m_nCurPage = nPageGroup * NUM_PAGE_PER_GROUP;
		RefreshPageControl();
		RefreshPetInvenSlot();
	}
}

void CDnInvenPetDlg::PrevGroupPage()
{
	int nPageGroup = m_nCurPage / NUM_PAGE_PER_GROUP;
	int nMaxPageGroup = m_nMaxPage / NUM_PAGE_PER_GROUP;

	if( nPageGroup > 0 ) {
		nPageGroup--;
		if( nPageGroup < 0 ) nPageGroup = 0;
		m_nCurPage = nPageGroup * NUM_PAGE_PER_GROUP;
		RefreshPageControl();
		RefreshPetInvenSlot();
	}
}

void CDnInvenPetDlg::ReleaseNewGain()
{
	if( !CDnItemTask::IsActive() )
		return;

	if( GetItemTask().GetPetInventory().GetInventoryItemList().empty() )
		return;

	CDnItem *pItem(NULL);
	CDnPetInventory::INVENTORY_MAP_ITER iter = GetItemTask().GetPetInventory().GetInventoryItemList().begin();
	for( ; iter != GetItemTask().GetPetInventory().GetInventoryItemList().end(); ++iter )
	{
		pItem = iter->second;
		if( !pItem ) continue;
		pItem->SetNewGain( false );
	}
}

int CDnInvenPetDlg::GetRegisteredItemCount()
{
	if( !CDnItemTask::IsActive() )
		return 0;

	if( GetItemTask().GetPetInventory().GetInventoryItemList().empty() )
		return 0;

	int nCount = 0;
	for( int i = 0; i < (int)m_pVecInvenSlotDlg.size(); ++i )
		nCount += m_pVecInvenSlotDlg[i]->GetRegisteredSlotCount();

	return nCount;
}

void CDnInvenPetDlg::MoveDialog( float fX, float fY ) 
{
	// 거래소에서 인벤토리 일괄적으로 이동시켜 작게 보여주는 기능때문에 이렇게 별도로 처리한다.
	CEtUIDialog::MoveDialog( fX, fY );
	for( int i = 0; i < (int)m_pVecInvenSlotDlg.size(); ++i )
		m_pVecInvenSlotDlg[i]->MoveDialog( fX, fY );
}

