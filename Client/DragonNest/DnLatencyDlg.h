#pragma once
#include "EtUIDialog.h"

class CDnLatencyDlg : public CEtUIDialog
{
private:
	CEtUIStatic *m_pStaticLag[3];

public:
	CDnLatencyDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnLatencyDlg(void);


public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void SetDelay(int ping);
	virtual void Render( float fElapsedTime );
};