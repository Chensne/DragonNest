#include "Stdafx.h"

#ifdef PRE_ADD_PVP_RANKING

#include "DnTableDB.h"
#include "DNTableFile.h"
#include "PvPSendPacket.h"

#include "DnGuildTask.h"
#include "DnPlayerActor.h"

#include "DnInterface.h"
#include "DnMainMenuDlg.h"
#include "DnMainFrame.h"
#include "DnPvPRankListItemDlg.h"
#include "DnPvPRankListItemLadderDlg.h"
#include "DnInterfaceString.h"
#include "DnPvPRankDlg.h"

#ifdef PRE_ADD_DWC
#include "DnDWCTask.h"
#include "DnPvPRankListItemDwcDlg.h"
#include "VillageSendPacket.h"
#endif // PRE_ADD_DWC

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

CDnPvPRankDlg::CDnPvPRankDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback, bool bAutoCursor )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback, bAutoCursor )
, m_pListRank(NULL)
, m_pTreeJob(NULL)
, m_pComboCondition(NULL)
, m_pBtnSearch(NULL)
, m_pBtnColosseum(NULL)
, m_pBtnLadder(NULL)
, m_pIMEEditBox(NULL)
, m_pListColosseum(NULL)
, m_pListLadder(NULL)
, m_pStaticSubTitle(NULL)
, m_pJobIcon(NULL)
, m_pTextureControl(NULL)
, m_pTextureControlGuild(NULL)
, m_CrrPage(1) // 현재출력 Page.
, m_RequestPage(1) // 요청 Page.
, m_Job(0)
, m_SubClass(0)
, m_StrSearch(NULL)
, m_RankType(ERankType::ERankType_None)
, m_crrSelectPage(0)
, m_crrMouseOverPage(0)
, m_totScrollPage(1)
, m_bScroll(false)
, m_pStaticBack(NULL)
, m_pStaticNext(NULL)
, m_pStaticFirst(NULL)
, m_pStaticLast(NULL)
, m_eCurrentLadderType( LadderSystem::MatchType::None )
, m_pStaticFirstRank( NULL )
, m_pStaticGuild( NULL )
, m_pStaticMyPoint( NULL )
, m_pStaticMyResult( NULL )
, m_pStaticName( NULL )
, m_pStaticPoint( NULL )
, m_pStaticResult( NULL )
, m_pStaticSecondRank( NULL )
, m_pStaticTotRank( NULL )
#ifdef PRE_ADD_DWC
, m_pListDWC(NULL)
#endif
#ifdef PRE_ADD_PVPRANK_INFORM
, m_pBtnRankInform(NULL)
#endif
{

}

void CDnPvPRankDlg::ReleaseDlg()
{
	m_vecPages.clear();
	SAFE_DELETE( m_pListColosseum );
	SAFE_DELETE( m_pListLadder );
#ifdef PRE_ADD_DWC
	SAFE_DELETE( m_pListDWC );
#endif // PRE_ADD_DWC

	m_mapParentJobBySub.clear();
	m_mapParentJob.clear();

	if( m_pTreeJob )
		m_pTreeJob->DeleteAllItems();
}

void CDnPvPRankDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName("RankBoardDlg.ui").c_str(), bShow );
}

void CDnPvPRankDlg::InitialUpdate()
{
	m_pStaticBack = GetControl<CEtUIButton>("ID_BT_BACK"); // <
	m_pStaticNext = GetControl<CEtUIButton>("ID_BT_NEXT"); // >
	m_pStaticFirst = GetControl<CEtUIButton>("ID_BT_FIRST"); // <<
	m_pStaticLast = GetControl<CEtUIButton>("ID_BT_LAST");   // >>

	m_pBtnColosseum = GetControl<CEtUIRadioButton>("ID_RBT0");
	m_pBtnColosseum->SetDisableTime( 1.0f );
	m_pBtnColosseum->SetChecked( true );

	m_pBtnSearch = GetControl<CEtUIButton>("ID_BT_SEARCH");
	m_pBtnSearch->SetDisableTime( 1.0f );
	
	m_pBtnLadder = GetControl<CEtUIRadioButton>("ID_RBT1");
	m_pBtnLadder->SetDisableTime( 1.0f );

	m_pTreeJob = GetControl<CEtUITreeCtl>("ID_TREE_LIST");

	m_pComboCondition = GetControl<CEtUIComboBox>("ID_COMBOBOX_TITLE");
	m_pComboCondition->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 5001 ), NULL, (int)EComboCondition::EComboCondition_Character, true ); // "캐릭터명"
#ifdef PRE_ADD_DWC
	bool bIsDWCChar = GetDWCTask().IsDWCChar();	
	CEtUIButton* pBtn = GetControl<CEtUIButton>("ID_BT_RANKINFO");
	if(pBtn) pBtn->Show(!bIsDWCChar);
	if(bIsDWCChar == false)
		m_pComboCondition->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 121050 ), NULL, (int)EComboCondition::EComboCondition_Guild, true ); // "길드명"		
#else
	m_pComboCondition->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 121050 ), NULL, (int)EComboCondition::EComboCondition_Guild, true ); // "길드명"		
#endif

#ifdef PRE_ADD_PVPRANK_INFORM
	m_pBtnRankInform = GetControl<CEtUIButton>("ID_BT_RANKINFO");
#endif

	m_pListRank = GetControl<CEtUIListBoxEx>("ID_LISTBOXEX_LIST");

	m_pIMEEditBox = GetControl<CEtUIIMEEditBox>("ID_IMEEDITBOX_NAME");

	m_pStaticMyPoint = GetControl<CEtUIStatic>("ID_TEXT0");  // 평점.
	m_pStaticMyResult = GetControl<CEtUIStatic>("ID_TEXT1"); // 결과.

	m_pStaticTotRank = GetControl<CEtUIStatic>("ID_TEXT_MYRANK0"); // 전체순위.
	m_pStaticFirstRank = GetControl<CEtUIStatic>("ID_TEXT_MYRANK1"); // 1차직업순위.
	m_pStaticSecondRank = GetControl<CEtUIStatic>("ID_TEXT_MYRANK2"); // 2차직업순위.
	m_pStaticGuild = GetControl<CEtUIStatic>("ID_TEXT_GUILD");   // 길드명.
	m_pStaticName = GetControl<CEtUIStatic>("ID_TEXT_NAME");    // 캐릭명.
	m_pStaticPoint = GetControl<CEtUIStatic>("ID_TEXT_COUNT");   // 포인트.
	m_pStaticResult = GetControl<CEtUIStatic>("ID_TEXT_RESULT");  // 결과.
	m_pTextureControl = GetControl<CEtUITextureControl>("ID_TEXTUREL_RANK"); // 콜로세움등급아이콘.
	m_pTextureControlGuild = GetControl<CEtUITextureControl>("ID_TEXTUREL_GUILD"); // 길드아이콘.
	m_pTextureControlGuild->Show( false );
	m_pStaticSubTitle = GetControl<CEtUIStatic>("ID_TEXT_SUBTITLE");
	m_pStaticSubTitle->SetText( GetSubString( ERankType::ERankType_Colosseum ) );

	m_pJobIcon = GetControl<CDnJobIconStatic>("ID_STATIC_CLASS");

	char str[256] = {0,};
	m_vecPages.resize( RankingSystem::RANKINGPAGECOUNT );
	for( int i=0; i<RankingSystem::RANKINGPAGECOUNT; ++i )
	{
		sprintf_s( str, 256, "ID_TEXT_PAGE%d", i+1 );
		m_vecPages[ i ] = GetControl<CEtUIStatic>( str );		
		m_vecPages[ i ]->SetButton( true );
	}

	int size = PVPTYPESIZE;
	for( int i=0; i<size; ++i )
	{	
		sprintf_s( str, 256, "ID_STATIC_RESULTTAB%d", i );
		m_arrControls[ i ].pResultTab = GetControl<CEtUIStatic>(str);

		sprintf_s( str, 256, "ID_STATIC_POINTTAB%d", i );
		m_arrControls[ i ].pPointTab = GetControl<CEtUIStatic>(str);

		sprintf_s( str, 256, "ID_TEXT_RESULTTAB%d", i );
		m_arrControls[ i ].pResultText = GetControl<CEtUIStatic>(str);
		
		sprintf_s( str, 256, "ID_TEXT_POINTTAB%d", i );
		m_arrControls[ i ].pPointText = GetControl<CEtUIStatic>(str);
	}

	for( int i=0; i<LadderSystem::MatchType::MaxMatchType; i++ )
	{
		if( i > LadderSystem::MatchType::None )
		{
			CEtUIRadioButton *pRadioButton = GetControl<CEtUIRadioButton>( FormatA( "ID_RBT%d" , i ).c_str() );
			if( pRadioButton )
			{
#if defined( PRE_FIX_TEAM_LADDER_3vs3 )
				if( i <= LadderSystem::MatchType::MaxCount )
				{
					pRadioButton->Show( i <= LadderSystem::MatchType::_2vs2 );

					if( i== LadderSystem::MatchType::_2vs2 )
						pRadioButton->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 126028 ) ); // 3vs3 Ladder 
				}
				else
				{
					pRadioButton->Show( false );
				}
#else
				pRadioButton->Show( i <= LadderSystem::MatchType::MaxCount );
#endif
			}
		}
	}
}

