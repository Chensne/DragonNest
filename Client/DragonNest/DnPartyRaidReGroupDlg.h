#pragma once

#include "DnCustomDlg.h"
#include "DnPartyData.h"
#include "DnPartyTask.h"

class CEtUIButton;
class CDnPartyRaidReGroupDlg : public CDnCustomDlg, public CEtUICallback
{
public:
	CDnPartyRaidReGroupDlg(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);
	virtual ~CDnPartyRaidReGroupDlg(void);

	virtual void InitialUpdate();
	virtual void Initialize(bool bShow);
	virtual void Show(bool bShow);
	virtual void Process(float fElapsedTime);
	virtual void ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0);
	virtual bool MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	void RefreshList(bool bUIOnly = false);

private:
	void	AddToList(const CDnPartyTask::PartyStruct& content, int index);
	int		FindInsideItem(float fMouseX, float fMouseY) const;
	void	ResetSwapIdx();
	void	SetSelect(int nIdx);
	void	ClearGroupIdexes();

	int		GetGroupIndex(int index) const;
	bool	IsOtherGroupIdxWithSwapSrcIdx(int index) const;
	int		GetGroupEmptySlotIndex(int destIdx) const;
	void	SortGroupSessionIDs();

	CEtUIButton* m_pOkBtn;
	CEtUIButton* m_pCancelBtn;
	std::vector<SRaidReGroupInfo> m_PartyReGroupList;
	UINT m_GroupSessionIDs[RAIDPARTYCOUNTMAX];

	int	m_nSwapSrcIdx;
	bool m_bChanged;
};