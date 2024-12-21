#include "StdAfx.h"
#include "DnCashShopPayDlg.h"
#include "DnCommonUtil.h"
#include "DnCashShopPayItemDlg.h"
#include "DnCashShopPayInfoItemDlg.h"
#include "DnCashShopTask.h"
#include "DnItemTask.h"
#include "DnInterface.h"
#include "DnSimpleTooltipDlg.h"
#include "DnCashShopMessageBox.h"
#include "DnTableDB.h"
#ifdef PRE_MOD_PETAL_WRITE
#include "DnInterfaceString.h"
#endif // PRE_MOD_PETAL_WRITE
#ifdef PRE_ADD_CADGE_CASH
#include "DnCashShopCommonDlgMgr.h"
#include "DnCashShopDlg.h"
#endif // PRE_ADD_CADGE_CASH

#define NO_PERIOD_COMBOBOX_SELECTED		-1
#define NO_ABILITY_COMBOBOX_SELECTED	-1

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
void CDnCashShopPayDlg::SPaySlotUnit::Set(const SCashShopItemInfo& info, const SCashShopCartItemInfo& cartItemInfo)
{
	pName->SetText(info.nameString.c_str());

	if (info.presentItemId != ITEMCLSID_NONE)
	{
		SAFE_DELETE(pItem);
		TItemInfo itemInfo;
		if (CDnItem::MakeItemInfo(info.presentItemId, info.count, itemInfo))
		{
			pItem = GetItemTask().CreateItem(itemInfo);
			if (pItem)
			{
				pItem->SetCashItemSN(info.sn);
				pSlotBtn->SetQuickItem(pItem);
			}
		}
	}

#ifdef PRE_ADD_PETALSHOP
#ifdef PRE_MOD_PETAL_WRITE
	int nUIStringIndex = 2030002;	// UISTRING : %s페탈
#else // PRE_MOD_PETAL_WRITE
	int nUIStringIndex = 4647;	// UISTRING : %d페탈
#endif // PRE_MOD_PETAL_WRITE
#else // PRE_ADD_PETALSHOP
#ifdef PRE_MOD_PETAL_WRITE
	int nUIStringIndex = 2030001;	// UISTRING : %s캐시
#else // PRE_MOD_PETAL_WRITE
	int nUIStringIndex = 4642;	// UISTRING : %d캐시
#endif // PRE_MOD_PETAL_WRITE
#endif // PRE_ADD_PETALSHOP

	m_bSelectPeriod = false;

#ifdef PRE_ADD_VIP
	if (info.type == eCSType_Costume || info.type == eCSType_Term)
#else
	if (info.type == eCSType_Costume)
#endif
	{
		bCostume = true;
		m_bSelectPeriod = true;
		const std::vector<ITEMCLSID>& itemIdList = info.abilityList;

		int i = 0;
		if (itemIdList.size() <= 0)
		{
			_ASSERT(0);
		}
		else
		{
			int abilityCount = GetCashShopTask().GetValidAbilityCount(itemIdList);
			if (abilityCount > 1)
			{
				pAbilityComboBox->AddItem(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4576), NULL, NO_ABILITY_COMBOBOX_SELECTED);	// UISTRING : 능력치를 선택해 주세요
			}

			for (i = 0; i < (int)itemIdList.size(); ++i)
			{
				ITEMCLSID id = itemIdList[i];

				std::wstring strAbility = GetCashShopTask().GetAbilityString( id );
				if( strAbility.length() > 0 )
					pAbilityComboBox->AddItem( strAbility.c_str(), NULL, id, true );
			}

			if (abilityCount <= 1)
			{
				pAbilityComboBox->SetSelectedByIndex(0);
				pAbilityComboBox->Enable(false);
			}
			else
			{
				pAbilityComboBox->SetSelectedByValue(cartItemInfo.ability);
				pAbilityComboBox->Enable(true);
			}
		}

		SetSkillList( info.presentItemId, cartItemInfo.nOptionIndex );
	}
	else
	{
		bCostume = false;
	}

	std::wstring str;
	const std::vector<CASHITEM_SN>& itemSnList = info.linkIdList;

	if( bCostume || itemSnList.size() >= 1 )
	{
		m_bSelectPeriod = true;

		pPeriodComboBox->Show( true );
		if( itemSnList.size() >= 1 )
			pPeriodComboBox->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4755 ), NULL, NO_PERIOD_COMBOBOX_SELECTED );	//	UISTRING : 기간을 선택해 주세요

		std::wstring date;
		std::wstring money;
		if( info.period < 0 )
			date = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4705 );
		else
			date = FormatW( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4640), info.period ); // UISTRING : %d일

#ifdef PRE_MOD_PETAL_WRITE
		money = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nUIStringIndex ), DN_INTERFACE::UTIL::GetAddCommaString( info.price ).c_str() );
#else // PRE_MOD_PETAL_WRITE
		money = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nUIStringIndex ), info.price );
#endif // PRE_MOD_PETAL_WRITE

		str = FormatW(L"%s %s", date.c_str(), money.c_str());
		pPeriodComboBox->AddItem( str.c_str(), NULL, info.sn );

		for( int i=0; i<(int)itemSnList.size(); ++i )
		{
			CASHITEM_SN curSN = itemSnList[i];
			const SCashShopItemInfo* pItemInfo = GetCashShopTask().GetItemInfo( curSN );
			if( pItemInfo )
			{
				std::wstring str;

				if( pItemInfo->period < 0 )
					date = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4705 );
				else
					date = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4640 ), pItemInfo->period ); // UISTRING : %d일

#ifdef PRE_MOD_PETAL_WRITE
				money = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nUIStringIndex ), DN_INTERFACE::UTIL::GetAddCommaString( pItemInfo->price ).c_str() );
#else // PRE_MOD_PETAL_WRITE
				money = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nUIStringIndex ), pItemInfo->price );
#endif // PRE_MOD_PETAL_WRITE

				str = FormatW( L"%s %s", date.c_str(), money.c_str() );
				pPeriodComboBox->AddItem( str.c_str(), NULL, curSN );
			}
		}
		pPeriodComboBox->SetSelectedByValue( cartItemInfo.selectedSN );
		pPeriodComboBox->Enable( cartItemInfo.isPackage == false );

		if( pPeriodComboBox->GetItemCount() == 1 )
			pPeriodComboBox->Enable( false );
	}
	else
	{
		pPeriodComboBox->Show( false );
		pPeriodComboBox->Enable( false );

		std::wstring str;
		str = FormatW(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4625), info.count );	// UISTRING : %d 개
		pCount->SetText(str.c_str());

#ifdef PRE_MOD_PETAL_WRITE
		str = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nUIStringIndex ), DN_INTERFACE::UTIL::GetAddCommaString( info.price ).c_str() );
#else // PRE_MOD_PETAL_WRITE
		str = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nUIStringIndex ), info.price );
#endif // PRE_MOD_PETAL_WRITE

		pPrice->SetText( str.c_str() );

		if( info.period > 0 )
		{
			std::wstring str;
			str = FormatW(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4613), info.period );	// UISTRING : (%d 일)
			pPeriod->SetTextColor( textcolor::YELLOW );
			pPeriod->SetText( str.c_str() );
		}
		else
		{
			pPeriod->Show( false );
		}
	}

	pClose->Show(cartItemInfo.isPackage == false);
}

void CDnCashShopPayDlg::SPaySlotUnit::SetSkillList( ITEMCLSID itemID, int nOptionIndex )
{
	pSkillComboBox->Enable( false );

	if( itemID == -1 )
		return;

	CDnItem* pItem = NULL;
	TItemInfo itemInfo;
	if( CDnItem::MakeItemInfo( itemID, 1, itemInfo ) )
	{
		pItem = GetItemTask().CreateItem( itemInfo );
	}

	if( pItem == NULL )
		return;

	int nPotentialIndex = pItem->GetTypeParam( 0 );
	SAFE_DELETE( pItem );

	if( nPotentialIndex == 0 )
		return;

	DNTableFileFormat*  pPotenSox = GetDNTable( CDnTableDB::TPOTENTIAL );
	if( pPotenSox == NULL )
	{
		_ASSERT(0);
		return;
	}
	DNTableFileFormat*  pSkillTable = GetTableDB().GetTable( CDnTableDB::TSKILL );
	if( pSkillTable == NULL )
	{
		_ASSERT(0);
		return;
	}

	std::vector<int> nVecItemID;
	pPotenSox->GetItemIDListFromField( "_PotentialID", nPotentialIndex, nVecItemID );

	if( nVecItemID.size() > 0 )
	{
		pSkillComboBox->AddItem( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4858), NULL, NO_ABILITY_COMBOBOX_SELECTED );	// UISTRING : 스킬을 선택해 주세요
	}

	std::vector<int>::const_iterator iter = nVecItemID.begin();
	for( ; iter!=nVecItemID.end(); iter++ )
	{
		int itemId = *iter;
		int nSkillID = pPotenSox->GetFieldFromLablePtr( itemId, "_SkillID" )->GetInteger();
		int nSkillLevel = pPotenSox->GetFieldFromLablePtr( itemId, "_SkillLevel" )->GetInteger();

		int iStringID = 0;
		iStringID = pSkillTable->GetFieldFromLablePtr( nSkillID, "_NameID" )->GetInteger();
		std::wstring skillName = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, iStringID );

		std::wstring strSkill;
		WCHAR wszLevelInfo[64] = {0,};
		swprintf_s( wszLevelInfo, _countof( wszLevelInfo ), L"%s +%d", skillName.c_str(), nSkillLevel );
		strSkill = wszLevelInfo;

		pSkillComboBox->AddItem( strSkill.c_str(), NULL, nSkillID, true );
	}

	if( pSkillComboBox->GetItemCount() > 1 )
	{
		if( nOptionIndex > 0 )
			pSkillComboBox->SetSelectedByIndex( nOptionIndex );
		pSkillComboBox->Enable( true );
	}
}

CDnCashShopPayDlg::CDnCashShopPayDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback)
			: CDnCustomDlg(dialogType, pParentDialog, nID, pCallback, true)
{
	m_pGoodsInfoListBox	= NULL;

	m_pTitle			= NULL;
	m_pSum				= NULL;
	m_pDiscount			= NULL;
	m_pReserve			= NULL;
	m_pInfo				= NULL;
	m_pBuyReserve		= NULL;
#ifdef PRE_ADD_CASHSHOP_CREDIT
	m_pBuyCash			= NULL;
	m_pBuyCredit		= NULL;
	m_pBuyCreditTitle	= NULL;
	m_pBuyMsg			= NULL;
#endif // PRE_ADD_CASHSHOP_CREDIT
	m_pBuyReserveTitle	= NULL;
#ifdef PRE_ADD_NEW_MONEY_SEED
	m_pBuySeed = NULL;
	m_pBuySeedTitle = NULL;
	m_nSeed = 0;
#endif // PRE_ADD_NEW_MONEY_SEED
	m_pBuyBtn			= NULL;
#ifdef PRE_ADD_CADGE_CASH
	m_pCadgeButton		= NULL;
	m_pCadgeRecieverName	= NULL;
	m_pCommonDlgMgr		= NULL;
#endif // PRE_ADD_CADGE_CASH
	m_pCancelBtn		= NULL;
	m_pChargeBtn		= NULL;
	m_pCloseBtn			= NULL;

	m_Sum				= 0;
	m_Mode				= eCashUnit_None;

	m_pPrevBtn			= NULL;
	m_pNextBtn			= NULL;
	m_pPageNum			= NULL;

	m_CurrentPage		= 1;
	m_MaxPage			= 0;

	m_pCashShopMsgBox	= NULL;
#ifdef PRE_ADD_CASHSHOP_REFUND_CL
	m_pDirectInvenCheckBox	= NULL;
	m_pDirectInvenCheckBoxTitle = NULL;
	m_pDirectInvenStatic = NULL;
#endif

#if defined( PRE_ADD_CASHSHOP_CREDIT ) || defined( PRE_ADD_NEW_MONEY_SEED )
	m_eBuyMode = CASHSHOP_BUY_NONE;
#endif // PRE_ADD_CASHSHOP_CREDIT or PRE_ADD_NEW_MONEY_SEED
#ifdef PRE_ADD_SALE_COUPON
	m_ApplyCouponSN = 0;
#endif // PRE_ADD_SALE_COUPON
	m_nLastPresentSN = 0;

#ifdef PRE_ADD_CASHSHOP_ACTOZ
	m_pStaticBuyMsg = NULL;
#endif // PRE_ADD_CASHSHOP_ACTOZ

}

