#include "StdAfx.h"
#include "DnCommunityDlg.h"
#include "DnPartyListDlg.h"
#include "DnPartyInfoListDlg.h"
#include "DnFriendDlg.h"
#include "DnGuildDlg.h"
#include "DnBlockDlg.h"
#include "DnNestDlg.h"
#include "DnMainMenuDlg.h"
#include "DnLocalPlayerActor.h"
#include "DnMasterListDlg.h"
#include "DnMasterInfoDlg.h"
#include "DnPupilListDlg.h"
#include "DnInterface.h"
#include "TaskManager.h"
#include "DnVillageTask.h"
#include "DnTableDB.h"
#include "DnGuildBaseDlg.h"
#ifdef PRE_ADD_BESTFRIEND
#include "DnBestFriendDlg.h"
#endif
#ifdef PRE_PRIVATECHAT_CHANNEL
#include "DnPrivateChannelDlg.h"
#endif // PRE_PRIVATECHAT_CHANNEL
#ifdef PRE_ADD_DWC
#include "DnDwcDlg.h"
#include "VillageSendPacket.h"
#include "DnDWCTask.h"
#endif

#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

CDnCommunityDlg::CDnCommunityDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback  )
	: CEtUITabDialog( dialogType, pParentDialog, nID, pCallback, true )
	, m_pPartyListDlg(NULL)
	, m_pPartyInfoListDlg(NULL)
	, m_pFriendDlg(NULL)
	, m_pGuildBaseDlg(NULL)
	, m_pBlockDlg(NULL)
	, m_pMasterListDlg(NULL)
	, m_pMasterInfoDlg(NULL)
	, m_pPupilListDlg(NULL)
	, m_pNestDlg(NULL)
#ifdef PRE_ADD_DWC
	, m_pDWCDlg(NULL)
#endif
#ifdef PRE_ADD_BESTFRIEND
	, m_pBestFriendDlg(NULL)
	, m_pTabBestFriend( NULL )
#endif
#ifdef PRE_PRIVATECHAT_CHANNEL
	, m_pTabPrivateChannel( NULL )
	, m_pPrivateChannelDlg( NULL )
#endif // PRE_PRIVATECHAT_CHANNEL
	, m_pTabParty(NULL)
	, m_pTabGuild(NULL)
	, m_pTabFriend(NULL)
	, m_pTabBlock(NULL)
	, m_pTabMaster(NULL)
	, m_pTabNestInfo(NULL)
	, m_emPartyDialogType(typePartyList)
{
}

CDnCommunityDlg::~CDnCommunityDlg(void)
{
	if( typePartyInfoList == m_emPartyDialogType )
	{
		SAFE_DELETE(m_pPartyListDlg);
	}
	else
	{
		SAFE_DELETE(m_pPartyInfoListDlg);
	}

	if( typeMasterList == m_emMasterDialogType )
	{
		SAFE_DELETE( m_pMasterInfoDlg );
		SAFE_DELETE( m_pPupilListDlg );
	}
	else if( typeMasterInfo == m_emMasterDialogType )
	{
		SAFE_DELETE( m_pMasterListDlg );
		SAFE_DELETE( m_pPupilListDlg );
	}
	else if( typePupilList == m_emMasterDialogType )
	{
		SAFE_DELETE( m_pMasterListDlg );
		SAFE_DELETE( m_pMasterInfoDlg );
	}

#ifdef PRE_ADD_DWC
	////SAFE_DELETE(m_pDWCDlg);
#endif // PRE_ADD_DWC
}

void CDnCommunityDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "CommunityTabDlg.ui" ).c_str(), bShow );
}

void CDnCommunityDlg::InitialUpdate()
{
	m_pPartyInfoListDlg = new CDnPartyInfoListDlg( UI_TYPE_CHILD, this );
	m_pPartyInfoListDlg->Initialize( false );

#ifdef PRE_ADD_DWC
	if(GetDWCTask().IsDWCChar())
	{
		m_pTabParty = GetControl<CEtUIRadioButton>("ID_TAB_PARTY");
		m_pDWCDlg = new CDnDwcDlg(UI_TYPE_CHILD, this);
		m_pDWCDlg->Initialize(false);
		AddTabDialog( m_pTabParty, m_pDWCDlg );

		m_pTabParty->SetText(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 120263)); // 팀원설정
	}
	else
	{
		m_pTabParty = GetControl<CEtUIRadioButton>("ID_TAB_PARTY");
		m_pPartyListDlg = new CDnPartyListDlg( UI_TYPE_CHILD, this );
		m_pPartyListDlg->Initialize( false );
		AddTabDialog( m_pTabParty, m_pPartyListDlg );
		
		m_pTabParty->SetText(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 632)); // 파티 : 632
	}
