#pragma once
#include "EtUIDialog.h"
#include "DnBlindDlg.h"

class CDnMissionFailDlg : public CEtUIDialog, public CDnBlindCallBack
{
public:
	CDnMissionFailDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnMissionFailDlg(void);

protected:
	float m_fDisplayTime;

public:
	virtual void Initialize( bool bShow );
	virtual void Show( bool bShow );
	virtual void Render( float fElapsedTime );

public:
	virtual void OnBlindOpen();
	virtual void OnBlindOpened();
	virtual void OnBlindClose();
	virtual void OnBlindClosed();
};
