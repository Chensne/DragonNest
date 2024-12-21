#pragma once

#include "DnCustomDlg.h"
#include "DnBaseRoomDlg.h"
#include "DnUIDefine.h"

#ifdef PRE_ADD_PVP_TOURNAMENT

using namespace PvPTournamentUIDef;

class CDnPVPTournamentDataMgr;
class CDnPVPTournamentRoomMatchListDlg;
class CDnPVPRoomEditDlg;
class CDnPVPTournamentRoomDlg : public CDnCustomDlg, public CDnBaseRoomDlg
{
public:
	CDnPVPTournamentRoomDlg(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL, bool bAutoCursor = false);
	virtual ~CDnPVPTournamentRoomDlg(void);

	virtual void InitialUpdate();
	virtual void Initialize(bool bShow);
	virtual void ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0);
	virtual void Process(float fElapsedTime);
	virtual void Show(bool bShow);
	virtual void Render(float fElapsedTime);

	void Reset();
#ifdef PRE_MOD_PVPOBSERVER
	virtual bool IsEventRoom() const { return m_bEventRoom; }
	virtual void InsertObserver(UINT uiUserState, UINT uiSessionID);
#endif // PRE_MOD_PVPOBSERVER
	virtual void RoomSetting(PvPCommon::RoomInfo* pInfo, bool IsModified);
	virtual void InsertPlayer(int team, UINT uiUserState, UINT uiSessionID, int iJobID, BYTE cLevel, BYTE cPVPLevel, const WCHAR * szPlayerName, char cPosition = 0);
	virtual void SetRoomState(UINT roomState);
	virtual void SetUserState(UINT uiSessionID, UINT uiUserState, bool IsOuterCall = false);
	virtual void UpdateSlot(bool bForceAll);
	virtual void RemovePlayer(UINT uiSessionID, PvPCommon::LeaveType::eLeaveType eType);
	virtual void ChangeTeam(UINT uiSessionID, int cTeam, char cTeamSlotIndex);
	virtual void PlaySound(byte cType);
#ifdef PRE_PVP_GAMBLEROOM
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
#endif // PRE_PVP_GAMBLEROOM

	CDnPVPTournamentDataMgr* GetDataMgr() const;
	void SetMatchListDlg(CDnPVPTournamentRoomMatchListDlg* pDlg);
	void OnSwapPosition(const int& srcPos, const int& destPos);

	void HandlePVPTournamentError(ePVPTournamentError error, bool bShowCode = false);
	bool IsEnableMoveToPlay() const;

private:
	enum ePVPTnmtUISwitch
	{
		eSwitchOn,
		eSwitchOff,
		eSwitchMax
	};

	void SetRoomUI();
	void SetRoomStateUI();

	void AddUserListBox(const SMatchUserInfo& info);
	void RemoveUserListBox(const SMatchUserInfo& info);

	void UpdateUserListBox();
	void UpdateFunctionButtons();
	void UpdateButtonsByMasterOrNot(const CDnPVPTournamentDataMgr& mgr, bool bMaster);
	void UpdateButtonsByState(CDnPVPTournamentDataMgr& mgr, const UINT& userState);

	void _SetContentOffset();
	void ResetHoldSwapPosition(CDnPVPTournamentDataMgr& mgr);
	bool IsEnableObserver() const;

	int GetMaxPlayerLevel() const;
	int GetMinPlayerLevel() const;

	void ToggleEditPositionButton(bool bEnableEditPosition);
	void ToggleReadyButton(bool bEnableReady);
	void ToggleObserverButton(bool bEnableObserver);

	void SendToServerKick(PvPCommon::BanType::eBanType banType);
	void SendToServerChangeMaster();

	// buttons
	CEtUIButton* m_pExit;
	CEtUIStatic* m_pTitle;
	CEtUIButton* m_pKick;
	CEtUIButton* m_pPermanentKick;
	CEtUIButton* m_pMoveToObserver;
	CEtUIButton* m_pMoveToPlay;
	CEtUIButton* m_pEditPosition;
	CEtUIButton* m_pEditPositionComplete;
	CEtUIButton* m_pChangeMaster;
	CEtUIButton* m_pStart;
	CEtUIButton* m_pReady[eSwitchMax];
	CEtUIButton* m_pEditRoom;

	// side infos
	CEtUIStatic* m_pRoomName;
	CEtUIStatic* m_pPasswordRoom;
	CEtUIStatic* m_pRoomIndexNum;
	CEtUIStatic* m_pMapName;
	CEtUIStatic* m_pUIPlayerNum;
	CEtUIStatic* m_pUIGameMode;
	CEtUIStatic* m_pRoomStateStatic;
	CEtUIStatic* m_pWinCondition;
	CEtUIStatic* m_pBreakIntoTitle;
	CEtUIStatic* m_pBreakIntoSwitch[eSwitchMax];
	CEtUIStatic* m_pDropItemTitle;
	CEtUIStatic* m_pDropItemSwitch[eSwitchMax];
	CEtUIStatic* m_pRevision[eSwitchMax];
	CEtUIStatic* m_pShowHp[eSwitchMax];

	CEtUITextureControl* m_pMiniMap;
	EtTextureHandle m_hMiniMapImage;

	CDnPVPTournamentRoomMatchListDlg* m_pMatchListDlg;
	CEtUIListBoxEx*	m_pMatchUserListBox;
	CEtUIListBox* m_pMatchListContainerListBox;
	CEtUIScrollBar* m_pScrollBar;

	CDnPVPRoomEditDlg* m_pPVPMakeRoomDlg;

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

#ifdef PRE_PVP_GAMBLEROOM
	CEtUIStatic* m_pStaticGamblePrice;
#endif // PRE_PVP_GAMBLEROOM

	void ChangeRoomInformation(int nTabID);
#endif

#ifdef PRE_MOD_PVPOBSERVER
	bool m_bEnterObserver;
	bool m_bAllowObserver;
	bool m_bEventRoom;
#endif

	int m_iScrollPos;
	float m_fOriContentDlgXPos;
	float m_fOriContentDlgYPos;

	int m_nBeepSound;
};

#endif	// PRE_ADD_PVP_TOURNAMENT