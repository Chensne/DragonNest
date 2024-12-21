#include "StdAfx.h"
#include "DnMarketPriceDlg.h"
#include "DnTradeTask.h"
#include "DnTableDB.h"
#include "DnInterfaceString.h"
#include "DnInterface.h"
#include "DnItemTask.h"
#include "DnStoreConfirmExDlg.h"
#include "DnCommonUtil.h"
#include "DnMainMenuDlg.h"
#include "DnMarketTabDlg.h"
#include "DnCashShopTask.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnMarketPriceDlg::CDnMarketPriceDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
	: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback , true )
	, m_MoneyControl( this )
	, m_pItem( NULL )
	, m_pSlotButton( NULL )
	, m_nRegistTax( -1 )
	, m_pSplitConfirmExDlg(NULL)
	, m_bPremium( false )
{
}

CDnMarketPriceDlg::~CDnMarketPriceDlg(void)
{
	SAFE_DELETE(m_pSplitConfirmExDlg);
}

void CDnMarketPriceDlg::Initialize( bool bShow )
{
#if defined(PRE_ADD_PETALTRADE)
	BaseClass::Initialize( CEtResourceMng::GetInstance().GetFullName( "MarketPriceDlg_wPetal.ui" ).c_str(), bShow );
#else
	BaseClass::Initialize( CEtResourceMng::GetInstance().GetFullName( "MarketPriceDlg.ui" ).c_str(), bShow );
#endif	//#if defined(PRE_ADD_PETALTRADE)
}

void CDnMarketPriceDlg::InitialUpdate()
{
	DN_CONTROL( ItemSlotButton , ID_BUTTON_ITEMSLOT )->SetSlotIndex( 0 );
	DN_CONTROL( ItemSlotButton , ID_BUTTON_ITEMSLOT )->SetSlotType( ST_MARKET_REGIST );

	m_MoneyControl.InitialUpdate( true );
	m_MoneyControl.SetMaxMoney( 1000000000 );
	m_MoneyControl.SetMaxPetal( 1000000000 );

	// 최종 빌드에선 하루만 일단 가능하고, 그외엔 캐쉬템 검색해서 날짜를 추가한다...
	int i = CGlobalWeightTable::MarketRegisterTime_Min;
	for (; i < CGlobalWeightTable::MarketRegisterTime_Max; ++i)
	{
		int value = (int)CGlobalWeightTable::GetInstance().GetValue((CGlobalWeightTable::WeightTableIndex)i);
		if (value > 0)
			CONTROL( ComboBox, ID_COMBOBOX_TIME )->AddItem( FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4075 ), value ).c_str(), NULL, i );
	}

	m_TaxColor[ 0 ] = CONTROL( Static, ID_GOLD )->GetTextColor();
	m_TaxColor[ 1 ] = CONTROL( Static, ID_SILVER )->GetTextColor();
	m_TaxColor[ 2 ] = CONTROL( Static, ID_BRONZE )->GetTextColor();

	CONTROL( Static, ID_STATIC_TAX)->SetTooltipText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4059 ) );

	m_pSplitConfirmExDlg = new CDnStoreConfirmExDlg( UI_TYPE_MODAL, NULL, ITEM_ATTACH_CONFIRM_DIALOG, this );
	m_pSplitConfirmExDlg->Initialize( false );

	CONTROL( Static, ID_PRIMIUM_BOX )->Show( false );

	CONTROL( Static, ID_AVERAGE_GOLD )->Show( true );
	CONTROL( Static, ID_AVERAGE_SILVER )->Show( true );
	CONTROL( Static, ID_AVERAGE_BRONZE )->Show( true );
	CONTROL( Static, ID_STATIC_BOARD )->Show( true );

#if defined(PRE_ADD_PETALTRADE)
	CONTROL( Static, ID_AVERAGE_PETALBASE )->Show( false );
	CONTROL( Static, ID_TEXT_PETAL )->Show( false );
#endif
	ResetMarketPrice();

#if defined(PRE_ADD_PETALTRADE)
	CONTROL( CheckBox, ID_CHECKBOX_GOLD )->SetChecked( true );
	CONTROL( CheckBox, ID_CHECKBOX_PETAL )->SetChecked( false );
#endif	//#if defined(PRE_ADD_PETALTRADE)
}

void CDnMarketPriceDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( !m_MoneyControl.ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg ) ) {
		return;
	}

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl( "ID_BUTTON_REGIST" ) && m_pSlotButton && ( m_MoneyControl.GetMoney() > 0 || m_MoneyControl.GetPetal() ) )
		{			

			wchar_t szString[32]={0,};
			INT64 nGold=0, nSilver=0,nBronze=0;
			CDnMoneyControl::GetStrMoneyG( m_nRegistTax, szString, &nGold);
			CDnMoneyControl::GetStrMoneyS( m_nRegistTax, szString, &nSilver);
			CDnMoneyControl::GetStrMoneyC( m_nRegistTax, szString, &nBronze);

#if defined(PRE_ADD_PETALTRADE)
			if( CONTROL( CheckBox, ID_CHECKBOX_PETAL )->IsChecked() )
			{
#if defined(PRE_FIX_34367)	
				if( m_pItem->GetItemType() == ITEMTYPE_PETALTOKEN )
				{
					GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4870 ), MB_OK );	// UISTRING : 페탈 상품권의 구입조건을 페탈로 설정 할 수 없습니다.
					return;
				}				
#endif
				GetInterface().MessageBox( 4159, MB_YESNO, MESSAGEBOX_REGIST_ITEM, this );
				return;
			}
#endif	//#if defined(PRE_ADD_PETALTRADE)
			if( m_bPremium )
			{
				GetInterface().MessageBox( 4588, MB_YESNO, MESSAGEBOX_REGIST_ITEM, this );
			}
			else
			{
				std::wstring wszMsg = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, MESSAGEBOX_REGIST_ITEM ) , 
					(int)nGold, (int)nSilver, (int)nBronze  );
				GetInterface().MessageBox( wszMsg.c_str(), MB_YESNO, MESSAGEBOX_REGIST_ITEM, this );
			}

			return;
		}

		if( IsCmdControl( "ID_BUTTON_CANCEL" ) || IsCmdControl( "ID_BUTTON_CLOSE" ) )
		{
			Show( false );
			return;
		}

		if( IsCmdControl("ID_BUTTON_ITEMSLOT") )
		{
			CDnSlotButton *pDragButton = ( CDnSlotButton * )drag::GetControl();

			// 우클릭으로 빼기
			if( uMsg == WM_RBUTTONUP )
			{
				CancelMarketItem();
				return;
			}

			if( pDragButton )
			{
				pDragButton->DisableSplitMode(false);
				drag::ReleaseControl();
				if( PickUpMarketItem( (CDnQuickSlotButton*)pDragButton ) == false )
					pDragButton->DisableSplitMode(true);
			}
		}
	}
	else if( nCommand == EVENT_CHECKBOX_CHANGED )
	{
		if( IsCmdControl("ID_CHECKBOX_GOLD") )
		{
			bool bIsChecked = CONTROL( CheckBox, ID_CHECKBOX_GOLD )->IsChecked();

			if( bIsChecked )
			{
				CONTROL( CheckBox, ID_CHECKBOX_PETAL )->SetChecked( false );
				m_MoneyControl.SetMoneyType( CDnMoneyControl::GOLD_TYPE );

				CONTROL( Static, ID_AVERAGE_GOLD )->Show( true );
				CONTROL( Static, ID_AVERAGE_SILVER )->Show( true );
				CONTROL( Static, ID_AVERAGE_BRONZE )->Show( true );
				CONTROL( Static, ID_STATIC_BOARD )->Show( true );

				CONTROL( Static, ID_AVERAGE_PETALBASE )->Show( false );
				CONTROL( Static, ID_TEXT_PETAL )->Show( false );
				CONTROL( Static, ID_STATIC0 )->Show( false );
				RequestMarketPrice();
			}
		}
		else if( IsCmdControl("ID_CHECKBOX_PETAL") )
		{
			bool bIsChecked = CONTROL( CheckBox, ID_CHECKBOX_PETAL )->IsChecked();

			if( bIsChecked )
			{
				CONTROL( CheckBox, ID_CHECKBOX_GOLD )->SetChecked( false );
				m_MoneyControl.SetMoneyType( CDnMoneyControl::PETAL_TYPE );

				CONTROL( Static, ID_AVERAGE_GOLD )->Show( false );
				CONTROL( Static, ID_AVERAGE_SILVER )->Show( false );
				CONTROL( Static, ID_AVERAGE_BRONZE )->Show( false );
				CONTROL( Static, ID_STATIC_BOARD )->Show( false );

				CONTROL( Static, ID_AVERAGE_PETALBASE )->Show( true );
				CONTROL( Static, ID_TEXT_PETAL )->Show( true );
				CONTROL( Static, ID_STATIC0 )->Show( true );
				RequestMarketPrice();
			}
		}
	}

	BaseClass::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnMarketPriceDlg::SetPremium( bool bPremium )
{
	CONTROL( Static , ID_STATIC_PRM )->Show(bPremium);
	m_bPremium = bPremium;
}

