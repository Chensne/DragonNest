
#include "StdAfx.h"
#include "DnNpcQuestRecompenseDlg.h"
#include "TaskManager.h"
#include "DnItemTask.h"
#include "DnInterface.h"
#include "DnCashShopTask.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnNpcQuestRecompenseDlg::CDnNpcQuestRecompenseDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback  )
, m_nSetItemCount( 0 )
, m_nSetAddItemCount( 0 )
, m_pMoneyGold( NULL )
, m_pMoneySilver( NULL )
, m_pStaticComment( NULL )
, m_nTypeRecompense( QuestRecompense_None )
, m_nAvailableCount( 0 )
, m_bRecompense( false )
, m_nRecompenseID( 0 )
, m_pStaticAdd( NULL )
, m_pStaticAddBoard( NULL )
, m_pStaticOnlyAdd( NULL )
, m_pStaticOnlyAddBoard( NULL )
, m_pItemBack(NULL)
, m_pMoneyBack(NULL)
, m_pMoneyBronze(NULL)
, m_pMoneyExp(NULL)
, m_pMoneyExpBack(NULL)
, m_pBoardFTG( NULL )
, m_pTextFTG( NULL )
{
}

CDnNpcQuestRecompenseDlg::~CDnNpcQuestRecompenseDlg(void)
{
	ResetAllData();
}

void CDnNpcQuestRecompenseDlg::Initialize(bool bShow)
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "NpcQuestRecompenseDlg.ui" ).c_str(), bShow );
}

void CDnNpcQuestRecompenseDlg::InitialUpdate()
{
	m_pMoneyGold = GetControl<CEtUIStatic>("ID_MONEY_GOLD");
	m_pMoneySilver = GetControl<CEtUIStatic>("ID_MONEY_SILVER");
	m_pMoneyBronze = GetControl<CEtUIStatic>("ID_MONEY_BRONZE");
	m_pMoneyExp = GetControl<CEtUIStatic>("ID_MONEY_EXP");
	m_pMoneyExpBack = GetControl<CEtUIStatic>("ID_STATIC1");
	m_pMoneyBack = GetControl<CEtUIStatic>("ID_STATIC2");
	m_pStaticComment = GetControl<CEtUIStatic>("ID_STATIC_COMMENT");
	m_pStaticAdd = GetControl<CEtUIStatic>("ID_RIGHT_BONUS");
	m_pStaticAddBoard = GetControl<CEtUIStatic>("ID_RIGHT_BONUSSLOT");
	m_pStaticOnlyAdd = GetControl<CEtUIStatic>("ID_LEFT_BONUS");
	m_pStaticOnlyAddBoard = GetControl<CEtUIStatic>("ID_LEFT_BONUSSLOT");

	m_pBoardFTG = GetControl<CEtUIStatic>("ID_STATIC3");
	m_pTextFTG = GetControl<CEtUIStatic>("ID_STATIC_FTG");

	m_vecMoneyAndExp.push_back( std::make_pair( m_pMoneyGold->GetUICoord(), m_pMoneyGold) );
	m_vecMoneyAndExp.push_back( std::make_pair( m_pMoneySilver->GetUICoord(), m_pMoneySilver) );
	m_vecMoneyAndExp.push_back( std::make_pair( m_pMoneyBronze->GetUICoord(), m_pMoneyBronze) );
	m_vecMoneyAndExp.push_back( std::make_pair( m_pMoneyExp->GetUICoord(), m_pMoneyExp) );
	m_vecMoneyAndExp.push_back( std::make_pair( m_pMoneyExpBack->GetUICoord(), m_pMoneyExpBack) );
	m_vecMoneyAndExp.push_back( std::make_pair( m_pMoneyBack->GetUICoord(), m_pMoneyBack) );
	m_vecMoneyAndExp.push_back( std::make_pair( m_pStaticComment->GetUICoord(), m_pStaticComment) );
	m_vecMoneyAndExp.push_back( std::make_pair( m_pTextFTG->GetUICoord(), m_pTextFTG) );
	m_vecMoneyAndExp.push_back( std::make_pair( m_pBoardFTG->GetUICoord(), m_pBoardFTG) );

	m_pItemBack = GetControl<CEtUIStatic>("ID_STATIC0");

	AddRecompenseItemSlot( "ID_BUTTON_ITEM_%02d", "ID_STATIC_ITEM_%02d", RECOMPENSE_ITEMSLOT_SIZE, m_vecRecompenseItemInfo );
	AddRecompenseItemSlot( "ID_RIGHT_BONUSITEM%d", "ID_RIGHT_BONUSSLOTSELECT%d", MAX_QUEST_LEVEL_CAP_REWARD, m_vecAddRecompenseItemInfo );
	AddRecompenseItemSlot( "ID_LEFT_BONUSITEM%d", "ID_LEFT_BONUSSLOTSELECT%d", MAX_QUEST_LEVEL_CAP_REWARD, m_vecOnlyAddRecompenseItemInfo );
}

void CDnNpcQuestRecompenseDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( strstr( pControl->GetControlName(), "ID_BUTTON_ITEM" ) )
		{
			if( !m_bRecompense )
			{
				return;
			}

			// Note : 전체보상이면 선택할 수 없다.
			//
			if( m_nTypeRecompense == QuestRecompense_All )
			{
				return;
			}

			// Note : 선택된 아이템을 표시한다.
			//
			if( m_nTypeRecompense == QuestRecompense_Select )
			{
				if( GetSlotCheckCount() >= m_nAvailableCount )
				{
					ResetSlotCheck();
				}

				CDnItemSlotButton *pButton = static_cast<CDnItemSlotButton*>(pControl);
				int nIndex = pButton->GetButtonID();

				CDnItem* pItem = m_vecRecompenseItemInfo[nIndex].GetSlot();
				if ( pItem )
					m_vecRecompenseItemInfo[nIndex].SetCheck( true );
			}

			return;
		}
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnNpcQuestRecompenseDlg::Show( bool bShow ) 
{
	if( m_bShow == bShow )
		return;

	if( bShow )
	{
	}
	else
	{
		ResetAllData();
	}

	CEtUIDialog::Show( bShow );
}

void CDnNpcQuestRecompenseDlg::ResetAllData()
{
	m_nTypeRecompense = QuestRecompense_None;
	m_nAvailableCount = 0;
	m_bRecompense = false;
	m_nRecompenseID = 0;

	m_nSetItemCount = 0;
	m_nSetAddItemCount = 0;

	for( int i=0; i<(int)m_vecRecompenseItemInfo.size(); i++ )
		m_vecRecompenseItemInfo[i].ResetSlot();

	for( DWORD itr = 0; itr < m_vecAddRecompenseItemInfo.size(); ++itr )
		m_vecAddRecompenseItemInfo[itr].ResetSlot();

	for( DWORD itr = 0; itr < m_vecOnlyAddRecompenseItemInfo.size(); ++itr )
		m_vecOnlyAddRecompenseItemInfo[itr].ResetSlot();

	m_pStaticAdd->Show( false );
	m_pStaticAddBoard->Show( false );
	m_pStaticOnlyAdd->Show( false );
	m_pStaticOnlyAddBoard->Show( false );

	m_pBoardFTG->Show( false );
	m_pTextFTG->Show( false );
}

