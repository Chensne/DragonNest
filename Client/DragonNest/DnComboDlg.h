#pragma once
#include "DnCountDlg.h"

class CDnComboDlg : public CDnCountDlg
{
public:
	CDnComboDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_BOTTOM, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnComboDlg(void);

protected:
	bool m_bAction;
	bool m_bShake;
	bool m_bHide;
	int m_nShakeCount;

protected:
	bool UpdateShowAni();
	bool UpdateHideAni();

public:
	void SetCombo( int nCombo, int nDelay );

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Process( float fElapsedTime );
	virtual void Render( float fElapsedTime );
};
