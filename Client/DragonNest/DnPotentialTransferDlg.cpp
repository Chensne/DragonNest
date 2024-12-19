#include "StdAfx.h"
#include "DnPotentialTransferDlg.h"
#include "DnPotentialTransferInfoDlg.h"
#include "DnItemTask.h"
#include "DnInterface.h"
#include "DnMainMenuDlg.h"
#include "DnInvenTabDlg.h"
#include "ItemSendPacket.h"
#include "DnTableDB.h"
#ifdef PRE_ADD_INSTANT_CASH_BUY
#include "DnInstantCashShopBuyDlg.h"
#include "DnCashShopTask.h"
#endif // PRE_ADD_INSTANT_CASH_BUY


#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

#ifdef PRE_ADD_EXCHANGE_POTENTIAL

CDnPotentialTransferDlg::CDnPotentialTransferDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback, true )
{
	m_pItem1 = NULL;
	m_pItemSlot1 = NULL;
	m_pButtonQuickSlot1 = NULL;
	m_pItem2 = NULL;
	m_pItemSlot2 = NULL;
	m_pButtonQuickSlot2 = NULL;

	m_pTransItem = NULL;
	m_pTransItemSlot = NULL;

	m_pStaticText = NULL;
	m_pButtonInfo = NULL;
	m_pStaticCount = NULL;
	m_pButtonDetail = NULL;
	m_pButtonApply = NULL;
	m_nSoundIdx = -1;
	m_dwCountColor = 0;

	m_nStep = 0;

	m_pInfoDlg = NULL;
}

CDnPotentialTransferDlg::~CDnPotentialTransferDlg()
{
	SAFE_DELETE( m_pItem1 );
	SAFE_DELETE( m_pItem2 );
	SAFE_DELETE( m_pTransItem );
	SAFE_DELETE( m_pInfoDlg );
	CEtSoundEngine::GetInstance().RemoveSound( m_nSoundIdx );
}

void CDnPotentialTransferDlg::Initialize( bool bShow )
{
	CDnCustomDlg::Initialize( CEtResourceMng::GetInstance().GetFullName( "InvenPotentialDlg.ui" ).c_str(), bShow );

	const char *szFileName = CDnTableDB::GetInstance().GetFileName( 10041 );
	if( strlen( szFileName ) > 0 )
		m_nSoundIdx = CEtSoundEngine::GetInstance().LoadSound( CEtResourceMng::GetInstance().GetFullName( szFileName ).c_str(), false, false );
}

void CDnPotentialTransferDlg::InitialUpdate()
{
	m_pItemSlot1 = GetControl<CDnItemSlotButton>("ID_ITEM0");
	m_pItemSlot2 = GetControl<CDnItemSlotButton>("ID_ITEM1");
	m_pTransItemSlot = GetControl<CDnItemSlotButton>("ID_ITEM2");
	m_pStaticText = GetControl<CEtUIStatic>("ID_TEXT_ASK0");
	m_pButtonInfo = GetControl<CEtUIButton>("ID_BT_OPTION");

	m_pStaticCount = GetControl<CEtUIStatic>("ID_TEXT_COUNT1");
	m_pButtonDetail = GetControl<CEtUIButton>("ID_BT_TOOLTIP");
	m_pButtonApply = GetControl<CEtUIButton>("ID_OK");
	m_dwCountColor = m_pStaticCount->GetTextColor();

	m_pInfoDlg = new CDnPotentialTransferInfoDlg( UI_TYPE_MODAL );
	m_pInfoDlg->Initialize( false );
}

