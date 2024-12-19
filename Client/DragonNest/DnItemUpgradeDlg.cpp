#include "StdAfx.h"
#include "DnItemUpgradeDlg.h"
#include "DnInterface.h"
#include "DnItemTask.h"
#include "DnTableDB.h"
#include "ItemSendPacket.h"
#include "DnMainMenuDlg.h"
#ifdef PRE_ADD_NPC_REPUTATION_SYSTEM
#include "TaskManager.h"
#include "DnQuestTask.h"
#include "DnRepStoreBenefitTooltipDlg.h"
#endif // #ifdef PRE_ADD_NPC_REPUTATION_SYSTEM
#include "DnTooltipDlg.h"
#include "GameOption.h"
#include "VillageSendPacket.h"
#include "DnLocalPlayerActor.h"
#ifdef PRE_ADD_INSTANT_CASH_BUY
#include "DnInstantCashShopBuyDlg.h"
#include "DnCashShopTask.h"
#endif // PRE_ADD_INSTANT_CASH_BUY
#ifdef PRE_ADD_JELLYCOUNT
#include "DnMessageBox.h"
#endif
#ifdef PRE_ADD_EQUIPED_ITEM_ENCHANT
#include "DnCharStatusDlg.h"
#endif 

#ifdef PRE_ADD_DIRECT_BUY_UPGRADEITEM
#include "DnMarketDirectBuyDlg.h"
#include "TradeSendPacket.h"
#endif
#ifdef PRE_ADD_SHORTCUT_HELP_DIALOG
#include "DnSystemDlg.h"
#endif

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

#ifdef PRE_ADD_EQUIPED_ITEM_ENCHANT
#include "DnFadeInOutDlg.h"
#include "DnInvenTabDlg.h"
#endif

CDnItemUpgradeDlg::CDnItemUpgradeDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback, true )
, m_pUpgradeItem(NULL)
, m_pUpgradeItemSlot(NULL)
, m_pQuickSlotButton(NULL)
, m_pStaticGold(NULL)
, m_pStaticSilver(NULL)
, m_pStaticBronze(NULL)
, m_pButtonOK(NULL)
, m_pButtonCancel(NULL)
, m_pGuardItem(NULL)
, m_pGuardItemSlotButton(NULL)
#if defined(PRE_ADD_ENCHANTSHIELD_CASHITEM)
, m_pComboBoxGuard(NULL)
, m_nTotalShieldNees(0)
, m_nCurShieldCount(0)
, m_nCurGuardCount(0)
, m_nGuardItemID(0)
, m_nShiledItemID(0)
, m_cEnchantGuardType(0)
#else
, m_pCheckGuard(NULL)
, m_pStaticTextCount(NULL)
, m_pStaticTextBase(NULL)
, m_pStaticTextUse(NULL)
, m_pStaticTextNotUse(NULL)
#ifdef PRE_ADD_JELLYCOUNT
,m_pStaticTextNotUse2(NULL)
#endif
#endif
, m_pStaticGuard(NULL)
, m_nTotalGuardNeeds(0)
, m_pButtonDetail(NULL)
#ifdef PRE_ADD_NPC_REPUTATION_SYSTEM
, m_pReputationBenefit( NULL )
, m_pStoreBenefitTooltip( NULL )
#endif // #ifdef PRE_ADD_NPC_REPUTATION_SYSTEM
, m_pStaticNextUpgradeState(NULL)
, m_nSlotIndex(0)
, m_bIsCashItem(false)
, m_pStaticTitle(NULL)
, m_nRemoteItemID(0)
#ifdef PRE_ADD_DIRECT_BUY_UPGRADEITEM		
, m_bIsMiniMarket(false)
#endif
{
	for( int i = 0; i < ITEMSLOT_MAX; ++i )
	{
		m_pItem[i] = NULL;
		m_pItemSlotButton[i] = NULL;
#ifdef PRE_ADD_DIRECT_BUY_UPGRADEITEM		
		m_pUpgradeItemButton[i] = NULL;
#endif
	}
	m_pNextUpgradeItem = NULL;
	m_bShowNextUpgradeTooltip = false;

#ifdef PRE_ADD_EQUIPED_ITEM_ENCHANT
	m_pVecEquippedUpgradeItemSlot.reserve( EQUIPPED_ITEM );
	m_bEnchantEquippedItem = false;
	m_pControlSlotButton = NULL;
	m_pPreInvenSlotButton = NULL;
	m_pCurrentInvenSlotButton = NULL;
#endif
}

CDnItemUpgradeDlg::~CDnItemUpgradeDlg(void)
{
	SAFE_DELETE(m_pUpgradeItem);
	for( int i = 0; i < ITEMSLOT_MAX; ++i )
	{
		SAFE_DELETE(m_pItem[i]);
	}
	SAFE_DELETE(m_pGuardItem);
#ifdef PRE_ADD_NPC_REPUTATION_SYSTEM
	SAFE_DELETE( m_pStoreBenefitTooltip );
#endif // #ifdef PRE_ADD_NPC_REPUTATION_SYSTEM
	ShowNextUpgradeToolTip( false );
}

void CDnItemUpgradeDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "ItemUpgradeDlg.ui" ).c_str(), bShow );
}

void CDnItemUpgradeDlg::InitialUpdate()
{
	m_pUpgradeItemSlot = GetControl<CDnItemSlotButton>("ID_ITEM_UPGRADE");
	m_pUpgradeItemSlot->SetSlotType( ST_ITEM_UPGRADE );

	char szName[32];
	char szName2[32];
	for( int i = 0; i < ITEMSLOT_MAX; ++i )
	{
		sprintf_s(szName, _countof(szName), "%s%d", "ID_ITEM", i);
		m_pItemSlotButton[i] = GetControl<CDnItemSlotButton>(szName);
		m_pItemSlotButton[i]->SetSlotType( ST_ITEM_UPGRADE_JEWEL );

#ifdef PRE_ADD_DIRECT_BUY_UPGRADEITEM
		sprintf_s(szName2, _countof(szName2), "%s%d", "ID_BT_BUY", i);
		m_pUpgradeItemButton[i] = GetControl<CEtUIButton>(szName2);

		m_pUpgradeItemButton[i]->Show(false);
		m_pUpgradeItemButton[i]->Enable(false);
#endif
	}

	m_pStaticGold = GetControl<CEtUIStatic>("ID_GOLD");
	m_pStaticSilver = GetControl<CEtUIStatic>("ID_SILVER");
	m_pStaticBronze = GetControl<CEtUIStatic>("ID_BRONZE");
	m_dwColorGold = m_pStaticGold->GetTextColor();
	m_dwColorSilver = m_pStaticSilver->GetTextColor();
	m_dwColorBronze = m_pStaticBronze->GetTextColor();

	m_pButtonOK = GetControl<CEtUIButton>("ID_OK");
	m_pButtonCancel = GetControl<CEtUIButton>("ID_CANCEL");

	m_pGuardItemSlotButton = GetControl<CDnItemSlotButton>("ID_BUTTON_ENCHANTGUARD");
#if defined(PRE_ADD_ENCHANTSHIELD_CASHITEM)	
	m_pComboBoxGuard = GetControl<CEtUIComboBox>("ID_COMBOBOX_USE");	
#else
	m_pCheckGuard = GetControl<CEtUICheckBox>("ID_CHECKBOX_USE");	
	m_pStaticTextBase = GetControl<CEtUIStatic>("ID_TEXT_BASE");
	m_pStaticTextUse = GetControl<CEtUIStatic>("ID_TEXT_USE");
	m_pStaticTextNotUse = GetControl<CEtUIStatic>("ID_TEXT_NOTUSE");
#ifdef PRE_ADD_JELLYCOUNT
	m_pStaticTextNotUse2 = GetControl<CEtUIStatic>("ID_TEXT_NOTUSE2");
#endif
#endif	
	m_pStaticTextCount = GetControl<CEtUIStatic>("ID_TEXT_COUNT");	
	m_pButtonDetail = GetControl<CEtUIButton>("ID_BT_TOOLTIP");
	m_pStaticGuard = GetControl<CEtUIStatic>("ID_STATIC_GUARD");

#ifdef PRE_ADD_NPC_REPUTATION_SYSTEM
	m_pReputationBenefit = GetControl<CEtUIStatic>( "ID_ICON_REPUTE" );
	m_pReputationBenefit->Show( false );

	m_pStoreBenefitTooltip = new CDnRepStoreBenefitTooltipDlg( UI_TYPE_CHILD, this );
	m_pStoreBenefitTooltip->Initialize( false );
#endif // #ifdef PRE_ADD_NPC_REPUTATION_SYSTEM
	m_pStaticNextUpgradeState = GetControl<CEtUIStatic>( "ID_STATIC_MARK" );

	m_pStaticTitle = GetControl<CEtUIStatic>( "ID_STATIC_TITLE" );
	m_wszTitle = m_pStaticTitle->GetText();

#ifdef PRE_ADD_EQUIPED_ITEM_ENCHANT
	char* pszEquippedParts[]= { "_HEAD", "_BODY", "_LEG", "_GLOVE", "_BOOTS", "_WEAPON1", "_WEAPON2" };
	for( int i = 0; i < EQUIPPED_ITEM; ++i )
	{
		sprintf_s( szName, _countof(szName), "%s%s", "ID_ITEM", pszEquippedParts[i] );
		m_pVecEquippedUpgradeItemSlot.push_back( GetControl<CDnItemSlotButton>(szName) );
		if( m_pVecEquippedUpgradeItemSlot[i] ) m_pVecEquippedUpgradeItemSlot[i]->SetSlotType( ST_CHARSTATUS );
	}
#endif 

	m_pSmallHelpButton = GetControl<CEtUIButton>("ID_BT_SMALLHELP");
#ifdef PRE_ADD_SHORTCUT_HELP_DIALOG
	if(m_pSmallHelpButton)
	{
		m_pSmallHelpButton->Show(true);
		m_pSmallHelpButton->Enable(true);
	}
#else
	if(m_pSmallHelpButton)
	{
		m_pSmallHelpButton->Show(false);
		m_pSmallHelpButton->Enable(false);
	}
#endif
}

void CDnItemUpgradeDlg::Show( bool bShow )
{ 
	if( m_bShow == bShow )
		return;

	if( bShow )
	{
#ifdef PRE_ADD_NPC_REPUTATION_SYSTEM
		m_pReputationBenefit->Show( false );
#endif // #ifdef PRE_ADD_NPC_REPUTATION_SYSTEM
		if( m_nRemoteItemID )
			m_pStaticTitle->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8060 ) );
		else
			m_pStaticTitle->SetText( m_wszTitle.c_str() );
		RefreshNextUpgradeItem();
#ifdef PRE_ADD_INSTANT_CASH_BUY
		int nShowType = GetCashShopTask().GetInstantCashBuyEnableType( INSTANT_BUY_ITEMUPGRADE );
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
#ifdef PRE_ADD_EQUIPED_ITEM_ENCHANT
		for( int i = 0; i < (int)m_pVecEquippedUpgradeItemSlot.size(); ++i )
		{
			m_pVecEquippedUpgradeItemSlot[i]->Show( true );
		}
#endif 
	}
	else
	{
		m_pUpgradeItemSlot->ResetSlot();
		for( int i = 0; i < ITEMSLOT_MAX; ++i )
			m_pItemSlotButton[i]->ResetSlot();

		m_pStaticGold->SetIntToText(0);
		m_pStaticSilver->SetIntToText(0);
		m_pStaticBronze->SetIntToText(0);
		m_pStaticGold->SetTextColor( m_dwColorGold );
		m_pStaticSilver->SetTextColor( m_dwColorSilver );
		m_pStaticBronze->SetTextColor( m_dwColorBronze );

		m_pButtonOK->Enable( false );
		m_pButtonCancel->Enable( true );

		m_pGuardItemSlotButton->ResetSlot();
#if defined(PRE_ADD_ENCHANTSHIELD_CASHITEM)
		m_pComboBoxGuard->RemoveAllItems();		
		m_pComboBoxGuard->SetTextColor(textcolor::RED);
		m_pComboBoxGuard->AddItem(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8067 ), NULL, ENCHANT_ITEM_NONE);
#else
		m_pCheckGuard->SetChecked( false );
		m_pCheckGuard->Enable( false );
#ifdef PRE_ADD_ITEM_ENCHANT_TOOLTIP
		m_pCheckGuard->SetTooltipText( L"" );
#endif
		m_pStaticTextBase->Show( true );
		m_pStaticTextUse->Show( false );		
		m_pStaticTextNotUse->SetText( L"" );
#ifdef PRE_ADD_JELLYCOUNT
		m_pStaticTextNotUse2->Show( false );
#endif
#endif
		m_pStaticTextCount->SetText( L"0 / 0" );
		m_pButtonDetail->Show( false );
		m_pStaticGuard->Show( false );

#ifdef PRE_ADD_EQUIPED_ITEM_ENCHANT
		if( m_pControlSlotButton )
		{
			m_pControlSlotButton->SetRegist( false );
			m_pControlSlotButton = NULL;
		}
#else
		if( m_pQuickSlotButton )
		{
			m_pQuickSlotButton->SetRegist( false );
			m_pQuickSlotButton = NULL;
		}
#endif 
		SAFE_DELETE( m_pUpgradeItem );
		SAFE_DELETE( m_pNextUpgradeItem );
		ShowNextUpgradeToolTip( false, 0.f, 0.f );
		GetInterface().CloseBlind();

		if( m_nRemoteItemID )
		{
			m_nRemoteItemID = 0;
			GetInterface().OpenBaseDialog();
			CDnLocalPlayerActor::LockInput( false );
		}

#ifdef PRE_ADD_DIRECT_BUY_UPGRADEITEM 
		ResetUpgradeItemBtn();
#endif
	}

#ifdef PRE_ADD_DIRECT_BUY_UPGRADEITEM
	CEtUIStatic* pNotice = GetControl<CEtUIStatic>("ID_TEXT_NOTICE");
	if( CDnWorld::MapTypeEnum::MapTypeVillage == CDnWorld::GetInstancePtr()->GetMapType() )
		pNotice->Show(true);
	else
		pNotice->Show(false);
#endif

	CEtUIDialog::Show( bShow );
}

void CDnItemUpgradeDlg::SetRemoteItemID( int nRemoteItemID )
{
	m_nRemoteItemID = nRemoteItemID;

	if( nRemoteItemID ) {
		GetInterface().GetMainMenuDialog()->SetIgnoreShowFunc( true );
		GetInterface().OpenBlind();
		CDnLocalPlayerActor::StopAllPartyPlayer();
		CDnLocalPlayerActor::LockInput( true );
	}
}

