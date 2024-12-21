#include "StdAfx.h"
#include "EtUIMan.h"
#include "DnCharStatusDlg.h"
#include "DnCharStatusStatic.h"
#include "DnItemSlotButton.h"
#include "DnWeapon.h"
#include "DnItemTask.h"
#include "DnParts.h"
#include "DnInvenTabDlg.h"
#include "MIInventoryItem.h"
#include "DnItem.h"
#include "DnInterfaceString.h"
#include "DnMainMenuDlg.h"
#include "DnPlayerActor.h"
#include "DnTableDB.h"
#include "DnInterface.h"
#include "DnTooltipDlg.h"
#include "DnCharStatusBaseInfoDlg.h"
#include "DnCharStatusDetailInfoDlg.h"
#include "DnAppellationDlg.h"
#include "DnCharStatusPVPInfoDlg.h"
#include "DnCharPlateDlg.h"
#include "DnLocalPlayerActor.h"
#include "DnNameLinkMng.h"
#include "DnChatTabDlg.h"
#include "DnFadeInOutDlg.h"
#include "DnCharVehicleDlg.h"
#include "DnCharPetDlg.h"
#include "DnCharStatusLadderInfoDlg.h"
#include "DnCharStatusGuildWarInfoDlg.h"
#include "DnCommonUtil.h"
#if defined(PRE_ADD_TALISMAN_SYSTEM)
#include "DnCharTalismanDlg.h"
#endif // PRE_ADD_TALISMAN_SYSTEM
#ifdef PRE_ADD_EQUIPLOCK
#include "SyncTimer.h"
#include "DnItemLockMgr.h"
#include "TaskManager.h"
#include "DnPvPGameTask.h"
#endif

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnCharStatusDlg::CDnCharStatusDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
	: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback, true )
	, m_pMagnetic(NULL)
#ifdef PRE_ADD_EQUIPLOCK
	, m_pEquipLockMagnetic(NULL)
	, m_bShowEquipLockMagnetic(false)
#endif
	, m_pStaticName(NULL)
	, m_bDetailInfo(false)
	, m_pBaseInfoDlg(NULL)
	, m_pDetailInfoDlg(NULL)
	, m_pAppellationDlg(NULL)
	, m_pPVPInfoDlg(NULL)
	, m_pNormalButton(NULL)
	, m_pCashButton(NULL)
	, m_pButtonAppellation(NULL)
	, m_pPlateButton(NULL)
	, m_pPlateDlg(NULL)
	, m_pCashEquipBase( NULL )
	, m_pNormalEquipBase( NULL )
	, m_pTempSlotButton( NULL )
	, m_pTempSlotItem( NULL )
	, m_pVehicleButton(NULL)
	, m_pVehicleDlg(NULL)
	, m_pPetButton( NULL )
	, m_pPetDlg( NULL )
	, m_pLadderInfoDlg(NULL)
	, m_pGuildWarInfoDlg( NULL )
	, m_pComboBoxInfo(NULL) // 버튼이 콤보박스로 개편됨.
	, m_pSourceItemSlot( NULL )
#if defined(PRE_ADD_TALISMAN_SYSTEM)
	, m_pTalismanDlg(NULL)
	, m_pTalismanButton(NULL)
#endif // PRE_ADD_TALISMAN_SYSTEM
#if defined( PRE_ADD_COSTUME_SKILL )
	, m_pCustumeSkillSlotButton( NULL )
#endif
	, m_pMercenaryHeroButton( NULL )
{
	m_iPage = GENERAL_PAGE;
	m_bFocusCashEquip = false;
	m_VecEquipSlotButton.reserve( 12 );
	m_VecCashEquipSlotButton.reserve( 15 );
	m_pCheckToggleHelmet = NULL;
}

CDnCharStatusDlg::~CDnCharStatusDlg(void)
{
	SAFE_DELETE( m_pBaseInfoDlg );
	SAFE_DELETE( m_pDetailInfoDlg );
	SAFE_DELETE( m_pPVPInfoDlg );
	SAFE_DELETE( m_pAppellationDlg );
	SAFE_DELETE( m_pPlateDlg );
	SAFE_DELETE( m_pVehicleDlg );
	SAFE_DELETE( m_pPetDlg );
	SAFE_DELETE( m_pLadderInfoDlg );
	SAFE_DELETE( m_pGuildWarInfoDlg );
#if defined(PRE_ADD_TALISMAN_SYSTEM)
	SAFE_DELETE( m_pTalismanDlg );
#endif // PRE_ADD_TALISMAN_SYSTEM
}

void CDnCharStatusDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "CharStatus.ui" ).c_str(), bShow );
}

void CDnCharStatusDlg::InitialUpdate()
{
	m_pMagnetic = GetControl<CEtUIStatic>( "ID_MAGNETIC" );
	m_pMagnetic->Show( false );
	m_pStaticName = GetControl<CEtUIStatic>( "ID_STATIC_NAME" );

#ifdef PRE_ADD_EQUIPLOCK
	m_pEquipLockMagnetic = GetControl<CEtUIStatic>("ID_STATIC_LOCK");
	m_pEquipLockMagnetic->Show(false);
#endif

	m_pNormalEquipBase = GetControl<CEtUIStatic>( "ID_STATIC3" );
	m_pCashEquipBase = GetControl<CEtUIStatic>( "ID_STATIC5" );
	
	
	m_pButtonAppellation = GetControl<CEtUIButton>( "ID_BUTTON_APPE" );

	m_pNormalButton = GetControl<CEtUIRadioButton>( "ID_RADIOBUTTON0" );
	m_pCashButton = GetControl<CEtUIRadioButton>( "ID_RADIOBUTTON1" );
	m_pPlateButton = GetControl<CEtUIRadioButton>( "ID_RADIOBUTTON4" );

	//rlkt_mercenary
	m_pMercenaryHeroButton = GetControl<CEtUIRadioButton>("ID_RADIOBUTTON6");
	m_pMercenaryHeroButton->Enable(false);

	m_pVehicleButton = GetControl<CEtUIRadioButton>("ID_RADIOBUTTON3");
	m_pVehicleButton->Show(true);

#if defined(PRE_ADD_TALISMAN_SYSTEM)
	m_pTalismanButton = GetControl<CEtUIRadioButton>("ID_RADIOBUTTON5");
#endif

#if defined( PRE_ADD_COSTUME_SKILL )
	m_pCustumeSkillSlotButton = GetControl<CDnQuickSlotButton>("ID_SKILL");// GetControl<CDnQuickSlotButton>( "ID_ITEM_CASH_SKILL" );
	m_pCustumeSkillSlotButton->SetSlotType( ST_SKILL );
#endif

	m_pSwapWeapon[0] = GetControl<CEtUIButton>( "ID_BUTTON_CROSS1" );
	m_pSwapWeapon[1] = GetControl<CEtUIButton>( "ID_BUTTON_CROSS2" );

	MoveToHead( m_pNormalButton );
	MoveToHead( m_pCashButton );

	m_pBaseInfoDlg = new CDnCharStatusBaseInfoDlg( UI_TYPE_CHILD, this );
	m_pBaseInfoDlg->Initialize( false );

	m_pDetailInfoDlg = new CDnCharStatusDetailInfoDlg( UI_TYPE_CHILD, this );
	m_pDetailInfoDlg->Initialize( false );

	m_pPVPInfoDlg = new CDnCharStatusPVPInfoDlg( UI_TYPE_CHILD, this );
	m_pPVPInfoDlg->Initialize( false );

	m_pAppellationDlg = new CDnAppellationDlg( UI_TYPE_CHILD, this );
	m_pAppellationDlg->Initialize( false );

	m_pPlateDlg = new CDnCharPlateDlg( UI_TYPE_CHILD, this );
	m_pPlateDlg->Initialize( false );

	m_pVehicleDlg = new CDnCharVehicleDlg( UI_TYPE_CHILD, this );
	m_pVehicleDlg->Initialize(false);

	m_pPetButton = GetControl<CEtUIRadioButton>( "ID_RADIOBUTTON2" );
	m_pPetButton->Show( true ); // 디폴트로 안보이게 되있습니다.
	m_pPetDlg = new CDnCharPetDlg( UI_TYPE_CHILD, this );
	m_pPetDlg->Initialize( false );

#ifdef PRE_REMOVE_GUILD_WAR_UI
	m_pGuildWarInfoDlg = new CDnCharStatusGuildWarInfoDlg( UI_TYPE_CHILD, this );
	m_pGuildWarInfoDlg->Initialize( false );
#endif // PRE_REMOVE_GUILD_WAR_UI

#ifdef 	PRE_FIX_HIDE_VEHICLETAB
	m_pVehicleButton->Show( false );
	m_pPetButton->Show(false);
#endif

	m_pLadderInfoDlg = new CDnCharStatusLadderInfoDlg(UI_TYPE_CHILD, this);
	m_pLadderInfoDlg->Initialize(false);

#if defined(PRE_ADD_TALISMAN_SYSTEM)
	m_pTalismanDlg = new CDnCharTalismanDlg(UI_TYPE_CHILD, this);
	m_pTalismanDlg->Initialize(false);
#endif // PRE_ADD_TALISMAN_SYSTEM

	m_pComboBoxInfo = GetControl<CEtUIComboBox>( "ID_COMBOBOX_INFO" );
	for(int i=0; i<MAX_PAGE ; i++)
	{
		int nUIString = 0;

		switch(i)
		{
		case GENERAL_PAGE:
			nUIString = UI_GeneralPage;
			break;
		case DETAILED_PAGE:
			nUIString = UI_DetailedPage;
			break;
		case PVP_PAGE:
			nUIString = UI_PVPPage;
			break;
#ifndef PRE_ADD_PVP_HIDE_LADDERSYSTEM
		case LADDER_PAGE:
			nUIString = UI_LadderPage;
			break;
#endif

#ifdef PRE_REMOVE_GUILD_WAR_UI
		case GUILD_WAR_PAGE:
			nUIString = UI_GuildWarPage;
			break;
#endif // PRE_REMOVE_GUILD_WAR_UI
		}
		m_pComboBoxInfo->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nUIString ),NULL,i,true);
	}

	m_VecSwapPartsButton.clear();
	m_VecSwapPartsButton.resize( CDnParts::SwapParts_Amount );
	m_VecSwapPartsButton[CDnParts::CashHelmet] = GetControl<CEtUIButton>( "ID_BUTTON_CROSS_HEAD" );
	m_VecSwapPartsButton[CDnParts::CashBody] = GetControl<CEtUIButton>( "ID_BUTTON_CROSS_BODY" );
	m_VecSwapPartsButton[CDnParts::CashLeg] = GetControl<CEtUIButton>( "ID_BUTTON_CROSS_LEG" );
	m_VecSwapPartsButton[CDnParts::CashHand] = GetControl<CEtUIButton>( "ID_BUTTON_CROSS_GLOVE" );
	m_VecSwapPartsButton[CDnParts::CashFoot] = GetControl<CEtUIButton>( "ID_BUTTON_CROSS_BOOTS" );
	m_VecSwapPartsButton[CDnParts::CashNecklace] = GetControl<CEtUIButton>( "ID_BUTTON_CROSS_NECKLACE" );
	m_VecSwapPartsButton[CDnParts::CashEarring] = GetControl<CEtUIButton>( "ID_BUTTON_CROSS_EARRING" );
	m_VecSwapPartsButton[CDnParts::CashRing] = GetControl<CEtUIButton>( "ID_BUTTON_CROSS_RING1" );
	m_VecSwapPartsButton[CDnParts::CashRing2] = GetControl<CEtUIButton>( "ID_BUTTON_CROSS_RING2" );
	m_VecSwapPartsButton[CDnParts::CashWing] = GetControl<CEtUIButton>( "ID_BUTTON_CROSS_WING" );
	m_VecSwapPartsButton[CDnParts::CashTail] = GetControl<CEtUIButton>( "ID_BUTTON_CROSS_TALE" );
	m_VecSwapPartsButton[CDnParts::CashFaceDeco] = GetControl<CEtUIButton>( "ID_BUTTON_CROSS_FACEDECO" );
	for( DWORD i=0; i<m_VecSwapPartsButton.size(); i++ ) {
		m_VecSwapPartsButton[i]->Show( true );
	}
	m_pCheckToggleHelmet = GetControl<CEtUICheckBox>( "ID_CHECKBOX_HIDE" );
	m_pCheckToggleHelmet->Show( true );

	m_pNormalButton->SetChecked( true );

	SelectPage();
	SwapEquipButtons( false, true );

	m_SmartMoveEx.SetControl( m_pNormalButton );
}

void CDnCharStatusDlg::HideInfoPage()
{
	ShowChildDialog( m_pBaseInfoDlg, false );
	ShowChildDialog( m_pDetailInfoDlg, false );
	ShowChildDialog( m_pPVPInfoDlg, false );
	ShowChildDialog( m_pLadderInfoDlg, false );
#ifdef PRE_REMOVE_GUILD_WAR_UI
	ShowChildDialog( m_pGuildWarInfoDlg, false );
#endif // PRE_REMOVE_GUILD_WAR_UI
}

void CDnCharStatusDlg::SelectPage()
{
	HideInfoPage();

	switch( m_iPage )
	{
	case GENERAL_PAGE:
		ShowChildDialog( m_pBaseInfoDlg, true );
		break;
	case DETAILED_PAGE:
		ShowChildDialog( m_pDetailInfoDlg, true );
		break;
	case PVP_PAGE:
		ShowChildDialog( m_pPVPInfoDlg, true );
		break;
#ifndef PRE_ADD_PVP_HIDE_LADDERSYSTEM
	case LADDER_PAGE:
		ShowChildDialog( m_pLadderInfoDlg, true );
		break;
#endif
#ifdef PRE_REMOVE_GUILD_WAR_UI
	case GUILD_WAR_PAGE:
		ShowChildDialog( m_pGuildWarInfoDlg, true );
		break;
#endif // PRE_REMOVE_GUILD_WAR_UI
	}

	if( m_pAppellationDlg && m_pAppellationDlg->IsShow() )
		ShowChildDialog( m_pAppellationDlg, true );
}

void CDnCharStatusDlg::InitCustomControl( CEtUIControl *pControl )
{

#ifdef PRE_ADD_COSTUME_SKILL
	if(pControl->GetProperty()->nCustomControlID != idItemSlotButton )
		return;
#endif

	CDnItemSlotButton *pItemSlotButton(NULL);
	pItemSlotButton = static_cast<CDnItemSlotButton*>(pControl);

	if( strstr( pControl->GetControlName(), "ID_ITEM_CASH" ) ) 
	{
		pItemSlotButton->SetSlotIndex((int)m_VecCashEquipSlotButton.size());
		pItemSlotButton->SetSlotType(ST_CHARSTATUS);
		m_VecCashEquipSlotButton.push_back( pItemSlotButton );
	}
	else 
	{
		pItemSlotButton->SetSlotIndex((int)m_VecEquipSlotButton.size());
		pItemSlotButton->SetSlotType(ST_CHARSTATUS);
		m_VecEquipSlotButton.push_back( pItemSlotButton );

		if( strcmp( pControl->GetControlName(), "ID_ITEM_CREST" ) == 0 )
			m_pSourceItemSlot = pItemSlotButton;
	}
}

