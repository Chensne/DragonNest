#include "StdAfx.h"
#include "DnSystemDlg.h"
#include "DnInterfaceDlgID.h"
#include "DnHelpDlg.h"
#include "DnChannelDlg.h"
#include "DnMainMenuDlg.h"
#include "DnGRBDlg.h"
#include "TaskManager.h"
#include "DnGameOptDlg.h"
#include "DnSystemOptDlg.h"
#include "DnInterface.h"
#include "DnBridgeTask.h"

#include "DnVillageTask.h"
#include "DnGameTask.h"

#ifdef PRE_ADD_PVP_COMBOEXERCISE
#include "DnPvPGameTask.h"
#endif // PRE_ADD_PVP_COMBOEXERCISE

#include "ItemSendPacket.h"
#include "DnLocalPlayerActor.h"
#include "SystemSendPacket.h"
#include "DnServiceSetup.h"
#include "DnAuthTask.h"
#include "DnTableDB.h"
#ifdef PRE_ADD_VIP
#include "DnVIPDlg.h"
#endif
#include "DnGameKeySetDlg.h"

#ifdef PRE_ADD_GAMEQUIT_REWARD
#include "DnCommonTask.h"
#endif // PRE_ADD_GAMEQUIT_REWARD

#ifdef PRE_ADD_DWC
#include "DnDWCTask.h"
#endif // PRE_ADD_DWC


#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif

bool g_bEndingBanner = false;
CDnSystemDlg::CDnSystemDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
	: CEtUIDialog( dialogType, pParentDialog, nID, pCallback, true )
	, m_pButtonHelp(NULL)
	, m_pButtonChannel(NULL)
	, m_pButtonHelpKeyboard(NULL)
	, m_pButtonGame(NULL)
	, m_pButtonSystem(NULL)
	, m_pButtonStage(NULL)
	, m_pButtonChar(NULL)
#ifdef _KRAZ
	, m_pButtonGRB(NULL)
#endif
	, m_pButtonExit(NULL)
	, m_pHelpDlg(NULL)
	, m_pGameOptDlg(NULL)
	, m_pSystemOptDlg(NULL)
	, m_pChannelListDlg(NULL)
	, m_pGameGradeDlg(NULL)
	, m_bOpenChannel(false)
#ifdef PRE_ADD_VIP
	, m_pButtonVIP(NULL)
	, m_pVIPDlg(NULL)
#endif
	, m_pEventTodayDlg(NULL)
	, m_pGameKeySetDlg( NULL )
#ifdef PRE_ADD_GAMEQUIT_REWARD
	,m_nGameQuitCharSelectStrIdx(0)
#endif // PRE_ADD_GAMEQUIT_REWARD
{
}

CDnSystemDlg::~CDnSystemDlg(void)
{
}

void CDnSystemDlg::Initialize(bool bShow)
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "SystemDlg.ui" ).c_str(), bShow );
}

