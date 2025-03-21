#include "StdAfx.h"
#include "DnChannelDlg.h"
#include "DnLoginTask.h"
#include "TaskManager.h"
#include "DnInterface.h"
#include "DnInterfaceDlgID.h"
#include "PartySendPacket.h"
#include "DnTradeTask.h"
#include "DnTradePrivateMarket.h"
#include "DnMainMenuDlg.h"
#include "DnChatRoomTask.h"
#ifdef PRE_ADD_DWC
#include "DnDWCTask.h"
#endif
//blondymarry start
//#include "DnVillageTask.h"
//#include "DnPartyTask.h"
//blondymarry end

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnChannelDlg::CDnChannelDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
	: CDnChannelListDlg( dialogType, pParentDialog, nID, pCallback, true )
{
}

CDnChannelDlg::~CDnChannelDlg(void)
{
}

void CDnChannelDlg::InitialUpdate()
{
	CDnChannelListDlg::InitialUpdate();

	m_pButtonOK->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 95 ) );
	m_pButtonCancel->SetText( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 72 ) );
	m_pButtonCancel->SetHotKey( VK_ESCAPE );
}

void CDnChannelDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl("ID_OK" ) )
		{
			SendChannel();
		}
	}
	else if( nCommand == EVENT_LISTBOX_ITEM_DBLCLK )
	{
		if( IsCmdControl("ID_LISTBOX_CHANNEL" ) )
		{
			SendChannel();
		}
	}
	else if( nCommand == EVENT_LISTBOX_SELECTION )
	{
		if( IsCmdControl("ID_LISTBOX_CHANNEL" ) )
		{
			SListBoxItem *pItem = m_pChannelList->GetSelectedItem();
			if( pItem )
			{
				SChannelInfo *pInfo = (SChannelInfo*)(pItem->pData);
				if( CGlobalInfo::GetInstance().m_cLocalAccountLevel == 0 && pInfo->m_nCongestion == HIGH_CONGESTION )
				{
					m_pButtonOK->Enable( false );
				}
				else
				{
					m_pButtonOK->Enable( true );
				}
			}
		}
	}
	else if( nCommand == EVENT_RADIOBUTTON_CHANGED ) 
	{
		if( IsCmdControl( "ID_RBT_TAB0" ) ) 
		{
			m_eChannelAttribute = CHANNEL_ATT_NORMAL;
			ResetList();
		}
		else if( IsCmdControl( "ID_RBT_TAB1" ) ) 
		{
#ifdef PRE_ADD_DWC
			(GetDWCTask().IsDWCChar()) ? m_eChannelAttribute = CHANNEL_ATT_DWC: m_eChannelAttribute = CHANNEL_ATT_PVP;
#else
			m_eChannelAttribute = CHANNEL_ATT_PVP;
#endif
			
			ResetList();
		}
		else if( IsCmdControl( "ID_RBT_TAB2" ) ) 
		{
			m_eChannelAttribute = CHANNEL_ATT_DARKLAIR;
			ResetList();
		}
		else if( IsCmdControl( "ID_RBT_TAB3" ) ) 
		{
			m_eChannelAttribute = CHANNEL_ATT_FARMTOWN;
			ResetList();
		}
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnChannelDlg::Show( bool bShow ) 
{
	if( m_bShow == bShow )
		return;

	if( bShow )
	{
		SendReqMoveChannelList();

		if( GetInterface().GetMainMenuDialog( CDnMainMenuDlg::CHATROOM_DIALOG ) && GetInterface().GetMainMenuDialog( CDnMainMenuDlg::CHATROOM_DIALOG )->IsShow() )
			GetInterface().GetMainMenuDialog()->ShowChatRoomCreateDlg( false );
	}

	CDnChannelListDlg::Show( bShow );
}

void CDnChannelDlg::Process( float fElapsedTime )
{
	CDnChannelListDlg::Process( fElapsedTime );
}

void CDnChannelDlg::SendChannel()
{
	SListBoxItem *pItem = m_pChannelList->GetSelectedItem();
	if( !pItem ) return;

	CDnChannelListDlg::SChannelInfo *pInfo = (CDnChannelListDlg::SChannelInfo*)(pItem->pData);
	if( CGlobalInfo::GetInstance().m_cLocalAccountLevel == 0 && pInfo->m_nCongestion == HIGH_CONGESTION )
	{
		GetInterface().MessageBox( 97, MB_OK );
		return;
	}
/*
	//일단은 파티에 속했다면 처리를 시작 
	CDnPartyTask* pPartyTask  = static_cast<CDnPartyTask*>(CTaskManager::GetInstancePtr()->GetTask( "PartyTask" ));

	if( pPartyTask != NULL && pPartyTask->GetPartyState() != CDnPartyTask::PartyStateEnum::SINGLE )
	{
		//blondymarry start 이동하려는 채널이 투기장마을인지 아니면 일반 마을인지 비교해서 같은 투기장끼리는 경고 없고 일반->투기장은 메세지 박스를 띄운다
		CDnVillageTask *pTask = (CDnVillageTask *)CTaskManager::GetInstance().GetTask( "VillageTask" );

		if( pTask != NULL )
		{
			if( !pTask->GetPVPType())//현재 있는 마을은 PVP마을이 아님  
			{
				if( pInfo->m_nChannelType == 1 )//갈려고 하는 마을은 PVP마을
				{
					GetInterface().MessageBox(L"임시:PVP채널로 이동하면 파티를 탈퇴하게 됩니다." , MB_OK ,MESSAGEBOX_36);					
				}			
			}
		}

	}	
*/
//blondymarry end

	// 혹시 요청중인 거래가 있다면 취소하고 이동패킷을 요청한다.
	GetTradeTask().GetTradePrivateMarket().ClearTradeUserInfoList();
	GetInterface().CloseAcceptRequestDialog(true);
	GetInterface().DisableCashShopMenuDlg(true);

	if( CDnChatRoomTask::IsActive() && GetChatRoomTask().IsRequestWait() )
		return;

	SendReqMoveSelectChannel(pInfo->m_nChannelID );
}

void CDnChannelDlg::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg )
{
	/*
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl( "ID_YES" ) ) 
		{
			switch( nID )
			{
			    case MESSAGEBOX_37:
					//파티탈퇴
					GetPartyTask().ReqOutParty();
					//이동
					SendReqMoveSelectChannel(m_nChannelID);
					break;
			}
		}
	}
	*/
}