bool CDnItemUpgradeDlg::IsUpgradableItem(CDnSlotButton* pDragButton, CDnItem* pItem) const
{
#ifdef PRE_ADD_EQUIPED_ITEM_ENCHANT
	// 인벤토리 외 ST_CHARSTATUS, 강화 슬롯 자신이 들어올 경우도 있기 때문에 제외시킨다. 
#else 
	if( pDragButton->GetSlotType() != ST_INVENTORY )
	{
		GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3610 ), MB_OK );
		return false;
	}
#endif 

#ifdef PRE_ADD_EQUIPED_ITEM_ENCHANT
	if( m_pControlSlotButton &&
		m_pControlSlotButton->GetSlotType() == ST_QUICKSLOT && 
		( pItem == m_pControlSlotButton->GetItem() ) )
	{
		return false;
	}
#else 
	// 올린거 또 올리는거면 패스
	if( m_pQuickSlotButton && (pItem == m_pQuickSlotButton->GetItem()) )
	{
		return false;
	}
#endif 

	// 아이템 종류 검사
	if( pItem->GetItemType() == ITEMTYPE_QUEST )
	{
		GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3609 ), MB_OK );
		return false;
	}

	// 강화가 안되는 아이템인지 검사.
	if( !pItem->GetEnchantID() )
	{
		GetInterface().MessageBox( 8007, MB_OK );
		return false;
	}

	// 혹시 강화가 다 된건지 확인
	if( pItem->GetEnchantLevel() >= pItem->GetMaxEnchantLevel() )
	{
		GetInterface().MessageBox( 8006, MB_OK );
		return false;
	}

	// 귀속아이템이면서 봉인상태의 아이템인지 확인
	if( pItem->GetReversion() > CDnItem::NoReversion && !pItem->IsSoulbBound() )
	{
		GetInterface().MessageBox( 1748, MB_OK );
		return false;
	}

	return true;
}


void CDnItemUpgradeDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl( "ID_OK" ) )
		{
			if( m_nRemoteItemID )
			{
				int nCount = 0;
				nCount += GetItemTask().GetCharInventory().GetItemCount( m_nRemoteItemID );
				nCount += GetItemTask().GetCashInventory().GetItemCount( m_nRemoteItemID );
				if( !nCount )
				{
					GetInterface().MessageBox( 8059, MB_OK );
					return;
				}
			}

#ifdef PRE_ADD_JELLYCOUNT

			CDnMessageBoxTextBox * pDnMessageBoxTextBox = GetInterface().GetMessageBoxTextBox();
			if( pDnMessageBoxTextBox )
			{				
				pDnMessageBoxTextBox->ClearText();

				int nUIString = 8008; // "강화 하시겠습니까? 강화 실패시 재료가 사라지거나 +6이후 부터 아이템이 파괴될 수 있습니다."

				std::wstring strMessage;
				//D3DCOLOR bgColor = textcolor::BLACK;
				D3DCOLOR fontColor = textcolor::WHITE;												

				// 상급 보호 아이템이 [있는] 국가 //
#ifdef PRE_ADD_ENCHANTSHIELD_CASHITEM

				int nEnchantGuardType = (int)ENCHANT_ITEM_NONE;
				m_pComboBoxGuard->GetSelectedValue(nEnchantGuardType);

				// 사용X.
				if( nEnchantGuardType == ENCHANT_ITEM_NONE )
				{
					// 사용가능.
					if( (m_nCurGuardCount >= m_nTotalGuardNeeds) || ( m_nCurShieldCount > m_nTotalShieldNees ) )
					{
						//bgColor = textcolor::YELLOW;
						//fontColor = textcolor::YELLOW;
						fontColor = textcolor::ORANGERED; // #54802.
						strMessage = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8070 ); // "강화 보호 아이템 비사용중(사용가능)"
					}
					// 사용불가.
					else
					{
						//bgColor = textcolor::SILVER;
						//fontColor = textcolor::WHITE;
						fontColor = textcolor::ORANGE;
						//strMessage = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8071 ); // "강화 보호 아이템 사용불가"
						strMessage = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8098 ); // "강화보호아이템 부족" #54798
					}
				}

				// 사용중.
				else
				{	
					//bgColor = textcolor::PALEGREEN;
					fontColor = textcolor::GREENYELLOW;

					// 강화 보호젤리
					if( nEnchantGuardType == ENCHANT_ITEM_GUARD )
					{
						nUIString = 8097; // "강화하시겠습니까?" #54760

						// "%s 사용중", "아이템보호젤리"
						if( m_nCurGuardCount < m_nTotalGuardNeeds )
						{
							fontColor = textcolor::ORANGE;
							strMessage = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8098 ) );  // "강화보호아이템 부족"
						}
						else
							strMessage = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8069 ), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1000005253 ) );						
					}

					// 상위 강화보호 젤리.
					else if( nEnchantGuardType == ENCHANT_ITEM_SHIELD )
					{
						nUIString = 8097; // "강화하시겠습니까?" #54760

						// "%s 사용중", "상위아이템보호젤리"
						if( m_nCurShieldCount < m_nTotalShieldNees )
						{
							fontColor = textcolor::ORANGE;
							strMessage = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8098 ) ); // "강화보호아이템 부족"
						}
						else
							strMessage = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8069 ), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8047 ) );						
					}					
				}

				// 상급 보호 아이템이 [없는] 국가 //
#else			
				// 사용X.
				if( !m_pCheckGuard->IsEnable() || !m_pCheckGuard->IsChecked() )
				{
					int nGuardItemTotalCount = GetItemTask().FindItemCountFromItemType( ITEMTYPE_ENCHANT_BREAKGAURD );

					// 사용가능.
					if( nGuardItemTotalCount >= m_nTotalGuardNeeds )
					{
						//bgColor = textcolor::YELLOW;
						//fontColor = textcolor::YELLOW;
						fontColor = textcolor::ORANGERED; // #54802.
						strMessage = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8070 ); // "강화 보호 아이템 비사용중(사용가능)"
					}

					// 사용불가.
					else
					{
						//bgColor = textcolor::SILVER;
						//fontColor = textcolor::WHITE;
						fontColor = textcolor::ORANGE;						
						strMessage = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8098 ); // "강화보호아이템 부족" #54798
					}
				}

				// 사용중.
				else
				{	
					nUIString = 8097; // "강화하시겠습니까?" #54760

					// "%s 사용중", "아이템보호젤리"										
					//bgColor = textcolor::PALEGREEN;			
					fontColor = textcolor::GREENYELLOW;
					strMessage = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8069 ), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8030 ) );					
				}				
#endif // PRE_ADD_ENCHANTSHIELD_CASHITEM

				// 보호아이템 사용불가 단계.
				if( m_nTotalGuardNeeds == 0 )
				{
					//fontColor = textcolor::RED;
					fontColor = textcolor::DARKGRAY; // #54793.
					strMessage = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8056 ); // "해당 단계에서는 사용 불가"
				}
#ifdef PRE_ADD_EQUIPED_ITEM_ENCHANT
				if( m_bEnchantEquippedItem )
				{
					pDnMessageBoxTextBox->AddMessage( L"\n" );
					pDnMessageBoxTextBox->AddMessage( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7917 ), textcolor::RED, UITEXT_CENTER ); // 장착 중인 장비를 강화합니다 !
					pDnMessageBoxTextBox->AddMessage( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nUIString ), textcolor::WHITE, UITEXT_CENTER ); // UIString : 8008 
				}
				else
				{
					pDnMessageBoxTextBox->AddMessage( L"\n" );
					pDnMessageBoxTextBox->AddMessage( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nUIString ), textcolor::WHITE, UITEXT_CENTER );
				}
#else
				pDnMessageBoxTextBox->AddMessage( L"\n" );
				pDnMessageBoxTextBox->AddMessage( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nUIString ), textcolor::WHITE, UITEXT_CENTER );
#endif	
				pDnMessageBoxTextBox->AddMessage( L"\n" );				
				pDnMessageBoxTextBox->AddMessage( strMessage.c_str(), fontColor, UITEXT_CENTER, 0 );

				GetInterface().MessageBoxTextBox( L" ", MB_YESNO, ITEM_UPGRADE_CONFIRM_DIALOG, this );
			}

#else

#if defined(PRE_ADD_ENCHANTSHIELD_CASHITEM)
			// 강화하시겠습니까? 메세지 후 처리해야한다.
			GetInterface().MessageBox( (m_cEnchantGuardType != ENCHANT_ITEM_NONE ? 8031 : 8008), MB_YESNO, ITEM_UPGRADE_CONFIRM_DIALOG, this );
#else
			// 강화하시겠습니까? 메세지 후 처리해야한다.
			GetInterface().MessageBox( (m_pCheckGuard->IsChecked() ? 8031 : 8008), MB_YESNO, ITEM_UPGRADE_CONFIRM_DIALOG, this );
#endif	

#endif

			CDnSlotButton *pDragButton = (CDnSlotButton *)drag::GetControl();
			if( pDragButton ) {
				pDragButton->DisableSplitMode(true);
				drag::ReleaseControl();
			}
			return;
		}
		else if( IsCmdControl( "ID_CANCEL" ) )
		{
			ProcessCommand( EVENT_BUTTON_CLICKED, false, GetControl("ID_CLOSE_DIALOG"), 0 );
			return;
		}
		else if( IsCmdControl("ID_ITEM_UPGRADE") )
		{
			CDnSlotButton *pDragButton = ( CDnSlotButton * )drag::GetControl();
			CDnSlotButton *pPressedButton = ( CDnItemSlotButton * )pControl;

			// 우클릭으로 빼기
			if( uMsg == WM_RBUTTONUP )
			{
				SAFE_DELETE(m_pUpgradeItem);
#ifdef PRE_ADD_EQUIPED_ITEM_ENCHANT
				if( pPressedButton ) pPressedButton->ResetSlot();
#else 
				pPressedButton->ResetSlot();
#endif 

#ifdef PRE_ADD_EQUIPED_ITEM_ENCHANT
				if( m_pControlSlotButton )
				{
					m_pControlSlotButton->SetRegist( false );
					m_pControlSlotButton = NULL;
				}
#else
				if( m_pQuickSlotButton )
				{
					m_pQuickSlotButton->SetRegist( false );
					m_pQuickSlotButton = NULL;
				}
#endif 
#ifdef PRE_ADD_EQUIPED_ITEM_ENCHANT
				if( m_bEnchantEquippedItem ) // 장착 해제되었으니 끈다. 
					m_bEnchantEquippedItem = false;
#endif 
				CheckUpgradeItem();
#ifdef PRE_ADD_DIRECT_BUY_UPGRADEITEM
				ResetUpgradeItemBtn();
#endif
				return;
			}
			// 다른 슬롯에서 강화 슬롯으로 아이템을 옮긴 경우 
			if( pDragButton )
			{
				drag::ReleaseControl();

				CDnItem *pItem = static_cast<CDnItem *>(pDragButton->GetItem());
				if (pItem == NULL || IsUpgradableItem(pDragButton, pItem) == false)
				{
					pDragButton->DisableSplitMode(true);
					return;
				}

				// 기존의 강화 슬롯에 아이템이 있었는가? 
				CDnItem *pPressedItem = static_cast<CDnItem *>( pPressedButton->GetItem() );

				// 강화슬롯의 아이템을 클릭했다가 다시 제자리로 놓는경우 

#ifdef PRE_ADD_EQUIPED_ITEM_ENCHANT
				if( pDragButton && pPressedButton && pDragButton == pPressedButton )
				{
					pDragButton->SetRegist( false );
					return;
				}
#endif 
				// 이미 아이템이 있는 경우 
				if( pPressedItem )
				{								
					// 해당 아이템을 창에서 제거한다.
#ifdef PRE_ADD_EQUIPED_ITEM_ENCHANT
					CDnSlotButton* pPreEquipitem = static_cast<CDnSlotButton*> ( GetEquipSlotButton(m_pUpgradeItem->GetSlotIndex()) );
					if( pPreEquipitem ) pPreEquipitem->SetRegist( false );
					if( m_pPreInvenSlotButton && m_pCurrentInvenSlotButton && m_pPreInvenSlotButton->GetItemSlotIndex() != m_pCurrentInvenSlotButton->GetItemSlotIndex() )
						// 강화 슬롯에 있는 아이템이 인벤 아이템인데 강화 슬롯에 또다른 인벤 아이템을 넣는 경우. 
					{
						m_pPreInvenSlotButton->SetRegist( false ); 
					} 
					else if( m_pCurrentInvenSlotButton )
					{
						m_pCurrentInvenSlotButton->SetRegist( false );
					}

					SAFE_DELETE(m_pUpgradeItem);
					pPressedButton->ResetSlot();		// 해당 컨트롤 날린다.
					SetUpgradeItem( pDragButton );	// 슬롯 아이템 새롭게 갱신 
#else 
					SAFE_DELETE(m_pUpgradeItem);
					pPressedButton->ResetSlot();
					m_pQuickSlotButton->SetRegist( false );
					m_pQuickSlotButton = NULL;
#endif			
				}

#ifdef PRE_ADD_EQUIPED_ITEM_ENCHANT
				// 빈 슬롯일 경우
				else 
				{
					pDragButton->SetRegist( false );
					if( IsUpgradeItem( pDragButton) )
						SetUpgradeItem( pDragButton );
				}
				m_pPreInvenSlotButton = m_pCurrentInvenSlotButton;
#else
				// 강화아이템을 등록할때 전처럼 이렇게 인벤토리 아이템의 포인터를 직접 넘기지 않고,
				//pPressedButton->SetItem( pItem );
				// 이렇게 임시템을 만들어서 넣어준다.
				TItemInfo itemInfo;
				pItem->GetTItemInfo(itemInfo);
				m_pUpgradeItem = GetItemTask().CreateItem(itemInfo);
				pPressedButton->SetItem(m_pUpgradeItem, CDnSlotButton::ITEM_ORIGINAL_COUNT);

				CheckUpgradeItem();
				CEtSoundEngine::GetInstance().PlaySound( "2D", pItem->GetDragSoundIndex() );

				m_pQuickSlotButton = (CDnQuickSlotButton *)pDragButton;
				m_pQuickSlotButton->SetRegist( true );
#endif 
			}
#ifdef PRE_ADD_EQUIPED_ITEM_ENCHANT
			// 클릭 & 드래그 
			else
			{				
				if (pPressedButton->IsRegist())
					return;

				if (GetInterface().IsOpenWorldMsgDialog())
					return;
				
				CDnItem* pItem = static_cast<CDnItem*>( pPressedButton->GetItem() );
				
				if( pItem )
				{
					pPressedButton->EnableSplitMode( pItem->GetOverlapCount());
					drag::SetControl(pPressedButton);
					CEtSoundEngine::GetInstance().PlaySound( "2D", pItem->GetDragSoundIndex() );
				}
			}
#endif 

			return;
		}
