#include "stdafx.h"
#include "DnLifeConditionDlg.h"
#include "DnLifeTooltipDlg.h"
#include "DnLifeSkillPlantTask.h"
#include "DnInterface.h"
#include "DnMinimap.h"
#include "DnMainMenuDlg.h"
#ifdef PRE_ADD_INSTANT_CASH_BUY
#include "DnInstantCashShopBuyDlg.h"
#include "DnCashShopTask.h"
#endif // PRE_ADD_INSTANT_CASH_BUY
#ifdef PRE_ADD_SHORTCUT_HELP_DIALOG
#include "DnSystemDlg.h"
#endif


CDnLifeConditionDlg::CDnLifeConditionDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnCustomDlg(dialogType, pParentDialog, nID, pCallback)
, m_bShowButton( true )
, m_nSelectPage(0)
, m_nFindSelect( -1 )
, m_pStaticPage( NULL )
, m_pStaticPossibleNum( NULL )
, m_pStaticEmptyNum( NULL )
, m_pCheckAll( NULL )
, m_pCheckEmpty( NULL )
, m_pButtonNext( NULL )
, m_pButtonPrev( NULL )
{
}

CDnLifeConditionDlg::~CDnLifeConditionDlg()
{

}

void CDnLifeConditionDlg::Initialize(bool bShow)
{
#if defined( PRE_REMOVE_FARM_WATER )
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "LifeConditionDlg.ui" ).c_str(), bShow );
#else
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "LifeCondition_WDlg.ui" ).c_str(), bShow );
#endif	// #if defined( PRE_REMOVE_FARM_WATER )
}

void CDnLifeConditionDlg::InitialUpdate()
{
	m_pStaticPage = GetControl<CEtUIStatic>( "ID_BT_PAGE" );
	m_pStaticPossibleNum = GetControl<CEtUIStatic>( "ID_TEXT_COUNT" );
	m_pStaticEmptyNum = GetControl<CEtUIStatic>( "ID_TEXT_EMPTYCOUNT" );
	m_pCheckAll = GetControl<CEtUICheckBox>( "ID_CHECKBOX_MAPALL" );
	m_pButtonNext = GetControl<CEtUIButton>( "ID_BT_NEXT" );
	m_pButtonPrev = GetControl<CEtUIButton>( "ID_BT_PRIOR" );
	m_pCheckEmpty = GetControl<CEtUICheckBox>( "ID_CHECKBOX_EMPTY" );

	char szControlName[32];
	for( int i = 0; i < PLANT_MAX; ++i )
	{
		sprintf_s(szControlName, _countof(szControlName), "ID_BT_ITME%d", i);
		m_PlantInfoList[i].m_pButtonSlot = GetControl<CDnItemSlotButton>(szControlName);
		m_PlantInfoList[i].m_pButtonSlot->SetSlotType( ST_FARM_CONDITION );

		sprintf_s(szControlName, _countof(szControlName), "ID_STATIC_BAR%d", i);
		m_PlantInfoList[i].m_pStaticBar = GetControl<CEtUIStatic>(szControlName);

		sprintf_s(szControlName, _countof(szControlName), "ID_TEXT_NAME%d", i);
		m_PlantInfoList[i].m_pStaticName = GetControl<CEtUIStatic>(szControlName);

		sprintf_s(szControlName, _countof(szControlName), "ID_PRB_TIME%d", i);
		m_PlantInfoList[i].m_pProgressTime = GetControl<CEtUIProgressBar>(szControlName);

		sprintf_s(szControlName, _countof(szControlName), "ID_PRB_WATER%d", i);
		m_PlantInfoList[i].m_pProgressWater = GetControl<CEtUIProgressBar>(szControlName);

		sprintf_s(szControlName, _countof(szControlName), "ID_CHECKBOX_MAP%d", i);
		m_PlantInfoList[i].m_pCheckBox = GetControl<CEtUICheckBox>(szControlName);

		m_PlantInfoList[i].Clear();
	}
}

void CDnLifeConditionDlg::Show(bool bShow)
{
#ifdef PRE_ADD_INSTANT_CASH_BUY
	if( m_bShow )
	{
		int nShowType = GetCashShopTask().GetInstantCashBuyEnableType( INSTANT_BUY_FARM );
		if( nShowType == INSTANT_BUY_HIDE )
		{
			GetControl<CEtUIButton>( "UI_BUTTON_BUYSHORTCUT" )->Show( false );
		}
		else if( nShowType == INSTANT_BUY_SHOW )
		{
			GetControl<CEtUIButton>( "UI_BUTTON_BUYSHORTCUT" )->Show( true );
			GetControl<CEtUIButton>( "UI_BUTTON_BUYSHORTCUT" )->Enable( true );
		}
		else if( nShowType == INSTANT_BUY_DISABLE )
		{
			GetControl<CEtUIButton>( "UI_BUTTON_BUYSHORTCUT" )->Show( true );
			GetControl<CEtUIButton>( "UI_BUTTON_BUYSHORTCUT" )->Enable( false );
		}
	}
#endif // PRE_ADD_INSTANT_CASH_BUY
	if( m_bShow == bShow )
		return;

	CDnCustomDlg::Show( bShow );
}

