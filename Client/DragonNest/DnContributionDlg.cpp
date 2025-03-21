
#include "StdAfx.h"

#ifdef PRE_ADD_DONATION


#include "EtUIDialogGroup.h"

#include "DnItemTask.h"
#include "DnContributionTask.h"

#include "DnInterface.h"
#include "DnMainMenuDlg.h"
#include "DnContributionGoldDlg.h"			// 골드기부Dlg.
#include "DnContributionRewardDlg.h"		// 기부보상Dlg.
#include "DnContributionRankListItemDlg.h"  // 랭커목록ItemDlg.
#include "DnContributionDlg.h"


#ifdef DEBUG
	#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

namespace NS_Contribution
{
	int g_nHighRankerSize = 3; // 상위랭커 3명.
};

CDnContributionDlg::CDnContributionDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback, bool bAutoCursor )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback, bAutoCursor )
, m_bFirstOpen( true )
, m_pDlgGroup( NULL )
, m_pStaticNameJobLv( NULL )
, m_pStaticRank( NULL )
, m_pStaticGold( NULL )
, m_pListRank( NULL )
, m_pBtnContribute( NULL )
{
}


void CDnContributionDlg::ReleaseDlg()
{
	//m_vecHighRanker.clear();
	SAFE_DELETE( m_pDlgGroup ); // Dialog Group.
}


void CDnContributionDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName("DonationDlg.ui").c_str(), bShow );
}


void CDnContributionDlg::InitialUpdate()
{
	//------------------------------------ 
	// Dialog Group.
	m_pDlgGroup = new CEtUIDialogGroup();
	CEtUIDialog * pChildDlg = NULL;

	// 골드기부Dlg.
	pChildDlg = new CDnContributionGoldDlg( UI_DIALOG_TYPE::UI_TYPE_SELF, this, EDlgGroup::EDlgGroup_GOLD, NULL, true ); 
	pChildDlg->Initialize( false );
	m_pDlgGroup->AddDialog( EDlgGroup::EDlgGroup_GOLD, pChildDlg );

	// 기부보상Dlg.
	pChildDlg = new CDnContributionRewardDlg( UI_DIALOG_TYPE::UI_TYPE_SELF, this, EDlgGroup::EDlgGroup_REWARD, NULL, true );
	pChildDlg->Initialize( false );
	m_pDlgGroup->AddDialog( EDlgGroup::EDlgGroup_REWARD, pChildDlg );


	//------------------------------------

	// LocalPlayer 컨트롤들.
	m_pStaticNameJobLv = GetControl<CEtUIStatic>("ID_TEXT_NAME");
	m_pStaticRank = GetControl<CEtUIStatic>("ID_TEXT_RANK");
	m_pStaticGold = GetControl<CEtUIStatic>("ID_GOLD");
	/*m_pStaticSivler = GetControl<CEtUIStatic>("ID_SILVER");
	m_pStaticCopper = GetControl<CEtUIStatic>("ID_BRONZE");*/

	//// 상위랭커 컨드롤들. UI변경으로 주석처리.
	//char str[32]={0,};
	//m_vecHighRanker.reserve( NS_Contribution::g_nHighRankerSize ); 
	//int size = NS_Contribution::g_nHighRankerSize + 1;
	//for( int i=1; i<size; ++i )
	//{
	//	SHighRankerControls aRanker;
	//	sprintf_s( str, 32, "ID_STATIC_CLASS%d", i );
	//	aRanker.pJobIcon = GetControl<CDnJobIconStatic>(str);

	//	sprintf_s( str, 32, "ID_BT_ITEM%d", i );
	//	aRanker.pItemSlot = GetControl<CDnItemSlotButton>(str);

	//	sprintf_s( str, 32, "ID_TEXT_GUILDNAME%d", i );
	//	aRanker.pStaticGuild = GetControl<CEtUIStatic>(str);

	//	sprintf_s( str, 32, "ID_TEXT_NAME%d", i );
	//	aRanker.pStaticID = GetControl<CEtUIStatic>(str);

	//	sprintf_s( str, 32, "ID_GOLD%d", i );
	//	aRanker.pStaticGold = GetControl<CEtUIStatic>(str);

	//	/*sprintf_s( str, 32, "ID_SILVER%d", i );
	//	aRanker.pStaticSilver = GetControl<CEtUIStatic>(str);

	//	sprintf_s( str, 32, "ID_BRONZE%d", i );
	//	aRanker.pStaticCopper = GetControl<CEtUIStatic>(str);		*/

	//	m_vecHighRanker.push_back( aRanker );
	//}	

	// 7~10위 ListBox.
	m_pListRank = GetControl< CEtUIListBoxEx >("ID_LISTBOXEX_LIST");	
}