void CDnCharStatusDlg::SwapPlateDlg( bool bPlate, bool bForce )
{
	HideInfoPage();

	if( bPlate ) 
	{
		ShowEquipButtons( false );
		ShowChildDialog( m_pPlateDlg, true );
		m_pPlateButton->SetChecked( true );

		CONTROL( Static, ID_STATIC0 )->Show( true );
		CONTROL( Static, ID_STATIC0 )->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2230 ) );
	}
	else
	{
		ShowEquipButtons( true );
		ShowChildDialog( m_pPlateDlg, false );
		CONTROL( Static, ID_STATIC0 )->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 35 ) );
	}	

	if( !bForce ) {
		CDnInvenTabDlg *pInvenDlg = (CDnInvenTabDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::INVENTORY_DIALOG );
		if( pInvenDlg && pInvenDlg->IsShow() && !drag::IsValid() ) {
			pInvenDlg->ShowTab( ST_INVENTORY );
		}
	}
}

void CDnCharStatusDlg::ShowEquipButtons( bool bShow )
{
	GetControl<CEtUIStatic>( "ID_STATIC0" )->Show( bShow );
	GetControl<CEtUIStatic>( "ID_STATIC3" )->Show( bShow );
	GetControl<CEtUIStatic>( "ID_STATIC5" )->Show( bShow );
	GetControl<CEtUIStatic>( "ID_STATIC_NAME" )->Show( bShow );
	for( DWORD i=0; i<m_VecEquipSlotButton.size(); i++ ) {
		m_VecEquipSlotButton[i]->Show( bShow );
	}

	for( DWORD i=0; i<m_VecCashEquipSlotButton.size(); i++ ) {
		m_VecCashEquipSlotButton[i]->Show( bShow );
	}

	for( int i=0; i<2; i++ ) {
		m_pSwapWeapon[i]->Show( bShow );
	}
	m_pButtonAppellation->Show( bShow );
	for( DWORD i=0; i<m_VecSwapPartsButton.size(); i++ ) {
		m_VecSwapPartsButton[i]->Show( bShow );
	}
	m_pCheckToggleHelmet->Show( bShow );

#ifdef PRE_ADD_COSTUME_SKILL
	m_pCustumeSkillSlotButton->Show( bShow );
#endif

}


void CDnCharStatusDlg::SwapVehicleDlg(bool bTrue, bool bForce)
{
	HideInfoPage();

	m_pComboBoxInfo->Show(!bTrue);

	if( bTrue ) 
	{
		ShowEquipButtons( false );
		ShowChildDialog( m_pVehicleDlg, true );
		m_pVehicleButton->SetChecked( true );

		CONTROL( Static, ID_STATIC0 )->Show( true );
		CONTROL( Static, ID_STATIC0 )->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 9200 ) );
	}
	else
	{
		ShowEquipButtons( true );
		ShowChildDialog( m_pVehicleDlg, false );
		CONTROL( Static, ID_STATIC0 )->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 35 ) );
	}	

	if( !bForce ) {
		CDnInvenTabDlg *pInvenDlg = (CDnInvenTabDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::INVENTORY_DIALOG );
		if( pInvenDlg && pInvenDlg->IsShow() && !drag::IsValid() ) {
			pInvenDlg->ShowTab( ST_INVENTORY_VEHICLE );
		}
	}

	// 넣어야 됩니다.
}

void CDnCharStatusDlg::SwapPetDlg( bool bTrue, bool bForce )
{
	HideInfoPage();

	m_pComboBoxInfo->Show(!bTrue);

	if( bTrue ) 
	{
		ShowEquipButtons( false );
		ShowChildDialog( m_pPetDlg, true );
		m_pPetButton->SetChecked( true );

		CONTROL( Static, ID_STATIC0 )->Show( true );
		CONTROL( Static, ID_STATIC0 )->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 4504 ) );
	}
	else
	{
		ShowEquipButtons( true );
		ShowChildDialog( m_pPetDlg, false );
		CONTROL( Static, ID_STATIC0 )->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 35 ) );
	}	

	if( !bForce )
	{
		CDnInvenTabDlg *pInvenDlg = (CDnInvenTabDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::INVENTORY_DIALOG );
		if( pInvenDlg && pInvenDlg->IsShow() && !drag::IsValid() )
			pInvenDlg->ShowTab( ST_INVENTORY_VEHICLE );
	}
}

#if defined(PRE_ADD_TALISMAN_SYSTEM)
void CDnCharStatusDlg::ReleaseTalismanClickFlag()
{
	m_pTalismanDlg->ReleaseTalismanClickFlag();
}

void CDnCharStatusDlg::ShowTalismanSlotEffciency(bool bShow)
{ 
	m_pTalismanDlg->ShowTalismanSlotEfficiency(bShow);
}

void CDnCharStatusDlg::RefreshDetailInfoDlg()
{
	m_pTalismanDlg->RefreshDetailInfoDlg();
}

void CDnCharStatusDlg::PlayTalismanMoveSound()
{
	m_pTalismanDlg->PlayTalismanMoveSound();
}

void CDnCharStatusDlg::SwapTalismanDlg( bool bShow, bool bForce )
{
	HideInfoPage();

	m_pComboBoxInfo->Show(!bShow);

	if( bShow ) 
	{
		ShowEquipButtons( false );
		ShowChildDialog( m_pTalismanDlg, true );
		m_pTalismanButton->SetChecked( true );

		CONTROL( Static, ID_STATIC0 )->Show( true );
		CONTROL( Static, ID_STATIC0 )->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1000054149 ) ); // MID : 탈리스만
	}
	else
	{
		ShowEquipButtons( true );
		ShowChildDialog( m_pTalismanDlg, false );
		CONTROL( Static, ID_STATIC0 )->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 35 ) ); // MID : 캐릭터
	}	

	if( !bShow ) {
		CDnInvenTabDlg *pInvenDlg = (CDnInvenTabDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::INVENTORY_DIALOG );
		if( pInvenDlg && pInvenDlg->IsShow() && !drag::IsValid() ) {
			pInvenDlg->ShowTab( ST_INVENTORY );
		}
	}
}
#endif // PRE_ADD_TALISMAN_SYSTEM

void CDnCharStatusDlg::ChangeEquipPage( EquipPageType Type, bool bFocus )
{
	if( Type == m_FocusEquipPage ) return;

	EquipPageType PrevPage = m_FocusEquipPage;
	switch( m_FocusEquipPage ) {
		case EquipPageNormal:
		case EquipPageCash:
			break;
		case EquipPagePlate:
			SwapPlateDlg( false, bFocus );
			break;
		case EquipPageVehicle:
			SwapVehicleDlg( false, bFocus );
			break;
		case EquipPagePet:
			SwapPetDlg( false, bFocus );
			break;
#if defined(PRE_ADD_TALISMAN_SYSTEM)
		case EquipPageTalisman:
			SwapTalismanDlg( false, bFocus );
			break;
#endif // PRE_ADD_TALISMAN_SYSTEM
	}

	m_FocusEquipPage = Type;
	switch( Type ) {
		case EquipPageNormal:
			SwapEquipButtons( false, bFocus );
			break;
		case EquipPageCash:
			SwapEquipButtons( true, bFocus );
			break;
		case EquipPagePlate:
			SwapPlateDlg( true, bFocus );
			break;

		case EquipPageVehicle:
			SwapVehicleDlg(true,bFocus);
			break;
		case EquipPagePet:
			SwapPetDlg( true, bFocus );
			break;
#if defined(PRE_ADD_TALISMAN_SYSTEM)
		case EquipPageTalisman:
			SwapTalismanDlg( true, bFocus );
			break;
#endif // PRE_ADD_TALISMAN_SYSTEM
	}

	MoveToTail( m_pNormalButton );
	MoveToTail( m_pCashButton );
	MoveToTail( m_pPlateButton );
	MoveToTail( m_pVehicleButton );
	MoveToTail( m_pPetButton );
#if defined(PRE_ADD_TALISMAN_SYSTEM)
	MoveToTail( m_pTalismanButton );
#endif // PRE_ADD_TALISMAN_SYSTEM

	if( m_pAppellationDlg && m_pAppellationDlg->IsShow() )
		ShowChildDialog( m_pAppellationDlg, true );
}

void CDnCharStatusDlg::SwapEquipButtons( bool bCash, bool bForce )
{
	if( !bForce && m_bFocusCashEquip == bCash ) return;  

	m_bFocusCashEquip = bCash;

	for( DWORD i=0; i<m_VecSwapPartsButton.size(); i++ ) {
		m_VecSwapPartsButton[i]->Show( true );
	}

#ifdef PRE_ADD_COSTUME_SKILL
	m_pCustumeSkillSlotButton->Show( bCash );
#endif

	m_pCheckToggleHelmet->Show( true );
	RefreshHideHelmetCheckBox();
	CDnItemSlotButton *pSlot;
	if( !m_bFocusCashEquip ) {
		for( DWORD i=0; i<m_VecCashEquipSlotButton.size(); i++ ) {
			pSlot = m_VecCashEquipSlotButton[i];
			pSlot->Show( false );
			MoveToHead( pSlot );
		}
		MoveToTail( m_pNormalEquipBase );
		m_pNormalEquipBase->Show( true );

		for( DWORD i=0; i<m_VecEquipSlotButton.size(); i++ ) {
			pSlot = m_VecEquipSlotButton[i];
			pSlot->Show( true );
			MoveToTail( pSlot );
		}
		MoveToHead( m_pCashEquipBase );
		m_pCashEquipBase->Show( false );
		m_pNormalButton->SetChecked( true );

		for( DWORD i=CDnParts::CashWing; i<m_VecSwapPartsButton.size(); i++ ) {
			m_VecSwapPartsButton[i]->Show( false );
		}
	}
	else {
		for( DWORD i=0; i<m_VecEquipSlotButton.size(); i++ ) {
			pSlot = m_VecEquipSlotButton[i];
			pSlot->Show( false );
			MoveToHead( pSlot );
		}
		MoveToTail( m_pCashEquipBase );
		m_pCashEquipBase->Show( true );

		for( DWORD i=0; i<m_VecCashEquipSlotButton.size(); i++ ) {
			pSlot = m_VecCashEquipSlotButton[i];
			pSlot->Show( true );
			MoveToTail( pSlot );
		}
		MoveToHead( m_pNormalEquipBase );
		m_pNormalEquipBase->Show( false );
		m_pCashButton->SetChecked( true );

	}
	for( int i=0; i<2; i++ ) {
		m_pSwapWeapon[i]->Show( true );
		MoveToTail( m_pSwapWeapon[i] );
	}
	for( DWORD i=0; i<m_VecSwapPartsButton.size(); i++ ) {
		MoveToTail( m_VecSwapPartsButton[i] );
	}
	MoveToTail( m_pCheckToggleHelmet );
#ifdef PRE_ADD_COSTUME_SKILL
	MoveToTail( m_pCustumeSkillSlotButton );
#endif
	RefreshOnepieceBlock();

	MoveToTail( m_pNormalButton );
	MoveToTail( m_pCashButton );
	MoveToTail( m_pButtonAppellation );
	MoveToTail( m_pStaticName );
	MoveToTail( m_pComboBoxInfo );

	if( !bForce ) {
		CDnInvenTabDlg *pInvenDlg = (CDnInvenTabDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::INVENTORY_DIALOG );
		if( pInvenDlg && pInvenDlg->IsShow() && !drag::IsValid() ) {
			if( m_bFocusCashEquip ) pInvenDlg->ShowTab( ST_INVENTORY_CASH );
			else pInvenDlg->ShowTab( ST_INVENTORY );
		}
	}
}

void CDnCharStatusDlg::RefreshHideHelmetCheckBox()
{
	if( !m_pCheckToggleHelmet ) return;
	if( CDnActor::s_hLocalActor ) {
		CDnLocalPlayerActor *pPlayer = static_cast<CDnLocalPlayerActor *>(CDnActor::s_hLocalActor.GetPointer());
		m_pCheckToggleHelmet->SetChecked( pPlayer->IsHideHelmet(), false );
	}
}

void CDnCharStatusDlg::RefreshOnepieceBlock()
{
	for( int i=0; i<(int)m_VecCashEquipSlotButton.size(); i++ )
	{
		m_VecCashEquipSlotButton[i]->SetClosed( false );
	}

	if( m_bFocusCashEquip ) {
		for( int i=0; i<CASHEQUIPMAX; i++ )
		{

			if(i==CASHEQUIP_EFFECT)
				continue;

			if( i > CDnParts::CashFairy ) continue;
			CDnItem *pItem = GetItemTask().GetCashEquipItem(i);
			if( !pItem ) continue;
			CDnParts *pParts = static_cast<CDnParts *>(pItem);
			if( !pParts->IsExistSubParts() ) continue;
			for( int j=0; j<pParts->GetSubPartsCount(); j++ ) {
				CDnParts::PartsTypeEnum SubPartsType = pParts->GetSubPartsIndex(j);
				m_VecCashEquipSlotButton[SubPartsType]->SetClosed( true );
			}
		}

	}

	for( int i=0; i<(int)m_VecSwapPartsButton.size(); i++ )
	{
		m_VecSwapPartsButton[i]->Enable( true );
	}
	for( int i=0; i<CASHEQUIPMAX; i++ )
	{

		if(i==CASHEQUIP_EFFECT)
			continue;

		if( i > CDnParts::CashFairy ) continue;
		CDnItem *pItem = GetItemTask().GetCashEquipItem(i);
		if( !pItem ) continue;
		CDnParts *pParts = static_cast<CDnParts *>(pItem);
		if( !pParts->IsExistSubParts() ) continue;
		for( int j=0; j<pParts->GetSubPartsCount(); j++ )
		{
			CDnParts::PartsTypeEnum SubPartsType = pParts->GetSubPartsIndex(j);			
			m_VecSwapPartsButton[SubPartsType]->Enable( false );
		}
	}
}

void CDnCharStatusDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_RADIOBUTTON_CHANGED ) {
		if( IsCmdControl( "ID_RADIOBUTTON0" ) ) {
			ChangeEquipPage( EquipPageNormal );
			SelectPage();
		}
		if( IsCmdControl( "ID_RADIOBUTTON1" ) ) {
			ChangeEquipPage( EquipPageCash );
			SelectPage();
		}
		if( IsCmdControl( "ID_RADIOBUTTON4" ) ) {
			ChangeEquipPage( EquipPagePlate );
		}
		if( IsCmdControl( "ID_RADIOBUTTON3" ) ) {
			ChangeEquipPage( EquipPageVehicle );
		}
		if( IsCmdControl( "ID_RADIOBUTTON2" ) ) {
			ChangeEquipPage( EquipPagePet );
		}
#if defined(PRE_ADD_TALISMAN_SYSTEM)
		if( IsCmdControl("ID_RADIOBUTTON5") ) {
			ChangeEquipPage( EquipPageTalisman );
		}
#endif
	}
	if( nCommand == EVENT_COMBOBOX_SELECTION_CHANGED)
	{
		if( IsCmdControl("ID_COMBOBOX_INFO" ) )
		{
			m_iPage = m_pComboBoxInfo->GetSelectedIndex();
			if(  m_iPage >= MAX_PAGE )
				m_iPage = GENERAL_PAGE;
			SelectPage();
			return;
		}
	}

	if(nCommand == EVENT_COMBOBOX_DROPDOWN_OPENED)
	{
		if( IsCmdControl("ID_COMBOBOX_INFO" ) )
		{
			HideInfoPage();
		}
	}
	if(nCommand == EVENT_COMBOBOX_DROPDOWN_CLOSED)
	{
		if( IsCmdControl("ID_COMBOBOX_INFO" ) )
		{
			SelectPage();
		}
	}

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl( "ID_BUTTON_APPE" ) ) 
		{
			if( CDnActor::s_hLocalActor ) {
				CDnLocalPlayerActor *pActor = ((CDnLocalPlayerActor *)CDnActor::s_hLocalActor.GetPointer());
				if( pActor->IsDie() || pActor->IsGhost() ) {
					GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 937 ), MB_OK );
					return;
				}
			}

			ShowChildDialog( m_pAppellationDlg, !m_pAppellationDlg->IsShow() );
		}
		if( IsCmdControl( "ID_BUTTON_CROSS1" ) ) {
			if( CDnActor::s_hLocalActor ) {
				CDnLocalPlayerActor *pPlayer = dynamic_cast<CDnLocalPlayerActor *>(CDnActor::s_hLocalActor.GetPointer());
				if(!pPlayer->IsSwapSingleSkin()
					&& !pPlayer->IsShootMode()
					)
					pPlayer->CmdToggleWeaponViewOrder( 0, !pPlayer->IsViewWeaponOrder( 0 ) );
			}
		}
		if( IsCmdControl( "ID_BUTTON_CROSS2" ) ) {
			if( CDnActor::s_hLocalActor ) {
				CDnLocalPlayerActor *pPlayer = dynamic_cast<CDnLocalPlayerActor *>(CDnActor::s_hLocalActor.GetPointer());
				if(!pPlayer->IsSwapSingleSkin()
					&& !pPlayer->IsShootMode()
					)
					pPlayer->CmdToggleWeaponViewOrder( 1, !pPlayer->IsViewWeaponOrder( 1 ) );
			}
		}

		// 캐시파츠 룩 전환버튼
		if( strstr( pControl->GetControlName(), "ID_BUTTON_CROSS_" ) ){
			DWORD dwIndex;
			for( dwIndex=0; dwIndex<m_VecSwapPartsButton.size(); dwIndex++ ) {
				if( pControl == m_VecSwapPartsButton[dwIndex] )
					break;
			}
			if( dwIndex < m_VecSwapPartsButton.size() ) {
				if( CDnActor::s_hLocalActor ) {
					CDnLocalPlayerActor *pPlayer = static_cast<CDnLocalPlayerActor *>(CDnActor::s_hLocalActor.GetPointer());
					if(!pPlayer->IsSwapSingleSkin())
						pPlayer->CmdTogglePartsViewOrder( dwIndex, !pPlayer->IsViewPartsOrder( dwIndex ) );
				}
			}
		}

		// 일반탬
		if( !m_bFocusCashEquip && strstr( pControl->GetControlName(), "ID_ITEM_" ) )
		{
			CDnSlotButton *pDragButton;
			CDnItemSlotButton *pPressedButton;

			if( GetItemTask().IsRequestWait() ) return;
			if( GetInterface().GetFadeDlg() && (GetInterface().GetFadeDlg()->GetFadeMode() != CDnFadeInOutDlg::modeEnd) )
				return;

			if( CDnActor::s_hLocalActor )
			{
				CDnLocalPlayerActor* pLocalPlayerActor = static_cast<CDnLocalPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
				if( pLocalPlayerActor && pLocalPlayerActor->IsObserver() )
					return;
			}

			// 기본적으로 거래중엔 캐릭터창이 안뜰테지만, 혹시 뜨더라도 장비해제 및 교체를 막아두겠다.
			CDnInvenTabDlg *pInvenDlg = (CDnInvenTabDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::INVENTORY_DIALOG );
			if( pInvenDlg && pInvenDlg->IsShowPrivateMarketDialog() ) return;

			if( strcmp(pControl->GetControlName(), "ID_ITEM_CREST") == 0 )
				return;

			focus::ReleaseControl();

			pDragButton = ( CDnSlotButton * )drag::GetControl();
			pPressedButton = ( CDnItemSlotButton * )pControl;

			if( pDragButton == NULL )
			{
				if( !pPressedButton->GetItem() ) 
					return;

				CDnItem *pItem = dynamic_cast<CDnItem *>(pPressedButton->GetItem());
				if( pItem )
				{
					if( uMsg == WM_RBUTTONUP )
					{
#ifdef PRE_ADD_EQUIPLOCK
						if (CDnItemTask::IsActive())
						{
							const CDnItemLockMgr& lockMgr = CDnItemTask::GetInstance().GetItemLockMgr();
							EquipItemLock::eLockStatus status = lockMgr.GetLockState(*pItem);
							if (status == EquipItemLock::Lock)
							{
								CDnGameTask *pGameTask = static_cast<CDnGameTask*>(CTaskManager::GetInstance().GetTask("GameTask"));
								if (pGameTask && pGameTask->GetGameTaskType() == GameTaskType::PvP) 
								{
									CDnPvPGameTask* pPvPGameTask = static_cast<CDnPvPGameTask*>(pGameTask);
									if (pPvPGameTask && pPvPGameTask->GetGameMode() == PvPCommon::GameMode::PvP_Zombie_Survival)
									{
										GetInterface().MessageBox(8431, MB_OK); // UISTRING : 구울 모드에서는 장비 잠금 및 해제가 불가능합니다
										return;
									}
								}

								m_pTempSlotItem = pItem;

								int nUnlockTerm = (int)CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::ItemUnLockWaitTime);
								std::wstring str = FormatW(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 8390), nUnlockTerm); // UISTRING : 잠금 해제에 72시간이 필요합니다. 선택한 아이템을 해제 상태로 변환하시겠습니까?
								GetInterface().MessageBox(str.c_str(), MB_YESNO, EQUIPUNLOCK_CONFIRM_DIALOG, this);
								return;
							}
							else if (status == EquipItemLock::RequestUnLock)
							{
								return;
							}
						}
#endif

						if( !GetInterface().IsEmptySlot( CDnMainMenuDlg::INVENTORY_DIALOG, ITEM_SLOT_TYPE::ST_INVENTORY ) )
						{
							GetInterface().MessageBox( 1925, MB_OK );
							return;
						}

						GetItemTask().RequestMoveItem( MoveType_EquipToInven, SLOTINDEX_2_EQUIPINDEX(pPressedButton->GetSlotIndex()), pItem->GetSerialID(),
							GetInterface().GetEmptySlot( CDnMainMenuDlg::INVENTORY_DIALOG ), pItem->GetOverlapCount() );
						return;
					}
#ifdef PRE_ADD_EQUIPLOCK
					else if (uMsg == WM_LBUTTONDOWN)
					{
						const CDnItemLockMgr& lockMgr = CDnItemTask::GetInstance().GetItemLockMgr();
						EquipItemLock::eLockStatus status = lockMgr.GetLockState(*pItem);
						if (status == EquipItemLock::Lock)
							return;
					}
#endif

					// todo by kalliste : refactoring with DnInvenSlotDlg.cpp
					if( uMsg & VK_SHIFT )
					{
						if (GetInterface().SetNameLinkChat(*pItem))
							return;
					}

					CEtSoundEngine::GetInstance().PlaySound( "2D", pItem->GetDragSoundIndex() );
				}
				else
				{
					if( uMsg == WM_RBUTTONUP || uMsg & VK_SHIFT )
						return;
				}

				pPressedButton->EnableSplitMode(1);
				drag::SetControl(pPressedButton);
			}
			else
			{
				pDragButton->DisableSplitMode(true);
				drag::ReleaseControl();

				int nEquipIndexP = SLOTINDEX_2_EQUIPINDEX(pPressedButton->GetSlotIndex());
				if( nEquipIndexP == -1 ) return;

				if( pDragButton->GetItem() ) 
				{
					if( pDragButton == pPressedButton )
					{
						// 제자리 이동
						CDnItem *pItem = dynamic_cast<CDnItem *>(pDragButton->GetItem());
						if( pItem ) CEtSoundEngine::GetInstance().PlaySound( "2D", pItem->GetDragSoundIndex() );
						// #23466 이슈관련. IsUsableFocusControl 처리때문에 슬롯이 눌리게 된다.
						// 현재 IsUsableFocusControl안에서 마우스좌표로 컨트롤 확인을 할 수 없기때문에,
						// focus 컨트롤을 해제하는걸로 우선 해결해둔다.
						focus::ReleaseControl();
						return;
					}

					switch( pDragButton->GetItem()->GetType() ) 
					{
						case MIInventoryItem::Item:
							{
								if( ((CDnItem*)pDragButton->GetItem())->IsSoulbBound() == false )
								{
									GetInterface().MessageBox( 1752, MB_OK );
									return;
								}
								if( GetItemTask().IsEquipItem( nEquipIndexP, ((CDnItem*)pDragButton->GetItem()), m_strLastErrorMsg ) == false ) 
								{
									GetInterface().MessageBox( m_strLastErrorMsg.c_str(), MB_OK );
									return;
								}
#ifdef PRE_ADD_EQUIPLOCK
								CDnItem* pDragItem = static_cast<CDnItem*>(pDragButton->GetItem());
								if (pDragItem == NULL)
								{
									_ASSERT(0);
									return;
								}

								if( pDragItem->IsCashItem() ) {
									m_strLastErrorMsg = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 5056 );
									GetInterface().MessageBox( m_strLastErrorMsg.c_str(), MB_OK );
									return;
								}

								CDnItem* pPressedItem = static_cast<CDnItem*>(pPressedButton->GetItem());

								if (pPressedItem != NULL)
								{
									const CDnItemLockMgr& lockMgr = CDnItemTask::GetInstance().GetItemLockMgr();
									EquipItemLock::eLockStatus status = lockMgr.GetLockState(*pPressedItem);
									if (status != EquipItemLock::None)
									{
										m_strLastErrorMsg = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 8389); // UISTRING : 잠금 상태에서는 장비 변경이 불가능 합니다.
										GetInterface().MessageBox(m_strLastErrorMsg.c_str(), MB_OK);
										return;
									}
								}
#else
								if( ((CDnItem*)pDragButton->GetItem())->IsCashItem() ) {
									m_strLastErrorMsg = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 5056 );
									GetInterface().MessageBox( m_strLastErrorMsg.c_str(), MB_OK );
									return;
								}
#endif

								eEquipType tempEquipIdx = (eEquipType)nEquipIndexP;
								CDnCharStatusDlg::eRetWearable ret = GetWearableEquipType( pDragButton->GetItem(), tempEquipIdx );
								if(ret != CDnCharStatusDlg::eWEAR_ENABLE)
								{
									if (ret == CDnCharStatusDlg::eWEAR_UNABLE)
										GetInterface().MessageBox( m_strLastErrorMsg.c_str(), MB_OK );
									else if (ret == CDnCharStatusDlg::eWEAR_NEED_UNSEAL)
										GetInterface().OpenItemUnsealDialog( pDragButton->GetItem(), pDragButton );
									return;
								}
							}
							break;
						default:
							break;
					}
				}

				switch( pDragButton->GetSlotType() ) 
				{
				case ST_INVENTORY:
					{
						CDnItem *pDrag = (CDnItem*)pDragButton->GetItem();
						GetItemTask().RequestMoveItem( MoveType_InvenToEquip, pDragButton->GetSlotIndex(), pDrag->GetSerialID(), nEquipIndexP, CDnInventoryDlg::GetCurrentAmount() );
					}
					break;
				case ST_CHARSTATUS:
					{
						int nEquipIndexD = SLOTINDEX_2_EQUIPINDEX(pDragButton->GetSlotIndex());
						if( nEquipIndexD == -1 ) return;

						/*
						if( nEquipIndexD < EQUIP_NECKLACE || nEquipIndexD > EQUIP_RING2 ||
							nEquipIndexP < EQUIP_NECKLACE || nEquipIndexP > EQUIP_RING2 ) {
								// 교환할 수 없는 품목일 때.
						}
						*/

						CDnItem *pDrag = (CDnItem*)pDragButton->GetItem();
						GetItemTask().RequestMoveItem( MoveType_Equip, nEquipIndexD, pDrag->GetSerialID(), nEquipIndexP, 1 );
					}
					break;
				case ST_QUICKSLOT:
					return;
				}
			}

			return;
		}
		// 캐쉬탬
		if( m_bFocusCashEquip && strstr( pControl->GetControlName(), "ID_ITEM_CASH_" ) )
		{
			CDnSlotButton *pDragButton;
			CDnItemSlotButton *pPressedButton;

			if( GetItemTask().IsRequestWait() ) return;

			if( GetInterface().GetFadeDlg() && (GetInterface().GetFadeDlg()->GetFadeMode() != CDnFadeInOutDlg::modeEnd) )
				return;

			if( CDnActor::s_hLocalActor )
			{
				CDnLocalPlayerActor* pLocalPlayerActor = static_cast<CDnLocalPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
				if( pLocalPlayerActor && pLocalPlayerActor->IsObserver() )
					return;
			}

			// 기본적으로 거래중엔 캐릭터창이 안뜰테지만, 혹시 뜨더라도 장비해제 및 교체를 막아두겠다.
			CDnInvenTabDlg *pInvenDlg = (CDnInvenTabDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::INVENTORY_DIALOG );
			if( pInvenDlg && pInvenDlg->IsShowPrivateMarketDialog() ) return;

			focus::ReleaseControl();

			pDragButton = ( CDnSlotButton * )drag::GetControl();
			pPressedButton = ( CDnItemSlotButton * )pControl;

			if( pDragButton == NULL )
			{
				if( !pPressedButton->GetItem() ) 
					return;

				CDnItem *pItem = dynamic_cast<CDnItem *>(pPressedButton->GetItem());

				if( pItem )
				{
					if( uMsg == WM_RBUTTONUP )
					{
#ifdef PRE_ADD_EQUIPLOCK
						if (CDnItemTask::IsActive())
						{
							const CDnItemLockMgr& lockMgr = CDnItemTask::GetInstance().GetItemLockMgr();
							EquipItemLock::eLockStatus status = lockMgr.GetLockState(*pItem);
							if (status == EquipItemLock::Lock)
							{
								m_pTempSlotItem = pItem;
								int nUnlockTerm = (int)CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::ItemUnLockWaitTime);
								std::wstring str = FormatW(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 8390), nUnlockTerm); // UISTRING : 잠금 해제에 72시간이 필요합니다. 선택한 아이템을 해제 상태로 변환하시겠습니까?
								GetInterface().MessageBox(str.c_str(), MB_YESNO, EQUIPUNLOCK_CONFIRM_DIALOG, this);
								return;
							}
							else if (status == EquipItemLock::RequestUnLock)
							{
								return;
							}
						}
#endif

						GetItemTask().RequestMoveItem( MoveType_CashEquipToCashInven, CASH_SLOTINDEX_2_EQUIPINDEX(pPressedButton->GetSlotIndex()), pItem->GetSerialID(),
							GetItemTask().GetCashInventory().FindFirstEmptyIndex(), pItem->GetOverlapCount() );
						return;
					}
#ifdef PRE_ADD_EQUIPLOCK
					else if (uMsg == WM_LBUTTONDOWN)
					{
						const CDnItemLockMgr& lockMgr = CDnItemTask::GetInstance().GetItemLockMgr();
						EquipItemLock::eLockStatus status = lockMgr.GetLockState(*pItem);
						if (status == EquipItemLock::Lock)
							return;
					}
#endif

					// todo by kalliste : refactoring with DnInvenSlotDlg.cpp
					if (uMsg & VK_SHIFT)
					{
						if (GetInterface().SetNameLinkChat(*pItem))
							return;
					}

					CEtSoundEngine::GetInstance().PlaySound( "2D", pItem->GetDragSoundIndex() );
				}
				else
				{
					if( uMsg == WM_RBUTTONUP || uMsg & VK_SHIFT )
						return;
				}

				pPressedButton->EnableSplitMode(1);
				drag::SetControl(pPressedButton);
			
			}
			else
			{
				pDragButton->DisableSplitMode(true);
				drag::ReleaseControl();

				int nEquipIndexP = CASH_SLOTINDEX_2_EQUIPINDEX(pPressedButton->GetSlotIndex());
				if( nEquipIndexP == -1 ) return;

				if( pDragButton->GetItem() ) 
				{
					if( pDragButton == pPressedButton )
					{
						// 제자리 이동
						CDnItem *pItem = dynamic_cast<CDnItem *>(pDragButton->GetItem());
						if( pItem ) CEtSoundEngine::GetInstance().PlaySound( "2D", pItem->GetDragSoundIndex() );
						focus::ReleaseControl();
						return;
					}

					switch( pDragButton->GetItem()->GetType() ) 
					{
					case MIInventoryItem::Item:
						{
							if( ((CDnItem*)pDragButton->GetItem())->IsSoulbBound() == false )
							{
								GetInterface().MessageBox( 1752, MB_OK );
								return;
							}

							if( GetItemTask().IsEquipItem( nEquipIndexP, ((CDnItem*)pDragButton->GetItem()), m_strLastErrorMsg ) == false ) 
							{
								GetInterface().MessageBox( m_strLastErrorMsg.c_str(), MB_OK );
								return;
							}
#ifdef PRE_ADD_EQUIPLOCK
							const CDnItem* pItem = static_cast<const CDnItem*>(pDragButton->GetItem());
							if (pItem == NULL)
							{
								_ASSERT(0);
								return;
							}

							if( !pItem->IsCashItem() ) {
								m_strLastErrorMsg = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 5056 );
								GetInterface().MessageBox( m_strLastErrorMsg.c_str(), MB_OK );
								return;
							}

							CDnItem* pPressedItem = static_cast<CDnItem*>(pPressedButton->GetItem());

							if (pPressedItem != NULL)
							{
								const CDnItemLockMgr& lockMgr = CDnItemTask::GetInstance().GetItemLockMgr();
								EquipItemLock::eLockStatus status = lockMgr.GetLockState(*pPressedItem);
								if (status != EquipItemLock::None)
								{
									m_strLastErrorMsg = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 8389); // UISTRING : 잠금 상태에서는 장비 변경이 불가능 합니다.
									GetInterface().MessageBox(m_strLastErrorMsg.c_str(), MB_OK);
									return;
								}
							}
#else
							if( !((CDnItem*)pDragButton->GetItem())->IsCashItem() ) {
								m_strLastErrorMsg = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 5056 );
								GetInterface().MessageBox( m_strLastErrorMsg.c_str(), MB_OK );
								return;
							}
#endif
							eCashEquipType tempEquipIdx = (eCashEquipType)nEquipIndexP;
							CDnCharStatusDlg::eRetWearable ret = GetWearableCashEquipType( pDragButton->GetItem(), tempEquipIdx );
							if(ret != CDnCharStatusDlg::eWEAR_ENABLE)
							{
								if (ret == CDnCharStatusDlg::eWEAR_UNABLE)
									GetInterface().MessageBox( m_strLastErrorMsg.c_str(), MB_OK );
								else if (ret == CDnCharStatusDlg::eWEAR_NEED_UNSEAL)
									GetInterface().OpenItemUnsealDialog( pDragButton->GetItem(), pDragButton );
								return;
							}
						}
						break;
					default:
						break;
					}
				}

				switch( pDragButton->GetSlotType() ) 
				{
				case ST_INVENTORY_CASH:
					{
#ifdef PRE_ADD_CASHINVENTAB
						CDnItem *pItem = dynamic_cast<CDnItem *>(pDragButton->GetItem());
						pItem = GetCashInvenItemBySN( pItem->GetSerialID() );
						if( pItem )
							GetItemTask().RequestMoveItem( MoveType_CashInvenToCashEquip, pItem->GetSlotIndex(), pItem->GetSerialID(), nEquipIndexP, CDnInventoryDlg::GetCurrentAmount() );
						
#else
						CDnItem *pItem = dynamic_cast<CDnItem *>(pDragButton->GetItem());
						GetItemTask().RequestMoveItem( MoveType_CashInvenToCashEquip, pDragButton->GetSlotIndex(), pItem->GetSerialID(), nEquipIndexP, CDnInventoryDlg::GetCurrentAmount() );
#endif // PRE_ADD_CASHINVENTAB
					}
					break;
				case ST_CHARSTATUS:
					{
						int nEquipIndexD = CASH_SLOTINDEX_2_EQUIPINDEX(pDragButton->GetSlotIndex());
						if( nEquipIndexD == -1 ) return;
						if( nEquipIndexD == CASHEQUIP_RING1 || nEquipIndexD == CASHEQUIP_RING2 ) return;

						CDnItem *pItem = dynamic_cast<CDnItem *>(pDragButton->GetItem());
						GetItemTask().RequestMoveItem( MoveType_CashEquip, nEquipIndexD, pItem->GetSerialID(), nEquipIndexP, 1 );
					}
					break;
				case ST_QUICKSLOT:
					return;
				}
			}

			return;
		}
	}

	if( nCommand == EVENT_CHECKBOX_CHANGED ) {
		if( IsCmdControl( "ID_CHECKBOX_HIDE" ) ) {
			if( CDnActor::s_hLocalActor ) {
				CDnLocalPlayerActor *pPlayer = static_cast<CDnLocalPlayerActor *>(CDnActor::s_hLocalActor.GetPointer());
				if(!pPlayer->IsSwapSingleSkin())
					pPlayer->CmdToggleHideHelmet( !pPlayer->IsHideHelmet() );
				m_pCheckToggleHelmet->SetChecked( pPlayer->IsHideHelmet(), false );
			}
		}
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnCharStatusDlg::Render( float fElapsedTime )
{
	CEtUIDialog::Render( fElapsedTime );

	if( !IsShow() )
		return;

	if( !CDnActor::s_hLocalActor )
		return;

	DnActorHandle hActor = CDnActor::s_hLocalActor;
	m_pStaticName->SetText( hActor->GetName() );

	if( drag::IsValid() )
	{
		CDnSlotButton *pDragButton = (CDnSlotButton*)drag::GetControl();
		switch( pDragButton->GetSlotType() ) {
			case ST_INVENTORY:
			case ST_INVENTORY_CASH:
			case ST_CHARSTATUS:
			case ST_INVENTORY_VEHICLE:
				break;
			
			default: 
				return;
		}

		CDnItem *pItem = dynamic_cast<CDnItem *>(pDragButton->GetItem());
		if( !pItem ) return;

		switch( pItem->GetItemType() ) {
			case ITEMTYPE_GLYPH:
				{
					if( pDragButton->GetSlotType() != ST_INVENTORY && 
						pDragButton->GetSlotType() != ST_INVENTORY_CASH  &&
						pDragButton->GetSlotType() != ST_INVENTORY_VEHICLE) break;

					bool bShowMagnetic = true;
					eGlyph tempEquipType;
					bShowMagnetic = (GetWearableGlyphEquipType( pDragButton->GetItem(), tempEquipType ) == eWEAR_ENABLE);

					if (bShowMagnetic)
						MoveToTail( m_pMagnetic );
					else
						MoveToHead( m_pMagnetic );
					m_pMagnetic->Show( bShowMagnetic );
				}
				break;

#ifdef PRE_ADD_TALISMAN_SYSTEM
			case ITEMTYPE_TALISMAN:
				{
					if( pDragButton->GetSlotType() != ST_INVENTORY && 
						pDragButton->GetSlotType() != ST_INVENTORY_CASH  &&
						pDragButton->GetSlotType() != ST_INVENTORY_VEHICLE) break;

					CDnCharStatusDlg* pCharStatusDlg = (CDnCharStatusDlg*)GetInterface().GetMainMenuDialog(CDnMainMenuDlg::CHARSTATUS_DIALOG);
					if(pCharStatusDlg && pCharStatusDlg->IsShow()) {
						pCharStatusDlg->ShowTalismanSlotEffciency(true);
					}
				}
				break;
#endif // PRE_ADD_TALISMAN_SYSTEM
		
			case ITEMTYPE_VEHICLEEFFECT:
			case ITEMTYPE_VEHICLEPARTS:
			case ITEMTYPE_VEHICLE:
			case ITEMTYPE_VEHICLE_SHARE:
				{
					if(pItem->GetType() == ITEMTYPE_VEHICLEPARTS)
					{
						DNTableFileFormat* pVehiclePartsTable = GetDNTable( CDnTableDB::TVEHICLEPARTS );
						if( !pVehiclePartsTable || !pVehiclePartsTable->IsExistItem(pItem->GetClassID())) 
							break;

						int equipType = -1;
						equipType = pVehiclePartsTable->GetFieldFromLablePtr( pItem->GetClassID(), "_VehiclePartsType" )->GetInteger();

						if(equipType == Vehicle::Parts::Hair)
							break;
					}

					if( pDragButton->GetSlotType() != ST_INVENTORY && 
						pDragButton->GetSlotType() != ST_INVENTORY_CASH  &&
						pDragButton->GetSlotType() != ST_INVENTORY_VEHICLE) break;
					MoveToTail( m_pMagnetic );
					m_pMagnetic->Show( true );
				}
				break;

			default:
#ifdef PRE_ADD_EQUIPLOCK
				if (m_bShowEquipLockMagnetic)
				{
					if (m_pEquipLockMagnetic != NULL && m_pEquipLockMagnetic->IsShow() == false)
					{
						eItemTypeEnum curItemType = pItem->GetItemType();
						if (CommonUtil::IsLockableEquipItem(curItemType))
						{
							bool bNoEquipMagnetic = false;
							CDnGameTask *pGameTask = static_cast<CDnGameTask*>(CTaskManager::GetInstance().GetTask("GameTask"));
							if (pGameTask && pGameTask->GetGameTaskType() == GameTaskType::PvP) 
							{
								CDnPvPGameTask* pPvPGameTask = static_cast<CDnPvPGameTask*>(pGameTask);
								if (pPvPGameTask && pPvPGameTask->GetGameMode() == PvPCommon::GameMode::PvP_Zombie_Survival)
									bNoEquipMagnetic = true;
							}

							if (bNoEquipMagnetic == false)
							{
								MoveToTail(m_pEquipLockMagnetic);
								m_pEquipLockMagnetic->Show(true);
								EnableRadioButton(false);
							}
						}
					}
				}
#endif

				if( pDragButton->GetSlotType() != ST_INVENTORY && 
					pDragButton->GetSlotType() != ST_INVENTORY_CASH  &&
					pDragButton->GetSlotType() != ST_INVENTORY_VEHICLE) break;

				if( GetWithDrawOpen() )
					return;

				if( pItem->IsCashItem() ) {
					eCashEquipType equipType;
					if( GetWearableCashEquipType( pDragButton->GetItem(), equipType ) == CDnCharStatusDlg::eWEAR_ENABLE )
					{
						int nSlotIndex(-1);

						if( equipType == CASHEQUIP_RING1 || equipType == CASHEQUIP_RING2 )
						{
							nSlotIndex = CASH_EQUIPINDEX_2_SLOTINDEX(CASHEQUIP_RING1);
							m_VecCashEquipSlotButton[nSlotIndex]->SetWearable( true );
							nSlotIndex = CASH_EQUIPINDEX_2_SLOTINDEX(CASHEQUIP_RING2);
							m_VecCashEquipSlotButton[nSlotIndex]->SetWearable( true );
						}
						else
						{
							nSlotIndex = CASH_EQUIPINDEX_2_SLOTINDEX(equipType);
							if( nSlotIndex != -1 )
							{
								m_VecCashEquipSlotButton[nSlotIndex]->SetWearable( true );
							}
						}

						MoveToTail( m_pMagnetic );
						m_pMagnetic->Show( true );
					}
				}
				else {
					eEquipType equipType;
					if( GetWearableEquipType( pDragButton->GetItem(), equipType ) == eWEAR_ENABLE)
					{
						int nSlotIndex(-1);

						if( equipType == EQUIP_RING1 || equipType == EQUIP_RING2 )
						{
							nSlotIndex = EQUIPINDEX_2_SLOTINDEX(EQUIP_RING1);
							m_VecEquipSlotButton[nSlotIndex]->SetWearable( true );
							nSlotIndex = EQUIPINDEX_2_SLOTINDEX(EQUIP_RING2);
							m_VecEquipSlotButton[nSlotIndex]->SetWearable( true );
						}
						else
						{
							nSlotIndex = EQUIPINDEX_2_SLOTINDEX(equipType);
							if( nSlotIndex != -1 )
							{
								m_VecEquipSlotButton[nSlotIndex]->SetWearable( true );
							}
						}

						MoveToTail( m_pMagnetic );
						m_pMagnetic->Show( true );
					}
				}

				break;
		}
	}
	else
	{
		for( int i=0; i<(int)m_VecEquipSlotButton.size(); i++ )
			m_VecEquipSlotButton[i]->SetWearable( false );

		for( int i=0; i<(int)m_VecCashEquipSlotButton.size(); i++ )
			m_VecCashEquipSlotButton[i]->SetWearable( false );

#ifdef PRE_ADD_EQUIPLOCK
		MoveToHead(m_pEquipLockMagnetic);
		m_pEquipLockMagnetic->Show(false);
		EnableRadioButton(true);

		m_bShowEquipLockMagnetic = false;
#endif

		MoveToHead( m_pMagnetic );
		m_pMagnetic->Show( false );
	}
}

void CDnCharStatusDlg::Show( bool bShow )
{
	if( m_bShow == bShow )
		return;

	if( !bShow )
	{
		if( drag::IsValid() )
		{
			CDnSlotButton *pDragButton = static_cast<CDnSlotButton*>(drag::GetControl());
			if( pDragButton->GetSlotType() == ST_CHARSTATUS )
			{
				drag::ReleaseControl();
			}
		}
		ShowChildDialog( m_pAppellationDlg, false );

		// pvp정보창의 경우 한번 열어두고 char창을 켰다컸다하면 refresh가 안되어서 이상하게 보인다.
		// 기본적으로 부모가 Show되었다고 해서 자식 역시 다시 Show하는 방식이 아니기때문에,
		// 필요할 경우엔 이렇게 별도로 처리해야한다.
		if( m_iPage == PVP_PAGE )
			ShowChildDialog( m_pPVPInfoDlg, false );
		SwapEquipButtons( false, true );

#ifdef PRE_ADD_EQUIPLOCK
		m_bShowEquipLockMagnetic = false;
		m_pEquipLockMagnetic->Show(false);
#endif
	}
	else
	{
		switch( m_FocusEquipPage ) {
			case EquipPageNormal:
			case EquipPageCash:
				if( m_iPage == PVP_PAGE )
					ShowChildDialog( m_pPVPInfoDlg, true );
				break;
		}
		RefreshHideHelmetCheckBox();

		m_SmartMoveEx.MoveCursor();
	}

	CEtUIDialog::Show( bShow );
//	CDnMouseCursor::GetInstance().ShowCursor( bShow );

	if( m_pCallback )
	{
		m_pCallback->OnUICallbackProc( GetDialogID(), bShow?EVENT_DIALOG_SHOW:EVENT_DIALOG_HIDE, NULL );
	}
}

bool CDnCharStatusDlg::SetEquipItemFromInven( int nInvenIndex, MIInventoryItem *pItem, CDnSlotButton *pSlotButton )
{
	if( !pItem ) return false;

	CDnItem *pItemPtr = dynamic_cast<CDnItem *>(pItem);
	if( !pItemPtr ) return false;

	switch( pItemPtr->GetItemType() ) {
		case ITEMTYPE_WEAPON:
		case ITEMTYPE_PARTS:
			{
				if( pItemPtr->IsCashItem() )
				{
#ifdef PRE_ADD_EQUIPLOCK
					int equipIndex = GetCashEquipIndex(pItemPtr);
					MIInventoryItem* pEquipItem = GetCashEquipItem(equipIndex);
					if (pEquipItem != NULL)
					{
						CDnItem* pEquipDnItem = static_cast<CDnItem*>(pEquipItem);
						const CDnItemLockMgr& lockMgr = CDnItemTask::GetInstance().GetItemLockMgr();
						EquipItemLock::eLockStatus status = lockMgr.GetLockState(*pEquipDnItem);
						if (status != EquipItemLock::None)
						{
							m_strLastErrorMsg = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 8389);
							GetInterface().MessageBox(m_strLastErrorMsg.c_str(), MB_OK);
							return false;
						}
					}
#endif
					eCashEquipType equipType;
					if( GetWearableCashEquipType( pItem, equipType ) != eWEAR_ENABLE )
					{
						GetInterface().MessageBox( m_strLastErrorMsg.c_str(), MB_OK );
						return false;
					}

					return GetItemTask().RequestMoveItem( MoveType_CashInvenToCashEquip, nInvenIndex, pItemPtr->GetSerialID(), equipType, pItem->GetOverlapCount() );
				}
				else 
				{
#ifdef PRE_ADD_EQUIPLOCK
					int equipIndex = GetEquipIndex(pItemPtr);
					MIInventoryItem* pEquipItem = GetEquipItem(equipIndex);
					if (pEquipItem != NULL)
					{
						CDnItem* pEquipDnItem = static_cast<CDnItem*>(pEquipItem);
						const CDnItemLockMgr& lockMgr = CDnItemTask::GetInstance().GetItemLockMgr();
						EquipItemLock::eLockStatus status = lockMgr.GetLockState(*pEquipDnItem);
						if (status != EquipItemLock::None)
						{
							m_strLastErrorMsg = GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 8389);
							GetInterface().MessageBox(m_strLastErrorMsg.c_str(), MB_OK);
							return false;
						}
					}
#endif
					eEquipType equipType;
					eRetWearable ret = GetWearableEquipType( pItem, equipType );
					if(ret != eWEAR_ENABLE)
					{
						if (ret == eWEAR_UNABLE)
							GetInterface().MessageBox( m_strLastErrorMsg.c_str(), MB_OK );
						else if (ret == eWEAR_NEED_UNSEAL)
							GetInterface().OpenItemUnsealDialog(pItem, pSlotButton);
						return false;
					}

					return GetItemTask().RequestMoveItem( MoveType_InvenToEquip, nInvenIndex, pItemPtr->GetSerialID(), equipType, pItem->GetOverlapCount() );
				}
			}
			break;

#if defined(PRE_ADD_TALISMAN_SYSTEM)
		case ITEMTYPE_TALISMAN:
			{
				return m_pTalismanDlg->RequestEquipTalismanItem( nInvenIndex, pItem );
			}
			break;
#endif // PRE_ADD_TALISMAN_SYSTEM
	
		case ITEMTYPE_GLYPH:
			{
				std::wstring wszLastErrorMsg;
				eGlyph nEquipIndexP;
				eRetWearable ret = m_pPlateDlg->GetWearableGlyphEquipType( pItemPtr, (eGlyph &)nEquipIndexP, &wszLastErrorMsg );

				if(ret != eWEAR_ENABLE)
				{
					if (ret == eWEAR_UNABLE)
						GetInterface().MessageBox( wszLastErrorMsg.c_str(), MB_OK );
					else if (ret == eWEAR_NEED_UNSEAL)
						GetInterface().OpenItemUnsealDialog( pItemPtr, pSlotButton );
					return false;
				}

				if( (GLYPH_CASH1 <= nEquipIndexP && GLYPH_CASH3 >= nEquipIndexP) && GetItemTask().IsCashGlyphOpen(nEquipIndexP - GLYPH_CASH1) )
				{
					tm date;
					__time64_t tTime = GetItemTask().GetCashGlyphPeriod(nEquipIndexP - GLYPH_CASH1);
					DnLocalTime_s( &date, &tTime );

					std::wstring wszTimeString;
					CommonUtil::GetDateString( CommonUtil::DATESTR_FULL, wszTimeString, date );

					WCHAR wszOutputString[512];
					swprintf_s( wszOutputString, _countof(wszOutputString), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 9039 ), wszTimeString.c_str() );

					std::wstring wszString = std::wstring( wszOutputString );
					std::wstring::size_type nPosFirst = wszString.find( L"\\n" );
					std::wstring::size_type nPostSecond = wszString.find( L"\\n", nPosFirst+2 );

					std::wstring wszFirst = wszString.substr( 0, nPosFirst );
					std::wstring wszSecond = wszString.substr( nPosFirst+2, nPostSecond - nPosFirst - 2 );
					std::wstring wszThird = wszString.substr( nPostSecond+2 );

					swprintf_s( wszOutputString, _countof(wszOutputString), L"%s\n%s\n%s", wszFirst.c_str(), wszSecond.c_str(), wszThird.c_str() );

					GetInterface().BigMessageBox( wszOutputString, MB_YESNO, CDnCharStatusDlg::GLYPH_ATTACH_DIALOG, this );
				}
				else
				{
					std::wstring wszString = wstring( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 9038 ) );
					std::wstring::size_type nPosInStr = wszString.find( L"\\n" );

					WCHAR pString[256];
					std::wstring wszFirst = wszString.substr(0, nPosInStr);
					std::wstring wszSecond = wszString.substr(nPosInStr+2);

					swprintf_s( pString, _countof(pString), L"%s\n%s", wszFirst.c_str(), wszSecond.c_str() );

					GetInterface().MessageBox(pString, MB_YESNO, CDnCharStatusDlg::GLYPH_ATTACH_DIALOG, this );
				}

				m_pTempSlotItem = pItem;
				m_pTempSlotButton = pSlotButton;
				m_nTempSlotIndex = nInvenIndex;
			}
			break;

		case ITEMTYPE_PET:
			{
				if( pItemPtr->IsCashItem() ) 
				{
					Pet::Slot::ePetSlot equipType;
					equipType = Pet::Slot::Body;

					if( CDnActor::s_hLocalActor )
					{
						CDnPlayerActor *pPlayer = dynamic_cast<CDnPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
						if( pPlayer )
						{
							if(pPlayer->GetLevel() < pItemPtr->GetLevelLimit())
							{
								GetInterface().MessageBox(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 5070 ));	// 아이템 랩제 !
								break;
							}
						}
					}

					return GetItemTask().RequestMoveItem( MoveType_PetInvenToPetBody, nInvenIndex, pItemPtr->GetSerialID(), equipType, pItem->GetOverlapCount() );	
				}
			}
			break;
		case ITEMTYPE_PETPARTS:
			{
				if( pItemPtr->IsCashItem() ) 
				{
					DNTableFileFormat* pVehiclePartsTable = GetDNTable( CDnTableDB::TVEHICLEPARTS );

					if( !pVehiclePartsTable || !pVehiclePartsTable->IsExistItem(pItemPtr->GetClassID())) 
						return false;

					int equipType = -1;
					equipType = pVehiclePartsTable->GetFieldFromLablePtr( pItemPtr->GetClassID(), "_VehiclePartsType" )->GetInteger();
					if( equipType == 0 )
						equipType = Pet::Slot::Accessory1;
					else if( equipType == 1 )
						equipType = Pet::Slot::Accessory2;

					int VehicleClassType = -1;
					VehicleClassType = pVehiclePartsTable->GetFieldFromLablePtr( pItemPtr->GetClassID(), "_VehicleClassID" )->GetInteger();

					if( !CDnActor::s_hLocalActor ) break;
					CDnLocalPlayerActor *pActor = ((CDnLocalPlayerActor *)CDnActor::s_hLocalActor.GetPointer());

					if( !pActor->IsSummonPet() ) 
					{
						CDnInterface::GetInstance().ShowCaptionDialog( CDnInterface::typeCaption3, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 9235 ), textcolor::YELLOW, 4.0f );
						break;	// 펫을 소환하지 않은 상태에서는 사용할 수 없습니다.
					}

					if( pActor->GetPetClassType() != VehicleClassType ||  VehicleClassType == -1 )
					{
						CDnInterface::GetInstance().ShowCaptionDialog( CDnInterface::typeCaption3, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 9236 ), textcolor::YELLOW, 4.0f );
						break; // 펫의 타입과 맞지않아 착용 할 수 없습니다.
					}

					return GetItemTask().RequestMoveItem( MoveType_CashInvenToPetParts, nInvenIndex, pItemPtr->GetSerialID(), equipType, pItem->GetOverlapCount() );
				}
			}
			break;
	
		case ITEMTYPE_VEHICLEEFFECT:
			{
				if( pItemPtr->IsCashItem() ) {

					if(CDnActor::s_hLocalActor)
					{
						CDnPlayerActor *pPlayer = dynamic_cast<CDnPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
						if(pPlayer)
						{
							if(!pPlayer->IsCanVehicleMode())
							{
								CDnInterface::GetInstance().ShowCaptionDialog( CDnInterface::typeCaption3, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 9214 ), textcolor::YELLOW, 4.0f );
								break;
							}
							else if(pPlayer->GetLevel() < pItemPtr->GetLevelLimit())
							{
								GetInterface().MessageBox(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 5070 ));	// 아이템 랩제 !
								break;
							}
						}
					}
					
					return GetItemTask().RequestMoveItem( MoveType_CashInvenToCashEquip , nInvenIndex, pItemPtr->GetSerialID(), CASHEQUIP_EFFECT , pItem->GetOverlapCount() );
				}
			}
			break;

		case ITEMTYPE_VEHICLE_SHARE:
		case ITEMTYPE_VEHICLE:
			{
				if( pItemPtr->IsCashItem() ) {
							
					Vehicle::Slot::eVehicleSlot equipType;
					equipType = Vehicle::Slot::Body;

					if(CDnActor::s_hLocalActor)
					{
						CDnPlayerActor *pPlayer = dynamic_cast<CDnPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
						if(pPlayer)
						{
							if(!pPlayer->IsCanVehicleMode())
							{
								CDnInterface::GetInstance().ShowCaptionDialog( CDnInterface::typeCaption3, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 9214 ), textcolor::YELLOW, 4.0f );
								break;
							}
							else if(pPlayer->IsBattleMode())
							{
								CDnInterface::GetInstance().ShowCaptionDialog( CDnInterface::typeCaption3, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 9208 ), textcolor::YELLOW, 4.0f );
								break;
							}
							else if(pPlayer->GetLevel() < pItemPtr->GetLevelLimit())
							{
								GetInterface().MessageBox(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 5070 ));	// 아이템 랩제 !
								break;
							}
							else if(pPlayer->IsCallingVehicle())
							{
								break;
							}
						}
					}

					return GetItemTask().RequestMoveItem( MoveType_VehicleInvenToVehicleBody , nInvenIndex, pItemPtr->GetSerialID(), equipType , pItem->GetOverlapCount() );	
				}
			}
			break;
		case ITEMTYPE_VEHICLEPARTS:
			{
				if( pItemPtr->IsCashItem() ) 
				{
					DNTableFileFormat* pVehiclePartsTable = GetDNTable( CDnTableDB::TVEHICLEPARTS );
				
					if( !pVehiclePartsTable || !pVehiclePartsTable->IsExistItem(pItemPtr->GetClassID())) 
						return false;

					int equipType = -1;
					equipType = pVehiclePartsTable->GetFieldFromLablePtr( pItemPtr->GetClassID(), "_VehiclePartsType" )->GetInteger();

					int VehicleClassType = -1;
					VehicleClassType = pVehiclePartsTable->GetFieldFromLablePtr( pItemPtr->GetClassID(), "_VehicleClassID" )->GetInteger();
					
					if((equipType == Vehicle::Parts::Default) || (equipType == Vehicle::Parts::Hair)) // 현재 탈것은 안장만 착용가능.
						break;
					else if(equipType == Vehicle::Parts::Saddle)
						equipType = Vehicle::Slot::Saddle;


					if( !CDnActor::s_hLocalActor ) break;
					CDnLocalPlayerActor *pActor = ((CDnLocalPlayerActor *)CDnActor::s_hLocalActor.GetPointer());

					if( !pActor->IsCanVehicleMode())
					{
						CDnInterface::GetInstance().ShowCaptionDialog( CDnInterface::typeCaption3, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 9214 ), textcolor::YELLOW, 4.0f );
						break; // 여기선 탈수 없습니다.
					}
					if( !pActor->IsVehicleMode()) 
					{
						CDnInterface::GetInstance().ShowCaptionDialog( CDnInterface::typeCaption3, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 9209 ), textcolor::YELLOW, 4.0f );
						break;// 타고있을때만 장착이 됩니다.
					}

					if( (pActor->GetVehicleClassType() != VehicleClassType) ||  VehicleClassType == -1)
					{
						CDnInterface::GetInstance().ShowCaptionDialog( CDnInterface::typeCaption3, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 9212 ), textcolor::YELLOW, 4.0f );
						break; // 해당 탈것은 그 아이템을 장착 할 수 없습니다.
					}

					return GetItemTask().RequestMoveItem( MoveType_CashInvenToVehicleParts , nInvenIndex, pItemPtr->GetSerialID(), equipType , pItem->GetOverlapCount() );
				}
			}
			break;
	}
	
	return true;
}

