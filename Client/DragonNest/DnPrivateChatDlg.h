#pragma once
#include "EtUIDialog.h"

class CDnPrivateChatDlg : public CEtUIDialog
{
public:
	CDnPrivateChatDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnPrivateChatDlg(void);

protected:
	CEtUIStatic *m_pStaticChat;
	CEtUIIMEEditBox *m_pEditBoxChat;

	std::wstring m_strName;
	eChatType m_eChatType;

public:
	void SetPrivateName( LPCWSTR szPrivateName );
	void SetGuildName( LPCWSTR szGuildName );

	void DoSendChat();

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
};