#ifdef PRE_ADD_INSTANT_CASH_BUY
		else if( IsCmdControl( "UI_BUTTON_BUYSHORTCUT" ) )
		{
			GetInterface().ShowInstantCashShopBuyDlg( true, INSTANT_BUY_ITEMUPGRADE, this );
			return;
		}
#endif // PRE_ADD_INSTANT_CASH_BUY

#ifdef PRE_ADD_EQUIPED_ITEM_ENCHANT
		else if( strstr( pControl->GetControlName(), "ID_ITEM_" ) )
		{
			// 강화창에 슬롯을 클릭 했을때 ... 
			CDnSlotButton *pDragButton;
			CDnItemSlotButton *pPressedButton;

			if( GetItemTask().IsRequestWait() ) return;

			if( GetInterface().GetFadeDlg() && (GetInterface().GetFadeDlg()->GetFadeMode() != CDnFadeInOutDlg::modeEnd) )
				return;

			CDnLocalPlayerActor* pLocalPlayerActor = dynamic_cast<CDnLocalPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
			if( pLocalPlayerActor && pLocalPlayerActor->IsObserver() )
				return;

			// 기본적으로 거래중엔 캐릭터창이 안뜰테지만, 혹시 뜨더라도 장비해제 및 교체를 막아두겠다.
			CDnInvenTabDlg *pInvenDlg = (CDnInvenTabDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::INVENTORY_DIALOG );
			if( pInvenDlg && pInvenDlg->IsShowPrivateMarketDialog() ) return;

			focus::ReleaseControl();

			pDragButton = ( CDnSlotButton * )drag::GetControl();
			pPressedButton = ( CDnItemSlotButton * )pControl; 

			// 장착중인 슬롯을 클릭한 경우
			if( pDragButton == NULL )
			{
				if( pPressedButton && !pPressedButton->GetItem() ) 
					return;

#ifdef PRE_ADD_EQUIPLOCK
				CDnItem *pItem = static_cast<CDnItem *>(pPressedButton->GetItem());
				if (pItem && CDnItemTask::IsActive())
				{
					const CDnItemLockMgr& lockMgr = CDnItemTask::GetInstance().GetItemLockMgr();
					EquipItemLock::eLockStatus status = lockMgr.GetLockState(*pItem);
					if (status != EquipItemLock::None)
					{
						GetInterface().MessageBox(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 8388), MB_OK);
						return;
					}
				}
#endif

				// 우클릭으로 바로 강화슬롯에 옮기는 경우
				if( uMsg == WM_RBUTTONUP )
				{
					if( IsUpgradeItem( pPressedButton ) )
						SetUpgradeItem( pPressedButton );
				}
				// WM_LBUTTONUP ( 좌클릭으로 강화슬롯에 옮기기 위해 아이템을 잡은 경우 ) 
				else
				{
					if (pPressedButton->IsRegist())
						return;

					if (GetInterface().IsOpenWorldMsgDialog())
						return;

					pPressedButton->EnableSplitMode( pPressedButton->GetItem()->GetOverlapCount());
					drag::SetControl(pPressedButton);

#ifdef PRE_ADD_EQUIPLOCK
#else
					CDnItem *pItem = static_cast<CDnItem *>(pPressedButton->GetItem());
#endif
					CEtSoundEngine::GetInstance().PlaySound( "2D", pItem->GetDragSoundIndex() );
				}
			}
			// 다른 슬롯에서 장착 아이템 슬롯에다가 아이템을 놓을때
			else if( pDragButton ) 
			{
				// #70609 버그 수정
				if( !pDragButton->GetItem() )
					return;

				drag::ReleaseControl();

				CDnItem *pItem = static_cast<CDnItem *>(pPressedButton->GetItem());		// #70419
				if (pItem == NULL || pDragButton->GetSlotType() == ST_INVENTORY || m_bEnchantEquippedItem == false )  // m_bEnchantEquippedItem == false 이면 인벤아이템으로 간주 
				{
					pDragButton->DisableSplitMode(true);
					return;
				}

				// 제자리에 두거나, 다른 장착중인 슬롯에 넣을경우, 인벤 아이템을 강화슬롯에 옮겼다가 장착 슬롯으로 옮기는 경우	
				if( pDragButton == pPressedButton || 
					pDragButton != pPressedButton	
					&& pDragButton->GetItem()->GetSlotIndex() != pPressedButton->GetItem()->GetSlotIndex() )
				{
					pDragButton->SetRegist( false );
					return;
				}
				else
				{
					// 강화슬롯에 있는 아이템을 장착중인 슬롯에다가 옮긴 경우
					SAFE_DELETE(m_pUpgradeItem);

					// 강화 슬롯 초기화 
					pDragButton->ResetSlot();
					pDragButton->SetRegist( false );
					pDragButton = NULL;

					// 장착중인 아이템 슬롯 초기화 
					pPressedButton->SetRegist( false ); 
				}

#ifdef PRE_ADD_EQUIPLOCK
				if (CDnItemTask::IsActive())
				{
					const CDnItemLockMgr& lockMgr = CDnItemTask::GetInstance().GetItemLockMgr();
					EquipItemLock::eLockStatus status = lockMgr.GetLockState(*pItem);
					if (status != EquipItemLock::None)
					{
						GetInterface().MessageBox(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 8388), MB_OK);
						return;
					}
				}
#endif

				CheckUpgradeItem();
#ifdef PRE_ADD_DIRECT_BUY_UPGRADEITEM
				ResetUpgradeItemBtn();
#endif
			}
		}
#endif 

#ifdef PRE_ADD_DIRECT_BUY_UPGRADEITEM
		else if( strstr(pControl->GetControlName(), "ID_BT_BUY") )
		{
			for(int i = 0 ; i < ITEMSLOT_MAX ; ++i)
			{
				if( IsCmdControl(FormatA("ID_BT_BUY%d", i).c_str()) )
				{
					ShowDirectBuyMarketDlg(i);
				}
			}
		}
#endif // PRE_ADD_DIRECT_BUY_UPGRADEITEM

#ifdef PRE_ADD_SHORTCUT_HELP_DIALOG
		else if( IsCmdControl("ID_BT_SMALLHELP") )
		{
			CDnMainMenuDlg* pMainMenuDlg = GetInterface().GetMainMenuDialog();
			CDnSystemDlg* pSystemDlg = pMainMenuDlg->GetSystemDlg();
			if(pSystemDlg)
				pSystemDlg->ShowChoiceHelpDlg(HELP_SHORTCUT_ITEM_UPGRADE);
		}
#endif // PRE_ADD_SHORTCUT_HELP_DIALOG

	}
#if defined(PRE_ADD_ENCHANTSHIELD_CASHITEM)
	else if( nCommand == EVENT_COMBOBOX_SELECTION_CHANGED )
	{
		if( IsCmdControl( "ID_COMBOBOX_USE") )
		{
			SetUpgardeGuardItem();
			return;
		}
	}
#else
	else if( nCommand == EVENT_CHECKBOX_CHANGED )
	{
		if( IsCmdControl( "ID_CHECKBOX_USE" ) )
		{
			if( m_pCheckGuard->IsEnable() )
			{
				if( m_pCheckGuard->IsChecked() )
				{
					m_pGuardItemSlotButton->SetRegist( false );
					m_pStaticTextUse->Show( true );
					m_pStaticTextNotUse->Show( false );
					m_pStaticGuard->Show( true );

#ifdef PRE_ADD_JELLYCOUNT
					m_pStaticTextNotUse2->Show( false );
#endif					
				}
				else
				{
					m_pGuardItemSlotButton->SetRegist( true );
					m_pStaticTextUse->Show( false );
					m_pStaticTextNotUse->Show( true );
					m_pStaticGuard->Show( false );

#if defined(PRE_ADD_JELLYCOUNT) && !defined(PRE_ADD_ENCHANTSHIELD_CASHITEM)					
					CheckUpgradState();
#endif
				}
			}
		}
	}
#endif

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnItemUpgradeDlg::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg )
{
#ifdef PRE_ADD_INSTANT_CASH_BUY
	if( pControl == NULL && nID == INSTANT_CASHSHOP_BUY_DIALOG )
	{
#ifdef PRE_ADD_ENCHANTSHIELD_CASHITEM
		int nEnchantGuardType;
		m_pComboBoxGuard->GetSelectedValue( nEnchantGuardType );
		CheckUpgradeGuardItem();
		m_pComboBoxGuard->SetSelectedByValue( nEnchantGuardType );
#else // PRE_ADD_ENCHANTSHIELD_CASHITEM
		bool bCheckGuard = m_pCheckGuard->IsChecked();
		CheckUpgradeGuardItem();
		m_pCheckGuard->SetChecked( bCheckGuard );
#endif // PRE_ADD_ENCHANTSHIELD_CASHITEM

		if( nCommand == 1 )
			Show( false );

		return;
	}
#endif // PRE_ADD_INSTANT_CASH_BUY

	SetCmdControlName( pControl->GetControlName() );

	switch( nID ) 
	{
	case ITEM_UPGRADE_CONFIRM_DIALOG:
		if( nCommand == EVENT_BUTTON_CLICKED )
		{
			if( IsCmdControl("ID_YES") )
			{
				// 서버에다 등록 요청 후 해당 패킷 받으면 연출동영상 띄우는거다.
				OutputDebug( "강화요청 전송\n" );
				if( !CDnItemTask::IsActive() ) return;

#ifdef PRE_ADD_EQUIPED_ITEM_ENCHANT 
				CDnItem *pItem = static_cast<CDnItem *>(m_pControlSlotButton->GetItem());
#else 
				// 해당 처리는 퀵슬롯아이템일 경우에만 처리된다.
				CDnItem *pItem = static_cast<CDnItem *>(m_pQuickSlotButton->GetItem());
#endif 
#if defined(PRE_ADD_ENCHANTSHIELD_CASHITEM)
#ifdef PRE_ADD_EQUIPED_ITEM_ENCHANT
				CDnItemTask::GetInstance().RequestEnchant( pItem, m_cEnchantGuardType, m_bEnchantEquippedItem );
#else 
				CDnItemTask::GetInstance().RequestEnchant( pItem, m_cEnchantGuardType );
#endif // PRE_ADD_EQUIPED_ITEM_ENCHANT
#else
#ifdef PRE_ADD_EQUIPED_ITEM_ENCHANT
				CDnItemTask::GetInstance().RequestEnchant( pItem, m_pCheckGuard->IsChecked(), m_bEnchantEquippedItem );
#else
				CDnItemTask::GetInstance().RequestEnchant( pItem, m_pCheckGuard->IsChecked() );
#endif 
#endif
				m_pButtonOK->Enable( false );
			}
		}
		break;
	case ITEM_UPGRADE_MOVIE_DIALOG:
		if( nCommand == EVENT_BUTTON_CLICKED )
		{
			// 안보이는 버튼. 프로세스 처리가 끝났음을 알린다.
			if (IsCmdControl("ID_PROCESS_COMPLETE"))
			{
				// 완료 패킷을 보낸다.
				OutputDebug("강화 프로세스 완료 전송\n");

#ifdef PRE_ADD_EQUIPED_ITEM_ENCHANT 
				CDnItem *pItem = static_cast<CDnItem *>(m_pControlSlotButton->GetItem());
#else 
				CDnItem *pItem = static_cast<CDnItem *>(m_pQuickSlotButton->GetItem());
#endif

#if defined(PRE_ADD_ENCHANTSHIELD_CASHITEM)
#ifdef PRE_ADD_EQUIPED_ITEM_ENCHANT 
				if (pItem->IsCashItem())
				{
					SendEnchantComplete(pItem->GetSlotIndex(), pItem->GetSerialID(), m_cEnchantGuardType, m_bEnchantEquippedItem,true);
				}
				else {

					SendEnchantComplete(pItem->GetSlotIndex(), pItem->GetSerialID(), m_cEnchantGuardType, m_bEnchantEquippedItem,false);
				}
#else 
				SendEnchantComplete( m_pQuickSlotButton->GetSlotIndex(), pItem->GetSerialID(), m_cEnchantGuardType);
#endif		
#else
#ifdef PRE_ADD_EQUIPED_ITEM_ENCHANT 
				SendEnchantComplete(pItem->GetSlotIndex(), pItem->GetSerialID(), m_pCheckGuard->IsChecked(), m_bEnchantEquippedItem );
#else
				SendEnchantComplete( m_pQuickSlotButton->GetSlotIndex(), pItem->GetSerialID(), m_pCheckGuard->IsChecked());
#endif 
#endif // PRE_ADD_ENCHANTSHIELD_CASHITEM 

			}
			else if( IsCmdControl("ID_BUTTON_CANCEL") )
			{
				// 취소 패킷을 보낸다.
				OutputDebug( "강화 취소 전송\n" );
				SendEnchantCancel();
				m_pButtonOK->Enable( true );
			}
		}
		break;
	}
}

void CDnItemUpgradeDlg::OnRecvEnchant(int nSlotIndex)
{
	// 강화하려는 아이템에 따라 적절한 시간을 구해온 후
	ASSERT(m_pQuickSlotButton != NULL);

#ifdef PRE_ADD_EQUIPED_ITEM_ENCHANT
	CDnItem *pItem = NULL;
	if (m_pControlSlotButton)
		pItem = static_cast<CDnItem *>(m_pControlSlotButton->GetItem());
#else 
	if (!m_pQuickSlotButton) return;					// 뻑나는거 임시로 처리해놉니다.. by realgaia
	CDnItem *pItem = static_cast<CDnItem *>(m_pQuickSlotButton->GetItem());
#endif 

	if (!pItem) return;
	if (pItem->GetSlotIndex() != nSlotIndex) return;

	m_nSlotIndex = nSlotIndex;
	m_emUpgradeItemRank = pItem->GetItemRank();

	char szMovieFileName[32] = { 0, };
	float fTime = 0.0f;
	if (pItem->GetItemRank() == ITEMRANK_D || pItem->GetItemRank() == ITEMRANK_C)
	{
		sprintf_s(szMovieFileName, _countof(szMovieFileName), "Enchanting01.avi");
#ifdef PRE_MOD_ITEMUPGRADE_COOLTIME
		fTime = float(CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::ITEMUPGRADE_COOLTIME_NOMAL_MAGIC_ITEM) * 0.001f);
		if (fTime <= 0.f)
			fTime = 2.f;
#else
		fTime = 2.0f;
#endif
	}
	else if (pItem->GetItemRank() == ITEMRANK_B || pItem->GetItemRank() == ITEMRANK_A)
	{
		sprintf_s(szMovieFileName, _countof(szMovieFileName), "Enchanting02.avi");
#ifdef PRE_MOD_ITEMUPGRADE_COOLTIME
		fTime = float(CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::ITEMUPGRADE_COOLTIME_RARE_EPIC_ITEM) * 0.001f);
		if (fTime <= 0.f)
			fTime = 4.f;
