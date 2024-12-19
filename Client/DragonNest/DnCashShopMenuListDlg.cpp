#include "stdafx.h"

#ifdef PRE_ADD_CASHSHOP_RENEWAL

#include "DnCashShopTask.h"

#include "DnMenuListItem.h"

#include "DnCashShopRenewalDlg.h"

#include "DnCashShopMenuListDlg.h"

//#include "DnCashShopTask.h"
//#include "DnInterface.h"


#define MAX_CASHSHOP_CAT_RADIO_BTN_COUNT 8 // 메인카테고리 개수.
#define MAX_RADIO_BTN_COUNT 9  // 서브카테고리 개수.

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif
CDnCashShopMenuListDlg::CDnCashShopMenuListDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback)
: CDnCustomDlg(dialogType, pParentDialog, nID, pCallback, true)
,m_pMenuList(NULL)
{
}

CDnCashShopMenuListDlg::~CDnCashShopMenuListDlg()
{
}

void CDnCashShopMenuListDlg::Initialize(bool bShow)
{
	CEtUIDialog::Initialize(CEtResourceMng::GetInstance().GetFullName("CS_MenuDlg.ui").c_str(), bShow);
}

void CDnCashShopMenuListDlg::InitCustomControl(CEtUIControl *pControl)
{
	if( pControl == NULL || !strstr(pControl->GetControlName(), "ID_CUSTOM_MENU") )
		return;

	if( CDnCashShopTask::IsActive() == false )
		return;

	// "할인품목"
	const std::wstring strSaleTabName( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1000039876 ) );

	m_pMenuList = static_cast<CDnMenuList * >(pControl); //GetControl<CDnMenuList>("ID_CUSTOM_MENU");
	if( m_pMenuList )
	{
		m_pMenuList->Show( true );
		m_pMenuList->AutoSelectToSubItem( true );

		const std::map<int, SCashShopMainTabShowInfo>& mainTabList = GetCashShopTask().GetShowMainTabList();
		for( int i=0; i < MAX_CASHSHOP_CAT_RADIO_BTN_COUNT; ++i )
		{
			std::map<int, SCashShopMainTabShowInfo>::const_iterator iter = mainTabList.find(i);
			if (iter != mainTabList.end())
			{
				const SCashShopMainTabShowInfo& mainTabInfo = (*iter).second;
				if( mainTabInfo.showType != eCST_SHOW )
					continue;

				const std::wstring title = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, mainTabInfo.uiStringNum );
				if( !title.empty() && title.size() > 0 )
				{
					//-----------------
					// Main Caterogy //
					int mainTabNum = iter->first;
					CDnMenuListItem * pMainItem = m_pMenuList->AddItem( title.c_str() );
					pMainItem->SetItemValueInt( mainTabNum );


					// WISHLIST.
					bool bWishList = false;
					const SCashShopMainTabShowInfo* pInfo = GetCashShopTask().GetShowMainTabInfo(mainTabNum);
					if (pInfo->option & eCSMTOPT_WISHLIST)
					{
						bWishList = true;
						pMainItem->Show( false, true );
					}

					// "할인품목" 탭 은 숨긴다.				
					if( strSaleTabName.compare( title ) == 0 )
					{
						pMainItem->SetTextColor( textcolor::YELLOW );
						pMainItem->Show( false, true );
					}

					//-----------------
					// Sub Category  //
					const std::map<int, SCashShopTabShowInfo> * pShowSubTabList = GetCashShopTask().GetShowSubTabList( mainTabNum );
					if (pShowSubTabList == NULL)
						continue;

#ifdef PRE_ADD_CASHSHOP_SUBCAT_COLOR
					const std::map<int, SCashShopSubTabShowInfo>* pShowSubTabCatList = GetCashShopTask().GetShowSubTabCatList( mainTabNum );
					std::map<int, SCashShopSubTabShowInfo>::const_iterator SubCatiter = pShowSubTabCatList->begin();

					std::wstring SelectedColor;
					std::wstring DeSelectedColor;	

					if( pShowSubTabCatList == NULL )
						return;
#endif


					std::map<int, SCashShopTabShowInfo>::const_iterator iter = pShowSubTabList->begin();
					for ( int k=0; k < MAX_RADIO_BTN_COUNT; ++k )
					{
						if (iter == pShowSubTabList->end())
							continue;

						int curType = (*iter).first;
						const SCashShopTabShowInfo & subTabInfo = (*iter).second;
						++iter;

						if ( subTabInfo.showType != eCST_SHOW )
							continue;

						const std::wstring subTitle = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, subTabInfo.uiStringNum );
						if( subTitle.empty() == false )
						{
							CDnMenuListItem * pChildItem = m_pMenuList->AddChildItem( pMainItem, subTitle.c_str() );
							//pChildItem->SetItemValueInt( curType );
							pChildItem->SetItemValueInt( k );				

						}

					}

					// Sub Category //


				}
			}		
		}



		//m_pMenuList->AddItem( L"ABCD1234한글도abcd1234길게써봅시다" );
	}

}

