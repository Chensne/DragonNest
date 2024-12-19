#include "StdAfx.h"
#include "DnCommonUtil.h"
#include "DnCashShopGoodsDlg.h"
#include "DnCashShopPayDlg.h"
#include "DnCashShopCommonDlgMgr.h"
#include "DnCashShopTask.h"
#include "DnItemTask.h"
#include "DnInterfaceString.h"
#include "DnInterface.h"
#include "DnTableDB.h"
#include "DnCashShopDlg.h"
#include "DnCashShopPreviewDlg.h"
#include "DnPetActor.h"
#ifdef PRE_ADD_LIMITED_CASHITEM
#include "DnSimpleTooltipDlg.h"
#endif // PRE_ADD_LIMITED_CASHITEM

#ifdef PRE_ADD_CASHSHOP_RENEWAL
#include <wininet.h>
#include "shlobj.h"
#include "DnMainFrame.h"
#include "DnTableDB.h"
#endif // PRE_ADD_CASHSHOP_RENEWAL

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

#ifdef PRE_ADD_CASHSHOP_RENEWAL
std::vector< EtTextureHandle > CDnCashShopGoodsDlg::SCashShopSlotUnit::vStateTexture;
char * g_strControlName[32] = { "ID_CASH_LIMITEDCLOSE", "ID_CASH_HOT", "ID_CASH_NEW", "ID_CASH_EVENT", "ID_CASH_DISCOUNT", "ID_CASH_LIMITED", "ID_CASH_LIMITEDITEM", "ID_CASH_COUPON" };
const int g_strControlSize = 8;
const int g_BannerRolling = 6;
#endif // PRE_ADD_CASHSHOP_RENEWAL


void CDnCashShopGoodsDlg::SCashShopSlotUnit::Set(const SCashShopItemInfo& data, int userClassId)
{
	Clear();

	bool bEnableJob = true;
	if (userClassId != 0 && userClassId < _countof(data.bEnableJob))
	{
		bEnableJob = data.bEnableJob[userClassId];
		if (bEnableJob == false)
		{
			pItemName->Enable(false);
			pBuyBtn->Enable(false);
#ifdef PRE_ADD_PETALSHOP
			pCartBtn->Enable(false);
#else
			pCartBtn->Enable(true);
#endif // PRE_ADD_PETALSHOP
#ifdef PRE_MOD_SALE_COUPON_LEVEL_LIMIT
			pBuyBtn->SetTooltipText( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 9172) );	// UISTRING : »ç¿ë °¡´ÉÇÑ Å¬·¡½º°¡ ´Þ¶ó ¾ÆÀÌÅÛÀ» ±¸¸ÅÇÒ ¼ö ¾÷½À´Ï´Ù.
#endif // PRE_MOD_SALE_COUPON_LEVEL_LIMIT
		}
	}

	if (GetCashShopTask().IsPackageItem(data.sn) || data.bCartAble == false)
		pCartBtn->Enable(false);

#ifdef PRE_ADD_CASHSHOP_RENEWAL
	// ...
#else
	DWORD itemNameColor = GetNameColor((eGoodsIconType)data.state);
#endif // PRE_ADD_CASHSHOP_RENEWAL

#ifdef PRE_MOD_SALE_COUPON_LEVEL_LIMIT
	if( bEnableJob && data.bLimitUseCouponLevel && data.nLimitUseCouponLevel > 0 )
	{
		std::wstring str;
		str = FormatW( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 9173), data.nLimitUseCouponLevel );	// UISTRING : %d·¹º§ ÀÌ»óºÎÅÍ ±¸¸Å °¡´ÉÇÕ´Ï´Ù.

		pBuyBtn->Enable( false );
		pBuyBtn->SetTooltipText( str.c_str() );
		pGiftBtn->Enable( false );
		pCartBtn->Enable( false );
		pItemName->Enable( false );
	}
#endif // PRE_MOD_SALE_COUPON_LEVEL_LIMIT

#ifdef PRE_ADD_CASHSHOP_RENEWAL
	// ...
#else
	pItemName->SetTextColor(itemNameColor, true);
#endif // PRE_ADD_CASHSHOP_RENEWAL
	pItemName->SetText(data.nameString.c_str());

	if (data.presentItemId != ITEMCLSID_NONE)
	{
		SAFE_DELETE(pItem);
		TItemInfo itemInfo;
		if (CDnItem::MakeItemInfo(data.presentItemId, data.count, itemInfo))
		{
			pItem = GetItemTask().CreateItem(itemInfo);
			if (pItem)
			{
				pItem->SetCashItemSN(data.sn);
				pQuickSlotBtn->SetQuickItem(pItem);
			}
			else
			{
				_ASSERT(0);
				return;
			}
		}
	}

	std::wstring str;
	str = FormatW(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4625), data.count);	// UISTRING : %d °³
	pItemCount->SetText(str.c_str());

#ifdef PRE_ADD_NEW_MONEY_SEED
	if( data.bCashUsable && data.bReserveOffer && data.bSeedReserveAmount )
	{
		pItemReserve->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4984 ) );
		nReserveAmount = data.ReserveAmount;
		nSeedReserveAmount = data.SeedReserveAmount;
	}
	else if( data.bCashUsable && data.bReserveOffer )
	{
#ifdef PRE_MOD_PETAL_WRITE
		str = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2030005 ), DN_INTERFACE::UTIL::GetAddCommaString( data.ReserveAmount ).c_str() ); // UISTRING : (%s ÆäÅ»Àû¸³)
#else // PRE_MOD_PETAL_WRITE
		str = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4658 ), data.ReserveAmount ); // UISTRING : (%d ÆäÅ»Àû¸³)
#endif // PRE_MOD_PETAL_WRITE
		pItemReserve->SetText( str.c_str() );
		nReserveAmount = data.ReserveAmount;
	}
	else if( data.bCashUsable && data.bSeedReserveAmount )
	{
#ifdef PRE_MOD_PETAL_WRITE
		str = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4985 ), DN_INTERFACE::UTIL::GetAddCommaString( data.SeedReserveAmount ).c_str() ); // UISTRING : (%s ½ÃµåÀû¸³)
#else // PRE_MOD_PETAL_WRITE
		str = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4971 ), data.SeedReserveAmount ); // UISTRING : (%d ½ÃµåÀû¸³)
#endif // PRE_MOD_PETAL_WRITE
		pItemReserve->SetText( str.c_str() );
		nSeedReserveAmount = data.SeedReserveAmount;
	}
#else // PRE_ADD_NEW_MONEY_SEED
	if (data.bReserveOffer)
	{
#ifdef PRE_MOD_PETAL_WRITE
		str = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2030005 ), DN_INTERFACE::UTIL::GetAddCommaString( data.ReserveAmount ).c_str() ); // UISTRING : (%s ÆäÅ»Àû¸³)
#else // PRE_MOD_PETAL_WRITE
		str = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4658 ), data.ReserveAmount ); // UISTRING : (%d ÆäÅ»Àû¸³)
#endif // PRE_MOD_PETAL_WRITE
		pItemReserve->SetText(str.c_str());
	}
#endif // PRE_ADD_NEW_MONEY_SEED

	if (data.linkIdList.size() > 1)
	{
#ifdef PRE_ADD_VIP
		if (data.type != eCSType_Costume && data.type != eCSType_Term)
#else
		if (data.type != eCSType_Costume)
#endif
		{
			std::wstring str;
			str = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4756);	// UISTRING : ±â°£¼±ÅÃ
			if (pItemPeriod->IsEnable())
				pItemPeriod->SetTextColor(textcolor::YELLOW);
			pItemPeriod->SetText(str.c_str());
		}
		else
		{
			pItemPeriod->Show(false);
		}
	}
	else
	{
		pItemPeriod->Show(false);
	}

#ifdef PRE_ADD_LIMITED_CASHITEM
	if( dwTextColor == 0 )
		dwTextColor = pItemDiscountRate->GetTextColor();
	pItemDiscountRate->SetTextColor( dwTextColor );

	if( data.bLimit && GetCashShopTask().GetCashLimitItemMaxCount( data.sn ) > 0 )
	{
		std::wstring strRemainCount;
		strRemainCount = FormatW( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4895), GetCashShopTask().GetCashLimitItemRemainCount( data.sn ) ); // UISTRING : %d°³ ³²À½
	
		if( GetCashShopTask().GetCashLimitItemRemainCount( data.sn ) == 0 )
			pItemDiscountRate->SetTextColor( textcolor::RED );

		pItemDiscountRate->SetText( strRemainCount.c_str() );
	}
	else if( data.priceFix > 0 )
	{
		std::wstring fixedPrice;
#ifdef PRE_MOD_PETAL_WRITE
		fixedPrice = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2030000 ), DN_INTERFACE::UTIL::GetAddCommaString( data.priceFix ).c_str() );	// UITSRING : %s ¡æ
#else // PRE_MOD_PETAL_WRITE
		fixedPrice = FormatW(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4733 ), data.priceFix);	// UITSRING : %d ¡æ
#endif // PRE_MOD_PETAL_WRITE
		pItemPrice->AppendText(fixedPrice.c_str(), D3DCOLOR_ARGB(255,189,44,58));

		std::wstring discountString;
		discountString = FormatW(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4718), GetCashShopTask().GetCashShopDiscountRate((float)data.price, (float)data.priceFix, (float)data.ReserveAmount)); // UISTRING : %d%% ÇÒÀÎ
		pItemDiscountRate->SetText(discountString.c_str());
	}
#else // PRE_ADD_LIMITED_CASHITEM
	std::wstring fixedPrice;
	if (data.priceFix > 0)
	{
#ifdef PRE_MOD_PETAL_WRITE
		fixedPrice = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2030000 ), DN_INTERFACE::UTIL::GetAddCommaString( data.priceFix ).c_str() );	// UITSRING : %s ¡æ
#else // PRE_MOD_PETAL_WRITE
		fixedPrice = FormatW(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4733 ), data.priceFix);	// UITSRING : %d ¡æ
#endif // PRE_MOD_PETAL_WRITE
		pItemPrice->AppendText(fixedPrice.c_str(), D3DCOLOR_ARGB(255,189,44,58));

		std::wstring discountString;
		discountString = FormatW(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4718), GetCashShopTask().GetCashShopDiscountRate((float)data.price, (float)data.priceFix, (float)data.ReserveAmount)); // UISTRING : %d%% ÇÒÀÎ
		pItemDiscountRate->SetText(discountString.c_str());
	}
#endif // PRE_ADD_LIMITED_CASHITEM

#ifdef PRE_ADD_PETALSHOP
#ifdef PRE_MOD_PETAL_WRITE
	int nUIStringIndex = 2030002;	// UISTRING : %sÆäÅ»
#else // PRE_MOD_PETAL_WRITE
	int nUIStringIndex = 4647;	// UISTRING : %dÆäÅ»
#endif // PRE_MOD_PETAL_WRITE
#else // PRE_ADD_PETALSHOP
#ifdef PRE_MOD_PETAL_WRITE
	int nUIStringIndex = 2030001;	// UISTRING : %sÄ³½Ã
#else // PRE_MOD_PETAL_WRITE
	int nUIStringIndex = 4642;	// UISTRING : %dÄ³½Ã
#endif // PRE_MOD_PETAL_WRITE
#endif // PRE_ADD_PETALSHOP

#ifdef PRE_ADD_NEW_MONEY_SEED
	DWORD dwMoneyColor = textcolor::MONEY_CASH;
