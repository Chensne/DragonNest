#pragma once
#include "EtUIDialog.h"
#include "DnPartyMemberDlg.h"
#include "DnPartyData.h"

class CEtUIButton;
class CDnPartyRaidMemberDlg : public CDnPartyMemberDlg, public CEtUICallback
{
public:
	CDnPartyRaidMemberDlg(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);
	virtual ~CDnPartyRaidMemberDlg(void);

	virtual void InitialUpdate();
	virtual void Initialize(bool bShow);
	virtual void Show(bool bShow);
	virtual bool MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual void Process(float fElapsedTime);

	virtual void RefreshList();
	virtual int	 FindInsideItem(float fX, float fY);
	virtual void SetPartyInfoLeader(int nLeaderIndex);
	virtual bool ShowInvitationCtrl(bool bShow);
	virtual void UpdateSelectBar();

	CEtUIControl* GetCurSelectCtrl();
	virtual int GetCurSelectUserSessionID() const;

#ifdef PRE_ADD_NEWCOMEBACK
	virtual void SetComebackAppellation( UINT sessionID, bool bComeback );
#endif // PRE_ADD_NEWCOMEBACK

private:
	std::vector<SPartyMemberRaidUIData> m_PartyMemberDataList;
};