CDnCashShopPayDlg::~CDnCashShopPayDlg(void)
{
	m_pGoodsInfoListBox->RemoveAllItems();
	SAFE_DELETE(m_pCashShopMsgBox);
}

#ifdef PRE_ADD_CADGE_CASH
void CDnCashShopPayDlg::Initialize(bool bShow, CDnCashShopCommonDlgMgr* pCommonDlgMgr)
{
	m_pCommonDlgMgr = pCommonDlgMgr;
#else // PRE_ADD_CADGE_CASH
void CDnCashShopPayDlg::Initialize(bool bShow)
{
#endif // PRE_ADD_CADGE_CASH
#if defined(PRE_ADD_CASHSHOP_REFUND_CL)
	std::string dlgName = CDnCashShopTask::GetInstance().IsCashShopMode(eCSMODE_REFUND) ? "CSPayment_Refund.ui" : "CSPayment.ui";
	CDnCustomDlg::Initialize(CEtResourceMng::GetInstance().GetFullName(dlgName.c_str()).c_str(), bShow);
#elif defined(PRE_ADD_NEW_MONEY_SEED)
	CDnCustomDlg::Initialize(CEtResourceMng::GetInstance().GetFullName("CS_CSPayment_Seed.ui").c_str(), bShow);
#else
	CDnCustomDlg::Initialize(CEtResourceMng::GetInstance().GetFullName("CSPayment.ui").c_str(), bShow);
#endif

	m_pCashShopMsgBox = new CDnCashShopMessageBox(UI_TYPE_MODAL);
	m_pCashShopMsgBox->Initialize(false);
}

void CDnCashShopPayDlg::InitialUpdate()
{
	char szControlName[32]={0};

	for(int i=0; i<(int)m_SlotUnits.size(); i++)
	{
		sprintf_s(szControlName, 32, "ID_STATIC_BASE%d", i);
		m_SlotUnits[i].pBase = GetControl<CEtUIStatic>(szControlName);

		sprintf_s(szControlName, 32, "ID_STATIC_CBASE%d", i);
		m_SlotUnits[i].pCBase = GetControl<CEtUIStatic>(szControlName);

		sprintf_s(szControlName, 32, "ID_STATIC_PBASE%d", i);
		m_SlotUnits[i].pPBase = GetControl<CEtUIStatic>(szControlName);

		sprintf_s(szControlName, 32, "ID_STATIC_NAME%d", i);
		m_SlotUnits[i].pName = GetControl<CEtUIStatic>(szControlName);

		sprintf_s(szControlName, 32, "ID_STATIC_COUNT%d", i);
		m_SlotUnits[i].pCount = GetControl<CEtUIStatic>(szControlName);

		sprintf_s(szControlName, 32, "ID_STATIC_PRICE%d", i);
		m_SlotUnits[i].pPrice = GetControl<CEtUIStatic>(szControlName);

		sprintf_s(szControlName, 32, "ID_STATIC_PERIOD%d", i);
		m_SlotUnits[i].pPeriod = GetControl<CEtUIStatic>(szControlName);

		sprintf_s(szControlName, 32, "ID_STATIC_SLOTBASE%d", i);
		m_SlotUnits[i].pQuickSlotBG = GetControl<CEtUIStatic>(szControlName);

		sprintf_s(szControlName, 32, "ID_COMBOBOX_PRICE%d", i);
		m_SlotUnits[i].pPeriodComboBox = GetControl<CEtUIComboBox>(szControlName);

		sprintf_s(szControlName, 32, "ID_COMBOBOX_ABILITY%d", i);
		m_SlotUnits[i].pAbilityComboBox = GetControl<CEtUIComboBox>(szControlName);

		sprintf_s(szControlName, 32, "ID_COMBOBOX_SKILL%d", i);
		m_SlotUnits[i].pSkillComboBox = GetControl<CEtUIComboBox>(szControlName);

		sprintf_s(szControlName, 32, "ID_BUTTON_CLOSE%d", i);
		m_SlotUnits[i].pClose = GetControl<CEtUIButton>(szControlName);

		m_SlotUnits[i].Show(false);
	}

	m_pTitle			= GetControl<CEtUIStatic>("ID_STATIC0");
	m_pGoodsInfoListBox	= GetControl<CEtUIListBoxEx>("ID_LISTBOXEX1");

	m_pSum				= GetControl<CEtUITextBox>("ID_STATIC_SUM");
	m_pDiscount			= GetControl<CEtUIStatic>("ID_STATIC_DISCOUNT");
#ifdef PRE_ADD_NEW_MONEY_SEED
	m_pReserve			= GetControl<CEtUITextBox>( "ID_TEXTBOX_RESERVE" );
#else // PRE_ADD_NEW_MONEY_SEED
	m_pReserve			= GetControl<CEtUIStatic>("ID_STATIC_RESERVE");
#endif // PRE_ADD_NEW_MONEY_SEED
	m_pInfo				= GetControl<CEtUIStatic>("ID_STATIC_INFO");
	m_pBuyReserve		= GetControl<CEtUICheckBox>("ID_CHECKBOX_RESERVE");
#ifdef PRE_ADD_CASHSHOP_CREDIT
	m_pBuyCash			= GetControl<CEtUICheckBox>("ID_CHECKBOX_CASH");
	m_pBuyCredit		= GetControl<CEtUICheckBox>("ID_CHECKBOX_CREDIT");
	m_pBuyCreditTitle	= GetControl<CEtUIStatic>("ID_STATIC_CREDIT");
	m_pBuyCredit->Enable( false );
	m_pBuyMsg			= GetControl<CEtUIStatic>("ID_STATIC_BUYMSG");
#endif // PRE_ADD_CASHSHOP_CREDIT
	m_pBuyReserveTitle	= GetControl<CEtUIStatic>("ID_STATIC2");
#ifdef PRE_ADD_NEW_MONEY_SEED
	m_pBuySeed = GetControl<CEtUICheckBox>( "ID_CHECKBOX_SEED" );
	m_pBuySeedTitle = GetControl<CEtUIStatic>( "ID_STATIC3" );
#endif // PRE_ADD_NEW_MONEY_SEED
#ifdef PRE_ADD_CASHSHOP_REFUND_CL
	if (CDnCashShopTask::GetInstance().IsCashShopMode(eCSMODE_REFUND))
	{
		m_pDirectInvenCheckBox	= GetControl<CEtUICheckBox>("ID_CHECKBOX_SENDCI");
		m_pDirectInvenCheckBoxTitle = GetControl<CEtUIStatic>("ID_STATIC10");
		m_pDirectInvenStatic = GetControl<CEtUIStatic>("UI_STATIC_BUYMSG");
	}
#endif

#ifdef PRE_ADD_CADGE_CASH
	m_pCadgeButton = GetControl<CEtUIButton>("ID_BUTTON_ASK");
	m_pCadgeRecieverName = GetControl<CEtUIStatic>("ID_TEXT_ASKNAME");
#else // PRE_ADD_CADGE_CASH
	CEtUIButton* pCadgeButton = GetControl<CEtUIButton>("ID_BUTTON_ASK");
	if( pCadgeButton )
		pCadgeButton->Show( false );
	CEtUIStatic* pCadgeUserName = GetControl<CEtUIStatic>("ID_TEXT_ASKNAME");
	if( pCadgeUserName )
		pCadgeUserName->Show( false );
#endif // ID_BUTTON_ASK
	m_pBuyBtn			= GetControl<CEtUIButton>("ID_BUTTON_BUY");
	m_pCancelBtn		= GetControl<CEtUIButton>("ID_BUTTON_CANCEL");
	m_pChargeBtn		= GetControl<CEtUIButton>("ID_BUTTON_CHARGE");

	m_pPrevBtn			= GetControl<CEtUIButton>("ID_BUTTON_PGUP");
	m_pNextBtn			= GetControl<CEtUIButton>("ID_BUTTON_PGDN");
	m_pPageNum			= GetControl<CEtUIStatic>("ID_STATIC_PAGE");

	m_pCloseBtn			= GetControl<CEtUIButton>("ID_BUTTON_CLOSE");

#ifdef PRE_ADD_CASHSHOP_ACTOZ	
	m_pStaticBuyMsg = GetControl<CEtUIStatic>("UI_STATIC_BUYINFOMSG");
	if( m_pStaticBuyMsg )
		m_pStaticBuyMsg->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4818 ) ); // "캐시 소지품 창으로 이동 시 청약철회 불가"
#endif // PRE_ADD_CASHSHOP_ACTOZ

}

void CDnCashShopPayDlg::InitCustomControl(CEtUIControl* pControl)
{
	if (pControl == NULL || !strstr(pControl->GetControlName(), "ID_BUTTON_ITEM"))
		return;

	CDnQuickSlotButton *pQuickSlotBtn(NULL);
	pQuickSlotBtn = static_cast<CDnQuickSlotButton*>(pControl);

	pQuickSlotBtn->SetSlotType(ST_INVENTORY_CASHSHOP);
	pQuickSlotBtn->SetSlotIndex((int)m_SlotUnits.size());

	SPaySlotUnit slotUnit;
	slotUnit.pSlotBtn = pQuickSlotBtn;
	m_SlotUnits.push_back(slotUnit);
}

void CDnCashShopPayDlg::Clear(bool bAll)
{
	if (bAll)
	{
		std::vector<SPaySlotUnit>::iterator iter = m_SlotUnits.begin();
		for (; iter != m_SlotUnits.end(); ++iter)
		{
			SPaySlotUnit& slotUnit = *iter;
			slotUnit.Clear();
		}
		m_pGoodsInfoListBox->RemoveAllItems();
		m_pItemCacheList.clear();
	}

	m_Sum = 0;
	m_Reserve = 0;

	m_CurrentPage = 1;
	m_MaxPage = 0;

#ifdef PRE_ADD_PETALSHOP
	m_pBuyReserve->SetChecked(true);
#ifdef PRE_ADD_CASHSHOP_CREDIT
	m_pBuyCash->SetChecked( false );
	m_pBuyCredit->SetChecked( false );
#endif // PRE_ADD_CASHSHOP_CREDIT
#else
	m_pBuyReserve->SetChecked(false);
#ifdef PRE_ADD_NEW_MONEY_SEED
	m_nSeed = 0;
	m_pBuySeed->SetChecked( false );
#endif // PRE_ADD_NEW_MONEY_SEED
#ifdef PRE_ADD_CASHSHOP_CREDIT
	m_pBuyCash->SetChecked( false );
	m_pBuyCredit->SetChecked( false );
#endif // PRE_ADD_CASHSHOP_CREDIT
#endif

#ifdef PRE_ADD_CASHSHOP_REFUND_CL
	m_pDirectInvenCheckBox->SetChecked(false);
#endif
}

