#pragma once

#include "DnCustomDlg.h"
#include "DnBaseRoomDlg.h"

class CDnPVPRoomEditDlg;
class CDnGuildWarPopupDlg;

class CDnGuildWarRoomDlg : public CDnCustomDlg, public CDnBaseRoomDlg
{
public:
	CDnGuildWarRoomDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnGuildWarRoomDlg(void);

	enum eSlotLimit
	{
		GuildWar_MaxUserSlot = 32, 
		GuildWar_TeamUserSlot = 16, 

		GuildWar_TeamPerPartySlot = 4,
		GuildWar_PartyPerUserSlot = 4,
	};

	enum{
		ROOM_BOSS = 0x0001,
		TEAM_MASTER = 0x0010,
		GAME_READY = 0x0100,
		GAME_PLAY  = 0x1000
	};

	struct SUserIndex
	{
		int nTeam;
		int nParty;
		int nUser;
		int nIndex;	//0~15까지 인덱스

		SUserIndex() 
			: nTeam( 0 )
			, nParty( 0 )
			, nUser( 0 )
			, nIndex( -1 )
		{}
		SUserIndex( int nSelectCount )
		{
			Translate( nSelectCount );
		}

		void Translate( int nSelectCount )
		{
			nIndex = nSelectCount;

			nTeam = nSelectCount / GuildWar_TeamUserSlot;
			nParty = nSelectCount / GuildWar_TeamPerPartySlot - nTeam * GuildWar_TeamPerPartySlot;
			nUser = nSelectCount % GuildWar_PartyPerUserSlot;
		}
	};

	struct sUserSlot{
		CEtUIStatic * m_pSelectBar;
		CEtUIStatic * m_pOpenText;
		CEtUIStatic * m_pOpenImage;
		CEtUIStatic * m_pCloseText;
		CEtUIStatic * m_pCloseImage;
		CDnJobIconStatic * m_pClassIcon;
		CEtUIStatic * m_pNameText;
		CEtUIStatic * m_pBossIcon;
		CEtUIStatic * m_pMasterIcon;
		CEtUIStatic * m_pReadyIcon;
		CEtUIStatic * m_pGaimingIcon;
		CEtUITextureControl * m_pRankTexture;

		SlotState m_eSlotState;
		int  m_nJobID;
		BYTE m_cLevel;
		BYTE m_cPVPLevel;
		UINT m_uiUserState;
		UINT m_uiSessionID;
		bool m_bIsNeedUpdate;
		bool m_bShow;
		WCHAR m_wszPlayerName[NAMELENMAX];
		char m_cPosition;
		char m_cTeam;
#if defined( PRE_ADD_REVENGE )
		UINT uiRevengeUserID;
#endif	// #if defined( PRE_ADD_REVENGE )

		sUserSlot()
			: m_pSelectBar( NULL )
			, m_pOpenText( NULL )
			, m_pOpenImage( NULL )
			, m_pCloseText( NULL )
			, m_pCloseImage( NULL )
			, m_pClassIcon( NULL )
			, m_pNameText( NULL )
			, m_pBossIcon( NULL )
			, m_pMasterIcon( NULL )
			, m_pReadyIcon( NULL )
			, m_pGaimingIcon( NULL )
			, m_pRankTexture( NULL )
			, m_eSlotState( Closed )
			, m_nJobID( 0 )
			, m_cLevel( 0 )
			, m_cPVPLevel( 0 )
			, m_uiUserState( 0 )
			, m_uiSessionID( 0 )
			, m_bIsNeedUpdate( false )
			, m_bShow( false )
			, m_cPosition( 0 )
			, m_cTeam( 0 )
#if defined( PRE_ADD_REVENGE )
			, uiRevengeUserID(0)
#endif	// #if defined( PRE_ADD_REVENGE )
		{
			SecureZeroMemory( m_wszPlayerName, sizeof(m_wszPlayerName) );
		}

		void Show( bool bShow )
		{
			m_pSelectBar->Show( bShow );
			m_pOpenText->Show( bShow );
			m_pOpenImage->Show( bShow );
			m_pCloseText->Show( bShow );
			m_pCloseImage->Show( bShow );
			m_pClassIcon->Show( bShow );
			m_pNameText->Show( bShow );
			m_pBossIcon->Show( bShow );
			m_pMasterIcon->Show( bShow );
			m_pReadyIcon->Show( bShow );
			m_pGaimingIcon->Show( bShow );
			m_pRankTexture->Show( bShow );
		}

