#include "stdafx.h"
#include "EtPrimitiveDraw.h"
#include "DnCashShopDlg.h"
#include "DnCashShopCatDlg.h"
#include "DnCashShopPreviewDlg.h"
#include "DnCashShopCartDlg.h"
#include "DnCashShopTask.h"
#include "DnInterfaceCamera.h"
#include "DnInterface.h"
#include "DnCashShopSendGiftDlg.h"
#include "DnCashShopCommonDlgMgr.h"
#include "DnCashShopInvenTabDlg.h"
#include "DnCashShopCouponDlg.h"
#include "DnPartyTask.h"
#include "PartySendPacket.h"
#include "DnCashShopAboutPetal.h"
#include "DnCashShopGiftFailDlg.h"
#include "DnDummyModalDlg.h"
#ifdef PRE_ADD_CASHSHOP_REFUND_CL
#include "DnCashShopRefundInvenDlg.h"
#endif // PRE_ADD_CASHSHOP_REFUND_CL
#ifdef PRE_MOD_PETAL_WRITE
#include "DnInterfaceString.h"
#endif // PRE_MOD_PETAL_WRITE
#ifdef PRE_ADD_SALE_COUPON
#include "DnCashShopGoodsDlg.h"
#endif // PRE_ADD_SALE_COUPON
#include "DnCashShopMessageBox.h"
#ifdef PRE_ADD_CASHSHOP_ACTOZ
#include "DnCashShopConfirmRefundDlg.h"
#endif // PRE_ADD_CASHSHOP_ACTOZ
#ifdef PRE_RECEIVEGIFTALL
#include "DnCashShopRecvGiftDlg.h"
#endif // PRE_RECEIVEGIFTALL

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnCashShopDlg::CDnCashShopDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback)
			: CEtUITabDialog(dialogType, pParentDialog, nID, pCallback, true)
{
#ifdef PRE_ADD_CASHSHOP_CREDIT
	m_pCreditAmount		= NULL;
#endif // PRE_ADD_CASHSHOP_CREDIT
	m_pCashAmount		= NULL;
	m_pReserveAmount	= NULL;
#if defined (PRE_ADD_NEW_MONEY_SEED) && defined (PRE_ADD_CASHSHOP_RENEWAL)
	m_pAboutSeedDlg = NULL;
	m_pSeedAmount = NULL;
#endif // PRE_ADD_NEW_MONEY_SEED

	m_pChargeBtn			= NULL;
	m_pPreviewDlg			= NULL;
	m_pCartDlg				= NULL;
	m_pExitBtn				= NULL;
	m_pInvenDlg				= NULL;
	m_pCouponDlg			= NULL;
	m_pAboutPetalBtn		= NULL;
	m_pGiftBtn				= NULL;
	m_pGiftFailDlg			= NULL;
	m_pDummyModalDlg		= NULL;
	m_pAboutPetalDlg		= NULL;
	m_pCashInvenBtn			= NULL;
	m_pCouponBtn			= NULL;
#ifdef PRE_ADD_CASHSHOP_REFUND_CL
	m_pInvenRefundDlg		= NULL;
#endif
	m_pPetPreviewDlg		= NULL;
	m_nPetCategory			= -1;
	m_DisableDlgDelayTime	= 0.f;
	m_bOpenGiftWindow		= false;
	m_pCashShopMsgBox	= NULL;
#ifdef PRE_ADD_CASHSHOP_ACTOZ
	m_pConfirmRefundDlg = NULL;
#endif // PRE_ADD_CASHSHOP_ACTOZ

#ifdef PRE_ADD_INVEN_EXTENSION
	m_ReserveBuyItemNowSN	= 0;
#endif
#ifdef PRE_ADD_LIMITED_CASHITEM
	m_pRefreshBtn			= NULL;
	m_fRefreshCoolTime		= 0.f;
	m_bRefreshCoolTimer		= false;
#endif // PRE_ADD_LIMITED_CASHITEM
#ifdef PRE_ADD_CADGE_CASH
	m_nMailID				= 0;
	m_nPackageSN			= 0;
	m_bCadgeButtonClick		= false;
#endif // PRE_ADD_CADGE_CASH
#ifdef PRE_ADD_SALE_COUPON
	m_bSaleCouponOpen = false;
#ifdef PRE_ADD_SALE_COUPON_CATEGORY
	m_nSaleCouponSubCategory = 0;
#endif // PRE_ADD_SALE_COUPON_CATEGORY
#endif // PRE_ADD_SALE_COUPON
}

CDnCashShopDlg::~CDnCashShopDlg()
{
	SAFE_DELETE(m_pPreviewDlg);
	SAFE_DELETE(m_pCartDlg);
	SAFE_DELETE(m_pInvenDlg);
	SAFE_DELETE(m_pCouponDlg);
	SAFE_DELETE(m_pAboutPetalDlg);
#if defined (PRE_ADD_NEW_MONEY_SEED) && defined (PRE_ADD_CASHSHOP_RENEWAL)
	SAFE_DELETE(m_pAboutSeedDlg);
#endif // PRE_ADD_NEW_MONEY_SEED
	SAFE_DELETE(m_pGiftFailDlg);
	SAFE_RELEASE_SPTR(m_hBgTexture);
	SAFE_DELETE(m_pDummyModalDlg);
#ifdef PRE_ADD_CASHSHOP_REFUND_CL
	SAFE_DELETE(m_pInvenRefundDlg);
#endif
	SAFE_DELETE( m_pPetPreviewDlg );
	SAFE_DELETE(m_pCashShopMsgBox);
#ifdef PRE_ADD_CASHSHOP_ACTOZ
	SAFE_DELETE( m_pConfirmRefundDlg );
#endif // PRE_ADD_CASHSHOP_ACTOZ
#ifdef PRE_ADD_CADGE_CASH
	m_ReserveCadgeItemList.clear();
#endif // PRE_ADD_CADGE_CASH
}

#ifdef PRE_ADD_CASHSHOP_RENEWAL
void CDnCashShopDlg::Initialize( const char *pFileName, bool bShow )
{	
	SAFE_RELEASE_SPTR(m_hBgTexture);
	m_hBgTexture = LoadResource(CEtResourceMng::GetInstance().GetFullName("CashShopBG.dds").c_str(), RT_TEXTURE);

	CEtUIDialog::Initialize(CEtResourceMng::GetInstance().GetFullName( pFileName ).c_str(), bShow);

	if (CDnCashShopTask::IsActive())
	{
		GetCashShopTask().SetCashShopDlg(this);
	}
	else
	{
		_ASSERT(0);
	}	
}
#else
void CDnCashShopDlg::Initialize(bool bShow)
{
	SAFE_RELEASE_SPTR(m_hBgTexture);
	m_hBgTexture = LoadResource(CEtResourceMng::GetInstance().GetFullName("CashShopBG.dds").c_str(), RT_TEXTURE);

#ifdef PRE_ADD_CASHSHOP_REFUND_CL
	std::string base = CDnCashShopTask::GetInstance().IsCashShopMode(eCSMODE_REFUND) ? "CSBase_Refund.ui" : "CSBase.ui";
	CEtUIDialog::Initialize(CEtResourceMng::GetInstance().GetFullName(base.c_str()).c_str(), bShow);
#else
	CEtUIDialog::Initialize(CEtResourceMng::GetInstance().GetFullName("CS_Base.ui").c_str(), bShow);//RLKT TEST
#endif

	if (CDnCashShopTask::IsActive())
	{
		GetCashShopTask().SetCashShopDlg(this);
	}
	else
	{
		_ASSERT(0);
	}
}
#endif // PRE_ADD_CASHSHOP_RENEWAL

