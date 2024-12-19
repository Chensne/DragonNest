#include "StdAfx.h"
#include "DnMissionOnOffDlg.h"
#include "DnMissionListItem.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnMissionOnOffDlg::CDnMissionOnOffDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback, true )
, m_pListBoxEx(NULL)
, m_pTreeCtrl(NULL)
, m_MainCategory(CDnMissionTask::Normal)
{	
}

CDnMissionOnOffDlg::~CDnMissionOnOffDlg(void)
{
	m_pListBoxEx->RemoveAllItems();
}

void CDnMissionOnOffDlg::InitialUpdate()
{
	m_pListBoxEx = GetControl<CEtUIListBoxEx>( "ID_LISTBOXEX0" );
	m_pTreeCtrl = GetControl<CEtUITreeCtl>("ID_TREE01");
}

void CDnMissionOnOffDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "MissionSubTabDlg.ui" ).c_str(), bShow );
}

void CDnMissionOnOffDlg::SetMainCategory( CDnMissionTask::MainCategoryEnum Category )
{
	m_MainCategory = Category;
}
void CDnMissionOnOffDlg::RefreshSubCategory()
{
#ifdef PRE_MOD_MISSION_HELPER
	m_pListBoxEx->RemoveAllItems();
	m_pTreeCtrl->DeleteAllItems();
#endif

	CDnMissionTask *pTask = &CDnMissionTask::GetInstance();
	DWORD dwCount = pTask->GetMissionCount();

	// 일단 현제 보여질 객체 골라내고.
	std::vector<CDnMissionTask::MissionInfoStruct *> pVecList;

	for( DWORD i=0; i<dwCount; i++ ) {
		CDnMissionTask::MissionInfoStruct *pInfo = pTask->GetMissionInfo(i);
		if( !pInfo ) continue;
		if( pInfo->MainCategory != m_MainCategory ) continue;
		pVecList.push_back( pInfo );
	}

	struct TreeItemStruct {
		CTreeItem *pItem;
		std::map<int, TreeItemStruct *> nMapTreeList;
	};
	std::map<int, TreeItemStruct *> nMapTreeList;
	std::vector<TreeItemStruct *> pVecTemp;

	CTreeItem *pSelectItem = NULL;
	for( DWORD i=0; i<pVecList.size(); i++ ) {
		CDnMissionTask::MissionInfoStruct *pInfo = pVecList[i];

		// 보상관련 이벤트 미션은 카테고리에 넣지 않는다.
		if( 6 == pInfo->nType )
			continue;

		std::map<int, TreeItemStruct *> *pTreeList = &nMapTreeList;
		CTreeItem *pParentItem = NULL;
		std::string szCodeStr;
		for( int j=0;; j++ ) {
			//char *szCategoryID = _GetSubStrByCount( j, (char*)pInfo->szSubCategory.c_str(), '/' );
			std::string strValue = _GetSubStrByCountSafe( j, (char*)pInfo->szSubCategory.c_str(), '/' );
			if( strValue.size() == 0 ) break;
			if( j > 0 ) szCodeStr += "/";
			int nCategoryID = atoi( strValue.c_str() );
			szCodeStr += strValue;

			std::map<int, TreeItemStruct *>::iterator it = pTreeList->find( nCategoryID );
			if( it == pTreeList->end() ) {
				TreeItemStruct *pStruct = new TreeItemStruct;
				pVecTemp.push_back( pStruct );
				if( j == 0 ) pStruct->pItem = m_pTreeCtrl->AddItem( CTreeItem::typeOpen, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nCategoryID ) );
				else pStruct->pItem = m_pTreeCtrl->AddChildItem( pParentItem, CTreeItem::typeOpen, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nCategoryID ) );

				pParentItem = pStruct->pItem;
				pStruct->pItem->SetItemValueStringA( szCodeStr );
				pTreeList->insert( make_pair( nCategoryID, pStruct ) );
				pTreeList = &pStruct->nMapTreeList;

				if( pSelectItem == NULL ) {
					if( m_szLastSelectCatogory.empty() ) {
						m_szLastSelectCatogory = szCodeStr;
						pSelectItem = pStruct->pItem;
					}
					else {
						if( strcmp( szCodeStr.c_str(), m_szLastSelectCatogory.c_str() ) == NULL ) {
							pSelectItem = pStruct->pItem;
						}
					}
				}
			}
			else {
				pParentItem = it->second->pItem;
				pTreeList = &it->second->nMapTreeList;
			}
		}

	}
#ifdef PRE_ADD_MISSION_NEST_TAB
	if( m_MainCategory == CDnMissionTask::MainCategoryEnum::Nest )
		m_pTreeCtrl->Sort( true );
#endif
	m_pTreeCtrl->ExpandAll();
	if( pSelectItem ) {
		m_pTreeCtrl->SetSelectItem( pSelectItem );
		RefreshList( m_szLastSelectCatogory );
	}
	SAFE_DELETE_PVEC( pVecTemp );
}

void CDnMissionOnOffDlg::RefreshList( std::string &szStrCode )
{
	m_pListBoxEx->RemoveAllItems();

	CDnMissionTask *pTask = &CDnMissionTask::GetInstance();
	DWORD dwCount = pTask->GetMissionCount();

	std::vector<CDnMissionTask::MissionInfoStruct *> pVecList[2];
	for( DWORD i=0; i<dwCount; i++ ) {
		CDnMissionTask::MissionInfoStruct *pInfo = pTask->GetMissionInfo(i);
		if( !pInfo ) continue;
		if( strcmp( pInfo->szSubCategory.c_str(), szStrCode.c_str() ) ) continue;

		pVecList[pInfo->bAchieve].push_back( pInfo );
	}
#ifdef PRE_MOD_MISSION_HELPER
	bool bFull = pTask->IsMissionNotifierFull();
#endif
	for( int i=0; i<2; i++ ) {
		for( DWORD j=0; j<pVecList[i].size(); j++ ) {
			CDnMissionListItem *pItem = m_pListBoxEx->AddItem<CDnMissionListItem>();
			pItem->SetInfo( pVecList[i][j] );
#ifdef PRE_MOD_MISSION_HELPER
			pItem->SetParentListBoxExControl( m_pListBoxEx );
			if( bFull && pItem->IsChecked() == false ) pItem->EnableCheckBox( false );
#endif
		}
	}
	m_szLastSelectCatogory = szStrCode;
}

#ifdef PRE_MOD_MISSION_HELPER
void CDnMissionOnOffDlg::SelectMissionItemFromNotifier( int nMissionItemID )
{
	if( nMissionItemID == -1 )
		return;

	int nSize = m_pListBoxEx->GetSize();
	for( int i = 0; i < nSize; i++)  {
		CDnMissionListItem *pListItem = m_pListBoxEx->GetItem< CDnMissionListItem > ( i );
		if( nMissionItemID == pListItem->GetItemID() )  {
			m_pListBoxEx->SelectItem( i );
		}
	}
}
#endif

void CDnMissionOnOffDlg::Show( bool bShow )
{
	if( m_bShow == bShow )
		return;

	if( bShow )
	{
		RefreshSubCategory();
	}
	else
	{
		m_pListBoxEx->RemoveAllItems();
		m_pTreeCtrl->DeleteAllItems();
	}

	CEtUIDialog::Show( bShow );
}

void CDnMissionOnOffDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_TREECONTROL_SELECTION ) {
		if( IsCmdControl("ID_TREE01" ) ) {
			CTreeItem *pItem = m_pTreeCtrl->GetSelectedItem();
			if( pItem ) {
				RefreshList( pItem->GetItemValueStringA() );
			}
		}
	}
	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}