		void SetState();
		sUserSlot & operator = ( const sUserSlot & data )
		{
			m_eSlotState = data.m_eSlotState;
			m_nJobID = data.m_nJobID;
			m_cLevel = data.m_cLevel;
			m_cPVPLevel = data.m_cPVPLevel;
			m_uiUserState = data.m_uiUserState;
			m_uiSessionID = data.m_uiSessionID;
			m_bIsNeedUpdate = data.m_bIsNeedUpdate;
			m_bShow = data.m_bShow;
			m_cPosition = data.m_cPosition;
			m_cTeam = data.m_cTeam;

			if( SlotState::InPlayer == data.m_eSlotState )
				swprintf_s( m_wszPlayerName, NAMELENMAX, data.m_wszPlayerName );

			return *this;
		}
	};

	void Reset();
	void InitUserSlot();

protected:
	CEtUITextureControl *m_pMiniMap;
	EtTextureHandle m_hMiniMapImage; 
	CDnPVPRoomEditDlg * m_pPVPMakeRoomDlg;
	CDnGuildWarPopupDlg * m_pGuildWarPopupDlg;

	CEtUIStatic *m_pRoomStateStatic;
	CEtUIStatic *m_pUIGameMode;
	CEtUIStatic *m_pUIPlayerNum;
	CEtUIStatic *m_pMapName;
	CEtUIStatic *m_pRoomName;
	CEtUIStatic *m_pRoomIndexNum;
	CEtUIStatic *m_pWinCon;
	CEtUIStatic *m_pInGameJoin;
	CEtUIStatic *m_pNotInGameJoin;
	CEtUIStatic *m_pPasswordRoom;
	CEtUIStatic *m_pDropItem;
	CEtUIStatic *m_pNoDropItem;
	CEtUIStatic *m_pShowHp;
	CEtUIStatic *m_pHideHp;
	CEtUIStatic *m_pBlueOption;
	CEtUIStatic *m_pRedOption;
	CEtUIStatic *m_pRevision;
	CEtUIStatic *m_pNoRevision;

	CEtUIButton *m_pTeamChangeButton;
	CEtUIButton *m_pReadyButton;
	CEtUIButton *m_pSatrtButton;
	CEtUIButton *m_pCancelButton;
	CEtUIButton *m_pRoomEditButton;
	CEtUIButton *m_pReadyCancelButton;
	CEtUIButton *m_pUserMoveOK;
	CEtUIButton *m_pUserMove;

	CEtUIButton *m_pKick;
	CEtUIButton *m_pPermanenceKick;

	CEtUIButton *m_pMoveToObserver;
	CEtUIButton *m_pMoveToPlay;

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

	UINT    m_uiMapIndex;            // 맵인덱스
	UINT    m_nGameModeID;            // 게임모드
	UINT    m_nRoomIndex;            // 게임모드
	bool    m_IsBreakIntoFlag;        // 난입여부		
	WCHAR   m_wszRoomName[PvPCommon::TxtMax::RoomName];
	BYTE    m_cMaxPlayerNum;            // 몇명짜리 방인가?
	BYTE    m_cCurrentPlayerNum;        // 현재 인원
	bool	m_bDropItem;				// 드랍아이템 허용
	bool    m_bShowHp;					// HP 보여주기
	bool	m_bRevision;				// 보정 여부
	bool	m_bMoveParty;				// 파티원 이동
	int		m_nSelectUserCount;			// 현재 선택된 유저 수

#ifdef PRE_MOD_PVPOBSERVER
	bool	m_bAllowObserver;				// 관전허용.
	bool    m_bEnterObserver;	
	bool    m_bEventRoom;
#endif // PRE_MOD_PVPOBSERVER

	bool	m_bIsGuildWarSystem;		// 길드전 시스템으로 만들어진 방인가

	std::vector< std::pair<SUserIndex, sUserSlot> > m_vPrevSlot;
	SUserIndex m_FirstSelectUser;
	SUserIndex m_SecondSelectUser;