#else
	m_pTabParty = GetControl<CEtUIRadioButton>("ID_TAB_PARTY");
	m_pPartyListDlg = new CDnPartyListDlg( UI_TYPE_CHILD, this );
	m_pPartyListDlg->Initialize( false );
	AddTabDialog( m_pTabParty, m_pPartyListDlg );
#endif

	m_pTabFriend = GetControl<CEtUIRadioButton>("ID_TAB_FRIEND");
	m_pFriendDlg = new CDnFriendDlg( UI_TYPE_CHILD, this );
	m_pFriendDlg->Initialize( false );
	AddTabDialog( m_pTabFriend, m_pFriendDlg );
#ifdef PRE_ADD_DWC
	m_pTabFriend->Show(!GetDWCTask().IsDWCChar());
#endif

	m_pTabBlock = GetControl<CEtUIRadioButton>("ID_TAB_BLOCK");
	m_pBlockDlg = new CDnBlockDlg( UI_TYPE_CHILD, this );
	m_pBlockDlg->Initialize( false );
	AddTabDialog( m_pTabBlock, m_pBlockDlg );

	//rlkt_test
	/*m_pTabGuild = GetControl<CEtUIRadioButton>("ID_TAB_GUILD");
	m_pGuildBaseDlg = new CDnGuildBaseDlg( UI_TYPE_CHILD, this );
	m_pGuildBaseDlg->Initialize( false );
	AddTabDialog( m_pTabGuild, m_pGuildBaseDlg );
	*/


	m_pTabMaster = GetControl<CEtUIRadioButton>("ID_TAB_MASTER");
	m_pTabMaster->Show( true );
	m_pMasterListDlg = new CDnMasterListDlg( UI_TYPE_CHILD, this );
	m_pMasterListDlg->Initialize( false );
	m_pMasterInfoDlg = new CDnMasterInfoDlg( UI_TYPE_CHILD, this );
	m_pMasterInfoDlg->Initialize( false );
	m_pPupilListDlg = new CDnPupilListDlg( UI_TYPE_CHILD, this );
	m_pPupilListDlg->Initialize( false );
	//AddTabDialog( m_pTabMaster, m_pMasterListDlg );
	//m_emMasterDialogType = typeMasterList;

	if( !CDnActor::s_hLocalActor ) return;
	CDnLocalPlayerActor * localActor = ((CDnLocalPlayerActor *)(CDnActor::s_hLocalActor.GetPointer()));

	if( localActor->GetLevel() > static_cast<int>(CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::MasterSystem_PupilMaxLevel)) )
	{
		AddTabDialog( m_pTabMaster, m_pPupilListDlg );
		m_emMasterDialogType = typePupilList;
	}
	else
	{
		AddTabDialog( m_pTabMaster, m_pMasterListDlg );
		m_emMasterDialogType = typeMasterList;
	}
	m_pTabNestInfo = GetControl<CEtUIRadioButton>("ID_TAB_DLIST");
	m_pNestDlg = new CDnNestDlg( UI_TYPE_CHILD, this );
	m_pNestDlg->Initialize( false );
	AddTabDialog( m_pTabNestInfo, m_pNestDlg );
	
	//rlkt_test move guild to best friend tab.
	//uistring: 613
	GetControl<CEtUIRadioButton>("ID_TAB_BF")->SetText(GetEtUIXML().GetUIString(CEtUIXML::idCategory1, 613));
	GetControl<CEtUIRadioButton>("ID_TAB_BF")->Show(true);
	m_pTabGuild = GetControl<CEtUIRadioButton>("ID_TAB_BF");
	m_pGuildBaseDlg = new CDnGuildBaseDlg( UI_TYPE_CHILD, this );
	m_pGuildBaseDlg->Initialize( false );
	AddTabDialog( m_pTabGuild, m_pGuildBaseDlg );

	/*
#ifdef PRE_ADD_BESTFRIEND
	m_pTabBestFriend = GetControl<CEtUIRadioButton>("ID_TAB_BF");
	m_pTabBestFriend->Show( false );
	m_pBestFriendDlg = new CDnBestFriendDlg( UI_TYPE_CHILD, this );
	m_pBestFriendDlg->Initialize( false );
	AddTabDialog( m_pTabBestFriend, m_pBestFriendDlg );
#endif*/
#ifdef PRE_PRIVATECHAT_CHANNEL
	m_pTabPrivateChannel = GetControl<CEtUIRadioButton>( "ID_TAB_CHANNEL" );
	m_pTabPrivateChannel->Show( true );
	m_pPrivateChannelDlg = new CDnPrivateChannelDlg( UI_TYPE_CHILD, this );
	m_pPrivateChannelDlg->Initialize( false );
	AddTabDialog( m_pTabPrivateChannel, m_pPrivateChannelDlg );
