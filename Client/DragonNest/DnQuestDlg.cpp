#include "StdAfx.h"
#include "DnQuestDlg.h"
#include "DnQuestDescriptionDlg.h"
#include "DnQuestRecompenseDlg.h"
#include "EtUIQuestTreeCtl.h"
#include "EtUIQuestTreeItem.h"
#include "EtUITextBox.h"
#include "DnDataManager.h"
#include "DNUserData.h"
#include "DnQuestTask.h"
#include "DnMainMenuDlg.h"
#include "DnInterface.h"
#include "GameOption.h"
#include "DnTableDB.h"
#include "DnLocalPlayerActor.h"
#ifdef PRE_ADD_REMOTE_QUEST
#include "DnMainDlg.h"
#include "DnInvenTabDlg.h"
#endif // PRE_ADD_REMOTE_QUEST



#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnQuestDlg::CDnQuestDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
	: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback  )
	, m_pDescriptionDlg(NULL)
	, m_pRecompenseDlg(NULL)
	, m_pTreeControl(NULL)
	, m_nQuestIndex(0)
	, m_bSaveNotifierToServerNeeded( false )	
	, m_nLocalLevel( -1 )
	, m_pButtonTrace( NULL )
	, m_pButtonDel( NULL )
	, m_pQuestShowLevelCheckBox( NULL )
    , m_pQuestNotifyPriority( NULL )
	, m_emQuestType(typeMain)
{
}

CDnQuestDlg::~CDnQuestDlg(void)
{
	SAFE_DELETE(m_pDescriptionDlg);
	SAFE_DELETE(m_pRecompenseDlg);
}

void CDnQuestDlg::Initialize(const char *pFileName, bool bShow)
{
	CEtUIDialog::Initialize( pFileName, bShow );
}

void CDnQuestDlg::InitialUpdate()
{
	m_pDescriptionDlg = new CDnQuestDescriptionDlg( UI_TYPE_CHILD, this, QUEST_DESCRIPTION_DIALOG, this );
	m_pDescriptionDlg->Initialize( false );

	m_pRecompenseDlg = new CDnQuestRecompenseDlg( UI_TYPE_CHILD, this, QUEST_RECOMPENSE_DIALOG, this );
	m_pRecompenseDlg->Initialize( false );

	m_pTreeControl = GetControl<CDnQuestTree>("ID_TREE_QUEST");	
	m_pTreeControl->SetMainQuest( m_emQuestType == typeMain );

	CDnMainMenuDlg *pMainMenuDlg = GetInterface().GetMainMenuDialog();
	if( pMainMenuDlg ) pMainMenuDlg->SetQuestSumInfoDlg();

	CEtUICheckBox	*pQuestAlimiCheck = GetControl < CEtUICheckBox > ("ID_CHECKBOX_ALIMI");
	m_pTreeControl->SetCheckBoxProperty( pQuestAlimiCheck->GetProperty() );
	DeleteControl( pQuestAlimiCheck );

	m_pButtonTrace = GetControl<CEtUIButton>( "ID_BT_TRACING" );
	m_pButtonDel = GetControl<CEtUIButton>( "ID_SURRENDER" );

	m_pQuestShowLevelCheckBox = GetControl<CEtUICheckBox>("ID_CHECKBOX_ALIMIMARK");
	m_pQuestNotifyPriority = GetControl<CEtUICheckBox>("ID_CHECKBOX_SORT");

#ifdef PRE_ADD_REMOTE_QUEST
	CEtUIButton* pRemoteButton = GetControl<CEtUIButton>( "ID_BT_ASK" );
	m_pTreeControl->SetRemoteButtonProperty( pRemoteButton->GetProperty() );
	DeleteControl( pRemoteButton );
#endif // PRE_ADD_REMOTE_QUEST
}

void CDnQuestDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl( "ID_BUTTON_MAP" ) )
		{
			CDnMainMenuDlg *pMainMenuDlg = GetInterface().GetMainMenuDialog();
			if( pMainMenuDlg ) pMainMenuDlg->ShowWorldMapDialog( true );
			return;
		}
		// 서브 퀘스트 취소
		if( IsCmdControl( "ID_SURRENDER" ) )
		{
			CEtUIQuestTreeItem *pItem = static_cast<CEtUIQuestTreeItem*>(m_pTreeControl->GetSelectedItem());
			if( m_nQuestIndex > 0 && pItem)
			{
				GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 419 ), MB_YESNO, MESSAGEBOX_CONFIRM_REMOVEQUEST, this );
			}
			else
			{
				GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 420 ), MB_OK);
			}
			return;
		}
		if( IsCmdControl("ID_BT_TRACING") )
		{
			if( 0 != m_nQuestIndex )
			{
				GetQuestTask().SetTraceQuestID( m_nQuestIndex );

				CDnMainMenuDlg *pMainMenuDlg = GetInterface().GetMainMenuDialog();
				if (pMainMenuDlg )	pMainMenuDlg->ToggleShowDialog(CDnMainMenuDlg::ZONEMAP_DIALOG);
				GetQuestTask().OnUpdateNotifier( false );
			}
		}
#ifdef PRE_ADD_REMOTE_QUEST
		if( IsCmdControl( "ID_BT_ASK" ) )
		{
			CEtUIButton* pButton = dynamic_cast<CEtUIButton*>( pControl );
			if( pButton )
			{
				int nQuestIndex = pButton->GetButtonID();
				CDnQuestTree::eRemoteQuestState remoteQuestState = ((CDnQuestTree*)m_pTreeControl)->GetRemoteQuestState( nQuestIndex );
				((CDnQuestTree*)m_pTreeControl)->SetSelectRemoteQuest( nQuestIndex );
				OnTreeSelected();

				if( remoteQuestState == CDnQuestTree::REMOTEQUEST_ASK )
				{
					GetQuestTask().SendAcceptRemoteQuest( nQuestIndex );
				}
				else if( remoteQuestState == CDnQuestTree::REMOTEQUEST_COMPLETE )
				{
					Journal* pJournal = g_DataManager.GetJournalData( nQuestIndex );
					if( !pJournal ) return;
					if( !CDnActor::s_hLocalActor ) return;

					int nRecompenseID = pJournal->GetMyRecompenseID( CDnActor::s_hLocalActor->GetClassID() );
					if( nRecompenseID != 0 )
					{
						TQuestRecompense table;
						g_DataManager.GetQuestRecompense( nRecompenseID, table );
						bool bItemArray[QUESTREWARD_INVENTORYITEMMAX] = {0,};
						bool bCashItemArray[MAILATTACHITEMMAX] = {0,};

						if( table.cType == QuestRecompense_Select )
						{
							if( m_pRecompenseDlg->GetSlotSelectCount() != table.cSelectMax )
							{
								std::wstring strMsg;
								strMsg = FormatW( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1199999 ), table.cSelectMax );
								GetInterface().MessageBox( strMsg.c_str(), MB_OK );
								return;
							}
						}

						m_pRecompenseDlg->GetRemoteQuestRecompenseItemArray( bItemArray, bCashItemArray );

						int nItemCount = 0, nCashItemCount = 0;
						for( int nIndex=0; _countof(bItemArray)>nIndex; ++nIndex )
						{
							if (!bItemArray[nIndex]) continue;
							++nItemCount;
						}

						for( int nIndex=0; _countof(bCashItemArray)>nIndex; ++nIndex )
						{
							if (!bCashItemArray[nIndex]) continue;
							++nCashItemCount;
						}

						if( ( (0 < nItemCount) || ( (0 < nCashItemCount) && (nItemCount != 0) ) ) )
						{
							CDnInvenTabDlg *pInvenDlg = (CDnInvenTabDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::INVENTORY_DIALOG );
							if( pInvenDlg ) 
							{
								if( pInvenDlg->GetEmptySlotCount() < nItemCount )
								{
									GetInterface().MessageBox( 1925, MB_OK );
									return;
								}
							}
						}

						GetQuestTask().SendCompleteRemoteQuest( nQuestIndex, nRecompenseID, bItemArray, bCashItemArray );
					}
				}
			}
		}