void CDnMarketPriceDlg::Show( bool bShow )
{ 
	if( m_bShow == bShow )
		return;

	if( bShow )
	{
		CONTROL( ComboBox, ID_COMBOBOX_TIME )->RemoveAllItems();

		int i = CGlobalWeightTable::MarketRegisterTime_Min;
		for (; i < CGlobalWeightTable::MarketRegisterTime_Max; ++i)
		{
			int value = (int)CGlobalWeightTable::GetInstance().GetValue((CGlobalWeightTable::WeightTableIndex)i);
			if (value > 0)
				CONTROL( ComboBox, ID_COMBOBOX_TIME )->AddItem( FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4075 ), value ).c_str(), NULL, i );
		}

		//콤보 박스 초기화
		if( m_bPremium )
		{
			int i = CGlobalWeightTable::MarketPremiumTime_Min;
			for(; i < CGlobalWeightTable::MarketPremiumTime_Max; ++i )
			{
				int value = (int)CGlobalWeightTable::GetInstance().GetValue((CGlobalWeightTable::WeightTableIndex)i);
				if (value > 0)
					CONTROL( ComboBox, ID_COMBOBOX_TIME )->AddItem( FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4094 ), value ).c_str(), NULL, i );
			}
			CONTROL( ComboBox, ID_COMBOBOX_TIME )->SetSelectedByIndex( CONTROL( ComboBox, ID_COMBOBOX_TIME )->GetItemCount() - 1 );
		}
		ResetMarketPrice();
	}
	else
	{
		CancelMarketItem();
		CDnMarketTabDlg* pMarketTabDlg = (CDnMarketTabDlg*)GetInterface().GetMainMenuDialog(CDnMainMenuDlg::MARKET_DIALOG);
		if( pMarketTabDlg ) {
			pMarketTabDlg->EnableAllControl( true );
			pMarketTabDlg->EnableChildDlg( true );
		}
		m_nRegistTax = -1;
		m_pItem = NULL;
	}

	m_MoneyControl.Show( bShow );

	BaseClass::Show( bShow );
}

bool CDnMarketPriceDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsShow() )
		return false;

	if( uMsg == WM_KEYDOWN )
	{
		if( wParam == VK_ESCAPE )
		{
			Show(false);
			return true;
		}
		else if( wParam == VK_RETURN )
		{
			if( m_MoneyControl.IsEditBoxFocus() )
			{
				RequestFocus( CONTROL( Button, ID_BUTTON_REGIST ) );
				return true;
			}
		}
	}

	if( m_MoneyControl.MsgProc( hWnd, uMsg, wParam, lParam ) ) {
		return true;
	}

	return BaseClass::MsgProc( hWnd, uMsg, wParam, lParam );
}

