#pragma once
#include "EtUIDialog.h"

class CDnGuildRenameConfirmDlg;
class CDnGuildRenameDlg : public CEtUIDialog, public CEtUICallback
{
public:
	CDnGuildRenameDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnGuildRenameDlg(void);

public:
	void SetInfo( char cInvenType, BYTE cInvenIndex, INT64 biItemSerial );

private :
	CDnGuildRenameConfirmDlg*				m_pConfirmDlg;

protected:
	
	CEtUIIMEEditBox *m_pEditBoxGuildName;	
	CEtUIButton *m_pButtonOK;

	char m_cInvenType;
	BYTE m_cInvenIndex;
	INT64 m_biItemSerial;

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg = 0 );
};