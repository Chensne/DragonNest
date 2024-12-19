#pragma once

#include "DnCustomDlg.h"
#include "DnSmartMoveCursor.h"

class CDnItem;
class CDnCostumeMixCompleteDlg : public CDnCustomDlg
{
public:
	enum eCompleteType
	{
		eCOSMIX,
		eDESIGNMIX,
#ifdef PRE_ADD_COSRANDMIX
		eRANDOMMIX,
#endif
	};
	CDnCostumeMixCompleteDlg(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);
	virtual ~CDnCostumeMixCompleteDlg(void);

	void SetInfo(ITEMCLSID resultItemID, char cOption);
	void SetInfo(CDnItem* pCompleteItem);
	void SetType(eCompleteType type ) { m_Type = type; }

	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Show( bool bShow );

protected:
	virtual void ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg /* = 0 */);

	CDnItemSlotButton*	m_pResultSlotBtn;
	CEtUIStatic*		m_pResultStatic;
	CEtUIButton*		m_pOkBtn;
	CEtUIButton*		m_pCloseBtn;
	eCompleteType		m_Type;
	CDnSmartMoveCursor m_SmartMove;
};