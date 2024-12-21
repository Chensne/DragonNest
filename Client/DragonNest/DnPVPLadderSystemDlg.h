#pragma once
#include "DnCustomDlg.h"
#include "DnPVPLadderPreviewDlg.h"
#include "DnPVPLadderTeamGameDlg.h"
#include "DnPVPLadderObserverInfoDlg.h"

class CDnPVPLadderSystemDlg : public CDnCustomDlg, public CEtUICallback
{
public:					 
	
	CDnPVPLadderSystemDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	
	virtual ~CDnPVPLadderSystemDlg(void);
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void Process( float fElapsedTime );
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual void Show( bool bShow );

	enum RoomListRefreshTime
	{
		Default = 10,
	};

	struct sLadderRoomList
	{
#ifdef PRE_FIX_PVP_LOBBY_RENEW
		CDnJobIconStatic* pJobIconTeam_A;
		CDnJobIconStatic* pJobIconTeam_B;
#endif
		CEtUIStatic *pStaticTeam_A;
		CEtUIStatic *pStaticTeam_B;
		CEtUIStatic *pStaticVS;
		CEtUIStatic *pStaticDummy;

		sLadderRoomList()
			: pStaticTeam_A(NULL)
			, pStaticTeam_B(NULL)
			, pStaticVS(NULL)
			, pStaticDummy(NULL)
#ifdef PRE_FIX_PVP_LOBBY_RENEW
			, pJobIconTeam_A(NULL)
			, pJobIconTeam_B(NULL)
#endif
		{}
	};

	struct sLadderInfo // 지저분 하지만 정보가많으니 ㅠㅠ
	{
		CEtUIStatic *pStatic_Score;
		CEtUIStatic *pStatic_ExpectPoint;
		CEtUIStatic *pStatic_WeekCount;
		CEtUIStatic *pStatic_TotalWinningRate;
		CEtUIStatic *pStatic_TotalWinningStreak;
		CEtUIStatic *pStatic_TotalLosingStreak ;
		CEtUIStatic *pStatic_TotalHighClass;
		CEtUIStatic *pStatic_TotalLowClass;
		CEtUIStatic *pStatic_TodayWin;
		CEtUIStatic *pStatic_TodayDefeat;
		CEtUIStatic *pStatic_TodayDraw;
		CEtUIStatic *pStatic_TodayWinningRate;
		CEtUIStatic *pStatic_TotalWin;
		CEtUIStatic *pStatic_TotalDefeat;
		CEtUIStatic *pStatic_TotalDraw;
		CEtUIStatic *pStatic_TotalWinningRate_S;

		sLadderInfo()
			: pStatic_Score(NULL)
			, pStatic_ExpectPoint(NULL)
			, pStatic_WeekCount(NULL)
			, pStatic_TotalWinningRate(NULL)
			, pStatic_TotalWinningStreak(NULL)
			, pStatic_TotalLosingStreak(NULL)
			, pStatic_TotalHighClass(NULL)
			, pStatic_TotalLowClass(NULL)
			, pStatic_TodayWin(NULL)
			, pStatic_TodayDefeat(NULL)
			, pStatic_TodayDraw(NULL)
			, pStatic_TodayWinningRate(NULL)
			, pStatic_TotalWin(NULL)
			, pStatic_TotalDefeat(NULL)
			, pStatic_TotalDraw(NULL)
			, pStatic_TotalWinningRate_S(NULL)
		{}
	};

public:

	void InitializeCommonInfo();
	void InitializeLadderInfo(); // 지저분하니따로빼자
	void InitializeLadderRoomInfo();

	void ShowLadderListBox(bool bShow , LadderSystem::MatchType::eCode eMatchCode);
	void SetPVPLadderState(int nState);
	void SetRoomLeaderName(std::wstring wszName);
	void RefreshRoomList(UINT nPage = 1);
	void ResetRoomList(bool bShow = false);
	void ResetRoomPage();
	void SetRoomList(LadderSystem::SC_PLAYING_ROOMLIST *pData);
	void SetLadderScoreInfo(LadderSystem::SC_SCOREINFO *pData);
	void SetLadderJobScoreInfo(LadderSystem::SC_SCOREINFO_BYJOB *pData);
	void SetLadderLeftTime(LadderSystem::SC_MATCHING_AVGSEC *pData);
	std::wstring GetRoomLeaderID(){return m_wszRoomLeaderName;}
	void EnableSearchButton(bool bShow,bool bEnable = true);
	void EnalbeStopSearchButton(bool bShow, bool bEnable = true);
	void SetLadderType(int nLadderType){m_nSelectedLadderType = nLadderType;}
	UINT GetLocalUserState() const { return m_nLadderState; }

	CDnPVPLadderTeamGameDlg *GetLadderTeamGameDlg(){return m_pTeamGameDlg;}
	CDnPVPLadderObserverInfoDlg *GetLadderObserverInfoDlg(){return m_pLadderObserverInfoDlg;}

#ifdef PRE_ADD_COLOSSEUM_BEGINNER
	bool IsLeavenLadderByExitBtn() { return m_bLeavenLadderByExitButton; };
#endif // #ifdef PRE_ADD_COLOSSEUM_BEGINNER

#ifdef PRE_ADD_DWC
	void SetDWCScoreInfo(TDWCTeam pData);
	void SetDWCModeControls();
#endif // PRE_ADD_DWC


protected:

	int m_nSelectedLadderType;
	int m_nMatchingSoundIndex;
	std::wstring m_wszRoomLeaderName;
	int m_nLadderState;
	int m_nSelectedRoomIndex;
	float m_nRoomRefreshDelay;

	UINT m_nLadderRoom_Page;
	UINT m_nLadderRoom_MaxPage;

	CEtUIButton *m_pButtonStartSearch;
	CEtUIButton *m_pButtonStopSearch;
	CEtUIButton *m_pButtonExit;
	CEtUIButton *m_pButtonNext;
	CEtUIButton *m_pButtonPrev;
	CEtUIButton *m_pButtonObserverJoin;
	CEtUIButton *m_pButtonRefresh;

	CEtUIListBoxEx *m_pLadderListBox;
	CEtUIStatic *m_pLadderRoomPage;
	CEtUIStatic *m_pLadderLeftTime;
	CEtUIStatic *m_pStaticRoomSelect;

	std::vector<sLadderRoomList> m_vecLadderRoomList;
	sLadderInfo m_sLadderInfo;

	CDnPVPLadderPreviewDlg *m_pPreviewDlg;
	CDnPVPLadderTeamGameDlg *m_pTeamGameDlg;
	CDnPVPLadderObserverInfoDlg *m_pLadderObserverInfoDlg;
	LadderSystem::SC_PLAYING_ROOMLIST m_sPlayingRoomListInfo; // 플레이어 정보를 가지고있는다.

#ifdef PRE_ADD_COLOSSEUM_BEGINNER
	bool m_bLeavenLadderByExitButton;
#endif // #ifdef PRE_ADD_COLOSSEUM_BEGINNER
};