#endif // PRE_ADD_REMOTE_QUEST
	}
	else if( nCommand == EVENT_TREECONTROL_SELECTION )
	{
		OnTreeSelected();
		return;
	}
	else if( nCommand == EVENT_TREECONTROL_SELECTION_END )
	{
		ShowChildDialog( m_pDescriptionDlg, false );
		ShowChildDialog( m_pRecompenseDlg, false );
		return;
	}
	else if( nCommand == EVENT_CHECKBOX_CHANGED )
	{		
		if( IsCmdControl( "ID_CHECKBOX_ALIMI" ) )
		{
			CEtUICheckBox *pCheckBox = ((CEtUICheckBox*)pControl);
			int nQuestIndex = pCheckBox->GetButtonID();
			DNNotifier::Type::eType questType= ( m_emQuestType == typeMain ) ? DNNotifier::Type::MainQuest : DNNotifier::Type::SubQuest;
			ASSERT( nQuestIndex > 0 );
			if( pCheckBox->IsChecked() ) {
				if( GetQuestTask().AddQuestNotifier( questType, nQuestIndex ) ) {
					GetQuestTask().OnUpdateNotifier( false );
					m_bSaveNotifierToServerNeeded = true;
				}
			}
			else {
				if( GetQuestTask().RemoveQuestNotifier( questType, nQuestIndex ) ) {
					GetQuestTask().OnUpdateNotifier( false );
					m_bSaveNotifierToServerNeeded = true;
				}
			}
			RefreshEnableNotifierCheck();
			CDnMainMenuDlg *pMainMenuDlg = GetInterface().GetMainMenuDialog();
			if( pMainMenuDlg ) pMainMenuDlg->SetQuestSumInfoDlg();
			return;
		}
		if( IsCmdControl( "ID_CHECKBOX_ALIMIMARK" ) )
		{
			CGameOption::GetInstance().m_bHideQuestMarkByLevel =  m_pQuestShowLevelCheckBox->IsChecked();
			CGameOption::GetInstance().Save();
			if( bTriggeredByUser ) GetQuestTask().SetRefreshNpcQuestState( true );
		}
		if( IsCmdControl( "ID_CHECKBOX_SORT" ) )
		{
			CGameOption::GetInstance().m_bEnableQuestNotifyPriority =  m_pQuestNotifyPriority->IsChecked();
			CGameOption::GetInstance().Save();
			if( bTriggeredByUser ) GetInterface().RefreshQuestSummaryInfo();
		}
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnQuestDlg::OnTreeSelected()
{
	ShowChildDialog( m_pDescriptionDlg, false );
	ShowChildDialog( m_pRecompenseDlg, false );

	CEtUIQuestTreeItem *pItem = static_cast<CEtUIQuestTreeItem*>(m_pTreeControl->GetSelectedItem());
	if( !pItem ) return;

	std::wstring &strValue = pItem->GetItemValueString();

	m_pButtonTrace->Enable( false );
	m_pButtonDel->Enable( false );

	if( strValue == L"Chapter" || strValue == L"DestnationMapIndex" )
	{
		ShowChildDialog( m_pDescriptionDlg, false );
		ShowChildDialog( m_pRecompenseDlg, false );
	}
	else if( strValue == L"Journal" )
	{
		m_nQuestIndex = pItem->GetItemValueInt();
		SetQuestRecompenseDlg( m_nQuestIndex );

		const TQuest* pQuest = GetQuestTask().GetQuestIndex( m_nQuestIndex );

		// 퀘스트 제목을 클릭했을땐 현재까지 열린 저널창중 가장 나중 저널을 출력해준다.
		Journal *pJournal = g_DataManager.GetJournalData( m_nQuestIndex );
		if( !pJournal ) return;
		if ( pJournal->JounalPages.size() > 0 )
		{
			JournalPage* pPage = NULL;
			if ( pQuest )
			{
				pPage = pJournal->FindJournalPage( pQuest->cQuestJournal );
				if ( !pPage )
					LogWnd::Log( 1, L"해당 저널페이지가 없음 : %d 퀘스트 %d 저널 ", m_nQuestIndex, pQuest->cQuestJournal );
			}
			else 
			{
				if ( GetQuestTask().IsClearQuest(pJournal->nQuestIndex) )
				{
					pPage = &(pJournal->JounalPages[pJournal->JounalPages.size()-1]);
				}
			}
#ifdef PRE_ADD_REMOTE_QUEST
			if( GetQuestTask().IsExistRemoteQuestAskList( m_nQuestIndex, CDnQuestTree::REMOTEQUEST_ASK ) )
				pPage = pJournal->FindJournalPage( 1 );
#endif // PRE_ADD_REMOTE_QUEST
			if ( pPage )
			{
				bool bShow = m_pDescriptionDlg->SetJournalText( m_nQuestIndex, pPage );
				ShowChildDialog( m_pDescriptionDlg, bShow );
			}
		}
		m_pButtonTrace->Enable( true );
		m_pButtonDel->Enable( true );
	}
	else if( strValue == L"JournalPage" )
	{
		m_nQuestIndex = (int)pItem->GetItemValueFloat();
		SetQuestRecompenseDlg( m_nQuestIndex );

		JournalPage* pPage = static_cast<JournalPage*>(pItem->GetItemData());

		bool bShow = m_pDescriptionDlg->SetJournalText( m_nQuestIndex, pPage );
		ShowChildDialog( m_pDescriptionDlg, bShow );

		m_pButtonTrace->Enable( true );
		m_pButtonDel->Enable( true );
	}
}

void CDnQuestDlg::Show( bool bShow )
{ 
	if( bShow )
	{
		m_pTreeControl->ResetSelectedItem();
		m_pTreeControl->ScrollToEnd();

		ShowChildDialog( m_pDescriptionDlg, false );
		ShowChildDialog( m_pRecompenseDlg, false );
		
		CEtUIQuestTreeItem *pItem = GetLatestJournalPage();

		if( pItem )
		{
			m_pTreeControl->SetSelectItem( pItem );
			m_nQuestIndex = (int)pItem->GetItemValueFloat();
			JournalPage* pPage = static_cast<JournalPage*>(pItem->GetItemData());

			bool bShow2 = m_pDescriptionDlg->SetJournalText( m_nQuestIndex, pPage );
			ShowChildDialog( m_pDescriptionDlg, bShow2 );
			SetQuestRecompenseDlg( m_nQuestIndex );
			
			ImportNotifierToCheckBox();
			m_pButtonTrace->Enable( true );
		}
		else
		{
			m_pButtonTrace->Enable( false );
		}
	}
	else
	{
		ShowChildDialog( m_pDescriptionDlg, false );
		ShowChildDialog( m_pRecompenseDlg, false );
		if( m_bSaveNotifierToServerNeeded ) {
			GetQuestTask().OnUpdateNotifier( true );
			m_bSaveNotifierToServerNeeded = false;
		}
	}

	if (CGameOption::IsActive() && m_pQuestShowLevelCheckBox)
	{
		m_pQuestShowLevelCheckBox->SetChecked(CGameOption::GetInstance().m_bHideQuestMarkByLevel);
	}

	if (CGameOption::IsActive() && m_pQuestNotifyPriority)
	{
		m_pQuestNotifyPriority->SetChecked(CGameOption::GetInstance().m_bEnableQuestNotifyPriority);
	}

	CEtUIDialog::Show( bShow );
}

CEtUIQuestTreeItem *CDnQuestDlg::GetLatestJournalPage()
{
	CEtUIQuestTreeItem *pItem = (CEtUIQuestTreeItem*)m_pTreeControl->GetEndItem();
	if( pItem )
	{
		while(1)
		{
			pItem = (CEtUIQuestTreeItem*)pItem->GetEndItem();
			if( !pItem ) break;

			std::wstring &strValue = pItem->GetItemValueString();
			if( strValue == L"JournalPage" )
			{
				return pItem;
			}
		}
	}
	
	return NULL;
}

void CDnQuestDlg::RefreshCompleteMainQuest()
{
	std::vector<Journal*> vecCompleteMainQuest;
	GetQuestTask().GetCompleteMainQuest( vecCompleteMainQuest );

	const Journal *pJournal(NULL);
	CEtUIQuestTreeItem *pRootItem(NULL);

	for( int i=0; i<(int)vecCompleteMainQuest.size(); ++i )
	{
		pJournal = vecCompleteMainQuest[i];
		if( !pJournal ) continue;

		CEtUIQuestTreeItem *pJournalItem = AddMainQuest( NULL, pJournal, CTreeItem::typeOpen|CTreeItem::typeComplete );
		if( !pJournalItem ) continue;

		// Note : 저널 추가
		//
		const Journal::JOURNALPAGE_VEC &vecJournalPage = pJournal->JounalPages;
		CEtUIQuestTreeItem *pJournalPageItem(NULL);

		for( int j=0; j<(int)vecJournalPage.size(); j++ )
		{
			pJournalPageItem = (CEtUIQuestTreeItem*)m_pTreeControl->AddQuestChildItem( pJournalItem, CTreeItem::typeOpen|CTreeItem::typeComplete, vecJournalPage[j].szTitle.c_str() );
			pJournalPageItem->SetItemData( (void*)&(vecJournalPage[j]) );				// 저널 페이지 포인터
			pJournalPageItem->SetItemValueInt( vecJournalPage[j].nJournalPageIndex );	// 저널 인덱스
			pJournalPageItem->SetItemValueFloat( (float)pJournal->nQuestIndex );
			pJournalPageItem->SetItemValueString( L"JournalPage" );
		}
	}
	Show(true);
}

void CDnQuestDlg::RefreshCompleteSubQuest()
{
	// 완료된 서브퀘스트는 추가하지 않는다.
	/*
	std::vector<Journal*> vecCompleteSubQuest;
	GetQuestTask().GetCompleteSubQuest( vecCompleteSubQuest );

	const Journal *pJournal(NULL);
	CEtUIQuestTreeItem *pRootItem(NULL);

	for( int i=0; i<(int)vecCompleteSubQuest.size(); ++i )
	{
		pJournal = vecCompleteSubQuest[i];
		if( !pJournal ) continue;

		CEtUIQuestTreeItem *pJournalItem = AddSubQuest( NULL, pJournal, CTreeItem::typeOpen|CTreeItem::typeComplete );
		if( !pJournalItem ) continue;

		// Note : 저널 추가
		//
		const Journal::JOURNALPAGE_VEC &vecJournalPage = pJournal->JounalPages;
		CEtUIQuestTreeItem *pJournalPageItem(NULL);

		for( int j=0; j<(int)vecJournalPage.size(); j++ )
		{
			pJournalPageItem = m_pTreeControl->AddQuestChildItem( pJournalItem, CTreeItem::typeOpen|CTreeItem::typeComplete, vecJournalPage[j].szTitle.c_str() );
			pJournalPageItem->SetItemData( (void*)&(vecJournalPage[j]) );				// 저널 페이지 포인터
			pJournalPageItem->SetItemValueInt( vecJournalPage[j].nJournalPageIndex );	// 저널 인덱스
			pJournalPageItem->SetItemValueFloat( (float)pJournal->nQuestIndex );
			pJournalPageItem->SetItemValueString( L"JournalPage" );
		}
	}	
	*/
	Show(true);
}

bool CDnQuestDlg::CompareSubQuestLevel( TQuest * pFirst, TQuest * pSecond )
{
	Journal *pFirstJournal = g_DataManager.GetJournalData( pFirst->nQuestID );
	Journal *pSecondJournal = g_DataManager.GetJournalData( pSecond->nQuestID );

	if( !pFirstJournal || !pSecondJournal )
		return false;

	if( pFirstJournal->nQuestLevel < pSecondJournal->nQuestLevel )
		return true;

	return false;
}

void CDnQuestDlg::RefreshProgQuest( const std::vector<TQuest*> &vecProgQuest )
{
	const TQuest *pQuest(NULL);

	for( int i=0; i<(int)vecProgQuest.size(); ++i )
	{
		pQuest = vecProgQuest[i];
		if( !pQuest ) continue;

		AddQuest( pQuest );
		AddJournal( pQuest, true );
	}
}

void CDnQuestDlg::RefreshProgMainQuest()
{
	std::vector<TQuest*> vecProgMainQuest;
	GetQuestTask().GetPlayMainQuest( vecProgMainQuest );
	RefreshProgQuest( vecProgMainQuest );
}

void CDnQuestDlg::RefreshProgSubQuest()
{
	std::vector<TQuest*> vecProgSubQuest;
	GetQuestTask().GetPlayQuest( EnumQuestType::QuestType_SubQuest, vecProgSubQuest );

	std::stable_sort( vecProgSubQuest.begin(), vecProgSubQuest.end(), CompareSubQuestLevel );
	RefreshProgQuest( vecProgSubQuest );
	SortSubQuest( vecProgSubQuest );
}

void CDnQuestDlg::RefreshCompletePeriodQuest()
{
	Show(true);
}

void CDnQuestDlg::RefreshProgPeriodQuest()
{
	std::vector<TQuest *> vecProgPeriodQuest;
	GetQuestTask().GetPlayQuest( EnumQuestType::QuestType_PeriodQuest, vecProgPeriodQuest );

	std::stable_sort( vecProgPeriodQuest.begin(), vecProgPeriodQuest.end(), CompareSubQuestLevel );
	RefreshProgQuest( vecProgPeriodQuest );
	SortSubQuest( vecProgPeriodQuest );
}

void CDnQuestDlg::AddQuest( const TQuest *pQuest )
{
	ASSERT(pQuest&&"CDnQuestDlg::AddQuest");

	if( pQuest->nQuestID == 0 )
		return;

	Journal *pJournal = g_DataManager.GetJournalData( pQuest->nQuestID );
	if( !pJournal ) return;

	if( m_emQuestType == typeMain )
		AddMainQuest( pQuest, pJournal, CTreeItem::typeOpen );
	else if( typeSub == m_emQuestType )
		AddSubQuest( pQuest, pJournal, CTreeItem::typeOpen );
	else if( typePeriod == m_emQuestType )
		AddSubQuest( pQuest, pJournal, CTreeItem::typeOpen );
#ifdef PRE_ADD_REMOTE_QUEST
	else if( typeRemote == m_emQuestType )
		AddRemoteQuest( pQuest, pJournal, CTreeItem::typeOpen );
#endif // PRE_ADD_REMOTE_QUEST
}

CEtUIQuestTreeItem *CDnQuestDlg::AddMainQuest( const TQuest *pQuest, const Journal *pJournal, DWORD dwType )
{
	CDnQuestTreeItem *pRootItem = (CDnQuestTreeItem*)m_pTreeControl->FindItemInt( pJournal->nChapterIndex, false );

	if( !pRootItem )
	{
		pRootItem = m_pTreeControl->AddQuestItem( CTreeItem::typeOpen, L"" );
		pRootItem->SetMainQuest( true );
		std::wstring szChapterStr;
		szChapterStr = g_DataManager.GetChapterString(pJournal->nChapterIndex);
		pRootItem->SetText( szChapterStr.c_str() );

		std::string szChapterImgFile = g_DataManager.GetChapterImageFileName(pJournal->nChapterIndex);
		pRootItem->SetTexture( LoadResource( CEtResourceMng::GetInstance().GetFullName( szChapterImgFile.c_str() ).c_str(), RT_TEXTURE ), 0, 0, 318, 43 );
		pRootItem->SetTextColor( EtInterface::textcolor::ORANGE );
		pRootItem->SetItemValueInt( pJournal->nChapterIndex );
		pRootItem->SetItemValueString( L"Chapter" );		
	}

	pRootItem->AddSubCount();

	DWORD dwColor = GetQuestColor( pJournal->nQuestLevel );

	pRootItem = m_pTreeControl->AddQuestChildItem( pRootItem, dwType, L"" );
	pRootItem->SetMainQuest( true );
	std::string szQuestImageFile = g_DataManager.GetQuestImageFileName(pJournal->nQuestIndex);
	pRootItem->SetTexture( LoadResource( CEtResourceMng::GetInstance().GetFullName( szQuestImageFile.c_str() ).c_str(), RT_TEXTURE ), 0, 0, 318, 43 );
	//pRootItem->SetItemData( (void*)pQuest );
	pRootItem->SetItemValueInt( pJournal->nQuestIndex );
	pRootItem->SetText( pJournal->wszQuestLevelTitle.c_str() );
	pRootItem->SetTextColor( dwColor );
	pRootItem->SetItemValueString( L"Journal" );
	if(!GetQuestTask().IsClearQuest(pJournal->nQuestIndex))
		pRootItem->SetCheckBox( ((CDnQuestTree*)m_pTreeControl)->CreateCheckBox( pJournal->nQuestIndex, pRootItem ) );

	return pRootItem;
}

CEtUIQuestTreeItem *CDnQuestDlg::AddSubQuest( const TQuest *pQuest, const Journal *pJournal, DWORD dwType )
{
	QuestInfo* pQuestInfo = g_DataManager.FindQuestInfo( pQuest->nQuestID );
	CDnQuestTreeItem *pRootItem = NULL;

	if( pQuestInfo )
	{
#ifdef PRE_ADD_QUEST_SORT_BY_JOURNAL_MAP // 저널의 목적지가 변동된다면 MapIndex를 동적으로 설정합니다. 
		if( pJournal && pQuest )
		{
			const JournalPage *pPage = const_cast<Journal*>(pJournal)->FindJournalPage( (int)pQuest->cQuestJournal );
			if( pPage )
			{
				if( pPage->nDestnationMapIndex > 0 )
				{
					pQuestInfo->nMapIndex = pPage->nDestnationMapIndex;
				}
				else if( pQuestInfo->nMapIndex == 0 )
				{
					DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TQUEST );
					pQuestInfo->nMapIndex =  pSox->GetFieldFromLablePtr( pQuest->nQuestID, "_Map" )->GetInteger();
				}
			}
		}

		pRootItem = FindMatchedQuestTreeItem( pQuestInfo->nMapIndex );
#else
		pRootItem = (CDnQuestTreeItem*)m_pTreeControl->FindItemInt( pQuestInfo->nMapIndex, false );
#endif	

		if( !pRootItem )
		{
   			DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TMAP );

			pRootItem = m_pTreeControl->AddQuestItem( CTreeItem::typeOpen, L"" );
			pRootItem->SetMainQuest( false );
			pRootItem->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, pSox->GetFieldFromLablePtr( pQuestInfo->nMapIndex, "_MapNameID" )->GetInteger() ) );

			std::string szChapterImgFile = g_DataManager.GetQuestImageFileName( pJournal->nChapterIndex );
			pRootItem->SetTexture( LoadResource( CEtResourceMng::GetInstance().GetFullName( szChapterImgFile.c_str() ).c_str(), RT_TEXTURE ), 0, 0, 318, 43 );
			pRootItem->SetTextColor( EtInterface::textcolor::ORANGE );
			pRootItem->SetItemValueInt( pQuestInfo->nMapIndex );
			pRootItem->SetItemValueString( L"nMapIndex" );
		}
	}

	if( pRootItem )
	{
		pRootItem->AddSubCount();
		pRootItem = m_pTreeControl->AddQuestChildItem( pRootItem, dwType, L"" );
	}
	else
		pRootItem = m_pTreeControl->AddQuestItem( dwType, L"" );

	
	pRootItem->SetMainQuest( false );

	std::string szQuestImageFile = g_DataManager.GetQuestImageFileName( pJournal->nQuestIndex );
	pRootItem->SetTexture( LoadResource( CEtResourceMng::GetInstance().GetFullName( szQuestImageFile.c_str() ).c_str(), RT_TEXTURE ), 0, 0, 342, 50 );

	//pRootItem->SetItemData( (void*)pQuest );

	DWORD dwColor = GetQuestColor( pJournal->nQuestLevel );

	pRootItem->SetItemValueInt( pJournal->nQuestIndex );
	pRootItem->SetText( pJournal->wszQuestLevelTitle.c_str() );	
	pRootItem->SetTextColor( dwColor );
	pRootItem->SetItemValueString( L"Journal" );
	pRootItem->SetCheckBox( ((CDnQuestTree*)m_pTreeControl)->CreateCheckBox( pJournal->nQuestIndex, NULL ) );

	return pRootItem;
}

