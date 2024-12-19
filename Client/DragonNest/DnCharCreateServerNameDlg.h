#pragma once
#include "EtUIDialog.h"

class CDnCharCreateServerNameDlg : public CEtUIDialog
{
public:
	CDnCharCreateServerNameDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnCharCreateServerNameDlg(void);

protected:
	CEtUIStatic *m_pStaticName;

public:
	void SetServerName( const WCHAR *wszName ) { m_pStaticName->SetText( wszName ); }

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
};