void CDnSystemDlg::InitialUpdate()
{
	m_pButtonHelp = GetControl<CEtUIButton>("ID_BUTTON_HELP");
#ifdef PRE_ADD_NO_HELP_DLG
	m_pButtonHelp->Enable(false);
#endif
	m_pButtonChannel = GetControl<CEtUIButton>("ID_BUTTON_CHANNEL");
	m_pButtonHelpKeyboard = GetControl<CEtUIButton>("ID_BUTTON_CONTROL");
	m_pButtonGame = GetControl<CEtUIButton>("ID_BUTTON_GAME");
	m_pButtonSystem = GetControl<CEtUIButton>("ID_BUTTON_SYSTEM");
	m_pButtonStage = GetControl<CEtUIButton>("ID_BUTTON_STAGE");
	m_pButtonChar = GetControl<CEtUIButton>("ID_BUTTON_CHAR");
#ifdef _KRAZ
	m_pButtonGRB = GetControl<CEtUIButton>("ID_BUTTON_GRB");
#endif
	m_pButtonExit = GetControl<CEtUIButton>("ID_BUTTON_EXIT");
#ifdef PRE_ADD_VIP
	m_pButtonVIP = GetControl<CEtUIButton>("ID_BUTTON_VIP");
	if (m_pButtonVIP)
		m_pButtonVIP->Enable(false);
#endif

	m_pButtonGame->Enable( true );
	
	m_pHelpDlg = new CDnHelpDlg( UI_TYPE_MODAL, NULL, HELP_DIALOG, this );
	m_pHelpDlg->Initialize( false );
	m_SystemDlgGroup.AddDialog( HELP_DIALOG, m_pHelpDlg );

	m_pChannelListDlg = new CDnChannelDlg( UI_TYPE_MODAL, NULL, CHANNEL_DIALOG, this );
	m_pChannelListDlg->Initialize( false );
	m_SystemDlgGroup.AddDialog( CHANNEL_DIALOG, m_pChannelListDlg );

	m_pGameOptDlg = new CDnGameOptDlg( UI_TYPE_MODAL, NULL, GAME_OPTION_DIALOG, this );
	m_pGameOptDlg->Initialize( false );
	m_SystemDlgGroup.AddDialog( GAME_OPTION_DIALOG, m_pGameOptDlg );

	m_pSystemOptDlg = new CDnSystemOptDlg( UI_TYPE_MODAL, NULL, SYSTEM_OPTION_DIALOG, this );
	m_pSystemOptDlg->Initialize( false );
	m_SystemDlgGroup.AddDialog( SYSTEM_OPTION_DIALOG, m_pSystemOptDlg );

	m_pGameGradeDlg = new CDnGRBDlg( UI_TYPE_MODAL, NULL, GAME_GRAGE_DIALOG, this );
	m_pGameGradeDlg->Initialize( false );
	m_SystemDlgGroup.AddDialog( GAME_GRAGE_DIALOG, m_pGameGradeDlg );

#ifdef PRE_ADD_VIP
	m_pVIPDlg = new CDnVIPDlg(UI_TYPE_MODAL, NULL, VIP_DIALOG, this);
	m_pVIPDlg->Initialize(false);
	m_SystemDlgGroup.AddDialog(VIP_DIALOG, m_pVIPDlg);
#endif // PRE_ADD_VIP

	m_pGameKeySetDlg = new CDnGameKeySetDlg(UI_TYPE_MODAL, NULL, GAME_KEY_DIALOG, this);
	m_pGameKeySetDlg->Initialize(false);
	m_SystemDlgGroup.AddDialog(GAME_KEY_DIALOG, m_pGameKeySetDlg);
}

void CDnSystemDlg::Show( bool bShow ) 
{
	if( m_bShow == bShow )
		return;

	if( bShow )
	{
		m_SystemDlgGroup.CloseAllDialog();

#ifdef PRE_ADD_VIP
		if (m_pButtonVIP)
		{
			const CDnVIPDataMgr* pDataMgr = GetInterface().GetLocalPlayerVIPDataMgr();
			if (pDataMgr != NULL)
				m_pButtonVIP->Enable(pDataMgr->IsVIP());
		}
#endif // PRE_ADD_VIP
	}
	CEtUIDialog::Show( bShow );
//	CDnMouseCursor::GetInstance().ShowCursor( bShow );

	// 진짜 어쩔 수 없이 조이패드때문에..
	if( bShow )
	{
		m_nHotKey = VK_ESCAPE;
		m_HotKeyState = (SHORT)0xff80;
	}

	if( m_pCallback )
	{
		m_pCallback->OnUICallbackProc( GetDialogID(), bShow?EVENT_DIALOG_SHOW:EVENT_DIALOG_HIDE, NULL );
	}
}

void CDnSystemDlg::OnCloseCompletely()
{
	m_SystemDlgGroup.CloseAllDialog();
}

void CDnSystemDlg::Process( float fElapsedTime )
{
	CEtUIDialog::Process( fElapsedTime );

	if( IsShow() )
	{
		// Note : 마을인지 체크해서 스테이지 포기 버튼 비/활성화
		//
		bool bEnableStageGiveUp = false;
		switch( CDnWorld::GetInstance().GetMapType() ) {
			case CDnWorld::MapTypeDungeon:
			case CDnWorld::MapTypeWorldMap:
				bEnableStageGiveUp = true;
				break;
		}

		// Enable이더라도 맵테이블에 비활성이라고 체크되어있으면 비활성으로 바꾼다.
		if( bEnableStageGiveUp )
		{
			DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TMAP );
			int nID = CGlobalInfo::GetInstance().m_nCurrentMapIndex;
			if( pSox && nID )
			{
				if( pSox->GetFieldFromLablePtr( nID, "_ReturnVillage" )->GetInteger() == FALSE )
					bEnableStageGiveUp = false;
			}
		}

		m_pButtonStage->Enable( bEnableStageGiveUp );

		bool bPvP = false;
		CDnGameTask *pGameTask = (CDnGameTask *)CTaskManager::GetInstance().GetTask( "GameTask" );

		if( pGameTask ) {
			switch( pGameTask->GetGameTaskType() ) {
				case GameTaskType::Normal:
				case GameTaskType::DarkLair:
				case GameTaskType::Farm:
					break;
				case GameTaskType::PvP:
					bPvP = true;
					break;
			}
		}
		m_pButtonStage->SetText(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, bPvP ? 120049 : 3006 ));