#ifdef PRE_ADD_REMOTE_QUEST
CEtUIQuestTreeItem* CDnQuestDlg::AddRemoteQuest( const TQuest* pQuest, const Journal* pJournal, DWORD dwType )
{
	if( pQuest == NULL || pJournal == NULL )
		return NULL;

	SetRemoteQuest( pQuest->nQuestID, (int)pQuest->cQuestJournal, dwType );
	((CDnQuestTree*)m_pTreeControl)->SetRemoteQuestState( pJournal->nQuestIndex, CDnQuestTree::REMOTEQUEST_PROGRESSING );
	return NULL;
}

CDnQuestTreeItem* CDnQuestDlg::SetRemoteQuest( int nQuestIndex, int nJournalPageIndex, DWORD dwType )
{
	QuestInfo* pQuestInfo = g_DataManager.FindQuestInfo( nQuestIndex );
	Journal* pJournal = g_DataManager.GetJournalData( nQuestIndex );

	if( pJournal == NULL )
	{
		if( g_DataManager.LoadQuestData( nQuestIndex ) )
			pJournal = g_DataManager.GetJournalData( nQuestIndex );
	}

	if( pQuestInfo == NULL || pJournal == NULL )
		return NULL;

	CDnQuestTreeItem* pRootItem = NULL;

#ifdef PRE_ADD_QUEST_SORT_BY_JOURNAL_MAP // 저널의 목적지가 변동된다면 MapIndex를 동적으로 설정합니다. 
	const JournalPage *pPage = const_cast<Journal*>(pJournal)->FindJournalPage( nJournalPageIndex );
	if( pPage )
	{
		if( pPage->nDestnationMapIndex > 0 )
		{
			pQuestInfo->nMapIndex = pPage->nDestnationMapIndex;
		}
		else if( pQuestInfo->nMapIndex == 0 )
		{
			DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TQUEST );
			pQuestInfo->nMapIndex =  pSox->GetFieldFromLablePtr( nQuestIndex, "_Map" )->GetInteger();
		}
	}

	pRootItem = FindMatchedQuestTreeItem( pQuestInfo->nMapIndex );