void CDnCharStatusDlg::SetEquipItem( int nEquipIndex, MIInventoryItem *pItem )
{
	int nSlotIndex = EQUIPINDEX_2_SLOTINDEX(nEquipIndex);
	if( nSlotIndex == -1 ) return;

	m_VecEquipSlotButton[nSlotIndex]->SetItem(pItem, CDnSlotButton::ITEM_ORIGINAL_COUNT);
	m_VecEquipSlotButton[nSlotIndex]->OnRefreshTooltip();
}

void CDnCharStatusDlg::SetSourceItem( MIInventoryItem* pItem )
{
	if( pItem && m_pSourceItemSlot )
	{
		m_pSourceItemSlot->SetItem( pItem, CDnSlotButton::ITEM_ORIGINAL_COUNT );
	}
}

MIInventoryItem *CDnCharStatusDlg::GetEquipItem( int nEquipIndex )
{
	int nSlotIndex = EQUIPINDEX_2_SLOTINDEX(nEquipIndex);
	if( nSlotIndex == -1 ) return NULL;

	return m_VecEquipSlotButton[nSlotIndex]->GetItem();
}

MIInventoryItem *CDnCharStatusDlg::GetEquipGlyph( int nTypeParam )
{
	return m_pPlateDlg->GetEquipGlyph( nTypeParam );
}

