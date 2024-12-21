#include "StdAfx.h"
#ifdef PRE_MOD_ITEM_COMPOUND_DIALOG_RENEWAL

#include "DnItemCompoundRenewalTab.h"
#include "DnCompoundBase.h"
#include "DnItemCompoundListDlg.h"
#include "DnItemTask.h"
#include "DnLocalPlayerActor.h"
#include "DnMainMenuDlg.h"
#ifdef PRE_ADD_SHORTCUT_HELP_DIALOG
#include "DnSystemDlg.h"
#endif


#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif 

CDnItemCompoundRenewalTabDlg::CDnItemCompoundRenewalTabDlg( UI_DIALOG_TYPE dialogType , CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback ) 
: CEtUITabDialog( dialogType, pParentDialog, nID, pCallback, true )
, m_pItemCompoundMsgBox( NULL )
, m_nSelectEquipType( 0 )
, m_nSelectSortType( 0 )
, m_nSelectSetItemType( 0 )
, m_nCompoundMainCategory( 0 )
, m_nRemoteItemID( 0 )
, m_bForceOpenMode( false ) 
, m_pCurrentCompoundListDlg( NULL )
, m_pListBoxEx( NULL )
{
	for( int i = 0; i < 4; ++i )
		m_pTabButton[i] = NULL;
}
CDnItemCompoundRenewalTabDlg::~CDnItemCompoundRenewalTabDlg()
{
	m_pListBoxEx->RemoveAllItems();
	SAFE_DELETE( m_pItemCompoundMsgBox );
}

void CDnItemCompoundRenewalTabDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "ItemCompound_BaseDlg.ui" ).c_str(), bShow );
}

void CDnItemCompoundRenewalTabDlg::InitialUpdate()
{
	char szControlName[32];

	for( int i = 0 ; i < 4; i ++ )
	{
		sprintf_s( szControlName, "ID_RBT_TAB%d", i );
		m_pTabButton[i] = GetControl<CEtUIRadioButton>(szControlName);
	}

	//ID_LISTBOXEX_LIST	

	m_pListBoxEx = GetControl<CEtUIListBoxEx>("ID_LISTBOXEX_LIST");

	CDnSetItemCompoundDlg* pSetItemDlg = new CDnSetItemCompoundDlg( UI_TYPE_CHILD, this, CDnCompoundBase::COMPOUND_SETITEM_DIALOG );
	if( pSetItemDlg ) pSetItemDlg->Initialize( false );
	AddTabDialog( m_pTabButton[ CDnCompoundBase::COMPOUND_SETITEM_DIALOG], pSetItemDlg );

	CDnSuffixCompoundDlg* pSuffixDlg = new CDnSuffixCompoundDlg( UI_TYPE_CHILD, this, CDnCompoundBase::COMPOUND_SUFFIX_DIALOG );
	if( pSuffixDlg ) pSuffixDlg->Initialize( false );
	AddTabDialog( m_pTabButton[ CDnCompoundBase::COMPOUND_SUFFIX_DIALOG], pSuffixDlg );

	CDnArtficeCompoundDlg* pArtficeDlg = new CDnArtficeCompoundDlg( UI_TYPE_CHILD, this, CDnCompoundBase::COMPOUND_ARTFICE_DIALOG );
	if( pArtficeDlg ) pArtficeDlg->Initialize( false );
	AddTabDialog( m_pTabButton[ CDnCompoundBase::COMPOUND_ARTFICE_DIALOG], pArtficeDlg );

	CDnChandleryCompoundDlg* pChandleryDlg = new CDnChandleryCompoundDlg( UI_TYPE_CHILD, this, CDnCompoundBase::COMPOUND_CHANDLERY_DIALOG );
	if( pChandleryDlg ) pChandleryDlg->Initialize( false ); 
	AddTabDialog( m_pTabButton[ CDnCompoundBase::COMPOUND_CHANDLERY_DIALOG], pChandleryDlg );
	
	m_pItemCompoundMsgBox = new CDnItemCompoundMessageBox( UI_TYPE_MODAL, NULL, COMPOUND_CONFIRM_DIALOG, this );
	if( m_pItemCompoundMsgBox ) m_pItemCompoundMsgBox->Initialize( false );

	SetCheckedTab( m_pTabButton[CDnCompoundBase::COMPOUND_SETITEM_DIALOG]->GetTabID() );
}

