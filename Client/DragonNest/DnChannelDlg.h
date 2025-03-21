#pragma once
#include "EtUIDialog.h"
#include "DnChannelListDlg.h"

struct sChannelInfo;

class CDnChannelDlg : public CDnChannelListDlg , public CEtUICallback
{
public:
	CDnChannelDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnChannelDlg(void);

protected:
	virtual void SendChannel();

public:
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void Process( float fElapsedTime );
//blondymarry start
	// CEtUICallback
	virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0 );
//blondymarry end
};