void CDnPotentialTransferDlg::Show( bool bShow )
{
	CDnCustomDlg::Show( bShow );

	if( bShow )
	{
		CDnInvenTabDlg *pInvenDlg = (CDnInvenTabDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::INVENTORY_DIALOG );
		if( pInvenDlg && pInvenDlg->IsShow() && !drag::IsValid() )
			pInvenDlg->ShowTab(ST_INVENTORY_CASH);

		std::vector<CDnItem*> pVecTransferItem;
		std::vector<int> VecItemID;
		std::vector<bool> VecEternity;
		GetItemTask().FindItemListFromItemTypeEx( ITEMTYPE_POTENTIAL_EXTRACTOR, pVecTransferItem );
		CDnItemTask::GetItemListForExtendUseItem( pVecTransferItem, VecItemID, VecEternity );

		std::wstring wszTooltip;
		wszTooltip = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8077 );
		for( int i = 0; i < (int)VecItemID.size(); ++i )
		{
			wszTooltip += L"\n        ";
			wszTooltip += CDnItem::GetItemFullName( VecItemID[i] );
			if( VecEternity[i] == false ) wszTooltip += GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4144 );
		}
		m_pButtonDetail->SetTooltipText( wszTooltip.c_str() );

		SetRenderPriority( this, true );
#ifdef PRE_ADD_INSTANT_CASH_BUY
		int nShowType = GetCashShopTask().GetInstantCashBuyEnableType( INSTANT_BUY_POTENTRANS );
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
		m_nStep = 0;
		OnChangeStep( m_nStep );
	}
}

void CDnPotentialTransferDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl( "ID_OK" ) ) {
			if( !m_pButtonQuickSlot1 || !m_pButtonQuickSlot2 ) return;
			GetInterface().MessageBox( 8078, MB_YESNO, 0, this, false );
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
		else if( IsCmdControl("ID_BT_OPTION") )
		{
			m_pInfoDlg->Show( true );
		}
		else if( IsCmdControl("ID_ITEM0") )
		{
			CDnSlotButton *pDragButton = ( CDnSlotButton * )drag::GetControl();
			CDnSlotButton *pPressedButton = ( CDnItemSlotButton * )pControl;

			// 우클릭으로 빼기
			if( uMsg == WM_RBUTTONUP )
			{
				m_nStep = 0;
				OnChangeStep( m_nStep );
				return;
			}

			if( pDragButton )
			{
				drag::ReleaseControl();

				if( pDragButton->GetItemType() != MIInventoryItem::Item )
					return;

				CDnItem *pItem = static_cast<CDnItem *>(pDragButton->GetItem());
				if( pItem == NULL || IsPotentialExtractItem( pDragButton ) == false )
				{
					pDragButton->DisableSplitMode( true );
					return;
				}

				CDnItem *pPressedItem = static_cast<CDnItem *>(pPressedButton->GetItem());
				if( pPressedItem )
				{
					SAFE_DELETE( m_pItem1 );
					pPressedButton->ResetSlot();
					m_pButtonQuickSlot1->SetRegist( false );
					m_pButtonQuickSlot1 = NULL;
				}

				if( m_nStep == 1 )
				{
					CDnItem *pItem2 = static_cast<CDnItem *>(m_pItemSlot2->GetItem());
					if( pItem2 )
					{
						SAFE_DELETE( m_pItem2 );
						m_pItemSlot2->ResetSlot();
						m_pButtonQuickSlot2->SetRegist( false );
						m_pButtonQuickSlot2 = NULL;
					}
				}

				TItemInfo itemInfo;
				pItem->GetTItemInfo( itemInfo );
				m_pItem1 = GetItemTask().CreateItem( itemInfo );
				pPressedButton->SetItem( m_pItem1, CDnSlotButton::ITEM_ORIGINAL_COUNT );

				m_nStep = 1;
				OnChangeStep( m_nStep );
				CEtSoundEngine::GetInstance().PlaySound( "2D", pItem->GetDragSoundIndex() );

				m_pButtonQuickSlot1 = (CDnQuickSlotButton *)pDragButton;
				m_pButtonQuickSlot1->SetRegist( true );
			}
			return;
		}
		else if( IsCmdControl("ID_ITEM1") )
		{
			CDnSlotButton *pDragButton = ( CDnSlotButton * )drag::GetControl();
			CDnSlotButton *pPressedButton = ( CDnItemSlotButton * )pControl;

			// 우클릭으로 빼기
			if( uMsg == WM_RBUTTONUP )
			{
				m_nStep = 1;
				OnChangeStep( m_nStep );
				return;
			}

			if( pDragButton )
			{
				drag::ReleaseControl();

				if( pDragButton->GetItemType() != MIInventoryItem::Item )
					return;

				if( m_nStep == 0 )
				{
					GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8087 ), MB_OK );
					pDragButton->DisableSplitMode( true );
					return;
				}

				CDnItem *pItem = static_cast<CDnItem *>(pDragButton->GetItem());
				if( pItem == NULL || IsPotentialableItem( pDragButton ) == false )
				{
					pDragButton->DisableSplitMode( true );
					return;
				}

				CDnItem *pPressedItem = static_cast<CDnItem *>(pPressedButton->GetItem());
				if( pPressedItem )
				{
					SAFE_DELETE( m_pItem2 );
					pPressedButton->ResetSlot();
					m_pButtonQuickSlot2->SetRegist( false );
					m_pButtonQuickSlot2 = NULL;
				}

				TItemInfo itemInfo;
				pItem->GetTItemInfo( itemInfo );
				m_pItem2 = GetItemTask().CreateItem( itemInfo );
				pPressedButton->SetItem( m_pItem2, CDnSlotButton::ITEM_ORIGINAL_COUNT );

				RefreshPotentialTransfer();
				CEtSoundEngine::GetInstance().PlaySound( "2D", pItem->GetDragSoundIndex() );

				m_pButtonQuickSlot2 = (CDnQuickSlotButton *)pDragButton;
				m_pButtonQuickSlot2->SetRegist( true );
			}
		}