void CDnItemCompoundRenewalTabDlg::Show( bool bShow )
{
	if( m_bShow == bShow )
		return;
		
	if( bShow )
	{
		GetInterface().ShowNpcTalkReturnDlgButton(false);
		m_pListBoxEx->Show( true );
	}
	else
	{
		m_pListBoxEx->Show( false );
		SetCheckedTab( m_pTabButton[CDnCompoundBase::COMPOUND_SETITEM_DIALOG]->GetTabID() );
		GetInterface().CloseBlind();	
		if( m_nRemoteItemID )
		{
			m_nRemoteItemID = 0;
			GetInterface().OpenBaseDialog();
			CDnLocalPlayerActor::LockInput( false );
		}
//#ifdef PRE_ADD_NOTIFY_ITEM_COMPOUND
//		if( m_pCurrentCompoundListDlg) m_pCurrentCompoundListDlg->ResetForceOpenMode();
//#endif
		GetInterface().ForceShowCashShopMenuDlg(true);
	}

	CEtUITabDialog::Show( bShow );
		
}

void CDnItemCompoundRenewalTabDlg::UpdateGroupList( int nTabID )
{
	if( nTabID < 0 || nTabID >= CDnCompoundBase::MAX_COMPOUND_DIALOG_COUNT ) return;

	Clear();

	CEtUIDialog* pDialog = m_groupTabDialog.GetDialog( m_pTabButton[ nTabID ]->GetTabID() );
	if( ! pDialog ) return;
	
	CDnCompoundBase* pCompoundBase = static_cast< CDnCompoundBase* >( pDialog );
	if( ! pCompoundBase ) return;

	pCompoundBase->UpdateGroupList( m_pVecResultGroupList );
	RefreshGroupList();
}

void CDnItemCompoundRenewalTabDlg::Reset()
{
	m_pListBoxEx->RemoveAllItems();

	CEtUIDialog* pDialog  = NULL;
	CDnCompoundBase* pCompoundBase = NULL;
	for( int i = 0; i < m_groupTabDialog.GetDialogCount(); ++i )
	{
		pDialog = m_groupTabDialog.GetDialog( m_pTabButton[ i ]->GetTabID() );
		if( ! pDialog ) continue;
		pCompoundBase = static_cast< CDnCompoundBase* >( pDialog );
		if( ! pCompoundBase ) continue;	
		pCompoundBase->Reset();
	}
	
}

void CDnItemCompoundRenewalTabDlg::OnRecvItemCompoundOpen()
{
	CDnItemCompounder::S_COMPOUND_INFO_EXTERN CompoundInfo;

	if( ! m_pCurrentCompoundListDlg )
		return;

	GetItemTask().GetCompoundInfo( m_pCurrentCompoundListDlg->GetCompoundItemPackage().nCompoundID, &CompoundInfo );
	GetInterface().OpenMovieProcessDlg(
		"ItemCombine.avi", CompoundInfo.fTimeRequired / 1000.0f, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 8019 ), COMPOUND_MOVIE_DIALOG, this );
}

void CDnItemCompoundRenewalTabDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_CHECKBOX_CHANGED )
	{
		//if( strstr( pControl->GetControlName(), "")		
#ifdef PRE_ADD_NOTIFY_ITEM_COMPOUND
		if( IsCmdControl( "ID_CHECKBOX_INFO" ) ) 
		{
			int nIndex = m_pListBoxEx->GetSelectedIndex();
			if( nIndex < 0 ) return;

			CDnItemCompoundListDlg* pCompoundListDlg = NULL;
			for( int i = 0; i < m_pListBoxEx->GetSize(); ++i )
			{
				pCompoundListDlg = m_pListBoxEx->GetItem<CDnItemCompoundListDlg>( i );
				if( pCompoundListDlg->IsNotifyCheck() && i != nIndex )
				{
					pCompoundListDlg->EnableNotify( false );
					pCompoundListDlg->SetNotifyCheckBox( false );					
				}
			}
		}
#endif 
	}
	else if( nCommand == EVENT_COMBOBOX_SELECTION_CHANGED )
	{
		m_pListBoxEx->RemoveAllItems();
	} 
	else if( nCommand == EVENT_BUTTON_CLICKED ) 
	{
		if( IsCmdControl( "ID_CLOSE_DIALOG" ) || IsCmdControl( "ID_BT_CLOSE" ) )
		{
			Show(false);
			return;
		}
		else if( IsCmdControl("ID_BT_ALL") )
		{
			int nIndex = m_pListBoxEx->GetSelectedIndex();
				
			if( nIndex < 0 )
				return;
			
			m_pCurrentCompoundListDlg = m_pListBoxEx->GetItem<CDnItemCompoundListDlg>( nIndex );
			if( m_pCurrentCompoundListDlg && m_bForceOpenMode == false )
			{
				m_pItemCompoundMsgBox->SetItemSlot( m_pCurrentCompoundListDlg->GetItem() );
				m_pItemCompoundMsgBox->Show(true);
			}	
		}
#ifdef PRE_ADD_SHORTCUT_HELP_DIALOG
		else if( IsCmdControl("ID_BT_SMALLHELP") )
		{
			CDnMainMenuDlg* pMainMenuDlg = GetInterface().GetMainMenuDialog();
			CDnSystemDlg* pSystemDlg = pMainMenuDlg->GetSystemDlg();
			if(pSystemDlg)
				pSystemDlg->ShowChoiceHelpDlg(HELP_SHORTCUT_ITEM_CREATE);
		}
#endif
	}
	else if( nCommand == EVENT_RADIOBUTTON_CHANGED )
	{
		if( strstr( pControl->GetControlName(), "ID_RBT_TAB" ) ) 
		{
			sscanf_s( pControl->GetControlName(), "ID_RBT_TAB%d", &m_nCompoundMainCategory, sizeof(int) );
			UpdateGroupList( m_nCompoundMainCategory );
			//if( m_pItemCompoundMsgBox->IsShow() )	m_pItemCompoundMsgBox->Show( false );
		}
	}


	CEtUITabDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

bool CDnItemCompoundRenewalTabDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsShow() )
		return false;
	
	switch( uMsg )
	{
	case WM_LBUTTONDOWN:
		{
			POINT MousePoint;
			float fMouseX, fMouseY;

			MousePoint.x = short( LOWORD( lParam ) );
			MousePoint.y = short( HIWORD( lParam ) );
			PointToFloat( MousePoint, fMouseX, fMouseY );

			int nSelected = -1;
			int nSize = m_pListBoxEx->GetSize();

			//// 선택한 아이템 리스트 인덱스 찾기 
			//for( int i = 0 ; i < nSize; i++ )
			//{	
			//	pListItem = m_pListBoxEx->GetItem<CDnItemCompoundListDlg>( i );
			//	if( pListItem && pListItem->IsInsideItem( (float)MousePoint.x, (float)MousePoint.y ) )
			//	{
			//		//CEtUIButton* pButton = pListItem->GetControl<CEtUIButton>("ID_BT_ALL");
			//		//if( pButton && pButton->IsInside(fMouseX, fMouseY) )
			//		//{
			//		//	m_pItemCompoundMsgBox->SetItemSlot( pListItem->GetItem() );
			//		//	m_pItemCompoundMsgBox->Show(true);
			//		//}
			//		break;
			//	}
			//}
			
			//UpdateGroupList( m_nCompoundMainCategory );
			//RefreshGroupList();

		}break;
	}

	return CEtUIDialog::MsgProc( hWnd, uMsg, wParam, lParam );
}