void CDnMarketPriceDlg::CalcRegistrationFee()
{
	if( CDnMoneyControl::GOLD_TYPE == m_MoneyControl.GetMoneyType() )
	{
		CONTROL( Button, ID_BUTTON_REGIST )->Enable( true );

		if( m_MoneyControl.GetMoney() == 0 ) {
			CONTROL( Button, ID_BUTTON_REGIST )->Enable(  false  );
		}

		if( DN_CONTROL( ItemSlotButton , ID_BUTTON_ITEMSLOT )->GetItem()  ) {
			int nMaxOverlapCount = DN_CONTROL( ItemSlotButton , ID_BUTTON_ITEMSLOT )->GetItem()->GetOverlapCount();
			int nOverlapCount = DN_CONTROL( ItemSlotButton , ID_BUTTON_ITEMSLOT )->GetRenderCount();		
			if( nOverlapCount > nMaxOverlapCount ) {
				CONTROL( Button, ID_BUTTON_REGIST )->Enable(  false  );
			}
		}

		if( !CDnActor::s_hLocalActor ) return;
		if( !CDnItemTask::IsActive() ) return;

		TAX_TYPE nTax;
		if( m_bPremium )
			nTax = CommonUtil::CalcTax(TAX_TRADEREGISTER_CASH, CDnActor::s_hLocalActor->GetLevel(), m_MoneyControl.GetMoney() );
		else
			nTax = EtMax( 1, CommonUtil::CalcTax(TAX_TRADEREGISTER, CDnActor::s_hLocalActor->GetLevel(), m_MoneyControl.GetMoney() ));

		if( nTax > GetItemTask().GetCoin() ) {
			CONTROL( Static, ID_GOLD )->SetTextColor( textcolor::RED );
			CONTROL( Static, ID_SILVER )->SetTextColor( textcolor::RED );
			CONTROL( Static, ID_BRONZE )->SetTextColor( textcolor::RED );
			CONTROL( Button, ID_BUTTON_REGIST )->Enable( false );
		}
		else {
			CONTROL( Static, ID_GOLD )->SetTextColor( m_TaxColor[0] );
			CONTROL( Static, ID_SILVER )->SetTextColor( m_TaxColor[1] );
			CONTROL( Static, ID_BRONZE )->SetTextColor( m_TaxColor[2] );
		}

		wchar_t wszMoney[16]={0,};
		std::wstring strString;
		INT64 nResultMoney=0;
		CDnMoneyControl::GetStrMoneyG( nTax, wszMoney, &nResultMoney );		
		DN_INTERFACE::UTIL::GetValue_2_String( (int)nResultMoney, strString );
		CONTROL( Static, ID_GOLD )->SetTooltipText( strString.c_str() );
		CONTROL( Static, ID_GOLD )->SetText( wszMoney );

		CDnMoneyControl::GetStrMoneyS( nTax, wszMoney, &nResultMoney );		
		DN_INTERFACE::UTIL::GetValue_2_String( (int)nResultMoney, strString );
		CONTROL( Static, ID_SILVER )->SetTooltipText( strString.c_str() );
		CONTROL( Static, ID_SILVER )->SetText( wszMoney );

		CDnMoneyControl::GetStrMoneyC( nTax, wszMoney, &nResultMoney );		
		DN_INTERFACE::UTIL::GetValue_2_String( (int)nResultMoney, strString );
		CONTROL( Static, ID_BRONZE )->SetTooltipText( strString.c_str() );
		CONTROL( Static, ID_BRONZE )->SetText( wszMoney );

		m_nRegistTax = nTax;	
	}
	else	// Petal
	{
		CONTROL( Button, ID_BUTTON_REGIST )->Enable( true );

		if( m_MoneyControl.GetPetal() == 0 ) {
			CONTROL( Button, ID_BUTTON_REGIST )->Enable(  false  );
		}

		if( DN_CONTROL( ItemSlotButton , ID_BUTTON_ITEMSLOT )->GetItem()  ) {
			int nMaxOverlapCount = DN_CONTROL( ItemSlotButton , ID_BUTTON_ITEMSLOT )->GetItem()->GetOverlapCount();
			int nOverlapCount = DN_CONTROL( ItemSlotButton , ID_BUTTON_ITEMSLOT )->GetRenderCount();		
			if( nOverlapCount > nMaxOverlapCount ) {
				CONTROL( Button, ID_BUTTON_REGIST )->Enable(  false  );
			}
		}

		if( !CDnActor::s_hLocalActor ) return;
		if( !CDnItemTask::IsActive() ) return;
	}

#if defined (PRE_ADD_PETALTRADE)
	if( !CONTROL( CheckBox, ID_CHECKBOX_PETAL )->IsChecked() && !CONTROL( CheckBox, ID_CHECKBOX_GOLD )->IsChecked() )
		CONTROL( Button, ID_BUTTON_REGIST )->Enable(  false  );
#endif	//#if defined (PRE_ADD_PETALTRADE)
}

void CDnMarketPriceDlg::Process( float fElapsedTime )
{	
	CalcRegistrationFee();
	BaseClass::Process( fElapsedTime );	
}