void CDnLifeConditionDlg::Process(float fElapsedTime)
{
	RefreshEmptyAreaInfo( m_pCheckEmpty->IsChecked() );

	for( int itr = 0; itr < PLANT_MAX; ++itr )
		m_PlantInfoList[itr].Process();

	if( -1 != m_nFindSelect && m_PlantInfoList[m_nFindSelect].m_pPlantInfo )
	{
		CDnLifeSkillPlantTask::SPlantTooltip sInfo;

		sInfo = GetLifeSkillPlantTask().PlantInfo( m_PlantInfoList[m_nFindSelect].m_pPlantInfo->m_nAreaIndex );

		GetInterface().GetLifeTooltipDlg()->SetInfo( sInfo );
		GetInterface().GetLifeTooltipDlg()->AddRenderCount();
		GetInterface().GetLifeTooltipDlg()->UpdateTooltip( this, m_fMouseX, m_fMouseY );
	}

	//현재 열린 창이 있을 경우 숨긴다.
	if( GetInterface().GetMainMenuDialog()->IsOpenDialog() )
	{
		if( m_bShow )
			Show( false );
	}
	else
	{
		if( m_bShowButton && !m_bShow )
			Show( true );
	}

	CEtUIDialog::Process( fElapsedTime );
}

void CDnLifeConditionDlg::ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg)
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( strstr( pControl->GetControlName(), "ID_BT_PRIOR" ) )
		{
			if( m_nSelectPage <= 0 )
				return;

			--m_nSelectPage;
			RefreshPage();
		}
		else if( strstr( pControl->GetControlName(), "ID_BT_NEXT" ) )
		{
			if( (m_nSelectPage+1) * PLANT_MAX >= (int)m_mPlantInfo.size() )
				return;

			++m_nSelectPage;
			RefreshPage();
		}
		else if( strstr( pControl->GetControlName(), "ID_BT_SHOWMAP" ) )
		{
			CDnMainMenuDlg *pMainMenuDlg = GetInterface().GetMainMenuDialog();
			if (pMainMenuDlg )
				pMainMenuDlg->ToggleShowDialog(CDnMainMenuDlg::ZONEMAP_DIALOG);
		}
#ifdef PRE_ADD_INSTANT_CASH_BUY
		if( IsCmdControl( "UI_BUTTON_BUYSHORTCUT" ) )
		{
			GetInterface().ShowInstantCashShopBuyDlg( true, INSTANT_BUY_FARM, NULL );
			return;
		}
#endif // PRE_ADD_INSTANT_CASH_BUY

#ifdef PRE_ADD_SHORTCUT_HELP_DIALOG
		else if( IsCmdControl("ID_BT_SMALLHELP") )
		{
			CDnMainMenuDlg* pMainMenuDlg = GetInterface().GetMainMenuDialog();
			CDnSystemDlg* pSystemDlg = pMainMenuDlg->GetSystemDlg();
			if(pSystemDlg)
				pSystemDlg->ShowChoiceHelpDlg(HELP_SHORTCUT_FARM);
		}