	BYTE    m_cCurrentTeamNum_A;
	BYTE    m_cCurrentTeamNum_B;

	byte	m_cMyTeam;					//내팀
	UINT	m_uiMyUserState;				//내상태
	UINT	m_uiRoomState;				//방상태
	byte    m_cMaxLevel;
	byte    m_cMinLevel;
	
	int m_nBeepSound;
	float m_fElapsedKickTime;
	int m_nWaringNum;

	sUserSlot m_pUserSlot[MAXTeamNUM][GuildWar_TeamPerPartySlot][GuildWar_PartyPerUserSlot];

protected:
	bool IsReadyRoom(){ return m_uiRoomState == PvPCommon::RoomState::None || m_uiRoomState == PvPCommon::RoomState::Password; }
	bool IsPasssWordRoom(){ return m_uiRoomState&PvPCommon::RoomState::Password?true:false; }
	bool IsSyncingRoom(){ return m_uiRoomState&PvPCommon::RoomState::Syncing?true:false; }
	bool IsStartingRoom(){ return m_uiRoomState&PvPCommon::RoomState::Starting?true:false; }
	bool IsPlayingRoom(){ return m_uiRoomState&PvPCommon::RoomState::Playing?true:false; }
	bool IsInGamePlayingRoom(){ return m_uiRoomState&PvPCommon::RoomState::Playing?true:false || m_uiRoomState&PvPCommon::RoomState::Starting?true:false || m_uiRoomState&PvPCommon::RoomState::Syncing ; }

	void SetButtonState();

	bool IsMouseInButton( float fX, float fY );
	void UpdateSelectBar( float fX, float fY );
	void UpdateSelectBarMoveMode( float fX, float fY );
	void * FindUserSlot( UINT uiSessionID );
	sUserSlot * FindUserSlot( SUserIndex & nIndex )	{ return &(m_pUserSlot[nIndex.nTeam][nIndex.nParty][nIndex.nUser]); }
	void SwapSlot( SUserIndex & sFirstIndex, SUserIndex & sSecondIndex );
	void SwapAllSlot();
	void SavePrevSlot();

	bool IsAllReady();
	int GetRoomPlayerNumber();

#ifdef PRE_ADD_PVP_HELP_MESSAGE
	void ChangeRoomInformation(int nTabID);
#endif

public :
	void RevertSlot();
	void RefreshSlot( SCPvPMemberIndex * pPacket );

	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void Process( float fElapsedTime );
	virtual void Show( bool bShow );

	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

public :	/// CDnBaseRoom 가상 함수들
	virtual void RoomSetting( PvPCommon::RoomInfo* pInfo ,bool IsModified);
	virtual void InsertPlayer(int team, UINT uiUserState, UINT uiSessionID, int iJobID, BYTE  cLevel, BYTE cPVPLevel, const WCHAR * szPlayerName, char cPosition = 0 );
#ifdef PRE_MOD_PVPOBSERVER
	virtual void InsertObserver( UINT uiUserState , UINT uiSessionID );
#endif PRE_MOD_PVPOBSERVER
	virtual void SetRoomState( UINT uiRoomState );
	virtual void RemovePlayer( UINT uiSessionID, PvPCommon::LeaveType::eLeaveType eType = PvPCommon::LeaveType::Normal );
	virtual void UpdateSlot( bool ForceAll );
	virtual void SetUserState( UINT uiSessionID , UINT  uiUserState , bool IsOuterCall = false );
	virtual int GetCurMapIndex();
	virtual void UpdatePvPLevel( SCPVP_LEVEL* pPacket );
	virtual void ShowPVPPopUpDlg(bool bShow);
	virtual void PlaySound ( byte cType);

	virtual void ChangeTeam( UINT uiSessionID , int cTeam, char cTeamSlotIndex );

	virtual void EnableKickButton(bool bTrue);
	virtual void ResetSelectButton();

	virtual void BaseProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg );

	virtual UINT GetLocalUserState() const { return m_uiMyUserState; }

#ifdef PRE_MOD_PVPOBSERVER	
	virtual bool IsEventRoom() const {
		return m_bEventRoom;
	}
#endif // PRE_MOD_PVPOBSERVER
};

