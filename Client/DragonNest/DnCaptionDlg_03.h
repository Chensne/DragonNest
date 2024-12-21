#pragma once
#include "DnCustomDlg.h"

class CDnCaptionDlg_03 : public CDnCustomDlg
{
public:
	CDnCaptionDlg_03( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnCaptionDlg_03(void);

protected:
	std::vector<CDnMessageStatic*> m_vecStaticMessage;

public:
	void AddCaption( LPCWSTR wszMsg, DWORD dwColor, float fFadeTime );
	void CloseCaption();

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
};