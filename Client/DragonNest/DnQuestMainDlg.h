#pragma once

#include "EtUIDialog.h"

class CDnQuestDescriptionDlg;
class CEtUITreeCtl;
class CEtUITextBox;

class CDnQuestMainDlg : public CEtUIDialog
{
public:
	CDnQuestMainDlg( int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnQuestMainDlg(void);

protected:
	CDnQuestDescriptionDlg *m_pDescriptionDlg;
	CEtUITreeCtl *m_pTreeControl;
	CEtUITextBox *m_pTextBoxTitle;

public:
	virtual void Initialize();
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
};