#else
		fTime = 4.0f;
#endif
	}
	else if (pItem->GetItemRank() >= ITEMRANK_S)
	{
		sprintf_s(szMovieFileName, _countof(szMovieFileName), "Enchanting03.avi");
#ifdef PRE_MOD_ITEMUPGRADE_COOLTIME
		fTime = float(CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::ITEMUPGRADE_COOLTIME_ABOVE_UNIQUE) * 0.001f);
		if (fTime <= 0.f)
			fTime = 6.f;
#else
		fTime = 6.0f;
#endif
	}

	GetInterface().OpenMovieProcessDlg(
		szMovieFileName, fTime, GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 8016), ITEM_UPGRADE_MOVIE_DIALOG, this);
}
void CDnItemUpgradeDlg::OnRecvEnchantCash(int nSlotIndex)
{
	// 강화하려는 아이템에 따라 적절한 시간을 구해온 후
	ASSERT(m_pQuickSlotButton != NULL);

#ifdef PRE_ADD_EQUIPED_ITEM_ENCHANT
	CDnItem *pItem = NULL;
	if (m_pControlSlotButton)
		pItem = static_cast<CDnItem *>(m_pControlSlotButton->GetItem());
#else 
	if (!m_pQuickSlotButton) return;					// 뻑나는거 임시로 처리해놉니다.. by realgaia
	CDnItem *pItem = static_cast<CDnItem *>(m_pQuickSlotButton->GetItem());
#endif 

	if (!pItem) return;
	if (pItem->GetSlotIndex() != nSlotIndex) return;

	m_bIsCashItem = true;
	m_nSlotIndex = nSlotIndex;
	m_emUpgradeItemRank = pItem->GetItemRank();

	char szMovieFileName[32] = { 0, };
	float fTime = 0.0f;
	if (pItem->GetItemRank() == ITEMRANK_D || pItem->GetItemRank() == ITEMRANK_C)
	{
		sprintf_s(szMovieFileName, _countof(szMovieFileName), "Enchanting01.avi");
#ifdef PRE_MOD_ITEMUPGRADE_COOLTIME
		fTime = float(CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::ITEMUPGRADE_COOLTIME_NOMAL_MAGIC_ITEM) * 0.001f);
		if (fTime <= 0.f)
			fTime = 2.f;
#else
		fTime = 2.0f;
#endif
	}
	else if (pItem->GetItemRank() == ITEMRANK_B || pItem->GetItemRank() == ITEMRANK_A)
	{
		sprintf_s(szMovieFileName, _countof(szMovieFileName), "Enchanting02.avi");
#ifdef PRE_MOD_ITEMUPGRADE_COOLTIME
		fTime = float(CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::ITEMUPGRADE_COOLTIME_RARE_EPIC_ITEM) * 0.001f);
		if (fTime <= 0.f)
			fTime = 4.f;
#else
		fTime = 4.0f;
#endif
	}
	else if (pItem->GetItemRank() >= ITEMRANK_S)
	{
		sprintf_s(szMovieFileName, _countof(szMovieFileName), "Enchanting03.avi");
#ifdef PRE_MOD_ITEMUPGRADE_COOLTIME
		fTime = float(CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::ITEMUPGRADE_COOLTIME_ABOVE_UNIQUE) * 0.001f);
		if (fTime <= 0.f)
			fTime = 6.f;
#else
		fTime = 6.0f;
#endif
	}

	GetInterface().OpenMovieProcessDlg(
		szMovieFileName, fTime, GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 8016), ITEM_UPGRADE_MOVIE_DIALOG, this);
}

void CDnItemUpgradeDlg::OnRecvEnchantComplete( char cEnchantResult, int nItemID, char cLevel, char cOption )
{
	// cEnchantResult
	// 0 : 강화실패. 아이템파괴됨. nItemID에 값 있을 경우 대체보상.
	// 1 : 강화성공. 아이템파괴안됨.
	// 2 : 레벨다운. 다운값 0 초과.
	// 3 : 레벨다운. 다운값 0.

	bool bBreakItem = false;
	if( cEnchantResult == 0 ) bBreakItem = true;

	// 인챈트할 아이템이 파괴되지 않았을 때는 패킷으로 온 아이템ID와 강화하려고 등록한 아이템ID가 같은지 확인.
	if( !bBreakItem )
	{
#ifdef PRE_ADD_EQUIPED_ITEM_ENCHANT
		if( m_pQuickSlotButton && ! m_bEnchantEquippedItem ) // 장착중인 장비의 경우에는 해당 조건검사를 패스한다. 
#else
		if( m_pQuickSlotButton )	// 혹시 NULL일수도 있으니 NULL이면 검증 무시한다.
#endif 
		{
			CDnItem *pItem = static_cast<CDnItem *>(m_pQuickSlotButton->GetItem());
			if( !pItem ) return;
			if( pItem->GetClassID() != nItemID ) return;
		}
	}

	WCHAR wszMsg[256];
#ifdef PRE_ADD_EQUIPED_ITEM_ENCHANT	
	CDnItem *pItem = m_bEnchantEquippedItem ? GetItemTask().GetEquipItem( m_nSlotIndex ) : CDnItemTask::GetInstance().GetCharInventory().GetItem( m_nSlotIndex );
#else
	CDnItem *pItem = CDnItemTask::GetInstance().GetCharInventory().GetItem( m_nSlotIndex );
#endif
	if( !pItem ) pItem = ( nItemID > 0 ) ? CDnItemTask::GetInstance().GetCharInventory().GetItem( m_nSlotIndex ) : NULL;
	switch( cEnchantResult ) {
		case 0:
			{
				if( pItem ) {
					swprintf_s(wszMsg, _countof(wszMsg), L"%s", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8009 ) );
					GetInterface().ShowItemAlarmDialog( wszMsg, pItem, pItem->GetOverlapCount(), textcolor::WHITE, 2.0f );
				}
				else {
					swprintf_s(wszMsg, _countof(wszMsg), L"\n\n%s", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8010 ) );
					GetInterface().ShowItemAlarmDialog( wszMsg, NULL, 0, textcolor::WHITE, 2.0f );
				}
			}
			break;
		case 1:
			{
				if( pItem ) {
					// 강화 되어있다면,
					swprintf_s(wszMsg, _countof(wszMsg), L"+%d %s", cLevel, pItem->GetName() );

					// pItem->GetOverlapCount() 으로 수량얻는거 버그 생길 가능성 있다.
					// 중첩되는 아이템으로 나올 경우 장비류와 달리 실제 얻은 개수와 다를 수 있기때문이다.
					// 우선은 문장보옥, 조합, 강화에서만 사용되니 1개 고정이라 우선 이렇게 하고 다음에 패킷에다 개수 추가하던지 해서 처리해야겠다.
					GetInterface().ShowItemAlarmDialog( wszMsg, pItem, pItem->GetOverlapCount(), textcolor::WHITE, 2.0f );
				}
			}
			break;
		case 2:
			{
				swprintf_s( wszMsg, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8023 ), cLevel );
				GetInterface().ShowItemAlarmDialog( wszMsg, NULL, 0, textcolor::WHITE, 2.0f );
			}
			break;
		case 3:
			{
				swprintf_s( wszMsg, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8025 ) );
				GetInterface().ShowItemAlarmDialog( wszMsg, NULL, 0, textcolor::WHITE, 2.0f );
			}
			break;
	}

	// 기본상태로 초기화.
	SAFE_DELETE(m_pUpgradeItem);
	m_pUpgradeItemSlot->ResetSlot();
	for( int i = 0; i < ITEMSLOT_MAX; ++i )
		m_pItemSlotButton[i]->ResetSlot();

#ifdef PRE_ADD_DIRECT_BUY_UPGRADEITEM 
	ResetUpgradeItemBtn();
#endif

	m_pStaticGold->SetIntToText(0);
	m_pStaticSilver->SetIntToText(0);
	m_pStaticBronze->SetIntToText(0);
	m_pStaticGold->SetTextColor( m_dwColorGold );
	m_pStaticSilver->SetTextColor( m_dwColorSilver );
	m_pStaticBronze->SetTextColor( m_dwColorBronze );

	m_pButtonOK->Enable( false );
	m_pButtonCancel->Enable( true );
	m_pGuardItemSlotButton->ResetSlot();

#if defined(PRE_ADD_ENCHANTSHIELD_CASHITEM)
	int nPrevGuardIndex = m_pComboBoxGuard->GetSelectedIndex();
	int nPrevGuardValue = 0;
	m_pComboBoxGuard->GetSelectedValue(nPrevGuardValue);
#else
	bool bPrevGuardItem = m_pCheckGuard->IsChecked();	
	m_pCheckGuard->SetChecked( false );
	m_pCheckGuard->Enable( false );
#ifdef PRE_ADD_ITEM_ENCHANT_TOOLTIP
	m_pCheckGuard->SetTooltipText( L"" );
#endif
	m_pStaticTextBase->Show( true );
	m_pStaticTextUse->Show( false );
	m_pStaticTextNotUse->SetText( L"" );
#ifdef PRE_ADD_JELLYCOUNT
	m_pStaticTextNotUse2->Show( false );
#endif
#endif	
	m_pStaticTextCount->SetText( L"0 / 0" );
	m_pButtonDetail->Show( false );
	m_pStaticGuard->Show( false );

#ifdef PRE_ADD_EQUIPED_ITEM_ENCHANT 
	CDnSlotButton *pUpgradeSlot = NULL;
	if( m_pControlSlotButton )
	{
		pUpgradeSlot = m_pControlSlotButton;
		m_pControlSlotButton->SetRegist( false );
		m_pControlSlotButton = NULL;
	}
#else 
	CDnQuickSlotButton *pUpgradeSlot = m_pQuickSlotButton;
	if( m_pQuickSlotButton )
	{
		m_pQuickSlotButton->SetRegist( false );
		m_pQuickSlotButton = NULL;
	}
#endif 

	// 아이템이 파괴되지 않았고, Max수치까지 강화한게 아니라면 자동으로 다시 올린다.
	if( pUpgradeSlot && !bBreakItem )
	{
		CDnItem *pItem = static_cast<CDnItem *>(pUpgradeSlot->GetItem());
		if( pItem->GetEnchantLevel() < pItem->GetMaxEnchantLevel() )
		{
			SetUpgradeItem( pUpgradeSlot );
#if defined(PRE_ADD_ENCHANTSHIELD_CASHITEM)
			m_pComboBoxGuard->SetSelectedByIndex(nPrevGuardIndex);
			if( nPrevGuardValue != m_cEnchantGuardType )
			{
				bool bSmartMove = CGameOption::GetInstance().m_bSmartMove;
				if( bSmartMove ) CGameOption::GetInstance().m_bSmartMove = false;
				if( nPrevGuardValue == ENCHANT_ITEM_GUARD )
				{
					// 앞에서 젤리를 썼고 현재는 필요한 갯수가 0이면 12강 이상일듯.
					if( m_nTotalGuardNeeds == 0 )
						GetInterface().MessageBox((FormatW(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8068 ), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1000005253))).c_str() , MB_OK);	// UISTRING : 이 이상의 단계에서는 %s를 사용할 수 없습니다.
					else if( m_nCurGuardCount < m_nTotalGuardNeeds )
#ifdef PRE_ADD_JELLYCOUNT
						GetInterface().MessageBox((FormatW(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8072 ), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1000005253))).c_str() , MB_OK);	// UISTRING : %s가 부족하여 강화 보호 아이템을 사용하지 않는 상태로 변경되었습니다.									
#else
						GetInterface().MessageBox((FormatW(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8048 ), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1000005253))).c_str() , MB_OK);	// UISTRING : %s가 부족하여 사용이 취소되었습니다.				
#endif						
				}
				else if( nPrevGuardValue == ENCHANT_ITEM_SHIELD )
				{
					if( m_nTotalShieldNees == 0)
						GetInterface().MessageBox((FormatW(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8068 ), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8047))).c_str() , MB_OK);	// UISTRING : 이 이상의 단계에서는 %s를 사용할 수 없습니다.
					else if( m_nCurShieldCount < m_nTotalShieldNees )						
#ifdef PRE_ADD_JELLYCOUNT
						GetInterface().MessageBox((FormatW(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8072 ), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8047))).c_str() , MB_OK);	// UISTRING : %s가 부족하여 강화 보호 아이템을 사용하지 않는 상태로 변경되었습니다.
#else
						GetInterface().MessageBox((FormatW(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8048 ), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8047))).c_str() , MB_OK);	// UISTRING : %s가 부족하여 사용이 취소되었습니다.				
#endif
				}
				if( bSmartMove ) CGameOption::GetInstance().m_bSmartMove = true;
			}
#else
			if( bPrevGuardItem && m_pCheckGuard->IsEnable() == false )
			{
				bool bSmartMove = CGameOption::GetInstance().m_bSmartMove;
				if( bSmartMove ) CGameOption::GetInstance().m_bSmartMove = false;

				if( m_nTotalGuardNeeds == 0 )
					GetInterface().MessageBox((FormatW(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8068 ), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1000005253))).c_str() , MB_OK);	// UISTRING : 이 이상의 단계에서는 %s를 사용할 수 없습니다.
				else
#ifdef PRE_ADD_JELLYCOUNT
					GetInterface().MessageBox((FormatW(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8072 ), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1000005253))).c_str() , MB_OK);	// UISTRING : %s가 부족하여 강화 보호 아이템을 사용하지 않는 상태로 변경되었습니다.
#else	
					GetInterface().MessageBox((FormatW(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8048 ), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1000005253))).c_str() , MB_OK);	// UISTRING : %s가 부족하여 사용이 취소되었습니다.
