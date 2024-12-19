#include "StdAfx.h"
#include "DnPVPGameRoomDlg.h"
#include "PVPSendPacket.h"
#include "DnActor.h"
#include "DnInterface.h"
#include "DnBridgeTask.h"
#include "DnTableDB.h"
#include "DnChatTabDlg.h"
#include "DnMainMenuDlg.h"
#include "DnInterfaceString.h"
#include "DnInvenTabDlg.h"
#include "DnPVPLobbyChatTabDlg.h"
#include "DnPVPLobbyVillageTask.h"
#include "TaskManager.h"
#include "DnPVPRevengeMessageBoxDlg.h"
#include "DnRevengeTask.h"
#ifdef PRE_MOD_PVPOBSERVER
#include "DnLocalPlayerActor.h"
#endif // PRE_MOD_PVPOBSERVER


#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#endif


CDnPVPGameRoomDlg::CDnPVPGameRoomDlg( UI_DIALOG_TYPE dialogType, CEtUIDialog *pParentDialog, int nID, CEtUICallback *pCallback )
: CEtUIDialog( dialogType, pParentDialog, nID, pCallback )
,m_pTeamChangeButton(NULL)
,m_pReadyButton(NULL)
,m_pStartButton(NULL)
,m_pCancelButton(NULL)
,m_pRoomName(NULL)
,m_pRoomIndexNum(NULL)
,m_pPasswordRoom(NULL)
,m_pRoomEditButton(NULL)
,m_pPVPMakeRoomDlg(NULL)
,m_pReadyCancelButton(NULL)
,m_pKick(NULL)
,m_pPermanenceKick(NULL)
#if defined( PRE_ADD_REVENGE )
, m_pRevengeButton(NULL)
#endif	// #if defined( PRE_ADD_REVENGE )
,m_pChangeMasterButton(NULL)
,m_pMoveToObserver(NULL)
,m_pMoveToPlay(NULL)
,m_pPVPPopupDlg(NULL)
,m_bIndividualMode(false)
,m_nBeepSound(-1)
,m_nEventRoomIndex(0)
,m_pUIGameMode(NULL)
,m_pUIPlayerNum(NULL)
,m_pMiniMap(NULL)
,m_pMapName(NULL)
,m_pInGameJoin(NULL)
,m_pNotInGameJoin(NULL)
,m_pDropItem(NULL)
,m_pNoDropItem(NULL)
,m_pRevision(NULL)
,m_pNoRevision(NULL)
,m_pShowHp(NULL)
,m_pHideHp(NULL)
,m_pWinCon(NULL)
,m_pChangeGroupCaptain(NULL)
,m_pStaticNameBreakInto(NULL)
,m_pStaticNameUseItem(NULL)
#ifdef PRE_ADD_PVP_HELP_MESSAGE
,m_nRadioButtonIndex(E_PVP_RADIOBUTTON_ROOM_INFO)
#endif
#ifdef PRE_ADD_PVP_COMBOEXERCISE
,m_bComboExeMode(false)
#endif // PRE_ADD_PVP_COMBOEXERCISE
{ 
	Reset();

	for(int i=0;i<3;i++)
		m_pIndividualUIBar[i] =NULL;
	
	for(int i=0;i<8;i++)
		m_pTeamUIBar[i] =NULL;
}

CDnPVPGameRoomDlg::~CDnPVPGameRoomDlg(void)
{
	SAFE_RELEASE_SPTR( m_hMiniMapImage );	
	SAFE_DELETE( m_pPVPMakeRoomDlg );
	SAFE_DELETE(m_pPVPPopupDlg);
	if( m_nBeepSound != -1 )
		CEtSoundEngine::GetInstance().RemoveSound( m_nBeepSound );
}

void CDnPVPGameRoomDlg::Reset()
{
	SecureZeroMemory(m_wszRoomName,sizeof(m_wszRoomName));
	SecureZeroMemory(m_wszOriginalRoomName,sizeof(m_wszOriginalRoomName));

	m_uiMapIndex = 0;					
	m_nGameModeID = 0;					
	m_cMaxPlayerNum = 0;
	m_cMyTeam = 255;
	m_uiMyUserState = 0;
	m_uiRoomState = 0;
	m_cCurrentPlayerNum = 0;
	m_cCurrentTeamNum_A = 0;
	m_cCurrentTeamNum_B = 0;
	m_nMaxLevel = 100;
	m_nMinLevel = 1;

	m_nRoomIndex = 0;
	m_fElapsedKickTime = 0.0f;
	m_nWaringNum =0;

	m_bRandomTeamMode = false;
	m_IsBreakIntoFlag = false; 
	m_bDropItem = false;
	m_bShowHp = false;
	m_bRevision = false;
	m_bRandomOrder = false;
#ifdef PRE_MOD_PVPOBSERVER
	m_bEnterObserver = false;
	m_bAllowObserver = false;
	m_bEventRoom = false;
#endif // PRE_MOD_PVPOBSERVER

#ifdef PRE_ADD_PVP_HELP_MESSAGE
	m_nRadioButtonIndex = E_PVP_RADIOBUTTON_ROOM_INFO;
#endif

#ifdef PRE_ADD_PVP_COMBOEXERCISE
	m_bComboExeMode = false;
#endif // PRE_ADD_PVP_COMBOEXERCISE

	for(int iTeamNum = 0 ; iTeamNum < MAXTeamNUM ; iTeamNum++)
	{
		for(int iPlayerNum = 0 ; iPlayerNum < PvP_TeamUserSlot ; iPlayerNum++)
		{
			m_sPlayerSlot[iTeamNum][iPlayerNum].emSlotState = Closed;
			m_sPlayerSlot[iTeamNum][iPlayerNum].iJobID = 0;
			m_sPlayerSlot[iTeamNum][iPlayerNum].cLevel = 0;
			m_sPlayerSlot[iTeamNum][iPlayerNum].cPVPLevel = 0;
			m_sPlayerSlot[iTeamNum][iPlayerNum].uiUserState = 0;
			m_sPlayerSlot[iTeamNum][iPlayerNum].uiSessionID = 0;
			m_sPlayerSlot[iTeamNum][iPlayerNum].isNeedUpdate = true;			
		}
	}	

	CDnBridgeTask::GetInstance().ResetPVPGameStatus();
}


void CDnPVPGameRoomDlg::RoomSetting( PvPCommon::RoomInfo* pInfo ,bool IsModified)
{
#if defined( PRE_ADD_PVP_COMBOEXERCISE )
	SetComboExerciseMode( false );
#endif // PRE_ADD_PVP_COMBOEXERCISE


	m_uiMapIndex = pInfo->uiMapIndex;			
	m_nGameModeID = pInfo->uiGameModeTableID;		
	
	m_IsBreakIntoFlag = pInfo->unRoomOptionBit & PvPCommon::RoomOption::BreakInto ? true:false;	
	m_bDropItem = pInfo->unRoomOptionBit & PvPCommon::RoomOption::DropItem ? true:false;	
	m_bShowHp = pInfo->unRoomOptionBit & PvPCommon::RoomOption::ShowHP ? true:false;	
	m_bRandomTeamMode =  pInfo->unRoomOptionBit & PvPCommon::RoomOption::RandomTeam ? true:false;	
	m_bRevision = pInfo->unRoomOptionBit & PvPCommon::RoomOption::NoRegulation ? true:false;
	m_bRandomOrder =  pInfo->unRoomOptionBit & PvPCommon::RoomOption::AllKill_RandomOrder ? true:false;;
#ifdef PRE_MOD_PVPOBSERVER
	m_bAllowObserver = pInfo->unRoomOptionBit & PvPCommon::RoomOption::AllowObserver ? true : false;
	m_bEventRoom = pInfo->bExtendObserver;
	m_pMoveToPlay->Enable( !m_bEventRoom );
#endif // PRE_MOD_PVPOBSERVER

	m_pPasswordRoom->Show(false);
	m_pInGameJoin->Show(m_IsBreakIntoFlag);
	m_pNotInGameJoin->Show(!m_IsBreakIntoFlag);
	m_pDropItem->Show(m_bDropItem);
	m_pNoDropItem->Show(!m_bDropItem);
	m_pShowHp->Show( m_bShowHp );
	m_pHideHp->Show( !m_bShowHp);
	m_pRevision->Show(!m_bRevision);
	m_pNoRevision->Show(m_bRevision);

	m_nMaxLevel = (int)pInfo->cMaxLevel;
	m_nMinLevel = (int)pInfo->cMinLevel;

	m_pStaticNameUseItem->Show(true);
	m_pStaticNameBreakInto->Show(true);
	m_pStaticNameBreakInto->SetText(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 120101 )); // 난입

	WCHAR	wszRoomName[PvPCommon::TxtMax::RoomName];
	SecureZeroMemory(wszRoomName,sizeof(wszRoomName));
	memcpy(wszRoomName,pInfo->wszBuffer,pInfo->cRoomNameLen * sizeof(WCHAR));
	_wcscpy(m_wszOriginalRoomName, _countof(m_wszOriginalRoomName), wszRoomName, (int)wcslen(wszRoomName));	  
	swprintf_s(m_wszRoomName,_countof(m_wszRoomName),L"%s",wszRoomName);	
	m_pRoomName->SetText(wszRoomName);

	m_cMaxPlayerNum = pInfo->cMaxUserCount;

	int iTeamplayNum = int(m_cMaxPlayerNum/2);
	iTeamplayNum = min(iTeamplayNum, PvP_TeamUserSlot);

	if( !IsModified )
	{

#ifdef PRE_ADD_PVP_COMBOEXERCISE
		// 콤보연습모드의 경우 1인플레이가능.
		if( m_cMaxPlayerNum == 1 )
		{
			m_sPlayerSlot[0][0].emSlotState = Open;
			m_sPlayerSlot[0][0].isNeedUpdate = true;			
		}
		else
		{
			for(int iTeamNum = 0 ; iTeamNum < MAXTeamNUM ; iTeamNum++)
			{
				for(int iPlayerNum = 0 ; iPlayerNum < iTeamplayNum ; iPlayerNum++)
				{
					m_sPlayerSlot[iTeamNum][iPlayerNum].emSlotState = Open;
					m_sPlayerSlot[iTeamNum][iPlayerNum].isNeedUpdate = true;
				}
			}			
		}
		m_nRoomIndex = pInfo->uiIndex;
#else

		for(int iTeamNum = 0 ; iTeamNum < MAXTeamNUM ; iTeamNum++)
		{
			for(int iPlayerNum = 0 ; iPlayerNum < iTeamplayNum ; iPlayerNum++)
			{
				m_sPlayerSlot[iTeamNum][iPlayerNum].emSlotState = Open;
				m_sPlayerSlot[iTeamNum][iPlayerNum].isNeedUpdate = true;
			}
		}

		m_nRoomIndex = pInfo->uiIndex;
#endif // PRE_ADD_PVP_COMBOEXERCISE

	}
	
	WCHAR wszIndex[256];
	wsprintf(wszIndex,L"%d. ", m_nRoomIndex);
	m_pRoomIndexNum->SetText(wszIndex);	

	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TPVPGAMEMODE );
	if ( !pSox ) 
	{
		ErrorLog("CDnPVPGameRoomDlg::RoomSetting:: gamemode table Not found!! ");
		return;
	}

	
	int nGameModeUIString = 0;
	nGameModeUIString = pSox->GetFieldFromLablePtr( m_nGameModeID , "GameModeUIString" )->GetInteger();

	const wchar_t * wszModeName = NULL; 
	wszModeName = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nGameModeUIString );
	if( wszModeName )
		m_pUIGameMode->SetText(wszModeName);

	int nGameModeIndex = 0;
	nGameModeIndex = pSox->GetFieldFromLablePtr( m_nGameModeID , "GamemodeID" )->GetInteger();

	WCHAR wszWinCon[256];
	SecureZeroMemory(wszWinCon,sizeof(wszWinCon));
	wsprintf(wszWinCon,L"%d%s",pInfo->uiWinCondition,GetEtUIXML().GetUIString( CEtUIXML::idCategory1, pSox->GetFieldFromLablePtr( m_nGameModeID , "WinCondition_UIString" )->GetInteger()));
	m_pWinCon->SetText(wszWinCon);

	switch(nGameModeIndex)
	{
	case PvPCommon::GameMode::PvP_IndividualRespawn:
	case PvPCommon::GameMode::PvP_Zombie_Survival:
#if defined( PRE_ADD_RACING_MODE )
	case PvPCommon::GameMode::PvP_Racing:
#endif	// #if defined( PRE_ADD_RACING_MODE )
		{
			SetIndividualMode(true);
		}
		break;
	case PvPCommon::GameMode::PvP_AllKill:
		{
			m_pInGameJoin->Show(m_bRandomOrder);
			m_pNotInGameJoin->Show(!m_bRandomOrder);
			m_pDropItem->Show(false);
			m_pNoDropItem->Show(false);
			m_pStaticNameUseItem->Show(false);
			m_pStaticNameBreakInto->SetText(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 121124 )); // 랜덤순서
			
			SetIndividualMode(m_bRandomTeamMode);

			wsprintf(wszWinCon,L"%s", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, pSox->GetFieldFromLablePtr( m_nGameModeID , "WinCondition_UIString" )->GetInteger()));
			m_pWinCon->SetText(wszWinCon);

		}
		break;