#ifdef PRE_PARTY_DB
		bool bIsPartyMember = false;
		if (CDnPartyTask::IsActive() && CDnLocalPlayerActor::s_hLocalActor)
		{
			CDnLocalPlayerActor *pLocalActor = static_cast<CDnLocalPlayerActor*>(CDnLocalPlayerActor::s_hLocalActor.GetPointer());
			if (pLocalActor)
				bIsPartyMember = CDnPartyTask::GetInstance().IsPartyMember(pLocalActor->GetUniqueID());
		}

		if( CDnWorld::GetInstance().GetMapType() == CDnWorld::MapTypeVillage && bIsPartyMember == false )
#else
		if( CDnWorld::GetInstance().GetMapType() == CDnWorld::MapTypeVillage &&
			CDnPartyTask::IsActive() && GetPartyTask().GetPartyRole() != CDnPartyTask::MEMBER )
#endif
		{
			m_pButtonChannel->Enable( true );
		}
		else
		{
			m_pButtonChannel->Enable( false );
		}
	}
}

void CDnSystemDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl("ID_BUTTON_CHANNEL" ) )
		{
			m_SystemDlgGroup.ShowDialog( CHANNEL_DIALOG, true );
			m_bOpenChannel = false;
		}
		
		if( IsCmdControl("ID_BUTTON_CONTROL" ) )
		{
			GetInterface().ShowHelpKeyboardDialog( true );
		}

		if( IsCmdControl("ID_BUTTON_HELP" ) )
		{
#ifdef PRE_ADD_NO_HELP_DLG
#else
			m_SystemDlgGroup.ShowDialog( HELP_DIALOG, true );
#endif
		}
		else if( IsCmdControl("ID_BUTTON_GAME" ) )
		{
			m_pGameOptDlg->ImportSetting();
			m_SystemDlgGroup.ShowDialog( GAME_OPTION_DIALOG, true );
		}
		else if( IsCmdControl("ID_BUTTON_SYSTEM" ) )
		{
			m_pSystemOptDlg->ImportSetting();
			m_SystemDlgGroup.ShowDialog( SYSTEM_OPTION_DIALOG, true );
		}
#ifdef _KRAZ
		else if( IsCmdControl("ID_BUTTON_GRB" ) )
		{
			m_SystemDlgGroup.ShowDialog( GAME_GRAGE_DIALOG, true );
		}
#endif
		else if( IsCmdControl("ID_BUTTON_STAGE" ) )
		{
			int nType = 0;
			CDnGameTask *pGameTask = (CDnGameTask *)CTaskManager::GetInstance().GetTask( "GameTask" );
			if( pGameTask ) nType = pGameTask->GetGameTaskType();

			switch( nType ) {
				case GameTaskType::Normal:
				case GameTaskType::DarkLair:
					{
						CDnPartyTask* pPartyTask  = static_cast<CDnPartyTask*>(CTaskManager::GetInstancePtr()->GetTask( "PartyTask" ));
						CDnWorld::MapTypeEnum curMapType = CDnWorld::GetInstance().GetMapType();
						bool bGeneralMsgBox = false;
						if (curMapType == CDnWorld::MapTypeWorldMap)
						{
							bGeneralMsgBox = true;
						}
						else if (curMapType == CDnWorld::MapTypeDungeon)
						{
							if (pPartyTask && pPartyTask->GetPartyState() == CDnPartyTask::STAGE_CLEAR_WARP_STANDBY)
							{
								bGeneralMsgBox = true;
							}
							else
							{
								GetInterface().MessageBox( MakeDurabilityWarningStr(100037).c_str(), MB_YESNO, MESSAGEBOX_STAGE, this, false, true );
							}
						}

						if (bGeneralMsgBox)
						{
							int uiStringNum = 100057;		// UISTRING : 파티에서 탈퇴하고 마을로 이동하시겠습니까?

							if (pPartyTask && pPartyTask->GetPartyCountExcepGM() <= 1)
								uiStringNum = 100063;		// UISTRING : 마을로 이동하시겠습니까?
							GetInterface().MessageBox(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, uiStringNum), MB_YESNO, MESSAGEBOX_STAGE, this);
						}
					}
					break;
				case GameTaskType::Farm:
					GetInterface().MessageBox(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 100063), MB_YESNO, MESSAGEBOX_STAGE, this);
					break;
				case GameTaskType::PvP:
					// UISTRING : 중도 퇴장시 경험치를 30%만 얻을 수 있습니다.

					if(CDnActor::s_hLocalActor && CDnActor::s_hLocalActor->GetTeam() == PvPCommon::Team::Observer)
					{
						GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 120072  ), MB_YESNO, MESSAGEBOX_STAGE, this );
						break;
					}

