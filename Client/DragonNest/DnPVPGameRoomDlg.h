#pragma once
#include "EtUIDialog.h"
#include "DnCommonDef.h"
#include "DnPVPRoomEditDlg.h"
#include "DnPVPPopupDlg.h"
#include "DnBaseRoomDlg.h"

const float fKickTime = 60;
const float fMasterKickTime = 90; // 방장은 너무 쉽게 강퇴 되지 않습니다

class CDnPVPRevengeMessageBoxDlg;

class CDnPVPGameRoomDlg : public CEtUIDialog, public CDnBaseRoomDlg
{
public:					 
	CDnPVPGameRoomDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnPVPGameRoomDlg(void);

	enum ChildWinID
	{
		ROOMMAKE_DIALOG,		
	};

	enum eSlotLimit
	{
		PvP_MaxUserSlot = 16, // 
		PvP_TeamUserSlot = 8, // PvPCommon::Common::MaxTeamPVPPlayer 가 유동적으로 변경함에따라 변경합니다.
	};

	enum UserState
	{
		Master = 0,
		Ready = 1,
		Playing = 2,
		GroupCaptain = 3,
		MasterAndGroupCaptain = 4,
		State_Max,
	};

	enum eModeNameString
	{
		Mode_IndividualRespawn = 120056,
		Mode_Captain = 121074,
		Mode_Respawn = 120025,
		Mode_Round   = 120024,
	};

	struct sUserSlot{
		SlotState	emSlotState;
		int iJobID;
		BYTE cLevel;
		BYTE cPVPLevel;

		WCHAR wszPlayerName[NAMELENMAX];
		UINT uiSessionID;
		bool isNeedUpdate;
		UINT uiUserState;

		CEtUITextureControl *pPVPIcon;
		CEtUIStatic *pLevelUI;
		CEtUIStatic *pClassUI;
		CEtUIStatic *pNameUI;
		CEtUIStatic *pStateUI[State_Max];
		CEtUIStatic *pOpen;
		CEtUIStatic *pOpenText;
		CEtUIStatic *pClose;
		CEtUIStatic *pCloseText;
		CEtUIStatic *pSelect;

		sUserSlot()
			:emSlotState(Closed)
			,iJobID(0)
			,cLevel(0)
			,cPVPLevel(0)
			,isNeedUpdate(true)
			,uiUserState(0)
			,uiSessionID(0)
		{			
			pLevelUI = NULL;
			pClassUI = NULL;
			pNameUI = NULL;
			pOpen = NULL;
			pOpenText = NULL;
			pClose = NULL;
			pCloseText = NULL;
			pPVPIcon = NULL;
			pSelect = NULL;
			SecureZeroMemory(wszPlayerName,sizeof(wszPlayerName));

			for(int i=0;i< State_Max;i++)
				pStateUI[i] = NULL;
		}
	};
	
protected:
	UINT    m_uiMapIndex;          
	UINT    m_nGameModeID;            
	UINT    m_nRoomIndex;            
  	UINT	m_uiMyUserState;				
	UINT	m_uiRoomState;		
	
	WCHAR   m_wszRoomName[PvPCommon::TxtMax::RoomName];
	WCHAR   m_wszOriginalRoomName[PvPCommon::TxtMax::RoomName];

	BYTE    m_cMaxPlayerNum;           
	BYTE    m_cCurrentPlayerNum;        
	BYTE    m_cCurrentTeamNum_A;
	BYTE    m_cCurrentTeamNum_B;
	byte	m_cMyTeam;	
	byte    m_nMaxLevel;
	byte    m_nMinLevel;
		
	bool    m_IsBreakIntoFlag;    
	bool    m_bIndividualMode;
	bool	m_bDropItem;			
	bool    m_bShowHp;                  
	bool    m_bRandomTeamMode;
	bool	m_bRevision;
	bool    m_bRandomOrder;
#ifdef PRE_MOD_PVPOBSERVER
	bool    m_bEnterObserver;
	bool    m_bAllowObserver;
	bool    m_bEventRoom;
#endif // PRE_MOD_PVPOBSERVER

#ifdef PRE_ADD_PVP_COMBOEXERCISE
	bool    m_bComboExeMode;
#endif // PRE_ADD_PVP_COMBOEXERCISE

