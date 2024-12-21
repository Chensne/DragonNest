#pragma once

class CDnStickAniDlg : public CEtUIDialog
{
public:
	CDnStickAniDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnStickAniDlg(void);

protected:
	CEtUIAnimation *m_pAnimation;

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
};