std::wstring CDnPvPRankDlg::GetSubString( ERankType type )
{
	std::wstring str;
	if( type == ERankType::ERankType_Colosseum )
		str.assign( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2419 ) ); // "콜로세움"
	else if( type == ERankType::ERankType_Ladder )
		str.assign( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 126040 ) ); // "래더"

	str.append( L" " );
	str.append( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 126095 ) ); // "순위"

	return str;
}

void CDnPvPRankDlg::Show( bool bShow )
{
 	if( m_bShow == bShow )
		return;

	CEtUIDialog::Show( bShow );

	// Open.
	if( bShow )
	{
#if defined(PRE_ADD_DWC)
		if(GetDWCTask().IsDWCChar())
		{
			m_RankType = ERankType::ERankType_DWC;
			ChangeStaticControlsState();
			SendDWCRankList(m_RequestPage, DWC::Common::RankPageMaxSize);
		}
		else
		{
			BuildTreeJob();  // 직업별 TreeControl 구축.
			m_pTreeJob->SetSelectItem( m_pTreeJob->GetBeginItem() );
			RequestMyInfo( ERankType::ERankType_Colosseum );
		}
#else 
		BuildTreeJob();  // 직업별 TreeControl 구축.
		m_pTreeJob->SetSelectItem( m_pTreeJob->GetBeginItem() );
		RequestMyInfo( ERankType::ERankType_Colosseum ); // 내정보요청.
#endif // PRE_ADD_DWC
	}
	// Close.
	else
	{
		Reset();

		m_crrSelectPage = m_crrMouseOverPage = 0;		
		m_RankType = ERankType::ERankType_None;
		m_pBtnLadder->SetChecked( false );
		m_pBtnColosseum->SetChecked( true );
		m_bScroll = false;		

		if( m_pTreeJob )
			m_pTreeJob->ResetSelectedItem();
	}

	if (m_pCallback)
		m_pCallback->OnUICallbackProc( CDnMainMenuDlg::PVP_RANK_DIALOG, bShow ? EVENT_DIALOG_SHOW : EVENT_DIALOG_HIDE, NULL);

	
	// 화면 우측 알리미창들 - 미션,퀘스트,제작 등..
	GetInterface().OpenRightAlramDlg( bShow );
}

bool CDnPvPRankDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsShow() ) {
		return false;
	}
	/*if( uMsg == WM_LBUTTONDOWN )
	{
		float fMouseX, fMouseY;
		POINT MousePoint;
		MousePoint.x = short( LOWORD( lParam ) );
		MousePoint.y = short( HIWORD( lParam ) );
		PointToFloat( MousePoint, fMouseX, fMouseY );

		if( CONTROL( IMEEditBox, ID_IMEEDITBOX_SEARCHWORD )->IsInside( fMouseX, fMouseY ) )
		{
			std::wstring curText = CONTROL( IMEEditBox, ID_IMEEDITBOX_SEARCHWORD )->GetText();
			OnSetSearchItemList( curText );
		}
	}*/

	if( m_pComboCondition && !m_pComboCondition->IsOpenedDropDownBox() )
	{
		switch( uMsg )
		{
			case WM_KEYDOWN:
				{
					if( wParam == VK_RETURN )
					{
						if(m_pIMEEditBox->IsFocus())
						{
							this->ProcessCommand(EVENT_BUTTON_CLICKED, true, GetControl("ID_BT_SEARCH"), 0);
						}
						return true;
					}
				}
				break;

			case WM_MOUSEWHEEL:
				{
					POINT MousePoint;
					MousePoint.x = short( LOWORD( lParam ) );
					MousePoint.y = short( HIWORD( lParam ) );

					ScreenToClient( hWnd, &MousePoint );
					lParam = MAKELPARAM( MousePoint.x, MousePoint.y );

					if( IsMouseInDlg() )
					{
						UINT uLines;
						SystemParametersInfo( SPI_GETWHEELSCROLLLINES, 0, &uLines, 0 );
						int nScrollAmount = int( ( short )HIWORD( wParam ) ) / WHEEL_DELTA * uLines;
						if( nScrollAmount > 0 )
							ProcessCommand( EVENT_BUTTON_CLICKED, false, GetControl<CEtUIButton>("ID_BT_BACK"), 0 );
						else if( nScrollAmount < 0 )
							ProcessCommand( EVENT_BUTTON_CLICKED, false, GetControl<CEtUIButton>("ID_BT_NEXT"), 0 );

						CEtUIDialogBase::StaticMsgProc( CDnMainFrame::GetInstance().GetHWnd(), WM_MOUSEMOVE, 0, (LPARAM)lParam );
						return true;
					}
				}
				break;

			case WM_MOUSEMOVE:
				{				
					int size = (int)m_vecPages.size();
					for( int i=0; i<size; ++i )
					{
						if( m_vecPages[ i ]->IsMouseEnter() )
						{
							m_crrMouseOverPage = i;
							break;
						}	
					}				
				}
				break;
		}
	}

	return CEtUIDialog::MsgProc( hWnd, uMsg, wParam, lParam );
}

void CDnPvPRankDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl * pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	//// TEST.
	//if( nCommand == EVENT_TREECONTROL_SELECTION )
	//{
	//	if( IsCmdControl("ID_TREE_LIST") )
	//	{
	//		CTreeItem * pItem = m_pTreeJob->GetSelectedItem();
	//		if( pItem )
	//		{
	//			WCHAR str[256]={0,};
	//			_itow( pItem->GetItemValueInt(), str, 10 );
	//			GetControl<CEtUIStatic>("ID_TEXT_GUILD")->SetText( str );
	//		}
	//	}
	//}

	if( nCommand == EVENT_BUTTON_CLICKED )
	{			
		// 검색.
		if( IsCmdControl("ID_BT_SEARCH") )
			SelectionTreeJob();

		// 닫기.
		else if( IsCmdControl("ID_BT_CLOSE") )
			Show( false );
		
		// <.
		else if( IsCmdControl("ID_BT_BACK") )
		{
			m_CrrPage -= 1;
			PageChange();
		}
		// >.
		else if( IsCmdControl("ID_BT_NEXT") )
		{
			m_CrrPage += 1;
			PageChange();
		}
		// <<.
		else if( IsCmdControl("ID_BT_FIRST") )
		{
			m_bScroll = true;
			PageScroll( -1 );
		}
		// >>.
		else if( IsCmdControl("ID_BT_LAST") )
		{
			m_bScroll = true;
			PageScroll( 1 );
		}
		else if( strstr( pControl->GetControlName(), "ID_TEXT_PAGE" ) != NULL )
		{
			int nIndex = -1;
			if( sscanf( pControl->GetControlName() + strlen("ID_TEXT_PAGE"), "%d", &nIndex) == 1 )
			{	
				if( m_RequestPage > 1 )
				{
					int denominator = ( m_RequestPage - 1 ) * RankingSystem::RANKINGPAGECOUNT;
					int crrSelectionPage = ( m_CrrPage - 1 ) / denominator;
					crrSelectionPage += ( m_CrrPage - 1 ) % denominator;
					if( nIndex != crrSelectionPage )
					{
						m_CrrPage += ( nIndex - crrSelectionPage );
						PageChange();
					}
				}
				else
				{
					if( m_CrrPage != nIndex )
					{	
						m_CrrPage += ( nIndex - m_CrrPage );
						PageChange();
					}
				}
								
			}
		}
#ifdef PRE_ADD_PVPRANK_INFORM
		else if (IsCmdControl("ID_BT_RANKINFO"))
		{
			GetInterface().ShowPVPRankInformDlg(true);
		}
#endif
	}

	else if( nCommand == EVENT_RADIOBUTTON_CHANGED )
	{
		if( IsShow() && m_pListRank )
		{
			// 콜로세움
#if defined(PRE_ADD_DWC)
			if( IsCmdControl("ID_RBT0") )
			{
				if(GetDWCTask().IsDWCChar())
					SendDWCRankList(m_RequestPage, DWC::Common::RankPageMaxSize);
				else
					RequestMyInfo( ERankType::ERankType_Colosseum ); // 내정보요청.
			}
#else
			if( IsCmdControl("ID_RBT0") )
				RequestMyInfo( ERankType::ERankType_Colosseum ); // 내정보요청.
#endif // PRE_ADD_DWC
			
			// 1:1래더
			else if( IsCmdControl("ID_RBT1") )
				RequestMyInfo( ERankType::ERankType_Ladder, LadderSystem::MatchType::_1vs1 ); // 내정보요청.

#if defined(PRE_FIX_TEAM_LADDER_3vs3)
			// 3:3래더
			else if( IsCmdControl("ID_RBT2") )
				RequestMyInfo( ERankType::ERankType_Ladder, LadderSystem::MatchType::_3vs3 ); // 내정보요청.
#else
			// 2:2래더
			else if( IsCmdControl("ID_RBT2") )
				RequestMyInfo( ERankType::ERankType_Ladder, LadderSystem::MatchType::_2vs2 ); // 내정보요청.
#endif

			// 3:3래더
			else if( IsCmdControl("ID_RBT3") )
				RequestMyInfo( ERankType::ERankType_Ladder, LadderSystem::MatchType::_3vs3 ); // 내정보요청.

		}
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

// 직업별 TreeControl 구축.
void CDnPvPRankDlg::BuildTreeJob()
{	
	if( m_pTreeJob->GetExpandedItemCount() > 0 )
		return;
	
	DNTableFileFormat * pTable = GetDNTable( CDnTableDB::TJOB );
	if( !pTable )
		return;

	DNTableCell * pCell = NULL;
		
	std::map< int, CTreeItem * >::iterator itFind;
	
	CTreeItem * pTreeItem = m_pTreeJob->AddItem( CTreeItem::typeOpen, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 601 ) ); // "전체"
	
	int nSize = pTable->GetItemCount();
	for( int i=0; i<nSize; ++i )
	{
		int nID = pTable->GetItemID( i );
		
		// 플레이어 직업이 아님.
		pCell = pTable->GetFieldFromLablePtr( nID, "_Class" );
		if( pCell == NULL ||  pCell->GetInteger() == 0 )
			continue;
		
#ifdef PRE_FIX_PVPRANK_NOSERVICE_JOB
		// 현재추가되지 않은 직업.
		pCell = pTable->GetFieldFromLablePtr( nID, "_Service" );
		if( pCell == NULL || pCell->GetInteger() == 0 )
			continue;
#endif

		int nClass = pCell->GetInteger();

		pCell = pTable->GetFieldFromLablePtr( nID, "_JobNumber" );
		if( !pCell )
			continue;

		// 대분류.
		if( pCell->GetInteger() == 0 )
		{
			pCell = pTable->GetFieldFromLablePtr( nID, "_JobName" );
			if( pCell )
			{
				pTreeItem = m_pTreeJob->AddItem( CTreeItem::typeOpen, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, pCell->GetInteger() ) );	
				pTreeItem->SetItemValueInt( nID );
				m_mapParentJob.insert( std::map< int, CTreeItem * >::value_type( nID, pTreeItem ) ); 			
			}
		}

		// 소분류 - 1차직업까지만.
		else if( pCell->GetInteger() == 1 )
		{
#ifdef PRE_FIX_PVPRANK_NOSERVICE_JOB
#else
			// 현재추가되지 않은 직업.
			pCell = pTable->GetFieldFromLablePtr( nID, "_Service" );
			if( !pCell || ( pCell && pCell->GetInteger() == 0) )
				continue;
#endif

			pCell = pTable->GetFieldFromLablePtr( nID, "_ParentJob" );
			if( pCell )
			{
				itFind = m_mapParentJob.find( pCell->GetInteger() );
				if( itFind != m_mapParentJob.end() )
				{
					// 서브클래스의 직종.
					m_mapParentJobBySub.insert( std::map< int, int >::value_type( nID, nClass ) );

					pCell = pTable->GetFieldFromLablePtr( nID, "_JobName" );
					if( pCell )
					{
						pTreeItem = m_pTreeJob->AddChildItem( itFind->second, CTreeItem::typeOpen, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, pCell->GetInteger() ) );
						pTreeItem->SetItemValueInt( nID );
					}
				}				
			}
		}
	}
}

// 직업선택.
void CDnPvPRankDlg::SelectionTreeJob()
{
	m_StrSearch = const_cast< wchar_t * >( m_pIMEEditBox->GetText() );

	// ComboBox.
	EComboCondition comboCondition = EComboCondition::EComboCondition_NONE;
	SComboBoxItem * pComboItem = m_pComboCondition->GetSelectedItem();
	if( pComboItem )
		comboCondition = (EComboCondition)pComboItem->nValue;

	// 캐릭터명 검색시 문자열검사.
	int strSize = m_pIMEEditBox->GetTextLength() + CEtUIIME::GetCompStringLength();
	if( comboCondition == EComboCondition::EComboCondition_Character && strSize > 0 && strSize < CHARNAMEMIN )
	{		
		GetInterface().MessageBox( FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 100303 ), CHARNAMEMIN ).c_str() ); // "캐릭터 이름은 %d자 이상이어야 합니다."
		return;
	}

	// Page초기화.
	m_CrrPage = m_RequestPage = 1;

	// #68604 - 검색버튼누를시에 트리컨트롤 선택이되어 있지않으면 첫번째아이템으로 지정.
	if( m_pTreeJob->GetSelectedItem() == NULL )
		m_pTreeJob->SetSelectItem( m_pTreeJob->GetBeginItem() );


	// 검색조건이 없는 경우 - 직업별검색( 길드명 검색도 같은 처리 ).
	if( m_StrSearch == NULL || strSize < 1 ||
		EComboCondition::EComboCondition_Guild == comboCondition )
	{
		CTreeItem * pSelectedItem = m_pTreeJob->GetSelectedItem();
		if( !pSelectedItem )
		{
#ifdef PRE_ADD_DWC
			SendDWCRankList(1, DWC::Common::RankPageMaxSize);
#endif
			return;
		}

		m_SubClass = 0;
		m_Job = pSelectedItem->GetItemValueInt();			
		std::map< int, CTreeItem * >::iterator itFind;
		itFind = m_mapParentJob.find( m_Job );
	
		// 서브Job.
		if( itFind == m_mapParentJob.end() )
		{
			m_SubClass = m_Job;

			// 해당 SubClass의 직종.
			//std::map< int, int >::iterator it = m_mapParentJobBySub.find( m_SubClass );
			//if( it != m_mapParentJobBySub.end() )
			//	m_Job = it->second;

			m_Job = 0;
		}

		//if( m_Job == m_SubClass )
		//	m_SubClass = 0;

		// #68497 - 길드검색시 직업카테고리 무시( 전체길드원 검색됨 )
		if( m_StrSearch != NULL && strSize > 0 )
		{
			m_Job = m_SubClass = 0;
			m_pTreeJob->SetSelectItem( m_pTreeJob->GetBeginItem() );
		}

		// Send.
		if( m_RankType == ERankType::ERankType_Colosseum )
			OnSendPvPRankList( m_RequestPage, m_Job, m_SubClass, ( m_StrSearch == NULL ? L"" : m_StrSearch ) );
		else if( m_RankType == ERankType::ERankType_Ladder )
			OnSendPvPRankLadderList( m_eCurrentLadderType, m_RequestPage, m_Job, m_SubClass, ( m_StrSearch == NULL ? L"" : m_StrSearch ) );
		
		return;
	}

	// 검색조건이 있는 경우 - 캐릭터명 검색.	
	else if( EComboCondition::EComboCondition_Character == comboCondition )
	{	
		// Send.
		if( m_RankType == ERankType::ERankType_Colosseum )
			OnSendPvPRankCharacter( m_StrSearch );
		else if( m_RankType == ERankType::ERankType_Ladder )
			OnSendPvPRankLadderCharacter( m_eCurrentLadderType, m_StrSearch );
#ifdef PRE_ADD_DWC
		else if( m_RankType == ERankType::ERankType_DWC )
		{
			std::wstring wszRemakeStr = L"DWCGM_";
			wszRemakeStr.append(m_StrSearch);
			SendDWCFindRank( wszRemakeStr.c_str() );
		}
#endif
		m_pTreeJob->SetSelectItem( m_pTreeJob->GetBeginItem() );
		return;	
	}
}

