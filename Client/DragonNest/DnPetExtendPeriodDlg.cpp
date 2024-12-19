#include "StdAfx.h"
#include "DnPetExtendPeriodDlg.h"
#include "DnItem.h"
#include "DnInvenTabDlg.h"
#include "DnInterface.h"
#include "DnMainMenuDlg.h"
#include "DnVehicleTask.h"
#include "DnItemTask.h"
#include "SyncTimer.h"
#include "DnPetTask.h"
#ifdef PRE_ADD_PET_EXTEND_PERIOD
#include "DnBuyPetExtendPeriodDlg.h"
#endif // PRE_ADD_PET_EXTEND_PERIOD


#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnPetExtendPeriodDlg::CDnPetExtendPeriodDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback, true )
, m_pItemSlot( NULL )
, m_pButtonOK( NULL )
, m_pButtonSlot( NULL )
, m_pStatic( NULL )
, m_pPetItem( NULL )
, m_pExtendPeriodItem( NULL )
, m_nExtendPeriod( 0 )
#ifdef PRE_ADD_PET_EXTEND_PERIOD
, m_pDnBuyConfirmPetExtendPeriodDlg( NULL )
#endif // PRE_ADD_PET_EXTEND_PERIOD
{
}

CDnPetExtendPeriodDlg::~CDnPetExtendPeriodDlg()
{
	SAFE_DELETE( m_pPetItem );
#ifdef PRE_ADD_PET_EXTEND_PERIOD
	SAFE_DELETE( m_pDnBuyConfirmPetExtendPeriodDlg );
#endif // PRE_ADD_PET_EXTEND_PERIOD
}

void CDnPetExtendPeriodDlg::InitialUpdate()
{
	CEtUIDialog::InitialUpdate();

	m_pItemSlot = GetControl<CDnItemSlotButton>( "ID_ITEM" );
	m_pButtonOK = GetControl<CEtUIButton>( "ID_OK" );
	m_pStatic = GetControl<CEtUIStatic>( "ID_STATIC4" );
#ifdef PRE_ADD_PET_EXTEND_PERIOD
	m_pDnBuyConfirmPetExtendPeriodDlg = new CDnBuyConfirmPetExtendPeriodDlg( UI_TYPE_MODAL );
	m_pDnBuyConfirmPetExtendPeriodDlg->Initialize( false );
#endif // PRE_ADD_PET_EXTEND_PERIOD
}

void CDnPetExtendPeriodDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "InvenPetTimeDlg.ui" ).c_str(), bShow );
}

void CDnPetExtendPeriodDlg::Show( bool bShow )
{
	if( m_bShow == bShow )
		return;

	if( bShow )
	{
		CDnInvenTabDlg *pInvenDlg = (CDnInvenTabDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::INVENTORY_DIALOG );
		if( pInvenDlg && pInvenDlg->IsShow() && !drag::IsValid() )
			pInvenDlg->ShowTab( ST_INVENTORY_VEHICLE );

		SetRenderPriority( this, true );

		if( m_pExtendPeriodItem )
		{
			DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TITEM );
			if( !pSox ) return;
			if( !pSox->IsExistItem( m_pExtendPeriodItem->GetClassID() ) ) return;

			m_nExtendPeriod = pSox->GetFieldFromLablePtr( m_pExtendPeriodItem->GetClassID(), "_TypeParam1" )->GetInteger();
			WCHAR wszString[256] = {0,};
			swprintf_s( wszString, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 9225 ), m_nExtendPeriod );
			m_pStatic->SetText( wszString );
		}
		CEtUIButton* pButton = GetControl<CEtUIButton>( "ID_CANCEL" );
		if( pButton )
			pButton->Enable( true );
		pButton = GetControl<CEtUIButton>( "ID_CLOSE" );
		if( pButton )
			pButton->Enable( true );
	}
	else
	{
		m_pItemSlot->ResetSlot();
		m_pButtonOK->Enable( false );

		if( m_pButtonSlot )
		{
			m_pButtonSlot->SetRegist( false );
			m_pButtonSlot = NULL;
		}
#ifdef PRE_ADD_PET_EXTEND_PERIOD
		m_pDnBuyConfirmPetExtendPeriodDlg->Show( false );
#endif // PRE_ADD_PET_EXTEND_PERIOD
	}

	CEtUIDialog::Show( bShow );
}

void CDnPetExtendPeriodDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl( "ID_OK" ) ) 
		{
			if( !m_pButtonSlot || !m_pPetItem ) return;

#ifdef PRE_ADD_PET_EXTEND_PERIOD
			TVehicleCompact* pPetCompact = GetVehicleTask().GetVehicleInfoFromSerial( m_pPetItem->GetSerialID() );
			if( pPetCompact == NULL ) return;

			m_pDnBuyConfirmPetExtendPeriodDlg->SetBuyInfo( GetPetTask().GetPetLevel( pPetCompact->Vehicle[Pet::Slot::Body].nSerial ), 
															pPetCompact->wszNickName, m_pExtendPeriodItem->GetName() );
			m_pDnBuyConfirmPetExtendPeriodDlg->SetDialogID( BUYCONFIRM_PETEXTENDPERIODITEM );
			m_pDnBuyConfirmPetExtendPeriodDlg->SetCallback( this );
			m_pDnBuyConfirmPetExtendPeriodDlg->Show( true );
#else // PRE_ADD_PET_EXTEND_PERIOD
			GetPetTask().SendPetExtendPeriod( m_pExtendPeriodItem->GetSerialID(), m_pPetItem->GetSerialID(), m_nExtendPeriod );
#endif // PRE_ADD_PET_EXTEND_PERIOD

			m_pButtonOK->Enable( false );
			CEtUIButton* pButton = GetControl<CEtUIButton>( "ID_CANCEL" );
			if( pButton )
				pButton->Enable( false );
			pButton = GetControl<CEtUIButton>( "ID_CLOSE" );
			if( pButton )
				pButton->Enable( false );
		}
		else if( IsCmdControl( "ID_CANCEL" ) ) 
		{
			Show( false );
			return;
		}
		else if( IsCmdControl( "ID_CLOSE" ) )
		{
			Show( false );
			return;
		}
		else if( IsCmdControl( "ID_ITEM" ) )
		{
			CDnSlotButton *pDragButton = (CDnSlotButton*)drag::GetControl();
			CDnSlotButton *pPressedButton = (CDnItemSlotButton*)pControl;

			// 우클릭으로 빼기
			if( uMsg == WM_RBUTTONUP )
			{
				SAFE_DELETE( m_pPetItem );
				pPressedButton->ResetSlot();
				if( m_pButtonSlot )
				{
					m_pButtonSlot->SetRegist( false );
					m_pButtonSlot = NULL;
				}
				CheckPetItem();
				return;
			}

			if( pDragButton )
			{
				drag::ReleaseControl();
				SetPetItem( pDragButton );

				if( pDragButton->GetItemType() != MIInventoryItem::Item )
					return;
			}
			return;
		}
	}
	CDnCustomDlg::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnPetExtendPeriodDlg::CheckPetItem()
{
	if( m_pItemSlot && !m_pItemSlot->IsEmptySlot() && m_pPetItem )
	{
		m_pButtonOK->Enable( true );
	}
	else
	{
		m_pButtonOK->Enable( false );
	}
}

void CDnPetExtendPeriodDlg::SetPetItem( CDnSlotButton* pSlotButton )
{
	if( pSlotButton == NULL || pSlotButton->GetItemType() != MIInventoryItem::Item )
		return;

	CDnItem *pItem = static_cast<CDnItem *>(pSlotButton->GetItem());
	if( pItem == NULL ) return;

	// 올린거 또 올리는거면 패스
	if( m_pButtonSlot && (pItem == m_pButtonSlot->GetItem()) ) return;

	// 아이템 종류 검사
	if( pItem->GetItemType() != ITEMTYPE_PET )
	{
		GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3609 ), MB_OK );
		return;
	}

	if( pItem->IsEternityItem() )
	{
		GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8051 ), MB_OK );
		return;
	}

	TVehicleCompact* pPetCompact = GetVehicleTask().GetVehicleInfoFromSerial( pItem->GetSerialID() );
	if( pPetCompact == NULL ) return;
	const time_t pNowTime = CSyncTimer::GetInstance().GetCurTime();

	if( pNowTime < pPetCompact->Vehicle[Pet::Slot::Body].tExpireDate )	// 만료기간 지났는지 체크
	{
		GetInterface().MessageBox( 9254, MB_OK );
		return;
	}

	CDnItem *pOriginItem = static_cast<CDnItem *>(m_pItemSlot->GetItem());
	if( pOriginItem )
	{
		SAFE_DELETE( m_pPetItem );
		m_pItemSlot->ResetSlot();
		m_pButtonSlot->SetRegist( false );
		m_pButtonSlot = NULL;
	}

	TItemInfo itemInfo;
	pItem->GetTItemInfo( itemInfo );
	m_pPetItem = GetItemTask().CreateItem( itemInfo );
	m_pItemSlot->SetItem( m_pPetItem, CDnSlotButton::ITEM_ORIGINAL_COUNT );

	CheckPetItem();
	CEtSoundEngine::GetInstance().PlaySound( "2D", pItem->GetDragSoundIndex() );

	m_pButtonSlot = pSlotButton;
	m_pButtonSlot->SetRegist( true );
}

#ifdef PRE_ADD_PET_EXTEND_PERIOD
void CDnPetExtendPeriodDlg::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg )
{
	if( pControl && nID == BUYCONFIRM_PETEXTENDPERIODITEM )
	{
		if( strcmp( pControl->GetControlName(), "ID_BT_OK" ) == 0 ) 
		{
			GetPetTask().SendPetExtendPeriod( m_pExtendPeriodItem->GetSerialID(), m_pPetItem->GetSerialID(), m_nExtendPeriod );
		}
		else if( strcmp( pControl->GetControlName(), "ID_BT_CANCEL" ) == 0 || strcmp( pControl->GetControlName(), "ID_BT_CLOSE" ) == 0 )
		{
			m_pButtonOK->Enable( true );
			CEtUIButton* pButton = GetControl<CEtUIButton>( "ID_CANCEL" );
			if( pButton )
				pButton->Enable( true );
			pButton = GetControl<CEtUIButton>( "ID_CLOSE" );
			if( pButton )
				pButton->Enable( true );
		}
	}
}
#endif // PRE_ADD_PET_EXTEND_PERIOD