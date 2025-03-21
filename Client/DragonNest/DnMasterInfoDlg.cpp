#include "Stdafx.h"
#include "DnMasterInfoDlg.h"
#include "DnCommunityDlg.h"
#include "DnMasterInformationDlg.h"
#include "DnPupilInformationDlg.h"
#include "DnInterface.h"
#include "DnPartyTask.h"
#include "DnMasterTask.h"
#include "SyncTimer.h"
#include "DnGameTask.h"
#include "TaskManager.h"

CDnMasterInfoDlg::CDnMasterInfoDlg(UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback)
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
, m_pMasterInformationDlg( NULL )
, m_pPupilInformationDlg( NULL )
, m_pMasterTree( NULL )
, m_pPupilTree( NULL )
, m_bHide( false )
{
}

CDnMasterInfoDlg::~CDnMasterInfoDlg()
{
	SAFE_DELETE( m_pMasterInformationDlg );
	SAFE_DELETE( m_pPupilInformationDlg );
}

void CDnMasterInfoDlg::Initialize(bool bShow)
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "MasterInforDlg.ui" ).c_str(), bShow );
}

void CDnMasterInfoDlg::InitialUpdate()
{
	CONTROL( Button, ID_BT_CALL )->Enable( false );
	CONTROL( Button, ID_BT_JOINPARTY )->Enable( false );
	CONTROL( Button, ID_BT_INFO )->Enable( false );
	CONTROL( Button, ID_BT_CHAT )->Enable( false );
	CONTROL( Button, ID_BT_DEL )->Enable( false );

	m_pMasterInformationDlg = new CDnMasterInformationDlg(UI_TYPE_CHILD_MODAL, this);
	m_pMasterInformationDlg->Initialize( false );

	m_pPupilInformationDlg = new CDnPupilInformationDlg(UI_TYPE_CHILD_MODAL, this);
	m_pPupilInformationDlg->Initialize( false );
}

void CDnMasterInfoDlg::Show(bool bShow)
{
	if( m_bShow == bShow )
		return;

	if( bShow )
	{
		GetMasterTask().RequestMasterAndClassmate();

		CONTROL( Button, ID_BT_CALL )->Enable( false );
		CONTROL( Button, ID_BT_JOINPARTY )->Enable( false );
		CONTROL( Button, ID_BT_INFO )->Enable( false );
		CONTROL( Button, ID_BT_CHAT )->Enable( false );
		CONTROL( Button, ID_BT_DEL )->Enable( false );

		ShowChildDialog( m_pMasterInformationDlg, false );
		ShowChildDialog( m_pPupilInformationDlg, false );
	}

	CEtUIDialog::Show( bShow );
}

void CDnMasterInfoDlg::Process(float fElapsedTime)
{
	CEtUIDialog::Process( fElapsedTime );
}

