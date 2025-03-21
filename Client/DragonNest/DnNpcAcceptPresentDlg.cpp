
#include "StdAfx.h"
#include "DnNpcAcceptPresentDlg.h"
#include "TaskManager.h"
#include "DnItemTask.h"
#include "DnInterface.h"
#include "DnCashShopTask.h"
#include "DnTableDB.h"

#ifdef PRE_ADD_NPC_REPUTATION_SYSTEM

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

const int REPUTE_PRESENT_MONEY_ITEM_ID = 400194;

CDnNpcAcceptPresentDlg::CDnNpcAcceptPresentDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback  )
, m_pMoneyGold(NULL)
, m_pMoneySilver(NULL)
, m_pStaticComment(NULL)
, m_iAvailableCount( 0 ) 
, m_iSelectedSlotIndex( -1 )
, m_pItemBG(NULL)
, m_pMoneyBack(NULL)
, m_pMoneyBronze(NULL)
, m_pMoneyExp(NULL)
, m_pMoneyExpBack(NULL)
{
}

CDnNpcAcceptPresentDlg::~CDnNpcAcceptPresentDlg(void)
{
	DeleteAllItem();
}

void CDnNpcAcceptPresentDlg::Initialize(bool bShow)
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "NpcQuestRecompenseDlg.ui" ).c_str(), bShow );
}

void CDnNpcAcceptPresentDlg::InitialUpdate()
{
	m_pMoneyGold = GetControl<CEtUIStatic>("ID_MONEY_GOLD");
	m_pMoneySilver = GetControl<CEtUIStatic>("ID_MONEY_SILVER");
	m_pMoneyBronze = GetControl<CEtUIStatic>("ID_MONEY_BRONZE");
	m_pMoneyExp = GetControl<CEtUIStatic>("ID_MONEY_EXP");
	m_pMoneyExpBack = GetControl<CEtUIStatic>("ID_STATIC1");
	m_pMoneyBack = GetControl<CEtUIStatic>("ID_STATIC2");
	m_pStaticComment = GetControl<CEtUIStatic>("ID_STATIC_COMMENT");

	m_vlMoneyAndExp.push_back( std::make_pair( m_pMoneyGold->GetUICoord(), m_pMoneyGold) );
	m_vlMoneyAndExp.push_back( std::make_pair( m_pMoneySilver->GetUICoord(), m_pMoneySilver) );
	m_vlMoneyAndExp.push_back( std::make_pair( m_pMoneyBronze->GetUICoord(), m_pMoneyBronze) );
	m_vlMoneyAndExp.push_back( std::make_pair( m_pMoneyExp->GetUICoord(), m_pMoneyExp) );
	m_vlMoneyAndExp.push_back( std::make_pair( m_pMoneyExpBack->GetUICoord(), m_pMoneyExpBack) );
	m_vlMoneyAndExp.push_back( std::make_pair( m_pMoneyBack->GetUICoord(), m_pMoneyBack) );
	m_vlMoneyAndExp.push_back( std::make_pair( m_pStaticComment->GetUICoord(), m_pStaticComment) );

	m_pItemBG = GetControl<CEtUIStatic>("ID_STATIC0");

	SAcceptPresentItemSlot AcceptItemSlot;
	char szButton[ 32 ] = { 0 };
	char szStatic[ 32 ] = { 0 };

	for( int i = 0; i < MAX_ACCEPT_NPC_PRESENT_VARIATION_COUNT; i++ )
	{
		sprintf_s( szButton, 32, "ID_BUTTON_ITEM_%02d", i );
		sprintf_s( szStatic, 32, "ID_STATIC_ITEM_%02d", i );

		AcceptItemSlot.pItemButton = GetControl<CDnItemSlotButton>(szButton);
		AcceptItemSlot.pStaticSelected = GetControl<CEtUIStatic>(szStatic);
		AcceptItemSlot.pItemButton->SetSlotType( ST_NPC_PRESENT );

		AcceptItemSlot.ResetSlot();
		m_vlAcceptPresentItemInfo.push_back( AcceptItemSlot );
	}

	// 경험치 관련 UI 는 사용하지 않으므로 숨긴다.
	m_pMoneyExp->Show( false );
	m_pMoneyExpBack->Show( false );
}

void CDnNpcAcceptPresentDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( strstr( pControl->GetControlName(), "ID_BUTTON_ITEM" ) )
		{
			// Note : 선택된 아이템을 표시한다.
			//

			// 하나밖에 선택이 안되기 때문에 나머지 선택은 풀어준다.
			ResetSlotCheck();

			CDnItemSlotButton* pBtnItemSlot = static_cast<CDnItemSlotButton*>( pControl );

			// 현재 채워지지 않은 아이템 슬롯은 패스
			if( pBtnItemSlot->GetItem() )
			{
				if( pBtnItemSlot->IsEnable() )
				{
					// 돈 아이템을 제외하고 인벤에 없는 아이템은 선택 불가..
					if( REPUTE_PRESENT_MONEY_ITEM_ID == pBtnItemSlot->GetItem()->GetClassID() )
					{
						CDnItemSlotButton *pButton = static_cast<CDnItemSlotButton*>(pControl);
						int nIndex = pButton->GetButtonID();
						int iCoin = m_vlAcceptPresentItemInfo.at( nIndex ).iCoin;

						if( GetItemTask().GetCoin() < iCoin )
						{
							//GetInterface().MessageBox( L"선물할 코인이 모자라서 선물할 수 없습니다. (UIString 필요함)", MB_OK );
							GetInterface().MessageBox( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 3207), MB_OK );
						}
						else
						{
							int iGold = iCoin/10000;
							int iSilver = (iCoin%10000)/100;
							int iBronze = iCoin%100;

							m_pMoneyGold->SetIntToText( iGold );
							m_pMoneySilver->SetIntToText( iSilver );
							m_pMoneyBronze->SetIntToText( iBronze );

							// 선택된 인덱스 셋팅.
							m_iSelectedSlotIndex = nIndex;
							m_vlAcceptPresentItemInfo[ nIndex ].SetCheck( true );
						}
					}
					else
					{
						CDnItemSlotButton *pButton = static_cast<CDnItemSlotButton*>( pControl );

						if( NULL == GetItemTask().GetCharInventory().FindItem( pButton->GetItem()->GetClassID() ) )
						{
							//GetInterface().MessageBox( L"해당 아이템을 가지고 있지 않아서 선물할 수 없습니다. (UIString 필요함)", MB_OK );
							GetInterface().MessageBox( GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 3206), MB_OK );
						}
						else
						{
							int nIndex = pButton->GetButtonID();

							// 선택된 인덱스 셋팅.
							m_iSelectedSlotIndex = nIndex;

							CDnItem* pItem = m_vlAcceptPresentItemInfo[ nIndex ].GetSlot();
							if ( pItem )
								m_vlAcceptPresentItemInfo[ nIndex ].SetCheck( true );
						}
					}
				}
			}

			return;
		}
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnNpcAcceptPresentDlg::Show( bool bShow ) 
{
	if( m_bShow == bShow )
		return;

	if( bShow )
	{
	}
	else
	{
		m_pMoneyGold->ClearText();
		m_pMoneySilver->ClearText();
		m_pStaticComment->ClearText();

		m_iAvailableCount = 0;

		// 숨겨질 때 아이템들 전부 지우면 서버로 보낼 선물 테이블 아이템 ID도 리셋되어 버린다.
		// 어차피 SetPresent 할 때 DeleteAllItem() 호출 하기 때문에 여기선 호출하지 않는다.
		// m_iSelectedSlotIndex = -1; 도 DeleteAllItem() 함수 내부에 있음.
		//DeleteAllItem();
	}

	CEtUIDialog::Show( bShow );
}

void CDnNpcAcceptPresentDlg::DeleteAllItem()
{
	m_iSelectedSlotIndex = -1;
	for( int i=0; i<(int)m_vlAcceptPresentItemInfo.size(); i++ )
	{
		m_vlAcceptPresentItemInfo[i].ResetSlot();
	}
}

