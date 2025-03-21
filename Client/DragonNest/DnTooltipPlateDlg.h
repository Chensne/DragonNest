#include "EtUIDialog.h"

class CDnTooltipPlateDlg : public CEtUIDialog
{
public :
	CDnTooltipPlateDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnTooltipPlateDlg(void);

	void SetGlyphSlot( int nSlotIndex );

protected :

	CEtUITextBox * m_pTextBox;

public :
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Show( bool bShow );
};