// 내정보요청.
void CDnPvPRankDlg::RequestMyInfo( ERankType rank , LadderSystem::MatchType::eCode eMatchType )
{
	if( m_RankType == rank )
	{
		if( rank == ERankType::ERankType_Ladder )
		{
			if( m_eCurrentLadderType == eMatchType )
				return;
		}
		else
		{
			return;
		}
	}

	// 이전정보리셋.
	Reset();

	m_RankType = rank;	

	// SubTitle 변경.
	m_pStaticSubTitle->SetText( GetSubString( rank ) );

	// List Header 변경.
	int size = PVPTYPESIZE;
	for( int i=0; i<size; ++i )
	{	
		bool bShow = ( i==(int)rank ) ? true : false;

		m_arrControls[ i ].pResultTab->Show( bShow );		
		m_arrControls[ i ].pPointTab->Show( bShow );		
		m_arrControls[ i ].pResultText->Show( bShow );		
		m_arrControls[ i ].pPointText->Show( bShow );
	}

	// Send.
	if( rank == ERankType::ERankType_Colosseum )
	{		
		OnSendPvPRankMyInfo();
		OnSendPvPRankList( m_RequestPage, m_Job, m_SubClass, ( m_StrSearch == NULL ? L"" : m_StrSearch ) );
	}

	else if( rank == ERankType::ERankType_Ladder )
	{
		OnSendPvPLadderRankMyInfo( eMatchType );
		OnSendPvPRankLadderList( eMatchType , m_RequestPage, m_Job, m_SubClass, ( m_StrSearch == NULL ? L"" : m_StrSearch ) );
	}	
}

// 내정보 - 콜로세움.
void CDnPvPRankDlg::SetInfoMyRankColosseum( struct TPvPRankingDetail * pInfo )
{
	m_totScrollPage = 1;

	wchar_t str[256] = {0,};

	if( pInfo->biRank > 0 )
	{
		swprintf_s( str, 256, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 120203 ), pInfo->biRank ); // "전체순위 %d위"
		m_pStaticTotRank->SetText( str );	
	}
	else
		m_pStaticTotRank->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 120210 ) ); // "전적없음"
		
	int job = pInfo->cJobCode;	

	// 콜로세움등급 아이콘.
	if( pInfo->cPvPLevel > 0 )
	{
		m_pTextureControl->Show( true );
		GetInterface().SetPvPClassIcon( m_pTextureControl, pInfo->cPvPLevel );
	}
	else
		m_pTextureControl->Show( false );

	m_pStaticMyPoint->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 120206 ) );// "PVP포인트"
	m_pStaticMyResult->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 120205 ) ); // "킬/데스"

	if( pInfo->wszCharName != NULL && ( wcslen( pInfo->wszCharName ) > 1 ) )
	{
		std::wstring strName( pInfo->wszCharName );

		// 직업아이콘.
		LPCWSTR strJob = DN_INTERFACE::STRING::GetJobString( job );
		m_pJobIcon->Show( true );
		m_pJobIcon->SetIconID( job );
		m_pJobIcon->SetTooltipText( strJob );

		// 이름 (직업 LVxx).
		strName.append( L" (" );
		strName.append( strJob );
		swprintf_s( str, 256, GetUIString( CEtUIXML::idCategory1, 2020022 ), pInfo->iLevel ); // "Lv%d"
		strName.append( str );
		strName.append( L")" );
		m_pStaticName->SetText( strName );

		// 길드마크.		
		SetGuildIcon(); 
	}
	else
	{
		m_pTextureControlGuild->Show( false );
		m_pJobIcon->Show( false );
		m_pStaticName->SetText( L"" );
	}

	// 직업별 순위설정.
	SetJobRank( pInfo->cJobCode, pInfo->biClassRank, pInfo->biSubClassRank );		

	// 길드명.
	m_pStaticGuild->SetText( pInfo->wszGuildName );

	std::wstring strPoint = AtoCommaString( pInfo->uiExp );
	m_pStaticPoint->SetText( strPoint.c_str() );

	// Result.
	/*int nPersent = 0;
	if( pInfo->iDeath == 0 )
		nPersent = pInfo->iKill * 100;
	else if( pInfo->iDeath > 0 )
		nPersent = (int)( ( (float)pInfo->iKill / (float)pInfo->iDeath ) * 100.0f );	
	swprintf_s( str, 256, L"%d / %d (%d%%)", pInfo->iKill, pInfo->iDeath, nPersent );*/
	swprintf_s( str, 256, L"%d / %d", pInfo->iKill, pInfo->iDeath );
	m_pStaticResult->SetText( str );
}

// 내정보 - 레더.
void CDnPvPRankDlg::SetInfoMyRankLadder( struct TPvPLadderRankingDetail * pInfo )
{
	m_totScrollPage = 1;

	wchar_t str[256] = {0,};

	if( pInfo->biRank > 0 )
	{
		swprintf_s( str, 256, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 120203 ), pInfo->biRank ); // "전체순위 %d위"
		m_pStaticTotRank->SetText( str );

		SetGuildIcon(); // 길드마크.
	}
	else
		m_pStaticTotRank->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 120210 ) ); // "전적없음"
	

	int job = pInfo->cJobCode;	

	// 콜로세움등급 아이콘.
	if( pInfo->cPvPLevel > 0 )
	{
		m_pTextureControl->Show( true );
		GetInterface().SetPvPClassIcon( m_pTextureControl, pInfo->cPvPLevel );
	}
	else
		m_pTextureControl->Show( false );

	m_pStaticMyPoint->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 126020 ) );// "평점"
	m_pStaticMyResult->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 2331 ) ); // "결과"

	if( pInfo->wszCharName != NULL && ( wcslen( pInfo->wszCharName ) > 1 ) )
	{
		std::wstring strName( pInfo->wszCharName );
		
		// 직업아이콘.
		LPCWSTR strJob = DN_INTERFACE::STRING::GetJobString( job );
		m_pJobIcon->Show( true );
		m_pJobIcon->SetIconID( job );
		m_pJobIcon->SetTooltipText( strJob );

		// 이름 (직업 LVxx).
		strName.append( L" (" );
		strName.append( strJob );
		swprintf_s( str, 256, GetUIString( CEtUIXML::idCategory1, 2020022 ), pInfo->iLevel ); // "Lv%d"
		strName.append( str );
		strName.append( L")" );
		m_pStaticName->SetText( strName );
	}
	else
	{
		m_pTextureControlGuild->Show( false );
		m_pJobIcon->Show( false );
		m_pStaticName->SetText( L"" );
	}

	// 직업별 순위설정.
	SetJobRank( pInfo->cJobCode, pInfo->biClassRank, pInfo->biSubClassRank );		
	
	// 길드명.
	m_pStaticGuild->SetText( pInfo->wszGuildName );
	
	std::wstring strPoint = AtoCommaString( pInfo->iPvPLadderGradePoint );
	m_pStaticPoint->SetText( strPoint.c_str() );

	// Result.
	int nPersent = 0;
	int denominator = pInfo->iWin + pInfo->iLose;
	if( denominator > 0 )
		nPersent = (int)( ( (float)pInfo->iWin / (float)denominator ) * 100 );

	swprintf_s( str, 256, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 126039 ), pInfo->iWin, pInfo->iLose, pInfo->iDraw ); // "%d 승 %d 패 %d 무"
	std::wstring strResult( str );
	strResult.append( L" (" );
	swprintf_s( str, 256, L"%s %d%%", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 126177 ), nPersent );  // "승률 :"
	strResult.append( str );
	strResult.append( L")" );
	m_pStaticResult->SetText( strResult.c_str() );
	memset( str, 0, 256 );	

}

