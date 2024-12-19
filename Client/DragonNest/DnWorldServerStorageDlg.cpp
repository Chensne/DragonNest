#include "StdAfx.h"

#ifdef PRE_ADD_SERVER_WAREHOUSE

#include "DnWorldServerStorageDlg.h"
#include "DnWorldServerStorageSlotDlg.h"
#include "DnInven.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

CDnWorldServerStorageDlg::CDnWorldServerStorageDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback)
	: CEtUITabDialog(dialogType, pParentDialog, nID, pCallback)
{
}

CDnWorldServerStorageDlg::~CDnWorldServerStorageDlg(void)
{
}

void CDnWorldServerStorageDlg::Initialize(bool bShow)
{
	CEtUIDialog::Initialize(CEtResourceMng::GetInstance().GetFullName( "StorageDlg.ui" ).c_str(), bShow);
}

void CDnWorldServerStorageDlg::InitialUpdate()
{
	CDnWorldServerStorageSlotDlg *pStoreSlotDlg(NULL);
	CEtUIRadioButton *pTabButton(NULL);

	pTabButton = GetControl<CEtUIRadioButton>("ID_TAB_BASIC01");
	pStoreSlotDlg = new CDnWorldServerStorageSlotDlg( UI_TYPE_CHILD, this, PAGE_01 );
	pStoreSlotDlg->Initialize( false );
	AddTabDialog( pTabButton, pStoreSlotDlg );
	m_vecTabButton.push_back(pTabButton);

	pTabButton = GetControl<CEtUIRadioButton>("ID_TAB_BASIC02");
	pStoreSlotDlg = new CDnWorldServerStorageSlotDlg( UI_TYPE_CHILD, this, PAGE_02 );
	pStoreSlotDlg->Initialize( false );
	AddTabDialog( pTabButton, pStoreSlotDlg );
	m_vecTabButton.push_back(pTabButton);

	pTabButton = GetControl<CEtUIRadioButton>("ID_TAB_BASIC03");
	pStoreSlotDlg = new CDnWorldServerStorageSlotDlg( UI_TYPE_CHILD, this, PAGE_03 );
	pStoreSlotDlg->Initialize( false );
	AddTabDialog( pTabButton, pStoreSlotDlg );
	m_vecTabButton.push_back(pTabButton);

	pTabButton = GetControl<CEtUIRadioButton>("ID_TAB_BASIC04");
	pStoreSlotDlg = new CDnWorldServerStorageSlotDlg( UI_TYPE_CHILD, this, PAGE_04 );
	pStoreSlotDlg->Initialize( false );
	AddTabDialog( pTabButton, pStoreSlotDlg );
	m_vecTabButton.push_back(pTabButton);

	pTabButton = GetControl<CEtUIRadioButton>("ID_TAB_BASIC05");
	pStoreSlotDlg = new CDnWorldServerStorageSlotDlg( UI_TYPE_CHILD, this, PAGE_05 );
	pStoreSlotDlg->Initialize( false );
	AddTabDialog( pTabButton, pStoreSlotDlg );
	m_vecTabButton.push_back(pTabButton);

#if defined( PRE_PERIOD_INVENTORY )
	pTabButton = GetControl<CEtUIRadioButton>("ID_TAB_BASIC06");
	pTabButton->Enable( false );
#endif	// #if defined( PRE_PERIOD_INVENTORY )

	SetCheckedTab( m_vecTabButton[PAGE_01]->GetTabID() );
}

void CDnWorldServerStorageDlg::SetItem( MIInventoryItem *pItem )
{
	ASSERT(pItem&&"CDnStoreDlg::SetItem");
	if( !pItem ) return;

	int nSlotIndex = pItem->GetSlotIndex();
	int nSlotPage = nSlotIndex / ITEM_MAX;

	CDnWorldServerStorageSlotDlg *pStoreSlotDlg = static_cast<CDnWorldServerStorageSlotDlg*>(m_groupTabDialog.GetDialog(nSlotPage+1));
	pStoreSlotDlg->SetItem( pItem );
}

