#pragma once

#include "EtUIDialog.h"

#if defined( PRE_ADD_EASYGAMECASH )

class CDnPackageBoxProgressDlg : public CEtUIDialog
{
public:
	CDnPackageBoxProgressDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnPackageBoxProgressDlg(void);

protected:
	CEtUIButton *m_pCancelButton;
	CEtUIProgressBar *m_pProgressBarTime;
	CEtUIStatic * m_pString;

	float m_fTimer;
	char m_cFlag;
	char m_cInvenType;
	INT64 m_biInvenSerial;
	BYTE m_cInvenIndex;

protected:

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Process( float fElapsedTime );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void Show( bool bShow );

	void ShowEx( bool bShow, char cInvenType, BYTE cInvenIndex = -1, INT64 biInvenSerial = 0, float fTimer = 0.f );
};

#endif	// #if defined( PRE_ADD_EASYGAMECASH )