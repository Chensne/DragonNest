#pragma once
#include "DnCustomDlg.h"
#include "DnMissionTask.h"

class CDnItem;

class CDnMissionAppellationTooltipDlg : public CEtUIDialog
{
public:
	CDnMissionAppellationTooltipDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnMissionAppellationTooltipDlg(void);

protected:
	CEtUITextBox *m_pDescription;

protected:
	int m_nAppellationIndex;

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );

	void SetAppellationInfo( int nItemID );
};
