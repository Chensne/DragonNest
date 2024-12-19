#include "StdAfx.h"
#include "DnEnchantJewelDlg.h"
#include "DnItemTask.h"
#include "DnInterface.h"
#include "DnMainMenuDlg.h"
#include "DnInvenTabDlg.h"
#include "ItemSendPacket.h"
#include "DnEnchantJewel.h"
#include "DnTableDB.h"
#ifdef PRE_ADD_SHORTCUT_HELP_DIALOG
#include "DnSystemDlg.h"
#endif

CDnEnchantJewelDlg::CDnEnchantJewelDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback, true )
{
	m_pItem = NULL;
	m_pItemSlot = NULL;
	m_pStaticText = NULL;
	m_pButtonApply = NULL;
	m_pButtonQuickSlot = NULL;

	m_pEnchantJewelItem = NULL;
	m_nSoundIdx = -1;
}

CDnEnchantJewelDlg::~CDnEnchantJewelDlg()
{
	SAFE_DELETE( m_pItem );
	CEtSoundEngine::GetInstance().RemoveSound( m_nSoundIdx );
}

void CDnEnchantJewelDlg::Initialize( bool bShow )
{
#ifdef PRE_MOD_POTENTIAL_JEWEL_RENEWAL
	CDnCustomDlg::Initialize( CEtResourceMng::GetInstance().GetFullName( "InvenItemSubPowerDlg.ui" ).c_str(), bShow );
#else
	CDnCustomDlg::Initialize( CEtResourceMng::GetInstance().GetFullName( "InvenItemPowerDlg.ui" ).c_str(), bShow );
#endif // PRE_MOD_POTENTIAL_JEWEL_RENEWAL

	const char *szFileName = CDnTableDB::GetInstance().GetFileName( 10041 );
	if( strlen( szFileName ) > 0 )
		m_nSoundIdx = CEtSoundEngine::GetInstance().LoadSound( CEtResourceMng::GetInstance().GetFullName( szFileName ).c_str(), false, false );
}

void CDnEnchantJewelDlg::InitialUpdate()
{
	m_pItemSlot = GetControl<CDnItemSlotButton>("ID_ITEM");
	m_pStaticText = GetControl<CEtUIStatic>("ID_TEXT_ASK");
	m_pButtonApply = GetControl<CEtUIButton>("ID_OK");
	GetControl<CEtUIStatic>("ID_STATIC0")->SetText(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 8058));
#ifdef PRE_MOD_POTENTIAL_JEWEL_RENEWAL	
	GetControl<CEtUIStatic>("ID_TEXT")->SetText(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 8343)); // 사용한 코드에 맞는 장비를 소지품(I)에서 우클릭하면 등록됩니다.
#endif
}

void CDnEnchantJewelDlg::Show( bool bShow )
{
	CDnCustomDlg::Show( bShow );

	if( bShow )
	{
		CDnInvenTabDlg *pInvenDlg = (CDnInvenTabDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::INVENTORY_DIALOG );
		if( pInvenDlg && pInvenDlg->IsShow() && !drag::IsValid() )
			pInvenDlg->ShowTab( ST_INVENTORY );

		WCHAR wszText[256] = {0,};
		swprintf_s( wszText, _countof(wszText), GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 8053), m_pEnchantJewelItem->GetName() );
		m_pStaticText->SetText( wszText );

		SetRenderPriority( this, true );
	}
	else
	{
		m_pItemSlot->ResetSlot();
		m_pStaticText->SetText( L"" );

		m_pButtonApply->Enable( false );

		if( m_pButtonQuickSlot )
		{
			m_pButtonQuickSlot->SetRegist( false );
			m_pButtonQuickSlot = NULL;
		}
	}
}

void CDnEnchantJewelDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl( "ID_OK" ) ) {
			if( !m_pButtonQuickSlot || !m_pEnchantJewelItem ) return;
			SendEnchantJewel( m_pButtonQuickSlot->GetSlotIndex(), m_pItem->GetSerialID(), ( m_pEnchantJewelItem->IsCashItem() == true ) ? m_pEnchantJewelItem->GetSerialID() : m_pEnchantJewelItem->GetSlotIndex() );
			m_pButtonApply->Enable( false );
		}
		else if( IsCmdControl( "ID_CANCEL" ) ) {
			Show(false);
			return;
		}
		else if( IsCmdControl("ID_CLOSE") )
		{
			Show(false);
			return;
		}
#ifdef PRE_ADD_SHORTCUT_HELP_DIALOG
		else if( IsCmdControl("ID_BT_SMALLHELP") )
		{
			CDnMainMenuDlg* pMainMenuDlg = GetInterface().GetMainMenuDialog();
			CDnSystemDlg* pSystemDlg = pMainMenuDlg->GetSystemDlg();
			if(pSystemDlg)
				pSystemDlg->ShowChoiceHelpDlg(HELP_SHORTCUT_POTENTIAL);
		}