#endif
	}
	else if( nCommand == EVENT_CHECKBOX_CHANGED )
	{
		CEtUICheckBox * pCheckBox = (CEtUICheckBox *)pControl;

		bool bCheck = pCheckBox->IsChecked();

		if( strstr( pControl->GetControlName(), "ID_CHECKBOX_MAP" ) )
		{
			int nCount = 0;

			if( strstr( pControl->GetControlName(), "ID_CHECKBOX_MAPALL" ) )
			{
				for( PLANT_MAP_ITOR itor = m_mPlantInfo.begin(); itor != m_mPlantInfo.end(); ++itor )
					itor->second.m_bShowCheck = bCheck;

				for( int itr = 0; itr < PLANT_MAX; ++itr )
					m_PlantInfoList[itr].m_pCheckBox->SetChecked( bCheck );

				RefreshMinimap( ALL, bCheck );
				return;
			}
			else if( strstr( pControl->GetControlName(), "ID_CHECKBOX_MAP0" ) )
				nCount = 0;
			else if( strstr( pControl->GetControlName(), "ID_CHECKBOX_MAP1" ) )
				nCount = 1;
			else if( strstr( pControl->GetControlName(), "ID_CHECKBOX_MAP2" ) )
				nCount = 2;
			else if( strstr( pControl->GetControlName(), "ID_CHECKBOX_MAP3" ) )
				nCount = 3;
			else if( strstr( pControl->GetControlName(), "ID_CHECKBOX_MAP4" ) )
				nCount = 4;

			if( m_PlantInfoList[nCount].m_pPlantInfo )
			{
				m_PlantInfoList[nCount].m_pPlantInfo->m_bShowCheck = bCheck;
				RefreshMinimap( m_PlantInfoList[nCount].m_pPlantInfo->m_nAreaIndex, bCheck );
			}
		}
		else if( strstr( pControl->GetControlName(), "ID_CHECKBOX_EMPTY" ) )
		{
			RefreshEmptyAreaInfo( bCheck );
		}
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

bool CDnLifeConditionDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	m_hWnd = hWnd;

	if( !IsShow() )
		return false;

	bool bRet = CEtUIDialog::MsgProc( hWnd, uMsg, wParam, lParam );

	switch( uMsg )
	{
	case WM_KEYDOWN:
	case WM_MOUSEMOVE:
		{
			POINT MousePoint;
			MousePoint.x = short( LOWORD( lParam ) );
			MousePoint.y = short( HIWORD( lParam ) );

			float fMouseX, fMouseY;
			PointToFloat( MousePoint, fMouseX, fMouseY );

			m_nFindSelect = FindInsideItem( fMouseX, fMouseY );
		}
		break;
	}

	return bRet;
}

void CDnLifeConditionDlg::ChangeState(int nAreaIndex, int nItemID, int eAreaState, float fTime, float fWater, char * szHarvestIconName )
{
	PLANT_MAP_ITOR itor = m_mPlantInfo.find( nAreaIndex );

	if( itor != m_mPlantInfo.end() )
	{
		if( Farm::AreaState::NONE == eAreaState )
		{
			RefreshMinimap( itor->first, false );
			m_mPlantInfo.erase( itor );	
		}
		else 
		{
			itor->second.m_eAreaState = eAreaState;
			itor->second.m_nItemID = nItemID;
			itor->second.m_fTime = fTime;
			itor->second.m_fWater = fWater;
		}
	}
	else
	{
		if( Farm::AreaState::NONE == eAreaState )
			return;

		SPlantInfo info(nItemID, eAreaState, fTime, fWater, nAreaIndex, m_pCheckAll->IsChecked(), szHarvestIconName );

		m_mPlantInfo.insert( make_pair(nAreaIndex, info) );
	}
}

void CDnLifeConditionDlg::RefreshPage()
{
	for( int itr = 0; itr < PLANT_MAX; ++itr )
		m_PlantInfoList[itr].Clear();

	int nStart = m_nSelectPage * PLANT_MAX;

	if( 0 != nStart && nStart >= (int)m_mPlantInfo.size() )
	{
		--m_nSelectPage;
		nStart = m_nSelectPage * PLANT_MAX;
	}

	PLANT_MAP_ITOR itor = m_mPlantInfo.begin();
	for( int itr = 0; itr < nStart; ++itr )
		++itor;

	for( int itr = 0; itr < PLANT_MAX && itor != m_mPlantInfo.end(); ++itor )
	{
		m_PlantInfoList[itr].m_pItem = CDnItem::CreateItem( itor->second.m_nItemID, 0, true );
		m_PlantInfoList[itr].m_pButtonSlot->SetItem( m_PlantInfoList[itr].m_pItem, CDnSlotButton::ITEM_ORIGINAL_COUNT );
		m_PlantInfoList[itr].m_pStaticBar->Show( true );
		m_PlantInfoList[itr].m_pStaticName->SetText( m_PlantInfoList[itr].m_pItem->GetName() );
		m_PlantInfoList[itr].m_pCheckBox->Show( true );
		m_PlantInfoList[itr].m_pCheckBox->SetChecked( itor->second.m_bShowCheck );
		m_PlantInfoList[itr].m_pPlantInfo = &(itor->second);

		if( Farm::AreaState::GROWING == itor->second.m_eAreaState )
		{
			m_PlantInfoList[itr].m_pProgressTime->Show( true );
			m_PlantInfoList[itr].m_pProgressTime->SetProgress( itor->second.m_fTime );
			
#if defined(PRE_REMOVE_FARM_WATER)
			m_PlantInfoList[itr].m_pProgressWater->Show( false );
#else
			m_PlantInfoList[itr].m_pProgressWater->Show( true );
			m_PlantInfoList[itr].m_pProgressWater->SetProgress( itor->second.m_fWater );
#endif	// PRE_REMOVE_FARM_WATER
		}
		else if( Farm::AreaState::COMPLETED == itor->second.m_eAreaState )
		{
			m_PlantInfoList[itr].m_pProgressTime->Show( true );
			m_PlantInfoList[itr].m_pProgressWater->Show( false );
			m_PlantInfoList[itr].m_pPlantInfo->m_fTime = 100.0f;
		}

		RefreshMinimap( itor->first, itor->second.m_bShowCheck );
		
		++itr;
	}

	PageCount();
}