void CDnCashShopPayDlg::SortItemList(CART_ITEM_LIST& resultList, const CART_ITEM_LIST& srcList, std::vector<int>& vecWillPayItemIndex)
{
	resultList.clear();

	// 패키지안에 SN이 겹치는게 있어서 인덱스까지 추가해서 검사한다.
	int nIndex = 0;
	std::list<int> listWillPayItemIndex;

	CART_ITEM_LIST::const_iterator iter = srcList.begin();
	for (; iter != srcList.end(); ++iter, ++nIndex)
	{
		const SCashShopCartItemInfo& info = (*iter);

		const SCashShopItemInfo* pItemInfo = CDnCashShopTask::GetInstance().GetItemInfo(info.presentSN);
		if (pItemInfo)
		{
			int count = CDnCashShopTask::GetInstance().GetValidAbilityCount(pItemInfo->abilityList);
			if (count > 1 || pItemInfo->linkIdList.size() > 0)
			{
				resultList.push_front(info);
				listWillPayItemIndex.push_front(nIndex);
			}
			else
			{
				resultList.push_back(info);
				listWillPayItemIndex.push_back(nIndex);
			}
		}
		else
		{
			_ASSERT(0);
			resultList.push_back(info);
			listWillPayItemIndex.push_back(nIndex);
		}
	}

	vecWillPayItemIndex.clear();
	std::list<int>::iterator iterList = listWillPayItemIndex.begin();
	for( ; iterList != listWillPayItemIndex.end(); ++iterList )
		vecWillPayItemIndex.push_back( (*iterList) );
}

void CDnCashShopPayDlg::Update(eCashUnitType type)
{
	m_Mode = type;
	if (IsGiftMode())
	{
		m_pTitle->SetText(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4760 ));	// UISTRING : 선물 결제
		m_pBuyBtn->SetText(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4610 )); // UISTRING : 선물
#ifdef PRE_ADD_CADGE_CASH
		m_pCadgeButton->Show( false );
		std::wstring strRecieverInfo;
		const SGiftSendBasicInfo& info = GetCashShopTask().GetGiftBasicInfo();
		strRecieverInfo = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4917 ), info.receiverName.c_str() );
		m_pCadgeRecieverName->SetText( strRecieverInfo.c_str() );
#endif // PRE_ADD_CADGE_CASH

#ifdef PRE_ADD_CASHSHOP_ACTOZ	
		if( m_pStaticBuyMsg )
			m_pStaticBuyMsg->Show( false );
#endif // PRE_ADD_CASHSHOP_ACTOZ

	}
	else
	{
		m_pTitle->SetText(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4615 ));	// UISTRING : 결제
		m_pBuyBtn->SetText(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1805 )); // UISTRING : 구입
#ifdef PRE_ADD_CADGE_CASH
		m_pCadgeButton->Show( true );
		m_pCadgeRecieverName->ClearText();
#endif // PRE_ADD_CADGE_CASH

#ifdef PRE_ADD_CASHSHOP_ACTOZ	
		if( m_pStaticBuyMsg )
			m_pStaticBuyMsg->Show( true );
#endif // PRE_ADD_CASHSHOP_ACTOZ

	}

	if (type == eCashUnit_Cart || type == eCashUnit_Gift_Cart)
	{
		const CART_ITEM_LIST& cartList = GetCashShopTask().GetCartList();

		UpdateItemList(cartList);
		UpdateInfoList(cartList);
	}
	else if (type == eCashUnit_Package)
	{
		const CART_ITEM_LIST& cartList = GetCashShopTask().GetBuyPackageItemList();

		UpdateItemList(cartList);
		UpdateInfoList(cartList);
	}
	else if (type == eCashUnit_BuyItemNow || type == eCashUnit_GiftItemNow)
	{
		const SCashShopCartItemInfo& info = GetCashShopTask().GetBuyItemNow();
		CART_ITEM_LIST list;
		list.push_back(info);

		UpdateItemList(list);

		//	update item list 에서 buyitemnow 항목을 업데이트함.
		list.clear();
		const SCashShopCartItemInfo& updatedInfo = GetCashShopTask().GetBuyItemNow();
		list.push_back(updatedInfo);
		UpdateInfoList(list);
	}
	else if (type == eCashUnit_PreviewCart || type == eCashUnit_Gift_PreviewCart)
	{
		const CART_ITEM_LIST* pInfo = GetCashShopTask().GetBuyPreviewCartList();
		if (pInfo)
		{
			UpdateItemList(*pInfo);
			UpdateInfoList(*pInfo);
		}
	}
	else if (type == eCashUnit_Gift_Package)
	{
		const CART_ITEM_LIST& cartList = GetCashShopTask().GetGiftPackageItemList();

		UpdateItemList(cartList);
		UpdateInfoList(cartList);
	}

#ifdef PRE_ADD_CASHSHOP_REFUND_CL
	SetRefundStatic();
#endif
}

void CDnCashShopPayDlg::UpdatePageStatic()
{
	wchar_t wszPage[10]={0};
	swprintf_s( wszPage, 10, L"%d/%d", m_CurrentPage, m_MaxPage);
	m_pPageNum->SetText(wszPage);
}

void CDnCashShopPayDlg::UpdatePageControllers(const CART_ITEM_LIST& itemList)
{
	int listSize = (int)itemList.size();
	m_MaxPage = listSize / _MAX_PAY_SLOT_NUM_PER_PAGE;
	if ((listSize % _MAX_PAY_SLOT_NUM_PER_PAGE) != 0)
		m_MaxPage++;

	if (m_MaxPage < m_CurrentPage)
		m_CurrentPage = m_MaxPage;

	UpdatePageStatic();

	m_pPrevBtn->Enable(m_CurrentPage > 1);
	m_pNextBtn->Enable(m_CurrentPage < m_MaxPage);
}

void CDnCashShopPayDlg::ClearSlots(bool bShowOff)
{
	std::vector<SPaySlotUnit>::iterator iter = m_SlotUnits.begin();
	for (; iter != m_SlotUnits.end(); ++iter)
	{
		SPaySlotUnit& slot = *iter;
		slot.Clear();
		if (bShowOff)
			slot.Show(false);
	}
}

void CDnCashShopPayDlg::UpdateItemList(const CART_ITEM_LIST& willPayItemList)
{
	m_DataSlotIdxMatchList.clear();

	ClearSlots(true);
	CART_ITEM_LIST listSorted;
	SortItemList(listSorted, willPayItemList, m_vecWillPayItemIndex);
	int listSize = (int)listSorted.size();

	UpdatePageControllers(listSorted);

	int i = 0;
	for (; i < int(m_SlotUnits.size()); ++i)
	{
		SPaySlotUnit& slot = m_SlotUnits[i];
		//slot.Clear();
		int dataIdx = ((m_CurrentPage - 1) * _MAX_PAY_SLOT_NUM_PER_PAGE) + i;
		if (dataIdx < 0 || dataIdx >= listSize)
			break;

		const SCashShopCartItemInfo* pCartInfo = GetCashShopTask().GetCartListItemInfo(listSorted, dataIdx);
		if (pCartInfo != NULL)
		{
			const SCashShopItemInfo* pInfo = GetCashShopTask().GetItemInfo(pCartInfo->presentSN);
			if (pInfo == NULL)
			{
				_ASSERT(0);
				return;
			}

			if (m_Mode == eCashUnit_PreviewCart)
			{
				m_DataSlotIdxMatchList.insert(std::make_pair(i, pCartInfo->id));
			}
			else
			{
				if (pCartInfo->isPackage)
					m_DataSlotIdxMatchList.insert(std::make_pair(i, pCartInfo->selectedSN));
				else
					m_DataSlotIdxMatchList.insert(std::make_pair(i, pCartInfo->id));
			}
#ifdef PRE_ADD_CADGE_CASH
			slot.Enable( true );
			slot.Set(*pInfo, *pCartInfo);
			if( GetInterface().GetCashShopDlg()->GetCadgeMailID() > 0 )
				slot.Enable( false );
#else // PRE_ADD_CADGE_CASH
			slot.Set(*pInfo, *pCartInfo);
#endif // PRE_ADD_CADGE_CASH
			SComboBoxItem* pItem = slot.pAbilityComboBox->GetSelectedItem();
			if (pItem != NULL)
				SetCartItemAbility(pCartInfo->id, pItem->nValue, m_vecWillPayItemIndex[dataIdx]);

			int nOptionIndex = slot.pSkillComboBox->GetSelectedIndex();
			if( nOptionIndex > 0 )
				SetCartItemSkill( pCartInfo->id, nOptionIndex );

			slot.Show(true);
			if (m_Mode == eCashUnit_BuyItemNow || m_Mode == eCashUnit_Package || 
				m_Mode == eCashUnit_GiftItemNow || m_Mode == eCashUnit_Gift_Package)
				slot.ShowCloseBtn(false);
		}
	}
}

