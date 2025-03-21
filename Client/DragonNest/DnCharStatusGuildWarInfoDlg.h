#pragma once
#include "DnCustomDlg.h"

class CDnCharStatusGuildWarInfoDlg : public CDnCustomDlg
{
public:
	CDnCharStatusGuildWarInfoDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnCharStatusGuildWarInfoDlg();
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void Show( bool bShow );

public:
	void SetGuildWarInfo();

protected:
	CEtUIStatic*	m_pGuildWarPoint;
	CEtUIStatic*	m_pCountGoFinals;
	CEtUIStatic*	m_pCountWin;

	CEtUIStatic* m_pGuildFestPoint;
};
