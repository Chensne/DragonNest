#include "stdafx.h"

#ifdef PRE_ADD_CASHSHOP_RENEWAL

#include "EtPrimitiveDraw.h"
#include "DnCashShopRenewalDlg.h"
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

#include "DnInterfaceString.h"

#include "DnCashShopMainBannerDlg.h"


#include "DnCashShopMenuListDlg.h"



#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnCashShopRenewalDlg::CDnCashShopRenewalDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback)
			: CDnCashShopDlg(dialogType, pParentDialog, nID, pCallback)
			,m_pCSMainBanner(NULL)
			,m_pMenuListDlg(NULL)
			,m_bShowIngameBannerItem(false)
{
}

CDnCashShopRenewalDlg::~CDnCashShopRenewalDlg()
{	
	SAFE_DELETE( m_pMenuListDlg );
}

void CDnCashShopRenewalDlg::Initialize(bool bShow)
{
	std::string fileName;
#if defined(PRE_ADD_CASHSHOP_REFUND_CL)	
	fileName.assign( CDnCashShopTask::GetInstance().IsCashShopMode(eCSMODE_REFUND) ? "CS_Base_refund.ui" : "CS_Base.ui" );
#elif defined(PRE_ADD_NEW_MONEY_SEED)
	fileName.assign( "CS_Base_Seed.ui" );
#else	
	fileName.assign( "CS_Base.ui" );
#endif

	CDnCashShopDlg::Initialize( fileName.c_str(), bShow );	
}

#define MAX_CASHSHOP_CAT_RADIO_BTN_COUNT 8

void CDnCashShopRenewalDlg::InitialUpdate()
{
	CDnCashShopDlg::InitialUpdate();

	m_EditBoxSearch = GetControl<CEtUIIMEEditBox>("ID_IMEEDITBOX_NAME");
	m_pStaticText = GetControl<CEtUIStatic>("ID_TEXT_SEARCH");
	


	m_pMenuListDlg = new CDnCashShopMenuListDlg( UI_TYPE_CHILD, this );
	m_pMenuListDlg->Initialize( true );

	
}

int CDnCashShopRenewalDlg::GetSortStringNum(eCashShopEtcSortType type) const
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


void CDnCashShopRenewalDlg::Process( float fElapsedTime )
{	
	CDnCashShopDlg::Process(fElapsedTime);
}