#if defined( PRE_ADD_PVP_COMBOEXERCISE )
	case PvPCommon::GameMode::PvP_ComboExercise:
		{
			SetIndividualMode( true );
			SetComboExerciseMode( true );
			wsprintf(wszWinCon,L"%s", GetEtUIXML().GetUIString( CEtUIXML::idCategory1, pSox->GetFieldFromLablePtr( m_nGameModeID , "WinCondition_UIString" )->GetInteger()));
			m_pWinCon->SetText(wszWinCon);
		}
		break;
#endif // PRE_ADD_PVP_COMBOEXERCISE

	default:
		{
			SetIndividualMode(m_bRandomTeamMode);
		}
		break;
	}

	DNTableFileFormat* pMapSox = GetDNTable( CDnTableDB::TPVPMAP );
	if ( !pMapSox ) 
	{
		ErrorLog("CDnPVPGameRoomDlg::RoomSetting:: PVP Map table Not found!! ");
		return;
	}

	int nMapNameUIString = 0;
	nMapNameUIString = pMapSox->GetFieldFromLablePtr( m_uiMapIndex , "MapNameUIstring" )->GetInteger();

	const wchar_t * wszMapName = NULL; 
	wszMapName = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nMapNameUIString );

	if( wszMapName )
		m_pMapName->SetText(wszMapName);


	SAFE_RELEASE_SPTR( m_hMiniMapImage );
	std::string szMiniTextureName = pMapSox->GetFieldFromLablePtr( m_uiMapIndex , "MapImage" )->GetString();
	if( szMiniTextureName.c_str())
		m_hMiniMapImage = LoadResource( CEtResourceMng::GetInstance().GetFullName(szMiniTextureName.c_str()).c_str(), RT_TEXTURE );
	if( m_hMiniMapImage )
		m_pMiniMap->SetTexture( m_hMiniMapImage, 0, 0, m_hMiniMapImage->Width() , m_hMiniMapImage->Height() );

	

	SetEventRoom(pInfo->nEventID);

	UpdateSlot( false );

	std::wstring wszFullRoomName = wszIndex;
	wszFullRoomName.append( wszRoomName , wcslen(wszRoomName));

	SetRoomState(pInfo->cRoomState);
	CDnBridgeTask::GetInstance().SetPVPGameStatus(wszFullRoomName, pInfo->uiWinCondition, m_nGameModeID, pInfo->uiPlayTimeSec, m_cMaxPlayerNum , m_bShowHp, m_bRevision );

#ifdef PRE_ADD_PVP_HELP_MESSAGE
	if(IsModified)
	{
		ChangeRoomInformation(E_PVP_RADIOBUTTON_ROOM_INFO);
		if(m_pRoomInfoButton[m_nRadioButtonIndex])
			m_pRoomInfoButton[m_nRadioButtonIndex]->SetChecked(true);
	}
#endif
}


void CDnPVPGameRoomDlg::InsertPlayer(int team , UINT uiUserState , UINT    uiSessionID , int iJobID , BYTE  cLevel , BYTE  cPVPLevel , const WCHAR * szPlayerName, char cPosition )
{
	int nPlayerTeamIndex = -1;

	switch(team) // 
	{
	case (PvPCommon::Team::A):
		nPlayerTeamIndex = 0;
		break;
	case (PvPCommon::Team::B):
		nPlayerTeamIndex = 1;
		break;
	case (PvPCommon::Team::Observer):
		break;
	}

	if( nPlayerTeamIndex >= MAXTeamNUM || nPlayerTeamIndex < 0 )
	{
		ErrorLog("CDnPVPGameRoomDlg::InsertPlayer:: Not insert player because invalid teamIndex.");
		return;
	}
	
	if( CDnBridgeTask::GetInstance().GetSessionID() ==  uiSessionID )//자기 자신일때 
		m_cMyTeam = nPlayerTeamIndex;		

	for(int iPlayerNum = 0 ; iPlayerNum < PvP_TeamUserSlot ; iPlayerNum++)
	{
		if ( m_sPlayerSlot[nPlayerTeamIndex][iPlayerNum].emSlotState == Open )
		{
			m_sPlayerSlot[nPlayerTeamIndex][iPlayerNum].iJobID = iJobID;
			m_sPlayerSlot[nPlayerTeamIndex][iPlayerNum].cLevel = cLevel;
			m_sPlayerSlot[nPlayerTeamIndex][iPlayerNum].uiUserState = uiUserState;
			m_sPlayerSlot[nPlayerTeamIndex][iPlayerNum].cPVPLevel = cPVPLevel;
			swprintf_s(m_sPlayerSlot[nPlayerTeamIndex][iPlayerNum].wszPlayerName ,_countof(m_sPlayerSlot[nPlayerTeamIndex][iPlayerNum].wszPlayerName),szPlayerName);
			m_sPlayerSlot[nPlayerTeamIndex][iPlayerNum].uiSessionID = uiSessionID;
			m_sPlayerSlot[nPlayerTeamIndex][iPlayerNum].emSlotState = InPlayer;			
			m_sPlayerSlot[nPlayerTeamIndex][iPlayerNum].isNeedUpdate = true;
			SetUserState( uiSessionID , uiUserState );
			m_cCurrentPlayerNum++;

			if(nPlayerTeamIndex == 0)
			{
				m_cCurrentTeamNum_A++;
			}
			else if(nPlayerTeamIndex == 1)
			{
				m_cCurrentTeamNum_B++;
			}

			UpdateSlot( false );
			return;
		}
	}
	ErrorLog("CDnPVPGameRoomDlg::InsertPlayer:: Not insert player because no empty slot.");

}


#ifdef PRE_MOD_PVPOBSERVER
void CDnPVPGameRoomDlg::InsertObserver( UINT uiUserState , UINT uiSessionID )
{
	if( CDnBridgeTask::GetInstance().GetSessionID() ==  uiSessionID )
		SetUserState( uiSessionID , uiUserState );
}
#endif // PRE_MOD_PVPOBSERVER


void CDnPVPGameRoomDlg::CopySlot( int iSourceTeamIndex , int iSourcePlayerIndex , int iDestTeamIndex , int iDestPlayerIndex )
{
	if( iSourcePlayerIndex >= PvP_TeamUserSlot || 
		iDestPlayerIndex >= PvP_TeamUserSlot ||
		iSourceTeamIndex >= MAXTeamNUM ||
		iDestTeamIndex >= MAXTeamNUM
		)
		return;

	m_sPlayerSlot[iDestTeamIndex][iDestPlayerIndex].emSlotState = m_sPlayerSlot[iSourceTeamIndex][iSourcePlayerIndex].emSlotState;
	m_sPlayerSlot[iDestTeamIndex][iDestPlayerIndex].iJobID = m_sPlayerSlot[iSourceTeamIndex][iSourcePlayerIndex].iJobID;
	m_sPlayerSlot[iDestTeamIndex][iDestPlayerIndex].cLevel = m_sPlayerSlot[iSourceTeamIndex][iSourcePlayerIndex].cLevel;
	m_sPlayerSlot[iDestTeamIndex][iDestPlayerIndex].cPVPLevel = m_sPlayerSlot[iSourceTeamIndex][iSourcePlayerIndex].cPVPLevel;
	m_sPlayerSlot[iDestTeamIndex][iDestPlayerIndex].uiUserState = m_sPlayerSlot[iSourceTeamIndex][iSourcePlayerIndex].uiUserState;
	m_sPlayerSlot[iDestTeamIndex][iDestPlayerIndex].isNeedUpdate = m_sPlayerSlot[iSourceTeamIndex][iSourcePlayerIndex].isNeedUpdate = true;
	m_sPlayerSlot[iDestTeamIndex][iDestPlayerIndex].uiSessionID = m_sPlayerSlot[iSourceTeamIndex][iSourcePlayerIndex].uiSessionID;
	_wcscpy( m_sPlayerSlot[iDestTeamIndex][iDestPlayerIndex].wszPlayerName , _countof(m_sPlayerSlot[iDestTeamIndex][iDestPlayerIndex].wszPlayerName),
		m_sPlayerSlot[iSourceTeamIndex][iSourcePlayerIndex].wszPlayerName, (int)wcslen(m_sPlayerSlot[iSourceTeamIndex][iSourcePlayerIndex].wszPlayerName));
	UpdateSlot( false );

}

void CDnPVPGameRoomDlg::RemoveSlot( int iTeamIndex , int iPlayerIndex , SlotState emChanheState )
{
	if( iPlayerIndex >= PvP_TeamUserSlot ||	iPlayerIndex < 0 || iTeamIndex >= MAXTeamNUM || iTeamIndex < 0)
		return;

	if( m_sPlayerSlot[iTeamIndex][iPlayerIndex].emSlotState == Closed )
	{
		DebugLog("Dont remove this slot because it is closed.");
		return;
	}

	ClearSlot( iTeamIndex , iPlayerIndex , emChanheState );

	int nNextSlotIndex = iPlayerIndex + 1;
	if( nNextSlotIndex >= 0 && nNextSlotIndex < PvP_TeamUserSlot )
	{
		for(int nMoveSlotIndex = nNextSlotIndex; nMoveSlotIndex < PvP_TeamUserSlot ; nMoveSlotIndex++)
		{
			if( m_sPlayerSlot[iTeamIndex][nMoveSlotIndex].emSlotState != Closed ) //클로즈드가 아니면 위로 땡긴다
			{
				CopySlot( iTeamIndex , nMoveSlotIndex , iTeamIndex , nMoveSlotIndex - 1 );
				ClearSlot( iTeamIndex , nMoveSlotIndex , SlotState::Open );
			}
		}
	}

	UpdateSlot( false );
}

void CDnPVPGameRoomDlg::ClearSlot( int iTeamIndex , int iPlayerIndex , SlotState emChanheState )
{
	m_sPlayerSlot[iTeamIndex][iPlayerIndex].iJobID = 0;
	m_sPlayerSlot[iTeamIndex][iPlayerIndex].cLevel = 0;
	m_sPlayerSlot[iTeamIndex][iPlayerIndex].cPVPLevel = 0;
	m_sPlayerSlot[iTeamIndex][iPlayerIndex].uiSessionID = 0;
	m_sPlayerSlot[iTeamIndex][iPlayerIndex].uiUserState = 0;
	m_sPlayerSlot[iTeamIndex][iPlayerIndex].emSlotState = emChanheState;				
	m_sPlayerSlot[iTeamIndex][iPlayerIndex].isNeedUpdate = true;
	SecureZeroMemory(m_sPlayerSlot[iTeamIndex][iPlayerIndex].wszPlayerName,sizeof(m_sPlayerSlot[iTeamIndex][iPlayerIndex].wszPlayerName));

	m_sPlayerSlot[iTeamIndex][iPlayerIndex].pLevelUI->Show(false);
	m_sPlayerSlot[iTeamIndex][iPlayerIndex].pClassUI->Show(false);
	m_sPlayerSlot[iTeamIndex][iPlayerIndex].pNameUI->Show(false);
	m_sPlayerSlot[iTeamIndex][iPlayerIndex].pOpen->Show(false);
	m_sPlayerSlot[iTeamIndex][iPlayerIndex].pOpenText->Show(false);
	m_sPlayerSlot[iTeamIndex][iPlayerIndex].pClose->Show(false);
	m_sPlayerSlot[iTeamIndex][iPlayerIndex].pCloseText->Show(false);
	m_sPlayerSlot[iTeamIndex][iPlayerIndex].pPVPIcon->Show(false);
}



void CDnPVPGameRoomDlg::RemovePlayer( UINT uiSessionID, PvPCommon::LeaveType::eLeaveType eType )
{
	WCHAR wszMessage[256] = {0,};

	for(int iTeamNum = 0 ; iTeamNum < MAXTeamNUM ; iTeamNum++)
	{
		for(int iPlayerNum = 0 ; iPlayerNum < PvP_TeamUserSlot ; iPlayerNum++)
		{
			if ( m_sPlayerSlot[iTeamNum][iPlayerNum].uiSessionID ==  uiSessionID)
			{
				int nUIString = 0;
				if( eType == PvPCommon::LeaveType::Ban ) nUIString = 121061;
				else if( eType == PvPCommon::LeaveType::PermanenceBan ) nUIString = 121062;
				if( nUIString ) {
					wsprintf( wszMessage, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nUIString ), m_sPlayerSlot[iTeamNum][iPlayerNum].wszPlayerName );
					GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", wszMessage, false );
				}

				RemoveSlot( iTeamNum , iPlayerNum , SlotState::Open );
				m_cCurrentPlayerNum--;

				if(iTeamNum == 0)
				{
					m_cCurrentTeamNum_A--;
				}
				else if(iTeamNum ==1)
				{
					m_cCurrentTeamNum_B--;
				}
				return;
			}
		}
	}

	ErrorLog("CDnPVPGameRoomDlg::RemovePlayer:: SessionID Not Found %d",uiSessionID);
}