void CDnMasterInfoDlg::ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg)
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		CTreeItem * pItem = CONTROL( TreeCtl, ID_TREE_FRIEND )->GetSelectedItem();

		if( IsCmdControl("ID_BT_LIST") )
		{
			Show( false );
			((CDnCommunityDlg *)m_pParentDialog)->ChangeMasterDialog( CDnCommunityDlg::typeMasterList );
		}
		else if( IsCmdControl("ID_BT_CALL") )
		{
			CTreeItem * pTreeItem = CONTROL( TreeCtl, ID_TREE_FRIEND )->GetSelectedItem();
			
			if( pTreeItem && pTreeItem->GetItemData() )
			{
				TMasterAndClassmateInfoWithLocation * pItem = (TMasterAndClassmateInfoWithLocation *)pTreeItem->GetItemData();

				WCHAR wszString[256];

				swprintf_s( wszString, _countof(wszString), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7234 ),  pItem->wszCharName );
				GetInterface().MessageBox( wszString, MB_YESNO, MASTER_RECALL_DIALOG, this );
			}
		}
		else if( IsCmdControl("ID_BT_JOINPARTY") )
		{
			if( pItem )
				GetPartyTask().ReqInviteParty( pItem->GetText() );
		}
		else if( IsCmdControl("ID_BT_INFO") )
		{
			CTreeItem * pTreeItem = CONTROL( TreeCtl, ID_TREE_FRIEND )->GetSelectedItem();

			if( pTreeItem && pTreeItem->GetItemData() )
			{
				TMasterAndClassmateInfoWithLocation * pItem = (TMasterAndClassmateInfoWithLocation *)pTreeItem->GetItemData();

				if( MasterClassmateKind::eCode::Master == pItem->Code )
				{
					GetMasterTask().RequestMyMasterInfo( pItem->biCharacterDBID );
					ShowChildDialog( m_pMasterInformationDlg, true );
				}
				else if( MasterClassmateKind::eCode::Classmate == pItem->Code )
				{
					GetMasterTask().RequestClassmateInfo( pItem->biCharacterDBID );
					ShowChildDialog( m_pPupilInformationDlg, true );
				}
			}
		}
		else if( IsCmdControl("ID_BT_CHAT") )
		{
			if( pItem )
				GetInterface().OpenPrivateChatDialog( pItem->GetText() );
		}
		else if( IsCmdControl("ID_BT_DEL") )
		{
			GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7233 ), MB_YESNO, MASTER_DELETE_DIALOG, this );
		}
	}
	else if( nCommand == EVENT_CHECKBOX_CHANGED )
	{
		if( IsCmdControl("ID_CHECKBOX_HIDE") )
		{
			if( CONTROL( CheckBox, ID_CHECKBOX_HIDE )->IsChecked() )
			{
				m_bHide = true;
				RefreshTree();
			}
			else
			{
				m_bHide = false;
				RefreshTree();
			}
		}
	}
	else if( nCommand == EVENT_TREECONTROL_SELECTION )
	{
		if( IsCmdControl("ID_TREE_FRIEND") )
		{
			CTreeItem * pTreeItem = CONTROL( TreeCtl, ID_TREE_FRIEND )->GetSelectedItem();

			if( pTreeItem && pTreeItem->GetItemData() )
			{
				CDnGameTask* pGameTask = (CDnGameTask*)(CTaskManager::GetInstance().GetTask("GameTask"));

#ifdef PRE_FIX_62281
				TMasterAndClassmateInfoWithLocation * pItem = (TMasterAndClassmateInfoWithLocation *)pTreeItem->GetItemData();
				if (pItem == NULL)
					return;
#endif
				if( pGameTask )
				{
#ifdef PRE_FIX_62281
					CONTROL( Button, ID_BT_JOINPARTY )->Enable(pItem->Location.cServerLocation != 0);
#else
					CONTROL( Button, ID_BT_JOINPARTY )->Enable( false );
#endif
					CONTROL( Button, ID_BT_CALL )->Enable( false );
					CONTROL( Button, ID_BT_INFO )->Enable( false );
					CONTROL( Button, ID_BT_DEL )->Enable( false );
					
					CONTROL( Button, ID_BT_CHAT )->Enable( true );
					return;
				}

#ifdef PRE_FIX_62281
#else
				TMasterAndClassmateInfoWithLocation * pItem = (TMasterAndClassmateInfoWithLocation *)pTreeItem->GetItemData();
#endif

				if( MasterClassmateKind::eCode::Master == pItem->Code )
				{
					if( pItem->Location.cServerLocation )
						CONTROL( Button, ID_BT_CALL )->Enable( true );
					else
						CONTROL( Button, ID_BT_CALL )->Enable( false );
					CONTROL( Button, ID_BT_DEL )->Enable( true );
				}
				else
				{
					CONTROL( Button, ID_BT_CALL )->Enable( false );
					CONTROL( Button, ID_BT_DEL )->Enable( false );
				}

				if( pItem->Location.cServerLocation )
				{
					CONTROL( Button, ID_BT_CHAT )->Enable( true );
					CONTROL( Button, ID_BT_JOINPARTY )->Enable( true );
				}
				else
				{
					CONTROL( Button, ID_BT_CHAT )->Enable( false );
					CONTROL( Button, ID_BT_JOINPARTY )->Enable( false );
				}

				CONTROL( Button, ID_BT_INFO )->Enable( true );
			}
			else
			{
				CONTROL( Button, ID_BT_CALL )->Enable( false );
				CONTROL( Button, ID_BT_JOINPARTY )->Enable( false );
				CONTROL( Button, ID_BT_INFO )->Enable( false );
				CONTROL( Button, ID_BT_CHAT )->Enable( false );
				CONTROL( Button, ID_BT_DEL )->Enable( false );
			}
		}
		else
		{
			CONTROL( Button, ID_BT_CALL )->Enable( false );
			CONTROL( Button, ID_BT_JOINPARTY )->Enable( false );
			CONTROL( Button, ID_BT_INFO )->Enable( false );
			CONTROL( Button, ID_BT_CHAT )->Enable( false );
			CONTROL( Button, ID_BT_DEL )->Enable( false );
		}
	}
	else if( nCommand == EVENT_TREECONTROL_SELECTION_END )
	{
		CTreeItem * pTreeItem = CONTROL( TreeCtl, ID_TREE_FRIEND )->GetSelectedItem();

		if( !pTreeItem )
		{
			CONTROL( Button, ID_BT_CALL )->Enable( false );
			CONTROL( Button, ID_BT_JOINPARTY )->Enable( false );
			CONTROL( Button, ID_BT_INFO )->Enable( false );
			CONTROL( Button, ID_BT_CHAT )->Enable( false );
			CONTROL( Button, ID_BT_DEL )->Enable( false );
		}
	}
	

	CEtUIDialog::ProcessCommand(nCommand, bTriggeredByUser, pControl, uMsg);
}