bool CDnMarketPriceDlg::PickUpMarketItem( CDnQuickSlotButton *pPressedButton, bool bSplit )
{
	CDnItem *pItem = static_cast<CDnItem *>(pPressedButton->GetItem());

	if( !pItem ) 
		return false;
	if( pPressedButton->GetSlotType() != ST_INVENTORY && pPressedButton->GetSlotType() != ST_INVENTORY_CASH )
	{
		GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4035 ), MB_OK );	// UISTRING : 거래가 불가능한 아이템 입니다.
		return false;
	}
	// 올린거 또 올리는거면 패스
	if( m_pSlotButton && (pItem == m_pSlotButton->GetItem()) )
	{
		return true;
	}
	if( pPressedButton->IsRegist() ) 
	{
		return false;
	}
	// 아이템 종류 검사
	if( pItem->GetItemType() == ITEMTYPE_QUEST )
	{
		GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3609 ), MB_OK );	// UISTRING : 이동이 불가능한 아이템 입니다.
		return false;
	}

	if ( !pItem->IsEternityItem() )
	{
		GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4082 ), MB_OK ); // UISTRING : 기간제 아이템은 등록 할 수 없습니다.
		return false;
	}

#ifdef PRE_ADD_ONESTOP_TRADECHECK
	if (CDnTradeTask::IsActive() && CDnTradeTask::GetInstance().IsTradable(CDnTradeTask::eTRADECHECK_MARKET, *pItem) == false)
	{
		GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4035 ), MB_OK );	// UISTRING : 거래가 불가능한 아이템입니다
		return false;
	}
#endif

	if (pItem->GetReversion() > CDnItem::NoReversion && pItem->IsSoulbBound() && pItem->GetReversion() != CDnItem::Trade )
	{
		if (pItem->IsCashItem())
		{
			GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4035 ), MB_OK );	// UISTRING : 거래가 불가능한 아이템입니다
		}
		else
		{
			// todo by kalliste : Refatoring with MailWriteDlg/Trade
			eItemTypeEnum type = pItem->GetItemType();
			if (type == ITEMTYPE_WEAPON || 
				type == ITEMTYPE_PARTS ||
				type == ITEMTYPE_GLYPH ||
				type == ITEMTYPE_POTENTIAL_JEWEL)
			{
				if (pItem->GetSealCount() > 0)
				{
					GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3626 ), MB_OK ); // UISTRING : 재봉인 가능한 귀속 아이템을 거래 하려면 봉인의 인장으로 봉인해야 합니다
					return false;
				}
			}

			GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4035 ), MB_OK ); // UISTRING : 거래가 불가능한 아이템입니다
		}
		return false;
	}

	if (CDnCashShopTask::GetInstance().IsCashTradeCountRestrict())
	{
		if ((pItem->GetReversion() == CDnItem::Trade) && (pItem->GetSealCount() == 0)){
			GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4035 ), MB_OK ); // UISTRING : 거래가 불가능한 아이템입니다
			return false;
		}
	}

	if ( pPressedButton->GetRenderCount() > 1 && bSplit && !pItem->IsCashItem() ) {
		m_pSplitConfirmExDlg->SetItem( pPressedButton , CDnInterface::MARKET_REGISTER);
		m_pSplitConfirmExDlg->Show(true);
		return true;
	}

	// 칸이 하나이므로 더 이상 등록할 수 없습니다.보단 재등록이 낫다.
	CDnItem *pOriginItem = static_cast<CDnItem *>(DN_CONTROL( ItemSlotButton , ID_BUTTON_ITEMSLOT )->GetItem());
	if( pOriginItem )
	{
		// 해당 아이템을 창에서 제거한다.
		SAFE_DELETE(m_pItem);
		DN_CONTROL( ItemSlotButton , ID_BUTTON_ITEMSLOT )->ResetSlot();
		m_pSlotButton->SetRegist( false );
		m_pSlotButton->DisableSplitMode(true);
		m_pSlotButton = NULL;
	}
	
	m_pSlotButton = (CDnSlotButton*)pPressedButton;
	m_pSlotButton->SetRegist( true );

	// 플레이트를 등록할때 전처럼 이렇게 인벤토리 아이템의 포인터를 직접 넘기지 않고,
	//pPressedButton->SetItem( pItem );
	// 이렇게 임시템을 만들어서 넣어준다.
	TItemInfo itemInfo;
	pItem->GetTItemInfo(itemInfo);

	m_pItem = GetItemTask().CreateItem(itemInfo);
	m_pItem->SetSerialID( pItem->GetSerialID() );
	pPressedButton->EnableSplitMode(CDnSlotButton::ITEM_ORIGINAL_COUNT);
	DN_CONTROL( ItemSlotButton , ID_BUTTON_ITEMSLOT )->SetItem(m_pItem, CDnSlotButton::ITEM_ORIGINAL_COUNT );

	CEtSoundEngine::GetInstance().PlaySound( "2D", pItem->GetDragSoundIndex() );

	CONTROL( Static, ID_ITEM_NAME )->SetText( pItem->GetName() );

	m_MoneyControl.SetMoney( m_pItem->GetItemSellAmount() * itemInfo.Item.wCount );

	RequestMarketPrice();

	return true;
}