#ifdef PRE_ADD_PVP_COMBOEXERCISE
					if( IsPvPComboExerciseMode() )
					{
						GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 120111  ), MB_YESNO, MESSAGEBOX_STAGE, this ); // "콜로세움으로 돌아가시겠습니까?"
						break;
					}
#endif // PRE_ADD_PVP_COMBOEXERCISE
				

					GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 120137 ), MB_YESNO, MESSAGEBOX_STAGE, this, false, true );

					break;
			}
		}
		else if( IsCmdControl("ID_BUTTON_CHAR" ) )
		{
			int nType = 0;
			CDnGameTask *pGameTask = (CDnGameTask *)CTaskManager::GetInstance().GetTask( "GameTask" );
			if( pGameTask ) nType = pGameTask->GetGameTaskType();

			switch( nType ) {
				case GameTaskType::Normal:
				case GameTaskType::Farm:
				case GameTaskType::DarkLair:
#ifdef PRE_ADD_GAMEQUIT_REWARD
					{
						CTask * _pTask = CTaskManager::GetInstance().GetTask( "CommonTask" );
						if( _pTask )
						{
							m_nGameQuitCharSelectStrIdx = 100038;							
							static_cast<CDnCommonTask *>(_pTask)->SendLogOutMessage();
						}
					}
#else
					// UISTRING : 캐릭터 선택으로 이동하시겠습니까?
					GetInterface().MessageBox( MakeDurabilityWarningStr(100038).c_str(), MB_YESNO, MESSAGEBOX_CHAR, this );
#endif // PRE_ADD_GAMEQUIT_REWARD
					break;
				case GameTaskType::PvP:
					{
						int nUIStringID = 120137; // "중도 퇴장시 콜로세움 경험치를 받으실 수 없습니다."
						if(CDnActor::s_hLocalActor && CDnActor::s_hLocalActor->GetTeam() == PvPCommon::Team::Observer
#ifdef PRE_ADD_PVP_COMBOEXERCISE
							|| IsPvPComboExerciseMode()
#endif // PRE_ADD_PVP_COMBOEXERCISE
							) 
						{								
							nUIStringID = 100038; // "캐릭터 선택으로 이동하시겠습니까?"						
						}
#ifdef PRE_ADD_GAMEQUIT_REWARD
						CTask * _pTask = CTaskManager::GetInstance().GetTask( "CommonTask" );
						if( _pTask )
						{
							m_nGameQuitCharSelectStrIdx = nUIStringID;							
							static_cast<CDnCommonTask *>(_pTask)->SendLogOutMessage();
						}
#else
						GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nUIStringID ), MB_YESNO, MESSAGEBOX_CHAR, this );							
#endif // PRE_ADD_GAMEQUIT_REWARD						
					}
					break;
			}
		}
		else if( IsCmdControl("ID_BUTTON_EXIT" ) )
		{
#ifdef PRE_ADD_GAMEQUIT_REWARD
			OnExitButton( false );
#else
			OnExitButton();
#endif // PRE_ADD_GAMEQUIT_REWARD
		}
#ifdef PRE_ADD_VIP
		else if (IsCmdControl("ID_BUTTON_VIP"))
		{
			const CDnVIPDataMgr* pMgr = GetInterface().GetLocalPlayerVIPDataMgr();
			if (pMgr && pMgr->IsVIPMode())
			{
				const CDnVIPDataMgr& mgr = *pMgr;
				const __time64_t* pTime = mgr.GetVIPExpireDate();
				if (pTime != NULL && m_pVIPDlg)
				{
					std::wstring contents;
					bool bNear = mgr.IsVIPNearExpireDate();
					mgr.MakeServiceString(contents);
					m_pVIPDlg->SetInfo(mgr.IsVIPAutoPay(), *pTime, bNear ? textcolor::RED : textcolor::WHITE, contents);
					m_SystemDlgGroup.ShowDialog(VIP_DIALOG, true);
				}
			}
		}
