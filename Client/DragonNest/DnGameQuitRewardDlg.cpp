#include "Stdafx.h"

#ifdef PRE_ADD_GAMEQUIT_REWARD

#include "DnTableDB.h"

#include "TaskManager.h"
#include "DnPVPLobbyVillageTask.h"
#include "DnCommonTask.h"
#include "DnItemTask.h"

#include "DnInterface.h"
#include "DnMainMenuDlg.h"
#include "DnSystemDlg.h"

#include "DnGameQuitRewardDlg.h"

CDnGameQuitRewardDlg::CDnGameQuitRewardDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback, bool bAutoCursor )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback, bAutoCursor )
,m_pStatic(NULL)
, m_nRewardType(0)
,m_bRecvReward(false)
{

}

CDnGameQuitRewardDlg::~CDnGameQuitRewardDlg(){
	
	ReleaseDlg();
	m_vecItemSlot.clear();

}

void CDnGameQuitRewardDlg::ReleaseDlg()
{
	int size = (int)m_vecItemSlot.size();
	for( int i=0; i<size; ++i )
		m_vecItemSlot[i]->ResetSlot();	

	for( int i=0; i<2; ++i )
	{
		int size = (int)m_vecItem[i].size();
		for( int k=0; k<size; ++k )
			delete m_vecItem[i][k];
		m_vecItem[i].clear();
	}
	
}

void CDnGameQuitRewardDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName("NewbieGiftBoxDlg.ui").c_str(), bShow );
}

void CDnGameQuitRewardDlg::InitialUpdate()
{
	m_vecItemSlot.reserve( 7 );

	char str[64] = {0,};
	for( int i=0; i<7; ++i )
	{
		sprintf_s( str, 64, "ID_BT_ITEM%d", i );
		CDnItemSlotButton * pControl = GetControl<CDnItemSlotButton>( str );
		if( pControl )
			m_vecItemSlot.push_back( pControl );
	}

	m_pStatic = GetControl<CEtUIStatic>("ID_TEXT_ASK");

	//GetControl<CEtUITextBox>("ID_TEXTBOX_NOTICE")->AddColorText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7960 ) );
	GetControl<CEtUIStatic>("ID_TEXTBOX_NOTICE")->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7960 ) );
}

void CDnGameQuitRewardDlg::Show( bool bShow )
{
	if( m_bShow == bShow )
		return;

	//if( bShow )
	//{
	//}

	CEtUIDialog::Show( bShow );
}

void CDnGameQuitRewardDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl * pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );


	// Button.
	if( nCommand == EVENT_BUTTON_CLICKED )
	{	
		// 닫기.
		if( IsCmdControl("ID_BT_CLOSE") )
		{
			Show( false );
		}

		// 게임종료.
		if( IsCmdControl("ID_BT_CANCEL" ) )
		{
			Show( false );
			
			// 다음접속시 보상출력.
			ShowNextTimeReward();
		}

		// 선물받기.
		if( IsCmdControl("ID_BT_OK") )
		{
			GiftReceiving();
			Show( false );
		}
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}


void CDnGameQuitRewardDlg::SetRewardType( int type )
{
	if( m_pStatic == NULL )
		return;
	m_pStatic->ClearText();

	if( type < GameQuitReward::RewardType::eType::None || type > GameQuitReward::RewardType::eType::ComeBackReward )
		return;

	m_nRewardType = type;
	
	// 신규유저.
	int strIndex = 7954; // 새로 시작하는 용자님들을 위해 마련한 아이템!! 지급된 선물은 소지품..

	// 귀환유저.
	if( type == GameQuitReward::RewardType::eType::ComeBackReward )
		strIndex = 7959; // 다시 시작하는 용자님들을 위해 마련한 아이템!! 지급된 선물은 소지품..

	m_pStatic->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, strIndex ) );
	
	// 보상아이템 설정.
	SetRewardItem();
}


