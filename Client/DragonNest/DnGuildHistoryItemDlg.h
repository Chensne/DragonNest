#pragma once
#include "EtUIDialog.h"

class CDnGuildHistoryItemDlg : public CEtUIDialog
{
public:
	CDnGuildHistoryItemDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnGuildHistoryItemDlg(void);

protected:
	CEtUIStatic *m_pStaticList0;
	CEtUIStatic *m_pStaticList1;

public:
	void SetInfo( const WCHAR *wszText1, const WCHAR *wszText2 );

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
};