#endif
				if( bSmartMove ) CGameOption::GetInstance().m_bSmartMove = true;
			}
			if( m_pCheckGuard->IsEnable() )
				m_pCheckGuard->SetChecked( bPrevGuardItem );
#endif
		}
		else RefreshNextUpgradeItem();
	}
	else {
		RefreshNextUpgradeItem();
	}

	if( m_nRemoteItemID ) {
		int nCount = 0;
		nCount += GetItemTask().GetCharInventory().GetItemCount( m_nRemoteItemID );
		nCount += GetItemTask().GetCashInventory().GetItemCount( m_nRemoteItemID );
		if( !nCount ) Show( false );
	}

#ifdef PRE_ADD_JELLYCOUNT	

#ifdef PRE_ADD_ENCHANTSHIELD_CASHITEM
	int nEnchantGuardType = (int)ENCHANT_ITEM_NONE;
	m_pComboBoxGuard->GetSelectedValue(nEnchantGuardType);

	if( nEnchantGuardType == ENCHANT_ITEM_GUARD )       // 강화 보호젤리 (파괴x)
	{
		m_pGuardItemSlotButton->SetItem( m_pGuardItemSlotButton->GetItem(), GetItemTask().FindItemCountFromItemType( ITEMTYPE_ENCHANT_BREAKGAURD ) );	
	}
	else if( nEnchantGuardType == ENCHANT_ITEM_SHIELD ) // 상위 강화보호 젤리(파괴x, 다운x)	
	{
		m_pGuardItemSlotButton->SetItem( m_pGuardItemSlotButton->GetItem(), GetItemTask().FindItemCountFromItemType( ITEMTYPE_ENCHANT_SHIELD ) );
	}

#else
	m_pGuardItemSlotButton->SetItem( m_pGuardItemSlotButton->GetItem(), GetItemTask().FindItemCountFromItemType( ITEMTYPE_ENCHANT_BREAKGAURD ) );
#endif // PRE_ADD_ENCHANTSHIELD_CASHITEM

#endif

}

void CDnItemUpgradeDlg::OnRecvEnchantCompleteCash(char cEnchantResult, int nItemID, char cLevel, char cOption)
{
	// cEnchantResult
	// 0 : 강화실패. 아이템파괴됨. nItemID에 값 있을 경우 대체보상.
	// 1 : 강화성공. 아이템파괴안됨.
	// 2 : 레벨다운. 다운값 0 초과.
	// 3 : 레벨다운. 다운값 0.

	bool bBreakItem = false;
	if (cEnchantResult == 0) bBreakItem = true;

	// 인챈트할 아이템이 파괴되지 않았을 때는 패킷으로 온 아이템ID와 강화하려고 등록한 아이템ID가 같은지 확인.
	if (!bBreakItem)
	{
#ifdef PRE_ADD_EQUIPED_ITEM_ENCHANT
		if (m_pQuickSlotButton && !m_bEnchantEquippedItem) // 장착중인 장비의 경우에는 해당 조건검사를 패스한다. 
#else
		if (m_pQuickSlotButton)	// 혹시 NULL일수도 있으니 NULL이면 검증 무시한다.
#endif 
		{
			CDnItem *pItem = static_cast<CDnItem *>(m_pQuickSlotButton->GetItem());
			if (!pItem) return;
			if (pItem->GetClassID() != nItemID) return;
		}
	}

	WCHAR wszMsg[256];
#ifdef PRE_ADD_EQUIPED_ITEM_ENCHANT	
	CDnItem *pItem = CDnItemTask::GetInstance().GetCashInventory().GetItem(m_nSlotIndex);//GetCharInventory().GetItem(m_nSlotIndex);
	//CDnItem *pItem = m_bEnchantEquippedItem ? GetItemTask().GetEquipItem(m_nSlotIndex) : CDnItemTask::GetInstance().GetCharInventory().GetItem(m_nSlotIndex);
#else
	CDnItem *pItem = CDnItemTask::GetInstance().GetCharInventory().GetItem(m_nSlotIndex);
#endif
	if (!pItem) pItem = (nItemID > 0) ? CDnItemTask::GetInstance().GetCashInventory().GetItem(m_nSlotIndex) : NULL;
	switch (cEnchantResult) {
	case 0:
	{
		swprintf_s(wszMsg, _countof(wszMsg), L"\n\n%s", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 8010));
		GetInterface().ShowItemAlarmDialog(wszMsg, NULL, 0, textcolor::WHITE, 2.0f);
	}
	break;
	case 1:
	{
		if (pItem) {
			// 강화 되어있다면,
			swprintf_s(wszMsg, _countof(wszMsg), L"+%d %s", cLevel, pItem->GetName());

			// pItem->GetOverlapCount() 으로 수량얻는거 버그 생길 가능성 있다.
			// 중첩되는 아이템으로 나올 경우 장비류와 달리 실제 얻은 개수와 다를 수 있기때문이다.
			// 우선은 문장보옥, 조합, 강화에서만 사용되니 1개 고정이라 우선 이렇게 하고 다음에 패킷에다 개수 추가하던지 해서 처리해야겠다.
			GetInterface().ShowItemAlarmDialog(wszMsg, pItem, pItem->GetOverlapCount(), textcolor::WHITE, 2.0f);
			//pItem->SetEnchantLevel(pItem->GetEnchantLevel() + 1);
		}
	}
	break;
	case 2:
	{
		swprintf_s(wszMsg, GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 8023), cLevel);
		GetInterface().ShowItemAlarmDialog(wszMsg, NULL, 0, textcolor::WHITE, 2.0f);
	//	pItem->SetEnchantLevel(pItem->GetEnchantLevel() - 1);
	}
	break;
	case 3:
	{
		swprintf_s(wszMsg, GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 8025));
		GetInterface().ShowItemAlarmDialog(wszMsg, NULL, 0, textcolor::WHITE, 2.0f);
		//pItem->SetEnchantLevel(0);
	}
	break;
	}

	// 기본상태로 초기화.
	SAFE_DELETE(m_pUpgradeItem);
	m_pUpgradeItemSlot->ResetSlot();
	for (int i = 0; i < ITEMSLOT_MAX; ++i)
		m_pItemSlotButton[i]->ResetSlot();

#ifdef PRE_ADD_DIRECT_BUY_UPGRADEITEM 
	ResetUpgradeItemBtn();
#endif

	m_pStaticGold->SetIntToText(0);
	m_pStaticSilver->SetIntToText(0);
	m_pStaticBronze->SetIntToText(0);
	m_pStaticGold->SetTextColor(m_dwColorGold);
	m_pStaticSilver->SetTextColor(m_dwColorSilver);
	m_pStaticBronze->SetTextColor(m_dwColorBronze);

	m_pButtonOK->Enable(false);
	m_pButtonCancel->Enable(true);
	m_pGuardItemSlotButton->ResetSlot();

#if defined(PRE_ADD_ENCHANTSHIELD_CASHITEM)
	int nPrevGuardIndex = m_pComboBoxGuard->GetSelectedIndex();
	int nPrevGuardValue = 0;
	m_pComboBoxGuard->GetSelectedValue(nPrevGuardValue);
#else
	bool bPrevGuardItem = m_pCheckGuard->IsChecked();
	m_pCheckGuard->SetChecked(false);
	m_pCheckGuard->Enable(false);
#ifdef PRE_ADD_ITEM_ENCHANT_TOOLTIP
	m_pCheckGuard->SetTooltipText(L"");
#endif
	m_pStaticTextBase->Show(true);
	m_pStaticTextUse->Show(false);
	m_pStaticTextNotUse->SetText(L"");
#ifdef PRE_ADD_JELLYCOUNT
	m_pStaticTextNotUse2->Show(false);
#endif
#endif	
	m_pStaticTextCount->SetText(L"0 / 0");
	m_pButtonDetail->Show(false);
	m_pStaticGuard->Show(false);

#ifdef PRE_ADD_EQUIPED_ITEM_ENCHANT 
	CDnSlotButton *pUpgradeSlot = NULL;
	if (m_pControlSlotButton)
	{
		pUpgradeSlot = m_pControlSlotButton;
		m_pControlSlotButton->SetRegist(false);
		m_pControlSlotButton = NULL;
	}
#else 
	CDnQuickSlotButton *pUpgradeSlot = m_pQuickSlotButton;
	if (m_pQuickSlotButton)
	{
		m_pQuickSlotButton->SetRegist(false);
		m_pQuickSlotButton = NULL;
	}
#endif 

	// 아이템이 파괴되지 않았고, Max수치까지 강화한게 아니라면 자동으로 다시 올린다.
	if (pUpgradeSlot && !bBreakItem)
	{
		CDnItem *pItem = static_cast<CDnItem *>(pUpgradeSlot->GetItem());
		if (!pItem) return;//fix
		if (pItem->GetEnchantLevel() < pItem->GetMaxEnchantLevel())
		{
			SetUpgradeItem(pUpgradeSlot);
#if defined(PRE_ADD_ENCHANTSHIELD_CASHITEM)
			m_pComboBoxGuard->SetSelectedByIndex(nPrevGuardIndex);
			if (nPrevGuardValue != m_cEnchantGuardType)
			{
				bool bSmartMove = CGameOption::GetInstance().m_bSmartMove;
				if (bSmartMove) CGameOption::GetInstance().m_bSmartMove = false;
				if (nPrevGuardValue == ENCHANT_ITEM_GUARD)
				{
					// 앞에서 젤리를 썼고 현재는 필요한 갯수가 0이면 12강 이상일듯.
					if (m_nTotalGuardNeeds == 0)
						GetInterface().MessageBox((FormatW(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 8068), GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 1000005253))).c_str(), MB_OK);	// UISTRING : 이 이상의 단계에서는 %s를 사용할 수 없습니다.
					else if (m_nCurGuardCount < m_nTotalGuardNeeds)
#ifdef PRE_ADD_JELLYCOUNT
						GetInterface().MessageBox((FormatW(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 8072), GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 1000005253))).c_str(), MB_OK);	// UISTRING : %s가 부족하여 강화 보호 아이템을 사용하지 않는 상태로 변경되었습니다.									
#else
						GetInterface().MessageBox((FormatW(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 8048), GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 1000005253))).c_str(), MB_OK);	// UISTRING : %s가 부족하여 사용이 취소되었습니다.				
#endif						
				}
				else if (nPrevGuardValue == ENCHANT_ITEM_SHIELD)
				{
					if (m_nTotalShieldNees == 0)
						GetInterface().MessageBox((FormatW(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 8068), GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 8047))).c_str(), MB_OK);	// UISTRING : 이 이상의 단계에서는 %s를 사용할 수 없습니다.
					else if (m_nCurShieldCount < m_nTotalShieldNees)
#ifdef PRE_ADD_JELLYCOUNT
						GetInterface().MessageBox((FormatW(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 8072), GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 8047))).c_str(), MB_OK);	// UISTRING : %s가 부족하여 강화 보호 아이템을 사용하지 않는 상태로 변경되었습니다.
#else
						GetInterface().MessageBox((FormatW(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 8048), GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 8047))).c_str(), MB_OK);	// UISTRING : %s가 부족하여 사용이 취소되었습니다.				
#endif
				}
				if (bSmartMove) CGameOption::GetInstance().m_bSmartMove = true;
			}
#else
			if (bPrevGuardItem && m_pCheckGuard->IsEnable() == false)
			{
				bool bSmartMove = CGameOption::GetInstance().m_bSmartMove;
				if (bSmartMove) CGameOption::GetInstance().m_bSmartMove = false;

				if (m_nTotalGuardNeeds == 0)
					GetInterface().MessageBox((FormatW(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 8068), GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 1000005253))).c_str(), MB_OK);	// UISTRING : 이 이상의 단계에서는 %s를 사용할 수 없습니다.
				else
#ifdef PRE_ADD_JELLYCOUNT
					GetInterface().MessageBox((FormatW(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 8072), GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 1000005253))).c_str(), MB_OK);	// UISTRING : %s가 부족하여 강화 보호 아이템을 사용하지 않는 상태로 변경되었습니다.
#else	
					GetInterface().MessageBox((FormatW(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 8048), GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 1000005253))).c_str(), MB_OK);	// UISTRING : %s가 부족하여 사용이 취소되었습니다.
#endif
				if (bSmartMove) CGameOption::GetInstance().m_bSmartMove = true;
			}
			if (m_pCheckGuard->IsEnable())
				m_pCheckGuard->SetChecked(bPrevGuardItem);
#endif
		}
		else RefreshNextUpgradeItem();
	}
	else {
		RefreshNextUpgradeItem();
	}

	if (m_nRemoteItemID) {
		int nCount = 0;
		nCount += GetItemTask().GetCharInventory().GetItemCount(m_nRemoteItemID);
		nCount += GetItemTask().GetCashInventory().GetItemCount(m_nRemoteItemID);
		if (!nCount) Show(false);
	}

#ifdef PRE_ADD_JELLYCOUNT	

#ifdef PRE_ADD_ENCHANTSHIELD_CASHITEM
	int nEnchantGuardType = (int)ENCHANT_ITEM_NONE;
	m_pComboBoxGuard->GetSelectedValue(nEnchantGuardType);

	if (nEnchantGuardType == ENCHANT_ITEM_GUARD)       // 강화 보호젤리 (파괴x)
	{
		m_pGuardItemSlotButton->SetItem(m_pGuardItemSlotButton->GetItem(), GetItemTask().FindItemCountFromItemType(ITEMTYPE_ENCHANT_BREAKGAURD));
	}
	else if (nEnchantGuardType == ENCHANT_ITEM_SHIELD) // 상위 강화보호 젤리(파괴x, 다운x)	
	{
		m_pGuardItemSlotButton->SetItem(m_pGuardItemSlotButton->GetItem(), GetItemTask().FindItemCountFromItemType(ITEMTYPE_ENCHANT_SHIELD));
	}

#else
	m_pGuardItemSlotButton->SetItem(m_pGuardItemSlotButton->GetItem(), GetItemTask().FindItemCountFromItemType(ITEMTYPE_ENCHANT_BREAKGAURD));
#endif // PRE_ADD_ENCHANTSHIELD_CASHITEM

#endif

}

