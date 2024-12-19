#include "StdAfx.h"
#include "DnLifeSkillCookingDlg.h"
#include "DnLifeSkillCookingTask.h"
#include "ManufactureSkill.h"
#include "TaskManager.h"
#include "DnItem.h"
#include "DnItemTask.h"
#include "SecondarySkillRecipe.h"
#include "DnInterface.h"
#include "DnSkillTask.h"
#include "DnTableDB.h"
#include "DnSecondarySkillRecipeItem.h"
#ifdef PRE_ADD_SHORTCUT_HELP_DIALOG
#include "DnSystemDlg.h"
#include "DnMainMenuDlg.h"
#endif

#ifdef PRE_ADD_COOKING_SYSTEM

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

//////////////////////////////////////////////////////////////////////////
// CDnLifeSkillCookingDlg
//////////////////////////////////////////////////////////////////////////

CDnLifeSkillCookingDlg::CDnLifeSkillCookingDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback, true )
, m_pDnLifeSkillCookingRecipeDlg( NULL )
, m_pButtonAddRecipe( NULL )
, m_pButtonRemoveRecipe( NULL )
, m_pButtonCook( NULL )
, m_pButtonCookAll( NULL )
, m_pButtonExtract( NULL )
, m_pButtonCancel( NULL )
, m_pButtonPrevious( NULL )
, m_pButtonNext( NULL )
, m_pStaticPage( NULL )
, m_pCookingSlotBtn( NULL )
, m_pStaticTitle( NULL )
, m_pStaticLevel( NULL )
, m_pStaticCompletion( NULL )
, m_nCurrentPage( 0 )
, m_nMaximumPage( 0 )
, m_nCurrentPageListCount( 0 )
, m_nSelectRecipe( -1 )
{
	memset( m_pRequiredToolSlotBtn, 0, sizeof( m_pRequiredToolSlotBtn ) );
	memset( m_pRequiredIngredientSlotBtn, 0, sizeof( m_pRequiredIngredientSlotBtn ) );
}

CDnLifeSkillCookingDlg::~CDnLifeSkillCookingDlg()
{
	SAFE_DELETE( m_pDnLifeSkillCookingRecipeDlg );
}

void CDnLifeSkillCookingDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "LifeCookDlg.ui" ).c_str(), bShow );
}

void CDnLifeSkillCookingDlg::InitialUpdate()
{
	m_pDnLifeSkillCookingRecipeDlg = new CDnLifeSkillCookingRecipeDlg( UI_TYPE_MODAL );
	m_pDnLifeSkillCookingRecipeDlg->Initialize( false  );

	char szStr[64];
	for( int i=0; i<MAX_COOKING_LIST; i++ )
	{
		sprintf_s( szStr, "ID_BT_ITEM%d", i );
		m_stUICookingListInfo[i].m_pCookingSlotBtn = GetControl<CDnItemSlotButton>( szStr );
		sprintf_s( szStr, "ID_TEXT_NAME%d", i );
		m_stUICookingListInfo[i].m_pStaticTitle = GetControl<CEtUIStatic>( szStr );
		sprintf_s( szStr, "ID_TEXT_LEVEL%d", i );
		m_stUICookingListInfo[i].m_pStaticLevel = GetControl<CEtUIStatic>( szStr );
		sprintf_s( szStr, "ID_TEXT_COUNT%d", i );
		m_stUICookingListInfo[i].m_pStaticPercent = GetControl<CEtUIStatic>( szStr );
		sprintf_s( szStr, "ID_SELECT%d", i );
		m_stUICookingListInfo[i].m_pStaticSelect = GetControl<CEtUIStatic>( szStr );
		sprintf_s( szStr, "ID_STATIC_BAR%d", i );
		m_stUICookingListInfo[i].m_pStaticBackground = GetControl<CEtUIStatic>( szStr );
		sprintf_s( szStr, "ID_STATIC1%d", i );
		m_stUICookingListInfo[i].m_pStaticLevelText = GetControl<CEtUIStatic>( szStr );
		sprintf_s( szStr, "ID_PRB_GAUGE%d", i );
		m_stUICookingListInfo[i].m_pProgressAchievement = GetControl<CEtUIProgressBar>( szStr );
	}

	m_pButtonAddRecipe = GetControl<CEtUIButton>( "ID_BT_ADD" );
	m_pButtonRemoveRecipe = GetControl<CEtUIButton>( "ID_BT_DELET" );
	m_pButtonRemoveRecipe->Enable( false );
	m_pButtonCook = GetControl<CEtUIButton>( "ID_BT_COOK" );
	m_pButtonCook->Enable( false );
	m_pButtonCookAll = GetControl<CEtUIButton>( "ID_BT_ALLCOOK" );
	m_pButtonCookAll->Enable( false );
	m_pButtonExtract = GetControl<CEtUIButton>( "ID_BT_EXTRACT" );
	m_pButtonExtract->Enable( false );
	m_pButtonCancel = GetControl<CEtUIButton>( "ID_BT_CANCEL" );
	m_pButtonPrevious = GetControl<CEtUIButton>( "ID_BT_PRIOR" );
	m_pButtonPrevious->Enable( false );
	m_pButtonNext = GetControl<CEtUIButton>( "ID_BT_NEXT" );
	m_pButtonNext->Enable( false );
	m_pStaticPage = GetControl<CEtUIStatic>( "ID_TEXT_PAGE" );

	m_pCookingSlotBtn = GetControl<CDnItemSlotButton>( "ID_BT_ITEM4" );
	m_pStaticTitle = GetControl<CEtUIStatic>( "ID_TEXT0" );
	m_pStaticLevel = GetControl<CEtUIStatic>( "ID_TEXT_LEVEL" );
	m_pStaticCompletion = GetControl<CEtUIStatic>( "ID_TEXT_COMPLETION" );

	for( int i=0; i<MAX_REQUIRED_NUM; i++ )
	{
		sprintf_s( szStr, "ID_BT_ITEM%d", i+5 );
		m_pRequiredToolSlotBtn[i] = GetControl<CDnItemSlotButton>( szStr );
		sprintf_s( szStr, "ID_BT_ITEM%d", i+10 );
		m_pRequiredIngredientSlotBtn[i] = GetControl<CDnItemSlotButton>( szStr );
		m_pRequiredIngredientSlotBtn[i]->SetSlotType( ST_ITEM_COMPOUND_JEWEL );
	}
}