#ifdef PRE_MOD_PETAL_WRITE
	nUIStringIndex = 2030001;
	if( !data.bCashUsable )
	{
		if( ( data.bReserveUsable && data.bSeedUsable ) || ( data.bReserveUsable && !data.bSeedUsable ) )
		{
			nUIStringIndex = 2030002;
			dwMoneyColor = textcolor::MONEY_RESERVE;
		}
		else if( !data.bReserveUsable && data.bSeedUsable )
		{
			nUIStringIndex = 4980;
			dwMoneyColor = textcolor::MONEY_SEED;
		}
	}
#else // PRE_MOD_PETAL_WRITE
	nUIStringIndex = 4642;
	if( !data.bCashUsable )
	{
		if( ( data.bReserveUsable && data.bSeedUsable ) || ( data.bReserveUsable && !data.bSeedUsable ) )
		{
			nUIStringIndex = 4647;
			dwMoneyColor = textcolor::MONEY_RESERVE;
		}
		else if( !data.bReserveUsable && data.bSeedUsable )
		{
			nUIStringIndex = 4968;
			dwMoneyColor = textcolor::MONEY_SEED;
		}
	}
#endif // PRE_MOD_PETAL_WRITE
#endif // PRE_ADD_NEW_MONEY_SEED

#ifdef PRE_MOD_PETAL_WRITE
	str = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nUIStringIndex ), DN_INTERFACE::UTIL::GetAddCommaString( data.price ).c_str() );
#else // PRE_MOD_PETAL_WRITE
	str = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nUIStringIndex ), data.price );
#endif // PRE_MOD_PETAL_WRITE
	
#ifdef PRE_ADD_NEW_MONEY_SEED
	pItemPrice->AppendText( str.c_str(), dwMoneyColor );
#else // PRE_ADD_NEW_MONEY_SEED
	pItemPrice->AppendText(str.c_str(), D3DCOLOR_ARGB(255,239,152,49));
#endif // PRE_ADD_NEW_MONEY_SEED

#ifdef PRE_ADD_PETALSHOP
	pGiftBtn->Enable(false);
#else
#ifdef PRE_ADD_NEW_MONEY_SEED
	if( data.bCashUsable )
		pGiftBtn->Enable( data.bGiftUsable );
	else
		pGiftBtn->Enable( false );
#else // PRE_ADD_NEW_MONEY_SEED
	pGiftBtn->Enable( data.bGiftUsable );
#endif // PRE_ADD_NEW_MONEY_SEED
#endif

//	pWishBtn->Enable(false);

#ifdef PRE_ADD_CASHSHOP_RENEWAL	
	int size = (int)vItemState.size();
	for( int i=0; i<size; ++ i )
		vItemState[ i ]->Show( false );	
#else
	int i = ICON_MIN;
	for (; i < MAXICON; ++i)
		pTypeIcon[i]->Show(false);
#endif // PRE_ADD_CASHSHOP_RENEWAL

#ifdef PRE_ADD_LIMITED_CASHITEM
	if( data.bLimit && GetCashShopTask().GetCashLimitItemMaxCount( data.sn ) > 0 && GetCashShopTask().GetCashLimitItemRemainCount( data.sn ) == 0 )
	{
		SetCloseIcon();

		pBuyBtn->Enable( false );
		pGiftBtn->Enable( false );
		pCartBtn->Enable( false );
#ifdef PRE_MOD_SALE_COUPON_LEVEL_LIMIT
		pBuyBtn->SetTooltipText( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 9171) );	// UISTRING : ÆÇ¸Å ±â°£ÀÌ Á¾·áµÇ¾ú½À´Ï´Ù.
#endif // PRE_MOD_SALE_COUPON_LEVEL_LIMIT
	}
	else
	{
#ifdef PRE_ADD_CASHSHOP_RENEWAL
		//ShowIcon( data.arrState );		
		int size = (int)data.arrState.size();
		for( int k=0; k<size; ++k )
		{
			EtTextureHandle texture = vStateTexture[ data.arrState[ k ] ];			
			if( texture ) 
				texture->AddRef();
			vItemState[ k ]->GetTemplate().m_hTemplateTexture = texture;			
			vItemState[ k ]->Show( true );
		}
#else
		ShowIcon((eGoodsIconType)data.state);
#endif // PRE_ADD_CASHSHOP_RENEWAL

	}
#else // PRE_ADD_LIMITED_CASHITEM
	#ifdef PRE_ADD_CASHSHOP_RENEWAL
	//ShowIcon( data.arrState );		
	size = (int)data.arrState.size();
	for( int k=0; k<size; ++k )
	{
		const int& state = data.arrState[ k ];
		EtTextureHandle texture = vStateTexture[ state ];			
		if( texture ) 
			texture->AddRef();
		vItemState[ k ]->GetTemplate().m_hTemplateTexture = texture;
		vItemState[ k ]->Show( true );
	}
	#else
	ShowIcon((eGoodsIconType)data.state);
	#endif // PRE_ADD_CASHSHOP_RENEWAL
#endif // PRE_ADD_LIMITED_CASHITEM
}

#ifdef PRE_ADD_CASHSHOP_RENEWAL
	CDnCashShopGoodsDlg::CDnCashShopGoodsDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback, bool bMainTab )
#else
CDnCashShopGoodsDlg::CDnCashShopGoodsDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback)
#endif // PRE_ADD_CASHSHOP_RENEWAL
				: CDnCustomDlg(dialogType, pParentDialog, nID, pCallback), m_Type((eCashShopSubCatType)nID)
{
	m_MaxPage		= 0;
	m_CurrentPage	= 1;
	m_SelectedIndex	= 0;

	m_pCommonDlgMgr	= NULL;
	m_bWishList		= false;

	m_pPageGroupPrev = NULL;
	m_pPageGroupNext = NULL;
	m_pPagePrev = NULL;
	m_pPageNext = NULL;
	memset( m_pPageBtn, 0, sizeof( m_pPageBtn ) );

	//m_pJobSortBG		= NULL;
	m_pJobSortComboBox	= NULL;
	//m_pEtcSortBG		= NULL;
	m_pEtcSortComboBox	= NULL;
#ifdef PRE_ADD_SALE_COUPON
	m_nCategory			= 0;
#endif // PRE_ADD_SALE_COUPON
#ifdef PRE_ADD_CASHSHOP_JPN_REVISEDLAW
	m_pNoticeBrowserBtn = NULL;
#endif

#ifdef PRE_ADD_CASHSHOP_RENEWAL
	m_crrIdx = 0;
	m_bMainTab = bMainTab;
	m_SortJob = 0; // Á¤·Ä.	
	m_BannerRollCnt = 0;
	m_time = 0.0f;
	m_bForceRolling = false;
#endif // PRE_ADD_CASHSHOP_RENEWAL

}

CDnCashShopGoodsDlg::~CDnCashShopGoodsDlg(void)
{

	// test.
	OnExceptionalCloseChargeBrowser(true);

#ifdef PRE_ADD_CASHSHOP_RENEWAL

	if( !CDnCashShopGoodsDlg::SCashShopSlotUnit::vStateTexture.empty() )
		CDnCashShopGoodsDlg::SCashShopSlotUnit::vStateTexture.clear();

	// »óÇ°»óÅÂ ÅØ½ºÃÄ.	
	if( m_bMainTab )
	{		
		for( int i=0; i<g_strControlSize; ++i )
			ReleaseHideTexture( g_strControlName[ i ] );
	}
	

	// Banner Texture.
	if( m_bMainTab )
	{
		int size = (int)m_vTexture.size();
		for( int i=0; i<size; ++i )
		{
			EtTextureHandle texture = m_vTexture[i];
			if( texture )
				texture->Release();
		}
		m_vTexture.clear();

		m_bannerRadioBtn.clear();
	}

	

#endif // PRE_ADD_CASHSHOP_RENEWAL

}

#ifdef PRE_ADD_SALE_COUPON
void CDnCashShopGoodsDlg::Initialize( bool bShow, CDnCashShopCommonDlgMgr* pCommonDlgMgr, bool bWishList, bool bDisableJobSort, int nCategory )
#else // PRE_ADD_SALE_COUPON
void CDnCashShopGoodsDlg::Initialize( bool bShow, CDnCashShopCommonDlgMgr* pCommonDlgMgr, bool bWishList, bool bDisableJobSort )
#endif // PRE_ADD_SALE_COUPON
{
	if (pCommonDlgMgr == NULL)
	{
		_ASSERT(0);
		return;
	}

#ifdef PRE_ADD_SALE_COUPON
	m_nCategory = nCategory;
#endif // PRE_ADD_SALE_COUPON
	m_pCommonDlgMgr = pCommonDlgMgr;
	m_bWishList		= bWishList;
	m_bDisableJobSort = bDisableJobSort;

	std::string strUI;

#ifdef PRE_ADD_CASHSHOP_RENEWAL
	#ifdef PRE_ADD_CASHSHOP_REFUND_CL
	//strUI = CDnCashShopTask::GetInstance().IsCashShopMode(eCSMODE_REFUND) ? "CS_Commodity_refund.ui" : ( m_bMainTab==true ? "CS_CommodityMain_refund.ui" : "CS_CommodityDlg.ui" );
		if( CDnCashShopTask::GetInstance().IsCashShopMode(eCSMODE_REFUND) )
			strUI = ( m_bMainTab==true ? "CS_CommodityMain_refund.ui" : "CS_Commodity_refund.ui" );
		else
			strUI = ( m_bMainTab==true ? "CS_CommodityMainDlg.ui" : "CS_CommodityDlg.ui" );
	#else
		if( m_bMainTab )
			strUI = "CS_CommodityMainDlg.ui";
		else
			strUI = "CS_CommodityDlg.ui";
	#endif // PRE_ADD_CASHSHOP_REFUND_CL
#else
	#ifdef PRE_ADD_CASHSHOP_REFUND_CL
		strUI = CDnCashShopTask::GetInstance().IsCashShopMode(eCSMODE_REFUND) ? "CSCommodity_Refund.ui" : "CSCommodity.ui";
	#else
		strUI = "CSCommodity.ui";
	#endif // PRE_ADD_CASHSHOP_REFUND_CL
#endif // PRE_ADD_CASHSHOP_RENEWAL

	CEtUIDialog::Initialize(CEtResourceMng::GetInstance().GetFullName(strUI.c_str()).c_str(), bShow);

//#ifdef PRE_ADD_CASHSHOP_REFUND_CL
//	//std::string dlgName = CDnCashShopTask::GetInstance().IsCashShopMode(eCSMODE_REFUND) ? "CSCommodity_Refund.ui" : "CSCommodity.ui";
//	std::string dlgName = CDnCashShopTask::GetInstance().IsCashShopMode(eCSMODE_REFUND) ? "CS_Commodity_refund.ui" : "CS_CommodityDlg.ui";
//	CEtUIDialog::Initialize(CEtResourceMng::GetInstance().GetFullName(dlgName.c_str()).c_str(), bShow);
//#else
//	//CEtUIDialog::Initialize(CEtResourceMng::GetInstance().GetFullName( "CSCommodity.ui" ).c_str(), bShow);
//	CEtUIDialog::Initialize(CEtResourceMng::GetInstance().GetFullName( "CS_CommodityDlg.ui" ).c_str(), bShow);
//#endif

	SetFadeMode(CEtUIDialog::None);
}

