#include "Stdafx.h"

#ifdef PRE_ADD_GAMEQUIT_REWARD

#include "TaskManager.h"
#include "DnCommonTask.h"
#include "DnItemTask.h"
#include "DnPVPLobbyVillageTask.h"

#include "DnInterface.h"
#include "DnMainMenuDlg.h"
#include "DnSystemDlg.h"

#include "SyncTimer.h"

#include "DnGameQuitNextTimeRewardDlg.h"

CDnGameQuitNextTimeRewardDlg::CDnGameQuitNextTimeRewardDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback, bool bAutoCursor )
: CDnCustomDlg( dialogType, pParentDialog, nID, pCallback, bAutoCursor )
,m_pStatic(NULL)
,m_nRewardType(0)
,m_bLogin(false)
{

}

CDnGameQuitNextTimeRewardDlg::~CDnGameQuitNextTimeRewardDlg(){
	
	ReleaseDlg();
	m_vecItemSlot.clear();
	
}


void CDnGameQuitNextTimeRewardDlg::ReleaseDlg()
{
	int size = (int)m_vecItemSlot.size();
	for( int i=0; i<size; ++i )
		m_vecItemSlot[i]->ResetSlot();
		
	size = (int)m_vecItem.size();
	for( int k=0; k<size; ++k )
		delete m_vecItem[k];
	m_vecItem.clear();
	

}

void CDnGameQuitNextTimeRewardDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName("NewbieGiftBoxListDlg.ui").c_str(), bShow );
}

void CDnGameQuitNextTimeRewardDlg::InitialUpdate()
{
	m_vecItemSlot.reserve( 21 );

	char str[64] = {0,};
	for( int i=0; i<21; ++i )
	{
		sprintf_s( str, 64, "ID_BT_ITEM%d", i );
		CDnItemSlotButton * pControl = GetControl<CDnItemSlotButton>( str );
		if( pControl )
			m_vecItemSlot.push_back( pControl );
	}

	m_pStatic = GetControl<CEtUIStatic>("ID_TEXT_ASK");	
	
}

void CDnGameQuitNextTimeRewardDlg::Show( bool bShow )
{
	if( m_bShow == bShow )
		return;

	if( bShow )
	{
	}
	else
	{
		m_bLogin = false;
	}

	CEtUIDialog::Show( bShow );
}

void CDnGameQuitNextTimeRewardDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl * pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );


	// Button.
	if( nCommand == EVENT_BUTTON_CLICKED )
	{	
		// 닫기.
		if( IsCmdControl("ID_BT_CLOSE") )
			Show( false );

		// 게임종료.
		if( IsCmdControl("ID_BT_CANCEL" ) )
		{
			// 로그인.
			if( m_bLogin )
			{
				
			}
			
			// 게임.
			else
			{
				CEtUIDialog * pDlg = GetInterface().GetMainMenuDialog( CDnMainMenuDlg::SYSTEM_DIALOG );
				if( pDlg )
				{
					CDnSystemDlg * pSystemDlg = (CDnSystemDlg *)pDlg;
					int strId = pSystemDlg->IsGameQuitCharSelectStrIdx();
					if( strId == 0 )
						GetInterface().GetMainMenuDialog()->GetSystemDlg()->OnExitButton( true );
					else
						GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, strId ), MB_YESNO, CDnSystemDlg::MESSAGEBOX_CHAR, pSystemDlg );						
				}
			}

			Show( false );
		}
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}