void CDnLifeSkillCookingDlg::Show( bool bShow )
{ 
	if( m_bShow == bShow )
		return;

	if( !bShow )
	{
		if( m_pDnLifeSkillCookingRecipeDlg )
			m_pDnLifeSkillCookingRecipeDlg->Show( bShow );
	}

	CEtUIDialog::Show( bShow );
}

void CDnLifeSkillCookingDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( strstr( pControl->GetControlName(), "ID_BT_PRIOR" ) )
		{
			--m_nCurrentPage;
			if( m_nCurrentPage < 0 )
				m_nCurrentPage = 0;

			m_nSelectRecipe = -1;
			ResetSelectedRecipeInfo();
			UpdateCookingList();
		}
		else if( strstr( pControl->GetControlName(), "ID_BT_NEXT" ) )
		{
			++m_nCurrentPage;
			if( m_nCurrentPage > m_nMaximumPage )
				m_nCurrentPage = m_nMaximumPage;

			m_nSelectRecipe = -1;
			ResetSelectedRecipeInfo();
			UpdateCookingList();
		}
		else if( strstr( pControl->GetControlName(), "ID_BT_ADD" ) )
		{
			if( m_pDnLifeSkillCookingRecipeDlg )
				m_pDnLifeSkillCookingRecipeDlg->Show( true );
		}
		else if( strstr( pControl->GetControlName(), "ID_BT_DELET" ) && m_nSelectRecipe > -1 )
		{
			CSecondarySkillRecipe* pCookRecipe = GetCurrentCookRecipe();

			if( pCookRecipe )
			{
				TItemInfo itemInfo;
				if( CDnItem::MakeItemInfo( pCookRecipe->GetItemID(), 1, itemInfo ) )
				{
					CDnItem* pItem = GetItemTask().CreateItem( itemInfo );
					if( pItem )
					{
						WCHAR wszMessage[256];
						wsprintf( wszMessage, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7446 ), pItem->GetName() );
						
						GetInterface().MessageBox( wszMessage, MB_YESNO, MSGBOX_DELETE_RECIPE, this );
					}
					SAFE_DELETE( pItem );
				}
			}
		}
		else if( strstr( pControl->GetControlName(), "ID_BT_EXTRACT" ) && m_nSelectRecipe > -1 )
			GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7445 ), MB_YESNO, MSGBOX_EXTRACT_RECIPE, this );
		else if( strstr( pControl->GetControlName(), "ID_BT_COOK" )  && m_nSelectRecipe > -1 )
			GetLifeSkillCookingTask().StartManufacture( GetCurrentCookRecipe(), false );
		else if( strstr( pControl->GetControlName(), "ID_BT_ALLCOOK" ) && m_nSelectRecipe > -1 )
			GetLifeSkillCookingTask().StartManufacture( GetCurrentCookRecipe(), true );
		else if( IsCmdControl( "ID_BT_CANCEL" ) || IsCmdControl( "ID_BT_CLOSE" ) )
			Show( false );
