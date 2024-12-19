#pragma once
#include "EtUIDialog.h"
#include "DnDataManager.h"
#include "DnQuestDlg.h"

class CEtUITextBox;

class CDnQuestDescriptionDlg : public CEtUIDialog
{
public:
	CDnQuestDescriptionDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnQuestDescriptionDlg(void);

protected:
	CEtUIHtmlTextBox *m_pTextHtmlBoxJournal;
	CEtUITextureControl *m_pQuestImage;
	CEtUIStatic*	 m_pStaticDescTitle; 
	EtTextureHandle m_hQuestImage;

public:
	bool SetJournalText( int nQuestIndex, const JournalPage *pJournalPage );

public:
	virtual void InitialUpdate();
	virtual void Initialize(bool bShow);
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
};
