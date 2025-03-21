#pragma once

#include "EtUIDialog.h"
#include "DnRoundModeHUD.h"

#ifdef PRE_MOD_PVPOBSERVER
#include "DnPvPObserverOrderListDlg.h"
#endif // PRE_MOD_PVPOBSERVER

#ifdef PRE_ADD_PVP_TOURNAMENT

class CDnPVPTournamentModeHUD : public CDnRoundModeHUD
{
public:
	CDnPVPTournamentModeHUD(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);
	virtual ~CDnPVPTournamentModeHUD(void);
	virtual void Initialize(bool bShow);

	void SetVersusUserName(const std::wstring& leftUser, const std::wstring& rightUser);
};

#endif // PRE_ADD_PVP_TOURNAMENT