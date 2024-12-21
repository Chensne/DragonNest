#pragma once
#include "DnSlideCaptionDlg.h"

class CDnGameTipDlg : public CDnSlideCaptionDlg
{
public:
	CDnGameTipDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnGameTipDlg(void);

public:
	virtual void InitialUpdate();
	virtual void UpdateBlind();
	virtual void Render( float fElapsedTime );
};