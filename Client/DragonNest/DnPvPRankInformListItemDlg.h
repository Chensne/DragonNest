#pragma once

#ifdef PRE_ADD_PVPRANK_INFORM
#include "DnUIDefine.h"
#include "DnCustomDlg.h"
#include "DnPvPRankInformDlg.h"

class CDnPvPRankInformListItemDlg : public CDnCustomDlg
{
public:
	CDnPvPRankInformListItemDlg(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);
	virtual ~CDnPvPRankInformListItemDlg(void);

	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	void ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg);
	void Process(float fElapsedTime);

	void SetData(const PvPRankInfoUIDef::SRankUnit& items);
	float GetCurrentListUIHeight() const;

protected:
	CEtUITextureControl* m_pRankIcon;
	CEtUIStatic* m_pRankName;
	CEtUIStatic* m_pNeedExp;
	CEtUIStatic* m_pBackground;
	CDnItemSlotButton* m_pRewardItem[PvPRankInfoUIDef::eMAX_REWARD_ITEM_COUNT];

private:
	void ClearListItem();
};

#endif // PRE_ADD_PVPRANK_INFORM