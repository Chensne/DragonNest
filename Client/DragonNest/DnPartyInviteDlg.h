#pragma once
#include "EtUIDialog.h"
#ifdef PRE_PARTY_DB
#include "DnTextAutoCompleteUIMgr.h"
#endif

class CDnPartyInviteDlg : public CEtUIDialog, public CEtUICallback
{
public:
	CDnPartyInviteDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnPartyInviteDlg(void);

protected:
	CEtUIIMEEditBox*	m_pEditBoxGuestName;
	CEtUIButton*		m_pButtonOk;
	CEtUIButton*		m_pButtonCancel;
#ifdef PRE_PARTY_DB
	CEtUIComboBox* m_pFriendComboBox;
	CDnTextAutoCompleteUIMgr m_AutoCompleteMgr;
	std::wstring m_AutoCompleteTextCache;
#endif

	std::wstring		m_GuestName;

private:
	bool				DoInvite();
#ifdef PRE_PARTY_DB
	void SetFriendComboBox();
	void OnAddEditBoxString(const std::wstring& strName);
#endif

public:
	virtual void		InitialUpdate();
	virtual void		Initialize( bool bShow );
	virtual void		Show( bool bShow );
	void				ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg );
	virtual void		OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg );
	virtual bool		MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
};
