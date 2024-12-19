#include "StdAfx.h"
#include "DnItemSealDlg.h"
#include "DnInterface.h"
#include "DnItemTask.h"
#include "DnMainMenuDlg.h"
#include "DnInvenTabDlg.h"
#include "DnItem.h"
#include "ItemSendPacket.h"
#include "DnCharInventory.h"
#include "DnTableDB.h"
#ifdef PRE_ADD_INSTANT_CASH_BUY
#include "DnInstantCashShopBuyDlg.h"
#include "DnCashShopTask.h"
#endif // PRE_ADD_INSTANT_CASH_BUY


#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnItemSealDlg::CDnItemSealDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback )
, m_pItem(NULL)
, m_pItemSlotButton(NULL)
, m_pQuickSlotButton(NULL)
, m_pStaticSeal(NULL)
, m_pStaticRandomSeal(NULL)
, m_pStaticItemName(NULL)
, m_pStaticTextAccount(NULL)
, m_pButtonDetail(NULL)
, m_pButtonOK(NULL)
, m_nSealSoundIdx(-1)
, m_nSealTypeParam(0)
, m_biItemSerial(0)
, m_nTotalSealNeeds(0)
{
}

CDnItemSealDlg::~CDnItemSealDlg(void)
{
	SAFE_DELETE( m_pItem );
	CEtSoundEngine::GetInstance().RemoveSound( m_nSealSoundIdx );
}

void CDnItemSealDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "InvenItemPackDlg.ui" ).c_str(), bShow );

	const char *szFileName = CDnTableDB::GetInstance().GetFileName( 10041 );
	if( strlen( szFileName ) > 0 )
		m_nSealSoundIdx = CEtSoundEngine::GetInstance().LoadSound( CEtResourceMng::GetInstance().GetFullName( szFileName ).c_str(), false, false );
}

void CDnItemSealDlg::InitialUpdate()
{
	m_pItemSlotButton = GetControl<CDnItemSlotButton>("ID_ITEM");
	//m_pItemSlotButton->SetSlotType( ST_ITEM_UPGRADE );
	m_pStaticSeal = GetControl<CEtUIStatic>("ID_STATIC7");
	m_pStaticRandomSeal = GetControl<CEtUIStatic>("ID_STATIC5");
	m_pStaticRandomSeal->Show( false );

	m_pStaticItemName = GetControl<CEtUIStatic>("ID_TEXT_NAME"); 
	m_pStaticTextAccount = GetControl<CEtUIStatic>("ID_TEXT_ACCOUNT");
	m_pButtonDetail = GetControl<CEtUIButton>("ID_BT_TOOLTIP");
	m_pButtonOK = GetControl<CEtUIButton>("ID_OK");
}

void CDnItemSealDlg::Show( bool bShow ) 
{ 
	CEtUIDialog::Show( bShow );

	if( bShow )
	{
		CDnInvenTabDlg *pInvenDlg = (CDnInvenTabDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::INVENTORY_DIALOG );
		if( pInvenDlg && pInvenDlg->IsShow() && !drag::IsValid() )
			pInvenDlg->ShowTab( ST_INVENTORY );

		WCHAR wszText[128] = {0,};
		swprintf_s( wszText, _countof(wszText), L"%s x %d", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4141 ), 0 );
		m_pStaticTextAccount->SetText( wszText );

		std::vector<CDnItem*> pVecSealItem;
		std::vector<int> VecItemID;
		std::vector<bool> VecEternity;
		GetItemTask().FindItemListFromItemTypeEx( ITEMTYPE_SEAL, pVecSealItem, m_nSealTypeParam );
		CDnItemTask::GetItemListForExtendUseItem( pVecSealItem, VecItemID, VecEternity );

		std::wstring wszTooltip;
		wszTooltip = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4142 );
		for( int i = 0; i < (int)VecItemID.size(); ++i )
		{
			wszTooltip += L"\n        ";
			wszTooltip += CDnItem::GetItemFullName( VecItemID[i] );
			if( VecEternity[i] == false ) wszTooltip += GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4144 );
		}
		m_pButtonDetail->SetTooltipText( wszTooltip.c_str() );

		m_pStaticSeal->Show( m_nSealTypeParam != 2 );
		m_pStaticRandomSeal->Show( m_nSealTypeParam == 2 );

		SetRenderPriority( this, true );

#ifdef PRE_ADD_INSTANT_CASH_BUY
		int nShowType = GetCashShopTask().GetInstantCashBuyEnableType( INSTANT_BUY_SEAL );
		if( nShowType == INSTANT_BUY_HIDE )
		{
			GetControl<CEtUIButton>( "UI_BUTTON_BUYSHORTCUT" )->Show( false );
			GetControl<CEtUIStatic>( "ID_STATIC_CASHBASE" )->Show( false );
		}
		else if( nShowType == INSTANT_BUY_SHOW )
		{
			GetControl<CEtUIButton>( "UI_BUTTON_BUYSHORTCUT" )->Show( true );
			GetControl<CEtUIButton>( "UI_BUTTON_BUYSHORTCUT" )->Enable( true );
			GetControl<CEtUIStatic>( "ID_STATIC_CASHBASE" )->Show( true );
		}
		else if( nShowType == INSTANT_BUY_DISABLE )
		{
			GetControl<CEtUIButton>( "UI_BUTTON_BUYSHORTCUT" )->Show( true );
			GetControl<CEtUIButton>( "UI_BUTTON_BUYSHORTCUT" )->Enable( false );
			GetControl<CEtUIStatic>( "ID_STATIC_CASHBASE" )->Show( true );
		}