void CDnCashShopGoodsDlg::InitialUpdate()
{
	char szControlName[32]={0};	
	char strState[64]={0};
	for( int i=0; i<(int)m_SlotUnits.size(); i++)
	{
		sprintf_s(szControlName, 32, "ID_BUTTON_BASE%d", i);
		m_SlotUnits[i].pBase = GetControl<CEtUIStatic>(szControlName);

		sprintf_s(szControlName, 32, "ID_STATIC_NAME%d", i);
		m_SlotUnits[i].pItemName = GetControl<CEtUIStatic>(szControlName);

		sprintf_s(szControlName, 32, "ID_STATIC_COUNT%d", i);
		m_SlotUnits[i].pItemCount = GetControl<CEtUIStatic>(szControlName);

		sprintf_s(szControlName, 32, "ID_STATIC_PRICE%d", i);
		m_SlotUnits[i].pItemPrice = GetControl<CEtUITextBox>(szControlName);

		sprintf_s(szControlName, 32, "ID_STATIC_DSCRATE%d", i);
		m_SlotUnits[i].pItemDiscountRate = GetControl<CEtUIStatic>(szControlName);

		sprintf_s(szControlName, 32, "ID_STATIC_RESERVE%d", i);
		m_SlotUnits[i].pItemReserve = GetControl<CEtUIStatic>(szControlName);

		sprintf_s(szControlName, 32, "ID_STATIC_PERIOD%d", i);
		m_SlotUnits[i].pItemPeriod = GetControl<CEtUIStatic>(szControlName);

		sprintf_s(szControlName, 32, "ID_BUTTON_BUY%d", i);
		m_SlotUnits[i].pBuyBtn = GetControl<CEtUIButton>(szControlName);

		sprintf_s(szControlName, 32, "ID_BUTTON_PRESENT%d", i);
		m_SlotUnits[i].pGiftBtn = GetControl<CEtUIButton>(szControlName);

		sprintf_s(szControlName, 32, "ID_BUTTON_CART%d", i);
		m_SlotUnits[i].pCartBtn = GetControl<CEtUIButton>(szControlName);

//		sprintf_s(szControlName, 32, "ID_BUTTON_WISH%d", i);
//		m_SlotUnits[i].pWishBtn = GetControl<CEtUIButton>(szControlName);

		sprintf_s(szControlName, 32, "ID_STATIC_SLOTBG%d", i);
		m_SlotUnits[i].pQuickSlotBG = GetControl<CEtUIStatic>(szControlName);

		sprintf_s(szControlName, 32, "ID_BUTTON_DEL%d", i);
		m_SlotUnits[i].pDeleteBtn = GetControl<CEtUIButton>(szControlName);

#ifdef PRE_ADD_CASHSHOP_RENEWAL
		for( int k=0; k<4; ++k )
		{
			sprintf_s( strState, 64, "ID_CASH_STATE%d%d", i, k );
			CEtUIStatic * _pStatic = GetControl<CEtUIStatic>( strState );
			_pStatic->Show( true );
			m_SlotUnits[ i ].vItemState.push_back( _pStatic );
			memset( strState, 0, 64 );
		}

#else
		sprintf_s(szControlName, 32, "ID_CASH_NEW%d", i);
		m_SlotUnits[i].pTypeIcon[ICON_NEW] = GetControl<CEtUIStatic>(szControlName);

		sprintf_s(szControlName, 32, "ID_CASH_HOT%d", i);
		m_SlotUnits[i].pTypeIcon[ICON_HOT] = GetControl<CEtUIStatic>(szControlName);

		sprintf_s(szControlName, 32, "ID_CASH_DISCOUNT%d", i);
		m_SlotUnits[i].pTypeIcon[ICON_DISCOUNT] = GetControl<CEtUIStatic>(szControlName);

		sprintf_s(szControlName, 32, "ID_CASH_EVENT%d", i);
		m_SlotUnits[i].pTypeIcon[ICON_EVENT] = GetControl<CEtUIStatic>(szControlName);

		sprintf_s(szControlName, 32, "ID_CASH_LIMITED%d", i);
		m_SlotUnits[i].pTypeIcon[ICON_LIMITED] = GetControl<CEtUIStatic>(szControlName);

#ifdef PRE_ADD_SALE_COUPON
		sprintf_s(szControlName, 32, "ID_CASH_COUPON%d", i);
		m_SlotUnits[i].pTypeIcon[ICON_COUPON] = GetControl<CEtUIStatic>(szControlName);
#endif // PRE_ADD_SALE_COUPON

#ifdef PRE_ADD_LIMITED_CASHITEM
		sprintf_s(szControlName, 32, "ID_CASH_LIMITEDITEM%d", i);
		m_SlotUnits[i].pTypeIcon[ICON_COUNTLIMIT] = GetControl<CEtUIStatic>(szControlName);
		sprintf_s(szControlName, 32, "ID_CASH_LIMITEDCLOSE%d", i);
		m_SlotUnits[i].pTypeIcon[ICON_COUNTLIMITCLOSE] = GetControl<CEtUIStatic>(szControlName);
#endif // PRE_ADD_LIMITED_CASHITEM

#endif // PRE_ADD_CASHSHOP_RENEWAL

		m_SlotUnits[i].Show(false, false);
	}
//#endif // PRE_ADD_CASHSHOP_RENEWAL


	m_pPagePrev = GetControl<CEtUIButton>("ID_BUTTON_PGUP");
	m_pPageNext = GetControl<CEtUIButton>("ID_BUTTON_PGDN");
	m_pPagePrev->Enable(false);
	m_pPageNext->Enable(false);

	m_pPageGroupPrev = GetControl<CEtUIButton>("ID_BUTTON_WPGUP");
	m_pPageGroupNext = GetControl<CEtUIButton>("ID_BUTTON_WPGDN");
	m_pPageGroupPrev->Enable(false);
	m_pPageGroupNext->Enable(false);

	//m_pJobSortBG = GetControl<CEtUIStatic>("ID_STATIC10");
	m_pJobSortComboBox = GetControl<CEtUIComboBox>("ID_COMBOBOX_JOBSORT");
	//m_pEtcSortBG = GetControl<CEtUIStatic>("ID_STATIC11");
	m_pEtcSortComboBox = GetControl<CEtUIComboBox>("ID_COMBOBOX_ETCSORT");
#ifdef PRE_ADD_CASHSHOP_JPN_REVISEDLAW
	m_pNoticeBrowserBtn = GetControl<CEtUIButton>("ID_BT_NOTICE");
#endif

	int i = 0;
	std::wstring str;
#ifdef PRE_ADD_ACADEMIC
	int nMaxClass = CommonUtil::eCLASS_Max;

#if defined(PRE_ADD_KALI) && defined(PRE_REMOVE_KALI)
	nMaxClass = CommonUtil::eCLASS__KALI;
#endif

#ifdef PRE_REMOVE_ACADEMIC
	nMaxClass = CommonUtil::eCLASS_ACADEMIC;
#endif // PRE_REMOVE_ACADEMIC

	for (; i < nMaxClass; ++i)
	{
		str = (i == CommonUtil::eCLASS_NONE) ? GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4772) : DN_INTERFACE::STRING::GetClassString(i);	// UISTRING : ¸ðµç Á÷¾÷
		m_pJobSortComboBox->AddItem(str.c_str(), NULL, i);
	}
#else
	for (; i < CASHCLASS_COUNT + 1; ++i)
	{
		str = (i == CASHCLASS_NONE) ? GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4772) : DN_INTERFACE::STRING::GetClassString(i);	// UISTRING : ¸ðµç Á÷¾÷
		m_pJobSortComboBox->AddItem(str.c_str(), NULL, i);
	}
#endif

	if (m_bDisableJobSort)
		m_pJobSortComboBox->Enable(false);

	for (i = 0; i < SORT_MAX; ++i)
	{
		if (i == SORT_MYCLASS)
			continue;

		m_pEtcSortComboBox->AddItem(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, GetSortStringNum((eCashShopEtcSortType)i)), NULL, i, true); // #74223 [EU] UIStringÀÌ ±æ¾î 2ÁÙ·Î Ç¥½ÃµÇ´Â ¹®Á¦.
	}

	std::string forPageBtnID;
	for (i = 0; i < _MAX_PAGE_COUNT_PER_GROUP; ++i)
	{
		forPageBtnID = FormatA("ID_BUTTON_PAGE%d", i);
		m_pPageBtn[i] = GetControl<CEtUIButton>(forPageBtnID.c_str());
	}


#ifdef PRE_ADD_CASHSHOP_RENEWAL

	// Banner //
	if( m_bMainTab )
	{
		m_pTextureCtr = GetControl<CEtUITextureControl>( "ID_TEXTUREL_BANNER" );

		TCHAR pBuffer[MAX_PATH]={0};
		TCHAR pImagePath[MAX_PATH]={0};

		SHGetSpecialFolderPath(CDnMainFrame::GetInstance().GetHWnd(), pBuffer, CSIDL_PERSONAL, 0);
		wsprintf( pBuffer, L"%s\\DragonNest", pBuffer );

		// URL.	
		std::string strURLs;
		DNTableFileFormat * pTable = GetDNTable(CDnTableDB::TCASHTAB);
		if( pTable )
		{
			DNTableCell * pCell = pTable->GetFieldFromLablePtr( 1, "_MainBannerURL");
			if( pCell )
				strURLs.assign( pCell->GetString() );
		}

		//// Test¿ë.
		//if( strURLs.empty() )	
		//	strURLs.assign( "http://dn.image.happyoz.com/Img/2013/01/25/0c850d0e5dedcdc54908ed36774082d33.jpg;http://dn.image.happyoz.com/Img/2013/01/09/5d6836f4f73f37afecf44e07f9e9d133.jpg;http://dn.image.happyoz.com/Img/2013/01/27/19ada7f82dc5d40977282f7bdd7f80c2.jpg" );

		std::vector< std::string > arrURL;
		TokenizeA( strURLs, arrURL, std::string(";") );

		HRESULT hr;
		EtTextureHandle hTexture;

		int size = (int)arrURL.size();
		for( int i=0; i<size; ++i )
		{
			//ÀÌ¹ÌÁö °æ·Î ¼³Á¤
			char BANNERIMAGE_FILE_NAME[1024] = "";
			wsprintf( pImagePath, L"%s\\WebBanner_%d.jpg", pBuffer, i );
			WideCharToMultiByte( CP_ACP, 0, pImagePath, -1, BANNERIMAGE_FILE_NAME, sizeof(BANNERIMAGE_FILE_NAME), NULL, NULL );

			std::string & url = arrURL[ i ];
			DeleteUrlCacheEntryA( url.c_str() );

			// Download.
			hr = URLDownloadToFileA( NULL, url.c_str(), BANNERIMAGE_FILE_NAME , 0, NULL  );

			if( hr == S_OK )
			{
				CFileStream Stream( BANNERIMAGE_FILE_NAME );
				hTexture = (new CEtTexture)->GetMySmartPtr();
				hTexture->LoadResource( &Stream );			

				DeleteFileA( BANNERIMAGE_FILE_NAME );

				m_vTexture.push_back( hTexture );
			}
		}

		m_BannerRollCnt = (int)m_vTexture.size();
		if( m_BannerRollCnt > g_BannerRolling )
			m_BannerRollCnt = g_BannerRolling;

		char strTemp[32] = {0,};
		for( int i=0; i<g_BannerRolling; ++i )
		{
			sprintf_s( strTemp, 32, "ID_RBT_BANNER%d", i );
			CEtUIRadioButton * pRadio = GetControl<CEtUIRadioButton>(strTemp);
			m_bannerRadioBtn.push_back( pRadio );
			if( i < m_BannerRollCnt )
				pRadio->Show( true );
			else
				pRadio->Show( false );
		}
	}

	

	// »óÇ°»óÅÂ ÅØ½ºÃÄ.	
	for( int i=0; i<g_strControlSize; ++i )
		SetHideControl( g_strControlName[ i ] );