#else
	pRootItem = (CDnQuestTreeItem*)m_pTreeControl->FindItemInt( pQuestInfo->nMapIndex, false );
#endif	

	if( !pRootItem )
	{
		DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TMAP );

		pRootItem = m_pTreeControl->AddQuestItem( CTreeItem::typeOpen, L"" );
		pRootItem->SetMainQuest( false );
		pRootItem->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, pSox->GetFieldFromLablePtr( pQuestInfo->nMapIndex, "_MapNameID" )->GetInteger() ) );

		std::string szChapterImgFile = g_DataManager.GetQuestImageFileName( pJournal->nChapterIndex );
		pRootItem->SetTexture( LoadResource( CEtResourceMng::GetInstance().GetFullName( szChapterImgFile.c_str() ).c_str(), RT_TEXTURE ), 0, 0, 318, 43 );
		pRootItem->SetTextColor( EtInterface::textcolor::ORANGE );
		pRootItem->SetItemValueInt( pQuestInfo->nMapIndex );
		pRootItem->SetItemValueString( L"nMapIndex" );
	}

	if( pRootItem )
	{
		pRootItem->AddSubCount();
		pRootItem = m_pTreeControl->AddQuestChildItem( pRootItem, dwType, L"" );
	}
	else
		pRootItem = m_pTreeControl->AddQuestItem( dwType, L"" );


	pRootItem->SetMainQuest( false );

	std::string szQuestImageFile = g_DataManager.GetQuestImageFileName( pJournal->nQuestIndex );
	pRootItem->SetTexture( LoadResource( CEtResourceMng::GetInstance().GetFullName( szQuestImageFile.c_str() ).c_str(), RT_TEXTURE ), 0, 0, 342, 50 );

	DWORD dwColor = GetQuestColor( pJournal->nQuestLevel );
	pRootItem->SetItemValueInt( pJournal->nQuestIndex );
	pRootItem->SetText( pJournal->wszQuestLevelTitle.c_str() );	
	pRootItem->SetTextColor( dwColor );
	pRootItem->SetItemValueString( L"Journal" );
	pRootItem->SetCheckBox( ((CDnQuestTree*)m_pTreeControl)->CreateCheckBox( pJournal->nQuestIndex, NULL ) );

	return pRootItem;
}

