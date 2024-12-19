#pragma once
#include "EtUIDialog.h"

class CDnGuildRecruitDlg : public CEtUIDialog
{
public:
	CDnGuildRecruitDlg( UI_DIALOG_TYPE dialogType = UI_TYPE_FOCUS, CEtUIDialog *pParentDialog = NULL, int nID = -1, CEtUICallback *pCallback = NULL );
	virtual ~CDnGuildRecruitDlg(void);

protected:
	CEtUICheckBox *m_pCheckJob[CLASSKINDMAX];
	CEtUICheckBox *m_pCheckLevel;
	CEtUIEditBox *m_pEditBoxMin;
	CEtUIEditBox *m_pEditBoxMax;
	CEtUILineIMEEditBox *m_pLineIMEEditBoxAD;
	CEtUIButton *m_pButtonOK;
	CEtUIButton *m_pButtonCancel;
#ifdef PRE_ADD_GUILD_EASYSYSTEM
	CEtUIComboBox *m_pComboCategory;
#ifdef PRE_MOD_GUILD_EASYSYSTEM_NO_HOMEPAGE
#else
	CEtUICheckBox *m_pCheckHomepage;
#endif
#endif

	bool m_bModify;

	void SetEnableControl();
	void CheckRecruit();

public:
	void OnRecvGuildRecruitRegisterInfo( GuildRecruitSystem::SCGuildRecruitRegisterInfo *pPacket );

public:
	virtual void Initialize( bool bShow );
	virtual void InitialUpdate();
	virtual void Show( bool bShow );
	virtual void ProcessCommand( UINT nCommand, bool bTriggeredByUser, CEtUIControl *pControl, UINT uMsg = 0 );
	virtual void Process( float fElapsedTime );
};