void CDnItemUpgradeDlg::CheckUpgradeItem()
{
	int nPrice = 0;
	bool bEmpty = true;

	// 기본적으로 슬롯 초기화.
	for( int i = 0; i < ITEMSLOT_MAX; ++i )
		m_pItemSlotButton[i]->ResetSlot();

	// 먼저 슬롯확인
	if( m_pUpgradeItemSlot && !m_pUpgradeItemSlot->IsEmptySlot() )
		bEmpty = false;

	if( bEmpty )
	{
		m_pButtonOK->Enable( false );
	}
	else
	{
		// NULL일리 없겠지만, 그래도 검사.
		if( m_pUpgradeItem )
		{
			int nEnchantTableItemID = GetEnchantTableItemID( m_pUpgradeItem );
			ASSERT(nEnchantTableItemID&&"EnchantTableItemID가 0이 될리가 없는데.");
			m_pButtonOK->Enable( true );
			if( !CheckNeedItemInfo( nEnchantTableItemID ) )
				m_pButtonOK->Enable( false );

			if( (nPrice = GetEnchantPrice( nEnchantTableItemID )) > GetItemTask().GetCoin() )
				m_pButtonOK->Enable( false );
		}
	}

	int nGold = nPrice/10000;
	int nSilver = (nPrice%10000)/100;
	int nBronze = nPrice%100;

	m_pStaticGold->SetIntToText( nGold );
	m_pStaticSilver->SetIntToText( nSilver );
	m_pStaticBronze->SetIntToText( nBronze );
	if( nPrice > GetItemTask().GetCoin() )
	{
		if( nPrice >= 10000 ) m_pStaticGold->SetTextColor( 0xFFFF0000, true );
		if( nPrice >= 100 ) m_pStaticSilver->SetTextColor( 0xFFFF0000, true );
		m_pStaticBronze->SetTextColor( 0xFFFF0000, true );
	}
	else
	{
		m_pStaticGold->SetTextColor( m_dwColorGold );
		m_pStaticSilver->SetTextColor( m_dwColorSilver );
		m_pStaticBronze->SetTextColor( m_dwColorBronze );
	}

	CheckUpgradeGuardItem();
	RefreshNextUpgradeItem();
}

int CDnItemUpgradeDlg::GetEnchantTableItemID( CDnItem *pItem ) const
{
	if( !pItem ) return 0;
	char cEnchantLevel = pItem->GetEnchantLevel();
	int nEnchantID = pItem->GetEnchantID();

#ifdef PRE_FIX_MEMOPT_ENCHANT
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TENCHANT_MAIN );
#else
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TENCHANT );
#endif
	ASSERT(pSox&&"왜 EnchantTable 못읽어오나.");
	if( !pSox ) return 0;

	int nEnchantTableItemID = 0;
	std::vector<int> nVecItemID;
	pSox->GetItemIDListFromField( "_EnchantID", nEnchantID, nVecItemID );

	for( DWORD i=0; i<nVecItemID.size(); i++ )
	{
		// 현재 강화레벨보다 1더한 강화테이블의 가격을 얻어와야한다.
		if( pSox->GetFieldFromLablePtr( nVecItemID[i], "_EnchantLevel" )->GetInteger() == (cEnchantLevel+1) )
		{
			nEnchantTableItemID = nVecItemID[i];
			break;
		}
	}
	return nEnchantTableItemID;
}

int CDnItemUpgradeDlg::GetEnchantPrice( int nEnchantTableItemID )
{
#ifdef PRE_FIX_MEMOPT_ENCHANT
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TENCHANT_MAIN );
#else
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TENCHANT );
#endif

#ifdef PRE_ADD_NPC_REPUTATION_SYSTEM
	// 호감도 시스템에 강화 비용 할인 조건이 되는지 체크 및 계산.
	int iNeedCoin = pSox->GetFieldFromLablePtr( nEnchantTableItemID, "_NeedCoin" )->GetInteger();

	if( m_nRemoteItemID )
	{
		int nTypeParam = CDnItem::GetItemTypeParam( m_nRemoteItemID );
		int nDiscount = iNeedCoin * nTypeParam / 100;
		iNeedCoin -= nDiscount;
	}
	else
	{
		bool bAvailBenefit = GetQuestTask().CheckAndCalcStoreBenefit( NpcReputation::StoreBenefit::EnchantFeeDiscount, iNeedCoin );
		m_pReputationBenefit->Show( bAvailBenefit );
	}

	return iNeedCoin;
#else
	int nNeedCoin = pSox->GetFieldFromLablePtr( nEnchantTableItemID, "_NeedCoin" )->GetInteger();
	if( m_nRemoteItemID )
	{
		int nTypeParam = CDnItem::GetItemTypeParam( m_nRemoteItemID );
		int nDiscount = nNeedCoin * nTypeParam / 100;
		nNeedCoin -= nDiscount;
	}
	return nNeedCoin;
#endif //#ifdef PRE_ADD_NPC_REPUTATION_SYSTEM
}

bool CDnItemUpgradeDlg::CheckNeedItemInfo( int nEnchantTableItemID )
{
	// 다이얼로그에서 Table에 접근하기 싫었지만,
	// DnItem에서 읽기엔 매번 사용하는 데이터도 아니고,
	// 그렇다고 ItemCompounder처럼 별도의 클래스를 만들기엔 별로라.
	// (기능은 이미 DnItem에서 InitialEnchant에서 처리하고, 여기서는 필요아이템만 있으면 끝이라)
	// 그냥 여기서 해당 아이템 보고 테이블에서 읽어오기로 하겠다.
	//
#ifdef PRE_FIX_MEMOPT_ENCHANT
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TENCHANT_MAIN );
	DNTableFileFormat*  pNeedItemSox = GetDNTable( CDnTableDB::TENCHANT_NEEDITEM );
	if (pSox == NULL || pNeedItemSox == NULL)
		return false;

	int needItemTableID = pSox->GetFieldFromLablePtr(nEnchantTableItemID, "_NeedItemTableID")->GetInteger();
#else
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TENCHANT );
#endif

	int nItemID;
	int nItemCount;

	bool bOK = true;
	char szLabel[32];
	
#ifdef PRE_ADD_DIRECT_BUY_UPGRADEITEM
	ResetUpgradeItemBtn();
#endif

	for( int i=0; i<ITEMSLOT_MAX; i++ )
	{
#ifdef PRE_FIX_MEMOPT_ENCHANT
		sprintf_s( szLabel, "_NeedItemID%d", i + 1 );
		nItemID = pNeedItemSox->GetFieldFromLablePtr( needItemTableID, szLabel )->GetInteger();
		sprintf_s( szLabel, "_NeedItemCount%d", i + 1 );
		nItemCount = pNeedItemSox->GetFieldFromLablePtr( needItemTableID, szLabel )->GetInteger();
#else
		sprintf_s( szLabel, "_NeedItemID%d", i + 1 );
		nItemID = pSox->GetFieldFromLablePtr( nEnchantTableItemID, szLabel )->GetInteger();
		sprintf_s( szLabel, "_NeedItemCount%d", i + 1 );
		nItemCount = pSox->GetFieldFromLablePtr( nEnchantTableItemID, szLabel )->GetInteger();
#endif

		if( nItemID <= 0 )
			continue;

		// 인벤토리를 보면서 종류과 갯수 확인.
		int nCurItemCount = GetItemTask().GetCharInventory().GetItemCount( nItemID );
		if( nCurItemCount < nItemCount )
			bOK = false;

		// 갯수가 모자라도 설정은 하고 넘어간다.
		SetItemSlot( i, nItemID, nItemCount, nCurItemCount );
	}
	return bOK;
}

void CDnItemUpgradeDlg::SetItemSlot( int nSlotIndex, int nItemID, int nNeedItemCount, int nCurItemCount )
{
	if( nItemID == 0 )
		return;

	SAFE_DELETE( m_pItem[nSlotIndex] );

	TItemInfo itemInfo;	
	if( CDnItem::MakeItemInfo( nItemID, 1, itemInfo ) == false ) return;
	m_pItem[nSlotIndex] = GetItemTask().CreateItem( itemInfo );

	// 기본 수량은 1로 설정하고 실제로 카운트는 다르게 렌더링한다.
	m_pItemSlotButton[nSlotIndex]->SetItem(m_pItem[nSlotIndex], CDnSlotButton::ITEM_ORIGINAL_COUNT);
	m_pItemSlotButton[nSlotIndex]->SetJewelCount( nNeedItemCount, nCurItemCount );

	if( nCurItemCount < nNeedItemCount )
		m_pItemSlotButton[nSlotIndex]->SetRegist( true );
	else
		m_pItemSlotButton[nSlotIndex]->SetRegist( false );

#ifdef PRE_ADD_DIRECT_BUY_UPGRADEITEM
	// 마을인지 체크 ( 마을에서만, 사용한 컨텐츠 )
	if( CDnWorld::MapTypeEnum::MapTypeVillage == CDnWorld::GetInstancePtr()->GetMapType() )
	{
		if(!m_pUpgradeItemButton[nSlotIndex]) return;
		m_pUpgradeItemButton[nSlotIndex]->Show(true);

		// 귀속 아이템인지 체크.
		if(!m_pItem[nSlotIndex]) return;
		bool bIsEnable = true;
		if( m_pItem[nSlotIndex]->GetReversion() > CDnItem::NoReversion )
			bIsEnable = false;
		m_pUpgradeItemButton[nSlotIndex]->Enable(bIsEnable);
	}
#endif
}

void CDnItemUpgradeDlg::CheckUpgradeGuardItem()
{
	m_nTotalGuardNeeds = 0;
	m_pGuardItemSlotButton->ResetSlot();
	SAFE_DELETE(m_pGuardItem);

#if defined(PRE_ADD_ENCHANTSHIELD_CASHITEM)
	m_nTotalShieldNees = 0;
	m_nCurGuardCount = 0;
	m_nCurShieldCount = 0;
	m_cEnchantGuardType = ENCHANT_ITEM_NONE;
	m_pComboBoxGuard->RemoveAllItems();
	m_pComboBoxGuard->SetTextColor(textcolor::RED);
	m_pComboBoxGuard->AddItem(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8067 ),NULL, ENCHANT_ITEM_NONE); // UISTRING : 사용 안함
#else

#if defined(PRE_ADD_DIRECT_BUY_UPGRADEITEM) && !defined(PRE_ADD_ENCHANTSHIELD_CASHITEM)
	bool bIsCheckGuard = false;
	if(m_pCheckGuard)
		bIsCheckGuard = m_pCheckGuard->IsChecked();
#endif
	m_pCheckGuard->SetChecked( false );
	m_pCheckGuard->Enable( false );
#ifdef PRE_ADD_ITEM_ENCHANT_TOOLTIP
	m_pCheckGuard->SetTooltipText( L"" );
#endif
	m_pStaticTextBase->Show( true );
	m_pStaticTextUse->Show( false );
	m_pStaticTextNotUse->SetText( L"" );
	m_pStaticTextNotUse2->Show( false ); // #55413.
#endif	
	m_pStaticTextCount->SetText( L"0 / 0" );
	m_pButtonDetail->Show( false );
	m_pStaticGuard->Show( false );

	if( !m_pUpgradeItem ) return;

#ifdef PRE_FIX_MEMOPT_ENCHANT
	// 보호 아이템 개수
	int nEnchantTableItemID = GetEnchantTableItemID( m_pUpgradeItem );
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TENCHANT_MAIN );
	DNTableFileFormat* pNeedItemSox = GetDNTable( CDnTableDB::TENCHANT_NEEDITEM );
	int needItemTableID = pSox->GetFieldFromLablePtr(nEnchantTableItemID, "_NeedItemTableID")->GetInteger();
	if (pSox && pNeedItemSox && nEnchantTableItemID && needItemTableID)
	{
		m_nTotalGuardNeeds = pNeedItemSox->GetFieldFromLablePtr( needItemTableID, "_ProtectItemCount" )->GetInteger();
#if defined(PRE_ADD_ENCHANTSHIELD_CASHITEM) // 상위 강화보호젤리
		DNTableCell* pField = pNeedItemSox->GetFieldFromLablePtr( needItemTableID, "_ShieldItemCount");
		if( pField )
			m_nTotalShieldNees = pField->GetInteger();
#endif
	}
#else // PRE_FIX_MEMOPT_ENCHANT
	// 보호 아이템 개수
	int nEnchantTableItemID = GetEnchantTableItemID( m_pUpgradeItem );
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TENCHANT );
	if( pSox && nEnchantTableItemID )
	{
		m_nTotalGuardNeeds = pSox->GetFieldFromLablePtr( nEnchantTableItemID, "_ProtectItemCount" )->GetInteger();
#if defined(PRE_ADD_ENCHANTSHIELD_CASHITEM) // 상위 강화보호젤리
		DNTableCell* pField = pSox->GetFieldFromLablePtr( nEnchantTableItemID, "_ShieldItemCount");
		if( pField )
			m_nTotalShieldNees = pField->GetInteger();		
#endif
	}
#endif // PRE_FIX_MEMOPT_ENCHANT

#if defined(PRE_ADD_ENCHANTSHIELD_CASHITEM)

#ifdef PRE_ADD_JELLYCOUNT

	int nItemID = 0;
	int nItemCount = 0;

	// 콤보박스 설정.강화보호젤리, 상위 강화보호젤리	
	if( m_nTotalGuardNeeds > 0 )
	{
		nItemID = m_nGuardItemID = ENCHANTGUARD_ITEMID;
		nItemCount = m_nCurGuardCount = GetItemTask().FindItemCountFromItemType( ITEMTYPE_ENCHANT_BREAKGAURD );
		if( m_nCurGuardCount >= m_nTotalGuardNeeds )
		{
			m_pComboBoxGuard->AddItem(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1000005253 ),NULL, ENCHANT_ITEM_GUARD);	// UISTRING : 아이템 보호 마법젤리
		}
		else
		{
			WCHAR wszTemp[128] = {0, };
			swprintf_s( wszTemp, L"%s (%s)", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1000005253), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3816));
			m_pComboBoxGuard->AddItem(wszTemp, NULL, ENCHANT_ITEM_GUARD);	// UISTRING : 아이템 보호 마법젤리 (사용불가)
		}
	}

	if( m_nTotalShieldNees > 0 )
	{
		nItemID = m_nShiledItemID = ENCHANTSHIELD_ITEMID;
		nItemCount = m_nCurShieldCount = GetItemTask().FindItemCountFromItemType( ITEMTYPE_ENCHANT_SHIELD );
		if( m_nCurShieldCount >= m_nTotalShieldNees )
		{
			m_pComboBoxGuard->AddItem(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8047 ),NULL, ENCHANT_ITEM_SHIELD);    // UISTRING : 상위 아이템 보호 마법젤리
		}
		else
		{
			WCHAR wszTemp[128] = {0, };
			swprintf_s( wszTemp, L"%s (%s)", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8047), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3816));
			m_pComboBoxGuard->AddItem(wszTemp, NULL, ENCHANT_ITEM_SHIELD);	// UISTRING : 상위 아이템 보호 마법젤리 (사용불가)
		}
	}

	TItemInfo itemInfo;	
	if( CDnItem::MakeItemInfo( nItemID, 1, itemInfo ) == false ) return;	// 보호아이템 생성 못하면 그냥 기본상태에서 진행 안함.
	m_pGuardItem = GetItemTask().CreateItem( itemInfo );
	m_pGuardItem->SetInfoItem(true);

	m_pGuardItemSlotButton->SetItem( m_pGuardItem, nItemCount );

	// #58332.
	int nEnchantGuardType;
	if( m_pComboBoxGuard->GetSelectedValue(nEnchantGuardType) )
	{
		if( nEnchantGuardType == ENCHANT_ITEM_NONE ) // 사용안함은 따로 셋팅할 필요가 없다.
			m_pGuardItemSlotButton->ResetSlot();
	}


