#include "Stdafx.h"
#include "DnLifeSlotDlg.h"
#include "DnItemTask.h"
#include "DnLifeSkillPlantTask.h"
#include "DnTableDB.h"
#include "DnLocalPlayerActor.h"
#include "DnSkillTask.h"
#include "SecondarySkill.h"


#define START_PAGE	0

CDnLifeSlotDlg::CDnLifeSlotDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback, true )
, m_pSeedButton( NULL )
, m_pPrevButton( NULL )
, m_pNextButton( NULL )
, m_pPageStatic( NULL )
, m_pNumberStatic( NULL )
, m_pEnableStatic( NULL )
, m_pDisableStatic( NULL )
, m_pCheckCash( NULL )
, m_nPage( START_PAGE )
, m_pCashItemSlot( NULL )
, m_nSelectSlotIndex( -1 )
, m_pSelectItem( NULL )
{
	m_pCashItem = CDnItem::CreateItem( 348334081, 0, true );
}

CDnLifeSlotDlg::~CDnLifeSlotDlg()
{
	SAFE_DELETE( m_pCashItem );
}

void CDnLifeSlotDlg::Initialize(bool bShow)
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "LifeSlotDlg.ui" ).c_str(), bShow );
}

void CDnLifeSlotDlg::InitialUpdate()
{
	m_pSeedButton = GetControl<CEtUIButton>( "ID_BT_OK" );
	m_pPrevButton = GetControl<CEtUIButton>( "ID_BT_PRIOR" );
	m_pNextButton = GetControl<CEtUIButton>( "ID_BT_NEXT" );
	m_pPageStatic = GetControl<CEtUIStatic>( "ID_TEXT_PAGE" );
	m_pNumberStatic = GetControl<CEtUIStatic>( "ID_TEXT_COUNT" );
	m_pEnableStatic = GetControl<CEtUIStatic>( "ID_TEXT_USE" );
	m_pDisableStatic = GetControl<CEtUIStatic>( "ID_TEXT_NOTUSE" );
	m_pCheckCash = GetControl<CEtUICheckBox>( "ID_CHECKBOX_USE" );

	m_pEnableStatic->Show( false );
	m_pDisableStatic->Show( false );
	m_pCheckCash->Enable( false );
	m_pSeedButton->Enable( false );
}

void CDnLifeSlotDlg::InitCustomControl(CEtUIControl *pControl)
{
	CDnItemSlotButton *pItemSlotButton(NULL);
	pItemSlotButton = static_cast<CDnItemSlotButton*>(pControl);

	if( strstr( pControl->GetControlName(), "ID_BT_ITEM" ) ) 
	{
		pItemSlotButton->SetSlotIndex((int)m_vSeedSlot.size());
		//pItemSlotButton->SetSlotType(ST_CHARSTATUS);
		m_vSeedSlot.push_back( pItemSlotButton );
	}

	if( strstr( pControl->GetControlName(), "ID_BT_CASHITEM" ) ) 
	{
		m_pCashItemSlot = pItemSlotButton;
		m_pCashItemSlot->SetItem( m_pCashItem, CDnSlotButton::ITEM_ORIGINAL_COUNT );
		m_pCashItemSlot->SetIconColor( 0xFF888888 );
	}
}

void CDnLifeSlotDlg::Show(bool bShow)
{
	if( m_bShow == bShow )
		return;

	if( !CDnActor::s_hLocalActor ) return;
	CDnLocalPlayerActor * localActor = ((CDnLocalPlayerActor *)(CDnActor::s_hLocalActor.GetPointer()));

	if( bShow )
	{
		m_nPage = START_PAGE;

		m_vInvenSeed.clear();
		m_vInvenIndex.clear();
		GetItemTask().GetCharInventory().FindItemFromItemType( ITEMTYPE_SEED, m_vInvenSeed, m_vInvenIndex );

		RefreshPage();

		localActor->SetAction( "Stand", 0.0f, 0.0f );
		localActor->LockInput( true );
	}
	else
		localActor->LockInput( false );

	CDnCustomDlg::Show( bShow );
}