#ifdef PRE_ADD_SHORTCUT_HELP_DIALOG
		else if( IsCmdControl("ID_BT_SMALLHELP") )
		{
			CDnMainMenuDlg* pMainMenuDlg = GetInterface().GetMainMenuDialog();
			CDnSystemDlg* pSystemDlg = pMainMenuDlg->GetSystemDlg();
			if(pSystemDlg)
				pSystemDlg->ShowChoiceHelpDlg(HELP_SHORTCUT_COOK);
		}
#endif
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnLifeSkillCookingDlg::UpdatePageUI()
{
	wchar_t wszPage[10]={0};
	swprintf_s( wszPage, 10, L"%d/%d", m_nCurrentPage+1, m_nMaximumPage+1 );
	m_pStaticPage->SetText( wszPage );

	if( m_nCurrentPage == m_nMaximumPage )
		m_pButtonNext->Enable(false);
	else
		m_pButtonNext->Enable(true);

	if( m_nCurrentPage == 0 )
		m_pButtonPrevious->Enable(false);
	else
		m_pButtonPrevious->Enable(true);
}

void CDnLifeSkillCookingDlg::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nID == MSGBOX_DELETE_RECIPE )
	{
		if( nCommand == EVENT_BUTTON_CLICKED )
		{
			if( IsCmdControl( "ID_YES" ) )
			{
				CSecondarySkillRecipe* pCookRecipe = GetCurrentCookRecipe();
				if( pCookRecipe )
					GetLifeSkillCookingTask().SendDeleteRecipe( pCookRecipe->GetSkillID(), pCookRecipe->GetItemID() );
			}
		}
	}
	else if( nID == MSGBOX_EXTRACT_RECIPE )
	{
		if( nCommand == EVENT_BUTTON_CLICKED )
		{
			if( IsCmdControl( "ID_YES" ) )
			{
				CSecondarySkillRecipe* pCookRecipe = GetCurrentCookRecipe();
				if( pCookRecipe )
					GetLifeSkillCookingTask().SendExtractRecipe( pCookRecipe->GetSkillID(), pCookRecipe->GetItemID() );
			}
		}
	}
}

CSecondarySkillRecipe* CDnLifeSkillCookingDlg::GetCurrentCookRecipe()
{
	CSecondarySkillRecipe* pCookRecipe = NULL;
	if( m_nSelectRecipe > -1 )
	{
		CManufactureSkill* pCookingSkill = GetLifeSkillCookingTask().GetCookingSkill();
		if( pCookingSkill )
		{
			std::vector<CSecondarySkillRecipe*>& RecipeList = pCookingSkill->GetList();
			std::vector<CSecondarySkillRecipe*>::iterator iter = RecipeList.begin();

			int nRecipeCount = static_cast<int>( RecipeList.size() );

			if( nRecipeCount > 0 && nRecipeCount > ( m_nCurrentPage * MAX_COOKING_LIST ) + m_nSelectRecipe )
			{
				iter += ( m_nCurrentPage * MAX_COOKING_LIST ) + m_nSelectRecipe;
				pCookRecipe = (*iter);
			}
		}
	}

	return pCookRecipe;
}

bool CDnLifeSkillCookingDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsShow() )
		return false;

	switch( uMsg )
	{
		case WM_KEYDOWN:
			{
				if( wParam == VK_ESCAPE )
				{
					Show( false );
					return true;
				}
			}
			break;
		case WM_LBUTTONDOWN:
			{
				POINT MousePoint;
				float fMouseX, fMouseY;

				MousePoint.x = short( LOWORD( lParam ) );
				MousePoint.y = short( HIWORD( lParam ) );
				PointToFloat( MousePoint, fMouseX, fMouseY );

				bool bSelect = false;
				int nSelect = 0;
				for( ; nSelect<m_nCurrentPageListCount; nSelect++ )
				{
					SUICoord uiCoordsSelect;
					m_stUICookingListInfo[nSelect].m_pStaticSelect->GetUICoord( uiCoordsSelect );
					if( uiCoordsSelect.IsInside( fMouseX, fMouseY ) )
					{
						bSelect = true;
						break;
					}
				}
				
				if( bSelect && m_nSelectRecipe != nSelect )
				{
					ResetSelectedRecipeInfo();
					m_nSelectRecipe = nSelect;

					for( int i=0; i<MAX_COOKING_LIST; i++ )
						m_stUICookingListInfo[i].m_pStaticSelect->Show( false );

					m_stUICookingListInfo[nSelect].m_pStaticSelect->Show( true );
					
					SetSelectedRecipeInfo();
					m_pButtonRemoveRecipe->Enable( true );

					CSecondarySkillRecipe* pCookRecipe = GetCurrentCookRecipe();
					if( pCookRecipe && pCookRecipe->GetExp() >= pCookRecipe->GetMaxExp() )
						m_pButtonExtract->Enable( true );
				}
			}
			break;
	}

	return CEtUIDialog::MsgProc( hWnd, uMsg, wParam, lParam );
}

