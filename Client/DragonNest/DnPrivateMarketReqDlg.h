#pragma once
#include "EtUIDialog.h"

class CDnPrivateMarketReqDlg : public CEtUIDialog
{
public:
	CDnPrivateMarketReqDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnPrivateMarketReqDlg(void);

protected:
	CEtUIStatic *m_pStaticText;
	CEtUIProgressBar *m_pProgressBarTime;
	CEtUIButton *m_pButtonCancel;

	float m_fProgressBarTime;
	float m_fElapsedTime;

public:
	void SetInfo( LPCWSTR pwszMessage, float fTime, int nID = -1, CEtUICallback *pCall = NULL );

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Show( bool bShow );
	virtual void Process( float fElapsedTime );
};
