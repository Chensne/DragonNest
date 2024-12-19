#pragma once
#include "EtUIDialog.h"


class CDnInstantSpecializationDlg : public CEtUIDialog
{
public:
	CDnInstantSpecializationDlg(UI_DIALOG_TYPE dialogType = UI_TYPE_CHILD, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);
	virtual ~CDnInstantSpecializationDlg(void);

protected:
	CEtUIStatic* m_pPopupBackground;
	CEtUIStatic* m_pPopupText;
	CEtUIStatic* m_pBookText;
	CEtUIButton* m_pBookButton;

private:
	int HasNextJob();

public:
	virtual void InitialUpdate();
	virtual void Initialize(bool bShow);
	virtual void Show(bool bShow);
	virtual void ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0);
};