void CDnLifeSkillCookingDlg::SetSelectedRecipeInfo()
{
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TSecondarySkillRecipe );
	if( pSox == NULL )
	{
		DN_ASSERT( 0, "Invalid SecondarySkillRecipe" );
		return;
	}

	char szStr[64];
	CSecondarySkillRecipe* pCookRecipe = GetCurrentCookRecipe();
	if( pCookRecipe )
	{
		// 레시피 현재 달성율을 구함
		int fAchievementRate = static_cast<int>(( static_cast<float>( pCookRecipe->GetExp() ) / static_cast<float>( pCookRecipe->GetMaxExp() ) ) * 100.0f);
		int nMakeItemCount = pSox->GetFieldFromLablePtr( pCookRecipe->GetItemID(), "_MakeItemCount" )->GetInteger();

		int nMakeItem = 1;
		for( int i=0; i<nMakeItemCount; i++ )
		{
			sprintf_s( szStr, "_MinExp%d", i+1 );
			int nMinExp = pSox->GetFieldFromLablePtr( pCookRecipe->GetItemID(), szStr )->GetInteger();
			sprintf_s( szStr, "_MaxExp%d", i+1 );
			int nMaxExp = pSox->GetFieldFromLablePtr( pCookRecipe->GetItemID(), szStr )->GetInteger();
			if( fAchievementRate >= nMinExp && fAchievementRate <= nMaxExp )
			{
				nMakeItem = i+1;
				break;
			}
		}

		sprintf_s( szStr, "_MakeItem%dID", nMakeItem );
		int nItemID = pSox->GetFieldFromLablePtr( pCookRecipe->GetItemID(), szStr )->GetInteger();

		TItemInfo itemInfo;
		if( CDnItem::MakeItemInfo( nItemID, 1, itemInfo ) )
		{
			CDnItem* pItem = GetItemTask().CreateItem( itemInfo );
			if( pItem )
			{
				m_pCookingSlotBtn->SetItem( pItem, CDnSlotButton::ITEM_ORIGINAL_COUNT );
				m_pCookingSlotBtn->Show( true );
				m_pStaticTitle->SetText( pItem->GetName() );
				m_pStaticTitle->Show( true );

				WCHAR wszGrade[64] = {0,};
				int nGradeStringIndex = 0;
				switch( pCookRecipe->GetRequiredGrade() )
				{
					case SecondarySkill::Grade::Beginning:
						nGradeStringIndex = 7303;
						break;
					case SecondarySkill::Grade::Intermediate:
						nGradeStringIndex = 7304;
						break;
					case SecondarySkill::Grade::Advanced:
						nGradeStringIndex = 7305;
						break;
				}
				swprintf_s( wszGrade, _countof( wszGrade ), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nGradeStringIndex ) );
				WCHAR wszLevel[64] = {0,};
				swprintf_s( wszLevel, _countof( wszLevel ), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7430 ), pCookRecipe->GetRequiredSkillLevel() );
				WCHAR wszText[128] = {0,};
				swprintf_s( wszText, _countof( wszText ), L"%s %s", wszGrade, wszLevel );
				m_pStaticLevel->SetText( wszText );
				m_pStaticLevel->Show( true );

				sprintf_s( szStr, "_TypeCompletion%d", nMakeItem );
				int nCompletionID = pSox->GetFieldFromLablePtr( pCookRecipe->GetItemID(), szStr )->GetInteger();
				m_pStaticCompletion->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nCompletionID ) );
				m_pStaticCompletion->Show( true );

				bool bCanManufacture = true;
				for( int i=0; i<MAX_REQUIRED_NUM; i++ )
				{
					sprintf_s( szStr, "_ToolItem%dID", i+1 );
					int nTooltemID = pSox->GetFieldFromLablePtr( pCookRecipe->GetItemID(), szStr )->GetInteger();
					if( nTooltemID > 0 )
					{
						if( CDnItem::MakeItemInfo( nTooltemID, 1, itemInfo ) )
						{
							pItem = GetItemTask().CreateItem( itemInfo );
							m_pRequiredToolSlotBtn[i]->SetItem( pItem, CDnSlotButton::ITEM_ORIGINAL_COUNT );
							m_pRequiredToolSlotBtn[i]->Show( true );
							int nCurItemCount = GetItemTask().GetCharInventory().GetItemCount( nTooltemID );

							if( nCurItemCount > 0 )
								m_pRequiredToolSlotBtn[i]->SetRegist( false );
							else
							{
								bCanManufacture = false;
								m_pRequiredToolSlotBtn[i]->SetRegist( true );
							}
						}
					}
					sprintf_s( szStr, "_MaterialItem%dID", i+1 );
					int nIngredientID = pSox->GetFieldFromLablePtr( pCookRecipe->GetItemID(), szStr )->GetInteger();
					if( nIngredientID > 0 )
					{
						if( CDnItem::MakeItemInfo( nIngredientID, 1, itemInfo ) )
						{
							sprintf_s( szStr, "_Count%d", i+1 );
							int nNeedItemCount = pSox->GetFieldFromLablePtr( pCookRecipe->GetItemID(), szStr )->GetInteger();
							pItem = GetItemTask().CreateItem( itemInfo );
							m_pRequiredIngredientSlotBtn[i]->SetItem( pItem, CDnSlotButton::ITEM_ORIGINAL_COUNT );
							int nCurItemCount = GetItemTask().GetCharInventory().GetItemCount( nIngredientID );

							if( nNeedItemCount <= nCurItemCount )
								m_pRequiredIngredientSlotBtn[i]->SetRegist( false );
							else
							{
								bCanManufacture = false;
								m_pRequiredIngredientSlotBtn[i]->SetRegist( true );
							}

							m_pRequiredIngredientSlotBtn[i]->SetJewelCount( nNeedItemCount, nCurItemCount, false );
							m_pRequiredIngredientSlotBtn[i]->Show( true );
						}
					}
				}

				if( bCanManufacture )
				{
					m_pButtonCook->Enable( true );
					m_pButtonCookAll->Enable( true );
				}
			}
		}
	}
}