#define MAX_CASHSHOP_CAT_RADIO_BTN_COUNT 8

void CDnCashShopDlg::InitialUpdate()
{
	CDnCashShopCatDlg*	pCashShopCatDlg(NULL);
	CEtUIRadioButton*	pTabButton(NULL);

	if (CDnCashShopTask::IsActive() == false)
	{
		_ASSERT(0);
		return;
	}

	int i = 0;
	std::string tabBtnID;

	const std::map<int, SCashShopMainTabShowInfo>& mainTabList = GetCashShopTask().GetShowMainTabList();
	for (; i < MAX_CASHSHOP_CAT_RADIO_BTN_COUNT; ++i)
	{
		tabBtnID = FormatA("ID_BUTTON_CATEGORY%d", i);
		pTabButton = GetControl<CEtUIRadioButton>(tabBtnID.c_str());
		m_pCategoryBtns.push_back(pTabButton);

		std::map<int, SCashShopMainTabShowInfo>::const_iterator iter = mainTabList.find(i);
		if (iter != mainTabList.end())
		{
			const SCashShopMainTabShowInfo& mainTabInfo = (*iter).second;
			if (mainTabInfo.showType == eCST_HIDE)
			{
				pTabButton->Show(false);
				continue;
			}

			const std::wstring title = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, mainTabInfo.uiStringNum);
			if (title.empty() == false)
			{
				pTabButton->SetText(title.c_str());
				if (mainTabInfo.showType == eCST_DISABLE)
					pTabButton->Enable(false);
			}
			else
			{
				pTabButton->Show(false);
				continue;
			}

			int mainTabNum = (*iter).first;
			pCashShopCatDlg = new CDnCashShopCatDlg(UI_TYPE_CHILD, this, mainTabNum);
			pCashShopCatDlg->Initialize(false, mainTabNum, &m_CommonDlgMgr);
			pCashShopCatDlg->SetCheckedTab(pCashShopCatDlg->GetFrontTabID());
			AddTabDialog(pTabButton, pCashShopCatDlg);
			m_pCategoryDlgs.push_back(pCashShopCatDlg);
		}
		else
		{
			pTabButton->Show(false);
			continue;
		}
	}

#ifdef PRE_ADD_CASHSHOP_RENEWAL
	// 검색카테고리생성.
	pTabButton = GetControl<CEtUIRadioButton>("ID_BUTTON_CATEGORY_SEARCH");
	//pTabButton->Show( false );
	m_pCategoryBtns.push_back(pTabButton);
	pCashShopCatDlg = new CDnCashShopCatDlg(UI_TYPE_CHILD, this, MAINCATEGORY_SERACH);
	pCashShopCatDlg->Initialize(false, MAINCATEGORY_SERACH, &m_CommonDlgMgr);
	//pCashShopCatDlg->SetCheckedTab(pCashShopCatDlg->GetFrontTabID());
	AddTabDialog(pTabButton, pCashShopCatDlg);
	m_pCategoryDlgs.push_back(pCashShopCatDlg);
#endif // PRE_ADD_CASHSHOP_RENEWAL
	

	if (m_pCategoryBtns.empty() == false)
		SetCheckedTab(m_pCategoryBtns[0]->GetTabID());

	for( int i=0; i<static_cast<int>( m_pCategoryDlgs.size() ); i++ )
	{
		const std::map<int, SCashShopTabShowInfo>* pShowSubTabList = GetCashShopTask().GetShowSubTabList( i );
		if( pShowSubTabList == NULL )
			continue;

		bool bFindPetCategory = false;
		std::map<int, SCashShopTabShowInfo>::const_iterator iter = pShowSubTabList->begin();
		for( ; iter != pShowSubTabList->end(); iter++ )
		{
			const SCashShopTabShowInfo& subTabInfo = (*iter).second;
			if( subTabInfo.uiStringNum == 4504 )	// 4504 uiString == "펫"
			{
				bFindPetCategory = true;
				m_nPetCategory = i;
				break;
			}
		}

		if( bFindPetCategory )
			break;
	}

	m_pPetPreviewDlg = new CDnCashShopPetPreviewDlg( UI_TYPE_CHILD, this, PREVIEW_DLG );
	m_pPetPreviewDlg->Initialize( false );

	m_pPreviewDlg = new CDnCashShopPreviewDlg(UI_TYPE_CHILD, this, PREVIEW_DLG);
	m_pPreviewDlg->Initialize(false);

	m_pCartDlg = new CDnCashShopCartDlg(UI_TYPE_CHILD, this, INVEN_DLG);
	m_pCartDlg->Initialize(true);

	m_pInvenDlg = new CDnCashShopInvenTabDlg( UI_TYPE_MODAL, NULL);
	m_pInvenDlg->Initialize( false );

	m_pCouponDlg = new CDnCashShopCouponDlg(UI_TYPE_MODAL, NULL);
	m_pCouponDlg->Initialize(false);

	m_pAboutPetalDlg = new CDnCashShopAboutPetal(UI_TYPE_MODAL, NULL);
	m_pAboutPetalDlg->Initialize(false);

#if defined (PRE_ADD_NEW_MONEY_SEED) && defined (PRE_ADD_CASHSHOP_RENEWAL)
	m_pAboutSeedDlg = new CDnCashShopAboutSeed(UI_TYPE_MODAL, NULL);
	m_pAboutSeedDlg->Initialize(false);
#endif // PRE_ADD_NEW_MONEY_SEED

	m_pGiftFailDlg = new CDnCashShopGiftFailDlg(UI_TYPE_MODAL, NULL);
	m_pGiftFailDlg->Initialize(false);

	m_pDummyModalDlg = new CDnDummyModalDlg;
	m_pDummyModalDlg->Initialize(false);

	m_pCashShopMsgBox = new CDnCashShopMessageBox(UI_TYPE_MODAL);
	m_pCashShopMsgBox->Initialize(false);

#ifdef PRE_ADD_CASHSHOP_REFUND_CL
	if (CDnCashShopTask::GetInstance().IsCashShopMode(eCSMODE_REFUND))
	{
		m_pInvenRefundDlg = new CDnCashShopRefundInvenDlg;
		m_pInvenRefundDlg->Initialize(false);
	}
#endif

#ifdef PRE_ADD_CASHSHOP_ACTOZ
	m_pConfirmRefundDlg = new CDnCashShopConfirmRefundDlg( UI_TYPE_MODAL, NULL );
	m_pConfirmRefundDlg->Initialize( false );
#endif // PRE_ADD_CASHSHOP_ACTOZ

	m_CommonDlgMgr.InitializeDlgs(this);

#if defined PRE_ADD_PETALSHOP || defined PRE_CHN_OBTCASHSHOP
	m_pCashTitle		= GetControl<CEtUIStatic>("ID_STATIC13");
	m_pCashUnit			= GetControl<CEtUIStatic>("ID_STATIC2");
	m_pCashUnitBG		= GetControl<CEtUIStatic>("ID_STATIC0");
#endif // PRE_ADD_PETALSHOP

#ifdef PRE_CHN_OBTCASHSHOP
	m_pReserveTitle		= GetControl<CEtUIStatic>("ID_STATIC14");
	m_pReserveUnit		= GetControl<CEtUIStatic>("ID_STATIC3");
	m_pReserveUnitBG	= GetControl<CEtUIStatic>("ID_STATIC12");
#endif // PRE_CHN_OBTCASHSHOP

#ifdef PRE_ADD_CASHSHOP_CREDIT
	m_pCreditAmount		= GetControl<CEtUIStatic>("ID_STATIC_NXCREDIT");
