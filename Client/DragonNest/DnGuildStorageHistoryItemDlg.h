#pragma once
#include "EtUIDialog.h"

class CDnGuildStorageHistoryItemDlg : public CEtUIDialog
{
public:
	CDnGuildStorageHistoryItemDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnGuildStorageHistoryItemDlg(void);

protected:
	CEtUIStatic *m_pStaticList0;
	CEtUIStatic *m_pStaticList1;
	CEtUIStatic *m_pStaticList2;
	CEtUIStatic *m_pStaticList3;
	CEtUIStatic *m_pStaticList4;

public:
	void SetInfo( const WCHAR *wszText1, const WCHAR *wszText2, const WCHAR *wszText3, const WCHAR *wszText4, const WCHAR *wszText5 );

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
};