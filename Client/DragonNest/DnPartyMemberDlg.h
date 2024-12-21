
#pragma once

#include "DnCustomDlg.h"
#include "DnPartyData.h"

class CDnPartyMemberDlg : public CDnCustomDlg
{
public:
	CDnPartyMemberDlg(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);
	virtual ~CDnPartyMemberDlg() {}

	void InitialUpdate();

	void ResetCurSelectIndex();
	void SetCurSelectIndex(int idx) { m_nCurSelectIndex = idx; }
	int GetCurSelectIndex() const { return m_nCurSelectIndex; }
	virtual CEtUIControl* GetCurSelectCtrl() = 0;
	virtual int GetCurSelectUserSessionID() const = 0;

	virtual void ProcessInvitaion();
	virtual void Process(float fElapsedTime);
	virtual bool ShowInvitationCtrl(bool bShow) = 0;
	virtual void RefreshList() = 0;

	virtual int	 FindInsideItem( float fX, float fY ) = 0;
	virtual void UpdateSelectBar();
	virtual void SetPartyInfoLeader(int nLeaderIndex) = 0;
	virtual bool MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

#ifdef PRE_ADD_NEWCOMEBACK
	virtual void SetComebackAppellation( UINT sesssionID, bool bComeback ){}
#endif // PRE_ADD_NEWCOMEBACK

protected:
	void ShowPartyContextMenu(bool bShow, float fX, float fY, const SPartyMemberData& memberInfo);
	CEtUIStatic* GetSelectBar() { return m_pSelectBar; }

private:
	int		m_nCurSelectIndex;
	bool	m_bPartyMemberInviting;
	CEtUIStatic* m_pSelectBar;
};