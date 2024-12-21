#pragma once

#include "EtUIDialog.h"

class CDnGuildWarSituationMng;

class CDnMiniPlayerGuildWarMainTabDlg : public CEtUIDialog
{
public:
	CDnMiniPlayerGuildWarMainTabDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL  );
	virtual ~CDnMiniPlayerGuildWarMainTabDlg();

protected:

	enum{
		PARTY_COUNT = 4,
	};

	CEtUIStatic * m_pStaticBar[PARTY_COUNT];
	CEtUIStatic * m_pStaticName;
	CEtUIButton * m_pButtonOK;
	CEtUIButton * m_pButtonCross;
	CEtUICheckBox * m_pCheckParty;

	bool m_bMaster;
	bool m_bCross;

	CDnGuildWarSituationMng * m_pGuildWarSituationMng;

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg );
	virtual void OnChangeResolution();

	void SetMaster( bool bMaster );
	bool IsMaster()	{ return m_bMaster; }
	void SetPartyCount( BYTE cCount);

	void SetGuildWarSituation( CDnGuildWarSituationMng * pGuildWarSituationMng );
	void PartyShowCheck( bool bCheck )	{ m_pCheckParty->SetChecked( bCheck ); }
};