void CDnContributionDlg::Show( bool bShow )
{	
	if( m_bShow == bShow )
		return;

	CEtUIDialog::Show( bShow );

	if( bShow )
	{	
		// Dlg 최초 Open.
		if( m_bFirstOpen )
		{
			m_bFirstOpen = false;			

			CDnContributionTask::GetInstance().OnFirstOpenRewardDlg(); // Task에 랭킹보상목록 설정요청.			
		}

		//INVENTORY_DIALOG
		// 랭킹목록갱신패킷 전송.	
		OnSendRefreshRankerList();
	}
	else
	{
		// BlindDlg 닫기.
		GetInterface().CloseBlind();

		// Child Group Dlg 닫기.
		m_pDlgGroup->CloseAllDialog();
	}

}


void CDnContributionDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl( "ID_CLOSE_DIALOG" ) )
			Show( false );

		// 기부하기 Dlg.
		if( IsCmdControl( "ID_BT_DONATION" ) )
			m_pDlgGroup->ShowDialog( EDlgGroup::EDlgGroup_GOLD, true );			

		// 보상보기 Dlg.
		if( IsCmdControl( "ID_BT_REWARD" ) )
			m_pDlgGroup->ShowDialog( EDlgGroup::EDlgGroup_REWARD, true );
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}