void CDnPVPGameRoomDlg::SetUserState( UINT uiSessionID , UINT  uiUserState , bool IsOuterCall)
{
	sUserSlot * pSlot = NULL;
	bool IsMy =  false;
	bool IsChangeMaster =  false;
	WCHAR * wszMasterName = NULL;


	pSlot = (sUserSlot *)FindUserSlot( uiSessionID );

	if( pSlot )
	{
		if( !IsMaster(pSlot->uiUserState) && IsMaster(uiUserState) )
		{
			IsChangeMaster= true;
			wszMasterName = pSlot->wszPlayerName;
			if( CDnBridgeTask::GetInstance().GetSessionID() ==  uiSessionID )//자기 자신일때
				IsMy = true;
		}
		pSlot->uiUserState =  uiUserState;
		pSlot->isNeedUpdate =  true;
	}

	if( CDnBridgeTask::GetInstance().GetSessionID() ==  uiSessionID )//자기 자신일때
	{
		m_uiMyUserState = uiUserState;		
	}

	if( IsOuterCall )
	{
		//님이 방장 됐음.
		if( IsMy && IsChangeMaster)
		{
			GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 120132 ) , MB_OK );
		}
		else if( IsChangeMaster )
		{
			WCHAR wszMessage[256];
			SecureZeroMemory(wszMessage,sizeof(wszMessage));
			wsprintf(wszMessage,GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 120131 ), wszMasterName);

			GetInterface().AddChatMessage(CHATTYPE_SYSTEM, L"",wszMessage);
		}
	}

	UpdateSlot( false );
}

void *  CDnPVPGameRoomDlg::FindUserSlot( UINT uiSessionID )
{
	for(int iTeamNum = 0 ; iTeamNum < MAXTeamNUM ; iTeamNum++)
	{
		for(int iPlayerNum = 0 ; iPlayerNum < PvP_TeamUserSlot ; iPlayerNum++)
		{
			if ( m_sPlayerSlot[iTeamNum][iPlayerNum].uiSessionID ==  uiSessionID)
			{
				return &(m_sPlayerSlot[iTeamNum][iPlayerNum]); 
			}
		}
	}

	ErrorLog("CDnPVPGameRoomDlg::FindUserSlot:: User Not Found%d",uiSessionID);

	return NULL;
}

bool  CDnPVPGameRoomDlg::FindUserSlot( UINT uiSessionID , int & pSotIndex , int & pPlayerIndex )
{
	for(int iTeamNum = 0 ; iTeamNum < MAXTeamNUM ; iTeamNum++)
	{
		for(int iPlayerNum = 0 ; iPlayerNum < PvP_TeamUserSlot ; iPlayerNum++)
		{
			if ( m_sPlayerSlot[iTeamNum][iPlayerNum].uiSessionID ==  uiSessionID)
			{
				pSotIndex = iTeamNum;
				pPlayerIndex = iPlayerNum;
				return true;
			}
		}
	}

	ErrorLog("CDnPVPGameRoomDlg::FindUserSlot2 :: User Not Found%d",uiSessionID);
	
	return false;
}


void CDnPVPGameRoomDlg::UpdateSlot( bool ForceAll )
{
	WCHAR wszPlayerNUm[256];
	SecureZeroMemory(wszPlayerNUm,sizeof(wszPlayerNUm));

	for(int iTeamNum = 0 ; iTeamNum < MAXTeamNUM ; iTeamNum++)
	{
		for(int iPlayerNum = 0 ; iPlayerNum < PvP_TeamUserSlot ; iPlayerNum++)
		{
			if ( m_sPlayerSlot[iTeamNum][iPlayerNum].isNeedUpdate )
			{
				UpdateSlot(UITeam(iTeamNum),iPlayerNum);				
			}
		}
	}

	//방정보 수정	
	
	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TPVPWINCONDITION );
	if ( !pSox ) 
	{
		ErrorLog(" CDnPVPGameRoomDlg::UpdateSlot: PVPWINCONDITION table Not found!! ");
		return;
	}

	int nItemID = 1;

	for( int i=0 ; i<pSox->GetItemCount() ; ++i )
	{
		nItemID = pSox->GetItemID(i);

		if( m_cMaxPlayerNum == pSox->GetFieldFromLablePtr( nItemID , "PlayerNumber" )->GetInteger() )
			break;
	}


	if(m_bIndividualMode)
	{
#ifdef PRE_ADD_PVP_COMBOEXERCISE
		// 콤보연습모드의 최소인원은 1명.
		if( m_bComboExeMode )
			wsprintf( wszPlayerNUm,GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 120073 ),m_cCurrentPlayerNum,m_cMaxPlayerNum ,1 );
		else
			wsprintf( wszPlayerNUm,GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 120073 ),m_cCurrentPlayerNum,m_cMaxPlayerNum ,pSox->GetFieldFromLablePtr( nItemID , "Min_TeamPlayerNum_Needed" )->GetInteger()*2);
#else
		wsprintf( wszPlayerNUm,GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 120073 ),m_cCurrentPlayerNum,m_cMaxPlayerNum ,pSox->GetFieldFromLablePtr( nItemID , "Min_TeamPlayerNum_Needed" )->GetInteger()*2);
#endif // PRE_ADD_PVP_COMBOEXERCISE
	}
	else
		wsprintf( wszPlayerNUm,GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 120051 ),m_cCurrentPlayerNum,m_cMaxPlayerNum ,pSox->GetFieldFromLablePtr( nItemID , "Min_TeamPlayerNum_Needed" )->GetInteger());

	m_pUIPlayerNum->SetText(wszPlayerNUm);
}

void CDnPVPGameRoomDlg::UpdateSlot( UITeam Team , int SlotIndex  )
{
	if( SlotIndex >= PvP_TeamUserSlot  )
		return;

	WCHAR wszLevel[256];	
	int nStringTableID = 0;

	SecureZeroMemory(wszLevel,sizeof(wszLevel));	

	m_sPlayerSlot[Team][SlotIndex].isNeedUpdate = false;

	SetPlayerState(Team ,SlotIndex , Ready , true ); //닫힌 곳이나 오픈 슬롯은 상태 창 끈다

	m_sPlayerSlot[Team][SlotIndex].pOpen->Show(false);
	m_sPlayerSlot[Team][SlotIndex].pOpenText->Show(false);
	m_sPlayerSlot[Team][SlotIndex].pClose->Show(false);
	m_sPlayerSlot[Team][SlotIndex].pCloseText->Show(false);

	if( m_sPlayerSlot[Team][SlotIndex].emSlotState != InPlayer )
	{	

		if( m_sPlayerSlot[Team][SlotIndex].emSlotState == Open ) {
			m_sPlayerSlot[Team][SlotIndex].pOpen->Show(true);
			m_sPlayerSlot[Team][SlotIndex].pOpenText->Show(true);
		}
		else {
			m_sPlayerSlot[Team][SlotIndex].pClose->Show(true);
			m_sPlayerSlot[Team][SlotIndex].pCloseText->Show(true);
		}

		m_sPlayerSlot[Team][SlotIndex].pLevelUI->Show(false);
		m_sPlayerSlot[Team][SlotIndex].pClassUI->Show(false);
		m_sPlayerSlot[Team][SlotIndex].pNameUI->Show(false);
		m_sPlayerSlot[Team][SlotIndex].pPVPIcon->Show(false);
		return;
	}else 
	{
		m_sPlayerSlot[Team][SlotIndex].pLevelUI->Show(true);
		m_sPlayerSlot[Team][SlotIndex].pClassUI->Show(true);
		m_sPlayerSlot[Team][SlotIndex].pNameUI->Show(true);		 
	}

	if( IsReady(m_sPlayerSlot[Team][SlotIndex].uiUserState))
		SetPlayerState(Team ,SlotIndex , Ready );

	if( IsMaster(m_sPlayerSlot[Team][SlotIndex].uiUserState) )
		SetPlayerState(Team ,SlotIndex , Master );

	if( IsInGame(m_sPlayerSlot[Team][SlotIndex].uiUserState) )
		SetPlayerState(Team ,SlotIndex , Playing );

	if( IsGroupCaptain(m_sPlayerSlot[Team][SlotIndex].uiUserState) )
	{
		if( IsMaster(m_sPlayerSlot[Team][SlotIndex].uiUserState) )
			SetPlayerState(Team ,SlotIndex , MasterAndGroupCaptain );
		else
			SetPlayerState(Team ,SlotIndex , GroupCaptain );
	}

#ifdef PRE_MOD_CONSTANT_TEXT_CONVERT_TO_TABLE_TEXT
	wsprintf(wszLevel, L"%s %d",GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 7888 ), m_sPlayerSlot[Team][SlotIndex].cLevel);
#else
	wsprintf(wszLevel, L"LV %d",m_sPlayerSlot[Team][SlotIndex].cLevel);
#endif 

	m_sPlayerSlot[Team][SlotIndex].pClassUI->SetText( DN_INTERFACE::STRING::GetJobString( m_sPlayerSlot[Team][SlotIndex].iJobID ) );

	m_sPlayerSlot[Team][SlotIndex].pLevelUI->SetText(wszLevel);


	if(IsReady(m_sPlayerSlot[Team][SlotIndex].uiUserState) )
	{
		m_sPlayerSlot[Team][SlotIndex].pNameUI->SetTextColor( D3DCOLOR_ARGB( 255, 100, 205, 255 ) );
	}
	else if( IsMaster(m_sPlayerSlot[Team][SlotIndex].uiUserState) )
	{
		m_sPlayerSlot[Team][SlotIndex].pNameUI->SetTextColor( D3DCOLOR_ARGB( 255, 255, 205, 5 ) );
	}
	else
	{
		if( CDnBridgeTask::GetInstance().GetSessionID() ==  m_sPlayerSlot[Team][SlotIndex].uiSessionID )
			m_sPlayerSlot[Team][SlotIndex].pNameUI->SetTextColor( EtInterface::textcolor::PVP_MY_SLOTCOLOR );
		else
			m_sPlayerSlot[Team][SlotIndex].pNameUI->SetTextColor( textcolor::WHITE );
	}

#if defined( PRE_ADD_REVENGE )
	CDnRevengeTask * pRevengeTask = (CDnRevengeTask *)CTaskManager::GetInstance().GetTask( "RevengeTask" );
	if( NULL != pRevengeTask && CDnBridgeTask::GetInstance().GetSessionID() != m_sPlayerSlot[Team][SlotIndex].uiSessionID)
	{
		UINT eRevengeTargetType = Revenge::RevengeTarget::eRevengeTarget_None;
		pRevengeTask->GetRevengeUserID( m_sPlayerSlot[Team][SlotIndex].uiSessionID, eRevengeTargetType );

		if( Revenge::RevengeTarget::eRevengeTarget_Target == eRevengeTargetType )
			m_sPlayerSlot[Team][SlotIndex].pNameUI->SetTextColor( textcolor::PVP_REVENGE_TARGET );
		else if( Revenge::RevengeTarget::eRevengeTarget_Me == eRevengeTargetType )
			m_sPlayerSlot[Team][SlotIndex].pNameUI->SetTextColor( textcolor::PVP_REVENGE_ME );
	}
#endif	// #if defined( PRE_ADD_REVENGE )

	m_sPlayerSlot[Team][SlotIndex].pNameUI->SetText(m_sPlayerSlot[Team][SlotIndex].wszPlayerName);

	if( GetInterface().GetPVPIconTex() )
	{
		int iIconW,iIconH;
		int iU,iV;

		iIconW = GetInterface().GeticonWidth();
		iIconH = GetInterface().GeticonHeight();

		if( GetInterface().ConvertPVPGradeToUV( m_sPlayerSlot[Team][SlotIndex].cPVPLevel ,iU, iV ))
		{
			m_sPlayerSlot[Team][SlotIndex].pPVPIcon->SetTexture(GetInterface().GetPVPIconTex(),iU, iV ,iIconW,iIconH );
			m_sPlayerSlot[Team][SlotIndex].pPVPIcon->Show(true);
		}

	}
}

void CDnPVPGameRoomDlg::Initialize( bool bShow )
{
	CEtUIDialog::Initialize( CEtResourceMng::GetInstance().GetFullName( "PvpReady.ui" ).c_str(), bShow );
}


void CDnPVPGameRoomDlg::InitialUpdate()
{
	InitializeUserSlot();
	InitializeHidableStatic();
	InitializeCommonUI();

	m_pPVPMakeRoomDlg = new CDnPVPRoomEditDlg( UI_TYPE_MODAL );
	m_pPVPMakeRoomDlg->Initialize( false );

	m_pPVPPopupDlg = new CDnPVPPopupDlg(UI_TYPE_CHILD,this);
	m_pPVPPopupDlg->Initialize(false);

	m_pReadyCancelButton = GetControl<CEtUIButton>( "ID_PVP_READY_END" );
	m_pReadyCancelButton->Show(false);

	//rlkt_fix_ui
	for (int i = 0; i < 8; i++)
	{
		GetControl<CEtUIComboBox>(FormatA("ID_COMBOBOX_LEVEL_%d",i).c_str())->Show(false);
		GetControl<CEtUIComboBox>(FormatA("ID_COMBOBOX_MONSTER_%d", i).c_str())->Show(false);
	}

	const char *szFileName = CDnTableDB::GetInstance().GetFileName( 10007 );
	if( strlen( szFileName ) > 0 )
	{
		if( m_nBeepSound== -1 )
			m_nBeepSound = CEtSoundEngine::GetInstance().LoadSound( CEtResourceMng::GetInstance().GetFullName( szFileName ).c_str(), false, false );			
	}

	if( CGlobalInfo::GetInstance().GetGlobalMessageCode() != 0 )
	{
		GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 120064) , MB_OK);//입력없어서 강퇴
		CGlobalInfo::GetInstance().SetGlobalMessageCode( 0 );
	}
}

