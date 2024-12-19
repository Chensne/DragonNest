#pragma once
#include "EtUIDialog.h"

class CDnGuildDlg;
class CDnGuildIntroduceDlg;
class CDnGuildMemberInfoDlg : public CEtUIDialog
{
public:
	CDnGuildMemberInfoDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnGuildMemberInfoDlg(void);

protected:
	CDnGuildDlg *m_pGuildDlg;
	CDnGuildIntroduceDlg *m_pGuildIntroduceDlg;

	INT64 m_nCharacterDBID;
	void InitControl();

public:
	void SetGuildDlg( CDnGuildDlg *pGuildDlg ) { m_pGuildDlg = pGuildDlg; }
	void SetGuildMemberDBID( INT64 nDBID );

public:
	virtual void InitialUpdate();
	virtual void Initialize(bool bShow);
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
};