void CDnCashShopPayDlg::UpdateInfoList(const CART_ITEM_LIST& willPayItemList)
{
	m_pGoodsInfoListBox->RemoveAllItems();

	m_Sum = 0;
	m_Reserve = 0;
	bool bBuyReserveOn = true;
#ifdef PRE_ADD_NEW_MONEY_SEED
	m_nSeed = 0;
	bool bBuySeedOn = true;
	bool bBuyOnlySeed = false;
	bool bBuyOnlyReserve = false;
#endif // PRE_ADD_NEW_MONEY_SEED

	if (m_Mode == eCashUnit_Package || m_Mode == eCashUnit_Gift_Package)
	{
		const SCashShopCartItemInfo& info = GetCashShopTask().GetBuyItemNow();
		if (info.isPackage == false)
		{
			_ASSERT(0);
			return;
		}

		if (info.selectedSN == -1)
		{
			_ASSERT(0);
			return;
		}

		const SCashShopItemInfo* pItemInfo = GetCashShopTask().GetItemInfo(info.selectedSN);
		if (pItemInfo)
		{
			if (pItemInfo->bReserveUsable == false)
				bBuyReserveOn = false;

#ifdef PRE_ADD_NEW_MONEY_SEED
			if( pItemInfo->bSeedUsable == false )
				bBuySeedOn = false;
			if( pItemInfo->bSeedUsable && !pItemInfo->bCashUsable && !pItemInfo->bReserveUsable )
				bBuyOnlySeed = true;
			if( !pItemInfo->bSeedUsable && !pItemInfo->bCashUsable && pItemInfo->bReserveUsable )
				bBuyOnlyReserve = true;

			if( pItemInfo->bSeedReserveAmount )
				m_nSeed = pItemInfo->SeedReserveAmount;
			if( pItemInfo->bReserveOffer )
				m_Reserve = pItemInfo->ReserveAmount;
			m_Sum = pItemInfo->price;
#else // PRE_ADD_NEW_MONEY_SEED
			m_Sum = pItemInfo->price;
			m_Reserve = pItemInfo->ReserveAmount;
#endif // PRE_ADD_NEW_MONEY_SEED
		}
		else
		{
			_ASSERT(0);
			return;
		}
	}

	bool bNotAllPeriodSelected = false;

	std::vector<int> vecWillPayItemIndex;	// m_vecWillPayItemIndex는 ItemList구할때만 하고 InfoList에선 사용하지 않는다.
	CART_ITEM_LIST listSorted;
	SortItemList(listSorted, willPayItemList, vecWillPayItemIndex);

	CART_ITEM_LIST::const_iterator iter = listSorted.begin();
	for (; iter != listSorted.end(); ++iter)
	{
		const SCashShopCartItemInfo& cartInfo = (*iter);
		m_nLastPresentSN = cartInfo.presentSN;
		const SCashShopItemInfo* pInfo = GetCashShopTask().GetItemInfo(cartInfo.presentSN);
		if (pInfo)
		{
			CDnCashShopPayInfoItemDlg* pItemInfoDlg = m_pGoodsInfoListBox->AddItem<CDnCashShopPayInfoItemDlg>();
			if (pItemInfoDlg)
				pItemInfoDlg->SetInfo(*pInfo, cartInfo);

			if (pInfo->bReserveUsable == false)
				bBuyReserveOn = false;
#ifdef PRE_ADD_NEW_MONEY_SEED
			if( pInfo->bSeedUsable == false )
				bBuySeedOn = false;

			if( pInfo->bSeedUsable && !pInfo->bCashUsable && !pInfo->bReserveUsable )
				bBuyOnlySeed = true;
			if( !pInfo->bSeedUsable && !pInfo->bCashUsable && pInfo->bReserveUsable )
				bBuyOnlyReserve = true;
#endif // PRE_ADD_NEW_MONEY_SEED

			if (m_Mode != eCashUnit_Package && m_Mode != eCashUnit_Gift_Package)
			{
				if (GetCashShopTask().IsValidSN(cartInfo.selectedSN))
				{
					const SCashShopItemInfo* pInfo = GetCashShopTask().GetItemInfo(cartInfo.selectedSN);
					m_Sum += pInfo->price;
#ifdef PRE_ADD_NEW_MONEY_SEED
					if( pInfo->bSeedReserveAmount )
						m_nSeed += pInfo->SeedReserveAmount;
					if( pInfo->bReserveOffer )
						m_Reserve += pInfo->ReserveAmount;
#else // PRE_ADD_NEW_MONEY_SEED
					m_Reserve += pInfo->ReserveAmount;
#endif // PRE_ADD_NEW_MONEY_SEED
				}
			}
		}
	}

#ifdef PRE_ADD_PETALSHOP
	m_pBuyReserve->Enable(false);
	m_pBuyReserveTitle->Enable(false);

#ifdef PRE_ADD_CASHSHOP_REFUND_CL
	m_pDirectInvenCheckBox->Enable(false);
	m_pDirectInvenCheckBoxTitle->Enable(false);
#endif

#else

#ifdef PRE_ADD_NEW_MONEY_SEED
	if( IsGiftMode() )
	{
		bBuyReserveOn = false;
		bBuySeedOn = false;
	}

	m_pBuySeed->Enable( bBuySeedOn );
	m_pBuySeedTitle->Enable( bBuySeedOn );
	m_pBuyReserve->Enable( bBuyReserveOn );
	m_pBuyReserveTitle->Enable( bBuyReserveOn );
#else // PRE_ADD_NEW_MONEY_SEED
	if (IsGiftMode())
		bBuyReserveOn = false;

	m_pBuyReserve->Enable(bBuyReserveOn);
	m_pBuyReserveTitle->Enable(bBuyReserveOn);
#endif // PRE_ADD_NEW_MONEY_SEED

#ifdef PRE_ADD_CASHSHOP_REFUND_CL
	m_pDirectInvenCheckBox->Enable(bBuyReserveOn);
	m_pDirectInvenCheckBoxTitle->Enable(bBuyReserveOn);
#endif

#endif // PRE_ADD_PETALSHOP

	UpdateEtc();
}

bool CDnCashShopPayDlg::HasEnoughMoneyToBuy(int moneyNeeded, bool bBuyReserve) const
{
#ifdef PRE_ADD_CASHSHOP_CREDIT
	if( IsBuyReserveMode() )
		return (GetCashShopTask().GetUserReserve() >= moneyNeeded && (GetCashShopTask().GetUserReserve() - moneyNeeded) >= 0);
	else if( IsBuyCreditMode() )
		return (GetCashShopTask().GetUserCredit() >= moneyNeeded && (GetCashShopTask().GetUserCredit() - moneyNeeded) >= 0);
	else if( IsBuyCashMode() )
		return (GetCashShopTask().GetUserPrepaid() >= moneyNeeded && (GetCashShopTask().GetUserPrepaid() - moneyNeeded) >= 0);

	return false;
#else // PRE_ADD_CASHSHOP_CREDIT
	if (bBuyReserve)
		return (GetCashShopTask().GetUserReserve() >= moneyNeeded && (GetCashShopTask().GetUserReserve() - moneyNeeded) >= 0);
#ifdef PRE_ADD_NEW_MONEY_SEED
	else if( IsBuySeedMode() )
		return (GetCashShopTask().GetUserSeed() >= moneyNeeded && (GetCashShopTask().GetUserSeed() - moneyNeeded) >= 0);
#endif // PRE_ADD_NEW_MONEY_SEED
	else
		return (GetCashShopTask().GetUserCash() >= moneyNeeded && (GetCashShopTask().GetUserCash() - moneyNeeded) >= 0);
#endif // PRE_ADD_CASHSHOP_CREDIT
}

bool CDnCashShopPayDlg::IsBuyReserveMode() const
{
	return m_pBuyReserve->IsChecked();//(m_pBuyReserve->IsEnable() && m_pBuyReserve->IsChecked());
}

#ifdef PRE_ADD_NEW_MONEY_SEED
bool CDnCashShopPayDlg::IsBuySeedMode() const
{
	return m_pBuySeed->IsChecked();
}
#endif // PRE_ADD_NEW_MONEY_SEED

#ifdef PRE_ADD_CASHSHOP_CREDIT
bool CDnCashShopPayDlg::IsBuyCashMode() const
{
	return m_pBuyCash->IsChecked();
}

bool CDnCashShopPayDlg::IsBuyCreditMode() const
{
	return m_pBuyCredit->IsChecked();
}
#endif // PRE_ADD_CASHSHOP_CREDIT

void CDnCashShopPayDlg::UpdateEtc()
{
	std::wstring str, reserveStr;
	m_pSum->ClearText();
	m_pDiscount->ClearText();

	int nSum = m_Sum;
	const SCashShopCartItemInfo& info = GetCashShopTask().GetBuyItemNow();
#ifdef PRE_ADD_SALE_COUPON
	m_ApplyCouponSN = 0;
#endif // PRE_ADD_SALE_COUPON
	CASHITEM_SN nLastPresentSN = 0;
	bool bCartMode = false;

	if( m_Mode == eCashUnit_Cart || m_Mode == eCashUnit_Gift_Cart || m_Mode == eCashUnit_PreviewCart || m_Mode == eCashUnit_Gift_PreviewCart )
	{
		bCartMode = true;
		nLastPresentSN = m_nLastPresentSN;
	}
	else
		nLastPresentSN = info.presentSN;

	const SCashShopItemInfo* pItemInfo = GetCashShopTask().GetItemInfo( nLastPresentSN );

	if( pItemInfo != NULL )
	{
#ifdef PRE_ADD_CADGE_CASH
		if( !IsGiftMode() )
		{
#ifdef PRE_ADD_NEW_MONEY_SEED
			if( !pItemInfo->bCashUsable || !pItemInfo->bGiftUsable )
#else // PRE_ADD_NEW_MONEY_SEED
			if( !pItemInfo->bGiftUsable )
#endif // PRE_ADD_NEW_MONEY_SEED
				m_pCadgeButton->Show( false );
		}
#endif // PRE_ADD_CADGE_CASH

		if( nSum == 0 )
			nSum = pItemInfo->price;

		std::wstring fixedPrice;
		
#ifdef PRE_ADD_SALE_COUPON
		if( pItemInfo->category == 7 )
			m_ApplyCouponSN = GetCashShopTask().GetItemApplyCouponSN( nLastPresentSN );
#endif // PRE_ADD_SALE_COUPON
		if( pItemInfo->priceFix > 0 && !bCartMode )
		{
#ifdef PRE_FIX_CASHSHOP_FIXEDPRICE
			int priceFix = pItemInfo->priceFix;
			const SCashShopItemInfo* pSelectItemInfo = GetCashShopTask().GetItemInfo(info.selectedSN);
			if (pSelectItemInfo)
				priceFix = pSelectItemInfo->priceFix;
	#ifdef PRE_MOD_PETAL_WRITE
			fixedPrice = FormatW(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2030000 ), DN_INTERFACE::UTIL::GetAddCommaString(priceFix).c_str());
	#else // PRE_MOD_PETAL_WRITE
			fixedPrice = FormatW(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4733 ), priceFix);
	#endif // PRE_MOD_PETAL_WRITE
#else
	#ifdef PRE_MOD_PETAL_WRITE
			fixedPrice = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2030000 ), DN_INTERFACE::UTIL::GetAddCommaString( pItemInfo->priceFix ).c_str() );
	#else // PRE_MOD_PETAL_WRITE
			fixedPrice = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4733 ), pItemInfo->priceFix );
	#endif // PRE_MOD_PETAL_WRITE
#endif
			m_pSum->AddText(fixedPrice.c_str(), textcolor::RED);

			std::wstring discountString;
#ifdef PRE_ADD_SALE_COUPON
			if( pItemInfo->category == 7 )
			{
				CDnItem* pCouponItem = GetItemTask().GetCashInventory().FindItemFromSerialID( m_ApplyCouponSN );
				if( pCouponItem )
					discountString = FormatW( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4866), pCouponItem->GetName(), GetCashShopTask().GetCashShopDiscountRate((float)pItemInfo->price, (float)pItemInfo->priceFix, (float)pItemInfo->ReserveAmount) );
			}
			else
			{
				discountString = FormatW( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4718), GetCashShopTask().GetCashShopDiscountRate((float)pItemInfo->price, (float)pItemInfo->priceFix, (float)pItemInfo->ReserveAmount) );
			}
#else // PRE_ADD_SALE_COUPON
			discountString = FormatW( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4718), GetCashShopTask().GetCashShopDiscountRate((float)pItemInfo->price, (float)pItemInfo->priceFix, (float)pItemInfo->ReserveAmount) );
#endif // PRE_ADD_SALE_COUPON
			m_pDiscount->SetText( discountString.c_str() );	// UISTRING : %d%% 할인 효과
		}
	}

#ifdef PRE_ADD_PETALSHOP
	int stringNumber = 4642;	// UISTRING : %d 캐시
	if( IsBuyReserveMode() )
		stringNumber = 4647;	// UISTRING : %d 페탈
#ifdef PRE_ADD_NEW_MONEY_SEED
	if( IsBuySeedMode() )
		stringNumber = 4968;	// UISTRING : %d 시드
#endif // PRE_ADD_NEW_MONEY_SEED
#ifdef PRE_MOD_PETAL_WRITE
	if( IsBuyReserveMode() )
		str = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2030002 ), DN_INTERFACE::UTIL::GetAddCommaString( nSum ).c_str() );	// UISTRING : %s 페탈
	else
		str = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2030001 ), DN_INTERFACE::UTIL::GetAddCommaString( nSum ).c_str() );	// UISTRING : %s 캐시
#else // PRE_MOD_PETAL_WRITE
		str = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, stringNumber ), nSum );
#endif // PRE_MOD_PETAL_WRITE
#else
#ifdef PRE_MOD_PETAL_WRITE
	if( IsBuyReserveMode() )
		str = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2030002 ), DN_INTERFACE::UTIL::GetAddCommaString( nSum ).c_str() );	// UISTRING : %s 페탈
#ifdef PRE_ADD_NEW_MONEY_SEED
	else if( IsBuySeedMode() )
		str = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4980 ), DN_INTERFACE::UTIL::GetAddCommaString( nSum ).c_str() );	// UISTRING : %s 시드
#endif // PRE_ADD_NEW_MONEY_SEED
	else
		str = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2030001 ), DN_INTERFACE::UTIL::GetAddCommaString( nSum ).c_str() );	// UISTRING : %s 캐시
