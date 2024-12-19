#include "StdAfx.h"
#include "DnStorageDlg.h"
#include "DnStorageSlotDlg.h"
#include "DnInven.h"
#include "DnInterfaceString.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

CDnStorageDlg::CDnStorageDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
	: CEtUITabDialog( dialogType, pParentDialog, nID, pCallback )
{
}

CDnStorageDlg::~CDnStorageDlg(void)
{
}

void CDnStorageDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "StorageDlg.ui" ).c_str(), bShow );
}

void CDnStorageDlg::InitialUpdate()
{
	CDnStorageSlotDlg *pStoreSlotDlg(NULL);
	CEtUIRadioButton *pTabButton(NULL);

	pTabButton = GetControl<CEtUIRadioButton>("ID_TAB_BASIC01");
	pStoreSlotDlg = new CDnStorageSlotDlg( UI_TYPE_CHILD, this, PAGE_01 );
	pStoreSlotDlg->Initialize( false );
	AddTabDialog( pTabButton, pStoreSlotDlg );
	m_vecTabButton.push_back(pTabButton);

	pTabButton = GetControl<CEtUIRadioButton>("ID_TAB_BASIC02");
	pStoreSlotDlg = new CDnStorageSlotDlg( UI_TYPE_CHILD, this, PAGE_02 );
	pStoreSlotDlg->Initialize( false );
	AddTabDialog( pTabButton, pStoreSlotDlg );
	m_vecTabButton.push_back(pTabButton);

	pTabButton = GetControl<CEtUIRadioButton>("ID_TAB_BASIC03");
	pStoreSlotDlg = new CDnStorageSlotDlg( UI_TYPE_CHILD, this, PAGE_03 );
	pStoreSlotDlg->Initialize( false );
	AddTabDialog( pTabButton, pStoreSlotDlg );
	m_vecTabButton.push_back(pTabButton);

	pTabButton = GetControl<CEtUIRadioButton>("ID_TAB_BASIC04");
	pStoreSlotDlg = new CDnStorageSlotDlg( UI_TYPE_CHILD, this, PAGE_04 );
	pStoreSlotDlg->Initialize( false );
	AddTabDialog( pTabButton, pStoreSlotDlg );
	m_vecTabButton.push_back(pTabButton);

	pTabButton = GetControl<CEtUIRadioButton>("ID_TAB_BASIC05");
	pStoreSlotDlg = new CDnStorageSlotDlg( UI_TYPE_CHILD, this, PAGE_05 );
	pStoreSlotDlg->Initialize( false );
	AddTabDialog( pTabButton, pStoreSlotDlg );
	m_vecTabButton.push_back(pTabButton);

#if defined( PRE_PERIOD_INVENTORY )
	pTabButton = GetControl<CEtUIRadioButton>("ID_TAB_BASIC06");
	pStoreSlotDlg = new CDnStorageSlotDlg( UI_TYPE_CHILD, this, PAGE_PERIOD );
	pStoreSlotDlg->Initialize( false );
	AddTabDialog( pTabButton, pStoreSlotDlg );
	m_vecTabButton.push_back(pTabButton);
#endif	//	#if defined( PRE_PERIOD_INVENTORY )

	SetCheckedTab( m_vecTabButton[PAGE_01]->GetTabID() );
}

void CDnStorageDlg::SetItem( MIInventoryItem *pItem )
{
	ASSERT(pItem&&"CDnStoreDlg::SetItem");
	if( !pItem ) return;

	int nSlotIndex = pItem->GetSlotIndex();
	int nSlotPage = nSlotIndex / ITEM_MAX;

	CDnStorageSlotDlg *pStoreSlotDlg = static_cast<CDnStorageSlotDlg*>(m_groupTabDialog.GetDialog(nSlotPage+1));
	pStoreSlotDlg->SetItem( pItem );
}

void CDnStorageDlg::ResetSlot( int nSlotIndex )
{
	int nSlotPage = nSlotIndex / ITEM_MAX;

	CDnStorageSlotDlg *pStoreSlotDlg = static_cast<CDnStorageSlotDlg*>(m_groupTabDialog.GetDialog(nSlotPage+1));
	pStoreSlotDlg->ResetSlot( nSlotIndex );
}

