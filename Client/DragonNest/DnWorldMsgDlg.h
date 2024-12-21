#pragma once
#include "EtUIDialog.h"



#ifdef PRE_ADD_WORLD_MSG_RED
#define MAX_WORLD_MSG_CHAR 40
#endif // PRE_ADD_WORLD_MSG_RED


class CDnWorldMsgDlg : public CEtUIDialog, public CEtUICallback
{
public:
	CDnWorldMsgDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnWorldMsgDlg(void);

public:
	void SetInfo( int nType, INT64 biItemSerial );

protected:
	CEtUIStatic *m_pStaticTitle;
#ifdef PRE_ADD_WORLD_MSG_RED
	CEtUILineIMEEditBox *m_pEditBoxMsg;
#else // PRE_ADD_WORLD_MSG_RED
	CEtUIIMEEditBox *m_pEditBoxMsg;
#endif // PRE_ADD_WORLD_MSG_RED
	CEtUIButton *m_pButtonOK;

	int m_nType;
	INT64 m_biItemSerial;

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0 );
	void		 Process(float fElapsedTime);
	void		 AppendChatEditBox(LPCWSTR wszString, bool bFocus);
};