#else // PRE_MOD_PETAL_WRITE
	if( IsBuyReserveMode() )
		str = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4647 ), nSum );	// UISTRING : %d 페탈
#ifdef PRE_ADD_NEW_MONEY_SEED
	else if( IsBuySeedMode() )
		str = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4968 ), nSum );	// UISTRING : %d 시드
	else if( pItemInfo->bCashUsable )
		str = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4642 ), nSum );	// UISTRING : %d 캐시
#else // PRE_ADD_NEW_MONEY_SEED
	else
		str = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4642 ), nSum );	// UISTRING : %d 캐시
#endif // PRE_ADD_NEW_MONEY_SEED
#endif // PRE_MOD_PETAL_WRITE
#endif // PRE_ADD_PETALSHOP
	m_pSum->AppendText(str.c_str(), textcolor::WHITE);

	eCashShopError err = GetCashShopTask().IsAllCartItemSelected(m_Mode);
	if (err == eERRCS_NO_ALL_ITEM_ABILITY_SELECTED)
	{
		str = FormatW(L" (%s)", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4576)); //	UISTRING : 능력치를 선택해주세요
		m_pSum->AppendText(str.c_str(), textcolor::RED, UITEXT_SYMBOL);
	}
	else if (err == eERRCS_NO_ALL_ITEM_PERIOD_SELECTED)
	{
		str = FormatW(L" %s", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4758));	//	UISTRING : (기간을 선택하세요)
		m_pSum->AppendText(str.c_str(), textcolor::RED, UITEXT_SYMBOL);
	}
	else if( err == eERRCS_NO_ALL_ITEM_SKILL_SELECTED )
	{
		str = FormatW(L" %s", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4860));	//	UISTRING : (스킬을 선택하세요)
		m_pSum->AppendText(str.c_str(), textcolor::RED, UITEXT_SYMBOL);
	}

#ifdef PRE_ADD_NEW_MONEY_SEED
	m_pReserve->ClearText();
	std::wstring reserveSeedStr;
	if( !pItemInfo->bCashUsable || ( m_Reserve == 0 && m_nSeed == 0 ) )	// 캐시 구매 불가 이거나 적립이 아무것도 없을 경우
	{
		str = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4709);	// UISTRING : 적립없음
		m_pReserve->AppendText( str.c_str(), textcolor::WHITE );
	}
	else
	{
		if( m_Reserve > 0 )
		{
#ifdef PRE_MOD_PETAL_WRITE
			reserveStr = FormatW( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 2030005), DN_INTERFACE::UTIL::GetAddCommaString( m_Reserve ).c_str() ); // UISTRING : %s 페탈적립
#else // PRE_MOD_PETAL_WRITE
			reserveStr = FormatW( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4658), m_Reserve ); // UISTRING : %d 페탈적립
#endif // PRE_MOD_PETAL_WRITE
		}
		if( m_nSeed > 0 )
		{
#ifdef PRE_MOD_PETAL_WRITE
			reserveSeedStr = FormatW( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4985), DN_INTERFACE::UTIL::GetAddCommaString( m_nSeed ).c_str() ); // UISTRING : %s 시드적립
#else // PRE_MOD_PETAL_WRITE
			reserveSeedStr = FormatW( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4971), m_nSeed ); // UISTRING : %d 시드적립
#endif // PRE_MOD_PETAL_WRITE
		}

		if( m_pBuyReserve->IsChecked() || m_pBuySeed->IsChecked() )
		{
			str = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4986 );	//	UISTRING : 페탈, 시드로 구매 시 적립없음
			m_pReserve->AppendText( str.c_str(), textcolor::RED );
		}
		else
		{
			if( reserveStr.length() > 0 )
				m_pReserve->AppendText( reserveStr.c_str(), textcolor::GOLD );

			if( reserveSeedStr.length() > 0 )
			{
				if( reserveStr.length() > 0 )
					m_pReserve->AppendText( L", " );

				m_pReserve->AppendText( reserveSeedStr.c_str(), textcolor::YELLOW );
			}
		}
	}
#else // PRE_ADD_NEW_MONEY_SEED
#ifdef PRE_MOD_PETAL_WRITE
	reserveStr = FormatW( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 2030005), DN_INTERFACE::UTIL::GetAddCommaString( m_Reserve ).c_str() ); // UISTRING : %d 페탈적립
#else // PRE_MOD_PETAL_WRITE
	reserveStr = FormatW( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4658), m_Reserve ); // UISTRING : %d 페탈적립
#endif // PRE_MOD_PETAL_WRITE
	if (m_Reserve > 0)
	{
		if (m_pBuyReserve->IsChecked())
			str = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4754);	//	UISTRING : 페탈로 구매 시 적립없음
		else
			str = reserveStr.c_str();
	}
	else
	{
		str = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4709);	// UISTRING : 적립없음
	}
	m_pReserve->SetText(str.c_str());
#endif // PRE_ADD_NEW_MONEY_SEED

	int userReserve = GetCashShopTask().GetUserReserve();
#ifdef PRE_ADD_CASHSHOP_CREDIT
	int userCredit	= GetCashShopTask().GetUserCredit();
	int userCash	= GetCashShopTask().GetUserPrepaid();
#else // PRE_ADD_CASHSHOP_CREDIT
	int userCash	= GetCashShopTask().GetUserCash();
#endif // PRE_ADD_CASHSHOP_CREDIT
#ifdef PRE_ADD_NEW_MONEY_SEED
	int userSeed = GetCashShopTask().GetUserSeed();
#endif // PRE_ADD_NEW_MONEY_SEED

	if (IsBuyReserveMode())// && HasEnoughMoneyToBuy(m_Sum, true))
	{
#ifdef PRE_MOD_PETAL_WRITE
		str = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2030007 ), DN_INTERFACE::UTIL::GetAddCommaString( userReserve - m_Sum ).c_str() );	// UISTRING : 남는 적립금 : %s 페탈
#else // PRE_MOD_PETAL_WRITE
		str = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4708 ), userReserve - m_Sum );	// UISTRING : 남는 적립금 : %d 페탈
#endif // PRE_MOD_PETAL_WRITE
		m_pInfo->SetTextColor((userReserve - m_Sum < 0) ? 0xffff0000 : 0xffffffff);
		m_pInfo->SetText(str.c_str());
	}
#ifdef PRE_ADD_CASHSHOP_CREDIT
	else if( IsBuyCashMode() )
	{
		int rest = userCash - m_Sum;
		m_pInfo->SetTextColor( (rest < 0) ? 0xffff0000 : 0xffffffff );
#ifdef PRE_MOD_PETAL_WRITE
		str = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2030006 ), DN_INTERFACE::UTIL::GetAddCommaString( rest ).c_str() );		// UISTRING : 남는 캐시 : %s 캐시
#else // PRE_MOD_PETAL_WRITE
		str = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4707 ), rest );		// UISTRING : 남는 캐시 : %d 캐시
#endif // PRE_MOD_PETAL_WRITE
		m_pInfo->SetText( str.c_str() );
	}
	else if( IsBuyCreditMode() )
	{
		int rest = userCredit - m_Sum;
		m_pInfo->SetTextColor( (rest < 0) ? 0xffff0000 : 0xffffffff );
#ifdef PRE_MOD_PETAL_WRITE
		std::wstring strFormat = FormatW( L"%s %s", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4844), GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 2030001) );		// UISTRING : 남는 NxCredit : %s NX
		str = FormatW( strFormat.c_str(), DN_INTERFACE::UTIL::GetAddCommaString( rest ).c_str() );
#else // PRE_MOD_PETAL_WRITE
		std::wstring strFormat = FormatW( L"%s %s", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4844), GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4845) );		// UISTRING : 남는 NxCredit : %d NX
		str = FormatW( strFormat.c_str(), rest );
#endif // PRE_MOD_PETAL_WRITE
		m_pInfo->SetText( str.c_str() );
	}
	else
		m_pInfo->SetText( L"" );
#else // PRE_ADD_CASHSHOP_CREDIT
#ifdef PRE_ADD_NEW_MONEY_SEED
	else if( IsBuySeedMode() )
	{
#ifdef PRE_MOD_PETAL_WRITE
		str = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4982 ), DN_INTERFACE::UTIL::GetAddCommaString( userSeed - m_Sum ).c_str() );	// UISTRING : 남는 시드 : %s 시드
#else // PRE_MOD_PETAL_WRITE
		str = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4972 ), userSeed - m_Sum );	// UISTRING : 남는 시드 : %d 시드
#endif // PRE_MOD_PETAL_WRITE
		m_pInfo->SetTextColor( (userSeed - m_Sum < 0) ? 0xffff0000 : 0xffffffff );
		m_pInfo->SetText( str.c_str() );
	}
#endif // PRE_ADD_NEW_MONEY_SEED
	else
	{
		int rest = userCash - m_Sum;
		m_pInfo->SetTextColor((rest < 0) ? 0xffff0000 : 0xffffffff);
#ifdef PRE_MOD_PETAL_WRITE
		str = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2030006 ), DN_INTERFACE::UTIL::GetAddCommaString( rest ).c_str() );		// UISTRING : 남는 캐시 : %s 캐시
#else // PRE_MOD_PETAL_WRITE
		str = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4707 ), rest );		// UISTRING : 남는 캐시 : %d 캐시
#endif // PRE_MOD_PETAL_WRITE
		m_pInfo->SetText(str.c_str());

#ifdef PRE_ADD_NEW_MONEY_SEED
		if( pItemInfo->bCashUsable == false )
		{
			m_pInfo->SetTextColor( 0xffff0000 );
			m_pInfo->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4975 ) );
		}
#endif // PRE_ADD_NEW_MONEY_SEED
	}
#endif // PRE_ADD_CASHSHOP_CREDIT
}

void CDnCashShopPayDlg::Show(bool bShow)
{
	if( m_bShow == bShow )
		return;

#ifdef PRE_ADD_PETALSHOP
	m_pBuyReserve->SetChecked(true);
	m_pBuyReserve->Enable(false);

#ifdef PRE_ADD_CASHSHOP_CREDIT
	m_pBuyCash->SetChecked( false );
	m_pBuyCash->Enable( false );
	m_pBuyCredit->SetChecked( false );
	m_pBuyCredit->Enable( false );
#endif // PRE_ADD_CASHSHOP_CREDIT

	m_pChargeBtn->Enable(false);
#endif

#ifdef PRE_ADD_NEW_MONEY_SEED
	if( bShow )
		m_eBuyMode = CASHSHOP_BUY_CASH;
#endif // PRE_ADD_NEW_MONEY_SEED

#ifdef PRE_ADD_CASHSHOP_CREDIT
	if( bShow )
	{
		if( IsGiftMode() )
		{
			m_pBuyCredit->Enable( false );
			m_pBuyCreditTitle->Enable( false );
		}
		else
		{
			m_pBuyCredit->Enable( true );
			m_pBuyCreditTitle->Enable( true );
			m_pBuyMsg->SetText( L"" );
		}
	}
#endif // PRE_ADD_CASHSHOP_CREDIT

#ifdef PRE_ADD_CADGE_CASH
	if( !bShow )
		GetInterface().GetCashShopDlg()->ResetCadgeMailID();
#endif // PRE_ADD_CADGE_CASH

	CDnCustomDlg::Show( bShow );
}

void CDnCashShopPayDlg::UpdatePage(const CART_ITEM_LIST& itemList)
{
	//	Set Page Static
	UpdatePageControllers(itemList);

	//	Fill Slots
	UpdateItemList(itemList);
}

