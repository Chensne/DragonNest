#pragma once
#include "EtUIDialog.h"

#ifdef PRE_ADD_EXCHANGE_POTENTIAL

class CDnPotentialTransferInfoItemDlg : public CEtUIDialog
{
public:
	CDnPotentialTransferInfoItemDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnPotentialTransferInfoItemDlg(void);

protected:
	CEtUIStatic *m_pStaticList0;
	CEtUIStatic *m_pStaticList1;
	CEtUIStatic *m_pStaticList2;

public:
	void SetInfo( const WCHAR *wszText1, const WCHAR *wszText2, const WCHAR *wszText3, DWORD dwColor );

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
};

#endif