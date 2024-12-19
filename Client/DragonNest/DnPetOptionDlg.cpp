#include "StdAfx.h"
#include "DnPetOptionDlg.h"
#include "DnPetTask.h"
#include "DnInterface.h"
#include "DnItemTask.h"
#include "DnMainDlg.h"
//#include "DnInvenTabDlg.h" // test.
//#include "DnMainMenuDlg.h"
#include "DnMainMenuDlg.h" // test.
#include "DnInvenTabDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnPetOptionDlg::CDnPetOptionDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback, true )
, m_pCheckBoxGetItem( NULL )
, m_pCheckBoxNormal( NULL )
, m_pCheckBoxMagic( NULL )
, m_pCheckBoxRare( NULL )
, m_pCheckBoxEpic( NULL )
, m_pCheckBoxUnique( NULL )
, m_pCheckBoxLegend( NULL )
//, m_pCheckBoxAutoUseHPPotion( NULL )
//, m_pCheckBoxAutoUseMPPotion( NULL )
//, m_pSliderAutoUseHPPotion( NULL )
//, m_pSliderAutoUseMPPotion( NULL )
//, m_pStaticAutoUseHPPotion( NULL )
//, m_pStaticAutoUseMPPotion( NULL )
, m_pFoodItem( NULL )
, m_pFoodItemSlot( NULL )
, m_pCheckBoxAutoFeed( NULL )
, m_pSliderAutoFeed( NULL )
, m_pStaticAutoFeedPercent( NULL )
{
}

CDnPetOptionDlg::~CDnPetOptionDlg()
{
}

void CDnPetOptionDlg::InitialUpdate()
{
	CEtUIDialog::InitialUpdate();

	m_pCheckBoxGetItem = GetControl<CEtUICheckBox>( "ID_CHECKBOX_GETITEM" );
	m_pCheckBoxNormal = GetControl<CEtUICheckBox>( "ID_CHECKBOX_NORMAL" );
	m_pCheckBoxMagic = GetControl<CEtUICheckBox>( "ID_CHECKBOX_MAGIC" );
	m_pCheckBoxRare = GetControl<CEtUICheckBox>( "ID_CHECKBOX_RARE" );
	m_pCheckBoxEpic = GetControl<CEtUICheckBox>( "ID_CHECKBOX_EPIC" );
	m_pCheckBoxUnique = GetControl<CEtUICheckBox>( "ID_CHECKBOX_UNIQUE" );
	m_pCheckBoxLegend = GetControl<CEtUICheckBox>( "ID_CHECKBOX_LEGEND" );
//	m_pCheckBoxAutoUseHPPotion =  GetControl<CEtUICheckBox>( "ID_CHECKBOX_HP" );
//	m_pCheckBoxAutoUseMPPotion =  GetControl<CEtUICheckBox>( "ID_CHECKBOX_MP" );
//	m_pSliderAutoUseHPPotion = GetControl<CEtUISlider>( "ID_SLIDER_HP" );
//	m_pSliderAutoUseMPPotion = GetControl<CEtUISlider>( "ID_SLIDER_MP" );
//	m_pStaticAutoUseHPPotion =  GetControl<CEtUIStatic>( "ID_TEXT_HP" );
//	m_pStaticAutoUseMPPotion =  GetControl<CEtUIStatic>( "ID_TEXT_MP" );
	m_pFoodItemSlot = GetControl<CDnItemSlotButton>( "ID_BT_ITEM" );
	m_pCheckBoxAutoFeed = GetControl<CEtUICheckBox>( "ID_CHECKBOX0" );
	m_pSliderAutoFeed = GetControl<CEtUISlider>( "ID_SLIDER_FOOD" );
	m_pStaticAutoFeedPercent = GetControl<CEtUIStatic>( "ID_TEXT_COUNT" );
	m_pStaticAutoFeedPercent->SetText( L"0%" );
}

void CDnPetOptionDlg::Initialize( bool bShow )
{
#ifdef _USE_PET_FEEDING
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "CharPetOptionDlg.ui" ).c_str(), bShow );
#else // _USE_PET_FEEDING
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "CharPetOption_NPDlg.ui" ).c_str(), bShow );
#endif // _USE_PET_FEEDING
}

void CDnPetOptionDlg::Show( bool bShow )
{
	if( m_bShow == bShow )
		return;

	if( bShow )
	{
		GetPetOption();
#ifdef _USE_PET_FEEDING
		CDnInvenTabDlg *pInvenDlg = (CDnInvenTabDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::INVENTORY_DIALOG );
		if( pInvenDlg )
		{
			if( !pInvenDlg->IsShow() )
				pInvenDlg->Show( true );

			pInvenDlg->ShowTab( ST_INVENTORY_CASH );
		}
#endif // _USE_PET_FEEDING
	}

	CEtUIDialog::Show( bShow );
}

void CDnPetOptionDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl( "ID_CANCEL" ) || IsCmdControl("ID_BUTTON_CLOSE") )
		{
			if( m_pFoodItem )
			{
				SAFE_DELETE( m_pFoodItem );
				m_pFoodItemSlot->ResetSlot();
			}
			Show( false );
		}
		else if( IsCmdControl( "ID_OK" ) )
		{
			stPetOption stPetOptionData;
			stPetOptionData.m_bGetItem = m_pCheckBoxGetItem->IsChecked();
			stPetOptionData.m_bGetNormalItem = m_pCheckBoxNormal->IsChecked();
			stPetOptionData.m_bGetMagicItem = m_pCheckBoxMagic->IsChecked();
			stPetOptionData.m_bGetRareItem = m_pCheckBoxRare->IsChecked();
			stPetOptionData.m_bGetEpicItem = m_pCheckBoxEpic->IsChecked();
			stPetOptionData.m_bGetUniqueItem = m_pCheckBoxUnique->IsChecked();
			stPetOptionData.m_bGetLegendItem = m_pCheckBoxLegend->IsChecked();
//			stPetOptionData.m_bAutoUseHPPotion = m_pCheckBoxAutoUseHPPotion->IsChecked();
//			stPetOptionData.m_bAutoUseMPPotion = m_pCheckBoxAutoUseMPPotion->IsChecked();
//			stPetOptionData.m_nAutoUseHPPotionPercent = m_pSliderAutoUseHPPotion->GetValue();
//			stPetOptionData.m_nAutoUseMPPotionPercent = m_pSliderAutoUseMPPotion->GetValue();
			if( m_pFoodItem )
				stPetOptionData.m_nFoodItemID = m_pFoodItem->GetClassID();

			stPetOptionData.m_bAutoFeed = m_pCheckBoxAutoFeed->IsChecked();
			stPetOptionData.m_nAutoFeedPercent = m_pSliderAutoFeed->GetValue();

			CDnMainDlg *pMainBarDlg = GetInterface().GetMainBarDialog();
			if( pMainBarDlg )
				pMainBarDlg->SetPetFoodInfo( stPetOptionData.m_nFoodItemID );

			GetPetTask().SetPetOption( stPetOptionData );
			GetInterface().AddChatMessage( CHATTYPE_SYSTEM, NULL, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 100060) );

			Show( false );
		}
		else if( IsCmdControl( "ID_BT_ITEM" ) )
		{
			CDnSlotButton *pDragButton = ( CDnSlotButton * )drag::GetControl();
			CDnSlotButton *pPressedButton = ( CDnItemSlotButton * )pControl;

			// 우클릭으로 빼기
			if( uMsg == WM_RBUTTONUP )
			{
				SAFE_DELETE( m_pFoodItem );
				pPressedButton->ResetSlot();
				m_pCheckBoxAutoFeed->SetChecked( false );
				return;
			}

			if( pDragButton )
			{
				drag::Command( UI_DRAG_CMD_CANCEL );
				drag::ReleaseControl();

				if( pDragButton->GetItemType() != MIInventoryItem::Item )
					return;

				CDnItem* pItem = static_cast<CDnItem *>(pDragButton->GetItem());
				if( pItem == NULL )
					return;

				CDnItem *pPressedItem = static_cast<CDnItem *>(pPressedButton->GetItem());
				if( pPressedItem )
				{
					SAFE_DELETE( m_pFoodItem );
					pPressedButton->ResetSlot();
				}

				SetPetFoodItem( pItem->GetClassID() );
			}

			return;
		}
		else if( IsCmdControl( "ID_BT_DOWN" ) )
		{
			WCHAR wszStr[64];
			int nPercent = m_pSliderAutoFeed->GetValue();
			nPercent--;
			if( nPercent < 0 )
				nPercent = 0;
			m_pSliderAutoFeed->SetValue( nPercent );
			wsprintf( wszStr, L"%d%%", m_pSliderAutoFeed->GetValue() );
			m_pStaticAutoFeedPercent->SetText( wszStr );
		}
		else if( IsCmdControl( "ID_BT_UP" ) )
		{
			WCHAR wszStr[64];
			int nPercent = m_pSliderAutoFeed->GetValue();
			nPercent++;
			if( nPercent > 100 )
				nPercent = 100;
			m_pSliderAutoFeed->SetValue( nPercent );
			wsprintf( wszStr, L"%d%%", m_pSliderAutoFeed->GetValue() );
			m_pStaticAutoFeedPercent->SetText( wszStr );
		}
	}
	else if( nCommand == EVENT_SLIDER_VALUE_CHANGED )
	{
		WCHAR wszStr[64];
		if( strcmp( pControl->GetControlName(), "ID_SLIDER_FOOD" ) == 0 )
		{
			wsprintf( wszStr, L"%d%%", m_pSliderAutoFeed->GetValue() );
			m_pStaticAutoFeedPercent->SetText( wszStr );
		}
/*		if( strcmp( pControl->GetControlName(), "ID_SLIDER_HP" ) == 0 )
		{
			wsprintf( wszStr, L"%d%%", m_pSliderAutoUseHPPotion->GetValue() );
			m_pStaticAutoUseHPPotion->SetText( wszStr );
		}
		else if( strcmp( pControl->GetControlName(), "ID_SLIDER_MP" ) == 0 )
		{
			wsprintf( wszStr, L"%d%%", m_pSliderAutoUseMPPotion->GetValue() );
			m_pStaticAutoUseMPPotion->SetText( wszStr );
		}
*/
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnPetOptionDlg::GetPetOption()
{
	stPetOption stPetOption = GetPetTask().GetPetOption();
	
	m_pCheckBoxGetItem->SetChecked( stPetOption.m_bGetItem );
	m_pCheckBoxNormal->SetChecked( stPetOption.m_bGetNormalItem );
	m_pCheckBoxMagic->SetChecked( stPetOption.m_bGetMagicItem );
	m_pCheckBoxRare->SetChecked( stPetOption.m_bGetRareItem );
	m_pCheckBoxEpic->SetChecked( stPetOption.m_bGetEpicItem );
	m_pCheckBoxUnique->SetChecked( stPetOption.m_bGetUniqueItem );
	m_pCheckBoxLegend->SetChecked( stPetOption.m_bGetLegendItem );
/*
	m_pCheckBoxAutoUseHPPotion->SetChecked( stPetOption.m_bAutoUseHPPotion );
	m_pCheckBoxAutoUseMPPotion->SetChecked( stPetOption.m_bAutoUseMPPotion );

	m_pSliderAutoUseHPPotion->SetValue( stPetOption.m_nAutoUseHPPotionPercent );
	m_pSliderAutoUseMPPotion->SetValue( stPetOption.m_nAutoUseMPPotionPercent );

	WCHAR wszStr[64];
	wsprintf( wszStr, L"%d%%", stPetOption.m_nAutoUseHPPotionPercent );
	m_pStaticAutoUseHPPotion->SetText( wszStr );
	wsprintf( wszStr, L"%d%%", stPetOption.m_nAutoUseMPPotionPercent );
	m_pStaticAutoUseMPPotion->SetText( wszStr );
*/
	SetPetFoodItem( stPetOption.m_nFoodItemID );
	m_pCheckBoxAutoFeed->SetChecked( stPetOption.m_bAutoFeed );
	m_pSliderAutoFeed->SetValue( stPetOption.m_nAutoFeedPercent );
	WCHAR wszStr[64];
	wsprintf( wszStr, L"%d%%", stPetOption.m_nAutoFeedPercent );
	m_pStaticAutoFeedPercent->SetText( wszStr );

	int nPetFoodItemCount = GetPetTask().GetPetFoodItemCount( stPetOption.m_nFoodItemID );
	if( nPetFoodItemCount == 0 )
	{
		SAFE_DELETE( m_pFoodItem );
		m_pFoodItemSlot->ResetSlot();
		stPetOption.m_nFoodItemID = 0;
		GetPetTask().SetPetOption( stPetOption );
	}
	else if( nPetFoodItemCount > 0 )
	{
		m_pFoodItemSlot->SetRenderCount( nPetFoodItemCount );
	}
}

void CDnPetOptionDlg::SetPetFoodItem( int nPetFoodItemID )
{
	if( nPetFoodItemID == 0 )
		return;

	TItemInfo itemInfo;
	if( CDnItem::MakeItemInfo( nPetFoodItemID, 1, itemInfo ) )
	{
		m_pFoodItem = GetItemTask().CreateItem( itemInfo );

		if( m_pFoodItem && m_pFoodItem->GetItemType() == ITEMTYPE_PET_FOOD && m_pFoodItem->IsCashItem() )
		{
			int nPetFoodCount = GetPetTask().GetPetFoodItemCount( nPetFoodItemID );
			m_pFoodItemSlot->SetItem( m_pFoodItem, nPetFoodCount );

			m_pCheckBoxAutoFeed->SetChecked( true );
		}
		else
		{
			SAFE_DELETE( m_pFoodItem );
		}
	}

	drag::Command( UI_DRAG_CMD_CANCEL );
	drag::ReleaseControl();
}

void CDnPetOptionDlg::RefreshPetFoodCount()
{
	if( m_pFoodItem == NULL )
		return;

	int nPetFoodItemCount = GetPetTask().GetPetFoodItemCount( GetPetTask().GetPetOption().m_nFoodItemID );
	if( nPetFoodItemCount == 0 )
	{
		SAFE_DELETE( m_pFoodItem );
		m_pFoodItemSlot->ResetSlot();
		stPetOption stPetOption = GetPetTask().GetPetOption();
		stPetOption.m_nFoodItemID = 0;
		GetPetTask().SetPetOption( stPetOption );
	}
	else if( nPetFoodItemCount > 0 )
	{
		m_pFoodItemSlot->SetRenderCount( nPetFoodItemCount );
	}
}