bool CDnCashShopPayDlg::DoTurnOverPage(bool bPrev, const CART_ITEM_LIST& itemList)
{
	if (itemList.empty() || m_MaxPage <= 1)
		return false;

	m_CurrentPage = bPrev ? m_CurrentPage - 1 : m_CurrentPage + 1;
	CommonUtil::ClipNumber(m_CurrentPage, 1, m_MaxPage);

	UpdatePage(itemList);
	return true;
}

void CDnCashShopPayDlg::PrevPage()
{
	if (m_Mode == eCashUnit_Cart || m_Mode == eCashUnit_Gift_Cart)
	{
		DoTurnOverPage(true, GetCashShopTask().GetCartList());
		return;
	}
	else if (m_Mode == eCashUnit_Gift_Package)
	{
		DoTurnOverPage(true, GetCashShopTask().GetGiftPackageItemList());
		return;
	}
	else if (m_Mode == eCashUnit_Package)
	{
		DoTurnOverPage(true, GetCashShopTask().GetBuyPackageItemList());
		return;
	}
	else if (m_Mode == eCashUnit_BuyItemNow || m_Mode == eCashUnit_GiftItemNow)
	{
		const SCashShopCartItemInfo& info = GetCashShopTask().GetBuyItemNow();
		CART_ITEM_LIST itemList;
		itemList.push_back(info);
		DoTurnOverPage(true, itemList);
	}
	else if (m_Mode == eCashUnit_PreviewCart || m_Mode == eCashUnit_Gift_PreviewCart)
	{
		const CART_ITEM_LIST* pInfo = GetCashShopTask().GetBuyPreviewCartList();
		if (pInfo)
			DoTurnOverPage(true, *pInfo);
	}
}

void CDnCashShopPayDlg::NextPage()
{
	if (m_Mode == eCashUnit_Cart || m_Mode == eCashUnit_Gift_Cart)
	{
		DoTurnOverPage(false, GetCashShopTask().GetCartList());
		return;
	}
	else if (m_Mode == eCashUnit_Package)
	{
		DoTurnOverPage(false, GetCashShopTask().GetBuyPackageItemList());
		return;
	}
	else if (m_Mode == eCashUnit_Gift_Package)
	{
		DoTurnOverPage(false, GetCashShopTask().GetGiftPackageItemList());
		return;
	}
	else if (m_Mode == eCashUnit_BuyItemNow || m_Mode == eCashUnit_GiftItemNow)
	{
		const SCashShopCartItemInfo& info = GetCashShopTask().GetBuyItemNow();
		CART_ITEM_LIST itemList;
		itemList.push_back(info);

		DoTurnOverPage(false, itemList);
		return;
	}
	else if (m_Mode == eCashUnit_PreviewCart || m_Mode == eCashUnit_Gift_PreviewCart)
	{
		const CART_ITEM_LIST* pInfo = GetCashShopTask().GetBuyPreviewCartList();
		if (pInfo)
			DoTurnOverPage(false, *pInfo);
		return;
	}
}

void CDnCashShopPayDlg::Process(float fElapsedTime)
{
	if (IsShow())
	{
#ifdef PRE_ADD_NEW_MONEY_SEED
		if (IsGiftMode())
		{
			m_pBuyReserveTitle->SetTooltipText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4797 ) );	// UISTRING : 페탈로는 선물할 수 없습니다.
			m_pBuyReserve->SetTooltipText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4797 ) );
			m_pBuySeedTitle->SetTooltipText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4981 ) );	// UISTRING : 시드로는 선물할 수 없습니다.
			m_pBuySeed->SetTooltipText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4797 ) );
		}
		else
		{
			m_pBuyReserveTitle->ClearTooltipText();
			m_pBuyReserve->ClearTooltipText();
			m_pBuySeedTitle->ClearTooltipText();
			m_pBuySeed->ClearTooltipText();
		}
#else // PRE_ADD_NEW_MONEY_SEED
		if (IsGiftMode())
		{
			m_pBuyReserveTitle->SetTooltipText(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4797));	// UISTRING : 페탈로는 선물할 수 없습니다.
			m_pBuyReserve->SetTooltipText(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4797));
		}
		else
		{
			m_pBuyReserveTitle->ClearTooltipText();
			m_pBuyReserve->ClearTooltipText();
		}
#endif // PRE_ADD_NEW_MONEY_SEED
	}
	CDnCustomDlg::Process(fElapsedTime);
}

#ifdef PRE_ADD_CASHSHOP_REFUND_CL
bool CDnCashShopPayDlg::IsDirectMoveCashInven() const
{
	if (CDnCashShopTask::GetInstance().IsCashShopMode(eCSMODE_REFUND))
		return (m_pDirectInvenCheckBox->IsChecked() || m_pBuyReserve->IsChecked());

	return true;
}
#endif

void CDnCashShopPayDlg::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	switch( nID ) 
	{
	case MESSAGEBOX_CONFIRM:
		{
			if( strcmp( pControl->GetControlName(), "ID_YES" ) == 0 ) 
			{
#if defined( PRE_ADD_CASHSHOP_CREDIT ) || defined( PRE_ADD_NEW_MONEY_SEED )
				GetCashShopTask().SetBuyMode( m_eBuyMode );
#endif // PRE_ADD_CASHSHOP_CREDIT or PRE_ADD_NEW_MONEY_SEED
				if (m_Mode == eCashUnit_BuyItemNow || m_Mode == eCashUnit_Package)
				{
#ifdef PRE_ADD_SALE_COUPON
					GetCashShopTask().SetApplyCouponSN( m_ApplyCouponSN );
#endif // PRE_ADD_SALE_COUPON
#ifdef PRE_ADD_CASHSHOP_REFUND_CL
					GetCashShopTask().RequestCashShopBuyItemNow(m_pBuyReserve->IsChecked(), IsDirectMoveCashInven());
#else
					GetCashShopTask().RequestCashShopBuyItemNow(m_pBuyReserve->IsChecked());
#endif
				}
				else if (m_Mode == eCashUnit_PreviewCart)
				{
#ifdef PRE_ADD_CASHSHOP_REFUND_CL
					GetCashShopTask().RequestCashShopBuyPreviewCart(m_pBuyReserve->IsChecked(), IsDirectMoveCashInven());
#else
					GetCashShopTask().RequestCashShopBuyPreviewCart(m_pBuyReserve->IsChecked());
#endif
				}
				else if (m_Mode == eCashUnit_GiftItemNow || m_Mode == eCashUnit_Gift_Package)
				{
					const SGiftSendBasicInfo& info = GetCashShopTask().GetGiftBasicInfo();
#ifdef PRE_ADD_CADGE_CASH
					GetCashShopTask().RequestCashShopSendGiftItemNow( info.receiverName, info.memo, GetInterface().GetCashShopDlg()->GetCadgeMailID() );
					GetInterface().GetCashShopDlg()->ResetCadgeMailID();
#else // PRE_ADD_CADGE_CASH
					GetCashShopTask().RequestCashShopSendGiftItemNow(info.receiverName, info.memo);
#endif // PRE_ADD_CADGE_CASH
				}
				else if (m_Mode == eCashUnit_Gift_PreviewCart)
				{
					const SGiftSendBasicInfo& info = GetCashShopTask().GetGiftBasicInfo();
					GetCashShopTask().RequestCashShopSendGiftPreviewCart(info.receiverName, info.memo);
				}
				else if (m_Mode == eCashUnit_Gift_Cart)
				{
					const SGiftSendBasicInfo& info = GetCashShopTask().GetGiftBasicInfo();
#ifdef PRE_ADD_CADGE_CASH
					GetCashShopTask().RequestCashShopSendGiftCart( info.receiverName, info.memo, GetInterface().GetCashShopDlg()->GetCadgeMailID() );
					GetInterface().GetCashShopDlg()->ResetCadgeMailID();
#else // PRE_ADD_CADGE_CASH
					GetCashShopTask().RequestCashShopSendGiftCart(info.receiverName, info.memo);
#endif // PRE_ADD_CADGE_CASH
				}
				else
				{
#ifdef PRE_ADD_CASHSHOP_REFUND_CL
					GetCashShopTask().RequestCashShopBuyCart(m_pBuyReserve->IsChecked(), IsDirectMoveCashInven());
#else
					GetCashShopTask().RequestCashShopBuyCart(m_pBuyReserve->IsChecked());
#endif
				}
			}
		}
		break;
#ifdef PRE_ADD_CASHSHOP_CREDIT
	case MESSAGEBOX_CREDIT_ERROR:
		{
			if( strcmp( pControl->GetControlName(), "ID_OK" ) == 0 )
			{
				m_pBuyCredit->SetChecked( false );
			}
		}
		break;
#endif // PRE_ADD_CASHSHOP_CREDIT
	}
}

void CDnCashShopPayDlg::ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg)
{
	SetCmdControlName(pControl->GetControlName());

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl("ID_BUTTON_PGUP" ) )
		{
			PrevPage();
			return;
		}

		if( IsCmdControl("ID_BUTTON_PGDN" ) )
		{
			NextPage();
			return;
		}

		if (IsCmdControl("ID_BUTTON_CANCEL") ||
			IsCmdControl("ID_BUTTON_CLOSE"))
		{
			Show(false);
			return;
		}

		if (IsCmdControl("ID_BUTTON_BUY"))
		{
#ifdef PRE_ADD_CASHSHOP_CREDIT
			if( !IsBuyCashMode() && !IsBuyReserveMode() && !IsBuyCreditMode() )
			{
				GetInterface().MessageBox( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4843), MB_OK, MESSAGEBOX_ERROR, this );	// UISTRING : 결제 수단을 선택하셔야 구매할 수 있습니다.
				return;
			}
#endif // PRE_ADD_CASHSHOP_CREDIT

#ifdef PRE_ADD_NEW_MONEY_SEED
			if( !IsBuyReserveMode() && !IsBuySeedMode() )
			{
				const SCashShopCartItemInfo& info = GetCashShopTask().GetBuyItemNow();
				CASHITEM_SN nLastPresentSN = 0;

				if( m_Mode == eCashUnit_Cart || m_Mode == eCashUnit_Gift_Cart || m_Mode == eCashUnit_PreviewCart || m_Mode == eCashUnit_Gift_PreviewCart )
					nLastPresentSN = m_nLastPresentSN;
				else
					nLastPresentSN = info.presentSN;

				const SCashShopItemInfo* pItemInfo = GetCashShopTask().GetItemInfo( nLastPresentSN );
				if( !pItemInfo->bCashUsable )
				{
					GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4975 ), MB_OK, MESSAGEBOX_ERROR, this );	// UISTRING : 캐시로 구매할 수 없는 상품입니다.
					return;
				}
			}
#endif // PRE_ADD_NEW_MONEY_SEED

			eCashShopError err = GetCashShopTask().IsAllCartItemSelected(m_Mode, true);
			if (err != eERRCS_NONE)
				return;

			if (m_Sum < 0)
				return;

			bool bCanBuy = HasEnoughMoneyToBuy(m_Sum, IsBuyReserveMode());
			if (bCanBuy == false)
			{
				std::wstring str;
				if (IsBuyReserveMode())
				{
					GetInterface().GetServerMessage(str, 528);	// UISTRING : 페탈이 부족합니다.
				}
				else
				{
					str = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4771); // UISTRING : 소지한 금액이 부족합니다.
				}
				GetInterface().MessageBox(str.c_str(), MB_OK, MESSAGEBOX_ERROR, this);
				return;
			}

			// todo by kalliste : Remove duplicate with DoBuy()
			std::wstring str;
			if (IsGiftMode())
			{
				const SGiftSendBasicInfo& info = GetCashShopTask().GetGiftBasicInfo();
				if (info.IsEmpty() == false)
				{
					if (GetCashShopTask().IsGiftNeedWarning(m_Mode))
					{
#ifdef PRE_ADD_CASHSHOP_REFUND_CL
						if (CDnCashShopTask::GetInstance().IsCashShopMode(eCSMODE_REFUND))
							str = FormatW(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4828), info.receiverName.c_str()); // UISTRING : 선물에 대상이 더 가지거나 늘릴수 없으면, 받을 수 없는 상품이 있습니다. 선물전 확인이 필요하며, 선물후에는 청약철회가 불가능합니다. 정말로 %s님께 선물하시겠습니까?
						else
							str = FormatW(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4767)); // UISTRING : 선물 목록에 상대방이 더 가지거나 늘릴 수 없는 경우, 받을 수 없는 상품이 있습니다. 선물하기 전에 꼭 확인하셔야 합니다. 정말로 선물하시겠습니까?
#else
						str = FormatW(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4767)); // UISTRING : 선물 목록에 상대방이 더 가지거나 늘릴 수 없는 경우, 받을 수 없는 상품이 있습니다. 선물하기 전에 꼭 확인하셔야 합니다. 정말로 선물하시겠습니까?