void CDnPVPGameRoomDlg::InitializeCommonUI()
{
	m_pTeamChangeButton = GetControl<CEtUIButton>( "ID_PVP_CHANGETEAM" );
	m_pTeamChangeButton ->SetDisableTime(2.0f);
	m_pReadyButton = GetControl<CEtUIButton>( "ID_PVP_READY" );
	m_pReadyButton ->SetDisableTime(2.0f);

	m_pMoveToObserver = GetControl<CEtUIButton>( "ID_PVP_OBSERVER" );
	m_pMoveToPlay = GetControl<CEtUIButton>( "ID_PVP_INGAME" );

	m_pStartButton = GetControl<CEtUIButton>( "ID_PVP_START" );
	m_pCancelButton = GetControl<CEtUIButton>( "ID_PVP_CANCEL" );
	
	m_pRoomName = GetControl<CEtUIStatic>( "ID_PVP_ROOMNAME" );
	m_pRoomIndexNum = GetControl<CEtUIStatic>( "ID_PVP_ROOMNUMBER" );
	



	m_pUIGameMode = GetControl<CEtUIStatic>( "ID_PVP_MAP_MODE" );		//
	m_pUIPlayerNum = GetControl<CEtUIStatic>( "ID_PVP_MAP_REGULAR" );	//
	m_pMiniMap = GetControl<CEtUITextureControl>( "ID_PVP_MAPIMG" );		//
	m_pMapName = GetControl<CEtUIStatic>( "ID_PVP_MAPNAME" );			//
	m_pRoomStateStatic = GetControl<CEtUIStatic>( "ID_PVP_MAP_STATE" );	// 
	m_pInGameJoin = GetControl<CEtUIStatic>( "ID_PVP_MAP_INTRUCTION" );	// 
	m_pInGameJoin->Show(false);											//
	m_pNotInGameJoin = GetControl<CEtUIStatic>( "ID_PVP_MAP_NOINTRUCTION" );//
	m_pNotInGameJoin->Show(false);										//
	
	m_pDropItem = GetControl<CEtUIStatic>( "ID_PVP_ITEM" );				//
	m_pNoDropItem = GetControl<CEtUIStatic>( "ID_PVP_NOITEM" );	//
	m_pDropItem->Show(false);		//
	m_pNoDropItem->Show(false);		//
	
	m_pRevision = GetControl<CEtUIStatic>( "ID_PVP_FIT" );				//
	m_pNoRevision = GetControl<CEtUIStatic>( "ID_PVP_NOFIT" );		//
	m_pRevision->Show( false );		//
	m_pNoRevision->Show( false );	//

	m_pShowHp = GetControl<CEtUIStatic>( "ID_PVP_HP" );		//
	m_pHideHp = GetControl<CEtUIStatic>( "ID_PVP_NOHP" );		//
	m_pShowHp->Show(false);		//
	m_pHideHp->Show(false);		//
	m_pWinCon = GetControl<CEtUIStatic>( "ID_PVP_MAP_WINCONDITION" );//





	m_pPasswordRoom = GetControl<CEtUIStatic>( "ID_PVP_SECRET" );
	m_pPasswordRoom->Show(false);

	m_pRoomEditButton = GetControl<CEtUIButton>( "ID_EDIT" );
	m_pRoomEditButton->Show(false);

	m_pKick = GetControl<CEtUIButton>( "ID_NORMAL_KICK" );
	m_pKick->Enable(false);
	m_pKick->Show(false);
	m_pPermanenceKick = GetControl<CEtUIButton>( "ID_PERMAN_KICK" );
	m_pPermanenceKick->Enable(false);
	m_pPermanenceKick->Show(false);
#if defined( PRE_ADD_REVENGE )
	m_pRevengeButton = GetControl<CEtUIButton>( "ID_REVENGE" );
	m_pRevengeButton->Enable( false );
	m_pRevengeButton->Show( false );
#endif	// #if defined( PRE_ADD_REVENGE )
	m_pChangeMasterButton = GetControl<CEtUIButton>( "ID_CHANGE_MASTER" );
	m_pChangeMasterButton->Enable(false);
	m_pChangeMasterButton->Show(false);

	m_pChangeGroupCaptain = GetControl<CEtUIButton>( "ID_CHANGE_GROUPLEADER" );
	m_pChangeGroupCaptain->Enable(false);
	m_pChangeGroupCaptain->Show(false);
	m_pStaticNameBreakInto	= GetControl<CEtUIStatic>( "ID_STATIC6" );
	m_pStaticNameUseItem	= GetControl<CEtUIStatic>( "ID_STATIC31" );

#ifdef PRE_ADD_PVP_HELP_MESSAGE
	for(int i = 0 ; i < E_PVP_RADIOBUTTON_MAX ; ++i)
		m_pRoomInfoButton[i] = GetControl<CEtUIRadioButton>( FormatA("ID_RBT_INFO%d",i).c_str() ); // 라디오 버튼

	m_pStaticModeInfo = GetControl<CEtUITextBox>("ID_TEXTBOX__MODEINFO"); // 모드 설명 백판
	m_pStaticModeInfo->Show(false);

	for(int i = 0 ; i < 2 ; ++i)
	{
		m_pStaticInfoBoard[i] = GetControl<CEtUIStatic>( FormatA("ID_STATIC_INFOBOARD%d",i).c_str() );
		m_pStaticInfoBoard[i]->Show(false);
	}

	for(int i = 0 ; i < 3 ; ++i)
		m_pStaticMapBoard[i] = GetControl<CEtUIStatic>( FormatA("ID_STATIC_MAPBOARD%d",i).c_str() );

	for(int i = 0 ; i < 8 ; ++i)
		m_pStaticTextMapInfo[i] = GetControl<CEtUIStatic>( FormatA("ID_TEXT_MAPINFO%d",i).c_str() ); // 인원,모드,상태~승리조건

	for(int i = 0 ; i < 5 ; ++i)
		m_pStaticMapLine[i] = GetControl<CEtUIStatic>( FormatA("ID_STATIC_MAPLINE%d",i).c_str() ); // 백판 라인

	m_pRoomInfoButton[E_PVP_RADIOBUTTON_ROOM_INFO]->SetChecked(true);
#endif

}

void CDnPVPGameRoomDlg::InitializeHidableStatic()
{
	m_pIndividualUIBar[0] = GetControl<CEtUIStatic>("ID_BAR_PVPALL0");
	m_pIndividualUIBar[1] = GetControl<CEtUIStatic>("ID_BAR_PVPALL1");
	m_pIndividualUIBar[2] = GetControl<CEtUIStatic>("ID_TEXT_PVPALL");
	m_pTeamUIBar[0] = GetControl<CEtUIStatic>("ID_TEXT_PVPTEAM0");
	m_pTeamUIBar[1] = GetControl<CEtUIStatic>("ID_TEXT_PVPTEAM1");
	m_pTeamUIBar[2] = GetControl<CEtUIStatic>("ID_BAR_PVPTEAM0");
	m_pTeamUIBar[3] = GetControl<CEtUIStatic>("ID_BAR_PVPTEAM1");
	m_pTeamUIBar[4] = GetControl<CEtUIStatic>("ID_BAR_PVPTEAM2");
	m_pTeamUIBar[5] = GetControl<CEtUIStatic>("ID_BAR_PVPTEAM3");
	m_pTeamUIBar[6] = GetControl<CEtUIStatic>("ID_BOARD_ALL0");
	m_pTeamUIBar[7] = GetControl<CEtUIStatic>("ID_BOARD_ALL1");

	for(int i=0;i<3;i++)
		m_pIndividualUIBar[i]->Show(false);

	for(int i=0;i<8;i++)
		m_pTeamUIBar[i]->Show(true);
}

void CDnPVPGameRoomDlg::InitializeUserSlot()
{
	char szUIName[256];
	char szUITeam[256];

	SecureZeroMemory(szUIName,sizeof(szUIName));
	SecureZeroMemory(szUITeam,sizeof(szUITeam));

	for(int iTeamNum = 0 ; iTeamNum < MAXTeamNUM ; iTeamNum++)
	{
		for(int iPlayerNum = 0 ; iPlayerNum < PvP_TeamUserSlot ; iPlayerNum++)
		{
			if(iTeamNum == 0 )
				sprintf(szUITeam,"BLUE");
			else
				sprintf(szUITeam,"RED");

			sprintf(szUIName,"ID_PVP_%s_LV%d",szUITeam,iPlayerNum);
			m_sPlayerSlot[iTeamNum][iPlayerNum].pLevelUI = GetControl<CEtUIStatic>( szUIName );	
	
			sprintf(szUIName,"ID_PVP_%s_JOB%d",szUITeam,iPlayerNum);
			m_sPlayerSlot[iTeamNum][iPlayerNum].pClassUI = GetControl<CEtUIStatic>( szUIName );	
			
			sprintf(szUIName,"ID_PVP_%s_NAME%d",szUITeam,iPlayerNum);
			m_sPlayerSlot[iTeamNum][iPlayerNum].pNameUI  = GetControl<CEtUIStatic>( szUIName );

			sprintf(szUIName,"ID_PVP_%s_OPEN%d",szUITeam,iPlayerNum);
			m_sPlayerSlot[iTeamNum][iPlayerNum].pOpen  = GetControl<CEtUIStatic>( szUIName );

			sprintf(szUIName,"ID_PVP_%s_CLOSE%d",szUITeam,iPlayerNum);
			m_sPlayerSlot[iTeamNum][iPlayerNum].pClose  = GetControl<CEtUIStatic>( szUIName );

			sprintf(szUIName,"ID_PVP_%s_RANK%d",szUITeam,iPlayerNum);
			m_sPlayerSlot[iTeamNum][iPlayerNum].pPVPIcon = GetControl<CEtUITextureControl>( szUIName );
			
			sprintf(szUIName,"ID_PVP_%s_OPENTEXT%d",szUITeam,iPlayerNum);
			m_sPlayerSlot[iTeamNum][iPlayerNum].pOpenText  = GetControl<CEtUIStatic>( szUIName );

			sprintf(szUIName,"ID_PVP_%s_CLOSETEXT%d",szUITeam,iPlayerNum);
			m_sPlayerSlot[iTeamNum][iPlayerNum].pCloseText  = GetControl<CEtUIStatic>( szUIName );

			sprintf(szUIName,"ID_%s_SELECT%d",szUITeam,iPlayerNum);
			m_sPlayerSlot[iTeamNum][iPlayerNum].pSelect = GetControl<CEtUIStatic>( szUIName );
			m_sPlayerSlot[iTeamNum][iPlayerNum].pSelect->Show(false);

			sprintf(szUIName,"ID_PVP_%s_BOSS%d",szUITeam,iPlayerNum);
			m_sPlayerSlot[iTeamNum][iPlayerNum].pStateUI[Master]  = GetControl<CEtUIStatic>( szUIName );
			m_sPlayerSlot[iTeamNum][iPlayerNum].pStateUI[Master]->Show(false);

			sprintf(szUIName,"ID_PVP_%s_READY%d",szUITeam,iPlayerNum);
			m_sPlayerSlot[iTeamNum][iPlayerNum].pStateUI[Ready]  = GetControl<CEtUIStatic>( szUIName );
			m_sPlayerSlot[iTeamNum][iPlayerNum].pStateUI[Ready]->Show(false);

			sprintf(szUIName,"ID_PVP_%s_INGAME%d",szUITeam,iPlayerNum);
			m_sPlayerSlot[iTeamNum][iPlayerNum].pStateUI[Playing]  = GetControl<CEtUIStatic>( szUIName );
			m_sPlayerSlot[iTeamNum][iPlayerNum].pStateUI[Playing]->Show(false);

			sprintf(szUIName,"ID_PVP_%s_CAPTAIN%d",szUITeam,iPlayerNum);
			m_sPlayerSlot[iTeamNum][iPlayerNum].pStateUI[GroupCaptain]  = GetControl<CEtUIStatic>( szUIName );
		
			sprintf(szUIName,"ID_PVP_%s_MASTER%d",szUITeam,iPlayerNum);
			m_sPlayerSlot[iTeamNum][iPlayerNum].pStateUI[MasterAndGroupCaptain]  = GetControl<CEtUIStatic>( szUIName );
		}
	}
}

