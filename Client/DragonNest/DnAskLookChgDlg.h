#pragma once
#include "DnInCodeResource.h"
#include "DnEtcObject.h"
#include "DnCustomDlg.h"
#include "DnSmartMoveCursor.h"

class CEtUIEditBox;
class CDnItem;
class CDnAskLookChgDlg : public CDnCustomDlg
{
public:
	CDnAskLookChgDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnAskLookChgDlg(void);

protected:
	CDnItem *m_pItem;

	int		m_nSlotIndex;
	char	m_cSlotType;

	DnEtcHandle m_hCommonEffect;

	// 스마트무브
	CDnSmartMoveCursor m_SmartMove;

protected:
	typedef CDnCustomDlg BaseClass;

public:
	bool SetItem( CDnItem *pItem );

public:
	void InitialUpdate() override;
	void Initialize( bool bShow ) override;
	void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 ) override;
	void Show( bool bShow ) override;
	bool MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) override;
};
