#pragma once
#include "EtUIDialog.h"
#include "DnAppellationTask.h"

class CDnAppellationDlg;
class CDnAppellationItemDlg : public CEtUIDialog
{
public:
	CDnAppellationItemDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnAppellationItemDlg(void);

protected:
	enum{
		EXPIRE_TIME_COLOR = 86400,	// 24½Ã°£
	};

	CEtUIStatic*	m_pStaticName;

	CEtUIStatic*	m_pStaticNew;
	CEtUICheckBox*	m_pCheckBox;
	CEtUICheckBox*	m_pCoverCheckBox;

	CDnAppellationTask::AppellationStruct* m_pInfo;
	CDnAppellationDlg* m_pParent;

public:
	void SetInfo( CDnAppellationTask::AppellationStruct *pInfo, CDnAppellationDlg *pDlg );
	CDnAppellationTask::AppellationStruct *GetInfo() const { return m_pInfo; }
	void SetChecked( bool bCheck );
	void SetCoverChecked( bool bCheck );
	bool IsOneDayExpire();

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
};