#endif // PRE_ADD_VIP
		else if( IsCmdControl("ID_BUTTON_ESCAPE" ) )
		{
#ifdef PRE_FIX_ESCAPE
			GetInterface().ShowEscapeDlg(true);
#else
			if( !CDnActor::s_hLocalActor ) return;
			CDnLocalPlayerActor *pPlayer = dynamic_cast<CDnLocalPlayerActor *>(CDnActor::s_hLocalActor.GetPointer());
			if( !pPlayer ) return;
			pPlayer->CmdEscape();
#endif
		}
		else if( IsCmdControl("ID_BUTTON_KEYSET" ) )
		{
			m_SystemDlgGroup.ShowDialog( GAME_KEY_DIALOG, true );
		}
	}

	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}

void CDnSystemDlg::OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg )
{
	SetCmdControlName( pControl->GetControlName() );

	GetInterface().CloseEventToday();

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl( "ID_YES" ) ) 
		{
			switch( nID )
			{
			case MESSAGEBOX_STAGE:
				{
					if( CDnLocalPlayerActor::IsLockInput() ) break;

					GetInterface().TerminateStageClearWarpStandBy();

					// Note : 마을로 이동
					CDnGameTask* pGameTask = (CDnGameTask *) CTaskManager::GetInstance().GetTask( "GameTask" );
					if( pGameTask ) {
						switch( pGameTask->GetGameTaskType() ) {
							case GameTaskType::Normal:
							case GameTaskType::DarkLair:
							case GameTaskType::Farm:
								{
#ifdef PRE_FIX_GAMEQUIT_TO_VILLAGE
									SendAbandonStage(false, false);
#else
									SendAbandonStage(false);
#endif
								}
								break;
							case GameTaskType::PvP:
								SendMovePvPGameToPvPLobby();
								break;
						}
					}
				}
				break;
			case MESSAGEBOX_CHAR:
				{
					if( CDnLocalPlayerActor::IsLockInput() ) break;

#ifdef PRE_ADD_GAMEQUIT_REWARD
					m_nGameQuitCharSelectStrIdx = 0;
#endif // PRE_ADD_GAMEQUIT_REWARD

					// Note : 캐릭터 선택창으로 이동
					//
					CDnBridgeTask *pBridgeTask = (CDnBridgeTask *)CTaskManager::GetInstance().GetTask( "BridgeTask" );
					if (pBridgeTask)
					{
						GetInterface().TerminateStageClearWarpStandBy();
						if( CTaskManager::GetInstance().GetTask( "VillageTask" ) ) 
							pBridgeTask->SetBridgeState( eBridgeState::VillageToLogin );
						else if( CTaskManager::GetInstance().GetTask( "GameTask" ) ) 
							pBridgeTask->SetBridgeState( eBridgeState::VillageToLogin );
						else ASSERT(0&&"이럴수 없지않나?");
					}

					g_pServiceSetup->WriteStageLog_(IServiceSetup::ExitChannel , _T("ExitChannel"))	;

					CDnLocalPlayerActor::LockInput( true );
					GetInterface().FadeDialog( 0x00000000, 0xff000000, CGlobalInfo::GetInstance().m_fFadeDelta, pBridgeTask );
					CEtSoundEngine::GetInstance().FadeVolume( NULL, 0.f, CGlobalInfo::GetInstance().m_fFadeDelta, false );
				}
			    break;
			case MESSAGEBOX_EXIT:
				{
					/*
					CDnMainMenuDlg *pMainMenuDlg = (CDnMainMenuDlg*)GetParentDialog();
					if( pMainMenuDlg ) pMainMenuDlg->ShowSystemDialog( false );

					CTaskManager::GetInstance().RemoveAllTask();
					*/

#ifdef PRE_ADD_GAMEQUIT_REWARD
					m_nGameQuitCharSelectStrIdx = 0;
#endif // PRE_ADD_GAMEQUIT_REWARD

					if (CDnActor::s_hLocalActor && CDnPartyTask::IsActive())
					{
						if (CDnPartyTask::GetInstance().IsPartyMember(CDnActor::s_hLocalActor->GetUniqueID()))
						{
							CDnGameTask* pGameTask = (CDnGameTask*)(CTaskManager::GetInstance().GetTask( "GameTask" ));
							if (pGameTask && pGameTask->GetGameTaskType() == GameTaskType::Normal)
							{
#ifdef PRE_FIX_GAMEQUIT_TO_VILLAGE
								CDnPartyTask::GetInstance().ReqOutParty(true);
#else
								CDnPartyTask::GetInstance().ReqOutParty();
#endif
							}
						}
					}
					// Note : 아래 부분을 추가해서 종료시 페이드 다이얼로그에서 콜백을 넣어주고
					//		태스크가 종료되어서 그런지 덤프발생. 나중에 수정후 추가
					//
					CDnBridgeTask *pBridgeTask = (CDnBridgeTask *)CTaskManager::GetInstance().GetTask( "BridgeTask" );

					GetInterface().TerminateStageClearWarpStandBy();
				
					g_pServiceSetup->WriteStageLog_(IServiceSetup::ExitChannel , _T("ExitChannel"))	;

					pBridgeTask->SetBridgeState( eBridgeState::Exit );
					GetInterface().FadeDialog( 0x00000000, 0xff000000, CGlobalInfo::GetInstance().m_fFadeDelta, pBridgeTask );
					CEtSoundEngine::GetInstance().FadeVolume( NULL, 0.f, CGlobalInfo::GetInstance().m_fFadeDelta, false );

					g_bEndingBanner = true;
				}
				break;
			}

			return;
		}

		if( IsCmdControl("ID_BTN_AUTH_PW"))
		{
			if( nID == MESSAGEBOX_EXIT )
			{
				GetInterface().OpenSecurityCreateDlg();
			}
		}


		if( IsCmdControl("ID_OK" ) )
		{
			if( nID == CHANNEL_DIALOG )
			{
				m_SystemDlgGroup.ShowDialog(CHANNEL_DIALOG, false);
			}
			return;
		}

		if( IsCmdControl( "ID_BACK" ) )
		{
			m_SystemDlgGroup.ShowDialog(CHANNEL_DIALOG, false);

			if( !m_bOpenChannel )
			{
				CDnMainMenuDlg *pMainMenuDlg = (CDnMainMenuDlg*)GetParentDialog();
				if( pMainMenuDlg ) pMainMenuDlg->ShowSystemDialog( true );
			}
			return;
		}

		if( IsCmdControl( "ID_CLOSE_DIALOG" ) )
		{
			m_SystemDlgGroup.ShowDialog(nID, false);

			CDnMainMenuDlg *pMainMenuDlg = (CDnMainMenuDlg*)GetParentDialog();
			if( pMainMenuDlg ) pMainMenuDlg->ShowSystemDialog( true );
			return;
		}
	}
	else if( nCommand == EVENT_LISTBOX_ITEM_DBLCLK )
	{
		if( IsCmdControl("ID_LISTBOX_CHANNEL" ) )
		{
			m_SystemDlgGroup.ShowDialog(CHANNEL_DIALOG, false);
		}
	}
}