void CDnItemCompoundRenewalTabDlg::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		switch( nID ) 
		{
		case COMPOUND_CONFIRM_DIALOG:
			{
				if( IsCmdControl("ID_BT_OK") ) 
				{
					if( !GetInterface().IsEmptySlot( CDnMainMenuDlg::INVENTORY_DIALOG ) )
					{
						GetInterface().MessageBox( 7012, MB_OK );
						return;
					}
#ifdef PRE_ADD_NOTIFY_ITEM_COMPOUND
					if( m_bForceOpenMode )
						return;
#endif
					if( m_pCurrentCompoundListDlg && m_pCurrentCompoundListDlg->GetCompoundItemPackage().nCompoundID )
					{
						CDnItemCompounder::S_COMPOUND_INFO_EXTERN CompoundInfo;
						GetItemTask().GetCompoundInfo( m_pCurrentCompoundListDlg->GetCompoundItemPackage().nCompoundID, &CompoundInfo );

						CDnItem* pNotSuffixItem;
						INT64 nSerialID = 0;
						if( CompoundInfo.bResultItemIsNeedItem == true )
						{
							pNotSuffixItem = GetItemTask().GetCharInventory().FindItem( CompoundInfo.aiItemID[0], CompoundInfo.iCompoundPreliminaryID );	
							if( ! pNotSuffixItem )	return;
							nSerialID = pNotSuffixItem->GetSerialID();
						}
		
						GetItemTask().RequestItemCompoundOpen( m_pCurrentCompoundListDlg->GetCompoundItemPackage().nCompoundID, 
							m_pCurrentCompoundListDlg->GetCompoundItemPackage().vNeedItemInfo, 
							nSerialID, 
							m_pCurrentCompoundListDlg->GetCompoundItemPackage().nCost  );

					}
				}
			}
			break;
		case COMPOUND_MOVIE_DIALOG:
			if( nCommand == EVENT_BUTTON_CLICKED )
			{
				if( IsCmdControl("ID_PROCESS_COMPLETE") ) 
				{
					if( m_pCurrentCompoundListDlg && m_pCurrentCompoundListDlg->GetCompoundItemPackage().nCompoundID ) 
					{

						CDnItemCompounder::S_COMPOUND_INFO_EXTERN CompoundInfo;
						GetItemTask().GetCompoundInfo( m_pCurrentCompoundListDlg->GetCompoundItemPackage().nCompoundID, &CompoundInfo );

						CDnItem* pNotSuffixItem;
						INT64 nSerialID = 0;
						if( CompoundInfo.bResultItemIsNeedItem == true )
						{
							pNotSuffixItem = GetItemTask().GetCharInventory().FindItem( CompoundInfo.aiItemID[0], CompoundInfo.iCompoundPreliminaryID );	
							if( ! pNotSuffixItem )	return;
							nSerialID = pNotSuffixItem->GetSerialID();
						}

						GetItemTask().RequestItemCompound( m_pCurrentCompoundListDlg->GetCompoundItemPackage().nCompoundID,
							m_pCurrentCompoundListDlg->GetCompoundItemPackage().vNeedItemInfo, 
							nSerialID, 
							m_pCurrentCompoundListDlg->GetCompoundItemPackage().nCost );
					}

				}
				else if( IsCmdControl("ID_BUTTON_CANCEL") ) {
					GetItemTask().RequestItemCompoundCancel();
				}
			}
			break;
		}
	}

}

void CDnItemCompoundRenewalTabDlg::SortLevel()
{
	int nLevels[11] = { 24, 32, 40, 45, 50, 55, 60, 70, 80, 90, 93 };
	int nLevelIndex = 0;

	std::vector< CDnCompoundBase::ItemGroupStruct * >::iterator Itor = m_pVecResultGroupList.begin();
	std::vector< CDnCompoundBase::ItemGroupStruct * > VecCopyList;

	while( ! m_pVecResultGroupList.empty() )
	{
		if( nLevels[nLevelIndex] == (*Itor)->nLevel )
		{
			VecCopyList.push_back( (*Itor) ); 
			Itor = m_pVecResultGroupList.erase( Itor );
		}
		else
			++Itor;

		if( Itor == m_pVecResultGroupList.end() )
		{
			nLevelIndex++;
			Itor = m_pVecResultGroupList.begin();
		}
	}

	m_pVecResultGroupList = VecCopyList;	
}

void CDnItemCompoundRenewalTabDlg::SortRank()
{
	int nRanks[3] = { CDnCompoundBase::eRARE, CDnCompoundBase::eEPIC, CDnCompoundBase::eUNIQUE };
	int nRankIndex = 0;

	std::vector< CDnCompoundBase::ItemGroupStruct * >::iterator Itor = m_pVecResultGroupList.begin();
	std::vector< CDnCompoundBase::ItemGroupStruct * > VecCopyList;

	while( ! m_pVecResultGroupList.empty() )
	{
		if( nRanks[nRankIndex] == (*Itor)->nRank )
		{
			VecCopyList.push_back( (*Itor) ); 
			Itor = m_pVecResultGroupList.erase( Itor );
		}
		else
			++Itor;

		if( Itor == m_pVecResultGroupList.end() )
		{
			nRankIndex++;
			Itor = m_pVecResultGroupList.begin();
		}
	}

	m_pVecResultGroupList = VecCopyList;
}

void CDnItemCompoundRenewalTabDlg::SetRemoteCompoundDlg( CDnCompoundBase::ItemGroupStruct& CompoundGroupData )
{
	int nTabID = CompoundGroupData.nCompoundMainCategory;

	if( nTabID < 0 || nTabID >= CDnCompoundBase::MAX_COMPOUND_DIALOG_COUNT ) return;
	
	// 컨트롤들의 옵션을 지정하는 것을 만들자. 

	CEtUIDialog* pDialog = m_groupTabDialog.GetDialog( m_pTabButton[ nTabID ]->GetTabID() );
	if( ! pDialog ) return;

	CDnCompoundBase* pCompoundBase = static_cast< CDnCompoundBase* >( pDialog );
	if( ! pCompoundBase )	return;

	SetCheckedTab( nTabID + 1 );
	pCompoundBase->SetRemoteCompoundDlg( CompoundGroupData );

}

