#pragma once
#include "EtUIDialog.h"

class CDnBlindCaptionDlg : public CEtUIDialog
{
public:
	CDnBlindCaptionDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnBlindCaptionDlg(void);

protected:
	CEtUIStatic *m_pStaticCaption;

public:
	void SetCaption( const wchar_t *wszCaption );
	void ClearCaption();

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
};