CDnQuestTreeItem* CDnQuestDlg::GetRemoteQuestTreeItem( int nQuestIndex )
{
	CDnQuestTreeItem* pQuestTreeItem = NULL;
	QuestInfo* pQuestInfo = g_DataManager.FindQuestInfo( nQuestIndex );
	if( pQuestInfo == NULL )
		return pQuestTreeItem;

	CDnQuestTreeItem* pRootItem = FindMatchedQuestTreeItem( pQuestInfo->nMapIndex );
	if( pRootItem )
		pQuestTreeItem = dynamic_cast<CDnQuestTreeItem*>( pRootItem->FindItemInt( nQuestIndex ) );

	return pQuestTreeItem;
}

void CDnQuestDlg::RecieveRemoteQuest( int nQuestIndex, CDnQuestTree::eRemoteQuestState remoteQuestState )
{
	CDnQuestTreeItem* pRootItem = NULL;
	if( remoteQuestState == CDnQuestTree::REMOTEQUEST_ASK || remoteQuestState == CDnQuestTree::REMOTEQUEST_PROGRESSING )
	{
		pRootItem = SetRemoteQuest( nQuestIndex, 1, CTreeItem::typeOpen );
	}
	else if( remoteQuestState == CDnQuestTree::REMOTEQUEST_COMPLETE )
	{
		pRootItem = GetRemoteQuestTreeItem( nQuestIndex );
	}

	if( pRootItem )
		pRootItem->SetRemoteButton( ((CDnQuestTree*)m_pTreeControl)->CreateRemoteButton( nQuestIndex ) );

	((CDnQuestTree*)m_pTreeControl)->SetRemoteQuestState( nQuestIndex, remoteQuestState );

	std::vector<TQuest*> vecProgRemoteQuest;
	GetQuestTask().GetPlayQuest( EnumQuestType::QuestType_RemoteQuest, vecProgRemoteQuest );
	std::vector<CDnQuestTask::stRemoteQuestAsk>& vecRemoteQuestAskList = GetQuestTask().GetRemoteQuestAskList();
	if( vecProgRemoteQuest.empty() && vecRemoteQuestAskList.empty() )
	{
		CONTROL( Static, ID_TEXT1 )->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 437) );	// 현재 진행 중인 퀘스트가 없습니다.
		CONTROL( Static, ID_TEXT1 )->Show( true );
	}
	else
		CONTROL( Static, ID_TEXT1 )->Show( false );
}

void CDnQuestDlg::RefreshCompleteRemoteQuest()
{
	Show( true );
}

void CDnQuestDlg::RefreshProgRemoteQuest()
{
	std::vector<TQuest*> vecProgRemoteQuest;
	GetQuestTask().GetPlayQuest( EnumQuestType::QuestType_RemoteQuest, vecProgRemoteQuest );
	RefreshProgQuest( vecProgRemoteQuest );

	std::vector<TQuest*> vecRecompenseRemoteQuest;
	GetQuestTask().GetRecompenseRemoteQuest( EnumQuestType::QuestType_RemoteQuest, vecRecompenseRemoteQuest );
	RefreshProgQuest( vecRecompenseRemoteQuest );

	std::vector<CDnQuestTask::stRemoteQuestAsk>& vecRemoteQuestAskList = GetQuestTask().GetRemoteQuestAskList();
	for( int i=0; i<static_cast<int>( vecRemoteQuestAskList.size() ); i++ )
	{
		if( vecRemoteQuestAskList[i].remoteQuestState == CDnQuestTree::REMOTEQUEST_ASK )
		{
			TQuest tQuest;
			memset( &tQuest, 0 , sizeof(TQuest) );
			tQuest.nQuestID = vecRemoteQuestAskList[i].nQuestIndex;

			Journal* pJournal = g_DataManager.GetJournalData( tQuest.nQuestID );
			if( pJournal == NULL )
				g_DataManager.LoadQuestData( tQuest.nQuestID );

			AddQuest( &tQuest );
			AddJournal( &tQuest, true );
		}

		CDnQuestTreeItem* pRootItem = GetRemoteQuestTreeItem( vecRemoteQuestAskList[i].nQuestIndex );
		if( pRootItem )
			pRootItem->SetRemoteButton( ((CDnQuestTree*)m_pTreeControl)->CreateRemoteButton( vecRemoteQuestAskList[i].nQuestIndex ) );

		((CDnQuestTree*)m_pTreeControl)->SetRemoteQuestState( vecRemoteQuestAskList[i].nQuestIndex, vecRemoteQuestAskList[i].remoteQuestState );
	}

	if( vecProgRemoteQuest.empty() && vecRecompenseRemoteQuest.empty() && vecRemoteQuestAskList.empty() )
	{
		CONTROL( Static, ID_TEXT1 )->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 437) );	// 현재 진행 중인 퀘스트가 없습니다.
		CONTROL( Static, ID_TEXT1 )->Show( true );
	}
	else
		CONTROL( Static, ID_TEXT1 )->Show( false );

	vecProgRemoteQuest.clear();
	vecRecompenseRemoteQuest.clear();
}
#endif // PRE_ADD_REMOTE_QUEST

