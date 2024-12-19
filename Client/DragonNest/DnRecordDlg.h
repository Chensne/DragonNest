#pragma once

#include "EtUIDialog.h"

class CDnRecordDlg : public CEtUIDialog
{
public:
	CDnRecordDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnRecordDlg();

protected:
	CEtUIStatic *m_pRecordTime;
	CEtUIStatic *m_pRecordSize;
	CEtUIStatic *m_pRecordIcon[3];

	DWORD m_dwPrevTime;

	int	m_ResWidth;
	int m_ResHeight;

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void Process( float fElapsedTime );
};