#endif // PRE_ADD_CASHSHOP_RENEWAL


}

// ÇØ´ç StaticÄÁÆ®·ÑÀÇ À§Ä¡¸¦ º¯°æ½ÃÄÑ º¸ÀÌÁö¾Ê°ÔÇÑ´Ù.
// - ÀÌÀ¯ : Show(false) ·Î ¼û±æ°æ¿ì ÅÛÇÃ¸´ÅØ½ºÃÄ¸¦ Release½ÃÅ°±â ¶§¹®¿¡ ÀÌ¸¦ ¹æÁöÇÏ±âÀ§ÇØ¼­ÀÓ.
#ifdef PRE_ADD_CASHSHOP_RENEWAL
void CDnCashShopGoodsDlg::SetHideControl( char * strName )
{
	CEtUIStatic * pStatic = GetControl<CEtUIStatic>( strName );
	if( pStatic )
	{
		pStatic->Show( true );
		pStatic->SetPosition( 0.0f, -0.5f );

		if( m_bMainTab )
		{
			//[debug1] ÕâÀïµ¼ÖÂÉÌ³Ç±ÀÀ££¬ÔÝÊ±×¢ÊÍµô

#if 1
			pStatic->GetTemplate().m_hTemplateTexture->AddRef();
			SCashShopSlotUnit::vStateTexture.push_back( pStatic->GetTemplate().m_hTemplateTexture );
#else
			//pStatic->GetTemplate().m_hTemplateTexture->AddRef();
			//SCashShopSlotUnit::vStateTexture.push_back( pStatic->GetTemplate().m_hTemplateTexture );
#endif

		}
	}
}

void CDnCashShopGoodsDlg::ReleaseHideTexture( char * strName )
{
	CEtUIStatic * pStatic = GetControl<CEtUIStatic>( strName );
	if( pStatic )
	{
		EtTextureHandle texture = pStatic->GetTemplate().m_hTemplateTexture;
		if( texture )
			texture->Release();	
	}
}

#endif // PRE_ADD_CASHSHOP_RENEWAL

int CDnCashShopGoodsDlg::GetSortStringNum(eCashShopEtcSortType type) const
{
	switch(type)
	{
	case SORT_BASIC:				return 4637;
	case SORT_LEVEL_DESCENDING:		return 4632;
	case SORT_LEVEL_ASCENDING:		return 4633;
	case SORT_NAME_DESCENDING:		return 4634;
	case SORT_NAME_ASCENDING:		return 4635;
	//case SORT_RELEASE_NEW:			return 4636;
	}

	return -1;
}

void CDnCashShopGoodsDlg::InitCustomControl(CEtUIControl *pControl)
{
	if (pControl == NULL || !strstr(pControl->GetControlName(), "ID_BUTTON_ITEM"))
		return;

	CDnQuickSlotButton *pQuickSlotBtn(NULL);
	pQuickSlotBtn = static_cast<CDnQuickSlotButton*>(pControl);

	pQuickSlotBtn->SetSlotType(ST_INVENTORY_CASHSHOP);
	pQuickSlotBtn->SetSlotIndex((int)m_SlotUnits.size());

	SCashShopSlotUnit slotUnit;
	slotUnit.pQuickSlotBtn = pQuickSlotBtn;
	m_SlotUnits.push_back(slotUnit);
}

void CDnCashShopGoodsDlg::PrevPage(bool bCircle)
{
	const CS_INFO_LIST* pDataList = GetDataList(m_Type);
	if (pDataList == NULL)
	{
		_ASSERT(0);
		return;
	}
	const CS_INFO_LIST& dataList = *pDataList;

	if (dataList.empty())
		return;

	m_CurrentPage--;
	if (bCircle)
	{
		if (m_CurrentPage <= 0)
			m_CurrentPage = m_MaxPage;
	}
	else
	{
		CommonUtil::ClipNumber(m_CurrentPage, 1, m_MaxPage);
	}

	//	Set Page Static
	UpdatePageControllers();

	//	Fill Slots
	UpdateSlots(pDataList, true);
}

void CDnCashShopGoodsDlg::NextPage(bool bCircle)
{
	const CS_INFO_LIST* pDataList = GetDataList(m_Type);
	if (pDataList == NULL)
	{
		_ASSERT(0);
		return;
	}
	const CS_INFO_LIST& dataList = *pDataList;

	if (dataList.empty())
		return;

	m_CurrentPage++;
	if (bCircle)
	{
		if (m_CurrentPage > m_MaxPage)
			m_CurrentPage = 1;
	}
	else
	{
		CommonUtil::ClipNumber(m_CurrentPage, 1, m_MaxPage);
	}

	//	Set Page Static
	UpdatePageControllers();

	//	Fill Slots
	UpdateSlots(pDataList, true);
}

void CDnCashShopGoodsDlg::NextGroupPage()
{
	int curPageGroupNum		= (m_CurrentPage % _MAX_PAGE_COUNT_PER_GROUP) ? m_CurrentPage / _MAX_PAGE_COUNT_PER_GROUP + 1 : m_CurrentPage / _MAX_PAGE_COUNT_PER_GROUP;
	int nextPageStartNum	= (curPageGroupNum * _MAX_PAGE_COUNT_PER_GROUP) + 1;
	
	if (nextPageStartNum > m_MaxPage)
		return;

	m_CurrentPage = nextPageStartNum;

	const CS_INFO_LIST* pDataList = GetDataList(m_Type);
	if (pDataList == NULL)
	{
		_ASSERT(0);
		return;
	}
	UpdatePages(pDataList, true);
}

void CDnCashShopGoodsDlg::PrevGroupPage()
{
	int curPageGroupNum		= (m_CurrentPage % _MAX_PAGE_COUNT_PER_GROUP) ? m_CurrentPage / _MAX_PAGE_COUNT_PER_GROUP + 1 : m_CurrentPage / _MAX_PAGE_COUNT_PER_GROUP;
	int prevPageStartNum	= ((curPageGroupNum - 2) * _MAX_PAGE_COUNT_PER_GROUP) + 1;

	if (prevPageStartNum <= 0)
		return;

	m_CurrentPage = prevPageStartNum;

	const CS_INFO_LIST* pDataList = GetDataList(m_Type);
	if (pDataList == NULL)
	{
		_ASSERT(0);
		return;
	}
	UpdatePages(pDataList, true);
}

const SCashShopItemInfo* CDnCashShopGoodsDlg::GetMatchListData(int index) const
{
	if (m_bWishList)
	{
		std::map<int, int>::const_iterator iter = m_WishListMatchList.find(index);
		const CASHITEM_SN& sn = (*iter).second;
		return GetCashShopTask().GetItemInfo(sn);
	}
	else
	{
		std::map<int, int>::const_iterator iter = m_DataSlotIdxMatchList.find(index);
		const int& dataIdx = (*iter).second;
		return GetData(m_Type, dataIdx);
	}

	return NULL;
}