void CDnCharStatusDlg::RefreshEquip()
{
	if( !CDnItemTask::IsActive() ) return;

	for( DWORD i=0; i<EQUIPMAX; i++ )
	{
		SetEquipItem( i, GetItemTask().GetEquipItem(i) );
	}

	CDnItem* pItem = GetItemTask().GetSourceItem();
	if( m_pSourceItemSlot )
		m_pSourceItemSlot->SetItem( pItem, CDnSlotButton::ITEM_ORIGINAL_COUNT );
}

void CDnCharStatusDlg::ResetEquipSlot( int nEquipIndex )
{
	int nSlotIndex = EQUIPINDEX_2_SLOTINDEX(nEquipIndex);
	if( nSlotIndex == -1 ) return;

	m_VecEquipSlotButton[nSlotIndex]->ResetSlot();
	m_VecEquipSlotButton[nSlotIndex]->OnRefreshTooltip();
}

void CDnCharStatusDlg::ResetCashEquipSlot( int nEquipIndex )
{
	int nSlotIndex = CASH_EQUIPINDEX_2_SLOTINDEX(nEquipIndex);
	if( nSlotIndex == -1 ) return;

	m_VecCashEquipSlotButton[nSlotIndex]->ResetSlot();
	m_VecCashEquipSlotButton[nSlotIndex]->OnRefreshTooltip();
	RefreshOnepieceBlock();
}

