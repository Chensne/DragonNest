#pragma once
#include "EtUIDialog.h"

class CDnPVPObserverItemDlg : public CEtUIDialog
{
public:
	CDnPVPObserverItemDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnPVPObserverItemDlg(void);

protected:
	CEtUIStatic *	m_pStaticLevel;
	CEtUIStatic *	m_pStaticJob;
	CEtUIStatic *	m_pStaticName;

public:

	void SetCharInfo(int Level,LPCWSTR Jobindex,LPCWSTR Name);

	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
};