void CDnCashShopRenewalDlg::ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg)
{
	SetCmdControlName(pControl->GetControlName());

	if (nCommand == EVENT_BUTTON_CLICKED)
	{
		if( IsCmdControl("ID_BT_SEARCH") )
		{	
			// 상품검색시 메뉴리스트를 초기화함.
			if( m_pMenuListDlg )
				m_pMenuListDlg->ResetMenuList();

			SearchItems( m_EditBoxSearch->GetText() ); // 상품검색.
			//GetCashShopTask().SearchItems( std::wstring(m_EditBoxSearch->GetText()) );
		}	
	}
	
	//else if( nCommand == EVENT_EDITBOX_IME_CHANGE )
	//{
	//	if( IsCmdControl("ID_IMEEDITBOX_NAME") )
	//	{			
	//		int size = (int)wcslen( m_EditBoxSearch->GetText() );
	//		if( size > 0 )
	//			m_pStaticText->Show( false );
	//		else
	//			m_pStaticText->Show( true );
	//	}
	//}

	else if( nCommand == EVENT_EDITBOX_FOCUS )
	{
		if( IsCmdControl("ID_IMEEDITBOX_NAME") )
			m_pStaticText->Show( false );
	}

	else if( nCommand == EVENT_EDITBOX_RELEASEFOCUS )
	{
		if( IsCmdControl("ID_IMEEDITBOX_NAME") )
		{
			m_EditBoxSearch->ClearText();
			m_pStaticText->Show( true );
		}
	}

	else if( nCommand == EVENT_EDITBOX_STRING )
	{
		if( IsCmdControl("ID_IMEEDITBOX_NAME") )
		{
			// 상품검색시 메뉴리스트를 초기화함.
			if( m_pMenuListDlg )
				m_pMenuListDlg->ResetMenuList();

			SearchItems( m_EditBoxSearch->GetText() ); // 상품검색.			
			//GetCashShopTask().SearchItems( std::wstring(m_EditBoxSearch->GetText()) );
		}
	}

	/*else if (nCommand == EVENT_COMBOBOX_SELECTION_CHANGED ) // && bTriggeredByUser)
	{
		if (IsCmdControl("ID_COMBOBOX_JOBSORT"))
		{
			SComboBoxItem* pItem = m_pJobSortComboBox->GetSelectedItem();
			if (pItem)
			{	
				int classId = 0;				
				m_pJobSortComboBox->GetSelectedValue(classId); // job.			

				ChangeSortOrder( true, classId );

				if (classId > 0)
					GetCashShopTask().ChangePreviewAvatar(classId);

				focus::ReleaseControl();
			}
			return;
		}

		if (IsCmdControl("ID_COMBOBOX_ETCSORT"))
		{
			SComboBoxItem* pItem = m_pEtcSortComboBox->GetSelectedItem(); // Etc.
			if (pItem)
			{		
				ChangeSortOrder( false, pItem->nValue );

				focus::ReleaseControl();
				return;
			}
		}
	}*/


//	if (nCommand == EVENT_BUTTON_CLICKED)
//	{
//		if (IsCmdControl("ID_BUTTON_EXIT"))
//			GetCashShopTask().RequestCashShopClose();
//
//		if (IsCmdControl("ID_BUTTON_PRESENT"))
//		{
//			GetCashShopTask().RequestCashShopRecvGiftBasicInfo();
//			m_bOpenGiftWindow = true;
//		}
//
//		if (IsCmdControl("ID_BUTTON_INVEN"))
//			m_pInvenDlg->Show(true);
//
//		if (IsCmdControl("ID_BUTTON_COUPON"))
//			m_pCouponDlg->Show(true);
//
//		if (IsCmdControl("ID_BUTTON_ABOUTPETAL"))
//			m_pAboutPetalDlg->Show(true);
//
//		if (IsCmdControl("ID_BUTTON_NCCHARGE"))
//			GetCashShopTask().OpenChargeBrowser();
//#ifdef PRE_ADD_LIMITED_CASHITEM
//		if (IsCmdControl("ID_BUTTON_REFRESH"))
//		{
//			GetCashShopTask().RequestCashShopRefresh();
//			m_fRefreshCoolTime = 2.f;
//			m_bRefreshCoolTimer = true;
//			m_pRefreshBtn->Enable( false );
//		}
//#endif // PRE_ADD_LIMITED_CASHITEM
//	}
//	else if( nCommand == EVENT_RADIOBUTTON_CHANGED )
//	{
//		if( m_pPetPreviewDlg && m_pPreviewDlg )
//		{
//			if( GetCurrentTabID() == m_nPetCategory  )
//			{
//				if( m_pPreviewDlg->IsShow() )
//					m_pPreviewDlg->Show( false );
//				if( !m_pPetPreviewDlg->IsShow() )
//					m_pPetPreviewDlg->Show( true );
//			}
//			else
//			{
//				if( m_pPetPreviewDlg->IsShow() )
//					m_pPetPreviewDlg->Show( false );
//				if( !m_pPreviewDlg->IsShow() )
//					m_pPreviewDlg->Show( true );
//			}
//		}
//#ifdef PRE_ADD_SALE_COUPON
//		if( GetCurrentTabID() == 7  )
//		{
//			if( GetInterface().GetCashShopDlg() )
//				GetInterface().GetCashShopDlg()->SetSaleTabButtonBlink( false );
//		}
//#endif // PRE_ADD_SALE_COUPON
//	}

	CDnCashShopDlg::ProcessCommand(nCommand, bTriggeredByUser, pControl, uMsg);
}