#ifdef PRE_ADD_INSTANT_CASH_BUY
		else if( IsCmdControl( "UI_BUTTON_BUYSHORTCUT" ) )
		{
			GetInterface().ShowInstantCashShopBuyDlg( true, INSTANT_BUY_POTENTRANS, this );
			return;
		}
#endif // PRE_ADD_INSTANT_CASH_BUY
	}
	CDnCustomDlg::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );

}

void CDnPotentialTransferDlg::OnChangeStep( int nStep )
{
	if( m_nStep == 0 )
	{
		m_pItemSlot1->ResetSlot();
		m_pItemSlot2->ResetSlot();
		m_pTransItemSlot->ResetSlot();
		m_pStaticText->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8074 ) );

		m_pButtonApply->Enable( false );

		if( m_pButtonQuickSlot1 )
		{
			m_pButtonQuickSlot1->SetRegist( false );
			m_pButtonQuickSlot1 = NULL;
		}
		if( m_pButtonQuickSlot2 )
		{
			m_pButtonQuickSlot2->SetRegist( false );
			m_pButtonQuickSlot2 = NULL;
		}
	}
	else if( m_nStep == 1 )
	{
		m_pItemSlot2->ResetSlot();
		m_pTransItemSlot->ResetSlot();
		m_pStaticText->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8079 ) );

		m_pButtonApply->Enable( false );

		if( m_pButtonQuickSlot2 )
		{
			m_pButtonQuickSlot2->SetRegist( false );
			m_pButtonQuickSlot2 = NULL;
		}
	}
	RefreshPotentialTransfer();
}