void CDnItemCompoundRenewalTabDlg::Clear()
{
	int nSize = static_cast<int>( m_pVecResultGroupList.size() );
	for( int i = 0; i < nSize; ++i )
	{
		m_pVecResultGroupList.erase( m_pVecResultGroupList.begin() + ( nSize - 1 - i ) );
	}
}


void CDnItemCompoundRenewalTabDlg::SetRemoteItemID( int nRemoteItemID )
{
	m_nRemoteItemID = nRemoteItemID;
	Reset();
	UpdateGroupList( 0 );

	if( nRemoteItemID ) {
		GetInterface().GetMainMenuDialog()->CloseInvenDialog();
		GetInterface().GetMainMenuDialog()->SetIgnoreShowFunc( true );
		GetInterface().OpenBlind();
		CDnLocalPlayerActor::StopAllPartyPlayer();
		CDnLocalPlayerActor::LockInput( true );
	}
}


bool CDnItemCompoundRenewalTabDlg::SortByEnableList( int nLsh, int nRsh )
{
	CDnItemCompounder::S_COMPOUND_INFO_EXTERN LCompoundInfo;	
	GetItemTask().GetCompoundInfo( nLsh, &LCompoundInfo );

	CDnItemCompounder::S_COMPOUND_INFO_EXTERN RCompoundInfo;	
	GetItemTask().GetCompoundInfo( nRsh, &RCompoundInfo );

	return CDnItemCompoundListDlg::IsNeedItemList( LCompoundInfo.iCompoundTableID )  > CDnItemCompoundListDlg::IsNeedItemList( RCompoundInfo.iCompoundTableID )  ;
}

void CDnItemCompoundRenewalTabDlg::RefreshItemList()
{
	int nSize = m_pListBoxEx->GetSize();
	CDnItemCompoundListDlg*	pListItem = NULL;

	for( int i = 0 ; i < nSize; i++ )
	{	
		pListItem = m_pListBoxEx->GetItem<CDnItemCompoundListDlg>( i );
		if( pListItem ) pListItem->UpdateSelectOption();
	}
}

void CDnItemCompoundRenewalTabDlg::RefreshGroupList()
{
	CDnCompoundBase::ItemGroupStruct* pItem = NULL;
	CDnItemCompoundListDlg*	pListItem = NULL;

	m_pListBoxEx->RemoveAllItems();

	if( m_pVecResultGroupList.empty() )
		return;

	std::vector<int> VecSort;

	SortLevel();
	SortRank();

	// 재료를 기준으로 장비가 필터링 되어야함. 
	int nSize = (int)m_pVecResultGroupList.size();
	for( int i = 0; i < nSize; ++i )
	{
		pItem = m_pVecResultGroupList[i];		 
		if( ! pItem ||  pItem->nVecResultCompoundList.empty() )
			continue;

		for( DWORD j = 0; j < pItem->nVecResultCompoundList.size(); ++j )
		{
			if( pItem->nVecResultCompoundList[j] == 0 )
				continue;
			
			std::vector<int>::iterator nExist = std::find( VecSort.begin(), VecSort.end(), pItem->nVecResultCompoundList[j] );
			if( nExist == VecSort.end() )
				VecSort.push_back( pItem->nVecResultCompoundList[j] );
		}
	}

	// 재료 기준으로 정렬 
	std::sort( VecSort.begin(), VecSort.end(),  SortByEnableList );

	nSize = (int)VecSort.size();
	for( int i = 0; i < nSize; ++i )
	{
		pListItem = m_pListBoxEx->AddItem<CDnItemCompoundListDlg>( i );
		if( ! pListItem )
			continue;

		pListItem->Clear();

		CDnItemCompounder::S_COMPOUND_INFO_EXTERN CompoundInfo;	
		GetItemTask().GetCompoundInfo( VecSort[i], &CompoundInfo );

		if( CompoundInfo.aiSuccessItemID[0] > 0 ) 
		{
			pListItem->SetInfo( CompoundInfo.iCompoundTableID, CompoundInfo.aiSuccessItemID[0], this );
			pListItem->Enable( true );
			pListItem->ForceBlend();
			pListItem->Show( true );
		}

		if( m_bForceOpenMode )	pListItem->SetForceOpenMode( true );
	}	
}

#endif 