#endif // PRE_ADD_CASHSHOP_CREDIT

	m_pCashAmount		= GetControl<CEtUIStatic>("ID_STATIC_NXCASH");
	m_pReserveAmount	= GetControl<CEtUIStatic>("ID_STATIC_RESERVE");
#if defined (PRE_ADD_NEW_MONEY_SEED) && defined (PRE_ADD_CASHSHOP_RENEWAL)
	m_pSeedAmount = GetControl<CEtUIStatic>( "ID_STATIC_SEED" );
#endif // PRE_ADD_NEW_MONEY_SEED

	m_pAboutPetalBtn	= GetControl<CEtUIButton>("ID_BUTTON_ABOUTPETAL");

#if defined PRE_ADD_PETALSHOP || defined PRE_CHN_OBTCASHSHOP
	m_pAboutPetalBtn->Show(false);
#endif // PRE_ADD_PETALSHOP

	m_pChargeBtn			= GetControl<CEtUIButton>("ID_BUTTON_NCCHARGE");

	m_pExitBtn				= GetControl<CEtUIButton>("ID_BUTTON_EXIT");

	m_pCashInvenBtn			= GetControl<CEtUIButton>("ID_BUTTON_INVEN");
	m_pCouponBtn			= GetControl<CEtUIButton>("ID_BUTTON_COUPON");
	m_pGiftBtn				= GetControl<CEtUIButton>("ID_BUTTON_PRESENT");
#ifdef PRE_ADD_LIMITED_CASHITEM
	m_pRefreshBtn			= GetControl<CEtUIButton>("ID_BUTTON_REFRESH");
#endif // PRE_ADD_LIMITED_CASHITEM

}

void CDnCashShopDlg::Process( float fElapsedTime )
{
	if (m_bShow)
	{
		if( CDnCashShopTask::IsActive() && CDnPartyTask::IsActive() && GetCashShopTask().IsOpenCashShop()  && CDnPartyTask::GetInstance().IsLocalActorEnterGateReady())
		{
			GetCashShopTask().RequestCashShopClose();
			GetCashShopTask().TerminateCashShop();

			SendCancelStage();
		}

		if( CDnCashShopTask::IsActive() && !m_bOpenGiftWindow && GetCashShopTask().GetGiftListCount() > 0 )
		{
			if( !m_pGiftBtn->IsBlink() )
			{
				m_pGiftBtn->SetBlink( true );
#ifdef PRE_MOD_CASHSHOP_GIFTBOTTON_TWINKLE_SPEEDUP
				m_pGiftBtn->SetBlendTime( 400 );		
#endif
			}
		}
		else
			m_pGiftBtn->SetBlink( false );
	}

	if (m_DisableDlgDelayTime > 0.f)
	{
		m_DisableDlgDelayTime -= fElapsedTime;
		if (m_DisableDlgDelayTime <= 0.f)
		{
			DisableAllDlgs(true, m_DisableDlgStringCache);
			m_DisableDlgStringCache.clear();
			m_DisableDlgDelayTime = 0.f;
		}
	}
#ifdef PRE_ADD_LIMITED_CASHITEM
	if( m_fRefreshCoolTime > 0.f )
	{
		m_fRefreshCoolTime -= fElapsedTime;
	}
	else
	{
		m_fRefreshCoolTime = 0.f;
		if( m_bRefreshCoolTimer )
		{
			m_pRefreshBtn->Enable( true );
			m_bRefreshCoolTimer = false;
		}
	}
#endif // PRE_ADD_LIMITED_CASHITEM
	CEtUITabDialog::Process(fElapsedTime);
}

void CDnCashShopDlg::ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg)
{
	SetCmdControlName(pControl->GetControlName());

	if (nCommand == EVENT_BUTTON_CLICKED)
	{
		if (IsCmdControl("ID_BUTTON_EXIT"))
			GetCashShopTask().RequestCashShopClose();

		if (IsCmdControl("ID_BUTTON_PRESENT"))
		{
			GetCashShopTask().RequestCashShopRecvGiftBasicInfo();
			m_bOpenGiftWindow = true;
		}

		if (IsCmdControl("ID_BUTTON_INVEN"))
			m_pInvenDlg->Show(true);

		if (IsCmdControl("ID_BUTTON_COUPON"))
			m_pCouponDlg->Show(true);

		if (IsCmdControl("ID_BUTTON_ABOUTPETAL"))
			m_pAboutPetalDlg->Show(true);

#if defined (PRE_ADD_NEW_MONEY_SEED) && defined (PRE_ADD_CASHSHOP_RENEWAL)
		if (IsCmdControl("ID_BUTTON_ABOUTSEED"))
			m_pAboutSeedDlg->Show(true);
#endif // PRE_ADD_NEW_MONEY_SEED

		if (IsCmdControl("ID_BUTTON_NCCHARGE"))
			GetCashShopTask().OpenChargeBrowser();
#ifdef PRE_ADD_LIMITED_CASHITEM
		if (IsCmdControl("ID_BUTTON_REFRESH"))
		{
			GetCashShopTask().RequestCashShopRefresh();
			m_fRefreshCoolTime = 2.f;
			m_bRefreshCoolTimer = true;
			m_pRefreshBtn->Enable( false );
		}
#endif // PRE_ADD_LIMITED_CASHITEM
	}
	else if( nCommand == EVENT_RADIOBUTTON_CHANGED )
	{
		if( m_pPetPreviewDlg && m_pPreviewDlg )
		{
			if( GetCurrentTabID() == m_nPetCategory  )
			{
				if( m_pPreviewDlg->IsShow() )
					m_pPreviewDlg->Show( false );
				if( !m_pPetPreviewDlg->IsShow() )
					m_pPetPreviewDlg->Show( true );
			}
			else
			{
				if( m_pPetPreviewDlg->IsShow() )
					m_pPetPreviewDlg->Show( false );
				if( !m_pPreviewDlg->IsShow() )
					m_pPreviewDlg->Show( true );
			}
		}
#ifdef PRE_ADD_SALE_COUPON
		if( GetCurrentTabID() == 7 )
		{
			if( GetInterface().GetCashShopDlg() )
				GetInterface().GetCashShopDlg()->SetSaleTabButtonBlink( false );
		}
#endif // PRE_ADD_SALE_COUPON
	}

	CEtUITabDialog::ProcessCommand(nCommand, bTriggeredByUser, pControl, uMsg);
}

void CDnCashShopDlg::Render(float fElapsedTime)
{
	if (m_bShow && m_hBgTexture)
	{
		// DarklairDungeonEnterDlg 참고.
		SUICoord ScreenCoord;
		SUICoord UVCoord;

		ScreenCoord.fX = 0.0f;
		ScreenCoord.fY = 0.0f;
		ScreenCoord.fWidth = 1.0f;
		ScreenCoord.fHeight = 1.0f;

		int nWidth = CEtDevice::GetInstance().Width();
		int nHeight = CEtDevice::GetInstance().Height();

		float fTemp = ( ( nHeight / 9.f ) * 16.f - nWidth ) / nWidth;
		UVCoord.fX = fTemp / 2.0f;
		UVCoord.fY = 0.0f;
		UVCoord.fWidth = 1.0f - fTemp;
		UVCoord.fHeight = 1.0f;

		CEtSprite::GetInstance().DrawSprite( (EtTexture *)m_hBgTexture->GetTexturePtr(),
			m_hBgTexture->Width(), m_hBgTexture->Height(), UVCoord, 0xFFFFFFFF, ScreenCoord, 0.0f );
	}

	CEtUITabDialog::Render(fElapsedTime);
}