#endif
					}
					else
					{
#ifdef PRE_ADD_CASHSHOP_REFUND_CL
						if (CDnCashShopTask::GetInstance().IsCashShopMode(eCSMODE_REFUND))
							str = FormatW(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4827), info.receiverName.c_str()); // UISTRING : 선물한 상품은 청약철회 할 수 없습니다. 정말로 %s님께 선물하시겠습니까?
						else
							str = FormatW(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4768), info.receiverName.c_str()); // UISTRING : 정말로 %s님께 선물하시겠습니까?
#else
						str = FormatW(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4768), info.receiverName.c_str()); // UISTRING : 정말로 %s님께 선물하시겠습니까?
#endif
					}
				}
				else
				{
					Show(false);
					std::wstring str = FormatW(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4789), L"NO GIFT BASIC INFO"); // UISTRING : 선물 프로세스 중 에러가 발생했습니다
					GetInterface().MessageBox(str.c_str(), MB_OK);
					return;
				}
				GetInterface().MessageBox(str.c_str(), MB_YESNO, MESSAGEBOX_CONFIRM, this);
			}
			else
			{
#ifdef PRE_ADD_CASHSHOP_REFUND_CL
				int msgNumber = 4750;	// UISTRING : 본 아이템은 구입 시점부터 캐릭터에 적용되어 해제할 수 없기 때문에, 환불이 불가능 합니다. 정말로 구매하시겠습니까?
				if (CDnCashShopTask::GetInstance().IsCashShopMode(eCSMODE_REFUND) && m_pDirectInvenCheckBox->IsChecked() == false)
				{
					if (CDnCashShopTask::GetInstance().IsAllCartItemRefundable(m_Mode))
						msgNumber = 1725;	// UISTRING : 정말로 구매하시겠습니까?
				}

				str = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, msgNumber);
#else
				str = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4750); // UISTRING : 본 아이템은 구입 시점부터 캐릭터에 적용되어 해제할 수 없기 때문에, 환불이 불가능 합니다. 정말로 구매하시겠습니까?
#endif
				if (m_pCashShopMsgBox)
				{
					m_pCashShopMsgBox->SetMsgBoxText(CDnCashShopMessageBox::eMain, str);
					if (CDnCashShopTask::GetInstance().IsCashTradable())
					{
						str = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 3628); // UISTRING : 기간 무제한 상품 중 (거래소 등록 가능) 아이템은 게임 내 거래소에 등록 할 수 있습니다.
						m_pCashShopMsgBox->SetMsgBoxText(CDnCashShopMessageBox::eSub, str);
					}
					m_pCashShopMsgBox->SetMsgBox(MESSAGEBOX_CONFIRM, this);
				}
			}
		}

#ifdef PRE_ADD_CADGE_CASH
		if( IsCmdControl( "ID_BUTTON_ASK" ) )
		{
			eCashShopError err = GetCashShopTask().IsCheckEnableCadge( m_Mode );

			if( err == eERRCS_NONE )
			{
				// 조르기 창 열기
				m_pCommonDlgMgr->OpenCadgeDlg( m_Mode );
			}
			else
			{
				switch( err )
				{
					case eERRCS_NO_ALL_ITEM_PERIOD_SELECTED:
						GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4911 ), MB_OK, MESSAGEBOX_ERROR, this );
						break;
					case eERRCS_NO_ALL_ITEM_ABILITY_SELECTED:
						GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4910 ), MB_OK, MESSAGEBOX_ERROR, this );
						break;
					case eERRCS_NO_ALL_ITEM_SKILL_SELECTED:
						GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4912 ), MB_OK, MESSAGEBOX_ERROR, this );
						break;
					case eERRCS_GIFT_UNABLE_NOGIFT_OPTION:
						GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4909 ), MB_OK, MESSAGEBOX_ERROR, this );
						break;
				}
				return;
			}
		}
#endif // PRE_ADD_CADGE_CASH

		if (IsCmdControl("ID_BUTTON_CHARGE"))
		{
			GetCashShopTask().OpenChargeBrowser();
			return;
		}

		const std::string& cmdName = GetCmdControlName();
		std::string::size_type numberPos = cmdName.find("ID_BUTTON_CLOSE");
		if (numberPos != std::string::npos)
		{
			size_t pieceLen = strlen("ID_BUTTON_CLOSE");
			if (pieceLen < cmdName.size())
			{
				int slotIdx = atoi(&(cmdName[pieceLen]));
				
				std::map<int, int>::const_iterator iter = m_DataSlotIdxMatchList.find(slotIdx);
				if (iter != m_DataSlotIdxMatchList.end())
				{
					const int& dataIdx = (*iter).second;

					if (m_Mode == eCashUnit_Cart || m_Mode == eCashUnit_Gift_Cart)
					{
						CDnCashShopTask::GetInstance().RemoveCartItem(dataIdx);

						UpdateItemList(CDnCashShopTask::GetInstance().GetCartList());
						UpdateInfoList(CDnCashShopTask::GetInstance().GetCartList());

						if (CDnCashShopTask::GetInstance().GetCartItemCount() <= 0)
							Show(false);
					}
					else if (m_Mode == eCashUnit_PreviewCart || m_Mode == eCashUnit_Gift_PreviewCart)
					{
						bool bPetPreview = false;
						if( CDnCashShopTask::GetInstance().GetPreviewCartClassId() == PET_CLASS_ID )
							bPetPreview = true;

						CDnCashShopTask::GetInstance().RemovePreviewCartItemById(dataIdx);

						if( bPetPreview )
						{
							Show( false );
						}
						else
						{
							const CART_ITEM_LIST* pList = CDnCashShopTask::GetInstance().GetBuyPreviewCartList();
							if( pList )
							{
								UpdateItemList(*pList);
								UpdateInfoList(*pList);
							}

							if (CDnCashShopTask::GetInstance().GetBuyPreviewCartCount() <= 0)
								Show(false);
						}
					}
				}
			}
		}
	}
	else if (nCommand == EVENT_CHECKBOX_CHANGED)
	{
#ifdef PRE_ADD_CASHSHOP_REFUND_CL
		if (IsCmdControl("ID_CHECKBOX_RESERVE"))
		{
			if (CDnCashShopTask::GetInstance().IsCashShopMode(eCSMODE_REFUND))
			{
				m_pDirectInvenCheckBox->SetChecked(m_pBuyReserve->IsChecked(), false);
				m_pDirectInvenCheckBoxTitle->Enable(m_pBuyReserve->IsChecked() == false);
				m_pDirectInvenCheckBox->Enable(m_pBuyReserve->IsChecked() == false);
			}

			UpdateEtc();
		}

		SetRefundStatic();
#else
#ifdef PRE_ADD_CASHSHOP_CREDIT
		if( IsCmdControl( "ID_CHECKBOX_CASH" ) )
		{
			if( m_pBuyCash->IsChecked() )
			{
				m_pBuyReserve->SetChecked( false );
				m_pBuyCredit->SetChecked( false );
				m_eBuyMode = CASHSHOP_BUY_CASH;
			}
		}
		else if( IsCmdControl( "ID_CHECKBOX_RESERVE" ) )
		{
			if( m_pBuyReserve->IsChecked() )
			{
				m_pBuyCash->SetChecked( false );
				m_pBuyCredit->SetChecked( false );
				m_eBuyMode = CASHSHOP_BUY_RESERVE;
			}
		}
		else if( m_pBuyCredit->IsEnable() && IsCmdControl( "ID_CHECKBOX_CREDIT" ) )
		{
			if( m_pBuyCredit->IsChecked() )
			{
				// credit으로 구매 불가항목이 있는지 검사
				const SCashShopItemInfo* pItemInfo = CheckCreditableItem();
				if( pItemInfo != NULL )
				{
					WCHAR wzStr[1024] = {0,};
					swprintf_s( wzStr, _countof(wzStr), L"%s\n%s", GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4839), pItemInfo->nameString.c_str() );

					GetInterface().MessageBox( wzStr, MB_OK, MESSAGEBOX_CREDIT_ERROR, this );
				}

				m_pBuyCash->SetChecked( false );
				m_pBuyReserve->SetChecked( false );
				m_eBuyMode = CASHSHOP_BUY_CREDIT;
			}
		}
		else
		//if( !m_pBuyCash->IsChecked() || !m_pBuyReserve->IsChecked() || !m_pBuyCredit->IsChecked() )
			m_eBuyMode = CASHSHOP_BUY_NONE;

		if( IsCmdControl( "ID_CHECKBOX_CASH" ) || IsCmdControl( "ID_CHECKBOX_RESERVE" ) || IsCmdControl( "ID_CHECKBOX_CREDIT" ) )
			UpdateEtc();
#else // PRE_ADD_CASHSHOP_CREDIT
#ifdef PRE_ADD_NEW_MONEY_SEED
		if( IsCmdControl( "ID_CHECKBOX_RESERVE" ) )
		{
			if( m_pBuyReserve->IsChecked() )
				m_pBuySeed->SetChecked( false );
		}
		else if( IsCmdControl( "ID_CHECKBOX_SEED" ) )
		{
			if( m_pBuySeed->IsChecked() )
				m_pBuyReserve->SetChecked( false );
		}

		if( IsCmdControl( "ID_CHECKBOX_RESERVE" ) || IsCmdControl( "ID_CHECKBOX_SEED" ) )
		{
			if( m_pBuyReserve->IsChecked() )
				m_eBuyMode = CASHSHOP_BUY_RESERVE;
			else if( m_pBuySeed->IsChecked() )
				m_eBuyMode = CASHSHOP_BUY_SEED;
			else
				m_eBuyMode = CASHSHOP_BUY_CASH;

			UpdateEtc();
		}
#else // PRE_ADD_NEW_MONEY_SEED
		if( IsCmdControl( "ID_CHECKBOX_RESERVE" ) )
			UpdateEtc();