void CDnNpcAcceptPresentDlg::SetPresent( int nNpcID )
{
	DeleteAllItem();

	CDnItemTask* pItemTask = static_cast<CDnItemTask*>( CTaskManager::GetInstance().GetTask("ItemTask") );
	if( !pItemTask ) 
		return;

	// npc 호감도 테이블을 뒤져서 이 npc 가 받는 선물이 무엇인지 표시해준다.
	DNTableFileFormat*  pNpcReputationTable = GetDNTable( CDnTableDB::TREPUTE );
	DNTableFileFormat*  pPresentTable = GetDNTable( CDnTableDB::TPRESENT );
	vector<int> vlNpcReputeInfo;
	pNpcReputationTable->GetItemIDListFromField( "_NpcID", nNpcID, vlNpcReputeInfo );

	_ASSERT( 1 == vlNpcReputeInfo.size() );

	// 테이블에서 못 찾으면 여기서 뻑남.
	int iReputeTableID = vlNpcReputeInfo.front();

	// 현재 선물 최대 갯수는 6개임
	bool bItemExist = false;
	char acBuffer[ 256 ] = { 0 };
	for( int i = 0; i < MAX_ACCEPT_NPC_PRESENT_VARIATION_COUNT; ++i )
	{
		sprintf_s( acBuffer, "_PresentID%d", i+1 );
		int iPresentTableItemID = pNpcReputationTable->GetFieldFromLablePtr( iReputeTableID, acBuffer )->GetInteger();

		if( 0 == iPresentTableItemID )
			continue;

		++m_iAvailableCount;

		m_vlAcceptPresentItemInfo.at( i ).SetPresentID( iPresentTableItemID );

		// 아이템인지 코인인지 구분한다.
		// 아이템은 일반 템이던 캐쉬템이던 구분없이 표시해주도록 한다.
		int iPresentType = pPresentTable->GetFieldFromLablePtr( iPresentTableItemID, "_PresentType" )->GetInteger();
		if( 0 == iPresentType ||
			1 == iPresentType )
		{
			int iPresentItemID = pPresentTable->GetFieldFromLablePtr( iPresentTableItemID, "_PresentTypeID" )->GetInteger();
			int iAcceptCount = pPresentTable->GetFieldFromLablePtr( iPresentTableItemID, "_Count" )->GetInteger();

			TItemInfo ItemInfo;
			bool bSuccess = CDnItem::MakeItemInfo( iPresentItemID, iAcceptCount, ItemInfo );
			_ASSERT( bSuccess );
			CDnItem* pItem = pItemTask->CreateItem( ItemInfo );
			if( pItem )
			{
				_ASSERT( i < (int)m_vlAcceptPresentItemInfo.size() );

				m_vlAcceptPresentItemInfo.at( i ).SetSlot( pItem );
				bItemExist = true;
				m_vlAcceptPresentItemInfo.at( i ).pItemButton->SetRenderCount(false);

				// 선물하기 눌렀을 때 안 갖고 있으면 선물 못하게 처리하자.
				// 실제로 가지고 있지 않은 아이템은 선택할 수 없다. 표시해 줌.
				if( 0 == iPresentType )
				{
					std::vector<CDnItem *> invenItemList;
					int count = CDnItemTask::GetInstance().GetCharInventory().FindItemList(iPresentItemID, -1, invenItemList);
					if (count <= 0)
					{
						m_vlAcceptPresentItemInfo.at( i ).pItemButton->SetRegist( true );
					}
					else
					{
						CDnItemSlotButton* pBtn = m_vlAcceptPresentItemInfo.at( i ).pItemButton;
						if (pBtn)
						{
							m_vlAcceptPresentItemInfo.at( i ).SetSlot( pItem, count );
							pBtn->SetRegist(false);
							pBtn->SetRenderCount(true);
						}
					}
				}
				else
				if( 1 == iPresentType )
				{
					std::vector<CDnItem *> invenItemList;
					int count = CDnItemTask::GetInstance().GetCashInventory().FindItemList(iPresentItemID, -1, invenItemList);
					if (count <= 0)
					{
						m_vlAcceptPresentItemInfo.at( i ).pItemButton->SetRegist( true );
					}
					else
					{
						CDnItemSlotButton* pBtn = m_vlAcceptPresentItemInfo.at( i ).pItemButton;
						if (pBtn)
						{
							m_vlAcceptPresentItemInfo.at( i ).SetSlot( pItem, count );
							pBtn->SetRegist(false);
							pBtn->SetRenderCount(true);
						}
					}
				}
			}
		}
		else
		if( 2 == iPresentType )		// 선물 타입이 '돈'임.
		{
			int iCoin = pPresentTable->GetFieldFromLablePtr( iPresentTableItemID, "_PresentTypeID" )->GetInteger();

			//  돈 액수 표시는 돈 아이템 선택했을 때만 표시해준다.
			//int iGold = iCoin/10000;
			//int iSilver = (iCoin%10000)/100;
			//int iBronze = iCoin%100;

			//m_pMoneyGold->SetIntToText( iGold );
			//m_pMoneySilver->SetIntToText( iSilver );
			//m_pMoneyBronze->SetIntToText( iBronze );

			m_vlAcceptPresentItemInfo.at( i ).SetCoin( iCoin );

			// 선물이 돈 타입인 경우 돈 표시용 잡템을 표시해준다. 가지고 있지 않아도 상관 없다.
			// 고유한 아이템 아이디로 처리한다. 돈 표시 아이템 번호가 바뀌면 안된다.
			TItemInfo ItemInfo;
			bool bSuccess = CDnItem::MakeItemInfo( REPUTE_PRESENT_MONEY_ITEM_ID, 1, ItemInfo );
			_ASSERT( bSuccess );
			CDnItem* pItem = pItemTask->CreateItem( ItemInfo );
			m_vlAcceptPresentItemInfo.at( i ).SetSlot( pItem );

			// 선물하기 눌렀을 때 안 갖고 있으면 선물 못하게 처리하자.
			//// 실제로 갖고 있는 돈이 될 때만 enable
			//if( iCoin < GetItemTask().GetCoin() )
			//	m_vlAcceptPresentItemInfo.at( i ).pItemButton->Enable( false );
		}
	}

	if( false == bItemExist )
	{
		m_pItemBG->Show( false );

		for each( std::pair< SUICoord, CEtUIStatic* > e in m_vlMoneyAndExp ) 
		{
			SUICoord Coord = e.first;
			Coord.fY += 0.05f;
			e.second->SetUICoord( Coord );
		}
	}
	else
	{
		m_pItemBG->Show( true );
		
		for each( std::pair< SUICoord, CEtUIStatic* > e in m_vlMoneyAndExp ) 
		{
			e.second->SetUICoord( e.first );
		}
	}
}