void CDnLifeConditionDlg::PageCount()
{
	WCHAR wszPage[32];

	int nNowPage = m_nSelectPage + 1;
	int nMaxPage = ((int)m_mPlantInfo.size()-1) / PLANT_MAX + 1;

	swprintf( wszPage, L"%d / %d", nNowPage, nMaxPage );

	m_pStaticPage->SetText(wszPage);
	
	m_pButtonPrev->Enable( false );
	m_pButtonNext->Enable( false );

	if( nNowPage > 1 ) m_pButtonPrev->Enable( true );
	if( nNowPage < nMaxPage ) m_pButtonNext->Enable( true );
}

int CDnLifeConditionDlg::FindInsideItem( float fX, float fY )
{
	for( int itr = 0; itr < PLANT_MAX; ++itr )
	{
		if( m_PlantInfoList[itr].m_pItem && m_PlantInfoList[itr].IsInsideItem( fX, fY ) )
			return itr;
	}

	return -1;
}

void CDnLifeConditionDlg::RefreshMinimap( int nAreaIndex, bool bCheck )
{	
	if( ALL == nAreaIndex )
	{
		for( PLANT_MAP_ITOR itor = m_mPlantInfo.begin(); itor != m_mPlantInfo.end(); ++itor )
		{
			EtVector3 vPos = GetLifeSkillPlantTask().GetFarmAreaPos( itor->first );
			if( bCheck )
				GetMiniMap().InsertHarvest( itor->first, vPos, itor->second.m_szHarvestIconName );
			else
				GetMiniMap().DeleteHarvest( itor->first );
		}
	}
	else
	{
		EtVector3 vPos = GetLifeSkillPlantTask().GetFarmAreaPos( nAreaIndex );
		PLANT_MAP_ITOR itor = m_mPlantInfo.find( nAreaIndex );

		if( itor == m_mPlantInfo.end() )
			return;

		if( bCheck )
			GetMiniMap().InsertHarvest( nAreaIndex, vPos, itor->second.m_szHarvestIconName );
		else
			GetMiniMap().DeleteHarvest( nAreaIndex );
	}
}

void CDnLifeConditionDlg::RefreshEmptyAreaInfo( bool bShow )
{
	if( bShow )
	{
		std::vector< std::pair<int, EtVector3> > vecEmptyAreaInfo;

		GetMiniMap().AllDeleteEmptyArea();

		GetLifeSkillPlantTask().GetEmptyAreaInfo( vecEmptyAreaInfo );

		for( int itr = 0; itr < (int)vecEmptyAreaInfo.size(); ++itr )
			GetMiniMap().InsertEmptyArea( vecEmptyAreaInfo[itr].first, vecEmptyAreaInfo[itr].second );
	}
	else
		GetMiniMap().AllDeleteEmptyArea();
}

void CDnLifeConditionDlg::GetMyFarmInfo( std::vector< std::pair<int, int> > & vecMyFarmInfo )
{
	for( PLANT_MAP_ITOR itor = m_mPlantInfo.begin(); itor != m_mPlantInfo.end(); ++itor )
		vecMyFarmInfo.push_back( std::make_pair( itor->first, itor->second.m_nItemID ) );
}

void CDnLifeConditionDlg::GetEmptyAreaInfo( std::vector< std::pair<int, EtVector3> > & vecEmptyAreaInfo )
{
	if( m_pCheckEmpty->IsChecked() )
		GetLifeSkillPlantTask().GetEmptyAreaInfo( vecEmptyAreaInfo );
}

void CDnLifeConditionDlg::SetPossibleCount( int nPossibleCount, int nMax )
{
	WCHAR wszString[128] = {0,};

	swprintf_s( wszString, _countof(wszString), L"%d / %d", nPossibleCount, nMax );
	m_pStaticPossibleNum->SetText( wszString );
}

void CDnLifeConditionDlg::SetEmptyCount( int nEmpty, int nMax )
{
	WCHAR wszString[512];
	swprintf_s( wszString, _countof(wszString), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7529 ), nEmpty, nMax );	// 농장 빈 공간 ( %d / %d)
	m_pStaticEmptyNum->SetText( wszString );
}