void CDnCashShopGoodsDlg::ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg)
{
	SetCmdControlName( pControl->GetControlName() );

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

		if (IsCmdControl("ID_BUTTON_WPGUP"))
		{
			PrevGroupPage();
			return;
		}

		if (IsCmdControl("ID_BUTTON_WPGDN"))
		{
			NextGroupPage();
			return;
		}

#ifdef PRE_ADD_CASHSHOP_JPN_REVISEDLAW
		if (IsCmdControl("ID_BT_NOTICE"))
		{
			std::string url = "http://dragonnest.hangame.co.jp/spann/";
			GetInterface().OpenBrowser(url, (float)CEtDevice::GetInstance().Width(), (float)CEtDevice::GetInstance().Height(), CDnInterface::eBPT_CENTER, eGBT_SIMPLE);
			return;
		}
#endif

		if (strstr(pControl->GetControlName(), "ID_BUTTON_PAGE"))
		{
			const std::wstring& text = pControl->GetText();
			if (text.empty() == false)
			{
				m_CurrentPage = _wtoi(text.c_str());
				const CS_INFO_LIST* pDataList = GetDataList(m_Type);
				if (pDataList == NULL)
				{
					_ASSERT(0);
					return;
				}

				UpdatePages(pDataList, true);
				return;
			}
		}

		const std::string& cmdName = GetCmdControlName();
		std::string::size_type numberPos = cmdName.find("ID_BUTTON_BUY");
		if (numberPos != std::string::npos)
		{
			size_t pieceLen = strlen("ID_BUTTON_BUY");
			if (pieceLen < cmdName.size())
			{
				int slotIdx = atoi(&(cmdName[pieceLen]));
				const SCashShopItemInfo* pData = GetMatchListData(slotIdx);
				if (pData)
				{
					SCashShopCartItemInfo info;
					info.presentSN	= pData->sn;

					if (GetCashShopTask().PutItemIntoBuyItemNow(pData->sn))
					{
						if (CDnCashShopTask::GetInstance().IsCashShopMode(eCSMODE_WARN_PACKAGEWITHINVEN) && 
							CDnCashShopTask::GetInstance().HandleWarning(info.presentSN, BUY_INVEN_STORAGE_BUYITEMNOW_DLG, this) == false)
							return;

						CART_ITEM_LIST tempList;
						tempList.push_back(GetCashShopTask().GetBuyItemNow());

						if (CDnActor::s_hLocalActor)
						{
							int localActorLevel = CDnActor::s_hLocalActor->GetLevel();
							if (GetCashShopTask().HandleItemLevelLimit(tempList, localActorLevel, LEVEL_ALERT_BUYITEMNOW_DLG, this) == false)
								return;
						}
						else
						{
							return;
						}
						if( GetCashShopTask().HandleItemOverlapBuy(tempList, eCashUnit_BuyItemNow, OVERLAP_ALERT_BUYITEMNOW_DLG, this) == false )
							return;

						eCashUnitType type = (GetCashShopTask().IsPackageItem(pData->sn)) ? eCashUnit_Package : eCashUnit_BuyItemNow;
						m_pCommonDlgMgr->OpenPayDlg(type, true);
					}
				}
			}
		}

		numberPos = cmdName.find("ID_BUTTON_PRESENT");
		if (numberPos != std::string::npos)
		{
			size_t pieceLen = strlen("ID_BUTTON_PRESENT");
			if (pieceLen < cmdName.size())
			{
				int slotIdx = atoi(&(cmdName[pieceLen]));
				const SCashShopItemInfo* pData = GetMatchListData(slotIdx);
				if (pData)
				{
					SCashShopCartItemInfo info;
					info.presentSN = pData->sn;

					if (GetCashShopTask().PutItemIntoGiftItemNow(pData->sn))
					{
						eCashUnitType type = (GetCashShopTask().IsPackageItem(pData->sn)) ? eCashUnit_Gift_Package : eCashUnit_GiftItemNow;
						m_pCommonDlgMgr->OpenSendGiftDlg(type, true);
					}
				}
			}
		}

		numberPos = cmdName.find("ID_BUTTON_CART");
		if (numberPos != std::string::npos)
		{
			size_t pieceLen = strlen("ID_BUTTON_CART");
			if (pieceLen < cmdName.size())
			{
				int slotIdx = atoi(&(cmdName[pieceLen]));
				const SCashShopItemInfo* pData = GetMatchListData(slotIdx);

				if (pData)
				{
					if( GetCashShopTask().PutItemIntoCartItemNow(pData->sn) )
					{
						CART_ITEM_LIST tempList;
						tempList.push_back(GetCashShopTask().GetBuyItemNow());

						if (CDnActor::s_hLocalActor)
						{
							int localActorLevel = CDnActor::s_hLocalActor->GetLevel();
							if (GetCashShopTask().HandleItemLevelLimit(tempList, localActorLevel, LEVEL_ALERT_CARTITEMNOW_DLG, this) == false)
								return;
						}
						else
						{
							return;
						}
						if( GetCashShopTask().HandleItemOverlapBuy(tempList, eCashUnit_Cart, OVERLAP_ALERT_CARTITEMNOW_DLG, this) == false )
							return;

						if (pData)
							GetCashShopTask().PutItemIntoCart(pData->sn);
					}
				}
			}
		}

		numberPos = cmdName.find("ID_BUTTON_DEL");
		if (numberPos != std::string::npos)
		{
			size_t pieceLen = strlen("ID_BUTTON_DEL");
			if (m_bWishList && pieceLen < cmdName.size())
			{
				int slotIdx = atoi(&(cmdName[pieceLen]));
				std::map<int, int>::const_iterator iter = m_WishListMatchList.find(slotIdx);
				const CASHITEM_SN& sn = (*iter).second;
				GetCashShopTask().DeleteWishListItem(sn);
				Update();
			}
		}
/*
		numberPos = cmdName.find("ID_BUTTON_WISH");
		if (numberPos != std::string::npos)
		{
			size_t pieceLen = strlen("ID_BUTTON_WISH");
			if (pieceLen < cmdName.size())
			{
				int slotIdx = atoi(&(cmdName[pieceLen]));
				std::map<int, int>::const_iterator iter = m_DataSlotIdxMatchList.find(slotIdx);
				const int& dataIdx = (*iter).second;
				const SCashShopItemInfo* pData = GetData(m_Type, dataIdx);
				if (pData)
					GetCashShopTask().AddWishListItem(pData->sn);
			}
		}
*/
		numberPos = cmdName.find("ID_BUTTON_ITEM");
		if (numberPos != std::string::npos)
		{
			size_t pieceLen = strlen("ID_BUTTON_ITEM");
			if (pieceLen < cmdName.size())
			{
				int slotIdx = atoi(&(cmdName[pieceLen]));

				std::vector<SCashShopSlotUnit>::const_iterator iter = m_SlotUnits.begin();
				for (; iter != m_SlotUnits.end(); ++iter)
				{
					const SCashShopSlotUnit& unit = *iter;
					if (IsCmdControl(unit.pQuickSlotBtn->GetControlName()))
					{
						if (unit.pItem)
						{
							if( unit.pItem->GetItemType() == ITEMTYPE_WEAPON ||
								unit.pItem->GetItemType() == ITEMTYPE_PARTS ||
								unit.pItem->GetItemType() == ITEMTYPE_HAIRDYE ||
								unit.pItem->GetItemType() == ITEMTYPE_EYEDYE ||
								unit.pItem->GetItemType() == ITEMTYPE_SKINDYE ||
								unit.pItem->GetItemType() == ITEMTYPE_HAIRDRESS ||
								unit.pItem->GetItemType() == ITEMTYPE_FACIAL
								|| GetCashShopTask().IsPackageItem(unit.pItem->GetCashItemSN())	)
							{
								GetCashShopTask().AttachPartsToPreview(unit.pItem->GetCashItemSN(), unit.pItem->GetClassID());
							}
							else if (unit.pItem->GetItemType() == ITEMTYPE_VOICEFONT)
							{
								GetInterface().ShowVoiceChatPremiumOptDialog(true, true);
							}
							else if( unit.pItem->GetItemType() == ITEMTYPE_VEHICLEPARTS || unit.pItem->GetItemType() == ITEMTYPE_VEHICLEHAIRCOLOR )
							{
								DNTableFileFormat* pVehicleTable = GetDNTable( CDnTableDB::TVEHICLEPARTS );
								if( pVehicleTable || pVehicleTable->IsExistItem(unit.pItem->GetClassID()) )
								{
									GetCashShopTask().ChangeVehicleParts(0,unit.pItem->GetClassID(),false);
								}
							}
							else if(unit.pItem->GetItemType() == ITEMTYPE_VEHICLE || unit.pItem->GetItemType() == ITEMTYPE_VEHICLE_SHARE)
							{
								DNTableFileFormat* pVehicleTable = GetDNTable( CDnTableDB::TVEHICLE );
								if( pVehicleTable || pVehicleTable->IsExistItem(unit.pItem->GetClassID()) )
								{
									int nVehicleClassID = pVehicleTable->GetFieldFromLablePtr( unit.pItem->GetClassID() , "_VehicleActorID" )->GetInteger();
									GetCashShopTask().ChangePreviewAvatar(nVehicleClassID,true); // °­Á¦·Î Ä³¸¯ÅÍÇü ÀÌ¿ÜÀÇ ¾×ÅÍ¸¦ ÇÁ¸®ºä¿¡ °»½Å½ÃÅ²´Ù.
									GetCashShopTask().ChangeVehicleParts(unit.pItem->GetClassID(),0,true);
								}
							}
							else if( unit.pItem->GetItemType() == ITEMTYPE_PET )
							{
								DNTableFileFormat*  pPetTable = GetDNTable( CDnTableDB::TVEHICLE );
								if( pPetTable && pPetTable->IsExistItem( unit.pItem->GetClassID() ) )
								{
									if( GetInterface().GetCashShopDlg() && GetInterface().GetCashShopDlg()->IsShow() )
									{
										GetCashShopTask().ClearPreviewCart( PET_CLASS_ID );
										if( GetCashShopTask().PutItemIntoPetPreviewCart( unit.pItem->GetCashItemSN(), unit.pItem->GetClassID(), 2 ) )
											GetInterface().GetCashShopDlg()->ChangePetPreviewAvatar( unit.pItem->GetClassID() );
									}
								}
							}
							else if( unit.pItem->GetItemType() == ITEMTYPE_PETPARTS )
							{
								DNTableFileFormat*  pPetPartsTable = GetDNTable( CDnTableDB::TVEHICLEPARTS );
								if( pPetPartsTable && pPetPartsTable->IsExistItem( unit.pItem->GetClassID() ) )
								{
									if( GetInterface().GetCashShopDlg()->IsExistPetAvatar() )
									{
										if( GetInterface().GetCashShopDlg() && GetInterface().GetCashShopDlg()->IsShow() )
										{
											int nPetActorPetClassID = 0;
											int nPartsType = pPetPartsTable->GetFieldFromLablePtr( unit.pItem->GetClassID(), "_VehiclePartsType" )->GetInteger();
											int nPetPartsPetClassID = pPetPartsTable->GetFieldFromLablePtr( unit.pItem->GetClassID(), "_VehicleClassID" )->GetInteger();
											DNTableFileFormat*  pPetTable = GetDNTable( CDnTableDB::TVEHICLE );
											if( pPetTable && GetInterface().GetCashShopDlg()->GetPetPreviewDlg() )
											{
												CDnPetActor* pPetActor = dynamic_cast<CDnPetActor*>( GetInterface().GetCashShopDlg()->GetPetPreviewDlg()->GetPetAvatar() );
												if( pPetActor )
												{
													nPetActorPetClassID = pPetTable->GetFieldFromLablePtr( pPetActor->GetPetInfo().Vehicle[Pet::Slot::Body].nItemID, "_VehicleClassID" )->GetInteger();
												}
											}

											if( nPetPartsPetClassID != nPetActorPetClassID )
											{
												std::vector<int> nVecItemList;
												pPetTable->GetItemIDListFromField( "_VehicleClassID", nPetPartsPetClassID, nVecItemList );
												if( nVecItemList.size() > 0 )
												{
													GetCashShopTask().ClearPreviewCart( PET_CLASS_ID );
													GetInterface().GetCashShopDlg()->ChangePetPreviewAvatar( nVecItemList[0] );
												}
											}

											if( GetCashShopTask().PutItemIntoPetPreviewCart( unit.pItem->GetCashItemSN(), unit.pItem->GetClassID(), nPartsType ) )
												GetInterface().GetCashShopDlg()->ChangePetPreviewParts( nPartsType, unit.pItem->GetClassID() );
										}
									}
									else
									{
										GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4854 ) );
									}
								}
							}
							else if( unit.pItem->GetItemType() == ITEMTYPE_PETCOLOR_BODY || unit.pItem->GetItemType() == ITEMTYPE_PETCOLOR_TATOO )
							{
								DNTableFileFormat*  pPetPartsTable = GetDNTable( CDnTableDB::TVEHICLEPARTS );
								DNTableFileFormat*  pItemTable = GetDNTable( CDnTableDB::TITEM );
								if( pItemTable )
								{
									DWORD dwColor = (DWORD)pItemTable->GetFieldFromLablePtr( unit.pItem->GetClassID(), "_TypeParam1" )->GetInteger();
									if( pPetPartsTable && pPetPartsTable->IsExistItem( unit.pItem->GetClassID() ) )
									{
										if( GetInterface().GetCashShopDlg()->IsExistPetAvatar() )
										{
											if( GetInterface().GetCashShopDlg() && GetInterface().GetCashShopDlg()->IsShow() )
											{
												if( GetCashShopTask().PutItemIntoPetPreviewCart( unit.pItem->GetCashItemSN(), unit.pItem->GetClassID() ) )
													GetInterface().GetCashShopDlg()->ChangePetPreviewColor( unit.pItem->GetItemType(), dwColor );
											}
										}
										else
										{
											GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4854 ) );
										}
									}
								}
							}
							break;
						}
					}
				}
			}
		}
	}
	else if (nCommand == EVENT_RADIOBUTTON_CHANGED)
	{
#ifdef PRE_ADD_CASHSHOP_RENEWAL

		// Banner //		
		if( m_bMainTab && strstr( pControl->GetControlName() , "ID_RBT_BANNER") )
		{
			std::string str( pControl->GetControlName() );
			//std::string::size_type pos = str.find( "ID_RBT_BANNER" );
			str = str.substr( strlen("ID_RBT_BANNER"), str.size() );
			m_crrIdx = ::atoi( str.c_str() );
			m_bForceRolling = true;
		}
		else
		{
			m_CurrentPage = 1;
			const CS_INFO_LIST* pDataList = GetDataList(m_Type);
			if (pDataList == NULL)
			{
				_ASSERT(0);
				return;
			}
			UpdatePages(pDataList, true);
		}
#else // PRE_ADD_CASHSHOP_RENEWAL

		m_CurrentPage = 1;
		const CS_INFO_LIST* pDataList = GetDataList(m_Type);
		if (pDataList == NULL)
		{
			_ASSERT(0);
			return;
		}
		UpdatePages(pDataList, true);
#endif // PRE_ADD_CASHSHOP_RENEWAL

		return;
	}
	else if (nCommand == EVENT_COMBOBOX_SELECTION_CHANGED && bTriggeredByUser)
	{
		if (IsCmdControl("ID_COMBOBOX_JOBSORT"))
		{
			SComboBoxItem* pItem = m_pJobSortComboBox->GetSelectedItem();
			if (pItem)
			{
				Update();
				int classId = 0;
				m_pJobSortComboBox->GetSelectedValue(classId);
				if (classId > 0)
					GetCashShopTask().ChangePreviewAvatar(classId);
				focus::ReleaseControl();
			}
			return;
		}
		
		if (IsCmdControl("ID_COMBOBOX_ETCSORT"))
		{
			SComboBoxItem* pItem = m_pEtcSortComboBox->GetSelectedItem();
			if (pItem)
			{
				if (GetCashShopTask().Sort(m_Type, (eCashShopEtcSortType)pItem->nValue))
					Update();

				focus::ReleaseControl();
				return;
			}
		}
	}

	CDnCustomDlg::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnCashShopGoodsDlg::Process( float fElapsedTime )
{
	if (m_bShow == false)
		return;

#ifdef PRE_ADD_CASHSHOP_RENEWAL
	if( m_bMainTab && !m_vTexture.empty() )
	{
		if( m_time >= 3.0f || m_bForceRolling )
		{			
			m_time = 0.0f;

			if( !m_bForceRolling )
			{
				++m_crrIdx;
				if( m_crrIdx >= (int)m_vTexture.size() )
					m_crrIdx = 0;

				m_bannerRadioBtn[ m_crrIdx ]->SetChecked( true );
			}
			if( m_bForceRolling )
				m_bForceRolling = false;

			m_pTextureCtr->SetTexture( m_vTexture[ m_crrIdx ] );
		}
		m_time += fElapsedTime;
	}
#endif // PRE_ADD_CASHSHOP_RENEWAL

	CDnCustomDlg::Process(fElapsedTime);
}