// 보상대상에 따른 보상아이템 설정.
void CDnGameQuitRewardDlg::SetRewardItem()
{		
	if( m_nRewardType < GameQuitReward::RewardType::eType::NewbieReward || m_nRewardType > GameQuitReward::RewardType::eType::ComeBackReward )
		return;

	int nLevel = 1;
	int nClass = 1;

	// LocalPlayerActor 가 있는 경우.
	if( CDnActor::s_hLocalActor )
	{
		CDnActor * pActor = static_cast< CDnActor * >(CDnActor::s_hLocalActor);
		nLevel = pActor->GetLevel();
		nClass = pActor->GetClassID();
	}

	// LocalPlayerActor 가 없는 경우.
	else
	{
		bool bPVP = false;
		CTask * pTask = CTaskManager::GetInstance().GetTask( "PVPLobbyTask" );
		if( pTask )
		{
#ifdef PRE_ADD_PVPRANK_INFORM
			CDnPVPLobbyVillageTask* pPVPLobbyVillageTask = static_cast<CDnPVPLobbyVillageTask*>(pTask);
			if (pPVPLobbyVillageTask)
			{
				const SCEnter& userInfo = pPVPLobbyVillageTask->GetUserInfo();
				nLevel = userInfo.cLevel;
				nClass = userInfo.cClassID;
			}
#else
			SCEnter & userInfo = ((CDnPVPLobbyVillageTask *)pTask)->GetUserInfo();
			nLevel = userInfo.cLevel;
			nClass = userInfo.cClassID;			
#endif
		}
	}

	// 아이템을 수령할 수 없는 대상.
	if( m_nRewardType == GameQuitReward::RewardType::eType::None )
	{
		ReleaseDlg();
		return;
	}

	int idx = m_nRewardType - 1;

	// Item참조를 한번만 하도록한다.
	if( !m_vecItem[ idx ].empty() )
		return;

	int nEventType = 6; // 신규유저는 6.
	if( m_nRewardType == GameQuitReward::RewardType::eType::ComeBackReward )
		nEventType = 8; // 귀환유저는 8.
	
	// Table로부터 Item 찾기.	
	CDnItemTask::GetInstance().FindEventItems( nEventType, nLevel, nClass, m_vecItem[ idx ] );

	// ItemSlot설정.
	CDnItem * pItem = NULL;
	int size = (int)m_vecItem[ idx ].size();
	int slotSize = (int)m_vecItemSlot.size();
	if( slotSize >= size )
	{
		for( int i=0; i<size; ++i )
		{
			pItem = m_vecItem[ idx ][i];
			if( pItem )
				m_vecItemSlot[ i ]->SetItem( pItem, pItem->GetOverlapCount() );
		}
	}

}


// 선물받기요청.
void CDnGameQuitRewardDlg::GiftReceiving()
{
	if( m_nRewardType == GameQuitReward::RewardType::eType::None )
		return;

	CTask * _pTask = CTaskManager::GetInstance().GetTask( "CommonTask" );
	if( _pTask )
		static_cast<CDnCommonTask *>(_pTask)->SendGiftReceiving( m_nRewardType );
}


void CDnGameQuitRewardDlg::RecvReward()
{
	if( m_nRewardType == GameQuitReward::RewardType::eType::NewbieReward )
		m_bRecvReward = true;
}

bool CDnGameQuitRewardDlg::ShowNewbieNextTimeReward()
{
	if( m_bRecvReward )
	{
		m_bRecvReward = false;
		GetInterface().ShowGameQuitNextTimeRewardDlg( true, GameQuitReward::RewardType::eType::NewbieReward );
		return true;
	}

	return false;
}


// 다음접속시 보상출력.
void CDnGameQuitRewardDlg::ShowNextTimeReward()
{
	if( m_nRewardType == GameQuitReward::RewardType::eType::None || m_nRewardType == GameQuitReward::RewardType::eType::ComeBackReward )
	{
		CEtUIDialog * pDlg = GetInterface().GetMainMenuDialog( CDnMainMenuDlg::SYSTEM_DIALOG );
		if( pDlg )
		{
			CDnSystemDlg * pSystemDlg = (CDnSystemDlg *)pDlg;
			int strId = pSystemDlg->IsGameQuitCharSelectStrIdx();
			if( strId == 0 )
				pSystemDlg->OnExitButton( true );
			else
				pSystemDlg->OnCharButton();				
		}

		//GetInterface().MessageBox( MakeDurabilityWarningStr(100038).c_str(), MB_YESNO, MESSAGEBOX_CHAR, this );
		//GetInterface().GetMainMenuDialog()->GetSystemDlg()->OnExitButton( true );
	}

	else
	{
		GetInterface().ShowGameQuitNextTimeRewardDlg( true, 
			m_nRewardType == GameQuitReward::RewardType::eType::NewbieReward ? m_nRewardType : 0 );
	}

}

#endif // PRE_ADD_GAMEQUIT_REWARD