#endif // PRE_PRIVATECHAT_CHANNEL

	SetCheckedTab( m_pTabParty->GetTabID() );


//#ifdef PRE_ADD_SHORTCUT_HELP_DIALOG 
//	m_vecTabIDList.clear();
//
//	m_vecTabIDList.push_back(m_pTabParty->GetTabID());
//	m_vecTabIDList.push_back(m_pTabFriend->GetTabID());
//	m_vecTabIDList.push_back(m_pTabBlock->GetTabID());
//	m_vecTabIDList.push_back(m_pTabGuild->GetTabID());
//	m_vecTabIDList.push_back(m_pTabNestInfo->GetTabID());
//	
//	m_vecTabIDList.push_back(m_pTabMaster->GetTabID());
//
//#ifdef PRE_ADD_BESTFRIEND
//	m_vecTabIDList.push_back(m_pTabPrivateChannel->GetTabID());
//#endif
//
//#ifdef PRE_PRIVATECHAT_CHANNEL
//	m_vecTabIDList.push_back(m_pTabBestFriend->GetTabID());
//#endif
//
//#endif // PRE_ADD_SHORTCUT_HELP_DIALOG


	//m_pTabFriend->Show( false );
	//m_pTabBlock->Show( false );
	//m_pTabGuild->Show( false );

	m_SmartMoveEx.SetControl( m_pTabParty );
}

#ifdef PRE_ADD_DWC
void CDnCommunityDlg::SetCommunityTab(bool bHasDWCTeam)
{
	if(!m_pTabParty || !m_pTabFriend) return;

	if(bHasDWCTeam)
	{
		m_pTabParty->Enable(true);
		m_pTabParty->SetChecked(true);
	}
	else
	{
		// 팀이 없으니 비활성화 시켜서 클릭 방지
		m_pTabParty->Enable(false); // DWC모드일때는 파티탭이 DWC탭 ( 공용으로 사용중 )
		//SetCheckedTab(m_pTabFriend->GetTabID());
		//ShowChildDialog(m_pFriendDlg, true);
	}
}

void CDnCommunityDlg::SetDwcUIControls()
{
	// DWC에서 사용하는 컨트롤들을 On/OFF한다
	if( GetDWCTask().IsDWCChar() )
	{
		bool bIsHaveDwcTeam = GetDWCTask().HasDWCTeam();
		m_pTabParty->Enable(bIsHaveDwcTeam);
		if(m_pTabParty->IsEnable())
			SetCheckedTab( m_pTabParty->GetTabID() );

		//m_pTabFriend->Enable(true);
		m_pTabFriend->Show(false);

		m_pTabBlock->Show(false);
		m_pTabGuild->Show(false);
		m_pTabNestInfo->Show(false);
		m_pTabMaster->Show(false);
#ifdef PRE_PRIVATECHAT_CHANNEL
		m_pTabPrivateChannel->Show(false);
#endif
#ifdef PRE_ADD_BESTFRIEND
		m_pTabBestFriend->Show(false);
#endif
	}
}
#endif

