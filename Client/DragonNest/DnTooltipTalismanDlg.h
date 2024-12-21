#include "EtUIDialog.h"

#if defined(PRE_ADD_TALISMAN_SYSTEM)

class CDnTooltipTalismanDlg : public CEtUIDialog
{
public:
	CDnTooltipTalismanDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnTooltipTalismanDlg();
	
protected:
	CEtUITextBox* m_pTextBox;

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Show( bool bShow );

	void SetTalismanSlot( int nSlotIndex , bool bIsCash=false);
};

#endif // PRE_ADD_TALISMAN_SYSTEM