void CDnSystemDlg::AddChannelList( sChannelInfo *pChannelInfo )
{
	ASSERT( pChannelInfo&&"CDnSystemDlg::AddChannelList, pChannelInfo is NULL");

	if( !m_pChannelListDlg ) return;
	m_pChannelListDlg->AddChannelList( pChannelInfo );
}

void CDnSystemDlg::SetDefaultList()
{
	if( !m_pChannelListDlg ) return;
	m_pChannelListDlg->SetDefaultList();
}

void CDnSystemDlg::OpenChannelDlg()
{
	m_SystemDlgGroup.ShowDialog(CHANNEL_DIALOG, true);
	m_bOpenChannel = true;

	CDnInterface::GetInstance().CloseWorldMsgDialog();
#ifdef PRE_ADD_SECONDARY_SKILL
	CDnInterface::GetInstance().CloseCookingDialog();
#endif // PRE_ADD_SECONDARY_SKILL
}

void CDnSystemDlg::ToggleHelpDlg()
{
#ifdef PRE_ADD_NO_HELP_DLG
#else
	bool bShow = m_SystemDlgGroup.IsShowDialog( HELP_DIALOG );
	
	m_SystemDlgGroup.ShowDialog(HELP_DIALOG, !bShow);
#endif
}

void CDnSystemDlg::ShowHelpDlg(int nIndex)
{
#ifdef PRE_ADD_NO_HELP_DLG
	return;
#endif
	if (m_pHelpDlg)
		m_pHelpDlg->ShowCategory(nIndex);

	m_SystemDlgGroup.ShowDialog(HELP_DIALOG, true);
}