bool CDnCashShopGoodsDlg::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if( !IsShow() )
		return false;

	switch( uMsg )
	{
	case WM_MOUSEWHEEL:
		{
			if( ( m_pJobSortComboBox && m_pJobSortComboBox->IsOpenedDropDownBox() ) || 
				( m_pEtcSortComboBox && m_pEtcSortComboBox->IsOpenedDropDownBox() ) )
				break;

			if( IsMouseInDlg() )
			{
				UINT uLines;
				SystemParametersInfo( SPI_GETWHEELSCROLLLINES, 0, &uLines, 0 );
				int nScrollAmount = int( ( short )HIWORD( wParam ) ) / WHEEL_DELTA * uLines;
				if( nScrollAmount < 0 )
				{
					NextPage(true);
				}
				else if( nScrollAmount > 0 )
				{
					PrevPage(true);
				}
			}
		}
		break;
	case WM_KEYDOWN:
		{
			if( wParam == VK_PRIOR )
			{
				PrevPage(true);
			}
			else if (wParam == VK_NEXT)
			{
				NextPage(true);
			}
		}
		break;

	case WM_MOUSEMOVE:
		{
			float fMouseX, fMouseY;
			GetScreenMouseMovePoints( fMouseX, fMouseY );
			fMouseX -= GetXCoord();
			fMouseY -= GetYCoord();
			SUICoord uiCoord;

			for( int i=0; i<(int)m_SlotUnits.size(); i++ )
			{
				if( m_SlotUnits[i].pItem == NULL ) 
					continue;
#ifdef PRE_ADD_LIMITED_CASHITEM
				if( m_SlotUnits[i].pItemDiscountRate )
				{
					m_SlotUnits[i].pItemDiscountRate->GetUICoord( uiCoord );
				
					CDnSimpleTooltipDlg* pSimpleTooltipDlg = GetInterface().GetSimpleTooltipDialog();
					if( pSimpleTooltipDlg && uiCoord.IsInside( fMouseX, fMouseY ) )
					{
						if( GetCashShopTask().GetCashLimitItemMaxCount( m_SlotUnits[i].pItem->GetCashItemSN() ) > 0 )
						{
							std::wstring str;

							if( GetCashShopTask().GetCashLimitItemRemainCount( m_SlotUnits[i].pItem->GetCashItemSN() ) > 0 )
							{
								str = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4899 );
							}
							else
							{
								str = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4901 );
							}

							pSimpleTooltipDlg->ShowTooltipDlg( m_SlotUnits[i].pItemDiscountRate, true, str, 0xffffffff, true );
						}
					}
				}
#endif // PRE_ADD_LIMITED_CASHITEM
#ifdef PRE_ADD_NEW_MONEY_SEED
				if( m_SlotUnits[i].nReserveAmount > 0 && m_SlotUnits[i].nSeedReserveAmount > 0 && m_SlotUnits[i].pItemReserve )
				{
					m_SlotUnits[i].pItemReserve->GetUICoord( uiCoord );

					CDnSimpleTooltipDlg* pSimpleTooltipDlg = GetInterface().GetSimpleTooltipDialog();
					if( pSimpleTooltipDlg && uiCoord.IsInside( fMouseX, fMouseY ) )
					{
						std::wstring str;
#ifdef PRE_MOD_PETAL_WRITE
						str = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2030005 ), m_SlotUnits[i].nReserveAmount );
#else // PRE_MOD_PETAL_WRITE
						str = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4658 ), m_SlotUnits[i].nReserveAmount );
#endif // PRE_MOD_PETAL_WRITE
						str += L", ";
#ifdef PRE_MOD_PETAL_WRITE
						str += FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4985 ), m_SlotUnits[i].nSeedReserveAmount );
#else // PRE_MOD_PETAL_WRITE
						str += FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4971 ), m_SlotUnits[i].nSeedReserveAmount );
#endif // PRE_MOD_PETAL_WRITE
						pSimpleTooltipDlg->ShowTooltipDlg( m_SlotUnits[i].pItemReserve, true, str, 0xffffffff, true );
					}
				}
#endif // PRE_ADD_NEW_MONEY_SEED
			}
		}
		break;
	}

	return CDnCustomDlg::MsgProc( hWnd, uMsg, wParam, lParam );
}

void CDnCashShopGoodsDlg::ClearSlots(bool bShowOff)
{
	std::vector<SCashShopSlotUnit>::iterator iter = m_SlotUnits.begin();
	for (; iter != m_SlotUnits.end(); ++iter)
	{
		SCashShopSlotUnit& slot = *iter;
		slot.Clear();
		if (bShowOff)
			slot.Show(false, false);
	}
}

void CDnCashShopGoodsDlg::Update()
{
	if (m_bWishList)
	{
		const CS_INFO_LIST& wishList = GetCashShopTask().GetWishList();
		UpdatePages(&wishList, true);
	}
	else
	{
		const CS_INFO_LIST* pDataList = GetDataList(m_Type);
		if (pDataList == NULL)
		{
			_ASSERT(0);
			return;
		}

		UpdatePages(pDataList, true);
	}
}

void CDnCashShopGoodsDlg::UpdatePageControllers()
{
	int curPageGroupNum = (m_CurrentPage % _MAX_PAGE_COUNT_PER_GROUP) ? m_CurrentPage / _MAX_PAGE_COUNT_PER_GROUP + 1 : m_CurrentPage / _MAX_PAGE_COUNT_PER_GROUP;
	int pageStartNum	= ((curPageGroupNum - 1) * _MAX_PAGE_COUNT_PER_GROUP) + 1;

	int i = 0;
	std::wstring pageNumString;
	for (; i < _MAX_PAGE_COUNT_PER_GROUP; ++i)
	{
		m_pPageBtn[i]->ClearText();
		int currentNum = pageStartNum + i;
		if (m_MaxPage >= currentNum)
		{
			pageNumString = FormatW(L"%d", currentNum);
			m_pPageBtn[i]->SetText(pageNumString.c_str());
			m_pPageBtn[i]->Enable(m_CurrentPage != currentNum);
		}
	}

	bool bGroupBtnEnable = m_MaxPage > _MAX_PAGE_COUNT_PER_GROUP;
	m_pPageGroupPrev->Enable(bGroupBtnEnable);
	m_pPageGroupNext->Enable(bGroupBtnEnable);

	bool bPageBtnEnable = m_MaxPage > 1;
	m_pPagePrev->Enable(bPageBtnEnable);
	m_pPageNext->Enable(bPageBtnEnable);

	if (m_CurrentPage <= 1)
		m_pPagePrev->Enable(false);
	else if (m_CurrentPage >= m_MaxPage)
		m_pPageNext->Enable(false);
}

bool CDnCashShopGoodsDlg::IsShowEnableItem(const SCashShopItemInfo* pInfo) const
{
	if(pInfo)
	{
		if (pInfo->bOnSale == false)
			return false;
		else if (CDnCashShopTask::GetInstance().IsNotOnSaleItem(pInfo->sn))
			return false;

//#ifdef PRE_ADD_CASHSHOP_RENEWAL
//		else
//		{
//			if ( m_SortJob != 0 && m_SortJob < _countof(pInfo->bEnableJob))
//				return pInfo->bEnableJob[ m_SortJob ];			
//		}
//#else
		else if ( m_pJobSortComboBox )
		{
			int jobFilterNum = 0;
			bool bRet = m_pJobSortComboBox->GetSelectedValue(jobFilterNum);

			if (bRet)
			{
				if (jobFilterNum != 0 && jobFilterNum < _countof(pInfo->bEnableJob))
					return pInfo->bEnableJob[jobFilterNum];
			}
		}
//#endif // PRE_ADD_CASHSHOP_RENEWAL

		return true;
	}

	return false;
}

#ifdef PRE_ADD_SALE_COUPON

bool CDnCashShopGoodsDlg::IsExistItemList( CS_INFO_LIST& dataList, int nItemdID )
{
	bool bExsitItem = false;
	CS_INFO_LIST::const_iterator iter = dataList.begin();
	for( ; iter != dataList.end(); ++iter )
	{
		SCashShopItemInfo* pInfo = *iter;
		if( pInfo && pInfo->id == nItemdID )
		{
			bExsitItem = true;
			break;
		}
	}

	return bExsitItem;
}

#endif // PRE_ADD_SALE_COUPON

#ifdef PRE_ADD_CASHSHOP_RENEWAL
static bool CompareCashShopMyClass( SCashShopItemInfo* s1, SCashShopItemInfo* s2 )
{
	int classId = CDnActor::s_hLocalActor->GetClassID();
	if (CommonUtil::IsValidCharacterClassId(classId) == false)
		return false;

	if( s1->bEnableJob[classId] && s2->bEnableJob[classId] == false )
		return true;
	else if( s1->bEnableJob[classId] && s2->bEnableJob[classId] )
		return (s1->bOnlyOneClassEnable && s2->bOnlyOneClassEnable == false);

	return false;
}
#endif // PRE_ADD_CASHSHOP_RENEWAL

