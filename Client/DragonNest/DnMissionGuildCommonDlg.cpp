#include "StdAfx.h"
#include "DnMissionGuildCommonDlg.h"
#include "DnMissionListItem.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnMissionGuildCommonDlg::CDnMissionGuildCommonDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback, true )
, m_pListBoxEx(NULL)
, m_pTreeCtrl(NULL)
{
	m_LastSelectType = CDnMissionTask::GuildCommon;
}

CDnMissionGuildCommonDlg::~CDnMissionGuildCommonDlg(void)
{
	m_pListBoxEx->RemoveAllItems();
}

void CDnMissionGuildCommonDlg::InitialUpdate()
{
	m_pListBoxEx = GetControl<CEtUIListBoxEx>( "ID_LISTBOXEX0" );
	m_pTreeCtrl = GetControl<CEtUITreeCtl>("ID_TREE01");
}

void CDnMissionGuildCommonDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "MissionSubTabDlg.ui" ).c_str(), bShow );
}

void CDnMissionGuildCommonDlg::RefreshSubCategory()
{
	m_pListBoxEx->RemoveAllItems();
	m_pTreeCtrl->DeleteAllItems();

	CDnMissionTask *pTask = &CDnMissionTask::GetInstance();

	CTreeItem *pTreeItemDaily = m_pTreeCtrl->AddItem( CTreeItem::typeOpen, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1000029520 ) );
	pTreeItemDaily->SetItemValueInt( CDnMissionTask::GuildCommon );
	m_pTreeCtrl->ExpandAll();

	switch( m_LastSelectType )
	{

	case CDnMissionTask::GuildCommon: 
		m_pTreeCtrl->SetSelectItem( pTreeItemDaily ); 
		break;

	}

	RefreshList( m_LastSelectType );
}

void CDnMissionGuildCommonDlg::RefreshList( CDnMissionTask::DailyMissionTypeEnum Type )
{
	m_pListBoxEx->RemoveAllItems();

	CDnMissionTask *pTask = &CDnMissionTask::GetInstance();

	std::vector<CDnMissionTask::MissionInfoStruct *> pVecList[2];
	DWORD dwCount = pTask->GetDailyMissionCount( Type );
	for( DWORD i=0; i<dwCount; i++ ) {
		CDnMissionTask::MissionInfoStruct *pInfo = pTask->GetDailyMissionInfo( Type, i );
		if( !pInfo ) continue;

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
#else
			pItem->ShowCheckBox( false );
#endif
		}
	}

	m_LastSelectType = Type;
}

void CDnMissionGuildCommonDlg::SelectMissionItemFromNotifier( int nMissionItemID )
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

void CDnMissionGuildCommonDlg::Show( bool bShow )
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

void CDnMissionGuildCommonDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_TREECONTROL_SELECTION ) {
		if( IsCmdControl("ID_TREE01" ) ) {
			CTreeItem *pItem = m_pTreeCtrl->GetSelectedItem();
			if( pItem ) {

				RefreshList( (CDnMissionTask::DailyMissionTypeEnum)pItem->GetItemValueInt() );
			}
		}
	}
	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}