	int m_nBeepSound;
	int m_nWaringNum;
	float m_fElapsedKickTime;

	int m_nEventRoomIndex;

	CDnPVPRoomEditDlg *m_pPVPMakeRoomDlg;
	
	EtTextureHandle m_hMiniMapImage; //지도 옆에 표시될 이미지

	sUserSlot m_sPlayerSlot[MAXTeamNUM][PvP_TeamUserSlot];
	CDnPVPPopupDlg *m_pPVPPopupDlg;

	CEtUIButton *m_pTeamChangeButton;
	CEtUIButton *m_pReadyButton;
	CEtUIButton *m_pStartButton;
	CEtUIButton *m_pCancelButton;
	CEtUIButton *m_pRoomEditButton;
	CEtUIButton *m_pReadyCancelButton;
	CEtUIButton *m_pChangeMasterButton;

	CEtUIButton *m_pChangeGroupCaptain;
	CEtUIStatic *m_pStaticNameBreakInto;
	CEtUIStatic *m_pStaticNameUseItem;

	CEtUIStatic *m_pUIGameMode;
	CEtUIStatic *m_pUIPlayerNum;
	CEtUITextureControl *m_pMiniMap;
	CEtUIStatic *m_pMapName;
	CEtUIStatic *m_pRoomStateStatic;
	CEtUIStatic *m_pInGameJoin;
	CEtUIStatic *m_pNotInGameJoin;
	CEtUIStatic *m_pDropItem;
	CEtUIStatic *m_pNoDropItem;
	CEtUIStatic *m_pRevision;
	CEtUIStatic *m_pNoRevision;
	CEtUIStatic *m_pShowHp;
	CEtUIStatic *m_pHideHp;
	CEtUIStatic *m_pWinCon;
	CEtUIStatic *m_pRoomName;
	CEtUIStatic *m_pRoomIndexNum;
	CEtUIStatic *m_pPasswordRoom;
	CEtUIStatic *m_pIndividualUIBar[3];
	CEtUIStatic *m_pTeamUIBar[8];
	CEtUIButton *m_pMoveToObserver;
	CEtUIButton *m_pMoveToPlay;
	CEtUIButton *m_pKick;
	CEtUIButton *m_pPermanenceKick;

#if defined( PRE_ADD_REVENGE )
	CEtUIButton *m_pRevengeButton;
#endif	// #if defined( PRE_ADD_REVENGE )

#ifdef PRE_ADD_PVP_HELP_MESSAGE
	enum{
		E_PVP_RADIOBUTTON_ROOM_INFO = 0,
		E_PVP_RADIOBUTTON_MODE_INFO,
		
		E_PVP_RADIOBUTTON_MAX
	};
	CEtUIRadioButton *m_pRoomInfoButton[E_PVP_RADIOBUTTON_MAX];
	
	int m_nRadioButtonIndex;
	
	CEtUITextBox	 *m_pStaticModeInfo;
	CEtUIStatic		 *m_pStaticInfoBoard[2];

	CEtUIStatic		 *m_pStaticMapBoard[3];
	CEtUIStatic		 *m_pStaticTextMapInfo[8];
	CEtUIStatic		 *m_pStaticMapLine[5];
#endif

protected:

	virtual void UpdateSlot( UITeam Team , int SlotIndex  );

	void InitializeUserSlot();
	void InitializeHidableStatic();
	void InitializeCommonUI();

	void *FindUserSlot( UINT uiSessionID );
	void SetPlayerState( int iteam , int Player , UserState emUserState , bool IsOff = false );
	void CopySlot( int iSourceTeamIndex , int iSourcePlayerIndex , int iDestTeamIndex , int iDestPlayerIndex );
	void RemoveSlot( int iTeamIndex , int iPlayerIndex , SlotState emChanheState );
	void ClearSlot( int iTeamIndex , int iPlayerIndex , SlotState emChanheState );
	void SetUIRoomState( UINT uiRoomState );
	void SetButtonStates();
	
	void UpdateSelectBar( float fX, float fY );
	void ProcessKickPlayer( float fElapsedTime );

