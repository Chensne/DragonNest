#include "StdAfx.h"
#include "DnGuildStorageDlg.h"
#include "DnGuildStorageSlotDlg.h"
#include "DnInven.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

CDnGuildStorageDlg::CDnGuildStorageDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
	: CEtUITabDialog( dialogType, pParentDialog, nID, pCallback )
{
}

CDnGuildStorageDlg::~CDnGuildStorageDlg(void)
{
}

void CDnGuildStorageDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "GuildStorageDlg.ui" ).c_str(), bShow );
}

void CDnGuildStorageDlg::InitialUpdate()
{
	CDnGuildStorageSlotDlg *pStoreSlotDlg(NULL);
	CEtUIRadioButton *pTabButton(NULL);

	pTabButton = GetControl<CEtUIRadioButton>("ID_TAB_BASIC01");
	pStoreSlotDlg = new CDnGuildStorageSlotDlg( UI_TYPE_CHILD, this, PAGE_01 );
	pStoreSlotDlg->Initialize( false );
	AddTabDialog( pTabButton, pStoreSlotDlg );
	m_vecTabButton.push_back(pTabButton);

	pTabButton = GetControl<CEtUIRadioButton>("ID_TAB_BASIC02");
	pStoreSlotDlg = new CDnGuildStorageSlotDlg( UI_TYPE_CHILD, this, PAGE_02 );
	pStoreSlotDlg->Initialize( false );
	AddTabDialog( pTabButton, pStoreSlotDlg );
	m_vecTabButton.push_back(pTabButton);

	pTabButton = GetControl<CEtUIRadioButton>("ID_TAB_BASIC03");
	pStoreSlotDlg = new CDnGuildStorageSlotDlg( UI_TYPE_CHILD, this, PAGE_03 );
	pStoreSlotDlg->Initialize( false );
	AddTabDialog( pTabButton, pStoreSlotDlg );
	m_vecTabButton.push_back(pTabButton);

	pTabButton = GetControl<CEtUIRadioButton>("ID_TAB_BASIC04");
	pStoreSlotDlg = new CDnGuildStorageSlotDlg( UI_TYPE_CHILD, this, PAGE_04 );
	pStoreSlotDlg->Initialize( false );
	AddTabDialog( pTabButton, pStoreSlotDlg );
	m_vecTabButton.push_back(pTabButton);

	pTabButton = GetControl<CEtUIRadioButton>("ID_TAB_BASIC05");
	pStoreSlotDlg = new CDnGuildStorageSlotDlg( UI_TYPE_CHILD, this, PAGE_05 );
	pStoreSlotDlg->Initialize( false );
	AddTabDialog( pTabButton, pStoreSlotDlg );
	m_vecTabButton.push_back(pTabButton);

	SetCheckedTab( m_vecTabButton[PAGE_01]->GetTabID() );
}

void CDnGuildStorageDlg::SetItem( MIInventoryItem *pItem )
{
	ASSERT(pItem&&"CDnGuildStorageDlg::SetItem");
	if( !pItem ) return;

	int nSlotIndex = pItem->GetSlotIndex();
	int nSlotPage = nSlotIndex / ITEM_MAX;

	CDnGuildStorageSlotDlg *pStoreSlotDlg = static_cast<CDnGuildStorageSlotDlg*>(m_groupTabDialog.GetDialog(nSlotPage+1));
	pStoreSlotDlg->SetItem( pItem );
}

void CDnGuildStorageDlg::ResetSlot( int nSlotIndex )
{
	int nSlotPage = nSlotIndex / ITEM_MAX;

	CDnGuildStorageSlotDlg *pStoreSlotDlg = static_cast<CDnGuildStorageSlotDlg*>(m_groupTabDialog.GetDialog(nSlotPage+1));
	pStoreSlotDlg->ResetSlot( nSlotIndex );
}

void CDnGuildStorageDlg::SetUseItemCnt( DWORD dwItemCnt )
{
	CDnGuildStorageSlotDlg *pStoreSlotDlg(NULL);

	DWORD dwPageCnt = dwItemCnt/ITEM_MAX;
	DWORD i = 0;

	for( ; i<dwPageCnt; i++ )
	{
		pStoreSlotDlg = static_cast<CDnGuildStorageSlotDlg*>(m_groupTabDialog.GetDialog(i+1));
		pStoreSlotDlg->SetUseItemCnt(ITEM_MAX);
	}

	for( ; i<(DWORD)m_groupTabDialog.GetDialogCount(); i++ )
	{
		pStoreSlotDlg = static_cast<CDnGuildStorageSlotDlg*>(m_groupTabDialog.GetDialog(i+1));
		pStoreSlotDlg->SetUseItemCnt(0);
	}

	DWORD dwTemp = dwItemCnt%ITEM_MAX;
	if( dwTemp > 0 )
	{
		pStoreSlotDlg = static_cast<CDnGuildStorageSlotDlg*>(m_groupTabDialog.GetDialog(dwPageCnt+1));
		pStoreSlotDlg->SetUseItemCnt(dwTemp);
	}
}

bool CDnGuildStorageDlg::IsEmptySlot()
{
	CEtUIDialogGroup::DIALOG_VEC &vecDialog = m_groupTabDialog.GetDialogList();
	for( int i=0; i<(int)vecDialog.size(); ++i )
	{
		CEtUIDialogGroup::ID_DIALOG_PAIR &id_dlg_pair = vecDialog[i];
		if( ((CDnGuildStorageSlotDlg*)id_dlg_pair.second)->IsEmptySlot() )
		{
			return true;
		}
	}

	return false;
}

int CDnGuildStorageDlg::GetEmptySlot()
{
	int nSlotIndex(-1);

	CEtUIDialogGroup::DIALOG_VEC &vecDialog = m_groupTabDialog.GetDialogList();
	for( int i=0; i<(int)vecDialog.size(); ++i )
	{
		CEtUIDialogGroup::ID_DIALOG_PAIR &id_dlg_pair = vecDialog[i];
		nSlotIndex = ((CDnGuildStorageSlotDlg*)id_dlg_pair.second)->GetEmptySlot();
		if( nSlotIndex != -1 )
		{
			return nSlotIndex;
		}
	}

	return nSlotIndex;
}


bool CDnGuildStorageDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
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

void CDnGuildStorageDlg::PrevPage()
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

void CDnGuildStorageDlg::NextPage()
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