void CDnCashShopGoodsDlg::MakeItemListShowable( CS_INFO_LIST& dataList, const CS_INFO_LIST& wholeItemList )
{
#ifdef PRE_ADD_SALE_COUPON
	dataList.clear();

	if( m_nCategory == 7 )	// ÇÒÀÎ Ç°¸ñ Ä«Å×°í¸®´Â À¯Àú°¡ ¼ÒÀ¯ÇÑ ÄíÆùÀ» °Ë»çÇØ¼­ ¸®½ºÆ®¸¦ »õ·Î ¸¸µê
	{
		DNTableFileFormat* pCouponSox = GetDNTable( CDnTableDB::TSALECOUPON );
		if( pCouponSox == NULL )
			return;

		std::vector<CDnItem *> vecCouponItem;
		GetItemTask().FindItemFromItemType( ITEMTYPE_SALE_COUPON, ITEM_SLOT_TYPE::ST_INVENTORY_CASH, vecCouponItem );
		if( vecCouponItem.empty() )
			return;

		std::sort( vecCouponItem.begin(), vecCouponItem.end(), CompareCouponExpire );	// ÄíÆù¸¸·áÀÏÀÌ Á¦ÀÏ ÂªÀº ¸®½ºÆ® ºÎÅÍ º¸¿©ÁÜ

		char szLabel[32];
		for( int i=0; i<static_cast<int>( vecCouponItem.size() ); i++ )
		{
			if( vecCouponItem[i] == NULL )
				continue;

			int nCouponID = vecCouponItem[i]->GetTypeParam( 0 );

			for( int j=0; j<MAX_COUPON_SALEITEM; j++ )
			{
				if( j+1 < 10 )
					sprintf_s( szLabel, "_UseItem0%d", j+1 );
				else
					sprintf_s( szLabel, "_UseItem%d", j+1 );
				
				int nSaleItemID = pCouponSox->GetFieldFromLablePtr( nCouponID, szLabel )->GetInteger();

				if( nSaleItemID == 0 )
					continue;

#ifdef PRE_MOD_SALE_COUPON_LEVEL_LIMIT
				bool bLimitUseCouponLevel = false;
				int nLimitUseCouponLevel = 0;
				int nLocalActorLevel = CDnActor::s_hLocalActor->GetLevel();

				if( nLocalActorLevel < vecCouponItem[i]->GetLevelLimit() )
				{
					bLimitUseCouponLevel = true;
					nLimitUseCouponLevel = vecCouponItem[i]->GetLevelLimit();
				}
#endif // PRE_MOD_SALE_COUPON_LEVEL_LIMIT

				CS_INFO_LIST::const_iterator iter = wholeItemList.begin();
				for( ; iter != wholeItemList.end(); ++iter )
				{
					SCashShopItemInfo* pInfo = *iter;
					if( pInfo && pInfo->id == nSaleItemID )
					{
#ifdef PRE_MOD_SALE_COUPON_LEVEL_LIMIT
						if( IsShowEnableItem( pInfo ) )
						{
							if( !IsExistItemList( dataList, nSaleItemID ) )
							{
								pInfo->bLimitUseCouponLevel = bLimitUseCouponLevel;
								pInfo->nLimitUseCouponLevel = nLimitUseCouponLevel;
								dataList.push_back( pInfo );
							}
							else	// Áßº¹ ¾ÆÀÌÅÛ¿¡ ´ëÇØ¼­ ÄíÆù ·¹º§ Á¦ÇÑ »óÅÂ¸¦ °»½Å
							{
								CS_INFO_LIST::iterator iter = dataList.begin();
								for( ; iter != dataList.end(); iter++ )
								{
									SCashShopItemInfo* pListInfo = *iter;
									if( pListInfo && pListInfo->id == pInfo->id )
									{
										if( bLimitUseCouponLevel == false )
											pInfo->bLimitUseCouponLevel = bLimitUseCouponLevel;
										if( pInfo->nLimitUseCouponLevel > nLimitUseCouponLevel )
											pInfo->nLimitUseCouponLevel = nLimitUseCouponLevel;
									}
								}
							}
						}
#else // PRE_MOD_SALE_COUPON_LEVEL_LIMIT
						if( IsShowEnableItem( pInfo ) && !IsExistItemList( dataList, nSaleItemID ) )
							dataList.push_back( pInfo );
#endif // PRE_MOD_SALE_COUPON_LEVEL_LIMIT
					}
				}
			}
		}

#ifdef PRE_ADD_CASHSHOP_RENEWAL
		// Á÷¾÷Á¤·Ä.
		std::sort( dataList.begin(), dataList.end(), CompareCashShopMyClass );
#endif // PRE_ADD_CASHSHOP_RENEWAL

		vecCouponItem.clear();
		int listSize = static_cast<int>( dataList.size() );
		if( listSize == 0 )
		{
			m_MaxPage = 1;
		}
		else
		{			
#ifdef PRE_ADD_CASHSHOP_RENEWAL			
			m_MaxPage = listSize / ( m_bMainTab == true ? _MAX_GOODS_SLOT_NUM_PER_MAINPAGE : _MAX_GOODS_SLOT_NUM_PER_PAGE );
			if( m_MaxPage == 0 || (listSize % ( m_bMainTab == true ? _MAX_GOODS_SLOT_NUM_PER_MAINPAGE : _MAX_GOODS_SLOT_NUM_PER_PAGE )) != 0)
				m_MaxPage++;
#else
			m_MaxPage = listSize / _MAX_GOODS_SLOT_NUM_PER_PAGE;
			if( m_MaxPage == 0 || (listSize % _MAX_GOODS_SLOT_NUM_PER_PAGE) != 0)
				m_MaxPage++;
#endif // PRE_ADD_CASHSHOP_RENEWAL
		}
		UpdatePageControllers();
	}
	else
	{
		CS_INFO_LIST::const_iterator iter = wholeItemList.begin();
		for( ; iter != wholeItemList.end(); ++iter )
		{
			SCashShopItemInfo* pInfo = *iter;
			if( pInfo )
			{
#ifdef PRE_MOD_SALE_COUPON_LEVEL_LIMIT
				pInfo->bLimitUseCouponLevel = false;
				pInfo->nLimitUseCouponLevel = 0;
#endif // PRE_MOD_SALE_COUPON_LEVEL_LIMIT
				if( IsShowEnableItem( pInfo ) )
				{
#ifdef PRE_ADD_CASHSHOP_RENEWAL
					if( CompareItem( dataList, pInfo ) )
						dataList.push_back( pInfo );
#else
					dataList.push_back( pInfo );
#endif // PRE_ADD_CASHSHOP_RENEWAL
				}
			}
		}
	}
#else // PRE_ADD_SALE_COUPON
	dataList.clear();
	CS_INFO_LIST::const_iterator iter = wholeItemList.begin();
	for( ; iter != wholeItemList.end(); ++iter )
	{
		SCashShopItemInfo* pInfo = *iter;
		if( pInfo )
		{
			if( IsShowEnableItem( pInfo ) )
			{
#ifdef PRE_ADD_CASHSHOP_RENEWAL
				if( CompareItem( dataList, pInfo ) )
					dataList.push_back( pInfo );
#else
				dataList.push_back( pInfo );
#endif // PRE_ADD_CASHSHOP_RENEWAL
			}
		}
	}
#endif // PRE_ADD_SALE_COUPON

#ifdef PRE_ADD_CASHSHOP_RENEWAL
	RefreshPageControllers( (int)dataList.size() );
#endif // PRE_ADD_CASHSHOP_RENEWAL

}

void CDnCashShopGoodsDlg::UpdateSlots(const CS_INFO_LIST* pDataList, bool bShowPage)
{
	int listSizeOnSale = (pDataList == NULL) ? 0 : GetCashShopTask().GetOnSaleCount(*pDataList);

	ClearSlots(true);

#ifdef PRE_ADD_NEW_MONEY_SEED
	CDnSimpleTooltipDlg* pSimpleTooltipDlg = GetInterface().GetSimpleTooltipDialog();
	if( pSimpleTooltipDlg && pSimpleTooltipDlg->IsShow() )
		pSimpleTooltipDlg->Show( false );
#endif // PRE_ADD_NEW_MONEY_SEED

	if (pDataList == NULL || listSizeOnSale <= 0)
	{
		m_pPageGroupPrev->Show(false);
		m_pPageGroupNext->Show(false);
		m_pPagePrev->Show(false);
		m_pPageNext->Show(false);
		int i = 0;
		for (; i < _MAX_PAGE_COUNT_PER_GROUP; ++i)
			m_pPageBtn[i]->Show(false);
		return;
	}

	if (m_bWishList == false)
		m_DataSlotIdxMatchList.clear();
	else
		m_WishListMatchList.clear();

	CS_INFO_LIST dataList;
	MakeItemListShowable(dataList, *pDataList);
	if (dataList.empty())
	{
		_ASSERT(0);
		return;
	}

	int i = 0;
	int forDataIdx = 0;
	for (; i < int(m_SlotUnits.size()); ++forDataIdx)
	{
		SCashShopSlotUnit& slot = m_SlotUnits[i];
		slot.Clear();
#ifdef PRE_ADD_CASHSHOP_RENEWAL
		int dataIdx = ((m_CurrentPage - 1) * ( m_bMainTab == true ? _MAX_GOODS_SLOT_NUM_PER_MAINPAGE : _MAX_GOODS_SLOT_NUM_PER_PAGE ) ) + forDataIdx;
#else
		int dataIdx = ((m_CurrentPage - 1) * _MAX_GOODS_SLOT_NUM_PER_PAGE) + forDataIdx;
#endif // PRE_ADD_CASHSHOP_RENEWAL
		if (dataIdx < 0 || dataIdx >= int(dataList.size()))
			break;

		const SCashShopItemInfo* pInfo = dataList[dataIdx];
		if (pInfo)
		{
			if (CDnActor::s_hLocalActor)
				slot.Set(*pInfo, CDnActor::s_hLocalActor->GetClassID());
			else
				_ASSERT(0);
		}
		else
		{
			_ASSERT(0);
		}

		if (m_bWishList)
			m_WishListMatchList.insert(std::make_pair(i, pInfo->sn));
		else
			m_DataSlotIdxMatchList.insert(std::make_pair(i, pInfo->id));

		if (bShowPage)
			slot.Show(true, m_bWishList);

		++i;
	}
}

int CDnCashShopGoodsDlg::GetOnSalePageCount(const CS_INFO_LIST& dataList) const
{
	int pageCount = 0;
	CS_INFO_LIST::const_iterator iter = dataList.begin();
	for (; iter != dataList.end(); ++iter)
	{
		const SCashShopItemInfo* pInfo = *iter;
		if (IsShowEnableItem(pInfo))
			pageCount++;
	}

	return pageCount;
}

void CDnCashShopGoodsDlg::UpdatePages(const CS_INFO_LIST* pDataList, bool bShowPage)
{	
	int listSize = (pDataList == NULL) ? 0 : int(pDataList->size());
	if (listSize == 0)
	{
		m_MaxPage = 1;
	}
	else
	{
		listSize = GetOnSalePageCount(*pDataList);
#ifdef PRE_ADD_CASHSHOP_RENEWAL
		m_MaxPage = listSize / ( m_bMainTab == true ? _MAX_GOODS_SLOT_NUM_PER_MAINPAGE : _MAX_GOODS_SLOT_NUM_PER_PAGE );
		if (m_MaxPage == 0 || (listSize % ( m_bMainTab == true ? _MAX_GOODS_SLOT_NUM_PER_MAINPAGE : _MAX_GOODS_SLOT_NUM_PER_PAGE )) != 0)
			m_MaxPage++;
#else
		m_MaxPage = listSize / _MAX_GOODS_SLOT_NUM_PER_PAGE;
		if (m_MaxPage == 0 || (listSize % _MAX_GOODS_SLOT_NUM_PER_PAGE) != 0)
			m_MaxPage++;
#endif // PRE_ADD_CASHSHOP_RENEWAL
	}
	CommonUtil::ClipNumber(m_CurrentPage, 1, m_MaxPage);

	//	Set Page Static
	UpdatePageControllers();

	//	Fill Slots
	UpdateSlots(pDataList, bShowPage);

	//	Set Select Index
#ifdef PRE_ADD_CASHSHOP_RENEWAL
	m_SelectedIndex = (m_CurrentPage - 1) * ( m_bMainTab == true ? _MAX_GOODS_SLOT_NUM_PER_MAINPAGE : _MAX_GOODS_SLOT_NUM_PER_PAGE );
#else
	m_SelectedIndex = (m_CurrentPage - 1) * _MAX_GOODS_SLOT_NUM_PER_PAGE;
#endif // PRE_ADD_CASHSHOP_RENEWAL
	if (m_SelectedIndex < 0 || m_SelectedIndex > listSize)
	{
		_ASSERT(0);
		return;
	}
}