MIInventoryItem *CDnCharStatusDlg::GetCashEquipItem( int nEquipIndex )
{
	int nSlotIndex = CASH_EQUIPINDEX_2_SLOTINDEX(nEquipIndex);
	if( nSlotIndex == -1 ) return NULL;

	return m_VecCashEquipSlotButton[nSlotIndex]->GetItem();
}

void CDnCharStatusDlg::SetCashEquipItem( int nEquipIndex, MIInventoryItem *pItem )
{
	int nSlotIndex = CASH_EQUIPINDEX_2_SLOTINDEX(nEquipIndex);
	if( nSlotIndex == -1 ) return;

	m_VecCashEquipSlotButton[nSlotIndex]->SetItem(pItem, CDnSlotButton::ITEM_ORIGINAL_COUNT);
	m_VecCashEquipSlotButton[nSlotIndex]->OnRefreshTooltip();
	RefreshOnepieceBlock();
}

void CDnCharStatusDlg::RefreshCashEquip()
{
	if( !CDnItemTask::IsActive() ) return;

	for( DWORD i=0; i<CASHEQUIPMAX; i++ )
	{

		if(i==CASHEQUIP_EFFECT)
		{
			SetVehicleEquipItem(CDnCharVehicleDlg::VehicleEffectSlot,GetItemTask().GetCashEquipItem(i));
			continue;
		}

		SetCashEquipItem( i, GetItemTask().GetCashEquipItem(i) );
	}

	RefreshOnepieceBlock();
}


void CDnCharStatusDlg::SetSlotEvent()
{ 
	CDnSlotButton *pDragButton;
	pDragButton = ( CDnSlotButton * )drag::GetControl();

	if( pDragButton )
	{
		pDragButton->DisableSplitMode(false);
		drag::ReleaseControl();
	}
}

int CDnCharStatusDlg::EQUIPINDEX_2_SLOTINDEX( int nEquipIndex )
{
	switch( nEquipIndex )
	{
	case EQUIP_FACE:
	case EQUIP_HAIR:		return -1;
	case EQUIP_HELMET:		return 0;
	case EQUIP_BODY:		return 1;
	case EQUIP_LEG:			return 2;
	case EQUIP_HAND:		return 3;
	case EQUIP_FOOT:		return 4;
	case EQUIP_WEAPON1:		return 5;
	case EQUIP_WEAPON2:		return 6;
	case EQUIP_NECKLACE:	return 7;
	case EQUIP_EARRING:		return 8;
	case EQUIP_RING1:		return 9;
	case EQUIP_RING2:		return 10;
		/*
	case EQUIP_CREST_ENCHANT1: return 11;
	case EQUIP_CREST_ENCHANT2: return 12;
	case EQUIP_CREST_ENCHANT3: return 13;
	case EQUIP_CREST_ENCHANT4: return 14;
	case EQUIP_CREST_ENCHANT5: return 15;
	case EQUIP_CREST_ENCHANT6: return 16;
	case EQUIP_CREST_ENCHANT7: return 17;
	case EQUIP_CREST_ENCHANT8: return 18;
	case EQUIP_CREST_SKILL1: return 19;
	case EQUIP_CREST_SKILL2: return 20;
	case EQUIP_CREST_SKILL3: return 21;
	case EQUIP_CREST_SKILL4: return 22;
	case EQUIP_CREST_SPECIALSKILL: return 23;
	case EQUIP_CREST_CASH1: return 24;
	case EQUIP_CREST_CASH2: return 25;
	case EQUIP_CREST_CASH3: return 26;
	//case EQUIP_CREST:		return 11;
		*/
	}

	return -1;
}

int CDnCharStatusDlg::SLOTINDEX_2_EQUIPINDEX( int nSlotIndex )
{
	switch( nSlotIndex )
	{
	case 0:		return EQUIP_HELMET;
	case 1:		return EQUIP_BODY;
	case 2:		return EQUIP_LEG;
	case 3:		return EQUIP_HAND;
	case 4:		return EQUIP_FOOT;
	case 5:		return EQUIP_WEAPON1;
	case 6:		return EQUIP_WEAPON2;
	case 7:		return EQUIP_NECKLACE;
	case 8:		return EQUIP_EARRING;
	case 9:		return EQUIP_RING1;
	case 10:	return EQUIP_RING2;
		/*
	case 11:	return EQUIP_CREST_ENCHANT1;
	case 12:	return EQUIP_CREST_ENCHANT2;
	case 13:	return EQUIP_CREST_ENCHANT3;
	case 14:	return EQUIP_CREST_ENCHANT4;
	case 15:	return EQUIP_CREST_ENCHANT5;
	case 16:	return EQUIP_CREST_ENCHANT6;
	case 17:	return EQUIP_CREST_ENCHANT7;
	case 18:	return EQUIP_CREST_ENCHANT8;
	case 19:	return EQUIP_CREST_SKILL1;
	case 20:	return EQUIP_CREST_SKILL2;
	case 21:	return EQUIP_CREST_SKILL3;
	case 22:	return EQUIP_CREST_SKILL4;
	case 23:	return EQUIP_CREST_SPECIALSKILL;
	case 24:	return EQUIP_CREST_CASH1;
	case 25:	return EQUIP_CREST_CASH2;
	case 26:	return EQUIP_CREST_CASH3;
		*/

	default:
		CDebugSet::ToLogFile( "CDnCharStatusDlg::SLOTINDEX_2_EQUIPINDEX, default case(%d)", nSlotIndex );
		//ASSERT(0&&"CDnCharStatusDlg::SLOTINDEX_2_EQUIPINDEX");
		break;
	}

	return -1;
}

int CDnCharStatusDlg::PARTSTYPE_2_EQUIPINDEX( CDnParts::PartsTypeEnum partsType )
{
	switch( partsType )
	{
	case CDnParts::Face:		return EQUIP_FACE;
	case CDnParts::Hair:		return EQUIP_HAIR;
	case CDnParts::Helmet:		return EQUIP_HELMET;
	case CDnParts::Body:		return EQUIP_BODY;
	case CDnParts::Leg:			return EQUIP_LEG;
	case CDnParts::Hand:		return EQUIP_HAND;
	case CDnParts::Foot:		return EQUIP_FOOT;
	case CDnParts::Necklace:	return EQUIP_NECKLACE;
	case CDnParts::Earring:		return EQUIP_EARRING;
	case CDnParts::Ring:		return EQUIP_RING1;
	case CDnParts::Ring2:		return EQUIP_RING2;
	//case CDnParts::Crest:		return EQUIP_CREST;
	default:
		CDebugSet::ToLogFile( "CDnCharStatusDlg::PARTSTYPE_2_EQUIPINDEX, default case(%d)", partsType );
		//ASSERT(0&&"CDnCharStatusDlg::PARTSTYPE_2_EQUIPINDEX");
		break;
	}

	return -1;
}

int CDnCharStatusDlg::EQUIPTYPE_2_EQUIPINDEX( CDnWeapon::EquipTypeEnum equipType )
{
	switch( equipType )
	{
	case CDnWeapon::Sword:
	case CDnWeapon::Axe:
	case CDnWeapon::Hammer:
	case CDnWeapon::SmallBow:
	case CDnWeapon::BigBow:
	case CDnWeapon::CrossBow:
	case CDnWeapon::Staff:
	case CDnWeapon::Mace:
	case CDnWeapon::Flail:
	case CDnWeapon::Wand:		
	case CDnWeapon::Cannon:
	case CDnWeapon::BubbleGun:	
	case CDnWeapon::Chakram:
	case CDnWeapon::Fan:
	case CDnWeapon::Scimiter:
	case CDnWeapon::Dagger:
	case CDnWeapon::Spear:
	case CDnWeapon::KnuckleGear:
		return EQUIP_WEAPON1;
	case CDnWeapon::Shield:
	case CDnWeapon::Arrow:
	case CDnWeapon::Book:
	case CDnWeapon::Orb:
	case CDnWeapon::Puppet:
	case CDnWeapon::Gauntlet:	
	case CDnWeapon::Glove:
	case CDnWeapon::Charm:
	case CDnWeapon::Crook:
	case CDnWeapon::Bracelet:
	case CDnWeapon::Claw:
		return EQUIP_WEAPON2;
	default:
		CDebugSet::ToLogFile( "CDnCharStatusDlg::EQUIPTYPE_2_EQUIPINDEX, default case(%d)", equipType );
		//ASSERT(0&&"CDnCharStatusDlg::EQUIPTYPE_2_EQUIPINDEX");
		break;
	}

	return -1;
}

int CDnCharStatusDlg::CASH_EQUIPINDEX_2_SLOTINDEX( int nEquipIndex )
{
	switch( nEquipIndex )
	{
	case CASHEQUIP_HELMET:		return 0;
	case CASHEQUIP_BODY:		return 1;
	case CASHEQUIP_LEG:			return 2;
	case CASHEQUIP_HAND:		return 3;
	case CASHEQUIP_FOOT:		return 4;
	case CASHEQUIP_NECKLACE:	return 11;
	case CASHEQUIP_EARRING:		return 10;
	case CASHEQUIP_RING1:		return 9;
	case CASHEQUIP_RING2:		return 8;
	case CASHEQUIP_WEAPON1:		return 5;
	case CASHEQUIP_WEAPON2:		return 6;
	case CASHEQUIP_WING:		return 12;
	case CASHEQUIP_TAIL:		return 13;
	case CASHEQUIP_FACEDECO:	return 14;
	case CASHEQUIP_FAIRY:		return 7;
	}

	return -1;
}

int CDnCharStatusDlg::CASH_SLOTINDEX_2_EQUIPINDEX( int nSlotIndex )
{
	switch( nSlotIndex )
	{
	case 0:		return CASHEQUIP_HELMET;
	case 1:		return CASHEQUIP_BODY;
	case 2:		return CASHEQUIP_LEG;
	case 3:		return CASHEQUIP_HAND;
	case 4:		return CASHEQUIP_FOOT;
	case 11:	return CASHEQUIP_NECKLACE;
	case 10:	return CASHEQUIP_EARRING;
	case 9:		return CASHEQUIP_RING1;
	case 8:		return CASHEQUIP_RING2;
	case 5:		return CASHEQUIP_WEAPON1;
	case 6:		return CASHEQUIP_WEAPON2;
	case 12:	return CASHEQUIP_WING;
	case 13:	return CASHEQUIP_TAIL;
	case 14:	return CASHEQUIP_FACEDECO;
	case 7:	return CASHEQUIP_FAIRY;
	default:
		CDebugSet::ToLogFile( "CDnCharStatusDlg::SLOTINDEX_2_EQUIPINDEX, default case(%d)", nSlotIndex );
		break;
	}

	return -1;
}