void CDnNpcQuestRecompenseDlg::SetPositionCoinUI()
{
	if( 0 == m_nSetItemCount && 0 == m_nSetAddItemCount )
	{
		m_pItemBack->Show( false );
		for each( std::pair< SUICoord, CEtUIStatic* > e in m_vecMoneyAndExp )
		{
			SUICoord Coord = e.first;
			Coord.fY += 0.05f;
			e.second->SetUICoord( Coord );
		}
	}
	else if( 0 == m_nSetItemCount && 0 != m_nSetAddItemCount )
	{
		m_pItemBack->Show( false );
		for each( std::pair< SUICoord, CEtUIStatic* > e in m_vecMoneyAndExp )
			e.second->SetUICoord( e.first );
	}
	else
	{
		m_pItemBack->Show( true );
		for each( std::pair< SUICoord, CEtUIStatic* > e in m_vecMoneyAndExp )
			e.second->SetUICoord( e.first );
	}
}

void CDnNpcQuestRecompenseDlg::SetCoinText( const UINT uiCoin )
{
	int nGold = uiCoin/10000;
	int nSilver = (uiCoin%10000)/100;
	int nBronze = uiCoin%100;

	m_pMoneyGold->SetIntToText( nGold );
	m_pMoneySilver->SetIntToText( nSilver );
	m_pMoneyBronze->SetIntToText( nBronze );
}

void CDnNpcQuestRecompenseDlg::SetExpText( const int nExp )
{
	m_pMoneyExp->SetIntToText( nExp);
}

void CDnNpcQuestRecompenseDlg::SetCommentText( const UINT nStringIndex )
{
	m_pStaticComment->ClearText();
	m_pStaticComment->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nStringIndex ) );
}

void CDnNpcQuestRecompenseDlg::SetItemOption( const int nTableID, const int nCount, OUT TItemInfo & itemInfo )
{
	DNTableFileFormat* pSoxRecompense = GetDNTable( CDnTableDB::TQUESTREWARD );

	if( pSoxRecompense && pSoxRecompense->IsExistItem( nTableID ) )
	{
		int nRecompsendOptionIndex = pSoxRecompense->GetFieldFromLablePtr( nTableID, FormatA("_ItemOptionID%d", nCount+1).c_str() )->GetInteger();

		if(nRecompsendOptionIndex > 0)
		{
			DNTableFileFormat* pSoxRewardOption = GetDNTable( CDnTableDB::TITEMOPTION );

			if( pSoxRewardOption && pSoxRewardOption->IsExistItem(nRecompsendOptionIndex) )
			{
				itemInfo.Item.cLevel = pSoxRewardOption->GetFieldFromLablePtr( nRecompsendOptionIndex , "_Enchant" )->GetInteger();
				itemInfo.Item.cOption = pSoxRewardOption->GetFieldFromLablePtr( nRecompsendOptionIndex , "_Potential1" )->GetInteger();
				itemInfo.Item.cPotential = pSoxRewardOption->GetFieldFromLablePtr( nRecompsendOptionIndex , "Potential2" )->GetInteger();
			}
		}
	}
}

bool CDnNpcQuestRecompenseDlg::GetNormalItemInfo( const int nItemID, const int nItemCount, OUT TItemInfo & itemInfo )
{
	if( false == CDnItem::MakeItemInfo( nItemID, nItemCount, itemInfo ) )
		return false;

	return true;
}

CDnItem * CDnNpcQuestRecompenseDlg::CreateNormalItem( TItemInfo & itemInfo )
{
	CDnItemTask* pTask = static_cast<CDnItemTask*>(CTaskManager::GetInstance().GetTask("ItemTask"));
	if( !pTask )
		return NULL;

	return pTask->CreateItem( itemInfo );
}

bool CDnNpcQuestRecompenseDlg::GetCashItemInfo( const int nCashItemSN, OUT TItemInfo & itemInfo )
{
	if( nCashItemSN == 0 )
		return false;

	const SCashShopItemInfo* pCashInfo = GetCashShopTask().GetItemInfo( nCashItemSN );
	if( !pCashInfo )
		return false;

	if( CDnItem::MakeItemInfo( pCashInfo->presentItemId, pCashInfo->count, itemInfo ) == false )
		return false;

	return true;
}

