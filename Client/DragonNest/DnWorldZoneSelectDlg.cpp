#include "stdafx.h"
#include "DnWorldZoneSelectDlg.h"
#include "DnWorldZoneSelectListDlg.h"
#include "DnWorld.h"
#include "DnInterface.h"
#include "DnMainMenuDlg.h"
#include "PartySendPacket.h"

#ifdef PRE_ADD_QUESTCOUNT_IN_STAGELIST
#include "DnWorldZoneSelectListDlg.h"
#endif

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnWorldZoneSelectDlg::CDnWorldZoneSelectDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback, true )
, m_pStaticTitleName( NULL )
, m_pButtonStart( NULL )
, m_pButtonCancel( NULL )
, m_pButtonPartyDlgOpen( NULL )
, m_pListBox( NULL )
, m_pStaticDungeon(NULL)
, ID_LISTBOX_DUNGEONLIST(NULL)
{
}

CDnWorldZoneSelectDlg::~CDnWorldZoneSelectDlg(void)
{
	int c = 10;
}

void CDnWorldZoneSelectDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "WorldZoneSelectDlg.ui" ).c_str(), bShow );
}

void CDnWorldZoneSelectDlg::InitialUpdate()
{
	m_pStaticTitleName = GetControl<CEtUIStatic>( "ID_STATIC_TITLE" );
	m_pButtonStart = GetControl<CEtUIButton>( "ID_OK" );
	m_pButtonCancel = GetControl<CEtUIButton>( "ID_CANCEL" );
	m_pButtonPartyDlgOpen = GetControl<CEtUIButton>( "ID_BT_PARTY" );
	m_pListBox = GetControl<CEtUIListBoxEx>( "ID_LISTBOXEX_LIST" );


	//RLKT FIX
	m_pStaticDungeon = GetControl<CEtUIStatic>( "ID_STATIC1" );
	m_pStaticDungeon->Show(false);
	ID_LISTBOX_DUNGEONLIST = GetControl<CEtUIListBoxEx>( "ID_LISTBOX_DUNGEONLIST" );
	ID_LISTBOX_DUNGEONLIST->Show(false);

}

void CDnWorldZoneSelectDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_LISTBOX_SELECTION )
	{
		if( IsCmdControl("ID_LISTBOXEX_LIST") )
		{
			if( GetPartyTask().GetPartyRole() != CDnPartyTask::MEMBER )
			{
				SendSelectStage( m_pListBox->GetSelectedIndex() );

				int select = m_pListBox->GetSelectedIndex();

				if( -1 != select )
				{
					CDnWorldZoneSelectListDlg * pItem = m_pListBox->GetItem<CDnWorldZoneSelectListDlg>(select);

					m_pButtonStart->Enable( pItem->IsEnterStage() );
				}
			}
		}
	}
	else if( nCommand == EVENT_LISTBOX_ITEM_DBLCLK )
	{

	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnWorldZoneSelectDlg::Process( float fElapsedTime )
{
	CEtUIDialog::Process( fElapsedTime );

	if( IsShow() )
	{
		if( !GetInterface().IsEnableRender() && m_pButtonStart->IsEnable() )
		{
			CEtUIDialog::ProcessCommand( EVENT_BUTTON_CLICKED, false, m_pButtonStart, 0 );
			Show( false );
		}
	}
}

void CDnWorldZoneSelectDlg::Show( bool bShow )
{
	if( m_bShow == bShow )
		return;

	if( bShow )
	{
		CDnMainMenuDlg *pMainMenuDlg = GetInterface().GetMainMenuDialog();
		if( pMainMenuDlg ) pMainMenuDlg->CloseMenuDialog();

		GetInterface().CloseGuildInviteReqDlg( true );
		GetInterface().CloseAcceptRequestDialog(true);

		GetInterface().ShowHelpKeyboardDialog( false );
		GetInterface().ShowMapMoveCashItemDlg( false, NULL, 0 );
		GetInterface().CloseItemUnsealDialog();
		GetInterface().ShowItemSealDialog( false );
		GetInterface().ShowGuildMarkCreateDialog( false );
#ifdef PRE_ADD_EXCHANGE_POTENTIAL
		GetInterface().ShowItemPotentialTransferDialog( false );
#endif
#ifdef PRE_ADD_PVP_VILLAGE_ACCESS
		GetInterface().ShowPVPVillageAccessDlg( false );
#endif

		EnableButtons();
	}

	CEtUIDialog::Show( bShow );
}

void CDnWorldZoneSelectDlg::EnableButtons()
{
	bool bEnable(false);

	if( GetPartyTask().GetPartyRole() == CDnPartyTask::MEMBER )
		bEnable = false;
	else
		bEnable = true;

	m_pButtonStart->Enable(bEnable);
	m_pButtonCancel->Enable(bEnable);
	m_pButtonPartyDlgOpen->Enable((CDnWorld::GetInstance().GetMapType() == CDnWorld::MapTypeVillage) && (GetPartyTask().GetPartyRole() == CDnPartyTask::SINGLE));

	m_pListBox->Enable( bEnable );
}

void CDnWorldZoneSelectDlg::SetMapInfo( const BYTE cGateNo )
{
	m_pListBox->RemoveAllItems();

	CDnWorld * pWorld = &CDnWorld::GetInstance();

	if( NULL == pWorld )
		return;

	std::vector<CDnWorld::GateStruct *> vecGateList;
	pWorld->GetGateStructList( cGateNo, vecGateList );
	std::vector<CDnWorld::GateStruct *>::reverse_iterator rItor = vecGateList.rbegin();
	for( ; rItor != vecGateList.rend(); ++rItor )
	{
		CDnWorld::GateStruct * pInfo = (*rItor);

		if( NULL == pInfo )
			continue;

		CDnWorldZoneSelectListDlg * pItem = NULL;

		pItem = m_pListBox->InsertItem<CDnWorldZoneSelectListDlg>();

		pItem->SetGateInfo( pInfo );
	}

	std::wstring wszTitleName = CDnWorld::GetInstance().GetGateTitleName( cGateNo );
	if( wszTitleName.empty() && false == vecGateList.empty() )
		wszTitleName = vecGateList[0]->szMapName.c_str();

	m_pStaticTitleName->SetText( wszTitleName.c_str() );

	m_pListBox->SelectItem( 0 );
}

#ifdef PRE_ADD_QUESTCOUNT_IN_STAGELIST
int	CDnWorldZoneSelectDlg::GetStageQuestCount( int nStageNumber )
{	
	int nGateIndex = (int)GetSelectIndex();
	
	if( nGateIndex < 0 )
		return -1;

	CDnWorldZoneSelectListDlg* pWorldZoneSelectListDlg = m_pListBox->GetItem<CDnWorldZoneSelectListDlg>(nGateIndex);

	if( pWorldZoneSelectListDlg )
		return pWorldZoneSelectListDlg->GetEnableQuestCountWithMapIndex( nStageNumber );
	else
		return -1;
}
#endif 

BYTE CDnWorldZoneSelectDlg::GetSelectIndex()
{
	return m_pListBox->GetSelectedIndex();
}

void CDnWorldZoneSelectDlg::SetSelectIndex( BYTE cSelectIndex )
{
	m_pListBox->SelectItem( cSelectIndex );
}