void CDnCashShopDlg::Show(bool bShow)
{
	if( m_bShow == bShow )
		return;

	CEtUITabDialog::Show( bShow );
	m_pCartDlg->Show(bShow);

	std::string tabBtnID;
	bool bCheckedPetCategory = false;
	for( int i=0; i < MAX_CASHSHOP_CAT_RADIO_BTN_COUNT; ++i )
	{
		tabBtnID = FormatA( "ID_BUTTON_CATEGORY%d", i );
		CEtUIRadioButton* pTabButton = GetControl<CEtUIRadioButton>( tabBtnID.c_str() );
		if( pTabButton && wcscmp( pTabButton->GetText(), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4504 ) ) == 0 && pTabButton->IsChecked() )
		{
			bCheckedPetCategory = true;
			break;
		}
	}

	if( bShow == false )
	{
		m_pPreviewDlg->Show( false );
		m_pPreviewDlg->ResetRenderAvatar();
		m_pPetPreviewDlg->Show( false );
	}
	else
	{
		if( bCheckedPetCategory )
		{
			m_pPreviewDlg->Show( false );
			m_pPreviewDlg->InitRenderAvatar();
			m_pPetPreviewDlg->Show( true );
		}
		else
		{
			m_pPreviewDlg->Show( true );
			m_pPreviewDlg->InitRenderAvatar();
			m_pPetPreviewDlg->Show( false );
		}
	}

#ifdef PRE_ADD_CASHSHOP_REFUND_CL
	if (m_pInvenRefundDlg)
		m_pInvenRefundDlg->Show(bShow);
#endif

	drag::Command(UI_DRAG_CMD_CANCEL);
	drag::ReleaseControl();

	if (m_bShow)
	{
		float x, y;
		GetPosition(x, y);
		m_BgCoord.SetPosition(0.f - x, 0.f - y);
		m_BgCoord.SetSize((float)CEtDevice::GetInstance().Width(), (float)CEtDevice::GetInstance().Height());

#ifdef PRE_ADD_SALE_COUPON
		CheckShowSaleTabButton();
#endif // PRE_ADD_SALE_COUPON

		if (m_pCouponBtn)
		{
#if defined _CH
			m_pCouponBtn->Show(false);
#elif defined _JP
			m_pCouponBtn->Enable(false);
#else
			m_pCouponBtn->Enable(true);
#endif
		}
		m_pGiftBtn->Enable(true);
		m_bOpenGiftWindow = false;

#if defined PRE_ADD_PETALSHOP || defined PRE_CHN_OBTCASHSHOP
 		m_pCashTitle->Show(false);
 		m_pCashUnit->Show(false);
#ifdef PRE_ADD_CASHSHOP_CREDIT
		m_pCreditAmount->Show(false);
#endif // PRE_ADD_CASHSHOP_CREDIT
 		m_pCashAmount->Show(false);
 		m_pCashUnitBG->Show(false);
 
 		m_pChargeBtn->Enable(false);
		if (m_pCouponBtn)
			m_pCouponBtn->Enable(false);
#endif // PRE_ADD_PETALSHOP

#ifdef PRE_CHN_OBTCASHSHOP
		m_pReserveTitle->Show(false);
		m_pReserveUnit->Show(false);
		m_pReserveAmount->Show(false);
		m_pReserveUnitBG->Show(false);

		m_pCashInvenBtn->Enable(false);
#endif // PRE_CHN_OBTCASHSHOP

#ifdef PRE_ADD_INVEN_EXTENSION
		if (m_ReserveBuyItemNowSN > 0)
		{
			if (GetCashShopTask().PutItemIntoBuyItemNow(m_ReserveBuyItemNowSN))
				m_CommonDlgMgr.OpenPayDlg(eCashUnit_BuyItemNow, true);
			m_ReserveBuyItemNowSN = 0;
		}
#endif

#ifdef PRE_ADD_CADGE_CASH
		if( m_bCadgeButtonClick )
		{
			if( static_cast<int>( m_ReserveCadgeItemList.size() ) > 0 )
			{
				if( GetCashShopTask().SetCadgeItemList( m_nPackageSN, m_ReserveCadgeItemList ) )
				{
					eCashUnitType type = ( GetCashShopTask().IsPackageItem( m_nPackageSN ) ) ? eCashUnit_Gift_Package : eCashUnit_Gift_Cart;
					m_CommonDlgMgr.OpenSendGiftDlg( type, true );
					m_CommonDlgMgr.SetCadgeReciever( m_ReserveCadgeReciever );
				}
				m_ReserveCadgeItemList.clear();
				m_ReserveCadgeReciever = L"";
				m_nPackageSN = 0;
			}
			m_bCadgeButtonClick = false;
		}
#endif // PRE_ADD_CADGE_CASH

		UpdateCashReserveAmount();
#ifdef PRE_ADD_LIMITED_CASHITEM
		GetCashShopTask().RequestCashShopRefresh();
#endif // PRE_ADD_LIMITED_CASHITEM
	}

	if( m_pCallback )
		m_pCallback->OnUICallbackProc( GetDialogID(), bShow?EVENT_DIALOG_SHOW:EVENT_DIALOG_HIDE, NULL );
}

void CDnCashShopDlg::UpdateCashReserveAmount()
{
#ifdef PRE_MOD_PETAL_WRITE
	m_pReserveAmount->SetText( DN_INTERFACE::UTIL::GetAddCommaString( GetCashShopTask().GetUserReserve() ).c_str() );
#ifdef PRE_ADD_CASHSHOP_CREDIT
	m_pCreditAmount->SetText( DN_INTERFACE::UTIL::GetAddCommaString( GetCashShopTask().GetUserCredit() ).c_str() );
	m_pCashAmount->SetText( DN_INTERFACE::UTIL::GetAddCommaString( GetCashShopTask().GetUserPrepaid() ).c_str() );
#else // PRE_ADD_CASHSHOP_CREDIT
	m_pCashAmount->SetText( DN_INTERFACE::UTIL::GetAddCommaString( GetCashShopTask().GetUserCash() ).c_str() );
#endif // PRE_ADD_CASHSHOP_CREDIT
#ifdef PRE_ADD_NEW_MONEY_SEED
	m_pSeedAmount->SetText( DN_INTERFACE::UTIL::GetAddCommaString( GetCashShopTask().GetUserSeed() ).c_str() );
#endif // PRE_ADD_NEW_MONEY_SEED
#else // PRE_MOD_PETAL_WRITE
	std::wstring str;
#ifdef PRE_ADD_CASHSHOP_CREDIT
	str = FormatW( L"%d", GetCashShopTask().GetUserCredit() );
	m_pCreditAmount->SetText( str.c_str() );
	str = FormatW( L"%d", GetCashShopTask().GetUserPrepaid() );
	m_pCashAmount->SetText(str.c_str());
#else // PRE_ADD_CASHSHOP_CREDIT
	str = FormatW( L"%d", GetCashShopTask().GetUserCash() );
	m_pCashAmount->SetText(str.c_str());
#endif // PRE_ADD_CASHSHOP_CREDIT
	str = FormatW( L"%d", GetCashShopTask().GetUserReserve() );
	m_pReserveAmount->SetText( str.c_str() );
#if defined (PRE_ADD_NEW_MONEY_SEED) && defined (PRE_ADD_CASHSHOP_RENEWAL)
	str = FormatW( L"%d", GetCashShopTask().GetUserSeed() );
	m_pSeedAmount->SetText( str.c_str() );
#endif // PRE_ADD_NEW_MONEY_SEED

#endif // PRE_MOD_PETAL_WRITE
}

