#pragma once
#include "EtUIDialog.h"

class CDnPartyKickDlg;
class CDnChatRoomPopupDlg : public CEtUIDialog, public CEtUICallback
{
public:
	CDnChatRoomPopupDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnChatRoomPopupDlg(void);

protected:

	enum
	{
		KICK_DIALOG,
	};

	CEtUIButton *m_pButtonFriend;
	CEtUIButton *m_pButtonPrivate;
	CEtUIButton *m_pButtonGuild;
	CEtUIButton *m_pButtonExile;
	CEtUIButton *m_pButtonNameCopy;

	bool m_bMaster;
	std::wstring m_wszName;

	CDnPartyKickDlg *m_pKickDlg;

protected:
	void InitControl();

public:
	void SetInfo( bool bMaster, LPCWSTR pwszName );

public:
	virtual void InitialUpdate();
	virtual void Initialize(bool bShow);
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg );

private:
	bool CheckFriendCondition(const WCHAR* name);
	bool IsFriend( const WCHAR *name );
};