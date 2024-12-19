#pragma once
#include "EtUIDialog.h"

class CDnServerListDlg : public CEtUIDialog
{
public:
	CDnServerListDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnServerListDlg(void);

protected:
	CEtUIListBox *m_pServerList;
#ifndef PRE_MOD_SELECT_CHAR
	std::map<std::wstring,int> m_mapServerList;
#endif // PRE_MOD_SELECT_CHAR
	CEtUIButton *m_pButtonOK;
	CEtUIButton *m_pButtonBack;
	CEtUIButton *m_pButtonExit;

protected:
	void SelectServer();
	void EnableControl( bool bEnable );

public:
	void ClearServerList();
	void AddServerList( int nIndex, LPCWSTR szServerName, float fUserRatio, int nCharCount );
#ifndef PRE_MOD_SELECT_CHAR
	std::wstring GetServerName( int nIndex );
	int GetServerIndex( LPCWSTR szServerName );
#endif // PRE_MOD_SELECT_CHAR
	void SetDefaultList();
	void ShowBackButton( bool bShow );

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void Show( bool bShow );
};