bool CDnCashShopDlg::AttachPartsToPreview(CASHITEM_SN sn, ITEMCLSID itemId, bool bCheckPackageCashRing2)
{
	if( m_pPetPreviewDlg->IsShow() )
		m_pPetPreviewDlg->Show( false );
	if( !m_pPreviewDlg->IsShow() )
		m_pPreviewDlg->Show( true );

	return m_pPreviewDlg->AttachParts(sn, itemId, bCheckPackageCashRing2);
}

void CDnCashShopDlg::DetachPartsToPreview(int classId, CASHITEM_SN sn)
{
	m_pPreviewDlg->DetachPartsWithSN(classId, sn);
}

void CDnCashShopDlg::UpdateGiftReceiverInfo(const SCCashShopCheckReceiver& info)
{
	m_CommonDlgMgr.UpdateGiftReceiverInfo(info);
}

void CDnCashShopDlg::UpdateRecvGiftList()
{
	m_CommonDlgMgr.UpdateRecvGiftList();
}

void CDnCashShopDlg::UpdateCart(eRetCartAction action, const SCartActionParam& param)
{
	if (m_pCartDlg)
		m_pCartDlg->UpdateItem(action, param);
}

#ifdef PRE_ADD_INVEN_EXTENSION
void CDnCashShopDlg::ReserveBuyItemNowOnNextOpened(CASHITEM_SN sn)
{
	m_ReserveBuyItemNowSN = sn;
}
#endif

void CDnCashShopDlg::OpenBuyCart(bool bShow)
{
	if (bShow)
	{
		if (GetCashShopTask().IsCartEmpty(eCashUnit_Cart))
		{
			HandleCashShopError(eERRCS_CART_EMPTY, false);
			return;
		}

		if (GetCashShopTask().IsCartPurchasble(eCashUnit_Cart) == false)
			return;

		if (IsCartBuyable() == false)
			return;

		m_CommonDlgMgr.OpenPayDlg(eCashUnit_Cart, false);
	}
	else
	{
		m_CommonDlgMgr.ClosePayDlg();
	}
}

void CDnCashShopDlg::OpenBuyPreviewCart(bool bShow)
{
	if (bShow)
	{
		if (GetCashShopTask().IsCartEmpty(eCashUnit_PreviewCart, GetCashShopTask().GetPreviewCartClassId()))
		{
			HandleCashShopError(eERRCS_CART_EMPTY, false);
			return;
		}

		if (GetCashShopTask().IsCartPurchasble(eCashUnit_PreviewCart, GetCashShopTask().GetPreviewCartClassId()) == false)
			return;

		if (IsPreviewCartBuyable() == false)
			return;

		if (CDnActor::s_hLocalActor)
		{
			int localActorLevel = CDnActor::s_hLocalActor->GetLevel();
			const CART_ITEM_LIST* pList = GetCashShopTask().GetPreviewCartList(GetCashShopTask().GetPreviewCartClassId());
			if (pList == NULL)
				return;

			if (GetCashShopTask().HandleItemLevelLimit(*pList, localActorLevel, LEVEL_ALERT_PREVIEWCART_DLG, this) == false)
				return;
		}
		else
		{
			return;
		}

		m_CommonDlgMgr.OpenPayDlg(eCashUnit_PreviewCart, false);
	}
	else
	{
		m_CommonDlgMgr.ClosePayDlg();
	}
}

bool CDnCashShopDlg::IsPreviewCartBuyable() const
{
	std::vector<CASHITEM_SN> unableBuyItemList;
	const CART_ITEM_LIST* previewCart = CDnCashShopTask::GetInstance().GetBuyPreviewCartList();
	if (previewCart == NULL)
	{
		HandleCashShopError(eERRCS_CART_UNABLE_BUY_INVALID_CART, false);
		return false;
	}

	if (GetCashShopTask().IsCartItemBuyable(*previewCart, unableBuyItemList) == false)
	{
		if (unableBuyItemList.empty())
		{
			HandleCashShopError(eERRCS_CART_UNABLE_BUY_INVALID_CART, false);
		}
		else
		{
			std::wstring msg = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4779);	// UISTRING : 구입할 수 없는 상품이 장바구니에 있습니다.
			ShowGiftFailDlg(true, msg.c_str(), unableBuyItemList, MB_OK);
		}
		return false;
	}

	return true;
}

bool CDnCashShopDlg::IsCartBuyable() const
{
	std::vector<CASHITEM_SN> unableBuyItemList;
	if (GetCashShopTask().IsCartItemBuyable(CDnCashShopTask::GetInstance().GetCartList(), unableBuyItemList) == false)
	{
		if (unableBuyItemList.empty())
		{
			HandleCashShopError(eERRCS_CART_UNABLE_BUY_INVALID_CART, false);
		}
		else
		{
			std::wstring msg = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4779);	// UISTRING : 구입할 수 없는 상품이 장바구니에 있습니다.
			ShowGiftFailDlg(true, msg.c_str(), unableBuyItemList, MB_OK);
		}
		return false;
	}

	return true;
}

eCashShopError CDnCashShopDlg::IsCartGiftable(eCashUnitType type) const
{
	std::vector<CASHITEM_SN> unableGiftItemList;
	eCashShopError err = GetCashShopTask().IsItemGiftable(type, unableGiftItemList);
	if (err != eERRCS_NONE)
	{
		if (unableGiftItemList.empty())
		{
			HandleCashShopError(eERRCS_GIFT_UNABLE_INVALID_CART, false);
		}
		else
		{
			std::wstring msg;
			if (err == eERRCS_GIFT_UNABLE_CLASS)
				msg = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4660); // UISTRING : 직업이 달라서 사용할 수 없는 아이템입니다.
			else
				msg = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4728); // UISTRING : 선물할 수 없는 상품이 장바구니에 있습니다.
			ShowGiftFailDlg(true, msg.c_str(), unableGiftItemList, MB_OK);
			unableGiftItemList.clear();
		}
	}

	return err;
}

void CDnCashShopDlg::ShowGiftFailDlg(bool bShow, const std::wstring& msg, const std::vector<CASHITEM_SN>& itemList, UINT uType, int callBackDlgID, CEtUICallback* pCall) const
{
	if (m_pGiftFailDlg)
	{
		m_pGiftFailDlg->SetInfo(msg.c_str(), itemList, uType, callBackDlgID, pCall);
		m_pGiftFailDlg->Show(bShow);
	}
}

void CDnCashShopDlg::ShowCashShopMsgBox(const std::wstring& firstmsg, const std::wstring& secondmsg, int nID, CEtUICallback* pCallBack)
{
	if (m_pCashShopMsgBox)
	{
		if( !firstmsg.empty() )
			m_pCashShopMsgBox->SetMsgBoxText(CDnCashShopMessageBox::eMain, firstmsg);		
		if( !secondmsg.empty() )
			m_pCashShopMsgBox->SetMsgBoxText(CDnCashShopMessageBox::eSub, secondmsg);		
		m_pCashShopMsgBox->SetMsgBox(nID, pCallBack);
	}
}

void CDnCashShopDlg::GiftCart(bool bClear)
{
	if (IsCartGiftable(eCashUnit_Gift_Cart) != eERRCS_NONE)
		return;

	m_CommonDlgMgr.OpenSendGiftDlg(eCashUnit_Gift_Cart, bClear);
}

void CDnCashShopDlg::GiftPreviewCart(bool bClear)
{
	if (IsCartGiftable(eCashUnit_Gift_PreviewCart) != eERRCS_NONE)
		return;
	m_CommonDlgMgr.OpenSendGiftDlg(eCashUnit_Gift_PreviewCart, bClear);
}

