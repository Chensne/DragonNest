#pragma once
#include "EtUIDialog.h"

#ifdef PRE_ADD_INSTANT_CASH_BUY
class CDnRebirthCaptionDlg : public CEtUIDialog, public CEtUICallback
#else // PRE_ADD_INSTANT_CASH_BUY
class CDnRebirthCaptionDlg : public CEtUIDialog
#endif // PRE_ADD_INSTANT_CASH_BUY
{
public:
	CDnRebirthCaptionDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnRebirthCaptionDlg(void);

public:
	void SetRebirthCaptionHotKey( BYTE cVK );

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
#ifdef PRE_ADD_INSTANT_CASH_BUY
	virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0 );
#endif // PRE_ADD_INSTANT_CASH_BUY

public:
	enum emRebirthMode
	{
		Normal = 0,
		DragonNest,
	};
protected:
	emRebirthMode m_Mode;

public:
	void SetMode(emRebirthMode _mode) { m_Mode = _mode; }

	virtual void Show(bool bShow);

};