void CDnPotentialTransferDlg::OnRButtonClick( CDnQuickSlotButton *pPressedButton )
{


	if( pPressedButton->IsRegist() )
		return;

	CDnItem *pItem = static_cast<CDnItem *>(pPressedButton->GetItem());

	if( m_nStep == 0 )
	{
		// 올린거 또 올리는거면 패스
		if( m_pButtonQuickSlot1 && (pItem == m_pButtonQuickSlot1->GetItem()) )
		{
			return;
		}

		// 잠재력 붙어있는지
		// 잠재력 추출이 가능한지
		// 봉인되어있는지
		if( pItem == NULL || IsPotentialExtractItem( pPressedButton ) == false )
		{
			return;
		}

		// 다 통과했으면
		CDnItem *pOriginItem1 = static_cast<CDnItem *>(m_pItemSlot1->GetItem());
		if( pOriginItem1 )
		{
			SAFE_DELETE( m_pItem1 );
			m_pItemSlot1->ResetSlot();
			m_pButtonQuickSlot1->SetRegist( false );
			m_pButtonQuickSlot1 = NULL;
		}

		TItemInfo itemInfo;
		pItem->GetTItemInfo( itemInfo );
		m_pItem1 = GetItemTask().CreateItem( itemInfo );
		m_pItemSlot1->SetItem( m_pItem1, CDnSlotButton::ITEM_ORIGINAL_COUNT );

		m_nStep = 1;
		OnChangeStep( m_nStep );
		CEtSoundEngine::GetInstance().PlaySound( "2D", pItem->GetDragSoundIndex() );

		m_pButtonQuickSlot1 = (CDnQuickSlotButton *)pPressedButton;
		m_pButtonQuickSlot1->SetRegist( true );
	}
	else if( m_nStep == 1 )
	{
		// 올린거 또 올리는거면 패스
		if( m_pButtonQuickSlot2 && (pItem == m_pButtonQuickSlot2->GetItem()) )
		{
			return;
		}

		// 잠재력이 없는지
		// 잠재력 적용이 가능한지
		// 봉인되어있는지
		if( pItem == NULL || IsPotentialableItem( pPressedButton ) == false )
		{
			return;
		}

		CDnItem *pOriginItem2 = static_cast<CDnItem *>(m_pItemSlot2->GetItem());
		if( pOriginItem2 )
		{
			GetInterface().MessageBox( 8086, MB_OK );
			return;
		}

		TItemInfo itemInfo;
		pItem->GetTItemInfo( itemInfo );
		m_pItem2 = GetItemTask().CreateItem( itemInfo );
		m_pItemSlot2->SetItem( m_pItem2, CDnSlotButton::ITEM_ORIGINAL_COUNT );

		RefreshPotentialTransfer();
		CEtSoundEngine::GetInstance().PlaySound( "2D", pItem->GetDragSoundIndex() );

		m_pButtonQuickSlot2 = (CDnQuickSlotButton *)pPressedButton;
		m_pButtonQuickSlot2->SetRegist( true );
	}
}

bool CDnPotentialTransferDlg::IsPotentialExtractItem( CDnSlotButton *pDragButton )
{


	CDnItem *pItem = static_cast<CDnItem *>(pDragButton->GetItem());

	// 올린거 또 올리는거면 패스
	if( m_pButtonQuickSlot1 && (pItem == m_pButtonQuickSlot1->GetItem()) ) {
		return false;
	}

	// 아이템 종류 검사
	switch( pItem->GetItemType() ) {
		case ITEMTYPE_WEAPON:
		case ITEMTYPE_PARTS:
			break;
		default:
			GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 805111112), MB_OK );
			return false;
	}
	if( pItem->GetTypeParam(0) == 0 ) {
		GetInterface().MessageBox( 8083, MB_OK );
		return false;
	}

	// 테이블 확인



	int nPotentialMaxMoveCount = (int)CGlobalWeightTable::GetInstance().GetValue( CGlobalWeightTable::PotentialMovealbleCount );
	int nRemain = nPotentialMaxMoveCount - pItem->GetPotentialMoveCount();
	if( nRemain <= 0 )
	{
		GetInterface().MessageBox( 8088, MB_OK );
		return false;
	}
	if( pItem->IsSoulbBound() == false ) {
		GetInterface().MessageBox(805111113, MB_OK );
		return false;
	}

	return true;
}

bool CDnPotentialTransferDlg::IsPotentialableItem( CDnSlotButton *pDragButton )
{


	CDnItem *pItem = static_cast<CDnItem *>(pDragButton->GetItem());

	// 올린거 또 올리는거면 패스
	if( m_pButtonQuickSlot2 && (pItem == m_pButtonQuickSlot2->GetItem()) ) {
		return false;
	}

	// 아이템 종류 검사
	switch( pItem->GetItemType() ) {
		case ITEMTYPE_WEAPON:
		case ITEMTYPE_PARTS:
			break;
		default:
			GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 805111115), MB_OK );
			return false;
	}
	if( pItem->GetTypeParam(0) == 0 ) {
		GetInterface().MessageBox( 80841111, MB_OK );
		return false;
	}
#if defined(PRE_ADD_PCBANG_RENTAL_ITEM)
	if( pItem->IsPcCafeRentItem() )
	{
		GetInterface().MessageBox( 8084, MB_OK );
		return false;
	}