void CDnCashShopDlg::OpenGiftReply(const WCHAR* pName, int giftDBID)
{
	m_CommonDlgMgr.OpenGiftReplyDlg(pName);
}

void CDnCashShopDlg::ShowGiftPayDlg(bool bShow, eCashUnitType type)
{
	if (bShow)
	{
		bool bError = false;
		if (type == eCashUnit_GiftItemNow || type == eCashUnit_Gift_Package)
			bError = (GetCashShopTask().GetBuyItemNow().IsEmpty());
		else if (type == eCashUnit_Gift_PreviewCart)
			bError = GetCashShopTask().IsCartEmpty(type, GetCashShopTask().GetPreviewCartClassId());
		else if (type == eCashUnit_PreviewCart)
			bError = GetCashShopTask().IsCartEmpty(type);

		if (bError)
		{
			HandleCashShopError(eERRCS_CART_EMPTY, false);
			return;
		}

		m_CommonDlgMgr.OpenPayDlg(type, false);
	}
	else
	{
		m_CommonDlgMgr.ClosePayDlg();
	}
}

void CDnCashShopDlg::ShowCouponDlg(bool bShow)
{
	m_pCouponDlg->Show(bShow);
}

void CDnCashShopDlg::ShowRecvGiftDlg(bool bShow)
{
	m_CommonDlgMgr.ShowRecvGiftDlg(bShow);

#if defined(PRE_ADD_68286)
	//단축 아이콘으로 선물함을 열었다면, 캐쉬창은 안 열려 있음..
	if (IsShow() == false)
	{
		if (bShow)
		{
			if (CDnActor::s_hLocalActor)
			{
				CDnLocalPlayerActor* pLocalActor = ((CDnLocalPlayerActor*)CDnActor::s_hLocalActor.GetPointer());

				if (pLocalActor)
				{
					//자동 달리기? 따라가기?? 초기화.
					if (pLocalActor->IsFollowing())
					{
						pLocalActor->ResetAutoRun();
						pLocalActor->CmdStop("Stand");
					}
				}

				CDnLocalPlayerActor::LockInput(true);
			}
		}
		else
		{
			CDnLocalPlayerActor::LockInput(false);
		}
	}
#endif // PRE_ADD_68286

}

void CDnCashShopDlg::ClearPreviewCart(int classId)
{
	if( m_pPreviewDlg->IsShow() )
		m_pPreviewDlg->ClearParts( classId );
	else if( m_pPetPreviewDlg->IsShow() )
		m_pPetPreviewDlg->ResetBasePet();
}

// todo : callback handler
void CDnCashShopDlg::HandleCashShopError(eCashShopError code, bool bShowCode, CEtUIDialog* pCallbackDlg) const
{
	std::wstring whole, errString;
	switch(code)
	{
	case eERRCS_CART_MAX:
	case eERRCS_PREVIEWCART_MAX:
	case eERRCS_PREVIEWCART_CHR_ID:
		{
			errString = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4720);	// UISTRING : 해당 품목을 카트에 담을 수 없습니다
		}
		break;
	case eERRCS_PREVIEWCART_CLEAR_INVALID_CLASS:
		{
			errString = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4721);	// UISTRING : 입은 품목을 비울 수 없습니다
		}
		break;
	case eERRCS_CART_NOT_EXIST:
		{
			errString = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4722);	// UISTRING : 카트 오류입니다
		}
		break;
	case eERRCS_CART_EMPTY:
		{
			errString = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4723);	// UISTRING : 카트가 비어 있습니다
		}
		break;
	case eERRCS_PREVIEWCART_ALREADY_EXIST:
		{
			errString = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4724);	// UISTRING : 이미 입고 있는 아이템입니다
		}
		break;
	case eERRCS_CANT_OPEN_CHARGEBROWSER_ATTACH_IE_EVENT_FAIL:
	case eERRCS_CANT_OPEN_CHARGEBROWSER_TYPE_FAIL:
	case eERRCS_CANT_OPEN_CHARGEBROWSER_GET_ACCOUNT_FAIL:
	case eERRCS_CANT_OPEN_CHARGEBROWSER_NAVIGATE_FAIL:
	case eERRCS_CANT_OPEN_CHARGEBROWSER_SIZE_FAIL:
	case eERRCS_CANT_OPEN_CHARGEBROWSER_COCREATE_FAIL:
	case eERRCS_CANT_OPEN_CHARGEBROWSER_D3D:
	case eERRCS_CANT_OPEN_CHARGEBROWSER:
		{
			errString = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4776 );	// UISTRING : 캐시충전창을 열 수 없습니다
		}
		break;
	case eERRCS_PREVIEWCART_ALREADY_EQUIP:
		{
			errString = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4788 );	// UISTRING : 현재 캐릭터가 입고 있는 상품입니다.
		}
		break;
	case eERRCS_NO_ALL_ITEM_PERIOD_SELECTED:
		{
			errString = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4757);	// UISTRING : 기간을 선택하셔야 구매할 수 있습니다.
		}
		break;
	case eERRCS_NO_ALL_ITEM_ABILITY_SELECTED:
		{
			errString = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4571);	// UISTRING : 능력치를 선택하셔야 구매할 수 있습니다.
		}
		break;
	case eERRCS_NO_ALL_ITEM_SKILL_SELECTED:
		{
			errString = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4857);	// UISTRING : 스킬을 선택하셔야 구매할 수 있습니다.
		}
		break;
	case eERRCS_RECV_GIFT_PACKET:
		{
			errString = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4764);	// UISTRING : 선물을 받는데 실패했습니다. 잠시 후 다시 시도해 주세요.
		}
		break;
	case eERRCS_CART_UNABLE_BUY_COUNT:
		{
			errString = FormatW(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4572), CARTLISTMAX);	// UISTRING : 카트에 담긴 아이템은 %d개까지 구입할 수 있습니다.
		}
		break;
	case eERRCS_PREVIEWCART_UNABLE_BUY_COUNT:
		{
			errString = FormatW(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4573), PREVIEWCARTLISTMAX);	// UISTRING : 미리보기에 입힌 아이템은 %d개까지 구입할 수 있습니다.
		}
		break;
	case eERRCS_CART_UNABLE_BUY_DLG_INFO_FAIL:
	case eERRCS_NO_DIALOG:
	case eERRCS_NO_PACKAGE_ITEM_INFO:	
	case eERRCS_REFUND_ITEM_ERROR:
	default:
		{
			errString = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4725);	// UISTRING : 캐시샵 동작 오류입니다
			bShowCode = true;
		}
	}

	if (bShowCode)
		whole = FormatW(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 4726), errString.c_str(), code);	// UISTRING : %s 캐시샵을 종료 후 다시 시작하여 주십시요.(코드번호:%d)
	else
		whole = errString;

	GetInterface().MessageBox(whole.c_str(), MB_OK);
}

void CDnCashShopDlg::OnCloseCashShopDlg()
{
	GetInterface().FadeDialog( 0xff000000, 0x00000000, 0.f, NULL, true );
	GetInterface().OpenBaseDialog();
	Show(false);
	EnableDummyModalDlg(false);

	if (CDnWorld::GetInstance().IsActive())
		CDnWorld::GetInstance().PlayBGM();

	CEtSoundEngine::GetInstance().FadeVolume( NULL, 1.f, CGlobalInfo::GetInstance().m_fFadeDelta, false );
}