// 기부랭킹목록 갱신.
void CDnContributionDlg::RefreshList()
{	
	// Ranker List.
	CDnContributionTask & pTask = CDnContributionTask::GetInstance();
	const std::vector< SContributionRanker * > & vRanker = pTask.GetListRanker();

	wchar_t buf[64]={0,};

	// Ranker List가 없어도 LocalPlayer의 기본정보는 출력.	
	wchar_t * wStr = L"";
	DNTableFileFormat * pTableJob = GetDNTable( CDnTableDB::TJOB );
	int nJobID = CDnActor::s_hLocalActor->GetClassID();
	if( pTableJob && pTableJob->IsExistItem(nJobID) )
	{
		DNTableCell * pCell = pTableJob->GetFieldFromLablePtr( nJobID, "_JobName" );
		if( pCell )
			wStr = const_cast<wchar_t *>( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, pCell->GetInteger() ) );
	}
	// id, job, level.
	swprintf_s( buf, GetEtUIXML().GetUIString(CEtUIXML::idCategory1,812), CDnActor::s_hLocalActor->GetName(), wStr, CDnActor::s_hLocalActor->GetLevel() );
	m_pStaticNameJobLv->SetText( std::wstring(buf) ); 

	
	if( vRanker.empty() )
	{
		std::wstring strEmpty(L"");		
		m_pStaticRank->SetText( strEmpty ); // Rank.	
		m_pStaticGold->SetText( strEmpty ); // Gold.

		return;
	}


	// ListBox 초기화.
	m_pListRank->RemoveAllItems(); 

	// Rank - #55269 [기부시스템] 기부를 전혀 하지 않은 캐릭터의 순위가 10000위로 표시됨. 전혀하지않는 경우에는 순위표시X.
	if( pTask.GetLPRank() == -1 )
		m_pStaticRank->SetText( L"" );
	else
	{
		swprintf_s( buf, 64, GetEtUIXML().GetUIString(CEtUIXML::idCategory1,809), pTask.GetLPRank() );
		m_pStaticRank->SetText( std::wstring(buf) );
	}

	// Gold.
	INT64 nGold = pTask.GetLPGold();
	if( nGold > 0 )
	{		
		m_pStaticGold->SetText( GoldToMoneyString( nGold ) );	
		//m_pStaticGold->SetText( std::wstring(::_i64tow(nGold/10000,buf,10)) );
		/*m_pStaticSivler->SetText( std::wstring(::_i64tow(nGold%10000/100,buf,10)) );
		m_pStaticCopper->SetText( std::wstring(::_i64tow(nGold%100,buf,10)) );*/
	}


	// 1~3위 월별 보상아이템(칭호).
	CDnContributionTask & rTask = CDnContributionTask::GetInstance();	
	const std::vector< SContributionRewardItems > & vecRewardMonth = rTask.GetRewardItemsMonth();

	// 1~10 위.	
	int size = (int)vRanker.size();
	for( int i=0; i<size; ++i )
	{
		//// 상위랭커 1~3 위.
		//if( i < 3 )
		//{
		//	// vecRewardMonth[ i+1 ]  i+1 인 이유는 vecRewardMonth벡터는 랭크별(1~10)로 저장되어있기때문이다.
		//	SetDataHighRanker( vRanker[ i ], m_vecHighRanker[ i ], vecRewardMonth[ i+1 ].nItemID[ 0 ] );
		//}

		//// 하위 7~10 위.
		//else
		//{
		//	CDnContributionRankListItemDlg * pItem = m_pListRank->AddItem< CDnContributionRankListItemDlg >();
		//	pItem->SetData( vRanker[i] );
		//}

		CDnContributionRankListItemDlg * pItem = m_pListRank->AddItem< CDnContributionRankListItemDlg >();
		pItem->SetData( vRanker[i], vecRewardMonth[ i+1 ].nItemID[ 0 ] );
	}
}


// 기부하기 Dialog 갱신 - 현재소지금.
void CDnContributionDlg::RefreshContributionGoldDlg()
{
	CDnContributionGoldDlg * pDlg = static_cast< CDnContributionGoldDlg * >( m_pDlgGroup->GetDialog( EDlgGroup::EDlgGroup_GOLD ) );
	pDlg->SetData();
}


//void CDnContributionDlg::SetDataHighRanker( const struct SContributionRanker * aRanker, SHighRankerControls & aControls, int nItemID )
//{
//	wchar_t buf[64];
//	aControls.pStaticGuild->SetText( aRanker->strGuildName ); 
//	aControls.pStaticID->SetText( aRanker->strID );
//	aControls.pStaticGold->SetText( std::wstring(::_i64tow(aRanker->nGold/10000,buf,10)) );
//	/*aControls.pStaticSilver->SetText( std::wstring(::_i64tow(aRanker->nGold%10000/100,buf,10)) );
//	aControls.pStaticCopper->SetText( std::wstring(::_i64tow(aRanker->nGold%100,buf,10)) );*/
//
//	// 보상아이템.
//	TItemInfo info;
//	if( CDnItem::MakeItemInfo( nItemID, 1, info ) )
//	{
//		CDnItem * pItem = CDnItemTask::GetInstance().CreateItem( info );
//		if( pItem )
//			aControls.pItemSlot->SetItem( pItem, info.Item.wCount );
//	}
//
//	aControls.pJobIcon->SetIconID( aRanker->nJob );
//}


// 랭킹목록갱신패킷 전송.	
void CDnContributionDlg::OnSendRefreshRankerList()
{	
	// 패킷전송.
	CDnContributionTask::GetInstance().OnSendRefreshRankerList();
}


#endif