void CDnQuestDlg::DeleteQuest( const TQuest *pQuest )
{
	ASSERT(pQuest&&"CDnQuestDlg::DeleteQuest");

	if( pQuest->nQuestID == 0 )	return;

	if( m_emQuestType == typeMain )	
	{
		DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TQUEST);
		int nQuestTypeIndex = -1; // 퀘스트 타입
		nQuestTypeIndex = pSox->GetFieldFromLablePtr( pQuest->nQuestID, "_QuestType" )->GetInteger();

		if(nQuestTypeIndex != EnumQuestType::QuestType_MainQuest)
			return;

		GetQuestTask().ClearMainQuestFromQuestID(pQuest->nQuestID);

		Clear();
		RefreshCompleteMainQuest();
		RefreshProgMainQuest();
		
		m_pTreeControl->ResetSelectedItem();
		m_pTreeControl->SelectLastTree();

		CTreeItem *pChildItem = m_pTreeControl->GetSelectedItem();
		if(pChildItem)
			m_pTreeControl->ExpandAllChildren(pChildItem);

		return;
	}

	CTreeItem *pQuestItem = NULL;

	QuestInfo* pQuestInfo = g_DataManager.FindQuestInfo( pQuest->nQuestID );
	if( !pQuestInfo ) return;

#ifdef PRE_ADD_QUEST_SORT_BY_JOURNAL_MAP
	CEtUIQuestTreeItem *pMapItem = FindMatchedQuestTreeItem( pQuestInfo->nMapIndex );
#else
	CEtUIQuestTreeItem *pMapItem = (CEtUIQuestTreeItem*)m_pTreeControl->FindItemInt( pQuestInfo->nMapIndex, false );
#endif
	
	if( !pMapItem) return;

	pMapItem->Expand();
	pQuestItem = (CEtUIQuestTreeItem*)pMapItem->FindItemInt( pQuest->nQuestID, false );

	if( !pQuestItem ) return;

	m_pTreeControl->DeleteItem( pQuestItem );
	m_pTreeControl->ResetSelectedItem();

#ifdef PRE_ADD_REMOTE_QUEST
	if( typeSub == m_emQuestType || typePeriod == m_emQuestType || typeRemote == m_emQuestType )
#else // PRE_ADD_REMOTE_QUEST
	if( typeSub == m_emQuestType || typePeriod == m_emQuestType )
#endif // PRE_ADD_REMOTE_QUEST
	{
		// 퀘스트가 지워지고 상위 목표지역 내에 아무것도 없다면 상위 목표 지역도 삭제 한다.
		TREEITEM_LIST& listItem = pMapItem->GetTreeItemList();
		int nCount = (int)listItem.size();
		if( 0 == nCount )
			m_pTreeControl->DeleteItem( pMapItem );
		else
			pMapItem->DelSubCount();
	}

	if( NULL == m_pTreeControl->GetBeginItem() )
	{
		CONTROL( Static, ID_TEXT1 )->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 437) );	// 현재 진행 중인 퀘스트가 없습니다.
		CONTROL( Static, ID_TEXT1 )->Show( true );
	}
	else
		CONTROL( Static, ID_TEXT1 )->Show( false );

	// 트리 젤 마지막 노드를 선택한다.
	m_pTreeControl->SelectLastTree();
	OnTreeSelected();

	// 현재 선택된게 없다면 깨끗이 세팅해주자.
	CEtUIQuestTreeItem *pItem = static_cast<CEtUIQuestTreeItem*>(m_pTreeControl->GetSelectedItem());
	if( !pItem ) 
	{
		ShowChildDialog( m_pDescriptionDlg, false );
		ShowChildDialog( m_pRecompenseDlg, false );
		return;
	}
	
	// 갱신
	RefreshEnableNotifierCheck();
}

void CDnQuestDlg::AddJournal( const TQuest *pQuest, bool bAll )
{
	ASSERT(pQuest&&"CDnQuestDlg::AddJournal");

	Journal *pJournal = g_DataManager.GetJournalData( pQuest->nQuestID );
	if( !pJournal ) return;

	QuestInfo* pQuestInfo = g_DataManager.FindQuestInfo( pQuest->nQuestID );
	if( !pQuestInfo )	return;

	CEtUIQuestTreeItem *pJournalItem = FindJournal( pJournal->nChapterIndex, pJournal->nQuestIndex, pQuestInfo->nMapIndex );
	if( !pJournalItem ) return;

	// 자식 다 지워주고
	pJournalItem->DeleteAll();
	Journal::JOURNALPAGE_VEC &vecJournalPage = pJournal->JounalPages;

	CEtUIQuestTreeItem *pJournalPageItem(NULL);

	if( bAll )
	{
		int nCurJournalIndex = pQuest->cQuestJournal;
		int nSize = min((int)vecJournalPage.size(), nCurJournalIndex);

		for( int i=0; i< nSize ; i++ )
		{
			pJournalPageItem = m_pTreeControl->AddQuestChildItem( pJournalItem, CTreeItem::typeOpen|CTreeItem::typeComplete, vecJournalPage[i].szTitle.c_str() );
			pJournalPageItem->SetItemData( (void*)&(vecJournalPage[i]) );				// 저널 페이지 포인터
			pJournalPageItem->SetItemValueInt( vecJournalPage[i].nJournalPageIndex );	// 저널 인덱스
			pJournalPageItem->SetItemValueFloat( (float)pJournal->nQuestIndex );		// 퀘스트 인덱스
			pJournalPageItem->SetItemValueString( L"JournalPage" );
		}

		if( pJournalPageItem )
			pJournalPageItem->Complete( false );
	}
	else
	{
		bool bFindJournalPage(false);

		for( int i=0; i<(int)vecJournalPage.size(); i++ )
		{
			if( vecJournalPage[i].nJournalPageIndex > pQuest->cQuestJournal )
			{
				break;
			}
			m_pTreeControl->AddItemTypeChildren( pJournalItem, CTreeItem::typeComplete );

			pJournalPageItem = m_pTreeControl->AddQuestChildItem( pJournalItem, CTreeItem::typeOpen, vecJournalPage[i].szTitle.c_str() );
			pJournalPageItem->SetItemData( (void*)&(vecJournalPage[i]) );
			pJournalPageItem->SetItemValueInt( vecJournalPage[i].nJournalPageIndex );
			pJournalPageItem->SetItemValueFloat( (float)pJournal->nQuestIndex );		// 퀘스트 인덱스
			pJournalPageItem->SetItemValueString( L"JournalPage" );
		}
	}

	if( pJournalPageItem )
	{
		m_pTreeControl->CollapseAll();
		Expand( pJournal->nChapterIndex, pJournal->nQuestIndex );
	}
	else
	{
		CDebugSet::ToLogFile( "CDnQuestDlg::AddJournal, Quest Journal Index(%d), JournalPage is none!", pQuest->cQuestJournal );
	}
}

void CDnQuestDlg::RefreshJournal( const TQuest *pQuest )
{
	ASSERT(pQuest&&"CDnQuestDlg::RefreshJournal");

	if( !m_pDescriptionDlg->IsShow() )
		return;

	CEtUIQuestTreeItem *pTreeItem = static_cast<CEtUIQuestTreeItem*>(m_pTreeControl->GetSelectedItem());
	if( !pTreeItem ) return;

	Journal *pJournal = g_DataManager.GetJournalData( pQuest->nQuestID );
	if( !pJournal ) return;

	CEtUIQuestTreeItem *pJournalPage = FindJournalPage( pJournal->nChapterIndex, pJournal->nQuestIndex, pQuest->cQuestJournal );
	if( !pJournalPage ) return;

	m_pDescriptionDlg->SetJournalText( pQuest->nQuestID, (JournalPage*)pTreeItem->GetItemData() );
}