// 캐릭터정보설정 - 콜로세움.
void CDnPvPRankDlg::SetInfoColosseum( struct TPvPRankingDetail * pInfo )
{
	if( !m_pListRank )
		return;

	// 목록초기화.
	ResetList();

	// 
	ResetPvPList();

	if( pInfo->iLevel > 0 )
	{
		CDnPvPRankListItemDlg * pListItemDlg = m_pListRank->AddItem< CDnPvPRankListItemDlg >();
		if( pListItemDlg )
			pListItemDlg->SetInfoUserColosseum( pInfo );
	}
}

// 캐릭터정보설정 - 콜로세움.
void CDnPvPRankDlg::SetInfoLadder( struct TPvPLadderRankingDetail * pInfo )
{
	if( !m_pListRank )
		return;

	// 목록초기화.
	ResetList();

	// 
	ResetPvPList();

	if( pInfo->iLevel > 0 )
	{
		//CDnPvPRankListItemDlg * pListItemDlg = m_pListRank->AddItem< CDnPvPRankListItemDlg >();
		CDnPvPRankListItemLadderDlg * pListItemDlg = m_pListRank->AddItem< CDnPvPRankListItemLadderDlg >();
		if( pListItemDlg )
			pListItemDlg->SetInfoUserLadder( pInfo );
	}
}


//// job의 상위 직업명 반환.
//int CDnPvPRankDlg::GetParentJob( int job )
//{
//	DNTableFileFormat * pTable = GetDNTable( CDnTableDB::TJOB );
//	if( !pTable )
//		return job;
//
//	DNTableCell * pCell = NULL;
//
//	int nParentJob = job;
//	int nSize = pTable->GetItemCount();
//	for( int i=0; i<nSize; ++i )
//	{
//		int nID = pTable->GetItemID( i );
//		if( nID != job )
//			continue;
//		
//		/*pCell = pTable->GetFieldFromLablePtr( nID, "_JobNumber" );
//		if( pCell && pCell->GetInteger() == 2 )
//		{*/
//			pCell = pTable->GetFieldFromLablePtr( nID, "_ParentJob" );
//			if( pCell && pCell->GetInteger() > 0 )
//			{
//				nParentJob = pCell->GetInteger();
//				break;
//			}
//		//}		
//	}
//
//	return nParentJob;	
//}

// job의 상위 직업명 반환.
int CDnPvPRankDlg::GetParentJob( int job )
{
	DNTableFileFormat * pTable = GetDNTable( CDnTableDB::TJOB );
	if( !pTable )
		return 0;

	DNTableCell * pCell = NULL;

	int nParentJob = 0;
	int nSize = pTable->GetItemCount();
	for( int i=0; i<nSize; ++i )
	{
		int nID = pTable->GetItemID( i );
		if( nID != job )
			continue;

		pCell = pTable->GetFieldFromLablePtr( nID, "_ParentJob" );
		if( pCell && pCell->GetInteger() > 0 )
		{
			nParentJob = pCell->GetInteger();
			break;
		}		
	}

	return nParentJob;	
}

// 목록 - 콜로세움.
void CDnPvPRankDlg::SetListColosseum( SCPvPRankList * pInfo )
{
	// 목록초기화.
	ResetList();

	INT64 tot = 0;
	if( m_pListColosseum ) 
		tot = m_pListColosseum->nRankingTotalCount;
	SAFE_DELETE( m_pListColosseum );
	
	if( pInfo->nRankingTotalCount != -1 )
		m_totScrollPage = (int)( pInfo->nRankingTotalCount / (INT64)( RankingSystem::RANKINGPAGEMAX * RankingSystem::RANKINGPAGECOUNT ) + 1 );

	// 현재페이지 계산.
	if( m_bScroll )
		m_CrrPage = ( m_RequestPage - 1 ) * RankingSystem::RANKINGPAGECOUNT + 1;

	m_pListColosseum = new SCPvPRankList;
	m_pListColosseum->cRankingCount = pInfo->cRankingCount;
	m_pListColosseum->nRankingTotalCount = pInfo->nRankingTotalCount != -1 ? pInfo->nRankingTotalCount : tot;
	for( int i=0; i<m_pListColosseum->cRankingCount; ++i )
		m_pListColosseum->RankingInfo[ i ] = pInfo->RankingInfo[ i ];	


	// 유저리스트 설정.
	PageChange();
}


// 목록 - 레더.
void CDnPvPRankDlg::SetListLadder( SCPvPLadderRankList * pInfo )
{
	// 목록초기화.
	ResetList();

	INT64 tot = 0;
	if( m_pListLadder )  
		tot = m_pListLadder->nRankingTotalCount;
	SAFE_DELETE( m_pListLadder );

	if( pInfo->nRankingTotalCount != -1 )
		m_totScrollPage = (int)( pInfo->nRankingTotalCount / (INT64)( RankingSystem::RANKINGPAGEMAX * RankingSystem::RANKINGPAGECOUNT ) + 1 );
		//m_totScrollPage = pInfo->nRankingTotalCount / ( RankingSystem::RANKINGPAGEMAX * RankingSystem::RANKINGPAGECOUNT ) + 1;

	// 현재페이지 계산.
	if( m_bScroll )
		m_CrrPage = ( m_RequestPage - 1 ) * RankingSystem::RANKINGPAGECOUNT + 1;

	m_pListLadder = new SCPvPLadderRankList;
	m_pListLadder->cRankingCount = pInfo->cRankingCount;
	m_pListLadder->nRankingTotalCount = pInfo->nRankingTotalCount != -1 ? pInfo->nRankingTotalCount : tot;
	for( int i=0; i<m_pListLadder->cRankingCount; ++i )
		m_pListLadder->RankingInfo[ i ] = pInfo->RankingInfo[ i ];	

	m_eCurrentLadderType = pInfo->MatchType;

	// 유저리스트 설정.
	PageChange();
}