void CDnCommunityDlg::Show( bool bShow )
{
	if( m_bShow == bShow )
		return;

	if( bShow )
	{
		if( !CDnActor::s_hLocalActor ) return;
		CDnPlayerActor* pPlayer = dynamic_cast<CDnPlayerActor*>(CDnActor::s_hLocalActor.GetPointer());
		bool bDisable = (pPlayer->IsJoinGuild() == false && CDnWorld::GetInstance().GetMapType() != CDnWorld::MapTypeVillage);
		if( m_pTabGuild ) {
			m_pTabGuild->Enable( !bDisable );
			if( bDisable && GetCurrentTabID() == m_pTabGuild->GetTabID() )
				SetCheckedTab( m_pTabParty->GetTabID() );
		}

		if( m_pTabParty ) {
			bool bPvpVillage = false;
			bool bPvpGame = false; 
			bool bFarmVillage = false;
			bool bFarmGame = false;

			CDnVillageTask *pVillageTask = (CDnVillageTask *)CTaskManager::GetInstance().GetTask( "VillageTask" );
			if( pVillageTask && pVillageTask->GetVillageType() == CDnVillageTask::PvPVillage )
				bPvpVillage = true;
			else if( pVillageTask && pVillageTask->GetVillageType() == CDnVillageTask::FarmVillage )
				bFarmVillage = true;

			if (GetInterface().IsPVP())
				bPvpGame = true;
			else if (GetInterface().IsFarm())
				bFarmGame = true;

			m_pTabParty->Enable( !bPvpVillage && !bPvpGame && !bFarmGame && !bFarmVillage );
			m_pTabMaster->Enable( !bPvpVillage && !bPvpGame && !bFarmGame && !bFarmVillage );
			if( (bPvpGame || bPvpVillage || bFarmVillage || bFarmGame) && (GetCurrentTabID() == m_pTabParty->GetTabID() 
				|| GetCurrentTabID() == m_pTabMaster->GetTabID()) )
				SetCheckedTab( m_pTabFriend->GetTabID() );

			if( !CDnActor::s_hLocalActor ) return;
			CDnLocalPlayerActor * localActor = ((CDnLocalPlayerActor *)(CDnActor::s_hLocalActor.GetPointer()));

			if( localActor->GetLevel() > static_cast<int>(CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::MasterSystem_PupilMaxLevel)) )
				ChangeMasterDialog( typePupilList );
			else
			{
				if( GetMasterTask().GetSimpleInfo().iMasterCount == 0 )
					ChangeMasterDialog( typeMasterList );
				else
					ChangeMasterDialog( typeMasterInfo );
			}
		}
#if defined(PRE_ADD_DWC)
		SetDwcUIControls();
#endif // PRE_ADD_DWC

		m_SmartMoveEx.MoveCursor();
	}

	CEtUITabDialog::Show( bShow );
//	CDnMouseCursor::GetInstance().ShowCursor( bShow );

	if( m_pCallback )
	{
		m_pCallback->OnUICallbackProc( GetDialogID(), bShow?EVENT_DIALOG_SHOW:EVENT_DIALOG_HIDE, NULL );
	}
}

void CDnCommunityDlg::Process( float fElapsedTime )
{
	m_SmartMoveEx.Process();
	CEtUITabDialog::Process( fElapsedTime );
}

int CDnCommunityDlg::GetPartyListStartIndex()
{
	return m_pPartyListDlg->GetStartIndex();
}

void CDnCommunityDlg::ShowPartyDialog( emPartyDialogType typePartyDialog )
{
	SwapPartyDialog( typePartyDialog );
}

void CDnCommunityDlg::SwapPartyDialog( emPartyDialogType typePartyDialog )
{
#ifdef PRE_ADD_DWC
	if( GetDWCTask().IsDWCChar() == false )
#endif
	{
		m_emPartyDialogType = typePartyDialog;

		if( IsShow() )
		{
			int tabId = m_pTabParty->GetTabID();
			if( m_emPartyDialogType == typePartyInfoList )
			{
				m_groupTabDialog.ShowDialog(GetCurrentTabID(), false);
				m_groupTabDialog.ChangeDialog(tabId, m_pPartyInfoListDlg);
				m_groupTabDialog.ShowDialog(tabId, true);
			}
			else
			{
				m_groupTabDialog.ShowDialog(GetCurrentTabID(), false);
				m_groupTabDialog.ChangeDialog(tabId, m_pPartyListDlg);
				m_groupTabDialog.ShowDialog(tabId, true);
			}

			SetCheckedTab(tabId);
		}
		else
		{
			if( m_emPartyDialogType == typePartyInfoList )
			{
				m_groupTabDialog.ChangeDialog( m_pTabParty->GetTabID(), m_pPartyInfoListDlg );
			}
			else
			{
				m_groupTabDialog.ChangeDialog( m_pTabParty->GetTabID(), m_pPartyListDlg );
			}
		}
	}
}

