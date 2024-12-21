#pragma once
#include "EtUIDialog.h"

class CDnCharStatusBasePopDlg : public CEtUIDialog, public CEtUICallback
{
public:
	enum eUI_Option
	{
		MAX_USAGE = 7, // == CDnCharStatusBaseInfoDlg::ePageType::MAX
	};
	
public:
	CDnCharStatusBasePopDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnCharStatusBasePopDlg(void);
	
	virtual void InitialUpdate();
	virtual void Initialize(bool bShow);
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );

public:
	void SetEnabledPage(int nPage) { m_nSelectedSection = nPage; }

protected:

	int m_nSelectedSection;
	CEtUIButton *m_pButton[eUI_Option::MAX_USAGE];
		
};