// 1Page씩 변경 & 유저리스트 설정.
void CDnPvPRankDlg::PageChange()
{	
	if( m_CrrPage < 1 )
	{
		m_crrSelectPage = m_CrrPage = 1;
		return;
	}

	// << 와 같은상황.
	int cnt = m_CrrPage * RankingSystem::RANKINGPAGEMAX;
	if( cnt < ( m_RequestPage - 1 ) * RankingSystem::RANKINGMAX + 1 )
	{
		m_bScroll = false;
		PageScroll( -1 );
		return;
	}

	// >> 와 같은상황.
	if( cnt > m_RequestPage * RankingSystem::RANKINGMAX )
	{
		m_bScroll = false;
		PageScroll( 1 );
		return;
	}

	// Max Page.
	INT64 tot = _I64_MAX;
	if( m_RankType == ERankType::ERankType_Colosseum )
	{
		if( m_pListColosseum == NULL )
			return;

		tot = m_pListColosseum->nRankingTotalCount;
	}
	else if( m_RankType == ERankType::ERankType_Ladder )
	{
		if( m_pListLadder == NULL )
			return;
		tot = m_pListLadder->nRankingTotalCount;
	}
#ifdef PRE_ADD_DWC
	else if( m_RankType == ERankType::ERankType_DWC )
	{
		if( m_pListDWC == NULL )
			return;
		tot = m_pListDWC->nTotalRankSize;
	}
#endif

	if( tot < ( RankingSystem::RANKINGPAGEMAX * (m_CrrPage-1) ) )
	{
		m_CrrPage -= 1;
		return;
	}

	// SelectPage.
	if( m_RequestPage > 1 )
	{
		int denominator = ( m_RequestPage - 1 ) * RankingSystem::RANKINGPAGECOUNT;
		m_crrSelectPage = ( m_CrrPage - 1 ) / denominator;
		m_crrSelectPage += ( m_CrrPage - 1 ) % denominator;
	}
	else
	{
		m_crrSelectPage = m_CrrPage;
	}
	RefreshPageControls();

	// 리스트설정.
	if( !m_pListRank )
		return;

	m_pListRank->RemoveAllItems();

	// Page 당 보여질 Item 개수 계산.
	int page = m_CrrPage - ( (m_RequestPage - 1) * RankingSystem::RANKINGPAGECOUNT ) ;	
	int min = ( page - 1 ) * RankingSystem::RANKINGPAGEMAX;
	int max = page * RankingSystem::RANKINGPAGEMAX;

	int cRankingCount = 0;
	if( m_RankType == ERankType::ERankType_Colosseum )
		cRankingCount = m_pListColosseum->cRankingCount;
	else if( m_RankType == ERankType::ERankType_Ladder )
		cRankingCount = m_pListLadder->cRankingCount;
#ifdef PRE_ADD_DWC
	else if( m_RankType == ERankType::ERankType_DWC )
		cRankingCount = m_pListDWC->nTotalRankSize;
#endif

	if( max > cRankingCount )
	{
		if( RankingSystem::RANKINGPAGEMAX > ( max - cRankingCount ) )		
		{
			if( page > 1 )
			{
				max = cRankingCount % ( ( page - 1 ) * RankingSystem::RANKINGPAGEMAX );
				max += min;
			}
			else
				max = cRankingCount - min;
		}
	}
	
	int nRankCount = 0;
	if( m_RankType == ERankType::ERankType_Colosseum && m_pListColosseum )
	{
		nRankCount = m_pListColosseum->cRankingCount;
		
		if( m_pListColosseum->cRankingCount > 0 )
		{
			for( int i=min; i<max; ++i )	
			{
				CDnPvPRankListItemDlg * pListItemDlg = m_pListRank->AddItem< CDnPvPRankListItemDlg >();
				if( pListItemDlg )
					pListItemDlg->SetInfoColosseum( &m_pListColosseum->RankingInfo[ i ] );			
			}
		}
	}
	
	else if( m_RankType == ERankType::ERankType_Ladder && m_pListLadder )
	{
		nRankCount = m_pListLadder->cRankingCount;

		if( m_pListLadder->cRankingCount > 0 )
		{
			for( int i=min; i<max; ++i )	
			{
				CDnPvPRankListItemLadderDlg * pListItemDlg = m_pListRank->AddItem< CDnPvPRankListItemLadderDlg >();
				if( pListItemDlg )
					pListItemDlg->SetInfoLadder( &m_pListLadder->RankingInfo[ i ] );			
			}
		}
	}

#if defined(PRE_ADD_DWC)
	else if( m_RankType == ERankType::ERankType_DWC && m_pListDWC )
	{
		nRankCount = m_pListDWC->nTotalRankSize;
		
		if( m_pListDWC->nPageSize > 0)	
		{
			for( int i=min; i<max; ++i )	
			{
				CDnPvPRankListItemDwcDlg* pListItemDlg = m_pListRank->AddItem<CDnPvPRankListItemDwcDlg>();
				if( pListItemDlg )
					pListItemDlg->SetDWCRankInfo( &m_pListDWC->RankDataList[i] );
			}
		}
	}
#endif // PRE_ADD_DWC


	// Page 컨트롤 설정.
	std::wstring strP;
	wchar_t strPage[256] = {0,};
	int pageCnt = ( nRankCount - 1 ) / RankingSystem::RANKINGPAGEMAX + 1;
	for( int i=0; i<RankingSystem::RANKINGPAGECOUNT; ++i )
	{
		if( i < pageCnt )
		{		
			strP.assign( L" " );
			_itow( i + ( ( m_RequestPage - 1 ) * RankingSystem::RANKINGPAGECOUNT ) + 1, strPage, 10 );
			strP.append( strPage );
			strP.append( L" " );
			m_vecPages[ i ]->SetText( strP.c_str() );
			m_vecPages[ i ]->Show( true );
		}
		else
			m_vecPages[ i ]->Show( false );
	}

}

// RANKINGPAGECOUNT 단위 변경.
void CDnPvPRankDlg::PageScroll( int n )
{
	m_RequestPage += n;
	if( m_RequestPage < 1 )
	{
		m_RequestPage = 1;
		return;
	}

	if( m_RequestPage > m_totScrollPage )
	{
		m_RequestPage = m_totScrollPage;
		return;
	}

	INT64 tot = _I64_MAX;
	if( m_RankType == ERankType::ERankType_Colosseum && m_pListColosseum )
		tot = (int)m_pListColosseum->nRankingTotalCount;
	else if( m_RankType == ERankType::ERankType_Ladder && m_pListLadder )
		tot = m_pListLadder->nRankingTotalCount;
#ifdef PRE_ADD_DWC
	else if( m_RankType == ERankType::ERankType_DWC && m_pListDWC )
		tot = m_pListDWC->nTotalRankSize;
#endif
	
	if( tot < RankingSystem::RANKINGPAGEMAX * RankingSystem::RANKINGPAGECOUNT * (m_RequestPage-1) )
		return;
	
	// Send.
	if( m_RankType == ERankType::ERankType_Colosseum )
		OnSendPvPRankList( m_RequestPage, m_Job, m_SubClass, ( m_StrSearch == NULL ? L"" : m_StrSearch ) );
	else if( m_RankType == ERankType::ERankType_Ladder )
		OnSendPvPRankLadderList( m_eCurrentLadderType, m_RequestPage, m_Job, m_SubClass, ( m_StrSearch == NULL ? L"" : m_StrSearch ) );
#ifdef PRE_ADD_DWC
	else if( m_RankType == ERankType::ERankType_DWC )
		SendDWCRankList( m_RequestPage, DWC::Common::RankPageMaxSize);
#endif
}

void CDnPvPRankDlg::Reset()
{	
	m_pListRank->SelectItem( 0 );
	m_pComboCondition->ClearSelectedItem();
	m_pComboCondition->SetSelectedByIndex( (int)ERankType::ERankType_Colosseum );

	m_pIMEEditBox->SetText( L"" );	

	m_totScrollPage = m_CrrPage = m_RequestPage = 1;
	m_Job = m_SubClass = 0;
	m_StrSearch = NULL;

	int size = (int)m_vecPages.size();
	for( int i=0; i<size; ++i )
		m_vecPages[ i ]->Show( false );


	//for( int i=0; i<RankingSystem::RANKINGPAGECOUNT; ++i )
	//	m_vecPages[ i ]->SetBgTextColor( D3DCOLOR_ARGB(90, 0, 0, 0), UI_STATE_NORMAL );

	//pStaticPage->SetBgTextColor( D3DCOLOR_ARGB(128, 64, 86, 128), UI_STATE_MOUSEENTER );			
	//pStaticPage->SetBgTextColor( D3DCOLOR_ARGB(90, 0, 0, 0), UI_STATE_NORMAL );		
}


void CDnPvPRankDlg::RefreshPageControls()
{
	int size = (int)m_vecPages.size();
	for( int i=0; i<size; ++i )
	{
		if( i == (m_crrSelectPage-1) ) {
			m_vecPages[ i ]->SetBgTextColor( D3DCOLOR_ARGB(90, 164, 32, 48), UI_STATE_MOUSEENTER );
			m_vecPages[ i ]->SetBgTextColor(  D3DCOLOR_ARGB(90, 164, 32, 48) , UI_STATE_NORMAL );
		}
		else {
			m_vecPages[ i ]->SetBgTextColor( D3DCOLOR_ARGB(128, 64, 86, 128), UI_STATE_MOUSEENTER );			
			m_vecPages[ i ]->SetBgTextColor( D3DCOLOR_ARGB(90, 0, 0, 0), UI_STATE_NORMAL );		
		}			
	}


	m_pStaticBack->Enable( true );
	m_pStaticNext->Enable( true );
	m_pStaticFirst->Enable( true );
	m_pStaticLast->Enable( true );

	INT64 totPage = 0;
	if( m_RankType == ERankType::ERankType_Colosseum )
	{
		if( m_pListColosseum )
		{
			totPage = m_pListColosseum->nRankingTotalCount / RankingSystem::RANKINGPAGEMAX;
			//totPage += ( m_pListColosseum->nRankingTotalCount % RankingSystem::RANKINGPAGEMAX ) / RankingSystem::RANKINGPAGEMAX + 1;
			int rest = (int)m_pListColosseum->nRankingTotalCount % RankingSystem::RANKINGPAGEMAX;
			if( rest > 0 )
				rest = rest / RankingSystem::RANKINGPAGEMAX + 1;
			totPage += rest;
		}
		else
		{
			totPage = 1;
		}
	}
	else if( m_RankType == ERankType::ERankType_Ladder )
	{
		if( m_pListLadder )
		{
			totPage = m_pListLadder->nRankingTotalCount / RankingSystem::RANKINGPAGEMAX;
			//totPage += ( m_pListLadder->nRankingTotalCount % RankingSystem::RANKINGPAGEMAX ) / RankingSystem::RANKINGPAGEMAX + 1;
			int rest = (int)m_pListLadder->nRankingTotalCount % RankingSystem::RANKINGPAGEMAX;
			if( rest > 0 )
				rest = rest / RankingSystem::RANKINGPAGEMAX + 1;
			totPage += rest;			
		}
		else
		{
			totPage = 1;
		}
	}
#ifdef PRE_ADD_DWC
	else if( m_RankType == ERankType::ERankType_DWC )
	{
		if( m_pListDWC )
		{
			totPage = (m_pListDWC->nTotalRankSize) / RankingSystem::RANKINGPAGEMAX;
			int rest = (m_pListDWC->nTotalRankSize) % RankingSystem::RANKINGPAGEMAX;
			if( rest > 0 )
				rest = rest / RankingSystem::RANKINGPAGEMAX + 1;
			totPage += rest;
		}
		else
		{
			totPage = 1;
		}
	}
#endif

	if( m_CrrPage >= totPage )
	{
		m_CrrPage = (int)totPage;
		m_pStaticNext->Enable( false );
	}

	if( m_CrrPage == 1 )
	{
		m_pStaticBack->Enable( false );  // <
		m_pStaticFirst->Enable( false ); // <<
	}
	
	if( m_RequestPage == 1 )
		m_pStaticFirst->Enable( false ); // <<

	if( m_RequestPage == m_totScrollPage )
		m_pStaticLast->Enable( false ); // >>
}

