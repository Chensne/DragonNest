#include "StdAfx.h"
#include "DnGameCommOptDlg.h"
#include "DnGameOptionTask.h"
#include "DnGameProfileOptDlg.h"

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnGameCommOptDlg::CDnGameCommOptDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
: CDnOptionDlg( dialogType, pParentDialog, nID, pCallback )
, m_pGameProfileOptDlg( NULL )
, m_pComboPartyInviteAcceptable( NULL )
, m_pComboGuildInviteAcceptable( NULL )
, m_pComboTradeRequestAcceptable( NULL )
, m_pComboDuelRequestAcceptable( NULL )
, m_pComboObserveStuffAcceptable( NULL )
, m_pComboLadderInviteAcceptable( NULL )
#ifdef PRE_ADD_CHAT_RENEWAL
, m_pComboPartyChatAcceptable( NULL )
#endif
{
}

CDnGameCommOptDlg::~CDnGameCommOptDlg(void)
{
	SAFE_DELETE( m_pGameProfileOptDlg );
}

void CDnGameCommOptDlg::Initialize( bool bShow )
{
	BaseClass::Initialize( CEtResourceMng::GetInstance().GetFullName( "GameCommOptDlg.ui" ).c_str(), bShow );
}

void CDnGameCommOptDlg::InitialUpdate()
{
#ifdef PRE_ADD_CHAT_RENEWAL
	m_pComboPartyChatAcceptable = GetControl< CEtUIComboBox >( "ID_OP_ADMIT_CHAT_BOX" );
	m_pComboPartyChatAcceptable->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3128 ), NULL, 0 );
	m_pComboPartyChatAcceptable->AddItem( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3127 ), NULL, 1 );
#endif


	m_pComboPartyInviteAcceptable = GetControl<CEtUIComboBox>( "ID_OP_ADMIT_PARTY_BOX" );
	m_pComboPartyInviteAcceptable->AddItem(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3113 ), NULL, 0  );
	m_pComboPartyInviteAcceptable->AddItem(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3114 ), NULL, 1  );
	/*m_pComboPartyInviteAcceptable->AddItem(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3115 ), NULL, 2  );*/

	m_pComboGuildInviteAcceptable = GetControl<CEtUIComboBox>( "ID_OP_ADMIT_GUILD_BOX" );
	m_pComboGuildInviteAcceptable->AddItem(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3113 ), NULL, 0  );
	m_pComboGuildInviteAcceptable->AddItem(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3114 ), NULL, 1  );

	m_pComboTradeRequestAcceptable = GetControl<CEtUIComboBox>( "ID_OP_ADMIT_TRADE_BOX" );
	m_pComboTradeRequestAcceptable->AddItem(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3113 ), NULL, 0  );
	m_pComboTradeRequestAcceptable->AddItem(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3114 ), NULL, 1  );
	/*m_pComboTradeRequestAcceptable->AddItem(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3115 ), NULL, 2  );*/

	m_pComboDuelRequestAcceptable = GetControl<CEtUIComboBox>( "ID_OP_ADMIT_DUEL_BOX" );
	m_pComboDuelRequestAcceptable->AddItem(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3113 ), NULL, 0  );
	m_pComboDuelRequestAcceptable->AddItem(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3114 ), NULL, 1  );
	/*m_pComboDuelRequestAcceptable->AddItem(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3115 ), NULL, 2  );*/

	m_pComboObserveStuffAcceptable = GetControl<CEtUIComboBox>( "ID_OP_ADMIT_INFO_BOX" );
	m_pComboObserveStuffAcceptable->AddItem(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3113 ), NULL, 0  );
	m_pComboObserveStuffAcceptable->AddItem(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3114 ), NULL, 1  );
	/*m_pComboObserveStuffAcceptable->AddItem(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3115 ), NULL, 2  );*/
	m_pComboLadderInviteAcceptable = GetControl<CEtUIComboBox>( "ID_OP_ADMIT_LADDER_BOX" );
	m_pComboLadderInviteAcceptable->AddItem(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3113 ), NULL, 0  );
	m_pComboLadderInviteAcceptable->AddItem(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 3114 ), NULL, 1  );