void CDnMasterInfoDlg::OnUICallbackProc(int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg)
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl( "ID_YES" ) )
		{
			CTreeItem * pTreeItem = CONTROL( TreeCtl, ID_TREE_FRIEND )->GetSelectedItem();

			if( !pTreeItem || !pTreeItem->GetItemData() )
				return;

			TMasterAndClassmateInfoWithLocation * pItem = (TMasterAndClassmateInfoWithLocation *)pTreeItem->GetItemData();

			if( MASTER_DELETE_DIALOG == nID )
				GetMasterTask().RequestLeave( pItem->biCharacterDBID, false );
			else if( MASTER_RECALL_DIALOG == nID )
				GetMasterTask().RequestRecallMaster( pItem->wszCharName );
		}
	}
}

void CDnMasterInfoDlg::RefreshTree()
{
	const SCMasterClassmate * pMasterClassmate = GetMasterTask().GetMasterClassmate();

	if( !pMasterClassmate )
		return;

	CONTROL( TreeCtl, ID_TREE_FRIEND )->DeleteAllItems();

	WCHAR szString[256];
	int nMasterActive = 0, nClassmateActive = 0, nMasterMax = 0, nClassmateMax = 0;

	m_pMasterTree = CONTROL( TreeCtl, ID_TREE_FRIEND )->AddItem( CTreeItem::typeOpen, L"", textcolor::FONT_GREEN );
	m_pPupilTree = CONTROL( TreeCtl, ID_TREE_FRIEND )->AddItem( CTreeItem::typeOpen, L"", textcolor::FONT_GREEN );

	CTreeItem * pTreeItem;
	for( int itr = 0; itr < pMasterClassmate->cCount; ++itr )
	{
		if( m_bHide && 0 == pMasterClassmate->DataList[itr].Location.cServerLocation )
			continue;

		if( MasterClassmateKind::eCode::Master ==  pMasterClassmate->DataList[itr].Code )
		{
			pTreeItem = CONTROL( TreeCtl, ID_TREE_FRIEND )->AddChildItem( m_pMasterTree, CTreeItem::typeOpen, pMasterClassmate->DataList[itr].wszCharName, textcolor::DARKGRAY );
			pTreeItem->SetItemData( const_cast<TMasterAndClassmateInfoWithLocation *>(&pMasterClassmate->DataList[itr]) );
			++nMasterMax;

			if( pMasterClassmate->DataList[itr].Location.cServerLocation )
			{
				pTreeItem->SetClassIconType( CTreeItem::eGREEN );
				++nMasterActive;
			}
			else
				pTreeItem->SetClassIconType( CTreeItem::eYELLOW );
		}
		else if( MasterClassmateKind::eCode::Classmate == pMasterClassmate->DataList[itr].Code )
		{
			pTreeItem = CONTROL( TreeCtl, ID_TREE_FRIEND )->AddChildItem( m_pPupilTree, CTreeItem::typeOpen, pMasterClassmate->DataList[itr].wszCharName, textcolor::DARKGRAY );
			pTreeItem->SetItemData( const_cast<TMasterAndClassmateInfoWithLocation *>(&pMasterClassmate->DataList[itr]) );
			++nClassmateMax;

			if( pMasterClassmate->DataList[itr].Location.cServerLocation )
			{
				pTreeItem->SetClassIconType( CTreeItem::eGREEN );
				++nClassmateActive;
			}
			else
				pTreeItem->SetClassIconType( CTreeItem::eYELLOW );
		}
	}

	swprintf_s( szString, _countof(szString), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7212 ), nMasterActive, nMasterMax );
	m_pMasterTree->SetText( szString, textcolor::FONT_GREEN );
	swprintf_s( szString, _countof(szString), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7213 ), nClassmateActive, nClassmateMax );
	m_pPupilTree->SetText( szString, textcolor::FONT_GREEN );

	CONTROL( TreeCtl, ID_TREE_FRIEND )->ExpandAll();

	swprintf_s( szString, _countof(szString), L"%d/%d", nClassmateActive + nMasterActive, pMasterClassmate->cCount );
	CONTROL( Static, ID_TEXT_COUNT )->SetText( szString );

	if( nMasterMax == MasterSystem::Max::MasterCount )
		CONTROL( Button, ID_BT_LIST )->Enable( false );
	else
		CONTROL( Button, ID_BT_LIST )->Enable( true );
}

void CDnMasterInfoDlg::RefreshMyMasterInfo()
{
	m_pMasterInformationDlg->RefreshInformation();
}

void CDnMasterInfoDlg::RefreshClassmateInfo()
{
	m_pPupilInformationDlg->RefreshInformation();
}