#ifdef PRE_ADD_SHORTCUT_HELP_DIALOG
void CDnSystemDlg::ShowChoiceHelpDlg(eShortCutHelpDlgType eDlgType)
{
#ifdef PRE_ADD_NO_HELP_DLG
	return;
#endif
	
	DNTableFileFormat* pHelpShortCut = GetDNTable( CDnTableDB::THELPSHORTCUT );
	if( pHelpShortCut == NULL )
		return;

	int nCount = pHelpShortCut->GetItemCount();
	int nCategoryID = -1;

	for(int i = 0 ; i < nCount ; ++i)
	{
		int nItemIndex = pHelpShortCut->GetItemID(i);
		int nType = pHelpShortCut->GetFieldFromLablePtr(nItemIndex, "_ButtonIndex")->GetInteger();

		if( nType == eDlgType )
		{
			nCategoryID = pHelpShortCut->GetFieldFromLablePtr(nItemIndex, "_CategoryID")->GetInteger();
		}
	}

	if (m_pHelpDlg && nCategoryID != -1)
	{
		m_pHelpDlg->ShowCategory(nCategoryID);
		m_SystemDlgGroup.ShowDialog(HELP_DIALOG, true, false);
	}
}
#endif

tstring CDnSystemDlg::MakeDurabilityWarningStr( int nUIString )
{
	bool bAdd = false;
	int nAddUIStringIndex = 0;
	CDnGameTask *pTask = (CDnGameTask *)CTaskManager::GetInstance().GetTask( "GameTask" );
	if( CDnWorld::IsActive() && CDnWorld::GetInstance().GetMapType() == CDnWorld::MapTypeDungeon && pTask ) {
		switch( pTask->GetGameTaskType() ) {
			case GameTaskType::Normal:
			case GameTaskType::DarkLair:
				if( CDnActor::s_hLocalActor && !CDnActor::s_hLocalActor->IsDie() )
				{
					CDnPartyTask* pPartyTask  = static_cast<CDnPartyTask*>(CTaskManager::GetInstancePtr()->GetTask( "PartyTask" ));
					if (pPartyTask && pPartyTask->GetPartyState() == CDnPartyTask::STAGE_CLEAR_WARP_STANDBY)
						bAdd = false;
					else {
						if( pTask->GetGameTaskType() == GameTaskType::Normal )
						{
							nAddUIStringIndex = 100049;
							DNTableFileFormat* pMapSox = GetDNTable( CDnTableDB::TMAP );
							int nDungeonEnterTableID = pMapSox->GetFieldFromLablePtr( pTask->GetGameMapIndex(), "_EnterConditionTableID" )->GetInteger();
							if( nDungeonEnterTableID > 0 ) 
							{
								if( pMapSox->IsExistItem( nDungeonEnterTableID ) ) 
								{
									DNTableFileFormat *pDungeonSox = GetDNTable( CDnTableDB::TDUNGEONENTER );
									if( pDungeonSox->GetFieldFromLablePtr( nDungeonEnterTableID, "_StageOutDurability" )->GetInteger() == 0)								
										nAddUIStringIndex = 109046;
								}
							}	
						}
						else 
							nAddUIStringIndex = 100061;
						bAdd = true;
					}
				}
				break;
		}
	}

	WCHAR wszStr[256] = { 0, };
	wsprintf( wszStr, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nUIString ) );
	if( bAdd && nAddUIStringIndex ) {
		wcscat( wszStr, L"\n" );
		wcscat( wszStr, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nAddUIStringIndex ) );	// UISTRING : (스테이지 포기시 내구도가 하락합니다.)
	}
	return tstring(wszStr);
}

