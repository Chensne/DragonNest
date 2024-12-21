#pragma once

#include "DnCustomDlg.h"
#include "DnUIDefine.h"

#ifdef PRE_ADD_PVP_TOURNAMENT

using namespace PvPTournamentUIDef;

class CDnPVPTournamentRoomUserListItemDlg : public CDnCustomDlg
{
public:
	CDnPVPTournamentRoomUserListItemDlg(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL, bool bAutoCursor = false);
	virtual ~CDnPVPTournamentRoomUserListItemDlg(void);

	virtual void Initialize(bool bShow);
	virtual void Show(bool bShow);

	void SetInfo(const SMatchUserInfo& info);

	bool IsSame(const SMatchUserInfo& info) const;

protected:
	virtual void InitialUpdate();

private:
	CDnJobIconStatic* m_pJobIcon;
	CEtUIStatic* m_pMaster;
	CEtUIStatic* m_pNameStatic;
	CEtUITextureControl* m_pTextureRank;
};

#endif