#endif // PRE_ADD_NEW_MONEY_SEED
#endif // PRE_ADD_CASHSHOP_CREDIT
#endif
	}
	if ((nCommand == EVENT_COMBOBOX_SELECTION_CHANGED) && bTriggeredByUser)
	{
		const std::string& cmdName = GetCmdControlName();
		std::string::size_type numberPos = cmdName.find("ID_COMBOBOX_PRICE");
		if (numberPos != std::string::npos)
		{
			size_t pieceLen = strlen("ID_COMBOBOX_PRICE");
			if (pieceLen < cmdName.size())
			{
				int slotIdx = atoi(&(cmdName[pieceLen]));

				std::map<int, int>::const_iterator iter = m_DataSlotIdxMatchList.find(slotIdx);
				if (iter != m_DataSlotIdxMatchList.end())
				{
					const int& dataIdx = (*iter).second;

					SComboBoxItem* pItem = m_SlotUnits[slotIdx].pPeriodComboBox->GetSelectedItem();
					if (pItem)
					{
						GetCashShopTask().SelectCartItemSN(m_Mode, dataIdx, pItem->nValue);
						if (m_Mode == eCashUnit_Cart || m_Mode == eCashUnit_Gift_Cart)
						{
							UpdateInfoList(GetCashShopTask().GetCartList());
						}
						else if (m_Mode == eCashUnit_BuyItemNow || m_Mode == eCashUnit_Package)
						{
							CART_ITEM_LIST list;
							list.push_back(GetCashShopTask().GetBuyItemNow());
							UpdateInfoList(list);
						}
						else if (m_Mode == eCashUnit_PreviewCart || m_Mode == eCashUnit_Gift_PreviewCart)
						{
							const CART_ITEM_LIST* pList = GetCashShopTask().GetBuyPreviewCartList();
							if (pList)
								UpdateInfoList(*pList);
						}
						else if (m_Mode == eCashUnit_GiftItemNow || m_Mode == eCashUnit_Gift_Package)
						{
							CART_ITEM_LIST list;
							list.push_back(GetCashShopTask().GetBuyItemNow());
							UpdateInfoList(list);
						}

						SPaySlotUnit& unit = m_SlotUnits[slotIdx];
						CASHITEM_SN newSN = unit.pItem->GetCashItemSN();
						if (GetCashShopTask().IsValidSN(pItem->nValue))
							newSN = pItem->nValue;
						unit.pItem->SetCashItemSN(newSN);
						unit.pItem->SetEternityItem(CDnCashShopTask::GetInstance().IsItemPermanent(newSN));
					}
				}
			}
		}

		numberPos = cmdName.find("ID_COMBOBOX_ABILITY");
		if (numberPos != std::string::npos)
		{
			size_t pieceLen = strlen("ID_COMBOBOX_ABILITY");
			if (pieceLen < cmdName.size())
			{
				int slotIdx = atoi(&(cmdName[pieceLen]));

				std::map<int, int>::const_iterator iter = m_DataSlotIdxMatchList.find(slotIdx);
				if (iter != m_DataSlotIdxMatchList.end())
				{
					const int& dataIdx = (*iter).second;

					SComboBoxItem* pItem = m_SlotUnits[slotIdx].pAbilityComboBox->GetSelectedItem();
					if (pItem)
					{
						int itemListidx = ((m_CurrentPage - 1) * _MAX_PAY_SLOT_NUM_PER_PAGE) + slotIdx;
						SetCartItemAbility(dataIdx, pItem->nValue, m_vecWillPayItemIndex[itemListidx]);
					}
				}
			}
		}
		numberPos = cmdName.find("ID_COMBOBOX_SKILL");
		if (numberPos != std::string::npos)
		{
			size_t pieceLen = strlen("ID_COMBOBOX_SKILL");
			if( pieceLen < cmdName.size() )
			{
				int slotIdx = atoi(&(cmdName[pieceLen]));

				std::map<int, int>::const_iterator iter = m_DataSlotIdxMatchList.find(slotIdx);
				if( iter != m_DataSlotIdxMatchList.end() )
				{
					const int& dataIdx = (*iter).second;
					int nOptionIndex = m_SlotUnits[slotIdx].pSkillComboBox->GetSelectedIndex();
					SetCartItemSkill( dataIdx, nOptionIndex );
				}
			}
		}
	}

	CDnCustomDlg::ProcessCommand(nCommand, bTriggeredByUser, pControl, uMsg);
}

void CDnCashShopPayDlg::SetCartItemAbility(int id, ITEMCLSID itemId, int itemListIndex)
{
	if (m_Mode == eCashUnit_Cart || m_Mode == eCashUnit_Gift_Cart)
	{
		GetCashShopTask().SetCartItemAbility(id, itemId);
		UpdateInfoList(GetCashShopTask().GetCartList());
	}
	else if (m_Mode == eCashUnit_Gift_Package)
	{
		GetCashShopTask().SetPackageItemAbility(true, id, itemId, itemListIndex);
		UpdateInfoList(GetCashShopTask().GetGiftPackageItemList());
	}
	else if (m_Mode == eCashUnit_Package)
	{
		GetCashShopTask().SetPackageItemAbility(false, id, itemId, itemListIndex);
		UpdateInfoList(GetCashShopTask().GetBuyPackageItemList());
	}
	else if (m_Mode == eCashUnit_BuyItemNow)
	{
		GetCashShopTask().SetBuyItemNowAbility(itemId);
		CART_ITEM_LIST list;
		list.push_back(GetCashShopTask().GetBuyItemNow());
		UpdateInfoList(list);
	}
	else if (m_Mode == eCashUnit_PreviewCart || m_Mode == eCashUnit_Gift_PreviewCart)
	{
		GetCashShopTask().SetPreviewCartItemAbility(id, itemId);

		const CART_ITEM_LIST* pList = GetCashShopTask().GetBuyPreviewCartList();
		if (pList)
			UpdateInfoList(*pList);
	}
	else if (m_Mode == eCashUnit_GiftItemNow)
	{
		GetCashShopTask().SetBuyItemNowAbility(itemId);
		CART_ITEM_LIST list;
		list.push_back(GetCashShopTask().GetBuyItemNow());
		UpdateInfoList(list);
	}
	else
	{
		_ASSERT(0);
		GetCashShopTask().HandleCashShopError(eERRCS_CART_NOT_EXIST, false);
	}
}

void CDnCashShopPayDlg::SetCartItemSkill( int id, int nOptionIndex )
{ 
	if( m_Mode == eCashUnit_Cart || m_Mode == eCashUnit_Gift_Cart )
	{
		GetCashShopTask().SetCartItemSkill( id, nOptionIndex );
		UpdateInfoList( GetCashShopTask().GetCartList() );
	}
	else if( m_Mode == eCashUnit_Gift_Package )
	{
		GetCashShopTask().SetPackageItemSkill( true, id, nOptionIndex );
		UpdateInfoList( GetCashShopTask().GetGiftPackageItemList() );
	}
	else if( m_Mode == eCashUnit_Package )
	{
		GetCashShopTask().SetPackageItemSkill( false, id, nOptionIndex );
		UpdateInfoList( GetCashShopTask().GetBuyPackageItemList() );
	}
	else if( m_Mode == eCashUnit_BuyItemNow )
	{
		GetCashShopTask().SetBuyItemNowSkill( nOptionIndex );
		CART_ITEM_LIST list;
		list.push_back( GetCashShopTask().GetBuyItemNow() );
		UpdateInfoList( list );
	}
	else if( m_Mode == eCashUnit_PreviewCart || m_Mode == eCashUnit_Gift_PreviewCart )
	{
		GetCashShopTask().SetPreviewCartItemSkill( id, nOptionIndex );
		const CART_ITEM_LIST* pList = GetCashShopTask().GetBuyPreviewCartList();
		if( pList )
			UpdateInfoList( *pList );
	}
	else if( m_Mode == eCashUnit_GiftItemNow )
	{
		GetCashShopTask().SetBuyItemNowSkill( nOptionIndex );
		CART_ITEM_LIST list;
		list.push_back( GetCashShopTask().GetBuyItemNow() );
		UpdateInfoList( list );
	}
	else
	{
		_ASSERT(0);
		GetCashShopTask().HandleCashShopError( eERRCS_CART_NOT_EXIST, false );
	}
}

bool CDnCashShopPayDlg::IsGiftMode() const
{
	return (m_Mode >= eCashUnit_Gift_Min && m_Mode < eCashUnit_Gift_Max);
}

#ifdef PRE_ADD_CASHSHOP_REFUND_CL
void CDnCashShopPayDlg::SetRefundStatic()
{
	if (IsGiftMode())
	{
		m_pDirectInvenStatic->ClearText();
	}
	else
	{
		if (m_pBuyReserve->IsChecked())
			m_pDirectInvenStatic->SetText(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4814)); // UISTRING : 페탈로 구매 캐시 소지품으로 바로 이동
#ifdef PRE_ADD_CASHSHOP_ACTOZ
		// 변경된 액토즈UI에서는 캐시소지품 체크처리가 제외됨.
#else
		else if (m_pDirectInvenCheckBox->IsChecked())
			m_pDirectInvenStatic->SetText(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4818)); // UISTRING : 캐시 소지품 창으로 이동 시 청약철회 불가
#endif // PRE_ADD_CASHSHOP_ACTOZ
		else
			m_pDirectInvenStatic->SetText(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4815)); // UISTRING : 결제 소지품에서 우클릭 이동하여 사용
	}
}
#endif

#ifdef PRE_ADD_CASHSHOP_CREDIT
const SCashShopItemInfo* CDnCashShopPayDlg::CheckCreditableItem()
{
	const SCashShopItemInfo* pRetItemInfo = NULL;
	if( m_Mode == eCashUnit_Cart || m_Mode == eCashUnit_Gift_Cart )
	{
		const CART_ITEM_LIST& cartList = GetCashShopTask().GetCartList();
		CART_ITEM_LIST::const_iterator iter = cartList.begin();
		for( ; iter != cartList.end(); iter++ )
		{
			const SCashShopItemInfo* pItemInfo = GetCashShopTask().GetItemInfo( (*iter).presentSN );
			if( !pItemInfo->bCreditAble )
			{
				pRetItemInfo = pItemInfo;
				break;
			}
		}
	}
	else if( m_Mode == eCashUnit_Package )
	{
		const CART_ITEM_LIST& cartList = GetCashShopTask().GetBuyPackageItemList();
		CART_ITEM_LIST::const_iterator iter = cartList.begin();
		for( ; iter != cartList.end(); iter++ )
		{
			const SCashShopItemInfo* pItemInfo = GetCashShopTask().GetItemInfo( (*iter).presentSN );
			if( !pItemInfo->bCreditAble )
			{
				pRetItemInfo = pItemInfo;
				break;
			}
		}
	}
	else if( m_Mode == eCashUnit_BuyItemNow || m_Mode == eCashUnit_GiftItemNow )
	{
		const SCashShopCartItemInfo& info = GetCashShopTask().GetBuyItemNow();
		const SCashShopItemInfo* pItemInfo = GetCashShopTask().GetItemInfo( info.presentSN );
		if( !pItemInfo->bCreditAble )
			pRetItemInfo = pItemInfo;
	}
	else if( m_Mode == eCashUnit_PreviewCart || m_Mode == eCashUnit_Gift_PreviewCart )
	{
		const CART_ITEM_LIST* cartList = GetCashShopTask().GetBuyPreviewCartList();
		if( cartList )
		{
			CART_ITEM_LIST::const_iterator iter = cartList->begin();
			for( ; iter != cartList->end(); iter++ )
			{
				const SCashShopItemInfo* pItemInfo = GetCashShopTask().GetItemInfo( (*iter).presentSN );
				if( !pItemInfo->bCreditAble )
				{
					pRetItemInfo = pItemInfo;
					break;
				}
			}
		}
	}
	else if( m_Mode == eCashUnit_Gift_Package )
	{
		const CART_ITEM_LIST& cartList = GetCashShopTask().GetGiftPackageItemList();
		CART_ITEM_LIST::const_iterator iter = cartList.begin();
		for( ; iter != cartList.end(); iter++ )
		{
			const SCashShopItemInfo* pItemInfo = GetCashShopTask().GetItemInfo( (*iter).presentSN );
			if( !pItemInfo->bCreditAble )
			{
				pRetItemInfo = pItemInfo;
				break;
			}
		}
	}

	return pRetItemInfo;
}
#endif // PRE_ADD_CASHSHOP_CREDIT