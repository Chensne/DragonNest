#pragma once
#include "EtUIDialog.h"

#ifdef PRE_ADD_EXCHANGE_POTENTIAL

class CDnPotentialTransferInfoDlg : public CEtUIDialog
{
public:
	CDnPotentialTransferInfoDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnPotentialTransferInfoDlg();

	enum
	{
		NUM_MAX_TRANSFER_ITEM = 10,
	};

protected:
	CEtUIComboBox *m_pComboSort0;
	CEtUIComboBox *m_pComboSort1;
	CEtUIListBoxEx *m_pListBoxEx;

	CEtUIButton *m_pButtonPagePrev;
	CEtUIButton *m_pButtonPageNext;
	CEtUIStatic *m_pStaticPage;
	int m_nCurPage;
	int m_nMaxPage;

	std::vector<int> m_vecTransferItemID;

	void InitControl();
	void UpdateList();
	void UpdateComboBoxSub();
	void UpdateTransferList();
	void UpdatePage();
	void UpdateTransferCurrentPage();

	void PrevPage();
	void NextPage();

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
};

#endif