void CDnCashShopRenewalDlg::Render(float fElapsedTime)
{
	//if (m_bShow && m_hBgTexture)
	//{
	//	// DarklairDungeonEnterDlg 참고.
	//	SUICoord ScreenCoord;
	//	SUICoord UVCoord;

	//	ScreenCoord.fX = 0.0f;
	//	ScreenCoord.fY = 0.0f;
	//	ScreenCoord.fWidth = 1.0f;
	//	ScreenCoord.fHeight = 1.0f;

	//	int nWidth = CEtDevice::GetInstance().Width();
	//	int nHeight = CEtDevice::GetInstance().Height();

	//	float fTemp = ( ( nHeight / 9.f ) * 16.f - nWidth ) / nWidth;
	//	UVCoord.fX = fTemp / 2.0f;
	//	UVCoord.fY = 0.0f;
	//	UVCoord.fWidth = 1.0f - fTemp;
	//	UVCoord.fHeight = 1.0f;

	//	CEtSprite::GetInstance().DrawSprite( (EtTexture *)m_hBgTexture->GetTexturePtr(),
	//		m_hBgTexture->Width(), m_hBgTexture->Height(), UVCoord, 0xFFFFFFFF, ScreenCoord, 0.0f );
	//}

	CDnCashShopDlg::Render(fElapsedTime);
}

void CDnCashShopRenewalDlg::Show(bool bShow)
{	
	if( bShow )
	{
		/*// 메인탭의 첫번째 서브탭에서만 나오는 배너.
		// : MainTab - MainSubTab 에 BannerDlg 등록 - 한번만 등록하면 된다.
		if( m_pCSMainBanner == NULL )
		{
			CEtUIDialog * pDlg = m_groupTabDialog.GetDialog( m_pCategoryBtns[0]->GetTabID() );
			if( pDlg )
			{
				CDnCashShopCatDlg * pCatDlg = (CDnCashShopCatDlg *)pDlg;
				pDlg = NULL;
				pDlg = pCatDlg->GetMainSubDlg();
				if( pDlg )
				{
					// Banner 생성 - 하위탭들이 모두 생성된 뒤에 추가해야 되므로 이곳에서 new 한다.
					m_pCSMainBanner = new CDnCashShopMainBanner( UI_DIALOG_TYPE::UI_TYPE_CHILD, pDlg );
					m_pCSMainBanner->Initialize( true ); // test

					// MainPage.
					//CDnCashShopGoodsDlg * pGoodsDlg = static_cast<CDnCashShopGoodsDlg*>( pDlg );
					//pGoodsDlg->SetMainPage();
				}
			}
		}*/	

		// 열릴때 메인탭이 열리도록한다.
		if( m_pMenuListDlg )
			m_pMenuListDlg->SelectMainTab();

		ShowIngameBannerItem();
	}

	else
	{
		/*// 캐시샵이 닫힐때 정렬기준 초기화.
		if( CDnCashShopTask::IsActive() )
		{	
			m_pJobSortComboBox->SetSelectedByIndex( 0 );
			m_pEtcSortComboBox->SetSelectedByIndex( 0 );

			CDnCashShopTask::GetInstance().SetSortJob( 0 );
			CDnCashShopTask::GetInstance().SetSortEtc( eCashShopEtcSortType::SORT_BASIC );
		}*/

		/*// 닫을때 MenuList초기화.
		if( m_pMenuListDlg )
			m_pMenuListDlg->Reset();*/
	}
	
	CDnCashShopDlg::Show( bShow );
}

void CDnCashShopRenewalDlg::UpdateCashReserveAmount()
{
	CDnCashShopDlg::UpdateCashReserveAmount();
}

bool CDnCashShopRenewalDlg::AttachPartsToPreview(CASHITEM_SN sn, ITEMCLSID itemId, bool bCheckPackageCashRing2)
{
	return CDnCashShopDlg::AttachPartsToPreview( sn, itemId, bCheckPackageCashRing2 );
}

void CDnCashShopRenewalDlg::DetachPartsToPreview(int classId, CASHITEM_SN sn)
{
	CDnCashShopDlg::DetachPartsToPreview( classId, sn );
}

void CDnCashShopRenewalDlg::UpdateGiftReceiverInfo(const SCCashShopCheckReceiver& info)
{
	CDnCashShopDlg::UpdateGiftReceiverInfo( info );
}

void CDnCashShopRenewalDlg::UpdateRecvGiftList()
{
	CDnCashShopDlg::UpdateRecvGiftList();
}

void CDnCashShopRenewalDlg::UpdateCart(eRetCartAction action, const SCartActionParam& param)
{
	CDnCashShopDlg::UpdateCart( action, param );
}

#ifdef PRE_ADD_INVEN_EXTENSION
void CDnCashShopRenewalDlg::ReserveBuyItemNowOnNextOpened(CASHITEM_SN sn)
{
	CDnCashShopDlg::ReserveBuyItemNowOnNextOpened( sn );
}
#endif