bool CDnCommunityDlg::IsShowPartyDialog( emPartyDialogType typePartyDialog )
{
	if( m_emPartyDialogType == typePartyInfoList )
	{
		return m_pPartyListDlg->IsShow();
	}
	else
	{
		return m_pPartyInfoListDlg->IsShow();
	}
}

void CDnCommunityDlg::RefreshPartyList()
{
	m_pPartyListDlg->RefreshList();
}

void CDnCommunityDlg::RefreshPartyInfoList()
{
	m_pPartyInfoListDlg->RefreshList();
}

void CDnCommunityDlg::RefreshPartyMemberList()
{
	m_pPartyListDlg->RefreshPartyMemberList();
}

void CDnCommunityDlg::OnPartyJoin()
{
	m_pPartyInfoListDlg->OnPartyJoin();
}

void CDnCommunityDlg::SetPartyInfoLeader( int nLeaderIndex )
{
	m_pPartyInfoListDlg->SetPartyInfoLeader( nLeaderIndex );
}

void CDnCommunityDlg::SetPartyCreateDifficulties(const BYTE* pDifficulties)
{
	m_pPartyListDlg->SetDifficultyToPartyCreateDlg(pDifficulties);
}

void CDnCommunityDlg::OnRecvPartyMemberInfo(ePartyType type)
{
	m_pPartyListDlg->OnRecvPartyMemberInfo(type);
}

void CDnCommunityDlg::OnPartyJoinFailed()
{
	m_pPartyListDlg->OnPartyJoinFailed();
}

void CDnCommunityDlg::ChangeMasterDialog( emMasterDilogType nType )
{
	if( m_emMasterDialogType == nType )
		return;

	m_emMasterDialogType = nType;

	if( typeMasterList == nType )
	{
		ChangeDialog( m_pTabMaster, m_pMasterListDlg );
		m_pMasterListDlg->Show( true );
	}
	else if( typeMasterInfo == nType )
	{
		ChangeDialog( m_pTabMaster, m_pMasterInfoDlg );
		m_pMasterInfoDlg->Show( true );
	}
	else if( typePupilList == nType )
	{
		ChangeDialog( m_pTabMaster, m_pPupilListDlg );
		m_pPupilListDlg->Show( true );
	}
}

void CDnCommunityDlg::RefreshMasterListDialog()
{
	m_pMasterListDlg->RefreshList();
}

void CDnCommunityDlg::RefreshMasterSimpleInfo()
{
	if( !CDnActor::s_hLocalActor ) return;
	CDnLocalPlayerActor * localActor = ((CDnLocalPlayerActor *)(CDnActor::s_hLocalActor.GetPointer()));

	m_pMasterListDlg->RefreshSimple();
}

void CDnCommunityDlg::RefreshMasterCharacterInfo()
{
	if( !CDnActor::s_hLocalActor ) return;
	CDnLocalPlayerActor * localActor = ((CDnLocalPlayerActor *)(CDnActor::s_hLocalActor.GetPointer()));

	if( localActor->GetLevel() > static_cast<int>(CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::MasterSystem_PupilMaxLevel)) && GetMasterTask().GetSimpleInfo().iMasterCount == 0 )
		m_pPupilListDlg->RefreshIntroduction();
}

void CDnCommunityDlg::RefreshMasterClassmate()
{
	m_pMasterInfoDlg->RefreshTree();
}

void CDnCommunityDlg::RefreshPupilList()
{
	if( !CDnActor::s_hLocalActor ) return;
	CDnLocalPlayerActor * localActor = ((CDnLocalPlayerActor *)(CDnActor::s_hLocalActor.GetPointer()));

	if( localActor->GetLevel() > static_cast<int>(CGlobalWeightTable::GetInstance().GetValue(CGlobalWeightTable::MasterSystem_PupilMaxLevel)) && GetMasterTask().GetSimpleInfo().iMasterCount == 0 )
		m_pPupilListDlg->RefreshPupilList();
	else
		m_pMasterListDlg->RefreshMaster();
}

void CDnCommunityDlg::RefreshMyMasterInfo()
{
	m_pMasterInfoDlg->RefreshMyMasterInfo();
}

void CDnCommunityDlg::RefreshClassmateInfo()
{
	m_pMasterInfoDlg->RefreshClassmateInfo();
}