void CDnCashShopDlg::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg )
{
	if (pControl)
		SetCmdControlName( pControl->GetControlName() );

	if (nCommand == EVENT_BUTTON_CLICKED)
	{
		if (IsCmdControl("ID_BUTTON_OK"))
		{
			if (nID == LEVEL_ALERT_CART_DLG)			m_CommonDlgMgr.OpenPayDlg(eCashUnit_Cart, false);
			if (nID == LEVEL_ALERT_PREVIEWCART_DLG)		m_CommonDlgMgr.OpenPayDlg(eCashUnit_PreviewCart, false);
		}
	}

	switch( nID ) 
	{
		case FADE_DIALOG:
		{
			if( nCommand == EVENT_FADE_COMPLETE )
			{
				if (IsShow() == false)
				{
					Show(true);
					GetCashShopTask().OnOpenCashShop();
					GetInterface().FadeDialog( 0xff000000, 0x00000000, 0.f, NULL, true );
				}
				else
				{
					OnCloseCashShopDlg();
				}
			}
		}
		break;
	}
}

bool CDnCashShopDlg::IsOpenCashShopInvenDlg() const
{
	return m_pInvenDlg->IsShow();
}

bool CDnCashShopDlg::IsOpenCashShopRecvGiftDlg() const
{
	return m_CommonDlgMgr.IsOpenDlg(CDnCashShopCommonDlgMgr::RECV_GIFT_DLG);
}

void CDnCashShopDlg::DisableAllDlgs(bool bEnable, const std::wstring& text, float delayTime)
{
	if (bEnable)
	{
		if (delayTime <= 0.f)
		{
			GetInterface().CloseMessageBox();
			GetInterface().MessageBox(text.c_str(), MB_OK, CASHCHARGE_ALERT_DLG, this, true, false, true, false);
		}
		else
		{
			m_DisableDlgDelayTime = delayTime;
			m_DisableDlgStringCache = text;
		}
	}
	else
	{

		GetInterface().CloseMessageBox();
		m_DisableDlgDelayTime = 0.f;
		m_DisableDlgStringCache.clear();
	}
}

eRetPreviewChange CDnCashShopDlg::ChangePreviewAvatar(int classId,bool bForce)
{
	if (m_pPreviewDlg)
		return m_pPreviewDlg->ChangePreviewAvatar(classId,bForce);

	return eRETPREVIEW_ERROR;
}

void CDnCashShopDlg::ChangeVehicleParts(int nClassID,int nPartClassID, bool bDefault)
{
	if (m_pPreviewDlg)
		m_pPreviewDlg->ChangeVehicleParts(nClassID,nPartClassID,bDefault);
}

void CDnCashShopDlg::MoveCategory(bool bNext)
{
	int i = 0;
	int currentTabID = GetCurrentTabID();
	if (currentTabID < 0 || currentTabID >= UINT_MAX)
	{
		_ASSERT(0);
		return;
	}

	if (bNext)
	{
		for (i = (currentTabID + 1) % MAX_CASHSHOP_CAT_RADIO_BTN_COUNT; i != currentTabID; i = (i + 1) % MAX_CASHSHOP_CAT_RADIO_BTN_COUNT)
		{
			CEtUIRadioButton* pBtn = m_pCategoryBtns[i];
			if (pBtn && pBtn->IsShow() && pBtn->IsEnable())
			{
				pBtn->SetChecked(true);
				break;
			}
		}
	}
	else
	{
		i = currentTabID;
		for (i = ((--i) < 0) ? MAX_CASHSHOP_CAT_RADIO_BTN_COUNT - 1 : i; i != currentTabID;)
		{
			CEtUIRadioButton* pBtn = m_pCategoryBtns[i];
			if (pBtn && pBtn->IsShow() && pBtn->IsEnable())
			{
				pBtn->SetChecked(true);
				break;
			}
			i = ((--i) < 0) ? MAX_CASHSHOP_CAT_RADIO_BTN_COUNT - 1 : i;
		}
	}
}

bool CDnCashShopDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsShow() )
	{
		return false;
	}

	switch( uMsg )
	{
	case WM_KEYDOWN:
		{
			if (wParam == VK_OEM_3)
				MoveCategory(GetKeyState(VK_SHIFT) >= 0);
		}
		break;
	}

	return CEtUITabDialog::MsgProc( hWnd, uMsg, wParam, lParam );
}

void CDnCashShopDlg::EnableDummyModalDlg(bool bSet)
{
	if (m_pDummyModalDlg)
		m_pDummyModalDlg->Show(bSet);
}

#ifdef PRE_ADD_CASHSHOP_REFUND_CL
void CDnCashShopDlg::UpdateRefundInven()
{
	//m_pInvenRefundDlg->Upda
}
#endif

void CDnCashShopDlg::ChangePetPreviewAvatar( int nItemID )
{
	if( m_pPreviewDlg->IsShow() )
		m_pPreviewDlg->Show( false );
	if( !m_pPetPreviewDlg->IsShow() )
		m_pPetPreviewDlg->Show( true );

	if( m_pPetPreviewDlg )
		m_pPetPreviewDlg->SetPetAvatar( nItemID );
}

void CDnCashShopDlg::ChangePetPreviewParts( int nPartsType, int nItemID )
{
	if( m_pPetPreviewDlg )
		m_pPetPreviewDlg->SetPetAvatarParts( nPartsType, nItemID );
}

void CDnCashShopDlg::ChangePetPreviewColor( eItemTypeEnum eItemType, DWORD dwColor )
{
	if( m_pPetPreviewDlg )
		m_pPetPreviewDlg->SetPetAvatarColor( eItemType, dwColor );
}

bool CDnCashShopDlg::IsExistPetAvatar()
{
	bool bExistPetAvatar = false;

	if( m_pPetPreviewDlg )
		bExistPetAvatar = m_pPetPreviewDlg->GetPetAvatar() != NULL ? true : false; 

	return bExistPetAvatar;
}

void CDnCashShopDlg::OpenBuyPetPreviewCart()
{
	const CART_ITEM_LIST* pList = GetCashShopTask().GetPreviewCartList( PET_CLASS_ID );
	if( pList == NULL ) return;
	if( pList->size() == 0 )
	{
		HandleCashShopError( eERRCS_CART_EMPTY, false );
		return;
	}

	if( pList->size() > PREVIEWCARTLISTMAX )
	{
		HandleCashShopError( eERRCS_PREVIEWCART_UNABLE_BUY_COUNT, false );
		return;
	}

	std::vector<CASHITEM_SN> unableBuyItemList;
	if( GetCashShopTask().IsCartItemBuyable( *pList, unableBuyItemList ) == false )
	{
		if( unableBuyItemList.empty() )
		{
			HandleCashShopError( eERRCS_CART_UNABLE_BUY_INVALID_CART, false );
		}
		else
		{
			std::wstring msg = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4779 );	// UISTRING : 구입할 수 없는 상품이 장바구니에 있습니다.
			ShowGiftFailDlg( true, msg.c_str(), unableBuyItemList, MB_OK );
		}
		return;
	}

	m_CommonDlgMgr.OpenPayDlg( eCashUnit_PreviewCart, false );
}

void CDnCashShopDlg::SwapPreview( bool bShowChar )
{
	if( bShowChar )
	{
		if( m_pPetPreviewDlg->IsShow() )
			m_pPetPreviewDlg->Show( false );
		if( !m_pPreviewDlg->IsShow() )
			m_pPreviewDlg->Show( true );
	}
	else
	{
		if( m_pPreviewDlg->IsShow() )
			m_pPreviewDlg->Show( false );
		if( !m_pPetPreviewDlg->IsShow() )
			m_pPetPreviewDlg->Show( true );
	}
}

