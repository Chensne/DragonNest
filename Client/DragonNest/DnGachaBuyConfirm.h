#pragma once
#include "EtUIDialog.h"
#include "DnSmartMoveCursor.h"

#ifdef PRE_ADD_GACHA_JAPAN

class CDnGachaBuyConfirmDlg : public CEtUIDialog
{
private:
	//CEtUIStatic* m_pStaticBuyConfirm;

	CDnSmartMoveCursor m_SmartMove;

protected:

public:
	CDnGachaBuyConfirmDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnGachaBuyConfirmDlg(void);

	void Initialize( bool bShow );
	void InitialUpdate( void );
	void Show( bool bShow );
};

#endif // PRE_ADD_GACHA_JAPAN