void CDnLifeSlotDlg::ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg)
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( strstr(pControl->GetControlName(), "ID_BT_ITEM" ) && uMsg == WM_LBUTTONDOWN )
		{
			for( int itr = 0; itr < (int)m_vSeedSlot.size(); ++itr )
				m_vSeedSlot[itr]->SetIconColor( 0xFFFFFFFF );

			CDnItemSlotButton *pItemSlotButton(NULL);
			pItemSlotButton = static_cast<CDnItemSlotButton*>(pControl);
			m_pSelectItem = (CDnItem *)pItemSlotButton->GetItem();

			pItemSlotButton->SetIconColor( 0xFF888888 );

			if( m_pSelectItem && IsUseSeedItem(m_pSelectItem->GetClassID()) )
			{
				m_nSelectSlotIndex = pItemSlotButton->GetSlotIndex();
				m_pSeedButton->Enable( true );

				int nCashNum = BoostCount();
				
				DNTableFileFormat* pFarmSox = GetDNTable( CDnTableDB::TFARMCULTIVATE );
				int nNeedItemNum = pFarmSox->GetFieldFromLablePtr( m_pSelectItem->GetClassID(), "_CatalystCount" )->GetInteger();;

				if( nCashNum >= nNeedItemNum )
					SetGrowBoost( true, nNeedItemNum, nNeedItemNum );
				else
					SetGrowBoost( false, nCashNum, nNeedItemNum );
			}
			else
			{
				SetGrowBoost( false ,0, 0 );
				m_pSelectItem = NULL;
				m_pSeedButton->Enable( false );
				return;
			}
		}
		else if( IsCmdControl( "ID_BT_OK" ) )
		{
			if( !m_pSelectItem )
				return;

			if( m_pCheckCash->IsChecked() )
			{
				DNTableFileFormat* pFarmSox = GetDNTable( CDnTableDB::TFARMCULTIVATE );
				int nNeedItemNum = pFarmSox->GetFieldFromLablePtr( m_pSelectItem->GetClassID(), "_CatalystCount" )->GetInteger();;

				std::vector<CDnItem *> vBoostItems;
				INT64 pBoostSerial[Farm::Max::PLANTING_ATTACHITEM];
				BYTE pBoostCount[Farm::Max::PLANTING_ATTACHITEM];
				int nCount, nBoostItemCount = 0;
				GetItemTask().GetCashInventory().FindItemFromItemType( ITEMTYPE_GROWING_BOOST, vBoostItems );

				for( nCount =0; nCount < (int)vBoostItems.size(); ++nCount )
				{
					pBoostSerial[nCount] = vBoostItems[nCount]->GetSerialID();
					nBoostItemCount += vBoostItems[nCount]->GetOverlapCount();

					if( nBoostItemCount > nNeedItemNum )
					{
						int delta = nNeedItemNum - (nBoostItemCount - vBoostItems[nCount]->GetOverlapCount() );
						pBoostCount[nCount] = delta;
						break;
					}
					else if( nBoostItemCount == nNeedItemNum )
					{
						pBoostCount[nCount] = vBoostItems[nCount]->GetOverlapCount();
						break;
					}
					else
						pBoostCount[nCount] = vBoostItems[nCount]->GetOverlapCount();					
				}

				GetLifeSkillPlantTask().SeedPlant( (BYTE)m_vInvenIndex[m_nSelectSlotIndex + (m_nPage * MAX_SEED_SLOT)], nCount+1, pBoostSerial, pBoostCount );
			}
			else 
				GetLifeSkillPlantTask().SeedPlant( (BYTE)m_vInvenIndex[m_nSelectSlotIndex + (m_nPage * MAX_SEED_SLOT)], 0, 0, 0 );

			Show( false );
		}
		else if( IsCmdControl( "ID_BT_CANCEL" ) || IsCmdControl( "ID_BT_CLOSE" ) )
		{
			Show( false );
		}
		else if( IsCmdControl( "ID_BT_PRIOR" ) )
		{
			if( START_PAGE == m_nPage )
				return;
			
			--m_nPage;
			RefreshPage();
		}
		else if( IsCmdControl( "ID_BT_NEXT" ) )
		{
			if( !IsPage( m_nPage + 1 ) )
				return;
			++m_nPage;
			RefreshPage();
		}
	}
	if( nCommand == EVENT_CHECKBOX_CHANGED )
	{
		if( IsCmdControl( "ID_CHECKBOX_USE" ) )
		{

		}
	}

	CEtUIDialog::ProcessCommand(nCommand, bTriggeredByUser, pControl, uMsg);
}

