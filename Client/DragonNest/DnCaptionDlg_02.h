#pragma once
#include "DnCaptionDlg.h"

class CDnCaptionDlg_02 : public CDnCaptionDlg
{
public:
	CDnCaptionDlg_02( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnCaptionDlg_02(void);

	void SetCaption( LPCWSTR wszMsg, DWORD dwColor, float fFadeTime, bool bBottomPos );

protected:
	SUICoord m_uiDefaultPos;
	SUICoord m_uiBottomPos;

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
};
