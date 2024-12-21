#pragma once
#include "EtUIDialog.h"

class CEtUIStatic;
class CEtUIProgressBar;

class CDnAcceptDlg : public CEtUIDialog
{
public:
	CDnAcceptDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnAcceptDlg(void);

protected:
	CEtUIStatic *m_pStaticText;
	CEtUIProgressBar *m_pProgressBarTime;
	CEtUIButton *m_pButtonCancel;

	float m_fProgressBarTime;
	float m_fElapsedTime;

public:
	void SetInfo( LPCWSTR pwszMessage, float fTotalTime, float fElapsedTime, int nID = -1, CEtUICallback *pCall = NULL );

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Show( bool bShow );
	virtual void Process( float fElapsedTime );
};