void CDnLifeSlotDlg::RefreshPage()
{
	WCHAR wszPage[32];
	int nCurPage = m_nPage + 1;
	int nMaxPage = ((int)m_vInvenSeed.size() - 1) / MAX_SEED_SLOT + 1;

	swprintf( wszPage, L"%d / %d", nCurPage, nMaxPage );
	m_pPageStatic->SetText( wszPage );

	int nInvenIndex = m_nPage * MAX_SEED_SLOT;

	for( int itr = 0; itr < MAX_SEED_SLOT; ++itr )
	{
		m_vSeedSlot[itr]->ResetSlot();
		m_vSeedSlot[itr]->SetIconColor( 0xFFFFFFFF );

		if( nInvenIndex < (int)m_vInvenSeed.size() )
			m_vSeedSlot[itr]->SetItem( (MIInventoryItem *)m_vInvenSeed[nInvenIndex], CDnSlotButton::ITEM_ORIGINAL_COUNT );

		++nInvenIndex;
	}

	SetGrowBoost( false, 0, 0 );

	m_pSeedButton->Enable( false );

//	int nCurPage = m_nPage + 1;
//	int nMaxPage = (int)( m_vInvenSeed.size() / MAX_SEED_SLOT ) + 1;

	m_pNextButton->Enable( false );
	m_pPrevButton->Enable( false );

	if( nCurPage > 1 ) m_pPrevButton->Enable( true );
	if( nCurPage < nMaxPage ) m_pNextButton->Enable( true );
}

bool CDnLifeSlotDlg::IsPage(int nPage)
{
	if( (int)m_vInvenSeed.size() - (nPage * MAX_SEED_SLOT) > 0 )
		return true;

	return false;
}

int CDnLifeSlotDlg::BoostCount()
{
	return GetItemTask().GetCashInventory().GetItemCount( CDnLifeSkillPlantTask::GROWING_BOOST_ITEM );
}

void CDnLifeSlotDlg::SetGrowBoost( bool bIs, int nGetCount, int nNeedCount )
{
	WCHAR wszPage[32];
	swprintf( wszPage, L"%d / %d", nGetCount, nNeedCount );
	m_pNumberStatic->SetText(wszPage);

	if( bIs )
	{
		m_pEnableStatic->Show( true );
		m_pDisableStatic->Show( false );
		m_pCheckCash->Enable( true );
		m_pCheckCash->SetChecked( false );
		m_pCashItemSlot->SetIconColor( 0xFFFFFFFF );
	}
	else
	{
		m_pEnableStatic->Show( false );
		m_pDisableStatic->Show( true );
		m_pCheckCash->Enable( false );
		m_pCheckCash->SetChecked( false );
		m_pCashItemSlot->SetIconColor( 0xFF888888 );
	}
}

bool CDnLifeSlotDlg::IsUseSeedItem( int nItemID )
{
	CSecondarySkill * pSecondarySkill = GetSkillTask().GetSecondarySkill( SecondarySkill::SubType::CultivationSkill );
	DNTableFileFormat* pSoxCultivate = GetDNTable( CDnTableDB::TFARMCULTIVATE );

	if( !pSecondarySkill || !pSoxCultivate )
		return false;

	int nGrade = pSoxCultivate->GetFieldFromLablePtr( nItemID, "_SecondarySkillClass" )->GetInteger();
	int nLevel = pSoxCultivate->GetFieldFromLablePtr( nItemID, "_SecondarySkillLevel" )->GetInteger();

	if( pSecondarySkill->GetGrade() < nGrade )
		return false;
	if( pSecondarySkill->GetGrade() == nGrade && pSecondarySkill->GetLevel() < nLevel )
		return false;
	
	return true;
}