void CDnCashShopRenewalDlg::OpenBuyCart(bool bShow)
{
	CDnCashShopDlg::OpenBuyCart( bShow );
}

void CDnCashShopRenewalDlg::OpenBuyPreviewCart(bool bShow)
{
	CDnCashShopDlg::OpenBuyPreviewCart( bShow );
}

bool CDnCashShopRenewalDlg::IsPreviewCartBuyable() const
{
	return CDnCashShopDlg::IsPreviewCartBuyable();
}

bool CDnCashShopRenewalDlg::IsCartBuyable() const
{
	return CDnCashShopDlg::IsCartBuyable();
}

eCashShopError CDnCashShopRenewalDlg::IsCartGiftable(eCashUnitType type) const
{
	return CDnCashShopDlg::IsCartGiftable( type );
}

void CDnCashShopRenewalDlg::ShowGiftFailDlg(bool bShow, const std::wstring& msg, const std::vector<CASHITEM_SN>& itemList, UINT uType, int callBackDlgID, CEtUICallback* pCall) const
{
	CDnCashShopDlg::ShowGiftFailDlg( bShow, msg, itemList, uType, callBackDlgID, pCall );
}

void CDnCashShopRenewalDlg::ShowCashShopMsgBox(const std::wstring& firstmsg, const std::wstring& secondmsg, int nID, CEtUICallback* pCallBack)
{
	CDnCashShopDlg::ShowCashShopMsgBox( firstmsg, secondmsg, nID, pCallBack );
}

void CDnCashShopRenewalDlg::GiftCart(bool bClear)
{
	CDnCashShopDlg::GiftCart( bClear );
}

void CDnCashShopRenewalDlg::GiftPreviewCart(bool bClear)
{
	CDnCashShopDlg::GiftPreviewCart( bClear );
}

void CDnCashShopRenewalDlg::OpenGiftReply(const WCHAR* pName, int giftDBID)
{
	CDnCashShopDlg::OpenGiftReply( pName, giftDBID );
}

void CDnCashShopRenewalDlg::ShowGiftPayDlg(bool bShow, eCashUnitType type)
{
	CDnCashShopDlg::ShowGiftPayDlg( bShow, type );
}

void CDnCashShopRenewalDlg::ShowCouponDlg(bool bShow)
{
	CDnCashShopDlg::ShowCouponDlg( bShow );
}

void CDnCashShopRenewalDlg::ShowRecvGiftDlg(bool bShow)
{
	CDnCashShopDlg::ShowRecvGiftDlg( bShow );

}

void CDnCashShopRenewalDlg::ClearPreviewCart(int classId)
{
	CDnCashShopDlg::ClearPreviewCart( classId );
}

// todo : callback handler
void CDnCashShopRenewalDlg::HandleCashShopError(eCashShopError code, bool bShowCode, CEtUIDialog* pCallbackDlg) const
{
	CDnCashShopDlg::HandleCashShopError( code, bShowCode, pCallbackDlg );
}

void CDnCashShopRenewalDlg::OnCloseCashShopDlg()
{
	CDnCashShopDlg::OnCloseCashShopDlg();
}

void CDnCashShopRenewalDlg::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg )
{
	CDnCashShopDlg::OnUICallbackProc( nID, nCommand, pControl, uiMsg );
}

bool CDnCashShopRenewalDlg::IsOpenCashShopInvenDlg() const
{
	return CDnCashShopDlg::IsOpenCashShopInvenDlg();
}

bool CDnCashShopRenewalDlg::IsOpenCashShopRecvGiftDlg() const
{
	return CDnCashShopDlg::IsOpenCashShopRecvGiftDlg();
}

void CDnCashShopRenewalDlg::DisableAllDlgs(bool bEnable, const std::wstring& text, float delayTime)
{
	CDnCashShopDlg::DisableAllDlgs( bEnable, text, delayTime );
}

eRetPreviewChange CDnCashShopRenewalDlg::ChangePreviewAvatar(int classId,bool bForce)
{
	return CDnCashShopDlg::ChangePreviewAvatar( classId, bForce );
}