#ifdef PRE_ADD_GAMEQUIT_REWARD
void CDnSystemDlg::OnExitButton( bool bForce )
#else
void CDnSystemDlg::OnExitButton()
#endif // PRE_ADD_GAMEQUIT_REWARD
{
	int nType = 0;
	CDnGameTask *pGameTask = (CDnGameTask *)CTaskManager::GetInstance().GetTask( "GameTask" );
	if( pGameTask ) nType = pGameTask->GetGameTaskType();
	UINT MsgType = MB_YESNO; 

#ifdef PRE_ADD_GAMEQUIT_REWARD
	
#ifdef PRE_ADD_DWC
	if( bForce == false && GetDWCTask().IsDWCChar() == false)
#else
	if( bForce == false )
#endif
	{
		CTask * _pTask = CTaskManager::GetInstance().GetTask( "CommonTask" );
		if( _pTask )
		{
			m_nGameQuitCharSelectStrIdx = 0;
			static_cast<CDnCommonTask *>(_pTask)->SendLogOutMessage();
			return;
		}
	}
#endif // PRE_ADD_GAMEQUIT_REWARD

	//MB_CANCELTRYCONTINUE 2차 비밀번호 생성 버튼이 추가됨에 따라 형식을 맞추기위해 . 현재쓰이지 않는 MB_CANCELTRYCONTINUE 으로 사용하고있습니다. 
	//시스템 종료버튼에만 추가되니 .. 여기에있는 메세지는 꼭 MB_CANCELTRYCONTINUE 처리해주세요.
	switch( nType ) {
		case GameTaskType::Normal:
		case GameTaskType::Farm:
			GetInterface().MessageBox( MakeDurabilityWarningStr(100021).c_str(), MsgType, MESSAGEBOX_EXIT, this );
			break;
		case GameTaskType::DarkLair:
			GetInterface().MessageBox( MakeDurabilityWarningStr(100021).c_str(), MsgType, MESSAGEBOX_EXIT, this );
			break;
		case GameTaskType::PvP:
			// UISTRING : 중도 퇴장시 경험치를 30%만 얻을 수 있습니다.
			if(CDnActor::s_hLocalActor && CDnActor::s_hLocalActor->GetTeam() == PvPCommon::Team::Observer
#ifdef PRE_ADD_PVP_COMBOEXERCISE	
				|| IsPvPComboExerciseMode()
#endif // PRE_ADD_PVP_COMBOEXERCISE
				)
			{
				GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 100021  ), MB_YESNO, MESSAGEBOX_EXIT, this );
				break;
			}
			GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 120137 ), MB_YESNO, MESSAGEBOX_EXIT, this );
			break;
	}
}

bool CDnSystemDlg::IsOpenChannelDlg() const
{
	if( !m_pChannelListDlg ) return false;
	return m_pChannelListDlg->IsShow();
}

#ifdef PRE_ADD_PVP_COMBOEXERCISE
bool CDnSystemDlg::IsPvPComboExerciseMode()
{
	CTask * pTask = CTaskManager::GetInstance().GetTask("GameTask");
	if( pTask )
	{
		CDnPvPGameTask * pPvPGameTask = dynamic_cast< CDnPvPGameTask * >( pTask );
		if( pPvPGameTask && (pPvPGameTask->GetGameMode() == PvPCommon::GameMode::PvP_ComboExercise) )
			return true;		
	}

	return false;
}
#endif // PRE_ADD_PVP_COMBOEXERCISE


void CDnSystemDlg::OnCharButton()
{
	int nType = 0;
	CDnGameTask *pGameTask = (CDnGameTask *)CTaskManager::GetInstance().GetTask( "GameTask" );
	if( pGameTask ) nType = pGameTask->GetGameTaskType();

	switch( nType ) {
				case GameTaskType::Normal:
				case GameTaskType::Farm:
				case GameTaskType::DarkLair:
					// UISTRING : 캐릭터 선택으로 이동하시겠습니까?
					GetInterface().MessageBox( MakeDurabilityWarningStr(100038).c_str(), MB_YESNO, MESSAGEBOX_CHAR, this );
					break;
				case GameTaskType::PvP:
					{
						int nUIStringID = 120137; // "중도 퇴장시 콜로세움 경험치를 받으실 수 없습니다."
						if(CDnActor::s_hLocalActor && CDnActor::s_hLocalActor->GetTeam() == PvPCommon::Team::Observer
#ifdef PRE_ADD_PVP_COMBOEXERCISE
							|| IsPvPComboExerciseMode()
#endif // PRE_ADD_PVP_COMBOEXERCISE
							) 
						{								
							nUIStringID = 100038; // "캐릭터 선택으로 이동하시겠습니까?"
							GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nUIStringID ), MB_YESNO, MESSAGEBOX_CHAR, this );							
							break;
						}

						GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nUIStringID ), MB_YESNO, MESSAGEBOX_CHAR, this );
					}
					break;
	}
}

#ifdef PRE_FIX_APPELATION_TOOLTIP
bool CDnSystemDlg::IsShowDlg(DWORD dwDlgType) const
{
	return (m_SystemDlgGroup.IsShowDialog(dwDlgType));
}
#endif
