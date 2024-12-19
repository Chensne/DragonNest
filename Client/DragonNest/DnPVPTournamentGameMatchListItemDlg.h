#pragma once

#ifdef PRE_ADD_PVP_TOURNAMENT

#include "DnCustomDlg.h"
#include "DnUIDefine.h"

using namespace PvPTournamentUIDef;

class CDnPVPTournamentDataMgr;
class CDnPVPTournamentGameMatchListDlg;
class CDnPVPTournamentGameMatchListItemDlg : public CDnCustomDlg
{
public:
	struct SGameMatchListUI
	{
		CEtUIStatic* pNameStatic;
		CEtUIStatic* pSelectStatic;
		CEtUIStatic* pLeader;
		CEtUIStatic* pCurrentUserStatic;
		CDnJobIconStatic* pJobIcon;
		bool bLeftSide;

		SGameMatchListUI() : pNameStatic(NULL), pSelectStatic(NULL), pLeader(NULL), pJobIcon(NULL), bLeftSide(true), pCurrentUserStatic(NULL) {}
		void SetCtrls(bool bLeftSide, int index, CEtUIDialog& parent);
		void ShowAll(bool bShow);
		bool IsEmpty() const;
	};

	CDnPVPTournamentGameMatchListItemDlg(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL, bool bAutoCursor = false);
	virtual ~CDnPVPTournamentGameMatchListItemDlg();

	virtual void InitialUpdate();
	virtual void Initialize(bool bShow) = 0;
	virtual const int GetMaxUserCount() const = 0;
	virtual const int GetMaxRoundCount() const = 0;

#ifdef _TEST_CODE_KAL
	virtual void Show(bool bShow);
#endif

	void Update();
	void Reset();

	void SetBaseDlg(CDnPVPTournamentGameMatchListDlg* pBaseDlg) { m_pBaseDlg = pBaseDlg; }
	float GetCurrentListUIHeight() const;

protected:
	CDnPVPTournamentDataMgr* GetDataMgr() const;

private:
	std::vector<std::vector<CEtUIStatic*> > m_LeftAdvanceLine;
	std::vector<std::vector<CEtUIStatic*> > m_RightAdvanceLine;
	CEtUIStatic* m_pLeftFinalRoundWinLine;
	CEtUIStatic* m_pRightFinalRoundWinLine;

	std::vector<SGameMatchListUI> m_pMatchListUI;
	CDnPVPTournamentGameMatchListDlg* m_pBaseDlg;
	CEtUIStatic* m_pBackground;
	CEtUIStatic* m_pTrophySelect;
};

class CDnPVPTournamentGameMatchListItemDlg_4User : public CDnPVPTournamentGameMatchListItemDlg
{
public:
	CDnPVPTournamentGameMatchListItemDlg_4User(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);
	virtual ~CDnPVPTournamentGameMatchListItemDlg_4User() {}

	virtual void Initialize(bool bShow);
	virtual const int GetMaxUserCount() const { return 4; }
	virtual const int GetMaxRoundCount() const { return 1; }
};

class CDnPVPTournamentGameMatchListItemDlg_8User : public CDnPVPTournamentGameMatchListItemDlg
{
public:
	CDnPVPTournamentGameMatchListItemDlg_8User(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);
	virtual ~CDnPVPTournamentGameMatchListItemDlg_8User() {}

	virtual void Initialize(bool bShow);
	virtual const int GetMaxUserCount() const { return 8; }
	virtual const int GetMaxRoundCount() const { return 2; }
};

class CDnPVPTournamentGameMatchListItemDlg_16User : public CDnPVPTournamentGameMatchListItemDlg
{
public:
	CDnPVPTournamentGameMatchListItemDlg_16User(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);
	virtual ~CDnPVPTournamentGameMatchListItemDlg_16User() {}

	virtual void Initialize(bool bShow);
	virtual const int GetMaxUserCount() const { return 16; }
	virtual const int GetMaxRoundCount() const { return 3; }
};

class CDnPVPTournamentGameMatchListItemDlg_32User : public CDnPVPTournamentGameMatchListItemDlg
{
public:
	CDnPVPTournamentGameMatchListItemDlg_32User(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);
	virtual ~CDnPVPTournamentGameMatchListItemDlg_32User() {}

	virtual void Initialize(bool bShow);
	virtual const int GetMaxUserCount() const { return 32; }
	virtual const int GetMaxRoundCount() const { return 4; }
};

#endif	// PRE_ADD_PVP_TOURNAMENT