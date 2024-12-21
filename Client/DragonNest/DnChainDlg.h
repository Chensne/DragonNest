#pragma once
#include "DnCountDlg.h"

class CDnChainDlg : public CDnCountDlg
{
public:
	CDnChainDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_BOTTOM, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnChainDlg(void);

public:
	void SetChain( int nChain );

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Render( float fElapsedTime );
};
