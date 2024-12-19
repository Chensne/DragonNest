#include "StdAfx.h"
#include "DnRemovePrefixDlg.h"
#include "DnItemTask.h"
#include "DnInterface.h"
#include "DnMainMenuDlg.h"
#include "DnInvenTabDlg.h"
#include "ItemSendPacket.h"
#include "DnPotentialJewel.h"
#include "DnTableDB.h"

CDnRemovePrefixDlg::CDnRemovePrefixDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback, true )
{
	m_pItem = NULL;
	m_pItemSlot = NULL;
	m_pStaticText = NULL;
	m_pButtonApply = NULL;
	m_pButtonQuickSlot = NULL;

	m_pPrefixItem = NULL;
	m_nSoundIdx = -1;
}

CDnRemovePrefixDlg::~CDnRemovePrefixDlg()
{
	SAFE_DELETE( m_pItem );
	CEtSoundEngine::GetInstance().RemoveSound( m_nSoundIdx );
}

void CDnRemovePrefixDlg::Initialize( bool bShow )
{
	CDnCustomDlg::Initialize( CEtResourceMng::GetInstance().GetFullName( "InvenItemCubeDlg.ui" ).c_str(), bShow );

	const char *szFileName = CDnTableDB::GetInstance().GetFileName( 10041 );
	if( strlen( szFileName ) > 0 )
		m_nSoundIdx = CEtSoundEngine::GetInstance().LoadSound( CEtResourceMng::GetInstance().GetFullName( szFileName ).c_str(), false, false );
}

void CDnRemovePrefixDlg::InitialUpdate()
{
	m_pItemSlot = GetControl<CDnItemSlotButton>("ID_ITEM");
	m_pStaticText = GetControl<CEtUIStatic>("ID_TEXT_ASK");
	m_pButtonApply = GetControl<CEtUIButton>("ID_OK");
	//GetControl<CEtUIStatic>("ID_STATIC0")->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8058 ) );
}

void CDnRemovePrefixDlg::Show( bool bShow )
{
	CDnCustomDlg::Show( bShow );

	if( bShow )
	{
		CDnInvenTabDlg *pInvenDlg = (CDnInvenTabDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::INVENTORY_DIALOG );
		if( pInvenDlg && pInvenDlg->IsShow() && !drag::IsValid() )
			pInvenDlg->ShowTab( ST_INVENTORY );

		WCHAR wszText[256] = {0,};
		swprintf_s( wszText, _countof(wszText), GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 3322), m_pPrefixItem ? m_pPrefixItem->GetName() : L"" );
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

void CDnRemovePrefixDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl( "ID_OK" ) ) {
			if( !m_pButtonQuickSlot || !m_pPrefixItem ) return;
#if defined(PRE_ADD_REMOVE_PREFIX)
			SendRemovePrefix( m_pButtonQuickSlot->GetSlotIndex(), m_pItem->GetSerialID(), ( m_pPrefixItem->IsCashItem() == true ) ? m_pPrefixItem->GetSerialID() : m_pPrefixItem->GetSlotIndex() );
#endif	// #if defined(PRE_ADD_REMOVE_PREFIX)
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
				CheckRemovePrefixItem();
				return;
			}

			if( pDragButton )
			{
				drag::ReleaseControl();

				if( pDragButton->GetItemType() != MIInventoryItem::Item )
					return;

				CDnItem *pItem = static_cast<CDnItem *>(pDragButton->GetItem());
				if( pItem == NULL || CanApplyRemovePrefix( pDragButton ) == false )
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

				CheckRemovePrefixItem();
				CEtSoundEngine::GetInstance().PlaySound( "2D", pItem->GetDragSoundIndex() );

				m_pButtonQuickSlot = (CDnQuickSlotButton *)pDragButton;
				m_pButtonQuickSlot->SetRegist( true );
			}
			return;
		}
	}
	CDnCustomDlg::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnRemovePrefixDlg::SetItem( CDnQuickSlotButton *pPressedButton )
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

	int nSkillID = 0;
	int nSkillLevel = 0;

	if( pItem->HasPrefixSkill(nSkillID, nSkillLevel) == false ) {
		GetInterface().MessageBox( 8051, MB_OK );
		return;
	}

	int nApplyApplicableValue = ((CDnPotentialJewel*)m_pPrefixItem)->GetApplyApplicableValue();
	int nCurApplicableValue = pItem->GetApplicableValue();
	if( ( nCurApplicableValue & nApplyApplicableValue ) != nCurApplicableValue ) {
		GetInterface().MessageBox( 8051, MB_OK );
		return;
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

	CheckRemovePrefixItem();
	CEtSoundEngine::GetInstance().PlaySound( "2D", pItem->GetDragSoundIndex() );

	m_pButtonQuickSlot = (CDnQuickSlotButton *)pPressedButton;
	m_pButtonQuickSlot->SetRegist( true );
}

bool CDnRemovePrefixDlg::CanApplyRemovePrefix( CDnSlotButton *pDragButton )
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
	
	int nSkillID = 0;
	int nSkillLevel = 0;

	if( pItem->HasPrefixSkill(nSkillID, nSkillLevel) == false ) {
		GetInterface().MessageBox( 8051, MB_OK );
		return false;
	}

	return true;
}

void CDnRemovePrefixDlg::CheckRemovePrefixItem()
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

void CDnRemovePrefixDlg::SetItem( CDnItem *pItem )
{
	m_pPrefixItem = pItem;
	if( m_pPrefixItem && m_pPrefixItem->GetItemType() != ITEMTYPE_POTENTIAL_JEWEL ) {
		m_pPrefixItem = NULL;
		return;
	}
}


void CDnRemovePrefixDlg::OnRecvRemovePrefixItem( int nResult, int nSlotIndex )
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