void CDnLifeSkillCookingDlg::ClearSelectedRecipe()
{
	ResetSelectedRecipeInfo();
	m_nCurrentPage = 0;
}

void CDnLifeSkillCookingDlg::ResetSelectedRecipeInfo()
{
	m_nSelectRecipe = -1;

	m_pCookingSlotBtn->Show( false );
	m_pStaticTitle->Show( false );
	m_pStaticLevel->Show( false );
	m_pStaticCompletion->Show( false );

	for( int i=0; i<MAX_REQUIRED_NUM; i++ )
	{
		m_pRequiredToolSlotBtn[i]->ResetSlot();
		m_pRequiredToolSlotBtn[i]->Show( false );
		m_pRequiredIngredientSlotBtn[i]->ResetSlot();
		m_pRequiredIngredientSlotBtn[i]->Show( false );
	}

	for( int i=0; i<MAX_COOKING_LIST; i++ )
		m_stUICookingListInfo[i].m_pStaticSelect->Show( false );

	m_pButtonRemoveRecipe->Enable( false );
	m_pButtonCook->Enable( false );
	m_pButtonCookAll->Enable( false );
	m_pButtonExtract->Enable( false );
}

void CDnLifeSkillCookingDlg::UpdateCookingList()
{
	for( int i=0; i<MAX_COOKING_LIST; i++ )
	{
		m_stUICookingListInfo[i].m_pCookingSlotBtn->ResetSlot();
		m_stUICookingListInfo[i].m_pCookingSlotBtn->Show( false );
		m_stUICookingListInfo[i].m_pStaticTitle->Show( false );
		m_stUICookingListInfo[i].m_pStaticLevel->Show( false );
		m_stUICookingListInfo[i].m_pStaticPercent->Show( false );
		m_stUICookingListInfo[i].m_pStaticBackground->Show( false );
		m_stUICookingListInfo[i].m_pStaticLevelText->Show( false );
		m_stUICookingListInfo[i].m_pProgressAchievement->Show( false );
	}

	CManufactureSkill* pCookingSkill = GetLifeSkillCookingTask().GetCookingSkill();
	if( pCookingSkill )
	{
		int nRecipeCount = static_cast<int>( pCookingSkill->GetRecipeCount() );
		m_nMaximumPage = (nRecipeCount - 1) / MAX_COOKING_LIST;
		if( m_nCurrentPage > m_nMaximumPage )
			m_nCurrentPage = m_nMaximumPage;

		std::vector<CSecondarySkillRecipe*>& RecipeList = pCookingSkill->GetList();
		std::vector<CSecondarySkillRecipe*>::iterator iter = RecipeList.begin();

		if( nRecipeCount <= m_nCurrentPage * MAX_COOKING_LIST )
			return;

		iter += m_nCurrentPage * MAX_COOKING_LIST;

		int nCount = 0;
		for( ; iter!=RecipeList.end(); iter++, nCount++ )
		{
			if( nCount >= MAX_COOKING_LIST )
				break;

			TItemInfo itemInfo;
			if( CDnItem::MakeItemInfo( (*iter)->GetItemID(), 1, itemInfo ) )
			{
				CDnItem* pItem = GetItemTask().CreateItem( itemInfo );
				if( pItem )
				{
					m_stUICookingListInfo[nCount].m_pStaticBackground->Show( true );
					m_stUICookingListInfo[nCount].m_pStaticLevelText->Show( true );
					m_stUICookingListInfo[nCount].m_pCookingSlotBtn->SetItem( pItem, CDnSlotButton::ITEM_ORIGINAL_COUNT );
					m_stUICookingListInfo[nCount].m_pCookingSlotBtn->Show( true );

					m_stUICookingListInfo[nCount].m_pStaticTitle->SetText( pItem->GetName() );
					m_stUICookingListInfo[nCount].m_pStaticTitle->Show( true );

					WCHAR wszGrade[64] = {0,};
					int nGradeStringIndex = 0;
					switch( (*iter)->GetRequiredGrade() )
					{
						case SecondarySkill::Grade::Beginning:
							nGradeStringIndex = 7303;
							break;
						case SecondarySkill::Grade::Intermediate:
							nGradeStringIndex = 7304;
							break;
						case SecondarySkill::Grade::Advanced:
							nGradeStringIndex = 7305;
							break;
					}
					swprintf_s( wszGrade, _countof( wszGrade ), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nGradeStringIndex ) );
					WCHAR wszLevel[64] = {0,};
					swprintf_s( wszLevel, _countof( wszLevel ), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7430 ), (*iter)->GetRequiredSkillLevel() );
					WCHAR wszText[128] = {0,};
					swprintf_s( wszText, _countof( wszText ), L"%s %s", wszGrade, wszLevel );
					m_stUICookingListInfo[nCount].m_pStaticLevel->SetText( wszText );
					m_stUICookingListInfo[nCount].m_pStaticLevel->Show( true );

					float fAchievementRate = (( static_cast<float>( (*iter)->GetExp() ) / static_cast<float>( (*iter)->GetMaxExp() ) ) * 100.0f);
					WCHAR wszPercent[64] = {0,};
					WCHAR wszPercentText[64] = {0,};
					swprintf_s( wszPercent, _countof( wszPercent ), L"%.2f", fAchievementRate );
					swprintf_s( wszPercentText, _countof( wszPercentText ), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7307 ), wszPercent );
					m_stUICookingListInfo[nCount].m_pStaticPercent->SetText( wszPercentText );
					m_stUICookingListInfo[nCount].m_pStaticPercent->Show( true );

					m_stUICookingListInfo[nCount].m_pProgressAchievement->SetProgress( fAchievementRate );
					m_stUICookingListInfo[nCount].m_pProgressAchievement->Show( true );
				}
			}
		}
		m_nCurrentPageListCount = nCount;
		SetSelectedRecipeInfo();

		UpdatePageUI();
	}
}