CDnItem * CDnNpcQuestRecompenseDlg::CreateCashItem( const int nCashItemSN, TItemInfo & itemInfo )
{
	CDnItem * pItem = CreateNormalItem( itemInfo );

	if( NULL == pItem )
		return NULL;

	pItem->SetCashItemSN( nCashItemSN );

	return pItem;
}

bool CDnNpcQuestRecompenseDlg::SetItemSlot( CDnItem * pItem )
{
	if( NULL == pItem )
		return false;

	if ( m_vecRecompenseItemInfo.size() > m_nSetItemCount )
		m_vecRecompenseItemInfo[m_nSetItemCount].SetSlot( pItem );
	else
		return false;

	if( m_bRecompense && m_nTypeRecompense == QuestRecompense_All )
		m_vecRecompenseItemInfo[m_nSetItemCount].SetCheck( true );

	++m_nSetItemCount;

	return true;
}

void CDnNpcQuestRecompenseDlg::SetRecompense( const TQuestRecompense &questRecompense, const bool bRecompense )
{
	ResetAllData();

	m_nTypeRecompense = questRecompense.cType;
	m_nAvailableCount = questRecompense.cSelectMax;
	m_bRecompense = bRecompense;
	m_nRecompenseID = questRecompense.nIndex;

	SetCoinText( questRecompense.nCoin );
	SetExpText( questRecompense.nExp );
	SetCommentText( questRecompense.nStringIndex );
	SetFTGText( questRecompense.nRewardFatigue );

	for( DWORD itr = 0; itr < RECOMPENSE_ITEM_MAX; ++itr )
	{
		TItemInfo itemInfo;
		if( false == GetNormalItemInfo( questRecompense.ItemArray[itr].nItemID, questRecompense.ItemArray[itr].nItemCount, itemInfo ) )
			continue;

		SetItemOption( questRecompense.nIndex, itr, itemInfo );

		CDnItem * pItem = CreateNormalItem( itemInfo );
		if( false == SetItemSlot( pItem ) ) 
			_ASSERT( 0 && "퀘스트보상 : 일반템+캐시템 개수를 합산해서 6개를 초과했습니다. 또는 아이템 생성에 실패 했습니다. 데이터를 확인하세요." );
	}

	m_nRewardCashItemStartSlotIndex = m_nSetItemCount;
	for( DWORD itr = 0; itr < RECOMPENSE_ITEM_MAX; ++itr )
	{
		TItemInfo itemInfo;
		if( false == GetCashItemInfo( questRecompense.CashItemSNArray[itr], itemInfo ) ) 
			continue;

		CDnItem *pItem = CreateCashItem( questRecompense.CashItemSNArray[itr], itemInfo );
		if( false == SetItemSlot( pItem ) ) 
			_ASSERT( 0 && "퀘스트 보상 : 일반템+캐시템 개수를 합산해서 6개를 초과했습니다. 또는 아이템 생성에 실패 했습니다. 데이터를 확인하세요." );
	}

	SetPositionCoinUI();
}

void CDnNpcQuestRecompenseDlg::InitAddRecompenseUI()
{
	if( 0 == m_nSetItemCount )
	{
		m_pStaticOnlyAdd->Show( true );
		m_pStaticOnlyAddBoard->Show( true );
	}
	else
	{
		m_pStaticAdd->Show( true );
		m_pStaticAddBoard->Show( true );
	}
}

bool CDnNpcQuestRecompenseDlg::SetAddItemSlot( CDnItem * pItem, std::vector<SRecompenseItemSlot> & vecItemSlotButton )
{
	if( NULL == pItem )
		return false;

	if ( vecItemSlotButton.size() > m_nSetAddItemCount )
		vecItemSlotButton[m_nSetAddItemCount].SetSlot( pItem );
	else
		return false;

	++m_nSetAddItemCount;

	return true;
}

