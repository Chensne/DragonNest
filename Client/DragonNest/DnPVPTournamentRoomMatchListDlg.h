#pragma once

#ifdef PRE_ADD_PVP_TOURNAMENT

#include "DnCustomDlg.h"
#include "DnBaseRoomDlg.h"
#include "DnUIDefine.h"

using namespace PvPTournamentUIDef;

class CDnPVPTournamentDataMgr;
class CDnPVPTournamentRoomDlg;
class CDnPVPTournamentRoomMatchListDlg : public CDnCustomDlg
{
public:
	struct SMatchListUI
	{
		CEtUIStatic* pNameStatic;
		CEtUIStatic* pSelectStatic;
		CEtUIStatic* pLeader;
		CEtUIStatic* pInGame;
		CEtUIStatic* pReady;

		SMatchListUI() : pNameStatic(NULL), pSelectStatic(NULL), pLeader(NULL), pInGame(NULL), pReady(NULL) {}
		void SetCtrls(bool bLeftSide, int index, CEtUIDialog& parent);
		void ShowAll(bool bShow);
		bool IsEmpty() const;
	};

	CDnPVPTournamentRoomMatchListDlg(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL, bool bAutoCursor = false);
	virtual ~CDnPVPTournamentRoomMatchListDlg(void);

	virtual void InitialUpdate();
	virtual void Initialize(bool bShow);
	virtual void ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0);
	virtual void Process(float fElapsedTime);
	virtual void Show(bool bShow);
	virtual bool MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	void ActivateMode(eMatchModeByCount modeType);
	void SetBaseDlg(CDnPVPTournamentRoomDlg* pDlg);
	void UpdateMatchListUI(const SMatchUserInfo& info);

	float GetCurrentListUIHeight() const;
	void ResetSelect();
	void ResetMatchListUI();

	int GetCurrentSelectUserUIIndex() const { return m_CurrentSelectStaticIndex; }

private:
	CDnPVPTournamentDataMgr* GetDataMgr() const;
	void Reset();
	void SendToServerSwapUserPosition(int uiIndex);

	CEtUIStatic* m_pMatchListBase[eMatchModeByCount::eMODE_MAX];
	std::vector<SMatchListUI> m_pMatchListUI;
	eMatchModeByCount m_CurrentModeCache;
	int m_CurrentSelectStaticIndex;
	CDnPVPTournamentRoomDlg* m_pBaseDlg;
};

#endif	// PRE_ADD_PVP_TOURNAMENT