// 길드마크.
void CDnPvPRankDlg::SetGuildIcon()
{	
	if( CDnActor::s_hLocalActor )
	{
		CDnPlayerActor * pPlayerActor = static_cast< CDnPlayerActor * >( CDnActor::s_hLocalActor.GetPointer() );

		if( pPlayerActor->IsJoinGuild() )
		{			
			if( pPlayerActor->GetGuildSelfView().IsSet() ) 
			{								
				const TGuildView &GuildView = pPlayerActor->GetGuildSelfView();
				if( GetGuildTask().IsShowGuildMark( GuildView ) )
				{
					m_pTextureControlGuild->Show( true );
					EtTextureHandle hGuildMark = GetGuildTask().GetGuildMarkTexture( GuildView );
					m_pTextureControlGuild->SetTexture( hGuildMark );					
				}
			}
		}	
	}

}

// 목록초기화.
void CDnPvPRankDlg::ResetList()
{
	m_pListRank->RemoveAllItems();

	int size = (int)m_vecPages.size();
	for( int i=1; i<size; ++i )
		m_vecPages[ i ]->Show( false );
}


void CDnPvPRankDlg::ResetPvPList()
{
	m_CrrPage = m_RequestPage = m_totScrollPage = 1;
	if( m_RankType == ERankType::ERankType_Colosseum )
		SAFE_DELETE( m_pListColosseum );
	if( m_RankType == ERankType::ERankType_Ladder )
		SAFE_DELETE( m_pListLadder );

	int size = (int)m_vecPages.size();
	for( int i=0; i<size; ++i )
		m_vecPages[ i ]->Show( false );
	m_vecPages[ 0 ]->Show( true );
	m_vecPages[ 0 ]->SetText( FormatW( L"%d", m_CrrPage) );

	RefreshPageControls();
}


void CDnPvPRankDlg::SetJobRank( int job, INT64 classRank, INT64 subClassRank )
{	
	// 전적없음.
	if( job == 0 )
	{
		m_pStaticFirstRank->SetText( L"" );
		m_pStaticSecondRank->SetText( L"" );
		return;
	}

	int parentJob = job;

	int changeClass = 1;
	while( true )
	{
		parentJob = GetParentJob( parentJob );
		if( parentJob == 0 )
			break;

		++changeClass;
	}

	const wchar_t * pStr = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 120204 ); // "- %s 순위 %d위"
	
	// 1차직업.
	if( changeClass == 1 )
	{
		m_pStaticFirstRank->SetText( FormatW( pStr, DN_INTERFACE::STRING::GetJobString( job ), classRank ) );
		m_pStaticSecondRank->SetText( L"" );
	}

	// 2차직업
	else if( changeClass == 2 )
	{
		m_pStaticFirstRank->SetText( FormatW( pStr, DN_INTERFACE::STRING::GetJobString( GetParentJob(job) ), classRank ) );
		m_pStaticSecondRank->SetText( FormatW( pStr, DN_INTERFACE::STRING::GetJobString( job ), subClassRank) );
	}

	// 3차직업.
	else if( changeClass == 3 )
	{
		int paJob = GetParentJob(job);
		m_pStaticFirstRank->SetText( FormatW( pStr, DN_INTERFACE::STRING::GetJobString( GetParentJob( paJob ) ), classRank ) );
		m_pStaticSecondRank->SetText( FormatW( pStr, DN_INTERFACE::STRING::GetJobString( paJob ), subClassRank ) );
	}
}

#if defined(PRE_ADD_DWC)
void CDnPvPRankDlg::SetListDWC(SCGetDWCRankPage* pInfo)
{
	if(pInfo)
	{
#ifndef _FINAL_BUILD
		bool TEST = false;
#endif
		if(pInfo->nPageSize <= 0)
		{
			m_CrrPage--;
			if(m_CrrPage < 0) m_CrrPage = 0;
			
			m_RequestPage--;
			if(m_RequestPage < 1) m_RequestPage = 1;

			m_pStaticLast->Enable(false);
			return;
		}

		// 목록 초기화.
		ResetList();

		int tot = 0;
		if( m_pListDWC ) 
			tot = m_pListDWC->nPageSize;
		SAFE_DELETE( m_pListDWC );

		m_pListDWC = new SCGetDWCRankPage;
		m_pListDWC->nPageNum  = pInfo->nPageNum;
		m_pListDWC->nPageSize = pInfo->nPageSize != -1 ? pInfo->nPageSize : tot;
		m_pListDWC->nTotalRankSize = pInfo->nTotalRankSize;
		for( int i=0; i<m_pListDWC->nPageSize; ++i )
			m_pListDWC->RankDataList[i] = pInfo->RankDataList[i];

		// "DWCGM_"을 지운다.		
		for(int i = 0 ; i < (int)m_pListDWC->nTotalRankSize ; ++i)
		{
			std::wstring wszName(m_pListDWC->RankDataList[i].LeaderData.wszMemberName);
			RemoveStringW(wszName, std::wstring(L"DWCGM_"));
			_wcscpy(m_pListDWC->RankDataList[i].LeaderData.wszMemberName, _countof(m_pListDWC->RankDataList[i].LeaderData.wszMemberName), wszName.c_str(), (int)wszName.length());
			
			for(int j = 0 ; j < DWC::DWC_MAX_MEMBERISZE ; ++j)
			{
				wszName = m_pListDWC->RankDataList[i].MemberData[j].wszMemberName;
				RemoveStringW(wszName, std::wstring(L"DWCGM_"));
				_wcscpy(m_pListDWC->RankDataList[i].MemberData[j].wszMemberName, _countof(m_pListDWC->RankDataList[i].MemberData[j].wszMemberName),wszName.c_str(), (int)wszName.length());
			}
		}

#ifndef _FINAL_BUILD
		if(TEST == false)
#endif
		{
			if( m_pListDWC->nTotalRankSize != -1 )
				m_totScrollPage = (int)(m_pListDWC->nTotalRankSize / (INT64)(DWC::Common::RankPageMaxSize) + 1);
		}
		
		// 현재페이지 계산
		if( m_bScroll )
			m_CrrPage = (m_RequestPage - 1) * RankingSystem::RANKINGPAGECOUNT + 1;

#ifndef _FINAL_BUILD
		/////////////// /////////////// /////////////// /////////////// /////////////// /////////////// 
		// 테스트 소스
		/////////////// /////////////// /////////////// /////////////// /////////////// /////////////// 
		if(TEST == true && pInfo->nPageSize > 0)
		{
			struct sRandomDWCTeamData
			{
				TDWCRankData data;

				sRandomDWCTeamData(int i)
				{
					memset(&data, 0, sizeof(TDWCRankData));
					wsprintf(data.wszTeamName, L"테스트_%d", i);
					data.nWinCount  = (rand()%10) + i;
					data.nLoseCount = (rand()%20) + i;
					data.nDrawCount = (rand()%30) + i;
					
					wsprintf(data.LeaderData.wszMemberName, L"팀장_%d", i);
					data.LeaderData.cJobCode = 11;
					data.nDWCPoint = (rand()%1500) + i;
					data.nRank = (rand()%20) + i;
					
					for(int j = 0 ; j < DWC::DWC_MAX_MEMBERISZE ; ++j)
						wsprintf(data.MemberData[j].wszMemberName, L"파티원_%d", j);
				}
			};

			int i = m_pListDWC->nTotalRankSize;
			for( ; i < DWC::Common::RankPageMaxSize ; ++ i)
			{
				sRandomDWCTeamData D(i);
				m_pListDWC->RankDataList[i] = D.data;			
				m_pListDWC->nTotalRankSize++;
			}

			if( m_pListDWC->nTotalRankSize != -1 )
				m_totScrollPage = (int)(m_pListDWC->nTotalRankSize / (INT64)(DWC::Common::RankPageMaxSize) + 1);	
		}		
#endif // #ifndef _FINAL_BUILD

		// 유저리스트 설정.
		PageChange();

		// UI 설정
		SetUIDWCTeamData(pInfo);
	}		
}