#else

	// 콤보박스 설정.강화보호젤리, 상위 강화보호젤리	
	if( m_nTotalGuardNeeds > 0 )
	{
		m_nGuardItemID = ENCHANTGUARD_ITEMID;
		m_nCurGuardCount = GetItemTask().FindItemCountFromItemType( ITEMTYPE_ENCHANT_BREAKGAURD );
		if( m_nCurGuardCount >= m_nTotalGuardNeeds )
		{
			m_pComboBoxGuard->AddItem(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1000005253 ),NULL, ENCHANT_ITEM_GUARD);	// UISTRING : 아이템 보호 마법젤리
		}
		else
		{
			WCHAR wszTemp[128] = {0, };
			swprintf_s( wszTemp, L"%s (%s)", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1000005253), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3816));
			m_pComboBoxGuard->AddItem(wszTemp, NULL, ENCHANT_ITEM_GUARD);	// UISTRING : 아이템 보호 마법젤리 (사용불가)
		}
	}

	if( m_nTotalShieldNees > 0 )
	{
		m_nShiledItemID = ENCHANTSHIELD_ITEMID;
		m_nCurShieldCount = GetItemTask().FindItemCountFromItemType( ITEMTYPE_ENCHANT_SHIELD );
		if( m_nCurShieldCount >= m_nTotalShieldNees )
		{
			m_pComboBoxGuard->AddItem(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8047 ),NULL, ENCHANT_ITEM_SHIELD);    // UISTRING : 상위 아이템 보호 마법젤리
		}
		else
		{
			WCHAR wszTemp[128] = {0, };
			swprintf_s( wszTemp, L"%s (%s)", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8047), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3816));
			m_pComboBoxGuard->AddItem(wszTemp, NULL, ENCHANT_ITEM_SHIELD);	// UISTRING : 상위 아이템 보호 마법젤리 (사용불가)
		}
	}

#endif // PRE_ADD_JELLYCOUNT

#else
	// 슬롯을 설정. 만약 인벤에 가드 아이템이 없다면, 프로그램에서 설정된 ID로 강제로 생성해서 보여준다.
	bool bCheckNeedItem = false;
	int nGuardItemID = 0;
	int nGuardItemTotalCount = 0;
	nGuardItemID = ENCHANTGUARD_ITEMID;
	nGuardItemTotalCount = GetItemTask().FindItemCountFromItemType( ITEMTYPE_ENCHANT_BREAKGAURD );
	if( nGuardItemTotalCount >= m_nTotalGuardNeeds )
		bCheckNeedItem = true;

	TItemInfo itemInfo;	
	if( CDnItem::MakeItemInfo( nGuardItemID, 1, itemInfo ) == false ) return;	// 보호아이템 생성 못하면 그냥 기본상태에서 진행 안함.
	m_pGuardItem = GetItemTask().CreateItem( itemInfo );
	m_pGuardItem->SetInfoItem(true);

#ifdef PRE_ADD_JELLYCOUNT
	m_pGuardItemSlotButton->SetItem( m_pGuardItem, nGuardItemTotalCount );
#else
	m_pGuardItemSlotButton->SetItem( m_pGuardItem, CDnSlotButton::ITEM_ORIGINAL_COUNT );
#endif

	m_pGuardItemSlotButton->SetRegist( true );

	// 0개로 설정되었다는 이야기는 이번 강화단계에서 보호아이템을 사용할 수 없다는 이야기다.
	// 이땐 강제로 강화보호아이템을 사용할 수 없는 상태로 바꾼다.
	if( m_nTotalGuardNeeds == 0 )
		bCheckNeedItem = false;

	WCHAR wszText[64] = {0,};
	swprintf_s( wszText, L"%d / %d", min(nGuardItemTotalCount, m_nTotalGuardNeeds), m_nTotalGuardNeeds );
	m_pStaticTextCount->SetText( wszText );
	CheckUseItemTooltip();

	m_pStaticTextBase->Show( false );

	if( bCheckNeedItem )
	{
		m_pCheckGuard->Enable( bCheckNeedItem );
		m_pStaticTextNotUse->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8027 ) );

#ifdef PRE_ADD_DIRECT_BUY_UPGRADEITEM
		// 아이템 원격 구매후, 강화젤리 체크박스 자동해제 방지.
		if(m_bIsMiniMarket && bIsCheckGuard)
		{
			m_pCheckGuard->SetChecked(true);
			m_pStaticTextUse->Show(true);
		}
		else
		{
			m_pCheckGuard->SetChecked( false );

#ifdef PRE_ADD_JELLYCOUNT
			m_pStaticTextNotUse->Show( false );
			m_pStaticTextNotUse2->Show( true );		
#endif // PRE_ADD_JELLYCOUNT
		}
		m_bIsMiniMarket = false;
#else
		m_pCheckGuard->SetChecked( false );

	#ifdef PRE_ADD_JELLYCOUNT
			m_pStaticTextNotUse->Show( false );
			m_pStaticTextNotUse2->Show( true );		
	#endif // PRE_ADD_JELLYCOUNT

#endif // PRE_ADD_DIRECT_BUY_UPGRADEITEM
	}
	else
	{
#ifdef PRE_ADD_JELLYCOUNT
		m_pStaticTextNotUse->Show( true );
#endif
		// #54793.
		//m_pStaticTextNotUse->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, m_nTotalGuardNeeds ? 8028 : 8056 ) );
		m_pStaticTextNotUse->SetTextColor( m_nTotalGuardNeeds ? textcolor::ORANGE : textcolor::DARKGRAY );
		m_pStaticTextNotUse->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, m_nTotalGuardNeeds ? 8098 : 8056 ) );

#ifdef PRE_ADD_ITEM_ENCHANT_TOOLTIP
		m_pCheckGuard->SetTooltipText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8342 ) );
#endif
	}


#endif
}

#if defined(PRE_ADD_ENCHANTSHIELD_CASHITEM)
void CDnItemUpgradeDlg::SetUpgardeGuardItem()
{	
	m_cEnchantGuardType = ENCHANT_ITEM_NONE;
	m_pGuardItemSlotButton->ResetSlot();
	SAFE_DELETE(m_pGuardItem);
	int nEnchantGuardType;
	if( !m_pComboBoxGuard->GetSelectedValue(nEnchantGuardType) )
		return;

	if( nEnchantGuardType == ENCHANT_ITEM_NONE ) // 사용안함은 따로 셋팅할 필요가 없다.
	{
		m_pStaticTextCount->SetText( L"0 / 0" );
		m_pButtonDetail->Show( false );
		m_pStaticGuard->Show( false );
		m_pComboBoxGuard->SetTextColor(textcolor::RED);
		return;
	}

	TItemInfo itemInfo;	
	int nNeedCount = 0, nCurCount = 0;
	switch(nEnchantGuardType)
	{	
	case ENCHANT_ITEM_GUARD:
		{
			if( CDnItem::MakeItemInfo( m_nGuardItemID, 1, itemInfo ) == false )
				return;	// 보호아이템 생성 못하면 그냥 기본상태에서 진행 안함.
			m_cEnchantGuardType = ENCHANT_ITEM_GUARD;
			nNeedCount = m_nTotalGuardNeeds;
			nCurCount = m_nCurGuardCount;
		}
		break;
	case ENCHANT_ITEM_SHIELD:
		{
			if( CDnItem::MakeItemInfo( m_nShiledItemID, 1, itemInfo ) == false )
				return;	// 보호아이템 생성 못하면 그냥 기본상태에서 진행 안함.
			m_cEnchantGuardType = ENCHANT_ITEM_SHIELD;
			nNeedCount = m_nTotalShieldNees;
			nCurCount = m_nCurShieldCount;
		}
		break;

	default:
		return;
	}
	m_pGuardItem = GetItemTask().CreateItem( itemInfo );
	m_pGuardItem->SetInfoItem(true);

#ifdef PRE_ADD_JELLYCOUNT	
	m_pGuardItemSlotButton->SetItem( m_pGuardItem, nCurCount );	
#else
	m_pGuardItemSlotButton->SetItem( m_pGuardItem, CDnSlotButton::ITEM_ORIGINAL_COUNT );	
#endif

	if( nNeedCount <= nCurCount )
	{
		m_pGuardItemSlotButton->SetRegist( false );
		m_pStaticGuard->Show( true );
		m_pComboBoxGuard->SetTextColor(textcolor::WHITE);
	}
	else
	{
		m_cEnchantGuardType = ENCHANT_ITEM_NONE;
		m_pGuardItemSlotButton->SetRegist( true );
		m_pStaticGuard->Show( false );
		m_pComboBoxGuard->SetTextColor(textcolor::RED);
	}
	WCHAR wszText[64] = {0,};
	swprintf_s( wszText, L"%d / %d", min(nCurCount, nNeedCount), nNeedCount );
	m_pStaticTextCount->SetText( wszText );	
	CheckUseItemTooltip();
}
#endif //#if defined(PRE_ADD_ENCHANTSHIELD_CASHITEM)

#ifdef PRE_ADD_EQUIPED_ITEM_ENCHANT
bool  CDnItemUpgradeDlg::IsUpgradeItem( CDnSlotButton *pPressedButton )
{
	CDnItem *pItem = static_cast<CDnItem *>(pPressedButton->GetItem() );

	// 퀵슬롯일 경우에만 중복 검사 
	if( m_pControlSlotButton && 
		m_pControlSlotButton->GetSlotType() == ST_QUICKSLOT && 
		(pItem == m_pControlSlotButton->GetItem() ) )
	{
		return false;
	}
	// 아이템 종류 검사
	if( pItem->GetItemType() == ITEMTYPE_QUEST )
	{
		GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3609 ), MB_OK );
		return false;
	}

	// 강화가 안되는 아이템인지 검사.
	if( !pItem->GetEnchantID() )
	{
		GetInterface().MessageBox( 8007, MB_OK );
		return false;
	}

	// 혹시 강화가 다 된건지 확인
	if( pItem->GetEnchantLevel() >= pItem->GetMaxEnchantLevel() )
	{
		GetInterface().MessageBox( 8006, MB_OK );
		return false;
	}

	// 귀속아이템이면서 봉인상태의 아이템인지 확인
	if( pItem->GetReversion() > CDnItem::NoReversion && !pItem->IsSoulbBound() )
	{
		GetInterface().MessageBox( 1748, MB_OK );
		return false;
	}

	return true;
};

void CDnItemUpgradeDlg::SetUpgradeItem( CDnSlotButton *pPressedButton )
{
	CDnItem *pItem = static_cast<CDnItem *>(pPressedButton->GetItem() );

	// 칸이 하나이므로 더 이상 등록할 수 없습니다.보단 재등록이 낫다.
	CDnItem *pOriginItem = static_cast<CDnItem *>(m_pUpgradeItemSlot->GetItem());
	if( pOriginItem )
	{
		// 해당 아이템을 창에서 제거한다.
		SAFE_DELETE(m_pUpgradeItem);
		m_pUpgradeItemSlot->ResetSlot();	
		if( m_pControlSlotButton ) 
		{
			m_pControlSlotButton->SetRegist( false );
			m_pControlSlotButton = NULL;
		}
	}
	
	std::wstring wszError;
	// 이 함수에서는 강화 슬롯에 인벤슬롯 or 장착슬롯만 온다. 
	// #70419 
	if( pPressedButton->GetSlotType() == ST_INVENTORY )
		m_bEnchantEquippedItem = false;
	else
		m_bEnchantEquippedItem = true;

	// 강화아이템을 등록할때 전처럼 이렇게 인벤토리 아이템의 포인터를 직접 넘기지 않고,
	//pPressedButton->SetItem( pItem );
	// 이렇게 임시템을 만들어서 넣어준다.
	TItemInfo itemInfo;
	pItem->GetTItemInfo(itemInfo);
	m_pUpgradeItem = GetItemTask().CreateItem(itemInfo);
	m_pUpgradeItemSlot->SetItem( m_pUpgradeItem, CDnSlotButton::ITEM_ORIGINAL_COUNT);

	CheckUpgradeItem();
	CEtSoundEngine::GetInstance().PlaySound( "2D", pItem->GetDragSoundIndex() );

	m_pControlSlotButton = static_cast<CDnSlotButton*>( pPressedButton );
	if( m_pControlSlotButton ) m_pControlSlotButton->SetRegist( true );

}
#else 
void CDnItemUpgradeDlg::SetUpgradeItem( CDnQuickSlotButton *pPressedButton )
{
	CDnItem *pItem = static_cast<CDnItem *>(pPressedButton->GetItem() );

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

	// 강화가 안되는 아이템인지 검사.
	if( !pItem->GetEnchantID() )
	{
		GetInterface().MessageBox( 8007, MB_OK );
		return;
	}

	// 혹시 강화가 다 된건지 확인
	if( pItem->GetEnchantLevel() >= pItem->GetMaxEnchantLevel() )
	{
		GetInterface().MessageBox( 8006, MB_OK );
		return;
	}

	// 귀속아이템이면서 봉인상태의 아이템인지 확인
	if( pItem->GetReversion() > CDnItem::NoReversion && !pItem->IsSoulbBound() )
	{
		GetInterface().MessageBox( 1748, MB_OK );
		return;
	}

	// 칸이 하나이므로 더 이상 등록할 수 없습니다.보단 재등록이 낫다.
	CDnItem *pOriginItem = static_cast<CDnItem *>(m_pUpgradeItemSlot->GetItem());
	if( pOriginItem )
	{
		// 해당 아이템을 창에서 제거한다.
		SAFE_DELETE(m_pUpgradeItem);
		m_pUpgradeItemSlot->ResetSlot();
		m_pQuickSlotButton->SetRegist( false );
		m_pQuickSlotButton = NULL;
	}

	// 강화아이템을 등록할때 전처럼 이렇게 인벤토리 아이템의 포인터를 직접 넘기지 않고,
	//pPressedButton->SetItem( pItem );
	// 이렇게 임시템을 만들어서 넣어준다.
	TItemInfo itemInfo;
	pItem->GetTItemInfo(itemInfo);
	m_pUpgradeItem = GetItemTask().CreateItem(itemInfo);
	m_pUpgradeItemSlot->SetItem(m_pUpgradeItem, CDnSlotButton::ITEM_ORIGINAL_COUNT);

	CheckUpgradeItem();
	CEtSoundEngine::GetInstance().PlaySound( "2D", pItem->GetDragSoundIndex() );

	m_pQuickSlotButton = (CDnQuickSlotButton *)pPressedButton;
	m_pQuickSlotButton->SetRegist( true );
}
#endif // PRE_ADD_EQUIPED_ITEM_ENCHANT