void CDnMarketPriceDlg::CancelMarketItem()
{
	SAFE_DELETE(m_pItem);
	DN_CONTROL( ItemSlotButton , ID_BUTTON_ITEMSLOT )->ResetSlot();
	if( m_pSlotButton )
	{
		m_pSlotButton->SetRegist( false );
		m_pSlotButton->DisableSplitMode(true);
		m_pSlotButton = NULL;
	}
	CONTROL( Static, ID_ITEM_NAME )->SetText( L"" );
	ResetMarketPrice();
}

void CDnMarketPriceDlg::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED ) {
		switch( nID ) {
		case ITEM_ATTACH_CONFIRM_DIALOG:
			{
				if (IsCmdControl("ID_OK")) {
					CDnItem* pItem = m_pSplitConfirmExDlg->GetItem();
					if (pItem) {
						int nCount = m_pSplitConfirmExDlg->GetItemCount();
						if (nCount <= 0) {
							m_pSplitConfirmExDlg->Show( false );
							return;
						}
						CDnItem *pOriginItem = static_cast<CDnItem *>(DN_CONTROL( ItemSlotButton , ID_BUTTON_ITEMSLOT )->GetItem());
						if( pOriginItem && m_pSlotButton )
						{
							// 해당 아이템을 창에서 제거한다.
							SAFE_DELETE(m_pItem);
							DN_CONTROL( ItemSlotButton , ID_BUTTON_ITEMSLOT )->ResetSlot();
							m_pSlotButton->SetRegist( false );
							m_pSlotButton->DisableSplitMode(true);
							m_pSlotButton = NULL;
						}

						TItemInfo itemInfo;
						pItem->GetTItemInfo(itemInfo);
						m_pItem = GetItemTask().CreateItem(itemInfo);
						DN_CONTROL( ItemSlotButton , ID_BUTTON_ITEMSLOT )->SetItem( m_pItem, nCount );

						m_pSlotButton = m_pSplitConfirmExDlg->GetFromSlot();
						m_pSlotButton->SetRegist( true );
						m_pSlotButton->EnableSplitMode(nCount);

						CONTROL( Static, ID_ITEM_NAME )->SetText( pItem->GetName() );

						m_MoneyControl.SetMoney( m_pItem->GetItemSellAmount() * nCount );
						CEtSoundEngine::GetInstance().PlaySound( "2D", pItem->GetDragSoundIndex() );
						RequestMarketPrice();
					}
					m_pSplitConfirmExDlg->Show( false );
				}
				else if ( IsCmdControl("ID_CANCEL") ) {
					m_pSplitConfirmExDlg->Show( false );					
				}
			}
			break;
		case MESSAGEBOX_REGIST_ITEM:
			if (IsCmdControl("ID_YES")) {	
				ProcessRegister();
			}
			break;
		}
	}
}

void CDnMarketPriceDlg::ProcessRegister()
{
	int nSellingPeriod = -1;
	bool bResult = CONTROL( ComboBox, ID_COMBOBOX_TIME )->GetSelectedValue( nSellingPeriod );

	int nOverlapCount = DN_CONTROL( ItemSlotButton , ID_BUTTON_ITEMSLOT )->GetRenderCount();
	int nMaxOverlapCount = DN_CONTROL( ItemSlotButton , ID_BUTTON_ITEMSLOT )->GetItem()->GetOverlapCount();

	if( nOverlapCount > nMaxOverlapCount ) {
		return;
	}

	if( bResult && nSellingPeriod != -1 ) {

#if defined (PRE_ADD_PETALTRADE)
		bool bGold = CONTROL( CheckBox, ID_CHECKBOX_GOLD )->IsChecked();
#else
		bool bGold = true;
#endif  // #if defined (PRE_ADD_PETALTRADE)

		int  nMoney = bGold ? (int)m_MoneyControl.GetMoney() : (int)m_MoneyControl.GetPetal();
		char cPayMethodCode = bGold ? 1 : 3;

		GetTradeTask().GetTradeMarket().RequestMarketRegister( 
			m_pItem->GetSerialID(),
			nOverlapCount, 
			nMoney, 
			nSellingPeriod, 
			m_pSlotButton->GetSlotType() == ST_INVENTORY_CASH ? ITEMPOSITION_CASHINVEN :ITEMPOSITION_INVEN,
			m_pSlotButton->GetSlotType() == ST_INVENTORY_CASH ? 0 : m_pSlotButton->GetSlotIndex(),
			m_bPremium,
			cPayMethodCode );
		Show( false );
	}
}