void CDnPVPGameRoomDlg::ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	CEtUIDialog::ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );

	SetCmdControlName( pControl->GetControlName() );

	if( nCommand == EVENT_BUTTON_CLICKED )
	{
		if( IsCmdControl("ID_PVP_CANCEL" ) )
		{
			GetInterface().GetPVPLobbyChatTabDlg()->RemoveObserver(0,true); // 
			SendPvPLeaveRoom();			
			return;
		}

		if( IsCmdControl("ID_PVP_READY" ) )
		{
			DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TPVPGAMEMODE );
			if(pSox && pSox->IsExistItem(m_nGameModeID))
			{
				int nNeedInvenCount = 0;
				nNeedInvenCount = pSox->GetFieldFromLablePtr( m_nGameModeID , "_NeedInven" )->GetInteger();

				if(nNeedInvenCount != 0)
				{
					CDnInvenTabDlg *pInvenDlg = (CDnInvenTabDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::INVENTORY_DIALOG );
					if( pInvenDlg && pInvenDlg->GetEmptySlotCount() < nNeedInvenCount )
					{
						WCHAR wszMsg[256]={0,};
						wsprintf( wszMsg, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 120077 ), nNeedInvenCount );
						GetInterface().ShowCaptionDialog( CDnInterface::typeCaption4,  wszMsg , textcolor::ORANGERED ,5.0f );
						return;
					}
				}
			}
			SendPvPReady( IsReady(m_uiMyUserState)?0:1 );		
			return;
		}

		if( IsCmdControl("ID_PVP_READY_END" ) )
		{
			SendPvPReady( IsReady(m_uiMyUserState)?0:1 );		
			return;
		}

		if( IsCmdControl("ID_PVP_START" ) )
		{
			DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TPVPGAMEMODE );
			if(pSox && pSox->IsExistItem(m_nGameModeID))
			{
				int nNeedInvenCount = 0;
				nNeedInvenCount = pSox->GetFieldFromLablePtr( m_nGameModeID , "_NeedInven" )->GetInteger();

				if(nNeedInvenCount != 0)
				{
					CDnInvenTabDlg *pInvenDlg = (CDnInvenTabDlg*)GetInterface().GetMainMenuDialog( CDnMainMenuDlg::INVENTORY_DIALOG );
					if( pInvenDlg && pInvenDlg->GetEmptySlotCount() < nNeedInvenCount )
					{
						WCHAR wszMsg[256]={0,};
						wsprintf( wszMsg, GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 120079 ), nNeedInvenCount );
						GetInterface().ShowCaptionDialog( CDnInterface::typeCaption4,  wszMsg , textcolor::ORANGERED ,5.0f );
						return;
					}
				}
			}

#ifdef _FINAL_BUILD
			SendPvPStart( PvPCommon::Check::AllCheck );
#else
			SendPvPStart( CGlobalValue::GetInstance().m_bPVPGameIgnoreCondition?PvPCommon::Check::AllCheck:PvPCommon::Check::CheckTimeOver|PvPCommon::Check::CheckScore );
#endif
			return;
		}

		if( IsCmdControl("ID_PVP_CHANGETEAM" ) )
		{
			SendPvPChangeTeam( m_cMyTeam == TeamA?PvPCommon::Team::B:PvPCommon::Team::A );
			return;
		}

		if( IsCmdControl("ID_EDIT" ) )
		{
#ifdef PRE_MOD_PVP_ROOM_CREATE
			m_pPVPMakeRoomDlg->Show( true );		
			m_pPVPMakeRoomDlg->SetMaxPlayerDownList( m_cMaxPlayerNum );
			m_pPVPMakeRoomDlg->SetMapAndModeDropDownList(m_uiMapIndex, m_nGameModeID, m_pMapName->GetText());
			m_pPVPMakeRoomDlg->SetGameWinObjectNum( m_pWinCon->GetText() );
			m_pPVPMakeRoomDlg->SetRoomName( m_wszOriginalRoomName );
			m_pPVPMakeRoomDlg->SetPassword();
			m_pPVPMakeRoomDlg->SetInGameJoin(m_IsBreakIntoFlag );
			m_pPVPMakeRoomDlg->SetDropItem( m_bDropItem );
			m_pPVPMakeRoomDlg->SetShowHp(m_bShowHp);
			m_pPVPMakeRoomDlg->SetRevision( m_bRevision );
			m_pPVPMakeRoomDlg->SetRandomTeamMode(m_bRandomTeamMode);
			m_pPVPMakeRoomDlg->SetEditState(true);
			m_pPVPMakeRoomDlg->SetMinMaxPlayerLevel(GetMinPlayerLevel(),GetMaxPlayerLevel());
			m_pPVPMakeRoomDlg->SetPlayerLevel(m_nMinLevel,m_nMaxLevel);
			m_pPVPMakeRoomDlg->SetRandomOrder(m_bRandomOrder);
#ifdef PRE_MOD_PVPOBSERVER
			m_pPVPMakeRoomDlg->SetObserver(m_bAllowObserver);
#endif

#else // PRE_MOD_PVP_ROOM_CREATE
			m_pPVPMakeRoomDlg->Show( true );		
			m_pPVPMakeRoomDlg->SetMaxPlayerDownList( m_cMaxPlayerNum );
			m_pPVPMakeRoomDlg->SetMapDropDownList(m_uiMapIndex, m_pMapName->GetText() , m_cMaxPlayerNum);
			m_pPVPMakeRoomDlg->SetGameModeDownList( m_uiMapIndex , m_pUIGameMode->GetText() );
			m_pPVPMakeRoomDlg->SetGameWinObjectNum( m_pWinCon->GetText() );
			m_pPVPMakeRoomDlg->SetRoomName( m_wszOriginalRoomName );
			m_pPVPMakeRoomDlg->SetPassword();
			m_pPVPMakeRoomDlg->SetInGameJoin(m_IsBreakIntoFlag );
			m_pPVPMakeRoomDlg->SetDropItem( m_bDropItem );
			m_pPVPMakeRoomDlg->SetShowHp(m_bShowHp);
			m_pPVPMakeRoomDlg->SetRevision( m_bRevision );
			m_pPVPMakeRoomDlg->SetRandomTeamMode(m_bRandomTeamMode);
			m_pPVPMakeRoomDlg->SetEditState(true);
			m_pPVPMakeRoomDlg->SetMinMaxPlayerLevel(GetMinPlayerLevel(),GetMaxPlayerLevel());
			m_pPVPMakeRoomDlg->SetPlayerLevel(m_nMinLevel,m_nMaxLevel);
			m_pPVPMakeRoomDlg->SetRandomOrder(m_bRandomOrder);
#ifdef PRE_MOD_PVPOBSERVER
			m_pPVPMakeRoomDlg->SetObserver(m_bAllowObserver);
#endif
#endif // PRE_MOD_PVP_ROOM_CREATE
			return;
		}	

		if( IsCmdControl("ID_PVP_OBSERVER") ) // 옵저버로 이동 !
		{
			SendPvPChangeTeam(PvPCommon::Team::Observer); // Observer = 1003입니다
			return;
		}

		if( IsCmdControl("ID_PVP_INGAME") ) // 플레이어로 이동 !
		{
			// 어느쪽이 슬롯이 더 많이 남아있는가 검색 A이냐 B냐 결정
			if(m_cCurrentTeamNum_A <= m_cCurrentTeamNum_B ) // 동등할때는 A팀이 우선순위를 가집니다. 
			{
				SendPvPChangeTeam(PvPCommon::Team::A);
			}
			else if(m_cCurrentTeamNum_A > m_cCurrentTeamNum_B )
			{
				SendPvPChangeTeam(PvPCommon::Team::B);
			}
		
			return;
		}

		if( IsCmdControl("ID_NORMAL_KICK") )
		{
			int iTeamNum = 0;
			int iPlayerNum = 0;
			for(iTeamNum = 0 ; iTeamNum < MAXTeamNUM ; iTeamNum++)
			{
				for(iPlayerNum = 0 ; iPlayerNum < PvP_TeamUserSlot ; iPlayerNum++)
				{
					if( m_sPlayerSlot[iTeamNum][iPlayerNum].pSelect->IsShow() ) {
						if( m_sPlayerSlot[iTeamNum][iPlayerNum].emSlotState == InPlayer ) {

							if(m_sPlayerSlot[iTeamNum][iPlayerNum].uiSessionID != CDnBridgeTask::GetInstance().GetSessionID()) // 자신은 강퇴불가
							{
								SendPvPBan( m_sPlayerSlot[iTeamNum][iPlayerNum].uiSessionID, PvPCommon::BanType::Normal );
								return;
							}
						}
						
					}
				}
			}
			CEtUIListBoxEx* pUI = GetInterface().GetPVPLobbyChatTabDlg()->GetObserverListBox();
			if(GetInterface().GetPVPLobbyChatTabDlg()->GetObserverNumber() > 0)
			{
				if(GetInterface().GetPVPLobbyChatTabDlg()->IsSelectedListBox()) // 관전자가 없는데 강퇴 시키면 안됩니다. 
				{
					int SelectedSessionID = GetInterface().GetPVPLobbyChatTabDlg()->GetSessionIDFromList(pUI->GetSelectedIndex());
					SendPvPBan( SelectedSessionID, PvPCommon::BanType::Normal );
					GetInterface().GetPVPLobbyChatTabDlg()->RemoveObserver(SelectedSessionID);
				}
				return;
			}
			// 관전자를 강퇴 해줍니다.
			return;
		}
		
		if( IsCmdControl("ID_PERMAN_KICK") )
		{
			int iTeamNum = 0;
			int iPlayerNum = 0;
			for(iTeamNum = 0 ; iTeamNum < MAXTeamNUM ; iTeamNum++)
			{
				for(iPlayerNum = 0 ; iPlayerNum < PvP_TeamUserSlot ; iPlayerNum++)
				{
					if( m_sPlayerSlot[iTeamNum][iPlayerNum].pSelect->IsShow() ) {
						if( m_sPlayerSlot[iTeamNum][iPlayerNum].emSlotState == InPlayer ) {

							if(m_sPlayerSlot[iTeamNum][iPlayerNum].uiSessionID != CDnBridgeTask::GetInstance().GetSessionID()) // 자신은 강퇴불가
							{
								SendPvPBan( m_sPlayerSlot[iTeamNum][iPlayerNum].uiSessionID, PvPCommon::BanType::Permanence );
								return;
							}
						}
					}
				}
			}

			CEtUIListBoxEx* pUI = GetInterface().GetPVPLobbyChatTabDlg()->GetObserverListBox();
		
			if(GetInterface().GetPVPLobbyChatTabDlg()->GetObserverNumber() > 0) // 관전자가 없는데 강퇴 시키면 안됩니다. 
			{
				if(GetInterface().GetPVPLobbyChatTabDlg()->IsSelectedListBox())
				{
					int SelectedSessionID = GetInterface().GetPVPLobbyChatTabDlg()->GetSessionIDFromList(pUI->GetSelectedIndex());
					SendPvPBan( SelectedSessionID, PvPCommon::BanType::Permanence );
					GetInterface().GetPVPLobbyChatTabDlg()->RemoveObserver(SelectedSessionID);
					return;
				}
			}

			
			// 관전자를 영구강퇴 해줍니다.
			return;
		}
		
		if( IsCmdControl("ID_CHANGE_MASTER") )
		{
			int iTeamNum = 0;
			int iPlayerNum = 0;
			for(iTeamNum = 0 ; iTeamNum < MAXTeamNUM ; iTeamNum++)
			{
				for(iPlayerNum = 0 ; iPlayerNum < PvP_TeamUserSlot ; iPlayerNum++)
				{
					if( m_sPlayerSlot[iTeamNum][iPlayerNum].pSelect->IsShow() ) {
						if( m_sPlayerSlot[iTeamNum][iPlayerNum].emSlotState == InPlayer ) {

							if(m_sPlayerSlot[iTeamNum][iPlayerNum].uiSessionID != CDnBridgeTask::GetInstance().GetSessionID()) // 자신은 위임불가
							{
								SendPvPChangeCaptain( m_sPlayerSlot[iTeamNum][iPlayerNum].uiSessionID , PvPCommon::CaptainType::eCode::Captain );
								return;
							}
						}
					}
				}
			}
		}
		if( IsCmdControl("ID_CHANGE_GROUPLEADER") )
		{
			int iTeamNum = 0;
			int iPlayerNum = 0;
			for(iTeamNum = 0 ; iTeamNum < MAXTeamNUM ; iTeamNum++)
			{
				for(iPlayerNum = 0 ; iPlayerNum < PvP_TeamUserSlot ; iPlayerNum++)
				{
					if( m_sPlayerSlot[iTeamNum][iPlayerNum].pSelect->IsShow() ) {
						if( m_sPlayerSlot[iTeamNum][iPlayerNum].emSlotState == InPlayer ) {

							if(m_sPlayerSlot[iTeamNum][iPlayerNum].uiSessionID != CDnBridgeTask::GetInstance().GetSessionID())
							{
								SendPvPChangeCaptain( m_sPlayerSlot[iTeamNum][iPlayerNum].uiSessionID , PvPCommon::CaptainType::eCode::GroupCaptain );
								return;
							}
						}
					}
				}
			}
		}
	}
	else if( nCommand == EVENT_EDITBOX_CHANGE )
	{
		if( !IsMaster( m_uiMyUserState ) )
		{
			m_fElapsedKickTime = 0.0f;
			m_nWaringNum = 0;
		}
	}
	else if( nCommand == EVENT_RADIOBUTTON_CHANGED )
	{
#ifdef PRE_ADD_PVP_HELP_MESSAGE
		if( strstr(pControl->GetControlName() , "ID_RBT_INFO") )
		{
			int nTabID = static_cast<CEtUIRadioButton*>(pControl)->GetTabID();
			ChangeRoomInformation(nTabID);
		}
#endif
	}
}

