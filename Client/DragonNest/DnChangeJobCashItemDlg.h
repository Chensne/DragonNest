#pragma once
#ifdef PRE_ADD_CHANGEJOB_CASHITEM
#include "EtUIDialog.h"
#include "DnSmartMoveCursor.h"

class CDnItem;

class CDnChangeJobCashItemDlg : public CEtUIDialog, public CEtUICallback
{
private:
	enum
	{
		CHANGEJOB_CONFIRM_MB,
	};

	CEtUIListBox* m_pJobListBox;
	CEtUITextBox* m_pJobExplainTextBox;
	CEtUIButton* m_pChangeJobOKButton;

	CDnSmartMoveCursor m_SmartMove;

	int m_iSelectedJobID;

	CDnItem* m_pChangeJobCashItem;
	vector<int> m_vlChangableJobIDs;

protected:
	void _CalcChangableJobs( void );

public:
	CDnChangeJobCashItemDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnChangeJobCashItemDlg( void );

	void SetItem( CDnItem* pChangeJobCashItem );

	virtual void Initialize( bool bShow );
	virtual void InitialUpdate( void );
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void OnUICallbackProc( int nID, UINT nCommand, CEtUIControl *pControl, UINT uiMsg );

	void OnResponseUseItemFromServer( void );
};

#endif // #ifdef PRE_ADD_CHANGEJOB_CASHITEM