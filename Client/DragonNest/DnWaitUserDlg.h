#pragma once
#include "EtUIDialog.h"

class CDnWaitUserDlg  : public CEtUIDialog
{
public:
	CDnWaitUserDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnWaitUserDlg(void);

protected:
	CEtUIStatic *m_pServerName;
	CEtUIStatic *m_pPeopleNum;
	CEtUIStatic *m_pWaitTime;
	CEtUIButton *m_pQuitButton;

	std::wstring m_wszServerNameStatic;

protected:

public:
	void SetProperty( LPCWSTR  wszServerName, UINT nWaitUserCount, ULONG nEstimateTime );

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg );
};