void CDnItemUpgradeDlg::CheckUseItemTooltip()
{
	std::vector<CDnItem*> pVecGuardItem;
	std::vector<int> VecItemID;
	std::vector<bool> VecEternity;
#if defined(PRE_ADD_ENCHANTSHIELD_CASHITEM)
	int nEnchantGuardType = 0;
	m_pComboBoxGuard->GetSelectedValue( nEnchantGuardType );
	if( nEnchantGuardType == ENCHANT_ITEM_GUARD )
		GetItemTask().FindItemListFromItemTypeEx( ITEMTYPE_ENCHANT_BREAKGAURD, pVecGuardItem );
	else if( nEnchantGuardType == ENCHANT_ITEM_SHIELD )
		GetItemTask().FindItemListFromItemTypeEx( ITEMTYPE_ENCHANT_SHIELD, pVecGuardItem );
#else
	GetItemTask().FindItemListFromItemTypeEx( ITEMTYPE_ENCHANT_BREAKGAURD, pVecGuardItem );
#endif	// #if defined(PRE_ADD_ENCHANTSHIELD_CASHITEM)
	CDnItemTask::GetItemListForExtendUseItem( pVecGuardItem, VecItemID, VecEternity );

	std::wstring wszTooltip;
	wszTooltip = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4143 );
	for( int i = 0; i < (int)VecItemID.size(); ++i )
	{
		wszTooltip += L"\n";
		wszTooltip += CDnItem::GetItemFullName( VecItemID[i] );
		if( VecEternity[i] == false ) wszTooltip += GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4144 );
	}
	m_pButtonDetail->Show( true );
	m_pButtonDetail->SetTooltipText( wszTooltip.c_str() );
}

#ifdef PRE_ADD_EQUIPED_ITEM_ENCHANT
void CDnItemUpgradeDlg::SetEquippedItem( int nEquipIndex, MIInventoryItem *pItem )
{
	int nSlotIndex = CDnCharStatusDlg::EQUIPINDEX_2_SLOTINDEX(nEquipIndex);
	if( nSlotIndex < 0 || nSlotIndex > 6 ) return;  
	// EQUIPINDEX_2_SLOTINDEX 에서 EQUIP_WEAPON2 = 6으로 변환된다.

	m_pVecEquippedUpgradeItemSlot[nSlotIndex]->SetItem(pItem, CDnSlotButton::ITEM_ORIGINAL_COUNT);
	m_pVecEquippedUpgradeItemSlot[nSlotIndex]->OnRefreshTooltip();

}

MIInventoryItem *CDnItemUpgradeDlg::GetEquippedItem( int nEquipIndex )
{
	int nSlotIndex = CDnCharStatusDlg::EQUIPINDEX_2_SLOTINDEX(nEquipIndex);
	if( nSlotIndex < 0 || nSlotIndex > 6 ) return NULL;

	return m_pVecEquippedUpgradeItemSlot[nSlotIndex]->GetItem();
}

CDnSlotButton* CDnItemUpgradeDlg::GetEquipSlotButton( int nEquipIndex )
{	
	int nSlotIndex = CDnCharStatusDlg::EQUIPINDEX_2_SLOTINDEX(nEquipIndex);
	if( nSlotIndex < 0 || nSlotIndex > 6 ) return NULL;

	return m_pVecEquippedUpgradeItemSlot[nSlotIndex];
}

void CDnItemUpgradeDlg::ResetEquippedSlot( int nEquipIndex )
{
	int nSlotIndex = CDnCharStatusDlg::EQUIPINDEX_2_SLOTINDEX(nEquipIndex);
	if( nSlotIndex < 0 || nSlotIndex > 6 ) return;

	m_pVecEquippedUpgradeItemSlot[nSlotIndex]->ResetSlot();
	m_pVecEquippedUpgradeItemSlot[nSlotIndex]->OnRefreshTooltip();
}
#endif

void CDnItemUpgradeDlg::RefreshNextUpgradeItem()
{
	SAFE_DELETE( m_pNextUpgradeItem );
	if( m_pUpgradeItem ) {
		switch( m_pUpgradeItem->GetItemType() ) {
			case ITEMTYPE_WEAPON:
			case ITEMTYPE_PARTS:
				if( m_pUpgradeItem->GetEnchantLevel() >= m_pUpgradeItem->GetMaxEnchantLevel() ) break;
				TItemInfo itemInfo;
				m_pUpgradeItem->GetTItemInfo(itemInfo);
				itemInfo.Item.cLevel += 1;
				itemInfo.Item.nRandomSeed = m_pUpgradeItem->GetSeed();
				m_pNextUpgradeItem = GetItemTask().CreateItem(itemInfo);
				break;
			default:
				break;
		}
	}
	if( m_pNextUpgradeItem ) {
		m_pNextUpgradeItem->SetSealCount( m_pUpgradeItem->GetSealCount() );
		m_pNextUpgradeItem->SetSoulBound( m_pUpgradeItem->IsSoulbBound() );
	}
	m_pStaticNextUpgradeState->Enable( ( m_pNextUpgradeItem ) ? true : false );
}

bool CDnItemUpgradeDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsShow() )
	{
		return false;
	}

	bool bRet = CEtUIDialog::MsgProc( hWnd, uMsg, wParam, lParam );
#ifdef PRE_ADD_NPC_REPUTATION_SYSTEM

	switch( uMsg )
	{
	case WM_MOUSEMOVE:
		{
			float fMouseX = 0;
			float fMouseY = 0;
			POINT MousePoint;
			MousePoint.x = short( LOWORD( lParam ) );
			MousePoint.y = short( HIWORD( lParam ) );
			PointToFloat( MousePoint, fMouseX, fMouseY );

			bool bMouseEnter = false;
			bool bShowNextUpgradeItemTooltip = false;
			if( CDnMouseCursor::GetInstance().IsShowCursor() ) {
				if( m_pReputationBenefit->IsInside( fMouseX, fMouseY ) ) {
					CDnQuestTask* pQuestTask = static_cast<CDnQuestTask*>(CTaskManager::GetInstance().GetTask( "QuestTask" ));
					CReputationSystemRepository* pReputationRepos = pQuestTask->GetReputationRepository();
					if( pQuestTask && pReputationRepos ) {
						bMouseEnter = true;

						float fReputationValue = 0.0f;
						float fReputationValueMax = 100.0f;

						if( CDnActor::s_hLocalActor )
						{
							int iBenefitValue = pQuestTask->GetStoreBenefitValue( NpcReputation::StoreBenefit::EnchantFeeDiscount );

							wchar_t awcBuffer[ 256 ] = { 0 };
							//swprintf_s( awcBuffer, L"호감도 효과 발동중\n강화비용 %d%% 할인 (UIString 에 없는 임시 텍스트)", iBenefitValue );
							swprintf_s( awcBuffer, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3219 ), iBenefitValue );
							m_pStoreBenefitTooltip->SetText( awcBuffer );
							ShowChildDialog( m_pStoreBenefitTooltip, true );

							SUICoord dlgCoord;
							m_pStoreBenefitTooltip->GetDlgCoord( dlgCoord );
							dlgCoord.fX = fMouseX + GetXCoord() - (dlgCoord.fWidth / 2.0f);
							dlgCoord.fY = fMouseY + GetYCoord() - dlgCoord.fHeight - 0.004f;
							m_pStoreBenefitTooltip->SetDlgCoord( dlgCoord );
						}
					}
				}
				else if( m_pStaticNextUpgradeState->IsInside( fMouseX, fMouseY ) ) {
					bMouseEnter = true;
					if( m_pNextUpgradeItem ) bShowNextUpgradeItemTooltip = true;
				}
#ifdef PRE_ADD_DIRECT_BUY_UPGRADEITEM
				for( int i = 0 ; i < ITEMSLOT_MAX ; ++i )
				{
					if( !m_pUpgradeItemButton[i]->IsShow() )
						continue;

					if( !m_pUpgradeItemButton[i]->IsEnable() && m_pUpgradeItemButton[i]->IsInside(fMouseX, fMouseY) )
					{
						wchar_t awcBuffer[ 256 ] = { 0 };
						swprintf_s( awcBuffer, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4169 ) );
						m_pUpgradeItemButton[i]->SetTooltipText(awcBuffer);
					}
				}					
#endif
			}
			if( !bMouseEnter || !m_pReputationBenefit->IsShow() )
			{
				ShowChildDialog( m_pStoreBenefitTooltip, false );
			}

			ShowNextUpgradeToolTip( bShowNextUpgradeItemTooltip, fMouseX, fMouseY );
		}
		break;
	}
#endif // #ifdef PRE_ADD_NPC_REPUTATION_SYSTEM

	return bRet;
}

void CDnItemUpgradeDlg::ShowNextUpgradeToolTip( bool bShow, float fX, float fY )
{
	CDnTooltipDlg *pToolTip = GetInterface().GetTooltipDialog();
	if( !pToolTip ) return;
	if( bShow ) {
		pToolTip->SetMode(CDnTooltipDlg::TOOLTIP_MANUALCONTROL);
		GetPosition( fX, fY );
		fX += Width();
		fY = fY + ( ( Height() - pToolTip->Height() ) * 0.5f );
		pToolTip->ShowTooltip( m_pNextUpgradeItem, m_pNextUpgradeItem->GetType(), ST_INVENTORY, fX, fY, false, false, m_pUpgradeItem );
		m_bShowNextUpgradeTooltip = true;
	}
	else {
		if( m_bShowNextUpgradeTooltip ) {
			pToolTip->SetTargetDiffItem( NULL );
			pToolTip->HideTooltip();
			pToolTip->SetMode( CDnTooltipDlg::TOOLTIP_NORMAL );
			m_bShowNextUpgradeTooltip = false;
		}
	}
}

#ifdef PRE_ADD_DIRECT_BUY_UPGRADEITEM
void CDnItemUpgradeDlg::ResetUpgradeItemBtn()
{
	for(int i = 0 ; i < ITEMSLOT_MAX ; ++i)
	{
		m_pUpgradeItemButton[i]->Show(false);
		m_pUpgradeItemButton[i]->Enable(false);
		m_pUpgradeItemButton[i]->ClearTooltipText();
	}
}

void CDnItemUpgradeDlg::ShowDirectBuyMarketDlg(int nSlotIndex)
{
	if( m_pItem[nSlotIndex] )
	{
		char cPayMethodCode = 1; // 0-혼합, 1-코인, 3-페탈
		int  nItemID		= m_pItem[nSlotIndex]->GetClassID();
#ifdef PRE_ADD_PETALTRADE
		cPayMethodCode = 0;
#else
		cPayMethodCode = 1;
#endif // PRE_ADD_PETALTRADE
		SendMarketMiniList(nItemID, cPayMethodCode);
	}
}

void CDnItemUpgradeDlg::RefreshUpgradeItemButton()
{
	CheckUpgradeItem();
}
#endif


#ifdef PRE_ADD_JELLYCOUNT

// 아이템의 upgrade 상태에 따른 텍스트처리.
void CDnItemUpgradeDlg::CheckUpgradState()
{
	if( !m_pUpgradeItem ) 
		return;

#ifdef PRE_FIX_MEMOPT_ENCHANT

	// 보호 아이템 개수
	int nEnchantTableItemID = GetEnchantTableItemID( m_pUpgradeItem );
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TENCHANT_MAIN );
	DNTableFileFormat* pNeedItemSox = GetDNTable( CDnTableDB::TENCHANT_NEEDITEM );
	int needItemTableID = pSox->GetFieldFromLablePtr(nEnchantTableItemID, "_NeedItemTableID")->GetInteger();
	if (pSox && pNeedItemSox && nEnchantTableItemID && needItemTableID)
		m_nTotalGuardNeeds = pNeedItemSox->GetFieldFromLablePtr( needItemTableID, "_ProtectItemCount" )->GetInteger();

#else // PRE_FIX_MEMOPT_ENCHANT

	// 보호 아이템 개수
	int nEnchantTableItemID = GetEnchantTableItemID( m_pUpgradeItem );
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TENCHANT );
	if( pSox && nEnchantTableItemID )
		m_nTotalGuardNeeds = pSox->GetFieldFromLablePtr( nEnchantTableItemID, "_ProtectItemCount" )->GetInteger();

#endif // PRE_FIX_MEMOPT_ENCHANT


	bool bCheckNeedItem = false;

	if( m_nTotalGuardNeeds == 0 )
		bCheckNeedItem = false;

	int nGuardItemTotalCount = GetItemTask().FindItemCountFromItemType( ITEMTYPE_ENCHANT_BREAKGAURD );
	if( nGuardItemTotalCount >= m_nTotalGuardNeeds )
		bCheckNeedItem = true;

	// 0개로 설정되었다는 이야기는 이번 강화단계에서 보호아이템을 사용할 수 없다는 이야기다.
	// 이땐 강제로 강화보호아이템을 사용할 수 없는 상태로 바꾼다.
	if( m_nTotalGuardNeeds == 0 )
		bCheckNeedItem = false;

#ifndef PRE_ADD_ENCHANTSHIELD_CASHITEM

	if( bCheckNeedItem )
	{
		m_pStaticTextNotUse->Show( false );
		m_pStaticTextNotUse2->Show( true );	
	}
	else
	{
		m_pStaticTextNotUse2->Show( FALSE );	
	}
#endif
}

#endif // PRE_ADD_JELLYCOUNT 