void CDnCashShopRenewalDlg::ChangeVehicleParts(int nClassID,int nPartClassID, bool bDefault)
{
	CDnCashShopDlg::ChangeVehicleParts( nClassID, nPartClassID, bDefault );
}

void CDnCashShopRenewalDlg::MoveCategory(bool bNext)
{
	CDnCashShopDlg::MoveCategory( bNext );
}

bool CDnCashShopRenewalDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	return CDnCashShopDlg::MsgProc( hWnd, uMsg, wParam, lParam );
}

void CDnCashShopRenewalDlg::EnableDummyModalDlg(bool bSet)
{
	CDnCashShopDlg::EnableDummyModalDlg( bSet );
}

#ifdef PRE_ADD_CASHSHOP_REFUND_CL
void CDnCashShopRenewalDlg::UpdateRefundInven()
{
	//m_pInvenRefundDlg->Upda
}
#endif

void CDnCashShopRenewalDlg::ChangePetPreviewAvatar( int nItemID )
{
	CDnCashShopDlg::ChangePetPreviewAvatar( nItemID );
}

void CDnCashShopRenewalDlg::ChangePetPreviewParts( int nPartsType, int nItemID )
{
	CDnCashShopDlg::ChangePetPreviewParts( nPartsType, nItemID );
}

void CDnCashShopRenewalDlg::ChangePetPreviewColor( eItemTypeEnum eItemType, DWORD dwColor )
{
	CDnCashShopDlg::ChangePetPreviewColor( eItemType, dwColor );
}

bool CDnCashShopRenewalDlg::IsExistPetAvatar()
{
	return CDnCashShopDlg::IsExistPetAvatar();
}

void CDnCashShopRenewalDlg::OpenBuyPetPreviewCart()
{
	CDnCashShopDlg::OpenBuyPetPreviewCart();
}

void CDnCashShopRenewalDlg::SwapPreview( bool bShowChar )
{
	CDnCashShopDlg::SwapPreview( bShowChar );
}

void CDnCashShopRenewalDlg::DetachPartsToPetPreview( int nPartsSlot )
{
	CDnCashShopDlg::DetachPartsToPetPreview( nPartsSlot );
}

#ifdef PRE_ADD_SALE_COUPON