const SCashShopItemInfo* CDnCashShopGoodsDlg::GetData(eCashShopSubCatType type, int idx) const
{
	const SCashShopItemInfo* pItemInfo = GetCashShopTask().GetSubCatItem(type, idx);

	return pItemInfo;
}

const CS_INFO_LIST* CDnCashShopGoodsDlg::GetDataList(eCashShopSubCatType type) const
{
	return GetCashShopTask().GetSubCatItemList(type);
}

void CDnCashShopGoodsDlg::Show(bool bShow)
{
	if (bShow == m_bShow)
		return;

	if (bShow)
	{
		m_CurrentPage = 1;

		eCashShopEtcSortType sortTypeEtc = eCashShopEtcSortType::SORT_BASIC;
#ifdef PRE_ADD_CASHSHOP_RENEWAL
		SetSortJob( CDnCashShopTask::GetInstance().GetSortJob() );   // job.
		sortTypeEtc = (eCashShopEtcSortType)CDnCashShopTask::GetInstance().GetetSortEtc(); // etc.
#endif // PRE_ADD_CASHSHOP_RENEWAL

		//GetCashShopTask().Sort(m_Type, SORT_BASIC);
		GetCashShopTask().Sort( m_Type, sortTypeEtc );
		Update();

		// test.
		//OpenBrowser( std::string("http://dn.happyoz.com"), SUICoord(200,100,200,100) );	
	}

	CDnCustomDlg::Show(bShow);
}

void CDnCashShopGoodsDlg::OnChangeResolution()
{
	CEtUIDialog::OnChangeResolution();
	Update();
}

void CDnCashShopGoodsDlg::OnUICallbackProc(int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg)
{
	SetCmdControlName( pControl->GetControlName() );

	if (nCommand == EVENT_BUTTON_CLICKED)
	{
		if (IsCmdControl("ID_YES"))
		{
			if (CDnCashShopTask::GetInstance().IsCashShopMode(eCSMODE_WARN_PACKAGEWITHINVEN))
			{
				if (nID == BUY_INVEN_STORAGE_BUYITEMNOW_DLG)
				{
					const SCashShopCartItemInfo& info = GetCashShopTask().GetBuyItemNow();
					eCashUnitType type = (GetCashShopTask().IsPackageItem(info.presentSN)) ? eCashUnit_Package : eCashUnit_BuyItemNow;
					m_pCommonDlgMgr->OpenPayDlg(type, true);
				}
				else if (nID == BUY_INVEN_STORAGE_CART_DLG)
				{
					const SCashShopCartItemInfo& info = GetCashShopTask().GetBuyItemNow();
					GetCashShopTask().PutItemIntoCart(info.presentSN);
				}
			}
			if( nID == OVERLAP_ALERT_BUYITEMNOW_DLG )
			{
				const SCashShopCartItemInfo& info = GetCashShopTask().GetBuyItemNow();
				eCashUnitType type = (GetCashShopTask().IsPackageItem(info.presentSN)) ? eCashUnit_Package : eCashUnit_BuyItemNow;
				m_pCommonDlgMgr->OpenPayDlg(type, true);
			}
		}

		if (IsCmdControl("ID_BUTTON_OK"))
		{
			if (nID == LEVEL_ALERT_BUYITEMNOW_DLG || nID == OVERLAP_ALERT_BUYITEMNOW_DLG )
			{
				const SCashShopCartItemInfo& info = GetCashShopTask().GetBuyItemNow();
				eCashUnitType type = (GetCashShopTask().IsPackageItem(info.presentSN)) ? eCashUnit_Package : eCashUnit_BuyItemNow;
				m_pCommonDlgMgr->OpenPayDlg(type, true);
			}
			else if (nID == LEVEL_ALERT_CARTITEMNOW_DLG || nID == OVERLAP_ALERT_CARTITEMNOW_DLG)
			{
				const SCashShopCartItemInfo& info = GetCashShopTask().GetBuyItemNow();
				GetCashShopTask().PutItemIntoCart(info.presentSN);
			}
		}
	}
}



// test
#include "DnMainFrame.h"
#include "GameOption.h"
CComQIPtr<IWebBrowser2>			pChargeWebBrowserTest;
HWND							g_hwndChargeBrowserTest;
//WNDPROC							g_ChildWndProcTest;
CComObject<CDnCashShopIESink>*	pIESinkTest;

//LRESULT CALLBACK ChildWndProcTest(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
//{
//	switch(message)
//	{
//	case WM_WINDOWPOSCHANGING:
//		{
//			((WINDOWPOS*)lParam)->flags |= SWP_NOMOVE;
//		}
//		break;
//	}
//
//	return CallWindowProc(g_ChildWndProcTest, hWnd, message, wParam, lParam);
//}


void CDnCashShopGoodsDlg::OpenBrowser(const std::string& url, const SUICoord& coord )
{
	if (SUCCEEDED(OleInitialize(NULL)))
	{
		HRESULT hr;

		if (pIESinkTest)
			pIESinkTest = NULL;

		if (pChargeWebBrowserTest == NULL)
		{
			AtlAxWinInit();			

			RECT mainRect;
			SUICoord correctCoord = coord;
			if (CGameOption::GetInstance().m_bWindow)
			{
				GetWindowRect(CDnMainFrame::GetInstance().GetHWnd(), &mainRect);
				correctCoord.fX = coord.fX;
				correctCoord.fY = coord.fY;

			}
			g_hwndChargeBrowserTest = CreateWindow(L"AtlAxWin80", L"Shell.Explorer.2", WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_BORDER, (int)correctCoord.fX, (int)correctCoord.fY, (int)correctCoord.fWidth, (int)correctCoord.fHeight, 
				CDnMainFrame::GetInstance().GetHWnd(), (HMENU)0, CDnMainFrame::GetInstance().GetHInst(), NULL);
//#ifdef PRE_FIX_NOMOVE_BROWSER
//			g_ChildWndProcTest = (WNDPROC)SetWindowLongPtr(g_hwndChargeBrowserTest, GWL_WNDPROC, (LONG_PTR)ChildWndProcTest);
//#endif

			CComPtr<IUnknown> punkIE;
			if (AtlAxGetControl(g_hwndChargeBrowserTest, &punkIE) == S_OK)
			{
				pChargeWebBrowserTest = punkIE;

				if (pIESinkTest == NULL)
				{
					hr = CComObject<CDnCashShopIESink>::CreateInstance(&pIESinkTest);
					if(FAILED(hr))
						_ASSERT(0);
					pIESinkTest->SetTask( &CDnCashShopTask::GetInstance() );
				}

				AtlGetObjectSourceInterface(punkIE, &(pIESinkTest->m_libid), &(pIESinkTest->m_iid), &(pIESinkTest->m_wMajorVerNum), &(pIESinkTest->m_wMinorVerNum));
				hr = pIESinkTest->DispEventAdvise(pChargeWebBrowserTest, &(pIESinkTest->m_iid));
				pIESinkTest->SetCashShopSinkType(CDnCashShopIESink::eGENERAL);

				if (FAILED(hr))
					_ASSERT(0);
			}


		}
		else
		{
			CComVariant vUrl(url.c_str()), vEmpty;
			hr = pChargeWebBrowserTest->Navigate2(&vUrl, &vEmpty, &vEmpty, &vEmpty, &vEmpty);
			if (SUCCEEDED(hr))
			{
				pChargeWebBrowserTest->put_Visible(VARIANT_TRUE);
				ShowWindow((HWND)g_hwndChargeBrowserTest, SW_SHOW);
				InvalidateRect(g_hwndChargeBrowserTest, NULL, true);

				//m_bGeneralBrowserOpened = true;
				//m_GeneralBrowserOpenedType = type;

				//m_pCashShopDlg->DisableAllDlgs(true, msgBoxString);
			}

			//m_pCashShopDlg->HandleCashShopError(eERRCS_CANT_OPEN_CHARGEBROWSER, true);
			return;
		}

		if (CGameOption::IsActive() && CGameOption::GetInstance().m_bWindow == false && GetEtDevice())
		{
			LPDIRECT3DDEVICE9 pDevice = (LPDIRECT3DDEVICE9)GetEtDevice()->GetDevicePtr();
			if (pDevice == NULL)
			{
				//m_pCashShopDlg->HandleCashShopError(eERRCS_CANT_OPEN_CHARGEBROWSER_D3D, true);
				return;
			}

			pDevice->SetDialogBoxMode(TRUE);			
		}
		

		CComVariant vUrl(url.c_str()), vEmpty;
		hr = pChargeWebBrowserTest->Navigate2(&vUrl, &vEmpty, &vEmpty, &vEmpty, &vEmpty);
		if (SUCCEEDED(hr))
		{
			pChargeWebBrowserTest->put_Visible(VARIANT_TRUE);
			ShowWindow((HWND)g_hwndChargeBrowserTest, SW_SHOW);
			InvalidateRect(g_hwndChargeBrowserTest, NULL, true);

			//m_bGeneralBrowserOpened = true;
			//m_GeneralBrowserOpenedType = type;

			//m_pCashShopDlg->DisableAllDlgs(true, msgBoxString);
		}

		OleUninitialize();
	}
}


void CDnCashShopGoodsDlg::OnExceptionalCloseChargeBrowser(bool onInitOLE)
{
	if (pChargeWebBrowserTest)
	{
		pChargeWebBrowserTest->Quit();
		pChargeWebBrowserTest.Release();
		//pChargeWebBrowserTest = NULL;
	}

	if (onInitOLE)
		OleUninitialize();
}

#ifdef PRE_ADD_CASHSHOP_RENEWAL
bool CDnCashShopGoodsDlg::CashshopSort( int nValue )
{
	return GetCashShopTask().Sort(m_Type, (eCashShopEtcSortType)nValue );
}

// id¿Í snÀÌ °°Àº ¾ÆÀÌÅÛÀº µî·ÏÇÏÁö ¾Ê´Â´Ù.
bool CDnCashShopGoodsDlg::CompareItem( CS_INFO_LIST & dataList, SCashShopItemInfo * pInfo )
{
	int size = (int)dataList.size();
	for( int i=0; i<size; ++i )
	{
		SCashShopItemInfo * pDataInfo = dataList[ i ];
		if( pDataInfo->id == pInfo->id && pDataInfo->sn == pInfo->sn )
			return false;
	}

	return true;
}


void CDnCashShopGoodsDlg::RefreshPageControllers( int listSize )
{	
	m_MaxPage = 1;
	if( listSize > 0 )
	{			
#ifdef PRE_ADD_CASHSHOP_RENEWAL			
		m_MaxPage = listSize / ( m_bMainTab == true ? _MAX_GOODS_SLOT_NUM_PER_MAINPAGE : _MAX_GOODS_SLOT_NUM_PER_PAGE );
		if( m_MaxPage == 0 || (listSize % ( m_bMainTab == true ? _MAX_GOODS_SLOT_NUM_PER_MAINPAGE : _MAX_GOODS_SLOT_NUM_PER_PAGE )) != 0)
			m_MaxPage++;
#else
		m_MaxPage = listSize / _MAX_GOODS_SLOT_NUM_PER_PAGE;
		if( m_MaxPage == 0 || (listSize % _MAX_GOODS_SLOT_NUM_PER_PAGE) != 0)
			m_MaxPage++;
#endif // PRE_ADD_CASHSHOP_RENEWAL
	}
	UpdatePageControllers();
}

#endif // PRE_ADD_CASHSHOP_RENEWAL