#ifdef PRE_MOD_ALLOW_INSPECT
	GetControl<CEtUIStatic>( "ID_OP_ADMIT_INFO" )->Show( false );
	m_pComboObserveStuffAcceptable->Show( false );
#endif	// #ifdef PRE_MOD_ALLOW_INSPECT

	m_pGameProfileOptDlg = new CDnGameProfileOptDlg( UI_TYPE_CHILD_MODAL, this );
	m_pGameProfileOptDlg->Initialize( false );
}

void CDnGameCommOptDlg::ImportSetting()
{
	m_pComboPartyInviteAcceptable->SetSelectedByIndex( CGameOption::GetInstance().cPartyInviteAcceptable );
	m_pComboGuildInviteAcceptable->SetSelectedByIndex( CGameOption::GetInstance().cGuildInviteAcceptable );
	m_pComboTradeRequestAcceptable->SetSelectedByIndex( CGameOption::GetInstance().cTradeRequestAcceptable );
	m_pComboDuelRequestAcceptable->SetSelectedByIndex( CGameOption::GetInstance().cDuelRequestAcceptable );
	m_pComboObserveStuffAcceptable->SetSelectedByIndex( CGameOption::GetInstance().cObserveStuffAcceptable );
	m_pComboLadderInviteAcceptable->SetSelectedByIndex( CGameOption::GetInstance().cLadderInviteAcceptable );

#ifdef PRE_ADD_CHAT_RENEWAL
	m_pComboPartyChatAcceptable->SetSelectedByIndex( CGameOption::GetInstance().cPartyChatAcceptable );
#endif

}

void CDnGameCommOptDlg::ExportSetting()
{
	if( IsChanged() ) 
	{
		CGameOption::GetInstance().cPartyInviteAcceptable = m_pComboPartyInviteAcceptable->GetSelectedIndex();
		CGameOption::GetInstance().cGuildInviteAcceptable = m_pComboGuildInviteAcceptable->GetSelectedIndex();
		CGameOption::GetInstance().cTradeRequestAcceptable = m_pComboTradeRequestAcceptable->GetSelectedIndex();
		CGameOption::GetInstance().cDuelRequestAcceptable = m_pComboDuelRequestAcceptable->GetSelectedIndex();
		CGameOption::GetInstance().cObserveStuffAcceptable = m_pComboObserveStuffAcceptable->GetSelectedIndex();
		CGameOption::GetInstance().cLadderInviteAcceptable = m_pComboLadderInviteAcceptable->GetSelectedIndex();

#ifdef PRE_ADD_CHAT_RENEWAL
		CGameOption::GetInstance().cPartyChatAcceptable = m_pComboPartyChatAcceptable->GetSelectedIndex();
#endif

		GetGameOptionTask().ReqSendSetGameOption( *static_cast<TGameOptions*>(CGameOption::GetInstancePtr()) );

	}	
}

bool CDnGameCommOptDlg::IsChanged()
{
	if(
		CGameOption::GetInstance().cPartyInviteAcceptable != m_pComboPartyInviteAcceptable->GetSelectedIndex() ||
		CGameOption::GetInstance().cGuildInviteAcceptable != m_pComboGuildInviteAcceptable->GetSelectedIndex() ||
		CGameOption::GetInstance().cTradeRequestAcceptable != m_pComboTradeRequestAcceptable->GetSelectedIndex() ||
		CGameOption::GetInstance().cDuelRequestAcceptable != m_pComboDuelRequestAcceptable->GetSelectedIndex() ||
		CGameOption::GetInstance().cObserveStuffAcceptable != m_pComboObserveStuffAcceptable->GetSelectedIndex() 
		|| CGameOption::GetInstance().cLadderInviteAcceptable != m_pComboLadderInviteAcceptable->GetSelectedIndex()
#ifdef PRE_ADD_CHAT_RENEWAL
		|| CGameOption::GetInstance().cPartyChatAcceptable != m_pComboPartyChatAcceptable->GetSelectedIndex()
#endif
	) 
		return true;
	return false;
} 

void CDnGameCommOptDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl( "ID_BT_PROFILE" ) )
		{
			ShowChildDialog( m_pGameProfileOptDlg, true );
		}
	}

	BaseClass::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}