//////////////////////////////////////////////////////////////////////////
// CDnLifeSkillCookingRecipeDlg
//////////////////////////////////////////////////////////////////////////

CDnLifeSkillCookingRecipeDlg::CDnLifeSkillCookingRecipeDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback, true )
, m_pButtonPrevious( NULL )
, m_pButtonNext( NULL )
, m_pStaticPage( NULL )
, m_pButtonAddRecipe( NULL )
, m_nCurrentPage( 0 )
, m_nMaximumPage( 0 )
, m_nCurrentPageListCount( 0 )
, m_nSelectRecipe( -1 )
{
	memset( m_pRecipeSlotBtn, 0, sizeof( m_pRecipeSlotBtn ) );
	memset( m_pRecipeSelect, 0, sizeof( m_pRecipeSelect ) );
}

CDnLifeSkillCookingRecipeDlg::~CDnLifeSkillCookingRecipeDlg()
{
	m_vecRecipeIDList.clear();
}

void CDnLifeSkillCookingRecipeDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "LifeCookSlotDlg.ui" ).c_str(), bShow );
}

void CDnLifeSkillCookingRecipeDlg::InitialUpdate()
{
	char szStr[64];
	for( int i=0; i<MAX_RECIPE_LIST; i++ )
	{
		sprintf_s( szStr, "ID_BT_ITEM%d", i );
		m_pRecipeSlotBtn[i] = GetControl<CDnItemSlotButton>( szStr );
		sprintf_s( szStr, "ID_SELECT%d", i );
		m_pRecipeSelect[i] = GetControl<CEtUIStatic>( szStr );
		m_pRecipeSelect[i]->Show( false );
	}

	m_pButtonPrevious = GetControl<CEtUIButton>( "ID_BT_PRIOR" );
	m_pButtonPrevious->Enable( false );
	m_pButtonNext = GetControl<CEtUIButton>( "ID_BT_NEXT" );
	m_pButtonNext->Enable( false );
	m_pStaticPage = GetControl<CEtUIStatic>( "ID_TEXT_PAGE" );
	m_pButtonAddRecipe = GetControl<CEtUIButton>( "ID_BT_OK" );
	m_pButtonAddRecipe->Enable( false );
}