void CDnQuestDlg::Clear()
{
	m_pTreeControl->DeleteAllItems();
}

CEtUIQuestTreeItem *CDnQuestDlg::FindJournal( int nChapterIndex, int nQuestIndex, int nMapIndex )
{
	CEtUIQuestTreeItem *pTreeItem(NULL);

	if( m_emQuestType == typeMain )
	{
		CEtUIQuestTreeItem *pChapter = (CEtUIQuestTreeItem*)m_pTreeControl->FindItemInt( nChapterIndex, false );
		if( !pChapter ) return NULL;

		pTreeItem = (CEtUIQuestTreeItem*)pChapter->FindItemInt( nQuestIndex, false );
		if( !pTreeItem ) return NULL;
	}
	else
	{
#ifdef PRE_ADD_QUEST_SORT_BY_JOURNAL_MAP
		CEtUIQuestTreeItem *pMapItem = FindMatchedQuestTreeItem( nMapIndex );
#else
		CEtUIQuestTreeItem *pMapItem = (CEtUIQuestTreeItem*)m_pTreeControl->FindItemInt( nMapIndex, false );
#endif
		if( !pMapItem) return NULL;

		pTreeItem = (CEtUIQuestTreeItem*)pMapItem->FindItemInt( nQuestIndex, false );
		if( !pTreeItem ) return NULL;
	}

	return pTreeItem;
}

CEtUIQuestTreeItem *CDnQuestDlg::FindJournalPage( int nChapterIndex, int nQuestIndex, int nJournalPageIndex )
{
	QuestInfo* pQuestInfo = g_DataManager.FindQuestInfo( nQuestIndex );
	if( !pQuestInfo )	return NULL;

	CEtUIQuestTreeItem *pJournalItem = FindJournal( nChapterIndex, nQuestIndex, pQuestInfo->nMapIndex );
	if( !pJournalItem ) return NULL;

	return (CEtUIQuestTreeItem*)pJournalItem->FindItemInt( nJournalPageIndex, false );
}

CEtUIQuestTreeItem* CDnQuestDlg::Expand( int nChapterIndex, int nQuestIndex )
{
	CEtUIQuestTreeItem *pJournalItem(NULL);

	if( m_emQuestType == typeMain )
	{
		CEtUIQuestTreeItem *pChapter = (CEtUIQuestTreeItem*)m_pTreeControl->FindItemInt( nChapterIndex, false );
		if( !pChapter ) return NULL;

		pChapter->Expand();

		pJournalItem = (CEtUIQuestTreeItem*)pChapter->FindItemInt( nQuestIndex, false );
		if( !pJournalItem ) return NULL ;
	}
	else
	{
		QuestInfo* pQuestInfo = g_DataManager.FindQuestInfo( nQuestIndex );
		if( !pQuestInfo ) return NULL;

#ifdef PRE_ADD_QUEST_SORT_BY_JOURNAL_MAP
		CEtUIQuestTreeItem *pMapItem = FindMatchedQuestTreeItem( pQuestInfo->nMapIndex );
#else
		CEtUIQuestTreeItem *pMapItem = (CEtUIQuestTreeItem*)m_pTreeControl->FindItemInt( pQuestInfo->nMapIndex, false );
#endif
		if( !pMapItem) return NULL;

		pMapItem->Expand();

		pJournalItem = (CEtUIQuestTreeItem*)pMapItem->FindItemInt( nQuestIndex, false );
		if( !pJournalItem ) return NULL;
	}

	m_pTreeControl->ExpandAllChildren( pJournalItem );	
	return pJournalItem;
}

void CDnQuestDlg::CompleteQuest( const TQuest *pQuest )
{
	ASSERT(pQuest&&"CDnQuestDlg::CompleteQuest");

	Journal *pJournal = g_DataManager.GetJournalData( pQuest->nQuestID );
	if( !pJournal ) return;	

	QuestInfo* pQuestInfo = g_DataManager.FindQuestInfo( pQuest->nQuestID );
	if( !pQuestInfo )	return;

	CEtUIQuestTreeItem *pJournalItem = FindJournal( pJournal->nChapterIndex, pJournal->nQuestIndex, pQuestInfo->nMapIndex );
	if( !pJournalItem ) return;

	if ( !pJournal->IsMainQuest() && GetQuestTask().IsClearQuest(pJournal->nQuestIndex) )	// 완료된 서브 퀘스트는 지워준다.
	{
		m_pTreeControl->DeleteItem( pJournalItem );

#ifdef PRE_ADD_QUEST_SORT_BY_JOURNAL_MAP
		CEtUIQuestTreeItem *pParentItem = FindMatchedQuestTreeItem( pQuestInfo->nMapIndex );
#else
		CEtUIQuestTreeItem *pParentItem = (CEtUIQuestTreeItem*)m_pTreeControl->FindItemInt( pQuestInfo->nMapIndex, false );
#endif
		pParentItem->DelSubCount();

		if( pParentItem && 0 == pParentItem->GetSubCount() )
			m_pTreeControl->DeleteItem( pParentItem );

		if( NULL == m_pTreeControl->GetBeginItem() )
		{
			CONTROL( Static, ID_TEXT1 )->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 437) );	// 현재 진행 중인 퀘스트가 없습니다.
			CONTROL( Static, ID_TEXT1 )->Show( true );
		}
		return;
	}

	m_pTreeControl->AddItemTypeChildren( pJournalItem, CTreeItem::typeComplete );
	pJournalItem->AddItemType( CTreeItem::typeComplete );

	// 자식 다 지워주고
	pJournalItem->DeleteAll();
	Journal::JOURNALPAGE_VEC &vecJournalPage = pJournal->JounalPages;

	const JournalPage *pJournalPage(NULL);
	CEtUIQuestTreeItem *pJournalPageItem(NULL);

	int nSize = (int)vecJournalPage.size();

	for( int i=0; i< nSize ; i++ )
	{
		pJournalPageItem = m_pTreeControl->AddQuestChildItem( pJournalItem, CTreeItem::typeOpen|CTreeItem::typeComplete, vecJournalPage[i].szTitle.c_str() );
		pJournalPageItem->SetItemData( (void*)&(vecJournalPage[i]) );				// 저널 페이지 포인터
		pJournalPageItem->SetItemValueInt( vecJournalPage[i].nJournalPageIndex );	// 저널 인덱스
		pJournalPageItem->SetItemValueFloat( (float)pJournal->nQuestIndex );		// 퀘스트 인덱스
		pJournalPageItem->SetItemValueString( L"JournalPage" );
		pJournalPageItem->Complete( true );
	}
}

void CDnQuestDlg::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	switch( nID ) 
	{
	case QUEST_DESCRIPTION_DIALOG:
		{
			if( nCommand == EVENT_BUTTON_CLICKED )
			{
				ShowChildDialog( m_pDescriptionDlg, false );
			}
		}
		break;
	case QUEST_RECOMPENSE_DIALOG:
		{
			if( nCommand == EVENT_BUTTON_CLICKED )
			{
				ShowChildDialog( m_pRecompenseDlg, false );
			}
		}
		break;
	case MESSAGEBOX_CONFIRM_REMOVEQUEST:
		{
			if( strcmp( pControl->GetControlName(), "ID_YES" ) == 0 )
				GetQuestTask().CancelQuest( m_nQuestIndex );
		}
		break;
	}
}