#endif // PRE_ADD_INSTANT_CASH_BUY
	}
	else
	{
		m_pItemSlotButton->ResetSlot();
		m_pStaticSeal->Show( true );
		m_pStaticRandomSeal->Show( false );
		m_pStaticItemName->SetText( L"" );

		m_pButtonOK->Enable( false );

		if( m_pQuickSlotButton )
		{
			m_pQuickSlotButton->SetRegist( false );
			m_pQuickSlotButton->DisableSplitMode(true);
			m_pQuickSlotButton = NULL;
		}
	}
}

void CDnItemSealDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl("ID_OK") )
		{
			// 인장 개수 체크
			if( GetItemTask().FindItemCountFromItemType( ITEMTYPE_SEAL, m_nSealTypeParam ) < m_nTotalSealNeeds )
			{
				GetInterface().MessageBox( 4135, MB_OK );
				return;
			}

			SendSeal( m_pQuickSlotButton->GetSlotIndex(), m_pItem->GetSerialID(), m_biItemSerial );
			m_pButtonOK->Enable( false );
			return;
		}
		else if( IsCmdControl("ID_CANCEL") || IsCmdControl("ID_CLOSE") )
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
				if( m_pQuickSlotButton )
				{
					m_pQuickSlotButton->SetRegist( false );
					m_pQuickSlotButton->DisableSplitMode(true);
					m_pQuickSlotButton = NULL;
				}
				CheckSealItem();
				return;
			}

			if( pDragButton )
			{
				drag::ReleaseControl();

				if( pDragButton->GetItemType() != MIInventoryItem::Item )
					return;
				
				CDnItem *pItem = static_cast<CDnItem *>(pDragButton->GetItem());
				if( pItem == NULL || IsSealableItem( pDragButton ) == false )
				{
					pDragButton->DisableSplitMode( true );
					return;
				}

				CDnItem *pPressedItem = static_cast<CDnItem *>(pPressedButton->GetItem());
				if( pPressedItem )
				{
					SAFE_DELETE( m_pItem );
					pPressedButton->ResetSlot();
					m_pQuickSlotButton->SetRegist( false );
					m_pQuickSlotButton->DisableSplitMode(true);
					m_pQuickSlotButton = NULL;
				}

				TItemInfo itemInfo;
				pItem->GetTItemInfo( itemInfo );
				m_pItem = GetItemTask().CreateItem( itemInfo );
				pPressedButton->SetItem( m_pItem, CDnSlotButton::ITEM_ORIGINAL_COUNT );

				CheckSealItem();
				CEtSoundEngine::GetInstance().PlaySound( "2D", pItem->GetDragSoundIndex() );

				m_pQuickSlotButton = (CDnQuickSlotButton *)pDragButton;
				m_pQuickSlotButton->SetRegist( true );
			}

			return;
		}
#ifdef PRE_ADD_INSTANT_CASH_BUY
		else if( IsCmdControl( "UI_BUTTON_BUYSHORTCUT" ) )
		{
			GetInterface().ShowInstantCashShopBuyDlg( true, INSTANT_BUY_SEAL, NULL );
		}
#endif // PRE_ADD_INSTANT_CASH_BUY
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

bool CDnItemSealDlg::IsSealableItem( CDnSlotButton *pDragButton )
{
	if( pDragButton->GetSlotType() != ST_INVENTORY )
	{
		GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4137 ), MB_OK );
		return false;
	}

	CDnItem *pItem = static_cast<CDnItem *>(pDragButton->GetItem());

	// 올린거 또 올리는거면 패스
	if( m_pQuickSlotButton && (pItem == m_pQuickSlotButton->GetItem()) )
	{
		return false;
	}

	// 아이템 종류 검사
	if( pItem->GetItemType() == ITEMTYPE_QUEST )
	{
		GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3609 ), MB_OK );
		return false;
	}

	// 귀속 아이템인지, 이미 봉인된 아이템인지 검사
	if( pItem->GetReversion() == CDnItem::NoReversion || !pItem->IsSoulbBound() || !pItem->GetSealID() )
	{
		GetInterface().MessageBox( 4137, MB_OK );
		return false;
	}

	// 재봉인 횟수가 0인것도 검사.
	if( pItem->GetSealCount() == 0 )
	{
		GetInterface().MessageBox( 4136, MB_OK );
		return false;
	}

	// SealCount테이블 정보보고 봉인 가능한지 판단
	m_nTotalSealNeeds = pItem->GetTotalSealCount( m_nSealTypeParam );
	if( m_nTotalSealNeeds == 0 )
	{
		WCHAR wszMsg[256] = {0,};
		swprintf_s( wszMsg, _countof(wszMsg), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4116 ), m_wszSealItemName.c_str() );
		GetInterface().MessageBox( wszMsg, MB_OK );
		return false;
	}
	if( pItem->GetOverlapCount() )
		m_nTotalSealNeeds *= pItem->GetOverlapCount();

	return true;
}