void CDnLifeSkillCookingRecipeDlg::Show( bool bShow )
{ 
	if( m_bShow == bShow )
		return;

	if( bShow )
		UpdaetRecipeList();

	CEtUIDialog::Show( bShow );
}

void CDnLifeSkillCookingRecipeDlg::UpdatePageUI()
{
	wchar_t wszPage[10]={0};
	swprintf_s( wszPage, 10, L"%d/%d", m_nCurrentPage+1, m_nMaximumPage+1 );
	m_pStaticPage->SetText( wszPage );

	if( m_nCurrentPage == m_nMaximumPage )
		m_pButtonNext->Enable(false);
	else
		m_pButtonNext->Enable(true);

	if( m_nCurrentPage == 0 )
		m_pButtonPrevious->Enable(false);
	else
		m_pButtonPrevious->Enable(true);
}

void CDnLifeSkillCookingRecipeDlg::UpdaetRecipeList()
{
	m_vecRecipeIDList.clear();
	m_pButtonAddRecipe->Enable( false );

	CDnItem* pItem = NULL;
	CDnInventory::INVENTORY_MAP_ITER iter = GetItemTask().GetCharInventory().GetInventoryItemList().begin();
	for( ; iter != GetItemTask().GetCharInventory().GetInventoryItemList().end(); iter++ )
	{
		pItem = iter->second;
		if( !pItem )
			continue;

		if( pItem->GetItemType() == ITEMTYPE_SECONDARYSKILL_RECIPE )
			m_vecRecipeIDList.push_back( pItem );
	}

	m_nMaximumPage = (static_cast<int>( m_vecRecipeIDList.size() ) - 1) / MAX_RECIPE_LIST;
	m_nCurrentPage = 0;
	m_nSelectRecipe = -1;
	
	SetRecipePageList();
}

void CDnLifeSkillCookingRecipeDlg::SetRecipePageList()
{
	for( int i=0; i<MAX_RECIPE_LIST; i++ )
	{
		m_pRecipeSlotBtn[i]->ResetSlot();
		m_pRecipeSlotBtn[i]->Show( false );
		m_pRecipeSelect[i]->Show( false );
	}

	m_nCurrentPageListCount = 0;
	int nRecipeCount = static_cast<int>( m_vecRecipeIDList.size() );
	if( nRecipeCount <= m_nCurrentPage * MAX_RECIPE_LIST )
		return;

	std::vector<CDnItem*>::iterator iter = m_vecRecipeIDList.begin();
	iter += m_nCurrentPage * MAX_RECIPE_LIST;

	int nCount = 0;
	for( ; iter!=m_vecRecipeIDList.end(); iter++, nCount++ )
	{
		if( nCount >= MAX_RECIPE_LIST )
			break;

		TItemInfo itemInfo;
		CDnSecondarySkillRecipeItem* pRecipeItem = static_cast<CDnSecondarySkillRecipeItem*>( (*iter) );
		pRecipeItem->GetTItemInfo( itemInfo );
		CDnItem* pItem = GetItemTask().CreateItem( itemInfo );

		if( pItem )
		{
			m_pRecipeSlotBtn[nCount]->SetItem( pItem, CDnSlotButton::ITEM_ORIGINAL_COUNT );
			m_pRecipeSlotBtn[nCount]->Show( true );
		}
	}

	m_nCurrentPageListCount = nCount;
	UpdatePageUI();
}

void CDnLifeSkillCookingRecipeDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( strstr( pControl->GetControlName(), "ID_BT_PRIOR" ) )
		{
			--m_nCurrentPage;
			if( m_nCurrentPage < 0 )
				m_nCurrentPage = 0;

			m_nSelectRecipe = -1;
			SetRecipePageList();
		}
		else if( strstr( pControl->GetControlName(), "ID_BT_NEXT" ) )
		{
			++m_nCurrentPage;
			if( m_nCurrentPage > m_nMaximumPage )
				m_nCurrentPage = m_nMaximumPage;

			m_nSelectRecipe = -1;
			SetRecipePageList();
		}
		if( strstr( pControl->GetControlName(), "ID_BT_OK" ) )
		{
			std::vector<CDnItem*>::iterator iter = m_vecRecipeIDList.begin();

			int nRecipeCount = static_cast<int>( m_vecRecipeIDList.size() );
			if( nRecipeCount > 0 && nRecipeCount > ( m_nCurrentPage * MAX_RECIPE_LIST ) + m_nSelectRecipe )
			{
				iter += ( m_nCurrentPage * MAX_RECIPE_LIST ) + m_nSelectRecipe;

				// 이미 추가된 레시피인지 확인
				CManufactureSkill* pCookingSkill = GetLifeSkillCookingTask().GetCookingSkill();
				if( pCookingSkill && pCookingSkill->GetRecipe( (*iter)->GetClassID() ) )
					GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7489 ) );
				else
					GetLifeSkillCookingTask().SendAddRecipe( (*iter) );
			}

			Show( false );
		}
		else if( IsCmdControl( "ID_BT_CANCEL" ) || IsCmdControl( "ID_BT_CLOSE" ) )
		{
			Show( false );
		}
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnLifeSkillCookingRecipeDlg::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg )
{
}

bool CDnLifeSkillCookingRecipeDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsShow() )
		return false;

	switch( uMsg )
	{
		case WM_KEYDOWN:
			{
				if( wParam == VK_ESCAPE )
				{
					Show( false );
					return true;
				}
			}
			break;
		case WM_LBUTTONDOWN:
			{
				POINT MousePoint;
				float fMouseX, fMouseY;

				MousePoint.x = short( LOWORD( lParam ) );
				MousePoint.y = short( HIWORD( lParam ) );
				PointToFloat( MousePoint, fMouseX, fMouseY );

				bool bSelect = false;
				int nSelect = 0;
				for( ; nSelect<m_nCurrentPageListCount; nSelect++ )
				{
					SUICoord uiCoordsSelect;
					m_pRecipeSlotBtn[nSelect]->GetUICoord( uiCoordsSelect );
					if( uiCoordsSelect.IsInside( fMouseX, fMouseY ) )
					{
						bSelect = true;
						break;
					}
				}

				if( bSelect && m_nSelectRecipe != nSelect )
				{
					m_nSelectRecipe = nSelect;

					for( int i=0; i<MAX_RECIPE_LIST; i++ )
						m_pRecipeSelect[i]->Show( false );

					m_pRecipeSelect[nSelect]->Show( true );

					std::vector<CDnItem*>::iterator iter = m_vecRecipeIDList.begin();

					int nRecipeCount = static_cast<int>( m_vecRecipeIDList.size() );
					if( nRecipeCount > 0 && nRecipeCount > ( m_nCurrentPage * MAX_RECIPE_LIST ) + m_nSelectRecipe )
					{
						iter += ( m_nCurrentPage * MAX_RECIPE_LIST ) + m_nSelectRecipe;

						DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TSecondarySkillRecipe );
						if( pSox == NULL )
						{
							DN_ASSERT( 0, "Invalid SecondarySkillRecipe" );
							return false;
						}
						SecondarySkill::Grade::eType eRequiredGrade = (SecondarySkill::Grade::eType)pSox->GetFieldFromLablePtr( (*iter)->GetClassID(), "_SecondarySkillClass" )->GetInteger();
						int nRequiredSkillLevel = pSox->GetFieldFromLablePtr( (*iter)->GetClassID(), "_SecondarySkillLevel" )->GetInteger();
						int nSecondarySkillID = pSox->GetFieldFromLablePtr( (*iter)->GetClassID(), "_SecondarySkillID" )->GetInteger();

						CManufactureSkill* pCookingSkill = GetSkillTask().GetLifeSkillRepository().GetManufactureSkill( nSecondarySkillID );
						if( pCookingSkill )
						{
							if( ( eRequiredGrade < pCookingSkill->GetGrade() ) || ( eRequiredGrade == pCookingSkill->GetGrade() && nRequiredSkillLevel <= pCookingSkill->GetLevel() ) )
								m_pButtonAddRecipe->Enable( true );
							else
								m_pButtonAddRecipe->Enable( false );
						}
					}
				}
			}
			break;
	}

	return CEtUIDialog::MsgProc( hWnd, uMsg, wParam, lParam );
}

#endif PRE_ADD_COOKING_SYSTEM