bool CDnQuestDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !m_bShow )
	{
		return false;
	}

	POINT MousePoint;
	float fMouseX, fMouseY;
	MousePoint.x = short( LOWORD( lParam ) );
	MousePoint.y = short( HIWORD( lParam ) );
	PointToFloat( MousePoint, fMouseX, fMouseY );

	switch( uMsg )
	{
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_XBUTTONDOWN:
	case WM_XBUTTONUP:
	case WM_LBUTTONDBLCLK:
	case WM_MBUTTONDBLCLK:
	case WM_RBUTTONDBLCLK:
	case WM_XBUTTONDBLCLK:
	case WM_MOUSEWHEEL:
	
		CEtUIControl* pControl = GetControlAtPoint( fMouseX, fMouseY );
		if (pControl && pControl->IsShow())
		{
			if( pControl->HandleMouse( uMsg, fMouseX, fMouseY, wParam, lParam ) )
			{
				return true;
			}
		}
	}

	return BaseClass::MsgProc( hWnd, uMsg, wParam, lParam );
}

void CDnQuestDlg::ImportNotifierToCheckBox()
{
	std::vector< CEtUICheckBox* > &vecList = m_pTreeControl->GetCheckBoxList();
	for each( CEtUICheckBox *pCheckBox in vecList )  {
		int nQuestIndex = pCheckBox->GetButtonID();
		if( nQuestIndex <= 0 ) continue;
		
		bool bIsRegisterd =  ( m_emQuestType == typeMain ) ? GetQuestTask().IsMainQuestNotifierExist() : 
																					GetQuestTask().IsRegisterQuestNotifier( DNNotifier::Type::SubQuest , nQuestIndex );

		pCheckBox->SetChecked( bIsRegisterd, false );
		
	}
	RefreshEnableNotifierCheck();		
}

void CDnQuestDlg::RefreshEnableNotifierCheck()
{
	if( m_emQuestType == typeMain ) return;

	std::vector< CEtUICheckBox* > &vecList = m_pTreeControl->GetCheckBoxList();
	
	bool bIsFull = GetQuestTask().IsSubQuestNotifierFull();

	if( bIsFull ) {
		for each( CEtUICheckBox *pCheckBox  in vecList )  {
			if( !pCheckBox->IsChecked() ) {
				pCheckBox->Enable( false );
				pCheckBox->SetTooltipText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 421 ) );
			}
#ifdef PRE_MOD_QUEST_NOTIFIER_FULLCHECK
			else {
				pCheckBox->Enable( true );
				pCheckBox->SetTooltipText(L"");
			}
#endif
		}
	}
	else {
		for each( CEtUICheckBox *pCheckBox in vecList )  {
			pCheckBox->Enable( true );
			pCheckBox->SetTooltipText(L"");
		}
	}
}

void CDnQuestDlg::CollapseAll()
{
	m_pTreeControl->CollapseAll();
}

void CDnQuestDlg::SetSelectItem( CEtUIQuestTreeItem *pItem )
{
	m_pTreeControl->SetSelectItem( pItem );
}

DWORD CDnQuestDlg::GetQuestColor( int nLevel )
{
	DWORD dwColor = textcolor::WHITE;

	int nMyLevel = CDnBridgeTask::GetInstance().GetCurrentCharLevel();

	if( nMyLevel >= nLevel + 10 )
		dwColor = textcolor::DARKGRAY;
	else if( nMyLevel <= nLevel + 9 && nLevel + 3 <= nMyLevel )
		dwColor = textcolor::FONT_GREEN;
	else if( nMyLevel <= nLevel + 2 && nLevel - 1 <= nMyLevel )
		dwColor = textcolor::YELLOW;
	else if( nMyLevel <= nLevel - 2 && nLevel - 4 <= nMyLevel )
		dwColor = textcolor::ORANGERED;
	else if( nMyLevel <= nLevel - 5 )
		dwColor = textcolor::RED;

	return dwColor;
}

void CDnQuestDlg::SortSubQuest( std::vector<TQuest*> & vecProgQuest )
{
	if( vecProgQuest.empty() )
	{
		CONTROL( Static, ID_TEXT1 )->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 437) );	// 현재 진행 중인 퀘스트가 없습니다.
		CONTROL( Static, ID_TEXT1 )->Show( true );
	}
	else
		CONTROL( Static, ID_TEXT1 )->Show( false );

	// '기타'를 가장 아래에 두도록 한다.
	CTreeItem * pItem = m_pTreeControl->FindItemInt( LAST_SUB_QEUST_ID );
	CTreeItem * pLastItem = m_pTreeControl->GetEndItem();

	if( NULL == pItem || NULL == pLastItem )
		return;

	m_pTreeControl->SetExceptLastSort( LAST_SUB_QEUST_ID );
	m_pTreeControl->UpdateRects();
}

void CDnQuestDlg::SetLevelCapRecompense( const TQuestRecompense & questRecompese )
{
	if( !CDnActor::s_hLocalActor )
		return;

	CDnLocalPlayerActor * pLocalActor = dynamic_cast<CDnLocalPlayerActor *>( CDnActor::s_hLocalActor.GetPointer() );
	if( !pLocalActor )
		return;

	if( pLocalActor->GetLevel() < (int)CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::PlayerLevelLimit) )
		return;

	QuestInfo* pQuestInfo = g_DataManager.FindQuestInfo( questRecompese.nQuestIndex );
	if( NULL == pQuestInfo )
		return;

	if( false == pQuestInfo->bLevelCapReward )
		return;

	std::vector<int> vlJobHistory;
	pLocalActor->GetJobHistory( vlJobHistory );
	int iFirstJob = (int)vlJobHistory.front();

	TQuestLevelCapRecompense questLevelCapRecompense;
	if( false == g_DataManager.GetQuestLevelCapRecompense( pQuestInfo->cQuestType, iFirstJob, questLevelCapRecompense ) )
		return;

	m_pRecompenseDlg->SetLevelCapRecompense( questRecompese, questLevelCapRecompense );
}

void CDnQuestDlg::SetQuestRecompenseDlg( int nQuestIndex )
{
	Journal *pJournal = g_DataManager.GetJournalData( m_nQuestIndex );
	if( !pJournal ) return;
	if( !CDnActor::s_hLocalActor ) return;

	int nRecompenseID = pJournal->GetMyRecompenseID( CDnActor::s_hLocalActor->GetClassID() );
	if( nRecompenseID != 0 )
	{
		TQuestRecompense table;
		g_DataManager.GetQuestRecompense( nRecompenseID, table);
		m_pRecompenseDlg->SetRecompense( table );
		SetLevelCapRecompense( table );
		ShowChildDialog( m_pRecompenseDlg, true );
	}
}

CDnQuestTreeItem* CDnQuestDlg::FindMatchedQuestTreeItem( int nMapIndex )
{
	CDnQuestTreeItem *pRootItem = NULL;

	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TMAP );
	DNTableCell* pField = pSox->GetFieldFromLablePtr( nMapIndex , "_MapNameID");
	int nMapTableStringId = pField->GetInteger();
	const std::vector<int>* pMapGroupList = CDnWorldDataManager::GetInstance().GetMapGroupByName( nMapTableStringId );
	if( pMapGroupList )
	{
		std::vector<int>::const_iterator iter = pMapGroupList->begin();
		for( iter = pMapGroupList->begin(); iter != pMapGroupList->end(); ++iter )
		{
			pRootItem = (CDnQuestTreeItem*)m_pTreeControl->FindItemInt( (*iter) , false );
			if( pRootItem )
			{
				break;
			}
		}
	}
	else
	{
		pRootItem =  (CDnQuestTreeItem*)m_pTreeControl->FindItemInt( nMapIndex, false );
	}

	return pRootItem;
}