void CDnCashShopRenewalDlg::CheckShowSaleTabButton( bool bSaleCouponGift )
{
	// 인게임배너에서 상품을 클릭해서 캐시샾을 오픈한경우에는 쿠폰탭을 열지않는다.
	if( m_bShowIngameBannerItem )
	{
		m_bShowIngameBannerItem = false;
		return;
	}

	//CDnCashShopDlg::CheckShowSaleTabButton( bSaleCouponGift );

	//GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1000039876 )

	const std::wstring strSaleName( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1000039876 ) );

	if( GetCashShopTask().CheckCoupon() )
	{
		// 할인품목 탭 Show.
		if( m_pMenuListDlg )
			m_pMenuListDlg->ShowSaleCouponTab( true, strSaleName, m_bSaleCouponOpen );
	

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
			if( m_bSaleCouponOpen && m_pMenuListDlg->ShowSaleCouponSubCategory( strSaleName, nSubCategory ) )
			    break;
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
	else
	{
		// 할인품목 탭 Hide.
		if( m_pMenuListDlg )
			m_pMenuListDlg->ShowSaleCouponTab( false, strSaleName, false );

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

void CDnCashShopRenewalDlg::SetSaleTabButtonBlink( bool bBlink )
{
	CDnCashShopDlg::SetSaleTabButtonBlink( bBlink );
}

#endif // PRE_ADD_SALE_COUPON

#ifdef PRE_ADD_LIMITED_CASHITEM

void CDnCashShopRenewalDlg::UpdateCurrentGoodsDlg()
{
	CDnCashShopDlg::UpdateCurrentGoodsDlg();
}

#endif // PRE_ADD_LIMITED_CASHITEM

#ifdef PRE_ADD_CADGE_CASH

void CDnCashShopRenewalDlg::SetReserveCadgeItemList( std::wstring strCadgeReciever, int nMailID, int nPackageSN, int nCount, TWishItemInfo* pItemList )
{
	CDnCashShopDlg::SetReserveCadgeItemList( strCadgeReciever, nMailID, nPackageSN, nCount, pItemList );
}

#endif // PRE_ADD_CADGE_CASH


#ifdef PRE_ADD_CASHSHOP_ACTOZ
void CDnCashShopRenewalDlg::ShowConfirmRefund( CDnItem * pItem )
{
	CDnCashShopDlg::ShowConfirmRefund( pItem );

}
#endif // PRE_ADD_CASHSHOP_ACTOZ

void CDnCashShopRenewalDlg::ChangeSortOrder( bool bJob, int nValue )
{
	if( m_pCategoryDlgs.size() < 1 )
		return;
	
	CEtUIDialog * pDlg = m_groupTabDialog.GetDialog( GetCurrentTabID() );
	if( pDlg )
	{
		CDnCashShopCatDlg * pCatDlg = (CDnCashShopCatDlg *)pDlg;

		// 직업.
		if( bJob )
		{			
			if( CDnCashShopTask::IsActive() )
				CDnCashShopTask::GetInstance().SetSortJob( nValue );
			pCatDlg->ChangeSortJob( nValue );
		}
		else
		{
			if( CDnCashShopTask::IsActive() )
				CDnCashShopTask::GetInstance().SetSortEtc( nValue );
			pCatDlg->ChangeSortEtc( nValue );
		}
	}
		
	//if( m_pCategoryDlgs.size() < 1 )
	//	return;

	//CDnCashShopCatDlg * pCatDlg = m_pCategoryDlgs[ m_pCategoryDlgs.size() - 1 ];
	//if( pCatDlg && pCatDlg->GetDialogID() == 999 )
	//	pCatDlg->ChangeSortOrder( nValue ); // 검색카테고리는 제일끝에있다.
}

// 상품검색.
void CDnCashShopRenewalDlg::SearchItems( const std::wstring & str )
{
	if( str.empty() )
		return;

	m_EditBoxSearch->ClearText();

	if( CDnCashShopTask::IsActive() )
	{
		GetCashShopTask().SearchItems( str );

		CDnCashShopCatDlg * pCatDlg = m_pCategoryDlgs[ m_pCategoryDlgs.size()-1 ];
		eCashShopSubCatType type = pCatDlg->SearchItems( SUBCATEGORY_SERACH ); 
		
		// 검색된 상품목록.
		CS_INFO_LIST * pSearchDataList = GetCashShopTask().GetSubCatItemList( type );
		if( pSearchDataList && pSearchDataList->size() > 0 )
		{
			pCatDlg->SetCheckedTab( SUBCATEGORY_SERACH );
			SetCheckedTab( MAINCATEGORY_SERACH );
			//pCatDlg->SearchItems( SUBCATEGORY_SERACH );
		}
		
		// 상품검색카테고리 상태에서 검색한결과가 없을때에는 메인탭으로 전환.
		else if( MAINCATEGORY_SERACH == pCatDlg->GetMainTabNum() )
		{			
			//pCatDlg->SetCheckedTab( 0 );
			SetCheckedTab( m_pCategoryDlgs.front()->GetMainTabNum() );
		}

	}
}

void CDnCashShopRenewalDlg::SelectMainCategory( int tabID )
{
	m_EditBoxSearch->ClearText();

	SetCheckedTab( tabID );
}


void CDnCashShopRenewalDlg::SelectSubCategory( int mainTabID, int subTabID )
{
	m_EditBoxSearch->ClearText();

	CDnCashShopCatDlg * pDlg = NULL;
	int size = (int)m_pCategoryDlgs.size();
	for( int i=0; i<size; ++i )
	{
		pDlg = m_pCategoryDlgs[i];
		if( pDlg && pDlg->GetMainTabNum() == mainTabID )
			pDlg->SelectCategory( subTabID );
	}

}


void CDnCashShopRenewalDlg::OnChangeResolution()
{
	float x, y;
	this->GetPosition( x, y );

	CEtUIDialog::OnChangeResolution();
}


void CDnCashShopRenewalDlg::ShowIngameBannerItem()
{
	if( m_bShowIngameBannerItem == false )
		return;	

	// 상품검색시 메뉴리스트를 초기화함.
	if( m_pMenuListDlg )
		m_pMenuListDlg->ResetMenuList();

	SearchItems( m_strBannerItemName ); // 상품검색.
	m_strBannerItemName.clear();	
}

#endif // PRE_ADD_CASHSHOP_RENEWAL