#pragma once
#include "EtUIDialog.h"


#ifdef PRE_MOD_APPELLATIONBOOK_RENEWAL

class CDnAppellationBookContentsDlg : public CEtUIDialog
{
public:
	CDnAppellationBookContentsDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);
	virtual ~CDnAppellationBookContentsDlg(void);

	CEtUIStatic* m_pStaticContentsName_Select;
	CEtUIStatic* m_pStaticContentsName_DeSelect;
	CEtUIStatic* m_pStaticLine;
	CEtUIStatic* m_pStaticEnable;
	CEtUIStatic* m_pStaticDisable;
	CEtUIStatic* m_pSelectImage;
	int			 m_nIndexID;

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Show( bool bShow );	
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg );
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	
	void SetData(std::wstring wszTitle, bool bIsEnable, int nIndexID);
	void ResetData();
	void SelectListBox(bool bShow);
};

#endif