#ifdef PRE_ADD_PVP_HELP_MESSAGE
void CDnPVPGameRoomDlg::ChangeRoomInformation(int nTabID)
{
	m_nRadioButtonIndex = nTabID;
	
	if(nTabID == E_PVP_RADIOBUTTON_ROOM_INFO)
	{
		bool bShow = true;

		//--------------------------
		// 맵 관련 컨트롤
		if(m_pUIGameMode)
			m_pUIGameMode->Show(bShow);
		if(m_pUIPlayerNum)
			m_pUIPlayerNum->Show(bShow);
		if(m_pMiniMap)
			m_pMiniMap->Show(bShow);
		if(m_pMapName)
			m_pMapName->Show(bShow);
		if(m_pRoomStateStatic)
			m_pRoomStateStatic->Show(bShow);
		if(m_pInGameJoin)
			m_pInGameJoin->Show(m_IsBreakIntoFlag);
		if(m_pNotInGameJoin)
			m_pNotInGameJoin->Show(!m_IsBreakIntoFlag);
		if(m_pRevision)
			m_pRevision->Show(!m_bRevision);
		if(m_pNoRevision)
			m_pNoRevision->Show(m_bRevision);
		if(m_pDropItem)
			m_pDropItem->Show(m_bDropItem);
		if(m_pNoDropItem)
			m_pNoDropItem->Show(!m_bDropItem);
		if(m_pShowHp)
			m_pShowHp->Show(m_bShowHp);
		if(m_pHideHp)
			m_pHideHp->Show(!m_bShowHp);
		if(m_pWinCon)
			m_pWinCon->Show(bShow);

		for(int i = 0 ; i < 3 ; ++i)
		{
			if(m_pStaticMapBoard[i])
				m_pStaticMapBoard[i]->Show(bShow);
		}

		for(int i = 0 ; i < 8 ; ++i)
		{
			if(m_pStaticTextMapInfo[i])
				m_pStaticTextMapInfo[i]->Show(bShow);
		}

		for(int i = 0 ; i < 5 ; ++i)
		{
			if(m_pStaticMapLine[i])
				m_pStaticMapLine[i]->Show(bShow);
		}


		//--------------------------
		// 모드 관련 컨트롤
		for(int i = 0 ; i < 2 ; ++i)
		{
			// 모드설명 백판
			if(m_pStaticInfoBoard[i])
				m_pStaticInfoBoard[i]->Show(!bShow);
		}

		m_pStaticModeInfo->ClearText();
		m_pStaticModeInfo->Show(!bShow);
	}
	else if(nTabID == E_PVP_RADIOBUTTON_MODE_INFO)
	{
		bool bShow = false;

		//--------------------------
		// 맵 관련 컨트롤
		if(m_pUIGameMode)
			m_pUIGameMode->Show(bShow);
		if(m_pUIPlayerNum)
			m_pUIPlayerNum->Show(bShow);
		if(m_pMiniMap)
			m_pMiniMap->Show(bShow);
		if(m_pMapName)
			m_pMapName->Show(bShow);
		if(m_pRoomStateStatic)
			m_pRoomStateStatic->Show(bShow);
		if(m_pInGameJoin)
			m_pInGameJoin->Show(bShow);
		if(m_pNotInGameJoin)
			m_pNotInGameJoin->Show(bShow);
		if(m_pRevision)
			m_pRevision->Show(bShow);
		if(m_pNoRevision)
			m_pNoRevision->Show(bShow);
		if(m_pDropItem)
			m_pDropItem->Show(bShow);
		if(m_pNoDropItem)
			m_pNoDropItem->Show(bShow);
		if(m_pShowHp)
			m_pShowHp->Show(bShow);
		if(m_pHideHp)
			m_pHideHp->Show(bShow);
		if(m_pWinCon)
			m_pWinCon->Show(bShow);

		for(int i = 0 ; i < 3 ; ++i)
		{
			if(m_pStaticMapBoard[i])
				m_pStaticMapBoard[i]->Show(bShow);
		}

		for(int i = 0 ; i < 8 ; ++i)
		{
			if(m_pStaticTextMapInfo[i])
				m_pStaticTextMapInfo[i]->Show(bShow);
		}

		for(int i = 0 ; i < 5 ; ++i)
		{
			if(m_pStaticMapLine[i])
				m_pStaticMapLine[i]->Show(bShow);
		}


		//--------------------------
		// 모드 관련 컨트롤
		for(int i = 0 ; i < 2 ; ++i)
		{
			// 모드설명 백판
			if(m_pStaticInfoBoard[i])
				m_pStaticInfoBoard[i]->Show(!bShow);
		}

		m_pStaticModeInfo->ClearText();
		if(nTabID == E_PVP_RADIOBUTTON_MODE_INFO)
		{
			// 툴팁 설정
			DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TPVPGAMEMODE );
			if (!pSox) return;

			int nToolTipStringID = pSox->GetFieldFromLablePtr(m_nGameModeID , "_TooltipUIString")->GetInteger();
			std::vector<std::wstring> vecString;
			TokenizeW_NewLine(GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nToolTipStringID ), vecString, L"\\n" );

			for(int i = 0 ; i < (int)vecString.size() ; ++i)
			{
				m_pStaticModeInfo->AddText(vecString[i].c_str());
			}
		}

		// 모드설명 Text Static
		m_pStaticModeInfo->Show(!bShow);
	}
}
#endif

void CDnPVPGameRoomDlg::SetPlayerState( int iteam , int Player , UserState emUserState , bool IsOff )
{
	if( Player >= PvP_TeamUserSlot || iteam >= MAXTeamNUM )
		return;

	if( IsOff )
	{
		for(int i=0;i< State_Max;i++)
		{
			if( m_sPlayerSlot[iteam][Player].pStateUI[i] )
				m_sPlayerSlot[iteam][Player].pStateUI[i]->Show(false);
		     
		}
		return;
	}

	for(int i=0;i< State_Max;i++)
	{
		m_sPlayerSlot[iteam][Player].pStateUI[i]->Show( i == emUserState ? true:false );
	}

}

void CDnPVPGameRoomDlg::ChangeTeam( UINT uiSessionID , int cTeam, char cTeamSlotIndex )
{

	int nPlayerTeamIndex = -1;

	switch(cTeam)
	{
	case (PvPCommon::Team::A):
		nPlayerTeamIndex = 0;
		break;
	case (PvPCommon::Team::B):
		nPlayerTeamIndex = 1;
		break;
	case (PvPCommon::Team::Observer):
		break;
	}

	//먼저 찾고
	int iSlotIndex = -1;
	int iPlayerIndex= -1;

	if( CDnBridgeTask::GetInstance().GetSessionID() ==  uiSessionID )//자기 자신일때 
		m_cMyTeam = nPlayerTeamIndex;


	bool IsObserver = false;

	for(int i=0;i<GetInterface().GetPVPLobbyChatTabDlg()->GetObserverNumber();i++)
	{
		if(GetInterface().GetPVPLobbyChatTabDlg()->GetSessionIDFromList(i) == uiSessionID) 
		{
			IsObserver = true; // 세션아이디가 옵져버 리스트중에 하나일때 = 옵져버 -> 플레이어 전환
		}
	}

	if(!IsObserver) // 일반 이동 처리 / 일반 - > 옵져버 이동
	{
		if( FindUserSlot( uiSessionID , iSlotIndex , iPlayerIndex )) // 기본 슬롯중에 찾았다 = 나는 슬롯 플레이어다
		{
			if( nPlayerTeamIndex == iSlotIndex)  // SlotIndex = A,B팀 
			{
				DebugLog("CDnPVPGameRoomDlg::ChangeTeam  %d is request change same team( %d).",uiSessionID, nPlayerTeamIndex );
				return;
			}

			sUserSlot MovePlayer;

			MovePlayer.emSlotState = m_sPlayerSlot[iSlotIndex][iPlayerIndex].emSlotState;
			MovePlayer.iJobID = m_sPlayerSlot[iSlotIndex][iPlayerIndex].iJobID;
			MovePlayer.cLevel = m_sPlayerSlot[iSlotIndex][iPlayerIndex].cLevel;
			MovePlayer.cPVPLevel = m_sPlayerSlot[iSlotIndex][iPlayerIndex].cPVPLevel;
			MovePlayer.uiUserState = m_sPlayerSlot[iSlotIndex][iPlayerIndex].uiUserState ;
			MovePlayer.isNeedUpdate = m_sPlayerSlot[iSlotIndex][iPlayerIndex].isNeedUpdate = true;
			MovePlayer.uiSessionID = m_sPlayerSlot[iSlotIndex][iPlayerIndex].uiSessionID;
			_wcscpy( MovePlayer.wszPlayerName  , _countof(MovePlayer.wszPlayerName), m_sPlayerSlot[iSlotIndex][iPlayerIndex].wszPlayerName, (int)wcslen(m_sPlayerSlot[iSlotIndex][iPlayerIndex].wszPlayerName));

			RemovePlayer( uiSessionID  );

			if( cTeam != (PvPCommon::Team::Observer))
			{
				InsertPlayer(cTeam ,
					MovePlayer.uiUserState,	
					uiSessionID , 
					MovePlayer.iJobID ,
					MovePlayer.cLevel ,
					MovePlayer.cPVPLevel ,
					MovePlayer.wszPlayerName
					);
			}
			else if(cTeam == (PvPCommon::Team::Observer))
			{
				GetInterface().GetPVPLobbyChatTabDlg()->InsertObserver(cTeam,
					MovePlayer.uiUserState,
					uiSessionID , 
					MovePlayer.iJobID, 
					MovePlayer.cLevel,
					MovePlayer.cPVPLevel,
					MovePlayer.wszPlayerName
					);
			}
	
			UpdateSlot( false );
		}
	}
	else // 옵져버 - > 플레이어 이동
	{
				
		CDnPVPLobbyChatTabDlg::sObserverSlot sSlot = GetInterface().GetPVPLobbyChatTabDlg()->GetObserverSlotFromSessionID(uiSessionID); // 옵져버에 저장되어있는 정보
		GetInterface().GetPVPLobbyChatTabDlg()->RemoveObserver(uiSessionID,false);  // 옵져버 슬롯에서 지워줌
		
		InsertPlayer(cTeam ,
			sSlot.uiUserState,	
			sSlot.uiSessionID , 
			sSlot.iJobID ,
			sSlot.cLevel ,
			sSlot.cPVPLevel ,
			sSlot.wszPlayerName 
			, 0
			); // 플레이어로 토스
		
		UpdateSlot( false );

		CEtUIListBoxEx* pUI = GetInterface().GetPVPLobbyChatTabDlg()->GetObserverListBox();
#ifdef PRE_MOD_PVPOBSERVER
		if(pUI->GetSelectedIndex() == -1 &&
			( CGlobalInfo::GetInstance().m_cLocalAccountLevel < eAccountLevel::AccountLevel_New ) )
#else
		if(pUI->GetSelectedIndex() == -1)
#endif // PRE_MOD_PVPOBSERVER
		{
			m_pKick->Enable(false);
			m_pPermanenceKick->Enable(false);
		}

	}
}

void CDnPVPGameRoomDlg::SetRoomState( UINT uiRoomState )
{
	WCHAR wszRoomState[256];
	wsprintf(wszRoomState,L"%d",uiRoomState);
	m_pRoomStateStatic->SetText(wszRoomState);
	m_uiRoomState = uiRoomState;
	SetUIRoomState(m_uiRoomState);
	if( IsStartingGame(m_uiMyUserState) || IsInGame(m_uiMyUserState))
		return;	

}

void CDnPVPGameRoomDlg::UpdatePvPLevel( SCPVP_LEVEL* pPacket )
{
	sUserSlot* pSlot = static_cast<sUserSlot*>(FindUserSlot( pPacket->uiSessionID ));
	if( !pSlot )
		return;

	pSlot->isNeedUpdate = true;
	pSlot->cPVPLevel	= pPacket->cLevel;

	UpdateSlot( false );
}

void CDnPVPGameRoomDlg::SetUIRoomState( UINT uiRoomState )
{
	int nRoomStateUIString = 120028;	

	if(IsSyncingRoom())
		nRoomStateUIString = 120029;
	
	if(IsStartingRoom())
		nRoomStateUIString = 120029;
	
	if(IsPlayingRoom())
		nRoomStateUIString = 120030;

	const wchar_t * wszRoomState = NULL; 
	wszRoomState = GetEtUIXML().GetUIString( CEtUIXML::idCategory1, nRoomStateUIString );

	m_pPasswordRoom->Show(IsPasssWordRoom());

	if( wszRoomState )
		m_pRoomStateStatic->SetText(wszRoomState);
}

