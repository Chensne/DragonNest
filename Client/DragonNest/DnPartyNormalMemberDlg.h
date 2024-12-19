#pragma once
#include "EtUIDialog.h"
#include "DnPartyData.h"
#include "DnPartyMemberDlg.h"

class CEtUIButton;
class CDnPartyNormalMemberDlg : public CDnPartyMemberDlg, public CEtUICallback
{
public:
	CDnPartyNormalMemberDlg(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);
	virtual ~CDnPartyNormalMemberDlg(void);

	virtual void InitialUpdate();
	virtual void Initialize(bool bShow);
	virtual void Show(bool bShow);
	virtual void Process(float fElapsedTime);

	virtual bool ShowInvitationCtrl(bool bShow);
	virtual void RefreshList();

	virtual int	 FindInsideItem(float fX, float fY);
	virtual void SetPartyInfoLeader(int nLeaderIndex);

	CEtUIControl* GetCurSelectCtrl();
	virtual int GetCurSelectUserSessionID() const;

	bool MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

#ifdef PRE_ADD_NEWCOMEBACK
	virtual void SetComebackAppellation( UINT sessionID, bool bComeback );
#endif // PRE_ADD_NEWCOMEBACK

private:
	std::vector<SPartyMemberNormalUIData> m_PartyMemberDataList;
};