#endif

#ifdef PRE_MOD_75892
#else		//#ifdef PRE_MOD_75892
	if( pItem->GetPotentialIndex() > 0 ) {
		GetInterface().MessageBox( 8081, MB_OK );
		return false;
	}
#endif		//#ifdef PRE_MOD_75892
	if( pItem->IsSoulbBound() == false ) {
		GetInterface().MessageBox(805111116, MB_OK );
		return false;
	}

	if( !m_pItem1 )
		return false;

	if( m_pItem1->GetItemType() != pItem->GetItemType() ) {
		GetInterface().MessageBox(80841112, MB_OK );
		return false;
	}

	bool bCheck = true;
	int nMainType = (int)m_pItem1->GetItemType();
	int nSubType = -1;
	bool bCheckNeedJob = true;
	if( m_pItem1->GetItemType() == ITEMTYPE_WEAPON ) {
		CDnWeapon *pWeapon1 = dynamic_cast<CDnWeapon *>(m_pItem1);
		CDnWeapon *pWeapon2 = dynamic_cast<CDnWeapon *>(pItem);
		if( pWeapon1 == NULL || pWeapon2 == NULL ) {
			bCheck = false;
		}
		else {
			if( pWeapon1->GetEquipType() != pWeapon2->GetEquipType() ) bCheck = false;
			nSubType = (int)pWeapon1->GetEquipType();
		}
	}
	else if( m_pItem1->GetItemType() == ITEMTYPE_PARTS ) {
		CDnParts *pParts1 = dynamic_cast<CDnParts *>(m_pItem1);
		CDnParts *pParts2 = dynamic_cast<CDnParts *>(pItem);
		if( pParts1 == NULL || pParts2 == NULL ) {
			bCheck = false;
		}
		else {
			if( pParts1->GetPartsType() != pParts2->GetPartsType() ) bCheck = false;
			nSubType = (int)pParts1->GetPartsType();
			if( nSubType >= CDnParts::Necklace && nSubType <= CDnParts::Ring2 ) bCheckNeedJob = false;
		}
	}

	if( m_pItem1->GetItemRank() != pItem->GetItemRank() ) {
		GetInterface().MessageBox(80841113, MB_OK );
		return false;
	}

	if( bCheckNeedJob ) {
		if( m_pItem1->GetNeedJobCount() != pItem->GetNeedJobCount() ) {
			GetInterface().MessageBox(80841114, MB_OK );
			return false;
		}

		for( int i = 0; i < m_pItem1->GetNeedJobCount(); ++i ) {
			if( m_pItem1->GetNeedJob(i) != pItem->GetNeedJob(i) ) {
				GetInterface().MessageBox(80841115, MB_OK );
				return false;
			}
		}
	}

	if( !bCheck ) {
		GetInterface().MessageBox(80841116, MB_OK );
		return false;
	}

	// 여기서 테이블 확인
	int nNeedCount = 20;
	if( nNeedCount <= 0 ) {
		GetInterface().MessageBox(80841117, MB_OK );
		return false;
	}

	return true;
}

int CDnPotentialTransferDlg::GetPotentialTransferItemCount( int nMainType, int nSubType, int nItemLevel1, int nItemRank1, int nItemLevel2, int nItemRank2 )
{
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TPOTENTIAL_TRANS );
	if( !pSox )
	{
		_ASSERT(0);
		return 0;
	}

	for (int i = 0; i < (int)pSox->GetItemCount(); i++)
	{
		int nItemID = pSox->GetItemID( i );
		int nTableMainType = pSox->GetFieldFromLablePtr( nItemID, "_MainType" )->GetInteger();
		int nTableSubType = pSox->GetFieldFromLablePtr( nItemID, "_SubType" )->GetInteger();
		int nLevel1 = pSox->GetFieldFromLablePtr( nItemID, "_FromLevel" )->GetInteger();
		int nRank1 = pSox->GetFieldFromLablePtr( nItemID, "_FromGrade" )->GetInteger();
		int nLevel2 = pSox->GetFieldFromLablePtr( nItemID, "_ToLevel" )->GetInteger();
		int nRank2 = pSox->GetFieldFromLablePtr( nItemID, "_ToGrade" )->GetInteger();

		if( nMainType == nTableMainType && nSubType == nTableSubType && nLevel1 == nItemLevel1 && nRank1 == nItemRank1 && nLevel2 == nItemLevel2 && nRank2 == nItemRank2 )
			return pSox->GetFieldFromLablePtr( nItemID, 20 )->GetInteger();
	}

	return 0;
}

