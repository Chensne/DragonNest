#pragma once
#include "EtUIDialog.h"

class CDnRespawnGauageDlg : public CEtUIDialog
{
public:
	CDnRespawnGauageDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnRespawnGauageDlg(void);

protected:
	CEtUIStatic *m_pStaticText;
	CEtUIProgressBar *m_pProgressBarTime;

	float m_fProgressBarTime;
	float m_fElapsedTime;

public:
	void SetInfo( LPCWSTR pwszMessage, float fTime, int nID = -1, CEtUICallback *pCall = NULL );
	void SetSpaceText( bool bShow );

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Show( bool bShow );
	virtual void Process( float fElapsedTime );
};