//void CDnCashShopMenuListDlg::InitialUpdate()
//{	
//}

void CDnCashShopMenuListDlg::ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg)
{
	SetCmdControlName(pControl->GetControlName());

	if( nCommand == EVENT_MENULIST_SELECTION )
	{
		if( m_pMenuList )
		{
			CEtUIDialog * pParentDlg = GetParentDialog();
			CDnMenuListItem * pItem = m_pMenuList->GetSelectedItem();
			//CDnMenuListItem * pItem = m_pMenuList->GetSelectedMainItem();
			if( pItem && pParentDlg )
			{
				CDnCashShopRenewalDlg * pCashShopDlg = static_cast< CDnCashShopRenewalDlg * >( pParentDlg );

				// Main.
				if( CDnMenuListItem::EITEMTYPE::EITEMTYPE_MAIN == pItem->GetType() )
				{
					pCashShopDlg->SelectMainCategory( pItem->GetItemValueInt() );

					// m_bAutoSelectSubItem : true 인경우에는 MainItem 선택시 SubItem도 선택되기 때문에 서브카테고리처리까지 한다.
					if( m_pMenuList->IsAutoSelectToSubItem() )
					{
						CDnMenuListItem * pSubItem = m_pMenuList->GetSelectedSubItem();
						if( pSubItem )
							pCashShopDlg->SelectSubCategory( m_pMenuList->GetSelectedMainItem()->GetItemValueInt(), pSubItem->GetItemValueInt() );
					}
				}
				// Sub.
				if( CDnMenuListItem::EITEMTYPE::EITEMTYPE_SUB == pItem->GetType() )
				{					
					pCashShopDlg->SelectMainCategory( m_pMenuList->GetSelectedMainItem()->GetItemValueInt() );
					pCashShopDlg->SelectSubCategory( m_pMenuList->GetSelectedMainItem()->GetItemValueInt(), pItem->GetItemValueInt() );
				}
			}			
		}
	}
	

	CEtUIDialog::ProcessCommand(nCommand, bTriggeredByUser, pControl, uMsg);
}

void CDnCashShopMenuListDlg::Process(float fElapsedTime)
{
	if (m_bShow == false)
		return;

	if (m_bShow)
	{
	
	}

	CEtUIDialog::Process(fElapsedTime);
}

void CDnCashShopMenuListDlg::Show(bool bShow)
{
	if( m_bShow == bShow )
		return;

	CEtUIDialog::Show( bShow );

	//if (m_bShow)
	//{
	//	if( m_pMenuList )
	//		m_pMenuList->SelectByIndex( 0 );
	//}
	//else
	//{
	//	if( m_pMenuList )
	//		m_pMenuList->ResetnUpdatePos();
	//}
};


void CDnCashShopMenuListDlg::Render( float fElapsedTime )
{
	
	CEtUIDialog::Render( fElapsedTime );
}

void CDnCashShopMenuListDlg::ResetMenuList()
{
	if( m_pMenuList )
		m_pMenuList->ResetnUpdatePos();
}

CDnMenuListItem * CDnCashShopMenuListDlg::ShowSaleCouponTab( bool bShow, const std::wstring & strName, bool bSelect )
{
	if( m_pMenuList == NULL )
		return NULL;

	CDnMenuListItem * pItem = NULL;
	int size = m_pMenuList->GetMainTabSize();
	for( int i=0; i<size; ++i )
	{
		pItem = m_pMenuList->GetMainTab( i );
		if( pItem )
		{
			if( strName.compare( pItem->GetText() ) == 0 )
			{
				// 숨기는 경우에는 자식Item들 까지 숨긴다.
				pItem->Show( bShow, ( bShow==false ? true : false ) );

				if( bSelect )
					m_pMenuList->SetSelectItem( pItem );

				break;
			}
		}
	}

	return pItem;
}

bool CDnCashShopMenuListDlg::ShowSaleCouponSubCategory( const std::wstring & strName, int tabID )
{
	CDnMenuListItem * pMainItem = ShowSaleCouponTab( true, strName, false );
	if( pMainItem )
	{
		CDnMenuListItem * pItem = NULL;
		int size = pMainItem->GetSubItemSize();
		for( int i=0; i<size; ++i )
		{
			pItem = pMainItem->GetSubItem( i );
			if( pItem && pItem->GetItemValueInt() == tabID )
			{
				m_pMenuList->SetSelectItem( pItem );
				return true;
			}
		}
	}

	return false;
}

void CDnCashShopMenuListDlg::SelectMainTab()
{
	if( m_pMenuList )
		m_pMenuList->SelectByIndex( 0 );
}

void CDnCashShopMenuListDlg::Reset()
{
	if( m_pMenuList )
		m_pMenuList->ResetnUpdatePos();
}

#endif // PRE_ADD_CASHSHOP_RENEWAL