#endif
		else if( IsCmdControl("ID_ITEM") )
		{
			CDnSlotButton *pDragButton = ( CDnSlotButton * )drag::GetControl();
			CDnSlotButton *pPressedButton = ( CDnItemSlotButton * )pControl;

			// 우클릭으로 빼기
			if( uMsg == WM_RBUTTONUP )
			{
				SAFE_DELETE(m_pItem);
				pPressedButton->ResetSlot();
				if( m_pButtonQuickSlot )
				{
					m_pButtonQuickSlot->SetRegist( false );
					m_pButtonQuickSlot = NULL;
				}
				CheckEnchantJewelItem();
				return;
			}

			if( pDragButton )
			{
				drag::ReleaseControl();

				if( pDragButton->GetItemType() != MIInventoryItem::Item )
					return;

				CDnItem *pItem = static_cast<CDnItem *>(pDragButton->GetItem());
				if( pItem == NULL || CanApplyEnchantJewel( pDragButton ) == false )
				{
					pDragButton->DisableSplitMode( true );
					return;
				}

				CDnItem *pPressedItem = static_cast<CDnItem *>(pPressedButton->GetItem());
				if( pPressedItem )
				{
					SAFE_DELETE( m_pItem );
					pPressedButton->ResetSlot();
					m_pButtonQuickSlot->SetRegist( false );
					m_pButtonQuickSlot = NULL;
				}

				TItemInfo itemInfo;
				pItem->GetTItemInfo( itemInfo );
				m_pItem = GetItemTask().CreateItem( itemInfo );
				pPressedButton->SetItem( m_pItem, CDnSlotButton::ITEM_ORIGINAL_COUNT );

				CheckEnchantJewelItem();
				CEtSoundEngine::GetInstance().PlaySound( "2D", pItem->GetDragSoundIndex() );

				m_pButtonQuickSlot = (CDnQuickSlotButton *)pDragButton;
				m_pButtonQuickSlot->SetRegist( true );
			}
			return;
		}
	}
	CDnCustomDlg::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnEnchantJewelDlg::SetEnchantJewelItem( CDnQuickSlotButton *pPressedButton )
{
	if( pPressedButton->GetSlotType() != ST_INVENTORY )
	{
		GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4137 ), MB_OK );
		return;
	}

	CDnItem *pItem = static_cast<CDnItem *>(pPressedButton->GetItem());

	// 올린거 또 올리는거면 패스
	if( m_pButtonQuickSlot && (pItem == m_pButtonQuickSlot->GetItem()) )
	{
		return;
	}

	// 아이템 종류 검사
	switch( pItem->GetItemType() ) {
		case ITEMTYPE_WEAPON:
		case ITEMTYPE_PARTS:
			break;
		default:
			GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3609 ), MB_OK );
			return;
	}
	if( pItem->GetEnchantID() == 0 ) {
		GetInterface().MessageBox( 8051, MB_OK );
		return;
	}

	int nApplyApplicableValue = ((CDnEnchantJewel*)m_pEnchantJewelItem)->GetApplyApplicableValue();
	int nCurApplicableValue = pItem->GetApplicableValue();
	if( ( nCurApplicableValue & nApplyApplicableValue ) != nCurApplicableValue ) {
		GetInterface().MessageBox( 8051, MB_OK );
		return;
	}
	if( ((CDnEnchantJewel*)m_pEnchantJewelItem)->GetRequireMaxItemLevel() != 0 ) {
		if( pItem->GetLevelLimit() > ((CDnEnchantJewel*)m_pEnchantJewelItem)->GetRequireMaxItemLevel() ) {
			GetInterface().MessageBox( 8051, MB_OK );
			return;
		}
	}
	if( pItem->GetEnchantLevel() < ((CDnEnchantJewel*)m_pEnchantJewelItem)->GetRequireEnchantLevel() ) {
		GetInterface().MessageBox( 8051, MB_OK );
		return;
	}
	if( pItem->GetEnchantLevel() >= ((CDnEnchantJewel*)m_pEnchantJewelItem)->GetEnchantLevel() ) {
		GetInterface().MessageBox( 8051, MB_OK );
		return;
	}
	if( ((CDnEnchantJewel*)m_pEnchantJewelItem)->IsCanApplySealedItem() == false ) {
		if( pItem->IsSoulbBound() == false ) {
			GetInterface().MessageBox( 8051, MB_OK );
			return;
		}
	}

	CDnItem *pOriginItem = static_cast<CDnItem *>(m_pItemSlot->GetItem());
	if( pOriginItem )
	{
		SAFE_DELETE( m_pItem );
		m_pItemSlot->ResetSlot();
		m_pButtonQuickSlot->SetRegist( false );
		m_pButtonQuickSlot = NULL;
	}

	TItemInfo itemInfo;
	pItem->GetTItemInfo( itemInfo );
	m_pItem = GetItemTask().CreateItem( itemInfo );
	m_pItemSlot->SetItem( m_pItem, CDnSlotButton::ITEM_ORIGINAL_COUNT );

	CheckEnchantJewelItem();
	CEtSoundEngine::GetInstance().PlaySound( "2D", pItem->GetDragSoundIndex() );

	m_pButtonQuickSlot = (CDnQuickSlotButton *)pPressedButton;
	m_pButtonQuickSlot->SetRegist( true );
}