void CDnItemSealDlg::SetSealItem( CDnQuickSlotButton *pPressedButton )
{
	if( pPressedButton->GetSlotType() != ST_INVENTORY )
	{
		GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4137 ), MB_OK );
		return;
	}

	CDnItem *pItem = static_cast<CDnItem *>(pPressedButton->GetItem());

	// 올린거 또 올리는거면 패스
	if( m_pQuickSlotButton && (pItem == m_pQuickSlotButton->GetItem()) )
	{
		return;
	}

	// 아이템 종류 검사
	if( pItem->GetItemType() == ITEMTYPE_QUEST )
	{
		GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3609 ), MB_OK );
		return;
	}

	// 귀속 아이템인지, 이미 봉인된 아이템인지, SealID가 0인지 검사.
	if( pItem->GetReversion() == CDnItem::NoReversion || !pItem->IsSoulbBound() || !pItem->GetSealID() )
	{
		GetInterface().MessageBox( 4137, MB_OK );
		return;
	}

	// 재봉인 횟수가 0인것도 검사.
	if( pItem->GetSealCount() == 0 )
	{
		GetInterface().MessageBox( 4136, MB_OK );
		return;
	}

	// SealCount테이블 정보보고 봉인 가능한지 판단
	m_nTotalSealNeeds = pItem->GetTotalSealCount( m_nSealTypeParam );
	if( m_nTotalSealNeeds == 0 )
	{
		WCHAR wszMsg[256] = {0,};
		swprintf_s( wszMsg, _countof(wszMsg), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4116 ), m_wszSealItemName.c_str() );
		GetInterface().MessageBox( wszMsg, MB_OK );
		return;
	}
	if( pItem->GetOverlapCount() )
		m_nTotalSealNeeds *= pItem->GetOverlapCount();

	CDnItem *pOriginItem = static_cast<CDnItem *>(m_pItemSlotButton->GetItem());
	if( pOriginItem )
	{
		SAFE_DELETE( m_pItem );
		m_pItemSlotButton->ResetSlot();
		m_pQuickSlotButton->SetRegist( false );
		m_pQuickSlotButton = NULL;
	}

	TItemInfo itemInfo;
	pItem->GetTItemInfo( itemInfo );
	m_pItem = GetItemTask().CreateItem( itemInfo );
	m_pItemSlotButton->SetItem( m_pItem, CDnSlotButton::ITEM_ORIGINAL_COUNT );

	CheckSealItem();
	CEtSoundEngine::GetInstance().PlaySound( "2D", pItem->GetDragSoundIndex() );

	m_pQuickSlotButton = (CDnQuickSlotButton *)pPressedButton;
	m_pQuickSlotButton->SetRegist( true );
}

void CDnItemSealDlg::CheckSealItem()
{
	if( m_pItemSlotButton && !m_pItemSlotButton->IsEmptySlot() && m_pItem )
	{
		m_pButtonOK->Enable( true );
	}
	else
	{
		m_nTotalSealNeeds = 0;
		m_pButtonOK->Enable( false );
	}

	WCHAR wszText[128] = {0,};
	swprintf_s( wszText, _countof(wszText), L"%s x %d", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4141 ), m_nTotalSealNeeds );
	m_pStaticTextAccount->SetText( wszText );
}

void CDnItemSealDlg::SetSealItem( CDnItem *pSealItem )
{
	if( pSealItem )
	{
		m_nSealTypeParam = pSealItem->GetTypeParam();
		m_wszSealItemName = pSealItem->GetName();
		m_biItemSerial = ( pSealItem->IsCashItem() == true ) ? pSealItem->GetSerialID() : pSealItem->GetSlotIndex();
	}
}

void CDnItemSealDlg::OnRecvSealItem( int nResult, int nSlotIndex )
{
	if( nResult == ERROR_NONE )
	{
		CDnItem *pItem = GetItemTask().GetCharInventory().GetItem( nSlotIndex );
		if( !pItem || !m_pItem || pItem->GetClassID() != m_pItem->GetClassID() )
			return;

		if (m_nSealSoundIdx >= 0)
			CEtSoundEngine::GetInstance().PlaySound( "2D", m_nSealSoundIdx, false );
	}

	SAFE_DELETE(m_pItem);
	m_pItemSlotButton->ResetSlot();

	if( m_pQuickSlotButton )
	{
		m_pQuickSlotButton->SetRegist( false );
		m_pQuickSlotButton->DisableSplitMode(true);
		m_pQuickSlotButton = NULL;
	}

	// 창을 닫아버리는게 더 편한 듯
	Show( false );
}