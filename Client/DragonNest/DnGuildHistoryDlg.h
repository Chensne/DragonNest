#pragma once
#include "EtUIDialog.h"

class CDnGuildHistoryDlg : public CEtUIDialog
{
public:
	CDnGuildHistoryDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnGuildHistoryDlg(void);

protected:
	CEtUIListBoxEx *m_pListBoxEx;

	CEtUIButton *m_pButtonPagePrev;
	CEtUIButton *m_pButtonPageNext;
	CEtUIStatic *m_pStaticPage;
	int m_nPage;

	void InitControl();

public:
	void OnRecvGetGuildHistoryList( SCGetGuildHistoryList *pPacket );

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg );
};