void CDnStorageDlg::SetUseItemCnt( DWORD dwItemCnt )
{
	CDnStorageSlotDlg *pStoreSlotDlg(NULL);

	DWORD dwPageCnt = dwItemCnt/ITEM_MAX;
	DWORD i = 0;

	for( ; i<dwPageCnt; i++ )
	{
		pStoreSlotDlg = static_cast<CDnStorageSlotDlg*>(m_groupTabDialog.GetDialog(i+1));
		pStoreSlotDlg->SetUseItemCnt(ITEM_MAX);
	}

	for( ; i<(DWORD)m_groupTabDialog.GetDialogCount(); i++ )
	{
#if defined( PRE_PERIOD_INVENTORY )
		if( PAGE_PERIOD == i )
			continue;
#endif	// #if defined( PRE_PERIOD_INVENTORY )

		pStoreSlotDlg = static_cast<CDnStorageSlotDlg*>(m_groupTabDialog.GetDialog(i+1));
		pStoreSlotDlg->SetUseItemCnt(0);
	}

	DWORD dwTemp = dwItemCnt%ITEM_MAX;
	if( dwTemp > 0 )
	{
		pStoreSlotDlg = static_cast<CDnStorageSlotDlg*>(m_groupTabDialog.GetDialog(dwPageCnt+1));
		pStoreSlotDlg->SetUseItemCnt(dwTemp);
	}

#ifdef PRE_ADD_INVEN_EXTENSION
	for( i = 0; i<(int)m_vecTabButton.size(); i++ )
	{
#if defined( PRE_PERIOD_INVENTORY )
		if( PAGE_PERIOD == i )
			continue;
#endif	// #if defined( PRE_PERIOD_INVENTORY )

		SUIElement *pElement = m_vecTabButton[i]->GetElement(0);
		if( !pElement ) continue;

		bool bForceAlpha = false;
		int nOpenCnt = ITEM_MAX;
		if( dwItemCnt <= i*ITEM_MAX )
		{
			bForceAlpha = true;
			nOpenCnt = 0;
		}
		else if( dwItemCnt > i*ITEM_MAX && dwItemCnt < (i+1)*ITEM_MAX )
		{
			nOpenCnt = dwItemCnt%ITEM_MAX;
		}
		else if( dwItemCnt >= (i+1)*ITEM_MAX )
		{
		}

		EtColor Color = pElement->TextureColor.dwColor[UI_STATE_NORMAL];
		Color.a = bForceAlpha ? 0.4f : 1.0f;
		pElement->TextureColor.dwColor[UI_STATE_NORMAL] = Color;
		m_vecTabButton[i]->SetIgnoreAdjustAlpha( bForceAlpha );

		if( nOpenCnt == ITEM_MAX )
			m_vecTabButton[i]->SetTooltipText( L"" );
		else
		{
			WCHAR wszTemp[128] = {0,};
			swprintf_s( wszTemp, _countof(wszTemp), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4868 ), nOpenCnt, ITEM_MAX );
			m_vecTabButton[i]->SetTooltipText( wszTemp );
		}
	}
#endif
}

bool CDnStorageDlg::IsEmptySlot()
{
	CEtUIDialogGroup::DIALOG_VEC &vecDialog = m_groupTabDialog.GetDialogList();
	for( int i=0; i<(int)vecDialog.size(); ++i )
	{
		CEtUIDialogGroup::ID_DIALOG_PAIR &id_dlg_pair = vecDialog[i];
		if( ((CDnStorageSlotDlg*)id_dlg_pair.second)->IsEmptySlot() )
		{
			return true;
		}
	}

	return false;
}

int CDnStorageDlg::GetEmptySlot()
{
	int nSlotIndex(-1);

	CEtUIDialogGroup::DIALOG_VEC &vecDialog = m_groupTabDialog.GetDialogList();
	for( int i=0; i<(int)vecDialog.size(); ++i )
	{
		CEtUIDialogGroup::ID_DIALOG_PAIR &id_dlg_pair = vecDialog[i];
		nSlotIndex = ((CDnStorageSlotDlg*)id_dlg_pair.second)->GetEmptySlot();
		if( nSlotIndex != -1 )
		{
			return nSlotIndex;
		}
	}

	return nSlotIndex;
}