void CDnPvPRankDlg::SetFindListDWC(SCGetDWCFindRank* pInfo)
{
	if(pInfo)
	{
		// 목록 초기화.
		ResetList();

		SAFE_DELETE( m_pListDWC );

		// 1개 이상 올수가 없다
		m_pListDWC = new SCGetDWCRankPage;
		m_pListDWC->nPageNum  = 0;
		m_pListDWC->nPageSize = 1;
		m_pListDWC->nTotalRankSize = 1;
		m_pListDWC->RankDataList[0] = pInfo->Rank;

		std::wstring wszName(m_pListDWC->RankDataList[0].LeaderData.wszMemberName);
		RemoveStringW(wszName, std::wstring(L"DWCGM_"));
		_wcscpy(m_pListDWC->RankDataList[0].LeaderData.wszMemberName, _countof(m_pListDWC->RankDataList[0].LeaderData.wszMemberName), wszName.c_str(), (int)wszName.length());
		
		if( m_pListDWC->nTotalRankSize != -1 )
			m_totScrollPage = (int)(m_pListDWC->nTotalRankSize / (INT64)(DWC::Common::RankPageMaxSize) + 1);

		// 현재페이지 계산
		if( m_bScroll )
			m_CrrPage = (m_RequestPage - 1) * RankingSystem::RANKINGPAGECOUNT + 1;

		// 유저리스트 설정.
		PageChange();
	}	
}

void CDnPvPRankDlg::ChangeStaticControlsState()
{
	bool bIsDwcUser = GetDWCTask().IsDWCChar();
	if(bIsDwcUser == false)
		return;

	const char* szShowCtrls[]	  = { "ID_STATIC_DWCTAB0", "ID_STATIC_DWCTAB1", "ID_STATIC_DWCTAB2", "ID_STATIC_RESULTTAB1", "ID_STATIC_POINTTAB1", 
									  "ID_TEXT_DWCTAB0", "ID_TEXT_DWCTAB1", "ID_TEXT_DWCTAB2", "ID_TEXT_RESULTTAB1" };
	const char* szDontShowCtrls[] = { "ID_STATIC_TAB1","ID_STATIC_TAB2","ID_STATIC_TAB3","ID_STATIC_TAB4","ID_STATIC_RESULTTAB0","ID_STATIC_POINTTAB0",
								      "ID_TEXT_TAB1","ID_TEXT_TAB2","ID_TEXT_TAB3","ID_TEXT_TAB4","ID_TEXT_RESULTTAB0", "ID_TEXT_POINTTAB0" };
	
	int nShowCtrlSize		= sizeof(szShowCtrls) / sizeof(szShowCtrls[0]);
	int nDontShowCtrlSize	= sizeof(szDontShowCtrls) / sizeof(szDontShowCtrls[0]);

	for(int i = 0 ; i < nShowCtrlSize ; ++i )
	{
		CEtUIStatic* pStatic = GetControl<CEtUIStatic>(szShowCtrls[i]);
		if(pStatic) pStatic->Show(bIsDwcUser);
	}

	for(int i = 0 ; i < nDontShowCtrlSize ; ++i )
	{
		CEtUIStatic* pStatic = GetControl<CEtUIStatic>(szDontShowCtrls[i]);
		if(pStatic) pStatic->Show(!bIsDwcUser);
	}
	
	for(int i = 0 ; i < 3 ; ++i)
	{
		CEtUIRadioButton* pBtn = GetControl<CEtUIRadioButton>(FormatA("ID_RBT%d", i+1).c_str());
		if(pBtn) pBtn->Show(!bIsDwcUser);
	}
	CEtUIRadioButton* pBtn = GetControl<CEtUIRadioButton>(FormatA("ID_RBT0").c_str());
	if(pBtn) pBtn->SetText(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 120254)); // 라디오버튼::DWC본선

	if(bIsDwcUser)
	{
		m_pTreeJob->DeleteAllItems();
		m_pTreeJob->Show(false);
		m_pStaticSubTitle->SetText(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 120250)); // 제목스테틱::DWC 본선 순위
	}
	else
	{
		m_pTreeJob->Show(true);
		m_pStaticSubTitle->SetText( GetSubString( ERankType::ERankType_Colosseum ) );
	}
}

void CDnPvPRankDlg::SetUIDWCTeamData(SCGetDWCRankPage* pInfo)
{
	std::vector<TDWCTeam> vDWCTeamData = GetDWCTask().GetDwcTeamInfoList();
	if(!vDWCTeamData.empty())
	{
		// 결과
		WCHAR wszTemp[256] = { 0 , };
		std::wstring wTempStr = FormatW(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 126039), vDWCTeamData[0].nTotalWin, vDWCTeamData[0].nTotalLose, vDWCTeamData[0].nTotalDraw);
		int TotalCount = (vDWCTeamData[0].nTotalWin + vDWCTeamData[0].nTotalLose + vDWCTeamData[0].nTotalDraw)? (vDWCTeamData[0].nTotalWin + vDWCTeamData[0].nTotalLose + vDWCTeamData[0].nTotalDraw) : 1;
		wsprintf(wszTemp, L" %d%%", (int)( (float)vDWCTeamData[0].nTotalWin / (float)TotalCount * 100) );
		wTempStr.append(wszTemp);
		m_pStaticResult->SetText(wTempStr);

		// 평점
		_itow(vDWCTeamData[0].nDWCPoint, wszTemp, 10);
		m_pStaticPoint->SetText( wszTemp );

		struct sCheckRankData
		{
			bool bHasRank;
			int  nIndex;
			sCheckRankData() : bHasRank(false), nIndex(-1) {}
		};
		sCheckRankData sRankData;

		for(int i = 0 ; i < (int)pInfo->nTotalRankSize ; ++i)
		{
			if( _tcscmp(pInfo->RankDataList[i].wszTeamName, vDWCTeamData[0].wszTeamName) == 0 )
			{
				sRankData.bHasRank = true;
				sRankData.nIndex = i;
				break;
			}
		}

		if(sRankData.bHasRank)
		{
			wchar_t str[256] = {0,};
			if( pInfo->RankDataList[sRankData.nIndex].nRank > 0 )
			{
				swprintf_s( str, 256, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 120203 ), pInfo->RankDataList[sRankData.nIndex].nRank ); // "전체순위 %d위"
				m_pStaticTotRank->SetText( str );	
			}
		}
		else
			m_pStaticTotRank->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 120210 ) ); // "전적없음"

		m_pStaticFirstRank->Show(false);
		m_pStaticSecondRank->Show(false);
	}
	else
	{
		m_pStaticTotRank->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 120210 ) ); // "전적없음"
		m_pStaticFirstRank->Show(false);
		m_pStaticSecondRank->Show(false);

		m_pStaticResult->SetText(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 93));
		m_pStaticPoint->SetText(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 93));
	}
}
#endif // PRE_ADD_DWC

#endif // PRE_ADD_PVP_RANKING