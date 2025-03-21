#pragma once

#ifdef PRE_ADD_PVP_TOURNAMENT

#include "DnCustomDlg.h"
#include "DnUIDefine.h"

using namespace PvPTournamentUIDef;

class CDnPVPTournamentGameMatchListItemDlg;
class CDnPVPTournamentDataMgr;
class CDnPVPTournamentGameMatchListDlg : public CDnCustomDlg
{
public:
	CDnPVPTournamentGameMatchListDlg(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);
	virtual ~CDnPVPTournamentGameMatchListDlg(void);

	virtual void InitialUpdate();
	virtual void Initialize(bool bShow);
	virtual void ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg /* = 0 */);
	virtual void Show(bool bShow);
	virtual void Render(float fElapsedTime);
#ifdef PRE_PVP_GAMBLEROOM
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
#endif // PRE_PVP_GAMBLEROOM
	void SetGameMatchListItemDlg(CDnPVPTournamentGameMatchListItemDlg* pItemDlg);
	const CDnPVPTournamentDataMgr* GetDataMgr() const;
	void Update();

private:
	void _SetContentOffset();

	CEtUIListBox* m_pMatchListContainerListBox;
	CEtUIScrollBar* m_pScrollBar;
	CEtUIStatic* m_pMatchTitle;
	CEtUIButton* m_pCloseButton;
	CDnPVPTournamentGameMatchListItemDlg* m_pGameMatchListItemDlg;
#ifdef PRE_PVP_GAMBLEROOM
	CEtUIStatic* m_pStaticGamblePrice;
#endif // PRE_PVP_GAMBLEROOM

	bool m_bInitUpdate;
	int m_iScrollPos;
	float m_fOriContentDlgXPos;
	float m_fOriContentDlgYPos;
};

#endif	// PRE_ADD_PVP_TOURNAMENT