	bool IsAllReady( );
	bool IsReadyRoom(){ return m_uiRoomState == PvPCommon::RoomState::None || m_uiRoomState == PvPCommon::RoomState::Password; }
	bool IsPasssWordRoom(){ return m_uiRoomState&PvPCommon::RoomState::Password?true:false; }
	bool IsSyncingRoom(){ return m_uiRoomState&PvPCommon::RoomState::Syncing?true:false; }
	bool IsStartingRoom(){ return m_uiRoomState&PvPCommon::RoomState::Starting?true:false; }
	bool IsPlayingRoom(){ return m_uiRoomState&PvPCommon::RoomState::Playing?true:false; }
	bool IsInGamePlayingRoom(){ return m_uiRoomState&PvPCommon::RoomState::Playing?true:false || m_uiRoomState&PvPCommon::RoomState::Starting?true:false || m_uiRoomState&PvPCommon::RoomState::Syncing ; }
	bool IsMouseInButton( float fX, float fY );
	bool FindUserSlot( UINT uiSessionID , int & pSotIndex , int & pPlayerIndex );

#ifdef PRE_ADD_PVP_HELP_MESSAGE
	void ChangeRoomInformation(int nTabID);
#endif

public:

	void Reset();
	void SetIndividualMode(bool bTrue);

	int GetMaxPlayerLevel();
	int GetMinPlayerLevel();
	int GetRoomPlayerNumber();

	CDnPVPGameRoomDlg::sUserSlot GetSlotDataFromSessionID(int nSessionID);
	CDnPVPPopupDlg* GetPVPPopupDlg(){return m_pPVPPopupDlg;}

	void SetEventRoom(int nRoomID){m_nEventRoomIndex = nRoomID;}
	int GetEventRoom(){return m_nEventRoomIndex;}

#ifdef PRE_MOD_PVPOBSERVER	
	virtual bool IsEventRoom() const {
		return m_bEventRoom;
	}
#endif // PRE_MOD_PVPOBSERVER

#ifdef PRE_ADD_PVP_COMBOEXERCISE
	void SetComboExerciseMode( bool bMode){
		m_bComboExeMode = bMode;
	}
#endif // PRE_ADD_PVP_COMBOEXERCISE


public:
	virtual int GetCurMapIndex() { return m_uiMapIndex; }
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual void RoomSetting( PvPCommon::RoomInfo* pInfo ,bool IsModified);
	virtual void UpdateSlot( bool ForceAll );
	virtual void InsertPlayer(int team  , UINT uiUserState , UINT    uiSessionID , int iJobID , BYTE  cLevel , BYTE  cPVPLevel , const WCHAR * szPlayerName, char cPosition = 0 );
#ifdef PRE_MOD_PVPOBSERVER
	virtual void InsertObserver( UINT uiUserState, UINT uiSessionID );
#endif // PRE_MOD_PVPOBSERVER
	virtual void RemovePlayer( UINT uiSessionID, PvPCommon::LeaveType::eLeaveType eType = PvPCommon::LeaveType::Normal );
	virtual void SetUserState( UINT uiSessionID , UINT  uiUserState , bool IsOuterCall = false );
	virtual void SetRoomState( UINT uiRoomState );
	virtual void PlaySound ( byte cType);
	virtual void UpdatePvPLevel( SCPVP_LEVEL* pPacket );
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void Process( float fElapsedTime );
	virtual void Show( bool bShow );
	virtual void ShowPVPPopUpDlg(bool bShow){ShowChildDialog(m_pPVPPopupDlg,bShow);}
	virtual void EnableChangeMasterButton(bool bTrue){m_pChangeMasterButton->Enable(bTrue);}
	virtual void BaseProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg );
	virtual void ChangeTeam( UINT uiSessionID , int cTeam, char cTeamSlotIndex );
	virtual void EnableKickButton(bool bTrue){m_pKick->Enable(bTrue); m_pPermanenceKick->Enable(bTrue);}; // 다른클래스에서 킥버튼을 활성화 시키기위해서 추가했습니다.
	virtual void ResetSelectButton(); // 선택 탭 초기화

	virtual UINT GetLocalUserState() const { return m_uiMyUserState; }

};