void CDnNpcQuestRecompenseDlg::SetLevelCapRecompense( const TQuestRecompense & questRecompese, const TQuestLevelCapRecompense & questLevelCapRecompense )
{
	InitAddRecompenseUI();
	SetCommentText( questRecompese.nLevelCapStringIndex );

	for( DWORD itr = 0; itr < MAX_QUEST_LEVEL_CAP_REWARD; ++itr )
	{
		TItemInfo itemInfo;
		if( false == GetNormalItemInfo( questLevelCapRecompense.RecompenseItem[itr].nItemID, questLevelCapRecompense.RecompenseItem[itr].nItemCount, itemInfo ) )
			continue;

		CDnItem * pItem = CreateNormalItem( itemInfo );
		if( false == SetAddItemSlot( pItem, m_nSetItemCount == 0 ? m_vecOnlyAddRecompenseItemInfo : m_vecAddRecompenseItemInfo ) )
			_ASSERT( 0 && "퀘스트 추가 보상 : 일반템+캐시템 개수를 합산해서 6개를 초과했습니다. 또는 아이템 생성에 실패 했습니다. 데이터를 확인하세요." );
	}

	SetPositionCoinUI();
}

void CDnNpcQuestRecompenseDlg::ResetSlotCheck()
{
	for( int i=0; i<(int)m_vecRecompenseItemInfo.size(); i++ )
	{
		m_vecRecompenseItemInfo[i].SetCheck( false );
	}
}

int CDnNpcQuestRecompenseDlg::GetSlotCheckCount()
{
	int nRetCount(0);

	for( int i=0; i<(int)m_vecRecompenseItemInfo.size(); i++ )
	{
		if( m_vecRecompenseItemInfo[i].IsCheckSlot() )
		{
			nRetCount++;
		}
	}

	return nRetCount;
}

bool CDnNpcQuestRecompenseDlg::IsCompleteCheck()
{
	if( m_nTypeRecompense == QuestRecompense_All )
	{
		return true;
	}

	if( m_nTypeRecompense == QuestRecompense_Select )
	{
		if( GetSlotCheckCount() == m_nAvailableCount )
		{
			return true;
		}
	}

	return false;
}

void CDnNpcQuestRecompenseDlg::GetRecompenseItemArray( bool itemArray[], bool cashitemArray[] )
{
	for( int i=0; i<(int)m_vecRecompenseItemInfo.size(); i++ )
	{
		itemArray[i] = cashitemArray[i] = false;

		bool bChecked = m_vecRecompenseItemInfo[i].IsCheckSlot() ? true : false;
		if( i < m_nRewardCashItemStartSlotIndex )
			itemArray[i] = bChecked;
		else
			cashitemArray[i-m_nRewardCashItemStartSlotIndex] = bChecked;
	}
}

void CDnNpcQuestRecompenseDlg::AddRecompenseItemSlot( const char * szButtonName, const char * szStaticName, const int nSize, std::vector<SRecompenseItemSlot> & vecRecompenseItemInfo )
{
	SRecompenseItemSlot recompenseItemSlot;
	char szButton[32]={0,};
	char szStatic[32]={0,};

	for( int itr = 0; itr < nSize; ++itr )
	{
		sprintf_s( szButton, 32, szButtonName, itr );
		sprintf_s( szStatic, 32, szStaticName, itr );

		recompenseItemSlot.pItemButton = GetControl<CDnItemSlotButton>( szButton );
		recompenseItemSlot.pStaticSelected = GetControl<CEtUIStatic>( szStatic );
		recompenseItemSlot.pItemButton->SetSlotType( ST_NPCREWARD );

		recompenseItemSlot.ResetSlot();
		vecRecompenseItemInfo.push_back( recompenseItemSlot );
	}
}

void CDnNpcQuestRecompenseDlg::SetFTGText( const int nFTG )
{
 	if( NULL == m_pBoardFTG || NULL == m_pTextFTG || 0 >= nFTG )
		return;

	m_pBoardFTG->Show( true );
	m_pTextFTG->Show( true );

	m_pTextFTG->SetIntToText( nFTG );
}