void CDnPotentialTransferDlg::RefreshPotentialTransfer()
{
	m_pButtonApply->Enable( false );
	if( m_pItemSlot1 && !m_pItemSlot1->IsEmptySlot() && m_pItem1 &&
		m_pItemSlot2 && !m_pItemSlot2->IsEmptySlot() && m_pItem2 )
	{
		int nMainType = (int)m_pItem1->GetItemType();;
		int nSubType = -1;
		if( m_pItem1->GetItemType() == ITEMTYPE_WEAPON ) {
			CDnWeapon *pWeapon1 = dynamic_cast<CDnWeapon *>(m_pItem1);
			nSubType = (int)pWeapon1->GetEquipType();
		}
		else if( m_pItem1->GetItemType() == ITEMTYPE_PARTS ) {
			CDnParts *pParts1 = dynamic_cast<CDnParts *>(m_pItem1);
			nSubType = (int)pParts1->GetPartsType();
		}

		int nNeedCount = 20;
		if( nNeedCount )
		{
			// 다 통과했으면
			bool bCheckNeedItem = false;
			int nItemID = 0;
			int nTotalCount = 0;
			nItemID = POTENTIAL_EXTRACTOR_ITEMID;
			nTotalCount = GetItemTask().FindItemCountFromItemType( ITEMTYPE_POTENTIAL_EXTRACTOR );
			if( nTotalCount >= nNeedCount )
				bCheckNeedItem = true;

			TItemInfo itemInfo;	
			if( CDnItem::MakeItemInfo( nItemID, nTotalCount, itemInfo ) )
			{
				SAFE_DELETE( m_pTransItem );
				m_pTransItem = GetItemTask().CreateItem( itemInfo );
				m_pTransItemSlot->SetItem( m_pTransItem, CDnSlotButton::ITEM_ORIGINAL_COUNT );
			}

			WCHAR wszTemp[64] = {0,};
			swprintf_s( wszTemp, _countof(wszTemp), L"%d%s", nNeedCount, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1728 ) );
			m_pStaticCount->SetText( wszTemp );

			if( bCheckNeedItem )
			{
				m_pStaticCount->SetTextColor( m_dwCountColor );
				m_pButtonApply->Enable( true );
			}
			else
			{
				m_pStaticCount->SetTextColor( textcolor::RED );
			}
		}
	}
	else
	{
		m_pTransItemSlot->ResetSlot();
		m_pStaticCount->SetText( L"" );
	}
}

void CDnPotentialTransferDlg::OnRecvExchangePotential( int nResult )
{
	if( nResult == ERROR_NONE )
	{
		if (m_nSoundIdx >= 0)
			CEtSoundEngine::GetInstance().PlaySound( "2D", m_nSoundIdx, false );
		Show( false );
	}
}

void CDnPotentialTransferDlg::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg )
{
#ifdef PRE_ADD_INSTANT_CASH_BUY
	if( pControl == NULL && nID == INSTANT_CASHSHOP_BUY_DIALOG )
	{
		RefreshPotentialTransfer();

		if( nCommand == 1 )
			Show( false );

		return;
	}
#endif // PRE_ADD_INSTANT_CASH_BUY

	if( pControl )
		SetCmdControlName( pControl->GetControlName() );

	switch( nID ) 
	{
	case 0:
		if( nCommand == EVENT_BUTTON_CLICKED )
		{
			if( IsCmdControl("ID_YES") )
			{
				SendExchangePotential( m_pButtonQuickSlot1->GetSlotIndex(), m_pItem1->GetSerialID(), m_pButtonQuickSlot2->GetSlotIndex(), m_pItem2->GetSerialID() );
				m_pButtonApply->Enable( false );
			}
		}
		break;
	}
}

#endif