void CDnWorldServerStorageDlg::ResetSlot( int nSlotIndex )
{
	int nSlotPage = nSlotIndex / ITEM_MAX;

	CDnWorldServerStorageSlotDlg *pStoreSlotDlg = static_cast<CDnWorldServerStorageSlotDlg*>(m_groupTabDialog.GetDialog(nSlotPage+1));
	pStoreSlotDlg->ResetSlot( nSlotIndex );
}

void CDnWorldServerStorageDlg::SetUseItemCnt( DWORD dwItemCnt )
{
	CDnWorldServerStorageSlotDlg *pStoreSlotDlg(NULL);

	DWORD dwPageCnt = dwItemCnt/ITEM_MAX;
	DWORD i = 0;

	for( ; i<dwPageCnt; i++ )
	{
		pStoreSlotDlg = static_cast<CDnWorldServerStorageSlotDlg*>(m_groupTabDialog.GetDialog(i+1));
		pStoreSlotDlg->SetUseItemCnt(ITEM_MAX);
	}

	for( ; i<(DWORD)m_groupTabDialog.GetDialogCount(); i++ )
	{
		pStoreSlotDlg = static_cast<CDnWorldServerStorageSlotDlg*>(m_groupTabDialog.GetDialog(i+1));
		pStoreSlotDlg->SetUseItemCnt(0);
	}

	DWORD dwTemp = dwItemCnt%ITEM_MAX;
	if( dwTemp > 0 )
	{
		pStoreSlotDlg = static_cast<CDnWorldServerStorageSlotDlg*>(m_groupTabDialog.GetDialog(dwPageCnt+1));
		pStoreSlotDlg->SetUseItemCnt(dwTemp);
	}
}

bool CDnWorldServerStorageDlg::IsEmptySlot()
{
	CEtUIDialogGroup::DIALOG_VEC &vecDialog = m_groupTabDialog.GetDialogList();
	for( int i=0; i<(int)vecDialog.size(); ++i )
	{
		CEtUIDialogGroup::ID_DIALOG_PAIR &id_dlg_pair = vecDialog[i];
		if( ((CDnWorldServerStorageSlotDlg*)id_dlg_pair.second)->IsEmptySlot() )
		{
			return true;
		}
	}

	return false;
}

int CDnWorldServerStorageDlg::GetEmptySlot()
{
	int nSlotIndex(-1);

	CEtUIDialogGroup::DIALOG_VEC &vecDialog = m_groupTabDialog.GetDialogList();
	for( int i=0; i<(int)vecDialog.size(); ++i )
	{
		CEtUIDialogGroup::ID_DIALOG_PAIR &id_dlg_pair = vecDialog[i];
		nSlotIndex = ((CDnWorldServerStorageSlotDlg*)id_dlg_pair.second)->GetEmptySlot();
		if( nSlotIndex != -1 )
		{
			return nSlotIndex;
		}
	}

	return nSlotIndex;
}

void CDnWorldServerStorageDlg::SetSlotType(ITEM_SLOT_TYPE type)
{
	CEtUIDialogGroup::DIALOG_VEC &vecDialog = m_groupTabDialog.GetDialogList();
	for( int i=0; i<(int)vecDialog.size(); ++i )
	{
		CEtUIDialogGroup::ID_DIALOG_PAIR &id_dlg_pair = vecDialog[i];
		CEtUIDialog* pDlg = id_dlg_pair.second;
		if (pDlg)
		{
			CDnWorldServerStorageSlotDlg* pWWSDlg = static_cast<CDnWorldServerStorageSlotDlg*>(pDlg);
			if (pWWSDlg)
				pWWSDlg->SetSlotType(type);
		}
	}

}

bool CDnWorldServerStorageDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
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

void CDnWorldServerStorageDlg::PrevPage()
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

void CDnWorldServerStorageDlg::NextPage()
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

#endif // PRE_ADD_ACCOUNT_STORAGE