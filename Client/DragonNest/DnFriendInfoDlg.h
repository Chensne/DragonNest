#pragma once
#include "EtUIDialog.h"

class CDnTextInputDlg;

class CDnFriendInfoDlg : public CEtUIDialog, public CEtUICallback
{
public:
	CDnFriendInfoDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnFriendInfoDlg(void);

protected:
	CDnTextInputDlg *m_pTextInputDlg;

	CEtUIComboBox *m_pComboBoxGroup;
	CEtUIStatic *m_pStaticName;
	CEtUIStatic *m_pStaticClass;
	CEtUIStatic *m_pStaticUserLevel;
// 	CEtUIStatic *m_pStaticWorldLevel;
	CEtUIStatic *m_pStaticMap;
	//CEtUIStatic *m_pStaticParty;
	CEtUIButton *m_pButtonOK;
	CEtUIButton *m_pButtonCancel;
// 	CEtUITextBox *m_pTextBoxMemo;
// 	CEtUIButton *m_pButtonEdit;
// 	CEtUIButton *m_pButtonDelete;

	INT64 m_biFrinedCharDBID;
		
protected:
	void SetFriendMemo( const wchar_t *wszMemo );
	void SetFriendGroup( int nGroupID );

	void GetLocationText(tstring& result, const TCommunityLocation& info);

	void UpdateFriendInfo();
	bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	void Process( float fElapsedTime );

public:
	void RefreshFriendInfo( INT64 biCharDBID );
	void SetFriendID( INT64 biCharDBID ) { m_biFrinedCharDBID = biCharDBID; }

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0 );
};
