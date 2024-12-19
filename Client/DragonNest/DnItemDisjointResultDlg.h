#pragma once
#include "EtUIDialog.h"

class CEtUITextBox;
class CDnItemDisjointInfoDlg;
class CDnItemDisjointResultDlg : public CEtUIDialog
{
public:
	CDnItemDisjointResultDlg(UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL);
	virtual ~CDnItemDisjointResultDlg(void);
	void OnRecvDropList(SCItemDisjointResNew *pPacket);
	
protected:
	CEtUIListBoxEx *m_pResultListBox;

private:
	void Clear();

public:
	virtual void InitialUpdate();
	virtual void Initialize(bool bShow);
	virtual void ProcessCommand(UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg);
};