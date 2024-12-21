#pragma once
#include "DnOptionDlg.h"
#include "GameOption.h"

class CDnGameProfileOptDlg;
class CDnGameCommOptDlg : public CDnOptionDlg
{
public:
	CDnGameCommOptDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnGameCommOptDlg(void);

protected:
	CEtUIComboBox *m_pComboPartyInviteAcceptable;
	CEtUIComboBox *m_pComboGuildInviteAcceptable;
	CEtUIComboBox *m_pComboTradeRequestAcceptable;
	CEtUIComboBox *m_pComboDuelRequestAcceptable;
	CEtUIComboBox *m_pComboObserveStuffAcceptable;
	CEtUIComboBox *m_pComboLadderInviteAcceptable;
#ifdef PRE_ADD_CHAT_RENEWAL
	CEtUIComboBox * m_pComboPartyChatAcceptable;
#endif

	typedef CDnOptionDlg BaseClass;

	CDnGameProfileOptDlg *m_pGameProfileOptDlg;

public:
	virtual void InitialUpdate();
	virtual void Initialize( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );

public:
	void ImportSetting();
	void ExportSetting();

	virtual bool IsChanged();
};
