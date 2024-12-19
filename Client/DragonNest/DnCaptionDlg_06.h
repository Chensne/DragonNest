#pragma once

#include "DnCustomDlg.h"


class CDnCaptionDlg_06 : public CDnCustomDlg
{
public:
	CDnCaptionDlg_06( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnCaptionDlg_06(void);

	
protected:
	CDnMessageStatic * m_pStaticMessage;

public:
	void SetCaption( LPCWSTR wszMsg, DWORD dwColor, float fFadeTime );
	void CloseCaption();

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
};