void CDnPVPGameRoomDlg::SetButtonStates( )
{
	bool IsEnableStartGame =  false;
	bool IsEnableCancel =  false;
	bool IsEnableReady  =  false;
	bool IsEnableEdit = false;
	bool IsEnableTeamChange =  false;
	bool IsEnableMoveToObserver = false;
	bool IsEnableMoveToPlay = false;
	bool IsEnableChangeMasterButton = false;
	bool IsEnableKickButton = false;
	bool IsPlayerObserver = false;
	int  nSessionID = CDnBridgeTask::GetInstance().GetSessionID();

#ifdef PRE_MOD_PVPOBSERVER
	bool bIsMaster = IsMaster( m_uiMyUserState );
#else
	const bool bIsMaster = IsMaster( m_uiMyUserState );
#endif // PRE_MOD_PVPOBSERVER
	const bool bIsReady = IsReady( m_uiMyUserState );

#ifdef PRE_MOD_PVPOBSERVER
	//bool bGameMaster = ( CGlobalInfo::GetInstance().m_cLocalAccountLevel >= eAccountLevel::AccountLevel_New );			
	bool bGameMaster = false;
	if(CDnActor::s_hLocalActor)
		bGameMaster = static_cast<CDnLocalPlayerActor*>(CDnActor::s_hLocalActor.GetPointer())->IsDeveloperAccountLevel(CGlobalInfo::GetInstance().m_cLocalAccountLevel);
#endif // PRE_MOD_PVPOBSERVER

	for(int i=0;i<GetInterface().GetPVPLobbyChatTabDlg()->GetObserverNumber();i++)
	{
		if(GetInterface().GetPVPLobbyChatTabDlg()->GetSessionIDFromList(i) == nSessionID) 
			IsPlayerObserver = true;
	}

	if(IsReadyRoom())
	{
		IsEnableStartGame = bIsMaster;
		IsEnableCancel = true; 
		IsEnableReady = false == bIsMaster && !IsPlayerObserver;
		IsEnableEdit = bIsMaster;
		IsEnableTeamChange = false == bIsReady && !IsPlayerObserver;		
		IsEnableMoveToObserver = false == bIsMaster && false == bIsReady;
#ifdef PRE_MOD_PVPOBSERVER				
		IsEnableMoveToPlay = !bIsMaster;
		if( m_bEventRoom && bIsMaster )
			IsEnableMoveToPlay = false;				
#endif // PRE_MOD_PVPOBSERVER

		IsEnableChangeMasterButton = bIsMaster;
		IsEnableKickButton = bIsMaster;
	}
	
	if( IsSyncingRoom() )
		IsEnableCancel =  true;

	if( IsStartingRoom() )
	{
		IsEnableChangeMasterButton = false;
		IsEnableKickButton = false;
	}

	if( IsPlayingRoom() )
	{
		IsEnableCancel = true;
		IsEnableTeamChange = true;
		IsEnableMoveToObserver = true;
		IsEnableMoveToPlay = true;

		if( m_IsBreakIntoFlag || IsPlayerObserver )
			IsEnableStartGame = true;
	}

#ifdef PRE_MOD_PVPOBSERVER
	int nPlayerCnt = m_bEventRoom ? PvPCommon::Common::ExtendMaxObserverPlayer : PvPCommon::Common::MaxObserverPlayer;		
	if(GetInterface().GetPVPLobbyChatTabDlg()->GetObserverNumber() >= nPlayerCnt )
#else
	if(GetInterface().GetPVPLobbyChatTabDlg()->GetObserverNumber() >= PvPCommon::Common::MaxObserverPlayer)
#endif // PRE_MOD_PVPOBSERVER
		IsEnableMoveToObserver = false;

	if(m_cCurrentPlayerNum == m_cMaxPlayerNum)
		IsEnableMoveToPlay = false;

	if(m_cMyTeam == TeamA) 
	{
		if(m_cMaxPlayerNum/2 == m_cCurrentTeamNum_B)
			IsEnableTeamChange = false;
	}
	else if(m_cMyTeam == TeamB)
	{
		if(m_cMaxPlayerNum/2 == m_cCurrentTeamNum_A)
			IsEnableTeamChange = false;
	}       

	if(GetEventRoom() > 0)
	{
		IsEnableReady = false;
		IsEnableMoveToObserver = false;
		IsEnableMoveToPlay = false;
	}

	// Show
	m_pStartButton->Show( bIsMaster || IsPlayingRoom() );
	m_pReadyButton->Show( false == bIsMaster && false == bIsReady && !IsPlayingRoom() );
	m_pReadyCancelButton->Show( false == bIsMaster && true == bIsReady && !IsPlayingRoom() );
	m_pRoomEditButton->Show( bIsMaster );
	m_pChangeMasterButton->Show( bIsMaster );
	m_pKick->Show( bIsMaster );
	m_pPermanenceKick->Show( bIsMaster );
	m_pMoveToPlay->Show(IsPlayerObserver);
	m_pMoveToObserver->Show(!IsPlayerObserver && false == bIsMaster && !IsGroupCaptain(m_uiMyUserState) );

	// Enable
	m_pReadyButton->Enable(IsEnableReady);
	m_pReadyCancelButton->Enable(IsEnableReady);
	m_pStartButton->Enable( IsEnableStartGame );
	m_pCancelButton->Enable(IsEnableCancel);
	m_pTeamChangeButton->Enable(IsEnableTeamChange);
	m_pChangeMasterButton->Enable(IsEnableChangeMasterButton);
#ifdef PRE_MOD_PVP_ROOM_CREATE
	m_pRoomEditButton->Enable(IsEnableEdit);
#endif

// #64379 : 게임참여유저가 1명의 경우 관전하기를 비활성화.
#ifdef PRE_MOD_PVPOBSERVER 
	if( m_cCurrentPlayerNum < 2 || m_bEventRoom || !m_bAllowObserver )
		IsEnableMoveToObserver = false;		
#endif // PRE_MOD_PVPOBSERVER
	m_pMoveToObserver->Enable(IsEnableMoveToObserver);

#ifdef PRE_MOD_PVPOBSERVER	
	CDnPVPLobbyChatTabDlg * pChatTabDlg = GetInterface().GetPVPLobbyChatTabDlg();
	if( pChatTabDlg )
		m_bEnterObserver = pChatTabDlg->IsObserverMode();

	//m_pMoveToPlay->Enable( ( !bGameMaster && ( m_bEventRoom && m_bEnterObserver ) ? false : IsEnableMoveToPlay) );
	m_pMoveToPlay->Enable( ( m_bEventRoom && m_bEnterObserver ) ? false : IsEnableMoveToPlay );
#else
	m_pMoveToPlay->Enable(IsEnableMoveToPlay);
#endif // PRE_MOD_PVPOBSERVER	

	m_pKick->Enable(IsEnableKickButton);
	m_pPermanenceKick->Enable(IsEnableKickButton);
	m_pChangeGroupCaptain->Show( IsGroupCaptain(m_uiMyUserState) );

#if defined( PRE_ADD_REVENGE )
	CDnRevengeTask * pRevengeTask = (CDnRevengeTask *)CTaskManager::GetInstance().GetTask( "RevengeTask" );
	if( true == bIsMaster && NULL != pRevengeTask )
	{
		if( 0 == pRevengeTask->GetMyRevengeUser() )
			m_pRevengeButton->Enable( false );
		else
			m_pRevengeButton->Enable( true );
	}
	m_pRevengeButton->Show( bIsMaster );
#endif	// #if defined( PRE_ADD_REVENGE )
}

void CDnPVPGameRoomDlg::Process( float fElapsedTime )
{
	CEtUIDialog::Process( fElapsedTime );

	if( !IsShow() )
		return;

	SetButtonStates();
	ProcessKickPlayer( fElapsedTime );
}

void CDnPVPGameRoomDlg::Show( bool bShow ) 
{ 
	if( m_bShow == bShow )
		return;

	CEtUIDialog::Show( bShow );

	if( bShow )
	{
		CDnChatTabDlg *pChatDialog = (CDnChatTabDlg*)GetInterface().GetPVPLobbyChatTabDlg();
		if(pChatDialog)
			pChatDialog->ShowEx(true);

		if( !GetInterface().IsEmptySlot( CDnMainMenuDlg::INVENTORY_DIALOG ) )
		{
			WCHAR wszTemp[256];
			swprintf_s( wszTemp, _countof(wszTemp), GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 1925 ) );
			GetInterface().AddChatMessage( CHATTYPE_SYSTEM, L"", wszTemp, false );
		}
	}
	else
	{
#ifdef PRE_MOD_PVPOBSERVER
		m_bEnterObserver = false;
#endif // PRE_MOD_PVPOBSERVER
	}

#ifdef PRE_ADD_PVP_HELP_MESSAGE
	for(int i = 0 ; i < E_PVP_RADIOBUTTON_MAX ; ++i)
	{
		if(m_pRoomInfoButton[i])
		{
			m_pRoomInfoButton[i]->SetChecked(false);
			if(i == m_nRadioButtonIndex)
				m_pRoomInfoButton[i]->SetChecked(true);
		}
	}
#endif // PRE_ADD_PVP_HELP_MESSAGE

}

void CDnPVPGameRoomDlg::PlaySound ( byte cType)
{
	if( m_nBeepSound!= -1 )
		CEtSoundEngine::GetInstance().PlaySound( "2D", m_nBeepSound, false );
}

bool CDnPVPGameRoomDlg::IsAllReady( )
{
	BYTE    cReadyPlayer = 0;
	
	for(int iTeamNum = 0 ; iTeamNum < MAXTeamNUM ; iTeamNum++)
	{
		for(int iPlayerNum = 0 ; iPlayerNum < m_cMaxPlayerNum/2 ; iPlayerNum++)
		{
			if(m_sPlayerSlot[iTeamNum][iPlayerNum].uiUserState) // 한번 더 걸어줍니다
			{
				if (IsReady(m_sPlayerSlot[iTeamNum][iPlayerNum].uiUserState))
				{
					cReadyPlayer++;
				}
			}
		}
	}

	if( cReadyPlayer == (GetRoomPlayerNumber()-1/*방장제외*/) && (cReadyPlayer!=0) && cReadyPlayer == m_cMaxPlayerNum-1) 
		return true;


	return false;
}


bool CDnPVPGameRoomDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsShow() )
		return false;

	switch( uMsg )
	{
	case WM_KEYDOWN:
	case WM_MOUSEMOVE :
		{

			if(IsMaster( m_uiMyUserState ))
			{
				if(!IsAllReady())
				{
					m_fElapsedKickTime = 0.0f;
					m_nWaringNum = 0;
				}
			}
			else if( !IsMaster( m_uiMyUserState ) )
			{
				m_fElapsedKickTime = 0.0f;
				m_nWaringNum = 0;
				//return true;
			}

		}
		break;
	case WM_LBUTTONDOWN:
		{
			if( m_pPVPMakeRoomDlg->IsShow() )
				break;

			POINT MousePoint;
			float fMouseX, fMouseY;

			MousePoint.x = short( LOWORD( lParam ) );
			MousePoint.y = short( HIWORD( lParam ) );
			PointToFloat( MousePoint, fMouseX, fMouseY );

			UpdateSelectBar( fMouseX, fMouseY );

			if( IsMouseInDlg() ) {
				if( m_pPVPPopupDlg->IsShow() ) {
					SUICoord uiCoord;
					m_pPVPPopupDlg->GetDlgCoord( uiCoord );
					fMouseX = MousePoint.x / GetScreenWidth();
					fMouseY = MousePoint.y / GetScreenHeight();
					if( !uiCoord.IsInside( fMouseX, fMouseY ) ) {
						ShowChildDialog( m_pPVPPopupDlg, false );
						return true;
					}
				}
			}
		}
		break;
	case WM_RBUTTONDOWN:
		{
			POINT MousePoint;
			MousePoint.x = short( LOWORD( lParam ) );
			MousePoint.y = short( HIWORD( lParam ) );

			float fMouseX, fMouseY;
			PointToFloat( MousePoint, fMouseX, fMouseY );

			bool bIsInsideItem = false;
		
			for( int i = 0 ; i < PvP_TeamUserSlot ; ++i ) 
			{
				for(int j=0 ;j<MAXTeamNUM ; j++ )
				{
					if( !m_sPlayerSlot[j][i].emSlotState == InPlayer) continue; // 슬롯에 사람이 있어야 됩니다. !
					SUICoord uiCoord;
					m_sPlayerSlot[j][i].pSelect->GetUICoord( uiCoord );

					if( uiCoord.IsInside( fMouseX, fMouseY ) ) 
					{
						m_pPVPPopupDlg->SetTargetName(m_sPlayerSlot[j][i].wszPlayerName);
						bIsInsideItem = true;
						break;
					}
				}
			}

			if( bIsInsideItem ) {

				fMouseX = MousePoint.x / GetScreenWidth();
				fMouseY = MousePoint.y / GetScreenHeight();

				SUICoord sDlgCoord;
				static float fyBGap(16.f /DEFAULT_UI_SCREEN_WIDTH);   // 약간 오른쪽에 위치해야 보기가 좋다..


				ShowChildDialog( m_pPVPPopupDlg, false );
				m_pPVPPopupDlg->SetPosition( fMouseX+fyBGap, fMouseY );

				m_pPVPPopupDlg->GetDlgCoord( sDlgCoord );
				if( (sDlgCoord.Right()) > GetScreenWidthRatio() )
					sDlgCoord.fX -= (sDlgCoord.Right() - GetScreenWidthRatio()); //  화면 삐져나오면 밀어주기.
				m_pPVPPopupDlg->SetDlgCoord( sDlgCoord );
				ShowChildDialog( m_pPVPPopupDlg, true );
				GetInterface().GetPVPLobbyChatTabDlg()->GetPVPObserverDlg()->ShowPVPPopUpDlg(false);
			}
		}
		break;

	}

	return CEtUIDialog::MsgProc( hWnd, uMsg, wParam, lParam );
}

