#pragma once
#include "EtUIDialog.h"

class CDnCountDownDlg : public CEtUIDialog
{
public:
	CDnCountDownDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnCountDownDlg(void);

protected:
	CEtUIAnimation *m_pAniCount;

public:
	bool IsCounting();

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void Render( float fElapsedTime );
};