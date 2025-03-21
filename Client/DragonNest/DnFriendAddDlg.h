#pragma once
#include "EtUIDialog.h"

class CDnFriendAddDlg : public CEtUIDialog, public CEtUICallback
{
	enum
	{
		MSGBOXID_CANNOT_ADDSELF
	};
public:
	CDnFriendAddDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnFriendAddDlg(void);

protected:
	CEtUIIMEEditBox *m_pEditBoxFriendName;
	CEtUIComboBox	*m_pComboBoxGroup;
	CEtUIButton		*m_pOkButton;

protected:
	void SetFriendGroup();
	bool DoAddFriend();

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void Process( float fElapsedTime );
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0 );
};
