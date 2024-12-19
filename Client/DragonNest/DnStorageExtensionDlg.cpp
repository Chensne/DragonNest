#include "StdAfx.h"
#include "DnStorageExtensionDlg.h"
#include "DnInterface.h"
#include "DnLocalPlayerActor.h"
#include "DnMainMenuDlg.h"
#include "DnTradeTask.h"
#include "DnCashShopMenuDlg.h"
#include "DnCashShopTask.h"
#include "DnTableDB.h"
#include "DnItemTask.h"
#include "DnCashShopDlg.h"
#include "DnCommonTask.h"
#include "TaskManager.h"
#ifdef PRE_ADD_INSTANT_CASH_BUY
#include "DnInstantCashShopBuyDlg.h"
#include "DnCashShopTask.h"
#include "DnStorageSlotDlg.h"
#endif // PRE_ADD_INSTANT_CASH_BUY



#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnStorageExtensionDlg::CDnStorageExtensionDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
, m_pButtonCash(NULL)
{
}

CDnStorageExtensionDlg::~CDnStorageExtensionDlg(void)
{
}

void CDnStorageExtensionDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "StorageSlotButtonDlg.ui" ).c_str(), bShow );
}

void CDnStorageExtensionDlg::InitialUpdate()
{
	m_pButtonCash = GetControl<CEtUIButton>("ID_BT_SLOTCASH");
}

void CDnStorageExtensionDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
#ifdef PRE_ADD_INSTANT_CASH_BUY
		if( IsCmdControl( "ID_BT_SLOTCASH" ) )
		{
			GetInterface().ShowInstantCashShopBuyDlg( true, INSTANT_BUY_STORAGE, this );
			return;
		}
#else // PRE_ADD_INSTANT_CASH_BUY
		if( IsCmdControl( "ID_BT_SLOTCASH" ) )
		{
			if( !GetInterface().GetCashShopDlg() || !CDnActor::s_hLocalActor )
				return;

			if( GetInterface().GetMainMenuDialog() )
				GetInterface().GetMainMenuDialog()->CloseMenuDialog();

#ifdef PRE_ADD_INVEN_EXTENSION
			int nSN = 0;
			int nBasis = CGlobalWeightIntTable::GetInstance().GetValue( CGlobalWeightIntTable::CheckInvenCountBasis );
			int nUsableWareCount = CDnItemTask::GetInstance().GetStorageInventory().GetUsableSlotCount();
			if( WAREHOUSEMAX -  nUsableWareCount < nBasis ) nSN = CGlobalWeightIntTable::GetInstance().GetValue( CGlobalWeightIntTable::WareCountUnderBasis );
			else nSN = CGlobalWeightIntTable::GetInstance().GetValue( CGlobalWeightIntTable::WareCountOverBasis );
			GetInterface().GetCashShopDlg()->ReserveBuyItemNowOnNextOpened( nSN );
			static_cast<CDnLocalPlayerActor*>(CDnActor::s_hLocalActor.GetPointer())->LockInput( false );
			CDnCashShopTask::GetInstance().RequestCashShopOpen();
			return;
#endif
		}
#endif // PRE_ADD_INSTANT_CASH_BUY
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

#ifdef PRE_ADD_INSTANT_CASH_BUY
void CDnStorageExtensionDlg::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg )
{
	if( nID == INSTANT_CASHSHOP_BUY_DIALOG )
	{
#ifdef PRE_ADD_INVEN_EXTENSION
		CDnStorageSlotDlg* pStorageSlotDlg = dynamic_cast<CDnStorageSlotDlg*>( GetParentDialog() );
		if( pStorageSlotDlg )
			pStorageSlotDlg->CheckShowExtensionDlg( true );
#endif
	}
}
#endif // PRE_ADD_INSTANT_CASH_BUY

void CDnStorageExtensionDlg::Process( float fElapsedTime )
{
	CEtUIDialog::Process( fElapsedTime );

	if( !IsShow() ) return;

	bool bEnable = false;
	if( GetInterface().GetCashShopMenuDlg() && GetInterface().GetCashShopMenuDlg()->IsEnableButton() ) bEnable = true;
	m_pButtonCash->Enable( bEnable );

#ifdef PRE_ADD_INSTANT_CASH_BUY
	int nShowType = GetCashShopTask().GetInstantCashBuyEnableType( INSTANT_BUY_STORAGE );
	if( nShowType == INSTANT_BUY_HIDE )
	{
		m_pButtonCash->Show( false );
	}
	else if( nShowType == INSTANT_BUY_SHOW )
	{
		m_pButtonCash->Show( true );
		m_pButtonCash->Enable( true );
	}
	else if( nShowType == INSTANT_BUY_DISABLE )
	{
		m_pButtonCash->Show( true );
		m_pButtonCash->Enable( false );
	}
#endif // PRE_ADD_INSTANT_CASH_BUY
}