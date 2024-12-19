#pragma once
#include "EtUIDialog.h"


class CDnMiniPlayerGuildWarPopupDlg : public CEtUIDialog, public CEtUICallback
{
public:
	CDnMiniPlayerGuildWarPopupDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnMiniPlayerGuildWarPopupDlg(void);

protected:

	CEtUIButton *m_pButtonMaster;
	CEtUIButton *m_pButtonSubMaster;
	CEtUIButton *m_pButtonSubMasterOff;
	std::wstring m_wstTargetName; // 대상이름

	UINT m_uiSessionID;

	bool IsExistActor();

public:
	virtual void InitialUpdate();
	virtual void Initialize(bool bShow);
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg );
	virtual void Process( float fElapsedTime );

	void SetTargetName(std::wstring wstName){ m_wstTargetName = wstName; }
	void SetSessionID( UINT uiSessionID )	{ m_uiSessionID = uiSessionID; }

	void SetControl( UINT eMyState, UINT eTargetState, UINT nSessionID, bool bSecondary );

private:
	
};
