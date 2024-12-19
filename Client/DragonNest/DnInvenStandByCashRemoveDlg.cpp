#include "StdAfx.h"

#ifdef PRE_ADD_CASHREMOVE

#include "SyncTimer.h"

#include "DnItemTask.h"
#include "GameOption.h"

#include "DnInterface.h"
#include "DnInvenSlotDlg.h"
#include "DnInven.h"
#include "DnInvenStandByCashRemoveDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnInvenStandByCashRemoveDlg::CDnInvenStandByCashRemoveDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
, m_nCurPage(0)
, m_nMaxPage(0)
, m_pButtonPageGroupPrev( NULL )
, m_pButtonPageGroupNext( NULL )
, m_pButtonPagePrev( NULL )
, m_pButtonPageNext( NULL )
{
	SecureZeroMemory( m_pButtonPage, sizeof(m_pButtonPage) );
}

CDnInvenStandByCashRemoveDlg::~CDnInvenStandByCashRemoveDlg(void)
{
	SAFE_DELETE_PVEC( m_pVecInvenSlotDlg );
}

void CDnInvenStandByCashRemoveDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "InvenCashDlg.ui" ).c_str(), bShow );
}

void CDnInvenStandByCashRemoveDlg::InitialUpdate()
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

	// �ּ��� �Ѱ��� �������� �ʿ��ϴ� �Ѱ��� �̸� �����д�.
	CDnInvenSlotDlg *pInvenSlotDlg = new CDnInvenSlotDlg( UI_TYPE_CHILD, this );
	pInvenSlotDlg->Initialize( true, 0, ITEM_MAX );
	pInvenSlotDlg->MoveDialog( 0.0f, -24.0f/DEFAULT_UI_SCREEN_HEIGHT );
	m_pVecInvenSlotDlg.push_back( pInvenSlotDlg );
}

void CDnInvenStandByCashRemoveDlg::Show( bool bShow )
{
	if( m_bShow == bShow )
		return;

	if( bShow )
	{
		RefreshPageControl();
		RefreshCashInvenSlot();
	}

	CEtUIDialog::Show( bShow );
}

void CDnInvenStandByCashRemoveDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	// EVENT_BUTTON_CLICKED
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
				RefreshCashInvenSlot();
				return;
			}
		}
	}

	// EVENT_BUTTON_RCLICKED
	else if( nCommand == EVENT_BUTTON_RCLICKED )
	{
		if( strstr( pControl->GetControlName(), "ID_ITEM") )
		{
			CDnQuickSlotButton * pSlotBtn = static_cast< CDnQuickSlotButton * >( pControl );
			GetInterface().OpenCashItemRestoreDialog( true, pSlotBtn->GetItem() );
		}
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}


void CDnInvenStandByCashRemoveDlg::Process( float fElapsedTime )
{
	CEtUIDialog::Process( fElapsedTime );

	if( IsShow() )
	{
		// �»�ܿ� ��������ϼ� ����ϱ����� �����ϼ� ���.	
		if( m_nCurPage < (int)m_pVecInvenSlotDlg.size() )
		{
			const std::vector< CDnQuickSlotButton* > & vecSlotBtn = m_pVecInvenSlotDlg[ m_nCurPage ]->GetSlotButtons();
			if( !vecSlotBtn.empty() )
			{	
				CDnQuickSlotButton * pSlotBtn = NULL;
				MIInventoryItem * pMIInvenItem = NULL;
				CDnItem * pItem = NULL;
				int size = (int)vecSlotBtn.size();
				for( int i=0; i<size; ++i )
				{
					pSlotBtn = vecSlotBtn[ i ];
					pMIInvenItem = pSlotBtn->GetItem();
					if( pMIInvenItem )
					{
						pItem = static_cast< CDnItem * >( pMIInvenItem );

						__time64_t exTime = pItem->GetExpireTime();									 // �������ð�.
						__time64_t beginTime = GetItemTask().GetTimeBySN( pItem->GetSerialID() );	 // ���۽ð�.
						if( beginTime != -1 )
						{
							__time64_t pastTime = CSyncTimer::GetInstance().GetCurTime() - beginTime;// ����ð�.
							exTime -= pastTime; // �������ð�.

							if( exTime > 0 )
							{	
								int _hour = int((exTime % 86400) / 3600);
								int _day = (int)(exTime / 86400);
								if( _hour > 0 ) // �ø�.
									_day += 1;
								pSlotBtn->SetExpire( _day );
							}
							else
								pSlotBtn->SetExpire( 0 );
						}
					}
				}//for

			}
		}

	}//if( IsShow() )	

}


bool CDnInvenStandByCashRemoveDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
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

void CDnInvenStandByCashRemoveDlg::SetSlotType( ITEM_SLOT_TYPE slotType )
{
	for( int i = 0; i < (int)m_pVecInvenSlotDlg.size(); ++i )
		m_pVecInvenSlotDlg[i]->SetSlotType( slotType );
}

void CDnInvenStandByCashRemoveDlg::CheckSlotIndex( int nSlotIndex, bool bInsert )
{
	if( bInsert )
	{
		// ���� ������ ǥ���ϱ� ���� �ʿ��� ������ ����
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
				pInvenSlotDlg->SetSlotType( ST_INVENTORY_CASHREMOVE );

				// �ƹ����� �ŷ���ó�� �̵��� ���¿��� ���� �����ؾ��� ���� �־ ù �������� ��ġ�� ���ļӼ��� ���󰡴� ���·� �ϰڴ�.
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
		// ���� �κ��丮�� ������ �������� �����ֱ� ���� �ʿ��� ������ ������ ���ؿ� ������ ���� ����.
		int nLastCashInvenSlotIndex = 0;
		std::map<int,CDnItem*>::reverse_iterator iter = GetItemTask().GetCashRemoveInventory().GetInventoryItemList().rbegin();
		if( iter != GetItemTask().GetCashRemoveInventory().GetInventoryItemList().rend() )
			nLastCashInvenSlotIndex = iter->first;
		int nMaxPage = nLastCashInvenSlotIndex / ITEM_MAX;

		// ���� �ʱ�ȭ�ϴ� ���԰� ������ ���� �ε����� ���ٸ�, �� �������� �ִ� �������� �����ϴ°Ŵ�.
		// (�������� �����Ҷ� ������ ���� �ʱ�ȭ��Ű�� �κ��丮 ����Ʈ���� ���⶧���� �κ��丮 ���뿣 ���� �����ɰ� �����־ �̷��� ó���ϴ� �Ŵ�.)
		if( nSlotIndex == nLastCashInvenSlotIndex )
		{
			// �̷��� ���ʿ��� �ι�° ��ġ�ϴ� �������� �����ε����� ���ؼ�,
			int nLastCashInvenSlotIndex2 = 0;
			std::map<int,CDnItem*>::reverse_iterator iter2 = GetItemTask().GetCashRemoveInventory().GetInventoryItemList().rbegin();
			if( iter2 != GetItemTask().GetCashRemoveInventory().GetInventoryItemList().rend() )
			{
				++iter2;
				if( iter2 != GetItemTask().GetCashRemoveInventory().GetInventoryItemList().rend() )
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

		// �ش� ������ ǥ���ϱ� ���� �ʿ��� ������ ����
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

void CDnInvenStandByCashRemoveDlg::SetItem( MIInventoryItem *pItem )
{
	ASSERT(pItem&&"CDnInvenStandByCashRemoveDlg::SetItem");
	if( !pItem ) return;

	int nSlotIndex = pItem->GetSlotIndex();
	int nSlotPage = nSlotIndex / ITEM_MAX;

	// ���ο� �������� ���ö����� �������� �����Ѵ�.
	CheckSlotIndex( nSlotIndex, true );

	if( (int)m_pVecInvenSlotDlg.size() <= nSlotPage )
	{
		ASSERT(0&&"CDnInvenStandByCashRemoveDlg::SetItem");
		return;
	}

	m_pVecInvenSlotDlg[nSlotPage]->SetItem( pItem );
}

void CDnInvenStandByCashRemoveDlg::ResetSlot( int nSlotIndex )
{
	int nSlotPage = nSlotIndex / ITEM_MAX;

	if( (int)m_pVecInvenSlotDlg.size() <= nSlotPage )
	{
		ASSERT(0&&"CDnInvenStandByCashRemoveDlg::ResetSlot");
		return;
	}

	m_pVecInvenSlotDlg[nSlotPage]->ResetSlot( nSlotIndex );

	// �������� �����ɶ� �������� �����Ѵ�.
	CheckSlotIndex( nSlotIndex, false );
}

void CDnInvenStandByCashRemoveDlg::RefreshPageControl( bool bCheckMaxPage )
{
	if( bCheckMaxPage )
	{
		// �ƽ� ������ �����ϰ�(�������� �Ѱ��� 0, �ΰ��� 1�� �ְ� �ε��� �����̴�.)
		int nLastCashInvenSlotIndex = 0;
		std::map<int,CDnItem*>::reverse_iterator iter = GetItemTask().GetCashRemoveInventory().GetInventoryItemList().rbegin();
		if( iter != GetItemTask().GetCashRemoveInventory().GetInventoryItemList().rend() )
			nLastCashInvenSlotIndex = iter->first;
		m_nMaxPage = nLastCashInvenSlotIndex / ITEM_MAX;
	}

	// ���� �������� �ƽ� �������� �Ѿ��ٸ� ��ȿ�� ���� ������ ����
	if( m_nCurPage > m_nMaxPage )
		m_nCurPage = m_nMaxPage;
	if( m_nCurPage < 0 ) m_nCurPage = 0;

	// ���� �������� ���� ������ �׷��� �����ϰ�(������ �׷��� 0�̸� 1,2,3,4,5 �������� ���̰� 1�̸� 6,7,8,9,10 �������� ���δ�.)
	int nPageGroup = m_nCurPage / NUM_PAGE_PER_GROUP;
	int nMaxPageGroup = m_nMaxPage / NUM_PAGE_PER_GROUP;

	// ������ �׷�� ���� �������� �������� ������ ��Ʈ�� ����.
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

void CDnInvenStandByCashRemoveDlg::RefreshCashInvenSlot()
{
	for( int i = 0; i < (int)m_pVecInvenSlotDlg.size(); ++i )
		m_pVecInvenSlotDlg[i]->Show( i == m_nCurPage );
}

void CDnInvenStandByCashRemoveDlg::NextPage()
{
	if( m_nCurPage < m_nMaxPage ) {
		m_nCurPage++;
		if( m_nCurPage > m_nMaxPage ) m_nCurPage = m_nMaxPage;
		RefreshPageControl();
		RefreshCashInvenSlot();
	}
}

void CDnInvenStandByCashRemoveDlg::PrevPage()
{
	if( m_nCurPage > 0 ) {
		m_nCurPage--;
		if( m_nCurPage < 0 ) m_nCurPage = 0;
		RefreshPageControl();
		RefreshCashInvenSlot();
	}
}

void CDnInvenStandByCashRemoveDlg::NextGroupPage()
{
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

void CDnInvenStandByCashRemoveDlg::PrevGroupPage()
{
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

void CDnInvenStandByCashRemoveDlg::ReleaseNewGain()
{
	if( !CDnItemTask::IsActive() )
		return;

	if( GetItemTask().GetCashRemoveInventory().GetInventoryItemList().empty() )
		return;

	CDnItem *pItem(NULL);
	CDnCashInventory::INVENTORY_MAP_ITER iter = GetItemTask().GetCashRemoveInventory().GetInventoryItemList().begin();
	for( ; iter != GetItemTask().GetCashRemoveInventory().GetInventoryItemList().end(); ++iter )
	{
		pItem = iter->second;
		if( !pItem ) continue;
		pItem->SetNewGain( false );
	}
}

int CDnInvenStandByCashRemoveDlg::GetRegisteredItemCount()
{
	if( !CDnItemTask::IsActive() )
		return 0;

	if( GetItemTask().GetCashRemoveInventory().GetInventoryItemList().empty() )
		return 0;

	int nCount = 0;
	for( int i = 0; i < (int)m_pVecInvenSlotDlg.size(); ++i )
		nCount += m_pVecInvenSlotDlg[i]->GetRegisteredSlotCount();

	return nCount;
}

void CDnInvenStandByCashRemoveDlg::MoveDialog( float fX, float fY ) 
{
	// �ŷ��ҿ��� �κ��丮 �ϰ������� �̵����� �۰� �����ִ� ��ɶ����� �̷��� ������ ó���Ѵ�.
	CEtUIDialog::MoveDialog( fX, fY );
	for( int i = 0; i < (int)m_pVecInvenSlotDlg.size(); ++i )
		m_pVecInvenSlotDlg[i]->MoveDialog( fX, fY );
}

#endif