void CDnGameQuitNextTimeRewardDlg::SetNextTimeRewardType( int type, bool bCharSelect, int nLevel, int nClass )
{
	m_bLogin = bCharSelect;

	if( m_pStatic == NULL )
		return;	
	m_pStatic->ClearText();

	if( type < GameQuitReward::RewardType::eType::None || type > GameQuitReward::RewardType::eType::ComeBackReward )
		return;

	m_nRewardType = type;

	//// 신규유저.
	//if( type == GameQuitReward::RewardType::eType::NewbieReward )
	//	m_pStatic->AddColorText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7954 ) );	

	//// 귀환자유저.
	//else if( type == GameQuitReward::RewardType::eType::ComeBackReward )
	//	m_pStatic->AddColorText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7959 ) );	

	// 보상아이템 설정.
	SetNextTimeRewardItem( bCharSelect, nLevel, nClass );



	// Text 설정.
	CDnBridgeTask *pTask = (CDnBridgeTask *)CTaskManager::GetInstance().GetTask( "BridgeTask" );
	if( pTask )
	{
		__time64_t tEnd = CSyncTimer::GetInstance().GetCurTime(); // 현재시간.
		__time64_t tBegin = pTask->GetAccountRegTime();        // 계정생성시간.

		__time64_t elapsedTime = tEnd - tBegin; // 계정생성 후 경과시간.

		std::wstring str;	
	
		int newbieRewardTime = (int)CGlobalWeightIntTable::GetInstance().GetValue( CGlobalWeightIntTable::NewbieRewardTime ) * 3600; // 보상시간.
		
		// 아이템을 수령할 수 있는 시간충족.
		if( elapsedTime >= newbieRewardTime )
		{
			str.assign( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7972 ) );
		}

		// 아이템수령 시간 부족.
		else
		{
			// 보상을 받을 수 있는 시간.
			tBegin += newbieRewardTime; // elapsedTime;

			tm * _tm = _localtime64( &tBegin );

			// "%d년 %d월 %d일 %d시 %d분 이후 접속 시\n다음의 아이템을 받을 수 있습니다"
			wchar_t buf[128]={0,};
			swprintf_s( buf, 128, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7952 ), 
				        _tm->tm_year+1900, _tm->tm_mon+1, _tm->tm_mday, _tm->tm_hour, _tm->tm_min );
			str.assign( buf );			 
		}
		
		//m_pStatic->AddColorText( str.c_str() );
		m_pStatic->SetText( str.c_str() );
	}

}


// 보상대상에 따른 보상아이템 설정.
void CDnGameQuitNextTimeRewardDlg::SetNextTimeRewardItem( bool bCharSelect, int nLevel, int nClass )
{	
	if( m_nRewardType < GameQuitReward::RewardType::eType::NewbieReward || m_nRewardType > GameQuitReward::RewardType::eType::ComeBackReward )
		return;

	ReleaseDlg();
	
	// 캐릭터선택창에서 호출한 경우.
	if( bCharSelect )
	{
		// Table로부터 Item 찾기.	
		CDnItemTask::GetInstance().FindEventItems( 7, nLevel, nClass, m_vecItem );	
	}

	// 게임안에서 호출된경우 - Item참조를 한번만 하도록한다.
	else if( CDnActor::s_hLocalActor )
	{
		CDnActor * pActor = static_cast< CDnActor * >(CDnActor::s_hLocalActor);
		int nLevel = pActor->GetLevel();
		int nClass = pActor->GetClassID();

		// Table로부터 Item 찾기.	
		CDnItemTask::GetInstance().FindEventItems( 7, nLevel, nClass, m_vecItem );
	}

	// LocalPlayerActor 가 없는 경우.
	else
	{		
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
			// Table로부터 Item 찾기.	
			CDnItemTask::GetInstance().FindEventItems( 7, nLevel, nClass, m_vecItem );
		}
	}

	// ItemSlot설정.
	CDnItem * pItem = NULL;
	int size = (int)m_vecItem.size();
	int slotSize = (int)m_vecItemSlot.size();
	if( slotSize >= size )
	{
		for( int i=0; i<size; ++i )
		{
			pItem = m_vecItem[i];
			if( pItem )
				m_vecItemSlot[ i ]->SetItem( pItem, pItem->GetOverlapCount() );
		}
	}

}

#endif // PRE_ADD_GAMEQUIT_REWARD