int CDnCharStatusDlg::CASH_PARTSTYPE_2_EQUIPINDEX( CDnParts::PartsTypeEnum partsType )
{
	switch( partsType )
	{
	case CDnParts::CashHelmet:		return CASHEQUIP_HELMET;
	case CDnParts::CashBody:		return CASHEQUIP_BODY;
	case CDnParts::CashLeg:			return CASHEQUIP_LEG;
	case CDnParts::CashHand:		return CASHEQUIP_HAND;
	case CDnParts::CashFoot:		return CASHEQUIP_FOOT;
	case CDnParts::CashNecklace:	return CASHEQUIP_NECKLACE;
	case CDnParts::CashEarring:		return CASHEQUIP_EARRING;
	case CDnParts::CashRing:		return CASHEQUIP_RING1;
	case CDnParts::CashRing2:		return CASHEQUIP_RING2;
	case CDnParts::CashWing:		return CASHEQUIP_WING;
	case CDnParts::CashTail:		return CASHEQUIP_TAIL;
	case CDnParts::CashFaceDeco:	return CASHEQUIP_FACEDECO;
	case CDnParts::CashFairy:		return CASHEQUIP_FAIRY;

	default:
		CDebugSet::ToLogFile( "CDnCharStatusDlg::PARTSTYPE_2_EQUIPINDEX, default case(%d)", partsType );
		break;
	}

	return -1;
}

int CDnCharStatusDlg::CASH_EQUIPTYPE_2_EQUIPINDEX( CDnWeapon::EquipTypeEnum equipType )
{
	switch( equipType )
	{
	case CDnWeapon::Sword:
	case CDnWeapon::Axe:
	case CDnWeapon::Hammer:
	case CDnWeapon::SmallBow:
	case CDnWeapon::BigBow:
	case CDnWeapon::CrossBow:
	case CDnWeapon::Staff:
	case CDnWeapon::Mace:
	case CDnWeapon::Flail:
	case CDnWeapon::Wand:		
	case CDnWeapon::Cannon:
	case CDnWeapon::BubbleGun:	
	case CDnWeapon::Chakram:
	case CDnWeapon::Fan:
	case CDnWeapon::Scimiter:
	case CDnWeapon::Dagger:
	case CDnWeapon::Spear:
	case CDnWeapon::KnuckleGear:
		return CASHEQUIP_WEAPON1;
	case CDnWeapon::Shield:
	case CDnWeapon::Arrow:
	case CDnWeapon::Book:
	case CDnWeapon::Orb:
	case CDnWeapon::Puppet:
	case CDnWeapon::Gauntlet:	
	case CDnWeapon::Glove:		
	case CDnWeapon::Charm:
	case CDnWeapon::Crook:
	case CDnWeapon::Bracelet:
	case CDnWeapon::Claw:
		return CASHEQUIP_WEAPON2;
	default:
		CDebugSet::ToLogFile( "CDnCharStatusDlg::EQUIPTYPE_2_EQUIPINDEX, default case(%d)", equipType );
		break;
	}

	return -1;
}

#ifdef PRE_ADD_EQUIPLOCK
int CDnCharStatusDlg::GetEquipIndex( const CDnItem *pItem ) const
{
	if( !pItem ) 
		return -1;

	switch( pItem->GetItemType() )
	{
	case ITEMTYPE_WEAPON:
		{
			const CDnWeapon *pWeapon = static_cast<const CDnWeapon *>(pItem);
			return EQUIPTYPE_2_EQUIPINDEX( pWeapon->GetEquipType() );
		}
		break;
	case ITEMTYPE_PARTS:
		{
			const CDnParts *pParts = static_cast<const CDnParts *>(pItem);
			return PARTSTYPE_2_EQUIPINDEX( pParts->GetPartsType() );
		}
		break;
	case ITEMTYPE_GLYPH :
		{
			const CDnGlyph * pGlyph = static_cast<const CDnGlyph *>(pItem);
			if( pGlyph->GetGlyphType() != CDnGlyph::ActiveSkill )
				return pItem->GetTypeParam(1);
		}
		break;

#if defined(PRE_ADD_TALISMAN_SYSTEM)
	case ITEMTYPE_TALISMAN:
		{
			// 슬롯에서 동일한 아이템ID를 가진  탈리스만이 있는지 찾는다.
			return m_pTalismanDlg->GetWearedSlotIndex(pItem);
		}
		break;
#endif
	}

	return -1;
}
#else
int CDnCharStatusDlg::GetEquipIndex( CDnItem *pItem )
{
	if( !pItem ) 
		return -1;

	switch( pItem->GetItemType() )
	{
	case ITEMTYPE_WEAPON:
		{
			CDnWeapon *pWeapon = (CDnWeapon *)pItem;
			return EQUIPTYPE_2_EQUIPINDEX( pWeapon->GetEquipType() );
		}
		break;
	case ITEMTYPE_PARTS:
		{
			CDnParts *pParts = (CDnParts *)pItem;
			return PARTSTYPE_2_EQUIPINDEX( pParts->GetPartsType() );
		}
		break;
	case ITEMTYPE_GLYPH :
		{
			CDnGlyph * pGlyph = (CDnGlyph *)pItem;
			if( pGlyph->GetGlyphType() != CDnGlyph::ActiveSkill )
				return pItem->GetTypeParam(1);
		}
		break;

#if defined(PRE_ADD_TALISMAN_SYSTEM)
	case ITEMTYPE_TALISMAN:
		{
			// 슬롯에서 동일한 아이템ID를 가진  탈리스만이 있는지 찾는다.
			return m_pTalismanDlg->GetWearedSlotIndex(pItem);
		}
		break;
#endif
	}

	return -1;
}
#endif

#ifdef PRE_ADD_TALISMAN_SYSTEM
CDnItem* CDnCharStatusDlg::GetEquipTalisman(int nIndex)
{
	return m_pTalismanDlg->GetEquipTalisman(nIndex);
}
#endif // PRE_ADD_TALISMAN_SYSTEM

#ifdef PRE_ADD_EQUIPLOCK
int CDnCharStatusDlg::GetCashEquipIndex( const CDnItem *pItem ) const
{
	if( !pItem ) 
		return -1;

	switch( pItem->GetItemType() )
	{
	case ITEMTYPE_WEAPON:
		{
			const CDnWeapon *pWeapon = static_cast<const CDnWeapon *>(pItem);
			return CASH_EQUIPTYPE_2_EQUIPINDEX( pWeapon->GetEquipType() );
		}
		break;
	case ITEMTYPE_PARTS:
		{
			const CDnParts *pParts = static_cast<const CDnParts *>(pItem);
			return CASH_PARTSTYPE_2_EQUIPINDEX( pParts->GetPartsType() );
		}
		break;
	}

	return -1;
}
#else
int CDnCharStatusDlg::GetCashEquipIndex( CDnItem *pItem )
{
	if( !pItem ) 
		return -1;

	switch( pItem->GetItemType() )
	{
	case ITEMTYPE_WEAPON:
		{
			CDnWeapon *pWeapon = (CDnWeapon *)pItem;
			return CASH_EQUIPTYPE_2_EQUIPINDEX( pWeapon->GetEquipType() );
		}
		break;
	case ITEMTYPE_PARTS:
		{
			CDnParts *pParts = (CDnParts *)pItem;
			return CASH_PARTSTYPE_2_EQUIPINDEX( pParts->GetPartsType() );
		}
		break;
	}

	return -1;
}
#endif

bool CDnCharStatusDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	m_hWnd = hWnd;

	if( !m_bShow )
	{
		return false;
	}

	if( drag::IsValid() )
	{
		POINT MousePoint;
		MousePoint.x = short( LOWORD( lParam ) );
		MousePoint.y = short( HIWORD( lParam ) );

		float fMouseX, fMouseY;
		PointToFloat( MousePoint, fMouseX, fMouseY );

		switch( uMsg )
		{
		case WM_MOUSEMOVE:
			{
				if( m_pMagnetic->IsInside( fMouseX, fMouseY ) )
				{
					SetSlotMagneticMode( true );
				}
				else
				{
					SetSlotMagneticMode( false );
				}
			}
			break;
		case WM_LBUTTONDOWN:
			{
				if( m_pMagnetic->IsInside( fMouseX, fMouseY ) && m_pMagnetic->IsShow() )
				{
					CDnSlotButton *pDragButton = (CDnSlotButton*)drag::GetControl();

					pDragButton->DisableSplitMode(true);
					drag::ReleaseControl();

					CDnItem *pItem = dynamic_cast<CDnItem *>(pDragButton->GetItem());
					if( !pItem ) break;
					switch( pItem->GetItemType() ) {
						case ITEMTYPE_WEAPON:
						case ITEMTYPE_PARTS:
						case ITEMTYPE_GLYPH:
						case ITEMTYPE_VEHICLE:
						case ITEMTYPE_VEHICLE_SHARE:// Rotha
						case ITEMTYPE_VEHICLEPARTS:// Rotha 
						case ITEMTYPE_VEHICLEEFFECT:

							if( !SetEquipItemFromInven( pDragButton->GetSlotIndex(), pDragButton->GetItem(), pDragButton ) )
								return false;
							break;
					}

					return true;
				}
			}
			break;

#ifdef PRE_ADD_EQUIPLOCK
		case WM_LBUTTONUP:
			{
				CDnSlotButton *pDragButton = (CDnSlotButton*)drag::GetControl();

				if (m_pEquipLockMagnetic->IsInside(fMouseX, fMouseY) && m_pEquipLockMagnetic->IsShow())
				{
					if (pDragButton == NULL || pDragButton->GetSlotType() != ST_CHARSTATUS)
						break;

					pDragButton->DisableSplitMode(true);
					drag::ReleaseControl();

					if (pDragButton->GetItemType() != MIInventoryItem::Item)
						break;

					CDnItem* pItem = static_cast<CDnItem *>(pDragButton->GetItem());
					if (pItem == NULL)
						break;

					const CDnItemLockMgr& lockMgr = CDnItemTask::GetInstance().GetItemLockMgr();
					EquipItemLock::eLockStatus status = lockMgr.GetLockState(*pItem);
					if (status == EquipItemLock::Lock)
						break;

					switch (pItem->GetItemType())
					{
					case ITEMTYPE_WEAPON:
					case ITEMTYPE_PARTS:
					case ITEMTYPE_GLYPH:
					case ITEMTYPE_VEHICLE:
					case ITEMTYPE_VEHICLE_SHARE:     // Rotha
					case ITEMTYPE_VEHICLEPARTS:// Rotha 
					case ITEMTYPE_VEHICLEEFFECT:
						{
							m_pTempSlotItem = pItem;
							int nUnlockTerm = (int)CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::ItemUnLockWaitTime);
							std::wstring str = FormatW(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 8383), nUnlockTerm);
							GetInterface().BigMessageBox(str.c_str(), MB_YESNO, EQUIPLOCK_CONFIRM_DIALOG, this); // UISTRING : 선택한 아이템을 잠금 상태로 변환하시겠습니까?
						}
						break;
					}
				}

				if (pDragButton && pDragButton->GetSlotType() == ST_CHARSTATUS)
				{
					if (m_pEquipLockMagnetic == NULL || m_pEquipLockMagnetic->IsShow())
						break;

					m_bShowEquipLockMagnetic = true;
					break;
				}
			}
			break;
#endif
		}
	}
	else
	{
		SetSlotMagneticMode( false );
	}

	bool bRet;
	bRet = CEtUIDialog::MsgProc( hWnd, uMsg, wParam, lParam );

	if (uMsg == WM_LBUTTONDOWN)
	{
		POINT MousePoint;
		MousePoint.x = short( LOWORD( lParam ) );
		MousePoint.y = short( HIWORD( lParam ) );

		float fMouseX, fMouseY;
		PointToFloat( MousePoint, fMouseX, fMouseY );

		SUICoord base;
		GetDlgCoord(base);

		if (base.IsInside(fMouseX, fMouseY))
		{
			SetRenderPriority(this, true);
			SetRenderPriority((CEtUIDialog*)GetInterface().GetItemUnsealDlg(), true);
			SetRenderPriority((CEtUIDialog*)GetInterface().GetItemSealDlg(), true);
			SetRenderPriority((CEtUIDialog*)GetInterface().GetItemPotentialDlg(), true);
			SetRenderPriority((CEtUIDialog*)GetInterface().GetEnchantJewelDlg(), true);
#if defined(PRE_ADD_REMOVE_PREFIX)
			SetRenderPriority((CEtUIDialog*)GetInterface().GetRemovePrefixDlg(), true);
#endif // PRE_ADD_REMOVE_PREFIX
			return true;
		}
	}

	return bRet;
}

void CDnCharStatusDlg::Process( float fElapsedTime )
{
	m_SmartMoveEx.Process();
	CDnCustomDlg::Process( fElapsedTime );
}

CDnCharStatusDlg::eRetWearable CDnCharStatusDlg::GetWearableEquipType(MIInventoryItem *pItem, eEquipType &equipType)
{
	m_strLastErrorMsg.clear();

	if( !pItem )
	{
		m_strLastErrorMsg = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 5059 );
		return eWEAR_UNABLE;
	}

	if( pItem->GetType() != MIInventoryItem::Item )
	{
		m_strLastErrorMsg = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 5060 );
		return eWEAR_UNABLE;
	}

	CDnItem *pEquipItem = dynamic_cast<CDnItem*>(pItem);
	if( !pEquipItem )
	{
		m_strLastErrorMsg = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 5060 );
		return eWEAR_UNABLE;
	}

	if (pEquipItem->IsSoulbBound() == false)
		return eWEAR_NEED_UNSEAL;

	switch( pEquipItem->GetItemType() )
	{
	case ITEMTYPE_QUEST:
		m_strLastErrorMsg = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 5060 );
		return eWEAR_UNABLE;

	case ITEMTYPE_WEAPON:
		{
			CDnWeapon *pWeapon = dynamic_cast<CDnWeapon*>(pEquipItem);
			if( !pWeapon )
			{
				m_strLastErrorMsg = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 5061 );
				return eWEAR_UNABLE;
			}

			equipType = (eEquipType)EQUIPTYPE_2_EQUIPINDEX( pWeapon->GetEquipType() );
			if( GetItemTask().IsEquipItem( equipType, pEquipItem, m_strLastErrorMsg ) == false )
			{
				return eWEAR_UNABLE;
			}
		}
		return eWEAR_ENABLE;

	case ITEMTYPE_PARTS:
		{
			CDnParts *pParts = dynamic_cast<CDnParts*>(pEquipItem);
			if( !pParts )
			{
				m_strLastErrorMsg = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 5061 );
				return eWEAR_UNABLE;
			}

			equipType = (eEquipType)pParts->GetPartsType();

			if( (equipType == CDnParts::Ring) || (equipType == CDnParts::Ring2) )
			{
				int nSlotIndex = EQUIPINDEX_2_SLOTINDEX(EQUIP_RING1);
				if( nSlotIndex == -1 ) break;

				if( m_VecEquipSlotButton[nSlotIndex]->IsEmptySlot() )
				{
					pParts->SetPartsType( CDnParts::Ring );
					equipType = (eEquipType)pParts->GetPartsType();
				}
				else
				{
					nSlotIndex = EQUIPINDEX_2_SLOTINDEX(EQUIP_RING2);
					if( nSlotIndex == -1 ) break;

					if( m_VecEquipSlotButton[nSlotIndex]->IsEmptySlot() )
					{
						pParts->SetPartsType( CDnParts::Ring2 );
						equipType = (eEquipType)pParts->GetPartsType();
					}
				}
			}

			if( GetItemTask().IsEquipItem( equipType, pEquipItem, m_strLastErrorMsg ) == false )
			{
				return eWEAR_UNABLE;
			}
		}
		return eWEAR_ENABLE;

	case ITEMTYPE_GLYPH:
			/*
			CDnItem::GlyphType Type = (CDnItem::GlyphType)( dynamic_cast<CDnItem *>(pItem)->GetTypeParam(1) );
			switch( Type ) {
				case CDnItem::GlyphType::TemperedStatus:
					for( int i=EQUIP_CREST_ENCHANT1; i<=EQUIP_CREST_ENCHANT8; i++ ) {
					}
					break;
				case CDnItem::GlyphType::TemperedSkill:
				case CDnItem::GlyphType::AddActiveSkill:
					break;
			}
			if( GetItemTask().IsEquipItem( equipType, pEquipItem, m_strLastErrorMsg ) == false )
			{
				return eWEAR_UNABLE;
			}
			*/
		m_strLastErrorMsg = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 5056 );
		return eWEAR_UNABLE;
	}

	m_strLastErrorMsg = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 5060 );
	return eWEAR_UNABLE;
}