void CDnCommunityDlg::RefreshPupilLeave()
{
	m_pPupilListDlg->RefreshLeave();
}

void CDnCommunityDlg::RefreshRespectPoint()
{
	m_pPupilListDlg->RefreshRespectPoint();
}

CDnGuildDlg *CDnCommunityDlg::GetGuildDialog()
{
	return m_pGuildBaseDlg->GetGuildDialog();
}

#ifdef PRE_ADD_GUILD_EASYSYSTEM
CDnGuildWantedListTabDlg *CDnCommunityDlg::GetGuildWantedListTabDialog()
{
	return m_pGuildBaseDlg->GetGuildWantedListTabDialog();
}
#else
CDnGuildWantedListDlg *CDnCommunityDlg::GetGuildWantedListDialog()
{
	return m_pGuildBaseDlg->GetGuildWantedListDialog();
}
#endif


#ifdef PRE_ADD_BESTFRIEND

// 절친정보.
void CDnCommunityDlg::SetBFData( struct TBestFriendInfo & bfInfo )
{
	m_pTabBestFriend->Show( true );

	if( m_pBestFriendDlg )
		m_pBestFriendDlg->SetBFData( bfInfo );
}

// 절친정보Dlg.
void CDnCommunityDlg::OpenBestFriendDlg( bool bShow )
{
	if( m_pBestFriendDlg )
		m_pBestFriendDlg->Show( bShow );
}

// 메모수신.
void CDnCommunityDlg::SetMemo( bool bFromMe, WCHAR * pStrMemo )
{
	if( m_pBestFriendDlg )
		m_pBestFriendDlg->SetMemo( bFromMe, pStrMemo );
}

// 절친파기 or 파기취소.
void CDnCommunityDlg::BrokeupOrCancelBF( bool bCancel, WCHAR * strName )
{
	if( m_pBestFriendDlg )
		m_pBestFriendDlg->BrokeupOrCancelBF( bCancel, strName );
}

// 절친파기.
void CDnCommunityDlg::DestroyBF()
{
	if( GetCurrentTabID() == m_pTabBestFriend->GetTabID() )
	{
		if( IsShow() )
			Show( false );
	}
	
	m_pTabBestFriend->Show( false );
}


#endif

#ifdef PRE_ADD_NEWCOMEBACK
void CDnCommunityDlg::SetComebackAppellation( UINT sessionID, bool bComeback )
{
	if( m_pPartyInfoListDlg )
		m_pPartyInfoListDlg->SetComebackAppellation( sessionID, bComeback );
}
#endif // PRE_ADD_NEWCOMEBACK


//#ifdef PRE_ADD_SHORTCUT_HELP_DIALOG
//void CDnCommunityDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
//{
//	SetCmdControlName( pControl->GetControlName() );
//
//	if( nCommand == EVENT_BUTTON_CLICKED )
//	{
//		if( IsCmdControl("ID_BT_SMALLHELP") )
//		{
//			if(m_vecTabIDList.empty())
//				return;
//
//			int nCurrentTabID = -1;
//			for( int i = 0 ; i < (int)m_vecTabIDList.size() ; ++i )
//			{
//				if(GetCurrentTabID() == m_vecTabIDList[i])
//				{
//					nCurrentTabID = m_vecTabIDList[i];
//					break;
//				}
//			}
//			
//			eShortCutHelpDlgType eType;
//			if( nCurrentTabID == eRadioTap_Party )
//				eType = HELP_SHORTCUT_PARTY_PLAY;
//			else if( nCurrentTabID == eRadioTap_Friend )
//				eType = HELP_SHORTCUT_FRIEND;
//			else if( nCurrentTabID == eRadioTap_Master )
//				eType = HELP_SHORTCUT_MASTER;
//			else if( nCurrentTabID == eRadioTap_Guild )
//				eType = HELP_SHORTCUT_GUILD;
//			else if( nCurrentTabID == eRadioTap_DList )
//				eType = HELP_SHORTCUT_STAGE;
//
//			CDnMainMenuDlg* pMainMenuDlg = GetInterface().GetMainMenuDialog();
//			CDnSystemDlg* pSystemDlg = pMainMenuDlg->GetSystemDlg();
//			if(pSystemDlg)
//				pSystemDlg->ShowChoiceHelpDlg(eType);
//		}
//	}
//
//	CEtUITabDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
//}
//#endif