bool CDnEnchantJewelDlg::CanApplyEnchantJewel( CDnSlotButton *pDragButton )
{
	if( pDragButton->GetSlotType() != ST_INVENTORY )
	{
		GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8051 ), MB_OK );
		return false;
	}

	CDnItem *pItem = static_cast<CDnItem *>(pDragButton->GetItem());

	// 올린거 또 올리는거면 패스
	if( m_pButtonQuickSlot && (pItem == m_pButtonQuickSlot->GetItem()) )
	{
		return false;
	}

	// 아이템 종류 검사
	switch( pItem->GetItemType() ) {
		case ITEMTYPE_WEAPON:
		case ITEMTYPE_PARTS:
			break;
		default:
			GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3609 ), MB_OK );
			return false;
	}
	if( pItem->GetTypeParam(1) == 0 ) {
		GetInterface().MessageBox( 8051, MB_OK );
		return false;
	}

	int nApplyApplicableValue = ((CDnEnchantJewel*)m_pEnchantJewelItem)->GetApplyApplicableValue();
	int nCurApplicableValue = pItem->GetApplicableValue();
	if( ( nCurApplicableValue & nApplyApplicableValue ) != nCurApplicableValue ) {
		GetInterface().MessageBox( 8051, MB_OK );
		return false;
	}
	if( ((CDnEnchantJewel*)m_pEnchantJewelItem)->GetRequireMaxItemLevel() != 0 ) {
		if( pItem->GetLevelLimit() > ((CDnEnchantJewel*)m_pEnchantJewelItem)->GetRequireMaxItemLevel() ) {
			GetInterface().MessageBox( 8051, MB_OK );
			return false;
		}
	}
	if( pItem->GetEnchantLevel() < ((CDnEnchantJewel*)m_pEnchantJewelItem)->GetRequireEnchantLevel() ) {
		GetInterface().MessageBox( 8051, MB_OK );
		return false;
	}
	if( pItem->GetEnchantLevel() >= ((CDnEnchantJewel*)m_pEnchantJewelItem)->GetEnchantLevel() ) {
		GetInterface().MessageBox( 8051, MB_OK );
		return false;
	}
	if( ((CDnEnchantJewel*)m_pEnchantJewelItem)->IsCanApplySealedItem() == false ) {
		if( pItem->IsSoulbBound() == false ) {
			GetInterface().MessageBox( 8051, MB_OK );
			return false;
		}
	}

	return true;
}

void CDnEnchantJewelDlg::CheckEnchantJewelItem()
{
	if( m_pItemSlot && !m_pItemSlot->IsEmptySlot() && m_pItem )
	{
		m_pButtonApply->Enable( true );
	}
	else
	{
		m_pButtonApply->Enable( false );
	}
}

void CDnEnchantJewelDlg::SetEnchantJewelItem( CDnItem *pItem )
{
	m_pEnchantJewelItem = pItem;
	if( m_pEnchantJewelItem && m_pEnchantJewelItem->GetItemType() != ITEMTYPE_ENCHANT_JEWEL ) {
		m_pEnchantJewelItem = NULL;
		return;
	}
}


void CDnEnchantJewelDlg::OnRecvEnchantJewelItem( int nResult, int nSlotIndex )
{
	if( nResult == ERROR_NONE )
	{
		CDnItem *pItem = GetItemTask().GetCharInventory().GetItem( nSlotIndex );
		if( !pItem || !m_pItem || pItem->GetClassID() != m_pItem->GetClassID() )
			return;

		if (m_nSoundIdx >= 0)
			CEtSoundEngine::GetInstance().PlaySound( "2D", m_nSoundIdx, false );
	}

	SAFE_DELETE(m_pItem);
	m_pItemSlot->ResetSlot();

	if( m_pButtonQuickSlot )
	{
		m_pButtonQuickSlot->SetRegist( false );
		m_pButtonQuickSlot = NULL;
	}

	// 창을 닫아버리는게 더 편한 듯
	Show( false );
}