CDnCharStatusDlg::eRetWearable CDnCharStatusDlg::GetWearableCashEquipType( MIInventoryItem *pItem, eCashEquipType &equipType )
{
	m_strLastErrorMsg.clear();

	if( !pItem )
	{
		m_strLastErrorMsg = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 5059 );
		return eWEAR_UNABLE;
	}

	if( pItem->GetType() != MIInventoryItem::Item )
	{
		m_strLastErrorMsg = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 5060 );
		return eWEAR_UNABLE;
	}

	CDnItem *pEquipItem = dynamic_cast<CDnItem*>(pItem);
	if( !pEquipItem )
	{
		m_strLastErrorMsg = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 5060 );
		return eWEAR_UNABLE;
	}

	switch( pEquipItem->GetItemType() )
	{
	case ITEMTYPE_QUEST:
		m_strLastErrorMsg = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 5060 );
		return eWEAR_UNABLE;

	case ITEMTYPE_WEAPON:
		{
			CDnWeapon *pWeapon = dynamic_cast<CDnWeapon*>(pEquipItem);
			if( !pWeapon )
			{
				m_strLastErrorMsg = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 5061 );
				return eWEAR_UNABLE;
			}

			equipType = (eCashEquipType)CASH_EQUIPTYPE_2_EQUIPINDEX( pWeapon->GetEquipType() );
			if( GetItemTask().IsEquipItem( equipType, pEquipItem, m_strLastErrorMsg ) == false )
			{
				return eWEAR_UNABLE;
			}
		}
		return eWEAR_ENABLE;

	case ITEMTYPE_PARTS:
		{
			CDnParts *pParts = dynamic_cast<CDnParts*>(pEquipItem);
			if( !pParts )
			{
				m_strLastErrorMsg = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 5061 );
				return eWEAR_UNABLE;
			}

			equipType = (eCashEquipType)pParts->GetPartsType();

			if( (equipType == CDnParts::CashRing) || (equipType == CDnParts::CashRing2) )
			{
				int nSlotIndex = CASH_EQUIPINDEX_2_SLOTINDEX(CASHEQUIP_RING1);
				if( nSlotIndex == -1 ) break;

				if( m_VecCashEquipSlotButton[nSlotIndex]->IsEmptySlot() )
				{
					pParts->SetPartsType( CDnParts::CashRing );
					equipType = (eCashEquipType)pParts->GetPartsType();
				}
				else
				{
					nSlotIndex = CASH_EQUIPINDEX_2_SLOTINDEX(CASHEQUIP_RING2);
					if( nSlotIndex == -1 ) break;

					if( m_VecCashEquipSlotButton[nSlotIndex]->IsEmptySlot() )
					{
						pParts->SetPartsType( CDnParts::CashRing2 );
						equipType = (eCashEquipType)pParts->GetPartsType();
					}
				}
			}

			if( GetItemTask().IsEquipItem( equipType, pEquipItem, m_strLastErrorMsg ) == false )
			{
				return eWEAR_UNABLE;
			}
		}
		return eWEAR_ENABLE;
	}

	m_strLastErrorMsg = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 5060 );
	return eWEAR_UNABLE;
}

CDnCharStatusDlg::eRetWearable CDnCharStatusDlg::GetWearableGlyphEquipType( MIInventoryItem *pItem, eGlyph &equipType )
{
	return m_pPlateDlg->GetWearableGlyphEquipType( pItem, equipType, &m_strLastErrorMsg );
}


void CDnCharStatusDlg::SetSlotMagneticMode( bool bMagnetic )
{
	for( int i=0; i<(int)m_VecEquipSlotButton.size(); i++ )
	{
		m_VecEquipSlotButton[i]->SetMagnetic( bMagnetic );
	}

	for( int i=0; i<(int)m_VecCashEquipSlotButton.size(); i++ )
	{
		m_VecCashEquipSlotButton[i]->SetMagnetic( bMagnetic );
	}

	m_pPlateDlg->SetSlotMagneticMode( bMagnetic );
	m_pVehicleDlg->SetSlotMagneticMode( bMagnetic );

}

void CDnCharStatusDlg::SetGlyphEquipItem( int nEquipIndex, MIInventoryItem *pItem )
{
	m_pPlateDlg->SetGlyphEquipItem( nEquipIndex, pItem );
}

void CDnCharStatusDlg::RefreshGlyphEquip()
{
	m_pPlateDlg->RefreshGlyphEquip();
}

void CDnCharStatusDlg::RefreshGlyphCover()
{
	m_pPlateDlg->RefreshCover();
}

void CDnCharStatusDlg::ResetGlyphEquipSlot( int nEquipIndex )
{
	m_pPlateDlg->ResetGlyphEquipSlot( nEquipIndex );
}

void CDnCharStatusDlg::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED ) 
	{
		if (IsCmdControl("ID_YES"))
		{
			switch( nID )
			{
			case CDnCharStatusDlg::GLYPH_ATTACH_DIALOG :
				{
					m_pPlateDlg->EquipGlyphItem( m_nTempSlotIndex, m_pTempSlotItem, m_pTempSlotButton );
					break;
				}
				break;
#ifdef PRE_ADD_EQUIPLOCK
			case EQUIPLOCK_CONFIRM_DIALOG:
				{
					if (CDnItemTask::IsActive() == false || m_pTempSlotItem == NULL)
					{
						_ASSERT(0);
						GetInterface().MessageBox(L"잠금이 불가능한 상태입니다. 다시 시도해주세요.", MB_OK);
						break;
					}

					if (m_pTempSlotItem->GetType() != MIInventoryItem::Item)
						break;

					CDnItem* pItem = static_cast<CDnItem *>(m_pTempSlotItem);
					if (pItem == NULL)
						break;

					const CDnItemLockMgr& lockMgr = CDnItemTask::GetInstance().GetItemLockMgr();
					lockMgr.RequestLock(*pItem);
				}
				break;

			case EQUIPUNLOCK_CONFIRM_DIALOG:
				{
					if (CDnItemTask::IsActive() == false || m_pTempSlotItem == NULL)
					{
						_ASSERT(0);
						GetInterface().MessageBox(L"잠금해제가 불가능한 상태입니다. 다시 시도해주세요.", MB_OK);
						break;
					}

					if (m_pTempSlotItem->GetType() != MIInventoryItem::Item)
						break;

					CDnItem* pItem = static_cast<CDnItem *>(m_pTempSlotItem);
					if (pItem == NULL)
						break;

					const CDnItemLockMgr& lockMgr = CDnItemTask::GetInstance().GetItemLockMgr();
					lockMgr.RequestUnLock(*pItem);
				}
				break;
#endif
			}
		}
	}
}

void CDnCharStatusDlg::SetVehicleEquipItem( int nEquipIndex, MIInventoryItem *pItem )
{
	if(m_pVehicleDlg)
		m_pVehicleDlg->SetVehicleEquipItem( nEquipIndex, pItem );
}

void CDnCharStatusDlg::RefreshVehicleEquip()
{
	if(m_pVehicleDlg)
		m_pVehicleDlg->RefreshVehicleEquip();
}

void CDnCharStatusDlg::ResetVehicleEquipSlot( int nEquipIndex )
{
	if(m_pVehicleDlg)
		m_pVehicleDlg->ResetVehicleEquipSlot( nEquipIndex );
}


void CDnCharStatusDlg::RefreshVehiclePreview()
{
	if(m_pVehicleDlg)
		m_pVehicleDlg->RefreshVehicleEquipPreview();
}

void CDnCharStatusDlg::RefreshLadderInfo()
{
	if(m_pLadderInfoDlg)
		m_pLadderInfoDlg->RefreshLadderInfo();
}

void CDnCharStatusDlg::SetPetEquipItem( CDnItem* pItem )
{
	if( m_pPetDlg )
		m_pPetDlg->SetPetEquipItem( pItem );
}

void CDnCharStatusDlg::ResetPetEquipSlot( int nEquipIndex )
{
	if( m_pPetDlg )
		m_pPetDlg->ResetPetEquipSlot( nEquipIndex );
}

void CDnCharStatusDlg::RefreshPetPreview()
{
	if( m_pPetDlg )
		m_pPetDlg->RefreshPetEquipPreview();
}

void CDnCharStatusDlg::SetWithDarwOpen( bool bOpen )
{
	if( m_pPlateDlg )
		m_pPlateDlg->SetWithDrawOpen( bOpen );
}

bool CDnCharStatusDlg::GetWithDrawOpen()
{
	if( m_pPlateDlg )
		return m_pPlateDlg->GetWithDrawOpen();

	return false;
}

void CDnCharStatusDlg::SetRadioButton( CDnCharStatusDlg::eRadioOptType eType)
{
	switch( eType )
	{
	case CDnCharStatusDlg::eRadioOpt_None :
		{
			if( m_pNormalButton )
				m_pNormalButton->Enable( true );
			if( m_pCashButton )
				m_pCashButton->Enable( true );
			if( m_pPlateButton )
				m_pPlateButton->Enable( true );
			if( m_pVehicleButton )
				m_pVehicleButton->Enable( true );
			if( m_pPetButton )
				m_pPetButton->Enable( true );
			return;
		}
	case CDnCharStatusDlg::eRadioOpt_Glyph :
		{
			if( m_pNormalButton )
				m_pNormalButton->Enable( false );
			if( m_pCashButton )
				m_pCashButton->Enable( false );
			if( m_pPlateButton )
				m_pPlateButton->Enable( true );
			if( m_pVehicleButton )
				m_pVehicleButton->Enable( false );
			if( m_pPetButton )
				m_pPetButton->Enable( false );
			return;
		}
	default:
		return;
	}
}

#ifdef PRE_ADD_EQUIPLOCK
void CDnCharStatusDlg::EnableRadioButton(bool bEnable)
{
	if (m_pNormalButton)
		m_pNormalButton->Enable(bEnable);
	if (m_pCashButton)
		m_pCashButton->Enable(bEnable);
	if (m_pPlateButton)
		m_pPlateButton->Enable(bEnable);
	if (m_pVehicleButton)
		m_pVehicleButton->Enable(bEnable);
	if (m_pPetButton)
		m_pPetButton->Enable(bEnable);
}
#endif

void CDnCharStatusDlg::RefreshGuildInfo()
{
	if (m_pGuildWarInfoDlg)
		m_pGuildWarInfoDlg->SetGuildWarInfo();
}


// 장착중인가?
bool CDnCharStatusDlg::IsEquipped( CDnItem * pItem, bool bCash )
{
	int equipIdx = -1;
	int slotIdx = -1;

	// Cash Item.
	if( bCash )
	{
		equipIdx = GetCashEquipIndex( pItem );
		if( equipIdx == -1 ) // 캐시장비 아님.
			return false;

		slotIdx = CASH_EQUIPINDEX_2_SLOTINDEX( equipIdx );
		if( slotIdx == -1 )
			return false;

		if( m_VecCashEquipSlotButton[ slotIdx ]->GetItem() != pItem )
			return false;
	}

	// Common Item.
	else
	{
		equipIdx = GetEquipIndex( pItem );
		if( equipIdx == -1 ) // 캐시장비 아님.
			return false;

		slotIdx = EQUIPINDEX_2_SLOTINDEX( equipIdx );
		if( slotIdx == -1 )
			return false;

		if( m_VecEquipSlotButton[ slotIdx ]->GetItem() != pItem )
			return false;
	}

	return true;
}

void CDnCharStatusDlg::OnRefreshPlayerStatus()
{
	if( m_pBaseInfoDlg && m_pBaseInfoDlg->IsShow() )
		m_pBaseInfoDlg->OnRefreshPlayerStatus();

	if( m_pDetailInfoDlg && m_pDetailInfoDlg->IsShow() )
		m_pDetailInfoDlg->OnRefreshPlayerStatus();
}

#if defined(PRE_ADD_TALISMAN_SYSTEM)
void CDnCharStatusDlg::SetTalismanEquipItem( int nEquipIndex, MIInventoryItem *pItem )
{
	m_pTalismanDlg->SetTalismanItem(nEquipIndex, pItem);
}

void CDnCharStatusDlg::RemoveTalismanEquipItem(int nEquipIndex )
{
	m_pTalismanDlg->RemoveTalismanItem(nEquipIndex);
}

void CDnCharStatusDlg::SetOpenTalismanSlot(int nSlotOpenFlag)
{
	m_pTalismanDlg->SetOpenTalismanSlot(nSlotOpenFlag);
}

float CDnCharStatusDlg::GetTalismanSlotRatio(int nSlotIndex)
{
	return m_pTalismanDlg->GetTalismanSlotRatio(nSlotIndex);
}
#endif // PRE_ADD_TALISMAN_SYSTEM

#if defined( PRE_ADD_COSTUME_SKILL )
void CDnCharStatusDlg::RefreshCustumeSkill( DnSkillHandle hSkill )
{
	m_pCustumeSkillSlotButton->ResetSlot();
	if( hSkill )
	{
		m_pCustumeSkillSlotButton->SetQuickItem( hSkill.GetPointer() );
	}
}
#endif

#ifdef PRE_ADD_CASHINVENTAB
CDnItem * CDnCharStatusDlg::GetCashInvenItemBySN( INT64 SN )
{
	CDnInvenTabDlg *pInvenDlg = (CDnInvenTabDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::INVENTORY_DIALOG );
	if( pInvenDlg )
		return pInvenDlg->GetCashInvenItemBySN( SN );
	
	return NULL;
}
#endif // PRE_ADD_CASHINVENTAB