bool CDnStorageDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsShow() )
	{
		return false;
	}

	switch( uMsg )
	{
	case WM_MOUSEWHEEL:
		{
			CEtUIDialog *pShowDialog = m_groupTabDialog.GetShowDialog();
			if( !pShowDialog ) return false;
			if( !pShowDialog->IsShow() ) return false;

			if( IsMouseInDlg() || pShowDialog->IsMouseInDlg() )
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

	return CEtUITabDialog::MsgProc( hWnd, uMsg, wParam, lParam );
}

void CDnStorageDlg::PrevPage()
{
	const CEtUIRadioButton *pCurrRadioButton = GetCurrentTabControl();
	CEtUIRadioButton *pPrevRadioButton = (CEtUIRadioButton *)pCurrRadioButton;
	for( int i = 0; i < (int)m_vecTabButton.size(); ++i )
	{
		if( m_vecTabButton[i] == pCurrRadioButton )
			break;
		pPrevRadioButton = m_vecTabButton[i];
	}
	if( pPrevRadioButton != pCurrRadioButton )
		SetCheckedTab( pPrevRadioButton->GetTabID() );
}

void CDnStorageDlg::NextPage()
{
	const CEtUIRadioButton *pCurrRadioButton = GetCurrentTabControl();
	CEtUIRadioButton *pNextRadioButton = (CEtUIRadioButton *)pCurrRadioButton;
	bool bNext = false;
	for( int i = 0; i < (int)m_vecTabButton.size(); ++i )
	{
		if( bNext )
		{
			pNextRadioButton = m_vecTabButton[i];
			break;
		}
		if( m_vecTabButton[i] == pCurrRadioButton )
			bNext = true;
	}
	if( pNextRadioButton != pCurrRadioButton )
		SetCheckedTab( pNextRadioButton->GetTabID() );
}

#if defined( PRE_PERIOD_INVENTORY )
void CDnStorageDlg::EnablePeriodStorage( const bool bEnable, const __time64_t tTime )
{
	m_vecTabButton[PAGE_PERIOD]->Enable( bEnable );
	SetPeriodStorageTooltip( bEnable, tTime );

	CEtUIDialogGroup::DIALOG_VEC &vecDialog = m_groupTabDialog.GetDialogList();
	for( int i=0; i<(int)vecDialog.size(); ++i )
	{
		CEtUIDialogGroup::ID_DIALOG_PAIR &id_dlg_pair = vecDialog[i];

		int nDialogID = id_dlg_pair.second->GetDialogID();
		if( PAGE_PERIOD != nDialogID )
			continue;

		((CDnStorageSlotDlg*)id_dlg_pair.second)->SetUseItemCnt( PERIOD_ITEM_MAX );
	}
}

void CDnStorageDlg::SetPeriodStorageTooltip( const bool bEnable, const __time64_t tTime )
{
	if( true == bEnable )
	{
		std::wstring wszString;
#ifdef PRE_ADD_EQUIPLOCK
		DN_INTERFACE::STRING::GetTimeText2( wszString, tTime, 8364 );	// UISTRING : 기간제 확장 슬롯 입니다.\n#y(%d년 %d월 %d일.........
#endif	// #ifdef PRE_ADD_EQUIPLOCK

		m_vecTabButton[PAGE_PERIOD]->SetTooltipText( wszString.c_str() );
	}
	else
		m_vecTabButton[PAGE_PERIOD]->SetTooltipText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8365 ) );	// UISTRING : 기간제 확장 슬롯의 사용기한이 만료되어, 아이템을 이동........
}

bool CDnStorageDlg::IsPeriodStorageEnable()
{
	return m_vecTabButton[PAGE_PERIOD]->GetTabID() == GetCurrentTabID();
}
#endif	//	#if defined( PRE_PERIOD_INVENTORY )