void CDnPVPGameRoomDlg::UpdateSelectBar( float fX, float fY )
{

	bool bIsShowSelectBar = false;

	if( !m_pKick || !m_pPermanenceKick || !m_pChangeMasterButton) return;
	if( IsMouseInButton( fX, fY ) ) return;

	if( m_pKick->IsShow() ) m_pKick->Enable(false);
	if( m_pPermanenceKick->IsShow() ) m_pPermanenceKick->Enable(false);
	if( m_pChangeMasterButton->IsShow() ) m_pChangeMasterButton->Enable(false);
	if( m_pChangeGroupCaptain->IsShow() ) m_pChangeGroupCaptain->Enable(false);

	int iTeamNum = 0;
	int iPlayerNum = 0;
	for(iTeamNum = 0 ; iTeamNum < MAXTeamNUM ; iTeamNum++)
	{
		for(iPlayerNum = 0 ; iPlayerNum < PvP_TeamUserSlot ; iPlayerNum++)
		{
			SUICoord uiBaseCoord;
			m_sPlayerSlot[iTeamNum][iPlayerNum].pSelect->GetUICoord(uiBaseCoord);
			if( uiBaseCoord.IsInside( fX, fY ) && m_sPlayerSlot[iTeamNum][iPlayerNum].emSlotState == InPlayer )
			{
				m_sPlayerSlot[iTeamNum][iPlayerNum].pSelect->Show( true );
				if( !m_sPlayerSlot[iTeamNum][iPlayerNum].pStateUI[Master]->IsShow() ) 
				{
					m_pKick->Enable(true);
					m_pPermanenceKick->Enable(true);
					m_pChangeMasterButton->Enable(true);
				}
				if( iTeamNum == m_cMyTeam && !m_sPlayerSlot[iTeamNum][iPlayerNum].pStateUI[GroupCaptain]->IsShow() && !m_sPlayerSlot[iTeamNum][iPlayerNum].pStateUI[MasterAndGroupCaptain]->IsShow() )
					m_pChangeGroupCaptain->Enable(true);
			}
			else
			{
				m_sPlayerSlot[iTeamNum][iPlayerNum].pSelect->Show( false );

			}
			if(m_sPlayerSlot[iTeamNum][iPlayerNum].pSelect->IsShow())
			{
				bIsShowSelectBar = true;
			}
		}
	}

	if(!bIsShowSelectBar)
	{
		m_pKick->Enable(false);
		m_pPermanenceKick->Enable(false);
		m_pChangeMasterButton->Enable(false);
		m_pChangeGroupCaptain->Enable(false);
	}

	CEtUIListBoxEx* pUI = GetInterface().GetPVPLobbyChatTabDlg()->GetObserverListBox();
	GetInterface().GetPVPLobbyChatTabDlg()->SetSelectedListBox(false);
	pUI->SetRenderSelectBar(false);
}

bool CDnPVPGameRoomDlg::IsMouseInButton( float fX, float fY )
{
	SUICoord uiCoord;
	if( m_pKick ) {
		m_pKick->GetUICoord(uiCoord);
		if( uiCoord.IsInside( fX, fY ) ) return true;
	}

	if( m_pPermanenceKick ) {
		m_pPermanenceKick->GetUICoord(uiCoord);
		if( uiCoord.IsInside( fX, fY ) ) return true;
	}

	if( m_pChangeMasterButton ) {
		m_pChangeMasterButton->GetUICoord(uiCoord);
		if( uiCoord.IsInside( fX, fY) ) return true;
	}

	if( m_pReadyCancelButton ) {
		m_pReadyCancelButton->GetUICoord(uiCoord);
		if( uiCoord.IsInside( fX, fY ) ) return true;
	}

	if( m_pReadyCancelButton ) {
		m_pReadyCancelButton->GetUICoord(uiCoord);
		if( uiCoord.IsInside( fX, fY ) ) return true;
	}

	if( m_pCancelButton ) {
		m_pCancelButton->GetUICoord(uiCoord);
		if( uiCoord.IsInside( fX, fY ) ) return true;
	}

	if( m_pRoomEditButton ) {
		m_pRoomEditButton->GetUICoord(uiCoord);
		if( uiCoord.IsInside( fX, fY ) ) return true;
	}
	if( m_pChangeGroupCaptain )
	{
		m_pChangeGroupCaptain->GetUICoord(uiCoord);
		if( uiCoord.IsInside( fX, fY ) ) return true;
	}
	return false;
}


int CDnPVPGameRoomDlg::GetMaxPlayerLevel()
{
	int Temp=0;

	for(int iTeamNum = 0 ; iTeamNum < MAXTeamNUM ; iTeamNum++)
	{
		for(int iPlayerNum = 0 ; iPlayerNum < PvP_TeamUserSlot ; iPlayerNum++)
		{
			if(Temp< (int) m_sPlayerSlot[iTeamNum][iPlayerNum].cLevel)
			{
				Temp = m_sPlayerSlot[iTeamNum][iPlayerNum].cLevel;
			}
		}
	}

	return Temp;
}

int CDnPVPGameRoomDlg::GetMinPlayerLevel()
{
	int Temp=100;

	for(int iTeamNum = 0 ; iTeamNum < MAXTeamNUM ; iTeamNum++)
	{
		for(int iPlayerNum = 0 ; iPlayerNum < PvP_TeamUserSlot ; iPlayerNum++)
		{
			if(Temp > (int) m_sPlayerSlot[iTeamNum][iPlayerNum].cLevel && (int) m_sPlayerSlot[iTeamNum][iPlayerNum].cLevel != 0)
			{
				Temp = m_sPlayerSlot[iTeamNum][iPlayerNum].cLevel;
			}
		}
	}

	return Temp;
}


int CDnPVPGameRoomDlg::GetRoomPlayerNumber()
{
	int userCount = 0;
	
	for(int iTeamNum = 0 ; iTeamNum < MAXTeamNUM ; iTeamNum++)
	{
		for(int iPlayerNum = 0 ; iPlayerNum < PvP_TeamUserSlot ; iPlayerNum++)
		{
			if(m_sPlayerSlot[iTeamNum][iPlayerNum].emSlotState == InPlayer)
			{
				userCount++;
			}
		}
	}
	return userCount;
}


void CDnPVPGameRoomDlg::ResetSelectButton()
{
	int iTeamNum = 0;
	int iPlayerNum = 0;
	for(iTeamNum = 0 ; iTeamNum < MAXTeamNUM ; iTeamNum++)
	{
		for(iPlayerNum = 0 ; iPlayerNum < PvP_TeamUserSlot ; iPlayerNum++)
		{
			m_sPlayerSlot[iTeamNum][iPlayerNum].pSelect->Show(false);
		}
	}
}

CDnPVPGameRoomDlg::sUserSlot CDnPVPGameRoomDlg::GetSlotDataFromSessionID(int nSessionID)
{
	int iTeamNum = 0;
	int iPlayerNum = 0;
	for(iTeamNum = 0 ; iTeamNum < MAXTeamNUM ; iTeamNum++)
	{
		for(iPlayerNum = 0 ; iPlayerNum < PvP_TeamUserSlot ; iPlayerNum++)
		{
			if(m_sPlayerSlot[iTeamNum][iPlayerNum].uiSessionID == nSessionID)
			{
				return m_sPlayerSlot[iTeamNum][iPlayerNum];
			}
		}
	}

	return m_sPlayerSlot[0][0];
}


void CDnPVPGameRoomDlg::SetIndividualMode(bool bTrue)
{
	m_bIndividualMode = bTrue;

	for(int i=0;i<8;i++)
	{
		if(m_pTeamUIBar[i])
			m_pTeamUIBar[i]->Show(!bTrue);

		if(m_pTeamChangeButton)
			m_pTeamChangeButton->Show(!bTrue);
	}

	for(int i=0;i<3;i++)
	{
		if(m_pIndividualUIBar[i])
			m_pIndividualUIBar[i]->Show(bTrue);
	}

	DNTableFileFormat* pSox = GetDNTable( CDnTableDB::TPVPGAMEMODE );
	if(pSox)
	{
		int nGameModeIndex = 0;
		nGameModeIndex = pSox->GetFieldFromLablePtr( m_nGameModeID , "GamemodeID" )->GetInteger();

		if(m_bRandomTeamMode)
		{
			m_pIndividualUIBar[2]->SetText(GetEtUIXML().GetUIString( CEtUIXML::idCategory1,120075));
		}
		else 
		{
			if(nGameModeIndex == PvPCommon::GameMode::PvP_IndividualRespawn)
				m_pIndividualUIBar[2]->SetText(GetEtUIXML().GetUIString( CEtUIXML::idCategory1,120056));
			if(nGameModeIndex == PvPCommon::GameMode::PvP_Zombie_Survival)
				m_pIndividualUIBar[2]->SetText(GetEtUIXML().GetUIString( CEtUIXML::idCategory1,120074));

#if defined( PRE_ADD_RACING_MODE )
			if(nGameModeIndex == PvPCommon::GameMode::PvP_Racing)
				m_pIndividualUIBar[2]->SetText(GetEtUIXML().GetUIString( CEtUIXML::idCategory1,1000023353));	// UISTRING : 콜로세움 레이스 모드
#endif	// #if defined( PRE_ADD_RACING_MODE )

#if defined( PRE_ADD_PVP_COMBOEXERCISE )
			if( nGameModeIndex == PvPCommon::GameMode::PvP_ComboExercise )
				m_pIndividualUIBar[2]->SetText(GetEtUIXML().GetUIString( CEtUIXML::idCategory1,7932));	// "콤보 연습"
#endif	// PRE_ADD_PVP_COMBOEXERCISE

		}
	}
}


void CDnPVPGameRoomDlg::BaseProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg )
{
	ProcessCommand( nCommand, bTriggeredByUser, pControl, uMsg );
}


void CDnPVPGameRoomDlg::ProcessKickPlayer( float fElapsedTime )
{
//#ifdef _WORK // 개발 환경에서 강퇴설정하지 않음.
//	return;
//#endif

	CDnPVPLobbyVillageTask *pPVPLobbyTask = (CDnPVPLobbyVillageTask *)CTaskManager::GetInstance().GetTask( "PVPLobbyTask" );
	if(pPVPLobbyTask)
	{
		if( pPVPLobbyTask->GetUserInfo().cAccountLevel > AccountLevel_New )
			return;
	}

	if( IsMaster( m_uiMyUserState ) ) //방장일때
	{
		if( IsAllReady() || GetRoomPlayerNumber() >= 1) 
			m_fElapsedKickTime += fElapsedTime;
		else
			m_fElapsedKickTime = 0.0f;		


		if( m_fElapsedKickTime > fMasterKickTime-30 && (m_nWaringNum == 0) ) // 30초전
		{
			WCHAR wsWarnningMSG[256];
			SecureZeroMemory(wsWarnningMSG,sizeof(wsWarnningMSG));

			if(!IsAllReady())
				wsprintf(wsWarnningMSG,GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 120060),30); // 이경우라면 키입력이 없다.
			else
				wsprintf(wsWarnningMSG,GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 120062),30); // 모두 레디중인데 시작을 안한다.

			GetInterface().AddChatMessage(CHATTYPE_SYSTEM ,L""  , wsWarnningMSG);
			m_nWaringNum = 1;

		}

		if( m_fElapsedKickTime > fMasterKickTime - 10 && (m_nWaringNum == 1) ) // 10초전
		{
			WCHAR wsWarnningMSG[256];
			SecureZeroMemory(wsWarnningMSG,sizeof(wsWarnningMSG));

			if(!IsAllReady())
				wsprintf(wsWarnningMSG,GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 120060),10); // 방장 키입력이 없다.
			else
				wsprintf(wsWarnningMSG,GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 120062),10);     // 모두 레디중인데 시작을 하지 않는 경우.

			GetInterface().AddChatMessage(CHATTYPE_SYSTEM ,L""  , wsWarnningMSG);
			m_nWaringNum = 2;
		}

		if( m_fElapsedKickTime > fMasterKickTime && (m_nWaringNum == 2) )
		{
			GetInterface().GetPVPLobbyChatTabDlg()->RemoveObserver(0,true); 

			m_pPVPMakeRoomDlg->SetEditState(false);
			m_pPVPMakeRoomDlg->Show(false);

			SendPvPLeaveRoom(); // 이부분에 창을 닫는부분 추가
			GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 120064) , MB_OK);//입력없어서 강퇴
			m_nWaringNum = 0;
			m_fElapsedKickTime =0.0f;
		}

	}else
	{
		if(!IsReady( m_uiMyUserState ) && !(m_cMyTeam > 1) && !(m_cMyTeam < 0)) // m_cMyTeam  0 =A , 1 = B 
			m_fElapsedKickTime += fElapsedTime;
		else
			m_fElapsedKickTime = 0.0f;		

		if( m_fElapsedKickTime > fKickTime/2 && (m_nWaringNum == 0) )
		{
			WCHAR wsWarnningMSG[256];
			SecureZeroMemory(wsWarnningMSG,sizeof(wsWarnningMSG));
			wsprintf(wsWarnningMSG,GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 120060),30);
			GetInterface().AddChatMessage(CHATTYPE_SYSTEM ,L""  , wsWarnningMSG);
			m_nWaringNum = 1;

		}

		if( m_fElapsedKickTime > fKickTime - (fKickTime/6) && (m_nWaringNum == 1) )
		{
			WCHAR wsWarnningMSG[256];
			SecureZeroMemory(wsWarnningMSG,sizeof(wsWarnningMSG));
			wsprintf(wsWarnningMSG,GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 120060),10);
			GetInterface().AddChatMessage(CHATTYPE_SYSTEM ,L""  , wsWarnningMSG);
			m_nWaringNum = 2;
		}

		if( m_fElapsedKickTime > fKickTime && (m_nWaringNum == 2) )
		{
			GetInterface().GetPVPLobbyChatTabDlg()->RemoveObserver(0,true); // 
			SendPvPLeaveRoom();
			GetInterface().MessageBox( GetEtUIXML().GetUIString( CEtUIXML::idCategory1, 120064) , MB_OK);//입력없어서 강퇴
			m_nWaringNum = 0;
			m_fElapsedKickTime =0.0f;
		}
	}
}