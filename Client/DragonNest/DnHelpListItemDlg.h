#pragma once

#include "DnCustomDlg.h"
#include "DnHelpDlg.h"

class CDnHelpListItemDlg : public CDnCustomDlg
{
public:
	CDnHelpListItemDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnHelpListItemDlg(void);

protected:
	CEtUIStatic *m_pStaticText;
	int m_nLinkHelpTableID;

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void Process( float fElapsedTime );

	void SetInfo( CDnHelpDlg::KeywordStruct *pStruct );
	int GetHelpTableID() { return m_nLinkHelpTableID; }
};