void CDnMarketPriceDlg::RequestMarketPrice()
{
	if( !m_pItem )
		return;

	GetTradeTask().GetTradeMarket().RequestMarketPrice( -1, m_pItem->GetClassID(), m_pItem->GetEnchantLevel(), m_pItem->GetOptionIndex() );
}

void CDnMarketPriceDlg::OnRecvMarketPrice( int nMarketDBID, std::vector< TMarketPrice > & vecPrice )
{
	wchar_t szString[32]={0,};

#if defined (PRE_ADD_PETALTRADE)
	if( CONTROL( CheckBox, ID_CHECKBOX_GOLD )->IsChecked() )
	{
		INT64 nGold=0, nSilver=0,nBronze=0;

		CDnMoneyControl::GetStrMoneyG( (INT64)vecPrice[0].nAvgPrice, szString, &nGold );
		CDnMoneyControl::GetStrMoneyS( (INT64)vecPrice[0].nAvgPrice, szString, &nSilver );
		CDnMoneyControl::GetStrMoneyC( (INT64)vecPrice[0].nAvgPrice, szString, &nBronze );

		CONTROL( Static, ID_AVERAGE_GOLD )->SetIntToText( (int)nGold );
		CONTROL( Static, ID_AVERAGE_SILVER )->SetIntToText( (int)nSilver );
		CONTROL( Static, ID_AVERAGE_BRONZE )->SetIntToText( (int)nBronze );
	}
	else
	{
#ifdef PRE_MOD_PETAL_WRITE
		CONTROL( Static, ID_TEXT_PETAL )->SetText( DN_INTERFACE::UTIL::GetAddCommaString( vecPrice[1].nAvgPrice ).c_str() );
#else // PRE_MOD_PETAL_WRITE
		INT64 nPetal = 0;	
		CDnMoneyControl::GetStrMoneyP( (INT64)vecPrice[1].nAvgPrice, szString, &nPetal );
		CONTROL( Static, ID_TEXT_PETAL )->SetIntToText( (int)nPetal );
#endif // PRE_MOD_PETAL_WRITE
	}
#else
	INT64 nGold=0, nSilver=0,nBronze=0;

	CDnMoneyControl::GetStrMoneyG( (INT64)vecPrice[0].nAvgPrice, szString, &nGold );
	CDnMoneyControl::GetStrMoneyS( (INT64)vecPrice[0].nAvgPrice, szString, &nSilver );
	CDnMoneyControl::GetStrMoneyC( (INT64)vecPrice[0].nAvgPrice, szString, &nBronze );

	CONTROL( Static, ID_AVERAGE_GOLD )->SetIntToText( (int)nGold );
	CONTROL( Static, ID_AVERAGE_SILVER )->SetIntToText( (int)nSilver );
	CONTROL( Static, ID_AVERAGE_BRONZE )->SetIntToText( (int)nBronze );
#endif
}

void CDnMarketPriceDlg::ResetMarketPrice()
{
#if defined (PRE_ADD_PETALTRADE)
	if( CONTROL( CheckBox, ID_CHECKBOX_GOLD )->IsChecked() )
	{
		CONTROL( Static, ID_AVERAGE_GOLD )->SetIntToText( 0 );
		CONTROL( Static, ID_AVERAGE_SILVER )->SetIntToText( 0 );
		CONTROL( Static, ID_AVERAGE_BRONZE )->SetIntToText( 0 );
	}
	else
	{
		CONTROL( Static, ID_TEXT_PETAL )->SetIntToText( 0 );
	}
#else

	CONTROL( Static, ID_AVERAGE_GOLD )->SetIntToText( 0 );
	CONTROL( Static, ID_AVERAGE_SILVER )->SetIntToText( 0 );
	CONTROL( Static, ID_AVERAGE_BRONZE )->SetIntToText( 0 );
#endif
}