void CDnNpcAcceptPresentDlg::ResetSlotCheck()
{
	m_iSelectedSlotIndex = -1;
	for( int i=0; i<(int)m_vlAcceptPresentItemInfo.size(); i++ )
	{
		m_vlAcceptPresentItemInfo[i].SetCheck( false );
	}

	// 돈 표시 했던 것 제거.
	m_pMoneyGold->ClearText();
	m_pMoneySilver->ClearText();
	m_pMoneyBronze->ClearText();
}

int CDnNpcAcceptPresentDlg::GetSelectedPresentID( void )
{
	int iResult = 0;

	if( -1 != m_iSelectedSlotIndex )
	{
		const SAcceptPresentItemSlot& AcceptPresentSlot = m_vlAcceptPresentItemInfo.at( m_iSelectedSlotIndex );
		iResult = AcceptPresentSlot.iPresentID;
	}

	return iResult;
}

//int CDnNpcAcceptPresentDlg::GetSlotCheckCount()
//{
//	int nRetCount(0);
//
//	for( int i=0; i<(int)m_vlAcceptPresentItemInfo.size(); i++ )
//	{
//		if( m_vlAcceptPresentItemInfo[i].IsCheckSlot() )
//		{
//			nRetCount++;
//		}
//	}
//
//	return nRetCount;
//}

CDnSlotButton* CDnNpcAcceptPresentDlg::GetSelectedPresentSlotButton() const
{
	int iResult = 0;

	if (m_iSelectedSlotIndex != -1)
	{
		const SAcceptPresentItemSlot& AcceptPresentSlot = m_vlAcceptPresentItemInfo.at(m_iSelectedSlotIndex);
		return AcceptPresentSlot.pItemButton;
	}

	return NULL;
}
#endif // #ifdef PRE_ADD_NPC_REPUTATION_SYSTEM