void CDnCashShopDlg::DetachPartsToPetPreview( int nPartsSlot )
{
	m_pPetPreviewDlg->DetachPartsWithType( nPartsSlot );
}

#ifdef PRE_ADD_SALE_COUPON

void CDnCashShopDlg::CheckShowSaleTabButton( bool bSaleCouponGift )
{
	std::string tabBtnID;
	CEtUIRadioButton* pSaleTabButton = NULL;
	for( int i=0; i<MAX_CASHSHOP_CAT_RADIO_BTN_COUNT; ++i )
	{
		tabBtnID = FormatA( "ID_BUTTON_CATEGORY%d", i );
		CEtUIRadioButton* pTabButton = GetControl<CEtUIRadioButton>( tabBtnID.c_str() );
		if( pTabButton && wcscmp( pTabButton->GetText(), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1000039876 ) ) == 0 )
		{
			pSaleTabButton = pTabButton;
			break;
		}
	}

	if( GetCashShopTask().CheckCoupon() )
	{
		if( pSaleTabButton )
		{

#ifdef PRE_ADD_CASHSHOP_RENEWAL
	// ...
#else
			if( bSaleCouponGift )
				pSaleTabButton->SetBlink( true );

			pSaleTabButton->Show( true );
#endif // PRE_ADD_CASHSHOP_RENEWAL

			for( int i=0; i<static_cast<int>( m_pCategoryDlgs.size() ); i++ )
			{
				if( m_pCategoryDlgs[i] == NULL )
					continue;

				if( m_pCategoryDlgs[i]->GetMainTabNum() == 7 )
				{
					CEtUIDialogGroup& groupTabDialog = m_pCategoryDlgs[i]->GetGroupTabDialog();
					CEtUIDialogGroup::DIALOG_VEC &vecDialog = groupTabDialog.GetDialogList();

#ifdef PRE_ADD_SALE_COUPON_CATEGORY
					int nSubCategory = 0;
#endif // PRE_ADD_SALE_COUPON_CATEGORY
					for( int j=0; j<static_cast<int>( vecDialog.size() ); j++ )
					{
						CEtUIDialogGroup::ID_DIALOG_PAIR &id_dlg_pair = vecDialog[j];
						if( id_dlg_pair.second == NULL ) continue;
						CDnCashShopGoodsDlg* pCashShopGoodsDlg = dynamic_cast<CDnCashShopGoodsDlg*>( id_dlg_pair.second );
						if( pCashShopGoodsDlg )
						{
							pCashShopGoodsDlg->Update();
#ifdef PRE_ADD_SALE_COUPON_CATEGORY
							if( pCashShopGoodsDlg->GetDialogID() == m_nSaleCouponSubCategory )
								nSubCategory = j;
#endif // PRE_ADD_SALE_COUPON_CATEGORY

						}
					}
#ifdef PRE_ADD_SALE_COUPON_CATEGORY
					CEtUIRadioButton* pBtn = m_pCategoryDlgs[i]->GetTabControl( nSubCategory );
					if( pBtn && pBtn->IsShow() && pBtn->IsEnable() )
					{
						pBtn->SetChecked( true );
						break;
					}
#endif // PRE_ADD_SALE_COUPON_CATEGORY
				}
			}

			if( m_bSaleCouponOpen )
			{
				CEtUIRadioButton* pBtn = m_pCategoryBtns[7];
				if( pBtn && pBtn->IsShow() && pBtn->IsEnable() )
				{
					pBtn->SetChecked( true );
				}
				m_bSaleCouponOpen = false;
			}
		}
	}
	else
	{
		if( pSaleTabButton )
			pSaleTabButton->Show( false );

		int currentTabID = GetCurrentTabID();
		if( currentTabID == 7 && m_pCategoryBtns.size() > 0 )
		{
			CEtUIRadioButton* pBtn = m_pCategoryBtns[0];
			if( pBtn && pBtn->IsShow() && pBtn->IsEnable() )
			{
				pBtn->SetChecked( true );
			}
		}
	}
}

void CDnCashShopDlg::SetSaleTabButtonBlink( bool bBlink )
{
	std::string tabBtnID;
	CEtUIRadioButton* pSaleTabButton = NULL;
	for( int i=0; i<MAX_CASHSHOP_CAT_RADIO_BTN_COUNT; ++i )
	{
		tabBtnID = FormatA( "ID_BUTTON_CATEGORY%d", i );
		CEtUIRadioButton* pTabButton = GetControl<CEtUIRadioButton>( tabBtnID.c_str() );
		if( pTabButton && wcscmp( pTabButton->GetText(), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1000039876 ) ) == 0 )
		{
			pSaleTabButton = pTabButton;
			break;
		}
	}

	if( pSaleTabButton )
	{
		if( pSaleTabButton->IsShow() )
			pSaleTabButton->SetBlink( bBlink );
	}
}

#endif // PRE_ADD_SALE_COUPON

#ifdef PRE_ADD_LIMITED_CASHITEM

void CDnCashShopDlg::UpdateCurrentGoodsDlg()
{
	for( int i=0; i<static_cast<int>( m_pCategoryDlgs.size() ); i++ )
	{
		if( m_pCategoryDlgs[i] == NULL )
			continue;

		if( m_pCategoryDlgs[i]->GetMainTabNum() == GetCurrentTabID() )
		{
			CEtUIDialogGroup& groupTabDialog = m_pCategoryDlgs[i]->GetGroupTabDialog();
			CEtUIDialogGroup::DIALOG_VEC &vecDialog = groupTabDialog.GetDialogList();

			for( int j=0; j<static_cast<int>( vecDialog.size() ); j++ )
			{
				CEtUIDialogGroup::ID_DIALOG_PAIR &id_dlg_pair = vecDialog[j];
				if( id_dlg_pair.second == NULL ) continue;
			
				CDnCashShopGoodsDlg* pCashShopGoodsDlg = dynamic_cast<CDnCashShopGoodsDlg*>( id_dlg_pair.second );
				if( pCashShopGoodsDlg )
					pCashShopGoodsDlg->Update();
			}
		}
	}
}

#endif // PRE_ADD_LIMITED_CASHITEM

#ifdef PRE_ADD_CADGE_CASH

void CDnCashShopDlg::SetReserveCadgeItemList( std::wstring strCadgeReciever, int nMailID, int nPackageSN, int nCount, TWishItemInfo* pItemList )
{
	if( nCount == 0 || pItemList == NULL )
		return;

	m_ReserveCadgeItemList.clear();
	for( int i=0; i<nCount; i++ )
	{
		SCashShopCartItemInfo sCartItemInfo;
		sCartItemInfo.presentSN = sCartItemInfo.selectedSN = pItemList[i].nItemSN;
		sCartItemInfo.ability = pItemList[i].nItemID;
		sCartItemInfo.nOptionIndex = pItemList[i].nOptionIndex;
		m_ReserveCadgeItemList.push_back( sCartItemInfo );
	}

	m_ReserveCadgeReciever = strCadgeReciever;
	m_nMailID = nMailID;
	m_nPackageSN = nPackageSN;
}

#endif // PRE_ADD_CADGE_CASH


#ifdef PRE_ADD_CASHSHOP_ACTOZ
void CDnCashShopDlg::ShowConfirmRefund( CDnItem * pItem )
{
	if( m_pConfirmRefundDlg )
	{		
		m_pConfirmRefundDlg->Show( true );
		m_pConfirmRefundDlg->SetItem( pItem );
	}

}
#endif // PRE_ADD_CASHSHOP_ACTOZ